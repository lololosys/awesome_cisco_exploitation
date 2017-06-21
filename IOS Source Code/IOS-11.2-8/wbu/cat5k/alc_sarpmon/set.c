/* $Id: set.c,v 1.1.4.1 1996/05/09 15:00:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/set.c,v $
 *------------------------------------------------------------------
 * set.c -- PMON source.
 *
 * 
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: set.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:14  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:10  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <string.h>
#include <defines.h>
#include "pmon.h"

#ifdef MIPSTOOLS
extern char _ftext,etext,_fdata,edata;
#endif


#ifndef PROMPT
#define PROMPT "PMON> "
#endif

#ifndef DLECHO
#define DLECHO "off"
#endif

struct envstr {
	char *name;
	char *init;
	char *value;
	char *values;
	};

struct envstr envvar[] = {
	{"brkcmd","l @epc 1",0,0},
	{"datasz","-b",0,"-b -h -w"},
	{"dlecho",DLECHO,0,"off on lfeed"},
	{"dlproto",DLPROTO,0,"none XonXoff EtxAck"},
	{"hostport",HOSTPORT,0,0},
	{"inalpha","hex",0,"hex symbol"},
	{"inbase","16",0,"auto 8 10 16"},
	{"moresz","10",0,0},
	{"prompt",PROMPT,0,0},
	{"regstyle","sw",0,"hw sw"},
	{"rptcmd","trace",0,"off on trace"},
	{"trabort","^K",0,0},
	{"uleof","%",0,0},
	{"ulcr","off",0,"off on"},
	/* Ratinder: changed etext to _etext {"validpc","_ftext etext",0,0},*/
	{"validpc","_ftext _etext",0,0},
	{"heaptop",0,0,0},
#ifdef ETHERNET
	{"etheraddr","08:00:69:03:00:00",0,0},
#endif
	{0}};

#define  RX_PROMPT  "RX_SAR> "
#define  TX_PROMPT  "TX_SAR> "

/*************************************************************
*  envinit()
*/
envinit()
{
int i;
char buf[LINESZ];
int rd_val;

/* set defaults */
for (i=0;envvar[i].name != 0;i++) {
	if (envvar[i].init)
	{
	   if (strequ(envvar[i].init,PROMPT))
      {
			rd_val = *(int *)0xA0E80000;
			if (!(rd_val & 0x03))
				envvar[i].init = RX_PROMPT;
			else
				envvar[i].init = TX_PROMPT;
		}
		sprintf(buf,"set %s '%s'",envvar[i].name,envvar[i].init);
		do_cmd(buf);
	}
}
sprintf(buf,"set heaptop %08x",CLIENTPC);
do_cmd(buf);

newsym("Pmon",FTEXT);
newsym("start",CLIENTPC);
#ifdef MIPSTOOLS
newsym("free",(&etext-&_ftext)+(&edata-FDATA));
#endif
}

/*************************************************************
*  do_set(ac,av)
*/
do_set(ac,av)
int ac;
char *av[];
{
int i;

#ifdef NVRAM
if (ac >= 2) {
	if (strequ(av[1],"nvram")) {
		if (nvCheck()) set_nvram(ac-2,&av[2]);
		else printf("nvram not supported\n");
		return;
		}
	}
#endif
		
switch (ac) {
	case 1 : /* display all variables */
		for (i=0;envvar[i].name;i++) {
			printvar(i);
			}
		break;
	case 2 : /* display specific variable */
		for (i=0;envvar[i].name;i++)
			if (strequ(av[1],envvar[i].name)) break;
		if (envvar[i].name) printvar(i);
		else printf("%s: not found\n",av[1]);
		break;
	case 3 : /* set specific variable */
		for (i=0;envvar[i].name;i++)
			if (strequ(av[1],envvar[i].name)) break;
		if (envvar[i].name) {
			if (envvar[i].value) free(envvar[i].value);
			envvar[i].value = malloc(strlen(av[2])+1);
			strcpy(envvar[i].value,av[2]);
			}
		else printf("%s: not found\n",av[1]);
		break;
	}
}

/*************************************************************
*  printvar(i)
*/
printvar(i)
int i;
{

if (envvar[i].value == 0) 
	printf("%10s = %-10s",envvar[i].name,"UNDEFINED");
else if (strchr(envvar[i].value,' ')) 
	printf("%10s = \"%s\"",envvar[i].name,envvar[i].value);
else 
	printf("%10s = %-10s",envvar[i].name,envvar[i].value);
if (envvar[i].values) printf("  [%s]",envvar[i].values);
printf("\n");
}

/*************************************************************
*  char *getenv(name) 
*	returns pointer to current value of named variable 
*/
char *getenv(p)
char *p;
{
int i;

for (i=0;envvar[i].name;i++) {
	if (strequ(p,envvar[i].name)) break;
	}
if (envvar[i].name == 0) return(0);

return(envvar[i].value);
}

/*************************************************************
*  matchenv(name) 
*	returns integer corresponding to current value 
*/
matchenv(name)
char *name;
{
int i,j;
char *t,wrd[20];

for (i=0;envvar[i].name;i++)
	if (strequ(name,envvar[i].name)) break;
if (!envvar[i].name) return(-1);

if (envvar[i].value == 0) {
	envvar[i].value = malloc(2);
	strcpy(envvar[i].value,"");
	}

for (j=0,t=envvar[i].values;;j++) {
	t = getword(wrd,t);
	if (t == 0) return(-2);
	if (striequ(wrd,envvar[i].value)) return(j);
	}
}

#ifdef NVRAM
/*************************************************************
*  set_nvram(ac,av)
*/
set_nvram(ac,av)
int ac;
char *av[];
{
int i,j;
char tmp[80];
long adr;

/* write nvram */
for (i=0,adr=0;envvar[i].name;i++) {
	if (envvar[i].value == 0) continue;
	sprintf(tmp,"set %s \"%s\"\n",envvar[i].name,envvar[i].value);
	for (j=0;j<strlen(tmp);j++,adr++) nvWrite(adr,tmp[j]);
	}
/* get stty settings */
for (i=0;;i++) {
	if (getstty(tmp,i)) break;
	for (j=0;j<strlen(tmp);j++,adr++) nvWrite(adr,tmp[j]);
	}
/* get stuff from command line */
for (i=0;i<ac;i++) {
	if (i) nvWrite(adr++,' ');
	for (j=0;j<strlen(av[i]);j++,adr++) nvWrite(adr,av[i][j]);
	}
if (ac > 0) nvWrite(adr++,'\n');
nvWrite(adr,0);
}
#endif
