/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmontrap.c,v 3.2 1995/11/17 18:39:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_rmontrap.c,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmontrap.c,v $
 * Revision 3.2  1995/11/17  18:39:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:35  shaker
 * Bump version numbers from 2.x to 3.x.
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

#include "master.h"
#include "sr_rmonmib.h"

#ifdef SR_SNMPv2
/* this routine sends a trap to all rfc1157 communities sharing the community
 * octet string passed in as a parameter.  No view access checking is
 * performed on the outgoing varbind list, since it's assumed that this
 * routine is being called by something akin to the rmon alarm control table.
 *
 * Parameters:
 *      genTrap    - The generic trap type.
 *      specTrap   - The specific trap type. This parameter is only used if
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
 *	community  - Octet string representing the v1 trap community string.
 */
static void
SendTrapToV1Community (
    OID            *enterprise,
    long            genTrap,
    long            specTrap,
    VarBind        *vb,
    OctetString     localAddr,
    unsigned long   time_now,
    char           *entV2Trap,
    OctetString    *community)
{
    int             pos;
    int             src_pos, dst_pos, context_pos;
    long            dstAddr = 0;

    /* look through the acl table for matching parties */
    for (pos = 0; pos < num_acls; pos++) {
	/* look for an rfc1157 party with a matching community string */
	if (access_control[pos].acl != NULL
	    && access_check(pos, &src_pos, &dst_pos, &context_pos) >= 0
	    && parties[dst_pos].party->partyTDomain == RFC_1157_DOMAIN
	    && CmpOctetStrings(parties[dst_pos].party->partyAuthPrivate,
			       community) == 0) {

	    /* send the trap */
	    send_comm_v2_trap(enterprise, genTrap, specTrap, vb, localAddr,
			  time_now, src_pos, dst_pos, context_pos, dstAddr);
	}
    }
}				/* SendTrapToV1Community() */

#endif				/* SR_SNMPv2 */

/*
 * Send a trap message of the indicated type to each management station
 * whose security parameters match those given by the eventEntry_t structure
 * associated with this trap.
 *
 * If the eventCommunity octet string is of zero length and the agent
 * has been compiled to understand snmp version 2, do_trap() is
 * called to send the trap to all manager stations configured to receive traps.
 *
 * Parameters:
 *      genTrap    - The generic trap type.
 *      specTrap   - The specific trap type. This parameter is only used if
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
 *	evp	   - Pointer to an eventEntry_t structure, which contains
 *		     trap community information.
 */
void
RmonTrap(genTrap, specTrap, vb, enterprise, entV2Trap, evp)
    long            genTrap;
    long            specTrap;
    VarBind        *vb;
    OID            *enterprise;
    char           *entV2Trap;
    eventEntry_t   *evp;
{
    OctetString     localAddr;
    OctetString    *community;
    unsigned char   buffer[4];
    static unsigned long LocalIP;
    SR_FNAME("do_trap")
    unsigned long   time_now;

#ifndef	SR_SNMPv2
    int             i;
#endif				/* SR_SNMPv2 */

    /*
     * Convert our local IP address into an octet string.
     */
    localAddr.length = 4;
    localAddr.octet_ptr = buffer;
    LocalIP = 0;	/* we will actually get our address later */
    *((long *) localAddr.octet_ptr) = LocalIP;


    time_now = GetTimeNow();

    /* get a pointer to the community we are sending to */
    community = evp->eventCommunity;

#ifndef	SR_SNMPv2
    /* make sure we have a valid trap community */
    if (community->length == 0) {
	goto done;
    }

    for (i = 0; i < num_trap_communities; i++) {
	/*
	 * check to see if this community string matches, remembering to
	 * check for null termination
	 */
	if (community->length < MAXCOMMLEN
	 && memcmp(community->octet_ptr, trap_communities[i].community_name,
		   community->length) == 0
	    && trap_communities[i].community_name[community->length] == 0) {
	    SendTrap_comm(&trap_communities[i], enterprise, genTrap, specTrap,
			  vb, &localAddr, time_now);
	}
    }
#else				/* SR_SNMPv2 */
    /* if this is a zero-length community, broadcast the trap */
    if (community->length == 0) {
	/* send it to everyone */
	do_trap(genTrap, specTrap, vb, enterprise, entV2Trap);
	vb = NULL;		/* freed by do_trap() */
    }
    else {
	/* send the trap to all rfc1157 domain parties with this community */
	SendTrapToV1Community(enterprise, genTrap, specTrap, vb,
			      localAddr, time_now, entV2Trap, community);
    }
#endif				/* SR_SNMPv2 */

#ifndef	SR_SNMPv2
done:
#endif				/* SR_SNMPv2 */

    /* free the varbind list */
    if (vb != NULL) {
	FreeVarBindList(vb);
	vb = NULL;
    }
}				/* do_trap() */
