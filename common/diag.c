
#include <freewpc.h>

#define MAX_DIAG_ERRORS 4

U8 diag_error_list[MAX_DIAG_ERRORS];

U8 diag_error_count;


void
diag_post_error (U8 errcode)
{
	if (diag_error_count < MAX_DIAG_ERRORS)
		diag_error_list[diag_error_count++] = errcode;
}

void
diag_post_fatal_error (U8 errcode)
{
}

int
diag_get_error_count (void)
{
	return diag_error_count;
}


void
diag_test_cpu (void)
{
}

void
diag_test_ram (void)
{
}

void
diag_test_rom (void)
{
}

void
diag_test_wpc (void)
{
}

void
diag_run (void)
{
	diag_error_count = 0;

	diag_test_cpu ();
	diag_test_ram ();
	diag_test_rom ();
	diag_test_wpc ();
}



void
diag_run_at_reset (void)
{
	diag_run ();
	if (diag_get_error_count () > 0)
	{
	}
}

