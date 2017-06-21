/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_prse_sid.c,v 3.2.62.1 1996/03/18 21:48:53 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_prse_sid.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_prse_sid.c,v $
 * Revision 3.2.62.1  1996/03/18  21:48:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:33:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:14:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:00:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:19  hampton
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

#if (! ( defined(lint) ) && defined(SR_RCSID))
static char     rcsid[] = "prse_sid.c,v 1.1 1995/02/03 19:12:58 levi Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * prse_sid.c,v
 * Revision 1.1  1995/02/03  19:12:58  levi
 * Reorganization of seclib.
 *   Stuff for command line utilities -> mgrutils.c
 *   OID structure manipulation -> oid_lib.c
 *   OID <--> English translation -> oidtran.c
 *   Type and access constant <--> string conversion -> convrsn.c
 *   Finding nodes in the MIB tree -> mib_lib.c
 *   Reading MIB files and building MIB structures -> mibread[m].c
 *   Parsing SIDs of an OID stored in a char* -> prse_sid.c
 *
 */

#define NO_SSCANF
#include "master.h"
#include <ctype.h>


#include "sr_diag.h"


#include "sr_snmp.h"

#include "sr_snmpuser.h"

/*
 *   The following routines provide replacements for the sscanf
 *   function in the packaged agent system WARNING: they are
 *   non-portable to the extent that the target may not be using the
 *   ASCII collating sequence and must be re-written in that
 *   (unlikely) case.
 */

/*
 * DecChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was decchar2l().
 */
int
DecChar2L(str, value)
    char           *str;
    unsigned long  *value;
{
    if ((str[0] >= '0') && (str[0] <= '9')) {
	*value = str[0] - '0';
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = 0L;
    return (0);
}

/*
 * HexChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was hexchar2l().
 */
int
HexChar2L(str, value)
    char           *str;
    unsigned long  *value;
{
    if (((str[0] >= '0') && (str[0] <= '9')) ||
	((str[0] >= 'a') && (str[0] <= 'f')) ||
	((str[0] >= 'A') && (str[0] <= 'F'))) {

	*value = (str[0] <= '9') ? (str[0] - '0') :
	    ((str[0] <= 'F') ? (str[0] - 'A' + 10) : (str[0] - 'a' + 10));
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = 0L;
    return (0);
}

/*
 * OctChar2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was octchar2l().
 */
int
OctChar2L(str, value)
    char           *str;
    unsigned long  *value;
{
    if ((str[0] >= '0') && (str[0] <= '7')) {
	*value = str[0] - '0';
	return (1);
    }
    /* was not a digit ... signal a problem */
    *value = 0L;
    return (0);
}

/*
 * Str2L:
 *
 * The former name (pre-snmp12.1.0.0 release) was str2l().
 */
#ifdef NOBODY_CALLS_ME
int
Str2L(str, base, value)
    char           *str;
    int             base;
    unsigned long  *value;
{
    int             i, j;

    unsigned long   result;

    result = 0L;

    if (base == 8)
	for (i = 0; str[i] >= '0' && str[i] <= '7'; i++) {
	    result = result * base + (str[i] - '0');
	}
    else if (base == 10)
	for (i = 0; str[i] >= '0' && str[i] <= '9'; i++) {
	    result = result * base + (str[i] - '0');
	}
    else if (base == 16)
	for (i = 0; (((str[i] >= '0') && (str[i] <= '9')) ||
		     ((str[i] >= 'a') && (str[i] <= 'f')) ||
		     ((str[i] >= 'A') && (str[i] <= 'F'))); i++) {

	    j = (str[i] <= '9') ? (str[i] - '0') :
		((str[i] <= 'F') ? (str[i] - 'A' + 10) : (str[i] - 'a' + 10));
	    result = result * base + j;
	}
    *value = result;
    return (1);
}
#endif

/*
 * ParseSubIdHex:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_hex().
 */
int
ParseSubIdHex(_temp_ptr, value)
    unsigned char **_temp_ptr;
    unsigned long  *value;
{
    int             i;
    unsigned long   temp_value;
    unsigned char  *temp_ptr = *_temp_ptr;

    temp_ptr++;			/* skip the '0' */
    temp_ptr++;			/* skip the 'x' or 'X' */

    *value = 0L;

    for (i = 0; ((i < 8) && (*temp_ptr != '.') && (*temp_ptr != '\0')); i++) {
	if (!isxdigit((int) *temp_ptr)) {
	    DPRINTF((APWARN, "ParseSubIdHex, bad digit: %s\n", temp_ptr));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}

#ifdef NO_SSCANF
        if (HexChar2L(temp_ptr, &temp_value) != 1) {
#else
	if (sscanf((char *) temp_ptr, "%1x", &temp_value) != 1) {
#endif

	    DPRINTF((APERROR, "ParseSubIdHex, serious error.\n"));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}
	/* Have to mask because MSC does not handle longs well occasionally */
	*value = (*value << 4) + (0x0f & (unsigned long) temp_value);
	temp_ptr++;
    }				/* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}

/*
 * ParseSubIdOctal:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_octal().
 */
int
ParseSubIdOctal(_temp_ptr, value)
    unsigned char **_temp_ptr;
    unsigned long  *value;
{
    short           i;
    unsigned long   temp_value;
    unsigned char  *temp_ptr = *_temp_ptr;

    *value = 0L;

    for (i = 0; ((i < 11) && (*temp_ptr != '.') && (*temp_ptr != '\0')); i++) {
	if (*temp_ptr < '0' || *temp_ptr > '7') {
	    DPRINTF((APWARN, "ParseSubIdOctal, bad digit: %s\n", temp_ptr));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}

#ifdef NO_SSCANF
        if (OctChar2L(temp_ptr, &temp_value) != 1) {
#else
	if (sscanf((char *) temp_ptr, "%1o", &temp_value) != 1) {
#endif

	    DPRINTF((APERROR, "ParseSubIdOctal, serious error.\n"));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}
	/* Have to mask because MSC does not handle longs well occasionally */
	*value = (*value << 3) + (0x07 & (unsigned long) temp_value);
	temp_ptr++;
    }				/* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}

/*
 * ParseSubIdDecimal:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sub_id_decimal().
 */
int
ParseSubIdDecimal(_temp_ptr, value)
    unsigned char **_temp_ptr;
    unsigned long  *value;
{
    short           i;
    unsigned long   temp_value;
    unsigned char  *temp_ptr = *_temp_ptr;

    *value = 0L;
    for (i = 0; (i < 11) && (*temp_ptr != '.') && (*temp_ptr != '\0'); i++) {
	if (*temp_ptr < '0' || *temp_ptr > '9') {
	    DPRINTF((APWARN, "ParseSubIdDecimal, bad digit: %s\n", temp_ptr));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}

#ifdef NO_SSCANF
        if (DecChar2L(temp_ptr, &temp_value) != 1) {
#else
	if (sscanf((char *) temp_ptr, "%1d", &temp_value) != 1) {
#endif

	    DPRINTF((APERROR, "ParseSubIdDecimal, serious error.\n"));
	    *_temp_ptr = temp_ptr;
	    return (-1);
	}
	/* Have to mask because MSC does not handle longs well occasionally */
	*value = (*value * (unsigned long) 10) + (0x0f & (unsigned long) temp_value);
	temp_ptr++;
    }				/* end of for loop */

    *_temp_ptr = temp_ptr;
    return (0);
}
