#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "native/gpio.h"

#define CONFIG_BEAGLEBONE

#ifdef CONFIG_BEAGLEBONE

#define MAX_GPIOS 128
#define MAX_LEDS 4

char gpio_file_name[128];

struct gpio
{
	const char *mux;
	FILE *value_fd;
	unsigned short connector;
	unsigned short pin;
};

#define P8(p) .connector = 8, .pin = p
#define P9(p) .connector = 9, .pin = p

struct gpio gpio_table[MAX_GPIOS] =
{
	[GPID(1,6)] = { .mux = "gpmc_ad6", P8(3) },
	[GPID(1,7)] = { .mux = "gpmc_ad7", P8(4) },
	[GPID(1,2)] = { .mux = "gpmc_ad2", P8(5) },
	[GPID(1,3)] = { .mux = "gpmc_ad3", P8(6) },
	[GPID(1,13)] = { .mux = "gpmc_ad13", P8(11) },
	[GPID(1,12)] = { .mux = "gpmc_ad12", P8(12) },
	[GPID(0,23)] = { .mux = "gpmc_ad9", P8(13) },
	[GPID(0,26)] = { .mux = "gpmc_ad10", P8(14) },
	[GPID(1,15)] = { .mux = "gpmc_ad15", P8(15) },
	[GPID(1,14)] = { .mux = "gpmc_ad14", P8(16) },
	[GPID(0,27)] = { .mux = "gpmc_ad11", P8(17) },
	[GPID(2,1)] = { .mux = "gpmc_clk", P8(18) },
	[GPID(1,31)] = { .mux = "gpmc_csn2", P8(20) },
	[GPID(1,30)] = { .mux = "gpmc_csn1", P8(21) },
	[GPID(1,5)] = { .mux = "gpmc_ad5", P8(22) },
	[GPID(1,4)] = { .mux = "gpmc_ad4", P8(23) },
	[GPID(1,1)] = { .mux = "gpmc_ad1", P8(24) },
	[GPID(1,0)] = { .mux = "gpmc_ad0", P8(25) },
	[GPID(1,29)] = { .mux = "gpmc_csn0", P8(26) },
	[GPID(2,22)] = { .mux = "lcd_vsync", P8(27) },
	[GPID(2,24)] = { .mux = "lcd_pclk", P8(28) },
	[GPID(2,23)] = { .mux = "lcd_hsync", P8(29) },
	[GPID(2,25)] = { .mux = "lcd_ac_bias_en", P8(30) },
	[GPID(2,12)] = { .mux = "lcd_data6", P8(39) },
	[GPID(2,13)] = { .mux = "lcd_data7", P8(40) },
	[GPID(2,10)] = { .mux = "lcd_data4", P8(41) },
	[GPID(2,11)] = { .mux = "lcd_data5", P8(42) },
	[GPID(2,8)] = { .mux = "lcd_data2", P8(43) },
	[GPID(2,9)] = { .mux = "lcd_data3", P8(44) },
	[GPID(2,6)] = { .mux = "lcd_data0", P8(45) },
	[GPID(2,7)] = { .mux = "lcd_data1", P8(46) },
	[GPID(1,28)] = { .mux = "gpmc_ben1", P9(12) },
	[GPID(1,16)] = { .mux = "mii1_rxd3", P9(15) },
	[GPID(1,17)] = { .mux = "gpmc_a1", P9(23) },
	[GPID(3,21)] = { .mux = "mcasp0_ahclkx", P9(25) },
	[GPID(3,19)] = { .mux = "mcasp0_fsr", P9(27) },
	[GPID(0,7)] = { .mux = "", P9(42) },
};

#endif /* CONFIG_BEAGLEBONE */


FILE *led_files[MAX_LEDS];

static FILE *gpio_data_file (gpio_id_t gpio)
{
	struct gpio *gp = gpio_table + gpio;
	if (gp->value_fd == 0)
	{
		sprintf (gpio_file_name, "/sys/class/gpio/gpio%d/value", gpio);
		gp->value_fd = fopen (gpio_file_name, "w");
	}
	return gp->value_fd;
}

static FILE *led_data_file (int led)
{
	if (led_files[led] == NULL)
	{
		sprintf (gpio_file_name,
		   "/sys/class/leds/beaglebone::usr%d/brightness", led);
		led_files[led] = fopen (gpio_file_name, "w");
	}
	return led_files[led];
}

int gpio_request (gpio_id_t gpio)
{
	FILE *fp = fopen ("/sys/class/gpio/export", "w");
	if (fp == NULL)
		return -1;
	fprintf (fp, "%d", gpio);
	fclose (fp);
	return 0;
}

int gpio_config (gpio_id_t gpio)
{
	struct gpio *gp;
	if (gpio >= MAX_GPIOS)
		return -1;
	gp = gpio_table + gpio;
	if (!gp->mux)
		return -1;
	sprintf (gpio_file_name, "/sys/kernel/debug/omap_mux/%s", gp->mux);
	FILE *fp = fopen (gpio_file_name, "w");
	if (!fp)
		return -1;
	fprintf (fp, "7");
	fclose (fp);
	return 0;
}

int gpio_set_direction (gpio_id_t gpio, const char *direction)
{
	sprintf (gpio_file_name, "/sys/class/gpio/gpio%d/direction", gpio);
	FILE *fp = fopen (gpio_file_name, "w");
	if (!fp)
		return -1;
	fprintf (fp, "%s", direction);
	fclose (fp);
	return 0;
}

int gpio_read (gpio_id_t gpio)
{
	FILE *fp = gpio_data_file (gpio);
	return 0;
}

int gpio_write (gpio_id_t gpio, int value)
{
	FILE *fp = gpio_data_file (gpio);
	if (fp == NULL)
		return -1;
	fputc (value + '0', fp);
	fputc ('\n', fp);
	fflush (fp);
	return 0;
}

int gpio_write_led (int led, int value)
{
	FILE *fp = led_data_file (led);
	if (fp == NULL)
		return -1;
	fputc (value + '0', fp);
	fputc ('\n', fp);
	fflush (fp);
	return 0;
}

int gpio_release (gpio_id_t gpio)
{
	FILE *fp = fopen ("/sys/class/gpio/unexport", "w");
	if (fp == NULL)
		return -1;
	fprintf (fp, "%d", gpio);
	fclose (fp);
	return 0;
}

int gpio_request_input (gpio_id_t gpio)
{
	int rc;
	if ((rc = gpio_request (gpio)) < 0)
		return rc;
	if ((rc = gpio_config (gpio)) < 0)
		return rc;
	return gpio_set_direction (gpio, "in");
}

int gpio_request_output (gpio_id_t gpio)
{
	int rc;
	if ((rc = gpio_request (gpio)) < 0)
		return rc;
	if ((rc = gpio_config (gpio)) < 0)
		return rc;
	return gpio_set_direction (gpio, "out");
}

#ifdef TESTME
void sigterm_handler (int signum)
{
	gpio_release (71);
}

int main (void)
{
	int c;
	gpio_id_t g;
	int i;

	signal (SIGTERM, sigterm_handler);
	signal (SIGILL, sigterm_handler);
	signal (SIGKILL, sigterm_handler);
	signal (SIGSEGV, sigterm_handler);

	for (g=0; g < MAX_GPIOS; g = (g+1) % MAX_GPIOS)
	{
		struct gpio *gp = gpio_table + g;

		if (gp->mux == NULL || gp->connector == 0 || gp->connector == 0)
			continue;
		printf ("GPIO %d", g);
		printf ("   P%d-%d\n", gp->connector, gp->pin);

		if (gpio_request_output (g) < 0)
		{
			printf ("error: could not request GPIO\n");
		}

repeat: for (i=0; i < 5; i++)
		{
			if (gpio_write (g, 1) < 0)
			{
				printf ("error: could not write GPIO\n");
				break;
			}
			usleep (500000);
			gpio_write (g, 0);
			usleep (500000);
		}

what_now:
		printf ("(r)epeat, (n)ext");
		c = getchar ();
		if (c == 'r')
			goto repeat;
		else if (c == 'n')
			continue;
		else
			goto what_now;
	}
}
#endif /* TESTME */
