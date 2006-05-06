	.title	Machine Independent Assembler Test
	.module Asmtst

	;  The file 'asmt3l.asm' must be assembled with
	; a LO/HI assembler (in file ___ext.c, hilo = 0).
	;
	;  The file 'asmt3h.asm' must be assembled with
	; a HI/LO assembler (in file ___ext.c, hilo = 1).
	
	.sbttl	Memory Allocation Directives

	.radix	O			; set default to octal
	.24bit				; 24-bit addressing

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
byte3:	.3byte	.+2			;R00s00r9A
	.3byte	.-2			;R00s00r99
	.3byte	2+.			;R00s00rA0
	.3byte	.-(byte3+3)		; 00 00 06
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
	n =	 1 +  1			; 0x000001 + 0x000001 ; 0x000002
	.byte	>n,<n			; 00 02
	n =	-1 +  1			; 0xFFFFFF + 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 + -1			; 0xFFFFFF + 0xFFFFFF ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	 32768 +  32768		; 0x008000 + 0x008000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 +  32768		; 0xFF8000 + 0x008000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 + -32768		; 0xFF8000 + 0xFF8000 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	 65535 +  1		; 0x00FFFF + 0x000001 ; 0x010000
	.byte	>n,<n			; 00 00
	n =	 65535 + -1		; 0x00FFFF + 0xFFFFFF ; 0x00FFFE
	.byte	>n,<n			; FF FE
	.sbttl	Subtraction
	n =	 1 -  1			; 0x000001 - 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 -  1			; 0xFFFFFF - 0x000001 ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	-1 - -1			; 0xFFFFFF - 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32768 -  32768		; 0x008000 - 0xFF8000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 -  32768		; 0xFF8000 - 0x008000 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-32768 - -32768		; 0xFF8000 - 0xFF8000 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65535 -  1		; 0x00FFFF - 0x000001 ; 0x00FFFE
	.byte	>n,<n			; FF FE
	n =	 65535 - -1		; 0x00FFFF - 0xFFFFFF ; 0x010000
	.byte	>n,<n			; 00 00
	.sbttl	Multiplication
	n =	 1 *  1			; 0x000001 * 0x000001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 1 * -1			; 0x000001 * 0xFFFFFF ; 0xFFFFFF
	.byte	>n,<n			; FF FF
	n =	-1 * -1			; 0xFFFFFF * 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 256 *  256		; 0x000100 * 0x000100 ; 0x010000
	.byte	>n,<n			; 00 00
	n =	 256 * -256		; 0x000100 * 0xFFFF00 ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-256 * -256		; 0xFFFF00 * 0xFFFF00 ; 0xFF0000
	.byte	>n,<n			; 00 00
	.sbttl	Division
	n =	 1 /  1			; 0x000001 / 0x000001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	10 /  2			; 0x00000A / 0x000002 ; 0x000005
	.byte	>n,<n			; 00 05
	n =	512 / 4			; 0x000200 / 0x000004 ; 0x000080
	.byte	>n,<n			; 00 80
	n =	32768 / 2		; 0x008000 / 0x000002 ; 0x004000
	.byte	>n,<n			; 40 00
	n =	65535 / 2		; 0x00FFFF / 0x000002 ; 0x007FFF
	.byte	>n,<n			; 7F FF
	n = 	 1 / -1			; 0x000001 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32767 / -1		; 0x007FFF / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 32768 / -1		; 0x008000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65534 / -1		; 0x00FFFE / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	 65535 / -1		; 0x00FFFF / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-1 / -1			; 0xFFFFFF / 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-2 / -1			; 0xFFFFFE / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32768 / -1		; 0xFF8000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-32769 / -1		; 0xFF7FFF / OxFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-65535 / -1		; 0xFF0001 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-65536 / -1		; 0xFF0000 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 /   -1		; 0xFFFF00 / 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 / -255		; 0xFFFF00 / 0xFFFF01 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 / -256		; 0xFFFF00 / 0xFFFF00 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -257		; 0xFFFF00 / 0xFFFEFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -32767		; 0xFFFF00 / 0xFF8001 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 / -32768		; 0xFFFF00 / 0xFF8000 ; 0x000001
	.byte	>n,<n			; 00 01
	n =	-256 /  32768		; 0xFFFF00 / 0x008000 ; 0x0001FF
	.byte	>n,<n			; 01 FF
	n =	-256 /  65280		; 0xFFFF00 / 0x00FF00 ; 0x000101
	.byte	>n,<n			; 01 01
	n =	-256 /  65281		; 0xFFFF00 / 0x00FF01 ; 0x000100
	.byte	>n,<n			; 01 00
	n =	-256 /  65535		; 0xFFFF00 / 0x00FFFF ; 0x000100
	.byte	>n,<n			; 01 00
	.sbttl	Modulus
	n =	 1 %  1			; 0x000001 % 0x000001 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	10 %  2			; 0x00000A % 0x000002 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	512 % 4			; 0x000200 % 0x000004 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	32768 % 2		; 0x008000 % 0x000002 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	65535 % 2		; 0x00FFFF % 0x000002 ; 0x000001
	.byte	>n,<n			; 00 01
	n = 	 1 % -1			; 0x000001 % 0xFFFFFF ; 0x000001
	.byte	>n,<n			; 00 01
	n =	 32767 % -1		; 0x007FFF % 0xFFFFFF ; 0x007FFF
	.byte	>n,<n			; 7F FF
	n =	 32768 % -1		; 0x008000 % 0xFFFFFF ; 0x008000
	.byte	>n,<n			; 80 00
	n =	 65534 % -1		; 0x00FFFE % 0xFFFFFF ; 0x00FFFE
	.byte	>n,<n			; FF FE
	n =	 65535 % -1		; 0x00FFFF % 0xFFFFFF ; 0x00FFFF
	.byte	>n,<n			; FF FF
	n =	-1 % -1			; 0xFFFFFF % 0xFFFFFF ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-2 % -1			; 0xFFFFFE % 0xFFFFFF ; 0xFFFFFE
	.byte	>n,<n			; FF FE
	n =	-32768 % -1		; 0xFF8000 % 0xFFFFFF ; 0xFF8000
	.byte	>n,<n			; 80 00
	n =	-32769 % -1		; 0xFF7FFF % 0xFFFFFF ; 0xFF7FFF
	.byte	>n,<n			; 7F FF
	n =	-65535 % -1		; 0xFF0001 % 0xFFFFFF ; 0xFF0001
	.byte	>n,<n			; 00 01
	n =	-65536 % -1		; 0xFF0000 % 0xFFFFFF ; 0xFF0000
	.byte	>n,<n			; 00 00
	n =	-256 %   -1		; 0xFFFF00 % 0xFFFFFF ; 0XFFFFF0
	.byte	>n,<n			; FF 00
	n =	-256 % -255		; 0xFFFF00 % 0xFFFF01 ; 0xFFFF00
	.byte	>n,<n			; FF 00
	n =	-256 % -256		; 0xFFFF00 % 0xFFFF00 ; 0X000000
	.byte	>n,<n			; 00 00
	n =	-256 % -257		; 0xFFFF00 % 0XFFFF01 ; 0xFF0001
	.byte	>n,<n			; 00 01
	n =	-256 % -32767		; 0xFFFF00 % 0xFF8001 ; 0x007EFF
	.byte	>n,<n			; 7E FF
	n =	-256 % -32768		; 0xFFFF00 % 0xFF8000 ; 0xFF7F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  32768		; 0xFFFF00 % 0x008000 ; 0x007F00
	.byte	>n,<n			; 7F 00
	n =	-256 %  65280		; 0xFFFF00 % 0x00FF00 ; 0x000000
	.byte	>n,<n			; 00 00
	n =	-256 %  65281		; 0xFFFF00 % 0x00FF01 ; 0x00FE00
	.byte	>n,<n			; FE 00
	n =	-256 %  65535		; 0xFFFF00 % 0x00FFFF ; 0x000000
	.byte	>n,<n			; 00 00
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
	.word	0$,1$			;s01rAEs01rB0
	.word	2$,3$			;s01rB2s01rB4
	.word	4$,5$			;s01rB6s01rB8
	.word	6$,7$			;s01rBAs01rBC
	.word	8$,9$			;s01rBEs01rC0
0$:	.word	9$			;s01rC0
1$:	.word	8$			;s01rBE
2$:	.word	7$			;s01rBC
3$:	.word	6$			;s01rBA
4$:	.word	5$			;s01rB8
5$:	.word	4$			;s01rB6
6$:	.word	3$			;s01rB4
7$:	.word	2$			;s01rB2
8$:	.word	1$			;s01rB0
9$:	.word	0$			;s01rAE
10$:
					; backward references
	.word	0$,1$			;s01rAEs01rB0
	.word	2$,3$			;s01rB2s01rB4
	.word	4$,5$			;s01rB6s01rB8
	.word	6$,7$			;s01rBAs01rBC
	.word	8$,9$			;s01rBEs01rC0
lclsym1:
					; forward references
	.word	0$,1$			;s01rEAs01rEC
	.word	2$,3$			;s01rEEs01rF0
	.word	4$,5$			;s01rF2s01rF4
	.word	6$,7$			;s01rF6s01rF8
	.word	8$,9$			;s01rFAs01rFC
0$:	.word	9$			;s01rFC
1$:	.word	8$			;s01rFA
2$:	.word	7$			;s01rF8
3$:	.word	6$			;s01rF6
4$:	.word	5$			;s01rF4
5$:	.word	4$			;s01rF2
6$:	.word	3$			;s01rF0
7$:	.word	2$			;s01rEE
8$:	.word	1$			;s01rEC
9$:	.word	0$			;s01rEA
10$:
					; backward references
	.word	0$,1$			;s01rEAs01rEC
	.word	2$,3$			;s01rEEs01rF0
	.word	4$,5$			;s01rF2s01rF4
	.word	6$,7$			;s01rF6s01rF8
	.word	8$,9$			;s01rFAs01rFC
	.sbttl	Offset calculations
	ofsbyte	=	(10$-0$)	; 0x0014
	ofsword	=	ofsbyte/2	; 0x000A
					; 1$ + 0x001E
	.word	1$+ofsbyte+ofsword	;s02r0A
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

	.sbttl	Assembler Output File asmt3_.sym
;       assembled by:
;                asxxxx -gloxff asmt3_.asm
;
;Symbol Table
;
;    .__.ABS.       =   000000 G   |     AreaB              ****** GX
;  1 a0                 000000 GR  |     a1                 ****** GX
;    a2                 ****** GX  |   2 abcdabcd           00004A GR
;  0 byte3              000098 GR  |     cnstnt0        =   00ABCD G
;    cnstnt1        =   001234 G   |     cnstnt2        =   005678 G
;  0 code0              000218 GR  |     extern             ****** GX
;  0 lclsym0            00019A GR  |   0 lclsym1            0001D6 GR
;    m              =   000000 G   |     n              =   000001 G
;    n0x00          =   000000 G   |     n0x01          =   000001 G
;    n0x10          =   000010 G   |     n0xeeff        =   00EEFF G
;    n0xff          =   0000FF G   |     ofsbyte        =   000014 G
;    ofsword        =   00000A G   |   0 word               000090 GR
;
;Area Table
;
;   0 _CODE            size    21C   flags   0
;   1 AreaA            size     24   flags   4
;   2 AreaB            size     4A   flags   C

