/* $Id: sr_ds1c3000mib.h,v 3.2 1996/03/29 03:50:13 sagarwal Exp $
 * $Source: /release/111/cvs/Xsys/les/sr_ds1c3000mib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, David Williams (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_ds1c3000mib.h,v $
 * Revision 3.2  1996/03/29  03:50:13  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:38  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
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

#ifndef _ds1c3000mib_h_
#define _ds1c3000mib_h_

#include "../snmp/snmp_api.h"
#include "sr_ds1c3000mibdefs.h"
#include "sr_ds1c3000mibsupp.h"
#include "sr_ds1c3000mibpart.h"
#include "sr_ds1c3000mibtype.h"

extern const ObjectInfo ds1c3000mib_OidList[];
extern const uint       ds1c3000mib_OidListNum;

#endif  /* _ds1c3000mib_h_ */
      
