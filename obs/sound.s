
#include "wpc.h"

#define MUSIC_STACK_SIZE	8

.area fastram

music_stack::		.blkb MUSIC_STACK_SIZE
music_head::		.blkw 1

.area sysrom


proc(sound_init)
	clra
	sta	WPCS_CONTROL_STATUS
	jsr	music_off
endp


proc(music_set)
	requires(a)		; A = music code
	uses(x)
	ldx	music_head
	sta	,x
	sta 	WPCS_DATA
endp


proc(music_change)
	uses(x)
	requires(a)		; A = music code
	ldx	music_head
	cmpa	,x
	ifnz
		sta	,x
		sta	WPCS_DATA
	endif
endp


proc(music_off)
	uses(a)
	clra
	sta	music_stack
	ldx	#music_stack
	stx	music_head
	sta	WPCS_DATA
endp


proc(sound_send_const)
	definline(x,a)
	sta	WPCS_DATA
endp


