/* $Id: snmp_util.c,v 3.2.60.1 1996/07/03 20:43:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_util.c,v $
 *------------------------------------------------------------------
 * snmp_util.c:  SNMP utilities that probably should be part of the
 * SNMP Research stack, but aren't
 *
 * April 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_util.c,v $
 * Revision 3.2.60.1  1996/07/03  20:43:25  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.2  1995/11/17  18:58:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sr_snmpd.h"

/*
 * Convert an OctetString to an unsigned long.
 */
unsigned long
OctetStringToUlong (OctetString *os)
{
   int i, len;
   unsigned long ul = 0L;

   /*
    * limit the conversion to 4 octets [array elements 0-3]
    */
   len = os->length;
   if (len > 4)
       len = 4;

   for (i = 0; i < len; i++) {
       ul = (ul << 8) | os->octet_ptr[i];
   }
   return ul;
}

int
snmp_scalar_instance (OID	       *incoming,
		      ObjectInfo       *object,
		      int		searchType)
{
    int             instLength = incoming->length - object->oid.length;

    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and the single
     * instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    if (((searchType == NEXT) &&
	 (instLength <= 0))
	||
        ((searchType == EXACT) &&
	 (instLength == 1) &&
	 (incoming->oid_ptr[incoming->length - 1] == 0)))
	return (object->nominator);
    else
	return (-1);
}
