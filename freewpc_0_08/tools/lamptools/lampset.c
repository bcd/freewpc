
#define MAX_LAMPS 64

typedef unsigned char U8, task_ticks_t;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/lamp.h>
#include <mach/lamp.h>

struct lampset {
	U8 lamps[MAX_LAMPS];
	int count;
};


struct lampset *
lampset_allocate (void)
{
	struct lampset *lms = malloc (sizeof (struct lampset));
	lms->count = 0;
}


void
lampset_free (struct lampset *lms)
{
	free (lms);
}


void
lampset_dump (struct lampset *lms)
{
	int count = lms->count;
	const U8 *lamps = lms->lamps;

	printf ("[ ");
	while (count-- > 0)
		printf ("%02X ", *lamps++);		
	printf ("]\n");
}


void
lampset_fill (struct lampset *lms, const U8 *lamps, int count)
{
	memcpy (lms->lamps, lamps, count);
	lms->count = count;
}

void
lampset_reverse (struct lampset *lms)
{
}

void
lampset_add (struct lampset *lms1, struct lampset *lms2)
{
}

void
lampset_subtract (struct lampset *lms1, struct lampset *lms2)
{
}

void
lampset_and (struct lampset *lms1, struct lampset *lms2)
{
}

void
lampset_or (struct lampset *lms1, struct lampset *lms2)
{
}

void
lampset_randomize (struct lampset *lms, int count)
{
	int i;
	for (i=0; i < lms->count * 8 * count; i++)
	{
		int j = ((rand () * 1.0) / RAND_MAX) * lms->count;
		int k = ((rand () * 1.0) / RAND_MAX) * lms->count;
		U8 temp = lms->lamps[j];
		lms->lamps[j] = lms->lamps[k];
		lms->lamps[k] = temp;
	}
}


void
lampset_output (struct lampset *lms, const char *name)
{
	int i;

	printf ("#define %s ", name);
	for (i=0; i < lms->count; i++)
		printf ("%d%c", 
			lms->lamps[i],
			(i == lms->count - 1) ? ' ' : ',');
	printf ("\n");
}

int main (int argc, char *argv[])
{
	struct lampset *lms = lampset_allocate ();
	const U8 temp[] = { LMSET_AMODE_ALL };

	lampset_fill (lms, temp, sizeof (temp));
	lampset_randomize (lms, 2);
	lampset_output (lms, "LMSET_AMODE_RAND");
	return (0);
}

