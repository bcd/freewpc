/*
 * Copyright 2006, 2007 by Brian Dominy <brian@oddchange.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * \file
 * \brief Hardware diagnostic checks.
 * Failures detected by this module result in the "credit dot".
 */

#include <freewpc.h>
#include <diag.h>


diag_error_code_t diag_error_list[MAX_DIAG_ERRORS];

U8 diag_error_count;


U8
diag_get_error_count (void)
{
	return diag_error_count;
}

#ifdef NOT_IMPL
void
diag_post_error (diag_error_code_t errcode)
{
	if (diag_error_count < MAX_DIAG_ERRORS)
		diag_error_list[diag_error_count++] = errcode;
}

void
diag_post_fatal_error (diag_error_code_t errcode)
{
}


void
diag_test_cpu (void)
{
}

void
diag_test_ram (void)
{
}

void
diag_test_rom (void)
{
}

void
diag_test_wpc (void)
{
}


void
diag_report_errors (void)
{
}

#endif

void
diag_run (void)
{
	diag_error_count = 0;

#ifdef NOT_IMPL
#if 0
	diag_post_error (DIAG_NOT_IMPLEMENTED);
#endif
	diag_test_cpu ();
	diag_test_ram ();
	diag_test_rom ();
	diag_test_wpc ();
#endif
}


CALLSET_ENTRY (diag, init)
{
	diag_run ();
	if (diag_get_error_count () > 0)
	{
	}
}

