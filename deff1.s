
#include "wpc.h"

#define DEFF_FUNC_OFF	0
#define DEFF_PRIO_OFF	2
#define SIZEOF_DEFF		3

#define MAX_DEFFS			8

.area ram

deff_table:		.blkw (MAX_DEFFS * SIZEOF_DEFF)

deff_running:	.blkw 1


.area sysrom

deff_table_desc::
	.db	MAX_DEFFS			; Number of table entries
	.db	SIZEOF_DEFF			; Size of each entry
	.dw	deff_table			; Pointer to table base
	.db	DEFF_FUNC_OFF		; Key offset
	.db	sizeof(word)		; Key size
	.db	DEFF_PRIO_OFF		; Sort value offset


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
	uses(x)

	ldx	#deff_table_desc
	jsr	table_init
	ldx	#0
	stx	deff_running
endp

   /**********************************************************
   *	
	* Function:		
	*	
	* Description:
	*	
	* Inputs:
	*	
   * Outputs:
   *	
	**********************************************************/
	; X = address, A = priority
proc(deff_add_entry)
	uses(a,u,x,y)

	/* Find a blank table entry */
	ldx	#deff_table_desc
	ldu	#0000
	jsr	table_find_key_word
	iftrue
		ldx	saved(x)
		stx	DEFF_FUNC_OFF,y
		sta	DEFF_PRIO_OFF,y
		true
	else
		false
	endif
endp

   /**********************************************************
   *	
	* Function:		
	*	
	* Description:
	*	
	* Inputs:
	*	
   * Outputs:
   *	
	**********************************************************/
	; X = address
proc(deff_find_entry)
	uses(x)
	returns(y)
	tfr	x,u
	ldx	#deff_table_desc
	jsr	table_find_key_word
endp


   /**********************************************************
   *	
	* Function:		
	*	
	* Description:
	*	
	* Inputs:
	*	
   * Outputs:
   *	
	**********************************************************/
	; X = address
proc(deff_remove_entry)
	uses(a,u,y)
	bsr	deff_find_entry
	iftrue
		clra
		sta	DEFF_PRIO_OFF,y
		ldu	#0
		stu	DEFF_FUNC_OFF,y
	endif
endp


   /**********************************************************
   *	
	* Function:		
	*	
	* Description:
	*	
	* Inputs:
	*	
   * Outputs:
   *	
	**********************************************************/
proc(deff_update_highest_priority)
	uses(a,x)
	ldx	#deff_table_desc
	jsr	table_find_largest_sort_byte
	cmpy	deff_running
	ifne
		/* Stop the currently running effect, if nonzero */
		jsr	c_task_kill_gid(GID_DEFF)

		/* Start the new one and mark it as the current */
		ldx	DEFF_FUNC_OFF,y
		lda	#GID_DEFF
		jsr	task_create_gid
	endif
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
	jsr	deff_add_entry
	jsr	deff_update_highest_priority
endp

proc(deff_stop_const)
	definline(y,x)
	bsr	deff_stop
endp

	; X = procedure
proc(deff_stop)
	jsr	deff_remove_entry
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



;;;;;proc(deff_exit)
;;;;;	jmp	task_exit
;;;;;endp

