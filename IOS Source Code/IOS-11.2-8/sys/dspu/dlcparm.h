/* $Id: dlcparm.h,v 3.2.60.1 1996/05/17 10:47:12 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dlcparm.h,v $
 *------------------------------------------------------------------
 * DSPU DLC Parameter structures.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This header defines structures which contain DLC specific parameters.
 *------------------------------------------------------------------
 * $Log: dlcparm.h,v $
 * Revision 3.2.60.1  1996/05/17  10:47:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/05/03  17:18:09  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/03/28  08:20:17  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.2.4.1  1996/01/20  06:08:31  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:04:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:28:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DLCPARM_H
#define __DLCPARM_H

#include "dtypes.h"


/*---------------------------------------------------------------------------*/
/* RSRB Parameter Structure. */

typedef struct RsrbParmT {              /* Remote Source Route Bridge     */
        hword localRing;                /* Local ring number              */
        byte  bridgeNum;                /* Bridge number                  */
        hword targetRing;               /* Target ring number             */
        MacAddrT virtualMac;            /* Virtual MAC address            */
        AdaptorId adaptorId;            /* RSRB adaptor definition        */
        boolean snaOrDspu;              /* Configured via "sna" or "dspu" */
        } RsrbParmT;

/*---------------------------------------------------------------------------*/
/* VDLC Parameter Structure. */

typedef struct VdlcParmT {              /* Virtual DLC                    */
        hword ringNum;                  /* VDLC ring number               */
        MacAddrT virtualMac;            /* Virtual MAC address            */
        AdaptorId adaptorId;            /* VDLC adaptor definition        */
        boolean snaOrDspu;              /* Configured via "sna" or "dspu" */
        } VdlcParmT;

/*---------------------------------------------------------------------------*/
/* NCIA Parameter Structure. */

typedef struct NciaParmT {              /* NCIA Server                    */
        hword serverNum;                /* NCIA Server number             */
        AdaptorId adaptorId;            /* NCIA adaptor definition        */
        boolean snaOrDspu;              /* Configured via "sna" or "dspu" */
                                        /* Only dspu is supported now     */
        } NciaParmT;

/*----------------------------------------------------------------------------*/
/* TOKEN_RING Parameter Structure. */

typedef struct TokenRingParmT {
        MacAddrT localMac;              /* Local MAC address. */
        hword ringSpeed;                /* Token Ring speed. */
        } TokenRingParmT;


#endif  /* __DLCPARM_H. */
/*---- End include file: dlcparm.h. */
