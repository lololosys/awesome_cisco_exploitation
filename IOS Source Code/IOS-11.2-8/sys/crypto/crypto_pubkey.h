/* $Id: crypto_pubkey.h,v 3.1.58.4 1996/07/01 09:05:59 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_pubkey.h,v $
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 * crypto_pubkey.h - IOS public definitions for public/private keys
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_pubkey.h,v $
 * Revision 3.1.58.4  1996/07/01  09:05:59  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.1.58.3  1996/06/21  22:29:19  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.58.2  1996/06/05  23:11:32  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.58.1  1996/04/19  14:55:56  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.12  1996/04/16  21:52:40  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.11  1996/03/13  22:45:14  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.10  1996/02/29  19:03:39  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 2.1.2.9  1996/01/13  05:46:47  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.8  1995/11/28  01:46:38  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.7  1995/11/17  16:33:21  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.6  1995/10/17  01:19:38  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.5  1995/09/02  05:05:26  xliu
 * Branch: IosSec_branch
 *
 * Add function to retrive dss keys when reinitializaing.
 *
 * Revision 2.1.2.4  1995/08/30  20:41:41  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.2.3  1995/08/02  19:24:34  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.2.2  1995/07/08  01:55:44  bew
 * Branch: IosSec_branch
 * Added "show crypto pubkey" commands to show a peer public key by
 * either peer name or peer serial number.
 *
 * Revision 2.1.2.1  1995/07/01  05:31:24  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 1.1.2.1  1995/06/23  02:57:03  bew
 * Branch: IosSec_branch
 * Support the storing of public keys in the configuration.
 *
 * Revision 3.1  1995/11/09  11:13:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/29  22:59:54  carrel
 * Placeholders for IosSecurity.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Peer public key list definitions
 */

/* 
 * Value of the variable "dhPublicLen" in sdu/task/auth/authcy.c. This
 * ought not be hardcoded, but somehow use that variable, sigh.
 */
#define CRYPTO_KEYSIZE 64  
#define CRYPTO_EPA_NAME_LENGTH 32

typedef struct crypto_key_ {
    ushort keylen;
    uchar key[CRYPTO_KEYSIZE];
} crypto_key;

typedef struct crypto_pubkey_ {
    struct crypto_pubkey_ *next;
    uchar *name;
    uint serial;
    crypto_key pubkey;
} crypto_pubkey;

typedef struct crypto_mykey_ {
    struct crypto_mykey_ *next;
    uint serial;
    crypto_key private_key;
    crypto_key public_key;
    ushort slot;
    uchar *name;
} crypto_mykey;

/*
 * Externs
 */
extern void crypto_pubkey_print(uint, uchar *, crypto_key *);

extern void crypto_pubkey_print_key_data(int, uchar *, boolean, boolean);

extern crypto_pubkey *crypto_pubkey_get_by_name(uchar *);

extern crypto_pubkey *crypto_pubkey_get_by_serial(uint);

extern void crypto_pubkey_hexmode (parseinfo *);

extern void crypto_pubkey_cmd(parseinfo *);

extern void crypto_pubkey_dss_private_key_cmd(parseinfo *);

extern void crypto_pubkey_dss_public_key_cmd(parseinfo *);
  
extern void crypto_show_pubkey_by_name(parseinfo *);

extern void crypto_show_pubkey_by_serial(parseinfo *);

extern void crypto_show_pubkey_all(parseinfo *);

extern void crypto_show_mypubkey(ulong);

extern void crypto_show_mypubkey_all(parseinfo *);

extern crypto_mykey *crypto_pubkey_get_keypair_by_name(uchar *);

extern void crypto_pubkey_keypair_cmd(parseinfo *);

extern void crypto_pubkey_keypair_hexmode (parseinfo *);

extern void crypto_pubkey_keypair_delete(crypto_mykey *);

extern boolean crypto_pubkey_keypair_add(crypto_mykey *);

extern crypto_mykey *crypto_dsskey_get_by_id(ushort);

extern crypto_pubkey *crypto_public_key_list_head;
extern crypto_mykey *crypto_my_key_list_head;

extern boolean crypto_pubkey_add(crypto_pubkey *);

extern void crypto_pubkey_freekey(crypto_pubkey *);

extern crypto_mykey *crypto_pubkey_get_keypair_by_serial(ulong);

uchar *crypto_get_key_name_by_slot(ushort);

crypto_mykey *crypto_mykey_get_by_slot(ushort);
extern crypto_mykey *crypto_pubkey_get_keypair_by_slot(ushort);

extern void crypto_pubkey_keypair_freekey(crypto_mykey *);
