#!/usr/bin/perl

$id = <STDIN>;
$comment = <STDIN>;
$dims = <STDIN>;
$max = <STDIN>;

# removed "invert" for now
@ops = (@ARGV);

print $id;
print $dims;
print $max;

while (<STDIN>) {
	chomp;
	my $val = $_;

	foreach $op (@ops) {
		if ($op eq "invert") {
			$val = 255 - $val;
		}
		elsif ($op eq "n2") {
			if ($val < 43) { $val = 0; }
			elsif ($val < 85 + 43) { $val = 85; }
			elsif ($val < 170 + 43) { $val = 170; }
			else { $val = 255; }
		}
		elsif ($op eq "p1") {
			if ($val < 43) { $val = 0; }
			elsif ($val < 85 + 43) { $val = 255; }
			elsif ($val < 170 + 43) { $val = 0; }
			else { $val = 255; }
		}
		elsif ($op eq "p2") {
			if ($val < 43) { $val = 0; }
			elsif ($val < 85 + 43) { $val = 0; }
			elsif ($val < 170 + 43) { $val = 255; }
			else { $val = 255; }
		}
		else {
			die "Unknown operation $op\n";
		}
	}

	# Output
	print "$val\n";
}
