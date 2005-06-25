
#include <freewpc.h>

typedef uint8_t deffnum_t;

typedef void (*deff_static_fn_t) (void) __taskentry__;
typedef void (*deff_task_fn_t) (void) __taskentry__;

/* A quick deff is one that last for a short time period.
 * By default, a deff is assumed to run forever until stopped.
 * If a quick deff can't be started due to priority issues, then it
 * is ignored.  Normally a deff is queued up. */
#define D_QUICK		0x01

typedef struct
{
	uint8_t flags;
	uint8_t prio;
	deff_static_fn_t static_fn;
	deff_task_fn_t task_fn;
} deff_t;


static const deff_t deff_table[] = {
};

static uint8_t deff_prio;


void deff_start (deffnum_t dn)
{
}

void deff_init (void)
{
}


