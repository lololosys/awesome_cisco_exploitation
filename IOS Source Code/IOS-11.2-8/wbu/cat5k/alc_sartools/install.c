/* $Id: install.c,v 1.1.4.1 1996/05/09 15:00:41 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sartools/install.c,v $
 *------------------------------------------------------------------
 * install.c -- pmcc environment generator.
 *
 * 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: install.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:41  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:50  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:24  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:31  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef host_mips
#include <ctype.h>
char *strstr();
#endif

#define BUFSZ 200
#define AVSIZE 40
#define strequ(x,y)	((strcmp(x,y)==0)?1:0)
#define CWDSZ 500
#define CYGBIN "progressive-930929/H-sparc-sun-sunos4.1.3/bin"

/*
 * This program is used in both Unix and MSDOS environments. ifdef MSDOS
 * is used to bracket all MSDOS specific code.
 */

typedef int Func();
typedef char *Lst;

char rev[] = "4.1_Beta";

#ifdef MSDOS
struct FileLst {
	char *src;
	char *dst;
	};

/* list of files that get fixed up */
struct FileLst fileLst[] = {
	{"makefile.dst","makefile"},
	{"tools/makefile.dst","tools/makefile"},
	{"pmon/makefile.dst","pmon/makefile"},
	{"pmon/target.dst","pmon/target.mk"},
	{"lib/makefile.dst","lib/makefile"},
	{"lib/target.dst","lib/target.mk"},
	{"libsa/makefile.dst","libsa/makefile"},
	{"libsa/target.dst","libsa/target.mk"},
	{"examples/makefile.dst","examples/makefile"},
	{"examples/pmon/makefile.dst","examples/pmon/makefile"},
	{"examples/pmon/stand/makefile.dst","examples/pmon/stand/makefile"},
	{"examples/bench/makefile.dst","examples/bench/makefile"},
	{"examples/intapp/makefile.dst","examples/intapp/makefile"},
	{"examples/pollio/makefile.dst","examples/pollio/makefile"},
	{"examples/intio/makefile.dst","examples/intio/makefile"},
	{"examples/graphx/makefile.dst","examples/graphx/makefile"},
	{"examples/graphx/keybd/makefile.dst","examples/graphx/keybd/makefile"},
	{"examples/graphx/graphics/makefile.dst","examples/graphx/graphics/makefile"},
	{"examples/sonic/makefile.dst","examples/sonic/makefile"},
	{"examples/o3opt/makefile.dst","examples/o3opt/makefile"},
	{"examples/misc/makefile.dst","examples/misc/makefile"},
	{0}};
#endif

char *defines[] = {
"######################################################################",
"#     This file contains information about your configuration.       #",
"######################################################################",
"",
"CFLAGS = $(OPTLEV) $(TFLAG)",
"ASFLAGS = $(OPTLEV) $(TFLAG)",
"",
".c.o:",
"	pmcc -c $(CFLAGS) $<",
"",
".s.o:",
"	pmcc -c $(ASFLAGS) $<",
"",
".SUFFIXES: .o .s",
0};

char *devinfo[] = {
"#include <termio.h>",
"#include <terms.h>",
"#include \"pmon.h\"",
"",
"int p2681(),ptty(),ps2driver();",
0};

char *bauds[] = {
	"75","110","134","150","200","300","600","1200",
	"1800","2400","4800","9600","19200","38400",0};
/*
 * This program has far too many global variables, but that's because
 * it was converted into C from a csh script. That also accounts for
 * its really horrible structure!
 */
Func *fileFixer;
char ftext[10],fdata[10],clientpc[10];
char *dlproto,*fpem,*nvram,*ethernet,*board,*remote;
char *hostport,etheraddr[12],*NMOD,*prompt;
char COMP_ROOT[80],BUILD_DIRS[80],CLEAN_DIRS[80],PMON_TARG[80],LIB_TARG[80];
char mach[80],*FPEM_DIR,*tools,*chkfile;
char cwd[CWDSZ+2],targets[80],dlport[10],manpath[200];
Lst cfg[20],cpu[20],pkgsw[20];
Lst devtyp[20],devadr[20],devaux[20];
int no_chksum,clkfreq;
int MIPSTOOLS,CROSSTOOLS;
long memsize;
int umsk = 0777;

int Argc,Argcnt;
char **Argv;

FILE *sfp; /* dot.sh or setenv.bat */
FILE *dfp; /* defines.mk */
FILE *hfp; /* include/defines.h */
FILE *vfp; /* pmon/devinfo.c */
FILE *rfp; /* root.sh */
FILE *cfp; /* source.csh */

int do_bso(),do_epi(),do_ghs(),do_algor();
char *getenv();
char *lststr(),*lstnstr();

/*************************************************************
*  main(argc,argv)
*/
main(argc,argv)
int argc;
char *argv[];
{
int n,err,i,ac,j;
char buf[80],*d,tmp[80];
char *av[10],*def,*adr,*baud;
long r;

Argc = argc; Argv = argv; Argcnt = 1;

printf("\n");
printf("\n");
printf("This is the LSIPKG installation program for Version %s. Use it\n",rev);
printf("to define your tools environment.\n");
printf("\n");
printf("For support Call 800-232-MIPS, Fax 408-954-4874 or email pjb@lsil.com\n");
printf("\n");
printf("For each of the questions that follow, the choices are shown\n");
printf("in square brackets, the default is shown in parentheses.\n");
printf("\n");

loop1:
lstdel(cfg); lstdel(cpu); lstdel(devtyp); lstdel(devadr); lstdel(devaux);
lstdel(pkgsw); fpem = "";

getcwd(cwd,CWDSZ);

#ifdef MSDOS
printf("1. BSO tools.\n");
printf("2. Algorithmics tools.\n");
#if 0
printf("3. EPI tools.\n");
printf("4. Green Hills tools.\n");
printf("5. GNU tools from Cygnus Support.\n");
#endif
#else
printf("1. MIPS tools hosted on a MIPS system\n");
printf("2. MIPS tools hosted on a SPARCstation\n");
printf("3. A MIPS-based Decstation\n");
printf("4. BSO/Tasking tools hosted on a SPARCstation\n");
printf("5. A MIPS-based SGI workstation\n");
printf("6. EPI tools hosted on a SPARCstation\n");
printf("7. Algorithmics tools on a SPARCstation\n");
printf("8. GNU tools from Cygnus Support on a SPARCstation\n");
#endif
printf("\n");
printf("\nEnter tool type [1-n q] (q) > ");
for (;;) {
	Gets(buf);
	printf("\n");
	if (*buf == 0) exit(1);
	if (striequ(buf,"q")) exit(1);
	sscanf(buf,"%ld",&r);
	COMP_ROOT[0] = 0;
	switch(r) {
#ifdef MSDOS
		case 1 : 
			tools = "BSODOS"; 
			printf("Enter location of %s tools (/) > ",tools);
			chkfile = "r3000/bin386/cc3.exe";
			for (;;) {
				Gets(buf);
				printf("\n");
				if (strempty(buf)) strcpy(buf,"/.");
				sprintf(tmp,"%s/%s",buf,chkfile);
				if (fileExists(tmp)) break;
			printf("%s: not found, reenter the path > ",tmp);
				}
			fileFixer = do_bso; 
			strcpy(dlport,"com1");
			strcpy(COMP_ROOT,buf);
			break;
		case 2 : 
			tools = "ALGDOS"; 
			printf("Enter location of %s tools (/) > ",tools);
			chkfile = "bin/gcc-sde.exe";
			for (;;) {
				Gets(buf);
				printf("\n");
				if (strempty(buf)) strcpy(buf,"/.");
				sprintf(tmp,"%s/%s",buf,chkfile);
				if (fileExists(tmp)) break;
			printf("%s: not found, reenter the path > ",tmp);
				}
			fileFixer = do_algor; 
			strcpy(dlport,"com1");
			strcpy(COMP_ROOT,buf);
			break;
#if 0
		case 3 : 
			tools = "EPIDOS"; 
			printf("Enter location of %s tools (/) > ",tools);
			chkfile = "bin/hc.exe";
			for (;;) {
				Gets(buf);
				printf("\n");
				if (strempty(buf)) strcpy(buf,"/.");
				sprintf(tmp,"%s/%s",buf,chkfile);
				if (fileExists(tmp)) break;
			printf("%s: not found, reenter the path > ",tmp);
				}
			fileFixer = do_epi; 
			strcpy(dlport,"com1");
			strcpy(COMP_ROOT,buf);
			break;
		case 4 : 
			tools = "GHSDOS"; 
			printf("Enter location of %s tools (/) > ",tools);
			chkfile = "ccmips";
			for (;;) {
				gets(buf);
				printf("\n");
				if (strempty(buf)) strcpy(buf,"/.");
				sprintf(tmp,"%s/%s",buf,chkfile);
				if (fileExists(tmp)) break;
			printf("%s: not found, reenter the path > ",tmp);
				}
			fileFixer = do_ghs; 
			strcpy(dlport,"com1");
			strcpy(COMP_ROOT,buf);
			break;
		case 5 : 
			tools = "CYGNUS"; 
			printf("Enter location of %s tools (/) > ",tools);
			chkfile = "progressive/H-sparc-sun-sunos4.1.3/bin/mips-idt-ecoff-as";
			for (;;) {
				gets(buf);
				printf("\n");
				if (strempty(buf)) strcpy(buf,"/.");
				sprintf(tmp,"%s/%s",buf,chkfile);
				if (fileExists(tmp)) break;
			printf("%s: not found, reenter the path > ",tmp);
				}
			fileFixer = do_cygnus; 
			strcpy(dlport,"com1");
			strcpy(COMP_ROOT,buf);
			break;
#endif
#else /* ======================= Unix ========================= */
	    case 1 :
	    	/* MIPSNATIVE */
		backtick(buf,"uname -m");
		if (!strstr(buf,"mips")) {
		printf("Are you sure this is a MIPS computer? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		backtick(buf,"uname -r");
		if (strstr(buf,"4_0")) {
			printf("uname -r=[%s]\n",buf);
			printf("Due to problems with the assembler,\n");
			printf("this package will not build on RISC/os 4.0\n");
			printf("exiting.\n");
			exit(1);
			}
		if (strstr(buf,"5.")) lstadd(cfg,"RISC5");
		if (strstr(buf,"4_5")) lstadd(cfg,"RISC45");
		tools = "MIPSNATIVE";
		remote = "/etc/remote.pdbx";
		strcpy(dlport,"tty1");
		strcpy(COMP_ROOT,"native");
		MIPSTOOLS = 1;
		break;
	    case 2 :
		/* MIPSSUN */
		backtick(buf,"uname -m");
		if (!strstr(buf,"sun4")) {
		printf("Are you sure this is a SPARCstation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		tools = "MIPSSUN";
		MIPSTOOLS = 1;
		CROSSTOOLS = 1;
		remote = "/etc/remote.pdbx";
		strcpy(dlport,"ttyb");
		while (1) {
			printf("Where are the tools located? > ");
			Gets(buf);
			printf("\n");
			sprintf(tmp,"%s/usr/bin/ccr3000",buf);
			if (fileExists(tmp)) {
				NMOD = "r3000";
				break;
				}
			sprintf(tmp,"%s/usr/bin/ccx",buf);
			if (fileExists(tmp)) {
				NMOD = "x";
				break;
				}
			printf("%s/usr/bin/cc{r3000,x}: File not found.\n",buf);
			}
		strcpy(COMP_ROOT,buf);
		break;
	    case 3 :
		/* DECSTATION */
		backtick(buf,"uname -m");
		if (!strequ(buf,"RISC")) {
		printf("Are you sure this is a DecStation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		lstadd(cfg,"DECRISC");
		tools = "DECSTATION";
		remote = "/etc/remote";
		strcpy(dlport,"tty01");
		strcpy(COMP_ROOT,"native");
		MIPSTOOLS = 1;
		break;
	    case 4 :
		/* BSOSUN */
		backtick(buf,"uname -m");
		if (!strstr(buf,"sun4")) {
		printf("Are you sure this is a SPARCstation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		tools = "BSOSUN";
		CROSSTOOLS = 1;
		while (1) {
			printf("Where are the tools located? > ");
			Gets(buf);
			printf("\n");
			printf(tmp,"%s/r3000/bin/cc3",buf);
			if (fileExists(tmp)) break;
			printf("%s: File not found.",tmp);
			}
		strcpy(COMP_ROOT,buf);
		strcpy(dlport,"ttyb");
		break;
	    case 5 :
		/* SGI */
		backtick(buf,"uname -m");
		if (!strequ(buf,"IRIS")) {
		printf("Are you sure this is an SGI computer? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		lstadd(cfg,"SGIRISC");
		tools = "SGI";
		remote = "/etc/remote";
		strcpy(dlport,"ttyd2");
		strcpy(COMP_ROOT,"native");
		MIPSTOOLS = 1;
		break;
	    case 6 :
		/* EPISUN */
		backtick(buf,"uname -m");
		if (!strstr(buf,"sun4")) {
		printf("Are you sure this is a SPARCstation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		tools = "EPISUN";
		CROSSTOOLS = 1;
		while (1) {
			printf("Where are the tools located? > ");
			Gets(buf);
			printf("\n");
			printf(tmp,"%s/bin/hc",buf);
			if (fileExists(tmp)) break;
			printf("%s: File not found.",tmp);
			}
		strcpy(COMP_ROOT,buf);
		strcpy(dlport,"ttyb");
		break;
	    case 7 :
		/* ALGSUN */
		backtick(buf,"uname -m");
		if (!strstr(buf,"sun4")) {
		printf("Are you sure this is a SPARCstation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		tools = "ALGSUN";
		CROSSTOOLS = 1;
		strcpy(COMP_ROOT,"/usr/local/sde");
		while (1) {
			printf("Where are the tools located? (%s) > ",
							COMP_ROOT);
			Gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,COMP_ROOT);
			printf(tmp,"%s/bin/gcc-sde",buf);
			if (fileExists(tmp)) break;
			printf("%s: File not found.",tmp);
			}
		strcpy(COMP_ROOT,buf);
		strcpy(dlport,"ttyb");
		break;
	    case 8 :
		/* CYGSUN */
		backtick(buf,"uname -m");
		if (!strstr(buf,"sun4")) {
		printf("Are you sure this is a SPARCstation? [y n] (n) >");
			gets(buf);
			if (striequ(buf,"n") || buf[0] == 0) exit(1);
			}
		tools = "CYGSUN";
		CROSSTOOLS = 1;
		while (1) {
			printf("Where are the tools located? > ");
			Gets(buf);
			printf("\n");
	chkfile = "progressive/H-sparc-sun-sunos4.1.3/bin/mips-idt-ecoff-as";
			printf(tmp,"%s/%s",buf,chkfile);
			if (fileExists(tmp)) break;
			printf("%s: File not found.",tmp);
			}
		strcpy(COMP_ROOT,buf);
		break;
#endif
		default :
			printf("BAD value, reenter your selection > ");
		}
#ifdef MSDOS
	if (fileFixer) break;
#else
	if (COMP_ROOT[0] != 0) break;
	printf("reenter > ");
#endif
	}



/* ################## START OF PMON STUFF ########################## */
def = "3";
printf("Target Configuration\n");
printf("\n");
printf("Note that:\n");
printf("\tSelection 2 includes support for the Pocket Rocket.\n");
printf("\tSelection 3 includes support for RacerX and Pocket Rocket.\n");
printf("\n");
printf("1. Pocket Rocket (128KB PROM)\n");
printf("2. RacerX (128KB PROM)\n");
printf("3. Turbo Rocket or RacerXc (256KB PROM)\n");
printf("4. RacerX/Xc with console on PS2 port plus 8-bit color video interface\n");
printf("5. Single ATMizer R-T or Host ATMizer R-T\n");
printf("6. ATMizer R-T SAR engine\n");
printf("7. Custom setup\n");
printf("\n");
printf("Enter selection [1-n] (%s) > ",def);
while (1) {
	Gets(buf);
	printf("\n");
	if (buf[0] == 0) strcpy(buf,def);
	r = 0;
	sscanf(buf,"%ld",&r);
	switch (r) {
	    case 1 :
		/*  Pocket Rocket */
		board = "PRock";
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devadr,"0");
		lstadd(devadr,"0xbe000000");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(cpu,"LR33000");
		lstadd(cpu,"LR33050");
		ethernet = "no";
		nvram = "no";
		strcpy(ftext,"9fc00000");
		strcpy(fdata,"80000100");
		strcpy(clientpc,"80020000");
		lstadd(pkgsw,"TIMING");
		break;
	    case 2 :
		/* RacerX */
		board = "RacerX";
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devadr,"0");
		lstadd(devadr,"0xbe000000");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(cpu,"LR33000"); 
		lstadd(cpu,"LR33050");
		lstadd(cpu,"LR33020");

		r = 2;
		if (fileExists("fpem") || fileExists("optfpem")) {
			printf("You must choose between floating-point");
			printf(" emulation and Ethernet download.\n");
			printf("\n");
			while (1) {
			    printf("fp-emulation(1) Ethernet(2) [1 2] (1) > ");
				Gets(buf);
				if (buf[0] == 0) strcpy(buf,"1");
				if (sscanf(buf,"%ld",&r) == 1) {
					if (r == 1 || r == 2) break;
					}
			  printf("BAD input, please reenter your selection\n");
				}
			}
		if (r == 1) ethernet = "no";
		else {
			ethernet = "yes";
			strcpy(etheraddr,"0xbc000000");
			fpem = "CP1";
			}
		nvram = "no";
		strcpy(ftext,"9fc00000");
		strcpy(fdata,"80000100");
		strcpy(clientpc,"80020000");
		lstadd(pkgsw,"TIMING");
		break;
	    case 3 :
		/* Turbo Rocket or RacerXc */
		board = "TRock+RacerXc";
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devadr,"0");
		lstadd(devadr,"0xbe000000");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(cpu,"LR33000");
		lstadd(cpu,"LR33050");
		lstadd(cpu,"LR33020");
		lstadd(cpu,"LR33120");
		lstadd(cpu,"LR33300");
		ethernet = "ask";
		strcpy(etheraddr,"0xbc000000");
		nvram = "yes";
		strcpy(ftext,"9fc00000");
		strcpy(fdata,"80000100");
		strcpy(clientpc,"80020000");
		lstadd(pkgsw,"TIMING");
		break;
	    case 4 :
		/* RacerX with console on PS2 port plus 8-bit..*/
		board = "RacerX";
		lstadd(devtyp,"ps2");
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devadr,"NONE");
		lstadd(devadr,"0");
		lstadd(devadr,"0xbe000000");
		lstadd(devaux,"NONE");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(cpu,"LR33000");
		lstadd(cpu,"LR33050");
		lstadd(cpu,"LR33020");
		lstadd(cpu,"LR33120");
		lstadd(cpu,"LR33300");
		ethernet = "ask";
		strcpy(etheraddr,"0xbc000000");
		nvram = "yes";
		strcpy(ftext,"9fc00000");
		strcpy(fdata,"80000100");
		strcpy(clientpc,"80020000");
		lstadd(pkgsw,"TIMING");
		break;
	    case 5 :
		/* Single ATMizer R-T or Host ATMizer R-T */
		board = "ATMRTHost";
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devtyp,"ptty");
		lstadd(devtyp,"ptty");
		lstadd(devadr,"0");
		lstadd(devadr,"0xa0e00000");
		lstadd(devadr,"0xa0800000");
		lstadd(devadr,"0xa0800010");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(devaux,"0");
		lstadd(devaux,"0");
		lstadd(cpu,"ATMIZER");
		ethernet = "no";
		nvram = "no";
		strcpy(ftext,"a0c00000");
		strcpy(fdata,"a0d00000");
		strcpy(clientpc,"a0d80000");
		clkfreq = 50;
		break;
	    case 6 :
		/* ATMizer R-T SAR engine */
		board = "ATMRTEng";
		lstadd(devtyp,"ptty");
		lstadd(devtyp,"ptty");
		lstadd(devtyp,"2681");
		lstadd(devtyp,"2681");
		lstadd(devadr,"0xa0800000");
		lstadd(devadr,"0xa0800010");
		lstadd(devadr,"0");
		lstadd(devadr,"0xa0e00000");
		lstadd(devaux,"1");
		lstadd(devaux,"1");
		lstadd(devaux,"9600");
		lstadd(devaux,"9600");
		lstadd(cpu,"ATMIZER");
		prompt = "SAR> ";
		ethernet = "no";
		nvram = "no";
		strcpy(ftext,"a0c00000");
		strcpy(fdata,"a0d00000");
		strcpy(clientpc,"a0d80000");
		clkfreq = 50;
		break;
	    case 7 :
		/* Custom setup */
		board = "custom";
		ethernet = "ask";
		nvram = "ask";
		goto customdev;
		break;
	    default :
		printf("BAD INPUT, reenter your selection > ");
		continue;
		break;
	}
	break;
}


def = "2";
if (!strequ(ethernet,"no")) def = "3";

printf("You can download to PMON using a number of techniques.\n");
printf("\n");
printf("1. Single-port mode (using tip or a terminal emulator)\n");
printf("2. Dual-port mode (using edown)\n");
if (strequ(def,"3")) printf("3. Ethernet (using tftp)\n");
printf("\n");

printf("Select default download technique [1-n] (%s) > ",def);
while (1) {
	Gets(buf);
	printf("\n");
	if (buf[0] == 0) strcpy(buf,def);
	r = 0;
	sscanf(buf,"%ld",&r);
	switch (r) {
	    case 1 :
		ethernet = "no";
		hostport = "tty0";
		dlport[0] = 0;
		lstadd(cfg,"SREC");
		dlproto = "none";
		break;
	    case 2 :
		ethernet = "no";
		hostport = "tty1";
		dlproto = "EtxAck";
		break;
	    case 3 :
		if (!strequ(ethernet,"no")) {
			hostport = "ethernet";
			ethernet = "yes";
			dlproto = "EtxAck";
			break;
			}
	    default :
		printf("BAD input, please reenter your selection > ");
		continue;
		break;
	}
	break;
}

if (fpem[0] != 0) ;
else if (fileExists("optfpem")) fpem = "OPT";
else if (fileExists("fpem")) fpem = "STD";
else fpem = "CP1";

goto customdone;

/* ########################### CUSTOM ################################# */
customdev:
def = "2 3 4 5 6";
printf("This package can be used to build a version of PMON for execution\n");
printf("on just the LR3000 or a combination of any LR33xxx family devices,\n");
printf("or the ATMizer.\n");
printf("\n");
printf("CPU types supported:\n");
printf("\n");
printf("1. LR3000\n");
printf("2. LR33000 (33K)\n");
printf("3. LR33050 (33K with FPU)\n");
printf("4. LR33020 (33K with BLIT engine)\n");
printf("5. LR33120 (33K with enhanced BLIT engine)\n");
printf("6. LR33300/33310 (enhanced 33K)\n");
printf("7. ATMizer\n");
printf("\n");
printf("Select one or more CPU type [1-n] (%s) > ",def);
while (1) {
	err = 0;
	gets(buf);
	printf("\n");
	if (*buf == 0) strcpy(buf,def);
	ac = argvize(av,buf);
	for (i=0;i<ac;i++) {
		if (strequ(av[i],"1")) {
			if (lststr(cpu,"LR3000")) err = 1;
			else lstadd(cpu,"LR3000");
			}
		else if (strequ(av[i],"2")) {
			if (lststr(cpu,"LR33000")) err = 1;
			else lstadd(cpu,"LR33000");
			}
		else if (strequ(av[i],"3")) {
			if (lststr(cpu,"LR33050")) err = 1;
			lstadd(cpu,"LR33050");
			}
		else if (strequ(av[i],"4")) {
			if (lststr(cpu,"LR33020")) err = 1;
			lstadd(cpu,"LR33020");
			}
		else if (strequ(av[i],"5")) {
			if (lststr(cpu,"LR33120")) err = 1;
			lstadd(cpu,"LR33120");
			}
		else if (strequ(av[i],"6")) {
			if (lststr(cpu,"LR33300")) err = 1;
			lstadd(cpu,"LR33300");
			}
		else if (strequ(av[i],"7")) {
			if (lststr(cpu,"ATMIZER")) err = 1;
			lstadd(cpu,"ATMIZER");
			}
		else err = 1;
		}
	if (lststr(cpu,"LR3000") && lstnstr(cpu,"LR33",4)) {
		printf("The LR3000 and LR33000 family are mutually exclusive.\n");
		err = 1;
		}
	if (lstnstr(cpu,"LR3",3) && strstr(lststr,"ATMIZER")) {
		printf("The ATMizer and LR3xxxx family are mutually exclusive.\n");
		err = 1;
		}
	if (err == 0) break;
	printf("BAD INPUT, reenter your selection > ");
	}


printf("Custom device configuration. Select each device in turn, enter\n");
printf("'q' when this list is complete.\n");
printf("\n");
printf("1. 2681 DUART\n");
printf("2. RacerX PS2 port plus 8-bit color video interface\n");
printf("3. Memory-mapped pseudo tty\n");
printf("\n");
n = 0;
i = 1;
while (1) {
	printf("Select device for tty%d [1-3 q] (q) > ",n);
	gets(buf);
	printf("\n");
	if (buf[0] == 0) strcpy(buf,"q");
	if (striequ(buf,"q")) break;
	r = 0;
	sscanf(buf,"%ld",&r);
	switch (r) {
	    case 1 :
		lstadd(devtyp,"2681");
		if (n == 0) {
			if (strstr(cpu,"ATMIZER")) adr = "0xa0e00000";
			else adr = "0xbe000000";
			}
		else adr = "none";
	printf("If this DUART is located at a non standard address\n");
	printf("you will need to enter it's address, otherwise just press\n");
	printf("enter. The address if entered, should be a Kseg1 virtual\n");
	printf("address of the form 0xbe000000. Note that the DUART is\n");
	printf("assumed to be connected to D0-D7 of the data bus.\n");
	printf("\n");
		while (1) {
			printf("Enter base address in hex (%s) > ",adr);
			gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,adr);
			if (sscanf(buf,"0x%lx",&r) == 1) break;
			printf("BAD INPUT\n");
			}
		lstadd(devadr,"0");
		lstadd(devadr,buf);
		printf("Valid Baud rates:");
		for (j=0;bauds[j];j++) {
			if ((j%8) == 0) printf("\n\t\t");
			else printf(", ");
			printf("%s",bauds[j]);
			}
		printf("\n");
		baud = "9600";
		while (1) {
			printf("Enter Baud rate for port A (%s) > ",baud);
			gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,baud);
			if (lststr(bauds,buf)) break;
			printf("BAD INPUT\n");
			}
		lstadd(devaux,buf);
		lstadd(devtyp,"2681");
		i++;
		n++;
		while (1) {
			printf("Enter Baud rate for port B (%s) > ",baud);
			gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,baud);
			if (lststr(bauds,buf)) break;
			printf("BAD INPUT\n");
			}
		lstadd(devaux,buf);
		break;
	    case 2 :
		lstadd(devtyp,"ps2");
		lstadd(devadr,"NONE");
		lstadd(devaux,"NONE");
		break;
	    case 3 :
		lstadd(devtyp,"ptty");
		printf("Each pseudo tty occupies a 16-byte block of shared\n");
		printf("memory and must be specified to be either a Dce or\n");
		printf("Dte connection. Use Dce for tty0 and tty1, use Dte\n");
		printf("for all others.\n");
		printf("\n");
		adr = "0xa0800000";
		while (1) {
			printf("Enter base address in hex (%s) > ",adr);
			gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,adr);
			if (sscanf(buf,"0x%lx",&r) == 1) break;
			printf("BAD INPUT\n");
			}
		lstadd(devadr,buf);
		baud = "1";
		while (1) {
			printf("Select Dte(1) or Dce(2) [1 2] (%s) > ",baud);
			gets(buf);
			printf("\n");
			if (buf[0] == 0) strcpy(buf,baud);
			if (strequ(buf,"1") || strequ(buf,"2")) break;
			printf("BAD INPUT\n");
			}
		lstadd(devaux,buf);
		break;
	    default :
		printf("BAD INPUT, reenter your selection\n");
		continue;
	}
	i++;
	n++;
}


if (lststr(cpu,"ATMIZER")) {
	strcpy(ftext,"a0c00000");
	strcpy(fdata,"a0d00000");
	}
else {
	strcpy(ftext,"9fc00000");
	strcpy(fdata,"80000100");
	}


printf("If you have located your ROM and RAM at non-standard\n");
printf("addresses you will need to enter their addresses, otherwise\n");
printf("just press enter. The address if entered must be a virtual\n");
printf("address of the form a0c00000\n");
printf("\n");
while (1) {
	printf("Enter ROM base address in hex (%s) > ",ftext);
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%lx",&r) == 1) break;
	printf("BAD INPUT\n");
	}
if (*buf) strcpy(ftext,buf);
while (1) {
	printf("Enter RAM base address in hex (%s) > ",fdata);
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%lx",&r) == 1) break;
	printf("BAD INPUT\n");
	}
if (*buf) strcpy(fdata,buf);

sscanf(fdata,"%lx",&r);
sprintf(clientpc,"%08x",(r&~0xfff)+(128*1024));
printf("Specify the base address to be used when building client programs.\n");
printf("This address must be at least 128K beyond the start of RAM, and\n");
printf("fall on a 64KB boundary.\n");
while (1) {
	printf("Enter client base address in hex (%s) > ",clientpc);
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%lx",&r) == 1) break;
	printf("BAD INPUT\n");
	}
if (*buf) strcpy(clientpc,buf);

printf("Normally PMON figures out how much RAM there is automatically.\n");
printf("However, some boards don't like you to access not existent\n");
printf("memory, in that case using this option to tell PMON how much\n");
printf("RAM you have available to client programs. This is usually the\n");
printf("distance from the client base address to the top of memory.\n");
printf("\n");
while (1) {
	printf("Enter memory size in Kbytes (AUTO) > ");
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%ld",&r) == 1) break;
	printf("BAD INPUT\n");
	}
if (!strequ(buf,"")) memsize = ((long)r)*1024;

printf("Normally PMON figures out what the clock frequency is automatically.\n");
printf("If this feature causes a problem in your system, you can specify\n");
printf("the frequency directly.\n");
printf("\n");
while (1) {
	printf("Enter CPU clock frequency in MHz (AUTO) > ");
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%ld",&r) == 1) break;
	printf("BAD INPUT\n");
	}
if (!strequ(buf,"")) clkfreq = r;


printf("\n");
printf("You need to choose an I/O port that can be used for\n");
printf("downloading to the target system.\n");
while (1) {
	printf("Which port do you wish to use? (%s) > ",dlport);
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
#ifdef MSDOS
	if (!strncmp(buf,"com",3) && isdigit(buf[3])) break;
#else
	if (!strncmp(buf,"tty",3)) break;
#endif
	printf("%s: bad port name\n",buf);
	}
if (*buf) strcpy(dlport,buf);

printf("Do you want to include a checksum in the fast-record format?\n");
printf("Note that records that include checksum data cannot be\n");
printf("downloaded to pre-3.0 versions of PMON.\n");
printf("\n");
while (1) {
	printf("Include checksum? [y n] (y) > ");
	gets(buf);
	printf("\n");
	if (strequ(buf,"") || striequ(buf,"y") || striequ(buf,"n")) break;
	printf("BAD INPUT\n");
	}
if (striequ(buf,"n")) no_chksum = 1;

printf("Do you want the pmcc command to default to using the LSI fast-record\n");
printf("format for downloading? Note that if you plan to use PMON in its\n");
printf("single-port mode, and download using tip or some similar type of\n");
printf("terminal emulator, you will probably find it better to use S-records.\n");
printf("\n");
while (1) {
	printf("Default to fast-record format? [y n] (y) > ");
	gets(buf);
	printf("\n");
	if (strequ(buf,"") || striequ(buf,"y") || striequ(buf,"n")) break;
	printf("BAD INPUT\n");
	}
if (striequ(buf,"n")) lstadd(cfg,"SREC");

printf("Do you want to be able to time the execution of programs? Note\n");
printf("that programs that are compiled with this feature  switched on,\n");
printf("require that they are invoked with the command \"g -c\" on\n");
printf("pre-3.0 versions of PMON. Also, if you use this feature on the\n");
printf("RacerX board, the beeper will sound unless you disconnect it.\n");
printf("\n");
while (1) {
	printf("Include timing? [y n] (y) > ");
	gets(buf);
	printf("\n");
	if (strequ(buf,"") || striequ(buf,"y") || striequ(buf,"n")) break;
	printf("BAD INPUT\n");
	}
if (striequ(buf,"y") || strequ(buf,"")) lstadd(pkgsw,"TIMING");

ethernet = "no";
printf("Do you want to include support for download via Ethernet? Note\n");
printf("that this requires a SONIC Ethernet controller, and will not\n");
printf("fit in a 128K byte EPROM if floating-point emulation is also\n");
printf("selected.\n");
printf("\n");
while (1) {
	printf("Include Ethernet support [y n] (y) > ");
	gets(buf);
	printf("\n");
	if (buf[0] == 0) strcpy(buf,"y");
	if (striequ(buf,"y") || striequ(buf,"n")) break;
	printf("BAD INPUT\n");
	}
if (striequ(buf,"y")) ethernet = "yes";

strcpy(etheraddr,"0xbc000000");
if (strequ(ethernet,"yes")) {
	printf("Select the address of your SONIC Ethernet controller.\n");
	printf("\n");
	while (1) {
		printf("SONIC base address (%s) > ",etheraddr);
		gets(buf);
		printf("\n");
		if (*buf == 0) break;
		if (sscanf(buf,"0x%lx",&r) == 1) break;
		printf("BAD INPUT\n");
		}
	if (*buf) strcpy(etheraddr,buf);
	}

if (strequ(ethernet,"yes")) hostport = "ethernet";
else hostport = "tty1";
printf("PMON normally accepts download records on %s,\n",hostport);
printf("and although you can change that at any time using the set\n");
printf("command. It may be more convenient to change the default.\n");
printf("\n");
printf("1. tty0\n");
printf("2. tty1\n");
printf("3. tty2\n");
if (strequ(ethernet,"yes")) printf("4. ethernet\n");
printf("\n");
printf("Select default hostport [1-n] (%s) > ",hostport);
while (1) {
	gets(buf);
	printf("\n");
	if (*buf == 0) break;
	if (sscanf(buf,"%ld",&r) != 1) {
		printf("BAD input, reenter your selection > ");
		continue;
		}
	if (r == 1) {
		hostport = "tty0";
		break;
		}
	else if (r == 2) {
		hostport = "tty1";
		break;
		}
	else if (r == 3) {
		hostport = "tty2";
		break;
		}
	else if (r == 4 && strequ(ethernet,"yes")) {
		hostport = "ethernet";
		break;
		}
	printf("BAD input, reenter your selection > ");
	}

dlproto = "EtxAck";
if (!strequ(hostport,"ethernet")) {
	printf("PMON normally uses the %s protocol to provide flow\n",dlproto);
	printf("control when downloading from a host, and although this\n");
	printf("can be changed at any time using the set command, it may\n");
	printf("be more convenient to change the default.\n");
	printf("\n");
	printf("1. EtxAck protocol\n");
	printf("2. XonXoff protocol\n");
	printf("3. None\n");
	printf("\n");
	printf("Select default dlproto [1-n] (1) > ");
	while (1) {
		gets(buf);
		printf("\n");
		if (*buf == 0) break;
		if (sscanf(buf,"%ld",&r) != 1) {
			printf("BAD input, reenter your selection > ");
			continue;
			}
		if (r == 1) {
			dlproto = "EtxAck";
			break;
			}
		else if (r == 2) {
			dlproto = "XonXoff";
			break;
			}
		else if (r == 3) {
			dlproto = "none";
			break;
			}
		printf("BAD input, reenter your selection > ");
		}
	}

nvram = "no";
printf("Do you want PMON to check for the existence of NVRAM?\n");
printf("\n");
while (1) {
	printf("Check for NVRAM? [y n] (y) > ");
	gets(buf);
	printf("\n");
	if (strequ(buf,"")) strcpy(buf,"y");
	if (striequ(buf,"y") || striequ(buf,"n")) break;
	printf("BAD INPUT\n");
	}
if (striequ(buf,"y")) nvram = "yes";

if (fileExists("optfpem")) d = "2";
else if (fileExists("fpem")) d = "1";
else d = "3";

printf("Floating-point support selection:\n");
printf("\n");
printf("1. Standard floating-point emulation\n");
printf("2. Optimized floating-point emulation\n");
printf("3. No floating-point emulation\n");
printf("4. No floating-point support\n");
printf("\n");
printf("Select fp support [1-n] (%s) > ",d);
while (1) {
	gets(buf);
	printf("\n");
	if (strempty(buf)) strcpy(buf,d);
	if (strequ(buf,"1")) {
		if (fileExists("fpem")) {
			fpem = "STD";
			break;
			}
		printf("Standard fp emulation library not found\n");
		}
	else if (strequ(buf,"2")) {
		if (fileExists("optfpem")) {
			fpem = "OPT";
			break;
			}
		printf("Optimized fp emulation library not found\n");
		}
	else if (strequ(buf,"3")) {
		fpem = "CP1";
		break;
		}
	else if (strequ(buf,"4")) {
		fpem = "NONE";
		break;
		}
	printf("BAD INPUT, reenter your selection > ");
	}

/* #################### END OF CUSTOM STUFF ########################## */
customdone:
/* #################### END OF PMON STUFF ########################## */

skippmon1:

if (lststr(cpu,"ATMIZER")) {
	if (strequ(board,"custom")) def = "1";
	else {
		strcat(targets,"BG BO");
		goto endiandone;
		}
	}
else def = "1 2";

#ifdef MSDOS
if (strequ(tools,"ALGDOS")) def = "1";
#endif
if (strequ(tools,"CYGSUN")) def = "1";

printf("This package can be used to build binaries for execution on big- or\n");
printf("little-endian target systems. You may select both of these options.\n");
printf("\n");
printf("Target types supported:\n");
printf("\n");
printf("1. Big Endian\n");
printf("2. Little Endian\n");
printf("\n");
printf("Select one or more target types [1 2] (%s) > ",def);
while (1) {
	targets[0] = 0;
	err = 0;
	Gets(buf);
	printf("\n");
	if (strempty(buf)) strcpy(buf,def);
	ac = argvize(av,buf);
	for (i=0;i<ac;i++) {
		if (strequ(av[i],"1")) {
			if (strstr(targets,"BG")) err = 1;
			strcat(targets," BG BO");
			}
		else if (strequ(av[i],"2")) {
			if (strstr(targets,"LG")) err = 1;
			strcat(targets," LG LO");
			}
		else err = 1;
		}
	if (lststr(cpu,"ATMIZER") && strstr(targets,"LG")) {
		printf("Warning: the ATMizer only operates in Big Endian mode\n");
		}
#ifdef MSDOS
	if (strequ(tools,"ALGDOS") && strstr(targets,"BG") && 
		strstr(targets,"LG")) {
	      printf("You must configure for Big or Little Endian, not both\n");
	      err = 1;
	      }
#endif
	if (err == 0) break;
	printf("BAD INPUT, reenter your selection > ");
	}
endiandone:

printf("Your selections are:\n");
printf("\n");
printf("	tools type = %s\n",tools);
if (! fileExists("pmon")) goto skippmon2;

/* ################## START OF PMON STUFF ####################### */
printf("\tCPU type =");
for (i=0;cpu[i];i++) printf(" %s",cpu[i]);
printf("\n");

if (no_chksum) printf("\tChecksum disabled\n");
else printf("\tChecksum enabled\n");

if (! lststr(pkgsw,"TIMING")) printf("\tTiming disabled\n");
else printf("\tTiming enabled\n");

if (prompt) printf("\tPROMPT = \"%s\"\n",prompt);

printf("\tDevices:\n");
printf("\t\t%-5s  %-4s  %-10s      %s\n","dev","type","address","baud");
for (i=0;devtyp[i];i++) {
	printf("\t\ttty%-2d  %-4s  %-10s      %s\n",i,devtyp[i],
	(strequ(devadr[i],"0"))?"":devadr[i],
	(strequ(devaux[i],"0"))?"dce":(strequ(devaux[i],"1"))?"dte":devaux[i]);
	}
if (strequ(ethernet,"yes")) {
	printf("\tEthernet download = %s\n",ethernet);
	printf("\tSONIC ADDR = %s\n",etheraddr);
	}
printf("\tDownload port = %s\n",hostport);
printf("\tDownload protocol = %s\n",dlproto);
if (memsize) printf("\tRAM memory size = %dK bytes\n",memsize/1024);
if (clkfreq) printf("\tCPU clock frequency = %d MHz\n",clkfreq);
if (*dlport) printf("\tHost port = %s\n",dlport);
if (strequ(nvram,"yes")) printf("\tNVRAM check = %s\n",nvram);
	
/* ################## END OF PMON STUFF ####################### */
skippmon2:

printf("	Targets = %s\n",targets);
printf("	fp support type = %s\n",fpem);
#if 0
if (lststr(cpu,"LR33020") && strequ(fpem,"STD")) {
	printf("\n");
	printf("Warning: the selections you have made, may not fit into a\n");
	printf("128K byte EPROM.\n");
	}
#endif
printf("\n");
printf("is this correct? [y n q] (y) > ");
Gets(buf);
if (striequ(buf,"q")) exit(1);
if (striequ(buf,"n")) goto loop1;

/*#########################################################################*/
/*#           Now implement the selections                                #*/
/*#########################################################################*/

if (COMP_ROOT[0] == 0) {
	printf("COMP_ROOT unknown, exiting\n");
	exit(1);
	}

if (fileExists("defines.mk")) {
	printf("Saving existing defines.mk in defines.old.\n");
	unlink("defines.old");
	rename("defines.mk","defines.old");
	}
else printf("Creating defines.mk.\n");
dfp = fopen("defines.mk","w");
if (dfp == 0) {
	fprintf(stderr,"can't open defines.mk\n");
	exit(1);
	}

if (fileExists("pmon/devinfo.c")) {
	printf("Saving existing pmon/devinfo.c in pmon/devinfo.old.\n");
	unlink("pmon/devinfo.old");
	rename("pmon/devinfo.c","pmon/devinfo.old");
	}
else printf("Creating pmon/devinfo.c.\n");
vfp = fopen("pmon/devinfo.c","w");
if (vfp == 0) {
	fprintf(stderr,"can't open pmon/devinfo.c\n");
	exit(1);
	}

/* set BUILD & CLEAN DIRS, also PMON & LIB TARG */
strcpy(BUILD_DIRS,"TOOLS");
CLEAN_DIRS[0] = 0;
PMON_TARG[0] = 0;
LIB_TARG[0] = 0;
FPEM_DIR = "lib";
if (strequ(fpem,"STD")) {
	strcat(BUILD_DIRS," FPEM");
	strcat(CLEAN_DIRS," FPEM");
	FPEM_DIR = "fpem";
	}
if (strequ(fpem,"OPT")) {
	strcat(BUILD_DIRS," OPTFPEM");
	strcat(CLEAN_DIRS," OPTFPEM");
	FPEM_DIR = "optfpem";
	}
if (MIPSTOOLS && fileExists("pmon")) {
	strcat(BUILD_DIRS," LIBSA");
	strcat(CLEAN_DIRS," LIBSA");
	}
if (fileExists("pmon")) {
	strcat(BUILD_DIRS," LIB PMON");
	strcat(CLEAN_DIRS," LIB PMON");
	if (strstr(targets,"BG")) strcat(PMON_TARG," BG");
	if (strstr(targets,"LG")) strcat(PMON_TARG," LG");
	}
if (strstr(targets,"BG")) strcat(LIB_TARG," BG BO");
if (strstr(targets,"LG")) strcat(LIB_TARG," LG LO");

for (i=0;defines[i];i++) fprintf(dfp,"%s\n",defines[i]);
fprintf(dfp,"BUILD_DIRS = %s\n",BUILD_DIRS);
fprintf(dfp,"CLEAN_DIRS = %s\n",CLEAN_DIRS);
fprintf(dfp,"PMON_TARG = %s\n",PMON_TARG);
fprintf(dfp,"LIB_TARG = %s\n",LIB_TARG);
fprintf(dfp,"FPEM_DIR = %s\n",FPEM_DIR);
fprintf(dfp,"REV = %s\n",rev);
fprintf(dfp,"FTEXT = %s\n",ftext);
fprintf(dfp,"FDATA = %s\n",fdata);

if (strstr(BUILD_DIRS,"PMON")) {
	if (strstr(LIB_TARG,"BG")) {
		mkdir("lib/bg",umsk); 
		mkdir("pmon/bg",umsk);
		}
	if (strstr(LIB_TARG,"BO")) mkdir("lib/bo",umsk);
	if (strstr(LIB_TARG,"LG")) {
		mkdir("lib/lg",umsk); 
		mkdir("pmon/lg",umsk);
		}
	if (strstr(LIB_TARG,"LO")) mkdir("lib/lo",umsk);
	}

if (MIPSTOOLS && fileExists("pmon")) {
	if (strstr(LIB_TARG,"BG")) mkdir("libsa/bg",umsk); 
	if (strstr(LIB_TARG,"BO")) mkdir("libsa/bo",umsk);
	if (strstr(LIB_TARG,"LG")) mkdir("libsa/lg",umsk); 
	if (strstr(LIB_TARG,"LO")) mkdir("libsa/lo",umsk);
	}

if (strstr(BUILD_DIRS,"FPEM")) {
	if (strstr(LIB_TARG,"BG")) {
		sprintf(tmp,"%s/bg",FPEM_DIR);
		mkdir(tmp,umsk); 
		}
	if (strstr(LIB_TARG,"BO")) {
		sprintf(tmp,"%s/bo",FPEM_DIR);
		mkdir(tmp,umsk); 
		}
	if (strstr(LIB_TARG,"LG")) {
		sprintf(tmp,"%s/lg",FPEM_DIR);
		mkdir(tmp,umsk); 
		}
	if (strstr(LIB_TARG,"LO")) {
		sprintf(tmp,"%s/lo",FPEM_DIR);
		mkdir(tmp,umsk); 
		}
	}

#ifdef MSDOS /* ================= MSDOS ================== */
fprintf(dfp,"EXE = .exe\n");
fprintf(dfp,"O = bj\n");
fprintf(dfp,"SPCLTOOLS = man.exe\n");
fprintf(dfp,"RMF = rm-f\n");

unlink("setenv.bat");
sfp = fopen("setenv.bat","w");
if (sfp == 0) {
	fprintf(stderr,"can't open setenv.bat\n");
	exit(1);
	}

fprintf(sfp,"@echo off\n");
fprintf(sfp,"echo Connecting with pmcc version %s using %s tools\n",rev,tools);
fprintf(sfp,"echo path %%path%% > unsetenv.bat\n");

if (strequ(tools,"BSODOS")) {
	fprintf(sfp,"set LSIPKG=%s\n",cwd);
	fprintf(sfp,"set BSO_TASKING=%s\n",COMP_ROOT);
	fprintf(sfp,"path %%path%%;%s/r3000/bin386;%s/r3000/bin;%s/tools\n",
					COMP_ROOT,COMP_ROOT,cwd);
	/* create defines.mk */
	fprintf(dfp,"# tools type BSODOS\n");
	fprintf(dfp,"TOOLSET = BSODOS\n");
	fprintf(dfp,"PMCC = bso.c\n");
	fprintf(dfp,"OPTLEV = -O\n");
	fprintf(dfp,"\n");
	}
else if (strequ(tools,"ALGDOS")) {
	fprintf(sfp,"set LSIPKG=%s\n",cwd);
	if (strstr(targets,"BG")) {
	 fprintf(sfp,"set COMPILER_PATH=%s/libeb;%s/bin\n",COMP_ROOT,COMP_ROOT);
	 fprintf(sfp,"set LIBRARY_PATH=%s/libeb\n",COMP_ROOT);
	 fprintf(sfp,"set GNUTARGET=elf-big\n");
	 }
	else {
	 fprintf(sfp,"set COMPILER_PATH=%s/libel;%s/bin\n",COMP_ROOT,COMP_ROOT);
	 fprintf(sfp,"set LIBRARY_PATH=%s/libel\n",COMP_ROOT);
	 fprintf(sfp,"set GNUTARGET=elf-little\n");
	 }
	fprintf(sfp,"if NOT exist %s\\tmp md %s\\tmp\n",cwd,cwd);
	fprintf(sfp,"set TMPDIR=%s/tmp\n",cwd);
	fprintf(sfp,"set GO32TMP=%s/tmp\n",cwd);
	fprintf(sfp,"path %%path%%;%s/bin;%s/tools\n", COMP_ROOT,cwd);
	/* create defines.mk */
	fprintf(dfp,"# tools type ALGDOS\n");
	fprintf(dfp,"TOOLSET = ALGDOS\n");
	fprintf(dfp,"PMCC = algo.c\n");
	fprintf(dfp,"OPTLEV = -O\n");
	fprintf(dfp,"\n");
	}
#else /* ================= must be Unix ================= */
fprintf(dfp,"RMF = rm -f\n");

sfp = fopen("dot.sh","w");
if (sfp == 0) {
	fprintf(stderr,"can't open dot.sh\n");
	exit(1);
	}
cfp = fopen("source.csh","w");
if (cfp == 0) {
	fprintf(stderr,"can't open source.csh\n");
	exit(1);
	}
rfp = fopen("root.sh","w");
if (rfp == 0) {
	fprintf(stderr,"can't open root.sh\n");
	exit(1);
	}

fprintf(sfp,"echo Connecting with pmcc version %s using %s tools\n",rev,tools);
fprintf(cfp,"echo Connecting with pmcc version %s using %s tools\n",rev,tools);

	if (strequ(tools,"MIPSNATIVE")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"PATH=$PATH:$LSIPKG/tools\n");
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path $LSIPKG/tools)\n");
		/* create defines.mk */
		fprintf(dfp,"# tools type MIPSNATIVE\n");
		fprintf(dfp,"TOOLSET = MIPSNATIVE\n");
		fprintf(dfp,"AR = ar crs\n");
		fprintf(dfp,"PMCC = mips.c\n");
		fprintf(dfp,"SPCLTOOLS = edown promize\n");
		backtick(tmp,"ls -l /usr/bin/cc");
		if (!(strstr(tmp,"cc2.20") && strstr(targets,"LO"))) 
			fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"\n");
		if (!lststr(cfg,"RISC5")) {
		if (strstr(targets,"BO") && !fileExists("/lsipkgbo/usr/lib")) {
			if (! fileExists("/lsipkgbo/usr"))
				fprintf(rfp,"mkdir -p /lsipkgbo/usr\n");
			fprintf(rfp,"ln -s %s/lib/bo /lsipkgbo/usr/lib\n",cwd);
			}
		if (strstr(targets,"LO") && !fileExists("/lsipkglo/usr/lib")) {
			if (! fileExists("/lsipkglo/usr"))
				fprintf(rfp,"mkdir -p /lsipkglo/usr\n");
			fprintf(rfp,"ln -s %s/lib/lo /lsipkglo/usr/lib\n",cwd);
			}
		    }
		}
	else if (strequ(tools,"DECSTATION")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"PATH=$PATH:$LSIPKG/tools\n");
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path $LSIPKG/tools)\n");
		/* create defines.mk */
		fprintf(dfp,"# tools type DECSTATION\n");
		fprintf(dfp,"TOOLSET = DECSTATION\n");
		fprintf(dfp,"AR = ar crs\n");
		fprintf(dfp,"PMCC = mips.c\n");
		fprintf(dfp,"SPCLTOOLS = edown promize\n");
		fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"\n");
		if (!fileExists("/lsipkg")) fprintf(rfp,"mkdir /lsipkg\n");
		if (strstr(targets,"LO") && !fileExists("/lsipkg/lib")) 
			fprintf(rfp,"ln -s %s/lib/lo /lsipkg/lib\n",cwd);
		if (strstr(targets,"BO") && !fileExists("/lsipkg/libeb")) 
			fprintf(rfp,"ln -s %s/lib/bo /lsipkg/libeb\n",cwd);
		}
	else if (strequ(tools,"SGI")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"PATH=$PATH:$LSIPKG/tools\n");
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path $LSIPKG/tools)\n");
		/* create defines.mk */
		fprintf(dfp,"# tools type SGI\n");
		fprintf(dfp,"TOOLSET = SGI\n");
		fprintf(dfp,"AR = ar crs\n");
		fprintf(dfp,"PMCC = mips.c\n");
		fprintf(dfp,"SPCLTOOLS = edown promize\n");
		fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"\n");
		}
	else if (strequ(tools,"BSOSUN")) {
		/* Bourne shell */
		fprintf(sfp,"BSO_TASKING=%s;export BSO_TASKING\n",COMP_ROOT);
		fprintf(sfp,"LM_LICENSE_FILE=%s/flexlm/licenses/license.dat\n",
					COMP_ROOT);
		fprintf(sfp,"LSIPKG=%s;export LSIPKG;export LM_LICENSE_FILE\n",
					cwd);
		fprintf(sfp,"PATH=$PATH:%s/r3000/bin:$LSIPKG/tools\n",
					COMP_ROOT);
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
	fprintf(cfp,"setenv LM_LICENSE_FILE %s/flexlm/licenses/license.dat\n",
					COMP_ROOT);
		fprintf(cfp,"setenv BSO_TASKING %s\n",COMP_ROOT);
		fprintf(cfp,"set path = ($path %s/r3000/bin $LSIPKG/tools)\n",
					COMP_ROOT);
#if 0
		# create links for floating-point libs
		#if (! -d $cwd/fpem/bg) then
		#	mkdir -p $cwd/fpem/{bg,lg}
		#	echo "all:" > $cwd/fpem/Makefile
		#	echo "clean:" >> $cwd/fpem/Makefile
		#	echo "	@rm -f bg/libf.a" >> $cwd/fpem/Makefile
		#	echo "	@rm -f lg/libf.a" >> $cwd/fpem/Makefile
		#	ln -s $COMP_ROOT/r3000/lib/libfb.a $cwd/fpem/bg/libf.a
		#	ln -s $COMP_ROOT/r3000/lib/libfl.a $cwd/fpem/lg/libf.a
		#endif
		#if (! -d $cwd/optfpem/bg) then
		#	mkdir -p $cwd/optfpem/{bg,lg}
		#	echo "all:" > $cwd/optfpem/Makefile
		#	echo "clean:" >> $cwd/optfpem/Makefile
		#	echo "	@rm -f bg/libf.a" >> $cwd/optfpem/Makefile
		#	echo "	@rm -f lg/libf.a" >> $cwd/optfpem/Makefile
		#	ln -s $COMP_ROOT/r3000/lib/libfntb.a $cwd/optfpem/bg/libf.a
		#	ln -s $COMP_ROOT/r3000/lib/libfntl.a $cwd/optfpem/lg/libf.a
		#endif
#endif
		/* create defines.mk */
		fprintf(dfp,"# tools type BSOSUN\n");
		fprintf(dfp,"TOOLSET = BSOSUN\n");
		fprintf(dfp,"AR = ar3 r\n");
		fprintf(dfp,"PMCC = bso.c\n");
		fprintf(dfp,"SPCLTOOLS = edown\n");
		fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"\n");
		}
	else if (strequ(tools,"MIPSSUN")) {
		sprintf(manpath,"%s/usr/man",COMP_ROOT);
		/* Bourne shell */
		fprintf(sfp,"COMP_HOST_ROOT=%s\n",COMP_ROOT);
		fprintf(sfp,"COMP_TARGET_ROOT=%s\n",COMP_ROOT);
		fprintf(sfp,"LSIPKG=%s\n",cwd);
		fprintf(sfp,"PATH=$PATH:%s/usr/bin:$LSIPKG/tools\n",COMP_ROOT);
		fprintf(sfp,"export COMP_HOST_ROOT COMP_TARGET_ROOT\n");
		fprintf(sfp,"export LSIPKG PATH\n");
		/* C shell */
		fprintf(cfp,"setenv COMP_HOST_ROOT %s\n",COMP_ROOT);
		fprintf(cfp,"setenv COMP_TARGET_ROOT %s\n",COMP_ROOT);
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path %s/usr/bin $LSIPKG/tools)\n",
					COMP_ROOT);

		/* create defines.mk */
		fprintf(dfp,"# tools type MIPSSUN\n");
		fprintf(dfp,"TOOLSET = MIPSSUN\n");
		fprintf(dfp,"AR = ar%s crs\n",NMOD);
		fprintf(dfp,"PMCC = mips.c\n");
		fprintf(dfp,"SPCLTOOLS = edown promize\n");
		fprintf(dfp,"OPTLEV = -O\n");
		if (strequ(NMOD,"r3000")) {
			fprintf(dfp,"NMOD = r3000\n");
			sprintf(tmp,"%s/lsipkg",COMP_ROOT);
			if (!fileExists(tmp)) {
				fprintf(rfp,"mkdir %s/lsipkg",COMP_ROOT);
				}

			sprintf(tmp,"%s/lsipkg/lib",COMP_ROOT);
			if (strstr(targets,"BO") && !fileExists(tmp)) {
				fprintf(rfp,"ln -s %s/lib/bo %s/lsipkg/lib\n",
					cwd,COMP_ROOT);
				}

			sprintf(tmp,"%s/lsipkg/libel",COMP_ROOT);
			if (strstr(targets,"LO") && !fileExists(tmp)) {
				fprintf(rfp,"ln -s %s/lib/lo %s/lsipkg/libel\n",
					cwd,COMP_ROOT);
				}
		  }
		fprintf(dfp,"\n");
		}
	else if (strequ(tools,"EPISUN")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"HCDIR=%s;export HCDIR\n",COMP_ROOT);
		fprintf(sfp,"HCINC=%s/include;export HCINC\n",COMP_ROOT);
		fprintf(sfp,"LIBDIR=%s/lib;export LIBDIR\n",COMP_ROOT);
		fprintf(sfp,"PATH=$PATH:$LSIPKG/tools:%s/bin\n",COMP_ROOT);
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"setenv HCDIR %s\n",COMP_ROOT);
		fprintf(cfp,"setenv HCINC %s/include\n",COMP_ROOT);
		fprintf(cfp,"setenv LIBDIR %s/lib\n",COMP_ROOT);
		fprintf(cfp,"set path = ($path $LSIPKG/tools %s/bin)\n",COMP_ROOT);
		/* create defines.mk */
		fprintf(dfp,"# tools type EPISUN\n");
		fprintf(dfp,"TOOLSET = EPISUN\n");
		fprintf(dfp,"AR = elib -q\n");
		fprintf(dfp,"PMCC = epi.c\n");
		fprintf(dfp,"SPCLTOOLS = edown\n");
		fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"\n");
		}
	else if (strequ(tools,"ALGSUN")) {
		/* Bourne shell */
		sprintf(manpath,"%s/usr/man",COMP_ROOT);
		fprintf(sfp,"SDEROOT=%s;export SDEROOT\n",COMP_ROOT);
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"PATH=$PATH:%s/bin:$LSIPKG/tools\n",COMP_ROOT);
		/* C shell */
		fprintf(cfp,"setenv SDEROOT %s\n",COMP_ROOT);
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path %s/bin $LSIPKG/tools)\n",COMP_ROOT);
		/* defines.mk */
		fprintf(dfp,"# tools type ALGSUN\n");
		fprintf(dfp,"TOOLSET = ALGSUN\n");
		fprintf(dfp,"AR = ar-sde cr\n");
		fprintf(dfp,"PMCC = algo.c\n");
		fprintf(dfp,"SPCLTOOLS = edown\n");
		fprintf(dfp,"OPTLEV = -O\n");
		}
	else if (strequ(tools,"GHSMIPS")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"PATH=$PATH:%s:$LSIPKG/tools\n",COMP_ROOT);
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"set path = ($path %s $LSIPKG/tools)\n",COMP_ROOT);
		/* defines.mk */
		fprintf(dfp,"# tools type GHSMIPS\n");
		fprintf(dfp,"TOOLSET = GHSMIPS\n");
		fprintf(dfp,"AR = ax q\n");
		fprintf(dfp,"PMCC = ghs.c\n");
		fprintf(dfp,"SPCLTOOLS = edown\n");
		fprintf(dfp,"OPTLEV = -O\n");
		}
	else if (strequ(tools,"CYGSUN")) {
		/* Bourne shell */
		fprintf(sfp,"LSIPKG=%s;export LSIPKG\n",cwd);
		fprintf(sfp,"CYGROOT=%s;export CYGROOT\n",COMP_ROOT);
		fprintf(sfp,"PATH=$PATH:%s/%s:$LSIPKG/tools\n",COMP_ROOT,CYGBIN);
		/* C shell */
		fprintf(cfp,"setenv LSIPKG %s\n",cwd);
		fprintf(cfp,"setenv CYGROOT %s\n",COMP_ROOT);
		fprintf(cfp,"set path = ($path %s/%s $LSIPKG/tools)\n",COMP_ROOT,CYGBIN);
		/* defines.mk */
		fprintf(dfp,"# tools type CYGSUN\n");
		fprintf(dfp,"TOOLSET = CYGSUN\n");
		fprintf(dfp,"AR = mips-idt-ecoff-ar cr\n");
		fprintf(dfp,"PMCC = cygnus.c\n");
		fprintf(dfp,"SPCLTOOLS = edown\n");
		fprintf(dfp,"OPTLEV = -O\n");
		fprintf(dfp,"NMOD = x\n");
		}
#endif /* Unix */
else {
	printf("BAD tools type. Exiting\n");
	exit(1);
	}

if (strequ(fpem,"STD")) {
	fprintf(dfp,"# fp emulation type STD\n");
	lstadd(pkgsw,"FLOATINGPT");
	lstadd(pkgsw,"FPEM");
	fprintf(dfp,"FPEM_DEP = $(LSIPKG)/$(FPEM_DIR)/$(ENDIAN)/lib$(NMOD)f.a\n");
	fprintf(dfp,"LIBF = -lf\n");
	}
else if (strequ(fpem,"OPT")) {
	fprintf(dfp,"# fp emulation type OPT\n");
	lstadd(pkgsw,"FLOATINGPT");
	lstadd(pkgsw,"FPEM");
	lstadd(pkgsw,"NEWFP");
	fprintf(dfp,"FPEM_DEP = $(LSIPKG)/$(FPEM_DIR)/$(ENDIAN)/lib$(NMOD)f.a\n");
	fprintf(dfp,"LIBF = -lf\n");
	}
else if (strequ(fpem,"CP1")) {
	fprintf(dfp,"# fp support via cp1\n");
	lstadd(pkgsw,"FLOATINGPT");
	}
else if (strequ(fpem,"NONE")) {
	fprintf(dfp,"# fp emulation type NONE\n");
	}
else {
	printf("%s: BAD emulation type. Exiting\n",fpem);
	exit(1);
	}
fprintf(dfp,"\n");

if (! fileExists("pmon")) goto skippmon3;

if (lststr(cpu,"LR33120") && !lststr(cpu,"LR33020")) lstadd(cpu,"LR33020");
if (lststr(cpu,"ATMIZER") && !lststr(cpu,"LR33300")) lstadd(cpu,"LR33300");
if (lststr(cpu,"LR33300") && !lststr(cpu,"LR33000")) lstadd(cpu,"LR33000");
if (lststr(cpu,"LR33050") && !lststr(cpu,"LR33000")) lstadd(cpu,"LR33000");
if (lststr(cpu,"LR33020") && !lststr(cpu,"LR33000")) lstadd(cpu,"LR33000");

skippmon3:
if (mach[0] == 0) strcpy(mach,"-DLR33000");

/* do defines.h */
if (fileExists("include/defines.h")) {
	printf("Saving include/defines.h as include/defines.old\n");
	unlink("include/defines.old"); /* for DOS */
	rename("include/defines.h","include/defines.old");
	}
hfp = fopen("include/defines.h","w");
if (hfp == 0) {
	fprintf(stderr,"can't open include/defines.h\n");
	exit(1);
	}
if (prompt) fprintf(hfp,"#define PROMPT \"%s\"\n",prompt);
if (*hostport) fprintf(hfp,"#define HOSTPORT \"%s\"\n",hostport); 
if (*dlproto) fprintf(hfp,"#define DLPROTO \"%s\"\n",dlproto);
if (memsize) fprintf(hfp,"#define MEMSIZE %ld\n",memsize);
if (clkfreq) fprintf(hfp,"#define CLKFREQ %d\n",clkfreq);
if (no_chksum) fprintf(hfp,"#define NO_CHKSUM\n");
if (strequ(nvram,"yes")) fprintf(hfp,"#define NVRAM\n");
if (strequ(ethernet,"yes")) {
	fprintf(hfp,"#define ETHERNET\n");
	fprintf(hfp,"#define ETHERADDR %s\n",etheraddr);
	}
for (i=0;pkgsw[i];i++) fprintf(hfp,"#define %s\n",pkgsw[i]);
fprintf(hfp,"#define CLIENTPC 0x%s\n",clientpc);
fprintf(hfp,"#define FTEXT 0x%s\n",ftext);
fprintf(hfp,"#define FDATA 0x%s\n",fdata);
for (i=0;cpu[i];i++) {
	fprintf(hfp,"#define %s\n",cpu[i]);
	if (strequ(cpu[i],"ATMIZER")) fprintf(hfp,"#define NON_CACHED\n");
	}

#ifdef MSDOS
/* finish up setenv.bat */
if (fileExists("pmon") && *dlport) fprintf(sfp,"set DLPORT=%s\n",dlport);
fprintf(sfp,"set MANPATH=%s/man\n",cwd);
#else
/* finish up dot.sh */
if (fileExists("pmon") && *dlport) 
	fprintf(sfp,"DLPORT=/dev/%s;export DLPORT\n",dlport);
fprintf(sfp,"MANPATH=${MANPATH=/usr/man}:%s:%s/man;export MANPATH\n",
						manpath,cwd);
/* finish up source.csh */
if (fileExists("pmon") && *dlport) 
	fprintf(cfp,"setenv DLPORT /dev/%s\n",dlport);
fprintf(cfp,"if (! $?MANPATH) setenv MANPATH /usr/man\n");
fprintf(cfp,"setenv MANPATH ${MANPATH}:%s:%s/man\n",manpath,cwd);
#endif


/* pmon/devinfo.c */
for (i=0;devinfo[i];i++) fprintf(vfp,"%s\n",devinfo[i]);
for (i=0;devtyp[i];i++) {
	if (strequ(devtyp[i],"2681")) {
		if (!strequ(devadr[i],"0")) 
		fprintf(vfp,"struct p2681info tty%ddat = {(ubyte *)%s};\n",
			i,devadr[i]);
		}
	else if (strequ(devtyp[i],"ps2"))
		fprintf(vfp,"struct ps2struct tty%ddat = {0};\n",i);
	else if (strequ(devtyp[i],"ptty"))
		fprintf(vfp,"struct pttyinfo tty%ddat = {(ubyte *)%s};\n",
			i,devadr[i]);
	}
fprintf(vfp,"ConfigEntry ConfigTable[] = {\n");
for (i=0;devtyp[i];i++) {
	if (strequ(devtyp[i],"2681")) {
		if (strequ(devadr[i],"0")) 
			fprintf(vfp,"{(Addr)&tty%ddat,0,p2681,1024,B%s},\n",
				i+1,devaux[i]);
		else
			fprintf(vfp,"{(Addr)&tty%ddat,1,p2681,1024,B%s},\n",
				i,devaux[i]);
		}
	else if (strequ(devtyp[i],"ps2"))
		fprintf(vfp,"{(Addr)&tty%ddat,0,ps2driver,1024,0},\n",i);
	else if (strequ(devtyp[i],"ptty"))
		fprintf(vfp,"{(Addr)&tty%ddat,0,ptty,1024,%s},\n",i,devaux[i]);
	}
fprintf(vfp,"{0}};\n");

fclose(dfp);
fclose(sfp);
fclose(hfp);
fclose(vfp);

#ifdef MSDOS /* ================ MSDOS ======================= */
printf("fixing up files\n");
fixupfiles();

printf("\n");
printf("To complete the installation:\n");
printf("\n");
printf("	- type \"setenv\"\n");
printf("\n");
printf("	- type \"nmake\"\n");
printf("\n");
#else /* ======================= Unix ======================= */
close(cfp);
close(rfp);
if (fileSize("root.sh") < 5) unlink("root.sh");

printf("\n");
printf("To complete the installation:\n");
printf("\n");
if (fileExists("root.sh")) printf("	- As root, type \"sh root.sh\"\n\n");
printf("	- C-shell users type \"source source.csh\"\n");
printf("	- Bourne shell users type \". dot.sh\"\n");
printf("\n");
printf("	- type \"make\"\n");
printf("\n");
#endif

}

#ifdef MSDOS
/*************************************************************
*  fixupfiles()
*/
fixupfiles()
{
int i;
FILE *ifp,*ofp;

for (i=0;fileLst[i].src;i++) {
	ifp = fopen(fileLst[i].src,"r");
	if (!ifp) {
		printf("can't open %s\n",fileLst[i].src);
		exit(1);
		}
	ofp = fopen(fileLst[i].dst,"w");
	if (!ofp) {
		printf("can't open %s\n",fileLst[i].dst);
		exit(1);
		}
	if ((* fileFixer)(ifp,ofp)) {
		printf("Fixup error in %s.\n",fileLst[i].src);
		exit(1);
		}
	fclose(ifp);
	fclose(ofp);
	}
}

/*************************************************************
*  do_algor(ifp,ofp)
*/
do_algor(ifp,ofp)
FILE *ifp,*ofp;
{
char *arlist,buf[BUFSZ],*av[AVSIZE],tmp[BUFSZ];
int ac,i;

arlist = "\tmklist -o files.lst -O ../files.mk . CFILES ASFILES\n\tar -r %s @files.lst\n";
while (fgets(buf,BUFSZ,ifp)) {
	if (buf[strlen(buf)-1] != '\n') return(2);
	buf[strlen(buf)-1] = 0;
	if (strempty(buf)) {
		fprintf(ofp,"\n");
		continue;
		}
	if (argvize(NULL,buf) > AVSIZE) return(1);
	strcpy(tmp,buf);
	ac = argvize(av,tmp);
	if (strequ(av[0],"include")) 
		fprintf(ofp,"!include \"%s\"\n",av[1]);
	else if (strequ(av[0],"$(AR)")) fprintf(ofp,arlist,av[1]);
	else if (strequ(av[0],"@cd")) {
		strcpy(tmp,buf);
		for (i=0;tmp[i];i++) if (tmp[i] == ';') tmp[i] = ' ';
		ac = argvize(av,tmp);
		fprintf(ofp,"\t@cd %s\n\t@",av[1]);
		for (i=2;i<ac;i++) {
			if (i) fprintf(ofp," ");
			if (strequ(av[i],"\\")) break;
			fprintf(ofp,"%s",av[i]);
			}
		if (strequ(av[i],"\\")) {
			fgets(buf,BUFSZ,ifp);
			fprintf(ofp,"%s",buf);
			}
		fprintf(ofp,"\n\t@cd ..\n");
		}
#if 1
	else fprintf(ofp,"%s\n",buf);
#else
	else {
		if (*buf == '\t') fprintf(ofp,"\t");
		for (i=0;i<ac;i++) {
			if (i) fprintf(ofp," ");
			fprintf(ofp,"%s",av[i]);
			}
		fprintf(ofp,"\n");
		}
#endif
	}
return(0);
}

/*************************************************************
*  do_bso(ifp,ofp)
*/
do_bso(ifp,ofp)
FILE *ifp,*ofp;
{
char *arlist,buf[BUFSZ],*av[AVSIZE],tmp[BUFSZ];
int ac,i;

arlist = "\tmklist -O ../files.mk . CFILES ASFILES|ar3 -r %s -f-\n";
while (fgets(buf,BUFSZ,ifp)) {
	if (buf[strlen(buf)-1] != '\n') return(2);
	buf[strlen(buf)-1] = 0;
	if (strempty(buf)) {
		fprintf(ofp,"\n");
		continue;
		}
	if (argvize(NULL,buf) > AVSIZE) return(1);
	strcpy(tmp,buf);
	ac = argvize(av,tmp);
	if (strequ(av[0],"include")) 
		fprintf(ofp,"!include \"%s\"\n",av[1]);
	else if (strequ(av[0],"$(AR)")) fprintf(ofp,arlist,av[1]);
	else if (strequ(av[0],"@cd")) {
		strcpy(tmp,buf);
		for (i=0;tmp[i];i++) if (tmp[i] == ';') tmp[i] = ' ';
		ac = argvize(av,tmp);
		fprintf(ofp,"\t@cd %s\n\t@",av[1]);
		for (i=2;i<ac;i++) {
			if (i) fprintf(ofp," ");
			if (strequ(av[i],"\\")) break;
			fprintf(ofp,"%s",av[i]);
			}
		if (strequ(av[i],"\\")) {
			fgets(buf,BUFSZ,ifp);
			fprintf(ofp,"%s",buf);
			}
		fprintf(ofp,"\n\t@cd ..\n");
		}
#if 1
	else fprintf(ofp,"%s\n",buf);
#else
	else {
		if (*buf == '\t') fprintf(ofp,"\t");
		for (i=0;i<ac;i++) {
			if (i) fprintf(ofp," ");
			fprintf(ofp,"%s",av[i]);
			}
		fprintf(ofp,"\n");
		}
#endif
	}
return(0);
}

/*************************************************************
*  do_epi(ifp,ofp)
*/
do_epi(ifp,ofp)
FILE *ifp,*ofp;
{
char *arlist,buf[BUFSZ],*av[AVSIZE];
int ac,i;

arlist = "\tmklist -o ../files.mk CFILES ASFILES OBJ_FILES > list.lk\n";
while (fgets(buf,BUFSZ,ifp)) {
	if (buf[strlen(buf)-1] != '\n') return(2);
	if (argvize(NULL,buf) > AVSIZE) return(1);
	ac = argvize(av,buf);
	if (strequ(av[0],"include")) 
		fprintf(ofp,"!include \"%s\"\n",av[1]);
	else if (strequ(av[0],"$(AR)")) {
		fprintf(ofp,arlist);
		fprintf(ofp,"\telib q %s @list.mk\n",av[1]);
		}
	else {
		if (*buf == '\t') fprintf(ofp,"\t");
		for (i=0;i<ac;i++) {
			if (i) fprintf(ofp," ");
			fprintf(ofp,"%s",av[i]);
			}
		fprintf(ofp,"\n");
		}
	}
return(0);
}

do_ghs(ifp,ofp)
FILE *ifp,*ofp;
{
return(0);
}
#endif /* !MSDOS */

/*************************************************************
*  int argvize(av,s) 
*	place address of each word in s into the array av. If av
*	is NULL, just return the number of words found.
*/
int argvize(av,s)
char *av[];
char *s;
{
char **pav = av;
int ac;

for (ac=0;;ac++) {
	/* step over cntrls and spaces */
	while(*s && *s <= ' ') ++s;

	/* if eos quit */
	if(!*s) break;

	/* find end of word */
	if (pav) *pav++ = s;
	while(' ' < *s) ++s;

	/* not eos inc ptr */
	if(*s && pav) *s++ = 0;
	}
return(ac);
}

/*************************************************************
*  fileSize(p)
*/
fileSize(p)
char *p;
{
struct stat st;

if (stat(p,&st)) return(0);
return(st.st_size);
}

/*************************************************************
*  fileExists(p)
*/
fileExists(p)
char *p;
{
struct stat st;

if (stat(p,&st)) return(0);
return(1);
}

/*************************************************************
*  strempty(p) 
*	returns 1 if p contains nothing but isspace 
*/
strempty(p)
char *p;
{

if (!p) return(1);
for (;*p;p++) if (!isspace(*p)) return(0);
return(1);
}

/*************************************************************
*  copy(src,dst)
*	copy file
*/
copy(src,dst)
char *src,*dst;
{
FILE *sfp,*dfp;
int c;

sfp = fopen(src,"rb");
if (sfp == 0) {
	fprintf(stderr,"can't open %s\n",src);
	exit(1);
	}
dfp = fopen(dst,"wb");
if (dfp == 0) {
	fprintf(stderr,"can't open %s\n",dst);
	fclose(sfp);
	exit(1);
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
*  lststr(lst,str)
*	find a str in a list
*/
char *lststr(lst,str)
Lst *lst;
char *str;
{
char **p;

for (p=lst;*p;p++) {
	if (strequ(*p,str)) return(*p);
	}
return(0);
}

/*************************************************************
*  lstnstr(lst,str,n)
*	find a str of length n in a list
*/
char *lstnstr(lst,str,n)
Lst *lst;
char *str;
int n;
{
char **p;

for (p=lst;*p;p++) {
	if (!strcmp(*p,str,n)) return(*p);
	}
return(0);
}

/*************************************************************
*  lstadd(lst,str)
*	add a str to a list
*/
lstadd(lst,str)
Lst *lst;
char *str;
{
char **p;

for (p=lst;*p;p++) ;
*p = (char *)malloc(strlen(str)+1);
strcpy(*p,str);
p++;
*p = 0;
}

/*************************************************************
*  lstdel(lst)
*	delete all the items in a list
*/
lstdel(lst)
Lst *lst;
{
int i;

for (i=0;lst[i];i++) free(lst[i]);
lst[0] = 0;
}

/*************************************************************
*  backtick(dst,cmd)
*	places the result of command execution into dst
*/
backtick(dst,cmd)
char *dst,*cmd;
{
char *tname = "instalXXXXXX";
char tmp[200],*p;
FILE *fp;
int c;

mktemp(tname);
sprintf(tmp,"%s > %s",cmd,tname);
system(tmp);

fp = fopen(tname,"r");
if (fp == 0) {
	printf("can't open %s\n",tname);
	exit(1);
	}

p = dst;
while ((c=getc(fp)) != EOF) {
	if (c == '\n') c = ' ';
	*p++ = c;
	}
*p = 0;
fclose(fp);
unlink(tname);
}

/*************************************************************
*  int striequ(s1,s2) 
*	returns 1 if s1 matches s2 ignoring case, else 0
*/
int striequ(s1,s2)
char *s1,*s2;
{

if (!s1 || !s2) return(0);
for (;*s1;s1++,s2++) {
	if (toupper(*s1) != toupper(*s2)) return(0);
	}
if (*s2) return(0);
return(1);
}

/*************************************************************
*  Gets(p)
*	This feature permits completely automatic builds
*/
Gets(p)
char *p;
{

if (Argcnt < Argc) strcpy(p,Argv[Argcnt++]);
else gets(p);
}

#ifdef host_mips
/* the strstr() supplied with the MIPS box is broken */
/** char *strstr(p,q) returns a ptr to q in p, else 0 if not found */
char *strstr(p,q)
char *p,*q;
{
char *s,*t;

if (!p || !q) return(0);

if (! *q) return(p);
for (;*p;p++) {
	if (*p == *q) {
		t = p; s = q;
		for (;*t;s++,t++) {
			if (*t != *s) break;
			}
		if (! *s) return(p);
		}
	}
return(0);
}

#endif
