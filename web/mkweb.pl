#!/usr/bin/perl

sub finish {
	$first_output = "<p align=\"right\">";
	my $sep = "";
	foreach $first (@firsts) {
		$first_output .= "$sep<a href=\"\">$first</a>";
		$sep = " | ";
	}
	$first_output .= "</p>";
	$output =~ s/\@firsts/$first_output/g;
	$output =~ s/\@title/$var{'title'}/;

	print $output;
}

sub addSection {
	my ($sec, $name) = @_;
}

sub output {
	$output .= shift;
}

sub weblink {
	my ($line, $term, $url) = @_;
	if (($line =~ /[ \t]${term}[ .,\t]/) && (!defined $webterms{$term})) {
		$webterms{$term} = 1;
		$line =~ s/([ \t])${term}([ .,\t])/$1<a href="http:\/\/$url">$term<\/a>$2/g;
	}
	return $line;
}

sub htmlify {
	$_ = shift;
	s/<\^(.*)\^>/<a href="mailto:$1">$1<\/a>/g;
	s/<<([^,>]*)>>/<a href="$1">$1<\/a>/g;
	s/&&([a-zA-Z0-9_()-]+)/<span class="C">$1<\/span>/g;
	if (/exec(.*)cexe/) {
		my $res = `$1`;
		s/exec(.*)cexe/$res/;
	}
	$_ = weblink ($_, "newlib", "sources.redhat.com/newlib");
	$_ = weblink ($_, "GCC", "gcc.gnu.org");
	$_ = weblink ($_, "Subversion", "subversion.tigris.org");
	$_ = weblink ($_, "WebSVN", "websvn.tigris.org");
	$_ = weblink ($_, "FreeWPC", "www.oddchange.com/freewpc");
	$_ = weblink ($_, "Bugzilla", "www.bugzilla.org");
	$_ = weblink ($_, "autoconf", "www.gnu.org/software/autoconf");
	$_ = weblink ($_, "Cygwin", "www.cygwin.com");
	return $_;
} 


output <<END;
<html>
<head>
<title>\@title</title>
<style type="text/css">
body { background:white; font-family: arial; margin-top: 50px; margin-bottom: 50px; margin-left: 5%; margin-right: 10%; }
a { text-decoration: none; color: #22e; }
a:hover { text-decoration: underline; }
h1 { font-size: 18px; margin-top: 40px; margin-bottom: 1px; }
h2 { font-size: 16px; }
hr { margin: 4px; }
p, li { font-size: 14px; margin-right: 5%; }
pre { background: #ea9; color: black; margin: 20px; padding: 10px; }
.C { color: #f34; }
.T { font-size: 20px; font-weight: bold; margin-bottom: 15px; }
.S { background: #6f9; margin: 10px; margin-bottom: 25px; padding: 10px; border-width: 1px; border-color: black; }
.SN { font-weight: bold; }
</style>
</head>
<body>
END


while (<>) {
	chomp;
	next if (/^#/);
	if (/^\@deftitle (.*)/) {
		$var{'title'} = $1;
		output ("<span class=\"T\">$1</span>");
		next;
	}
	elsif (/^\@/) {
	}
	elsif (/^==/) {
		s/^==/<h2>/;
		addSection (2, $_);
		s/$/<\/h2>/;
	}
	elsif (/^=/) {
		s/^=//;
		addSection (1, $_);
		push @firsts, $_ if ($firstfirst++ != 0);
		$_ = "<h1>$_</h1><hr />";
	}
	elsif (/^!/) {
		output "<div class=\"S\">\n";
		s/^!/<span class="SN">/;
		s/$/<\/span>/;
		$inDiv = 1;
	}
	elsif (/^\* /) {
		output "<ul>\n" if (!$inList);
		output "</li>\n" if ($inList);
		s/^\* /<li>/;
		$inList = 1;
	}
	elsif (/^\&/) {
		s/^\&//;
		output "<pre>" if (!$inPre);
		$inPre = 1;
	}
	elsif (/^$/) {
		if ($inList) {
			output "</li></ul>\n";
			$inList = 0;
		}
		elsif ($inPara) {
			output "</p>\n";
			$inList = $inPara = 0;
		}
		elsif ($inPre) {
			output "</pre>\n";
			$inList = $inPara = $inPre = 0;
		}
		elsif ($inDiv) {
			output "</div>\n";
			$inList = $inPara = $inPre = $inDiv = 0;
		}
		next;
	}
	elsif (!$inList) {
		output "<p>" if (!$inPara);
		$inPara = 1;
	}
	output (htmlify ($_) . "\n");
}

finish;
print <<END;
</body>
</html>
END
