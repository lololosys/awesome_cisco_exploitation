/* $Id: lane_ilmi.c,v 3.2.62.3 1996/05/09 14:32:25 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_ilmi.c,v $
 *------------------------------------------------------------------
 * lane_ilmi.c: LAN Emulation interface to ATM ILMI
 *
 * January 1995, Norm Finn
 *
 * Copyright (c) 1995-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_ilmi.c,v $
 * Revision 3.2.62.3  1996/05/09  14:32:25  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.62.2.2.1  1996/04/27  07:06:20  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.62.2  1996/04/25  23:14:19  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.62.1.8.2  1996/04/08  14:51:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.62.1.8.1  1996/04/08  01:56:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.1  1996/03/18  20:32:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.66.2  1996/03/29  05:23:35  cakyol
 * get rid of the silly and redundant OR statement.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.66.1  1996/03/22  22:54:11  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/05  06:42:48  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.40.1  1996/02/27  21:10:09  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/02/13  00:17:53  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.2  1995/11/17  17:50:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  00:24:34  cakyol
 * CSCdi37287:  show lane default-atm-addresses should handle no prfix case
 * - fixed
 *
 * Revision 2.1  1995/06/07  21:19:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "interface_private.h"
#include "address.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "../atm/ilmi_api.h"
#include "../ui/debug.h"
#include "lane_debug.h"
#include "lane_ilmi.h"

static uchar lane_zero_sap [STATIONLEN_ATMNSAP] = {0};

/*****************************************************************
**		used for ILMI adddress registration
*****************************************************************/

/* fixed part of the oid */
static ulong addressValidateOidSequence [SET_ADDRESS_OID_LEN] = 
{
    /* this is always the same */
    1, 3, 6, 1, 4, 1, 353, 2, 6, 1, 1, 3, 0, 20,

    /* this is the actual nsap */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static OID IlmiSetAddressOid = 
{
    length:	SET_ADDRESS_OID_LEN,
    oid_ptr:	&addressValidateOidSequence [0]
};

VarBind IlmiSetAddressVarBind = 
{
    data_length:	0, 
    name:		&IlmiSetAddressOid, 
    name_size:		0, 
    value:		{ type:	INTEGER_TYPE, { slval: 0 }},
    value_size:		0, 
    next_var:		NULL
};

/*****************************************************************/

boolean getIlmiPrefix (hwidbtype *hwidb, uchar *prefix)
{
    OctetString *os;
    int len;

    bzero(prefix, PREFIX_SIZE);
    if (!hwidb) return FALSE;
    os = ilmi_addrtraverse(hwidb->hw_if_index, NULL);
    if (!os) return FALSE;
    len = os->length;
    if ((len < 0) || (len > PREFIX_SIZE)) return FALSE;
    if (!os->octet_ptr) return FALSE;
    bcopy(os->octet_ptr, prefix, len);
    return TRUE;
}

char *lane_ilmi_error_text (lane_ilmi_compback_t err)
{
    switch(err)
    {
	case LANE_ILMI_COMPUTE_BAD_INPUTS: 
	    return "Bad Inputs";

	case LANE_ILMI_COMPUTE_NO_PREFIX: 
	    return "ATM Switch Prefix is not available";

	case LANE_ILMI_COMPUTE_SUCCESS: 
	    return "No error";

	default:
	    return "UnExpected Error Code";
    }
}

/*
** compute an nsap given the swidb, the LANE component 
** asking and a masked address and return it in result.
*/
lane_ilmi_compback_t lane_ilmi_compute_atmsap (idbtype *swidb, 
    lane_ilmi_entity_t entity, MaskedAddress *maskedAddr, 
    hwaddrtype *result)
{
    hwidbtype *hwidb;
    int subIfNumber;
    uchar prefix [PREFIX_SIZE];
    uchar autoValues [STATIONLEN_ATMNSAP];
    uchar macAddress [6];
    uchar *baseMacAddress;
    uchar *atmsap, *mask;
    int i, j;

    /* sanity checks, these should NOT happen */
    if ((swidb == NULL) || (maskedAddr == NULL) || (result == NULL))
	return LANE_ILMI_COMPUTE_BAD_INPUTS;
    hwidb = hwidb_or_null(swidb);
    if (hwidb == NULL)
	return LANE_ILMI_COMPUTE_BAD_INPUTS;

    /* record the data & mask */
    if ((maskedAddr->type == STATION_ILLEGAL) || 
	(maskedAddr->length <= 0))
    {
	atmsap = mask = lane_zero_sap;
    }
    else
    {
	atmsap = maskedAddr->dataBytes;
	mask   = maskedAddr->maskBytes;
    }

    /*
    ** depending on the entity type, determine the mac 
    ** address AND the sub interface number to be used.  
    ** The sub interface number is used for all components 
    ** EXCEPT the lecs, which uses zero.
    */
    subIfNumber = swidb->sub_number;

    /*
    ** Note that if "mac-address" command has been entered, this
    ** means that "forced_mac" is set and therefore we should
    ** use the mac address from there, otherwise we should take
    ** it from the burnt in address "bia".
    */
    if (bcmp(hwidb->forced_mac, lane_zero_sap, 6) == 0) {
	baseMacAddress = hwidb->bia;
    } else {
	baseMacAddress = hwidb->forced_mac;
    }

    switch (entity)
    {
	case LANE_ILMI_LE_CLIENT:
	    increment_macaddress_by(baseMacAddress, macAddress, 0);
	    break;

	case LANE_ILMI_LE_SERVER:
	    increment_macaddress_by(baseMacAddress, macAddress, 1);
	    break;

	case LANE_ILMI_BUS:
	    increment_macaddress_by(baseMacAddress, macAddress, 2);
	    break;

	case LANE_ILMI_CONFIG_SERVER:
	    subIfNumber = 0;
	    increment_macaddress_by(baseMacAddress, macAddress, 3);
	    break;

	default:
	    return LANE_ILMI_COMPUTE_BAD_INPUTS;
    }

    /* prefix MAY be needed */
    if (!allOnesMask(mask, PREFIX_SIZE))
    {
	/* if we are here, then prefix is DEFINITELY needed */
	if (!getIlmiPrefix(hwidb, prefix))
	    return LANE_ILMI_COMPUTE_NO_PREFIX;

	for (i=0; i<PREFIX_SIZE; i++) 
	    autoValues [i] = prefix [i];
    }
    for (i=0, j=PREFIX_SIZE; i<STATIONLEN_IEEE48; i++, j++) 
	autoValues [j] = macAddress [i];
    autoValues [PREFIX_SIZE+STATIONLEN_IEEE48] = subIfNumber;

    /*
    ** copy from autoValues OR source to result, 
    ** taking mask into account 
    */
    for (i=0; i<STATIONLEN_ATMNSAP; i++)
	result->addr [i] = 
	   (atmsap [i] & mask [i]) | 
	   (autoValues [i] & ~mask [i]);

    /* set the hwaddrtype fields properly */
    result->type = STATION_ATMNSAP;
    result->length = STATIONLEN_ATMNSAP;
    result->target_type = TARGET_UNICAST;

    /* everything went ok */
    return LANE_ILMI_COMPUTE_SUCCESS;
}

