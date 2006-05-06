	.title	Machine Independent Assembler Test
	.module Asmtst

	;  The file 'asmtl.asm' must be assembled with
	; a LO/HI assembler (in file ___ext.c, hilo = 0).
	;
	;  The file 'asmth.asm' must be assembled with
	; a HI/LO assembler (in file ___ext.c, hilo = 1).
	
	.sbttl	Memory Allocation Directives

	.radix	O			; set default to octal

					; binary constants
	.byte	0b11000000		; C0
	.byte	0B1110			; 0E
	.byte	$%11000000		; C0

					; octal constants
	.byte	24			; 14
	.byte	024			; 14
	.byte	0q024			; 14
	.byte	0Q024			; 14
	.byte	0o024			; 14
	.byte	0O024			; 14
	.byte	$&24			; 14
	
					; decimal constant
	.byte	0d024			; 18
	.byte	0D024			; 18
	.byte	$#24			; 18

					; hexidecimal constants
	.byte	0h024			; 24
	.byte	0H024			; 24
	.byte	0x024			; 24
	.byte	0X024			; 24
	.byte	$$24			; 24

	.db	0			; 00
	.dw	0			; 00 00

	.radix	D			; set default to decimal

	.byte	1,2,3			; 01 02 03
	.byte	4,5,6			; 04 05 06
	.byte	7,8,9			; 07 08 09
	.word	1,2,3			; 00 01 00 02 00 03
	.word	4,5,6			; 00 04 00 05 00 06
	.word	7,8,9			; 00 07 00 08 00 09

	.blkb	16
	.ds	16

	.blkw	16
	.ds	16*2

word:	.word	.+2			;s00r92
	.word	.-2			;s00r90
	.word	2+.			;s00r96
	.word	.-(word+2)		; 00 04


	.sbttl	Boundary Directives

	.even
	.byte	0			; 00
	.even
	.byte	1			; 01
	.even
	.odd
	.byte	2			; 02
	.odd
	.byte	3			; 03
	.odd
	.even
	.word	4			; 00 04
	.odd
	.word	5			; 00 05
	.even
	.word	6			; 00 06


	.sbttl	String Directives

	.ascii	"abcde"			; 61 62 63 64 65
	.asciz	"abcde"			; 61 62 63 64 65 00
	.ascis	"abcde"			; 61 62 63 64 E5
	

	.sbttl	Expression Evaluation

	n0x00	=	0x00
	n0x01	=	0x01
	n0x10	=	0x10
	n0xff	=	0xff
	n0xeeff =	0xeeff


	n	=	< n0xeeff	; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n	=	> n0xeeff	; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	'A		; 0x41		single character
	.byte	>n,<n			; 00 41
	n	=	"AB		; 0x4142	double character
	.byte	>n,<n			; 41 42
	n	=	n0x01		; 0x01		assignment
	.byte	>n,<n			; 00 01
	n	=	n + n0x01	; 0x02		addition
	.byte	>n,<n			; 00 02
	n	=	n - n0x01	; 0x01		subtraction
	.byte	>n,<n			; 00 01
	n	=	n * 0x05	; 0x05		multiplication
	.byte	>n,<n			; 00 05
	n	=	n / 0x02	; 0x02		division
	.byte	>n,<n			; 00 02
	n	=	n0x10 % 0x05	; 0x01		modulus
	.byte	>n,<n			; 00 01
	n	=	n0x10 | n0x01	; 0x11		or
	.byte	>n,<n			; 00 11
	n	=	n0xff & n0x01	; 0x01		and
	.byte	>n,<n			; 00 01
	n	=	n0x01 << 4	; 0x10		left shift
	.byte	>n,<n			; 00 10
	n	=	n0x10 >> 4	; 0x01		right shift
	.byte	>n,<n			; 00 01
	n	=	n0xff ^ n0x10	; 0xef		xor
	.byte	>n,<n			; 00 EF
	n	=	n ^ n0x10	; 0xff		xor
	.byte	>n,<n			; 00 FF
	n	=	~n0x10		; 0xffef	1's complement
	.byte	>n,<n			; FF EF
	n	=	-n0x10		; 0xfff0	2's complement
	.byte	>n,<n			; FF F0



	n	=	n0xeeff & 0xff	; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n = (n0xeeff & 0xff00)/0x100	; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	n0xeeff % 0x100 ; 0xff		low byte
	.byte	>n,<n			; 00 FF
	n	=	n0xeeff / 0x100 ; 0xee		high byte
	.byte	>n,<n			; 00 EE


	n	=	3*(2 + 4*(6))	; 0x4e		expression evaluation
	.byte	>n,<n			; 00 4E
	n	=	2*(0x20 + <~n0x10)	; 0x21e
	.byte	>n,<n			; 02 1E


	.sbttl	arithmatic tests

	;
	; The following series of tests verify that
	; the arithmetic is 16-Bit and unsigned.


	.sbttl	Addition

	n =	 1 +  1			; 0x0001 + 0x0001 ; 0x0002
	.byte	>n,<n			; 00 02
	n =	-1 +  1			; 0xFFFF + 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 + -1			; 0xFFFF + 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE

	n =	 32768 +  32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 +  32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 + -32768		; 0x8000 + 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 65535 +  1		; 0xFFFF + 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65535 + -1		; 0xFFFF + 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE


	.sbttl	Subtraction

	n =	 1 -  1			; 0x0001 - 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 -  1			; 0xFFFF - 0x0001 ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	-1 - -1			; 0xFFFF - 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 32768 -  32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 -  32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 - -32768		; 0x8000 - 0x8000 ; 0x0000
	.byte	>n,<n			; 00 00

	n =	 65535 -  1		; 0xFFFF - 0x0001 ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	 65535 - -1		; 0xFFFF - 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Multiplication

	n =	 1 *  1			; 0x0001 * 0x0001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	 1 * -1			; 0x0001 * 0xFFFF ; 0xFFFF
	.byte	>n,<n			; FF FF
	n =	-1 * -1			; 0xFFFF * 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01

	n =	 256 *  256		; 0x0100 * 0x0100 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 256 * -256		; 0x0100 * 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 * -256		; 0xFF00 * 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Division

	n =	 1 /  1			; 0x0001 / 0x0001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	10 /  2			; 0x000A / 0x0002 ; 0x0005
	.byte	>n,<n			; 00 05
	n =	512 / 4			; 0x0200 / 0x0004 ; 0x0080
	.byte	>n,<n			; 00 80
	n =	32768 / 2		; 0x8000 / 0x0002 ; 0x4000
	.byte	>n,<n			; 40 00
	n =	65535 / 2		; 0xFFFF / 0x0002 ; 0x7FFF
	.byte	>n,<n			; 7F FF

	n = 	 1 / -1			; 0x0001 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 32767 / -1		; 0x7FFF / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 32768 / -1		; 0x8000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65534 / -1		; 0xFFFE / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	 65535 / -1		; 0xFFFF / 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-1 / -1			; 0xFFFF / 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-2 / -1			; 0xFFFE / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32768 / -1		; 0x8000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-32769 / -1		; 0x7FFF / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-65535 / -1		; 0x0001 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-65536 / -1		; 0x0000 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	-256 /   -1		; 0xFF00 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 / -255		; 0xFF00 / 0xFF01 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 / -256		; 0xFF00 / 0xFF00 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -257		; 0xFF00 / 0xFEFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -32767		; 0xFF00 / 0x8001 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 / -32768		; 0xFF00 / 0x8000 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  32768		; 0xFF00 / 0x8000 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  65280		; 0xFF00 / 0xFF00 ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 /  65281		; 0xFF00 / 0xFF01 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 /  65535		; 0xFF00 / 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00


	.sbttl	Modulus

	n =	 1 %  1			; 0x0001 % 0x0001 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	10 %  2			; 0x000A % 0x0002 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	512 % 4			; 0x0200 % 0x0004 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	32768 % 2		; 0x8000 % 0x0002 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	65535 % 2		; 0xFFFF % 0x0002 ; 0x0001
	.byte	>n,<n			; 00 01

	n = 	 1 % -1			; 0x0001 % 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	 32767 % -1		; 0x7FFF % 0xFFFF ; 0x7FFF
	.byte	>n,<n			; 7F FF
	n =	 32768 % -1		; 0x8000 % 0xFFFF ; 0x8000
	.byte	>n,<n			; 80 00
	n =	 65534 % -1		; 0xFFFE % 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	 65535 % -1		; 0xFFFF % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-1 % -1			; 0xFFFF % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-2 % -1			; 0xFFFE % 0xFFFF ; 0xFFFE
	.byte	>n,<n			; FF FE
	n =	-32768 % -1		; 0x8000 % 0xFFFF ; 0x8000
	.byte	>n,<n			; 80 00
	n =	-32769 % -1		; 0x7FFF % 0xFFFF ; 0x7FFF
	.byte	>n,<n			; 7F FF
	n =	-65535 % -1		; 0x0001 % 0xFFFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-65536 % -1		; 0x0000 % 0xFFFF ; 0x0000
	.byte	>n,<n			; 00 00

	n =	-256 %   -1		; 0xFF00 % 0xFFFF ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -255		; 0xFF00 % 0xFF01 ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -256		; 0xFF00 % 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 % -257		; 0xFF00 % 0xFEFF ; 0x0001
	.byte	>n,<n			; 00 01
	n =	-256 % -32767		; 0xFF00 % 0x8001 ; 0x7EFF
	.byte	>n,<n			; 7E FF
	n =	-256 % -32768		; 0xFF00 % 0x8000 ; 0x7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  32768		; 0xFF00 % 0x8000 ; 0x7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  65280		; 0xFF00 % 0xFF00 ; 0x0000
	.byte	>n,<n			; 00 00
	n =	-256 %  65281		; 0xFF00 % 0xFF01 ; 0xFF00
	.byte	>n,<n			; FF 00
	n =	-256 %  65535		; 0xFF00 % 0xFFFF ; 0xFF00
	.byte	>n,<n			; FF 00


	.sbttl	IF, ELSE, and ENDIF

	n = 0
	m = 0

	.if	0
		n = 1
		.if	0
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 2, m = 0
	.byte	n,m			; 02 00

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
		.if	0
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 1, m = 2
	.byte	n,m			; 01 02



	n = 0
	m = 0

	.if	0
		n = 1
		.if	1
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 2, m = 0
	.byte	n,m			; 02 00

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
		.if	1
		m = 1
		.else
		m = 2
		.endif
	.else
		n = 2
	.endif

					; n = 1, m = 1
	.byte	n,m			; 01 01



	n = 0
	m = 0

	.if	0
		n = 1
	.else
		.if	0
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 2, m = 2
	.byte	n,m			; 02 02

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
	.else
		.if	0
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 1, m = 0
	.byte	n,m			; 01 00



	n = 0
	m = 0

	.if	0
		n = 1
	.else
		.if	1
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 2, m = 1
	.byte	n,m			; 02 01

	;*******************************************************

	n = 0
	m = 0

	.if	1
		n = 1
	.else
		.if	1
		m = 1
		.else
		m = 2
		.endif
		n = 2
	.endif

					; n = 1, m = 0
	.byte	n,m			; 01 00


	.sbttl	Local Symbols

lclsym0:
					; forward references
	.word	0$,1$			;s01rA2s01rA4
	.word	2$,3$			;s01rA6s01rA8
	.word	4$,5$			;s01rAAs01rAC
	.word	6$,7$			;s01rAEs01rB0
	.word	8$,9$			;s01rB2s01rB4

0$:	.word	9$			;s01rB4
1$:	.word	8$			;s01rB2
2$:	.word	7$			;s01rB0
3$:	.word	6$			;s01rAE
4$:	.word	5$			;s01rAC
5$:	.word	4$			;s01rAA
6$:	.word	3$			;s01rA8
7$:	.word	2$			;s01rA6
8$:	.word	1$			;s01rA4
9$:	.word	0$			;s01rA2
10$:

					; backward references
	.word	0$,1$			;s01rA2s01rA4
	.word	2$,3$			;s01rA6s01rA8
	.word	4$,5$			;s01rAAs01rAC
	.word	6$,7$			;s01rAEs01rB0
	.word	8$,9$			;s01rB2s01rB4

lclsym1:
					; forward references
	.word	0$,1$			;s01rDEs01rE0
	.word	2$,3$			;s01rE2s01rE4
	.word	4$,5$			;s01rE6s01rE8
	.word	6$,7$			;s01rEAs01rEC
	.word	8$,9$			;s01rEEs01rF0

0$:	.word	9$			;s01rF0
1$:	.word	8$			;s01rEE
2$:	.word	7$			;s01rEC
3$:	.word	6$			;s01rEA
4$:	.word	5$			;s01rE8
5$:	.word	4$			;s01rE6
6$:	.word	3$			;s01rE4
7$:	.word	2$			;s01rE2
8$:	.word	1$			;s01rE0
9$:	.word	0$			;s01rDE
10$:

					; backward references
	.word	0$,1$			;s01rDEs01rE0
	.word	2$,3$			;s01rE2s01rE4
	.word	4$,5$			;s01rE6s01rE8
	.word	6$,7$			;s01rEAs01rEC
	.word	8$,9$			;s01rEEs01rF0

	.sbttl	Offset calculations

	ofsbyte	=	(10$-0$)	; 0x0014
	ofsword	=	ofsbyte/2	; 0x000A

					; 1$ + 0x001E
	.word	1$+ofsbyte+ofsword	;s01rFE

	.sbttl	Lower/Upper Byte Selections

	.globl	extern

					; low byte
	.byte	< (extern + 0x0001)	;r01

					; low byte
	.byte	< (extern + 0x0200)	;r00

					; high byte 
	.byte	> (extern + 0x0003)	;s00

					; high byte
	.byte	> (extern + 0x0400)	;s04


	.sbttl	Area Definitions

	.globl	code0
	.globl	cnstnt1,cnstnt2

	cnstnt0 == 0xabcd		; global equate

code0:	.word	a0			;s00r00
	.word	cnstnt0			; AB CD

	.area	AreaA (OVR)

	cnstnt1 = 0x1234

a0:	.word	0x00ff			; 00 FF

	.area	AreaB (ABS,OVR)

	cnstnt2 = 0x5678

	.word	a1			;s00r00

	.area	AreaA

	.=.+0x0020
	.word	a2			;s00r00

	.area	AreaB
	.org	0x40

	.word	a0,a1,a2		;s00r00s00r00s00r00
	.word	AreaB,OVR		;s00r00s00r00

abcdabcd::				; global symbol

	.sbttl	Assembler Output File asmt_.sym

;       assembled by:
;                asxxxx -glosxff asmt_.asm
;
; symbol table
;
;    .__.ABS.=  0000 G   |     AreaB      **** GX  |     OVR        **** GX
;  1 a0         0000 R   |     a1         **** GX  |     a2         **** GX
;  2 abcdabcd   004A GR  |     cnstnt0 =  ABCD G   |     cnstnt1 =  1234 G
;    cnstnt2 =  5678 G   |   0 code0      020C GR  |     extern     **** GX
;  0 lclsym0    018E R   |   0 lclsym1    01CA R   |     m       =  0000 
;    n       =  0001     |     n0x00   =  0000     |     n0x01   =  0001 
;    n0x10   =  0010     |     n0xeeff =  EEFF     |     n0xff   =  00FF 
;    ofsbyte =  0014     |     ofsword =  000A     |   0 word       0090 R
;
; area table
;
;   0 _CODE      size  210   flags 0
;   1 A          size   24   flags 4
;   2 B          size   4A   flags C
;
