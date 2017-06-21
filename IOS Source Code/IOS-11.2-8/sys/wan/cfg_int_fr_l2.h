/* $Id: cfg_int_fr_l2.h,v 3.1.2.4 1996/07/25 03:00:30 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_fr_l2.h,v $
 *------------------------------------------------------------------
 * Frame Relay SVC layer 2 (LAPF) interface configuration commands
 *
 * July 1996, Chifei Cheng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_fr_l2.h,v $
 * Revision 3.1.2.4  1996/07/25  03:00:30  chifei
 * CSCdi63992:  No-form of each LAPF command does not need value entry
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/05  21:00:27  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * hidden commands shouldn't goto frame_relay_command either
 *
 * Revision 3.1.2.2  1996/07/04  17:05:36  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Moved command handling out of frame_relay_command()
 *
 * Revision 3.1.2.1  1996/07/03  01:57:53  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1  1996/07/03  00:55:17  chifei
 * a placeholder for modularizing LAPF commands.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************
 * frame-relay lapf <parameter|test-event> [<number>]
 *	- <parameter> includes "t200", "t203", "n200", "n201", "k", "frmr".
 *	- OBJ(int,1) is coded keyword
 *	- If <parameter>, except for frmr, OBJ(int,2) is parameter value 
 *	- All test-event commands are hidden.
 */
EOLS	(ci_FR_lapf_eol, lapf_cfgCmd, FR_LAPF_CMD);

/*************************************************************
 * [no] frame-relay lapf retry <number>
 */
NUMBER (ci_FR_l2_retry_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 1, 65535, "link-up retry - # seconds");
KEYWORD_ID (ci_FR_l2_retry, ci_FR_l2_retry_val, no_alt,
	OBJ(int,1), LAPF_RETRY_CMD, "retry",
	"set LAPF link-up retry time period in seconds",
	PRIV_CONF|PRIV_HIDDEN);
 
/*************************************************************
 * [no] frame-relay lapf k <number>
 */
NUMBER (ci_FR_lapf_k_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 1, 127, "window size - # packets");
NOPREFIX(ci_FR_lapf_noprfx_k, ci_FR_lapf_k_val, ci_FR_lapf_eol);
KEYWORD_ID (ci_FR_lapf_k, ci_FR_lapf_noprfx_k, ci_FR_l2_retry,
	OBJ(int,1), LAPF_K_CMD, "k",
	"set LAPF k value, the max. of outstanding I frames to be sent",
	PRIV_CONF);
 
/*************************************************************
 * [no] frame-relay lapf t203 <number>
 */
NUMBER (ci_FR_lapf_t203_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 1, 65535, "idle timer - # seconds");
NOPREFIX(ci_FR_lapf_noprfx_t203, ci_FR_lapf_t203_val, ci_FR_lapf_eol);
KEYWORD_ID (ci_FR_lapf_t203, ci_FR_lapf_noprfx_t203, ci_FR_lapf_k,
	OBJ(int,1), LAPF_T203_CMD, "t203",
	"set LAPF T203, the idle timer period in seconds",
	PRIV_CONF);
 
/*************************************************************
 * [no] frame-relay lapf n200 <number>
 */
NUMBER (ci_FR_lapf_n200_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 0, 20, "# retransmissions");
NOPREFIX(ci_FR_lapf_noprfx_n200, ci_FR_lapf_n200_val, ci_FR_lapf_eol);
KEYWORD_ID (ci_FR_lapf_n200, ci_FR_lapf_noprfx_n200, ci_FR_lapf_t203,
	OBJ(int,1), LAPF_N200_CMD, "n200",
	"set LAPF N200, the max. retransmission count",
	PRIV_CONF);
 
/*************************************************************
 * [no] frame-relay lapf n201 <number>
 */
NUMBER (ci_FR_lapf_n201_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 1, 16384, "# octets");
NOPREFIX(ci_FR_lapf_noprfx_n201, ci_FR_lapf_n201_val, ci_FR_lapf_eol);
KEYWORD_ID (ci_FR_lapf_n201, ci_FR_lapf_noprfx_n201, ci_FR_lapf_n200,
	OBJ(int,1), LAPF_N201_CMD, "n201",
	"set LAPF N201, the max. length of Information field in I frame",
	PRIV_CONF);
 
/*************************************************************
 * [no] frame-relay lapf t200 <number>
 */
NUMBER (ci_FR_lapf_t200_val, ci_FR_lapf_eol, no_alt,
         OBJ(int,2), 1, 100, "retransmission timer - # 1/10 seconds");
NOPREFIX(ci_FR_lapf_noprfx_t200, ci_FR_lapf_t200_val, ci_FR_lapf_eol);
KEYWORD_ID (ci_FR_lapf_t200, ci_FR_lapf_noprfx_t200, ci_FR_lapf_n201,
	OBJ(int,1), LAPF_T200_CMD, "t200",
	"set LAPF T200, the retransmission time period in 1/10 seconds", 
	PRIV_CONF);
 
/*************************************************************
 * [no] frame-relay lapf frmr
 *	- default is to send FRMR
 */
KEYWORD_ID (ci_FR_lapf_frmr, ci_FR_lapf_eol, ci_FR_lapf_t200,
	OBJ(int,1), LAPF_FRMR_CMD, "frmr",
	"set LAPF option on sending FRMR at Frame Reject",
	PRIV_CONF);
 
/**************************************************************
 * frame-relay lapf <test-event>
 *	below are for internal testing and hence hidden
 *	OBJ(int,1) brings coded parameter to command handler with value
 *	matching to enum Lapf_event_index for lapf_event_table
 */
KEYWORD_ID (ci_FR_l2_rel, ci_FR_lapf_eol, ci_FR_lapf_frmr,
	    OBJ(int,1), FR_DL_REL_REQ, "test-rel", "Lapf rcv RelReq test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_est, ci_FR_lapf_eol, ci_FR_l2_rel,
	    OBJ(int,1), FR_DL_EST_REQ, "test-est", "Lapf rcv EstReq test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_rej, ci_FR_lapf_eol, ci_FR_l2_est,
	    OBJ(int,1), LAPF_REJ_INDEX, "test-rej", "Lapf send REJ test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_rnr, ci_FR_lapf_eol, ci_FR_l2_rej,
	    OBJ(int,1), LAPF_RNR_INDEX, "test-rnr", "Lapf send RNR test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_rr, ci_FR_lapf_eol, ci_FR_l2_rnr,
	    OBJ(int,1), LAPF_RR_INDEX, "test-rr", "Lapf send RR test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_frmr, ci_FR_lapf_eol, ci_FR_l2_rnr,
	    OBJ(int,1), LAPF_FRMR_INDEX, "test-frmr", "Lapf send FRMR test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_dm, ci_FR_lapf_eol, ci_FR_l2_rnr,
	    OBJ(int,1), LAPF_DM_INDEX, "test-dm", "Lapf send DM test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_disc, ci_FR_lapf_eol, ci_FR_l2_dm,
	    OBJ(int,1), LAPF_DISC_INDEX, "test-disc", "Lapf send DISC test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_ua, ci_FR_lapf_eol, ci_FR_l2_disc,
	    OBJ(int,1), LAPF_UA_INDEX, "test-ua", "Lapf send UA test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_sabme, ci_FR_lapf_eol, ci_FR_l2_ua,
	    OBJ(int,1), LAPF_SABME_INDEX, "test-sabme", "Lapf send SABME test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_i, ci_FR_lapf_eol, ci_FR_l2_sabme,
	    OBJ(int,1), LAPF_I_INDEX, "test-i", "Lapf send I frame test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_test, ci_FR_lapf_eol, ci_FR_l2_i,
	    OBJ(int,1), LAPF_TEST, "test-mode", "enter LAPF-only test mode",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_simulate, ci_FR_lapf_eol, ci_FR_l2_test,
	    OBJ(int,1), LAPF_LAPD, "simulate-lapd", "Simulating LAPD test",
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_FR_l2_clrsts, ci_FR_lapf_eol, ci_FR_l2_simulate,
	    OBJ(int,1), LAPF_CLRCNT, "clear-counters", "clear LAPF counters",
	    PRIV_CONF|PRIV_HIDDEN);

NVGENS  (ci_FR_lapf_nvgen, ci_FR_l2_clrsts, lapf_cfgCmd,
	FR_LAPF_CMD);
KEYWORD	(ci_FR_lapf, ci_FR_lapf_nvgen, no_alt,
	 "lapf", "set LAPF parameter", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	ci_FR_lapf
