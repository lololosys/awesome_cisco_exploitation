/* $Id: at_debug.c,v 3.5.18.4 1996/09/11 07:26:29 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_debug.c,v $
 *------------------------------------------------------------------
 * at_debug.c -- Appletalk debugging support
 *
 * September 1992, Steven Lin
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_debug.c,v $
 * Revision 3.5.18.4  1996/09/11  07:26:29  dwong
 * CSCdi68670:  75k with low memory crashed, while booting
 * Branch: California_branch
 * Check for NULL return from malloc().
 *
 * Revision 3.5.18.3  1996/07/25  09:40:56  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.5.18.2  1996/05/02  08:39:15  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.5.18.1  1996/03/18  18:51:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.4.2  1996/03/07  08:28:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.4.1  1996/02/20  00:16:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:09:34  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/01/15  20:03:33  slin
 * CSCdi46910:  Typo in AT EIGRP
 *
 * Revision 3.3  1995/12/07  22:27:13  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  08:41:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:07:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "parser.h"
#include "atalk_private.h"

/* Forward declaration */
void atalk_debug_show_flag (atalkdebugtype const *flagp,
			    boolean always, boolean indent);

/* Global storage */
atalkdebugtype *atalknbp_debug;
atalkdebugtype *atalkpkt_debug;
atalkdebugtype *atalkrsp_debug;
atalkdebugtype *atalkrtmp_debug;
atalkdebugtype *atalkzip_debug;
static atalkdebugtype *atalkfs_debug;
atalkdebugtype *atalkerr_debug;
atalkdebugtype *appletalk_debug;
atalkdebugtype *aarp_debug;
atalkdebugtype *atalkredist_debug;
atalkdebugtype *atalkigrp_debug;
atalkdebugtype *atalklb_debug;


void
atalk_debug_command (parseinfo *csb)
{
    idbtype	*idb;

    switch (csb->which) {
      case DEBUG_APPLEEVT:
	atalk_debug_setflag(csb, appletalk_debug);
        /* only fiddle atalk_events if apple event-logging isn't in effect */
	if (!atalk_events_logged)
	    atalk_debug_setflag(csb, atalk_events);
	atalk_debug_show_flag(appletalk_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKROUTE:
	atalk_debug_setflag(csb, atalkrtmp_debug);
	atalk_debug_show_flag(atalkrtmp_debug, TRUE, FALSE);
	atalk_debug_setflag(csb, atalkigrp_debug);
	atalk_debug_show_flag(atalkigrp_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKRSP:
	atalk_debug_setflag(csb, atalkrsp_debug);
	atalk_debug_show_flag(atalkrsp_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKRTMP:
	atalk_debug_setflag(csb, atalkrtmp_debug);
	atalk_debug_show_flag(atalkrtmp_debug, TRUE, FALSE);
	break;

      case DEBUG_ZIP:
	atalk_debug_setflag(csb, atalkzip_debug);
	atalk_debug_show_flag(atalkzip_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKPKT:
	atalk_debug_setflag(csb, atalkpkt_debug);
	atalk_debug_show_flag(atalkpkt_debug, TRUE, FALSE);
	break;

      case DEBUG_APPLEERRS:
	atalk_debug_setflag(csb, atalkerr_debug);
	atalk_debug_show_flag(atalkerr_debug, TRUE, FALSE);
	break;

      case DEBUG_APPLEARP:
	atalk_debug_setflag(csb, aarp_debug);
	atalk_debug_show_flag(aarp_debug, TRUE, FALSE);
	break;

      case DEBUG_NBP:
	atalk_debug_setflag(csb, atalknbp_debug);
	atalk_debug_show_flag(atalknbp_debug, TRUE, FALSE);
	break;

    case DEBUG_ATALKREDIST:
	atalk_debug_setflag(csb, atalkredist_debug);
	atalk_debug_show_flag(atalkredist_debug, TRUE, FALSE);
	break;

      case DEBUG_ATALKFS:
	atalk_debug_setflag(csb, atalkfs_debug);
	atalk_debug_show_flag(atalkfs_debug, TRUE, FALSE);
	/*
	 * NB: While we use the debug flag to display the current state,
	 * what we really check in the f/s function is the AT_FS_DEBUG_FLAG
	 * bit in the idb->atalk_compat field.
	 */
	if (GETOBJ(int,1) == ATALK_DEBUG_INTERFACE) {
	    idb = GETOBJ(idb,1);
	    if (csb->sense)
		idb->hwptr->atalk_compat_flags |= AT_FS_DEBUG_FLAG;
	    else
		idb->hwptr->atalk_compat_flags &= ~AT_FS_DEBUG_FLAG;
	}
	break;


    case DEBUG_ATALK_LB :
	atalk_debug_setflag(csb, atalklb_debug);
	atalk_debug_show_flag(atalklb_debug, TRUE, FALSE);
	  break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;

    }
}

/*
 * atalk_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the AppleTalk debug flags at once.
 */
void
atalk_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    atalk_events->flag = num;
    atalknbp_debug->flag = num;
    atalkrsp_debug->flag = num;
    atalkrtmp_debug->flag = num;
    atalkzip_debug->flag = num;
    appletalk_debug->flag = num;
    atalkpkt_debug->flag = num;
    atalkerr_debug->flag = num;
    aarp_debug->flag = num;
    atalkredist_debug->flag = num;
    atalkigrp_debug->flag = num;
    atalkfs_debug->flag = num;
    atalklb_debug->flag = num;
}

/*
 * atalk_debug_show
 * Implements the "show debug" command for AppleTalk flags.
 */
static atalkdebugtype **const atalk_flagstoshow[] = {
	&appletalk_debug,
	&atalknbp_debug,
	&atalkrsp_debug,
	&atalkrtmp_debug,
	&atalkzip_debug,
	&atalkpkt_debug,
	&atalkfs_debug,
	&atalkerr_debug,
	&aarp_debug,
	&atalkredist_debug,
	&atalkigrp_debug,
	&atalklb_debug,
};
static const int atalk_nflagstoshow =
    sizeof(atalk_flagstoshow) / sizeof (atalkdebugtype **);

void
atalk_debug_show(void)
{
    int i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < atalk_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*atalk_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*atalk_flagstoshow[i])->flag != 0)))) {
	    printf("\nAppletalk:");
	    hdisplayed = TRUE;
	}
	atalk_debug_show_flag(*atalk_flagstoshow[i], FALSE, TRUE);
    }
}

/* 
 * Allocate space for all the debugging flags and attach descriptions 
 */
static void
atalk_debug_create_flag (atalkdebugtype **debugblock, char *name)
{
    *debugblock = malloc_named(sizeof(atalkdebugtype), "AT Debug");
    if (*debugblock)
        (*debugblock)->text = name;
}


/*
 * atalk_debug_init
 * Malloc and initialize all AppleTalk debugging flags, do general
 * AppleTalk debugging setup.
 */
void
atalk_debug_init (void)
{
    atalk_debug_create_flag(&atalk_events, "AppleTalk Event Logging");

    atalk_debug_create_flag(&atalknbp_debug, "AppleTalk NBP packets");

    atalk_debug_create_flag(&atalkrsp_debug, "AppleTalk Responder events");

    atalk_debug_create_flag(&atalkrtmp_debug, "AppleTalk RTMP routing");

    atalk_debug_create_flag(&atalkzip_debug, "AppleTalk ZIP Packets");

    atalk_debug_create_flag(&appletalk_debug, "AppleTalk Events");

    atalk_debug_create_flag(&atalkpkt_debug, "AppleTalk packets");

    atalk_debug_create_flag(&atalkfs_debug, "AppleTalk fast-switching");

    atalk_debug_create_flag(&atalkerr_debug, "AppleTalk packet errors");

    atalk_debug_create_flag(&aarp_debug, "AppleTalk ARP");

    atalk_debug_create_flag(&atalktrue_debug, "");

    atalk_debug_create_flag(&atalkfalse_debug, "");

    atalk_debug_create_flag(&atalkredist_debug, "AppleTalk Route Redistribution");

    atalk_debug_create_flag(&atalkigrp_debug, "AppleTalk EIGRP routing");

    atalk_debug_create_flag(&atalklb_debug, "AppleTalk load-balancing");
    
    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(atalk_debug_all, "atalk_debug_all");
    reg_add_debug_show(atalk_debug_show, "atalk_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    atalk_debug_all(debug_all_p());
}

/*
 * atalk_debug_setflag
 * Set debugging variable, flagp, according to parsed commands in csb.  
 * If "no" processing was
 * specified (i.e. csb->sense == 0), then the bit vector flagp->flag is
 * cleared.  Else the appropriate bit flag in flagp->flag is set according
 * to GETOBJ(int, 1).  
 */
void
atalk_debug_setflag (parseinfo const *csb, atalkdebugtype *flagp)
{
    boolean dodefault = TRUE;
    
    if ((csb == NULL) || (flagp == NULL))
	return;

    flagp->flag = 0;
    flagp->list.idb = NULL;
    flagp->list.next = NULL;
    flagp->level = 0;
    
    if (csb->sense) {
	if (GETOBJ(idb, 1)) { /* Restrict debugging to interface */
	    flagp->flag |= ATALK_INTERFACE_MASK;
	    flagp->list.idb = GETOBJ(idb, 1);
	    flagp->list.next = NULL;
	    dodefault = FALSE;
	}	
	if (GETOBJ(int, 2)) {	/* Set debugging level */
	    flagp->flag |= ATALK_LOGLEVEL_MASK;
	    flagp->level = GETOBJ(int, 2);
	    dodefault = FALSE;
	}
	if (dodefault == TRUE)
	    flagp->flag = -1;
    }
}

/*
 * at_debug
 * Indicates whether calling routine should display debugging information.
 * The decision is based on the arguments, which are supplied in
 * the following order:
 *    1) debugging flag
 *    2) bitvector which describes the arguments that follow
 *    3) variable list of arguments described in 2).
 * Each non-zero bit in 2) indicates the presence of its associated argument
 * in 3).  Arguments in 3) are ordered, with arguments associated with the
 * least significant bits in 2) appearing first.
 *
 * NOTE: IF ARGUMENT 1) IS NON-NULL THEN ARGUMENT 2) MUST APPEAR!  IN
 * ADDITION, FOR EACH NON-ZERO BIT IN ARGUMENT 2), THERE MUST BE AN EQUAL
 * NUMBER OF SUITABLE ARGUMENTS PROVIDED IN 3).
 *
 * Each argument in 3) is checked against the restrictions imposed by the
 * debugging flag.  If any argument fails to pass the restriction, then
 * we return FALSE.  Otherwise, if all arguments in 3) pass the restrictions
 * then we return TRUE.
 */
boolean
at_debug (atalkdebugtype const *debugsym, int bitvector, ...)
{
    va_list argp;
    idbtype *idbp;
    int flag;
    int type;
    int level;
    
    if (debugsym == NULL) {
	return (FALSE);
    }
    flag = debugsym->flag;
    if (flag == 0) {			/* debugging flag is off */
	return (FALSE);
    }
    if (flag == -1) {			/* debugging flag is set to */
	if (bitvector & ATALK_LOGLEVEL_MASK)
	    return (FALSE);
	else
	    return (TRUE);
    }
    
    va_start(argp, bitvector);
    
    while (bitvector) {
	type = bitvector & -bitvector; /* pull off least-significant bit */
	switch (type) {
	    /* non-zero bit */
	case ATALK_INTERFACE_MASK:
	    if (debugsym->flag & type) {
		idbp = va_arg(argp, idbtype *);
		if (!idbp || idbp->if_number != debugsym->list.idb->if_number) {
		    va_end(argp);
		    return (FALSE);
		}
	    }
	    break;
	case ATALK_LOGLEVEL_MASK:
	    if (debugsym->flag & type) {
		level = va_arg(argp, int);
		if (debugsym->level < level) {
		    va_end(argp);
		    return (FALSE);
		}
	    } else
		return (FALSE);
	    break;
		
	default:
	    buginf("Undefined argument in at_debug()\n");
	}
	bitvector ^= type;		/* turn off least-significant */
    }					/* non-zero bit */
    va_end(argp);
    return (TRUE);
}

/*
 * atalk_debug_show_flag
 * Outputs to the display the status of debugging flag, flagp (i.e. on or off).
 * If the debugging flag has special restrictions imposed (i.e. interface-
 * specific debugging), then those are indicated.
 */
void
atalk_debug_show_flag (atalkdebugtype const *flagp,
		       boolean always, boolean indent)
{
    int flag;
    int type;

    if (flagp == NULL)
	return;

    flag = flagp->flag;

    if (!always &&
	((debug_all_p() && (flag == -1)) ||
	 (!debug_all_p() && (flag == 0))))
	return;

    printf("\n%s%s debugging is %s", indent ? "  " : "",
	   flagp->text, (flag ? "on" : "off"));

    if (flag == -1)
	return;

    while (flag) {
	switch (type = flag & -flag) {	/* pull off least significant */
					/* non-zero bit */
	  case ATALK_INTERFACE_MASK:
	    printf(" for interface %s", flagp->list.idb->namestring);
	    break;
	    
	case ATALK_LOGLEVEL_MASK:
	    printf(" at level %d", flagp->level);
	    break;
	    
	  default:
	    buginf("\nUnknown command in atalk_debug_show_flag()");
	}
	flag ^= type;			/* turn off least significant */
					/* non-zero bit */
    }
}
