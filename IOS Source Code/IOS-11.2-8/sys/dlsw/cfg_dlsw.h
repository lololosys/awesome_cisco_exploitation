/* $Id: cfg_dlsw.h,v 3.9.6.12 1996/09/11 18:36:55 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/cfg_dlsw.h,v $
 *------------------------------------------------------------------
 * C F G _ D L S W . H
 *
 * DLSw global configuration commands.
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_dlsw.h,v $
 * Revision 3.9.6.12  1996/09/11  18:36:55  akhanna
 * CSCdi67926:  DLSW: ethernet direct encap causes crash
 * Branch: California_branch
 *
 * Revision 3.9.6.11  1996/09/11  05:00:24  snyder
 * CSCdi68758:  spellink errors
 *              adresses -> addresses
 * Branch: California_branch
 *
 * Revision 3.9.6.10  1996/09/09  19:08:23  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.9.6.9  1996/08/22  20:50:33  athippes
 * CSCdi66797:  Increase linger timeout from 300 mins
 * Branch: California_branch
 *
 * Revision 3.9.6.8  1996/08/21  22:18:32  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.9.6.7  1996/08/16  06:55:38  ioakley
 * CSCdi58903:  Add dlsw touch timer support for sna/netbios
 *              reachability cache entries.
 * Branch: California_branch
 *
 * Revision 3.9.6.6  1996/08/09  21:54:55  ravip
 * CSCdi59181:  Best path not chosen for the first circuit
 * Branch: California_branch
 *
 * Revision 3.9.6.5  1996/07/31  18:19:44  akhanna
 * CSCdi61887:  dlsw icanreach saps
 * Branch: California_branch
 *
 * Revision 3.9.6.4  1996/07/23  06:29:41  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.9.6.3  1996/06/17  22:38:22  widmer
 * CSCdi60344:  Parser does not recognize dlsw remote-peer subint
 * w/frame-relay
 * Branch: California_branch
 * Change Interface to Forward_Interface
 *
 * Revision 3.9.6.2  1996/05/30  23:43:18  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.9.6.1  1996/05/21  09:45:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9  1996/03/06  16:55:16  fbordona
 * CSCdi50687:  dlsw peer-on-demand-defaults does not allow the largest
 *              frame lf.
 *
 * Revision 3.8  1996/03/05  18:49:20  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.7  1996/02/27  21:36:17  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.6  1996/02/20  14:02:53  widmer
 * CSCdi49343:  dlsw config command should use keyword_options macro
 *
 * Revision 3.5  1996/01/18  02:12:32  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.4  1995/12/11  14:11:19  fbordona
 * CSCdi45182:  dlsw duplicate-path-bias shows up twice in config
 *
 * Revision 3.3  1995/11/20  22:44:54  snyder
 * CSCdi44202:  spellink errors
 *              frmae -> frame
 *              inactivty -> inactivity
 *
 * Revision 3.2  1995/11/17  09:02:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:22:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:54:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/09  20:18:35  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.3  1995/08/09  00:18:50  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/05  21:23:22  thille
 * CSCdi36635:  nv_add string should not contain newline
 * Spellink error noted while code reviewing...
 *
 * Revision 2.1  1995/07/04  01:52:50  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.4  1995/06/30  18:52:36  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.3  1995/06/28  19:00:38  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.2  1995/06/13  17:47:02  ravip
 * CSCdi35261:  DLSw should permit ARB from target
 *              Nerd knob to control ARB and SRB of SNA and Netbios frames
 *              Store test frames if search is pending for the dmac
 *
 * Revision 2.1  1995/06/07  22:02:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


static keyword_options peer_lf_number_options[] = {
    { "516", "maximum frame size in bytes", RC_LF516 },
    { "1470", NULL, RC_LF1470 },
    { "1500", NULL, RC_LF1500 },
    { "2052", NULL, RC_LF2052 },
    { "4472", NULL, RC_LF4472 },
    { "8144", NULL, RC_LF8144 },
    { "11407", NULL, RC_LF11407 },
    { "11454", NULL, RC_LF11454 },
    { "17800", "", RC_LF17800 },
    { NULL, NULL, 0 }
};

/**********************************************************************
 * [no] dlsw prom-peer-defaults
 *      [tcp-queue-max <size>] [cost <cost>]
 *      [keepalive <time>] [lsap-output-list <list>]
 *      [host-netbios-out <host-list-name>]
 *      [bytes-netbios-out <bytes-list-name>]
 *      [lf <size>] [dest-mac] [dmac-output-list]

 *
 * OBJ(int,1) = <size> of tcp-queue-max
 * OBJ(int,2) = remote peer's <cost>
 * OBJ(int,3) = TRUE if "keepalive"
 * OBJ(int,4) = <time> of "keepalive"
 * OBJ(int,5) = <list> number of "lsap-output-list" or 0
 * OBJ(int,6) = TRUE if "host-netbios-out"
 * OBJ(string,1) = <host-list-name> of "host-netbios-out"
 * OBJ(int,7) = TRUE if "bytes-netbios-out"
 * OBJ(string,2) = <bytes-list-name> of "bytes-netbios-out"
 * OBJ(int,8) = lf <size>
 * OBJ(int,9) = list <number> of "dmac-output-list"
 * OBJ(hwaddr,1) = <mac-addr> of "dest-mac"
 */

 EOLS    (prompeer_eol, dlsw_peer_command, PEER_PROM_DEFAULTS);
 pdecl(prompeer_opts);

/*
 * The below options loop back to prompeer_opts so that they can
 * be input in any order.  If an option is specified multiple times,
 * the last value is used.
 */

/* [dmac-output-list <list>] */
NUMBER(prompeer_dmac_outlist, prompeer_opts, no_alt,
        OBJ(int,9), MINMACADDRACC, MAXMACADDRACC, "access list number");
KEYWORD(prompeer_dmac_list, prompeer_dmac_outlist, no_alt,
         "dmac-output-list", "Filter output destination mac addresses",
         PRIV_CONF);

/* [dest-mac <mac-addr> ] */
ENADDR(prompeer_dmac_out_addr, prompeer_opts, no_alt,
         OBJ(hwaddr,1), "Destination MAC address");
KEYWORD(prompeer_dmac_out, prompeer_dmac_out_addr, prompeer_dmac_list,
           "dest-mac",
           "Exclusive destination mac-addr for promiscous peer",
           PRIV_CONF);

/* [lf <size>] */
KEYWORD_ID(prompeer_lf_17800, prompeer_opts, no_alt,
           OBJ(int,8), RC_LF17800,
           "17800", "17800 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_11454, prompeer_opts, prompeer_lf_17800,
           OBJ(int,8), RC_LF11454,
           "11454", "11454 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_11407, prompeer_opts, prompeer_lf_11454,
           OBJ(int,8), RC_LF11407,
           "11407", "11407 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_8144, prompeer_opts, prompeer_lf_11407,
           OBJ(int,8), RC_LF8144,
           "8144", "8144 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_4472, prompeer_opts, prompeer_lf_8144,
           OBJ(int,8), RC_LF4472,
           "4472", "4472 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_2052, prompeer_opts, prompeer_lf_4472,
           OBJ(int,8), RC_LF2052,
           "2052", "2052 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_1500, prompeer_opts, prompeer_lf_2052,
           OBJ(int,8), RC_LF1500,
           "1500", "1500 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_1470, prompeer_opts, prompeer_lf_1500,
           OBJ(int,8), RC_LF1470,
           "1470", "1470 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(prompeer_lf_516, prompeer_opts, prompeer_lf_1470,
           OBJ(int,8), RC_LF516,
           "516", "516 byte maximum frame size", PRIV_CONF);
KEYWORD (prompeer_lf, prompeer_lf_516, prompeer_dmac_out, "lf",
         "Configure largest frame size for prom peers", PRIV_CONF);

/* [bytes-netbios-out <bytes-list-name>] */
STRING  (prompeer_net_bname, prompeer_opts, no_alt, OBJ(string,2),
         "Name of previously defined netbios bytes access list filter");
KEYWORD_ID(prompeer_net_bytes, prompeer_net_bname, prompeer_lf,
         OBJ(int,7), TRUE, "bytes-netbios-out",
         "Configure netbios bytes output filtering for prom peers",
         PRIV_CONF);

/* [host-netbios-out <host-list-name>] */
STRING  (prompeer_net_hname, prompeer_opts, no_alt, OBJ(string,1),
         "Name of previously defined netbios host access list filter");
KEYWORD_ID(prompeer_net_host, prompeer_net_hname, prompeer_net_bytes,
         OBJ(int,6), TRUE, "host-netbios-out",
         "Configure netbios host output filtering for prom peers",
         PRIV_CONF);

/* [lsap-output-list <list>] */
NUMBER(prompeer_lsap_list, prompeer_opts, no_alt,
        OBJ(int, 5), MINTYPEACC, MAXTYPEACC, "access list number");
KEYWORD (prompeer_lsap, prompeer_lsap_list, prompeer_net_host, "lsap-output-list",
         "Configure LSAP output filtering for prom peers", PRIV_CONF);

/* [keepalive <time>] */
NUMBER  (prompeer_keep_time, prompeer_opts, no_alt,
         OBJ(int,4), 0, 1200, "prom peer keepalive time");
KEYWORD_ID(prompeer_keep_int, prompeer_keep_time, prompeer_lsap,
         OBJ(int,3), TRUE, "keepalive",
         "Configure prom peer  keepalive interval (default is 30 secs)",
         PRIV_CONF);

/* [cost <cost>] */
NUMBER  (prompeer_cost_num, prompeer_opts, no_alt,
         OBJ(int,2), 1, 5, "prom peer cost number");
KEYWORD (prompeer_cost, prompeer_cost_num, prompeer_keep_int, "cost",
         "Configure cost to reach prom peers (default is 3)", PRIV_CONF);

/* [tcp-queue-max <size>] */
NUMBER  (prompeer_tcpq_num, prompeer_opts, no_alt, OBJ(int,1), 25, 2000,
         "Maximum TCP Queue Size in Packets");
KEYWORD (prompeer_tcpq, prompeer_tcpq_num, prompeer_cost, "tcp-queue-max",
         "Maximum output TCP queue size for prom peers", PRIV_CONF);

NOP   (prompeer_opts, prompeer_tcpq, prompeer_eol);

/* must pick at least one option for the command to be valid */
NOP     (prompeer_first_opts, prompeer_tcpq, no_alt);

SET     (prompeer_def_dmac_list, prompeer_first_opts, OBJ(int,9), 0);
SET     (prompeer_def_lf, prompeer_def_dmac_list, OBJ(int,8), RC_LFINITIAL);
SET     (prompeer_def_lsap, prompeer_def_lf, OBJ(int,5), 0);
SET     (prompeer_def_cost, prompeer_def_lsap, OBJ(int,2), DLSW_NORM_PCOST);
SET     (prompeer_def_tcpq, prompeer_def_cost, OBJ(int,1), DLSW_MAX_TCP_QLEN);
NOPREFIX(prompeer_noprefix, prompeer_def_tcpq, prompeer_eol);

NVGENS  (prompeer_nvgen, prompeer_noprefix, dlsw_peer_command, PEER_PROM_DEFAULTS);

KEYWORD (prompeer_defaults, prompeer_nvgen, no_alt, "prom-peer-defaults",
         "Change prom-peer-defaults", PRIV_CONF);

/*
 *******************************************************************
 * [no] dlsw allroute-netbios
 *******************************************************************
 */
EOLS    (arb_netbios_eol, dlsw_csm_command, CSM_ALLROUTE_NETBIOS);
KEYWORD (dlsw_arb_netbios, arb_netbios_eol, prompeer_defaults, "allroute-netbios",
         "Use All routes Broadcast for NETBIOS Explorers",
         PRIV_CONF|PRIV_HIDDEN);

/*
 *******************************************************************
 * [no] dlsw allroute-sna
 *******************************************************************
 */
EOLS    (arb_sna_eol, dlsw_csm_command, CSM_ALLROUTE_SNA);
KEYWORD (dlsw_arb_sna, arb_sna_eol, dlsw_arb_netbios, "allroute-sna",
         "Use All routes Broadcast for SNA Explorers", PRIV_CONF|PRIV_HIDDEN);


/*********************************************************************
 * [no] dlsw bridge-group (bridge_group)
 */

EOLS (dlsw_bgrp_eol, dlsw_peer_command, PEER_BGROUP_ASSIGN);
pdecl(dlsw_bgrp_opts);
pdecl(dlsw_bgrp_llc2_opts);

NUMBER     (bgrp_llc2_txq_max, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,15), 20, 200, "Depth of queue in I-frames");
KEYWORD_ID (bgrp_llc2_txq_max_def, bgrp_llc2_txq_max, no_alt,
            OBJ(int,15), 0, "txq-max",
            "Queue for holding llc2 information frames", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_xid_rtry, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,14), 0, 60000, 
            "Wait time for reply to XID in milliseconds");
KEYWORD_ID (bgrp_llc2_xid_rtry_def, bgrp_llc2_xid_rtry, bgrp_llc2_txq_max_def,
            OBJ(int,14), 0, "xid-retry-time",
            "How long router waits for reply to XID", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_xid_neg, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,13), 1, 60000, "Frequency of XID in milliseconds");
KEYWORD_ID (bgrp_llc2_xid_neg_def, bgrp_llc2_xid_neg, bgrp_llc2_xid_rtry_def,
            OBJ(int,13), 0,"xid-neg-val-time",
            "Frequency of exchange of identification (XID)", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_tpf, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,12), 1, 60000, "Tpf time in milliseconds");
KEYWORD_ID (bgrp_llc2_tpf_def, bgrp_llc2_tpf, bgrp_llc2_xid_neg_def,
            OBJ(int,12), 0,"tpf-time",
            "Amount of time router waits for a final response to a poll "
            "frame before re-sending the original poll frame", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_trej, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,11), 1, 60000, "Trej time in milliseconds");
KEYWORD_ID (bgrp_llc2_trej_def, bgrp_llc2_trej, bgrp_llc2_tpf_def,
            OBJ(int,11), 0,"trej-time",
            "Amount of time router waits for a resend of a rejected frame "
            " before sending the reject command", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_tbusy, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,10), 1, 60000, "Tbusy time in milliseconds");
KEYWORD_ID (bgrp_llc2_tbusy_def, bgrp_llc2_tbusy, bgrp_llc2_trej_def,
            OBJ(int,10), 0,"tbusy-time",
            "Amount of time router waits while the other LLC2 station "
            "is in a busy state before attempting to poll the remote station",
            PRIV_CONF);
NUMBER     (bgrp_llc2_t1, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,9), 1, 60000, "T1 time in milliseconds");
KEYWORD_ID (bgrp_llc2_t1_def, bgrp_llc2_t1, bgrp_llc2_tbusy_def,
            OBJ(int,9), 0,"t1-time",
            "How long router waits for an acknowledgment to transmitted "
            "I-frames", 
            PRIV_CONF);
NUMBER     (bgrp_llc2_n2, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,8), 1, 255, 
            "Number of times router should retry various operations");
KEYWORD_ID (bgrp_llc2_n2_def, bgrp_llc2_n2, bgrp_llc2_t1_def,
            OBJ(int,8), 0, "N2",
            "Number of times router should retry various operations",
            PRIV_CONF);
NUMBER     (bgrp_llc2_local, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,7), 1, 127, 
            "Max number of I-frames to send before waiting for an "
            "acknowledgment");
KEYWORD_ID (bgrp_llc2_local_def, bgrp_llc2_local, bgrp_llc2_n2_def,
            OBJ(int,7), 0, "local-window",
            "Max number of I-frames to send before waiting for an "
            "acknowledgment",
            PRIV_CONF);
NUMBER     (bgrp_llc2_idle, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,6), 1, 60000, "Frequency of polls in milliseconds");
KEYWORD_ID (bgrp_llc2_idle_def, bgrp_llc2_idle, bgrp_llc2_local_def,
            OBJ(int,6), 0, "idle-time",
            "Frequency of polls during periods of idle traffic",
            PRIV_CONF);
NUMBER     (bgrp_llc2_ackdelay, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,5), 1, 60000, "Ack delay time in milliseconds");
KEYWORD_ID (bgrp_llc2_ackdelay_def, bgrp_llc2_ackdelay, bgrp_llc2_idle_def,
            OBJ(int,5), 0, "ack-delay-time",
            "Max time the router allows incoming I-frames to stay "
            "unacknowledged",
            PRIV_CONF);
NUMBER     (bgrp_llc2_akmax, dlsw_bgrp_llc2_opts, no_alt,
            OBJ(int,4), 1, 255, "Ack max");
KEYWORD_ID (bgrp_llc2_akmax_def, bgrp_llc2_akmax, bgrp_llc2_ackdelay_def,
            OBJ(int,4), 0, "ack-max",
            "Max number of I-frames received before an acknowledgment "
            "must be sent",
            PRIV_CONF);
NOP     (dlsw_bgrp_llc2_opts, bgrp_llc2_akmax_def, dlsw_bgrp_eol);

KEYWORD (dlsw_bgrp_llc2, dlsw_bgrp_llc2_opts, no_alt, "llc2", 
         "LLC2 Interface Subcommands", PRIV_CONF); 
NUMBER (dlsw_locaddr_pri, dlsw_bgrp_opts, no_alt,
        OBJ(int,3), 1, 10, "LU Addr priority list number");
KEYWORD_ID (dlsw_locaddr_pri_def, dlsw_locaddr_pri, dlsw_bgrp_llc2,
            OBJ(int,3), 0, "locaddr-priority",
            "Assign an input SNA LU Addr priority list to this bridge group",
            PRIV_CONF);
NUMBER (dlsw_sap_pri, dlsw_bgrp_opts, no_alt,
        OBJ(int,2), 1, 10, "Priority list number");
KEYWORD_ID (dlsw_sap_pri_def, dlsw_sap_pri, dlsw_locaddr_pri_def,
            OBJ(int,2), 0, "sap-priority",
            "Assign an input sap priority list to this bridge group",
            PRIV_CONF);
NOP    (dlsw_bgrp_opts, dlsw_sap_pri_def,dlsw_bgrp_eol);
NUMBER (dlsw_bgrp_num, dlsw_bgrp_opts, no_alt,
        OBJ(int,1), 1, DLSW_SPAN_MAXINDEX - 1,
        "Assign a Bridge Group to be assigned to DLSw");
NVGENS  (dlsw_bgrp_num_nvgen, dlsw_bgrp_num, dlsw_peer_command,
         PEER_BGROUP_ASSIGN);
KEYWORD (dlsw_bgroup, dlsw_bgrp_num_nvgen, dlsw_arb_sna, "bridge-group",
               "DLSw interconnection to transparent bridging",PRIV_CONF);


/**********************************************************************
 * [no] dlsw force-ssp
 */
EOLS    (force_ssp_eol, dlsw_peer_command, PEER_FORCE_SSP);
KEYWORD (force_ssp, force_ssp_eol, dlsw_bgroup, "force-ssp",
         "Use SSP protocol between cisco peers", PRIV_CONF|PRIV_HIDDEN);

/**********************************************************************
 * [no] dlsw peer-on-demand-defaults 
 *      [tcp | fst] [local-ack] [priority] [cost <cost>] 
 *      [keepalive <time>] [lsap-output-list <list>]
 *      [host-netbios-out <host-list-name>]
 *      [bytes-netbios-out <bytes-list-name>]
 *      [inactivity <timeout>] [lf <size>]
 *	[dest-mac] [dmac-output-list]
 *
 * OBJ(int,1) = <port-list>
 * OBJ(int,2) = DLSW_TCP | DLSW_FST
 * OBJ(int,3) = <size> of tcp-queue-max
 * OBJ(int,4) = TRUE if "priority"
 * OBJ(int,5) = remote peer's <cost>
 * OBJ(int,6) = TRUE if "keepalive"
 * OBJ(int,7) = <time> of "keepalive"
 * OBJ(int,8) = <list> number of "lsap-output-list" or 0
 * OBJ(int,9) = TRUE if "host-netbios-out"
 * OBJ(string,1) = <host-list-name> of "host-netbios-out"
 * OBJ(int,10) = TRUE if "bytes-netbios-out"
 * OBJ(string,2) = <bytes-list-name> of "bytes-netbios-out"
 * OBJ(int,11) = inactivity <timeout> in minutes
 * OBJ(int,12) = lf <size>
 * OBJ(int,13) = list <number> of "dmac-output-list"
 * OBJ(hwaddr,1) = <mac-addr> of "dest-mac"
 */

EOLS    (pod_eol, dlsw_peer_command, PEER_POD_DEFAULTS);
pdecl(pod_opts);

/*
 * The below options loop back to rpeer_opts so that they can
 * be input in any order.  If an option is specified multiple times,
 * the last value is used.
 */

/* [dmac-output-list <list>] */
NUMBER(pod_dmac_outlist, pod_opts, no_alt,
        OBJ(int,13), MINMACADDRACC, MAXMACADDRACC, "access list number");
KEYWORD(pod_dmac_list, pod_dmac_outlist, no_alt,
         "dmac-output-list", "Filter output destination mac addresses",
         PRIV_CONF);

/* [dest-mac <mac-addr> ] */
ENADDR(pod_dmac_out_addr, pod_opts, no_alt,
         OBJ(hwaddr,1), "Destination MAC address");
KEYWORD(pod_dmac_out, pod_dmac_out_addr, pod_dmac_list,
           "dest-mac",
           "Exclusive destination mac-addr for pod peer",
           PRIV_CONF);

/* [lf <size>] */
KEYWORD_OPTIONS(pod_lf_number, pod_opts, no_alt,
                peer_lf_number_options, OBJ(int,12), PRIV_CONF, 0);
KEYWORD (pod_lf, pod_lf_number, pod_dmac_out, "lf",
         "Configure largest frame size for p-o-d peers", PRIV_CONF);

/* [inactivity <timeout>] */
NUMBER  (pod_inact_min, pod_opts, no_alt,
         OBJ(int,11), 0, DLSW_POD_MAX_DISC_DELAY,
         "inactivity minutes after which peer is disconnected");
KEYWORD (pod_inact, pod_inact_min, pod_lf, "inactivity",
         "Configure inactivity interval to disconnect p-o-d peers",
         PRIV_CONF);

/* [bytes-netbios-out <bytes-list-name>] */
STRING  (pod_net_bname, pod_opts, no_alt, OBJ(string,2), 
         "Name of previously defined netbios bytes access list filter");
KEYWORD_ID(pod_net_bytes, pod_net_bname, pod_inact,
         OBJ(int,10), TRUE, "bytes-netbios-out", 
         "Configure netbios bytes output filtering for p-o-d peers",
         PRIV_CONF);

/* [host-netbios-out <host-list-name>] */
STRING  (pod_net_hname, pod_opts, no_alt, OBJ(string,1), 
         "Name of previously defined netbios host access list filter");
KEYWORD_ID(pod_net_host, pod_net_hname, pod_net_bytes,
         OBJ(int,9), TRUE, "host-netbios-out", 
         "Configure netbios host output filtering for p-o-d peers", 
         PRIV_CONF);

/* [lsap-output-list <list>] */
ACCESS_LIST(pod_lsap_list, pod_opts, no_alt,
        OBJ(int, 8), MINTYPEACC, MAXTYPEACC,
	TYPECODE_MIN_ACL, TYPECODE_MAX_ACL, "access list number");
KEYWORD (pod_lsap, pod_lsap_list, pod_net_host, "lsap-output-list", 
	 "Configure LSAP output filtering for p-o-d peers", PRIV_CONF);

/* [keepalive <time>] */
NUMBER  (pod_keep_time, pod_opts, no_alt,
         OBJ(int,7), 0, 1200, "p-o-d keepalive time");
KEYWORD_ID(pod_keep_int, pod_keep_time, pod_lsap, 
         OBJ(int,6), TRUE, "keepalive", 
	 "Configure p-o-d keepalive interval (default is 30 secs)", 
	 PRIV_CONF);

/* <port-list> */
NUMBER  (pod_rlist_num, pod_opts, no_alt,
         OBJ(int,1), 0, SRB_MAX_RING_NUM, "Remote peer port list number");
KEYWORD (pod_rlist, pod_rlist_num, pod_keep_int, "port-list",
         "Configure a port-list for p-o-d peers", PRIV_CONF);

/* [cost <cost>] */
NUMBER  (pod_cost_num, pod_opts, no_alt,
         OBJ(int,5), 1, 5, "p-o-d cost number");
KEYWORD (pod_cost, pod_cost_num, pod_rlist, "cost", 
	 "Configure cost to reach p-o-d peers (default is 3)", PRIV_CONF);

/* [tcp-queue-max <size>] */
NUMBER  (pod_tcpq_num, pod_opts, no_alt, OBJ(int,3), 25, 2000,
         "Maximum TCP Queue Size in Packets");
KEYWORD (pod_tcpq, pod_tcpq_num, pod_cost, "tcp-queue-max",
         "Maximum output TCP queue size for p-o-d peers", PRIV_CONF);

/* [fst] */
KEYWORD_ID(pod_fst, pod_opts, pod_tcpq, OBJ(int,2), DLSW_FST, "fst", 
           "Configure FST for p-o-d transport (default is TCP)", PRIV_CONF);

/* [priority] */
KEYWORD_ID(pod_pri, pod_opts, pod_fst, OBJ(int,4), TRUE, "priority",
           "Configure prioritization for p-o-d peers (default is off)", 
           PRIV_CONF);

/* If fst configured, can not specify priority or tcp-queue-max */
SPLIT   (pod_ck_fst, pod_cost, pod_pri, OBJ(int,2), DLSW_FST);

/* If priority configured, can not specify FST as transport */
SPLIT   (pod_ck_pri, pod_tcpq, pod_ck_fst, OBJ(int,4), TRUE);
NOP     (pod_opts, pod_ck_pri, pod_eol);

/* must pick at least one option for the command to be valid */
NOP     (pod_first_opts, pod_pri, no_alt);

SET     (pod_def_dmac_list, pod_first_opts, OBJ(int,13), 0);
SET     (pod_def_lf, pod_def_dmac_list, OBJ(int,12), RC_LFINITIAL);
SET     (pod_def_inact, pod_def_lf, OBJ(int,11), DLSW_POD_DEF_DISC_DELAY);
SET     (pod_def_lsap, pod_def_inact, OBJ(int,8), 0);
SET     (pod_def_cost, pod_def_lsap, OBJ(int,5), DLSW_NORM_PCOST);
SET     (pod_def_pri, pod_def_cost, OBJ(int,4), FALSE);
SET     (pod_def_tcpq, pod_def_pri, OBJ(int,3), DLSW_MAX_TCP_QLEN);
SET     (pod_def_tcp, pod_def_tcpq, OBJ(int,2), DLSW_TCP);
SET     (pod_def_rlist, pod_def_tcp, OBJ(int,1), 0);
NOPREFIX(pod_noprefix, pod_def_rlist, pod_eol);

NVGENS  (pod_nvgen, pod_noprefix, dlsw_peer_command, PEER_POD_DEFAULTS);

KEYWORD (pod_defaults, pod_nvgen, force_ssp, "peer-on-demand-defaults", 
	 "Change peer-on-demand defaults", PRIV_CONF);

/**********************************************************************
 * [no] dlsw { mac-addr <macaddr> |  netbios <netbios-name> }
 *             options->  { rif <rif-string> |
 *                          ring <ring> |
 *                          remote-peer {ip <ip-addr> | interface serial # } }
 *
 * (*OBJ(hwaddr,1)) = <mac-addr>
 * OBJ(string,1) = <netbios-name>
 * OBJ(int,1) = CSM_MAC_ADDR | CSM_NETBIOS_NAME
 * OBJ(int,2) = mapping type CSM_RES2{RIF, RING, IPADDR, INTERFACE, PGROUP}
 *
 * If mapped to rif        :  OBJ(int,4) : number of rif numbers
 *                         :  uiarray    : rif array
 * If mapped to ring       :  OBJ(int,3) : <ring>
 * If mapped to remote-peer:
 *    If specifying remote peer by ipadddr   : OBJ(paddr,1) : <ipaddr>
 *    If specifying remote peer by interface : interface <name> <mac-addr>
 * If mapped to group : OBJ(int,3) : <group>
 */
 
pdecl(csm_rif_1);
 
EOLS    (csm_static_nb_eol, dlsw_csm_command, CSM_NETBIOS_NAME);
EOLS    (csm_static_mac_eol, dlsw_csm_command, CSM_MAC_ADDR);
SPLIT   (csm_static_fork, csm_static_mac_eol, csm_static_nb_eol,
         OBJ(int,1), CSM_MAC_ADDR);
 
/* [interface <name> <mac-addr>] */
ENADDR  (csm_int_addr, csm_static_fork, no_alt,
        OBJ(hwaddr,1), "MAC Address");
TESTVAR (csm_int_test, csm_static_fork, csm_int_addr,
        NONE, NONE, NONE, OBJ(int,15), FALSE);
EVAL    (csm_int_eval, csm_int_test,
        (GETOBJ(int,15) = GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK));
INTERFACE(csm_int, csm_int_eval, no_alt, OBJ(idb,1), IFTYPE_SERIAL);
KEYWORD_ID(csm_peer_int, csm_int, no_alt,
        OBJ(int,2), CSM_RES2INTERFACE, common_str_interface,
        "Specify the remote peer by direct interface", PRIV_CONF);
 
/* ip-address <ipaddr> */
IPADDR  (csm_peer_ipaddr, csm_static_fork, no_alt,
        OBJ(paddr,1), "Remote peer's IP address");
KEYWORD_ID(csm_peer_ip, csm_peer_ipaddr, csm_peer_int,
        OBJ(int,2), CSM_RES2IPADDR, "ip-address",
        "Specify the remote peer by ip address", PRIV_CONF);
KEYWORD (csm_peer, csm_peer_ip, no_alt, "remote-peer",
        "Map the MAC address or NetBios name to a specific remote peer", PRIV_CONF);
 
/* ring <ring> */
NUMBER  (csm_ring_num, csm_static_fork, no_alt, OBJ(int,3), 0,
        SRB_MAX_RING_NUM, "Ring number");
KEYWORD_ID(csm_ring, csm_ring_num, csm_peer,
        OBJ(int,2), CSM_RES2RING, "ring",
        "Map the MAC address or NetBios name to ring number", PRIV_CONF);
 
 
/* <rif-string> - this loops to gather all the RIF numbers.  */
WHITESPACE(csm_rif_3b, csm_static_fork, csm_rif_1);
HELP    (csm_rif_3a, csm_rif_1, "H.H...\t");
CHARACTER(csm_rif_3, csm_rif_3a, csm_rif_3b, '.');
/* Make sure that the user doesn't attempt to give more than we can handle */
PRINT   (csm_rif_2b, no_alt, "RIF string too long");
TESTVAR (csm_rif_2a, csm_rif_2b, csm_rif_3,
         NONE, NONE, NONE, OBJ(int,4), SRB_MAXRIF/2);
EVAL    (csm_rif_2, csm_rif_2a,
         csb->uiarray[GETOBJ(int,4) ++] = GETOBJ(int,5));
HELP    (csm_rif_1a, csm_rif_2, "H.H...\t");
HEXADECIMAL(csm_rif_1, csm_rif_1a, no_alt, OBJ(int,5));
HEXADECIMAL(csm_rif_0, csm_rif_1a, no_alt, OBJ(int,5));
HELP    (csm_rif_h, csm_rif_0, "  H.H...\tRIF entry\n");
KEYWORD_ID(csm_rif, csm_rif_h, csm_ring,
        OBJ(int,2), CSM_RES2RIF, "rif",
        "Map the MAC address or NetBios name to a rif", PRIV_CONF);
 
/* netbios <netbios-name> */
STRING  (csm_nb_string, csm_rif, no_alt, OBJ(string,1),
        "Netbios name");
NVGENS  (csm_nb_nvgen, csm_nb_string, dlsw_csm_command, CSM_NETBIOS_NAME);
KEYWORD_ID(csm_static_nb, csm_nb_nvgen, pod_defaults,
        OBJ(int,1), CSM_NETBIOS_NAME, "netbios-name",
        "Configure a static NetBios name - location or path", PRIV_CONF);
 
/* mac-addr <macaddr> */
ENADDR  (csm_mac_addr, csm_rif, no_alt, OBJ(hwaddr,1), "MAC Address");
NVGENS  (csm_mac_nvgen, csm_mac_addr, dlsw_csm_command, CSM_MAC_ADDR);
KEYWORD_ID(csm_static_mac, csm_mac_nvgen, csm_static_nb,
        OBJ(int,1), CSM_MAC_ADDR, "mac-addr",
        "Configure a static MAC address - location or path", PRIV_CONF);


/**********************************************************************
 * [no] dlsw touch-timer { cache-entry-timeout <cache-timeout-active>
 *                                              <cache-timeout-reset> }
 *
 * Note: When configured, this command will supersede the operative values
 *       of the "dlsw timer (sna-cache-timeout) and/or (netbios-cache-timeout)
 *       after the cache entry timer is "touched" for the first time.  From
 *       that point onwards the operative value of the "dlsw timer (sna-cache-
 *       timeout) and/or (netbios-cache-timeout) will be the <cache-timeout-
 *       reset> value specified as the second parameter to this configuration
 *       command.  The <cache-timeout-activity-interval> specified as the
 *       first parameter to this configuration command will permit the user
 *       to control the frequency at which the cache entry timeout is "touched"
 *       or reset to the value of the <cache-timeout-reset-interval>.
 *
 * OBJ(int,1) = touch-timer type constant
 * OBJ(int,2) = <cache-timeout-activity-interval>
 * OBJ(int,3) = <cache-timeout-reset-interval>
 */
EOLS    (csm_touch_timer_eol, dlsw_csm_command, CSM_TOUCH_TIMERS);

NUMBER  (csm_touch_time1_sec, csm_touch_timer_eol, no_alt, OBJ(int,3),
         1, 86400,  "Timer Timer Reset Value (in seconds)");
NUMBER  (csm_touch_time1_min, csm_touch_timer_eol, no_alt, OBJ(int,3),
         1, 1440,   "Timer Value (in minutes)");

NUMBER  (csm_touch_time_sec, csm_touch_time1_sec, no_alt, OBJ(int,2),
	 1, 86400,  "Touch Timer Activity Value (in seconds)");
NUMBER  (csm_touch_time_min, csm_touch_time1_min, no_alt, OBJ(int,2),
         1, 1440,   "Touch Timer Activity Value (in minutes)");

NOPREFIX(csm_touch_sec_noprefix, csm_touch_time_sec, csm_touch_timer_eol);
NOPREFIX(csm_touch_min_noprefix, csm_touch_time_min, csm_touch_timer_eol);

KEYWORD_ID(csm_touch_sna_cache_t, csm_touch_sec_noprefix, no_alt,
	   OBJ(int,1), CSM_TOUCH_SNA_CACHE_T, "touch-sna-cache-timeout",
	   "Action/Reset timeout intervals for SNA cache entries",
	   PRIV_CONF);

KEYWORD_ID(csm_touch_nb_cache_t, csm_touch_sec_noprefix, csm_touch_sna_cache_t,
	   OBJ(int,1), CSM_TOUCH_NB_CACHE_T, "touch-nb-cache-timeout",
	   "Action/Reset timeout intervals for NetBIOS name cache entries",
	   PRIV_CONF);

NVGENS  (csm_touch_timer_nvgen, csm_touch_nb_cache_t, dlsw_csm_command,
         CSM_TOUCH_TIMERS);
KEYWORD (csm_touch_timer, csm_touch_timer_nvgen, csm_static_mac,
         "touch-timer", "Configure DLSw touch timers", PRIV_CONF);

/**********************************************************************
 * [no] dlsw timer { sna-explorer-timeout <time> |
 *                   sna-cache-timeout <time> |
 *                   sna-verify-interval <time> |
 *                   sna-retry-interval <time> |
 *                   netbios-explorer-timeout <time> |
 *                   netbios-cache-timeout <time> |
 *                   netbios-verify-interval <time> |
 *                   netbios-retry-interval <time> |
 *                   icannotreach-block-time <time> |
 *                   explorer-wait-time <time> }
 *
 * OBJ(int,1) = timer type constant
 * OBJ(int,2) = <time>
 */

EOLS    (csm_timer_eol, dlsw_csm_command, CSM_TIMERS);

NUMBER  (csm_time_sec, csm_timer_eol, no_alt, OBJ(int,2),  1, 86400, 
         "Timer Value (in seconds)");
NUMBER  (csm_time_min, csm_timer_eol, no_alt, OBJ(int,2),  1, 5, 
         "Timer Value (in minutes)");

NOPREFIX(csm_sec_noprefix, csm_time_sec, csm_timer_eol);
NOPREFIX(csm_min_noprefix, csm_time_min, csm_timer_eol);

KEYWORD_ID(csm_expl_delay_t, csm_min_noprefix, no_alt,
           OBJ(int,1), CSM_EXPL_DELAY_T, "explorer-delay-time",
           "Amount of time to wait before sending or accepting explorers",
           PRIV_CONF);
KEYWORD_ID(csm_expl_wait_t, csm_sec_noprefix, csm_expl_delay_t,
           OBJ(int,1), CSM_EXPL_WAIT_T, "explorer-wait-time",
           "Amount of time to wait for all stations to respond",
           PRIV_CONF);
KEYWORD_ID(csm_neg_cache_t, csm_sec_noprefix, csm_expl_wait_t,
           OBJ(int,1), CSM_NEG_CACHE_T, "icannotreach-block-time",
           "Search interval when a resource is known to be unreachable",
           PRIV_CONF);
KEYWORD_ID(csm_nbretry_t, csm_sec_noprefix, csm_neg_cache_t,
           OBJ(int,1), CSM_NB_RETRY_T, "netbios-retry-interval",
           "NetBios explorer (Name Query) retry interval (lan and wan)",
           PRIV_CONF);
KEYWORD_ID(csm_nbverify_t, csm_sec_noprefix, csm_nbretry_t,
           OBJ(int,1), CSM_NB_VERIFY_T, "netbios-verify-interval",
           "NetBios verify interval for name location (lan and wan)",
           PRIV_CONF);
KEYWORD_ID(csm_nbcache_t, csm_sec_noprefix, csm_nbverify_t,
           OBJ(int,1), CSM_NB_CACHE_T, "netbios-cache-timeout",
           "NetBios Name location (lan and wan) cache life)",
           PRIV_CONF);
KEYWORD_ID(csm_nbexpl_t, csm_sec_noprefix, csm_nbcache_t,
           OBJ(int,1), CSM_NB_EXPL_T, "netbios-explorer-timeout",
           "NetBios explorer response wait time (lan and wan)",
           PRIV_CONF);
KEYWORD_ID(csm_snaretry_t, csm_sec_noprefix, csm_nbexpl_t,
           OBJ(int,1), CSM_SNA_RETRY_T, "sna-retry-interval",
           "SNA explorer retry interval (lan and wan)",
           PRIV_CONF);
KEYWORD_ID(csm_snaverify_t, csm_sec_noprefix, csm_snaretry_t,
           OBJ(int,1), CSM_SNA_VERIFY_T, "sna-verify-interval",
           "SNA verify interval for MAC/SAP location (lan and wan)",
           PRIV_CONF);
KEYWORD_ID(csm_snacache_t, csm_sec_noprefix, csm_snaverify_t,
           OBJ(int,1), CSM_SNA_CACHE_T, "sna-cache-timeout",
           "SNA MAC/SAP location (lan and wan) cache life)",
           PRIV_CONF);
KEYWORD_ID(csm_snaexpl_t, csm_sec_noprefix, csm_snacache_t,
           OBJ(int,1), CSM_SNA_EXPL_T, "sna-explorer-timeout",
           "SNA explorer response wait time (lan and wan)",
           PRIV_CONF);

NVGENS  (csm_timer_nvgen, csm_snaexpl_t, dlsw_csm_command, CSM_TIMERS);
KEYWORD (csm_timer, csm_timer_nvgen, csm_touch_timer, "timer",
         "Configure DLSw timers", PRIV_CONF);


/**********************************************************************
 * [no] dlsw explorer-queue-depth <packets>
 *
 * OBJ(int,1) = <packets>
 */
PARAMS_KEYONLY(csm_explq, csm_timer, "explorerQ-depth",
	       OBJ(int,1), 25, 500,
	       dlsw_csm_command, CSM_EXPLQ_DEPTH,
	       "Configure depth of DLSw explorer queue",
	       "Maximum Depth of explorer queue in packets", PRIV_CONF);

/**********************************************************************
 * [no] dlsw duplicate-path-bias load-balance
 */

EOLS    (csm_dup_eol, dlsw_csm_command, CSM_DUP_PATH);
KEYWORD (csm_load_bal, csm_dup_eol, no_alt, "load-balance",
           "Sessions will be load balanced across duplicate paths",
           PRIV_CONF);
NOPREFIX(csm_dup_noprefix, csm_load_bal, csm_dup_eol);
KEYWORD (csm_dup_path, csm_dup_noprefix, csm_explq, "duplicate-path-bias",
         "Configure how duplicate paths will be handled",
         PRIV_CONF);


/**********************************************************************
 * [no] dlsw icannotreach saps <sap> <sap> ...
 * (format should allow for future extensions)
 * OBJ(int,1) = {PEER_ICNR_SAP}
 * OBJ(int,2) = number of valid saps in uiarray
 * uiarray = array of saps
 */

EOLS    (icnr_eol, dlsw_peer_command, PEER_ICNR);
pdecl(icnr_sap_1a);

PRINT   (icnr_sap_2b, no_alt, "Too many saps in list");
TESTVAR (icnr_sap_2a, icnr_sap_2b, icnr_sap_1a,
         NONE, NONE, NONE, OBJ(int,2), (MAX_SAPS_IN_LIST + 1));
EVAL    (icnr_sap_2, icnr_sap_2a,
         csb->uiarray[GETOBJ(int,2) ++] = GETOBJ(int,9));
HEXDIGIT(icnr_sap_1a, icnr_sap_2, icnr_eol,
         OBJ(int, 9), 0, 0xFE, "Even SAP Value (hex)");
HEXDIGIT(icnr_sap_1, icnr_sap_2, no_alt, 
         OBJ(int, 9), 0, 0xFE, "Even SAP Value (hex)");
KEYWORD_ID(icnr_sap, icnr_sap_1, no_alt,
           OBJ(int,1), PEER_ICNR_SAP, "saps",
           "Configure a list of SAPs not locally reachable by this router",
           PRIV_CONF);

NVGENS  (icnr_nvgen, icnr_sap, dlsw_peer_command, PEER_ICNR);
KEYWORD (dlsw_icnr, icnr_nvgen, csm_dup_path, "icannotreach", 
         "Configure a resource not locally reachable by this router",
         PRIV_CONF);


/**********************************************************************
 * [no] dlsw icanreach { mac-exclusive [remote] |
 *                       netbios-exclusive [remote] |
 *                       mac-address |
 *                       netbios-name |
 *                       saps }
 *
 * OBJ(int,1) = {ICR_MAC_EXCL | ICR_NBNAME_EXCL | ICR_MAC | ICR_NBNAME | ICR_SAPS}
 * (*OBJ(hwaddr,1)) = <mac-addr>
 * OBJ(int,2) = TRUE, if "mask"
 * (*OBJ(hwaddr,2)) = <mac-mask>
 * OBJ(string,1) = netbios-name
 * OBJ(int,3) = number of valid saps in uiarray
 * uiarray = array of saps
 * OBJ(int, 4) = TRUE, if mac-exclusive remote or
 *                        netbios-exclusive remote
 *
 * "exclusive" option:
 * If configured, this DLSw router will advertise to DLSw remote peers
 * that it will only etablish circuits for the resources defined via the
 * dlsw resource mac-address and resource netbios-name configuration
 * commands.
 */

EOLS    (icr_eol, dlsw_peer_command, PEER_ICANREACH);
pdecl(icr_sap_1a);

PRINT   (icr_sap_2b, no_alt, "Too many saps in list");
TESTVAR (icr_sap_2a, icr_sap_2b, icr_sap_1a,
         NONE, NONE, NONE, OBJ(int,3), (MAX_SAPS_IN_LIST + 1));
EVAL    (icr_sap_2, icr_sap_2a,
         csb->uiarray[GETOBJ(int,3) ++] = GETOBJ(int,9));
HEXDIGIT(icr_sap_1a, icr_sap_2, icr_eol,
         OBJ(int, 9), 0, 0xFE, "Even SAP Value (hex)");
HEXDIGIT(icr_sap_1, icr_sap_2, no_alt,
         OBJ(int, 9), 0, 0xFE, "Even SAP Value (hex)");
KEYWORD_ID(icr_saps, icr_sap_1, no_alt,
           OBJ(int,1), ICR_SAPS, "saps",
           "Configure a list of SAPs ONLY locally reachable by this router",
           PRIV_CONF);

STRING  (icr_nb_name, icr_eol, no_alt,
        OBJ(string,1), "Netbios name (wildcards allowed)");
KEYWORD_ID(icr_nb, icr_nb_name, icr_saps,
           OBJ(int,1), ICR_NBNAME, "netbios-name",
           "Configure a NetBios name that this router can locally reach",
           PRIV_CONF);
ENADDR  (icr_mask_val, icr_eol, no_alt,
         OBJ(hwaddr,2), "MAC address Mask");
KEYWORD_ID(icr_mac_mask, icr_mask_val, icr_eol, OBJ(int,2), TRUE, "mask",
           "Set a MAC Mask - MACx is reachable if (MACx & MASK) == MACVALUE",
           PRIV_CONF);
SET     (icr_mask_def, icr_mac_mask, OBJ(int,2), FALSE);

ENADDR  (icr_mac_addr, icr_mask_def, no_alt,
         OBJ(hwaddr,1), "MAC address (MAC Value)");
KEYWORD_ID(icr_mac, icr_mac_addr, icr_nb, OBJ(int,1), ICR_MAC, "mac-address",
           "Configure a MAC address that this router can locally reach",
           PRIV_CONF);

KEYWORD_ID(icr_nb_excl_rem, icr_eol, icr_eol,
           OBJ(int,4), TRUE, "remote",
           "Netbios exclusive applied only on the remote",
           PRIV_CONF);

KEYWORD_ID(icr_nb_excl, icr_nb_excl_rem, icr_mac,
           OBJ(int,1), ICR_NB_EXCL, "netbios-exclusive",
           "Router can ONLY reach the NetBios names that are user configured",
           PRIV_CONF);

KEYWORD_ID(icr_mac_excl_rem, icr_eol, icr_eol,
           OBJ(int,4), TRUE, "remote",
           "Mac exclusive applied only on the remote",
           PRIV_CONF);

KEYWORD_ID(icr_mac_excl, icr_mac_excl_rem, icr_nb_excl,
           OBJ(int,1), ICR_MAC_EXCL, "mac-exclusive",
           "Router can ONLY reach the mac addresses that are user configured",
           PRIV_CONF);

NVGENS  (icr_nvgen, icr_mac_excl, dlsw_peer_command, PEER_ICANREACH);
KEYWORD (dlsw_icr, icr_nvgen, dlsw_icnr, "icanreach",
         "Configure resources locally reachable by this router",
         PRIV_CONF);


/**********************************************************************
 * [no] dlsw remote-peer <port-list-num>
 *      {tcp <ipaddr> | fst <ipaddr> | interface <name> [<mac-addr>]}
 *      frame-relay interface serial <number> <dlci-number>}
 *      [local-ack] (not an option in release 1)
 *      [priority] [cost <cost>] [lf <size>]
 *      [keepalive <time>] [tcp-queue-max <size] 
 *      [lsap-output-list <list>] 
 *      [host-netbios-out <host-list-name>] 
 *      [bytes-netbios-out <bytes-list-name>]
 *      [backup-peer <ip-addr>] [timeout <seconds>]
 *      [linger <seconds>] [dynamic] [inactivity <minutes>]
 *      [no-llc <minutes>]
 *	[dest-mac <mac-addr> ]
 *	[dmac-output-list <list>]
 *      [passive]
 *
 * (*OBJ(paddr,1)) = <ipaddr>
 * (*OBJ(hwaddr,1)) = <mac-addr> if <name> is a LAN interface
 * OBJ(idb,1) = <name>
 * OBJ(int,1) = <port-list>
 * OBJ(int,2) = DLSW_TCP | DLSW_FST | DLSW_IF | DLSW_FR
 * OBJ(int,3) = TRUE if "local-ack"
 * OBJ(int,4) = TRUE if "priority"
 * OBJ(int,5) = remote peer's <cost>
 * OBJ(int,6) = largest frame <size> 
 * OBJ(int,7) = TRUE if "keepalive"
 * OBJ(int,8) = <time> of "keepalive"
 * OBJ(int,9) = <size> of "tcp-queue-max"
 * OBJ(int,10) = <list> number of "lsap-output-list" or 0
 * OBJ(int,11) = TRUE if "host-netbios-out"
 * OBJ(string,1) = <host-list-name> of "host-netbios-out"
 * OBJ(int,12) = TRUE if "bytes-netbios-out"
 * OBJ(string,2) = <bytes-list-name> of "bytes-netbios-out"
 * OBJ(int,13) = TRUE if "backup-peer"
 * OBJ(int,14) = Frame-Relay DLCI Number
 * OBJ(int,15) = <time> of "timeout"
 * OBJ(int,16) = TRUE if "dynamic"
 * OBJ(int,17) = <time> of "inactivity"
 * OBJ(int,18) = <time> of "linger"
 * OBJ(int,19) = <time> of "no-llc"
 * OBJ(int,20) = <list> number of "dmac-output-list"
 * OBJ(int,21) = TRUE, if "passive"
 * OBJ(hwaddr,2) = <mac-addr> of "dest-mac"
 * (*OBJ(paddr,2)) = <ipaddr> of primary peer
 */

EOLS    (rpeer_eol, dlsw_peer_command, PEER_REMOTE);
pdecl(rpeer_opts);

/*
 * The below options loop back to rpeer_opts so that they can
 * be input in any order.  If an option is specified multiple times,
 * the last value is used.
 */

/* [tcp-queue-max <size] */
NUMBER  (rpeer_tcpq_num, rpeer_opts, no_alt,
         OBJ(int,9), 25, 2000, "Maximum TCP Queue Size in Packets");
KEYWORD (rpeer_tcpq, rpeer_tcpq_num, no_alt, "tcp-queue-max",
         "Maximum output TCP queue size for this remote peer", PRIV_CONF);

/* [priority] */
KEYWORD_ID(rpeer_pri, rpeer_opts, rpeer_tcpq, OBJ(int,4), TRUE, "priority", 
           "Enable prioritization features for this remote peer", PRIV_CONF);

/* only provide tcp options for TCP transport peers */
EQUAL     (rpeer_tcp_opts, rpeer_pri, no_alt, OBJ(int,2), DLSW_TCP);

/* [passive] */
KEYWORD_ID(rpeer_passive, rpeer_opts, rpeer_tcp_opts,  OBJ(int,21), TRUE,
           "passive", "Local peer will not initiate this remote peer connection",
           PRIV_CONF);

/* [backup-peer <ip-addr>] */
IPADDR  (rpeer_primary_ip, rpeer_opts, no_alt,
         OBJ(paddr,2), "Primary peer's IP address");
KEYWORD_ID(rpeer_backup, rpeer_primary_ip, rpeer_passive,
           OBJ(int,13), TRUE, "backup-peer",
           "Configure as a backup to an existing TCP/FST peer",
           PRIV_CONF);

/* [timeout <time>] */
NUMBER (rpeer_rtto_time, rpeer_opts, no_alt, OBJ(int,15), 5, 1200,
	"Remote peer transmission timeout");
KEYWORD(rpeer_rtto_int, rpeer_rtto_time, rpeer_backup, "timeout",
	   "Set retransmission timeout value for this remote peer",
	   PRIV_CONF);

/* [dynamic] */
KEYWORD_ID(rpeer_dynamic, rpeer_opts, rpeer_rtto_int, OBJ(int,16), 
	   TRUE, "dynamic", 
	   "Enable dynamic connection for this remote peer",
	   PRIV_CONF);

/* [inactivity <time>] */
NUMBER (rpeer_inactive_time, rpeer_opts, no_alt, 
        OBJ(int,17), 1, 300, "Dynamic peer inactivity timer[minutes]");
KEYWORD(rpeer_inactivity, rpeer_inactive_time, rpeer_dynamic,
           "inactivity", "Dynamic peer inactivity", PRIV_CONF);

/* [linger <time>] */
NUMBER(rpeer_linger_time, rpeer_opts, no_alt,
       OBJ(int,18), 0, 1440, "Backup peer linger time[minutes]");
KEYWORD(rpeer_linger, rpeer_linger_time, rpeer_inactivity,
	   "linger", "Backup peer linger", PRIV_CONF);

/* [no-llc <time>] */
NUMBER(rpeer_no_llc_time, rpeer_opts, no_alt,
       OBJ(int,19), 1, 300, "dynamic peer no LLC timer[minutes]");
KEYWORD(rpeer_no_llc, rpeer_no_llc_time, rpeer_linger,
           "no-llc", "dynamic peer no LLC", PRIV_CONF);

/* [dmac-output-list <list>] */
NUMBER(rpeer_dmac_outlist, rpeer_opts, no_alt,
        OBJ(int,20), MINMACADDRACC, MAXMACADDRACC, "access list number");
KEYWORD(rpeer_dmac_list, rpeer_dmac_outlist, rpeer_no_llc,
         "dmac-output-list", "Filter output destination mac addresses",
         PRIV_CONF);

/* [dest-mac <mac-addr> ] */
ENADDR(rpeer_dmac_out_addr, rpeer_opts, no_alt,
      OBJ(hwaddr,2), "Destination MAC address");
KEYWORD(rpeer_dmac_out, rpeer_dmac_out_addr, rpeer_dmac_list,
        "dest-mac",
        "Exclusive destination mac-addr for remote peer",
        PRIV_CONF);

/* [bytes-netbios-out <bytes-list-name>] */
STRING  (rpeer_net_bname, rpeer_opts, no_alt, OBJ(string,2),
         "Name of previously defined netbios bytes access list filter");
KEYWORD_ID(rpeer_net_bytes, rpeer_net_bname, rpeer_dmac_out,
         OBJ(int,12), TRUE, "bytes-netbios-out",
         "Configure netbios bytes output filtering for this peer",
         PRIV_CONF);

/* [host-netbios-out <host-list-name>] */
STRING  (rpeer_net_hname, rpeer_opts, no_alt, OBJ(string,1),
         "Name of previously defined netbios host access list filter");
KEYWORD_ID(rpeer_net_host, rpeer_net_hname, rpeer_net_bytes,
         OBJ(int,11), TRUE, "host-netbios-out",
         "Configure netbios host output filtering for this peer",
         PRIV_CONF);

/* [lsap-output-list <list>] */
ACCESS_LIST(rpeer_lsap_list, rpeer_opts, no_alt,
        OBJ(int, 10), MINTYPEACC, MAXTYPEACC,
        TYPECODE_MIN_ACL, TYPECODE_MAX_ACL, "access list number");
KEYWORD (rpeer_lsap, rpeer_lsap_list, rpeer_net_host,
         "lsap-output-list", "Filter output IEEE 802.5 encapsulated packets",
         PRIV_CONF);

/* [pass-thru] - In Arizona, default is local-ack*/
KEYWORD_ID(rpeer_lack, rpeer_opts, rpeer_lsap, OBJ(int,3), FALSE, "pass-thru",
           "Remote peer is capable of locally acknowledging DLC sessions",
           PRIV_CONF);

/* [keepalive <time>] */
NUMBER  (rpeer_keep_time, rpeer_opts, no_alt,
         OBJ(int,8), 0, 1200, "Remote peer keepalive time");
KEYWORD_ID(rpeer_keep_int, rpeer_keep_time, rpeer_lack, 
         OBJ(int,7), TRUE, "keepalive", 
         "Set keepalive interval for this remote peer", PRIV_CONF);

/* [lf <size>] */
KEYWORD_OPTIONS(rpeer_lf_number, rpeer_opts, no_alt,
		peer_lf_number_options, OBJ(int,6),
		PRIV_CONF, 0);
KEYWORD (rpeer_lf, rpeer_lf_number, rpeer_keep_int,
         "lf", "Largest Frame Size for this Remote Peer", PRIV_CONF);

/* [cost <cost>] */
NUMBER  (rpeer_cost_num, rpeer_opts, no_alt,
         OBJ(int,5), 1, 5, "Remote Peer Cost Number");
KEYWORD (rpeer_cost, rpeer_cost_num, rpeer_lf,
         "cost", "Cost to Reach this Remote Peer", PRIV_CONF);

/* NOP is for looping to get options in any order. */
NOP     (rpeer_opts, rpeer_cost, rpeer_eol);
SET     (rpeer_def_dmac_list, rpeer_opts, OBJ(int,20), 0);
SET     (rpeer_def_no_llc, rpeer_def_dmac_list, OBJ(int,19), 0);
SET     (rpeer_def_linger, rpeer_def_no_llc, OBJ(int,18), -1);
SET     (rpeer_def_inact, rpeer_def_linger, OBJ(int,17), -1);
SET     (rpeer_def_timeout, rpeer_def_inact, OBJ(int,15), 0);
SET     (rpeer_def_backup, rpeer_def_timeout, OBJ(int,13), FALSE);
SET     (rpeer_def_lsap, rpeer_def_backup, OBJ(int,10), 0);
SET     (rpeer_def_tcpq, rpeer_def_lsap, OBJ(int,9), DLSW_MAX_TCP_QLEN);
SET     (rpeer_def_lf, rpeer_def_tcpq, OBJ(int,6), RC_LFINITIAL);
SET     (rpeer_def_cost, rpeer_def_lf, OBJ(int,5), DLSW_NORM_PCOST);
SET     (rpeer_def_lack, rpeer_def_cost, OBJ(int,3), TRUE);
NOPREFIX(rpeer_noprefix, rpeer_def_lack, rpeer_eol);

/* [interface <name> <mac-addr>] */
ENADDR  (rpeer_int_addr, rpeer_noprefix, no_alt,
         OBJ(hwaddr,1), "MAC address");
TESTVAR (rpeer_int_test, rpeer_noprefix, rpeer_int_addr,
         NONE, NONE, NONE, OBJ(int,15), FALSE);
EVAL    (rpeer_int_eval, rpeer_int_test,
         (GETOBJ(int,15) = GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK));

NUMBER  (rpeer_dlci, rpeer_noprefix, no_alt,
         OBJ(int,14), 7, 1007, "Frame-Relay DLCI Number");
TESTVAR (rpeer_fr_test, rpeer_noprefix, rpeer_dlci,
         NONE, NONE, NONE, OBJ(int,9), FALSE);
EVAL    (rpeer_fr_eval, rpeer_fr_test,
         (GETOBJ(int,9) = GETOBJ(idb,1)->hwptr->status & IDB_SERIAL));

FORWARD_INTERFACE(rpeer_fr_ser, rpeer_fr_eval, no_alt,
		  OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_SUBIF);
KEYWORD_ID(rpeer_fr_int, rpeer_fr_ser, no_alt,
           OBJ(int,2), DLSW_FR, common_str_interface,
           "Serial interface over which to run TR traffic", PRIV_CONF);
KEYWORD_ID(rpeer_fr, rpeer_fr_int, no_alt,
           OBJ(int,2), DLSW_FR, "frame-relay",
           "Use Frame Relay for remote peer transport", PRIV_CONF);

INTERFACE(rpeer_int, rpeer_int_eval, no_alt, OBJ(idb,1), IFTYPE_SERIAL);
KEYWORD_ID(rpeer_interface, rpeer_int, rpeer_fr,
           OBJ(int,2), DLSW_IF, common_str_interface,
           "Use a direct interface for remote peer transport", PRIV_CONF);

/* [fst <ipaddr>] */
IPADDR  (rpeer_fst_ipaddr, rpeer_noprefix, no_alt,
         OBJ(paddr,1), "Remote FST peer's IP address");
KEYWORD_ID(rpeer_fst, rpeer_fst_ipaddr, rpeer_interface,
           OBJ(int,2), DLSW_FST, "fst", 
           "Use fast sequence transport (FST) for remote peer transport", 
           PRIV_CONF);

/* [tcp <ip-address>] */
IPADDR  (rpeer_tcp_ipaddr, rpeer_noprefix, no_alt,
         OBJ(paddr,1), "Remote peer IP address");
KEYWORD_ID(rpeer_tcp, rpeer_tcp_ipaddr, rpeer_fst,
           OBJ(int,2), DLSW_TCP, "tcp", 
           "Use TCP for remote peer transport", PRIV_CONF);

/* <port-list> */
NUMBER  (rpeer_rlist, rpeer_tcp, no_alt,
         OBJ(int,1), 0, SRB_MAX_RING_NUM, "Remote peer ring group list");

NVGENS  (rpeer_nvgen, rpeer_rlist, dlsw_peer_command, PEER_REMOTE);

KEYWORD (rpeer, rpeer_nvgen, dlsw_icr,
         "remote-peer", "Configure a remote peer", PRIV_CONF);



/**********************************************************************
 * [no] dlsw bgroup-list <list-num-num> bgroups <bgroup-no> <bgroup-no> ...
 *
 * OBJ(int,1) = <list_num>
 * OBJ(int,2) = number of valid bgroup numbers in uiarray
 * uiarray = array of bgroup numbers
 */

EOLS    (bgroup_list_eol, dlsw_peer_command, PEER_BGROUP_LIST);
pdecl(bgroup_list_1a);

PRINT   (bgroup_list_2b, no_alt, 
         "Too many transparent bridge groups in bgroup-list");
TESTVAR (bgroup_list_2a, bgroup_list_2b, bgroup_list_1a,
         NONE, NONE, NONE, OBJ(int,2), (MAX_BGROUPS_IN_LIST + 1));
EVAL    (bgroup_list_2, bgroup_list_2a,
         csb->uiarray[GETOBJ(int,2) ++] = GETOBJ(int,9));
NUMBER  (bgroup_list_1a, bgroup_list_2, bgroup_list_eol, OBJ(int,9), 1, 
         DLSW_SPAN_MAXINDEX - 1,
         "Transparent bridge group number");
NUMBER  (bgroup_list_1, bgroup_list_2, no_alt, OBJ(int,9), 1, 
          SRB_MAX_RING_NUM,
         "Transparent bridge group number");

KEYWORD (bgroups, bgroup_list_1, no_alt, "bgroups",
         "Required keyword before specifying transparent bridge group numbers",
         PRIV_CONF);

NUMBER  (bgroup_list_num , bgroups, no_alt,
         OBJ(int,1), 1, 255, "transparent bridge group list number");
NVGENS  (bgroup_list_nvgen, bgroup_list_num, dlsw_peer_command, 
         PEER_BGROUP_LIST);
KEYWORD (bgroup_list, bgroup_list_nvgen, rpeer,
         "bgroup-list", "Configure a transparent bridge group list", 
         PRIV_CONF);




/**********************************************************************
 * [no] dlsw ring-list <list-num-num> rings <ring> <ring> ...
 *
 * OBJ(int,1) = <list_num>
 * OBJ(int,2) = number of valid rings in uiarray
 * uiarray = array of rings
 */

EOLS    (ring_list_eol, dlsw_peer_command, PEER_RING_LIST);
pdecl(ring_list_1a);

PRINT   (ring_list_2b, no_alt, "Too many rings in ring-list");
TESTVAR (ring_list_2a, ring_list_2b, ring_list_1a,
         NONE, NONE, NONE, OBJ(int,2), (MAX_RINGS_IN_LIST + 1));
EVAL    (ring_list_2, ring_list_2a,
         csb->uiarray[GETOBJ(int,2) ++] = GETOBJ(int,9));
NUMBER  (ring_list_1a, ring_list_2, ring_list_eol, OBJ(int,9), 1, 
         SRB_MAX_RING_NUM,
         "Physical or virtual ring number");
NUMBER  (ring_list_1, ring_list_2, no_alt, OBJ(int,9), 1, SRB_MAX_RING_NUM,
         "Physical or virtual ring number");

KEYWORD (rings, ring_list_1, no_alt, "rings",
         "Required keyword before specifying physical/virtual ring numbers",
         PRIV_CONF);

NUMBER  (ring_list_num , rings, no_alt,
         OBJ(int,1), 1, 255, "Ring list number");
NVGENS  (ring_list_nvgen, ring_list_num, dlsw_peer_command, PEER_RING_LIST);
KEYWORD (ring_list, ring_list_nvgen, bgroup_list,
         "ring-list", "Configure a ring list", PRIV_CONF);




/**********************************************************************
 * [no] dlsw port-list <list-num> <interface> <interface> ...
 *
 * OBJ(int,1) = <list_num>
 * OBJ(int,2) = number of hwidb pointers in uiarray
 * uiarray = array of hwidb's
 */

EOLS    (port_list_eol, dlsw_peer_command, PEER_PORT_LIST);
pdecl(port_loop);

PRINT   (port_error, no_alt, "Too many interfaces in port-list");
TESTVAR (port_test, port_error, port_loop,
         NONE, NONE, NONE, OBJ(int,2), (MAX_PORTS_IN_LIST + 1));
EVAL    (port_int_eval, port_test,
         csb->uiarray[GETOBJ(int,2) ++] = (uint) GETOBJ(idb,1));

INTERFACE(port_int2, port_int_eval, port_list_eol, OBJ(idb,1), 
          IFTYPE_TOKENRING | IFTYPE_SERIAL | IFTYPE_ETHER | IFTYPE_FASTETHER);
NOP     (port_loop, port_int2, no_alt);

INTERFACE(port_int1, port_int_eval, no_alt, OBJ(idb,1), 
          IFTYPE_TOKENRING | IFTYPE_SERIAL | IFTYPE_ETHER | IFTYPE_FASTETHER);
SET     (port_def, port_int1, OBJ(int,2), 0);
NUMBER  (port_list_num , port_def, no_alt,
         OBJ(int,1), 1, 255, "Port-list number");
NVGENS  (port_list_nvgen, port_list_num, dlsw_peer_command, PEER_PORT_LIST);
KEYWORD (port_list, port_list_nvgen, ring_list,
         "port-list", "Configure a port list", PRIV_CONF);


/************************************************************************
 * [no] dlsw local-peer [peer-id <ipaddr>] [group <group>] 
 *      [border] [cost <cost>] [lf <size>]
 *      [keepalive <time>] [passive] [promiscuous] [biu-segment]
 *    [init-pacing-window <size>] [max-pacing-window <size>]
 *
 * OBJ(int,1) = TRUE if "peer-id"
 * (*OBJ(paddr,1)) = <ipaddr>
 * OBJ(int,2) = TRUE if "group"
 * OBJ(int,3) = <group>
 * OBJ(int,4) = TRUE if "border-peer"
 * OBJ(int,5) = TRUE if "cost"
 * OBJ(int,6) = <cost>
 * OBJ(int,7) = TRUE if "lf"
 * OBJ(int,8) = <size>
 * OBJ(int,9) = TRUE if "keepalive"
 * OBJ(int,10) = <time>
 * OBJ(int,11) = TRUE if "passive"
 * OBJ(int,12) = TRUE if "promiscuous"
 * OBJ(int,13) = TRUE if "biu-segment"
 * OBJ(int,14) = initial pacing window <size>
 * OBJ(int,15) = maximum pacing window <size>
 */

EOLS    (lpeer_eol, dlsw_peer_command, PEER_LOCAL);
pdecl(lpeer_opts);

/*
 * The below options loop back to lpeer_opts so that they can
 * be input in any order.  If an option is specified multiple times,
 * the last value is used.
 */

/* [max-pacing-window <size>] */
NUMBER  (lpeer_mpw_num, lpeer_opts, no_alt,
         OBJ(int,15), 1, 2000, "Maximum pacing window size");
KEYWORD (lpeer_mpw, lpeer_mpw_num, no_alt, "max-pacing-window",
         "Maximum Pacing Window Size for this local peer", PRIV_CONF);

/* [init-pacing-window <size>] */
NUMBER  (lpeer_ipw_num, lpeer_opts, no_alt,
         OBJ(int,14), 1, 2000, "Initial pacing window size");
KEYWORD (lpeer_ipw, lpeer_ipw_num, lpeer_mpw, "init-pacing-window",
         "Initial Pacing Window Size for this local peer", PRIV_CONF);

/* [biu-segment] */
KEYWORD_ID (lpeer_biu_seg, lpeer_opts, lpeer_ipw,
           OBJ(int,13), TRUE, "biu-segment",
           "XID3 max receivable i-field spoofing and BIU segmenting",
           PRIV_CONF);

/* [promiscuous] */
KEYWORD_ID (lpeer_pod_dis, lpeer_opts, lpeer_biu_seg,
           OBJ(int,12), TRUE, "promiscuous",
           "Accept connections from non-configured remote peers",
           PRIV_CONF);

/* [passive] */
KEYWORD_ID (lpeer_passive, lpeer_opts, lpeer_pod_dis,
           OBJ(int,11), TRUE, "passive",
           "This router will not initiate remote peer connections", PRIV_CONF);

/* [keepalive time] */
NUMBER  (lpeer_keep_time, lpeer_opts, no_alt,
         OBJ(int,10), 0, 1200, "Keepalive Interval (seconds)");
KEYWORD_ID(lpeer_keep_int, lpeer_keep_time, lpeer_passive, OBJ(int,9), TRUE,
           "keepalive", 
           "Set the default remote peer keepalive interval", PRIV_CONF);

/* [lf <size>] */
KEYWORD_ID(lpeer_lf_17800, lpeer_opts, no_alt,
           OBJ(int,8), RC_LF17800,
           "17800", "17800 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_11454, lpeer_opts, lpeer_lf_17800,
           OBJ(int,8), RC_LF11454,
           "11454", "11454 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_11407, lpeer_opts, lpeer_lf_11454,
           OBJ(int,8), RC_LF11407,
           "11407", "11407 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_8144, lpeer_opts, lpeer_lf_11407,
           OBJ(int,8), RC_LF8144,
           "8144", "8144 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_4472, lpeer_opts, lpeer_lf_8144,
           OBJ(int,8), RC_LF4472,
           "4472", "4472 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_2052, lpeer_opts, lpeer_lf_4472,
           OBJ(int,8), RC_LF2052,
           "2052", "2052 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_1500, lpeer_opts, lpeer_lf_2052,
           OBJ(int,8), RC_LF1500,
           "1500", "1500 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_1470, lpeer_opts, lpeer_lf_1500,
           OBJ(int,8), RC_LF1470,
           "1470", "1470 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(lpeer_lf_516, lpeer_opts, lpeer_lf_1470,
           OBJ(int,8), RC_LF516,
           "516", "516 byte maximum frame size", PRIV_CONF);

KEYWORD_ID(lpeer_lf, lpeer_lf_516, lpeer_keep_int,
           OBJ(int,7), TRUE,
           "lf", "Local peer largest frame size", PRIV_CONF);

/* [cost <cost>] */
NUMBER  (lpeer_cost_num, lpeer_opts, no_alt,
         OBJ(int,6), DLSW_MIN_PCOST, DLSW_MAX_PCOST, "DLSw Local Peer Cost");
KEYWORD_ID (lpeer_cost, lpeer_cost_num, lpeer_lf,
           OBJ(int,5), TRUE,
           "cost", "Set peer cost advertised to remote peers", PRIV_CONF);

/* [border] */
KEYWORD_ID(lpeer_border, lpeer_opts, lpeer_cost,
           OBJ(int,4), TRUE,
           "border", "Capable of operating as a border peer ", PRIV_CONF);

/* [group <group>] */
NUMBER  (lpeer_group_num, lpeer_opts, no_alt,
         OBJ(int,3), DLSW_MIN_PGROUP, DLSW_MAX_PGROUP,
         "Peer group number");
KEYWORD_ID (lpeer_group, lpeer_group_num, lpeer_border,
           OBJ(int,2), TRUE,
           "group", "Set the peer group number for this router", PRIV_CONF);


/* [peer-id <ip-address>] */
IPADDR  (lpeer_id_num, lpeer_opts, no_alt,
         OBJ(paddr,1), "Local Peer IP address");
KEYWORD_ID (lpeer_id, lpeer_id_num, lpeer_group,
           OBJ(int,1), TRUE, "peer-id", 
           "local-peer IP address; required for TCP/FST and peer groups", 
           PRIV_CONF);

/* NOP is for looping to get options in any order. */
NOP     (lpeer_opts, lpeer_id, lpeer_eol);
SET     (lpeer_def_mpw, lpeer_opts, OBJ(int, 15), DLSW_FC_MAX_WINDOW);
SET     (lpeer_def_ipw, lpeer_def_mpw, OBJ(int, 14), DLSW_FC_INIT_WINDOW);
NOPREFIX(lpeer_noprefix, lpeer_def_ipw, lpeer_eol);

NVGENS  (lpeer_nvgen, lpeer_noprefix, dlsw_peer_command, PEER_LOCAL);

KEYWORD (lpeer, lpeer_nvgen, port_list,
         "local-peer", "Configure local peer", PRIV_CONF);

/************************************************************************
 * [no] dlsw nornr
 */

EOLS    (nornr_eol, dlsw_nornr_command, DLSW_NORNR);

KEYWORD (dlsw_llc2_opts, nornr_eol,no_alt, "nornr",
         "Disable RNR xmit after llc2 circuit comes up",
          PRIV_CONF|PRIV_HIDDEN);

KEYWORD (dlsw_llc2, dlsw_llc2_opts, lpeer, "llc2",
         "Dlsw llc2 options", PRIV_CONF|PRIV_HIDDEN);

/************************************************************************
 * [no] dlsw disable
 */
EOLS    (disable_eol, dlsw_disable_command, DLSW_DISABLE);
KEYWORD (dlsw_disable, disable_eol, dlsw_llc2, "disable",
         "Disable DLSw without altering the configuration", PRIV_CONF);

/************************************************************************
 * Global DLSW Configuration Commands
 */
KEYWORD (configure_dlsw, dlsw_disable, NONE, "dlsw",
         "Data Link Switching global configuration commands", PRIV_CONF);
TEST_BOOLEAN(dlsw_test, configure_dlsw, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE dlsw_test
