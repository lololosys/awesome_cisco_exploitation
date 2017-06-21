/* 
 * $Id: cfg_appn.h,v 3.2.10.2 1996/06/11 14:00:41 mregan Exp $
 * $Source: /release/112/cvs/Xsys/appn/cfg_appn.h,v $
 *------------------------------------------------------------------
 *
 * 25 April 1994, Dave McCowan
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 *------------------------------------------------------------------
 *
 * $Log: cfg_appn.h,v $
 * Revision 3.2.10.2  1996/06/11  14:00:41  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: California_branch
 * added code review comments.
 *
 * Revision 3.2.10.1  1996/05/17  10:39:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.2.1  1996/03/30  05:51:01  mregan
 * CSCdi53126:  APPN needs to support VDLC
 * Branch: IbuMod_Calif_branch
 * add VDLC support to APPN
 *
 * Revision 3.2  1996/02/26  06:52:49  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.1  1996/02/01  04:56:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:42:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:20:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/27  22:29:48  dmccowan
 * CSCdi36612:  minor problems with appn commands
 *
 * Revision 2.1  1995/06/07  22:01:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
*/


/******************************************************************
 * appn <appn object keyword>  <appn object name>
 */

EOLS	(cfg_appn_eol, appn_command, APPN_NAME);

APPN_FQNAME (cfg_appn_fqname, cfg_appn_eol, no_alt,
             OBJ(string,1), "Fully qualified name of APPN Object");

APPN_FQNAME (cfg_appn_fqname_opt, cfg_appn_eol, cfg_appn_eol,
             OBJ(string,1), "Fully qualified name of APPN Object");

APPN_TYPE_A_NAME (cfg_appn_a_name, cfg_appn_eol, no_alt,
             OBJ(string,1), "Name of APPN Object");

APPN_TYPE_A_NAME (cfg_appn_a_name_opt, cfg_appn_eol, cfg_appn_eol,
             OBJ(string,1), "Name of APPN Object");

INTERFACE (cfg_appn_int, cfg_appn_eol, cfg_appn_eol,
           OBJ(idb,1), IFTYPE_ALL);
KEYWORD_ID (cfg_appn_vdlc, cfg_appn_eol, cfg_appn_int,
 	    OBJ(int, 1), APPN_PORT_VDLC_TYPE,
 	    "vdlc", "Virtual DLC Port Type", PRIV_CONF);
KEYWORD_ID (cfg_appn_rsrb, cfg_appn_eol, cfg_appn_vdlc,
	    OBJ(int, 1), APPN_PORT_RSRB_TYPE,
	    "rsrb", "Remote Source Bridge Port Type", PRIV_CONF);

APPN_TYPE_A_NAME (cfg_appn_name_int, cfg_appn_rsrb, no_alt,
             OBJ(string,1), "Name of APPN Object");

IFELSE (cfg_appn_if6, cfg_appn_name_int, cfg_appn_a_name,
        (csb->appn_object_type==DEFPORT_POSITION));
IFELSE (cfg_appn_if5, cfg_appn_eol, cfg_appn_if6,
        (csb->appn_object_type==DEFSTART_POSITION));
IFELSE (cfg_appn_if4, cfg_appn_a_name_opt, cfg_appn_if5,
        (csb->appn_object_type==DEFMODE_POSITION));
IFELSE (cfg_appn_if3, cfg_appn_fqname_opt, cfg_appn_if4,
        (csb->appn_object_type==DEFRLU_LOC_POSITION));
IFELSE (cfg_appn_if2, cfg_appn_fqname, cfg_appn_if3,
        (csb->appn_object_type==DEFCN_POSITION));
IFELSE (cfg_appn_if1, cfg_appn_fqname_opt, cfg_appn_if2,
        (csb->appn_object_type==DEFCP_POSITION)); 

NVGENS    (cfg_appn_nvgen, cfg_appn_if1,
           appn_command, APPN_NAME);

APPN_COMMAND(cfg_appn_command, cfg_appn_nvgen, no_alt);

KEYWORD_ID(cfg_appn, cfg_appn_command, ALTERNATE,
	   leave_subcommand, TRUE,
	   "appn", "Configure APPN", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cfg_appn
