/* $Id: cfg_int_channel.h,v 3.4.18.5 1996/08/12 16:02:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_channel.h,v $
 *------------------------------------------------------------------
 * cfg_int_channel.h
 *
 * February 1994, Stig Thormodsrud
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_channel.h,v $
 * Revision 3.4.18.5  1996/08/12  16:02:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.18.4  1996/06/17  23:31:07  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/05/28  07:44:36  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: California_branch
 *
 * Revision 3.4.18.2  1996/05/28  06:22:01  bchan
 * CSCdi58128:  add timing mark configuration and change keepalive
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/05/17  11:22:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.4.2  1996/04/09  09:04:27  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.4.1  1996/03/28  11:29:05  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4  1996/02/08  03:21:24  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.3  1995/12/01  15:56:15  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.2  1995/11/17  09:26:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  10:24:04  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.1  1995/07/02  10:53:07  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.4  1995/06/25  21:27:34  kmitchel
 * CSCdi35580:  CIP-SNA Virtual Lan: verbiage change - virtual to internal
 *
 * Revision 2.3  1995/06/21  18:16:36  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:19:10  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  22:04:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * Macro for constructing help strings with #defined defaults
 */
#define VALOF(a) #a
#define CONCAT(a,b) a##VALOF(b)

/*
 * TN3270 Server feature commands
 */
/*
 * Setup transition for TN3270S sub commands
 */
#define	TN3270S_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt),\
	      (trans_func) tn3270s_svr_subcommand, NULL}
#define	TN3270S_PU_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt),\
	  (trans_func) tn3270s_pu_subcommand, NULL}
#define	TN3270S_DLUR_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt),\
	  (trans_func) tn3270s_dlur_subcommand, NULL}
#define	TN3270S_DLUR_PU_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt),\
	  (trans_func) tn3270s_dlur_pu_subcommand, NULL}
#define	TN3270S_DLUR_SAP_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt),\
	  (trans_func) tn3270s_dlur_sap_subcommand, NULL}

pdecl(tn3270s_siftdown);

/***************************************************************
 * tn3270-svr ----------------------------------------------------
 * exit - exit from tn3270s configuration command
 *        Go back to start of interface cmds 
 *      exit
 * [no] exit
 */
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD NONE

EOLS	(tn3270s_svr_exit_eol,
	 tn3270s_exit_command, TN3270S_CFG_SVR);
KEYWORD	(tn3270s_svr_exit, tn3270s_svr_exit_eol, NONE,
         "exit", "Exit from TN3270 server configuration mode", 
	 PRIV_MIN);


/***  Timing mark Subcommand: ************************************
* tn3270-svr
*   timing-mark: Turn the sending of timing marks off or on.
*
*      timing-mark
* [no] timing-mark
*/
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_svr_exit

EOLS	(tn3270s_svr_timing_mark_eol,
	 tn3270s_cfg_command, TN3270S_CFG_SVR_TIMING_MARK);
KEYWORD	(tn3270s_svr_timing_mark, tn3270s_svr_timing_mark_eol,
         NEXT_TN3270S_SVR_SUBCMD, "timing-mark", "Send timing marks", 
	 PRIV_MIN);


#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_svr_timing_mark
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * exit - exit from tn3270s dlur configuration command
 *        Go back to start of tn3270 server commands
 *      exit
 * [no] exit
 */
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD NONE

EOLS	(tn3270s_dlur_exit_eol,
	 tn3270s_exit_command, TN3270S_CFG_DLUR);
KEYWORD	(tn3270s_dlur_exit, tn3270s_dlur_exit_eol, NONE,
         "exit", "Exit from TN3270 server DLUR configuration mode", 
	 PRIV_MIN);

#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD tn3270s_dlur_exit
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu---------------------------------------------
 * exit - exit from tn3270s dlur pu configuration command
 *        Go back to start of tn3270 dlur commands
 *      exit
 * [no] exit
 */
#undef  NEXT_TN3270S_DLUR_PU_SUBCMD
#define NEXT_TN3270S_DLUR_PU_SUBCMD NONE

EOLS	(tn3270s_dlur_pu_exit_eol,
	 tn3270s_exit_command, TN3270S_CFG_DLUR_PU);
KEYWORD	(tn3270s_dlur_pu_exit, tn3270s_dlur_pu_exit_eol, NONE,
         "exit", "Exit from TN3270 server DLUR PU configuration mode", 
	 PRIV_MIN);

#undef  NEXT_TN3270S_DLUR_PU_SUBCMD
#define NEXT_TN3270S_DLUR_PU_SUBCMD tn3270s_dlur_pu_exit
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      generic-pool { permit | deny } 
 * [no] generic-pool
 *
 * OBJ(int,1) = <generic_pool> TN3270S_GPOOL_PERMIT |
 *                             TN3270S_GPOOL_DENY
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      unbind-action { keep | disconnect } 
 * [no] unbind-action
 *
 * OBJ(int,1) = <unbind_action> TN3270S_UNBIND_KEEP |
 *                              TN3270S_UNBIND_DISCON
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      keepalive <seconds>
 * [no] keepalive
 * OBJ(int,1) = <seconds>
 *
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      idle-time <seconds> 
 * [no] idle-time
 *
 * OBJ(int,1) = <seconds>
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      tcp-port <port_num> 
 * [no] tcp-port
 *
 * OBJ(int,1) = <port_num>
 */
SET       (tn3270s_dlur_pu_siftdown, tn3270s_siftdown,
	   OBJ(int,20), TN3270S_MODE_DLUR_PU);
#undef  NEXT_TN3270S_DLUR_PU_SUBCMD
#define NEXT_TN3270S_DLUR_PU_SUBCMD tn3270s_dlur_pu_siftdown

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      pu-dns-name <pu-fqdn>
 * [no] pu-dns-name
 * OBJ(string,1) = <pu-fqdn>
 */
EOLS	   (tn3270s_dlur_pu_dns_eol, 
	    tn3270s_cfg_command, TN3270S_CFG_PU_DNS );
STRING     (tn3270s_dlur_pu_dns_fqdn, tn3270s_dlur_pu_dns_eol,
	    no_alt,
	    OBJ(string,1),"help string for <pu-fqdn>");
NOPREFIX   (tn3270s_dlur_pu_dns_no, tn3270s_dlur_pu_dns_fqdn,
	    tn3270s_dlur_pu_dns_eol);
NVGENS     (tn3270s_dlur_pu_dns_nvgen, tn3270s_dlur_pu_dns_no, 
  	    tn3270s_cfg_command, TN3270S_CFG_PU_DNS );
KEYWORD_ID_MM (tn3270s_dlur_pu_dns, tn3270s_dlur_pu_dns_nvgen,
	       NEXT_TN3270S_DLUR_PU_SUBCMD,
	       OBJ(int,20), TN3270S_MODE_DLUR_PU,
	       TN3270S_KEY_PU_DNS, "DLUR pu-dns-name keyword help text",
	       PRIV_CONF|PRIV_HIDDEN, 3);

#undef  NEXT_TN3270S_DLUR_PU_SUBCMD
#define NEXT_TN3270S_DLUR_PU_SUBCMD tn3270s_dlur_pu_dns
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur pu --------------------------------------------
 *      shutdown
 * [no] shutdown
 */
EOLS       (tn3270s_dlur_pu_shut_eols,
	    tn3270s_cfg_command, TN3270S_CFG_SHUT );
KEYWORD_ID (tn3270s_dlur_pu_shut, tn3270s_dlur_pu_shut_eols,
	    NEXT_TN3270S_DLUR_PU_SUBCMD,
	    OBJ(int,20), TN3270S_MODE_DLUR_PU,
	    TN3270S_KEY_SHUT, "Shutdown a TN3270 server DLUR PU",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_PU_SUBCMD
#define NEXT_TN3270S_DLUR_PU_SUBCMD  tn3270s_dlur_pu_shut
/*****************************************************************
 * Start of tn3270 DLUR pu sub commands
 */
NO_OR_DEFAULT(tn3270s_dlur_pu_cmds_no, NEXT_TN3270S_DLUR_PU_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);
HELP    (top_config_tn3270s_dlur_pu, tn3270s_dlur_pu_cmds_no,
         "TN3270 server DLUR PU configuration commands:\n");
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr-dlur -----------------------------------------------
 *      pu <pu-name> <idblk-idnum> <ip-address>
 * [no] pu <pu-name>
 *
 * if already exists, only <pu-name> is required
 *
 *      This command will cause entry into subcmd mode
 *      The parse tree base is set to point at:
 *        top_config_tn3270s_dlur_pu, if doing normal parsing
 *        tn3270s_dlur_pu_cmds_start, if doing nvgen 
 *     The sub commands are "chained" together
 *         via NEXT_TN3270S_DLUR_PU_SUBCMD  
 *
 * OBJ(string,1)= <pu-name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(string,2)= <idblk-idnum>
 * OBJ(paddr,1) = <ip-address>
 */

EOLS      (tn3270s_dlur_pu_eol,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_PU );
IPADDR    (tn3270s_dlur_pu_ipaddr, tn3270s_dlur_pu_eol,
	   no_alt,
	   OBJ(paddr,1), "IP address of this PU");

SET       (tn3270s_dlur_pu_new, tn3270s_dlur_pu_ipaddr,
	   OBJ(int,1), TRUE);

STRING    (tn3270s_dlur_pu_xid, tn3270s_dlur_pu_new,
	   tn3270s_dlur_pu_eol,
	   OBJ(string,2),"8 digit hexidecimal SNA Node id of this PU");
/* If the PU is already configured, only the name is reqd */
NOPREFIX  (tn3270s_dlur_pu_no, tn3270s_dlur_pu_xid,
	   tn3270s_dlur_pu_eol);
STRING    (tn3270s_dlur_pu_name, tn3270s_dlur_pu_no,
	   no_alt,
	   OBJ(string,1), "Unique name for this PU");
NVGENS    (tn3270s_dlur_pu_nvgen, tn3270s_dlur_pu_name,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_PU );
TN3270S_DLUR_PU_COMMAND(tn3270s_dlur_pu_subcmd, tn3270s_dlur_pu_nvgen, 
			no_alt);
KEYWORD_ID(tn3270s_dlur_pu, tn3270s_dlur_pu_subcmd, 
	   NEXT_TN3270S_DLUR_SUBCMD,
	   leave_subcommand, TRUE,
	   TN3270S_KEY_PU, "Configure a TN3270 server DLUR PU",
	   PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD tn3270s_dlur_pu
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur lsap ------------------------------------------
 * exit - exit from tn3270s dlur lsap configuration command
 *        Go back to start of tn3270 dlur commands
 *      exit
 * [no] exit
 */
#undef  NEXT_TN3270S_DLUR_SAP_SUBCMD
#define NEXT_TN3270S_DLUR_SAP_SUBCMD NONE

EOLS	(tn3270s_dlur_sap_exit_eol,
	 tn3270s_exit_command, TN3270S_CFG_DLUR_SAP);
KEYWORD	(tn3270s_dlur_sap_exit, tn3270s_dlur_sap_exit_eol, NONE,
         "exit", "Exit from TN3270 server DLUR LSAP configuration mode", 
	 PRIV_MIN);

#undef  NEXT_TN3270S_DLUR_SAP_SUBCMD
#define NEXT_TN3270S_DLUR_SAP_SUBCMD tn3270s_dlur_sap_exit
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur lsap ------------------------------------------
 *      link <name> [rmac <rmac>] [rsap <rsap>]
 * [no] link <name>
 *
 * if already exists, only <name> is required
 *
 * OBJ(string,1)= <name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(hwaddr,1)= <rmac>   DEFAULT 0000.0000.0000
 * OBJ(int,2)   = <rsap>   DEFAULT 4
 */
#define	LSAP_RMAC_SET	1<<1
#define	LSAP_RSAP_SET	1<<2

pdecl(tn3270s_dlur_sap_link_opts);

EOLS      (tn3270s_dlur_sap_link_eol,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP_LINK );

SET       (tn3270s_dlur_sap_link_new, tn3270s_dlur_sap_link_opts,
	   OBJ(int,1), TRUE);

OPT_HEXDIGIT(tn3270s_dlur_sap_link_rsap, tn3270s_dlur_sap_link_new,
	     no_alt,
	     OBJ(int,2), TN3270S_RSAP_MIN, TN3270S_RSAP_MAX,
	     CONCAT("Remote SAP address, default value is ",
		    TN3270S_RSAP_DEF));

KEYWORD_ORTEST(tn3270s_dlur_sap_link_key_rsap, tn3270s_dlur_sap_link_rsap, 
	       tn3270s_dlur_sap_link_eol,
	       OBJ(int,19), LSAP_RSAP_SET,
	       TN3270S_KEY_RSAP,
	       "Specify a remote SAP address",
	       PRIV_CONF);

ENADDR	  (tn3270s_dlur_sap_link_rmac, tn3270s_dlur_sap_link_new,
	   no_alt,
	   OBJ(hwaddr,1), "MAC address of the remote adapter");
KEYWORD_ORTEST(tn3270s_dlur_sap_link_key_rmac, tn3270s_dlur_sap_link_rmac, 
	       tn3270s_dlur_sap_link_key_rsap,
	       OBJ(int,19), LSAP_RMAC_SET,
	       TN3270S_KEY_RMAC,
	       "Specify a remote adapter MAC address",
	       PRIV_CONF);
NOP       (tn3270s_dlur_sap_link_opts, tn3270s_dlur_sap_link_key_rmac, NONE);

SET       (tn3270s_dlur_sap_link_set_rsap, tn3270s_dlur_sap_link_opts,
	   OBJ(int,2), TN3270S_RSAP_DEF);

/* If the link is already configured, only the name is reqd */
NOPREFIX  (tn3270s_dlur_sap_link_no, tn3270s_dlur_sap_link_set_rsap,
	   tn3270s_dlur_sap_link_eol);
STRING    (tn3270s_dlur_sap_link_name, tn3270s_dlur_sap_link_no,
	   no_alt,
	   OBJ(string,1), "Unique name for this link");
NVGENS    (tn3270s_dlur_sap_link_nvgen, tn3270s_dlur_sap_link_name,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP_LINK );

KEYWORD   (tn3270s_dlur_sap_lnk, tn3270s_dlur_sap_link_nvgen,
	   NEXT_TN3270S_DLUR_SAP_SUBCMD,
	   TN3270S_KEY_DLUR_SAP_LINK, "Configure a TN3270 server DLUR LSAP link",
	   PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SAP_SUBCMD
#define NEXT_TN3270S_DLUR_SAP_SUBCMD tn3270s_dlur_sap_lnk

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur lsap ------------------------------------------
 *      vrn <VRN-name>
 * [no] vrn
 *
 * OBJ(string,1) = <VRN-name>
 */
EOLS	   (tn3270s_dlur_sap_vrn_eol, 
	    tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP_VRN );
APPN_FQNAME(tn3270s_dlur_sap_vrn_name, tn3270s_dlur_sap_vrn_eol,
	    no_alt,
	    OBJ(string,1),"Fully qualified SNA name of the VRN");
NOPREFIX   (tn3270s_dlur_sap_vrn_no, tn3270s_dlur_sap_vrn_name,
	    tn3270s_dlur_sap_vrn_eol);
NVGENS     (tn3270s_dlur_sap_vrn_nvgen, tn3270s_dlur_sap_vrn_no, 
  	    tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP_VRN );
KEYWORD    (tn3270s_dlur_sap_vrn, tn3270s_dlur_sap_vrn_nvgen,
	    NEXT_TN3270S_DLUR_SAP_SUBCMD,
	    TN3270S_KEY_DLUR_SAP_VRN, "Specify the Virtual Routing Node name",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SAP_SUBCMD
#define NEXT_TN3270S_DLUR_SAP_SUBCMD tn3270s_dlur_sap_vrn
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 * tn3270-svr dlur lsap ------------------------------------------
 *      shutdown
 * [no] shutdown
 */
EOLS       (tn3270s_dlur_sap_shut_eols,
	    tn3270s_cfg_command, TN3270S_CFG_SHUT );
KEYWORD_ID (tn3270s_dlur_sap_shut, tn3270s_dlur_sap_shut_eols,
	    NEXT_TN3270S_DLUR_SAP_SUBCMD,
	    OBJ(int,20), TN3270S_MODE_DLUR_SAP,
	    TN3270S_KEY_SHUT, "Shutdown a TN3270 server DLUR LSAP",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SAP_SUBCMD
#define NEXT_TN3270S_DLUR_SAP_SUBCMD  tn3270s_dlur_sap_shut
/*****************************************************************
 * Start of tn3270 lsap sub commands
 */
NO_OR_DEFAULT(tn3270s_dlur_sap_cmds_no, NEXT_TN3270S_DLUR_SAP_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);
HELP    (top_config_tn3270s_dlur_sap, tn3270s_dlur_sap_cmds_no,
         "TN3270 server DLUR LSAP configuration commands:\n");
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr-dlur -----------------------------------------------
 *
 *      lsap <type> <adapno> [<lsap>]
 * [no] lsap <type> <adapno> [<lsap>]
 *
 *      This command will cause entry into subcmd mode
 *      The parse tree base is set to point at:
 *        top_config_tn3270s_dlur_sap, if doing normal parsing
 *        tn3270s_dlur_sap_cmds_start, if doing nvgen 
 *     The sub commands are "chained" together
 *         via NEXT_TN3270S_DLUR_SAP_SUBCMD  
 * OBJ(int,1)   = <lsap specified> TRUE | FALSE
 * OBJ(int,2)   = <type> CSNA_VLAN_ETHER |
 *                       CSNA_VLAN_TOKEN |
 *                       CSNA_VLAN_FDDI
 * OBJ(int,3)   = <adapno>
 * OBJ(int,4)   = <lsap>    DEFAULT xC0
 */

EOLS      (tn3270s_dlur_sap_eol,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP );

SET       (tn3270s_dlur_sap_new, tn3270s_dlur_sap_eol,
	   OBJ(int,1), TRUE);

OPT_HEXDIGIT(tn3270s_dlur_sap_lsap, tn3270s_dlur_sap_new,
	     tn3270s_dlur_sap_eol,
	     OBJ(int,4), TN3270S_LSAP_MIN, TN3270S_LSAP_MAX,
	     CONCAT("Local SAP address to be used on this adapter, default value is ",
		    TN3270S_LSAP_DEF));

NUMBER    (tn3270s_dlur_sap_adapno, tn3270s_dlur_sap_lsap,
	   no_alt,
	   OBJ(int,3), 0, CSNA_VLAN_MAX_VADAPTER_NUM, 
	   "Relative Adapter Number on the csna lan");

KEYWORD_ID(tn3270s_dlur_sap_type_fddi, tn3270s_dlur_sap_adapno, 
	   no_alt,
	   OBJ(int,2), CSNA_VLAN_FDDI, TN3270S_KEY_FDDI_ADAPTER,
	   TN3270S_HELP_STR_FDDI_ADAPTER, PRIV_CONF);
KEYWORD_ID(tn3270s_dlur_sap_type_token, tn3270s_dlur_sap_adapno,
	   tn3270s_dlur_sap_type_fddi,
	   OBJ(int,2), CSNA_VLAN_TOKEN, TN3270S_KEY_TOKEN_ADAPTER,
	   TN3270S_HELP_STR_TOKEN_ADAPTER, PRIV_CONF);
KEYWORD_ID(tn3270s_dlur_sap_type_ether, tn3270s_dlur_sap_adapno,
	   tn3270s_dlur_sap_type_token,
	   OBJ(int,2), CSNA_VLAN_ETHER, TN3270S_KEY_ETHER_ADAPTER,
	   TN3270S_HELP_STR_ETHER_ADAPTER, PRIV_CONF);
SET       (tn3270s_dlur_sap_set_lsap, tn3270s_dlur_sap_type_ether,
	   OBJ(int,4), TN3270S_LSAP_DEF);
NVGENS    (tn3270s_dlur_sap_nvgen, tn3270s_dlur_sap_set_lsap,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR_SAP );
TN3270S_DLUR_SAP_COMMAND(tn3270s_dlur_sap_subcmd, tn3270s_dlur_sap_nvgen, 
			no_alt);
KEYWORD_ID(tn3270s_dlur_sap, tn3270s_dlur_sap_subcmd, 
	   NEXT_TN3270S_DLUR_SUBCMD,
	   leave_subcommand, TRUE,
	   TN3270S_KEY_DLUR_SAP, "Configure a TN3270 server DLUR LSAP",
	   PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD tn3270s_dlur_sap
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 *      preferred-NNserver <NNserver>
 * [no] preferred-NNserver
 *
 * OBJ(string,1) = <NNserver>
 */
EOLS	   (tn3270s_dlur_NN_svr_eol, 
	    tn3270s_cfg_command, TN3270S_CFG_DLUR_NN_SVR );
APPN_FQNAME(tn3270s_dlur_NN_svr_name, tn3270s_dlur_NN_svr_eol,
	    no_alt,
	    OBJ(string,1), "Fully qualified SNA name of the"
	    " preferred Network Node");
NOPREFIX   (tn3270s_dlur_NN_svr_no, tn3270s_dlur_NN_svr_name,
	    tn3270s_dlur_NN_svr_eol);
NVGENS     (tn3270s_dlur_NN_svr_nvgen, tn3270s_dlur_NN_svr_no, 
  	    tn3270s_cfg_command, TN3270S_CFG_DLUR_NN_SVR );
KEYWORD    (tn3270s_dlur_NN_svr, tn3270s_dlur_NN_svr_nvgen,
	    NEXT_TN3270S_DLUR_SUBCMD,
	    TN3270S_KEY_DLUR_NN_SVR, "Specify a preference for"
	    " a particular Network Node offering CP services",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD tn3270s_dlur_NN_svr
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 *      dlus-backup <dlusname2>
 * [no] dlus-backup
 *
 * OBJ(string,1) = <dlusname2>
 */
EOLS	   (tn3270s_dlur_dlus_backup_eol, 
	    tn3270s_cfg_command, TN3270S_CFG_DLUR_DLUS );
APPN_FQNAME(tn3270s_dlur_dlus_backup_name, tn3270s_dlur_dlus_backup_eol,
	    no_alt,
	    OBJ(string,1),"Fully qualified SNA name of the"
	    " fallback Dependent LU Server");
NOPREFIX   (tn3270s_dlur_dlus_backup_no, tn3270s_dlur_dlus_backup_name,
	    tn3270s_dlur_dlus_backup_eol);
NVGENS     (tn3270s_dlur_dlus_backup_nvgen, tn3270s_dlur_dlus_backup_no, 
  	    tn3270s_cfg_command, TN3270S_CFG_DLUR_DLUS );
KEYWORD    (tn3270s_dlur_dlus_backup, tn3270s_dlur_dlus_backup_nvgen,
	    NEXT_TN3270S_DLUR_SUBCMD,
	    TN3270S_KEY_DLUR_DLUS, "Specify a fallback"
	    " Dependent LU Server (DLUS)",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD tn3270s_dlur_dlus_backup
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr dlur------------------------------------------------
 *      shutdown
 * [no] shutdown
 */
EOLS       (tn3270s_dlur_shut_eols,
	    tn3270s_cfg_command, TN3270S_CFG_SHUT );
KEYWORD_ID (tn3270s_dlur_shut, tn3270s_dlur_shut_eols,
	    NEXT_TN3270S_DLUR_SUBCMD,
	    OBJ(int,20), TN3270S_MODE_DLUR,
	    TN3270S_KEY_SHUT, "Shutdown the TN3270 server DLUR feature",
	    PRIV_CONF);
#undef  NEXT_TN3270S_DLUR_SUBCMD
#define NEXT_TN3270S_DLUR_SUBCMD  tn3270s_dlur_shut
/*****************************************************************
 * Start of tn3270 dlur sub commands
 */
NO_OR_DEFAULT(tn3270s_dlur_cmds_no, NEXT_TN3270S_DLUR_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);
HELP    (top_config_tn3270s_dlur, tn3270s_dlur_cmds_no,
         "TN3270 server Dependent LU Requester (DLUR) configuration commands:\n");

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      dlur <fq-cpname> <fq-dlusname>
 * [no] dlur
 *
 * if already exists, <fq-cpname> and <fq-dlusname> are optional
 * otherwise required
 *
 *      This command will cause entry into subcmd mode
 *      The parse tree base is set to point at:
 *        top_config_tn3270s_dlur, if doing normal parsing
 *        tn3270s_dlur_cmds_start, if doing nvgen 
 *     The sub commands are "chained" together
 *         via NEXT_TN3270S_DLUR_SUBCMD  
 *
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 *
 * OBJ(string,1)= <fq-cpname>
 * OBJ(string,2)= <fq-dlusname>
 *
 */

EOLS      (tn3270s_dlur_eol,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR );
SET       (tn3270s_dlur_new, tn3270s_dlur_eol,
	   OBJ(int,1), TRUE);
APPN_FQNAME(tn3270s_dlur_dlusname, tn3270s_dlur_new,
	    no_alt,
	    OBJ(string,2), "Fully qualified SNA name of the"
	    " primary Dependent LU Server (DLUS)");
APPN_FQNAME(tn3270s_dlur_cpname, tn3270s_dlur_dlusname,
	    tn3270s_dlur_eol,
	    OBJ(string,1), "Fully qualified SNA name of the"
	    " Control Point (CP) to be used by DLUR");
NOPREFIX  (tn3270s_dlur_no, tn3270s_dlur_cpname,
	   tn3270s_dlur_eol);
NVGENS    (tn3270s_dlur_nvgen, tn3270s_dlur_no,
	   tn3270s_cfg_command, TN3270S_CFG_DLUR );
TN3270S_DLUR_COMMAND(tn3270s_dlur_subcmd, tn3270s_dlur_nvgen,
		     no_alt);
KEYWORD_ID(tn3270s_dlur, tn3270s_dlur_subcmd, 
	   NEXT_TN3270S_SVR_SUBCMD,
	   leave_subcommand, TRUE,
	   TN3270S_KEY_DLUR, "Configure the TN3270 server"
	   " Dependent LU Requester (DLUR) feature",
	   PRIV_CONF);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_dlur

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 * exit - exit from tn3270s pu configuration command
 *        Go back to start of tn3270 server commands
 *      exit
 * [no] exit
 */
#undef  NEXT_TN3270S_PU_SUBCMD
#define NEXT_TN3270S_PU_SUBCMD NONE

EOLS	(tn3270s_pu_exit_eol,
	 tn3270s_exit_command, TN3270S_CFG_PU);
KEYWORD	(tn3270s_pu_exit, tn3270s_pu_exit_eol, NONE,
         "exit", "Exit from TN3270 server direct PU configuration mode", 
	 PRIV_MIN);

#undef  NEXT_TN3270S_PU_SUBCMD
#define NEXT_TN3270S_PU_SUBCMD tn3270s_pu_exit
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      generic-pool { permit | deny } 
 * [no] generic-pool
 *
 * OBJ(int,1) = <generic_pool> TN3270S_GPOOL_PERMIT |
 *                             TN3270S_GPOOL_DENY
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      unbind-action { keep | disconnect } 
 * [no] unbind-action
 *
 * OBJ(int,1) = <unbind_action> TN3270S_UNBIND_KEEP |
 *                              TN3270S_UNBIND_DISCON
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      keepalive <seconds>
 * [no] keepalive
 * OBJ(int,1) = <seconds>
 *
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      idle-time <seconds>
 * [no] idle-time
 *
 * OBJ(int,1) = <seconds>
 */
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      tcp-port <port_num>
 * [no] tcp-port
 *
 * OBJ(int,1) = <port_num>
 */
SET       (tn3270s_pu_siftdown, tn3270s_siftdown,
	   OBJ(int,20), TN3270S_MODE_PU);
#undef  NEXT_TN3270S_PU_SUBCMD
#define NEXT_TN3270S_PU_SUBCMD tn3270s_pu_siftdown

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      pu-dns-name <pu-fqdn>
 * [no] pu-dns-name
 * OBJ(string,1) = <pu-fqdn>
 */
EOLS	   (tn3270s_pu_dns_eol, 
	    tn3270s_cfg_command, TN3270S_CFG_PU_DNS );
STRING     (tn3270s_pu_dns_fqdn, tn3270s_pu_dns_eol,
	    no_alt,
	    OBJ(string,1),"help string for <pu-fqdn>");
NOPREFIX   (tn3270s_pu_dns_no, tn3270s_pu_dns_fqdn,
	    tn3270s_pu_dns_eol);
NVGENS     (tn3270s_pu_dns_nvgen, tn3270s_pu_dns_no, 
  	    tn3270s_cfg_command, TN3270S_CFG_PU_DNS );
KEYWORD_ID_MM (tn3270s_pu_dns, tn3270s_pu_dns_nvgen,
	       NEXT_TN3270S_PU_SUBCMD,
	       OBJ(int,20), TN3270S_MODE_PU,
	       TN3270S_KEY_PU_DNS, "DIRECT pu-dns-name keyword help text",
	       PRIV_CONF|PRIV_HIDDEN, 3);
#undef  NEXT_TN3270S_PU_SUBCMD
#define NEXT_TN3270S_PU_SUBCMD tn3270s_pu_dns
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 * tn3270-svr pu--------------------------------------------------
 *      shutdown
 * [no] shutdown
 */
EOLS       (tn3270s_pu_shut_eols,
	    tn3270s_cfg_command, TN3270S_CFG_SHUT );
KEYWORD_ID (tn3270s_pu_shut, tn3270s_pu_shut_eols,
	    NEXT_TN3270S_PU_SUBCMD,
	    OBJ(int,20), TN3270S_MODE_PU,
	    TN3270S_KEY_SHUT, "Shutdown a TN3270 server direct PU",
	    PRIV_CONF);
#undef  NEXT_TN3270S_PU_SUBCMD
#define NEXT_TN3270S_PU_SUBCMD  tn3270s_pu_shut
/*****************************************************************
 * Start of tn3270 pu sub commands
 */
NO_OR_DEFAULT(tn3270s_pu_cmds_no, NEXT_TN3270S_PU_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);
HELP    (top_config_tn3270s_pu, tn3270s_pu_cmds_no,
         "TN3270 server direct PU configuration commands:\n");

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      pu <pu-name> <idblk-idnum> <ip-address> <type> <adapno> <lsap>
 *         [rmac <rmac>] [rsap <rsap>] [lu-seed <lu-name-stem>]
 *
 *      pu <pu-name>
 * [no] pu <pu-name>
 *
 *      This command will cause entry into subcmd mode
 *      The parse tree base is set to point at:
 *        top_config_tn3270s_pu, if doing normal parsing
 *        tn3270s_pu_cmds_start, if doing nvgen 
 *     The sub commands are "chained" together
 *         via NEXT_TN3270S_PU_SUBCMD  
 *
 * if already exists, only <pu-name> is required
 *
 * OBJ(string,1)= <pu-name>
 * OBJ(int,1)   = <remaining parameters specified> TRUE | FALSE
 * OBJ(string,2)= <idblk-idnum>
 * OBJ(paddr,1) = <ip-address>
 * OBJ(int,2)   = <type> CSNA_VLAN_ETHER |
 *                       CSNA_VLAN_TOKEN |
 *                       CSNA_VLAN_FDDI
 * OBJ(int,3)   = <adapno>
 * OBJ(int,4)   = <lsap>
 * OBJ(hwaddr,1)= <rmac>   DEFAULT 0000.0000.0000
 * OBJ(int,5)   = <rsap>   DEFAULT 4
 * OBJ(string,3)= <lu-seed>
 */
#define	PU_RMAC_SET	1<<1
#define	PU_RSAP_SET	1<<2
#define	PU_LUSEED_SET	1<<3

pdecl(tn3270s_pu_opts);

EOLS      (tn3270s_pu_eol,
	   tn3270s_cfg_command, TN3270S_CFG_PU );

STRING        (tn3270s_pu_luseed, tn3270s_pu_opts,
	       no_alt,
	       OBJ(string,3), "LU name stem");
KEYWORD_ORTEST(tn3270s_pu_key_luseed, tn3270s_pu_luseed, 
	       tn3270s_pu_eol,
	       OBJ(int,19), PU_LUSEED_SET,
	       TN3270S_KEY_PU_LUSEED,
	       "Specify the LU seed a TN3270 client can use to request a specific LU name",
	       PRIV_CONF);

OPT_HEXDIGIT  (tn3270s_pu_rsap, tn3270s_pu_opts,
	       no_alt,
	       OBJ(int,5), TN3270S_RSAP_MIN, TN3270S_RSAP_MAX,
	       CONCAT("Remote SAP address, default value is ",
	              TN3270S_RSAP_DEF));
KEYWORD_ORTEST(tn3270s_pu_key_rsap, tn3270s_pu_rsap, 
	       tn3270s_pu_key_luseed,
	       OBJ(int,19), LSAP_RSAP_SET,
	       TN3270S_KEY_RSAP,
	       "Specify a remote SAP address",
	       PRIV_CONF);

ENADDR	      (tn3270s_pu_rmac, tn3270s_pu_opts,
	       no_alt,
	       OBJ(hwaddr,1), "MAC address of the remote adapter");
KEYWORD_ORTEST(tn3270s_pu_key_rmac, tn3270s_pu_rmac, 
	       tn3270s_pu_key_rsap,
	       OBJ(int,19), LSAP_RMAC_SET,
	       TN3270S_KEY_RMAC,
	       "Specify a remote adapter MAC address",
	       PRIV_CONF);

NOP       (tn3270s_pu_opts, tn3270s_pu_key_rmac, NONE);
SET       (tn3270s_pu_set_rsap, tn3270s_pu_opts,
	   OBJ(int,5), TN3270S_RSAP_DEF);

OPT_HEXDIGIT(tn3270s_pu_lsap, tn3270s_pu_set_rsap,
	     no_alt,
	     OBJ(int,4), TN3270S_LSAP_MIN, TN3270S_LSAP_MAX,
	     "Local SAP address to be used on this adapter");
NUMBER    (tn3270s_pu_adapno, tn3270s_pu_lsap,
	   no_alt,
	   OBJ(int,3), 0, CSNA_VLAN_MAX_VADAPTER_NUM, 
	   "Relative Adapter Number on the csna lan");

KEYWORD_ID(tn3270s_pu_type_fddi, tn3270s_pu_adapno, 
	   no_alt,
	   OBJ(int,2), CSNA_VLAN_FDDI, TN3270S_KEY_FDDI_ADAPTER,
	   TN3270S_HELP_STR_FDDI_ADAPTER, PRIV_CONF);
KEYWORD_ID(tn3270s_pu_type_token, tn3270s_pu_adapno,
	   tn3270s_pu_type_fddi,
	   OBJ(int,2), CSNA_VLAN_TOKEN, TN3270S_KEY_TOKEN_ADAPTER,
	   TN3270S_HELP_STR_TOKEN_ADAPTER, PRIV_CONF);
KEYWORD_ID(tn3270s_pu_type_ether, tn3270s_pu_adapno,
	   tn3270s_pu_type_token,
	   OBJ(int,2), CSNA_VLAN_ETHER, TN3270S_KEY_ETHER_ADAPTER,
	   TN3270S_HELP_STR_ETHER_ADAPTER, PRIV_CONF);

IPADDR    (tn3270s_pu_ipaddr, tn3270s_pu_type_ether,
	   no_alt,
	   OBJ(paddr,1), "IP address of this PU");

SET       (tn3270s_pu_new, tn3270s_pu_ipaddr,
	   OBJ(int,1), TRUE);

STRING    (tn3270s_pu_xid, tn3270s_pu_new,
	   tn3270s_pu_eol,
	   OBJ(string,2),"8 digit hexidecimal SNA Node id of this PU");
/* If the PU is already configured, only the name is reqd */
NOPREFIX  (tn3270s_pu_no, tn3270s_pu_xid,
	   tn3270s_pu_eol);
STRING    (tn3270s_pu_name, tn3270s_pu_no,
	   no_alt,
	   OBJ(string,1), "Unique name for this PU");
NVGENS    (tn3270s_pu_nvgen, tn3270s_pu_name,
	   tn3270s_cfg_command, TN3270S_CFG_PU );
TN3270S_PU_COMMAND(tn3270s_pu_subcmd, tn3270s_pu_nvgen,
		   no_alt);
KEYWORD_ID(tn3270s_pu, tn3270s_pu_subcmd, 
	   NEXT_TN3270S_SVR_SUBCMD,
	   leave_subcommand, TRUE,
	   TN3270S_KEY_PU, "Configure a TN3270 server direct PU",
	   PRIV_CONF);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_pu

#define FROM_TN3270S_SIFTDOWN_SVR     tn3270s_pu
#define FROM_TN3270S_SIFTDOWN_PU      tn3270s_pu_exit
#define FROM_TN3270S_SIFTDOWN_DLUR_PU tn3270s_dlur_pu_exit

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 */
EQUAL     (return_to_dlur_pu, FROM_TN3270S_SIFTDOWN_DLUR_PU,
	   no_alt, OBJ(int,20), TN3270S_MODE_DLUR_PU);
EQUAL     (return_to_pu, FROM_TN3270S_SIFTDOWN_PU,
	   return_to_dlur_pu, OBJ(int,20), TN3270S_MODE_PU);
EQUAL     (return_to_svr, FROM_TN3270S_SIFTDOWN_SVR,
	   return_to_pu, OBJ(int,20), TN3270S_MODE_SVR);

#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN return_to_svr

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      generic-pool { permit | deny } 
 * [no] generic-pool
 *
 * OBJ(int,1) = <generic_pool> TN3270S_GPOOL_PERMIT |
 *                             TN3270S_GPOOL_DENY
 */
EOLS	  (tn3270s_svr_gpool_eol, 
	   tn3270s_cfg_command, TN3270S_CFG_GPOOL );
KEYWORD_ID(tn3270s_svr_gpool_deny, tn3270s_svr_gpool_eol,
	   no_alt,
	   OBJ(int,1), TN3270S_GPOOL_DENY,
	   TN3270S_KEY_GPOOL_DENY,
	   "Leftover LUs will not be given to the generic pool",
	   PRIV_CONF);
KEYWORD_ID(tn3270s_svr_gpool_permit, tn3270s_svr_gpool_eol,
	   tn3270s_svr_gpool_deny,
	   OBJ(int,1), TN3270S_GPOOL_PERMIT, 
	   TN3270S_KEY_GPOOL_PERMIT, 
	   "Leftover LUs will be made available to TN3270s sessions",
	   PRIV_CONF);
NOPREFIX  (tn3270s_svr_gpool_no, tn3270s_svr_gpool_permit,
	   tn3270s_svr_gpool_eol);
NVGENS    (tn3270s_svr_gpool_nvgen, tn3270s_svr_gpool_no, 
	   tn3270s_cfg_command, TN3270S_CFG_GPOOL );
KEYWORD   (tn3270s_svr_gpool, tn3270s_svr_gpool_nvgen,
	   NEXT_TN3270S_SIFTDOWN,
	   TN3270S_KEY_GPOOL, "Specify usage of leftover LUs",
	   PRIV_CONF);
#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN tn3270s_svr_gpool
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      unbind-action { keep | disconnect } 
 * [no] unbind-action
 *
 * OBJ(int,1) = <unbind_action> TN3270S_UNBIND_KEEP |
 *                              TN3270S_UNBIND_DISCON
 */
EOLS	  (tn3270s_svr_unbind_eol, 
	   tn3270s_cfg_command, TN3270S_CFG_UNBIND_ACT );
KEYWORD_ID(tn3270s_svr_unbind_disc, tn3270s_svr_unbind_eol,
	   no_alt,
	   OBJ(int,1), TN3270S_UNBIND_ACT_DISC, 
	   TN3270S_KEY_UNBIND_DISC, 
	   "Disconnect the session following an UNBIND",
	   PRIV_CONF);
KEYWORD_ID(tn3270s_svr_unbind_keep, tn3270s_svr_unbind_eol,
	   tn3270s_svr_unbind_disc,
	   OBJ(int,1), TN3270S_UNBIND_ACT_KEEP, 
	   TN3270S_KEY_UNBIND_KEEP, 
	   "Keep the session following an UNBIND",
	   PRIV_CONF);
NOPREFIX  (tn3270s_svr_unbind_no, tn3270s_svr_unbind_keep,
	   tn3270s_svr_unbind_eol);
NVGENS    (tn3270s_svr_unbind_nvgen, tn3270s_svr_unbind_no, 
	   tn3270s_cfg_command, TN3270S_CFG_UNBIND_ACT );
KEYWORD   (tn3270s_svr_unbind, tn3270s_svr_unbind_nvgen,
	   NEXT_TN3270S_SIFTDOWN,
	   TN3270S_KEY_UNBIND_ACT, "Specify session action following an UNBIND",
	   PRIV_CONF);
#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN tn3270s_svr_unbind

/*****************************************************************
* tn3270-svr -----------------------------------------------------
*      keepalive <seconds>
*    | no keepalive
*    | keepalive        (didn't really want to support keepalive
*                        with no parameter, but if we don't then
*                        the command gets interpreted at a higher
*                        level, which is pretty confusing)
* OBJ(int,1) = <seconds>
*/
EOLS	 (tn3270s_svr_keepalive_eol, 
	  tn3270s_cfg_command, TN3270S_CFG_KEEPALIVE );
NUMBER	 (tn3270s_svr_keepalive_val, tn3270s_svr_keepalive_eol,
	  tn3270s_svr_keepalive_eol, OBJ(int,1), TN3270S_KEEPALIVE_MIN,
          TN3270S_KEEPALIVE_MAX, "keepalive period in seconds");
NOPREFIX (tn3270s_svr_keepalive_no, tn3270s_svr_keepalive_val,
	  tn3270s_svr_keepalive_eol);
NVGENS   (tn3270s_svr_keepalive_nvgen, tn3270s_svr_keepalive_no, 
	  tn3270s_cfg_command, TN3270S_CFG_KEEPALIVE);
SET       (tn3270s_svr_keepalive_set, tn3270s_svr_keepalive_nvgen,
           OBJ(int,1), TN3270S_KEEPALIVE_DEF);
KEYWORD  (tn3270s_svr_keepalive,  tn3270s_svr_keepalive_set,
	  NEXT_TN3270S_SIFTDOWN,
	  TN3270S_KEY_KEEPALIVE, "Specify keepalive",
	  PRIV_CONF);
#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN tn3270s_svr_keepalive

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      idle-time <seconds> 
 * [no] idle-time
 *
 * OBJ(int,1) = <seconds>
 */
EOLS	 (tn3270s_svr_idle_time_eol, 
	  tn3270s_cfg_command, TN3270S_CFG_IDLE_TIME );
NUMBER	 (tn3270s_svr_idle_time_val, tn3270s_svr_idle_time_eol,
	  no_alt,
	  OBJ(int,1), TN3270S_IDLE_TIME_MIN, TN3270S_IDLE_TIME_MAX,
	  "idle-time in seconds, after which an inactive LU session"
	  " will be disconnected. Specify 0 for no idle-time disconnection");
NOPREFIX (tn3270s_svr_idle_time_no, tn3270s_svr_idle_time_val,
	  tn3270s_svr_idle_time_eol);
NVGENS   (tn3270s_svr_idle_time_nvgen, tn3270s_svr_idle_time_no, 
	  tn3270s_cfg_command, TN3270S_CFG_IDLE_TIME );
KEYWORD  (tn3270s_svr_key_idle_time, tn3270s_svr_idle_time_nvgen,
	  NEXT_TN3270S_SIFTDOWN,
	  TN3270S_KEY_IDLE_TIME, "Specify idle-time disconnection",
	  PRIV_CONF);
#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN tn3270s_svr_key_idle_time
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      tcp-port <port_num> 
 * [no] tcp-port
 *
 * OBJ(int,1) = <port_num>
 */
EOLS	  (tn3270s_svr_tcp_port_eol, 
	   tn3270s_cfg_command, TN3270S_CFG_TCP_PORT );
NUMBER	  (tn3270s_svr_tcp_port_num, tn3270s_svr_tcp_port_eol,
	   no_alt,
	   OBJ(int,1), TN3270S_TCP_PORT_MIN, TN3270S_TCP_PORT_MAX,
	   "TCP port number");
NOPREFIX  (tn3270s_svr_tcp_port_no, tn3270s_svr_tcp_port_num, 
	   tn3270s_svr_tcp_port_eol);
NVGENS    (tn3270s_svr_tcp_port_nvgen, tn3270s_svr_tcp_port_no, 
	   tn3270s_cfg_command, TN3270S_CFG_TCP_PORT );
KEYWORD   (tn3270s_svr_tcp_port, tn3270s_svr_tcp_port_nvgen,
	   NEXT_TN3270S_SIFTDOWN,
	   TN3270S_KEY_TCP_PORT,
	   "TCP port number to be used by a TN3270 server telnet session",
	   PRIV_CONF);
#undef  NEXT_TN3270S_SIFTDOWN
#define NEXT_TN3270S_SIFTDOWN tn3270s_svr_tcp_port

NOP       (tn3270s_siftdown, NEXT_TN3270S_SIFTDOWN, NONE);
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 */
SET       (tn3270s_svr_siftdown, tn3270s_siftdown,
	   OBJ(int,20), TN3270S_MODE_SVR);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_svr_siftdown

/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      lu-dns-domain-stem <dns-fqdn><rev-dns-fqdn> 
 * [no] lu-dns-domain-stem
 *
 * OBJ(string,1) = <dns-fqdn>
 * OBJ(string,2) = <rev-dns-fqdn>
 */
EOLS	 (tn3270s_svr_lu_dns_eol, 
	  tn3270s_cfg_command, TN3270S_CFG_SVR_LU_DNS );
STRING   (tn3270s_svr_rev_dns_fqdn, tn3270s_svr_lu_dns_eol,
	  no_alt,
	  OBJ(string,2),"help string for <rev-dns-fqdn>");
STRING   (tn3270s_svr_dns_fqdn, tn3270s_svr_rev_dns_fqdn,
	  no_alt,
	  OBJ(string,1),"help string for <dns-fqdn>");
NOPREFIX (tn3270s_svr_lu_dns_no, tn3270s_svr_dns_fqdn,
	  tn3270s_svr_lu_dns_eol);
NVGENS   (tn3270s_svr_lu_dns_nvgen, tn3270s_svr_lu_dns_no, 
	  tn3270s_cfg_command, TN3270S_CFG_SVR_LU_DNS );
KEYWORD  (tn3270s_svr_lu_dns, tn3270s_svr_lu_dns_nvgen,
	  NEXT_TN3270S_SVR_SUBCMD,
	  TN3270S_KEY_SVR_LU_DNS, "lu-dns-domain-stem keyword help text",
	  PRIV_CONF|PRIV_HIDDEN);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_svr_lu_dns
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      maximum-lus <number> 
 * [no] maximum-lus
 *
 * OBJ(int,1) = <max_lu>
 */
EOLS	 (tn3270s_svr_maxlu_eol, 
	  tn3270s_cfg_command, TN3270S_CFG_SVR_MAX_LU );
NUMBER	 (tn3270s_svr_maxlu_val, tn3270s_svr_maxlu_eol,
	  no_alt,
	  OBJ(int,1), TN3270S_SVR_MAX_LU_MIN, TN3270S_SVR_MAX_LU_MAX,
	  CONCAT("maximum number of concurrent LUs, default value is ",
		 TN3270S_SVR_MAX_LU_DEF));
NOPREFIX (tn3270s_svr_maxlu_no, tn3270s_svr_maxlu_val,
	  tn3270s_svr_maxlu_eol);
NVGENS   (tn3270s_svr_maxlu_nvgen, tn3270s_svr_maxlu_no, 
	  tn3270s_cfg_command, TN3270S_CFG_SVR_MAX_LU );
KEYWORD  (tn3270s_svr_maxlu, tn3270s_svr_maxlu_nvgen,
	  NEXT_TN3270S_SVR_SUBCMD,
	  TN3270S_KEY_SVR_MAX_LU, "Set a limit on the number of concurrent LUs",
	  PRIV_CONF);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD tn3270s_svr_maxlu
/*****************************************************************
 * tn3270-svr ----------------------------------------------------
 *      shutdown
 * [no] shutdown
 */
EOLS       (tn3270s_svr_shut_eols,
	    tn3270s_cfg_command, TN3270S_CFG_SHUT );
KEYWORD_ID (tn3270s_svr_shut, tn3270s_svr_shut_eols,
	    NEXT_TN3270S_SVR_SUBCMD,
	    OBJ(int,20), TN3270S_MODE_SVR,
	    TN3270S_KEY_SHUT, "Shutdown the TN3270 server feature",
	    PRIV_CONF);
#undef  NEXT_TN3270S_SVR_SUBCMD
#define NEXT_TN3270S_SVR_SUBCMD  tn3270s_svr_shut
/*****************************************************************
 * Start of tn3270-server sub commands
 */
NO_OR_DEFAULT(tn3270s_cmds_no, NEXT_TN3270S_SVR_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);
HELP    (top_config_tn3270s, tn3270s_cmds_no,
         "TN3270 server configuration commands:\n");
/*****************************************************************
 * ---------------------------------------------------------------
 * [no] tn3270-server
 *      This command will cause entry into subcmd mode
 *      The parse tree base is set to point at:
 *        top_config_tn3270s, if doing normal parsing
 *        tn3270s_cmds_start, if doing nvgen 
 *     The sub commands are "chained" together
 *         via NEXT_TN3270S_SVR_SUBCMD  
 */
EOLS      (tn3270s_svr_eol,
	   tn3270s_cfg_command, TN3270S_CFG_SVR);
TN3270S_COMMAND(tn3270s_svr_subcmd, tn3270s_svr_eol,
		no_alt);
SET       (tn3270s_svr_set, tn3270s_svr_subcmd,
	   OBJ(int,20), TN3270S_MODE_INTERFACE);
KEYWORD_ID(tn3270s_svr, tn3270s_svr_set,
	   ALTERNATE,
	   leave_subcommand, TRUE,
	   TN3270S_KEY_SVR, "Configure the TN3270 server feature",
	   PRIV_CONF);
#undef  CHANNEL_NEXT_INTERFACE_SUBCMD
#define CHANNEL_NEXT_INTERFACE_SUBCMD tn3270s_svr

/* =========================== TN3270 =========================== */

/* =========================== TN3270 =========================== */

/*
 * Setup transition for Virtual Lan sub commands
 */
#define	VLAN_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt), (trans_func) csna_vlan_subcommand, NULL}
#define	VADAP_COMMAND(name, acc, alt)\
    static transition pname(name) =\
         {&pname(acc), &pname(alt), (trans_func) csna_vadap_subcommand, NULL}
/***************************************************************
 * exit - exit from vlan configuration command
 *        Go back to start of interface cmds 
 */
#undef  CSNA_NEXT_VLAN_SUBCMD
#define CSNA_NEXT_VLAN_SUBCMD NONE

EOLNS	(vlan_exit_eol, vlan_exit_command);
KEYWORD	(vlan_exit, vlan_exit_eol, NONE,
         "exit", "Exit from CIP internal Lan configuration mode", 
	 PRIV_MIN);

#undef  CSNA_NEXT_VLAN_SUBCMD
#define CSNA_NEXT_VLAN_SUBCMD vlan_exit
/***************************************************************
 * exit - exit from vadapter configuration command
 *        Go back to start of vlan cmds 
 */
#undef  CSNA_NEXT_VADAP_SUBCMD
#define CSNA_NEXT_VADAP_SUBCMD NONE

EOLNS	(vadap_exit_eol, vadap_exit_command);
KEYWORD	(vadap_exit, vadap_exit_eol, NONE,
         "exit", "Exit from CIP internal Lan Adapter configuration mode", 
	 PRIV_MIN);

#undef  CSNA_NEXT_VADAP_SUBCMD
#define CSNA_NEXT_VADAP_SUBCMD vadap_exit

#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN no_alt
/***************************************************************
 * (Note: CIP llc2 stack uses only the dynamic window NW value) 
 * llc2 Nw
 */
PARAMS_KEYONLY(csna_llc2_parm_nw, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_NW, OBJ(int,1), 0, 8,
	       csna_llc2_command, CSNA_LLC2_NW,
	       CSNA_LLC2_NEWKEY_NW
	       " - Increase the window size for consecutive good I-frame received"
	       " (zero is disabled)",
	       CONCAT("Congestion control slow-start interval"
		      ", default is ", CSNA_LLC2_DEF_NW),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_nw
/***************************************************************
 * llc2 t1-pf
 * llc2 tpf-time
 */
PARAMS_KEYONLY(csna_llc2_parm_t1_pf, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_T1_PF, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_T1_PF,
	       CSNA_LLC2_NEWKEY_T1_PF
	       " - Amount of time to wait for a final response to a poll"
	       " frame before re-sending the original poll frame",
	       CONCAT("T1 Poll Frequency time in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_T1_PF),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_t1_pf

/***************************************************************
 * llc2 t1-rej
 * llc2 trej-time
 */
PARAMS_KEYONLY(csna_llc2_parm_t1_rej, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_T1_REJ, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_T1_REJ,
	       CSNA_LLC2_NEWKEY_T1_REJ
	       " - Amount of time to wait for a resend of a rejected frame"
	       " before sending the reject command",
	       CONCAT("T1 rej time in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_T1_REJ),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_t1_rej
/***************************************************************
 * llc2 t1-busy
 * llc2 tbusy-time
 */
PARAMS_KEYONLY(csna_llc2_parm_t1_busy, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_T1_BUSY, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_T1_BUSY,
	       CSNA_LLC2_NEWKEY_T1_BUSY
	       " - Amount of time to wait while the other LLC2 station"
	       " is in a busy state before attempting to poll the remote station",
	       CONCAT("T1 busy time in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_T1_BUSY),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_t1_busy
/***************************************************************
 * llc2 t1-ack
 * llc2 t1-time
 */
PARAMS_KEYONLY(csna_llc2_parm_t1_ack, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_T1_ACK, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_T1_ACK,
	       CSNA_LLC2_NEWKEY_T1_ACK
	       " - How long to wait for an acknowledgment to transmit I-frames",
	       CONCAT("T1 Acknowledgement timer in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_T1_ACK),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_t1_ack
/***************************************************************
 * llc2 n2
 * llc2 N2
 */
PARAMS_KEYONLY(csna_llc2_parm_n2, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_N2, OBJ(int,1), 1, 255,
	       csna_llc2_command, CSNA_LLC2_N2,
	       CSNA_LLC2_NEWKEY_N2
	       " - Max retry count",
	       CONCAT("Max retry count"
		      ", default is ", CSNA_LLC2_DEF_N2),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_n2
/***************************************************************
 * (Note: new parameter for CIP llc2 stack) 
 * llc2 Rw
 * llc2 recv-window
 */
PARAMS_KEYONLY(csna_llc2_parm_rw, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_RW, OBJ(int,1), 1, 127,
	       csna_llc2_command, CSNA_LLC2_RW,
	       CSNA_LLC2_NEWKEY_RW
	       " - Receive window",
	       CONCAT("Receive window"
		      ", default is ", CSNA_LLC2_DEF_RW),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_rw
/***************************************************************
 * llc2 Tw
 * llc2 local-window
 */
PARAMS_KEYONLY(csna_llc2_parm_tw, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_TW, OBJ(int,1), 1, 127,
	       csna_llc2_command, CSNA_LLC2_TW,
	       CSNA_LLC2_NEWKEY_TW
	       " - Max number of I-frames to send before waiting for an acknowledgment",
	       CONCAT("Transmit window"
		      ", default is ", CSNA_LLC2_DEF_TW),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_tw
/***************************************************************
 * llc2 Ti
 * llc2 idle-time
 */
PARAMS_KEYONLY(csna_llc2_parm_ti, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_TI, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_TI,
	       CSNA_LLC2_NEWKEY_TI
	       " - Frequency of polls during periods of idle traffic",
	       CONCAT("Frequency of polls in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_TI),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_ti

/***************************************************************
 * llc2 T2
 * llc2 ack-delay-time
 */
PARAMS_KEYONLY(csna_llc2_parm_T2, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_T2, OBJ(int,1), 1, 60000,
	       csna_llc2_command, CSNA_LLC2_T2,
	       CSNA_LLC2_NEWKEY_T2
	       " - Max time for incoming I-frames to stay unacknowledged",
	       CONCAT("Ack delay time in milliseconds"
		      ", default is ", CSNA_LLC2_DEF_T2),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_T2

/***************************************************************
 * llc2 N3
 * llc2 ack-max
 */
PARAMS_KEYONLY(csna_llc2_parm_n3, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_N3, OBJ(int,1), 1, 255,
	       csna_llc2_command, CSNA_LLC2_N3,
	       CSNA_LLC2_NEWKEY_N3
	       " - Max number of I-frames received before an acknowledgment must be sent",
	       CONCAT("Delay acknowledgment count"
		      ", default is ", CSNA_LLC2_DEF_N3),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_n3

/***************************************************************
 * (Note: new parameter for CIP llc2 stack) 
 * llc2 N1
 */
PARAMS_KEYONLY(csna_llc2_parm_n1, CHANNEL_NEXT_IN_CHAIN, 
               CSNA_LLC2_KEY_N1, OBJ(int,1), 265, 4105,
	       csna_llc2_command, CSNA_LLC2_N1,
	       CSNA_LLC2_NEWKEY_N1
	       " - Maximum size of an I-frame in bytes",
	       CONCAT("Maximum size of an I-frame in bytes"
		      ", default is ", CSNA_LLC2_DEF_N1),
	       PRIV_CONF );
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN csna_llc2_parm_n1

/*****************************************************************
 * [no] llc2  some-parm some-val
 */
KEYWORD(csna_llc2_keyword, CHANNEL_NEXT_IN_CHAIN, CSNA_NEXT_VADAP_SUBCMD,
	"llc2", "CIP internal Lan Adapter LLC2 values",
	PRIV_CONF);

#undef  CSNA_NEXT_VADAP_SUBCMD
#define CSNA_NEXT_VADAP_SUBCMD csna_llc2_keyword
/*****************************************************************
 *      name <name>
 * [no] name 
 *
 * OBJ(string,1) = <name>
 */
EOLNS     (vadap_name_eols, csna_vadap_name_command);
STRING   (vadap_name_string, vadap_name_eols, no_alt,
	  OBJ(string,1), "Name");
NOPREFIX (vadap_name_no, vadap_name_string, vadap_name_eols);
NVGENNS   (vadap_name_nvgen, vadap_name_no, csna_vadap_name_command);
KEYWORD  (vadap_name, vadap_name_nvgen, CSNA_NEXT_VADAP_SUBCMD,
	 "name", "CIP internal Lan Adapter Name", PRIV_CONF);
#undef  CSNA_NEXT_VADAP_SUBCMD
#define CSNA_NEXT_VADAP_SUBCMD vadap_name
/*****************************************************************
 * Start of vadapter sub commands
 */
NO_OR_DEFAULT(vadap_cmds_no, CSNA_NEXT_VADAP_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);

HELP	(top_config_vadap, vadap_cmds_no,
	 "CIP internal Lan Adapter configuration commands:\n");

/*****************************************************************
 *      adapter <adapno> [<mac-address>]
 * [no] adapter <adapno>
 *
 *      This command will cause entry into subcmd mode, accomplished
 *      by the VADAP_COMMAND macro.  The parse tree base is set to 
 *      point at:
 *        top_config_vadap, if doing normal parsing
 *        vadap_cmds_start, if doing nvgen 
 *     The VADAP sub commands are "chained" together
 *         via CSNA_NEXT_VADAP_SUBCMD 
 *
 * OBJ(int,1)   = <adapno>
 * OBJ(hwaddr,1)= <mac-address>
 * OBJ(int,2)   =  presence of <mac-adddress>
 */

EOLNS (vlan_vadapter_eol, vlan_vadapter_command);
SET   (vlan_vadapter_mac_set1, vlan_vadapter_eol, OBJ(int,2), TRUE);
ENADDR	(vlan_vadapter_macaddr, vlan_vadapter_mac_set1, vlan_vadapter_eol,
	 OBJ(hwaddr,1), "Adapter IEEE MAC address");
NUMBER(vlan_vadapter_adapno, vlan_vadapter_macaddr, no_alt,
	OBJ(int,1), 0, CSNA_VLAN_MAX_VADAPTER_NUM, "Relative Adapter Number");
NVGENNS (vlan_vadapter_nvgen, vlan_vadapter_adapno, vlan_vadapter_command);
VADAP_COMMAND(vlan_vadapter_subcmd, vlan_vadapter_nvgen, no_alt);
KEYWORD_ID (vlan_vadapter, vlan_vadapter_subcmd, CSNA_NEXT_VLAN_SUBCMD,
	    leave_subcommand, TRUE,
	    "adapter", "Configure CIP internal Lan Adapter", PRIV_CONF);
#undef  CSNA_NEXT_VLAN_SUBCMD
#define CSNA_NEXT_VLAN_SUBCMD vlan_vadapter
/*****************************************************************
 *	source-bridge <local-ring> <bridge> <target_ring>
 *   no source-bridge 
 *
 * OBJ(int,1) = <local-ring>
 * OBJ(int,2) = <bridge>
 * OBJ(int,3) = <target-ring>
 */
EOLNS	(vlan_srb_eol, csna_vlan_srb_command);

NUMBER	(vlan_srb_targetring, vlan_srb_eol, no_alt,
	 OBJ(int,3), 1, CSNA_VLAN_SRB_MAX_RING_NUM, 
	 "Target ring number");
NUMBER	(vlan_srb_bridge, vlan_srb_targetring, no_alt,
	 OBJ(int,2), 1, CSNA_VLAN_SRB_MAX_BRIDGE_NUM, 
	 "Bridge number");
NUMBER	(vlan_srb_localring, vlan_srb_bridge, no_alt,
	 OBJ(int,1), 1, CSNA_VLAN_SRB_MAX_RING_NUM, 
	 "Local ring number");

NOPREFIX (vlan_srb_no, vlan_srb_localring, vlan_srb_eol);
NVGENNS   (vlan_srb_nvgen, vlan_srb_no, csna_vlan_srb_command);

KEYWORD (vlan_srb, vlan_srb_nvgen, CSNA_NEXT_VLAN_SUBCMD,
	 CSNA_VLAN_KEY_SRB, "Configure CIP internal Lan for source-route bridging",
	 PRIV_CONF);
#undef  CSNA_NEXT_VLAN_SUBCMD
#define CSNA_NEXT_VLAN_SUBCMD vlan_srb
/*****************************************************************
 *      bridge-group <group> 
 * [no] bridge-group
 *
 * OBJ(int,1) = <group>
 */
EOLNS	(vlan_bridge_eol, csna_vlan_bridge_command);

NUMBER	(vlan_bridge_group, vlan_bridge_eol, no_alt,
	 OBJ(int,1), 1, CSNA_VLAN_TB_MAX_BRIDGE_GRP,
	 "Bridge Group");

NOPREFIX (vlan_bridge_no, vlan_bridge_group, vlan_bridge_eol);
NVGENNS   (vlan_bridge_nvgen, vlan_bridge_no, csna_vlan_bridge_command);

KEYWORD (vlan_bridge, vlan_bridge_nvgen, CSNA_NEXT_VLAN_SUBCMD,
	 CSNA_VLAN_KEY_TB, "Configure CIP internal Lan for transparent bridging",
	 PRIV_CONF);
#undef  CSNA_NEXT_VLAN_SUBCMD
#define CSNA_NEXT_VLAN_SUBCMD vlan_bridge
/*****************************************************************
 * Start of vlan sub commands
 */
NO_OR_DEFAULT(vlan_cmds_no, CSNA_NEXT_VLAN_SUBCMD,
	   PRIV_MIN | PRIV_NONVGEN);

HELP	(top_config_vlan, vlan_cmds_no,
	 "CIP internal Lan configuration commands:\n");

/*****************************************************************
 * [no] lan <lan type> <lan id>
 *      This command will cause entry into subcmd mode, accomplished
 *      by the VLAN_COMMAND macro.  The parse tree base is set to 
 *      point at:
 *        top_config_vlan, if doing normal parsing
 *        vlan_cmds_start, if doing nvgen 
 *     The VLAN sub commands are "chained" together
 *         via CSNA_NEXT_VLAN_SUBCMD 
 *
 * OBJ(int,1) = <lan type> CSNA_VLAN_ETHER | CSNA_VLAN_TOKEN | CSNA_VLAN_FDDI
 * OBJ(int,2) = <lan id>
 */
EOLNS (csna_vlan_eol, csna_vlan_command);
NUMBER(csna_vlan_id, csna_vlan_eol, no_alt,
	OBJ(int,2), 0, CSNA_VLAN_MAX_LANID_NUM, "Internal Lan Id");
KEYWORD_ID(csna_vlan_type_fddi, csna_vlan_id, no_alt,
	OBJ(int,1), CSNA_VLAN_FDDI, "fddi",
        "FDDI", PRIV_CONF);
KEYWORD_ID(csna_vlan_type_token, csna_vlan_id, csna_vlan_type_fddi,
	OBJ(int,1), CSNA_VLAN_TOKEN, "tokenring",
        "TokenRing", PRIV_CONF);
KEYWORD_ID(csna_vlan_type_ether, csna_vlan_id, csna_vlan_type_token,
	OBJ(int,1), CSNA_VLAN_ETHER, "ethernet",
        "Ethernet", PRIV_CONF);
NVGENNS (csna_vlan_nvgen, csna_vlan_type_ether, csna_vlan_command);
VLAN_COMMAND(csna_vlan_subcmd, csna_vlan_nvgen, no_alt);
KEYWORD_ID(csna_vlan, csna_vlan_subcmd, CHANNEL_NEXT_INTERFACE_SUBCMD,
	   leave_subcommand, TRUE,
	   "lan", "CIP internal Lan configuration and subcommands", PRIV_CONF);
#undef  CHANNEL_NEXT_INTERFACE_SUBCMD
#define CHANNEL_NEXT_INTERFACE_SUBCMD csna_vlan
/*****************************************************************
 *      max-llc2-sessions <max-sessions>
 * [no] max-llc2-sessions
appn *
 * OBJ(int,1) = <max-sessions>
 */
PARAMS_KEYONLY(csna_vlan_max_llc2_sessions, CHANNEL_NEXT_INTERFACE_SUBCMD,
	       "max-llc2-sessions", OBJ(int,1), 
	       CSNA_VLAN_MIN_LLC2_SESSIONS, CSNA_VLAN_MAX_LLC2_SESSIONS,
	       csna_vlan_max_llc2_sess_command, CHANNEL_CFG_MAX_LLC2,
	       "Restrict the max number of concurrent CIP internal lan LLC2 sessions",
	       "Number of concurrent LLC2 sessions allowed - specify 0 for no restriction",
	       PRIV_CONF );
#undef  CHANNEL_NEXT_INTERFACE_SUBCMD
#define CHANNEL_NEXT_INTERFACE_SUBCMD csna_vlan_max_llc2_sessions
/******************************************************************
 *      csna <path> <device> [{maxpiu <size> | time-delay <time> | length-delay <piu-length>}]
 * [no] csna <path> <device>
 *
 * OBJ(int,8) = length-delay
 * OBJ(int,6) = time-delay
 * OBJ(int,4) = maxpiu
 *
 * OBJ(int,7) = (FALSE) | TRUE  length-delay
 * OBJ(int,5) = (FALSE) | TRUE  time-delay
 * OBJ(int,3) = (FALSE) | TRUE  maxpiu
 *
 * OBJ(int,2) = <device>
 * OBJ(int,1) = <path>
 * OBJ(idb,1) = <interface>
 */  

pdecl(csna_opts);

EOLS      (csna_eol, channel_interface_csna_command, CHANNEL_CSNA);
NUMBER    (csna_val_length_d, csna_opts, no_alt,
	   OBJ(int,8), CSNA_DEVICE_MIN_LENGTH_D, CSNA_DEVICE_MAX_LENGTH_D,
	   CONCAT("Block length in bytes, default is ",CSNA_DEVICE_DEF_LENGTH_D)
	   " Specify 0 to indicate blocking of all frames before transmission");
KEYWORD_ID(csna_key_length_d, csna_val_length_d, csna_eol,
	   OBJ(int,7), TRUE,
	   CSNA_DEVICE_KEY_LENGTH_D, 
	   "Minimum length of a block that can be transmitted to the mainframe before the "
	   "block delay time is expired",
	   PRIV_CONF);
NUMBER    (csna_val_time_d, csna_opts, no_alt,
	   OBJ(int,6), CSNA_DEVICE_MIN_TIME_D, CSNA_DEVICE_MAX_TIME_D,
	   CONCAT("Block delay time, in milliseconds, default value is ",CSNA_DEVICE_DEF_TIME_D));
KEYWORD_ID(csna_key_time_d, csna_val_time_d, csna_key_length_d,
	   OBJ(int,5), TRUE,
	   CSNA_DEVICE_KEY_TIME_D, 
	   "Amount of delay permitted to allow frames to be blocked for "
	   "transmission on the channel",
	   PRIV_CONF);
NUMBER    (csna_val_maxpiu, csna_opts, no_alt,
	   OBJ(int,4), CSNA_DEVICE_MIN_MAXPIU, CSNA_DEVICE_MAX_MAXPIU,
	   CONCAT("Size in bytes, default value is ",CSNA_DEVICE_DEF_MAXPIU));
KEYWORD_ID(csna_key_maxpiu, csna_val_maxpiu, csna_key_time_d,
	   OBJ(int,3), TRUE,
	   CSNA_DEVICE_KEY_MAXPIU,
	   "Maximum block length transmitted over the channel",
	   PRIV_CONF);
NOP       (csna_opts, csna_key_maxpiu, no_alt);
NOPREFIX  (csna_no, csna_opts, csna_eol);
OPT_HEXNUM(csna_device, csna_no, no_alt,
	   OBJ(int,2), "Device address");
OPT_HEXNUM(csna_path, csna_device, no_alt,
	   OBJ(int,1), "Hex path identifier");
NVGENS    (csna_nvgen, csna_path, channel_interface_csna_command, CHANNEL_CSNA);
KEYWORD   (channel_csna, csna_nvgen, CHANNEL_NEXT_INTERFACE_SUBCMD,
	   "csna", "Specify Channel csna device command and parameters", PRIV_CONF);
#undef  CHANNEL_NEXT_INTERFACE_SUBCMD
#define CHANNEL_NEXT_INTERFACE_SUBCMD channel_csna
/*****************************************************************
 * claw <path> <device> <ipaddr> <hostname> <cipname> <hostappl> <cipappl> [broadcast]
 * no claw <path> <device> 
 */
EOLS (channel_claw_eol, channel_interface_command, CHANNEL_CLAW);
KEYWORD_ID(channel_claw_brdcst, channel_claw_eol, channel_claw_eol,
	OBJ(int,3), 1, "broadcast",
        "Enable broadcast processing for this subchannel", PRIV_CONF);
STRING (channel_claw_cipappl, channel_claw_brdcst, channel_claw_eol,
	OBJ(string,4), "Workstation application name for the CLAW IP link");
STRING (channel_claw_hostappl, channel_claw_cipappl, no_alt,
	OBJ(string,3), "Host application name for the CLAW IP link");
STRING (channel_claw_cipname, channel_claw_hostappl, no_alt,
	OBJ(string,2), "Workstation name for this CLAW device");
STRING (channel_claw_hostname, channel_claw_cipname, no_alt,
	OBJ(string,1), "Host name for this CLAW device");
IPADDR (channel_claw_ipaddr, channel_claw_hostname, no_alt,
	OBJ(paddr,1), "IP address of the host application");
NOPREFIX (channel_claw_no, channel_claw_ipaddr, channel_claw_eol);
OPT_HEXNUM (channel_claw_device, channel_claw_no, no_alt,
	OBJ(int,2), "Device address");
OPT_HEXNUM (channel_claw_path, channel_claw_device, no_alt,
	   OBJ(int,1), "Hex path identifier");
NVGENS (channel_claw_nvgen, channel_claw_path, channel_interface_command, CHANNEL_CLAW);
KEYWORD (channel_claw, channel_claw_nvgen, CHANNEL_NEXT_INTERFACE_SUBCMD,
	 "claw", "Specify Channel CLAW mode", PRIV_CONF);

/*****************************************************************
 * cta-test-tool <command> <offset> <string>
 * no cta-test-tool <command> <offset>
 */
EOLS (channel_cta_tt_eol, channel_interface_cta_tt_cmd, 
      CHANNEL_CTA_TEST_TOOL);
HEXDATA (channel_cta_tt_hex, channel_cta_tt_eol, no_alt,
         OBJ(string,1), OBJ(int,3), "Hex CTA replacement values", 8);
NOPREFIX (channel_cta_tt_no, channel_cta_tt_hex, channel_cta_tt_eol);
NUMBER (channel_cta_tt_offset, channel_cta_tt_no, no_alt,
        OBJ(int,2), 0, 255, "CTA Message Offset");
HEXNUM (channel_cta_tt_cmdcode, channel_cta_tt_offset, no_alt,
	OBJ(int,1), "Hex CTA Command Code");
NVGENS (channel_cta_tt_nvgen, channel_cta_tt_cmdcode, 
        channel_interface_cta_tt_cmd, CHANNEL_CTA_TEST_TOOL);
KEYWORD (channel_cta_tt, channel_cta_tt_nvgen, channel_claw,
	 "cta-test-tool", "Specify CNA CTA Test Tool command",
         PRIV_CONF|PRIV_HIDDEN);
/*****************************************************************
 * offload <path> <device> <ipaddr> <hostname> <cipname> <hostappl> <cipappl>
 *         <hostappl2> <cipappl2> [broadcast]
 * no offload <path> <device>
 */
EOLS (channel_offload_eol, channel_interface_command, CHANNEL_OFFLOAD);
KEYWORD_ID(channel_offload_brdcst, channel_offload_eol, channel_offload_eol,
	   OBJ(int,3), 1, "broadcast",
	   "Enable broadcast processing for this subchannel", PRIV_CONF);
STRING (channel_offload_cip_api, channel_offload_brdcst, no_alt,
	OBJ(string,6), "Workstation application name for the CLAW API link");
STRING (channel_offload_host_api, channel_offload_cip_api, no_alt,
	OBJ(string,5), "Host application name for the CLAW API link");
STRING (channel_offload_cipappl, channel_offload_host_api, no_alt, 
	OBJ(string,4), "Workstation application name for the CLAW IP link");
STRING (channel_offload_hostappl, channel_offload_cipappl, no_alt,
	OBJ(string,3), "Host application name for the CLAW IP link");
STRING (channel_offload_cipname, channel_offload_hostappl, no_alt,
	OBJ(string,2), "Workstation name for this CLAW device");
STRING (channel_offload_hostname, channel_offload_cipname, no_alt,
	OBJ(string,1), "Host name for this CLAW device");
IPADDR (channel_offload_ipaddr, channel_offload_hostname, no_alt,
	OBJ(paddr,1), "IP address of the host application");
NOPREFIX (channel_offload_no, channel_offload_ipaddr, channel_offload_eol);
OPT_HEXNUM (channel_offload_device, channel_offload_no, no_alt,
	OBJ(int,2), "Device address");
OPT_HEXNUM (channel_offload_path, channel_offload_device, no_alt,
	   OBJ(int,1), "Hex path identifier");
NVGENS (channel_offload_nvgen, channel_offload_path, channel_interface_command, CHANNEL_OFFLOAD);
KEYWORD (channel_offload, channel_offload_nvgen, channel_cta_tt,
	 "offload", "Specify Channel Offload mode", PRIV_CONF);

/*****************************************************************
 * dp <path> <device> <ipaddr1> <ipaddr2>
 * no dp <path> <device> 
 */
EOLS (channel_dp_eol, channel_interface_command, CHANNEL_DP);
IPADDR (channel_dp_ipaddr2, channel_dp_eol, no_alt,
	OBJ(paddr,2), "IP address of the host application");
IPADDR (channel_dp_ipaddr, channel_dp_ipaddr2, no_alt,
	OBJ(paddr,1), "IP address of the host application");
NOPREFIX (channel_dp_no, channel_dp_ipaddr, channel_dp_eol);
OPT_HEXNUM (channel_dp_device, channel_dp_no, no_alt,
	OBJ(int,2), "Device address");
OPT_HEXNUM (channel_dp_path, channel_dp_device, no_alt,
	   OBJ(int,1), "Hex path identifier");
NVGENS (channel_dp_nvgen, channel_dp_path, channel_interface_command, CHANNEL_DP);
KEYWORD (channel_dp, channel_dp_nvgen, channel_offload,
	 "dp", "Specify Channel DP mode", PRIV_CONF|PRIV_HIDDEN);

/*****************************************************************
 * ivorie <path> <device> <ipaddr1> <ipaddr2>
 * [no] ivorie <path> <device> 
 */
EOLS (channel_ivorie_eol, channel_interface_command, CHANNEL_IVORIE);
IPADDR (channel_ivorie_ipaddr2, channel_ivorie_eol, no_alt,
	OBJ(paddr,2), "IP address of the host application");
IPADDR (channel_ivorie_ipaddr, channel_ivorie_ipaddr2, no_alt,
	OBJ(paddr,1), "IP address of the host application");
NOPREFIX (channel_ivorie_no, channel_ivorie_ipaddr, channel_ivorie_eol);
OPT_HEXNUM (channel_ivorie_device, channel_ivorie_no, no_alt,
	OBJ(int,2), "Device address");
OPT_HEXNUM (channel_ivorie_path, channel_ivorie_device, no_alt,
	   OBJ(int,1), "Hex path identifier");
NVGENS (channel_ivorie_nvgen, channel_ivorie_path, channel_interface_command, CHANNEL_IVORIE);
KEYWORD (channel_ivorie, channel_ivorie_nvgen, channel_dp,
	 "ivorie", "Specify Channel Ivorie mode", PRIV_CONF|PRIV_HIDDEN);

/*****************************************************************
 * channel protocol [S|S4] 
 * no channel-protocol
 */
EOLNS (channel_rate_eol, channel_rate_command);
STRING (channel_rate_value, channel_rate_eol, no_alt,
	OBJ(string,1), "S or S4");
NOPREFIX (channel_rate_no, channel_rate_value, channel_rate_eol);
NVGENNS (channel_rate_nvgen, channel_rate_no, channel_rate_command);
KEYWORD (channel_rate, channel_rate_nvgen, channel_ivorie,
	 "channel-protocol", "Channel Data Transfer Rate", PRIV_CONF);


TEST_IDBSTATUS(config_channel, channel_rate, NONE, ALTERNATE, IDB_CHANNEL);

#undef	ALTERNATE
#define	ALTERNATE	config_channel

