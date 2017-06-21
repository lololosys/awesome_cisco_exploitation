/* $Id: dspu_pu.c,v 3.8.6.6 1996/08/28 12:42:06 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_pu.c,v $
 *------------------------------------------------------------------
 * DSPU PU Support.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * * This includes all the support for PUs, including the SSCP-PU FSMs
 * for both PU Type 2 and PU Type 5.
 *------------------------------------------------------------------
 * $Log: dspu_pu.c,v $
 * Revision 3.8.6.6  1996/08/28  12:42:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.6.5  1996/07/29  21:46:59  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/06/21  02:22:05  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.8.6.3  1996/05/27  06:01:36  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/05/17  10:47:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.11  1996/05/05  22:55:29  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.10  1996/05/03  17:18:20  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.9  1996/04/24  00:50:12  lcox
 * Branch: IbuMod_Calif_branch
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.7.2.8  1996/04/22  03:48:23  lcox
 * CSCdi55251:  change DspuMalloc(of Pu)/malloc to malloc_named call
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.7  1996/04/22  00:06:37  lcox
 * CSCdi55112:  mem corruption on ACTPU when NMVT rsp owed downstream
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.6  1996/04/15  05:58:31  lcox
 * CSCdi53314:  missing focalpt init for PU with DDDLU cfgd
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.7.2.5  1996/04/09  22:36:31  lmetzger
 * Update of Initial Version of Response Time Reporter
 *
 * Revision 3.7.2.4  1996/04/03  14:02:54  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.2.3  1996/03/28  08:20:29  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.7.2.2  1996/03/22  18:50:26  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.7.2.1  1996/03/17  17:38:38  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.8.6.1  1996/03/18  19:33:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.3  1996/03/16  06:40:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.6.2  1996/03/07  08:45:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.6.1  1996/02/20  00:47:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/05  03:25:29  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.7  1996/02/26  19:04:14  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Insert CV 0x0E and CV 0x10 into XID3
 *
 * Revision 3.6  1996/01/22  23:10:01  lcox
 * CSCdi47250:  correct reset for actpu to active pu with DDDLU support
 *
 * Revision 3.5  1996/01/10  01:13:45  lcox
 * CSCdi46175:  bad sense code sent on neg rsp to Req RTM NMVT
 *
 * Revision 3.4  1995/11/20  16:40:56  ppearce
 * CSCdi43891:  %ALIGN-3-SPURIOUS messages
 *   Check puP before accesing fields of structure
 *
 * Revision 3.3  1995/11/20  16:37:32  ppearce
 * CSCdi43278:  dspu start and sna start commands disappear from config
 *
 * Revision 3.2  1995/11/17  09:04:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:56:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/19  21:12:55  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/09/20  17:03:06  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 
/* The template is used to defined proper CPS type 
 * and timer object type.
 */
#define PU_TEMPLATE

#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>

#include "dspu.h"
#include "dmalloc.h"    /* malloc */

#include "dspu_pu.h"
#include "dspu_fp.h"
#include "dspu_itimer.h"
#include "dspu_lu.h"
#include "dspu_puutil.h"
#include "dspu_pool.h"
#include "dspu_alert.h"
#include "dspu_fsmstr.h"
#include "dspu_ls.h"
#include "snaprim.h"
#include "snautil.h"
#include "snapiu.h"
#include "dspu_error.h"
#include "dspumib_mgmt.h"
#include "dspu_util.h"
#include "dspu_adaptor.h"
#include "dspu_sap.h"
#include "dspu_notification.h"


#define RetryConnectOutTime 3
static int retryConnectOutTimer;


extern char* const downPuStatus[];
extern char* const upPuStatus[];
static char* const t5Input[];
static char* const t2Input[];


/*=====================================================================*
* ResetLuMap
*======================================================================*
*
* Description: 
* 
* Initialise/Reset lu map to record all LUs as inactive.
*
**/
static void ResetLuMap (LuMap* map)
{
   int i;
   if (map)
      /* reset bit map */
      for (i = 0; i < 8; i++)
         map->active[i] = 0; 
}


/*=====================================================================*
* Pu_nw, 
*======================================================================*
*
* Description: 
* 
* PU constructor. 
*
* The PU can have one of the 4 properties up/down inactive/active. 
* Normally, the system wants to know if a pu is inactive only when an 
* incoming connect requires a PU search. So we had a Pu active and inactive
* queue to simplify the search. Also when a pu is inactive, it does not 
* have a link station.
* A default PU is a template only.
* 
*
**/


Pu* Pu_nw(
     LinkType puType,        /* Upstream or Downstream */
     const char*     puName, /* PU name                */
     hword dlci,             /* DLCI number            */
     byte  sdlc,             /* SDLC Address           */
     const char* x25,        /* X.25 Address           */
     const char* rMac,       /* Remote MAC Address     */
     byte  rSap,            
     byte  lSap,
     idbtype *idb,    
     byte  windowSize,
     hword maxIframe,
     word  xid,
     byte  maxRetryCount,
     hword maxRetryTimeout)
{
   Pu* this;
   LuMap* map = (LuMap*) NULL;

   if (puType == Upstream) {
      map = malloc_named(sizeof(LuMap), "DSPU PU LUmap");
      if (map) {
         this = malloc_named(sizeof(Pu), "DSPU UP PU");
         if (!this)
            free (map);
      }
      else
         this = (Pu*) NULL;        
   }
   else
      this = malloc_named(sizeof(Pu), "DSPU DN PU");

   if (this) {
      hword index;
      this->luMap = map;
      ResetLuMap(this->luMap);
      this->puChildLocP = puType == Upstream? &dspu.upstrPuRoot:  GetInactDpuRoot();
      /* if puName is not defined, then it must be default PU, dont put it in the list */
      index = puName? AddChildFirstFree(this->puChildLocP,this, 0,  &this->lsP): 0;
      if (index !=  BAD_SIBLING_INDEX) {
         this->puIndex =  index;
         InitChild(MULTIPLE, &this->luChildren);
         this->puType = puType;
         this->debugLevel = (byte)GetDebugLevel();

         this->cfgConnParms.windowSize = windowSize;
         this->cfgConnParms.maxIFrame = maxIframe;
         this->negConnParms.windowSize = 0;
         this->negConnParms.maxIFrame = 0;
         this->retryInfo.maxCount = maxRetryCount;
         this->retryInfo.maxTimeout = maxRetryTimeout;
         this->timer = 0;

         DspuQueueInit(&this->poolRangeHeader);
         if (puName) {
            /* copy pu name, ensure pu name is in upper case */
            char* p = (char*) this->name.name;
            char c; int i = 0;
            while ( ((c = *puName++) != 0) && (i++ < MAX_ENTRYNAME_LN)) {
               if (c >= 'a' && c <= 'z')
                  c = c - 'a' + 'A';
               *p++ = c;
            }
            *p = 0;
         }
         else
            this->name.name[0] = 0;

         snpa_clear(GetPuAddrP(this));
         this->lSap      = NoSap;
         this->remoteSap = NoSap;

         if (dlci) {
             uchar temp[2];
             temp[0] = dlci >> 8;
             temp[1] = dlci & 0xFF;
             snpa_init(GetPuAddrP(this), STATION_FR10, 
                           TARGET_UNICAST, (uchar *) &temp);
             this->lSap = lSap;
             this->remoteSap = rSap;
         } else if (sdlc) {
             snpa_init(GetPuAddrP(this), STATION_SDLC, 
                           TARGET_UNICAST, (uchar *) &sdlc);
         } else if (x25) {
             snpa_init(GetPuAddrP(this), STATION_X121, 
                           TARGET_UNICAST, (uchar *) x25);
             this->lSap = lSap;
         } else if (rMac) {
             snpa_init(GetPuAddrP(this), STATION_IEEE48, 
                           TARGET_UNICAST, (uchar *) rMac);
             this->lSap = lSap;
             this->remoteSap = rSap;
         }
      
         this->configuredIdb = idb;

         this->sscpPuFsmState = T2Reset;
         this->adaptorId.dlcType = DSPU_DLC_UNDEFINED;
         this->xid.xid = xid;
         this->rcvdXidFmt = xidFmt_Bad;               /* rcvdXidFmt unknown */
         this->rcvdXid.xid = NoXid;
         this->byteSents = 0;
         this->frameSents = 0;
         this->byteReceives = 0;
         this->frameReceives = 0;
         this->puFlag = GetSnaMode()? ConstructedBySna:0;
         this->isDefault = FALSE;
         this->puWorkBits = 0;

         this->lastSscpPuSnf = 0;

         this->negativeRspSents = 0;
         this->negativeRspReceives = 0;

         this->actFailCnt = 0;
         this->lastActFailRsn = PuActFailRsn_None;

         if (puName)
             AddEntry_DspuPuTable(this);
        
         TIMER_STOP(this->startTime);
         TIMER_STOP(this->stateChgTime);
      }
      else {
         free (this);
         this = (Pu*) NULL;
         if (map)
           free(map);
      }
   }
   return this;

}  /* Pu_nw. */


/*=====================================================================*
* Pu_dl
*======================================================================*
*
* Description:
*    This is destructor is called when a default PU is disconnected or at 
* the configuration at NO DSPU PU statement. All lu connection had been 
* ensured reset before this routine is called.
*/

void Pu_dl(Pu* this)
{

   PoolRange* pRP;
   LinkStn* lsP;


   /* Set Rqd state of PU to inactive, pending deletion of PU                */
   /*   to ensure PU does not try to do work while its LUs are being deleted */
   /* ---------------------------------------------------------------------- */
   this->puFlag |= RqdPendDelete;

   while ((pRP = DspuDequeue(&this->poolRangeHeader)) != 0)
      PoolRange_dl(pRP);
   
   /*---- Dissociate LS<->PU linkage; & destroy LS. */
   lsP = this->lsP;                     /* DelChild trashes this->lsP! */
   DelChild(this->puChildLocP, this->puIndex, &this->lsP);
   
   if (this->puType == Downstream)
      /* disconnect the lu from the PU if the lu is not dedicated */
        DoToAllChildren(&this->luChildren, LuDisconnect);

   /* Now delete all the lu. For downstream only those dedicated lu will be still
    * in the PU control block,now we can call LuDelete to delete the dedicated lu.
    */
  
   DoToAllChildren(&this->luChildren,  LuPuChildDelete);
   
  if (lsP)
      LsFsm (lsP, LsI_PuStopped, 0);

  
  Fp_PuDeleted(this);         /* tell focal point that pu is deleted        */


  RemoveEntry_DspuPuTable(this);

  if (this->luMap)
    free(this->luMap);

  free (this);

}

/*=====================================================================*
*  DefaultPu_nw
*======================================================================*
*
* Description:
*   This construtor is called when an incoming connect match the default 
* PU. It copies all default PU parameters
*/

Pu* DefaultPu_nw(
     LinkStn* lsP,          /* link station parent */
     hwaddrtype* hwPuAddrP,  
     byte  lSap,
     byte  rSap,
     word  xid
     )
{
   PoolRange* poolP;
   Pu* defaultP = GetDefaultPu();
   Pu* this = malloc_named(sizeof(Pu), "DSPU DF PU");
   if (this) {
      memcpy(this, defaultP, sizeof(Pu));   /* inherit all from default */
      
      /* construct new pool range header. This is necessary because pool
       * range can be changed any time.
       */
      DspuQueueInit(&this->poolRangeHeader);    /* clear pool range */
      poolP = GetFirstQelement(defaultP->poolRangeHeader);
      while (poolP)
      {
         PoolRange_nw(this, poolP->luStart,
                               poolP->luRange, poolP->poolName);
         poolP = poolP->next;
      }
      
      this->isDefault = TRUE;
      this->xid.xid = xid;

      /* take on current dspu debug level */
      this->debugLevel = (byte)GetDebugLevel();

      this->negConnParms.windowSize = this->cfgConnParms.windowSize;
      this->negConnParms.maxIFrame  = this->cfgConnParms.maxIFrame; 
      
      snpa_clear(GetPuAddrP(this));
      this->lSap = NoSap;
      this->remoteSap = NoSap;

      if (snpa_is_valid(hwPuAddrP)) {
          snpa_copy(hwPuAddrP, GetPuAddrP(this));
          if (snpa_get_type(hwPuAddrP) != STATION_SDLC) {
              this->lSap = lSap;
              this->remoteSap = rSap;
          } 
      } 

      this->puChildLocP = lsP? GetPuChildren(lsP): GetInactDpuRoot();
      this->puIndex =  AddChildFirstFree(this->puChildLocP,this, lsP, &this->lsP);

      AddEntry_DspuPuTable(this);
   }
   return this;

}  /* DefaultPu_nw. */


/*=====================================================================*
* PuChangeToActive 
*======================================================================*
*
* Description:
*   Downstream PU is changed to active.   
* 
*/
void PuChangeToActive(Pu* this, LinkStn* lsP)
{
   this->puFlag |= PuStartRqsted;   /* Start had been recorded */
   DelChild(this->puChildLocP, this->puIndex, &this->lsP);
   this->puChildLocP = GetPuChildren(lsP);
   this->puIndex =  AddChildFirstFree(this->puChildLocP,this, lsP, &this->lsP);
   DebugLs(lsP, this->debugLevel); /* set ls to pu's debuglevel */
}


/*=====================================================================*
*  PuSendXidReq, PuSendXidRsp: 
*======================================================================*
*
* Description:
*   Get either PU to send XID(3) Req/Rsp   
* 
*/

IdReq_t*
PuSendXidReq (Pu *this, boolean xid3negotiation)
{
  return Build_Xid3_Req(this->xid.blk_num, GetPuName(this),
                          this->cfgConnParms.maxIFrame, 
                                this->cfgConnParms.windowSize,xid3negotiation );
}

IdRsp_t*
PuSendXidRsp (Pu *this)
{
  return Build_Xid3_Rsp(this->xid.blk_num, GetPuName(this),
                          this->cfgConnParms.maxIFrame, 
                                this->cfgConnParms.windowSize);
}



/*=====================================================================*
* PuActivationRetry
*======================================================================*
*
* Description:
*    Pu connect out fail, now do a retry after 3 seconds time out
*
**/

static void PuActivationRetry (Pu* this)
{
   if ((this->puFlag & PuStartRqsted) != 0 && IsPuStarted(this) && 
                                   IsPuRetryCountInfinite(this) ){
      switch( snpa_get_type(GetPuAddrP(this)) ) {
           case STATION_SDLC:
               PuActivateSDLC(this);
               break;
           case STATION_X121:
               PuActivateQLLC(this);
               break;
           case STATION_IEEE48:
           case STATION_FR10:
               PuActivateLLC(this);
               break;
           default:
               break;
         } /* End switch(addressType) */

     } /* End if - Activation retry */
}

/*=====================================================================*
* PuReconnectCheck
*======================================================================*
*
* Description:
*    This is called every second to check if reconnect is required
*
**/

void PuReconnectCheck (void)
{
   if (retryConnectOutTimer && --retryConnectOutTimer == 0){
      DoToAllChildren(GetInactUpuRoot(),  (ProcChild)PuActivationRetry);
      DoToAllChildren(GetInactDpuRoot(),  (ProcChild)PuActivationRetry);
   }
}

/*=====================================================================*
* ParentLinkStationClosed
*======================================================================*
*
* Description:
*  This is called when the Link Station is closed(destructed). 
*    if pu is default, then delete the Pu else move pu to the
* inactive list and do a reset.
*
**/
void
ParentLinkStationClosed (Pu* this)
{
  if (this->isDefault) 
    Pu_dl( this);       
  else {

     /*---- Add (the once) active pu onto inactive-list. */
     this->puChildLocP = (this->puType == Upstream) ?
                         GetInactUpuRoot() : GetInactDpuRoot();
     this->puIndex =  AddChildFirstFree (this->puChildLocP, this,
                                               0, &this->lsP);
     /*---- For downstream pu's only: disconnect all lu's from this pu. */
     if (this->puType == Downstream){
        DoToAllChildren(&this->luChildren, LuDisconnect);
        T5sscpPuFsm(this, ResetPu, 0);
     }
     else
        T2sscpPuFsm(this, T2ResetPu, 0);

     if ((this->puFlag & PuStartRqsted) != 0)  /* if connect out requested */
        retryConnectOutTimer = this->retryInfo.maxTimeout;  /* set up retry time */

     Fp_GoingDown(this);     /* tell focal point that the pu is now inactive */
  }  
}  /* ParentLinkStationClosed. */



/*=====================================================================*
* ReceivePu
*======================================================================*
*
* Description:
*   This is called by the link station to pass data packet from CLSI
* interface.
*
**/

void ReceivePu(
       Pu*        this,
       DataInd_t* indBfrP       /* clsi buffer for data          */
       )
{
   SNASessionFlow sessionFlow;
   SNAMU          snaMu;
   boolean        freeBuffer;
   hword          fsmInput;
   Lu*            luP;
   hword          len;
   
   DataReq_t* clsiBfrP = Indication2Request(indBfrP);
   
   if ((this->debugLevel & TraceIframeTraffic) != 0){
      char  dataBuffer[MAX_IFRAME_TRC_BUFFER_LN];
      memset(dataBuffer, '\0', sizeof(dataBuffer));
      len = GetDataLength(clsiBfrP);
      GetDataBufferInHexAscii(ReturnDataHandle(clsiBfrP), dataBuffer, len);
      DspuIframeTrace(IframeTraceNum, "Rx", this, len, dataBuffer);
   }
   
   this->frameReceives++;
   this->byteReceives += GetDataLength(clsiBfrP);
   DecodeSession(clsiBfrP, &sessionFlow);
   
   /* find a session */
   
   if (!DecodeMu(clsiBfrP, &snaMu)){    
      /* Bad sna data, cannot decode 
       */
      byte* snaData = ReturnDataHandle(clsiBfrP);
      len = GetDataLength(clsiBfrP);
      if (len > 9)
         len = 9;
      errmsg(&msgsym(SnaBadData, DSPU), this->name.name,
              ByteToHexAscii(snaData,len));
      freeBuffer = TRUE;
   }
   else if (snaMu.reqRsp == SNA_INCOMPLETE_RH) {
      /* Incomplete RH. Send negative response with system 
       * sense data 4005 - Incomplete RH.
       */
      if (ChangeMuToNegRsp(clsiBfrP, IncompleteRH, 0)) {
        SendPu(this, clsiBfrP);  /* indication -> request */
        IncrPuNegativeRspSents(this);
        freeBuffer = FALSE;
      }
      else
        freeBuffer = TRUE;  /* Free the clsiBfr */
   }
   else if (snaMu.reqRsp == SNA_INVALID_FID) {
      /* Invalid FID found. Send negative response with system 
       * sense data 8006 - Invalid FID.
       */
      if (ChangeMuToNegRsp(clsiBfrP, InvalidFID, 0)) {
        SendPu(this, clsiBfrP);  /* indication -> request */
        IncrPuNegativeRspSents(this);
        freeBuffer = FALSE;
      }
      else
        freeBuffer = TRUE;  /* Free the clsiBfr */
   }
   else if (snaMu.reqRsp == SNA_RU_LEN_ERROR) {
      /* RU incompete. Send negative response with system 
       * sense data 1002 - RU Length Error.
       */
      if (ChangeMuToNegRsp(clsiBfrP, RULengthError, 0)) {
        SendPu(this, clsiBfrP);  /* indication -> request */
        IncrPuNegativeRspSents(this);
        freeBuffer = FALSE;
      }
      else
        freeBuffer = TRUE;  /* Free the clsiBfr */
    }
    else {
      freeBuffer = FALSE;

      /* Adds to path length for all received frames         -ppearce*/
      /*   may want to move this down so that this check is  -ppearce*/
      /*   not always peformed here                          -ppearce*/
      if (snaMu.reqRsp == SNA_RESPONSE && snaMu.rspType == SNA_NEGATIVE)
        IncrPuNegativeRspReceives(this);

      switch(sessionFlow.sessionType){
         case SSCP_PU:
            if (this->puType == Upstream){
               if (snaMu.reqRsp == SNA_REQUEST){
               
                  /* upstream sna data request type */
                  
                  if (snaMu.ruType == SNA_ACTPU) {
                     if (snaMu.enable) {
                        fsmInput = ActpuDddlu;
                        this->puFlag |= DddluSupported;
		     }
                     else {
                        fsmInput = Actpu;
                        this->puFlag &= ~DddluSupported;
		     }
		  }
                  else if (snaMu.ruType == SNA_DACTPU)
                     fsmInput = Dactpu;
                  else
                     fsmInput = RequestFromHost;
               }
               else {
                  fsmInput = ResponseFromHost;

                  if ( snaMu.ruType == SNA_ACTPU &&
                        snaMu.rspType == SNA_NEGATIVE) {
                       DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                           PuActFailRsn_NegativeRsp, GetPuType(this));
                  }
               }
               T2sscpPuFsm((Pu*)this, fsmInput, clsiBfrP);
            }
            else{
            
               /* down stream PU */
               
               if (snaMu.reqRsp == SNA_REQUEST){
           
                  /* down stream SSCP PU sna request */
                  
                  /* save the seq no in case we need to build a response */
                  this->lastSscpPuSnf = ReturnSequenceNumber(clsiBfrP);
               
                  if (snaMu.ruType == SNA_NMVT || snaMu.ruType == SNA_RECFMS)
                     fsmInput = SupRequestFromPu;
                  else if (snaMu.ruType == SNA_REQDISCONT)
                     fsmInput = ReqDiscon;
                  else
                     fsmInput = UnsupRequestFromPu;
               
               }
               else if (snaMu.ruType == SNA_ACTPU) {

                  fsmInput = (snaMu.rspType == SNA_POSITIVE)?
                           ActpuPosRsp: ActpuNegRsp;

                  if (snaMu.rspType == SNA_NEGATIVE) 
                      DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                             PuActFailRsn_NegativeRsp, GetPuType(this));

               }
               else if (snaMu.ruType == SNA_DACTPU)
                  fsmInput = DactpuRsp;
            
               else
                  fsmInput = ResponseFromPu;
               
               T5sscpPuFsm(this, fsmInput, clsiBfrP);
            }
            break;
            
         case SSCP_LU:
            luP = GetChild(&this->luChildren, sessionFlow.luNumber);
            if (luP == 0)
               ReceiveSscpLuUndef(this,
                                  sessionFlow.luNumber,
                                  clsiBfrP, &snaMu);
            else
               ReceiveSscpLu(luP, this->puType, clsiBfrP, &snaMu);
            break;
         
         case LU_LU:
            luP = GetChild(&this->luChildren, sessionFlow.luNumber);
            if (luP == 0)
               ReceiveLuLuUndef(this, clsiBfrP, &snaMu);
            else
               ReceiveLuLu(luP, this->puType, clsiBfrP, &snaMu);
            break;
            
        case IND_LU:
           if (snaMu.reqRsp == SNA_REQUEST){
              if (ChangeMuToNegRsp(clsiBfrP,FunctionNotSupported,0 )) {
                 SendPu(this,  clsiBfrP);
                 IncrPuNegativeRspSents(this);
              }
              else
                 freeBuffer = TRUE;
           }
           else
              freeBuffer = TRUE;
           break;
         }/*switch*/
   }/*else*/
   
   if (freeBuffer)
      DspuCLSIMsgDestroy(clsiBfrP);
}

           
/*=====================================================================*
* ReceiveNmvtPsidRsp
*======================================================================*
*
* Description:
*   This function is called out of the T2 FSM action a12 when
*   there is a NMVT PSID response to pass down to an LU.
*
**/
static void ReceiveNmvtPsidRsp(
       Pu*          this,
       DataReq_t*   clsiBfrP
       )
{
   Lu* luP;

   if (this->luPendingNmvtPsidRsp) {
      luP = GetChild(&this->luChildren, this->luPendingNmvtPsidRsp);
      if (luP)
         ReceiveLuNmvtPsidRsp(luP, clsiBfrP);
   }
   else
      /* we are the end point, so just get rid of the buffer */
      DspuCLSIMsgDestroy(clsiBfrP);
}
 
 
/*=====================================================================*
* SendPu
*======================================================================*
*
* Description:
*   Send data out. All lu and pu data sent out will call this routine.
*
**/
void SendPu( 
       Pu*        this,
       DataReq_t* clsiBfrP       /* clsi buffer for data          */
       )
{
   hword      offset = 0;
 
   hword maxIframe = (this->negConnParms.maxIFrame == 0)? 
                     this->cfgConnParms.maxIFrame: 
                     this->negConnParms.maxIFrame;
  
   if ((this->debugLevel & TraceIframeTraffic) != 0){
      hword len;
      char  dataBuffer[MAX_IFRAME_TRC_BUFFER_LN];
      memset(dataBuffer, '\0', sizeof(dataBuffer));
      len = GetDataLength(clsiBfrP);
      GetDataBufferInHexAscii(ReturnDataHandle(clsiBfrP), dataBuffer, len);
      DspuIframeTrace(IframeTraceNum, "Tx", this, len, dataBuffer);
   }
   
   if (this->lsP == 0){
       printf("pu state = %d\n", this->sscpPuFsmState);
       DspuCLSIMsgDestroy(clsiBfrP);
       return;
    }
   this->frameSents++;
   this->byteSents += GetDataLength(clsiBfrP);
   do {
      LsSendData(this->lsP,Segmentation(clsiBfrP, maxIframe, &offset));
   } while (offset != 0);
}

/*=====================================================================*
* QueryMatchPuName
*======================================================================*
*
* Description:
*    return TRUE if name matches puName. Note that Cisco does not support
* stricmp.
* This routine assumes the existing puName is upper case.
*   
*
**/
boolean QueryMatchPuName(Pu* this, const char* name)
{
   /* stricmp(this->name.name,name) == 0 */
   char  c;
   char* p = (char*)this->name.name;
   
   while ((c = *p++) != 0){
      if (c != *name && c != (*name + 'A' - 'a'))
         return FALSE;
      name++;
   }
   return *name == 0;
}


/*=====================================================================*
* SendPosRsp
*======================================================================*
*
* Description:
*    Change a request into a positive response and send it.
*    If the Change fails then it is because the request did not
*    request a definite response, so then we free the buffer.
*
**/
static void SendPosRsp (Pu*        this,
                        DataReq_t* clsiBfrP
                       )
{
  if ( ChangeMuToPosRsp(clsiBfrP) )
     SendPu(this, clsiBfrP);
  else
     DspuCLSIMsgDestroy(clsiBfrP);
}

/*=====================================================================*
* SendNegRsp
*======================================================================*
*
* Description:
*    Change a request into a negative response and send it.
*    If the Change fails then it is because the request was sent
*    "no response", so then the buffer is freed.
*
**/
static void SendNegRsp (Pu*        this,
                        DataReq_t* clsiBfrP,
                        hword      systemSense,
                        hword      userSense
                       )
{
  if ( ChangeMuToNegRsp(clsiBfrP, systemSense, userSense) ) {
     SendPu(this, clsiBfrP);
     IncrPuNegativeRspSents(this);
  }
  else
     DspuCLSIMsgDestroy(clsiBfrP);
}


/*=====================================================================*
* T2 FSM table
*======================================================================*/

const static FsmStruct t2sscpPuFsm[MaxT2Input][T2Max] = {

/*                   0                 1                  2                */
/*                 Reset            Active              Busy               */
/*startPu*/       {{0,a0},           {1,a0},              {2,a0}},
/*Actpu*/         {{1,a1},           {1,a2},              {1,a2}},
/*ActpuDddlu*/    {{1,a3},           {1,a4},              {1,a4}},
/*Dactpu*/        {{0,a5},           {0,a6},              {0,a6}},
/*RqstFromHost*/  {{0,a7},           {1,a8},              {2,a8}},
/*RqstToHost*/    {{0,a9},           {2,a10},             {2,a0}},
/*RespFromHost*/  {{0,a0},           {1,a12},             {1,a12}},
/*ResetPu*/       {{0,a14},          {0,a13},             {0,a13}},
/*ResetSignal*/   {{0,a0},           {1,a0},              {2,a0}},
/*SendRqstToHost*/{{0,a0},           {1,a15},             {2,a0}}
};

/*=====================================================================*
* T2sscpPuFsm
*======================================================================*
*
* Description:
*   Upstream PU FSM
*
**/
void T2sscpPuFsm(
             Pu* this, 
             T2Input  fsmInput, 
             DataReq_t* clsiBfrP)
{
   const FsmStruct* t2Fsm = &t2sscpPuFsm[fsmInput][this->sscpPuFsmState];

   /* Check if dspuPuOperState has changed                               */
   /*   PuStateChangeTrap will be generated from within Check() routine  */
   if (this->sscpPuFsmState != t2Fsm->state) {
       CheckPuOperStateChanged(this, t2Fsm->state);
       DspuFsmChgTrace(this->debugLevel, PuFsmChgTrace, this, 
                         t2Input[fsmInput],
                         upPuStatus[this->sscpPuFsmState], 
                         upPuStatus[t2Fsm->state]);
   }
   this->sscpPuFsmState = t2Fsm->state;
   DspuFsmTrace(this->debugLevel, PuFsmTrace, this,
                  fsmInput, t2Fsm->state, t2Fsm->action);
   
   switch(t2Fsm->action){
     word senseCode;
     case a0:
        if (clsiBfrP)
           DspuCLSIMsgDestroy(clsiBfrP);
        break;
    
     case a1:    /* Actpu received and pu not active */
        /* send Cold Actpu positve response          */
        
        ChangeActpuToPosRsp(clsiBfrP, this->name.name, COLD);
        SendPu(this,  clsiBfrP); /* indication -> request */
        SetPuStartTime(this);
        DspuActivationTrace(this->debugLevel, PuActivatedTrace, this,
                              GetPuName(this), 0);
        Fp_HostPuUp(this);      /* this pu could be focal point */
        break;
     
     case a2:   /* actpu received and pu is active or busy */
        /* Hierachically reset all children LU and send Erp Actpu rsp */
        
        DoToAllChildren(&this->luChildren, UResetLu);
        ResetLuMap(this->luMap);
        ChangeActpuToPosRsp(clsiBfrP, this->name.name, ERP);
        SendPu(this,  clsiBfrP);
        DspuActivationTrace(this->debugLevel, PuActivatedTrace, this,
                              GetPuName(this), 0);
        break;
     
     case a3:   /* actpu with dddlu received and pu is not active */
     
        /* send Cold Actpu positve response and log all LU to send
         * DDDLU NMVT
         */
     
        ChangeActpuToPosRsp(clsiBfrP, this->name.name, COLD);
        SendPu(this,  clsiBfrP);
        SetPuStartTime(this);
        Fp_HostPuUp(this);      /* this pu could be focal point */
        DoToAllChildren(&this->luChildren, LogDddlu);
        DspuActivationTrace(this->debugLevel, PuActivatedTrace, this,
                              GetPuName(this), 0);
        break;
     
     case a4: /* actpDddlu received and pu is active or busy */
        
        /* Hierachically reset all children LUs, send Erp +ve Rsp
         * log all children LU to send DDDLU NMVT
         */
        DoToAllChildren(&this->luChildren, UResetLu);
        ResetLuMap(this->luMap);
        ChangeActpuToPosRsp(clsiBfrP, this->name.name, ERP);
        SendPu(this,  clsiBfrP);
        DoToAllChildren(&this->luChildren, LogDddlu);
        DspuActivationTrace(this->debugLevel, PuActivatedTrace, this,
                              GetPuName(this), 0);
        break;

     case a5:   /* Dactpu received and pu is not active */
        SendPosRsp(this,  clsiBfrP);
        break;
     
     case a6:   /* Dactpu received and pu is active     */
        SendPosRsp(this,  clsiBfrP);
        DoToAllChildren(&this->luChildren, UResetLu);
        ResetLuMap(this->luMap);
        DspuActivationTrace(this->debugLevel, PuDeactivatedTrace, this,
                              GetPuName(this), 0);
        Fp_GoingDown(this);
        break;
        
     case a7:   /* reqeust from host and PU is inactive */
        SendNegRsp(this, clsiBfrP, NoSession, 0);
        break;
     
     case a8:   /* request from host and PU is active or busy */
        /* check if focal point accept the data */
        if ((senseCode = Fp_ReqFromHost(this,clsiBfrP)) != 0)
           SendNegRsp( this, clsiBfrP, senseCode>>16, senseCode);
        else
           SendPosRsp( this, clsiBfrP);
        break;
     
     case a9:   /* request to host and pu is not active */
        break;
     
     case a10:   /* request to host and pu is active */
        SendPu(this,  clsiBfrP);
        break;
     
     case a12:   /* response from host */
        if (this->puFlag & NmvtPsidSent){   /* Is this a response of nmvt psid */
           this->puFlag &= ~NmvtPsidSent;
           ReceiveNmvtPsidRsp(this, clsiBfrP);
        }
        else{
            Fp_ResponseReceived(clsiBfrP);
	}
        T2sscpPuFsm(this, SendRequestToHost, 0); 
               /* check if any more data to send */
        break;

     case a13:   /* Reset request from control PU */
        DoToAllChildren(&this->luChildren, UResetLu);
        ResetLuMap(this->luMap);
        DspuActivationTrace(this->debugLevel, PuDeactivatedTrace, this,
                              GetPuName(this), 0);
        break;
        
     case a14:   /* Reset Request and PU had been reset. */
        break;

     case a15:   /* pu is active and not busy, check any data to send */
        if (this->puFlag & RqdSendNmvtPsid){
            if (!QueryAllChildren(&this->luChildren,QueryLuSendNmvt))
               this->puFlag &= ~RqdSendNmvtPsid;
	}
        if ((this->puFlag & RqdSendNmvtPsid) == 0){
           SscpClearToSend(this); 
	}
        break;

   }
}
/*=====================================================================*
* PuTimer
*======================================================================*
*
* Description:
*  
**/
static void PuTimer(Pu* this)
{
   T5sscpPuFsm(this, T5Timeout, 0);
}

/*=====================================================================*
* StartTimer
*======================================================================*
*
* Description:
*  
**/
static void StartTimer(Pu* this)
{
   if (this->timer)
      ReStartItimer(this->timer, 20*SECONDS, PuTimer, this);
   else
      this->timer = GetAndStartItimer(20*SECONDS, PuTimer, this);
}

/*=====================================================================*
* StopTimer
*======================================================================*
*
* Description:
*  
**/
static void StopTimer(Pu* this)
{
  if (this->timer){
     StopItimer(this->timer);
     this->timer = 0;
  }
}



/*=====================================================================*
* CleanupPu
*======================================================================*
*
* Description:
*     Send PuStopped to Link station. Clear any request buffer if any.
*
**/
static void CleanupPu(Pu* this)
{
   StopTimer(this);
   if (this->lsP)
       LsFsm (this->lsP, LsI_PuStopped, 0);
}

/*=====================================================================*
* t5sscpPuFsm: FSM table
*======================================================================*
*
**/

#define b11 a17
#define b12 a18

const static FsmStruct t5sscpPuFsm[MaxT5Input][T5Max] = {

/*                   0         1         2           3           4           */
/*                 Reset    PendActive Active       Busy      PendInact      */
/*StartPU*/       {{1,a1},     {1,a0},    {2,a0},      {3,a0},      {4,a2}},
/*Actpu+ve*/      {{0,a0},     {2,a3},    {2,a0},      {3,a0},      {4,a0}},
/*Actpu-ve*/      {{0,a0},     {0,a4},    {2,a0},      {3,a0},      {4,a0}},
/*DactpuRsp*/     {{0,a0},     {1,a0},    {2,a0},      {3,a0},      {0,a15}},
/*SupRqstFromPu*/ {{0,a5},     {1,a5},    {2,a6},      {3,a6},      {4,a5}},
/*unSupRqstFromPu*/{{0,a5},    {1,a5},    {2,a6},      {3,a13},     {4,a5}},
/*RequestToPu*/   {{0,a7},     {1,a7},    {3,a8},      {3,a9},      {4,a7}},
/*RspFromPu*/     {{0,a0},     {1,a0},    {2,a10},     {2,a10},     {4,a0}},
/*StopPu*/        {{0,a0},     {4,b11},   {4,a11},     {4,a11},     {4,a0}},
/*ReqDiscon*/     {{0,a0},     {4,b11},   {4,a11},     {4,a11},     {4,a0}},
/*ResetPu*/       {{0,a0},     {0,b12},   {0,a12},     {0,a12},     {0,a16}},
/*T5Timeout*/     {{0,a0},     {0,a14},   {2,a0},      {3,a0},      {0,a15}}
};

             
/*=====================================================================*
* T5sscpPuFsm
*======================================================================*
*
* Description:
*   Downstream PU FSM
*
**/
void T5sscpPuFsm(
             Pu* this, 
             T5Input  fsmInput, 
             DataReq_t* clsiBfrP)
{
   const FsmStruct* t5Fsm = &t5sscpPuFsm[fsmInput][this->sscpPuFsmState];

   /* Check if dspuPuOperState has changed                               */
   /*   PuStateChangeTrap will be generated from within Check() routine  */
   if (this->sscpPuFsmState != t5Fsm->state) {
       CheckPuOperStateChanged(this, t5Fsm->state);
       DspuFsmChgTrace(this->debugLevel, PuFsmChgTrace, this,
                         t5Input[fsmInput],
                         downPuStatus[this->sscpPuFsmState],
                         downPuStatus[t5Fsm->state]);
   }
   this->sscpPuFsmState = t5Fsm->state;
   DspuFsmTrace(this->debugLevel, PuFsmTrace, this,
                  fsmInput, t5Fsm->state, t5Fsm->action);
   
   switch(t5Fsm->action){
     case a0:
        if (clsiBfrP)
          DspuCLSIMsgDestroy(clsiBfrP);
        break;
    
     case a1:   /* Start PU signal from Link station, state = Reset */
        LogForPuWork(this, WORK_ACTPU);
        this->puFlag |= RqdActive;        /* change to active after Actpu Rsp */
        break;
     
     case a2:  /* Start Pu, state = PendingInactive */
        this->puFlag |= RqdActive;        /* change to active after Actpu Rsp */
        break;
     
     case a3: /* Actpu +ve Response */
        PuFreeBuffer(this);
        StopTimer(this);
        SetPuStartTime(this);
        ConnectAllPoolLusToPu(this);    /* connect all lu to upstream */
        DoToAllChildren(&this->luChildren, StartLu);
        if (this->isDefault)
           SetPuName(clsiBfrP, this->name.name);
        DspuCLSIMsgDestroy(clsiBfrP);
        DspuActivationTrace(this->debugLevel, PuActivatedTrace, this,
                              GetPuName(this), 0);
        break;
     
     case a4:   /* Actpu -ve Resp   */
        errmsg(&msgsym(ActpuNegResponse, DSPU), this->name.name);
        PuFreeBuffer(this);
        StopTimer(this);
        LsFsm (this->lsP, LsI_PuStopped, 0);
        DspuCLSIMsgDestroy(clsiBfrP);
        break;
     
     case a5:   /* Request from PU while PU is not active */
        SendNegRsp( this, clsiBfrP, NoSession, 0);
        break;
     
     case a6:   /* Request from PU and PU is active  */
        /* Pass data to focal point */
        clsiBfrP = AlertPassThru(this, clsiBfrP);
        if (!Fp_Send(clsiBfrP, this)) {
            SendNegRsp(this, clsiBfrP, ResourceNotAvailable, 0);
        }
        break;
     
     case a7:  /* RequestToPU and PU is inactive  */
        /* Send -ve response to the host. This function is not implemeted
         * yet and we dont know where is the host
         */
        break;
     
     case a8:   /* RequestToPU and PU is active  */
        SendPu(this,  clsiBfrP);
        break;
     
     case a9:   /* RequestToPu and PU is busy */
        SendNegRsp( this, clsiBfrP, FunctionNotSupported, 0);
        break;
     
     case a10:  /* Response from PU */
        DspuCLSIMsgDestroy(clsiBfrP);
        break;
     
     case b11:
        PuFreeBuffer(this);
        /* fall down to a11 */
        
     case a11:  /* StopPU or ReqDisc signal from Link station */
        if (clsiBfrP)
          DspuCLSIMsgDestroy(clsiBfrP);
        LogForPuWork(this, WORK_DACTPU);
        DoToAllChildren(&this->luChildren, DResetLu);
        this->puFlag &= ~RqdActive;
        break;
     
     case b12:
        PuFreeBuffer(this);
        /* fall down to a12 */
        
     case a12:  /* Reset PU */
        StopTimer(this);
        RemoveAllLuFromWaitList(this);
        DoToAllChildren(&this->luChildren, DResetLu);
        DspuActivationTrace(this->debugLevel, PuDeactivatedTrace, this,
                              GetPuName(this), 0);
        if (clsiBfrP)
          DspuCLSIMsgDestroy(clsiBfrP);
        break;
     
     case a13:  /* Unsupported request */
        SendNegRsp( this, clsiBfrP, FunctionNotSupported, 0);
        break;
     
     case a14:  /* Actpu response timer expired */
        PuFreeBuffer(this);
        this->timer = 0;     /* timer expired */
        if (clsiBfrP)
          DspuCLSIMsgDestroy(clsiBfrP);
        CleanupPu(this);
        break;
     
     case a15:  /* We had a DactPu response or timeout for DactPU */
        PuFreeBuffer(this);
        StopTimer(this);     /* stop timer if not expired */
        if (clsiBfrP)
          DspuCLSIMsgDestroy(clsiBfrP);
        RemoveAllLuFromWaitList(this);
        if ((this->puFlag & RqdActive) != 0)
           T5sscpPuFsm(this, StartPu, 0);
        else if (this->lsP)
           LsFsm (this->lsP, LsI_PuStopped, 0);
        StopTimer(this);
        break;
     case a16:
        PuFreeBuffer(this);
        StopTimer(this);    /* destruct pu if default */
        break;
        
   }
}


/*=====================================================================*
* PuSendNmvt
*======================================================================*
*
* Description:
*   This routine is called by lu to send nmvt. This is in response of
* PU being freed to send data.
*
**/
void PuSendNmvt(Pu* this, DataReq_t *clsiBfrP, byte luNumber)
{
   this->puFlag |= NmvtPsidSent;             /* this is nmvt psid sent */
   this->luPendingNmvtPsidRsp = luNumber;
   T2sscpPuFsm(this, RequestToHost,clsiBfrP); 
}

/*=====================================================================*
* PuRequestSentNmvt
*======================================================================*
*
* Description:
*   This is called by lu to log to send an nmvt
*
**/
void PuRequestSentNmvt(Pu* this)
{
   this->puFlag |= RqdSendNmvtPsid;      /* log to the pu that we can send nmvt */
   T2sscpPuFsm(this, SendRequestToHost, 0);
}


/*=====================================================================*
* ProcessPuCommand
*======================================================================*
*
* Description:
*    Process PU command. 
*
**/
void ProcessPuCommand( 
       Pu*         this,
       PuCommand   command
       )
{
   if (this->puType == Upstream){
       if (command == PuResetCmd)
          command = T2ResetPu;
       else
          command = T2ResetPu;
       T2sscpPuFsm(this, command, 0);
   }
   else{
      if (command == PuResetCmd)
         command = ResetPu;
      else if (command == PuStopCmd)
         command = StopPu;
      else if (command == PuStartCmd)
         command = StartPu;
      else
         command = ResetPu;
      T5sscpPuFsm(this, command, 0);
   }
}


/*=====================================================================*
* ConnectDefaultPu
*======================================================================*
*
* Description:
*   Creates the default PU if default PU is defined and pool range is
* defined
*
**/

Pu *
ConnectDefaultPu (LinkStn * lsP,
                  XidAddrSapT *xidAddrSapP)

{

  Pu * puP;
  
  if ((puP = GetDefaultPu()) != 0 && !IsEmpty(puP->poolRangeHeader)){

     puP = DefaultPu_nw(lsP, &xidAddrSapP->addr, 
                          SapGetSapNum(xidAddrSapP->lsapP),
                          xidAddrSapP->rsap, 
                          xidAddrSapP->u_xid.xid);
  }
  else
     return 0;
  return puP;

}  /* ConnectDefaultPu. */


/*=====================================================================*
* LogForPuWork
*======================================================================*
*
* Description:
*   This procedure is called whenever the PU wants to send DACTPU, ACTPU,  
* or when the LU wants to send something; because it must ask permission
* of the global buffer management to get a buffer.
*   It turns on the appropriate bit in the PU work bits, and calls 
* LsLogForWork(PUWORK).
*
**/
void LogForPuWork
       (Pu  *puP
       ,PuWorkType puWork
       )
{
  if (puP && puP->lsP){
     puP->puWorkBits |= puWork;
     LsLogForWork(puP->lsP, WORK_PU);
  }             
}       

/*=====================================================================*
* DoPuWork
*======================================================================*
*
* Description:
*   This checks the PU work bits in the following order: LU Work, 
* DACTPU, ACTPU. 
*   If the LU work bit is on it calls DoLuWork in all children LUs;
* if WORK_NOT_DONE is returned by all children, then it turns off the
* LU work bit; if WORK_DONE is returned it starts a timer and returns 
* WORK_DONE.
*   If no LU work is done, it then tries to do PU work; if the work 
* bit is on it sends the appropriate command, turns the bit off,
* starts a timer, and returns the enumeration WORK_DONE.
*   If there is no PU work to do this returns the enumeration
* WORK_NOT_DONE. 
*
**/

WorkResult
DoPuWork (Pu *this)

{
  DataReq_t *clsiBfrP;
  WorkResult workResult = WORK_DONE;


  /* Check Rqd state of PU                    */
  /*   If PU is inactive, pending deletion    */
  /*     don't bother with completing PU work */
  /* ---------------------------------------- */
  if ((!this) || (this->puFlag & RqdPendDelete))
     return(workResult);

  if (this->puWorkBits & WORK_LU_UP)
  {
    /* DoLuWorkUp returns TRUE to terminate QueryAllChildrenParm which 
     * returns the lu object. The workResult was set either WORK_DONE
     * or NO_BUFFER_AVAILABLE.
     */
    if (QueryAllChildrenParm(&this->luChildren, (ProcQChild1)DoLuWorkUp, 
                            &workResult) == 0)
    {
      /* Up LU work bit was not set so turn the bit off in the PU
       * and attempt to do PU work. 
       */
      this->puWorkBits &= (~WORK_LU_UP); 
    }  
    else
    {
      /* Up LU work bit was set and either work was done
       * or no buffers were available. In either case
       * we return to the link station with the work result.
       */
       return workResult;
    }   
  }
  
  if (this->puWorkBits & WORK_LU_DOWN)
  {
    /* DoLuWorkDown returns TRUE to terminate QueryAllChildrenParm which 
     * returns the lu object. The workResult was set either WORK_DONE
     * or NO_BUFFER_AVAILABLE.
     */
    if (QueryAllChildrenParm(&this->luChildren, (ProcQChild1)DoLuWorkDown, 
                             &workResult) == 0)
    {
      /* Down LU work bit was not set so turn the bit off in the PU
       * and attempt to do PU work. 
       */
      this->puWorkBits &= (~WORK_LU_DOWN); 
    }  
    else
    {
      /* Down LU work bit was set and either work was done
       * or no buffers were available. In either case
       * we return to the link station with the work result.
       */
       return workResult;
    }   
  }

  if (this->puWorkBits & WORK_DACTPU)
  {
    if ((clsiBfrP=BuildDactpu()) == 0)
      workResult = NO_WORK_BUFFERS_AVAILABLE;
    else
    {   
      SendPu(this, clsiBfrP);
      StartTimer(this);
      this->puWorkBits &= (~WORK_DACTPU);
      dspuActTraceType = TraceTypeDactpu;
    }
    /* close a window - where we MAY have thought we needed to send
     * an NMVT RSP but it is now overidden by sending DACTPU
     */
    this->puWorkBits &= (~WORK_NMVTRSP);
  }
  else if (this->puWorkBits & WORK_ACTPU)
  {
    if ((clsiBfrP=BuildActpu()) == 0)
      workResult = NO_WORK_BUFFERS_AVAILABLE;
    else
    {   
      SendPu(this, clsiBfrP);
      StartTimer(this);
      this->puWorkBits &= (~WORK_ACTPU);
      dspuActTraceType = TraceTypeActpu;
    }
    /* close a window - where we MAY have thought we needed to send
     * an NMVT RSP but it is now overidden by sending ACTPU
     */
    this->puWorkBits &= (~WORK_NMVTRSP);
  }
  else if (this->puWorkBits & WORK_NMVTRSP)
  {
    if ((clsiBfrP=BuildNmvtRsp(this->lastSscpPuSnf)) == 0)
      workResult = NO_WORK_BUFFERS_AVAILABLE;
    else
    {   
      SendPu(this, clsiBfrP);
      this->puWorkBits &= (~WORK_NMVTRSP);
    }
  }
  else
     workResult = WORK_NOT_DONE;
  
  return workResult;
}

/*=====================================================================*
* PuFreeBuffer
*======================================================================*
*
* Description:
*   This calls FreeLsWorkBuffer in the Link Station parent.
*
**/
void PuFreeBuffer
       (Pu *this
       )
{
   if (this && this->lsP)     /* Check if link station has closed */
       LsFreeBuffer(this->lsP);

}

void PuActivateDLC(Pu* this)
{
   Adaptor* adaptorP;
   Sap* sapP;
   if (this->lsP) {
      DspuError0(ActivateAlreadyActivated);
      DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                               PuActFailRsn_AlreadyActive, GetPuType(this));

   }
   else if (this->adaptorId.dlcType == DSPU_DLC_UNDEFINED) {
       DspuError0(ActNotStarted);
       DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                                PuActFailRsn_ConfigError, GetPuType(this));

   }
   else if ((adaptorP = AdaptorGetOrCreate(&this->adaptorId)) != 0){
       if ((sapP = QuerySapExisted(adaptorP, this->lSap)) != 0){
          if (this->puType  != sapP->sapDirection) {
               DspuError1(SapDefinedTwice, this->lSap);
               DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                                PuActFailRsn_ConfigError, GetPuType(this));
          }
          else{
              LinkStn* lsP  = LsCreate (this->puType == Upstream? GetUplsRoot(): GetDnlsRoot(), 
                         GetPuAddrP(this), this->remoteSap, sapP);
              if (lsP){
                this->negConnParms.windowSize = this->cfgConnParms.windowSize;
                this->negConnParms.maxIFrame  = this->cfgConnParms.maxIFrame; 
                PuChangeToActive(this, lsP);
                LsProcessCmd (lsP, LsI_StartLs, 0);
              }
              else {
                DspuError0(ConfigNoMemory);
                DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                               PuActFailRsn_InternalError, GetPuType(this));

              }
          }
       }  
       else
          DspuError1(SapNotDefined,this->lSap );
   }
}

void PuActivateSDLC(Pu* this)
{
   Adaptor* adaptorP;
   Sap* sapP;
   byte sdlcAddr;

   snpa_copy_to_buffer(GetPuAddrP(this), &sdlcAddr);
   if (this->lsP) {
      DspuError0(ActivateAlreadyActivated);
      DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                               PuActFailRsn_AlreadyActive, GetPuType(this));
   }
   else if (this->adaptorId.dlcType == DSPU_DLC_UNDEFINED) {
       DspuError0(ActNotStarted);
       DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                                PuActFailRsn_ConfigError, GetPuType(this));

   }
   else if ((adaptorP = AdaptorGetOrCreate(&this->adaptorId)) != 0){
       if ((sapP = QuerySapExisted(adaptorP, sdlcAddr)) != 0) {
          LinkStn* lsP  = LsCreate(this->puType == Upstream? GetUplsRoot(): GetDnlsRoot(), NULL, sdlcAddr, sapP);
          if (lsP){
            this->negConnParms.windowSize = this->cfgConnParms.windowSize;
            this->negConnParms.maxIFrame  = this->cfgConnParms.maxIFrame; 
            PuChangeToActive(this, lsP);
            LsProcessCmd (lsP, LsI_StartLs, 0);
          }
          else {
            DspuError0(ConfigNoMemory); 
            DspuNetMgmtNotification((void *) this, PU_ACTFAIL, 
                               PuActFailRsn_InternalError, GetPuType(this));
          }
       }  
       else
          DspuError1(SapNotDefined,this->lSap );
   }

} /* End PuActivateSDLC() */

void PuDeactivate(Pu* this)
{
    if (this->lsP){
       LsProcessCmd (this->lsP, LsI_PuStopped, 0);
    }
    else
       DspuError0(NoActivateFailed);
}

void PuNotConnectOut(Pu* this)
{
    this->adaptorId.dlcType = DSPU_DLC_UNDEFINED;
    this->puFlag &= ~PuStartRqsted;   /* dont restart pu */
    if (this->lsP){
       LsProcessCmd (this->lsP, LsI_PuStopped, 0);
    }
}

/*=====================================================================*
* PuConnectOut
*======================================================================*
*
* Description:
*   This is called at configuration time at DSPU START to save AdaptorId.
*
**/
void PuConnectOut(Pu* this, AdaptorId *idP)
{


   if (this->lsP != 0) 
       DspuErrors(StartAlreadyStarted, GetPuName(this));
   else {

      boolean errorDetected = FALSE;

      this->adaptorId.dlcType = idP->dlcType;
      this->adaptorId.u.adaptornumber = idP->u.adaptornumber;
      this->adaptorId.adaptorSubIfNumber = idP->adaptorSubIfNumber;
      this->adaptorId.dlcIdb = idP->dlcIdb;

      switch (idP->dlcType) {

          case DSPU_DLC_SDLC:
            if (snpa_get_type(GetPuAddrP(this)) == STATION_SDLC)
              PuActivateSDLC(this);
            else 
              errorDetected = TRUE;
            break;

          case DSPU_DLC_QLLC:
            if (snpa_get_type(GetPuAddrP(this)) == STATION_X121)
              PuActivateQLLC(this);
            else 
              errorDetected = TRUE;
            break;

          case DSPU_DLC_ETHERNET:
          case DSPU_DLC_TOKEN_RING:
          case DSPU_DLC_RSRB:
          case DSPU_DLC_VDLC:
          case DSPU_DLC_NCIA:
          case DSPU_DLC_FDDI:
          case DSPU_DLC_CHANNEL:
            if (snpa_get_type(GetPuAddrP(this)) == STATION_IEEE48)
              PuActivateLLC(this);
            else 
              errorDetected = TRUE;
            break;

          case DSPU_DLC_FRAME_RELAY:
            if (snpa_get_type(GetPuAddrP(this)) == STATION_FR10)
              PuActivateLLC(this);
            else
              errorDetected = TRUE;
            break;

          default:
              errorDetected = TRUE;
            break;
     
      } /* End switch(idp->dlcType) */

      if (errorDetected) {
          this->adaptorId.dlcType = DSPU_DLC_UNDEFINED;
          this->adaptorId.u.adaptornumber = 0;
          this->adaptorId.adaptorSubIfNumber = 0;
          this->adaptorId.dlcIdb = 0;
          DspuErrors(StartBadInterface, GetPuName(this));
      }

   }
}


static char* const t5Input[] = {
   "StartPu",      /* Singal from Link station", PuStartCmd */
   "ActpuPosRsp",  /* ACTPU positive response from downstream PU */
   "ActpuNegRsp",  /* ACTPU negative response from downstream PU */
   "DactpuRsp",    /* DACTPU positive or negative response from downstr PU */
   "SupRequestFromPu",  /* supported SNA request from downstream PU */
   "UnsupRequestFromPu",/* unsupported SNA request from downstream PU */
   "RequestToPu",  /* SNA request to downstream PU  */
   "ResponseFromPu", /* SNA response from downstrea PU */
   "StopPu",      /* Singal from Link station", PuStopCmd */
   "ReqDiscon",
   "ResetPu",      /* Singal from Link station", PuResetCmd */
   "T5Timeout"     /* SSCP_PU timer has timed out */
};

static char* const t2Input[] = {

    "T2StartPu",        /* Signal from link station */
    "Actpu",            /* ACTPU received from host */
    "ActpuDddlu",
    "Dactpu",           /* DACTPU received from host */ 
    "RequestFromHost",  /* SNA reqeust received from host on the SSCP_PU sess*/
    "RequestToHost",    /* An SNA request(probably NMVT) to host */
    "ResponseFromHost", /* An SNA response from host */
    "T2ResetPu",        /* Hierachical reset from DSPU Control Point */
    "ChildrenResetSignal",
    "SendRequestToHost" /* Signal internal driven when request is sent */
 };


/*=====================================================================*
* FindActivePu
*======================================================================*
*
* Description:
*    Scan through all ls to find a pu with a pu name.
**/
Pu* FindActivePu(const char* name, Sibling* root)
{
   LinkStn* lsP = 
   QueryAllChildrenParm(root, (ProcQChild1)QueryFindChild, (void*)name);
   if (lsP)
      return LsChild(lsP);   /* find Link station child */
   return 0;
}


