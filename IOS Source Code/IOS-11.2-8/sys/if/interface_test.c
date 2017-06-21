/* $Id: interface_test.c,v 3.8.12.4 1996/08/12 16:02:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/interface_test.c,v $
 *------------------------------------------------------------------
 * interface_test.c -- Test harness for very high density routers
 *
 * November 1994, Kirk Lougheed
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface_test.c,v $
 * Revision 3.8.12.4  1996/08/12  16:02:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.8.12.3  1996/06/27  16:35:57  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/05/21  09:49:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.12.1  1996/03/18  20:14:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  01:35:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/02/20  00:57:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/12  18:17:41  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.7  1996/01/29  07:28:19  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6.2.1  1996/01/24  22:18:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/18  15:47:23  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.5  1995/12/17  18:28:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/06  18:30:10  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  09:29:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  03:28:06  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  07:16:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:53:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../ip/ip.h"
#include "../os/free.h"
#include "../parser/parser_defs_parser.h"

static void iftest_command (parseinfo *csb);
static void iftest_resetprocess (parseinfo *csb);

/*
 * Local storage
 */

/*
 * Interface test commands
 */
#define ALTERNATE NONE
EOLNS	(test_iftest_eol, iftest_command);
KEYWORD (test_iftest, test_iftest_eol, NONE,
	 "idb", "test long idb chains",
	 PRIV_USER);
#undef	ALTERNATE
#define	ALTERNATE	test_iftest
LINK_POINT      (iftest_command, ALTERNATE);
#undef  ALTERNATE


#define ALTERNATE NONE
EOLNS	(test_iftest_eol2, iftest_resetprocess);
KEYWORD (test_iftest2, test_iftest_eol2, NONE,
	 "reset-process", "reset process timers",
	 PRIV_USER);
#undef	ALTERNATE
#define	ALTERNATE	test_iftest2
LINK_POINT      (iftest_resetprocess, ALTERNATE);
#undef  ALTERNATE



/*
 *  The net is 128.1.0.0.  12 bits of subnet.  Always host 1.
 */
static ulong iftest_address = 0x80010011L;         /* start with subnet 1 */
static ulong iftest_addressmask = 0xFFFFFFF0;
#define IFTEST_ADDRESS_INCR  0x10           /* bump to next subnet */


/*
 * iftest_soutput
 * Start output.  Copies packet to the raw input queue so
 * we can simulate an input load as well.
 */

static void iftest_soutput (hwidbtype *hwidb)
{
    paktype *opak, *ipak;

    while ((opak = holdq_dequeue(hwidb))) {
	hwidb->counters.outputs++;
	hwidb->counters.tx_cumbytes += opak->datagramsize;
	ipak = pak_duplicate(opak);
	if (ipak) {
	    hwidb->counters.inputs++;
	    hwidb->counters.rx_cumbytes += ipak->datagramsize;
	    ipak->if_input = opak->if_output;
	    ipak->if_output = NULL;
	    raw_enqueue(hwidb, ipak);
	}
	datagram_done(opak);
    }
}

/*
 * iftest_create
 * Create an additional "number" serial interfaces
 */

static void iftest_create (int number)
{
    int maxunit, unit;
    hwidbtype *hwidb;
    idbtype *swidb;

    maxunit  = number + nserialnets;
    for (unit = nserialnets; unit < maxunit; unit++) {
	hwidb = idb_create();
	if (!hwidb) {
	    printf(nomemory);
	    return;
	}
	nserialnets++;
	nnets++;
	swidb = hwidb->firstsw;
	hwidb->name = IFNAME_SERIAL;
	hwidb->unit = unit;
	hwidb->slotunit = 0;
	hwidb->vc = NO_VC;

	/*
	 * Pretend we are a DS0 channel
	 */
	serial_idb_init(hwidb, MAXSERIALSIZE, MAXSERIALSIZE, 
			    VISIBLE_MCI_SERIAL_BANDSLOW, MCI_SERIAL_DELAY);

	/*
	 * The following is lifted from the nullidb creation code
	 */
	hwidb->state = IDBS_UP;		/* hardware is always up */
	hwidb->linestate = (linestate_t)return_true; /*force line protocol up*/
	swidb->subif_state = SUBIF_STATE_UP;
	hwidb->soutput =(soutput_t) iftest_soutput;
	hwidb->reset = (reset_t) return_nothing;
	hwidb->oqueue_dequeue = (oqueue_dequeue_t) return_null;
	hwidb->setencap = (setencap_t) return_nothing;

	idb_final_hw_init(hwidb);
	idb_final_sw_init(swidb);
	reg_invoke_sw_idb_init(swidb, FALSE);
	serial_idb_enqueue(hwidb);
	idb_enqueue(hwidb);

	/*
	 * Set up an IP address, one subnet per interface
	 */
        swidb->ip_address = iftest_address;
	swidb->ip_nets_mask = iftest_addressmask;
	swidb->ip_enabled = TRUE;
	iftest_address += IFTEST_ADDRESS_INCR;
	class_resolve(swidb);
    }
}

/*
 * iftest_command
 * Very simple stuff for now
 */

static void iftest_command (parseinfo *csb)
{
    int howmany, status;
    int memory_before, nserial_before;
    char buff[PARSEBUF];
    boolean parsed;

    printf("There are currently %d serial interfaces", nserialnets);
    printf("\nHow many do you wish to add? ");
    status = rdtty(buff, PARSEBUF-2);
    howmany = parse_unsigned(deblank(buff), &parsed);
    if (!status || (!parsed)) {
	printf("%% Need a number");
	return;
    }
    memory_before = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
					   MEMPOOL_ALTERNATE_INCLUDE);
    nserial_before = nserialnets;
    iftest_create(howmany);
    printf("\nAdded %d serial interfaces, used %d bytes of memory",
	   nserialnets - nserial_before,
	   memory_before - mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
						  MEMPOOL_ALTERNATE_INCLUDE));
}

/*
 * iftest_resetprocess
 * Clear some timing counters to get an accurate view
 * of how much CPU we're burning.
 */

static void iftest_resetprocess (parseinfo *csb)
{
    char buff[PARSEBUF];
    pid_t pid;

    printf("\nResetting process timing counters");
    while (TRUE) {
	printf("\nProcess pid? ");
	if (!rdtty(buff, PARSEBUF-2) || null(buff))
	    return;
	pid = atoi(buff);
	process_set_timing(pid, 0, 0);
    }
}

/*
 * iftest_init
 * Initialize this subsystem
 */

static void iftest_init (subsystype *subsys)
{
    parser_add_commands(PARSE_ADD_TEST_CMD, &pname(iftest_command),
			"idb");
    parser_add_commands(PARSE_ADD_TEST_CMD, &pname(iftest_resetprocess),
			"process-timers");
}

/*
 * IFTEST subsystem header
 */

#define IFCTEST_MAJVERSION 1
#define IFCTEST_MINVERSION 0
#define IFCTEST_EDITVERSION  1

SUBSYS_HEADER(iftest,
	      IFCTEST_MAJVERSION, IFCTEST_MINVERSION, IFCTEST_EDITVERSION,
	      iftest_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);
