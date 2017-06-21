/* $Id: dspu_fp.c,v 3.1.62.2 1996/07/29 07:04:08 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_fp.c,v $
 *------------------------------------------------------------------
 * Focal point operation.
 * This is part of dspu sub-system and it is the interface to the SNA Network
 * management. The dspu-subsystem can compile and link without sna network
 * management syb-system.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_fp.c,v $
 * Revision 3.1.62.2  1996/07/29  07:04:08  lcox
 * CSCdi57699:  watchdog crash on invalid NMVT handling
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/05/17  10:47:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.3  1996/04/22  03:48:25  lcox
 * CSCdi55251:  change DspuMalloc(of Pu)/malloc to malloc_named call
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.2  1996/04/22  00:06:39  lcox
 * CSCdi55112:  mem corruption on ACTPU when NMVT rsp owed downstream
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.1  1996/03/30  08:10:32  doner
 * CSCdi52657:  IBU modularity - DSPU conversion to use new IOS scheduler
 * Branch: IbuMod_Calif_branch
 * Update to new scheduler interface
 *
 * Revision 3.1  1995/11/09  11:28:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:50  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "dspu_fp.h"
#include "mv_sr.h"
#include "mv_datah.h"
#include "dspu_pu.h"
#include "snaprim.h"
#include "snapiu.h"
#include "dspu.h"


/*=====================================================================*
* FP vectors initialisation
*======================================================================*
*
* Description:
* The focal point will talk to the SNA network management through these vectors.
* They are initialised to do nothing if there is no API
*/

static word
FpReceive(MV*     majorVector,
             hword   vecLen,
             char*   correlator,
             hword   correlatorLength)
{
   return NoClients;
}


static void FpDummy (void){}

MVSRreceive*  fpVectorReceived = FpReceive;
FpStatusChanged* fpUp = FpDummy;
FpStatusChanged* fpDown = FpDummy;
FpStatusChanged* fpConfigured = FpDummy;

/*=====================================================================*
* FP structure
*======================================================================*
*
* Description:
*   The focal point data structure contains a fifo queue FpQ. This fifo has
* 64 elements and 500 is assigned for NMVT sent by Network Management and 10
* is assigned for ALERTS sent by DSPU. Data is sent to the host as follows:
* 1. Data is stored in the fifo q and FpHasDataToSend is called. This tells 
*    dspu main process to call SscpPuRequestToSend. 
* 2. SscpPuRequestToSend is DSPU PU FSM, which checks if pu is not busy. It 
*    will
*    call Fp_ClearToSend immediately if pu is not busy or it will wait until
*    pu is not busy.
* 3. Fp_ClearToSend reads data from the fifo and calls SscpPuSend to send
*    the data.
* 4. When a pu response is received, Fp_ResponseReceived will be called by 
*    the pu.
*    Fp_ClearToSend will be called and a response routine will be called. 
*    In the case of DSPU alert, the response will pass down to the DSPU. 
*    In the case of NMVT, the response buffer is freed.
*/
/* maximum network management nmvt can pass to focal point */
#define MaxNmvt         50

/* maximum dspu alert can pass to the focal point */
#define MaxAlert        10


/* Maximum fifo size for free queue  */
#define MaxFifoSize     64
#define FifoMask        (MaxFifoSize - 1)


struct FpQ {
    RspRoutine*     rspRoutine;
    Pu*             rspObject; 
    DataReq_t*      clsiBfrP;
};

typedef struct FpQ FpQ;

struct Fp_ {
    Pu*            focalPu;
    /* fifo implementation.
     * Because the fifo had to protect against reentrancy, and also we may
     * need more than one buffer which had to be contiguous, we need to
     * implement our own fifo rather than using system fifo queue.
     * Both readIndex and writeIndex are incremented until 0xff and wrapped
     * back to zero. The number of elements in the fifo is writeIndex -
     * readIndex. Access to the fifo element always using the mask to get
     * to one of the 64 elements.
     */
    
    byte     readIndex;   /* current fifo read index */
    byte     writeIndex;
    byte     freeAlertQ;      /* number of dspu alertQ left */
    byte     freeNmvtQ;       /* number of nmvtQ left       */
    
    FpQ      fpQ[MaxFifoSize];
    FpQ*     waitForResponseP;
 };

static Fp* focalPoint;

boolean IsFocalPointExisted (void)
{
   return focalPoint != 0;
}

Pu*  GetFocalPu(void)
{
   return focalPoint == 0? 0: focalPoint->focalPu;
}

const char* GetFocalPuName (void)
{
   return focalPoint? GetPuName(focalPoint->focalPu): 0;
}


/*=====================================================================*
* Fp_new, Fp_delete
*======================================================================*
*
* Description:
*   A host pu with focalPoint is configured. Create the focal point 
* object
*/

Fp* Fp_new (Pu* focalPu)
{
   focalPoint = malloc_named(sizeof(Fp), "DSPU FP");
   if (focalPoint != NULL){
      focalPoint->focalPu = focalPu;
      focalPoint->readIndex = 0;
      focalPoint->writeIndex = 0;
      focalPoint->freeAlertQ = MaxAlert;
      focalPoint->freeNmvtQ = MaxNmvt;
  }
   return focalPoint;
}


static void Fp_delete (void)
{
     byte index;
     Fp* fp = focalPoint;
     focalPoint = 0;        /* stop all focal point operation */

     /* tell api that the focal point is deleted */
     fpDown();                           /* tell Client fp is not active */
     index = fp->readIndex;
     /* delete all clsi buffer */
     while (fp->writeIndex != index){
          FpQ* readQ = &fp->fpQ[index++ & FifoMask];
          if (readQ->clsiBfrP)
             DspuCLSIMsgDestroy(readQ->clsiBfrP);
     }
     free(fp);
}

/*=====================================================================*
* Fp_PuDeleted 
*======================================================================*
*
* Description:
*   A pu had been destructed. Check if this pu is the focal point pu. If
* so, destruct the focal point object
*/

void Fp_PuDeleted (Pu* puP)
{
   if (focalPoint) {
       if (focalPoint->focalPu == puP)
          Fp_delete();
       else if (focalPoint->waitForResponseP  &&
              focalPoint->waitForResponseP->rspObject == puP)
          /* Pu had been deleted before a alert been responded */
          focalPoint->waitForResponseP->rspObject = 0;  /* remove the pu */

       else{
          /* a pu had been deleted, check if this pu had any alert sending
           * to the focal point. If so, we need to delete this object. Since
           * the fifo is strictly FIFO, we cannot remove the object, so we
           * set rspObject to zero to indicate this fifo element had been deleted.
           */

          byte index = focalPoint->readIndex;
          
          while (focalPoint->writeIndex != index){
             FpQ* readQ = &focalPoint->fpQ[index++ & FifoMask];
             if (readQ->rspObject == puP){
                 readQ->rspObject = 0;
                 DspuCLSIMsgDestroy(readQ->clsiBfrP);
                 readQ->clsiBfrP = 0;       /* remove from the queue */
	     }
          }

       }
   }
}

/*=====================================================================*
*  Fp_ReqFromHost
*======================================================================*
*
* Description:
*   A sscp-pu data request from the host. If this is the NMVT data, then
* passed it down to MVSR.
*/
word Fp_ReqFromHost(Pu* hostPu, DataReq_t* clsiBfrP)
{
   /* we can only accept sscp pu data from focal point */

   if (focalPoint && focalPoint->focalPu == hostPu){
       /* pass to Major Vector received */
       SNA* snaP = (SNA *)ReturnDataHandle(clsiBfrP);
       MV* mv = (MV*) GetNmvtMV(clsiBfrP);
       if (mv){
         return fpVectorReceived(mv, MV_Len(mv),&snaP->ruData[5], 2);
       }
    }
   return FunctionNotSupported << 16;
}


/*=====================================================================*
*  Fp_ClearToSend
*======================================================================*
*
* Description:
*   The focal point is ready to pass data, send the data up
*/

static void Fp_ClearToSend (void)
{
   while (focalPoint->writeIndex != focalPoint->readIndex){
       FpQ* readQ = &focalPoint->fpQ[focalPoint->readIndex++ & FifoMask];
       if (readQ->clsiBfrP == 0) {  /* someone cancel the message */
           readQ->rspRoutine(readQ->rspObject, 0);  /* pass response down */
           focalPoint->waitForResponseP = 0;
       }
       else{
          focalPoint->waitForResponseP = readQ;
          SscpPuSend(focalPoint->focalPu, readQ->clsiBfrP);
          readQ->clsiBfrP = 0;
          break;
       }
   }

}

/*=====================================================================*
*  ProcessFpData
*======================================================================*
*
* Description:
*   This subroutine is called by the dspu main to check if any data is to be
*   sent to the host.  A client uses the focal point to pass NMVT data
*   and will terminate at focal point. It then informs dspu there are data. 
*   This is necessary to reduce the stack usage and to ensure all dspu
*   code is always run under the process.
*/

void ProcessFpData (void)
{
   if (focalPoint) {

       /*
        * Check that SSCP-PU session is still active
        *
        */
       if (PuQueryUpPuActive(focalPoint->focalPu)) {

           if (focalPoint->writeIndex != focalPoint->readIndex) 
               SscpPuRequestToSend(focalPoint->focalPu);

           /* Retest since readIndex may have changed */
           if (focalPoint->writeIndex != focalPoint->readIndex)
               FpHasDataToSend();

       } else {
           /*
            * Notify clients that FP is no longer active
            *
            */
           if ( PuQueryUpPuInActive(focalPoint->focalPu) ) {
	     fpDown();
	   }
       }

   }
}

/* This routine is called by DSPU PU */
void SscpClearToSend(Pu* puP)
{
   if (focalPoint && focalPoint->focalPu == puP)
      Fp_ClearToSend();
}
/*=====================================================================*
*  Fp_RspFromHost
*======================================================================*
*
* Description:
*   A sscp-pu data response coming from host. Check if a request had been
* sent. If so, then we pass the response to the requester. 
*/
void Fp_ResponseReceived (DataReq_t* buf)
{
   if (focalPoint && focalPoint->waitForResponseP){
      /* We have to call Fp_ClearToSend before rspRoutine because rspRoutine
       * could call Fp_Send to send another message to cause PU busy. Similarly 
       * Fp_ClearToSend can call Fp_Send, then rspRoutine's Fp_Send will fail.
       * Note that only 60 elements out of 64 elements are used, so that the
       * fifo will never get full, therefore tmpWriteQ will not be overwritten
       * during the time Fp_Send or Fp_VectorSent is called.
       */

      FpQ* tmpWriteQ = focalPoint->waitForResponseP;
      Fp_ClearToSend();
      /* send response down to the requester's response routine */
      tmpWriteQ->rspRoutine(tmpWriteQ->rspObject, buf);
   }
   else
      DspuCLSIMsgDestroy(buf);
}


/* A response is sent to downstream PU */

static void DspuAlertRsp (Pu* puP, DataReq_t* buf)
{
   focalPoint->freeAlertQ++;          /* one more alert can be sent */
   if (puP)              /* Pu can be deleted */
      if (buf)
         SendPu(puP, buf);      /* send to downstream */
      else {
         /* downstream is waiting on a response so we must build one */
         LogForPuWork( puP, WORK_NMVTRSP);
      }
}



#define DISABLE_INTERRUPT leveltype status = raise_interrupt_level(ALL_DISABLE);
#define ENABLE_INTERRUPT  reset_interrupt_level(status);

/*=====================================================================*
*  Fp_Send
*======================================================================*
*
* Description:
*   Send downstream PU alert data to focalPoint PU. This routine is called
* by downstream PU which is a process, therefore freeAlertQ is not 
* interrupt protected.
* Return TRUE is data can be sent.
*/
boolean Fp_Send (DataReq_t* buf, Pu* objP)
{
   if (focalPoint && focalPoint->freeAlertQ > 0){
       FpQ* writeQ;
       DISABLE_INTERRUPT
       focalPoint->freeAlertQ--;
       writeQ = &focalPoint->fpQ[focalPoint->writeIndex++ & FifoMask];
       writeQ->rspRoutine = DspuAlertRsp;
       writeQ->rspObject = objP;
       writeQ->clsiBfrP = buf;
       ENABLE_INTERRUPT
       SscpPuRequestToSend(focalPoint->focalPu);
       return TRUE;
   }
   return FALSE;
}


/*=====================================================================*
*  Fp_HostPuUp
*======================================================================*
*
* Description:
*   A host pu is up, check if this is the focal point pu and if there is
* any sscp data waiting to send. If so, scan through all the vector and
* set the held bit
*/

void Fp_HostPuUp(Pu* puP)
{
#define GenericAlertSubVector 0x92
  if (focalPoint && focalPoint->focalPu == puP){
      byte index;
      fpConfigured();
      fpUp();

      /* update the "delayed" flag in any alerts we are holding */
      index = focalPoint->readIndex;
      while ((focalPoint->writeIndex - index) & FifoMask){
          FpQ* readQ = &focalPoint->fpQ[index++ & FifoMask];
          MV* mv;
          byte* sv;
          if (readQ->clsiBfrP) {
              mv = (MV*) GetNmvtMV(readQ->clsiBfrP);
              if (mv) {
                  sv = (byte*) MV_FindSV(mv, GenericAlertSubVector);
                  if (sv)
                      sv[2] |= 0x40;     /* alert was delayed */
	      }
          }
      }

      if (focalPoint->waitForResponseP){
          /* we may have waited for response and host is down */
          focalPoint->waitForResponseP->rspRoutine(
                    focalPoint->waitForResponseP->rspObject, 0);
          focalPoint->waitForResponseP = 0;
      }
      if ((focalPoint->writeIndex - focalPoint->readIndex) & FifoMask)
          Fp_ClearToSend();
   }
}

/*=====================================================================*
*  Fp_GoingDown
*======================================================================*
*
* Description:
*   A host pu is deactivated, check if this is the focal point pu, if so
* display a message
*
*/

void Fp_GoingDown(Pu* puP)
{
   if (focalPoint && focalPoint->focalPu == puP){
      fpDown();
   }
}


/* acknowledged from the host, we simply destroy the buffer */

static void DestroyRspBuffer(Pu* puP, DataReq_t* clsiBfrP)
{
   focalPoint->freeNmvtQ++;
   if (clsiBfrP)
       DspuCLSIMsgDestroy(clsiBfrP);
}



/*=====================================================================*
*  Fp_VectorSent
*======================================================================*
*
* Description:
*   This is called by MVSR to send multiple clsi buffers
*/

/* WARNING:  this routine can be reentrant or called by interrupt */
   
MV_SV_RC
Fp_VectorSent (hword totalBuf, DataReq_t** buffer)
{
   hword i;
   if (focalPoint == 0)
       return MV_SV_NoFP;


   {DISABLE_INTERRUPT
   if (totalBuf <= focalPoint->freeNmvtQ){   /* if fifo had room to send */
       focalPoint->freeNmvtQ -= totalBuf;
       for (i = 0; i < totalBuf; i++){
           FpQ* writeQ = &focalPoint->fpQ[focalPoint->writeIndex++ & FifoMask];
           writeQ->rspRoutine = DestroyRspBuffer;
           writeQ->rspObject = 0;
           writeQ->clsiBfrP = buffer[i];
        }
        ENABLE_INTERRUPT
        FpHasDataToSend();      /* tell focal point we had data */
        return MV_SV_OK;
   }
   ENABLE_INTERRUPT}
   return MV_SV_Overflow;

}
