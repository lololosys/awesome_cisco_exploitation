/* $Id: crypto_conntable.h,v 3.2.60.2 1996/05/31 07:50:44 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_conntable.h,v $
 *------------------------------------------------------------------
 * crypto_conntable.h - Crypto Master Connection Table definitions
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_conntable.h,v $
 * Revision 3.2.60.2  1996/05/31  07:50:44  bew
 * CSCdi56128:  Invalid connection pair
 * Branch: California_branch
 * Fixed off-by-1 error in determining if a connection id is valid.
 *
 * Revision 3.2.60.1  1996/04/19  14:55:33  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.17  1996/04/16  21:51:55  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.16  1996/04/12  23:07:25  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.6.15  1996/03/13  22:44:59  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.6.14  1996/03/01  01:38:51  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.13  1996/01/13  05:46:37  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.6.12  1996/01/04  18:47:47  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.6.11  1995/12/20  23:30:41  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.6.10  1995/11/28  01:46:22  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.9  1995/10/16  04:04:15  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.7  1995/10/10  18:19:31  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.6  1995/09/26  04:12:02  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.6.5  1995/09/22  01:21:53  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.6.4  1995/09/14  19:32:33  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.3  1995/08/30  20:41:35  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.2  1995/08/02  19:24:30  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 1.1.2.1  1995/06/02  00:19:54  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01  23:16:25  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Values which define states for ACL processing & coonnection setup code.
 * The errors must be below the minimum threshhold for valid unique pending 
 * values. The maximum connection table entry number is determined by the
 * size of the crypto engine connection table (CRYPTO_SWEPA_CONNTBL_SIZE - 1).
 */
#define CRYPTO_CONN_MAXIMUM 299
#define CRYPTO_CONN_UNSET   0
#define CRYPTO_CONN_MINIMUM -100000
#define CRYPTO_CONN_PENDING 	CRYPTO_CONN_MINIMUM - 2
#define CRYPTO_CONN_BAD     	CRYPTO_CONN_MINIMUM - 3
#define CRYPTO_CONN_BLOCKED 	CRYPTO_CONN_MINIMUM - 4
#define CRYPTO_CONN_ERROR   	CRYPTO_CONN_MINIMUM - 5
/*
 * The following are defined for convenience for crypto_test_initiate()
 */
#define CRYPTO_CONN_MAPERROR	CRYPTO_CONN_MINIMUM - 6
#define CRYPTO_CONN_ADDRERROR	CRYPTO_CONN_MINIMUM - 7

/*
 * Inline's for checking valid connection id values.
 */
static inline boolean crypto_conntable_conn_is_pending (int status)
{
    if ((status >= CRYPTO_CONN_MINIMUM) && (status < CRYPTO_CONN_UNSET)) {
	return(TRUE);
    } 
    return(FALSE);
}

static inline boolean crypto_conntable_conn_is_valid (int status)
{
    if ((status > CRYPTO_CONN_UNSET) && (status <= CRYPTO_CONN_MAXIMUM)) {
	return(TRUE);
    } 
    return(FALSE);
}

/*
 * Chunk defines
 */
#define CRYPTO_CONN_CHUNK_MAX 10
extern chunk_type *crypto_blk_pool;
extern short crypto_conn_pending_value;


/*
 * Function Prototypes
 */

extern void crypto_conn_init (void);

extern void crypto_conn_entry_initiate_ip(int *, ipaddrtype, ipaddrtype,
					  ipaddrtype, ipaddrtype,
					  ushort, int, char **, uint);

extern long crypto_conn_find_uniq_pending_value (void);
