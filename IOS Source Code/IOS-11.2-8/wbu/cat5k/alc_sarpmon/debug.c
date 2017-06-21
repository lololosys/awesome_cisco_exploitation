/* $Id: debug.c,v 1.1.4.1 1996/05/09 15:00:17 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/debug.c,v $
 *------------------------------------------------------------------
 * debug.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:17  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:59  rlowe
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

/*************************************************************
 * This module provides dbx support, permitting PMON to be used as
 * an execution vehicle when source-level debugging with dbx.

 To use this feature perform the following steps:
 
	1. Create the file /etc/remote.pdbx which should
	   contain the following single line.

		pmon:dv=/dev/tty1:br#9600

	2. Create the file ~/.dbxinit which should contain the following,

		set $pdbxport="pmon"
		set $usesockets=0
		set $manual_load=1

	3. Download the program to the target system

	4. Invoke dbx using the following command,

		dbx -prom <obj_file>

	5. Optionally set breakpoints e.g., "bp main".

	6. Start execution using the dbx command "run".

	7. Type "debug" on the target system's console.

*************************************************************/
/*#define NO_DBX_SUPPORT /* enable this to disable dbx support */

/*
 * Messages are transmitted over an RS232 serial link, and are of the
 * form:

 *	^V type+len sequence# data checkSum
 
 * These messages are transmitted by the routine putpkt and received by
 * getpkt.
 */

#include <stdio.h>
#include <termio.h>
#include <mips.h>
#include "pmon.h"

#ifndef NO_DBX_SUPPORT

#define DATA 0
#define ACKPKT 0x20
#define nextseq(x)	(((x)>=63)?0:x+1)
#define TIMEOUT 300000

Optdesc debug_opts[] = {
	{"-s","don't set client sp"},
	{"-c <args>","args to be passed to client"},
	{"-v","report protocol errors"},
	{"-V","verbose mode"},
	{0}};

int myseq;
int hisseq;
int hostfd;
int Vflag;
jmp_buf pktstart;
jmp_buf timeout;

extern char clientcmd[LINESZ];
extern char *clientav[MAX_AC];

/*************************************************************
*  debug(ac,av)
*	The 'debug' command
*/
debug(ac,av)
int ac;
char *av[];
{
struct termio tbuf;
char *hostport;
int i,j,sflag;

strcpy(clientcmd,av[0]);
strcat(clientcmd," ");

/* set client's a0 (argc) & a1 (argv) */
Gpr[4] = 1; 
Gpr[5] = (word)clientav;

vflag = Vflag = sflag = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j] != 0;j++) {
			if (av[i][j] == 'v') vflag = 1;
			else if (av[i][j] == 'V') Vflag = 1;
			else if (av[i][j] == 's') sflag = 1;
			else if (av[i][j] == 'c') {
				for (i++;i<ac;i++) {
					strcat(clientcmd,av[i]);
					strcat(clientcmd," ");
					}
				Gpr[4] = argvize(clientav,clientcmd);
				break;
				}
			else printf("%c: unknown option\n",av[i][j]);
			}
		}
	else printf("%s: unrecognized argument\n",av[i]);
	}

hostport = getenv("hostport");
if (Vflag) printf("hostport=%s\n",hostport);
hostfd = open(hostport,0);
if (hostfd == -1) {
	printf("can't open %s\n",hostport);
	return;
	}
if (Vflag) printf("hostfd=%d\n",hostfd);

ioctl(hostfd,TCGETA,&tbuf);
tbuf.c_lflag &= ~(ICANON|ECHO|ECHOE);
tbuf.c_cc[4] = 1;
ioctl(hostfd,TCSETAF,&tbuf);

#ifdef NEWVERSION
/* the idea is to make the startup easier. However, this won't
work the old way. So it's commented out for now. */
for (;;) {
	write(hostfd,"\r",1);
	for (i=0;i<1000000;i++) ;
	ioctl(hostfd,FIONREAD,&i);
	if (i > 0) break;
	for (i=0;i<1000000;i++) ;
	if (Vflag) putchar('.');
	}
#else
write(hostfd,"\r",1);
#endif

myseq = hisseq = 0;

if (!sflag) Gpr[29] = clienttos();
dbgmode(0);
}

/*************************************************************
*  dbgmode(type) enter dbx mode
*/
dbgmode(type)
int type;
{
char rxstr[80],*rxarg[8];
int ac;

switch (type) {
	case 0 : putmsg(hostfd,&myseq,"0x1 b 0x0 0x57F"); break;
	case 1 : putmsg(hostfd,&myseq,"0x1 c 0x0 0x57f"); break;
	case 2 : putmsg(hostfd,&myseq,"0x1 s 0x0 0x57f"); break;
	}

for (;;) {
	getmsg(hostfd,&hisseq,rxstr);
	ac = argvize(rxarg,rxstr);
	do_req(hostfd,ac,rxarg);
	}
}

/*************************************************************
*  putmsg(fd,seq,msg)	send msg, including wait for the ACK
*/
putmsg(fd,seq,msg)	
int fd,*seq;
char *msg;
{
int type,ts,ns;

ns = nextseq(*seq);
for (;;) {
	setjmp(timeout);
	putpkt(fd,*seq,msg);
	if ((type = getpkt(fd,&ts,0)) == ACKPKT && ts == ns) break;
	if (vflag) printf("bad ACK  type=%02x got seq=%d wanted seq=%d\n",
						type,ts,ns);
	if (ts != ns) putpkt(fd,nextseq(ts),0);
	}
*seq = ns;
if (Vflag) printf("\n");
}

/*************************************************************
*  getmsg(fd,seq,msg)	get msg, including send the ACK
*/
getmsg(fd,seq,msg)
int fd,*seq;
char *msg;
{
int type,ts;

for (;;) {
	if ((type = getpkt(fd,&ts,msg)) == DATA && ts == *seq) break;
	if (vflag) printf("bad DATA type=%02x seq=%d msg=%s\n",type,ts,msg);
	}
*seq = nextseq(*seq);
putpkt(fd,*seq,0);
}

/*************************************************************
*  putpkt(fd,seq,msg) send a packet, if msg == 0, type = ACK
*/
putpkt(fd,seq,msg) 
int fd;		/* file descriptor */
int seq;	/* sequence number to be sent */
char *msg;	/* message to be sent */
{
int len,type,type_len,i;
int csum,csum1,csum2,csum3;
char tmp[80];

if (Vflag) printf("PutPkt: fd=%d seq=%d msg=%s\n",fd,seq,msg); 
if (msg == 0) type = ACKPKT;
else type = DATA;

if (msg) len = strlen(msg);
else len = 0;

type_len = type | (len>>6);
type_len |= 0x40;
csum = type_len;
len &= 0x3f;
len |= 0x40;
csum += len;

seq |= 0x40;
csum += seq;

if (msg) for (i=0;msg[i] != 0;i++) csum += msg[i];
csum1 = csum>>12;
csum1 |= 0x40;
csum2 = (csum>>6)&0x3f;
csum2 |= 0x40;
csum3 = csum&0x3f;
csum3 |= 0x40;

if (msg) sprintf(tmp,"%c%c%c%c%s%c%c%c\n",CNTRL('v'),type_len,len,seq,msg,
					csum1,csum2,csum3);
else sprintf(tmp,"%c%c%c%c%c%c%c\n",CNTRL('v'),type_len,len,seq,
					csum1,csum2,csum3);
write(fd,tmp,strlen(tmp));
}

/*************************************************************
*  getpkt(fd,seq,msg) get a packet as a string, returns type
*/
getpkt(fd,seq,msg)	
int fd;		/* file descriptor */
int *seq;	/* received sequence number */
char *msg;	/* destination for message */
{
int len,type,csum,rsum,n,i;
char ch;

type = DATA;
setjmp(pktstart);
ch = readc(fd,msg);
csum = ch;
if (ch&ACKPKT) {
	type = ACKPKT;
	ch &= ~ACKPKT;
	}
len = ch - '@';
ch = readc(fd,msg);
csum += ch;
len = (len<<6) + (ch - '@');
ch = readc(fd,msg);
csum += ch;
*seq = ch - '@';
for (i=0;i<len;i++) {
	ch = readc(fd,msg);
	csum += ch;
	if (msg) msg[i] = ch;
	}
if (msg) msg[i] = 0;
rsum = readc(fd,msg) - '@';
rsum = (rsum<<6) + (readc(fd,msg) - '@');
rsum = (rsum<<6) + (readc(fd,msg) - '@');
if (Vflag) printf("GetPkt: fd=%d seq=%d msg=%s\n",fd,*seq,msg);
if (rsum != csum) {
	if (vflag) printf("error: csum expected %x actual %x\n",rsum,csum);
	return(-1);
	}
return(type);
}

/*************************************************************
*  readc(fd,msg)
*/
readc(fd,msg)
int fd;
char *msg;
{
char ch;
int n,i;

for (i=0;;i++) {
	if (msg) break; /* it's not an ACK */
	if (i > TIMEOUT) {
		if (vflag) printf("timeout\n");
		longjmp(timeout,1);
		}
	ioctl(fd,FIONREAD,&n);
	if (n > 0) break;
	}
		
read(fd,&ch,1);
if (ch == CNTRL('V')) longjmp(pktstart,1);
return(ch);
}


/*************************************************************
*  do_req(fd,ac,av) handle a dbx request
*	av[0]=pid av[1]=req av[2]=addr av[3]=data
*/
do_req(fd,ac,av)
int fd,ac;
char *av[];
{
unsigned long adr,dat,val;
word *rp;
int i;
char msg[80];

if (ac != 4) {
	printf("ac=%d: ");
	for (i=0;i<ac;i++) printf("av[%d]=%s ",i,av[i]);
	printf("\n");
	return;
	}

if (strlen(av[1]) != 1) {
	printf("unknown request type %s\n",av[1]);
	return;
	}

atob(&adr,av[2],0);
atob(&dat,av[3],0);

	switch (av[1][0]) {
		case 'i' : /* read Ispace */
		case 'd' : /* read Dspace */
			val = load_word(adr);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'h' : /* read half-word */
			val = load_half(adr);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'p' : /* read byte */
			val = load_byte(adr);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'r' : /* read reg */
			if (adr <= 31) rp = &DBGREG[adr];
			else if (adr <= 63) rp = &DBGREG[R_ZERO]; /* Fpr */
			else {
				switch (adr) {
				    case 96 : adr=R_EPC; break;
				    case 97 : adr=R_CAUSE; break;
				    case 98 : adr=R_HI; break;
				    case 99 : adr=R_LO; break;
				    case 100 : adr=R_ZERO; break; /* fcsr */
				    case 101 : adr=R_ZERO; break; /* feir */
				    default : adr=R_ZERO; break;
				    }
				rp = &DBGREG[adr];
				}
			val = *rp;
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'g' : /* read multiple regs */
			for (i=0;i<32;i++) {
				if (adr&1) rp = &DBGREG[i];
				val = *rp;
				sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
				putmsg(fd,&myseq,msg);
				adr >>= 1;
				}
			break;

		case 'I' : /* write Ispace */
		case 'D' : /* write Dspace */
			val = load_word(adr);
			store_word(adr,dat);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'H' : /* write half-word */
			val = load_byte(adr);
			store_half(adr,dat);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'P' : /* write byte */
			val = load_byte(adr);
			store_byte(adr,dat);
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;
		case 'R' : /* write reg */
			if (adr <= 31) rp = &DBGREG[adr];
			else if (adr <= 63) rp = &DBGREG[R_ZERO]; /* Fpr */
			else {
				switch (adr) {
				    case 96 : adr=R_EPC; break;
				    case 97 : adr=R_CAUSE; break;
				    case 98 : adr=R_HI; break;
				    case 99 : adr=R_LO; break;
				    case 100 : adr=R_ZERO; break; /* fcsr */
				    case 101 : adr=R_ZERO; break; /* feir */
				    default : adr=R_ZERO; break;
				    }
				rp = &DBGREG[adr];
				}
			val = *rp;
			if (rp != &DBGREG[R_ZERO]) *rp = dat;
			sprintf(msg,"%s %s 0x0 0x%x",av[0],av[1],val);
			putmsg(fd,&myseq,msg);
			break;

		case 's' : /* step */
			/* optional addr */
			if (adr != 1) DBGREG[R_EPC] = adr;
			sstep();
			break;
		case 'c' : /* continue */
			/* optional addr */
			if (adr != 1) DBGREG[R_EPC] = adr;
			trace_mode = TRACE_DC;
			_go();
			break;
		case 'x' : /* exit */
			longjmp(jmpb,1);
			break;

		default :
			printf("unknown request type '%s %s %s %s'\n",
				av[0],av[1],av[2],av[3]);
		}
}
#else
Optdesc debug_opts[] = {{0}};
debug(){printf("dbx support has been disabled in this build\n");}
dbgmode() {}
#endif
