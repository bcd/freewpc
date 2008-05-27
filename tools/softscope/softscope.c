
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WPC_BASE_ADDR 0x3FB0

#define WPC_PARALLEL_STATUS_PORT 	0x3FC0
#define WPC_PARALLEL_DATA_PORT 		0x3FC1
#define WPC_PARALLEL_STROBE_PORT 	0x3FC2
#define WPC_SERIAL_DATA_OUTPUT 		0x3FC3
#define WPC_SERIAL_CONTROL_OUTPUT	0x3FC4
#define WPC_SERIAL_BAUD_SELECT 		0x3FC5
#define WPC_TICKET_DISPENSE 			0x3FC6

#define WPC_DCS_SOUND_DATA_OUT 		0x3FD0
#define WPC_DCS_SOUND_DATA_IN 		0x3FD1
#define WPC_DCS_SOUND_RESET 			0x3FD2
#define WPC_DCS_SOUND_DATA_READY 	0x3FD3
#define WPC_FLIPTRONIC_PORT_A 		0x3FD4
#define WPC_FLIPTRONIC_PORT_B 		0x3FD5
#define WPCS_DATA 						0x3FDC
#define WPCS_CONTROL_STATUS 			0x3FDD
#define WPC_SOL_FLASH2_OUTPUT 		0x3FE0
#define WPC_SOL_HIGHPOWER_OUTPUT 	0x3FE1
#define WPC_SOL_FLASH1_OUTPUT 		0x3FE2
#define WPC_SOL_LOWPOWER_OUTPUT 		0x3FE3
#define WPC_LAMP_ROW_OUTPUT 			0x3FE4
#define WPC_LAMP_COL_STROBE 			0x3FE5
#define WPC_GI_TRIAC 					0x3FE6
#define WPC_SW_JUMPER_INPUT 			0x3FE7
#define WPC_SW_CABINET_INPUT 			0x3FE8

#define WPC_SW_ROW_INPUT 				0x3FE9
#define WPC_SW_COL_STROBE 				0x3FEA
#if (MACHINE_PIC == 1)
#define WPCS_PIC_READ 					0x3FE9
#define WPCS_PIC_WRITE 					0x3FEA
#endif

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_POS 					0x3FEB
#define WPC_ALPHA_ROW1 					0x3FEC
#else
#define WPC_EXTBOARD1 					0x3FEB
#define WPC_EXTBOARD2 					0x3FEC
#define WPC_EXTBOARD3 					0x3FED
#endif

#if (MACHINE_WPC95 == 1)
#define WPC95_FLIPPER_COIL_OUTPUT 	0x3FEE
#define WPC95_FLIPPER_SWITCH_INPUT 	0x3FEF
#else
#endif

#if (MACHINE_DMD == 0)
#define WPC_ALPHA_ROW2 					0x3FEE
#else
#endif

#define WPC_LEDS 							0x3FF2
#define WPC_RAM_BANK 					0x3FF3
#define WPC_SHIFTADDR 					0x3FF4
#define WPC_SHIFTBIT 					0x3FF6
#define WPC_SHIFTBIT2 					0x3FF7
#define WPC_PERIPHERAL_TIMER_FIRQ_CLEAR 0x3FF8
#define WPC_ROM_LOCK 					0x3FF9
#define WPC_CLK_HOURS_DAYS 			0x3FFA
#define WPC_CLK_MINS 					0x3FFB
#define WPC_ROM_BANK 					0x3FFC
#define WPC_RAM_LOCK 					0x3FFD
#define WPC_RAM_LOCKSIZE 				0x3FFE
#define WPC_ZEROCROSS_IRQ_CLEAR 		0x3FFF


#define ADDR_INPUT 0
#define ADDR_OUTPUT 1
#define NUM_DIRS 2

#define IO_COUNT 0x50

#define IO_ADDR(x)  ((x) - WPC_BASE_ADDR)

struct io_signal
{
	unsigned int valid : 1;
	unsigned int other_rw : 1;
	unsigned int other_addr : 1;
	unsigned int other_bit : 1;
	unsigned int length : 8;
	unsigned int base_addr : 8;
	const char *name;
};

#define IO_PIN_DECL(_highbit,_lowbit,_name) \
	[_highbit] = { .valid = 1, .name = _name, .length = _highbit - _lowbit + 1 }

#define OUT_BYTE_DECL(_cpuaddr,_name) \
	[IO_ADDR(_cpuaddr)] = { [ADDR_OUTPUT] = { IO_PIN_DECL (7, 0, _name) } }


struct io_signal io_info_table[IO_COUNT][2][8] = {
	OUT_BYTE_DECL (WPC_PARALLEL_DATA_PORT, "PAR_DATA"),
	OUT_BYTE_DECL (WPC_DCS_SOUND_DATA_OUT, "DCS_OUT"),
	OUT_BYTE_DECL (WPC_DCS_SOUND_DATA_IN, "DCS_IN"),

	[IO_ADDR(WPC_SOL_HIGHPOWER_OUTPUT)] = {
		[ADDR_OUTPUT] = { 
			IO_PIN_DECL (7, 0, "SOL_HI"),
			IO_PIN_DECL (4, 4, "GI4"),
			IO_PIN_DECL (3, 3, "GI3"),
			IO_PIN_DECL (2, 2, "GI2"),
			IO_PIN_DECL (1, 1, "GI1"),
			IO_PIN_DECL (0, 0, "GI0"),
		},
	},


	[IO_ADDR(WPC_GI_TRIAC)] = {
		[ADDR_OUTPUT] = { 
			IO_PIN_DECL (7, 7, "FLEN"),
			IO_PIN_DECL (4, 4, "GI4"),
			IO_PIN_DECL (3, 3, "GI3"),
			IO_PIN_DECL (2, 2, "GI2"),
			IO_PIN_DECL (1, 1, "GI1"),
			IO_PIN_DECL (0, 0, "GI0"),
		},
	},

	[IO_ADDR(WPC_LEDS)] = {
		[ADDR_OUTPUT] = { 
			IO_PIN_DECL (7, 7, "LED_DIAG"),
		},
	},

	[IO_ADDR(WPC_PERIPHERAL_TIMER_FIRQ_CLEAR)] = {
		[ADDR_OUTPUT] = { 
			IO_PIN_DECL (7, 7, "FIRQ_CLR"),
		},
		[ADDR_INPUT] = {
			IO_PIN_DECL (7, 0, "TMRVAL"),
		},
	},

	OUT_BYTE_DECL (WPC_ROM_LOCK, "ROMLOCK"),
	OUT_BYTE_DECL (WPC_RAM_LOCK, "RAMLOCK"),
	OUT_BYTE_DECL (WPC_RAM_LOCKSIZE, "RAMBANKSZ"),

	[IO_ADDR(WPC_ZEROCROSS_IRQ_CLEAR)] = {
		[ADDR_OUTPUT] = { 
			IO_PIN_DECL (7, 7, "IRQ_CLR"),
		},
		[ADDR_INPUT] = {
			IO_PIN_DECL (7, 7, "ZEROCROSS"),
		},
	},
};

const char *get_signal_name (unsigned int offset, unsigned char bitmask)
{
	struct io_signal *ios;
	int firstbit;

	static char result[64];

	if (bitmask & 0x80) firstbit = 7;
	else if (bitmask & 0x40) firstbit = 6;
	else if (bitmask & 0x20) firstbit = 5;
	else if (bitmask & 0x10) firstbit = 4;
	else if (bitmask & 0x8) firstbit = 3;
	else if (bitmask & 0x4) firstbit = 2;
	else if (bitmask & 0x2) firstbit = 1;
	else firstbit = 0;

	ios = &io_info_table[offset / 2][offset % 2][firstbit];
	if (ios->valid)
		goto finish;

	if (ios->other_rw)
	{
		ios = &io_info_table[offset / 2][1 - offset % 2][firstbit];
		goto finish;
	}

	sprintf (result, "???");
	return result;

finish:
	if (ios->length > 1)
		sprintf (result, "%s[%d:%d]", ios->name, firstbit, firstbit - ios->length + 1);
	else
		sprintf (result, "%s", ios->name);
	return result;
}


void print_time (unsigned long t)
{
	const unsigned int cycles_per_ms = 2000;
	unsigned long ms = t / cycles_per_ms;
	unsigned int frac = (t % cycles_per_ms) / 2;
	printf ("%-8.8ld.%03d", ms, frac);
}



unsigned char io_state[IO_COUNT*2];
unsigned char io_masked[IO_COUNT*2];
int fd;
unsigned long when_last = 0;



unsigned long read_marker_and_time (int fd, int *setp, int *errorp)
{
	unsigned char v_byte;
	unsigned short v_short;
	unsigned long v_long;

	*errorp = *setp = 0;
read_long:
	*errorp = read (fd, &v_byte, sizeof (v_byte));
	if (*errorp <= 0)
		return 0;

	switch (v_byte)
	{
		case 0xf8:
			*setp = 1;
			goto read_long;
		case 0xfe:
			*errorp = read (fd, &v_short, sizeof (v_short));
			return v_short;	
		case 0xff:
			*errorp = read (fd, &v_long, sizeof (v_long));
			return v_long;	
		default:
			return v_byte;
	}
}


int write_long (int fd, unsigned long v)
{
	unsigned char v_byte;
	unsigned short v_short;

	if (v < 0xF8)
	{
		v_byte = v;
		write (fd, &v_byte, sizeof (v_byte));
	}
	else if (v < 0xFFFF)
	{
		v_byte = 0xFE;
		write (fd, &v_byte, sizeof (v_byte));
		v_short = v;
		write (fd, &v_short, sizeof (v_short));
	}
	else
	{
		v_byte = 0xFF;
		write (fd, &v_byte, sizeof (v_byte));
		write (fd, &v, sizeof (v));
	}
	return 0;
}


void write_log_state (unsigned long when, unsigned char offset, unsigned char state)
{
	unsigned long since_last = when - when_last;
	unsigned char marker = 0xF8;
	int rc;

	rc = write (fd, &marker, sizeof (marker));
	rc = write_long (fd, since_last);
	rc =write (fd, &offset, sizeof (offset));
	rc = write (fd, &state, sizeof (state));
	when_last = when;
	io_state[offset] = state;
}


void write_log_delta (unsigned long when, unsigned char offset, unsigned char delta)
{
	unsigned long since_last = when - when_last;
	int rc;
	
	rc = write_long (fd, since_last);
	rc = write (fd, &offset, sizeof (offset));
	rc = write (fd, &delta, sizeof (delta));
	when_last = when;
}


void latch_byte (unsigned long when, unsigned char offset, unsigned char value)
{
	unsigned char delta;

	/* Only pay attention to the bits we care about */
	//value &= io_masked[offset];

	/* See if any bits changed */
	delta = value ^ io_state[offset];
	if (delta)
		write_log_delta (when, offset, delta);
	io_state[offset] = value;
}


void latch_input (unsigned long when, unsigned char addr, unsigned char value)
{
	latch_byte (when, addr * NUM_DIRS + ADDR_INPUT, value);
}

void latch_output (unsigned long when, unsigned char addr, unsigned char value)
{
	latch_byte (when, addr * NUM_DIRS + ADDR_OUTPUT, value);
}

void init_input (unsigned long when, unsigned char addr, unsigned char value)
{
	write_log_state (when, addr * NUM_DIRS + ADDR_INPUT, value);
}

void init_output (unsigned long when, unsigned char addr, unsigned char value)
{
	write_log_state (when, addr * NUM_DIRS + ADDR_OUTPUT, value);
}



void log_open_for_writing (unsigned long now)
{
	when_last = 0;
	fd = open ("pinmame.sco", O_CREAT | O_TRUNC | O_WRONLY, 0666);

	memset (io_state, 0x0, sizeof (io_state));
	memset (io_masked, 0xFF, sizeof (io_masked));
}


void log_open_for_reading (void)
{
	when_last = 0;
	fd = open ("pinmame.sco", O_RDONLY, 0666);
	if (fd < 0)
	{
		fprintf (stderr, "error: could not open logfile for reading\n");
	}
}


void log_close (void)
{
	close (fd);
}


void write_test (void)
{
	unsigned long now = 0x12345;

	log_open_for_writing (now);

	now += 50;
	latch_output (now, IO_ADDR(WPC_RAM_LOCKSIZE), 0x00);
	now += 10;
	latch_output (now, IO_ADDR(WPC_ZEROCROSS_IRQ_CLEAR), 0x00);

	log_close ();
}


unsigned long min_time = 0;
unsigned long max_time = 1000000;

void parse_data_file (void)
{
	unsigned long event_time = 0;
	unsigned char event_addr;
	unsigned char event_data;
	int setflag;
	int errorflag;

	memset (io_state, 0, sizeof (io_state));
	log_open_for_reading ();

	for (;;)
	{
		/* Read a record */
		event_time += read_marker_and_time (fd, &setflag, &errorflag);
		if (errorflag <= 0)
			break;
		read (fd, &event_addr, sizeof (event_addr));
		read (fd, &event_data, sizeof (event_data));

		if (setflag == 1)
		{
			event_data = io_state[event_addr];
			io_state[event_addr] = event_data;
		}
		else
		{
			io_state[event_addr] ^= event_data;
			event_data = io_state[event_addr];
		}

		/* Skip records that we don't want to print */
		if (event_time < min_time)
			continue;
		if (event_time > max_time)
			continue;

		/* Print the record */
		print_time (event_time);
		printf (" : %04X %s %02X\n", 
			//get_signal_name (event_addr, 0xff),
			event_addr / 2 + WPC_BASE_ADDR,
			(event_addr % 2) ? "W" : "R",
			event_data);
	}

	log_close ();
}


#ifdef STANDALONE
int main (int argc, char *argv[])
{
	//write_test ();
	parse_data_file ();
	exit (0);
}
#endif
