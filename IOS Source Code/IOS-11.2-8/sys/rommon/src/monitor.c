/* $Id: monitor.c,v 3.2.58.2 1996/07/08 22:42:49 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/monitor.c,v $
 *------------------------------------------------------------------
 * monitor.c
 *
 * July 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The monitor command driven user interface.
 *------------------------------------------------------------------
 * $Log: monitor.c,v $
 * Revision 3.2.58.2  1996/07/08  22:42:49  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:27:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/08  04:58:52  mbeesley
 * Branch: ELC_branch
 * Remove password protection on priv.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:39  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "fsio.h"
#include "fstat.h"
#include "monlib.h"
#include "sh.h"
#include "error.h"
#include "proto.h"
#include "dnld.h"
#include "menu.h"
#include "error.h"
#include "flash.h"
#include "testmem.h"
#include "dev.h"
#include "confreg.h"
#include "extern.h"

#define CMD_LEN 32

volatile unsigned char envflag, hkeepflags;
jmp_buf monjmpbuf, *monjmpptr;

static struct cmdhist cmdhist[NUMMONHIST];
static struct cmdhist *curhistptr;   /* current history slot */
static unsigned short monhistcount;  /* monitor valid history count */


/* forward declarations */
int history(int argc, char *argv[]), repeat(int argc, char *argv[]);
int help(void);
struct cmdhist *check_chp(struct cmdhist *chp);


/*
** Setup some variables (in the text segment) to allow run from
** ROM and run from RAM with no code changes
*/
int netflashbooted = 0;
int netboot_memorysize = 0;

static char *repptr;
static long repcnt;
jmp_buf monjmp;
static struct cmdhist *getlohist(), *gethistptr();
static char *monrc = "$MONRC";

/*
**  Get a command line from the user and maintain the history mechanism.
**  The monitor prompt is taken from the monitor "shell" variable PS1 if
**  it exists.  If this variable contains a '!' character the history count
**  will replace this in the prompt.
*/
void
monitor(int reset)
{
    int i;
    char c, *strptr;
    struct shstuff shstuff;
    struct shstuff *shp = &shstuff;
    struct cmdhist *chp;
    struct nvram *nvptr = NVRAM;


    envflag = INMON;
    bzero(shp, sizeof(struct shstuff));
    if(reset) {
	printf("%s processor with %d Kbytes of main memory\n\n",
	       platform_string, (sizemainmem()/1024));
	if(!setjmp(monjmp)) {           /* allow for <break> */
	    monjmpptr = &monjmp;
	    if((shp->cmdptr = getvar(nvptr->evartbl, monrc+1))) {
		shcmdline(shp);             /* execute it */
	    }
	}
	if(!setjmp(monjmp))   /* allow for <break> */
	    autoboot();
	monjmpptr = 0;
    }

    chp = curhistptr;  /* get pointer from last execution of monitor */
    if(chp) {  /* something in history */
	if(reset && (i = monhistcount) > NUMMONHIST) {
	    /*
	     ** Renumber the items in the history buffer and reset
	     ** monhistcount.  Strange things will happen if monhistcount
	     ** is allowed to roll over to zero.
	     */
	    monhistcount = i = (NUMMONHIST - 1);
	    while(i) {
		if(--chp < cmdhist)
		    chp = &cmdhist[NUMMONHIST - 1];  /* wraparound */
		chp->histnum = --i;
	    }
	    chp = curhistptr;  /* get pointer again */
	}
    } else monhistcount = 1;  /* nothing in history, init this */

    while(1) {  /* prompt for commands forever & do all history slots */
	envflag = INMON;
	chp = check_chp(chp);         /* init or wraparound */
	curhistptr = chp;             /* update for next execution of monitor */
	chp->histnum = monhistcount;  /* fill in history number */

	/* in case user <breaks> from entering command line */
	if(setjmp(monjmp)) putchar('\n');
	monjmpptr = &monjmp;
	if((strptr = getvar(nvptr->evartbl,"PS1")) == (char *)0)
	    strptr = "$$ ";  /* if PS1 is not set, use this */
	while(c = *strptr++) {  /* prompt the user */
	    if(c == '!') printf("%d",monhistcount);  /* print hist num */
	    else putchar(c);
	}
	switch(getline(chp->cmdbuf,MONLINESIZ)) {
	case -1:  /* buffer overflow, do not execute command */
	    puts("command NOT executed\n");
	    continue;
	case 0:   /* empty buffer, reuse this line */
	    continue;
	default:
	    repcnt = 0;
	    shp->cmdptr = chp->cmdbuf;  /* set parser pointer to our command */

	    /*
	    ** The following 4 lines were added for those commands that do not
	    ** return from shcmdline (launch, step, and cont).  This insures that
	    ** these commands get accounted for in the history mechanism.
	    */
	    curhistptr = check_chp(chp+1);  /* update for next execution of monitor */
	    curhistptr->histnum = 0;        /* NULL history number */
	    curhistptr->cmdbuf[0] = '\0';   /* NULL command */
	    ++monhistcount;

	    shcmdline(shp);  /* parse and execute the command */

	    if ( repcnt ) {
		strcpy(chp->cmdbuf, repptr);
		printf("%s", repptr);
		if ( repcnt > 1 ) 
		    printf(", repeating %d times\n", repcnt);
		else
		    printf("\n");
		while ( repcnt > 0 ) {
		    /* set parser pointer to our command */
		    shp->cmdptr = chp->cmdbuf;
		    shcmdline(shp);  /* parse and execute the command */
		    repcnt -= 1;
		}
	    }
	    break;
	}
	chp++;
    }
}


/*
** See if history pointer is within the proper range and handle wrap
** if necessary.
*/
struct cmdhist *
check_chp(struct cmdhist *chp)
{
    if(chp < cmdhist || chp >= &cmdhist[NUMMONHIST])
	    chp = cmdhist;  /* init or wraparound */
    return(chp);
}


/*
** See if the command is valid and execute it.
*/
int
invoke(struct shstuff *shp)
{
    register i;
    register struct monitem *monptr;
    extern int optind;
    char buffer[32];
    char cmd_name[CMD_LEN];
    int retval = -1;  /* bad return value (pessimistic?) */
    char *reason_str = (char *)0;  /* init */

    /*
     * Save the command name for later use since
     * some command implementation change the 
     * command name itself 
     */
    strncpy(cmd_name, shp->argv[0], CMD_LEN - 1); 

#ifdef DEBUG
    printf("in invoke, argv[0] is \"%s\"\n",shp->argv[0]);
#endif
    /* see if this is a shell variable assignment command */
    while(strchr(shp->argv[0],'=')) {  /* do possible multiple assignments */
	retval = setvar((NVRAM)->evartbl,EVTSIZ,shp->argv[0]);
	shift(1, shp->argv);  /* shift this word out of the picture */
	if(!shp->argc) return(retval);  /* nothing left in command buffer */
    }
    initsigs();  /* initialize all signals to default */
    /* reset all bits but H_MORE, etc. */
    hkeepflags &= (H_MORE | H_PRIVILEGED | H_SCRIPT);
    optind = 0;  /* init for getopt() */
    strcpy(testnamebuf,"???");  /* initialize */
    switch(setjmp(monjmp)) { /* set up a trap door */
    case 0:
	monjmpptr = &monjmp;
	/* see if the command is a built-in (in the moncmd list) */
	for(i=0, monptr=moncmd; i<moncmdsiz; i++, monptr++) {
	    if(monptr->flags & MF_PRIVILEGED && !(hkeepflags & H_PRIVILEGED))
		continue;
	    if(strcmp(shp->argv[0],monptr->command) == 0) {  /* match */
		shp->retval = (*monptr->procedure)(shp->argc,shp->argv);
		sprintf(buffer,"?=%d",shp->retval);
		setvar((NVRAM)->evartbl,EVTSIZ,buffer);
		if(getvar((NVRAM)->evartbl,"XCODE")) {
		    sprintf(buffer,"exited with code 0x%x",shp->retval);
		    reason_str = buffer;
		}
		retval = 0;  /* a good return value */
		goto exitinvoke;
	    }
	}
	reason_str = "not found";
	break;
    case 1:
	reason_str = "aborted due to user interrupt";
	break;
    case 2:
	reason_str = "aborted due to exception";
	break;
    case 3:
	break;  /* quiet termination */
    case 4:
	reason_str = "aborted due to alarm signal";
	break;
    default:
	reason_str = "terminated - reason unknown";
	break;
    }
    shp->retval = 2;  /* command not allowed to finish, bad return value */
    /* should this be recorded in evartbl??? */
exitinvoke:
    hkeepflags &= ~H_MORE;  /* H_MORE good for one command only */
    /* if we're not in a script clear all file descriptors */
    if(!(hkeepflags & H_SCRIPT)) closeall();
    if(reason_str) {
	printf("\nmonitor: command \"%s\" %s\n",cmd_name,reason_str);
	repcnt = 0;
    }
    return(retval);
}

/*
** Print out the history list.
** Prints 1 less than NUMMONHIST including the current item.
*/
int
history(int argc, char *argv[])
{
    register i;
    register struct cmdhist *lptr;
    
#ifdef DEBUG
    printf("argc = %d\n",argc);
    for(i=0; i<argc; i++) printf("argv[%d] = \"%s\"\n",i,argv[i]);
    lptr = getlohist();  /* lowest number in history */
#endif
    lptr = curhistptr;
    putchar('\n');
    for(i=0; i < NUMMONHIST; i++) {
	if(++lptr >= &cmdhist[NUMMONHIST])
	    lptr = cmdhist;  /* wraparound */
	if(i==0) continue;  /* do not print lowest number item */
	if(lptr->histnum && lptr->cmdbuf[0])
	    printf("%d   %s\n",lptr->histnum,lptr->cmdbuf);
    }
    return(0);
}

/*
** Repeat a specified command or last command if there is no arg.
** The argument can either be a history number or a string to match.
*/
int
repeat(int argc, char *argv[])
{
    register i;
    register struct cmdhist *hptr;
    long num;

    if ( repcnt != 0 ) {
	printf("%s: can not nest calls to repeat\n", argv[0]);
	repcnt = 0;
	if(monjmpptr) longjmp(monjmpptr, 3);
	return(1);
    }

    if(monhistcount == 1) {  /* first command in history */
cmdnotfound:
	repcnt = 0;
	puts("command not found in history\n");
	return(1);
    }

    if ( argc == 1 ) {                   /* no arg means repeat last command once */
	num = monhistcount - 2;
	repcnt = 1;
findhist:
	if(!(hptr = gethistptr(num)) ||  /* valid number? */
	   hptr == curhistptr)  /* prevent recursive runaway */
	    goto cmdnotfound;
	repptr = hptr->cmdbuf;           /* set repeat pointer to this command */
	return(0);
    } else if ( argc > 3 ) {
usage:
	printf("usage: %s [histnum | matchstring] [count]\n",argv[0]);
	repcnt = 0;
	return(1);
    } else if ( getnum(argv[1],10,&num) > 0) {  /* user specified a number */
	if(!(hptr = gethistptr(num)) ||         /* valid number? */
	   hptr == curhistptr)         /* prevent recursive runaway */
	    goto cmdnotfound;
	repcnt = 1;
	if ( argc == 3 && getnum(argv[2],10,&repcnt) == 0 )
	    goto usage;
	goto findhist;
    } else {                                    /* user specified a match string */
	repcnt = 1;
	if ( argc == 3 && getnum(argv[2],10,&repcnt) == 0 )
	    goto usage;
	num = strlen(argv[1]);                  /* how many chars do we compare? */
	hptr = gethistptr(monhistcount-1);
	for(i=0; i<NUMMONHIST; i++, hptr--) {  /* search backwards thru list */
	    if(hptr < cmdhist)
		hptr = &cmdhist[NUMMONHIST-1];    /* wraparound */
	    if(strncmp(argv[1],hptr->cmdbuf,num) == 0) {   /* match */
		repptr = hptr->cmdbuf;
		return(0);
	    }
	}
	goto cmdnotfound;
    }
}
    
/*
** Return the pointer of the history item with histnum == num.
*/
static struct cmdhist *
gethistptr(int num)
{
    register struct cmdhist *chp = cmdhist;  /* init */
  
    while(1) {
	if(num == chp->histnum) return(chp);
	if(++chp >= &cmdhist[NUMMONHIST]) break;
    }
    return((struct cmdhist *)0);
}

int
help(void)
{
    register int i, numchars;
    register struct monitem *monptr;
    
    for(i=0, monptr=moncmd; i<moncmdsiz; i++, monptr++) {
	/* must be privileged to view privileged commands */
	if(monptr->flags & MF_PRIVILEGED && !(hkeepflags & H_PRIVILEGED))
	    continue;
	/* hidden commands never get displayed */
	if(monptr->flags & MF_HIDDEN) continue;
	numchars = printf("%s  ",monptr->command);
	while(numchars++ < 20) putchar(' ');
	printf("%s\n",monptr->description);
    }
    return(0);
}

int
priv(int argc, char *argv[])
{
    char buffer[5];
    char c;
    int i;
    unsigned long chksum, uchksum;
    unsigned short *cptr;

#if !defined(RSP) && !defined(RSP2) && !defined(PREDATOR) && !defined(SVIP)
    puts("Password: ");
    for(i=0; ; i++) {
	c = getchar();
	if(c == '\r') {
	    putchar('\n');
	    c = '\0';
	}
	if(i < sizeof(buffer)) buffer[i] = c;
	if(!c) break;
    }
    if(buffer[sizeof(buffer) - 1] != '\0') return(1);  /* fail password */
    cptr = (unsigned short *)get_cookie();
    chksum = 0;
    for(i=0; i<5; i++) chksum += *cptr++; /* add up the first 5 words */
#ifdef MAKEHARDER
    chksum = (~(chksum - 1) & 0xffff);    /* 2's compliment of num */
#else
    chksum &= 0xffff;
#endif
    if(getnum(buffer, 16, &uchksum) != (sizeof(buffer) - 1)) return(1);
#ifdef DEBUG
    printf("chksum: %#x,  uchksum: %#x\n", chksum, uchksum);
#endif
    if(chksum != uchksum) return(1);
#endif
    printf("You now have access to the full set of monitor commands.\n"
           "Warning: some commands will allow you to destroy your\n"
           "configuration and/or system images and could render\n"
	   "the machine unbootable.\n");
    hkeepflags |= H_PRIVILEGED;
    return(0);
}

/*
 * return_to_roms
 */

int 
return_to_roms(void)
{
    do_reset();
}

int
rom_reload(void)
{
    for (;;) {
	return_to_roms();
    }
}
