# FreeWPC: a framework/OS for WPC pinball machines of the 90s

Note: FreeWPC is no longer being maintained!

This framework was actively developed between 2005-2013 as a side
project to experiment with and learn about coding for actual
pinball machines.  In 2012, I joined the startup Heighway Pinball
to do software for their new games and used FreeWPC as a starting
point.  Later, they were acquired by Pinball Brothers who continues
to make games as of 2026.

Because of this, my time shifted by hobby project to part-time
commercial development and this project became abandoned.

I'm keeping this repository online for those who are curious how
pinball games work, but no further commits are being made to it.
Also, the framework has evolved significantly and this does not
accurately reflect how today's generation of games are made.


## Original README:

This is FreeWPC, an open source operating system for the Williams
Pinball Controller (WPC) family of pinball machines.  FreeWPC is
licensed under the GNU General Public License, Version 2.

The sources for the core routines are located in 'kernel';
other common code is in 'common', and game-specific code can be found 
in a subdirectory based on the machine name, under 'machine'.
There are also a number of tools included with the distribution
for generating some of the source code and for building ROM images
that are under 'tools'.

You will also need several tools not here in order to build.
See the file doc/build.html for more detailed information.  Most
importantly, you'll need a copy of the gcc6809 cross-compiler.
Many standard UNIX utilities are needed as well.  FreeWPC has been
built successfully on Linux and Cygwin.

Email suggestions/comments/questions to <brian@oddchange.com>.

Visit the homepage at http://www.oddchange.com/freewpc for more
information.

