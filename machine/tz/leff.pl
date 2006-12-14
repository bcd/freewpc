#!/usr/bin/perl

@EXPORT = qw (build_lampset_from_lock);

print "/* Included $1 */\n\n";

sub build_lampset_explode_from_lamp {
	my ($lamp) = @_;
	die "no such lamp" if (!defined $lamp);
	my ($C_x, $C_y) = ($lamp->{'x'}, $lamp->{'y'});
	my ($a_x, $a_y) = ($a->{'x'}, $a->{'y'});
	my ($b_x, $b_y) = ($b->{'x'}, $b->{'y'});

   return int sqrt (((($a_x - $C_x) ** 2) + (($a_y - $C_y) ** 2)))
   	<=> int sqrt (((($b_x - $C_x) ** 2) + (($b_y - $C_y) ** 2)));
}

sub build_lampset_from_lock {
	my $lamps = $m->{'lamps'};
	my $lamp = $lamps->{'Lock Arrow'};
	return build_lampset_explode_from_lamp ($lamp);
}

1;
