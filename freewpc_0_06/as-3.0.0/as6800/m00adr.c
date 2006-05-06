/* m00adr.c */

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
#include "m6800.h"

int
addr(esp)
register struct expr *esp;
{
	register int c;

	if ((c = getnb()) == '#') {
		expr(esp, 0);
		esp->e_mode = S_IMMED;
	} else if (c == ',') {
		esp->e_mode = S_INDX;
		if (admode(abx) != S_X)
			aerr();
	} else if (c == '*') {
		expr(esp, 0);
		esp->e_mode = S_DIR;
		if (esp->e_addr & ~0xFF)
			err('d');
		if (more()) {
			comma();
			esp->e_mode = S_INDX;
			if (admode(abx) != S_X)
				aerr();
		}
	} else {
		unget(c);
		if ((esp->e_mode = admode(abx)) != 0) {
			;
		} else {
			expr(esp, 0);
			if (more()) {
				comma();
				esp->e_mode = S_INDX;
				if (admode(abx) != S_X)
					aerr();
				if ((!esp->e_flag)
					&& (esp->e_base.e_ap==NULL)
					&& (esp->e_addr & ~0xFF))
					aerr();
			} else {
				if ((!esp->e_flag)
					&& (esp->e_base.e_ap==NULL)
					&& !(esp->e_addr & ~0xFF)) {
					esp->e_mode = S_DIR;
				} else {
					esp->e_mode = S_EXT;
				}
			}
		}
	}
	return (esp->e_mode);
}
	
/*
 * Enter admode() to search a specific addressing mode table
 * for a match. Return the addressing value on a match or
 * zero for no match.
 */
int
admode(sp)
register struct adsym *sp;
{
	register char *ptr;
	register int i;
	register char *ips;

	ips = ip;
	unget(getnb());

	i = 0;
	while ( *(ptr = &sp[i].a_str[0]) ) {
		if (srch(ptr)) {
			return(sp[i].a_val);
		}
		i++;
	}
	ip = ips;
	return(0);
}

/*
 *      srch --- does string match ?
 */
int
srch(str)
register char *str;
{
	register char *ptr;
	ptr = ip;

	while (*ptr && *str) {
		if(ccase[*ptr & 0x007F] != ccase[*str & 0x007F])
			break;
		ptr++;
		str++;
	}
	if (ccase[*ptr & 0x007F] == ccase[*str & 0x007F]) {
		ip = ptr;
		return(1);
	}

	if (!*str)
		if (any(*ptr," \t\n,];")) {
			ip = ptr;
			return(1);
		}
	return(0);
}

/*
 *      any --- does str contain c?
 */
int
any(c,str)
int c;
char *str;
{
	while (*str)
		if(*str++ == c)
			return(1);
	return(0);
}

struct adsym	abx[] = {	/* a, b, or x registers */
    {	"a",	S_A	},
    {	"b",	S_B	},
    {	"x",	S_X	},
    {	"",	0x00	}
};
