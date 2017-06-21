/* $Id: srb_lane.h,v 3.1.6.2 1996/07/11 00:49:36 bbenson Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_lane.h,v $
 *------------------------------------------------------------------
 * Defines for TokenRing LANE
 *
 * April 1996, Badri Ramaswamy
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb_lane.h,v $
 * Revision 3.1.6.2  1996/07/11  00:49:36  bbenson
 * CSCdi61915:  TR Lane doesnt check return value of pak_duplicate()
 * Branch: California_branch
 * Continuation of first commit.  Modified lane registries.
 *
 * Revision 3.1.6.1  1996/05/09  14:46:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.4.1  1996/04/27  07:26:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.2.1  1996/04/17  23:15:26  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 * Move contents of lec_srb* srt/srb_lane.[ch], to facilitate
 * creation of new sub-system.
 *
 * Revision 3.1  1996/04/17  23:00:14  rbadri
 * Placeholder for TokenRing LANE sub-system support.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SRT_LANE_H_
#define _SRT_LANE_H_

/*
 * LEC SRB external variables
 */

extern boolean		lec_srb_running;

/*
 * Extern function declarations.
 */


extern void             tr_lane_vidb_input(hwidbtype *idb, paktype *pak, 
			                   ushort opcode, tr_ventrytype *vre);
extern boolean          rsrb_tr_lane_oqueue(hwidbtype *idb,
			                    paktype *pak, enum HEADTAIL dummy);
extern void		tr_lane_srb_arp_req(lec_info_t *lec_info, 
					    paktype *pak);

/*
 * Registry functions for TR_LANE
 */
extern void tr_lane_start(void);

extern boolean 
tr_lane_add_vring_interface(hwidbtype *hwidb, idbtype *swidb,
			    ushort elan_rn, uchar srb_bn,
			    ushort vr_rn);

extern void
tr_lane_srb_arp_req(lec_info_t *lec_info, paktype *pak);

extern void
tr_lane_srb_enq(lec_info_t *lec_info, paktype *pak);

extern long
tr_lane_tring_getlink(paktype *pak);


extern iqueue_t
tr_lane_tring_parse_packet(hwidbtype *hwidb, 
			   paktype *pak, 
			   uchar *sniff_ptr);

extern void
tr_lane_vrforward_pak(lec_info_t *lec_info, paktype *pak);

extern boolean
lane_tr_vencap(paktype *pak, long address);

extern tr_vgrouptype *
tr_lane_vrfind_ringgroup(ushort vr_rn);

#endif _SRT_LANE_H_




