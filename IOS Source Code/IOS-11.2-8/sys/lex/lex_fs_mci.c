/* $Id: lex_fs_mci.c,v 3.3 1995/11/17 17:43:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_fs_mci.c,v $
 *------------------------------------------------------------------
 * lex_fs_mci.c
 *
 * Mar. 1994, Andy Harvey
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_fs_mci.c,v $
 * Revision 3.3  1995/11/17  17:43:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:40:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "access.h"
#include "../if/arp.h"
#include "../if/ether.h"
#include "config.h"
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../wan/ppp.h"
#include "../if/network.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "lex_fs_mci.h"

/*
 * Note: Always check for IDB_LEX before IDB_ETHER because
 * both bits are in the status word.  Thus this is a lex
 * interface that thinks its an ethernet interface.
 *
 * A Lex packet is just like an ethernet packet with an extra header
 * at the front.
 */
boolean lex_fs (hwidbtype *serial)
{
    mciregtype *regp;
    ushort rxtype;
    ushort typelen;
    idbtype *lex_swidb;
    hwidbtype *lex_hwidb;

    /*
     * This case should no happen.  The only legal reason for being
     * at this point in the code is that a packet has arrived with
     * the lex PPP code (0041).  The packet is punted to process
     * level where it is freed by the enqueue routine.
     */
    lex_swidb = serial->fast_rcvidb->lex_swidb;
    if (lex_swidb == NULL)
	return(FALSE);

    /*
     * The lex idb is considered to be the input idb, even though the
     * "real" input idb is the serial idb.
     *
     * Because all of the attributes of interest are stored in the
     * Lex IDB pairs care must be taken when looking for information.
     */
    lex_hwidb = lex_swidb->hwptr;

    /*
     * Skip over the dst and src mac addresses to grab the type/len.
     * Note the offset is in 16 bit / word chunks.
     */
    regp = lex_hwidb->mci_regptr;
    regp->argreg = LEX_ARPA_OFFSET - 1;
    regp->cmdreg = MCI_CMD_RX_SELECT;
    typelen = regp->readshort;

    /*
     * Classify the packet to see what we really have.
     * Note: The check on packet length works for all 802.3
     * frames except Xerox PUP and a few experimental types.
     * These protocols are not considered important and thus
     * will be clasified as unknown.  The lex protocol calls
     * for an media specific frame to be encapsulated within
     * a lex frame.  The first version is ethernet only and
     * thus the check below is sufficient.
     */
    if (typelen > MAXETHERSIZE) {
	if (typelen == TYPE_IP10MB) {
	    rxtype = RXTYPE_DODIP;
	} else if (typelen == TYPE_NOVELL1) {
	    rxtype = RXTYPE_NOVELL1;
	} else if (typelen == TYPE_ETHERTALK) {
	    rxtype = RXTYPE_APPLE;
	} else if (typelen == TYPE_VINES) {
	    rxtype = RXTYPE_VINES;
	} else if (typelen == TYPE_DECNET) {
	    rxtype = RXTYPE_DECNET;
	} else if (typelen == TYPE_XNS) {
	    rxtype = RXTYPE_XNS;
	} else {
	    rxtype = RXTYPE_UNKNOWN;
	}
    } else {
	snap_hdr snap;
	ulong *off = (ulong *)&snap;

	/*
	 * Read and check for SAP.
	 */
	*off++ = regp->readlong;

	if ((snap.dsap == SAP_SNAP) && (snap.ssap == SAP_SNAP)) {
	    *off++ = regp->readlong;
	    switch (snap.type) {
	    case TYPE_VINES:
	    case TYPE_VINES2:
		rxtype = RXTYPE_SNAP_VINES;
		break;
	    case TYPE_NOVELL1:
		rxtype = RXTYPE_SNAP_NOV1;
		break;
	    case TYPE_ETHERTALK:
		rxtype = RXTYPE_SNAP_APPLE;
		break;
	    default: 
		rxtype = RXTYPE_SNAP_UNK;
		break;
	    }
	} else {
	    ushort lsap = (snap.dsap | (snap.ssap << 8));
	    
	    switch (lsap) {
	    case (SAP_NOVELL_ETHER | SAP_NOVELL_ETHER << 8):
		rxtype = RXTYPE_NOVELL2;
		break;
	    case (SAP_NOVELL | SAP_NOVELL << 8):
		rxtype = RXTYPE_SAP_NOVELL;
		break;
	    default:
		rxtype = RXTYPE_UNKNOWN;
		break;
	    }
	}
    }

    /*
     * Initalize the input idb to reflect the serial idb.
     */
    lex_hwidb->rxlength = serial->rxlength;
    lex_hwidb->rxtype = rxtype;
    lex_hwidb->rxoffset = serial->rxoffset;
    GET_TIMESTAMP(lex_hwidb->lastinput);    /* remember time of last input */

    /*
     * Invoke the lex fastswitch routine with the "correct"
     * rxtype.
     */
    return(reg_invoke_lex_fs(rxtype, lex_hwidb));
}

/*
 * lex_mci_register_drivers
 * Set up fast switching drivers for LEX
 */
static void lex_mci_register_drivers (ulong flags, hwidbtype *hwidb)
{
    /*
     * Register the lex master fastswitch routine with the PPP
     * fastswitch registry.  This is registered from the lex
     * sub-system.  If the lex sub-system is not included in the
     * image no lex PPP packets will be fastswitched.
     */
    reg_add_ppp_fs(RXTYPE_LEX, (service_ppp_fs_type)lex_fs, "lex_fs");
}

/*
 * lex_mci_init
 * Initialize Lex fastswitching support
 */
static void lex_mci_init (subsystype* subsys)
{
    reg_add_register_drivers(lex_mci_register_drivers,
			     "lex_mci_register_drivers");
}

/*
 * LEX_MCI subsystem header
 */
#define LEX_MCI_MAJVERSION 1
#define LEX_MCI_MINVERSION 0
#define LEX_MCI_EDITVERSION 0

SUBSYS_HEADER(lex_mci,
	      LEX_MCI_MAJVERSION, LEX_MCI_MINVERSION, LEX_MCI_EDITVERSION,
	      lex_mci_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch, lex");
