	.title	AS6808 Error Test

	.area	DIRECT

	.setdp	0,DIRECT

	bit0	=	0
	bit1	=	1
	bit2	=	2
	bit3	=	3
	bit4	=	4
	bit5	=	5
	bit6	=	6
	bit7	=	7

	ext = 0x0123
	ix2 = 0xABCD
	ix1 = 0x00EF

begin:	.byte	0
loca:	.byte	0
locb:	.byte	0
locc:	.byte	0
locd:	.byte	0
loce:	.byte	0


	.area	AS6808A

1$:	jsr	28$		; CD __ __
	jmp	28$		; CC __ __
	sub	28$		; C0 __ __

	jsr	29$		; CD 00 00
	jmp	30$		; CC 00 03
	sub	31$		; C0 00 06


	.page

	. = 1$+256

	; Illegal Modes for:
	;	neg, com, lsr, ror,
	;	asr, lsl, rol, asl,
	;	dec, inc, tst and clr

	neg	,s		;a
	neg	ix2,x		;a (may cause a link error if ix2 is external)
	neg	ix2,s		;a (may cause a link error if ix2 is external)
	neg	,x+		;a
	neg	ix1,x+		;a

	; Illegal Modes for:
	;	sub, cmp, sbc, cpx,
	;	and, bit, lda, sta,
	;	eor, adc, ora, add,
	;	jmp, jsr, ldx, and stx

	sub	,s		;a
	sub	,x+		;a
	sub	ix1,x+		;a

	; Additional Illegal Modes for:
	;	sta and stx

	sta	#0		;a

	; Additional Illegal Modes for:
	;	jmp and jsr

	jmp	#0		;a
	jmp	ix2,s		;a
	jmp	ix1,s		;a
	jmp	,x+		;a
	jmp	ix1,x+		;a

	; Illegal Modes for:
	;	cbeqa and cbeqx

2$:	cbeqa	*loca,2$	;a
3$:	cbeqa	,x,3$		;a
4$:	cbeqa	ix1,x,4$	;a
5$:	cbeqa	ix2,x,5$	;a
6$:	cbeqa	,x+,6$		;a
7$:	cbeqa	ix1,x+,7$	;a
8$:	cbeqa	ix1,s,8$	;a
9$:	cbeqa	ix2,s,9$	;a

	; Illegal Modes for:
	;	cbeq

10$:	cbeq	#0x21,10$	;a
11$:	cbeq	,x,11$		;a
12$:	cbeq	ix1,x,12$	;a
13$:	cbeq	ix2,x,13$	;a (may cause link error if ix2 is external)
14$:	cbeq	ix2,s,14$	;a (may cause link error if ix2 is external)

	; Illegal Modes for:
	;	dbnza and dbnzx

15$:	dbnza	*loca,15$	;q
16$:	dbnza	,x,16$		;q
17$:	dbnza	ix1,x,17$	;q
18$:	dbnza	ix2,x,18$	;aq
19$:	dbnza	,x+,19$		;q
20$:	dbnza	ix1,x+,20$	;q
21$:	dbnza	ix1,s,21$	;q
22$:	dbnza	ix2,s,22$	;aq

	; Illegal Modes for:
	;	dbnz

23$:	dbnz	#0x21,23$	;a
24$:	dbnz	,x+,24$		;a
25$:	dbnz	ix1,x+,25$	;a
26$:	dbnz	ix2,x+,26$	;a
27$:	dbnz	ix2,s,27$	;a (may cause link error if ix2 is external)


	.page

28$:	jsr	*begin		; BD __
	jmp	*begin		; BC __
	sub	*begin		; B0 __

	.area	AS6808B

29$:	jsr	1$		; CD __ __
30$:	jmp	1$		; CC __ __
31$:	sub	1$		; C0 __ __

