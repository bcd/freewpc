#!/usr/bin/perl -w

use lamp_group;

lamp_group::mdread ("../machine/wcs/wcs.md");

my $lg = lamp_group->new ();
$lg->add (1);
$lg->add (3);
$lg->add (5);
$lg->set_ordered (0);
$lg->output ();

my $lg2 = lamp_group->new ();
$lg2->add (10);
$lg2->add (5);
$lg2->output ();

$lg->remove_group ($lg2);
$lg->output ();

my $lg3 = $lg2->copy ();
$lg3->output ();

my $lg4 = lamp_group::find ('color', 'white');
$lg4->output ();
