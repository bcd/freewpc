/*
 * $Header: /home/eric/src/gcc-2.95.3/gcc/config/m6809/RCS/m6809.c,v 1.4 2001/12/18 05:17:33 eric Exp $
 */
/*-------------------------------------------------------------------
	FILE: out-m6809.c
	last change Nov,21 1996
-------------------------------------------------------------------*/
/* Subroutines for insn-output.c for MC6809.
   Copyright (C) 1989 Free Software Foundation, Inc.

 MC6809 Version by Tom Jones (jones@sal.wisc.edu)
 Space Astronomy Laboratory
 University of Wisconsin at Madison

 minor changes to adapt it to gcc-2.5.8 by Matthias Doerfel
 ( msdoerfe@informatik.uni-erlangen.de )
 also added #pragma interrupt (inspired by gcc-6811)

 minor changes to adapt it to gcc-2.8.0 by Eric Botcazou
 (ebotcazou@multimania.com)

 minor changes to adapt it to gcc-2.95.3 by Eric Botcazou
 (ebotcazou@multimania.com)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"
#include "system.h"
#include "rtl.h"
#include "regs.h"
#include "flags.h"
#include "hard-reg-set.h"
#include "real.h"
#include "tree.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "function.h"
#include "target.h"
#include "target-def.h"
#include "expr.h"
#include "cpplib.h"
#include "c-pragma.h"
#if (TARGET_GCC_VERSION < 3003)
#include "c-lex.h"
#endif

/* macro to return TRUE if length of operand mode is one byte */
#define BYTE_MODE(X) ((GET_MODE_SIZE (GET_MODE (X))) == 1)
#define DEBUG

/*-------------------------------------------------------------------
    Custom abort function
-------------------------------------------------------------------*/
void
abort_2 ( file, line)
	char *file;
	int line;
{
    fprintf (stderr, "ABORT in file %s, line %d \n", file, line );
    exit (1);
}


/*-------------------------------------------------------------------
    Target hooks, moved from target.h
-------------------------------------------------------------------*/
void output_function_prologue ( FILE *file, int size );
void output_function_epilogue ( FILE *file, int size );

#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE	output_function_prologue

#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE	output_function_epilogue

#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP "\t.word\t"

#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP "\t.word\t"

#undef TARGET_ASM_UNALIGNED_HI_OP
#define TARGET_ASM_UNALIGNED_HI_OP "\t.word\t"

#undef TARGET_ASM_UNALIGNED_SI_OP
#define TARGET_ASM_UNALIGNED_SI_OP "\t.word\t"

struct gcc_target targetm = TARGET_INITIALIZER;

override_options ()
{
}

/* External variables used */
extern int reload_completed;   /* set in toplev.c */
extern FILE *asm_out_file;

static int last_mem_size;   /* operand size (bytes) */

/* True if a #pragma interrupt has been seen for the current function.  */
static int in_interrupt = 0;

/* True if a #pragma naked preceded the current function */
static int in_naked_function = 0;

/* True if the section was recently changed and another .area
 * directive needs to be output before emitting the next label. */
int section_changed = 0;

/* Section names.  The defaults here are used until a #pragma is seen
 * that changes it. */
char code_section_op[128] = "\t.area sysrom"; /* was _CODE */
char data_section_op[128] = "\t.area sysrom"; /* was _DATA */
char bss_section_op[128] = "\t.area ram"; /* was _BSS */

/*-------------------------------------------------------------------
    Return 1 if OP is a data register (there is only one)
-------------------------------------------------------------------*/
int
data_reg_operand (op, mode)
	rtx op;
	enum machine_mode mode;
{
	int code = 0;

	if (reg_operand (op, mode)) {
		if (reload_completed) {
			if (REGNO (op) == HARD_D_REGNUM) {
				code = 1;
			}
		}
		else {
			code = 1;
		}
    }
    return (code);
}

/*-------------------------------------------------------------------
    Return 1 if OP can be used as a byte-length operand
-------------------------------------------------------------------*/
int
byte_operand (op, mode)
	rtx op;
	enum machine_mode mode;
{
	if ((GET_CODE (op) == REG) || (GET_CODE (op) == SUBREG))
		return (byte_reg_operand (op, mode));
	else
		return (general_operand (op, mode));
}

/*-------------------------------------------------------------------
    Return 1 if OP can be used as a byte-length register
-------------------------------------------------------------------*/
int
byte_reg_operand (op, mode)
	rtx op;
	enum machine_mode mode;
{
	int code = 0;

	if (reg_operand (op, mode)) {
		if (reload_completed) {
			if (
				(REGNO (op) == HARD_D_REGNUM) ||
				(REGNO (op) == HARD_A_REGNUM) ||
				(REGNO (op) == HARD_B_REGNUM)) {
				code = 1;
			}
		}
		else {
			code = 1;
		}
	}
	return (code);
}

/*-------------------------------------------------------------------
    Return 1 if OP can be used as a register
    Hacked from "register_operand" in recog.c, similar but
    with looser mode checking.
-------------------------------------------------------------------*/
int
reg_operand (op, mode)
	rtx op;
	enum machine_mode mode;
{
	if (mode == VOIDmode)
		return 1;

	if ((GET_CODE (op) == SUBREG) && (!reload_completed))
		return general_operand (op, mode);

	while (GET_CODE (op) == SUBREG)
		op = SUBREG_REG (op);

  return GET_CODE (op) == REG;
}

/*-------------------------------------------------------------------
    Print Operand for assembler
-------------------------------------------------------------------*/
void
print_operand (file, x, code)
	FILE *file;
	rtx x;
	int code;
{
	extern void print_operand_address ();

	if (GET_CODE (x) == REG) {
		/* gcc currently allocates the entire 16-bit 'd' register
		 * even when it only needs an 8-bit value.  So here it
		 * is tricked into printing only the lower 8-bit 'b'
		 * register into the assembly output.
		 *
		 * Eventually gcc should be modified to allocate a/b
		 * independently and this hack can be removed.
		 */
		if ((BYTE_MODE (x)) && (REGNO (x) == HARD_D_REGNUM))
			fputs ("b", file);
		else
			fputs (reg_names[REGNO (x)], file);
	}

	else if (GET_CODE (x) == MEM) {
		last_mem_size = GET_MODE_SIZE (GET_MODE (x));
		if (code == 'L') {	/* LSH of word address */
			x = adjust_address (x, SImode, 1);
		}
		output_address (XEXP (x, 0));
	}

	else if (GET_CODE (x) == CONST_DOUBLE && GET_MODE (x) != DImode) {
		union { double d; int i[2]; } u;
		u.i[0] = CONST_DOUBLE_LOW (x);
		u.i[1] = CONST_DOUBLE_HIGH (x);
		fprintf (file, "#%.9#g", u.d);
	}

	else {
		if (code == 'L') {	/* LSH of word address */
			x = gen_rtx (CONST_INT, VOIDmode, (INTVAL(x) & 0xff));
		}
		else if (code == 'M') {	/* MSH of word address */
			x = gen_rtx (CONST_INT, VOIDmode, ((INTVAL(x) >> 8) & 0xff));
		}
		putc ('#', file);
		output_addr_const (file, x);
	}
}

/*-------------------------------------------------------------------
    Print a memory operand whose address is X, on file FILE.
-------------------------------------------------------------------*/
void
print_operand_address (file, addr)
FILE *file;
register rtx addr;
{
	extern void debug_rtx ();   /* DEBUG */
	register rtx base = 0;
	register rtx offset = 0;
	int regno;
	int indirect_flag = 0;

	/*** check for indirect addressing ***/
	if (GET_CODE (addr) == MEM) {
		last_mem_size = GET_MODE_SIZE (GET_MODE (addr));
		indirect_flag = 1;
		fprintf (file, "[");
		addr = XEXP (addr, 0);
	}

	switch (GET_CODE (addr)) {
		case REG:
			regno = REGNO (addr);
			/* Not sure about the following: you can't index from b
			 * anyway, so I don't think this is valid. */
			if ((BYTE_MODE (addr)) && (REGNO (addr) == HARD_D_REGNUM))
				fprintf (file, ",b");
			else
				fprintf (file, ",%s", reg_names[regno]);
			break;

		case PRE_DEC:
			regno = REGNO (XEXP (addr, 0));
			fputs (((last_mem_size == 1) ? ",-" : ",--"), file);
			fprintf (file, "%s", reg_names[regno]);
			break;

		case POST_INC:
			regno = REGNO (XEXP (addr, 0));
			fprintf (file, ",%s", reg_names[regno]);
			fputs (((last_mem_size == 1) ? "+" : "++"), file);
			break;

		case PLUS:
			base = XEXP (addr, 0);
			if (GET_CODE (base) == MEM)
				base = XEXP (base, 0);

			offset = XEXP (addr, 1);
			if (GET_CODE (offset) == MEM)
				offset = XEXP (offset, 0);

			if ((CONSTANT_ADDRESS_P (base))
			&& (CONSTANT_ADDRESS_P (offset))) {
				output_addr_const (file, base);
				fputs ("+", file);
				output_addr_const (file, offset);
			}

			else if ((CONSTANT_ADDRESS_P (base)) && (A_REG_P (offset))) {
				output_addr_const (file, base);
				fprintf (file, ",%s", reg_names[REGNO (offset)]);
			}

			else if ((CONSTANT_ADDRESS_P (offset)) && (A_REG_P (base))) {
				output_addr_const (file, offset);
				fprintf (file, ",%s", reg_names[REGNO (base)]);
			}

			/*** accumulator offset ***/
			else if (((D_REG_P (offset)) || (Q_REG_P (offset)))
			&& (A_REG_P (base))) {
				fprintf (file, "%s,%s",
				reg_names[REGNO (offset)], reg_names[REGNO (base)]);
			}

			else if (((D_REG_P (base)) || (Q_REG_P (base)))
			&& (A_REG_P (offset))) {
				fprintf (file, "%s,%s",
				reg_names[REGNO (base)], reg_names[REGNO (offset)]);
			}

			else if (GET_CODE (base) == PRE_DEC) {
				regno = REGNO (XEXP (base, 0));
				fputs (((last_mem_size == 1) ? ",-" : ",--"), file);
				fprintf (file, "%s", reg_names[regno]);
			}

			else if ((REG_P (base)) && (REG_P (offset))) {
				fprintf (file, "%s,%s ;BOGUS",
				reg_names[REGNO (offset)], reg_names[REGNO (base)]);
				debug_rtx (addr);   /* DEBUG */
			}

			else {
				fprintf (file, "BOGUS+");
				debug_rtx (addr);   /* DEBUG */
			}
			break;

	default:
		output_addr_const (file, addr);
		break;
	}

	if (indirect_flag)
		fprintf (file, "]");
}

/*-------------------------------------------------------------------
    Update the CC Status
---------------------------------------------------------------------
   Set the cc_status for the results of an insn whose pattern is EXP.
   We assume that jumps don't affect the condition codes.
   All else, clobbers the condition codes, by assumption.

   We assume that ALL add, minus, etc. instructions effect the condition
   codes.
-------------------------------------------------------------------*/
void
notice_update_cc (exp, insn)
	rtx exp;
	rtx insn;
{
    /*** recognize SET insn's ***/
    if (GET_CODE (exp) == SET) {
	/* Jumps do not alter the cc's.  */
	if (SET_DEST (exp) == pc_rtx)
	    return;

	/* Moving one register into another register (tfr):
	Doesn't alter the cc's.  */
	if (REG_P (SET_DEST (exp)) && (REG_P (SET_SRC (exp)))) {
	    return;
	}

	/* Moving memory into a register (load): Sets cc's. */
	if (REG_P (SET_DEST (exp)) && GET_CODE (SET_SRC (exp)) == MEM) {
	    cc_status.value1 = SET_SRC (exp);
	    cc_status.value2 = SET_DEST (exp);
	    return;
	}

	/* Moving register into memory (store): Sets cc's. */
	if (GET_CODE (SET_DEST (exp)) == MEM && REG_P (SET_SRC (exp))) {
	    cc_status.value1 = SET_SRC (exp);
	    cc_status.value2 = SET_DEST (exp);
	    return;
	}

	/* Function calls clobber the cc's.  */
	else if (GET_CODE (SET_SRC (exp)) == CALL) {
	    CC_STATUS_INIT;
	    return;
	}

	/* Tests and compares set the cc's in predictable ways.  */
	else if (SET_DEST (exp) == cc0_rtx) {
	    cc_status.flags = 0;
	    cc_status.value1 = SET_SRC (exp);
	    cc_status.value2 = SET_DEST (exp);
	    return;
	}

	/* Certain instructions effect the condition codes. */
	else if (GET_MODE (SET_SRC (exp)) == SImode
	    || GET_MODE (SET_SRC (exp)) == HImode) {
		switch (GET_CODE (SET_SRC (exp))) {
		case PLUS: case MINUS: case NEG:
		case AND: case IOR: case XOR:
		if (A_REG_P (SET_DEST (exp))) {
		    CC_STATUS_INIT;
		}
		else {
		    cc_status.value1 = SET_SRC (exp);
		    cc_status.value2 = SET_DEST (exp);
		}
		    break;

		default:
		    CC_STATUS_INIT;
		}
	    return;
	}
    }
    else if (GET_CODE (exp) == PARALLEL
	&& GET_CODE (XVECEXP (exp, 0, 0)) == SET) {
	    if (SET_DEST (XVECEXP (exp, 0, 0)) == pc_rtx)
		return;
	if (SET_DEST (XVECEXP (exp, 0, 0)) == cc0_rtx) {
	    CC_STATUS_INIT;
	    cc_status.value1 = SET_SRC (XVECEXP (exp, 0, 0));
	    return;
	}
    }

    /*** default action if we haven't recognized something
    and returned earlier ***/
    CC_STATUS_INIT;
}

				/* See flags.h and toplev.c */
extern int optimize;
/* extern int flag_combine_regs; */
extern int flag_strength_reduce;
extern int flag_no_peephole;
extern int flag_inline_functions;
extern int flag_omit_frame_pointer;
/* extern char *main_input_filename;*/
extern char *asm_file_name;
				/* See c-tree.h  and c-decl.c */
extern int flag_signed_char;

#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>


/* strip path from the filename
 *  for AS6809 assembler.
 */
static char*
my_basename (filename)
	char *filename;
{
   char *p = filename + strlen(filename) - 1;

   while ((p >= filename) && (*p != '/'))
      p--;

   return p+1;
}


void
print_options (file)
	FILE *file;
{
	char *a_time;
	long c_time;

	fprintf (file, ";;; Source:\t%s\n", main_input_filename);
	fprintf (file, ";;; Destination:\t%s\n", asm_file_name);

	c_time = time (0);
	a_time = ctime (&c_time);
	fprintf (file, ";;; Compiled:\t%s", a_time);

#ifdef __GNUC__
#ifndef __VERSION__
#define __VERSION__ "[unknown]"
#endif
	fprintf (file, ";;; (META)compiled by GNU C version %s.\n", __VERSION__);
#else
	fprintf (file, ";;; (META)compiled by CC.\n");
#endif

	fprintf (file, ";;; OPTIONS:\t%s%s\n",
		(TARGET_SHORT_INT ? " -mshort_int":" -mlong_int"),
		(TARGET_SHORT_BRANCH ? " -mshort_branch":" -mlong_branch"));
	fprintf (file, ";;; OPTIONS:\t%s\n",
		(flag_signed_char ? " signed-char" : " !signed-char"));
	fprintf (file, "\t.module\t%s\n", my_basename(main_input_filename));
}


void
output_function_prologue ( file, size )
	FILE *file;
	int size;
{ 
	register int regno;
	int offset = 0; 
	char reglist[30]; 

	if (in_naked_function)
	{
		fprintf (file, ";;;;; prologue omitted for naked function\n");
		return;
	}

	/* TODO : if function does not return, this can be optimized not to
	* save away all the regs */
	fprintf (file, ";;;-----------------------------------------\n"); 
	fprintf (file, ";;;  PROLOGUE for %s\n", current_function_name); 
	fprintf (file, ";;;-----------------------------------------\n"); 

	reglist[0] = '\0'; 

	if ((frame_pointer_needed) && (!regs_ever_live[HARD_U_REGNUM])) { 
		strcat (reglist, reg_names[HARD_U_REGNUM]); 
		offset += 2; 
	} 

	for (regno = HARD_X_REGNUM; regno <= HARD_U_REGNUM; regno++) {
		if  (regs_ever_live[regno]) { 
			if (reglist[0] != '\0') 
				strcat (reglist, ","); 
			strcat (reglist, reg_names[regno]); 
			offset += 2; 
		} 
	} 

	if (size != 0) 
		fprintf (file, "\tleas\t-(%d),s\t; allocate auto variables\n", size);
	if (offset != 0) 
		fprintf (file, "\tpshs\t%s\t;save registers\n", reglist); 
	if (frame_pointer_needed) { 
		fprintf (file, 
			"\tleau\t%d,s\t;use U-reg as frame pointer\n", offset+size); 
	} 
	fprintf (file, ";;;END PROLOGUE\n"); 
}


void
output_function_epilogue ( file, size )
	FILE *file;
	int size;
{
	register int regno; 
	int offset = 0; 
	char reglist[30]; 

    fprintf (file, ";;;EPILOGUE\n"); 

	 if (in_naked_function)
	 {
		fprintf (file, ";;;;; epilogue omitted for naked function\n\n\n");
		in_naked_function = 0;
		return;
	 }

    reglist[0] = '\0'; 
    if ((frame_pointer_needed) && (!regs_ever_live[HARD_U_REGNUM])) { 
          strcat (reglist, reg_names[HARD_U_REGNUM]); 
          offset += 2; 
    } 
    for  (regno = HARD_X_REGNUM; regno <= HARD_U_REGNUM; regno++) 
        { if  (regs_ever_live[regno]) 
            { 
              if (reglist[0] != '\0') 
                  strcat (reglist, ","); 
              strcat (reglist, reg_names[regno]); 
              offset += 2; 
            } 
        } 

    if (offset != 0)
      if ((size == 0) && !in_interrupt)
        fprintf (file, "\tpuls\t%s,pc\t;restore registers\n", reglist);
      else
        fprintf (file, "\tpuls\t%s\t;restore registers\n", reglist);
    if (size != 0)
        fprintf (file, "\tleas\t%d,s\t; deallocate auto variables\n",
            size);
    if (in_interrupt)
      fprintf (file, "\trti\t\t; return from interrupt\n");
    else if (offset==0 || (size != 0))
      fprintf (file, "\trts\t\t; return from function\n");

	fprintf (file, ";;;-----------------------------------------\n"); 
	fprintf (file, ";;; END EPILOGUE for %s\n", current_function_name); 
	fprintf (file, ";;;-----------------------------------------\n\n\n"); 
    
	in_interrupt = 0;
}


/*
 * Handle pragmas.  Note that only the last branch pragma seen in the 
 * source has any affect on code generation.  
 */
#define BAD_PRAGMA(msgid, arg) \
	do { warning (msgid, arg); return -1; } while (0)

static int
pragma_parse (name, sect)
     const char *name;
     tree *sect;
{
  tree s, x;

  if (c_lex (&x) != CPP_OPEN_PAREN)
    BAD_PRAGMA ("missing '(' after '#pragma %s' - ignored", name);

  if (c_lex (&s) != CPP_STRING)
    BAD_PRAGMA ("missing section name in '#pragma %s' - ignored", name);

  if (c_lex (&x) != CPP_CLOSE_PAREN)
    BAD_PRAGMA ("missing ')' for '#pragma %s' - ignored", name);

  if (c_lex (&x) != CPP_EOF)
    warning ("junk at end of '#pragma %s'", name);

  *sect = s;
  return 0;
}


void
pragma_short_branch (pfile)
	cpp_reader *pfile ATTRIBUTE_UNUSED;
{
	target_flags |= 1;
}

void 
pragma_long_branch (pfile)
	cpp_reader *pfile ATTRIBUTE_UNUSED;
{
   target_flags &= ~1;
}

void 
pragma_interrupt (pfile)
	cpp_reader *pfile ATTRIBUTE_UNUSED;
{
   in_interrupt = 1;
}

void 
pragma_naked (pfile)
	cpp_reader *pfile ATTRIBUTE_UNUSED;
{
	in_naked_function = 1;
}

void pragma_section (pfile)
	cpp_reader *pfile ATTRIBUTE_UNUSED;
{
	tree sect;

	if (pragma_parse ("section", &sect))
		return;

	snprintf (code_section_op, 6+TREE_STRING_LENGTH (sect),
		".area\t%s", TREE_STRING_POINTER (sect));
	section_changed++;
}


/* Check a `double' value for validity for a particular machine mode.
 * Called by the CHECK_FLOAT_VALUE() machine-dependent macro.
 */
int
check_float_value (mode, d, overflow)
	enum machine_mode mode;
	double *d;
	int overflow;
{
	if (mode == SFmode) {
		if (*d > 1.7014117331926443e+38) {
			error("magnitude of constant too large for `float'");
			*d = 1.7014117331926443e+38;
		}
		else if (*d < -1.7014117331926443e+38) {
			error("magnitude of constant too large for `float'");
			*d = -1.7014117331926443e+38;
		}
		else if ((*d > 0) && (*d < 2.9387358770557188e-39)) {
			warning("`float' constant truncated to zero");
			*d = 0.0;
		}
		else if ((*d < 0) && (*d > -2.9387358770557188e-39)) {
			warning("`float' constant truncated to zero");
			*d = 0.0;
		}
	}
	return overflow;
}


/* TBD */
void
m6809_gen_register_shift (operands)
	rtx *operands;
{
    output_asm_insn ("lea%2 -1,%2		;decr shift count", operands);
    output_asm_insn ("cmp%2 -1", operands);
    output_asm_insn ("bmi .+9", operands);
    output_asm_insn ("aslb", operands);
    output_asm_insn ("rola", operands);
    output_asm_insn ("bra .-9			   ;loop", operands);
}


