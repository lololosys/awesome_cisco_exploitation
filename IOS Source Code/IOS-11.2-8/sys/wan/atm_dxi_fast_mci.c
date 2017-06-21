/* $Id: atm_dxi_fast_mci.c,v 3.3 1996/01/19 22:16:54 wmay Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/wan/atm_dxi_fast_mci.c,v $
 *------------------------------------------------------------------
 * atm_dxi_fast_mci.c -- MCI and cBus Fast Switching Support
 *
 * 23-Sept-1994, Bill May (from fr_fast_mci.c)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi_fast_mci.c,v $
 * Revision 3.3  1996/01/19  22:16:54  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add include file for serial_inline; add sniff buffer for dest address
 *
 * Revision 3.2  1995/11/17  18:00:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "fastswitch_registry.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "frame_relay.h"
#include "atm_dxi.h"
#include "serial.h"
#include "ppp.h"
#include "smds.h"
#include "atm_dxi.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "../if/network.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../if/serial_inline.h"

/*
 * Initial fast switch routine that sets up the proper subinterface
 * pointer, and looks for PVC switching.  This routine is called for all
 * rxtypes prior to calling a network layer protocol specific routine.
 */

static boolean atm_dxi_hes_classify_pak_and_fs (hwidbtype *input)
{
    mciregtype *srcptr;
    ushort dfa, save_dfa, in_type, type;
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    ulong temp;

    srcptr = input->mci_regptr;
    dxi_idb = input->atm_dxi_stuff;
    input->rxtype = RXTYPE_UNKNOWN;
    /*
     * Be sure we have a valid struct before starting
     */
    if (!dxi_idb)
	return(FALSE);

    /*
     * Lookup the DLCI to check the action to take
     */
    temp = input->rxlength;
    srcptr->argreg = MCI_SERIAL_OFFSET;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    dfa = srcptr->readshort;
    save_dfa = DFA_TO_NUM(dfa);
    if (save_dfa > MAX_DFA)
	return(FALSE);
    dxi_pvc = dxi_idb->dxi_dfa_tab[save_dfa];
    if (!dxi_pvc)
	return(FALSE);

    dxi_idb->fast_input_dfa = save_dfa;

    /* Get any extra board encap bytes here */
    dxi_idb->fast_input_encsize = input->encsize - ATM_DXI_ENCAPBYTES;
    switch (dxi_pvc->encaps_type) {
    case DXI_MAP_SNAP:
	{
	    in_type = srcptr->readshort;
	    if (in_type == DXI_ISO_LLC) {
		/* Have an iso pdu */
		type = RXTYPE_CLNS;
		dxi_idb->fast_input_encsize += ATM_DXI_ISOPDU_HDRBYTES;
	    } else {
		temp = srcptr->readlong;	/* read through oui */
		in_type = srcptr->readshort;
		switch (in_type) {
		    SERIAL_SNAP_TYPE_CASES;
		default:
		    type = RXTYPE_SNAP_UNK;
		    break;
		}
		input->rxtype = type;
		dxi_idb->fast_input_encsize += ATM_DXI_SNAPENCAPBYTES;
	    }
	}
	break;
    case DXI_MAP_NLPID:
	{
	    in_type = srcptr->readshort;
	    switch (in_type) {
	    case ATM_DXI_IETF_IP:
		dxi_idb->fast_input_encsize += ATM_DXI_ENCAPBYTES;
		type = RXTYPE_DODIP;
		break;
	    case ATM_DXI_CLNP:
	    case ATM_DXI_ESIS:
	    case ATM_DXI_ISIS:
	    case ATM_DXI_SNAP0PAD:
	    default:
		return(FALSE);
	    case ATM_DXI_SNAP1PAD:
		{
		    temp = srcptr->readlong;	/* read past oui */
		    in_type = srcptr->readshort;
		    switch (in_type) {
			SERIAL_SNAP_TYPE_CASES;
		    default:
			type = RXTYPE_SNAP_UNK;
			break;
		    }
		    dxi_idb->fast_input_encsize += ATM_DXI_SNAPENCAPBYTES;
		}
		break;
	    }
	}
	break;
    case DXI_MAP_MUX:
	dxi_idb->fast_input_encsize += ATM_DXI_MUXENCAPBYTES;
	type = dxi_pvc->rxtype;
	/* Get the type from the atm_dxi info */
	break;
    default:
	type = RXTYPE_UNKNOWN;
	break;
    }

    if (type == RXTYPE_DODIP || type == RXTYPE_SNAP_DODIP) {
	in_type = input->rxoffset + MCI_IP_DSTADR_OFFSET +
	    btow(dxi_idb->fast_input_encsize);
	srcptr->argreg = in_type;
	srcptr->cmdreg = MCI_CMD_RX_SELECT;
    }
    if (reg_invoke_atm_dxi_fs(type,input)) {
	INC_DXI_PVC_FAST_INPUT(dxi_pvc, input->rxlength);
	return(TRUE);
    }
    return(FALSE);
}


/*
 * fr_mci_register_drivers
 *
 * This routine initializes the fast switching table for PVC switching.
 * The fast switching table for routed protocols is initialized in
 * fastswitch_init.c.
 */
static void atm_dxi_mci_register_drivers (ulong flags, hwidbtype *hwidb)
{
    if (hwidb->enctype != ET_ATM_DXI)
	return;
    reg_add_atm_dxi_classify_pak_and_fs(
       (service_atm_dxi_classify_pak_and_fs_type)
	atm_dxi_hes_classify_pak_and_fs,
	"atm_dxi_hes_classify_pak_and_fs");
}

/*
 * fr_mci_init
 * Initialize Frame Relay fastswitching support
 */

static void atm_dxi_mci_init (subsystype* subsys)
{
    reg_add_register_drivers(atm_dxi_mci_register_drivers,
			     "atm_dxi_mci_register_drivers");
}

/*
 * MCI FR subsystem header
 */

#define ATM_DXI_MCI_MAJVERSION 1
#define ATM_DXI_MCI_MINVERSION 0
#define ATM_DXI_MCI_EDITVERSION  1

SUBSYS_HEADER(atm_dxi_mci,
	      ATM_DXI_MCI_MAJVERSION, ATM_DXI_MCI_MINVERSION,
	      ATM_DXI_MCI_EDITVERSION,
	      atm_dxi_mci_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch, atm_dxi, iphost");
