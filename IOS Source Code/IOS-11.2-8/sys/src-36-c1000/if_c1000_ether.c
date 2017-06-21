/* $Id: if_c1000_ether.c,v 3.5.44.5 1996/08/28 13:10:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/if_c1000_ether.c,v $
 *------------------------------------------------------------------
 * if_c1000_ether.c  C1000 specific ethernet driver
 *
 * November 1994, Ashwin Baindur
 * Based on "if_les_lance.c", December 1989,  Chris Shaker
 * Based on "if_lance.c", June 1988,  Bill Westfield
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c1000_ether.c,v $
 * Revision 3.5.44.5  1996/08/28  13:10:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.44.4  1996/08/13  02:22:42  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.5.44.3  1996/07/18  21:52:57  bcole
 * CSCdi57941:  Windows95 reports duplicate IP address on network
 * Branch: California_branch
 *
 * Place interface in promiscuous mode while using ARP for address
 * discovery.  This way we can completely & transparently spoof the web
 * client.
 *
 * Revision 3.5.44.2  1996/05/21  10:02:59  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.44.1  1996/03/18  21:57:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/13  02:00:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/03/07  10:42:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  17:49:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/12/17  18:37:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/21  07:27:29  thille
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 * fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 * Add file missed in earlier commit, mea culpa...
 *
 * Revision 3.3  1995/11/17  18:40:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/22  04:36:44  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.4  1995/07/03  07:11:40  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Fix another dual-homed file.
 *
 * Revision 2.3  1995/06/23  18:48:32  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  07:16:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:52:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "ieee.h"

#include "media_registry.h"
#include "subsys.h"

#include "config.h"
#include "logger.h"
#include "../src-36-c1000/msg_c1000_ether.c" /* Not a typo, see logger.h */

#include "../os/pool.h"
#include "../les/if_les.h"
#include "../src-36/quicc_driver_lib.h"
#include "../les/if_les_ether.h"
#include "../les/les_driver.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "sys_registry.h"
#include "packet.h"
#include "../if/network.h"
#include "../os/buffers.h"
#include "../ip/remif_proto.h"
#include "../smf/smf_public.h"


void c1000_ethernet_line_down (hwidbtype *idb)
{
    if (idb->ethernet_lineup) {
        idb->ethernet_lineup = FALSE; /* Fatal error; Tx stopped */
        idb->reason = "Fatal Tx error";
        reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
    errmsg(&msgsym(TXERR, ETHERNET), idb->hw_namestring);
}


/*
 * Get the default hardware address of network interface.
 * Note that the cookie only contains the hardware ethernet address
 * for Ethernet unit number 0. Increment the 24 bit host number for
 * each additional unit.
 */
static void 
c1000_ethernet_get_default_hw_address (hwidbtype *idb, uchar addr[])
{
    /*
     * Copy the ethernet 0 hardware address into addr:
     */
    ieee_copy(cookie->ether0_hw_addr, addr);
}

/*
 * Shut down an ethernet interface
 */
static void
c1000_ethernet_shutdown (hwidbtype *idb, int newstate)
{
    leveltype SR;
    les_driver_instance_t *ds = (les_driver_instance_t *)INSTANCE;

#ifdef DEBUG
    ttyprintf(CONTTY, "Ethernet(%d): c1000_ethernet_shutdown: idb = 0x%x\n",
	      idb->unit, idb);
#endif

    net_cstate(idb, newstate);

    SR = raise_interrupt_level(ETHER_DISABLE);
    if (ds && ds->hw_stop)
        (*ds->hw_stop)(idb);
    reset_interrupt_level(SR);
}

/*
 * Enable an ethernet interface whose receiver has been shut down.
 * Please note; this function must be called with NETS_DISABLED, to 
 * work correctly.
 */
static void c1000_ethernet_enable (hwidbtype *idb)
{
    if (idb->type == IDBTYPE_QUICC_ETHER) {
	
	les_driver_instance_t *ds = (les_driver_instance_t *)INSTANCE;

	idb_mark_unthrottled(idb);

        (*ds->hw_enable)(idb);
	ds->enabled++;		/* Count number of times enabled */
    }
}


/*
 * periodically check whether the chip seems to be working.
 */
static void c1000_ethernet_periodic (hwidbtype *idb)
{
    les_ethernet_periodic(idb);

    if (idb->state == IDBS_UP) {
        les_driver_instance_t *ds;

        ds = (les_driver_instance_t *)INSTANCE;
        if (ds->hw_health)
            (*ds->hw_health)(idb); 
    }

    /*
     * Provide bells and whistle vendor support
     */
    reg_invoke_ethernet_update_vendor_states(idb);
}


/*
 * Add another ethernet hardware address to the logical address filter
 */
static void
c1000_ethernet_add_listen_address (idbtype *swidb, uchar *address, uchar fsm)
{
    reg_invoke_ethernet_hw_listen(swidb->hwptr, address);
}

/*
 * Add multicast address range to the logical address filter. 
 *
 * NOTE: Turns out that enabling just 256 IP multicast addresses
 * (0100.5e00.0000 - 0100.5e00.00ff) enables all filter bits. Therefore,
 * just enable all multicasts to support IP multicast routing.
 */
static void 
c1000_ethernet_add_listen_range (idbtype *swidb, uchar *address, uchar *mask)
{
    hwidbtype         *idb;

    idb = swidb->hwptr;
    /*
     * Do in loop ??
     */
    reg_invoke_ethernet_hw_listen_range(idb, address, mask);
}


/*
 * Initialize the ethernet interface:
 */
static void c1000_ethernet_interface_init (hwidbtype *idb,
					   boolean maxdgram_change)
{
    les_driver_instance_t *ds;
    boolean prom_mode = FALSE;
    leveltype SR;
    idbtype *swidb;
    pooltype *old_pool;

    SR = raise_interrupt_level(NETS_DISABLE);
    
    /*
     * stop the controller and count the reset:
     */
    ds = (les_driver_instance_t *)INSTANCE;
    ds->fatal_tx_err = FALSE;

    if (ds->hw_stop)
        (*ds->hw_stop)(idb);

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Show that we are being reset:
	 */
	net_cstate(idb, IDBS_RESET);
	idb->counters.resets++;
    }

    /*
     * Flush the hold queue:
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, don't re-enable it.
     * Note that we do this check after shutting down the controller.
     */
    if (idb->state == IDBS_ADMINDOWN) {
	idb->ethernet_lineup = TRUE;
	reset_interrupt_level(SR);
	return;
    }

    /*
     * initialize the hardware addresses
     * If our station address has not already been set, default it 
     * to the address given in the cookie:
     */
    reg_invoke_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware))
        reg_invoke_get_default_hw_address(idb, idb->hardware);

    /*
     * If a private buffer pool for this interface exists AND there
     * has been an MTU change, discard the pool with the old-sized
     * buffers. 
     */
    
    if (idb->pool && maxdgram_change) {
	old_pool = idb->pool;
	pool_destroy(old_pool);
	idb->pool = NULL;
    }
    
    /* 
     * If the private buffer pool has not been allocated yet, or if
     * it was destroyed due to an MTU change above, allocate it now.
     * Populate the pool, initialize a private buffer cache,
     * and populate the buffer cache with buffers from the newly
     * created private pool.  Also set up the fallback public buffer
     * pool. 
     */
    
    if (!idb->pool) {
	if (idb->pool_group == POOL_GROUP_INVALID)
	    idb->pool_group = pool_create_group();
	
	if (!idb->hw_namestring)
	    idb_init_names(idb->firstsw, FALSE);
	
	idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
				    idb->max_buffer_size,
				    POOL_SANITY | POOL_CACHE,
				    NULL); 
	if (!idb->pool) {
	    crashdump(0);
	}
	
	idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
	pool_adjust(idb->pool, 0, PRIV_NUM_BUF(ds->max_rx_ring_size),
		    PRIV_NUM_BUF(ds->max_rx_ring_size), TRUE);
	
	pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(ds->max_rx_ring_size));
	pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(ds->max_rx_ring_size)); 
    }
    
    if (ds->hw_init)
        (*ds->hw_init)(idb, maxdgram_change);

    /*
     * Enable Reception for appropriate addresses and address groups.
     */

    /*
     * Enable promiscuous listening if bridging is configured
     * on this interface and all those other caveats apply.
     */
    if (idb->tbridge_on_hwidb && bridge_enable &&
	!(system_loading | bootstrap_enable))
	prom_mode = TRUE;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	/*
	 * Refresh any MAC filters.
	 */
	reg_invoke_tbridge_smf_update(idb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
	if (!(system_loading | bootstrap_enable))
	    reg_invoke_media_interesting_addresses(swidb);
    }

    /*
     * NOTE:
     * This checks to see if we're tunneling this interface to a virtual
     * router running on a SGI workstation.  If we are, and we're bridging
     * then go into promiscuous mode.  We really aren't bridging on THIS
     * interface, but on the virtual interface.
     */
    if ((idb->remif != NULL) && (idb->remif->bridging_enabled)) {
        prom_mode = TRUE;
    }

    if (prom_mode || reg_invoke_media_if_should_use_promiscuous(idb)) {
        if (ds->hw_mode)
            (*ds->hw_mode)(idb, MODE_PROM);
    }

    /*
     * Tell the controller to start
     */
    if (ds->hw_start)
        (*ds->hw_start)(idb);

    reset_interrupt_level(SR);		/* restore interrupt level */
    return;
}

/*
 * Wrapper routine for idb->reset vector:
 */
static void c1000_ethernet_reset (hwidbtype *idb)
{
    if (idb->state != IDBS_ADMINDOWN)
        c1000_ethernet_interface_init(idb, FALSE);  /* Reset interface */
}


/*
 * Set up for bridging on this interface:
 */
static void c1000_ethernet_setup_bridging (hwidbtype *idb)
{
    c1000_ethernet_interface_init(idb, FALSE);  /* Reset code will set it up */
    if (idb->counters.resets > 0)
	idb->counters.resets--;
}


/*
 * Ethernet IDB initialization routine:
 */
static void c1000_ethernet_idb_init (hwidbtype *idb)
{
    /*
     * Set up function calls
     */
    idb->reset = c1000_ethernet_reset;
    idb->periodic = c1000_ethernet_periodic;
    idb->listen = c1000_ethernet_add_listen_address;
    idb->listen_range = c1000_ethernet_add_listen_range;
    idb->setup_bridging = c1000_ethernet_setup_bridging;
    idb->shutdown = c1000_ethernet_shutdown;
    idb->enable = c1000_ethernet_enable;
    idb->txqlength = les_driver_txqlength;

    les_ethernet_idb_init(idb);
}


/*
 * Set up an idb for an ethernet interface
 * and initialize that interface.
 */
static void 
c1000_ethernet_init (int driver, int which, long membase, long port)
{
    hwidbtype *idb;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    /*
     * Save our unit number
     */
    idb->unit = which;    /* unit number */


    /*
     * Initialize the ethernet idb.
     */
    c1000_ethernet_idb_init(idb);

    /*
     * Initialize any platform dependent stuff in the idb.
     */

    /*
     * Set up the hardware.
     * Allocate the driver instance data structure.
     * Initialize controller specific stuff in the idb.
     * The registry call will CASE on the driver type.
     */
    reg_invoke_ethernet_driver_setup(driver, idb, membase, port);

    /*
     * Reset (completely initialize) the hardware:
     */
    if (idb->reset)
        (*idb->reset)(idb);
    idb->counters.resets = 0;
}

/*
 * Ethernet show controllers commands
 */
#define ALTERNATE       NONE
#include "../les/exec_show_controllers_ether.h"
LINK_POINT(ether_showcontroller_commands, ALTERNATE);
#undef  ALTERNATE


/*
 * Parser init for ethernet show controller command
 */
static void
c1000_ethernet_parser_init (void)
{
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
                        &pname(ether_showcontroller_commands), "quicc");
}

/*
 * c1000 ethernet subsystem initialization entry point
 */
static void c1000_ethernet_subsys_init (subsystype *subsys)
{
    int driver_type = UNKNOWN_DRIVER;  /* default; keep compiler happy */
    long dpram_base = -1;  /* default; keep compiler real happy */
    long serial_port = 0;  /* default; keep compiler extremely happy */

    /*
     * Discover all the Ethernet interfaces in the system.
     * If any found, initialize them.
     */
    switch (cookie->interfaces) {
      case INTERFACE_1E1T:
      case INTERFACE_1E1B:
      case INTERFACE_1E1N:
         nethernets = 1;
         driver_type = QUICC_DRIVER;
         dpram_base = (long)getmbar();
         serial_port = QUICC_PORT_NO(1);
         break;
      default:
         nethernets = 0;
         break;
    }
    /*
     * If any ethernets found, set up registry calls,
     * initialize the interface(s), and set up the parser stuff.
     */
    if (nethernets) {
        /*
         * Initialize registry stuff
         */

        reg_add_get_default_hw_address(
            c1000_ethernet_get_default_hw_address,
            "get_default_hw_address");

        switch(nethernets) {
        default:
        case 1:
            c1000_ethernet_init(driver_type, 0, dpram_base, serial_port);
            break;
        }
        c1000_ethernet_parser_init();
    }
}

#define C1000_ETHERNET_MAJVERSION   1
#define C1000_ETHERNET_MINVERSION   0
#define C1000_ETHERNET_EDITVERSION  1

SUBSYS_HEADER(c1000_ethernet, C1000_ETHERNET_MAJVERSION, 
              C1000_ETHERNET_MINVERSION,
              C1000_ETHERNET_EDITVERSION,
              c1000_ethernet_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: quicc_ethernet",
              NULL);


      
