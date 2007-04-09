#!/usr/bin/perl

@EXPORT = qw (build_lampset_from_lock);

print "/* Included $1 */\n\n";

#
# build_lampset_explode_from_lamp
#
# This is a generic lampset generator that orders all playfield
# lamps based on distance to a given lamp.  When the lamps are
# rapidly turned on/off, it appears as if the lamps are 'exploding'
# outwards from the starting location.
#
sub build_lampset_explode_from_lamp {
	my ($lamp) = @_;
	die "no such lamp" if (!defined $lamp);
	my ($C_x, $C_y) = ($lamp->{'x'}, $lamp->{'y'});
	my ($a_x, $a_y) = ($a->{'x'}, $a->{'y'});
	my ($b_x, $b_y) = ($b->{'x'}, $b->{'y'});

   return int sqrt (((($a_x - $C_x) ** 2) + (($a_y - $C_y) ** 2)))
   	<=> int sqrt (((($b_x - $C_x) ** 2) + (($b_y - $C_y) ** 2)));
}

#
# build_lampset_from_lock - an instance of the above which originates
# from TZ's green lock arrow.
#
sub build_lampset_from_lock {
	my $lamps = $m->{'lamps'};
	my $lamp = $lamps->{'Lock Arrow'};
	return build_lampset_explode_from_lamp ($lamp);
}


1;
