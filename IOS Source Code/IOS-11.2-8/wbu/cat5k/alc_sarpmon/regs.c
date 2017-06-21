/* $Id: regs.c,v 1.1.4.1 1996/05/09 15:00:29 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/regs.c,v $
 *------------------------------------------------------------------
 * regs.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: regs.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:29  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:12  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:07  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <mips.h>
#include <string.h>
#include <termio.h>
#include "pmon.h"

#define loop for (;;)
#define BADREG "error: arg2 [%s] bad register name\n"
#define BADFIELD "error: arg3: [%s] bad field name\n"
#define BADBASE "error: arg%d: [%s] base %d value expected\n"
#define BADVALUE "error: arg%d: [%s] max value %d expected\n"
#define BADSVALUE "error: arg4: [%s] bad symbolic value\n"
#define REG_RO "Register is readonly\n"

#ifdef MIPSEB
#define MSHALF 0
#define LSHALF 1
#else
#define MSHALF 1
#define LSHALF 0
#endif

char *btoa();
extern char *regs_sw[];
extern RegList reglist[];
extern char *excodes[];

Optdesc r_opts[] = {
	{"*","display all registers"},
	{"t*","display all registers starting with t"},
	{"reg value","set specified register"},
	{"reg field value","set specified field"},
	{"f*","display all fp registers"},
	{0}};

/*************************************************************
*  registers(ac,av) 
*	the 'r' (display registers) command
*/
registers(ac,av)
int ac;
char *av[];
{
int i,j,len,lmargin[2],flag,wild,l,siz;
unsigned int n,w;
RegSpec *p;
char buf[80],tmp1[20],tmp2[20];
word *pr,val;

if (ac > 1 && (av[1][0] == 'f' || av[1][0] == 'F')) {
	fregs(ac,av);
	return(0);
	}

prnbuf[0] = 0;
if (!atob(&siz,getenv("moresz"),10)) {
	printf("%s: bad moresz value\n",getenv("moresz"));
	return(1);
	}
l = siz;
ioctl(STDIN,CBREAK,NULL);

lmargin[0] = lmargin[1] = 0;
for (j=0;reglist[j].reg != 0;j++) {
	len = strlen(reglist[j].name);
	if (reglist[j].spec) {
		if (len > lmargin[1]) lmargin[1] = len;
		}
	else {
		if (len > lmargin[0]) lmargin[0] = len;
		}
	}

switch (ac) {
	case 1 : /* displ GP registers */
			disp_Gprs(1);
		break;
	case 2 : /* displ selected register(s) */
		if (strequ(av[1],"*")) { /* all regs */
			disp_Gprs(0);
			l -= 4;
			for (j=0;reglist[j].reg != 0;j++) {
				if (reglist[j].flags&F_GPR) continue;
				if (!in_machtype(reglist[j].flags)) continue;
				if (dispReg(lmargin,j,&l,siz)) break;
				}
			if (strlen(prnbuf)) more(prnbuf,&l,siz);
			}
		else { /* display reg(s) by name */
			flag = 0;
			if (strchr(av[1],'*') || strchr(av[1],'?')) wild = 1;
			else wild = 0;
			strtoupper(av[1]);
			for (j=0;reglist[j].reg != 0;j++) {
				if (!in_machtype(reglist[j].flags)) continue;
				strcpy(tmp1,reglist[j].name);
				if (wild) *tmp2 = 0;
				else strcpy(tmp2,reglist[j].aname);
				strtoupper(tmp1);
				strtoupper(tmp2);
				if (strpat(tmp1,av[1]) || strpat(tmp2,av[1])) {
					if (dispReg(lmargin,j,&l,siz)) break;
					flag = 1;
					}
				}
			if (strlen(prnbuf)) more(prnbuf,&l,siz);
			if (flag == 0) printf(BADREG,av[1]);
			}
		break;
	case 3 : /* set entire register */
		if (!get_rsa(&n,av[2])) break;
		flag = 0;
		if (strchr(av[1],'*') || strchr(av[1],'?')) wild = 1;
		else wild = 0;
		strtoupper(av[1]);
		for (j=0;reglist[j].reg != 0;j++) {
			if (!in_machtype(reglist[j].flags)) continue;
			strcpy(tmp1,reglist[j].name);
			if (wild) *tmp2 = 0;
			else strcpy(tmp2,reglist[j].aname);
			strtoupper(tmp1);
			strtoupper(tmp2);
			if (strpat(tmp1,av[1]) || strpat(tmp2,av[1])) {
				if (reglist[j].flags&F_RO) printf(REG_RO);
				else if (reglist[j].flags&F_CF)
					((Func *)reglist[j].reg)(1,
						reglist[j].regnum,n);
				else *reglist[j].reg = n;
				flag = 1;
				}
			}
		if (flag == 0) printf(BADREG,av[1]);
		break;
	case 4 : /* set register field */
		for (j=0;reglist[j].reg != 0;j++) {
			if (!in_machtype(reglist[j].flags)) continue;
			if (striequ(av[1],reglist[j].name)) break;
			if (striequ(av[1],reglist[j].aname)) break;
			}
		if (reglist[j].reg == 0) {
			printf(BADREG,av[1]);
			break;
			}
		/* found the reg, now find the field */
		p = reglist[j].spec;
		for (i=0;p[i].name != 0;i++) {
			if (striequ(av[2],p[i].name)) break;
			}
		if (p[i].name == 0) {
			printf(BADFIELD,av[2]);
			break;
			}
		/* found the field, now find the value */	
		if (p[i].ro) {
			printf(REG_RO);
			break;
			}
		/* first check to see if should be symbolic */
		if (p[i].base == 0) {
			for (n=0;p[i].values[n] != 0;n++) {
				if (striequ(av[3],p[i].values[n])) break;
				}
			if (p[i].values[n] == 0) {
				printf(BADSVALUE,av[3]);
				break;
				}
			}
		else {
			if (!atob(&n,av[3],p[i].base)) {
				printf(BADBASE,4,av[3],p[i].base);
				break;
				}
			if (n >= (1<<p[i].size)) {
				printf(BADVALUE,4,av[3],(1<<p[i].size)-1);
				break;
				}
			}
		if (reglist[j].flags&F_CF) {
			w = ((Func *)reglist[j].reg)(0,reglist[j].regnum);
			setfield(&w,p[i].size,p[i].lsb,n);
			((Func *)reglist[j].reg)(1,reglist[j].regnum,w);
			}
		else setfield(reglist[j].reg,p[i].size,p[i].lsb,n);
#ifdef SETBS
		if (reglist[j].reg == (word *)CFGREG 
			&& strequ(p[i].name,"IBS")) setibs(n);
		if (reglist[j].reg == (word *)CFGREG 
			&& strequ(p[i].name,"DBS")) setdbs(n);
#endif
		break;
	}
return(0);
}

/*************************************************************
*  dispReg(lmargin,j,ln,siz)
*/
dispReg(lmargin,j,ln,siz)
int lmargin[],j,*ln,siz;
{
char tmp[80];
int value,state;

if (reglist[j].flags&F_WO) {
	if (lmargin[0]+strlen(prnbuf)+10 > 79) 
		if (more(prnbuf,ln,siz)) return(1);
	sprintf(tmp,"%*s=WRTEONLY ",lmargin[0],reglist[j].name);
	strcat(prnbuf,tmp);
	return(0);
	}

if (reglist[j].flags&F_CF)
	value = ((Func *)reglist[j].reg)(0,reglist[j].regnum);
else value = *reglist[j].reg;

if (reglist[j].spec) {
	if (strlen(prnbuf)) 
		if (more(prnbuf,ln,siz)) return(1);
	state = 0;
	while (disp_reg(prnbuf,&reglist[j],lmargin[1],value,state++)) {
		if (more(prnbuf,ln,siz)) return(1);
		}
	}
else if (reglist[j].flags&F_GPR) {
	if (lmargin[0]+strlen(prnbuf)+10 > 79) 
		if (more(prnbuf,ln,siz)) return(1);
	sprintf(tmp,"%*s=%s=%08x ",lmargin[0]-3,reglist[j].aname,
					reglist[j].name,value);
	strcat(prnbuf,tmp);
	}
else {
	if (lmargin[0]+strlen(prnbuf)+10 > 79) 
		if (more(prnbuf,ln,siz)) return(1);
	sprintf(tmp,"%*s=%08x ",lmargin[0],reglist[j].name,value);
	strcat(prnbuf,tmp);
	}
return(0);
}

/*************************************************************
*  disp_Gprs(n)
*/
disp_Gprs(n)
int n;
{
int i;

if (n==1) {
	printf("    ");
	for (i=0;i<8;i++) printf(" %~8s",regs_sw[i]);
	printf("\n");
	}
printf(" $0-");
for(i = 0; i < 8; i++) printf(" %08x",Gpr[i]);

if (n==1) {
	printf("\n    ");
	for (i=8;i<16;i++) printf(" %~8s",regs_sw[i]);
	}
printf("\n $8-");
for(i = 8; i < 16; i++) printf(" %08x",Gpr[i]);

if (n==1) {
	printf("\n    ");
	for (i=16;i<24;i++) printf(" %~8s",regs_sw[i]);
	}
printf("\n$16-");
for(i = 16; i < 24; i++) printf(" %08x",Gpr[i]);

if (n==1) {
	printf("\n    ");
	for (i=24;i<32;i++) printf(" %~8s",regs_sw[i]);
	}
printf("\n$24-");
for(i = 24; i < 32; i++) printf(" %08x",Gpr[i]);
printf("\n");
}

/*************************************************************
*  disp_reg(pb,q,lmargin,value,state)
*/
disp_reg(pb,q,lmargin,value,state)
char *pb;
RegList *q;
int lmargin,state;
uword value;
{
int i,val,len,width,col,n;
char buf[80],tmp[80];
RegSpec *p;
static int posn;

if (lmargin == 0) lmargin = strlen(q->name);

if (q->spec == 0) {
	printf("disp_reg: spec==0\n");
	return(1);
	}

if (state == 0) posn = 0;
p = q->spec;
if (p[posn].size == 0) return(0);

*pb = 0;
if ((state&1) == 0) { /* print field names */
	sprintf(pb,"%*s: ",lmargin,q->name);
	col = lmargin+2;
	for (i=posn;p[i].size != 0;i++) {
		if (p[i].name == 0) continue;
		width = fieldsz(&p[i]);
		if (col+width+1 > 76) break;
		sprintf(buf,"%~*s ",width,p[i].name);
		strcat(pb,buf);
		col += width+1;
		}
	}
else { /* print field values */
	for (n=lmargin+2;n>0;n--) strcat(pb," "); /* +2 for ": " */
	col = lmargin+2;
	for (i=posn;p[i].size != 0;i++) {
		if (p[i].name) len = strlen(p[i].name);
		else len = 0;
		width = fieldsz(&p[i]);
		if (col+width+1 > 76) break;
		val = getfield(value,p[i].size,p[i].lsb);
		if (p[i].base == 0) strcpy(buf,p[i].values[val]);
		else {
			btoa(buf,val,p[i].base);
			if (p[i].base != 10) str_fmt(buf,p[i].size,FMT_RJUST0);
			}
		sprintf(tmp,"%~*s ",width,buf);
		strcat(pb,tmp);
		col += width+1;
		}
	posn = i;
	}
return(1);
}

/*************************************************************
*  fieldsz(p)
*/
fieldsz(p)
RegSpec *p;
{
int width,nmsz,valsz,len;
char **q;

nmsz = strlen(p->name);
switch (p->base) {
	case 0 :
		valsz = 0;
		for (q=p->values;*q;q++) {
			len = strlen(*q);
			if (len > valsz) valsz = len;
			}
		break;
	case 2 :
		valsz = p->size;
		break;
	case 10 :
		if (p->size > 3) valsz = 2;
		else valsz = 1;
		break;
	case 16 :
		valsz = 1;
		break;
	}
if (nmsz > valsz) width = nmsz;
else width = valsz;
return(width);
}

/*************************************************************
*  setfield(dstp,size,lsb,value)
*/
setfield(dstp,size,lsb,value)
unsigned int *dstp;
int size,lsb,value;
{
unsigned int msk,v;

msk = ((1<<size)-1)<<lsb;
v = *dstp;
v &= ~msk;
v |= value<<lsb;
*dstp =v;
}

/*************************************************************
*  getreg(vp,p)
*/
getreg(vp,p)
unsigned long *vp;
char *p;
{
unsigned long adr;

if (!getregadr(&adr,p)) return(0);
*vp = load_word(adr);
return(1);
}

/*************************************************************
*  getregadr(vp,p)
*/
getregadr(vp,p)
unsigned long *vp;
char *p;
{
int j;

if (isdigit(*p)) {
	if(!atob(&j,p,10) || j > 31) return(0);
	*vp = (unsigned long) &Gpr[j];
	}
else if ((j = regno(p)) != -1) *vp = (unsigned long) &Gpr[j];
else {
	for (j=0;reglist[j].reg != 0;j++) {
		if (striequ(p,reglist[j].name)) break;
		if (striequ(p,reglist[j].aname)) break;
		}
	if (!reglist[j].reg) return(0);
	*vp = (unsigned long) reglist[j].reg;
	}
return(1);
}

/*************************************************************
*  regno(p)
*/
regno(p)
char *p;
{
int i;

for (i=0;i<32;i++) {
	if (striequ(p,regs_sw[i])) return(i);
	}
return(-1);
}

/*************************************************************
*  char *getexcname(n)
*/
char *getexcname(n)
int n;
{
return(excodes[n>>2]);
}

/*************************************************************
*  fregs(ac,av)
*/
fregs(ac,av)
int ac;
char *av[];
{
int i,v,temp[2];
#ifdef FLOATINGPT
double *dp;
float *sp;

sp = (float *)&v;
dp = (double *)temp;
if (ac == 2) {
	if (av[1][1] == '*') {
		for (i=0;i<32;i+=2) {
			temp[MSHALF] = Fpr[i+1];
			temp[LSHALF] = Fpr[i];
			v = Fpr[i];
			printf("$f%-2d = %08x %08x sp=%e dp=%e\n",
				i,v,Fpr[i+1],*sp,*dp);
			}
		disp_as_reg(Fcr,"C1_CSR");
		disp_as_reg(Fid,"C1_FRID");
		}
	else {
		if (atob(&i,&av[1][1],10)) {
			i &= ~1; /* clear LSB */
			temp[MSHALF] = Fpr[i+1];
			temp[LSHALF] = Fpr[i];
			v = Fpr[i];
			printf("$f%-2d = %08x %08x sp=%e dp=%e\n",
				i,v,Fpr[i+1],*sp,*dp);
			}
		else printf("bad register number\n");
		}
	}
else if (ac == 3) {
	get_rsa(&v,av[2]);
	if (atob(&i,&av[1][1],10)) Fpr[i] = v;
	else printf("bad register number\n");
	}
else printf("Bad arg count\n");
#endif
}

/*************************************************************
*  disp_as_reg(value,p)
* 	display reg by name
*/
disp_as_reg(value,p)
uword value;
char *p;
{
int j,state;


prnbuf[0] = 0;
for (j=0;reglist[j].reg != 0;j++) {
	if (striequ(reglist[j].name,p) || striequ(reglist[j].aname,p)) {
		if (reglist[j].spec) {
			state = 0;
			while (disp_reg(prnbuf,&reglist[j],0,value,state++)) 
				printf("%s\n",prnbuf);
			}
		else {
			if (reglist[j].flags&F_ANAME) 
				printf("$%s-%s-%08x\n",reglist[j].aname,
						reglist[j].name,value);
			else printf("%s: %08x\n",reglist[j].name,value);
			}
		return(1);
		}
	}
return(0);
}

#ifdef LR33020

#ifdef SABLE
long c2c[32],c2d[32];
cfc2(reg) {return(c2c[reg]);}
ctc2(reg,value) {c2c[reg] = value;}
mfc2(reg) {return(c2d[reg]);}
mtc2(reg,value) {c2d[reg] = value;}
cp2init() {}
#endif /* SABLE */

/*************************************************************
*  mXc2(mode,reg,value)
*/
mXc2(mode,reg,value)
int mode,reg,value;
{

if (mode == 0) /* read */ return(mfc2(reg));
else mtc2(reg,value);
}

/*************************************************************
*  cXc2(mode,reg,value)
*/
cXc2(mode,reg,value)
int mode,reg,value;
{

if (mode == 0) /* read */ return(cfc2(reg));
else ctc2(reg,value);
}
#endif /* LR33020 */

/*************************************************************
*  in_machtype(n)
*	checks to see if a particular register is in the current
*	machine type.
*/
in_machtype(n)
int n;
{

if (!n) return(1);
switch (machtype) {
	case 33000 : if (n&F_00) return(1); break;
	case 33020 : if (n&F_020) return(1); break;
	case 33050 : if (n&F_50) return(1); break;
	case 33120 : if (n&F_120) return(1); break;
	case 33300 : 
	case 33310 : if (n&F_333) return(1); break;
	case 99000 : if (n&(F_333|F_ATM)) return(1); break;
	}
return(0);
}

