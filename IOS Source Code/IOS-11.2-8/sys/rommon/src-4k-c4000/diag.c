/* $Id: diag.c,v 3.3.56.1 1996/03/21 23:30:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/diag.c,v $
 *------------------------------------------------------------------
 * diag.c
 *
 * June 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Diagnostic Menus for Sierra.
 *------------------------------------------------------------------
 * $Log: diag.c,v $
 * Revision 3.3.56.1  1996/03/21  23:30:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/11  19:14:52  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:02  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1995/11/27  21:52:36  enf
 * CSCdi44486:  Warnings when building c4500 ROMMON image
 *
 * Revision 3.2  1995/11/17  18:45:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/17  01:01:15  clev
 * CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
 *
 * Revision 2.1  1995/06/07  22:40:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** The diagnostic menu structures and diag routine.
*/
#include "monitor.h"
#include "menu.h"
#include "signal_4k.h"
#include "error.h"

typedef int (*PFI)();  /* pointer to a function returning an integer */
extern int netflashbooted;

/* often used menu strings */
static char  dgmenustr[] = "Diagnostic Menu";
static char  adiagfstr[] = "alter diag flags";
static char  aparamstr[] = "alter test parameters";
static char doalldgstr[] = "do all diags in this menu";
static char dogrpdgstr[] = "do group of diags in this menu";
static char  regtststr[] = "register test";
static char  inttststr[] = "interrupt test";

/* misc. */
static long one   = 1;
static long two   = 2;
static long three = 3;

/* forward declarations */
void doalldiags(struct menuinfo *menup);
void dogrpdiags(struct menuinfo *menup);
void togglediagflag(int flagmask);
void showdflags(void);

static char togglestr[] = "toggle \"%s\" flag";
static char contstr[] = "Continuous";
static char stopstr[] = "Stop on error";
static char loopstr[] = "Loop on error";
static char quietstr[] = "Quiet mode";
static int contmask = D_CONTINUOUS, stopmask = D_STOPONERR,
  loopmask = D_LOOPONERR, quietmask = D_QUIETMODE;
static struct mitem diagflagitems[] = {
  togglestr, 0, (int)contstr, (PFI)togglediagflag, &contmask, 0, (int(*)())0, 0,
  togglestr, 0, (int)stopstr, (PFI)togglediagflag, &stopmask, 0, (int(*)())0, 0,
  togglestr, 0, (int)loopstr, (PFI)togglediagflag, &loopmask, 0, (int(*)())0, 0,
  togglestr, 0, (int)quietstr, (PFI)togglediagflag, &quietmask, 0, (int(*)())0, 0,
};
static struct menuinfo diagflag = {
  "Diagnostic Flag Menu",                      /* title */
  0,                                           /* no title param */
  (PFI)showdflags,                             /* menu title function */
  "enter item to toggle > ",                   /* our special prompt */
  sizeof(diagflagitems)/sizeof(struct mitem),  /* size of menu */
  diagflagitems,
};
static struct menuinfo *diagflagp = &diagflag;

extern int alt_mem(), dis_mem(), fil_mem(), mov_mem(), cmp_mem(), reset();
extern int memtest(), memloop(), addrloop(), memsizing(), sysmemtest(), berrscan();
extern conbrkinttest(), flash_test(), imagesumtest(), auxecho();
extern auxloopback(), berrtest(), auxinttest(), memdebug(), timertest();
extern checknims(), syspartest();
extern ck2444(), toss_cookie(), tlb_diag(), intr_test(), bev_diag();
extern int pretest_orioncache(int one);

static struct mitem utilmenuitems[] = {
    "alter memory",                0,0,alt_mem,        (int *)&one,0,(int(*)())0, 0,
    "bus error scan",              0,0,berrscan,       (int *)&one,0,(int(*)())0, 0,
    "compare memory block",        0,0,cmp_mem,        (int *)&one,0,(int(*)())0, 0,
    "display memory",              0,0,dis_mem,        (int *)&one,0,(int(*)())0, 0,
    "move memory block",           0,0,mov_mem,        (int *)&one,0,(int(*)())0, 0,
    "fill memory",                 0,0,fil_mem,        (int *)&one,0,(int(*)())0, 0,
    "memory test",                 0,0,memtest,        (int *)&one,0,(int(*)())0, 0,
    "memory read or write loop",   0,0,memloop,        (int *)&one,0,(int(*)())0, 0,
    "memory debug loop",           0,0,memdebug,       (int *)&one,0,(int(*)())0, 0,
    "address loop",                0,0,addrloop,       (int *)&one,0,(int(*)())0, 0,
    "console break interrupt test",0,0,conbrkinttest,  (int *)&one,0,(int(*)())0, 0,
    "system reset",                0,0,reset,          (int *)&one,0,(int(*)())0, 0,
    "AUX port echo test",          0,0,auxecho,        (int *)&one,0,(int(*)())0, 0,
    "poll NIMs test",              0,0,checknims,      (int *)&one,0,(int(*)())0, 0,
    "serial cookie utility",       0,0,toss_cookie,    (int *)&one,0,(int(*)())0, 0,
};
static struct menuinfo utilmenu = {
    "Diagnostic Utilities Menu",
    0,
    0,
    0,
    sizeof(utilmenuitems)/sizeof(struct mitem),
    utilmenuitems,
};
static struct menuinfo *utilmenup = &utilmenu;

extern struct menuinfo *maindiagp;
static struct mitem maindiagitems[] = {
    adiagfstr,                   0, 0, (PFI)menu,       (int *)&diagflagp, 0,(int(*)())0, 0,
    "basic utilities",           0, 0, (PFI)menu,       (int *)&utilmenup, 0,(int(*)())0, 0,
    doalldgstr,                  0, 0, (PFI)doalldiags, (int *)&maindiagp, MF_CONTINUOUS,(int(*)())0, 0,
    dogrpdgstr,                  0, 0, (PFI)dogrpdiags, (int *)&maindiagp, MF_CONTINUOUS,(int(*)())0, 0,
    "bus error test",            0, 0, berrtest,   (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "monitor image checksum test",0, 0, imagesumtest,  (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "internal interrupt test",   0, 0, intr_test,  (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "bev state test",            0, 0, bev_diag,   (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "timer interrupt test",      0, 0, timertest,  (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "size main memory",          0, 0, memsizing,  (int *)&one,       MF_DOALL | MF_NOTNET,(int(*)())0, 0,
    "main memory test",          0, 0, sysmemtest, (int *)&one,       MF_CONTINUOUS | MF_DOALL | MF_NOTNET,(int(*)())0, 0,
    "size shared memory",        0, 0, memsizing,  (int *)&two,       MF_DOALL,(int(*)())0, 0,
    "shared memory test",        0, 0, sysmemtest, (int *)&two,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "shared memory parity test", 0, 0, syspartest, (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "flash memory test",         0, 0, flash_test, (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "aux loopback test",         0, 0, auxloopback,(int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "aux port interrupt test",   0, 0, auxinttest, (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "serial cookie test",        0, 0, ck2444,     (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "data cache test",           0, 0, pretest_orioncache,(int *)&one,MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
    "tlb test",                  0, 0, tlb_diag,   (int *)&one,       MF_CONTINUOUS | MF_DOALL,(int(*)())0, 0,
};
static struct menuinfo maindiag = {
  "Main %s",                                   /* title */
  (int)dgmenustr,                              /* title param */
  (PFI)showdflags,                             /* menu title function */
  0,                                           /* generic prompt */
  sizeof(maindiagitems)/sizeof(struct mitem),  /* size of menu */
  maindiagitems,
};
static struct menuinfo *maindiagp = &maindiag;

/*
** The menu driven diagnostics are called from either the low level
** power-on routine or from the monitor.  Frommon will be non-zero if
** called from the monitor.  Note that this value is actually the
** argc parameter (a little slight of hand here), which is always
** greater than zero.
*/
void
diag_menu(int argc, char *argv[])
{
    char arg;

    if(argc > 1) arg = *argv[1];
    else arg = 0;
    (NVRAM)->pollcon = 1;            /* poll the console */
    envflag = INDIAG;                /* set the environment flag */
    menu(&maindiag, arg);
}

void
togglediagflag(int flagmask)
{
  (NVRAM)->diagflag ^= flagmask;
}

void
showdflags(void)
{
  register unsigned char flag = (NVRAM)->diagflag;
  static char onstr[] = "ON", offstr[] = "OFF";

  printf("FLAGS: %s %s  %s %s  %s %s  %s %s\n",
	 contstr, (flag & D_CONTINUOUS) ? onstr : offstr,
	 stopstr, (flag & D_STOPONERR)  ? onstr : offstr,
	 loopstr, (flag & D_LOOPONERR)  ? onstr : offstr,
	 quietstr, (flag & D_QUIETMODE)  ? onstr : offstr);
}

void
pr_err_accum(void)
{
    printf("\n%d total accumulated errors\n", err_accum);
}

void
doalldiags(struct menuinfo *menup)
{
    register char i;
    register struct nvram *nvptr = NVRAM;
    register struct mitem *miptr =  menup->miptr;
    jmp_buf *savjmp = monjmpptr;   /* save original jmpbuf */
    jmp_buf localjmp;
    int abort;

    abort = setjmp(localjmp);
    if(!abort) {
	monjmpptr = &localjmp; /* redirect breaks */
	for(i=0; i<menup->msize; i++, miptr++) {

	    /* 
	     * Check that if NetBooted, MF_NOTNET not set and
	     * the the MF_DOALL flag is set
	     */
	    if( (!netflashbooted || !(miptr->mflag & MF_NOTNET) ) &&
	        (miptr->mflag & MF_DOALL) ) {
		/* Check for existance of the menu item (i.e. interface) */
		if ( !(miptr->mixfunc) || (*miptr->mixfunc)(miptr->mixparam) ) {
		    (*miptr->mfunc)(*miptr->mfparam);
		    err_accum += errcount;
		    errcount = 0;
		}
	    }
	}
    }
    pr_err_accum();
    monjmpptr = savjmp;  /* redirect breaks back */
    if(abort && savjmp) longjmp(savjmp,1);
}

void
dogrpdiags(struct menuinfo *menup)
{
    register char i, menuitem;
    register struct mitem *miptr;
    register struct nvram *nvptr = NVRAM;
    jmp_buf *savjmp = monjmpptr;  /* save original jmpbuf */
    jmp_buf localjmp;
    char numitems;
    char buffer[sizeof(nvptr->diaglist)];
    int abort;

    /*
    ** Prompt the user for the items to include in the group test.
    ** If the tests are to be run continuously prompt only on the
    ** first pass.
    */
    if(!(nvptr->diagflag & D_CONTINUOUS) || testpass == 1) {
	displaymenu(menup);
	printf("\nenter the menu items to include in test (in order) "
	       "followed by <RET>\nor just <RET> to re-execute current "
	       "list [%s]: ", nvptr->diaglist);
	getline(buffer, sizeof(buffer));
	if(*buffer) strcpy(nvptr->diaglist, buffer);  /* user entered a list */
    }
    numitems = strlen(nvptr->diaglist);
    abort = setjmp(localjmp);
    if(!abort) {
	monjmpptr = &localjmp; /* redirect breaks */
	for(i=0; i<numitems; i++) {
	    menuitem = (nvptr->diaglist[i] - 'a');
	    miptr = &menup->miptr[menuitem];

	    /*
	     * Check that item is in the list and is valid
	     * even if the image is net booted
	     */
	    if( (netflashbooted && (miptr->mflag & MF_NOTNET) ) ||
	        (menuitem < 0) ||
	        (menuitem >= menup->msize) ) {
		continue;
	    }
	    if(miptr->mflag & MF_DOALL) {  /* check to make sure we can */
		(*miptr->mfunc)(*miptr->mfparam);
		/* Check for existance of the menu item (i.e. interface) */
		if ( !(miptr->mixfunc) || (*miptr->mixfunc)(miptr->mixparam) ) {
		    err_accum += errcount;
		    errcount = 0;
		}
	    }
	}
    }
    pr_err_accum();
    monjmpptr = savjmp;  /* redirect breaks back */
    if(abort && savjmp) longjmp(savjmp,1);
}

int
flags(int c)
{
  int mask;

  switch(c) {
  case 'C': mask = D_CONTINUOUS; break;
  case 'L': mask = D_LOOPONERR; break;
  case 'Q': mask = D_QUIETMODE; break;
  case 'S': mask = D_STOPONERR; break;
  default: puts("unknown flag\n"); return(-1);
  }
  togglediagflag(mask);
  return(0);
}
