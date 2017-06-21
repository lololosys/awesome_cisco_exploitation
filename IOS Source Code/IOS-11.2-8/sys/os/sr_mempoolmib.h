/* $Id: sr_mempoolmib.h,v 3.2 1996/02/20 06:15:32 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/os/sr_mempoolmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1996, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_mempoolmib.h,v $
 * Revision 3.2  1996/02/20  06:15:32  jjohnson
 * CSCdi49356:  Need MIB variable for total DRAM
 * Fully instrument the memory pools
 *
 * Revision 3.1  1996/02/13  18:27:22  jjohnson
 * Initial placeholder
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

#ifndef _mempoolmib_h_
#define _mempoolmib_h_

#include "../snmp/snmp_api.h"
#include "sr_mempoolmibdefs.h"
#include "sr_mempoolmibsupp.h"
#include "sr_mempoolmibpart.h"
#include "sr_mempoolmibtype.h"

extern const ObjectInfo mempoolmib_OidList[];
extern const uint       mempoolmib_OidListNum;

#endif  /* _mempoolmib_h_ */
      
