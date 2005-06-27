
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

