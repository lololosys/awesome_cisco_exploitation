/* $Id: lex_fs_les.c,v 3.3 1995/11/17 17:43:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_fs_les.c,v $
 *------------------------------------------------------------------
 * lex_fs_les.c - LEX low-end fastswitching routines
 *
 * May 4, 1994, Andy Harvey
 *
 * Copyright (c) 1994-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_fs_les.c,v $
 * Revision 3.3  1995/11/17  17:43:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:15:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:40:44  hampton
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
#include "../if/network.h"
#include "../if/arp.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "config.h"
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../wan/ppp.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "lex_fs_les.h"

/*
 * Note: Always check for IDB_LEX before IDB_ETHER because
 * both bits are in the status word.  Thus this is a lex
 * interface that thinks its an ethernet interface.
 *
 * A Lex packet is just like an ethernet packet with an extra header
 * at the front.
 */
boolean lex_fs (paktype *pak)
{
    ether_hdr *ether;
    ushort rxtype;
    idbtype *swidb;
    idbtype *serial;
    
    /*
     * Check that a lex interface is bound to this serial interface
     * if not just return.
     */
    serial = pak->if_input;
    swidb = serial->lex_swidb;
    if (swidb == NULL)
	return(FALSE);

    /*
     * Change the input interface so that it points to the lex
     * interface.
     */
    change_if_input(pak, swidb);
    
    /*
     * Just bump datagramstart and throw at ethernet routines.
     */
    REMOVE_PPP_LEX_HEADER(pak);
    /*
     * Decode the ethernet packet and set rxtype.
     */
    ether = (ether_hdr *)pak->datagramstart;
    rxtype = ether_decode_encapsulation_inline(pak, ether);
    pak->rxtype = rxtype;

    if ((reg_invoke_ether_fs(rxtype, pak) == FALSE)) {
	/*
	 * Undo what was done earlier.
	 */
	ADD_PPP_LEX_HEADER(pak);
	change_if_input(pak, serial);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lex_les_register_drivers
 * Set up fast switching drivers for LEX
 */

static void lex_les_register_drivers (ulong flags, hwidbtype *hwidb)
{
    reg_add_ppp_fs(RXTYPE_LEX, (service_ppp_fs_type)lex_fs, "lex_fs");
}

/*
 * lex_les_init
 * Initialize Lex fastswitching support
 */

static void lex_les_init (subsystype* subsys)
{
    reg_add_register_drivers(lex_les_register_drivers,
			     "lex_les_register_drivers");
}


/*
 * LEX_LES subsystem header
 */

#define LEX_LES_MAJVERSION 1
#define LEX_LES_MINVERSION 0
#define LEX_LES_EDITVERSION 0

SUBSYS_HEADER(lex_les,
	      LEX_LES_MAJVERSION, LEX_LES_MINVERSION, LEX_LES_EDITVERSION,
	      lex_les_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch, lex");
