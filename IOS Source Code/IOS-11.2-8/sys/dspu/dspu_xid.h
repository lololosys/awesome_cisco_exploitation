/* $Id: dspu_xid.h,v 3.2 1996/02/26 17:33:04 ppearce Exp $
 * $Source: /release/111/cvs/Xsys/dspu/dspu_xid.h,v $
 *------------------------------------------------------------------
 * DSPU XID Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains the prototype definitions for XID handling.
 *------------------------------------------------------------------
 * $Log: dspu_xid.h,v $
 * Revision 3.2  1996/02/26  17:33:04  ppearce
 * CSCdi49863:  DSPU reports info from CV 0x22 incorrectly
 *
 * Revision 3.1  1995/11/09  11:28:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:14  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __XID_H
#define __XID_H

#include "dtypes.h"
#include "dmalloc.h"

/*---- The following defines and structures are exported. */

/*---- XID Negotiation Error definitions. */

#define NEGOTIATION_ERROR 0x22          /* XID negotiation error, ctlvec 22. */
#define XID_MAXIFRAME_BYTE 21           /* Byte offset in XID fmt 3. */
#define XID_MAXIFRAME_BIT 0             /* Bit offset in above. */
#define XID_WINDOWSIZE_BYTE 27          /* Byte offset in XID fmt 3. */
#define XID_WINDOWSIZE_BIT 1            /* Bit offset in above. */
#define XID_ACTPU_BYTE 9                /* Byte offset in XID fmt 3. */
#define XID_ACTPU_BIT 0                 /* Bit offset in above. */

/*---- Enumerated negotiation error types. */

typedef enum NegErrE {
        Xid_NP_NoError,
        Xid_NP_BadWindowSize,
        Xid_NP_BadMaxIFrame,
        Xid_NP_NoActpuCap
        } NegErrE;

/*---- Physical XID negotiation error control vector. */

typedef struct XidCtlVec22T {
        byte byte[5];
        } XidCtlVec22T;

/*---- Logical XID negotiation error control vector. */

typedef struct XidNegErrT {
        hword byteOffset;
        hword bitOffset;
        byte  senseData[8];
        } XidNegErrT;

/*---- XID format types. */

typedef enum XidFmtE {
        xidFmt_Null,                    /* XID(null). */
        xidFmt_0,                       /* XID(0). */
        xidFmt_3,                       /* XID(3). */
        xidFmt_Bad                      /* Bad/unknown XID. */
        } XidFmtE;

/*---- The following interfaces are exported. */

/*======================================================================*/
/* XidGetFormat: Get xid format from ID & ID_STN signals. */
/*======================================================================*/

XidFmtE
XidGetFormat (TCLSIMsg *clsiMsgP);

/*======================================================================*/
/* XidGetXidData: Get xid data address and length from ID & ID_STN signals. */
/*======================================================================*/

void
XidGetXidData (TCLSIMsg *clsiMsgP,
               byte **xidDataP,
               hword *xidLength);

/*======================================================================*/
/* XidGetXid: Get xid (idblk.idnum) from ID.IND xid data. */
/*======================================================================*/

word
XidGetXid (TCLSIMsg *clsiMsgP);

/*======================================================================*/
/* XidGetMaxIFrame: Get maxIFrame from ID.IND xid data. */
/*======================================================================*/

hword
XidGetMaxIFrame (TCLSIMsg *clsiMsgP);

/*======================================================================*/
/* XidGetWindowSize: Get windowSize from ID.IND xid data. */
/*======================================================================*/

byte
XidGetWindowSize (TCLSIMsg *clsiMsgP);

/*======================================================================*/
/* XidGetACTPUCapability: Get ACTPU capability from ID.IND(3). */
/*======================================================================*/

boolean
XidGetACTPUCapability (TCLSIMsg *clsiMsgP);

/*============================================================================*/
/* XidCtlVec22_Updt. */
/*============================================================================*/

void
XidCtlVec22_Updt (XidCtlVec22T *cv22P,
                  byte errorByteOffset,
                  byte errorBitOffset);

/*============================================================================*/
/* XidQueryNegError. */
/*============================================================================*/

boolean
XidQueryNegError (TCLSIMsg *clsiMsgP,
                  XidNegErrT *xidNegErrP);

/* word GetXidBlockNum(Xid xid) */
#define GetXidBlockNum(xid)  (xid>>20)

/* word GetXidIdNum(Xid xid) */
#define GetXidIdNum(xid)  (((xid<<12)>>12))


#endif  /* __XID_H. */
/*---- End include file: xid.h. */
