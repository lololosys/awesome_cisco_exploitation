/* $Id: sr_snanaumib.h,v 3.1.2.1 1996/05/24 02:06:45 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_snanaumib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * May 1996, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snanaumib.h,v $
 * Revision 3.1.2.1  1996/05/24  02:06:45  bchan
 * CSCdi58530:  sr_snanaumib.h missing from source code repository
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/24  01:17:53  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1992-1996 by SNMP Research, Incorporated.
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

#ifndef _snanaumib_h_
#define _snanaumib_h_

#include "../snmp/snmp_api.h"
#include "sr_snanaumibdefs.h"
#include "sr_snanaumibsupp.h"
#include "sr_snanaumibpart.h"
#include "sr_snanaumibtype.h"

extern const ObjectInfo snanaumib_OidList[];
extern const uint       snanaumib_OidListNum;

#endif  /* _snanaumib_h_ */
      
