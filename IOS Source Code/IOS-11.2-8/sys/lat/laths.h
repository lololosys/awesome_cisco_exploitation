/* $Id: laths.h,v 3.2 1995/11/17 17:34:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/laths.h,v $
 *------------------------------------------------------------------
 * $Log: laths.h,v $
 * Revision 3.2  1995/11/17  17:34:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    laths		define host system structure

    Copyright 1989-1991 Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATHS_H
#define LATHS_H		0		/* Indicate file has been read	*/
#include "latco.h"

/*----------------------------------------------------------------------
    Define the local host structure.
 -----------------------------------------------------------------------*/
    typedef struct hs
    {
	struct	hs *hs_link;		/* Link to next host structure	*/

	struct	as AS_VIEW(hs_as_head[2]);	/* AS list header	*/
	struct	ls LS_VIEW(hs_ls_head[2]);	/* LS list header	*/
	struct  hi HI_VIEW(hs_hi_head[2]);	/* HI list header	*/

	struct	nd ND_VIEW(hs_nd_hash[ND_HASHES][2]); /* ND hash table	*/
	struct	cb CB_VIEW(hs_cb_table[CB_TABLES]);   /* CB table	*/
	byte	hs_cb_seqno[CB_TABLES];	/* Circuit sequence numbers	*/
	int	hs_cb_index;		/* Current circuit index	*/
	word	hs_multicast[3];	/* LAT multicast address	*/
	word	hs_slave_mc[3];		/* LAT slave multicast address	*/
	word	hs_ptype;		/* Ethernet protocol number	*/
	byte	hs_prot_hi;		/* LAT protocol, high version	*/
	byte	hs_prot_lo;		/* LAT protocol, low  version	*/
	byte	hs_prot_ver;		/* LAT protocol version		*/
	byte	hs_prot_eco;		/* LAT protocol ECO		*/
	word	hs_facility;		/* Facility number		*/
	byte	hs_prod_code;		/* Product code			*/
	byte	hs_prod_ver;		/* Product version		*/

	int	hs_cb_maximum;		/* Maximum number of circuits	*/
	int	hs_sb_maximum;		/* Maximum number of sessions	*/
	int	hs_as_maximum;		/* Maximum advertised services	*/
	int	hs_ls_maximum;		/* Maximum learned services	*/
	int	hs_nd_maximum;		/* Maximum remote nodes		*/
	int	hs_hi_maximum;		/* Maximum HIC requests		*/

	int	hs_cb_current;		/* Current number of circuits	*/
	int	hs_sb_current;		/* Current number of sessions	*/
	int	hs_as_current;		/* Current advertised services	*/
	int	hs_ls_current;		/* Current learned services	*/
	int	hs_nd_current;		/* Current remote nodes		*/
	int	hs_hi_current;		/* Current HIC request		*/

	int	hs_cb_highest;		/* Highest number of circuits	*/
	int	hs_sb_highest;		/* Highest number of sessions	*/
	int	hs_as_highest;		/* Highest advertised services	*/
	int	hs_ls_highest;		/* Highest learned services	*/
	int	hs_nd_highest;		/* Highest remote nodes		*/
	int	hs_hi_highest;		/* Highest HIC request		*/

	word	hs_dll_max;		/* Maximum datalink output size	*/
	word	hs_inp_max;		/* Maximum datalink input  size	*/
	word	hs_status;		/* Current host status		*/

	byte	hs_cb_max_ses;		/* Maximum sessions/circuit	*/
	byte	hs_cb_s_rcv_extra;	/* Maximum extra receives (S)	*/
	byte	hs_cb_h_rcv_extra;	/* Maximum extra receives (H)	*/
	byte	hs_cb_retries;		/* Default circuit retries	*/
	byte	hs_cb_vc_timer;		/* Default circuittimer    (ms)	*/
	byte	hs_cb_hd_timer;		/* Default host delay      (ms)	*/
	byte	hs_cb_ka_timer;		/* Default keep-alive timer (s)	*/
	byte	hs_cb_progress;		/* Host progress interval   (s) */

	byte	hs_sb_max_data;		/* Maximum data slot size	*/
	byte	hs_sb_max_attn;		/* Maximum attention slot size	*/

	word	hs_hi_sts_timer;	/* HIC status  timer (s)	*/
	word	hs_hi_cmd_timer;	/* HIC command timer (h)	*/
	word	hs_hi_cmd_retries;	/* HIC command retries		*/
	word	hs_hi_req_no;		/* HIC next request number	*/

	word	hs_as_space;		/* AS message space remaining	*/
	byte	hs_as_elapse;		/* Seconds till next multicast	*/
	byte	hs_as_timer;		/* Current AS multicast timer	*/
	byte	hs_as_sequence;		/* Current AS sequence number	*/
	byte	hs_as_next_changes;	/* Changes in next AS message 	*/
	byte	hs_as_last_changes;	/* Changes in last AS message	*/
	byte	hs_as_next_rating;	/* Rating for next message	*/

	int	hs_nd_age_max;		/* Maximum remote node age	*/
	int	hs_nd_interval;		/* Interval to purge nodes	*/
	
	byte	hs_acl_group[GROUPS];	/* Groups for ACL processing	*/
	byte	hs_idl_group[GROUPS];	/* Groups for IDL processing	*/

 	byte	hs_node[ND_STR+1];	/* Local node name  string	*/
	byte	hs_ident[ID_STR];	/* Local node ident string 	*/
	byte	hs_class[SC_STR];	/* Local node services classes	*/

#if LE_HS_STATS
	co_node	hs_stats;		/* Host statistics		*/
	ch_node	hs_count;		/* Host statistics		*/
#endif
	word	hs_tail;		/* End-of-stucture		*/
   } hs_node, *hs_ptr;
#endif
