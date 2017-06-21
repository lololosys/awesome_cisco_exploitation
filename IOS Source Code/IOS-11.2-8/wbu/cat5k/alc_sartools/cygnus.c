/* $Id: cygnus.c,v 1.1.4.3 1996/06/01 00:04:51 venkat Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sartools/cygnus.c,v $
 *------------------------------------------------------------------
 * cygnus.c -- LSI Logic ATMizer pmcc source.
 *
 * 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cygnus.c,v $
 * Revision 1.1.4.3  1996/06/01  00:04:51  venkat
 * CSCdi59228:  Pull compiler usage out from pmcc tool into makefile.
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/31  01:03:22  venkat
 * CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
 * Branch: California_branch
 * Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
 * tool
 * chain
 *
 * Revision 1.1.4.1  1996/05/09  15:00:39  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  02:17:49  venkat
 * Branch: LE_Cal_V112_0_2_branch
 * Fixed Warnings while compiling the sartools (pmcc)
 *
 * Revision 1.1.28.1  1996/04/08  01:41:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/06  01:02:21  sakumar
 * Delete all files created in /tmp while building SAR firmware images.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:12:48  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  04:06:06  vkrishna
 * Changes to support SAR firmware build as part of Cat5K ATM Module
 * IOS build procedure.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:23  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:29  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <defines.h>
/*************************************************************
pmcc -- PMON compiler interface

This program provides a standardized interface to the EPI toolset.
When linking it places the object file in OFILE, the downloadable records
in OFILE.rec, and the link map in OFILE.MAP.

This program assumes that the environment variable LSIPKG has been set.
On Little Endian Hosts, the environment variable LEHOST should also be set.
 
In addition to the usual cc options, this script supports:
	-fast		- generate fast-format download records (default)
	-srec		- generate S-record download records
	-syms		- omit symbols from the download file
	-double		- use LSI-supplied double-precision math library
	-chksum		- don't generate chksum data in fast records
	-crt0		- don't include the standard crt0
	-prom		- append data to text for building proms
*************************************************************/
/* #define Exit(x)		printf("status=%d",x),exit(((x)>127)?1:(x)) */
#define strequ(x,y)	((strcmp(x,y)==0)?1:0)
#define strNcpy(x,y,z)	strncpy(x,y,z),(x)[z]=0
/* #define addarg(d,s) 	strcat(d," "), strcat(d,s) /* */
/* #define system(x)	((printf("system: %s\n",x)==0)?1:0) /* */

#define FLGSZ 1000
#define CFLGSZ 1000
#define AFLGSZ 1000
#define LFLGSZ 1000
#define PFLGSZ 1000
#define OFILSZ 500
#define RFLGSZ 100
#define LLISTSZ 500
#define LNMAX 400

/* set various defaults */
int vflag = 1;	/* verbose flag */
char *prom = "";
int fast = 1;
/*char *syms = "-s";*/
char *syms = "-oformat symbolsrec";		/* Ratinder P. S. Ahuja	*/
char *chksum = "";
char *ofile = "a";
char LIBM[100] = "/usr/lib/cmplrs/cc";
char tmp[400],tmp2[400];
char *ENDIAN,*Gnum,*LSIPMCC;
char *COMP_HOST_ROOT;
char flags[FLGSZ] ;
char cflags[CFLGSZ];
char asflags[AFLGSZ];
char ldflags[LFLGSZ];
char ppflags[PFLGSZ];
char rflags[RFLGSZ];
char llist[LLISTSZ];
char ilist[100];
char *LSIPKG,*CYGROOT;
char LIBC[100];
char objfile[100];
char tstart[10];
char *dstart;
char *files[100];
char CC[300];
int nfiles;
char ofiles[OFILSZ];
char stoppoint;
char crt0[100];
char tmppath[200],cpppath[200];

char *getenv(),*extent();
char *getRoot();

struct utsname platform;
char *osdir;

#ifndef PMCC_DRIVER

main(argc, argv)
int argc;
char *argv[];
{
	char *input_file;

	if (argc < 3) {
		printf("Usage :: %s -h||-s input_file \n", argv[0]);
		printf("      -h      for fixhash\n");
		printf("      -s      for fixstruct\n");
		exit(1);
	}

	input_file = argv[2];
	if (strequ(argv[1], "-h"))
		fixhash(input_file);
	else if (strequ(argv[1], "-s"))
		fixstruct(input_file);
	else {
		printf("%s :: Unknown option %s\n", argv[0], argv[1]);
		printf("Usage :: %s -h||-s input_file \n", argv[0]);
		printf("      -h      for fixhash\n");
		printf("      -s      for fixstruct\n");
		exit(1);
	}
	exit(0);
}
		

/*************************************************************
*  fixstruct(in,out)
*/
fixstruct(in)
char *in;
{
FILE *ifp;
char *p;
char plabel[100],pval[100];
char label[100],val[100],direct[100];
int structflg;

ifp = fopen(in,"r");

structflg = 0;
while (fgets(tmp,LNMAX,ifp) != NULL) {
	/* These tools don't support .struct, so simulate it */
	strcpy(tmp2,tmp);
	tmp2[strlen(tmp2)-1] = 0; /* remove terminating \n */
	p = strtok(tmp2," \t");
	if (p == NULL) ;
	else if (structflg) {
		if (strequ(p,".data")||strequ(p,".text")) structflg = 0;
		else {
			tmp[strlen(tmp)-1] = 0; /* remove terminating \n */
			/* terminate line at /* */
			if (p=(char *)strstr(tmp,"/*")) *p = 0;
			/* replace ':' with ' '*/
			if (p=strchr(tmp,':')) *p = ' ';
			if (!(p=strtok(tmp," \t"))) continue;
			if (*p != '.') {
				strcpy(label,p);
				p = strtok(0," \t");
				}
			if (!p) p = "";
			strcpy(direct,p);
			strcpy(val,p+(strlen(p)+1));
			if (strlen(label)) 
			    sprintf(tmp,"#define %s (%s+(%s))\n",label,
							plabel,pval);
			strcpy(plabel,label);
			if (strequ(direct,".space")) strcpy(pval,val);
			else if (strequ(direct,".word")) strcpy(pval,"4");
			else if (strequ(direct,".half")) strcpy(pval,"2");
			else if (strequ(direct,".byte")) strcpy(pval,"1");
			else strcpy(pval,"0");
			strcpy(label,"");
			}
		}
	else if (strequ(p,".struct")) {
		structflg = 1;
		strcpy(tmp,"\t/* struct 0 */\n");
		strcpy(plabel,"0");
		strcpy(pval,"0");
		}
	puts(tmp);
	}
fclose(ifp);
}

/*************************************************************
*  fixhash(in,out)
*/
fixhash(in)
char *in;
{
FILE *ifp;
char *p;

ifp = fopen(in,"r");

while (fgets(tmp,LNMAX,ifp) != NULL) {
	if (p=strchr(tmp+1,'#'))  {
		p[0] = '\n';
		p[1] = 0;
		}
	printf("%s", tmp);
	}
fclose(ifp);
}

#endif /* #ifndef PMCC_DRIVER */

#ifdef PMCC_DRIVER
/*************************************************************
*  main(argc,argv)
*/
main(argc,argv)
int argc;
char *argv[];
{
int i,status,SZ;
char *e,*p,h[20];
FILE *cfp,*sfp;
char tmp2[100];

SZ = 0;

if (! (LSIPKG=getenv("LSIPKG"))) {
 	fprintf(stderr,"pmcc: LSIPKG not set\n");
 	Exit(1);
 	}
if (! (CYGROOT=getenv("CYGROOT"))) {
	fprintf(stderr,"pmcc: CYGROOT not set\n");
	Exit(1);
	}

/* for backward compatability we set LSIPMCC */
if (! (LSIPMCC=getenv("LSIPMCC"))) LSIPMCC = "";

memset(&platform, '\0', sizeof(platform));
uname(&platform);

if (!strcmp (platform.machine, "sun4d")) {
    osdir = "/progressive-96q1/sparc-sun-solaris2";
    sprintf(cpppath,"%s%s/lib/gcc-lib/mips64-elf/2.7-96q1/", CYGROOT, osdir);
} else {
    osdir = "/progressive-96q1/sparc-sun-sunos4.1.2";
    sprintf(cpppath,"%s%s/lib/gcc-lib/sparc-sun-sunos4.1.2/2.7-96q1/", CYGROOT, osdir);
}


#ifdef SREC
fast = 0;
#endif

#ifdef CLIENTPC
sprintf(tmp,"%08x",CLIENTPC);
strcpy(tstart,tmp);
#else
strcpy(tstart,"80020000");
#endif

strcpy(tmppath,"/tmp/PMXXXXXX");
mktemp(&tmppath[5]);

ENDIAN = "b";
Gnum = "o";

/*
**
**	Ratinder P. S. Ahuja 17 Aug 94
**	Modified by CC and cpppath
**	Sice we are using mips64 need to use flags -mips1 -mfp32 to 
**      get instructions limited to the MIPS-I instruction set rather 
**	than the default MIPS-III instruction set
**
*/

sprintf(CC, "%s%s/bin/mips64-elf-gcc -nostdinc -mcpu=r3000 -mips1 -mfp32", CYGROOT, osdir); 
 

sprintf(ilist,"-I%s/include",LSIPKG);
sprintf(llist,"-L%s/lib/%%s%%s",LSIPKG);


sprintf(flags,"-DPMCC -DCYGNUS %s",LSIPMCC);

/*
# This while loop does several jobs:
# 	1. strips off any pmcc defined options
# 	2. some options, if specified, must preceed path specs
# 	   so extract them from here, and add them to flags.
# 	3. find the output filename, if specified
*/

for (i=1;i < argc;i++) {
	if (strequ(argv[i],"-fast")) fast = 1;
	else if (!strncmp(argv[i],"-mips",5)) addarg(flags,argv[i],FLGSZ);
	else if (strequ(argv[i],"-prom")) {
		prom = "-p";
		fast = 0;
		}
	else if (strequ(argv[i],"-crt0")) strcpy(crt0," ");
	else if (strequ(argv[i],"-T")) {
		i++;
		strcpy(tstart,argv[i]);
		}
	else if (strequ(argv[i],"-D")) {
		i++;
		dstart = argv[i];
		}
	else if (!strncmp(argv[i],"-D",2)) addarg(ppflags,argv[i],PFLGSZ);
	else if (strequ(argv[i],"-target")) {
		/* just to keep Sun make happy */
		i++;
		}
	else if (strequ(argv[i],"-L")) llist[0] = 0;
	else if (!strncmp(argv[i],"-L",2)) addarg(llist,argv[i],LLISTSZ);
	else if (strequ(argv[i],"-G")) {
		i++;
		addarg(flags,"-G ",FLGSZ);
		strcat(flags,argv[i]);
		if (strequ(argv[i],"0")) Gnum = "g";
		else Gnum = "o";
		}
	else if (strequ(argv[i],"-I")) ilist[0] = 0;
	else if (!strncmp(argv[i],"-I",2)) addarg(ppflags,argv[i],PFLGSZ);
	else if (strequ(argv[i],"-g")) addarg(flags,argv[i],FLGSZ);
	else if (strequ(argv[i],"-c")) {
		stoppoint = 'c';
		addarg(cflags,argv[i],CFLGSZ);
		}
	else if (strequ(argv[i],"-S")) {
		stoppoint = 'S';
		addarg(cflags,argv[i],CFLGSZ);
		}
	else if (strequ(argv[i],"-v")) {
		vflag = 1;
		addarg(flags,argv[i],FLGSZ);
		}
	else if (strequ(argv[i],"-srec")) fast = 0;
	else if (strequ(argv[i],"-chksum")) chksum = "-c";
	else if (strequ(argv[i],"-syms")) syms = "";
	else if (!strncmp(argv[i],"-O",2)) addarg(cflags,argv[i],FLGSZ);
	else if (strequ(argv[i],"-float")) {
		addarg(flags,argv[i],FLGSZ);
		addarg(flags,"-DFLOAT",FLGSZ);
		SZ = 1; }
	else if (strequ(argv[i],"-double")) SZ = 1;
	else if (strequ(argv[i],"-EB")) ENDIAN = "b";
	else if (strequ(argv[i],"-EL")) ENDIAN = "l";
	else if (strequ(argv[i],"-o")) {
		i++;
		ofile = argv[i];
		}
	else if (argv[i][0] == '-') {
		fprintf(stderr,"%s: bad arg\n",argv[i]);
		}
	else files[nfiles++] = argv[i];
	}

if (strequ(ENDIAN,"b")) {
	addarg(asflags,"-EB",FLGSZ);
	addarg(cflags,"-EB",FLGSZ);
	addarg(ppflags,"-DMIPSEB",PFLGSZ);
	}
else {
	addarg(asflags,"-EL",FLGSZ);
	addarg(cflags,"-EL",FLGSZ);
	addarg(ppflags,"-DMIPSEL",PFLGSZ);
	}

strcpy(objfile,ofile);
if (strequ(ofile,"a")) strcpy(objfile,"a.out");

if (stoppoint==0) {
	addarg(cflags,"-c",CFLGSZ); /* never want the cc driver to call ld */
	addarg(ldflags,"-o",LFLGSZ);
	addarg(ldflags,objfile,LFLGSZ);
	}
else if (nfiles == 1) {
	e = extent(files[0]);
	getHead(h,files[0]);
	if (strequ(e,"c")) {
		addarg(cflags,"-o",CFLGSZ);
		if (!strequ(ofile,"a")) addarg(cflags,ofile,CFLGSZ);
		else {
			addarg(cflags,h,CFLGSZ);
			if (stoppoint == 'c') strcat(cflags,".o");
			else strcat(cflags,".s");
			}
		}
	else if (strequ(e,"s")) {
		addarg(asflags,"-o",AFLGSZ);
		if (!strequ(ofile,"a")) addarg(asflags,ofile,AFLGSZ);
		else {
			addarg(asflags,h,AFLGSZ);
			strcat(asflags,".o");
			}
		}
	}

sprintf(tmp,llist,ENDIAN,Gnum,ENDIAN,Gnum);
addarg(ldflags,tmp,LFLGSZ);
addarg(cflags,ilist,CFLGSZ);
addarg(ppflags,ilist,PFLGSZ);

addarg(asflags,"-DLANGUAGE_ASSEMBLY",AFLGSZ);
addarg(cflags,"-DLANGUAGE_C",CFLGSZ);

/* now invoke the compiler */
for (i=0;i<nfiles;i++) {
	e = extent(files[i]);
	getHead(h,files[i]);
	if (strequ(e,"s")) {
#if 0
		sprintf(tmp,"fixhash(%s,%s.S)",files[i],tmppath);
		if (vflag) fprintf(stderr,"%s\n",tmp);
		sprintf(tmp,"%s.S",tmppath);
		fixhash(files[i],tmp);
		getRoot(tmp2,files[i]);
#else
		sprintf(tmp,"fixhash(%s,%s.t1)",files[i],tmppath);
		if (vflag) fprintf(stderr,"%s\n",tmp);
		sprintf(tmp,"%s.t1",tmppath);
		fixhash(files[i],tmp);
		getRoot(tmp2,files[i]);
		sprintf(tmp,"%scpp %s %s -I%s %s %s.t1 %s.t2",
			cpppath,flags,ppflags,tmp2,"-DLANGUAGE_ASSEMBLY",
			tmppath,tmppath); 
		if (vflag) fprintf(stderr,"%s\n",tmp);
		if (status=system(tmp)) Exit(status);
		sprintf(tmp,"%s.t2",tmppath);
		sprintf(tmp2,"%s.S",tmppath);
		if (vflag) fprintf(stderr,"fixstruct(%s,%s)\n",tmp,tmp2);
		fixstruct(tmp,tmp2);
#endif
		sprintf(tmp,"%s -c %s %s -I%s %s %s.S",CC,flags,ppflags,tmp2,
			asflags,tmppath);

		if (vflag) fprintf(stderr,"%s\n",tmp);
		if (status=system(tmp)) Exit(status);
		getHead(tmp,files[i]);
		strcat(tmp,".o");
		addarg(ofiles,tmp,OFILSZ);
		}
	else if (strequ(e,"c")) {
		/*
		sprintf(tmp,"%s %s %s %s",CC,flags,cflags,files[i]);
		*/
		sprintf(tmp,"%s %s %s %s %s",CC,flags,ppflags,cflags,files[i]);
		if (vflag) fprintf(stderr,"%s\n",tmp);
		if (status=system(tmp)) Exit(status);
		getHead(tmp,files[i]);
		strcat(tmp,".o");
		addarg(ofiles,tmp,OFILSZ);
		}
	else if (strequ(e,"o")) {
		addarg(ofiles,files[i],OFILSZ);
		}
	else {
		fprintf(stderr,"%s: bad extent\n",e);
		Exit(1);
		}
	}

if (stoppoint != 0) Exit(0);

/* Select math library */
if (SZ) {
	sprintf(tmp,"%s/libm/%s/libm.a",LSIPKG,ENDIAN);
	if (fileExists(tmp)) strcpy(LIBM,tmp);
	}

/* Select C library */
sprintf(LIBC,"%s/lib/%s%s",LSIPKG,ENDIAN,Gnum);
if (crt0[0]==0) sprintf(crt0,"%s/crt0x.o",LIBC);

/* -nostdlib */

/*
**
**	Ratinder P. S. Ahuja. 23 Aug 94.  Changed mips-idt-ecoff to mips64-elf-ld and used 
**	-oformat symbolsrec option to produce symbols and srec format
**	Also produce a normal executable for remote debugging using gdb
**
if (dstart) 
sprintf(tmp,"mips-idt-ecoff-ld -N -M -e _start -Ttext %s -Tdata %s %s %s > %s.map",tstart,dstart,ldflags,ofiles,ofile); 
else
sprintf(tmp,"mips-idt-ecoff-ld -N -M -e _start -Ttext %s %s %s %s > %s.map",tstart,ldflags,ofiles,ofile);

**
**
*/

if (dstart) 
sprintf(tmp,"%s%s/bin/mips64-elf-ld -N -M %s -e _start -Ttext %s -Tdata %s %s %s > %s.map",CYGROOT,osdir,syms,tstart,dstart,ldflags,ofiles,ofile); 
else
sprintf(tmp,"%s%s/bin/mips64-elf-ld -N -M %s -e _start -Ttext %s %s %s > %s.map",CYGROOT,osdir,syms,tstart,ldflags,ofiles,ofile);

/*
sprintf(tmp,"mips64-elf-ld -N -M %s -e _start -T %s %s %s %s > %s.map",syms,tstart,ldflags,crt0,ofiles,ofile);
*/


if (vflag) fprintf(stderr,"%s\n",tmp);
if (status=system(tmp)) Exit(status);

/* now make the appropriate download records */
#ifdef NOTYET
if (fast) sprintf(tmp,"genfrec %s %s %s > %s.rec",syms,chksum,objfile,ofile);
else sprintf(tmp,"gensrec %s %s %s > %s.rec",prom,syms,objfile,ofile);
if (vflag) fprintf(stderr,"%s\n",tmp);
if (status=system(tmp)) Exit(status);
#endif

#if 0
/* make srecs */
sprintf(tmp,"gensrec %s %s > %s.rec",prom,objfile,ofile);

/* now make the symbols */
if (syms) {
	sprintf(tmp,"%s.rec",ofile);
	sprintf(tmp2,"%s.tmp",ofile);
	rename(tmp,tmp2);
	mksyms(ofile);
	appendFile(tmp,tmp2);
	if (vflag) fprintf(stderr,"rm %s\n",tmp2);
	unlink(tmp2);
	}
#endif
Exit(0);
}

/*************************************************************
*  appendFile(dst,src)
*/
appendFile(dst,src)
char *dst,*src;
{
int c;
FILE *dfp,*sfp;

if (vflag) fprintf(stderr,"cat %s >> %s\n",src,dst);
sfp = fopen(src,"r");
if (sfp == 0) {
	fprintf(stderr,"Can't open %s\n",src);
	Exit(1);
	}
dfp = fopen(dst,"a+");
if (dfp == 0) {
	fprintf(stderr,"Can't open %s\n",dst);
	Exit(1);
	}

for (;;) {
	c = getc(sfp);
	if (c == EOF) break;
	putc(c,dfp);
	}
fclose(sfp);
fclose(dfp);
}

/*************************************************************
*  fixhash(in,out)
*/
fixhash(in,out)
char *in,*out;
{
FILE *ifp,*ofp;
char *p;

ifp = fopen(in,"r");
ofp = fopen(out,"w");

while (fgets(tmp,LNMAX,ifp) != NULL) {
	if (p=strchr(tmp+1,'#'))  {
		p[0] = '\n';
		p[1] = 0;
		}
	fputs(tmp,ofp);
	}
fclose(ifp);
fclose(ofp);
}

/*************************************************************
*  mksyms(f)
*/
mksyms(f)
char *f;
{
FILE *mfp,*ofp;
char buf[100],*field[10];
int nf,flag,csum,len;

if (vflag) fprintf(stderr,"mksyms %s.map > %s.rec\n",f,f);

flag = 0;
sprintf(tmp,"%s.map",f);
mfp = fopen(tmp,"r");
if (mfp == 0) {
	fprintf(stderr,"Can't open %s\n",tmp);
	Exit(1);
	}

sprintf(tmp,"%s.rec",f);
ofp = fopen(tmp,"w");
if (ofp == 0) {
	fprintf(stderr,"Can't open %s\n",tmp);
	Exit(1);
	}

for (;;) {
	if (!fgets(buf,100,mfp)) break;
	buf[strlen(buf)-1] = 0;
	if (strlen(buf)==0) continue;
	if (buf[0] != ' ') continue;
	nf = argvize(field,buf);
	if (nf != 2) continue;
	if (strlen(field[0]) != 8) continue;
	csum = 0;
	len = strlen(field[1])+8+2;
	fprintf(ofp,"S4%02X%s%s,%02x\n",len,field[0],field[1],csum);
	}
fclose(mfp);
fclose(ofp);
}

/*************************************************************
*  fileExists(p)
*	returns true if the file exists
*/
fileExists(p)
char *p;
{
struct stat st;

if (stat(p,&st)) return(0);
return(1);
}

/*************************************************************
*  getHead(d,s)
*	isolates the file name, e.g. "jim/fred.o" returns "fred" in d.
*/
getHead(d,s)
char *d,*s;
{
char *b,*e;

b = strrchr(s,'/');
if (b) b++;
else b = s;

e = strrchr(s,'.');
strncpy(d,b,e-b);
d[e-b] = 0;
}

/*************************************************************
*  getRoot(d,s)
*	isolates the root, e.g. "jim/fred.o" returns "jim" in d.
*/
char *getRoot(d,s)
char *d,*s;
{
char *b,*e;

b = strrchr(s,'/');
if (!b) b = s;

strncpy(d,s,b-s);
d[b-s] = 0;
if (*d == 0) strcpy(d,".");
return(d);
}

/*************************************************************
*  char *extent(s)
*	returns a pointer to the extent portion of a filename
*/
char *extent(s)
char *s;
{
char *p;

p = strrchr(s,'.');
return(p+1);
}

/*************************************************************
*  int argvize(av,s) 
*	place address of each word in s into the array av
*/
int argvize(av,s)
char *av[];
char *s;
{
char **pav = av, c;
int ac;

for (ac=0;;ac++) {
	/* step over cntrls and spaces */
	while(*s && *s <= ' ') ++s;

	/* if eos quit */
	if(!*s) break;

	c = *s;
	/* find end of word */
	if (pav) *pav++ = s;
	while(' ' < *s) ++s;

	/* not eos inc ptr */
	if(*s) *s++ = 0;
	}
return(ac);
}


/*************************************************************
*  addarg(d,s,max)
*	a safer version of addarg
*/
addarg(d,s,max)
char *d,*s;
int max;
{

if (strlen(d) + strlen(s) > max) {
	fprintf(stderr,"%s: out of string space.. %s\n",s,d);
	Exit(1);
	}

strcat(d," ");
strcat(d,s);
}

/*************************************************************
*  fixstruct(in,out)
*/
fixstruct(in,out)
char *in,*out;
{
FILE *ifp,*ofp;
char *p;
char plabel[100],pval[100];
char label[100],val[100],direct[100];
int structflg;

ifp = fopen(in,"r");
ofp = fopen(out,"w");

structflg = 0;
while (fgets(tmp,LNMAX,ifp) != NULL) {
#if 1
	/* These tools don't support .struct, so simulate it */
	strcpy(tmp2,tmp);
	tmp2[strlen(tmp2)-1] = 0; /* remove terminating \n */
	p = strtok(tmp2," \t");
	if (p == NULL) ;
	else if (structflg) {
		if (strequ(p,".data")||strequ(p,".text")) structflg = 0;
		else {
			tmp[strlen(tmp)-1] = 0; /* remove terminating \n */
			/* terminate line at /* */
			if (p=(char *)strstr(tmp,"/*")) *p = 0;
			/* replace ':' with ' '*/
			if (p=strchr(tmp,':')) *p = ' ';
			if (!(p=strtok(tmp," \t"))) continue;
			if (*p != '.') {
				strcpy(label,p);
				p = strtok(0," \t");
				}
			if (!p) p = "";
			strcpy(direct,p);
			strcpy(val,p+(strlen(p)+1));
			if (strlen(label)) 
			    sprintf(tmp,"#define %s (%s+(%s))\n",label,
							plabel,pval);
			strcpy(plabel,label);
			if (strequ(direct,".space")) strcpy(pval,val);
			else if (strequ(direct,".word")) strcpy(pval,"4");
			else if (strequ(direct,".half")) strcpy(pval,"2");
			else if (strequ(direct,".byte")) strcpy(pval,"1");
			else strcpy(pval,"0");
			strcpy(label,"");
			}
		}
	else if (strequ(p,".struct")) {
		structflg = 1;
		strcpy(tmp,"\t/* struct 0 */\n");
		strcpy(plabel,"0");
		strcpy(pval,"0");
		}
#endif
	fputs(tmp,ofp);
	}
fclose(ifp);
fclose(ofp);
}

#ifndef Exit
/*************************************************************
*  Exit(x)
*/
Exit(x)
int x;
{
sprintf(tmp,"%s.S",tmppath);
if (fileExists(tmp)) unlink(tmp);
sprintf(tmp,"%s.t1",tmppath);
if (fileExists(tmp)) unlink(tmp);
sprintf(tmp,"%s.t2",tmppath);
if (fileExists(tmp)) unlink(tmp);
exit(((x)>127)?1:(x));
}
#endif
#endif /* #ifdef PMCC_DRIVER */
