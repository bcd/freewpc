
#include <freewpc.h>

U8 flippers_enabled;

U8 flipper_inputs;

U8 flipper_outputs;



void sw_left_flipper_handler (void) 
{
	if (deff_get_active () == DEFF_AMODE)
	{
		amode_left_flipper ();
	}
}

void sw_right_flipper_handler (void)
{
	if (deff_get_active () == DEFF_AMODE)
	{
		amode_right_flipper ();
	}
}

void sw_upper_left_flipper_handler (void)
{
}

void sw_upper_right_flipper_handler (void)
{
}


DECLARE_SWITCH_DRIVER (sw_left_flipper) {
	.fn = sw_left_flipper_handler,
};
DECLARE_SWITCH_DRIVER (sw_right_flipper) {
	.fn = sw_right_flipper_handler,
};
DECLARE_SWITCH_DRIVER (sw_upper_left_flipper) {
	.fn = sw_upper_left_flipper_handler,
};
DECLARE_SWITCH_DRIVER (sw_upper_right_flipper) {
	.fn = sw_upper_right_flipper_handler,
};


void flipper_enable (void)
{
}


void flipper_disable (void)
{
}


extern inline void flipper_service (
	U8 sw_button,
	U8 sw_eos,
	U8 sol_power,
	U8 sol_hold )
{
	if (flipper_inputs & sw_button)
	{
		if (flipper_inputs & sw_eos)
		{
			flipper_outputs |= sol_hold;
		}
		else
		{
			flipper_outputs |= sol_power;
		}
	}
}


void flipper_rtt (void)
{
	flipper_inputs = ~wpc_read_flippers ();
	flipper_outputs = 0;

	if (flippers_enabled)
	{
		flipper_service (WPC_LL_FLIP_SW, WPC_LL_FLIP_EOS, WPC_LL_FLIP_POWER, WPC_LL_FLIP_HOLD);
		flipper_service (WPC_LR_FLIP_SW, WPC_LR_FLIP_EOS, WPC_LR_FLIP_POWER, WPC_LR_FLIP_HOLD);
		flipper_service (WPC_UL_FLIP_SW, WPC_UL_FLIP_EOS, WPC_UL_FLIP_POWER, WPC_UL_FLIP_HOLD);
		flipper_service (WPC_UR_FLIP_SW, WPC_UR_FLIP_EOS, WPC_UR_FLIP_POWER, WPC_UR_FLIP_HOLD);
	}

	wpc_write_flippers (~flipper_outputs);
}

void flipper_init (void)
{
	flippers_enabled = 1;
}


