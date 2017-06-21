/* $Id: commands.c,v 1.1.4.1 1996/05/09 15:00:16 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/commands.c,v $
 *------------------------------------------------------------------
 * commands.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: commands.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:16  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:23  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:58  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:54  rlowe
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

char *searching = "searching..  ";

char *badhexsym = "%s: neither hex value nor symbol\n";

/*************************************************************
*  transp(ac,av)
*	the 'tr' command 
*/
transp(ac,av)
int ac;
char *av[];
{
int fd,n,i,j,auxA,auxB,twoflg;
jmp_buf intrsave;
char *hostport,buf[80],abortch,*trabort,*auxport,auxportB[10];
struct termio tbuf,consave,hostsave;

trabort = getenv("trabort");
abortch = str2cc(trabort);
if (abortch == 0) {
	printf("tr: error: trabort not set\n");
	return;
	}

auxport = 0;
twoflg = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j];j++) {
			if (av[i][j] == '2') twoflg = 1;
			else {
				printf("%c: unknown option\n",av[i][j]);
				return;
				}
			}
		}
	else {
		if (!auxport) auxport = av[i];
		else {
			printf("%s: unknown option\n",av[i]);
			return;
			}
		}
	}

hostport = getenv("hostport");
if (auxport) {
	if (strequ(auxport,"tty0")) {
		printf("can't use tty0 for transparent mode\n");
		return;
		}
	fd = open(auxport,0);
	if (fd == -1) {
		printf("can't open %s\n",auxport);
		return;
		}
	if (twoflg) {
		if (!hostport) {
			printf("hostport not defined\n");
			return;
			}
		auxA = open(hostport,0);
		if (auxA == -1) {
			close(fd);
			printf("can't open %s\n",hostport);
			return;
			}
		atob(&n,&auxport[3],10);
		n++;
		sprintf(auxportB,"tty%d",n);
		auxB = open(auxportB,0);
		if (auxB == -1) {
			close(fd);
			close(auxA);
			printf("can't open %s\n",auxportB);
			return;
			}
		}
	}
else {
	if (!hostport) {
		printf("hostport not defined\n");
		return;
		}
	if (strequ(hostport,"tty0")) {
		printf("can't use tty0 for transparent mode\n");
		return;
		}
	fd = open(hostport,0);
	if (fd == -1) {
		printf("can't open %s\n",hostport);
		return;
		}
	}

printf("Entering transparent mode. %s to abort\n",trabort);

ioctl(fd,TCGETA,&tbuf);
hostsave = tbuf;
tbuf.c_lflag &= ~(ICANON|ECHO|ECHOE);
tbuf.c_iflag &= ~(ICRNL);
tbuf.c_iflag |= IXOFF; /* enable tandem mode */
tbuf.c_oflag &= ~ONLCR;
tbuf.c_cc[4] = 1;
ioctl(fd,TCSETAF,&tbuf);

ioctl(STDIN,TCGETA,&tbuf);
consave = tbuf;
tbuf.c_lflag &= ~(ICANON|ECHO|ECHOE);
tbuf.c_iflag &= ~(ICRNL|IXON);
tbuf.c_oflag &= ~ONLCR;
tbuf.c_cc[4] = 1;
ioctl(STDIN,TCSETAF,&tbuf);

if (twoflg) {
	ioctl(auxA,TCGETA,&tbuf);
	tbuf.c_lflag &= ~(ICANON|ECHO|ECHOE);
	tbuf.c_iflag &= ~(ICRNL);
	tbuf.c_iflag |= IXOFF; /* enable tandem mode */
	tbuf.c_oflag &= ~ONLCR;
	tbuf.c_cc[4] = 1;
	ioctl(auxA,TCSETAF,&tbuf);

	ioctl(auxB,TCGETA,&tbuf);
	tbuf.c_lflag &= ~(ICANON|ECHO|ECHOE);
	tbuf.c_iflag &= ~(ICRNL|IXON);
	tbuf.c_oflag &= ~ONLCR;
	tbuf.c_cc[4] = 1;
	ioctl(auxB,TCSETAF,&tbuf);
	}

/* disable INTR char */
ioctl(STDIN,GETINTR,intrsave);
ioctl(STDIN,SETINTR,0);

for (;;) {
	ioctl(STDIN,FIONREAD,&n);
	if (n > 0) {
		n = read(STDIN,buf,n);
		if (strchr(buf,abortch)) break;
		if (ioctl(fd,TXRDY,0)) write(fd,buf,n);
		}
	ioctl(fd,FIONREAD,&n);
	if (n > 0) {
		n = read(fd,buf,n);
		write(STDOUT,buf,n);
		}
	if (twoflg) {
		ioctl(auxA,FIONREAD,&n);
		if (n > 0) {
			n = read(auxA,buf,n);
			write(auxB,buf,n);
			}
		ioctl(auxB,FIONREAD,&n);
		if (n > 0) {
			n = read(auxB,buf,n);
			write(auxA,buf,n);
			}
		}
	}
ioctl(STDIN,TCSETAF,&consave);
ioctl(fd,TCSETAF,&hostsave);
ioctl(STDIN,SETINTR,intrsave);
}

Optdesc m_opts[] = {
	{"hexval","set memory, forward one"},
	{"CR","forward one, no change"},
	{"=","re-read"},
	{"^|-","back one"},
	{".","quit"},
	{0}};

/*************************************************************
*  modify(ac,av)
*	the 'm' command 
*/
modify(ac,av)
int ac;
char *av[];
{
word a,adr, i;
char *p;
unsigned long c;
int datasz;

i = matchenv("datasz");
switch (i) {
	case 0 : datasz=1; break;
	case 1 : datasz=2; break;
	case 2 : datasz=4; break;
	default : 
		printf("%s: bad datasz value\n",getenv("datasz"));
		return(1);
	}

if (ac > 1) if(!get_rsa(&adr,av[1])) return(1);
if (ac > 2) {
	for (i = 2; i < ac; i++) {
		if (strequ(av[i],"-s")) {
			i++;
			if (i >= ac) printf("bad arg count\n");
			else for (p=av[i];*p;p++) store_byte(adr++,*p);
			}
		else {
			if(!get_rsa(&a,av[i])) return(1);
			c = a;
			switch (datasz) {
				case 1 : store_byte(adr,c); break;
				case 2 : store_half(adr,c); break;
				case 4 : store_word(adr,c); break;
				}
			adr += datasz;
			}
		}
	}
else {
	for (;;) {
		switch (datasz) {
		    case 1 :
			printf("%08x %02x ",adr,(unsigned)load_byte(adr));
			break;
		    case 2 :
			printf("%08x %04x ",adr,(unsigned)load_half(adr));
			break;
		    case 4 :
			printf("%08x %08x ",adr,load_word(adr));
			break;
			}
		gets(line);
		for (p = line; *p == ' '; p++) ;
		if (*p == 0) adr += datasz;
		else if (*p == '.') break;
		else if (*p == '^' || *p == '-') adr -= datasz;
		else if (*p == '=') adr += datasz;
		else if (get_rsa(&c,p)) {
			switch (datasz) {
				case 1 : store_byte(adr,c); break;
				case 2 : store_half(adr,c); break;
				case 4 : store_word(adr,c); break;
				}
			adr += datasz;
			}
		}
	}
return(0);
}

/*************************************************************
*  search(ac,av)
*	the search command 
*/
search(ac,av)
int ac;
char *av[];
{
uword from, to, adr, i,a;
char *s, *d, c;
char pat[PATSZ];
int siz,ln;

if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}
ln = siz;
ioctl(STDIN,CBREAK,NULL);

if(!get_rsa(&from,av[1]) || !get_rsa(&to,av[2])) return(1);
for(d=pat,i=3; i < ac; i++){
	if (strequ(av[i],"-s")) {
		i++;
		if (i >= ac) printf("bad arg count\n");
		else for (s=av[i];*s;s++) *d++ = *s;
		}
	else {
			if(!get_rsa(&a,av[i])) return(1);
			c = a;
			*d++ = c;
		}
	}

if (to <= from) {
	printf("'to' address too small\n");
	return(1);
	}

printf("%s",searching);
if (from <= to) { /* forward search */
	to -= d - pat - 1;
	while (from <= to) {
		s = pat;
		adr = from++;
		while (s < d) {
			if(*s != load_byte(adr)) break;
			s++; adr++;
			}
		if (d <= s) {
			era_line(searching);
			dispmem(prnbuf,from - 1,4); 
			if (more(prnbuf,&ln,siz)) break;
			printf("%s",searching);
			}
		else dotik(1,0);
		}
	if (from > to) era_line(searching);
	}
else { /* backward search */
	from -= d - pat - 1;
	while (to <= from) {
		s = pat;
		adr = from--;
		while (s < d) {
			if(*s != load_byte(adr)) break;
			s++; adr++;
			}
		if(d <= s) {
			era_line(searching);
			dispmem(prnbuf,from - 1,4); 
			if (more(prnbuf,&ln,siz)) break;
			printf("%s",searching);
			}
		}
	if (to > from) era_line(searching);
	}
return(0);
}

/*************************************************************
*  call(ac,av)
*	the call command 
*/
call(ac,av)
int ac;
char *av[];
{
int i,j,k;
char *arg[10];

arg[0] = 0;
k = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		j = 1;
		while (av[i][j] != 0) {
			if (av[i][j] == 's') {
				i++;
				if (i >= ac) printf("bad arg count\n");
				else arg[k++] = av[i];
				break;
				}
			else printf("%c: unknown option\n",av[i][j]);
			j++;
			}
		}
	else {
		if (!get_rsa(&arg[k],av[i])) return;
		k++;
		}
	}
if (arg[0] != 0) (((Func *)arg[0]))(arg[1],arg[2],arg[3],arg[4],arg[5]);
else printf("Function address not specified\n");
}

Optdesc flush_opts[] = {
	{"-i","flush I-cache"},
	{"-d","flush D-cache"},
	{0}};

/*************************************************************
*  flush(ac,av)
*	the 'flush' command 
*/
flush(ac,av)
int ac;
char *av[];
{
int flags,i,j;

flags = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j];j++) {
			if (av[i][j] == 'd') flags |= 1;
			else if (av[i][j] == 'i') flags |= 2;
			else printf("%c: bad option\n",av[i][j]);
			}
		}
	else printf("%s: bad argument\n",av[i]);
	}
if (!flags) flags = 3;

if (flags&2) flush_cache(ICACHE);
if (flags&1) flush_cache(DCACHE);
return(0);
}

/*************************************************************
*  sdump(ac,av)
*	the 'dump' command 
*/
sdump(ac,av)
int ac;
char *av[];
{
uword adr,siz,len,i,a;
char tmp[4];
char *uleof,*ulcr;
int fd,cs,v;
struct termio tbuf;

if(!get_rsa(&adr,av[1]) || !get_rsa(&siz,av[2])) return(1);
fd = open(getenv("hostport"),0);
if (fd == -1) {
	printf("can't open %s\n",getenv("hostport"));
	return(1);
	}
ioctl(fd,TCGETA,&tbuf);
tbuf.c_iflag &= ~IXANY; 
ioctl(fd,TCSETAF,&tbuf);

uleof = getenv("uleof");
ulcr = getenv("ulcr");

while (siz > 0) {
	if (siz < 32) len = siz;
	else len = 32;
	cs = len+5;
	for (i=0;i<4;i++) cs += (adr>>(i*8))&0xff;
	sprintf(line,"S3%02X%08X",len+5,adr);
	for (a=adr,i=0;i<len;a++,i++) {
		v = load_byte(a);
		cs += v;
		sprintf(tmp,"%02X",v);
		strcat(line,tmp);
		}
	sprintf(tmp,"%02X\n",(~cs)&0xff);
	strcat(line,tmp);
	if (strequ(ulcr,"on")) line[strlen(line)-1] = '\r';
	write(fd,line,strlen(line));
	adr += len;
	siz -= len;
	}
strcpy(line,"S70500000000FA\n");
if (strequ(ulcr,"on")) line[strlen(line)-1] = '\r';
write(fd,line,strlen(line));
write(fd,uleof,strlen(uleof));
return(0);
}

/*************************************************************
*  copy(ac,av)
*	the 'copy' command 
*/
copy(ac,av)
int ac;
char *av[];
{
uword from, to, n;

if(!get_rsa(&from,av[1]) || !get_rsa(&to,av[2]) || !get_rsa(&n,av[3]))
	return(1);
if (to < from)
	while(n-- > 0) write_target(to++, load_byte(from++),1);
else
	for (from += n,to += n;n-- > 0;) 
		write_target(--to, load_byte(--from),1);
flush_cache(ICACHE);
return(0);
}

Optdesc d_opts[] = {
	{"-b","display as bytes"},
	{"-h","display as half-words"},
	{"-w","display as words"},
	{"-s","display a null terminated string"},
	{"-r<reg>","display as register"},
	{0}};

/*************************************************************
*  dump(ac,av)
*	the 'd' command 
*/
dump(ac,av)
int ac;
char *av[];
{
word adr, siz, l;
char *reg;
int i,j,datasz,n;

if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}

i = matchenv("datasz");
switch (i) {
	case 0 : datasz = 1; break;
	case 1 : datasz = 2; break;
	case 2 : datasz = 4; break;
	default :
		printf("%s: bad datasz value\n",getenv("datasz"));
		return(1);
	}

n = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		for (j=1;av[i][j] != 0;j++) {
			switch (av[i][j]) {
				case 'w' : datasz=4; break;
				case 'b' : datasz=1; break;
				case 'h' : datasz=2; break;
				case 's' : datasz=0; break;
				case 'r' : 
					reg = &av[i][j+1];
					datasz = -1; 
					break;
				default :
					printf("%c: unknown option\n",av[i][j]);
					return(1);
				}
			if (datasz == -1) break;
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
				printf("%s: unknown argument\n",av[i]);
				return(1);
			}
		n++;
		}
	}

if (n == 0) {
	printf("address not specified\n");
	return(1);
	}

if (datasz == 0) { /* -s, print string */
	strncpy(prnbuf,adr,70);
	for (i=0;prnbuf[i] != 0;i++) 
		if (!isprint(prnbuf[i])) prnbuf[i] = '.';
	printf("%s\n",prnbuf);
	return;
	}

if (datasz == -1) { /* -r<reg> print as register */
	if (!disp_as_reg(load_word(adr),reg)) 
		printf("%s: bad register name\n",reg);
	return;
	}

ioctl(STDIN,CBREAK,NULL);
l = siz;
while(1) {
	adr = dispmem(prnbuf,adr,datasz);
	if (more(prnbuf,&l,(n > 1)?0:siz)) break;
	}
}

/*************************************************************
*  word dispmem(p,adr,siz)
*/
word dispmem(p,adr,siz)
char *p;
word adr;
int siz;
{
int i;
char v;
char tmp[10];
unsigned char buf[16];
unsigned short *ph;
unsigned long *pw;
word w;

w = adr;
for(i = 0; i < 16 ; i++){
	v = load_byte(w++);
	buf[i] = v;
	}

sprintf(p,"%08x  ",adr);
for(i = 0; i < 16 ; i += siz){
	if(i == 8) strccat(p,' ');
	switch (siz) {
		case 1 :
			sprintf(tmp,"%02x ",buf[i]);
			break;
		case 2 :
			ph = (unsigned short *)&buf[i];
			sprintf(tmp,"%04x ",*ph);
			break;
		case 4 :
			pw = (unsigned long *)&buf[i];
			sprintf(tmp,"%08x ",*pw);
			break;
		}
	strcat(p,tmp);
	}
strcat(p,"  ");
for(i = 0; i < 16 ; i++){
	v = buf[i];
	if (isprint(v)) strccat(p,v);
	else strccat(p,'.');
	}
return(adr+16);
}

/*************************************************************
*  fill(ac,av)
*	the fill command 
*/
fill(ac,av)
int ac;
char *av[];
{
uword from,to,i,a,w;
unsigned char *s, *d, c;
unsigned char pat[PATSZ],*p;
int len;

if(!get_rsa(&from,av[1]) || !get_rsa(&to,av[2])) return(1);
if(to < from)
	return(1);
for(d=pat,i=3; i < ac; i++){
	if (strequ(av[i],"-s")) {
		i++;
		if (i >= ac) printf("bad arg count\n");
		else for (s=av[i];*s;s++) *d++ = *s;
		}
	else {
		if(!get_rsa(&a,av[i])) return(1);
		c = a;
		*d++ = c;
		}
	}

len = d-pat; p = pat;
if ((len==1 || len==2 || len==4) && (from&3)==0 && (to&3)==0) {
	/* do special cases using word writes (faster) */
	w = *p++;
	switch (len) {
		case 1 :
			w = (w<<8)+w;
			w = (w<<16)+w;
			break;
		case 2 :
			w = (w<<8)+(*p++);
			w = (w<<16)+w;
			break;
		case 4 :
			w = (w<<8)+(*p++);
			w = (w<<8)+(*p++);
			w = (w<<8)+(*p++);
			break;
		}
	for (;from <= to;from+=4) write_target(from,w,4);
	return(0);
	}

/* all other cases */
for (s = pat;from <= to; from++) {
	*(char *)from = *s;
	if (++s >= d) s = pat;
	}
flush_cache(ICACHE);
return(0);
}

/*************************************************************
*  get_rsa(vp,p)
*	get Register/Symbol/Address
*/
get_rsa(vp,p)
unsigned long *vp;
char *p;
{
int r,inbase,inalpha;
unsigned long v1,v2;
char *q,subexpr[LINESZ];

/* strip enclosing parens */
while (*p == '(' && strbalp(p) == p+strlen(p)-1) {
	strdchr(p);
	p[strlen(p)-1] = 0;
	}

if (q=strrpset(p,"+-")) { /* is compound */
	strncpy(subexpr,p,q-p);
	r = get_rsa(&v1,subexpr);
	if (r == 0) return(r);
	r = get_rsa(&v2,q+1);
	if (r == 0) return(r);
	if (*q == '+') *vp = v1 + v2;
	else *vp = v1 - v2;
	return(1);
	}

if (q=strrpset(p,"*/")) { 
	strncpy(subexpr,p,q-p);
	r = get_rsa(&v1,subexpr);
	if (r == 0) return(r);
	r = get_rsa(&v2,q+1);
	if (r == 0) return(r);
	if (*q == '*') *vp = v1 * v2;
	else *vp = v1 / v2;
	return(1);
	}

if (*p == '^') {
	r = get_rsa(&v2,p+1);
	if (r == 0) return(r);
	*vp = load_word(v2);
	return(1);
	}

if (*p == '@') {
	r = getreg(vp,&p[1]);
	if (r == 0) printf("%s: bad register name\n",&p[1]);
	}
else if (strequ(p,".")) {
	r = getreg(vp,"epc");
	if (r == 0) printf("%s: bad register name\n",&p[1]);
	}
else if (*p == '&') {
	r = sym2adr(vp,&p[1]);
	if (r == 0) printf("%s: bad symbol name\n",&p[1]);
	}
else if (isdigit(*p)) {
	inbase = matchenv("inbase");
	if (inbase == TEN) r = atob(vp,p,10);
	else if (inbase == SIXTEEN) r = atob(vp,p,16);
	else if (inbase == EIGHT) r = atob(vp,p,8);
	else if (inbase == AUTO) r = atob(vp,p,0);
	else {
		printf("%s: bad inbase value\n",getenv("inbase"));
		return(0);
		}
	if (r == 0) {
		r = atob(vp,p,0);
		if (r == 0) printf("%s: bad base %s value\n",
						p,getenv("inbase"));
		}
	}
else if (isxdigit(*p)) {
	inalpha = matchenv("inalpha");
	if (inalpha == HEX) {
		r = atob(vp,p,16);
		if (r == 0) {
			r = sym2adr(vp,p);
			if (r == 0) printf(badhexsym,p);
			}
		}
	else if (inalpha == SYMBOL) {
		r = sym2adr(vp,p);
		if (r == 0) {
			r = atob(vp,p,16);
			if (r == 0) printf(badhexsym,p);
			}
		}
	else {
		printf("%s: bad inalpha value\n",getenv("inalpha"));
		return(0);
		}
	}
else {
	r = sym2adr(vp,p);
	if (r == 0) printf("%s: bad symbol name\n",p);
	}
return(r);
}

