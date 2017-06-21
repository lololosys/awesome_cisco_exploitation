/* $Id: exec_dlutest.h,v 3.4.20.2 1996/08/12 16:00:50 widmer Exp $
 * $Source: /release/112/cvs/Xsys/cls/exec_dlutest.h,v $
 *------------------------------------------------------------------
 * E X E C _ D L U T E S T . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_dlutest.h,v $
 * Revision 3.4.20.2  1996/08/12  16:00:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.1  1996/05/17  10:43:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.1  1996/03/28  08:19:48  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.4  1996/02/01  23:33:04  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1996/01/29  07:08:27  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2.4.2  1996/03/03  00:26:20  ioakley
 * CSCdi50585:  Pick up Phil Morton's Vdlc_Rework and
 *              Mark Regan's Appn over DLSw updates.
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2.4.1  1995/12/17  09:50:19  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:56:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:53:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/19  20:58:34  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.1  1995/07/04  01:52:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:20:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * dlutest <start/stop>
 * dlutest start prim|sec enaddr num_frames token|rsrb|frame-relay [dlci]|fddi
 *
 * dlutest start prim|sec [xid idblk/idnum] num_frames \
 *	qllc {subaddress <subaddress> | vmac <ieee-address>} \
 *      <interface> <remote x121>
 *
 * dlutest start {<primary>|<secondary>} vdlc <ring-group> \
 *        <local-vmac> [remote-mac] \
 *        [sap <local sap> <remote sap>]
 */

EOLNS    (exec_dlutest_eol, dlutest_command);

KEYWORD_ID (exec_dlutest_stop, exec_dlutest_eol, no_alt,
            OBJ(int,1), DLUSTOP, "stop", "Stop DLU test program", PRIV_ROOT);

INTERFACE (exec_dlutest_idb,
	   exec_dlutest_eol, exec_dlutest_eol,
	   OBJ(idb,1), (IFTYPE_ANYSUB & ~IFTYPE_LEX));

NUMBER (exec_dlutest_vdlc_sap_rsap, exec_dlutest_eol, no_alt,
      OBJ(int,7), 2, 0xFE, "Remote Sap");

NUMBER (exec_dlutest_vdlc_sap_lsap, exec_dlutest_vdlc_sap_rsap, no_alt,
      OBJ(int,6), 2, 0xFE, "Local Sap");

KEYWORD (exec_dlutest_vdlc_sap, exec_dlutest_vdlc_sap_lsap, exec_dlutest_eol,
         "sap", "Set saps", PRIV_ROOT);

ENADDR (exec_dlutest_vdlc_remote_mac,
	exec_dlutest_vdlc_sap,  exec_dlutest_vdlc_sap,
	 OBJ(hwaddr,2), "Remote Virtual Mac Address");

ENADDR (exec_dlutest_vdlc_local_mac,
	exec_dlutest_vdlc_remote_mac,  no_alt,
	 OBJ(hwaddr,1), "Local Virtual Mac Address");

NUMBER (exec_dlutest_vdlc_ring, exec_dlutest_vdlc_local_mac, no_alt,
      OBJ(int,5), 1, 4095, "Ring group (Virtual Ring");

KEYWORD_ID (exec_dlutest_vdlc,
	    exec_dlutest_vdlc_ring, no_alt,
            OBJ(int,4), DLU_Vdlc,
            "VDLC", "Set the DLU to use VDLC", PRIV_ROOT);

ENADDR (exec_dlutest_qllc_remote_vmac,
	exec_dlutest_idb,  no_alt,
	OBJ(hwaddr,3), "Remote address - virtual mac address");

KEYWORD_ID (exec_dlutest_qcep_vmac,
	    exec_dlutest_qllc_remote_vmac, no_alt,
	    OBJ(int,7), FALSE, "vmac", "Use Vmac for QSap", PRIV_ROOT);

X121ADDR (exec_dlutest_qllc_remote_x121,
	  exec_dlutest_idb, no_alt,
	  OBJ(paddr,3), "Remote address - X.121 address");

KEYWORD_ID (exec_dlutest_qcep_x121,
	    exec_dlutest_qllc_remote_x121,
	    exec_dlutest_qcep_vmac,
	    OBJ(int,7), TRUE, "x121", "Use X121 address", PRIV_ROOT);

ENADDR(exec_dlutest_qllc_vmacaddr,
       exec_dlutest_qcep_x121, no_alt,
       OBJ(hwaddr,2), "Virtual Mac address for the QLLC SAP");

X121ADDR(exec_dlutest_qllc_subaddr,
	 exec_dlutest_qcep_x121,
	 exec_dlutest_qllc_vmacaddr,
	 OBJ(paddr,2), "Sub address for the SAP");

KEYWORD_ID (exec_dlutest_qllc,
	    exec_dlutest_qllc_subaddr, exec_dlutest_vdlc,
            OBJ(int,4), DLU_Qllc,
            "QLLC", "Set the DLU to use QLLC", PRIV_ROOT);
/*
 * PPP
 */
KEYWORD_ID (exec_dlutest_ppp, exec_dlutest_idb, exec_dlutest_qllc,
            OBJ(int,4), DLU_Ppp,
            "PPP", "Set the DLU to use PPP", PRIV_ROOT);

/*
 * SMDS
 */
SMDSADDR (exec_dlutest_smds_addr,
	  exec_dlutest_idb, no_alt,
	  OBJ(hwaddr,2), "Remote SMDS Address");

KEYWORD_ID (exec_dlutest_smds, exec_dlutest_smds_addr, exec_dlutest_ppp,
            OBJ(int,4), DLU_Smds,
            "SMDS", "Set the DLU to use SMDS", PRIV_ROOT);

/*
 * Frame Relay
 */
NUMBER	(exec_dlutest_dlci, exec_dlutest_idb, no_alt,
	 OBJ(int,6), 1, 1023,
	 "DLCI number for frame-relay connection (default=100)");

SET (exec_dlutest_dlci_set, exec_dlutest_dlci, OBJ(int,5), 100);

KEYWORD_ID (exec_dlutest_frame, exec_dlutest_dlci_set, exec_dlutest_smds,
            OBJ(int,4), DLU_FrameRelay,
            "frame-relay", "Set the DLU to use Frame Relay", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_rsrb, exec_dlutest_idb, exec_dlutest_frame,
            OBJ(int,4), DLU_Rsrb,
            "rsrb", "Set the DLU to use RSRB (VMAC == 4000.dead.beef)",
	    PRIV_ROOT);
KEYWORD_ID (exec_dlutest_type_ieee, exec_dlutest_idb, exec_dlutest_eol,
            OBJ(int, 6), FALSE,
            "ieee", "Set address type to IEEE.",
            PRIV_ROOT);
KEYWORD_ID (exec_dlutest_type, exec_dlutest_idb, exec_dlutest_type_ieee,
            OBJ(int, 6), TRUE,
            "sdlc", "Set address type to SDLC.",
            PRIV_ROOT);
HEXDIGIT   (exec_dlutest_addr, exec_dlutest_type, exec_dlutest_eol,
            OBJ(int, 5), 1, 0xFE, "SDLC address in hex");

KEYWORD_ID (exec_dlutest_sdlc, exec_dlutest_addr, exec_dlutest_rsrb,
            OBJ(int,4), DLU_Sdlc,
            "sdlc", "Set the DLU to use SDLC", PRIV_USER);

KEYWORD_ID (exec_dlutest_fddi, exec_dlutest_idb, exec_dlutest_sdlc,
            OBJ(int,4), DLU_Fddi,
            "fddi", "Set the DLU to use FDDI", PRIV_USER);

KEYWORD_ID (exec_dlutest_ether, exec_dlutest_idb, exec_dlutest_fddi,
            OBJ(int,4), DLU_EtherNet,
            "ether", "Set the DLU to use EtherNet 0", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_token, exec_dlutest_idb, exec_dlutest_ether,
            OBJ(int,4), DLU_TokenRing,
            "token", "Set the DLU to use Token-Ring 0", PRIV_ROOT);

SET (exec_dlutest_token_set, exec_dlutest_token, OBJ(int,4), DLU_TokenRing);

NUMBER  (exec_dlutest_num_frames, exec_dlutest_token_set,
	 exec_dlutest_token_set,
         OBJ(int,3), 0, 10000,
	 "Number of I-frames transferred during sessions (default = 0)");

SET (exec_dlutest_num_frames_set, exec_dlutest_num_frames, OBJ(int,3), 0);

ENADDR  (exec_dlutest_macaddr_set, exec_dlutest_num_frames_set, no_alt,
         OBJ(hwaddr,1), "Token-ring mac address");

NOP	(exec_dlutest_macaddr, exec_dlutest_macaddr_set,
	 exec_dlutest_num_frames_set);

KEYWORD_ID (exec_dlutest_call, exec_dlutest_macaddr, exec_dlutest_macaddr,
	    OBJ(int,11), TRUE,
	    "call", "Initiate the connection", PRIV_ROOT);

STRING   (exec_dlutest_xid_val, exec_dlutest_call, no_alt,
          OBJ(string,1), "XID (must be 8 hex digits)");
KEYWORD_ID (exec_dlutest_xid_kw, exec_dlutest_xid_val, exec_dlutest_call,
	    OBJ(int,10), TRUE,
	    "xid", "Specify an XID string", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_echo_kw, exec_dlutest_xid_kw, exec_dlutest_call,
	    OBJ(int,8), TRUE,
	    "echo", "Echo received data back", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_psec, exec_dlutest_echo_kw, no_alt,
            OBJ(int,2), DLUPSEC,
            "psec", "Set the DLU to be proxy sec", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_pprim, exec_dlutest_call, exec_dlutest_psec,
            OBJ(int,2), DLUPPRIM,
            "pprim", "Set the DLU to be proxy prim", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_secondary, exec_dlutest_echo_kw, exec_dlutest_pprim,
            OBJ(int,2), DLUSECONDARY,
            "secondary", "Set the DLU to be secondary", PRIV_ROOT);

KEYWORD_ID (exec_dlutest_primary, exec_dlutest_call, exec_dlutest_secondary,
            OBJ(int,2), DLUPRIMARY,
            "primary", "Set the DLU to be primary", PRIV_ROOT);
KEYWORD_ID(exec_dlutest_start, exec_dlutest_primary, exec_dlutest_stop,
	   OBJ(int,1), DLUSTART,
	   "start", "Start DLU test program", PRIV_ROOT);

KEYWORD (exec_dlutest, exec_dlutest_start, ALTERNATE,
         "dlutest", "DLU test program execution command", PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE       exec_dlutest
