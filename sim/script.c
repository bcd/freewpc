/*
 * Copyright 2009-2011 by Brian Dominy <brian@oddchange.com>
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
#include <simulation.h>
#include <ctype.h>

const char *tlast = NULL;

char tstringbuf[128];

#define delims " \t\n"
#define tfirst(cmd)   strtok (cmd, delims)
#define teq(t, s)     !strcmp (t, s)

/**
 * Return the next token as a string.
 */
const char *tnext (void)
{
	if (tlast)
	{
		const char *res = tlast;
		tlast = NULL;
		return res;
	}
	return strtok (NULL, delims);
}


/**
 * Push back the last token that was retrieved via
 * tnext().
 */
void tunget (const char *t)
{
	tlast = t;
}


const char *tstring (void)
{
	const char *t;
	char *c;

	t = tnext ();
	if (!t)
		return NULL;
	if (*t != '"')
		return t;

	strcpy (tstringbuf, t+1);
	do {
		t = tnext ();
		if (!t)
		{
			simlog (SLC_DEBUG, "Parse error after '%s'", tstringbuf);
			return NULL;
		}
		strcat (tstringbuf, " ");
		strcat (tstringbuf, t);
		c = strchr (tstringbuf, '"');
	} while (c == NULL);
	*c = '\0';
	return tstringbuf;
}


/**
 * Read the next token and interpret it as a constant.
 * Return its value.
 */
uint32_t tconst (void)
{
	const char *t = tnext ();
	uint32_t i;

	/* If no value is given, default to 0. */
	if (!t)
		return 0;

	/* Interpret certain fixed strings */
	if (teq (t, "on") || teq (t, "high") || teq (t, "active"))
		return 1;
	if (teq (t, "off") || teq (t, "low") || teq (t, "inactive"))
		return 0;

	/* Dollar sign indicates a variable expansion */
	if (*t == '$')
	{
		if (isdigit (t[1]))
			return conf_read_stack (t[1] - '0');
		else
			return conf_read (t+1);
	}

	/* TODO : Builtin strings */

	/* Anything else is interpreted as a C-formatted number. */
	i = strtoul (t, NULL, 0);

	/* Optional modifiers that can occur after the number */
	t = tnext ();
	if (t)
	{
		if (teq (t, "ms"))
			;
		else if (teq (t, "secs"))
			i *= 1000;
		else
			tunget (t);
	}
	return i;
}


/**
 * Read the next token, which must be a signal name.
 * Currently, this must be the last token in the command.
 * The general format is <type> <number>.
 */
uint32_t tsigno (void)
{
	const char *t = tnext ();
	uint32_t signo;

	if (teq (t, "sol"))
		signo = SIGNO_SOL;
	else if (teq (t, "zerocross"))
		signo = SIGNO_ZEROCROSS;
	else if (teq (t, "triac"))
		signo = SIGNO_TRIAC;
	else if (teq (t, "lamp"))
		signo = SIGNO_LAMP;
	else if (teq (t, "sol_voltage"))
		signo = SIGNO_SOL_VOLTAGE;
	else if (teq (t, "ac_angle"))
		signo = SIGNO_AC_ANGLE;
	else
		return 0;
	signo += tconst ();
	return signo;
}


/**
 * Parse a complex expression and return a
 * signal_expression that describes it.
 */
struct signal_expression *texpr (void)
{
	struct signal_expression *ex, *ex1, *ex2;
	const char *t;

	t = tnext ();
	if (!t)
		return NULL;

	ex = expr_alloc ();
	if (teq (t, "at"))
	{
		ex->op = SIG_TIME;
		ex->u.timer = tconst ();
	}
	else if (teq (t, "after"))
	{
		ex->op = SIG_TIMEDIFF;
		ex->u.timer = tconst ();
	}
	else
	{
		tunget (t);
		ex->op = SIG_SIGNO;
		ex->u.signo = tsigno ();
		simlog (SLC_DEBUG, "Signo changed = 0x%X\n", ex->u.signo);

		t = tnext ();
		if (t)
		{
			if (teq (t, "is"))
			{
				ex1 = ex;

				ex2 = expr_alloc ();
				ex2->op = SIG_CONST;
				ex2->u.value = tconst ();

				ex = expr_alloc ();
				ex->op = SIG_EQ;
				ex->u.binary.left = ex1;
				ex->u.binary.right = ex2;
			}
		}
	}

	t = tnext ();
	if (t)
	{
		ex1 = ex;
		ex2 = texpr ();
		ex = expr_alloc ();
		ex->u.binary.left = ex1;
		ex->u.binary.right = ex2;
		if (teq (t, "and"))
			ex->op = SIG_AND;
		else if (teq (t, "or"))
			ex->op = SIG_OR;
	}

	return ex;
}


uint32_t tsw (void)
{
	const char *t;
	uint32_t n;

	t = tstring ();
	for (n=0; n < NUM_SWITCHES; n++)
	{
		const char *swname = names_of_switches[n];
		if (swname && !strcmp (swname, t))
			return n;
	}
	tunget (t);
	return tconst ();
}


/**
 * Parse and execute a script command.
 */
void exec_script (char *cmd)
{
	const char *t;
	uint32_t v, count;

	tlast = NULL;

	/* Blank lines and comments are ignored */
	t = tfirst (cmd);
	if (!t)
		return;
	if (*t == '#')
		return;

	/*********** capture [subcommand] [args...] ***************/
	if (teq (t, "capture"))
	{
		struct signal_expression *ex;

		t = tnext ();
		if (teq (t, "start"))
		{
			ex = texpr ();
			signal_capture_start (ex);
		}
		else if (teq (t, "stop"))
		{
			ex = texpr ();
			signal_capture_stop (ex);
		}
		else if (teq (t, "debug"))
		{
		}
		else if (teq (t, "file"))
		{
			t = tnext ();
			signal_capture_set_file (t);
		}
		else if (teq (t, "add"))
		{
			signal_capture_add (tsigno ());
		}
		else if (teq (t, "del"))
		{
			signal_capture_del (tsigno ());
		}
	}

	/*********** set [var] [value] ***************/
	else if (teq (t, "set"))
	{
		t = tnext ();
		v = tconst ();
		conf_write (t, v);
	}
	/*********** p/print [var] ***************/
	else if (teq (t, "p") || teq (t, "print"))
	{
		v = tconst ();
		simlog (SLC_DEBUG, "%d", v);
	}
	/*********** include [filename] ***************/
	else if (teq (t, "include"))
	{
		t = tnext ();
		exec_script_file (t);
	}
	/*********** sw [id] ***************/
	else if (teq (t, "sw"))
	{
		v = tsw ();
		count = tconst ();
		if (count == 0)
			count = 1;
		while (count > 0)
		{
			sim_switch_depress (v);
			count--;
		}
	}
	/*********** swtoggle [id] ***************/
	else if (teq (t, "swtoggle"))
	{
		v = tsw ();
		count = tconst ();
		if (count == 0)
			count = 1;
		while (count > 0)
		{
			sim_switch_toggle (v);
			count--;
		}
	}
	/*********** key [keyname] [switch] ***************/
	else if (teq (t, "key"))
	{
		t = tnext ();
		v = tsw ();
		simlog (SLC_DEBUG, "Key '%c' = %s", *t, names_of_switches[v]);
		sim_key_install (*t, v);
	}
	/*********** push [value] ***************/
	else if (teq (t, "push"))
	{
		v = tconst ();
		conf_push (v);
	}
	/*********** pop [argcount] ***************/
	else if (teq (t, "pop"))
	{
		v = tconst ();
		conf_pop (v);
	}
	/*********** sleep [time] ***************/
	else if (teq (t, "sleep"))
	{
		v = tconst ();
		simlog (SLC_DEBUG, "Sleeping for %d ms", v);
		v /= IRQS_PER_TICK;
		do {
			task_sleep (TIME_16MS);
		} while (--v > 0);
		simlog (SLC_DEBUG, "Awake again.", v);
	}
	/*********** exit ***************/
	else if (teq (t, "exit"))
	{
		sim_exit (0);
	}
}


/**
 * Execute a series of script commands in the named file.
 */
void exec_script_file (const char *filename)
{
	FILE *in;
	char buf[256];

	in = fopen (filename, "r");
	if (!in)
		return;
	simlog (SLC_DEBUG, "Reading commands from '%s'", filename);
	for (;;)
	{
		if (!fgets (buf, 255, in))
			break;
		if (feof (in))
			break;
		exec_script (buf);
	}
	simlog (SLC_DEBUG, "Closing '%s'", filename);
	fclose (in);
}

