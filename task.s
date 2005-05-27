

#include "wpc.h"


/*
 * Task structure
 */
#define TASK_OFF_GID			0
#define TASK_OFF_PC			2
#define TASK_OFF_NEXT		4		/* Not used today */
#define TASK_OFF_X			6
#define TASK_OFF_Y			8
#define TASK_OFF_S			10
#define TASK_OFF_U			12
#define TASK_OFF_DELAY		14
#define TASK_OFF_ASLEEP		15
#define TASK_OFF_STATE		16
#define TASK_OFF_A			17
#define TASK_OFF_B			18
#define TASK_OFF_STACK		19
#define TASK_SIZE 			64

/* Number of tasks to create */
#define NUM_TASKS				16

/* Size of the task stack - uses all of the remaining
 * bytes of the task structure after the fixed values */
#define TASK_STACK_SIZE		(TASK_SIZE - TASK_OFF_STACK - 1)

/* Task states */
#define TASK_FREE			0x0
#define TASK_USED			0x1
#define TASK_BLOCKED		0x2

.area ram

;;; Temporary memory locations needed during save/restore
task_save_U::				.BLKW 1
task_save_X::				.BLKW 1

;;; Declaration of task buffers
task_buffer::		.BLKB	(NUM_TASKS * TASK_SIZE)
task_buffer_end = .

;;; The task that is currently running
task_current::				.BLKW 1

;;; The time of the last dispatch
task_dispatch_tick::		.BLKB 1



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
proc(task_init)
	uses(x,y)
	ldx	#task_buffer
	loop
		jsr	task_free
		leax	TASK_SIZE,x
		cmpx	#task_buffer_end
	while(ne)

	jsr	task_allocate
	stx	task_current

	lda	tick_count
	sta	task_dispatch_tick
endp


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
proc(task_allocate)
	uses(a)
	ldx	#task_buffer
	loop
		tst	TASK_OFF_STATE,x
		ifz
			lda	#TASK_USED
			sta	TASK_OFF_STATE,x
			clr	TASK_OFF_DELAY,x
			return
		endif

		leax	TASK_SIZE,x
		cmpx	#task_buffer_end
	while(nz)

	jsr	c_sys_error(ERR_NO_FREE_TASKS)
endp


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
proc(task_free)
	clr	TASK_OFF_STATE,x
endp


task_yield::
task_save::
	stu	task_save_U		/* save U first since it's needed as a temp */
	stx	task_save_X		; same goes for X
	puls	u					; U = PC
	ldx	task_current	; get pointer to current task structure
	stu	TASK_OFF_PC,X	; save PC
	ldu	task_save_U
	stu	TASK_OFF_U,X	; save U
	ldu	task_save_X
	stu	TASK_OFF_X,X	; save X
	sta	TASK_OFF_A,X	; save A
	stb	TASK_OFF_B,X	; save B
	sty	TASK_OFF_Y,X	; save Y
	leau	,s					; get current stack pointer
	stu	TASK_OFF_S,X	; save S

	leay	TASK_OFF_STACK,x	; get lowest valid stack address
	cmpy	TASK_OFF_S,X		; compare with current pointer
	ifgt
		jsr	c_sys_error(ERR_TASK_STACK_OVERFLOW)	
	endif

	jmp	task_dispatcher	; ok, find a new task to run


task_restore::	; X = address of task block to restore
	stx	task_current
	lds	TASK_OFF_S,X
	ldu	TASK_OFF_PC,X
	pshs	u
	ldy	TASK_OFF_Y,X
	lda	TASK_OFF_A,X
	ldb	TASK_OFF_B,X
	ldu	TASK_OFF_U,X
	clr	TASK_OFF_DELAY,X
	ldx	TASK_OFF_X,x
	puls	pc

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
proc(task_create_const)
	definline(y,x)
	jsr	task_create
endp

	; X = address of function
proc(task_create)
	requires(x)
	returns(x)
	uses(a,y,u)
	pshs	u
	tfr	x,u
	jsr	task_allocate
	stu	TASK_OFF_PC,x
	puls	u
	clr	TASK_OFF_GID,x
	sta	TASK_OFF_A,x
	stb	TASK_OFF_B,x
	sty	TASK_OFF_Y,x
	stu	TASK_OFF_U,x
	leay	TASK_OFF_STACK+TASK_STACK_SIZE,x
	sty	TASK_OFF_S,x
endp


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		
	;
	; Description:
	;
	; Inputs:
	; X = address of function
	; A = new group ID
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(task_create_gid_const)
	definline(y,x,a)
	bsr	task_create_gid
endp


proc(task_create_gid)
	bsr	task_create
	sta	TASK_OFF_GID,x
endp


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
proc(task_create_gid1_const)
	definline(y,x,a)
	bsr	task_create_gid1
endp

proc(task_create_gid1)
	jsr	task_find_gid
	iffalse
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
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(task_recreate_gid_const)
	definline(y,x,a)
	bsr	task_recreate_gid
endp

proc(task_recreate_gid)
	jsr	task_kill_gid
	jsr	task_create_gid
endp

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Function:		
	;
	; Description:
	;
	; Inputs:
	; X = task handle (pid)
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
proc(task_getgid)
	uses(x)
	returns(a)
	ldx	task_current
	lda	TASK_OFF_GID,x
endp


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
proc(task_sleep_const)
	definline(x,a)
	bra	task_sleep1	
endp

proc(task_sleep)
	uses(a,x)
task_sleep1::
	ldx	task_current
	cmpx	#0000
	ifz
		jsr	c_sys_error(ERR_IDLE_CANNOT_SLEEP)
	endif
	sta	TASK_OFF_DELAY,x
	lda	tick_count
	sta	TASK_OFF_ASLEEP,x
	lda	#TASK_BLOCKED
	ora	TASK_OFF_STATE,x
	sta	TASK_OFF_STATE,x
	puls	a,x
	jmp	task_save
endp


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
proc(task_sleepl_const)
	definline(x,d)
	jsr	task_sleepl
endp

proc(task_sleepl)
	uses(d)
	loop
		tsta
		ifz
			tfr	b,a
			jsr	task_sleep
			return
		endif
		jsr	task_sleep
		deca
	endloop
endp


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
proc(task_exit)
	ldx	task_current
	cmpx	#0000
	ifz
		jsr	c_sys_error(ERR_IDLE_CANNOT_EXIT)
	endif
	jsr	task_free
	ldu	#0000
	stu	task_current
	jmp	task_dispatcher
endp


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
proc(task_kill_pid)
	requires(x)
	cmpx	task_current
	ifne
		jsr	task_free
	else
		jsr	c_sys_error(ERR_TASK_KILL_CURRENT)
	endif
endp


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
proc(task_find_gid)
	requires(a)
	uses(x)
	ldx	#task_buffer
	loop
		tst	TASK_OFF_STATE,x
		ifnz
			cmpa	TASK_OFF_GID,x
			ifeq
				cmpx	task_current
				ifne
					true
					return
				endif
			endif
		endif

		leax	TASK_SIZE,x
		cmpx	#task_buffer_end
	while(nz)
	false
endp


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
proc(task_kill_gid_const)
	definline(x,a)
	bsr	task_kill_gid
endp

proc(task_kill_gid)
	requires(a)
	uses(x)
	ldx	#task_buffer
	loop
		tst	TASK_OFF_STATE,x
		ifnz
			cmpa	TASK_OFF_GID,x
			ifeq
				cmpx	task_current
				ifne
					jsr	task_kill_pid	
				endif
			endif
		endif

		leax	TASK_SIZE,x
		cmpx	#task_buffer_end
	while(nz)
endp

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; Label:
	;
	; Description:
	;
	; Inputs:
	;
	; Outputs:
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
task_dispatcher::
	;;; Pseudocode:
	;;;  if time tick has advanced:
	;;;     check to see if any tasks have expired
	;;;     if so move them to the ready queue
	;;;  move next ready task to current
	;;;  restore context of current task, starting it
	;;;  (does not return)

	; Advance current pointer to next block
dispatch_loop:
	leax	TASK_SIZE,x
dispatch_check:
	cmpx	#task_buffer_end
	beq	task_list_end

	; Skip empty slots
	tst	TASK_OFF_STATE,x
	beq	dispatch_loop

	; Can this task be executed?
	lda	#TASK_USED
	cmpa	TASK_OFF_STATE,x
	lbeq	task_restore			; Yes, restore it

	; No, check to see if it is asleep
	lda	#TASK_BLOCKED
	bita	TASK_OFF_STATE,x
	beq	dispatch_loop			; No, continue scanning

	lda	tick_count
	suba	TASK_OFF_ASLEEP,x		; Compute time spent asleep so far
	cmpa	TASK_OFF_DELAY,x		; Compare against scheduled delay
	blt	dispatch_loop			; Not ready yet, continue

	lda	#TASK_BLOCKED
	coma
	anda	TASK_OFF_STATE,x
	sta	TASK_OFF_STATE,x
	lbra	task_restore

task_list_end:
	; Execute idle tasks on system stack
	lds	#STACK_BASE
	jsr	switch_idle_task

	; Start scanning from beginning of table again
	ldx	#task_buffer			; Reset to beginning of buffer
	lbra	dispatch_check


