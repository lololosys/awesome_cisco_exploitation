/* $Id: luapi_handle.h,v 3.1.4.2 1996/08/29 02:43:47 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi_handle.h,v $
 *------------------------------------------------------------------
 * LU API control block structures and methods.
 *
 * Sept 1995, Lesley Cox
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the structures, and
 * procedures for the LU API handle control block.
 * This handle is intended to aid lookup of appropriate functions
 * for LU 0 or LU 2 or LU 6.2(future) session traffic passing.
 *------------------------------------------------------------------
 * $Log: luapi_handle.h,v $
 * Revision 3.1.4.2  1996/08/29  02:43:47  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:48  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:38  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LUAPI_HANDLE_H__
#define __LUAPI_HANDLE_H__

#include "luapi.h"

typedef LuApiReturnCode (*TluSend) (
     void*    handle,    /* luapi LU Handle     */
     uint     ruDataLen, /* Length of RU data   */
     byte*    ruData     /* RU data             */
    );

typedef LuApiReturnCode (*TluReceive) (
     void*              handle,    /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf,    /* receive buffer             */
     TReceiveIndication luRcvInd   /* rcv indication entry point */
    );

typedef LuApiReturnCode (*TluReceiveContinue) (
     void*              handle,    /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf     /* receive buffer             */
    );

typedef LuApiReturnCode (*TluClose) (
     void*  handle,   /* luapi LU Handle                          */
     byte** bufferP   /* outstanding rcv buffer, if any (returned)*/
    );


/*
 * LU API METHOD STRUCTURE
 *
 */
typedef struct LuApiMethod {
   TluSend             luSend;
   TluReceive          luReceive;
   TluReceiveContinue  luReceiveContinue;
   TluClose            luClose;
} LuApiMethod;


/*
 * LU API HANDLE STRUCTURE
 *
 */
typedef struct LuApiHandle {
   void *              specificHandle;
   const LuApiMethod * method;
} LuApiHandle;

#endif

