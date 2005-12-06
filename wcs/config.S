;;; gcc built Dec  5 2005 22:46:30
;;; MC6809 $Revision: 1.9 $
;;; Source:	wcs/config.c
;;; Destination:	wcs/config.S
;;; Compiled:	Mon Dec  5 23:53:35 2005
;;; (META)compiled by GNU C version 3.3.6 (Gentoo 3.3.6, ssp-3.3.6-1.0, pie-8.7.8).
;;; OPTIONS:	-mint8 -mshort-branch
;;; OPTIONS:	 signed-char
	.module	config.c
	; extern	___lda
	; extern	___sta
	; extern	_memcpy
	; extern	_task_current
	.globl _mach_opto_mask
	.area sysrom
_mach_opto_mask:
	.byte	0
	.byte	0
	.byte	8
	.byte	63
	.byte	31
	.byte	7
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.globl _mach_edge_switches
_mach_edge_switches:
	.byte	0
	.byte	0
	.byte	0
	.byte	31
	.byte	18
	.byte	56
	.byte	0
	.byte	-64
	.byte	0
	.byte	0
	; extern	_unused_init
	.globl _funhouse_init
_funhouse_init:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_init
;;;-----------------------------------------
;;;END PROLOGUE
	jsr	_unused_init	;CALL: (VOIDmode) _unused_init (0 bytes)
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_init
;;;-----------------------------------------


	.globl _unused_init
_unused_init:
;;;-----------------------------------------
;;;  PROLOGUE for unused_init
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for unused_init
;;;-----------------------------------------


	.globl _unused_start_game
_unused_start_game:
;;;-----------------------------------------
;;;  PROLOGUE for unused_start_game
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for unused_start_game
;;;-----------------------------------------


	.globl _unused_end_game
_unused_end_game:
;;;-----------------------------------------
;;;  PROLOGUE for unused_end_game
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for unused_end_game
;;;-----------------------------------------


	.globl _unused_start_ball
_unused_start_ball:
;;;-----------------------------------------
;;;  PROLOGUE for unused_start_ball
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for unused_start_ball
;;;-----------------------------------------


	; extern	_unused_start_game
	; extern	_task_sleep_sec
	.globl _funhouse_start_game
_funhouse_start_game:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_start_game
;;;-----------------------------------------
;;;END PROLOGUE
	jsr	_unused_start_game	;CALL: (VOIDmode) _unused_start_game (0 bytes)
	ldb	#2	;movqi: #2 -> R:b
	jsr	_task_sleep_sec	;CALL: (VOIDmode) _task_sleep_sec (0 bytes)
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_start_game
;;;-----------------------------------------


	; extern	_unused_end_game
	; extern	_in_test
	; extern	_music_set
	.globl _funhouse_end_game
_funhouse_end_game:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_end_game
;;;-----------------------------------------
;;;END PROLOGUE
	jsr	_unused_end_game	;CALL: (VOIDmode) _unused_end_game (0 bytes)
	tst	_in_test	;tstqi: MEM:_in_test
	bne	L7
	ldd	#9	;movhi: #9 -> R:d
	jsr	_music_set	;CALL: (VOIDmode) _music_set (0 bytes)
L7:
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_end_game
;;;-----------------------------------------


	; extern	_unused_start_ball
	.globl _funhouse_start_ball
_funhouse_start_ball:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_start_ball
;;;-----------------------------------------
;;;END PROLOGUE
	ldd	#17	;movhi: #17 -> R:d
	jsr	_music_set	;CALL: (VOIDmode) _music_set (0 bytes)
	jsr	_unused_start_ball	;CALL: (VOIDmode) _unused_start_ball (0 bytes)
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_start_ball
;;;-----------------------------------------


	.globl _funhouse_ball_in_play
_funhouse_ball_in_play:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_ball_in_play
;;;-----------------------------------------
;;;END PROLOGUE
	ldd	#18	;movhi: #18 -> R:d
	jsr	_music_set	;CALL: (VOIDmode) _music_set (0 bytes)
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_ball_in_play
;;;-----------------------------------------


	; extern	_sound_reset
	.globl _funhouse_end_ball
_funhouse_end_ball:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_end_ball
;;;-----------------------------------------
;;;END PROLOGUE
	jsr	_sound_reset	;CALL: (VOIDmode) _sound_reset (0 bytes)
	ldb	#1	;movqi: #1 -> R:b
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_end_ball
;;;-----------------------------------------


	.globl _funhouse_add_player
_funhouse_add_player:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_add_player
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_add_player
;;;-----------------------------------------


	.globl _funhouse_any_pf_switch
_funhouse_any_pf_switch:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_any_pf_switch
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_any_pf_switch
;;;-----------------------------------------


	; extern	_deff_start
	; extern	_leff_start
	; extern	_deff_get_active
	; extern	_task_sleep
	; extern	_leff_stop
	.globl _funhouse_bonus
_funhouse_bonus:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_bonus
;;;-----------------------------------------
;;;END PROLOGUE
	ldb	#3	;movqi: #3 -> R:b
	jsr	_deff_start	;CALL: (VOIDmode) _deff_start (0 bytes)
	ldb	#6	;movqi: #6 -> R:b
	jsr	_leff_start	;CALL: (VOIDmode) _leff_start (0 bytes)
	ldb	#1	;movqi: #1 -> R:b
	jsr	_task_sleep_sec	;CALL: (VOIDmode) _task_sleep_sec (0 bytes)
	jmp	L15
L18:
	ldb	#4	;movqi: #4 -> R:b
	jsr	_task_sleep	;CALL: (VOIDmode) _task_sleep (0 bytes)
L15:
	jsr	_deff_get_active	;CALL: R:b = _deff_get_active (0 bytes)
	cmpb	#3	;cmpqi:
	beq	L18
	ldb	#6	;movqi: #6 -> R:b
	jsr	_leff_stop	;CALL: (VOIDmode) _leff_stop (0 bytes)
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_bonus
;;;-----------------------------------------


	.globl _funhouse_tilt
_funhouse_tilt:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_tilt
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_tilt
;;;-----------------------------------------


	.globl _funhouse_tilt_warning
_funhouse_tilt_warning:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_tilt_warning
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_tilt_warning
;;;-----------------------------------------


	.globl _funhouse_start_without_credits
_funhouse_start_without_credits:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_start_without_credits
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_start_without_credits
;;;-----------------------------------------


	.globl _funhouse_coin_added
_funhouse_coin_added:
;;;-----------------------------------------
;;;  PROLOGUE for funhouse_coin_added
;;;-----------------------------------------
;;;END PROLOGUE
;;;EPILOGUE
	rts		; return from function
;;;-----------------------------------------
;;; END EPILOGUE for funhouse_coin_added
;;;-----------------------------------------


	.globl _funhouse_hooks
	.area sysrom
_funhouse_hooks:
	.word	_funhouse_init
	.blkb	2	;reserve space
	.word	_funhouse_start_game
	.word	_funhouse_add_player
	.word	_funhouse_start_ball
	.word	_funhouse_ball_in_play
	.blkb	2	;reserve space
	.word	_funhouse_end_ball
	.word	_funhouse_bonus
	.blkb	2	;reserve space
	.word	_funhouse_end_game
	.blkb	4	;reserve space
	.word	_funhouse_tilt_warning
	.word	_funhouse_tilt
	.blkb	8	;reserve space
	.word	_funhouse_coin_added
	.blkb	2	;reserve space
	.word	_funhouse_start_without_credits
	.blkb	4	;reserve space
	.word	_funhouse_any_pf_switch
	.blkb	2	;reserve space
