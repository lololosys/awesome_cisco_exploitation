/* $Id: if_pas_dec21140_chain.c,v 3.1.64.3 1996/06/17 23:36:17 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_dec21140_chain.c,v $
 *------------------------------------------------------------------
 * if_pas_dec21140_chain.c - Parser support for 1fe Port Adaptor
 *
 * December 1995, Michael Beesley
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_dec21140_chain.c,v $
 * Revision 3.1.64.3  1996/06/17  23:36:17  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/04/23  01:50:54  ssangiah
 * CSCdi55246:  CLI for configuring full-duplex on Fast-Ethernet PAs broken
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/03/21  23:23:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/03/18  23:28:52  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.2  1996/02/08  08:47:58  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.1  1995/12/06  23:13:46  mbeesley
 * Branch: ELC_branch
 * Add fastethernet parser support for Predator.
 *
 * Revision 3.1  1995/12/06  21:46:53  mbeesley
 * Add placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"
#include "registry.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "sched.h"
#include "parser.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_private.h"

#include "../hes/if_fci.h"
#include "../hes/if_feip.h"
#include "../if/parser_defs_fast_ethernet.h"
#include "../parser/parser_defs_half_duplex.h"

/*
 * dec21140_media_type_command:
 * Generic media type wrapper for DEC21140
 */
static void dec21140_media_type_command (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    enum MEDIA_TYPE old_type;
    ethersb *esb;

    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {

	case PARSER_MEDIATYPE_100BASET:
	    /* Default, No NV generation */
	    break;
	case PARSER_MEDIATYPE_MII:
	    nv_write(esb->media_type == FEIP_MII, csb->nv_command);
	    break;
	default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
	return;
    }

    old_type = esb->media_type;
    if (!csb->sense) {
	/* "no media-type" should reset the default */
	esb->media_type = FEIP_RJ45;
    } else {
	switch (GETOBJ(int,1)) {
	  case PARSER_MEDIATYPE_100BASET:
	    esb->media_type = FEIP_RJ45;
	    break;
	  case PARSER_MEDIATYPE_MII:
	    esb->media_type = FEIP_MII;
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
    }
    if (old_type != esb->media_type)
	(*hwidb->reset)(hwidb);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
}

/*
 * dec21140_full_duplex_command:
 * Configure FastEthernet interface in full-duplex mode
 */
static void dec21140_full_duplex_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    ethersb *esb;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    esb = idb_use_hwsb_inline(hwidb, HWIDB_SB_ETHER);

    if (csb->nvgen) {
        if (csb->which == CFG_HDX_SET_FULL_DUPLEX)
            nv_write(esb->feip_fullduplex, csb->nv_command);
    } else {
        if (csb->sense) {
            switch (csb->which) {
                case CFG_HDX_SET_FULL_DUPLEX:
                    esb->feip_fullduplex = TRUE;
                    break;
                case CFG_HDX_SET_HALF_DUPLEX:
                    esb->feip_fullduplex = FALSE;
                    break;
                default:
                    break;
            }
        }
        else {
            switch (csb->which) {
                case CFG_HDX_SET_FULL_DUPLEX:
                    esb->feip_fullduplex = FALSE;
                    break;
                case CFG_HDX_SET_HALF_DUPLEX:
                    esb->feip_fullduplex = TRUE;
                    break;
                default:
                    break;
            }
        }
        (*hwidb->reset)(hwidb);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_ETHER);
}

/*
 * feipui_init:
 * Initialize 1fe parse chains
 */
static void feipui_init (subsystype *subsys)
{
    reg_add_media_type_cmd(IDBTYPE_FEIP, dec21140_media_type_command,
		           "dec21140_media_type_command");
    reg_add_media_half_or_full_duplex_cmd(IDBTYPE_FEIP, 
                                          dec21140_full_duplex_command,
                                         "dec21140_full_duplex_command");
    fast_ethernet_debug_init();
    fast_ethernet_parser_init();
}

/*
 * FEIP UI subsystem header
 */
#define FEIPUI_MAJVERSION   1
#define FEIPUI_MINVERSION   0
#define FEIPUI_EDITVERSION  1

SUBSYS_HEADER(feipui, 
	      FEIPUI_MAJVERSION, 
	      FEIPUI_MINVERSION, 
	      FEIPUI_EDITVERSION,
	      feipui_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL, NULL);

/* end of file */
