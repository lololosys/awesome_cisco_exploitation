/* $Id: crypto_sesmgmt.h,v 3.2.60.8 1996/09/11 23:25:08 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_sesmgmt.h,v $
 *------------------------------------------------------------------
 * crypto_sesmgmt.h - Crypto Session Management definitions
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_sesmgmt.h,v $
 * Revision 3.2.60.8  1996/09/11  23:25:08  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.2.60.7  1996/08/28  19:29:14  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.2.60.6  1996/08/16  05:56:09  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 3.2.60.5  1996/07/06  19:44:41  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/07/01  09:06:03  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/07  00:24:23  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.2  1996/05/12  23:12:27  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:56:02  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.13  1996/04/16  21:52:47  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.12  1995/11/30  05:57:25  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.11  1995/11/28  01:46:41  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.10  1995/11/17  16:33:23  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.9  1995/10/16  04:04:22  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.7  1995/10/10  18:19:43  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.6  1995/09/27  23:20:56  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.5  1995/09/14  19:32:38  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.4  1995/09/12  19:46:15  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.6.3  1995/08/30  20:41:42  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.2  1995/08/02  19:24:36  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.1  1995/07/01  05:31:25  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.2  1995/06/06  21:33:56  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:54  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:26  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This must be kept in sync with the Cylink DEF_SIG_SIZE definition.
 */
#define CRYPTO_DEF_SIG_SIZE            40

typedef enum crypto_sesmgmt_message_types_ {
    Inititate_Connection,	/* Initial Request for CS to begin */
    CIM,			/* CIM */
    CRM,			/* CRM */
    CCM,			/* CCM */
    Retransmitted_Message,	/* Resend a message */
    Undeclared_Message,		/* Send a message without declaring the type */
    Unsuccessful_Message,	/* An error occured on sending */
} crypto_sesmgmt_message_types;


typedef struct crypto_sesmgmt_block_
{
    struct crypto_sesmgmt_block_ *next;
    crypto_sesmgmt_message_types message_type;
    /*
     * These are the addresses of the PE and UPE stored in the Cylink
     * connection tables.
     */
    addrtype pe;
    addrtype upe;
    /*
     * These are the actual src & dst for a connection message.
     */
     addrtype src;
     addrtype dst;
     ushort slot;
    /*
     * The following is policy which is passed onto the glue layer to stick
     * in a cib.
     */
    int method;
    char **peer;
    ulong pending_conn_id;
    uint  mode_flags;
    /*
     * The following field is used by types CIM, CRM & CCM.
     */
    paktype *pak;
    /*
     * The following fields are used by types "Retransmitted_Message" and 
     * "Undeclared_Message".
     */
    char *message;
    ushort message_length;
} crypto_sesmgmt_block;

typedef struct {
    ipaddrtype    pe;
    ipaddrtype    upe;
    ushort        slot;
    char **       peer;
    ulong         correlator;
} crypto_session_entry;

typedef struct {
    crypto_session_entry  tbl[100];
    int                  index;
} crypto_session_t;

/*
 * Function prototypes
 */
void crypto_sesmgmt_enqueue(crypto_sesmgmt_block *);
crypto_sesmgmt_message_types crypto_get_cs_msg_type(paktype *);
