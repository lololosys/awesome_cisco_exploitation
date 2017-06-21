/* $Id: sr_encryptmib.h,v 3.1.4.1 1996/04/19 14:56:16 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sr_encryptmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, Subodh Nijsure (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_encryptmib.h,v $
 * Revision 3.1.4.1  1996/04/19  14:56:16  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1996/04/10  17:40:37  snijsure
 * Branch: IosSec_branch
 * Checking in implementation of Cisco IP encryption MIB.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
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

#ifndef _encryptmib_h_
#define _encryptmib_h_

#define N_cieTestCompletion

#include "../snmp/snmp_api.h"
#include "sr_encryptmibdefs.h"
#include "sr_encryptmibsupp.h"
#include "sr_encryptmibpart.h"
#include "sr_encryptmibtype.h"

extern const ObjectInfo encryptmib_OidList[];
extern const uint       encryptmib_OidListNum;

#endif  /* _encryptmib_h_ */
      
