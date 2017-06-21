/* $Id: lex_snmp.c,v 3.2.60.2 1996/05/21 09:56:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex_snmp.c,v $
 *------------------------------------------------------------------
 * snmp support for lex, Cisco 1000
 *
 * November 1994, steve elias
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * this file contains snmp support routines for the Cisco 1000
 *------------------------------------------------------------------
 * $Log: lex_snmp.c,v $
 * Revision 3.2.60.2  1996/05/21  09:56:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.1  1996/05/21  06:35:48  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2  1995/11/17  17:43:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/30  01:49:40  sdurham
 * CSCdi39454:  ifType for ATM interface returns 0 - other
 *              changed syntax of ifType to IANAifType to match RFC1573
 *              (as per code review)
 *
 * Revision 2.1  1995/06/07  21:41:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "packet.h"
#include "config.h"
#include "../if/network.h"
#include "access.h"
#include "ieee.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "../if/if_vidb.h"
#include "../snmp/sr_ifmibdefs.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "../snmp/ifmib_registry.h"

/*
 * try to update remote lex box ether or serial stats.
 * return TRUE if successful.
 * input parameters:
 * iab - pointer to iab for which statistics need to be updated
 * ether_or_serial - set to either LEX_SERIAL_STATS or LEX_ETHER_STATS. 
 *
 * please note that the routine lex_show_interface_common() in 
 * sys/lex/lex_rcmd.c also sends rcmds.  possibly the common code
 * should be combined at some point for ease of maintenance.
 */
static boolean iab_lex_get_stats (iabtype *iab, ulong ether_or_serial)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_rcmdst_type *rcmd;
    lex_command *rcmd_headstart;
    boolean retval = FALSE;

    if ((ether_or_serial != LEX_SERIAL_STATS) &&
	(ether_or_serial != LEX_ETHER_STATS) )
      return(retval);

    rcmd = lex_setup_rcmd(LEX_STATS,swidb,TRUE);
    if ( !rcmd ) { 
	/* no need for lex_rcmd_cleanup(), because command not sent yet. */
	return(retval);
    }
    rcmd_headstart = (lex_command *)lex_rcmd_headstart(rcmd->pak);  
    /* not ether_or_serial sanity checked above, so not needed here */
    if (ether_or_serial == LEX_SERIAL_STATS)
	rcmd_headstart->opt.flags = LEX_RESET_SERIAL_STATS | LEX_SERIAL_STATS;
    else if (ether_or_serial == LEX_ETHER_STATS)
	rcmd_headstart->opt.flags = LEX_RESET_ETHER_STATS | LEX_ETHER_STATS;
    lex_send_rcmd(rcmd);
    edisms((blockproc *)lex_rcmdBLOCK, (ulong)rcmd);
    if (rcmd->status != LEX_BLOCK_ACK) {  
        if (lex_rcmd_debug)
	  buginf("\nLEX-RCMD: iab/get_stats failure %d",rcmd->status);
    }
    else retval = TRUE;
    lex_rcmd_cleanup(rcmd);
    return(retval);
}

/*
 * update lex serial stats if possible, 
 * and return TRUE no matter if the update was successful.
 * (snmp gets confused if FALSE is returned.)
 */
static boolean iab_lex_serial_get_stats (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_serial_counters_t *serial = &swidb->lex_info->lex_stats->serial;
    iab_lex_get_stats(iab,LEX_SERIAL_STATS);
    iab->input_broad = 0;
    iab->nobuffers = serial->nobuffers;
    iab->input_error = serial->input_error;
    iab->output_error = serial->output_error;
    return(TRUE);
}

/*
 * update lex ether stats if possible, 
 * and return TRUE no matter if the update was successful.
 */
static boolean iab_lex_ether_get_stats (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_ether_counters_t *ether = &swidb->lex_info->lex_stats->ether;
    iab_lex_get_stats(iab,LEX_ETHER_STATS);
    
    /* here we set variable for ifInNUcastPkts semi-correctly. */
    /* we are setting it with the # of broadcasts rather than */
    /* the number of non unicasts.  this is a bug.  */
    iab->input_broad = ether->input_broad;
    iab->nobuffers = ether->nobuffers;
    iab->input_error = ether->input_error;
    iab->output_error = ether->output_error;

    return(TRUE);
}

/*
 * here are the lex iab callback routines which
 * return SNMP Object values to the snmp code.
 * see interface.h for more info on these callbacks,
 * as well as snmp/sr_ifmib2.c.
 */

static boolean iab_lex_if_up (iabtype *iab)
{
    idbtype *swidb = (idbtype *) iab->data_ptr;
    return(interface_up(swidb));
}

static boolean iab_lex_ether_up (iabtype *iab)
{
    idbtype *swidb = (idbtype *) iab->data_ptr;
    if ( swidb->lex_info->last_remote_ether_lineproto != 
	LEXETHER_LINEPROTOCOL_DOWN)
      return(TRUE);
    return(FALSE);
}

static ulong iab_lex_serial_rx_cumbytes (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_idb_info_t *lex_info = swidb->lex_info;
    lex_stats_area_t *lex_stats = lex_info->lex_stats;
    lex_serial_counters_t *serial_counters = &lex_stats->serial;
    return(serial_counters->rx_cumbytes);
}

static ulong iab_lex_serial_tx_cumbytes (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_serial_counters_t *serial_counters = &swidb->lex_info->lex_stats->serial;
    return(serial_counters->tx_cumbytes);
}

static ulong iab_lex_serial_inputs (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_serial_counters_t *serial_counters = &swidb->lex_info->lex_stats->serial;
    return(serial_counters->inputs);
}

static ulong iab_lex_serial_outputs (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_serial_counters_t *serial_counters = &swidb->lex_info->lex_stats->serial;
    return(serial_counters->outputs);
}

static ulong iab_lex_serial_oqueue_len (iabtype *iab)
{
    idbtype *swidb = (idbtype *) iab->data_ptr;
    lex_idb_info_t *lex_info = swidb->lex_info;
    lex_stats_area_t *lex_stats = lex_info->lex_stats;
    lex_serial_counters_t *lexctrs = &lex_stats->serial;
    return (lexctrs->output_qcount_high + lexctrs->output_qcount_medium +
	    lexctrs->output_qcount_normal + lexctrs->output_qcount_low);
}


static long iab_lex_serial_iftype (iabtype *iab)
{
        return (D_ifType_ppp);
}

static ulong iab_lex_ether_rx_cumbytes (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_ether_counters_t *ether_counters = &swidb->lex_info->lex_stats->ether;
    return(ether_counters->rx_cumbytes);
}

static ulong iab_lex_ether_tx_cumbytes (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_ether_counters_t *ether_counters = &swidb->lex_info->lex_stats->ether;
    return(ether_counters->tx_cumbytes);
}

static ulong iab_lex_ether_inputs (iabtype *iab)
{ 
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_ether_counters_t *ether_counters = &swidb->lex_info->lex_stats->ether;
    return(ether_counters->inputs);
}

static ulong iab_lex_ether_outputs (iabtype *iab)
{
    idbtype *swidb = (idbtype *)iab->data_ptr;
    lex_ether_counters_t *ether_counters = &swidb->lex_info->lex_stats->ether;
    return(ether_counters->outputs);
}

static ulong iab_lex_ether_oqueue_len (iabtype *iab)
{
    idbtype *swidb = (idbtype *) iab->data_ptr;
    lex_idb_info_t *lex_info = swidb->lex_info;
    lex_stats_area_t *lex_stats = lex_info->lex_stats;
    lex_ether_counters_t *lexctrs = &lex_stats->ether;
    return (lexctrs->output_qcount);
}

static long iab_lex_ether_iftype (iabtype *iab)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    swidb = iab->data_ptr;
    hwidb = swidb->hwptr;
    if ( hwidb->enctype == ET_ARPA )
      return(D_ifType_ethernetCsmacd);
    else if ( hwidb->enctype == ET_SNAP )
      return(D_ifType_iso88023Csmacd);
    else
      return(D_ifType_other);
}

/*
 * setup_lex_iabs() creates iabs for a new lex
 * 
 * call it only if the physical lex which binds to
 * a virtual lex interface is different from the previous
 * physical lex which was bound to that particular virtual lex.
 */ 
boolean setup_lex_iabs (idbtype *swidb, lex_info_t *info)
{
    iabtype *lex_serial_iab = NULL, *lex_ether_iab = NULL;
    boolean needto_register = FALSE;   /* initialized to quiet compiler */
    if ( info->serial_iab ) { /* iabs for this virtual lex exist */
	lex_serial_iab = info->serial_iab;
	lex_ether_iab = info->ether_iab;
	needto_register = !ieee_equal(swidb->hwptr->bia,info->last_bia);
	}
    else {
	lex_serial_iab = malloc(sizeof(iabtype));
	if ( !lex_serial_iab ) 
	  return(FALSE);
	lex_ether_iab = malloc(sizeof(iabtype));
	if ( !lex_ether_iab ) {
	    free(lex_serial_iab);
	    return(FALSE);
	}
	needto_register = TRUE;
    }
    /* 
     * initialize the iab function vectors,
     * and a couple of iab fields which will never change.
     */
    lex_serial_iab->get_stats = iab_lex_serial_get_stats;
    lex_serial_iab->data_ptr = (void *)swidb;
    lex_serial_iab->iftype_get = iab_lex_serial_iftype;
    lex_serial_iab->hw_if_up = iab_lex_if_up;
    lex_serial_iab->rx_cumbytes = iab_lex_serial_rx_cumbytes;
    lex_serial_iab->tx_cumbytes = iab_lex_serial_tx_cumbytes;
    lex_serial_iab->inputs = iab_lex_serial_inputs;
    lex_serial_iab->outputs = iab_lex_serial_outputs;
    lex_serial_iab->oqueue_len = iab_lex_serial_oqueue_len;
    lex_serial_iab->update_ifalias = NULL;
    if ( !lex_serial_iab->if_namestring ) 
      lex_serial_iab->if_namestring = malloc(MAX_LEX_IFDESCR_LENGTH);
    if ( !lex_serial_iab->if_namestring ) {
	free(lex_serial_iab);
	free(lex_ether_iab);
	return(FALSE);
    }
    sprintf(lex_serial_iab->if_namestring,"%s remote serial",
	    swidb->namestring);
    lex_serial_iab->maxmtu = swidb->lex_swidb->hwptr->maxmtu;

    lex_ether_iab->data_ptr = (void *)swidb;
    lex_ether_iab->iftype_get = iab_lex_ether_iftype;
    lex_ether_iab->get_stats = iab_lex_ether_get_stats;
    lex_ether_iab->oqueue_len = iab_lex_ether_oqueue_len;
    lex_ether_iab->hw_if_up = iab_lex_ether_up;
    lex_ether_iab->rx_cumbytes = iab_lex_ether_rx_cumbytes;
    lex_ether_iab->tx_cumbytes = iab_lex_ether_tx_cumbytes;
    lex_ether_iab->inputs = iab_lex_ether_inputs;
    lex_ether_iab->outputs = iab_lex_ether_outputs;
    lex_ether_iab->oqueue_len = iab_lex_ether_oqueue_len;
    lex_ether_iab->update_ifalias = NULL;
    if ( !lex_ether_iab->if_namestring ) 
      lex_ether_iab->if_namestring = malloc(MAX_LEX_IFDESCR_LENGTH);
    if ( !lex_ether_iab->if_namestring ) {
	free(lex_serial_iab->if_namestring);
	free(lex_serial_iab);
	free(lex_ether_iab);
	return(FALSE);
    }
    sprintf(lex_ether_iab->if_namestring,"%s remote ethernet",
	    swidb->namestring);
    lex_ether_iab->maxmtu = MAXETHERSIZE;
    lex_ether_iab->status = IDB_LEX | IDB_ETHER;

    /* 
     * if necessary,
     * register the interface accounting blocks for lex-eth and lex-serial.
     */
    if ( needto_register ) {
	/* deregister the old iabs if they exist */
	if ( info->serial_iab ) 
	  reg_invoke_ifmib_deregister_iab(info->serial_iab);
	if ( info->ether_iab ) 
	reg_invoke_ifmib_deregister_iab(info->ether_iab);

	/* register the new iabs */
	lex_serial_iab->snmp_if_index = 
	    reg_invoke_ifmib_register_iab(lex_serial_iab);
	if ( !lex_serial_iab->snmp_if_index ) 
	  buginf("\nLEX: failure to register lex serial iab");
	lex_ether_iab->snmp_if_index = 
	    reg_invoke_ifmib_register_iab(lex_ether_iab);
	if ( !lex_ether_iab->snmp_if_index  ) 
	  buginf("\nLEX: failure to register lex ether iab");
    	info->serial_iab = lex_serial_iab;
	info->ether_iab = lex_ether_iab;
    }

    info->ether_iab->visible_bandwidth_ptr = &swidb->visible_bandwidth;
    info->serial_iab->visible_bandwidth_ptr = &swidb->visible_bandwidth;
    GET_TIMESTAMP(info->ether_iab->state_time);
    GET_TIMESTAMP(info->serial_iab->state_time);
    return(TRUE);
}

