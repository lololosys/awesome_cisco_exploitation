/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_secure.c,v 3.2.60.1 1996/03/18 21:48:57 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_secure.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_secure.c,v $
 * Revision 3.2.60.1  1996/03/18  21:48:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  12:34:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  17:14:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:00:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:25  hampton
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
static char     rcsid[] = "secure.c,v 1.33 1995/02/06 18:11:25 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * secure.c,v
 * Revision 1.33  1995/02/06  18:11:25  battle
 * Changes for vrtx.
 *
 * Revision 1.32  1995/01/18  20:07:45  reid
 * If FTP and SR_WINSOCK are both defined, don't define baseclock twice
 *
 * Revision 1.31  1995/01/17  16:37:16  levi
 * Added version of BaseClock for SR_WINSOCK
 *
 * Revision 1.30  1994/12/09  19:43:22  reid
 * removed extern global_start_time. This only affects FTP && SR_AGENT
 *
 * Revision 1.29  1994/11/23  17:34:35  reid
 * only need one BaseClock for FTP
 *
 * Revision 1.28  1994/11/02  14:44:42  battle
 * put current_context inside #ifdef DUAL_ROLE_ENTITY
 *
 * Revision 1.27  1994/10/28  16:38:01  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.26  1994/10/28  14:51:26  levi
 * Misc changes to support mlm coexistence
 *
 * Revision 1.25  1994/10/26  21:52:03  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.24  1994/09/30  16:10:58  battle
 * hprt changes
 *
 * Revision 1.23  1994/09/19  13:39:20  levi
 * Removed #include "snmpuser.h" from files that don't need it.
 *
 * Revision 1.22  1994/08/16  14:11:50  reid
 * added UNIXWARE
 *
 * Revision 1.21  1994/08/08  16:03:59  battle
 * Fixed same problem in CheckClassMibView.
 *
 * Revision 1.20  1994/08/08  16:02:20  battle
 * Fixed bit-rotation problem in view mask.
 *
 * Revision 1.19  1994/08/02  15:29:26  reid
 * include timeb.h if FTP is defined
 *
 * Revision 1.18  1994/07/29  13:54:27  reid
 * move #include <errno.h> to before #include "snmp.h"
 *
 * Revision 1.17  1994/05/25  19:38:31  levi
 * Added SetAgentContext and SetManagerContext functions for switching between
 *   agent and manager 'context' in a dual-role entity.
 *
 * Revision 1.16  1994/04/18  18:28:46  battle
 * pSOS changes
 *
 * Revision 1.15  1994/04/11  14:23:12  levi
 * Added #include <sys/types.h> inside #ifdef FTP.  Must make sure DEVKIT
 * include path is before MSC include path.
 *
 * Revision 1.14  1994/04/11  14:09:20  levi
 * Added #ifdef's for FTP
 *
 * Revision 1.13  1994/03/28  16:14:45  levi
 * Updates for WINDOWS port of reqlib and scriptlib
 *
 * Revision 1.12  1994/03/11  16:23:39  kallen
 * Modified so that dgux systems run correctly
 *
 * Revision 1.11  1994/02/14  19:12:55  battle
 * Changes for sox/genmake Makefile automation.
 *
 * Revision 1.10  1994/02/14  16:39:22  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.9  1994/02/08  18:43:35  reid
 * added support for WinSock
 *
 * Revision 1.8  1994/01/25  20:01:26  reid
 * made GetViewIndex return a long instead of an int
 *
 * Revision 1.7  1994/01/20  18:38:05  reid
 * added a new BaseClock() for FTP PCTCP managers (FTP defined, SR_AGENT not
 * defined)
 *
 * Revision 1.6  1994/01/13  15:32:22  battle
 * Changes for Dynix/ptx.
 *
 * Revision 1.5  1993/12/23  16:54:08  battle
 * Added cvs headers to all .c files.
 *
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  8-July-92 DBL
 *  Rewrote InitParties to handle new configuration file.
 *
 *  20-July-92 MRD
 *  Removed "system" call, and replaced with file copy routine.
 *  system("cp x y") is not very portable.
 *
 *  22-July-92 MGH
 *  Added check in InitAclEntries and GetAclIndex to make sure that
 *  aclTarget and aclSubject have the same authentication protocol.
 *
 *  30-July-92 MGH
 *  Changed InitParties() so partyTAddress is stored correctly.
 *
 *  7-Aug-92 MGH
 *  Added the routines, WriteAclFile() and WriteViewFile(), which
 *  save changes to the acl table and view table, respectively.
 *
 *  14-Aug-92 MGH
 *  Added code in InitViewEntries() and WriteViewFile() to read in
 *  and write out viewMask, respectively.
 *
 *  17-Aug-92 MGH
 *  Changed InitParties and WriteParties to read/write the authPrivate
 *  and privPrivate keys as hex strings. Also changed the file permissions
 *  for the configuration files to read-write for the owner.
 *
 *  27-Aug-92 MGH
 *  Added support for the transport domain, rfc1157Domain, in InitParties()
 *  and WriteParties().  This domain is for trivial authentication.
 *
 *  1-Sept-92 DBL
 *  Changed InitParties(), WriteParties(), InitViewEntries() and
 *  WriteViewFile to read/write strings in decimal instead of
 *  hexadecimal. (for authPrivate, privPrivate and viewMask). This was
 *  done because the packaged agent's sscanf cannot handle field widths or
 *  hexadecimal formats.
 *
 * 2-Sept-92 KWK
 * Made errno declaration conditinal on version of Microsoft C used.
 *
 * 10-Sept-92 MGH
 * Changed InitViewEntries() to read in viewMask on a separate line from the
 * configuration file and changed WriteViewFile() to write viewMask to the
 * configuration file on a separate line.
 *
 * 13-Sep-92 KWK
 * Made chmod()'s be conditional on PKG not being defined.  Probably
 * also needs to be taken out for FTP.
 *
 * 8-Oct-92 DLB
 * Added casts to make svr4 compiler shut up.  Primarily arguments to functions
 * expecting char * when unsigned char * was passed and vice versa, plus the
 * fact that strlen return unsigned on svr4 and comparison and % between
 * signed and unsigned quantities have strange semantics the compiler feels
 * compelled to warn us about.
 *
 * 17-Nov-92 MGH
 * Made the memory allocation for the party, acl, and view tables dynamic. The
 * routines InitParties, InitAclEntries, and InitViewEntries now make
 * two passes through the configuration files.  The first pass determines the
 * memory needed and the second pass mallocs the space and fills in the values.
 *
 * 3-Dec-92 MGH
 * Zeroed out the malloc'ed space for the party, acl, and view tables in
 * InitParties, InitAclEntries, and InitViewEntries, respectively.
 *
 * Changed code in WriteParties so keys can have a \0 occur in the string
 * and not be treated as a null terminator (do not use strlen to compute
 * length).
 *
 * 8-Dec-1992 DLB
 *   Lint nits.
 *
 * 10-Dec-1992 DLB
 *   Included snmpuser.h instead of snmpio.h
 *   Changed tmpfile to tempfile everywhere.  tmpfile() is a C lib function.
 *
 * 10-Dec-92 KWK
 *   PKG does not have an netinet/in.h
 *
 * 10-Dec-1992 MGH
 *   Beginning of major rewrite.
 *   Made changes to InitParties(), InitAclEntries(), InitViewEntries(),
 *   WriteParties(), WriteAclFile(), and WriteViewFile() to reflect the
 *   changes in the configuration files.  Added the init_family_entries() and
 *   write_family_file() routines to work with the family table.
 *
 * 11-Dec-92 KWK
 *   Added stuff that PKG Agent needs that used to get included with snmpio.h
 *
 * 11-Dec-1992 MGH
 *   Made changes in GetPartyEntry(), GetPartyPos(), GetPartyIndex(),
 *   and GetAclIndex().  The GetAclIndex() routine also requires three
 *   parameters (all of type long) now.
 *
 * 15-Dec-1992 DLB
 *   Removed extra include of <sys/types.h>.
 *   For the instrumented malloc library, <malloc.h> must preceed "snmp.h".
 *   Fixed with #ifdef MY_MALLOC's.
 *
 * 15-Dec-1992 MGH
 *   Added the following routines: init_proxy_entries(), init_target_entries(),
 *   write_proxy_entries(), and write_target_entries().
 *
 * 17-Dec-1992 MGH
 *   Changed InitSecure to call MakeOID() to make the global OIDs instead of
 *   crafting them by hand.
 *
 * 06-Jan-1993 JDC
 *   Fixed bug in get_target_index() and removed deadwood code with #if 0
 *
 * 06-Jan-1993 MGH
 * Changed all references of "target" to "context", changed clock code to
 * deal with unsigned longs.
 *
 * 07-Jan-1993 MGH
 * Made changes in InitAclEntries(), InitViewEntries(),
 * init_family_entries(), InitContextEntries(), and the corresponding write_*
 * files to reflect the changes in the configuration files.
 *
 * 27-Jan-1993 MGH
 * Added the initialization of restartTime and cacheTime global OIDS to
 * InitSecure().
 *
 * 29-Jan-93 MGH
 * Added partyCloneFrom initialization to InitParties().
 *
 * 5-Feb-93 MGH
 * Moved init_families() to mgrsupp.c and renamed it init_handles() to
 * avoid conflict with the family table.
 *
 * 19-Feb-93 KWK
 * Modifed TPort of the parties table to be in NETWORK byte order.
 *
 * 19-Feb-93 KWK
 * Modified the PKG/FTP backup-file name changing code in all write_*
 * routines.
 *
 * 21-Feb-93 KWK
 * Misc. fixes to the PKG specific write routines.
 *
 * 23-Mar-93 MGH
 * Modified InitParties() and WriteParties() to include a new variable
 * in the party configuration file (partyAuthClock).
 *
 * 21-Apr-1992 DLB
 *    malloc(strlen(x)/3) => malloc((unsigned)strlen(x)/(unsigned)3).
 *
 * 24-Mar-93 MGH
 * Converted internal representation of partyTDomain and partyCloneFrom from
 * OIDs to integers.
 *
 * 4-May-93 MGH
 * Deleted all family table routines and changed global OIDs to use the
 * numbers from the new RFCs.
 *
 * 5-May-93 MGH
 * Changed all LIB_ERROR* calls to DPRINTF messages.
 *
 * 10-May-93 MGH
 * Changed all references of EXPORT to the new compliance levels in
 * RFC 1447.
 *
 * 26-May-1993 DLB
 * Changed MY_MALLOC to THREAD_MALLOC.
 *
 * 28-May-1993 MGH
 * Deleted some unused variables in InitSecure, InitParties, InitAclEntries,
 * InitViewEntries, InitContextEntries, WriteContextFile, and
 * CheckMIBView.
 *
 * 2-June-1993 MGH
 * Started dividing secure.c into system independent and system depedent parts.
 * All init_* routines except InitSecure are moved into a system dependent
 * file and all write_* routines are moved into a system dependent file.
 * All table searching routines and CheckMIBView stay in this file.
 *
 * 10-June-1993 MGH
 * Changed method of accessing party table.  See readv2.c and secure.h
 *
 * 14-June-1993 MGH
 * Changed method of accessing context, acl, and view table.  See readv2.c and
 * secure.h.  Added insertion sort routines for each security table.
 *
 * 17-June-1993 DLB
 * Rewrote parts of CheckMIBView().
 *
 * 18-Jun-1993 DLB
 * Added CheckClassMIBView() from my tree.  Changed second parameter
 * to CheckMIBView() to be the view_index instead of the context OID since
 * that is all that is needed.
 *
 * 18-June-1993 MGH
 * Added the CheckSecurityTables() routine.
 * Moved the delete_element and free routines from the v_ routines for the
 * security tables to this file.
 *
 * 25-June-1993 MGH
 * Changed the name of the security structures from plural names to singular
 * names.
 *
 * 19-July-1993 MGH
 * Changed the #ifdef SNMPD blocks to #ifdef SR_AGENT blocks.
 */

#include "master.h"





#include "sr_snmp.h"

#include "sr_diag.h"

#include "sr_secure.h"

#include "sr_snmpv2mib.h"
#define exit(code)












static unsigned long noAuth_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 1, 1};
static unsigned long noPriv_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 1, 2};

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
static unsigned long desPriv_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 1, 3};

#endif				/* SR_NO_PRIVACY */
static unsigned long md5Auth_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 1, 4};

#endif				/* SR_UNSECURABLE */
static unsigned long snmpUDPDomain_OID[] = {1, 3, 6, 1, 6, 1, 1};
static unsigned long rfc1157Domain_OID[] = {1, 3, 6, 1, 6, 2, 1, 1};
static unsigned long rfc1157noAuth_OID[] = {1, 3, 6, 1, 6, 2, 1, 2};

static unsigned long currentTime_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 2, 1};
static unsigned long restartTime_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 2, 2};
static unsigned long cacheTime_OID[] = {1, 3, 6, 1, 6, 3, 3, 1, 2, 3};

/*
 * Note that if your program will be ONLY a manager or ONLY an agent,
 * you can ignore context switching and everything should work properly.
 */


/*
 * InitSecure: Initializes some widely used global OIDs, calls all of the
 *             SNMPv2 initialization routines.
 *
 *   The former name (pre-snmp12.1.0.0 release) was init_secure().
 */
void
InitSecure()
{

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
    (void) desinit(0);
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

    if ((noAuthOID = MakeOID(noAuth_OID, (short) NOAUTH_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: Cannot create noAuth OID\n"));
	exit(-1);
    }

#ifndef SR_UNSECURABLE
    if ((md5AuthOID = MakeOID(md5Auth_OID, (short) MD5AUTH_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create md5Auth OID\n"));
	exit(-1);
    }
#endif				/* SR_UNSECURABLE */

    if ((noPrivOID = MakeOID(noPriv_OID, (short) NOPRIV_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create noPriv OID\n"));
	exit(-1);
    }

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
    if ((desPrivOID = MakeOID(desPriv_OID, (short) DESPRIV_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create desPriv OID\n"));
	exit(-1);
    }
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

    if ((snmpUDPDomainOID = MakeOID(snmpUDPDomain_OID, (short) SNMPUDPDOMAIN_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create snmpUDPDomain OID\n"));
	exit(-1);
    }
    if ((rfc1157DomainOID = MakeOID(rfc1157Domain_OID, (short) RFC1157_OID_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create rfc1157Domain OID\n"));
	exit(-1);
    }

    if ((rfc1157noAuthOID = MakeOID(rfc1157noAuth_OID, (short) RFC1157NOAUTH_OID_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create rfc1157noAuth OID\n"));
	exit(-1);
    }

    if ((currentTimeOID = MakeOID(currentTime_OID, (short) CURRENT_TIME_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create currentTime OID\n"));
	exit(-1);
    }

    if ((restartTimeOID =
    MakeOID(restartTime_OID, (short) RESTART_TIME_LENGTH)) == (OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create restartTime OID\n"));
	exit(-1);
    }

    if ((cacheTimeOID = MakeOID(cacheTime_OID, (short) CACHE_TIME_LENGTH)) ==
	(OID *) NULL) {
	DPRINTF((APWARN, "InitSecure: unable to create cacheTime OID\n"));
	exit(-1);
    }

    InitParties();
    InitViewEntries();
    InitContextEntries();
    InitAclEntries();

    ComputeViewVectors();

    DPRINTF((APTRACE, "init secure completed\n"));
}				/* InitSecure() */

/*
 * GetPartyEntry: This routine searches for the given party in the party
 *                table.  If it finds a match and the row is active, it
 *                returns the index of an entry in the parties array.
 *
 * Arguments: party - the party that we are looking for.
 *
 * Returns: the index into the parties table on success,
 *         -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_party_entry().
 */
int
GetPartyEntry(party)
    OID            *party;
{
    int             i;
    char            party_name[256];

    if (party == NULL) {
	DPRINTF((APTRACE, "GetPartyEntry: party is NULL\n"));
	return (-1);
    }

    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if ((CmpOID(party, parties[i].party->partyIdentity) == 0) &&
	    (parties[i].party->partyStatus == ACTIVE)) {
	    return (i);
	}			/* if cmp oid */
    }				/* for */

    MakeDotFromOID(party, party_name);
    DPRINTF((APTRACE, "GetPartyEntry: searched entire party table for %s \
but no hits -- returning -1\n", party_name));
    return (-1);
}				/* GetPartyEntry() */

/*
 * GetPartyPos: This routine searches for the given party in the party
 *              table.  If it finds a match, it returns the index for an
 *              entry in the parties array.
 *
 * Arguments: party - the party that we are looking for.
 *
 * Returns: the index into the parties table on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_party_pos().
 */
int
GetPartyPos(party)
    OID            *party;
{
    int             i;
    char            party_name[256];

    if (party == NULL) {
	DPRINTF((APTRACE, "GetPartyPos: party is NULL\n"));
	return (-1);
    }

    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if (CmpOID(party, parties[i].party->partyIdentity) == 0) {
	    return (i);
	}			/* if cmp oid */
    }				/* for */
    MakeDotFromOID(party, party_name);
    DPRINTF((APTRACE, "GetPartyPos: searched entire party table for %s but \
no hits -- returning -1\n", party_name));
    return (-1);
}				/* GetPartyPos() */

/*
 * GetPartyLocation: If the acl_index matches the partyIndex of a party
 *                   entry and the row is active, return the index for that
 *                   entry.
 *
 * Arguments: acl_index - an integer index from the acl table that should
 *                        match the partyIndex of an entry in the party table.
 *
 * Returns: the index of an entry in the party table on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_party_location().
 */
int
GetPartyLocation(acl_index)
    long            acl_index;
{

    int             i;

    if (acl_index < 0) {
	DPRINTF((APTRACE, "GetPartyLocation: negative acl_index\n"));
	return (-1);
    }

    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if ((parties[i].party->partyIndex == acl_index) &&
	    (parties[i].party->partyStatus == ACTIVE)) {
	    return (i);
	}
    }
    DPRINTF((APTRACE, "GetPartyLocation: searched entire table for partyIndex\
 = %d but no hits\n", acl_index));
    return (-1);
}				/* GetPartyLocation() */

/*
 * GetPartyIndex: This routine searches for the given party in the party
 *                table.  If it finds a match, the entry is active, and
 *                the partyLocal field equals location, it returns the index
 *                for an entry in the parties array.
 *
 * Arguments: party - the party that we are looking for.
 *            location - the desired value for the partyLocal field.
 *
 * Returns: the index into the parties table on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_party_index().
 */
int
GetPartyIndex(party, location)
    OID            *party;
    int             location;
{
    int             i;
    char            party_name[256];

    if (party == NULL) {
	DPRINTF((APTRACE, "GetPartyIndex: party is NULL\n"));
	return (-1);
    }
    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if ((CmpOID(party, parties[i].party->partyIdentity) == 0) &&
	    (parties[i].party->partyStatus == ACTIVE)) {
	    if (parties[i].party->partyLocal == (long) location) {
		return (i);
	    }
	}			/* if cmp oid */
    }				/* for */
    MakeDotFromOID(party, party_name);
    DPRINTF((APTRACE, "GetPartyIndex: searched entire party table for %s, \
location = %d, but no hits -- returning -1\n", party_name, location));
    return (-1);
}				/* GetPartyIndex() */

/*
 * GetContextEntry: This routine tries to find an active entry in the context
 *                  table that matches the specified index (contextIndex =
 *                  context).
 *
 * Arguments: context - integer value that should match the contextIndex field
 *                      of a context entry.
 *
 * Returns: the index for the matching entry if it is found, and
 *          -1 otherwise.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_context_entry().
 */
int
GetContextEntry(context)
    long            context;
{

    int             i;

    if (context < 0) {
	DPRINTF((APTRACE, "GetContextEntry: negative acl_index\n"));
	return (-1);
    }
    for (i = 0; i < num_contexts; i++) {
	if (contexts[i].context == NULL) {
	    continue;
	}
	if ((contexts[i].context->contextIndex == context) &&
	    (contexts[i].context->contextStatus == ACTIVE)) {
	    return (i);
	}
    }				/* for */

    DPRINTF((APTRACE, "GetContextEntry: searched entire context table for \
contextIndex = %d, but no hits\n", context));
    return (-1);

}				/* GetContextEntry() */

/*
 * GetViewIndex: Gets the viewIndex for the matching context entry and
 *               returns it if the context is an active entry in the
 *               context table.  Otherwise, it returns -1.
 *
 * Arguments: context - a context (OID) that should match an entry in the
 *                      context table.
 *
 * Returns: the index of the matching entry in the table on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_view_index().
 */
long
GetViewIndex(context)
    OID            *context;
{
    int             i;
    char            context_name[256];

    if (context == NULL) {
	DPRINTF((APTRACE, "GetViewIndex: context is NULL\n"));
	return (-1);
    }

    for (i = 0; i < num_contexts; i++) {
	if (contexts[i].context == NULL) {
	    continue;
	}
	if ((CmpOID(context,
		    contexts[i].context->contextIdentity) == 0) &&
	    (contexts[i].context->contextStatus == ACTIVE)) {
	    return (contexts[i].context->contextViewIndex);
	}
    }				/* for */

    MakeDotFromOID(context, context_name);

    DPRINTF((APTRACE, "GetViewIndex: searched entire context table for %s, but no hits\n", context_name));
    return (-1);

}				/* GetViewIndex() */

/*
 * GetContextIndex: Gets the index for the (possibly) matching entry in the
 *                  context table. The entry must be active to be a match.
 *
 * Arguments: context - context (OID) that should match an entry in the table.
 *
 * Returns: the index of the matching entry in the table on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_context_index().
 */
int
GetContextIndex(context)
    OID            *context;
{
    int             i;
    char            context_name[256];

    if (context == NULL) {
	DPRINTF((APTRACE, "GetContextIndex: context is NULL\n"));
	return (-1);
    }
    for (i = 0; i < num_contexts; i++) {
	if (contexts[i].context == NULL) {
	    continue;
	}
	if ((CmpOID(context,
		    contexts[i].context->contextIdentity) == 0) &&
	    (contexts[i].context->contextStatus == ACTIVE)) {
	    return (i);
	}
    }				/* for */

    MakeDotFromOID(context, context_name);
    DPRINTF((APTRACE, "GetContextIndex: searched entire context table for %s, but no hits\n", context_name));
    return (-1);

}				/* GetContextIndex() */

/*
 * GetContextLocation: Gets the index for the (possibly) matching entry in
 *                the context table. The entry's value for contextLocal
 *                must match location, and the entry must be active.
 *
 * Arguments: context - context (OID) that should match an entry in the table.
 *            location - contextLocal value, must match contextLocal field in
 *                       context entry.
 *
 * Returns: the index of the matching entry on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_context_location().
 */
int
GetContextLocation(context, location)
    OID            *context;
    int             location;
{
    int             i;
    char            context_name[256];

    if (context == NULL) {
	DPRINTF((APTRACE, "GetContextLocation: context is NULL\n"));
	return (-1);
    }
    for (i = 0; i < num_contexts; i++) {
	if (contexts[i].context == NULL) {
	    continue;
	}
	if ((CmpOID(context,
		    contexts[i].context->contextIdentity) == 0) &&
	    (contexts[i].context->contextStatus == ACTIVE) &&
	    (contexts[i].context->contextLocal == (long) location)) {
	    return (i);
	}
    }				/* for */

    MakeDotFromOID(context, context_name);
    DPRINTF((APTRACE, "GetContextLocation: searched entire context table for %s \
with contextLocal = %d, but no hits\n", context_name, location));
    return (-1);

}				/* GetContextLocation() */

/*
 * GetAclIndex: This routine tries to find an entry in the acl table that
 *              matches the specified acl resource, destination, and
 *              source. The matching entry also must be "active".
 *
 * Arguments: resource - should match aclResource field of acl entry.
 *            source - should match aclSubject field of acl entry.
 *            dest - should match aclTarget field of acl entry.
 *
 * Returns: index for the entry if it is found, and
 *          -1 otherwise.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_acl_index().
 */
int
GetAclIndex(resource, source, dest)
    long            resource;
    long            source;
    long            dest;
{

    int             i;
    int             sub_pos, tgt_pos, res_pos;
    char            buffer[256];

    if ((resource < 0) || (source < 0) || (dest < 0)) {
	DPRINTF((APTRACE, "GetAclIndex: one of the indexes is negative.\n"));
	return (-1);
    }

    for (i = 0; i < num_acls; i++) {
	if (access_control[i].acl == NULL) {
	    continue;
	}
	if (resource == access_control[i].acl->aclResources) {
	    if (source == access_control[i].acl->aclSubject) {
		if (dest == access_control[i].acl->aclTarget) {
		    if (access_control[i].acl->aclStatus == ACTIVE) {

			sub_pos = GetPartyLocation(source);
			res_pos = GetContextEntry(resource);
			tgt_pos = GetPartyLocation(dest);

			/*
			 * aclTarget and aclSubject must have identical
			 * authentication protocols.
			 */
			if ((sub_pos >= 0) && (tgt_pos >= 0) &&
			    (res_pos >= 0) &&
			    (parties[sub_pos].party->partyAuthProtocol ==
			     parties[tgt_pos].party->partyAuthProtocol)) {
			    return (i);
			}
		    }		/* if (access_control ... ) */
		}		/* if (dest == .... ) */
	    }			/* if (source == ....) */
	}			/* if (resource == ....) */
    }				/* for (i = 0; .... ) */

    sprintf(buffer, "aclResources = %d, aclSubject = %d, aclTarget = %d ",
	    resource, source, dest);
    DPRINTF((APTRACE, "GetAclIndex: searched entire acl table but no matches \
for %s -- returning -1\n", buffer));
    return (-1);
}				/* GetAclIndex() */

/*
 * GetAclEntry: This routine looks for an entry in the acl table that
 *              matches the specified acl destination and source. The
 *              matching entry must also be an active row.
 *
 *
 * Arguments: source - Should match aclSubject field of matching acl entry.
 *            dest   - Should match aclTarget field of matching acl entry.
 *
 * Returns: index for the entry if it is found, and
 *          -1 otherwise.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_acl_entry().
 */

int
GetAclEntry(source, dest)
    long            source;
    long            dest;
{
    int             i;
    int             sub_pos, tgt_pos;
    char            buffer[256];

    if ((source < 0) || (dest < 0)) {
	DPRINTF((APTRACE, "GetAclEntry: one of the indexes is negative: source: %d, dest = %d\n",
		 source, dest));
	return (-1);
    }

    for (i = 0; i < num_acls; i++) {
	if (access_control[i].acl == NULL) {
	    continue;
	}
	if (source == access_control[i].acl->aclSubject) {
	    if (dest == access_control[i].acl->aclTarget) {
		if (access_control[i].acl->aclStatus == ACTIVE) {

		    sub_pos = GetPartyLocation(source);
		    tgt_pos = GetPartyLocation(dest);

		    /*
		     * aclTarget and aclSubject must have identical
		     * authentication protocols.
		     */
		    if ((sub_pos >= 0) && (tgt_pos >= 0) &&
			(parties[sub_pos].party->partyAuthProtocol ==
			 parties[tgt_pos].party->partyAuthProtocol)) {
			return (i);
		    }
		}
	    }
	}
    }
    sprintf(buffer, "aclSubject = %d, aclTarget = %d ", source, dest);
    DPRINTF((APTRACE, "GetAclEntry: searched entire acl table for %s but no \
matches -- returning -1\n", buffer));
    return (-1);
}				/* GetAclEntry() */

/*
 * CheckMIBView: This routine checks to see if the given object,
 *               object_instance, is in this MIB view (context).  This goes
 *               through the view table and looks at all entries that have a
 *               viewIndex field that matches the contextViewIndex field of
 *               the given context.  The matching row that has the longest
 *               viewSubtree field (or is lexicographically greater on equal
 *               viewSubtrees) is the correct entry.
 *
 * Arguments: object_instance - is this object in the MIB view?
 *            view_index - the MIB view (matches any viewIndex values in
 *                         the view table).
 *
 * Returns:  1 if the match is an included subtree.
 *          -1 if the match is an excluded subtree, there were no matches, or
 *             another error occurred.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_mib_view().
 */
int
CheckMIBView(object_instance, view_index)
    OID            *object_instance;
    long            view_index;
{
    long            len_tree;
    long            len_bits;
    long            i;
    long            pos;
    unsigned char  *view_mask;
    MIB_VIEW       *longest_match = NULL;


    if (object_instance == NULL) {
	DPRINTF((APTRACE, "CheckMIBView: object_instance is NULL.\n"));
	return (-1);
    }

    /*
     * Need to find the longest viewSubtree shorter than or equal to the
     * object_instance in length (or the lexically greatest of these if there
     * are more than one).
     */

    for (i = 0; i < num_views; i++) {
	if (party_views[i].mib_view == NULL) {
	    continue;
	}
	if ((party_views[i].mib_view->viewIndex == view_index) &&
	    (party_views[i].mib_view->viewStatus == ACTIVE)) {

	    len_tree = party_views[i].mib_view->viewSubtree->length;

	    if (object_instance->length >= len_tree) {

		view_mask = party_views[i].mib_view->viewMask->octet_ptr;
		len_bits = party_views[i].mib_view->viewMask->length * 8;

		/*
		 * Check to see if the viewSubtree matches the object. There
		 * is a match if the two are exactly equal or the viewMask
		 * has a value of 0 (wildcard) where they differ.
		 */
		for (pos = 0; pos < len_tree; pos++) {
		    if ((pos >= len_bits ||
		      (view_mask[pos / 8] & (1 << (7 - (pos % 8)))) != 0) &&
			object_instance->oid_ptr[pos] !=
			party_views[i].mib_view->viewSubtree->oid_ptr[pos]) {

			break;
		    }
		}		/* for (pos = 0; ..... ) */

		if (pos == len_tree) {
		    /* find the lexically greatest of the longest matched */
		    if (longest_match == NULL ||
			len_tree > longest_match->viewSubtree->length ||
			CmpOID(party_views[i].mib_view->viewSubtree,
			       longest_match->viewSubtree) > 0) {
			longest_match = party_views[i].mib_view;
		    }
		}		/* if (pos < len_tree) */
	    }			/* if object_instance ... */
	}			/* if (viewIndex == view_index ...) */
    }				/* for */

    /*
     * If there was no match in the view table or the match says that this
     * tree is EXCLUDED from their view, return -1.
     */

    if ((longest_match == NULL) || (longest_match->viewType == EXCLUDED)) {
	DPRINTF((APTRACE, "CheckMIBView: OID not in MIB view.\n"));
	return (-1);
    }

    DPRINTF((APTRACE, "CheckMIBView: OID is in MIB view.\n"));
    return (1);

}				/* CheckMIBView() */

#define FNAME "CheckClassMIBView"
/*
 * CheckClassMIBView:
 *
 * The former name (pre-snmp12.1.0.0 release) was check_class_mib_view().
 */
int
CheckClassMIBView(object_class, view_index)
    OID            *object_class;
    long            view_index;
{
    long            len_tree;
    long            len_bits;
    long            len_common;
    long            i;
    unsigned char  *view_mask;
    long            pos;
    MIB_VIEW       *longest_match = NULL;
    int             AllIncluded = 1;
    int             AllExcluded = 1;


    if (object_class == NULL) {
	DPRINTF((APTRACE, "%s: object_class is NULL.\n", FNAME));
	return (-1);
    }

    /*
     * Need to find all the viewSubtrees which are *longer* than the
     * object_class which match as well as the longest one shorter than or
     * equal to the object_class in length (or the lexically greatest of
     * these if there are more than one).
     * 
     * If all of the (zero or more viewSubtrees in) above collection are
     * EXCLUDED, then we return -1 indicating a method routine call is
     * unnecessary.
     * 
     * Else if some are INCLUDED and some are EXCLUDED then we return 0
     * indicating that the method call must be made, but a post check must
     * also be made with CheckMIBView(object_instance, view_index).
     * 
     * Else all such matches are INCLUDED and we return 1 indicating that the
     * method call should be made and no post check is necessary.
     */

    for (i = 0; i < num_views; i++) {
	if (party_views[i].mib_view == NULL) {
	    continue;
	}
	if (party_views[i].mib_view->viewIndex == view_index &&
	    party_views[i].mib_view->viewStatus == ACTIVE) {

	    len_tree = party_views[i].mib_view->viewSubtree->length;

	    view_mask = party_views[i].mib_view->viewMask->octet_ptr;
	    len_bits = party_views[i].mib_view->viewMask->length * 8;

	    len_common = MIN(len_tree, object_class->length);

	    for (pos = 0; pos < len_common; pos++) {
		if ((pos >= len_bits ||
		     (view_mask[pos / 8] & (1 << (7 - (pos % 8)))) != 0) &&
		    object_class->oid_ptr[pos] !=
		    party_views[i].mib_view->viewSubtree->oid_ptr[pos]) {
		    break;
		}
	    }

	    if (pos == len_common) {
		if (len_tree <= object_class->length) {
		    /*
		     * find the lexically greatest of the longest matched of
		     * this type
		     */
		    if (longest_match == NULL ||
			len_tree > longest_match->viewSubtree->length ||
			CmpOID(party_views[i].mib_view->viewSubtree,
			       longest_match->viewSubtree) > 0) {
			longest_match = party_views[i].mib_view;
		    }
		}
		else {
		    /*
		     * remember whether all the matches of this type are all
		     * EXCLUDED, whether all are INCLUDED
		     */

		    if (party_views[i].mib_view->viewType == INCLUDED) {
			AllExcluded = 0;
		    }
		    if (party_views[i].mib_view->viewType == EXCLUDED) {
			AllIncluded = 0;
		    }
		}
	    }
	}
    }
    if (longest_match == NULL || longest_match->viewType == EXCLUDED) {
	AllIncluded = 0;
    }
    if (longest_match != NULL && longest_match->viewType == INCLUDED) {
	AllExcluded = 0;
    }
    if (AllExcluded) {
	DPRINTF((APTRACE, "%s: all excluded\n", FNAME));
	return -1;
    }
    if (!AllIncluded) {
	DPRINTF((APTRACE, "%s: included and excluded\n", FNAME));
	return 0;
    }
    DPRINTF((APTRACE, "%s: all included\n", FNAME));
    return 1;
}				/* CheckClassMibView() */

#undef FNAME

/*
 * GetViewEntry: Gets the index for the (possibly) matching entry in the
 *               view table.  The matching entry must also be an active row.
 *
 * Arguments: view_index - should match viewIndex of a view table entry.
 *
 * Returns: the index of the matching view table entry on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was get_view_entry().
 */
int
GetViewEntry(view_index)
    long            view_index;
{
    int             i;

    for (i = 0; i < num_views; i++) {
	if (party_views[i].mib_view == NULL) {
	    continue;
	}
	if ((party_views[i].mib_view->viewIndex == view_index) &&
	    (party_views[i].mib_view->viewStatus == ACTIVE)) {
	    return (i);
	}
    }				/* for */

    DPRINTF((APTRACE, "GetViewEntry: searched entire view table for viewIndex = %d, but no hits\n", view_index));
    return (-1);

}				/* GetViewEntry() */


/*
 * PartyInsertSort: This routine uses an insertion sort to make sure
 *                  the linked list of parties is in lexicographic order.
 *
 * arguments: party_ptr - pointer to the party structure to be added.
 *
 * globals: v2party_head_ptr - points to the head of the linked list.
 *          v2party_tail_ptr - points to the tail of the linked list.
 *
 * Both of the globals are declared in secure.h
 *
 * returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was party_insert_sort().
 */
void
PartyInsertSort(party_ptr)
    PARTY          *party_ptr;
{
    PARTY          *prev_ptr, *cur_ptr;

    prev_ptr = NULL;
    cur_ptr = NULL;

    if (party_ptr == NULL) {
	DPRINTF((APWARN, "PartyInsertSort: party_ptr is NULL.\n"));
	return;
    }

    /*
     * If there are no entries yet, make this party the head and tail of the
     * list.
     */
    if (v2party_head_ptr == NULL) {
	party_ptr->next_ptr = NULL;
	v2party_head_ptr = party_ptr;
	v2party_tail_ptr = party_ptr;
	return;
    }

    /*
     * Find correct place in linked list (first entry that is greater than
     * the one being inserted).
     */
    cur_ptr = v2party_head_ptr;

    while (cur_ptr != NULL) {

	if (CmpOID(party_ptr->partyIdentity, cur_ptr->partyIdentity) < 0) {	/* Found position */
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = cur_ptr->next_ptr;
    }

    /*
     * New element needs to be added after the tail ptr.
     */
    if (cur_ptr == NULL) {
	party_ptr->next_ptr = NULL;
	v2party_tail_ptr->next_ptr = party_ptr;
	v2party_tail_ptr = party_ptr;
    }

    /*
     * New element needs to be inserted before the head ptr.
     */

    else if (prev_ptr == NULL) {
	party_ptr->next_ptr = v2party_head_ptr;
	v2party_head_ptr = party_ptr;
    }

    /*
     * New element is inserted somewhere else in the list.
     */
    else {
	party_ptr->next_ptr = cur_ptr;
	prev_ptr->next_ptr = party_ptr;
    }

}				/* PartyInsertSort() */

/*
 * FreePartyElement: This routine frees the party element passed to it.
 *
 * Arguments: party_ptr - party element to be freed and nulled.
 *
 * Returns: nothing.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_party_element().
 */
void
FreePartyElement(party_ptr)
    PARTY          *party_ptr;
{

    if (party_ptr == NULL) {
	DPRINTF((APTRACE, "FreePartyElement: party_ptr already NULL.\n"));
	return;
    }

    if (party_ptr->partyIdentity != NULL) {
	FreeOID(party_ptr->partyIdentity);
	party_ptr->partyIdentity = NULL;
    }
    if (party_ptr->partyTAddress != NULL) {
	FreeOctetString(party_ptr->partyTAddress);
	party_ptr->partyTAddress = NULL;
    }
    if (party_ptr->partyAuthPublic != NULL) {
	FreeOctetString(party_ptr->partyAuthPublic);
	party_ptr->partyAuthPublic = NULL;
    }
    if (party_ptr->partyAuthPrivate != NULL) {
	FreeOctetString(party_ptr->partyAuthPrivate);
	party_ptr->partyAuthPrivate = NULL;
    }
    if (party_ptr->partyPrivPublic != NULL) {
	FreeOctetString(party_ptr->partyPrivPublic);
	party_ptr->partyPrivPublic = NULL;
    }
    if (party_ptr->partyPrivPrivate != NULL) {
	FreeOctetString(party_ptr->partyPrivPrivate);
	party_ptr->partyPrivPrivate = NULL;
    }
    if (party_ptr->partyHandle)
	free(party_ptr->partyHandle);
    free((char *) party_ptr);
    party_ptr = NULL;

}				/* FreePartyElement() */

/*
 * ContextInsertSort: This routine uses an insertion sort to make sure
 *                    the linked list of contexts is in lexicographic order.
 *
 * arguments: context_ptr - pointer to the context structure to be added.
 *
 * globals: v2context_head_ptr - points to the head of the linked list.
 *          v2context_tail_ptr - points to the tail of the linked list.
 *
 * Both of the globals are declared in secure.h
 *
 * returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was context_insert_sort().
 */
void
ContextInsertSort(context_ptr)
    CONTEXT        *context_ptr;
{
    CONTEXT        *prev_ptr, *cur_ptr;

    prev_ptr = NULL;
    cur_ptr = NULL;

    if (context_ptr == NULL) {
	DPRINTF((APWARN, "ContextInsertSort: context_ptr is NULL.\n"));
	return;
    }

    /*
     * If there are no entries yet, make this context the head and tail of
     * the list.
     */
    if (v2context_head_ptr == NULL) {
	context_ptr->next_ptr = NULL;
	v2context_head_ptr = context_ptr;
	v2context_tail_ptr = context_ptr;
	return;
    }

    /*
     * Find correct place in linked list (first entry that is greater than
     * the one being inserted).
     */
    cur_ptr = v2context_head_ptr;

    while (cur_ptr != NULL) {

	if (CmpOID(context_ptr->contextIdentity,
		   cur_ptr->contextIdentity) < 0) {	/* Found position */
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = cur_ptr->next_ptr;
    }

    /*
     * New element needs to be added after the tail ptr.
     */
    if (cur_ptr == NULL) {
	context_ptr->next_ptr = NULL;
	v2context_tail_ptr->next_ptr = context_ptr;
	v2context_tail_ptr = context_ptr;
    }

    /*
     * New element needs to be inserted before the head ptr.
     */

    else if (prev_ptr == NULL) {
	context_ptr->next_ptr = v2context_head_ptr;
	v2context_head_ptr = context_ptr;
    }

    /*
     * New element is inserted somewhere else in the list.
     */
    else {
	context_ptr->next_ptr = cur_ptr;
	prev_ptr->next_ptr = context_ptr;
    }

}				/* ContextInsertSort() */

/*
 * FreeContextElement:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_context_element().
 */
void
FreeContextElement(context_ptr)
    CONTEXT        *context_ptr;
{

    if (context_ptr == NULL) {
	DPRINTF((APTRACE, "FreeContextElement: context entry already NULL.\n"));
	return;
    }

    if (context_ptr->contextIdentity != NULL) {
	FreeOID(context_ptr->contextIdentity);
	context_ptr->contextIdentity = NULL;
    }
    if (context_ptr->contextLocalEntity != NULL) {
	FreeOctetString(context_ptr->contextLocalEntity);
	context_ptr->contextLocalEntity = NULL;
    }
    if (context_ptr->contextProxyDstParty != NULL) {
	FreeOID(context_ptr->contextProxyDstParty);
	context_ptr->contextProxyDstParty = NULL;
    }
    if (context_ptr->contextProxySrcParty != NULL) {
	FreeOID(context_ptr->contextProxySrcParty);
	context_ptr->contextProxySrcParty = NULL;
    }
    if (context_ptr->contextProxyContext != NULL) {
	FreeOID(context_ptr->contextProxyContext);
	context_ptr->contextProxyContext = NULL;
    }
    if (context_ptr->contextHandle)
	free(context_ptr->contextHandle);
    free((char *) context_ptr);
    context_ptr = NULL;

}				/* FreeContextElement() */

/*
 * AclInsertSort: This routine uses an insertion sort to make sure
 *                the linked list of acls is in lexicographic order.
 *
 * arguments: acl_ptr - pointer to the acl structure to be added.
 *
 * globals: v2acl_head_ptr - points to the head of the linked list.
 *          v2acl_tail_ptr - points to the tail of the linked list.
 *
 * Both of the globals are declared in secure.h
 *
 * returns: 1 on success,
 *          -1 on malloc failure
 *
 * The former name (pre-snmp12.1.0.0 release) was acl_insert_sort().
 */
int
AclInsertSort(acl_ptr)
    ACCESS         *acl_ptr;
{
    ACCESS         *prev_ptr, *cur_ptr;
    OID            *curInst, *inInst;
    unsigned long   instBuf[5];

    prev_ptr = NULL;
    cur_ptr = NULL;
    inInst = NULL;
    curInst = NULL;

    if (acl_ptr == NULL) {
	DPRINTF((APTRACE, "AclInsertSort: acl_ptr is NULL.\n"));
	return (-1);
    }

    /*
     * If there are no entries yet, make this acl the head and tail of the
     * list.
     */
    if (v2acl_head_ptr == NULL) {
	acl_ptr->next_ptr = NULL;
	v2acl_head_ptr = acl_ptr;
	v2acl_tail_ptr = acl_ptr;
	return (1);
    }

    instBuf[0] = (unsigned long) acl_ptr->aclTarget;
    instBuf[1] = (unsigned long) acl_ptr->aclSubject;
    instBuf[2] = (unsigned long) acl_ptr->aclResources;

    if ((inInst = MakeOID(instBuf, (short) 3)) == NULL) {
	DPRINTF((APWARN, "AclInsertSort: Cannot make inInst ptr.\n"));
	return (-1);
    }

    /*
     * Find correct place in linked list (first entry that is greater than
     * the one being inserted).
     */
    cur_ptr = v2acl_head_ptr;

    while (cur_ptr != NULL) {
	instBuf[0] = (unsigned long) cur_ptr->aclTarget;
	instBuf[1] = (unsigned long) cur_ptr->aclSubject;
	instBuf[2] = (unsigned long) cur_ptr->aclResources;

	if ((curInst = MakeOID(instBuf, (short) 3)) == NULL) {
	    DPRINTF((APWARN, "AclInsertSort: Cannot make curInst ptr.\n"));
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}

	if (CmpOID(inInst, curInst) < 0) {	/* Found position */
	    FreeOID(curInst);
	    curInst = NULL;
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = (ACCESS *) cur_ptr->next_ptr;
	FreeOID(curInst);
	curInst = NULL;
    }

    FreeOID(inInst);
    inInst = NULL;

    /*
     * New element needs to be added after the tail ptr.
     */
    if (cur_ptr == NULL) {
	acl_ptr->next_ptr = NULL;
	v2acl_tail_ptr->next_ptr = acl_ptr;
	v2acl_tail_ptr = acl_ptr;
    }

    /*
     * New element needs to be inserted before the head ptr.
     */
    else if (prev_ptr == NULL) {
	acl_ptr->next_ptr = v2acl_head_ptr;
	v2acl_head_ptr = acl_ptr;
    }

    /*
     * New element is inserted somewhere else in the list.
     */
    else {
	acl_ptr->next_ptr = cur_ptr;
	prev_ptr->next_ptr = acl_ptr;
    }

    return (1);

}				/* AclInsertSort() */

/*
 * FreeAclElement:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_acl_element().
 */
void
FreeAclElement(acl_ptr)
    ACCESS         *acl_ptr;
{

    if (acl_ptr == NULL) {
	DPRINTF((APTRACE, "FreeAclElement: acl entry already NULL.\n"));
	return;
    }
    free((char *) acl_ptr);
    acl_ptr = NULL;

}				/* FreeAclElement() */

/*
 * ViewInsertSort: This routine uses an insertion sort to make sure
 *                 the linked list of views is in lexicographic order.
 *
 * arguments: view_ptr - pointer to the view structure to be added.
 *
 * globals: v2view_head_ptr - points to the head of the linked list.
 *          v2view_tail_ptr - points to the tail of the linked list.
 *
 * Both of the globals are declared in secure.h
 *
 * returns: 1 on success,
 *         -1 on malloc failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was view_insert_sort().
 */
int
ViewInsertSort(view_ptr)
    MIB_VIEW       *view_ptr;
{
    MIB_VIEW       *prev_ptr, *cur_ptr;
    OID            *curInst, *inInst, *indexInst;
    unsigned long   instBuf[2];

    prev_ptr = NULL;
    cur_ptr = NULL;
    inInst = NULL;
    curInst = NULL;
    indexInst = NULL;

    if (view_ptr == NULL) {
	DPRINTF((APTRACE, "ViewInsertSort: view_ptr is NULL.\n"));
	return (-1);
    }

    /*
     * If there are no entries yet, make this view the head and tail of the
     * list.
     */
    if (v2view_head_ptr == NULL) {
	view_ptr->next_ptr = NULL;
	v2view_head_ptr = view_ptr;
	v2view_tail_ptr = view_ptr;
	return (1);
    }

    instBuf[0] = (unsigned long) view_ptr->viewIndex;

    if ((indexInst = MakeOID(instBuf, (short) 1)) == NULL) {
	DPRINTF((APWARN, "ViewInsertSort: Cannot make indexInst ptr.\n"));
	return (-1);
    }

    if ((inInst = (OID *) CatOID(indexInst, view_ptr->viewSubtree)) == NULL) {
	DPRINTF((APWARN, "ViewInsertSort: Cannot make inInst ptr.\n"));
	FreeOID(indexInst);
	indexInst = NULL;
	return (-1);
    }

    FreeOID(indexInst);
    indexInst = NULL;

    /*
     * Find correct place in linked list (first entry that is greater than
     * the one being inserted).
     */
    cur_ptr = v2view_head_ptr;

    while (cur_ptr != NULL) {
	instBuf[0] = (unsigned long) cur_ptr->viewIndex;
	if ((indexInst = MakeOID(instBuf, (short) 1)) == NULL) {
	    DPRINTF((APWARN, "ViewInsertSort: Cannot make indexInst ptr 2.\n"));
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}
	if ((curInst = (OID *) CatOID(indexInst, cur_ptr->viewSubtree)) == NULL) {
	    DPRINTF((APWARN, "ViewInsertSort: Cannot make curInst ptr.\n"));
	    FreeOID(indexInst);
	    indexInst = NULL;
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}
	FreeOID(indexInst);
	indexInst = NULL;

	if (CmpOID(inInst, curInst) < 0) {	/* Found position */
	    FreeOID(curInst);
	    curInst = NULL;
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = cur_ptr->next_ptr;
	FreeOID(curInst);
	curInst = NULL;
    }

    FreeOID(inInst);
    inInst = NULL;

    /*
     * New element needs to be added after the tail ptr.
     */
    if (cur_ptr == NULL) {
	view_ptr->next_ptr = NULL;
	v2view_tail_ptr->next_ptr = view_ptr;
	v2view_tail_ptr = view_ptr;
    }

    /*
     * New element needs to be inserted before the head ptr.
     */
    else if (prev_ptr == NULL) {
	view_ptr->next_ptr = v2view_head_ptr;
	v2view_head_ptr = view_ptr;
    }

    /*
     * New element is inserted somewhere else in the list.
     */
    else {
	view_ptr->next_ptr = cur_ptr;
	prev_ptr->next_ptr = view_ptr;
    }

    return (1);
}				/* ViewInsertSort() */

/*
 * FreeViewElement:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_view_element().
 */
void
FreeViewElement(view_ptr)
    MIB_VIEW       *view_ptr;
{

    if (view_ptr == NULL) {
	DPRINTF((APTRACE, "FreeViewElement: view entry already NULL.\n"));
	return;
    }
    if (view_ptr->viewSubtree != NULL) {
	FreeOID(view_ptr->viewSubtree);
	view_ptr->viewSubtree = NULL;
    }
    if (view_ptr->viewMask != NULL) {
	FreeOctetString(view_ptr->viewMask);
	view_ptr->viewMask = NULL;
    }
    if (view_ptr->viewHandle)
	free(view_ptr->viewHandle);
    free((char *) view_ptr);
    view_ptr = NULL;

}				/* FreeViewElement() */

/*
 * RecomputeViewVectors:
 *
 * The former name (pre-snmp12.1.0.0 release) was recompute_view_vectors().
 */
int
RecomputeViewVectors(viewIndex)
    long            viewIndex;
{
    return (1);
}				/* RecomputeViewVectors() */


/*
 * ComputeViewVectors:
 *
 * The former name (pre-snmp12.1.0.0 release) was compute_view_vectors().
 */
int
ComputeViewVectors()
{
    return (1);
}				/* ComputeViewVectors() */

/*
 * UpperIt:
 *
 *   The former name (pre-snmp12.1.0.0 release) was upperit().
 */
void
UpperIt(ptr)
    char           *ptr;
{

    while (*ptr != '\0') {
	if (islower(*ptr))
	    *ptr = _toupper(*ptr);
	ptr++;
    }
}				/* UpperIt() */






/*
 * BaseClock:
 *
 *   The former name (pre-snmp12.1.0.0 release) was baseclock().
 */
unsigned long
BaseClock()
{
    return(0);
}				/* BaseClock() */




/*
 * DeleteAclElement: This routine removes the passed element from the
 *                   linked list if it exists in the linked list.
 *
 * Arguments: acl_ptr - The element of the acl table that should be
 *                      deleted.
 *
 * Globals: v2acl_head_ptr and v2acl_tail_ptr, which are declared in
 *          secure.h
 *
 * Returns: 1 on success,
 *          -1 on failure.
 *
 * The former name (pre-snmp12.1.0.0 release) was delete_acl_element().
 */
int
DeleteAclElement(acl_ptr)
    ACCESS         *acl_ptr;
{
    SR_FNAME("DeleteAclElement")
    ACCESS         *prev_ptr, *cur_ptr;
    OID            *curInst, *inInst;
    unsigned long   instBuf[5];

    prev_ptr = NULL;
    cur_ptr = NULL;
    curInst = NULL;
    inInst = NULL;

    if (acl_ptr == NULL) {
	DPRINTF((APWARN, "%s: acl_ptr is NULL.\n", FName));
	return (-1);
    }
    if (v2acl_head_ptr == NULL) {
	DPRINTF((APWARN, "%s: v2acl_head_ptr is NULL.\n", FName));
	DPRINTF((APWARN, "%s: This should not happen.\n", FName));
	FreeAclElement(acl_ptr);
	acl_ptr = NULL;
	return (1);
    }
    if (v2acl_tail_ptr == NULL) {
	DPRINTF((APWARN, "%s: v2acl_tail_ptr is NULL.\n", FName));
	DPRINTF((APWARN, "%s: This should not happen.\n", FName));
	FreeAclElement(acl_ptr);
	acl_ptr = NULL;
	return (1);
    }

    instBuf[0] = (unsigned long) acl_ptr->aclTarget;
    instBuf[1] = (unsigned long) acl_ptr->aclSubject;
    instBuf[2] = (unsigned long) acl_ptr->aclResources;

    if ((inInst = MakeOID(instBuf, (short) 3)) == NULL) {
	DPRINTF((APWARN, "%s: Cannot make indexInst ptr.\n", FName));
	return (-1);
    }

    /*
     * Search for a match in the linked list.
     */

    cur_ptr = v2acl_head_ptr;

    while (cur_ptr != NULL) {
	instBuf[0] = (unsigned long) cur_ptr->aclTarget;
	instBuf[1] = (unsigned long) cur_ptr->aclSubject;
	instBuf[2] = (unsigned long) cur_ptr->aclResources;

	if ((curInst = MakeOID(instBuf, (short) 3)) == NULL) {
	    DPRINTF((APWARN, "%s: Cannot make curInst ptr.\n", FName));
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}

	/*
	 * Found a match.
	 */
	if (CmpOID(inInst, curInst) == 0) {
	    FreeOID(curInst);
	    curInst = NULL;
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = (ACCESS *) cur_ptr->next_ptr;
	FreeOID(curInst);
	curInst = NULL;
    }

    FreeOID(inInst);
    inInst = NULL;

    /*
     * The first element of the tree needs to be deleted.
     */
    if (prev_ptr == NULL) {
	/*
	 * Only one element in the list.
	 */
	if (v2acl_head_ptr == v2acl_tail_ptr) {
	    v2acl_head_ptr = NULL;
	    v2acl_tail_ptr = NULL;
	}
	else {
	    v2acl_head_ptr = (ACCESS *) cur_ptr->next_ptr;
	}
	FreeAclElement(acl_ptr);
	acl_ptr = NULL;
    }
    /*
     * The tail element needs to be deleted.
     */
    else if (v2acl_tail_ptr == cur_ptr) {
	prev_ptr->next_ptr = NULL;
	v2acl_tail_ptr = prev_ptr;
	FreeAclElement(acl_ptr);
	acl_ptr = NULL;
    }
    else {
	prev_ptr->next_ptr = cur_ptr->next_ptr;
	FreeAclElement(acl_ptr);
	acl_ptr = NULL;
    }

    return (1);

}				/* DeleteAclElement() */


/*
 * DeleteContextElement: This routine removes the passed element from the
 *                       linked list if it exists in the linked list.
 *
 * Arguments: context_ptr - The element of the context table that should be
 *                          deleted.
 *
 * Globals: v2context_head_ptr and v2context_tail_ptr, which are declared in
 *          secure.h
 *
 * Returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was delete_context_element().
 */

void
DeleteContextElement(context_ptr)
    CONTEXT        *context_ptr;
{

    CONTEXT        *prev_ptr, *cur_ptr;

    prev_ptr = NULL;
    cur_ptr = NULL;

    if (context_ptr == NULL) {
	DPRINTF((APWARN, "DeleteContextElement: context_ptr is NULL.\n"));
	return;
    }
    if (v2context_head_ptr == NULL) {
	DPRINTF((APWARN, "DeleteContextElement: v2context_head_ptr is NULL.\n"));
	DPRINTF((APWARN, "DeleteContextElement: This should not happen.\n"));
	FreeContextElement(context_ptr);
	context_ptr = NULL;
	return;
    }
    if (v2context_tail_ptr == NULL) {
	DPRINTF((APWARN, "DeleteContextElement: v2context_tail_ptr is NULL.\n"));
	DPRINTF((APWARN, "DeleteContextElement: This should not happen.\n"));
	FreeContextElement(context_ptr);
	context_ptr = NULL;
	return;
    }

    /*
     * Search for a match in the linked list.
     */

    cur_ptr = v2context_head_ptr;

    while (cur_ptr != NULL) {
	/*
	 * Found a match.
	 */
	if (CmpOID(context_ptr->contextIdentity,
		   cur_ptr->contextIdentity) == 0) {
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = cur_ptr->next_ptr;
    }

    /*
     * The first element of the tree needs to be deleted.
     */
    if (prev_ptr == NULL) {
	/*
	 * Only one element in the list.
	 */
	if (v2context_head_ptr == v2context_tail_ptr) {
	    v2context_head_ptr = NULL;
	    v2context_tail_ptr = NULL;
	}
	else {
	    v2context_head_ptr = cur_ptr->next_ptr;
	}
	FreeContextElement(context_ptr);
	context_ptr = NULL;
    }
    /*
     * The tail element needs to be deleted.
     */
    else if (v2context_tail_ptr == cur_ptr) {
	prev_ptr->next_ptr = NULL;
	v2context_tail_ptr = prev_ptr;
	FreeContextElement(context_ptr);
	context_ptr = NULL;
    }
    else {
	prev_ptr->next_ptr = cur_ptr->next_ptr;
	FreeContextElement(context_ptr);
	context_ptr = NULL;
    }

}				/* DeleteContextElement() */


/*
 * DeletePartyElement: This routine removes the passed element from the
 *                     linked list if it exists in the linked list.
 *
 * Arguments: party_ptr - The element of the party table that should be
 *                        deleted.
 *
 * Globals: v2party_head_ptr and v2party_tail_ptr, which are declared in
 *          secure.h
 *
 * Returns: nothing
 *
 * The former name (pre-snmp12.1.0.0 release) was delete_party_element().
 */

void
DeletePartyElement(party_ptr)
    PARTY          *party_ptr;
{

    PARTY          *prev_ptr, *cur_ptr;

    prev_ptr = NULL;
    cur_ptr = NULL;

    if (party_ptr == NULL) {
	DPRINTF((APWARN, "DeletePartyElement: party_ptr is NULL.\n"));
	return;
    }
    if (v2party_head_ptr == NULL) {
	DPRINTF((APWARN, "DeletePartyElement: v2party_head_ptr is NULL.\n"));
	DPRINTF((APWARN, "DeletePartyElement: This should not happen.\n"));
	FreePartyElement(party_ptr);
	party_ptr = NULL;
	return;
    }
    if (v2party_tail_ptr == NULL) {
	DPRINTF((APWARN, "DeletePartyElement: v2party_tail_ptr is NULL.\n"));
	DPRINTF((APWARN, "DeletePartyElement: This should not happen.\n"));
	FreePartyElement(party_ptr);
	party_ptr = NULL;
	return;
    }

    /*
     * Search for a match in the linked list.
     */

    cur_ptr = v2party_head_ptr;

    while (cur_ptr != NULL) {
	/*
	 * Found a match.
	 */
	if (CmpOID(party_ptr->partyIdentity, cur_ptr->partyIdentity) == 0) {
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = cur_ptr->next_ptr;
    }

    /*
     * The first element of the tree needs to be deleted.
     */
    if (prev_ptr == NULL) {
	/*
	 * Only one element in the list.
	 */
	if (v2party_head_ptr == v2party_tail_ptr) {
	    v2party_head_ptr = NULL;
	    v2party_tail_ptr = NULL;
	}
	else {
	    v2party_head_ptr = cur_ptr->next_ptr;
	}
	FreePartyElement(party_ptr);
	party_ptr = NULL;
    }
    /*
     * The tail element needs to be deleted.
     */
    else if (v2party_tail_ptr == cur_ptr) {
	prev_ptr->next_ptr = NULL;
	v2party_tail_ptr = prev_ptr;
	FreePartyElement(party_ptr);
	party_ptr = NULL;
    }
    else {
	prev_ptr->next_ptr = cur_ptr->next_ptr;
	FreePartyElement(party_ptr);
	party_ptr = NULL;
    }

}				/* DeletePartyElement() */

/*
 * DeleteViewElement: This routine removes the passed element from the
 *                    linked list if it exists in the linked list.
 *
 * Arguments: view_ptr - The element of the view table that should be
 *                       deleted.
 *
 * Globals: v2view_head_ptr and v2view_tail_ptr, which are declared in
 *          secure.h
 *
 * Returns: 1 on success,
 *         -1 on failure,
 *
 * The former name (pre-snmp12.1.0.0 release) was delete_view_element().
 */

int
DeleteViewElement(view_ptr)
    MIB_VIEW       *view_ptr;
{
    SR_FNAME("DeleteViewElement")
    MIB_VIEW       *prev_ptr, *cur_ptr;
    OID            *curInst, *inInst, *indexInst;
    unsigned long   instBuf[2];

    prev_ptr = NULL;
    cur_ptr = NULL;
    curInst = NULL;
    inInst = NULL;
    indexInst = NULL;

    if (view_ptr == NULL) {
	DPRINTF((APWARN, "%s: view_ptr is NULL.\n", FName));
	return (-1);
    }
    if (v2view_head_ptr == NULL) {
	DPRINTF((APWARN, "%s: v2view_head_ptr is NULL.\n", FName));
	DPRINTF((APWARN, "%s: This should not happen.\n", FName));
	FreeViewElement(view_ptr);
	view_ptr = NULL;
	return (1);
    }
    if (v2view_tail_ptr == NULL) {
	DPRINTF((APWARN, "%s: v2view_tail_ptr is NULL.\n", FName));
	DPRINTF((APWARN, "%s: This should not happen.\n", FName));
	FreeViewElement(view_ptr);
	view_ptr = NULL;
	return (1);
    }

    instBuf[0] = (unsigned long) view_ptr->viewIndex;

    if ((indexInst = MakeOID(instBuf, (short) 1)) == NULL) {
	DPRINTF((APWARN, "%s: Cannot make indexInst ptr.\n", FName));
	return (-1);
    }

    if ((inInst = (OID *) CatOID(indexInst, view_ptr->viewSubtree)) == NULL) {
	DPRINTF((APWARN, "%s: Cannot make inInst ptr.\n", FName));
	FreeOID(indexInst);
	indexInst = NULL;
	return (-1);
    }

    FreeOID(indexInst);
    indexInst = NULL;

    /*
     * Search for a match in the linked list.
     */

    cur_ptr = v2view_head_ptr;

    while (cur_ptr != NULL) {
	instBuf[0] = (unsigned long) cur_ptr->viewIndex;
	if ((indexInst = MakeOID(instBuf, (short) 1)) == NULL) {
	    DPRINTF((APWARN, "%s: Cannot make indexInst ptr 2.\n", FName));
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}
	if ((curInst = (OID *) CatOID(indexInst, cur_ptr->viewSubtree)) == NULL) {
	    DPRINTF((APWARN, "%s: Cannot make curInst ptr.\n", FName));
	    FreeOID(indexInst);
	    indexInst = NULL;
	    FreeOID(inInst);
	    inInst = NULL;
	    return (-1);
	}
	FreeOID(indexInst);
	indexInst = NULL;

	/*
	 * Found a match.
	 */
	if (CmpOID(inInst, curInst) == 0) {
	    FreeOID(curInst);
	    curInst = NULL;
	    break;
	}

	prev_ptr = cur_ptr;
	cur_ptr = (MIB_VIEW *) cur_ptr->next_ptr;
	FreeOID(curInst);
	curInst = NULL;
    }

    FreeOID(inInst);
    inInst = NULL;

    /*
     * The first element of the tree needs to be deleted.
     */
    if (prev_ptr == NULL) {
	/*
	 * Only one element in the list.
	 */
	if (v2view_head_ptr == v2view_tail_ptr) {
	    v2view_head_ptr = NULL;
	    v2view_tail_ptr = NULL;
	}
	else {
	    v2view_head_ptr = cur_ptr->next_ptr;
	}
	FreeViewElement(view_ptr);
	view_ptr = NULL;
    }
    /*
     * The tail element needs to be deleted.
     */
    else if (v2view_tail_ptr == cur_ptr) {
	prev_ptr->next_ptr = NULL;
	v2view_tail_ptr = prev_ptr;
	FreeViewElement(view_ptr);
	view_ptr = NULL;
    }
    else {
	prev_ptr->next_ptr = cur_ptr->next_ptr;
	FreeViewElement(view_ptr);
	view_ptr = NULL;
    }

    return (1);

}				/* DeleteViewElement() */

/*
 * CheckSecurityTables: This routine updates the security tables.  If any
 *               row in the party, context, view, or acl table has not been
 *               active for SR_MAX_TIMEOUT seconds, it is deleted.
 *
 * Arguments: now - the current time
 *
 * Returns: TRUE - if there are still non-active rows around.
 *          FALSE - if there are not any more non-active rows.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_security_tables().
 */
int
CheckSecurityTables(now)
    unsigned long   now;
{
    int             i;
    int             write_flag, not_active_flag;

    not_active_flag = FALSE;
    write_flag = FALSE;

    for (i = 0; i < num_parties; i++) {
	if (parties[i].party == NULL) {
	    continue;
	}
	if (parties[i].party->partyStatus != ACTIVE) {
	    if ((now - parties[i].last_active) > SR_MAX_TIMEOUT) {
		DPRINTF((APWARN, "CheckSecurityTables: Deleting party row.\n"));
		DeletePartyElement(parties[i].party);
		parties[i].party = NULL;
		write_flag = TRUE;
	    }
	    else {
		not_active_flag = TRUE;
	    }
	}
    }

    if (write_flag == TRUE) {
	(void) WriteParties();
	write_flag = FALSE;
    }

    for (i = 0; i < num_contexts; i++) {
	if (contexts[i].context == NULL) {
	    continue;
	}
	if (contexts[i].context->contextStatus != ACTIVE) {
	    if ((now - contexts[i].last_active) > SR_MAX_TIMEOUT) {
		DPRINTF((APWARN, "CheckSecurityTables: Deleting context row.\n"));
		DeleteContextElement(contexts[i].context);
		contexts[i].context = NULL;
		write_flag = TRUE;
	    }
	    else {
		not_active_flag = TRUE;
	    }
	}
    }

    if (write_flag == TRUE) {
	(void) WriteContextFile();
	write_flag = FALSE;
    }

    for (i = 0; i < num_acls; i++) {
	if (access_control[i].acl == NULL) {
	    continue;
	}
	if (access_control[i].acl->aclStatus != ACTIVE) {
	    if ((now - access_control[i].last_active) > SR_MAX_TIMEOUT) {
		if (DeleteAclElement(access_control[i].acl) == 1) {
		    DPRINTF((APWARN, "CheckSecurityTables: Deleting acl row.\n"));
		    access_control[i].acl = NULL;
		    write_flag = TRUE;
		}
		else {
		    DPRINTF((APWARN, "CheckSecurityTables: Failed to delete acl row.\n"));
		    not_active_flag = TRUE;
		}
	    }
	    else {
		not_active_flag = TRUE;
	    }
	}
    }

    if (write_flag == TRUE) {
	(void) WriteAclFile();
	write_flag = FALSE;
    }

    for (i = 0; i < num_views; i++) {
	if (party_views[i].mib_view == NULL) {
	    continue;
	}
	if (party_views[i].mib_view->viewStatus != ACTIVE) {
	    if ((now - party_views[i].last_active) > SR_MAX_TIMEOUT) {
		if (DeleteViewElement(party_views[i].mib_view) == 1) {
		    DPRINTF((APWARN, "CheckSecurityTables: Deleting view row.\n"));
		    party_views[i].mib_view = NULL;
		    write_flag = TRUE;
		}
		else {
		    DPRINTF((APWARN, "CheckSecurityTables: Failed to delete view row.\n"));
		    not_active_flag = TRUE;
		}
	    }
	    else {
		not_active_flag = TRUE;
	    }
	}
    }

    if (write_flag == TRUE) {
	(void) WriteViewFile();
    }

    return (not_active_flag);

}				/* CheckSecurityTables() */
