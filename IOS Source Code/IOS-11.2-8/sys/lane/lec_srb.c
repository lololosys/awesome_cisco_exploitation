/* $Id: lec_srb.c,v 3.1.20.1 1996/05/09 14:33:03 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_srb.c,v $
 *------------------------------------------------------------------
 * lec_srb.c: Token Ring LAN Emulation Client
 *
 * February 1996, Duane DeCapite
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_srb.c,v $
 * Revision 3.1.20.1  1996/05/09  14:33:03  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.58.1  1996/04/27  07:06:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.46.4  1996/04/17  23:15:00  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.1.46.3  1996/04/13  19:58:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Removed duplicate definition of lec_srb_running, and call
 * lane_data_vencap through the registry.  On our way back to making this
 * file modular.
 *
 * Revision 3.1.46.2  1996/04/09  02:17:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.1.46.1  1996/04/08  14:52:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.44.1  1996/04/08  08:07:41  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.1  1996/02/26  18:39:18  ddecapit
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




















