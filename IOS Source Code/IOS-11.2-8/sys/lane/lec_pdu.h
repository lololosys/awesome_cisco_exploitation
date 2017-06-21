/* $Id: lec_pdu.h,v 3.2.58.1 1996/05/09 14:33:00 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lec_pdu.h,v $
 *------------------------------------------------------------------
 * lec_pdu.h: LAN Emulation Client pdu generation externs
 *
 * Jan 1995, Brad Benson
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_pdu.h,v $
 * Revision 3.2.58.1  1996/05/09  14:33:00  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.86.1  1996/04/27  07:06:52  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.76.2  1996/04/08  14:52:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.76.1  1996/04/08  01:57:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.2  1996/04/06  02:52:35  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * In LE-ARP requests, fill in the source MAC of the originator and not
 * the MAC of client.
 *
 * Revision 3.2.64.1  1996/03/22  22:55:16  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:43:22  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:10:40  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:46:40  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:57  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:12:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/06  21:59:55  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.2  1995/06/30  22:02:22  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.1  1995/06/07  21:20:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LEC_PDU_H_
#define _LEC_PDU_H_

/*
 * Typedef used for the buginf routines.
 */
typedef enum lec_sr_t_ {
    SEND,
    RECV,
} lec_sr_t;

/*
 * Typedef for the ARP response.
 */
typedef enum lec_arp_type_t_ {
    LEC_LOCAL_ADDR,
    LEC_REMOTE_ADDR,
} lec_arp_type_t;

/*
 * BLLI definitions.
 */
#define LEC_BLLI_LENGTH		9

extern const blli_type lec_blli_control;
extern const blli_type lec_blli_ether_data;
extern const blli_type lec_blli_token_ring_data;
extern const blli_type lec_blli_ether_mcast;
extern const blli_type lec_blli_token_ring_mcast;

/*
 * ID for ILMI requests
 */
extern ilmiClient lec_ilmi_clientid;

/*
 * Extern function definitions.
 */
extern boolean		lec_listen(lec_info_t *lec_info);
extern boolean		lec_cancel_listen(lec_info_t *lec_info);
extern void		lec_dump_control(lec_sr_t send_or_recv,
					 idbtype *swidb,
					 paktype *pak,
					 vcd_t vcd);
extern lec_vc_t		*lec_find_control(lec_info_t *lec_info, 
					  void *call_id);
extern char		*lec_prsignal(int opcode);
extern boolean		lec_send_arp_req(lec_info_t *lec_info, 
					 uchar const *dest_mac,
					 uchar const *src_mac);
extern boolean		lec_send_narp_req(lec_info_t *lec_info,
					 uchar const *mac);
extern boolean		lec_send_arp_rsp(lec_info_t *lec_info, lec_arp_type_t arp_type);
extern boolean		lec_send_cfg_req(lec_info_t *lec_info);
extern boolean		lec_send_connect(lec_vc_t *dest_vc);
extern boolean		lec_send_flush_req(lec_info_t *lec_info, 
				           lec_le_arp_t *le_arp_entry);
extern boolean		lec_send_flush_rsp(lec_info_t *lec_info);
extern boolean		lec_send_get_lecs_addr(lec_info_t *lec_info);
extern boolean		lec_send_join_req(lec_info_t *lec_info);
extern boolean		lec_send_ready_query(lec_info_t *lec_info, vcd_t vcd);
extern boolean		lec_send_ready_ind(lec_info_t *lec_info, vcd_t vcd);
extern boolean		lec_send_reg_req(lec_info_t *lec_info);
extern boolean		lec_send_setup(lec_info_t *lec_info, lec_vc_t *dest_vc);
extern boolean		lec_send_add_party_rej(lec_info_t *lec_info);
extern boolean		lec_send_release(lec_vc_t *dest_vc,
					 sig_api_cause_code cause);
extern boolean		lec_send_release_comp(lec_vc_t *dest_vc,
					      sig_api_cause_code cause);
extern boolean		lec_send_topo_change(lec_info_t *lec_info,
					     boolean TC);
extern void		lec_dump_sig_msg(lec_sr_t send_or_recv,
					 sig_api_msg *sig_msg);

#endif _LEC_PDU_H_
