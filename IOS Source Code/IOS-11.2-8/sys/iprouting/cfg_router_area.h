/* $Id: cfg_router_area.h,v 3.3.20.2 1996/05/22 18:10:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_area.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ A R E A . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_area.h,v $
 * Revision 3.3.20.2  1996/05/22  18:10:48  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Orignate -> Originate
 *
 * Revision 3.3.20.1  1996/04/16  19:01:22  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:33:55  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:31:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * no area <area-id>
 */
EOLS	(rtr_area_delete_eol, area_command, OSPF_AREA_DELETE);
TESTVAR	(rtr_area_delete, rtr_area_delete_eol, no_alt,
	 NONE, NONE, NONE,
	 sense, FALSE);

/**********************************************************************
 * [no] area <area-id> virtual-link <router-id>
 *		    [hello-interval <hello-seconds>]
 *		    [retransmit-interval <retransmit-seconds>]
 *		    [transmit-delay <transmit-seconds>]
 *		    [dead-interval <dead-seconds>]
 *		    [authentication-key <password>]
 *		    [message-digest-key <id> md5 <password>]
 *
 * (*OBJ(paddr,8)) = <area-id> if in IP address format
 * OBJ(int,1) = <area-id>
 * (*OBJ(paddr,1)) = <router-id>
 * OBJ(int,2) = Logical OR of OSPF_VIRT_AUTH, OSPF_VIRT_HELLO, OSPF_VIRT_DEAD,
 *	    OSPF_VIRT_RETRANS, OSPF_VIRT_TRANS
 *
 * OBJ(int,3) = <hello-seconds>
 * OBJ(int,4) = <retransmit-seconds>
 * OBJ(int,5) = <dead-seconds>
 * OBJ(int,6) = <transmit-seconds>
 * OBJ(int,7) = TRUE if no-summary for area stub command
 * OBJ(int,8) = <encryption type (0 for not yet encrypted, 7 for proprietary)>
 * OBJ(string,1) = <password>
 */

pdecl(rtr_area_virt_hello);    /* Forward reference to allow looping */

EOLS	(rtr_area_virt_eol, area_command, OSPF_AREA_VIRTUAL);

/* message-digest-key <password> */
/* Must be TEXT so that it will accept whitespace and comment leaders */
TEXT	(rtr_area_virt_auth_md_val, rtr_area_virt_eol, no_alt,
	 OBJ(string,1), "Authentication key (16 chars)");
TEST_MULTIPLE_FUNCS(rtr_area_virt_auth_md_test, rtr_area_virt_auth_md_val,
		    no_alt, NONE);
DECIMAL (rtr_area_virt_auth_md_enctype, rtr_area_virt_auth_md_val,
	 rtr_area_virt_auth_md_test,
	 OBJ(int,8), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 for not yet encrypted, 7 for proprietary)");
KEYWORD (rtr_area_virt_auth_md_md5, rtr_area_virt_auth_md_enctype, no_alt, 
	 "md5", "Use MD5 algorithm", PRIV_CONF|PRIV_SUBIF);
TESTVAR	(rtr_area_virt_auth_md_noprefix, rtr_area_virt_auth_md_md5,
	 rtr_area_virt_eol, NONE, NONE, NONE,
	 sense, TRUE);
NUMBER (rtr_area_virt_auth_md_keyid, rtr_area_virt_auth_md_noprefix,
        no_alt, OBJ(int,9), 1, 255, "Key ID");
KEYWORD_OR (rtr_area_virt_auth_md, rtr_area_virt_auth_md_keyid, 
	    rtr_area_virt_eol, OBJ(int,2), OSPF_VIRT_AUTH_MD,
	    "message-digest-key", "Set message digest key", 
	    PRIV_CONF);

/* authentication-key <password> */
/* Must be TEXT so that it will accept whitespace and comment leaders */
TEXT	(rtr_area_virt_auth_val, rtr_area_virt_eol, no_alt,
	 OBJ(string,1), "Authentication key (8 chars)");
TEST_MULTIPLE_FUNCS(rtr_area_virt_auth_test, rtr_area_virt_auth_val,
		    no_alt, NONE);
DECIMAL (rtr_area_virt_auth_enctype, rtr_area_virt_auth_val,
	 rtr_area_virt_auth_test,
	 OBJ(int,8), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 for not yet encrypted, 7 for proprietary)");
TESTVAR	(rtr_area_virt_auth_noprefix, rtr_area_virt_auth_enctype,
	 rtr_area_virt_eol, NONE, NONE, NONE,
	 sense, TRUE);
KEYWORD_OR(rtr_area_virt_auth, rtr_area_virt_auth_noprefix, 
	   rtr_area_virt_auth_md, OBJ(int,2), OSPF_VIRT_AUTH_CT,
	   "authentication-key", "Set authentication key", PRIV_CONF);

/* dead-interval <dead-seconds> */
NUMBER (rtr_area_virt_dead_val, rtr_area_virt_hello, no_alt,
	 OBJ(int,5), 1, 8192, "Seconds");
TESTVAR	(rtr_area_virt_dead_noprefix, rtr_area_virt_dead_val,
	 rtr_area_virt_hello, NONE, NONE, NONE,
	 sense, TRUE);
KEYWORD_OR(rtr_area_virt_dead, rtr_area_virt_dead_noprefix, rtr_area_virt_auth,
	   OBJ(int,2), OSPF_VIRT_DEAD,
	   "dead-interval", "Dead router detection time", PRIV_CONF);

/* transmit-delay */
NUMBER (rtr_area_virt_trans_val, rtr_area_virt_hello, no_alt,
	 OBJ(int,6), 1, 8192, "Seconds");
TESTVAR	(rtr_area_virt_trans_noprefix, rtr_area_virt_trans_val,
	 rtr_area_virt_hello, NONE, NONE, NONE,
	 sense, TRUE);
KEYWORD_OR (rtr_area_virt_trans, rtr_area_virt_trans_noprefix, 
	    rtr_area_virt_dead, OBJ(int,2), OSPF_VIRT_TRANS,
	   "transmit-delay", "LSA transmission delay", PRIV_CONF);

/* retransmit-interval <retransmit-seconds> */
NUMBER (rtr_area_virt_retrans_val, rtr_area_virt_hello, no_alt,
	 OBJ(int,4), 1, 8192, "Seconds");
TESTVAR	(rtr_area_virt_retrans_noprefix, rtr_area_virt_retrans_val,
	 rtr_area_virt_hello, NONE, NONE, NONE,
	 sense, TRUE);
KEYWORD_OR(rtr_area_virt_retrans, rtr_area_virt_retrans_noprefix,
	   rtr_area_virt_trans,
	   OBJ(int,2), OSPF_VIRT_RETRANS,
	   "retransmit-interval", "LSA retransmit interval", PRIV_CONF);

/* hello-interval <hello-seconds> */
NUMBER	(rtr_area_virt_hello_val, rtr_area_virt_hello, no_alt,
	 OBJ(int,3), 1, 8192, "Seconds");
TESTVAR	(rtr_area_virt_hello_noprefix, rtr_area_virt_hello_val,
	 rtr_area_virt_hello, NONE, NONE, NONE,
	 sense, TRUE);
KEYWORD_OR(rtr_area_virt_hello, rtr_area_virt_hello_noprefix,
	   rtr_area_virt_retrans,
	   OBJ(int,2), OSPF_VIRT_HELLO,
	   "hello-interval", "Hello packet interval", PRIV_CONF);

IPADDR	(rtr_area_virt_id, rtr_area_virt_hello, no_alt,
	 OBJ(paddr,1), "ID (IP addr) associated with virtual link neighbor");
KEYWORD (rtr_area_virt, rtr_area_virt_id, rtr_area_delete,
	 "virtual-link", "Define a virtual link and its parameters",
	 PRIV_CONF);


/**********************************************************************
 * [no] area <area-id> range <address> <mask>
 *
 * OBJ(int,1) = <area-id>
 * (*OBJ(paddr,1)) = <address>
 * (*OBJ(paddr,2)) = <mask>
 */
EOLS	(rtr_area_range_eol, area_command, OSPF_AREA_RANGE);
IPADDR	(rtr_area_range_mask, rtr_area_range_eol, no_alt,
	 OBJ(paddr,2), "IP mask for address");
IPADDR	(rtr_area_range_addr, rtr_area_range_mask, no_alt,
	 OBJ(paddr,1), "IP address to match");
KEYWORD (rtr_area_range, rtr_area_range_addr, rtr_area_virt,
	 "range",
	 "Summarize routes matching address/mask (border routers only)",
	 PRIV_CONF);

/**********************************************************************
 * area <area-id> nssa [no-redistribution] [default-information-originate]
 *		       [no-summary]
 *
 * OBJ(int,1) = <area-id>
 * OBJ(int,7) = TRUE if no-summary for NSSA 
 * OBJ(int,8) = TRUE if default-information-originate
 * OBJ(int,9) = TRUE if no-redistribution
 */
EOLS	(rtr_area_nssa_eol, area_command, OSPF_AREA_NSSA);
KEYWORD_ID (rtr_area_nssa_nosum, rtr_area_nssa_eol, rtr_area_nssa_eol,
	    OBJ(int,7), TRUE,
	    "no-summary", "Do not send summary LSA into NSSA",
	    PRIV_CONF);
KEYWORD_ID (rtr_area_nssa_def, rtr_area_nssa_nosum, rtr_area_nssa_nosum,
	    OBJ(int, 8), TRUE, "default-information-originate",
	    "Originate Type 7 default into NSSA area", PRIV_CONF);
KEYWORD_ID (rtr_area_nssa_no_redist, rtr_area_nssa_def, rtr_area_nssa_def,
	    OBJ(int, 9), TRUE, "no-redistribution", 
	    "No redistribution into this NSSA area", PRIV_CONF);
KEYWORD (rtr_area_nssa, rtr_area_nssa_no_redist, rtr_area_range,
	 "nssa", "Specify a NSSA area", PRIV_CONF);

/**********************************************************************
 * area <area-id> stub [no-summary]
 *
 * OBJ(int,1) = <area-id>
 * OBJ(int,7) = TRUE if no-summary for stub area
 */
EOLS	(rtr_area_stub_eol, area_command, OSPF_AREA_STUB);
KEYWORD_ID  (rtr_area_stub_nosum, rtr_area_stub_eol, rtr_area_stub_eol,
	     OBJ(int, 7), TRUE, 
	     "no-summary", "Do not send summary LSA into stub area", 
	     PRIV_CONF);
KEYWORD (rtr_area_stub, rtr_area_stub_nosum, rtr_area_nssa,
	 "stub", "Specify a stub area", PRIV_CONF);

/**********************************************************************
 * area <area-id> default-cost <cost>
 * no area <area-id> default-cost [<cost>]
 *
 * OBJ(int,1) = <area-id>
 * OBJ(int,2) = <cost>
 *
 */
EOLS	(rtr_area_defcost_eol, area_command, OSPF_AREA_DEFAULTCOST);
NUMBER	(rtr_area_defcost_cost, rtr_area_defcost_eol, no_alt,
	 OBJ(int,2), 0, 0xffffff, "Stub's advertised external route metric");
KEYWORD (rtr_area_defcost, rtr_area_defcost_cost, rtr_area_stub,
	 "default-cost", "Set the summary default-cost of a NSSA/stub area", 
	 PRIV_CONF);


/**********************************************************************
 * [no] area <area-id> authentication [message-digest]
 *
 * OBJ(int,1) = <area-id>
 * OBJ(int,2) = authentication type 1 or 2
 */
EOLS	(rtr_area_auth_eol, area_command, OSPF_AREA_AUTH);
KEYWORD_ID (rtr_area_auth_type2, rtr_area_auth_eol, rtr_area_auth_eol,
	    OBJ(int,2), OSPF_AUTH_MD, "message-digest", 
	    "Use message-digest authentication", PRIV_CONF);
KEYWORD_ID(rtr_area_auth, rtr_area_auth_type2, rtr_area_defcost,
	   OBJ(int,2), OSPF_AUTH_CT,
	   "authentication", "Enable authentication", PRIV_CONF);

/* area <area-id> */
NUMBER(rtr_area_id_num, rtr_area_auth, no_alt, 
	OBJ(int,1), 0, -1, "OSPF area ID as a decimal value");
IPADDR	(rtr_area_id, rtr_area_auth, rtr_area_id_num,
	 OBJ(paddr,8), "OSPF area ID in IP address format");
NVGENS	(rtr_area_nvgen, rtr_area_id, area_command, OSPF_AREA);

KEYWORD (rtr_area, rtr_area_nvgen, NONE,
	 "area", "OSPF area parameters", PRIV_CONF);
ASSERT	(router_area, rtr_area, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP &&
	 ((pdbtype *) csb->protocol)->proctype == PROC_OSPF);

#undef	ALTERNATE
#define ALTERNATE	router_area
