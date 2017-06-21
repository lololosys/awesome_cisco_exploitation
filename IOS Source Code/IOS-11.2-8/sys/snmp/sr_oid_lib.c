/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_oid_lib.c,v 3.3 1995/12/28 22:35:47 mordock Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_oid_lib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_oid_lib.c,v $
 * Revision 3.3  1995/12/28  22:35:47  mordock
 * CSCdi46168:  improve snmp modularity
 * move some code and globals out of the kernel subsystem and back into
 * the snmp subsystem
 *
 * Revision 3.2  1995/11/17  19:00:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:49:42  hampton
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
static char     rcsid[] = "oid_lib.c,v 1.39 1995/02/10 21:08:33 reid Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * oid_lib.c,v
 * Revision 1.39  1995/02/10  21:08:33  reid
 * put #include <malloc.h> back in for SR_WINDOWS. It will need added for
 * other platforms as well. But since it is not available on several platforms,
 * I will not put it in for everyone.
 *
 * Revision 1.38  1995/02/08  17:38:34  levi
 * Moved CheckOID from mgrutils.c into oid_lib.c
 *
 * Revision 1.37  1995/02/03  19:12:50  levi
 * Reorganization of seclib.
 *   Stuff for command line utilities -> mgrutils.c
 *   OID structure manipulation -> oid_lib.c
 *   OID <--> English translation -> oidtran.c
 *   Type and access constant <--> string conversion -> convrsn.c
 *   Finding nodes in the MIB tree -> mib_lib.c
 *   Reading MIB files and building MIB structures -> mibread[m].c
 *   Parsing SIDs of an OID stored in a char* -> prse_sid.c
 *
 * Revision 1.36  1995/01/17  20:21:07  levi
 * Added Fname in MergeMIBFromFile
 *
 * Revision 1.35  1995/01/17  16:50:26  levi
 * Changes to incorporate NM4W into main tree:
 *   The memory for storing name/number strings in mib_oid_table is now allocated
 *      in a single block (for each), instead of allocating tons of little
 *      blocks for each string.  This gives a big performance boost.
 *   The hash array is now an array of pointers into mib_oid_table.  The functions
 *      for building the hash array have been greatly simplified to reflect this.
 *   The functions for building the OID tree (OID_TREE_ELE structures) have been
 *      simplified (and made non-recursive).
 *   Passing mib_oid_table as a parameter has been eliminated.
 *   When merging things, passing the index into mib_oid_table instead of passing
 *      address of first element.
 *   Freeing memory properly when NewMIBFromFile or MergeMIBFromFile is called.
 *   Did some minor optimization on ParseSubId... functions.
 *   Got rid of SR_UTILITY_FASTSTART stuff.
 *   Made New/MergeMIBFromFile functions a bit more modular.
 * Note that conflicts when loading a MIB are still not really handled.  You
 * can end up with duplicate entries in mib_oid_table.
 *
 * Revision 1.34  1994/11/07  19:15:19  levi
 * Fixed SNMPTCL code to use mffip structure
 *
 * Revision 1.33  1994/11/04  17:44:12  levi
 * Added mffip in correct places
 *
 * Revision 1.32  1994/11/04  15:35:05  pearson
 * Fixed conflicts and removed big stack variables
 *
 * Revision 1.31  1994/11/02  14:50:23  battle
 * removed variable hash_ele unused in function MergeNameOIDHashArray
 *
 * Revision 1.30  1994/10/31  15:07:00  levi
 * More #ifdefs for SNMPTCL specific code
 *
 * Revision 1.29  1994/10/28  19:52:08  levi
 * Added #ifdef SR_SNMPTCL to remove extra mib stuff
 *
 * Revision 1.28  1994/10/28  16:37:52  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.27  1994/10/26  21:51:54  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.26  1994/10/12  21:49:27  levi
 * Added support to NewMIBFromFile for reading size/range/index information
 *   from the new snmpinfo.dat format.
 *
 * Revision 1.25  1994/10/12  16:04:41  reid
 * added #include <windows.h> for SR_WINDOWS
 *
 * Revision 1.24  1994/09/30  16:10:52  battle
 * hprt changes
 *
 * Revision 1.23  1994/09/19  20:13:34  levi
 * Fixed problem where MergeMIBFromFile adds a node to the tree that exists
 *   but is 'UNASSIGNED'.
 *
 * Revision 1.22  1994/09/19  13:28:25  levi
 * Fixed arguments of TypeShortToString, AccessShortToString
 * Fixed pointer assignment for building OID tree
 *
 * Revision 1.21  1994/09/16  21:27:33  levi
 * Plugged file descriptor leak in MergeMIBFromFile
 *
 * Revision 1.20  1994/08/10  15:15:50  reid
 * removed MakeOIDFragFromDot and MakeOIDFromDot. These were moved to
 * oidtran.c, but reappeared here somehow.
 *
 * Revision 1.19  1994/08/10  14:39:40  pearson
 * Removed #ifdef LIGHT from oid_lib.c; this allows name-to-OID translations
 * to work on the reduced snmp-mib.h even when LIGHT is defined.
 *
 * Revision 1.18  1994/08/04  15:37:29  battle
 * pSOS 2.0 changes
 *
 * Revision 1.17  1994/08/02  15:43:56  reid
 * changed the free routine to not free type and access from the MIB_OID structure
 * since they are now short instead of char *
 *
 * Revision 1.16  1994/07/21  13:37:24  levi
 * Put check for NULL mib_oid_tab into InitOIDTree
 *
 * Revision 1.15  1994/06/27  15:54:04  levi
 * Moved oid translation stuff into oidtran.c
 *
 * Revision 1.14  1994/06/24  16:16:12  levi
 * Oops, 'struct MIB_OID' not 'MIB_OID'
 *
 * Revision 1.13  1994/06/24  16:12:03  levi
 * Fixed multiple-declarations-of-mib_oid_table problem
 *
 * Revision 1.12  1994/06/23  18:07:42  levi
 * Changed oid_lib.c to use short integers instead of char* for type/access
 *
 * Revision 1.11  1994/04/18  18:28:37  battle
 * pSOS changes
 *
 * Revision 1.10  1994/03/09  15:04:32  battle
 * Added SR_CISCO to the #if around DecChar2L and friends.
 *
 * Revision 1.9  1994/02/28  17:50:41  levi
 * Removed memory leaks, added MergeNewOIDRoutines, MergeOIDTree,
 * MergeNameOIDHashArray functions to speed calls to MergeMIBFromFile.
 *
 * Revision 1.8  1994/02/14  16:39:17  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.7  1993/12/23  16:54:04  battle
 * Added cvs headers to all .c files.
 *
 *
 *  2/6/89 JDC
 *  Amended copyright notice
 *
 *  Added code to do free's if took error return and seal memory leaks
 *  note that memory leak occurred only under error conditions
 *
 *  Added code to test to see if first subidentifier of object identifier
 *  is greater than or equal to 4 and to fuss if it is
 *
 *  Added documentation/comments
 *
 *  Removed some lint nits
 *
 *  4/23/89 JDC
 *  Corrected erroneous comment above
 *
 *  8/3/89 JDC
 *  Cleaned up some more lint nits
 *
 *  8/8/89 JDC
 *  calloc --> malloc for compat with MSC on PC
 *
 *  9/25/89 JDC
 *  Fix bug in GetStringFromSubIds which caused MakeDotFromOID to
 *  return incorrect result ... error reported by xxxx xxxxxxxx of xxn
 *
 *  10/28/89 JDC
 *  change = to == on three lines to squelch lint warning that didn't matter
 *  then change them back because it broke things
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  01/08/90 JDC
 *  Added SILENT
 *
 *  30-Sep-90 KWK
 *  Modified to support new tree and hash-table structures to speed name-OID
 *  and OID-name translations.
 *
 *  01-Jan-91 JDC
 *  Changed SILENT to LIB_ERROR and LIB_PERROR
 *
 *  01-Jan-91 JDC
 *  Substituted NULLIT(pointer) for LIGHT
 *
 *  01-Jan-91 JDC
 *  Fixed bug in old OID library where would not get a correct match
 *  when entries were missing in snmp-mib.h.  Fix was to add a test on
 *  strlen(mib_oid_table[i].number).
 *
 *  18-Feb-91 JDC
 *  Ditched LIB_PERROR
 *
 *  25-Apr-92 MRD
 *  Merged in new CmpNOID(), CatOID(), Inst2IP(), and definitions of ZeroOID,
 *  NullOID, etc for Version 11
 *
 *  13-June-92 JDC
 *  SMP
 *
 *  22-July-92 MRD
 *  MSC 5.1 is not dealing well with isdigit() and isalpha(). So
 *  replaced these macros with explicit tests.
 *
 *  8-Dec-1992 DLB
 *    Lint nits.
 *
 *  10-Dec-1992 DLB
 *    To avoid confusion with the global variable mib_oid_table:
 *    Changed name of parameter to InitNewOIDRoutines to mib_oid_tab.
 *    Changed name of parameter to InitOIDTree to mib_oid_tab.
 *    Changed name of parameter to InitNameOIDHashArray to mib_oid_tab.
 *
 *  14-Dec-1992 DLB
 *      Added alternate version of file for agent.
 *      Changed min() to MIN() and removed #define min() ...
 *
 *  15-Dec-1992 DLB
 *      Removeed #undef min
 *
 *  11-Jan-1992 DLB
 *    Added inst_to_octetstring, InstToInt, int_to_inst, octetstring_to_inst.
 *    These are experimental and are being used in my implementation of the
 *    rmon mib.
 *
 *  5-May-1993 MGH
 *  Changed all LIB_ERROR* messages to DPRINTF messages.
 *
 *  18-May-1993 MGH
 *  Changed parse_sub_id_* routines to return ints.  They return -1 on
 *  failure and 0 on success.  Added parameter to these routines called
 *  value which will hold the value parsed by the routine if it is successful.
 *  This value used to be returned by these routines.
 *
 *  14-June-1993 DAR
 *  removed experimental inst_to_octetstring, InstToInt, int_to_inst,
 *  octetstring_to_inst.
 *
 *  19-July-1993 MGH
 *  Changed the #ifndef SNMPD blocks to #ifndef SR_AGENT blocks.
 */

#include "master.h"

#include "sr_snmp.h"
#include "sr_diag.h"
#include "sr_snmpuser.h"

/*
 * This is useful in all the scalar access methods.
 */
static unsigned long IDzero[] = {0};

#define LN_IDzero (sizeof( IDzero ) / sizeof(unsigned long))
OID             ZeroOid = {LN_IDzero, IDzero};

/*
 * This is useful in several cases when a "NULL" OID is needed.
 */
static unsigned long _nullOID[] = {0L, 0L};
OID             nullOID = {2, _nullOID};


/*
 * CmpNOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was cmp_n_oid().
 */
int
CmpNOID(oida, oidb, length)	/* if a < b return neg; if b < a return pos */
    OID            *oida, *oidb;
    int             length;	/* if 0xffff, longer oid is greater */
{

    long            i, runlength, diff;

    runlength = MIN(oida->length, oidb->length);
    runlength = MIN(runlength, length);

    for (diff = i = 0; (i < runlength) && !diff; i++) {
        if (oida->oid_ptr[i] != oidb->oid_ptr[i]) {
            if (oida->oid_ptr[i] < oidb->oid_ptr[i])
                diff = -1;
            else
                diff = 1;
        }
    }

    if (diff == 0 && (oida->length < length || oidb->length < length)) {
	diff = oida->length - oidb->length;
    }

    return ((int) diff);

}

/*
 * Inst2IP:
 *
 * Return an IP address derived from an OID instance.
 *     1) The IP address is in network byte order.
 *     2) A NEXT searchType returns the next possible IP address.
 *     3) Any instance elements beyond the 4th element are ignored.
 *
 * NOTE: The inst_lib.c file (in /snmpd/shared) has a similarly named routine,
 * InstToIP(), which performs a slightly different function. See inst_lib.c
 * for more information.
 *
 * The former name (pre-snmp12.1.0.0 release) was inst2ip().
 */
int
Inst2IP(inst, instLength, ipAddr, searchType)
    unsigned long   inst[];
    int             instLength;
    unsigned long  *ipAddr;
    int             searchType;
{
    int             i, carry;
    unsigned long   tmp;

    *ipAddr = 0L;

    if (searchType == GET_REQUEST_TYPE) {
	if (instLength != 4) {
	    return (0);
	}

	for (i = 3; i >= 0; i--) {
	    if (inst[i] > 255) {
		return (0);
	    }

	    ((unsigned char *) ipAddr)[i] = (unsigned char) inst[i];
	}

	return (1);
    }
    else {			/* must be a NEXT (GET_NEXT_REQUEST_TYPE) */
	carry = (instLength >= 4);

	for (i = ((instLength > 4) ? 3 : instLength - 1); i >= 0; i--) {
	    if ((tmp = inst[i] + carry) > 255) {
		*ipAddr = 0L;
		carry = 1;
	    }
	    else {
		((unsigned char *) ipAddr)[i] = (unsigned char) tmp;
		carry = 0;
	    }
	}

	return ((carry == 0));
    }
}

/*
 * CmpOIDClass:
 *
 * The former name (pre-snmp12.1.0.0 release) was cmp_oid_class().
 */
int
CmpOIDClass(ptr1, ptr2)
    OID            *ptr1, *ptr2;
{
    int             i;
    int             min = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* compare until the shorter oid runs out */
    for (i = 0; i < min; i++) {
	/*
	 * First check if not equal.  This prevents two comparisons on each
	 * interation.
	 */
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i]) {
	    if (ptr1->oid_ptr[i] > ptr2->oid_ptr[i]) {
		return (1);
	    }
	    if (ptr1->oid_ptr[i] < ptr2->oid_ptr[i]) {
		return (-1);
	    }
	}
    }

    /*
     * tie only indicates that they are equal up to the length of the shorter
     */
    return (0);
}

/*
 * CmpOID: Compares two OIDs.
 *
 * Returns:   0 if contents of ptr1 have the same length and values
 *              as the contents of ptr2,
 *          < 0 if contents of ptr1 are less than (or smaller") than
 *              the contents of ptr2,
 *          > 0 otherwise.
 *
 * The former name (pre-snmp12.1.0.0 release) was cmp_oid_values().
 */
int
CmpOID(ptr1, ptr2)
    OID            *ptr1, *ptr2;
{
    int             i;
    int             min = (int) ((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* check for mismatched values */
    for (i = 0; i < min; i++) {
	/*
	 * First check if not equal.  This prevents two comparisons on each
	 * interation.
	 */
	if (ptr1->oid_ptr[i] != ptr2->oid_ptr[i]) {
	    if (ptr1->oid_ptr[i] > ptr2->oid_ptr[i]) {
		return (1);
	    }
	    if (ptr1->oid_ptr[i] < ptr2->oid_ptr[i]) {
		return (-1);
	    }
	}
    }

    /*
     * equal for as long as the shorter one holds out.  The longer should be
     * considered bigger
     */
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);


    /* they are equal for their entire mutual lengths */
    return (0);
}				/* CmpOID() */

/*
 * CatOID: allocates space and concatenates the contents of two OIDs into a
 *         new OID.
 *
 * Returns: a pointer to the new,
 *          0 if error
 *
 * The former name (pre-snmp12.1.0.0 release) was cat_oid().
 */
OID            *
CatOID(oida, oidb)
    OID            *oida, *oidb;
{
    OID            *oid;
    int             i, j, length;

    if ((length = oida->length + oidb->length) > MAX_OID_SIZE) {
	DPRINTF((APTRACE, "CatOID: Too long: %d\n", length));
	return (NULL);
    }

    if ((oid = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "CatOID, cannot malloc oid\n"));
	return (NULL);
    }

    if (!(oid->oid_ptr =
    (unsigned long *) malloc((unsigned) (length * sizeof(unsigned long))))) {
	DPRINTF((APWARN, "CatOID, cannot malloc oid_ptr\n"));
	free((char *) oid);
	return (NULL);
    }

    for (j = i = 0; i < oida->length; oid->oid_ptr[j++] = oida->oid_ptr[i++]);
    for (i = 0; i < oidb->length; oid->oid_ptr[j++] = oidb->oid_ptr[i++]);
    oid->length = length;

    return (oid);
}				/* end of CatOID */

 /*
  * CloneOID: Clone the OID pointed to by oid_ptr and return the pointer to
  * the cloned OID.
  * 
  * The former name (pre-snmp12.1.0.0 release) was clone_oid().
  */
OID            *
CloneOID(oid_ptr)
    OID            *oid_ptr;
{
    OID            *new_oid_ptr;
    int             i;

    if (oid_ptr == NULL) {
	return (NULL);
    }
    if ((new_oid_ptr = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "CloneOID: clone oid struct malloc\n"));
	FreeOID(new_oid_ptr);
	new_oid_ptr = NULL;
	return (NULL);
    }
    new_oid_ptr->length = 0L;
    new_oid_ptr->oid_ptr = NULL;

    if ((oid_ptr->length == 0) || (oid_ptr->oid_ptr == NULL)) {
	return (new_oid_ptr);
    }
    if ((new_oid_ptr->oid_ptr =
	 (unsigned long *)
	 malloc((unsigned) MAX((oid_ptr->length *
				sizeof(unsigned long)), 1))) == NULL) {
	DPRINTF((APWARN, "CloneOID: clone oid malloc\n"));
	return (NULL);
    }
    for (i = 0; i < oid_ptr->length; i++) {
	new_oid_ptr->oid_ptr[i] = oid_ptr->oid_ptr[i];
    }
    new_oid_ptr->length = oid_ptr->length;
    return (new_oid_ptr);
}

 /*
  * ClonePartialOID: Clone the indicated portion of oid_ptr and return the
  * cloned pointer.
  * 
  * The former name (pre-snmp12.1.0.0 release) was clone_partial_oid().
  */
OID            *
ClonePartialOID(oid_ptr, start, len)
    OID            *oid_ptr;
    int             start;
    int             len;
{
    OID            *new_oid_ptr;
    int             i;

    if (oid_ptr == NULL) {
	return (NULL);
    }
    if ((new_oid_ptr = (OID *) malloc(sizeof(OID))) == NULL) {
	DPRINTF((APWARN, "ClonePartialOID: clone oid struct malloc\n"));
	FreeOID(new_oid_ptr);
	new_oid_ptr = NULL;
	return (NULL);
    }
    new_oid_ptr->length = 0L;
    new_oid_ptr->oid_ptr = NULL;

    if ((oid_ptr->length == 0) || (oid_ptr->oid_ptr == NULL)) {
	return (new_oid_ptr);
    }

    if ((new_oid_ptr->oid_ptr =
	 (unsigned long *)
	 malloc((unsigned) MAX((len *
				sizeof(unsigned long)), 1))) == NULL) {
	DPRINTF((APWARN, "ClonePartialOID: clone oid malloc\n"));
	return (NULL);
    }
    for (i = start; i < start + len; i++) {
	new_oid_ptr->oid_ptr[i - start] = oid_ptr->oid_ptr[i];
    }
    new_oid_ptr->length = len;

    return (new_oid_ptr);
}

/*
 * CheckOID:
 *
 *   The former name (pre-snmp12.1.0.0 release) was chk_oid().
 */
int
CheckOID(oid1_ptr, oid2_ptr)
    OID            *oid1_ptr;
    OID            *oid2_ptr;
{
    int             i;

    for (i = 0; i < oid1_ptr->length; i++) {
	if (oid1_ptr->oid_ptr[i] < oid2_ptr->oid_ptr[i])
	    return (-1);
    }
    return (0);
}				/* CheckOID() */
