/* $Id: hist.c,v 1.1.4.1 1996/05/09 15:00:22 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/hist.c,v $
 *------------------------------------------------------------------
 * hist.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hist.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:22  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:30  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:04  rlowe
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

#define PREV	CNTRL('P')
#define NEXT	CNTRL('N')
#define FORW	CNTRL('F')
#define BACK	CNTRL('B')
#define BEGIN	CNTRL('A')
#define END	CNTRL('E')
#define DELETE	CNTRL('D')

#define HISTMAX 200
char *hist[HISTMAX+1];	/* array of old commands */

int histi;	/* index to hist[], indicates next slot */
int histo;	/* index to hist[], indicates oldest entry */
int histr;	/* index to hist[], used while walking back with PREV */
int histno;	/* current history number */
int column;	/* current column number */

char *gethistn(),*gethists();

/*************************************************************
*  histinit() initialize history mechanism
*/
histinit()
{

histno = 1;
}

/*************************************************************
*  get_cmd(p) get a line from stdin using lineedit and history
*/
get_cmd(p)
char *p;
{
int c,oc,v,i;
struct termio tbuf;
char *q,buf[20],*t;

ioctl(STDIN,SETNCNE,&tbuf);
*p = 0;
histr = histi;
column = 0;
loop {
	c = getchar();
	if (c == PREV) {
		right(p,strlen(p)-column);
		backspace(column);
		if (histr != histo) histr = decmod(histr,HISTMAX);
		strcpy(p,hist[histr]);
		putstr(p);
		}
	else if (c == NEXT) {
		right(p,strlen(p)-column);
		backspace(column);
		if (histr != histi) histr = incmod(histr,HISTMAX);
		strcpy(p,hist[histr]);
		putstr(p);
		}
	else if (c == FORW) right(p,1);
	else if (c == BACK) left(1);
	else if (c == BEGIN) left(column);
	else if (c == END) right(p,strlen(&p[column]));
	else if (c == DELETE) {
		oc = column;
		strdchr(&p[column]);
		putstr(&p[column]);
		putstr(" ");
		left(strlen(&p[oc])+1);
		}
	else if (c == '\b') {
		left(1);
		oc = column;
		strdchr(&p[column]);
		putstr(&p[column]);
		putstr(" ");
		left(strlen(&p[oc])+1);
		}
	else if (c == '\n' || c == '\r') {
		putstr("\n");
		break;
		}
	else addchar(p,c);
	}

ioctl(STDIN,TCSETAW,&tbuf);
/* do C-shell history subs */
if (q = findbang(p)) {
	getword(buf,q);
	strdchr(buf); /* delete the bang */
	if (!strempty(buf)) {
		for (i=strlen(buf)+1;i>0;i--) strdchr(q);
		if (strequ(buf,"!")) { /* repeat last cmd */
			stristr(q,hist[decmod(histi,HISTMAX)]);
			printf("%s\n",p);
			}
		else if (atob(&v,buf,10)) { /* hist# */
			if (t = gethistn(v)) {
				stristr(q,t);
				printf("%s\n",p);
				}
			else printf("%s: Event not found.\n",buf);
			}
		else { /* hist string */
			if (t = gethists(buf)) {
				stristr(q,t);
				printf("%s\n",p);
				}
			else printf("%s: Event not found.\n",buf);
			}
		}
	}
addhist(p);
}

/*************************************************************
*  addchar(p,c) add char to line, updates 'column'
*/
addchar(p,c)
char *p;
int c;
{
int oc;

if (strlen(p) > LINESZ-40) return;
oc = column;
strichr(&p[column],c);
putstr(&p[column]);
left(strlen(&p[oc])-1);
}

/*************************************************************
*  word dohi() hi command
*/
word dohi(ac,av)
int ac;
char *av[];
{
int i,n,siz,l;

if(ac == 2) {
	if(!get_rsa(&siz,av[1])) return(1);
	}
else {
	if (!atob(&siz,getenv("moresz"),10)) {
		printf("%s: bad moresz value\n",getenv("moresz"));
		return(1);
		}
	}

ioctl(STDIN,CBREAK,NULL);
l = siz;
i = decmod(histi,HISTMAX);
n = histno-1;
while (i != decmod(histo,HISTMAX)) {
	sprintf(prnbuf,"%5d  %s",n,hist[i]);
	if (more(prnbuf,&l,(ac>1)?0:siz)) break;
	i = decmod(i,HISTMAX);
	n--;
	}
return(0);
}

/*************************************************************
*  backspace(n) distructive backspace, updates 'column'
*/
backspace(n)
int n;
{

for (;n>0 && column > 0;n--) {
	printf("\b \b");
	column--;
	}
}

/*************************************************************
*  right(p,n) move the cursor n places right, updates global 'column'
*/
right(p,n)
char *p;
int n;
{
int len;

len = strlen(p);
for (;n>0 && column < len;n--) {
	putchar(p[column]);
	column++;
	}
}

/*************************************************************
*  left(n) move the cursor n places left, updating global 'column'
*/
left(n)
int n;
{

for (;n>0 && column > 0;n--) {
	putchar('\b');
	column--;
	}
}

/*************************************************************
*  char *gethistn(n) return ptr to hist entry n
*/
char *gethistn(n)
int n;
{
int m,d;

if (n < (histno - HISTMAX) || n > histno) return(0);
m = histi;
d = histno - n;
for (;d>0;d--) m = decmod(m,HISTMAX);
return(hist[m]);
}

/*************************************************************
*  char *gethists(p) use strbequ to return ptr to matching hist string
*/
char *gethists(p)
char *p;
{
int i;

i = decmod(histi,HISTMAX);
while (i != decmod(histo,HISTMAX)) {
	if (hist[i] == 0) break;
	if (strbequ(hist[i],p)) return(hist[i]);
	i = decmod(i,HISTMAX);
	}
return(0);
}

/*************************************************************
*  addhist(p) add command line to history
*/
addhist(p)
char *p;
{
int i;

if (strempty(p)) return;

i = decmod(histi,HISTMAX);
if (hist[i] != 0) {
	if (strequ(p,hist[i])) return;
	}

if (incmod(histi,HISTMAX) == histo) {
	if (hist[histo] != 0) free(hist[histo]);
	hist[histo] = 0;
	histo = incmod(histo,HISTMAX);
	}
hist[histi] = malloc(strlen(p)+1);
if (hist[histi] == 0) {
	printf("addhist: out of memory\n");
	return;
	}
strcpy(hist[histi],p);
histi = incmod(histi,HISTMAX);
histno++;
}

/*************************************************************
*  putstr(p) print string and update global 'column'
*/
putstr(p)
char *p;
{
int i;

for (i=0;p[i] != 0;i++) {
	putchar(p[i]);
	if (p[i] == '\n') column = 0;
	else column++;
	}
}

/*************************************************************
*  char *findbang(p)
*/
char *findbang(p)
char *p;
{
int quote;

for (;*p;p++) {
	if (*p == '\'' || *p == '"') {
		quote = *p;
		p++;
		while (*p && *p != quote) p++;
		}
	else if (*p == '!') return(p);
	}
return(0);
}

