# FreeWPC master internals documentation
# (C) Copyright 2006 by Brian Dominy.
# This file is processed by the 'guide' perl script to generate
# the final documentation.
{Overview
	FreeWPC is a free platform for developing replacement game ROMs for
	Bally/Williams pinball machines based on the WPC 
	(Williams Pinball Controller)
	chip.  FreeWPC provides the foundation for building new ROMs with 
	custom game rules.

	WPC is the system that was used in all games from 1990-1999,
	starting with ///Funhouse/// and ending with ///Cactus Canyon///.

	FreeWPC only replaces your game ROM (the U6 chip), which contains all
	of the game code and dot matrix text/graphics.  In particular, sounds and
	music are NOT included in this part, so FreeWPC games use the
	same sounds as the real machines.

	Eventually, code should be able to run on an actual pinball machine; 
	however, for now, FreeWPC ROMs should only be used in emulators such as 
	pinmame. '''Do not attempt to put a FreeWPC ROM in a real machine!''' 
	It likely won't work and you may cause irreparable damage to your game, 
	yourself, your house, etc.  You have been warned.

	This document provides an overview of the FreeWPC architecture for
	developers who wish to understand how the system works.

	The details provided here reflect the status of the ///latest development///
	branch.  FreeWPC is constantly evolving and the information here is
	subject to change.

	If you are interested in working on this project, please contact me
	at <a href="mailto:freewpc@oddchange.com">freewpc@oddchange.com</a>.  
	Developers should be familiar
	with basic embedded systems programming concepts and the C programming
	language.  Familiarity with 6809 assembler is ///highly/// recommended.
	Also, knowledge of WPC pinball machines is expected, but I'm assuming
	anyone interested would probably meet that requirement.

	{Features
		<It is 100% Williams-free code, written from scratch, released under the GNU General Public License.
		<Code is written mostly in C, thanks to the availability of a halfway decent <a href="/gcc6809">C compiler for the 6809 CPU</a>.  In some places, assembly language code is used for performance or to do some really low-level things.  Using C over assembler should allow more people to get involved with the project.
		<Most development can be done under Linux or under Windows using the Cygwin UNIX emulation suite.  Some tasks require installation of other programs, for example, if you want to make your own fonts.
		<The system is designed to be flexible and extensive.  All generations of WPC will be supported, except for alphanumeric games.  Multiple machines can be targeted using the same common core "system" code.
		>
	}
	{Preview Releases
		You will need to copy the ROM file into your PinMAME <b>roms</b>
		directory so that PinMAME can find it.  The location of this directory
		depends on how you installed PinMAME.  These directories typically contain
		ZIP files of the game ROM and sound ROMs; you'll need to unzip the file,
		replace the game ROM, then zip it back.  Make sure to save your original
		version of the ROM file for when you want to revert back to the real deal.
	}
	{Source Code Repository
		The source code is maintained in a Subversion repository.  The URL
		for the repository is %url(svn/public/freewpc).  Beneath the
		root directory, there is a '''tags''' subdirectory which contains
		stable snapshots of the code and a '''trunk''' subdirectory that
		houses the ongoing development branch.

		See the <a href="/freewpc/subversion.html">Subversion starter page</a>
		for general information about Subversion commands.

		For those without Subversion, you can use the web tool at
		%url(websvn/) to browse the repository and download tarballs.

	}
	{Detailed Design
		This guide covers the high-level architecture and design of
		FreeWPC.  For more specific information about how a particular
		module works, refer to the low-level design documentation that is
		automatically generated from the code using %tool(doxygen).
		The latest version is at %url(freewpc/doxygen).


	}
	{Build System : How the source code gets compiled into a ROM image
		An older overview of the build system is 
		<a href="/freewpc/build.html">here</a>.

		{System Requirements : What you need in order to compile the program
			FreeWPC requires a UNIX or UNIX-compatible system in order
			to compile game ROMs.  The build system relies heavily on an
			assorted to standard UNIX utilities to perform many tasks.
			In addition, a few nonstandard utilities may be required as well.
		}
		{Directory Layout : How the files are organized
			At the root of a particular branch of the source code tree,
			FreeWPC files are divided into several more subdirectories:
			{Kernel Sources
				The %file(kernel) directory contains the source files
				for the kernel functions.  These are linked into the
				system region of the ROM.
			}
			{Kernel Includes
				The %file(include) directory contains all of the
				include files.  The hardware specific files tend to be
				in the %file(include/sys) subdirectory, while the other
				files are found directly under %file(include).
			}
			{Machine Sources
				%file(mach) is a symbolic link to the machine-specific
				source directory.  This symbolic link is set at
				configuration time.
			}
			{Machine Includes
				%file(include/mach) is similar, but points to a
				machine-specific include directory.
			}
			{Images
			}
			{Fonts
				%file(fonts) contains the glyphs and control information
				for all of the supported fonts that can be compiled in.
			}
		}
		{Configuring the Build : How to control what gets built
			The build procedure is configured by specifying different
			combinations of make variables.  Also, different make targets
			are provided to perform different actions on the current
			configuration.

			{Make Variables
				{Compiler and Assembler Options
					<GCC_VERSION
						Specifies the version of the gcc6809 compiler
						to use.  If not specified, this defaults to
						gcc 3.4.5.
					<ASVER
						Specifies the version of the as6809 assembler
						to use.  If not specified, this defaults to
						as 3.0.0.
					<NEWAS
						When using as 3.0.0 or higher, this should be
						set to 1.
					<SAVE_ASM
						When set to 'y', the intermediate assembler code
						is saved.  This defaults on.
					<DEBUG_COMPILER
						Set this to 'y' in order to generate lots of
						compiler debug output.  This is needed only
						to debug the compiler itself, not FreeWPC.
					>
				}
				{FreeWPC Build Options
					<MACHINE
						Define this to be the machine you want to
						build.  This is a required define.
					<SYSTEM_MAJOR, SYSTEM_MINOR
						Specifies the system major/minor version
						numbers.
					<MACHINE_MAJOR, MACHINE_MINOR
						Specifies the game major/minor revision 
						numbers.  If not set, the system version
						numbers will be used.
					<FREE_ONLY
						Define this to 'y' to create a ROM that will
						not count credits, and can only operate in
						free play mode.
					>
				}
				{Debugging Options
					<FREEWPC_DEBUGGER
						Define this to 'y' to enable debug messages.
					<USER_TAG
						Optionally this to a string that is included
						in the ROM image for tagging purposes.  It
						should not be defined if it is not needed.
						The tag is displayed at startup if set.
					<TARGET_ROMPATH
						Identifies the directory in which PinMAME
						zip files are located.  This is only needed
						if debugging with PinMAME.  The Makefile
						will automatically copy newly built ROM
						images into this directory for testing, if this
						is defined.
					<GAME_ROM_PREFIX
					>
				}
			}
			{user.make files
				When running '''make''', the build system will scan
				the top-level directory for a file called
				%file(user.make) for variable settings.  You should
				place commonly used options into %file(user.make)
				rather than specifying them each time you run
				'''make'''.

				You can also place a %file(user.make) file in your
				home directory.  It works the same way, but settings
				in the FreeWPC copy of %file(user.make) will override
				any settings from the home directory.

				A file named %file(user.make.example) is included
				in the distribution as an example of how to write
				the file.
			}
		}
		{Building the Code
			Running '''make''' without an explicit target name will 
			build the game ROM, and install it into the PinMAME ROM
			directory if one exists.  The procedure is fairly lengthy
			and involves a number of steps:
			<Create Blank File
				If the ROM size is larger than the number of pages that
				need to be compiled, then the final ROM image must be
				padded with blanks.  The %tool(dd) command is used
				to generate a file named '''blankxxx.bin''', where ///xxx///
				is the size of the file in kilobytes (KB).
			<Create Linker Command Files
			<Create XBM Prototypes
			<Setting the Machine Symbolic Links
			<Generating Defines
			<Generating Callsets
			<Compiling and Assembling Source Code
				Source code is compiled using the GCC6809 compiler.
				The compiler generates assembler code with the '''.S'''
				extension.  These files are then assembled using the
				asxxxx assembler tools into object files with the '''.o'''
				extension.
			<Compiling Page Headers
			<Linking Pages
				The aslink utility is used to create one S-record file,
				with the '''.S19''' extension, for each page of ROM.
			<Converting Pages to Binary
				The %file(tools/srec2bin) utility included with FreeWPC
				is run to convert the S-records into raw binary files.
			<Concatenating Pages
				The binary files are concatenated, along with any blank
				files, to form the final ROM image.
			>
		}
		{Alternate Make Targets
			There are a number of other actions you can take other 
			than just building the code.  Use '''make ///target///'''
			to perform these actions, where the possible values for
			///target/// are listed below:
			<clean
				Removes all temporary files, such as object files,
				debug output, etc.
			<doc
				Generates the %tool(doxygen) documentation from the
				source code.  The files are placed in the %file(doc)
				directory.
			<gendefines_again
				Recreates all of the gendefine header files.
				These are not automatically regenerated every time you
				add or remove a new define to the code.
			<callset
				Recreates all of the callset files.
			<info
				Dumps many of the Makefile variable settings.
			>
		}
	}
}
{Hardware Description
	{The 6809 CPU
		The 6809 is a 2MHz, 8-bit Motorola processor.  This means that
		it can execute 2 million cycles per second.  Each assembler
		instruction takes one or more cycles to complete, depending
		on its complexity.  A single cycle is 500ns long.

		It supports two interrupts: a normal '''IRQ''', and
		a '''fast IRQ''', or '''FIRQ'''.  The difference is that
		when an IRQ occurs, all registers are saved to the
		stack.  The FIRQ handler must manually save any registers
		that need to be preserved.  On the WPC platform, the
		IRQ is driven via a crystal to fire 1024 times per second,
		or about once every 976 microseconds (equivalently, once
		every 1952 cycles).

		The FIRQ is asserted by the WPC, when programmed to do so,
		when the DMD controller has finished drawing a particular
		scanline or when its internal timer register expires.

		Interrupts can be disabled/enabled using the following
		functions:

		<%api(disable_irq), %api(enable_irq)
		<%api(disable_firq), %api(enable_firq)
		<%api(disable_interrupts), %api(enable_interrupts)
		>

		FreeWPC uses a port of '''GCC''', the GNU C compiler,
		to generate the 6809 assembly language code.
	}
	{Memory Map : How the processor sees memory and devices
		All memory and I/O is mapped into a unified 64KB address
		space.  The 6809 read and writes are initiated to the
		WPC ASIC, which decodes the address and forwards the
		request on to the correct device.
			<RAM
				Mapped from $0000 to $1FFF, for a total of 8KB.
				The WPC allows a configurable portion of the RAM 
				to be write-protected; trying to write this area
				without first unlocking the memory will cause
				the write to be ignored.
			<Dot Matrix Controller
				Mapped from $3800 to $3BFF, for a total of 1KB.
				The controller provides 16 display pages, each
				512 bytes in size, but only 2 of them are mapped
				into memory at a time.  The ///current low page///
				is mapped at $3800, and the ///current high page///
				is mapped at $3A00.  There is also an
				///active page register/// which controls which
				of the 16 pages is actually visible; the visible
				page may or may not be mapped.
			<ASIC I/O
				Mapped from	$3E00 to $3FFF.  This region contains
				all of the WPC memory-mapped I/O registers for
				reading switches, turning on lamps, etc.

				More detailed descriptions of the ASIC registers
				are included in %link(Hardware Functions).
			<Paged ROM
				Mapped from $4000 to $7FFF.  This is a 16KB window
				in which a portion of the ROM appears.  ROM
				varies in size and can be anywhere from 128KB to
				1MB.  The WPC can be programmed to determine which
				16KB section of the ROM appears here.
			>
			<Fixed ROM
				Mapped from $8000 to $FFFF.  The upper 32KB of
				the ROM image is always mapped here.
	}
	{WPC ASIC Summary : How the hardware is controlled
		The WPC chip is essentially a giant address decoder that maps
		a bunch of peripheral devices into the address space of the CPU, a
		Motorola 6809.  By reading/writing addresses between $3F00 and $3FFF,
		the CPU is able to access the peripherals.  This includes all of
		the playfield switches, the solenoids, the lamps, etc.  It also
		includes a number of other devices like the real-time clock and
		a bit shifter.  Earlier generations of Williams pinballs implement
		a lot of the I/O in custom TTL logic like PIAs.  The WPC is a
		single ASIC.
	}
}
{The FreeWPC Runtime
	{Kernel Functions : Common functions used by all machines
		The kernel provides all of the functions that are common
		across different machines.  Where some variation is
		required, there are often #defines beginning with MACHINE_
		that declare alternate settings or behavior.

		Functions can be broken down into a number of categories:
		{Error Handling
			The function fatal() is called from a number of places
			whenever something goes really wrong.  fatal()
			takes one parameter, an ///errno value///.  A call
			to fatal() will lead to a reset of the machine.

			The values for errno are fixed and defined in
			%file(include/sys/errno.h).

			A companion function, nonfatal(), is used whenever
			the system can continue running.

			The debugger can be used to determine more information
			about what went wrong when an error is thrown.
		}
		{Operating System Functions : Typical embedded OS routines
			{Multitasking : How multiple things go on at once
				FreeWPC implements a round-robin, non-preemptive task
				scheduler.

				The minimum task granularity is defined by IRQS_PER_TICK.
				Currently, this is set to 16, meaning that 1 task tick
				is equal to about 16ms.  No task can request to sleep 
				for less than 1 tick.  Also, no task can directly sleep
				for longer than 256 ticks, because the internal task
				structure stores the sleep time as an 8-bit value.
				There is a higher-layer API for sleeping a certain number
				of seconds, which makes repeated calls to the underlying
				API to sleep 1s at a time.

				If a task does not give up control, either by sleeping,
				exiting, or yielding after a certain amount of time,
				the fatal error '''ERR_FCFS_LOCKUP''' will be asserted.
			}
			{Real-Time Functions : How to perform high priority, recurring actions
				Because tasks are not preemptive, they are not
				guaranteed to run with any particular frequency.  For
				high priority tasks, a guarantee is often required.
				These types of functions are invoked from the IRQ
				handler, which is called by the 6809 about once every
				976 microseconds.
			}
			{Timers
				Timers are implemented as tasks that execute a special
				timer function.  All of the normal task APIs can be
				used on timers as well, although renamed versions exist
				for clarity.  Tasks and timers share the same group IDs
				(GIDs).

				!file timer.c
			}
		}
		{Hardware Functions : Interfacing to the I/O
			The kernel provides a common set of hardware access functions
			that work across all of the different variations of the WPC
			architecture.
			{Switches
				WPC uses a switch matrix.

				!file switches.c
				The kernel provides the switch handlers for certain
				switches that exist in all machines.
				{Coin Switches
					Up to 4 coin switches are supported.  The values
					assigned to each of them can be configured in
					service mode.

					!file coin.c
				}
				{Tilt Switches
					!file tilt.c
				}
				{Service Switches
					!file service.c
				}
				{Trough Switches
					The number and location of the trough switches
					is given in the machine config header file.
					This module declares a trough ball device
					using those switches and the ball serve
					solenoid.

					Trough switch handling is special, because
					trough entry is a criterion for determine the
					end-of-ball.

					!file trough.c
				}
			}
			{Lamps
				WPC supports up to 64 lamps, arranged in an 8x8 matrix.
				The '''column strobe register''' identifies which of
				the 8 lamp columns is currently being driven.
				The '''row value register''' determines the states of
				those lamps, one bit per lamp.  All lamps on other
				columns remain off.

				The illusion of all 64 lamps being on at once is
				done by rapidly switching the column strobe.
				The strobe is changed every 2ms.

				!file lamp.c
			}
			{Solenoid Drives
				WPC supports four standard drive output registers,
				with one bit per drive, for a total of 32 drives.
				These are not arranged in a matrix at all, so all
				solenoids can potentially be active at the same time.

				The power driver board has different characteristics
				for each set of drives, providing for low-power,
				high-power, and general purpose solenoids.  From
				the CPU's perspective, all of them are equivalent.

				Two flavors of solenoid APIs are provided.  The
				first lets you directly control the state of a drive.
				The second allows you to specify a duty cycle period.
				Then, the drive is rapidly cycled between a set of
				states.  This is useful with solenoids to prevent
				coil fatigue.

				<%api(sol_on)
				<%api(sol_off)
				<%api(sol_pulse)
				>

				!file sol.c
			}
			{Flashlamps
				The flashlamp APIs are similar to the solenoid APIs,
				except that they are momentary.  A flashlamp is
				pulsed briefly, after which it is turned off.

				<%api(flasher_pulse)
				>

				!file flasher.c
			}
			{Dot-Matrix Display (DMD)
				!file dmd.c
			}
			{Sound Board
				Two flavors of sound board are supported: WPC, and DCS.
				The same APIs are used in either case.

				!file sound.c
			}
			{Triacs and General Illumination
				!file triac.c, gi.c
			}
			{Flippers
				WPC supports two styles of flipper hardware.
				FreeWPC only supports the newer, Fliptronic style.
				The I/O registers for the Fliptronic board moved when
				WPC95 came out, but they are functionally equivalent.

				!file flip.c
			}
			{Real-Time Clock
				The WPC contains an internal realtime clock.
				The battery power keeps the RTC running even when the
				machine is turned off.

				FreeWPC keeps the current time-of-day in NVRAM.
				Periodically, it checks the RTC and updates the
				copy in memory to reflect any changes.

				!file rtc.c
			}
			{Debug Port
				!file db.c
			}
			{Paging and Locking
				<%api(wpc_set_ram_protect)
				<%api(wpc_set_ram_protect_size)
				<%api(wpc_get_rom_page)
				<%api(wpc_set_rom_page)
				<%api(wpc_get_ram_page)
				<%api(wpc_set_ram_page)
				>
				<%api(wpc_nvram_get)
				<%api(wpc_nvram_put)
				<%api(wpc_push_page)
				<%api(wpc_pop_page)
				<%api(call_far)
			}
			{Bit Shifting
				The ASIC has a builtin shifter that can do some 
				bitwise math faster than native 6809 code.

				'''gcc6809''' has an option '''-mwpc''' which
				enables emitting instructions for the WPC platform.
				In some cases, the hardware shifter will be used
				automatically.  Note that gcc is unaware of the
				threading/interrupt model, so access to the shifter
				is not protected.  In the current implementation,
				the shifter is not used from the IRQ, and since
				tasks are nonpreemptive, that is no problem.

			}
		}
		{Standard Features : Pinball software constructs
			{Game Logic
				This module implements the standard state machine for
				the lifecycle of a pinball game.

				Initially, the machine is in ///attract mode/// when it
				is not being played.  Then, after coins are inserted
				and start is pressed, the machines goes into 
				///game mode///.  Each game consists of a number of
				///balls///, with each ball divided into ///start///,
				///in-play///, ///bonus///, and ///end/// states.

				At the end of a game, there are optionally
				///high score entry/// and ///match/// modes.  Then
				the machine returns to the attract mode.

				When the operator goes into test mode, that is also
				a different state, as most of the automatic functionality
				is disabled.

				There is also ///tilt mode///, which is a companion state
				to ///in-play/// mode after the player has tilted the
				ball away.

				!file game.c
			}
			{Ball Devices : Handling devices that can hold and release pinballs
				!file device.c
			}
			{Extra Balls : Tracking extra balls
				!file eb.c
			}
			{High Scores : Managing the high score list(s)
				!file highscore.c
			}
			{Player Local Data : Managing per-player data in multiplayer games
				!file player.c
			}
			{Ball Search : What to do when balls are missing
				!file search.c
			}
		}
		{Effects : More complex display and lamp shows
			Effects build upon the basic hardware control and make
			the game more user-friendly.

			{Text and Fonts
				!file printf.c, font.c
			}
			{Lampsets
				Typically, groups of lamps are often used for a
				similar purpose.  A lampset is a just of set of
				lamps that are often controlled together.  Each
				lampset has a unique ID, and is internally
				represented by an array of lamp values, terminated
				by a special lamp code named '''LAMP_END'''.

				The lampset APIs provide convenience whenever
				many lamps need to be updated at once.

				!file lampset.c
			}
			{Display Effects
				!file deff.c
			}
			{Lamp Effects
				!file leff.c
			}
			{DMD Transitions
				A transition is an interim effect that runs between
				the end of one display effect and the beginning of
				another.  By default, a new effect will simply kill
				the old one and overwrite it with a new image.

				Transitions are scheduled -- a handler is installed
				that runs the next time that a new image is shown.
				Instead of showing the image right away, the transition
				handler kicks in and displays some combination of
				the present view and the new one.  When the transition
				finishes, the display should consist entirely of the
				new image.
				
				A number of different transition types are available.

				<Striping
					The new image overwrites the old one in vertical
					sections.  Variations include ///left to right///,
					///right to left///, ///inside out///, and
					///outside in///.
				<Scrolling/Pushing
					The new image pushes the old image off the
					display.  The new image can originate from any
					of the 4 edges of the display.
				<Fading
				>
				!file dmdtrans.c
			}
		}
		{Adjustments
			!file adj.c
		}
		{Audits
			!file audit.c
		}
		{Scoring
			This module provides functions for dealing with ///scores///,
			which are kept as BCD arrays.
			!file score.c
		}
	}
	{Machine Functions
		{Machine Configuration File
			Each machine must provide a configuration file named
			%file(config.h) which defines most of the machine-specific
			configuration options.
			{General Configuration Options
				<MACHINE_NAME
					%reqdef(a short name for the machine)
				<MACHINE_PINMAME_ZIP
				<MACHINE_PINMAME_ROM
				<MACHINE_DMD
					Set to 1 if the machine has a dot matrix display,
					or 0 otherwise.  This must be defined.
				<MACHINE_DCS
					Set to 1 if the machine has a DCS sound board,
					or 0 otherwise.  This must be defined.
				<MACHINE_FLIPTRONIC
					Set to 1 if the machine uses Fliptronic-style flippers,
					or 0 otherwise.  This must be defined.
				<MACHINE_WPC95
					Set to 1 if the machine runs on the WPC95 variant of
					the hardware, or 0 otherwise.  This must be defined.
				>
			}
			{Declaring Switches
				<MACHINE_SHOOTER_SWITCH
					%reqdef(the shooter switch number)
				<MACHINE_TILT_SWITCH
					%reqdef(the tilt switch number)
				<MACHINE_SLAM_TILT_SWITCH
					%reqdef(the slam tilt switch number)
				<MACHINE_START_SWITCH
					%reqdef(the start button switch number)
				<MACHINE_BUYIN_SWITCH
					%optdef(the buyin switch number)
				<MACHINE_LAUNCH_SWITCH
					%optdef(the launch button switch number)
					This is only required for machines which use an autoplunger.
				<MACHINE_OUTHOLE_SWITCH
					%optdef(the outhole switch number)
					This is only required for machines which use an outhole mechanism to feed the ball trough.
				<MACHINE_TROUGH_SIZE
					%reqdef(the number of trough switches)
				<MACHINE_TROUGHn
					%reqdef(the switch numbers of all of the trough switches)
					You only need to define as many of these as are necessary.
				>
			}
			{Declaring Lamps
				<MACHINE_START_LAMP
					%optdef(the start button lamp number)
					The start lamp is controlled by the system to
					reflect when a game can be started.
					The machine should avoid controlling this lamp
					directly.
				<MACHINE_BUYIN_LAMP
					%optdef(the buy-in button lamp number)
					The buyin lamp is controlled by the system to
					reflect when an extra ball can be purchased.
					The machine can choose to use it at other times, too.
				<MACHINE_LAMPSETS
					%optdef(the list of machine-defined lampsets)
				>
			}
			{Declaring Solenoids
				<MACHINE_HAS_UPPER_LEFT_FLIPPER, MACHINE_HAS_UPPER_RIGHT_FLIPPER
					%optdef(if the machine has upper flippers or not)
					When not present, the flipper update function will
					avoid driving those ports.
				<MACHINE_BALL_SERVE_SOLENOID
					%reqdef(the ball serve solenoid number)
					This is the coil that kicks balls from the trough to the plunger lane.
				<MACHINE_KNOCKER_SOLENOID
					%optdef(the knocker solenoid number)
				<MACHINE_LAUNCH_SOLENOID
					%optdef(the autolaunch solenoid number)
					This is only required for machines which use an autoplunger.
				<MACHINE_SOL_FLASHERP
					%optdef(which drives are connected to flashers, as opposed to solenoids)
				<MACHINE_SOL_NOSEARCHP
					%optdef(which solenoids should not be pulsed during a ball search)
					If not set, then no solenoids will be fired 
					automatically during a ball search.  The machine can
					still fire solenoids from its ball search hook.
				>
			}
			{Declaring Display and Lamp Effects
				<MACHINE_DISPLAY_EFFECTS
					%optdef(a list of machine-defined display effects)
					These effects are concatenated with the list of
					effects defined by the system.
				<MACHINE_LAMP_EFFECTS
					%optdef(a list of machine-defined lamp effects)
					These effects are concatenated with the list of
					effects defined by the system.
				>
			}
			{Declaring Sounds
				<MACHINE_ADD_COIN_SOUND
					%optdef(a sound to be played when a coin is inserted, without awarding a full credit)
				<MACHINE_ADD_CREDIT_SOUND
					%optdef(a sound to be played when a credit is added)
				<MACHINE_VOLUME_CHANGE_MUSIC
					%optdef(the music to be played when the volume is being changed)
				>
			}
			{Other Machine Defines
				<MACHINE_SCORE_DIGITS
					%reqdef(the maximum number of digits for a variable of type ///score_t///)
				<MACHINE_AMODE_LEFT_FLIPPER_HANDLER, MACHINE_AMODE_RIGHT_FLIPPER_HANDLER
					%optdef(a function to be called whenever the flippers are pressed in attract mode)
				<MACHINE_CUSTOM_AMODE
					%optdef(the name of the machine's own attract mode function)
					It can be omitted, and the system will install a
					simple, default attract mode.
				<MACHINE_TEST_MENU_ITEMS
					%optdef(a list of extra items to be added to the Tests menu)
				<MACHINE_HOOKS
					%reqdef(the name of the machine hooks structure)
				<MACHINE_xMS_RTTS
					%optdef(the names of additional real-time functions to be run)
				>
			}
		}
		{Machine Hooks
			Hooks are extensions to the system program that provide
			for additional functions to be performed at various times.
			The MACHINE_ defines change kernel behavior, while the
			hooks augment it.

			All hooks are optional and can be left as NULL if no
			additional processing is required by the machine.

			<init
			<start_game_attempt
			<start_game
			<add_player
			<start_ball
			<ball_in_play
			<ball_drain
			<end_ball
			<bonus
			<abort_game
			<end_game
			<amode_start
			<amode_stop
			<tilt_warning
			<tilt
			<slam_tilt
			<replay
			<high_score
			<grand_champion
			<coin_added
			<credit_added
			<start_without_credits
			<volume_change
			<ball_search
			<any_pf_switch
			<lamp_reflash
			>
		}
	}
	{Test Mode
		!file window.c
	}
	{Initialization
		{Interrupt Vectors
			!file vector.c
		}
		{Reset Routine
		}
	}
}

{Build Scripts
	{Generating defines automatically
		The '''gendefine''' script scans all of the source code
		for uses of a ///#define/// that match a certain pattern.
		It then creates a ///.h/// file with all of these names,
		and assigns them sequential integer values.

		Gendefines are used to avoid having to update a header
		file anytime new values are added or removed.  You
		simply use the values, making sure that they are named
		appropriately for '''gendefine''' to find them.

		Gendefines are currently used for: task group IDs (GIDs),
		lampset numbers, and display/lamp effect numbers.

		The %file(tools/gendefine) Perl script implements this
		functionality.

		By convention, all of the output files are placed into the
		%file(include) directory and begin with the prefix
		'''gendefine_'''.
	}
	{Generating function calls automatically
	}
	{Font Generation
	}
}

{Debugging
	{PinMAME
		So far, all testing of FreeWPC has been done using the
		PinMAME emulator.
	}
	{The Debugger
		FreeWPC can be built with embedded debug support.  When this
		is turned on, using the FREEWPC_DEBUGGER make option,
		various messages get printed to the WPC debug port.
		PinMAME has been patched to capture this information and
		send it to an external debugger program, which can display
		it to the user.

		The support also allows a user to send commands
		from the debugger to a running FreeWPC ROM to interact with
		it.
	}
}

{Limitations
	{Performance
		The GCC compiler sometimes generates poor code, especially for
		complex expressions.

		Subroutine calls inside the IRQ handler account for
		a large portion of its running time.  The call
		sequence is mostly static and could be optimized
		at compile time by using jump instructions instead
		of calls.
	}
	{Fault Tolerance
		%api(fatal) should reboot the system immediately.

		Initialization should ensure that the hardware registers
		have sane values before doing anything else.

		ROM and RAM tests should be performed even earlier,
		to make sure that the program is valid and that the
		interface to memory is OK.
	}
	{Hacks
		{PinMAME Time-of-Day Functions
		PinMAME currently assumes that time-of-day information
		should be written to address $1800.  This is apparently
		the address where all real WPC programs keep their
		time data.

		FreeWPC doesn't work the same way, but it avoids using
		that address of RAM so that its own data isn't
		overwritten by PinMAME.
		}
	}
}
{References
}

