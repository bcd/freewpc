
#include <freewpc.h>
#include <simulation.h>

const char *tlast = NULL;

#define delims " \t\n"
#define tfirst(cmd)   strtok (cmd, delims)
#define teq(t, s)     !strcmp (t, s)

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

void tunget (const char *t)
{
	tlast = t;
}

uint32_t tconst (void)
{
	const char *t = tnext ();
	uint32_t i;

	if (!t)
		return 0;
	if (teq (t, "on") || teq (t, "high") || teq (t, "active"))
		return 1;
	if (teq (t, "off") || teq (t, "low") || teq (t, "inactive"))
		return 0;
	if (*t == '$')
		return conf_read (t+1);
	i = strtoul (t, NULL, 0);

	t = tnext ();
	if (t)
	{
		if (teq (t, "secs"))
			i *= 1000;
	}
	return i;
}

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

	t = tnext ();
	if (t)
		signo += strtoul (t, NULL, 0);
	return signo;
}


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


void exec_script (char *cmd)
{
	const char *t;
	uint32_t v;

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
		t = tnext ();
		v = conf_read (t);
		simlog (SLC_DEBUG, "%s = %d", t, v);
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
		v = tconst ();
		sim_switch_depress (v);
	}
}


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
		fgets (buf, 255, in);
		if (feof (in))
			break;
		exec_script (buf);
	}
	simlog (SLC_DEBUG, "Closing '%s'", filename);
	fclose (in);
}

