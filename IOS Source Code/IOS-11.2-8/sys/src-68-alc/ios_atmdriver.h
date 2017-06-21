/* $Id: ios_atmdriver.h,v 3.1.6.4 1996/07/02 23:44:37 sakumar Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/ios_atmdriver.h,v $
 *------------------------------------------------------------------
 * ios_atmdriver.h
 *
 * ?
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ios_atmdriver.h,v $
 * Revision 3.1.6.4  1996/07/02  23:44:37  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 3.1.6.3  1996/06/18  16:44:07  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/06/05  02:00:12  sakumar
 * Added dummy function atmSyn_verify_qos_params() and defined
 * atm->burstcells_factor to 1 to prevent Cat5k ATM module crashes.
 * This module currently does not support QOS.
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/05/09  14:41:09  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.52.1  1996/04/27  07:25:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.40.1  1996/04/08  02:08:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.24.2  1996/03/27  21:55:27  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.24.1  1996/03/22  09:43:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.2.1  1996/03/13  03:18:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1  1996/03/11  08:15:24  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define XX_ATM_SYN						0x10

#define	ATMSYN_HOLDQ_SZ					40

#define	ATMSYN_VISIBLE_BANDWIDTH_SONET 	156250

#define ATMSYN_BANDWIDTH_SONET   		64     	/* IGRP SONET bandwidth */

#define ATMSYN_SONET_DELAY       		8      	/* IGRP delay for SONET */

#define ATMSYN_RESP_TOUT				10		/* milliseconds till timeout */




/*******************************************************************
            IOS ATM Driver function prototypes
 *******************************************************************/

extern void atmSyn_subsys_init (subsystype *subsys);
extern void atmSyn_analyze_interface (int slot);
extern hwidbtype *atmSyn_init_idb (int slot, int unit);
extern void atmSyn_if_final_init (void);
extern void atmSyn_system_configured (hwidbtype *idb);
extern void atmSyn_shutdown (hwidbtype *idb, int newstate);
extern void atmSyn_line_action (hwidbtype *idb);
extern boolean atmSyn_line_state (hwidbtype *idb);
extern void atmSyn_enable (hwidbtype *idb);
extern void atmSyn_periodic (hwidbtype *idb);
extern ushort * atmSyn_sniff (hwidbtype *hwidb, paktype *pak, uchar *data,
             ushort *sniff_buffer, int offset, int num_shorts);
extern boolean atmSyn_show_controller (hwidbtype *idb, parseinfo *p);
extern void atmSyn_reset (hwidbtype *idb);
extern void atmSyn_setup (hwidbtype *idb);
extern unsigned atmSyn_encaps (atm_db_t *atmzr, paktype *pak,
                  vc_info_t *vc, ushort txopt);
extern boolean atmSyn_setup_vc (hwidbtype *idb, vc_info_t *vcinfo);
extern boolean atmSyn_teardown_vc (hwidbtype *idb, vc_info_t *vcinfo);
extern void atmSyn_raw_input (hwidbtype *idb, paktype *pak);
extern boolean atmSyn_get_vc_stats (hwidbtype *idb, vc_info_t *vc);
extern void atmSyn_output (hwidbtype *idb);
extern void atmSyn_fastsend (hwidbtype *idb, paktype *pak);
extern void atmSyn_clear_counters (hwidbtype *idb);
extern	void atmdrv_post_lec_event(void  *pMsg);
extern boolean atmSyn_set_maxdgram (hwidbtype *idb,int buffersize,int maxdgram);

extern void atmSyn_proc_input_pkt(UINT16 vcd, UINT16 event, UINT32 length, void *pData);
extern void atmSyn_config (hwidbtype *idb);
extern uint atmSyn_verify_qos_params(hwidbtype *hwidb, unsigned peakrate,
                                     unsigned avgrate, unsigned burstcells);


#define ALC_ERR_BAD_COLOR_RANGE		-1
#define ALC_ERR_PVC_NOT_AAL5SNAP 	-2
#define ALC_ERR_VLAN_ON_LANE 		-3
#define ALC_ERR_PVC_ON_OTHER_VLAN 	-4
#define ALC_ERR_OTHER 				-5

#define ALC_ERR_VLAN_NOT_PVC_SUPP	-6
#define ALC_ERR_VLAN_PVC_MISMATCH	-7

#define ALC_RET_OK					 1

/*
** alc_bind_PVC_to_vlan: Assign a PVC to a vlan
**     Error return (negative value) if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vlan already served by LANE
**     else return 1
**
**     Notes: If first PVC for VLAN, then sends port create msg to NMP.
*/
int alc_bind_PVC_to_vlan (UINT16 vlan, UINT16 vcd);

/*
** alc_unbind_PVC_from_vlan: Remove a PVC from a vlan
**     Error return 0 if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vcd not part of this vlan
**         - vlan not setup to run over PVCs
**     else return 1
**
**     Notes: If deleting last PVC for VLAN, then send port destroy msg to NMP.
*/
int alc_unbind_PVC_from_vlan (UINT16 vlan, UINT16 vcd); 

/*
** alc_get_vlan_from_PVC: Returns vlan number given PVC's vcd
**     Error return 0 if:
**         - vcd out of range
**         - vcd not setup as aal5snap
**         - vcd not bound to any vlan
**     else return vlan_number
*/
UINT16 alc_get_vlan_from_PVC (UINT16 vcd);

/*
** alc_get_PVC_from_vlan: Returns PVC vcd from vlan. If prev_vcd (2nd parameter)
**                        is zero, then the first PVC is returned. If not
**                        zero, then the next PVC after prev_vcd is returned.
**     Error return 0 if:
**         - prev_vcd out of range
**         - vlan not setup to run over PVCs
**         - prev_vcd not setup as aal5snap
**         - prev_vcd not part of vlan
**     else return vcd
*/
UINT16 alc_get_PVC_from_vlan (UINT16 vlan, UINT16 prev_vcd);

/*
** alc_is_PVC_aal5snap: Is PVC of type aal5snap?
**     return 0 if:
**         - vcd out of range
**         - vcd not of type aal5snap
**     else return 1 
*/
int alc_is_PVC_aal5snap (UINT16 vcd);

/*
** alc_is_vlan_on_PVCs: Is this VLAN configured to run over PVCs?
**     return 0 if:
**         - vlan not configured at all
**         - vlan configured to run LANE
**         - vlan not configured to run over PVCs
**     else return 1 
**
**     Note: Needs at least 1 PVC to be setup for this VLAN to return 1.
*/
int alc_is_vlan_on_PVCs (UINT16 vlan);

/*
** alc_delete_PVCs_for_vlan: Delete all PVCs for this VLAN.
**     Error return 0 if:
**         - vlan not configured at all
**         - vlan configured to run LANE
**         - vlan not configured to run over PVCs
**     else return 1 
**
**     Note: Sends port delete message to NMP too.
*/
int alc_delete_PVCs_for_vlan (UINT16 vlan);


/*
** alc_mark_PVC_as_aal5snap: Done when PVC is setup initially
**     Error return 0 if:
**         - vcd out of range
**     else return 1
**
*/
int alc_mark_PVC_as_aal5snap (UINT16 vcd);

/*
** alc_get_PVC_vlan: Get the next vlan that is configured to run over PVCs.
**     Error return 0 if:
**         - no such vlan exists
**     else return vlan_number
**
**     Notes: If parameter 0, then function returns the lowest numbered VLAN
**            that is configured to use PVCs, else it returns the next higher
**            number VLAN that is configured to run over PVCs.
*/
UINT16 alc_get_PVC_vlan (UINT16 vlan);






#define TXOPT_OAM       0x0800      /* OAM cell */

#ifdef ACP_IOS_102
typedef struct  atm_encap_ {
    vc_t            vcnum;      /* VC to xmit/rcv on */
    ushort          enctype;    /* Encapsulation type */
    uchar           data[0];
} atm_encap_t;

#define ATM_ENCAP_HDRBYTES  (sizeof(atm_encap_t))



#endif /* ACP_IOS_102 */
