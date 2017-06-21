/* $Id: pt_pad_common.h,v 3.2.62.1 1996/05/01 01:59:48 ravindra Exp $
 * $Source: /release/112/cvs/Xsys/pt/pt_pad_common.h,v $
 *------------------------------------------------------------------
 * pt_pad_common.h  Data definitions for pt_pad_.. to lat/tcp helper
 * subroutines.
 *
 * Aug, 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pt_pad_common.h,v $
 * Revision 3.2.62.1  1996/05/01  01:59:48  ravindra
 * CSCdi49877:  deleting translate with x25 pvc results in corruption crash
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:59:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:01:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void pt_pad_setup(translatetype *trans, tt_soc *tty);
boolean pt_pad_check_lci(translatetype *trans, tt_soc *tty);
void pt_pad_accept_trans(translatetype *trans, tt_soc *tty);
void pt_pad_setup_x3_options(translatetype *trans, tt_soc *tty);
void pt_pad_clear_connection(translatetype *trans, tt_soc *tty);
