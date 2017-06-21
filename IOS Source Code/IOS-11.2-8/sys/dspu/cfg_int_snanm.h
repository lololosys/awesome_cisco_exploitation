/* $Id: cfg_int_snanm.h,v 3.2 1995/11/17 09:04:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/cfg_int_snanm.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Interface Configuration
  *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_snanm.h,v $
 * Revision 3.2  1995/11/17  09:04:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  21:12:52  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/07/04  01:53:04  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/22  07:35:02  bchan
 * CSCdi36282:  interface command lan and lan-name conflict
 *
 * Revision 2.1  1995/06/07  22:07:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * sna start  <name>
 * no sna start <name>
 *
 */

EOLS	(cfg_sna_start_eol, snaif_command, SNA_START);
STRING	(cfg_sna_start_name, cfg_sna_start_eol, no_alt,
	 OBJ(string, 1), "SNA START Pu Name");
NVGENS	(ci_sna_start_nvgen, cfg_sna_start_name, snaif_command,
	 SNA_START);
KEYWORD (cfg_sna_start,ci_sna_start_nvgen, no_alt,
	 "start", "SNA Start command", PRIV_CONF|PRIV_SUBIF);
/*************************************************************************
 * sna enable-host [lsap <lsap>] [sdlc <sdlc_addr>] [qllc <qllc-subaddr>]
 * no sna enable-host
 *
 */
EOLS	(ci_sna_enableh_eol, snaif_command, SNA_ENABLE_HOST);
STRING (ci_sna_enableh_qllc_addr, ci_sna_enableh_eol, no_alt,
        OBJ(string, 1), "SNA Enable-host QLLC subaddress, up to 2 decimal digits");
KEYWORD_ID (ci_sna_enableh_qllc, ci_sna_enableh_qllc_addr,
         ci_sna_enableh_eol, OBJ(int,1), TRUE,
         "qllc", "SNA Enable-host QLLC command", PRIV_CONF);
HEXDIGIT(ci_sna_enableh_sdlc_addr, ci_sna_enableh_qllc, no_alt,
         OBJ(int,4), 0x01, 0xFE, "SNA Enable-host SDLC Address");
KEYWORD_ID (ci_sna_enableh_sdlc, ci_sna_enableh_sdlc_addr,
         ci_sna_enableh_qllc, OBJ(int,3), TRUE,
         "sdlc", "SNA Enable-host SDLC command", PRIV_CONF);
NUMBER  (ci_sna_enableh_lsap_addr, ci_sna_enableh_eol, no_alt,
        OBJ(int, 2), 1, 254, "SNA Enable-host LSAP address");
KEYWORD_ID (ci_sna_enableh_lsap, ci_sna_enableh_lsap_addr,
         ci_sna_enableh_sdlc, OBJ(int,1), TRUE,
         "lsap", "SNA Enable-host LSAP command", PRIV_CONF|PRIV_SUBIF);
SET     (ci_sna_enableh_sdlc_set, ci_sna_enableh_lsap, OBJ(int,4), 0x12);
SET     (ci_sna_enableh_lsap_set, ci_sna_enableh_sdlc_set, OBJ(int,2), 12);
NVGENS	(ci_sna_enablehost_nvgen, ci_sna_enableh_lsap_set, snaif_command,
	 SNA_ENABLE_HOST);
KEYWORD (ci_sna_enablehost, ci_sna_enablehost_nvgen, cfg_sna_start,
	 "enable-host", "SNA Enable Host command", PRIV_CONF|PRIV_SUBIF);

EOLS   (ci_lanname_eol, snaif_command, LAN_NAME);
STRING (ci_lanname_text, ci_lanname_eol, no_alt,
	 OBJ(string, 1), "Up to 8 characters lan name");
NOPREFIX (ci_lanname_noprefix, ci_lanname_text, ci_lanname_eol);
NVGENS	(ci_lanname_nvgen, ci_lanname_noprefix, snaif_command, LAN_NAME);
KEYWORD_MM (ci_lanname, ci_lanname_nvgen, no_alt,
	 "lan-name", "LAN Name command", PRIV_CONF, 4);

KEYWORD (ci_sna, ci_sna_enablehost, ci_lanname,
	 "sna", "SNA pu configuration", PRIV_CONF|PRIV_SUBIF);

TEST_BOOLEAN(ci_sna_test, ci_sna, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE ci_sna_test


