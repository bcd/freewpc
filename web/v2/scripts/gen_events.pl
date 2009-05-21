#!/usr/bin/perl

open FH, "doc/events.txt" or die;
print "<table border=1 cellpadding=8>\n";
while (<FH>) {
	chomp;
	my $boolType = 0;
	my $module = undef;
	my $event = undef;
	my $comment = "";

	my (@words) = split /[ \t]+/, $_;
	for $word (@words) {
		if ($word eq "boolean") {
			$boolType = 1;
		}
		elsif (defined $event) {
			$module = $event;
		}
		$event = $word;
	}

	while (1) {
		my $line = <FH>;
		chomp $line;
		last if ($line =~ /^$/);
		$comment .= $line . " ";
	}

	next if (defined $module);

	print "<tr>\n";
	print "<td>$event</td>\n";
	print "<td>$comment</td>\n";
	print "</tr>\n";

}
print "</table>\n";
close FH;
