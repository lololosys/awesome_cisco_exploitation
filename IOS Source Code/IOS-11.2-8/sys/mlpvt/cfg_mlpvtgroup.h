/* $Id: cfg_mlpvtgroup.h,v 1.1.42.6 1996/09/15 18:09:58 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/cfg_mlpvtgroup.h,v $
 *------------------------------------------------------------------
 * cfg_mlpvtgroup.h -- MLPVT group configuration commands
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mlpvtgroup.h,v $
 * Revision 1.1.42.6  1996/09/15  18:09:58  perryl
 * CSCdi68323:  SGBP member help text is misleading
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/08/12  16:04:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.42.4  1996/05/18  22:34:40  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/07  23:34:33  perryl
 * CSCdi56945:  MLPVT seed-bid parameter can be simpler
 * Branch: California_branch
 *
 * Revision 1.1.42.2  1996/05/06  23:41:32  perryl
 * CSCdi56533:  Redundant MLPVT parameter, connect retry interval
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/04/27  06:34:13  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.7  1996/04/22  00:31:15  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.6  1996/04/03  22:58:51  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.5  1996/03/09  07:18:04  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.4  1996/03/02  02:57:17  abaindur
 * Branch: Dial1_branch
 *  Remove hooks that were added for multi-MLPVT groups per system.
 *
 * Revision 1.1.2.3  1996/03/01  22:58:02  abaindur
 * Branch: Dial1_branch
 *  Remove config knob for a tie-breaker metric used in case of equal
 *  MLPPP bundle mastership bids.
 *
 * Revision 1.1.2.2  1996/02/08  06:51:40  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:35  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:27  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/********************************************************************
 *
 *  [no] sgbp member <name-string> <IP-addr>
 *  [no] sgbp seed-bid {default | offload | forward-only | <value>}
 *
 *  OBJ(int,1)    - <value>
 *  OBJ(string,1) - <name-string>
 *  OBJ(paddr,1)  - <IP-addr>
 */

EOLS    (mlpvt_seedbid_forward_only_eol, mlpvtgroup_command, 
	 MLPVTGROUP_QUERYBID_FORWARD_ONLY);

KEYWORD	(mlpvt_seedbid_forward_only, mlpvt_seedbid_forward_only_eol, no_alt,
	 "forward-only", "hangup if unable to forward seed bid setting", 
	 PRIV_CONF);

EOLS    (mlpvt_seedbid_num_eol, mlpvtgroup_command, MLPVTGROUP_QUERYBID_NUM);

DECIMAL (mlpvt_seedbid_num, mlpvt_seedbid_num_eol, mlpvt_seedbid_forward_only,
	 OBJ(int,1), 0, (MLPVT_MASTERSHIP_CLAIM-1), 
	 "Seed bid value");

EOLS    (mlpvt_seedbid_offload_eol, mlpvtgroup_command,
	 MLPVTGROUP_QUERYBID_OFFLOAD);

KEYWORD (mlpvt_seedbid_offload, mlpvt_seedbid_offload_eol, mlpvt_seedbid_num,
	 "offload", "Offload server seed bid setting, use this for offload server", 
	 PRIV_CONF);

EOLS    (mlpvt_seedbid_default_eol, mlpvtgroup_command,
	 MLPVTGROUP_QUERYBID_DEFAULT);

KEYWORD (mlpvt_seedbid_default, mlpvt_seedbid_default_eol, mlpvt_seedbid_offload,
	 "default", "Default seed bid setting", 
	 PRIV_CONF);

NOPREFIX (mlpvt_seedbid_noprefix, mlpvt_seedbid_default,
	  mlpvt_seedbid_default_eol);

KEYWORD	(mlpvt_seedbid, mlpvt_seedbid_noprefix, no_alt,
	 "seed-bid", "mastership query seed bid", PRIV_CONF);

EOLS    (mlpvt_member_eol, mlpvtgroup_command, MLPVTGROUP_MEMBER);

IPADDR  (mlpvt_member_addr, mlpvt_member_eol, mlpvt_member_eol,
             OBJ(paddr,1), "member address");

NOPREFIX (mlpvt_member_noprefix, mlpvt_member_addr, mlpvt_member_eol);

STRING	(mlpvt_member_name, mlpvt_member_noprefix, no_alt,
	 OBJ(string,1), "member name");

KEYWORD	(mlpvt_member, mlpvt_member_name, mlpvt_seedbid,
	 "member", "SGBP group member configuration", PRIV_CONF);

 /********************************************************************
 *	
 *    sgbp source-ip A.B.C.D
 *    no sgbp source-ip A.B.C.D
 */
EOLS	(mlpvt_source_ipaddr_eol, mlpvtgroup_command, MLPVTGROUP_SOURCE_IP);

IPADDR  (mlpvt_source_ipaddr, mlpvt_source_ipaddr_eol, 
         no_alt, OBJ(paddr,1), "source ip address");

KEYWORD	(mlpvt_source_ip, mlpvt_source_ipaddr, mlpvt_member,
	 "source-ip", "SGBP source ip address", PRIV_CONF);

 /********************************************************************
 *	
 *    sgbp ppp-forward
 *    no sgbp ppp-forward 
 */
EOLS	(mlpvtppp_eol, mlpvtgroup_command, MLPVTGROUP_PPP_FWD);

KEYWORD	(mlpvt_ppp, mlpvtppp_eol, mlpvt_source_ip,
	 "ppp-forward", "SGBP participation for non-Multilink PPP also",
        PRIV_CONF);


 /********************************************************************
 *	
 *    sgbp group <name>
 *    no sgbp  group 
 * 
 * OBJ(string,1) - mlpvt group name <name>
 */

EOLS	(mlpvtgroup_eol, mlpvtgroup_command, MLPVTGROUP_NAME);

/* ... mlpvtgroup name ... */
STRING	(mlpvtgroup_name, mlpvtgroup_eol, no_alt,
	 OBJ(string,1), "group name");

NOPREFIX (mlpvtgroup_noprefix, mlpvtgroup_name, mlpvtgroup_eol);

KEYWORD	(mlpvt_group, mlpvtgroup_noprefix, mlpvt_ppp,
	 "group", "SGBP group name", PRIV_CONF);

NVGENNS	(mlpvtgroup_nvgen, mlpvt_group, mlpvtgroup_command);

KEYWORD	(configure_mlpvtgroup, mlpvtgroup_nvgen, ALTERNATE,
	 "sgbp",
	 "SGBP Stack Group Bidding Protocol configuration",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_mlpvtgroup
