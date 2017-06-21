/* $Id: demo.c,v 1.1.4.1 1996/05/09 15:00:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/demo.c,v $
 *------------------------------------------------------------------
 * demo.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: demo.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:55  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <mips.h>
#include <stdio.h>
#include <termio.h>
#include "pmon.h"

#undef SABLE

/*************************************************************
*  demo.c
*	This module can be used to include a demo program with
*	PMON, 'hanoi' is one such demo.
*/

int hanoi();
int sinewave();
int ftest();
int fptst();
int ldsym();

Demo demo[] = {
#ifdef INCLUDE_HANOI
	{"hanoi",hanoi}, 
#endif /* INCLUDE_HANOI */
#ifdef SABLE
	{"ldsym",ldsym},
#endif
#ifdef FLOATINGPT
#ifdef SABLE
	{"fptst",fptst},
#endif /* SABLE */
#endif
	{0}};

demoinit()
{
int i;

for (i=0;demo[i].name;i++) {
	newsym(demo[i].name,demo[i].addr);
	}
}

#ifdef FLOATINGPT
#ifdef SABLE
fptst()
{
float f,x;
double y,z;

x = 4.5678;
y = 9.87654;
z = 2.3456;

f = x+y+z;
if (f >= 16.7899 && f <= 16.7900) printf("add test OK\n");
else printf("add test FAILED f=%e\n",f);

f = x-y-z;
if (f >= -7.65435 && f <= -7.65433) printf("sub test OK\n");
else printf("sub test FAILED f=%e\n",f);

f = x*y*z;
if (f >= 105.819 && f <= 105.820) printf("mult test OK\n");
else printf("mult test FAILED f=%e\n",f);

f = x/y/z;
if (f >= 0.197173 && f <= 0.197174) printf("div test OK\n");
else printf("div test FAILED f=%e\n",f);
}
#endif
#endif

#ifdef SABLE
ldsym(adr)
char *adr;
{
char buf[LINESZ];
int c;

for (;;) {
	c = getln(&adr,buf);
	if (c == 0) break;
	write(1,".",1);
	do_cmd(buf);
	}
}
#endif
