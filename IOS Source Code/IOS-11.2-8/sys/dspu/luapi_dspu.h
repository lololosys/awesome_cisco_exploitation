/* $Id: luapi_dspu.h,v 3.1.4.2 1996/08/29 02:43:42 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi_dspu.h,v $
 *------------------------------------------------------------------
 * LU API DSPU methods.
 *
 * Sept 1995, Lesley Cox
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the interface which DSPU_LU.C uses to 
 * communicate with LUAPI_DSPU.C.
 *------------------------------------------------------------------
 * $Log: luapi_dspu.h,v $
 * Revision 3.1.4.2  1996/08/29  02:43:42  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:46  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:36  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LUAPI_DSPU_H__
#define __LUAPI_DSPU_H__

#include "snaprim.h"

typedef struct LuApi LuApi;


/*
 * LuApiReceiveSscpLu
 *
 *   This is called to a pass data packet destined for an LU API LU
 *   on the SSCP-LU session.
 *
 */
void LuApiReceiveSscpLu(
       Lu*             this,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data          */
       SNAMU*          snaMu
       );


/*
 * LuApiReceiveLuLu
 *
 *   This is called to a pass data packet destined for an LU API LU
 *   on the LU-LU session.
 *
 */
void LuApiReceiveLuLu(
       Lu*             this,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data      */
       SNAMU*          snaMu
       );


/*
 * LuApiReceiveLuNmvtPsidRsp
 *
 *   This is called to a pass a response to an NMVT PSID
 *   request sent by the LU.  The response is passed so that the LU
 *   may detect +ve vs -ve response and sense code, but it is expected
 *   to be finished with the buffer on return and the PU will free it.
 *
 */
void LuApiReceiveLuNmvtPsidRsp( Lu* this, DataReq_t* clsiBfrP );


/*
 * LuApiResetLu
 *
 *   Drives the SSCP-LU FSM for hierachical reset
 *
 */
void LuApiResetLu( Lu*  this );


/*
 * LuApiLuDelete
 *
 *   This is the Lu destructor called when the upstream pu is destructed
 *   (from Pu_dl through LuPuChildDelete)
 */
void LuApiLuDelete( Lu* this );


/*
 * LuApiDoWork
 *
 *   This checks the work bits in order.
 * If the work bit is on and buffers are available it sends the
 * appropriate command and turns the bit off.
 * TRUE is returned if the work was done or no buffers were available.
 * FALSE is returned if no LU work bit was set.
 * The enum workResult is set to indicate if 
 * work was done, work was not done or no buffers were available.
 * 
 * Note: This function returns a boolean since it is called by
 *       DoLuWorkUp within QueryAllChildrenParm.
 *
 */
boolean LuApiDoWork( Lu* this, WorkResult* workResult );

/*
 * LuApiShowLu
 *
 *   This displays the LU details.
 *
 */
void LuApiShowLu( Lu* this );


/*
 * LuApiQueryLuActive
 *
 *   Returns TRUE if the LU is active.
 *
 */
boolean LuApiQueryLuActive( Lu* this );


/*
 * LuApiQueryLuBound
 *
 *   Returns TRUE if the LU is bound.
 *
 */
boolean LuApiQueryLuBound( Lu* this );

#endif
