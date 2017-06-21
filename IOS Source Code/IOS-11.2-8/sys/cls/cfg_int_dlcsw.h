/* $Id: cfg_int_dlcsw.h,v 3.3.12.2 1996/08/12 16:00:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/cfg_int_dlcsw.h,v $
 *------------------------------------------------------------------
 * DLC-SW Interface Commands Parser Definition
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_dlcsw.h,v $
 * Revision 3.3.12.2  1996/08/12  16:00:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.12.1  1996/07/11  04:53:15  monical
 * CSCdi61263:  Parser help for FRAS MAP command needs work
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/14  22:20:08  monical
 * CSCdi48278:  FRAS BAN allows illegal bridge numbers in config and BAN
 * breaks
 *
 * Revision 3.2  1995/11/17  08:54:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:52:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/15  17:08:54  sberl
 * CSCdi40292:  FRAS backup needs to retry
 * Implement retry logic for fras backup command. We will not try to
 * activate the primary link several times before switching over to the
 * backup.
 *
 * Revision 2.2  1995/07/27  16:22:38  dignacio
 * CSCdi35736:  fras map sdlc ? displays wrong help selection
 *
 * Revision 2.1  1995/07/04  01:52:18  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:16:40  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:04:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [NO] FRAS BAN {<LOCAL-RING> <BRIDGE-NUM> <RING-GROUP> <BAN-DLCI-MAC>
 * DLCI <DLCI#1>} [<DLCI#2>..<DLCI#5>] [BNI <BNI-MAC-ADDRESS>]
 */

EOLS    (pcdlu_ban_eol, pcdlu_command, PCDLU_BAN);
pdecl   (pcdlu_bni_opts);
ENADDR  (pcdlu_bni_mac, pcdlu_bni_opts, pcdlu_ban_eol, OBJ(hwaddr,2),
         "bni mac address");
KEYWORD (pcdlu_bni, pcdlu_bni_mac, no_alt, "bni", "bni is the
         destination mac address", PRIV_CONF | PRIV_SUBIF);
NOP     (pcdlu_bni_opts, pcdlu_bni, pcdlu_ban_eol);
NUMBER  (pcdlu_ban_dlci5, pcdlu_bni_opts, pcdlu_ban_eol, OBJ(int,8), 16,
         1007, "frame relay ban dlci number 5");
NUMBER  (pcdlu_ban_dlci4, pcdlu_ban_dlci5, pcdlu_bni_opts, OBJ(int,7), 16,
         1007, "frame relay ban dlci number 4");
NUMBER  (pcdlu_ban_dlci3, pcdlu_ban_dlci4, pcdlu_bni_opts, OBJ(int,6), 16,
         1007, "frame relay ban dlci number 3");
NUMBER  (pcdlu_ban_dlci2, pcdlu_ban_dlci3, pcdlu_bni_opts, OBJ(int,5), 16,
         1007, "frame relay ban dlci number 2");
NOPREFIX(pcdlu_dlci_noprefix, pcdlu_ban_dlci2, pcdlu_ban_eol);
NUMBER  (pcdlu_ban_dlci1, pcdlu_dlci_noprefix, pcdlu_bni_opts, OBJ(int,4),
         16, 1007, "frame relay ban dlci number 1");
KEYWORD (pcdlu_ban_dlci, pcdlu_ban_dlci1, no_alt, "dlci", "frame relay ban
         dlci", PRIV_CONF | PRIV_SUBIF);
ENADDR  (pcdlu_ban_mac, pcdlu_ban_dlci, no_alt, OBJ(hwaddr,1),
         "ban dlci mac address");
NUMBER  (pcdlu_ring_group, pcdlu_ban_mac, no_alt, OBJ(int,3), 1,
         SRB_MAX_RING_NUM, "ban ring group number");
NUMBER  (pcdlu_bridge_num, pcdlu_ring_group, no_alt, OBJ(int,2), 1,
         SRB_MAX_BRIDGE_NUM, "ban bridge number");
NUMBER  (pcdlu_ban_lring, pcdlu_bridge_num, no_alt, OBJ(int,1), 1,
         SRB_MAX_RING_NUM, "local ring number");

/************************************************************************
 * [NO] FRAS BAN {FRAME-RELAY <interface> <DLCI-MAC> DLCI <DLCI#1>}
 *      [<DLCI#2>..<DLCI#5>] [BNI <BNI-MAC-ADDRESS>]
 */
 
EOLS    (pcdlu_ban_sdlc_eol, pcdlu_command, PCDLU_BAN_SDLC);
pdecl   (pcdlu_sdlc_bni_opts);
ENADDR  (pcdlu_sdlc_bni_mac, pcdlu_sdlc_bni_opts, pcdlu_ban_sdlc_eol,
         OBJ(hwaddr,2), "bni mac address");
KEYWORD (pcdlu_sdlc_bni, pcdlu_sdlc_bni_mac, no_alt, "bni", "bni is the
         destination mac address", PRIV_CONF);
NOP     (pcdlu_sdlc_bni_opts, pcdlu_sdlc_bni, pcdlu_ban_sdlc_eol);
NUMBER  (pcdlu_ban_sdlc_dlci5, pcdlu_sdlc_bni_opts, pcdlu_ban_sdlc_eol,
         OBJ(int,8), 16, 1007, "frame relay ban dlci number 5");
NUMBER  (pcdlu_ban_sdlc_dlci4, pcdlu_ban_sdlc_dlci5, pcdlu_sdlc_bni_opts,
         OBJ(int,7), 16, 1007, "frame relay ban dlci number 4");
NUMBER  (pcdlu_ban_sdlc_dlci3, pcdlu_ban_sdlc_dlci4, pcdlu_sdlc_bni_opts,
         OBJ(int,6), 16, 1007, "frame relay ban dlci number 3");
NUMBER  (pcdlu_ban_sdlc_dlci2, pcdlu_ban_sdlc_dlci3, pcdlu_sdlc_bni_opts,
         OBJ(int,5), 16, 1007, "frame relay ban dlci number 2");
NOPREFIX(pcdlu_sdlc_dlci_noprefix, pcdlu_ban_sdlc_dlci2, pcdlu_ban_sdlc_eol);
NUMBER  (pcdlu_ban_sdlc_dlci1, pcdlu_sdlc_dlci_noprefix, pcdlu_sdlc_bni_opts,
         OBJ(int,4), 16, 1007, "frame relay ban dlci number 1");
KEYWORD (pcdlu_ban_sdlc_dlci, pcdlu_ban_sdlc_dlci1, no_alt, "dlci",
         "frame relay ban dlci", PRIV_CONF);
ENADDR  (pcdlu_ban_sdlc_dlci_mac, pcdlu_ban_sdlc_dlci, no_alt,
         OBJ(hwaddr,1), "dlci mac address");
INTERFACE (pcdlu_ban_sdlc_int, pcdlu_ban_sdlc_dlci_mac, no_alt,
          OBJ(idb, 1),
          IFTYPE_SERIAL | IFTYPE_SUBIF);
KEYWORD  (pcdlu_ban_sdlc, pcdlu_ban_sdlc_int, pcdlu_ban_lring, "frame-relay",
         "Specify frame relay interface", PRIV_CONF);
 
/************************************************************************
 * [no] fras backup rsrb <vmacaddr> <local-ring> <target-group> <macaddr>
 *                       [retry <number>]
 */

EOLS    (pcdlu_backup_eol, pcdlu_command, PCDLU_BACKUP);
NUMBER  (pcdlu_backup_retry_cnt, pcdlu_backup_eol, no_alt,
		OBJ(int,5), 1, 10, "Retry count");
KEYWORD (pcdlu_backup_retry, pcdlu_backup_retry_cnt, pcdlu_backup_eol,
     "retry", "Maximum retry to open up the FRAS connection ", PRIV_SUBIF);
ENADDR  (pcdlu_backup_host, pcdlu_backup_retry, no_alt,
         OBJ(hwaddr,4), "Host MAC address");
NUMBER  (pcdlu_backup_tring, pcdlu_backup_host, no_alt,
	 OBJ(int,3), 1, SRB_MAX_RING_NUM, "Target ring number");
NUMBER  (pcdlu_backup_lring, pcdlu_backup_tring, no_alt,
	 OBJ(int,2), 1, SRB_MAX_RING_NUM, "Local ring number");
ENADDR  (pcdlu_backup_vmac, pcdlu_backup_lring, no_alt,
         OBJ(hwaddr,1), "Virtual MAC address for backup");
KEYWORD (pcdlu_backup_rsrb, pcdlu_backup_vmac, no_alt, "rsrb",
         "RSRB is the backup route", PRIV_SUBIF);

/************************************************************************
 * [no] fras map { sdlc <address> | llc <macaddr> <lsap> <rsap> }
 *	    interface <int> <int#> 
 *	    {llc <macaddr> <lsap> <rsap> | frame-relay <dlci#> <lsap> <rsap> }
 *	    [ pfid2 | afid2 | fid4 ]
 */
EOLNS    (pcdlu_eol, pcdlu_command);

pdecl   (pcdlu_fidopts);
KEYWORD_ID (pcdlu_opts_fid4, pcdlu_eol, no_alt,
		 OBJ(int,12), PCDLU_OPTS_FID4,
		 "fid4", "for SNA subarea FID4", PRIV_CONF);
KEYWORD_ID (pcdlu_opts_afid2, pcdlu_eol, pcdlu_opts_fid4,
		 OBJ(int,12), PCDLU_OPTS_AFID2,
		 "afid2", "for APPN FID2", PRIV_CONF);
KEYWORD_ID (pcdlu_opts_pfid2, pcdlu_eol, pcdlu_opts_afid2,
		 OBJ(int,12), PCDLU_OPTS_PFID2,
		 "pfid2", "for SNA Peripheral FID2", PRIV_CONF);
NOP     (pcdlu_fidopts, pcdlu_opts_pfid2, pcdlu_eol);

HEXDIGIT  (pcdlu_fr2_rsap, pcdlu_fidopts, no_alt,
		OBJ(int,11), 0, 0xFF, "Remote SAP number");
HEXDIGIT  (pcdlu_fr2_lsap, pcdlu_fr2_rsap, no_alt,
		OBJ(int,10), 0, 0xFF, "Local SAP number");
NUMBER  (pcdlu_fr2, pcdlu_fr2_lsap, no_alt,
		OBJ(int,9), 16, 1007, "Frame-Relay DLCI number");
KEYWORD_ID (pcdlu_protocol23, pcdlu_fr2, no_alt,
		 OBJ(int,5), PCDLU_FR,
		 "frame-relay", "Frame-Relay Protocol", PRIV_CONF);

HEXDIGIT  (pcdlu_sdlc2, pcdlu_fidopts, no_alt,
		OBJ(int,8), 1, 0xFF, "SDLC Address");
KEYWORD_ID (pcdlu_protocol22, pcdlu_sdlc2, pcdlu_protocol23,
		 OBJ(int,5), PCDLU_SDLC,
		 "sdlc", "SDLC Protocol", PRIV_CONF);

HEXDIGIT  (pcdlu_llc2_rsap, pcdlu_fidopts, no_alt,
		OBJ(int,7), 0, 0xFF, "Remote SAP number");
HEXDIGIT  (pcdlu_llc2_lsap, pcdlu_llc2_rsap, no_alt,
		OBJ(int,6), 0, 0xFF, "Local SAPnumber");
ENADDR	(pcdlu_llc2, pcdlu_llc2_lsap, no_alt,
		OBJ(hwaddr,2), "Destination Token Ring MAC address");
KEYWORD_ID (pcdlu_protocol21, pcdlu_llc2, pcdlu_protocol22,
		 OBJ(int,5), PCDLU_LLC,
		 "llc2", "LLC2 Protocol", PRIV_CONF);
FORWARD_INTERFACE (pcdlu_int2, pcdlu_protocol21, no_alt,
		OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_SUBIF);

HEXDIGIT  (pcdlu_sdlc1, pcdlu_int2, no_alt,
		OBJ(int,4), 0, 0xFF, "SDLC Address");
KEYWORD_ID(pcdlu_protocol12, pcdlu_sdlc1, no_alt,
		 OBJ(int,1), PCDLU_SDLC,
		 "sdlc", "SDLC Protocol", PRIV_CONF);

HEXDIGIT  (pcdlu_llc1_rsap, pcdlu_int2, no_alt,
		OBJ(int,3), 0, 0xFF, "Remote SAP number");
HEXDIGIT  (pcdlu_llc1_lsap, pcdlu_llc1_rsap, no_alt,
		OBJ(int,2), 0, 0xFF, "Local SAP number");
ENADDR	(pcdlu_llc1, pcdlu_llc1_lsap, no_alt,
		OBJ(hwaddr,1), "Destination Token Ring MAC address");
KEYWORD_ID (pcdlu_protocol11, pcdlu_llc1, pcdlu_protocol12,
		 OBJ(int,1), PCDLU_LLC,
		 "llc2", "LLC2 Protocol", PRIV_CONF);
NVGENS  (pcdlu_backup_nvgen, pcdlu_backup_rsrb, pcdlu_command, PCDLU_BACKUP);
KEYWORD (pcdlu_backup, pcdlu_backup_nvgen, NONE, "backup",
	 "Setup backup route", PRIV_SUBIF);

NVGENS  (pcdlu_ban_nvgen, pcdlu_ban_sdlc, pcdlu_command, PCDLU_BAN);
KEYWORD (pcdlu_ban, pcdlu_ban_nvgen, pcdlu_backup, "ban",
         "RFC 1490 bridged 802.5 ", PRIV_CONF | PRIV_SUBIF);
NVGENS  (pcdlu_nvgen, pcdlu_protocol11, pcdlu_command, PCDLU_ROUTE);
KEYWORD (cfg_pcdlu_route, pcdlu_nvgen, pcdlu_ban, "map",
	 "Set forwarding of frames", PRIV_CONF);
KEYWORD (cfg_pcdlu, cfg_pcdlu_route, ALTERNATE,
	 "fras", "DLC Switch Interface Command", PRIV_CONF | PRIV_SUBIF);

#undef  ALTERNATE
#define ALTERNATE cfg_pcdlu
