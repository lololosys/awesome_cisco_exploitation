/* $Id: crypto_swepa.h,v 1.1.66.10 1996/08/09 23:34:08 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_swepa.h,v $
 *------------------------------------------------------------------
 * crypto_swepa.h
 * 
 * This file defines the types and constans used by software encryption
 * key mgt proccess.
 *
 * 11, 1995, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa.h,v $
 * Revision 1.1.66.10  1996/08/09  23:34:08  sbelair
 * CSCdi60324:  CPU hogs on 4000 (Encryption Process)
 * Branch: California_branch
 * No longer need EPA_SWCRYPTO_ENCRYPT_COUNT, since the crypto process
 * will now time its runs, rather than count packets.
 *
 * Revision 1.1.66.9  1996/07/22  07:38:48  xliu
 * CSCdi63492:  PPP does not work when switched from HDLC on EPA/VIP2
 * Branch: California_branch
 *
 * Revision 1.1.66.8  1996/07/17  09:32:18  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.66.7  1996/07/15  10:06:03  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.66.6  1996/07/03  05:54:50  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.66.5  1996/06/21  22:30:07  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.66.4  1996/06/01  01:14:38  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.66.3  1996/05/28  15:59:19  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.66.2  1996/05/12  23:12:48  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.66.1  1996/04/19  14:56:37  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.5  1996/03/21  10:07:20  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 1.1.2.4  1996/03/16  01:41:30  xliu
 * Branch: IosSec_branch
 * Add new constant.
 *
 * Revision 1.1.2.3  1996/01/19  07:00:51  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.2  1996/01/04  18:35:45  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.1  1995/11/30  04:59:47  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:53  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef SW_EPA_CRYPTO_H
#define SW_EPA_CRYPTO_H

#ifndef DH_LENGTH_MIN  
#define DH_LENGTH_MIN 64
#endif
/*
 * SW EPA process status and process id */
#define  EPA_SWCRYPTO_PROCESS_RUN	1
#define  EPA_SWCRYPTO_PROCESS_DEAD    0
#define  EPA_SWCRYPTO_PROCESS_FAKE_PID -1

/* DES encrypt/decrypt block lengthh    */
#define  EPA_DES_ENCDEC_BLOCK_LENGTH    8

/* Exp process timeout constants  */
#define  EPA_SWCRYPTO_EXP_YIELD_TIME    1000L
#define  EPA_SWCRYPTO_EXP_RUN_TIME      100

/* Default encryption process config constants */
#define  EPA_ENCRYPT_QUE_SIZE	        500	

/* Connection state */
#define  EPA_SWCRYPTO_CONN_UNSET        0
#define  EPA_SWCRYPTO_CONN_SET          1
#define  EPA_SWCRYPTO_CONN_CLEAR        2

/* DH table entry state */
#define  DH_NUMBER_PAIR_ASSIGNED        1
#define  DH_NUMBER_PAIR_NOT_ASSIGNED    0

/* SW DSS related constats. */
#define  CRYPTO_RANDOM_SEED_SIZE        33

/* Cylink define RVAL as SHA_LENGTH which is 20 bytes */
#define  CRYPTO_RANDOM_NUM_SIZE         20

/* SW DES related constants. */
#define  CRYPTO_DES_KEY_VALUE_SIZE      8

/* Dropped packet chunk related constant */
#define CRYPTO_DROPPED_PACKETS_CHUNK_MAX 10

typedef void (* crypto_encrypt_decrypt_proc)(int, uchar *, uchar *, uchar *,
                                             ushort);

typedef struct crypto_conn_idb_t_ {
    struct crypto_conn_idb_t_ *next;
    hwidbtype *hwidb;
}crypto_conn_idb_t;


/*
 * structure definition for the connection table. This is a
 * copy of the connection table used internally by the sw epa.
 */
typedef struct {
    ushort    connection_id;
    ushort    alg_type;                      /* used to encrypt/decrypt */
    uchar*    expanded_encrypt_key;          /* 16 * 8 byte key */ 
    uchar     encrypt_key[8];
    uchar     iv_key[8];                     /* key to generate iv */
    ushort    state;                         /* set or unset */
    idbtype*  swidb;                        
    ulong     encrypt_count;
    ulong     decrypt_count;
    ulong     drop_count;
    crypto_conn_idb_t *idbs;
} epa_swcrypto_connection;

/*
 * Structure definition for the connection table data that is transferred
 * from VIP to RSP. 
 */
typedef struct {
    ushort    connection_id;
    ushort    alg_type;                      
    ushort    state;                         /* set or unset */
    uint      if_index;                      /* hw_if_index for interface */
    ulong     encrypt_count;
    ulong     decrypt_count;
    ulong     drop_count;
} epa_vip_connection_tbl;

/*
 * swepa DH number pair table
 */
typedef struct {
   uchar     secret_number[DH_LENGTH_MIN];
   uchar     public_number[DH_LENGTH_MIN];   
} epa_swcrypto_DH_pair;

static inline void crypto_exp_iv (uchar *iv, uchar *iv_seed, 
                                  ushort iv_seed_size)
{
    ulong x;
    ushort i;

   switch (iv_seed_size) {
   case CRYPTO_TCP_IV_SIZE:
       bcopy(iv_seed, iv, CRYPTO_TCP_IV_SIZE);
       break;
   case CRYPTO_UDP_IV_SIZE:
   default:
       /* 
        * For all the protocls, except tcp, two bytes are taken from the
        * packet as the iv seed. This is consistent with Cylink with 
        * respect to UDP protocl. It is the Cisco defintion for other
        * protocol, including packets with only ip header.
        */
       x = *(ushort *)iv_seed;
       x = (x & 0x0000ffffL) | (~x << 16);  /* expand it into 32 bit value */
       for (i=0; i<8; i+=4)
           *((ulong*)&iv[i]) = x;           /* expand it into 64 bit value */
       break;
   }
}

static inline void crypto_iv_gen (uchar *iv, uchar* iv_seed, 
                                  ushort iv_seed_size, uchar* iv_key)
{
   ushort i;

   crypto_exp_iv(iv, iv_seed, iv_seed_size);

   /* XOR the iv key */
   for (i=0; i<8; i++) iv[i] ^= iv_key[i];
}

int crypto_release(int);
void crypto_drop_packet(crypto_encrypt_header *);

extern epa_swcrypto_connection epa_swcrypto_conn_tbl[]; 
extern ushort epa_packet_number;
extern uchar test_data[];
extern uchar epa_pub_num_tbl[][];
extern uchar crypto_pub_key[];
extern uchar crypto_pri_key[];
extern uchar crypto_ran_num[];
extern uchar fake_dh_number[];
extern uchar crypto_encrypt_pad[];
extern crypto_dh_pair crypto_dh_pairs[];
extern ulong crypto_release_counter;
extern int crypto_engine_key_proc_yield(int);
extern YIELD_context crypto_yield_handle;
extern crypto_drop_pak_t *crypto_dropped_packets_list;

/*
 * Error message
 */

#endif



