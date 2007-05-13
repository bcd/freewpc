
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 512

int font_spacing = 0;

int font_max_height = 0;

char font_name[64];

struct font_char
{
	int width;
	int height;
	unsigned long rows[64]; /* 64=max height */
} fchar[256];


static int
prefix (const char *line, const char *prefix)
{
	return !strncmp (line, prefix, strlen (prefix));
}


void
font_file_read (const char *filename)
{
	FILE *fp;
	int i, j;
	char line[MAXLINE];
	int defining_width = 1;
	char *p;
	char index;

	fp = fopen (filename, "r");

	for (i=0; i < 256; i++)
		fchar[i].width = fchar[i].height = 0;

	for (;;)
	{
		fgets (line, MAXLINE, fp);
		if (feof (fp))
			break;
		
		if (prefix (line, "#define glyph_"))
		{
			for (i = 0, p = line + strlen ("#define glyph_"); 
				*p != '_';
				i++, p++)
			{
				font_name[i] = *p;
			}

			++p;
			if (prefix (p, "period")) { index = '.'; p += strlen ("period")+1; }
			else if (prefix (p, "comma")) { index = ','; p += strlen ("comma")+1; }
			else if (prefix (p, "slash")) { index = '/'; p += strlen ("slash")+1; }
			else if (prefix (p, "amp")) { index = '&'; p += strlen ("amp")+1; }
			else if (prefix (p, "minus")) { index = '-'; p += strlen ("minus")+1; }
			else if (prefix (p, "bang")) { index = '!'; p += strlen ("bang")+1; }
			else if (prefix (p, "dollar")) { index = '$'; p += strlen ("dollar")+1; }
			else if (prefix (p, "percent")) { index = '%'; p += strlen ("percent")+1; }
			else { index = *p; p += 2; }

			if (*p == 'w')
				fchar[index].width = strtoul (p + strlen ("width "), NULL, 0);
			else
				fchar[index].height = strtoul (p + strlen ("height "), NULL, 0);
		}
		else if (prefix (line, "\t.spacing = "))
		{
			font_spacing = strtoul (line + strlen ("\t.spacing = "), NULL, 0);
		}
		else if (prefix (line, "\t.height = "))
		{
			font_max_height = strtoul (line + strlen ("\t.height = "), NULL, 0);
		}
		else if (prefix (line, " 0x"))
		{
			i = 0;
			p = line;
next_glyph:
			j = (fchar[index].width + 7) / 8;
			fchar[index].rows[i] = 0;

			while (*p != '}')
			{
				int val = strtoul (p, &p, 0);
				fchar[index].rows[i] <<= 8;
				fchar[index].rows[i] |= val;
				if (*p == ',') p++; /* skip comma */
				if (*p == '}') break;
				if (*p == '\n')
				{
					fgets (line, MAXLINE, fp);
					p = line;
				}

				j--;
				if (j == 0)
				{
					i++;
					goto next_glyph;
				}
			}
		}
	}

	fclose (fp);
}


const char *
index_print (char index)
{
	static char def[2];

	switch (index)
	{
		case '.':
			return "period";
		case ',':
			return "comma";
		case '/':
			return "slash";
		case '&':
			return "amp";
		case '-':
			return "minus";
		case '!':
			return "bang";
		case '$':
			return "dollar";
		case '%':
			return "percent";
		default:
			def[0] = index;	
			def[1] = '\0';
			return def;
	}
}


int
font_file_write (const char *filename)
{
	FILE *fp;
	int i, j;

	fp = stdout;

	fprintf (fp, "#include <freewpc.h>\n");
	fprintf (fp, "#pragma section (\"page61\")\n");
	for (i=0; i < 256; i++)
	{
		const char *idx = index_print (i);
		if (fchar[i].width || fchar[i].height)
		{
			fprintf (fp, "#define glyph_%s_%s_width %d\n",
				font_name, idx, fchar[i].width);
			fprintf (fp, "#define glyph_%s_%s_height %d\n",
				font_name, idx, fchar[i].height);
			fprintf (fp, "static char glyph_%s_%s_bits[] = { glyph_%s_%s_width, glyph_%s_%s_height,\n ",
				font_name, idx,
				font_name, idx,
				font_name, idx);
			for (j=0 ; j < fchar[i].height; j++)
			{
				switch ((fchar[i].width + 7) / 8)
				{
					case 4:
						fprintf (fp, "0x%02x,", (fchar[i].rows[j] >> 24) & 0xFF);
					case 3:
						fprintf (fp, "0x%02x,", (fchar[i].rows[j] >> 16) & 0xFF);
					case 2:
						fprintf (fp, "0x%02x,", (fchar[i].rows[j] >> 8) & 0xFF);
					case 1:
						fprintf (fp, "0x%02x,", fchar[i].rows[j] & 0xFF);
				}
			}
			fprintf (fp, "};\n");
		}
	}

	fprintf (fp, "static char *%s_glyphs[] = {\n", font_name);
	for (i=0; i < 256; i++)
	{
		const char *idx = index_print (i);
		if (fchar[i].width || fchar[i].height)
		{
			fprintf (fp, "   ['%c'] = glyph_%s_%s_bits,\n", i, font_name, idx);
		}
	}
	fprintf (fp, "};\n");

	fprintf (fp, "const font_t font_%s = {\n", font_name);
	fprintf (fp, "   .width = 0,\n");
	fprintf (fp, "   .bytewidth = 0,\n");
	fprintf (fp, "   .spacing = %d,\n", font_spacing);
	fprintf (fp, "   .height = %d,\n", font_max_height);
	fprintf (fp, "   .glyphs = %s_glyphs,\n", font_name);
	fprintf (fp, "};\n");

	if (fp != stdout)
		fclose (fp);
}


void
font_rename (const char *newname)
{
	strcpy (font_name, newname);
}


void
font_scale_width (int factor)
{
	int i, j, k, m;

	for (i=0; i < 256; i++)
	{
		unsigned int newrow;
		unsigned int bit;

		if (!fchar[i].width || !fchar[i].height)
			continue;

		for (j = 0; j < fchar[i].height; j++)
		{
			newrow = 0;
			for (k = fchar[i].width-1; k >= 0; --k)
			{
				bit = (fchar[i].rows[j] & (1 << k)) >> k;
				for (m=0; m < factor; m++)
				{
					newrow <<= 1;
					newrow |= bit;
				}
			}
			fchar[i].rows[j] = newrow;
		}
	}
	fchar[i].width *= factor;
}


void
font_scale_height (int factor)
{
	int i, j, k;

	font_max_height *= factor;
	for (i=0; i < 256; i++)
	{
		if (!fchar[i].width && !fchar[i].height)
			continue;

		for (j = fchar[i].height-1; j >= 0; --j)
			for (k=0; k < factor; k++)
				fchar[i].rows[j*factor+k] = fchar[i].rows[j];
		fchar[i].height *= factor;
	}
}


int
main (int argc, char *argv[])
{
	font_file_read (argv[1]);
	font_scale_height (2);
	font_rename ("test");
	font_file_write ("");
	return (0);
}
