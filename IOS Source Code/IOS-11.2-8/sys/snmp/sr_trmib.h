/* $Id: sr_trmib.h,v 3.2.62.1 1996/03/18 21:49:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_trmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_trmib.h,v $
 * Revision 3.2.62.1  1996/03/18  21:49:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:24:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  17:15:31  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:01:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  10:22:03  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.1  1995/06/07  22:51:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#ifndef _trmib_h_
#define _trmib_h_

#include "../snmp/snmp_api.h"
#include "sr_trmibdefs.h"
#include "sr_trmibsupp.h"

#undef I_dot5TimerIfIndex
#undef I_dot5TimerReturnRepeat
#undef I_dot5TimerHolding
#undef I_dot5TimerQueuePDU
#undef I_dot5TimerValidTransmit
#undef I_dot5TimerNoToken
#undef I_dot5TimerActiveMon
#undef I_dot5TimerStandbyMon
#undef I_dot5TimerErrorReport
#undef I_dot5TimerBeaconTransmit
#undef I_dot5TimerBeaconReceive

#include "sr_trmibpart.h"
#include "sr_trmibtype.h"

extern const ObjectInfo trmib_OidList[];
extern const uint       trmib_OidListNum;

extern void dot5Entry_free(dot5Entry_t *);
extern int dot5Entry_cleanup(doList_t *);
extern int dot5Entry_undo(doList_t *, doList_t *, ContextInfo *);

/*
 * Ring Status
 */
#define RSTS_NO_PROBLEMS              0
#define RSTS_RING_RECOVERY           32
#define RSTS_SINGLE_STATION          64
#define RSTS_REMOVE_RCVD            256
#define RSTS_RESERVED               512
#define RSTS_AUTO_REMOVAL_ERROR    1024
#define RSTS_LOBE_WIRE_FAULT       2048
#define RSTS_TRANSMIT_BEACON       4096
#define RSTS_SOFT_ERROR            8192 
#define RSTS_HARD_ERROR           16384
#define RSTS_SIGNAL_LOSS          32768
#define RSTS_NO_STATUS           131072 

#endif  /* _trmib_h_ */
      
