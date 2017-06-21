/* $Id: more.c,v 1.1.4.1 1996/05/09 15:00:27 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/more.c,v $
 *------------------------------------------------------------------
 * more.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: more.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:27  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:09  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:05  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <termio.h>
#include <mips.h>
#include "pmon.h"

#define BEL 007

char more_pat[LINESZ];
int tik_cnt;
char more_tiks[] = "|/-\\";
char *more_tik;
char *more_msg = "more... ";

Optdesc more_opts[] = {
	{"/str","search for str"},
	{"n","repeat last search"},
	{"SPACE","next page"},
	{"CR","next line"},
	{"q","quit"},
	{0}};

/*************************************************************
*  more(p,cnt,size)
*	The 'more' paginator 	
*/
more(p,cnt,size)
char *p;
int *cnt,size;
{
int r;

if (*cnt == 0) {
	if (size == 0) return(1); /* if cnt=0 and size=0 we're done */
	while ((r = getinp(cnt,size)) == -1) ;
	if (r) /* quit */ {
		*p = 0;
		return(1);
		}
	}

if (*cnt == -1) { /* search in progress */
	if (!strposn(p,more_pat)) {
		dotik(256,0);
		return(0); /* not found yet */
		}
	else { /* found */
		*cnt = size;
		printf("\b \n");
		}
	}

printf("%s\n",p);
*p = 0;
(*cnt)--;
return(0);
}

/*************************************************************
*  getinp(cnt,size)
*/
getinp(cnt,size)
int *cnt,size;
{
int i,c;

printf("%s",more_msg);
loop {
	c = getchar();
	if (strchr("nq/ \n",c)) break;
	putchar(BEL);
	}
era_line(more_msg);
if (c == 'q') return(1);
switch (c) {
	case ' ' : *cnt = size; 	break;
	case '\n': *cnt = 1;		break;
	case '/' : 
		/* get pattern */
		putchar('/');
		for (i=0;;) {
			c = getchar();
			if (c == '\n') break;
			if (c == '\b') {
				if (i > 0) {
					putchar(c);
					i--;
					}
				else {
					putchar('\b');
					return(-1);
					}
				}
			else {
				putchar(c);
				more_pat[i++] = c;
				}
			}
		more_pat[i] = 0;
		printf("  ");
		*cnt = -1; /* enter search mode */
		break;
	case 'n' : 
		printf("/%s  ",more_pat);
		*cnt = -1; /* enter search mode */
		break;
	}
return(0);
}

/*************************************************************
*  dotik(rate,use_ret)
*/
dotik(rate,use_ret)
int rate,use_ret;
{

tik_cnt -= rate;
if (tik_cnt > 0) return;
tik_cnt = TIKRATE;
if (more_tik == 0) more_tik = more_tiks;
if (*more_tik == 0) more_tik = more_tiks;
if (use_ret) printf(" %c\r",*more_tik);
else printf("\b%c",*more_tik);
more_tik++;
}

