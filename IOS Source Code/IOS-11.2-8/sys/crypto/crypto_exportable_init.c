/* $Id: crypto_exportable_init.c,v 3.1.2.2 1996/07/06 05:52:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_exportable_init.c,v $
 *------------------------------------------------------------------
 * crypto_exportable_init.c - Exportable crypto subsystem initialization
 *
 * May 1996, Brian Weis
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_exportable_init.c,v $
 * Revision 3.1.2.2  1996/07/06  05:52:31  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1.2.1  1996/05/30  20:24:00  bew
 * CSCdi58950:  Need to make exportable crypto images
 * Branch: California_branch
 * Added the exportable crypto subsystem. The guts of crypto_init() are
 * moved into crypto.c, and crypto_exportable_init.c is added. The
 * strong_encryption_required boolean added and set appropriately
 * by both the crypto and exportable crypto subsystems.
 *
 * Revision 3.1  1996/05/29  21:06:21  bew
 * Placeholder for exportable crypto subsystem init stuff.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "crypto.h"
#include "sys_registry.h"

/*
 * Strong encryption is included in this subsystem. 
 */
boolean strong_crypto_allowed = FALSE;

/*
 * WARNING! Do not change the following text. It was defined by the
 *          Cisco logistics (export) folks, namely Laura Ipsen.
 *	    This is the DES 40-bit export message.
 */
static void crypto_print_export_message (void)
{
    printf("\nCompliance with U.S. Export Laws and Regulations - 40-bit DES");
    printf("\n\nCisco's 40-bit DES images are controlled under the jurisdiction"
	   );
    printf("\nof the U.S. Department of Commerce and are subject to Export");
    printf("\nAdministration Regulations of the United States (EAR) and other");
    printf("\nU.S. and foreign laws and regulations. By downloading this");
    printf("\nsoftware you are agreeing to abide by all U.S. export and");
    printf("\nre-export restrictions and any import restrictions that may");
    printf("\nbe applicable in the foreign country in which you reside.\n\n");
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

SUBSYS_HEADER(exportable_crypto, CRYPTO_MAJVERSION, CRYPTO_MINVERSION, 
	      CRYPTO_EDITVERSION,
	      crypto_init,
	      SUBSYS_CLASS_LIBRARY,
	      "seq: crypto_engine_rsp",
	      subsys_req_iphost);
