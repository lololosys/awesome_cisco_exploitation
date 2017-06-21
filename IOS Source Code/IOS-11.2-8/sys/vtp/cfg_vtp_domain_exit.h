/* $Id: cfg_vtp_domain_exit.h,v 1.1.6.2 1996/08/12 16:09:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_domain_exit.h,v $
 *------------------------------------------------------------------
 * Parse tree to exit from vtp database global command.
 *
 * March 1996, Brad Benson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_domain_exit.h,v $
 * Revision 1.1.6.2  1996/08/12  16:09:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 1.1.6.1  1996/05/09  14:48:16  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.52.1  1996/04/27  07:38:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/08  02:42:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.24.1  1996/03/25  02:21:37  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/22  03:15:58  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS (exit_vtp_domain_eol, vtp_domain_exit);

KEYWORD (vtp_domain_exit_kwd, exit_vtp_domain_eol, NONE,
	 "exit", "Leave VTP domain mode",
	 PRIV_USER);

TESTVAR (vtp_domain_exit, NONE, vtp_domain_exit_kwd, NONE, NONE,
	 ALTERNATE, sense, FALSE);

#undef ALTERNATE
#define ALTERNATE vtp_domain_exit
	 
