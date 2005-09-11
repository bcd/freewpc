

#include <freewpc.h>

;;; TODO : instead of saving S, save offset within local stack (save 1 byte)

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
#define TASK_OFF_ARG			19
#define TASK_OFF_STACK		21
#define TASK_SIZE 			(TASK_OFF_STACK + TASK_STACK_SIZE)

/* Number of tasks to create */
#define NUM_TASKS				16

/* Task states */
#define TASK_FREE			0x0
#define TASK_USED			0x1
#define TASK_BLOCKED		0x2


;;; Temporary memory locations needed during save/restore
;;;;;task_save_U:				.BLKW 1
;;;;;task_save_X:				.BLKW 1


.globl _task_buffer
.globl _task_current
.globl _task_dispatch_tick



.area sysrom

lblreset
task_yield::
task_save::
	stu	_task_save_U		/* save U first since it's needed as a temp */
	stx	_task_save_X		; same goes for X
	puls	u					; U = PC
	ldx	_task_current	; get pointer to current task structure
	stu	TASK_OFF_PC,x	; save PC
	ldu	_task_save_U
	stu	TASK_OFF_U,x	; save U
	ldu	_task_save_X
	stu	TASK_OFF_X,x	; save X
	sta	TASK_OFF_A,x	; save A
	stb	TASK_OFF_B,x	; save B
	sty	TASK_OFF_Y,x	; save Y
	leau	,s					; get current stack pointer
	stu	TASK_OFF_S,x	; save S

	leay	TASK_OFF_STACK,x	; get lowest valid stack address
	cmpy	TASK_OFF_S,x		; compare with current pointer
	ifgt
		ldb	#ERR_TASK_STACK_OVERFLOW
		pshb
		leas	-2,s
		jmp	_do_fatal
	endif

	jmp	task_dispatcher	; ok, find a new task to run


task_restore::	; X = address of task block to restore
	stx	_task_current
	lds	TASK_OFF_S,x
	ldu	TASK_OFF_PC,x
	pshs	u
	ldy	TASK_OFF_Y,x
	lda	TASK_OFF_A,x
	ldb	TASK_OFF_B,x
	ldu	TASK_OFF_U,x
	clr	TASK_OFF_DELAY,x
	ldx	TASK_OFF_X,x
	puls	pc


;;;;proc(task_create_const)
;;;;	definline(y,x)
;;;;	jsr	task_create
;;;;endp

	; X = address of function
proc(task_create)
	requires(x)
	returns(x)
	uses(a,y,u)
	pshs	d,u
	tfr	x,u
	jsr	_task_allocate
	tfr	d,x
	stu	TASK_OFF_PC,x
	puls	d,u
	clr	TASK_OFF_GID,x
	clr	TASK_OFF_ARG,x
	clr	TASK_OFF_ARG+1,x
	sta	TASK_OFF_A,x
	stb	TASK_OFF_B,x
	sty	TASK_OFF_Y,x
	stu	TASK_OFF_U,x
	ldy	#0xEEEE
	sty	TASK_OFF_STACK,x
	leay	TASK_OFF_STACK+TASK_STACK_SIZE-1,x
	sty	TASK_OFF_S,x
endp


proc(task_sleep_const)
	definline(x,a)
	bra	task_sleep1	
endp

proc(task_sleep)
	uses(a,x)
task_sleep1::
	ldx	_task_current
	cmpx	#0000
	ifz
		ldb	#ERR_IDLE_CANNOT_SLEEP
		pshb
		leas	-2,s
		jmp	_do_fatal
	endif
	sta	TASK_OFF_DELAY,x
	lda	_tick_count
	sta	TASK_OFF_ASLEEP,x
	lda	#TASK_BLOCKED
	ora	TASK_OFF_STATE,x
	sta	TASK_OFF_STATE,x
	puls	a,x
	jmp	task_save
endp


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
	cmpx	#_task_buffer + (NUM_TASKS * TASK_SIZE)
	beq	task_list_end

	; Skip empty slots
	tst	TASK_OFF_STATE,x
	beq	dispatch_loop

	; Can this task be executed?
	lda	#TASK_USED
	cmpa	TASK_OFF_STATE,x
	lbeq	task_restore			; Yes, restore it

	; No, check to see if it is asleep
	lda	#TASK_BLOCKED+TASK_USED
	cmpa	TASK_OFF_STATE,x
	bne	dispatch_loop			; No, continue scanning

	lda	_tick_count
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
	ldx	#_task_buffer			; Reset to beginning of buffer
	lbra	dispatch_check


