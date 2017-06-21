/* $Id: main.c,v 1.1.4.1 1996/05/09 15:00:25 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/main.c,v $
 *------------------------------------------------------------------
 * main.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:25  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:37  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:07  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:02  rlowe
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
#include "sar_hw.h"
#include "target.h"

#ifdef MIPSEB
char endian[] = "EB";
#else
char endian[] = "EL";
#endif

Bps Bpt[MAX_AC];	/* user break points			*/
Bps BptTmp;		/* tmp bpt used for continue cmd	*/
Bps BptTrc;		/* bpt for tracing	 		*/
Bps BptTrcb;		/* bpt for tracing through branches 	*/

word DBGREG[NREGS];	/* debugger's register value holder */

int machtype;
jmp_buf jmpb;		/* non-local goto jump buffer */
char line[LINESZ+1];	/* input line */
int pmlst,clilst;	/* list of files opened by: PMON & client */
extern int *curlst;	/* list of files opened by: current context */
unsigned long initial_sr;
int memorysize;
char prnbuf[LINESZ+8];	/* commonly used print buffer */
unsigned long _filebase;
int cp1ok;		/* indicates that it's ok to use cp1 */
int vflag;		/* verbose flag */

#ifdef FPEM
#ifndef NEWFP
extern struct c1state *c1dat;
#endif
#endif

Optdesc sh_opts[] = {
	{"^P","recall previous cmd"},
	{"^N","recall next cmd"},
	{"^F","move cursor right"},
	{"^B","move cursor left"},
	{"^A","move cursor far left"},
	{"^E","move cursor far right"},
	{"^D","delete char at cursor"},
	{"^H","delete char before cursor"},
	{"",""},
	{"!!","repeat last cmd"},
	{"!str","recall and execute cmd str"},
	{"!num","recall and execute cmd num"},
	{"",""},
	{"+-*/()","operators"},
	{"^addr","contents of address"},
	{"@name","contents of register"},
	{"&name","value of symbol"},
	{"0xnum","hex number"},
	{"0onum","octal number"},
	{"0tnum","decimal number"},
	{0}};

/*************************************************************
*  main()
*/
main()
{
int i,t1;
char prompt[20],tmp[8],*p;

if(setjmp(jmpb)) printf(" break!\n");
else ioctl(STDIN,SETINTR,jmpb);
clr_iec();
rm_bpts();

while (1) {
	closelst(0);
	swlst(1);
	ioctl(STDIN,SETSANE,NULL);
	strcpy(prompt,getenv("prompt"));
	if (p = strchr(prompt,'!')) {
		strdchr(p); /* delete the bang */
		sprintf(tmp,"%d",histno);
		stristr(p,tmp);
		}
	printf("%s",prompt);
	get_cmd(line);
	do_cmd(line);
	}
}

/*************************************************************
*  dbginit(adr)
*/
dbginit(adr)
char *adr;
{
int i,c,n;
char ch,buf[80];


#ifdef NON_CACHED
initial_sr |= SR_BEV;
#endif
Status = initial_sr;

i=devinit();

histinit();
envinit();


printf("\n");
printf("PMON version %s [%s",vers,endian);

#ifdef FPEM
cp1ok = 1;
printf(",FP");
#endif

printf("], LSI PMON %s\n",date);

#ifdef  ALC_REV1 
#define MEMSIZE  3 * 1024
#else  /* !ALC_REV1 */
#define MEMSIZE  (5 * 64 * 1024)
#endif /* ALCR_REV1 */

#ifdef MEMSIZE  /* dynamic memory sizing may not work on some boards */
memorysize = MEMSIZE;
#else
memorysize = sizemem(CLIENTPC);
#endif
printf(" Avail RAM %d KBytes.",memorysize/1024);

printf("\n\n");

for(i = 0; i < MAX_BPT; i++) Bpt[i].addr = NO_BPT;
BptTmp.addr = NO_BPT;
BptTrc.addr = NO_BPT;
BptTrcb.addr = NO_BPT;
trace_mode = TRACE_NO;

#ifdef FPEM
#ifndef NEWFP
	c1dat = (struct c1state *)malloc(_fpstatesz());
	_fpinit(c1dat);
#endif
#endif
}  /* dbginit() */

/*************************************************************
*  do_cmd(p)
*	execute a command, the string pointed to by p.
*	warning: p does get written to
*/
do_cmd(p)
char *p;
{
char *av[MAX_AC];	/* argument holder */
word ac;		/* # of arguments */
char *cmdlist[20],*t,tmp[11];
int i,nc,j,c,rptcmd;

if (!*p || strempty(p)) { /* blank */
	rptcmd = matchenv("rptcmd");
	if (rptcmd) { /* repeat requested */
		t = gethistn(histno-1);
		if (rptcmd == 1) strcpy(p,t); /* all cmds */
		else if (rptcmd == 2) { /* trace only */
			if (wordsz(t) > 10) return;
			getword(tmp,t);
			if (strequ(tmp,"t") || strequ(tmp,"to")) strcpy(p,tmp);
			else return;
			}
		else {
			printf("bad rptcmd value [%s]\n",getenv("rptcmd"));
			return;
			}
		}
	else return;
	}

nc = 0;
cmdlist[nc++] = p;
for (;*p;) {
	c = *p;
	if(c == '\'' || c == '"') {
		p++;
		while(*p && *p != c) ++p;
		if(*p) p++;
		}
	else if (c == ';') {
		*p++ = 0;
		cmdlist[nc++] = p;
		}
	else p++;
	}

for (j=0;j<nc;j++) {
	ac = argvize(av,cmdlist[j]);
	if(ac > 0){
		for (i=0;CmdTable[i].name != 0;i++) 
			if (strequ(CmdTable[i].name,av[0])) break;
		if (CmdTable[i].name != 0) {
			if (ac >= CmdTable[i].minac && ac <= CmdTable[i].maxac)
				(CmdTable[i].func)(ac,av);
			else
				printf("usage: %s %s\n",CmdTable[i].name,
					CmdTable[i].opts);
			}
		else printf("%s: Command not found.\n",av[0]);
		}
	}
}

/*************************************************************
*  help(ac,av)
*	the 'help' command
*/
help(ac,av)
int ac;
char *av[];
{
int i,j,namemax,optsmax,descmax,len;
int ln,siz;

namemax = optsmax = descmax = 0;
for (i=0;CmdTable[i].name != 0;i++) {
	len = strlen(CmdTable[i].name);
	if (len > namemax) namemax = len;
	len = strlen(CmdTable[i].opts);
	if (len > optsmax) optsmax = len;
	len = strlen(CmdTable[i].desc);
	if (len > descmax) descmax = len;
	}

if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}
ioctl(STDIN,CBREAK,NULL);

ln = siz;
if (ac >= 2) { /* extended help */
	if (strequ(av[1],"*")) { /* all commands */
		for (i=0;CmdTable[i].name != 0;i++) {
			if (prhelp(i,&ln,siz,namemax,optsmax)) break;
			}
		}
	else { /* specific commands */
		for (j=1;j<ac;j++) {
			for (i=0;CmdTable[i].name != 0;i++) {
				if (strequ(CmdTable[i].name,av[j])) break;
				}
			if (CmdTable[i].name) {
				if (prhelp(i,&ln,siz,namemax,optsmax)) break;
				}
			else printf("%s: command not found\n",av[j]);
			}
		}
	}
else { /* general help only */
	for (i=0;CmdTable[i].name != 0;i++) {
		printf("%*s  %-*s",namemax,CmdTable[i].name,descmax,
				CmdTable[i].desc);
		if (i%2 != 0) printf("\n");
		else printf("   ");
		}
	if (i%2 != 0) printf("\n");
	}
return(0);
}

/*************************************************************
*  prhelp(n,lnp,siz,namemax,optsmax)
*/
prhelp(n,lnp,siz,namemax,optsmax)
int n,*lnp,siz,namemax,optsmax;
{
Optdesc *p;
int i;

sprintf(prnbuf,"%*s  %-*s    %s",namemax,CmdTable[n].name,optsmax,
					CmdTable[n].opts,CmdTable[n].desc);
if (more(prnbuf,lnp,siz)) return(1);

p = CmdTable[n].optdesc;
if (p != 0) {
	for (i=0;p[i].name;i++) {
		if (!in_machtype(p[i].flags)) continue;
		sprintf(prnbuf,"%*s  %15s    %s",namemax,"",
					p[i].name,p[i].desc);
		if (more(prnbuf,lnp,siz)) return(1);
		}
	}
return(0);
}


/*************************************************************
*  word no_cmd(ac,av)
*/
word no_cmd(ac,av)
int ac;
char *av[];
{

printf("The %s command cannot be invoked by name.\n",av[0]);
return(0);
}

/*************************************************************
*  closelst(lst)
*/
closelst(lst)
int lst;
{
int i,x;

switch (lst) {
	case 0 : 
		if (curlst == 0) return;
		x = *curlst; *curlst = 0; 
		break;
	case 1 : x = pmlst; pmlst = 0; break;
	case 2 : x = clilst; clilst = 0; break;
	}

for (i=0;i<OPEN_MAX;i++) {
	if (x&1 && i > 2) close(i);
	x >>= 1;
	}
}

/*************************************************************
*  swlst(lst)
*/
swlst(lst)
int lst;
{

switch (lst) {
	case 1 : 
		curlst = &pmlst; 
		break;
	case 2 : 
		curlst = &clilst; 
		break;
	}
}


#define BIU_IRAM_WR     0x00034800 /*
                        DS=0 => Disable D-Cache
                        IS1=1 => Enable I-Cache Set 1
                        IBLKSZ=0 => i-cache block fill size 2 words
                        INTP=0, NOPAD=0, BGNT=1 =>
                        INT[5:0]activeLow,WaitState,EnableBusGrant
                        LDSCH=1 => Enable load scheduling
                        NOSTR=1 => Enable instruction streaming.
                        */

/*************************************************************
*  ulong read_target(adr,sz)
*	read from the target address space
*/
ulong read_target(adr,sz)
ulong adr;
int sz;
{
ulong v,save,cur;

#ifdef ATMIZER
if (adr < (ulong)M_DMACR) {
	switch (sz) {
		case 1 :
			cur = load_word(adr&~3);
			return (ubyte)((cur >> (24 - (adr&3)*8 ) ) &0xff);
		case 2 :
			cur = load_word(adr&~3);
			return (uhalf)((cur >> (16 - (adr&3)*8 ) ) &0xffff);
		case 4 :
			save = BIU;
			BIU = BIU_IRAM_WR;
			v = read_Disolated(adr);
			BIU = save;
			return(v);
		}
	}
#endif
switch (sz) {
	case 1 : return (*(ubyte *)(adr));
	case 2 : return (*(uhalf *)(adr));
	case 4 : return (*(uword *)(adr));
	}
}

/*************************************************************
*  write_target(adr,v,sz)
*	write to the target address space
*/
write_target(adr,v,sz)
ulong adr;
ulong v;
int sz;
{
ulong save,cur;

#ifdef ATMIZER
if (adr < (ulong)M_DMACR) {
	switch (sz) {
		case 1 : 
			cur = load_word(adr&~3);
			cur &= ~((uword)0xff << (24 - ((adr&3)*8)) );
			cur |= (v << (24 - ((adr&3)*8)) );
			store_word(adr&~3,cur);
			return;
		case 2 : 
			cur = load_word(adr&~3);
			cur &= ~((uword)0xffff << (16 - ((adr&3)*8)) );
			cur |= (v << (16 - ((adr&3)*8)) );
			store_word(adr&~3,cur);
			return;
		case 4 : 
			save = BIU;
			BIU = BIU_IRAM_WR;	
			write_Disolated(adr,v);
			BIU = save;
			return;
		}
	}
#endif
switch (sz) {
	case 1 : *(ubyte *)adr = v; break;
	case 2 : *(half *)adr = v; break;
	case 4 : *(word *)adr = v; break;
	}
}
