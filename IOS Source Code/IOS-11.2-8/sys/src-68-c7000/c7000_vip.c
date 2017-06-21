/* $Id: c7000_vip.c,v 3.5.20.2 1996/07/23 13:28:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/c7000_vip.c,v $
 *------------------------------------------------------------------
 * c7000_vip.c: RP specific code for the VIP
 *
 * August 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7000_vip.c,v $
 * Revision 3.5.20.2  1996/07/23  13:28:24  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.20.1  1996/03/18  22:00:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/02/20  17:57:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/26  21:04:03  rharagan
 * CSCdi47313:  4T love letters lost when non-default encaps used
 * Use idb->rxoffset to find encap for serial media.
 *
 * Revision 3.4.20.1  1996/01/24  23:02:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/20  22:38:41  snyder
 * CSCdi44202:  spellink errors
 *              Could'nt -> Couldn't
 *
 * Revision 3.2  1995/11/17  18:44:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/31  01:26:05  sthormod
 * placeholder for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"

#include "../ui/debug.h"
#include "../if/rvip_debug_flags.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/if_vip.h"
#include "../hes/if_cmd.h"


/*
 * vip_love_letter:
 * Recieve a VIP love letter. Only gets called on RP/SP systems
 */
static void vip_love_letter (hwidbtype *hwidb, ulong bytes)
{
    paktype *pak;
    mcitype *cbus;
    
    cbus = hwidb->devctl;
        
    if (bytes <= 0) {
	return;
    }

    if (rvip_debug) { 
	buginf("\n%s: love letter received, %d bytes", hwidb->hw_namestring,
	       bytes);
    } 

    pak = getbuffer(bytes);
    if (!pak) {
	return;
    }
    mci2buffer(&cbus->regptr->readlong, pak->datagramstart, bytes);
    vip_parse_love_letter(hwidb, (uchar *)pak->datagramstart, bytes);
    retbuffer(pak);
}

/*
 * vip_sniff_encap
 */
boolean vip_sniff_encap (mcitype *cbus, hwidbtype *hwidb)
{
    ushort offset, result;
    mciregtype *regptr = cbus->regptr;
    ulong bytes;

    bytes = regptr->rxlength;

    switch (hwidb->type) {
    case IDBTYPE_FEIP:
    case IDBTYPE_CBUSETHER:
	offset = MCI_ETHER_OFFSET + VIP_LL_ETHER_OFFSET;
	bytes -= wtob(VIP_LL_ETHER_OFFSET);
	break;
    case IDBTYPE_CTR: 
	offset = CTR_RECEIVE_OFFSET + VIP_LL_TOKEN_OFFSET;
	bytes -= wtob(VIP_LL_TOKEN_OFFSET);
	break;
    case IDBTYPE_CBUSSERIAL:
	offset = hwidb->rxoffset + VIP_LL_SERIAL_OFFSET;
	bytes -= wtob(VIP_LL_SERIAL_OFFSET);
	break;
    default:
	return FALSE;
    }
    
    regptr->argreg = offset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    result = regptr->readshort; 

    if (result == 0xdead) {
	vip_love_letter(hwidb, bytes);
	return(TRUE);
    } 

    return(FALSE);
} 




