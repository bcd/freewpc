	.title	6500 Assembler Test

	dir	=	0x0033
	ext	=	0x1122
	offset	=	0x0044
	
	.globl	extext
	.globl	extdir

;
; All documented 650X, 651X, 65F11, 65F12, 65C00/21, 65C29,
; 65C02, 65C102, and 65C112 instructions with proper AS6500 syntax.
;

 	; Set Default Radix
	.radix x

	r6500	= 0
	r65f11	= 0
	r65c00	= 0
	r65c02	= 1
	
;	Enable 6500 Core Instructions Only
.if r6500
	.r6500
.endif

;	Enable 6500 Core Plus 65F11 / 65F12 Instructions
.if r65f11
	.r65f11
.endif

;	Enable 6500 Core Plus 65C00/21 and 65C29 Instructions
.if r65c00
	.r65c00
.endif

;	Enable 6500 Core Plus 65C02, 65C102, and 65C112 Instructions
.if r65c02
	.r65c02
.endif


adc #12		;69 12
		;---
adc 1234	;6D 34 12
adc ext		;6D 22 11
adc extext	;6Du00v00
		;---
adc 12		;65 12
adc *dir	;65 33
adc *extext	;65*00
		;---
adc 12,x	;75 12
adc *dir,x	;75 33
adc offset,x	;75 44
adc *extdir,x	;75*00
		;---
adc 1234,x	;7D 34 12
adc ext,x	;7D 22 11
adc extext,x	;7Du00v00
		;---
adc 1234,y	;79 34 12
adc *dir,y	;79 33 00
adc *extdir,y	;79u00v00
adc ext,y	;79 22 11
adc extext,y	;79u00v00
		;---
adc [12,x]	;61 12
adc [*dir,x]	;61 33
adc [offset,x]	;61 44
adc [*extdir,x]	;61*00
adc [extext,x]	;61*00
		;---
adc [12],y	;71 12
adc [*dir],y	;71 33
adc [offset],y	;71 44
adc [*extdir],y	;71*00
adc [extext],y	;71*00
		;---
.if r65c02
adc [12]	;72 12
adc [*dir]	;72 33
adc [offset]	;72 44
adc [*extdir]	;72*00
adc [extext]	;72*00
.endif

and #12		;29 12
		;---
and 1234	;2D 34 12
and ext		;2D 22 11
and extext	;2Du00v00
		;---
and 12		;25 12
and *dir	;25 33
and *extext	;25*00
		;---
and 12,x	;35 12
and *dir,x	;35 33
and offset,x	;35 44
and *extdir,x	;35*00
		;---
and 1234,x	;3D 34 12
and ext,x	;3D 22 11
and extext,x	;3Du00v00
		;---
and 1234,y	;39 34 12
and *dir,y	;39 33 00
and *extdir,y	;39u00v00
and ext,y	;39 22 11
and extext,y	;39u00v00
		;---
and [12,x]	;21 12
and [*dir,x]	;21 33
and [offset,x]	;21 44
and [*extdir,x]	;21*00
and [extext,x]	;21*00
		;---
and [12],y	;31 12
and [*dir],y	;31 33
and [offset],y	;31 44
and [*extdir],y	;31*00
and [extext],y	;31*00
		;---
.if	r65c02
and [12]	;32 12
and [*dir]	;32 33
and [offset]	;32 44
and [*extdir]	;32*00
and [extext]	;32*00
.endif

asl a		;0A
asl		;0A
		;---
asl 1234	;0E 34 12
asl ext		;0E 22 11
asl extext	;0Eu00v00
		;---
asl 12		;06 12
asl *dir	;06 33
asl *extext	;06*00
		;---
asl 12,x	;16 12
asl *dir,x	;16 33
asl offset,x	;16 44
asl *extdir,x	;16*00
		;---
asl 1234,x	;1E 34 12
asl ext,x	;1E 22 11
asl extext,x	;1Eu00v00

.if	r65f11+r65c00+r65c02
bbr0 12,.	;0F 12 FD
bbr1 12,.	;1F 12 FD
bbr2 12,.	;2F 12 FD
bbr3 12,.	;3F 12 FD
bbr4 12,.	;4F 12 FD
bbr5 12,.	;5F 12 FD
bbr6 12,.	;6F 12 FD
bbr7 12,.	;7F 12 FD

bbs0 12,.	;8F 12 FD
bbs1 12,.	;9F 12 FD
bbs2 12,.	;AF 12 FD
bbs3 12,.	;BF 12 FD
bbs4 12,.	;CF 12 FD
bbs5 12,.	;DF 12 FD
bbs6 12,.	;EF 12 FD
bbs7 12,.	;FF 12 FD

1$:	bbr0 12,3$	;0F 12 03
2$:	bbr0 *dir,2$	;0F 33 FD
3$:	bbr0 *extext,1$	;0F*00 F7

4$:	bbs0 12,6$	;8F 12 03
5$:	bbs0 *dir,5$	;8F 33 FD
6$:	bbs0 *extext,4$	;8F*00 F7
.endif

bcc .		;90 FE

bcs .		;B0 FE

beq .		;F0 FE

.if	r65c02
bit #12		;89 12
.endif
		;---
bit 1234	;2C 34 12
bit ext		;2C 22 11
bit extext	;2Cu00v00
		;---
bit 12		;24 12
bit *dir	;24 33
bit *extext	;24*00
		;---
.if	r65c02
bit 12,x	;34 12
bit *dir,x	;34 33
bit offset,x	;34 44
bit *extdir,x	;34*00
		;---
bit 1234,x	;3C 34 12
bit ext,x	;3C 22 11
bit extext,x	;3Cu00v00
.endif

bmi .		;30 FE

bne .		;D0 FE

bpl .		;10 FE

.if	r65c00+r65c02
bra .		;80 FE
.endif

brk		;00

bvc .		;50 FE

bvs .		;70 FE

clc		;18

cld		;D8

cli		;58

clv		;B8

cmp #12		;C9 12
		;---
cmp 1234	;CD 34 12
cmp ext		;CD 22 11
cmp extext	;CDu00v00
		;---
cmp 12		;C5 12
cmp *dir	;C5 33
cmp *extext	;C5*00
		;---
cmp 12,x	;D5 12
cmp *dir,x	;D5 33
cmp offset,x	;D5 44
cmp *extdir,x	;D5*00
		;---
cmp 1234,x	;DD 34 12
cmp ext,x	;DD 22 11
cmp extext,x	;DDu00v00
		;---
cmp 1234,y	;D9 34 12
cmp *dir,y	;D9 33 00
cmp *extdir,y	;D9u00v00
cmp ext,y	;D9 22 11
cmp extext,y	;D9u00v00
		;---
cmp [12,x]	;C1 12
cmp [*dir,x]	;C1 33
cmp [offset,x]	;C1 44
cmp [*extdir,x]	;C1*00
cmp [extext,x]	;C1*00
		;---
cmp [12],y	;D1 12
cmp [*dir],y	;D1 33
cmp [offset],y	;D1 44
cmp [*extdir],y	;D1*00
cmp [extext],y	;D1*00
		;---
.if	r65c02
cmp [12]	;D2 12
cmp [*dir]	;D2 33
cmp [offset]	;D2 44
cmp [*extdir]	;D2*00
cmp [extext]	;D2*00
.endif

.if	r65c02
cpx #12		;E0 12
.endif
		;---
cpx 12		;E4 12
cpx *dir	;E4 33
cpx *extdir	;E4*00
		;---
cpx 1234	;EC 34 12
cpx ext		;EC 22 11
cpx extext	;ECu00v00

.if	r65c02
cpy #12		;C0 12
.endif
		;---
cpy 12		;C4 12
cpy *dir	;C4 33
cpy *extdir	;C4*00
		;---
cpy 1234	;CC 34 12
cpy ext		;CC 22 11
cpy extext	;CCu00v00

.if	r65c02
dec a		;3A
dec		;3A
.endif
		;---
dec 1234	;CE 34 12
dec ext		;CE 22 11
dec extext	;CEu00v00
		;---
dec 12		;C6 12
dec *dir	;C6 33
dec *extext	;C6*00
		;---
dec 12,x	;D6 12
dec *dir,x	;D6 33
dec offset,x	;D6 44
dec *extdir,x	;D6*00
		;---
dec 1234,x	;DE 34 12
dec ext,x	;DE 22 11
dec extext,x	;DEu00v00

dex		;CA

dey		;88

eor #12		;49 12
		;---
eor 1234	;4D 34 12
eor ext		;4D 22 11
eor extext	;4Du00v00
		;---
eor 12		;45 12
eor *dir	;45 33
eor *extext	;45*00
		;---
eor 12,x	;55 12
eor *dir,x	;55 33
eor offset,x	;55 44
eor *extdir,x	;55*00
		;---
eor 1234,x	;5D 34 12
eor ext,x	;5D 22 11
eor extext,x	;5Du00v00
		;---
eor 1234,y	;59 34 12
eor *dir,y	;59 33 00
eor *extdir,y	;59u00v00
eor ext,y	;59 22 11
eor extext,y	;59u00v00
		;---
eor [12,x]	;41 12
eor [*dir,x]	;41 33
eor [offset,x]	;41 44
eor [*extdir,x]	;41*00
eor [extext,x]	;41*00
		;---
eor [12],y	;51 12
eor [*dir],y	;51 33
eor [offset],y	;51 44
eor [*extdir],y	;51*00
eor [extext],y	;51*00
		;---
.if	r65c02
eor [12]	;52 12
eor [*dir]	;52 33
eor [offset]	;52 44
eor [*extdir]	;52*00
eor [extext]	;52*00
.endif

.if	r65c02
inc a		;1A
inc		;1A
.endif
		;---
inc 1234	;EE 34 12
inc ext		;EE 22 11
inc extext	;EEu00v00
		;---
inc 12		;E6 12
inc *dir	;E6 33
inc *extext	;E6*00
		;---
inc 12,x	;F6 12
inc *dir,x	;F6 33
inc offset,x	;F6 44
inc *extdir,x	;F6*00
		;---
inc 1234,x	;FE 34 12
inc ext,x	;FE 22 11
inc extext,x	;FEu00v00

inx		;E8

iny		;C8

jmp 12		;4C 12 00
jmp *dir	;4C 33 00
jmp *extext	;4Cu00v00
		;---
jmp 1234	;4C 34 12
jmp ext		;4C 22 11
jmp extext	;4Cu00v00
		;---
jmp [1234]	;6C 34 12
jmp [ext]	;6C 22 11
jmp [extext]	;6Cu00v00
		;---
.if	r65c02
jmp [1234,x]	;7C 34 12
jmp [ext,x]	;7C 22 11
jmp [extext,x]	;7Cu00v00
.endif

jsr *dir	;20 33 00
jsr *extdir	;20u00v00
		;---
jsr 1234	;20 34 12
jsr ext		;20 22 11
jsr extext	;20u00v00

lda #12		;A9 12
		;---
lda 1234	;AD 34 12
lda ext		;AD 22 11
lda extext	;ADu00v00
		;---
lda 12		;A5 12
lda *dir	;A5 33
lda *extext	;A5*00
		;---
lda 12,x	;B5 12
lda *dir,x	;B5 33
lda offset,x	;B5 44
lda *extdir,x	;B5*00
		;---
lda 1234,x	;BD 34 12
lda ext,x	;BD 22 11
lda extext,x	;BDu00v00
		;---
lda 1234,y	;B9 34 12
lda *dir,y	;B9 33 00
lda *extdir,y	;B9u00v00
lda ext,y	;B9 22 11
lda extext,y	;B9u00v00
		;---
lda [12,x]	;A1 12
lda [*dir,x]	;A1 33
lda [offset,x]	;A1 44
lda [*extdir,x]	;A1*00
lda [extext,x]	;A1*00
		;---
lda [12],y	;B1 12
lda [*dir],y	;B1 33
lda [offset],y	;B1 44
lda [*extdir],y	;B1*00
lda [extext],y	;B1*00
		;---
.if	r65c02
lda [12]	;B2 12
lda [*dir]	;B2 33
lda [offset]	;B2 44
lda [*extdir]	;B2*00
lda [extext]	;B2*00
.endif

ldx #12		;A2 12
		;---
ldx 12		;A6 12
ldx *dir	;A6 33
ldx *extdir	;A6*00
		;---
ldx 12,y	;B6 12
ldx *dir,y	;B6 33
ldx *extdir,y	;B6*00
		;---
ldx 1234	;AE 34 12
ldx ext		;AE 22 11
ldx extext	;AEu00v00
		;---
ldx 1234,y	;BE 34 12
ldx ext,y	;BE 22 11
ldx extext,y	;BEu00v00

ldy #12		;A0 12
		;---
ldy 12		;A4 12
ldy *dir	;A4 33
ldy *extdir	;A4*00
		;---
ldy 12,x	;B4 12
ldy *dir,x	;B4 33
ldy *extdir,x	;B4*00
		;---
ldy 1234	;AC 34 12
ldy ext		;AC 22 11
ldy extext	;ACu00v00
		;---
ldy 1234,x	;BC 34 12
ldy ext,x	;BC 22 11
ldy extext,x	;BCu00v00

lsr a		;4A
lsr		;4A
		;---
lsr 1234	;4E 34 12
lsr ext		;4E 22 11
lsr extext	;4Eu00v00
		;---
lsr 12		;46 12
lsr *dir	;46 33
lsr *extext	;46*00
		;---
lsr 12,x	;56 12
lsr *dir,x	;56 33
lsr offset,x	;56 44
lsr *extdir,x	;56*00
		;---
lsr 1234,x	;5E 34 12
lsr ext,x	;5E 22 11
lsr extext,x	;5Eu00v00

.if	r65c00
mul		;02
.endif

nop		;EA

ora #12		;09 12
		;---
ora 1234	;0D 34 12
ora ext		;0D 22 11
ora extext	;0Du00v00
		;---
ora 12		;05 12
ora *dir	;05 33
ora *extext	;05*00
		;---
ora 12,x	;15 12
ora *dir,x	;15 33
ora offset,x	;15 44
ora *extdir,x	;15*00
		;---
ora 1234,x	;1D 34 12
ora ext,x	;1D 22 11
ora extext,x	;1Du00v00
		;---
ora 1234,y	;19 34 12
ora *dir,y	;19 33 00
ora *extdir,y	;19u00v00
ora ext,y	;19 22 11
ora extext,y	;19u00v00
		;---
ora [12,x]	;01 12
ora [*dir,x]	;01 33
ora [offset,x]	;01 44
ora [*extdir,x]	;01*00
ora [extext,x]	;01*00
		;---
ora [12],y	;11 12
ora [*dir],y	;11 33
ora [offset],y	;11 44
ora [*extdir],y	;11*00
ora [extext],y	;11*00
		;---
.if	r65c02
ora [12]	;12 12
ora [*dir]	;12 33
ora [offset]	;12 44
ora [*extdir]	;12*00
ora [extext]	;12*00
.endif

pha		;48

php		;08

.if	r65c00+r65c02
phx		;DA

phy		;5A
.endif

pla		;68

plp		;28

.if	r65c00+r65c02
plx		;FA

ply		;7A
.endif

.if	r65f11+r65c00+r65c02
rmb0 12		;07 12
rmb1 12		;17 12
rmb2 12		;27 12
rmb3 12		;37 12
rmb4 12		;47 12
rmb5 12		;57 12
rmb6 12		;67 12
rmb7 12		;77 12

rmb0 *dir	;07 33
rmb0 *extdir	;07*00
.endif

.if	r65c02
rol a		;2A
rol		;2A
.endif
		;---
rol 1234	;2E 34 12
rol ext		;2E 22 11
rol extext	;2Eu00v00
		;---
rol 12		;26 12
rol *dir	;26 33
rol *extext	;26*00
		;---
rol 12,x	;36 12
rol *dir,x	;36 33
rol offset,x	;36 44
rol *extdir,x	;36*00
		;---
rol 1234,x	;3E 34 12
rol ext,x	;3E 22 11
rol extext,x	;3Eu00v00

ror a		;6A
ror		;6A
		;---
ror 1234	;6E 34 12
ror ext		;6E 22 11
ror extext	;6Eu00v00
		;---
ror 12		;66 12
ror *dir	;66 33
ror *extext	;66*00
		;---
ror 12,x	;76 12
ror *dir,x	;76 33
ror offset,x	;76 44
ror *extdir,x	;76*00
		;---
ror 1234,x	;7E 34 12
ror ext,x	;7E 22 11
ror extext,x	;7Eu00v00

rti		;40

rts		;60

sbc #12		;E9 12
		;---
sbc 1234	;ED 34 12
sbc ext		;ED 22 11
sbc extext	;EDu00v00
		;---
sbc 12		;E5 12
sbc *dir	;E5 33
sbc *extext	;E5*00
		;---
sbc 12,x	;F5 12
sbc *dir,x	;F5 33
sbc offset,x	;F5 44
sbc *extdir,x	;F5*00
		;---
sbc 1234,x	;FD 34 12
sbc ext,x	;FD 22 11
sbc extext,x	;FDu00v00
		;---
sbc 1234,y	;F9 34 12
sbc *dir,y	;F9 33 00
sbc *extdir,y	;F9u00v00
sbc ext,y	;F9 22 11
sbc extext,y	;F9u00v00
		;---
sbc [12,x]	;E1 12
sbc [*dir,x]	;E1 33
sbc [offset,x]	;E1 44
sbc [*extdir,x]	;E1*00
sbc [extext,x]	;E1*00
		;---
sbc [12],y	;F1 12
sbc [*dir],y	;F1 33
sbc [offset],y	;F1 44
sbc [*extdir],y	;F1*00
sbc [extext],y	;F1*00
		;---
.if	r65c02
sbc [12]	;F2 12
sbc [*dir]	;F2 33
sbc [offset]	;F2 44
sbc [*extdir]	;F2*00
sbc [extext]	;F2*00
.endif

sec		;38

sed		;F8

sei		;78

.if	r65f11+r65c00+r65c02
smb0 12		;87 12
smb1 12		;97 12
smb2 12		;A7 12
smb3 12		;B7 12
smb4 12		;C7 12
smb5 12		;D7 12
smb6 12		;E7 12
smb7 12		;F7 12

smb0 *dir	;87 33
smb0 *extdir	;87*00
.endif

;sta #12	;89 12
		;---
sta 1234	;8D 34 12
sta ext		;8D 22 11
sta extext	;8Du00v00
		;---
sta 12		;85 12
sta *dir	;85 33
sta *extext	;85*00
		;---
sta 12,x	;95 12
sta *dir,x	;95 33
sta offset,x	;95 44
sta *extdir,x	;95*00
		;---
sta 1234,x	;9D 34 12
sta ext,x	;9D 22 11
sta extext,x	;9Du00v00
		;---
sta 1234,y	;99 34 12
sta *dir,y	;99 33 00
sta *extdir,y	;99u00v00
sta ext,y	;99 22 11
sta extext,y	;99u00v00
		;---
sta [12,x]	;81 12
sta [*dir,x]	;81 33
sta [offset,x]	;81 44
sta [*extdir,x]	;81*00
sta [extext,x]	;81*00
		;---
sta [12],y	;91 12
sta [*dir],y	;91 33
sta [offset],y	;91 44
sta [*extdir],y	;91*00
sta [extext],y	;91*00
		;---
.if	r65c02
sta [12]	;92 12
sta [*dir]	;92 33
sta [offset]	;92 44
sta [*extdir]	;92*00
sta [extext]	;92*00
.endif

stx 12		;86 12
stx *dir	;86 33
stx *extdir	;86*0
		;---
stx 1234	;8E 34 12
stx ext		;8E 22 11
stx extext	;8Eu00v00
		;---
stx 12,y	;96 12
stx *dir,y	;96 33
stx *extdir,y	;96*00

sty 12		;84 12
sty *dir	;84 33
sty *extdir	;84*0
		;---
sty 1234	;8C 34 12
sty ext		;8C 22 11
sty extext	;8Cu00v00
		;---
sty 12,x	;94 12
sty *dir,x	;94 33
sty *extdir,x	;94*00

.if	r65c02
stz 12		;64 12
stz *dir	;64 33
stz *extdir	;64*00
		;---
stz 1234	;9C 34 12
stz ext		;9C 22 11
stz extext	;9Cu00v00
		;---
stz 12,x	;74 12
stz *dir,x	;74 33
stz *extdir,x	;74*00
		;---
stz 1234,x	;9E 34 12
stz ext,x	;9E 22 11
stz extext,x	;9Eu00v00
.endif

tax		;AA

tay		;A8

.if	r65c02
trb 1234	;1C 34 12
trb ext		;1C 22 11
trb extext	;1Cu00v00
		;---
trb 12		;14 12
trb *dir	;14 33
trb *extdir	;14*00

tsb 1234	;0C 34 12
tsb ext		;0C 22 11
tsb extext	;0Cu00v00
		;---
tsb 12		;04 12
tsb *dir	;04 33
tsb *extdir	;04*00
.endif

tsx		;BA

txa		;8A

txs		;9A

tya		;98
