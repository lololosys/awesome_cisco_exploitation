/* $Id: dec_ppb_show_chain.h,v 3.4.48.2 1996/08/12 16:06:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/dec_ppb_show_chain.h,v $
 *------------------------------------------------------------------
 * dec_ppb_show_chain.h : DEC 21150 PCI-PCI bridge show parser chains
 *
 * May 1995, Dave Getchell
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dec_ppb_show_chain.h,v $
 * Revision 3.4.48.2  1996/08/12  16:06:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.48.1  1996/03/21  23:22:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1995/12/12  04:53:53  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.2.2.2  1995/12/08  05:33:25  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.2.2.1  1995/11/22  21:41:31  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:53:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show pci bridge <x>
 *
 */
 
EOLNS    (bridge_reg_number_eol, show_pci_bridge_regs);
NUMBER  (bridge_reg_number, bridge_reg_number_eol, no_alt,
	 OBJ(int,1), 0, MAX_PA_BAYS, "Bridge (PA) number");
KEYWORD (bridge_regs, bridge_reg_number, no_alt,
         "bridge", "Print PCI bridge registers",
         PRIV_ROOT);
 
#undef  ALTERNATE
#define ALTERNATE bridge_regs
