/* $Id: crypto_util.c,v 3.2.60.7 1996/08/15 07:55:07 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_util.c,v $
 *------------------------------------------------------------------
 * crypto_util.c - Utility routines for encryption
 *
 * April, 1995 Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_util.c,v $
 * Revision 3.2.60.7  1996/08/15  07:55:07  xliu
 * CSCdi65871:  Wrong password to crypto clear-latch causes hang
 * Branch: California_branch
 * Set wathed boolean to be TRUE when the crypto card returned with a error
 * status.
 *
 * Revision 3.2.60.6  1996/07/31  03:27:32  bew
 * CSCdi64375:  Invalid  function called in NVGEN of crypto clear-latch
 * Branch: California_branch
 * crypto clear-latch should be marked PRIV_NONVGEN. Also fixed the
 * error message in crypto_card_error_code to fully display.
 *
 * Revision 3.2.60.5  1996/07/28  07:41:56  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/07/06  19:44:42  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/07  00:24:24  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.2  1996/05/12  23:12:29  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:56:05  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.12  1996/04/09  20:02:46  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.11  1996/03/25  18:43:43  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.6.10  1995/12/10  09:33:16  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.6.9  1995/11/28  01:46:42  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.8  1995/11/17  16:33:24  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.7  1995/10/03  18:51:29  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.6.6  1995/09/27  23:20:58  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.5  1995/09/14  19:32:39  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.4  1995/08/16  23:43:53  bew
 * Branch: IosSec_branch
 * Fixed a couple of small bugs.
 *
 * Revision 2.1.6.3  1995/08/02  19:24:37  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.2  1995/07/21  04:36:38  xliu
 * Branch: IosSec_branch
 * Update epa_parser.c with new epa api. Add CRYPTO_DECRYPT flag.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:26  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/06/06  21:33:57  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.2  1995/06/05  17:22:49  bew
 * Branch: IosSec_branch
 * Addition of new utility functions used by crypto subsystem.
 *
 * Revision 1.1.2.1  1995/04/27  19:23:39  bew
 * Branch: IosSec_branch
 * Adding IP level files which communicate with the EPA API, and Utility
 * files used by both the parser EPA support and the IP level files.
 *
 * Revision 1.1  1995/04/27 19:11:59  bew
 * Placeholder files for crypto IP and Utility files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"


uchar *crypto_card_error_code (ushort code) 
{
    switch (code) {
    case ESA_DSS_SIGNATURE_ERROR:
        return ("Signature check failed.");
    case ESA_INPUT_INVALID:
        return ("Invalid input command.");
    case ESA_BAD_PASSWORD:
        return ("Bad password");
    case ESA_LATCH_SET:
        return ("Extraction latch set. Request not allowed.");
    default:
        return ("Crypto card operation failed");
    }
}

/*
 * Returns a pointer to a string associated with a given  EPA status code.
 */
char *
crypto_xlate_status (epa_status code)
{
    static char codestr[60];
    switch (code) {
	case EPA_OP_OK:
	    sstrncpy(codestr, "OK", 3);
	    break;
	case EPA_OP_FAIL:
	    sstrncpy(codestr, "FAIL", 5);
	    break;
	case EPA_INVALID_INPUT:
	    sstrncpy(codestr, "INVALID_INPUT", 14);
	    break;
	default:
    	    sstrncpy(codestr, crypto_card_error_code(code), 
                     strlen(crypto_card_error_code(code))+1);
	    break;
    }
    return(codestr); 
}

/*
 * Returns a pointer to a string associated with a given algorithm type.
 */
char *
crypto_xlate_algorithm_name (ushort alg)
{
    static char str[20];
    switch (alg) {
	case DES_CFB8_IV1:
	    sstrncpy(str, "DES_56_CFB8", 12);
	    break;
	case DES_CFB64_IV1:
	    sstrncpy(str, "DES_56_CFB64", 13);
	    break;
	case DES_40_CFB8_IV1:
	    sstrncpy(str, "DES_40_CFB8", 12);
	    break;
	case DES_40_CFB64_IV1:
	    sstrncpy(str, "DES_40_CFB64", 13);
	    break;
	default:
	    sstrncpy(str, "UNKNOWN_ALG", 12);
	    break;
    }
    return(str);
}

/*
 * Returns a pointer to a string associated with a given message type.
 */
char *
crypto_xlate_message_name (ushort type)
{
    static char str[25];
    switch ((crypto_sesmgmt_message_types)type) {
	case Inititate_Connection:
	    sstrncpy(str, "Inititate_Connection", 21);
	    break;
	case CIM:
	    sstrncpy(str, "CIM", 4);
	    break;
	case CRM:
	    sstrncpy(str, "CRM", 4);
	    break;
	case CCM:
	    sstrncpy(str, "CCM", 4);
	    break;
	case Retransmitted_Message:
	    sstrncpy(str, "Retransmitted_Message", 23);
	    break;
	case Undeclared_Message:
	    sstrncpy(str, "Undeclared_Message", 19);
	    break;
	case Unsuccessful_Message:
	    sstrncpy(str, "Unsuccessful_Message", 21);
	    break;
	default:
	    sstrncpy(str, "UNKNOWN_MSG", 12);
	    break;
    }
    return(str);
}
