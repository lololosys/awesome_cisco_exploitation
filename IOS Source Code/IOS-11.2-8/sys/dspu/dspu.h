/* $Id: dspu.h,v 3.3.6.1 1996/05/17 10:47:20 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Claude Alan Cartee
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu.h,v $
 * Revision 3.3.6.1  1996/05/17  10:47:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/05/03  17:18:11  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.2  1996/03/28  08:20:19  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/03/17  17:38:33  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.2.4.1  1996/01/20  06:08:33  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:04:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:02:39  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/09/14 06:15:07  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.1  1995/06/07  20:28:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPU_H                         
#define __DSPU_H

#include "address.h"
#include "../cls/clsi_msg.h"
#include "dtypes.h"
#include "dspu_const.h"
#include "dspu_cps.h"
#include "dspuqueu.h"
#include "dlcparm.h"


/*---------------------------------------------------------------------------*/
/* DSPU Structure. */

 struct Dspu {                   /* DSPU core, also super parent. */
        boolean dspuStarted;            /* TRUE if DSPU Main Process started */
        Sibling adaptorRoot;            /* Root of adaptors. */
        Sibling upstrLsRoot;            /* Root of upstream Link Stations. */
        DspuChild dnstrLsRoot;            /* Root of downstream Link Stations. */
        Sibling upstrPuRoot;            /* Root of upstream inactive Pu. */
        DspuChild dnstrPuRoot;            /* Root of downstream inactive Pu. */
        RsrbParmT rsrb;                 /* RSRB DLC parameters. */
        VdlcParmT vdlc;                 /* VDLC parameters. */
        NciaParmT ncia;			/* NCIA parameters. */
        Pu*   defaultPuP;               /* Default PU configuration. */
        hword debugLevel;               /* global debug level */
        hword SnaOrDspu;                /* Parameter passed by configuration */
        sys_timestamp lastConfigTime;   /* TimeStamp of last configuration */
        } ;

extern Dspu dspu;                       /* DSPU exported to world. */

void  DspuQueuePut (TCLSIMsg* theMsg);


/*---- Exported inlines. */

/* The followings are functional prototype which are implemented inline
 * as shown belows:
 */

/*
void *
GetDspuRoot();
Sibling* GetAdaptorRoot();
Sibling* GetUplsRoot();
Sibling* GetDnlsRoot();
Sibling* GetInactUpuRoot();
Sibling* GetInactDpuRoot();
QueueHeader* GetLuPoolRoot();
Pu* GetDefaultPu();
*/


/* void SetDspuStarted(boolean boolVal) */
#define SetDspuStarted(boolVal) (dspu.dspuStarted = boolVal)

/* boolean IsDspuStarted(void) */
#define IsDspuStarted()         (dspu.dspuStarted)

#define SetSnaMode(sna)         (dspu.SnaOrDspu = sna)

#define GetSnaMode()            (dspu.SnaOrDspu)

#define GetDspuRoot()           (&dspu)

#define GetAdaptorRoot()        (&dspu.adaptorRoot)

#define GetUplsRoot()           (&dspu.upstrLsRoot)

#define GetDnlsRoot()           ((Sibling*)&dspu.dnstrLsRoot)

#define GetInactUpuRoot()       (&dspu.upstrPuRoot)

#define GetInactDpuRoot()       ((Sibling*)&dspu.dnstrPuRoot)

#define GetDefaultPu()          (dspu.defaultPuP)

#define GetDebugLevel()         (dspu.debugLevel)

#define SetDebugLevel(debug)    (dspu.debugLevel = debug)

#define GetRsrbParm()           (&dspu.rsrb)

/* hword GetRsrbLocalRing() */
#define GetRsrbLocalRing()  (GetRsrbParm()->localRing)

/* byte GetRsrbBridgeNumber() */
#define GetRsrbBridgeNumber()  (GetRsrbParm()->bridgeNum)

/* hword GetRsrbTargetRing() */
#define GetRsrbTargetRing()  (GetRsrbParm()->targetRing)

/* byte* GetRsrbVirtualMacAddr() */
#define GetRsrbVirtualMacAddr()  (&GetRsrbParm()->virtualMac.mac)

/* AdaptorId* GetRsrbAdaptorId() */
#define GetRsrbAdaptorId()  (&GetRsrbParm()->adaptorId)

/* boolean GetRsrbSnaMode() */
#define GetRsrbSnaMode()  (GetRsrbParm()->snaOrDspu)

/* void SetRsrbSnaMode(boolean sna) */
#define SetRsrbSnaMode(sna)  (GetRsrbParm()->snaOrDspu = sna)


#define GetVdlcParm()           (&dspu.vdlc)

/* hword GetVdlcRingNum() */
#define GetVdlcRingNum()    (GetVdlcParm()->ringNum)

/* byte* GetVdlcVirtualMacAddr() */
#define GetVdlcVirtualMacAddr()  (&GetVdlcParm()->virtualMac.mac)

/* AdaptorId* GetVdlcAdaptorId() */
#define GetVdlcAdaptorId()  (&GetVdlcParm()->adaptorId)

/* boolean GetVdlcSnaMode() */
#define GetVdlcSnaMode()  (GetVdlcParm()->snaOrDspu)

/* void SetVdlcSnaMode(boolean sna) */
#define SetVdlcSnaMode(sna)  (GetVdlcParm()->snaOrDspu = sna)


#define GetNciaParm()           (&dspu.ncia)

/* hword GetNciaServerNum() */
#define GetNciaServerNum()    (GetNciaParm()->serverNum)

/* AdaptorId* GetNciaAdaptorId() */
#define GetNciaAdaptorId()  (&GetNciaParm()->adaptorId)

/* boolean GetNciaSnaMode() */
#define GetNciaSnaMode()  (GetNciaParm()->snaOrDspu)

/* void SetNciaSnaMode(boolean sna) */
#define SetNciaSnaMode(sna)  (GetNciaParm()->snaOrDspu = sna)


/* sys_timestamp GetDspuLastConfigTime() */
#define GetDspuLastConfigTime()  (dspu.lastConfigTime)

/* void SetDspuLastConfigTime(void) */
#define SetDspuLastConfigTime()  (GET_TIMESTAMP(dspu.lastConfigTime))
                

void
DspuReceive (TCLSIMsg *clsiBfrP);

void ProcessDspuReceive(void);

TCLSIMsg*
BlockForBuffer(size_t size);

void FpHasDataToSend(void);

#endif  /* __DSPU_H. */
/*---- End include file: dspu.h. */
