/* $Id: cfg_int_llc2.h,v 3.2 1995/11/17 17:43:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/llc2/cfg_int_llc2.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ L L C 2 . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_llc2.h,v $
 * Revision 3.2  1995/11/17  17:43:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  00:07:44  vrawat
 * CSCdi37921:  LLC2 tuning parameters on IETF int are not picked up
 *
 * Revision 2.1  1995/07/04  01:54:37  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:06:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * llc2
 *
 * ll2 interface config commands
 */

/***************************************************************
 * llc2 frame-relay dynwind
 */
EOLS(llc2_dynwind_eol, llc2_ifcommand, LLC2_DYNWIND);
pdecl (llc2_dynwind_nop);
NUMBER(llc2_dynwind_dwc_val, llc2_dynwind_nop, no_alt,
OBJ(int, 2), 1, 16, "The new window size is divided by this number: 1, 2, 4, 8, or 16");
KEYWORD(llc2_dynwind_dwc, llc2_dynwind_dwc_val, llc2_dynwind_eol,
      "dwc", "Decrease the window size if BECN received",
      PRIV_CONF|PRIV_SUBIF);
NUMBER(llc2_dynwind_nw_val, llc2_dynwind_nop, no_alt,
      OBJ(int, 1), 1, 8, "Number of good I-frame received");
KEYWORD(llc2_dynwind_nw, llc2_dynwind_nw_val, llc2_dynwind_dwc,
    "nw", "Increase the window size for consecutive good I-frame received",
    PRIV_CONF|PRIV_SUBIF);
NOP(llc2_dynwind_nop, llc2_dynwind_nw, no_alt);
NVGENS(llc2_dynwind_nvgen, llc2_dynwind_nop, llc2_ifcommand,
       LLC2_DYNWIND);
KEYWORD(llc2_dynwind, llc2_dynwind_nvgen, no_alt,
       "dynwind", "Congestion control with dynamic window", PRIV_CONF|PRIV_SUBIF);



/***************************************************************
 * llc2 txq-max
 */
PARAMS_KEYONLY(llc2_txq_max, llc2_dynwind, "txq-max",
	       OBJ(int,1), 20, 200,
	       llc2_ifcommand, LLC2_TXQ_MAX,
	       "Queue for holding llc2 information frames",
	       "Depth of queue in I-frames", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 xid-retry-time
 */
PARAMS_KEYONLY(llc2_xid_rtry, llc2_txq_max, "xid-retry-time",
	       OBJ(int,1), 0, 60000,
	       llc2_ifcommand, LLC2_XID_RTRY_T,
	       "How long router waits for reply to XID",
	       "Wait time for reply to XID in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 adm-timer-value
 */
PARAMS_KEYONLY(llc2_adm_timer, llc2_xid_rtry, "adm-timer-value",
	       OBJ(int,1), 0, 60000,
	       llc2_ifcommand, LLC2_ADM_T,
	       "How long router waits in ADM before giving up.",
	       "Wait time for session activation.", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 xid-neg-val-time
 */
PARAMS_KEYONLY(llc2_xid_neg, llc2_adm_timer, "xid-neg-val-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_NEG_VAL_T,
	       "Frequency of exchange of identification (XID)",
	       "Frequency of XID in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 tpf-time
 */
PARAMS_KEYONLY(llc2_tpf, llc2_xid_neg, "tpf-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_TPF_T,
	       "Amount of time router waits for a final response to a poll"
	       " frame before re-sending the original poll frame",
	       "Tpf time in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 trej-time
 */
PARAMS_KEYONLY(llc2_trej, llc2_tpf, "trej-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_TREJ_T,
	       "Amount of time router waits for a resend of a rejected frame"
	       " before sending the reject command",
	       "Trej time in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 tbusy-time
 */
PARAMS_KEYONLY(llc2_tbusy, llc2_trej, "tbusy-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_TBUSY_T,
	       "Amount of time router waits while the other LLC2 station"
	       " is in a busy state before attempting to poll the remote station",
	       "Tbusy time in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 t1-time
 */
PARAMS_KEYONLY(llc2_t1, llc2_tbusy, "t1-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_T1_T,
	       "How long router waits for an acknowledgment to transmitted I-frames",
	       "T1 time in milliseconds",
	       PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 n2
 */
PARAMS_KEYONLY(llc2_n2, llc2_t1, "N2",
	       OBJ(int,1), 1, 255,
	       llc2_ifcommand, LLC2_N2,
	       "Number of times router should retry various operations",
	       "Number of times router should retry various operations",
	       PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 local-window
 */
PARAMS_KEYONLY(llc2_local, llc2_n2, "local-window",
	       OBJ(int,1), 1, 127,
	       llc2_ifcommand, LLC2_LOC_WIND,
	       "Max number of I-frames to send before waiting for an acknowledgment",
	       "Max number of I-frames to send before waiting for an acknowledgment",
	       PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2 idle-time
 */
PARAMS_KEYONLY(llc2_idle, llc2_local, "idle-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_IDLE_T,
	       "Frequency of polls during periods of idle traffic",
	       "Frequency of polls in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 * llc2  ack-delay-time
 */
PARAMS_KEYONLY(llc2_ack_delay, llc2_idle, "ack-delay-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_AKDELAY_T,
	       "Max time the router allows incoming I-frames to stay unacknowledged",
	       "Ack delay time in milliseconds", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 *   This command is for compatibility with old config files
 *   It has been replaced with "llc2 ack-delay-time"
 *
 * llc2  akdelay-time
 */
PARAMS_KEYONLY(llc2_akdelay, llc2_ack_delay, "akdelay-time",
	       OBJ(int,1), 1, 60000,
	       llc2_ifcommand, LLC2_AKDELAY_T,
	       "", "", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN|PRIV_NONVGEN );

/***************************************************************
 * llc2 ack-max
 */
PARAMS_KEYONLY(llc2_ack_max, llc2_akdelay, "ack-max",
	       OBJ(int,1), 1, 255,
	       llc2_ifcommand, LLC2_AKMAX,
	       "Max number of I-frames received before an acknowledgment must be sent",
	       "Ack max", PRIV_CONF|PRIV_SUBIF );

/***************************************************************
 *   This command is for compatibility with old config files
 *   It has been replaced with "llc2 ack-max"
 *
 * llc2 akmax
 */
PARAMS_KEYONLY(llc2_akmax, llc2_ack_max, "akmax",
	       OBJ(int,1), 1, 255,
	       llc2_ifcommand, LLC2_AKMAX,
	       "", "", PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN|PRIV_NONVGEN );

KEYWORD	(llc2, llc2_akmax, ALTERNATE, "llc2",
	 "LLC2 Interface Subcommands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	llc2
