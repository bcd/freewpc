
#include <stdio.h>
#include <string.h>


typedef unsigned char U8;

U8 image[0x10000];

U8 fill_byte = 0x00;


unsigned int hexval (char *s, int len)
{
	unsigned int r = 0;
	while (--len >= 0)
	{
		r *= 16;
		r += ((*s >= 'A') ? (*s - 'A' + 10) : (*s - '0'));
		s++;
	}
	return r;
}

int main (int argc, char *argv[])
{
	FILE *ifp;
	FILE *ofp;
	char line[256];
	char *s;
	int len;
	int addr;
	int i;
	char *srec_file = "freewpc.s19";
	char *bin_file = "freewpc.bin";
	char *option;
	int start_offset = 0;
	int write_length = 0;
	char **argn = &argv[1];

	srec_file = *argn++;
	bin_file = *argn++;
	option = *argn++;
	if (!strcmp (option, "system"))
	{
		start_offset = 0x8000;
		write_length = 0x8000;
	}
	else
	{
		start_offset = 0x4000;
		write_length = 0x4000;
	}

	memset (image, fill_byte, sizeof (image));

	ifp = fopen (srec_file, "r");
	for (;;)
	{
		fgets (line, 255, ifp);
		if (feof (ifp))
			break;

		s = line;

		/* Skip over S */
		s++;

		/* Check S-record type */
		if (*s == '9')
			break;
		s++;

		/* Next two bytes are record length */
		len = hexval (s, 2) - 3;
		s += 2;

		/* Next four bytes are address in hex */
		addr = hexval (s, 4);
		s += 4;

		/* Next len bytes are data */
		for (i=0; i < len; i++)
		{
			image[addr+i] = hexval (s, 2);
			s += 2;
		}
	}
	fclose (ifp);
	
	ofp = fopen (bin_file, "wb");
	fwrite (image + start_offset, 
		sizeof (U8), 
		write_length, ofp);
	fclose (ofp);

	exit (0);
}

