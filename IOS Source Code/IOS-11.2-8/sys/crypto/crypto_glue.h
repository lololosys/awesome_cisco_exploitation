/* $Id: crypto_glue.h,v 3.2.62.4 1996/08/16 05:56:04 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue.h,v $
 *------------------------------------------------------------------
 * crypto_glue.h - Prototypes & symbols for use with the Cylink glue layer.
 *
 * September, 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue.h,v $
 * Revision 3.2.62.4  1996/08/16  05:56:04  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 3.2.62.3  1996/06/07  00:24:05  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.62.2  1996/06/05  23:11:20  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.2.62.1  1996/04/19  14:55:43  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.12  1996/04/16  21:52:07  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.11  1996/04/12  16:06:09  snijsure
 * Branch: IosSec_branch
 * Modified crypto_glue_clear_conn_id() to invoke
 * crypto_snmp_conn_update() which will send a trap
 * on completion of crypto ping, iff its initiated via
 * SNMP.
 *
 * Revision 2.1.2.10  1996/03/13  22:45:04  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.9  1996/03/01  01:38:57  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.2.8  1996/01/13  05:46:40  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.7  1995/12/12  23:09:26  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.2.6  1995/11/28  01:46:28  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.5  1995/10/16  04:04:16  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.3  1995/10/10  18:19:33  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.2  1995/09/29  01:00:31  bew
 * Added session key timeouts. Also renamed epa_parser.c to
 * crypto_parser.c & changed the timestamp used in SDU code
 * to be the seconds from Day 0 rather than the contents of
 * msclock.
 *
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.1  1995/09/27  23:20:46  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1  1995/09/27  07:38:03  bew
 * Placeholder crypto files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file contains interfaces to the crypto glue layer to get/set Cylink 
 * data and call Cylink interfaces. 
 */

/*
 * This must be kept in sync with the Cylink message definitions. No such
 * Cylink definition exists.
 */
#define CRYPTO_MAX_CONNECTION_MESSAGE_SIZE 300

/*
 * Constants to keep in sync with Cylink constants
 * NOTE: CRYPTO_DEF_SIG_SIZE is defined in crypto_sesmgmt.h, not here due
 *       to dependancies between the header files.
 */
#define CRYPTO_DEF_MAXKEYAGE 30 	/* value of DEF_MAXKEYAGE */
#define CRYPTO_UNDEFINED_METHOD	0	/* value of UNDEFINED */

/*
 * CRYPTO_PCONN_WATCH_TIME is the time between wakeups
 * to call the SDU code checking the pending connection list.
 *
 * Note that the SDU code checks every 800msec (this is hardcoded in the
 * unused SDU connstup() routine). This seems a little too short for us, 
 * especially as the timeout (MAX_PCONN_TIME) * is 30 seconds, but we've 
 * implemented it here anyway for compatibility.
 */
#define CRYPTO_PCONN_WATCH_TIME  800
#define CRYPTO_EPC_WATCH_TIME 1000 /* value epcPeriod is initialized */

/*
 * Function prototypes
 */

extern int crypto_glue_initiate_connection(crypto_sesmgmt_block *, int,
					   char **, ushort, boolean *);
extern void crypto_glue_send_nnc_req(crypto_sesmgmt_block *);
extern void crypto_glue_recv_nnc_req(crypto_sesmgmt_block *);
extern void crypto_glue_recv_nnc_rpy(crypto_sesmgmt_block *);
extern void crypto_glue_allocate_cot(crypto_sesmgmt_block *);
extern void crypto_glue_recv_nnc_cfm(crypto_sesmgmt_block *);
extern void *crypto_glue_get_cot(crypto_sesmgmt_block *, boolean);
extern void crypto_glue_cs_init(void);
extern void crypto_glue_ep_init(void);
extern boolean crypto_glue_pconn_mgmt (void);
extern void crypto_glue_epc_task(void);
extern void crypto_glue_init_protocol_vector(void);
extern void crypto_glue_set_session_key_timeout(ushort);
extern ushort crypto_glue_get_session_key_timeout(void);
extern void crypto_glue_clear_connection(int, ushort); 
extern void crypto_glue_initialize_cisco_nnc(void);
extern uchar *crypto_glue_find_msg_end_addr(uchar *);
extern int crypto_glue_get_msg_length(uchar *, boolean);
extern uchar *crypto_glue_find_sig_addr(uchar *);
extern int crypto_glue_find_message_type(uchar *, 
					 crypto_sesmgmt_message_types *);
extern void crypto_glue_insert_network_address(uchar *, uchar *, int);
extern void crypto_glue_long_pending_chk(void);
extern void crypto_snmp_conn_update(void);
extern void crypto_glue_clear_conn_id(int, int, ushort);
extern char *crypto_xlate_cib_status(ulong);
extern boolean crypto_glue_find_pending_conn (uchar *, crypto_sesmgmt_block *);
