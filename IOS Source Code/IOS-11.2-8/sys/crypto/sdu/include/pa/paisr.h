/* $Id: paisr.h,v 1.1.68.1 1996/04/19 14:57:50 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/pa/paisr.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: paisr.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: paisr.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:50  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:54  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:14:06  bew
 * Placeholders for SDU files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
*  FileName: paisr.h			Product Name: SecureLAN
*  File STATUS:
*  	$Header: /SecureLAN/SDU/Include/pa/PAISR.H 1     10/05/94 1:08p Venkat 
*
*  Description:
*       Protocol Analysis - PA ISR module interface.
*
*  Copyright (c) Cylink Corporation 1994. All rights reserved.
**********************************************************************
*
* $History: PAISR.H 
* 
* *****************  Version 1  *****************
* User: Venkat       Date: 10/05/94   Time: 1:08p
* Created in $/SecureLAN/SDU/Include/pa
* External interface to refer to paisr.h file.
*
*********************************************************************/
# ifndef PAISR_H
# define PAISR_H

# define PRIVATE 	static
/* ---------------------------external defintions---------------------- */
extern int 	  managerRtxcSignal_g ;
extern uchar  systemMacAddress_g[6] ;
extern long   paSduProtocolUnknown_g ;
extern long   paSduL2protocols_g ;
extern IQueue iq_sys_frm ;
extern void paReceiveFrame( Rx2PaFrameControl *rxp) ;
extern ushort computeHashValue( ulong address, int protocol) ;
# endif

