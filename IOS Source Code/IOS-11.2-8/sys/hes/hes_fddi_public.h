/* $Id: hes_fddi_public.h,v 3.3 1995/11/17 09:16:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/hes_fddi_public.h,v $
 *------------------------------------------------------------------
 * High-End platforms FDDI public function declarations.
 *
 * June 1994, Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: hes_fddi_public.h,v $
 * Revision 3.3  1995/11/17  09:16:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:40:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*********************************************************************
        External Prototypes for the Cisco HE FDDI Modules.

        File:                   hes_fddi_public.h
        Created:                02/20/94

        Version:
        Last Modified:

        Modification History:
*********************************************************************/


extern void	select_endec(hwidbtype *hwidb, ulong reg, ulong endec);
extern void	select_fiber_status(hwidbtype *hwidb, ulong endec);
extern void	get_frinc_count(hwidbtype *idb);
extern ushort	get_pc_state(hwidbtype *idb, ushort entity);
extern ushort	get_pc_neighbor(hwidbtype *idb, ushort entity);
extern ushort	get_pc_withhold(hwidbtype *idb, ushort entity);
extern ushort	get_pc_conn_state(hwidbtype *idb, ushort entity);
extern void	fddi_cmt_timeout(void);
extern void	fddi_smt_love_note_handler(hwidbtype *idb, ushort status);
