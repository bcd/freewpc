dnl	m6809.m4 - 6809 macroassembler instructions

dnl	The very first statement of this file should be a change
dnl	comment command to whatever comment character is used
dnl	by your assembler of choice.  For as6809, this is the
dnl	semicolon.
dnl	Hereafter, we can use similar comments through this file.
changecom(;)
divert(-1)

#include "wpc.h"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Set the quote characters to something a little easier to see.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
changequote([,])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Basic infrastructure defines
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(__cat,$1$2$3$4$5$6$7$8$9)dnl

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Local labels
;
; These macros are used internally to generate local labels.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(lblreset, [define([__lbl], 0)])dnl

define(lblalloc, [define([__lbl], incr(__lbl)) pushdef([__lblstack], __lbl)])dnl

define(lblcur, [__cat(__lblstack,[$])])

define(lblfree, [popdef([__lblstack])])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; TRUE and FALSE values are represented by a bit in the
; condition code (CC) register.  btrue and bfalse are pseudoinstructions
; that branch based on this bit.  The true and false macros
; are used to change the value of the bit.
; The parity is arbitrary and can be changed if desired.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(btrue_cond,[bcc])
define(bfalse_cond,[bcs])

define(true, andcc #~CC_CARRY)
define(false,orcc #CC_CARRY)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; bnz/bz sometimes convey a more suitable meaning.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(bnz,bne)
define(bz,beq)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Conditionals
;
; The ifxx/endif macros are used to mark a block of code to
; be conditionally executed.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(__condinv, [define([__expr], $1)ifelse(
   __expr, [ne], [eq],
   __expr, [eq], [ne],
   __expr, [nz], [z],
   __expr, [z], [nz],
   __expr, [lt], [ge],
   __expr, [ge], [lt],
   __expr, [le], [gt],
   __expr, [gt], [le],
   __expr, [hs], [lo],
   __expr, [lo], [hs],
   __expr, [hi], [ls],
   __expr, [ls], [hi],
	__expr, [true_cond], [false_cond],
	__expr, [false_cond], [true_cond])])dnl

define(if, [__cat(b, __condinv($1), lblalloc lblcur)])

define(ifne, [if(ne)])
define(ifeq, [if(eq)])
define(ifnz, [if(nz)])
define(ifz, [if(z)])
define(iflt, [if(lt)])
define(ifge, [if(ge)])
define(ifle, [if(le)])
define(ifgt, [if(gt)])
define(ifhs, [if(hs)])
define(iflo, [if(lo)])
define(ifhi, [if(hi)])
define(ifls, [if(ls)])
define(iftrue, [if(true_cond)])
define(iffalse, [if(false_cond)])

;define(ifne, [beq lblalloc lblcur])
;define(ifeq, [bne lblalloc lblcur])
;define(ifnz, [ifne])
;define(ifz, [ifeq])
;define(iflt, [bge lblalloc lblcur])
;define(ifge, [blt lblalloc lblcur])
;define(ifle, [bgt lblalloc lblcur])
;define(ifgt, [ble lblalloc lblcur])
;define(ifhs, [blo lblalloc lblcur])
;define(iflo, [bhs lblalloc lblcur])
;define(ifhi, [bls lblalloc lblcur])
;define(ifls, [bhi lblalloc lblcur])
;define(iftrue, [bcs lblalloc lblcur])
;define(iffalse, [bcc lblalloc lblcur])

define(else, [
	define(__endlabel, lblcur)
	lblfree
	bra lblalloc lblcur
	__endlabel:
])


define(endif, [ifelse(
	lblcur, [0$], [[endif]], 
	lblcur, [__lblstack$], [[endif]], 
	lblcur: lblfree )
])


define(loop, [lblalloc lblcur:])
define(endloop, [bra lblcur lblfree])

define(while, [b$1 lblcur lblfree])
define(lwhile, [lb$1 lblcur lblfree])
define(until, __cat(b, __condinv($1), lblcur lblfree))
define(luntil, __cat(lb, __condinv($1), lblcur lblfree))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Register primitives
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(setregconst, [__cat(ld, $1) #$2])dnl

define(increg, [define([reg], $1)ifelse(
   reg, [x], [leax 1,x],
   reg, [y], [leay 1,y],
   reg, [s], [leas 1,s],
   reg, [u], [leau 1,u],
   reg, [d], [addd #0x0001],
   reg, [a], inca,
   reg, [b], incb)])dnl

define(decreg, [define([reg], $1)ifelse(
   reg, [x], [leax -1,x],
   reg, [y], [leay -1,y],
   reg, [s], [leas -1,s],
   reg, [u], [leau -1,u],
   reg, [d], [addd #0xffff],
   reg, [a], deca,
   reg, [b], decb)])dnl

define(dectstreg, [define([reg], $1)ifelse(
   reg, [x], [leax -1,x
		cmpx #0],
   reg, [y], [leay -1,y
		cmpy #0],
   reg, [s], [leas -1,s
		cmps #0],
   reg, [u], [leau -1,u
		cmpu #0],
   reg, [d], [addd #0xffff
		cmpd #0],
   reg, [a], deca,
   reg, [b], decb)])dnl

define(zeroreg, [define([reg], $1)ifelse(
   reg, [x], [ldx #0],
   reg, [y], [ldy #0],
   reg, [s], [lds #0],
   reg, [u], [ldu #0],
   reg, [d], [ldd #0],
   reg, [a], [clra],
   reg, [b], [clrb]
)])dnl


define(autoincrement, [define([val], $1)ifelse(
   val, 1, [+],
   val, 2, [++]
)])dnl


define(autodecrement, [define([val], $1)ifelse(
   val, 1, [-],
   val, 2, [--]
)])dnl


define(cmpregconst, [__cat(cmp, $1) #$2])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Loops
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(repeat, [
   pushdef([repvar], $1)
   setregconst($1, $2)
   lblalloc lblcur:
])dnl


define(endrep, [
   dectstreg(repvar)
   bne lblcur lblfree
   popdef([repvar])
])dnl



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Procedures
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(interrupt, [
	pushdef(__irqproc, 1)
])

define(noreturn, [
	pushdef(__noreturn, 1)
])

define(proc, [
	define([__procname], [$1])
	define([__localoff], 0)
	define([__saveoff], 0)
	lblreset
	$1::
])

define(return, [bra __cat(__exitlabel_, __procname)])

define(uses, [
	pushdef(__useregs, [$*])pshs __useregs
	define([__saveoff], 0)
	ifelse(regexp([$*], a), -1,, [define([__saved_a], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(a)))])
	ifelse(regexp([$*], b), -1,, [define([__saved_b], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(b)))])
	ifelse(regexp([$*], d), -1,, [define([__saved_d], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(d)))])
	ifelse(regexp([$*], x), -1,, [define([__saved_x], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(x)))])
	ifelse(regexp([$*], y), -1,, [define([__saved_y], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(y)))])
	ifelse(regexp([$*], s), -1,, [define([__saved_s], __saveoff)
		define([__saveoff], eval(__saveoff + sizeof(s)))])
	ifelse(regexp([$*], u), -1,, [define([__saved_u], __saveoff) 
		define([__saveoff], eval(__saveoff + sizeof(u)))])])


define(saved, [eval(__cat(__saved_, $1) + __localoff)[,s]])


define(returns, [pushdef([__returnregs], [$*])])

define(requires, [pushdef([__requireregs], [$*])])


; Note: A 'local' must come AFTER the 'uses' in order to work OK.

define(local, [
	pushdef([$2], __localoff[,s])
	define([__localoff], eval(__localoff + sizeof([$1])))
])


define(endlocal, [
	leas -__localoff[,s]
])


; TODO - __noreturn should be checked to avoid generating rts or pc in
; the puls list.
define(endp, [
	__cat(__exitlabel_, __procname):
   ifelse(__localoff, 0,, leas __localoff[,s])
   ifdef([__useregs], [puls __useregs, pc], ifdef([__irqproc], rti, rts))
	ifdef([__irqproc],popdef([__irqproc]))
	ifdef([__noreturn],popdef([__noreturn]))
   popdef([__useregs])
	popdef([__returnregs])
	popdef([__requireregs])
   undefine([__procname])
   undefine([__localoff])
   undefine([__saveoff])
   undefine([__saved_a]) undefine([__saved_b])
   undefine([__saved_x]) undefine([__saved_y])
   undefine([__saved_s]) undefine([__saved_u])
])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Types
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(sizeof, 
   [define([__type], $1)ifelse(
      __type, [char], 1, 
      __type, [byte], 1, 
      __type, [word], 2, 
      __type, [a], 1,
      __type, [b], 1,
      __type, [d], 2,
      __type, [x], 2,
      __type, [y], 2,
      __type, [s], 2,
      __type, [u], 2,
      __type, [pc], 2,
      ifdef(__cat(__sizeof_, __type), __cat(__sizeof_, __type), 0))])dnl


define(defstruct, [
   define([__structoff], 0)dnl
   define(__cat(__offsetof_, $1, _, $3), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($2)))dnl
   define(__cat(__offsetof_, $1, _, $5), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($4)))dnl
   define(__cat(__offsetof_, $1, _, $7), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($6)))dnl
   define(__cat(__offsetof_, $1, _, $9), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($8)))dnl
   define(__cat(__offsetof_, $1, _, $11), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($10)))dnl
   define(__cat(__offsetof_, $1, _, $13), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($12)))dnl
   define(__cat(__offsetof_, $1, _, $15), __structoff)dnl
   define([__structoff], eval(__structoff + sizeof($14)))dnl
   define(__cat(__sizeof_, $1), __structoff)dnl
   undefine([__structoff])dnl
])dnl


define(offsetof, [__cat(__offsetof_, $1, _, $2)])dnl


define(typedecl, [define(__cat(__typeof_, $1), $2)])dnl


define(deref, [offsetof(__cat(__typeof_, $1), $2),$1])dnl


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Switch Statements
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(switch, [
   pushdef([__switchreg], $1)
	lblalloc
	pushdef([__switchexit], lblcur)
])


define(case, [
	cmpregconst(__switchreg, $1)
	bne lblalloc lblcur
])


define(endcase, [
	bra __switchexit
	lblcur: lblfree
])


define(default, [
	lblalloc
])


define(endswitch, [
	__switchexit:
	lblfree
	popdef([__switchexit])
	popdef([__switchreg])
])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Inline Procedure Arguments
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(definline, [
	uses([$*])
   define([__inlsize], eval(sizeof($2)+sizeof($3)+sizeof($4)+sizeof($5)))
   ld$1  eval(__saveoff),s
   ifelse(
      $#, 2, [ld$2	,__cat($1, autoincrement(sizeof($2)))],
      $#, 3, [ld$2	,__cat($1, autoincrement(sizeof($2)))
         ld$3	,__cat($1, autoincrement(sizeof($3)))],
      $#, 4, [ld$2	,__cat($1, autoincrement(sizeof($2)))
         ld$3	,__cat($1, autoincrement(sizeof($3)))
         ld$4	,__cat($1, autoincrement(sizeof($4)))],
      $#, 5, [ld$2	,__cat($1, autoincrement(sizeof($2)))
         ld$3	,__cat($1, autoincrement(sizeof($3)))
         ld$4	,__cat($1, autoincrement(sizeof($4)))
         ld$5	,__cat($1, autoincrement(sizeof($5)))])
   st$1  eval(__saveoff),s
])



define(pow2, [ifelse(
	$1, 0, 1,
	$1, 1, 2,
	$1, 2, 4,
	$1, 3, 8,
	$1, 4, 16,
	$1, 5, 32,
	$1, 6, 64,
	$1, 7, 128,
	$1, 8, 256,
	__invalid_arg_to_pow2)
])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; User Macros
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

define(macro, [define($1,])

define(endmacro, [)])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Data Declarations
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
define(global, [
	$1::
])

divert
