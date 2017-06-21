/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_frmt_lib.c,v 3.3.28.1 1996/03/18 21:48:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_frmt_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_frmt_lib.c,v $
 * Revision 3.3.28.1  1996/03/18  21:48:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  12:33:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/20  02:00:21  jjohnson
 * CSCdi47218:  excessive stack usage by snmp research library routines
 *
 * Revision 3.2  1995/11/17  18:59:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:11  hampton
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
static char     rcsid[] = "frmt_lib.c,v 1.2 1995/02/06 18:11:11 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * frmt_lib.c,v
 * Revision 1.2  1995/02/06  18:11:11  battle
 * Changes for vrtx.
 *
 * Revision 1.1  1994/11/02  15:15:28  levi
 * Added
 *
 * Revision 1.15  1994/10/28  16:37:56  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.14  1994/10/26  21:51:57  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.13  1994/09/30  16:10:54  battle
 * hprt changes
 *
 * Revision 1.12  1994/08/04  15:37:31  battle
 * pSOS 2.0 changes
 *
 * Revision 1.11  1994/06/27  15:54:05  levi
 * Moved oid translation stuff into oidtran.c
 *
 * Revision 1.10  1994/04/18  18:28:40  battle
 * pSOS changes
 *
 * Revision 1.9  1994/03/28  16:14:42  levi
 * Updates for WINDOWS port of reqlib and scriptlib
 *
 * Revision 1.8  1994/03/15  21:35:57  levi
 * Fixed NULL_TYPE cases.
 *
 * Revision 1.7  1994/02/28  19:36:27  reid
 * removed buffer[0] = '\0'; (which causes a core dump) from PrintVarBindValue
 *
 * Revision 1.6  1994/02/14  16:39:18  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.5  1994/02/08  00:09:25  pearson
 * Here's a brief description of the changes in this commit of the source
 * tree:
 * 	The structure of the packaged tree has changed somewhat.
 * 	Assembly language files and system-specific startup code are in
 * 	the pkg/lib/sys directory.  There is a 'serial' directory which
 * 	contains rs232 code for all of the different platforms.
 *
 * 	Uart code has a more uniform initialization call now.  For
 * 	example, init_uart() is mapped to init code for different uarts
 * 	on different systems.
 *
 * 	The national semiconductor "rib" code is in this release, along
 * 	with support for the debug/rt debugger and paradigm locate.
 * 	That created some new directories in the pkg/cmd tree.  There is
 * 	support now for a sonic driver, the ethernet repeater mib, and
 * 	sonic/ric statistics.
 *
 * 	The dos agent and utilities now configure themselves from an
 * 	ascii text file, rather than a "device driver".
 *
 * 	Various parts of the standard agent have had large stack
 * 	variables moved to malloc-ed structures to save room.
 *
 * 	Some bugs which were exposed by the marshall/levi test tool have
 * 	been fixed.
 *
 * 	Two new hello programs have been created.  Hello3 is essentially
 * 	a ping server, which allows embedded developers to test their
 * 	network stack, and hello4 allows tests of bootp and tftp.
 *
 * 	Some new routines have been added to the pkg/lib/c directory.
 * 	Dmemdump dumps memory in hex and in ascii.  32-bit versions of
 * 	memcpy, memset, etc have been created.
 *
 * Revision 1.4  1993/12/23  16:44:05  battle
 * Added cvs header.
 *
 *
 *  2/5/89 JDC
 *  Amended copyright notice
 *  Altered print_packet out to print 16 bytes per line (from 19).
 *  Expanded comments and documentation
 *
 *  8/3/89 JDC
 *  Added conditional compilation to handle CMU PC/IP printf wierdness
 *  on formatting of output
 *
 *  Shortened name of file to 8 characters so will have same name on DOS as
 *  UNIX
 *
 *  8/6/89 JDC
 *  Fixed code around isprint call for MSC
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  04-Oct-90 KWK
 *  Fix so that clip strings containing CR, LF, etc.
 *
 *  01-Jan-91 JDC
 *  Added use of LIB_ERROR family of macros
 *
 *  25-Apr-92 MRD
 *  Merged in dprintf() for Version 11
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  9-July-92 MRD
 *  Made it Packaged Agent Friendly.
 *
 * 14-Aug-1992 DLB
 * Changed to use & instead of < in dprintf, the comment there is no longer
 * accurate.
 *
 * 18-Aug-1992 DLB
 * Added dlfprint to print __LINE__ and __FILE__.
 *
 * 17-Sep-1992 DLB
 * Moved declaration of LogLevel from snmpd/unix/main.c to seclib/prnt_lib.c
 * so that it will be in a (eventually shared) library and therefore accessible
 * to static subagents on dgux.
 *
 * 13-Nov-1992 KWK
 * removed the #ifdef DEBUG around the whole file's contents to allow
 * UTIL's to be compiled without DEBUG.
 *
 * 6-Jan-1993 MGH
 * Added U_INTEGER_32_TYPE to the switch statement in PrintVarBindList.
 *
 * 7-Jan-1993 DAR & DLB
 * added PrintOID()
 *
 * 25-Jan-1993 MGH
 * Added INCONSISTENT_NAME_ERROR to the switch statement in
 *
 * 03-Mar-1993 JTT
 * Added a file name initialization rountine, InitUtilFileNames.
 *
 * 5-May-1993 MGH
 * Changed all LIB_ERROR* messages to DPRINTF messages.
 *
 * 11-May-1993 MGH
 * Changed PrintOctetString() to print octet strings out a little
 * differently.  If the octet string is less than 9 characters long, the
 * string is printed on the same line as the variable name.  There are also
 * extra blanks printed at certain positions in the string.
 *
 * 7-Jun-1993 DLB
 *   Added code to include program name in error messages.
 *
 * 10-Jun-1993 DLB
 *   Changed all short parameters to int.  Short parameters are a bad idea
 *   for numerous reasons I won't go into here.
 *
 * 19-July-1993 MGH
 * Changed the #ifndef SNMPD blocks to #ifndef SR_AGENT blocks.
 *
 * 21-Jul-1993 DLB
 *   Removed main() function (it was already ifdef'ed out).
 */

#include "master.h"
#include <ctype.h>

#define WANT_ALL_ERRORS

#include "sr_snmp.h"
#include "sr_snmpuser.h"
#include "sr_diag.h"

/*
 * SPrintOctetString:
 *
 *   This routine sprint's the contents  of  a  Octet  String's
 *   value  in  hex.   It  is called with a pointer to the Octet-
 *   String construct and the number of bytes to display  on  one
 *   line  (the  variable  'wrap').
 *
 * 11/2/93 DBL
 * Created this function from PrintOctetString
 *
 * This function can be used to sprintf a packet (instead of providing
 * an SPrintPacket function).
 */

void
SPrintOctetString(oct_ptr, wrap, buf)
    OctetString    *oct_ptr;
    int             wrap;
    char           *buf;
{
    long            i;
    long            pos;
    char            buffer[8];

    strcpy(buf, "");
    for (i = 0, pos = 1; i < oct_ptr->length; i++) {
	if (((i % wrap) == 0) && (oct_ptr->length > 8)) {
	    strcat(buf, "\n");
	}

	sprintf(buffer, "%02x ", oct_ptr->octet_ptr[i]);
	strcat(buf, buffer);

	if ((oct_ptr->length >= 8) || ((oct_ptr->length % 4) == 0)) {
	    if ((pos % 2) == 0) {
		strcat(buf, " ");
	    }
	    if ((pos % 4) == 0) {
		strcat(buf, " ");
	    }
	    if ((pos % 8) == 0) {
		strcat(buf, " ");
	    }
	}

	if ((pos % wrap) == 0) {
	    pos = 1;
	}
	else {
	    pos++;
	}
    }
}				/* PrintOctetString() */

/*
 * SPrintVarBind:
 *
 *   This routine sprint's out the contents of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *   The output is in the form "name = value"
 *
 * 11/1/93 DBL
 * Pulled printing code out of PrintVarBindList into this function.
 */

int
SPrintVarBind(vb_ptr, buf)
    VarBind        *vb_ptr;
    char           *buf;
{
    int retval;
    char *buffer1 = malloc(MAX_OCTET_STRING_SIZE + 256);
    if (buffer1 == NULL)
	return (-1);

    if (MakeDotFromOID(vb_ptr->name, buf) == -1) {
	DPRINTF((APTRACE, "SPrintVarBindList, vb_ptr->name:\n"));
	free(buffer1);
	return (-1);
    }
    strcpy(buffer1, buf);
    strcat(buf, " = ");
    retval = SPrintVarBindValue(vb_ptr, &(buf[strlen(buf)]), buffer1);
    free(buffer1);
    return (retval);
}

/*
 * SPrintVarBindValue:
 *
 *   This routine sprint's out the value of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *
 * 11/3/93 DBL
 * Pulled printing code out of PrintVarBind into this function.
 */

int
SPrintVarBindValue(vb_ptr, buf, name)
    VarBind        *vb_ptr;
    char           *buf, *name;
{
    char *buffer = malloc(MAX_OCTET_STRING_SIZE + 256);
    if (buffer == NULL)
	return -1;

    switch (vb_ptr->value.type) {
      case COUNTER_TYPE:	/* handle unsigned integers includes
				 * COUNTER_32 */
      case GAUGE_TYPE:		/* includes GAUGE_32 */
      case TIME_TICKS_TYPE:

#ifdef SR_SNMPv2
      case U_INTEGER_32_TYPE:
#endif				/* SR_SNMPv2 */

	sprintf(buf, "%lu", vb_ptr->value.ul_value);
	break;
      case INTEGER_TYPE:	/* handle signed integers includes INTEGER_32 */

	sprintf(buf, "%ld", vb_ptr->value.sl_value);

	break;
      case IP_ADDR_PRIM_TYPE:
	sprintf(buf, "%d.%d.%d.%d",
		vb_ptr->value.os_value->octet_ptr[0],
		vb_ptr->value.os_value->octet_ptr[1],
		vb_ptr->value.os_value->octet_ptr[2],
		vb_ptr->value.os_value->octet_ptr[3]);
	break;
      case OBJECT_ID_TYPE:
	buffer[0] = '\0';
	if (MakeDotFromOID(vb_ptr->value.oid_value, buffer) == -1) {
	    DPRINTF((APTRACE, "SPrintVarBindValue, vb_ptr->value.oid_value:\n"));
	    free(buffer);
	    return (-1);
	}
	sprintf(buf, "%s", buffer);
	break;
      case OCTET_PRIM_TYPE:	/* handle quasi-octet strings */
      case OPAQUE_PRIM_TYPE:
	if (SPrintAscii(vb_ptr->value.os_value, buf) < 0) {
	    /* cannot print ascii */
	    SPrintOctetString(vb_ptr->value.os_value, 16, buf);
	}
	break;
      case NULL_TYPE:
	strcpy(buf, "NULL");
	break;

#ifdef	SR_SNMPv2
      case NSAP_ADDR_TYPE:
	if (SPrintAscii(vb_ptr->value.os_value, buf) < 0) {
	    /* cannot print ascii */
	    SPrintOctetString(vb_ptr->value.os_value, 16, buf);
	}
	break;
      case COUNTER_64_TYPE:
	sprintf(buf, "0x%x%x",
		vb_ptr->value.uint64_value->big_end,
		vb_ptr->value.uint64_value->little_end);
	break;
      case BIT_STRING_TYPE:
	sprintf(buf, "(%d)", vb_ptr->value.bitstring_value->unused);
	SPrintOctetString((OctetString *) vb_ptr->value.bitstring_value, 16, buf);
	break;
      case NO_SUCH_OBJECT_EXCEPTION:
	sprintf(buf, "noSuchObject");
	break;
      case NO_SUCH_INSTANCE_EXCEPTION:
	sprintf(buf, "noSuchInstance");
	break;
      case END_OF_MIB_VIEW_EXCEPTION:
	sprintf(buf, "endOfMibView");
	break;
#endif				/* SR_SNMPv2 */

      default:
	DPRINTF((APTRACE, "SPrintVarBindValue: Illegal type: 0x%x\n", vb_ptr->value.type));
	free(buffer);
	return (0);

	break;
    };				/* end of switch */
    free(buffer);
    /* do next one now */
    return (0);
}				/* end of SPrintVarBindValue() */

/*
 * SPrintAscii:
 *
 *   This routine sprint's out the contents  of  a  Octet  String's
 *   value  as  an ascii string if the value only contains print-
 *   able characters.  It is called with a pointer to the  Octet-
 *   String  construct and checks if the string is printable.  If
 *   it is not it returns a -1 value, otherwise it returns a 1.
 *
 * 11/1/93 DBL
 * Converted PrintAscii to this function.
 */
int
SPrintAscii(os_ptr, buf)
    OctetString    *os_ptr;
    char           *buf;
{
    long            i;
    char            buffer[4];

    for (i = 0; i < os_ptr->length; i++) {
	/* ASCII space is not a printable character in MSC */
	if (((!isprint((int) os_ptr->octet_ptr[i])) && (os_ptr->octet_ptr[i] != 0x20) &&
	(os_ptr->octet_ptr[i] != 0x0a) && (os_ptr->octet_ptr[i] != 0x0d)) ||
	    ((os_ptr->octet_ptr[i] == 0x00) && (i != os_ptr->length - 1))) {
	    return (-1);
	}
    }

    strcpy(buf, "");
    for (i = 0; i < os_ptr->length; i++) {
	switch (os_ptr->octet_ptr[i]) {
	  case 0x00:
	  case 0x0d:
	  case 0x0a:
/*	    strcat(buf, "\n");*/
	    return (1);
	    break;
	  default:
	    sprintf(buffer, "%c", os_ptr->octet_ptr[i]);
	    strcat(buf, buffer);
	}
    }

    return (1);
}				/* SPrintAscii() */
