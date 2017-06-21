/* $Id: cfg_int_qllc.h,v 3.2.62.2 1996/07/08 01:10:56 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_int_qllc.h,v $
 *------------------------------------------------------------------
 * QLLC interface subcommands
 *
 * April, 1994		Phil Morton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Syntax for qllc interface subcommands
 *------------------------------------------------------------------
 * $Log: cfg_int_qllc.h,v $
 * Revision 3.2.62.2  1996/07/08  01:10:56  rbatz
 * CSCdi60002:  Npsi-poll and proxy-xid do not work for DLSw+/QLLC.
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:10:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  02:01:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  18:45:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:53:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  21:06:24  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.1  1995/07/04  01:57:57  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:07:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/***************************************************************
 *
 *	DLSw+ over QLLC configuration
 *
 * [no] qllc dlsw 
 *	[{subaddress subaddress}|{pvc pvc-low [pvc-high]}] 
 *		OBJ(paddr,1)	OR OBJ(int,2) [OBJ(int,3)]
 *	[vmac <virtual mac address> [<poolsize>]] 
 *		OBJ(hwaddr,1)        OBJ(int,4)
 *	[partner <partner mac address>] 
 *		OBJ(hwaddr,2)
 *	[sap dsap ssap] 
 *		OBJ(int,5) OBJ(int,6)
 *	[xid xidstring]
 *		OBJ(string,1)
 *	[npsi-poll]
 *
 *	The parse chain sets bits in a flag word OBJ(int,1)
 *	corresponding to options selected:
 *	The bits are defined in clsi_msg.h
 *
 *	dlsw						QLLC_DLSW  always set
 *	{pvc pvc-low [pvc-high]}			QLLC_PVC
 *	[vmac <virtual mac address> [pool <poolsize>]] 	QLLC_VMACS
 *	[partner <partner mac address>] 		QLLC_PARTNER
 *	[sap dsap ssap] 				QLLC_TR_SAPS
 *	[xid xidstring]					QLLC_PROXYXID
 *	[npsi-poll]					QLLC_NPSI_POLL
 *
 */

EOLS	 (qllc_dlsw_eol, lnxif_command, QLLC_CFG_DLSW);

pdecl(qllc_dlsw_options);

/* npsi_poll */

KEYWORD_OR(qllc_dlsw_npsi_poll_kw, qllc_dlsw_options, qllc_dlsw_eol,
	   OBJ(int,1), QLLC_NPSI_POLL, "npsi-poll", 
	   "PU2.0 on the LAN, FEP w/NPSI on X.25 interface",
	   PRIV_CONF | PRIV_SUBIF);

/* xid xidstring */

STRING   (qllc_dlsw_xid_string, qllc_dlsw_options, NONE,
          OBJ(string,1), "XID (must be 8 hex digits)");
KEYWORD_OR(qllc_dlsw_xid_kw, qllc_dlsw_xid_string, qllc_dlsw_npsi_poll_kw,
	   OBJ(int,1), QLLC_PROXYXID, "xid", "Provide proxy IDBLK/IDNUM for PU2.0",
	   PRIV_CONF | PRIV_SUBIF);

/* sap <dsap> <ssap> */

PRINT	(qllc_dlsw_sap_error, qllc_dlsw_options, "SAPs must be even values");
IFELSE(qllc_dlsw_sap_test, qllc_dlsw_options, qllc_dlsw_sap_error,
		(( (GETOBJ(int,5) | GETOBJ(int, 6)) & 1) ==0) );
HEXDIGIT (qllc_dlsw_sap_ssap, qllc_dlsw_sap_test, no_alt,
          OBJ(int,6), 2, 0xFE, "Source sap");
HEXDIGIT (qllc_dlsw_sap_dsap, qllc_dlsw_sap_ssap, NONE,
          OBJ(int,5), 2, 0xFE, "Destination sap");
KEYWORD_OR(qllc_dlsw_sap_kw, qllc_dlsw_sap_dsap, qllc_dlsw_xid_kw,
	   OBJ(int,1), QLLC_TR_SAPS, "sap", "Override default dsap and ssap",
	   PRIV_CONF | PRIV_SUBIF);

/* Partner <mac address> */

ENADDR	(qllc_dlsw_partner_addr, qllc_dlsw_options, NONE,
	 OBJ(hwaddr,2), "Partner MAC address");
KEYWORD_OR(qllc_dlsw_partner_kw, qllc_dlsw_partner_addr, qllc_dlsw_sap_kw,
	   OBJ(int,1), QLLC_PARTNER, "partner", "Configure partner mac address",
	   PRIV_CONF | PRIV_SUBIF);

/* VMAC <vmac> [poolsize] */

NUMBER	(qllc_dlsw_vmac_pool, qllc_dlsw_options, qllc_dlsw_options,
	 OBJ(int,4), 1, 4095, "Size of Virtual Mac address pool");
ENADDR	(qllc_dlsw_vmac_addr, qllc_dlsw_vmac_pool, NONE,
	 OBJ(hwaddr,1), "MAC address mapped to remote x.25 address");
KEYWORD_OR(qllc_dlsw_vmac_kw, qllc_dlsw_vmac_addr, qllc_dlsw_partner_kw,
	   OBJ(int,1), QLLC_VMACS, "vmacaddr", "Configure a virtual mac address",
	   PRIV_CONF | PRIV_SUBIF);

/* PVCs */

PRINT 	(qllc_dlsw_pvc_range_error, qllc_dlsw_options, "PVC range error");
IFELSE	(qllc_dlsw_pvc_range_check, qllc_dlsw_pvc_range_error, qllc_dlsw_vmac_kw, 		
		( GETOBJ(int,3) < GETOBJ(int,2) ) );
NUMBER	(qllc_dlsw_pvc_high, qllc_dlsw_pvc_range_check, qllc_dlsw_vmac_kw,
	 OBJ(int,3), 1, 4095, "High value for PVC Range");
NUMBER	(qllc_dlsw_pvc_low, qllc_dlsw_pvc_high, NONE,
	 OBJ(int,2), 1, 4095, "Low value for PVC Range");
KEYWORD_OR(qllc_dlsw_pvc, qllc_dlsw_pvc_low, qllc_dlsw_vmac_kw,
	   OBJ(int,1), QLLC_PVC, "pvc", "Configure PVCs",
	   PRIV_CONF | PRIV_SUBIF);

/* Subaddress */

NOPREFIX (qllc_dlsw_noprefix, qllc_dlsw_vmac_kw, qllc_dlsw_eol);
X121ADDR(qllc_dlsw_subaddr, qllc_dlsw_noprefix, NONE,
	 OBJ(paddr,1), "Subaddress string");
KEYWORD	(qllc_dlsw_options, qllc_dlsw_subaddr, qllc_dlsw_pvc,
	 "subaddress", "Specify x121 subaddress", PRIV_CONF);


NVGENS	 (qllc_dlsw_nvgen, qllc_dlsw_options,
	  lnxif_command, QLLC_CFG_DLSW);
KEYWORD_OR (qllc_dlsw, qllc_dlsw_nvgen, no_alt, 
	    OBJ(int,1), QLLC_DLSW, "dlsw", "Configure DLSw over QLLC", 
	    PRIV_CONF | PRIV_SUBIF);

/***************************************************************
 * [no] qllc npsi-poll <virtual-mac-addr>
 */

EOLS	 (qllc_poll_eol, lnxif_command, LNX_POLL);
NOPREFIX (qllc_poll_noprefix, qllc_poll_eol, qllc_poll_eol);
ENADDR	 (qllc_poll_vmacaddr, qllc_poll_noprefix, no_alt,
          OBJ(hwaddr,1),"Virtual Token Ring MAC address mapped to remote x.25 address");
NVGENS	 (qllc_poll_nvgen, qllc_poll_vmacaddr,
	  lnxif_command, LNX_POLL);
KEYWORD  (qllc_poll, qllc_poll_nvgen, qllc_dlsw, "npsi-poll",
          "Poll the secondary device on the LAN with a NULL XID",
          PRIV_CONF);

/***************************************************************
 * [no] qllc largest-packet <virtual-mac-addr> <max-size>
 */

EOLS     (qllc_lp_eol, lnxif_command, LNX_QLLCLF);
NOPREFIX (qllc_lp_noprefix, qllc_lp_eol,
         qllc_lp_eol);
NUMBER   (qllc_lp_num, qllc_lp_noprefix, no_alt,
          OBJ(int,1), 0, 1024, 
	  "largest x.25 packet size accepted on this interface");
ENADDR   (qllc_lp_vmacaddr, qllc_lp_num, no_alt,
	  OBJ(hwaddr,1),"Virtual Token Ring MAC address mapped to remote x.25 address"); 
NVGENS   (qllc_lp_nvgen, qllc_lp_vmacaddr,
          lnxif_command, LNX_QLLCLF);
KEYWORD  (qllc_lp, qllc_lp_nvgen, qllc_poll,
          "largest-packet", 
	  "largest packet that can be sent or received on the x.25 interface",
	  PRIV_CONF);



/***************************************************************
 * [no] qllc xid <virtual-mac-addr> <xid>
 */

EOLS     (qllc_xid_eol, lnxif_command, LNX_XID);
STRING   (qllc_xid_string, qllc_xid_eol, NONE,
          OBJ(string,1), "XID (must be 8 digits)");
NOPREFIX (qllc_xid_noprefix, qllc_xid_string,
         qllc_xid_eol);
ENADDR   (qllc_xid_vmacaddr, qllc_xid_noprefix, no_alt,
	  OBJ(hwaddr,1), 
	  "Virtual Token Ring MAC address mapped to remote x.25 address"); 
NVGENS   (qllc_xid_nvgen, qllc_xid_vmacaddr,
          lnxif_command, LNX_XID);
KEYWORD  (qllc_xid, qllc_xid_nvgen, qllc_lp,
          "xid", "Preconfigured xid value with a remote x.25 device", 
	  PRIV_CONF);


/***************************************************************
 * <no> qllc partner <virtual-mac-addr> <mac-addr>
 */

EOLS     (qllc_prtnr_eol, lnxif_command, LNX_PRTNR);
NOPREFIX (qllc_prtnr_noprefix, qllc_prtnr_eol,
         qllc_prtnr_eol);
ENADDR   (qllc_prtnr_macaddr, qllc_prtnr_noprefix, no_alt,
	  OBJ(hwaddr,2), "mac address for qllc partner"); 
ENADDR   (qllc_prtnr_vmacaddr, qllc_prtnr_macaddr, no_alt,
	  OBJ(hwaddr,1),"Virtual Token Ring MAC address mapped to remote x.25 address");
NVGENS   (qllc_prtnr_nvgen, qllc_prtnr_vmacaddr,
          lnxif_command, LNX_PRTNR);
KEYWORD  (qllc_prtnr, qllc_prtnr_nvgen, qllc_xid,
          "partner", 
	  "MAC address of partner for QLLC connection", PRIV_CONF);


/***************************************************************
 * <no> qllc sap <virtual-mac-addr> <ssap> <dsap>
 */


EOLS     (qllc_sap_eol, lnxif_command, LNX_SAP);
NOPREFIX (qllc_sap_noprefix, qllc_sap_eol,
         qllc_sap_eol);
HEXDIGIT (qllc_sap_dsap, qllc_sap_noprefix, no_alt,
          OBJ(int,2), 2, 0xFE, "Destination sap - even value between 0 and FE");
HEXDIGIT (qllc_sap_ssap, qllc_sap_dsap, no_alt,
          OBJ(int,1), 2, 0xFE, "Source sap - even value from 0 and FE");
ENADDR   (qllc_sap_vmacaddr, qllc_sap_ssap, no_alt,
	  OBJ(hwaddr,1),"Virtual Token Ring MAC address mapped to remote x.25 address");
NVGENS   (qllc_sap_nvgen, qllc_sap_vmacaddr,
          lnxif_command, LNX_SAP);
KEYWORD  (qllc_sap, qllc_sap_nvgen, qllc_prtnr,
          "sap", "Change the default sap values", PRIV_CONF);

/***************************************************************
 * qllc srb <virtual-token-ring-addr> <srn> <trn>
 * no qllc srb <srn> <trn>
 */

EOLS     (qllc_srb_eol, lnxif_command, LNX_SRB);
NOPREFIX (qllc_srb_noprefix, qllc_srb_eol,
         qllc_srb_eol);
NUMBER   (qllc_srb_trn, qllc_srb_noprefix, no_alt,
          OBJ(int,2), 1, SRB_MAX_RING_NUM, 
	  "target ring number - ring group");
NUMBER   (qllc_srb_srn, qllc_srb_trn, no_alt,
          OBJ(int,1), 1, SRB_MAX_RING_NUM, 
	  "QLLC local ring number - virtual ring number");
ENADDR   (qllc_srb_vmacaddr, qllc_srb_srn, no_alt,
	  OBJ(hwaddr,1),"Virtual Token Ring MAC address mapped to remote x.25 address");
NVGENS   (qllc_srb_nvgen, qllc_srb_vmacaddr,
          lnxif_command, LNX_SRB);
KEYWORD  (qllc_srb, qllc_srb_nvgen, qllc_sap,
          "srb", "Define QLLC local and target rings", PRIV_CONF);

/* qllc */
KEYWORD(cfg_qllc, qllc_srb, NONE, "qllc",
       "Configure qllc to llc or qllc to sdlc translation", PRIV_CONF);

ASSERT	(qllc_test, cfg_qllc, ALTERNATE,
	 (is_x25(csb->interface->hwptr)));

#undef  ALTERNATE
#define ALTERNATE      qllc_test
