/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_trap.c,v 3.3.28.2 1996/08/28 13:10:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_trap.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_trap.c,v $
 * Revision 3.3.28.2  1996/08/28  13:10:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.28.1  1996/04/10  05:17:01  jjohnson
 * CSCdi54210:  Cannot generate SNMP traps at interrupt level
 * Detect and prevent any evil doers
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/20  02:36:12  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1995/11/17  19:01:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:18:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:10  hampton
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
static char     rcsid[] = "trap.c,v 1.16 1995/02/16 17:11:17 vaughn Exp";
#endif				/* lint */

/*
 * Revision History:
 *
 * trap.c,v
 * Revision 1.16  1995/02/16  17:11:17  vaughn
 * Removed an unused ifndef SR_PSOS
 *
 * Revision 1.15  1995/02/16  17:05:24  vaughn
 * Added an ifdef for SCO that retries the sendto when it fails. sendto was failing
 * on the first try, but would succeed on the second try on the SCO box
 *
 * Revision 1.14  1995/02/06  18:12:06  battle
 * Changes for vrtx.
 *
 * Revision 1.13  1994/12/08  17:10:47  reid
 * changes to support WinSock. Only affected headear files
 *
 * Revision 1.12  1994/11/02  16:15:31  battle
 * removed ununsed variable systemString and added an ARGSUSED
 *
 * Revision 1.11  1994/10/28  17:33:11  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.10  1994/10/26  21:59:33  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.9  1994/08/04  15:38:04  battle
 * pSOS 2.0 changes
 *
 * Revision 1.8  1994/06/13  17:30:55  hecht
 * Added the snmpv1_access_check() routine, which does MIB view checking
 * for outgoing SNMPv1 trap packets on a bilingual agent. The routine
 * currently checks the varbind list and the enterprise OID to make sure they
 * are in the community string's MIB view.
 *
 * Revision 1.7  1994/04/18  18:29:07  battle
 * pSOS changes
 *
 * Revision 1.6  1994/03/11  16:56:45  kallen
 * Took out #ifndef DGUX, as this no longer works on DGUX systems
 *
 * Revision 1.5  1994/03/11  16:25:08  kallen
 * Modified so dgux systems run correclty
 *
 * Revision 1.4  1994/02/09  21:48:47  hecht
 * Found one more place where vb_clone was getting freed with a call to
 * FreeVarbind() instead of FreeVarBindList() (in send_comm_v2_trap() ). This is
 * not a problem unless some kind of error occurs and the vb parameter passed
 * into do_trap() has more than one varbind in its varbind list.
 *
 * Revision 1.3  1994/02/09  21:29:25  hecht
 * Added automatic logging information and comments describing latest changes
 * to this file.
 *
 *
 *  New for gw project
 *
 * 17-Aug-1992 DLB
 *     Removed & in front of systemData.sysObjectID in MakePdu() call;
 *       it is *already* an OID *.
 *
 * 18-Aug-1992 MGH
 *     Cloned systemData.sysObjectID for the call to MakePdu() because
 *     FreePdu() frees it otherwise.
 *
 * 27-Aug-1992 MGH
 *     Modified routine so it could send out trivial traps and/or SMP traps.
 *
 * 10-Sept-1992 MGH
 *     Changed from systemData.sysUpTime to GetTimeNow() to get the current
 *     system up time.
 *
 * 14-Sept-1992 MGH
 *     Before vb is linked with the pdu, a call is made to CloneVarBind,
 *     which will make a copy of the vb.  Now the vb will actually be linked
 *     into each pdu that is sent instead of being freed after one message.
 *
 * 18-Sep-1992 DLB
 *     Somebody cloned localAddr into localAddrOs, but then passed &localAddr
 *     to MakePdu().  I changed the code to pass localAddrOs to MakePdu().
 *     I presume this was the intent.
 *
 * 22-Sept-1992 MGH
 *     Moved CloneVarBind(), CloneUInt64(), and CloneBitString() to
 *     make_lib.c in the libraries.  Also renamed CloneVarBind(). It is now
 *     called CloneVarBindList().
 *
 * 30-Sept-1992 MGH
 *     Changed ToSin.sin_port to network-byte order instead of host-byte order.
 *
 * 9-Dec-1992 DLB
 *   Lint nits.
 *
 * 14-Dec-1992 DLB
 *   Removed definition of MAX and MIN.  They are new defined in snmp.h.
 *
 * 2-Feb-1993 MGH
 *   Reworked do_trap() to match the new SNMPv2 specifications.  Moved some of
 *   the code in do_trap() to SendTrap_comm(), SendTrap_party(),
 *   send_comm_v2_trap(), and send_snmpv2_trap() to make the code more modular.
 *
 * 4-Feb-1993 MGH
 *   Changed all calls of make_varbind() to MakeVarBind().
 *
 * 16-Feb-93 KWK
 *   Converted trap_community IP address back to Network Byte Order.
 *
 * 23-Feb-93 KWK
 *   authPrivate and privPrivate needed to be nulled at declaration for
 *   cleanup to work properly.
 *
 * 3-Mar-93 KWK
 *   Fixed memory leak with oid_ptr, oid_ptr2 and V2 traps.
 *
 * 24-Mar-93 MGH
 * Converted internal representation of partyTDomain from an OID to an integer.
 *
 * 10-May-93 MGH
 * Changed all references to EXPORT to match the new compliance levels in
 * RFC 1447.
 *
 * 20-Sep-1993 DLB
 * Converted to use GetSNMPTrapPort().
 *
 * 21-Sept-1993 MGH
 * Changed FreeVarBind(vb_clone) to FreeVarBindList(vb_clone) in two places
 * because vb_clone could be a varbind list instead of just a varbind.
 *
 * 09-Feb-1994 MGH
 * Changed the calling interface of do_trap(), SendTrap_party(),
 * send_snmpv2_trap(), and snmpv2_access_check() by adding a new parameter,
 * entV2Trap, to the end of the parameter list.  These changes, along with some
 * new code, will allow an agent to send SNMPv2 enterprise specific traps. Also
 * changed all calls to MakeOIDFromDot() to use numeric strings when -DLIGHT is
 * defined so these routines will work properly (see systemUpTimeString for
 * example).
 * There has been another change to snmpv2_access_check(). Instead of just
 * checking to see if the "snmpTraps" MIB subtree is in the destination's
 * MIB view, it checks to see if the specific trap being sent is in it's
 * MIB view.
 */

#include "master.h"
#include "address.h"

#include "sr_snmpd.h"
#include "sr_diag.h"
#include "sr_sysmib2.h"
#include "sr_snmpmib2.h"
#include "sr_snmpv2mib.h"
#include "snmp_registry.h"
#include "snmp_debug.h"
#include "logger.h"
#include "msg_snmp.c"			/* Not a typo, see logger.h */

#define TRAP_REQ_ID		0L
#define TRAP_ERROR_NUMBER	0L
#define TRAP_ERROR_INDEX	0L

#ifndef LIGHT
static char     sysUpTimeString[] = "sysUpTime.0";
static char     snmpTrapString[] = "snmpTrap";
static char     snmpTrapsString[] = "snmpTraps";
static char     snmpTrapOIDString[] = "snmpTrapOID";
#else				/* LIGHT */
static char     sysUpTimeString[] = "1.3.6.1.2.1.1.3.0";
static char     snmpTrapString[] = "1.3.6.1.6.3.1.1.4";
static char     snmpTrapsString[] = "1.3.6.1.6.3.1.1.5";
static char     snmpTrapOIDString[] = "1.3.6.1.6.3.1.1.4.1";
#endif				/* LIGHT */


/*
 * Send a trap message of the indicated type to each management station
 * listed in "trap_communities." or to qualified parties listed in the
 * party table (when SNMPv2 is defined).
 *
 * Parameters:
 *	genTrap    - The generic trap type.
 *	specTrap   - The specific trap type. This parameter is only used if
 *                   this is a SNMPv1 enterprise specific trap (genTrap =
 *                   ENTERPRISE_TRAP).
 *      vb         - The varbind list to append to the trap.
 *      enterprise - Will be used as the value for the enterprise field in a
 *                   SNMPv1 trap.
 *      entV2Trap  - Only used when sending a SNMPv2 enterprise specific
 *                   trap. This parameter will be used as the value for the
 *                   snmpTrapOID.0 varbind in the SNMPv2 trap. This parameter
 *                   replaces the specTrap and enterprise fields from a SNMPv1
 *                   trap. This value should be the concatenation of the
 *                   enterprise parameter, "0", and the specTrap field
 *                   (Ex: "enterprise.0.specTrap" - snmp.0.4).
 */
void
do_trap(genTrap, specTrap, vb, enterprise, entV2Trap)
    long            genTrap;
    long            specTrap;
    VarBind        *vb;
    OID            *enterprise;
    char           *entV2Trap;
{
    OctetString     localAddr;
    unsigned char   buffer[4];
    int             i;
    static int      ToSoc = -1;
    static unsigned long LocalIP = 0;
    SR_FNAME("do_trap")

    /*
     * traps cannot be generated from interrupt level.  Catch any evildoers
     */
    if (onintstack()) {
	errmsg(&msgsym(TRAPINTR, SNMP));
	return;
    }

    if (enterprise == NULL) {
	enterprise = systemData.sysObjectID;
    }

    localAddr.length = 4;
    localAddr.octet_ptr = buffer;
    *((long *) localAddr.octet_ptr) = LocalIP;

    /*
     * For each trap receiver, send a trap message.
     */

#ifndef	SR_SNMPv2
    for (i = 0; i < num_trap_communities; i++) {
	SendTrap_comm(&trap_communities[i], enterprise, genTrap, specTrap, vb,
		      &localAddr, ToSoc);
    }
#else				/* SR_SNMPv2 */
    for (i = 0; i < num_acls; i++) {
	if (access_control[i].acl != NULL) {
	    SendTrap_party(i, enterprise, genTrap, specTrap, vb, localAddr,
			   ToSoc, entV2Trap);
	}
    }
#endif				/* SR_SNMPv2 */

    if (vb != NULL) {
	FreeVarBindList(vb);
	vb = NULL;
    }
}				/* do_trap() */

#ifndef SR_SNMPv2
void
SendTrap_comm(trap_community, enterprise, genTrap, specTrap, vb, localAddr,
	      ToSoc)
    TRAP_COMMUNITY *trap_community;
    OID            *enterprise;
    long            genTrap;
    long            specTrap;
    VarBind        *vb;
    OctetString    *localAddr;
    int             ToSoc;
{

    OctetString    *localAddrOs = NULL;
    OctetString    *os = NULL;
    OID            *tmp_enterprise = NULL;
    VarBind        *vb_clone = NULL;
    AuthHeader     *authHeader = NULL;
    Pdu            *pdu = NULL;
    static struct sockaddr_in ToSin;
    static long     trapID;
    long            dstAddr;
    SR_FNAME("SendTrap_comm")

    dstAddr = trap_community->ip_addr;

    /* clone the localAddr */
    if ((localAddrOs = CloneOctetString(localAddr)) == (OctetString *) NULL) {
	DPRINTF((APWARN, "%s: unable to clone local addr.\n", FName));
	goto comm_cleanup;
    }

    /*
     * Clone enterprise so FreePdu() will not free a needed variable
     */

    if ((tmp_enterprise = (OID *) CloneOID(enterprise)) == (OID *) NULL) {
	DPRINTF((APWARN, "%s unable to make tmp_enterprise OID.\n", FName));
	goto comm_cleanup;
    }

    /*
     * Construct a pdu and place it in a network buffer.
     */
    if (!(pdu = MakePdu(TRAP_TYPE, trapID++, TRAP_ERROR_NUMBER,
			TRAP_ERROR_INDEX, tmp_enterprise,
			localAddrOs, genTrap, specTrap,
			GetTimeNow()))) {
	DPRINTF((APWARN, "%s cannot make pdu.\n", FName));
	goto comm_cleanup;
    }
    localAddrOs = (OctetString *) NULL;
    tmp_enterprise = (OID *) NULL;	/* will be deleted during FreePdu */

    if ((vb_clone = CloneVarBindList(vb)) != (VarBind *) NULL) {
	if (LinkVarBind(pdu, vb_clone) == (short) -1) {
	    DPRINTF((APTRACE, "%s: Cannot link varbind with pdu.\n", FName));
	    goto comm_cleanup;
	}
    }
    /*
     * vb_clone is linked into the pdu, so null it so comm_cleanup won't try
     * to free it
     */
    vb_clone = NULL;

    if (BuildPdu(pdu) == -1) {
	DPRINTF((APTRACE, "%s unable to BuildPdu()\n", FName));
	goto comm_cleanup;
    }

    if (!(os = MakeOctetStringFromText(trap_community->community_name))) {
	DPRINTF((APWARN, "%s unable to make octet string.\n", FName));
	goto comm_cleanup;
    }

    if (!(authHeader = MakeAuthentication(os))) {
	DPRINTF((APWARN, "%s unable to make auth header.\n", FName));
	goto comm_cleanup;
    }
    /* Keep os from being multiply freed during comm_cleanup */
    os = (OctetString *) NULL;

    if (BuildAuthentication(authHeader, pdu) == -1) {
	DPRINTF((APTRACE, "%s unable to make octet string.\n", FName));
	goto comm_cleanup;
    }

    ToSin.sin_addr.s_addr = dstAddr;	/* Already in Network Byte Order */
    ToSin.sin_port = htons(GetSNMPTrapPort());
    ToSin.sin_family = AF_INET;

    DPRINTF((APTRACE, "%s: Sending trap pdu to %s.\n", FName,
	     inet_ntoa(ToSin.sin_addr)));

    if (sendto(ToSoc, (char *) authHeader->packlet->octet_ptr,
       (int) authHeader->packlet->length, 0, (struct sockaddr_in *) & ToSin,
	       (int) sizeof(ToSin)) < 0) {
	DPRINTF((APWARN,
	    "%s: Sendto failed, unable to send trap to manager.\n", FName));
    }
    else {
	snmpData.snmpOutPkts++;
	snmpData.snmpOutTraps++;
    }

comm_cleanup:

    /* Blows away packlet, etc, but NOT pdu */
    if (authHeader) {
	FreeAuthentication(authHeader);
	authHeader = NULL;
    }
    /* Free the malloc'ed portions of the output pdu */
    if (pdu) {
	FreePdu(pdu);
	pdu = NULL;
    }
    if (tmp_enterprise != NULL) {
	FreeOID(tmp_enterprise);
	tmp_enterprise = NULL;
    }
    if (vb_clone != NULL) {
	FreeVarBindList(vb_clone);
	vb_clone = NULL;
    }
    if (localAddrOs != NULL) {
	FreeOctetString(localAddrOs);
	localAddrOs = NULL;
    }
    if (os != NULL) {
	FreeOctetString(os);
	os = NULL;
    }

}				/* SendTrap_comm() */

#endif				/* !SR_SNMPv2 */

#ifdef SR_SNMPv2
void
SendTrap_party(pos, enterprise, genTrap, specTrap, vb, localAddr, ToSoc,
	       entV2Trap)
    int             pos;
    OID            *enterprise;
    long            genTrap;
    long            specTrap;
    VarBind        *vb;
    OctetString     localAddr;
    int             ToSoc;
    char           *entV2Trap;
{

    int             src_pos, dst_pos, context_pos;
    long            dstAddr = 0;

    if (access_check(pos, &src_pos, &dst_pos, &context_pos) < 0) {
	return;
    }

    /*
     * If partyTDomain of the destination is equal to rfc1157Domain then send
     * a trivial trap (SNMPv1).
     */
    if (parties[dst_pos].party->partyTDomain == RFC_1157_DOMAIN) {
	if ((snmpData.snmpEnableAuthenTraps == D_snmpEnableAuthenTraps_enabled) ||
	    (genTrap != AUTHEN_FAILURE_TRAP)) {
	    /*
	     * the code as delivered by SNMP Research doesn't perform
	     * the SNMPv2 access check, but we use the view test for
	     * our trap filtering, and rather than duplicate that
	     * code, we'll just do the whole V2 test on V1 traps
	     */
	    if (snmpv2_access_check(src_pos, context_pos, genTrap, vb, entV2Trap) < 0) {
		return;
	    }
	    send_comm_v2_trap(enterprise, genTrap, specTrap, vb, localAddr,
			      ToSoc, src_pos, dst_pos, context_pos, dstAddr);
	}
    }

    else {			/* SNMPv2 trap */
	/*
	 * Do not send a v1 enterprise specific trap to a v2 manager unless
	 * the entV2Trap parameter has a value.
	 */
	if ((genTrap == ENTERPRISE_TRAP) && (entV2Trap == (char *) NULL)) {
	    return;
	}
	if ((snmpTrapData.snmpV2EnableAuthenTraps == SNMPv2_TRAPS_ENABLED) ||
	    (genTrap != AUTHEN_FAILURE_TRAP)) {
	    if (snmpv2_access_check(src_pos, context_pos, genTrap, vb,
				    entV2Trap) < 0) {
		return;
	    }
	    send_snmpv2_trap(genTrap, vb, ToSoc, src_pos,
			     dst_pos, context_pos, dstAddr, entV2Trap);
	}
    }

}				/* SendTrap_party() */

void
send_snmpv2_trap(genTrap, vb, ToSoc, src_pos, dst_pos, context_pos, dstAddr,
		 entV2Trap)
    long            genTrap;
    VarBind        *vb;
    int             ToSoc;
    int             src_pos;
    int             dst_pos;
    int             context_pos;
    long            dstAddr;
    char           *entV2Trap;
{

    VarBind        *vb_clone = NULL;
    Pdu            *pdu = NULL;
    AuthHeader     *authHeader = NULL;
    SR_FNAME("send_snmpv2_trap")
    OID            *privDst = NULL;
    OID            *dstParty = NULL;
    OID            *srcParty = NULL;
    OID            *context = NULL;
    OID            *oid_ptr = NULL;
    OID            *oid_ptr2 = NULL;
    OID            *trap_val = NULL;
    VarBind        *vb_ptr = NULL;
    VarBind        *vb_ptr2 = NULL;
    char            trap_instance[64];
    int             authType, privType;
    unsigned long   src_timestamp, dst_timestamp;
    unsigned long   now;
    OctetString    *authPrivate = NULL;
    OctetString    *privPrivate = NULL;
    OID             instance;
    ObjectInfo      object;
    boolean	    sent = FALSE;

    instance.oid_ptr = NULL;
    instance.length = 0;

    if ((pdu = MakePdu(SNMPv2_TRAP_TYPE,
		       (long) parties[dst_pos].party->snmpTrapNumbers + 1,
		       TRAP_ERROR_NUMBER, TRAP_ERROR_INDEX, (OID *) NULL,
		(OctetString *) NULL, 0L, 0L, (unsigned long) 0)) == NULL) {
	DPRINTF((APWARN, "%s: cannot make pdu.\n", FName));
	return;
    }

    /*
     * The first varbind is sysUpTime.0, the second varbind is snmpTrapOID.0,
     * and more varbinds may be in vb, which is passed in to this function.
     */
    if ((oid_ptr = MakeOIDFromDot(sysUpTimeString)) == NULL) {
	DPRINTF((APWARN, "%s: Could not make oid_ptr\n", FName));
	goto snmpv2_cleanup;
    }
    object.oid = *oid_ptr;
    object.oidtype = TIME_TICKS_TYPE;
    now = GetTimeNow();
    if ((vb_ptr = MakeVarBind(&object, &instance, (void *) &now)) == NULL) {
	DPRINTF((APWARN, "%s: Could not make vb_ptr\n", FName));
	goto snmpv2_cleanup;
    }
    FreeOID(oid_ptr);		/* MakeVarBind does an cat_oid which clones
				 * it */
    oid_ptr = NULL;

    if (LinkVarBind(pdu, vb_ptr) == (short) -1) {
	DPRINTF((APTRACE, "%s: Cannot link varbind with pdu\n", FName));
	goto snmpv2_cleanup;
    }
    vb_ptr = NULL;

    if (genTrap != ENTERPRISE_TRAP) {
	(void) sprintf(trap_instance, "%s.%d", snmpTrapsString, (int) genTrap + 1);
	if ((trap_val = MakeOIDFromDot(trap_instance)) == (OID *) NULL) {
	    DPRINTF((APWARN, "%s: unable to make trap_val\n", FName));
	    goto snmpv2_cleanup;
	}
    }
    else {
	if (entV2Trap == (char *) NULL) {
	    DPRINTF((APWARN, "%s: entV2Trap is NULL.\n", FName));
	    goto snmpv2_cleanup;
	}
	if ((trap_val = MakeOIDFromDot(entV2Trap)) == (OID *) NULL) {
	    DPRINTF((APWARN, "%s: unable to make trap_val\n", FName));
	    goto snmpv2_cleanup;
	}
    }

    (void) sprintf(trap_instance, "%s.0", snmpTrapOIDString);
    if ((oid_ptr2 = MakeOIDFromDot(trap_instance)) == (OID *) NULL) {
	DPRINTF((APWARN, "%s: unable to make oid_ptr2\n", FName));
	goto snmpv2_cleanup;
    }
    object.oid = *oid_ptr2;
    object.oidtype = OBJECT_ID_TYPE;
    if ((vb_ptr2 = MakeVarBind(&object, &instance, (void *) trap_val)) == NULL) {
	DPRINTF((APWARN, "%s: Could not make vb_ptr2\n", FName));
	goto snmpv2_cleanup;
    }
    FreeOID(oid_ptr2);		/* MakeVarBind does an oid_cat which clones
				 * it */
    oid_ptr2 = NULL;
    trap_val = NULL;

    if (LinkVarBind(pdu, vb_ptr2) == (short) -1) {
	DPRINTF((APTRACE, "%s: Cannot link varbind with pdu\n", FName));
	goto snmpv2_cleanup;
    }
    vb_ptr2 = NULL;

    if ((vb_clone = CloneVarBindList(vb)) != (VarBind *) NULL) {
	if (LinkVarBind(pdu, vb_clone) == (short) -1) {
	    DPRINTF((APTRACE, "%s: Cannot link varbind with pdu.\n", FName));
	    goto snmpv2_cleanup;
	}
    }
    /*
     * vb_clone is linked into the pdu, so null it so snmpv2_cleanup won't
     * try to free it
     */
    vb_clone = NULL;

    if (BuildPdu(pdu) == -1) {
	DPRINTF((APTRACE, "%s: unable to BuildPdu()\n", FName));
	goto snmpv2_cleanup;
    }

    authType = parties[src_pos].party->partyAuthProtocol;
    src_timestamp = parties[src_pos].party->partyAuthClock;

#ifndef SR_UNSECURABLE
    if (parties[src_pos].party->partyAuthProtocol == MD5_SNMPv2_AUTHENTICATION) {
	if ((authPrivate = CloneOctetString(parties[src_pos].party->partyAuthPrivate)) ==
	    (OctetString *) NULL) {
	    DPRINTF((APWARN, "%s: Could not clone authPrivate key\n", FName));
	    goto snmpv2_cleanup;
	}
	if (authPrivate->length != KEY_LENGTH) {
	    DPRINTF((APTRACE, "%s: authPrivate key must be 16 bytes long\n", FName));
	    goto snmpv2_cleanup;
	}

    }
    else {
	authPrivate = NULL;
    }
#else				/* SR_UNSECURABLE */
    authPrivate = NULL;
#endif				/* SR_UNSECURABLE */

    privType = parties[dst_pos].party->partyPrivProtocol;
    dst_timestamp = parties[dst_pos].party->partyAuthClock;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
    if (parties[dst_pos].party->partyPrivProtocol == DESPRIV) {
	if ((privPrivate = CloneOctetString(parties[dst_pos].party->partyPrivPrivate)) ==
	    (OctetString *) NULL) {
	    DPRINTF((APWARN, "%s: Could not clone privPrivate key\n", FName));
	    goto snmpv2_cleanup;
	}
	if (privPrivate->length != KEY_LENGTH) {
	    DPRINTF((APTRACE, "%s: privPrivate key must be exactly 16 bytes long\n", FName));
	    goto snmpv2_cleanup;
	}
    }
    else {
	privPrivate = NULL;
    }
#else				/* SR_NO_PRIVACY */
    privPrivate = NULL;
#endif				/* SR_NO_PRIVACY */

#else				/* SR_UNSECURABLE */
    privPrivate = NULL;
#endif				/* SR_UNSECURABLE */

    now = BaseClock();

    if ((dstParty = CloneOID(parties[dst_pos].party->partyIdentity)) == (OID *) NULL) {
	DPRINTF((APWARN, "%s: Cannot clone dstParty OID\n", FName));
	goto snmpv2_cleanup;
    }
    if ((srcParty = CloneOID(parties[src_pos].party->partyIdentity)) == (OID *) NULL) {
	DPRINTF((APWARN, "%s: Cannot clone srcParty OID\n", FName));
	goto snmpv2_cleanup;
    }
    if ((context = CloneOID(contexts[context_pos].context->contextIdentity)) ==
	(OID *) NULL) {
	DPRINTF((APWARN, "%s: Cannot clone context OID\n", FName));
	goto snmpv2_cleanup;
    }
    if ((privDst = CloneOID(parties[dst_pos].party->partyIdentity)) == (OID *) NULL) {
	DPRINTF((APWARN, "%s: Cannot clone dstParty OID\n", FName));
	goto snmpv2_cleanup;
    }

    if ((authHeader = MakeAuthentication(
					 (OctetString *) NULL,
					 privDst,
					 dstParty,
					 srcParty,
					 context,
					 authType,
					 authPrivate,
					 now + src_timestamp,
					 now + dst_timestamp,
					 privType,
					 privPrivate)) == NULL) {
	DPRINTF((APWARN, "%s: unable to make auth header.\n", FName));
	goto snmpv2_cleanup;
    }
    privDst = (OID *) NULL;
    dstParty = (OID *) NULL;
    srcParty = (OID *) NULL;
    context = (OID *) NULL;
    authPrivate = (OctetString *) NULL;
    privPrivate = (OctetString *) NULL;

    if (BuildAuthentication(authHeader, pdu) == -1) {
	DPRINTF((APTRACE, "%s: unable to do build authentication().\n", FName));
	goto snmpv2_cleanup;
    }

    /*
     * We "know" that this message will not be TOO BIG for the destination
     * because the minimum size is 484 bytes.  It is possible to send a
     * gigantogram (because of huge party-names, and/or many extra varbinds),
     * but it is not likely.  This may be fixed in a future release.
     */
    sent = reg_invoke_snmp_send_trap(parties[dst_pos].party->partyTDomain,
				     parties[dst_pos].party->partyTAddress,
				     authHeader->packlet->octet_ptr,
				     authHeader->packlet->length);

    if (sent) {
	parties[dst_pos].party->snmpTrapNumbers++;
    }

snmpv2_cleanup:
    if (snmp_packet_debug) {
	if (sent)
	    snmp_print_message(pdu);
	else
	    buginf("\nSNMP: Unable to send trap");
    }

    /* Blows away packlet, etc, but NOT pdu */
    if (authHeader) {
	FreeAuthentication(authHeader);
	authHeader = NULL;
    }
    /* Free the malloc'ed portions of the output pdu */
    if (pdu) {
	FreePdu(pdu);
	pdu = NULL;
    }
    if (vb_clone != NULL) {
	FreeVarBindList(vb_clone);
	vb_clone = NULL;
    }
    if (dstParty != NULL) {
	FreeOID(dstParty);
	dstParty = NULL;
    }
    if (srcParty != NULL) {
	FreeOID(srcParty);
	srcParty = NULL;
    }
    if (privDst != NULL) {
	FreeOID(privDst);
	privDst = NULL;
    }
    if (context != NULL) {
	FreeOID(context);
	context = NULL;
    }
    if (authPrivate != NULL) {
	FreeOctetString(authPrivate);
	authPrivate = NULL;
    }
    if (privPrivate != NULL) {
	FreeOctetString(privPrivate);
	privPrivate = NULL;
    }
    if (oid_ptr != NULL) {
	FreeOID(oid_ptr);
	oid_ptr = NULL;
    }
    if (oid_ptr2 != NULL) {
	FreeOID(oid_ptr2);
	oid_ptr2 = NULL;
    }
    if (trap_val != NULL) {
	FreeOID(trap_val);
	trap_val = NULL;
    }
    if (vb_ptr != NULL) {
	FreeVarBind(vb_ptr);
	vb_ptr = NULL;
    }
    if (vb_ptr2 != NULL) {
	FreeVarBind(vb_ptr2);
	vb_ptr2 = NULL;
    }

}				/* send_snmpv2_trap() */


/* ARGSUSED */
void
send_comm_v2_trap(enterprise, genTrap, specTrap, vb, localAddr, ToSoc,
		  src_pos, dst_pos, context_pos, dstAddr)
    OID            *enterprise;
    long            genTrap;
    long            specTrap;
    VarBind        *vb;
    OctetString     localAddr;
    int             ToSoc;
    int             src_pos;
    int             dst_pos;
    int             context_pos;
    long            dstAddr;
{
    ipaddrtype      agentAddr;
    OctetString     destAddrOs;
    OctetString    *localAddrOs = NULL;
    OID            *tmp_enterprise = NULL;
    VarBind        *vb_clone = NULL;
    OctetString    *os = NULL;
    boolean	    sent = FALSE;
    Pdu            *pdu = NULL;
    AuthHeader     *authHeader = NULL;
    static long     trapID;
    SR_FNAME("send_comm_v2_trap")

#ifdef SR_SNMPv1
    /*
     * The SNMPv1 trap pdu has an agent address field.  This is very
     * IP-centric, hence this IP-centric hack of assigning an address
     * of 0.0.0.0 if we're not ip, and calling ip via a registry if
     * we are.
     */

    if (parties[dst_pos].party->partyTDomain == RFC_1157_DOMAIN) {
	/*
	 * convert the first four octets of the party TAddress into
	 * an IP address
	 */
	destAddrOs.octet_ptr = parties[dst_pos].party->partyTAddress->octet_ptr;
	destAddrOs.length = 4;
	dstAddr = OctetStringToIP(&destAddrOs);
	agentAddr = reg_invoke_snmp_trap_address(dstAddr);
    } else {
	agentAddr = 0;
    }

    /*
     * Clone localAddr so FreePdu() will not free a static variable
     */

    if ((localAddrOs = IPToOctetString(agentAddr)) == (OctetString *) NULL) {
	DPRINTF((APWARN, "%s: unable to clone local addr.\n", FName));
	goto triv_cleanup;
    }

    /*
     * Clone enterprise so FreePdu() will not free a needed variable
     */
    if ((tmp_enterprise = (OID *) CloneOID(enterprise)) ==
	(OID *) NULL) {
	DPRINTF((APWARN, "%s unable to make tmp_enterprise OID.\n", FName));
	goto triv_cleanup;
    }

    /*
     * Construct a pdu and place it in a network buffer.
     */
    if (!(pdu = MakePdu(TRAP_TYPE, trapID++, TRAP_ERROR_NUMBER,
			TRAP_ERROR_INDEX, tmp_enterprise,
			localAddrOs, genTrap, specTrap,
			GetTimeNow()))) {
	DPRINTF((APWARN, "%s: cannot make pdu.\n", FName));
	goto triv_cleanup;
    }
    tmp_enterprise = (OID *) NULL;
    localAddrOs = (OctetString *) NULL;

    if ((vb_clone = CloneVarBindList(vb)) != (VarBind *) NULL) {
	if (LinkVarBind(pdu, vb_clone) == (short) -1) {
	    DPRINTF((APTRACE, "%s: Cannot link varbind with pdu.\n", FName));
	    goto triv_cleanup;
	}
    }
    /*
     * vb_clone is linked into the pdu, so null it so triv_cleanup won't try
     * to free it
     */
    vb_clone = NULL;

    if (BuildPdu(pdu) == -1) {
	DPRINTF((APTRACE, "%s unable to BuildPdu()\n", FName));
	goto triv_cleanup;
    }

    /*
     * When using SNMPv2 and the partyTDomain = rfc1157Domain, the community
     * string is stored in the partyAuthPrivate field.
     */
    if ((os = CloneOctetString(parties[dst_pos].party->partyAuthPrivate)) ==
	(OctetString *) NULL) {
	DPRINTF((APWARN, "%s unable to make octet string.\n", FName));
	goto triv_cleanup;
    }

    if ((authHeader = MakeAuthentication(
					 os,
					 (OID *) NULL,
					 (OID *) NULL,
					 (OID *) NULL,
					 (OID *) NULL,
					 TRIVIAL_AUTHENTICATION,
					 (OctetString *) NULL,
					 (unsigned long) 0,
					 (unsigned long) 0,
					 (int) 0,
					 (OctetString *) NULL)) ==
	NULL) {
	DPRINTF((APWARN, "%s: unable to make auth header.\n", FName));
	goto triv_cleanup;
    }

    /* make null to keep from being multiply freed during triv_cleanup */
    os = (OctetString *) NULL;

    if (BuildAuthentication(authHeader, pdu) == -1) {
	DPRINTF((APTRACE, "%s unable to do BuildAuthentication().\n", FName));
	goto triv_cleanup;
    }

    sent = reg_invoke_snmp_send_trap(parties[dst_pos].party->partyTDomain,
				     parties[dst_pos].party->partyTAddress,
				     authHeader->packlet->octet_ptr,
				     authHeader->packlet->length);
    if (sent) {
	snmpData.snmpOutPkts++;
	snmpData.snmpOutTraps++;
    }

triv_cleanup:
    if (snmp_packet_debug) {
	if (sent)
	    snmp_print_message(pdu);
	else
	    buginf("\nSNMP: Unable to send trap");
    }

    /* Blows away packlet, etc, but NOT pdu */
    if (authHeader) {
	FreeAuthentication(authHeader);
	authHeader = NULL;
    }
    /* Free the malloc'ed portions of the output pdu */
    if (pdu) {
	FreePdu(pdu);
	pdu = NULL;
    }
    if (tmp_enterprise != NULL) {
	FreeOID(tmp_enterprise);
	tmp_enterprise = NULL;
    }
    if (vb_clone != NULL) {
	FreeVarBindList(vb_clone);
	vb_clone = NULL;
    }
    if (localAddrOs != NULL) {
	FreeOctetString(localAddrOs);
	localAddrOs = NULL;
    }
    if (os != NULL) {
	FreeOctetString(os);
	os = NULL;
    }
#endif				/* SR_SNMPv1 */

}				/* send_comm_v2_trap() */

int
access_check(pos, src_pos, dst_pos, context_pos)
    int             pos;
    int            *src_pos;
    int            *dst_pos;
    int            *context_pos;
{
    int             pos1, pos2, pos3;
    SR_FNAME("access_check")

    pos1 = GetPartyLocation(access_control[pos].acl->aclSubject);
    pos2 = GetPartyLocation(access_control[pos].acl->aclTarget);
    pos3 = GetContextEntry(access_control[pos].acl->aclResources);

    *src_pos = pos1;
    *dst_pos = pos2;
    *context_pos = pos3;

    /* invalid parties */
    if ((pos1 < 0) || (pos2 < 0) || (pos3 < 0)) {
	DPRINTF((APTRACE, "%s: Invalid parties\n", FName));
	return (-1);
    }

    /* aclTarget and aclSubject must use the same partyAuthProtocol */
    if (parties[pos1].party->partyAuthProtocol !=
	parties[pos2].party->partyAuthProtocol) {
	DPRINTF((APTRACE, "%s: Not using same partyAuthProtocol\n", FName));
	return (-1);
    }

    /*
     * For now, the partyTDomain must be snmpUDPDomain or rfc1157Domain.
     */
    if ((parties[pos2].party->partyTDomain != RFC_1157_DOMAIN) &&
	(parties[pos2].party->partyTDomain != SNMP_UDP_DOMAIN)) {
	DPRINTF((APTRACE, "%s: Not using valid transport domain\n", FName));
	return (-1);
    }

    /*
     * The destination party of the trap should be a remote party (partyIndex
     * = pos2), and the source party of the trap should be a local party
     * (partyIndex = pos1).
     */
    if (parties[pos2].party->partyTDomain == SNMP_UDP_DOMAIN) {
	if ((parties[pos1].party->partyLocal == SR_REMOTE) ||
	    (parties[pos2].party->partyLocal == SR_LOCAL)) {
	    DPRINTF((APTRACE, "%s: dstParty not remote or srcParty not local\n",
		     FName));
	    return (-1);
	}
    }

    /*
     * If the entry is not allowed to do SNMPv2-Traps and the partyTDomain is
     * not rfc1157Domain (trivial),  return(-1)
     */
    if (((access_control[pos].acl->aclPrivileges & SNMPv2_TRAP_MASK) !=
	 SNMPv2_TRAP_MASK) && (parties[pos1].party->partyTDomain != RFC_1157_DOMAIN)) {
	DPRINTF((APTRACE, "%s: party not allowed to do SNMPv2-Traps\n", FName));
	return (-1);
    }

    /*
     * If the entry is not allowed to do SNMP-Traps and the partyTDomain is
     * rfc1157Domain (trivial),  return(-1)
     */

#ifdef SR_SNMPv1
    if (((access_control[pos].acl->aclPrivileges & TRAP_MASK) !=
     TRAP_MASK) && (parties[pos1].party->partyTDomain == RFC_1157_DOMAIN)) {
	DPRINTF((APTRACE, "%s: party not allowed to do SNMP-Traps\n", FName));
	return (-1);
    }
#endif				/* SR_SNMPv1 */

    if (parties[pos2].party->partyTAddress->length != TADDR_LEN) {
	DPRINTF((APTRACE, "%s: the length of the partyTAddress is wrong\n", FName));
	return (-1);
    }

    return (0);

}				/* access_check() */

int
snmpv2_access_check(src_pos, context_pos, genTrap, vb, entV2Trap)
    int             src_pos;
    int             context_pos;
    long            genTrap;
    VarBind        *vb;
    char           *entV2Trap;
{

    VarBind        *vb_ptr;
    OID            *oid_ptr, *oid_ptr2;
    SR_FNAME("snmpv2_access_check")
    char            tempString[64];

    vb_ptr = NULL;
    oid_ptr = (OID *) NULL;
    oid_ptr2 = (OID *) NULL;

    /* aclSubject must be a local party */
    if (parties[src_pos].party->partyLocal != SR_LOCAL) {
	DPRINTF((APTRACE, "%s: party not local to agent\n", FName));
	return (-1);
    }

    /*
     * If "snmpTrap" and "snmpTraps" are not included in the aclResource's
     * MIB view, return(-1). If this is an enterprise specific trap, the
     * entV2Trap OID is checked to see whether it is in the aclResource's MIB
     * view.
     */
    if ((oid_ptr = MakeOIDFromDot(snmpTrapString)) ==
	(OID *) NULL) {
	DPRINTF((APWARN, "%s: cannot make snmpTrap obj_id\n", FName));
	return (-1);
    }
    if (genTrap != ENTERPRISE_TRAP) {
	sprintf(tempString, "%s.%d", snmpTrapsString, genTrap + 1);
	if ((oid_ptr2 = MakeOIDFromDot(tempString)) ==
	    (OID *) NULL) {
	    DPRINTF((APWARN, "%s: cannot make snmpTraps obj_id\n", FName));
	    FreeOID(oid_ptr);
	    oid_ptr = (OID *) NULL;
	    return (-1);
	}
    }
    else {
	if (entV2Trap == (char *) NULL) {
	    DPRINTF((APWARN, "%s: SNMPv2 enterprise specific trap string is NULL\n", FName));
	    FreeOID(oid_ptr);
	    oid_ptr = (OID *) NULL;
	    return (-1);
	}

	if ((oid_ptr2 = MakeOIDFromDot(entV2Trap)) == (OID *) NULL) {
	    DPRINTF((APWARN, "%s: cannot make SNMPv2 enterprise specific trap obj_id\n", FName));
	    FreeOID(oid_ptr);
	    oid_ptr = (OID *) NULL;
	    return (-1);
	}
    }


    if ((CheckMIBView(oid_ptr, contexts[context_pos].context->contextViewIndex) < 0) ||
	(CheckMIBView(oid_ptr2, contexts[context_pos].context->contextViewIndex) < 0)) {
	DPRINTF((APTRACE, "%s: snmpTrap(s) not in context's MIB view.\n", FName));
	FreeOID(oid_ptr);
	oid_ptr = (OID *) NULL;
	FreeOID(oid_ptr2);
	oid_ptr2 = (OID *) NULL;
	return (-1);
    }
    FreeOID(oid_ptr);
    oid_ptr = (OID *) NULL;
    FreeOID(oid_ptr2);
    oid_ptr2 = (OID *) NULL;

    /*
     * Check the MIB view for all OBJECTs associated with this trap.
     */
    vb_ptr = vb;
    while (vb_ptr != NULL) {
	if (CheckMIBView(vb_ptr->name, contexts[context_pos].context->contextViewIndex) < 0) {
	    DPRINTF((APTRACE, "%s: vb_ptr->name not in context's MIB view\n", FName));
	    return (-1);
	}
	vb_ptr = vb_ptr->next_var;
    }

    return (0);

}				/* snmpv2_access_check() */

int
snmpv1_access_check(src_pos, context_pos, vb, enterprise)
    int             src_pos;
    int             context_pos;
    VarBind        *vb;
    OID            *enterprise;
{

    VarBind        *vb_ptr;
    SR_FNAME("snmpv1_access_check")

#ifdef SR_SNMPv1
    vb_ptr = NULL;

    /* aclSubject must be a local party */
    if (parties[src_pos].party->partyLocal != SR_LOCAL) {
	DPRINTF((APTRACE, "%s: party not local to agent\n", FName));
	return (-1);
    }

    /*
     * See if the enterprise OID is in the MIB view.
     */
    if (CheckMIBView(enterprise,
		     contexts[context_pos].context->contextViewIndex) < 0) {
	DPRINTF((APTRACE, "%s: Enterprise OID not in context's MIB view.\n",
		 FName));
	return (-1);
    }

    /*
     * Check the MIB view for all OBJECTs associated with this trap.
     */
    vb_ptr = vb;
    while (vb_ptr != NULL) {
	if (CheckMIBView(vb_ptr->name, contexts[context_pos].context->contextViewIndex) < 0) {
	    DPRINTF((APTRACE, "%s: vb_ptr->name not in context's MIB view\n", FName));
	    return (-1);
	}
	vb_ptr = vb_ptr->next_var;
    }
#endif				/* SR_SNMPv1 */

    return (0);

}				/* snmpv1_access_check() */

#endif				/* SR_SNMPv2 */
