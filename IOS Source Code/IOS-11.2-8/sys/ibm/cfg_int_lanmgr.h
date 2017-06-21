/* $Id: cfg_int_lanmgr.h,v 3.4.26.1 1996/07/25 05:46:59 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_int_lanmgr.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L A N M G R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *
 * NOTE:
 * ONLY VALID IF DEBUG IS DEFINED:
 *  'station', 'softerr', 'notification-interval' and 'ring'
 *------------------------------------------------------------------
 * $Log: cfg_int_lanmgr.h,v $
 * Revision 3.4.26.1  1996/07/25  05:46:59  kchiu
 * CSCdi35303:  Token ring shut down with different segment number
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/18  03:23:04  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.3  1995/12/07  02:25:11  pleung
 * CSCdi34018:  LNM: password 1 command cannot be removed
 *
 * Revision 3.2  1995/11/17  09:21:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:47  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:06:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************************
 *
 * [no]  lnm ring-number-mistach-allowed
 *
 */

EOLS  (ci_lanmgr_ring_num_eol, lanmgrif_command, LM_RINGNUM_MISMATCH_ALLOWED);
NVGENS (ci_lanmgr_ring_num_nvgen, ci_lanmgr_ring_num_eol,
       lanmgrif_command, LM_RINGNUM_MISMATCH_ALLOWED);
KEYWORD (ci_lanmgr_ring_num_mismatch, ci_lanmgr_ring_num_nvgen, no_alt,
	"ring-number-mismatch-allowed", "Allow ring number mismatch",
        PRIV_CONF|PRIV_HIDDEN);

/*************************************************************************
 * lnm softerr <num>
 * no lnm softerr <num>
 *
 * OBJ(int,1) = <num>
 */
EOLS	(ci_lanmgr_err_eol, lanmgrif_command, LM_SOFTERR);
NUMBER (ci_lanmgr_err_num, ci_lanmgr_err_eol, no_alt,
	 OBJ(int,1), 0, -1, "Frequency of error reports in tens of milliseconds");
NOPREFIX (ci_lanmgr_err_noprefix, ci_lanmgr_err_num, ci_lanmgr_err_eol);
NVGENS	(ci_lanmgr_err_nvgen, ci_lanmgr_err_noprefix, 
	 lanmgrif_command, LM_SOFTERR);
KEYWORD (ci_lanmgr_err, ci_lanmgr_err_nvgen, ci_lanmgr_ring_num_mismatch,
	 "softerr", "Set frequency of error reports", PRIV_CONF);

/*************************************************************************
 * lnm station
 * no lnm station
 *
 */
#ifdef DEBUG
EOLS	(ci_lanmgr_sta_eol, lanmgrif_command, LM_RS);
KEYWORD (ci_lanmgr_sta, ci_lanmgr_sta_eol, ci_lanmgr_err,
	 "station", "", PRIV_CONF);
#else
NOP	(ci_lanmgr_sta, ci_lanmgr_err, NONE);
#endif

/*************************************************************************
 * lnm rps [force]
 * no lnm rps [force]
 *
 * OBJ(int,1) = TRUE if 'force', FALSE otherwise
 */
EOLS	(ci_lanmgr_rps_eol, lanmgrif_command, LM_RPS);
KEYWORD_ID (ci_lanmgr_rps_force, ci_lanmgr_rps_eol, ci_lanmgr_rps_eol,
	    OBJ(int,1), TRUE,
	    "force", "Force RPS active", PRIV_CONF);
NVGENS	(ci_lanmgr_rps_nvgen, ci_lanmgr_rps_force,
	 lanmgrif_command, LM_RPS);
KEYWORD (ci_lanmgr_rps, ci_lanmgr_rps_nvgen, ci_lanmgr_sta,
	 "rps", "Enable the Ring Parameter Server (RPS)", PRIV_CONF);

/*************************************************************************
 * lnm ring <num>
 * no lnm ring <num>
 *
 * OBJ(int,1) = <num>
 */
#ifdef DEBUG
EOLS	(ci_lanmgr_ring_eol, lanmgrif_command, LM_ALTERNATE);
NUMBER (ci_lanmgr_ring_num, ci_lanmgr_ring_eol, no_alt,
	 OBJ(int,1), 1, 4095, "");
NOPREFIX (ci_lanmgr_ring_noprefix, ci_lanmgr_ring_num, ci_lanmgr_ring_eol);
NVGENS	(ci_lanmgr_ring_nvgen, ci_lanmgr_ring_noprefix, 
	 lanmgrif_command, LM_ALTERNATE);
KEYWORD (ci_lanmgr_ring, ci_lanmgr_ring_nvgen, ci_lanmgr_rps,
	 "ring", "", PRIV_CONF);
#else
NOP	(ci_lanmgr_ring, ci_lanmgr_rps, NONE);
#endif

/*************************************************************************
 * lnm express-buffer
 * * no lnm express-buffer
 */
EOLS	(ci_lanmgr_exp_buf_eol, lanmgrif_command, LM_MAC_USE_EXP_BUF);
NVGENS	(ci_lanmgr_exp_buf_nvgen, ci_lanmgr_exp_buf_eol,
	 lanmgrif_command, LM_MAC_USE_EXP_BUF);
KEYWORD (ci_lanmgr_exp_buf, ci_lanmgr_exp_buf_nvgen, ci_lanmgr_ring,
	 "express-buffer", "MAC frames use express buffer", PRIV_CONF);

/*************************************************************************
 * lnm rem
 * * no lnm rem
 */
EOLS	(ci_lanmgr_rem_eol, lanmgrif_command, LM_REM);
NVGENS	(ci_lanmgr_rem_nvgen, ci_lanmgr_rem_eol,
	 lanmgrif_command, LM_REM);
KEYWORD (ci_lanmgr_rem, ci_lanmgr_rem_nvgen, ci_lanmgr_exp_buf,
	 "rem", "Enable the Ring Error Monitor (REM)", PRIV_CONF);

/*************************************************************************
 * lnm password <linkid> <password>
 * * no lnm password 
 *
 * OBJ(int,1) = <linkid>
 * OBJ(string,1) = <password>
 */
EOLS	(ci_lanmgr_pass_eol, lanmgrif_command, LM_PASSWORD);
STRING	(ci_lanmgr_pass_str, ci_lanmgr_pass_eol, no_alt,
	 OBJ(string,1), "Password (maximum 8 characters)");
NUMBER (ci_lanmgr_pass_num, ci_lanmgr_pass_str, no_alt,
	 OBJ(int,1), 0, LM_MAX_ALT_LMS, "Link to which this password applies");
NOPREFIX (ci_lanmgr_pass_noprefix, ci_lanmgr_pass_num, ci_lanmgr_pass_eol);
NVGENS	(ci_lanmgr_pass_nvgen, ci_lanmgr_pass_noprefix, 
	 lanmgrif_command, LM_PASSWORD);
KEYWORD (ci_lanmgr_pass, ci_lanmgr_pass_nvgen, ci_lanmgr_rem,
	 "password", "Link password", PRIV_CONF);


/*************************************************************************
 * lnm notification-interval <num>
 * * no lnm notification-interval [<num>]
 *
 * OBJ(int,1) = <num>
 */
#ifdef DEBUG
EOLS	(ci_lanmgr_notif_eol, lanmgrif_command, LM_NOTIF);
NUMBER (ci_lanmgr_notif_num, ci_lanmgr_notif_eol, no_alt,
	 OBJ(int,1), 0, 99, "Threshold");
NOPREFIX (ci_lanmgr_notif_noprefix, ci_lanmgr_notif_num, ci_lanmgr_notif_eol);
NVGENS	(ci_lanmgr_notif_nvgen, ci_lanmgr_notif_noprefix, 
	 lanmgrif_command, LM_NOTIF);
KEYWORD (ci_lanmgr_notif, ci_lanmgr_notif_nvgen, ci_lanmgr_pass,
	 "notification-interval", "", PRIV_CONF);
#else
NOP (ci_lanmgr_notif, ci_lanmgr_pass, NONE);
#endif

/*************************************************************************
 * lnm loss-threshold <num>
 * * no lnm loss-threshold [<num>]
 *
 * OBJ(int,1) = <num>
 */
EOLS	(ci_lanmgr_loss_eol, lanmgrif_command, LM_LOSS);
NUMBER (ci_lanmgr_loss_num, ci_lanmgr_loss_eol, no_alt,
	 OBJ(int,1), 0, 999, "Threshold");
NOPREFIX (ci_lanmgr_loss_noprefix, ci_lanmgr_loss_num, ci_lanmgr_loss_eol);
NVGENS	(ci_lanmgr_loss_nvgen, ci_lanmgr_loss_noprefix, 
	 lanmgrif_command, LM_LOSS);
KEYWORD (ci_lanmgr_loss, ci_lanmgr_loss_nvgen, ci_lanmgr_notif,
	 "loss-threshold", "Loss threshold", PRIV_CONF);


/*************************************************************************
 * lnm crs
 * * no lnm crs
 */
EOLS	(ci_lanmgr_crs_eol, lanmgrif_command, LM_CRS);
NVGENS	(ci_lanmgr_crs_nvgen, ci_lanmgr_crs_eol, 
	 lanmgrif_command, LM_CRS);
KEYWORD (ci_lanmgr_crs, ci_lanmgr_crs_nvgen, ci_lanmgr_loss,
	 "crs", "Enable the Configuration Report Server (CRS)", PRIV_CONF);


/*************************************************************************
 * lnm alternate <num>
 * * no lnm alternate [<num>]
 *
 * OBJ(int,1) = <num>
 */
EOLS	(ci_lanmgr_alt_eol, lanmgrif_command, LM_ALTERNATE);
NUMBER (ci_lanmgr_alt_num, ci_lanmgr_alt_eol, no_alt,
	 OBJ(int,1), 0, LM_MAX_ALT_LMS, "Number of lan managers");
NOPREFIX (ci_lanmgr_alt_noprefix, ci_lanmgr_alt_num, ci_lanmgr_alt_eol);
NVGENS	(ci_lanmgr_alt_nvgen, ci_lanmgr_alt_noprefix, 
	 lanmgrif_command, LM_ALTERNATE);
KEYWORD (ci_lanmgr_alternate, ci_lanmgr_alt_nvgen, ci_lanmgr_crs,
	 "alternate", "Alternate number of lan managers", PRIV_CONF);

/*************************************************************************/
KEYWORD	(ci_lanmgr, ci_lanmgr_alternate, NONE,
	 "lnm", "IBM Lan Manager", PRIV_CONF);

TEST_BOOLEAN(ci_lanmgr_enable, ci_lanmgr, NONE, NONE, bridge_enable);
TEST_IDBSTATUS(ci_lanmgr_test, ci_lanmgr_enable, NONE, ALTERNATE, IDB_TR);

#undef  ALTERNATE
#define ALTERNATE ci_lanmgr_test
