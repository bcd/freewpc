	.title	AS6808 Sequential Test

	;	This is the AS6805 Sequential Test file
	;	t6805s.asm updated to include the 68HC08
	;	instructions.

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

begin:	.byte	0			; 00
loca:	.byte	0			; 00
locb:	.byte	0			; 00
locc:	.byte	0			; 00
locd:	.byte	0			; 00
loce:	.byte	0			; 00


	.area	AS6808A

1$:	jsr	18$			; CDs03r6A
	jmp	18$			; CCs03r6A
	sub	18$			; C0s03r6A

	jsr	19$			; CDs00r00
	jmp	20$			; CCs00r03
	sub	21$			; C0s00r06



	. = 1$+256

2$:	brset	#bit0,*loca, .		; 00*01 FD
	brclr	#bit0,*loca,2$		; 01*01 FA
	brset	#bit1,*loca,2$		; 02*01 F7
	brclr	#bit1,*loca,2$		; 03*01 F4
	brset	#bit2,*loca,2$		; 04*01 F1
	brclr	#bit2,*loca,2$		; 05*01 EE
	brset	#bit3,*loca,2$		; 06*01 EB
	brclr	#bit3,*loca,2$		; 07*01 E8
	brset	#bit4,*loca,2$		; 08*01 E5
	brclr	#bit4,*loca,2$		; 09*01 E2
	brset	#bit5,*loca,2$		; 0A*01 DF
	brclr	#bit5,*loca,2$		; 0B*01 DC
	brset	#bit6,*loca,2$		; 0C*01 D9
	brclr	#bit6,*loca,2$		; 0D*01 D6
	brset	#bit7,*loca,2$		; 0E*01 D3
	brclr	#bit7,*loca,2$		; 0F*01 D0

	bset	#bit0,*locb		; 10*02
	bclr	#bit0,*locb		; 11*02
	bset	#bit1,*locb		; 12*02
	bclr	#bit1,*locb		; 13*02
	bset	#bit2,*locb		; 14*02
	bclr	#bit2,*locb		; 15*02
	bset	#bit3,*locb		; 16*02
	bclr	#bit3,*locb		; 17*02
	bset	#bit4,*locb		; 18*02
	bclr	#bit4,*locb		; 19*02
	bset	#bit5,*locb		; 1A*02
	bclr	#bit5,*locb		; 1B*02
	bset	#bit6,*locb		; 1C*02
	bclr	#bit6,*locb		; 1D*02
	bset	#bit7,*locb		; 1E*02
	bclr	#bit7,*locb		; 1F*02

3$:	bra	3$			; 20 FE
	brn	3$			; 21 FC
	bhi	3$			; 22 FA
	bls	3$			; 23 F8
	bcc	3$			; 24 F6
	bcs	3$			; 25 F4
	bne	3$			; 26 F2
	beq	3$			; 27 F0
	bhcc	3$			; 28 EE
	bhcs	3$			; 29 EC
	bpl	3$			; 2A EA
	bmi	3$			; 2B E8
	bmc	3$			; 2C E6
	bms	3$			; 2D E4
	bil	3$			; 2E E2
	bih	3$			; 2F E0



	neg	*locc			; 30*03
; /* HC08
4$:	cbeq	*locc,4$		; 31*03 FD
; */
				;32
	com	*locc			; 33*03
	lsr	*locc			; 34*03
; /* HC08
	sthx	*locc			; 35*03
; */
	ror	*locc			; 36*03
	asr	*locc			; 37*03
	lsl	*locc			; 38*03
	rol	*locc			; 39*03
	dec	*locc			; 3A*03
; /* HC08
5$:	dbnz	*locc,5$		; 3B*03 FD
; */
	inc	*locc			; 3C*03
	tst	*locc			; 3D*03
				;3E
	clr	*locc			; 3F*03

	nega				; 40
; /* HC08
6$:	cbeqa	#0x21,6$		; 41 21 FD
	mul				; 42
; */
	coma				; 43
	lsra				; 44
; /* HC08
	ldhx	#0x21			; 45 00 21
; */
	rora				; 46
	asra				; 47
	lsla				; 48
	rola				; 49
	deca				; 4A
; /* HC08
7$:	dbnza	7$			; 4B FE
; */
	inca				; 4C
	tsta				; 4D
; /* HC08
	mov	*loca,*locb		; 4E*01*02
; */
	clra				; 4F

	negx				; 50
; /* HC08
8$:	cbeqx	#0x21,8$		; 51 21 FD
	div				; 52
; */
	comx				; 53
	lsrx				; 54
; /* HC08
	ldhx	*loca			; 55*01
; */
	rorx				; 56
	asrx				; 57
	lslx				; 58
	rolx				; 59
	decx				; 5A
; /* HC08
9$:	dbnzx	9$			; 5B FE
; */
	incx				; 5C
	tstx				; 5D
; /* HC08
	mov	*loca,x+		; 5E*01
; */
	clrx				; 5F



	neg	locd,x			; 60u04
; /* HC08
10$:	cbeq	4,x+,10$		; 61 04 FD
	nsa				; 62
; */
	com	locd,x			; 63u04
	lsr	locd,x			; 64u04
; /* HC08
	cphx	#0x21			; 65 00 21
; */
	ror	locd,x			; 66u04
	asr	locd,x			; 67u04
	lsl	locd,x			; 68u04
	rol	locd,x			; 69u04
	dec	locd,x			; 6Au04
; /* HC08
11$:	dbnz	locd,x,11$		; 6Bu04 FD
; */
	inc	locd,x			; 6Cu04
	tst	locd,x			; 6Du04
; /* HC08
	mov	#0x21,*loca		; 6E 21*01
; */
	clr	locd,x			; 6Fu04

; /* HC08
	neg	locd,s			; 9E 60u04
12$:	cbeq	locd,s,12$		; 9E 61u04 FC
				;9E 62
	com	locd,s			; 9E 63u04
	lsr	locd,s			; 9E 64u04
				;9E 65
	ror	locd,s			; 9E 66u04
	asr	locd,s			; 9E 67u04
	lsl	locd,s			; 9E 68u04
	rol	locd,s			; 9E 69u04
	dec	locd,s			; 9E 6Au04
13$:	dbnz	locd,s,13$		; 9E 6Bu04 FC
	inc	locd,s			; 9E 6Cu04
	tst	locd,s			; 9E 6Du04
				;9E 6E
	clr	locd,s			; 9E 6Fu04
; */

	neg	,x			; 70
; /* HC08
14$:	cbeq	,x+,14$			; 71 FE
; */
	daa				; 72
	com	,x			; 73
	lsr	,x			; 74
; /* HC08
	cphx	*loca			; 75*01
; */
	ror	,x			; 76
	asr	,x			; 77
	lsl	,x			; 78
	rol	,x			; 79
	dec	,x			; 7A
; /* HC08
15$:	dbnz	,x,15$			; 7B FE
; */
	inc	,x			; 7C
	tst	,x			; 7D
; /* HC08
	mov	,x+,*loca		; 7E*01
; */
	clr	,x			; 7F



	rti				; 80
	rts				; 81
				;82
	swi				; 83
; /* HC08
	tap				; 84
	tpa				; 85
	pula				; 86
	psha				; 87
	pulx				; 88
	pshx				; 89
	pulh				; 8A
	pshh				; 8B
	clrh				; 8C
; */
				;8D 
	stop				; 8E
	wait				; 8F

; /* HC08
16$:	bge	16$			; 90 FE
	blt	16$			; 91 FC
	bgt	16$			; 92 FA
	ble	16$			; 93 F8
	txs				; 94
	tsx				; 95
; */
				;96
	tax				; 97
	clc				; 98
	sec				; 99
	cli				; 9A
	sei				; 9B
	rsp				; 9C
	nop				; 9D
				;9E
	txa				; 9F

				

	sub	#0x21			; A0 21
	cmp	#0x21			; A1 21
	sbc	#0x21			; A2 21
	cpx	#0x21			; A3 21
	and	#0x21			; A4 21
	bit	#0x21			; A5 21
	lda	#0x21			; A6 21
; /* HC08
	ais	#0x21			; A7 21
; */
	eor	#0x21			; A8 21
	adc	#0x21			; A9 21
	ora	#0x21			; AA 21
	add	#0x21			; AB 21
				;AC
17$:	bsr	17$			; AD FE
	ldx	#0x21			; AE 21
; /* HC08
	aix	#0x21			; AF 21
; */

	sub	*loce			; B0*05
	cmp	*loce			; B1*05
	sbc	*loce			; B2*05
	cpx	*loce			; B3*05
	and	*loce			; B4*05
	bit	*loce			; B5*05
	lda	*loce			; B6*05
	sta	*loce			; B7*05
	eor	*loce			; B8*05
	adc	*loce			; B9*05
	ora	*loce			; BA*05
	add	*loce			; BB*05
	jmp	*loce			; BC*05
	jsr	*loce			; BD*05
	ldx	*loce			; BE*05
	stx	*loce			; BF*05

	sub	ext			; C0 01 23
	cmp	ext			; C1 01 23
	sbc	ext			; C2 01 23
	cpx	ext			; C3 01 23
	and	ext			; C4 01 23
	bit	ext			; C5 01 23
	lda	ext			; C6 01 23
	sta	ext			; C7 01 23
	eor	ext			; C8 01 23
	adc	ext			; C9 01 23
	ora	ext			; CA 01 23
	add	ext			; CB 01 23
	jmp	ext			; CC 01 23
	jsr	ext			; CD 01 23
	ldx	ext			; CE 01 23
	stx	ext			; CF 01 23



	sub	ix2,x			; D0 AB CD
	cmp	ix2,x			; D1 AB CD
	sbc	ix2,x			; D2 AB CD
	cpx	ix2,x			; D3 AB CD
	and	ix2,x			; D4 AB CD
	bit	ix2,x			; D5 AB CD
	lda	ix2,x			; D6 AB CD
	sta	ix2,x			; D7 AB CD
	eor	ix2,x			; D8 AB CD
	adc	ix2,x			; D9 AB CD
	ora	ix2,x			; DA AB CD
	add	ix2,x			; DB AB CD
	jmp	ix2,x			; DC AB CD
	jsr	ix2,x			; DD AB CD
	ldx	ix2,x			; DE AB CD
	stx	ix2,x			; DF AB CD

; /* HC08
	sub	ix2,s			; 9E D0 AB CD
	cmp	ix2,s			; 9E D1 AB CD
	sbc	ix2,s			; 9E D2 AB CD
	cpx	ix2,s			; 9E D3 AB CD
	and	ix2,s			; 9E D4 AB CD
	bit	ix2,s			; 9E D5 AB CD
	lda	ix2,s			; 9E D6 AB CD
	sta	ix2,s			; 9E D7 AB CD
	eor	ix2,s			; 9E D8 AB CD
	adc	ix2,s			; 9E D9 AB CD
	ora	ix2,s			; 9E DA AB CD
	add	ix2,s			; 9E DB AB CD
; */
				;9E DC
				;9E DD
; /* HC08
	ldx	ix2,s			; 9E DE AB CD
	stx	ix2,s			; 9E DF AB CD
; */

	sub	ix1,x			; E0 EF
	cmp	ix1,x			; E1 EF
	sbc	ix1,x			; E2 EF
	cpx	ix1,x			; E3 EF
	and	ix1,x			; E4 EF
	bit	ix1,x			; E5 EF
	lda	ix1,x			; E6 EF
	sta	ix1,x			; E7 EF
	eor	ix1,x			; E8 EF
	adc	ix1,x			; E9 EF
	ora	ix1,x			; EA EF
	add	ix1,x			; EB EF
	jmp	ix1,x			; EC EF
	jsr	ix1,x			; ED EF
	ldx	ix1,x			; EE EF
	stx	ix1,x			; EF EF

; /* HC08
	sub	ix1,s			; 9E E0 EF
	cmp	ix1,s			; 9E E1 EF
	sbc	ix1,s			; 9E E2 EF
	cpx	ix1,s			; 9E E3 EF
	and	ix1,s			; 9E E4 EF
	bit	ix1,s			; 9E E5 EF
	lda	ix1,s			; 9E E6 EF
	sta	ix1,s			; 9E E7 EF
	eor	ix1,s			; 9E E8 EF
	adc	ix1,s			; 9E E9 EF
	ora	ix1,s			; 9E EA EF
	add	ix1,s			; 9E EB EF
; */
				;9E EC
				;9E ED
; /* HC08
	ldx	ix1,s			; 9E EE EF
	stx	ix1,s			; 9E EF EF
; */

	sub	,x			; F0
	cmp	,x			; F1
	sbc	,x			; F2
	cpx	,x			; F3
	and	,x			; F4
	bit	,x			; F5
	lda	,x			; F6
	sta	,x			; F7
	eor	,x			; F8
	adc	,x			; F9
	ora	,x			; FA
	add	,x			; FB
	jmp	,x			; FC
	jsr	,x			; FD
	ldx	,x			; FE
	stx	,x			; FF



18$:	jsr	*begin			; BD*00
	jmp	*begin			; BC*00
	sub	*begin			; B0*00

	.area	AS6808B

19$:	jsr	1$			; CDs00r00
20$:	jmp	1$			; CCs00r00
21$:	sub	1$			; C0s00r00

