
#include <freewpc.h>

/** A pointer to the next byte of CVSD data to send */
U8 *cvsd_output;

/** A pointer to one past the end of the CVSD data.  When
the output pointer reaches this value, the CVSD is stopped. */
U8 *cvsd_end;

/** The page of ROM in which the CVSD data comes from. */
U8 cvsd_page;

U8 cvsd_data;

U8 cvsd_count;


/** Service the CVSD device, by sending at most a single bit. */
void cvsd_service (void)
{
}


void cvsd_start (U8 *start, U8 *end, U8 page)
{
	cvsd_data = far_read8 (start++, page);
	cvsd_output = start;
	cvsd_end = end;
	cvsd_count = 8;
}


void cvsd_stop (void)
{
	cvsd_count = 0;
	cvsd_output = NULL;
}

