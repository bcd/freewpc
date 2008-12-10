
#include <stdio.h>

#define MAX_SIGNALS 16
#define MAX_WAVE_LEN 60

enum value_range {
	INACTIVE,
	ACTIVE,
	RISING_EDGE,
	FALLING_EDGE,
	INDETERMINATE,
};

struct signal_info
{
	const char *name;
	enum value_range val;
	unsigned long last_clock;
	char wave[MAX_WAVE_LEN];
};

struct signal_info signal_table[MAX_SIGNALS];
unsigned int signal_longest_name;
unsigned long clock_min;
unsigned long clock_max;
unsigned int clock_width;

void set_window (unsigned int min, unsigned int max)
{
	clock_min = min;
	clock_max = max;
	clock_width = (max - min) / MAX_WAVE_LEN;
}

static enum value_range update_val (enum value_range old, enum value_range new)
{
	if (old == new)
		return old;
	else if ((old == INACTIVE || old == RISING_EDGE) && new == ACTIVE)
		return RISING_EDGE;
	else if ((old == ACTIVE || old == FALLING_EDGE) && new == INACTIVE)
		return FALLING_EDGE;
	else
		return INDETERMINATE;
}

char wave_char (enum value_range val)
{
	switch (val)
	{
		case INACTIVE:
			return '_';
		case ACTIVE:
			return '-';
		case RISING_EDGE:
			return '/';
		case FALLING_EDGE:
			return '\\';
		default:
		case INDETERMINATE:
			return '*';
	}
}


void add_signal (const char *name)
{
	int i;
	struct signal_info *sig;
	for (i=0, sig = signal_table ; i < MAX_SIGNALS; i++, sig++)
	{
		if (!sig->name)
		{
			sig->name = name;
			sig->val = INACTIVE;
			sig->last_clock = clock_min;
			memset (sig->wave, 'X', MAX_WAVE_LEN);
			sig->wave[0] = wave_char (sig->val);
			return;
		}
	}
}

void update_signal (const char *name, unsigned long clock, unsigned int val)
{
	int i;
	struct signal_info *sig;

	for (i=0, sig = signal_table ; i < MAX_SIGNALS; i++, sig++)
	{
		if (!strcmp (sig->name, name))
		{
			unsigned long clk;
			for (clk = sig->last_clock; clk <= clock; clk++)
			{
				if (clk < clock_min || clk >= clock_max)
					continue;

				if (clk < clock)
					continue;

				sig->val = update_val (sig->val, val);
				val = sig->val;

				if (clk / clock_width > sig->last_clock / clock_width)
				{
					unsigned int offset = (clk - clock_min) / clock_width;
					//printf ("signal %d clk %d array %d  :  %d\n", i, clk, offset, sig->val);
					sig->wave[offset] = wave_char (sig->val);

					if (sig->val == RISING_EDGE)
						sig->val = ACTIVE;
					else if (sig->val == FALLING_EDGE)
						sig->val = INACTIVE;

					sig->last_clock = clk;
					sig->wave[offset+1] = wave_char (sig->val);
				}
			}
			return;
		}
	}
}

void add_clock_signal (const char *name, unsigned long f)
{
	unsigned long clock;

	add_signal (name);

	if (f < clock_min / 2)
		return;

	for (clock = (clock_min / clock_width) * clock_width;
		clock < clock_max;
		clock++)
	{
		update_signal (name, clock, (clock % f) < f/2);
	}
}


void dump_signals (void)
{
	int i, j;
	struct signal_info *sig;
	char c;

	for (i=0, sig = signal_table ; i < MAX_SIGNALS; i++, sig++)
	{
		if (!sig->name)
			continue;
		c = 'X';
		printf ("%-8.8s ", sig->name);
		for (j = 0; j < MAX_WAVE_LEN; j++)
		{
			if (sig->wave[j] != 'X')
				c = sig->wave[j];
			putchar (c);
		}
		putchar ('\n');
		putchar ('\n');
	}
}

int trace_open (const char *filename)
{
	FILE *fp;
	char line[80];
	char *p;

	fp = fopen (filename, "r");

	for (;;)
	{
		fgets (line, 79, fp);
		if (feof (fp))
			break;

		p = line;
		switch (*p++)
		{
		}
	}

	fclose (fp);
	return 0;
}


int main (int argc, char *argv[])
{
	int argn;

	memset (signal_table, 0, sizeof (signal_table));
	signal_longest_name = 0;

	for (argn=1; argn < argc; argn++)
	{
	}

	set_window (0, 20000);
	add_signal ("ZC");
	add_signal ("COIL");
	//add_clock_signal ("IRQ", 10);
	update_signal ("ZC", 100, 1);
	update_signal ("COIL", 200, 1);
	update_signal ("ZC", 300, 0);
	update_signal ("COIL", 600, 0);
	update_signal ("ZC", 900, 1);
	dump_signals ();
	exit (0);
}

