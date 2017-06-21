/* $Id: aip_parse_support.c,v 3.1.4.4 1996/08/28 12:43:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/aip_parse_support.c,v $
 *------------------------------------------------------------------
 * Support for aip specific interface commands
 *
 * March 1996, Rob Zagst
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aip_parse_support.c,v $
 * Revision 3.1.4.4  1996/08/28  12:43:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.4.3  1996/05/21  09:47:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.2  1996/05/09  14:28:26  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1.2.1  1996/04/27  07:02:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:11:51  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/30  04:05:22  rzagst
 * Place for aip_parse_chain.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../atm/parser_defs_atm.h"
#include "../lane/parser_defs_lane.h"
#include "../wan/serial.h"
#include "address.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../atalk/atalk.h"
#include "../parser/parser_actions.h"
#include "../srt/span.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../if/static_map.h"
#include "../atm/atmsig_api.h"
#include "../atm/sscop_public.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_ie.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_public.h"
#include "../atm/atmsig_smap.h"
#include "../wan/smds.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"
#include "../lane/lanefast_les.h"

#include "fastswitch_registry.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../atm/atm_oam.h"
#include "../atm/atm_arp.h"
#include "../atm/ilmi_public.h"

#include "../atm/atm_registry.h"
#include "../atm/atm_registry.regc"
#include "../llc2/llc2.h"

/* Wired ATM descriptor blocks to cover the OIR case where we can't alloc
 * these puppies in the atm_setup() routine.  This routine may be called
 * from interrupt side where malloc() is not allowed!
 */
extern atm_db_t atm_db[MAX_ATM_INTFC];


/**************************Subfunctions***********************************/
/*
 * Section: Subroutines
 *	This is the subroutines section.  Contains routines for parsing
 *	protocol addresses, movement of strings, searching for static
 *	mapping entries, adding static maps, etc...
 */
/*
 * chk if any vcnum in the range of [l_vcnum .. h_vcnum]
 * is being used. return TRUE if yes; otherwise return FALSE
 */
static boolean
any_atm_vcnum_inuse (atm_db_t *atm, vcd_t l_vcnum, vcd_t h_vcnum)
{
    ushort ncd;
    for (ncd=l_vcnum; ncd<=h_vcnum; ncd++) {
	if (atm_getvc(atm, ncd))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * atm_aal34_remove_ok:  A simple check function.  Determines if it's
 *	save to 'no atm aal aal3/4' on the interface.
 *
 *	FALSE: Means the user still has AAL3/4 specific config.s on the
 *		interface, or subinterfaces to remove.
 *	TRUE: Means that it's save to disable AAL3/4 processing on the
 *		interface.
 */
static inline boolean
atm_aal34_remove_ok (hwidbtype *hwidb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (atm_get_aal34vcnum(swidb)) {
	    return(FALSE);
	}
	if (swidb->vaddr) {
	    return(FALSE);
	}
	if (atm_get_multicast(swidb)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * aip_interface_commands:	Called to do aip ATM command processing.
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	csb = Ptr. to the Parse block with command & arguments.
 *	csb->which = aip ATM Command type
 *	csb->sense = [NO] value(0 = NO), (1 = !NO)
 */
void
atm_aip_interface_commands (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    atm_db_t *atm;

    idb = csb->interface;
    if (idb) {
	hwidb = idb->hwptr;
    } else {
	printf("\nerror: source idb not set");
	return;
    }
    atm = hwidb->atm_db;

/*
 * csb->nvgen == TRUE when the 'write [term || mem || net]' command is
 * entered.  Otherwise, it's false to indicate that a command is being
 * specified by the user.
xo */
    if (csb->nvgen) {
	switch(csb->which) {
	case ATM_CMD_AAL:
	    nv_write(GETOBJ(int,1) && (atm->config.mode & ATM_CFG_MODE_AAL34), 
		csb->nv_command);
	    nv_write(!GETOBJ(int,1) && !(atm->config.mode & ATM_CFG_MODE_AAL5),
		"no %s", csb->nv_command);
	    break;
	case ATM_CMD_TXBUFF:
	    nv_write(atm->config.tx_buff != ATM_TXBUFF_DEFAULT, "%s %d",
		csb->nv_command, atm->config.tx_buff);
	    break;
	case ATM_CMD_RXBUFF:
	    nv_write(atm->config.rx_buff != ATM_RXBUFF_DEFAULT, "%s %d",
		csb->nv_command, atm->config.rx_buff);
	    break;
	case ATM_CMD_EXCQ:
	    nv_write(atm->config.exq != ATM_EXCQ_DEFAULT, "%s %d",
		csb->nv_command, atm->config.exq);
	    break;
	case ATM_CMD_RAWQ:
	    nv_write(atm->config.raw_queue_size != ATM_RAWQ_DEFAULT, "%s %d",
		csb->nv_command, atm->config.raw_queue_size);
	    break;
	case ATM_CMD_VPFILTER:
	    nv_write(atm->config.vp_filter != ATM_VP_FILTER_DEFAULT, "%s %x",
		csb->nv_command, atm->config.vp_filter);
	    break;
	case ATM_CMD_VC:
	    nv_write(atm->config.max_intf_vcs != ATM_MAX_VC_DEFAULT,
		     "%s %d",csb->nv_command, atm->config.max_intf_vcs);
	    break;
	case ATM_CMD_MID_PER_VC:
	    nv_write((int)atm->config.mid_per_vc != ATM_MID_DEFAULT, 
		"%s %d", csb->nv_command, atm->config.mid_per_vc);
	    break;
	default:
	    bad_parser_subcommand(csb,csb->which);
	    break;
	}
	return;
    }

/*
 * csb->nvgen == FALSE.  Go process user specific command
 */
    switch(csb->which) {
	case ATM_CMD_AAL:
	    if (csb->sense) {
		if(GETOBJ(int,1)) {
		    atm->config.mode |= ATM_CFG_MODE_AAL34;
		} else {
		    atm->config.mode |= ATM_CFG_MODE_AAL5;
		}
	    } else {
	    /*
	     * Set/Clear the appropriate AAL abilities.
	     * NOTE: AAL5 is the default if nothing else is specified!
	     *	[NO] AAL5 will revert to AAL5 only if AAL3/4 is not enabled.
	     */
		if(GETOBJ(int,1)) {
		    if(atm_aal34_remove_ok(hwidb)) {
		    	atm->config.mode &= ~ATM_CFG_MODE_AAL34;
		    	atm->config.mid_per_vc = ATM_MID_DEFAULT;
		    } else {
			printf("\n%%ATM: Can't disable AAL3/4 until other AAL3/4 configs are removed.");
		    }
		} else {
		    if(atm->config.mode & ATM_CFG_MODE_AAL34)
			atm->config.mode &= ~ATM_CFG_MODE_AAL5;
		    else
			atm->config.mode |= ATM_CFG_MODE_AAL5;
		}
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_TXBUFF:
	    if (csb->sense) {
		atm->config.tx_buff = GETOBJ(int,1);
	    } else {
		atm->config.tx_buff = ATM_TXBUFF_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_RXBUFF:
	    if (csb->sense) {
		atm->config.rx_buff = GETOBJ(int,1);
	    } else {
		atm->config.rx_buff = ATM_RXBUFF_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_EXCQ:
	    if (csb->sense) {
		atm->config.exq = GETOBJ(int,1);
	    } else {
		atm->config.exq = ATM_EXCQ_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_RAWQ:
	    if (csb->sense) {
		atm->config.raw_queue_size = GETOBJ(int,1);
	    } else {
		atm->config.raw_queue_size = ATM_RAWQ_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_VPFILTER:
	    if (csb->sense) {
		atm->config.vp_filter = GETOBJ(int,1);
	    } else {
		atm->config.vp_filter = ATM_VP_FILTER_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_VC:
	    if (csb->sense) {
		uint temp = GETOBJ(int,1);
		/*
		 * disallow to shrink if any vc w/ vcnum in the range of
		 * [temp .. atm->config.max_intf_vcs-1] exist.
		 */
		if ((temp < atm->config.max_intf_vcs) &&
		    any_atm_vcnum_inuse(atm, temp,
					atm->config.max_intf_vcs-1))  {
			printf("\n%%Change maxvc failed: all VCDs from %d to %d must be free", temp, atm->config.max_intf_vcs-1);
			return;
		    }
		
		atm->config.max_intf_vcs = temp;
	    } else {
		atm->config.max_intf_vcs = ATM_MAX_VC_DEFAULT;
	    }
	    atm->config.rx_vc = atm->config.max_intf_vcs;
	    atm_newconfig(atm);
	    break;
	case ATM_CMD_MID_PER_VC:
	    if(!(atm->config.mode & ATM_CFG_MODE_AAL34)) {
		printf("\n%% AAL3/4 not enabled");
		return;
	    }
	    if (csb->sense) {
		atm->config.mid_per_vc = GETOBJ(int,1);
	    } else {
		atm->config.mid_per_vc = ATM_MID_DEFAULT;
	    }
	    atm_newconfig(atm);
	    break;
	default:
	    bad_parser_subcommand(csb,csb->which);
	    break;
    }
}


      
