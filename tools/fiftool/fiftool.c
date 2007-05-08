/* FIF is the FreeWPC Image Format, and fiftool is the main utility for
reading/writing FIF files.  FIF is a compressed image format.  This is
a replacement for the xbmprog construct.

Common scenarios:

- you have a PGM (at most 2 bitplanes) generated through external means.
You want to convert it to FIF.  Use fiftool -o image.fif -c image.pgm.

- Or maybe it's in XBM format.  That works too: 
fiftool -o image.fif -c image.xbm

- If you have a sequence of FIF frames... you want a FAF, 
FreeWPC Animation Format.  Do: fiftool -o anim.faf -c image1.fif image2.fif...

- Before writing a frame out, maybe you want to apply some transformation on
it.  Use fiftool's command-line options to accept common pgmlib.c functions
without having to write a full-fledged program.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pgmlib.h>

#define error(format, rest...) \
do { \
	fprintf (stderr, "fiftool: "); \
	fprintf (stderr, format, ## rest); \
	fprintf (stderr, "\n"); \
	exit (1); \
} while (0)

#define output_write(ofp, b) fprintf (ofp, "0x%02X, ", b)


enum image_format { 
	FORMAT_BAD, FORMAT_XBM, FORMAT_PGM, FORMAT_FIF, FORMAT_FAF
}; 


unsigned int n_infiles = 0;

const char *infile[128];

const char *outfile;

enum image_format informat;

enum image_format outformat;


enum image_format get_file_format (const char *filename)
{
	char *sep = strrchr (filename, '.');
	if (!sep)
		return FORMAT_BAD;

	sep++;
	if (!strcmp (sep, "xbm"))
		return FORMAT_XBM;
	else if (!strcmp (sep, "pgm"))
		return FORMAT_PGM;
	else if (!strcmp (sep, "fif"))
		return FORMAT_FIF;
	else if (!strcmp (sep, "faf"))
		return FORMAT_FAF;
	else
		return FORMAT_BAD;
}


FILE *output_file_open (void)
{
	FILE *fp;
	char symbol_name[64], *sym;
	char *sep;

	fp = fopen (outfile, "w");
	if (!fp)
		return NULL;

	fprintf (fp, "#include <freewpc.h>\n");
	fprintf (fp, "#include <xbmprog.h>\n\n");

	sprintf (symbol_name, "%s", outfile);

	sym = symbol_name;
	while ((sep = strchr (sym, '/')) != NULL)
		sym = sep+1;

	sep = strrchr (sym, '.');
	if (!sep)
		error ("invalid output file name");
	*sep++ = '\0';

	fprintf (fp, "const U8 %s_%s[] = {\n", sep, sym);
	return fp;
}


void output_file_close (FILE *fp)
{
	fprintf (fp, "};\n");
	fclose (fp);
}


void write_fif (void)
{
	FILE *ofp;
	int n;
	int plane;
	PGM *pgm;
	XBM *xbm;
	XBMSET *xbmset;
	XBMPROG *xbmprog;
	enum image_format format;
	int n_planes = 2;

	ofp = output_file_open ();
	for (n=0 ; n < n_infiles; n++)
	{
		format = get_file_format (infile[n]);

		/* Output the format (XBM or PGM).  Equivalently, this
		is the number of bitplanes in the image. */
		output_write (ofp, format);

		switch (format)
		{
			case FORMAT_XBM:
				n_planes = 1;
				/* FALLTHROUGH */

			case FORMAT_PGM:	
				/* Convert PGM to FIF.
				First we have to convert the PGM to two XBMs. */
				/* Then convert each XBM in turn to FIF. */
				pgm = pgm_read (infile[n]);
				pgm_change_maxval (pgm, (1 << n_planes) - 1);
				xbmset = pgm_make_xbmset (pgm);
				for (plane = 0; plane < n_planes; plane++)
				{
					xbm = xbmset_plane (xbmset, plane);
					xbmprog = xbm_make_prog (xbm);
					fprintf (ofp, "\nXBMPROG_METHOD_RLE, ");
					xbmprog_write (ofp, NULL, 0, xbmprog);
					xbmprog_free (xbmprog);
				}
				xbmset_free (xbmset);
				pgm_free (pgm);
				break;
			default:
				error ("invalid input file format");
		}
	}
	output_file_close (ofp);
}


void write_faf (void)
{
	FILE *ofp = output_file_open ();
	output_file_close (ofp);
}


int main (int argc, char *argv[])
{
	int argn;
	const char *arg;
	enum image_format format;

	for (argn = 1; argn < argc; argn++)
	{
		arg = argv[argn];
		if (*arg == '-')
		{
			switch (arg[1])
			{
				case 'c':
					break;

				case 'o':
					outfile = argv[++argn];
					break;
			}
		}
		else
		{
			/* Add to input file list */
			infile[n_infiles++] = arg;
		}
	}

	if (!outfile)
		error ("no output file given");

	switch (format = get_file_format (outfile))
	{
		case FORMAT_FIF:
			write_fif ();
			break;
		case FORMAT_FAF:
			write_faf ();
			break;
		default:
			error ("invalid output file format (%d) for %s", format, outfile);
	}
}


