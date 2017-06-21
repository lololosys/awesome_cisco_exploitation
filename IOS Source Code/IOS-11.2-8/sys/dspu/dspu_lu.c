/* $Id: dspu_lu.c,v 3.2.58.6 1996/08/28 12:42:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_lu.c,v $
 *------------------------------------------------------------------
 * DSPU LU Function.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * LU FSMs and procedures.  Includes SSCP-LU and LU-LU FSMs.
 *------------------------------------------------------------------
 * $Log: dspu_lu.c,v $
 * Revision 3.2.58.6  1996/08/28  12:42:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.5  1996/08/07  08:58:18  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/07/29  21:46:57  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/21  02:22:00  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.2.58.2  1996/05/17  10:47:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.5  1996/04/09  22:36:26  lmetzger
 * Update of Initial Version of Response Time Reporter
 *
 * Revision 3.2.46.4  1996/04/09  18:37:14  ppearce
 * CSCdi54114:  DSPU crashes router during deactivation of downstream PUs
 * Branch: IbuMod_Calif_branch
 *   Check for null luluTimer before invoking StopItimer()
 *
 * Revision 3.2.46.3  1996/04/03  14:02:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.46.2  1996/03/22  18:50:21  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2.46.1  1996/03/19  03:20:56  ppearce
 * CSCdi51042:  LU cannot be recovered by DSPU w/out deact of downstream PU
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.58.1  1996/03/18  19:33:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:45:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:47:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:04:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  17:03:01  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* The template is used to defined proper CPS type 
 * and timer object type.
 */
#define LU_TEMPLATE

#include "master.h"     /* debug trace statment */
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>

#include "dspu.h"
#include "dspu_lu.h"
#include "dmalloc.h"   /* DspuMalloc and DspuFree */
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_pool.h"
#include "dspu_cps.h"
#include "snaprim.h"
#include "snapiu.h"
#include "dspu_fsmstr.h"
#include "dspu_error.h"
#include "dspu_itimer.h"
#include "dspu_notification.h"
#include "luapi_dspu.h"

static char* const luStatus[];
static char* const luInput[];
static void AckBuffer (Lu* this);


/*=====================================================================*
* SscpLuState
*======================================================================*
*
* Description:
*   SSCP-LU FSM state enumeration.
*   
**/
typedef enum SscpLuState {
   Reset,      /* ds PU is not active and ACTLU has not received */
   dLuStrt,    /* ds PU active, but ACTLU not received from up PU */
   uLuAct,     /* up LU is active, ds PU not active               */
   dLuPendAct, /* ACTLU sent ds but not response yet              */
   dLuActUnav, /* ACTLU response indicating not available         */
   uLuPendAvail,/* NOTIFY avaiable sent but not response yet      */
   BothAvail,   /* Both up down LU are active, LU-LU session ok   */
   dLuPendInact,/* Wait DACTLU response                           */
   uLuPendInact, /* Wait for Notify response                      */
   InactTimeout, /* Wait for Notify response                      */
   dLuPendTimeout, /* wait for Dactlu response                    */
   sscpluMaxState
   } SscpLuState;


/*=====================================================================*
* LuLuState
*======================================================================*
*
* Description:
*   LU-LU FSM state enumeration.
*   
**/
typedef enum LuLuState {LuReset,LuBound,LuPendUnbind,LuMaxState } LuLuState;



/***********************************************************************
* LU CONSTRUCTORS                                                      
************************************************************************
*
* The following routines are the LU constructors.
*   
**/

/*=====================================================================*
* Lu_nw
*======================================================================*
*
* Description:
*
*   This function creates the LU control block and connects it to the
* upstream PU. It is used for both DEDICATED and POOLED LUs. 
*   In the case of DEDICATED LUs this function is called within LuHost_nw  
* to establish the upstream connection and then the downstream connection
* is made with function AddChild. In the case of POOLED LUs, this 
* function is called at configuration of the POOL, and the downstream
* connection is made by a call to LuConnect when the downstream PU is
* active.
*   This function returns a pointer to the newly constructed LU control 
* block, however if the LU control block fails to connect to the upstream
* PU via function AddChild, then the LU control block is freed and a
* null pointer is returned.
*   
**/
Lu* Lu_nw  (Pu*     puP,       /* upstream PU pointer */
            hword   luNumber,  /* lu number           */
            PoolCB* poolP)     /* pool control block  */
{

  Lu* this = DspuMalloc(sizeof(Lu));
  if (this) {

    this->next = 0;              /* Must be set to be enqueued   */
    if (AddChild(GetLuChildren(puP), luNumber, this, puP, &this->upPuP)) { 

      this->downPuP = 0;         /* No downstream PU defined yet */
      this->upLuNumber = (byte) luNumber;
      this->downLuNumber = 0;
      this->timer = 0;
      this->luluTimer = 0;

      this->poolP = poolP;
      if (this->poolP) 
          IncrPoolUpLuDefs(this->poolP);

      this->saveMsgAtPending = 0;

      if (PuLuMapIsLuActive(puP, luNumber))
        this->sscpLuState = uLuAct;
      else
        this->sscpLuState = Reset;

      this->luLuState = Reset;
      this->upFramesReceive = 0;
      this->downFramesReceive = 0;
      this->sequenceNumber = 0;
      this->subState = 0;
      this->luWorkBits = 0;
      this->flowControl = 0;
      this->debugLevel = GetLuDebugLevel(puP);
      this->usage = luInUseByDspu;
      this->luApiP = 0;  /* default is dspu lu */
    } 
    else {
      DspuFree(this);
      this = 0;
    }  
  }
  return this;
}

/*=====================================================================*
* LuHost_nw
*======================================================================*
*
* Description:
*
*   This is invoked when a DEDICATED connection is configured. 
*     LU luStart luRange HOST hostName hostLuStart
*
*   It calls Lu_nw to create the LU and connect it to the upstream PU.
*  The downstream connection is made via the function AddChild.
*
*   If AddChild fails to connect the LU to the downstream PU, then 
*  the LU is deleted from the upstream pu and a null pointer 
*  is returned. However, if AddChild successfully connects the LU to 
*  the downstream PU and if the downstream PU is active, then the 
*  SSCP-LU FSM gets driven with dStartLu. 
**/
Lu* LuHost_nw(Pu*   upPuP,        /* upstream PU pointer   */
              hword luNumber,     /* upstream lu number    */
              Pu*   downPuP,      /* downstream PU pointer */
              hword downLuNumber  
             )
{
  Lu* this = Lu_nw(upPuP, luNumber, 0); /* Construct lu and connect to 
                                           upstream */ 
  if (this){
    this->downLuNumber = (byte) downLuNumber;
    if (AddChild(GetLuChildren(downPuP), downLuNumber, this, 
                 downPuP, &this->downPuP)) {
      if (PuQueryActive(downPuP))    /* Pu active ? */
        StartLu(this);
    }    
    else {
      LuDelete(this);   /* Delete the LU control block */
      this = 0;
    }    
  } 
  return this;
}


/*=====================================================================*
* LuDelete
*======================================================================*
*
* Description:
*   This is the Lu destructor called when:
* 1. No lu for host lu. "no dspu lu nn host" only if lu is inactive
* 2. No pool statement. " no dspu pool poola" only if lu is inactive
* 3. When upstream pu is destructed. (Pu_dl)
* 4. When downstream pu is destructed and the lu is dedicated to the pu
* 5. An LUAPI lu is destructed.
*/

void LuDelete(Lu* this)
{

   /* Call AckBuffer just in case we are due a FreeBuffer. */
   AckBuffer(this);

   if (this->downPuP){
      if ((this->flowControl & UpFlowOff) != 0)
         PuFlowControlFromLu(this->downPuP, FlwCtl_Resume);
      DelChild(GetLuChildren(this->downPuP), this->downLuNumber, &this->downPuP);
   }
   if (this->luluTimer){
       StopItimer(this->luluTimer);
       this->luluTimer = 0;
   }
   if (this->timer){
      StopItimer(this->timer);
      this->timer = 0;
   }
   
   if (this->poolP) {    /* if lu belongs to the pool */
      DecrPoolUpLuDefs(this->poolP);
      DspuDeleteQueueElement(&(GetPoolHostLuQhdr(this->poolP)), this);
   }

   DelChild(GetLuChildren(this->upPuP), this->upLuNumber, &this->upPuP);
   DspuFree(this);          /* remove LU control block */
}


/*=====================================================================*
* LuPuChildDelete
*======================================================================*
*
* Description:
*   This is the Lu destructor called when a pu is destructed (Pu_dl).
*/

void LuPuChildDelete (Lu* this)
{
   if (this->usage == luInUseByLuApi)
      LuApiLuDelete (this);

   else
      /* DSPU LU */
      LuDelete (this);
}


/*=====================================================================*
* LuDisconnect
*======================================================================*
*
* Description:
*    This subroutine is called by the downstream to delete lu when
* 1. No lu pool. " no dspu lu nn pool"
* 2. When a down stream pu is destructed. (PU_dl)
* 3. When a down streamp pu is changed to inactive.
*/

void LuDisconnect(Lu* this)
{
   if ((this->flowControl & UpFlowOff) != 0)
       PuFlowControlFromLu(this->downPuP, FlwCtl_Resume);
   
   if ((this->flowControl & DownFlowOff) != 0)
       PuFlowControlFromLu(this->upPuP, FlwCtl_Resume);
   
   this->flowControl = 0;

   DResetLu( this);
   
   if (this->poolP){
      DelChild(GetLuChildren(this->downPuP), this->downLuNumber, 
               &this->downPuP);
      this->downPuP = 0;   
   }


}



/***********************************************************************
* LU FSM DECLARATIONS                                                      
************************************************************************
**/
/*=====================================================================*
* SscpLuInput
*======================================================================*
*
* Description:
*   SSCP-LU FSM input enumeration.
*   
**/
typedef enum SscpLuInput {
   uActlu,     /* ACTLU reqeust from host                    */
   uDactlu,    /* DACTLU request from host                   */
   uNtfyRspP,  /* Notify +ve response from upstream          */
   uNtfyRspN,  /* Notify -ve response from upstream          */
   uOtherSscpLu,/* other SSCP_LU  sna from upstream          */
   uLuLu,      /* other LU_LU sna from upstream              */
   uStopLu,    /* Signal from upstream that PU is inactive   */
   dStartLu,   /* A signal from ds that PU is active         */
   dStopLu,    /* A signal from ds that PU is inactive       */
   dActluRspPA,/* ACTLU +ve available                        */
   dActluRspPN,/* ACTLU +ve not available                    */
   dActluRspN, /* ACTLU -ve                     */
   dDactluRsp, /* DACTLU rsp (+ve or -ve) from ds            */
   dNtfyAvail, /* Notify (available) from ds                 */
   dNtfyUnav,  /* Notify (unavailable) from ds               */
   dOtherSscpLu, /* SSCPLU or LU_LU reqeust/rsp from ds      */
   dLuLu,      /* other LU_LU sna from downStream            */
   uRecovered, /* internal signal for pending state */
   luTimeout,
   LuInactTimeout, /* no activity time out              */
   sscpluMaxInput
} SscpLuInput;


/*=====================================================================*
* LuInput
*======================================================================*
*
* Description:
*   LU-LU FSM input enumeration.
*   
**/
typedef enum LuInput {BindPosRsp, UnbindRequest, UnbindResponse,
                      uResetLu, dResetLu, luLuTimeout, LuMaxInput,
                     }LuInput;

/*====================================================================*
* SscpLuFsm and LuFsm forward declarations.
*=====================================================================*
* Forward declaration of the SSCP-LU and LU-LU FSM functions because
* they are called by the LU UTILITIES.
**/
boolean SscpLuFsm(Lu         * this, 
                  SscpLuInput  fsmInput, 
                  DataReq_t  * clsiBfrP);

static boolean                        /* forward frame if TRUE */
LuFsm(Lu* this, LuInput fsmInput, DataReq_t* clsiBfrP);



/***********************************************************************
* LU UTILITIES                                                      
************************************************************************
*
* The following routines are the LU utilities.
*   
**/

/*=====================================================================*
* SendPosRsp
*======================================================================*
*
* Description:
*    Change a request into a positive response and send it to the PU.
*    If the Change fails then it is because the request did not
*    request a definite response, so then we free the buffer.
*
**/
static void SendPosRsp (Pu*        puP,
                        DataReq_t* clsiBfrP
                       )
{
  if ( ChangeMuToPosRsp(clsiBfrP) )
     SendPu(puP, clsiBfrP);
  else
     DspuCLSIMsgDestroy(clsiBfrP);
}

/*=====================================================================*
* SendNegRsp
*======================================================================*
*
* Description:
*    Change a request into a negative response and send it to the PU.
*    If the Change fails then it is because the request was sent
*    "no response", so then the buffer is freed.
*
**/
static void SendNegRsp (Pu*        puP,
                        DataReq_t* clsiBfrP,
                        hword      systemSense,
                        hword      userSense
                       )
{
  if ( ChangeMuToNegRsp(clsiBfrP, systemSense, userSense) ) {
     SendPu(puP, clsiBfrP);
     IncrPuNegativeRspSents(puP);
  }
  else
     DspuCLSIMsgDestroy(clsiBfrP);
}

/*=====================================================================*
* SendLuDown
*======================================================================*
*
* Description:
*    Message received from the Upstream PU. Pass it to the Downstream 
* PU if it exists, updating the DAF and incrementing the upFramesReceive
* count. If the downstream PU does not exist, then change the message to 
* a negative response and send it back to the upstream PU. If we are
* unable to change the message to a negative response, then destroy
* the message.
*
**/
static void SendLuDown(Lu*        this,
                DataReq_t* clsiBfrP
                )
{
   if (this->downPuP == 0 || GetPuLsP(this->downPuP) == 0){
     if (ChangeMuToNegRsp(clsiBfrP, ResourceNotAvailable, 0)) {
        SendPu(this->upPuP, clsiBfrP);
        IncrPuNegativeRspSents(this->upPuP);
     }
     else
        DspuCLSIMsgDestroy(clsiBfrP);
   }
   else {
      this->upFramesReceive++;      /* update statistics */
      UpdateDAF(ReturnDataHandle(clsiBfrP), this->downLuNumber);
      SendPu(this->downPuP, clsiBfrP);
   }
}

/*=====================================================================*
* SendLuUp
*======================================================================*
*
* Description:
*    Message received from the Downstream PU. Pass it to the upstream
* PU if it exists, updating the OAF and incrementing the FramesReceive
* count. If the upstream PU does not exist, then change the message to 
* a negative response and send it back to the downstream PU. If we are
* unable to change the message to a negative response, then destroy
* the message.
*
**/
static void SendLuUp(Lu*        this,
              DataReq_t* clsiBfrP
              )
{
   if (this->upPuP == 0 || GetPuLsP(this->upPuP) == 0){
     if (ChangeMuToNegRsp(clsiBfrP, ResourceNotAvailable, 0)) {
        SendPu(this->downPuP, clsiBfrP);
        IncrPuNegativeRspSents(this->downPuP);
     }
     else
        DspuCLSIMsgDestroy(clsiBfrP);
   }
   else{
      this->downFramesReceive++;      /* update statistics */
      UpdateOAF(ReturnDataHandle(clsiBfrP), this->upLuNumber);
      SendPu(this->upPuP, clsiBfrP);
   }
}

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
       )
{
    if (snaMu->reqRsp == SNA_REQUEST) {
       if (puP->puType == Upstream) {
          if (snaMu->ruType == SNA_ACTLU) {

             DataReq_t*  actluRspP;          /* actlu response buffer */

             /* for ACTLU try to send +rsp so that the LU
              * remains active and usable by LU API
              */
             if ((actluRspP = ChangeActluToPosRsp(clsiBfrP, LU_INHIBITED)) == 0){
                /* failed to build +rsp ACTLU so turn existing buffer into -rsp */
                SendNegRsp( puP, clsiBfrP, ResourceNotAvailable, 0);
                PuLuMapSetLuInactive(puP, luNumber);
             }
             else {
                /* cleanup original buffer if copy was made */
                if (actluRspP != clsiBfrP)
                   DspuCLSIMsgDestroy(clsiBfrP);
                PuLuMapSetLuActive(puP, luNumber);
                SendPu(puP, actluRspP);
             }

          }
          else if (snaMu->ruType == SNA_DACTLU) {
             /* record status change in lu map and return pos rsp to DACTLU */
             PuLuMapSetLuInactive(puP, luNumber);
             SendPosRsp( puP, clsiBfrP);

          }
          else {
             if (PuLuMapIsLuActive(puP, luNumber))
                SendPosRsp( puP, clsiBfrP);
             else
                SendNegRsp( puP, clsiBfrP, NoSession, 0);
          }

       }
       else {
          /* downstream case */
          SendNegRsp( puP, clsiBfrP, ResourceNotAvailable, 0);
       }
    }
    else
       /* SNA_RESPONSE or not the first segment */
       DspuCLSIMsgDestroy(clsiBfrP);  /* discard responses */

}


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
       )
{
   /* SendNegRsp discards responses, or "no response" requests.
    * It changes other requests into response and forwards to the pu.
    */
   SendNegRsp( puP, clsiBfrP, NoSession, 0);
}


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
       )
{
   if (this->usage == luInUseByLuApi)
      LuApiReceiveSscpLu (this, clsiBfrP, snaMu);

   else {
      /* DSPU LU */
      hword          fsmInput;
   
      if (puType == Upstream){
         if (snaMu->reqRsp == SNA_REQUEST){
     
            /* up stream SSCP-LU sna request */
           
            if (snaMu->ruType == SNA_ACTLU)
               fsmInput = uActlu;
            else if (snaMu->ruType == SNA_DACTLU)
               fsmInput = uDactlu;
            else
               fsmInput = uOtherSscpLu;

         } else if (snaMu->reqRsp == SNA_RESPONSE) {

            /* up stream SSCP-LU sna response */
           
            if (snaMu->ruType == SNA_NOTIFY)
               fsmInput = (snaMu->rspType == SNA_POSITIVE)?
                        uNtfyRspP: uNtfyRspN;
            else 
               fsmInput = uOtherSscpLu;

         } else { 

            /* up stream SSCP-LU: only (snaMu->reqRsp == SNA_NO_RH) expected */

            /* we assume that none of the special stuff can be big enough
             * to be segmented - so this must be a middle or last segment
             * of sscp-lu session DATA
             */
            fsmInput = uOtherSscpLu;
	 }

      } else {
               
         /* downstream */
         
        if (snaMu->reqRsp == SNA_REQUEST) {

           /* down stream SSCP-LU sna request */
           
           if (snaMu->ruType == SNA_NOTIFY)
              fsmInput = snaMu->enable?
                          dNtfyAvail: dNtfyUnav;
           else
              fsmInput = dOtherSscpLu;

        } else if (snaMu->reqRsp == SNA_RESPONSE) {

           /* down stream SSCP-LU sna response */
           
           if (snaMu->ruType == SNA_ACTLU) {
              if (snaMu->rspType == SNA_NEGATIVE)
                 fsmInput = dActluRspN;
              else
                 fsmInput = snaMu->enable?
                            dActluRspPA: dActluRspPN;

           } else if (snaMu->ruType == SNA_DACTLU)
              fsmInput = dDactluRsp;
           else
              fsmInput = dOtherSscpLu;

         } else { 

            /* down stream SSCP-LU and (snaMu->reqRsp == SNA_NO_RH) */

            /* we assume that none of the special stuff can be big enough
             * to be segmented - so this must be a middle or last segment
             * of sscp-lu session DATA
             */
            fsmInput = dOtherSscpLu;

	 }
      }
      SscpLuFsm(this, fsmInput, clsiBfrP);

      if ((fsmInput == dActluRspN) && (puType == Downstream) )
         DspuNetMgmtNotification(this, LU_ACTFAIL, 
                               LuActFailRsn_NegativeRsp, Downstream);
   }
      
}


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
       )
{
   if (this->usage == luInUseByLuApi)
      LuApiReceiveLuLu (this, clsiBfrP, snaMu);

   else {
      /* DSPU LU */
      hword          fsmInput;
   
      fsmInput = (puType == Upstream)? uLuLu: dLuLu;
              
      if (SscpLuFsm(this, fsmInput, clsiBfrP)) {
              
         /* LU_LU data is accepted by SscpLUFsm, now pass to
          * the lu-lu fsm. First further decode sna data
          */

         /* Note that, in snaMu, ruType is only valid if reqRsp is
          * SNA_REQUEST or SNA_RESPONSE.  It is not valid if reqRsp
          * is SNA_NO_RH.
          */

         boolean forwardFrame = TRUE;
                
         if (snaMu->reqRsp == SNA_RESPONSE) {

            if ((snaMu->ruType == SNA_BIND) &&
                (snaMu->rspType == SNA_POSITIVE)) {

               forwardFrame = FALSE;
               fsmInput = BindPosRsp;

            } else if (snaMu->ruType == SNA_UNBIND) {
         
               forwardFrame = FALSE;
               fsmInput = UnbindResponse;
	    }

         } else if (snaMu->reqRsp == SNA_REQUEST) {

            if (snaMu->ruType == SNA_UNBIND) {
         
               forwardFrame = FALSE;
               fsmInput = UnbindRequest;
	    }

	 } /* else snaMu->reqRsp == SNA_NO_RH */

         if (!forwardFrame)
            forwardFrame = LuFsm(this, fsmInput, clsiBfrP);
                  
         if (forwardFrame){
            if (puType == Upstream)
               SendLuDown(this, clsiBfrP);
            else
               SendLuUp(this, clsiBfrP);
         }
      }
   }
}


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
void ReceiveLuNmvtPsidRsp(
       Lu*             this,
       DataReq_t*      clsiBfrP
       )
{
   /* only LUAPI LUs are really interested in these responses now */
   if (this->usage == luInUseByLuApi)
      LuApiReceiveLuNmvtPsidRsp (this, clsiBfrP);
   else
      DspuCLSIMsgDestroy(clsiBfrP);
}

 
/*=====================================================================*
* LuConnect
*======================================================================*
*
* Description:
*
*   When a POOL is configured, the LUs are created and connected only to 
* the upstream PU with the function Lu_nw. Then when the downstream
* PU becomes active, the POOLed LUs are connected to the downstream pu
* with the procedure LuConnect.   
**/
ConnectStatus LuConnect(Lu*   this,   /* LU to connect to downstream */
                        Pu*   puP,    /* downstream PU pointer */
                        hword luNumber  /* downstream LU number  */
                       )
{
  ConnectStatus connectStatus = Successful;
 
  this->downLuNumber = (byte) luNumber;
  if (AddChild(GetLuChildren(puP), luNumber, this, puP,&this->downPuP)) {
    this->debugLevel = GetLuDebugLevel(puP);
    if (PuQueryActive(puP))    /* Pu active ? */
       StartLu(this);
  }
  else 
    connectStatus = NoMemory;  /* Connect failed due to no memory */
    
  return connectStatus;
}

/*=====================================================================*
* QueryLuActive
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is active. The LU is active if it is NOT
* in Reset or dLuStrt state.
*
**/
boolean QueryLuActive(Lu* this)
{
   if (this->usage == luInUseByLuApi)
      return LuApiQueryLuActive (this);

   return this->sscpLuState != Reset && this->sscpLuState != dLuStrt;
}

/*=====================================================================*
* GetSnaLuOperState()
*======================================================================*
*
* Description:
*   Returns TRUE if SSCP-LU session is active
*
**/
static boolean GetSnaLuOperState(LinkType puType, SscpLuState luState)
{

  const boolean upStreamLuStateTable[sscpluMaxState] =
          { FALSE,    /* Reset          */
            FALSE,    /* dLuStrt        */
            TRUE,     /* uLuAct         */
            TRUE,     /* dLuPendAct     */
            TRUE,     /* dLuActUnav     */
            TRUE,     /* uLuPendAvail   */
            TRUE,     /* BothAvail      */
            TRUE,     /* dLuPendInact   */
            TRUE,     /* uLuPendInact   */
            TRUE,     /* InactTimeout   */
            TRUE,     /* dLuPendTimeout */
          };

  const boolean dnStreamLuStateTable[sscpluMaxState] =
          { FALSE,    /* Reset          */
            FALSE,    /* dLuStrt        */
            FALSE,    /* uLuAct         */
            FALSE,    /* dLuPendAct     */
            TRUE,     /* dLuActUnav     */
            TRUE,     /* uLuPendAvail   */
            TRUE,     /* BothAvail      */
            TRUE,     /* dLuPendInact   */
            TRUE,     /* uLuPendInact   */
            TRUE,     /* InactTimeout   */
            TRUE,     /* dLuPendTimeout */
          };

  if (puType == Upstream)
    return(upStreamLuStateTable[luState]);
  else
    return(dnStreamLuStateTable[luState]);

} /* End GetSnaLuOperState() */

/*=====================================================================*
* QueryLuActiveUpOrDown
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is active. Active is viewed from either up
* or down stream
*
**/
boolean QueryLuActiveUpOrDown(Lu* this, LinkType upOrDown)
{
   return(GetSnaLuOperState(upOrDown, this->sscpLuState));
}

/*=====================================================================*
* QueryLuActiveAndDedicated
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is active and dedicated.
*
**/
boolean QueryLuActiveAndDedicated(Lu* this)
{
   return QueryLuActive(this) && this->poolP == 0;
}

/*=====================================================================*
* QueryLuBound
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is bound.
*
**/
boolean QueryLuBound (Lu* this) {
   if (this->usage == luInUseByLuApi)
      return LuApiQueryLuBound (this);
   /* else DSPU LU */
   return (this->luLuState == LuBound);
}


/***********************************************************************
* LU-LU FSM routines                                                      
************************************************************************
* The following are the LU-LU FSM routines.
*   
**/

/*=====================================================================*
* luLuFsm
*======================================================================*
*
* Description:
*   LU-LU FSM Structure
*
**/
const static FsmStruct luLuFsm[LuMaxInput][LuMaxState] = {

/*                   0             1        2        */
/*                 Reset        Bound   PendUnbind */
/*BindPosRsp*/     {{1,a1},    {1,a1},   {1,a5}},
/*UnbindRequest*/  {{0,a1},    {0,a1},   {0,a4}},
/*UnbindResponse*/ {{0,a1},    {1,a1},   {0,a3}},
/*uResetLu*/       {{0,a1},    {1,a1},   {0,a3}},
/*dResetLu*/       {{0,a0},    {2,a2},   {2,a0}},
/*timeout*/        {{0,a0},    {1,a0},   {0,a3}}
};

/*=====================================================================*
* UResetLu, DResetLu
*======================================================================*
*
* Description:
*   Drives the SSCP-LU FSM for hierachical reset
*
**/
void UResetLu(Lu* this)
{
   if (this->usage == luInUseByLuApi)
      LuApiResetLu(this);
   else
      SscpLuFsm(this, uStopLu, 0);
}

void DResetLu(Lu* this)
{
   SscpLuFsm(this, dStopLu, 0);
}

static char *const luStatus[] = {
   "Reset",
   "dsLuStart",
   "upLuActive",
   "dsLuPendingActive",
   "dsLuActUnavail",
   "upLuPendAvail",
   "BothAvail",
   "dsLuPendInact",
   "UpLuPendInact",
   "InactivityTimeout",
   "dsInctivityPendInact"
   };

/*=====================================================================*
* LuFsm
*======================================================================*
*
* Description:
*   LU FSM
**/

static boolean                        /* forward frame if TRUE */
LuFsm(Lu* this, LuInput fsmInput, DataReq_t* clsiBfrP)
{
   boolean forward = FALSE;
   const FsmStruct* luFsm = &luLuFsm[fsmInput][this->luLuState];
   this->luLuState = luFsm->state;   
   switch(luFsm->action){

     /*---- Null action. */
     case a0:     
        break;
     
     /*---- Forward buffer to partner. */
     case a1:
        forward = TRUE;
        break;
        
     /*---- Reset LU from downstream, send UNBIND upstream. */
     case a2:     
        LogForLuWork(this, WORK_UNBIND);
        break;
     
     
     /*---- UNBIND response, upstream LU reset or timeout. 
            Call PuFreeBuffer to free the buffer used and 
            do not forward clsiBfr to partner. */
     case a3:
        PuFreeBuffer(this->upPuP);
        if (this->luluTimer) {
            StopItimer(this->luluTimer);
            this->luluTimer = 0;
        } 
        if (clsiBfrP)
           DspuCLSIMsgDestroy(clsiBfrP);
        break;
     
     /*---- UNBIND request. Call PuFreeBuffer to free the
            buffer used. Forward clsiBfr to partner. */
     case a4:
        forward = TRUE;
        PuFreeBuffer(this->upPuP);
        break;
    
      case a5:           /* we had received a bindPos response while
                          * waiting unbind response. Unbind resp 
                          * is swallowed. received it
                          */
        forward = TRUE;
        PuFreeBuffer(this->upPuP);
        if (this->luluTimer) {
            StopItimer(this->luluTimer);
            this->luluTimer = 0;
        }
        break;
    }

   return forward;
}


/***********************************************************************
* SSCP-LU FSM routines                                                      
************************************************************************
*
* The following are the SSCP-LU FSM routines.
*   
**/

/*=====================================================================*
* sscpLuFsm 
*======================================================================*
*
* Description:
*   SSCP-LU FSM structure
*
**/

#define T 10
#define b2  a34
#define b10 a35
#define b18 a36
#define b26 a37
#define b32 a38

const static FsmStruct sscpLuFsm[sscpluMaxInput][sscpluMaxState] = {

/*   0        1      2       3       4      5       6       7        8       9      T
 * Reset     dLu    uLu     dLu     dLu     uLu    Both    dLu      uLu    Inact    dLu
 *         Strted  Active PndAct   ActUna  PendAv  Avail   PInac   PInac   Tmout   Pinac
 *uActlu*/
  {{2,a1}, {3,a2}, {2,a1}, {3,a1}, {4,a1}, {5,a3}, {3,a2}, {3,b2}, {2,a40}, {3,b2}, {3,b2}},
/*uDactlu*/
  {{0,a5}, {1,a5}, {0,a5}, {7,a6}, {7,a6}, {7,a6}, {7,a6}, {7,a5}, {0,a41}, {T,a42}, {T,a5}},
/*uNtfyRspP*/
  {{0,a29},{1,a29},{2,a29},{3,a29},{4,a29},{6,a14},{6,a29},{7,a29},{2,a23},{T,b32},{T,a29}},
/*uNtfyRspN*/
  {{0,a29},{1,a29},{2,a29},{3,a29},{4,a29},{7,a7}, {6,a29},{7,a29},{2,a20},{T,a20},{T,a29}},
/*uOtherSscpLu*/
  {{0,a8}, {1,a8}, {2,a8}, {3,a8}, {4,a9}, {5,a9}, {6,a9}, {7,a8}, {8,a8}, {9,a8}, {T,a8}},
/*uLuLu*/
  {{0,a8}, {1,a8}, {2,a8}, {3,a8}, {4,a8}, {5,a16},{6,a16},{7,a8}, {8,a16},{9,a8}, {T,a8}},
/*uStopLu*/
  {{0,a0}, {1,a0}, {0,a0}, {7,b18},{7,a18},{7,b18},{7,a18},{7,a19},{0,a24},{T,b18},{T,a19}},
/*dStartLu*/
  {{1,a25},{1,a25},{3,a21},{3,a25},{4,a25},{5,a25},{6,a25},{7,a25},{8,a25},{9,a25},{T,a25}},
/*dStopLu*/
  {{0,a26},{0,a26},{2,a26},{2,b26},{2,a26},{8,b10},{8,a10},{0,a27},{8,a26},{8,a26},{0,a33}},
/*dActluRspPA*/
  {{0,a29},{1,a29},{2,a29},{5,a4}, {4,a29},{5,a29},{6,a29},{7,a29},{8,a29},{9,a29},{T,a29}},
/*dActluRspPN*/
  {{0,a29},{1,a29},{2,a29},{4,a29},{4,a29},{5,a29},{6,a29},{7,a29},{8,a29},{9,a29},{T,a29}},
/*dActluRspN*/
  {{0,a29},{1,a29},{2,a29},{2,a29},{4,a29},{5,a29},{6,a29},{7,a29},{8,a29},{9,a29},{T,a29}},
/*dDactluRsp*/
  {{0,a29},{1,a29},{2,a29},{3,a29},{4,a29},{5,a29},{6,a29},{1,a30},{8,a29},{9,a29},{0,a33}},
/*dNtfyAvail*/
  {{0,a13},{1,a13},{2,a13},{3,a13},{5,a11},{5,a39},{6,a39},{7,a13},{8,a13},{9,a39},{T,a13}},
/*dNtfyUnav*/
  {{0,a13},{1,a13},{2,a13},{3,a13},{4,a39},{4,a12},{4,a12},{7,a13},{8,a13},{9,a39},{T,a13}},
/*dOtherSscpLu*/
  {{0,a13},{1,a13},{2,a13},{3,a13},{4,a15},{5,a22},{6,a15},{7,a13},{8,a13},{9,a13},{T,a13}},
/*dLuLu*/
  {{0,a13},{1,a13},{2,a13},{3,a13},{4,a13},{5,a22},{6,a17},{7,a13},{8,a13},{9,a13},{T,a13}},
/*uRecovered*/
  {{2,a0}, {3,a0}, {2,a0}, {3,a0}, {4,a0}, {5,a0}, {6,a0}, {7,a0}, {8,a0}, {9,a0}, {T,a0}},
/*luTimeout*/
  {{0,a0}, {1,a0}, {2,a0}, {1,a29},{4,a29},{1,a29},{6,a0}, {0,a28},{0,a28},{T,b32},{0,a33}},
/*LuInactTimeout*/
  {{0,a0}, {1,a0}, {2,a0}, {3,a0}, {T,a32},{5,a0}, {9,a31},{7,a0}, {8,a0}, {9,a0}, {T,a0}}
};


/*=====================================================================*
* StartLu
*======================================================================*
*
* Description:
*   Drives the SSCP-LU FSM. 
*
**/
void StartLu(Lu* this)
{
   /* Before downstream started, check if the link station is in flow
    * off state, if so, send flowoff to partner
    */
   if ((this->flowControl & UpFlowOff) == 0 && PuQueryFlowOff(this->upPuP)){
       this->flowControl |= UpFlowOff;
       PuFlowControlFromLu(this->downPuP, FlwCtl_Stop);
   }
   if ((this->flowControl & DownFlowOff) == 0 && PuQueryFlowOff(this->upPuP)){
       this->flowControl |= DownFlowOff;
       PuFlowControlFromLu(this->upPuP, FlwCtl_Stop);
   }
   SscpLuFsm(this, dStartLu, 0);
}

/*=====================================================================*
* LuTimeout, LuLuTimeout
*======================================================================*
*
* Description:
*   Timeout for SSCP-LU and LU-LU FSM
*
**/
static void LuTimeout(Lu* this)
{
   SscpLuFsm(this, luTimeout, 0);
}

static void LuLuTimeout(Lu* this)
{
   LuFsm(this, luLuTimeout, 0);
}

/*=====================================================================*
* StartTimer
*======================================================================*
*
* Description:
*   Start the timer. If the timer is already started, then stop it
* and restart it.
*  
**/
static void StartTimer(Lu* this)
{
  if (this->timer)
      StopItimer(this->timer);
  this->timer = GetAndStartItimer(20*SECONDS, LuTimeout, this);
}

/*=====================================================================*
* AckBuffer
*======================================================================*
*
* Description:
*    Acknowledge the buffer by calling PuFreeBuffer and stop the timer.
*  
**/
static void AckBuffer(Lu* this)
{
   /* Only do a freeBuffer if we have one pending.
      Note that this algorithm depends on the fact that we only reserve
      one buffer at a time per SSCP-LU session, and we always free any
      outstanding one before reserving another.
      Further note that AckBuffer is only used to free SscpLu work buffers 
      - LuLu work buffers are freed by direct calls to PuFreeBuffer.       */
   if (this->subState & SscpLuWorkBufferInUse) {
      this->subState &= ~SscpLuWorkBufferInUse;

      PuFreeBuffer((this->subState & UpstreamAcknowledge)?
                   this->upPuP: this->downPuP);
      if (this->timer)
         StopItimer(this->timer);
      this->timer = 0;
    }
}

/*=====================================================================*
* LogForLuWork
*======================================================================*
*
* Description:
*   This procedure is called whenever the LU wants to send DACTLU, ACTLU,  
* NMVT PSID, NotifyAvail, NotifyNotAvail or UNBIND; because it must ask
* permission of the global buffer management to get a buffer.
*   It turns on the appropriate bit in the LU work bits, and calls 
* LogForPuWork(LuWork) in the appropriate PU parent (DACTLU and ACTLU
* are downstream, and the others upstream).
*
**/
void LogForLuWork
       (Lu  *this,
       byte luWork
       )
{
  
  this->luWorkBits |= luWork;

  if ((luWork == WORK_DACTLU) || (luWork == WORK_ACTLU))
    LogForPuWork(this->downPuP, WORK_LU_DOWN);
  else   
    LogForPuWork(this->upPuP, WORK_LU_UP);                
}       

/*=====================================================================*
* DoLuWorkUp
*======================================================================*
*
* Description:
*   This checks the UPSTREAM LU work bits in order. If the work bit   
* is on and buffers are available it sends the appropriate command, 
* turns the bit off, starts a timer. TRUE is returned if the work
* was done or no buffers were available. FALSE is returned if no
* LU work bit was set. The enum workResult is set to indicate if 
* work was done, work was not done or no buffers were available.
* 
* Note: This function returns a boolean since it is called within
*       QueryAllChildrenParm.
*
**/
boolean DoLuWorkUp
          (Lu* this
          ,WorkResult* workResult
          )
{
  DataReq_t *clsiBfrP;
  
  if (this->luWorkBits & WORK_UNBIND)
  { 
    if ((clsiBfrP= BuildUnbind(this->upLuNumber, 1, NON_BLOCKING))!=0){
       this->luWorkBits &= (~WORK_UNBIND);
       SendPu(this->upPuP, clsiBfrP);
       this->luluTimer = GetAndStartItimer(20*SECONDS, LuLuTimeout, this);
       *workResult = WORK_DONE;
       dspuActTraceType = TraceTypeUnbind;
       return TRUE;
    }
  }
  else if (this->luWorkBits & WORK_NOTIFYUNAVAIL) 
  {
    if ((clsiBfrP=BuildNotify(this->upLuNumber, LU_NOT_AVAILABLE))!=0) {
      this->luWorkBits &= (~WORK_NOTIFYUNAVAIL);
      dspuActTraceType = TraceTypeNotifyNotAvail;
    }
    
  }
  else if (this->luWorkBits & WORK_NOTIFYAVAIL)
  {
    if ((clsiBfrP = BuildNotify(this->upLuNumber, LU_AVAILABLE))!=0) {
      this->luWorkBits &= (~WORK_NOTIFYAVAIL);
      dspuActTraceType = TraceTypeNotifyAvail;
    }
    
  }
  else if (this->luWorkBits & WORK_LUAPI) {
     this->luWorkBits &= (~WORK_LUAPI);
     return LuApiDoWork( this, workResult);
   }

  else{
    *workResult = WORK_NOT_DONE;
    return FALSE;
  }
  
  if (clsiBfrP==0)
    *workResult = NO_WORK_BUFFERS_AVAILABLE;
  else{

    /* Before doing this work, check if the last buffer has been
       freed - this will hopefully avoid any "leakage" of buffers
       from the activation-window.                                */
    AckBuffer(this);  /* This will only free a buffer if one outstanding. */
    this->subState |= SscpLuWorkBufferInUse;

    this->subState |= UpstreamAcknowledge; /* for upstream ack */
    SendPu(this->upPuP, clsiBfrP);
    StartTimer(this);
    *workResult = WORK_DONE;
  }
  return TRUE;
}


/*=====================================================================*
* QueryLuSendNmvt
*======================================================================*
*
* Description:
*   This routine is called by the upstream pu that he is ready to send
* an nmvt if there is one to send
**/

boolean QueryLuSendNmvt(Lu* this)
{
  if (this->subState & LuNmvtPsig)
  {
    this->subState &= ~LuNmvtPsig;      /* the nmvt had been sent */
    PuSendNmvt(this->upPuP, BuildNmvtPsid(this->upLuNumber),
               this->upLuNumber);
    return TRUE;
  }
  return FALSE;
}

/*=====================================================================*
* DoLuWorkDown
*======================================================================*
*
* Description:
*   This checks the DOWNSTREAM LU work bits in order. If the work   
* bit is on and buffers are available it sends the appropriate command, 
* turns the bit off, starts a timer. TRUE is returned if the work
* was done or no buffers were available. FALSE is returned if no
* LU work bit was set. The enum workResult is set to indicate if 
* work was done, work was not done or no buffers were available.
* 
* Note: This function returns a boolean since it is called within
*       QueryAllChildrenParm.
*
**/
boolean DoLuWorkDown
          (Lu* this
          ,WorkResult* workResult
          )
{
  DataReq_t *clsiBfrP;
  
  if (this->luWorkBits & WORK_DACTLU) 
  {
     if ((clsiBfrP = BuildDactlu(this->downLuNumber)) != 0) {
        this->luWorkBits &= (~WORK_DACTLU);
        dspuActTraceType = TraceTypeDactlu;
      }
  }
  else if (this->luWorkBits & WORK_ACTLU)
  {
    if ((clsiBfrP = BuildActlu(this->downLuNumber)) != 0) {
      this->luWorkBits &= (~WORK_ACTLU);
      dspuActTraceType = TraceTypeActlu;
    }
  }
  else{
    *workResult = WORK_NOT_DONE;
    return FALSE;
  }
  
  if (clsiBfrP==0)
    *workResult = NO_WORK_BUFFERS_AVAILABLE;
  else{
    /* Before doing this work, check if the last buffer has been
       freed - this will hopefully avoid any "leakage" of buffers
       from the activation-window.                                */
    AckBuffer(this);  /* This will only free a buffer if one outstanding. */
    this->subState |= SscpLuWorkBufferInUse;

    this->subState &= ~UpstreamAcknowledge; /* for downstream ack */
    SendPu(this->downPuP, clsiBfrP);
    StartTimer(this);
    *workResult = WORK_DONE;
  }
  return TRUE;
}


/*=====================================================================*
* SendActluRsp
*======================================================================*
*
* Description:
*
*   This attempts to send an Actlu positive response. 
* Since the response is larger than the request, we must get another
* buffer. If we are unable to get another buffer, convert the request 
* into a negative response and send it to the PU.
*
**/
static void SendActluRsp(Lu* this, DataReq_t  * clsiBfrP)
{
   DataReq_t*  actluRspP;          /* actlu response buffer */
  
   if ((actluRspP = ChangeActluToPosRsp(clsiBfrP, LU_INHIBITED)) == 0){
        ChangeMuToNegRsp(clsiBfrP, ResourceNotAvailable, 0);
        IncrPuNegativeRspSents(this->upPuP);
        actluRspP = clsiBfrP;
   }
   this->subState &= ~UpRqstInactive;   /* upstream request active */
   SendPu(this->upPuP,  actluRspP);
}

/*=====================================================================*
* SscpLuFsm
*======================================================================*
*
* Description:
*   SSCP LU FSM
*
**/
boolean SscpLuFsm(Lu         * this, 
                  SscpLuInput  fsmInput, 
                  DataReq_t  * clsiBfrP)
{
  boolean     moreDecodeNeeded = FALSE;
  const FsmStruct * luFsm = &sscpLuFsm[fsmInput][this->sscpLuState];

  DspuFsmTrace2(this->debugLevel, LuFsmTrace, this->upPuP, this->upLuNumber,
                  fsmInput, this->sscpLuState, luFsm->state, luFsm->action);

  /* Check if LuState is changing between active<-->inactive           */
  /*   LuStateChangeTrap will be generated from within Check() routine */

  if (this->sscpLuState != luFsm->state){
      CheckLuOperStateChanged(this, luFsm->state);
      DspuFsmChgTrace2(this->debugLevel, LuFsmChgTrace, 
                         this->upPuP, this->upLuNumber, luInput[fsmInput], 
                         luStatus[this->sscpLuState], luStatus[luFsm->state]);
  }
  this->sscpLuState = luFsm->state;
  this->luActivityTimer = 0;        /* reset timer */
   
  switch(luFsm->action){

    /*---- Destroy clsi buffer. */
    case a0:
      if (clsiBfrP)
        DspuCLSIMsgDestroy(clsiBfrP);
      break;
    
    /*---- uActlu. Send actlu +ve response upstream with LU unavailable */
    case a1:   
      SendActluRsp(this, clsiBfrP);
      PuLuMapSetLuActive(this->upPuP, this->upLuNumber);
      break;
     
    /*---- uActlu. Send actlu +ve response upstream, send actlu 
           downstream. */
    case b2: 
      AckBuffer(this); 
      /* fall down to a2*/

    case a2:  
      SendActluRsp(this, clsiBfrP);
      PuLuMapSetLuActive(this->upPuP, this->upLuNumber);
      this->subState &= ~UpRqstInactive;
      LogForLuWork(this, WORK_ACTLU);
      break;
     
    /*---- uActlu. Send actlu +ve response upstream, send notify 
           upstream. */
    case a3:  
      SendActluRsp(this, clsiBfrP);
      PuLuMapSetLuActive(this->upPuP, this->upLuNumber);
      LogForLuWork(this, WORK_NOTIFYAVAIL);
      break;
    
    /*---- dActluRspPA. Actlu +ve response from downstream. Therefore, 
           send a notify available upstream. Free the actlu response
           buffer and clear the flag to indicate that notify response
           should not be forwarded downstream. */
    case a4:    
      this->subState &= ~OweNtfyRsp;  
      AckBuffer(this);
      LogForLuWork(this, WORK_NOTIFYAVAIL);
      DspuCLSIMsgDestroy(clsiBfrP);
      break;
     
    /*---- uDactlu and downstream lu is inactive. Dactlu request from 
           upstream. Send dactlu +ve response upstream. */
    case a5:  
      SendPosRsp( this->upPuP, clsiBfrP);
      PuLuMapSetLuInactive(this->upPuP, this->upLuNumber);
      this->subState |= UpRqstInactive;
      break;
     
    /*---- uDactlu and downstream lu is active. Send uResetLu to
           LU FSM. Send dactlu +ve response upstream. Send dactlu
           request downstream. */
    case a6: 
      LuFsm(this, uResetLu, 0);
      SendPosRsp( this->upPuP, clsiBfrP);
      PuLuMapSetLuInactive(this->upPuP, this->upLuNumber);
      LogForLuWork(this, WORK_DACTLU);
      this->subState |= UpRqstInactive;
      
      if (this->subState & LuActivated){    /* was lu activated before ? */
      
         /* log a message to the user */
         
         this->subState &= ~LuActivated;
         DspuActivationTrace(this->debugLevel, LuDeactivatedTrace, this->upPuP,
                               GetPuName(this->upPuP), this->upLuNumber);
      }
      break;
     
    /*---- Notify -ve response from host. 
           Send dactlu downstream. */
    case a7:    
      DspuCLSIMsgDestroy(clsiBfrP);
      AckBuffer(this);
      LogForLuWork(this, WORK_DACTLU);
      errmsg(&msgsym(NotifyNegRsp, DSPU), this->upLuNumber, 
             GetPuName(this->upPuP));
      break;
     
    /*---- Other SSCP_LU or LU_LU when LU is not available. Send -ve
           response upstream if able to change the to buffer to -ve 
          response, otherwise free the buffer. */
    case a8:    
      SendNegRsp( this->upPuP, clsiBfrP, NoSession, 0);  
      break;
     
    /*---- Other SSCP_LU data and LU is active. Forward buffer
           downstream. */
    case a9:   
      SendLuDown(this, clsiBfrP);
      break;
    
    /*---- dStopLu, signal downstream pu is inactive. Free the buffer,
           send dResetLu to LU FSM and send notify unavailable upstream. */ 
    case b10:
      AckBuffer(this);
      /* fall down to a10*/
      
    /*---- dStopLu, signal downstream pu is inactive and LU is available.
           Send dResetLu to LU FSM and notify unavailable upstream. */
    case a10:   
      LuFsm(this, dResetLu, 0);
      LogForLuWork(this, WORK_NOTIFYUNAVAIL);
      this->subState |= DownRqstInactive;;
      
      if (this->subState & LuActivated){    /* was lu activated before ? */
      
         /* log a message to the user */
         
         this->subState &= ~LuActivated;
         DspuActivationTrace(this->debugLevel, LuDeactivatedTrace, this->upPuP,
                               GetPuName(this->upPuP), this->upLuNumber);
      }
      break;
     
    /*---- Both a11 and a4 changed state to uLuPendAvail. In a11,
           NotifyAvail is received from downstream. So send NotifyAvail
           upstream. When the upstream sends a +ve NotifyAvail response,
           then send a +ve NotifyAvail response downstream. */
    case a11:   
      this->subState |= OweNtfyRsp; /* Notify response required */
      this->sequenceNumber = ReturnSequenceNumber(clsiBfrP);
      LogForLuWork(this, WORK_NOTIFYAVAIL);
      DspuCLSIMsgDestroy(clsiBfrP);
      break;
     
    /*---- dNotifyUnavl, Notify unavailable from downstream. Send
           dResetLu to LU FSM. Send notify unavailable upstream. 
           Send notify unavailable +ve response downstream. */
    case a12:   
      LuFsm(this, dResetLu, 0);
      LogForLuWork(this, WORK_NOTIFYUNAVAIL);
      SendPosRsp( this->downPuP, clsiBfrP);
      
      if (this->subState & LuActivated){    /* was lu activated before ? */
      
         /* log a message to the user */
         
         this->subState &= ~LuActivated;
         DspuActivationTrace(this->debugLevel, LuDeactivatedTrace, this->upPuP,
                               GetPuName(this->upPuP), this->upLuNumber);
      }
      break;
     
    /*---- dOtherSSCPLU,dLuLu when LU is not available. Send
           negative response downstream. */ 
    case a13:   
      SendNegRsp( this->downPuP, clsiBfrP, NoSession, 0);
      break;
     
    /*---- uNtfRspP. We have sent a notify request to the host and now
           we have received the notify response. Pass the notify
           response to the downstream. Send any message saved in a22
           (actlu available or notify available) to the upstream since
           the host has responsed. */  
    case a14:        
      if ((this->subState & OweNtfyRsp) != 0){
         this->subState &= ~OweNtfyRsp;
         SaveSequenceNumber(clsiBfrP,this->sequenceNumber);
         SendLuDown(this, clsiBfrP);
      }
      else
         DspuCLSIMsgDestroy(clsiBfrP);
      
      AckBuffer(this);
      
      if (this->saveMsgAtPending){
         SendLuUp(this, this->saveMsgAtPending);
         this->saveMsgAtPending = 0;
      }
      this->subState |= LuActivated;
      DspuActivationTrace(this->debugLevel, LuActivatedTrace, this->upPuP,
                            GetPuName(this->upPuP), this->upLuNumber);
      break;
     
    /*---- dOtherSscpLu. SSCP_LU request or response from downstream and
           both upstream and downstream LUs are active
           (SSCP_LU traffic can flow in both directions), thus
           forward data to the upstream. */
    case a15: 
      SendLuUp(this, clsiBfrP);
      break;
     
    /*---- uLuLu. An LU_LU request or response from the upstream and
           either LUs active/available or downstream PU becoming inactive.
           In either case send request/response unit to the LU_LU FSM. */
    case a16: 
      moreDecodeNeeded = TRUE;
      break;
     
    /*---- DLuLu. An LU_LU request or response from the downstream and
           LUs active/available. Send request/response unit to the 
           LU_LU FSM. */
    case a17: 
      moreDecodeNeeded = TRUE;
      break;
    
    /*---- uStopLu. A signal from the upstream PU that the PU is inactive. */
    case b18:
      AckBuffer(this);
      /* fall down to a18 */
      
    /*---- uStopLu and LU is active. A signal from the upstream PU
           that it is inactive. Send uResetLu to the LU_LU FSM send 
           dactlu downstream. */
    case a18:  
      LuFsm(this, uResetLu, 0);
      LogForLuWork(this, WORK_DACTLU);
      this->subState |= UpRqstInactive;
      break;
     
    /*---- uStopLu and wait for dactlu response */
    case a19: 
      this->subState |= UpRqstInactive;
      break;
     
    /*---- uNtfyRspN. Notify negative response has been sent from
           upstream and send notify unavailable a second time */
    case a20: 
      AckBuffer(this);  /* PU Buffer Management is UPSTREAM*/
      LogForLuWork(this, WORK_NOTIFYUNAVAIL);
      DspuCLSIMsgDestroy(clsiBfrP);
      break;
     
    /*---- dStartLu. Signal from the downstream PU that it is active
           so, send actlu downstream. */
    case a21: 
      LogForLuWork(this, WORK_ACTLU);
      this->subState &= ~DownRqstInactive;
      break;
     
    /*---- Message for SSCP_LU but host has not responded yet, so 
           save the message and send it in a14 when the host has
           responded. */
    case a22:   
      if (this->saveMsgAtPending) {
         /* if we had a message already - there are 2 possible reasons: */
         /* 1. downstream may have violated Request protocol            */
         /* 2. it is a segment                                          */
         /* Neither of these are likely occurences, but 2 must be more  */
         /* likely.  Given this, we decided to avoid a complex solution */
         /* that would be required for queueing up multiple messages.   */
         /* Instead we forward the currently saved message because the  */
         /* chances are that it will be queued behind the message for   */
         /* which a response is owed, and the response will still be    */
         /* generated before this message is processed at the host.     */

         SendLuUp(this, this->saveMsgAtPending);
      }
      this->saveMsgAtPending = clsiBfrP;
      break;
     

    /*---- NotifyRspP from upstream and we are waiting for it */
    case a23: 
      AckBuffer(this);
      if ((this->subState & DownRqstInactive) == 0)
         SscpLuFsm(this, dStartLu, 0);
      
      DspuCLSIMsgDestroy(clsiBfrP);
      break;
     
    /*---- uStopLu, wait for notify response. */
    case a24: 
      /* instead of getting notify response, we had a uStopLu. */
      this->subState |= UpRqstInactive;
      AckBuffer(this);
      if ((this->subState & DownRqstInactive) == 0)
        SscpLuFsm(this, dStartLu, 0);
      break;
     
    /*---- dStartLu, wait for Notify response, dLu inactive */
    case a25:   
      this->subState &= ~DownRqstInactive;;
      break;
    
    /*---- dStopLu, signal from the downstream PU that it is inactive, 
           acknowledge buffer and wait for Notify response */
    case b26: 
      AckBuffer(this);
      /* fall down to a26 */
      
    /*---- dStopLu, signal from the downstream PU that it is inactive
           and wait for Notify response */
    case a26: 
      this->subState |= DownRqstInactive;;
      break;
     
    /*---- dStopLu, wait for dactlu response */
    case a27: 
       /* check if upLu is still inactive, if so, drive the state to Reset 
        */
       AckBuffer(this);
       this->subState |= DownRqstInactive;;
       if ((this->subState & UpRqstInactive) == 0)
          SscpLuFsm(this, uRecovered, 0);
       break;
     
    /*---- A notify response or dactlu is lost, so timeout */
    case a28:  
      AckBuffer(this);
      if ((this->subState & DownRqstInactive) == 0)
        SscpLuFsm(this, dStartLu, 0);
      break;
    
    /*---- A dactlu/actlu/notify response or timeout */
    case a29:  
      if (clsiBfrP)
        DspuCLSIMsgDestroy(clsiBfrP);
      
      AckBuffer(this);
      break;
      
    /*---- Wait and get a dactlu response */ 
    case a30:   
      DspuCLSIMsgDestroy(clsiBfrP);
      AckBuffer(this);
      if ((this->subState & UpRqstInactive) == 0)
        SscpLuFsm(this, uRecovered, 0);
      break;
    
    /*---- Inactivity timeout so send notify unavailable upstream */
    case a31:     
      LuFsm(this, dResetLu, 0);
      LogForLuWork(this, WORK_NOTIFYUNAVAIL);
      break;
      
    /*---- wait and get a notify response or timeout at inactivity 
           timeout state */
    case b32:      
      if (clsiBfrP)
         DspuCLSIMsgDestroy(clsiBfrP);
      AckBuffer(this);
      /* fall down to a32 */

    /*---- LuInactTimeout, LU inactivity timeout and lu not available,
           so send  dactlu downstream*/
    case a32:    
      LogForLuWork(this, WORK_DACTLU);
      break;
      
    /*---- dactlu response from downstream, LU time out or
           signal from downstream PU that it is inactive. 
           So, disconnect the LU from the downstream PU and 
           put the LU in the lu wait list. Get the next available
           LU in the wait list and send dStartLu to the SSCP_LU
           FSM. */
    case a33:  
      {
      Lu* luP;
      Pu* dPuP = this->downPuP;
      byte lu  = this->downLuNumber;  /* save the PU and LU id before 
                                       * LuDisconnect destroy it
                                       */
      if (clsiBfrP)
         DspuCLSIMsgDestroy(clsiBfrP);
      AckBuffer(this);
      if ((this->subState & UpRqstInactive) == 0)
        SscpLuFsm(this, uRecovered, 0);
      
      LuDisconnect(this);   /* disconnect LU from the downstream PU */
      PutLuWait(this->poolP, dPuP, lu);   /* put LU in waiting list */
      
      if ((luP = GetNextLuWait(this->poolP)) != 0)
         SscpLuFsm(luP, dStartLu, 0);         /* start this lu */
      break;
      } 


    /*---- Notify (UNAVAIL) from downstream.
           Some devices may send Notify (UNAVAIL) immediately after
           +RSP ACTLU (UNAVAIL).
           Just return +RSP to downstream so session doesn't clag.
           Also return +RSP to Notify (AVAIL) when we are already
           in available state - although we haven't met the device
           that does this.
           Also, action is generic enough to use as a default action
           when we get a NOTIFY in a context we don't expect. */
    case a39:
      {
      ChangeMuToPosRsp(clsiBfrP);
      SendPu(this->downPuP, clsiBfrP);
      break;
      }

 	/*---- ACTLU received while waiting for Notify Unavailable response.
           So we no longer expect a Notify response - respond to the
           ACTLU, and check to see if the downstream LU has been restarted
           in the meantime (Note that it could be a new downstream LU). */
    case a40:
	  {
      AckBuffer(this);
      SendActluRsp(this, clsiBfrP);
      PuLuMapSetLuActive(this->upPuP, this->upLuNumber);
      if ((this->subState & DownRqstInactive) == 0)
         SscpLuFsm(this, dStartLu, 0);
	  break;
	  }

	/*---- DACTLU received while waiting for Notify Unavailable response.
           So we no longer expect a Notify response - respond to the
           DACTLU, and check to see if the downstream LU has been restarted
           in the meantime (Note that it could be a new downstream LU). */
    case a41:
	  {
      AckBuffer(this);
      SendPosRsp( this->upPuP, clsiBfrP);
      PuLuMapSetLuInactive(this->upPuP, this->upLuNumber);
      this->subState |= UpRqstInactive;
      if ((this->subState & DownRqstInactive) == 0)
         SscpLuFsm(this, dStartLu, 0);
	  break;
	  }


	/*---- DACTLU received while waiting for Notify Unavailable response
           during a timeout sequence.
           So we no longer expect a Notify response - respond to the
           DACTLU, and proceed with the timeout sequence. */

    case a42:
	  {
      AckBuffer(this);
      SendPosRsp( this->upPuP, clsiBfrP);
      PuLuMapSetLuInactive(this->upPuP, this->upLuNumber);
      this->subState |= UpRqstInactive;
      LogForLuWork(this, WORK_DACTLU);
      break;
	  }

  }

  /* To avoid any possible "leakage" of work buffers, free any work
     buffers any time we are in the reset state.                   */
  if (this->sscpLuState == Reset)
     AckBuffer(this);

  return moreDecodeNeeded;
}



/*=====================================================================*
* LuFlowControl
*======================================================================*
*
* Description:
*   If the flow type is off, it sets the appropriate FlowOff flag 
* (upstream or downstream). If the flow type is on, it resets the
* FlowOff flag. It then calls PuFlowControlFromLu in the "partner"
* pu. 
*   Note that these flags must be maintained in a re-entrant manner 
* to allow this procedure to be called on an interrupt level.
*
**/
void LuFlowControl(Lu* this, hword flow)
{
   Pu* partnerPuP;
   FlowControlE flowControl = (flow & FlowOffMask);
   
   if ((flow & FlowUpstream) != 0){
      partnerPuP = this->downPuP;
      if (partnerPuP == 0)
          return;         /* for pool lu, there is no downPuP */

      if (flowControl == FlwCtl_Stop)
         this->flowControl |= UpFlowOff;  
      else
         this->flowControl &= ~UpFlowOff;
   }
   else {
      partnerPuP = this->upPuP;
      if (flowControl == FlwCtl_Stop)
         this->flowControl |= DownFlowOff;
      else
         this->flowControl &= ~DownFlowOff;
   }
         
   /* pass flow control to pu */
   PuFlowControlFromLu(partnerPuP, flowControl);
}

/*=====================================================================*
* LogDddlu
*======================================================================*
*
* Description:
*   Log for NMVT_PSID work and request PU to send it. If PU is free to
* send, it will call QueryLuSendNmvt to send the nmvt
*
**/
void LogDddlu(Lu* this)
{
   this->subState |= LuNmvtPsig;
   PuRequestSentNmvt(this->upPuP);
}


/*=====================================================================*
* ShowLu
*======================================================================*
*
* Description:
*   This displays the LU details.
*
**/
void ShowLu(Lu* this, LinkType upDown)
{
   if (this->usage == luInUseByLuApi)
      LuApiShowLu (this);
      
   else {   /* DSPU LU */
      char  name[9];
      int   i;
      byte  myLu, peerLu;
      hword framesSent, framesReceived;

      memset(name, 0, 9);
      if (upDown == Upstream){
         framesSent = this->downFramesReceive;
         framesReceived = this->upFramesReceive;
         if (this->downPuP)
            strcpy(name,GetPuName(this->downPuP));
         myLu = this->upLuNumber;
         peerLu = this->downLuNumber;
      }
      else {
         framesSent = this->upFramesReceive;
         framesReceived = this->downFramesReceive;
         strcpy(name,GetPuName(this->upPuP));
         myLu = this->downLuNumber;
         peerLu = this->upLuNumber;
      }
      for (i = 7; i > 0; i--){
         if (name[i] == 0)
            name[i] = ' ';  /* fill up trailing blank to make 8 char */
         else
            break;
      }
      if (this->downPuP){
         printf("     LU %03d PEER PU %s PEER LU %03d status %s\n",
                myLu,name, peerLu, luStatus[this->sscpLuState]);
         printf("            FRAMES RECEIVED %d FRAMES SENT %d\n",
                framesReceived, framesSent);
      }
      else
         PrintLuInPool(myLu, this->poolP? GetPoolName(this->poolP): "");
   }
}

const char* LuStatus(Lu* this)
{
    return luStatus[this->sscpLuState];
}

/*=====================================================================*
* DebugLu
*======================================================================*
*
* Description:
*   Set the Lu debug level on
*
**/
void DebugLu (Lu* this, hword debugLevel)
{
  this->debugLevel |= (byte) debugLevel;
}

/*=====================================================================*
* NoDebugLu
*======================================================================*
*
* Description:
*   Set the Lu debug level off
*
**/
void NoDebugLu (Lu* this, hword debugLevel)
{
  this->debugLevel &= (byte) ~debugLevel;
}

/*=====================================================================*
* LuActivityTimeout
*======================================================================*
*
* Description:
*   This is called by the PoolCheckTimer every minute to check 
* if the activity timer has expired for the given lu. If so,
* then the SSCP-LU FSM is invoked with LuInactTimeout.
*   
**/
void LuActivityTimeout(Lu* this, byte timeoutValue)
{
  if (this->downPuP &&   /* Ensure the lu is connected to the downstream */
     (this->luActivityTimer++ >= timeoutValue))
    SscpLuFsm(this, LuInactTimeout, 0);  /* Tell FSM the lu has timed out */
}


/*=====================================================================*
* QueryLuLuActive()
*======================================================================*
*
* Description:
*   Returns state of LU-LU session (active/inactive)
*
**/
boolean QueryLuLuActive (Lu* luP)
{
  return (luP->luLuState != LuReset);
}

/*=====================================================================*
* CheckLuOperStateChanged()
*======================================================================*
*
* Description:
*   Returns TRUE if snaLuOperState has changed
*   Generates SNMP trap when snaLuOperState has changed
*
**/
boolean CheckLuOperStateChanged(Lu* luP, boolean newLuState)
{

  boolean currentLuOperState;
  boolean newLuOperState;

  boolean luOperStateChanged = FALSE;


  currentLuOperState = GetSnaLuOperState(Upstream, luP->sscpLuState);
  newLuOperState = GetSnaLuOperState(Upstream, newLuState);
  if (currentLuOperState != newLuOperState) {
    luOperStateChanged = TRUE;
    DspuNetMgmtNotification((void *) luP, LU_STATECHANGE, 
                             newLuOperState, Upstream);
  }
  else {
    currentLuOperState = GetSnaLuOperState(Downstream, luP->sscpLuState);
    newLuOperState = GetSnaLuOperState(Downstream, newLuState);
    if (currentLuOperState != newLuOperState) {
      luOperStateChanged = TRUE;
      DspuNetMgmtNotification((void *) luP, LU_STATECHANGE, 
                               newLuOperState, Downstream);
    }
  }

  return(luOperStateChanged);

} /* End CheckLuOperStateChanged() */

static char* const luInput[] = {
   "uActlu",         /* ACTLU reqeust from host                    */
   "uDactlu",        /* DACTLU request from host                   */
   "uNtfyRspP",      /* Notify +ve response from "upstream         */
   "uNtfyRspN",      /* Notify -ve response from "upstream         */
   "uOtherSscpLu",   /* other SSCP_LU  sna from "upstream          */
   "uLuLu",          /* other LU_LU sna from "upstream             */
   "uStopLu",        /* Signal from "upstream that PU is inactive  */
   "dStartLu",       /* A signal from "ds that PU is active        */
   "dStopLu",        /* A signal from "ds that PU is inactive      */
   "dActluRspPA",    /* ACTLU +ve available                        */
   "dActluRspPN",    /* ACTLU +ve not available                    */
   "dActluRspN",     /* ACTLU -ve                                  */
   "dDactluRsp",     /* "DACTLU rsp (+ve or -ve) from "ds          */
   "dNtfyAvail",     /* Notify (available) from "ds                */
   "dNtfyUnav",      /* Notify (unavailable) from "ds              */
   "dOtherSscpLu",   /* SSCPLU or LU_LU reqeust/rsp from "ds       */
   "dLuLu",          /* other LU_LU sna from "downStream           */
   "uRecovered",     /* internal signal for pending state          */
   "luTimeout",      /* LU timeout                                 */
   "LuInactTimeout"  /* no activity time out                       */
};

/*=====================================================================*
* QueryLuApiLu
*======================================================================*
*
* Description:
*   Returns TRUE if the LU is allocated as an LUAPI LU.
*
**/
boolean QueryLuApiLu(Lu* this) {
   return (this->usage == luInUseByLuApi);
}
