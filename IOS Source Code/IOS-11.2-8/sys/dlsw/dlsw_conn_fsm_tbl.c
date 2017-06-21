/* $Id: dlsw_conn_fsm_tbl.c,v 3.12.6.7 1996/07/28 19:09:15 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_conn_fsm_tbl.c,v $
 *------------------------------------------------------------------
 * dlsw_conn_fsm_tbl.c -- DLSW core finite state machine table
 *
 * Oct 1994, G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_conn_fsm_tbl.c,v $
 * Revision 3.12.6.7  1996/07/28  19:09:15  rbatz
 * CSCdi63350:  Change to DLSW+ FSM to allow circuit to activate
 *              when CONNECT.IND is received before XIDs exchanged.
 * Branch: California_branch
 *
 * Revision 3.12.6.6  1996/07/19  22:45:33  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.12.6.5  1996/07/19  03:54:59  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.12.6.4  1996/06/18  23:09:40  ravip
 * CSCdi60356:  DLSw MIB: Incorrent value returned for circuit timestamps
 * Branch: California_branch
 *
 * Revision 3.12.6.3  1996/06/11  15:00:55  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.12.6.2  1996/05/17  10:44:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.4  1996/04/26  07:33:40  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.11.2.3  1996/04/11  14:36:41  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/03  14:00:46  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  17:37:35  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.12.6.1  1996/03/18  19:31:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.3  1996/03/16  06:38:23  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.12.2  1996/03/07  08:43:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.12.1  1996/02/20  00:45:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/06  22:59:58  ravip
 * CSCdi49441:  Number of active circuits wrong on SNMP get of certain
 * objects
 *
 * Revision 3.11  1996/02/16  02:40:30  kpatel
 * CSCdi49171:  DLSw Circuit in Ckt Established state while other
 *              router has no ckt
 *
 * Revision 3.10  1996/02/05  21:56:15  fbordona
 * CSCdi47942:  dlsw sdlc abm bit not turned off in 1st xid sent to
 *              sdlc station. Use abm default for media until xid3
 *              is received from a device.
 *
 * Revision 3.9  1996/01/31  02:14:13  snyder
 * CSCdi47864:  spellink errors
 *              non-activaton -> non-activation
 *
 * Revision 3.8  1996/01/25  22:07:58  fbordona
 * CSCdi46883:  Incorrect field in ssp message.
 *              Do not send SSP_Halt_Noack in circuit_start state.
 *
 * Revision 3.7  1995/12/23  00:53:43  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.6  1995/12/09  16:48:55  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.5  1995/11/30  21:22:39  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.4  1995/11/21  22:33:09  kpatel
 * CSCdi42929:  DLSw : Second call fails
 *
 * Revision 3.3  1995/11/17  09:02:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:22:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:54:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/25  15:34:12  fbordona
 * CSCdi41072:  Netbios sessions do not get established after power off
 *              the client.
 *              CUR_cs verify should send TEST to NULL dsap.
 *              Do not retry SSP_HALT_DL.
 *
 * Revision 2.2  1995/07/24  13:04:27  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.1  1995/06/07  20:25:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "address.h"
#include "mgd_timers.h"
#include "ieee.h"

#include "../util/fsm.h"
#include "../if/rif.h"
#include "../ibm/sna_util.h"
#include "../ibm/libibm_externs.h"
#include "../ibm/netbios.h"
#include "../cls/clsi_msg.h"
#include "../ui/debug.h"

#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_core_fsm.h"
#include "../dlsw/dlsw_dlu.h"


/*
 * File Scope Variables
 */
static char print_buffer[30];

/*
 * File Scope Function Prototypes
 */
static void dlsw_send_seg_to_disp (void *, paktype *);

/* 
 * dlsw_restart_conn_fsm
 *
 * restart conn fsm with deferred event
 */
 
void dlsw_restart_fsm (void *d, int old_state, int major, int minor)
{
    dlsw_t		*dlsw_ptr;
 
    dlsw_ptr = d;
 
    if (dlsw_ptr->magic != DLSW_MAGIC_ID) {
 	/*
 	 * During the time that the delay_event was
 	 * queued, the circuit was deleted. 
 	 */
 	if (dlsw_core_debug_msgs || dlsw_core_debug_state) {
 	    dlsw_print_event(major, minor, print_buffer);
 	    buginf("\nDLSw: No circuit for delay event:%s state:%s",
 		   print_buffer, dlsw_print_state(old_state));
 	}
    } else {
 	if (dlsw_core_debug_msgs || dlsw_core_debug_state)
 	    buginf("\nDLSw: Processing delayed event - prev state:%s",
 		   dlsw_print_state(old_state));
 	dlsw_fsm(dlsw_ptr, major, minor);
    }
    return;
}
 
/*
 * DLSw FSM : State Disconnect
 */
const static struct fsm_state ds_disconnected[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{DLSW_ACTION_a,FSM_NOARG, dlswst_loc_resolve},	/* 05 dlswmi_conn_ind      */
{DLSW_ACTION_a,FSM_NOARG, dlswst_loc_resolve},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{DLSW_ACTION_c,FSM_NOARG, dlswst_rem_resolve},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Local Initiated Resolve Pending
 */
const static struct fsm_state ds_loc_resolve_pend[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{DLSW_ACTION_7,FSM_NOARG, dlswst_disconnect},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{DLSW_ACTION_b,FSM_NOARG, dlswst_ckt_start},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_6,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Remote Initiated Resolve Pending
 */
const static struct fsm_state ds_rem_resolve_pend[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{DLSW_ACTION_7,FSM_NOARG, dlswst_disconnect},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{DLSW_ACTION_d,FSM_NOARG, dlswst_ckt_pending},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_6,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};



/*
 * DLSw FSM : State Circuit Start
 *
 * When errors are detected in this state, can not send
 * Halt_dl_noack since we do not yet know the remote dlc correlator.
 */
const static struct fsm_state ds_ckt_start[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{DLSW_ACTION_e,FSM_NOARG, dlswst_established},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_6,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Circuit Pending
 */
const static struct fsm_state ds_ckt_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{DLSW_ACTION_h,FSM_NOARG, dlswst_connect_pend},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_v,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{DLSW_ACTION_x,FSM_NOARG, dlswst_established},	/* 15 dlswmi_wan_reach_ack */
{DLSW_ACTION_u,FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_5,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Circuit Established
 */
const static struct fsm_state ds_ckt_established[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{DLSW_ACTION_h,FSM_NOARG, dlswst_connect_pend},	/* 05 dlswmi_conn_ind      */
{DLSW_ACTION_f,FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{DLSW_ACTION_t,FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_v,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{DLSW_ACTION_u,FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{DLSW_ACTION_g,FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{DLSW_ACTION_i,FSM_NOARG, dlswst_contact_pend},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{DLSW_ACTION_p,FSM_NOARG, dlswst_close_pend},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Contact Pending
 */
const static struct fsm_state ds_contact_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{DLSW_ACTION_j,FSM_NOARG, dlswst_connected},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{DLSW_ACTION_t,FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_v,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{DLSW_ACTION_u,FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Connect Pending
 */
const static struct fsm_state ds_connect_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{DLSW_ACTION_t,FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_v,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{DLSW_ACTION_u,FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{DLSW_ACTION_k,FSM_NOARG, dlswst_connected},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_w,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Connected
 */
const static struct fsm_state ds_connected[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_r,FSM_NOARG, dlswst_halt_pend_noack},/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_o,FSM_NOARG, dlswst_halt_pend_noack},/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{DLSW_ACTION_1,FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{DLSW_ACTION_l,FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{DLSW_ACTION_t,FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{DLSW_ACTION_s,FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_n,FSM_NOARG, dlswst_disc_pending},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{DLSW_ACTION_u,FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{DLSW_ACTION_2,FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{DLSW_ACTION_m,FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{DLSW_ACTION_o,FSM_NOARG, dlswst_halt_pending},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_o,FSM_NOARG, dlswst_halt_pend_noack},/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Disconnect Pending
 */
const static struct fsm_state ds_disc_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_q,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{DLSW_ACTION_q,FSM_NOARG, dlswst_close_pend},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{DLSW_ACTION_3, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{DLSW_ACTION_q,FSM_NOARG, dlswst_close_pend},	/* 24 dlswmi_wan_dl_halted */
{DLSW_ACTION_q,FSM_NOARG, dlswst_close_pend},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_q,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};



/*
 * DLSw FSM : State Halt Pending
 */
const static struct fsm_state ds_halt_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_p,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{DLSW_ACTION_p,FSM_NOARG, dlswst_close_pend},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, dlswst_halt_pend_noack},/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{DLSW_ACTION_p,FSM_NOARG, dlswst_close_pend},	/* 10 dlswmi_disc_cnf      */
{DLSW_ACTION_4,FSM_NOARG, dlswst_close_pend},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_p,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Halt Pending No Ack
 */
const static struct fsm_state ds_halt_pend_noack[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 00 dlswmi_admin_stop   */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_z,FSM_NOARG, dlswst_close_pend},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Close Pending
 */
const static struct fsm_state ds_close_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{DLSW_ACTION_y,FSM_NOARG, dlswst_disconnect},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{DLSW_ACTION_y,FSM_NOARG, dlswst_disconnect},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{DLSW_ACTION_y,FSM_NOARG, dlswst_disconnect},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Restart Pending
 */
const static struct fsm_state ds_restart_pending[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};


/*
 * DLSw FSM : State Restarted
 */
const static struct fsm_state ds_ckt_restart[] =
{
/*  Action     Param Blk   Next State              Index  Decoded Input   */
/* ---------------------------------------------------------------------- */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 00 dlswmi_admin_stop   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 01 dlswmi_admin_error  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 02 dlswmi_admin_wan_fail*/

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 03 dlswmi_opn_stn_cnf   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 04 dlswmi_conn_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 05 dlswmi_conn_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 06 dlswmi_id_ind        */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 07 dlswmi_data_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 08 dlswmi_udata_ind     */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 09 dlswmi_flow_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 10 dlswmi_disc_cnf      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 11 dlswmi_disc_ind      */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 12 dlswmi_close_stn_cnf */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 13 dlswmi_wan_canureach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 14 dlswmi_wan_icanreach */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 15 dlswmi_wan_reach_ack */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 16 dlswmi_wan_dgmframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 17 dlswmi_wan_xidframe  */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 18 dlswmi_wan_contact   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 19 dlswmi_wan_contacted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 20 dlswmi_wan_restart_dl*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 21 dlswmi_wan_dl_restarted*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 22 dlswmi_wan_infoframe */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 23 dlswmi_wan_halt_dl   */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 24 dlswmi_wan_dl_halted */
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 25 dlswmi_wan_halt_dl_noack*/
{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 26 dlswmi_wan_ifcm      */

{FSM_NOACTION, FSM_NOARG, FSM_NOCHANGE},	/* 27 dlswmi_timer_expired */
/* ----------------------------------------------------------------------- */
};

static void dlsw_purge_reach (dlsw_t *dlsw_ptr)
{
    u_portid_type_t prp_id;
 
    prp_id.peer_id = dlsw_ptr->peer_handle;
    if (dlsw_ptr->my_ckt) 
        delete_xidfailure_mac_entry(dlsw_ptr->dlc_id.target_mac_address,
                                    &prp_id);
    else
        delete_xidfailure_mac_entry(dlsw_ptr->dlc_id.origin_mac_address,
                                    &prp_id);

    prp_id.port_id = dlsw_ptr->port_id_dgra;
    if (dlsw_ptr->my_ckt)
        delete_xidfailure_mac_entry(dlsw_ptr->dlc_id.origin_mac_address,
                                    &prp_id);
    else
        delete_xidfailure_mac_entry(dlsw_ptr->dlc_id.target_mac_address,
                                    &prp_id);
 
}

static ushort dlsw_close_station (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dp.type = D_CLOSESTN_REQ;
    if (dlsw_ptr->p_cep_id != 0) {
        dp.disp_closestn_req.identifierType = CLS_P_CEP_ID;
        dp.disp_closestn_req.identifier = dlsw_ptr->p_cep_id;
    } else {
        dp.disp_closestn_req.identifierType = CLS_U_CEP_ID;
        dp.disp_closestn_req.identifier = dlsw_ptr->u_cep_id;
    }
    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


static ushort dlsw_cleanup_circuit (dlsw_t *dlsw_ptr)
{
    dlsw_ptr->magic = 0;
    dlsw_ptr->u_cep_id = 0;
    dlsw_ptr->p_cep_id = 0;
    mgd_timer_stop(&dlsw_ptr->timer);

    STATE_D("\nDLSw: %u to dead queue",
            dlsw_ptr->our_dlc_corr);

    p_unqueue(&dlsw_hashQ[dlsw_ptr->bucket], dlsw_ptr);
    p_enqueue(&dlsw_deadQ, dlsw_ptr);
    --dlsw_active_circuits;
    
    /*
     * Notify the peer module that the circuit is no longer
     * using the peer. Used for dynamic-peer and
     * peer-on-demand management.
     */
    if (dlsw_ptr->peer_handle)
        peer_delete_circuit(dlsw_ptr->peer_handle);

    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


ushort dlsw_action_a (dlsw_t *dlsw_ptr)
{
    union D_primitives dp;

    STATE_D("\nDLSw: core: dlsw_action_a()");

    /*
     * Can't process the received clsi_msg until circuit is in
     * the ckt_established state, so save it.
     */
    dlsw_ptr->pending_clsi_msg = dlsw_ptr->clsi_msg;
    dlsw_ptr->clsi_msg = NULL;

    /* Give ReqOpnStn.Cfm extra time to come back */
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT_2);
    
    dp.type = D_REQ_OPNSTN_REQ;
    dp.disp_reqopenstn_req.uCepId = (uint) dlsw_ptr;

    if (dlsw_ptr->my_ckt) {
        ieee_copy(dlsw_ptr->dlc_id.origin_mac_address,
                  dp.disp_reqopenstn_req.remoteAddr);
        ieee_copy(dlsw_ptr->dlc_id.target_mac_address,
                  dp.disp_reqopenstn_req.localAddr);
        dp.disp_reqopenstn_req.lSap = dlsw_ptr->dlc_id.target_link_sap;
        dp.disp_reqopenstn_req.rSap = dlsw_ptr->dlc_id.origin_link_sap;
    } else {
        ieee_copy(dlsw_ptr->dlc_id.origin_mac_address,
                  dp.disp_reqopenstn_req.localAddr);
        ieee_copy(dlsw_ptr->dlc_id.target_mac_address,
                  dp.disp_reqopenstn_req.remoteAddr);
        dp.disp_reqopenstn_req.rSap = dlsw_ptr->dlc_id.target_link_sap;
        dp.disp_reqopenstn_req.lSap = dlsw_ptr->dlc_id.origin_link_sap;
    }
    dp.disp_reqopenstn_req.rifLength = RIF_LENGTH(dlsw_ptr->local_rif[0]);
    dp.disp_reqopenstn_req.rif = dlsw_ptr->local_rif;
    dp.disp_reqopenstn_req.options = EO_RIF_SPECIFIED;
    dp.disp_reqopenstn_req.p_sap_id = dlsw_ptr->port_id_dgra->p_sap_id;

    fsm_to_disp(&dp);

    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_b()
 *
 * Received a request open station confirm.
 * Send CUR_cs.
 */
ushort dlsw_action_b (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_b()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT_2);
    dlsw_ptr->p_cep_id = 
             ((ReqOpnStnCnf_t *)dlsw_ptr->clsi_msg)->fCLSIHdr.fPCepId;

    dlsw_send_ssp(dlsw_ptr, SSP_OP_canureach, NULL, 0);

    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_c()
 *
 * Received a CUR_cs.
 * Send Request Open Station Request.
 */
ushort dlsw_action_c (dlsw_t *dlsw_ptr)
{
    ssp_hdr_t *ssp_hdr;
    union D_primitives dp;

    STATE_D("\nDLSw: core: dlsw_action_c()");
    /* Give ReqOpnStn.Cfm extra time to come back */
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT_2);

    /*
     * Learn remote circuit correlators.
     */
    ssp_hdr = (ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart;
    dlsw_ptr->their_port_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_dlc_port_id : ssp_hdr->target_dlc_port_id;
    dlsw_ptr->their_dlc_corr =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
                ssp_hdr->origin_dlc_corr : ssp_hdr->target_dlc_corr;
    dlsw_ptr->their_transport_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_transport_id: ssp_hdr->target_transport_id;

    dp.type = D_REQ_OPNSTN_REQ;
    dp.disp_reqopenstn_req.uCepId = (uint) dlsw_ptr;

    if (dlsw_ptr->my_ckt) {
        ieee_copy(dlsw_ptr->dlc_id.origin_mac_address,
                  dp.disp_reqopenstn_req.remoteAddr);
        ieee_copy(dlsw_ptr->dlc_id.target_mac_address,
                  dp.disp_reqopenstn_req.localAddr);
        dp.disp_reqopenstn_req.lSap = dlsw_ptr->dlc_id.target_link_sap;
        dp.disp_reqopenstn_req.rSap = dlsw_ptr->dlc_id.origin_link_sap;
    } else {
        ieee_copy(dlsw_ptr->dlc_id.origin_mac_address,
                  dp.disp_reqopenstn_req.localAddr);
        ieee_copy(dlsw_ptr->dlc_id.target_mac_address,
                  dp.disp_reqopenstn_req.remoteAddr);
        dp.disp_reqopenstn_req.rSap = dlsw_ptr->dlc_id.target_link_sap;
        dp.disp_reqopenstn_req.lSap = dlsw_ptr->dlc_id.origin_link_sap;
    }
    dp.disp_reqopenstn_req.rifLength = RIF_LENGTH(dlsw_ptr->local_rif[0]);
    dp.disp_reqopenstn_req.rif = dlsw_ptr->local_rif;
    dp.disp_reqopenstn_req.options = EO_RIF_SPECIFIED;
    dp.disp_reqopenstn_req.p_sap_id = dlsw_ptr->port_id_dgra->p_sap_id;

    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_d()
 *
 * Received Request Open Station Confirm.
 * Send ICR_cs.
 */
ushort dlsw_action_d (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_d()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);
    dlsw_ptr->p_cep_id =
             ((ReqOpnStnCnf_t *)dlsw_ptr->clsi_msg)->fCLSIHdr.fPCepId;

    dlsw_send_ssp(dlsw_ptr, SSP_OP_icanreach, NULL, 0);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_e()
 *
 * Received an ICR_cs.
 * Send Reach_Ack.
 */
ushort dlsw_action_e (dlsw_t *dlsw_ptr)
{
    ssp_hdr_t *ssp_hdr;

    STATE_D("\nDLSw: core: dlsw_action_e()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_XID_TIMEOUT);

    /*
     * Learn remote circuit correlators.
     */
    ssp_hdr = (ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart;
	
    dlsw_ptr->their_port_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_dlc_port_id : ssp_hdr->target_dlc_port_id;
    dlsw_ptr->their_dlc_corr =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
                ssp_hdr->origin_dlc_corr : ssp_hdr->target_dlc_corr;
    dlsw_ptr->their_transport_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_transport_id: ssp_hdr->target_transport_id;

    dlsw_send_ssp(dlsw_ptr, SSP_OP_reach_ack, NULL, 0);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_f()
 *
 * DLC XID processing
 */
ushort dlsw_action_f (dlsw_t *dlsw_ptr)
{
    XID3type *xid;
    ulong xid_len;
    ushort cls_op;
    IdStnInd_t *id_stn;
    IdInd_t *id_ind;
    IdCnf_t *id_cnf;

    STATE_D("\nDLSw: core: dlsw_action_f()");
    if (!(mgd_timer_running(&dlsw_ptr->timer)) )
          mgd_timer_start(&dlsw_ptr->timer, DLSW_XID_TIMEOUT);

    cls_op = CLSIMsgGetPrimitiveCode(dlsw_ptr->clsi_msg);
    if (cls_op == (CLS_ID_STN | CLS_INDICATION)) {
        id_stn = (IdStnInd_t*) dlsw_ptr->clsi_msg;
        xid_len = id_stn->fXIDLength;
        xid = (XID3type *) (((uchar *) id_stn) + id_stn->fXIDOffset);
        dlsw_ptr->xid_state = dlsw_xid_ind_rcvd;
        dlsw_ptr->llc_flags.cmd_rsp = DLSW_LLC_CMD;
        dlsw_ptr->llc_flags.poll_final = 0;
    } else if (cls_op == (CLS_ID | CLS_INDICATION)) {
        id_ind = (IdInd_t *) dlsw_ptr->clsi_msg;
        xid_len = id_ind->fXIDLength;
        xid = (XID3type *) (((uchar *) id_ind) + id_ind->fXIDOffset);
        dlsw_ptr->xid_state = dlsw_xid_ind_rcvd;
        dlsw_ptr->llc_flags.cmd_rsp = DLSW_LLC_CMD;
        dlsw_ptr->llc_flags.poll_final = 0;
    } else if (cls_op == (CLS_ID | CLS_CONFIRM)) {
        id_cnf = (IdCnf_t *) dlsw_ptr->clsi_msg;
        xid_len = id_cnf->fXIDLength;
        xid = (XID3type *) (((uchar *) id_cnf) + id_cnf->fXIDOffset);
        dlsw_ptr->xid_state = dlsw_xid_reset;
        dlsw_ptr->llc_flags.cmd_rsp = DLSW_LLC_RSP;
        dlsw_ptr->llc_flags.poll_final = 0;
    } else {
        STATE_D("\nDLSw: dl_action_f(): bogus cls op %x", cls_op);
        return(FSM_OK);
    }

    /*
     * Learn the max recievable i-field, max i-frame (DLC modulo)
     * without acknowledgement, and ABM capabilities so we can echo
     * them back to the lan station when we "pass through" XID3s
     * end to end.
     */
    if (xid_len >= sizeof(XID3type)) {
        dlsw_learn_xid_parms(xid, &dlsw_ptr->abm_state,
                             &dlsw_ptr->max_i_field_len,
                             &dlsw_ptr->window_size);
    }

    /*
     * Finally, forward the XID onto the peer.
     */
    dlsw_send_ssp(dlsw_ptr, SSP_OP_xidframe, (uchar *) xid, xid_len);

    dlsw_ptr->llc_flags.cmd_rsp = 0;
    dlsw_ptr->llc_flags.poll_final = 0;
    return(FSM_OK);
}


/*
 * dlsw_action_g()
 *
 * WAN XID processing
 */
ushort dlsw_action_g (dlsw_t *dlsw_ptr)
{
    XID3type *xid;
    ulong xid_len;
    union D_primitives dp;

    uchar    cmdrsp;

    STATE_D("\nDLSw: core: dlsw_action_g()");
    if (!(mgd_timer_running(&dlsw_ptr->timer)) )
          mgd_timer_start(&dlsw_ptr->timer, DLSW_XID_TIMEOUT);

    xid_len = (ulong)
              ((ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart)->ssp_msg_len;
    xid = (XID3type *)
           ((ssp_ctl_t *) dlsw_ptr->wan_pak->datagramstart)->data;
    cmdrsp = ((ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart)->ssp_llc_cr;

    /*
     * Spoof 'em if you got 'em.
     */
    if (xid_len >= sizeof(XID3type)) {
        if (dlsw_ptr->port_id_dgra->port_type == CLS_SDLC) {
            dlsw_spoof_xid_parms(xid, dlsw_ptr->abm_state,
                                 DLSW_ABM_INCAPABLE,
                                 dlsw_ptr->max_i_field_len,
                                 dlsw_ptr->window_size);
        } else {
            dlsw_spoof_xid_parms(xid, dlsw_ptr->abm_state,
                                 DLSW_ABM_CAPABLE,
                                 dlsw_ptr->max_i_field_len,
                                 dlsw_ptr->window_size);
        }

    }

    /*
     * Forward the XID onto the target lan.
     */
    if ((peer_get_xid_op_support(dlsw_ptr->peer_handle)) && (cmdrsp)) {
        if (cmdrsp == DLSW_LLC_CMD) {
            dp.type = D_ID_REQ;
            dlsw_ptr->xid_state = dlsw_xid_req_sent;
        } else {    /* DLSW_LLC_RSP */
            dp.type = D_ID_RSP;
            dlsw_ptr->xid_state = dlsw_xid_reset;
        }
    } else {
        if ((dlsw_ptr->xid_state == dlsw_xid_reset) ||
            (dlsw_ptr->xid_state == dlsw_xid_req_sent)) {
            dp.type = D_ID_REQ;
            dlsw_ptr->xid_state = dlsw_xid_req_sent;
        } else { /* (xid_state == dlsw_xid_ind_rcvd) */
            dp.type = D_ID_RSP;
            dlsw_ptr->xid_state = dlsw_xid_reset;
        }
    }
    dp.disp_id_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_id_req.xidData = (uchar *) xid;
    dp.disp_id_req.xidLength = xid_len;
    fsm_to_disp(&dp);

    return(FSM_OK);
}


/*
 * dlsw_action_h()
 *
 * Received a connect indication.
 */
ushort dlsw_action_h (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_h()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dlsw_send_ssp(dlsw_ptr, SSP_OP_contact, NULL, 0);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_i()
 *
 * Received a WAN Contact.
 * Send DLC Connect Request.
 */
ushort dlsw_action_i (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_i()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dp.type = D_CONNECT_REQ;
    dp.disp_connect_req.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_j()
 *
 * Received a connect confirm.
 * Send WAN Contacted and UNBUSY DLC.
 */
ushort dlsw_action_j (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_j()");
    mgd_timer_stop(&dlsw_ptr->timer);
    dlsw_ptr->xid_state = dlsw_xid_reset;

    dlsw_send_ssp(dlsw_ptr, SSP_OP_contacted, NULL, 0);

    /*
     * Release the CEP from the BUSY state.
     */
    dp.type = D_FLOW_REQ;
    dp.disp_flow_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_flow_req.action = CLS_ACTION_RESUME_FLOW;
    fsm_to_disp(&dp);

    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_k()
 *
 * Received a WAN Contacted.
 * Send DLC Connect Response.
 */
ushort dlsw_action_k (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_k()");
    mgd_timer_stop(&dlsw_ptr->timer);
    dlsw_ptr->xid_state = dlsw_xid_reset;

    dp.type = D_CONNECT_RSP;
    dp.disp_connect_req.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_l()
 *
 * Received DLC Data Indication
 * Send WAN Info.
 */
ushort dlsw_action_l (dlsw_t *dlsw_ptr)
{
    DataInd_t *data_ind;

    STATE_D("\nDLSw: core: dlsw_action_l()");

    data_ind = (DataInd_t*) dlsw_ptr->clsi_msg;
    if (data_ind->fDataLength == 0) {
        STATE_D("\nDLSw: core: received data.ind of length zero");
        return(FSM_OK);
    }

    dlsw_send_ssp(dlsw_ptr, SSP_OP_infoframe,
                  data_ind->fData + data_ind->fReserveLength,
                  data_ind->fDataLength);

    return(FSM_OK);
}


/*
 * dlsw_action_m()
 *
 * Received WAN Infoframe
 * Send DLC Data Frame(s).
 */
ushort dlsw_action_m (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_m()");

    dlsw_ptr->wan_pak->datagramsize = ((ssp_hdr_t *)
               dlsw_ptr->wan_pak->datagramstart)->ssp_msg_len;
    dlsw_ptr->wan_pak->datagramstart += sizeof(ssp_i_frame_t);

    /*
     * If the current i-field size (payload_len) exceeds the max
     * receievable i-field of the destination lan station then
     * segment it.
     *
     * Note: do not have to consider the media header since the
     *       XID3 max i-field len does not include the media header.
     */
    if ((dlsw_sna_spoof_and_seg()) && (dlsw_ptr->max_i_field_len) &&
        (dlsw_ptr->wan_pak->datagramsize > dlsw_ptr->max_i_field_len)) {
	queuetype q;

        if (sna_th_frag(dlsw_ptr->wan_pak, dlsw_ptr->max_i_field_len, &q)) {
            sendqueue(&q, dlsw_send_seg_to_disp, dlsw_ptr);
        } else {
	datagram_done(dlsw_ptr->wan_pak);
	    dlsw_delayed_event(dlsw_restart_fsm, 
			       dlsw_ptr, dlsw_ptr->state,
			       dlswma_admin,
			       dlswmi_admin_stop);
        }
    } else {
        /* dlsw_send_seg_to_disp() consumes dlsw_ptr->pak */
        dlsw_send_seg_to_disp(dlsw_ptr, dlsw_ptr->wan_pak);
    }
    dlsw_ptr->wan_pak = NULL;
    return(FSM_OK);
}


static void dlsw_send_seg_to_disp (void *controlblock, paktype *pak)
{
    union D_primitives dp;
    dlsw_t      *dlsw_ptr = (dlsw_t *) controlblock;

    dp.type = D_DATA_REQ;
    dp.disp_data_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_data_req.theData = pak->datagramstart;
    dp.disp_data_req.dataLength = pak->datagramsize;
    dp.disp_data_req.pak = pak;
    fsm_to_disp(&dp);
}


/*
 * dlsw_action_n()
 *
 * Received DLC Disconnect Indication
 * Send WAN Halt_dl
 */
ushort dlsw_action_n (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_n()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dlsw_send_ssp(dlsw_ptr, SSP_OP_halt_dl, NULL, 0);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_o()
 *
 * Received WAN Halt_dl or WAN Failure.
 * Send DLC Disc Request
 */
ushort dlsw_action_o (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_o()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dp.type = D_DISCONNECT_REQ;
    dp.disp_connect_req.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_p()
 *
 * Received DLC Disconnect Confirm 
 *             or
 * Received a WAN HALT_DL when we are not connected
 *
 * Send WAN HALTED and DLC CLose Station Request
 */
ushort dlsw_action_p (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_p()");

    dlsw_send_ssp(dlsw_ptr, SSP_OP_dl_halted, NULL, 0);
    return(dlsw_close_station(dlsw_ptr));
}


/*
 * dlsw_action_q()
 *
 * Received WAN Dl_halted
 * Send DLC Disc Response and DLC Close Station Request
 */
ushort dlsw_action_q (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_q()");

    dp.type = D_DISCONNECT_RSP;
    dp.disp_connect_req.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);
    return(dlsw_close_station(dlsw_ptr));
}

/*
 * dlsw_action_r()
 *
 * Received Admin Stop after connected
 * Send DLC Disc Request and WAN Halt_dl_noack
 */
ushort dlsw_action_r (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_r()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dlsw_send_ssp(dlsw_ptr, SSP_OP_halt_dl_noack, NULL, 0);

    dp.type = D_DISCONNECT_REQ;
    dp.disp_connect_req.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_s()
 *
 * Received DLC Flow Control Indication
 */
ushort dlsw_action_s (dlsw_t *dlsw_ptr)
{
    char *str;

    STATE_D("\nDLSw: core: dlsw_action_s()");
    str = string_getnext();
    FLOW_D("\nDLSw-FC : Received %s",
           CLSIMsgSprintfShort(dlsw_ptr->clsi_msg, str));

    /*
     * If DLC is congested, reduce next window.
     */
    if (dlsw_ptr->fc_llc_flowlevel & DLC_CONGESTION_MAX) {
        dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_HWO;
    } else if (dlsw_ptr->fc_llc_flowlevel & DLC_CONGESTION_HIGH) {
        dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_DWO;
    }

    dlsw_ptr->fc_llc_flowlevel = ((FlowIndh_t *)
                                 dlsw_ptr->clsi_msg)->fFlowLevel;
    return(FSM_OK);
}

/*
 * dlsw_action_t()
 *
 * Received DLC UDATA Indication (NetBIOS)
 * Send SSP_OP_dgmframe.
 */
ushort dlsw_action_t (dlsw_t *dlsw_ptr)
{
    u_portid_type_t prp_id;
    netbios_header *nb_hdr;
    UDataInd_t *udata_ind;

    STATE_D("\nDLSw: core: dlsw_action_t()");

    /*
     * If the UDATA frame is netbios name recognized
     * then inform csm so it can update cache entries
     * and cleanup name recognized pending db
     */
    udata_ind = (UDataInd_t *) dlsw_ptr->clsi_msg;
    nb_hdr = (netbios_header *) udata_ind->fUDataPtr;
    if (nb_hdr->command == NETBIOS_NAME_RECOGNIZED) {
        prp_id.port_id = dlsw_ptr->port_id_dgra;
        nbnr_recd_update_info(LOCAL, nb_hdr->srcname,
                              nb_hdr->destname, &prp_id,
                              dlsw_ptr->local_rif);
    }

    dlsw_send_ssp(dlsw_ptr, SSP_OP_dgmframe,
                  udata_ind->fUDataPtr, udata_ind->fUDataLength);

    return(FSM_OK);
}


/*
 * dlsw_action_u()
 *
 * Received WAN SSP_OP_dgmframe (NetBIOS)
 * Send DLC UDATA Request
 */
ushort dlsw_action_u (dlsw_t *dlsw_ptr)
{
    uchar *udata_ptr;
    ulong udata_len;
    u_portid_type_t prp_id;
    netbios_header *nb_hdr;
    union D_primitives dp;

    STATE_D("\nDLSw: core: dlsw_action_u()");

    udata_len = (ulong)
              ((ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart)->ssp_msg_len;
    udata_ptr = (uchar *)
              ((ssp_ctl_t *) dlsw_ptr->wan_pak->datagramstart)->data;

    /*
     * If the WAN SSP datagram frame is a netbios name recognized
     * frame then inform csm so it can update cache entries
     * and cleanup name recognized pending db
     */
    nb_hdr = (netbios_header *)udata_ptr;
    if (nb_hdr->command == NETBIOS_NAME_RECOGNIZED) {
      prp_id.peer_id = dlsw_ptr->peer_handle;
      nbnr_recd_update_info(REMOTE,
                            nb_hdr->srcname,
                            nb_hdr->destname,
                            &prp_id,
                            NULL);
    }

    dp.type = D_UDATA_REQ;
    dp.disp_udata_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_udata_req.theUData = udata_ptr;
    dp.disp_udata_req.udataLength = udata_len;
    fsm_to_disp(&dp);

    return(FSM_OK);
}


/*
 * dlsw_action_v()
 *
 * Received a Disc.Ind when we are not connected.
 * Send SSP Halt Pending No Ack
 * Send Disc.Rsp and then Close_Stn.Req
 */
ushort dlsw_action_v (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_v()");

    dlsw_purge_reach(dlsw_ptr);
    dlsw_send_ssp(dlsw_ptr, SSP_OP_halt_dl_noack, NULL, 0);
    return(dlsw_close_station(dlsw_ptr));
}


/*
 * dlsw_action_w()
 *
 * Used to close down a circuit after it's established
 * (other station needs halt_noack).
 */
ushort dlsw_action_w (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_w()");

    dlsw_send_ssp(dlsw_ptr, SSP_OP_halt_dl_noack, NULL, 0);
    return(dlsw_close_station(dlsw_ptr));

}


/*
 * dlsw_action_x()
 *
 * Received a Reach_Ack.
 * Just learn remote circuit correlators.
 */
ushort dlsw_action_x (dlsw_t *dlsw_ptr)
{
    ssp_hdr_t *ssp_hdr;

    STATE_D("\nDLSw: core: dlsw_action_x()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT_2);

    ssp_hdr = (ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart;
    dlsw_ptr->their_port_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_dlc_port_id : ssp_hdr->target_dlc_port_id;
    dlsw_ptr->their_dlc_corr =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
                ssp_hdr->origin_dlc_corr : ssp_hdr->target_dlc_corr;
    dlsw_ptr->their_transport_id =
               (ssp_hdr->ssp_dirn == SSP_DIRN_FORWARD) ?
               ssp_hdr->origin_transport_id: ssp_hdr->target_transport_id;
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
    return(FSM_OK);
}


/*
 * dlsw_action_y()
 *
 * Takes care of admin cleanup after receiving CloseStn.cfn
 * Also good for handling timeouts before a cep is created.
 */
ushort dlsw_action_y (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_y()");

    return(dlsw_cleanup_circuit(dlsw_ptr));
}

/*
 * dlsw_action_z()
 *
 * Used to close down a circuit before it's established
 * (ie. before reach_ack is sent or received).
 */
ushort dlsw_action_z (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_z()");
    return(dlsw_close_station(dlsw_ptr));
}




/*******************************************************************
 *
 *            FSM Administation Structures/Routines
 *
 ********************************************************************/
 
#define DLSW_FSM_DECODE FSM_DECODE_ROUTINE(dlsw_fsm_decode)

const static struct finite_state_machine dlsw_fsm_table[] =
{
/*  State                   	Decoder        */
 { ds_disconnected,	      DLSW_FSM_DECODE },
 { ds_loc_resolve_pend,	      DLSW_FSM_DECODE },
 { ds_rem_resolve_pend,	      DLSW_FSM_DECODE },
 { ds_ckt_start,              DLSW_FSM_DECODE },
 { ds_ckt_pending,            DLSW_FSM_DECODE },
 { ds_ckt_established,        DLSW_FSM_DECODE },
 { ds_contact_pending,        DLSW_FSM_DECODE },
 { ds_connect_pending,        DLSW_FSM_DECODE },
 { ds_connected,              DLSW_FSM_DECODE },
 { ds_disc_pending,	      DLSW_FSM_DECODE },
 { ds_halt_pending,    	      DLSW_FSM_DECODE },
 { ds_halt_pend_noack,	      DLSW_FSM_DECODE },
 { ds_close_pending,	      DLSW_FSM_DECODE },
 { ds_restart_pending,        DLSW_FSM_DECODE },
 { ds_ckt_restart,            DLSW_FSM_DECODE },
};


int dlsw_fsm_decode (dlsw_t *dlsw_ptr, int major, int minor)
{
    int index;

    switch (major) {

      case dlswma_admin:

        switch (minor) {
          case dlswmi_admin_stop:
            index = 0;
            break;

          case dlswmi_admin_error:
            index = 1;
            break;

          case dlswmi_admin_wan_fail:
            index = 2;
            break;

          default:
            index = FSM_FAULT;
            break;
        }
        break;

      case dlswma_dlc:

        switch (minor) {
          case CLS_REQ_OPNSTN | CLS_CONFIRM:
	    index = 3;		 
	    break;

	  case CLS_CONNECT | CLS_CONFIRM:
          case CLS_CONNECTED | CLS_INDICATION:
	    index = 4;		 
	    break;

	  case CLS_CONNECT | CLS_INDICATION:
      	  case CLS_CONNECT_STN | CLS_INDICATION:
	    index = 5;		 
	    break;

	  case CLS_ID_STN | CLS_INDICATION:
	  case CLS_ID | CLS_CONFIRM:
	  case CLS_ID | CLS_INDICATION:
	    index = 6;		 
	    break;

	  case CLS_DATA | CLS_INDICATION:
	    index = 7;		 
	    break;

	  case CLS_UDATA | CLS_INDICATION:
	    index = 8;		 
	    break;

	  case CLS_FLOW | CLS_INDICATION:
	    index = 9;		 
	    break;

	  case CLS_DISCONNECT | CLS_CONFIRM:
	    index = 10;		 
	    break;

	  case CLS_DISCONNECT | CLS_INDICATION:
	    index = 11;		 
	    break;

	  case CLS_CLOSE_STN | CLS_CONFIRM:
	    index = 12;		 
	    break;

          default:
            index = FSM_FAULT;
            break;
        }
        break;

      case dlswma_wan:

        switch (minor) {
          case dlswmi_wan_canureach:
            index = 13;
            break;

          case dlswmi_wan_icanreach:
            index = 14;
            break;

          case dlswmi_wan_reach_ack:
            index = 15;
            break;

          case dlswmi_wan_dgmframe:
            index = 16;
            break;

          case dlswmi_wan_xidframe:
            index = 17;
            break;

          case dlswmi_wan_contact:
            index = 18;
            break;

          case dlswmi_wan_contacted:
            index = 19;
            break;

          case dlswmi_wan_restart_dl:
            index = 20;
            break;

          case dlswmi_wan_dl_restarted:
            index = 21;
            break;

          case dlswmi_wan_infoframe:
            index = 22;
            break;

          case dlswmi_wan_halt_dl:
            index = 23;
            break;

          case dlswmi_wan_dl_halted:
            index = 24;
            break;

          case dlswmi_wan_halt_dl_noack:
            index = 25;
            break;

          case dlswmi_wan_ifcm:
            index = 26;
            break;

          default:
            index = FSM_FAULT;
            break;

        }
        break;

      case dlswma_timer:

        switch (minor) {
          case dlswmi_timer_expired:
            index = 27;
            break;

          default:
            index = FSM_FAULT;
            break;

        }
	break;

      default:
        index = FSM_FAULT;
        break;
    }
    return (index);
}


/*
 * d l s w _ f s m
 */
ushort dlsw_fsm (dlsw_t *dlsw_ptr, dlswma_t major, int minor)
{
    int         fsm_rc;
    int         old_state;

    old_state = dlsw_ptr->state;

    if (dlsw_core_debug_state) {
        dlsw_print_event(major, minor, print_buffer);
        buginf("\nDLSw: START-FSM (%u): event:%s state:%s",
               dlsw_ptr->our_dlc_corr,
               print_buffer, dlsw_print_state(old_state));
    }

    fsm_rc = (int) fsm_execute ("DLSW-CORE", dlsw_fsm_table,
                                ((int *) &(dlsw_ptr->state)), dlswst_fsm_max,
                                dlsw_ptr, major, minor, FALSE);
    if (dlsw_core_debug_state) {
        buginf("\nDLSw: END-FSM (%u): state:%s->%s\n",
               dlsw_ptr->our_dlc_corr,
               dlsw_print_state(old_state), 
               dlsw_print_state(dlsw_ptr->state));
    }

    if (fsm_rc < 0) {
      switch (fsm_rc) {
      case FSM_RC_ERROR_INPUT:
      case FSM_RC_INVALID_INPUT:
        STATE_D("\nInvalid input"); break;
      case FSM_RC_UNKNOWN_STATE:
        STATE_D("\nIllegal state"); break;
      case FSM_RC_UNKNOWN_INPUT:
        STATE_D("\nIllegal input"); break;
      case FSM_RC_UNEXPECTED_INPUT:
        STATE_D("\nUnexpected input"); break;
      default:
        STATE_D("\nunknown error rtn code"); break;
      }
    }
    return (fsm_rc);
}


/*
 * dlsw_action_1()
 *
 * DLC XID processing in connected state - Check for non-activation XID
 */
ushort dlsw_action_1 (dlsw_t *dlsw_ptr)
{
    XID3type *xid;
    ulong xid_len;
    ushort cls_op;
    IdInd_t *id_ind;
    IdCnf_t *id_cnf;

    STATE_D("\nDLSw: core: dlsw_action_1()");

    cls_op = CLSIMsgGetPrimitiveCode(dlsw_ptr->clsi_msg);
    if (cls_op == (CLS_ID | CLS_INDICATION)) {
        id_ind = (IdInd_t *) dlsw_ptr->clsi_msg;
        xid_len = id_ind->fXIDLength;
        xid = (XID3type *) (((uchar *) id_ind) + id_ind->fXIDOffset);
    } else if (cls_op == (CLS_ID | CLS_CONFIRM)) {
        id_cnf = (IdCnf_t *) dlsw_ptr->clsi_msg;
        xid_len = id_cnf->fXIDLength;
        xid = (XID3type *) (((uchar *) id_cnf) + id_cnf->fXIDOffset);
    } else {
        STATE_D("\nDLSw: dl_action_1(): bogus cls op %x", cls_op);
        return(FSM_OK);
    }

    if (!xid_is_nonactivation((uchar *)xid)) {
      STATE_D("\nDLSw: dlsw_action_1(): Not a non-activation XID");
      return(FSM_OK);
    }

    if (cls_op == (CLS_ID | CLS_INDICATION))
        dlsw_ptr->xid_state = dlsw_xid_ind_rcvd;
    else
        dlsw_ptr->xid_state = dlsw_xid_reset;

    /*
     * Finally, forward the XID onto the peer.
     */
    dlsw_send_ssp(dlsw_ptr, SSP_OP_xidframe, (uchar *) xid, xid_len);
    return(FSM_OK);
}


/*
 * dlsw_action_2()
 *
 * WAN XID processing in connected state - Check for non-activation XID
 */
ushort dlsw_action_2 (dlsw_t *dlsw_ptr)
{
    XID3type *xid;
    ulong xid_len;
    union D_primitives dp;

    STATE_D("\nDLSw: core: dlsw_action_2()");

    xid_len = (ulong)
              ((ssp_hdr_t *) dlsw_ptr->wan_pak->datagramstart)->ssp_msg_len;
    xid = (XID3type *)
           ((ssp_ctl_t *) dlsw_ptr->wan_pak->datagramstart)->data;

    if (!xid_is_nonactivation((uchar *)xid)) {
      STATE_D("\nDLSw: dlsw_action_2(): Not a non-activation XID");
      return(FSM_OK);
    }

    /*
     * Forward the XID onto the target lan.
     */
    if ((dlsw_ptr->xid_state == dlsw_xid_reset) ||
        (dlsw_ptr->xid_state == dlsw_xid_req_sent)) {
        dp.type = D_ID_REQ;
        dlsw_ptr->xid_state = dlsw_xid_req_sent;
    } else { /* (xid_state == dlsw_xid_ind_rcvd) */
        dp.type = D_ID_RSP;
        dlsw_ptr->xid_state = dlsw_xid_reset;
    }
    dp.disp_id_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_id_req.xidData = (uchar *) xid;
    dp.disp_id_req.xidLength = xid_len;
    fsm_to_disp(&dp);

    return(FSM_OK);
}

/*
 * dlsw_action_3()
 *
 * WAN Halt DL processed in disconnected pending state.
 * This occurs in a race condition scenario, when both
 * end stations issue DISC simultaneously.
 * Send a DL HALTED SSP message.
 */
ushort dlsw_action_3 (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_3()");

    dlsw_send_ssp(dlsw_ptr, SSP_OP_dl_halted, NULL, 0);
    return (FSM_OK);
}

/*
 * dlsw_action_4()
 *
 * Received DLC Disconnect Indication
 * Send WAN HALTED,
 * Send DLC Disc Response and DLC CLose Station Request
 */
ushort dlsw_action_4 (dlsw_t *dlsw_ptr)
{
    union D_primitives  dp;

    STATE_D("\nDLSw: core: dlsw_action_4()");
    mgd_timer_start(&dlsw_ptr->timer, DLSW_GENERAL_TIMEOUT);

    dlsw_send_ssp(dlsw_ptr, SSP_OP_dl_halted, NULL, 0);

    dp.type = D_DISCONNECT_RSP;
    dp.disp_disconnect_rsp.pCepId = dlsw_ptr->p_cep_id;
    fsm_to_disp(&dp);

    return(dlsw_close_station(dlsw_ptr));
}


/*
 * dlsw_action_5()
 *
 * Timeout in the ckt_established state.
 * Clear reachability cache for this mac address. Used in cases
 * when there are duplicate TICs/VTAMs and we have reachability for a 
 * TIC/VTAM that is inactive.
 *
 * Other station needs halt_noack.
 */
ushort dlsw_action_5 (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_5()");
 
    dlsw_purge_reach(dlsw_ptr);
    dlsw_send_ssp(dlsw_ptr, SSP_OP_halt_dl_noack, NULL, 0);
    return(dlsw_close_station(dlsw_ptr));
}
 
/*
 * dlsw_action_6()
 *
 * Timeout in the rem_resolve_pend state.
 * Clear reachability cache for this mac address. Used in cases
 * where connecting to VDLC resources on the router like
 * APPN/DSPU 
 *
 */
ushort dlsw_action_6 (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_6()");
 
    dlsw_purge_reach(dlsw_ptr);
    return(dlsw_close_station(dlsw_ptr));
}
 
/*
 * dlsw_action_7()
 *
 * Takes care of admin_error cleanup after when in loc_resolve
 * or rem_resolve. Error was most likely UNKNOWN_SAP from VDLC
 * port. Purge reachability to give other paths a chance.
 */
ushort dlsw_action_7 (dlsw_t *dlsw_ptr)
{
    STATE_D("\nDLSw: core: dlsw_action_7()");

    dlsw_purge_reach(dlsw_ptr);
    return(dlsw_cleanup_circuit(dlsw_ptr));
}
