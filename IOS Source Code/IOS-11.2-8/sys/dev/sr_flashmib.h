/* $Id: sr_flashmib.h,v 3.2.60.1 1996/04/22 18:57:59 ssangiah Exp $
 * $Source: /release/112/cvs/Xsys/dev/sr_flashmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Shalabh Agarwal (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_flashmib.h,v $
 * Revision 3.2.60.1  1996/04/22  18:57:59  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:01:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:21:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
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

#ifndef _flashmib_h_
#define _flashmib_h_

#include "../snmp/snmp_api.h"
#include "sr_flashmibdefs.h"
#include "sr_flashmibsupp.h"
#include "sr_flashmibpart.h"
#include "sr_flashmibtype.h"

extern const ObjectInfo flashmib_OidList[];
extern const uint       flashmib_OidListNum;

#include "flash_defines.h"

extern boolean flash_file_delete(flash_copy_parms_t*),
	flash_file_undelete(flash_copy_parms_t*),
	flash_device_squeeze(flash_copy_parms_t*),
	flash_device_format(flash_copy_parms_t*);

#endif  /* _flashmib_h_ */
      
