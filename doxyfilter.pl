#!/usr/bin/perl
#
# doxyfilter.pl - filter source files before presenting to doxygen
#

open FH, $ARGV[0];
while (<FH>)
{
	chomp;
	s/CALLSET_ENTRY[ \t]*\((.*), (.*)\)/\/** Event catcher for the $2 event *\/\nvoid ${1}_${2} (void)/;
	s/CALLSET_BOOL_ENTRY[ \t]*\((.*), (.*)\)/\/** Event catcher for the $2 event *\/\nbool ${1}_${2} (void)/;
	print "$_\n";
}
close FH;
