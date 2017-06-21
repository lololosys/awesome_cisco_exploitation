/* $Id: atmtest_lane_vars.h,v 3.2 1995/11/17 08:47:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-test/atmtest_lane_vars.h,v $
 *------------------------------------------------------------------
 * atmtest_lane_vars.h -- Definitions of the LANE variables.  These 
 * variables are used to create the LANE control packets.
 * [Was ../tcl/tcl_lane_vars.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_lane_vars.h,v $
 * Revision 3.2  1995/11/17  08:47:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_LANE_VARS_H_
#define _ATMTEST_LANE_VARS_H_


/*
 * Variable names for the various fields of the LANE control 
 * packets.
 */

/*
 * Miscellaneous global variables needed to put together LANE
 * control packets and send them out an interface.
 */
#define LE_VAR_VCD			"vcd"
#define LE_VAR_SLOT			"slot"
#define LE_VAR_PORT			"port"
#define LE_VAR_SUBINTF			"subinterface"

/*
 * Fields in the control header common to all types of
 * control packets.
 */
#define LE_CP_VAR_MARKER		"marker"
#define LE_CP_VAR_PROTOCOL		"protocol"
#define LE_CP_VAR_VERSION		"version"
#define LE_CP_VAR_OPCODE		"opcode"
#define LE_CP_VAR_STATUS		"status"
#define LE_CP_VAR_TRANS_ID		"transaction_id"
#define LE_CP_VAR_REQ_LECID		"lecid"
#define LE_CP_VAR_FLAGS			"flags"

#define LE_CP_VAR_SRC_DST_TAG		"src_lan_dest_tag"
#define LE_CP_VAR_SRC_DST_MAC		"src_lan_dest_mac"
#define LE_CP_VAR_TGT_DST_TAG		"target_dest_tag"
#define LE_CP_VAR_TGT_DST_MAC		"target_dest_mac"


/*
 * Fields of the Config REQ/RESP packet body
 */
#define LE_CP_VAR_SRC_ATM_ADDR		"src_atm_addr"
#define LE_CP_VAR_LAN_TYPE		"lan_type"
#define LE_CP_VAR_MAX_FRM_SZ		"max_frame_size"
#define LE_CP_VAR_NUM_TLVS		"num_tlvs"
#define LE_CP_VAR_NAME_SZ		"name_size"
#define LE_CP_VAR_TGT_ATM_ADDR		"target_atm_addr"
#define LE_CP_VAR_ELAN_NAME		"elan_name"


/* 
 * Variables for the IP ARP packet 
 */
#define LE_IP_ARP_ARHRD			"arhrd"
#define LE_IP_ARP_ARPRO			"arpro"
#define LE_IP_ARP_ARHLN			"arhln"
#define LE_IP_ARP_ARPLN			"arpln"
#define LE_IP_ARP_OPCODE		"ip_arp_opcode"
#define LE_IP_ARP_IPSHDW		"ipshdw"
#define LE_IP_ARP_IPSPRO		"ipspro"
#define LE_IP_ARP_IPTHDW		"ipthdw"
#define LE_IP_ARP_IPTPRO		"iptpro"


/*
 * NSAP Variables
 */
#define ATM_VAR_LOC_NSAP		"loc_nsap"
#define ATM_VAR_LOC_NSAP_LEN		"loc_nsap_len"
#define ATM_VAR_RMT_NSAP		"rmt_nsap"
#define ATM_VAR_RMT_NSAP_LEN		"rmt_nsap_len"

/*
 * BLLI Variables - specific to ATM Signalling
 * KLUDGE ALERT!
 * Note that if the BLLI array size changes, the number
 * of elements specifically defined here will have to change...
 * It makes sense to have each a specific TCL variable defined
 * for each element for clarity and consistency with the rest 
 * of the variables, although I understand that the last two
 * elements won't be used right away...
 */
#define ATM_SIG_BLLI_ARRAY		"blli_array"
#define ATM_SIG_BLLI_STRING		"blli(blli_string)"
#define ATM_SIG_BLLI_LENGTH		"blli(blli_length)"
#define ATM_SIG_BLLI_STRING_2		"blli(blli_string_2)"
#define ATM_SIG_BLLI_LENGTH_2		"blli(blli_length_2)"
#define ATM_SIG_BLLI_STRING_3		"blli(blli_string_3)"
#define ATM_SIG_BLLI_LENGTH_3		"blli(blli_length_3)"

/*
 * The 'multiPoint' field of the atmsig_api messages gets
 * set to the value of this variable.  It defaults to
 * FALSE, indicating that SVCs will be point-to-point.
 */
#define ATM_SIG_MULTIPOINT		"multipoint"


/*
 * Current VC TCL Global.  Things are a little sticky wrt this variable...
 * to keep track of multiple SVCs, we need to know the 'current' SVC 
 * so that the call_id can be saved/retrieved.  It _must_ be set by the 
 * user prior to running the commands to set up an SVC, but we'll default
 * it just to maintain sanity.
 */
#define LANE_CURRENT_VC			"current_vc"

/*
 * Current Endpoint TCL Global.  Because of the urgent need to support 
 * testing point-to-multipoint SVCs, this stuff is being added to the 
 * list of 'LANE' variables.  It's starting to get messy...we will need
 * separate variable arrays at some point.  This var is used to specify 
 * the storage location for the current p2mpt SVC endpoint_id.
 */
#define CURRENT_ENDPOINT_ID		"current_endpoint_id"



/*-------------------------------------------------*/
/*
 * Defaults for the miscellaneous global variables.
 */
#define LE_VAR_VCD_DEF			"1"
#define LE_VAR_SLOT_DEF			"1"
#define LE_VAR_PORT_DEF			"0"
#define LE_VAR_SUBINTF_DEF		"0"

/*
 * Default values for all of the control packet fields
 * At this time, many of the vaiable defaults are arbitrary
 * for testing purposes.
 */
#define LE_CP_VAR_MARKER_DEF		"0xff00"
#define LE_CP_VAR_PROTOCOL_DEF		"0x01"
#define LE_CP_VAR_VERSION_DEF		"0x01"
#define LE_CP_VAR_OPCODE_DEF		"0x0102"
#define LE_CP_VAR_STATUS_DEF		"0x5678"
#define LE_CP_VAR_TRANS_ID_DEF		"0x12343212"
#define LE_CP_VAR_REQ_LECID_DEF		"0x5a5a"
#define LE_CP_VAR_FLAGS_DEF		"0xff8f"

#define LE_CP_VAR_SRC_DST_TAG_DEF	"0xa"
#define LE_CP_VAR_SRC_DST_MAC_DEF	"171.69.1"
#define LE_CP_VAR_TGT_DST_TAG_DEF	"0xb"
#define LE_CP_VAR_TGT_DST_MAC_DEF	"171.69.3"


/*
 * Defaults for the Config REQ/RESP packet body fields
 */
#define LE_CP_VAR_SRC_ATM_ADDR_DEF "45.14155551212.AAAA.BBBB.CCCC.DDDD.EEEE.FF"

#define LE_CP_VAR_LAN_TYPE_DEF		"0x01"
#define LE_CP_VAR_MAX_FRM_SZ_DEF	"0x01"
#define LE_CP_VAR_NUM_TLVS_DEF		"0"
#define LE_CP_VAR_NAME_SZ_DEF		"6"
#define LE_CP_VAR_TGT_ATM_ADDR_DEF "45.4085266222.1111.2222.3333.4444.5555.66"

#define LE_CP_VAR_ELAN_NAME_DEF		"yellow"

/*
 * IP ARP default fields; these _should_ be the same as the 
 * literal values defined elsewhere if you remove the "_STR".
 */
#define OP_REQ_STR			"1"
#define OP_REP_STR			"2"
#define PR_IP_STR			"0x800"
#define PL_IP_STR			"4"
#define HL_ETH_STR			"6"
#define HW_ETH_STR			"1"

/* 
 * Defaults for the IP ARP packet variables
 */
#define LE_IP_ARP_ARHRD_DEF		HW_ETH_STR
#define LE_IP_ARP_ARPRO_DEF		PR_IP_STR
#define LE_IP_ARP_ARHLN_DEF		HL_ETH_STR
#define LE_IP_ARP_ARPLN_DEF		PL_IP_STR
#define LE_IP_ARP_OPCODE_DEF		OP_REP_STR
#define LE_IP_ARP_IPSHDW_DEF		"0000.000c.aaaa"
#define LE_IP_ARP_IPSPRO_DEF		"1.1.1.1"
#define LE_IP_ARP_IPTHDW_DEF		"0000.000c.bbbb"
#define LE_IP_ARP_IPTPRO_DEF		"2.2.2.2"

/*
 * Default values for the NSAP variables.
 * (Currently these are arbitrary values.)
 */
#define ATM_VAR_LOC_NSAP_DEF	"22.ABCD.01.234567.890A.BCDE.F012.3456.7890.1234.10"
#define ATM_VAR_LOC_NSAP_LEN_DEF	"20"
#define ATM_VAR_RMT_NSAP_DEF	"33.CDEF.01.234567.890A.BCDE.F012.3456.7890.1234.10"
#define ATM_VAR_RMT_NSAP_LEN_DEF	"20"

/*
 * Defaults for the BLLI fields of the ATM Signalling messages
 */
#define ATM_SIG_BLLI_STRING_DEF		"6b40808000a03e0001"
#define ATM_SIG_BLLI_LENGTH_DEF		"9"
#define ATM_SIG_BLLI_STRING_2_DEF	"0"
#define ATM_SIG_BLLI_LENGTH_2_DEF	"0"
#define ATM_SIG_BLLI_STRING_3_DEF	"0"
#define ATM_SIG_BLLI_LENGTH_3_DEF	"0"

/*
 * Default SVCs to point-to-point (vs. point-to-multipoint).
 */
#define ATM_SIG_MULTIPOINT_DEF		"FALSE"


/* Arbitrary */
#define LANE_CURRENT_VC_DEF		"configure"
#define CURRENT_ENDPOINT_ID_DEF		"0"


/*
 * LAN Emulation Control Opcode strings
 * Used to assign the global 'opcode' variable.
 */
#define LANE_CONFIG_REQ_STR		"0x0001"
#define LANE_CONFIG_RESP_STR		"0x0101"
#define LANE_JOIN_REQ_STR		"0x0002"
#define LANE_JOIN_RESP_STR		"0x0102"
#define LANE_REG_REQ_STR		"0x0004"
#define LANE_REG_RESP_STR		"0x0104"
#define LANE_UNREG_REQ_STR		"0x0005"
#define LANE_UNREG_RESP_STR		"0x0105"
#define LANE_ARP_REQ_STR		"0x0006"
#define LANE_ARP_RESP_STR		"0x0106"
#define LANE_FLUSH_REQ_STR		"0x0007"
#define LANE_FLUSH_RESP_STR		"0x0107"
#define LANE_TOPO_CHANGE_STR		"0x0009"

#endif _ATMTEST_LANE_VARS_H_
