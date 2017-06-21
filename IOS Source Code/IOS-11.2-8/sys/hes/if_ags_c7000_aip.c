/* $Id: if_ags_c7000_aip.c,v 3.3.60.1 1996/08/13 02:17:19 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_ags_c7000_aip.c,v $
 *------------------------------------------------------------------
 * ATM Interface Processor support code specific to the AGS+ and the
 *     RP1 (C/7000)
 *
 * February 1994, Jack Jenney
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_ags_c7000_aip.c,v $
 * Revision 3.3.60.1  1996/08/13  02:17:19  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3  1995/11/17  09:16:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:40:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "if_cmd.h"
#include "if_mci.h"
#include "if_fci.h"
#include "address.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/atm_debug_flags.h"
#include "../if/if_controller.h"
#include "hes_controller.h"
#include "if_aip.h"
#include "if_hes_common.h"
#include "fastswitch_registry.h"


/*****************************************************************************
 * Name: aip_restart_init
 *
 * Description: Post AIP restart reinitialization
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_restart_init (hwidbtype *hwidb)
{
    /*
     * Re-initialize the one and only interface:
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool.  
     */
    select_if(hwidb);
    if (!(hwidb->oir_flags & EOIR_RESET)) {
	/* Sending these down causes fred chip screwup. */
	send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
	send_rx_offset_cmd(hwidb,hwidb->rxoffset);
    }
    send_assign_poolindex_cmd(hwidb,hwidb->buffer_pool);
    send_rx_error_cmd(hwidb,FCI_RX_ERROR_POOL);
}

/*****************************************************************************
 * Name: aip_love_note
 *
 * Description: love note handler for AIPs
 *   This routine handles the love notes from the AIP
 *  
 * Input: 
 *   cdb -- pointer to the controller data block for the AIP interface that
 *     generated the event (love note)
 *   asr -- AIP status / result information from love note
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_love_note (cdbtype *cdb, ushort asr)
{
    platform_hware_handle_type *hware_handle;
    hwidbtype *hwidb;

    hware_handle = cdb->platform_hware_handle;
    hwidb = hware_handle->interface_list[cdb->unit];

    if (atm_event_debug && asr != 0x1) {
	buginf("\naip_love_note(%s): asr=0x%x", hwidb->hw_namestring, asr);
    }

    if (asr & FCI_COND_DCD)
	cbus_mci_carrier(hwidb);

    aip_love_note_common(hwidb, asr);
}

/*****************************************************************************
 * Name: aip_get_nlpidtype
 *
 * Description: platform-specific retrieval of NLPID from MEMD
 *		Only valid for use when the atmtype is ATM_ENCTYPE_NLPID
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for this interface
 *   encaps -- pointer to the incoming atm encapsulation
 *
 * Return Value: nlpidtype value (ushort)
 *
 *****************************************************************************/

static ushort aip_get_nlpidtype (hwidbtype *hwidb, uchar *encaps)
{
    mciregtype *inreg = (mciregtype *)hwidb->mci_regptr;

    inreg->argreg = MCI_OFFSETSIZE;
    inreg->cmdreg = MCI_CMD_RX_SELECT;

    return(inreg->readshort);
}

/*****************************************************************************
 * Name: aip_sniff
 *
 * Description: 7000 specific routine "sniff" words from MEMD.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for this interface
 *   pak -- pak pointer (unused)
 *   data -- pointer first long from packet (unused)
 *   sniff_buffer -- pointer to sniff buffer
 *   offset -- offset in 16-bit words from rxoffset to data to sniff
 *   num_shorts -- number of 16-bit words to sniff
 *
 * Return Value: pointer to data buffer
 *
 *****************************************************************************/
ushort *
aip_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
           ushort *sniff_buffer, int offset, int num_shorts)
{
    mciregtype  *inreg = (mciregtype *)hwidb->mci_regptr;
    ushort      *ptr   = sniff_buffer;

    inreg->argreg = hwidb->rxoffset + offset;
    inreg->cmdreg = MCI_CMD_RX_SELECT;

    while (num_shorts--) {
        *ptr++ = inreg->readshort;
    }
    return(sniff_buffer);
}

/*****************************************************************************
 * Name: send_aip_cmdrev_cmd
 *
 * Description: Send the current AIP H/W Rev. to the AIP F/W.
 *	Works around a H/W bug.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   status -- the status of the command: CMD_RSP_OKAY if successful,
 *     otherwise and error code.	
 *   *NOTE* Status MUST be ignored, on return, for backward compatibility.
 *
 *****************************************************************************/
ushort send_aip_setver_cmd (hwidbtype *hwidb)
{
    mciregtype *regptr = hwidb->mci_regptr;
    ushort status;
    mcitype *cbus;
    dcitype *dci;

    cbus = (mcitype *)hwidb->devctl;
    if (!cbus)
 	return(CMD_RSP_OKAY);
    
    dci = cbus->dci[hwidb->cbus_ctrlr_index];
    if (!dci)
 	return(CMD_RSP_OKAY);
    
    regptr->argreg = dci->hw_version;
    regptr->cmdreg = FCI_CTRLR_ARGUMENT;
    regptr->argreg = FCI_CMD_AIP_SETVER;
    regptr->cmdreg = FCI_CTRLR_CMD;
    status = regptr->cmdreg;
    return(status);
}

/*****************************************************************************
 * Name: aip_platform_init
 *
 * Description: platform-specific AIP init
 *
 * Input: 
 *   subsys -- pointer to the sybsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void aip_platform_init (subsystype *subsys)
{
    reg_add_atm_get_nlpidtype(aip_get_nlpidtype, "aip_get_nlpidtype");
}
