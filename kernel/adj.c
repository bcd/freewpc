
#include <freewpc.h>

__nvram__ std_adj_t system_config;

__nvram__ pricing_adj_t price_config;


void adj_init (void)
{
	extern void adj_verify_all (void);
	call_far (TEST_PAGE, adj_verify_all ());
}

