/* $Id: dtypes.h,v 3.3.26.2 1996/05/17 10:48:26 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dtypes.h,v $
 *------------------------------------------------------------------
 * DSPU Types.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Note well: There are many "foward declared" typedefs included here
 * simply to satisfy the compiler's need for typing of variables. You
 * might say these are dummy typedefs...and you'd be right. The actual
 * structure declarations (associated with these typedefs) are left until
 * the structure is needed by a source module. That way it is possible to
 * isolate structure definitions to particular source modules. ...A very
 * nice method in "C" for encapsulating code and structures.
 *------------------------------------------------------------------
 * $Log: dtypes.h,v $
 * Revision 3.3.26.2  1996/05/17  10:48:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.4  1996/05/03  17:18:37  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.3  1996/04/26  07:35:15  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.14.2  1996/04/03  14:03:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.1  1996/03/28  08:20:43  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.26.1  1996/03/18  19:33:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  08:45:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  09:06:27  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2.4.1  1996/01/20  06:08:58  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:05:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/19  21:13:05  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.3  1995/09/14  06:15:27  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.2  1995/07/06  23:15:55  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/06/07  20:29:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DTYPES_H                         
#define __DTYPES_H

#include "master.h"
#include <ciscolib.h>
#include "address.h"
#include "interface.h"
#include "../dspu/dspu_const.h"
#include "../cls/clsi_msg.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef TRUE
typedef enum{
             FALSE, TRUE
            }boolean;
#endif

/*---- Foward declared "null/dummy" typedefs to satisfy compiler. */

typedef struct Dspu Dspu;               /* DSPU core, also super parent. */
typedef struct Cps Cps;                 /* Child-Parent linkage. */
typedef struct Sap Sap;                 /* Service Access Point. */
typedef struct Adaptor Adaptor;         /* Adaptor card. (TR, E, etc). */
typedef struct LinkStn LinkStn;         /* Link Station. */
typedef struct Pu Pu;                   /* Physical Unit. */
typedef struct Lu Lu;                   /* Logical Unit. */
typedef struct Itimer Itimer;
typedef struct WrkBBlk WrkBBlk;
typedef struct PoolCB  PoolCB;          /* pool control block */
typedef struct AdaptorId AdaptorId;
typedef struct PoolRange PoolRange;
typedef struct ConnParmsT  ConnParmsT;        /* Connection parameters. */
typedef struct RetryInfoT  RetryInfoT;      /* Connection retry information. */

typedef enum {
          Lu_Dedicated, Lu_Pooled
        } LuPooledE;

#ifndef offsetof
#define offsetof(t,i)	((size_t)((char *)&((t *)0)->i - (char *)0))
#endif

typedef union Xid {
        byte blk_num[4];    /* id block and id num */
        word xid;           /* pass as word        */
        } Xid;
typedef union Xid XidU;                 /* This XID stuff should be fixed! */

typedef struct  MacAddrT{                  /* For things like MAC-ADDRs. */
        byte mac[MAC_ADDR_LN];              /* Umm...change to IEEEBYTES soon. */
        } MacAddrT;


typedef struct  PuNameT {
        byte name[MAX_ENTRYNAME_LN+1]; 
        } PuNameT;

typedef enum XMS_SearchModeE {
        XMS_AddrSap,          /* match address/sap     */
        XMS_AddrXid           /* match address/sap/xid */
      } XMS_SearchModeE;

typedef struct XidAddrSapT {
        XMS_SearchModeE searchMode;
        XidU u_xid;
        hwaddrtype addr;
        byte rsap;
        Sap *lsapP;
        } XidAddrSapT;

typedef enum {
             FlwCtl_None, FlwCtl_Stop, FlwCtl_Resume} FlowControlE;
 
/* The Data Request and Data Indication in DSPU has the same structure. It
 * is used in DSPU as if there is only one data type. eg, when a request
 * is changed to a response or when a message from one pu is passed to the
 * other.
 */


/* Result of attempting to connect an LU to the downstream PU  
 */    
typedef enum ConnectStatus {
  NoMemory,      /* AddChild failed, not enough memory */
  NoLusInPool,   /* No LUs in pool, must wait          */
  Successful     /* Successfully connected LU to downstream PU */
}ConnectStatus;  

/*---- Link station activity modes. */

typedef enum LinkType {
        Upstream,                       /* upstream or ... */
        Downstream,                     /* downstream */
        } LinkType;

/*---- DLC types. Except for DLC_UNDEFINED this is a copy of CLSDLCType_e
       in CLSI_MSG.H. */

typedef enum {
        DSPU_DLC_UNDEFINED   = -1,
        DSPU_DLC_SDLC        =  1,
        DSPU_DLC_ETHERNET    =  5,
        DSPU_DLC_TOKEN_RING  =  6,
        DSPU_DLC_RSRB        =  8,
        DSPU_DLC_FRAME_RELAY =  9,
        DSPU_DLC_FDDI        = 10,
        DSPU_DLC_QLLC        = 11,
        DSPU_DLC_CHANNEL     = 12,
        DSPU_DLC_VDLC        = 13,
        DSPU_DLC_NCIA        = 14
        } DspuDLCType_e;

typedef enum WorkResult {
        WORK_NOT_DONE,
        WORK_DONE,
        NO_WORK_BUFFERS_AVAILABLE
        } WorkResult;

typedef enum CallDirection {
             CallDir_In, CallDir_Out} CallDirection;

typedef enum{
             NON_BLOCKING, 
             BLOCKING
            }BlockingType;


#define UnitIndex   0
#define PortIndex   1

struct AdaptorId {
        DspuDLCType_e dlcType;         /* Adaptor is this DLC type. */
        union {
           byte     adaptorPort[2];     /* unit + port     */
           hword    adaptornumber;      /* Adaptor number used by CLSI */
        } u;
        hword adaptorSubIfNumber;       /* Adaptor subinterface number */
        idbtype *dlcIdb;
   } ;


#endif    /* __DTYPES_H */
/*---- End include file: dtypes.h. */
