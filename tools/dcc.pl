#!/usr/bin/perl -w
#
# Copyright 2011 by Brian Dominy <brian@oddchange.com>
#
# This file is part of FreeWPC.
#
# FreeWPC is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# FreeWPC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with FreeWPC; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
use POSIX qw(ceil);

# The attribute hash contains all of the deff properties.
# There are two types of attributes here: per-element and global.
# They are listed in that order.
#
# Any attribute can be set by using a command of the same name in
# the source file.  The exact syntax of the arguments varies though.
#
# Attributes are kept in a stack, which is pushed whenever a new scope
# is started, so that exiting the scope will restore all modified attributes
# back to their original values.
#
# The values listed here are the global defaults; you need not specify any
# of these if the default suffices.
my $attr = {
	parent => undef,

	# The area that the current element occupies
	x => 0, y => 0,
	width=> 128, height => 32,

	# The attributes for the next "print" element.
	color => 3,
	bgcolor => 0,
	font => "mono5",
	blink => "none", # options: none, or a time rate
	align => "center", # options: left, center, right
	valign => "center", # options: top, center, bottom
	background => "none", # options: none, shadow, box

	name => "unnamed",
	rate => "TIME_50MS",
	transition => "none",
	flash => "none",
};


# A list of font heights, taken from the .fon files
my %fontheight = (
	"cu17" => 22,
	"var5" => 5,
	"mono5" => 5,
	"fixed6" => 9,
	"fixed10" => 12,
	"fixed12" => 18,
	"lucida9" => 13,
	"luctype" => 10,
	"miscfixed" => 10,
	"schu" => 10,
	"term6" => 8,
	"times10" => 12,
	"times8" => 12,
	"utopia" => 10,
);


# An array of the effect's elements.  A display effect is composed
# of multiple items: text labels, images, borders, etc.  Each is
# represented by an entry here.
#
# Each element is a copy of the attribute hash at the time it
# was declared, plus additional data specific to each element type,
# and a field named "type" to identify the type of element.
my @elements;


# Effects may take arguments; these are external data variables that
# they need to see, for conditionals or for printing changing values.
my @arguments;


# For diagnostic purposes, track the file/line number of the
# source file.
$lineno = 0;
$file = $ARGV[0] || "stdin";

sub warning {
	my ($message, $no) = @_;
	$no = $lineno if (!defined $no);
	print STDERR "$file:$no: warning: $message\n";
}

sub error {
	my ($message, $no) = @_;
	$no = $lineno if (!defined $no);
	print STDERR "$file:$no: error: $message\n";
	die;
}

sub internal_error {
	print STDERR "internal error: " . (shift) . "\n";
	die;
}


# For debug purposes to display the internal data structures.

sub debug_attr {
	my $ref = $attr;
	while ($ref) {
		print ">> ";
		for my $field (sort keys %$ref) {
			next if ($field eq "parent");
			print $field, ":", $ref->{$field}, " ";
		}
		print "\n";
		$ref = $ref->{"parent"};
	}
}

sub debug_elem {
	for my $elem (@elements) {
		print "(";
		for my $field (sort keys %$elem) {
			print $field, ":", $elem->{$field}, " ";
		}
		print ")\n";
	}
}

# Push/pop scopes.

sub push_scope {
	my %new_attr = %$attr;
	my $new_ref = \%new_attr;
	$new_ref->{"parent"} = $attr;
	$attr = $new_ref;
}

sub pop_scope {
	$attr = $attr->{"parent"};
	if (!defined $attr) {
		error "illegal use of end; not in a scope?";
	}
}

# Emit code to display a single element.

sub emit_element {
	my $elem = shift;
	my $type = $elem->{"type"};
	error "unknown type" if (!defined $type);
	if ($type eq "print") {
		my $align = $elem->{"align"};
		my $valign = $elem->{"valign"};
		my $y = $elem->{"y"};
		my $text = $elem->{"text"};
		my $argv = $elem->{"argv"};
		my $font = $elem->{"font"};

		if ($align eq "center") { $x = ceil ($elem->{"x"} + $elem->{"width"} / 2); }
		elsif ($align eq "right") { $x = $elem->{"width"} - 1; }
		else { $x = $elem->{"x"}; }

		if ($valign eq "center") { $y = ceil ($elem->{"y"} + $elem->{"height"} / 2); }
		elsif ($valign eq "bottom") { $y = int ($elem->{"y"} + $elem->{"height"} - $fontheight{$font} / 2); }
		else { $y = ceil ($elem->{"y"} + $fontheight{$font} / 2); }

		if ($text eq "current_score") {
			pr ("sprintf_current_score ();");
			$text = "sprintf_buffer";
		}
		elsif ($elem->{"argc"} > 1) {
			my @args = @$argv;
			pr ("sprintf ($text, $args[1]);");
			$text = "sprintf_buffer";
		}
		pr ("font_render_string_$align (&font_$font, $x, $y, $text);");
	}
	elsif ($type eq "image") {
		pr ("frame_draw (" . $elem->{"id"} . ");");
	}
	elsif ($type eq "overlay") {
		pr ("dmd_map_overlay ();");
		pr ("dmd_dup_mapped ();");
	}
	elsif ($type eq "border") {
		if ($elem->{"border_width"} == 1) {
			pr ("dmd_draw_thin_border (dmd_low_buffer);");
		}
		elsif ($elem->{"border_width"} == 2) {
			pr ("dmd_draw_border (dmd_low_buffer);");
		}
	}
	else {
		internal_error "unknown type '$type' encountered";
	}
}

# Emit code for the entire display effect.

sub emit_deff {
	local $indent = 0;
	sub pr { for (my $i=0; $i < $indent; $i++) { print " "; } print (shift); print "\n"; }
	sub block { pr ("{"); $indent += 3; }
	sub endblock { $indent -= 3; pr ("}"); }

	if (!$header_emitted) {
		pr "#include <freewpc.h>\n";
		$header_emitted = 1;
	}

	# Sort the final list of elements.
	@elements = sort { $a->{"sort_order"} <=> $b->{"sort_order"} } @elements;

	# Count the number of distinct colors used.
	# From this, determine the number of planes that need to be generated.
	# If an image is being drawn, we always use 2 planes.
	my $planes = 0;
	my $clean = 1;
	my $last_color = -1;
	for my $elem (@elements) {
		if ($elem->{"color"} == 255) {
			$planes = 2;
			$clean = 0;
		}
		elsif ($elem->{"color"} != $last_color) {
			$planes++;
			$last_color = $elem->{"color"};
		}
	}

	# Count the number of static and non-static elements, and the
	# number of blinking elements.
	my $statics = 0;
	my $dynamics = 0;
	my $blinks = 0;
	for my $elem (@elements) {
		if ($elem->{"sort_order"} >= 200) {
			$dynamics++;
		}
		else {
			$statics++;
		}
		if ($elem->{"blink"} ne "none") {
			$blinks++;
			# verify that blink rate is multiple of total effect rate
		}
	}

	# Determine the functions to be used for alloc and show.
	# If one page is needed, use dmd_alloc_low/dmd_show_low.
	# If two are needed, use dmd_alloc_low_high/dmd_show2.
	# If the pages need to be cleaned after allocation (typical,
	# unless a full-size image is being shown), then do that too.
	my $allocator;
	my $shower;

	if ($planes == 1) {
		$allocator = "dmd_alloc_low";
		$allocator .= "_clean" if ($clean == 1);
		$shower = "dmd_show_low";
	}
	else {
		$allocator = "dmd_alloc_low_high";
		$allocator .= "(); dmd_clean_page_low" if ($clean == 1);
		$shower = "dmd_show2";
	}

	my $name = $attr->{"name"} . "_deff";
	pr ("void $name (void)");
	block ();

	# If there is a mixture of static/nonstatic elements, then
	# factor the static parts out of the loop onto the overlay pages.
	# Replace the element with a call to display the overlay.
	if ($dynamics) {
		pr ("/* TODO : factor out static elements to overlay */");
	}

	# Emit code for each element.
	#
	# Ensure that an image is always displayed first, since that will overwrite
	# everything else.
	#
	# After displaying all elements, emit code to show the final composite
	# and wait the right amount of time before looping back and showing it
	# again, or exiting.
	pr ("for (;;)");
	block ();
	pr ("$allocator ();");
	for my $elem (@elements) {
		# TODOs:
		# If the element has the 'blink' attribute, then it should only be drawn
		# on some iterations.  Use the blink rate and the overall run rate to
		# determine when to draw it.  When we don't draw it, treat like a false
		# conditional.  We'll also need a runtime counter to track state.
		#
		# 'blink' on a static label that was moved to overlay requires code
		# in the loop on the erase cycle.
		#
		# If there are multiple colors in use, then render color=3 first,
		# copy low to high, then render color=2 (flip low/high first),
		# then color=1.  But if only color=1 is used, then it is simpler.
		if ($cond = $elem->{"cond"}) {
			pr ("if " . $cond);
			block ();
		}
		emit_element ($elem);
		if ($cond) {
			endblock ();
		}
	}
	if (($trans = $attr->{"transition"}) ne "none") {
		pr ("dmd_sched_transition (&$trans);");
	}
	pr ("$shower ();");

	$run = $attr->{"run"};
	if (($flash = $attr->{"flash"}) ne "none") {
		if ($planes > 1) {
			error "flash unsupported on 2-plane images";
		}
		pr ("dmd_copy_low_to_high ();");
		pr ("dmd_invert_page (dmd_low_buffer);");
		pr ("deff_swap_low_high ($run / $flash, $flash);");
		pr ("break;");
	}
	elsif ($run ne "forever") {
		pr ("task_sleep ($run);");
		pr ("break;");
	}
	else {
		my $rate = $attr->{"rate"};
		pr ("task_sleep ($rate);");
	}
	endblock ();

	# emit exit code
	pr ("deff_exit ();");
	endblock ();
	pr ("");
}

# Allocate a new element object.

sub new_elem {
	my %elem = %$attr;
	my $elemref = \%elem;
	delete $elemref->{"parent"};
	$elemref->{"type"} = shift;
	$elemref->{"lineno"} = $lineno;
	return $elemref;
}

# Parse one line from the source file.

sub cmd {
	my $line = $_;
	$line =~ /(^\w+)[ \t]*(.*)$/;
	my ($opcode, $argstring) = ($1, $2);
	my @args = split /,[ \t]+/, $argstring;

	if ($opcode =~ /vbox$|hbox$/) {
		my $size_field = ($opcode eq "vbox") ? "height" : "width";
		my $pos_field = ($opcode eq "vbox") ? "y" : "x";

		if (!defined $attr->{"parent"}) {
			push_scope ();
		}
		else {
			my $pos = $attr->{$pos_field} + $attr->{$size_field};
			pop_scope ();
			push_scope ();
			$attr->{$pos_field} = $pos;
		}

		if ($args[0] =~ /([0-9]+)\%$/) {
			my $percentage = $1;
			$attr->{$size_field} = $attr->{"parent"}->{$size_field} * $percentage / 100;
		}
		else {
			$attr->{$size_field} = $args[0];
		}
	}
	elsif ($opcode eq "end") {
		pop_scope ();
	}
	elsif ($opcode eq "print") {
		my $elemref = new_elem "print";
		$elemref->{"text"} = $args[0];
		$elemref->{"argc"} = @args;
		$elemref->{"argv"} = \@args;
		if ($elemref->{"text"} eq "current_score") {
			$elemref->{"argc"}++;
		}
		$elemref->{"sort_order"} = 100 * $elemref->{"argc"} + 4 - $elemref->{"color"};
		push @elements, $elemref;
	}
	elsif ($opcode eq "image") {
		my $elemref = new_elem "image";
		$elemref->{"color"} = 255;
		$elemref->{"id"} = $args[0];
		$elemref->{"sort_order"} = -100;
		push @elements, $elemref;
	}
	elsif ($opcode eq "border") {
		my $elemref = new_elem "border";
		$elemref->{"border_width"} = $args[0] || 2;
		$elemref->{"sort_order"} = -50;
		push @elements, $elemref;
	}
	elsif ($opcode =~ /U8$|U16$/) {
		my $argref = {};
		$argref->{"type"} = $opcode;
		$argref->{"name"} = $args[0];
		push @arguments, $argref;
	}
	elsif ($opcode eq "exit") {
		emit_deff ();
		@elements = ();
		@arguments = ();
	}
	else {
		if (!defined ($args[0])) {
			error ("syntax error");
		}
		$attr->{$opcode} = $args[0];
	}
}

# Main loop.  Parse the entire source file.
while (<>) {
	$lineno++;
	chomp;
	s/^[ \t]//; s/[ \t]$//;
	next if /^#/; next if /^$/;
	cmd ($_);
}

#debug_attr ();
#debug_elem ();
