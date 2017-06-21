/* $Id: snmp_util.h,v 3.1.2.2 1996/07/12 23:29:06 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_util.h,v $
 *------------------------------------------------------------------
 * snmp_util.h:  SNMP utilities that probably should be part of the
 * SNMP Research stack, but aren't
 *
 * June 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_util.h,v $
 * Revision 3.1.2.2  1996/07/12  23:29:06  jjohnson
 * CSCdi62131:  Need a mechanism to support logical snmp entities
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/07/03  20:43:26  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.1  1996/06/30  00:31:00  jjohnson
 * Initial Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNMP_UTIL_H__
#define __SNMP_UTIL_H__

unsigned long OctetStringToUlong(OctetString *os);

int snmp_scalar_instance(OID	       *incoming,
		         ObjectInfo    *object,
		         int		searchType);

void add_snmp_entity(const char *entity);
void delete_snmp_entity(const char *entity);

#endif /* __SNMP_UTIL_H__ */
