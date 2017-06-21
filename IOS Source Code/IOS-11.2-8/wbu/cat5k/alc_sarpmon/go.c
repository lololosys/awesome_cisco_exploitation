/* $Id: go.c,v 1.1.4.1 1996/05/09 15:00:21 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/go.c,v $
 *------------------------------------------------------------------
 * go.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: go.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:21  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:28  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:03  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:59  rlowe
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
#include "set.h"

int trace_mode;
int trace_count;
int trace_verbose;
int trace_invalid;
int trace_over;
int trace_bflag;
int trace_cflag;

char clientcmd[LINESZ];
char *clientav[MAX_AC];

extern int clkdat;

#define STOPMAX 10
Stopentry stopval[STOPMAX];

#define PCHISTSZ  200
unsigned long pchist_d[PCHISTSZ+1];
int pchist_ip,pchist_op;

Optdesc g_opts[] = {
	{"-s","don't set client sp"},
	{"-t","time execution"},
	{"<adr>","start address"},
	{"<bptadr>","temporary breakpoint"},
	{"-c <args>","args to be passed to client"},
	{0}};

/*************************************************************
*  go(ac,av), the 'g' command
*/
go(ac,av)
int ac;
char *av[];
{
word	adr;
int i,j,had_epc,sflag;

had_epc = 0;
sflag = 0;
BptTmp.addr = NO_BPT;
BptTrc.addr = NO_BPT;
BptTrcb.addr = NO_BPT;

strcpy(clientcmd,av[0]);
strcat(clientcmd," ");

for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j] != 0;j++) {
			if (av[i][j] == 's') sflag = 1;
			else if (av[i][j] == 't') clientcmd[0] = 't';
			else if (av[i][j] == 'c') {
				for (i++;i<ac;i++) {
					strcat(clientcmd,av[i]);
					strcat(clientcmd," ");
					}
				break;
				}
			else printf("%c: unknown option\n",av[i][j]);
			}
		}
	else {
		if (had_epc) {
			if(!get_rsa(&adr,av[i])) return(1);
			BptTmp.addr = adr;
			}
		else {
			had_epc++;
			if(!get_rsa(&adr,av[i])) return(1);
			Epc = adr;
			}
		}
	}

/* set client's a0 (argc) & a1 (argv) */
Gpr[4] = argvize(clientav,clientcmd);
Gpr[5] = (word)clientav;

if (!sflag) Gpr[29] = clienttos();
clrhndlrs();
closelst(2);
Status = initial_sr;
Fcr &= ~CSR_EMASK; /* clear any outstanding exceptions */
goclient();
}

/*************************************************************
*  cont(ac,av) the continue command
*/
cont(ac,av)
int ac;
char *av[];
{
word	adr;

BptTmp.addr = NO_BPT;
BptTrc.addr = NO_BPT;
BptTrcb.addr = NO_BPT;
if(ac > 1){
	if(!get_rsa(&adr,av[1])) return(1);
	BptTmp.addr = adr;
	}
goclient();
}

Optdesc t_opts[] = {
	{"-v","verbose, list each step"},
	{"-b","capture only branches"},
	{"-c","capture only calls (jal)"},
	{"-i","stop on pc invalid"},
	{"-m adr val","stop on mem equal"},
	{"-M adr val","stop on mem not equal"},
	{"-r reg val","stop on reg equal"},
	{"-R reg val","stop on reg not equal"},
	{0}};

/*************************************************************
*  trace(ac,av) the 't' (single-step) command
*/
trace(ac,av)
int ac;
char *av[];
{
int stepover,multi,i,j,n;
word target;
unsigned long adr,val;

trace_over = 0;
if (strequ(av[0],"to")) trace_over = 1;

n = multi = trace_verbose = trace_invalid = 0;
trace_bflag = trace_cflag = 0;
for (i=0;i<STOPMAX;i++) stopval[i].addr = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j] != 0;j++) {
			if (av[i][j] == 'v') {
				trace_verbose = 1;
				trace_count = 0;
				multi = 1;
				}
			else if (av[i][j] == 'm'|| av[i][j] == 'M') {
				if (i+2 >= ac) {
					printf("bad arg count\n");
					return(1);
					}
				if (!get_rsa(&adr,av[i+1])) return(1);
				if (!get_rsa(&val,av[i+2])) return(1);
				if (!addstop(adr,val,"MEMORY",av[i][j])) 
					return(1);
				trace_count = 0;
				multi = 1;
				i += 2;
				break;
				}
			else if (av[i][j] == 'r'||av[i][j] == 'R') {
				if (i+2 >= ac) {
					printf("bad arg count\n");
					return(1);
					}
				if (!getregadr(&adr,av[i+1])) {
					printf("%s: bad reg name\n",av[i+1]);
					return(1);
					}
				if (!get_rsa(&val,av[i+2])) return(1);
				if (!addstop(adr,val,av[i+1],av[i][j])) 
					return(1);
				trace_count = 0;
				multi = 1;
				i += 2;
				break;
				}
			else if (av[i][j] == 'b') trace_bflag = 1;
			else if (av[i][j] == 'c') trace_cflag = 1;
			else if (av[i][j] == 'i') {
				trace_invalid = 1;
				trace_count = 0;
				multi = 1;
				}
			else {
				printf("%c: unrecognized option\n",av[i][j]);
				return(1);
				}
			}
		}
	else {
		if (n == 0) {
			if (!get_rsa(&trace_count,av[i])) return(1);
			multi = 1;
			}
		else {
			printf("%s: unrecognized argument\n",av[i]);
			return(1);
			}
		n++;
		}
	}

if (setTrcbp(Epc,trace_over)) return(1);
clrpchist();
if (multi) trace_mode = TRACE_TN;
else trace_mode = TRACE_TB;
store_trace_breakpoint();
_go();
}

/*************************************************************
*  addstop(adr,val,name,sense)
*/
addstop(adr,val,name,sense)
unsigned long adr,val;
char *name,sense;
{
int i;

for (i=0;i<STOPMAX;i++) {
	if (stopval[i].addr == 0) break;
	}
if (i >= STOPMAX) {
	printf("stopval table full\n");
	return(0);
	}
stopval[i].addr = adr;
stopval[i].value = val;
strcpy(stopval[i].name,name);
if (sense == 'M' || sense == 'R') stopval[i].sense = 1;
else stopval[i].sense = 0;
return(1);
}
		
Optdesc b_opts[] = {
	{"-i","hw bpt for instr"},
	{"-d","hw bpt for data"},
	{"-k","hw bpt for kernel only"},
	{"-u","hw bpt for user only"},
	{"-r","hw bpt for data read only"},
	{"-w","hw bpt for data write only"},
	{"-m mask","mask for hw bpt",F_333},
	{"-s","command string"},
	{0}};

/*************************************************************
*  setbp(ac,av) the 'b' (set breakpoint) command
*/
setbp(ac,av)
int ac;
char *av[];
{
word adr,msk,i,j,w,x;
char *str;
int flag = 0;

if(ac == 1) {
	dspbpts();
	return(0);
	}

w = 0; str = 0;
for(i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		x = 0;
		for (j=1;av[i][j] != 0;j++) {
			if (av[i][j] == 's') {
				i++;
				if (i >= ac) {
					printf("bad arg count\n");
					return(1);
					}
				str = av[i];
				break;
				}
#ifdef LR33000
			else if (av[i][j] == 'i') {
				w = (DCIC_PCE|DCIC_TR|DCIC_UD|DCIC_KD|DCIC_DE); 
#ifdef LR33300
				Bpcm = ~0;
#endif
				}
			else if (av[i][j] == 'd') {
				w = (DCIC_DAE|DCIC_DW|DCIC_DR|DCIC_TR|DCIC_UD|
					DCIC_KD|DCIC_DE); 
#ifdef LR33300
				Bdam = ~0;
#endif
				}
			else if (av[i][j] == 'k') x |= DCIC_UD; 
			else if (av[i][j] == 'u') x |= DCIC_KD; 
			else if (av[i][j] == 'r') x |= DCIC_DW; 
			else if (av[i][j] == 'w') x |= DCIC_DR; 
#endif
#ifdef LR33300
			else if (av[i][j] == 'm' && 
				(machtype == 33300 || machtype == 33310)) {
				i++;
				if (!get_rsa(&msk,av[i])) return(1);
				if (w&DCIC_PCE) Bpcm = msk;
				else if (w&DCIC_DAE) Bdam = msk;
				else {
					printf("error: must specify i or d\n");
					return(1);
					}
				break;
				}
#endif
			else {
				printf("%c: unrecognized option\n",av[i][j]);
				return(1);
				}
			}
		}
	else {
		flag = 1;
		if(!get_rsa(&adr,av[i])) return(1);

		/* remove any duplicates */
		for(j=0;j < MAX_BPT;j++) {
			if (Bpt[j].addr == adr) {
				Bpt[j].addr = NO_BPT;
				if (Bpt[j].cmdstr) free(Bpt[j].cmdstr);
				}
			}

#ifdef LR33000
		if (w&DCIC_PCE)  {
			Dcic |= w; Dcic &= ~x; Bpc = adr; w = 0;
			printf("Bpt %2d = %08x\n",MAX_BPT,adr);
			}
		else if (w&DCIC_DAE) {
			Dcic |= w; Dcic &= ~x; Bda = adr; w = 0;
			printf("Bpt %2d = %08x\n",MAX_BPT+1,adr);
			}
		else {
#else
		if (1) {
#endif
			for(j=0;j < MAX_BPT && Bpt[j].addr != NO_BPT;j++) ;
			if(MAX_BPT <= j) {
				printf("too many breakpoints\n");
				return(1);
				}
			Bpt[j].addr = adr;
			printf("Bpt %2d = %08x",j,adr);
			if(adr & 3L) printf(" -> ??");
			if (str != 0) {
				Bpt[j].cmdstr = malloc(strlen(str)+1);
				strcpy(Bpt[j].cmdstr,str);
				str = 0;
				printf(" \"%s\"",Bpt[j].cmdstr);
				}
			else Bpt[j].cmdstr = 0;
			putchar('\n');
			}
		}
	}
if (!flag) printf("break address not specified\n");
return(0);
}

/*************************************************************
*  dspbpts() display all breakpoints
*/
dspbpts()
{
int i,ln,siz;
char tmp[64],buf[100];

if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}

ioctl(STDIN,CBREAK,NULL);
ln = siz;
for(i = 0; i < MAX_BPT; i++)
	if(Bpt[i].addr != NO_BPT){
		sprintf(buf,"Bpt %2d = %08x ",i,Bpt[i].addr);
		if (adr2symoff(tmp,Bpt[i].addr,0)) strcat(buf,tmp);
		if(Bpt[i].addr & 3L) strcat(buf," -> ??");
		if (Bpt[i].cmdstr) {
			sprintf(tmp," \"%s\"",Bpt[i].cmdstr);
			strcat(buf,tmp);
			}
		if (more(buf,&ln,siz)) break;
		}

#ifdef LR33000
if (Dcic&DCIC_DE) {
	if (Dcic&DCIC_PCE) {
		printf("Bpt %2d = %08x",MAX_BPT,Bpc);
		if (adr2symoff(tmp,Bpc,0)) printf(" %s",tmp);
		printf(" [instr");
		if (Dcic&DCIC_KD) printf(" kernel");
		if (Dcic&DCIC_UD) printf(" user");
#ifdef LR33300
		printf("] mask=%08x\n",Bpcm);
#else
		printf("]\n");
#endif
		}
	if (Dcic&DCIC_DAE) {
		printf("Bpt %2d = %08x",MAX_BPT+1,Bda);
		if (adr2symoff(tmp,Bda,0)) printf(" %s",tmp);
		printf(" [data");
		if (Dcic&DCIC_KD) printf(" kernel");
		if (Dcic&DCIC_UD) printf(" user");
		if (Dcic&DCIC_DR) printf(" read");
		if (Dcic&DCIC_DW) printf(" write");
#ifdef LR33300
		printf("] mask=%08x\n",Bdam);
#else
		printf("]\n");
#endif
		}
	}
#endif
}

/*************************************************************
*  clrbp(ac,av)
*	The 'db' command
*/
clrbp(ac,av)
int ac;
char *av[];
{
word adr, i, j;

if(ac > 1){
	for(i = j = 0; j < ac - 1; j++){
		if (strequ(av[1+j],"*")) {
			clrbpts();
			continue;
			}
		if (!atob(&i,av[1+j],10)) {
			printf("%s: decimal number expected\n",av[1+j]);
			continue;
			}
		if (i < MAX_BPT) {
			if (Bpt[i].addr == NO_BPT) printf("Bpt %2d not set\n",i);
			else {
				Bpt[i].addr = NO_BPT;
				if (Bpt[i].cmdstr) free(Bpt[i].cmdstr);
				}
			}
#ifdef LR33000
		else if (i == MAX_BPT) {
			if ((Dcic&DCIC_PCE) && (Dcic&DCIC_DE)) {
				Dcic &= ~DCIC_PCE;
				if (!(Dcic&DCIC_DAE)) Dcic &= ~DCIC_DE;
				}
			else printf("Bpt %2d not set\n",i);
			}
		else if (i == MAX_BPT+1) {
			if ((Dcic&DCIC_DAE) && (Dcic&DCIC_DE)) {
				Dcic &= ~DCIC_DAE;
				if (!(Dcic&DCIC_PCE)) Dcic &= ~DCIC_DE;
				}
			else printf("Bpt %2d not set\n",i);
			}
#endif
		else printf("%d: breakpoint number too large\n",i);
		}
	}
else dspbpts();
return(0);
}

/*************************************************************
*  rm_bpts()
*/
rm_bpts()
{
int i;

if(BptTmp.addr != NO_BPT && load_word(BptTmp.addr) == BPT_CODE) 
	store_word(BptTmp.addr, BptTmp.value);

for(i = 0; i < MAX_BPT; i++)
	if(Bpt[i].addr != NO_BPT && load_word(Bpt[i].addr) == BPT_CODE)
		store_word(Bpt[i].addr, Bpt[i].value);

if(BptTrc.addr != NO_BPT && load_word(BptTrc.addr) == BPT_CODE) 
	store_word(BptTrc.addr, BptTrc.value);

if(BptTrcb.addr != NO_BPT && load_word(BptTrcb.addr) == BPT_CODE)
	store_word(BptTrcb.addr, BptTrcb.value);
}

/*************************************************************
*  clrbpts()
*/
clrbpts()
{
int i;

for(i = 0; i < MAX_BPT; i++) {
	Bpt[i].addr = NO_BPT;
	if (Bpt[i].cmdstr) free(Bpt[i].cmdstr);
	}
#ifdef LR33000
Dcic &= ~DCIC_DE;
#endif
}

/*************************************************************
*  goclient()
*/
goclient()
{
word	adr,target;

if(is_break_point(Epc)){
	if (setTrcbp(Epc,0)) return(1);
	trace_mode = TRACE_TG;
	store_trace_breakpoint();
	}
else{
	trace_mode = TRACE_GB;
	store_breakpoint();
	}
_go();
}

/*************************************************************
* sstep()
*/
sstep()
{

if (setTrcbp(Epc,0)) return(1);
trace_mode = TRACE_DS;
store_trace_breakpoint();
_go();
}

/*************************************************************
*  pmexception(epc,cause)
*	An exception has been generated within PMON
*/
pmexception(epc,cause)
unsigned long epc,cause;
{
printf("\nException Epc=%08x Cause=%08x (%s)\n", epc,cause,
	getexcname(cause&CAUSE_EXCMASK));
main();
}

/*************************************************************
*  exception()
*	An exception has been generated within the client
*	control is passed here from _exception in mips.s
*/
exception()
{
int i,j,flag;
char tmp[80],*p;

if ((Cause&CAUSE_EXCMASK) != EXC_BP) {
	printf("\nException Cause=%s\n",getexcname(Cause&CAUSE_EXCMASK));
	stop(0);
	}

if(trace_mode == TRACE_NO){ /* no bpts set */
	printf("\nBreak\n");
	stop(0);
	}
else if(trace_mode == TRACE_GB){ /* go & break */
	if(BptTmp.addr != NO_BPT){
		store_word(BptTmp.addr, BptTmp.value);
		BptTmp.addr = NO_BPT;
		}
	for(i = 0; i < MAX_BPT; i++) 
		if(Bpt[i].addr != NO_BPT) store_word(Bpt[i].addr, Bpt[i].value);
	for(i=0;i<MAX_BPT;i++) {
		if (Bpt[i].addr == Epc) {
			printf("Stopped at Bpt %d\n",i);
			stop(Bpt[i].cmdstr);
			}
		}
	stop(0);
	}

if(BptTrc.addr != NO_BPT){
	store_word(BptTrc.addr, BptTrc.value);
	BptTrc.addr = NO_BPT;
	}
if(BptTrcb.addr != NO_BPT){
	store_word(BptTrcb.addr, BptTrcb.value);
	BptTrcb.addr = NO_BPT;
	}

if(trace_mode == TRACE_TB) stop(0);  /* trace & break */
else if(trace_mode == TRACE_DC) dbgmode(1);
else if(trace_mode == TRACE_DS) dbgmode(2);
else if(trace_mode == TRACE_TN) {
	for(i=0;i<MAX_BPT;i++) {
		if (Bpt[i].addr == Epc) {
			printf("Stopped at Bpt %d\n",i);
			stop(Bpt[i].cmdstr);
			}
		}
	if (trace_invalid && !is_validpc(Epc)) {
		printf("Stopped: Invalid PC value\n");
		stop(0);
		}
	for (i=0;i<STOPMAX;i++) {
		if (stopval[i].addr == 0) continue;
		if ((stopval[i].sense == 0 && 
			load_word(stopval[i].addr) == stopval[i].value) 
			|| (stopval[i].sense == 1 &&
			load_word(stopval[i].addr) != stopval[i].value)) {
			if (stopval[i].sense == 0) p = " == ";
			else p = " != ";
			if (strequ(stopval[i].name,"MEMORY"))
				printf("Stopped: 0x%08x%s0x%08x\n",
					stopval[i].addr,p,stopval[i].value);
			else
				printf("Stopped: %s%s0x%08x\n",stopval[i].name,
					p,stopval[i].value);
			stop(0);
			}
		}
	flag=1;
	if (trace_bflag || trace_cflag) {
		if (is_branch(Epc) && trace_bflag) flag=1;
		else if (is_jal(Epc) && trace_cflag) flag=1;
		else flag=0;
		}
	if (flag) {
		addpchist(Epc);
		if (trace_verbose) {
			disasm(tmp,Epc,load_word(Epc));
			printf("%s\n",tmp);
			}
		else dotik(256,1);
		}
	else dotik(256,1);
	if (trace_count) trace_count--;
	if (trace_count == 1) trace_mode = TRACE_TB;
	if (setTrcbp(Epc,trace_over)) stop(0);
	store_trace_breakpoint();
	_go();
	}
/* else TRACE_TG  trace & go, set on g or c if starting at bpt */

trace_mode = TRACE_GB; /* go & break */
store_breakpoint();
_go();
}

/*************************************************************
*  stop(cmdstr)
*/
stop(cmdstr)
char *cmdstr;
{
char cmd[LINESZ];

swlst(1);
trace_mode = TRACE_NO;
if (cmdstr) strcpy(cmd,cmdstr);
else strcpy(cmd,getenv("brkcmd"));
do_cmd(cmd);
main();
}

/*************************************************************
*  store_breakpoint()
*/
store_breakpoint()
{
int	i;

for (i = 0; i < MAX_BPT; i++) {
	if(BptTmp.addr == Bpt[i].addr) BptTmp.addr = NO_BPT;
	if(BptTrc.addr == Bpt[i].addr) BptTrc.addr = NO_BPT;
	if(BptTrcb.addr == Bpt[i].addr) BptTrcb.addr = NO_BPT;
	}
if (BptTrc.addr == BptTmp.addr) BptTrc.addr = NO_BPT;
if (BptTrcb.addr == BptTmp.addr || BptTrcb.addr == BptTrc.addr) 
	BptTrcb.addr = NO_BPT;

for (i = 0; i < MAX_BPT; i++)
	if (Bpt[i].addr != NO_BPT) {
		Bpt[i].value = load_word(Bpt[i].addr);
		store_word(Bpt[i].addr, BPT_CODE);
		}
if (BptTmp.addr != NO_BPT) {
	BptTmp.value = load_word(BptTmp.addr);
	store_word(BptTmp.addr, BPT_CODE);
	}
store_trace_breakpoint();
}

/*************************************************************
*  store_trace_breakpoint()
*/
store_trace_breakpoint()
{
if (BptTrc.addr != NO_BPT) {
	BptTrc.value = load_word(BptTrc.addr);
	store_word(BptTrc.addr, BPT_CODE);
	}
if (BptTrcb.addr != NO_BPT) {
	BptTrcb.value = load_word(BptTrcb.addr);
	store_word(BptTrcb.addr, BPT_CODE);
	}
}

/*************************************************************
*  int is_break_point(adr)
*/
int is_break_point(adr)
word adr;
{
int	i;

for (i=0;i < MAX_BPT;i++) if (Bpt[i].addr == adr) return(1);
if (BptTmp.addr == adr) return(1);
return(0);
}

/*************************************************************
*  is_validpc(adr)
*/
is_validpc(adr)
unsigned long adr;
{
int i,ac;
char *av[10],tmp[80];
unsigned long a1,a2;

strcpy(tmp,getenv("validpc"));
ac = argvize(av,tmp);
if (ac%2 != 0) {
	printf("validpc variable must have even number of values\n");
	return(0);
	}
for (i=0;i<ac;i++) {
	if (!get_rsa(&a1,av[i])) return(0);
	i++;
	if (!get_rsa(&a2,av[i])) return(0);
	if (adr >= a1 && adr <= a2) return(1);
	}
return(0);
}

/*************************************************************
*  addpchist(adr)
*/
addpchist(adr)
unsigned long adr;
{
pchist_d[pchist_ip] = adr;
pchist_ip = incmod(pchist_ip,PCHISTSZ);
if (pchist_ip == pchist_op) pchist_op = incmod(pchist_op,PCHISTSZ);
}

/*************************************************************
*  clrpchist()
*/
clrpchist()
{
pchist_ip = pchist_op = 0;
}

/*************************************************************
*  unsigned long getpchist(n)
*/
unsigned long getpchist(n)
int n;
{
int i;

i = pchist_ip-n-1;
if (i < 0) i += PCHISTSZ+1;
if (incmod(i,PCHISTSZ) == pchist_op) return(0);
return(pchist_d[i]);
}

/*************************************************************
*  setTrcbp(adr,stepover)
*/
setTrcbp(adr,stepover)
unsigned long adr;
int stepover;
{
unsigned long target;

BptTrc.addr = NO_BPT;
BptTrcb.addr = NO_BPT;
if(is_branch(adr)){
	if(is_branch(adr + 4)){
		printf("branch in delay slot\n");
		return(1);
		}
	target = branch_target_address(adr);
	if (target == adr) target = adr+8; /* skip self branches */
	if (is_conditional_branch(adr) && target != adr+8) BptTrc.addr = adr+8;
	if (is_jal(adr) && stepover) BptTrc.addr = adr+8;
	else if (is_jr(adr) && !is_writeable(target)) BptTrc.addr = Gpr[31];
	else BptTrcb.addr = target;
	}
else BptTrc.addr = adr + 4;
return(0);
}

