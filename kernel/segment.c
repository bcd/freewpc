
#include <freewpc.h>
#include <m6809_math.h>

const segbits_t seg_digit_table[] = {
   [0] = SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,
   [1] = SEG_RIGHT,
   [2] = SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT,
   [3] = SEG_TOP+SEG_MID+SEG_BOT+SEG_RIGHT,
   [4] = SEG_UPR_LEFT+SEG_MID+SEG_RIGHT,
   [5] = SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,
   [6] = SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID,
   [7] = SEG_TOP+SEG_RIGHT,
   [8] = SEG_TOP+SEG_MID+SEG_BOT+SEG_LEFT+SEG_RIGHT,
   [9] = SEG_TOP+SEG_MID+SEG_BOT+SEG_UPR_LEFT+SEG_RIGHT,
};

const segbits_t seg_alpha_table[] = {
   SEG_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT,							// A
   SEG_LEFT+SEG_MID+SEG_BOT+SEG_LWR_RIGHT,					// B
   SEG_TOP+SEG_LEFT+SEG_BOT,										// C
   SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT+SEG_MID,					// D
   SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT,							// E
   SEG_LEFT+SEG_TOP+SEG_MID,										// F
   SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID,			// G
	SEG_LEFT+SEG_RIGHT+SEG_MID,									// H
	SEG_TOP+SEG_VERT+SEG_BOT,										// I
	SEG_RIGHT+SEG_BOT,												// J
	0,																		// K
	SEG_LEFT+SEG_BOT,													// L
	SEG_LEFT+SEG_TOP+SEG_VERT+SEG_RIGHT,						// M
	SEG_LEFT+SEG_TOP+SEG_RIGHT,									// N
   SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,							// O
	SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID,					// P
	SEG_UPR_LEFT+SEG_TOP+SEG_MID+SEG_RIGHT,					// Q
	SEG_LEFT+SEG_TOP,													// R
   SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,	// S
	SEG_TOP+SEG_VERT,													// T
	SEG_LEFT+SEG_BOT+SEG_RIGHT,									// U
	0,																		// V
	SEG_LEFT+SEG_BOT+SEG_VERT+SEG_RIGHT,						// W
	0,																		// X
	SEG_UPR_LEFT+SEG_MID+SEG_BOT+SEG_RIGHT,					// Y 
   SEG_TOP+SEG_UPR_RIGHT+SEG_MID+SEG_LWR_LEFT+SEG_BOT,	// Z
};

void seg_set (segaddr_t sa, segbits_t bits)
{
	asm ("lda %0\n\tldb %1\n\tjsr seg_set\n\n"
			:
			: "m" (sa), "m" (bits)
			: "d"
		 );
}


const uint16_t seg_start_offset[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107,
	0x108, 0x109, 0x10A, 0x10B, 0x10C, 0x10D, 0x10E, 0x10F,
	0x180, 0x181, 0x182, 0x183, 0x184, 0x185, 0x186, 0x187,
	0x188, 0x189, 0x18A, 0x18B, 0x18C, 0x18D, 0x18E, 0x18F,
};


void seg_set_new (segaddr_t sa, segbits_t bits)
{
	register uint8_t *dmd_buf_ptr;
	uint8_t pix;

	dmd_buf_ptr = (uint8_t *)DMD_LOW_BASE;
	dmd_buf_ptr += seg_start_offset[sa];

	pix = (bits & SEG_TOP) ? 0xFC : 0x00;
	dmd_buf_ptr[16 * 0] = pix;

	pix = (bits & SEG_UPR_LEFT) ? 0x4 : 0x00;
	if (bits & SEG_VERT) pix |= 0x10;
	if (bits & SEG_UPR_RIGHT) pix |= 0x80;
	dmd_buf_ptr[16 * 1] = pix;
	dmd_buf_ptr[16 * 2] = pix;

	pix = (bits & SEG_MID) ? 0xFC : 0x00;
	dmd_buf_ptr[16 * 3] = pix;

	pix = (bits & SEG_LWR_LEFT) ? 0x4 : 0x00;
	if (bits & SEG_VERT) pix |= 0x10;
	if (bits & SEG_LWR_RIGHT) pix |= 0x80;
	dmd_buf_ptr[16 * 4] = pix;
	dmd_buf_ptr[16 * 5] = pix;

	pix = (bits & SEG_BOT) ? 0xFC : 0x00;
	dmd_buf_ptr[16 * 6] = pix;
}


segbits_t seg_translate_char (char c)
{
	if ((c >= '0') && (c <= '9'))
		return seg_digit_table[c - '0'];
	else if ((c >= 'A') && (c <= 'Z'))
		return seg_alpha_table[c - 'A'];
	else if (c == ' ')
		return 0;
	else
		return SEG_MID;
}


void seg_write_char (segaddr_t sa, char c)
{
	if (c != '\0')
		seg_set (sa, seg_translate_char (c));
}


void seg_write_digit (segaddr_t sa, uint8_t digit)
{
	if (digit < 10)
		seg_write_char (sa, digit + '0');
	else
		seg_write_char (sa, digit - 10 + 'A');
}


void seg_write_bcd (segaddr_t sa, bcd_t bcd)
{
	seg_write_digit (sa, bcd >> 4);
	seg_write_digit (sa+1, bcd & 0x0F);
}


void seg_write_uint8 (segaddr_t sa, uint8_t u8)
{
	uint8_t quot;
	uint8_t rem;
	uint16_t quot_rem = div10 (u8);

	asm ("sta %0" :: "m" (quot));
	asm ("stb %0" :: "m" (rem));

	seg_write_digit (sa, quot);
	seg_write_digit (sa+1, rem);
}


void seg_write_string (segaddr_t sa, const char *s)
{
	while (*s != '\0')
		seg_write_char (sa++, *s++);
}


void seg_erase (segaddr_t sa, int8_t len)
{
	while (--len > 0)
		seg_set (sa++, 0);
}

