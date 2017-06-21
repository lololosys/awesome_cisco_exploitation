/* $Id: stun_clslack.h,v 3.2 1995/11/17 09:24:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/stun_clslack.h,v $
 *------------------------------------------------------------------
 * Definitions for STUN over CLS-Local Acknowledgement 
 *
 * April 1995, Vipin Rawat 
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun_clslack.h,v $
 * Revision 3.2  1995/11/17  09:24:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STUNCLSLACK_H__
#define __STUNCLSLACK_H__

stunconnrtn stcls_connect (stunpeertype *stp);
void stcls_connectRet(void *pArgPointer, boolean IsError);
void stcls_err(void *pArgPointer);
void stcls_close (stunpeertype *stp, ulong param);
void stcls_abort(stunpeertype *stp);
boolean stcls_senderErr( void *arg);
boolean stcls_sender (stunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
                       boolean force, stunpeertype **outpeer,
                       boolean usethispeer, boolean toallpp);
void stcls_rddata(void* thisCep, paktype* pak);
void stcls_fini(void* cookie);
stunpeertype *stcls_findpeer (uchar group, uchar *addrp, idbtype *swidb,ushort dlci);
boolean stcls_initpeer(stunpeertype* stp, idbtype *outswidb,byte lsap);
void stcls_start(void);

#endif






