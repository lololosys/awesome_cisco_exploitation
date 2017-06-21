/* $Id: crypto_init.c,v 3.2.60.4 1996/07/06 05:52:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_init.c,v $
 *------------------------------------------------------------------
 * crypto_init.c - Crypto subsystem initialization 
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_init.c,v $
 * Revision 3.2.60.4  1996/07/06  05:52:33  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.60.3  1996/05/30  20:24:02  bew
 * CSCdi58950:  Need to make exportable crypto images
 * Branch: California_branch
 * Added the exportable crypto subsystem. The guts of crypto_init() are
 * moved into crypto.c, and crypto_exportable_init.c is added. The
 * strong_encryption_required boolean added and set appropriately
 * by both the crypto and exportable crypto subsystems.
 *
 * Revision 3.2.60.2  1996/05/12  23:12:17  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:55:49  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.23  1996/04/16  21:52:18  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.22  1996/03/25  18:43:40  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.6.21  1996/03/21  19:56:40  bew
 * Branch: IosSec_branch
 * Move crypto export message to be printed from crypto_init().
 *
 * Revision 2.1.6.20  1996/03/13  22:45:08  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.6.19  1996/02/13  19:15:50  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.18  1996/01/12  17:02:24  rlfs
 * Branch: IosSec_branch
 * Fix dependancy problem with crypto_init.
 *
 * Revision 2.1.6.17  1996/01/09  19:26:23  xliu
 * Branch: IosSec_branch
 * Add crypto_registry.regc to include.
 *
 * Revision 2.1.6.16  1996/01/09  00:53:40  xliu
 * Branch: IosSec_branch
 *
 * Change the crypto registry to be well known registry.
 *
 * Revision 2.1.6.15  1996/01/04  18:47:49  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.6.14  1995/12/10  09:33:14  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.6.13  1995/12/02  17:44:30  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 2.1.6.12  1995/11/30  05:57:21  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.11  1995/11/28  01:46:33  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.10  1995/11/17  16:33:18  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.9  1995/10/09  18:40:20  xliu
 * Branch: IosSec_branch
 *
 * Fixed the naming problem. Commit the deletion of epa dir.
 *
 * Revision 2.1.6.8  1995/10/06  17:29:39  xliu
 * Branch: IosSec_branch
 * Fixed naming bug.
 *
 * Revision 2.1.6.7  1995/10/03  18:51:23  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.6.6  1995/09/14  19:32:35  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.5  1995/09/06  00:55:46  bew
 * Branch: IosSec_branch
 * epa_init() is now called whenever new DSS keys are created or read in
 * from the configuration. Also added a crypto config command to initialize
 * the EPA to replace the exec epa command. The epa command was removed
 * altogether.
 *
 * Revision 2.1.6.4  1995/09/02  05:05:25  xliu
 * Branch: IosSec_branch
 *
 * Add function to retrive dss keys when reinitializaing.
 *
 * Revision 2.1.6.3  1995/09/01  21:37:32  bew
 * Branch: IosSec_branch
 * Implementation of 11.0 scheduling, implementation of
 * pending connections timeout handling, addition of
 * routine to safely pick crypto connection ids, and
 * other cleanup.
 *
 * Revision 2.1.6.2  1995/08/30  20:41:38  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:21  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/06/21  20:37:24  xliu
 * Branch: IosSec_branch
 *
 * Incorporate the changes made by epa api.
 *
 * Revision 1.1.2.2  1995/06/06  21:33:53  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:56  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:23  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "crypto.h"

/*
 * Strong encryption is included in this subsystem. 
 */
boolean strong_crypto_allowed = TRUE;

/*
 * WARNING! Do not change the following text. It was defined by the
 *          Cisco logistics (export) folks, namely Laura Ipsen.
 *	    This is the DES 56-bit export message.
 */
static void crypto_print_export_message (void)
{
    printf("\nCompliance with U.S. Export Laws and Regulations - Encryption");
    printf("\n\nThis product performs encryption and is regulated for export");
    printf("\nby the U.S. Government.");
    printf("\n\nThis product is not authorized for use by persons located");
    printf("\noutside the United States and Canada that do not have prior");
    printf("\napproval from Cisco Systems, Inc. or the U.S. Government.");
    printf("\n\nThis product may not be exported outside the U.S. and Canada");
    printf("\neither by physical or electronic means without PRIOR approval");
    printf("\nof Cisco Systems, Inc. or the U.S. Government.\n");
    printf("\nPersons outside the U.S. and Canada may not re-export, resell,");
    printf("\nor transfer this product by either physical or electronic means");
    printf("\nwithout  prior approval of Cisco Systems, Inc. or the U.S.");
    printf("\nGovernment.\n\n");
}

/*
 * Initialize the crypto subsystem
 */
void crypto_init (subsystype *subsys)
{
    /*
     * Print nasty export message
     */
    crypto_print_export_message();

    /*
     * Call the common initialalization
     */
    crypto_init_common(); 
}

/*
 * CRYPTO subsystem header
 */
#define CRYPTO_MAJVERSION  1
#define CRYPTO_MINVERSION  0
#define CRYPTO_EDITVERSION 1

SUBSYS_HEADER(crypto, CRYPTO_MAJVERSION, CRYPTO_MINVERSION, CRYPTO_EDITVERSION,
	      crypto_init,
	      SUBSYS_CLASS_LIBRARY,
	      "seq: crypto_engine_rsp",
	      subsys_req_iphost);
