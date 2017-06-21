/* $Id: cfg_int_dspu.h,v 3.2 1995/11/17 09:04:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/cfg_int_dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_dspu.h,v $
 * Revision 3.2  1995/11/17  09:04:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  21:12:51  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/07/04  01:53:03  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:04:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 * dspu start  <name>
 * no dspu start <name>
 *
 */

EOLS	(cfg_dspu_start_eol, dspuif_command, DSPU_START);
STRING	(cfg_dspu_start_name, cfg_dspu_start_eol, no_alt,
	 OBJ(string, 1), "DSPU START Pu Name");
NVGENS	(ci_dspu_start_nvgen, cfg_dspu_start_name, dspuif_command,
	 DSPU_START);
KEYWORD (cfg_dspu_start,ci_dspu_start_nvgen, no_alt,
	 "start", "DSPU Start command", PRIV_CONF|PRIV_SUBIF);
/*************************************************************************
 * dspu enable-host [lsap <lsap_addr>] [sdlc <sdlc_addr>] [qllc <qllc-subaddr>]
 * no dspu enable-host
 *
 */
EOLS	(ci_dspu_enableh_eol, dspuif_command, DSPU_ENABLE_HOST);
STRING (ci_dspu_enableh_qllc_addr, ci_dspu_enableh_eol, no_alt, 
	OBJ(string,1),"DSPU Enable-host QLLC subaddress, up to 2 decimal digits");
KEYWORD_ID (ci_dspu_enableh_qllc, ci_dspu_enableh_qllc_addr, 
         ci_dspu_enableh_eol, OBJ(int,1), TRUE,
         "qllc", "DSPU Enable-host QLLC command", PRIV_CONF);
HEXDIGIT(ci_dspu_enableh_sdlc_addr, ci_dspu_enableh_qllc, no_alt,
         OBJ(int,4), 0x01, 0xFE, "DSPU Enable-host SDLC Address");
KEYWORD_ID (ci_dspu_enableh_sdlc, ci_dspu_enableh_sdlc_addr, 
         ci_dspu_enableh_qllc, OBJ(int,3), TRUE,
         "sdlc", "DSPU Enable-host SDLC command", PRIV_CONF);
NUMBER	(ci_dspu_enableh_lsap_addr, ci_dspu_enableh_eol, no_alt, 
	OBJ(int, 2), 1, 254, "DSPU Enable-host LSAP address");
KEYWORD_ID (ci_dspu_enableh_lsap, ci_dspu_enableh_lsap_addr, 
         ci_dspu_enableh_sdlc, OBJ(int,1), TRUE,
         "lsap", "DSPU Enable-host LSAP command", PRIV_CONF|PRIV_SUBIF);
SET	(ci_dspu_enableh_sdlc_set, ci_dspu_enableh_lsap, OBJ(int,4), 0x12);
SET	(ci_dspu_enableh_lsap_set, ci_dspu_enableh_sdlc_set, OBJ(int,2), 12);
NVGENS	(ci_dspu_enableh_nvgen, ci_dspu_enableh_lsap_set, dspuif_command,
	 DSPU_ENABLE_HOST);
KEYWORD (ci_dspu_enableh, ci_dspu_enableh_nvgen, cfg_dspu_start,
	 "enable-host", "DSPU Enable Host command", PRIV_CONF|PRIV_SUBIF);

/*************************************************************************
 * dspu enable-pu [lsap <lsap_addr>] [sdlc <sdlc_addr>] [x25 <x121-subaddr>]
 * no dspu enable-pu
 *
 */
EOLS	(ci_dspu_enablep_eol, dspuif_command, DSPU_ENABLE);
STRING (ci_dspu_enablep_qllc_addr, ci_dspu_enablep_eol, no_alt,
        OBJ(string, 1), "DSPU Enable-pu QLLC subaddress, up to 2 decimal digits");
KEYWORD_ID (ci_dspu_enablep_qllc, ci_dspu_enablep_qllc_addr,
         ci_dspu_enablep_eol, OBJ(int,1), TRUE,
         "qllc", "DSPU Enable-pu QLLC command", PRIV_CONF);
HEXDIGIT(ci_dspu_enablep_sdlc_addr, ci_dspu_enablep_eol, no_alt,
         OBJ(int,4), 0x01, 0xFE, "DSPU Enable-pu SDLC Address");
KEYWORD_ID (ci_dspu_enablep_sdlc, ci_dspu_enablep_sdlc_addr,
         ci_dspu_enablep_qllc, OBJ(int,3), TRUE,
         "sdlc", "DSPU Enable-pu SDLC command", PRIV_CONF);
NUMBER  (ci_dspu_enablep_lsap_addr, ci_dspu_enablep_eol, no_alt,
        OBJ(int, 2), 1, 254, "DSPU Enable-pu LSAP address");
KEYWORD_ID (ci_dspu_enablep_lsap, ci_dspu_enablep_lsap_addr,
         ci_dspu_enablep_sdlc, OBJ(int,1), TRUE,
         "lsap", "DSPU Enable-pu LSAP command", PRIV_CONF|PRIV_SUBIF);
SET     (ci_dspu_enablep_sdlc_set, ci_dspu_enablep_lsap, OBJ(int,4), 0x08);
SET     (ci_dspu_enablep_lsap_set, ci_dspu_enablep_sdlc_set, OBJ(int,2), 8);
NVGENS	(ci_dspu_enable_nvgen, ci_dspu_enablep_lsap_set, dspuif_command,
	 DSPU_ENABLE);
KEYWORD (ci_dspu_enable, ci_dspu_enable_nvgen, ci_dspu_enableh,
	 "enable-pu", "DSPU Enable Pu command", PRIV_CONF|PRIV_SUBIF);

/*************************************************************************/
KEYWORD (ci_dspu, ci_dspu_enable, NONE,
	"dspu", "Down Stream PU", PRIV_CONF|PRIV_SUBIF);

TEST_BOOLEAN(ci_dspu_test, ci_dspu, NONE, ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE ci_dspu_test
