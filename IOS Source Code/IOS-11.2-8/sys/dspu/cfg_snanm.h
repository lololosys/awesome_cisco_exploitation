/* $Id: cfg_snanm.h,v 3.3.12.7 1996/09/13 08:31:36 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/cfg_snanm.h,v $
 *------------------------------------------------------------------
 * SNA Network Management  Configuration
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_snanm.h,v $
 * Revision 3.3.12.7  1996/09/13  08:31:36  lcox
 * CSCdi61967:  dspu - retries & retry-timeout dont work like doc says
 * Branch: California_branch
 *   retry-timeout range changed in code to agree with user
 *   documentation  (was 0..600, is now 1..600
 *
 * Revision 3.3.12.6  1996/08/09  01:53:58  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.3.12.5  1996/08/07  06:35:13  szdravko
 * CSCdi61970:  dspu - range of window parameter is incorrect
 * Branch: California_branch
 *
 * Revision 3.3.12.4  1996/07/24  01:23:32  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.3.12.3  1996/07/15  15:38:07  ppearce
 * CSCdi62916:  FDDI/FastEthernet i/f not supported on dspu host/pu command
 * Branch: California_branch
 *
 * Revision 3.3.12.2  1996/05/21  09:46:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.12.1  1996/05/17  10:47:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.1  1996/03/28  08:20:16  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3  1996/02/17  09:17:02  ppearce
 * CSCdi46728:  DSPU HOST can not be configured without XID parameter
 *
 * Revision 3.2.4.1  1996/01/20  06:08:30  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/17  09:04:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/11  04:20:46  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.3  1995/08/15  14:58:28  ppearce
 * CSCdi38786:  sna host configuration command is lost after reload
 *   "sna host" default parameter values set incorrectly
 *
 * Revision 2.2  1995/07/06  23:15:35  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/07/04  01:53:04  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.4  1995/06/13  06:52:20  bchan
 * CSCdi34759:  SNA parser bad spelling
 *
 * Revision 2.3  1995/06/11  20:38:43  dkatz
 * CSCdi35669:  SNA endnode command conflicts with end command
 * Require at least four characters to match "endnode".
 *
 * Revision 2.2  1995/06/09  14:15:02  widmer
 * CSCdi35304:  Parser help at second level broken in config.
 * Change bogus no_alt to NONE
 *
 * Revision 2.1  1995/06/07  22:13:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*************************************************************************
 * sna host <host_name> xid <xid_num>
 *              [dlci <remote_dlci_addr>]
 *              [sdlc <remote_sdlc_addr>]
 *              [rmac <remote_mac>] [rsap <remote_sap>] [lsap <local_sap>]
 *              [x25  <x121_addr> qllc <qllc_subaddress>]
 *              [interface <interface-name>]
 *		[focalpoint] [window <window_size>] [maxiframe <max_iframe>]
 *		[retries <retry_count>] [retry_timeout <retry_timeout>]
 * no sna host <host_name> 
 *
 */
EOLS	(cfg_sna_host_eol, sna_command, SNA_HOST);
pdecl   (cfg_sna_host_nop1);
pdecl   (cfg_sna_host_nop2);
 
KEYWORD_ID (cfg_sna_host_focalpoint, cfg_sna_host_nop2,
            cfg_sna_host_eol, OBJ(int, 14), TRUE,
            "focalpoint", "SNA Host Focalpoint",PRIV_CONF);
NUMBER  (cfg_sna_host_retry_timeout_value, cfg_sna_host_nop2, no_alt,
         OBJ(int, 13), DSPU_MIN_RETRYTIMEOUT, DSPU_MAX_RETRYTIMEOUT,
         "SNA Host Retries Timeout value");
KEYWORD (cfg_sna_host_retry_timeout, cfg_sna_host_retry_timeout_value,
         cfg_sna_host_focalpoint,
         "retry-timeout", "SNA Host Retry Timeout command", PRIV_CONF);
NUMBER  (cfg_sna_host_retry_count, cfg_sna_host_nop2, no_alt,
         OBJ(int, 12), DSPU_MIN_RETRIES, DSPU_MAX_RETRIES,
         "SNA Host Retries count");
KEYWORD (cfg_sna_host_retry, cfg_sna_host_retry_count,
        cfg_sna_host_retry_timeout,
         "retries", "SNA Host Retries command", PRIV_CONF);
NUMBER  (cfg_sna_host_max_iframe, cfg_sna_host_nop2, no_alt,
         OBJ(int, 11), DSPU_MIN_MAXIFRAME, DSPU_MAX_MAXIFRAME,
         "SNA Host MaxIframe");
KEYWORD (cfg_sna_host_maxiframe, cfg_sna_host_max_iframe,
         cfg_sna_host_retry,
         "maxiframe", "SNA Host MaxIframe command", PRIV_CONF);
NUMBER  (cfg_sna_host_window_size, cfg_sna_host_nop2, no_alt,
         OBJ(int, 10), 1, 127, "SNA Host Window size");
KEYWORD (cfg_sna_host_window, cfg_sna_host_window_size,
         cfg_sna_host_maxiframe,
         "window", "SNA Host Window command", PRIV_CONF);
INTERFACE (cfg_sna_host_if_name, cfg_sna_host_nop2, no_alt,
        OBJ(idb,1), IFTYPE_IEEE | IFTYPE_SERIAL);
KEYWORD_ID (cfg_sna_host_if, cfg_sna_host_if_name, cfg_sna_host_window,
        OBJ(int, 9), TRUE,
        common_str_interface, "SNA HOST Interface command", PRIV_CONF);
NOP(cfg_sna_host_nop2, cfg_sna_host_if, no_alt);
NUMBER  (cfg_sna_host_local_sap, cfg_sna_host_nop1, no_alt,
         OBJ(int, 8), 1, 254, "SNA Host Local SAP address");
KEYWORD (cfg_sna_host_lsap, cfg_sna_host_local_sap, cfg_sna_host_if,
         "lsap", "SNA Host Local SAP command", PRIV_CONF);
NUMBER  (cfg_sna_host_remote_sap, cfg_sna_host_nop1, no_alt,
         OBJ(int, 7), 1, 254, "SNA Host Remote SAP address");
KEYWORD (cfg_sna_host_rsap, cfg_sna_host_remote_sap, cfg_sna_host_lsap,
         "rsap", "SNA Host Remote SAP command", PRIV_CONF);
NOP(cfg_sna_host_nop1, cfg_sna_host_rsap, no_alt);
ENADDR  (cfg_sna_host_remote_mac, cfg_sna_host_nop1, no_alt,
         OBJ(hwaddr, 1), "SNA Host Remote MAC address");
KEYWORD_ID (cfg_sna_host_rmac, cfg_sna_host_remote_mac, no_alt,
            OBJ(int,6), TRUE,
            "rmac", "SNA Host RMAC command", PRIV_CONF);
STRING (cfg_sna_host_qllc_addr, cfg_sna_host_nop2, no_alt,
        OBJ(string, 3), "SNA HOST Local QLLC subaddress, up to 2 decimal digits");
KEYWORD (cfg_sna_host_qllc, cfg_sna_host_qllc_addr, cfg_sna_host_nop2,
         "qllc","SNA HOST QLLC command",PRIV_CONF);
X121ADDR(cfg_sna_host_x25_addr, cfg_sna_host_qllc, no_alt,
         OBJ(paddr,1), "SNA Host X.121 Address");
KEYWORD_ID (cfg_sna_host_x25, cfg_sna_host_x25_addr, cfg_sna_host_rmac,
            OBJ(int, 5), TRUE,
            "x25", "SNA Host X25 command", PRIV_CONF);
HEXDIGIT(cfg_sna_host_sdlc_addr, cfg_sna_host_nop2, no_alt,
         OBJ(int,4), 0x01, 0xFE, "SNA Host SDLC address");
KEYWORD_ID (cfg_sna_host_sdlc, cfg_sna_host_sdlc_addr,
            cfg_sna_host_x25, OBJ(int,3), TRUE,
            "sdlc", "SNA Host SDLC command", PRIV_CONF);
NUMBER  (cfg_sna_host_dlci_num, cfg_sna_host_nop1, no_alt,
         OBJ(int, 2), 16, 1007, "SNA Host DLCI number");
KEYWORD_ID (cfg_sna_host_dlci, cfg_sna_host_dlci_num,
            cfg_sna_host_sdlc, OBJ(int,1), TRUE,
            "dlci", "SNA Host DLCI command", PRIV_CONF);
SET     (cfg_sna_host_rtimeout_set, cfg_sna_host_dlci, OBJ(int,13), 30);
SET     (cfg_sna_host_retry_set, cfg_sna_host_rtimeout_set, OBJ(int,12), -1);
SET     (cfg_sna_host_maxIframe_set,
         cfg_sna_host_retry_set, OBJ(int,11), DSPU_DEFAULT_MAXIFRAME);
SET     (cfg_sna_host_window_set, cfg_sna_host_maxIframe_set,
         OBJ(int,10), DSPU_DEFAULT_WINDOW);
SET     (cfg_sna_host_lsap_set, cfg_sna_host_window_set, OBJ(int,8), 12);
SET     (cfg_sna_host_rsap_set, cfg_sna_host_lsap_set, OBJ(int,7), 4);
SET     (cfg_sna_host_sdlc_set, cfg_sna_host_rsap_set, OBJ(int,4), 0);
SET     (cfg_sna_host_dlci_set, cfg_sna_host_sdlc_set, OBJ(int,2), 0);
STRING  (cfg_sna_host_xid_num, cfg_sna_host_dlci_set, no_alt,
         OBJ(string, 2), "SNA Host Xid");
KEYWORD (cfg_sna_host_xid, cfg_sna_host_xid_num, cfg_sna_host_dlci_set,
         "xid-snd", "SNA Host Xid command", PRIV_CONF);
NOPREFIX (cfg_sna_host_noprefix, cfg_sna_host_xid, cfg_sna_host_eol);
STRING	(cfg_sna_host_name, cfg_sna_host_noprefix, no_alt, 
 	 OBJ(string, 1), "SNA Host name");
NVGENS	(cfg_sna_host_nvgen, cfg_sna_host_name, sna_command, SNA_HOST);
KEYWORD (cfg_sna_host, cfg_sna_host_nvgen, no_alt,
	 "host", "SNA Host command", PRIV_CONF);

/*************************************************************************
 * sna rsrb <local_ring> <bridge_number> <target_ring> <virtual_macaddr>
 * sna rsrb enable-host [lsap <lsap>]
 * sna rsrb enable-pu [lsap <lsap>]
 * sna rsrb start <name>
 * no sna rsrb 
 *
 */
EOLS	(cfg_sna_rsrb_eol, sna_command, SNA_RSRB);
pdecl (cfg_sna_rsrb_nop);

ENADDR	(cfg_sna_rsrb_virtual_macaddr, cfg_sna_rsrb_eol, no_alt,
        OBJ(hwaddr, 1), "SNA RSRB Virtual Macaddr");
NUMBER  (cfg_sna_rsrb_target_ring, cfg_sna_rsrb_virtual_macaddr, no_alt,
        OBJ(int, 3), 1, DSPU_MAX_RING_NUM, "SNA RSRB Target Ring");
NUMBER  (cfg_sna_rsrb_bridge_num, cfg_sna_rsrb_target_ring, no_alt,
        OBJ(int, 2), 1, DSPU_MAX_BRIDGE_NUM, "SNA RSRB Bridge Number");
NOPREFIX(cfg_sna_rsrb_noprefix, cfg_sna_rsrb_bridge_num, cfg_sna_rsrb_eol);
NUMBER  (cfg_sna_rsrb_local_ring, cfg_sna_rsrb_noprefix, no_alt,
        OBJ(int, 1), 1, DSPU_MAX_RING_NUM, "SNA RSRB Local Ring");
NUMBER  (cfg_sna_rsrb_lsap_num,cfg_sna_rsrb_eol, no_alt,
        OBJ(int, 8), 1, 254, "SNA RSRB lsap number");
KEYWORD (cfg_sna_rsrb_lsap, cfg_sna_rsrb_lsap_num,cfg_sna_rsrb_eol,
	 "lsap","SNA RSRB lsap command",PRIV_CONF);
SET     (cfg_sna_rsrb_set_hostsap, cfg_sna_rsrb_lsap, OBJ(int,8), 12);
KEYWORD_ID  (cfg_sna_rsrb_enable_host, cfg_sna_rsrb_set_hostsap,
        cfg_sna_rsrb_local_ring, OBJ(int, 6), TRUE, "enable-host",
        "SNA RSRB enable-host",PRIV_CONF);
STRING	(cfg_sna_rsrb_start_name, cfg_sna_rsrb_eol, no_alt, 
	OBJ(string, 1), "SNA Host_Name");
KEYWORD_ID  (cfg_sna_rsrb_start, cfg_sna_rsrb_start_name,
        cfg_sna_rsrb_enable_host, OBJ(int, 5), TRUE, "start",
        "SNA RSRB start",PRIV_CONF);
NVGENS	(cfg_sna_rsrb_nvgen, cfg_sna_rsrb_start, sna_command, SNA_RSRB);
KEYWORD (cfg_sna_rsrb, cfg_sna_rsrb_nvgen, cfg_sna_host,
	 "rsrb",
	 "SNA Remote Source Route Bridge command",
	 PRIV_CONF);

/*************************************************************************
 * sna vdlc <ringNumber> <virtual_macaddr>
 * sna vdlc enable-host [lsap <lsap>]
 * sna vdlc start <name>
 * no sna vdlc 
 *
 */
EOLS    (cfg_sna_vdlc_eol, sna_command, SNA_VDLC);
pdecl   (cfg_sna_vdlc_nop);

ENADDR	(cfg_sna_vdlc_virtual_macaddr, cfg_sna_vdlc_eol, no_alt,
         OBJ(hwaddr, 1), "SNA VDLC Virtual Macaddr");
NOPREFIX(cfg_sna_vdlc_noprefix, cfg_sna_vdlc_virtual_macaddr, 
         cfg_sna_vdlc_eol);
NUMBER  (cfg_sna_vdlc_ring_num, cfg_sna_vdlc_noprefix, no_alt,
         OBJ(int, 1), 1, DSPU_MAX_RING_NUM, "SNA VDLC ring number");
NUMBER  (cfg_sna_vdlc_lsap_num,cfg_sna_vdlc_eol, no_alt,
         OBJ(int, 8), 1, 254, "SNA VDLC lsap number");
KEYWORD (cfg_sna_vdlc_lsap, cfg_sna_vdlc_lsap_num,cfg_sna_vdlc_eol,
	 "lsap","SNA VDLC lsap command",PRIV_CONF);
SET     (cfg_sna_vdlc_set_hostsap, cfg_sna_vdlc_lsap, OBJ(int,8), 12);
KEYWORD_ID (cfg_sna_vdlc_enable_host, cfg_sna_vdlc_set_hostsap,
         cfg_sna_vdlc_ring_num, OBJ(int, 6), TRUE, "enable-host",
         "SNA VDLC enable-host",PRIV_CONF);
STRING	(cfg_sna_vdlc_start_name, cfg_sna_vdlc_eol, no_alt, 
 	 OBJ(string, 1), "SNA Host_Name");
KEYWORD_ID (cfg_sna_vdlc_start, cfg_sna_vdlc_start_name,
         cfg_sna_vdlc_enable_host, OBJ(int, 5), TRUE, "start",
         "SNA VDLC start",PRIV_CONF);
NVGENS	(cfg_sna_vdlc_nvgen, cfg_sna_vdlc_start, sna_command, SNA_VDLC);
KEYWORD (cfg_sna_vdlc, cfg_sna_vdlc_nvgen, cfg_sna_rsrb,
	 "vdlc", "SNA Virtual DLC command", PRIV_CONF);

EOLS   (cfg_sna_endnode_eol, sna_command, SNA_ENDNODE);
STRING (cfg_sna_cpname, cfg_sna_endnode_eol,no_alt,
	OBJ(string, 2), "network id");
STRING (cfg_sna_netid, cfg_sna_cpname,no_alt,
	OBJ(string, 1), "network id");
NVGENS(cfg_sna_endnode_nvgen, cfg_sna_netid, sna_command, SNA_ENDNODE);

EOLS   (cfg_location_eol, sna_command, SNA_LOCATION);
TEXT   (cfg_location_text, cfg_location_eol, no_alt,
	 OBJ(string, 1), "Up to 50 characters describing router location");
NOPREFIX (cfg_location_noprefix, cfg_location_text, cfg_location_eol);
NVGENS	(cfg_location_nvgen, cfg_location_noprefix, sna_command, SNA_LOCATION);

/*************************************************************************
*/
KEYWORD_MM (cfg_sna_endnode, cfg_sna_endnode_nvgen, NONE,
	 "endnode", "SNA APPN endnode command", PRIV_CONF, 4);

/*************************************************************************
*/
KEYWORD	(cfg_location, cfg_location_nvgen, cfg_sna_endnode,
	 "location", "Network Management Router location Command", PRIV_CONF);

/*************************************************************************
*/
KEYWORD	(cfg_sna, cfg_sna_vdlc, cfg_location,
	 "sna", "Network Management Physical Unit Command", PRIV_CONF);

TEST_BOOLEAN(cfg_sna_test, cfg_sna, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE cfg_sna_test

