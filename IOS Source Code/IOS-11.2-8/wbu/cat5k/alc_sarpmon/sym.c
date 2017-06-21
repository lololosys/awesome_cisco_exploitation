/* $Id: sym.c,v 1.1.4.1 1996/05/09 15:00:34 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/sym.c,v $
 *------------------------------------------------------------------
 * sym.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sym.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:34  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:17  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:12  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <termio.h>
#include <string.h>
#include <mips.h>
#include "pmon.h"

Sym *symahead,*symnhead;
Sym *nextsym(),*rmsym();

/*************************************************************
*  addsym(p)  
*	add a structure to the chain 
*/
addsym(p)
Sym *p;
{
Sym *q,*prev;
char *n1,*n2;

if (symahead == 0) { /* list was empty */
	p->anext = 0;
	p->nnext = 0;
	symnhead = p;
	symahead = p;
	return;
	}

/* insert into numerically ordered list */
q = symnhead;
prev = 0;
while (q != 0) {
	if (q->value > p->value) break;
	prev = q;
	q = q->nnext;
	}

if (prev == 0) { /* insert before first item */
	p->nnext = symnhead;
	symnhead = p;
	}
else {
	p->nnext = prev->nnext;
	prev->nnext = p;
	}

/* insert into alphabetically ordered list */
if (p->lname) n1 = p->lname;
else n1 = p->name;
q = symahead;
prev = 0;
while (q != 0) {
	if (q->lname) n2 = q->lname;
	else n2 = q->name;
	if (strcmp(n2,n1) > 0) break;
	prev = q;
	q = q->anext;
	}

if (prev == 0) { /* insert before first item */
	p->anext = symahead;
	symahead = p;
	}
else {
	p->anext = prev->anext;
	prev->anext = p;
	}
}

/** Sym *getsym(value) returns struct that matches value */
Sym *getsym(value)
unsigned long value;
{
Sym *p;

p = symnhead;
while (p != 0) {
	if (p->value == value) break;
	p = p->nnext;
	}
return(p);
}

/** Sym *findsym(value) returns struct that is <= value */
Sym *findsym(value)
unsigned int value;
{
Sym *p,*prev;

p = symnhead;
prev = 0;
while (p != 0) {
	if (p->value > value) break;
	prev = p;
	p = p->nnext;
	}
return(prev);
}

#define LFLAG 0x01
#define VFLAG 0x02
#define NFLAG 0x04
#define TFLAG 0x08
#define DFLAG 0x10
#define BFLAG 0x20
#define AFLAG 0x40

Optdesc ls_opts[] = {
	{"-l","long listing"},
	{"-n","numeric order"},
	{"-v","compute value"},
	{"-a","lookup address"},
	{0}};

/*************************************************************
*  do_ls(ac,av)
*	the 'ls' command 
*/
do_ls(ac,av)
int ac;
char *av[];
{
int i,j,flags,pats;

flags = pats = 0;
for (i=1;i<ac;i++) {
	if (av[i][0] == '-') {
		j = 1;
		while (av[i][j] != 0) {
			if (av[i][j] == 'l') flags |= LFLAG;
			else if (av[i][j] == 'v') flags |= VFLAG;
			else if (av[i][j] == 'n') flags |= NFLAG;
			else if (av[i][j] == 'a') flags |= AFLAG;
			/* t=text d=data b=bss
			else if (av[i][j] == 't') flags |= TFLAG;
			else if (av[i][j] == 'b') flags |= BFLAG;
			else if (av[i][j] == 'd') flags |= DFLAG;
			*/
			else printf("bad option [%c]\n",av[i][j]);
			j++;
			}
		}
	else pats++;
	}

ioctl(STDIN,CBREAK,NULL);

if (flags&(VFLAG|AFLAG)) {
	if (pats == 0) printf("no values specified\n");
	else {
		for (i=1;i<ac;i++) {
			if (av[i][0] != '-') doval(av[i],flags);
			}
		}
	return;
	}

if (pats == 0) dopat("*",flags);
else {
	for (i=1;i<ac;i++) {
		if (av[i][0] != '-') dopat(av[i],flags);
		}
	}
}

/** doval(val,flags) prints name from value */
doval(val,flags)
char *val;
int flags;
{
unsigned int value;
Sym *p;
char *nam,tmp[LINESZ];

if (!get_rsa(&value,val)) return;

if (flags&VFLAG) printf("0x%08x = 0t%d = 0o%o\n",value,value,value);
else { /* must be AFLAG */
	if (!adr2symoff(tmp,value,0)) printf("%08x: not found\n",value);
	else printf("%08x = %s\n",value,tmp);
	}
}

/** adr2symoff(dst,value,width) convert addr to symbol+offset */
adr2symoff(dst,value,width)
char *dst;
unsigned int value;
int width;
{
Sym *p;
char *nam,tmp[16];
long offset;

dst[0] = 0;
p = findsym(value);
if (p == 0) return(0);
else {
	if (p->lname) nam = p->lname;
	else nam = p->name;

	if (width == 0) sprintf(dst,"%s",nam);
	else sprintf(dst,"%*.*s",width,width,nam);

	offset = value-(p->value);
	if (offset == 0) strcat(dst,"       ");
	else {
		sprintf(tmp,"+0x%-4x",offset);
		strcat(dst,tmp);
		}
	}
return(1);
}

/** dopat(pat,flags) prints all syms that match pat */
dopat(pat,flags)
char *pat;
int flags;
{
Sym *q;
char sym[LINESZ];
int l,siz,col,maxlen,len,width;

if (!atob(&siz,getenv("moresz"),10)) {
	printf("moresz has bad value\n");
	return;
	}

if (flags&LFLAG) {
	l = siz;
	q = 0;
	loop {
		q = nextsym(prnbuf,q,pat,flags);
		if (more(prnbuf,&l,siz)) break;
		if (q == 0) break;
		}
	return;
	}

/* isn't a long listing, so need to columnize */
q = 0;
maxlen = 0;
loop {
	q = nextsym(sym,q,pat,flags);
	len = strlen(sym);
	if (len > maxlen) maxlen = len;
	if (q == 0) break;
	}
width = (maxlen+8)&~7;
l = siz;
q = 0;
loop {
	prnbuf[0] = 0;
	col = 0;
	loop {
		q = nextsym(sym,q,pat,flags);
		col += width;
		if (col+width <= 79) str_fmt(sym,width,FMT_LJUST);
		strcat(prnbuf,sym);
		if (col+width > 79 || q == 0) break;
		}
	if (more(prnbuf,&l,siz)) break;
	if (q == 0) break;
	}
}

/** do_sym(ac,av) sym command */
do_sym(ac,av)
int ac;
char *av[];
{
Sym *p;
unsigned long v;

if (!get_rsa(&v,av[2])) return;
if (!newsym(av[1],v)) printf("out of memory\n");
}

/** newsym(name,value)  allocates struct, & adds it to list */
newsym(name,value)
char *name;
unsigned long value;
{
Sym *p;
int len;

if (p = rmsym(name)) { /* name exists */
	p->value = value;
	addsym(p);
	return(1);
	}

p = (Sym *)malloc(sizeof(Sym));
if (p == 0) return(0);
p->lname = 0;
len = strlen(name);
if (len > MAXSYM-1) {
	p->lname = (char *) malloc(len+1);
	if (p->lname == 0) return(0);
	strcpy(p->lname,name);
	}
else strcpy(p->name,name); 
p->value = value;
addsym(p);
return(1);
}

/** Sym *rmsym(name) remove matching symbol, return NULL is not found */
Sym *rmsym(name)
char *name;
{
char *nam;
Sym *p,*prev;

/* remove from alphbetically ordered list */
p = symahead;
prev = 0;
while (p != 0) {
	if (p->lname) nam = p->lname;
	else nam = p->name;
	if (strequ(nam,name)) break;
	prev = p;
	p = p->anext;
	}
if (p == 0) return(0);
/* found, now remove it */
if (prev == 0)  symahead = p->anext; /* was the first item */
else prev->anext = p->anext;

/* remove from numerically ordered list */
p = symnhead;
prev = 0;
while (p != 0) {
	if (p->lname) nam = p->lname;
	else nam = p->name;
	if (strequ(nam,name)) break;
	prev = p;
	p = p->nnext;
	}
if (p == 0) return(0);
/* found, now remove it */
if (prev == 0)  symnhead = p->nnext; /* was the first item */
else prev->nnext = p->nnext;
return(p);
}

/** sym2adr(vp,name) finds value from name */
sym2adr(vp,name)
unsigned long *vp;
char *name;
{
Sym *p;
char *nam;

*vp = 0;
p = symahead;
while (p != 0) {
	if (p->lname) nam = p->lname;
	else nam = p->name;
	if (strequ(nam,name)) {
		*vp = p->value;
		return(1);
		}
	p = p->anext;
	}
return(0); /* return 0 if we can't find the symbol */
}

/** adr2sym(name,v) finds name from value */
char *adr2sym(name,v)
char *name;
unsigned long v;
{
Sym *p;

if (name) name[0] = 0;
p = getsym(v);
if (p == 0) return(0);
if (name) {
	if (p->lname) strcpy(name,p->lname);
	else strcpy(name,p->name);
	return(name);
	}
else {
	if (p->lname) return(p->lname);
	else return(p->name);
	}
}

/** Sym *nextsym(p,q,pat,flags) gets next matching symbol */
Sym *nextsym(p,q,pat,flags)
char *p,*pat;
Sym *q;
int flags;
{
char *nam;

if (q == 0) {
	if (flags&NFLAG) q = symnhead;
	else q = symahead;
	}

*p = 0;
loop {
	if (q == 0) return(q);
	if (q->lname) nam = q->lname;
	else nam = q->name;
	if (strpat(nam,pat)) {
		if (flags&LFLAG) sprintf(p,"%08x ",q->value);
		strcat(p,nam);
		if (flags&NFLAG) return(q->nnext);
		else return(q->anext);
		}
	if (flags&NFLAG) q = q->nnext;
	else q = q->anext;
	}
}

/** clrsyms() clear entire symbol table */
clrsyms()
{
Sym *p;

p = symahead;
while (p != 0) {
	symahead = p->anext;
	if (p->lname) free(p->lname);
	free(p);
	p = symahead;
	}
symahead = symnhead = 0;
newsym("Pmon",0x9fc00000);
}
