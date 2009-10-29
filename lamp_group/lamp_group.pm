#!/usr/bin/perl -w

package lamp_group;

# This module implements an algebraic parser for lamp groups.
# A lamp group can be a "lamp list" (ordered) or "lamp set"
# (unordered).  Lamps/lamp groups can be added/removed.
# The default is to keep a group ordered, unless explicitly
# requested.  Operations result in temporaries unless explicitly
# exported to a named ID.  Lists can also be sorted by some
# criteria, and breaks may be introduced too.


# A hash that stores properties of individual lamps.
my %lamp_props;


sub define {
	my ($lampid, $property, $value) = @_;

	if (!defined $lamp_props{$lampid}) {
		$lamp_props{$lampid} = {};
	}
	my $hash = $lamp_props{$lampid};
	$hash->{$property} = $value;
}

sub lookup {
	my ($lampname) = @_;
	if ($lampname =~ /^[A-Z]/) {
		return 0;
	}
	return $lampname;
}

sub mdread {
	my ($mdfile) = @_;
	open $fh, $mdfile or die "Cannot open $mdfile: $!";
	my $section = "";
	while (<$fh>) {
		chomp;
		if (/^\[(.*)\]$/) {
			$section = $1;
		}
		if ($section eq "lamps") {
			my ($id, $definition) = split /:/, $_;
			next if (!defined $id);
			if ($id =~ /([1-8])([1-8])/) {
				# Transform the ID.
				$id = ($1 - 1) * 8 + ($2 - 1);
				my ($name, @props) = split /,/, $definition;
				$name =~ s/^[ ]*//;
				$name =~ s/[ .][ .]*/_/g;
				$name =~ tr/a-z/A-Z/;
				$name = "LM_" . $name;
				define ($id, "name", $name);
				for $prop (@props) {
					if ($prop =~ /(.*)\((.*)\)/) {
						define ($id, $1, $2);
					}
					else {
						define ($id, "color", $prop);
					}
				}
			}
		}
	}
}

# A lamp group is implemented internally as a list of integers.
# The integer is the key for the lamp props.
# Conversions to bitmap for unordered groups is done at output time.

sub new {
	my $self = {};
	$self->{ord} = 1;     # ordered by default
	$self->{list} = [];	# create empty list for ordered
	$self->{hash} = {};   # create empty hash for unordered
	bless $self;
	return $self;
}

sub set_ordered {
	my ($self, $ordered) = @_;
	$self->{ord} = $ordered;
}

sub output {
	my ($self) = @_;
	print "{ ";
	if ($self->{ord}) {
		my $list = $self->{list};
		for $lamp (@$list) {
			my $props = $lamp_props{$lamp};
			my $name = $props->{'name'};
			print "$name, ";
		}
	}
	else {
	}
	print " }\n";
}

sub add {
	my ($self, $lamp) = @_;
	my $list = $self->{list};
	my $hash = $self->{hash};

	$lamp = lookup ($lamp);
	return if (defined $hash->{$lamp});

	push @$list, $lamp;
	$hash->{$lamp} = 1;
}

sub remove {
	my ($self, $lamp) = @_;
	my $oldlist = $self->{list};
	my $hash = $self->{hash};

	return if (!defined $hash->{$lamp});

	$lamp = lookup ($lamp);
	my $newlist = [];
	for $oldlamp (@$oldlist) {
		if ($oldlamp != $lamp) {
			push @$newlist, $oldlamp;
		}
	}
	$self->{list} = $newlist;
}


sub copy {
	my ($self) = @_;

	my $clone = lamp_group->new ();
	$clone->set_ordered ($self->{ord});

	my $list = $self->{list};
	for my $lamp (@$list) {
		$clone->add ($lamp);
	}
	return $clone;
}

sub find {
	my ($property, $value) = @_;
	my $matches = lamp_group->new ();
	for $lampid (keys %lamp_props) {
		my $property_hash = $lamp_props{$lampid};
		my $actual = $property_hash->{$property};
		if (defined $actual && ($actual =~ /$value/)) {
			$matches->add ($lampid);
		}
	}
	return $matches;
}

sub add_group {
	my ($self, $other) = @_;
	my $otherlamps = $other->{list};
	for my $lamp (@$otherlamps) {
		$self->add ($lamp);
	}
}


sub remove_group {
	my ($self, $other) = @_;
	my $otherlamps = $other->{list};
	for my $lamp (@$otherlamps) {
		$self->remove ($lamp);
	}
}


sub intersection {
	my ($self, $other) = @_;
	my $otherlamps = $other->{list};
}


sub sort {
	my ($self, $sortfn) = @_;
}

sub break {
	my ($self, $value, $breakfn) = @_;
}

1;
