/* $Id: menu.c,v 3.2.58.1 1996/03/21 23:27:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/menu.c,v $
 *------------------------------------------------------------------
 * menu.c
 *
 * April 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines for driving the diagnostic menues.
 *------------------------------------------------------------------
 * $Log: menu.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:30  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:35  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:37:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Routines for displaying menus and allowing the user to select
** menu items.
*/
#include "monitor.h"
#include "menu.h"
#include "error.h"

extern int netflashbooted;

/*
** Continuously display the menu prompt and call the corresponding
** menu function (mfunc).  The menu selections are boundary checked.
** The user may display the menu by entering a <CR> and may return
** to the calling procedure by typing 'x' or <break>.
*/
void
menu(struct menuinfo *menuptr, char menucmd)
{
    int c;
    struct mitem *miptr;
    struct nvram *nvptr = NVRAM;
    unsigned char continuous;
    unsigned char dismen = 1;
    jmp_buf *savjmp = monjmpptr;  /* save original jmpbuf */
    jmp_buf menujmp;
    char linebuf[80];
    int i;

    while(1) {  /* do until user says to quit */
	if(menucmd) {
	    c = menucmd;
	    menucmd = 'x';  /* exit after this one command */
	} else {
	    if(dismen) {
		displaymenu(menuptr);
		dismen = 0;
	    }
	    putchar('\n');
	    if(menuptr->mprompt) puts(menuptr->mprompt);
	    else {
		puts("enter ");
		printf(menuptr->mtitle, menuptr->mtparam);
		puts(" item > ");
	    }
	    i = getline(linebuf, sizeof(linebuf));
	    if ( i == (sizeof(linebuf) - 1) )
		continue;
	    if ( !strlen(linebuf) ) {
		dismen = 1;
		continue;
	    }
	    for (i=0; ; i++) {
		c = linebuf[i];
		if ( !c ) {
		    dismen = 1;
		    break;
		} else if ( c != ' ' && c != '\t' )
		    break;
	    }
	    if ( dismen )
		continue;
	}
	if(c == 'x') return;  /* 'x' (exit) entered, return to caller */
	if(c >= 'A' && c <= 'Z') {
	    flags(c);
	    dismen = 1;
	    continue;
	}
	c -= 'a';  /* adjust char to a menu index */
	miptr = menuptr->miptr;  /* point to first menu item structure */
	miptr += c;  /* increment to desired structure */
	
	/*
	 * Boundary check the selection and ensure that this item
	 * is valid when running net booted
	 */
	if( (c < 0 || c >= menuptr->msize) || 
	    (miptr->mixfunc && !(*miptr->mixfunc)(miptr->mixparam)) ||
	    (netflashbooted && (miptr->mflag & MF_NOTNET) ) ) {
	    puts("illegal menu item...try again\n");
	    continue;
	}
	dismen = 0;
	errcount = 0;  /* initialize */
	err_accum = 0;  /* initialize */
	testpass = 0;
	hkeepflags &= ~H_USRINT;
	initsigs();
	if(miptr->mflag & MF_CONTINUOUS) {  /* test can be run continuously */
	    continuous = (DIAGFLAG & D_CONTINUOUS);
	    if(continuous) testpass = 1;
	} else {
	    continuous = 0;
	}
	strcpy(testnamebuf,"???");  /* initialize */
	switch(setjmp(menujmp)) {
	case 0:
	    monjmpptr = &menujmp; /* redirect breaks */
	    while(1) {
		if(miptr->mfunc) (*miptr->mfunc)(*miptr->mfparam,0);
		if(!continuous) break;
		testpass++;  /* increment our pass count */
	    }
	    /* fall through */
	default:  /* longjmp(menujmp,X) */
	    monjmpptr = savjmp;  /* redirect breaks back */
	}
    }
}

/*
** Display the menu for the user.  Menu items are numbered (lettered)
** automatically starting with the letter 'a'.
*/
void
displaymenu(struct menuinfo *menuptr)
{
    register i, mletter = 'a';  /* menu item letters start with 'a' */
    register struct mitem *miptr = menuptr->miptr;  /* pointer to first mitem */

    /* display the title for the user */
    puts("\n\n      ");  /* indent it */
    printf(menuptr->mtitle, menuptr->mtparam);
    putchar('\n');
    for(i=0; i < menuptr->msize; i++, miptr++, mletter++) {
	if ( !netflashbooted || !(miptr->mflag & MF_NOTNET) ) {
	    if ( !(miptr->mixfunc) || (*miptr->mixfunc)(miptr->mixparam) ) {
		printf("%c: ", mletter);
		if(miptr->mline) printf(miptr->mline, miptr->mlparam);
		if(miptr->mlfunc) (*miptr->mlfunc)(miptr->mlparam);
		putchar('\n');
	    }
	}
    }
    if(menuptr->mtfunc) (*menuptr->mtfunc)();
}


