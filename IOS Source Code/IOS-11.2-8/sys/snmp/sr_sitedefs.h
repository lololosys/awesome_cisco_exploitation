/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_sitedefs.h,v 3.2 1995/11/17 19:01:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_sitedefs.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_sitedefs.h,v $
 * Revision 3.2  1995/11/17  19:01:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:33  hampton
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

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 * Revision History:
 *
 * 9-Feb-1993 DLB
 *   Removed #ifdef TRIV around define of MAXCOMMLEN.
 *
 * 2-Jul-1993 DLB
 *   Added SYS_OBJECT_ID #define.
 *
 * 8-Oct-1993 DLB
 *   Updated STARTUP banners.
 */

/*
 * Define site-specific definitions in this file.
 */

/*
 * Define 2 text strings to be displayed with the Agent starts.
 */
#define STARTUP_BANNER_1 \
    "SNMP Research SNMP Agent Resident Module Version 12.3.0.9 \n"

#define STARTUP_BANNER_2 \
    "Copyright 1989, 1990, 1991, 1992, 1993, 1994 SNMP Research, Inc.\n"

/*
 * Interface layer definitions.
 */
#define PHYS_ADDR_SIZE 6
/* #define IF_TESTING_SUPPORTED */


/*
 * IP configuration.
 */

/* #define SUPPORT_IP_FORWARDING */
#define MIN_IP_TTL	2

/* #define SUPPORT_ROUTE_METRIC_1 */
/* #define SUPPORT_ROUTE_METRIC_2 */
/* #define SUPPORT_ROUTE_METRIC_3 */
/* #define SUPPORT_ROUTE_METRIC_4 */
/* #define SUPPORT_ROUTE_METRIC_5 */



#define BUFLEN 512

#define PHYS_ADDR_SIZE 6

#define PKTBUFSIZ 2048

#define MAXCOMMLEN      64

/* The agent's sysObjectID is no longer defined here.  It is defined instead
   in snmpd.cnf.  If a textual name is used it must be in snmp-mib.h.
   Dotted decimal oid's do not need to be in snmp-mib.h.
*/

/* for read-write tables in v1 MIBs in which new rows can be created */
#define ipRouteEntry_READ_CREATE
#define ipNetToMediaEntry_READ_CREATE

