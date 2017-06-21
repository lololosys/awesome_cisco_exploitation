/* $Id: dspu_sap.h,v 3.1.62.2 1996/06/21 02:21:54 slogie Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_sap.h,v $
 *------------------------------------------------------------------
 * DSPU SAP Structure and Methods.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This defines the SAP structure.
 *------------------------------------------------------------------
 * $Log: dspu_sap.h,v $
 * Revision 3.1.62.2  1996/06/21  02:21:54  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.1.62.1  1996/05/17  10:48:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.2  1996/05/03  17:18:25  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.1  1996/03/28  21:07:27  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *   Missed files during VDLC commit
 *
 * Revision 3.1.6.1  1996/01/20  06:08:50  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.1  1995/11/09  11:28:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:12  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SAP_H                         
#define __SAP_H

#include "dtypes.h"
#include "dspu_itimer.h"
#include "dspuqueu.h"
#include "dspu_cps.h"


typedef enum SapInput{
        SapI_ActivateSapCnf, SapI_DeactiveSapCnf, SapI_IdStnInd
        } SapInput;

typedef enum SapState {
        SapClosed, SapOpening, SapClosing, SapOpened
        } SapState;

/*---- The following structures are exported. */

struct Sap {               /* SAP Structure. */
        Adaptor* sapParentP;    /* Ptr to our parent,ie adaptor. */
        Sibling* sapChildLocP;  /* Ptr to SAP Child locator (in parent). */
        byte     sapNumber;     /* SAP number. */
        LinkType sapDirection;  /* Direction of sap. */
        SapState sapState;      /* State of sap. */
        word     pSapId;        /* SAP handle from provider. */
        hword    buffersUsed;   /* Buffers consumed by sap. */
        boolean  snaOrDspu;     /* check if this sap is sna */
        Itimer   sapItimer;     /* Interval timer structure. */
        DspuQueueHeader WrkBBlkHeader; 
        } ;

/*---- The following entrypoints are exported. */

Sap *                                   /* Ptr to sap found. . */
SapFind (word sap_id);                  /* SAP id. . */

Sap *                                   /* Ptr to sap created. . */
SapCreate (byte  sapNumber,             /* SAP number. . */
           LinkType upstream,           /* upstream flag  . */
           Sibling *sapChildLocP,       /* Ptr to child locator. . */
           Adaptor *sapParentP);        /* Ptr to parent cblk. . */

Sap*  QuerySapExisted(Adaptor* adaptorP,
                     byte sap);

boolean
SapDelete (Sap *sapP);                  /* Ptr to sap to delete. . */

void
SapRequestOpenSap (Sap *sapP);

boolean
SapRequestOpenStation (const Sap* this, /* Ptr to sap. . */
                       ReqOpnStnReq_t *reqOpnStnReqP);

void
SapReceive (Sap *sapP,                  /* Ptr to sap. . */
            SapInput sapInput,          /* SAP event to process. . */
            TCLSIMsg *clsiBfrP);        /* Addr of CLSI buffer. . */

void SapRequestCloseSap (Sap *sapP);

/* LinkType SapQueryDirection (Sap *sapP){return sapP->sapDirection;}*/

#define SapQueryDirection(sapP) (sapP->sapDirection)


/* byte SapGetSapNum (Sap *sapP){return sapP->sapNumber;} */
#define SapGetSapNum(sapP) (sapP->sapNumber)

/* SapState SapGetSapState(Sap *sapP) */
#define SapGetSapState(sapP)  (sapP->sapState)


hword
SapGetAdaptorNum (Sap *sapP);

void
SapWorkBufferUnderflow (void);

/*=====================================================================*
* SapQueryGrantWorkBuffer
*======================================================================*
*
* Description:
*   This is a request for a buffer to do work; 
* 
**/
boolean                              
SapQueryGrantWorkBuffer(Sap     *sapP,            
                        hword   lsWorkBufferCount);
                        
/*=====================================================================*
* SapRequestWork
*======================================================================*
*
* Description:
*
**/
void                              
SapRequestWork(Sap*     sapP,
               WrkBBlk* wrkBBlkP);
                   
/*=====================================================================*
* SapFreeGrantedWorkBuffer
*======================================================================*
*
* Description:
*   This increments the BuffersLeft by the number passed. It then scans  
* all the Link Stations registered for work, looking for ones that can be 
* granted buffers to do work.
*
**/
void                                 
SapFreeGrantedWorkBuffer(Sap   *sapP,        
                         hword  numBuffersToFree);                               

/*=====================================================================*
* SapDeregisterForWork
*======================================================================*
*
* Description:
*   This is included to be used by a Link Station which is being destructed
* while its LsWorkBufferCount is non-zero (which should never happen!).
* (In the normal course of events, Link Stations are automatically 
* deregistered.) 
*
**/
void                                  
SapDeregisterForWork(Sap     *sapP,           
                     LinkStn *lsP);                 

/*============================================================================*
 * SapQueryBuffersUsed:
 *============================================================================*
 *
 * Description:
 * Query buffers used by this SAP.
 *
 **/

hword
SapQueryBuffersUsed (Sap *sapP);

void
SapSetActWindow (hword actWindowValue);

hword
SapGetActWindow (void);

/* byte SapGetSapNum (Sap *sapP){return sapP->sapNumber;} */
#define SapGetSapNum(sapP) (sapP->sapNumber)

/* const AdaptorId* SapGetAdaptorId(Sap* this); */
#define SapGetAdaptorId(this)  (AdaptorGetId(this->sapParentP))

/* boolean IsSapSna(Sap* sapP) */
#define IsSapSna(sapP)  (sapP ? sapP->snaOrDspu : 0)

void SapPrintLLC (Sap* this, hword direction);
void SapPrintSDLC(Sap* this, hword direction);
void SapPrintQLLC(Sap* this, hword direction);
void SapPrintRsrb(Sap* this, hword direction);
void SapPrintVdlc(Sap* this, hword direction);
void SapPrintNcia(Sap* this, hword direction);

#endif  /* __SAP_H. */
/*---- End include file: sap.h. */
