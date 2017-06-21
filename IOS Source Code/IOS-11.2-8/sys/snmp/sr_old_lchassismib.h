/* $Id: sr_old_lchassismib.h,v 3.6.44.1 1996/05/21 06:38:48 mordock Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_old_lchassismib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_lchassismib.h,v $
 * Revision 3.6.44.1  1996/05/21  06:38:48  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6  1995/12/26  23:10:59  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add rpratt's code for MIB/queue manipulation.
 *
 * Revision 3.5  1995/12/15  03:14:27  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/12/10  23:28:05  rpratt
 * CSCdi45343:  Chassis MIB enhancements for VIP & port adapters
 *
 * Revision 3.3  1995/11/24  16:41:13  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  19:00:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:49:48  hampton
 * Bump version numbers from 1.x to 2.x.
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

#ifndef _old_lchassismib_h_
#define _old_lchassismib_h_

#include "snmp_api.h"
#include "sr_old_lchassismibdefs.h"
#include "sr_old_lchassismibsupp.h"
#include "sr_old_lchassismibpart.h"
#include "sr_old_lchassismibtype.h"


extern const ObjectInfo old_lchassismib_OidList[];
extern const uint       old_lchassismib_OidListNum;

extern char	       *snmp_chassis_string;
extern char	       *snmp_chassis_string_default;

extern void init_old_lchassismib(void);

extern OID *make_card_indexOID (int);

#endif  /* _old_lchassismib_h_ */
      
