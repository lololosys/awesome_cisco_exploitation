/* $Id: parser_defs_atm_dxi.h,v 3.1 1996/02/01 05:14:29 hampton Exp $
 * $Source: /release/111/cvs/Xsys/wan/parser_defs_atm_dxi.h,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_atm_dxi.h,v $
 * Revision 3.1  1996/02/01  05:14:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:47  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:49:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:28:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* atm-dxi command identifiers.
 */
enum DXI_COMMANDS {
	ATM_DXI_ZAP = 1,
	ATM_DXI_DFA,
	ATM_DXI_PVC,
	ATM_DXI_MAP
};

enum DXI_ENCAPS_TYPE {
	ATM_DXI_VC_SNAP = 1,
	ATM_DXI_VC_NLPID,
	ATM_DXI_VC_MUX
};

/* ATM DXI show commands */
enum {
    SHOW_ATM_DXI_MAP,
    SHOW_ATM_DXI_PVC,
};

