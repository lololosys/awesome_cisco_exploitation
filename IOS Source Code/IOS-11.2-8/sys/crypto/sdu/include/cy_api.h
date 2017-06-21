/* $Id: cy_api.h,v 1.1.68.1 1996/04/19 14:57:34 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/cy_api.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: cy_api.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cy_api.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:33  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:07  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CY_API.H 27    8/21/95 11:44a Peacock 
*
* DESCRIPTION:	Crypto Function API Header File
*
* NOTES:		This file contains info related to calling the CCC functions
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CY_API.H 
* 
* *****************  Version 27  *****************
* User: Peacock      Date: 8/21/95    Time: 11:44a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 26  *****************
* User: Peacock      Date: 8/10/95    Time: 11:40a
* Updated in $/SecureLAN/SDU/Include
* Added ReadRTClock() called  by IO_MGR into task layer.  Moved CCC fifo
* setup routine to single vrersion.
* 
* *****************  Version 25  *****************
* User: Tchi         Date: 7/19/95    Time: 2:38p
* Updated in $/SecureLAN/SDU/Include
* added declaration of hMemSyndrome 
* 
* *****************  Version 24  *****************
* User: Croyle       Date: 6/19/95    Time: 9:27a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 23  *****************
* User: Tchi         Date: 5/26/95    Time: 10:00a
* Updated in $/SecureLAN/SDU/Include
* added BLK_NOACCESS (vs, tc)
* 
* *****************  Version 20  *****************
* User: Sduhost      Date: 4/17/95    Time: 10:20a
* Updated in $/SecureLAN/SDU/Include
* Added multiplication and addition TK functions
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 4/04/95    Time: 3:06p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 17  *****************
* User: Croyle       Date: 3/18/95    Time: 8:07p
* Updated in $/SecureLAN/SDU/Include
* New Functionality added for Alpha Testing
* 
* *****************  Version 16  *****************
* User: Sduhost      Date: 3/05/95    Time: 2:07p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 15  *****************
* User: Sduhost      Date: 3/05/95    Time: 11:40a
* Updated in $/SecureLAN/SDU/Include
* Added new error code
* 
* *****************  Version 14  *****************
* User: Sduhost      Date: 3/04/95    Time: 6:45p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 12  *****************
* User: Croyle       Date: 3/03/95    Time: 4:22p
* Updated in $/SecureLAN/SDU/Include
* 1st release of crypto interface
* 
* *****************  Version 10  *****************
* User: Croyle       Date: 2/27/95    Time: 4:42p
* Updated in $/SecureLAN/SDU/Include
* 1st version of working frame processing
* 
* *****************  Version 9  *****************
* User: Croyle       Date: 2/17/95    Time: 10:51a
* Updated in $/SecureLAN/SDU/Include
* Couple quick changes
* 
* *****************  Version 8  *****************
* User: Croyle       Date: 2/17/95    Time: 10:31a
* Updated in $/SecureLAN/SDU/Include
* New Crypto API prototypes now available
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 2/10/95    Time: 11:04a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 6  *****************
* User: Bald_eagle   Date: 2/10/95    Time: 10:35a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 5  *****************
* User: Bald_eagle   Date: 2/10/95    Time: 10:12a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 2/10/95    Time: 8:30a
* Updated in $/SecureLAN/SDU/Include
* Updated error codes
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 1/19/95    Time: 9:08a
* Updated in $/SecureLAN/SDU/Include
* Integration #2 Update
*******************************************************************************/

#ifndef	CY_API_H
#define	CY_API_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"

/*******************************************************************************
							GLOBAL MACROS
*******************************************************************************/

/* Miscellaneous definitions */

#define DES_KEY_SIZE		8
#define IV_KEY_SIZE			8
/*#define SM_IV_BASE_INDEX	0   <--- NOT SURE WHY THIS WAS DEFINED HERE... */
/*									 ALSO DEFINED IN AUTHxxx.h file */
#ifndef BLK_WRITE
#define BLK_NOACCESS	0x00			/* USR: No read or write */
#define BLK_PERM		0x04			/* USR: Should be Permanent */
#define BLK_CSUM		0x08			/* USR: Maintain a checksum on parm */
#define BLK_WRITE		0x10			/* USR: Allow writes of this memory */
#define BLK_READ		0x20			/* USR: Allow reads of this memory */
#endif

/* Crypto function return codes */

#define CYER_IFRESET		6			/* h/w and s/w interface had to be reset */
#define CYER_MEMXFR			5			/* problem with memory xfr interface */
#define CYER_UNAVAIL   	 	4			/* toolkit resource unavailable */
#define CYER_UNDEF		 	3			/* function not yet defined */
#define CYER_BADSTAT	 	2			/* CCC status (e.g.,tampered,bad memory) */
#define CYER_TIMEOUT	 	1			/* CCC not responding */

/* Crypto toolkit return codes */
/* NOTE:  most sig bit MUST be zero! (due to signalling mechanism) */

#define SUCCESS				 0				/* success acronym used by toolkit */
#define CY_OKAY				 0				/* result = success (else see below) */
#define CYER_DATA			-1				/* generic data error */
#define CYER_ALLOC			-2				/* insufficient memory */
#define CYER_INPUT_LEN		-3				/* bad length for input data (0bytes) */
#define CYER_DSS_LEN		-4				/* bad length for dss_p */
#define CYER_DH_LEN			-5				/* bad length for DH_modulus */
#define CYER_RSA_LEN		-6				/* bad length for rsa_n */
#define CYER_BLOCK_LEN		-7				/* bad length for ECB&CBC encrypt mode blocks */
#define CYER_HASH_LEN		-8				/* bad length for hash_result */
#define CYER_MODE			-9				/* bad value of encryption mode */
#define CYER_NUMBER			-10				/* bad number of testings (zero) */
#define CYER_POSITION		-11				/* bad value of triplet_position */
#define CYER_COUNT			-12				/* bad iteration count (zero) */
#define CYER_SIGNATURE		-21				/* bad signature */
#define CYER_PRIME			-22				/* number is not a prime */
#define CYER_WEAK       	-23     		/* weak key 							 */
#define CYER_INPUT_VALUE	-24     		/* invalid input value 						 */
#define CYER_ACCESS			-25				/* access error (not allowed) */
#define CYER_SIZECLAMP		-26				/* size clamped, too big (hma read/writes) */
#define CYER_FLASHFAIL		-27				/* flash operation failed */
#define CYER_DESFAIL		-28				/* DES chip failed during test */

/* CCC status bits */

#define CCC_TAMPER			0x80000000uL	/* CCC has been tampered */
#define CCC_DES1FAIL		0x00008000uL	/* DES #1 chip failure */
#define CCC_DES2FAIL		0x00004000uL	/* DES #2 chip failure */
#define CCC_EXP1FAIL		0x00002000uL	/* 1024 #1 chip failure */
#define CCC_EXP2FAIL		0x00001000uL	/* 1024 #2 chip failure */
#define CCC_RTCFAIL			0x00010000uL	/* Real Time clock has stopped */
#define CCC_INIT			0x00000001uL	/* CCC is currently initializing */

/******************************************************************************
							STRUCTURE DEFINITIONS
******************************************************************************/

#ifndef CY_API_C

#ifndef TOOLKIT_H

/* Secure Hash Algorithm structure */

typedef struct
{
	ulong state[5];							/* state */
	ulong count[2 ];						/* number of bits */
	uchar buffer[64];						/* input buffer */
}	SHA_context;

#endif

/*****************************************************************************
								 PUBLIC GLOBAL
*****************************************************************************/

/*=============================== MIB GLOBALS ================================= */

/*-------------------------- CY-API Error Counters ---------------------------- */

extern	ulong	cyErrNoSema;					/* No free semaphores */
extern	ulong	cyErrNoResp;					/* No response from CCC (timeout) */

/*=============================== SYSTEM GLOBALS ============================== */

extern volatile long	tk_req; 				/* toolkit request global */
extern volatile long	tk_adr;		  			/* address of toolkit request parameters */
extern volatile short	tk_size;		  		/* size of current toolkit request */
extern volatile short	tk_status;	  			/* status of last toolkit request */

extern unsigned long	ccc_status;				/* CCC status bit flags */
extern unsigned long	ccc_time;				/* CCC real-time clock value (in sec) */

/*****************************************************************************
								 FUNCTION PROTOTYPES
*****************************************************************************/

/*--------------------------- Cryptographic API ------------------------------- */

/* hidden memory support functions */

extern int hMemWrite ( short hdl, short size, void* dat );
extern int hMemCopy  ( short dst, short src,  short size );
extern int hMemRead  ( short hdl, short size, void* dat );
extern int hMemSyndrome (short hdl, short ofs, short size, short len, void* dat );
extern int hMemAlloc ( short* i,  short size, uchar atb );
extern int hMemFree  ( short hdl );

/* flash memory support */

extern int WriteCyFlash ( short ofs, short size, void* dat);
extern int ReadCyFlash  ( short ofs, short size, void* dat);

/* random number generator support */

extern int hReadRandom ( char* ptr,    short size );
extern int hGenRandom  ( short rand_i, short size );

/* DSS support functions (for verify see below) */

extern int hGenDSSparameters ( short size, uchar* p, uchar* q, uchar* g );
extern int hGenDSSkey (short size, uchar* p, uchar* q, uchar* g, short x_i, uchar *y);
extern int hGenDSSnumber (short key_i,uchar *mod );
extern int hGenDSSsignature (short size, uchar* p, uchar* q, uchar* g, short num_i, uchar* r, uchar* s, uchar* hash );

/* diffie-hellman support functions */

extern int hGenDHpair( short mod_n, short secret_i, uchar* pub, uchar* base, uchar* mod );
extern int hGetDHsharedNumber(short mod_n, short secret_i, uchar* pub, short shared_i, uchar* mod );
extern int hSetDESKAPPAkey (short mod_n, short shared_i, short offset, short key_i);
extern int hDHoneTimePad (short mod_n, short shared_i, uchar *x, uchar *y);
extern int hGetDHkey ( short mod_n, short secret_i, uchar* pub, short key1_n,short key1_i, short key2_n, short key2_i, uchar* mod );

/* des crypto functions */

extern int hDESkeyExpand (short  key_i,short expkey_i);
extern int hDESencryptIV_1 (uchar* iv, short key_i, short mode, uchar* buf, short size);
extern int hDESdecryptIV_1 (uchar* iv, short key_i, short mode, uchar* buf, short size);
extern int hDESencryptIVval_1 (short iv_i, short key_i, short mode, uchar* buf, short size);
extern int hDESdecryptIVval_1 (short  iv_i, short key_i, short mode, uchar* buf, short size);
extern int hDESencryptIVgen_1 (uchar* iv,  short iv_i, short key_i, short mode, uchar* buf, short size );
extern int hDESdecryptIVgen_1 (uchar* iv, short iv_i, short key_i, short mode, uchar* buf, short size );

/* kappa crypto functions */

extern int hKAPPAkeyExpand (short key_i,short expkey_i);
extern int hKAPPAencryptIVval_1( short iv_i,short key_i,short mode, uchar* buf, short buf_n);
extern int hKAPPAdecryptIVval_1 (short iv_i,short key_i,short mode, uchar* buf, short size);
extern int hKAPPAencryptIVgen_1 (uchar *iv, short iv_i, short key_i,short mode, uchar* buf, short size );
extern int hKAPPAdecryptIVgen_1 (uchar* iv, short iv_i, short key_i,short mode, uchar* buf, short size );

/* shamir support functions */

extern int hGenShamirTriplet (short size, short secret_i, short val1_i, short val2_i, short val3_i );
extern int hGetNumberShamirDuplex (short size, short secret_i, short val_a_i, short trip_a_pos, short val_b_i, short trip_b_pos );

/* miscellaneous functions */

extern int ReadRTClock( ulong* time );
extern int GetRTClock ( ulong* time );
extern int SetRTClock ( ushort sec, ushort min, ushort hr, ushort day, ushort mon, ushort yr);
extern int GetCCCstatus ( ulong* status );

/* extended math functions */

extern int	hExponent(ushort base_n, short base_i, ushort exp_n, short exp_i, ushort mod_n, short mod_i, void* result);
extern int	hInverse (ushort data_n, short data_i, short mod_n, void* mod, void* result);
extern int	hMul(ushort x_n, short x_i, short z_n, short z_i, ushort n_n, short n_i, void* y);
extern int	hAdd(ushort x_n, short x_i, short z_n, short z_i, ushort n_n, short n_i, void* y);

/* SHA functions */

#ifndef TOOLKIT_H
extern int SHA ( uchar* message, ushort message_bytes, uchar  *hash_result );
extern int SHAInit  ( void* hash_context );
extern int SHAUpdate( void* hash_context, uchar* message, ushort message_bytes );
extern int SHAFinal ( void* hash_context, uchar* hash_result );
extern int VerDSSSignature (ushort p_bytes, uchar* p, uchar* q, uchar* g, uchar* y, uchar* r, uchar* s, uchar* result);
#endif

#ifdef INTGR_DEBUG
extern int				lastCyErr;			/* last Cy API error code */
extern short			lastCyErrFunc;		/* the function that caues last Cy API error */
extern ushort			cyErrCount;			/* total number of Cy API errors */
#endif

#endif
#endif
/*------------------------- cy-api.h END OF FILE ------------------------------ */

