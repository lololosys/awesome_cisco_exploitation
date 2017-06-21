/* $Id: dspu_adaptor.h,v 3.1.62.1 1996/05/17 10:47:24 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_adaptor.h,v $
 *------------------------------------------------------------------
 * DSPU Adaptor structure.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This defines the structure for adaptors.
 *------------------------------------------------------------------
 * $Log: dspu_adaptor.h,v $
 * Revision 3.1.62.1  1996/05/17  10:47:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.2  1996/05/03  17:18:15  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.1  1996/03/28  08:20:22  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.1.6.1  1996/01/20  06:08:36  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.1  1995/11/09  11:27:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  21:12:54  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/09/20  17:02:41  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef __ADAPTOR_H                         
#define __ADAPTOR_H

#include "dtypes.h"
#include "dspu_cps.h"



typedef enum AdaptorState {
        AdaptorReset,
        AdaptorOpening,
        AdaptorOpened,
        AdaptorClosing,
        AdaptorClosed
        } AdaptorState;


struct Adaptor {
        Parent_t adParentP;              /* Ptr to parent of this structure. */
        Sibling *adChildLocP;           /* Ptr to adaptor child locator. */
        Sibling sapChildren;            /* SAP child locator. */
        AdaptorId   id;
        hword       index;              /* Unique adaptor wide index. */
        AdaptorState state;             /* State adaptor is in. */
        };

/*---- The following interfaces are exported. */

Adaptor *                               /* Ptr to new adaptor structure. */
AdaptorCreate (Parent_t adParentP,       /* Ptr to parent of adaptor. */
               Sibling* adChildLocP,    /* Ptr to adaptor child locator. */
               AdaptorId* idP);         /* Ptr to adaptor id (dlc/port). */

Adaptor *
AdaptorGetOrCreate (AdaptorId* idP);

void
AdaptorDelete (Adaptor *adaptorP);      /* Ptr to adaptor cblk. . */

boolean                                 /* TRUE if found            */
AdaptorFind (Adaptor* this,
             AdaptorId* id);            /* Adaptor number. . */
Adaptor*
FindNextAdaptor (AdaptorId* adaptorIdP);

void
AdaptorReceive (Adaptor *adaptorP,      /* Ptr to adaptor cblk. . */
                TCLSIMsg* clsiBfrP);    /* Ptr to clsi buffer. . */

boolean
AdaptorEnable (Adaptor* this);          /* Ptr to adaptor cblk. . */


hword
AdaptorGetAdaptorNum (Adaptor *adaptorP);

boolean
AdaptorSendActivateSap (Adaptor *adaptorP,
                        word uSapId,
                        byte sapNumber);

void
AdaptorSapClosed(Adaptor* this);


/* "dspu enable-[host|pu]" for a DLC interface */
void AdaptorPrintDLC (Adaptor* this, hword direction);

/* "dspu rsrb enable-[host|pu] lsap <lsap>" for the RSRB/DLC interface */
void AdaptorPrintRsrb (Adaptor* this);

/* "dspu vdlc enable-[host|pu] lsap <lsap>" for the VDLC interface */
void AdaptorPrintVdlc (Adaptor* this);

/* "dspu ncia enable-pu lsap <lsap>" for the NCIA interface */
void AdaptorPrintNcia (Adaptor* this);

/* hword GetAdaptorIndex(Adaptor* this) {return this->index;} */
#define  GetAdaptorIndex(this)      (this->index)

/* byte AdaptorGetSap(Adaptor* this) {return this->localSap;} */
#define AdaptorGetSap(this) (this->localSap)

/* Sibling* GetSapChildren(Adaptor* this) {return &this->sapChildren;} */
#define GetSapChildren(this)        (&(this->sapChildren))

/* const AdaptorId* AdaptorGetId(Adaptor* this) {return &this->id;} */
#define AdaptorGetId(this) (&(this->id))

/* hword GetAdaptorDlcNumber(AdaptorId* adaptorIdP) */
#define GetAdaptorDlcNumber(adaptorIdP)  (adaptorIdP->u.adaptornumber)

/* DlcType_e GetAdaptorDlcType(AdaptorId* adaptorIdP) */
#define GetAdaptorDlcType(adaptorIdP)  (adaptorIdP->dlcType)

/* byte GetAdaptorDlcUnit(AdaptorId* adaptorIdP) */
#define GetAdaptorDlcUnit(adaptorIdP)  (adaptorIdP->u.adaptorPort[UnitIndex])

/* byte GetAdaptorDlcPort(AdaptorId* adaptorIdP) */
#define GetAdaptorDlcPort(adaptorIdP)  (adaptorIdP->u.adaptorPort[PortIndex])

/* hword GetAdaptorDlcSubIf(AdaptorId* adaptorIdP) */
#define GetAdaptorDlcSubIf(adaptorIdP)  (adaptorIdP->adaptorSubIfNumber)

/* Adaptor* FindAdaptor(AdaptorId* adaptorIdP) */
#define FindAdaptor(adaptorIdP)  (QueryAllChildrenParm(GetAdaptorRoot(), \
                                    (ProcQChild1)AdaptorFind, adaptorIdP))

#endif  /* __ADAPTOR_H. */
/*---- End include file: adaptor.h. */
