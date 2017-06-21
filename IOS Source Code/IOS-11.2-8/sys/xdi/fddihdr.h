/* $Id: fddihdr.h,v 3.3 1996/01/22 09:06:59 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xdi/fddihdr.h,v $
 *------------------------------------------------------------------
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddihdr.h,v $
 * Revision 3.3  1996/01/22  09:06:59  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  18:10:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:55:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:25:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_fddihdr.h xm7_v3.1.1:cset.0166:11:11 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
	Station Management Header File

	FDDI Header File

	File:			xm7_fddihdr.h
	Created:		12/01/89

	Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
	Last Modified:	cset.0166		01/10/94

	Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

	This is the header file contains all definitions and constant
	values specified in the FDDI SMT documentation.

	Modification History:
	*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
*       General Constant Definitions
*********************************************************************/

/*
*       Boolean values
*/
#ifndef FALSE
#define FALSE   		((Flag) 0)
#endif							/* FALSE */

#ifndef TRUE
#define TRUE    		((Flag) 1)
#endif							/* TRUE */

#ifndef CLEAR
#define CLEAR   		((Flag) 0)
#endif							/* CLEAR */

#ifndef SET
#define SET     		((Flag) 1)
#endif							/* SET */

#ifndef NULL
#define NULL    		((void *) 0)
#endif							/* NULL */

#define DISABLE 		FALSE
#define ENABLE  		TRUE

#define INSERTED		TRUE
#define DEINSERTED		FALSE

#define	NOTPRESENT		((Flag) 0)
#define	PRESENT			((Flag) 1)

/*
*       XDI's IEEE OUI  00 02 F4
*       This information is used for XDI's vendor-specific MIB attributes.
*       The OUI is presented in MSB order.
*/
#define XDI_OUI_VALUE   ((uInt32) 0x000002F4)	/* numeric version */
#define XDI_OUI_STRING  "\000\002\364"	/* string version */


/*
*	For single-station to multistation conversion routines, the
*	default value for the station ID is given by the following
*	constant, it is only used as a placeholder in the parameter
*	sequence.
*/
#define FIRST_STATION   	((uInt16) 0)


/*********************************************************************
	Timer Values
*********************************************************************/

/*
*	These time values are specified in the FDDI SMT Connection Management
*	Facilities documentation. For convenience in handling time values, all
*	times are expressed in microseconds (usecs).
*/

/*	Timer Calculation Elements */
#define	D_Max			(uTime) 1773

/*	ECM Expiration Values */
#define	I_Max			(uTime) 25000	/* default */
#define	IN_Max			(uTime) 40000	/* default */
#define	TD_Min			(uTime) 5000	/* default */
/*	Trace_Max	defined by fddiSMTTrace-MaxExpiration */
#define	Trace_Max_Default	(uTime) 7000000	/* default */
#define	Min_Trace_Max		(uTime) 6001773

/* PCM Expiration Values */
#define	C_Min			(uTime) 1600	/* default */
/* #define	TL_Min			(uTime) 50 */	/* default */
#define TL_Min                  ((uTime) STADATA (stnID, TLMin))
/*	LS_Min	0.48 usecs must be handled by hardware */
#define	TB_Min			(uTime) 5000	/* default */
#define	TB_Max			(uTime) 50000	/* default */
#define	T_Out			(uTime) 100000	/* default */
#define	NS_Max			(uTime) 1300	/* default */
#define	LC_Short		(uTime) 50000	/* default */
#define	LC_Medium		(uTime) 500000	/* default */
#define	LC_Long			(uTime) 5000000	/* default */
#define	LC_Extended		(uTime) 50000000/* default */

/* CFM Expiration Values */
#define	T_Scrub			(uTime) 3500	/* default */

/* RMT Expiration Values */
#define	T_Non_OP		(uTime) 1000000	/* default */
#define	T_Stuck			(uTime) 8000000	/* default */
#define	T_Direct		(uTime) 370000	/* default */
#define	T_Jam			(uTime) 370000	/* default */
#define	T_DBJ			(uTime) 82000	/* default */
#define	T_Announce		(uTime) 2500000	/* default */
#define	Dup_Max			(D_Max + D_Max)	/* 2 * D_Max */
/*	T_Rmode		defined by fddiPATHT-Rmode */
#define	T_Rmode_Default		(uTime) 0
/*	T_Req		defined by T_Req_value in MAC */

/*
*	FSP Expiration Values
*	Note: These values represent time in seconds.
*/
#define	T_Notify_Default	30
#define	T_NN_Out		228
#define	T_Notify_Min		2
#define	T_Notify_Max		30
#define	T_Limit			2		/* min time between SRFs */
#define	Max_SRThreshold		32		/* max time between SRFs */

/*
*	MSP Range Limitations
*/
#define	Min_T_MaxLowerBound		(uTime) 10000
#define	FDDI_Min_T_MaxLowerBound	(uInt32) 0xFFFE17B8

/*********************************************************************
*
*	FDDI MIB Definitions
*
*	This section contains the enumerated and bit string type
*	constants for accessing and referencing MIB attributes.
*
*	Some of the bit string constants have an additional value of 0
*	added by XDI for convenience. This constant is usually labelled
*	"None" and is distinguished from a MIB name because it starts
*	with a capital "N".
*********************************************************************/


/*********************************************************************
	FDDI Definitions
*********************************************************************/

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *	For any value or any order changes to the following definitions,
 *	be sure the corresponding MIB support in sr_newfddimib.c is correct.
 *	-- noted 11/14/94
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* AvailablePaths */
#define	AP_None					0
#define	AP_primary				0x01
#define	AP_secondary				0x02
#define	AP_local				0x04

/* CurrentPath */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	fddimibMACCurrentPath, fddimibPATHConfigCurrentPath, and
 *	fddimibPORTCurrentPath per RFC 1512.
 */
#define	CP_isolated				((uInt16) 0)
#define	CP_local				((uInt16) 1)
#define	CP_secondary				((uInt16) 2)
#define	CP_primary				((uInt16) 3)
#define	CP_concatenated				((uInt16) 4)
#define	CP_thru					((uInt16) 5)

/* PortType */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibMACDownstreamPORTType, fddimibPORTMyType per RFC 1512.
 */
#define	PT_a					((uInt16) 0)
#define	PT_b					((uInt16) 1)
#define	PT_s					((uInt16) 2)
#define	PT_m					((uInt16) 3)
#define	PT_none					((uInt16) 4)

/* PermittedPaths */
#define	PP_None				0
#define	PP_local			0x01
#define	PP_secondary_alt		0x02
#define	PP_primary_alt			0x04
#define	PP_concatenated_alt		0x08
#define	PP_secondary_pref		0x10
#define	PP_primary_pref			0x20
#define	PP_concatenated_pref		0x40
#define	PP_thru				0x80

/* Default RequestedPaths Settings */
#define	RP_A_NONE_DEFAULT		(PP_local)
#define	RP_A_TREE_DEFAULT		(PP_local\
						| PP_concatenated_alt\
						| PP_secondary_pref)
#define	RP_A_PEER_DEFAULT		(PP_local\
						| PP_concatenated_alt\
						| PP_secondary_pref\
						| PP_thru)

#define	RP_B_NONE_DEFAULT		(PP_local)
#define	RP_B_TREE_DEFAULT		(PP_local\
						| PP_concatenated_alt\
						| PP_primary_pref)
#define	RP_B_PEER_DEFAULT_DM		(PP_local\
						| PP_concatenated_alt\
						| PP_primary_pref\
						| PP_thru)
#define	RP_B_PEER_DEFAULT_SM		(PP_local\
						| PP_primary_pref\
						| PP_concatenated_pref\
						| PP_thru)

#define	RP_M_NONE_DEFAULT		(PP_local)
#define	RP_M_TREE_DEFAULT		(PP_local\
						| PP_secondary_alt\
						| PP_primary_alt)

#define	RP_S_NONE_DEFAULT		(PP_local)
#define	RP_S_TREE_DEFAULT		(PP_local\
						| PP_concatenated_alt\
						| PP_primary_pref)
#define	RP_S_PEER_DEFAULT		(PP_concatenated_alt\
						| PP_primary_pref)

#define	RP_MAC_DEFAULT			(PP_local\
						| PP_secondary_alt\
						| PP_primary_alt)

/* Allowed RequestedPaths Bits */
#define	RP_A_NONE_ALLOWED		(PP_local)
#define	RP_A_TREE_ALLOWED		(PP_local\
						| PP_secondary_alt\
						| PP_concatenated_alt\
						| PP_secondary_pref\
						| PP_concatenated_pref)
#define	RP_A_PEER_ALLOWED		(PP_local\
						| PP_secondary_alt\
						| PP_concatenated_alt\
						| PP_secondary_pref\
						| PP_concatenated_pref\
						| PP_thru)

#define	RP_B_NONE_ALLOWED		(PP_local)
#define	RP_B_TREE_ALLOWED		(PP_local\
						| PP_primary_alt\
						| PP_concatenated_alt\
						| PP_primary_pref\
						| PP_concatenated_pref)
#define	RP_B_PEER_ALLOWED		(PP_local\
						| PP_primary_alt\
						| PP_concatenated_alt\
						| PP_primary_pref\
						| PP_concatenated_pref\
						| PP_thru)

#define	RP_M_NONE_ALLOWED		(PP_local)
#define	RP_M_TREE_ALLOWED		(PP_local\
						| PP_secondary_alt\
						| PP_primary_alt\
						| PP_secondary_pref\
						| PP_primary_pref)

#define	RP_S_NONE_ALLOWED		(PP_local)
#define	RP_S_TREE_ALLOWED		(PP_local\
						| PP_primary_alt\
						| PP_concatenated_alt\
						| PP_primary_pref\
						| PP_concatenated_pref)
#define	RP_S_PEER_ALLOWED		(PP_local\
						| PP_primary_alt\
						| PP_concatenated_alt\
						| PP_primary_pref\
						| PP_concatenated_pref)

#define	RP_MAC_ALLOWED			(PP_local\
						| PP_secondary_alt\
						| PP_primary_alt\
						| PP_secondary_pref\
						| PP_primary_pref)

/* ResourceTag resource type */
#define	RT_mac				2
#define	RT_port				4


/*********************************************************************
	SMT Definitions
*********************************************************************/

/* ActionInfo */
#define	SMTAction_connect		((uInt16) 0)
#define	SMTAction_disconnect		((uInt16) 1)
#define	SMTAction_path_Test		((uInt16) 2)
#define	SMTAction_self_Test		((uInt16) 3)
#define	SMTAction_disable_a		((uInt16) 4)
#define	SMTAction_disable_b		((uInt16) 5)
#define	SMTAction_disable_m		((uInt16) 6)

/* Capabilities */
#define	Cap_None			0x0000
#define	Cap_holdavailable		0x0001
#define	Cap_cf_wrap_ab			0x0002

/* CF-State */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibSMTCFState per RFC 1512.
 */
#define	CF_isolated			((uInt16) 0)
#define	CF_local_a			((uInt16) 1)
#define	CF_local_b			((uInt16) 2)
#define	CF_local_ab			((uInt16) 3)
#define	CF_local_s			((uInt16) 4)
#define	CF_wrap_a			((uInt16) 5)
#define	CF_wrap_b			((uInt16) 6)
#define	CF_wrap_ab			((uInt16) 7)
#define	CF_wrap_s			((uInt16) 8)
#define	CF_c_wrap_a			((uInt16) 9)
#define	CF_c_wrap_b			((uInt16) 10)
#define	CF_c_wrap_s			((uInt16) 11)
#define	CF_thru				((uInt16) 12)

/* Configpolicy */
#define	CFP_None			0
#define	CFP_configurationhold		0x01

/* ConnectionPolicy */
#define	Con_rejectA_A		0x0001
#define	Con_rejectA_B		0x0002
#define	Con_rejectA_S		0x0004
#define	Con_rejectA_M		0x0008
#define	Con_rejectB_A		0x0010
#define	Con_rejectB_B		0x0020
#define	Con_rejectB_S		0x0040
#define	Con_rejectB_M		0x0080
#define	Con_rejectS_A		0x0100
#define	Con_rejectS_B		0x0200
#define	Con_rejectS_S		0x0400
#define	Con_rejectS_M		0x0800
#define	Con_rejectM_A		0x1000
#define	Con_rejectM_B		0x2000
#define	Con_rejectM_S		0x4000
#define	Con_rejectM_M		0x8000
/*
* Default connection policy. These are:
* A-B, A-M
* B-A, B-B, B-M
* S-M
* M-A, M-B, M-S
*/
#define	Con_XDI_Default\
			( Con_rejectA_A | Con_rejectA_S\
				| Con_rejectB_S\
				| Con_rejectS_A | Con_rejectS_B | Con_rejectS_S\
				| Con_rejectM_M)


/* ECMState */
#define	ECM_ec0				((uInt16) 0)
#define	ECM_ec1				((uInt16) 1)
#define	ECM_ec2				((uInt16) 2)
#define	ECM_ec3				((uInt16) 3)
#define	ECM_ec4				((uInt16) 4)
#define	ECM_ec5				((uInt16) 5)
#define	ECM_ec6				((uInt16) 6)
#define	ECM_ec7				((uInt16) 7)

/* HoldState */
#define HS_not_holding		((uInt16) 0)
#define HS_holding_prm		((uInt16) 1)
#define HS_holding_sec		((uInt16) 2)

/* Status */
#define	Status_concatenated	((uInt16) 0)
#define	Status_separated	((uInt16) 1)
#define	Status_thru		((uInt16) 2)

/*
*   Graceful insertion capabilities and modes
*/
#define	GIC_None			0
#define	GIC_scheduledinsertion		1
#define	GIC_gracefulinsertion		2
#define	GIC_diagnostics			4

#define GIM_off     		   	0
#define GIM_scheduledinsertion		1
#define GIM_gracefulinsertion		2
#define	GIM_diagnostics			3

/*
*	Definitions of LMM states for graceful insertion support.
*/
#define	LM_IDLE				0
#define	LM_DIAG				1
#define	LM_GI_PRI			2
#define	LM_GI_SEC			3
#define	LM_LOOP				4
#define	LM_FIND				5
#define	LM_MON				6

/* Graceful insertion MaxPortsCanConnect */
#define Default_GracefulMax		1

/* No MAC available for graceful insertion */
#define	NO_GIMAC			((uInt16) ~0)


/*********************************************************************
	MAC Definitions
*********************************************************************/

/* BridgeType */
#define	BT_None				0x0000
#define	BT_tb				0x0001
#define	BT_sr				0x0002
#define	BT_srt				0x0004

/* DupAddressTest */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibMACDupAddressTest per RFC 1512.
 */
#define	DAT_none			((uInt16) 0)
#define	DAT_pass			((uInt16) 1)
#define	DAT_fail			((uInt16) 2)

/* DupCondition */
#define	DC_mydup			0x01
#define	DC_unadup			0x02

/* FS-Functions */
#define	FS_None				0x0000
#define	FS_repeating			0x0001
#define	FS_setting			0x0002
#define	FS_clearing			0x0004

/* NACondition */
#define	NAC_None			0x00
#define	NAC_unachange			0x01
#define	NAC_dnachange			0x02

/* RMTState */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibMACRMTState per RFC 1512.
 */
#define	RMT_rm0				((uInt16) 0)
#define	RMT_rm1				((uInt16) 1)
#define	RMT_rm2				((uInt16) 2)
#define	RMT_rm3				((uInt16) 3)
#define	RMT_rm4				((uInt16) 4)
#define	RMT_rm5				((uInt16) 5)
#define	RMT_rm6				((uInt16) 6)
#define	RMT_rm7				((uInt16) 7)

/* XDI ConfigurationStatus */
#define	XDIMAC_configstatus_success	((uInt16) 0)
#define	XDIMAC_configstatus_tvx		((uInt16) 1)
#define	XDIMAC_configstatus_tmax	((uInt16) 2)


/*********************************************************************
	PATH Definitions
*********************************************************************/

/* Index */
#define	PI_None				((uInt16) 0)
#define	PI_primary			((uInt16) 1)
#define	PI_secondary			((uInt16) 2)
#define	PI_local			((uInt16) 3)

/* array index */
#define	PII_primary			((uInt16) 0)
#define	PII_secondary			((uInt16) 1)
#define	PII_local			((uInt16) 2)

/* TraceStatus */
#define	TS_None				0x0000
#define	TS_traceinitiated		0x0001
#define	TS_tracepropagated		0x0002
#define	TS_traceterminated		0x0004
#define	TS_tracetimeout			0x0008


/*********************************************************************
	PORT Definitions
*********************************************************************/

/* ActionType */
#define	PORTAction_maintport		((uInt16) 0)
#define	PORTAction_enableport		((uInt16) 1)
#define	PORTAction_disableport		((uInt16) 2)
#define	PORTAction_startport		((uInt16) 3)
#define	PORTAction_stopport		((uInt16) 4)

/* ConnectionPolicies */
#define	CP_None				0x00
#define	CP_pc_mac_lct			0x01
#define	CP_pc_mac_loop			0x02

/* ConnectState */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibPORTConnectState per RFC 1512.
 */
#define	CS_disabled			((uInt16) 0)
#define	CS_connecting			((uInt16) 1)
#define	CS_standby			((uInt16) 2)
#define	CS_active			((uInt16) 3)

/* LineState */
#define	LS_qls				((uInt16) 0)
#define	LS_ils				((uInt16) 1)
#define	LS_mls				((uInt16) 2)
#define	LS_hls				((uInt16) 3)
#define	LS_pdr				((uInt16) 4)
#define	LS_lsu				((uInt16) 5)
#define	LS_nls				((uInt16) 6)

/* PCMState */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibPORTPCMState per RFC 1512.
 */
#define	PCM_pc0				((uInt16) 0)
#define	PCM_pc1				((uInt16) 1)
#define	PCM_pc2				((uInt16) 2)
#define	PCM_pc3				((uInt16) 3)
#define	PCM_pc4				((uInt16) 4)
#define	PCM_pc5				((uInt16) 5)
#define	PCM_pc6				((uInt16) 6)
#define	PCM_pc7				((uInt16) 7)
#define	PCM_pc8				((uInt16) 8)
#define	PCM_pc9				((uInt16) 9)

/* PC_Withhold */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibPORTPCWithhold per RFC 1512.
 */
#define	PCW_none			((uInt16) 0)
#define	PCW_m_m				((uInt16) 1)
#define	PCW_otherincompatible		((uInt16) 2)
#define	PCW_pathnotavailable		((uInt16) 3)

/* PMDClass */
/*
 *	Important that each match corresponding syntax value - 1 for
 *	the fddimibPORTPMDClass per RFC 1512.
 */
#define	PMD_multimode			((uInt16) 0)
#define	PMD_single_mode1		((uInt16) 1)
#define	PMD_single_mode2		((uInt16) 2)
#define	PMD_sonet			((uInt16) 3)
#define	PMD_low_cost_fiber		((uInt16) 4)
#define	PMD_twisted_pair		((uInt16) 5)
#define	PMD_unknown			((uInt16) 6)
#define	PMD_unspecified			((uInt16) 7)


/*********************************************************************
	MIB Packages
*********************************************************************/

#define	PKG_manufacturerdata			((uInt32) 0x00000001)
#define	PKG_parametermanagement			((uInt32) 0x00000002)
/* The Hold policy is not supported in release 7.3.1 and later */
#define PKG_hold not supported			((uInt32) 0x00000004)
#define	PKG_smtvendorspecific			((uInt32) 0x00000008)
#define	PKG_xdigracefulentry			((uInt32) 0x00000010)

#define	PKG_framenotcopied			((uInt32) 0x00000010)
#define	PKG_framenotcopiedmanagement		((uInt32) 0x00000020)
#define	PKG_prioritythreshold			((uInt32) 0x00000040)
#define	PKG_tvxexpired				((uInt32) 0x00000080)
#define	PKG_latecount				((uInt32) 0x00000100)
#define	PKG_ringopcount				((uInt32) 0x00000200)
#define	PKG_longgrpaddress			((uInt32) 0x00000400)
#define	PKG_shortgrpaddress			((uInt32) 0x00000800)
#define	PKG_tokencounter			((uInt32) 0x00001000)
#define	PKG_bridgefunctions			((uInt32) 0x00002000)
#define	PKG_macvendorspecific			((uInt32) 0x00004000)

#define	PKG_synchbandwidthrequestor		((uInt32) 0x00008000)
#define	PKG_synchbandwidthmgt			((uInt32) 0x00010000)
#define	PKG_ringlatency				((uInt32) 0x00020000)
#define	PKG_tracestatus				((uInt32) 0x00040000)
#define	PKG_restrictedtoken			((uInt32) 0x00080000)
#define	PKG_pathvendorspecific			((uInt32) 0x00100000)

#define	PKG_eberrorcounter			((uInt32) 0x00200000)
#define	PKG_maclocalloop			((uInt32) 0x00400000)
#define	PKG_maint_LS				((uInt32) 0x00800000)
#define	PKG_maint_LSmanagement			((uInt32) 0x01000000)
#define	PKG_portvendorspecific			((uInt32) 0x02000000)
#define	PKG_xdiportmapping			((uInt32) 0x04000000)


/*********************************************************************
	XLNT Manager(tm) Definitions

	This sections contains constants used by the XLNT Manager
	code to implement the SMT standard.
*********************************************************************/

/*********************************************************************
	General Support Values
*********************************************************************/

/*
*	Requested Path List Elements
*
*	These defines translate the list element numbers in CFM to
*	the C array element indexes.
*/
#define	RP_none				0
#define	RP_tree				1
#define	RP_peer				2

/*
*	Default Link Error Values and Limits
*/
#define	LER_MIN				4
#define	LER_MAX				15

#define	LER_CUTOFF_DEFAULT		7
#define	LER_ALARM_DEFAULT		8


/*********************************************************************
	CMT & RMT States
*********************************************************************/

/*
*	The following defined values are used to determine the current
*	states for each state machine. The values and names used follow
*	the FDDI SMT documentation.
*/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *	Important that these are defined in sync with those for MIB.
 *	Any changes to them, be sure the corresponding MIB support is right.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/*
 *	ECM States
 */
#define	EC_OUT				ECM_ec0
#define	EC_IN				ECM_ec1
#define	EC_TRACE			ECM_ec2
#define	EC_LEAVE			ECM_ec3
#define	EC_PATH_TEST			ECM_ec4
#define	EC_INSERT			ECM_ec5
#define	EC_CHECK			ECM_ec6
#define	EC_DEINSERT			ECM_ec7

/*
*	CM States
*/
#define	CM_ISOLATED			((uInt16) 0)
#define	CM_PRIMARY			((uInt16) 1)
#define	CM_SECONDARY			((uInt16) 2)
#define	CM_THRU				((uInt16) 3)
#define	CM_LOCAL			((uInt16) 4)
#define	CM_CONCAT			((uInt16) 5)

/*
*	PCM States
*/
#define PC_OFF                          PCM_pc0
#define PC_BREAK                        PCM_pc1
#define PC_TRACE                        PCM_pc2
#define PC_CONNECT                      PCM_pc3
#define PC_NEXT                         PCM_pc4
#define PC_SIGNAL                       PCM_pc5
#define PC_JOIN                         PCM_pc6
#define PC_VERIFY                       PCM_pc7
#define PC_ACTIVE                       PCM_pc8
#define PC_MAINT                        PCM_pc9

/*
*	RMT States
*/
#define	RM_ISOLATED			RMT_rm0
#define	RM_NON_OP			RMT_rm1
#define	RM_RING_OP			RMT_rm2
#define	RM_DETECT			RMT_rm3
#define	RM_NON_OP_DUP			RMT_rm4
#define	RM_RING_OP_DUP			RMT_rm5
#define	RM_DIRECTED			RMT_rm6
#define	RM_TRACE			RMT_rm7

/*
*	RMT Substates used in XDI SMT implementation.
*/
#define	RM_JAM_A_WAIT_TJAM	((uInt16) 41)
#define	RM_JAM_B_WAIT_TDBJ	((uInt16) 42)
#define	RM_JAM_B_WAIT_TJAM	((uInt16) 43)

/*
*	LEM States used in XDI SMT implementation.
*/
#define	LE_OFF			((uInt16) 0)
#define	LE_MONITOR		((uInt16) 1)
#define	LE_LCT			((uInt16) 2)


/*********************************************************************
	CMT Definitions
*********************************************************************/

/*
*	Path_Test Values
*/
#define	PT_None			((uInt16) 0)
#define	PT_Testing		((uInt16) 1)
#define	PT_Passed		((uInt16) 2)
#define	PT_Failed		((uInt16) 3)
#define	PT_Pending		((uInt16) 4)
#define	PT_Exiting		((uInt16) 5)

/*
*	PC_Mode Values
*/
#define	PC_Mode_None		((uInt16) RP_none)
#define	PC_Mode_Tree		((uInt16) RP_tree)
#define	PC_Mode_Peer		((uInt16) RP_peer)

/*
*	Line States
*	These lines define the constant names used in the CSP code.
*	The actual values for the line states are defined in smtdefs.h.
*/
#define	ALS			(Active_Line_State)
#define	HLS			(Halt_Line_State)
#define	ILS			(Idle_Line_State)
#define	MLS			(Master_Line_State)
#define	NLS			(Noise_Line_State)
#define	PDRLS			(Transmit_PHY_Data_Request)
#define	QLS			(Quiet_Line_State)
#define	SILS			(Super_Idle_Line_State)

/*
*	RMT Duplicate Address Policies
*/
#define	RMT_DUP_CHANGE		((uInt16) 1)/* perform change actions */
#define	RMT_DUP_JAM_A		((uInt16) 2)/* perform jam actions */
#define	RMT_DUP_JAM_B		((uInt16) 3)/* perform jam actions */
#define	RMT_DUP_LEAVE		((uInt16) 4)/* leave ring */

/*
*	RMT Duplicate Address Capability
*	This is a bit string of policies supported.
*/
#define	RMT_DAC_CHANGE		((uChar) 0x01)
#define	RMT_DAC_JAM_A		((uChar) 0x02)
#define	RMT_DAC_JAM_B		((uChar) 0x04)
#define	RMT_DAC_LEAVE		((uChar) 0x08)

/*
* T-Val9 and R-Val9 bit positions for reporting MACIndicated
* from CSP to MSP.
*/
#define	CSP_TVAL9		0x01
#define	CSP_RVAL9		0x02


/*********************************************************************
	FSP Definitions
*********************************************************************/

/*
*	Known Address Initialization Formats. Actual address references are
*	located in CSP, MSP and FSP header files.
*/
 /* FDDI broadcast address */
#define	SMT_Broadcast_INIT		{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

 /* FDDI Null address */
#define	SMT_NULL_INIT			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

 /* FDDI Unknown address */
#define	SMT_Unknown_INIT		{0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}

 /* Directed beacon for RMT */
#define	SMT_Directed_Beacon_INIT	{0x80, 0x01, 0x43, 0x00, 0x80, 0x00}

 /* SRF Multicast address */
#define	SMT_SRF_INIT			{0x80, 0x01, 0x43, 0x00, 0x80, 0x08}

 /* SBA Multicast address */
#define	SMT_SBA_INIT			{0x80, 0x01, 0x43, 0x00, 0x80, 0x0C}

/*
*	Beacon Types
*/
#define	BCN_CLAIM_FAILED		((uChar) 0)
#define	BCN_DIRECTED			((uChar) 1)
#define	BCN_JAM				((uChar) 2)

/*
*	Reason Denied Codes
*/
#define	RC_FrameClassNotSupported	((uInt32) 0x01)
#define	RC_FrameVersionNotSupported	((uInt32) 0x02)
#define	RC_Success			((uInt32) 0x03)
#define	RC_BadSetCount			((uInt32) 0x04)
#define	RC_IllegalOperation		((uInt32) 0x05)
#define	RC_NoParameter			((uInt32) 0x06)
#define	RC_OutOfRange			((uInt32) 0x08)
#define	RC_NotAuthorized		((uInt32) 0x09)
#define	RC_LengthError			((uInt32) 0x0A)
#define	RC_FrameTooLong			((uInt32) 0x0B)
#define	RC_SBADenied			((uInt32) 0x0D)
/* XLNT Manager values */
#define	RC_ERROR_UNKNOWN		((uInt32) 0xFE)
#define	RC_BUFFER_ERROR			((uInt32) 0xFF)


/*
*	Node Class of Station Descriptor Parameter
*	The type of station we are can be one of two things, either a
*	station or a concentrator.  The values for these are defined
*	below.
*/
#define SMT_Type_Station		0
#define SMT_Type_Concentrator		1

/*
*	Topology Bit Assignments of Station State Parameter
*	The station's current topology is indicated by which bit is set.
*/
#define	Topology_Thru			0x00
#define	Topology_Peer_Wrap		0x01
#define	Topology_Unattached_Conc	0x02
#define	Topology_Twisted_AA		0x04
#define	Topology_Twisted_BB		0x08
#define	Topology_Rooted_Station		0x10
#define	Topology_SRF			0x20
#define	Topology_SBA			0x40

/*
*	Duplicate Address Bit Assignments of Station State Parameter
*	MAC's current duplicate address status for the
*	Station State parameter.
*/
#define	DuplAddr_My_Duplicate		0x01
#define	DuplAddr_My_UNA_Duplicate	0x02

/*
*	Maximum Frame Sizes (in bytes)
*/

/* Max FDDI frame size */
#define	MAX_FRAME_SIZE			((uInt16) 4500)

/* Max MAC INFO field size */
#define	MAX_MAC_INFO_LEN		((uInt16) 4478)

/* Max SMT INFO field size */
#define	MAX_SMT_INFO_LEN		((uInt16) 4458)

/* Max transmit MAC InfoField length for SMT frames (excludes ECHO frames) */
#define	MAX_XMIT_LEN			((uInt16) 4352)

/* Max transmit SMT InfoField length for SMT frames (excludes ECHO frames) */
#define	MAX_SMT_XMIT_LEN		((uInt16) 4332)

/*
*	Frame Status Values
*	Defined in smtdefs.h.
*/
#define	E_Indicator			E_Bit_Position
#define	A_Indicator			A_Bit_Position
#define	C_Indicator			C_Bit_Position

/*
*	Resource Types
*/
#define	RESOURCE_SB		((uInt32) 1)	/* synchronous bandwidth */

/*
*	Synchronous Bandwidth Commands
*/
#define	SBA_Request_Allocation		((uInt32) 1)
#define	SBA_Report_Allocation		((uInt32) 2)
#define	SBA_Change_Allocation		((uInt32) 3)


/*********************************************************************
	MSP Definitions
*********************************************************************/

/*
* These values define the number of standard notifications (event and
* conditions) for each MIB object and the number of XDI-defined
* notifications for each object.
*/
#define	FDDI_SMT_EC_COUNT		2
#define	FDDI_MAC_EC_COUNT		5
#define	FDDI_PATH_EC_COUNT		0
#define	FDDI_PORT_EC_COUNT		4

#define	XDI_SMT_EC_COUNT		0
#define	XDI_MAC_EC_COUNT		2
#define	XDI_PATH_EC_COUNT		0
#define	XDI_PORT_EC_COUNT		0

/*
* Defined values for whether the notification is an event or condition.
*/
#define	EC_TYPE_EVENT			1
#define	EC_TYPE_CONDITION		2


/*********************************************************************
	6.2 Definitions

	The following constants are provided for compatibility with
	the 6.2 Manager code. Where possible, equivalent definitions
	have been provided. In some cases, 7.0 deleted attributes
	and code points. These deleted values are no longer supported.

	To include these definitions into the code, define the constant
		USE_62_DEFS
	prior to including this file.
*********************************************************************/

#ifdef	USE_62_DEFS

/* 6.2 PortType */
#define	PC_Type_A			PT_a
#define	PC_Type_B			PT_b
#define	PC_Type_S			PT_s
#define	PC_Type_M			PT_m
#define	PC_Type_Unknown			PT_none
#define	PC_Type_None			PT_none

/* 6.2 Resource Type */
#define	RT_MAC				RT_mac
#define	RT_PORT				RT_port

/* 6.2 SMT ActionInfo */
#define	StationAction_Connect		SMTAction_connect
#define	StationAction_Disconnect	SMTAction_disconnect
#define	StationAction_Path_Test		SMTAction_path_Test
#define	StationAction_Self_Test		SMTAction_self_Test

/* 6.2 Capabilities */
#define	Config_None			Cap_None
#define	Config_Hold_Available		Cap_holdavailable
#define	Config_CF_Wrap_AB		Cap_cf_wrap_ab

/* 6.2 Connection */
#define	Policy_rejectA_A		Con_rejectA_A
#define	Policy_rejectA_B		Con_rejectA_B
#define	Policy_rejectA_S		Con_rejectA_S
#define	Policy_rejectA_M		Con_rejectA_M
#define	Policy_rejectB_A		Con_rejectB_A
#define	Policy_rejectB_B		Con_rejectB_B
#define	Policy_rejectB_S		Con_rejectB_S
#define	Policy_rejectB_M		Con_rejectB_M
#define	Policy_rejectS_A		Con_rejectS_A
#define	Policy_rejectS_B		Con_rejectS_B
#define	Policy_rejectS_S		Con_rejectS_S
#define	Policy_rejectS_M		Con_rejectS_M
#define	Policy_rejectM_A		Con_rejectM_A
#define	Policy_rejectM_B		Con_rejectM_B
#define	Policy_rejectM_S		Con_rejectM_S
#define	Policy_rejectM_M		Con_rejectM_M
#define	Policy_limited			(~(Con_rejectA_B | Con_rejectB_A\
					| Con_rejectS_M | Con_rejectM_S))
#define	Policy_expanded			(Con_rejectM_M)
#define	Policy_rejectAll		0xFFFF

/* 6.2 HoldState */
#define HS_Not_Holding			HS_not_holding
#define HS_Holding_Prm			HS_holding_prm
#define HS_Holding_Sec			HS_holding_sec

/* 6.2 BridgeType */
#define	Bridge_None			BT_None
#define	Bridge_Type0			BT_tb
#define	Bridge_Type1			BT_sr

/* 6.2 DupAddressTest */
#define	DA_Test_None			DAT_none
#define	DA_Test_Pass			DAT_pass
#define	DA_Test_Fail			DAT_fail

/* 6.2 FS-Functions */
#define	FSC_None			FS_None
#define	FSC_Type0			FS_repeating
#define	FSC_Type1			FS_setting
#define	FSC_Type2			FS_clearing

/* 6.2 NACondition */
#define	EVENT_UNA_CHANGED		NAC_unachange
#define	EVENT_DNA_CHANGED		NAC_dnachange

/* 6.2 TraceStatus */
#define	Trace_NoCurrent			TS_None
#define	Trace_Initiated			TS_traceinitiated
#define	Trace_Propagated		TS_tracepropagated
#define	Trace_Terminated		TS_traceterminated

/* 6.2 PORT ActionType */
#define	PORTAction_Maint		PORTAction_maintport
#define	PORTAction_Enable		PORTAction_enableport
#define	PORTAction_Disable		PORTAction_disableport
#define	PORTAction_Start		PORTAction_startport
#define	PORTAction_Stop			PORTAction_stopport

/* 6.2 ConnectionPolicies */
#define	PC_MAC_None			CP_None
#define	PC_MAC_LCT			CP_pc_mac_lct
#define	PC_MAC_Loop			CP_pc_mac_loop

/* 6.2 ConnectState */
#define	Connect_Disabled		CS_disabled
#define	Connect_Connecting		CS_connecting
#define	Connect_Standby			CS_standby
#define	Connect_Active			CS_active

/* 6.2 LineState */
#define	Maint_QLS			LS_qls
#define	Maint_ILS			LS_ils
#define	Maint_MLS			LS_mls
#define	Maint_HLS			LS_hls
#define	Maint_PDR			LS_pdr

/* 6.2 PC_Withhold */
#define	PC_WH_None			PCW_none
#define	PC_WH_M_to_M			PCW_m_m
#define	PC_WH_Other			PCW_other

/* 6.2 PMDClass */
#define FOTX_Multimode			PMD_multimode
#define FOTX_Single_Mode1		PMD_single_mode1
#define FOTX_Single_Mode2		PMD_single_mode2
#define FOTX_SONET			PMD_sonet

/* 6.2 Reason Denied Codes */
#define	RC_FRAME_CLASS			RC_FrameClassNotSupported
#define	RC_FRAME_VERSION		RC_FrameVersionNotSupported
#define	RC_SUCCESS			RC_Success
#define	RC_BAD_SET_COUNT		RC_BadSetCount
#define	RC_READ_ONLY			RC_IllegalOperation
#define	RC_NO_PARAM			RC_NoParameter
#define	RC_OUT_OF_RANGE			RC_OutOfRange
#define	RC_NOT_AUTHORIZED		RC_NotAuthorized
#define	RC_LENGTH_ERROR			RC_LengthError
#define	RC_FRAME_TOO_LONG		RC_FrameTooLong

/* 6.2 Topology Bits */
#define	Topology_Wrapped		Topology_Peer_Wrap
#define	Topology_Unrooted		Topology_Unattached_Conc

#endif					/* USE_62_DEFS */
