/* $Id: sr_rttmib.h,v 1.1.4.2 1996/06/07 22:26:36 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/sr_rttmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * December 1995, Larry Metzger (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rttmib.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:36  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:42:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/03/22  18:41:59  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:42  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*

 * Copyright (C) 1994 by SNMP Research, Incorporated.
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

#ifndef _rttmib_h_
#define _rttmib_h_

#define N_rttMonConnectionChangeNotification
#define N_rttMonTimeoutNotification
#define N_rttMonThresholdNotification

#include "../snmp/snmp_api.h"
#include "sr_rttmibdefs.h"
#include "sr_rttmibsupp.h"
#include "sr_rttmibpart.h"
#include "sr_rttmibtype.h"
#include "rtt_lib.h"

extern const ObjectInfo rttmib_OidList[];
extern const uint rttmib_OidListNum;

#endif /* _rttmib_h_ */
      
