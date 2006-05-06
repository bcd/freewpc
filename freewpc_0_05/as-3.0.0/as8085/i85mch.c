/* i85mch.c */

/*
 * (C) Copyright 1989-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "i8085.h"

/*
 * Process machine ops.
 */
VOID
machine(mp)
struct mne *mp;
{
	register unsigned op, rd, rs;
	struct expr e;

	clrexpr(&e);
	op = mp->m_valu;
	switch (mp->m_type) {

	case S_INH:
		outab(op);
		break;

	case S_RST:
		rd = absexpr();
		if (rd > 7)
			aerr();
		out3(op, rd);
		break;

	case S_ADI:
		expr(&e, 0);
		outab(op);
		outrb(&e, 0);
		break;

	case S_ADD:
		rd = reg();
		if (rd > A)
			aerr();
		outab(op | rd);
		break;

	case S_JMP:
		expr(&e, 0);
		outab(op);
		outrw(&e, 0);
		break;

	case S_INR:
		rd = reg();
		if (rd > A)
			aerr();
		out3(op, rd);
		break;

	case S_LXI:
		rd = reg();
		comma();
		expr(&e, 0);
		out3(op, regpair(rd, SP));
		outrw(&e, 0);
		break;

	case S_LDAX:
		rd = reg();
		if (rd!=B && rd!=D)
			aerr();
		out3(op, rd);
		break;

	case S_INX:
		rd = reg();
		out3(op, regpair(rd, SP));
		break;

	case S_PUSH:
		rd = reg();
		out3(op, regpair(rd, PSW));
		break;

	case S_MOV:
		rd = reg();
		comma();
		rs = reg();
		if (rs>A || rd>A)
			aerr();
		outab(op | rs | (rd<<3));
		break;

	case S_MVI:
		rd = reg();
		comma();
		expr(&e, 0);
		if (rd > A)
			aerr();
		out3(op, rd);
		outrb(&e, 0);
		break;

	default:
		err('o');
	}
}

/*
 * Is the next character a comma ?
 */
int
comma()
{
	if (getnb() != ',')
		qerr();
	return(1);
}

/*
 * Output a | (b<<3);
 */
VOID
out3(a, b)
int a;
int b;
{
	outab(a | (b<<3));
}

/*
 * Make sure that `r' is usable as a
 * register pair specifier. The extra
 * register (code 3) is `s'.
 */
int
regpair(r, s)
int r;
int s;
{
	if (r < M) {
		if (r&01)
			aerr();
	} else  if (r == s)
		r = 6;
	else
		aerr();
	return (r);
}

/*
 * Read a register name.
 */
int
reg()
{
	register struct mne *mp;
	char id[NCPS];

	getid(id, -1);
	if ((mp = mlookup(id))==NULL || mp->m_type!=S_REG) {
		aerr();
		return (0);
	}
	return (mp->m_valu);
}

/*
 * Dummy machine specific init.
 */
VOID
minit()
{
}
