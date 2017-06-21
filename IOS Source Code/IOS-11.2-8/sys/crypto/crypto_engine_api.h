/* $Id: crypto_engine_api.h,v 3.2.62.18 1996/09/10 14:00:39 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine_api.h,v $
 *------------------------------------------------------------------
 * Type and Const. defintion file for crypto engine api.
 * Old file name: epa_api.h, revision 2.1.6.7.
 *
 * 10, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_api.h,v $
 * Revision 3.2.62.18  1996/09/10  14:00:39  sbelair
 * CSCdi67685:  Unable to fast-switch packets through encrypted tunnels
 * Branch: California_branch
 *
 * Define new switch mode to be used ater decryption for tunnel
 * fast-switching,
 * and define a new crypto vector to be invoked for decryption in tunnel
 * fast-
 * switching which will use the new switch mode.
 *
 * Revision 3.2.62.17  1996/09/01  00:12:25  xliu
 * CSCdi67790:  The trailing bytes are not encrypted correctly.
 * Branch: California_branch
 *
 * Revision 3.2.62.16  1996/08/27  19:45:42  snijsure
 * CSCdi66755:  Mib object cieEsaMode is not updated to show current
 * status of ESA.
 * Branch: California_branch
 * Extract status of ESA and copy it into cieESaMode.
 *
 * Revision 3.2.62.15  1996/08/27  06:30:50  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Incorporate comments from doc. review, as well as some syntax clean up.
 *
 * Revision 3.2.62.14  1996/08/15  07:55:09  xliu
 * CSCdi65871:  Wrong password to crypto clear-latch causes hang
 * Branch: California_branch
 * Set wathed boolean to be TRUE when the crypto card returned with a error
 * status.
 *
 * Revision 3.2.62.13  1996/08/07  09:21:06  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 3.2.62.12  1996/07/28  07:41:51  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.2.62.11  1996/07/22  18:53:49  bew
 * CSCdi62404:  password is not enabled when generating DSS key for EPA.
 * Branch: California_branch
 * Add password to gen-signature-keys when the target is an EPA.
 *
 * Revision 3.2.62.10  1996/07/17  18:31:10  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 3.2.62.9  1996/07/15  10:04:59  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.62.8  1996/07/06  19:44:23  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.62.7  1996/07/03  05:54:28  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 3.2.62.6  1996/06/11  18:19:23  xliu
 * CSCdi60131:  add CRYPTO_CFB8_SHIFT_COUNT constant to crypto_engine_api.h
 * Branch: California_branch
 *
 * Revision 3.2.62.5  1996/06/01  01:14:15  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/05/28  15:58:54  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/05/12  23:12:13  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/04/23  21:53:40  bew
 * CSCdi55458:  fix usage of slot when registering crypto vectors
 * Branch: California_branch
 * Fixed a bad deference of null pointer when applying a badly named
 * crypto map to an interface, and fixed a problem where the slot
 * was used instead of a crypto engine identifier.
 *
 * Revision 3.2.62.1  1996/04/19  14:55:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.14  1996/04/17  23:47:16  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.2.13  1996/03/30  05:11:16  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 2.1.2.12  1996/03/25  18:43:37  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.2.11  1996/03/21  10:07:01  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 2.1.2.10  1996/03/01  01:38:53  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.2.9  1996/01/30  22:35:44  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 2.1.2.8  1996/01/19  07:00:36  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 2.1.2.7  1996/01/04  18:47:48  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.2.6  1995/12/10  09:33:13  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.2.5  1995/11/30  05:55:36  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the neww crypto engine code.
 *
 * Revision 2.1.2.4  1995/11/28  01:46:25  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.3  1995/10/13  08:30:50  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser command. Add debug parser command.
 *
 * Revision 2.1.2.2  1995/10/06  17:29:38  xliu
 * Branch: IosSec_branch
 * Fixed naming bug.
 *
 * Revision 2.1.2.1  1995/10/04  22:18:22  xliu
 * Branch: IosSec_branch
 * Update crypto api command name.
 *
 * Revision 2.1  1995/10/04  22:12:01  xliu
 * Placeholder file for crypto_engine_api.h.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef EPA_API_H
#define EPA_API_H

#include "../iprouting/igrp.h"
#include "../ipmulticast/igmp.h"
#include "../ip/gre.h"
#include "../if/rif.h"
#include "../dlsw/dlsw_dlx.h"
#include "../rsvp/rsvp_objects.h"

#define EPA_CRYPTO_UNIT_NOT_DEFINED(id) (id)>1000? 1: 0 
#define EPA_TEST_DATA_LENGTH            4096 
#define CRYPTO_ENGINE_ON_RP_RSP         100
#define CRYPTO_ENGINE_NOT_PRESENT       200
#define CRYPTO_ENGINE_ON_VIP            1
#define CRYPTO_CONNTBL_SIZE             300   /* the size of the conn table */

#define CRYPTO_VALID_SIGNATURE                1
#define CRYPTO_INVALID_SIGNATURE              2

#define CRYPTO_FAST_SWITCH     		0x0001
#define CRYPTO_PROCESS_SWITCH  		0x0002
#define CRYPTO_RSP_FLOW_SWITCH 		0x0004
#define CRYPTO_VIP_FAST_SWITCH 		0x0008 
#define CRYPTO_TUNNEL_FAST_SWITCH	0x0010 

#define CRYPTO_PLATFORM_UNKNOWN 0x00
#define CRYPTO_PLATFORM_RP     0x01
#define CRYPTO_PLATFORM_RSP    0x02
#define CRYPTO_PLATFORM_VIP    0x03

#define CRYPTO_ENCRYPT_BLOCK_LEN  8
#define DH_LENGTH_MIN   64

#define CRYPTO_ENGINE_NAME_MAX_SIZE    50

#define CRYPTO_ENGINE_NOT_INSTALLED    0x00
#define CRYPTO_ENGINE_INSTALLED        0x01
#define CRYPTO_ENGINE_DSS_KEY_GEN      0x02

#define CRYPTO_VIP_SERIAL_SIZE         20

#define CRYPTO_PAK_OPTION_SIZE         20

#define CRYPTO_CHALLENGE_SIZE          20
#define CRYPTO_DSS_SIG_SIZE            40


/* Constants for crypto function unit type */
#define    CRYPTO_TYPE_UNINIT         0
#define    CRYPTO_TYPE_UNKNOWN        1  
#define    CRYPTO_SWEPA               2
#define    CRYPTO_HWEPA               3

#define CRYPTO_EPA_PASSWD_MAX_SIZE     50

/*
 * Constants for IV
 */
#define CRYPTO_TCP_IV_SIZE      8
#define CRYPTO_UDP_IV_SIZE      2
#define CRYPTO_IP_IV_SIZE       2
#define CRYPTO_DEFAULT_PROT       32
#define CRYPTO_TCP_PROT           31
#define CRYPTO_UDP_PROT           30
#define CRYPTO_DEFAULT_IV_OFFSET 2
#define CRYPTO_MAX_IV_SIZE      8

/* DES alg constats */
#define    CRYPTO_CFB8_SHIFT_COUNT   7

typedef enum epa_op_status_ {
    EPA_OP_OK,
    EPA_OP_FAIL,
    EPA_INVALID_INPUT,
} epa_status;

#define EPA_TIMEOUT    100

typedef enum epa_crypto_name_ {
    CRYPTO_INIT,
    CRYPTO_GET_ENGINE_INFO,
    CRYPTO_ZEROIZE,
    CRYPTO_CREATE_SESSION_KEY,
    CRYPTO_CLEAR_CONN_ENTRY,
    CRYPTO_ENCRYPT_PACKET,
    CRYPTO_DECRYPT_PACKET,
    CRYPTO_GEN_ALG_PARAM,
    CRYPTO_GEN_RANDOM_NUMBER,
    CRYPTO_GEN_KEY_PAIR,
    CRYPTO_SIGN,
    CRYPTO_VERIFY,
    CRYPTO_GEN_DH_ALG_PHASE1_PARAM,
    CRYPTO_GEN_DH_ALG_PHASE2_PARAM,
    CRYPTO_GEN_DSS_KEY_PAIR,
    CRYPTO_EXP,
    CRYPTO_VIP_VERIFY,
    CRYPTO_VIP_SIGN,
    CRYPTO_SET_EPA_PRIV_MODE,
    CRYPTO_INIT_RANDOM,
    CRYPTO_ENGINE_VIP_GET_CFG,
    CRYPTO_GET_EPA_STATUS,
    CRYPTO_ENGINE_VIP_REG_EPA,
    CRYPTO_ENGINE_VIP_SEND_TBL,
    CRYPTO_CONFIG_EPA,
    CRYPTO_CHECK_EPA_PASSWD,
    CRYPTO_CLEAR_LATCH,
    CRYPTO_GEN_SYNDROME,
    CRYPTO_CLEAR_DH_NUMBER,
} epa_crypto_command;

/*
 * error code
 */
#define CRYPTO_ENGINE_OP_SUCCESS                0x0000
#define CRYPTO_ENGINE_OP_FAIL                   0x8000
#define CRYPTO_ENGINE_GEN_DSS_KEY_FAIL          0x0001
#define CRYPTO_ENGINE_DSS_SIGN_FAIL             0x0002
#define CRYPTO_ENGINE_DSS_VER_FAIL              0x0003
#define CRYPTO_ENGINE_SHA_FAIL                  0x0004
#define CRYPTO_ENGINE_GEN_DH_PAIR_FAIL          0x0005
#define CRYPTO_ENGINE_GEN_SESSION_KEY_FAIL      0x0006
#define CRYPTO_ENGINE_GEN_IV_KEY_FAIL           0x0007
#define CRYPTO_ENGINE_INVALID_ALG_TYPE          0x0008
#define CRYPTO_ENGINE_INVALID_INPUT             0x0010
#define CRYPTO_ENGINE_MEM_ALLOC_FAIL            0x0011
#define CRYPTO_ENGINE_KEY_REQ_TIMEOUT           0x0012
#define CRYPTO_ENGINE_IPC_FAIL                  0x0013
#define CRYPTO_ENGINE_CLEAR_CONN_FAIL           0x0014
#define CRYPTO_ENGINE_ENCRYPT_QUEUE_FULL        0x0015
#define CRYPTO_ENGINE_ENCRYPT_DEQUEUE_FAIL      0x0016

/* Crypto card error code */
#define ESA_DSS_SIGNATURE_ERROR                 21
#define ESA_INPUT_INVALID                       24
#define ESA_BAD_PASSWORD                        29
#define ESA_LATCH_SET                           30

/* Key command timeout constants  */
#define CRYPTO_DSS_KEY_GEN_TIMEOUT              20000
#define CRYPTO_KEY_COMM_TIMEOUT                 20000
#define CRYPTO_SIGN_TIMEOUT                     20000

/* 
 * Constants and types for the cryptograph algorithms 
 */

#define CRYPTO_DH_PUBLIC_NUMBER_SIZE    64
#define CRYPTO_DSS_P_MAX_SIZE           128
#define CRYPTO_DSS_Q_MAX_SIZE           128
#define CRYPTO_DSS_G_MAX_SIZE           128

typedef enum alg_types_ {
    DH_ALG_PHASE1,       /* alg to generate DH numbers */
    DH_ALG_PHASE2,       /* alg to generate the shared secret num */
    DES_ECB,             /* des with ECB mode */
    DES_CBC_IV1,         
    DES_CBC_IV2,
    DES_CBC_IV3,
    DES_CFB8_IV1,
    DES_CFB64_IV1,
    DES_CFB64_IV2,
    DES_CFB64_IV3,
    DES_40_CFB8_IV1,	     /* DES 40 bit key */
    DES_40_CFB64_IV1,	     /* DES 40 bit key */
    KAPPA_ECB,               /* kappa with ECB mode */
    KAPPA_CBC_IV1,
    KAPPA_CBC_IV2,
    KAPPA_CBC_IV3,
    KAPPA_CFB_IV1,
    KAPPA_CFB_IV2,
    KAPPA_CFB_IV3,
    RSA_KEY_GEN,
    DSS_KEY_GEN,
    DSS_SHA_SIGN,
    DSS_SHA_VERIFY,
    SHA_DIGEST,
} alg_types;

/* DH algorithm phase1 parameters */
struct DH_alg_phase1_ {
    char *      prime_modulus;
    char *      base_modulus;
    ushort      size_of_prime;
    ushort      size_of_base;
};

/* DH algorithm phase2 parameter */
struct DH_alg_phase2_ {
    uchar      peer_public_number[DH_LENGTH_MIN];
};

struct DH_alg_ {
    union {
        struct DH_alg_phase1_    dh_alg_phase1;
        struct DH_alg_phase2_    dh_alg_phase2;
    } phase_tag;
    ushort    phase;
};

/* DSS key pair parameters */
struct DSS_KEY_GEN_alg_ {
    char*     dss_p;
    char*     dss_q;
    char*     dss_g;
    ushort    p_size;     
};

struct RSA_KEY_GEN_alg_ { 
    /* not used for the current release */
};

struct random_number_alg_ {
    ushort	random_number_size;
};

typedef struct {
    union {
        struct DH_alg_phase1_      dh_alg_phase1;
        struct DH_alg_phase2_      dh_alg_phase2;
        struct DSS_KEY_GEN_alg_    dss_key_gen_alg;
        struct RSA_KEY_GEN_alg_    rsa_key_gen_alg;
        struct random_number_alg_  random_number_alg;
    } type_tag;
    ushort     algorithm_type;
} epa_alg_obj;

/*
 * Constants and types for the keys 
 */

#define CRYPTO_DSS_PUBLIC_KEY_MAX_SIZE    128  /* 64 byte is used now */
#define CRYPTO_DSS_PRIVATE_KEY_MAX_SIZE   128  /* 20 byte is used now */
#define CRYPTO_RSA_PUBLIC_KEY_MAX_SIZE    128  /* unknown */
#define CRYPTO_RSA_PRIVATE_KEY_MAX_SIZE   128  /* unknown */
#define CRYPTO_RANDOM_MAX_SIZE            128  /* ? */
#define CRYTPO_DSS_SIG_SIZE               40   /* <r, s> */

#define CRYPTO_ENCRYPTION_QUEUE_LEN         8

typedef enum key_types_ {
	DSS_PUBLIC_KEY,
	RSA_PUBLIC_KEY,
        DSS_KEY_PAIR,
        RSA_KEY_PAIR,
} key_types;

struct DSS_PUBLIC_KEY_ {
   uchar*    key_value;
   ushort    size;
};

struct RSA_PUBLIC_KEY_ {
   uchar*    key_value;
   ushort    size;
};

typedef struct {
    ushort   pub_key_size;
    ushort   pri_key_size;
    uchar    pub_key[CRYPTO_DSS_PUBLIC_KEY_MAX_SIZE];
    uchar    pri_key[CRYPTO_DSS_PRIVATE_KEY_MAX_SIZE];
} crypto_dss_keys;

typedef struct {
    ushort   pub_key_size;
    ushort   pri_key_size;
    uchar    pub_key[CRYPTO_RSA_PUBLIC_KEY_MAX_SIZE];
    uchar    pri_key[CRYPTO_RSA_PRIVATE_KEY_MAX_SIZE];
} crypto_rsa_keys;

typedef struct {
    union {
        struct  DSS_PUBLIC_KEY_ dss_public_key;
        struct  RSA_PUBLIC_KEY_ rsa_public_key;
        crypto_dss_keys         dss_keys;
        crypto_rsa_keys         rsa_keys;
    } key_tag;
    ushort  key_type;
} epa_key_obj;

/* Definiton of session identification. */ 

typedef struct {
    ushort connection_id;
    ushort epa_id;
} epa_session_id;

/* Crypto engine configuration structure */
typedef struct crypto_engine_config_t_ {
    ulong	 serial;           
    ushort	 id;               /* crypto engine id */
    uchar*       name;
    ushort       state;
    ushort	 crypto_unit_type; /* sw or hw */
    ushort	 crypto_unit_ver;  /* for hwepa */
    ulong	 time_start;       /* time when crypto engine start */
    ushort       version;
    ushort       platform;         /* RP/RSP/VIP */
    hwidbtype*   vip_hwidb;
    ulong        epa_status;
} crypto_engine_config_t;

/*
 * epa_pak_info
 * Information returned through callback about the enc/dec packet.
 * Currently this is not used.
 */
typedef struct epa_pak_info_ {
    /* not finialized */
    uchar*    data_buffer;
    ushort    start_offset;
    ushort    data_length;
} epa_pak_info;

/* crypto_fs_pak_info
 * 
 * The packet info needed to fastswitch a packet after software encryption.
 * It is used by the crypto engine on both RP and RSP when sending out
 * a fastswitched packet.
 *
 */
typedef struct {
    int       switch_mode;
    iphdrtype *ip;
    hwidbtype *hwidb;
    paktype   *pak;
}crypto_fs_pak_info;

/*
 * crypto_ps_pak_info
 *
 * The packet info needed to switch a packet after software encryption.
 * Is is used by the crypto engine for the packets switched at process 
 * level.
 */
typedef struct {
    int       switch_mode;
    iphdrtype *ip;
    hwidbtype *hwidb;
    paktype   *pak;
    boolean   disposeflag;
    boolean   log;
} crypto_ps_pak_info;

/* 
 * crypto_pak_info
 * The buffer for packet output info on RP/RSP/VIP.
 */
typedef struct {
    int       switch_mode;
    iphdrtype *ip;
    hwidbtype *hwidb;
    paktype   *pak;
    uchar     others[CRYPTO_PAK_OPTION_SIZE];
} crypto_pak_info;

/*
 * typedef for the api callback and context.
 */

typedef struct {
    /* src_epa_id: the id of the epa engine  which         */
    /* send the request.                                   */
    /* When this id is o, assume the local epa engine is   */
    /* choosen.                                            */
    ushort src_epa_id;

    /* dst_epa_id: the id of the epa engine which should   */
    /* accept the output.                                  */
    /* If this id is 0, assume the local epa engine is choosen. */
    ushort dst_epa_id;

    /* destination epa engine callback handle name.         */ 
    /* If this is NULL, use the callback handle as passed   */
    /* through the api.                                     */
    unsigned char *dst_callback_handle_name;

    /* "timeout" defines the peorid the process can wait. */
    /* When "timeout" is set as 0, EPAEngin will serve the*/
    /* request in blocking mode.                          */
    ulong    req_timeout; 

    /* Pointer to the paktype buffer. The buffer should be*/
    /* requested and returned by the process.             */
    paktype    *pak;

    /* Pointer to the data buffer as needed by the process*/
    /* This gives the process the addressability to its   */
    /* ushorternal data which may be needed to process the   */
    /* callback.                                          */
    /* This buffer should be req/returned by the process  */
    void    *proc_data_buffer;

} epa_callback_context;

typedef void (*epa_callback_handle) (epa_callback_context *context,
				     ushort event_name,
                                     ushort op_status,
                                     void   *rsp_buffer,
                                     ushort rsp_buffer_size,
                                     struct epa_pak_info_ *pak_info);

/*
 * where
 *
 * epa_name:   
 * event_name interprets the meaning of the response.
 * For example, event_name RANDOM_NUMBER_GENERATED  
 * indicates that the data carried in the rsp_buffer
 * should be ushorterpreted as a random number.       

 * epa_op_status:
 * epa_op_status indicate the result of the corresponding  
 * request. For example, OK indicates the request  
 * has been succesful.                            

 * epa_rsp_buffer:
 * Pointer to the data buffer used by the EPAEngin to 
 * send back the response. This buffer is requested  
 * and returned by the EPAEngin.                    

 * epa_rsp_buffer_size:
 * the size of epa_rsp_buffer.    

 * epa_pak_info:
 * "pak_info" has the information about the encryption
 * decryption operation applied on the packet pointed
 * by *pak.  
 */

/*
 * Definition of the information returned in the callback rsp buffer. 
 */
typedef struct {
    ushort    id;
} epa_rtn_info;    
 
typedef struct { 
    uchar     number[CRYPTO_DH_PUBLIC_NUMBER_SIZE]; 
} epa_rtn_DH_pub_number; 

typedef struct { 
    uchar     DSS_P[CRYPTO_DSS_P_MAX_SIZE]; 
    uchar     DSS_Q[CRYPTO_DSS_Q_MAX_SIZE];
    uchar     DSS_G[CRYPTO_DSS_G_MAX_SIZE];
    uchar     pad;
} epa_rtn_DSS_number; 
 
typedef struct { 
    union { 
        epa_rtn_DH_pub_number   dh_pub; 
        epa_rtn_DSS_number      dss_number;    
    } alg_tag; 
    ushort   alg_type; 
} epa_rtn_alg_param; 


typedef struct {
    uchar    syndrome[CRYPTO_CHALLENGE_SIZE];
} epa_rtn_syndrome;

typedef struct {
    uchar    random_number[CRYPTO_RANDOM_MAX_SIZE];
    ushort   size;
} epa_rtn_random_number;

typedef struct {
    union {
        crypto_dss_keys         dss_keys;
        crypto_rsa_keys         rsa_keys;
    } key_tag;
    ushort  alg_type;
    uint    serial_number;
    int	    slot;
    ulong   status;
} epa_rtn_sig_key;

typedef struct {
    int      slot;
    ulong    status;
} epa_rtn_get_status;

typedef struct {
    uchar    sig[CRYPTO_DSS_SIG_SIZE];
} epa_rtn_sig;

/* 
 * Constatns and type for iv generation. 
 */

#define CRYPTO_NUM_PROTOCOLS 33

typedef struct {            /* parameters for encrypting packet */
    ushort  ivgen_ofs;      /* iv generation value offset(relative to L3 hdr) */
    ushort  ivgen_siz;      /* iv generation value size */
    ushort  skip;           /* begin crypting #bytes after Transport layer */
}   cylink_cyConfig[CRYPTO_NUM_PROTOCOLS];

/* Cylink crypto lib encrption method constants Don't change! */
#define CYLINK_DES40_CFB8       0x01
#define CYLINK_DES40_CFB64      0x02
#define CYLINK_DES56_CFB8       0x0A
#define CYLINK_DES56_CFB64      0x0B

/* 
 * Translation between Cisco encryption method and Cylink encryption 
 * method.
 */
static inline ushort get_cylink_encrypt_method (ushort method)
{
    switch (method) {
    case DES_CFB64_IV1:
    default:
        return CYLINK_DES56_CFB64;
    case DES_CFB8_IV1:
        return CYLINK_DES56_CFB8;
    case DES_40_CFB64_IV1:
        return CYLINK_DES40_CFB64;
    case DES_40_CFB8_IV1:
        return CYLINK_DES40_CFB8;
    }
}

static inline char * xlate_cylink_encrypt_method (ushort alg)
{
    static char str[20];
 
    switch (alg) {
        case CYLINK_DES56_CFB8:
            bcopy("DES_56_CFB8", str, 11);
            str[11] = '\0';
            break;
        case CYLINK_DES56_CFB64:
            bcopy("DES_56_CFB64", str, 12);
            str[12] = '\0';
            break;
        case CYLINK_DES40_CFB8:
            bcopy("DES_40_CFB8", str, 11);
            str[11] = '\0';
            break;
        case CYLINK_DES40_CFB64:
            bcopy("DES_40_CFB64", str, 12);
            str[12] = '\0';
            break;
        default:
            bcopy("UNKNOWN", str, 7);
            str[7] = '\0';
            break;
    }
    return(str);
}
 

/*
 * epa status macros 
 */
#define EPA_TAMPERED(x) ((x) & 0x80000000)
#define EPA_XTRACTED(x) ((x) & 0x40000000)
#define EPA_PASSWD_SET(x) ((x) & 0x08000000)
#define EPA_DSS_SET(x) ((x) & 0x00080000) 
#define EPA_BOOT_LED(x) ((x) & 0x00000080) 
#define EPA_NORMAL_LED(x) ((x) & 0x00000040) 
#define EPA_ERROR_LED(x) ((x) & 0x00000020) 
#define EPA_TAMPER_LED(x) ((x) & 0x00000010) 

/* egp.h does not define a constant for its header. The
 * CRYPTO_EGP_HDR_SIZE is defined according to egp fixed header
 * defintion, given in Internetworking with TCP/IP, Vol1, p229.
 * OSPF header is defined in p257.
 */
#define CRYPTO_EGP_HDR_SIZE   10
#define CRYPTO_OSPF_HDR_SIZE  24

extern cylink_cyConfig crypto_iv_gen_tbl;
extern ushort crypto_engine_rpsp_slot;
extern ushort crypto_engine_platform;
extern crypto_engine_config_t crypto_engine_tbl[];

/*
 * Crypto API function prototypes
 */

/* only the epa op status is returned */
epa_status 
epa_init(ushort,               /* crypto engine id */
         epa_callback_context *, 
         epa_callback_handle );

/* only the epa op status is returned */
epa_status 
epa_zeroize (ushort,        /*  crypto engine id */
             epa_callback_context*,
             epa_callback_handle);

/* return type: epa_status  */
epa_status  
epa_create_session_key(ushort,               /* crypto engine id */
                       epa_alg_obj *,        /* encryption algorithm */
                       short,            /* conn id */
                       epa_callback_context*,
                       epa_callback_handle);

/* return type: epa_status  */
epa_status 
epa_clear_connection_entry (ushort,          /* crypto engine id */
                            ushort,          /* connection id  */
                            epa_callback_context *,
                            epa_callback_handle);
/*
 * CryptoGraphic Operation
 */

/* return type:  epa_pak_info */
epa_status 
epa_encrypt_packet (uchar *,                /* data to be encrypt */
                    ushort,                 /* encrypt length       */
                    short,                  /* connection id        */ 
                    uchar * ,               /* iv                   */
                    ushort,
                    crypto_pak_info*);

/* return type:  epa_pak_info */
epa_status 
epa_decrypt_packet (uchar *,               /* data to be decrypt */
                    ushort,                /* decrypt length       */
                    short,                 /* connection id        */
                    uchar* ,               /* iv                   */
                    ushort,
                    crypto_pak_info*);

/* return type:  */
epa_status 
epa_generate_alg_parameter (ushort,         /* crypto engine id */
                            epa_alg_obj *,  /* algorthm  */
                            ushort,         /* the conn id */
                            epa_callback_context *,
                            epa_callback_handle );

/* the random_number is returned in epa_rsp_buffer */                        
epa_status 
epa_generate_random_number(ushort,          /* crypto engine id */
                           epa_alg_obj *,   /* algorithm  */
                           epa_callback_context *,
                           epa_callback_handle );


/* return type: epa_rtn_public_key */
epa_status 
epa_generate_key_pair (ushort,              /* crypto engine id */
                       epa_alg_obj *,       /* algorithm     */
		       uchar *,		    /* passwd */
		       ushort,		    /* passwd_size */
                       epa_callback_context *,
                       epa_callback_handle );

/* return type: epa_status */
epa_status 
epa_sign (ushort,                           /* crypto engine id */
          char*,                            /* data to be signed    */
          ushort,                           /* the length of data   */
          epa_alg_obj*,                     /* algorithm object     */
          epa_callback_context *,
          epa_callback_handle );

/* return type: an interger is returned through epa_status  */
epa_status 
epa_verify (ushort,                         /* crypto engine id */
            char *,                         /* hashed data to be verified */
            ushort,                         /* hashed data length         */
            char *,                         /* signature                  */
            ushort,                         /* signature length           */
            epa_alg_obj *,                  /* algorithm                  */
            epa_key_obj *,                  /* public key                 */
            epa_callback_context *,
            epa_callback_handle );

 
epa_status
epa_generate_syndrome(ushort,              /* crypto engine id */
                     ushort,                 /* conn id */
                     ushort,                 /* challenge_length */
                     ushort,                /* offset */
                     epa_callback_context*,
                     epa_callback_handle);


void crypto_engine_show_conntbl (ushort,     /* engine id */
                                ushort,     /* start */
                                ushort);    /* end */
void crypto_engine_show_all(void);

void crypto_engine_show_statis(ushort);    /* engine id */
void crypto_engine_show_active_conn(ushort);    /* engine id */
void crypto_engine_show_dead_conn(ushort);    /* engine id */
void crypto_engine_show_drop_pak(void);
void crypto_engine_show_config(void);
void crypto_init_vip_dss_keys(ushort);
void crypto_vip_map_head_add(void *head);
void crypto_vip_map_add(char *);
void crypto_vip_map_delete(char *);
void crypto_vip_map_head_delete(uchar *);
void crypto_vip_assign_map_to_int(hwidbtype *, void*);
void crypto_vip_map_set_peupe(char *map);
void crypto_vip_map_set_modeflags(char *map);
void crypto_vip_upd_map_conn_id (char *, int);
void crypto_engine_command(parseinfo *);
void crypto_engine_restore_media_vectors(hwidbtype *, void *);
void crypto_config_epa(ushort, uchar*, ushort);
void crypto_show_card_status(ushort);
char *crypto_get_tk_version(void);
void crypto_test_des(ushort);
void crypto_restore_media_vector(void *);
int crypto_clear_dh_numbers(int, int);
void crypto_test_des_with_input(parseinfo *);
void crypto_nist_runtests(parseinfo *);

epa_status crypto_create_digest (epa_alg_obj *,
                                 uchar*, /* message */
                                 ushort, /* message length */
                                 uchar*  /* digest */);

void crypto_engine_init_dss_keys(void);

epa_status crypto_config_epa_card (ushort,   /* crypto_engine_id */
                                   uchar *,  /* passwd */
                                   ushort,   /* passwd_size */
                       		   epa_callback_context *,
                       		   epa_callback_handle);

epa_status crypto_set_card_status (ushort, /* slot */
				   ulong   /* status */);

epa_status crypto_get_card_status (ushort, /*  slot */
				   ulong*  /* status */);


extern uchar crypto_ran_num[];

epa_status crypto_engine_gen_dh_pair(uchar*, uchar*, uchar*);

#define IS_ENCRYPTION(x)((x)->status & IDB_ENCRYPT)

static inline ushort crypto_get_engine_id (ushort slot)
{
    ushort engine_id;

    if (crypto_number_of_slots == 1) {
	engine_id = crypto_engine_rpsp_slot;
    } else {
        engine_id = crypto_engine_tbl[slot].id;
    }

    return engine_id;
}

/*
 * crypto_get_iv_seed
 * 
 * Retrieve IV seed value from the packet. Later, the seed value is
 * to be XORed with the iv gen key to create 8 byte iv (for swepa). 
 */
static inline void crypto_get_iv_seed (iphdrtype *ip, uchar *iv_seed, 
                                       ushort* size)
{
    uchar *start;
    ushort index;

    /*
     * If the fragment flag is turned on, we assume there is no
     * valid L4 header. So use the default iv.
     */

    if (get_ip_morefragments(ip)) {
        index = CRYPTO_DEFAULT_PROT;
        start = (uchar *)ip + CRYPTO_DEFAULT_IV_OFFSET;
    } else {
        switch (ip->prot) {
        case TCP_PROT:
            index = CRYPTO_TCP_PROT;
            start = (uchar*)ip + ipheadsize(ip) + 
                             crypto_iv_gen_tbl[index].ivgen_ofs;
            break;
        case UDP_PROT:
            index = CRYPTO_UDP_PROT;
            start = (uchar*)ip + ipheadsize(ip) + 
                             crypto_iv_gen_tbl[index].ivgen_ofs;
            break;
        default:
            index = CRYPTO_DEFAULT_PROT;
            start = (uchar *)ip + CRYPTO_DEFAULT_IV_OFFSET;
            break;
        }
    }

    bcopy (start, iv_seed, crypto_iv_gen_tbl[index].ivgen_siz);
    *size = crypto_iv_gen_tbl[index].ivgen_siz;
}

static inline int crypto_count_sre (uchar *sres, uchar *max_len)
{
    sretype *sre = (sretype *)sres;
    int counter = 0;

    while ((sres < max_len) && (sre->length != 0 ) && 
           (sre->address_family !=0)) {
        counter++;
        sres += sizeof(sretype);
    }
    return counter;
}

static inline void crypto_set_encrypt_start (iphdrtype *ip, int *offset)
{
    tcptype *tcp;
    ushort tl;

    switch (ip->prot) {
    case TCP_PROT:
        tcp = (tcptype *)((uchar *)ip + ipheadsize(ip));
        *offset =  ipheadsize(ip) + (tcp->dataoffset <<2);
        break;
    case UDP_PROT:
        *offset = ipheadsize(ip) + crypto_iv_gen_tbl[CRYPTO_UDP_PROT].skip;
        break;
    case ICMP_PROT:
        *offset = ipheadsize(ip) + ICMPHEADERBYTES;
        break;
    case IGMP_PROT:
        *offset = ipheadsize(ip) + IGMPHEADERBYTES;
        break;
    case IGRP_PROT:
    case NEWIGRP_PROT:
        *offset = ipheadsize(ip) + IGRPHEADERBYTES;
        break;
    case GRE_PROT:
        {
            gretype *gre = (gretype *)((uchar *)ip + ipheadsize(ip));
            ushort gre_hdr_len = 0, sre_num = 0;
            uchar *ptr;

            if (GETSHORT(gre) == 0) {
                *offset = ipheadsize(ip) + MINGREHEADERBYTES;
            } else {
                gre_hdr_len = MINGREHEADERBYTES;
                ptr = (uchar *)gre + MINGREHEADERBYTES;

                if (gre->have_checksum) {
                    gre_hdr_len += 2;
                    ptr += 2;
                }
                if (gre->have_routing) {
                    gre_hdr_len += 2;
                    ptr += 2;
                }

                if (gre->have_key) {
                    gre_hdr_len += 4;
                    ptr += 4;
                }

                if (gre->have_sequence) {
                    gre_hdr_len += 4;
                    ptr += 4;
                }

                if (gre->have_routing) {
                    sre_num = crypto_count_sre(ptr, (uchar *)ip + ip->tl);
                    gre_hdr_len += sizeof(sretype) * sre_num;
                }

                *offset = ipheadsize(ip) + gre_hdr_len;
            }
        }
        break;
    case FST_DLSW_PROT:
        *offset = ipheadsize(ip) + DLX_HDR_SZ;
        break;
    case OSPF_PROT:
        *offset = ipheadsize(ip) + CRYPTO_OSPF_HDR_SIZE;
        break;
    case EGP_PROT:
        /* egp.h does not define a constant for its header. The
         * CRYPTO_EGP_HDR_SIZE is defined according to egp fixed header
         * defintion, given in Internetworking with TCP/IP, Vol1, p229.
         */
        *offset = ipheadsize(ip) + CRYPTO_EGP_HDR_SIZE;
        break;
    case RSVP_PROT:
        *offset = ipheadsize(ip) + sizeof(rsvp_header);
        break;
    default:
        *offset = ipheadsize(ip) + crypto_iv_gen_tbl[CRYPTO_DEFAULT_PROT].skip;
        break;
    }

    /*
     * Make sure that the offset is inside the packet- in cases of
     * an incomplete level4 header the offset will be greater than
     * ip->tl and we could end up encrypting a very large number of
     * bytes when none should be encrypted. This is taken care of by
     * trimming offset to ip->tl if it is greater. See CSCdi91591.
     *
     * Access ip->tl in such a way as to not generate an alignment
     * error on particle platforms. (ip is not always aligned
     * on a word boundary and so ip->tl will generate an 
     * alignment error). Do a safe read of one word and just 
     * mask off two bytes.
     */
    tl = GETLONG(ip) & 0xFFFF;
    *offset = min(tl, *offset);
}

#endif

