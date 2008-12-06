
#include <freewpc.h>
#include <m6809/math.h>

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
   SEG_TOP+SEG_LEFT+SEG_BOT+SEG_MID_LEFT+SEG_UR_DIAG+SEG_LR_DIAG,	// B
   SEG_LEFT+SEG_TOP+SEG_BOT,										// C
   SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT+SEG_MID,					// D
   SEG_LEFT+SEG_TOP+SEG_MID+SEG_BOT,							// E
   SEG_LEFT+SEG_TOP+SEG_MID,										// F
   SEG_TOP+SEG_LEFT+SEG_BOT+SEG_LWR_RIGHT+SEG_MID_RIGHT,	// G
	SEG_LEFT+SEG_RIGHT+SEG_MID,									// H
	SEG_TOP+SEG_VERT+SEG_BOT,										// I
	SEG_RIGHT+SEG_BOT+SEG_LWR_LEFT,								// J
	SEG_LEFT+SEG_UR_DIAG+SEG_LR_DIAG+SEG_MID_LEFT,			// K
	SEG_LEFT+SEG_BOT,													// L
	SEG_LEFT+SEG_UL_DIAG+SEG_UR_DIAG+SEG_RIGHT,				// M
	SEG_LEFT+SEG_UL_DIAG+SEG_LR_DIAG+SEG_RIGHT,				// N
   SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT,							// O
	SEG_LEFT+SEG_TOP+SEG_UPR_RIGHT+SEG_MID,					// P
   SEG_TOP+SEG_RIGHT+SEG_BOT+SEG_LEFT+SEG_LR_DIAG,			// Q
	SEG_LEFT+SEG_TOP+SEG_UR_DIAG+SEG_MID_LEFT+SEG_LR_DIAG,	// R
   SEG_TOP+SEG_UPR_LEFT+SEG_MID+SEG_LWR_RIGHT+SEG_BOT,	// S
	SEG_TOP+SEG_VERT,													// T
	SEG_LEFT+SEG_BOT+SEG_RIGHT,									// U
	SEG_LEFT+SEG_LL_DIAG+SEG_UR_DIAG,							// V
	SEG_LEFT+SEG_BOT+SEG_VERT_BOT+SEG_RIGHT,					// W
	SEG_UL_DIAG+SEG_UR_DIAG+SEG_LL_DIAG+SEG_LR_DIAG,		// X
	SEG_UL_DIAG+SEG_UR_DIAG+SEG_VERT_BOT,						// Y 
   SEG_TOP+SEG_UR_DIAG+SEG_LL_DIAG+SEG_BOT,					// Z
};



segbits_t seg_data[2][16];

void seg_rtt (void)
{
	U8 offset = get_sys_time () & 0x0F;
	writeb (WPC_ALPHA_POS, offset);
	writew (WPC_ALPHA_ROW1, seg_data[0][offset]);
	writew (WPC_ALPHA_ROW2, seg_data[1][offset]);
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
		*sa = seg_translate_char (c);
}


void seg_write_digit (segaddr_t sa, U8 digit)
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


void seg_write_uint8 (segaddr_t sa, U8 u8)
{
	U8 quot;
	U8 rem;
	U16 quot_rem = div10 (u8);

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
		*sa++ = 0;
}


void seg_init (void)
{
	memset (seg_data, 0, sizeof (seg_data));
}

