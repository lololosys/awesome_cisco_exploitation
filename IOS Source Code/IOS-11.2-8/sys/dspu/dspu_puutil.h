/* $Id: dspu_puutil.h,v 3.3.52.2 1996/07/24 01:23:36 szdravko Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_puutil.h,v $
 *------------------------------------------------------------------
 * DSPU PU Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 *   PU utiltiy programs. 
 *   1. Link station parameters: ConnParm, RetryInfo, XID, sap, rmac
 *   2. Show and Debug
 *   3. Segmentation.
 *   4. Pool related.
 *------------------------------------------------------------------
 * $Log: dspu_puutil.h,v $
 * Revision 3.3.52.2  1996/07/24  01:23:36  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/05/17  10:48:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.1  1996/03/28  08:20:32  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.6.1  1996/01/20  06:08:48  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3  1995/11/30  19:09:00  ppearce
 * CSCdi44719:  sna rsrb start command does not appear in config
 *   Separate nv_write of "sna rsrb start" and "dspu rsrb start"
 *
 * Revision 3.2  1995/11/17  09:05:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  17:03:10  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _puutil_h
#define _puutil_h

#include "dtypes.h"
#include "dspu_cps.h"
#include "snaprim.h"
#include "bfrprim.h"
#include "dspu_xid.h"

/* Define default values */
/* --------------------- */
#define DefaultTimeout   30
#define DefaultRetry     4
#define DefaultLsap_Pu   8
#define DefaultLsap_Host 12
#define DefaultRsap      4
#define DefaultLuRange   1

/* a mac value that is impossible to indicate MAC is not defined in PU */
#define NO_DLCI 0x00
#define NO_SDLC 0x00
#define NO_X25  0x00
#define NO_MAC  0xfa


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
             hword* offset);          /* offset to the data */



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
          (Pu* puP
          ,byte  luStart
          ,byte luRange
          );


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
void
ConnectAllPoolLusToPu
          (Pu   * dnPuP
          );

#define ShowAllLu   1
#define ShowAllPu   2

/* showLevel is bit mask of ShowAllLu and ShowAllPu */

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
void ShowPu(Pu* this, hword showLevel);

void PuWrite(Pu* this);
void DspuPuWriteRsrbStart(Pu* puP);
void DspuPuWriteVdlcStart(Pu* puP);
void DefaultPuWrite(Pu* this);

/*=====================================================================*
* DebugPu: 
*======================================================================*
*
* Description:
*   Turns debugging on for the PU and its associated LUs
*   
*/
void DebugPu (Pu* this, hword debugLevel);

/*=====================================================================*
* NoDebugPu: 
*======================================================================*
*
* Description:
*   Turns debugging off for the PU and its associated LUs
*   
*/
void NoDebugPu (Pu* this, hword debugLevel);

/*=====================================================================*
* PuQueryFlowOff
*======================================================================*
*
* Description:
*   Calls LsQueryFlowOff to determine if the last flow received 
* was STOP, in which case TRUE is returned.
*   
*/
boolean PuQueryFlowOff(Pu* this);

/*=====================================================================*
* DebugPuWithName: 
*======================================================================*
*
* Description:
*   Turns debugging on for the PU and its associated LS
*   
*/
void DebugPuWithName (Pu* this, hword debugLevel);

/*=====================================================================*
* NoDebugPuWithName: 
*======================================================================*
*
* Description:
*   Turns debugging off for the PU and its associated LS
*   
*/
void NoDebugPuWithName(Pu* this, hword debugLevel);

/*=====================================================================*
* PrintLuInPool
*======================================================================*
*
* Description:
*   This routine displays the LU.
*   
**/
void PrintLuInPool(byte lu, const char* name);

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
ConnectStatus 
ConnectPoolRangeLusToPu(                 
              Pu*        dnPuP,     /* downstream Pu */
              PoolCB*    poolCBP,   /* pool control block */
              PoolRange* poolRgeP); /* pool range control */

void
PuConnectOutErrorPrint (Pu* this);

void
PuConnectInErrorPrint (Pu* this, hwaddrtype *hwaddrP);

typedef enum LlcParmTypeE {
        LLC_Cfg_Parms, LLC_Neg_Parms
        } LlcParmTypeE;

NegErrE
PuQueryUpdtNegConnParms (Pu *puP,
                         byte windowSize,
                         hword maxIFrame);

LlcParmsT*
PuSolicitLLCParms (LlcParmTypeE llcParmType,
                   Pu* puP);

ReqOpnStnReq_t *
PuSolicitReqOpnStnReq (Pu *puP,
                       word uCepid);
                       
void
ParentLinkStationClosed (Pu* this);

void
PuChangeToActive(Pu* this, LinkStn* lsP);

boolean
PuMatched (Pu* this, XidAddrSapT* xmsP);

Pu*
PuFindInactive (Sibling* inactPuRoot, XidAddrSapT* xmsP);
       
boolean
PuDeleteIfInactive (Pu* puP);

void DeletePoolRangeIfMatch(Pu* this, const char* poolName);

/*=====================================================================*
* PuFlowControlFromLs, PuFlowControlFromLu
*======================================================================*
*
**/

/* bit mask for flow control pass from Pu to Lu */

#define FlowOffMask    0xff
#define FlowUpstream   0x100

void PuFlowControlFromLs(Pu* this, FlowControlE flow);
void PuFlowControlFromLu(Pu* this, FlowControlE flow);


/*=====================================================================*
* GetPuActiveSscpLuSessions()
*======================================================================*
*
* Description:
*   Returns number of active SSCP-LU sessions on the PU
*
**/
word GetPuActiveSscpLuSessions(Pu* puP);


/*=====================================================================*
* GetPuInactiveSscpLuSessions()
*======================================================================*
*
* Description:
*   Returns number of inactive SSCP-LU sessions on the PU
*
**/
word GetPuInactiveSscpLuSessions(Pu* puP);


/*=====================================================================*
* GetPuActiveLuLuSessions()
*======================================================================*
*
* Description:
*   Returns number of active LU-LU sessions on the PU
*
**/
word GetPuActiveLuLuSessions(Pu* puP);


/*=====================================================================*
* GetFirstPuFromNextRoot()
*======================================================================*
*
* Description:
*   Returns address of first Pu on next puRoot structure
*
*   List order of puRoot structures as follows:
*     upstrLsRoot  (  active UpStream PU)
*     dnstrLsRoot  (  active DnStream PU)
*     upstrPuRoot  (inactive UpStream PU)
*     dnstrPuRoot  (inactive DnStream PU)
*
**/
Pu* GetFirstPuFromNextRoot(Sibling* puRootP);


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
Pu* FindPuName(char* puName);


/*=====================================================================*
* GetPuOperState()
*======================================================================*
*
* Description:
*   Returns TRUE if SSCP-PU session is active
*
**/
boolean GetPuOperState(LinkType puType, hword puState);


/*=====================================================================*
* CheckPuOperStateChanged()
*======================================================================*
*
* Description:
*   Returns TRUE is dspuPuOperState has changed
*
**/
boolean CheckPuOperStateChanged(Pu* puP, hword newPuState); 

void PuWriteOut(Pu* this, AdaptorId* idP);

#define GetLuDebugLevel(puP) (puP->debugLevel)

#endif

