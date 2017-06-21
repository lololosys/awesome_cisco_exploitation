/* $Id: lex.c,v 3.6.20.8 1996/08/28 13:00:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex.c,v $
 *------------------------------------------------------------------
 * lex.c - Core lex routines.
 *
 * Mar. 1994, Andy Harvey
 *
 * Copyright (c) 1994-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex.c,v $
 * Revision 3.6.20.8  1996/08/28  13:00:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.7  1996/08/07  09:01:29  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.20.6  1996/06/27  09:38:10  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.6.20.5  1996/06/01  01:32:19  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.6.20.4  1996/05/21  09:56:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.3  1996/05/21  06:35:46  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.20.2  1996/04/22  20:49:28  sgannu
 * CSCdi52515:  Lex interface ignores no shutdown command
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/03/18  20:43:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/13  01:50:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.2  1996/03/07  10:06:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:09:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:07:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/16  05:07:55  sgannu
 * CSCdi42843:  SegV exception crash in nov_fastswitch.
 * Copy some CT1 specific variables from serial idb into lex idb.
 *
 * Revision 3.4  1995/12/05  23:09:16  sgannu
 * CSCdi44441:  Lex dosnt respond to ARP when bridge-group is enabled in
 * lex int
 *
 * Revision 3.3  1995/11/17  17:42:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:34:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:15:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/18  17:22:05  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Revision 2.6  1995/09/20  23:08:51  dcoli
 * CSCdi39882:  7000 ppp to lex looped causes trans bridge problems
 * The lex linestate routine should invoke the linestate routine of the
 * serial interface to which it is bound.
 *
 * Revision 2.5  1995/08/25  11:44:35  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.4  1995/08/02  21:39:27  sgannu
 * CSCdi37788:  bridging does not work for flex box with ppp compression
 *
 * Revision 2.3  1995/07/20  17:55:05  wmay
 * CSCdi37067:  router crashes while fast switching ip over smds fsip to
 * lex.  add mci_testptr copy to lex idb, also default the registry that
 * was crashing.
 *
 * Revision 2.2  1995/06/21  03:01:48  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  21:40:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../if/network.h"
#include "access.h"
#include "ieee.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_lex.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "snmp_interface.h"
#include "../if/if_vidb.h"
#include "../wan/ppp.h"
#include "../wan/lex_ncp.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "../lex/lexfast.h"
#include "../smf/smf_public.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../snmp/ifmib_registry.h"

/*
 * Forward declarations.
 */
static void lex_soutput(hwidbtype *);

/*
 * Global definitions.
 */
static int lex_pid = 0;
queuetype lex_freeQ;
queuetype lex_activeQ;
const char lex_no_inventory[] = "\nNo inventory message received from LAN Extender";
ushort lex_fast_bridge_header[PPP_LEX_ENCAPBYTES/2 + 1];

/*
 * lex_on_idb()
 * This function is registered with the "proto_on_swidb" registry.
 * This registry service is passed a linktype and a software IDB pointer,
 * and returns TRUE or FALSE depending on whether the protocol corresponding
 * to the linktype is configured on the specified interface.  This registry
 * service is invoked from ncp_input(), check_ncp_status() and ncp_lowerup()
 * in the PPP subsystem to determine whether to pass packets to the
 * ncp FSM, close the ncp (since corresponding protocol is not running),
 * or to bring up the ncp.  The host must always be prepared to accept a 
 * connection from a FLEX device.  And since there is no specific protocol 
 * to configure on the serial interface for Lex (other than PPP 
 * encapsulation), we must have this lex_on_idb function which returns TRUE 
 * if one or more Lex interfaces have been created/configured.
 */
static boolean lex_on_idb (idbtype *idb)
{
    hwidbtype *hwidb = idb->hwptr;
    /* 
     * Lex not supported on async interfaces, so don't negotiate if
     * an async interface is configured with PPP encaps.
     */
    if (hwidb->status & IDB_ASYNC)
	return(FALSE);
    return(is_lex_configured());
}

/*
 * is_lex_configured.
 * Returns TRUE is a lex is configured else FALSE.
 */
boolean is_lex_configured (void)
{
    
    lex_info_t *info;
    hwidbtype *hwidb;

    if (!QUEUEEMPTY(&lex_activeQ))
	return (TRUE);
    
    FOR_ALL_LEX(&lex_freeQ, info) {
	hwidb = info->swidb->hwptr;
	if (!(hwidb->status & IDB_DELETED))
	    return(TRUE);
    }
    
    return(FALSE);
}
/* 
 * lex_cstate
 * If a Lex interface is in the IDBS_ADMINDOWN state, and the user
 * enters a "no shutdown" command, a "no shutdown" command is sent to the
 * FLEX device, but we do not change the state to IDBS_UP until we
 * receive an acknowledgement from the FLEX device.
 */

static boolean lex_cstate (hwidbtype *hwidb, int newstate)
{
    if (hwidb->firstsw->lex_swidb == NULL) { /* Unbound Lex interface */
	return (FALSE);
    }
    if (hwidb->state == IDBS_ADMINDOWN) {
	/* 
	 * If in state IDBS_ADMINDOWN and this is a "no shutdown" command, do
	 * NOT change state here.  Change states when we receive an
	 * acknowledgement.
	 */
        if (newstate == IDBS_UP) {
            lex_noshutdown(hwidb);
	    hwidb->state = IDBS_INIT; 
	}
        return (TRUE);
    }

    return(FALSE);
}


/*
 * Returns current line protocol status of the serial interface that the lex
 * interface is bound to.  If lex not bound, then return FALSE, line protocol
 * is down.
 */
static boolean lex_linestate (hwidbtype *hwidb)
{
    idbtype *serial_swidb;
    hwidbtype *serial_hwidb;

    if (!(hwidb->status & IDB_LEX))
        return(FALSE);

    serial_swidb =  hwidb->firstsw->lex_swidb;
    if (serial_swidb == NULL)
        return(FALSE);

    /*
     * Check the linestate of the serial interface that this
     * lex is bound to.
     */
    serial_hwidb = serial_swidb->hwptr;
    if (serial_hwidb->linestate)
        return((*(serial_hwidb->linestate))(serial_hwidb) ? TRUE : FALSE);
    else {
        errmsg(&msgsym(LINEST, LINK), serial_hwidb->hw_namestring);
        return(FALSE);
    }
}

static void lex_init (subsystype *subsys)
{
    /*
     * Initalize the free and active queues.
     */
    queue_init(&lex_freeQ, 0);
    queue_init(&lex_activeQ, 0);

    lex_add_parser_callbacks();
    lex_parser_init();
    lex_rcmd_init();
    lex_debug_init();
    
    reg_add_lex_bind_bymac(lex_bind_bymac, "lex_bind_bymac");
    reg_add_lex_unbind_byidb(lex_unbind_byidb, "lex_unbind_byidb");
    reg_add_lex_find_free_bymac(lex_find_free_bymac, "lex_find_free_bymac");
    reg_add_proto_on_swidb(LINK_LEX, lex_on_idb, "lex_on_idb");
    reg_add_pre_cstate(lex_cstate, "lex_cstate");

}

/*
 * Routines that access the free and active queues must be in this file.
 */
static void lex_moveQ (queuetype *from, queuetype *to, lex_info_t *info)
{
    leveltype level = raise_interrupt_level(NETS_DISABLE);

    unqueue(from, info);
    enqueue(to, info);

    reset_interrupt_level(level);
}


/*
 * lex_oqueue
 * This routine used to be the hwidb->oqueue vector for lex interfaces. 
 * It just calls the oqueue vector of the serial interface to 
 * which we are bound.  This prevented us from doing priority output 
 * queueing on lex interfaces.  So now we use the default oqueue vector
 * (holdq_enqueue) at the expense of being a little slower.
 *
 * If lex interface is unbound, this function returns FALSE and calls 
 * datagram_done() to drop the packet.
 */

static boolean lex_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL which)
{
    idbtype *swidb;
    hwidbtype *hwidb;

    /*
     * So far this function is only called by lex_soutput().
     * Under normal condition idb->soutput() (i.e. lex_soutput) is called by 
     * datagram_out(). Packets are encaped with lex headers in hwidb->vencap 
     * (i.e. lex_vencap) before datagram_out() get called. lex_vencap() has the
     * check to ensure no unbound lex interface. It returns FALSE and the caller
     * function calls datagram_done() to drop the packet.
     *
     * However, if lex_soutput is directly invoked via idb->soutput OR lex_oqueue()
     * is directly called, chances are that lex_vencap() is not called. So a check 
     * is necessary to ensure no packet is queued to unbound lex interface. 
     */
    swidb = pak->if_output ? pak->if_output->lex_swidb : NULL;
    hwidb = swidb ? swidb->hwptr : NULL;

    if (hwidb == NULL) {
       datagram_done(pak);
       return(FALSE);
    }

    GET_TIMESTAMP(idb->lastoutput);

    /*
     * Change the output interface to be that of the serial driver
     * we are bound to.
     */
    pak->if_output = swidb;
    pak->linktype = LINK_LEX;
 
    /*
     * Call the bound serial interfaces's oqueue and soutput routines.
     */
    (*hwidb->oqueue)(hwidb, pak, TAIL);
    (*hwidb->soutput)(hwidb);
    
    return(TRUE);
}

static void lex_show_interface_support (hwidbtype *idb)
{
    if (idb->firstsw->lex_swidb == NULL)
        printf("\n  Unbound");
    else
        printf("\n  Bound to %s", idb->firstsw->lex_swidb->namestring);
}

/*
 * lex_vencap
 */
static boolean lex_vencap (paktype *pak, long address)
{
    idbtype *if_input;
    idbtype *swidb;
    idbtype *serial_swidb;
    hwidbtype *serial_hwidb;
    ushort payload_linktype;
    ushort payload_enctype;
    
    swidb = pak->if_output;
    serial_swidb = swidb->lex_swidb;

    /*
     * Is a serial interface bound to this lex
     * interface.
     */
    if (serial_swidb == NULL)
	return(FALSE);
    serial_hwidb = serial_swidb->hwptr;

    /*
     * Save real if_input.  This must be set to NULL so that
     * the cache line is not added.
     */
    if_input = pak->if_input;
    pak->if_input = NULL;

    /*
     * Call the Media encap routine.  At the moment
     * the only encapsulation supported is ethernet.
     */
    if (((ether_vencap)(pak, address)) == FALSE) {
	pak->if_input = if_input;
	return(FALSE);
    }

    /*
     * Keep the "real" link type because the cache_update routine
     * will be called after the lower driver has added its encap.
     */
    payload_linktype = pak->linktype;
    pak->linktype = LINK_LEX;
    payload_enctype = pak->enctype;    

    /*
     * Call the serial vencap routine.
     */
    pak->if_output = serial_swidb;
    if (((*serial_hwidb->vencap)(pak, 0L)) == FALSE) {
	pak->if_input = if_input;
	pak->if_output = serial_swidb->lex_swidb;
	pak->linktype = payload_linktype;
	return(FALSE);
    }

    /*
     * Change things back again.
     */
    pak->if_output = serial_swidb->lex_swidb;
    pak->if_input = if_input;
    pak->linktype = payload_linktype;
    pak->enctype = payload_enctype;

    /*
     * invoke the cache update routine using the real
     * linktype.
     */
    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    return(TRUE);
}

static void lex_init_idbs (hwidbtype *hwidb, idbtype *swidb, uint unit)
{
    /*
     * The following two lines will change for the the token
     * ring version of the sharps product.
     */
    ether_idb_init(hwidb);

    hwidb->status |= (IDB_LEX | IDB_BRIDGE);
    /*
     * This is set in the vidb_malloc, we support fast bridging on lex
     * so we reset this variable.
     */
    hwidb->span_bridge_process_force = 0;

    hwidb->name = IFNAME_LEX;
    hwidb->type = IDBTYPE_LEX;
    hwidb->typestring = "Lan Extender";
    hwidb->fast_switch_type = FS_LEX;
    hwidb->state = IDBS_UP;
    hwidb->unit = unit;
    idb_init_names(swidb, TRUE);
    hwidb->hw_if_index = idb_get_next_hw_index();
    hwidb->firstsw->tbridge_media = TBR_LEX_TRANSIT;
    hwidb->snmp_if_index = reg_invoke_ifmib_register_hwidb(hwidb);

    hwidb->vencap = lex_vencap;
    hwidb->shutdown = lex_shutdown;
    hwidb->linestate = lex_linestate;
    hwidb->show_controller = lex_show_controller;
    hwidb->show_support = lex_show_interface_support;
    hwidb->reset = lex_reset;
    hwidb->soutput = lex_soutput;

    /*
     * These routines do nothing at the moment.
     */

    hwidb->listen = (listen_t)return_nothing;
    hwidb->periodic = (periodic_t) return_nothing;
    hwidb->keep_period = KEEPALIVE_DEFAULT;

    /*
     * Since this interface is created after hardware sizing and the
     * initial startup code, we call the protocol initialization registry
     * specifying that hardware fix up is required (TRUE).
     */
    reg_invoke_sw_idb_init(swidb, TRUE);
}




static idbtype *lex_createidb (uint unit)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    lex_info_t *info;
    lex_idb_info_t *lex_info;
    lex_stats_area_t *lex_stats;
    lex_inventory_msg *lex_inventory;

    info = malloc(LEX_INFO_SIZE);
    if (info == NULL)
	return(NULL);

    lex_info = malloc(sizeof(lex_idb_info_t));
    if (lex_info == NULL) {
        free(info);
        return(NULL);
    }

    lex_stats = malloc(sizeof(lex_stats_area_t));
    if (lex_stats == NULL) {
        free(info);
	free(lex_info);
        return(NULL);
    }
    
    lex_inventory = malloc(LEX_INVENTORY_LEN);
    if (lex_inventory == NULL) {
        free(info);
	free(lex_info);
	free(lex_stats);
        return(NULL);
    }

    /*
     * Grab this last because we don't want to use another
     * swidb index.
     */
    swidb = vidb_malloc();
    if (swidb == NULL) {
	free(info);
	free(lex_info);
	free(lex_stats);
	free(lex_inventory);
	return(NULL);
    }

    /*
     * Now we have all we need.
     */
    info->swidb = swidb;

    lex_info->retry_count = LEX_DEFAULT_CMD_RETRY;
    lex_info->rcmd_timeout = LEX_DEFAULT_TIMEOUT;
    lex_info->lex_stats = lex_stats;
    lex_info->inventory = lex_inventory;

    lex_info->last_remote_ether_lineproto = LEXETHER_LINEPROTOCOL_UNSURE;
    swidb->lex_info = lex_info;

    hwidb = swidb->hwptr;
    lex_init_idbs(hwidb, swidb, unit);

    if (lex_pid == 0)
        lex_pid = cfork(lex_rcmd_input, 0L, 1000, "LEX Input",
			CONSOLE_LINENUM);

    p_enqueue(&lex_freeQ, info);
    idb_enqueue(hwidb);

    /*
     * Increment nnets variable when creating virtual interfaces that go on
     * the idbQ so SNMP queries will show the proper number of interfaces on
     * the system.
     */
    nnets++;

    return(swidb);
}

/*
 * lex_reset.
 * At the moment sets things up for bridging only.
 */
void lex_reset (hwidbtype *hwidb)
{
    /*
     * Cosmetic Reasons to show the number of time the lex interface
     * has been reset. It is called when the lex is up.
     */
    hwidb->counters.resets++;

    /*
     * Only if on active queue.
     */
    if (lex_find_active_byidb(hwidb->firstsw, FALSE) != NULL) {
	if (hwidb->tbridge_on_hwidb) {

	    lex_set_span_mac_offset(hwidb);
	}

	/*
	 * Register lex interface MAC address in the bridge table.
	 */
	if (!is_ieee_zero(hwidb->hardware))
	    reg_invoke_tbridge_smf_update(hwidb->hardware,
					  hwidb->firstsw,
					  SMF_MACTYP_OWN_UNICAST |
					  SMF_ACTION_RECEIVE);
    }
}


/* 
 * lex_show_controller
 * Show FLEX controller information.  This information is sent to us 
 * by the cisco-1000 in the LEX_INVENTORY message. 
 * Returning FALSE tells the caller that no errors occurred.
 */
 
boolean lex_show_controller (hwidbtype *hwidb, parseinfo *csb)
{
    lex_inventory_msg *lex_inventory;

    if (hwidb->firstsw->lex_swidb == NULL) {
        printf("\n%s %s", hwidb->firstsw->namestring, lex_not_bound);
        return(FALSE);
    }

    lex_inventory = hwidb->firstsw->lex_info->inventory;

    if (lex_inventory->prom_sw_ver_maj == 0) {
        printf(lex_no_inventory);
        return(FALSE);
    }

    printf("\n%s:", hwidb->firstsw->namestring);
    printf("\nFLEX Hardware revision %u", lex_inventory->hw_rev);
    printf("\nFLEX PROM software version %u.%u", lex_inventory->prom_sw_ver_maj,
           lex_inventory->prom_sw_ver_min);
    if (!(lex_inventory->flags & PROM_IMAGE_CKSUM_OK))
        printf(", FAILED CHECKSUM");

    /*
     * Currently, the FLEX software cannot tell the difference between
     * no image in flash, and an image whose checksum is incorrect.
     * When the FLEX code can differentiate these two scenarios, it will
     * set the flash software major version number to 0 if no image exists
     * in flash.  Until then, the best we can do is report the bogus
     * software version number, and the fact that it failed the checksum.
     */
    if (lex_inventory->flash_sw_ver_maj != 0) {
	printf("\nFLEX Flash software version %u.%u",
	       lex_inventory->flash_sw_ver_maj, lex_inventory->flash_sw_ver_min);
	if (!(lex_inventory->flags & FLASH_IMAGE_CKSUM_OK))
	    printf(", FAILED CHECKSUM");
    }
    else
	printf("\nNo FLEX image present in flash");

    printf("\nCurrently running image software version %u.%u",
          lex_inventory->running_sw_ver_maj, lex_inventory->running_sw_ver_min);
    if (lex_inventory->flags & LEX_RUNNING_PROM)
        printf(", booted from PROM");
    else if (lex_inventory->flags & LEX_RUNNING_FLASH)
        printf(", booted from flash");


    printf("\n%uK bytes of flash memory", lex_inventory->flash_size/1024);
    printf("\nSerial number is %u", lex_inventory->serial_number);
    printf("\nStation address is %e", lex_inventory->mac_addr);

    return(FALSE);
}

static lex_info_t *lex_find_bymacaddr(queuetype *q, uchar const *src)
{
    lex_info_t *info;
    hwidbtype *hwidb;

    FOR_ALL_LEX(q,info) {
	hwidb = info->swidb->hwptr;
	if (hwidb->status & IDB_DELETED)
	    continue;
	if (ieee_equal(src, hwidb->bia) == TRUE)
	    return(info);
    }
    return(NULL);
}

lex_info_t *lex_find_free_bymacaddr (uchar const *src)
{
    return(lex_find_bymacaddr(&lex_freeQ, src));
}

/* 
 * lex_find_free_bymac
 * Called during the lex-ncp negotiation process and is necessary because 
 * we don't want to get all the way to the OPEN state and find that there
 * is no Lex interface with a matching MAC address.  This would be
 * considered anti-social behavior.
 */
boolean lex_find_free_bymac (uchar *macaddr)
{
    if (lex_find_free_bymacaddr(macaddr) == NULL)
        return(FALSE);
    else
        return(TRUE);
}

static lex_info_t *lex_find_byidb (queuetype *q, idbtype *swidb,
				  boolean check_idb_deleted)
{
    lex_info_t *info;
    hwidbtype *hwidb;

    FOR_ALL_LEX(q,info) {
	hwidb = info->swidb->hwptr;
	/*
	 * if check_idb_deleted is TRUE check if hwidb is marked
	 * deleted else ignore.
	 */
	if (check_idb_deleted && (hwidb->status & IDB_DELETED))
	    continue;
	if (info->swidb == swidb)
	    return(info);
    }
    return(NULL);
}

lex_info_t *lex_find_active_byidb (idbtype *swidb, boolean idb_deleted)
{
    return(lex_find_byidb(&lex_activeQ, swidb, idb_deleted));
}

lex_info_t *lex_find_free_byidb (idbtype *swidb, boolean idb_deleted)
{
    return(lex_find_byidb(&lex_freeQ, swidb, idb_deleted));
}

/*
 * lex_bind
 * Bind a lex interface to the serial interface over which the 
 * "connection request" came.  Called when the Lex-ncp reaches the OPEN 
 * state.
 */
void lex_bind (idbtype *serial, lex_info_t *info)
{
    hwidbtype *serial_hwidb;
    hwidbtype *lex_hwidb;
    idbtype *lex_swidb;
    lex_idb_info_t *lex_info;
    uchar encapsize;

    /*
     * Are we really already bound.
     */
    if (serial->lex_swidb != NULL)
	return;
    
    serial->lex_swidb = info->swidb;
    info->swidb->lex_swidb = serial;

    /*
     * Set up ready for fast switching.
     */
    serial_hwidb = serial->hwptr;
    lex_swidb = info->swidb;
    lex_hwidb = lex_swidb->hwptr;

    /*
     * Borrow some values from the host serial.
     */
    lex_hwidb->mci_regptr = serial_hwidb->mci_regptr;
    lex_hwidb->mci_testptr = serial_hwidb->mci_testptr;
    lex_hwidb->mci_index = serial_hwidb->mci_index;
    lex_hwidb->buffer_pool = serial_hwidb->buffer_pool;
    lex_hwidb->devctl = serial_hwidb->devctl;
    lex_hwidb->rxoffset = serial_hwidb->rxoffset;
    lex_hwidb->tx_acc_ptr = serial_hwidb->tx_acc_ptr;
    lex_hwidb->tx_q_ptr = serial_hwidb->tx_q_ptr;
    lex_hwidb->slot = serial_hwidb->slot;
    lex_hwidb->m32_channel = serial_hwidb->m32_channel;

    /*
     * Calculate the encsize now so it can be used in the
     * fastswitching code.
     */
    lex_hwidb->encsize = serial_hwidb->encsize + LEX_HEADER_LEN;

    /*
     * Used in fastswitching code for bridging.
     * Currently this is an even number. If this is calculated to an
     * odd number then the way lex header is written in the fast
     * bridging has to change (tbridge_mci_lex_transit_hdr_wr_inline()).
     */
    lex_hwidb->span_encapsize = serial_hwidb->encsize + LEX_HEADER_LEN;
    
    /*
     * Set up the lex header for fast bridging.
     */
    encapsize = 0;
    if (serial_hwidb->cache_board_encap) {
	(*serial_hwidb->cache_board_encap)(serial_hwidb,
					   lex_fast_bridge_header,
					   sizeof(ushort));
	encapsize++;
    }
    lex_fast_bridge_header[encapsize++] = PPP_STATION;
    lex_fast_bridge_header[encapsize++] = TYPE_PPP_LEX;
    /* 
     * Currently Only ethernet.
     */
    lex_fast_bridge_header[encapsize] = LEX_MEDIA_ETHER;
    lex_hwidb->lex_bridge_header_cache = &lex_fast_bridge_header;

    /*
     * Low end & RSP fastswitching.
     */
    lex_hwidb->fastsend = serial_hwidb->fastsend;

    /*
     * Match the bandwidth of the Lex interface with that of the
     * serial interface that is currently hosting it.
     */
    lex_swidb->delay = serial->delay;
    lex_swidb->delay_def = serial->delay_def;
    lex_swidb->visible_bandwidth = serial->visible_bandwidth;
    lex_swidb->visible_bandwidth_def = serial->visible_bandwidth_def;

    /*
     * Having these bits in the status field allows better
     * check to see if the "lex" is capable of supporting
     * fastswitching.
     */
    if (serial_hwidb->status & IDB_MCI)
	lex_hwidb->status |= IDB_MCI;
    else if (serial_hwidb->status & IDB_CBUS)
	lex_hwidb->status |= IDB_CBUS;

    if (!(info->swidb->hwptr->state == IDBS_ADMINDOWN))
        net_cstate(info->swidb->hwptr, IDBS_UP);
    else {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: bind occurred on shutdown interface %s:",
		   info->swidb->namestring);
    }

    /*
     * Move to the active queue.
     */
    lex_moveQ(&lex_freeQ, &lex_activeQ, info);

    /*
     * Turn on fastswitching if required.
     */
    reg_invoke_fast_setup(lex_hwidb);

    lex_rcmd_stub_request_send(LEX_INVENTORY, info->swidb);

    lex_info = info->swidb->lex_info;

    if (lex_hwidb->state == IDBS_ADMINDOWN)
        lex_rcmd_stub_request_send(LEX_SHUTDOWN, info->swidb);

    if (lex_info->lex_priority_list)
        lex_rcmd_stub_request_send(LEX_SET_PRIORITY, info->swidb);

    if (lex_info->lex_type_in)
        lex_rcmd_stub_request_send(LEX_PROTO_FILTER, info->swidb);

    if (lex_info->lex_address_in)
        lex_rcmd_stub_request_send(LEX_ADDR_FILTER, info->swidb);

    setup_lex_iabs(lex_swidb,info);

    lex_reset(lex_hwidb);
}

/*
 * lex_bind_bymac is called by lex_up (in the PPP subsystem) when the 
 * lex-ncp finite state machine reaches the OPEN state.
 *
 * serial - the serial interface over which the FLEX connection arrived.
 * macaddr - the MAC address negotiated by the FLEX device.
 */
void lex_bind_bymac (idbtype *serial, uchar *macaddr)
{
    lex_info_t *lex_info;

    lex_info = lex_find_free_bymacaddr(macaddr);
    if (lex_info == NULL) {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: %s- No free Lex interface found with negotiated MAC address %s",
                   serial->namestring, macaddr);
    }
    else {
        lex_bind(serial, lex_info);
	ieee_copy(macaddr,lex_info->last_bia);
	ieee_copy(macaddr,lex_info->ether_iab->hardware);
    reg_invoke_ifmib_create_rcvaddr(lex_info->ether_iab->snmp_if_index, 
			lex_info->ether_iab->hardware,IEEEBYTES);
    }
}

/*
 * lex_unbind
 * The association between a Lex interface and a particular serial
 * interface that was established in lex_bind is removed.
 * The most important thing is that the lex_swidb field of both
 * the serial and lex interfaces is set to NULL (where they used to
 * point to each other).
 */
void lex_unbind (lex_info_t *info)
{
    idbtype *serial;
    hwidbtype *hwidb;
    lex_rcmdst_type *rcmd = NULL;
    
    if (info->swidb == NULL)
	return;

    hwidb = info->swidb->hwptr;

    net_cstate(hwidb, IDBS_DOWN);

    /*
     * Reset to the original values (where important).
     */
    if (hwidb->status & IDB_MCI)
	hwidb->status &= ~IDB_MCI;
    else if (hwidb->status & IDB_CBUS)
	hwidb->status &= ~IDB_CBUS;
    
    for (rcmd = (lex_rcmdst_type *)LEXrcmdQ.qhead; rcmd; rcmd = rcmd->next) {
        if (info->swidb == rcmd->swidb) {

            if (rcmd->process_waiting) {
                rcmd->status = LEX_LINK_DOWN;
                rcmd->status_posted = TRUE;
            }
            else
                rcmd->delete_rcmd = TRUE;
        }
    }

    memset(info->swidb->lex_info->inventory, 0, LEX_INVENTORY_LEN);

    /*
     * Clean up all the dangling pointers.  Then place
     * the interface back on the free queue.
     */
    serial = info->swidb->lex_swidb;
    serial->lex_swidb = NULL;
    info->swidb->lex_swidb = NULL;    

    lex_moveQ(&lex_activeQ, &lex_freeQ, info);

    /*
     * note that the lex iabs do not get deregistered unless
     * a different lex comes along and binds to the serial
     * interface.  that is why the lex iabs are not freed
     * and deregistered here.
     */

}

void lex_unbind_byidb (idbtype *lex_swidb)
{
    lex_info_t *lex_info;

    /*
     * The FALSE indicates, look in all the lex hardware idbs.
     */
    lex_info = lex_find_active_byidb(lex_swidb, FALSE);

    if (lex_info == NULL) {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: No active Lex interface found for unbind");
    }
    else
        lex_unbind(lex_info);
}

static boolean lex_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_LEX) {
	csb->createidb = lex_createidb;
	return(TRUE);
    }
    return(FALSE);
}

static void lex_enqueue (paktype *pak)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    ether_hdr *ether;

    /*
     * If we get a LINK_LEX data packet, and the serial interface is not 
     * bound to a lex interface, ditch the packet.
     */
    swidb = pak->if_input->lex_swidb;
    if (swidb == NULL) {
	datagram_done(pak);
	return;
    }

    /*
     * Strip off lex headers.
     */
    REMOVE_PPP_LEX_HEADER(pak);

    ether = (ether_hdr *)pak->datagramstart;
    pak->rxtype = ether_decode_encapsulation_inline(pak, ether);
    
    hwidb = swidb->hwptr;
    hwidb->counters.inputs++;
    hwidb->counters.rx_cumbytes += pak->datagramsize;
    
    /*
     * Switch to lex interface.
     */
    change_if_input(pak, swidb);
    
    GET_TIMESTAMP(hwidb->lastinput);    /* remember time of last input */

    pak->linktype = (*hwidb->getlink)(pak);
    /*
     * Snap the frame to alignment if required
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));
    
    if (RUNNING_IF(LINK_BRIDGE, swidb) &&
	(BRIDGING_RXTYPE(hwidb->firstsw, pak->rxtype))){
	pak->flags |= PAK_TBRIDGE_IN;
	/*
	 * We would reach here if ppp compression is turned on the
	 * serial interface which has a lex bound to it. Normally
	 * bridging on lex is fast bridged. Let the broadcast and
	 * multicast packets take there own way, but if it is a unicast
	 * bridge packet we should do process bridging.
	 */
	if (pak->linktype != LINK_ARP) 
	    if (!((pak->linktype == LINK_IP) && reg_invoke_ip_forus(pak))){
		net_bridge_enqueue(pak);
		return;
	    }
    }

    reg_invoke_raw_enqueue(pak->linktype, pak);
}

static void lex_deleteidb(idbtype *swidb)
{
    lex_idb_info_t *lex_info;
    hwidbtype *hwidb = hwidb_or_null(swidb);

    lex_info = swidb->lex_info;
    if (!hwidb || !(hwidb->status & IDB_LEX) || !lex_info)
	return;
    /*
     * Check to see if lex is bound to a serial
     */

    if (swidb->lex_swidb != NULL)
	close_a_lex_ncp(swidb->lex_swidb->hwptr);


    /*
     * Unfortunatly, we cannot free the memory for the
     * idb pair because of other system limitations.
     * The idb has been marked invisable.
     */

    /*
     * Clean up any private structures.
     */
    memset(lex_info->lex_stats, 0, sizeof(lex_stats_area_t));
    memset(lex_info->inventory, 0, LEX_INVENTORY_LEN);
}

void lex_add_parser_callbacks (void)
{
    reg_add_create_idb(lex_parser_createidb, "lex_createidb");
    
    reg_add_swif_erase(lex_deleteidb, "lex_deleteidb");
    
    reg_add_raw_enqueue (LINK_LEX, lex_enqueue, "lex_enqueue");
}

/*
 * this routine prints the top part of the "sh int lex eth" display.
 */
static void lex_show_ether_interface (idbtype *idb)
{
    lex_stats_area_t *lex_stats;
    lex_inventory_msg *lex_inventory;

    if (lex_show_interface_common(idb, LEX_ETHER_INTF) == FALSE)
        return;

    lex_inventory = idb->lex_info->inventory;
    lex_stats = idb->lex_info->lex_stats;
    /* 
     * FLEX Ethernet interface state should be the same as the Lex
     * interface.
     */
    printf("%s%s is %s, ", idb->namestring, "-Ethernet0", print_idbstate(idb));
    printf("line protocol is ");
    if ( idb->lex_info->last_remote_ether_lineproto == LEXETHER_LINEPROTOCOL_DOWN )
      printf("down");
    else
      printf("up");


    printf("\n  Hardware is LAN-Extender");
    printf(", address is %e (bia %e)", lex_inventory->mac_addr,
           lex_inventory->mac_addr);

    lex_show_ether(idb, 
		   &(idb->lex_info->lex_stats->ether),
		   &(idb->lex_info->lex_stats->si_ether));
}

/*
 * this routine prints the top part of the "sh int lex ser" display
 */
static void lex_show_serial_interface(idbtype *idb)
{
    lex_stats_area_t *lex_stats;

    if (lex_show_interface_common(idb, LEX_SERIAL_INTF) == FALSE)
        return;

    lex_stats = idb->lex_info->lex_stats;
    /* 
     * If we got this far, the serial interface must be up.
     */
    printf("%s%s is %s, ", idb->namestring, "-Serial0", "up");

    printf("line protocol is %s", "up");
    printf("\n  Hardware is LAN-Extender");

    lex_show_serial(idb, 
		    &(idb->lex_info->lex_stats->serial),
		    &(idb->lex_info->lex_stats->si_serial));
}


/*
 * lex_show_ether
 * Show the statistics for the ethernet interface
 * of the remote cisco-1000.
 */
void lex_show_ether (idbtype *idb, 
		     lex_ether_counters_t *lc,
		     lex_ether_counters_t *si)
{
    lex_idb_info_t *lex_info;
    lex_info = idb->lex_info;

    printf("\n  Last input ");
    print_ticks_in_dhms(lc->lastinput_delta);
    printf(", output ");
    print_ticks_in_dhms(lc->lastoutput_delta);
    printf("\n  Last clearing of \"show interface\" counters ");
    print_dhms(lex_info->last_ether_cclear);


    printf("\n  Output queue %d/%d, %u drops",
	   lc->output_qcount, lc->output_qmaximum,
	   lc->output_qdrops - si->output_qdrops);
    
    printf("\n  Five minute input rate %lu bits/sec, %lu packets/sec",
           lc->rx_datarate*1000, lc->rx_packetrate);

    printf("\n  Five minute output rate %lu bits/sec, %lu packets/sec",
           lc->tx_datarate*1000, lc->tx_packetrate);

    printf("\n     %lu packets input, %lu bytes, %lu no buffer",
           lc->inputs - si->inputs, 
	   lc->rx_cumbytes - si->rx_cumbytes, 
	   lc->nobuffers - si->nobuffers);

    printf("\n     Received %lu broadcasts, %lu runts, %lu giants",
           lc->input_broad - si->input_broad, 
	   lc->runts - si->runts, 
	   lc->giants - si->giants);

    printf("\n     %lu input errors, %lu CRC, %lu frame, %lu overrun, %lu ignored, %lu abort",
           lc->input_error - si->input_error, 
	   lc->input_crc - si->input_crc,
           lc->input_frame - si->input_frame, 
	   lc->input_overrun - si->input_overrun,
           lc->input_resource - si->input_resource, 
	   lc->input_abort - si->input_abort);

    printf("\n     %lu packets output, %lu bytes, %lu underruns",
           lc->outputs - si->outputs, 
	   lc->tx_cumbytes - si->tx_cumbytes,
           lc->output_underrun - si->output_underrun);

    printf("\n     %lu output errors, %lu collisions, %lu interface resets, %lu restarts",
           lc->output_error - si->output_error, 
	   lc->output_collision - si->output_collision,
           lc->resets - si->resets, 0);

}


/*
 * lex_show_serial
 * Show the interface statistics for the serial interface
 * of the remote cisco-1000.
 */
void lex_show_serial (idbtype *idb, 
		      lex_serial_counters_t *lc,
		      lex_serial_counters_t *si)
{
    lex_idb_info_t *lex_info;
    lex_info = idb->lex_info;

    printf("\n  Last input ");
    print_ticks_in_dhms(lc->lastinput_delta);
    printf(", output ");
    print_ticks_in_dhms(lc->lastoutput_delta);
    printf("\n  Last clearing of \"show interface\" counters ");
    print_dhms(lex_info->last_serial_cclear);

    if (idb->lex_info->lex_priority_list) {
	   printf("\n  Total output drops: %u",
           lc->output_qdrops_low + lc->output_qdrops_normal + 
           lc->output_qdrops_medium + lc->output_qdrops_high -
	   si->output_qdrops_low - si->output_qdrops_normal -
           si->output_qdrops_medium - si->output_qdrops_high);

	printf("\n  Output queue: high %d/%d/%u, medium %d/%d/%u, normal %d/%d/%u, low %d/%d/%u",
	    lc->output_qcount_high,
	    lc->output_qmaximum_high,
	    lc->output_qdrops_high,
	    lc->output_qcount_medium,
	    lc->output_qmaximum_medium,
	    lc->output_qdrops_medium,
	    lc->output_qcount_normal,
	    lc->output_qmaximum_normal,
	    lc->output_qdrops_normal,
	    lc->output_qcount_low,
	    lc->output_qmaximum_low,
	    lc->output_qdrops_low);
    }
    else
	printf("\n  Output queue %d/%d, %u drops",
	    lc->output_qcount_normal, lc->output_qmaximum_normal,
	    lc->output_qdrops_normal - si->output_qdrops_normal);

    printf("\n  Five minute input rate %lu bits/sec, %lu packets/sec",
           lc->rx_datarate*1000, lc->rx_packetrate);

    printf("\n  Five minute output rate %lu bits/sec, %lu packets/sec",
           lc->tx_datarate*1000, lc->tx_packetrate);

    printf("\n     %lu packets input, %lu bytes, %lu no buffer",
           lc->inputs - si->inputs, 
	   lc->rx_cumbytes - si->rx_cumbytes, 
	   lc->nobuffers - si->nobuffers);

    printf("\n     Received %lu broadcasts, %lu runts, %lu giants",
	   0, 0, 0);

    printf("\n     %lu input errors, %lu CRC, %lu frame, %lu overrun, %lu ignored, %lu abort",
           lc->input_error - si->input_error, 
	   lc->input_crc - si->input_crc,
           lc->input_frame - si->input_frame, 
	   lc->input_overrun - si->input_overrun,
           lc->input_resource - si->input_resource, 
	   lc->input_abort - si->input_abort);

    printf("\n     %lu packets output, %lu bytes, %lu underruns",
           lc->outputs - si->outputs, 
	   lc->tx_cumbytes - si->tx_cumbytes,
           lc->output_underrun - si->output_underrun);

    printf("\n     %lu output errors, %lu collisions, %lu interface resets, %lu restarts",
           lc->output_error - si->output_error, 
	   0,0,0);

    printf("\n     %lu carrier transitions", 
	   lc->transitions - si->transitions);
    
}


void lex_show_interface (parseinfo *csb)
{
    idbtype *swidb;

    swidb = GETOBJ(idb,1);

    if (GETOBJ(int,8))
        lex_show_serial_interface(swidb);
    else if (GETOBJ(int,7))
        lex_show_ether_interface(swidb);
}


/*
 * lex_macaddr_command
 * Parser action routine for "[no] lex burned-in-address <ieee-address>"
 * command.
 * The generic "mac-address" command will not work with network protocols 
 * such as DECnet that change the MAC address.  Also, if a user enters 
 * "no mac-address", idb->hardware gets zeroed out, which could lead to 
 * addresses of 0 in the ARP cache, since lex interfaces, unlike other LAN 
 * interfaces, have no burned-in address to fall back on.
 * This command will only be allowed on lex interfaces that are not currently
 * bound to a serial interface.
 */

void lex_macaddr_command (parseinfo *csb)
{
    uchar *newmac;
    hwidbtype *idb;

    idb = csb->interface->hwptr;
    if (csb->nvgen) {
        nv_write((((ushort *)idb->bia)[0] |
                  ((ushort *)idb->bia)[1] |
                  ((ushort *)idb->bia)[2]),
                 "%s %e", csb->nv_command,
                 idb->bia);
        return;
    }
    if (csb->interface->lex_swidb != NULL) {
        printf("\nCannot set MAC address on active lex interfaces");
        return;
    }

    if (!csb->sense) {
        ieee_zero(idb->bia);
        ieee_zero(idb->hardware);
        return;
    }

    /*
     * Check for reasonable MAC address.  Don't allow Multicasts
     */
    newmac = GETOBJ(hwaddr,1)->addr;
    if (newmac[0] & 0x01) {
        printf("\n%% Malformed hex mac address");
        return;
    }
    ieee_copy(newmac, idb->bia);
    
    /*
     * Copy the bia into the hardware only if decnet or
     * xns is not configured on the interface.
     */
    if (!RUNNING_IF(LINK_DECNET, idb->firstsw) &&
	!RUNNING_IF(LINK_XNS, idb->firstsw) && is_ieee_zero(idb->forced_mac))
        coerce_hwaddr(newmac, idb);
}


/*
 * lex_soutput
 * The hwidb->soutput vector for lex interfaces.
 * Dequeue packets from the lex idb's ouput hold queue, but then turn
 * around and queue them onto the bound serial interface's hold queue
 * and invoke it's soutput routine.  This is ugly, but necessary for
 * supporting priority output queueing on lex interfaces.
 */

static void lex_soutput (hwidbtype *hwidb)
{
    paktype *pak;

    while ((pak = (*hwidb->oqueue_dequeue)(hwidb))) {
	hwidb->counters.outputs++;
	
	/*
	 * We remove  PPP_LEX_ENCAPBYTES from the datagramsize
	 * because it is a part of serial idb counters and not lex idb
	 * counters.
	 */
	hwidb->counters.tx_cumbytes += pak->datagramsize - PPP_LEX_ENCAPBYTES;
	lex_oqueue(hwidb, pak, TAIL);
    }
}

/*
 * LEX subsystem header
 */

#define LEX_MAJVERSION 1
#define LEX_MINVERSION 0
#define LEX_EDITVERSION 0

SUBSYS_HEADER(lex, LEX_MAJVERSION, LEX_MINVERSION, LEX_EDITVERSION,
	      lex_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: ppp")


