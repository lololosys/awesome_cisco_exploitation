/* $Id: sr_ds1mib.c,v 3.5.20.4 1996/08/22 21:48:10 vdukki Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_ds1mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Kenny Roberts (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ds1mib.c,v $
 * Revision 3.5.20.4  1996/08/22  21:48:10  vdukki
 * CSCdi65777:  Channelized E1 interface doesnt respond to ds1 MIB table
 * requests
 * Branch: California_branch
 * Modified the code to look for E1 interfaces, in addition to T1
 * interfaces, while accessing the ds1 table. Also did some code
 * cleanups.
 *
 * Revision 3.5.20.3  1996/08/07  21:56:45  sdurham
 * CSCdi62654:  Duplicate interface MIB instances. Add cdb interface for
 * T1 only
 * Branch: California_branch
 *
 * Revision 3.5.20.2  1996/07/01  18:44:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.20.1  1996/06/26  03:00:51  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.5  1996/02/02  02:55:01  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 * modularize the envmon and ds1 mib translation tables
 *
 * Revision 3.4  1996/01/18  15:45:42  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:24:00  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:20:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  07:31:43  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.2  1995/06/28  09:24:48  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:43:55  hampton
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

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/if_controller.h" 
#include "../if/dsx1.h"

#include "../snmp/snmp_api.h"
#include "sr_ds1mib.h"
#include "sr_ds1mib-mib.h"
#include "../snmp/ifmib_registry.h"
#include "../snmp/chassismib_registry.h"

#define IF_T1_SPEED  1544000
#define IF_E1_SPEED  2048000

static dsx1MIBDataTable *
snmp_get_dsx(int searchType, long *index);

static dsx1MIBDataTable *
snmp_get_dsx_interval(int searchType, long *index, long *interval);

static hwidbtype * 
snmp_get_dsx_idb(int searchType, long *index, long *interval);

static boolean ds1_admin_change(
	  enum SNMP_ADMIN_STATE	newStatus,
	  boolean		testPhase,
	  const	subiabtype      *subiab);

static void ds1_add_all_if(void);
static void ds1_add_cdb_index(cdbtype * cdb, ulong);
static void ds1_add_if(cdbtype *cdb);
static void ds1_update_ifAlias(subiabtype *subiab, OctetString *ifalias,
				  boolean set);

static enum SNMP_ADMIN_STATE ds1_get_adminstatus(const subiabtype *subiab);
static enum SNMP_ADMIN_STATE ds1_get_operstatus(const subiabtype *subiab);

static ulong ds1_get_if_speed (const subiabtype *subiab);

void
init_ds1mib(subsystype *subsys)
{
    load_mib(ds1mib_OidList, ds1mib_OidListNum);
    load_oid(ds1mib_oid_table);
    /*
     * register to process counters for ds1 interfaces for
     * ifmib.
     */

    reg_add_ifmib_admin_change(D_ifType_ds1,ds1_admin_change,"ds1_admin_change");
    reg_add_ifmib_get_adminstatus(D_ifType_ds1,ds1_get_adminstatus,"ds1_get_adminstatus");
    reg_add_ifmib_get_operstatus(D_ifType_ds1,ds1_get_operstatus,"ds1_get_operstatus");
    reg_add_ifmib_add_all_cdb_if(ds1_add_all_if,"ds1_add_all_if");
    reg_add_ifmib_add_cdb_index(ds1_add_cdb_index,"ds1_add_cdb_index");
    reg_add_ifmib_add_cdb_if(PIFT_T1,ds1_add_if,"ds1_add_if");
    reg_add_ifmib_update_ifAlias(D_ifType_ds1,ds1_update_ifAlias,
                 "ds1_update_ifAlias");
    reg_add_ifmib_add_cdb_if(PIFT_E1,ds1_add_if,"ds1_add_if");
    reg_add_cardifmib_get_card_index(cardifmib_get_card_index, 
        "cardifmib_get_card_index");
    reg_add_cardifmib_get_slot_number(cardifmib_get_slot_number, 
        "cardifmib_get_slot_number");
    reg_add_cardifmib_get_port_number(cardifmib_get_port_number, 
        "cardifmib_get_port_number");
    reg_add_cardifmib_get_connector_type(cardifmib_get_connector_type, 
        "cardifmib_get_connector_type");
    reg_add_ifmib_get_if_speed(D_ifType_ds1,ds1_get_if_speed,
			       "ds1_get_if_speed");

}


/*---------------------------------------------------------------------
 * Retrieve data from the dsx1ConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1ConfigEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1ConfigEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dsx1LineIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1LineIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1ConfigEntry_get(serialNum, contextInfo, arg, searchType, dsx1LineIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dsx1LineIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1LineIndex
      case I_dsx1LineIndex:
	dp = &data->dsx1LineIndex;
	break;
#endif				       /* I_dsx1LineIndex */

#ifdef I_dsx1IfIndex
      case I_dsx1IfIndex:
	dp = &data->dsx1IfIndex;
	break;
#endif				       /* I_dsx1IfIndex */

#ifdef I_dsx1TimeElapsed
      case I_dsx1TimeElapsed:
	dp = &data->dsx1TimeElapsed;
	break;
#endif				       /* I_dsx1TimeElapsed */

#ifdef I_dsx1ValidIntervals
      case I_dsx1ValidIntervals:
	dp = &data->dsx1ValidIntervals;
	break;
#endif				       /* I_dsx1ValidIntervals */

#ifdef I_dsx1LineType
      case I_dsx1LineType:
	dp = &data->dsx1LineType;
	break;
#endif				       /* I_dsx1LineType */

#ifdef I_dsx1LineCoding
      case I_dsx1LineCoding:
	dp = &data->dsx1LineCoding;
	break;
#endif				       /* I_dsx1LineCoding */

#ifdef I_dsx1SendCode
      case I_dsx1SendCode:
	dp = &data->dsx1SendCode;
	break;
#endif				       /* I_dsx1SendCode */

#ifdef I_dsx1CircuitIdentifier
      case I_dsx1CircuitIdentifier:
	dp = MakeOctetString(data->dsx1CircuitIdentifier->octet_ptr, data->dsx1CircuitIdentifier->length);
	break;
#endif				       /* I_dsx1CircuitIdentifier */

#ifdef I_dsx1LoopbackConfig
      case I_dsx1LoopbackConfig:
	dp = &data->dsx1LoopbackConfig;
	break;
#endif				       /* I_dsx1LoopbackConfig */

#ifdef I_dsx1LineStatus
      case I_dsx1LineStatus:
	dp = &data->dsx1LineStatus;
	break;
#endif				       /* I_dsx1LineStatus */

#ifdef I_dsx1SignalMode
      case I_dsx1SignalMode:
	dp = &data->dsx1SignalMode;
	break;
#endif				       /* I_dsx1SignalMode */

#ifdef I_dsx1TransmitClockSource
      case I_dsx1TransmitClockSource:
	dp = &data->dsx1TransmitClockSource;
	break;
#endif				       /* I_dsx1TransmitClockSource */

#ifdef I_dsx1Fdl
      case I_dsx1Fdl:
	dp = &data->dsx1Fdl;
	break;
#endif				       /* I_dsx1Fdl */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dsx1ConfigEntry data object.
 *---------------------------------------------------------------------*/
static void
dsx1ConfigEntry_free(
    dsx1ConfigEntry_t *data)
{
    if (data != NULL) {
	if (data->dsx1CircuitIdentifier != NULL) {
	    FreeOctetString(data->dsx1CircuitIdentifier);
	}
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dsx1ConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
dsx1ConfigEntry_cleanup(
    doList_t       *trash)
{
    dsx1ConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    dsx1ConfigEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

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
dsx1ConfigEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength;
    doList_t       *dp;
    /*VarBind        *vb = (VarBind *) NULL;*/
    int             found;
    int             carry;
    dsx1ConfigEntry_t *dsx1ConfigEntry;
    long            dsx1LineIndex;

    instLength = incoming->length - object->oid.length;
    carry = 0;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 1) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1LineIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dsx1ConfigEntry = k_dsx1ConfigEntry_get(-1, contextInfo, object->nominator, EXACT, dsx1LineIndex);

#ifndef dsx1ConfigEntry_READ_CREATE

    if (dsx1ConfigEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dsx1ConfigEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dsx1ConfigEntry_set) &&
	    (((dsx1ConfigEntry_t *) (dp->data)) != NULL) &&
	    (((dsx1ConfigEntry_t *) (dp->data))->dsx1LineIndex == dsx1LineIndex)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dsx1ConfigEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dsx1ConfigEntry_t));

	dp->setMethod = dsx1ConfigEntry_set;
	dp->cleanupMethod = dsx1ConfigEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dsx1LineIndex, ((dsx1ConfigEntry_t *) (dp->data))->valid);
	((dsx1ConfigEntry_t *) (dp->data))->dsx1LineIndex = dsx1LineIndex;

#ifdef dsx1ConfigEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dsx1ConfigEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dsx1ConfigEntry, sizeof(dsx1ConfigEntry_t));

	    SET_ALL_VALID(((dsx1ConfigEntry_t *) (dp->data))->valid);

	    ((dsx1ConfigEntry_t *) (dp->data))->dsx1CircuitIdentifier =
		CloneOctetString(dsx1ConfigEntry->dsx1CircuitIdentifier);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dsx1ConfigEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dsx1LineType
      case I_dsx1LineType:

	if ((value->sl_value != D_dsx1LineType_other) &&
	    (value->sl_value != D_dsx1LineType_dsx1ESF) &&
	    (value->sl_value != D_dsx1LineType_dsx1D4) &&
	    (value->sl_value != D_dsx1LineType_dsx1E1) &&
	    (value->sl_value != D_dsx1LineType_dsx1E1_CRC) &&
	    (value->sl_value != D_dsx1LineType_dsx1E1_MF) &&
	    (value->sl_value != D_dsx1LineType_dsx1E1_CRC_MF))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1LineType, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1LineType = value->sl_value;
	break;
#endif				       /* I_dsx1LineType */

#ifdef I_dsx1LineCoding
      case I_dsx1LineCoding:

	if ((value->sl_value != D_dsx1LineCoding_dsx1JBZS) &&
	    (value->sl_value != D_dsx1LineCoding_dsx1B8ZS) &&
	    (value->sl_value != D_dsx1LineCoding_dsx1HDB3) &&
	    (value->sl_value != D_dsx1LineCoding_dsx1ZBTSI) &&
	    (value->sl_value != D_dsx1LineCoding_dsx1AMI) &&
	    (value->sl_value != D_dsx1LineCoding_other))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1LineCoding, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1LineCoding = value->sl_value;
	break;
#endif				       /* I_dsx1LineCoding */

#ifdef I_dsx1SendCode
      case I_dsx1SendCode:

	if ((value->sl_value != D_dsx1SendCode_dsx1SendNoCode) &&
	    (value->sl_value != D_dsx1SendCode_dsx1SendLineCode) &&
	    (value->sl_value != D_dsx1SendCode_dsx1SendPayloadCode) &&
	    (value->sl_value != D_dsx1SendCode_dsx1SendResetCode) &&
	    (value->sl_value != D_dsx1SendCode_dsx1SendQRS) &&
	    (value->sl_value != D_dsx1SendCode_dsx1Send511Pattern) &&
	    (value->sl_value != D_dsx1SendCode_dsx1Send3in24Pattern) &&
	    (value->sl_value != D_dsx1SendCode_dsx1SendOtherTestPattern))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1SendCode, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1SendCode = value->sl_value;
	break;
#endif				       /* I_dsx1SendCode */

#ifdef I_dsx1CircuitIdentifier
      case I_dsx1CircuitIdentifier:

	SET_VALID(I_dsx1CircuitIdentifier, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	if (((dsx1ConfigEntry_t *) (dp->data))->dsx1CircuitIdentifier != NULL) {
	    FreeOctetString(((dsx1ConfigEntry_t *) (dp->data))->dsx1CircuitIdentifier);
	}
	((dsx1ConfigEntry_t *) (dp->data))->dsx1CircuitIdentifier =
	    MakeOctetString(value->os_value->octet_ptr, value->os_value->length);

	break;
#endif				       /* I_dsx1CircuitIdentifier */

#ifdef I_dsx1LoopbackConfig
      case I_dsx1LoopbackConfig:

	if ((value->sl_value != D_dsx1LoopbackConfig_dsx1NoLoop) &&
	    (value->sl_value != D_dsx1LoopbackConfig_dsx1PayloadLoop) &&
	    (value->sl_value != D_dsx1LoopbackConfig_dsx1LineLoop) &&
	    (value->sl_value != D_dsx1LoopbackConfig_dsx1OtherLoop))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1LoopbackConfig, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1LoopbackConfig = value->sl_value;
	break;
#endif				       /* I_dsx1LoopbackConfig */

#ifdef I_dsx1SignalMode
      case I_dsx1SignalMode:

	if ((value->sl_value != D_dsx1SignalMode_none) &&
	    (value->sl_value != D_dsx1SignalMode_robbedBit) &&
	    (value->sl_value != D_dsx1SignalMode_bitOriented) &&
	    (value->sl_value != D_dsx1SignalMode_messageOriented))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1SignalMode, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1SignalMode = value->sl_value;
	break;
#endif				       /* I_dsx1SignalMode */

#ifdef I_dsx1TransmitClockSource
      case I_dsx1TransmitClockSource:

	if ((value->sl_value != D_dsx1TransmitClockSource_loopTiming) &&
	    (value->sl_value != D_dsx1TransmitClockSource_localTiming) &&
	    (value->sl_value != D_dsx1TransmitClockSource_throughTiming))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1TransmitClockSource, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1TransmitClockSource = value->sl_value;
	break;
#endif				       /* I_dsx1TransmitClockSource */

#ifdef I_dsx1Fdl
      case I_dsx1Fdl:

	if ((value->sl_value != D_dsx1Fdl_other) &&
	    (value->sl_value != D_dsx1Fdl_dsx1Ansi_T1_403) &&
	    (value->sl_value != D_dsx1Fdl_dsx1Att_54016) &&
	    (value->sl_value != D_dsx1Fdl_dsx1Fdl_none))
	    return(WRONG_VALUE_ERROR);

	SET_VALID(I_dsx1Fdl, ((dsx1ConfigEntry_t *) (dp->data))->valid);

	((dsx1ConfigEntry_t *) (dp->data))->dsx1Fdl = value->sl_value;
	break;
#endif				       /* I_dsx1Fdl */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dsx1ConfigEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
dsx1ConfigEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_dsx1ConfigEntry_set((dsx1ConfigEntry_t *) (doCur->data),
				  contextInfo, doCur->state));
}

#endif				       /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the dsx1CurrentEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1CurrentEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1CurrentEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dsx1CurrentIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1CurrentIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1CurrentEntry_get(serialNum, contextInfo, arg, searchType, dsx1CurrentIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dsx1CurrentIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1CurrentIndex
      case I_dsx1CurrentIndex:
	dp = &data->dsx1CurrentIndex;
	break;
#endif				       /* I_dsx1CurrentIndex */

#ifdef I_dsx1CurrentESs
      case I_dsx1CurrentESs:
	dp = &data->dsx1CurrentESs;
	break;
#endif				       /* I_dsx1CurrentESs */

#ifdef I_dsx1CurrentSESs
      case I_dsx1CurrentSESs:
	dp = &data->dsx1CurrentSESs;
	break;
#endif				       /* I_dsx1CurrentSESs */

#ifdef I_dsx1CurrentSEFSs
      case I_dsx1CurrentSEFSs:
	dp = &data->dsx1CurrentSEFSs;
	break;
#endif				       /* I_dsx1CurrentSEFSs */

#ifdef I_dsx1CurrentUASs
      case I_dsx1CurrentUASs:
	dp = &data->dsx1CurrentUASs;
	break;
#endif				       /* I_dsx1CurrentUASs */

#ifdef I_dsx1CurrentCSSs
      case I_dsx1CurrentCSSs:
	dp = &data->dsx1CurrentCSSs;
	break;
#endif				       /* I_dsx1CurrentCSSs */

#ifdef I_dsx1CurrentPCVs
      case I_dsx1CurrentPCVs:
	dp = &data->dsx1CurrentPCVs;
	break;
#endif				       /* I_dsx1CurrentPCVs */

#ifdef I_dsx1CurrentLESs
      case I_dsx1CurrentLESs:
	dp = &data->dsx1CurrentLESs;
	break;
#endif				       /* I_dsx1CurrentLESs */

#ifdef I_dsx1CurrentBESs
      case I_dsx1CurrentBESs:
	dp = &data->dsx1CurrentBESs;
	break;
#endif				       /* I_dsx1CurrentBESs */

#ifdef I_dsx1CurrentDMs
      case I_dsx1CurrentDMs:
	dp = &data->dsx1CurrentDMs;
	break;
#endif				       /* I_dsx1CurrentDMs */

#ifdef I_dsx1CurrentLCVs
      case I_dsx1CurrentLCVs:
	dp = &data->dsx1CurrentLCVs;
	break;
#endif				       /* I_dsx1CurrentLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1IntervalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1IntervalEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1IntervalEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dsx1IntervalIndex;
    long            dsx1IntervalNumber;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 2
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 2) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &dsx1IntervalNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1IntervalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1IntervalEntry_get(serialNum, contextInfo, arg, searchType, dsx1IntervalIndex, dsx1IntervalNumber)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dsx1IntervalIndex;
	inst.oid_ptr[1] = data->dsx1IntervalNumber;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1IntervalIndex
      case I_dsx1IntervalIndex:
	dp = &data->dsx1IntervalIndex;
	break;
#endif				       /* I_dsx1IntervalIndex */

#ifdef I_dsx1IntervalNumber
      case I_dsx1IntervalNumber:
	dp = &data->dsx1IntervalNumber;
	break;
#endif				       /* I_dsx1IntervalNumber */

#ifdef I_dsx1IntervalESs
      case I_dsx1IntervalESs:
	dp = &data->dsx1IntervalESs;
	break;
#endif				       /* I_dsx1IntervalESs */

#ifdef I_dsx1IntervalSESs
      case I_dsx1IntervalSESs:
	dp = &data->dsx1IntervalSESs;
	break;
#endif				       /* I_dsx1IntervalSESs */

#ifdef I_dsx1IntervalSEFSs
      case I_dsx1IntervalSEFSs:
	dp = &data->dsx1IntervalSEFSs;
	break;
#endif				       /* I_dsx1IntervalSEFSs */

#ifdef I_dsx1IntervalUASs
      case I_dsx1IntervalUASs:
	dp = &data->dsx1IntervalUASs;
	break;
#endif				       /* I_dsx1IntervalUASs */

#ifdef I_dsx1IntervalCSSs
      case I_dsx1IntervalCSSs:
	dp = &data->dsx1IntervalCSSs;
	break;
#endif				       /* I_dsx1IntervalCSSs */

#ifdef I_dsx1IntervalPCVs
      case I_dsx1IntervalPCVs:
	dp = &data->dsx1IntervalPCVs;
	break;
#endif				       /* I_dsx1IntervalPCVs */

#ifdef I_dsx1IntervalLESs
      case I_dsx1IntervalLESs:
	dp = &data->dsx1IntervalLESs;
	break;
#endif				       /* I_dsx1IntervalLESs */

#ifdef I_dsx1IntervalBESs
      case I_dsx1IntervalBESs:
	dp = &data->dsx1IntervalBESs;
	break;
#endif				       /* I_dsx1IntervalBESs */

#ifdef I_dsx1IntervalDMs
      case I_dsx1IntervalDMs:
	dp = &data->dsx1IntervalDMs;
	break;
#endif				       /* I_dsx1IntervalDMs */

#ifdef I_dsx1IntervalLCVs
      case I_dsx1IntervalLCVs:
	dp = &data->dsx1IntervalLCVs;
	break;
#endif				       /* I_dsx1IntervalLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1TotalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1TotalEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1TotalEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dsx1TotalIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1TotalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1TotalEntry_get(serialNum, contextInfo, arg, searchType, dsx1TotalIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dsx1TotalIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1TotalIndex
      case I_dsx1TotalIndex:
	dp = &data->dsx1TotalIndex;
	break;
#endif				       /* I_dsx1TotalIndex */

#ifdef I_dsx1TotalESs
      case I_dsx1TotalESs:
	dp = &data->dsx1TotalESs;
	break;
#endif				       /* I_dsx1TotalESs */

#ifdef I_dsx1TotalSESs
      case I_dsx1TotalSESs:
	dp = &data->dsx1TotalSESs;
	break;
#endif				       /* I_dsx1TotalSESs */

#ifdef I_dsx1TotalSEFSs
      case I_dsx1TotalSEFSs:
	dp = &data->dsx1TotalSEFSs;
	break;
#endif				       /* I_dsx1TotalSEFSs */

#ifdef I_dsx1TotalUASs
      case I_dsx1TotalUASs:
	dp = &data->dsx1TotalUASs;
	break;
#endif				       /* I_dsx1TotalUASs */

#ifdef I_dsx1TotalCSSs
      case I_dsx1TotalCSSs:
	dp = &data->dsx1TotalCSSs;
	break;
#endif				       /* I_dsx1TotalCSSs */

#ifdef I_dsx1TotalPCVs
      case I_dsx1TotalPCVs:
	dp = &data->dsx1TotalPCVs;
	break;
#endif				       /* I_dsx1TotalPCVs */

#ifdef I_dsx1TotalLESs
      case I_dsx1TotalLESs:
	dp = &data->dsx1TotalLESs;
	break;
#endif				       /* I_dsx1TotalLESs */

#ifdef I_dsx1TotalBESs
      case I_dsx1TotalBESs:
	dp = &data->dsx1TotalBESs;
	break;
#endif				       /* I_dsx1TotalBESs */

#ifdef I_dsx1TotalDMs
      case I_dsx1TotalDMs:
	dp = &data->dsx1TotalDMs;
	break;
#endif				       /* I_dsx1TotalDMs */

#ifdef I_dsx1TotalLCVs
      case I_dsx1TotalLCVs:
	dp = &data->dsx1TotalLCVs;
	break;
#endif				       /* I_dsx1TotalLCVs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1FarEndCurrentEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1FarEndCurrentEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1FarEndCurrentEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dsx1FarEndCurrentIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1FarEndCurrentIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1FarEndCurrentEntry_get(serialNum, contextInfo, arg, searchType, dsx1FarEndCurrentIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dsx1FarEndCurrentIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1FarEndCurrentIndex
      case I_dsx1FarEndCurrentIndex:
	dp = &data->dsx1FarEndCurrentIndex;
	break;
#endif				       /* I_dsx1FarEndCurrentIndex */

#ifdef I_dsx1FarEndTimeElapsed
      case I_dsx1FarEndTimeElapsed:
	dp = &data->dsx1FarEndTimeElapsed;
	break;
#endif				       /* I_dsx1FarEndTimeElapsed */

#ifdef I_dsx1FarEndValidIntervals
      case I_dsx1FarEndValidIntervals:
	dp = &data->dsx1FarEndValidIntervals;
	break;
#endif				       /* I_dsx1FarEndValidIntervals */

#ifdef I_dsx1FarEndCurrentESs
      case I_dsx1FarEndCurrentESs:
	dp = &data->dsx1FarEndCurrentESs;
	break;
#endif				       /* I_dsx1FarEndCurrentESs */

#ifdef I_dsx1FarEndCurrentSESs
      case I_dsx1FarEndCurrentSESs:
	dp = &data->dsx1FarEndCurrentSESs;
	break;
#endif				       /* I_dsx1FarEndCurrentSESs */

#ifdef I_dsx1FarEndCurrentSEFSs
      case I_dsx1FarEndCurrentSEFSs:
	dp = &data->dsx1FarEndCurrentSEFSs;
	break;
#endif				       /* I_dsx1FarEndCurrentSEFSs */

#ifdef I_dsx1FarEndCurrentUASs
      case I_dsx1FarEndCurrentUASs:
	dp = &data->dsx1FarEndCurrentUASs;
	break;
#endif				       /* I_dsx1FarEndCurrentUASs */

#ifdef I_dsx1FarEndCurrentCSSs
      case I_dsx1FarEndCurrentCSSs:
	dp = &data->dsx1FarEndCurrentCSSs;
	break;
#endif				       /* I_dsx1FarEndCurrentCSSs */

#ifdef I_dsx1FarEndCurrentLESs
      case I_dsx1FarEndCurrentLESs:
	dp = &data->dsx1FarEndCurrentLESs;
	break;
#endif				       /* I_dsx1FarEndCurrentLESs */

#ifdef I_dsx1FarEndCurrentPCVs
      case I_dsx1FarEndCurrentPCVs:
	dp = &data->dsx1FarEndCurrentPCVs;
	break;
#endif				       /* I_dsx1FarEndCurrentPCVs */

#ifdef I_dsx1FarEndCurrentBESs
      case I_dsx1FarEndCurrentBESs:
	dp = &data->dsx1FarEndCurrentBESs;
	break;
#endif				       /* I_dsx1FarEndCurrentBESs */

#ifdef I_dsx1FarEndCurrentDMs
      case I_dsx1FarEndCurrentDMs:
	dp = &data->dsx1FarEndCurrentDMs;
	break;
#endif				       /* I_dsx1FarEndCurrentDMs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1FarEndIntervalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1FarEndIntervalEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1FarEndIntervalEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dsx1FarEndIntervalIndex;
    long            dsx1FarEndIntervalNumber;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;


    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 2
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 2) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &dsx1FarEndIntervalNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1FarEndIntervalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1FarEndIntervalEntry_get(serialNum, contextInfo, arg, searchType, dsx1FarEndIntervalIndex, dsx1FarEndIntervalNumber)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dsx1FarEndIntervalIndex;
	inst.oid_ptr[1] = data->dsx1FarEndIntervalNumber;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1FarEndIntervalIndex
      case I_dsx1FarEndIntervalIndex:
	dp = &data->dsx1FarEndIntervalIndex;
	break;
#endif				       /* I_dsx1FarEndIntervalIndex */

#ifdef I_dsx1FarEndIntervalNumber
      case I_dsx1FarEndIntervalNumber:
	dp = &data->dsx1FarEndIntervalNumber;
	break;
#endif				       /* I_dsx1FarEndIntervalNumber */

#ifdef I_dsx1FarEndIntervalESs
      case I_dsx1FarEndIntervalESs:
	dp = &data->dsx1FarEndIntervalESs;
	break;
#endif				       /* I_dsx1FarEndIntervalESs */

#ifdef I_dsx1FarEndIntervalSESs
      case I_dsx1FarEndIntervalSESs:
	dp = &data->dsx1FarEndIntervalSESs;
	break;
#endif				       /* I_dsx1FarEndIntervalSESs */

#ifdef I_dsx1FarEndIntervalSEFSs
      case I_dsx1FarEndIntervalSEFSs:
	dp = &data->dsx1FarEndIntervalSEFSs;
	break;
#endif				       /* I_dsx1FarEndIntervalSEFSs */

#ifdef I_dsx1FarEndIntervalUASs
      case I_dsx1FarEndIntervalUASs:
	dp = &data->dsx1FarEndIntervalUASs;
	break;
#endif				       /* I_dsx1FarEndIntervalUASs */

#ifdef I_dsx1FarEndIntervalCSSs
      case I_dsx1FarEndIntervalCSSs:
	dp = &data->dsx1FarEndIntervalCSSs;
	break;
#endif				       /* I_dsx1FarEndIntervalCSSs */

#ifdef I_dsx1FarEndIntervalLESs
      case I_dsx1FarEndIntervalLESs:
	dp = &data->dsx1FarEndIntervalLESs;
	break;
#endif				       /* I_dsx1FarEndIntervalLESs */

#ifdef I_dsx1FarEndIntervalPCVs
      case I_dsx1FarEndIntervalPCVs:
	dp = &data->dsx1FarEndIntervalPCVs;
	break;
#endif				       /* I_dsx1FarEndIntervalPCVs */

#ifdef I_dsx1FarEndIntervalBESs
      case I_dsx1FarEndIntervalBESs:
	dp = &data->dsx1FarEndIntervalBESs;
	break;
#endif				       /* I_dsx1FarEndIntervalBESs */

#ifdef I_dsx1FarEndIntervalDMs
      case I_dsx1FarEndIntervalDMs:
	dp = &data->dsx1FarEndIntervalDMs;
	break;
#endif				       /* I_dsx1FarEndIntervalDMs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1FarEndTotalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1FarEndTotalEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1FarEndTotalEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dsx1FarEndTotalIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

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

    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1FarEndTotalIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1FarEndTotalEntry_get(serialNum, contextInfo, arg, searchType, dsx1FarEndTotalIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dsx1FarEndTotalIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1FarEndTotalIndex
      case I_dsx1FarEndTotalIndex:
	dp = &data->dsx1FarEndTotalIndex;
	break;
#endif				       /* I_dsx1FarEndTotalIndex */

#ifdef I_dsx1FarEndTotalESs
      case I_dsx1FarEndTotalESs:
	dp = &data->dsx1FarEndTotalESs;
	break;
#endif				       /* I_dsx1FarEndTotalESs */

#ifdef I_dsx1FarEndTotalSESs
      case I_dsx1FarEndTotalSESs:
	dp = &data->dsx1FarEndTotalSESs;
	break;
#endif				       /* I_dsx1FarEndTotalSESs */

#ifdef I_dsx1FarEndTotalSEFSs
      case I_dsx1FarEndTotalSEFSs:
	dp = &data->dsx1FarEndTotalSEFSs;
	break;
#endif				       /* I_dsx1FarEndTotalSEFSs */

#ifdef I_dsx1FarEndTotalUASs
      case I_dsx1FarEndTotalUASs:
	dp = &data->dsx1FarEndTotalUASs;
	break;
#endif				       /* I_dsx1FarEndTotalUASs */

#ifdef I_dsx1FarEndTotalCSSs
      case I_dsx1FarEndTotalCSSs:
	dp = &data->dsx1FarEndTotalCSSs;
	break;
#endif				       /* I_dsx1FarEndTotalCSSs */

#ifdef I_dsx1FarEndTotalLESs
      case I_dsx1FarEndTotalLESs:
	dp = &data->dsx1FarEndTotalLESs;
	break;
#endif				       /* I_dsx1FarEndTotalLESs */

#ifdef I_dsx1FarEndTotalPCVs
      case I_dsx1FarEndTotalPCVs:
	dp = &data->dsx1FarEndTotalPCVs;
	break;
#endif				       /* I_dsx1FarEndTotalPCVs */

#ifdef I_dsx1FarEndTotalBESs
      case I_dsx1FarEndTotalBESs:
	dp = &data->dsx1FarEndTotalBESs;
	break;
#endif				       /* I_dsx1FarEndTotalBESs */

#ifdef I_dsx1FarEndTotalDMs
      case I_dsx1FarEndTotalDMs:
	dp = &data->dsx1FarEndTotalDMs;
	break;
#endif				       /* I_dsx1FarEndTotalDMs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dsx1FracEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dsx1FracEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength;
    int             arg;
    void           *dp;
    dsx1FracEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dsx1FracIndex;
    long            dsx1FracNumber;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;

    /*
     * Check the object instance.
     * 
     * An EXACT search requires that the instance be of length 2
     * 
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 2) {
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &dsx1FracNumber, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1FracIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dsx1FracEntry_get(serialNum, contextInfo, arg, searchType, dsx1FracIndex, dsx1FracNumber)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dsx1FracIndex;
	inst.oid_ptr[1] = data->dsx1FracNumber;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dsx1FracIndex
      case I_dsx1FracIndex:
	dp = &data->dsx1FracIndex;
	break;
#endif				       /* I_dsx1FracIndex */

#ifdef I_dsx1FracNumber
      case I_dsx1FracNumber:
	dp = &data->dsx1FracNumber;
	break;
#endif				       /* I_dsx1FracNumber */

#ifdef I_dsx1FracIfIndex
      case I_dsx1FracIfIndex:
	dp = &data->dsx1FracIfIndex;
	break;
#endif				       /* I_dsx1FracIfIndex */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS

/*----------------------------------------------------------------------
 * Free the dsx1FracEntry data object.
 *---------------------------------------------------------------------*/
static void
dsx1FracEntry_free(
    dsx1FracEntry_t *data)
{
    if (data != NULL) {
	free((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after dsx1FracEntry set/undo
 *---------------------------------------------------------------------*/
static int
dsx1FracEntry_cleanup(
    doList_t       *trash)
{
    dsx1FracEntry_free(trash->data);
#ifdef SR_SNMPv2
    dsx1FracEntry_free(trash->undodata);
#endif				       /* SR_SNMPv2 */
    return NO_ERROR;
}

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
dsx1FracEntry_test(
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    int             instLength;
    doList_t       *dp;
    /*VarBind        *vb = (VarBind *) NULL;*/
    int             found;
    int             carry;
    dsx1FracEntry_t *dsx1FracEntry;
    long            dsx1FracIndex;
    long            dsx1FracNumber;

    instLength = incoming->length - object->oid.length;
    carry = 0;

    /*
     * Validate the object instance
     * 
     */
    if (instLength != 2) {
	return NO_CREATION_ERROR;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dsx1FracIndex, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &dsx1FracNumber, EXACT, &carry)) < 0) {
	return (NO_CREATION_ERROR);
    }
    dsx1FracEntry = k_dsx1FracEntry_get(-1, contextInfo, object->nominator, EXACT, dsx1FracIndex, dsx1FracNumber);

#ifndef dsx1FracEntry_READ_CREATE

    if (dsx1FracEntry == NULL) {
	return (NO_ACCESS_ERROR);
    }
#endif				       /* dsx1FracEntry_READ_CREATE */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) {
	if ((dp->setMethod == dsx1FracEntry_set) &&
	    (((dsx1FracEntry_t *) (dp->data)) != NULL) &&
	(((dsx1FracEntry_t *) (dp->data))->dsx1FracIndex == dsx1FracIndex) &&
	    (((dsx1FracEntry_t *) (dp->data))->dsx1FracNumber == dsx1FracNumber)) {

	    found = 1;
	    break;
	}
    }

    if (!found) {
	dp = doCur;
	if ((dp->data = malloc(sizeof(dsx1FracEntry_t))) == NULL) {
	    DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
	    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	memset(dp->data, 0, sizeof(dsx1FracEntry_t));

	dp->setMethod = dsx1FracEntry_set;
	dp->cleanupMethod = dsx1FracEntry_cleanup;
#ifdef SR_SNMPv2
	dp->undoMethod = NULL;
#endif				       /* SR_SNMPv2 */
	dp->state = UNKNOWN;

	SET_VALID(I_dsx1FracIndex, ((dsx1FracEntry_t *) (dp->data))->valid);
	((dsx1FracEntry_t *) (dp->data))->dsx1FracIndex = dsx1FracIndex;

	SET_VALID(I_dsx1FracNumber, ((dsx1FracEntry_t *) (dp->data))->valid);
	((dsx1FracEntry_t *) (dp->data))->dsx1FracNumber = dsx1FracNumber;

#ifdef dsx1FracEntry_READ_CREATE

	/*
	 * Try to fill in reasonable default values for this new entry.
	 */

	if (dsx1FracEntry != NULL) {
	    /* use existing values as default values */
	    memcpy(dp->data, dsx1FracEntry, sizeof(dsx1FracEntry_t));

	    SET_ALL_VALID(((dsx1FracEntry_t *) (dp->data))->valid);

	} else {


	    /* Fill in default values here */


	}
#endif				       /* dsx1FracEntry_READ_CREATE */
    }
    switch (object->nominator) {

#ifdef I_dsx1FracIfIndex
      case I_dsx1FracIfIndex:

	SET_VALID(I_dsx1FracIfIndex, ((dsx1FracEntry_t *) (dp->data))->valid);

	((dsx1FracEntry_t *) (dp->data))->dsx1FracIfIndex = value->sl_value;
	break;
#endif				       /* I_dsx1FracIfIndex */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in dsx1FracEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */
    dp->state = ADD_MODIFY;

    return (NO_ERROR);

}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
dsx1FracEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
    return (k_dsx1FracEntry_set((dsx1FracEntry_t *) (doCur->data),
				contextInfo, doCur->state));
}

#endif				       /* SETS */




dsx1ConfigEntry_t *
k_dsx1ConfigEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1LineIndex)
{
    static dsx1ConfigEntry_t dsx1ConfigEntryData;
    dsx1MIBDataTable *mibdata;
    static OctetString dsx1CircuitIdentifier;

    mibdata = snmp_get_dsx(searchType, &dsx1LineIndex);
    if (mibdata == NULL)
	return(NULL);

    dsx1ConfigEntryData.dsx1LineIndex = dsx1LineIndex;
    dsx1ConfigEntryData.dsx1IfIndex = dsx1LineIndex;	/* not implemented */
    dsx1ConfigEntryData.dsx1TimeElapsed = mibdata->C.dsx1TimeElapsed;
    dsx1ConfigEntryData.dsx1ValidIntervals = mibdata->C.dsx1ValidIntervals;
    dsx1ConfigEntryData.dsx1LineType = mibdata->C.dsx1LineType;
    dsx1ConfigEntryData.dsx1LineCoding = mibdata->C.dsx1LineCoding;
    dsx1ConfigEntryData.dsx1SendCode = mibdata->C.dsx1SendCode;

    dsx1ConfigEntryData.dsx1CircuitIdentifier = &dsx1CircuitIdentifier;
    dsx1ConfigEntryData.dsx1CircuitIdentifier->octet_ptr = &mibdata->C.dsx1CircuitIdentifier[0];
    dsx1ConfigEntryData.dsx1CircuitIdentifier->length = 1;

    dsx1ConfigEntryData.dsx1LoopbackConfig = mibdata->C.dsx1LoopbackConfig;
    dsx1ConfigEntryData.dsx1LineStatus = mibdata->C.dsx1LineStatus;
    dsx1ConfigEntryData.dsx1SignalMode = mibdata->C.dsx1SignalMode;
    dsx1ConfigEntryData.dsx1TransmitClockSource = mibdata->C.dsx1TransmitClockSource;
    dsx1ConfigEntryData.dsx1Fdl = mibdata->C.dsx1Fdl;

    return (&dsx1ConfigEntryData);
}

#ifdef SETS
int
k_dsx1ConfigEntry_set(
    dsx1ConfigEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{
    /*
    currently, we are not implementing any of the sets or table row creation 

    dsx1MIBDataTable *mibdata;

    mibdata = snmp_get_dsx(EXACT, &data->dsx1LineIndex);
    if (mibdata == NULL)
	return(GEN_ERROR);
    */
	
    return (GEN_ERROR);
}

#endif				       /* SETS */

dsx1CurrentEntry_t *
k_dsx1CurrentEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1CurrentIndex)
{
    static dsx1CurrentEntry_t dsx1CurrentEntryData;
    dsx1MIBDataTable *mibdata;

    mibdata = snmp_get_dsx(searchType, &dsx1CurrentIndex);
    if (mibdata == NULL)
	return(NULL);

    dsx1CurrentEntryData.dsx1CurrentIndex = dsx1CurrentIndex;
    dsx1CurrentEntryData.dsx1CurrentESs = mibdata->CurrentData.dsx1ESs;
    dsx1CurrentEntryData.dsx1CurrentSESs = mibdata->CurrentData.dsx1SESs;
    dsx1CurrentEntryData.dsx1CurrentSEFSs = mibdata->CurrentData.dsx1SEFSs;
    dsx1CurrentEntryData.dsx1CurrentUASs = mibdata->CurrentData.dsx1UASs;
    dsx1CurrentEntryData.dsx1CurrentCSSs = mibdata->CurrentData.dsx1CSSs;
    dsx1CurrentEntryData.dsx1CurrentPCVs = mibdata->CurrentData.dsx1PCVs;
    dsx1CurrentEntryData.dsx1CurrentLESs = mibdata->CurrentData.dsx1LESs;
    dsx1CurrentEntryData.dsx1CurrentBESs = mibdata->CurrentData.dsx1BESs;
    dsx1CurrentEntryData.dsx1CurrentDMs = mibdata->CurrentData.dsx1DMs;
    dsx1CurrentEntryData.dsx1CurrentLCVs = mibdata->CurrentData.dsx1LCVs;

    return (&dsx1CurrentEntryData);
}

dsx1IntervalEntry_t *
k_dsx1IntervalEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1IntervalIndex,
    long            dsx1IntervalNumber)
{
    static dsx1IntervalEntry_t dsx1IntervalEntryData;
    dsx1MIBDataTable *mibdata;

    mibdata = snmp_get_dsx_interval(searchType, &dsx1IntervalIndex, &dsx1IntervalNumber);
    if (mibdata == NULL)
	return(NULL);

    dsx1IntervalEntryData.dsx1IntervalIndex = dsx1IntervalIndex;
    dsx1IntervalEntryData.dsx1IntervalNumber = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1Interval;
    dsx1IntervalEntryData.dsx1IntervalESs =  mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1ESs;
    dsx1IntervalEntryData.dsx1IntervalSESs =  mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1SESs;
    dsx1IntervalEntryData.dsx1IntervalSEFSs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1SEFSs;
    dsx1IntervalEntryData.dsx1IntervalUASs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1UASs;
    dsx1IntervalEntryData.dsx1IntervalCSSs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1CSSs;
    dsx1IntervalEntryData.dsx1IntervalPCVs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1PCVs;
    dsx1IntervalEntryData.dsx1IntervalLESs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1LESs; 
    dsx1IntervalEntryData.dsx1IntervalBESs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1BESs;
    dsx1IntervalEntryData.dsx1IntervalDMs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1DMs;
    dsx1IntervalEntryData.dsx1IntervalLCVs = mibdata->Intervals[(QUARTER_HOURS_IN_DAY + 
	mibdata->CurrentIndex - dsx1IntervalNumber) % QUARTER_HOURS_IN_DAY].dsx1LCVs;

    return (&dsx1IntervalEntryData);
}

dsx1TotalEntry_t *
k_dsx1TotalEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1TotalIndex)
{
    static dsx1TotalEntry_t dsx1TotalEntryData;
    dsx1MIBDataTable *mibdata;

    mibdata = snmp_get_dsx(searchType, &dsx1TotalIndex);
    if (mibdata == NULL)
	return(NULL);

    dsx1TotalEntryData.dsx1TotalIndex = dsx1TotalIndex;
    dsx1TotalEntryData.dsx1TotalESs = mibdata->TotalData.dsx1ESs;
    dsx1TotalEntryData.dsx1TotalSESs = mibdata->TotalData.dsx1SESs;
    dsx1TotalEntryData.dsx1TotalSEFSs = mibdata->TotalData.dsx1SEFSs;
    dsx1TotalEntryData.dsx1TotalUASs = mibdata->TotalData.dsx1UASs;
    dsx1TotalEntryData.dsx1TotalCSSs = mibdata->TotalData.dsx1CSSs;
    dsx1TotalEntryData.dsx1TotalPCVs = mibdata->TotalData.dsx1PCVs;
    dsx1TotalEntryData.dsx1TotalLESs = mibdata->TotalData.dsx1LESs;
    dsx1TotalEntryData.dsx1TotalBESs = mibdata->TotalData.dsx1BESs;
    dsx1TotalEntryData.dsx1TotalDMs = mibdata->TotalData.dsx1DMs;
    dsx1TotalEntryData.dsx1TotalLCVs = mibdata->TotalData.dsx1LCVs;

    return (&dsx1TotalEntryData);
}

dsx1FarEndCurrentEntry_t *
k_dsx1FarEndCurrentEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1FarEndCurrentIndex)
{
#ifdef NOT_YET			/* this would seem to be unsupported table */
    static dsx1FarEndCurrentEntry_t dsx1FarEndCurrentEntryData;

    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentIndex =;
    dsx1FarEndCurrentEntryData.dsx1FarEndTimeElapsed =;
    dsx1FarEndCurrentEntryData.dsx1FarEndValidIntervals =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentESs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentSESs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentSEFSs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentUASs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentCSSs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentLESs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentPCVs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentBESs =;
    dsx1FarEndCurrentEntryData.dsx1FarEndCurrentDMs =;

    return (&dsx1FarEndCurrentEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}

dsx1FarEndIntervalEntry_t *
k_dsx1FarEndIntervalEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1FarEndIntervalIndex,
    long            dsx1FarEndIntervalNumber)
{
#ifdef NOT_YET		/* this would seem to be unsupported table */
    static dsx1FarEndIntervalEntry_t dsx1FarEndIntervalEntryData;

    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalIndex =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalNumber =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalESs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalSESs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalSEFSs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalUASs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalCSSs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalLESs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalPCVs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalBESs =;
    dsx1FarEndIntervalEntryData.dsx1FarEndIntervalDMs =;

    return (&dsx1FarEndIntervalEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}

dsx1FarEndTotalEntry_t *
k_dsx1FarEndTotalEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1FarEndTotalIndex)
{
#ifdef NOT_YET		/* this would seem to be unsupported table */
    static dsx1FarEndTotalEntry_t dsx1FarEndTotalEntryData;

    dsx1FarEndTotalEntryData.dsx1FarEndTotalIndex =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalESs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalSESs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalSEFSs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalUASs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalCSSs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalLESs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalPCVs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalBESs =;
    dsx1FarEndTotalEntryData.dsx1FarEndTotalDMs =;

    return (&dsx1FarEndTotalEntryData);
#else				       /* NOT_YET */
    return (NULL);
#endif				       /* NOT_YET */
}

dsx1FracEntry_t *
k_dsx1FracEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dsx1FracIndex,
    long            dsx1FracNumber)
{
    static dsx1FracEntry_t dsx1FracEntryData;
    hwidbtype *hwidb;

    hwidb = snmp_get_dsx_idb(searchType, &dsx1FracIndex, &dsx1FracNumber);
    if (hwidb == NULL)
	return(NULL);

    dsx1FracEntryData.dsx1FracIndex = dsx1FracIndex;
    dsx1FracEntryData.dsx1FracNumber = dsx1FracNumber;
    dsx1FracEntryData.dsx1FracIfIndex = hwidb->snmp_if_index;

    return (&dsx1FracEntryData);
}

#ifdef SETS
int
k_dsx1FracEntry_set(
    dsx1FracEntry_t *data,
    ContextInfo    *contextInfo,
    int             function)
{

    return (GEN_ERROR);
}
#endif				       /* SETS */


static dsx1MIBDataTable *
snmp_get_dsx(int searchType, long *index)
{
    return (dsx1_find_SNMP(index, (searchType == NEXT)) );
}


static dsx1MIBDataTable *
snmp_get_dsx_interval(int searchType, long *index, long *interval)
{
    dsx1MIBDataTable *mibdata;

    if (searchType != NEXT)
	return (dsx1_find_SNMP(index, FALSE));

    if (*interval == 0)
	*interval = 1;			/* need to get off 0 instance */

    if (*interval > QUARTER_HOURS_IN_DAY) {
	*interval = 1;
	(*index)++;
	if (*index == 0)
	    return(NULL);
    }

    while (1) {
	mibdata = dsx1_find_SNMP(index, TRUE);
	if (mibdata == NULL)
	    return(NULL);

	/* All performance parameters are accumulated in fifteen minute
	 * intervals and up to 96 intervals (24 hours worth) are kept by an
	 * agent.  Fewer than 96 intervals of data will be available if the
	 * agent has been restarted within the last 24 hours.
	 *
	 * dsx1Interval will equal zero in this case.
	 */

	if (mibdata->Intervals[(QUARTER_HOURS_IN_DAY + mibdata->CurrentIndex
		- *interval) % QUARTER_HOURS_IN_DAY].dsx1Interval != 0)
	    return(mibdata);
        *interval = 1;
        (*index)++;
    }
}


static hwidbtype *
snmp_get_dsx_idb(int searchType, long *index, long *interval)
{
    cdbtype *cdb;
    
    dsx1info *dsx1;
    int i, chan;

    if (searchType != NEXT) {
	if (*interval == 0 || *interval > E1_MAXTS)
	    return(NULL);

        if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_T1, FALSE))
                    == NULL) {
             if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_E1, FALSE))
                         == NULL) {
	          return(NULL);
             }
        }

	dsx1 = dsx1_find_by_unit(cdb->unit);

	/* get the channel number associated wit the requested time slot */
	chan = dsx1->timeslots[*interval];

        /* find the idb this channel uses */
	if (chan != UNUSED_CHAN) {
	    if (dsx1->hwidb[chan] && (dsx1->hwidb[chan]->vc == chan))
		return(dsx1->hwidb[chan]);
	}
	return(NULL);
    }

    if (*interval > E1_MAXTS) {
	(*index)++;
	*interval = 1;
    }

    while (1) {
	/* find the requested ds1 */
        if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_T1, TRUE))
                    == NULL) {
             if ((cdb = controller_find_for_SNMP_hw_if(index, PIFT_E1, TRUE))
                         == NULL) {
	          return(NULL);
             }
        }

	dsx1 = dsx1_find_by_unit(cdb->unit);

	for (i= *interval; i<= E1_MAXTS; i++) {
	    /* from the timeslot, get the channel # */
	    chan = dsx1->timeslots[i];
	    if (chan != UNUSED_CHAN) {
		if (dsx1->hwidb[chan] && (dsx1->hwidb[chan]->vc == chan)) {
		    *interval = i;
		    return (dsx1->hwidb[chan]);
                }
	    }
	}
	/* nothing found in this ds1 advance to next one */
        *interval = 1;
        (*index)++;
    }
}


static void ds1_add_all_if (void)
{
    cdbtype  *cdb;

    FOR_ALL_CDBS(cdb) {
	if ((cdb->pif_type == PIFT_T1) || (cdb->pif_type == PIFT_E1)) {
	    ds1_add_if(cdb);
	}
    }
}

static void ds1_add_if (cdbtype *cdb)
{
   snmpifptrtype if_ptr;
   ulong state;
   snmpidbtype *snmpidb;
   subiabtype  *subiab;
   boolean result;

   snmpidb = NULL;

   if((cdb == NULL) || (cdb->cdb_state & IDBS_DELETED)) {
	return;
    }

    if (cdb->snmp_if_index) 
	snmpidb =  reg_invoke_ifmib_snmpidb_get(NULL, 0,EXACT,cdb->snmp_if_index,NULL,NULL);

    if (snmpidb != NULL)
	return;

   subiab = malloc(sizeof(subiabtype));
   if (!subiab)
       return;

   subiab->master_ptr.ifptr.cdb = cdb;
   subiab->master_type = SNMP_IF_CDB;
   subiab->data_ptr = NULL;
   subiab->cleaner = NULL;

   if_ptr.ifptr.subiab = subiab;

   state = reg_invoke_ifmib_get_adminstatus(D_ifType_ds1, subiab);
    
    subiab->snmp_if_index = 0; 	/*uint  ifIndex */
    subiab->if_descrstring = cdb->cdb_namestring;
    subiab->if_name = cdb->cdb_namestring;
    subiab->ifPhysAddr = 0;
    subiab->ifPhysAddrLen = NULL; 
    subiab->ifType = D_ifType_ds1;
    subiab->maxmtu = NULL;           /* not applicable */

    result = reg_invoke_ifmib_register_subif(&if_ptr, SNMP_IF_SUBIAB, state);
    if (result != TRUE)
	free(subiab);
}

static void ds1_add_cdb_index ( 
      cdbtype * cdb, 
      ulong ifIndex)
{

    /*
     * this is normally done in the ifmib_register_subif() function,
     * but not in this case, since can't include platform dependent
     * structure (cdbtype in if/if_controller.h) in platform
     * independent snmp/ifmibapi.c
     */
    if (cdb != NULL) {
	cdb->snmp_if_index = ifIndex;
    }
}

static void ds1_update_ifAlias ( 
      subiabtype  *subiab,
      OctetString *ifAlias,
      boolean set)				    
{

    cdbtype *cdb;
    char *tmp, *tmp2;

    cdb = subiab->master_ptr.ifptr.cdb;
    if ((cdb != NULL) && (ifAlias != NULL)) {
	if (set == TRUE) {
	    tmp = malloc(ifAlias->length + 1);
	    if (tmp) {
		memcpy(tmp,ifAlias->octet_ptr,ifAlias->length);
		tmp[ifAlias->length] = '\0';
		tmp2 = cdb->cdb_description;
		cdb->cdb_description = tmp;	   
		if (tmp2 != NULL) {
		    free(tmp2);
		}
	    }
	} else {
	    ifAlias->octet_ptr = cdb->cdb_description;
	    ifAlias->length = strlen(cdb->cdb_description);
	}
    }
}

static boolean ds1_admin_change (
	  enum SNMP_ADMIN_STATE	newStatus,
	  boolean		testPhase,
	  const	subiabtype      *subiab)

/*
 * support function for changing ifAdminStatus 
 *
 * LOG-IF-MIB ifAdminStatus change callback function.
 * INPUTS:
 *     newStatus == requested new value of ifAdminStatus
 *		     this must be different than the current value
 *     testPhase == TRUE if this the value is being checked
 *                   in the test phase of PDU processing
 *		     FALSE if the change should take effect now
 *     if_ptr == r/o pointer to IDB for this interface
 * RETURNS:
 *   TRUE if the change is allowed by LOG-IF-MIB
 *       if testPhase == TRUE:
 *          upon exit, ifAdminStatus has not changed.       
 *       if testPhase == FALSE:
 *	    upon exit, the new value of ifAdminStatus is in effect
 *  FALSE if LOG-IF-MIB is rejecting the requested change
 *       upon exit, ifAdminStatus has not changed.
 */ 

{
    cdbtype *cdb;
    dsx1info    *dsx1;
    uchar newstate, oldstate;

    cdb = subiab->master_ptr.ifptr.cdb;
    if (cdb == NULL)
        return FALSE;

    /* CSCdi83944. If dsx1 can not be found or dsx1 loop is set to 
     * DSX1_LOOP_REMOTE then function should return with FALSE. 
     * Since new state has been already set, old state must be 
     * recovered before return.
     */
    oldstate = cdb->cdb_state;

   /*
    * convert to subif state enum
    */
    switch ( newStatus) {
    case D_ifAdminStatus_down:
	newstate = IDBS_ADMINDOWN;
	break;
    case D_ifAdminStatus_up:
	newstate = IDBS_DOWN;
	break;
    default:
	return FALSE;

    }

    if (!testPhase) {
	cdb->cdb_state = newstate;
        if (newstate == IDBS_DOWN) {
	    cdb->cdb_status &= ~CDB_CRASHED;
	}
	/* The following code is added for bug CSCdi83944. The code here is
	 * similar to portion of code in t1ctrlr_command(), that handles
	 * T1 shutdown from CLI. 
	 */
	if (cdb->pif_type == PIFT_T1) {
	    if (newstate == IDBS_ADMINDOWN) {
                dsx1 = dsx1_find_by_unit(cdb->unit);
		if (dsx1 == NULL) {
		    cdb->cdb_state = oldstate;
	            return FALSE;
		}
	        if (dsx1->loop == DSX1_LOOP_REMOTE) {
                    printf("\n%%Must remove remote loop before
			        shuting down controller T1");
		    cdb->cdb_state = oldstate;
                    return FALSE;
                }
	        dsx1_change_all_idbs_state(cdb, IDBS_DOWN);
	    }
            if (cdb->reset)
                (*cdb->reset)(cdb);
	}
    }

    return TRUE;
}

static enum SNMP_ADMIN_STATE ds1_get_adminstatus (const subiabtype *subiab)
{

    cdbtype *cdb;

    cdb = subiab->master_ptr.ifptr.cdb;
    if (!cdb)
	return (SNMP_ADMIN_UNKNOWN);   /* state is really unkown...*/

    switch (cdb->cdb_state) {

    case IDBS_UP: 
    case IDBS_DOWN:
	return (SNMP_ADMIN_UP);

    case IDBS_ADMINDOWN:
    default:	
	return(SNMP_ADMIN_DOWN);
    }
}

static enum SNMP_ADMIN_STATE ds1_get_operstatus (const subiabtype *subiab)
{

    cdbtype *cdb;

    cdb = subiab->master_ptr.ifptr.cdb;

    if (!cdb)
	return (SNMP_ADMIN_UNKNOWN);   /* state is really unkown...*/

    switch (cdb->cdb_state) {

    case IDBS_TESTING: 
	return(SNMP_ADMIN_TESTING);

    case IDBS_UP: 
	return (SNMP_ADMIN_UP);

    case IDBS_ADMINDOWN:
    case IDBS_DOWN:
    default:	
	return(SNMP_ADMIN_DOWN);
    }
}

static ulong ds1_get_if_speed (const subiabtype *subiab)
{

    if (subiab->master_type != SNMP_IF_CDB)
	return 0;

    return ((subiab->master_ptr.ifptr.cdb->pif_type == PIFT_T1) ?
	   IF_T1_SPEED : IF_E1_SPEED);
}


/*
 * ds1mib subsystem header
 */

#define MAJVERSION_ds1mib 1
#define MINVERSION_ds1mib 0
#define EDITVERSION_ds1mib 0

SUBSYS_HEADER(ds1mib,
              MAJVERSION_ds1mib,
              MINVERSION_ds1mib,
              EDITVERSION_ds1mib,
              init_ds1mib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
