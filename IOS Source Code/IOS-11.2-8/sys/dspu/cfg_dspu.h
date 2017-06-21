/* $Id: cfg_dspu.h,v 3.3.12.8 1996/09/13 08:31:40 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/cfg_dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_dspu.h,v $
 * Revision 3.3.12.8  1996/09/13  08:31:40  lcox
 * CSCdi61967:  dspu - retries & retry-timeout dont work like doc says
 * Branch: California_branch
 *   retry-timeout range changed in code to agree with user
 *   documentation  (was 0..600, is now 1..600
 *
 * Revision 3.3.12.7  1996/08/30  02:25:15  lcox
 * CSCdi61969:  dspu pool command not parsed as documented
 * Branch: California_branch
 *
 * Revision 3.3.12.6  1996/08/09  01:53:56  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.3.12.5  1996/07/24  01:23:30  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.3.12.4  1996/07/15  15:38:03  ppearce
 * CSCdi62916:  FDDI/FastEthernet i/f not supported on dspu host/pu command
 * Branch: California_branch
 *
 * Revision 3.3.12.3  1996/07/05  00:59:32  szdravko
 * CSCdi61970:  dspu - range of window parameter is incorrect
 * Branch: California_branch
 *
 * Revision 3.3.12.2  1996/05/21  09:46:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.12.1  1996/05/17  10:47:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.3  1996/05/10  05:07:26  schiang
 * CSCdi57318:  dspu ncia configuration command needs default setting
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.2.2  1996/05/03  17:18:08  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.2.1  1996/03/28  08:20:14  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3  1996/02/17  09:17:00  ppearce
 * CSCdi46728:  DSPU HOST can not be configured without XID parameter
 *
 * Revision 3.2.4.1  1996/01/20  06:08:29  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:04:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:56:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/11  04:20:45  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.2  1995/07/06  23:15:31  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/07/04  01:53:02  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:02:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 * dspu default_pu [window <window_size>] [maxiframe <maxIframe>]
 * no dspu default_pu
 */
EOLS	(cfg_dspu_dpu_eol, dspu_command, DSPU_DEFAULT_PU);
pdecl	(cfg_dspu_dpu_nop);

NUMBER	(cfg_dspu_dpu_max_iframe, cfg_dspu_dpu_nop, no_alt, 
	 OBJ(int, 2), DSPU_MIN_MAXIFRAME, DSPU_MAX_MAXIFRAME, 
         "DSPU Default Pu MaxIframe");
KEYWORD (cfg_dspu_dpu_maxiframe, cfg_dspu_dpu_max_iframe, cfg_dspu_dpu_eol,
	 "maxiframe","DSPU Default PU MaxIframe command",PRIV_CONF);
NUMBER	(cfg_dspu_dpu_window_size, cfg_dspu_dpu_nop, no_alt, 
	OBJ(int, 1), 1, 127, "DSPU Default Pu Window Size");
KEYWORD (cfg_dspu_dpu_window, cfg_dspu_dpu_window_size, cfg_dspu_dpu_maxiframe,
	 "window","DSPU Default PU Window command",PRIV_CONF);
NOP	(cfg_dspu_dpu_nop, cfg_dspu_dpu_window, no_alt);
SET	(cfg_dspu_dpu_maxiframe_set, cfg_dspu_dpu_nop, OBJ(int,2), 0);
SET	(cfg_dspu_dpu_window_set, cfg_dspu_dpu_maxiframe_set, OBJ(int,1), 0);
NOPREFIX (cfg_dspu_dpu_noprefix, cfg_dspu_dpu_window_set, cfg_dspu_dpu_eol);

NVGENS	(cfg_dspu_dpu_nvgen, cfg_dspu_dpu_noprefix, 
	dspu_command, DSPU_DEFAULT_PU);
KEYWORD (cfg_dspu_dpu, cfg_dspu_dpu_nvgen, no_alt,
	 "default-pu",
	 "DSPU Default PU command",
	 PRIV_CONF);
/*************************************************************************
 * dspu lu <lu_start> [<lu_end>] [pool <pool_name>] 
 *		[host <host_name> <host_lu_start>] [pu <pu_name>]
 * no dspu lu <lu_start> [<lu_end>] [pool <pool_name>]
 *		 [host <host_name>] [<host_lu_start>] [pu <pu_name>]
 *
 */
EOLS	(cfg_dspu_lu_eol, dspu_command, DSPU_LU);
pdecl	(cfg_dspu_lu_nop);

STRING	(cfg_dspu_lu_pu_name, cfg_dspu_lu_nop, no_alt,
	 OBJ(string, 3), "DSPU LU Pu Name");
KEYWORD_ID (cfg_dspu_lu_pu, cfg_dspu_lu_pu_name, cfg_dspu_lu_eol,
         OBJ(int,6), TRUE,
	 "pu","DSPU LU Pu command",PRIV_CONF);
NUMBER	(cfg_dspu_lu_host_lu_start, cfg_dspu_lu_nop, no_alt, 
	OBJ(int, 5), 1, 255, "DSPU LU Host Lu StartAddr");
STRING	(cfg_dspu_lu_host_name, cfg_dspu_lu_host_lu_start, no_alt,
	 OBJ(string, 2), "DSPU LU Host Name");
KEYWORD_ID (cfg_dspu_lu_host, cfg_dspu_lu_host_name, cfg_dspu_lu_pu,
         OBJ(int,4), TRUE,
	 "host","DSPU LU Host command",PRIV_CONF);
STRING	(cfg_dspu_lu_pool_name, cfg_dspu_lu_nop, no_alt,
	 OBJ(string, 1), "DSPU LU Pool Name");
KEYWORD_ID (cfg_dspu_lu_pool, cfg_dspu_lu_pool_name, cfg_dspu_lu_host,
         OBJ(int,3), TRUE,
	 "pool","DSPU LU Pool command",PRIV_CONF);
NUMBER	(cfg_dspu_lu_end, cfg_dspu_lu_nop, cfg_dspu_lu_pool, 
	OBJ(int, 2), 1, 255, "DSPU LU EndAddr");
NOP	(cfg_dspu_lu_nop, cfg_dspu_lu_end, no_alt);
SET	(cfg_dspu_lu_end_set, cfg_dspu_lu_nop, OBJ(int,2), 0);

NUMBER	(cfg_dspu_lu_lu_start, cfg_dspu_lu_end_set, no_alt,
	OBJ(int, 1), 1, 255, "DSPU LU StartAddr");
NVGENS	(cfg_dspu_lu_nvgen, cfg_dspu_lu_lu_start, dspu_command, DSPU_LU);
KEYWORD (cfg_dspu_lu, cfg_dspu_lu_nvgen, cfg_dspu_dpu,
	 "lu",
	 "DSPU LU command",
	 PRIV_CONF);
/*************************************************************************
 * dspu pu <pu_name> xid <xid_num>
 *            [dlci <remote_dlci_addr>] 
 *            [sdlc <remote_sdlc_addr>] 
 *            [rmac <remote_mac>] [rsap <remote_sap>] [lsap <local_sap>]
 *            [x25  <x121_addr> qllc <qllc_subaddress>]
 *            [interface <interface-name>]
 *	      [window <window_size>] [maxiframe <maxIframe>] 
 *            [retries <retry_count>] [retry-timeout <retry_timeout>]
 * no dspu pu <pu_name> 
 *
 */
EOLS	(cfg_dspu_pu_eol, dspu_command, DSPU_PU);
pdecl   (cfg_dspu_pu_nop3);
pdecl   (cfg_dspu_pu_nop2);
pdecl   (cfg_dspu_pu_nop1);

NUMBER	(cfg_dspu_pu_retry_timeout_value, cfg_dspu_pu_nop3, no_alt, 
	OBJ(int, 14), DSPU_MIN_RETRYTIMEOUT, DSPU_MAX_RETRYTIMEOUT,
         "DSPU PU Retries Timeout Value");
KEYWORD (cfg_dspu_pu_retry_timeout, cfg_dspu_pu_retry_timeout_value, 
	cfg_dspu_pu_eol,
	 "retry-timeout","DSPU PU Retry Timeout command",PRIV_CONF);
NUMBER	(cfg_dspu_pu_retry_count, cfg_dspu_pu_nop3, no_alt, 
	OBJ(int, 13), DSPU_MIN_RETRIES, DSPU_MAX_RETRIES,
         "DSPU PU Retries Count");
KEYWORD (cfg_dspu_pu_retry, cfg_dspu_pu_retry_count, cfg_dspu_pu_retry_timeout,
	 "retries","DSPU PU Retries command",PRIV_CONF);
NUMBER	(cfg_dspu_pu_max_iframe, cfg_dspu_pu_nop3, no_alt, 
	OBJ(int, 12),  DSPU_MIN_MAXIFRAME, DSPU_MAX_MAXIFRAME,
         "DSPU PU MaxIframe");
KEYWORD (cfg_dspu_pu_maxiframe, cfg_dspu_pu_max_iframe,cfg_dspu_pu_retry,
	 "maxiframe","DSPU PU MaxIframe command",PRIV_CONF);
NUMBER	(cfg_dspu_pu_window_size, cfg_dspu_pu_nop3, no_alt, 
	OBJ(int, 11), 1, 127, "DSPU PU Window Size");
KEYWORD (cfg_dspu_pu_window, cfg_dspu_pu_window_size, cfg_dspu_pu_maxiframe,
	 "window","DSPU PU Window command",PRIV_CONF);
INTERFACE (cfg_dspu_pu_if_name, cfg_dspu_pu_nop3, no_alt,
        OBJ(idb,1), IFTYPE_IEEE | IFTYPE_SERIAL);
KEYWORD_ID (cfg_dspu_pu_if, cfg_dspu_pu_if_name, cfg_dspu_pu_window,
        OBJ(int, 10), TRUE, 
        common_str_interface, "DSPU PU Interface command", PRIV_CONF);
NOP     (cfg_dspu_pu_nop3, cfg_dspu_pu_if, no_alt);
NUMBER  (cfg_dspu_pu_local_sap, cfg_dspu_pu_nop2, no_alt,
	OBJ(int, 9), 1, 254, "DSPU PU Local SAP address");
KEYWORD (cfg_dspu_pu_lsap, cfg_dspu_pu_local_sap, cfg_dspu_pu_nop3,
	 "lsap","DSPU PU LSAP command",PRIV_CONF);
NUMBER	(cfg_dspu_pu_remote_sap, cfg_dspu_pu_nop2, no_alt, 
	OBJ(int, 8), 1, 254,  "DSPU PU Remote SAP address");
KEYWORD (cfg_dspu_pu_rsap, cfg_dspu_pu_remote_sap, cfg_dspu_pu_lsap,
	 "rsap","DSPU PU RSAP command",PRIV_CONF);
ENADDR	(cfg_dspu_pu_remote_mac, cfg_dspu_pu_nop2, no_alt, 
 	 OBJ(hwaddr, 1), "DSPU PU Remote MAC Address");
KEYWORD_ID (cfg_dspu_pu_rmac, cfg_dspu_pu_remote_mac, cfg_dspu_pu_rsap,
 	  OBJ(int,7), TRUE, "rmac","DSPU PU RMAC command",PRIV_CONF);
NOP     (cfg_dspu_pu_nop2, cfg_dspu_pu_rmac, no_alt);
STRING (cfg_dspu_pu_qllc_addr, cfg_dspu_pu_nop3, no_alt,
	OBJ(string, 3), "DSPU PU local QLLC address, up to 2 decimal digits");
KEYWORD_ID (cfg_dspu_pu_qllc, cfg_dspu_pu_qllc_addr, cfg_dspu_pu_nop3,
	    OBJ(int,15), TRUE, "qllc","DSPU PU QLLC command",PRIV_CONF);
X121ADDR(cfg_dspu_pu_x25_addr, cfg_dspu_pu_qllc, no_alt,
         OBJ(paddr,1), "DSPU PU X.121 Address");
KEYWORD_ID (cfg_dspu_pu_x25, cfg_dspu_pu_x25_addr, cfg_dspu_pu_rmac,
            OBJ(int, 6), TRUE, 
            "x25", "DSPU PU X25 command", PRIV_CONF);
HEXDIGIT(cfg_dspu_pu_sdlc_addr, cfg_dspu_pu_nop3, no_alt,
         OBJ(int,5), 0x01, 0xFE, "DSPU PU SDLC Address");
KEYWORD_ID (cfg_dspu_pu_sdlc, cfg_dspu_pu_sdlc_addr, cfg_dspu_pu_x25,
            OBJ(int, 4), TRUE, 
            "sdlc", "DSPU PU SDLC command", PRIV_CONF);
NUMBER  (cfg_dspu_pu_dlci_num, cfg_dspu_pu_nop2, no_alt,
         OBJ(int, 3), 16, 1007, "DSPU PU DLCI number");
KEYWORD_ID (cfg_dspu_pu_dlci, cfg_dspu_pu_dlci_num, cfg_dspu_pu_sdlc,
            OBJ(int, 2), TRUE, "dlci", "DSPU PU DLCI command", PRIV_CONF);
STRING	(cfg_dspu_pu_xid_num, cfg_dspu_pu_nop1, no_alt,
	 OBJ(string, 2), "DSPU PU Xid");
KEYWORD_ID (cfg_dspu_pu_xid, cfg_dspu_pu_xid_num, cfg_dspu_pu_dlci,
	    OBJ(int,1), TRUE, "xid-rcv", "DSPU PU Xid command", PRIV_CONF);
NOP     (cfg_dspu_pu_nop1, cfg_dspu_pu_xid, no_alt);
SET	(cfg_dspu_pu_rtimeout_set, cfg_dspu_pu_xid, OBJ(int,14), 30);
SET	(cfg_dspu_pu_retry_set, cfg_dspu_pu_rtimeout_set, OBJ(int,13), 4);
SET	(cfg_dspu_pu_maxIframe_set, cfg_dspu_pu_retry_set,
         OBJ(int,12), DSPU_DEFAULT_MAXIFRAME);
SET	(cfg_dspu_pu_window_set, cfg_dspu_pu_maxIframe_set,
         OBJ(int,11), DSPU_DEFAULT_WINDOW);
SET	(cfg_dspu_pu_lsap_set, cfg_dspu_pu_window_set, OBJ(int,9), 8);
SET	(cfg_dspu_pu_rsap_set, cfg_dspu_pu_lsap_set, OBJ(int,8), 4);
SET     (cfg_dspu_pu_sdlc_set, cfg_dspu_pu_rsap_set, OBJ(int,5), 0);
SET     (cfg_dspu_pu_dlci_set, cfg_dspu_pu_sdlc_set, OBJ(int,3), 0);
NOPREFIX (cfg_dspu_pu_noprefix, cfg_dspu_pu_dlci_set, cfg_dspu_pu_eol);
STRING	(cfg_dspu_pu_pu_name, cfg_dspu_pu_noprefix, no_alt, 
 	 OBJ(string, 1), "DSPU PU name");
NVGENS	(cfg_dspu_pu_nvgen, cfg_dspu_pu_pu_name, dspu_command, DSPU_PU);
KEYWORD (cfg_dspu_pu, cfg_dspu_pu_nvgen, cfg_dspu_lu,
	 "pu", "DSPU PU command", PRIV_CONF);

/*************************************************************************
 * dspu pool <pool_name> host <host_name> lu <lu_start> [<lu_end>]
 *     [inactivity-timeout <timer>]
 * no dspu pool <pool_name>
 *
 */
EOLS	(cfg_dspu_pool_eol, dspu_command, DSPU_POOL);

NUMBER	(cfg_dspu_pool_timer_val, cfg_dspu_pool_eol, no_alt, 
	OBJ(int, 3), 1, 255, "DSPU Pool inactivity timeout");
KEYWORD (cfg_dspu_pool_timer, cfg_dspu_pool_timer_val, cfg_dspu_pool_eol,
	 "inactivity-timeout","DSPU Pool inactivity timer command",PRIV_CONF);
NUMBER	(cfg_dspu_pool_lu_end, cfg_dspu_pool_timer, cfg_dspu_pool_timer, 
	OBJ(int, 2), 1, 255, "DSPU Pool Lu EndAddr");
NOP	(cfg_dspu_pool_nop, cfg_dspu_pool_lu_end, no_alt);
SET	(cfg_dspu_pool_lu_end_set, cfg_dspu_pool_nop, OBJ(int,2), 0);
NUMBER	(cfg_dspu_pool_lu_start, cfg_dspu_pool_lu_end_set, no_alt, 
	OBJ(int, 1), 1, 255, "DSPU Pool Lu StartAddr");
KEYWORD (cfg_dspu_pool_lu, cfg_dspu_pool_lu_start, no_alt,
	 "lu","DSPU Pool LU command",PRIV_CONF);
STRING	(cfg_dspu_pool_host_name, cfg_dspu_pool_lu, no_alt, 
	OBJ(string, 2), "DSPU Pool Host_Name");
KEYWORD (cfg_dspu_pool_host, cfg_dspu_pool_host_name, no_alt,
	 "host","DSPU Pool Host Name command",PRIV_CONF);
NOPREFIX (cfg_dspu_pool_noprefix, cfg_dspu_pool_host,cfg_dspu_pool_eol );
STRING	(cfg_dspu_pool_name, cfg_dspu_pool_noprefix, no_alt, 
	OBJ(string, 1), "DSPU Pool_Name");
SET	(cfg_dspu_pool_timer_set, cfg_dspu_pool_name, OBJ(int,3), 0);
NVGENS	(cfg_dspu_pool_nvgen, cfg_dspu_pool_timer_set, dspu_command, DSPU_POOL);
KEYWORD (cfg_dspu_pool, cfg_dspu_pool_nvgen, cfg_dspu_pu,
	 "pool",
	 "DSPU Pool command",
	 PRIV_CONF);

/*************************************************************************
 * dspu <notification-level>
 * no dspu <notification-level>
 *
 * OBJ(int,1) to DSPU_NOTIFY_LEVEL_OFF     if off
 * OBJ(int,1) to DSPU_NOTIFY_LEVEL_LOW     if low
 * OBJ(int,1) to DSPU_NOTIFY_LEVEL_MEDIUM  if medium
 * OBJ(int,1) to DSPU_NOTIFY_LEVEL_HI      if high
 */
EOLS	   (cfg_dspu_notification_eol, dspu_command, DSPU_NOTIFICATION);
KEYWORD_ID (cfg_dspu_notification_hi, cfg_dspu_notification_eol, no_alt,
            OBJ(int,1), DSPU_NOTIFY_LEVEL_HI, "high", 
            "Downstream PU and LU state changes + activation fails", 
            PRIV_CONF);
KEYWORD_ID (cfg_dspu_notification_med, cfg_dspu_notification_eol,
            cfg_dspu_notification_hi, OBJ(int,1), DSPU_NOTIFY_LEVEL_MEDIUM,
            "medium", "Downstream PU state changes + activation fails", 
            PRIV_CONF);
KEYWORD_ID (cfg_dspu_notification_low, cfg_dspu_notification_eol,
            cfg_dspu_notification_med, OBJ(int,1), DSPU_NOTIFY_LEVEL_LOW,
            "low", "PU and LU activation fails only", PRIV_CONF);
KEYWORD_ID (cfg_dspu_notification_off, cfg_dspu_notification_eol,
            cfg_dspu_notification_low, OBJ(int,1), DSPU_NOTIFY_LEVEL_OFF,
            "off", "No SNMP Traps or SNA Messages to Operator for DSPU", 
            PRIV_CONF);
NOPREFIX   (cfg_dspu_notification_noprefix, cfg_dspu_notification_off,
	    cfg_dspu_notification_eol);
NVGENS	   (cfg_dspu_notification_nvgen, cfg_dspu_notification_noprefix,
            dspu_command, DSPU_NOTIFICATION);
KEYWORD    (cfg_dspu_notification, cfg_dspu_notification_nvgen, cfg_dspu_pool,
	    "notification-level",
	    "DSPU command for controlling DSPU Notifications",
	    PRIV_CONF);

/*************************************************************************
 * dspu <activation-window>
 * no dspu <activation-window>
 *
 */
EOLS	(cfg_dspu_actwindow_eol, dspu_command, DSPU_ACTWINDOW);
NUMBER  (cfg_dspu_actwindow_size, cfg_dspu_actwindow_eol, no_alt,
	OBJ(int, 1), 1, 255, "DSPU Actwindow Size");
SET	(cfg_dspu_actwindow_set, cfg_dspu_actwindow_size, OBJ(int,1), 10);
NOPREFIX (cfg_dspu_actwindow_noprefix, cfg_dspu_actwindow_set, 
	cfg_dspu_actwindow_eol);
NVGENS	(cfg_dspu_actwindow_nvgen, cfg_dspu_actwindow_noprefix, dspu_command,
		DSPU_ACTWINDOW);
KEYWORD (cfg_dspu_actwindow, cfg_dspu_actwindow_nvgen, 
         cfg_dspu_notification, "activation-window",
         "DSPU activation-window command", PRIV_CONF);

/*************************************************************************
 * dspu host <host_name> xid <xid_num> 
 *              [dlci <remote_dlci_addr>] 
 *              [sdlc <remote_sdlc_addr>] 
 *              [rmac <remote_mac>] [rsap <remote_sap>] [lsap <local_sap>]
 *              [x25  <x121_addr> qllc <qllc_subaddress>]
 *              [interface <interface-name>]
 *		[focalpoint] [window <window_size>] [maxiframe <maxIframe>]
 *		[retries <retry_count>] [retry_timeout <retry_timeout>]
 * no dspu host <host_name> 
 *
 */
EOLS	(cfg_dspu_host_eol, dspu_command, DSPU_HOST);
pdecl   (cfg_dspu_host_nop1);
pdecl   (cfg_dspu_host_nop2);

KEYWORD_ID (cfg_dspu_host_focalpoint, cfg_dspu_host_nop2,
            cfg_dspu_host_eol, OBJ(int, 14), TRUE, 
	    "focalpoint", "DSPU Host Focalpoint",PRIV_CONF);
NUMBER	(cfg_dspu_host_retry_timeout_value, cfg_dspu_host_nop2, no_alt, 
 	 OBJ(int, 13), DSPU_MIN_RETRYTIMEOUT, DSPU_MAX_RETRYTIMEOUT,
         "DSPU Host Retries Timeout value");
KEYWORD (cfg_dspu_host_retry_timeout, cfg_dspu_host_retry_timeout_value, 
	 cfg_dspu_host_focalpoint,
	 "retry-timeout", "DSPU Host Retry Timeout command", PRIV_CONF);
NUMBER	(cfg_dspu_host_retry_count, cfg_dspu_host_nop2, no_alt, 
	 OBJ(int, 12), DSPU_MIN_RETRIES, DSPU_MAX_RETRIES,
         "DSPU Host Retries count");
KEYWORD (cfg_dspu_host_retry, cfg_dspu_host_retry_count, 
	cfg_dspu_host_retry_timeout,
         "retries", "DSPU Host Retries command", PRIV_CONF);
NUMBER	(cfg_dspu_host_max_iframe, cfg_dspu_host_nop2, no_alt, 
	 OBJ(int, 11), DSPU_MIN_MAXIFRAME, DSPU_MAX_MAXIFRAME,
         "DSPU Host MaxIframe");
KEYWORD (cfg_dspu_host_maxiframe, cfg_dspu_host_max_iframe, 
         cfg_dspu_host_retry,
	 "maxiframe", "DSPU Host MaxIframe command", PRIV_CONF);
NUMBER	(cfg_dspu_host_window_size, cfg_dspu_host_nop2, no_alt, 
	 OBJ(int, 10), 1, 127, "DSPU Host Window size");
KEYWORD (cfg_dspu_host_window, cfg_dspu_host_window_size, 
         cfg_dspu_host_maxiframe, 
         "window", "DSPU Host Window command", PRIV_CONF);
INTERFACE (cfg_dspu_host_if_name, cfg_dspu_host_nop2, no_alt,
        OBJ(idb,1), IFTYPE_IEEE | IFTYPE_SERIAL);
KEYWORD_ID (cfg_dspu_host_if, cfg_dspu_host_if_name, cfg_dspu_host_window,
        OBJ(int, 9), TRUE, 
        common_str_interface, "DSPU HOST Interface command", PRIV_CONF);
NOP(cfg_dspu_host_nop2, cfg_dspu_host_if, no_alt);
NUMBER	(cfg_dspu_host_local_sap, cfg_dspu_host_nop1, no_alt, 
	 OBJ(int, 8), 1, 254, "DSPU Host Local SAP address");
KEYWORD (cfg_dspu_host_lsap, cfg_dspu_host_local_sap, cfg_dspu_host_nop2,
	 "lsap", "DSPU Host Local SAP command", PRIV_CONF);
NUMBER	(cfg_dspu_host_remote_sap, cfg_dspu_host_nop1, no_alt, 
	 OBJ(int, 7), 1, 254, "DSPU Host Remote SAP address");
KEYWORD (cfg_dspu_host_rsap, cfg_dspu_host_remote_sap, cfg_dspu_host_lsap,
  	 "rsap", "DSPU Host Remote SAP command", PRIV_CONF);
NOP(cfg_dspu_host_nop1, cfg_dspu_host_rsap, no_alt);
ENADDR  (cfg_dspu_host_remote_mac, cfg_dspu_host_nop1, no_alt,
         OBJ(hwaddr, 1), "DSPU Host Remote MAC address");
KEYWORD_ID (cfg_dspu_host_rmac, cfg_dspu_host_remote_mac, no_alt,
            OBJ(int,6), TRUE,
            "rmac", "DSPU Host RMAC command", PRIV_CONF);
STRING (cfg_dspu_host_qllc_addr, cfg_dspu_host_nop2, no_alt,
	OBJ(string, 3), "DSPU HOST Local QLLC address, up to 2 decimal digits");
KEYWORD_ID (cfg_dspu_host_qllc, cfg_dspu_host_qllc_addr, cfg_dspu_host_nop2,
            OBJ(int, 15), TRUE, "qllc","DSPU HOST QLLC command",PRIV_CONF);
X121ADDR(cfg_dspu_host_x25_addr, cfg_dspu_host_qllc, no_alt,
         OBJ(paddr,1), "DSPU Host X.121 Address");
KEYWORD_ID (cfg_dspu_host_x25, cfg_dspu_host_x25_addr, cfg_dspu_host_rmac,
            OBJ(int, 5), TRUE,
            "x25", "DSPU Host X25 command", PRIV_CONF);
HEXDIGIT(cfg_dspu_host_sdlc_addr, cfg_dspu_host_nop2, no_alt,
         OBJ(int,4), 0x01, 0xFE, "DSPU Host SDLC address");
KEYWORD_ID (cfg_dspu_host_sdlc, cfg_dspu_host_sdlc_addr, 
            cfg_dspu_host_x25, OBJ(int,3), TRUE,
            "sdlc", "DSPU Host SDLC command", PRIV_CONF);
NUMBER	(cfg_dspu_host_dlci_num, cfg_dspu_host_nop1, no_alt, 
	 OBJ(int, 2), 16, 1007, "DSPU Host DLCI number");
KEYWORD_ID (cfg_dspu_host_dlci, cfg_dspu_host_dlci_num, 
            cfg_dspu_host_sdlc, OBJ(int,1), TRUE,
            "dlci", "DSPU Host DLCI command", PRIV_CONF);
SET	(cfg_dspu_host_rtimeout_set, cfg_dspu_host_dlci, OBJ(int,13), 30);
SET	(cfg_dspu_host_retry_set, cfg_dspu_host_rtimeout_set, OBJ(int,12), -1);
SET	(cfg_dspu_host_maxIframe_set, 
         cfg_dspu_host_retry_set, OBJ(int,11), DSPU_DEFAULT_MAXIFRAME);
SET	(cfg_dspu_host_window_set, cfg_dspu_host_maxIframe_set,
         OBJ(int,10), DSPU_DEFAULT_WINDOW);
SET	(cfg_dspu_host_lsap_set, cfg_dspu_host_window_set, OBJ(int,8), 12);
SET     (cfg_dspu_host_rsap_set, cfg_dspu_host_lsap_set, OBJ(int,7), 4);
SET     (cfg_dspu_host_sdlc_set, cfg_dspu_host_rsap_set, OBJ(int,4), 0);
SET     (cfg_dspu_host_dlci_set, cfg_dspu_host_sdlc_set, OBJ(int,2), 0);
STRING  (cfg_dspu_host_xid_num, cfg_dspu_host_dlci_set, no_alt,
         OBJ(string, 2), "DSPU Host Xid");
KEYWORD (cfg_dspu_host_xid, cfg_dspu_host_xid_num, cfg_dspu_host_dlci_set,
	 "xid-snd", "DSPU Host Xid command", PRIV_CONF);
NOPREFIX (cfg_dspu_host_noprefix, cfg_dspu_host_xid, cfg_dspu_host_eol);
STRING	(cfg_dspu_host_name, cfg_dspu_host_noprefix, no_alt, 
 	 OBJ(string, 1), "DSPU Host name");
NVGENS	(cfg_dspu_host_nvgen, cfg_dspu_host_name, dspu_command, DSPU_HOST);
KEYWORD (cfg_dspu_host, cfg_dspu_host_nvgen, cfg_dspu_actwindow,
	 "host", "DSPU Host command", PRIV_CONF);

/*************************************************************************
 * dspu rsrb <local_ring> <bridge_number> <target_ring> <virtual_macaddr> 
 * dspu rsrb enable-host [lsap <lsap>]
 * dspu rsrb enable-pu [lsap <lsap>]
 * dspu rsrb start <name>
 * no dspu rsrb 
 *
 */
EOLS	(cfg_dspu_rsrb_eol, dspu_command, DSPU_RSRB);

ENADDR	(cfg_dspu_rsrb_virtual_macaddr, cfg_dspu_rsrb_eol, no_alt,
        OBJ(hwaddr, 1), "DSPU RSRB Virtual Macaddr");
NUMBER  (cfg_dspu_rsrb_target_ring, cfg_dspu_rsrb_virtual_macaddr, no_alt,
        OBJ(int, 3), 1, DSPU_MAX_RING_NUM, "DSPU RSRB Target Ring");
NUMBER  (cfg_dspu_rsrb_bridge_num, cfg_dspu_rsrb_target_ring, no_alt,
        OBJ(int, 2), 1, DSPU_MAX_BRIDGE_NUM, "DSPU RSRB Bridge Number");
NOPREFIX(cfg_dspu_rsrb_noprefix, cfg_dspu_rsrb_bridge_num, cfg_dspu_rsrb_eol);
NUMBER  (cfg_dspu_rsrb_local_ring, cfg_dspu_rsrb_noprefix, no_alt,
        OBJ(int, 1), 1, DSPU_MAX_RING_NUM, "DSPU RSRB Local Ring");
NUMBER  (cfg_dspu_rsrb_lsap_num,cfg_dspu_rsrb_eol, no_alt,
        OBJ(int, 8), 1, 254, "DSPU RSRB lsap number");
KEYWORD (cfg_dspu_rsrb_lsap, cfg_dspu_rsrb_lsap_num,cfg_dspu_rsrb_eol,
	 "lsap","DSPU RSRB lsap command",PRIV_CONF);
SET     (cfg_dspu_rsrb_set_pusap, cfg_dspu_rsrb_lsap, OBJ(int,8), 8);
KEYWORD_ID  (cfg_dspu_rsrb_enable_pu, cfg_dspu_rsrb_set_pusap,
        cfg_dspu_rsrb_local_ring, OBJ(int, 7), TRUE, "enable-pu",
        "DSPU RSRB enable-pu",PRIV_CONF);
SET     (cfg_dspu_rsrb_set_hostsap, cfg_dspu_rsrb_lsap, OBJ(int,8), 12);
KEYWORD_ID  (cfg_dspu_rsrb_enable_host, cfg_dspu_rsrb_set_hostsap,
        cfg_dspu_rsrb_enable_pu, OBJ(int, 6), TRUE, "enable-host",
        "DSPU RSRB enable-host",PRIV_CONF);
STRING	(cfg_dspu_rsrb_start_name, cfg_dspu_rsrb_eol, no_alt, 
	OBJ(string, 1), "DSPU Host_Name");
KEYWORD_ID  (cfg_dspu_rsrb_start, cfg_dspu_rsrb_start_name,
        cfg_dspu_rsrb_enable_host, OBJ(int, 5), TRUE, "start",
        "DSPU RSRB start",PRIV_CONF);
NVGENS	(cfg_dspu_rsrb_nvgen, cfg_dspu_rsrb_start, dspu_command, DSPU_RSRB);
KEYWORD (cfg_dspu_rsrb, cfg_dspu_rsrb_nvgen, cfg_dspu_host,
	 "rsrb",
	 "DSPU Remote Source Route Bridge command",
	 PRIV_CONF);

/*************************************************************************
 * dspu vdlc <ringNumber> <virtual_macaddr> 
 * dspu vdlc enable-host [lsap <lsap>]
 * dspu vdlc enable-pu [lsap <lsap>]
 * dspu vdlc start <name>
 * no dspu vdlc 
 *
 */
EOLS	(cfg_dspu_vdlc_eol, dspu_command, DSPU_VDLC);

ENADDR	(cfg_dspu_vdlc_virtual_macaddr, cfg_dspu_vdlc_eol, no_alt,
         OBJ(hwaddr, 1), "DSPU VDLC Virtual Macaddr");
NOPREFIX(cfg_dspu_vdlc_noprefix, cfg_dspu_vdlc_virtual_macaddr, 
         cfg_dspu_vdlc_eol);
NUMBER  (cfg_dspu_vdlc_ring_num, cfg_dspu_vdlc_noprefix, no_alt,
         OBJ(int, 1), 1, DSPU_MAX_RING_NUM, "DSPU VDLC ring number");
NUMBER  (cfg_dspu_vdlc_lsap_num,cfg_dspu_vdlc_eol, no_alt,
         OBJ(int, 8), 1, 254, "DSPU VDLC lsap number");
KEYWORD (cfg_dspu_vdlc_lsap, cfg_dspu_vdlc_lsap_num,cfg_dspu_vdlc_eol,
	 "lsap","DSPU VDLC lsap command",PRIV_CONF);
SET     (cfg_dspu_vdlc_set_pusap, cfg_dspu_vdlc_lsap, OBJ(int,8), 8);
KEYWORD_ID (cfg_dspu_vdlc_enable_pu, cfg_dspu_vdlc_set_pusap,
         cfg_dspu_vdlc_ring_num, OBJ(int, 7), TRUE, "enable-pu",
         "DSPU VDLC enable-pu",PRIV_CONF);
SET     (cfg_dspu_vdlc_set_hostsap, cfg_dspu_vdlc_lsap, OBJ(int,8), 12);
KEYWORD_ID (cfg_dspu_vdlc_enable_host, cfg_dspu_vdlc_set_hostsap,
         cfg_dspu_vdlc_enable_pu, OBJ(int, 6), TRUE, "enable-host",
         "DSPU VDLC enable-host",PRIV_CONF);
STRING	(cfg_dspu_vdlc_start_name, cfg_dspu_vdlc_eol, no_alt, 
	 OBJ(string, 1), "DSPU Host_Name");
KEYWORD_ID  (cfg_dspu_vdlc_start, cfg_dspu_vdlc_start_name,
         cfg_dspu_vdlc_enable_host, OBJ(int, 5), TRUE, "start",
         "DSPU VDLC start",PRIV_CONF);
NVGENS	(cfg_dspu_vdlc_nvgen, cfg_dspu_vdlc_start, dspu_command, DSPU_VDLC);
KEYWORD (cfg_dspu_vdlc, cfg_dspu_vdlc_nvgen, cfg_dspu_rsrb,
	 "vdlc",
	 "DSPU Virtual DLC command",
	 PRIV_CONF);


/*************************************************************************
 * dspu ncia [server_num]
 * dspu ncia enable-pu [lsap <lsap>]
 * no dspu ncia 
 */
EOLS	(cfg_dspu_ncia_eol, dspu_command, DSPU_NCIA);

NUMBER  (cfg_dspu_ncia_server_num, cfg_dspu_ncia_eol, cfg_dspu_ncia_eol,
         OBJ(int, 3), 1, 1, "NCIA server number");

SET     (cfg_dspu_ncia_set_server_num, cfg_dspu_ncia_server_num, OBJ(int,3), 1);

NUMBER  (cfg_dspu_ncia_lsap_num, cfg_dspu_ncia_eol, no_alt,
         OBJ(int, 2), 4, 252, "DSPU NCIA lsap number");

KEYWORD (cfg_dspu_ncia_lsap, cfg_dspu_ncia_lsap_num, cfg_dspu_ncia_eol,
	 "lsap", "DSPU NCIA lsap command", PRIV_CONF);

SET     (cfg_dspu_ncia_set_pusap, cfg_dspu_ncia_lsap, OBJ(int,2), 8);

KEYWORD_ID (cfg_dspu_ncia_enable_pu, cfg_dspu_ncia_set_pusap,
         cfg_dspu_ncia_set_server_num, OBJ(int, 1), TRUE, "enable-pu",
         "DSPU NCIA enable-pu", PRIV_CONF);

NVGENS	(cfg_dspu_ncia_nvgen, cfg_dspu_ncia_enable_pu, dspu_command, DSPU_NCIA);

KEYWORD (cfg_dspu_ncia, cfg_dspu_ncia_nvgen, cfg_dspu_vdlc,
	 "ncia",
	 "DSPU NCIA command",
	 PRIV_CONF);



/*************************************************************************
*/
KEYWORD	(cfg_dspu, cfg_dspu_ncia, NONE,
	 "dspu", "DownStream Physical Unit Command", PRIV_CONF);

TEST_BOOLEAN(cfg_dspu_test, cfg_dspu, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE cfg_dspu_test

