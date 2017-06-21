/* $Id: if_c4000_lance_parser.c,v 3.5.12.3 1996/06/17 23:34:58 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_lance_parser.c,v $
 *------------------------------------------------------------------
 * C4000 lance driver parser routines
 *
 * Sept. 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_lance_parser.c,v $
 * Revision 3.5.12.3  1996/06/17  23:34:58  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/10  19:39:19  kew
 * CSCdi53129:  Unnecessary media-type AUI complaints at boot time
 * Branch: California_branch
 * Complain only if AUI is configured for 6E NIM.
 *
 * Revision 3.5.12.1  1996/03/18  20:41:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.28.2  1996/02/20  01:08:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  21:26:14  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1996/02/07  16:13:55  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:06:53  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2.28.1  1996/01/24  22:23:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/16  23:58:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:06:25  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "logger.h"
#include "if_les.h"
#include "if_les_lance.h"
#include "parser.h"
#include "parser_defs_xx.h"


/*
 * Generic media type wrapper for Lance/Am79970
 */
void lance_media_type_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    lance_instance_t *ds;
    ulong new_media_type;
    ethersb *esb;

    /* Check that this idb really is an Ethernet */
    if (idb->type != IDBTYPE_LANCE) {
	if (! csb->nvgen) {
	    printf("\ninterface type must be Ethernet");
	}
	return;
    }

    esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);

    new_media_type = esb->media_type;

    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case PARSER_MEDIATYPE_AUI:
	    /* Default, No NV generation */
	    break;
	  case PARSER_MEDIATYPE_10BASET:
	    nv_write((esb->media_type == ETHER_MEDIA_TYPE_10BASET),
		     csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
	return;
    }

    if (!csb->sense) {
	/* "no media-type" should reset the default */
	new_media_type = ETHER_MEDIA_TYPE_AUI;
    } else {
	switch (GETOBJ(int,1)) {
	  case PARSER_MEDIATYPE_AUI:
	    new_media_type = ETHER_MEDIA_TYPE_AUI;
	    break;
	  case PARSER_MEDIATYPE_10BASET:
	    new_media_type = ETHER_MEDIA_TYPE_10BASET;
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
    }

    ds = INSTANCE;

    if (new_media_type != esb->media_type) {
	esb->media_type = new_media_type;
	if (ds->set_media_type) {
	    (*ds->set_media_type)(idb);
	} else if (new_media_type == ETHER_MEDIA_TYPE_AUI) {
	    printf("\n media-type AUI not available for this interface.");
	}
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
}


/*
 * Generic squelch wrapper for Lance/Am79970
 */
void lance_squelch_command (parseinfo *csb)
{
    hwidbtype *idb;
    lance_instance_t *ds;
    int command;
    
    idb = csb->interface->hwptr;
    
    /* Check that this idb really is an Ethernet */
    if (idb->type != IDBTYPE_LANCE) {
	if (!csb->nvgen) {
	    printf("\ninterface type must be Ethernet");
	}
	return;
    }

    ds = INSTANCE;

    command = GETOBJ(int,1);

    if (csb->nvgen) {
	switch (command) {
	  case PARSER_SQUELCH_NORMAL:
	    /* Default, No NV generation */
	    break;
	  case PARSER_SQUELCH_REDUCED:
	    nv_write(((*ds->get_squelch) (idb)), csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, command);
	    break;
	}
	return;
    }
    
    switch (command) {
      case PARSER_SQUELCH_NORMAL:
      case PARSER_SQUELCH_REDUCED:
	(*ds->set_squelch) (idb, command);
	break;
      default:
	bad_parser_subcommand(csb, command);
	break;
    }
}
