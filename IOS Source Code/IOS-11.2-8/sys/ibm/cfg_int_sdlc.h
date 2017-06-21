/* $Id: cfg_int_sdlc.h,v 3.5.12.3 1996/06/17 23:30:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_int_sdlc.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S D L C . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_sdlc.h,v $
 * Revision 3.5.12.3  1996/06/17  23:30:37  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/05/17  11:19:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.2  1996/04/11  14:37:41  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/04/03  14:33:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.12.1  1996/03/18  20:11:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/13  01:23:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.1  1996/02/20  00:53:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  21:27:12  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.4  1996/02/08  03:20:48  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.3  1996/02/01  23:33:22  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:21:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/28  19:00:54  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.1  1995/06/07  22:07:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * sdlc	address
 * sdlc	fair-poll-timer
 * sdlc	frmr-disable
 * sdlc	hold-queue
 * sdlc	k
 * sdlc	n1
 * sdlc	n2
 * sdlc	poll-limit-value
 * sdlc rnr-limit
 * sdlc	poll-pause-timer
 * sdlc	poll-wait-timeout
 * sdlc	t1
 * sdlc slow-poll
 * sdlc line-speed
 * sdlc hdx
 * sdlc cts-delay
 * sdlc rts_timeout
 * sdlc qllc-prtnr
 * sdlc simultaneous
 * sdlc virtual-multidrop
 * sdlc role
 * sdlc group-poll
 */

/***************************************************************
 * sdlc sdlc-largest-frame <sdlcaddr> <number>
 * no sdlc sdlc-largest-frame <sdlcaddr> [<number>]
 */
EOLS	(cfg_sdlc_sdlc_eol, sdlc_encaps, SDLC_SDLCLF);
NUMBER	(cfg_sdlc_sdlc_num, cfg_sdlc_sdlc_eol, no_alt,
	 OBJ(int,2), 0, -1, "Largest I-frame size");

NOPREFIX(cfg_sdlc_sdlc_noprefix, cfg_sdlc_sdlc_num, cfg_sdlc_sdlc_eol);

HEXDIGIT(cfg_sdlc_sdlc_ether, cfg_sdlc_sdlc_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC station address");

NVGENS	(cfg_sdlc_sdlc_nvgen, cfg_sdlc_sdlc_ether,
	 sdlc_encaps, SDLC_SDLCLF);

KEYWORD	(cfg_sdlc_sdlc, cfg_sdlc_sdlc_nvgen, no_alt,
	 "sdlc-largest-frame",
	 "Set the largest I-frame size that can be sent or"
	 " received by a SDLC station at a specified address",
	 PRIV_CONF);

/***************************************************************
 * sdlc partner
 *
 */
EOLS	(cfg_sdlc_prtnr_eol, sdlc_encaps, SDLC_PRTNR);
pdecl( cfg_sdlc_opts);

KEYWORD_ID(cfg_sdlc_outbound, cfg_sdlc_opts, no_alt,
	 OBJ(int,3), TRUE,
	 "outbound", "initiate connection to partner", PRIV_CONF);

KEYWORD_ID(cfg_sdlc_inbound, cfg_sdlc_opts, cfg_sdlc_outbound,
	 OBJ(int,2), TRUE,
	 "inbound", "partner will initiate connection", PRIV_CONF);

NOP	(cfg_sdlc_opts, cfg_sdlc_inbound, cfg_sdlc_prtnr_eol);

HEXDIGIT(cfg_sdlc_prtnr_sdlcaddr, cfg_sdlc_opts, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
ENADDR	(cfg_sdlc_prtnr_enaddr, cfg_sdlc_prtnr_sdlcaddr, no_alt,
	 OBJ(hwaddr,1), "Partners Token Ring MAC address");
NVGENS	(cfg_sdlc_prtnr_nvgen, cfg_sdlc_prtnr_enaddr,
	 sdlc_encaps, SDLC_PRTNR);
KEYWORD	(cfg_sdlc_prtnr, cfg_sdlc_prtnr_nvgen, cfg_sdlc_sdlc,
	 "partner", "MAC address for partner", PRIV_CONF);

/***************************************************************
 * sdlc saps <srcaddr> <dstaddr>
 *
 */
EOLS	(cfg_sdlc_saps_eol, sdlc_encaps, SDLC_SAPS);
HEXDIGIT(cfg_sdlc_saps_dstsap, cfg_sdlc_saps_eol, no_alt,
	 OBJ(int,3), 1, 0xFF, "Destination SAP of partner");
HEXDIGIT(cfg_sdlc_saps_srcsap, cfg_sdlc_saps_dstsap, no_alt,
	 OBJ(int,2), 1, 0xFF, "Source SAP of partner");
HEXDIGIT(cfg_sdlc_saps_addr, cfg_sdlc_saps_srcsap, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
NVGENS	(cfg_sdlc_saps_nvgen, cfg_sdlc_saps_addr,
	 sdlc_encaps, SDLC_SAPS);
KEYWORD	(cfg_sdlc_saps, cfg_sdlc_saps_nvgen, cfg_sdlc_prtnr,
	 "saps", "Configure lsap and dsap for token ring", PRIV_CONF);

/***************************************************************
 * sdlc xid <sdlcaddr> <xid> [xid0-n2 <number>]
 * no sdlc xid <sdlcaddr>
 */
EOLS	(cfg_sdlc_xid_eol, sdlc_encaps, SDLC_CFG_XID);
NUMBER  (cfg_sdlc_xid_num, cfg_sdlc_xid_eol, no_alt,
         OBJ(int,2), 1, 255,
         "Number of times XID0 is retried before restarting with XID NULL");
KEYWORD (cfg_sdlc_xid_retry, cfg_sdlc_xid_num, cfg_sdlc_xid_eol,
         "xid0-n2", "Configure number of times XID0 is retried before"
         " restarting with XID NULL", PRIV_CONF | PRIV_HIDDEN);
STRING   (cfg_sdlc_xid_string, cfg_sdlc_xid_retry, no_alt,
	 OBJ(string,1), "XID (must be 8 digits)");
NOPREFIX(cfg_sdlc_xid_noprefix, cfg_sdlc_xid_string, cfg_sdlc_xid_eol);
HEXDIGIT(cfg_sdlc_xid_addr, cfg_sdlc_xid_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFE, "SDLC address in Hex");
NVGENS	(cfg_sdlc_xid_nvgen, cfg_sdlc_xid_addr, sdlc_encaps, SDLC_CFG_XID);
KEYWORD (cfg_sdlc_xid, cfg_sdlc_xid_nvgen, cfg_sdlc_saps,
	 "xid",
	 "Specify an XID value to be associated with the"
	 " SDLC station at a specified address",
	 PRIV_CONF);

/**********************************************************************
 * [no] sdlc dlsw <sdlc address> <sdlc address> ...
 *      sdlc dlsw default
 *      sdlc dlsw partner <mac addr> [inbound | outbound}
 * OBJ(int,1) = number of valid stations in uiarray
 * uiarray    = array of stations
 * OBJ(int,3) = TRUE ===> sdlc dlsw partner
 * OBJ(int,4) = TRUE ===> inbound follows partner
 * OBJ(int,5) = TRUE ===> outbound follows partner
 * OBJ(int,6) = TRUE ===> sdlc dlsw default
 */

EOLS    (sslist_eol, sdlc_encaps, SDLC_DLSW);

SET        (dlsw_default_set, sslist_eol, OBJ(int,6), TRUE);
KEYWORD_MM (dlsw_default, dlsw_default_set, no_alt,
            "default", "DLSW to run over all configured SDLC Addresses",
             PRIV_CONF, 3);

pdecl(sdlc_io_options);
KEYWORD_ID (sdlc_outbound, sdlc_io_options, no_alt,
            OBJ(int,5), TRUE,
            "outbound", "initiate connection to partner", PRIV_CONF);
KEYWORD_ID (sdlc_inbound, sdlc_io_options, sdlc_outbound,
            OBJ(int,4), TRUE,
            "inbound", "partner will initiate connection", PRIV_CONF);
NOP      (sdlc_io_options, sdlc_inbound, sslist_eol);
ENADDR  (sdlc_prtnr_enaddr, sdlc_io_options, no_alt,
         OBJ(hwaddr,1), "Partners Token Ring MAC address");
KEYWORD_ID (sdlc_prtnr, sdlc_prtnr_enaddr, dlsw_default, OBJ(int,3), TRUE,
            "partner", "MAC address for default partner", PRIV_CONF);

pdecl(sslist_1a);

PRINT   (sslist_2b, no_alt, "Too many SDLC Addresses in list");
TESTVAR (sslist_2a, sslist_2b, sslist_1a,
         NONE, NONE, NONE, OBJ(int,1), (SDLC_MAX_ADDRESS + 1));
EVAL    (sslist_2, sslist_2a,
         csb->uiarray[GETOBJ(int,1) ++] = GETOBJ(int,9));
HEXDIGIT (sslist_1a, sslist_2, sslist_eol, OBJ(int,9), 1,
         (SDLC_MAX_ADDRESS - 1), "SDLC Address");
HEXDIGIT (sslist, sslist_2, sdlc_prtnr, OBJ(int,9), 1,
         (SDLC_MAX_ADDRESS - 1), "SDLC Address");

NVGENS  (dlsw_sdlc_nvgen, sslist, sdlc_encaps, SDLC_DLSW);
KEYWORD (dlsw_sdlc, dlsw_sdlc_nvgen, cfg_sdlc_xid,
         "dlsw", "Configure DLSW to run over SDLC", PRIV_CONF);

/***************************************************************
 * [no] sdlc group-poll <group address>
 *
 */

EOLS    (cfg_sdlc_grp_poll_eol, sdlc_encaps, SDLC_CMD_GROUP_POLL);

HEXDIGIT(cfg_sdlc_grp_addr, cfg_sdlc_grp_poll_eol, no_alt,
         OBJ(int,1), 1, 0xFF,
         "Group Address in Hex");

NVGENS  (cfg_sdlc_grp_poll_nvgen, cfg_sdlc_grp_addr,
         sdlc_encaps, SDLC_CMD_GROUP_POLL);
KEYWORD (cfg_sdlc_grp_poll, cfg_sdlc_grp_poll_nvgen, dlsw_sdlc,
         "group-poll",
         "Turn on group poll for this interface",
         PRIV_CONF);

/***************************************************************
 * [no] sdlc virtual-multidrop
 *
 */

EOLS	(cfg_sdlc_virtual_multidrop_eol, sdlc_encaps,
	 SDLC_CMD_VIRTUAL_MULTIDROP);
KEYWORD	(cfg_sdlc_virtual_multidrop, cfg_sdlc_virtual_multidrop_eol,
         cfg_sdlc_grp_poll, "virtual-multidrop",
         "Enable virtual multidrop for the interface", PRIV_CONF);

/***************************************************************
 * [no] sdlc simultaneous [full-datmode|half-datmode]
 *
 */

EOLS	(cfg_sdlc_tws_eol, sdlc_encaps, SDLC_CMD_SIMULTANEOUS);
KEYWORD_ID(cfg_tws_half, cfg_sdlc_tws_eol, no_alt,
	   OBJ(int,1), FALSE,
	   "half-datmode", "DATMODE = HALF ", PRIV_CONF);
KEYWORD_ID(cfg_tws_full, cfg_sdlc_tws_eol,
	   cfg_tws_half,
	   OBJ(int,1), TRUE,
	   "full-datmode", "DATMODE = FULL ", PRIV_CONF);
NOPREFIX(cfg_tws_noprefix, cfg_tws_full,
	 cfg_sdlc_tws_eol);
NVGENS	(cfg_sdlc_tws_nvgen, cfg_tws_noprefix,
	 sdlc_encaps, SDLC_CMD_SIMULTANEOUS);
KEYWORD	(cfg_sdlc_tws, cfg_sdlc_tws_nvgen , cfg_sdlc_virtual_multidrop,
	 "simultaneous",
	 "Activate SDLC two-way simultaneous mode",
	 PRIV_CONF);

/***************************************************************
 * sdlc	qllc-prtnr <virtual-macaddr> <sdlc-address>
 * no sdlc qllc-prtnr <virtual-macaddr> <sdlc-address>
 *
 */

EOLS    (cfg_sdlc_qllc_prtnr_eol, sdlc_encaps, SDLC_CMD_QLLC_PRTNR);
HEXDIGIT(cfg_sdlc_qllc_prtnr_sdlcaddr, cfg_sdlc_qllc_prtnr_eol, no_alt,
	 OBJ(int, 1), 1, 0xFE, "SDLC address in hex");
ENADDR  (cfg_sdlc_qllc_prtnr_enaddr, cfg_sdlc_qllc_prtnr_sdlcaddr, no_alt,
	 OBJ(hwaddr,1), "Virtual mac address mapped to X.121 address");
NVGENS  (cfg_sdlc_qllc_prtnr_nvgen, cfg_sdlc_qllc_prtnr_enaddr,
	 sdlc_encaps, SDLC_CMD_QLLC_PRTNR);
KEYWORD (cfg_sdlc_qllc_prtnr, cfg_sdlc_qllc_prtnr_nvgen, cfg_sdlc_tws,
	 "qllc-prtnr", "Partner for SDLC-QLLC conversion", PRIV_CONF);

/***************************************************************
 * sdlc	rts-timeout <unit>
 * no sdlc rts-timeout
 *
 */
PARAMS_KEYONLY(cfg_sdlc_rts_timeout, cfg_sdlc_qllc_prtnr,
	       "rts-timeout", OBJ(int,1), 1, 64000,
	       sdlc_encaps, SDLC_CMD_RTS_TIMEOUT,
               "Adjust the time a DTE interface waits for the DCE "
               "to assert CTS before dropping RTS",
	       "timeout unit in ucode execution cycles",
	       PRIV_CONF | PRIV_HIDDEN );

/***************************************************************
 * sdlc	cts-delay <unit>
 * no sdlc cts-delay
 *
 */
PARAMS_KEYONLY(cfg_sdlc_cts_delay, cfg_sdlc_rts_timeout,
	       "cts-delay", OBJ(int,1), 1, 64000,
	       sdlc_encaps, SDLC_CMD_CTS_DELAY,
               "Adjust the delay between the detection of RTS "
               "and assertion of CTS",
	       "delay unit in ucode execution cycles",
	       PRIV_CONF | PRIV_HIDDEN );

/***************************************************************
 * [no] sdlc hdx
 *
 */
EOLS	(cfg_sdlc_hdx_eol, sdlc_encaps, SDLC_CMD_HDX);
KEYWORD	(cfg_sdlc_hdx, cfg_sdlc_hdx_eol , cfg_sdlc_cts_delay,
	 "hdx", "Set interface for half duplex operation",
	 PRIV_CONF | PRIV_HIDDEN );

/***************************************************************
 * sdlc slow-poll <secs>
 * no sdlc slow-poll
 *
 */
EOLS    (cfg_sdlc_slow_eol, sdlc_encaps, SDLC_CMD_SLOW_POLL);
NUMBER  (cfg_sdlc_slow_seconds, cfg_sdlc_slow_eol, cfg_sdlc_slow_eol,
         OBJ(int,1), 1, 60, "slow poll interval");

NVGENS  (cfg_sdlc_slow_nvgen, cfg_sdlc_slow_seconds, sdlc_encaps,
              SDLC_CMD_SLOW_POLL);

/* Set OBJ(int,1) to 10 for the default interval. */
KEYWORD_ID (cfg_sdlc_slow, cfg_sdlc_slow_nvgen, cfg_sdlc_hdx,
	    OBJ(int,1), 10,
	    "slow-poll", "Enable sdlc slow poll", PRIV_CONF);

/***************************************************************
 * sdlc	line-speed <rate>
 * no sdlc line-speed
 *
 */
PARAMS_KEYONLY(cfg_sdlc_speed, cfg_sdlc_slow,
	       "line-speed", OBJ(int,1), 1200, 2000000,
	       sdlc_encaps, SDLC_CMD_LINE_SPEED,
	       "Required to adjust T1 timer for different line speeds",
	       "Clockrate of the interface in bits/sec",
	       PRIV_CONF);

/***************************************************************
 * sdlc	holdq <sdlcaddr> <value>
 * no sdlc holdq <sdlcaddr>
 *
 */
EOLS	(cfg_sdlc_holdq_eol, sdlc_encaps, SDLC_CMD_HOLDQ);
SET	(cfg_sdlc_holdq_set2, cfg_sdlc_holdq_eol, OBJ(int,3), 1);
NUMBER	(cfg_sdlc_holdq_num, cfg_sdlc_holdq_eol, cfg_sdlc_holdq_set2,
	 OBJ(int,2), 0, -1, "Queue size");
NOPREFIX(cfg_sdlc_holdq_noprefix, cfg_sdlc_holdq_num, cfg_sdlc_holdq_eol);
/****  Hexnum is getting a CLNS address ****/
HEXDIGIT(cfg_sdlc_holdq_addr, cfg_sdlc_holdq_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFE,
	 "SDLC address for which to specify a queue size");
NVGENS	(cfg_sdlc_holdq_nvgen, cfg_sdlc_holdq_addr,
	 sdlc_encaps, SDLC_CMD_HOLDQ);
KEYWORD	(cfg_sdlc_holdq, cfg_sdlc_holdq_nvgen , cfg_sdlc_speed,
	 "holdq", "Set size of hold queue", PRIV_CONF);

/***************************************************************
 * sdlc	address <sdlcaddr> [test-mode] [twoaddr-mode] [echo]  [ack-mode]
 *                         [xid-poll] [switched]
 * no sdlc address <sdlcaddr>
 *
 */
EOLS	(cfg_sdlc_addr_eol, sdlc_encaps, SDLC_CMD_ADDRESS);


pdecl( cfg_sdlc_addr_opts);
KEYWORD_ID(cfg_sdlc_addr_neg, cfg_sdlc_addr_opts, NONE,
           OBJ(int,8), TRUE,
           "xid-poll", "Initiate connection with XID poll to peer", 
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_addr_switched,cfg_sdlc_addr_opts,cfg_sdlc_addr_neg,
           OBJ(int,10), TRUE,
	   "switched", "Initiate connection with station before host",
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_addr_xid_passthru, cfg_sdlc_addr_opts, cfg_sdlc_addr_switched,
           OBJ(int,9), TRUE,
           "xid-passthru", "Allow XID passthru for this station", 
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_addr_ackmode, cfg_sdlc_addr_opts, cfg_sdlc_addr_xid_passthru,
           OBJ(int,7), TRUE,
           "ack-mode", "Allow local acknowledgement for address FF",
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_addr_echo, cfg_sdlc_addr_opts, cfg_sdlc_addr_ackmode,
	   OBJ(int,6), TRUE,
	   "echo", "Set the echo bit", PRIV_CONF);
KEYWORD_ID(cfg_sdlc_addr_twoaddr, cfg_sdlc_addr_opts, cfg_sdlc_addr_echo,
	   OBJ(int,5), TRUE,
	   "two-addr-mode", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(cfg_sdlc_addr_test, cfg_sdlc_addr_opts, cfg_sdlc_addr_twoaddr,
	   OBJ(int,4), TRUE,
	   "test-mode", "", PRIV_CONF | PRIV_HIDDEN);
NOP	(cfg_sdlc_addr_opts, cfg_sdlc_addr_test, cfg_sdlc_addr_eol);


NOPREFIX(cfg_sdlc_addr_noprefix, cfg_sdlc_addr_opts,
	 cfg_sdlc_addr_eol);

HEXDIGIT(cfg_sdlc_get_addr, cfg_sdlc_addr_noprefix, no_alt,
	 OBJ(int,1), 1, 0xFF,
	 "SDLC Address in Hex");

NVGENS	(cfg_sdlc_addr_nvgen, cfg_sdlc_get_addr,
	 sdlc_encaps, SDLC_CMD_ADDRESS);
KEYWORD	(cfg_sdlc_addr, cfg_sdlc_addr_nvgen, cfg_sdlc_holdq,
	 "address",
	 "Assign the set of secondary stations attached to interface",
	 PRIV_CONF);

/***************************************************************
 * [no] sdlc frmr-disable
 *
 */
EOLS	(cfg_sdlc_frmr_eol, sdlc_encaps, SDLC_CMD_FRMR_DISABLE);
KEYWORD	(cfg_sdlc_frmr, cfg_sdlc_frmr_eol , cfg_sdlc_addr,
	 "frmr-disable",
	 "Secondary devices off this link support Frame Rejects (FRMRs)",
	 PRIV_CONF);

/***************************************************************
 * sdlc	k <value>
 * no sdlc k
 *
 */
PARAMS_KEYONLY(cfg_sdlc_k, cfg_sdlc_frmr, "K",
	       OBJ(int,1), 1, SDLC_MODULO-1,
	       sdlc_encaps, SDLC_CMD_K,
	       "Size of local send window",
	       "Size of local send window",
	       PRIV_CONF);

/***************************************************************
 * sdlc	n2 <value>
 * no sdlc n2
 *
 */
PARAMS_KEYONLY(cfg_sdlc_n2, cfg_sdlc_k, "N2",
	       OBJ(int,1), 1, 255,
	       sdlc_encaps, SDLC_CMD_N2,
	       "Number of times to retry an operation that has timed out",
	       "Number of times to retry an operation that has timed out",
	       PRIV_CONF);

/***************************************************************
 * sdlc	n1 <value>
 * no sdlc n1
 *
 */
PARAMS_KEYONLY(cfg_sdlc_n1, cfg_sdlc_n2,
	       "N1", OBJ(int,1), 1, -1,
	       sdlc_encaps, SDLC_CMD_N1,
	       "Max number of bits for incoming frames",
	       "Max number of bits for incoming frames",
	       PRIV_CONF);

/***************************************************************
 * sdlc	poll-wait-timeout <timer>
 * no sdlc poll-wait-timeout
 *
 */
PARAMS_KEYONLY(cfg_sdlc_timeout, cfg_sdlc_n1,
	       "poll-wait-timeout", OBJ(int,1), 500, 64000,
	       sdlc_encaps, SDLC_CMD_POLL_WAIT,
	       "Amount of time router will wait between polls as"
	       " a secondary station before dropping the SDLC session",
	       "Seconds",
	       PRIV_CONF);

/***************************************************************
 * sdlc	t1 <value>
 * no sdlc t1
 *
 */
PARAMS_KEYONLY(cfg_sdlc_t1, cfg_sdlc_timeout,
	       "T1", OBJ(int,1), 1, 64000,
	       sdlc_encaps, SDLC_CMD_T1,
	       "Time the router waits for a reply to a frame",
	       "Time to wait for a reply to a frame in milliseconds",
	       PRIV_CONF);

/***************************************************************
 * sdlc	poll-limit-value <value>
 * no sdlc poll-limit-value
 *
 */
PARAMS_KEYONLY(cfg_sdlc_value, cfg_sdlc_t1,
	       "poll-limit-value", OBJ(int,1), 0, 10,
	       sdlc_encaps, SDLC_CMD_POLL_LIMIT,
	       "Number of times router can poll a secondary station"
	       " before polling the next station in the poll list",
	       "Number of times router can poll a secondary station"
	       " before polling the next station in the poll list",
	       PRIV_CONF);

/***************************************************************
 * sdlc	rnr-limit <value>
 * no sdlc rnr-limit
 *
 */
PARAMS_KEYONLY(cfg_sdlc_rnrlimit, cfg_sdlc_value,
	       "rnr-limit", OBJ(int,1), 0, 30,
	       sdlc_encaps, SDLC_CMD_RNR_LIMIT,
	       "Time (in minutes) the router allows its adjacent link"
	       "station to remain in a busy (RNR) state before "
               "declaring it inoperative",
	       "Time (in minutes) the router allows its adjacent link"
	       "station to remain in a busy (RNR) state before "
               "declaring it inoperative",
	       PRIV_CONF);

/***************************************************************
 * sdlc	poll-pause-timer <timer>
 * no sdlc poll-pause-timer
 *
 */
PARAMS_KEYONLY(cfg_sdlc_timer, cfg_sdlc_rnrlimit,
	       "poll-pause-timer", OBJ(int,1), 1, 10000,
	       sdlc_encaps, SDLC_CMD_POLL_PAUSE,
	       "Time between polls for each secondary SDLC station",
	       "Time between polls for each secondary SDLC station",
	       PRIV_CONF);
/***************************************************************
 * sdlc vmac <enaddr>
 * no sdlc vmac [<enaddr>]
 */
EOLS	(cfg_sdlc_vmac_eol, sdlc_encaps, SDLC_VMAC);
ENADDR	(cfg_sdlc_vmac_addr, cfg_sdlc_vmac_eol, no_alt,
	 OBJ(hwaddr,1), "Virtual IEEE MAC address");
NOPREFIX(cfg_sdlc_vmac_noprefix, cfg_sdlc_vmac_addr,
	 cfg_sdlc_vmac_eol);
NVGENS	(cfg_sdlc_vmac_nvgen, cfg_sdlc_vmac_noprefix,
	 sdlc_encaps, SDLC_VMAC);
KEYWORD	(cfg_sdlc_vmac, cfg_sdlc_vmac_nvgen, cfg_sdlc_timer,
	 "vmac",
	 "Assign MAC address to interface",
	 PRIV_CONF);

/***************************************************************
 * sdlc	role [primary | secondary | negotiable]
 * no sdlc role
 */
EOLS	(cfg_sdlc_role_eol, sdlc_encaps, SDLC_CMD_ROLE);
KEYWORD_ID(cfg_sdlc_role_neg, cfg_sdlc_role_eol , no_alt,
	   OBJ(int,1), SDLC_NONE,
	   "none", "Set role to none",
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_role_sec, cfg_sdlc_role_eol, cfg_sdlc_role_neg,
	   OBJ(int,1), SDLC_SECONDARY,
	   "secondary", "Set role to secondary",
	   PRIV_CONF);
KEYWORD_ID(cfg_prim_xid_poll, cfg_sdlc_role_eol, cfg_sdlc_role_sec,
	   OBJ(int,1), SDLC_PRIM_XID_POLL,
	   "prim-xid-poll", "Initiate connection with XID poll",
	   PRIV_CONF);
KEYWORD_ID(cfg_sdlc_role_prim, cfg_sdlc_role_eol, cfg_prim_xid_poll,
	   OBJ(int,1), SDLC_PRIMARY,
	   "primary", "Set role to primary",
	   PRIV_CONF);
NOPREFIX(cfg_sdlc_role_noprefix, cfg_sdlc_role_prim, cfg_sdlc_role_eol);
NVGENS	(cfg_sdlc_role_nvgen, cfg_sdlc_role_noprefix, sdlc_encaps,
	 SDLC_CMD_ROLE);
KEYWORD	(cfg_sdlc_role, cfg_sdlc_role_nvgen , cfg_sdlc_vmac,
	 "role", "Set role for this serial interface",
	 PRIV_CONF);

/***************************************************************/
KEYWORD	(cfg_sdlc, cfg_sdlc_role, NONE,
	 "sdlc", "SDLC commands", PRIV_CONF);

ASSERT	(cfg_sdlc_test, cfg_sdlc, ALTERNATE,
	 (is_sdlc(csb->interface->hwptr)));

#undef	ALTERNATE
#define	ALTERNATE	cfg_sdlc_test
