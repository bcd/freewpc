
#include <stdio.h>
#include <math.h>

/*
 * Notes:
 *
 * - Resistance of the coil varies with its temperature.
 *
 * - FL11753 (yellow coil) - 9.8 ohms power/165 ohms hold
 * - FL11722 (green coil) - 6.2 ohms power/160 ohms hold
 * - FL11630 (red coil) - 4.7 ohms power/160 ohms hold
 * - FL15411 (orange coil) - 4.2 ohms power/145 ohms hold
 * - FL11629 (blue coil) - 4 ohms power/132 ohms hold
 */


/** The maximum AC voltage that is supplied. */
double max_voltage = 50.0;

/** The maximum root-mean-square (RMS) voltage.
 * This is derived from max_voltage by multiplying about 0.707. */
double max_rms_voltage;

/** The time step.  The smaller this is, the more accurate the
simulation of the AC waveform is, but also the longer it takes. */
double time_step = 0.1;

/** The frequency of the AC voltage.  For the US this should be 60,
for overseas it is 50. */
unsigned int hz = 60;

double ac_period;

double total_squared_voltage;
unsigned int total_intervals;

double kwh_cost = 0.10;

/**
 * Pulse a coil.
 *
 * ANGLE and DUTY_COUNT provide the state of the system.  ANGLE
 * says where the AC waveform will be at the next IRQ.  It
 * ranges from 0 to (2 * M_PI).  DUTY_COUNT says where the software
 * duty cycle counter is.  It ranges from 0 to 7.
 *
 * DURATION gives the length of the solenoid pulse, from the point
 * of view of the software.  DUTY_MASK limits the on-time to certain
 * intervals.
 */
double pulse (	double angle,
					unsigned int global_duty,
					unsigned int duration,
					unsigned int coil_duty)
{
	double timer;
	double next_irq_time = 0.0;
	double voltage;
	const double cpu_update_interval = 4.0;

	total_squared_voltage = 0.0;
	total_intervals = 0;

	timer = 0.0;
	while (duration)
	{
		/* Simulate an IRQ */
		if (timer >= next_irq_time)
		{
			/* Shift the global duty mask */
			global_duty <<= 1;
			if (global_duty > 0xff)
				global_duty = 0x1;

			if (duration)
				duration--;

			next_irq_time += cpu_update_interval;
		}

		/* Recalculate present voltage */
		if (duration && (global_duty & coil_duty))
		{
			voltage = abs(max_voltage * sin(angle));
		}
		else
		{
			voltage = 0.0;
		}

		/* Update the coil state */
#if 0
		printf ("duration: %d  angle: %g, sin(angle): %g  cpu_clk: %g   "
		        "   out: %g\n",
			duration, angle, sin(angle), timer, voltage);
#endif

		/* Update RMS calculation */
		total_intervals++;
		total_squared_voltage += (voltage * voltage);

		/* Update AC phase angle for next iteration */
		angle += (time_step / ac_period) * (2.0 * M_PI);
		timer += time_step;
	}

	voltage = sqrt (total_squared_voltage / total_intervals);
	return voltage;
}


double average_power (double rms_voltage, double resistance)
{
	return (rms_voltage * rms_voltage / resistance);
}

double joules_to_kwh (double joules)
{
	return joules * 2.7778E-7;
}


int main (int argc, char *argv[])
{
	double angle;
	unsigned int strength = 0;
	unsigned int duty = 0x22;
	unsigned int duration = 133;
	unsigned int global_duty;
	double min_v = max_voltage, max_v = 0.0;
	double min_power = 9999.0, max_power = 0.0;
	double min_work, avg_work, max_work;
	double min_current, max_current;
	double resistance = 5.0;
	int argn;

	/* Parse command-line options. */
	for (argn = 1; argn < argc; argn++)
	{
		if (argv[argn][0] == '-')
		{
			switch (argv[argn][1])
			{
				case 'h':
					printf (
					   "\nPulse Options:\n"
						"-s <strength>         Set coil strength as a percentage (1-100)\n"
						"-m <mask>             Set coil strength as a bitmask (01 - FF, default: %02X)\n"
						"-t <time>             Set coil on time in milliseconds (default: %d)\n"
						"-v <volts>            Set supply voltage in watts (default: %2f)\n"
						"-r <ohms>             Set coil resistance (default: 5 Ohms)\n"
						"\nSimulation Options:\n"
						"-i <step>             Set simulation time step (default: 0.1 ms)\n"
						"-E                    Use European 50Hz instead of US 60Hz\n"
						"\nOutput Options:\n",
						duty, duration, max_voltage
					);
					exit (0);
					break;
				case 's':
					strength = strtoul (argv[++argn], NULL, 0);
					break;
				case 'm':
					duty = strtoul (argv[++argn], NULL, 16);
					strength = 0;
					break;
				case 't':
					duration = strtoul (argv[++argn], NULL, 0);
					break;
				case 'v':
					max_voltage = strtoul (argv[++argn], NULL, 0);
					break;
				case 'i':
					time_step = strtod (argv[++argn], NULL);
					break;
				case 'E':
					hz = 50;
					break;
				case 'r':
					resistance = strtoul (argv[++argn], NULL, 0);
					break;
			}
		}
	}

	/* If a strength is given, convert that to a duty cycle mask. */
	switch (strength)
	{
		case 100:
			duty = 0xFF;
			break;
		case 87:	case 88:
			duty = 0xFE;
			break;
		case 75:
			duty = 0x77;
			break;
		case 62: case 63:
			duty = 0xB5;
			break;
		case 50:	
			duty = 0x55;
			break;
		case 37:
			duty = 0x92;
			break;
		case 25:
			duty = 0x22;
			break;
		case 12: case 13:
			duty = 0x40;
			break;
		default:	
			fprintf (stderr, "error: invalid strength percentage\n");
			exit (1);
	}

	/* Initialize other constants for this run, based on options. */
	max_rms_voltage = max_voltage * (sqrt(2) / 2.0);
	ac_period = (1000.0 / hz);

	/* Run the simulation. */
	for (angle = 0; angle < 2 * M_PI; angle += 0.2)
	{
		for (global_duty = 0x1; global_duty <= 0x80; global_duty <<= 1)
		{
			double v_rms;
			double power;

			/* Calculate the RMS voltage for these starting conditions. */
			v_rms = pulse (angle, global_duty, duration, duty);
			if (v_rms < min_v)
				min_v = v_rms;
			if (v_rms > max_v)
				max_v = v_rms;
#if 0
			printf ("%1.2f %02X %2.3f %01.3f\n",
				angle, global_duty, v_rms, v_rms / max_voltage);
#endif

			/* Calculate power consumed during this interval */
			power = average_power (v_rms, resistance);
			if (power < min_power)
				min_power = power;
			if (power > max_power)
				max_power = power;
		}
	}
	
	min_work = min_power * duration;
	max_work = max_power * duration;
	min_current = min_v / resistance;
	max_current = max_v / resistance;

	printf ("Voltage : %2.3f - %2.3f V (%1.02f - %1.02f %%)\n",
		min_v, max_v, 100 * min_v / max_rms_voltage, 100 * max_v / max_rms_voltage);
	printf ("Current : %2.3f - %2.3f A\n", min_current, max_current);
	printf ("Power   : %3.3f - %3.3f W\n", min_power, max_power);
	printf ("Work    : %3.3f - %3.3f J\n", min_work, max_work);

	avg_work = joules_to_kwh ((min_work + max_work) / 2.0);
	printf ("kWh(avg): %g\n", avg_work);
	printf ("Cost($) : %g\n", avg_work * kwh_cost);
	printf ("Pulses per $ : %d\n", (unsigned int)(1.0 / (avg_work * kwh_cost)));
	exit (0);
}

