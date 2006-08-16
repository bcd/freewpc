
#include "pgmlib.h"

void
pgm_read_test (void)
{
	PGM *pgm = pgm_read ("images/brianhead.pgm");
	pgm_change_maxval (pgm, 3);
	pgm_invert (pgm);
	pgm_write_xbmset (pgm, "images/readtest.xbm", "readtest");
}


void
machgen (void)
{
#if 1
	pgm_read_test ();
#endif
	gen_mborder ();
	gen_multisquare_background ();
	gen_dithered_dark_background ();
	gen_tile_background ();
	gen_ball_border ();
}

