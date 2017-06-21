/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_nvt_chk.c,v 3.2 1995/11/17 18:59:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/sr_nvt_chk.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_nvt_chk.c,v $
 * Revision 3.2  1995/11/17  18:59:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:36  hampton
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
static char     rcsid[] = "nvt_chk.c,v 1.14 1995/02/06 18:11:17 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * nvt_chk.c,v
 * Revision 1.14  1995/02/06  18:11:17  battle
 * Changes for vrtx.
 *
 * Revision 1.13  1994/10/28  16:37:51  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.12  1994/10/26  21:51:53  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.11  1994/09/30  16:10:51  battle
 * hprt changes
 *
 * Revision 1.10  1994/09/20  21:38:55  levi
 * Fix to ConvertToDisplayString to copy non-special characters into the
 *   new DisplayString.
 *
 * Revision 1.9  1994/08/04  15:37:28  battle
 * pSOS 2.0 changes
 *
 * Revision 1.8  1994/04/18  18:28:35  battle
 * pSOS changes
 *
 * Revision 1.7  1994/02/18  19:38:09  levi
 * Added (char *) cast in calls to free()
 *
 * Revision 1.6  1994/02/14  16:39:16  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.5  1994/02/04  16:01:03  levi
 * Changed == to =
 *
 * Revision 1.4  1993/12/23  16:54:03  battle
 * Added cvs headers to all .c files.
 *
 * Revision 1.3  1993/12/21  16:16:03  levi
 * Changed NVTString functions in nvt_chk.c to DisplayString.
 *
 * Revision 1.2  1993/12/20  19:22:01  levi
 * Fixed a few typos in initial version.
 *
 * Revision 1.1  1993/12/20  19:17:29  levi
 * Initial version of nvt_chk.c.  Performs NVT checking and conversion
 * on OctetString's.
 *
 */

#include "master.h"


#include "sr_snmp.h"
#include "sr_diag.h"

/*
 * IsDisplayString
 *
 * Checks if the contents of an OctetString is a valid NVT string.  If it
 * is, returns 1, otherwise returns 0.
 */
int
IsDisplayString(os)
    OctetString    *os;
{
    int             i;

    if (os == NULL) {
	return (0);
    }
    if (os->octet_ptr == NULL) {
	return (0);
    }

    for (i = 0; i < os->length; i++) {
	if (os->octet_ptr[i] == '\n') {
	    return (0);
	}
	if (os->octet_ptr[i] == '\r') {
	    i++;
	    if (i >= os->length) {
		return (0);
	    }
	    if ((os->octet_ptr[i] != '\n') && (os->octet_ptr[i] != '\0')) {
		return (0);
	    }
	}
	else if ((os->octet_ptr[i] < ' ') || (os->octet_ptr[i] > '~')) {
	    return (0);
	}
    }

    return (1);
}

/*
 * ConvertToDisplayString
 *
 * Converts an OctetString to an NVT string.  This consists of converting
 * all newline characters (\n) to a carriage-return/linefeed pair (\r\n),
 * and converting any single carriage-return characters which are not
 * followed by a NULL character into a carriage-return/NULL pair (\r\0).
 * If there are any non-printable characters, the function returns 0,
 * otherwise it returns 1.
 */
int
ConvertToDisplayString(os)
    OctetString    *os;
{
    int             i, j;
    unsigned char  *octet_ptr;

    octet_ptr = (unsigned char *) malloc(sizeof(unsigned char) * os->length * 2);
    if (octet_ptr == NULL) {
	DPRINTF((APTRACE, "ConvertToDisplayString: octet_ptr malloc\n"));
	return (0);
    }

    for (i = 0, j = 0; i < os->length; i++, j++) {
	if (os->octet_ptr[i] == '\n') {
	    octet_ptr[j] = '\r';
	    octet_ptr[++j] = '\n';
	}
	else if (os->octet_ptr[i] == '\r') {
	    octet_ptr[j] = '\r';
	    if ((i + 1) >= os->length) {
		octet_ptr[++j] = '\0';
		free((char *) os->octet_ptr);
		os->octet_ptr = octet_ptr;
		os->length = j;
		return (1);
	    }
	    if ((os->octet_ptr[i + 1] == '\n') ||
		(os->octet_ptr[i + 1] == '\0')) {
		octet_ptr[++j] = os->octet_ptr[++i];
	    }
	    else {
		octet_ptr[++j] = '\0';
	    }
	}
	else if ((os->octet_ptr[i] < ' ') ||
		 (os->octet_ptr[i] > '~')) {
	    free((char *) octet_ptr);
	    return (0);
	}
	else {
	    octet_ptr[j] = os->octet_ptr[i];
	}
    }
    free((char *) os->octet_ptr);
    os->octet_ptr = octet_ptr;
    os->length = j;
    return (1);
}

/*
 * ConvertFromDisplayString
 *
 * Converts an OctetString from an NVT string to a normal string.  This
 * consists of converting carriage-return/linefeed pairs (\r\n) into a
 * single newline character (\n), and converting carriage-return/NULL
 * pairs (\r\0) into a single carriage return (\r).  If there are any
 * non-printable characters, the function returns 0, otherwise it returns 1.
 */
#ifdef NOBODY_CALLS_ME
int
ConvertFromDisplayString(os)
    OctetString    *os;
{
    int             i, j;

    if (!IsDisplayString(os)) {
	return (0);
    }

    for (i = 0, j = 0; i < os->length; i++, j++) {
	if (os->octet_ptr[i] == '\r') {
	    i++;
	    if (os->octet_ptr[i] == '\n') {
		os->octet_ptr[j] = '\n';
	    }
	    else {
		os->octet_ptr[j] = '\r';
	    }
	}
	else {
	    os->octet_ptr[j] = os->octet_ptr[i];
	}
    }
    os->length = j;
    return (1);
}
#endif
