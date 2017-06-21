/* $Id: rsp_vip.c,v 3.4.54.6 1996/08/19 19:56:03 ptran Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_vip.c,v $
 *------------------------------------------------------------------
 * Versatile Interface Processor support code
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_vip.c,v $
 * Revision 3.4.54.6  1996/08/19  19:56:03  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.4.54.5  1996/08/13  02:24:30  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.4.54.4  1996/07/26  01:11:13  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.4.54.3  1996/06/07  00:34:25  kao
 * CSCdi59481:  4R does not work when hot swapped
 * CSCdi59000:  FDDI PA line status is always down after an eoir event
 * Branch: California_branch
 *
 * Revision 3.4.54.2  1996/05/22  13:51:36  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.4.54.1  1996/03/21  23:50:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.2.2  1995/12/23  04:41:56  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3.2.1  1995/12/11  19:24:35  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.4  1995/12/02  06:56:17  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 *
 * Revision 3.3  1995/11/17  18:49:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  14:46:55  getchell
 * Placeholder files for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_if.h"
#include "../hes/if_hes_common.h"
#include "rsp_common_if.h"
#include "../if/ether.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_fci.h"
#include "lovenote.h"
#include "../hes/if_hes_ether.h"
#include "../hes/if_hes_serial.h"
#include "../if/tring.h"
#include "../hes/if_trip.h"
#include "../hes/if_feip.h"
#include "../hes/if_fip.h"
#include "../if/fddi.h"
#include "../hes/if_vip.h"
#include "../hes/if_vip_4r.h"
#include "../ui/debug.h"
#include "../if/rvip_debug.h"
#include "rsp_vip.h"

#include "../if/fddi_debug.h"



/*****************************************************************************
 * Name: vip_love_letter
 *
 * Description: platform specific love letter routine
 *
 * Input: 
 *   
 *
 * Return Value: None
 *
 *****************************************************************************/
void vip_rsp_love_letter (hwidbtype *hwidb, lovenotetype *lovenote)
{
    ushort    asr = lovenote->love_code;


    if (rvip_debug) {
	buginf("\nvip_love_note(%s): asr=0x%x length=%d",
	       hwidb->hw_namestring, asr, lovenote->data_length);
    }

    if (lovenote->data_length) {
	vip_parse_love_letter(hwidb,
			      MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr),
			      lovenote->data_length);
	return;

    }

    if (asr & LOVE_CODE_CARRIER_CHANGE) {
	cbus_mci_carrier(hwidb);
    }

}


/*****************************************************************************
 * Name: vip2standard_types
 *
 * Description: map VIP FCItypes to "normal" types
 *
 * Input: pa_type: VIP fcitype
 *   
 *
 * Return Value: normalized fcitype and idbtype
 *
 *****************************************************************************/
int vip2standard_types (int pa_type, enum IDBTYPE *type)
{

    switch (pa_type) {
      case FCI_TYPE_VIP_FETHER:
      case FCI_TYPE_VIP_FETHER_TX_NISL:
      case FCI_TYPE_VIP_FETHER_TX_ISL:
      case FCI_TYPE_VIP_FETHER_FX_ISL:
	*type =  IDBTYPE_FEIP;
	break;

      case FCI_TYPE_VIP_ETHER8:
	*type = IDBTYPE_CBUSETHER;
	break;

      case FCI_TYPE_VIP_SERIAL4:
	*type = IDBTYPE_CBUSSERIAL;
	break;

      case FCI_TYPE_VIP_TRING4:
	*type = IDBTYPE_CTR;	
	break;

      case FCI_TYPE_VIP_FDDI:
	*type = IDBTYPE_FDDIT;	
	break;

      case FCI_TYPE_VIP_ESA:
        *type= IDBTYPE_CYLEPA;
        break;

    default:
	*type = IDBTYPE_UNKNOWN;
	return FCI_TYPE_UNKNOWN;
    }

    return pa_type;
}

/*****************************************************************************
 * Name: vip_ctrlr_init
 *
 * Description: per-card initialization for the VIP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the VIP now. 
 *   It may also be necessary to pass global information to the VIP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *       Ensure the max love letter length is within allowable limits
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the VIP
 *       cmd_q_ptr -- pointer to the command queue for the VIP
 *       event_q_ptr -- pointer to the event queue for this card to use
 *       love_note_free_q_ptr -- pointer to the queue of free love note buffer
 *         headers 
 *       love_letter_free_q_ptr -- pointer to the queue of free love letter
 *         buffers
 *
 * Output:
 *   Commands sent to the card
 *
 * Return Value: None
 *
 *****************************************************************************/
void vip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
    return;
}

/*****************************************************************************
 * Name: vip_if_init
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.  All fields
 *	should be valid when this function is called (as many as possible),
 *	but specifically
 *       raw_q_ptr
 *       rx_limit_lo
 *       rx_limit_hi
 *       global_free_q_size
 *       global_free_q_ptr
 *       local_free_q_ptr
 *       tx_q_ptr
 *       tx_acc_ptr
 *       rx_offset
 *
 * Output:
 *   Commands sent to the card
 *****************************************************************************/
void vip_if_init (hwidbtype *hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);

    rsp_common_if_init(hwidb);

    switch(hwidb->type) {
      case IDBTYPE_FEIP:
      case IDBTYPE_CBUSETHER:
	hwidb->reset = cbus_pas_ethernet_reset;
	hwidb->periodic = cbus_mci_ether_periodic;
	break;

      case IDBTYPE_CBUSSERIAL:
	send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
	hwidb->reset = (reset_t) cbus_mci_serial_reset;
	break;

      case IDBTYPE_CTR:
	send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
	hwidb->reset = (reset_t)cbus_vip_4r_reset;
	hwidb->periodic = cbus_vip_4r_periodic;
	break;

      case IDBTYPE_FDDIT:
	send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
	hwidb->reset = (reset_t) cbus_vip_1f_reset;
	hwidb->periodic = cbus_vip_1f_periodic;
	break;

      default:
	break;
    }

    reset_interrupt_level(level);
    return;
}
