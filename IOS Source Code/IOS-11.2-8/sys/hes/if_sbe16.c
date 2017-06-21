/* $Id: if_sbe16.c,v 3.10.12.4 1996/05/21 09:48:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_sbe16.c,v $
 *------------------------------------------------------------------
 * if_sbe16.c -- driver for the SBE 16/4 Token Ring Interface
 *
 * August 1990, David R. Hampton
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_sbe16.c,v $
 * Revision 3.10.12.4  1996/05/21  09:48:01  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.12.3  1996/05/17  11:18:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.4  1996/05/05  23:07:22  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.10.2.3  1996/05/02  22:03:47  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 * Revision 3.10.12.2  1996/05/04  00:44:17  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Revision 3.10.12.1  1996/03/18  19:42:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.4  1996/03/13  01:22:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.12.3  1996/03/07  09:40:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.2  1996/02/20  00:52:32  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/13  18:10:15  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.9  1996/02/01  06:02:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8.12.1  1996/01/24  21:47:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/05  10:24:50  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.7  1995/12/19  00:40:17  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.6  1995/12/17  18:27:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/13  04:20:00  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/12/06  18:28:53  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  09:18:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:00:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/07  05:34:01  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.6  1995/07/05  20:48:20  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.5  1995/06/23 18:47:43  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/21  03:27:11  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.3  1995/06/18  23:17:22  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:04:40  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:42:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"

/*
 * Global Storage
 */
int nSBE16;			/* number of SBE 16/4MB Token Rings */
int nSTR;			/* number of SBE Dual Token Ring cards */

/*
 * Hardware regs for the SBE16 board
 * 
 * addr of SBE multibus IO Mailbox
 */
#define SBE16_MULTIMEMBASE (MULTIMEMBASE+0x10000)
#define SBE16_IOB_START MULTIIOBASE+0xb0
#define SBE16_IOB_OFFSET 0x02	/* board are 2 bytes apart from each other */
#define SBE16_MAXBOARDS 7

/* Hardware regs for the STR board addr of STR multibus IO Mailbox */

#define STR_MULTIMEMBASE (MULTIMEMBASE + 0x10000)
#define STR_IOB_START MULTIIOBASE + 0x10
#define STR_IOB_OFFSET 0x02	/* board are 2 bytes apart */
#define STR_MAXBOARDS 7
#define STR_MBX_RDY		0x0080		/* read */


/*
 * SBE16_analyze
 *
 * This routine assumes that it is not the first initialization routine
 * If there are any old style ethernets or serial cards, we simply exit
 * early.  Later configuration checks will complain and reset the system.
 */

static int SBE16_analyze (void)
{
    int i, boards;
    uchar *trbase;

    boards = 0;
    trbase = (uchar *) SBE16_IOB_START;
    for (i = 0; i < SBE16_MAXBOARDS; i++, trbase += SBE16_IOB_OFFSET)
	if (touch_device(trbase))
	    boards++;
    return(boards);
}

/*
 * STR_analyze
 *
 * This routine assumes that it is not the first initialization routine
 * If there are any old style ethernets or serial cards, we simply exit
 * early.  Later configuration checks will complain and reset the system. 
 */
static int STR_analyze (void)
{
    uchar *mailbox = (uchar *)STR_IOB_START;
    int    unit    = 0;
    int    board;
    int    cnt;

    for (board=0; board < STR_MAXBOARDS;board++,mailbox += STR_IOB_OFFSET){

	if (touch_device(mailbox)) {  /* Check for card at this mailbox. */

            /* Wait for the monitor on the board to initialize. */

            for (cnt=60; cnt; --cnt) {
                if (*mailbox & STR_MBX_RDY) {
	            unit ++;
                    break;
                    }

                DELAY(ONESEC/10);
                }
            }
        }

    return(unit);
}

/*
 * OLDTR subsystem initialisation entry point
 */

static void oldtr_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the SBE16 interfaces in system
     */
    nSBE16 = SBE16_analyze();

    /*
     * Discover all the STR Dual Token Ring interfaces in system
     */
    nSTR = STR_analyze();
}

/*
 * SBE16 and STR (aka OLD TR) subsystem header
 */

#define OLDTR_MAJVERSION   1
#define OLDTR_MINVERSION   0
#define OLDTR_EDITVERSION  1

SUBSYS_HEADER(OLDTR,
	      OLDTR_MAJVERSION, OLDTR_MINVERSION, OLDTR_EDITVERSION,
	      oldtr_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
