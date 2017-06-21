/* $Id: dspu_lu.h,v 3.2.60.2 1996/06/21 02:22:02 slogie Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_lu.h,v $
 *------------------------------------------------------------------
 * DSPU LU control block structures and methods.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This header file contains the finite state machines, structures, and
 * procedures for the LU control block.
 *------------------------------------------------------------------
 * $Log: dspu_lu.h,v $
 * Revision 3.2.60.2  1996/06/21  02:22:02  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.2.60.1  1996/05/17  10:47:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/22  18:50:23  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2  1995/11/17  09:04:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  17:03:03  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __lu_h
#define __lu_h

#include "dtypes.h"
#include "snaprim.h"
#include "luapi_dspu.h"


typedef enum { 
          LuActFailRsn_None,
          LuActFailRsn_Other,
          LuActFailRsn_NegativeRsp, 
        } LuActFailRsnE;  

/***********************************************************************
* SSCP-LU                                                      
************************************************************************
*
* The following are the SSCP-LU enumerations and routine headers.
*   
**/

/*=====================================================================*
* Sscp Lu Sub State flag
*======================================================================*
*
* To make the sscp state simple, there is some flags
**/

/* Set at uStopLu or uDactlu and clear at uActlu. This flag is used during 
* pending state that no state changed until a response is received.
*/
#define UpRqstInactive   1

/* Set at dStopLu and clear at dStartLu. This flag is used during pending
* state that no state changed until a response is received.
*/
#define DownRqstInactive 2

/* Downstream can either send an Actlu response with lu available or a
* Notify request with lu available. In both cases they have the same 
* state. This flag is used to indicate that a Notify response is required
* in response of Notify request.
*/
#define OweNtfyRsp       4

/* In trace that when lu is activated and deactivated that a message is
* required to send to the user. There is a common entry point for the lu
* to be activated but there is no a common entry point to indicate the lu
* is deactiavted. This flag is used to indicate if a deactivation message
* is to be displayed.
*/
#define LuActivated      8



/* At DoLuWork, the data can be sent upstream(Notify) or downstream(d/actlu)
*. At timeout or a response, the PuFreeBuffer will be called which needs to
* know whether to acknowledge up or down PU.
*/
#define UpstreamAcknowledge 0x10

/* Record that a LuNmvtPsig is to be sent to the upstream pu. This bit is
* set when Actpu is received with dddlu capability. It then signal to
* upstream pu that an nmvt psig is to be sent. When the pu is ready to
* send, it will ask the lu to send it and this bit will be cleared
*/

#define LuNmvtPsig 0x20

/* Record that a work buffer has been used by the SscpLu session.
*  Only one work buffer is used per SscpLu session at any time, and
*  it is always freed before using another work buffer.  In this way
*  we guarantee that we never "forget" to free the buffer, and hence
*  create a "leakage" from the activation-window.
*/

#define SscpLuWorkBufferInUse 0x40

/* bit mask for flow control */
#define UpFlowOff   1
#define DownFlowOff 2


/*=====================================================================*
* LuUsageType 
*======================================================================*
*
* Description:
*   LU usage - configured to DSPU or allocated to LU API.
*   Determines which FSMs are used to manage the LU.   
*   
**/
typedef enum LuUsageType {
             luInUseByDspu, 
             luInUseByLuApi
            } LuUsageType;

/***********************************************************************
* LU STRUCTURE                                                      
************************************************************************
*
* The following is the LU structure.
*   
**/
struct Lu {
   struct Lu*  next;               /* lu pool chaining     */
   Pu*         upPuP;             /* upstream Pu parent   */
   Pu*         downPuP;           /* downstream Pu parent */ 
   Itimer *    timer;
   Itimer*     luluTimer;         /* for lu-lu fsm */
   PoolCB*     poolP;             /* either a Pool Lu or dedicated lu */
   DataReq_t*  saveMsgAtPending;
   byte        sscpLuState;       /* sscp lu state        */
   byte        luLuState;         /* lu lu state          */
   hword       upFramesReceive;
   hword       downFramesReceive;
   hword       sequenceNumber;    /* sequence number of notify request */
   byte        upLuNumber;        /* upstream lu number   */
   byte        downLuNumber;      /* downstream lu number */
   byte        luActivityTimer;   /* lu activity timer            */
   byte        subState;          /* each flag is explained above */
   byte        luWorkBits;
   byte        flowControl;
   byte        debugLevel;
   LuActFailRsnE lastActFailRsn;
   LuUsageType usage;             /* DSPU config'd or alloc'd to LUAPI */
   LuApi*      luApiP;            /* LU API CB */
};

/***********************************************************************
* LU CONSTRUCTORS                                                      
************************************************************************
*
* The following LU constructors headers.
*   
**/

/*=====================================================================*
* Lu_nw
*======================================================================*
*
* Description:
*   Lu constructor for Pool lu. It is called at configuration time. 
*     POOL poolName HOST hostName LU luStart luRange or
*
*   The lu defined only have the upstream connection. Downstream connection
* is called by LuConnect.
*   
**/
Lu* Lu_nw(Pu*     puP,        /* upstream PU pointer */
          hword   luNumber,    /* lu number           */
          PoolCB* poolP);     /* pool                */

/*=====================================================================*
* LuHost_nw
*======================================================================*
*
* Description:
*   Lu constructor for dedicated lu. It is called at configuration time. 
*     LU luStart luRange HOST hostName hostLuStart
*
*   The lu defined will have both upstream and downstream connection
**/
Lu* LuHost_nw(Pu*   hostPuP,
              hword hostLuStart,
              Pu*   puP,
              hword lu
             );

/*=====================================================================*
* LU destructor: LuDelete
*======================================================================*
*
* Description:
*   Lu destructor called by cfgtr and pool LU code and LUAPI LuApi_dl. 
*   Lu CB only deleted, c.f. LuPuChild which can delete LUAPI LUs too.
**/
void LuDelete(Lu* this);


/*=====================================================================*
* LU destructor: LuPuChildDelete
*======================================================================*
*
* Description:
*   Lu destructor called by upstream/downstream PU. 
*   Deletes DSPU and LUAPI LUs appropriately.
**/
void LuPuChildDelete(Lu* this);


/***********************************************************************
* LU UTILITIES                                                      
************************************************************************
*
* The following routines are the LU utilities.
*   
**/

/*=====================================================================*
* ReceiveSscpLuUndef
*======================================================================*
*
* Description:
*   This is called by the pu to a pass data packet destined for an LU
*   on the SSCP-LU session.  The LU object is not defined meaning that
*   it is an LU that has not been allocated to DSPU or LU API.
*
**/
void ReceiveSscpLuUndef(
       Pu*             puP,
       byte            luNumber,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data          */
       SNAMU*          snaMu
                );

/*=====================================================================*
* ReceiveLuLuUndef
*======================================================================*
*
* Description:
*   This is called by the pu to a pass data packet destined for an LU
*   on the LU-LU session.  The LU object is not defined meaning that
*   it is an LU that has not been allocated to DSPU or LU API.
*
**/
void ReceiveLuLuUndef(
       Pu*             puP,
       DataReq_t*      clsiBfrP,       /* clsi buffer for data          */
       SNAMU*          snaMu
              );

/*=====================================================================*
* ReceiveSscpLu
*======================================================================*
*
* Description:
*   This is called by the pu to a pass data packet destined for an LU
*   on the SSCP-LU session - the LU is defined meaning that it is in
*   use by DSPU or LU API.
*
**/
void ReceiveSscpLu(
       Lu*             this,
       LinkType        puType,         /* upstream or downstream    */
       DataReq_t*      clsiBfrP,       /* clsi buffer for data          */
       SNAMU*          snaMu
       );

/*=====================================================================*
* ReceiveLuLu
*======================================================================*
*
* Description:
*   This is called by the pu to a pass data packet destined for an LU.
*
**/
void ReceiveLuLu(
       Lu*             this,
       LinkType        puType,         /* upstream or downstream    */
       DataReq_t*      clsiBfrP,       /* clsi buffer for data      */
       SNAMU*          snaMu
       );

 
/*=====================================================================*
* ReceiveLuNmvtPsidRsp
*======================================================================*
*
* Description:
*   This is called by the pu to a pass a response to an NMVT PSID
*   request sent by the LU.  The response is passed so that the LU
*   may detect +ve vs -ve response and sense code, but it is expected
*   to be finished with the buffer on return and the PU will free it.
*
**/
void ReceiveLuNmvtPsidRsp (Lu*        this,
                           DataReq_t* clsiBfrP
                          );

 
/*=====================================================================*
* LuConnect
*======================================================================*
*
* Description:
*    During lu construction, lu can be a dedicated connection as which is
* constructed by LuHost_nw. Or it it a pool lu which is constructed by
* Lu_nw. In this case, the lu is only a child of upstream pu. 
* When a dowmstream PU becomes active, it will find a lu without downstream
* connection, then calls this routine to make the lu connection.
*
**/
ConnectStatus LuConnect(Lu* this, 
                        Pu*   puP,            /* downstream PU pointer */
                        hword luNumber        /* downstream lu number  */
                       );

/*=====================================================================*
* LuDisconnect
*======================================================================*
*
* Description:
*    Disconnect Lu from down stream PU.
*    
*/
void LuDisconnect(Lu* this);

/*=====================================================================*
* QueryLuActive
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is active.
**/
boolean QueryLuActive(Lu* this);

/*=====================================================================*
* QueryLuActiveUpOrDown
*======================================================================*
*
* Description:
*   Returns TRUE if the SSCP-LU session is active.
**/
boolean QueryLuActiveUpOrDown(Lu* this, LinkType upOrDown);

/*=====================================================================*
* QueryLuActiveAndDedicated
*======================================================================*
*
* Description:
*   Returns TRUE if the SSCP-LU session is active
*   and the LU is dedicated.
*
**/
boolean QueryLuActiveAndDedicated(Lu* this);

/*=====================================================================*
* QueryLuFree
*======================================================================*
*
* Description:
*    Check if lu is free.
*
* boolean  
* QueryLuFree(Lu* this);
**/

#define QueryLuFree(luP) (luP->downPuP == 0)

#define NextLu(luP) (luP->next)

/***********************************************************************
* LU-LU                                                      
************************************************************************
*
* The following are the LU-LU enumerations and routine headers.
*   
**/


/*=====================================================================*
* UResetLu, DResetLu
*======================================================================*
*
* Description:
*   Drives the SSCP-LU FSM for hierachical reset
*
**/
void UResetLu(Lu* this);
void DResetLu(Lu* this);



/*----- LU Work Bits -----*/   
#define WORK_DACTLU        0x01 
#define WORK_ACTLU         0x02 
#define WORK_NOTIFYAVAIL   0x08 
#define WORK_NOTIFYUNAVAIL 0x10 
#define WORK_UNBIND        0x20
#define WORK_LUAPI         0x40



void LogDddlu(Lu* this);

/*=====================================================================*
* StartLu
*======================================================================*
*
* Description:
*   Drives the SSCP-LU FSM
*
**/
void StartLu(Lu* this);



/*=====================================================================*
* LogForLuWork
*======================================================================*
*
* Description:
*   This procedure is called whenever the LU wants to send DACTLU, ACTLU,  
* NotifyAvail, NotifyNotAvail or UNBIND; because it must ask
* permission of the global buffer management to get a buffer.
*   It turns on the appropriate bit in the LU work bits, and calls 
* LogForPuWork(LuWork) in the appropriate PU parent (DACTLU and ACTLU
* are downstream, and the others upstream).
*
**/
void LogForLuWork
       (Lu  *luP,
       byte luWork
       );

/*=====================================================================*
* DoLuWorkDown
*======================================================================*
*
* Description:
*   This checks the DOWNSTREAM LU work bits in order. If the work bit is on  
* and buffers are available it sends the appropriate command, 
* turns the bit off, starts a timer. TRUE is returned if the work
* was done or no buffers were available. FALSE is returned if no
* LU work bit was set. The enum workResult is set to indicate if 
* work was done, work was not done or no buffers were available.
* 
* Note: This function returns a boolean since it is called within
*       QueryAllChildrenParm.
*
**/
boolean                   /* FALSE if there is no work to be done */
DoLuWorkDown
    (Lu *this, 
     WorkResult* result   /* result of the operation */
    );

/*=====================================================================*
* DoLuWorkUp
*======================================================================*
*
* Description:
*   This checks the UPSTREAM LU work bits in order. If the work bit is on  
* and buffers are available it sends the appropriate command, 
* turns the bit off, starts a timer. TRUE is returned if the work
* was done or no buffers were available. FALSE is returned if no
* LU work bit was set. The enum workResult is set to indicate if 
* work was done, work was not done or no buffers were available.
* 
* Note: This function returns a boolean since it is called within
*       QueryAllChildrenParm.
*
**/
boolean                   /* FALSE if there is no work to be done */
DoLuWorkUp
    (Lu *this, 
     WorkResult* result   /* result of the operation */
    );
/*=====================================================================*
* LuActivityTimeout
*======================================================================*
*
* Description:
*   This is called by the Pool periodically when the pool inactivity
* timeout.
*   
**/

void LuActivityTimeout(Lu* this, byte timeoutValue);

/*=====================================================================*
* QueryLuSendNmvt
*======================================================================*
*
* Description:
*   This routine is called by the upstream pu that he is ready to send
* an nmvt if there is one to send
**/

boolean QueryLuSendNmvt(Lu* this);


void LuFlowControl(Lu* this, hword flow);

void ShowLu (Lu* this, LinkType upDown);

void DebugLu (Lu* this, hword debugLevel);
void NoDebugLu (Lu* this, hword debugLevel);


/* byte GetUpLuNumber(Lu* luP) */
#define GetUpLuNumber(luP) (luP->upLuNumber)

/* byte GetDownLuNumber(Lu* luP) */
#define GetDownLuNumber(luP) (luP->downLuNumber)

/* char* GetHostName(Lu* luP) */
#define  GetHostName(luP) (GetPuName(luP->upPuP))

/* char* GetDownPuName(Lu* luP); */
#define  GetDownPuName(luP) (GetPuName(luP->downPuP))

/* Pu* GetLuUpPuP(Lu* luP) */
#define GetLuUpPuP(luP)  (luP->upPuP)

/* Pu* GetLuDnPuP(Lu* luP) */
#define GetLuDnPuP(luP)  (luP->downPuP)

/* PoolCB* GetLuPool(Lu* this) {return luP->poolP;} */
#define  GetLuPool(luP) (luP->poolP)

/* LuLuState GetLuLuState(Lu* luP) */
#define GetLuLuState(luP)  (luP->luLuState)

/* SscpLuState GetLuSscpState(Lu* luP) */
#define GetLuSscpState(luP)  (luP->sscpLuState)

/* hword GetLuUpFramesSent(Lu* luP) */
#define GetLuUpFramesSent(luP)    (luP->downFramesReceive)

/* hword GetLuDownFramesSent(Lu* luP) */
#define GetLuDownFramesSent(luP)  (luP->upFramesReceive)

/* hword GetLuUpFramesReceive(Lu* luP) */
#define GetLuUpFramesReceive(luP)   (luP->upFramesReceive)

/* hword GetLuDownFramesReceive(Lu* luP) */
#define GetLuDownFramesReceive(luP) (luP->downFramesReceive)

/* LuActFailRsnE GetLuLastActFailRsn(Lu* luP) */
#define GetLuLastActFailRsn(luP)  (luP->lastActFailRsn)

/* void SetLuLastActFailRsn(Lu* luP, LuActFailRsnE rsncode) */
#define SetLuLastActFailRsn(luP, rsncode) (luP->lastActFailRsn = rsncode)

const char* LuStatus(Lu* this);

/*=====================================================================*
* QueryLuLuActive()
*======================================================================*
*
* Description:
*   Returns state of LU-LU session (active/inactive)
*
**/
boolean QueryLuLuActive (Lu* luP);

/*=====================================================================*
* QueryLuBound
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is bound.
*
**/
boolean QueryLuBound(Lu* this);

/*=====================================================================*
* CheckLuOperStateChanged()
*======================================================================*
*
* Description:
*   Returns TRUE if snaLuOperState has changed 
*
**/
boolean CheckLuOperStateChanged(Lu* luP, boolean newLuState); 

/*=====================================================================*
* QueryLuApiLu
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is allocated as an LUAPI LU.
*
**/
boolean QueryLuApiLu(Lu* this);


#endif /* __lu_h */
