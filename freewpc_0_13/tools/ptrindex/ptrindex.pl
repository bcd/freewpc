#!/usr/bin/perl
#
# Name: ptrindex 
#
# Description: a Perl script to create an "index" of pointers to a fixed
# set of objects.
#
# Author: Brian Dominy (brian@oddchange.com)
#
# Algorithm:
#
# 1. Scan through all source files (specified by users) looking for
# *declarations of indexed objects*.  These are indicated by putting
# the macro XXXX_INDEXED_OBJECT() around the name in the declaration.
#
# 2. Strive to keep the order of the elements in the list the same
# from run to run.  New declarations should go to the end; old
# declarations should keep the same slots; deleted declarations can
# lead to holes (possibly to be filled with new declarations).
# The output of this step is an index file xxxx.idx.  (The previous
# version of this file is always kept as xxxx.idx.last.)
#
# 3. Generate some C macros in xxxx.h for various functions:
#
# XXXX_INDEXED_OBJECT(x) is the same as x
#
# XXXX_INDEX(ptr) returns the small integer index representing ptr
#
# XXXX_INDEX_TABLE defines a table of pointers to elements.
# So XXXX_INDEX_TABLE(XXXX_INDEX(ptr)) == ptr.
#
# Plus it defines all of the externs needed.
#
# 4. Generate a makefile extension file, xxxx.make, that defines what
# C files need to be rebuilt because indexes they use have changed.
# This prevents everything from needing to be recompiled after making
# a trivial change.
#
# Notes:
#
# If XXXX_INDEX() is found with a ptr that has not been defined, that is
# an error.

my $module_name = undef;
my $h_file = undef;
my $idx_file = undef;
my $last_idx_file = undef;
my @c_files = ();
my %found = ();

my %index = ();   		# Key is reference, value is its index
my %index_used = ();    	# Key is index, value is 1 if used
my %c_needs_recompile = ();	# Key is c_file entry, value if 1 if needs recompile

#
# Seed index for testing (should be done during old idx file read)
#
#%index = (
#	'attract_deff' => 0,
#	'credits_deff' => 1,
#	'scores_deff' => 2,
#	'tilt_deff' => 3,
#);
#
#%index_used = (
#	0 => 1,
#	1 => 1,
#	2 => 1,
#	3 => 1,
#);
#


sub set_module
{
	($module_name) = @_;
	print "Processing module '$module_name'\n";
	$h_file = "${module_name}_sset.h";
	$idx_file = "$module_name.sset";
	$last_idx_file = "$idx_file.sset.last";
	$ref_macro = "\U${module_name}_INDEX";
	$decl_macro = "\U${module_name}_INDEXED_OBJECT";
	$table_macro = "\U${module_name}_INDEX_TABLE";
	$object_type = "${module_name}_info";
	$object_table_name = "${module_name}_table";
	$include_guard = "\U${module_name}_INDEX_INCLUDE_GUARD";
}


sub add_source_directory
{
	my ($dir) = @_;
	opendir DIR, $dir or die "Couldn't open directory '$dir' for reading.\n";
	print "Reading files from $dir...\n";
	while ($file = readdir DIR) {
		if ($file =~ /\.c$/) {
			print "Using $file\n";
			push @c_files, $file;
		}
	}
	closedir DIR;
}

sub free_index
{
	my $index = 0;
	for (;;) {
		return $index if not exists ($index_used{$index});
		$index++;
	}	
}


sub define_macro
{
	my ($object) = @_;
	return "\U${module_name}_INDEX_" . $object;
}


sub dump_index
{
	open FH, ">$h_file" or die "Can't open $h_file\n";

	print FH "#ifndef __${include_guard}\n";
	print FH "#define __${include_guard}\n\n";

	###########################################################
	# Print the index defines
	###########################################################
	print FH "\n/* Index list */\n";
	for $entry (sort { $index{$a} <=> $index{$b} } (keys (%index))) {
		print FH "#define ", define_macro ($entry), "\t", $index{$entry}, "\n";
	}

	###########################################################
	# Print externs to all objects
	###########################################################
	print FH <<END;

/* Externs for all objects */
END
	for $entry (sort { $index{$a} <=> $index{$b} } (keys (%index))) {
		print FH "extern $object_type $entry;\n";
	}

	###########################################################
	# Print the table of objects macro
	###########################################################
	print FH <<END;

/* Table generation macro
 * Invoke this macro in the C file where you want the table of
 * pointers to objects to be defined.
 */

END
	print FH "#define $table_macro \\\n";
	print FH "$object_type *$object_table_name\[\] = \{ \\\n";
	for $entry (sort { $index{$a} <=> $index{$b} } (keys (%index))) {
		print FH "   \&$entry, \\\n";
	}
	print FH "\}\n\n";

	print FH <<END;
/*
 * Table lookup macro
 * Define the macro used by callers to translate an object's name into
 * an index into the table where its pointer is stored.
 */

END

	###########################################################
	# Print the referencing macro
	###########################################################
	print FH "#define $ref_macro\(obj\)\t",
		"\U${module_name}_INDEX_\E ## obj\n\n";

	print FH "/* Next free index is ", free_index (), "*/\n";
	print FH "\n\n#endif /* __${include_guard} */\n";
}


#
# Process command-line arguments
#
while ($arg = shift)
{
	if ($arg =~ m/^-m$/) { set_module (shift); next; }
	if ($arg =~ m/^-c$/) { push @c_files, shift; next; }
	if ($arg =~ m/^-C$/) { add_source_directory (shift); next; }
	if ($arg =~ m/^-o$/) { $h_file = shift; next; }
}

die "No module specified (use -m)\n" unless defined $module_name;

#
# Read in the reference list from the previous run.
# Construct a hash for the old data, where the reference names
# are the keys and the line number is the value.
#
open FH, $idx_file;
$lineno = 0;
while (<FH>) {
	$index{$_} = $lineno++;
	$index_used{$lineno} = 1;
}
close FH;


#
# Query the current set of objects that are referenced for this class.
#
print "Reading C files.\n";
for $c_file (@c_files) {
	open FH, $c_file;
	print "Scanning $c_file...\n";
	while (<FH>) {
		my $line = $_;
		if ($line =~ m/($ref_macro)\((\w+)\)/) {
			print "Found reference to $2 in $c_file\n";
			$found{$2} = 1;
		}
	}
	close FH;
}


#
# Go through the old list and remove any references that are no longer
# present (optional).
#
for $oldref (keys %index) {
	if (!exists ($found{$oldref})) {
		# oldref is no longer present in the code, so its
		# slot can be cleared.
		print "$oldref is no longer present in the code.\n";
		my $idx = $index{$oldref};
		delete $index{$oldref};
		delete $index_used{$idx};
	}
}


#
# Add any new reference to the end of the oldlist.  oldreflist then
# contains the 'current' list in proper sort order.
#
for $newref (keys %found) {
	if (!exists ($index{$newref})) {
		# newref is present in the current code but was not
		# there before.
		my $idx = free_index ();
		$index{$newref} = $idx;
		$index_used{$idx} = 1;
	}
}


#
# Rename the idx file to the last idx filename.
# Save the values in oldreflist to the latest idx filename.
#


#
# Regenerate the .h file
#
print "Writing to $h_file.\n";
dump_index $h_file;


#
# Regenerate the .make file
#

