/* $Id: ep_cust.h,v 1.1.68.1 1996/04/19 14:57:37 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/ep_cust.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: ep_cust.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ep_cust.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:37  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:36  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:10  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/EP_CUST.H 6     2/21/95 6:39p Croyle 
*
* DESCRIPTION:	Entity Protection Custodian Interface
*
* NOTES:		none.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: EP_CUST.H 
* 
* *****************  Version 6  *****************
* User: Croyle       Date: 2/21/95    Time: 6:39p
* Updated in $/SecureLAN/SDU/Include
* Separated Startup Parameters used by EP Custodian to EPC specific
* variables:
* 
* ep_frm::epPEnodes->ep_cust::pet_nodes,  
* ep_frm::epCOTnodes->ep_cust::cot_nodes, 
* ep_frm::epCOThash->ep_cust::cothash_size
* 
* *****************  Version 5  *****************
* User: Croyle       Date: 1/03/95    Time: 1:48p
* Updated in $/SecureLAN/SDU/Include
* Modified naming convention for all Croyle files.
* 
* *****************  Version 4  *****************
* User: Sduhost      Date: 3/05/94    Time: 3:11p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 11/03/94   Time: 2:28p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 11/02/94   Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 1  *****************
* User: Croyle       Date: 10/13/94   Time: 10:24a
* Created in $/SecureLAN/SDU/Include
* Entity Protection Module Custodian Header.
*******************************************************************************/

#ifndef	EP_CUST_H
#define	EP_CUST_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"

/*******************************************************************************
							GLOBAL MACROS
*******************************************************************************/

/******************************************************************************
							STRUCTURE DEFINITIONS
******************************************************************************/

#ifndef EP_CUST_C

/*****************************************************************************
							 PUBLIC GLOBAL
*****************************************************************************/

extern ushort epcPeriod;				/* interval at which to run custodian */
extern ushort epcKeyCtr;				/* ctr used so far init to max # keys */
extern ushort epcKeyMin;				/* Minimum number of keys */

extern ushort pet_nodes;				/* # PET nodes allocated */
extern ushort cot_nodes;				/* # COT nodes allocated */
extern ushort cothash_size;				/* size of cot hash table */

/*****************************************************************************
						 FUNCTION PROTOTYPES
*****************************************************************************/

extern void epc_task ( void );			/* EPC task (periodically does EP hkp) */

#endif
#endif
/*------------------------- ep_cust.h END OF FILE ----------------------------- */
