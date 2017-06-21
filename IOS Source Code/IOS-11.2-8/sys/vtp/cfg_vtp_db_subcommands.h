/* $Id: cfg_vtp_db_subcommands.h,v 1.1.6.3 1996/06/18 00:50:49 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_db_subcommands.h,v $
 *------------------------------------------------------------------
 * cfg_vtp_db_subcommands.h - CLI header file for VTP database subcommands.
 *
 * Jan 1996, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_db_subcommands.h,v $
 * Revision 1.1.6.3  1996/06/18  00:50:49  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.1.6.2  1996/05/21  22:29:12  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.1.6.1  1996/05/09  14:48:14  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.52.1  1996/04/27  07:38:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.2  1996/04/16  18:58:55  cyoung
 * Using 'exit','end' or '^Z' as an implicit apply breaks when the parser
 * needs to pop out of the sub-command scope to check against the global
 * parse chain.  Make 'apply' an explicit command to avoid this blunder.
 * CSCdi54003:  write to tftp file when it shouldnt, doesnt when it should
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.40.1  1996/04/08  02:42:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.24.1  1996/03/25  02:21:35  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/22  03:15:55  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.1.38.1  1996/02/27  21:20:13  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.6.3  1996/02/09  01:09:54  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.1.6.2  1996/01/23  00:47:09  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1.6.1  1996/01/11  01:47:45  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.1  1996/01/03  01:24:47  dcheng
 * Initial commit.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1996/01/03  01:19:17  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *
 * apply
 *
 *****************************************************************/
EOLS    (vtp_database_apply_eol, vtp_config_commands, VTP_DATABASE_APPLY);
KEYWORD (vtp_database_apply, vtp_database_apply_eol, NONE,
         "apply", "VTP VLAN database apply sub-command", PRIV_CONF);

/******************************************************************
 *
 * abort
 *
 *****************************************************************/
EOLS    (vtp_database_abort_eol, vtp_config_commands, VTP_DATABASE_ABORT);
KEYWORD (vtp_database_abort, vtp_database_abort_eol, vtp_database_apply,
         "abort", "VTP VLAN database abort sub-command", PRIV_CONF);

/******************************************************************
 *
 * [no] vlan <vlan-id>
 *     [name <vlan-name>]
 *     [sde <said>]                       
 *     [state {operational | suspended}]
 *     [mtu <mtu>]                                          
 *     [{ethernet                                                       | 
 *      {fddi | tokenring} ring-number <ring-number> parent-vlan <vlan> |   
 *      {fdi-net | tr-net} bridge-number <bridge-number> stp-type {srt | srb}}] 
 *
 *  OBJ(int, 1) = ISL VLAN ID
 *  OBJ(string, 1) = VLAN Name           
 *  OBJ(int, 2) = Logical OR of VTP_VLAN_NAME, VTP_VLAN_SAID, VTP_VLAN_STATE,
 *		                VTP_VLAN_MTU, VTP_VLAN_RING,
 *                              VTP_VLAN_PARENT, VTP_VLAN_BRIDGE, VTP_VLAN_STP
 *  OBJ(int, 3) = IEEE 801.10 SAID
 *  OBJ(int, 4) = MTU                 
 *  OBJ(int, 5) = VTP_VLAN_OPERATIONAL_STATE or VTP_VLAN_SUSPENDED_STATE
 *  OBJ(int, 6) = VTP_VLAN_ETH_TYPE, VTP_VLAN_FDDI_TYPE, VTP_VLAN_TR_TYPE, 
 *		  VTP_VLAN_FDDI_NET_TYPE, or VTP_VLAN_TR_NET_TYPE
 *  OBJ(int, 7) = ring number
 *  OBJ(int, 8) = parent vlan id
 *  OBJ(int, 9) = bridge number 
 *  OBJ(int, 10) = STP type      
 *  OBJ(int, 11) = TLB VLAN 1
 *  OBJ(int, 12) = TLB VLAN 2
 *  OBJ(int, 13) == TRUE if FDDI and ring-number present
 *
 *****************************************************************/

EOLS    (vtp_database_sub_eol, vtp_config_commands, VTP_DATABASE_VLAN);

pdecl(vlan_mtu);             /* Forward reference to allow looping */
pdecl(vlan_said);            /* Forward reference to allow looping */
pdecl(vlan_tlb1);            /* Forward reference to allow looping */
pdecl(vlan_tlb2);            /* Forward reference to allow looping */

/* FDDI-net/TR-net spanning tree type */
KEYWORD_ID (vlan_stp_type_3, vtp_database_sub_eol, no_alt, OBJ(int,10), 
            VTP_VLAN_STP_SRB, "IBM", "Use the IBM spanning tree", 
	    PRIV_CONF);
KEYWORD (vlan_stp_type_2, vtp_database_sub_eol, vlan_stp_type_3, 
           "IEEE", "Use the IEEE 802.1D spanning tree", PRIV_CONF);
SET (vlan_stp_type_1, vlan_stp_type_2, OBJ(int,10), VTP_VLAN_STP_SRT);

KEYWORD (vlan_net_2, vlan_stp_type_1, no_alt,      
           "stp-type", "spanning tree type", PRIV_CONF);

/* FDDI-net/TR-net bridge number */
NUMBER (vlan_net_val, vlan_net_2, no_alt,
         OBJ(int,9), 1, 15, "fddi-net/tr-net bridge number");
TESTVAR (vlan_net_test, vlan_net_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD (vlan_net_1, vlan_net_test, no_alt,      
           "bridge-number", "bridge-number", PRIV_CONF);

/* VLAN type */
KEYWORD_ID (vlan_type_5, vlan_net_1, vtp_database_sub_eol, OBJ(int,6), 
            VTP_VLAN_TR_NET_TYPE, "tr-net", "token ring net", PRIV_CONF);
/* VLAN type */
KEYWORD_ID_MM (vlan_type_4, vlan_net_1, vlan_type_5, OBJ(int,6), 
            VTP_VLAN_FDDI_NET_TYPE, "fddi-net", "fddi net", PRIV_CONF, 5);

/* FDDI/TR parent VLAN Id */
NUMBER (vlan_parent_val, vtp_database_sub_eol, no_alt,
         OBJ(int,8), 1, 1005, "FDDI/TR parent VLAN index");
TESTVAR (vlan_parent_test, vlan_parent_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD (vlan_ring_3, vlan_parent_test, no_alt,      
           "parent-vlan", "parent-vlan", PRIV_CONF);

/* FDDI/TR ring number */
NUMBER (vlan_ring_val, vlan_ring_3, no_alt,
         OBJ(int,7), 1, 4095, "fddi/token ring number");
TESTVAR (vlan_ring_test, vlan_ring_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);

/* TR */
KEYWORD (vlan_ring_2, vlan_ring_test,  no_alt,
         "ring-number", "Ring Number", PRIV_CONF);

/* VLAN type */
KEYWORD_ID (vlan_type_3, vlan_ring_2, vlan_type_4, OBJ(int,6), 
            VTP_VLAN_TR_TYPE, "tokenring", "token ring", PRIV_CONF);

/* FDDI ring number (optional)*/
KEYWORD_ID (vlan_ring_1, vlan_ring_test,  vtp_database_sub_eol,
	    OBJ(int,13), TRUE, "ring-number", "ring-number", PRIV_CONF);

/* VLAN type */
KEYWORD_ID (vlan_type_2, vlan_ring_1, vlan_type_3, OBJ(int,6), 
            VTP_VLAN_FDDI_TYPE, "fddi", "fddi", PRIV_CONF);

/* VLAN type */
KEYWORD_ID (vlan_type_1, vtp_database_sub_eol, vlan_type_2, OBJ(int, 6),
	    VTP_VLAN_ETH_TYPE, "ethernet", "ethernet", PRIV_CONF);

/* VLAN translational bridge 2 */
NUMBER (vlan_tlb2_val, vlan_tlb2, no_alt, OBJ(int,12), 1, 1005, 
	"ISL VLAN index");
TESTVAR (vlan_tlb2_test, vlan_tlb2_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD_OR (vlan_tlb2, vlan_tlb2_test, vlan_type_1, OBJ(int,2), VTP_VLAN_TLB2,
           "tlb-2", "VLAN Translational Bridge", PRIV_CONF);
 
/* VLAN translational bridge 1 */
NUMBER (vlan_tlb1_val, vlan_tlb1, no_alt, OBJ(int,11), 1, 1005, "ISL VLAN index");
TESTVAR (vlan_tlb1_test, vlan_tlb1_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD_OR (vlan_tlb1, vlan_tlb1_test, vlan_tlb2, OBJ(int,2), VTP_VLAN_TLB1,
           "tlb-1", "VLAN Translational Bridge", PRIV_CONF);

/* VLAN MTU */
NUMBER (vlan_mtu_val, vlan_mtu, no_alt, OBJ(int,4), 1500, 18190, "VLAN MTU");
TESTVAR (vlan_mtu_test, vlan_mtu_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD_OR (vlan_mtu, vlan_mtu_test, vlan_tlb1, OBJ(int,2), VTP_VLAN_MTU,
           "mtu", "VLAN MTU", PRIV_CONF);

/* VLAN state */
KEYWORD_ID (vlan_state_2, vlan_mtu, no_alt, OBJ(int, 5),
            VTP_VLAN_SUSPENDED_STATE, "suspended", "suspended", PRIV_CONF);
KEYWORD_ID (vlan_state_1, vlan_mtu, vlan_state_2, OBJ(int, 5),
            VTP_VLAN_OPERATIONAL_STATE, "operational", "operational", PRIV_CONF);
KEYWORD_OR (vlan_state, vlan_state_1, vlan_mtu, OBJ(int,2), VTP_VLAN_STATE,
           "state", "VLAN state", PRIV_CONF);

/* VLAN IEEE801.10 Said */
NUMBER (vlan_said_val, vlan_said, no_alt,
         OBJ(int,3), 1, 470000, "IEEE 801.10 Said");
TESTVAR (vlan_said_test, vlan_said_val,
         vtp_database_sub_eol, NONE, NONE, NONE, sense, TRUE);
KEYWORD_OR (vlan_said, vlan_said_test, vlan_state, OBJ(int,2), VTP_VLAN_SAID,
           "sde", "IEEE 801.10 Said", PRIV_CONF);

/* VLAN Name */
STRING (vlan_name_val, vlan_said, no_alt, OBJ(string,1),
        "Set new VTP domain name");
TEST_MULTIPLE_FUNCS(vlan_name_test, vlan_name_val, no_alt, NONE);
KEYWORD_OR (vlan_name, vlan_name_test, vlan_said, OBJ(int,2), VTP_VLAN_NAME,
           "name", "Set VLAN name", PRIV_CONF);

/* VLAN Id */
NOPREFIX (vlan_noprefix, vlan_name, vtp_database_sub_eol);
NUMBER (vlan_id_val, vlan_noprefix, no_alt,
         OBJ(int,1), 1, 1005, "ISL VLAN index");
NVGENS (vlan_nvgen, vlan_id_val, vtp_config_commands, VTP_DATABASE_VLAN);
KEYWORD (configure_vtp_database, vlan_nvgen, vtp_database_abort,
	 /*NONE,*/ "vlan", "VTP VLAN database sub-commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_vtp_database

