/* $Id: crypto_hwepa.h,v 1.1.4.3 1996/07/28 07:44:34 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_hwepa.h,v $
 *------------------------------------------------------------------
 * crypto_hwepa.h - type and constants for EPA. 
 *
 * Mar, 1996, xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_hwepa.h,v $
 * Revision 1.1.4.3  1996/07/28  07:44:34  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/07/15  09:59:28  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/19  14:57:19  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1996/03/26  08:55:09  xliu
 * Branch: IosSec_branch
 * Check in EPA support code.
 *
 * Revision 1.1  1996/03/26  08:49:01  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_HWEPA_H_
#define CRYPTO_HWEPA_H_

/*
 * Firmware commands
 */

#define EPA_HMA_ALLOC          0x03
#define EPA_HMA_FREE           0x04
#define EPA_HMA_WRITE          0x01
#define EPA_HMA_READ           0x02
#define EPA_MEM_SYNDROME       0x2E   
#define EPA_HMA_COPY           0x2b
#define EPA_READ_RANDOM        0x05
#define EPA_GEN_RANDOM         0x06
#define EPA_SHA                0x25
#define EPA_GEN_DSS_KEY        0x08
#define EPA_GEN_DSS_SIGNATURE  0x0a
#define EPA_VER_DSS_SIGNATURE  0x29
#define EPA_GEN_DH_PAIR        0x0b
#define EPA_GET_DH_SHARED      0x0c
#define EPA_SET_DESKAPPA_KEY   0x0d
#define EPA_SET_CIPHER_KEY     0x20
#define EPA_GET_EPA_STATUS     0x34
#define EPA_SET_EPA_STATUS     0x35
#define EPA_SET_EPA_PASSWD     0x1e
#define EPA_ZEROIZE_EPA        0x1f
#define EPA_READ_FLASH         0xa1
#define EPA_WRITE_FLASH        0x1d
#define EPA_SET_RN_SEED        0x21
#define EPA_SET_SESSION        0x11
#define EPA_FREE_SESSION       0x12
#define EPA_GET_TIME           0x2c
#define EPA_SET_TIME           0x2d 
#define EPA_TRANS_END          0xff

/* command used by IOS */
#define EPA_SET_ENCRYPT_KEY 1101
#define EPA_SET_IV_KEY      1102
#define EPA_CHECK_STATUS             1103 
#define EPA_FREE_ENCRYPT_KEY         1104
#define EPA_FREE_IV_KEY              1105
#define EPA_FREE_DH_PRIVATE          1107
#define EPA_FREE_DH_SHARED           1108 

/*
 * Algorithm constants
 */
#define CRYPTO_SHA_HASH_SIZE      20
#define CRYPTO_SIG_R_SIZE         20
#define CRYPTO_SIG_S_SIZE         20
#define CRYPTO_DH_PUBLIC_SIZE     64
#define CRYPTO_DSS_PRIVATE_KEY_SIZE 20

#define EPA_ENCRYPT_METHOD        0x0b
#define EPA_ENCRYPT_ALG           0x0b

#define CRYPTO_ESA_DSS_PRIVATE_KEY_HANDLE  65535

/*
 * EPA status flag mask
 */
#define EPA_TAMPER          0x80000000
#define EPA_XTRACT          0x40000000
#define EPA_TESTING         0x20000000
#define EPA_PRIVMOD         0x10000000
#define EPA_PASSWD          0x08000000
#define EPA_RNG_SD          0x04000000
#define EPA_DSSKEY          0x00080000
#define EPA_1024FAIL        0x00400000
#define EPA_1046FAIL        0x00200000
#define EPA_BOOTFAIL        0x00100000
#define EPA_LED_BOOT        0x00000080
#define EPA_LED_NORMAL      0x00000040
#define EPA_LED_ERROR       0x00000020
#define EPA_LED_TAMPER      0x00000010


/*
 * HMA memory attribute
 */
#define EPA_HMA_BLK_PERMANENT 0x04
#define EPA_HMA_CHECKSUM_ON   0x08
#define EPA_HMA_READ_ALLOWED  0x10
#define EPA_HMA_WRITE_ALLOWED 0x20

#define EPA_DSS_PRIVATE_KEY_HMA_ATTRI 0x30 
#define EPA_SESSION_KEY_HMA_ATTRI     0x30
#define EPA_IV_KEY_HMA_ATTRI          0x30
#define EPA_DH_SHARED_HMA_ATTRI       0x30
#define EPA_DH_PRIVATE_HMA_ATTRI      0x30
#define EPA_RANDOM_SIZE               0x14
#define EPA_RANDOM_HMA_ATTRI          0x30

/*
 * Key HMA size
 */
#define EPA_SESSION_KEY_SIZE          8
#define EPA_IV_KEY_SIZE               8
#define EPA_DSS_PRIVATE_KEY_SIZE      20
#define EPA_DH_PRIVATE_SIZE    64
#define EPA_DH_SHARED_SIZE     64

/*
 * Encryption Alg Method
 */
#define EPA_DES56_CFB8                0x0A
#define EPA_DES56_CFB64               0x0B
#define EPA_DES56_CBC64               0x0C
#define EPA_DES56_ECB                 0x0D
#define EPA_DES40_CFB8                0x01
#define EPA_DES40_CFB64               0x02
#define EPA_DES40_CBC64               0x03
#define EPA_DES40_ECB                 0x04



/*
 * key request command header definition. The actual data input/output
 * follows the command header. The size of the data is specified in the
 * header.
 */
typedef struct {
    uchar     command;
    uchar     status;
    ushort    byte_to_alloc;
    uchar     attribute;
    uchar     reserved;
    ushort    handle;      /* output */
} crypto_hma_alloc_t;

typedef struct {
    uchar     command;
    uchar     status;
    ushort    handle;
} crypto_hma_free_t;

typedef struct {
    uchar     command;
    uchar     status;
    ushort    handle;
    ushort    byte_to_write;
    ushort    reserved;
} crypto_hma_write_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   handle;
    ushort   byte_to_read;
    ushort   reserved;
} crypto_hma_read_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   handle;
    ushort   offset;
    ushort   size;
    ushort   syndrome_size;
} crypto_mem_syndrome_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   byte_to_copy;
    ushort   src_handle;
    ushort   dst_handle;
} crypto_hma_copy_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   rand_num_size;
} crypto_read_random_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   rand_num_size;
    ushort   handle;
} crypto_gen_random_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   msg_size;
} crypto_sha_t;

typedef struct {
    uchar    command;
    uchar    status;
    short    handle;
    ushort   modulus_size;
    ushort   reserved;
    uchar    public_number[DH_LENGTH_MIN];
} crypto_gen_dss_key_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   private_key_handle;
    ushort   modulus_size;
    ushort   reserved;
    uchar    sha_hash[CRYPTO_SHA_HASH_SIZE];
} crypto_gen_dss_signature_t;

typedef struct {
    uchar    sig_r[CRYPTO_SIG_R_SIZE];
    uchar    sig_s[CRYPTO_SIG_S_SIZE];
} crypto_sig_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   modulus_size;
    uchar    sha_hash[CRYPTO_SHA_HASH_SIZE];
    crypto_sig_t sig;
    uchar    public_number[DH_LENGTH_MIN];
}crypto_verify_dss_signature_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   private_number_handle;
    ushort   reserved;
    ushort   modulus_size;
    uchar    public_number[DH_LENGTH_MIN];
} crypto_gen_dh_pair_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   private_number_handle;
    ushort   shared_number_handle;
    ushort   modulus_size;
    uchar    public_number[CRYPTO_DH_PUBLIC_SIZE];  /* here we assume t*/
                                                    /* the fixed size */
} crypto_get_dh_shared_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   number_of_bytes;
    ushort   shared_number_handle;
    ushort   key_handle;
} crypto_set_deskappa_key_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   iv_key_handle;
    ushort   encrypt_key_handle;
    ushort   encrypt_method;
    ushort   new_session_handle;   /* output */
}crypto_set_new_session_t;

typedef struct {
    uchar command;
    uchar status;
    ushort   session_handle;
} crypto_free_session_t;

typedef struct {
    uchar command;
    uchar status;
    ushort pad;
    ulong flags;       /* output */
    ulong version;     /* output */
    ulong reset_cr;    /* output */
    ulong sys_error;   /* output */
    ulong error_time;  /* output */
} crypto_get_epa_status_t;

typedef struct {
    uchar command;
    uchar status;
    ushort passwd_size;
    ulong  mask;
    ulong  flag;
} crypto_set_epa_status_t;

typedef struct {
    uchar command;
    uchar status;
    ushort passwd_size;
} crypto_set_epa_passwd_t;

typedef struct {
    uchar command;
    uchar status;
    ushort seed_size;
} crypto_set_rn_seed_t;

typedef struct {
    uchar command;
    uchar status;
} crypto_zeroize_t;

typedef struct {
    uchar command;
    uchar status;
    ushort handle;
    ushort offset;
    ushort hma_buffer_size;
    ushort syndrome_size;
    ushort reserved;
} crypto_gen_syndrome_t;

typedef struct {
    uchar    command;
    uchar    status;
    ushort   number_of_bytes;
    ushort   shared_number_handle;
    ushort   offset;
    ushort   key_handle;
    ushort   method;
} crypto_set_cipher_key_t;

static inline ushort epa_encrypt_method (ushort alg_id) 
{
    switch (alg_id) {
    case DES_CFB8_IV1:
         return EPA_DES56_CFB8;
    case DES_CFB64_IV1:
    default:
         return EPA_DES56_CFB64;
    case DES_40_CFB8_IV1:         
         return EPA_DES40_CFB8;
    case DES_40_CFB64_IV1:        
         return EPA_DES40_CFB64;
    }
}

/* function prototype */
epa_status hMemAlloc(ushort, uchar, epa_crypto_blk*);
epa_status hMemFree (ushort, epa_crypto_blk *);
epa_status hMemRead (ushort, short, epa_crypto_blk *);
epa_status hMemWrite (ushort, short, void *, epa_crypto_blk *);
epa_status hGenRandom (ushort, ushort,  epa_crypto_blk *);
epa_status hReadRandom (ushort, uchar *, epa_crypto_blk *);
epa_status hGenDSSKey (ushort, ushort, epa_crypto_blk *);
epa_status 
    hGenDSSSignature (ushort, uchar*, ushort,  epa_crypto_blk*);
epa_status 
    hVerDSSSignature (uchar *,ushort,crypto_sig_t *, uchar *, epa_crypto_blk *);

epa_status
    hGenDHPair (ushort, ushort, epa_crypto_blk *);
epa_status
    hGetDHShared (ushort, ushort, ushort, uchar *, epa_crypto_blk *);
epa_status
    hSetDESKAPPAKey (ushort, ushort, ushort, epa_crypto_blk *);
epa_status
    hSetCipherKey (ushort, ushort, ushort, ushort, ushort, epa_crypto_blk *);
epa_status
    hSetSession (ushort, ushort, ushort, epa_crypto_blk *);
epa_status 
    hFreeSession (ushort, epa_crypto_blk *);
epa_status
    hSetEPAPasswd(uchar*, ushort, epa_crypto_blk *);
epa_status
    hSHA (uchar *, ushort, epa_crypto_blk *);

epa_status hSetRNSeed(uchar*, ushort, epa_crypto_blk*);

epa_status hSetEPAStatus(ushort, ulong, ulong, uchar *, epa_crypto_blk *);

epa_status hGetEPAStatus(epa_crypto_blk *);

epa_status hZeroize(epa_crypto_blk *);

epa_status hGenSyndrome(ushort, ushort, ushort, epa_crypto_blk*);

void send_set_des_key_comm (epa_crypto_blk *);
void send_sha_comm (ushort, uchar *, ushort, epa_crypto_blk *);
void send_hma_alloc_comm(ushort, ushort, ushort, epa_crypto_blk *);
void send_hma_free_comm(ushort, ushort, epa_crypto_blk *);
void send_gen_dss_pair_comm (ushort, epa_crypto_blk *);
void vip_crypto_module_init(epa_crypto_blk *);
void check_epa_status(void);
void send_set_epa_passwd_comm (ushort, uchar *, ushort, epa_crypto_blk*);
void send_set_epa_status_comm (ushort, ulong, ulong, epa_crypto_blk *);
void send_get_epa_status_comm (ushort, epa_crypto_blk *);
void send_set_new_session_comm (ushort, ushort, ushort, epa_crypto_blk *);
void send_set_iv_key_comm (ushort, epa_crypto_blk *blk);
void send_set_session_key_comm (ushort, epa_crypto_blk *blk);

void send_init_random_comm(ushort, ushort, epa_crypto_blk*);
void send_ver_dss_sig_comm (uchar *,ushort, uchar *,uchar *, epa_crypto_blk*);
void send_gen_dss_sig_comm (ushort, uchar *, ushort, epa_crypto_blk *);
void send_gen_dh_shr_comm (ushort, ushort, ushort, uchar *, epa_crypto_blk *);
void send_gen_dh_pair_comm (ushort, epa_crypto_blk *);
void send_free_session_comm (ushort, ushort, epa_crypto_blk *);
void send_set_rn_seed (ushort, uchar *, ushort, epa_crypto_blk *);
void send_zeroize(ushort, epa_crypto_blk *);
void send_gen_syndrome(ushort, ushort, ushort, ushort, epa_crypto_blk *);
void send_hma_read (ushort, ushort, ushort, epa_crypto_blk *);

#define CRYPTO_DSS_PH_1      -1
#define CRYPTO_DSS_PH_2      -2

#define EPA_TEST_ONLY  0xFFFF

#endif
