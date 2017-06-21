/* $Id: sr_bgpmib.c,v 3.8.10.6 1996/07/03 20:42:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/sr_bgpmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, Robert Snyder (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_bgpmib.c,v $
 * Revision 3.8.10.6  1996/07/03  20:42:32  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.8.10.5  1996/07/01  18:45:01  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.10.4  1996/05/21  09:53:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.10.3  1996/05/04  01:22:45  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.8.10.2  1996/04/04  05:34:41  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.8.10.1  1996/03/18  20:29:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/16  06:58:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.14.2  1996/03/07  09:50:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.1  1996/02/20  01:04:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/22  14:35:25  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/07  22:55:19  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.6  1996/01/24  22:04:19  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.5  1995/12/17  18:31:06  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/18  01:25:09  thille
 * CSCdi38737:  bgp4PathAttrTable requests improperly handled
 * Fix it.
 *
 * Revision 3.3  1995/11/18  00:59:01  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.
 *
 * Revision 3.2  1995/11/17  17:37:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:10:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:27:57  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/28  03:22:01  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.1  1995/06/07 21:11:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "login.h"
#include "../snmp/snmp_api.h"
#include "sr_bgpmib.h"
#include "sr_bgpmib-mib.h"
#include "route.h"
#include "iprouting_debug.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "bgp.h"
#include "bgp_private.h"
#include "neighbor.h"
#include "../tcp/tcp.h"
#include "registry.h"
#include "../snmp/snmp_registry.h"
#include "interface_private.h"		/* required by ip_registry.regh */
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_snmp.h"
/*
 * cruft needed for the parse chain stuff included below...
 */
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

static uchar    bgpversion_octet_str[1];
static OctetString     _bgpVersion;
static OctetString     _bgpPeerLastError;
static boolean  bgp_traps_enabled;	/* bgp trap control */
static uchar	null_aspath[2] = { AS_SET, 0 };
static uchar 	bgp4_aspath_buf[255];
static OctetString bgp4PathAttrASPathSegment;
static uchar 	bgp4_unknown_buf[255];
static OctetString   bgp4PathAttrUnknown;

/*
 * SNMP version 1 OID to identify the SNMP traps.
 * From rfc1657, we know that the enterprise OID for this trap
 * is bgp.
 */
static const OID enterpriseOID = { LNbgp, (ulong *)IDbgp };

/*
 * SNMP version 2 OIDs for the bgp traps
 */
static const OID bgpEstab_trapOID = { LNbgpEstablished,
				      (ulong *)IDbgpEstablished };
static const OID bgpBackTrans_trapOID = { LNbgpBackwardTransition,
					  (ulong*)IDbgpBackwardTransition };
static char bgpEstab_trapOID_str[80];
static char bgpBackTrans_trapOID_str[80];

/*
 * varbinds
 */

#define BGP_TRAP_VBCOUNT 2

static const OID bgp_trap_varbinds[BGP_TRAP_VBCOUNT] = {
    {LNbgpPeerLastError, (ulong *)IDbgpPeerLastError},
    {LNbgpPeerState, (ulong *)IDbgpPeerState} };


/*---------------------------------------------------------------------
 * Retrieve data from the bgp family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
bgp_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             arg;
    void           *dp;
    bgp_t          *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_bgp_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_bgpVersion
      case I_bgpVersion:
	dp = MakeOctetString(data->bgpVersion->octet_ptr,
			     data->bgpVersion->length); 
	break;
#endif				       /* I_bgpVersion */

#ifdef I_bgpLocalAs
      case I_bgpLocalAs:
	dp = &data->bgpLocalAs;
	break;
#endif				       /* I_bgpLocalAs */

#ifdef I_bgpIdentifier
      case I_bgpIdentifier:
	dp = MakeOctetString((unsigned char *) &data->bgpIdentifier, 4L);
	break;
#endif				       /* I_bgpIdentifier */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bgpPeerEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
bgpPeerEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    bgpPeerEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    unsigned long   bgpPeerRemoteAddr;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 4
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 4) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 0 + object->oid.length,
		  &bgpPeerRemoteAddr, searchType, &carry)) < 0) { 
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_bgpPeerEntry_get(serialNum, contextInfo, arg, searchType, bgpPeerRemoteAddr)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 4;
	inst.oid_ptr[0] = (data->bgpPeerRemoteAddr >> 24) & 0xff;
	inst.oid_ptr[1] = (data->bgpPeerRemoteAddr >> 16) & 0xff;
	inst.oid_ptr[2] = (data->bgpPeerRemoteAddr >> 8) & 0xff;
	inst.oid_ptr[3] = (data->bgpPeerRemoteAddr >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_bgpPeerIdentifier
      case I_bgpPeerIdentifier:
	dp = MakeOctetString((unsigned char *) &data->bgpPeerIdentifier, 4L);
	break;
#endif				       /* I_bgpPeerIdentifier */

#ifdef I_bgpPeerState
      case I_bgpPeerState:
	dp = &data->bgpPeerState;
	break;
#endif				       /* I_bgpPeerState */

#ifdef I_bgpPeerAdminStatus
      case I_bgpPeerAdminStatus:
	dp = &data->bgpPeerAdminStatus;
	break;
#endif				       /* I_bgpPeerAdminStatus */

#ifdef I_bgpPeerNegotiatedVersion
      case I_bgpPeerNegotiatedVersion:
	dp = &data->bgpPeerNegotiatedVersion;
	break;
#endif				       /* I_bgpPeerNegotiatedVersion */

#ifdef I_bgpPeerLocalAddr
      case I_bgpPeerLocalAddr:
	dp = MakeOctetString((unsigned char *) &data->bgpPeerLocalAddr, 4L);
	break;
#endif				       /* I_bgpPeerLocalAddr */

#ifdef I_bgpPeerLocalPort
      case I_bgpPeerLocalPort:
	dp = &data->bgpPeerLocalPort;
	break;
#endif				       /* I_bgpPeerLocalPort */

#ifdef I_bgpPeerRemoteAddr
      case I_bgpPeerRemoteAddr:
	dp = MakeOctetString((unsigned char *) &data->bgpPeerRemoteAddr, 4L);
	break;
#endif				       /* I_bgpPeerRemoteAddr */

#ifdef I_bgpPeerRemotePort
      case I_bgpPeerRemotePort:
	dp = &data->bgpPeerRemotePort;
	break;
#endif				       /* I_bgpPeerRemotePort */

#ifdef I_bgpPeerRemoteAs
      case I_bgpPeerRemoteAs:
	dp = &data->bgpPeerRemoteAs;
	break;
#endif				       /* I_bgpPeerRemoteAs */

#ifdef I_bgpPeerInUpdates
      case I_bgpPeerInUpdates:
	dp = &data->bgpPeerInUpdates;
	break;
#endif				       /* I_bgpPeerInUpdates */

#ifdef I_bgpPeerOutUpdates
      case I_bgpPeerOutUpdates:
	dp = &data->bgpPeerOutUpdates;
	break;
#endif				       /* I_bgpPeerOutUpdates */

#ifdef I_bgpPeerInTotalMessages
      case I_bgpPeerInTotalMessages:
	dp = &data->bgpPeerInTotalMessages;
	break;
#endif				       /* I_bgpPeerInTotalMessages */

#ifdef I_bgpPeerOutTotalMessages
      case I_bgpPeerOutTotalMessages:
	dp = &data->bgpPeerOutTotalMessages;
	break;
#endif				       /* I_bgpPeerOutTotalMessages */

#ifdef I_bgpPeerLastError
      case I_bgpPeerLastError:
	dp = MakeOctetString(data->bgpPeerLastError->octet_ptr,
			     data->bgpPeerLastError->length);
	break;
#endif				       /* I_bgpPeerLastError */

#ifdef I_bgpPeerFsmEstablishedTransitions
      case I_bgpPeerFsmEstablishedTransitions:
	dp = &data->bgpPeerFsmEstablishedTransitions;
	break;
#endif				       /* I_bgpPeerFsmEstablishedTransitions */

#ifdef I_bgpPeerFsmEstablishedTime
      case I_bgpPeerFsmEstablishedTime:
	dp = &data->bgpPeerFsmEstablishedTime;
	break;
#endif				       /* I_bgpPeerFsmEstablishedTime */

#ifdef I_bgpPeerConnectRetryInterval
      case I_bgpPeerConnectRetryInterval:
	dp = &data->bgpPeerConnectRetryInterval;
	break;
#endif				       /* I_bgpPeerConnectRetryInterval */

#ifdef I_bgpPeerHoldTime
      case I_bgpPeerHoldTime:
	dp = &data->bgpPeerHoldTime;
	break;
#endif				       /* I_bgpPeerHoldTime */

#ifdef I_bgpPeerKeepAlive
      case I_bgpPeerKeepAlive:
	dp = &data->bgpPeerKeepAlive;
	break;
#endif				       /* I_bgpPeerKeepAlive */

#ifdef I_bgpPeerHoldTimeConfigured
      case I_bgpPeerHoldTimeConfigured:
	dp = &data->bgpPeerHoldTimeConfigured;
	break;
#endif				       /* I_bgpPeerHoldTimeConfigured */

#ifdef I_bgpPeerKeepAliveConfigured
      case I_bgpPeerKeepAliveConfigured:
	dp = &data->bgpPeerKeepAliveConfigured;
	break;
#endif				       /* I_bgpPeerKeepAliveConfigured */

#ifdef I_bgpPeerMinASOriginationInterval
      case I_bgpPeerMinASOriginationInterval:
	dp = &data->bgpPeerMinASOriginationInterval;
	break;
#endif				       /* I_bgpPeerMinASOriginationInterval */

#ifdef I_bgpPeerMinRouteAdvertisementInterval
      case I_bgpPeerMinRouteAdvertisementInterval:
	dp = &data->bgpPeerMinRouteAdvertisementInterval;
	break;
#endif				       /* I_bgpPeerMinRouteAdvertisementInterv
				        * al */

#ifdef I_bgpPeerInUpdateElapsedTime
      case I_bgpPeerInUpdateElapsedTime:
	dp = &data->bgpPeerInUpdateElapsedTime;
	break;
#endif				       /* I_bgpPeerInUpdateElapsedTime */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS
#ifdef NOTYET

/*----------------------------------------------------------------------
 * Free the bgpPeerEntry data object.
 *---------------------------------------------------------------------*/
static void
bgpPeerEntry_free(
    bgpPeerEntry_t *data
    )
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after bgpPeerEntry set/undo
 *---------------------------------------------------------------------*/
static int
bgpPeerEntry_cleanup(
    doList_t       *trash
    )
{
    bgpPeerEntry_free(trash->data);
#ifdef SR_SNMPv2
    bgpPeerEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Undo a previous set of the bgpPeerEntry family.
 *---------------------------------------------------------------------*/
#ifdef SR_SNMPv2
static int
bgpPeerEntry_undo(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo
    )
{
    return UNDO_FAILED_ERROR;
}

#endif	/* SR_SNMPv2 */
#endif
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int
bgpPeerEntry_test (
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
#ifdef NOTYET
    int             instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    VarBind        *vb = (VarBind *) NULL;
    int             found;
    int             carry = 0;
    bgpPeerEntry_t *bgpPeerEntry;
    unsigned long   bgpPeerRemoteAddr;

    /*
     * Validate the object instance
     * 
     */

    if ((InstToIP(incoming, 0 + object->oid.length, &bgpPeerRemoteAddr, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    bgpPeerEntry = k_bgpPeerEntry_get(-1, contextInfo, object->nominator, EXACT, bgpPeerRemoteAddr);

#ifndef bgpPeerEntry_READ_CREATE

    if (bgpPeerEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* bgpPeerEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == bgpPeerEntry_set) &&
	    (((bgpPeerEntry_t *) (dp->data)) != NULL) &&
	    (((bgpPeerEntry_t *) (dp->data))->bgpPeerRemoteAddr == bgpPeerRemoteAddr)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(bgpPeerEntry_t))) == NULL) {
	    DPRINTF((0, "snmpd: Cannot allocate memory\n"));
	    return (GEN_ERROR);
	}
	memset(dp->data, 0, sizeof(bgpPeerEntry_t));

	dp->setMethod = bgpPeerEntry_set;
	dp->cleanupMethod = bgpPeerEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = bgpPeerEntry_undo;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_bgpPeerRemoteAddr, ((bgpPeerEntry_t *) (dp->data))->valid);
	((bgpPeerEntry_t *) (dp->data))->bgpPeerRemoteAddr = bgpPeerRemoteAddr;

#ifdef bgpPeerEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (bgpPeerEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, bgpPeerEntry, sizeof(bgpPeerEntry_t));

	    SET_ALL_VALID(((bgpPeerEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* bgpPeerEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_bgpPeerAdminStatus
      case I_bgpPeerAdminStatus:

	SET_VALID(I_bgpPeerAdminStatus, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerAdminStatus = value->sl_value;
	break;
#endif				       /* I_bgpPeerAdminStatus */

#ifdef I_bgpPeerConnectRetryInterval
      case I_bgpPeerConnectRetryInterval:

	SET_VALID(I_bgpPeerConnectRetryInterval, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerConnectRetryInterval = value->sl_value;
	break;
#endif				       /* I_bgpPeerConnectRetryInterval */

#ifdef I_bgpPeerHoldTimeConfigured
      case I_bgpPeerHoldTimeConfigured:

	SET_VALID(I_bgpPeerHoldTimeConfigured, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerHoldTimeConfigured = value->sl_value;
	break;
#endif				       /* I_bgpPeerHoldTimeConfigured */

#ifdef I_bgpPeerKeepAliveConfigured
      case I_bgpPeerKeepAliveConfigured:

	SET_VALID(I_bgpPeerKeepAliveConfigured, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerKeepAliveConfigured = value->sl_value;
	break;
#endif				       /* I_bgpPeerKeepAliveConfigured */

#ifdef I_bgpPeerMinASOriginationInterval
      case I_bgpPeerMinASOriginationInterval:

	SET_VALID(I_bgpPeerMinASOriginationInterval, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerMinASOriginationInterval = value->sl_value;
	break;
#endif				       /* I_bgpPeerMinASOriginationInterval */

#ifdef I_bgpPeerMinRouteAdvertisementInterval
      case I_bgpPeerMinRouteAdvertisementInterval:

	SET_VALID(I_bgpPeerMinRouteAdvertisementInterval, ((bgpPeerEntry_t *) (dp->data))->valid);

	((bgpPeerEntry_t *) (dp->data))->bgpPeerMinRouteAdvertisementInterval = value->sl_value;
	break;
#endif				       /* I_bgpPeerMinRouteAdvertisementInterv
				        * al */

      default:
	DPRINTF((0, "snmpd: Internal error (invalid nominator in bgpPeerEntry)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;
#endif
    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
bgpPeerEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_bgpPeerEntry_set((bgpPeerEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the bgpPathAttrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
bgpPathAttrEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    bgpPathAttrEntry_t *data = NULL;
    unsigned long   buffer[8];
    OID             inst;
    int             carry;
    unsigned long   bgpPathAttrDestNetwork;
    unsigned long   bgpPathAttrPeer;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 8
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 8) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 4 + object->oid.length, &bgpPathAttrPeer, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &bgpPathAttrDestNetwork, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_bgpPathAttrEntry_get(serialNum, contextInfo, arg, searchType, bgpPathAttrDestNetwork, bgpPathAttrPeer)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 8;
	inst.oid_ptr[0] = (data->bgpPathAttrDestNetwork >> 24) & 0xff;
	inst.oid_ptr[1] = (data->bgpPathAttrDestNetwork >> 16) & 0xff;
	inst.oid_ptr[2] = (data->bgpPathAttrDestNetwork >> 8) & 0xff;
	inst.oid_ptr[3] = (data->bgpPathAttrDestNetwork >> 0) & 0xff;
	inst.oid_ptr[4] = (data->bgpPathAttrPeer >> 24) & 0xff;
	inst.oid_ptr[5] = (data->bgpPathAttrPeer >> 16) & 0xff;
	inst.oid_ptr[6] = (data->bgpPathAttrPeer >> 8) & 0xff;
	inst.oid_ptr[7] = (data->bgpPathAttrPeer >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_bgpPathAttrPeer
      case I_bgpPathAttrPeer:
	dp = MakeOctetString((unsigned char *) &data->bgpPathAttrPeer, 4L);
	break;
#endif				       /* I_bgpPathAttrPeer */

#ifdef I_bgpPathAttrDestNetwork
      case I_bgpPathAttrDestNetwork:
	dp = MakeOctetString((unsigned char *) &data->bgpPathAttrDestNetwork, 4L);
	break;
#endif				       /* I_bgpPathAttrDestNetwork */

#ifdef I_bgpPathAttrOrigin
      case I_bgpPathAttrOrigin:
	dp = &data->bgpPathAttrOrigin;
	break;
#endif				       /* I_bgpPathAttrOrigin */

#ifdef I_bgpPathAttrASPath
      case I_bgpPathAttrASPath:
	dp = MakeOctetString(data->bgpPathAttrASPath->octet_ptr, data->bgpPathAttrASPath->length);
	break;
#endif				       /* I_bgpPathAttrASPath */

#ifdef I_bgpPathAttrNextHop
      case I_bgpPathAttrNextHop:
	dp = MakeOctetString((unsigned char *) &data->bgpPathAttrNextHop, 4L);
	break;
#endif				       /* I_bgpPathAttrNextHop */

#ifdef I_bgpPathAttrInterASMetric
      case I_bgpPathAttrInterASMetric:
	dp = &data->bgpPathAttrInterASMetric;
	break;
#endif				       /* I_bgpPathAttrInterASMetric */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the bgp4PathAttrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
bgp4PathAttrEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    bgp4PathAttrEntry_t *data = NULL;
    unsigned long   buffer[9];
    OID             inst;
    int             carry;
    unsigned long   bgp4PathAttrIpAddrPrefix;
    long            bgp4PathAttrIpAddrPrefixLen;
    unsigned long   bgp4PathAttrPeer;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 9
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 9) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToIP(incoming, 5 + object->oid.length, &bgp4PathAttrPeer, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 4 + object->oid.length, &bgp4PathAttrIpAddrPrefixLen, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToIP(incoming, 0 + object->oid.length, &bgp4PathAttrIpAddrPrefix, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_bgp4PathAttrEntry_get(serialNum, contextInfo, arg, searchType, bgp4PathAttrIpAddrPrefix, bgp4PathAttrIpAddrPrefixLen, bgp4PathAttrPeer)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 9;
	inst.oid_ptr[0] = (data->bgp4PathAttrIpAddrPrefix >> 24) & 0xff;
	inst.oid_ptr[1] = (data->bgp4PathAttrIpAddrPrefix >> 16) & 0xff;
	inst.oid_ptr[2] = (data->bgp4PathAttrIpAddrPrefix >> 8) & 0xff;
	inst.oid_ptr[3] = (data->bgp4PathAttrIpAddrPrefix >> 0) & 0xff;
	inst.oid_ptr[4] = data->bgp4PathAttrIpAddrPrefixLen;
	inst.oid_ptr[5] = (data->bgp4PathAttrPeer >> 24) & 0xff;
	inst.oid_ptr[6] = (data->bgp4PathAttrPeer >> 16) & 0xff;
	inst.oid_ptr[7] = (data->bgp4PathAttrPeer >> 8) & 0xff;
	inst.oid_ptr[8] = (data->bgp4PathAttrPeer >> 0) & 0xff;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_bgp4PathAttrPeer
      case I_bgp4PathAttrPeer:
	dp = MakeOctetString((unsigned char *) &data->bgp4PathAttrPeer, 4L);
	break;
#endif				       /* I_bgp4PathAttrPeer */

#ifdef I_bgp4PathAttrIpAddrPrefixLen
      case I_bgp4PathAttrIpAddrPrefixLen:
	dp = &data->bgp4PathAttrIpAddrPrefixLen;
	break;
#endif				       /* I_bgp4PathAttrIpAddrPrefixLen */

#ifdef I_bgp4PathAttrIpAddrPrefix
      case I_bgp4PathAttrIpAddrPrefix:
	dp = MakeOctetString((unsigned char *) &data->bgp4PathAttrIpAddrPrefix, 4L);
	break;
#endif				       /* I_bgp4PathAttrIpAddrPrefix */

#ifdef I_bgp4PathAttrOrigin
      case I_bgp4PathAttrOrigin:
	dp = &data->bgp4PathAttrOrigin;
	break;
#endif				       /* I_bgp4PathAttrOrigin */

#ifdef I_bgp4PathAttrASPathSegment
      case I_bgp4PathAttrASPathSegment:
	dp = MakeOctetString(data->bgp4PathAttrASPathSegment->octet_ptr, data->bgp4PathAttrASPathSegment->length);
	break;
#endif				       /* I_bgp4PathAttrASPathSegment */

#ifdef I_bgp4PathAttrNextHop
      case I_bgp4PathAttrNextHop:
	dp = MakeOctetString((unsigned char *) &data->bgp4PathAttrNextHop, 4L);
	break;
#endif				       /* I_bgp4PathAttrNextHop */

#ifdef I_bgp4PathAttrMultiExitDisc
      case I_bgp4PathAttrMultiExitDisc:
	dp = &data->bgp4PathAttrMultiExitDisc;
	break;
#endif				       /* I_bgp4PathAttrMultiExitDisc */

#ifdef I_bgp4PathAttrLocalPref
      case I_bgp4PathAttrLocalPref:
	dp = &data->bgp4PathAttrLocalPref;
	break;
#endif				       /* I_bgp4PathAttrLocalPref */

#ifdef I_bgp4PathAttrAtomicAggregate
      case I_bgp4PathAttrAtomicAggregate:
	dp = &data->bgp4PathAttrAtomicAggregate;
	break;
#endif				       /* I_bgp4PathAttrAtomicAggregate */

#ifdef I_bgp4PathAttrAggregatorAS
      case I_bgp4PathAttrAggregatorAS:
	dp = &data->bgp4PathAttrAggregatorAS;
	break;
#endif				       /* I_bgp4PathAttrAggregatorAS */

#ifdef I_bgp4PathAttrAggregatorAddr
      case I_bgp4PathAttrAggregatorAddr:
	dp = MakeOctetString((unsigned char *) &data->bgp4PathAttrAggregatorAddr, 4L);
	break;
#endif				       /* I_bgp4PathAttrAggregatorAddr */

#ifdef I_bgp4PathAttrCalcLocalPref
      case I_bgp4PathAttrCalcLocalPref:
	dp = &data->bgp4PathAttrCalcLocalPref;
	break;
#endif				       /* I_bgp4PathAttrCalcLocalPref */

#ifdef I_bgp4PathAttrBest
      case I_bgp4PathAttrBest:
	dp = &data->bgp4PathAttrBest;
	break;
#endif				       /* I_bgp4PathAttrBest */

#ifdef I_bgp4PathAttrUnknown
      case I_bgp4PathAttrUnknown:
	dp = (void *) (CloneOctetString(data->bgp4PathAttrUnknown));
	break;
#endif				       /* I_bgp4PathAttrUnknown */
  
        default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}




bgp_t *k_bgp_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static bgp_t    bgpData;

    bgpData.bgpVersion = &_bgpVersion;
    bgpData.bgpVersion->octet_ptr = &bgpversion_octet_str[0];
    bgpData.bgpVersion->length = sizeof(bgpversion_octet_str);
    bgpData.bgpLocalAs = (pdb_BGP && pdb_BGP->running) ? pdb_BGP->asystem : 0;
    bgpData.bgpIdentifier = bgp_rtr_id();

    return (&bgpData);
}

bgpPeerEntry_t *
k_bgpPeerEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   bgpPeerRemoteAddr)
{
    static bgpPeerEntry_t bgpPeerEntryData;

    neighbortype *bgp;

    if (!pdb_BGP)
        return(NULL);

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	/* exact match is always a best match */
	if (bgp->address == bgpPeerRemoteAddr) {
	    break;
	}

	/* forget this entry if get-request */
	if (searchType == EXACT) {
	    /*
	     * have we exceed the spot where we'd find the guy?
	     */
	    if (bgp->address > bgpPeerRemoteAddr) {
		return(NULL);
	    }
	    continue;
	}

	/*
	 * see if this entry has a "large" enough address
	 * if so, then this is the "next" guy because the list is sorted
	 */
	if (bgp->address > bgpPeerRemoteAddr) {
	    break;
        }
    }

    if (bgp == NULL)
        return(NULL);

    bgpPeerEntryData.bgpPeerIdentifier = bgp->bgp_router_id;
    bgpPeerEntryData.bgpPeerState = 
        bgp->bgp_state == BGPS_CLOSING ? BGPS_IDLE : bgp->bgp_state; 
    bgpPeerEntryData.bgpPeerAdminStatus = D_bgpPeerAdminStatus_stop;
    bgpPeerEntryData.bgpPeerNegotiatedVersion = bgp->protocol_version;
    bgpPeerEntryData.bgpPeerLocalAddr = 
        bgp->tcb ? bgp->tcb->localhost.ip_addr : 0;
    bgpPeerEntryData.bgpPeerLocalPort = bgp->tcb ? bgp->tcb->localport : 0;
    bgpPeerEntryData.bgpPeerRemoteAddr = bgp->address;
    bgpPeerEntryData.bgpPeerRemotePort = bgp->tcb ? bgp->tcb->foreignport : 0;
    bgpPeerEntryData.bgpPeerRemoteAs = bgp->yoursystem;
    bgpPeerEntryData.bgpPeerInUpdates = bgp->inupdates;
    bgpPeerEntryData.bgpPeerOutUpdates = bgp->outupdates;
    bgpPeerEntryData.bgpPeerInTotalMessages = bgp->inmsgs;
    bgpPeerEntryData.bgpPeerOutTotalMessages = bgp->outmsgs;
    bgpPeerEntryData.bgpPeerLastError = &_bgpPeerLastError;;
    bgpPeerEntryData.bgpPeerLastError->octet_ptr = bgp->bgp_last_error;
    bgpPeerEntryData.bgpPeerLastError->length = sizeof(bgp->bgp_last_error);
    bgpPeerEntryData.bgpPeerFsmEstablishedTransitions = bgp->cameup;
    bgpPeerEntryData.bgpPeerFsmEstablishedTime = 
        ELAPSED_TIME(bgp->started) / ONESEC;
    bgpPeerEntryData.bgpPeerConnectRetryInterval = ONEMIN/ONESEC;
    bgpPeerEntryData.bgpPeerHoldTime = bgp->bgp_holdtime / ONESEC;
    bgpPeerEntryData.bgpPeerKeepAlive = bgp->bgp_probetime / ONESEC;
    bgpPeerEntryData.bgpPeerHoldTimeConfigured = pdb_BGP->bgp_holdtime;
    bgpPeerEntryData.bgpPeerKeepAliveConfigured = pdb_BGP->bgp_keeptime;
    bgpPeerEntryData.bgpPeerMinASOriginationInterval = 
        bgp->bgp_minadverttime / ONESEC;
    bgpPeerEntryData.bgpPeerMinRouteAdvertisementInterval = 
        bgp->bgp_minadverttime / ONESEC;
    bgpPeerEntryData.bgpPeerInUpdateElapsedTime =
	TIMER_RUNNING(bgp->inupdatetime) ?
	    ELAPSED_TIME(bgp->inupdatetime) / ONESEC : 0;

    /* disappeared from draft bgpPeerType */
    /* It was implemented as bgp->yoursystem != pdb_BGP->asystem ? 1 : 2 */
    return (&bgpPeerEntryData);
}

int
k_bgpPeerEntry_set (
    bgpPeerEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}

bgpPathAttrEntry_t *
k_bgpPathAttrEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   bgpPathAttrDestNetwork,
    unsigned long   bgpPathAttrPeer)
{
#ifdef NOT_YET
    static bgpPathAttrEntry_t bgpPathAttrEntryData;

    bgpPathAttrEntryData.bgpPathAttrPeer =;
    bgpPathAttrEntryData.bgpPathAttrDestNetwork =;
    bgpPathAttrEntryData.bgpPathAttrOrigin =;
    bgpPathAttrEntryData.bgpPathAttrASPath =;
    bgpPathAttrEntryData.bgpPathAttrNextHop =;
    bgpPathAttrEntryData.bgpPathAttrInterASMetric =;

    return (&bgpPathAttrEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}

bgp4PathAttrEntry_t *
k_bgp4PathAttrEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    unsigned long   bgp4PathAttrIpAddrPrefix,
    long            bgp4PathAttrIpAddrPrefixLen,
    unsigned long   bgp4PathAttrPeer)
{
/*
 * this isn't the most optimal way to do this, but it most easily fits in
 * the framework of the SR mib compiler.  I'll probably optimize this at
 * a later date  --jeff
 *
 * Above comment stolen from ifmib2, along with the way the code is
 * implemented.  I'll need to steal the optimization as well.  -Nick
 */
    static int lastserial = MAXINT;
    static long lastIpAddrPrefix = MAXLONG;
    static long lastPrefixLen = MAXLONG;
    static long lastPathAttrPeer = MAXLONG;
      static bgp4PathAttrEntry_t bgp4PathAttrEntryData;
  
    bnettype *bnet;
    bpathtype *bpath;
    int path_index;

    /*
     * See if we got a cache hit.
     */
    
    if ((serialNum != -1) &&
	(serialNum == lastserial) && 
	(lastIpAddrPrefix == bgp4PathAttrIpAddrPrefix) && 
	(lastPrefixLen == bgp4PathAttrIpAddrPrefixLen) && 
	(lastPathAttrPeer == bgp4PathAttrPeer)) {
	/* hit cache */
	return(&bgp4PathAttrEntryData);
    }

    /*
     * missed cache, store info for next cache hit 
     */
    lastserial = serialNum;
    lastIpAddrPrefix = bgp4PathAttrIpAddrPrefix;
    lastPrefixLen = bgp4PathAttrIpAddrPrefixLen;
    lastPathAttrPeer = bgp4PathAttrPeer;

    bnet = bgp4_find_pathattr(bgp4PathAttrIpAddrPrefix, 
			      bgp4PathAttrIpAddrPrefixLen, 
			      bgp4PathAttrPeer,
			      &path_index);

    if (bnet == NULL) {
	lastserial = MAXINT;		/* Avoid cache hit on null entry */
	return(NULL);
    }

    bpath = &bnet->paths[path_index];

    bgp4PathAttrEntryData.bgp4PathAttrPeer = bpath->neighbor;
    bgp4PathAttrEntryData.bgp4PathAttrIpAddrPrefixLen =
	ip_bitsinmask(bnet->mask);
    bgp4PathAttrEntryData.bgp4PathAttrIpAddrPrefix = bnet->network;
    switch (bpath->attr->origin) {
      case ORIGIN_IGP:
	bgp4PathAttrEntryData.bgp4PathAttrOrigin = D_bgpPathAttrOrigin_igp;
	break;
      case ORIGIN_EGP:
	bgp4PathAttrEntryData.bgp4PathAttrOrigin = D_bgpPathAttrOrigin_egp;
	break;
      case ORIGIN_INCOMPLETE:
      default:
	bgp4PathAttrEntryData.bgp4PathAttrOrigin = 
	    D_bgpPathAttrOrigin_incomplete;
	break;
    }
    if (bpath->attr->as_path[0] == 0) {
	bgp4PathAttrASPathSegment.octet_ptr = null_aspath;
	bgp4PathAttrASPathSegment.length = 0;
    } else {
	bgp4PathAttrASPathSegment.length = bpath->attr->aslength;
	bcopy(bpath->attr->as_path, &bgp4_aspath_buf,
	      bgp4PathAttrASPathSegment.length); 
	bgp4PathAttrASPathSegment.octet_ptr = bgp4_aspath_buf;
    }	
    bgp4PathAttrEntryData.bgp4PathAttrASPathSegment =
	&bgp4PathAttrASPathSegment;

    bgp4PathAttrEntryData.bgp4PathAttrNextHop = bpath->gateway;

    if (!(bpath->flags & BFLAG_METRIC)) {
	bgp4PathAttrEntryData.bgp4PathAttrMultiExitDisc = -1;
    } else {
	if (bpath->attr->metric > MAXINT) 
	    bgp4PathAttrEntryData.bgp4PathAttrMultiExitDisc = MAXINT;
	else
	    bgp4PathAttrEntryData.bgp4PathAttrMultiExitDisc =
		bpath->attr->metric;	    
    }
    bgp4PathAttrEntryData.bgp4PathAttrLocalPref = -1;
    if (bpath->flags & BFLAG_ATOMICAGG) {
	bgp4PathAttrEntryData.bgp4PathAttrAtomicAggregate = 
	   D_bgp4PathAttrAtomicAggregate_lessSpecificRouteSelected; 
    } else { 
	bgp4PathAttrEntryData.bgp4PathAttrAtomicAggregate = 
	    D_bgp4PathAttrAtomicAggregate_lessSpecificRrouteNotSelected;
    }

    bgp4PathAttrEntryData.bgp4PathAttrAggregatorAS =
	bpath->attr->aggregator.as;
    bgp4PathAttrEntryData.bgp4PathAttrAggregatorAddr =
	bpath->attr->aggregator.ip;
    if (bpath->flags && BFLAG_LOCALPREF) {
	bgp4PathAttrEntryData.bgp4PathAttrCalcLocalPref =
	    bpath->attr->localpref; 
    } else {
	bgp4PathAttrEntryData.bgp4PathAttrCalcLocalPref =
	    bgp_default_localpref;  
    }
    if (bnet->best == path_index) {
	bgp4PathAttrEntryData.bgp4PathAttrBest = D_bgp4PathAttrBest_true;
    } else {
	bgp4PathAttrEntryData.bgp4PathAttrBest = D_bgp4PathAttrBest_false;
    }

    if (bpath->attr->translen == 0) {
	bgp4_unknown_buf[0] = 0;
	bgp4PathAttrUnknown.length = 0;
    } else {
	bgp4PathAttrUnknown.length = bpath->attr->translen;
	bcopy(bpath->attr->transptr, &bgp4_unknown_buf,
	      bgp4PathAttrUnknown.length);
    }	
    bgp4PathAttrEntryData.bgp4PathAttrUnknown = &bgp4PathAttrUnknown;
  
      return (&bgp4PathAttrEntryData);
}


/***************************************************************************
 * BGP trap support (lots of ugly stuff follows)                           *
 ***************************************************************************
 */

/*

                bgpEstablished NOTIFICATION-TYPE
                    OBJECTS { bgpPeerLastError,
                              bgpPeerState      }
                    STATUS  current
                    DESCRIPTION
                            "The BGP Established event is generated when
                            the BGP FSM enters the ESTABLISHED state."
                    ::= { bgpTraps 1 }

                bgpBackwardTransition NOTIFICATION-TYPE
                    OBJECTS { bgpPeerLastError,
                              bgpPeerState      }
                    STATUS  current
                    DESCRIPTION
                            "The BGPBackwardTransition Event is generated
                            when the BGP FSM moves from a higher numbered
                            state to a lower numbered state."
                    ::= { bgpTraps 2 }
 */

/*
 * First the ugly parser stuff
 * starting with action & nvgen routines
 */

/*
 * "snmp-server host <host> <community> bgp" nvgen routine
 */

static void bgp_snmp_host_nvgen (ulong flags)
{
    nv_add(flags & (1 << TRAP_BGP), " bgp");
}

/*
 * "snmp-server enable traps bgp" action routine
 */

static void bgp_trap_cfg_set (boolean enable, uint subsys, uint traps)
{
    if ((subsys & (1 << TRAP_BGP)))
        bgp_traps_enabled = enable;
}

/*
 * "snmp-server enable traps bgp" nvgen routine
 */

static void bgp_trap_cfg_nvwrt (parseinfo *csb)
{
    nv_write(bgp_traps_enabled, "%s traps bgp", csb->nv_command);
}

/*
 * parse chain addition for "snmp-server host <host> <community> bgp" 
 */
LINK_EXIT (cfg_snmp_host_bgptrap_exit, no_alt);
KEYWORD_OR (cfg_snmp_host_bgptrap, cfg_snmp_host_bgptrap_exit, NONE,
	    OBJ(int,1), (1<<TRAP_BGP),
	    "bgp", "Allow BGP state change traps", PRIV_CONF);
LINK_POINT(cfg_snmp_host_bgptrap_entry, cfg_snmp_host_bgptrap);

/*
 * parse chain addition for "snmp-server enable traps bgp"
 */
LINK_EXIT (cfg_snmp_enable_bgptrap_exit, no_alt);
KEYWORD_OR (cfg_snmp_enable_bgptrap, cfg_snmp_enable_bgptrap_exit, NONE,
	    OBJ(int,1), (1<<TRAP_BGP),
	    "bgp", "Enable BGP state change traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_bgptrap_entry, cfg_snmp_enable_bgptrap);

/*
 * parse chain registration array for BGP traps
 */
static parser_extension_request bgptrap_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_bgptrap_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_bgptrap_exit) },
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD,
	  &pname(cfg_snmp_enable_bgptrap_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT,
        (dynamic_transition *) &pname(cfg_snmp_enable_bgptrap_exit) },
    { PARSE_LIST_END, NULL }
};


/*
 * bgp_statechange_trap
 * Actually generate the traps.
 */
static void bgp_statechange_trap (int trap_num, ipaddrtype neighbor,
				  uchar last_error[2], int newstate)
{
    int         vbIndex;
    OID        *vbList[BGP_TRAP_VBCOUNT + 1];
    OID         instanceOID;
    ulong       instance[4];

    /* 
     * Don't play if traps not turned on.
     */
    if (!bgp_traps_enabled)
	return;

    /*
     * Instance for trap varbind objects.
     * All varbinds in the bgp traps are table objects, indexed by
     * bgpPeerRemoteAddr, passed in as neighbor.  Split it into 
     * "dotted" form and pass it along.
     */
    instance[0] = (ulong) ((neighbor >> 24) & 0xff);
    instance[1] = (ulong) ((neighbor >> 16) & 0xff);
    instance[2] = (ulong) ((neighbor >> 8) & 0xff);
    instance[3] = (ulong) ((neighbor >> 0) & 0xff);

    instanceOID.oid_ptr = instance;
    instanceOID.length = 4;

    /*
     * Create the trap's varbind list.
     */
    for (vbIndex = 0; vbIndex < BGP_TRAP_VBCOUNT; vbIndex++) {
        vbList[vbIndex] =
	    CatOID((OID *) &bgp_trap_varbinds[vbIndex],
			      &instanceOID);
    }
    vbList[vbIndex] = NULL;

    /*
     * Generate the trap.
     * If version 1 traps are needed, snmp_trap() will use the first
     * 4 parameters to generate a version 1 trap.  If version 2 traps
     * are needed, vbList and bgp<xxxx>_trapOID_str will be used to
     * generate the trap.
     */
    if (trap_num == BGP_TRAP_ESTAB) {
	snmp_trap(ENTERPRISE_TRAP, BGP_TRAP_ESTAB, vbList,
		  (OID *)&enterpriseOID, (char *)bgpEstab_trapOID_str);
    }
    else {
	snmp_trap(ENTERPRISE_TRAP, BGP_TRAP_BACKTRANS,
		  vbList, (OID *)&enterpriseOID,
		  (char *)bgpBackTrans_trapOID_str);
    }

    /*
     * free data allocated by the CatOID service
     */
    for (vbIndex = 0; vbIndex < BGP_TRAP_VBCOUNT; vbIndex++) {
        FreeOID(vbList[vbIndex]);
    }
}


void init_bgpmib (subsystype *subsys)
{
    int i;
    for (i = BGP_MIN_VERSION; i <= BGP_MAX_VERSION; i++)
        bgpversion_octet_str[0] |= 1<<(i-1);

    bgp_traps_enabled = FALSE;

    MakeDotFromOID((OID *)&bgpEstab_trapOID, bgpEstab_trapOID_str);
    MakeDotFromOID((OID *)&bgpBackTrans_trapOID, bgpBackTrans_trapOID_str);

    register_snmp_trap(TRAP_BGP, bgpEstab_trapOID_str);
    register_snmp_trap(TRAP_BGP, bgpBackTrans_trapOID_str);

    reg_add_ip_bgp_trap(bgp_statechange_trap, "bgp_statechange_trap");

    reg_add_snmp_host_nvgen(bgp_snmp_host_nvgen,
			    "bgp_snmp_host_nvgen");
    reg_add_Trap_cfg_set(bgp_trap_cfg_set, "bgp_trap_cfg_set");
    reg_add_Trap_cfg_nvwrt(bgp_trap_cfg_nvwrt, "bgp_trap_cfg_nvwrt");
    parser_add_command_list(bgptrap_chain_init_table, "bgp");

    bgp4PathAttrASPathSegment.octet_ptr = NULL;
    bgp4PathAttrUnknown.octet_ptr = bgp4_unknown_buf;

    load_mib(bgpmib_OidList, bgpmib_OidListNum);
    load_oid(bgpmib_oid_table);
}

/*
 * bgpmib subsystem header
 */

#define MAJVERSION_bgpmib 1
#define MINVERSION_bgpmib 0
#define EDITVERSION_bgpmib 0

SUBSYS_HEADER(bgpmib,
              MAJVERSION_bgpmib,
              MINVERSION_bgpmib,
              EDITVERSION_bgpmib,
              init_bgpmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
