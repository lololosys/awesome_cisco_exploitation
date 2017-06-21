/* $Id: cy_frm.h,v 1.1.68.1 1996/04/19 14:57:34 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/cy_frm.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: cy_frm.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cy_frm.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:34  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:08  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CY_FRM.H 6     7/12/95 4:01p Sduhost 
*
* DESCRIPTION:	Cryptographic Module Header File
*
* NOTES:		This file contains only info related to Cy frame handler
*				and ISR.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CY_FRM.H 
* 
* *****************  Version 6  *****************
* User: Sduhost      Date: 7/12/95    Time: 4:01p
* Updated in $/SecureLAN/SDU/Include
* Added queue size variable
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 1/19/95    Time: 9:08a
* Updated in $/SecureLAN/SDU/Include
* Integration #2 Update
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 12/16/94   Time: 10:37a
* Updated in $/SecureLAN/SDU/Include
* Modified Array Declarations to use TYPDEF'd array structure (allows
* sizeof() to be used for size determination).
* 
* *****************  Version 1  *****************
* User: Croyle       Date: 10/19/94   Time: 4:39p
* Created in $/SecureLAN/SDU/Include
* Initial Release of Crypto module
*******************************************************************************/

#ifndef	CY_FRM_H
#define	CY_FRM_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/iqueue.h"
#include "../crypto/sdu/include/pa/pactl.h"

/*******************************************************************************
							GLOBAL MACROS
*******************************************************************************/

#define NUM_METHODS		32			/* number of encryption methods supported */
#define NUM_PROTOCOLS	32			/* number of protocols supported */

/******************************************************************************
							STRUCTURE DEFINITIONS
******************************************************************************/

typedef ushort cyMethod[NUM_METHODS];	/* Method table consists of 32 ushort entries */

typedef struct							/* parameters for encrypting packet */
{
	ushort	ivgen_ofs;					/* iv generation value offset (relative to L3 hdr) */
	ushort	ivgen_siz;					/* iv generation value size */
	ushort	skip;						/* begin crypting #bytes after Transport layer */
}	cyConfig[NUM_PROTOCOLS];

#ifndef CY_FRM_C

/*****************************************************************************
								 PUBLIC GLOBAL
*****************************************************************************/

/*=============================== MIB GLOBALS ================================= */

/*--------------------- Required Configuration Parameters --------------------- */

extern  cyMethod	cyMethodTbl;		/* Crypto method table */
extern	cyConfig	cyConfigTbl;		/* init values per protocol stack */
extern  ushort		cyMode;				/* cryption mode (ECB:1-bit,CFB:8-bit) */

/*---------------------------- Cy Status Info --------------------------------- */

extern  ulong		cyReject;			/* # frames rejected */
extern  ulong		cyPass;				/* # frames passed in clear */
extern  ulong		cyDecrypt;	 		/* # frames decrypted */
extern  ulong		cyEncrypt;	 		/* # frames encrypted */

/*---------------------------- CY Error Counters ------------------------------ */

extern  ulong		cyErrBadMode;		/* # frames with unknown frameModes */
extern  ulong		cyErrNoMethod;		/* # no Method defined */
extern  ulong		cyErrNoFrame;		/* cy_isr() awoken with no frame ptr */

/*=============================== SYSTEM GLOBALS ============================== */

extern	IQueue		iq_cy_frm;			/* Crypto frame passing queue */
extern	short		iq_cy_num;			/* number of entries in queue */

/*****************************************************************************
								 FUNCTION PROTOTYPES
*****************************************************************************/

extern void cryp_frame ( Pa2EpFrameControl *f );  /* prepares frame for encryption */
extern void pass_frame ( Pa2EpFrameControl *f );  /* pass frame to transmitter */
extern void toss_frame ( Pa2EpFrameControl *f );  /* deallocate frame (don't tx) */
extern void cryp_config( Pa2EpFrameControl *f );  /* configs PCI i/f for crypto */

#endif
#endif
/*------------------------- cy_frm.h END OF FILE ------------------------------ */

