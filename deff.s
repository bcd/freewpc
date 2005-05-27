
#include "wpc.h"

#define DEFF_FUNC_OFF	0
#define DEFF_PRIO_OFF	2


.area ram

highest_prio:		.blkb 1


.area sysrom

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		
	;
	; Description:
	;
	; Inputs:
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(deff_init)
	clr	highest_prio
endp

proc(deff_add_entry)
endp

proc(deff_find_entry)
endp

proc(deff_remove_entry)
endp

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		
	;
	; Description:
	;
	; Inputs:
	; X = procedure
	; A = priority
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(deff_start_const)
	definline(y,x,a)
	bsr	deff_start
endp

proc(deff_start)
endp


	; X = procedure
proc(deff_stop)
endp


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		
	;
	; Description:
	;
	; Inputs:
	; X = procedure
	; A = priority
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(deff_popup_const)
	definline(y,x,a)
	bsr	deff_popup
endp

proc(deff_popup)
endp


