/* $Id: luapi_handle.c,v 3.1.4.2 1996/08/29 02:43:38 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi_handle.c,v $
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
 * $Log: luapi_handle.c,v $
 * Revision 3.1.4.2  1996/08/29  02:43:38  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:47  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:37  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "luapi.h"
#include "luapi_handle.h"


/*
 * luSend request
 *   Send request on session.
 */
LuApiReturnCode luSend (
     void*    handle,    /* luapi LU Handle     */
     uint     ruDataLen, /* Length of RU data   */
     byte*    ruData     /* RU data             */
    )
{
    LuApiHandle* luApiH;
   
    if (!handle)
        return (InvalidHandle);

    luApiH = handle;
    if (!luApiH->specificHandle)
        return (InvalidHandle);

    return (luApiH->method->luSend(luApiH->specificHandle,
                                   ruDataLen, ruData));
}


/*
 * luReceive request
 *   application ready to receive request on session.
 */
LuApiReturnCode luReceive (
     void*              handle,    /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf,    /* receive buffer             */
     TReceiveIndication luRcvInd   /* rcv indication entry point */
    )
{
    LuApiHandle* luApiH;

    if (!handle)
        return (InvalidHandle);

    luApiH = handle;
    if (!luApiH->specificHandle)
        return (InvalidHandle);

    return (luApiH->method->luReceive(luApiH->specificHandle,
                                      rcvBufLen, rcvBuf, luRcvInd));
}


/*
 * luReceiveContinue request
 *   application ready to continue to receive on session.
 */
LuApiReturnCode luReceiveContinue (
     void*              handle,    /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf     /* receive buffer             */
    )
{
    LuApiHandle* luApiH;

    if (!handle)
        return (InvalidHandle);

    luApiH = handle;
    if (!luApiH->specificHandle)
        return (InvalidHandle);

    return (luApiH->method->luReceiveContinue(luApiH->specificHandle,
                                              rcvBufLen, rcvBuf));
}


/*
 * luClose request
 *   Request session close down.
 */
LuApiReturnCode luClose (
     void*  handle,   /* luapi LU Handle                          */
     byte** bufferP   /* outstanding rcv buffer, if any (returned)*/
    )
{
    LuApiHandle* luApiH;

    if (!handle)
        return (InvalidHandle);

    luApiH = handle;
    if (!luApiH->specificHandle)
        return (InvalidHandle);

    return (luApiH->method->luClose(luApiH->specificHandle, bufferP));
}
