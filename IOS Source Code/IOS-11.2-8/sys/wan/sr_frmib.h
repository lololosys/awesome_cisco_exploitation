/* $Id: sr_frmib.h,v 3.3.62.1 1996/07/10 07:38:04 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/sr_frmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Chifei W. Cheng (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_frmib.h,v $
 * Revision 3.3.62.1  1996/07/10  07:38:04  chifei
 * CSCdi60658:  Clear counters clears frame relay MIB objects
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/18  00:22:56  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:05:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:33:05  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:20:16  hampton
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

#ifndef _frmib_h_
#define _frmib_h_

#include "../snmp/snmp_api.h"
#include "sr_frmibdefs.h"
#include "sr_frmibsupp.h"

/* the frErrTable is unsupported */
#undef I_frErrIfIndex
#undef I_frErrType
#undef I_frErrData
#undef I_frErrTime

/* force "frame-relay" OID to be generated for use by trap generator */
#define N_frame_relay

/* extra stuff we support -in draft state though */
#define D_frDlcmiState_ccitt_933A 5

#define D_frDlcmiState_unknownLmi 0	/* internal error */

#include "sr_frmibpart.h"
#include "sr_frmibtype.h"

extern const ObjectInfo frmib_OidList[];
extern const uint       frmib_OidListNum;

extern void init_frmib(subsystype *);

typedef struct frmib_pvc_stats_ {
    ulong in_pkts_sav;
    ulong out_pkts_sav;
    ulong in_bytes_sav;
    ulong out_bytes_sav;
    ulong in_fecn_sav;
    ulong in_becn_sav;
} frmib_pvc_stats_type;

#endif  /* _frmib_h_ */
      
