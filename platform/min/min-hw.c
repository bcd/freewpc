
#include <freewpc.h>
#include <system/platform.h>

void switch_rtt (void)
{
	U8 edge;
	sw_raw[0] = readb (IO_SWITCH);
	edge = sw_raw[0] ^ sw_logical[0];
	sw_stable[0] |= edge & sw_edge[0];
	sw_unstable[0] |= ~edge & sw_stable[0];
	sw_edge[0] = edge;
}

void lamp_rtt (void)
{
	U8 bits;
	bits = lamp_matrix[lamp_strobe_column];
	bits &= lamp_leff2_allocated[lamp_strobe_column];
	bits |= lamp_leff2_matrix[lamp_strobe_column];
	bits &= lamp_leff1_allocated[lamp_strobe_column];
	bits |= lamp_leff1_matrix[lamp_strobe_column];
	writeb (IO_LAMP, bits);	
}

