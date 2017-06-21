/* $Id: pas_test_chain.h,v 3.2.58.2 1996/08/12 16:06:47 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/pas_test_chain.h,v $
 *------------------------------------------------------------------
 * pas_test_chain.h - Platform independent PA test parse chains
 *
 * May 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pas_test_chain.h,v $
 * Revision 3.2.58.2  1996/08/12  16:06:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.58.1  1996/03/21  23:24:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/27  02:21:32  fsunaval
 * Branch: ELC_branch
 * 1.Port adapter specific test commands should not be in pas_show_chain.h
 *
 * Revision 3.2.2.1  1995/12/26  19:57:11  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.2  1995/11/17  18:55:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "../parser/parser_defs_parser.h"

LINK_TRANS(test_pas_extend_here, no_alt);

/*******************************************************************
 * test pas eeprom
 * 
 */
 
EOLNS    (pas_eeprom_eol, pas_eeprom);
 
KEYWORD (pas_eeprom, pas_eeprom_eol, test_pas_extend_here,
	 "eeprom", "Test eeprom functionality",
	 PRIV_ROOT);

/******************************************************************
 * test pas
 * 
 */
KEYWORD (test_pas, pas_eeprom, ALTERNATE,
	 "pas", "Port Adaptor Tests", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE test_pas

