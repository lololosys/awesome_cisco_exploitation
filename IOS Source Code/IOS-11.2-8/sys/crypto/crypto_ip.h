/* $Id: crypto_ip.h,v 3.2.60.5 1996/08/28 19:29:10 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_ip.h,v $
 *------------------------------------------------------------------
 * crypto_ip.h - Definitions for IP-level crypto routines.
 *
 * April, 1995 Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_ip.h,v $
 * Revision 3.2.60.5  1996/08/28  19:29:10  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.2.60.4  1996/08/07  21:58:45  bew
 * CSCdi65215:  crypto connection msgs not handled properly if router is
 *              destination
 * Branch: California_branch
 * If the connection message is a CIM, check each crypto map statement
 * until one matches. If a CRM or CCM which is for us, queue it up for
 * session mgr without checking the policy.
 *
 * Revision 3.2.60.3  1996/07/06  19:44:32  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/07  00:24:11  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.1  1996/04/19  14:55:51  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.23  1996/04/17  23:01:12  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 2.1.6.22  1996/04/16  21:52:25  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.21  1996/04/12  23:07:31  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.6.20  1996/03/14  00:57:53  bew
 * Branch: IosSec_branch
 * Changed the crypto key-exchange protocol to be more intuitive.
 *
 * Revision 2.1.6.19  1996/03/01  01:39:04  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.18  1996/01/13  05:46:43  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.6.17  1995/12/20  23:30:47  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.6.16  1995/11/30  05:57:23  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.15  1995/11/28  01:46:35  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.14  1995/11/17  16:33:19  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.13  1995/10/17  01:19:35  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.6.12  1995/10/16  04:04:19  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.10  1995/10/10  18:19:38  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.9  1995/09/27  23:20:53  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.8  1995/09/26  04:12:06  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.6.7  1995/09/22  01:21:59  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.6.6  1995/09/12  19:46:13  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.6.5  1995/09/08  08:03:07  xliu
 * Branch: IosSec_branch
 * Eliminate CRYPTO_FAST_ENCRYPT/DECRYPT cryptoflags. Replace it with the
 * switch_mode input parameter to epa encrypt/decrypt api.
 * Replace CRYPTO_DONE cryptoflag with CRYPTO_ENCRYPT_DONE and
 * CRYPTO_DECRYPT_DONE flag to fix the bug caused by reused pak in ping
 * application.
 *
 * Revision 2.1.6.4  1995/08/30  20:41:39  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.3  1995/08/11  00:59:18  xliu
 * Branch: IosSec_branch
 *
 * Modified epa_parser.c with the updated epa interface.
 *
 * Revision 2.1.6.2  1995/08/02  19:24:33  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.1  1995/07/01  05:31:22  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.4  1995/06/21  20:37:24  xliu
 * Branch: IosSec_branch
 *
 * Incorporate the changes made by epa api.
 *
 * Revision 1.1.2.3  1995/06/06  21:33:55  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.2  1995/06/01  22:56:14  bew
 * Branch: IosSec_branch
 * Addition of new crypto_ip_* functions which contain IP-specific
 * functions (duh) some of which have been moved from crypto.c. Also, some
 * macros have been renamed to better indicate they are IP specific. Many
 * of the functions have been developed with an eye to placing them under
 * a registry in the future.
 *
 * Revision 1.1.2.1  1995/04/27  19:23:40  bew
 * Branch: IosSec_branch
 * Adding IP level files which communicate with the EPA API, and Utility
 * files used by both the parser EPA support and the IP level files.
 *
 * Revision 1.1  1995/04/27 19:12:01  bew
 * Placeholder files for crypto IP and Utility files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_IP_H_
#define CRYPTO_IP_H_
 
/*
 * Define ICMP special code identifier for connection messages.
 */
#define CRYPTO_ICMP_CODE 74
#define START_OF_ICMP_DATA 1

/*
 * Decryption for prototype.  
 * Only decrypt if we are the destination and they are the source, we're
 * going out the decrypting interface, and we're not an ICMP message.
 */

static inline boolean crypto_ip_packet_needs_decryption (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if ((pak->cryptoflags & CRYPTO_DECRYPT) &&
	!(pak->cryptoflags & CRYPTO_DECRYPT_DONE))
	return(TRUE);
    else
	return(FALSE);
}

static inline boolean crypto_ip_packet_was_decrypted (paktype *pak)
{
    if (!(pak->cryptoflags & CRYPTO_DECRYPT))
	return(TRUE);
    else
	return(FALSE);
}

static inline boolean crypto_ip_packet_was_encrypted (paktype *pak)
{
    if (!(pak->cryptoflags & CRYPTO_ENCRYPT))
	return(TRUE);
    else
	return(FALSE);
}

static inline boolean crypto_ip_found_connection_message (iphdrtype *ip)
{
    icmptype *icmp;

    if (ip->prot == ICMP_PROT) {
    	icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    	if (((icmp->type == ICMP_ECHOREPLY) || (icmp->type == ICMP_ECHO)) &&
	      icmp->code == CRYPTO_ICMP_CODE) {
	    return(TRUE);
    	}
    }
    return(FALSE);
}

/*
 * extern functions
 */
extern void crypto_ip_encrypt(paktype *);

extern void crypto_ip_decrypt(paktype *);

extern boolean crypto_ip_send_echo(ipaddrtype , echotype *, int , ipaddrtype, 
			     uchar, char *, int);

extern void crypto_ip_queue_conn_message(paktype *, crypto_maptype *, int);

extern void crypto_ip_find_sig_params(uchar *, crypto_sig_params *);

extern void crypto_ip_find_network_addrs(paktype *, addrtype *, addrtype *);

extern void crypto_ip_recompute_icmp_checksum(paktype *);

extern void crypto_ip_verify_icmp_checksum(paktype *, boolean *);

extern void crypto_ip_queue_pak(paktype *, uchar *);

extern void crypto_ip_find_cm(paktype *, uchar **);

extern void crypto_ip_find_echo(paktype *, uchar **);

extern boolean crypto_ip_insert_network_address(uchar *, paktype *);

extern crypto_sesmgmt_message_types crypto_ip_send_echo_based_msg(
						      crypto_sesmgmt_block *);

/*
 * externs for crypto_exchange.c
 */
extern void crypto_exchange_ip (addrtype *, char *, ushort);

extern void crypto_exchange_passive_ip (ushort);

extern boolean crypto_exchange_sessionkey_ip(addrtype *, uchar *, uchar *);

extern boolean crypto_exchange_sessionkey_passive_ip(uchar *, uchar *);

crypto_sesmgmt_message_types crypto_ip_find_connection_msg_type (paktype *);

#endif
