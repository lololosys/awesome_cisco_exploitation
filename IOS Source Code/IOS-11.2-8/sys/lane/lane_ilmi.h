/* $Id: lane_ilmi.h,v 3.2 1995/11/17 17:50:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lane/lane_ilmi.h,v $
 *------------------------------------------------------------------
 * lane_ilmi.h: LAN Emulation access to ATM ILMI
 *
 * January 1995, Norm Finn
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_ilmi.h,v $
 * Revision 3.2  1995/11/17  17:50:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:22:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  22:34:53  cakyol
 * CSCdi37207:  auto addresses should be displayed without running the
 * components
 * - fixed.  The interface/subinterface command "lane
 * default-atm-addresses"
 *   now displays all would be auto addresses for all the lane components
 *   on the specific interface.
 *
 * Revision 2.1  1995/06/07  21:19:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _LANE_ILMI_H_
#define _LANE_ILMI_H_

#define PREFIX_SIZE	13


#define SET_ADDRESS_OID_LEN		34

/* where in the oid the atm address data actually starts */
#define START_OF_ATM_ADDRESS_INDEX	(SET_ADDRESS_OID_LEN - \
						STATIONLEN_ATMNSAP)

/* mib values for address add and delete */
#define ILMI_ADD_ADDR			1
#define ILMI_DELETE_ADDR		2

/*
 * OIDs
 */
#define atmForum		1, 3, 6, 1, 4, 1, 353
 
#define atmForumAdmin		atmForum, 1
#define atmfSrvcRegTypes	atmForumAdmin, 5
#define atmfSrvcRegLecs		atmfSrvcRegTypes, 1
 
#define atmForumUni		atmForum, 2
#define atmfSrvcRegistryGroup	atmForumUni, 8
#define atmfSrvcRegTable	atmfSrvcRegistryGroup, 1
#define atmfSrvcRegEntry	atmfSrvcRegTable, 1
#define atmfSrvcRegATMAddress	atmfSrvcRegEntry, 3
#define OID_LENGTH(x) (sizeof((ulong[]){x})/sizeof(ulong))


/*
 * codes returned from "lane_ilmi_compute_atmsap" below
 */
typedef enum lane_ilmi_compback_t_ 
{
    LANE_ILMI_COMPUTE_SUCCESS,
    LANE_ILMI_COMPUTE_BAD_INPUTS,
    LANE_ILMI_COMPUTE_NO_PREFIX

} lane_ilmi_compback_t;

/*
 * codes used in definining which entity address is computed for.
 * If you change these MAKE sure that you change "LANE_ENTITY_LOW"
 * and "LANE_ENTITY_HIGH" to correspond to the correct values.
 */
typedef enum lane_ilmi_entity_t_ 
{
    LANE_ILMI_LE_CLIENT,
    LANE_ILMI_LE_SERVER,
    LANE_ILMI_BUS,
    LANE_ILMI_CONFIG_SERVER,

} lane_ilmi_entity_t;
#define LANE_ENTITY_LOW		LANE_ILMI_LE_CLIENT
#define LANE_ENTITY_HIGH	LANE_ILMI_CONFIG_SERVER

/*
 * used for address registration and deregistration with ILMI
 */
extern VarBind IlmiSetAddressVarBind;

/*
 * gets the ilmi prefix 
 */
extern boolean getIlmiPrefix (hwidbtype*, uchar *prefix);

/*
 * Compute an ATMSAP based on the sub interface, entity, and
 * a masked address.  Never returns LANE_ILMI_WILL_CALLBACK.
 */
extern lane_ilmi_compback_t lane_ilmi_compute_atmsap
    (idbtype *swidb, lane_ilmi_entity_t, 
     MaskedAddress *maddr, hwaddrtype *result);

/*
 * convert the error code to text
 */
extern char *lane_ilmi_error_text (lane_ilmi_compback_t err);

#endif _LANE_ILMI_H_

