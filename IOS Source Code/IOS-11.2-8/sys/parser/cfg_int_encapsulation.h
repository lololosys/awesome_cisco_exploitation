/* $Id: cfg_int_encapsulation.h,v 3.6.20.4 1996/08/12 16:05:36 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_int_encapsulation.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ E N C A P S U L A T I O N . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_encapsulation.h,v $
 * Revision 3.6.20.4  1996/08/12  16:05:36  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6.20.3  1996/06/07  00:14:47  hou
 * CSCdi59621:  SAID invalid range (1 - 0xFFFFFFFF) on c70x0,c75xx
 * Branch: California_branch.
 *
 * Revision 3.6.20.2  1996/05/19  21:09:01  hou
 * CSCdi57265:  Bridging broke when ISL enabled on FEIP between Catalyst
 * and 7500
 * Branch: California_branch
 * - change sde range from 1 - 0x3FFFFFFF to 1 - 0xFFFFFFFF
 *
 * Revision 3.6.20.1  1996/03/18  21:33:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/13  01:51:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.20.1  1996/02/20  16:43:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  23:34:17  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.5  1996/01/23  08:51:09  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.4  1995/12/01  09:26:22  mmcneali
 * CSCdi44792:  Not allow same bridge-group to have different dot10 vLAN
 *              SAIDs.
 *
 *              In order to bridge different vLANs together, relax this
 *              restriction.
 *
 * Revision 3.3  1995/11/27  16:23:47  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.2  1995/11/17  18:43:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:20:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/21  18:16:42  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/08  18:31:51  wmay
 * CSCdi31154:  no diagnostic saying X25 is not supported on Hssi -
 * don't allow command for x25 or lapb
 *
 * Revision 2.1  1995/06/07  22:04:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *  The definitions for the Encapsulation types are in interface.h
 *
 *  OBJ(int,1) =     Serial Interface
 *					ET_STUN		ET_SMDS
 *					ET_SDLCP	ET_SDLCS
 *					ET_PPP		ET_X25
 *					ET_LAPB		ET_HDLC
 *					ET_FRAME_RELAY	ET_ATM_DXI
 *                                      ET_SDE (HDLC only)
 *                                      ET_BSTUN
 *
 *              Other Interfaces        ET_SNAP ET_NOVELL
 *                                      ET_SAP  ET_ARPA
 *                                      ET_SDE  ET_ISL
 */

EOLNS	(config_encap_eol, encapsulation_command);

TESTVAR	(config_no_encap, config_encap_eol, NONE,
	 NONE, NONE, no_alt, sense, FALSE);

/**********************************************************************
 *
 *  [no] encapsulation isl <vLAN Identifier>
 *
 *  Currently permitted only on Fast Ethernet, and FDDI-Translational
 *  sub-interface types ... This provides support for Synergy Switch 
 *  Virtual LANs on the router platforms.
 *
 *  OBJ(int, 1) = "isl"
 *  OBJ(int, 2) = The Virtual LAN Identifer, a two byte even value entity.
 *
 *  Need to match "isl", check range on the vLAN.
 */

NUMBER       (config_isl, config_encap_eol, no_alt, OBJ(int, 2), 1, 1000,
	     "Virtual LAN Identifier.");

             /* If we've parsed a "no" prefix, head for the door */
NOPREFIX(config_isl_noencap, config_isl, config_encap_eol);
NVGENNS	(config_isl_nvgen, config_isl_noencap,
	 encapsulation_command);

ENCAPS_SUBIF (config_encap_isl, config_isl_nvgen, config_no_encap,
	      OBJ(int,1), ET_ISL,
	      "isl", "Inter Switch Link - Virtual LAN encapsulation",
	      is_isl_vlan_capable(csb->interface));

/****************************************************************************
 *
 *  [no] encapsulation sde <SAID>
 *
 *  Currently permitted only on Ethernet, FDDI-Translational, CBus Token Ring 
 *  and HDLC Serial interface types ...
 *
 *  OBJ(int, 1) = "sde"
 *  OBJ(int, 2) = SAID value, a four byte entity.
 *
 *  Need to match "sde", check range on SAID (i.e. vLAN ID) and ensure we're
 *  on a subinterface.
 *
 ****************************************************************************/

NUMBER       (config_said, config_encap_eol, no_alt, OBJ(int, 2), 1, 0xFFFFFFFE,
              "Security Association ID required, range 1 - 0xFFFFFFFE.");

             /* If we've parsed a "no" prefix (sense = 0), head for the door */

NOPREFIX(config_sde_noencap, config_said, config_encap_eol);
NVGENNS	(config_sde_nvgen, config_sde_noencap,
	 encapsulation_command);

ENCAPS_SUBIF (config_encap_sde, config_sde_nvgen, config_encap_isl,
	      OBJ(int,1), ET_SDE,
	      "sde", "IEEE 802.10 Virtual LAN - Secure Data Exchange",
	      is_sde_capable(csb->interface));

KEYWORD	     (config_encap_subif_kwd, config_encap_sde, NONE,
	      "encapsulation", "Set encapsulation type for an interface",
	      PRIV_CONF | PRIV_SUBIF);

ASSERT	     (config_encap_subif, config_encap_subif_kwd, NONE,
	      is_vlan_capable(csb->interface));

/**********************************************************************
 * [no] encapsulation snap
 */
ENCAPS_HIDDEN(config_encap_snap, config_encap_eol, config_no_encap,
	      OBJ(int,1), ET_SNAP,
	      "snap", "IEEE 802.2 with SNAP",
	      (IDB_ETHER | IDB_FDDI | IDB_TR));

/**********************************************************************
 * [no] encapsulation novell
 */
ENCAPS_HIDDEN(config_encap_novell, config_encap_eol, config_encap_snap,
	      OBJ(int,1), ET_NOVELL_ETHER,
	      "novell-ether", "Novell IPX over Ethernet",
	      IDB_ETHER);

/**********************************************************************
 * [no] encapsulation iso3
 */
ENCAPS_HIDDEN(config_encap_iso3, config_encap_eol, config_encap_novell,
	      OBJ(int,1), ET_SAP,
	      "iso3", "IEEE 802.2",
	      IDB_FDDI);

/**********************************************************************
 * [no] encapsulation sap
 */

ENCAPS_HIDDEN(config_encap_iso1, config_encap_eol, config_encap_iso3,
	      OBJ(int,1), ET_SAP, "iso1", "",
	      (IDB_ETHER | IDB_FDDI));

ENCAPS_HIDDEN(config_encap_sap, config_encap_eol, config_encap_iso1,
	      OBJ(int,10), ET_SAP,
	      "sap", "IEEE 802.2",
	      (IDB_ETHER | IDB_FDDI));

/**********************************************************************
 * [no] encapsulation arpa
 */

ENCAPS_HIDDEN(config_encap_arpa, config_encap_eol, config_encap_sap,
	      OBJ(int,10), ET_ARPA,
	      "arpa", "Standard Ethernet version 2.0",
	      IDB_ETHER);

LINK_TRANS(config_encap_return, config_encap_eol);
LINK_TRANS(config_encap_extend_here, config_encap_arpa);

KEYWORD	(config_encap_kwd, config_encap_extend_here, NONE,
	 "encapsulation", "Set encapsulation type for an interface",
	 PRIV_CONF);

KEYWORD (config_encap_hidden, config_encap_extend_here, NONE,
	 "encapsulation", "Set encapsulation type for an interface",
	 PRIV_CONF | PRIV_HIDDEN);

TEST_IDBSTATUS(config_encap_test2, config_encap_kwd, config_encap_hidden, NONE,
	       IDB_SERIAL);

IFELSE	(config_encap_test1, config_encap_subif, config_encap_test2,
	 is_subinterface(csb->interface));

NOP	(config_encap_nop, config_encap_test1, ALTERNATE);

#undef  ALTERNATE
#define ALTERNATE       config_encap_nop
