/* $Id: pas_show_chain.h,v 3.3.48.3 1996/08/12 16:06:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_show_chain.h,v $
 *------------------------------------------------------------------
 * pas_show_chain.h - Platform independent PA show parse chains
 *
 * May 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pas_show_chain.h,v $
 * Revision 3.3.48.3  1996/08/12  16:06:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.48.2  1996/04/29  17:57:35  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.3.48.1  1996/03/21  23:24:24  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/01/26  02:50:20  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.2.2.2  1995/12/26  19:57:09  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.2.2.1  1995/12/08  05:33:32  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.2  1995/11/17  18:55:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show pci hardware
 * 
 */
EOLNS	(show_pci_hw_eol, show_pci);
KEYWORD (show_pci_hw, show_pci_hw_eol, no_alt,
	 "hardware", "PCI hardware registers", PRIV_ROOT);

LINK_POINT(pci_show_hw_command, show_pci_hw);

/******************************************************************
 * show controllers { interface }
 *
 * OBJ(idb,1) = <interface>
 */
EOLNS	(show_controller_pas_eol, show_controllers);

INTERFACE (show_controller_get_interface, show_controller_pas_eol,
	   no_alt, OBJ(idb,1),
	   (IFTYPE_ETHER | IFTYPE_SERIAL | IFTYPE_FASTETHER));

LINK_POINT(pas_show_controller_commands, show_controller_get_interface);

/******************************************************************
 * show pas
 * 
 */
LINK_TRANS(show_pas_extend_here, no_alt);

KEYWORD (show_pas, show_pas_extend_here, NONE,
	 "pas", "Port Adaptor Information", PRIV_USER | PRIV_USER_HIDDEN);

LINK_POINT(pas_show_commands, show_pas);

/******************************************************************
 * show pci
 * 
 */
LINK_TRANS(show_pci_extend_here, no_alt);

KEYWORD (show_pci, show_pci_extend_here, NONE,
	 "pci", "PCI Information", PRIV_USER | PRIV_USER_HIDDEN);

LINK_POINT(pci_show_commands, show_pci);

