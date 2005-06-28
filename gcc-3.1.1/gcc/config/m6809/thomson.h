/*-------------------------------------------------------------------
	FILE: thomson.h
-------------------------------------------------------------------*/
/* Definitions of target machine for GNU compiler.  Thomson version.

By Eric Botcazou (ebotcazou@multimania.com)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/


/* declarations for the specs file */
#define ASM_SPEC "-m"

#define CC1_SPEC "-fno-builtin"

#define ENDFILE_SPEC "crtn.o%s"

#define LINK_SPEC "-t"

#define LIB_SPEC "-lc"

#define STARTFILE_SPEC "%{compact:lcrt0.o%s} %{!compact:crt0.o%s}"
