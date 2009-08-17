/*
 * Copyright 2006, 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
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

#include <freewpc.h>

/**
 * \file
 * \brief Audit module.
 *
 * This module declares non-volatile variables (in the protected area
 * of the RAM) for storing audit information.
 */

__nvram__ std_audits_t system_audits;

#ifdef MACHINE_FEATURE_AUDITS
__nvram__ feature_audits_t feature_audits;
#define MACHINE_FEATURE_AUDIT_SIZE sizeof (feature_audits)
#else
#define MACHINE_FEATURE_AUDIT_SIZE 0
#endif

__nvram__ U8 audit_csum;

const struct area_csum audit_csum_info = {
	.area = (U8 *)&system_audits,
	.length = sizeof (system_audits) + MACHINE_FEATURE_AUDIT_SIZE,
	.csum = &audit_csum,
	.reset = audit_reset,
};



/** Resets all audits to zero */
void audit_reset (void)
{
	pinio_nvram_unlock ();
	memset (&system_audits, 0, sizeof (system_audits));
#ifdef MACHINE_FEATURE_AUDITS
	memset (&feature_audits, 0, sizeof (feature_audits));
#endif
	csum_area_update (&audit_csum_info);
	pinio_nvram_lock ();
}


/** Increment an audit by 1 */
void audit_increment (audit_t *aud)
{
	if (*aud < 0xFFFF)
	{
		pinio_nvram_unlock ();
		(*aud)++;
		csum_area_update (&audit_csum_info);
		pinio_nvram_lock ();
	}
}


/** Increment an audit by an arbitrary value */
void audit_add (audit_t *aud, U8 val)
{
	if (*aud < 0xFFFF - (val - 1))
	{
		pinio_nvram_unlock ();
		(*aud) += val;
		csum_area_update (&audit_csum_info);
		pinio_nvram_lock ();
	}
}


/** Assign an audit value directly */
void audit_assign (audit_t *aud, audit_t val)
{
	pinio_nvram_unlock ();
	(*aud) = val;
	csum_area_update (&audit_csum_info);
	pinio_nvram_lock ();
}


CALLSET_ENTRY (sys_audit, factory_reset)
{
	audit_reset ();
}

