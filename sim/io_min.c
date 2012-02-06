
#include <freewpc.h>
#include <simulation.h>
#include <hwsim/io.h>

void io_min_init (void)
{
	io_add_direct_switches (IO_SWITCH, 0);
	io_add_sol_bank (IO_SOL, 0);
}

