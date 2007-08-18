
#include <pgmlib.h>

int main (int argc, char *argv[])
{
	int i;

	PGM *pgm = pgm_read ("images/cow.pgm");
	PGM *pgm2 = pgm_alloc ();

	pgm_change_maxval (pgm, 3);
	pgm_set_plane_count (pgm2, 2);

	pgm_paste (pgm2, pgm, -4, 0);
	pgm_xor (pgm2, pgm2, pgm);
	pgm_write_xbm (pgm2, "build/cow1.xbm", "cow1", 0);
	pgm_invert (pgm2);
	pgm_write (pgm2, "build/cow1.pgm");


	FILE *fp = fopen ("build/cow2.prg", "wb");
	for (i=0; i < 8; i++)
	{
		/* Only plane 0 is of interest (it's mono) */
		XBMSET *xbmset = pgm_make_xbmset (pgm);
		XBM *xbm = xbmset_plane (xbmset, 0);

		/* Convert to prog format, and write */
		XBMPROG *xbmprog = xbm_make_prog (xbm);
		xbmprog_write (fp, "cow_anim", i, xbmprog);
		xbmprog_free (xbmprog);
		xbmset_free (xbmset);

		/* Shift image left by 4 pixels */
		pgm2 = pgm_alloc ();
		pgm_set_plane_count (pgm2, 2);
		pgm_paste (pgm2, pgm, -4, 0);
		pgm_free (pgm);
		pgm = pgm2;
	}
	fprintf (fp, "unsigned char __end = XBMPROG_METHOD_END;\n");
	fclose (fp);
	return 0;
}

