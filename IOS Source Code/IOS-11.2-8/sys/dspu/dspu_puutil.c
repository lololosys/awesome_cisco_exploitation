/* $Id: dspu_puutil.c,v 3.6.12.11 1996/09/10 06:08:45 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_puutil.c,v $
 *------------------------------------------------------------------
 * DSPU PU Utility Programs.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Includes functions used by the PU, e.g. Segmentation support,
 * some Pool functions and "Show" support.
 *------------------------------------------------------------------
 * $Log: dspu_puutil.c,v $
 * Revision 3.6.12.11  1996/09/10  06:08:45  lcox
 * CSCdi68591:  dspu pu matching algorithm behaviour is not clearly defined
 * Branch: California_branch
 *
 * Revision 3.6.12.10  1996/08/28  12:42:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.9  1996/08/09  01:54:00  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.6.12.8  1996/07/29  21:47:00  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.6.12.7  1996/07/24  01:23:34  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.6.12.6  1996/06/26  19:22:22  ppearce
 * CSCdi61351:  DSPU commands are not accepted on FDDI interface
 * Branch: California_branch
 *   Enable DSPU/FDDI configuration
 *
 * Revision 3.6.12.5  1996/06/21  02:21:58  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.6.12.4  1996/05/27  06:01:38  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/21  09:46:07  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.12.2  1996/05/17  10:48:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.5  1996/05/03  17:18:22  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.4  1996/04/26  07:35:10  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.2.3  1996/04/03  14:02:56  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.2.2  1996/03/28  08:20:31  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/03/22  18:50:29  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.6.12.1  1996/03/18  19:33:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.18.3  1996/03/16  06:40:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.18.2  1996/03/07  08:45:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.18.1  1996/02/20  13:47:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/17  09:17:03  ppearce
 * CSCdi46728:  DSPU HOST can not be configured without XID parameter
 *
 * Revision 3.5  1995/11/30  19:08:58  ppearce
 * CSCdi44719:  sna rsrb start command does not appear in config
 *   Separate nv_write of "sna rsrb start" and "dspu rsrb start"
 *
 * Revision 3.4  1995/11/21  00:25:16  rbatz
 * CSCdi40809:  Clean up DLC-level activation sequence for connect-ins.
 *
 * Revision 3.3  1995/11/20  16:37:33  ppearce
 * CSCdi43278:  dspu start and sna start commands disappear from config
 *
 * Revision 3.2  1995/11/17  09:04:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:56:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/19  21:12:56  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/09/20  17:03:09  hampton
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
#include "../ui/common_strings.h"
#include <string.h>

#include "dspu.h"     /* GetDefaultPu */
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_lu.h"
#include "dspu_pool.h"
#include "dspu_ls.h"
#include "dspu_sap.h"

#include "dspu_adaptor.h"
#include "dspu_alert.h"
#include "snaprim.h"
#include "snapiu.h"
#include "snautil.h"
#include "dspu_error.h"
#include "dspu_util.h"
#include "dspu_adaptor.h"
#include "dspu_fp.h"
#include "dspu_notification.h"
#include "parser_defs_dspu.h"

#include "interface_generic.h"

#define OneSecond 1000


/*=====================================================================*
* Segmentation 
*======================================================================*
*
* Description:
*   Segment a given frame into maxIframe size.  Each time it is called
* it will return one buffer within maxIframe size which can be sent.
* The parameter "offset" is an index starting from the start of data
* in the original buffer, indicating where the segmentation process
* is up to.  On the first call offset is zero, and it is set back
* to zero on the last return (when the original buffer is returned
* with the last segment).
*
*   If no segmentation is required the original buffer is returned 
* unchanged immediately, with offset equal to zero.
* 
*
**/

DataReq_t*                            /* message segmented  */
Segmentation(DataReq_t* clsiBfrP,     /* input message      */
             hword maxIframe,         /* maximum size allowed */
             hword* offset)           /* offset to the data */

{

#define THSIZE 6

  SNA       * snaP;
  DataReq_t * bufferP;
  hword       dataLen;
  
  dataLen = GetDataLength(clsiBfrP);
  snaP = (SNA *)ReturnDataHandle(clsiBfrP);

  /* Test if segmentation is required.
  */
  if (dataLen <= maxIframe && *offset == 0)
      return clsiBfrP;                  /* no segmentation required */
      
  else if (dataLen - *offset + THSIZE <= maxIframe) /* last of the segment */
  {
     /* This is the end of the segmentation call. The original buffer is
      * used to pass the data. Move the data from *offset to the beginning
      * of RH (no RH). The TH remains unchanged except BBIU is forced off.
      */
     
     memcpy(&snaP->RH0, (byte*)snaP + *offset, dataLen - *offset);
     SetDataLength(clsiBfrP, dataLen - *offset + THSIZE);
     snaP->TH0 &= ~TH0_BBIU;         /* set to last or middle segment     */
     *offset = 0;                    /* tell user end of segmentation */
  }
  
  else {                              /* not last segment */     
     
     /* Start segmentation. First get a new buffer with maxiframe size. 
      * Then copy data up to maxiframe size to this new buffer 
      */
     
     SNA* nsnaP;
     
     bufferP = Cnstrct_DataReq (BLOCKING, maxIframe);
     nsnaP = (SNA *) ReturnDataHandle (bufferP);

     if (*offset == 0){
         
         /* This is the first segmentation call. Just copy all data up
          * to the maxIframe to the new buffer. Set the new buffer to
          * first segment if this is whole or first segment, else set it
          * to middle segment;
          */

         memcpy(nsnaP, snaP, maxIframe);
         nsnaP->TH0 &= ~TH0_EBIU;       /* set to first or middle seg */
         *offset = maxIframe;
     }
     
     else{
     
        /* This is in the middle of segmentation, copy TH and the data 
        * indexed by the offset. Pass frame as middle segment.
        */
        
        memcpy(nsnaP, snaP, THSIZE);           /* copy TH field */
        memcpy(&nsnaP->RH0, (byte*)snaP + *offset, maxIframe - THSIZE);
        nsnaP->TH0 &= ~TH0_MPF;           /* set to middle segment */
        *offset += maxIframe - THSIZE;
     }
     return bufferP;
  }
  return clsiBfrP;
}


/*=====================================================================*
* QueryLuConflict
*======================================================================*
*
* Description:
*   This function is called when an LU connection, both dedicated and
* pooled is configured, to ensure there are no conflicts.
*
*   There are two ways an LU conflict can occur. Firstly, there could
* already be an existing connection, in which case we return TRUE.
* Secondly, for the downstream PU, we examine each poolRange control
* block in the poolRangeHeader to determine if the given start and 
* range is already defined. If found we return TRUE. If no conflict
* is detected, then FALSE is returned.
*
*/
boolean QueryLuConflict
          (Pu* this
          ,byte start
          ,byte range
          )
{
  PoolRange * pRP;
  byte lu;
  
  /* First check if there is connection already 
   */
  for (lu = 0; lu < range; lu++){
      if (GetChild(&this->luChildren, lu + start))
        return TRUE;         /* there is a connection */
  }
  
  /* Second, is start and range already defined ? 
   * Note: Used for the downstream PU only. 
   *       For the upstream PU the poolRangeHeader is empty. 
   */
  pRP = GetFirstQelement(this->poolRangeHeader);
  while (pRP)
  {
    byte last = (start +  range - 1);
    if ((pRP->luStart <= start && start < (pRP->luStart + pRP->luRange)) 
      ||(pRP->luStart <= last  && last  < (pRP->luStart + pRP->luRange)))
       return TRUE;
       
    pRP = pRP->next;
  }
  return FALSE;    
}

/*=====================================================================*
* ConnectPoolRangeLusToPu
*======================================================================*
*
* Description:
*
*   If there is a free LU then attempt to connect it to the downstream
*  PU through LuConnect and return this result to the calling routine. 
*
*   If there is no free LU then set luWait to the downstream lu number 
*  which is waiting to get connected. Then, enqueue this pool range 
*  onto the pool control blocks waitQueue. This waitQueue is a link list  
*  of pool ranges that are contain LUs (i.e. luWait) which are waiting 
*  to get connected to the downstream PU. 
*
*  Note : When the downstream PU frees up its LUs then the waitQueue
*         is dequeued and another LuConnect attempt is made.
**/

ConnectStatus ConnectPoolRangeLusToPu(                 
                         Pu*        dnPuP,     /* downstream Pu */
                         PoolCB*    poolCBP,   /* pool control block */
                         PoolRange* poolRgeP)  /* pool range control */
{
  byte   i;
  Lu*    luP;
  ConnectStatus connectStatus = NoLusInPool;
  
  for (i=0; i<poolRgeP->luRange; i++)
  {
     luP = GetFreeLuFromHost(poolCBP);
     if (luP) {
        connectStatus = LuConnect(luP, dnPuP, i+poolRgeP->luStart);
        if (connectStatus == NoMemory) {  
          /* Try again later */
          poolRgeP->luWait = i+poolRgeP->luStart;
          DspuEnqueue(&poolCBP->waitQueueHeader, &poolRgeP->nextWait);
          break;
        }  
     }   
     else{
        /* Try again later */
        connectStatus = NoLusInPool;       
        poolRgeP->luWait = i+poolRgeP->luStart;
        DspuEnqueue(&poolCBP->waitQueueHeader, &poolRgeP->nextWait);
        break;
     }
  }
  return connectStatus;
}

void DeletePoolRangeIfMatch(Pu* this, const char* poolName)
{
   PoolRange* poolP;
   poolP = GetFirstQelement(this->poolRangeHeader);
   while (poolP){
      PoolRange* tmp =  poolP->next;
      if ( strcmp(poolP->poolName, poolName) == 0){
          DspuDeleteQueueElement(&this->poolRangeHeader, poolP);
          PoolRange_dl(poolP);
      }
      poolP = tmp;
   }
}

/*=====================================================================*
* ConnectAllPoolLusToPu
*======================================================================*
*
* Description:
*   This is called when a downstream pu is activated (Actpu +ve response
* received) to connect all LUs defined in the pool. 
*   
**/
void
ConnectAllPoolLusToPu
          (Pu * dnPuP 
          )
{
  PoolRange * poolRgeP;

  poolRgeP = GetFirstQelement(dnPuP->poolRangeHeader);
  while (poolRgeP)
  { 
    PoolCB* poolCBP = FindPool(poolRgeP->poolName);
    if (poolCBP) 
      ConnectPoolRangeLusToPu(dnPuP,poolCBP,poolRgeP);

    poolRgeP = poolRgeP->next;         
  }  
}

/* The following is used for SHOW command to display the 
 * status of the PU.
 */  
char* const linkStationStatus[] = {
  "Reset",
  "PendConOut",
  "PendIn",             /* PendIn - Not relevent since no PU attached */
  "PendConIn",
  "PendXid",             /* PendXid - Not relevent since no PU attached */
  "Xid",         
  "ConnOut",      /* ConnOut */
  "ConnIn",       /* ConnIn  */
  "Connected",            
  "PendDisc",              
  "PendClose"             
  };  
  
char* const downPuStatus[] = {
  "Reset",
  "PendActive",
  "Active",
  "Busy",
  "PendInact"
  };

char* const upPuStatus[] = {
  "Reset",
  "Active",
  "Busy"
  };


static void PrintDspuDlcTypeInfo(AdaptorId *idP) 
{

    if (!idP)
      return;

    switch (idP->dlcType) {

      case DSPU_DLC_SDLC:
        printf("%s (SDLC)", idP->dlcIdb->hwptr->hw_namestring);
        break;

      case DSPU_DLC_QLLC:
        printf("%s (QLLC)", idP->dlcIdb->hwptr->hw_namestring);
        break;

      case DSPU_DLC_ETHERNET:
      case DSPU_DLC_TOKEN_RING:
      case DSPU_DLC_FDDI:
      case DSPU_DLC_CHANNEL:
        printf("%s ", idP->dlcIdb->hwptr->hw_namestring);
        break;

      case DSPU_DLC_RSRB:
        printf("Rsrb ");
        break;

      case DSPU_DLC_VDLC:
        printf("Vdlc ");
        break;

      case DSPU_DLC_NCIA:
        printf("Ncia ");
        break;

      case DSPU_DLC_FRAME_RELAY:
        printf("%s (FRAME-RELAY)", idP->dlcIdb->hwptr->hw_namestring);
        break;

      case DSPU_DLC_UNDEFINED:
      default:
        break;

    } /* End switch(idP->dlcType) */


} /* End PrintDspuDlcTypeInfo() */


static void PrintDspuDlcLsAddrInfo(AdaptorId *idP, Pu* puP) 
{

  ushort addrLen;
  uchar  addr[STATIONLEN_MAX];

  Xid        *xid;
  ConnParmsT *connParms;
  byte       qllcAddr;
  

  if ( (!idP) || (!puP) || (!puP->lsP) )
    return;

  addrLen = snpa_get_length(LsGetAddrP(puP->lsP));        
  snpa_copy_to_buffer(LsGetAddrP(puP->lsP), (uchar *) &addr);

  switch(idP->dlcType) {

      case DSPU_DLC_SDLC:
      {
          printf(" SDLC %s\n", ByteToHexAscii((byte *) &addr, addrLen));
          break;
      }
      case DSPU_DLC_QLLC:
      {
	  char asciiQllcAddr[3];
          printf(" X25 %s", addr);
	  qllcAddr = LsGetLocalSapAddr(puP->lsP);
	  QllcSubAddrToAscii(qllcAddr, asciiQllcAddr);
          printf(" QLLC %s\n", asciiQllcAddr);
          break;
      }
      case DSPU_DLC_RSRB:
      case DSPU_DLC_VDLC:
      case DSPU_DLC_NCIA:
      case DSPU_DLC_FDDI:
      case DSPU_DLC_ETHERNET:
      case DSPU_DLC_TOKEN_RING:
      case DSPU_DLC_CHANNEL:
      {
          printf(" RMAC %s", MacToAscii((byte *) &addr));
          printf(" RSAP %d",   LsGetRemoteSapAddr(puP->lsP));
          printf(" LSAP %d\n", LsGetLocalSapAddr(puP->lsP));
          break;
      }
      case DSPU_DLC_FRAME_RELAY:
      {
          hword dlciNumber;
          memcpy(&dlciNumber, &addr, 2);
          printf(" DLCI %d", dlciNumber);
          printf(" RSAP %d",   LsGetRemoteSapAddr(puP->lsP));
          printf(" LSAP %d\n", LsGetLocalSapAddr(puP->lsP));
          break;
      }
      case DSPU_DLC_UNDEFINED:
      default:
      {
          printf("\n"); 
          break;
      }
  } /* End switch(idP->dlcType) */

  if (puP->puType == Upstream) {
      xid = &puP->xid;
      connParms = &puP->cfgConnParms;
  } else {
      xid = &puP->rcvdXid;
      connParms = &puP->negConnParms;
  }

  printf("     XID %s RETRIES %d RETRY_TIMEOUT %d\n",
      ByteToHexAscii(&xid->blk_num[0], 4),
      puP->retryInfo.maxCount, puP->retryInfo.maxTimeout);
      
  printf("     WINDOW %d MAXIFRAME %d\n",
     connParms->windowSize, connParms->maxIFrame);

  printf("     ACT BUFFERS %d SAP ACT BUFFERS %d\n",
         puP->lsP->workBufCnt, SapQueryBuffersUsed(puP->lsP->sapP));


} /* End PrintDspuDlcLsAddrInfo() */


static void PrintDspuDlcPuAddrInfo(AdaptorId *idP, Pu* puP) 
{

  ushort addrLen;
  uchar  addr[STATIONLEN_MAX];

  DspuDLCType_e dlcType = DSPU_DLC_UNDEFINED;


  /*
   * If PU is not connected, 
   *   idP will be NULL or dlcType will be UNDEFINED 
   *
   * However, we might know based on what address info is configured 
   *   assume Token-Ring if no address info is configured
   *
   */
  if ( (!idP) || (idP->dlcType == DSPU_DLC_UNDEFINED) ) {
    if (snpa_get_type(GetPuAddrP(puP)) == STATION_SDLC)
      dlcType = DSPU_DLC_SDLC;
    if (snpa_get_type(GetPuAddrP(puP)) == STATION_X121)
      dlcType = DSPU_DLC_QLLC;
    else if (snpa_get_type(GetPuAddrP(puP)) == STATION_FR10)
      dlcType = DSPU_DLC_FRAME_RELAY;
    else 
      dlcType = DSPU_DLC_TOKEN_RING;
  }

  addrLen = snpa_get_length(GetPuAddrP(puP));
  snpa_copy_to_buffer(GetPuAddrP(puP), (uchar *) &addr);

  switch(dlcType) {

      case DSPU_DLC_SDLC:
      {
          if (snpa_get_type(GetPuAddrP(puP)) == STATION_SDLC)
            printf(" SDLC %s\n", ByteToHexAscii((byte *) &addr, addrLen));
          else 
            printf(" SDLC --\n");
          break;
      }
      case DSPU_DLC_QLLC:
      {
	  char asciiQllcAddr[3];
          if (snpa_get_type(GetPuAddrP(puP)) == STATION_X121)
            printf(" X25 %s", addr);
          else 
            printf(" X25 -- ");

          if (puP->lSap != NoSap) {
	    QllcSubAddrToAscii(puP->lSap, asciiQllcAddr);
            printf(" QLLC %s\n", asciiQllcAddr);
          } else
            printf(" QLLC --\n");
         
          break;
      }
      case DSPU_DLC_RSRB:
      case DSPU_DLC_VDLC:
      case DSPU_DLC_NCIA:
      case DSPU_DLC_FDDI:
      case DSPU_DLC_ETHERNET:
      case DSPU_DLC_TOKEN_RING:
      case DSPU_DLC_CHANNEL:
      {
          if (snpa_get_type(GetPuAddrP(puP)) == STATION_IEEE48)
            printf(" RMAC %s", MacToAscii((byte *) &addr));
          else 
            printf(" RMAC --");

          if (puP->remoteSap != NoSap) 
            printf(" RSAP %d", puP->remoteSap);
          else 
            printf(" RSAP --");

          if (puP->lSap != NoSap)
            printf(" LSAP %d\n", puP->lSap);
          else
            printf(" LSAP --\n");

          break;
      }
      case DSPU_DLC_FRAME_RELAY:
      {
          if (snpa_get_type(GetPuAddrP(puP)) == STATION_FR10) {
            hword dlciNumber;
            memcpy(&dlciNumber, &addr, 2);
            printf(" DLCI %d", dlciNumber);
          } else {
            printf(" DLCI -- ");
          }

          if (puP->remoteSap != NoSap) 
            printf(" RSAP %d", puP->remoteSap);
          else 
            printf(" RSAP --");

          if (puP->lSap != NoSap)
            printf(" LSAP %d\n", puP->lSap);
          else
            printf(" LSAP --\n");

          break;
      }
      default:
          printf("\n");
          break;

  } /* End switch(dlcType) */

  printf("     XID %s RETRIES %d RETRY_TIMEOUT %d\n",
      ByteToHexAscii(&puP->xid.blk_num[0], 4),
      puP->retryInfo.maxCount, puP->retryInfo.maxTimeout );
      
  printf("     WINDOW %d MAXIFRAME %d\n",
      puP->cfgConnParms.windowSize, puP->cfgConnParms.maxIFrame);

} /* End PrintDspuDlcPuAddrInfo() */


void DefaultPuWrite(Pu* this)
{
   nv_write(TRUE,"!\ndspu default-pu");
   nv_add(this->cfgConnParms.windowSize != DSPU_DEFAULT_WINDOW, 
          " window %d", this->cfgConnParms.windowSize);
   nv_add(this->cfgConnParms.maxIFrame != DSPU_DEFAULT_MAXIFRAME, 
          " maxiframe %d", this->cfgConnParms.maxIFrame);
   PrintRange(this);  /* LU luStart luEnd POOL poolName */
}


/* The following enum definition is to control the order of nvram calls
* such that POOL_WRITE will appear before LU_WRITE
*/
typedef enum {LU_WRITE, POOL_WRITE} LuPoolWrite;

static void LuOrPoolWrite(Pu* this, LuPoolWrite luOrPool);   /* forward declaration */


static inline void PuWriteRsrbStart(Pu* puP)
{
    nv_write(puP->adaptorId.dlcType == DSPU_DLC_RSRB, "%s rsrb start %s", 
                  (IsPuSna(puP) ? "sna" : "dspu"), GetPuName(puP));
}

static inline void PuWriteVdlcStart(Pu* puP)
{
    nv_write(puP->adaptorId.dlcType == DSPU_DLC_VDLC, "%s vdlc start %s", 
                  (IsPuSna(puP) ? "sna" : "dspu"), GetPuName(puP));
}

void PuWrite(Pu* this)
{
   char* hostOrPu;
   char* xid;
   byte  defaultRetry;
   boolean writeXid = TRUE;

   ushort addrLen;
   uchar  addr[STATIONLEN_MAX];
   SNPAType_e stationType;

   char qllcSubAddr[3];
   /*
    *
    * Don't write PU if SNA command (GetSnaMode() == TRUE), 
    *   but PU is not SNA PU (IsPuSna() == FALSE)
    *
    * Don't write PU if DSPU command (GetSnaMode() == FALSE), 
    *   but PU is not DSPU PU (IsPuSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsPuSna(this)) || (!GetSnaMode() && IsPuSna(this)) )
     return;

   if (this->isDefault)
       return;         /* dont write to pu if default */

   stationType = snpa_get_type(GetPuAddrP(this));

   if (this->puType == Upstream){
      hostOrPu = "host";
      xid = "snd";
      defaultRetry = 0xff;
   }
   else{
      hostOrPu = "pu";
      xid  = "rcv";
      defaultRetry = DefaultRetry;
      if (this->xid.xid == NoXid)   /* if XID is 00000000                 */
          writeXid = FALSE;         /*   don't nv_write for downstream PU */
      else if ((stationType == STATION_IEEE48) ||
               (stationType == STATION_X121) ||
               (stationType == STATION_SDLC) ||
               (stationType == STATION_FR10) )
          writeXid = FALSE;    /* don't write xid-rcv parm when addr cfgd */
   }

   nv_write(TRUE, "!\n%s %s %s ", 
              (IsPuSna(this) ? "sna":"dspu"), hostOrPu, &this->name);

   nv_add(writeXid == TRUE, "xid-%s %s ", 
              xid, ByteToHexAscii(this->xid.blk_num, 4));
   
   addrLen = snpa_get_length(GetPuAddrP(this));
   snpa_copy_to_buffer(GetPuAddrP(this), (uchar *) &addr);

   if (stationType == STATION_FR10) {
       hword dlciNumber;
       memcpy(&dlciNumber, &addr, 2);
       nv_add(TRUE, "dlci %d ", dlciNumber);
       nv_add(this->remoteSap != NoSap, "rsap %d ",this->remoteSap);
       nv_add(this->lSap != NoSap, "lsap %d ", this->lSap);
   }
   else if (stationType == STATION_SDLC) {
       nv_add(TRUE, "sdlc %s ", ByteToHexAscii((byte *) &addr, addrLen));
   }
   else if (stationType == STATION_X121) {
       nv_add(TRUE, "x25 %s ", addr);
       QllcSubAddrToAscii(this->lSap, qllcSubAddr);
       nv_add(this->lSap != NoSap, "qllc %s ",qllcSubAddr);
   }
   else if (stationType == STATION_IEEE48) {
       nv_add(TRUE, "rmac %s ", MacToAscii((byte *) &addr));
       nv_add(this->remoteSap != NoSap, "rsap %d ",this->remoteSap);
       nv_add(this->lSap != NoSap, "lsap %d ", this->lSap);
   }
    
   if (this->configuredIdb != NULL) { 
      nv_add(TRUE, "interface %s ", this->configuredIdb->hwptr->hw_namestring);
   }

   nv_add(this->cfgConnParms.windowSize != DSPU_DEFAULT_WINDOW,
      "window %d ", this->cfgConnParms.windowSize);

   nv_add(this->cfgConnParms.maxIFrame != DSPU_DEFAULT_MAXIFRAME,
       "maxiframe %d ", this->cfgConnParms.maxIFrame);

   nv_add(this->retryInfo.maxCount != defaultRetry,
       "retries %d ", this->retryInfo.maxCount);

   nv_add(this->retryInfo.maxTimeout != DefaultTimeout,
       "retry-timeout %d ", this->retryInfo.maxTimeout);
   
   if (this->puType == Downstream){
      LuOrPoolWrite(this, LU_WRITE);
      PrintRange(this);
   }
   else {
      nv_add(this == GetFocalPu(),"focalpoint");
      LuOrPoolWrite(this, POOL_WRITE);
   }

   if (IsPuSna(this) && IsPuStarted(this)) {
       PuWriteRsrbStart(this);
       PuWriteVdlcStart(this);
   }

} /* End PuWrite() */


void DspuPuWriteRsrbStart(Pu* puP)
{

   /*
    * Don't write if snaPu or if defaultPU
    *
    */
   if (IsPuSna(puP) || IsPuDefault(puP)) {
       return;
   }

   PuWriteRsrbStart(puP);

} /* End DspuPuWriteRsrbStart() */

void DspuPuWriteVdlcStart(Pu* puP)
{

   /*
    * Don't write if snaPu or if defaultPU
    *
    */
   if (IsPuSna(puP) || IsPuDefault(puP)) {
       return;
   }

   PuWriteVdlcStart(puP);

} /* End DspuPuWriteVdlcStart() */

static void LuOrPoolWrite(Pu* this, LuPoolWrite luOrPool)
{
   enum {Start, HostData, PoolData, PrintHost, PrintPool} state;
   hword index;
   hword loIndex;
   byte luStart = 0;
   byte hostLu = 0;
   byte puLu = 0;

   byte luRange = 0;
   PoolCB* poolP = 0;
   Lu* lastLuP = 0;
   Pu* lastUpPuP = 0;
     
   Lu* luP = GetFirstChild(&this->luChildren);
   state = Start;
   while (luP){
      index = GetUpLuNumber(luP);
      loIndex = GetDownLuNumber(luP);
      if (state == PoolData){
	 if (GetLuPool(luP) != poolP || hostLu + luRange != (byte)index){
	    state = PrintPool;
	    continue;
	 }
	 else
	    luRange++;
      }
      else if (state == HostData){
	 if ((GetLuPool(luP) != 0) ||               /* Not pooled LU         */
             (GetLuUpPuP(luP) != lastUpPuP) ||      /* Same hostPU           */
             (hostLu + luRange != (byte)index) ||   /* hostLU still in range */
             (puLu + luRange != (byte) loIndex)) {  /* puLU still in range   */
	    state = PrintHost;
	    continue;
	 }
	 else
	    luRange++;

      }
      else {
	 if (state == PrintHost){
            if (luOrPool == LU_WRITE){
	       nv_write(TRUE, "dspu lu %d ", luStart);
               nv_add(TRUE, "%d ",(luStart+luRange-1));
               nv_add(TRUE, "host %s %d",GetHostName(lastLuP), hostLu);
	    }
	 }
	 else if (state == PrintPool){
	    nv_write(luOrPool == POOL_WRITE,"dspu pool %s host %s lu %d %d",  
                     poolP->name,
                     &this->name, hostLu, (hostLu+luRange-1));
	    if (poolP->inactivityTimeout)
	       nv_add(luOrPool == POOL_WRITE," inactivity-timeout %d",poolP->inactivityTimeout);
	 }
	 hostLu = (byte)index;
         puLu = (byte)loIndex;
	 luRange = 1;
	 poolP = GetLuPool(luP);
	 state = (poolP)? PoolData: HostData;
	 luStart = GetDownLuNumber(luP);
      }
      lastLuP = luP;
      lastUpPuP = GetLuUpPuP(lastLuP);
      luP = GetNextChild(&this->luChildren, luOrPool == LU_WRITE? loIndex: index);
   }
      
   /* we terminate when luP = 0, but there may still data to be 
    * displayed at state HostData and Pool Data 
    */
   if (state == HostData){
      if (luOrPool == LU_WRITE){
	  nv_write(TRUE, "dspu lu %d ", luStart);
          nv_add(TRUE, "%d ",(luStart+luRange-1));
          nv_add(TRUE, "host %s %d",GetHostName(lastLuP), hostLu);
      }
   }
   else if (state == PoolData){
      nv_write(luOrPool == POOL_WRITE, "dspu pool %s host %s lu %d %d", 
                 poolP->name, &this->name, hostLu, (hostLu+luRange-1));
      if (poolP->inactivityTimeout)
	 nv_add(luOrPool == POOL_WRITE," inactivity-timeout %d",poolP->inactivityTimeout);
   }
}

void PuWriteOut(Pu* this, AdaptorId* idP)
{

    /*
     *
     * Don't write PU if SNA command (GetSnaMode() == TRUE),
     *   but PU is not SNA PU (IsPuSna() == FALSE)
     *
     * Don't write PU if DSPU command (GetSnaMode() == FALSE),
     *   but PU is not DSPU PU (IsPuSna() == TRUE)
     *
     */
    if ( (GetSnaMode() && !IsPuSna(this)) || (!GetSnaMode() && IsPuSna(this)) )
      return;

    nv_write(this->adaptorId.dlcType == idP->dlcType &&
             this->adaptorId.u.adaptornumber == idP->u.adaptornumber &&
             this->adaptorId.adaptorSubIfNumber == idP->adaptorSubIfNumber,
                  "%s start %s", (IsPuSna(this) ? "sna": "dspu"), &this->name);

}

/*=====================================================================*
* ShowPu: SHOW DSPU [resourceName] all
*======================================================================*
*
* Description:
*  Show PU with 3 different types:
*
*    SHOW DSPU         : show interface type only
*    SHOW DSPU name    : show all configured PU parameter
*    SHOW DSPU name ALL: show all configured PU parameter and LU
*   
*/
void ShowPu(Pu* this, hword showLevel)
{
   hword lu;
   Lu*   luP;
   AdaptorId* idP = 0;
   const char* status;
   hword configuredLu = 0;
   hword activeConfiguredLu = 0;
   hword availableLu = 0;
   hword apiLu = 0;
   hword activeApiLu = 0;


  /*
   *
   * Don't write PU if SNA command (GetSnaMode() == TRUE),
   *   but PU is not SNA PU (IsPuSna() == FALSE)
   *
   * Don't write PU if DSPU command (GetSnaMode() == FALSE),
   *   but PU is not DSPU PU (IsPuSna() == TRUE)
   *
   */
  if ( (GetSnaMode() && !IsPuSna(this)) || (!GetSnaMode() && IsPuSna(this)) )
    return;


  /* Check Rqd state of PU                 */
  /*   If PU is inactive, pending deletion */
  /*     don't bother displaying Pu info   */
  /* ------------------------------------- */
  if (this->puFlag & RqdPendDelete)
     return;

   printf("%s ", (IsPuSna(this) ? " sna" : "dspu"));
   printf("%s %s ", (this->puType == Upstream)? "host": "pu", GetPuName(this));

   if (this->adaptorId.dlcType != DSPU_DLC_UNDEFINED)
       idP = &this->adaptorId;
   else if (this->lsP)              /* if link station is defined */
      idP = SapGetAdaptorId(LsGetSap(this->lsP));

   if (idP)
       PrintDspuDlcTypeInfo(idP);

   if (this->lsP == 0)
       status = "inactive";

   else if (this->sscpPuFsmState == T5Reset)
        /* PU is in Reset state, so display the Link Station FSM */
          status = linkStationStatus[(this->lsP)->fsmState];    /* print link station */
   else if (this->puType == Upstream)
       status = upPuStatus[this->sscpPuFsmState];
   else
      status = downPuStatus[this->sscpPuFsmState];

   printf(" PU STATUS %s\n", status);
   
   if (showLevel & ShowAllPu){
  
      printf("    ");
      if (this->lsP) 
        PrintDspuDlcLsAddrInfo(idP, this);
      else 
        PrintDspuDlcPuAddrInfo(idP, this);
   }
   
   /*  FRAMES RECEIVED nnnn FRAMES SENT nnnn
    *  LUs configured nnn LUs ACTIVE mmm
    */
   printf("     FRAMES RECEIVED %d FRAMES SENT %d\n",this->frameReceives, 
                                           this->frameSents);
   
   /* Find the total of all active and configured LUs. 
    * The number of downstream configured LUs is the number 
    * of pool range control blocks with the given start.
    */
   
   for (lu = 1; lu < 256; lu++){
      if ((luP = GetChild(&this->luChildren, lu)) != 0){
         if (showLevel & ShowAllLu)
            ShowLu(luP, this->puType);
         
         if (QueryLuApiLu(luP)) {
            if (QueryLuActive(luP))
               activeApiLu++;
            apiLu++;
         }
         else {
            if (QueryLuActiveUpOrDown(luP, this->puType))
               activeConfiguredLu++;
            configuredLu++;
         }
      }
      else if (this->puType == Downstream){
         PoolRange* poolP;
         if ((poolP = FindPoolRange(this, (byte)lu)) != 0){
            configuredLu++;                   /* this is in pool range */
            if (showLevel & ShowAllLu)
               PrintLuInPool((byte)lu, poolP->poolName);
         }
      }
      else if (this->puType == Upstream) {
         if (PuLuMapIsLuActive(this, lu)) {
            /* an LU which is active but not configured */
            availableLu++;
            if (showLevel & ShowAllLu)
               printf("     LU %03d ACTIVATED BY HOST BUT NOT USED\n", lu);
         }
      }
   }
   if ((showLevel & ShowAllLu) == 0) {
       printf("     LUs USED BY DSPU %d LUs ACTIVE %d\n",
              configuredLu, activeConfiguredLu);
       printf("     LUs USED BY API %d LUs ACTIVE %d\n",
              apiLu, activeApiLu);
       printf("     LUs ACTIVATED BY HOST BUT NOT USED %d\n",
              availableLu);
   }

}

/*=====================================================================*
* PuQueryFlowOff
*======================================================================*
*
* Description:
*   Calls LsQueryFlowOff to determine if the last flow received 
* was STOP, in which case TRUE is returned.
*   
*/
boolean PuQueryFlowOff(Pu* this)
{
   return this->lsP? LsQueryFlowOff(this->lsP): FALSE;
}

/*=====================================================================*
* DebugPu: 
*======================================================================*
*
* Description:
*   Turns debugging on for the PU and its associated LUs
*
*/
void DebugPu (Pu* this, hword debugLevel)
{
   this->debugLevel |= (byte) debugLevel;   
   DoToAllChildrenParm(&this->luChildren, DebugLu, debugLevel);
} 

/*=====================================================================*
* NoDebugPu: 
*======================================================================*
*
* Description:
*   Turns debugging off for the PU and its associated LUs
*   
*/
void NoDebugPu (Pu* this, hword debugLevel)
{
   this->debugLevel &= (byte) ~debugLevel;   
   DoToAllChildrenParm(&this->luChildren, NoDebugLu, debugLevel);
}


/*=====================================================================*
* DebugPuWithName: 
*======================================================================*
*
* Description:
*   Turns debugging on for the PU and its associated LS 
*
*/
void DebugPuWithName (Pu* this, hword debugLevel)
{

    if (this->lsP)                       /* Is there a link station ?      */
       DebugLs(this->lsP, debugLevel);   /* Link station will call DebugPu */
    else
       DebugPu(this, debugLevel);        /* Debug PU only                  */
    DoToAllChildrenParm(&this->luChildren, DebugLu, debugLevel);

} /* DebugPuWithName() */

/*=====================================================================*
* NoDebugPuWithName: 
*======================================================================*
*
* Description:
*   Turns debugging off for the PU and its associated LS 
*   
*/
void NoDebugPuWithName (Pu* this, hword debugLevel)
{

    if (this->lsP)                       /* Is there a link station ?      */
       NoDebugLs(this->lsP, debugLevel); /* Link station will call DebugPu */
    else
       NoDebugPu(this, debugLevel);      /* debug PU only                  */
    DoToAllChildrenParm(&this->luChildren, NoDebugLu, debugLevel);

} /* End NoDebugPuWithName() */

/*=====================================================================*
* PrintLuInPool
*======================================================================*
*
* Description:
*   This routine displays the LU.
*   
**/
void PrintLuInPool(byte lu, const char* name)
{
   printf("     LU %03d PEER PU          PEER LU     status POOL %s\n", 
          lu, name);
}

/*======================================================================*
* PuConnectOutErrorPrint:
*======================================================================*
*
* Description:
*   Print console message saying connect out failed.
*
**/
void
PuConnectOutErrorPrint (Pu* this)
{

  if (this && this->debugLevel != 0) {
    if (this->puType == Upstream)
      errmsg(&msgsym(LSConnOutToHostFailed, DSPU), this->name.name);
    else
      errmsg(&msgsym(LSConnOutToPuFailed, DSPU), this->name.name);
  }

}  /* PuConnectOutErrorPrint. */


/*======================================================================*
* PuConnectInErrorPrint:
*======================================================================*
*
* Description:
*   Print console message saying connect in failed.
*
**/
void
PuConnectInErrorPrint (Pu* this, hwaddrtype *addrP)

{

  ushort addrLen;
  uchar  addr[STATIONLEN_MAX];

  addrLen = snpa_get_length(GetPuAddrP(this));
  snpa_copy_to_buffer(GetPuAddrP(this), (uchar *) &addr);

  if (this) {
    if (this->puType == Upstream) {
        errmsg(&msgsym(LSConnInFromHostFailed, DSPU), this->name.name,
                ByteToHexAscii((byte *) &addr, addrLen));
     } else {
         errmsg(&msgsym(LSConnInFromPuFailed, DSPU), this->name.name,
                 ByteToHexAscii((byte *) &addr, addrLen));
     }
  } else {
     errmsg(&msgsym(LSConnInFailed, DSPU), 
             ByteToHexAscii((byte *) &addr, addrLen));
  }

}  /* PuConnectInErrorPrint. */

/*============================================================================*
* PuQueryUpdtNegConnParms: Query and update negotiated connection parameters.*
*============================================================================*
*
* Description:
* Query and update negotiated connection parameters. Note that we will
* negotiate down, never up.
*
**/
NegErrE
PuQueryUpdtNegConnParms (Pu *this,
                         byte windowSize,
                         hword maxIFrame)

{
  this->negConnParms.maxIFrame = maxIFrame;

  if (windowSize < this->cfgConnParms.windowSize)
      this->negConnParms.windowSize = windowSize;
  else 
      this->negConnParms.windowSize = this->cfgConnParms.windowSize;

  return Xid_NP_NoError;

}  /* PuQueryUpdtNegConnParms. */


/*============================================================================*
* PuSolicitLLCParms:
*============================================================================*
*
* Description:
*   Return LLC parameters, either configured or negotiated; and some are
*   dependent on whether the pu is a PU or HOST.
*   Note that puP could be zero, in this case, default value is used
*
**/
LlcParmsT*
PuSolicitLLCParms (LlcParmTypeE llcParmType,
                   Pu* puP)

{

   static LlcParmsT llcParms;           /* Only want persistence quality. */

   memset(&llcParms, 0, sizeof(LlcParmsT));
  
   if (puP == 0){
      llcParms.dMaxOut = DSPU_DEFAULT_WINDOW;
      llcParms.dMaxIn = DSPU_DEFAULT_WINDOW;
      llcParms.dMaxIField = DSPU_DEFAULT_MAXIFRAME;
      llcParms.dExplorerOption = EO_INBOUND_DEFAULT;
      return (&llcParms);
   }
   if (llcParmType == LLC_Cfg_Parms) {
      llcParms.dMaxOut = puP->cfgConnParms.windowSize;
      llcParms.dMaxIn = puP->cfgConnParms.windowSize;
      llcParms.dMaxIField = puP->cfgConnParms.maxIFrame;
   }
   else {
      llcParms.dMaxOut = puP->negConnParms.windowSize;
      llcParms.dMaxIn = puP->negConnParms.windowSize;
      llcParms.dMaxIField = puP->negConnParms.maxIFrame;
   }

   /* Only validate RIF for a connect out */
   if (IsPuStarted(puP))
       llcParms.dExplorerOption = EO_OUTBOUND_DEFAULT;
   else
       llcParms.dExplorerOption = EO_INBOUND_DEFAULT;

   return (&llcParms);

}  /* PuSolicitLLCParms. */


/*============================================================================*
* PuSolicitReqOpnStnReq: *
*============================================================================*
*
* Description:
*   Construct a REQ_OPNSTN.REQ packet.
*
**/
ReqOpnStnReq_t *
PuSolicitReqOpnStnReq (Pu *puP, word uCepId)
{
   
  LlcParmsT* llcParmsP;

  llcParmsP = PuSolicitLLCParms (LLC_Cfg_Parms, puP);

  return Cnstrct_ReqOpnStnReq (NON_BLOCKING,  uCepId, CLS_NO_CEP,
                               GetPuAddrP(puP), puP->remoteSap, llcParmsP,
                               0, 0);   /* no rif for connect out */

} /* PuSolicitReqOpnStnReq */

   
/*=====================================================================*
* PuFlowControlFromLs, PuFlowControlFromLu
*======================================================================*
*
**/

/* call all lu with flow control.
* Implementation note:
*   3 ways can be implemented.
*   1. Scan through all children with GetNextChild. 
*      Iteration is required and GetNextChild is not efficient.
*   2. Use existing DoToAllChildren  which parameters cannot be passed, so
*      the flow parameter will have to pass as a global parameter.
*      This is efficient but ugly.
*   3. Create another version of DoToAllChildren to allow parameters 
*      passing.
*      This is used here. 
*/

void PuFlowControlFromLs(Pu* this, FlowControlE flow)
{
   hword flowEncode = (this->puType == Upstream)? FlowUpstream: 0;
   flowEncode |= flow;
   
   DoToAllChildrenParm(&this->luChildren, LuFlowControl, flowEncode);
}

void PuFlowControlFromLu(Pu* this, FlowControlE flow)
{
   if (this->lsP)
      LsFlowControlFromPu(this->lsP, flow);
}


/*=====================================================================*
* PuMatched
*======================================================================*
*
* Description:
*   Returns TRUE if a successful match has been found. 
*
**/
boolean
PuMatched (Pu* this,                    /* PU to match against     */
           XidAddrSapT* xmsP)           /* Xid, Addr, & Sap triple */

{
  boolean puMatch = FALSE;
  if ( (this->lSap != NoSap) && (this->lSap != SapGetSapNum(xmsP->lsapP)) )
      return FALSE;

  switch (xmsP->searchMode) {

    case XMS_AddrSap:
      puMatch = ( (this->remoteSap == NoSap || this->remoteSap == xmsP->rsap) 
                  && (snpa_equ(GetPuAddrP(this), &xmsP->addr)) );
      break;

    case XMS_AddrXid:
  
      puMatch = ( snpa_is_invalid(GetPuAddrP(this)) || 
                  snpa_equ(GetPuAddrP(this), &xmsP->addr) ) && 
                ( (this->xid.xid == NoXid) || 
                  ( (xmsP->u_xid.xid != NoXid) && 
                    (this->xid.xid == xmsP->u_xid.xid) ));
      break;

    default:                           /* Programmer error. Never get here. */
      break;
  }

  /* 
   * If common_str_interface parameter configured for PU
   *   ensure that connectIn interface matches as well
   *
   */
  if (GetPuConfiguredIdb(this) ) {
    AdaptorId* idP = SapGetAdaptorId(xmsP->lsapP);
    puMatch = ( puMatch && (GetPuConfiguredIdb(this) == idP->dlcIdb) );
  }
  return (puMatch);

}  /* PuMatched. */


/*=====================================================================*
* PuFindInactive
*======================================================================*
*
* Description:
*   Calls PuMatched to find an inactive PU with either matching 
* MAC addr and SAP, or XID. If found, it returns the pointer to
* the PU otherwise it returns 0. 
*
**/
Pu*
PuFindInactive(Sibling* inactPuRoot, XidAddrSapT* xmsP)
{
   return QueryAllChildrenParm(inactPuRoot, (ProcQChild1)PuMatched, xmsP);
}



/*=====================================================================*
* GetPuOperState()
*======================================================================*
*
* Description:
*   Returns TRUE if SSCP state is active
*
**/
boolean GetPuOperState(LinkType puType, hword puState)
{

  boolean T2PuOperStateTable[T2Max] =
            { FALSE,                    /* T2Reset         */
              TRUE,                     /* T2Active        */
              TRUE                      /* T2Busy          */
            };

  boolean T5PuOperStateTable[T5Max] =
            { FALSE,                    /* T5Reset         */
              FALSE,                    /* T5PendingActive */
              TRUE,                     /* T5Busy          */
              TRUE,                     /* T5Max           */
              TRUE                      /* T5PendingInact  */
            };


  if ( puType == Upstream)
    return(T2PuOperStateTable[puState]);
  else
    return(T5PuOperStateTable[puState]);

} /* End GetPuOperState() */


/*
 * PuGetNextChild
 *
 * Description:
 *   Get next lu child taking into account if PU is up or down
 */
static Lu* PuGetNextChild(Pu* puP, Lu* luP)
{
    hword luAddr;

    if (GetPuType(puP) == Upstream) {
        luAddr = GetUpLuNumber(luP);
    } else {
        luAddr = GetDownLuNumber(luP);
    }
    return (GetNextChild(GetLuChildren(puP), luAddr));
}


/*=====================================================================*
* GetPuActiveSscpLuSessions()
*======================================================================*
*
* Description:
*   Returns number of active SSCP-LU sessions on the PU
*
**/
word GetPuActiveSscpLuSessions(Pu* puP)
{

  boolean luOperState;
  word activeSscpLuSessions = 0;


  Lu* luP = GetFirstChild(GetLuChildren(puP));
  while (luP) {
    if (!QueryLuApiLu(luP)) {  /* count only DSPU LUs */
      luOperState = QueryLuActiveUpOrDown( luP, GetPuType(puP));
      if (luOperState)
        activeSscpLuSessions++;
    }
    luP = PuGetNextChild(puP, luP);
  }

  return (activeSscpLuSessions);

} /* End GetPuActiveSscpLuSessions() */


/*=====================================================================*
* GetPuInactiveSscpLuSessions()
*======================================================================*
*
* Description:
*   Returns number of inactive SSCP-LU sessions on the PU
*
**/
word GetPuInactiveSscpLuSessions(Pu* puP)
{

  Lu* luP;
  PoolRange* prP;

  byte luIdx;
  boolean luOperState;
  word inactiveSscpLuSessions = 0;


  /* Count inactive sessions on the LU list */
  luP = GetFirstChild(GetLuChildren(puP));
  while (luP) {
    if (!QueryLuApiLu(luP)) {  /* count only DSPU LUs */
      luOperState = QueryLuActiveUpOrDown( luP, GetPuType(puP));
      if (!luOperState)
        inactiveSscpLuSessions++;
    }
    luP = PuGetNextChild(puP, luP);
  }

  /* Count inactive sessions in PoolRanges */
  prP = (PoolRange *) GetFirstQueue(PuGetPoolRangeHeader(puP));
  while(prP) {

    luIdx = GetPoolRangeDnLuStart(prP);
    while(luIdx <= GetPoolRangeDnLuEnd(prP)) {
      if (GetChild(GetLuChildren(puP), luIdx) == 0)
        inactiveSscpLuSessions++;
      luIdx++;
    }
    prP = NextQueueElement(prP);
  }

  return (inactiveSscpLuSessions);

} /* End GetPuInactiveSscpLuSessions(puP) */


/*=====================================================================*
* GetPuActiveLuLuSessions()
*======================================================================*
*
* Description:
*   Returns number of active LU-LU sessions on the PU
*
**/
word GetPuActiveLuLuSessions(Pu* puP)
{

  word activeLuLuSessions = 0;


  Lu* luP = GetFirstChild(GetLuChildren(puP));
  while (luP) {
    if (QueryLuLuActive(luP))
      activeLuLuSessions++;
    luP = PuGetNextChild(puP, luP);
  }

  return (activeLuLuSessions);

} /* End GetPuActiveLuSessions() */


/*=====================================================================*
* FindPuName()
*======================================================================*
*
* Description:
*   Searches puRoot structures for Pu control block matching puName
*
*   List order of puRoot structures as follows:
*     upstrLsRoot  (  active UpStream PU)
*     dnstrLsRoot  (  active DnStream PU)
*     upstrPuRoot  (inactive UpStream PU)
*     dnstrPuRoot  (inactive DnStream PU)
*
**/
Pu* FindPuName(char* puName)
{

  LinkStn* lsP;
  Pu*      puP = 0;


  /* Try searching for puName on UplsRoot */
  lsP = QueryAllChildrenParm( GetUplsRoot(),
                              (ProcQChild1) QueryFindChild,
                              (void *) puName);
  if (lsP)
    puP = LsChild(lsP);

  else {

    /* Try searching for puName on DnlsRoot */
    lsP = QueryAllChildrenParm( GetDnlsRoot(),
                                (ProcQChild1) QueryFindChild,
                                (void *) puName);
    if (lsP)
      puP = LsChild(lsP);

    else {

      /* Try searching for puName on InactUpuRoot */
      puP = QueryAllChildrenParm( GetInactUpuRoot(),
                                  (ProcQChild1)QueryMatchPuName,
                                  (void *) puName);

      if (puP == 0) {

        /* Try searching for puName on InactDpuRoot */
        puP = QueryAllChildrenParm( GetInactDpuRoot(),
                                    (ProcQChild1)QueryMatchPuName,
                                    (void *) puName);

      } /* End if - puName not found on InactUpuRoot */

    } /* End else - puName not found on DnlsRoot */

  } /* End else - puName not found on UplsRoot */

  return(puP);

} /* End FindPuName() */


/*=====================================================================*
* CheckPuOperStateChanged()
*======================================================================*
*
* Description:
*   Returns TRUE is dspuPuOperState has changed
*   Generates SNMP trap when dspuPuOperState has changed
*
**/
boolean CheckPuOperStateChanged(Pu* puP, hword newPuState) 
{

 boolean currentPuOperState;
 boolean newPuOperState;

 boolean puOperStateChanged = FALSE;

 currentPuOperState = GetPuOperState(GetPuType(puP), GetPuSscpState(puP));
 newPuOperState = GetPuOperState(GetPuType(puP), newPuState);

 if (currentPuOperState != newPuOperState) {
   puOperStateChanged = TRUE;
   SetPuStateChgTime(puP);
   DspuNetMgmtNotification((void *) puP, PU_STATECHANGE, 
                            newPuOperState, GetPuType(puP));
 }

 return(puOperStateChanged);

} /* End CheckPuOperStateChanged() */
