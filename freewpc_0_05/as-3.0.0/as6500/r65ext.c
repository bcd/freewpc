/* r65ext.c */

/*
 * (C) Copyright 1995-2000
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

/*
 * With Contributions from
 *
 * Marko Makela
 * Sillitie 10 A
 * 01480 Vantaa
 * Finland
 * Internet: Marko.Makela@Helsinki.Fi
 * EARN/BitNet: msmakela@finuh
 */

#include <stdio.h>
#include <setjmp.h>
#include "asxxxx.h"
#include "r6500.h"

char *cpu  = "Rockwell 6502/6510/65C02";
int hilo   = 0;
char *dsft = "asm";
