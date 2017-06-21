/* $Id: load.c,v 1.1.4.1 1996/05/09 15:00:23 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/load.c,v $
 *------------------------------------------------------------------
 * load.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: load.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:23  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:30  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:05  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:00  rlowe
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

/*
 * Useful S-records for testing purposes:
 *	S0030000FC
 *	S309900200001234567860
 *	S7058002000073
 *
 * Useful fast-records for testing purposes:
 *	/ACgAgAA	- addr = a0020000
 *	/ZAI		- clear 24 bytes
 *	/E		- define entry at 'addr' and exit
 *	AAABAAAC	- 00 00 01 00 00 02
 *	/Sfred,X	- symbol = 'addr'
 *	/BAB		- byte of 01
 */

Optdesc load_opts[] = {
	{"-c cmdstr","send cmdstr to host"},
	{"-s","don't clear symbols"},
	{"-b","don't clear breakpoints"},
	{"-e","don't clear exception handlers"},
	{"-a","don't add offset to symbols"},
	{"-t","load at top of memory"},
	{"-i","ignore checksum errors"},
#ifdef ETHERNET
	{"-B","binary transfer mode"},
	{"-v","verbose mode"},
#endif
	{"-<num>","set baud rate"},
	{0}};

uchar *dlerrs[] = {0,"bad char","bad length","bad type",
		 "bad chksum", "out of symbol space"};

#define MSK 0xff

ulong nextaddr;
ulong wordbuf;
ulong tbase;
int bytecnt = 0;
int blkinx = 0;
int blksz = 0;
int chksum;

uchar exta = ',';
uchar extb = '.';
uchar extc = '/';

#define SFLAG 0x01
#define bFLAG 0x02
#define EFLAG 0x04
#define AFLAG 0x08
#define TFLAG 0x10
#define IFLAG 0x20
#define BFLAG 0x40

/*************************************************************
*  load(ac,av)
*/
load(ac,av)
int ac;
uchar *av[];
{
struct termio tbuf;
uchar *hostport,*cmdstr;
int fd,dlecho,dlproto,dltype,count,n;
int len,err,i,j,s,tot,flags;
ulong offset;
uchar recbuf[MAXREC],*baudrate;
int errs[6];

baudrate = 0;
offset = 0;
count = 0;
cmdstr = 0;
flags = 0;
for (i=0;i<6;i++) errs[i] = 0;
hostport = getenv("hostport");
vflag = 0;

for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j];j++) {
			if (av[i][j] == 's') flags |= SFLAG;
			else if (av[i][j] == 'b') flags |= bFLAG;
			else if (av[i][j] == 'B') flags |= BFLAG;
			else if (av[i][j] == 'e') flags |= EFLAG;
			else if (av[i][j] == 'a') flags |= AFLAG;
			else if (av[i][j] == 'v') vflag = 1;
			else if (av[i][j] == 't') {
				flags |= TFLAG;
				Gpr[7] = 0;
				}
			else if (av[i][j] == 'i') flags |= IFLAG;
			else if (av[i][j] == 'c') {
				i++;
				if (i >= ac) {
					printf("bad arg count\n");
					return;
					}
				cmdstr = av[i];
				break;
				}
			else if (isdigit(av[i][j])) {
				baudrate = &av[i][j];
				break;
				}
			else printf("%c: unrecognized option\n",av[i][j]);
			}
		}
	else {
		if (av[i][0] == 't') hostport = av[i];
		else if (count == 0) {
			if (!get_rsa(&offset,av[i])) return(1);
			count++;
			}
		else printf("%s: unrecognized argument\n",av[i]);
		}
	}

if (!(flags&SFLAG)) clrsyms();
if (!(flags&bFLAG)) clrbpts();
if (!(flags&EFLAG)) clrhndlrs();

fd = open(hostport,0,0);
if (fd == -1) {
	printf("can't open %s\n",hostport);
	return;
	}

dltype = -1; /* dltype is set by 1st char of 1st record */

printf("Downloading from %s, ^C to abort\n",hostport);
ioctl(fd,TCGETA,&tbuf);
tbuf.c_iflag &= ~IXOFF;
ioctl(fd,TCSETAF,&tbuf);

if (baudrate) {
	if ((n=getbaudrate(baudrate)) == 0) {
		printf("%s: unrecognized baud rate\n",baudrate);
		return;
		}
	tbuf.c_cflag &= ~CBAUD;
	tbuf.c_cflag |= n;
	ioctl(fd,TCSETAF,&tbuf);
	}

dlecho = matchenv("dlecho");
if (dlecho == OFF || dlecho == LFEED) {
	tbuf.c_lflag &= ~ECHO;
	ioctl(fd,TCSETAF,&tbuf);
	}

dlproto = matchenv("dlproto");

if (cmdstr) {
	write(fd,cmdstr,strlen(cmdstr));
	write(fd,"\r",1);
	}

#ifdef ETHERNET
if (flags&BFLAG) {
	ether_bcopy();
	return;
	}
#endif

chksum = 0;
err = tot = 0;
for (;;) {
#ifdef SABLE
	blksz = mread(fd,recbuf,MAXREC-1);
#else
	blksz = read(fd,recbuf,MAXREC-1);
#endif
	if (blksz) blksz--; /* remove line termination char */
	recbuf[blksz] = 0; /* null terminate the line */
	if (dlproto == XONXOFF) writec(fd,CNTRL('s'));
	if (strempty(recbuf)) {
		if (dlproto == XONXOFF) writec(fd,CNTRL('q'));
		else if (dlproto == ETXACK) writec(fd,CNTRL('f'));
		if (dlecho == LFEED) writec(fd,'\n');
		continue; /* skip blank lines */
		}
	blkinx = 0;
	if (dltype == -1) {
		if (recbuf[0] == '/') dltype = 1;
		else if (recbuf[0] == 'S') dltype = 0;
		else {
			printf("bad record type\n");
			dltype = ETXACK;
			err++;
			break;
			}
		}
	if (dltype == 1) s = fload(recbuf,offset,&len,flags);
	else if (dltype == 0) s = sload(recbuf,offset,&len,flags);
	/* return: -1..-N = err, 0 = end, 1 = ok */

	if (s < 0) {
		err++;
		errs[0-s]++;
		}
	else tot += len;

	if (s == 0) break;
	if (s < 0 && dlproto == ETXACK) break;

	if (dlproto == XONXOFF) writec(fd,CNTRL('q'));
	else if (dlproto == ETXACK) writec(fd,CNTRL('f'));
	if (dlecho == LFEED) writec(fd,'\n');
	}

if (dlproto == XONXOFF) writec(fd,CNTRL('q'));
else if (dlproto == ETXACK) {
	if (err > 0) writec(fd,CNTRL('u'));
	else writec(fd,CNTRL('f'));
	}
if (dlecho == LFEED) writec(fd,'\n');
if (fd > STDERR) close(fd);
flush_cache(ICACHE);

if (err) {
	printf("\n%d errors\n",err);
	for (i=1;i<6;i++) if (errs[i]) printf("   %3d %s\n",errs[i],dlerrs[i]);
	}
else printf("\ntotal = 0x%x bytes\n",tot);
}

/*************************************************************
*  sload(recbuf,offset,plen,flags)
*	load Motorola S-records
*/
sload(recbuf,offset,plen,flags)
uchar *recbuf;
ulong offset;
int *plen,flags;
{
int len,x;

*plen = 0;
len = 0;
if(recbuf[0] == 'S') {
	if (recbuf[1] >= '7' && recbuf[1] <= '9') {
		if (endrec(recbuf,offset)) return(0);
		else return(-1);
		}
	else if (recbuf[1] == '4') {
		if (dlsym(recbuf,(flags&AFLAG)?0:offset)) return(-1);
		}
	else if (recbuf[1] >= '1' && recbuf[1] <= '3') {
		if (x=datarec(recbuf,offset,&len,flags)) return(x);
		}
	else if (recbuf[1] == '0') return(1);
	else return(-1);
	}
else return(-1);

if (len >= 0) {
	*plen = len;
	return(1);
	}
else return(-1);
}

/*************************************************************
*  datarec(p,offset,plen,flags)
*	handle an S-record data record
*/
datarec(p,offset,plen,flags)
uchar *p;
ulong offset;
int *plen,flags;
{
int len,i,cs;
ulong addr,v;

*plen = 0;
chksum = 0;
if (!gethex(&len,&p[2],2)) return(-1);
chksum += len;
if (len*2 != strlen(p)-4) return(-2);
i = 4; addr = 0;
switch (p[1]) {
	case '3' :
		if (!gethex(&v,&p[i],2)) return(-1);
		addr = (addr<<8)+v;
		chksum += v;
		i += 2;
	case '2' :
		if (!gethex(&v,&p[i],2)) return(-1);
		addr = (addr<<8)+v;
		chksum += v;
		i += 2;
	case '1' :
		if (!gethex(&v,&p[i],2)) return(-1);
		addr = (addr<<8)+v;
		chksum += v;
		if (!gethex(&v,&p[i+2],2)) return(-1);
		addr = (addr<<8)+v;
		chksum += v;
		break;
	default  : return(-3);
	}

#if 1
if (flags&TFLAG) {
	if (Gpr[7] == 0) {
		memorysize -= addr;
		Gpr[29] = clienttos();
		tbase = memorysize+(FDATA&~0xfff)-addr;
		Gpr[7] = tbase+addr;
		}
	addr += tbase;
	}
else addr += offset;
#else
addr += offset; 
#endif

p = &p[i+4];
len -= (i/2)+1;
for (i=0;i<len;i++,p+=2) {
	if (!gethex(&v,p,2)) return(-1);
	write_target(addr++,v,1);
	chksum += v;
	}

if (!gethex(&cs,p,2)) return(-1);
if (!(flags&IFLAG) && cs != ((~chksum)&0xff)) return(-4);
*plen = i;
return(0);
}

/*************************************************************
*  writec(fd,ch)
*/
writec(fd,ch)
int fd;
uchar ch;
{
write(fd,&ch,1);
}

/*************************************************************
*  dlsym(p,offset)
*	handle S4 records, i.e., symbols
*/
dlsym(p,offset)
uchar *p;
ulong offset;
{
uchar val;
word adr;
int len,csum;
uchar name[LINESZ],*t;

/* 
S4LLAAAAAAAANNNNNNNN,AAAAAAAANNNNNN,AAAAAAAANNNNNNNNNN,CC
LL=length AAAAAAAA=addr NNNN,=name CC=checksum 
*/

p += 2; /* skip past S4 */
if (!gethex(&len,p,2)) return(1);
p += 2; /* skip past length */
for (;len > 2;) {
	if (!gethex(&adr,p,8)) return(2);
	p += 8; /* skip past addr */
	len -= 8;

	t = strchr(p,',');
	if (t == 0) return;
	strncpy(name,p,t-p);
	len -= t-p;

	if (!newsym(name,adr+offset)) return(3);

	p = t+1;
	}
if (!gethex(&csum,p,2)) return(4);
/* csum neither generated nor checked */
return(0);
}

/*************************************************************
*  endrec(p,offset)
*	handle the S-record termination record
*/
endrec(p,offset)
uchar *p;
ulong offset;
{
word adr;

switch (p[1]) {
	case '7' : gethex(&adr,&p[4],8); break;
	case '8' : gethex(&adr,&p[4],6); break;
	case '9' : gethex(&adr,&p[4],4); break;
	}
Epc = adr+offset;
printf("Entry address is %08x\n",adr+offset);
}


/*************************************************************
*  fload(recbuf,offset,plen,flags)
*	load a fast-mode record
*/
fload(recbuf,offset,plen,flags)
uchar *recbuf;
ulong offset;
int *plen,flags;
{
int n,i,j,c,len,s;
uchar adat[8],name[LINESZ];
uchar buf[3];
int bdat,zcnt;
ulong addr;

*plen = 0;
s = 1;
for (len=0;;) {
	if (!getrec(adat,recbuf)) break;
	if (adat[0] == '/') {
		switch (adat[1]) {
		    case 'K' :  /* klear checksum (sic) */
			chksum = 0;
			break;
		    case 'C' :  /* compare checksum */
			if (!getb64(&zcnt,&adat[2],-2)) return(-1);
			chksum &= 0xfff;
			if (!(flags&IFLAG) && zcnt != chksum) return(-4);
			chksum = 0;
			break;
		    case 'Z' :  /* zero fill */
			if (!getb64(&zcnt,&adat[2],2)) return(-1);
			buf[0] = buf[1] = buf[2] = 0;
			for (i=0;i<zcnt;i++) {
				memwrite(buf,3);
				len += 3;
				}
			break;
		    case 'B' :  /* byte */
			if (!getb64(&j,&adat[2],2)) return(-1);
			buf[0] = j;
			memwrite(buf,1);
			len++;
			break;
		    case 'A' :  /* address */
			if (!getrec(&adat[4],recbuf)) return(-2);
			if (!getb64(&addr,&adat[2],6)) return(-1);
			if (flags&TFLAG && Gpr[7] == 0) {
				memorysize -= addr;
				Gpr[29] = clienttos();
				nextaddr = memorysize+(FDATA&~0xfff);
				Gpr[7] = nextaddr;
				}
			else nextaddr = addr+offset;
			break;
		    case 'E' :  /* end */
			Epc = nextaddr;
			printf("Entry Address  = %08x\n",nextaddr);
			s = 0;
			break;
		    case 'S' :  /* symbol */
			i = 2;
			for (j=0;;) {
				c = adat[i++];
				/* chksum += c; */
				if (c == ',') break;
				name[j++] = c;
				if (i >= 4) {
					if (!getrec(adat,recbuf)) return(-2);
					i = 0;
					}
				}
			name[j] = 0;
			if (!newsym(name,nextaddr)) return(-5);
			break;
		    default : 
			return(-3);
		    	}
		}
	else {  /* data */
		if (!getb64(&bdat,&adat[0],4)) return(-1);
		for (i=2;i>=0;i--) {
			buf[i] = bdat&MSK;
			bdat>>= 8;
			}
		memwrite(buf,3);
		len += 3;
		}
	}
*plen = len;
return(s);
}

/*************************************************************
*  getrec(p,curblk)
*	get the next 4-byte record (if there's any left)
*/
getrec(p,curblk)
uchar *p,*curblk;
{
int i;

if (blkinx >= blksz) return(0);
for (i=0;i<4;i++) p[i] = curblk[blkinx++];
return(1);
}

/*************************************************************
*  memwrite(dat,n)
*	write the data to memory
*/
memwrite(dat,n)
uchar *dat;
int n;
{
int i;

for (i=0;i<n;i++) {
	write_target(nextaddr++,dat[i],1);
	}
}

/*************************************************************
*  getb64(vp,p,n)
*	get a number of base-64 digits
*	if n < 0 then don't add value to chksum.
*/
getb64(vp,p,n)
ulong *vp;
uchar *p;
int n;
{
ulong v;
int c,i;

v = 0; *vp = 0;
for (i=0;i<((n<0)?0-n:n);i++) {
	v <<= 6;
	c = *p++;
	if (c >= 'A' && c <= 'Z') v |= c-'A';
	else if (c >= 'a' && c <= 'z') v |= c-'a'+26;
	else if (c >= '0' && c <= '9') v |= c-'0'+52;
	else if (c == exta) v |= 62; 
	else if (c == extb) v |= 63; 
	else return(0);
	}

switch (n) {
	case 6 : chksum += (v>>24)&0xfff;
	case 4 : chksum += (v>>12)&0xfff;
	case 2 : chksum += v&0xfff;
	}

*vp = v;
return(1);
}


