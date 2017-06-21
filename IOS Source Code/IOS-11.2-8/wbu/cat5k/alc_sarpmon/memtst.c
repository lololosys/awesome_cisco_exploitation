/* $Id: memtst.c,v 1.1.4.1 1996/05/09 15:00:25 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/memtst.c,v $
 *------------------------------------------------------------------
 * memtst.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: memtst.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:25  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:33  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:07  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:03  rlowe
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

Optdesc mt_opts[] = {
	{"-c","continuous test"},
	{0}};

#ifdef TEST
#define memtst main
int tikcnt;
dotik(a,b) 
{
tikcnt--;
if (tikcnt > 0) return;
tikcnt = 256000;
printf(".");
}
long memorysize = 8*1024*1024;

get_rsa(sp,p)
long *sp;
char *p;
{
sscanf(p,"%x",sp);
return(1);
}
#endif

/*************************************************************
*  memtst(ac,av)
*	The 'mt' memory test command 
*/
memtst(ac,av)
int ac;
char *av[];
{
int i,j,cnt,cflag,err;
unsigned long adr,siz;

cflag = 0; cnt = 0;
adr = CLIENTPC|K1BASE;
siz = memorysize;

for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j];j++) {
			if (av[i][j] == 'c') cflag = 1;
			else printf("%c: bad option\n");
			}
		}
	else {
		switch (cnt) {
			case 0 : 
				if (!get_rsa(&siz,av[i])) return; 
				cnt++;
				break;
			case 1 :
				adr = siz;
				if (!get_rsa(&siz,av[i])) return;
				cnt++;
				break;
			default :
				printf("%s: Unknown argument\n",av[i]);
			}
		}
	}

/* make sure the address is word aligned */
adr = adr & ~0x3;

siz &= ~0x3;	/* make sure it's an exact number of words */

ioctl(STDIN,CBREAK,NULL);

printf("Testing %08x to %08x %s\n",adr,adr+siz,(cflag)?"continuous":"");
printf("Memory test running..  ");

if (cflag) while(! (err=domt(adr,siz))) ;
else err = domt(adr,siz);

if (err) printf("\b\nThere were %d errors.\n",err);
else printf("\b\nTest passed with no errors.\n");
}

/*************************************************************
*  domt(adr,siz)
*/
domt(adr,siz)
unsigned int *adr;
int siz;
{
int i,j,err,temp;
unsigned int w,*p,r;

err = 0;

/* walking ones test */
for (p=adr,i=0;i<siz;i+=4,p++) {
	w = 1;
	for (j=0;j<32;j++) {
		*p = w;
		temp = 0;
		r = *p;
		if (r != w) {
			err++;
			printf("\b\nerror: addr=%08x read=%08x expected=%08x  ",
				p,r,w);
			}
		w <<= 1;
		dotik(1,0);
		}
	}

/* store the address in each address */
for (p=adr,i=0;i<siz;i+=4,p++) {
	*p = (unsigned int)p;
	dotik(1,0);
	}

/* check that each address contains its address */
for (p=adr,i=0;i<siz;i+=4,p++) {
	r = *p;
	if (r != (unsigned int)p) {
		err++;
		printf("\b\nerror: adr=%08x read=%08x expected=%08x  ",p,r,p);
		}
	dotik(1,0);
	}
return(err);
}

