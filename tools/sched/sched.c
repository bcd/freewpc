
#include <stdio.h>
#include <string.h>


#define MAX_TICKS 32
#define MAX_SLOTS_PER_TICK 16
#define MAX_TASKS 64

struct task
{
	const char *name;
	unsigned int period;
	double len;
};

struct slot
{
	unsigned int divider;
	struct task *task;
};

struct tick
{
	unsigned int n_slots;
	struct slot slots[MAX_SLOTS_PER_TICK];
	double len;
};


/* The master scheduling table */
unsigned int n_ticks;
struct tick ticks[MAX_TICKS];

unsigned int n_tasks;
struct task tasks[MAX_TASKS];

unsigned int max_ticks = 8;

unsigned int max_divider = 1;


unsigned long gcd (unsigned long a, unsigned long b)
{
	if (b == 0)
		return a;
	else
		return gcd (b, a % b);
}


unsigned long lcm (unsigned long a, unsigned long b)
{
	return (a  / gcd (a, b)) * b;
}


void write_schedule (FILE *f)
{
	unsigned int n;

	for (n=0; n < n_ticks; n++)
	{
		struct tick *tick = &ticks[n];
		unsigned int slotno;

		fprintf (f, "Tick %d - %f :\n", n, tick->len);
		for (slotno = 0; slotno < tick->n_slots; slotno++)
		{
			struct slot *slot = &tick->slots[slotno];
			fprintf (f, "   %s - %f(/%d)\n", slot->task->name, slot->task->len, slot->divider);
		}
	}
}

void write_tick_driver (void)
{
}


void init_schedule (void)
{
	n_ticks = 0;
	n_tasks = 0;
}


void expand_ticks (unsigned int new_tick_count)
{
	unsigned int tickno;

	n_ticks = 4;
	for (tickno = 0; tickno < n_ticks ; tickno++)
	{
		ticks[tickno].n_slots = 0;
		ticks[tickno].len = 0.0;
	}
}


unsigned int find_best_tick (unsigned int period, unsigned int count, double len)
{
	/* Find the best starting bucket to put a task with PERIOD and length LEN.
	COUNT such buckets will have a slot added.
		We search through all buckets and look for the bucket that is 
	least utilized. */

	unsigned int tickno, best = 0;
	double best_len = 99999.0;
	unsigned int index;

	printf ("find_best_tick: period %d count %d len %f\n", period, count, len);
	printf ("checking %d different starting points\n", n_ticks / count);
	printf ("adding %d different ticks for each\n", count);

	for (tickno = 0; tickno < n_ticks / count; tickno++)
	{
		double total_len = 0.0;

		for (index = 0; index < count; index++)
		{
			double candidate_len = ticks[tickno + count * index].len;
			if (candidate_len + len >= 1.0)
				candidate_len = 99999.0;
			total_len += candidate_len;
		}

		if (total_len < best_len)
		{
			best_len = total_len;
			best = tickno;
		}
	}

	printf ("best is %d\n\n", best);
	return best;
}


struct slot *alloc_slot (unsigned int tickno)
{
	struct tick *tick = &ticks[tickno];
	return &tick->slots[tick->n_slots++];
}


void add_task (const char *name, unsigned int period, double len)
{
	unsigned int count, base;
	struct slot *slot;
	struct task *task;
	unsigned int divider = 1;

	task = &tasks[n_tasks++];
	task->name = name;
	task->period = period;
	task->len = len;

	if (period > n_ticks)
	{
		if (period <= max_ticks)
		{
			expand_ticks (period);
			count = n_ticks / period;
		}
		else
		{
			divider = period / n_ticks;
			if (divider > max_divider)
				max_divider = divider;
			count = 1;
		}
	}
	else
	{
		count = n_ticks / period;
	}

	base = find_best_tick (period, count, len);

	while (count > 0)
	{
		slot = alloc_slot (base);
		slot->divider = divider;
		slot->task = task;

		ticks[base].len += task->len;

		base = (base + period) % n_ticks;
		count--;
	}
}


int main (int argc, char *argv[])
{
	init_schedule ();
	add_task ("zerocross_calc", 1, 0.2);
	add_task ("switch_read", 2, 0.3);
	add_task ("flipper_write", 4, 0.7);
	add_task ("lockup_check", 128, 0.1);
	add_task ("led_toggle", 32, 0.01);
	add_task ("lamp_write", 2, 0.4);
	write_schedule (stdout);
	return 0;
}

