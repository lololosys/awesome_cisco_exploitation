/* $Id: atmtest_lane_arrays.h,v 3.2 1995/11/17 08:47:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-test/atmtest_lane_arrays.h,v $
 *------------------------------------------------------------------
 * atmtest_lane_arrays.h -- Definitions of the LANE variable arrays.
 * [Was ../tcl/tcl_lane_arrays.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_lane_arrays.h,v $
 * Revision 3.2  1995/11/17  08:47:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_LANE_ARRAYS_H_
#define _ATMTEST_LANE_ARRAYS_H_


/*
 * TCL LANE variable array and variable default arrays. 
 * These arrays are used by the variable init routine to
 * set variables to default values if they haven't been assigned
 * values at the console.
 *
 * KLUDGE ALERT!
 * The LE_CP_VAR_COUNT constant defined above *must be updated* each
 * time a new variable is added to the list.  Failure to do so will
 * prevent one (or more) variables from being initialized.  In addition,
 * there must be a matching default value in lane_var_def_array[] for
 * each variable in lane_var_array[], or there's no telling how the
 * variables will be initialized!
 *
 * One other obvious thing - the ordering between these two arrays
 * must be kept consistent to insure that each variable is 
 * initialized with the proper default value.
 */

static char *lane_var_array[] = {
      LE_CP_VAR_MARKER,
      LE_CP_VAR_PROTOCOL,
      LE_CP_VAR_VERSION,
      LE_CP_VAR_OPCODE,
      LE_CP_VAR_STATUS,
      LE_CP_VAR_TRANS_ID,
      LE_CP_VAR_REQ_LECID,
      LE_CP_VAR_FLAGS,

      LE_CP_VAR_SRC_DST_TAG,
      LE_CP_VAR_SRC_DST_MAC,
      LE_CP_VAR_TGT_DST_TAG,
      LE_CP_VAR_TGT_DST_MAC,

      LE_CP_VAR_SRC_ATM_ADDR,
      LE_CP_VAR_LAN_TYPE,
      LE_CP_VAR_MAX_FRM_SZ,
      LE_CP_VAR_NUM_TLVS,
      LE_CP_VAR_NAME_SZ,
      LE_CP_VAR_TGT_ATM_ADDR,
      LE_CP_VAR_ELAN_NAME,

      /* Variables for the IP ARP packet. */
      LE_IP_ARP_ARHRD,
      LE_IP_ARP_ARPRO,
      LE_IP_ARP_ARHLN,
      LE_IP_ARP_ARPLN,
      LE_IP_ARP_OPCODE,
      LE_IP_ARP_IPSHDW,
      LE_IP_ARP_IPSPRO,
      LE_IP_ARP_IPTHDW,
      LE_IP_ARP_IPTPRO,

      LE_VAR_VCD,
      LE_VAR_SLOT,
      LE_VAR_PORT,
      LE_VAR_SUBINTF,

      ATM_VAR_LOC_NSAP,
      ATM_VAR_LOC_NSAP_LEN,
      ATM_VAR_RMT_NSAP,
      ATM_VAR_RMT_NSAP_LEN,
      
      ATM_SIG_BLLI_STRING,
      ATM_SIG_BLLI_LENGTH,
      ATM_SIG_BLLI_STRING_2,
      ATM_SIG_BLLI_LENGTH_2,
      ATM_SIG_BLLI_STRING_3,
      ATM_SIG_BLLI_LENGTH_3,		/* <== 42 */

      ATM_SIG_MULTIPOINT,
      
      LANE_CURRENT_VC,
      CURRENT_ENDPOINT_ID };

static char *lane_var_def_array[] = {
      LE_CP_VAR_MARKER_DEF,
      LE_CP_VAR_PROTOCOL_DEF,
      LE_CP_VAR_VERSION_DEF,
      LE_CP_VAR_OPCODE_DEF,
      LE_CP_VAR_STATUS_DEF,
      LE_CP_VAR_TRANS_ID_DEF,
      LE_CP_VAR_REQ_LECID_DEF,
      LE_CP_VAR_FLAGS_DEF,

      LE_CP_VAR_SRC_DST_TAG_DEF,
      LE_CP_VAR_SRC_DST_MAC_DEF,
      LE_CP_VAR_TGT_DST_TAG_DEF,
      LE_CP_VAR_TGT_DST_MAC_DEF,

      LE_CP_VAR_SRC_ATM_ADDR_DEF,
      LE_CP_VAR_LAN_TYPE_DEF,
      LE_CP_VAR_MAX_FRM_SZ_DEF,
      LE_CP_VAR_NUM_TLVS_DEF,
      LE_CP_VAR_NAME_SZ_DEF,
      LE_CP_VAR_TGT_ATM_ADDR_DEF,
      LE_CP_VAR_ELAN_NAME_DEF,

      /* Defaults for the IP ARP packet variables. */
      LE_IP_ARP_ARHRD_DEF,
      LE_IP_ARP_ARPRO_DEF,
      LE_IP_ARP_ARHLN_DEF,
      LE_IP_ARP_ARPLN_DEF,
      LE_IP_ARP_OPCODE_DEF,
      LE_IP_ARP_IPSHDW_DEF,
      LE_IP_ARP_IPSPRO_DEF,
      LE_IP_ARP_IPTHDW_DEF,
      LE_IP_ARP_IPTPRO_DEF,

      LE_VAR_VCD_DEF,
      LE_VAR_SLOT_DEF,
      LE_VAR_PORT_DEF,
      LE_VAR_SUBINTF_DEF,

      ATM_VAR_LOC_NSAP_DEF,
      ATM_VAR_LOC_NSAP_LEN_DEF,
      ATM_VAR_RMT_NSAP_DEF, 
      ATM_VAR_LOC_NSAP_LEN_DEF,
      
      ATM_SIG_BLLI_STRING_DEF,
      ATM_SIG_BLLI_LENGTH_DEF,
      ATM_SIG_BLLI_STRING_2_DEF,
      ATM_SIG_BLLI_LENGTH_2_DEF,
      ATM_SIG_BLLI_STRING_3_DEF,
      ATM_SIG_BLLI_LENGTH_3_DEF,	/* <== 42 */

      ATM_SIG_MULTIPOINT_DEF,
      
      LANE_CURRENT_VC_DEF,
      CURRENT_ENDPOINT_ID_DEF };

/*
 * Number of variables to deal with;
 * Note that this must be updated if variables are added!!
 */
#define LE_CP_VAR_COUNT			45

/*
 * Number of variables (fields) in the control packet header.
 * Note that this must match the number of fields in the lane_ctl_hdr_t_
 * struct, or the stuffing won't work properly.
 */
#define LE_CP_HDR_COUNT			8
 
/*
 * This is specific to the lane_cfg_pdu_t stuffing; there
 * is a group of this may vars we stuff into the buffer 
 * using a for loop.
 */
#define LE_CP_CFG_STUFF_GRP_SZ		4

/* Similar literal for the rfc_arp stuffing. */
#define LE_IP_ARP_STUFF_GRP_SZ		5

/*
 * Position constants so that we can stuff several vars in a row.
 */
#define LE_CP_LAN_TYPE_VAR_POS		13
#define LE_IP_ARP_ARHRD_POS		19


#endif _ATMTEST_LANE_ARRAYS_H_
