/* $Id: smrp_debug.c,v 3.4.18.2 1996/07/25 09:54:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_debug.c,v $
 *------------------------------------------------------------------
 * Support routines for SMRP debug commands.
 *
 * April 1995, Kent Leung
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_debug.c,v $
 * Revision 3.4.18.2  1996/07/25  09:54:05  snyder
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
 * Revision 3.4.18.1  1996/03/18  21:47:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:42:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:10:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:15:16  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1995/11/17  18:53:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:12:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:24:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/19  18:40:20  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.5  1995/08/01  00:22:41  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.4  1995/06/23  18:49:11  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/20  07:17:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:54:57  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:10  hampton
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
#include "logger.h"
#include "parser.h"
#include "../os/chunk.h"
#include "smrp_private.h"

/* Forward declaration */
void smrp_debug_show_flag(smrpdebugtype const *flagp,
			  boolean always, boolean indent);

/* Global storage */

/* NUM_OF_SMRP_DEBUGS must always equal the number of SMRP debugging flags */
enum { NUM_OF_SMRP_DEBUGS = 8 };

smrpdebugtype *smrptrue_debug;
smrpdebugtype *smrpfalse_debug;
smrpdebugtype *smrp_group_debug;
smrpdebugtype *smrp_mcache_debug;
smrpdebugtype *smrp_neighbor_debug;
smrpdebugtype *smrp_port_debug;
smrpdebugtype *smrp_route_debug;
smrpdebugtype *smrp_trans_debug;

/* Local "static" storage */
static chunk_type *debug_chunk;

void
smrp_debug_commands (parseinfo *csb)
{

    switch (csb->which) {
      case DEBUG_SMRP_ALL:
	smrp_debug_setflag(csb, smrp_group_debug);
	smrp_debug_show_flag(smrp_group_debug, TRUE, FALSE);
	smrp_debug_setflag(csb, smrp_mcache_debug);
	smrp_debug_show_flag(smrp_mcache_debug, TRUE, FALSE);
	smrp_debug_setflag(csb, smrp_neighbor_debug);
	smrp_debug_show_flag(smrp_neighbor_debug, TRUE, FALSE);
	smrp_debug_setflag(csb, smrp_port_debug);
	smrp_debug_show_flag(smrp_port_debug, TRUE, FALSE);
	smrp_debug_setflag(csb, smrp_route_debug);
	smrp_debug_show_flag(smrp_route_debug, TRUE, FALSE);
	smrp_debug_setflag(csb, smrp_trans_debug);
	smrp_debug_show_flag(smrp_trans_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_GROUP:
	smrp_debug_setflag(csb, smrp_group_debug);
	smrp_debug_show_flag(smrp_group_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_MCACHE:
	smrp_debug_setflag(csb, smrp_mcache_debug);
	smrp_debug_show_flag(smrp_mcache_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_NEIGHBOR:
	smrp_debug_setflag(csb, smrp_neighbor_debug);
	smrp_debug_show_flag(smrp_neighbor_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_PORT:
	smrp_debug_setflag(csb, smrp_port_debug);
	smrp_debug_show_flag(smrp_port_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_ROUTE:
	smrp_debug_setflag(csb, smrp_route_debug);
	smrp_debug_show_flag(smrp_route_debug, TRUE, FALSE);
	break;

      case DEBUG_SMRP_TRANSACTION:
	smrp_debug_setflag(csb, smrp_trans_debug);
	smrp_debug_show_flag(smrp_trans_debug, TRUE, FALSE);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;

    }
}

/*
 * smrp_debug_all
 * Registered to be called whenever "debug all" or "undebug all" is issued,
 * or whenever it's desirable to set all the SMRP debug flags at once.
 */
void
smrp_debug_all (boolean flag)
{
    int num;
    if (flag)
	num = -1;
    else
	num = 0;

    smrp_group_debug->flag   = num;
    smrp_mcache_debug->flag   = num;
    smrp_neighbor_debug->flag   = num;
    smrp_port_debug->flag   = num;
    smrp_route_debug->flag   = num;
    smrp_trans_debug->flag   = num;
}

/*
 * smrp_debug_show
 * Implements the "show debug" command for SMRP flags.
 */
static smrpdebugtype **const smrp_flagstoshow[] = {
	&smrp_group_debug,
	&smrp_mcache_debug,
	&smrp_neighbor_debug,
	&smrp_port_debug,
	&smrp_route_debug,
	&smrp_trans_debug,

};
static const int smrp_nflagstoshow =
    sizeof(smrp_flagstoshow) / sizeof (smrpdebugtype **);

void
smrp_debug_show(void)
{
    int i;
    boolean hdisplayed = FALSE;

    for (i = 0; i < smrp_nflagstoshow; i++) {
	if (!hdisplayed &&
	    ((debug_all_p() && ((*smrp_flagstoshow[i])->flag != -1)) ||
	     (!debug_all_p() && ((*smrp_flagstoshow[i])->flag != 0)))) {
	    printf("\nSMRP:");
	    hdisplayed = TRUE;
	}
	smrp_debug_show_flag(*smrp_flagstoshow[i], FALSE, TRUE);
    }
}

/*
 * smrp_debug_malloc_flag
 * 
 * Allocate memory to hold a debugging flag.
 */
static smrpdebugtype *
smrp_debug_malloc_flag (void)
{
    smrpdebugtype *flag;
    
    flag = chunk_malloc(debug_chunk);

    return flag;
}

/*
 * smrp_debug_init
 * Malloc and initialize all SMRP debugging flags, do general
 * SMRP debugging setup.
 */
void
smrp_debug_init (void)
{

    debug_chunk = chunk_create(sizeof(smrpdebugtype),
			       NUM_OF_SMRP_DEBUGS,
			       CHUNK_FLAGS_NONE,
			       NULL, 0,
			       "SMRP Debugs");
    if (debug_chunk == NULL)
	return;
    
    /* Allocate space for all the debugging flags and attach descriptions */
    smrp_group_debug = smrp_debug_malloc_flag();
    if (smrp_group_debug)
	smrp_group_debug->text = "SMRP group";

    smrp_mcache_debug = smrp_debug_malloc_flag();
    if (smrp_mcache_debug)
	smrp_mcache_debug->text = "SMRP multicast fast switching";

    smrp_neighbor_debug = smrp_debug_malloc_flag();
    if (smrp_neighbor_debug)
	smrp_neighbor_debug->text = "SMRP neighbor";

    smrp_port_debug = smrp_debug_malloc_flag();
    if (smrp_port_debug)
	smrp_port_debug->text = "SMRP port";

    smrp_route_debug = smrp_debug_malloc_flag();
    if (smrp_route_debug)
	smrp_route_debug->text = "SMRP routing";

    smrp_trans_debug = smrp_debug_malloc_flag();
    if (smrp_trans_debug)
	smrp_trans_debug->text = "SMRP transaction";

    smrptrue_debug = smrp_debug_malloc_flag();
    if (smrptrue_debug)
	smrptrue_debug->text = ""; /* Internal use only */

    smrpfalse_debug = smrp_debug_malloc_flag();
    if (smrpfalse_debug)
	smrpfalse_debug->text = ""; /* Internal use only */

    /* Register for "debug/undebug all" and "show debug" commands */
    reg_add_debug_all(smrp_debug_all, "smrp_debug_all");
    reg_add_debug_show(smrp_debug_show, "smrp_debug_show");

    /* Initialize the flags to reflect the currents setting of "debug all" */
    smrp_debug_all(debug_all_p());
}

/*
 * smrp_debug_setflag
 * Set debugging variable, flagp, according to parsed commands in csb.  
 * If "no" processing was
 * specified (i.e. csb->sense == 0), then the bit vector flagp->flag is
 * cleared.  Else the appropriate bit flag in flagp->flag is set according
 * to GETOBJ(int, 1).  
 */
void
smrp_debug_setflag (parseinfo const *csb, smrpdebugtype *flagp)
{
    if ((csb == NULL) || (flagp == NULL))
	return;
    if (!csb->sense) {
	flagp->flag = 0;
    } else {
	switch (GETOBJ(int, 1)) {
	  case SMRP_DEBUG_INTERFACE:	/* Restrict debugging to interface */
	    flagp->flag = SMRP_INTERFACE_MASK;
	    flagp->list.idb = GETOBJ(idb, 1); /* XXX for now, allow only */
	    flagp->list.next = NULL;          /* one interface */
	    break;
	    
	  default:			/* Default is no restrictions */
	    flagp->flag = -1;
	    flagp->list.idb = NULL;	/* XXX later when allowing multiple  */
	    flagp->list.next = NULL;	/* interfaces, this must be fixed!  */
	    break;
	}
    }
}

/*
 * smrp_debug
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
smrp_debug (smrpdebugtype const *debugsym, int bitvector, ...)
{
    va_list argp;
    idbtype *idbp;
    int flag;
    int type;
    
    if (debugsym == NULL) {
	return (FALSE);
    }
    flag = debugsym->flag;
    if (flag == 0) {			/* debugging flag is off */
	return (FALSE);
    }
    if (flag == -1) {			/* debugging flag is set to */
	return (TRUE);
    }
    
    va_start(argp, bitvector);
    
    while (bitvector) {
	switch (type = bitvector & -bitvector) { /* pull off least-significant */
	    /* non-zero bit */
	  case SMRP_INTERFACE_MASK:
	    idbp = va_arg(argp, idbtype *);
	    if (!idbp || idbp->if_number != debugsym->list.idb->if_number) {
		va_end(argp);
		return (FALSE);
	    }
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
 * smrp_debug_show_flag
 * Outputs to the display the status of debugging flag, flagp (i.e. on or off).
 * If the debugging flag has special restrictions imposed (i.e. interface-
 * specific debugging), then those are indicated.
 */
void
smrp_debug_show_flag (smrpdebugtype const *flagp,
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
	  case SMRP_INTERFACE_MASK:
	    printf(" for interface %s", idb_get_namestring(flagp->list.idb));
	    break;
	    
	  default:
	    buginf("\nUnknown command in smrp_debug_show_flag()");
	}
	flag ^= type;			/* turn off least significant */
					/* non-zero bit */
    }
}





