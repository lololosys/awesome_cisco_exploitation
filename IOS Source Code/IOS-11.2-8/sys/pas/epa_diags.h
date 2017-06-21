/* $Id: epa_diags.h,v 
 *------------------------------------------------------------------
 * epa_diags.h - Diagnostic test for Encryption PA.
 *
 * June 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: epa_diags.h,v $
 * Revision 3.1.2.4  1996/09/11  20:13:23  ptran
 * CSCdi68839:  $Log is missing from esa files
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EPA_DIAGS_H__
#define __EPA_DIAGS_H__
/*****************************************************************************
 * Use for testing EPA board functionality
 *****************************************************************************/
#if defined (SVIP)
#define MAX_FRAME_SIZE 4096
#else
#define MAX_FRAME_SIZE	4096 /* Maximum frame size that EPA will accept */
#endif
#define MAX_OFFSET	999999
#define MAXPACKET	0xffffffff
#define SIZE_64K	65536
#define SIZE_32K	32768
#define SIZE_4K 	4*1024 
#define SIZE_1K 	1024
#define MAX_ALLOC 331 /*86*/
#define MAX_HMA_SIZE (64*1024) - (10*64) /*8 * 1024 - (29 * 64)*/
#define GHANDLE1_SIZE 64
#define GHANDLE2_SIZE 64
#define GHANDLE3_SIZE 20
#define GHANDLE4_SIZE 20
#define GHANDLE5_SIZE 1024

#define	ENCRYPT	0x0
#define DECRYPT 0x80
#define BITFLIP	0x20
#define TESTMODE 0x40
/*
 * Statistic counter
 */
typedef struct statistic {
    int intr_rcvd;
    int num_encrypt_snt;
    int num_encrypt_rcv;
    int num_decrypt_snt;
    int num_decrypt_rcv;
    int undef_cmd_snt;
    int undef_cmd_rcv;
} epad_stat_t;

/*
 * Encryption/Decryption control block
 */
typedef struct encrypt_cntl_blk_ {
    char call_id;
    char status;
    short session;
    uchar iv_gen[8];
    uchar reserved1[8];
    ushort offset;
    ushort total_length;
    uchar reserved2[8];
} encrypt_cntl_blk_t;


#define EPA_TAMPER  0x80000000
#define EPA_XTRACT  0x40000000
#define EPA_TESTING 0x20000000
#define EPA_PRIVMOD 0x10000000
#define EPA_PASSWD  0x08000000
#define EPA_RNG_SD  0x04000000
#define EPA_DSSKEY  0x00080000
#define EPA_1024FAIL    0x00040000
#define EPA_1046FAIL    0x00020000
#define EPA_BOOTFAIL    0x00010000
#define EPA_LED_BOOT    0x00000080
#define EPA_LED_NORMAL  0x00000040
#define EPA_LED_ERROR   0x00000020
#define EPA_LED_TAMPER  0x00000010

#define SHA_HASH_SIZE	20
#define R_SIZE		20
#define S_SIZE		20
#define Q_SIZE		20
#define PUBLIC_SIZE	64

typedef struct hMemAlloc_ {
    char	call_id;
    char	status;
    ushort	size;
    char	attribute;
    char	reserved;
    short	handle;
} hMemAlloc_t;

typedef struct hMemFree_ {
    char 	call_id;
    char	status;
    short	handle;
} hMemFree_t;

typedef struct hMemWrite_ {
    char 	call_id;
    char	status;
    ushort	handle;
    ushort	size;
    uchar	reserved[2];
} hMemWrite_t;

typedef struct hMemRead_ {
    char	call_id;
    char	status;
    short	handle;
    ushort	size;
    uchar	reserved[2];
} hMemRead_t;

typedef struct hMemSyndrome_ {
    char 	call_id;
    char	status;
    short	handle;
    ushort	offset;
    ushort	size;
    ushort	len;
    uchar	reserved[2];
} hMemSyndrome_t;

typedef struct hMemCopy_ {
    char	call_id;
    char	status;
    ushort	size;
    short	src_handle;
    short	dst_handle;
} hMemCopy_t;

typedef struct hReadRandom_ {
    char	call_id;
    char	status;
    ushort	size;
} hReadRandom_t;

typedef struct hGenRandom_ {
    char	call_id;
    char	status;
    ushort	size;
    short	handle;
} hGenRandom_t;

typedef struct hSHA_ {
    char	call_id;
    char	status;
    ushort	size;
} hSHA_t;

typedef struct 
{
    ulong state[5];
    ulong count[2];
    uchar buffer[64];
} SHA_context;

typedef struct hSHAinit_ {
    char	call_id;
    char	status;
    char	reserved[2];
    SHA_context context;
} hSHAinit_t;

/* fixme: what is context structure */
typedef struct hSHAupdate_ {
    char	call_id;
    char	status;
    ushort	size;
    SHA_context context;
} hSHAupdate_t;

typedef struct hSHAfinal_ {
    char	call_id;
    char	status;
    char	reserved[2];
    SHA_context context;
} hSHAfinal_t;

typedef struct hGenDSSparameters_ {
    char	call_id;
    char	status;
    ushort	size;
} hGenDSSparameters_t;

typedef struct hGetDSSpqg_ {
    char	call_id;
    char	status;
    ushort	size;
} hGetDSSpqg_t;

typedef struct hGenDSSnumber_ {
    char	call_id;
    char	status;
    short	handle;
    char	q[Q_SIZE];
} hGenDSSnumber_t;

typedef struct hGenDSSkey_ {
    char 	call_id;
    char	status;
    short	secret;
    ushort	size;
    uchar	reserved[2];
} hGenDSSkey_t;

typedef struct hGenDSSsignature_ {
    char	call_id;
    char	status;
    short	secret;
    ushort	size;
    char	reserved[2];
    uchar	hash[SHA_HASH_SIZE];
} hGenDSSsignature_t;

typedef struct hVerDSSsignature_ {
    char	call_id;
    char	status;
    ushort	size;
    uchar	hash[SHA_HASH_SIZE];
    uchar	R[R_SIZE];
    uchar	S[S_SIZE];
} hVerDSSsignature_t;

typedef struct hGenDHpair_ {
    char       	call_id;
    char	status;
    short	secret;
    char	reserved[2];
    ushort	size;
} hGenDHpair_t;

typedef struct hGetDHsharedNumber_ {
    char	call_id;
    char	status;
    short	secret;
    short	shared;
    ushort	size;
} hGetDHsharedNumber_t;

typedef struct hSetCipherkey_ {
    char	call_id;
    char	status;
    ushort	size;
    short	shared;
    short       offset;
    short	key;
    short       method;
} hSetCipherkey_t;

typedef struct hDHoneTimePad_ {
    char 	call_id;
    char	status;
    ushort	size;
    ushort	shared;
    char	reserved[2];
} hDHoneTimePad_t;

typedef struct hGetDHkey_ {
    char	call_id;
    char	status;
    ushort	size;
    short	secret;
    ushort	key1size;
    short	key1;
    ushort	key2size;
    short	key2;
} hGetDHkey_t;


typedef struct epaGetStatus_ {
    char 	call_id;
    char	status;
    char	reserved[2];
    ulong	flags;
    ulong	version;
    ulong	reset_ctr;
    ulong	sys_error;
    ulong	error_time;
} epaGetStatus_t;

typedef struct epaSetStatus_ {
    char	call_id;
    char	status;
    ushort	size;
    ulong	mask;
    ulong	flags;
} epaSetStatus_t;

typedef struct epaSetPassword_{
    char 	call_id;
    char	status;
    ushort	size;
} epaSetPassword_t;

typedef struct {
    char	call_id;
    char	status;
} epaZeroize_t;

typedef struct epaReKey_ {
    char	call_id;
    char	status;
    ushort	size;
} epaReKey_t;

typedef struct epaReadFlash_{
    char	call_id;
    char	status;
    ushort	size;
    ushort	offset;
    char	reserved[2];
} epaReadFlash_t;

typedef struct epaWriteFlash_ {
    char	call_id;
    char	status;
    ushort	size;
    ushort	offset;
    char	reserved[2];
} epaWriteFlash_t;

typedef struct setRNGseed_{
    char	call_id;
    char	status;
    ushort	size;
} setRNGseed_t;

typedef struct epaNewSession_ {
    char 	call_id;
    char	status;
    short	iv;
    short	key;
    ushort	method;
    short	handle;
} epaNewSession_t;

typedef struct epaFreeSession_ {
    char	call_id;
    char	status;
    short	handle;
} epaFreeSession_t;

typedef struct epaGetTime_{
    char	call_id;
    char	status;
    ushort	reserved;
    ulong	rtc_time;
    ulong 	boot_time;
} epaGetTime_t;

typedef struct epaGetMethods_ {
    char	call_id;
    char	status;
    ushort	reserved;
    char	methods[32];
} epaGetMethods_t;

typedef struct epa_ktest_ {
    char desc[200];
    char exp_status;
    boolean result;
} epa_ktest_t;

typedef struct epa_diag_ {
    volatile char	call_id;
    volatile char	status;
    volatile ushort	size;
    volatile uchar	function_code;
    volatile uchar	reserved1;
    volatile ushort	request_length;
    volatile ulong	diag_status;
} cyl_epa_diag_t;


#define SUCCESS			0
#define ERR_INVALID_DATA	1
#define ERR_MEM_ALLOC		2
#define ERR_INPUT_LEN		3
#define ERR_ERR_DSS_LEN		4
#define ERR_DH_LEN		5
#define	ERR_BLOCK_LEN		7
#define ERR_HASH_LEN		8
#define ERR_BAD_MODE		9
#define ERR_TK_NUMBER		10
#define	ERR_TK_POSITION		11
#define ERR_TK_COUNT		12
#define ERR_REQUEST_SIZE	13
#define ERR_DSS_SIGNATURE	21
#define ERR_TK_PRIME		22
#define ERR_WEAK_KEY		23
#define ERR_INPUT_VAL		24
#define ERR_ACCESS		25
#define ERR_CLAMP		26
#define ERR_FLASH_FAIL		27
#define ERR_DES_FAIL		28
#define ERR_BAD_PASSWD		29
#define ERR_BAD_STATUS		30
#define ERR_ALGORITHM		31
#define ERR_METHOD		32
#define ERR_HMA_HANDLE		33
#define ERR_HMA_SIZE		34
#define ERR_HMA_CHKSUM		35
#define ERR_HMA_ALLOC		36

#define ERR_INVALID_SESSION	101
#define ERR_OFFSET		102
#define ERR_BAD_SIZE		103

/*
 * Key commands
 */
#define CRYPTO_HMA_ALLOC          0x03
#define CRYPTO_HMA_FREE           0x04
#define CRYPTO_HMA_WRITE          0x01
#define CRYPTO_HMA_READ           0x02
#define CRYPTO_MEM_SYNDROME       0x2e
#define CRYPTO_HMA_COPY           0x2b
#define CRYPTO_READ_RANDOM        0x05
#define CRYPTO_GEN_RANDOM         0x06
#define CRYPTO_SHA                0x25
#define CRYPTO_SHA_INIT		  0x26
#define CRYPTO_SHA_UPDATE	  0x27
#define CRYPTO_SHA_FINAL	  0x28
#define CRYPTO_GEN_DSS_PARAMETERS 0x07
#define CRYPTO_GET_DSS_PQG	  0x30
#define CRYPTO_GEN_DSS_NUMBER	  0x09
#define CRYPTO_GEN_DSS_KEY        0x08
#define CRYPTO_GEN_DSS_SIGNATURE  0x0a
#define CRYPTO_VER_DSS_SIGNATURE  0x29
#define CRYPTO_GEN_DH_PAIR        0x0b
#define CRYPTO_GET_DH_SHARED      0x0c
#define CRYPTO_DH_ONE_TIME_PAD	  0x0e
#define CRYPTO_GET_DH_KEY	  0x0f
#define CRYPTO_GET_EPA_STATUS     0x34
#define CRYPTO_SET_EPA_STATUS     0x35
#define CRYPTO_SET_EPA_PASSWD     0x1e
#define CRYPTO_ZEROIZE_EPA        0x1f
#define CRYPTO_READ_FLASH         0x1c
#define CRYPTO_WRITE_FLASH        0x1d
#define CRYPTO_SET_CIPHER_KEY     0x20
#define CRYPTO_SET_RNG_SEED       0x21
#define CRYPTO_NEW_SESSION        0x11
#define CRYPTO_FREE_SESSION       0x12
#define CRYPTO_GET_TIME           0x2c
#define CRYPTO_GET_METHODS	  0x2d
#define CRYPTO_EPA_DIAG		  0x39
#define NUM_KEY_CMD		  60	
#define MAX_KEY_TEST		  20

extern void epad_interrupt (hwidbtype *, ushort);
extern boolean epad_send_init_block(hwidbtype *); /*rm w/ sendinit_cmd */
extern boolean epad_download(amcc_op_regs_t *); /* rm w/ download_cmd */
extern amcc_op_regs_t *epad_pci_init(uint, uint); /* rm w/ amcc cmd */
#endif __EPA_DIAGS_H__
