/* $Id: sr_trmib.c,v 3.6.10.4 1996/07/01 18:46:24 hampton Exp $
 * $Source: /release/112/cvs/Xsys/snmp/sr_trmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_trmib.c,v $
 * Revision 3.6.10.4  1996/07/01  18:46:24  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  10:02:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/05/21  06:38:53  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.10.1  1996/03/18  21:49:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.4  1996/03/16  07:30:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.3  1996/03/14  01:24:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.2.2  1996/03/07  12:34:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/01/24  22:48:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/02/23  17:38:14  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/18  15:52:30  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/14  08:27:43  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.3  1995/11/17  19:01:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/17  07:34:10  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.3  1995/06/28  10:22:02  smackie
 * Subsystem header cleanup. Round up the last few stragglers and rope 'em
 * in with the rest of the herd. (CSCdi23568)
 *
 * Revision 2.2  1995/06/21  03:14:24  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  22:51:13  hampton
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
#include "../snmp/snmp_api.h"
#include "sr_trmib.h"
#include "sr_trmib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"

#include "interface_private.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/network.h"
#include "../util/itemlist.h"

static void   dot5_ring_status(long *, hwidbtype *);
static int    mib_decode_ring_open_status(uint);
static boolean idb_tester(hwidbtype *, long* index2);

/* below must be global so value doesnt die with stack */
uchar func_address[IEEEBYTES] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00};


static void init_trmib (subsystype *subsys)
{
    load_mib(trmib_OidList, trmib_OidListNum);
    load_oid(trmib_oid_table);
}

/*---------------------------------------------------------------------
 * Retrieve data from the dot5Entry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *dot5Entry_get(OID *incoming, ObjectInfo *object, int searchType,
	ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dot5Entry_t    *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dot5IfIndex;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 1
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 1) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &dot5IfIndex, 
	searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot5Entry_get(serialNum, contextInfo, arg, 
	searchType, dot5IfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dot5IfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot5IfIndex
      case I_dot5IfIndex:
	dp = &data->dot5IfIndex;
	break;
#endif				       /* I_dot5IfIndex */

#ifdef I_dot5Commands
      case I_dot5Commands:
	dp = &data->dot5Commands;
	break;
#endif				       /* I_dot5Commands */

#ifdef I_dot5RingStatus
      case I_dot5RingStatus:
	dp = &data->dot5RingStatus;
	break;
#endif				       /* I_dot5RingStatus */

#ifdef I_dot5RingState
      case I_dot5RingState:
	dp = &data->dot5RingState;
	break;
#endif				       /* I_dot5RingState */

#ifdef I_dot5RingOpenStatus
      case I_dot5RingOpenStatus:
	dp = &data->dot5RingOpenStatus;
	break;
#endif				       /* I_dot5RingOpenStatus */

#ifdef I_dot5RingSpeed
      case I_dot5RingSpeed:
	dp = &data->dot5RingSpeed;
	break;
#endif				       /* I_dot5RingSpeed */

#ifdef I_dot5UpStream
      case I_dot5UpStream:
	dp = MakeOctetString(data->dot5UpStream->octet_ptr, 
		data->dot5UpStream->length);
	break;
#endif				       /* I_dot5UpStream */

#ifdef I_dot5ActMonParticipate
      case I_dot5ActMonParticipate:
	dp = &data->dot5ActMonParticipate;
	break;
#endif				       /* I_dot5ActMonParticipate */

#ifdef I_dot5Functional
      case I_dot5Functional:
	dp = MakeOctetString(data->dot5Functional->octet_ptr, 
		data->dot5Functional->length);
	break;
#endif				       /* I_dot5Functional */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dot5Entry data object.
 *---------------------------------------------------------------------*/
void dot5Entry_free(dot5Entry_t *data)
{
    if (data != NULL) {
	if (data->dot5Functional != NULL) {
	    FreeOctetString(data->dot5Functional);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dot5Entry set/undo
 *---------------------------------------------------------------------*/
int dot5Entry_cleanup(doList_t *trash)
{
    dot5Entry_free(trash->data);
#ifdef SR_SNMPv2
    dot5Entry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * Undo a previous set of the dot5Entry family.
 *---------------------------------------------------------------------*/
#ifdef SR_SNMPv2
int dot5Entry_undo(doList_t *doHead, doList_t *doCur, 
	ContextInfo *contextInfo)
{
    return UNDO_FAILED_ERROR;
}

#endif	/* SR_SNMPv2 */

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
int dot5Entry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
	doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    doList_t       *dp;
    int             found;
    int             carry = 0;
    dot5Entry_t    *dot5Entry;
    long            dot5IfIndex;

    /*
     * Validate the object instance
     * 
     */

    if ((InstToInt(incoming, 0 + object->oid.length, &dot5IfIndex, 
	EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dot5Entry = k_dot5Entry_get(-1, contextInfo, object->nominator, 
	EXACT, dot5IfIndex);

    if (dot5Entry == NULL) {
	return (NO_ACCESS_ERROR);      /* dont allow new rows */
    }
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dot5Entry_set) &&
	    (((dot5Entry_t *) (dp->data)) != NULL) &&
	    (((dot5Entry_t *) (dp->data))->dot5IfIndex == dot5IfIndex)) {
	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dot5Entry_t))) == NULL) {
	    DPRINTF((0, "snmpd: Cannot allocate memory\n"));
	    return (GEN_ERROR);
	}
	memset(dp->data, 0, sizeof(dot5Entry_t));

	dp->setMethod = dot5Entry_set;
	dp->cleanupMethod = dot5Entry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = dot5Entry_undo;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dot5IfIndex, ((dot5Entry_t *) (dp->data))->valid);

	((dot5Entry_t *) (dp->data))->dot5IfIndex = dot5IfIndex;

    }
    switch (object->nominator) {

#ifdef I_dot5Commands
      case I_dot5Commands:

	if ((value->sl_value != D_dot5Commands_no_op) &&
	    (value->sl_value != D_dot5Commands_open) &&
	    (value->sl_value != D_dot5Commands_reset) &&
	    (value->sl_value != D_dot5Commands_close))
	    return (WRONG_VALUE_ERROR);

	SET_VALID(I_dot5Commands, ((dot5Entry_t *) (dp->data))->valid);

	((dot5Entry_t *) (dp->data))->dot5Commands = value->sl_value;
	break;
#endif				       /* I_dot5Commands */

#ifdef I_dot5RingSpeed
      case I_dot5RingSpeed:

	if ((value->sl_value != D_dot5RingSpeed_unknown) &&
	    (value->sl_value != D_dot5RingSpeed_oneMegabit) &&
	    (value->sl_value != D_dot5RingSpeed_fourMegabit) &&
	    (value->sl_value != D_dot5RingSpeed_sixteenMegabit))
	    return (WRONG_VALUE_ERROR);
	
	return (NOT_WRITABLE_ERROR);	/* currently unsupported */	
	
	SET_VALID(I_dot5RingSpeed, ((dot5Entry_t *) (dp->data))->valid);
	((dot5Entry_t *) (dp->data))->dot5RingSpeed = value->sl_value;
	break;
#endif				       /* I_dot5RingSpeed */

#ifdef I_dot5ActMonParticipate
      case I_dot5ActMonParticipate:

	if ((value->sl_value != D_dot5ActMonParticipate_true) &&
	    (value->sl_value != D_dot5ActMonParticipate_false))
	    return (WRONG_VALUE_ERROR);

	return (NOT_WRITABLE_ERROR);	/* currently unsupported */	

	SET_VALID(I_dot5ActMonParticipate, ((dot5Entry_t *) (dp->data))->valid);
	((dot5Entry_t *) (dp->data))->dot5ActMonParticipate = value->sl_value;
	break;
#endif				       /* I_dot5ActMonParticipate */

#ifdef I_dot5Functional
      case I_dot5Functional:

	return (NOT_WRITABLE_ERROR);	/* currently unsupported */	

	SET_VALID(I_dot5Functional, ((dot5Entry_t *) (dp->data))->valid);
	if (((dot5Entry_t *) (dp->data))->dot5Functional != NULL) {
	    FreeOctetString(((dot5Entry_t *) (dp->data))->dot5Functional);
	}
	((dot5Entry_t *) (dp->data))->dot5Functional =
	    MakeOctetString(value->os_value->octet_ptr,value->os_value->length);

	break;
#endif				       /* I_dot5Functional */

      default:
	DPRINTF((0,"snmpd: Internal error (invalid nominator in dot5Entry)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int dot5Entry_set(doList_t *doHead, doList_t *doCur, 
	ContextInfo *contextInfo)
{
    return (k_dot5Entry_set((dot5Entry_t *) (doCur->data),
			    contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dot5StatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *dot5StatsEntry_get(OID *incoming, ObjectInfo *object,
	int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dot5StatsEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dot5StatsIfIndex;

    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 1
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 1) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &dot5StatsIfIndex, 
	searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot5StatsEntry_get(serialNum, contextInfo, 
	arg, searchType, dot5StatsIfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dot5StatsIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot5StatsIfIndex
      case I_dot5StatsIfIndex:
	dp = &data->dot5StatsIfIndex;
	break;
#endif				       /* I_dot5StatsIfIndex */

#ifdef I_dot5StatsLineErrors
      case I_dot5StatsLineErrors:
	dp = &data->dot5StatsLineErrors;
	break;
#endif				       /* I_dot5StatsLineErrors */

#ifdef I_dot5StatsBurstErrors
      case I_dot5StatsBurstErrors:
	dp = &data->dot5StatsBurstErrors;
	break;
#endif				       /* I_dot5StatsBurstErrors */

#ifdef I_dot5StatsACErrors
      case I_dot5StatsACErrors:
	dp = &data->dot5StatsACErrors;
	break;
#endif				       /* I_dot5StatsACErrors */

#ifdef I_dot5StatsAbortTransErrors
      case I_dot5StatsAbortTransErrors:
	dp = &data->dot5StatsAbortTransErrors;
	break;
#endif				       /* I_dot5StatsAbortTransErrors */

#ifdef I_dot5StatsInternalErrors
      case I_dot5StatsInternalErrors:
	dp = &data->dot5StatsInternalErrors;
	break;
#endif				       /* I_dot5StatsInternalErrors */

#ifdef I_dot5StatsLostFrameErrors
      case I_dot5StatsLostFrameErrors:
	dp = &data->dot5StatsLostFrameErrors;
	break;
#endif				       /* I_dot5StatsLostFrameErrors */

#ifdef I_dot5StatsReceiveCongestions
      case I_dot5StatsReceiveCongestions:
	dp = &data->dot5StatsReceiveCongestions;
	break;
#endif				       /* I_dot5StatsReceiveCongestions */

#ifdef I_dot5StatsFrameCopiedErrors
      case I_dot5StatsFrameCopiedErrors:
	dp = &data->dot5StatsFrameCopiedErrors;
	break;
#endif				       /* I_dot5StatsFrameCopiedErrors */

#ifdef I_dot5StatsTokenErrors
      case I_dot5StatsTokenErrors:
	dp = &data->dot5StatsTokenErrors;
	break;
#endif				       /* I_dot5StatsTokenErrors */

#ifdef I_dot5StatsSoftErrors
      case I_dot5StatsSoftErrors:
	dp = &data->dot5StatsSoftErrors;
	break;
#endif				       /* I_dot5StatsSoftErrors */

#ifdef I_dot5StatsHardErrors
      case I_dot5StatsHardErrors:
	dp = &data->dot5StatsHardErrors;
	break;
#endif				       /* I_dot5StatsHardErrors */

#ifdef I_dot5StatsSignalLoss
      case I_dot5StatsSignalLoss:
	dp = &data->dot5StatsSignalLoss;
	break;
#endif				       /* I_dot5StatsSignalLoss */

#ifdef I_dot5StatsTransmitBeacons
      case I_dot5StatsTransmitBeacons:
	dp = &data->dot5StatsTransmitBeacons;
	break;
#endif				       /* I_dot5StatsTransmitBeacons */

#ifdef I_dot5StatsRecoverys
      case I_dot5StatsRecoverys:
	dp = &data->dot5StatsRecoverys;
	break;
#endif				       /* I_dot5StatsRecoverys */

#ifdef I_dot5StatsLobeWires
      case I_dot5StatsLobeWires:
	dp = &data->dot5StatsLobeWires;
	break;
#endif				       /* I_dot5StatsLobeWires */

#ifdef I_dot5StatsRemoves
      case I_dot5StatsRemoves:
	dp = &data->dot5StatsRemoves;
	break;
#endif				       /* I_dot5StatsRemoves */

#ifdef I_dot5StatsSingles
      case I_dot5StatsSingles:
	dp = &data->dot5StatsSingles;
	break;
#endif				       /* I_dot5StatsSingles */

#ifdef I_dot5StatsFreqErrors
      case I_dot5StatsFreqErrors:
	dp = &data->dot5StatsFreqErrors;
	break;
#endif				       /* I_dot5StatsFreqErrors */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dot5TimerEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *dot5TimerEntry_get(OID *incoming, ObjectInfo *object,
	int searchType, ContextInfo *contextInfo, int serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dot5TimerEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dot5TimerIfIndex;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 1
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 1) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &dot5TimerIfIndex, 
	searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dot5TimerEntry_get(serialNum, contextInfo, 
	arg, searchType, dot5TimerIfIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dot5TimerIfIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dot5TimerIfIndex
      case I_dot5TimerIfIndex:
	dp = &data->dot5TimerIfIndex;
	break;
#endif				       /* I_dot5TimerIfIndex */

#ifdef I_dot5TimerReturnRepeat
      case I_dot5TimerReturnRepeat:
	dp = &data->dot5TimerReturnRepeat;
	break;
#endif				       /* I_dot5TimerReturnRepeat */

#ifdef I_dot5TimerHolding
      case I_dot5TimerHolding:
	dp = &data->dot5TimerHolding;
	break;
#endif				       /* I_dot5TimerHolding */

#ifdef I_dot5TimerQueuePDU
      case I_dot5TimerQueuePDU:
	dp = &data->dot5TimerQueuePDU;
	break;
#endif				       /* I_dot5TimerQueuePDU */

#ifdef I_dot5TimerValidTransmit
      case I_dot5TimerValidTransmit:
	dp = &data->dot5TimerValidTransmit;
	break;
#endif				       /* I_dot5TimerValidTransmit */

#ifdef I_dot5TimerNoToken
      case I_dot5TimerNoToken:
	dp = &data->dot5TimerNoToken;
	break;
#endif				       /* I_dot5TimerNoToken */

#ifdef I_dot5TimerActiveMon
      case I_dot5TimerActiveMon:
	dp = &data->dot5TimerActiveMon;
	break;
#endif				       /* I_dot5TimerActiveMon */

#ifdef I_dot5TimerStandbyMon
      case I_dot5TimerStandbyMon:
	dp = &data->dot5TimerStandbyMon;
	break;
#endif				       /* I_dot5TimerStandbyMon */

#ifdef I_dot5TimerErrorReport
      case I_dot5TimerErrorReport:
	dp = &data->dot5TimerErrorReport;
	break;
#endif				       /* I_dot5TimerErrorReport */

#ifdef I_dot5TimerBeaconTransmit
      case I_dot5TimerBeaconTransmit:
	dp = &data->dot5TimerBeaconTransmit;
	break;
#endif				       /* I_dot5TimerBeaconTransmit */

#ifdef I_dot5TimerBeaconReceive
      case I_dot5TimerBeaconReceive:
	dp = &data->dot5TimerBeaconReceive;
	break;
#endif				       /* I_dot5TimerBeaconReceive */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));
}


dot5Entry_t *k_dot5Entry_get (int serialNum, ContextInfo *contextInfo,
			      int nominator, int searchType, long dot5IfIndex)
{
    static dot5Entry_t dot5EntryData;
    static OctetString dot5UpStream, dot5Functional;
    hwidbtype *idb;
    ONE_ITEM_LIST(tmp_config, TOKEN_HW_FUNCTIONAL, sizeof(ulong), 0);
    N_ITEM_LIST(tmp_state, 2);

    idb = ifmib_hwidb_get(serialNum, searchType, dot5IfIndex,
			  idb_tester, NULL);
    if (idb==NULL)
	return (NULL);

    /*
     * Get the config info
     */
    (*idb->state_config)(idb, IDB_CONTROL_READ_CONFIG, &tmp_config_list);

    /*
     * Get the state info
     */
    item_desc_init(&tmp_state_item[0], TOKEN_HW_RINGOPENSTATUS, sizeof(ulong));
    item_desc_init(&tmp_state_item[1], TOKEN_HW_NAUN, sizeof(ulong));

    (*idb->state_config)(idb, IDB_CONTROL_READ_STATE, &tmp_state_list);

    bcopy((char *) &(tmp_config_item.u.l_value), (char *) (&func_address[2]),
	  IEEEBYTES - 2);

    dot5EntryData.dot5IfIndex = idb->snmp_if_index;
    dot5EntryData.dot5Commands = D_dot5Commands_no_op;

    dot5_ring_status(&dot5EntryData.dot5RingStatus, idb);

    switch (idb->state) {
      case IDBS_ADMINDOWN:
	dot5EntryData.dot5RingState = D_dot5RingState_closed;
	dot5EntryData.dot5RingOpenStatus = D_dot5RingOpenStatus_noOpen;
	break;
      case IDBS_UP:
	dot5EntryData.dot5RingState = D_dot5RingState_opened;
	dot5EntryData.dot5RingOpenStatus = D_dot5RingOpenStatus_open;
	break;
      case IDBS_DOWN:
	dot5EntryData.dot5RingState = D_dot5RingState_openFailure;
	dot5EntryData.dot5RingOpenStatus = D_dot5RingOpenStatus_noOpen;
	break;
      default:
	dot5EntryData.dot5RingState = D_dot5RingState_opening;
	dot5EntryData.dot5RingOpenStatus =
	    mib_decode_ring_open_status((uint)tmp_state_item[0].u.l_value);
	break;
    }
    switch (idb->tr_ring_speed) {
      case 16:
	dot5EntryData.dot5RingSpeed = D_dot5RingSpeed_sixteenMegabit;
	break;
      case 4:
	dot5EntryData.dot5RingSpeed = D_dot5RingSpeed_fourMegabit;
	break;
      default:
	dot5EntryData.dot5RingSpeed = D_dot5RingSpeed_unknown;
	break;
    }
    dot5EntryData.dot5ActMonParticipate = D_dot5ActMonParticipate_true;

    if (idb->dot5_get_smt_stats) {
	(*idb->dot5_get_smt_stats) (idb);
	(*idb->dot5_add_counters) (idb);
    }
    dot5EntryData.dot5UpStream = &dot5UpStream;
    dot5EntryData.dot5UpStream->octet_ptr = tmp_state_item[1].u.buffer;
    dot5EntryData.dot5UpStream->length = IEEEBYTES;

    dot5EntryData.dot5Functional = &dot5Functional;
    dot5EntryData.dot5Functional->octet_ptr = &(func_address[0]);
    dot5EntryData.dot5Functional->length = IEEEBYTES;

    return (&dot5EntryData);
}


int k_dot5Entry_set(dot5Entry_t *data, ContextInfo *contextInfo,
	int function)
{
    register hwidbtype *idb;

    idb = ifmib_hwidb_get(-1, EXACT, data->dot5IfIndex,
			  idb_tester, NULL);
    if (idb==NULL)
	return (GEN_ERROR);

    /* function must be ADD_MODIFY in this particular mib */

    if (VALID(I_dot5Commands, data->valid)) {
	switch (data->dot5Commands) {
	  case D_dot5Commands_no_op:
	    break;
	  case D_dot5Commands_open:
	    if (idb->state == IDBS_ADMINDOWN)
		net_cstate(idb, IDBS_DOWN);
	    (*idb->reset) (idb);
	    break;
	  case D_dot5Commands_reset:
	    if (idb->dot5_soft_reset)
		(void) (*idb->dot5_soft_reset) (idb);
	    break;
	  case D_dot5Commands_close:
	    if (idb->shutdown)
		(*idb->shutdown) (idb, IDBS_ADMINDOWN);
	    break;
	}
    }
    if (VALID(I_dot5RingSpeed, data->valid)) {
	switch (data->dot5RingSpeed) {
	  case D_dot5RingSpeed_unknown:
	  case D_dot5RingSpeed_oneMegabit:
	  case D_dot5RingSpeed_fourMegabit:
	  case D_dot5RingSpeed_sixteenMegabit:
	    return (NOT_WRITABLE_ERROR);
	}
    }
    if (VALID(I_dot5ActMonParticipate, data->valid)) {
	switch (data->dot5ActMonParticipate) {
	  case D_dot5ActMonParticipate_true:
	  case D_dot5ActMonParticipate_false:
	    return (NOT_WRITABLE_ERROR);
	}
    }
    if (VALID(I_dot5Functional, data->valid)) {
	return (NOT_WRITABLE_ERROR);
    }
    return (NO_ERROR);
}


dot5StatsEntry_t *
k_dot5StatsEntry_get(int serialNum, ContextInfo *contextInfo,
	int nominator, int searchType, long dot5StatsIfIndex)
{
    static dot5StatsEntry_t dot5StatsEntryData;
    hwidbtype *idb;
    itemlist *tmp_list = NULL;
    itemdesc *tmp_item = NULL;

    idb = ifmib_hwidb_get(serialNum, searchType, dot5StatsIfIndex,
			  idb_tester, NULL);
    if (idb==NULL)
	return (NULL);

    item_list_malloc(tmp_list, tmp_item, 15);

    if (tmp_list == NULL) {
	return(NULL);
    }

    /*
     * Get state info
     */
    item_desc_init(&tmp_item[0], TOKEN_HW_PREV_ERR_LINE, sizeof(ulong));
    item_desc_init(&tmp_item[1], TOKEN_HW_PREV_ERR_BURST, sizeof(ulong));
    item_desc_init(&tmp_item[2], TOKEN_HW_PREV_BAD_ACBITS, sizeof(ulong));
    item_desc_init(&tmp_item[3], TOKEN_HW_PREV_ERR_INTERNAL, sizeof(ulong));
    item_desc_init(&tmp_item[4], TOKEN_HW_PREV_ERR_LOSTFRAME, sizeof(ulong));
    item_desc_init(&tmp_item[5], TOKEN_HW_PREV_ERR_RCVCONGESTION,
		   sizeof(ulong));
    item_desc_init(&tmp_item[6], TOKEN_HW_PREV_ERR_COPY, sizeof(ulong));
    item_desc_init(&tmp_item[7], TOKEN_HW_PREV_ERR_BURST, sizeof(ulong));
    item_desc_init(&tmp_item[8], TOKEN_HW_PREV_SOFT_ERRS, sizeof(ulong));
    item_desc_init(&tmp_item[9], TOKEN_HW_PREV_HARD_ERRS, sizeof(ulong));
    item_desc_init(&tmp_item[10], TOKEN_HW_PREV_SIGNAL_LOSS, sizeof(ulong));
    item_desc_init(&tmp_item[11], TOKEN_HW_PREV_TX_BEACON, sizeof(ulong));
    item_desc_init(&tmp_item[12], TOKEN_HW_PREV_RING_RECOVERY, sizeof(ulong));
    item_desc_init(&tmp_item[13], TOKEN_HW_SINGLE_STATION, sizeof(ulong));
    item_desc_init(&tmp_item[14], TOKEN_HW_PREV_ERR_FREQ, sizeof(ulong));

    (*idb->state_config)(idb, IDB_CONTROL_READ_STATE, tmp_list);

    dot5StatsEntryData.dot5StatsIfIndex = idb->snmp_if_index;
    dot5StatsEntryData.dot5StatsLineErrors = tmp_item[0].u.l_value;
    dot5StatsEntryData.dot5StatsBurstErrors = tmp_item[1].u.l_value;
    dot5StatsEntryData.dot5StatsACErrors = tmp_item[2].u.l_value;
    dot5StatsEntryData.dot5StatsAbortTransErrors = 0;
    dot5StatsEntryData.dot5StatsInternalErrors = tmp_item[3].u.l_value;
    dot5StatsEntryData.dot5StatsLostFrameErrors = tmp_item[4].u.l_value;
    dot5StatsEntryData.dot5StatsReceiveCongestions = tmp_item[5].u.l_value;
    dot5StatsEntryData.dot5StatsFrameCopiedErrors = tmp_item[6].u.l_value;
    dot5StatsEntryData.dot5StatsTokenErrors = tmp_item[7].u.l_value;
    dot5StatsEntryData.dot5StatsSoftErrors = tmp_item[8].u.l_value;
    dot5StatsEntryData.dot5StatsHardErrors = tmp_item[9].u.l_value;
    dot5StatsEntryData.dot5StatsSignalLoss = tmp_item[10].u.l_value;
    dot5StatsEntryData.dot5StatsTransmitBeacons = tmp_item[11].u.l_value;
    dot5StatsEntryData.dot5StatsRecoverys = tmp_item[12].u.l_value;
    dot5StatsEntryData.dot5StatsLobeWires = 0;
    dot5StatsEntryData.dot5StatsRemoves = 0;
    dot5StatsEntryData.dot5StatsSingles = tmp_item[13].u.l_value;
    dot5StatsEntryData.dot5StatsFreqErrors = tmp_item[14].u.l_value;

    free(tmp_list);

    return (&dot5StatsEntryData);
}


dot5TimerEntry_t *
k_dot5TimerEntry_get(int serialNum, ContextInfo *contextInfo,
	int nominator, int searchType, long dot5TimerIfIndex)
{
#ifdef NOT_YET
    static dot5TimerEntry_t dot5TimerEntryData;
    register hwidbtype *idb;

    idb = ifmib_hwidb_get(serialNum, searchType, dot5TimerIfIndex,
			  idb_tester, NULL);
    if (idb==NULL)
	return (NULL);

    dot5TimerEntryData.dot5TimerIfIndex = idb->snmp_if_index;
    dot5TimerEntryData.dot5TimerReturnRepeat = 0;
    dot5TimerEntryData.dot5TimerHolding = 0;
    dot5TimerEntryData.dot5TimerQueuePDU = 0;
    dot5TimerEntryData.dot5TimerValidTransmit = 0;
    dot5TimerEntryData.dot5TimerNoToken = 0;
    dot5TimerEntryData.dot5TimerActiveMon = 0;
    dot5TimerEntryData.dot5TimerStandbyMon = 0;
    dot5TimerEntryData.dot5TimerErrorReport = 0;
    dot5TimerEntryData.dot5TimerBeaconTransmit = 0;
    dot5TimerEntryData.dot5TimerBeaconReceive = 0;

    return (&dot5TimerEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}


/*
 * dot5_ring_status
 */
static void dot5_ring_status (long *var, hwidbtype *tidb)
{
    ushort tr_ring_status;
    ONE_ITEM_LIST(tmp_config, TOKEN_HW_RING_STATUS, sizeof(ushort), 0);

    if (tidb->state == IDBS_ADMINDOWN) {
	*var = RSTS_NO_STATUS;
	return;
    }

    /*
     * Get the current ring status
     */
    (*tidb->state_config)(tidb, IDB_CONTROL_READ_STATE, &tmp_config_list);
    tr_ring_status = tmp_config_item.u.s_value[0];

    *var = RSTS_NO_PROBLEMS;

    if (tr_ring_status & RNG_SIGNAL_LOSS)
	*var += RSTS_SIGNAL_LOSS;

    if (tr_ring_status & RNG_HARD_ERROR)
	*var += RSTS_HARD_ERROR;

    if (tr_ring_status & RNG_SOFT_ERROR)
	*var += RSTS_SOFT_ERROR;

    if (tr_ring_status & RNG_BEACON)
	*var += RSTS_TRANSMIT_BEACON;

    if (tr_ring_status & RNG_WIRE_FAULT)
	*var += RSTS_LOBE_WIRE_FAULT;

    if (tr_ring_status & RNG_RMT_REMOVAL)
	*var += RSTS_REMOVE_RCVD;

    if (tr_ring_status & RNG_SINGLE)
	*var += RSTS_SINGLE_STATION;

    if (tr_ring_status & RNG_RECOVERY)
	*var += RSTS_RING_RECOVERY;
}


/*
 * mib_decode_ring_open_status
 */
static int mib_decode_ring_open_status (uint error)
{
    switch (error) {
      case 1:
	return (D_dot5RingOpenStatus_lobeFailed);
      case 2:
	return (D_dot5RingOpenStatus_badParam);
      case 3:
	return (D_dot5RingOpenStatus_lobeFailed);
      case 4:
	return (D_dot5RingOpenStatus_signalLoss);
      case 5:
	return (D_dot5RingOpenStatus_insertionTimeout);
      case 6:
	return (D_dot5RingOpenStatus_ringFailed);
      case 7:
	return (D_dot5RingOpenStatus_beaconing);
      case 8:
	return (D_dot5RingOpenStatus_duplicateMAC);
      case 9:
	return (D_dot5RingOpenStatus_requestFailed);
      case 10:
	return (D_dot5RingOpenStatus_removeReceived);
      default:
	return (D_dot5RingOpenStatus_noOpen);
    }
}

static boolean idb_tester(hwidbtype *idb, long* index2)
{
   if (idb->status & IDB_TR)
      return(TRUE);
   else
      return(FALSE);
}


/*
 * trmib subsystem header
 */


#define MAJVERSION_trmib 1
#define MINVERSION_trmib 0
#define EDITVERSION_trmib 0

SUBSYS_HEADER(trmib,
              MAJVERSION_trmib,
              MINVERSION_trmib,
              EDITVERSION_trmib,
              init_trmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: tring",
              NULL);
      
