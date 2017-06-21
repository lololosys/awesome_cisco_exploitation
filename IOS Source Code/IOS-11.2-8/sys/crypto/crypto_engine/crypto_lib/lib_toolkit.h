/* $Id: lib_toolkit.h,v 3.2.60.2 1996/05/28 15:59:41 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/crypto_lib/lib_toolkit.h,v $
 *------------------------------------------------------------------
 * Cylink lib type & const defintion file.
 * Old file nname: toolkit.h, revision: 2.1.2.1.
 *
 * 10, 1995, xlliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lib_toolkit.h,v $
 * Revision 3.2.60.2  1996/05/28  15:59:41  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:57:01  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.5  1996/03/25  18:44:00  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.4  1996/01/19  07:00:57  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.3  1996/01/04  18:54:26  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.2  1995/11/30  01:32:53  xliu
 * Branch: IosSec_branch
 * Integated with the new Cylink crypto lib source doe.
 *
 * Revision 1.1.2.1  1995/10/03  18:49:20  xliu
 * Branch: IosSec_branch
 *
 * Moved the old epa dir under crypto. Also renamed the files.
 *
 * Revision 1.1  1995/10/03  00:49:02  xliu
 * Placeholder files for crypto_lib dir.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/****************************************************************************
*  FILENAME:  toolkit.h           PRODUCT NAME: CRYPTOGRAPHIC TOOLKIT
*
*  FILE STATUS:
*
*  DESCRIPTION:     Cryptographic Toolkit Functions Header File
*
*  USAGE:	    File should be included to use Toolkit Functions
*
*
*  	Copyright (c) Cylink Corporation 1994. All rights reserved.
*
*  REVISION  HISTORY:
*
*  23 Aug 94	KPZ	Initial release
*  24 Sep 94    KPZ     Added prototypes of Toolkit functions
*  14 Oct 94    GKL     Second version (big endian support)
*
****************************************************************************/

#ifndef TOOLKIT_H     /* Prevent multiple inclusions of same header file */
#define TOOLKIT_H

typedef int (* YIELD_PROC)( int ); /*TKL00601*/

typedef struct {                   /*TKL00601*/
	YIELD_PROC yield_proc;
        void *     handle;         /* Application specific information */
}YIELD_context;

/* Error types */

#define SUCCESS	 0	/* no errors */
#define ERR_DATA	-1 	/* generic data error */
#define ERR_ALLOC	-2	/* insufficient memory */
#define ERR_INPUT_LEN	-3	/* invalid length for input data (zero bytes) */
#define ERR_DSS_LEN	-4	/* invalid length for dss_p */
#define ERR_DH_LEN      -5      /* invalid length for DH_modulus */
#define ERR_BLOCK_LEN	-7	/* invalid length for input block for ECB/CBC */
#define ERR_HASH_LEN	-8	/* invalid length for hash_result */
#define ERR_MODE	-9	/* invalid value of encryption mode */
#define ERR_NUMBER	-10	/* invalid number of testings (zero) */
#define ERR_POSITION	-11	/* invalid value of  triplet_position	*/
#define ERR_COUNT	-12	/* invalid iteration count (zero) */
#define ERR_SIGNATURE	-21	/* signature is not valid */
#define ERR_PRIME       -22     /* number is not prime */
#define ERR_WEAK        -23     /* weak key */
#define ERR_INPUT_VALUE -24     /* invalid input value */
#define ERR_CANCEL      -30     /* canceled by user */


/* Lengths of variables */
#define DSS_LENGTH_MIN   64  /* 512-bit minimal length for DSS functions */
#define DSS_LENGTH_MAX  128  /* 1024-bit maximal length for DSS functions */
#define SHA_LENGTH       20  /* 160-bit length for SHA hash result */
#define DH_LENGTH_MIN    64  /* the dh length */

/* Number of randomly choose base for Miller test */
#define TEST_COUNT              40

/* Encryption modes */
#define ECB_MODE	0  /* Elecronic CodeBook */
#define CBC_MODE	1  /* Cipher Block Chaining */
#define CFB1_MODE	2  /* 1-bit Cipher FeedBack */
#define CFB8_MODE	3  /* 8-bit Cipher FeedBack */
#define CFB64_MODE      4  /* 64-bit Cipher FeedBack */

/* Key generation offset (copied from connstup.h) */
#define SKEY_OFFSET     0
#define IKEY_OFFSET     32

/****************************************************************************
*  INCLUDE FILES
****************************************************************************/

/* system files */
#include "lib_cylink.h"

/* Secure Hash Algorithm structure */
typedef struct
{
	ulong state[ 5 ];       /* state */
	ulong count[ 2 ];       /* number of bits */
	uchar buffer[ 64 ];     /* input buffer */
} SHA_context;

#ifdef  __cplusplus
extern  "C" {
#endif

/* Compute a Secure Hash Function */
int SHAY( uchar   *message, ushort message_bytes,
	  uchar  *hash_result , YIELD_context *yield);

/* Initialize Secure Hash Function */
void SHAInit( SHA_context *hash_context );

/* Update Secure Hash Function */
int SHAUpdateY( SHA_context *hash_context,
	       uchar        *message,
	       ushort      message_bytes,
               YIELD_context *yield);

/* Update Secure Hash Function */
int SHAUpdate( SHA_context *hash_context,
	       uchar        *message,
	       ushort      message_bytes);

/* Finalize Secure Hash Function */
int SHAFinal( SHA_context *hash_context,
			  uchar       *hash_result );

/* Compute a DSS Signature */
int GenDSSSignatureY( ushort dss_p_bytes, uchar  *dss_p,
		      uchar  *dss_q,      uchar  *dss_g,
		      uchar  *dss_x,      uchar  *dss_k,
		      uchar  *r,          uchar  *s,
		      uchar  *hash_result,
                      YIELD_context * );

/* Verify a DSS Signature */
int VerDSSSignatureY( ushort dss_p_bytes, uchar  *dss_p,
		      uchar  *dss_q,      uchar  *dss_g,
		      uchar  *dss_y,      uchar  *r,
		      uchar  *s,          uchar  *hash_result,
                      YIELD_context *);

/* Initialize Random number Generator */
int InitRand( ushort SEED_bytes, uchar  *SEED,
			  uchar  *RVAL );

/* Generate random number */
int GenRand( ushort A_bytes, uchar  *A,
			 uchar  *RVAL );

/* Compute DSS public/secret number pair */
int GenDSSKeyY( ushort dss_p_bytes, uchar  *dss_p,
		       uchar  *dss_q,      uchar  *dss_g,
		       uchar  *dss_x,      uchar  *dss_y,
		       uchar  *XKEY, YIELD_context*); /* TKL00601*/

/* Generate secret number */
int GenDSSNumber( uchar *dss_k, uchar *dss_q,
				  uchar *KKEY );



/* Generate DSS Common Parameters */
int GenDSSParametersY( ushort dss_p_bytes, uchar  *dss_p,
					  uchar  *dss_q,      uchar  *dss_g,
					  uchar  *RVAL, YIELD_context *yield_cont ); /*TKL00701*/

/* Generate DH pair */
int GenDHPairY(ushort, uchar *, uchar *, uchar *, uchar *, uchar *,
               YIELD_context *);

/* Get DH keys, including both sesion key and iv key. */
int GetDHKeyY(ushort, uchar *, uchar *, ushort, uchar*,ushort,uchar*,uchar*,
              YIELD_context *); 

/* Get DH shared number */
int GetDHSharedNumberY(ushort, uchar*, uchar *,uchar*, uchar*,YIELD_context*);

int SetDESKAPPAKey(ushort, uchar *, uchar *);
int SetDESKAPPAKey_40(ushort, uchar *, uchar *);
int SetCipherKey(ushort, uchar *, uchar *, ushort);

#endif /* TOOLKIT_H */
