/* Definitions of target machine for GNU compiler.  MC6809 version.

 MC6809 Version by Tom Jones (jones@sal.wisc.edu)
 Space Astronomy Laboratory
 University of Wisconsin at Madison

 minor changes to adapt it to gcc-2.5.8 by Matthias Doerfel
 ( msdoerfe@informatik.uni-erlangen.de )
 also added #pragma interrupt (inspired by gcc-6811)

 minor changes to adapt it to gcc-2.8.0 by Eric Botcazou
 (ebotcazou@multimania.com)

 minor changes to adapt it to egcs-1.1.2 by Eric Botcazou
 (ebotcazou@multimania.com)

 minor changes to adapt it to gcc-2.95.3 by Eric Botcazou
 (ebotcazou@multimania.com)

 changes for gcc-3.1.1 by ???

 further changes for gcc-3.1.1 and beyond by Brian Dominy
 (brian@oddchange.com)


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
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/* Define the version of gcc which this machine description will
 * be combined with.  We support multiple variants for compatibility.
 */
#ifndef TARGET_GCC_VERSION
#define TARGET_GCC_VERSION 3001
#endif


/* Enable separate usage of 'A' and 'B' registers.
 * To work properly, we need to redefine a lot of other things.
 *
 * This does not work yet.
 */
/*** #define CONFIG_AB ***/


/* Names to predefine in the preprocessor for this target machine.  */
#if (TARGET_GCC_VERSION < 3004)
#define CPP_PREDEFINES "-Dmc6809 -DMC6809"
#endif

/* Print subsidiary information on the compiler version in use.  */
#define TARGET_VERSION fprintf (stderr, " (MC6809)");

/* Run-time compilation parameters selecting different hardware subsets.  */
extern int target_flags;
extern short *reg_renumber;	/* def in local_alloc.c */

/* Runtime current values of section names */
extern int section_changed;
extern char code_section_op[], data_section_op[], bss_section_op[];

/* Function prototypes (defined in m6809.c) */

/* Macros used in the machine description to test the flags.  */
/* 
   -margcount		use standard calling sequence, with arg count word
   -mnoargcount 	don't push arg count (it's in the symbol table)
   -mint8		use bytes for type "int"
   -mint16		use short integers for type "int"
   -mint32		use long integers for type "int"
   -mshort-branch	use short branch instructions
   -mlong-branch	use long branch instructions
*/

#define TARGET_FLAG_SHORT_BRANCH      0x1
#define TARGET_FLAG_ARGCOUNT          0x4
#define TARGET_FLAG_SHORT_INT         0x8
#define TARGET_FLAG_REG_ARGS          0x10
#define TARGET_FLAG_BYTE_INT          0x20
#define TARGET_FLAG_SMALL_SIZE_T      0x40


#define TARGET_ARGCOUNT (target_flags & TARGET_FLAG_ARGCOUNT)

/* Compile with 16-bit `int'.  */
#define TARGET_SHORT_INT (target_flags & TARGET_FLAG_SHORT_INT)

/* Compile with short (+-255) branch instructions */
#define TARGET_SHORT_BRANCH (target_flags & TARGET_FLAG_SHORT_BRANCH)

/* Compile with short (+-255) size_t */
#define TARGET_SMALL_SIZE_T (target_flags & TARGET_FLAG_SMALL_SIZE_T)

/* Enable function arguments in registers */
#define TARGET_REG_ARGS (target_flags & TARGET_FLAG_REG_ARGS)

/* Compile with 8-bit 'int' */
#define TARGET_BYTE_INT (target_flags & TARGET_FLAG_BYTE_INT)

/* Default target_flags if no switches specified.  */
#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT \
	(TARGET_FLAG_SHORT_INT | TARGET_FLAG_REG_ARGS | TARGET_FLAG_BYTE_INT)
#endif

/* For compatibility with older versions... */
#define CONFIG_REG_ARGS		TARGET_REG_ARGS
#define CONFIG_BYTE_INT		TARGET_BYTE_INT


/* Macro to define tables used to set the flags.
   This is a list in braces of pairs in braces,
   each pair being { "NAME", VALUE }
   where VALUE is the bits to set or minus the bits to clear.
   An empty string NAME is used to identify the default VALUE.  */

#define TARGET_SWITCHES \
  { { "short-branch", TARGET_FLAG_SHORT_BRANCH }, \
    { "long-branch", -TARGET_FLAG_SHORT_BRANCH }, \
    { "argcount", TARGET_FLAG_ARGCOUNT }, \
    { "noargcount", -TARGET_FLAG_ARGCOUNT }, \
    { "int8", TARGET_FLAG_BYTE_INT }, \
    { "int16", TARGET_FLAG_SHORT_INT }, \
    { "int32", -TARGET_FLAG_SHORT_INT }, \
    { "reg-args", TARGET_FLAG_REG_ARGS }, \
    { "noreg-args", -TARGET_FLAG_REG_ARGS }, \
    { "short_size", TARGET_FLAG_SMALL_SIZE_T }, \
    { "long_size", -TARGET_FLAG_SMALL_SIZE_T }, \
    { "", TARGET_DEFAULT } }

/* Pick a target if none was specified */
#define OVERRIDE_OPTIONS  override_options ();

/* Allow $ in identifiers */
#define DOLLARS_IN_IDENTIFIERS 1

/* Don't unroll too much! (default in unroll.c was 100) */
#define MAX_UNROLLED_INSNS 32

/*--------------------------------------------------------------
	Target machine storage layout
--------------------------------------------------------------*/

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.  */
#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN 1

/* Define this if most significant word of a multiword number is numbered. 
   Thera are no multiword instructions, so it's all equal */
#define WORDS_BIG_ENDIAN 0

/* Number of bits in an addressible storage unit */
#define BITS_PER_UNIT 8

/* Width in bits of a "word", or the contents of a machine register. */
/* TODO : change this when doing byte register allocation */
#define BITS_PER_WORD 16

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD (BITS_PER_WORD/8)

/* Width in bits of a pointer.  See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 16

/* Allocation boundary (bits) for storing pointers in memory.  */
#define POINTER_BOUNDARY 8

/* Allocation boundary (bits) for storing arguments in argument list.  */
/* PARM_BOUNDARY is divided by BITS_PER_WORD in expr.c -- tej */
#define PARM_BOUNDARY 8

/* Boundary (bits) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 8

/* Allocation boundary (bits) for the code of a function.  */
#define FUNCTION_BOUNDARY 8

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 8

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* A bitfield declared as `int' forces `int' alignment for the struct.  */
/*** #define PCC_BITFIELD_TYPE_MATTERS ***/	/* */

/* largest mode to use -- try 16 -- tej */
#define MAX_FIXED_MODE_SIZE 64

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 8

/* Define this if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 0

/*--------------------------------------------------------------
	 Standard register usage.
--------------------------------------------------------------*/
/* assign names to real MC6809 registers -- tej */
#define HARD_D_REGNUM	0   /* bit value = 0x01 */
#define HARD_X_REGNUM	1   /* bit value = 0x02 */
#define HARD_Y_REGNUM	2   /* bit value = 0x04 */
#define HARD_U_REGNUM	3   /* bit value = 0x08 */
#define HARD_S_REGNUM	4   /* bit value = 0x10 */
#define HARD_PC_REGNUM	5   /* bit value = 0x20 */
#define HARD_A_REGNUM	8   /* bit value = 0x100 */
#define HARD_B_REGNUM	9   /* bit value = 0x200 */
#define HARD_CC_REGNUM 10   /* bit value = 0x400 */
#define HARD_DP_REGNUM 11   /* bit value = 0x800 */

/* Same values, but in bitset form */
#define D_REGBIT			(1 << HARD_D_REGNUM)
#define X_REGBIT			(1 << HARD_X_REGNUM)
#define Y_REGBIT			(1 << HARD_Y_REGNUM)
#define U_REGBIT			(1 << HARD_U_REGNUM)
#define S_REGBIT			(1 << HARD_S_REGNUM)
#define PC_REGBIT			(1 << HARD_PC_REGNUM)
#define A_REGBIT			(1 << HARD_A_REGNUM)
#define B_REGBIT			(1 << HARD_B_REGNUM)
#define CC_REGBIT			(1 << HARD_CC_REGNUM)
#define DP_REGBIT			(1 << HARD_DP_REGNUM)

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.  */
#define FIRST_PSEUDO_REGISTER 12

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.  */
#ifdef CONFIG_AB
#define FIXED_REGISTERS \
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, }
  /* D, X, Y, U, S, PC,-, -, A, B, C, DP */
#else
#define FIXED_REGISTERS \
    {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, }
  /* D, X, Y, U, S, PC,-, -, A, B, C, DP */
#endif

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */
#define CALL_USED_REGISTERS \
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, }
  /* D, X, Y, U, S, PC,-, -, A, B, C, DP */

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.  */
#define HARD_REGNO_NREGS(REGNO, MODE) \
    ((REGNO >= HARD_A_REGNUM) ? (GET_MODE_SIZE (MODE)) \
	: ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD))

/* Value is 1 if hard register REGNO can hold a value
of machine-mode MODE.  */
#define HARD_REGNO_MODE_OK(REGNO, MODE) \
    (((G_REGNO_P (REGNO)) && (GET_MODE_SIZE (MODE) == 2)) \
  || ((Q_REGNO_P (REGNO)) && (GET_MODE_SIZE (MODE) == 1)) \
  || (MODE == VOIDmode))

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2) 0

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* program counter if referenced as a register */
#define PC_REGNUM HARD_PC_REGNUM

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM HARD_S_REGNUM

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM HARD_U_REGNUM

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 0

/* Store in the variable DEPTH the initial difference between the 
   frame pointer reg contents and the stack pointer reg contents,
   as of the start of the function body.  This depends on the layout
   of the fixed parts of the stack frame and on how registers are saved.  */
#define INITIAL_FRAME_POINTER_OFFSET(DEPTH)                       \
{ int regno;                                                      \
  int offset = 0;                                                 \
                                                                  \
  if ((frame_pointer_needed) && (!regs_ever_live[HARD_U_REGNUM])) \
    offset += 2;                                                  \
  for (regno = HARD_X_REGNUM; regno <= HARD_U_REGNUM; regno++)    \
    if (regs_ever_live[regno] && ! call_used_regs[regno])         \
      offset += 2;                                                \
  (DEPTH) = (offset + (get_frame_size ()));                       \
}

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* Register in which static-chain is passed to a function.  */
#define STATIC_CHAIN_REGNUM HARD_D_REGNUM

/* Register in which address to store a structure value
   is passed to a function.  */
#define STRUCT_VALUE_REGNUM HARD_D_REGNUM

/* default (ascending) order is (almost) fine for MC6809 */
#define REG_ALLOC_ORDER \
    {1, 2, 3, 0, 4, 5, 6, 7, 9, 8, 10, 11, }
  /* X, Y, U, D, S, PC,-, -, B, A, CC, DP */

/*--------------------------------------------------------------
	classes of registers
--------------------------------------------------------------*/
/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */
   
/* Define 3 classes A (address) and D (data), and Q (byte). */
enum reg_class {
    NO_REGS,

	 /* 16-bit (word (HI)) data (D) */
    D_REGS,

	 /* 8-bit (byte (QI)) data (D) */
    Q_REGS,

	 /* Either a D_REG or Q_REG can be an index register (A,B,D) */
	 I_REGS,

	 /* 16-bit addresses (X,Y,U,S,PC) */
    A_REGS,

	 /* 16-bit data and address (D,X,Y,U,S,PC) */
    G_REGS,

    ALL_REGS,
    LIM_REG_CLASSES
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Since GENERAL_REGS is a smaller class than ALL_REGS,
   it is not an alias to ALL_REGS, but to G_REGS. */
#define GENERAL_REGS G_REGS

/* Give names of register classes as strings for dump file.   */
#define REG_CLASS_NAMES \
 {"NO_REGS", "D_REGS", "Q_REGS", "I_REGS", "A_REGS", "G_REGS", "ALL_REGS" }

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define D_REG_CLASS_CONTENTS	(D_REGBIT)

#ifdef CONFIG_AB
#define Q_REG_CLASS_CONTENTS	(A_REGBIT | B_REGBIT)
#else
#define Q_REG_CLASS_CONTENTS	(D_REGBIT)
#endif

#define I_REG_CLASS_CONTENTS	\
	(A_REGBIT | B_REGBIT | D_REGBIT)

#define A_REG_CLASS_CONTENTS	\
	(X_REGBIT | Y_REGBIT | U_REGBIT | S_REGBIT | PC_REGBIT)

#define G_REG_CLASS_CONTENTS	\
	(D_REG_CLASS_CONTENTS | Q_REG_CLASS_CONTENTS | \
	 I_REG_CLASS_CONTENTS | A_REG_CLASS_CONTENTS)

#define ALL_REG_CLASS_CONTENTS \
	(G_REG_CLASS_CONTENTS | A_REGBIT | B_REGBIT)

#define REG_CLASS_CONTENTS { \
	0, \
	D_REGBIT, \
	Q_REG_CLASS_CONTENTS, \
	D_REGBIT | A_REGBIT | B_REGBIT, \
	A_REG_CLASS_CONTENTS, \
	G_REG_CLASS_CONTENTS, \
	ALL_REG_CLASS_CONTENTS, \
}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO) \
  (D_REGNO_P (REGNO) ? D_REGS : \
  (Q_REGNO_P (REGNO) ? Q_REGS : \
  (I_REGNO_P (REGNO) ? I_REGS : \
  (A_REGNO_P (REGNO) ? A_REGS : \
  (G_REGNO_P (REGNO) ? G_REGS : ALL_REGS)))))

#define D_REGNO_P(REGNO) ((REGNO) == HARD_D_REGNUM)

#ifdef CONFIG_AB
#define Q_REGNO_P(REGNO) (((REGNO) == HARD_A_REGNUM) || \
	((REGNO) == HARD_B_REGNUM))
#else
#define Q_REGNO_P(REGNO) ((REGNO) == HARD_D_REGNUM)
#endif

#define I_REGNO_P(REGNO) (((REGNO) == HARD_D_REGNUM) || \
	((REGNO) == HARD_A_REGNUM) || ((REGNO) == HARD_B_REGNUM))

#define A_REGNO_P(REGNO) (((REGNO) >= HARD_X_REGNUM) \
    && ((REGNO) <= HARD_PC_REGNUM))

#define G_REGNO_P(REGNO) ((REGNO) <= HARD_PC_REGNUM)

#define D_REG_P(X) (REG_P (X) && D_REGNO_P (REGNO (X)))
#define I_REG_P(X) (REG_P (X) && I_REGNO_P (REGNO (X)))
#define A_REG_P(X) (REG_P (X) && A_REGNO_P (REGNO (X)))
#define Q_REG_P(X) (REG_P (X) && Q_REGNO_P (REGNO (X)))

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS I_REGS
#define BASE_REG_CLASS A_REGS

/* Get reg_class from a letter in the machine description.  */
/* 'a' -- Address (HI) class */
/* 'd' -- Data (HI) class */
/* 'q' -- Data class for byte mode (QI) */

#define REG_CLASS_FROM_LETTER(C) \
  (((C) == 'a' ? A_REGS : \
   ((C) == 'd' ? D_REGS : \
	((C) == 'x' ? I_REGS : \
   ((C) == 'q' ? Q_REGS : NO_REGS)))))

/*--------------------------------------------------------------
   The letters I, J, K, L and M in a register constraint string
   can be used to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.

   For the 6809, J, K, L are used for indexed addressing.
   `I' is used for the constant 1.
   `J' is used for the 5-bit offsets.
   `K' is used for the 8-bit offsets.
   `L' is used for the range of signed numbers that fit in 16 bits.
	`N' is used for the constant -1.
   `O' is used for the constant 0.
--------------------------------------------------------------*/

#define CONST_OK_FOR_LETTER_P(VALUE, C)         \
  ((C) == 'I' ? ((unsigned)(VALUE) == 1) :      \
   (C) == 'J' ? ((unsigned)(VALUE) <= 31) :     \
   (C) == 'K' ? ((unsigned)(VALUE) <= 255) :    \
   (C) == 'L' ? ((unsigned)(VALUE) <= 0xffff) : \
   (C) == 'N' ? ((VALUE) == -1) :               \
   (C) == 'O' ? ((unsigned)(VALUE) == 0) : 0)

/* Similar, but for floating constants, and defining letters G and H.
   No floating-point constants are valid on MC6809.  */
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  0

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.  */

/*   On the 6809, use the data reg if possible when the
   value is a constant in the range where ldb could be used
   and we ensure that HImodes are reloaded into data regs.  */

#define PREFERRED_RELOAD_CLASS(X,CLASS)  \
  ((GET_CODE (X) == CONST_INT			\
    && (unsigned) (INTVAL (X) + 0x80) < 0x100	\
    && (CLASS) != A_REGS)			\
   ? Q_REGS					\
   : (GET_MODE (X) == QImode && (CLASS) != A_REGS)	\
   ? Q_REGS					\
   : (CLASS))

#define SMALL_REGISTER_CLASSES  1

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
#define CLASS_MAX_NREGS(CLASS, MODE) \
    ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/*--------------------------------------------------------------
	Stack layout; function entry, exit and calling.
--------------------------------------------------------------*/

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Nonzero if we need to generate stack-probe insns.
   On most systems they are not needed.
   When they are needed, define this as the stack offset to probe at.  */
/*#define NEED_PROBE 0*/  /* Not required for 6809 */

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD

/* Define this if should default to -fcaller-saves.  */
/*** #define DEFAULT_CALLER_SAVES ***/	/* */

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET 0

/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by. */
/* No rounding or alignment needed for MC6809 */
#define PUSH_ROUNDING(BYTES) (BYTES)

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL) 2

/* Value is 1 if returning from a function call automatically
   pops the arguments described by the number-of-args field in the call.
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name. */
/* The standard MC6809 call, with arg count word, includes popping the
   args as part of the call template.  We optionally omit the arg count
   word and let gcc combine the arg pops. */
#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) ((TARGET_ARGCOUNT) ? (SIZE) : 0)

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */

/* All return values are in the D-register (register 0) */
#ifdef CONFIG_AB
#define FUNCTION_VALUE(VALTYPE, FUNC) \
  gen_rtx (REG, TYPE_MODE (VALTYPE), \
  (TYPE_MODE (VALTYPE) == QImode) ? HARD_B_REGNUM : HARD_D_REGNUM)
#else
#define FUNCTION_VALUE(VALTYPE, FUNC) \
  gen_rtx (REG, TYPE_MODE (VALTYPE), HARD_D_REGNUM)
#endif

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

/* All return values are in the D-register (register 0) */
#define LIBCALL_VALUE(MODE)  gen_rtx (REG, MODE, HARD_D_REGNUM)

/* Define this if PCC uses the nonreentrant convention for returning
   structure and union values.  */
#define PCC_STATIC_STRUCT_RETURN

/* 1 if N is a possible register number for a function value. */
#define FUNCTION_VALUE_REGNO_P(N) \
  (((N) == HARD_D_REGNUM) || ((N) == HARD_B_REGNUM))

/* Define this to be true when FUNCTION_VALUE_REGNO_P is true for
   more than one register.  */
#define NEEDS_UNTYPED_CALL 1

/* 1 if N is a possible register number for function argument passing. */
#ifdef CONFIG_REG_ARGS
#define FUNCTION_ARG_REGNO_P(N) ((N) == HARD_D_REGNUM)
#else
#define FUNCTION_ARG_REGNO_P(N) 0
#endif

/*--------------------------------------------------------------
	Argument Lists
--------------------------------------------------------------*/
/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.

   This is a single integer, which is a number of bytes
   of arguments scanned so far.  */

#define CUMULATIVE_ARGS int


/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

#ifdef CONFIG_REG_ARGS
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT) \
	((CUM) = m6809_init_cumulative_args (CUM, FNTYPE, LIBNAME, INDIRECT))
#else
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT) ((CUM) = 0)
#endif


/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */

#ifdef CONFIG_REG_ARGS
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
	(((CUM) == -1) ?  \
		((CUM) = 0) : \
		((CUM) += ((MODE) != BLKmode \
			? (GET_MODE_SIZE (MODE)) \
			: (int_size_in_bytes (TYPE)))))
#else
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
 ((CUM) += ((MODE) != BLKmode \
	    ? (GET_MODE_SIZE (MODE)) \
	    : (int_size_in_bytes (TYPE))))
#endif


/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register rtx in which to store the argument.
	This macro is used _before_ FUNCTION_ARG_ADVANCE.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */

#ifdef CONFIG_REG_ARGS
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
	((((CUM) >= 0) || (MODE == BLKmode)) ? 0 : \
		gen_rtx_REG (MODE, HARD_D_REGNUM))
#else
/* Old style : all args are pushed. */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) 0
#endif

/* This macro generates the assembly code for function entry.
   FILE is a stdio stream to output the code to.
   SIZE is an int: how many units of temporary storage to allocate.
   Refer to the array `regs_ever_live' to determine which registers
   to save; `regs_ever_live[I]' is nonzero if register number I
   is ever used in the function.  This macro is responsible for
   knowing which registers should not be saved even if used.  */

/*--------------------------------------------------------------
    Function Prologue Definition
--------------------------------------------------------------*/

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO) \
   fprintf (FILE, "\tleax LP%d,a1\n\tjsr mcount\n", (LABELNO));

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */
/* Setting this to 1 seems to be wrong (df) */

#define EXIT_IGNORE_STACK 0

/*****************************************************************************
**
** Trampolines for Nested Functions
**
*****************************************************************************/

/* Output assembler code for a block containing the constant parts
   of a trampoline, leaving space for the variable parts.  */
#define TRAMPOLINE_TEMPLATE(FILE) { \
  fprintf (FILE, "\t.bogus\t\t; TRAMPOLINE_TEMPLATE unimplemented\n"); }

/* Length in units of the trampoline for entering a nested function.  */
#define TRAMPOLINE_SIZE         0

/* A C statement to initialize the variable parts of a trampoline.
   ADDR is an RTX for the address of the trampoline; FNADDR is an
   RTX for the address of the nested function; STATIC_CHAIN is an
   RTX for the static chain value that should be passed to the
   function when it is called.  */
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT) { \
        }


/*--------------------------------------------------------------
	Function Epilogue Definition
----------------------------------------------------------------
   This macro generates the assembly code for function exit,
   on machines that need it.  If FUNCTION_EPILOGUE is not defined
   then individual return instructions are generated for each
   return statement.  Args are same as for FUNCTION_PROLOGUE.
--------------------------------------------------------------*/

/* If the memory address ADDR is relative to the frame pointer,
   correct it to be relative to the stack pointer instead.
   This is for when we don't use a frame pointer.
   ADDR should be a variable name.  */

#define FIX_FRAME_POINTER_ADDRESS(ADDR,DEPTH) ADDR += DEPTH;

/*--------------------------------------------------------------
	Addressing modes,
	and classification of registers for them.
--------------------------------------------------------------*/

#define HAVE_POST_INCREMENT  1

#define HAVE_PRE_DECREMENT  1

/* Macros to check register numbers against specific register classes.  */
#define HARD_OK_FOR_BASE_P(REGNO)  A_REGNO_P (REGNO)

/* MC6809 index registers do not allow scaling, */
/* but there is "accumulator-offset" mode. */
#define HARD_OK_FOR_INDEX_P(REGNO) I_REGNO_P (REGNO)

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

#define REGNO_OK_FOR_BASE_P(REGNO) \
    (HARD_OK_FOR_BASE_P (REGNO) \
    || (((unsigned) reg_renumber[REGNO]) \
	&& HARD_OK_FOR_BASE_P ((unsigned) reg_renumber[REGNO])))

#define REGNO_OK_FOR_INDEX_P(REGNO) \
    (HARD_OK_FOR_INDEX_P (REGNO) \
    || (((unsigned) reg_renumber[REGNO]) \
	&& HARD_OK_FOR_INDEX_P ((unsigned) reg_renumber[REGNO])))

/* Maximum number of registers that can appear in a valid memory address */
#define MAX_REGS_PER_ADDRESS 2

/* 1 if X is an rtx for a constant that is a valid address.  */
#define CONSTANT_ADDRESS_P(X) (CONSTANT_P (X))
/* Note: much better on m68k:
#define CONSTANT_ADDRESS_P(X)   \
  (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF              \
   || GET_CODE (X) == CONST_INT || GET_CODE (X) == CONST                \
   || GET_CODE (X) == HIGH)
*/

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */
/* Any single-word constant is ok; the only contexts
   allowing general_operand of mode DI or DF are movdi and movdf. */
#define LEGITIMATE_CONSTANT_P(X) (GET_CODE (X) != CONST_DOUBLE)

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */

#ifndef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */
#define REG_OK_FOR_BASE_P(X) \
    ((REGNO(X) >= FIRST_PSEUDO_REGISTER) \
    || HARD_OK_FOR_BASE_P (REGNO (X)))

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define REG_OK_FOR_INDEX_P(X) \
    ((REGNO(X) >= FIRST_PSEUDO_REGISTER) \
    || HARD_OK_FOR_INDEX_P (REGNO (X)))

#else

/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X) (REGNO_OK_FOR_BASE_P (REGNO (X)))

/* Nonzero if X is a hard reg that can be used as an index.  */
#define REG_OK_FOR_INDEX_P(X) (REGNO_OK_FOR_INDEX_P (REGNO (X)))

#endif

/*--------------------------------------------------------------
	Test for valid memory addresses
--------------------------------------------------------------*/
/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression
   that is a valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address. */

/*--------------------------------------------------------------
   Valid addresses are either direct or indirect (MEM) versions
   of the following forms.
	constant		N
	register		,X
	constant indexed	N,X
	accumulator indexed	D,X
	auto_increment		,X++
	auto_decrement		,--X
--------------------------------------------------------------*/

#define REGISTER_ADDRESS_P(X) \
  (GET_CODE (X) == REG && REG_OK_FOR_BASE_P (X))

#define EXTENDED_ADDRESS_P(X) \
    CONSTANT_ADDRESS_P (X) \

#define LEGITIMATE_BASE_P(X) \
  ((GET_CODE (X) == REG && REG_OK_FOR_BASE_P (X))	\
   || (GET_CODE (X) == SIGN_EXTEND			\
       && GET_CODE (XEXP (X, 0)) == REG			\
       && GET_MODE (XEXP (X, 0)) == HImode		\
       && REG_OK_FOR_BASE_P (XEXP (X, 0))))

#define LEGITIMATE_OFFSET_P(X) \
    (CONSTANT_ADDRESS_P (X) \
    || (GET_CODE (X) == REG && REG_OK_FOR_INDEX_P (X)))

/* 1 if X is the sum of a base register and an offset. */
#define INDEXED_ADDRESS(X) \
   ((GET_CODE (X) == PLUS \
       && LEGITIMATE_BASE_P (XEXP (X, 0)) \
       && LEGITIMATE_OFFSET_P (XEXP (X, 1))) \
   || (GET_CODE (X) == PLUS \
       && LEGITIMATE_BASE_P (XEXP (X, 1)) \
       && LEGITIMATE_OFFSET_P (XEXP (X, 0))))

#define PUSH_POP_ADDRESS_P(X) \
    (((GET_CODE (X) == PRE_DEC) || (GET_CODE (X) == POST_INC)) \
	&& (LEGITIMATE_BASE_P (XEXP (X, 0))))

/* Go to ADDR if X is a valid address. */
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR) \
{ \
  if (REGISTER_ADDRESS_P(X)) goto ADDR; \
  if (PUSH_POP_ADDRESS_P (X)) goto ADDR; \
  if (EXTENDED_ADDRESS_P (X)) goto ADDR; \
  if (INDEXED_ADDRESS (X)) goto ADDR; \
  if ((GET_CODE (X) == MEM) && REGISTER_ADDRESS_P(XEXP (X, 0))) goto ADDR; \
  if ((GET_CODE (X) == MEM) && PUSH_POP_ADDRESS_P (XEXP (X, 0))) goto ADDR; \
  if ((GET_CODE (X) == MEM) && EXTENDED_ADDRESS_P (XEXP (X, 0))) goto ADDR; \
  if ((GET_CODE (X) == MEM) && INDEXED_ADDRESS (XEXP (X, 0))) goto ADDR; \
}

/*--------------------------------------------------------------
	Address Fix-up
--------------------------------------------------------------*/
/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.
   It exists to recognize opportunities to optimize the output.
   --------*/

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN) \
{ \
}

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for. */

#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL) \
{ if (GET_CODE (ADDR) == POST_INC || GET_CODE (ADDR) == PRE_DEC) \
    goto LABEL; \
}

/*--------------------------------------------------------------
	Cross-Compilation and Floating Point Format
--------------------------------------------------------------*/
/*** TBD ***/

/*--------------------------------------------------------------
	Miscellaneous Parameters
--------------------------------------------------------------*/
/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE Pmode

/* Define this if the case instruction expects the table
   to contain offsets from the address of the table.
   Do not define this if the table should contain absolute addresses.  */
/* #define CASE_VECTOR_PC_RELATIVE */

/* Define this if the case instruction drops through after the table
   when the index is out of range.  Don't define it if the case insn
   jumps to the default label instead.  */
/* #define CASE_DROPS_THROUGH */

/* Specify the tree operation to be used to convert reals to integers. */
/* #define IMPLICIT_FIX_EXPR FIX_ROUND_EXPR */

/* This is the kind of divide that is easiest to do in the general case.  */
/* #define EASY_DIV_EXPR TRUNC_DIV_EXPR */

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* Don't cse the address of the function being compiled.  */
#define NO_RECURSIVE_FUNCTION_CSE

/* define SCCS_DIRECTIVE if SCCS directives should be ignored */
#if (TARGET_GCC_VERSION < 3003)
#define SCCS_DIRECTIVE 1
#endif

/* This flag, if defined, says the same insns that convert to a signed fixnum
   also convert validly to an unsigned one.  */
#define FIXUNS_TRUNC_LIKE_FIX_TRUNC

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 2

/* Size (bits) of the type "int" on target machine */
/*   (If undefined, default is BITS_PER_WORD).  */
#ifdef CONFIG_BYTE_INT
#define INT_TYPE_SIZE 8
#else
#define INT_TYPE_SIZE (16)
#endif

/* Size (bits) of the type "short" on target machine */
#ifdef CONFIG_BYTE_INT
#define SHORT_TYPE_SIZE 8
#else
#define SHORT_TYPE_SIZE 16
#endif

/* Size (bits) of the type "long" on target machine */
#define LONG_TYPE_SIZE 16

/* Size (bits) of the type "long long" on target machine */
#define LONG_LONG_TYPE_SIZE 32

/* Size (bits) of the type "char" on target machine */
#define CHAR_TYPE_SIZE 8

/* Size (bits) of the type "float" on target machine */
#define FLOAT_TYPE_SIZE 32

/* Size (bits) of the type "double" on target machine */
#define DOUBLE_TYPE_SIZE 64

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS 0

/* Define if shifts truncate the shift count
   which implies one can omit a sign-extension or zero-extension
   of a shift count.  */
#define SHIFT_COUNT_TRUNCATED 0

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* It is as good to call a constant function address as to
   call an address kept in a register. */
#define NO_FUNCTION_CSE

/* When a prototype says `char' or `short', really pass an `int'.  */
/* When int is sizeof(char), this doesn't really matter. */
#undef PROMOTE_PROTOTYPES
/* #define PROMOTE_PROTOTYPES 1 */

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode HImode

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE HImode

/* Define TARGET_MEM_FUNCTIONS if we want to use calls to memcpy and
 * memset, instead of the BSD functions bcopy and bzero.  */
#define TARGET_MEM_FUNCTIONS

/* Compute the cost of computing a constant rtl expression RTX
   whose rtx-code is CODE.  The body of this macro is a portion
   of a switch statement.  If the code is computed here,
   return it with a return statement.
   Otherwise, break from the switch.  */

#define CONST_COSTS(RTX,CODE,OUTER_CODE) \
 case CONST: \
    /* Constant zero is super cheap due to clr instruction.  */ \
    if (RTX == const0_rtx) return 0;                            \
    if ((unsigned) INTVAL (RTX) < 077) return 1;                \
 case LABEL_REF: \
 case SYMBOL_REF: \
 case CONST_INT: \
   return 2; \
 case CONST_DOUBLE: \
   return 4;

/* April, 10th 1997 recommended by U.Boetzel */
#define RTX_COSTS(X,CODE,OUTER_CODE)                   \
  case PLUS:                                 \
  case MINUS:                                                   \
  case COMPARE:                                   \
  case AND:                                  \
  case IOR:                                  \
  case XOR:                                  \
    if (GET_MODE (X) == HImode                         \
     && GET_CODE (XEXP (X, 0)) == REG             \
     && GET_CODE (XEXP (X, 1)) == CONST_INT)                 \
      return COSTS_N_INSNS(1);                                  \
    if (GET_MODE (X) == HImode                         \
     && GET_CODE (XEXP (X, 0)) == REG             \
     && GET_CODE (XEXP (X, 1)) == REG)                  \
      return COSTS_N_INSNS(2);                                  \
    break;                                                      \
  case ASHIFT:                               \
  case ASHIFTRT:                             \
  case LSHIFTRT:                                                \
  case ROTATE:                                    \
  case ROTATERT:                                                \
    if (GET_CODE (XEXP (X, 1)) == CONST_INT)           \
      return COSTS_N_INSNS (4) + INTVAL (XEXP (X, 1));      \
    break;                                   \
  case MULT:                                 \
    if(GET_MODE(X)==HImode || GET_MODE(X)==SImode)          \
      return COSTS_N_INSNS (50);                  \
    else                                \
      return COSTS_N_INSNS (11);                  \
    break;                                   \
  case DIV:                                  \
  case UDIV:                                 \
  case MOD:                                  \
  case UMOD:                                 \
    if(GET_MODE(X)==HImode || GET_MODE(X)==SImode)          \
      return COSTS_N_INSNS (100);                 \
    else                                \
      return COSTS_N_INSNS (50);                  \
    break;
 
 
#define REGISTER_MOVE_COST(MODE, CLASS1, CLASS2) 7
 
#define MEMORY_MOVE_COST(MODE, CLASS, IN) 5
 


/* Check a `double' value for validity for a particular machine mode.  */

#define CHECK_FLOAT_VALUE(MODE, D, OVERFLOW) \
  ((OVERFLOW) = check_float_value (MODE, &D, OVERFLOW))


/*--------------------------------------------------------------
	machine-dependent
--------------------------------------------------------------*/
/* Tell final.c how to eliminate redundant test instructions.  */

/* Here we define machine-dependent flags and fields in cc_status
   (see `conditions.h').  */

/* Store in cc_status the expressions
   that the condition codes will describe
   after execution of an instruction whose pattern is EXP.
   Do not alter them if the instruction would not alter the cc's.  */

/* On the 6809, most of the insns to store in an address register
   fail to set the cc's.  However, in some cases these instructions
   can make it possibly invalid to use the saved cc's.  In those
   cases we clear out some or all of the saved cc's so they won't be used.  */

#define NOTICE_UPDATE_CC(EXP, INSN) \
  notice_update_cc((EXP), (INSN))

/*****************************************************************************
**
** pragma support
**
*****************************************************************************/


#define REGISTER_TARGET_PRAGMAS(PFILE) \
do { \
	extern void pragma_short_branch PARAMS ((cpp_reader *)); \
	extern void pragma_long_branch PARAMS ((cpp_reader *)); \
	extern void pragma_interrupt PARAMS ((cpp_reader *)); \
	extern void pragma_naked PARAMS ((cpp_reader *)); \
	extern void pragma_section PARAMS ((cpp_reader *)); \
	cpp_register_pragma (PFILE, 0, "short_branch", pragma_short_branch); \
	cpp_register_pragma (PFILE, 0, "long_branch", pragma_long_branch); \
	cpp_register_pragma (PFILE, 0, "interrupt", pragma_interrupt); \
	cpp_register_pragma (PFILE, 0, "naked", pragma_naked); \
	cpp_register_pragma (PFILE, 0, "section", pragma_section); \
} while (0)

/*--------------------------------------------------------------
	ASSEMBLER FORMAT
--------------------------------------------------------------*/
/* Output at beginning of assembler file.  */
/* TODO : we're using default for gcc 3.4, need to replace */
#if (TARGET_GCC_VERSION < 3004)

#define ASM_FILE_START(FILE) \
    fprintf (FILE, ";;; gcc built %s %s\n", __DATE__, __TIME__ ); \
    fprintf (FILE, ";;; MC6809 $Revision: 1.9 $\n"); \
    print_options (FILE); 

#define ASM_FILE_END(FILE) /* */
/*    fprintf (FILE, "\tend\n") */

#endif /* TARGET_GCC_VERSION */

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */
#define ASM_APP_ON "; Begin inline assembler code\n"

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */
#define ASM_APP_OFF "; End of inline assembler code\n"

/* Output assembly directives to switch to section 'name' */
#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION	m6809_asm_named_section

#undef TARGET_HAVE_NAMED_SECTION
#define TARGET_HAVE_NAMED_SECTION m6809_have_named_section


/* Output before read-only data.  */
#define TEXT_SECTION_ASM_OP (code_section_op)

/* Output before writable data.  */
#define DATA_SECTION_ASM_OP (data_section_op)

/* Output before uninitialized data.  */
#define BSS_SECTION_ASM_OP (bss_section_op)

/* Support the ctors and dtors sections for g++.  */
 
#undef CTORS_SECTION_ASM_OP
#define CTORS_SECTION_ASM_OP    "\t.area _CTORS"
#undef DTORS_SECTION_ASM_OP
#define DTORS_SECTION_ASM_OP    "\t.area _DTORS"


/* #define EXTRA_SECTIONS in_ctors, in_dtors */

/* A list of extra section function definitions.  */
 
#define CTORS_SECTION_FUNCTION                                          \
void                                                                    \
ctors_section ()                                                        \
{                                                                       \
  if (in_section != in_ctors)                                           \
    {                                                                   \
      fprintf (asm_out_file, "%s\n", CTORS_SECTION_ASM_OP);             \
      in_section = in_ctors;                                            \
    }                                                                   \
}
 
#define DTORS_SECTION_FUNCTION                                          \
void                                                                    \
dtors_section ()                                                        \
{                                                                       \
  if (in_section != in_dtors)                                           \
    {                                                                   \
      fprintf (asm_out_file, "%s\n", DTORS_SECTION_ASM_OP);             \
      in_section = in_dtors;                                            \
    }                                                                   \
}

/* A C statement (sans semicolon) to output an element in the table of
   global constructors.  */
#undef TARGET_ASM_OUTPUT_CONSTRUCTOR
#define TARGET_ASM_OUTPUT_CONSTRUCTOR(FILE,NAME)                               \
  do {                                                                  \
    ctors_section ();                                                   \
    fprintf (FILE, "\t%s\t ", ".word");                              \
    assemble_name (FILE, NAME);                                         \
    fprintf (FILE, "\n");                                               \
  } while (0)
 
/* A C statement (sans semicolon) to output an element in the table of
   global destructors.  */
#undef TARGET_ASM_OUTPUT_DESTRUCTOR
#define TARGET_ASM_OUTPUT_DESTRUCTOR(FILE,NAME)                                \
  do {                                                                  \
    dtors_section ();                                                   \
    fprintf (FILE, "\t%s\t ", ".word");                              \
    assemble_name (FILE, NAME);                                         \
    fprintf (FILE, "\n");                                               \
  } while (0)
 
#undef DO_GLOBAL_CTORS_BODY
#undef DO_GLOBAL_DTORS_BODY



/* Taken from ../sparc/svr4.h */
/* A C statement to output something to the assembler file to switch to section
   NAME for object DECL which is either a FUNCTION_DECL, a VAR_DECL or
   NULL_TREE.  Some target formats do not support arbitrary sections.  Do not
   define this macro in such cases.  */
 
#undef  ASM_OUTPUT_SECTION_NAME /* Override earlier definition.  */
#if 000
#define ASM_OUTPUT_SECTION_NAME(FILE, DECL, NAME, RELOC) \
do {                                             \
  fprintf (FILE, "\t.area %s\n", (NAME));         \
} while (0)
#endif


/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG) \
  if ((LOG) > 1) \
    fprintf (FILE, "\t.align %d\n", 1 << (LOG))

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */

#define REGISTER_NAMES \
{"d", "x", "y", "u", "s", "pc","-", "-", \
 "a", "b", "cc","dp",}

/* This is BSD, so it wants DBX format.  */

#define DBX_DEBUGGING_INFO

/* How to renumber registers for dbx and gdb. */

#define DBX_REGISTER_NUMBER(REGNO) (REGNO)

/* Do not break .stabs pseudos into continuations.  */

#define DBX_CONTIN_LENGTH 0

/* This is the char to use for continuation (in case we need to turn
   continuation back on).  */

#define DBX_CONTIN_CHAR '?'

/* Don't use the `xsfoo;' construct in DBX output; this system
   doesn't support it.  */

#define DBX_NO_XREFS

/* This is how to output a note to DBX telling it the line number
   to which the following sequence of instructions corresponds.
 
   This is taken from sparc.h */
#define ASM_OUTPUT_SOURCE_LINE(file, line)              \
  { static int sym_lineno = 1;                          \
    fprintf (file, ".stabn 68,0,%d,LM%d\nLM%d:\n",      \
             line, sym_lineno, sym_lineno);             \
    sym_lineno += 1; }


/*****************************************************************************
**
** Output and Generation of Labels
**
*****************************************************************************/

/* This is how to output the definition of a user-level label named NAME,
   such as the label on a static function or variable NAME.  */

#define ASM_OUTPUT_LABEL(FILE,NAME) \
do { \
  if (section_changed) { \
	  fprintf (FILE, "\n%s\n\n", code_section_op); \
     section_changed = 0; \
  } \
  assemble_name (FILE, NAME); \
  fputs (":\n", FILE); \
} while (0)

/* This is how to output a command to make the user-level label
    named NAME defined for reference from other files.  */

#if (TARGET_GCC_VERSION < 3003)
#define ASM_GLOBALIZE_LABEL(FILE,NAME) \
  {fputs ("\t.globl ", FILE); \
  assemble_name (FILE, NAME); \
  fputs ("\n", FILE);}
#else
#define GLOBAL_ASM_OP "\t.globl "
#endif

/* This is how to output a reference to a user label named NAME. */
#define ASM_OUTPUT_LABELREF(FILE,NAME) \
  fprintf (FILE, "_%s", NAME)

/* output external reference -- tej */
#define ASM_OUTPUT_EXTERNAL(FILE,DECL,NAME) \
  {fputs ("\t; extern\t", FILE); \
  assemble_name (FILE, NAME); \
  fputs ("\n", FILE);}

/* This is how to output an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.  */

#define ASM_OUTPUT_INTERNAL_LABEL(FILE,PREFIX,NUM) \
  fprintf (FILE, "%s%d:\n", PREFIX, NUM)

#define ASM_OUTPUT_CASE_LABEL(FILE,PREFIX,NUM,TABLE) \
  ASM_OUTPUT_ALIGN (FILE, 1); \
  ASM_OUTPUT_INTERNAL_LABEL (FILE, PREFIX, NUM)

#define ASM_OUTPUT_CASE_END(FILE,NUM,TABLE) \
  ASM_OUTPUT_ALIGN (FILE, 1)

/* This is how to store into the string LABEL
   the symbol_ref name of an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.
   This is suitable for output with `assemble_name'.  */

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM) \
  sprintf (LABEL, "*%s%d", PREFIX, NUM)

/* This is how to output an assembler line defining a `double' constant.  */

#define ASM_OUTPUT_DOUBLE(FILE,VALUE) \
  fprintf (FILE, "\t.bogus dd %.17#g\n", (VALUE))

/* This is how to output an assembler line defining a `float' constant.  */

#define ASM_OUTPUT_FLOAT(FILE,VALUE) \
  fprintf (FILE, "\t.bogus df %.9#g\n", (VALUE))

/* This is how to output an assembler line defining an `int' constant.  */

#define ASM_OUTPUT_INT(FILE,VALUE) \
( fprintf (FILE, "\t.word "), \
  output_addr_const (FILE, (VALUE)), \
  fprintf (FILE, "\n"))

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE,VALUE) \
( fprintf (FILE, "\t.word "), \
  output_addr_const (FILE, (VALUE)), \
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_CHAR(FILE,VALUE) \
( fprintf (FILE, "\t.byte "), \
  output_addr_const (FILE, (VALUE)), \
  fprintf (FILE, "\n"))

/* This is how to output an assembler line for a numeric constant byte.  */

#define ASM_OUTPUT_BYTE(FILE,VALUE) \
  fprintf (FILE, "\t.byte %#x\n", (VALUE))

/* This is how to output a string */

#define ASM_OUTPUT_ASCII(FILE,STR,SIZE) do { \
  int i; \
  fprintf (FILE, "\t.ascii \""); \
  for (i = 0; i < (SIZE); i++) { \
      register int c = (STR)[i] & 0377; \
      if (c >= ' ' && c < 0177 && c != '\\' && c != '"') \
	  putc (c, FILE); \
      else { \
	  switch (c) { \
	  case TARGET_CR: \
	    fputs ("\\r", FILE); \
	    break; \
	  case TARGET_NEWLINE: \
	    fputs ("\\n", FILE); \
	    break; \
	  case TARGET_TAB: \
	    fputs ("\\t", FILE); \
	    break; \
	  case TARGET_FF: \
	    fputs ("\\f", FILE); \
	    break; \
	  case 0: \
	    fputs ("\\0", FILE); \
	    break; \
	  default: \
	    fprintf (FILE, "\\%03o", c); \
	    break; \
	  } \
      } \
    } \
  fprintf (FILE, "\"\n");} while (0)

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO) \
   fprintf (FILE, "\tpshs\t%s\n", \
	    reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE,REGNO) \
   fprintf (FILE, "\tpuls\t%s\n", \
	    reg_names[REGNO])

/* This is how to output an element of a case-vector that is absolute. */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE) \
  fprintf (FILE, "\t.word L%d\n", VALUE)

/* This is how to output an element of a case-vector that is relative.  
*/

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL) \
  fprintf (FILE, "\t.word L%d-L%d\n", VALUE, REL)

/* This is how to output an assembler line
   that says to advance the location counter by SIZE bytes.  */

/*****************************************************************************
**
** Assembler Commands for Alignment
**
*****************************************************************************/

#define ASM_OUTPUT_SKIP(FILE,SIZE) \
  fprintf (FILE, "\t.blkb\t%d\t;reserve space\n", (SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED) \
  do {bss_section (); \
  fputs ("\t.globl\t", FILE); \
  assemble_name ((FILE), (NAME)); \
  fputs ("\n", FILE); \
  assemble_name ((FILE), (NAME)); \
  fprintf ((FILE), ":\t.blkb\t%d\n", (ROUNDED));} while(0)

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED) \
do {bss_section (); \
  assemble_name ((FILE), (NAME)); \
  fprintf ((FILE), ":\t.blkb\t%d\n", (ROUNDED));} while(0)

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO) \
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10), \
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* Define the parentheses used to group arithmetic operations
   in assembler code.  */

#undef TARGET_ASM_OPEN_PAREN
#define TARGET_ASM_OPEN_PAREN "("

#undef TARGET_ASM_CLOSE_PAREN
#define TARGET_ASM_CLOSE_PAREN ")"

/* Define results of standard character escape sequences.  */
#define TARGET_BELL 007
#define TARGET_BS 010
#define TARGET_TAB 011
#define TARGET_ESC 0x1B
#define TARGET_NEWLINE 012
#define TARGET_VT 013
#define TARGET_FF 014
#define TARGET_CR 015

/* Print an instruction operand X on file FILE.
   CODE is the code from the %-spec for printing this operand.
   If `%z3' was used to print operand 3, then CODE is 'z'. */

#define PRINT_OPERAND(FILE, X, CODE) print_operand (FILE, X, CODE)

/* Print a memory operand whose address is X, on file FILE. */
#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)

