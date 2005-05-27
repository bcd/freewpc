
#include "wpc.h"

#define TEST_BANNER_OFFSET				0
#define TEST_ENTER_PROC_OFFSET		2
#define TEST_NEXT_OFFSET				4
#define TEST_START_OFFSET				6

.area ram

test_info::			.blkw 1
test_item::			.blkb 1

.area sysrom

sound_test_banner:	
	.asciz "SOUND TEST   "
sound_test_info:
	.dw	sound_test_banner
	.dw	sound_enter_proc
	.dw	music_test_info
	.db	0x81

music_test_banner:
	.asciz "MUSIC TEST   "
music_test_info:
	.dw	music_test_banner
	.dw	music_enter_proc
	.dw	sol_test_info
	.db	0x00

sol_test_banner:
	.asciz "SOLENOID TEST"
sol_test_info:
	.dw	sol_test_banner
	.dw	sol_enter_proc
	.dw	rtc_test_info
	.db	0x00

rtc_test_banner:
	.asciz "RTC TEST     "
rtc_test_info:
	.dw	rtc_test_banner
	.dw	rtc_enter_proc
	.dw	clock_test_info
	.db	0x00

clock_test_banner:
	.asciz "TZ CLOCK TEST"
clock_test_info:
	.dw	clock_test_banner
	.dw	clock_enter_proc
	.dw	sound_test_info
	.db	0x00


proc(test_init)
	uses(x)
	ldx	#sound_test_info
	stx	test_info
	jsr	test_start
endp


proc(test_start)
	uses(a,b,x)
	ldx	test_info
	ldx	TEST_NEXT_OFFSET,x
	stx	test_info
	ldb	TEST_START_OFFSET,x
	stb	test_item
	jsr	c_sound_send(SND_ENTER)
	jsr	c_task_recreate_gid(test_loop, GID_TEST_LOOP)
endp


proc(test_loop)
	jsr	c_task_recreate_gid(test_deff_proc, GID_DEFF)
	loop
		loop
			jsr	c_task_sleep(TIME_66MS)
			lda	switch_raw_bits
			tsta
		while(eq)

		switch(a)
			case(SW_ROWMASK(SW_ESCAPE))
			endcase

			case(SW_ROWMASK(SW_DOWN))
				jsr	c_sound_send(SND_DOWN)
				decb
			endcase

			case(SW_ROWMASK(SW_UP))
				jsr	c_sound_send(SND_UP)
				incb
			endcase

			case(SW_ROWMASK(SW_ENTER))
				jsr	c_task_kill_gid(GID_DEFF)
				ldx	test_info
				ldx	TEST_ENTER_PROC_OFFSET,x
				jsr	task_create
				jsr	c_task_sleep(TIME_100MS * 5)
			endcase
		endswitch

		stb	test_item
		jsr	c_task_recreate_gid(test_deff_proc, GID_DEFF)

		ldu	#60
		loop
			jsr	c_task_sleep(TIME_16MS)
			leau	-1,u
			ifz
			endif
			lda	switch_raw_bits
		while(nz)
	endloop
endp


proc(test_deff_proc)
	jsr	dmd_alloc_low_high
	jsr	dmd_clean_low

	lda	#SEG_ADDR(0, 2, 1)
	ldb	test_item
	jsr	seg_write_bcd

	jsr	dmd_copy_low_to_high

	lda	#SEG_ADDR(0, 1, 1)
	ldy	test_info
	ldy	TEST_BANNER_OFFSET,y
	jsr	seg_write_string

	loop
		jsr	dmd_show_high
		jsr	c_task_sleep(TIME_100MS)
		jsr	dmd_show_low
		jsr	c_task_sleep(TIME_100MS)
	endloop
endp


sound_deff_msg::	.asciz "SOUND DEFF"

proc(sound_enter_proc)
	/* Write to the sound board */
	stb	WPCS_DATA
	jsr	task_exit
endp


proc(music_enter_proc)
	bsr	sound_enter_proc
endp


proc(sol_enter_proc)
	tfr	b,a
	jsr	sol_on
	jsr	c_task_sleep(TIME_100MS)
	jsr	sol_off
	jmp	task_exit
endp


proc(rtc_enter_proc)
	jsr	dmd_alloc_low_clean

	lda	#SEG_ADDR(0,3,6)
	ldb	WPC_CLK_HOURS_DAYS
	jsr	seg_write_short

	lda	#SEG_ADDR(0,3,9)
	ldb	WPC_CLK_MINS
	jsr	seg_write_short

	jsr	dmd_show_low
	jsr	c_task_sleep(TIME_1S * 2)
	jmp	task_exit
endp


proc(clock_enter_proc)
	switch(a)
		case(0)
			jsr	tz_clock_stop
		endcase
		case(1)
			jsr	tz_clock_start_forward
		endcase
		case(2)
			jsr	tz_clock_start_backward
		endcase
	endswitch
	jmp	task_exit
endp


