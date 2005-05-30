
#include "wpc.h"
#include "mach/coil.h"


/*
 * Twilight Zone Clock Driver
 */

proc(tz_clock_init)
endp


proc(tz_clock_reset)
endp


proc(tz_clock_start_forward)
	uses(a)
	lda	#SOL_CLOCK_REVERSE
	jsr	sol_off
	lda	#SOL_CLOCK_FORWARD
	jsr	sol_on
endp


proc(tz_clock_start_backward)
	uses(a)
	lda	#SOL_CLOCK_FORWARD
	jsr	sol_off
	lda	#SOL_CLOCK_REVERSE
	jsr	sol_on
endp


proc(tz_clock_stop)
	uses(a)
	lda	#SOL_CLOCK_FORWARD
	jsr	sol_off
	lda	#SOL_CLOCK_REVERSE
	jsr	sol_off
endp

