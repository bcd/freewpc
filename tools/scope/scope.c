
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char U8;

#define MAX_IO_LINES 64

#define IO_LOW 0
#define IO_HIGH 1

#define IO_SOL(n)    (n)        /* 0-47 reserved for solenoids */
#define IO_GI(n)     (48+(n))   /* 48-52 for GI */
#define IO_ZC        53         /* 53 is zerocross */
#define IO_IRQ       54         /* 54 : IRQ asserted by CPU */


FILE *fp;

struct io_line
{
	U8 id : 6;
	U8 state : 1;
	U8 reserved : 1;
};

struct io_record
{
	struct io_line line;
	unsigned long when;
} __attribute__((packed));

struct io_line io_line[MAX_IO_LINES];

char *io_symbol[MAX_IO_LINES] = {
	"SOL0", "SOL1", "SOL2", "SOL3", "SOL4", "SOL5", "SOL6", "SOL7",
	[IO_ZC] = "ZC",
	[IO_GI(0)] = "GI0",
	[IO_GI(1)] = "GI1",
	[IO_GI(2)] = "GI2",
	[IO_GI(3)] = "GI3",
	[IO_GI(4)] = "GI4",
};
//"01234567ABCDEFGHabcdefgh01234567ABCDEFGHabcdefghLMNOPXI";

unsigned int io_monitor_count = 0;
unsigned int io_monitor_ids[8];

void io_line_set (unsigned long now, U8 id, U8 state)
{
	if (state != io_line[id].state)
	{
		io_line[id].state = state;
		fwrite (&io_line[id], sizeof (U8), 1, fp);
		fwrite (&now, sizeof (now), 1, fp);
	}
}

void io_line_set_multiple (unsigned long now, U8 base_id, U8 states)
{
	unsigned int n;
	for (n = 0 ; n < 8; n++)
		io_line_set (now, base_id+n, (states & (1 << n)) ? 1 : 0);
}

void io_record_print (unsigned long when)
{
	unsigned int n;
	unsigned long ms = when / 2000;
	unsigned int ns = (when % 2000) / 2;

	printf ("%6ld (%8ld.%03d ms) : ", when % 100000, ms, ns);
	for (n = 0; n < io_monitor_count; n++)
	{
		char *symbol = io_symbol[io_monitor_ids[n]];
		static const char *spaces = "...............";

		if (io_line[io_monitor_ids[n]].state == IO_HIGH)
			printf ("%s", symbol);
		else
			printf ("%-*.*s", strlen (symbol), strlen (symbol), spaces);
		putchar ( ' ' );
	}
	putchar ('\n');
}

void io_line_init (void)
{
	unsigned int i;
	for (i=0 ; i < MAX_IO_LINES; i++)
	{
		io_line[i].id = i;
		io_line[i].state = IO_LOW;
	}
}

void io_line_open_for_writing (const char *name)
{
	fp = fopen (name, "w");
}

void io_line_open_for_reading (const char *name)
{
	fp = fopen (name, "r");
}

void io_line_close (void)
{
	fclose (fp);
}


#ifdef STANDALONE

int main (int argc, char *argv[])
{
	unsigned int write_file = 1;
	unsigned int argn = 1;
	char *filename = "test.sco";
	unsigned long min_time = 7500000;
	unsigned long time_width = 1000;

	if (sizeof (struct io_line) != 1)
	{
		fprintf (stderr, "error: io_line has wrong size\n");
		exit (1);
	}

	if (sizeof (struct io_record) != 5)
	{
		fprintf (stderr, "error: io_record has wrong size\n");
		exit (1);
	}

	while  (argn < argc)
	{
		if (argv[argn][0] == '-')
		{
			switch (argv[argn][1])
			{
				case 'r':
					write_file = 0;
					break;
				case 'w':
					write_file = 1;
					break;
				case 'a':
					io_monitor_ids[io_monitor_count++] = 
						strtoul (argv[++argn], NULL, 0);
					break;
				case 'A':
				{
					unsigned int min = strtoul (argv[++argn], NULL, 0);
					unsigned int max = strtoul (argv[++argn], NULL, 0);
					unsigned int n;
					for (n = min; n <= max; n++)
						io_monitor_ids[io_monitor_count++] = n;
					break;
				}
				case 'W':
					time_width = strtoul (argv[++argn], NULL, 0);
					break;	
				case 'f':
					filename = argv[++argn];
					break;
			}
		}
		argn++;
	}

	io_line_init ();
	if (write_file == 1)
	{
		io_line_open_for_writing (filename);
		io_line_set (0, 1, 0);
		io_line_set (4, 1, 1);
		io_line_set (8, 2, 1);
	}
	else
	{
		struct io_record io_record;
		unsigned long last_time = min_time;

		io_line_open_for_reading (filename);
		for (;;)
		{
			fread (&io_record, sizeof (struct io_record), 1, fp);
			if (feof (fp))
				break;

			while (last_time < io_record.when)
			{
				io_record_print (last_time);
				last_time += time_width;
			}
			io_line_set (io_record.when, io_record.line.id, io_record.line.state);
		}
	}
	io_line_close ();

	return 0;
}

#endif /* STANDALONE */
