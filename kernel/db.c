
#include <freewpc.h>
#include <m6809_math.h>

/* Initially zero, this is set to 1 when a remote debugger is
 * detected */
static uint8_t db_attached;


#ifdef DEBUGGER

void db_puts (const char *s)
{
	if (db_attached)
		while (*s)
			db_putc (*s++);
}

void db_puti (uint8_t v)
{
	uint8_t quot;
	uint8_t rem;

	DIV10 (v, quot, rem);

	db_putc (quot + '0');
	db_putc (rem + '0');
}


static char db_get_nibble_char (uint8_t v)
{
	if (v > 9)
		return v - 10 + 'A';
	else
		return v + '0';
}


void db_put2x (uint8_t v)
{
	db_putc (db_get_nibble_char (v >> 4));
	db_putc (db_get_nibble_char (v & 0x0F));
}

void db_put4x (uint16_t v)
{
	db_put2x (v >> 8);
	db_put2x (v & 0xFF);
}


#if 0 // doesn't compile anymore for some reason
void db_dump_switches (void)
{
	uint8_t row, col;

	for (row=1; row <= 8; row++)
	{
		for (col=1; col <= 8; col++)
		{
			int level = switch_poll_logical (MAKE_SW(col,row));
			db_putc (level ? '+' : '-');
		}
		db_putc ('\n');
	}
}
#endif

#endif /* DEBUGGER */

void db_rtt (void)
{
	if (!db_attached)
	{
		if (db_read_ready)
		{
			db_putc (0);
			db_attached = 1;
		}
	}
	else
	{
		if (db_read_ready)
		{
			char c = db_data_reg;
			switch (c)
			{
				case 't':
				{
					extern void task_dump (void);
					task_dump ();
					break;
				}

				case 'g':
				{
					extern void dump_game (void);
					dump_game ();
					break;
				}
			}
		}
	}
}

void db_init (void)
{
	db_attached = 0;
	db_rtt ();
}

