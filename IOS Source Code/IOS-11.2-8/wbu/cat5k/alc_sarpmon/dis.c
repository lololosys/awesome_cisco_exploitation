/* $Id: dis.c,v 1.1.4.1 1996/05/09 15:00:20 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/dis.c,v $
 *------------------------------------------------------------------
 * dis.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dis.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:20  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:01  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:57  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <mips.h>
#include <stdio.h>
#include <termio.h>
#include <string.h>
#include "pmon.h"

#ifdef SABLE
#define TIMEOUT 10
#else
#define TIMEOUT 1000
#endif

/* used by -b option, list only branches */
char *brlst[] = {
	"j","jr","jal","jalr","break","syscall","rfe","beq","bne",
	"bltzal","bltz","blez","bgtz","bgezal","bgez","bc1t","bc1f",
	"bc0t","bc0f","bc2t","bc2f","bc3t","bc3f",
	0};
	
/* used by -c option, list only calls */
char *cllst[] = {"jal","jalr",0};

Optdesc l_opts[] = {
	{"-b","list only branches"},
	{"-c","list only calls"},
	{"-t","list trace buffer"},
	{0}};

/*************************************************************
*  dis(ac,av)
*	the 'l' (disassemble) command
*/
dis(ac,av)
int ac;
char *av[];
{
word adr, siz, l;
char v;
char instr[20],*p,**q;
int bflag,cflag,i,j,n,tflag;

bflag = cflag = n = tflag = 0;

if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}

adr = Epc;

for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j] != 0;j++) {
			switch (av[i][j]) {
				case 'b' : bflag=1; break;
				case 'c' : cflag=1; break;
				case 't' : tflag=1; n++; break;
				default  : 
					printf("%c: unknown option\n",av[i][j]);
				}
			}
		}
	else {
		switch (n) {
			case 0 :
				if(!get_rsa(&adr,av[i])) return(1);
				break;
			case 1 :
				if(!get_rsa(&siz,av[i])) return(1);
				break;
			default :
				printf("%s: unknown option\n",av[i]);
				return(1);
			}
		n++;
		}
	}

ioctl(STDIN,CBREAK,NULL);

if (tflag) {
	dispchist(n,siz);
	return(0);
	}

l = siz;

if (cflag || bflag) printf("%s",searching);
while(1) {
	/* Enable this if you want a 'label' at the start of each
	procedure.
	if (adr2sym(prnbuf,adr)) {
		strcat(prnbuf,":");
		if (more(prnbuf,&l,(n>1)?0:siz)) break;
		}
	*/
	adr = disasm(prnbuf,adr, load_word(adr));
	if (bflag || cflag) {
		for (p=prnbuf,i=0;p != 0 && i < 3;i++) p = getword(instr,p);
		if (cflag) q = cllst;
		else q = brlst;
		for (i=0;q[i] != 0;i++) {
			if (strequ(instr,q[i])) break;
			}
		if (q[i] == 0) {
			dotik(128,0);
			continue;
			}
		else era_line(searching);
		}
	if (more(prnbuf,&l,(n>1)?0:siz)) break;
	if (cflag || bflag) printf("%s",searching);
	}
}

/*************************************************************
*  dispchist(args,siz)
*	display the pc history (trace buffer)
*/
dispchist(args,siz)
int args,siz;
{
int i,l;
unsigned long adr;

l = siz;
for (i=0;;i++) {
	adr = getpchist(i);
	if (adr == 0) break;
	disasm(prnbuf,adr, load_word(adr));
	if (more(prnbuf,&l,(args>1)?0:siz)) break;
	}
}
