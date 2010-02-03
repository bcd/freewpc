
#include <freewpc.h>

struct shell_context
{
	U8 exec : 1;
	U8 numeric : 1;
	U8 symbolic : 1;
	U8 size : 3;
	const char *text;
	union
	{
		void (*exec) (void);
		struct shell_context *ctx;
	} next;
};

struct shell_context *shell_ctx[8];
U8 shell_cmd[8];
U16 shell_cursor;


void shell_dmd_update (void)
{
	U16 pos;
	for (pos = 0; pos <= shell_cursor; pos++)
	{
		struct shell_context *ctx = shell_ctx[pos];
		U8 val = shell_cmd[pos];
	}
}


void shell_button_finish (void)
{
	while (sw_logical[0] != 0)
	{
		task_runs_long ();
	}
}


void debug_shell (void)
{
	shell_cursor = 0;
	for (;;)
	{
		if (switch_poll (SW_ESCAPE))
		{
			if (shell_cursor == shell_cmd)
			{
			}
			else
			{
				--shell_cursor;
			}
			shell_button_finish ();
		}
		else if (switch_poll (SW_UP))
		{
			shell_cmd[shell_cursor]++;
			shell_button_finish ();
		}
		else if (switch_poll (SW_DOWN))
		{
			shell_cmd[shell_cursor]--;
			shell_button_finish ();
		}
		else if (switch_poll (SW_ENTER))
		{
			if (shell_ctx[shell_cursor].exec)
			{
			}
			else
			{
				shell_cursor++;
			}
			shell_button_finish ();
		}
		else
		{
			switch_idle ();
			task_runs_long ();
		}
	}
}

