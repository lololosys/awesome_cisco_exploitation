/* $Id: crypto.h,v 3.2.58.6 1996/07/28 07:41:49 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto.h,v $
 *------------------------------------------------------------------
 * crypto.h - Protocal-independant crypto definitions
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto.h,v $
 * Revision 3.2.58.6  1996/07/28  07:41:49  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/06/18  00:21:50  bew
 * CSCdi60362:  Crypto router choose 40-des over 56-des
 * Branch: California_branch
 * The strongest DES possible is now negotiated during connection setup.
 *
 * Revision 3.2.58.4  1996/06/07  00:23:58  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.58.3  1996/06/05  23:11:16  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.2.58.2  1996/05/30  20:23:58  bew
 * CSCdi58950:  Need to make exportable crypto images
 * Branch: California_branch
 * Added the exportable crypto subsystem. The guts of crypto_init() are
 * moved into crypto.c, and crypto_exportable_init.c is added. The
 * strong_encryption_required boolean added and set appropriately
 * by both the crypto and exportable crypto subsystems.
 *
 * Revision 3.2.58.1  1996/04/19  14:55:30  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.28  1996/04/17  23:47:15  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.6.27  1996/04/16  21:51:49  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.26  1996/03/25  18:43:36  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.6.25  1996/03/21  19:56:38  bew
 * Branch: IosSec_branch
 * Move crypto export message to be printed from crypto_init().
 *
 * Revision 2.1.6.24  1996/03/13  22:44:52  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.6.23  1996/03/13  00:57:13  bew
 * Branch: IosSec_branch
 * Addition of "Compliance with Export Laws" message to images with crypto.
 *
 * Revision 2.1.6.22  1996/03/01  01:38:46  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.21  1996/01/25  18:19:46  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 2.1.6.20  1996/01/19  07:00:35  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 2.1.6.19  1996/01/13  05:46:35  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.6.18  1996/01/04  18:47:45  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.6.17  1995/12/11  21:37:47  xliu
 * Branch: IosSec_branch
 * Add CRYPTO_SWITCH_MODES.
 *
 * Revision 2.1.6.16  1995/12/10  09:33:11  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.6.15  1995/12/02  17:44:29  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 2.1.6.14  1995/11/30  05:54:11  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.13  1995/11/28  01:46:20  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.12  1995/11/17  16:33:12  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.11  1995/10/14  18:33:16  xliu
 * Branch: IosSec_branch
 * Add crypto-engine parser commands.
 *
 * Revision 2.1.6.9  1995/10/10  18:19:28  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.8  1995/09/27  23:20:41  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.7  1995/09/08  08:03:05  xliu
 * Branch: IosSec_branch
 * Eliminate CRYPTO_FAST_ENCRYPT/DECRYPT cryptoflags. Replace it with the
 * switch_mode input parameter to epa encrypt/decrypt api.
 * Replace CRYPTO_DONE cryptoflag with CRYPTO_ENCRYPT_DONE and
 * CRYPTO_DECRYPT_DONE flag to fix the bug caused by reused pak in ping
 * application.
 *
 * Revision 2.1.6.6  1995/09/07  22:41:05  bew
 * Branch: IosSec_branch
 * Connection setup now sets up a shared DH number from which DES keys
 * can be derived.
 *
 * Revision 2.1.6.5  1995/08/30  20:41:33  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.4  1995/08/11  00:59:15  xliu
 * Branch: IosSec_branch
 *
 * Modified epa_parser.c with the updated epa interface.
 *
 * Revision 2.1.6.3  1995/08/02  19:24:28  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.2  1995/07/21  04:36:33  xliu
 * Branch: IosSec_branch
 * Update epa_parser.c with new epa api. Add CRYPTO_DECRYPT flag.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:17  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:12:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.2  1995/06/06  21:33:52  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:55  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:24  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * number of switching mode: used by registry
 */
#define CRYPTO_SWITCH_MODES    0x04

/*
 * WARNING! Cisco representation of the crypto methods bitmap. Positions 
 * within the bitmap MUST correspond to the Cylink cyMethodTbl structure in 
 * cy_frm.c!
 */
#define CRYPTO_DES_56_8 0x01010000 /* Note: Two bits, one for SDU compat */
#define CRYPTO_DES_56   0x02000000
#define CRYPTO_DES_40_8 0x00100000
#define CRYPTO_DES_40   0x00200000

#define CRYPTO_DES_56_STR	"des"
#define CRYPTO_DES_40_STR	"40-bit-des"
#define CRYPTO_CFB64_STR	"cfb-64"
#define CRYPTO_CFB8_STR		"cfb-8"

#define CRYPTO_DES_56_DESC	"DES - Data Encryption Standard (56 bit keys)."
#define CRYPTO_DES_40_DESC	"DES with 40 bit keys."
#define CRYPTO_CFB64_DESC	"Cipher Feedback Mode (64 bits of feedback)"
#define CRYPTO_CFB8_DESC	"Cipher Feedback Mode (8 bits of feedback)"

/*
 * CRYPTO_CHALLENGE_SIZE needs to be synch'ed with CHALLENGE_SIZE in
 * the SDU code.
 */
#define CRYPTO_CHALLENGE_SIZE 20

/*
 * Data structure to find data & signature pointers for epa_sign &
 * epa_verify.
 */
typedef struct crypto_sig_params_ {
    char *clear_data;
    ushort clear_data_length;
    char   *signature;
    ushort signature_length;
} crypto_sig_params;

/* 
 * CRYPTO_MAX_SLOTS is the maximum number of slots on any chasis. Used by
 * the parser macros who require a constant, but the IOS MAX_SLOTS isn't
 * available on all platforms, sigh. Other functions should use 
 * crypto_number_of_slots() to get the exact number.
 */
#define CRYPTO_MAX_SLOTS 16  
extern ushort crypto_number_of_slots;
#define CRYPTO_INVALID_SLOT      100
#define NO_EXTENDED_SLOTS_ON_CHASSIS (crypto_number_of_slots == 1)
#define VALID_SLOT(slot) ((slot >= 0) && (slot < crypto_number_of_slots))

extern ulong crypto_processor_serial;

/*
 * This is used as a function in many places (such as parse chains) so can't 
 * be preprocessor define.
 */
static inline boolean crypto_chassis_has_slots (void)
{
    return(idb_name_format()==EXTENDED_SLOTTED_IDB_NAMES);
}

/*
 * DH pregeneration list stuff
 */
typedef struct crypto_dh_pregen_ {
    struct crypto_dh_pregen_ *next;
    ulong conn_id;
} crypto_dh_pregen;

extern crypto_dh_pregen **crypto_dh_pregen_head_array;
extern ushort *crypto_dh_pregen_count_array;

extern ushort crypto_sdu_connections;
extern ushort crypto_sdu_entities;

/*
 * Export thingy - defined in crypto_init.c and crypto_exportable_init.c
 */
extern boolean strong_crypto_allowed;

/*
 * Determine which default algorithm, based on whether this is the 
 * exportable crypto subsystem.
 */
static inline int crypto_get_default_algorithm (void)
{
    if (strong_crypto_allowed) {
	return (CRYPTO_DES_56);
    } else {
	return (CRYPTO_DES_40);
    }
}
/*
 * Function Prototypes
 */

extern boolean crypto_verify_me_sdu(uchar *, uchar *, ushort, uchar *);
extern boolean crypto_generate_DH_parameters(short, char *, short, ushort);
extern boolean crypto_create_session_key(short, ushort, short);
extern boolean crypto_generate_syndrome(short,ushort,short,short,uchar*);
extern void crypto_init_alg_vector(void);
extern uint crypto_get_alg_vector(void);
extern void crypto_set_alg_vector(int, boolean);
extern void crypto_clear_connection_entry(int, ushort);
extern boolean crypto_pregen_dh_pair(ushort, ushort);
extern boolean crypto_sign_me(ushort, char *, ushort, char *);
extern boolean crypto_is_alg_set(int);
extern void crypto_ip_recompute_icmp_checksum(paktype *);
extern boolean crypto_is_exportable_system(void);
extern ushort crypto_get_number_of_slots(void);
extern int GetRTClock(ulong *);
extern void crypto_parser_init(void);
extern void crypto_engine_init(ushort, ushort);
extern void crypto_debug_init(void);
extern void crypto_glue_cs_init(void);
extern void crypto_glue_ep_init(void);
extern void crypto_sesmgmt_init(void);
extern void crypto_conn_init(void);
extern void crypto_glue_init_connection_ids(void);
extern void crypto_glue_pregen_dh(void);
extern void crypto_init_common(void);
extern uchar * crypto_card_error_code(ushort);
