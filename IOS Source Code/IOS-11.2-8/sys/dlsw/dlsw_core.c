/* $Id: dlsw_core.c,v 3.21.6.21 1996/09/09 17:39:19 fbordona Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_core.c,v $
 *------------------------------------------------------------------
 * It processes all CLSI messages and SSP messages.
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * All CLSI messages are fed thru this module to 
 * classify and route to either CSM ( contextless) or fsm
 * Similarly all SSP from peer messages are fed thru this module to
 * classify and route to CSM or fsm.
 * Includes all the housekeeping routines, background processes and so on.
 *------------------------------------------------------------------
 * $Log: dlsw_core.c,v $
 * Revision 3.21.6.21  1996/09/09  17:39:19  fbordona
 * CSCdi66251:  Filtering at DLSw layer will not work on border router
 * Branch: California_branch
 *
 * Revision 3.21.6.20  1996/09/03  22:24:32  kpatel
 * CSCdi67883:  DLSw lite requires flow control to throttle back end
 *              station traffic
 * Branch: California_branch
 *
 * Revision 3.21.6.19  1996/09/03  18:19:11  akhanna
 * CSCdi64842:  Router sends packet to non existent ring
 * Branch: California_branch
 *
 * Revision 3.21.6.18  1996/08/28  12:40:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.21.6.17  1996/08/26  15:05:04  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.21.6.16  1996/08/21  22:18:46  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.21.6.15  1996/08/19  14:02:54  rbatz
 * CSCdi62416:  DLSw / QLLC: Inconsistent LF field in the RIF.
 * Branch: California_branch
 *
 * Revision 3.21.6.14  1996/08/10  01:03:36  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.21.6.13  1996/08/09  23:41:47  ravip
 * CSCdi50688:  dlsw netbios slow session bring up with peer on demand
 * Branch: California_branch
 *
 * Revision 3.21.6.12  1996/07/23  22:09:27  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 *              Using Registry stub.
 * Branch: California_branch
 *
 * Revision 3.21.6.11  1996/07/23  13:21:23  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.21.6.10  1996/07/19  03:55:08  athippes
 * CSCdi60758:  dlsw : duplicate circuit identifier
 * Branch: California_branch
 *
 * Revision 3.21.6.9  1996/07/09  05:49:14  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.21.6.8  1996/06/25  04:59:16  schiang
 * CSCdi61128:  dlsw_parse_if_cmds() doesnt check NCIA
 * Branch: California_branch
 *
 * Revision 3.21.6.7  1996/06/18  23:09:50  ravip
 * CSCdi60356:  DLSw MIB: Incorrent value returned for circuit timestamps
 * Branch: California_branch
 *
 * Revision 3.21.6.6  1996/06/18  00:01:59  kpatel
 * CSCdi60179:  dlsw icannotreach lsap is broken
 * Branch: California_branch
 *
 * Revision 3.21.6.5  1996/06/11  15:01:02  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.21.6.4  1996/06/02  05:09:25  schiang
 * CSCdi57445:  DLSw+ local switch doesnt block traffic from RSRB to
 * non-NCIA IFs
 * Branch: California_branch
 *
 * Revision 3.21.6.3  1996/05/29  22:50:51  rbatz
 * CSCdi55749:  Multiple "qllc dlsw..." commands do not work.
 * Branch: California_branch
 *
 * Revision 3.21.6.2  1996/05/17  10:44:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.17.2.20  1996/05/14  23:59:29  ioakley
 * CSCdi57686:  Streamline core_to_csm processing by replacing the
 *              core_to_csm function with inline calls to clsi/ssp/dlx
 *              handler routines from the dlsw_core.c module.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.19  1996/05/14  03:20:01  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.18  1996/05/10  15:00:40  ioakley
 * CSCdi57325:  Remove dlsw reliance on port_number/slot_number
 *              and identify all dlsw ports by associated swidb.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.17  1996/05/02  22:47:11  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.16  1996/05/02  15:07:43  sbales
 * CSCdi52483:  dlsw memory leak on session startup failure
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.15  1996/05/01  22:14:51  pmorton
 * CSCdi56307:  Primary link stations fail to send SNRMs
 * Branch: IbuMod_Calif_branch
 * Fix regression from CSCdi54558. Only test target ring
 * if interface is capable of SRB.
 *
 * Revision 3.17.2.14  1996/04/30  19:30:29  schiang
 * CSCdi56215:  dlsw local switch structure correpted in dlsw_cantput()
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.13  1996/04/26  15:17:12  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.12  1996/04/26  07:33:46  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.17.2.11  1996/04/25  23:09:37  ioakley
 * CSCdi54558:  Check for virtual ring before invoking dlsw_enable_port.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.10  1996/04/25  04:27:51  ioakley
 * CSCdi55673:  Reduce number of calls on DLSw LAN side and remove
 *              functions that are no longer useful or necessary.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.9  1996/04/24  06:30:11  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.17.2.8  1996/04/20  02:07:58  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.7  1996/04/15  00:08:42  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.17.2.6  1996/04/05  06:06:05  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.17.2.5  1996/04/05  00:31:23  kpatel
 * CSCdi52168:  DLSw+ crash on configuring dlsw bridge-group with debug on
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.4  1996/04/03  14:00:52  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.17.2.3  1996/03/28  08:19:58  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Revision 3.21.6.1  1996/03/18  19:31:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.2.4  1996/03/16  06:38:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.14.2.3  1996/03/13  01:19:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.14.2.2  1996/03/07  08:43:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.14.2.1  1996/02/20  00:45:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.21  1996/03/06  23:00:04  ravip
 * CSCdi49441:  Number of active circuits wrong on SNMP get of certain
 * objects
 *
 * Revision 3.20  1996/03/05  23:56:20  kpatel
 * CSCdi50720:  SNA session hangs under heavy traffic
 *
 * Revision 3.19  1996/02/29  04:07:56  kpatel
 * CSCdi50323:  DLSw+/LLC2 shows BUSY even though permitted units are
 *              not zero
 *
 * Revision 3.18  1996/02/28  09:32:44  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.17  1996/02/26  19:54:49  fbordona
 * CSCdi49900:  DLSw should increment circuit flow control window faster
 *
 * Revision 3.16  1996/02/21  03:35:51  fbordona
 * CSCdi48915:  DLSw+ send frames out out sequence.
 *              Fix deadlock when permitted count reaches 0.
 *
 * Revision 3.15  1996/02/07  21:42:08  ravip
 * CSCdi46935:  DLSw does not propogate largest frame bits correctly
 *
 * Revision 3.14  1996/01/18  22:24:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/15  02:52:05  fbordona
 * CSCdi46810:  Alignment errors caused by DLSW routines
 *
 * Revision 3.12  1995/12/21  00:36:00  kpatel
 * CSCdi45362:  DLSw: Netbios call doesnt go thru for DLSw over
 *              FrameRelay
 *
 * Revision 3.11  1995/12/15  17:18:09  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.17.2.2  1996/03/19  18:36:28  fbordona
 * CSCdi51832:  DLSw: p_unqueue didnt find 179780 in queue 5091C
 *              Create dlsw_delay_eventQ.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.2.1  1996/03/17  17:37:39  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.2.1  1995/12/17  09:50:23  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.9  1995/12/09  16:49:00  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.8  1995/12/01  23:38:21  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.7  1995/11/30  21:22:44  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.6  1995/11/29  22:06:15  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.5  1995/11/29  19:43:27  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.4  1995/11/20  23:25:39  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.3  1995/11/17  09:02:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:22:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/11/08  20:54:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.19  1995/10/23  19:29:52  fbordona
 * CSCdi42615:  DLSw sap prioritization ignores source mac address
 *
 * Revision 2.18  1995/10/19  08:08:10  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.17  1995/10/18  23:03:52  fbordona
 * CSCdi42436:  DLSw sap prioritization for dmac only works in one
 *              direction.  Map DLSw origin/target macs to token ring
 *              source/destination macs depending on which DLSw peer
 *              owns the circuit.
 *
 * Revision 2.16  1995/09/07  04:37:48  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.15  1995/09/01  15:58:36  ppearce
 * CSCdi39719:  CIP support for DLSw+ (TCP encapsulation only)
 *
 * Revision 2.14  1995/08/24  03:27:30  kpatel
 * CSCdi39082:  DLSw flow control out of sync causing session lock-up
 *
 * Revision 2.13  1995/08/07  05:31:01  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.12  1995/08/02  20:40:14  fbordona
 * CSCdi38108:  Memory leak during biu segmentation
 *
 * Revision 2.11  1995/07/31  18:58:40  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.10  1995/07/25  17:40:10  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Incorporate Code reivew comment
 *
 * Revision 2.9  1995/07/24  13:04:59  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.8  1995/07/19  21:00:42  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add registry service for callback to get QLLC's Activate Ring CLSI
 * message.
 * Move code around for default Activate ring, so that it doesn't fall in
 * the qllc path.
 *
 * Revision 2.7  1995/06/28  18:49:21  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.6  1995/06/26  18:48:20  kpatel
 * CSCdi36367:  DLSw+ circuit entry lookup slow with large number of
 * circuits
 *
 * Revision 2.5  1995/06/21  08:54:26  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/19  23:58:40  rnaderi
 * CSCdi36049:  mallocs need to check return code
 *
 * Revision 2.3  1995/06/18  21:21:11  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:03:46  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:26:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  I n c l u d e  F i l e s
 */
#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_core.c"	/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "subsys.h"
#include "mgd_timers.h"

#include "../if/network.h"
#include "../if/tring.h"
#include "../if/if_vidb.h"
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../ibm/netbios.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "../ibm/sdlc.h"
#include "../util/fsm.h"
#include "../srt/srt_registry.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../cls/dlc_registry.h"

#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_local_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_core_fsm.h"
#include "../dlsw/dlsw_dlu.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_debug.h"



/*
 * Globals
 */
queuetype       dlsw_ctlQ;
queuetype       dlsw_ringQ;
queuetype       dlsw_bgroupQ;
boolean		dlsw_running = FALSE;
boolean		dlsw_started = FALSE; /* has DLSw ever been started */
int             dlsw_circuit_creates = 0;
int             dlsw_active_circuits = 0;

queuetype dlsw_hashQ[NDLSW_HASH_SIZE];
queuetype dlsw_deadQ;

/*
 * dlsw_delay_eventQ holds dlsw_delay_t stuctures for circuits
 * that require an event to be processed in the background.
 */
static queuetype dlsw_delay_eventQ;


/*
 * File Scope Data
 */
static int      dlsw_msg_pid;
static int      dlsw_back_pid;
static boolean  dlsw_core_keepalive_flag;
static mgd_timer 	dlsw_master_timer;


/*
 * File Scope Function Prototypes
 */
static void dlsw_pre_proc_clsi (TCLSIMsg* clsi_msg);
static void dlsw_core_proc_clsi (dlsw_t *dlsw_ptr, TCLSIMsg* clsi_msg);
static void dlsw_init_clsi_op_proc (TCLSIMsg* clsi_msg);
static void dlsw_mark_peerckts(peerhandle_t peer_handle, ulong cong_level);
static ClsUSapIdT dlsw_default_get_sap(hwidbtype *idb);

/***********************************************************************
 *                    Utility Functions
 ***********************************************************************/

boolean dlsw_is_init_clsi_op (ushort cls_op)
{
    int i;

    for (i=0; i < NDLSW_INIT_CLSI_OP; i++) {
        if (cls_op == init_clsi_op_tbl[i])
            return TRUE;
    }
     return FALSE;
}


boolean dlsw_is_cstm_clsi_msg (TCLSIMsg *clsi_msg)
{
    int i;
    ushort cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);

    for (i=0; i < NDLSW_CSTM_CLSI_OP; i++) {
        if (cls_op == cstm_clsi_op_tbl[i])
            return(TRUE);
    }
    return FALSE;
}


boolean dlsw_is_cstm_ssp_op (ssp_hdr_t *ssp_ptr)
{
    ssp_op_t ssp_op;
    ssp_op = ssp_ptr->ssp_msg_type;

    if (is_ssp_csm_op(ssp_op)) {
        if ((ssp_op == SSP_OP_icanreach) &&
            (!(ssp_ptr->ssp_flags & SSP_FLAGS_EX) ))
            return FALSE;
        else
            return TRUE;
    }
    return FALSE;
}


char *dlsw_sspop2str (uint op)
{
    switch(op) {
    case SSP_OP_NONE            :  return(" NONE ");
    case SSP_OP_canureach       :  return(" CUR  ");
    case SSP_OP_icanreach       :  return(" ICR  ");
    case SSP_OP_reach_ack       :  return(" ACK  ");
    case SSP_OP_dgmframe        :  return(" DGM  ");
    case SSP_OP_xidframe        :  return(" XID  ");
    case SSP_OP_contact         :  return(" CONQ ");
    case SSP_OP_contacted       :  return(" CONR ");
    case SSP_OP_restart_dl      :  return(" RSTQ ");
    case SSP_OP_dl_restarted    :  return(" RSTR ");
    case SSP_OP_infoframe       :  return(" INFO ");
    case SSP_OP_halt_dl         :  return(" HLTQ ");
    case SSP_OP_dl_halted       :  return(" HLTR ");
    case SSP_OP_netbios_nq      :  return(" NBNQ ");
    case SSP_OP_netbios_nr      :  return(" NBNR ");
    case SSP_OP_dataframe       :  return(" DATA ");
    case SSP_OP_halt_dl_noack   :  return(" HLTN ");
    case SSP_OP_netbios_anq     :  return(" NBAQ ");
    case SSP_OP_netbios_anr     :  return(" NBAR ");
    case SSP_OP_test_ckt_req    :  return(" TSTQ ");
    case SSP_OP_test_ckt_rsp    :  return(" TSTR ");
    case SSP_OP_ifcm            :  return(" IFCM ");
    case SSP_OP_capability_xchg :  return(" CAPX ");
    case SSP_OP_keepalive       :  return(" KEEP ");
    default:
        return("UNKNOWN");
    }
}


boolean dlsw_is_ssp_ctl_mac_hdr(dlsw_t *dlsw_ptr, ssp_op_t  ssp_op)
{
    if (is_ssp_ctl_mac_hdr(ssp_op)) {
        if (ssp_op == SSP_OP_dgmframe) {
            if ( (dlsw_ptr->dlc_id.origin_link_sap == SAP_NETBIOS) &&
                (dlsw_ptr->dlc_id.target_link_sap == SAP_NETBIOS) )
                return TRUE;
            else
                return FALSE;
        }
        else
            return TRUE;
    }
    else
        return FALSE;
}


static dlsw_t *remdlc2dlsw (uint rem_dlc)
{
    dlsw_t *dlsw_ptr;

    dlsw_ptr = (dlsw_t *) rem_dlc;
    if (dlsw_ptr && (dlsw_ptr == (dlsw_t *) dlsw_ptr->u_cep_id))
        return(dlsw_ptr);
    else
        return(NULL);
}


static dlsw_t *clsi2dlsw (TCLSIMsg *clsi_msg)
{
    dlsw_t *dlsw_ptr;

    if ((clsi_msg->fIdentifierType == CLS_U_CEP_ID) &&
        (clsi_msg->fIdentifier != CLS_NO_CEP)) {
        dlsw_ptr = (dlsw_t *) clsi_msg->fIdentifier;
        if ((dlsw_ptr == (dlsw_t *) dlsw_ptr->u_cep_id) &&
            (dlsw_ptr->magic == DLSW_MAGIC_ID))
            return(dlsw_ptr);
    }
    return(NULL);
}


dlsw_ring_t *usapid2dgra (uint u_sap_id)
{
  dlsw_ring_t *dlsw_ring_tmp;

  for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
       dlsw_ring_tmp; dlsw_ring_tmp = dlsw_ring_tmp->next)  {
      if (dlsw_ring_tmp == (dlsw_ring_t *) u_sap_id)
          return dlsw_ring_tmp;
  }
  return(NULL);
}


dlsw_ring_t *dlsw_get_ring_frpeer(idbtype *swidb)
{
  dlsw_ring_t *dlsw_ring_tmp;

  if (swidb == NULL)
    return(NULL);

  for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
      dlsw_ring_tmp; dlsw_ring_tmp = dlsw_ring_tmp->next ) {
      if (dlsw_ring_tmp->swidb == swidb)
          return(dlsw_ring_tmp);
  }
  return(NULL);
}


dlsw_bgroup_t *dlsw_get_bgroup (ushort span_index)
{
dlsw_bgroup_t *dlsw_bgroup;

  for (dlsw_bgroup = (dlsw_bgroup_t *)dlsw_bgroupQ.qhead;
      dlsw_bgroup; dlsw_bgroup = dlsw_bgroup->next ) {
      if (dlsw_bgroup->span_index == span_index)
          return(dlsw_bgroup);
  }

  return NULL;

}

 
dlsw_ring_t *dlsw_get_ring(CLSPortID *port_id, ushort span_index)
{
  idbtype       *sw_idb;
  ushort        port_type;
  dlsw_ring_t   *dlsw_ring_tmp;

  sw_idb = CLSIPortIdGetSwIdb(port_id);
  if (sw_idb == NULL)
    return(NULL);

  port_type   = CLSIPortIdGetPortType(port_id);
  for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
      dlsw_ring_tmp; dlsw_ring_tmp = dlsw_ring_tmp->next ) {

      if ((port_type == CLS_TBRIDGE) &&
            (dlsw_ring_tmp->port_type == CLS_TBRIDGE)) {
          if (dlsw_ring_tmp->trans_bridge_group == span_index)
              return(dlsw_ring_tmp);
      }

      if ((port_type == CLS_VDLC) &&
            (dlsw_ring_tmp->port_type == CLS_VDLC)) {
          if (srbV.srb_sb_get_targetring(sw_idb) == dlsw_ring_tmp->ring_group)
              return(dlsw_ring_tmp);
      }

      if (dlsw_ring_tmp->swidb == sw_idb)
          return(dlsw_ring_tmp);
  }
  return(NULL);
}


dlsw_sap_t *dlsw_get_sap(dlsw_ring_t *dlsw_ring_tmp, uchar sap)
{
  dlsw_sap_t *dlsw_sap_tmp;

  for ( dlsw_sap_tmp = (dlsw_sap_t *)((dlsw_ring_tmp->dlsw_sapQ).qhead);
        dlsw_sap_tmp; dlsw_sap_tmp = dlsw_sap_tmp->next ) {
        if (dlsw_sap_tmp->sap == sap)
           return dlsw_sap_tmp;
  }
  return(NULL);
}


static void dlsw_enable_port (CLSPortID *port_id, ushort span_index)
{
    union D_primitives dp;
    dlsw_ring_t *dlsw_ring_tmp;

    /* 
     * For QLLC, always do the enable because QLLC requires DLSw
     * to send multiple activate rings for each "qllc dlsw ..."
     * configured. For CLS_LLC, the srb-triple of bridge group may
     * have changed. In this case, we will record the new parameters
     * in send_activate_ring(), but we will not send the activate ring
     * message if a parameter was changed for a previously enabled port.
     */
    dlsw_ring_tmp = dlsw_get_ring(port_id, span_index);
    if ((dlsw_ring_tmp) && (dlsw_ring_tmp->enabled == TRUE) &&
        (dlsw_ring_tmp->port_type != CLS_QLLC) &&
	(dlsw_ring_tmp->port_type != CLS_LLC)) {
        if (dlsw_core_debug_state)
            buginf("\nDLSw-CORE: %s already enabled", 
                   dlsw_ring_tmp->swidb->hwptr->hw_namestring);
        return;
    }

    if (dlsw_core_debug_state) {
      if (port_id->fTypeInfo.swidb != NULL)
        buginf("\nDLSw-CORE : Sending enable port to %s",
               port_id->fTypeInfo.swidb->hwptr->hw_namestring);
      else
        /* For tbridge port, swidb is NULL. Just indicate sending
           enable port since in future other swidbs could be NULL */
        buginf("\nDLSw-CORE : Sending enable port");
    }

    dp.type = D_ENABLE_REQ;
    dp.disp_enable_req.port_type = CLSIPortIdGetPortType(port_id);
    dp.disp_enable_req.swidb     = port_id->fTypeInfo.swidb;
    dp.disp_enable_req.span_index = span_index;
    fsm_to_disp(&dp);
}


static void dlsw_disable_port (CLSPortID *port_id)
{
  union D_primitives dp;
  dlsw_ring_t *dlsw_ring_tmp;
  ushort span_index;
  
  if (CLSIPortIdGetPortType(port_id) == CLS_TBRIDGE)
      span_index = port_id->fTypeInfo.swidb->span_index;
  else
      span_index = 0;
  dlsw_ring_tmp = dlsw_get_ring(port_id, span_index);

  if (!dlsw_ring_tmp) {
      if (dlsw_core_debug_state)
          buginf("\nDLSw-CORE : dlsw_disable_port(): %s not in list",
                 port_id->fTypeInfo.swidb->hwptr->hw_namestring);
      return;
  }

  if (dlsw_ring_tmp->enabled == FALSE) {
      if (dlsw_core_debug_state)
          buginf("\nDLSw-CORE : dlsw_disable_port(): %s not enabled",
                 port_id->fTypeInfo.swidb->hwptr->hw_namestring);
      return;
  }

  if (dlsw_core_debug_state)
      buginf("\nDLSw-CORE : sending deactivate ring to %s",
             port_id->fTypeInfo.swidb->hwptr->hw_namestring);

  dp.type = D_DEACTIVATERING_REQ;
  dp.disp_deactivatering_req.u_sap_id = (uint) dlsw_ring_tmp;
  dp.disp_deactivatering_req.p_sap_id = dlsw_ring_tmp->p_sap_id;
  fsm_to_disp(&dp);
}


/*
 * Attach/Detach bridge-group to/from DLSw. Only bridge-groups are handled
 * here because of its uniqueness that the swidb is NULL and not known
 * until enable confirm comes back and the need to keep track of span_index
 */
void dlsw_set_bgroup_params (hwidbtype *idb, boolean sense, ushort span_index)
{
    CLSPortID  port_id;

    port_id.type = CLS_P_TYPE_SWIDB;
    port_id.fTypeInfo.swidb = firstsw_or_null(idb);

    if (sense) {
        dlsw_enable_port(&port_id, span_index);
    } else {
        dlsw_disable_port(&port_id);
    }
}


/*
 * Attach/Detach interface and DLSw.
 *
 * Note: If the interface is capable of doing SRB, and its
 * target ring is not a virtual ring, then we don't need to
 * (and probably shouldn't) attach to DLSw
 */
static void dlsw_set_srb_params (hwidbtype *idb, boolean sense)
{
    idbtype    *swidb;
    CLSPortID  port_id;

    port_id.type = CLS_P_TYPE_SWIDB;
    port_id.fTypeInfo.swidb = swidb = firstsw_or_null(idb);

    if (sense) {
       if (is_srb_hw(idb) &&
           vrfind_ringgroup(srbV.srb_sb_get_targetring(swidb)) == NULL) {
        return;
       }
	dlsw_enable_port(&port_id, 0);
    } else {
        dlsw_disable_port(&port_id);
    }
}

/*
 * This routine was born due to the fact that we use some of the
 * RSRB commands. Initialize based upon the interface sub command.
 *
 * For TR
 *      Covered here
 * For Ethernet
 *      dlsw bridge-group 1 100 1000
 *      forces the lpeer to be in existance.
 *      So, we are covered.
 * For SDLC
 *      Covered here.
 *      For all sdlc idbs, if an sdlc address is assigned to dlsw
 *      then call dlsw_set_srb_params(hwidbtype *idb, boolean sense)
 *      to enable port, etc...
 *
 * For QLLC
 *      We have to go back and get the CLSI Messages to activate rings
 *      Once the port enable completes.
 *      This is done with the registry service.
 *
 * For VDLC
 *	We have to go through all the ring groups to make sure that
 *	the bridging SAP is enabled
 *
 * For NCIA
 *      Covered here
 */
static void dlsw_parse_if_cmds (void)
{
    hwidbtype *idb;
    idbtype   *swidb;
    int i;
    tr_vgrouptype	*vrg;

    FOR_ALL_HWIDBS(idb) {
        if (is_tokenring(idb) || is_cip_lan(idb) || is_fddi(idb)) {
            idbtype *swidb = firstsw_or_null(idb);
            srb_triplet_t srb_triplet;
            srbV.srb_sb_get_triplet(swidb, &srb_triplet);
            if ((srb_triplet.localRing  == SRB_INVALID_RING_NUM) ||
                (srb_triplet.bridgeNum  == SRB_INVALID_BRIDGE_NUM) ||
                (srb_triplet.remoteRing == SRB_INVALID_RING_NUM)) {
                    continue;
            }
            dlsw_set_srb_params(idb, TRUE);
        } else if (idb->enctype == ET_SDLC) {
            for (i = 1; i <= SDLC_MAX_ADDRESS; i++) {
                if (IsDlswSdb(idb->sdlc_data[i]))  {
                    dlsw_set_srb_params(idb, TRUE);
                    break;
                }
            }
        }
    }
    FOR_ALL_RING_GROUPS(vrg) {
	reg_invoke_vdlc_default_port(vrg, vrg->vr_vmac, TRUE);
    }

    /* NCIA has vidb only */
    swidb = NULL;
    while ((swidb = get_next_vidb(swidb)))
	if (swidb->hwptr->enctype == ET_NDLC)
            dlsw_set_srb_params(swidb->hwptr, TRUE);
}

/* This routine exists so that we support the following sequence of
 * configuration
 *
 * int tokenring 0
 *  source-bridge 200 1 2000
 *
 * source-bridge ring-group 2000
 *
 */
static void dlsw_parse_srb_if (boolean sense)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
        if (is_tokenring(idb) || is_cip_lan(idb) || is_fddi(idb)) {
            idbtype *swidb = firstsw_or_null(idb);
            srb_triplet_t srb_triplet;
            srbV.srb_sb_get_triplet(swidb, &srb_triplet);
            if ((srb_triplet.localRing  == SRB_INVALID_RING_NUM) ||
                (srb_triplet.bridgeNum  == SRB_INVALID_BRIDGE_NUM) ||
                (srb_triplet.remoteRing == SRB_INVALID_RING_NUM)) {
                    continue;
            }
            dlsw_set_srb_params(idb, TRUE);
        }
    }
}


/*
 * dlsw_ports_disable()
 *   Used to deactivate and disable all ports attached to DLSw+. Called
 *   when "no dlsw local" configuration command is entered.
 */
void dlsw_ports_disable(void)
{
dlsw_ring_t *dlsw_ring_tmp;

  for (dlsw_ring_tmp = (dlsw_ring_t *)dlsw_ringQ.qhead;
       dlsw_ring_tmp; dlsw_ring_tmp = dlsw_ring_tmp->next ) {

      /* Frame relay ports are disabled on removal of remote peers */
      /* and they deactivate sap instead of deactivate ring. So    */
      /* don't do it here.                                         */
      if ((dlsw_ring_tmp->enabled) && 
          (dlsw_ring_tmp->port_type != CLS_FRAME_RELAY)) {
          dlsw_set_srb_params(dlsw_ring_tmp->swidb->hwptr, FALSE);
      }
  }

}


static ushort dlsw_get_hash_bucket (uchar *mac_a, uchar *mac_b,
                                    uchar sap_a, uchar sap_b)
{
   ushort ii, n = 0;

   for (ii = 0; ii < IEEEBYTES; ii++) {
       n += mac_a[ii];
       n += mac_b[ii];
   }
   n += sap_a;
   n += sap_b;
   n &= NDLSW_HASH_MASK;
   return (n);
}


static dlsw_t *dlsw_find_ckt (uchar *mac_a, uchar *mac_b,
                              uchar sap_a, uchar sap_b)
{
    dlsw_t *dlsw_ptr;
    ushort bucket;

    bucket = dlsw_get_hash_bucket(mac_a, mac_b, sap_a, sap_b);
    dlsw_ptr = (dlsw_t *) dlsw_hashQ[bucket].qhead;
    for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
        if (!bcmp(dlsw_ptr->dlc_id.origin_mac_address,
                                             mac_a, IEEEBYTES)) {
            if ((!bcmp(dlsw_ptr->dlc_id.target_mac_address,
                       mac_b, IEEEBYTES)) &&
                (dlsw_ptr->dlc_id.origin_link_sap == sap_a) &&
                (dlsw_ptr->dlc_id.target_link_sap == sap_b)) {
                return(dlsw_ptr);
            }
        } else if (!bcmp(dlsw_ptr->dlc_id.origin_mac_address,
                                             mac_b, IEEEBYTES)) {
            if ((!bcmp(dlsw_ptr->dlc_id.target_mac_address,
                       mac_a, IEEEBYTES)) &&
                (dlsw_ptr->dlc_id.origin_link_sap == sap_b) &&
                (dlsw_ptr->dlc_id.target_link_sap == sap_a)) {
                return(dlsw_ptr);
            }
        }
    }
    return(NULL);
}


static void dlsw_init_fields (dlsw_t *dlsw_ptr)
{
    dlsw_ptr->next = NULL;
    dlsw_ptr->magic = DLSW_MAGIC_ID;
    dlsw_ptr->u_cep_id = (int)dlsw_ptr;
    dlsw_ptr->p_cep_id = 0;
    dlsw_ptr->state = dlswst_disconnect;

    dlsw_ptr->port_id_dgra = NULL;
    dlsw_ptr->our_port_id = 0;
    dlsw_ptr->our_dlc_corr = (uint)dlsw_ptr;
    dlsw_ptr->our_transport_id = 0;
    dlsw_ptr->their_port_id = 0;
    dlsw_ptr->their_dlc_corr = 0;
    dlsw_ptr->their_transport_id = 0;

    dlsw_ptr->my_ckt = TRUE;

    /* flow control inits */
    dlsw_ptr->fc_llc_flowlevel = DLC_CONGESTION_LOW;
    dlsw_ptr->fc_peer_flowlevel = DLSW_PEER_CONG_LOW;
    dlsw_ptr->fc_recv_grant_delayed = FALSE;

    dlsw_ptr->fc_send_granted_units = 0;
    dlsw_ptr->fc_send_curr_window =
                      peer_get_init_pac_window(dlsw_ptr->peer_handle);
    dlsw_ptr->fc_largest_send_granted = dlsw_ptr->fc_send_curr_window;
    dlsw_ptr->fc_send_owe_fcack = FALSE;

    dlsw_ptr->fc_recv_curr_window = lpeer->cap.initial_pacing_window;
    dlsw_ptr->fc_largest_recv_granted = dlsw_ptr->fc_recv_curr_window;
    dlsw_ptr->fc_recv_granted_units = 0;
    dlsw_ptr->fc_recv_owe_fcack = FALSE;
    dlsw_ptr->fc_recv_init_needed = TRUE;
    dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_RWO;
    dlsw_ptr->fc_stop_flow_sent = FALSE;
    dlsw_ptr->fc_data_ind_queued = FALSE;

    dlsw_ptr->fc_half_window_sents = 0;
    dlsw_ptr->fc_half_window_rcvds = 0;
    dlsw_ptr->fc_reset_window_sents = 0;
    dlsw_ptr->fc_reset_window_rcvds = 0;

    dlsw_ptr->wan_pak = NULL;
    dlsw_ptr->clsi_msg = NULL;
    dlsw_ptr->pending_clsi_msg = NULL;

    dlsw_ptr->xid_state = dlsw_xid_reset;
    dlsw_ptr->max_i_field_len = 0;
    dlsw_ptr->window_size = 0;
    dlsw_ptr->abm_state = DLSW_ABM_UNKNOWN;

    mgd_timer_init_leaf(&dlsw_ptr->timer, &dlsw_master_timer,
                        0, dlsw_ptr, FALSE);

    dlsw_ptr->dlsw_core_debug_msgs =  dlsw_core_debug_msgs;
    dlsw_ptr->dlsw_core_debug_state = dlsw_core_debug_state;
    dlsw_ptr->dlsw_core_debug_flow = dlsw_core_debug_flow;
    dlsw_ptr->dlsw_core_debug_xid = dlsw_core_debug_xid;

    dlsw_ptr->forwards = 0;
    dlsw_ptr->explorers = 0;
    dlsw_ptr->datagrams = 0;
    dlsw_ptr->iframes = 0;

    GET_TIMESTAMP(dlsw_ptr->circuit_entry_time);
    GET_TIMESTAMP(dlsw_ptr->circuit_state_time);
}


static dlsw_t *dlsw_create_ckt (dlsw_ring_t *port_id,
                                peerhandle_t peer_handle, uchar *saddr,
                                uchar *daddr, uchar ssap, uchar dsap,
                                uchar *rif, ulong mtu_size)
{
    dlsw_t      *dlsw_ptr;
    uint        rif_len;

    dlsw_ptr = malloc_named(sizeof(dlsw_t), "DLSW-CIRCUIT-DYNAMIC");
    if (!dlsw_ptr)
        return(NULL);

    dlsw_ptr->peer_handle = peer_handle; /* must preceed dlsw_init_fields */
    dlsw_init_fields(dlsw_ptr);

    dlsw_ptr->bucket = dlsw_get_hash_bucket(saddr, daddr, ssap, dsap);

    bcopy(saddr, dlsw_ptr->dlc_id.origin_mac_address, IEEEBYTES);
    bcopy(daddr, dlsw_ptr->dlc_id.target_mac_address, IEEEBYTES);
    dlsw_ptr->dlc_id.origin_link_sap = ssap;
    dlsw_ptr->dlc_id.target_link_sap = dsap;

    dlsw_ptr->our_transport_id = (uint) peer_handle;
    dlsw_ptr->port_id_dgra = port_id;

    mgd_timer_init_leaf(&dlsw_ptr->timer,
                        &dlsw_master_timer, 0, dlsw_ptr, FALSE);
    mgd_timer_stop(&dlsw_ptr->timer);
    dlsw_ptr->reach_mtu_size = mtu_size;
    dlsw_ptr->del_flag = FALSE;

    dlsw_ptr->our_port_id = (uint)port_id->swidb; 
    rif_len = RIF_LENGTH(rif[0]);
    if ( (rif) && (rif_len <= SRB_MAXRIF) ){
        bcopy(rif, dlsw_ptr->local_rif, rif_len);
        memset(dlsw_ptr->local_rif+rif_len, 0, (SRB_MAXRIF - rif_len));
        dlsw_ptr->local_rif_len = rif_len;
    } else {
        memset(dlsw_ptr->local_rif, 0, SRB_MAXRIF);
        dlsw_ptr->local_rif_len = 0;
    }

    p_enqueue(&dlsw_hashQ[dlsw_ptr->bucket], dlsw_ptr);
    ++dlsw_circuit_creates;
    ++dlsw_active_circuits;

    dlsw_ptr->llc_flags.cmd_rsp = 0;
    dlsw_ptr->llc_flags.poll_final = 0;

    /*
     * Notify the peer that a new circuit will be established using
     * this peer handle. The peer uses this information to create
     * peer-on-demand connections.
     */
    peer_add_circuit(peer_handle);

    return(dlsw_ptr);
}


/***********************************************************************
 *                     DLSw Initialization
 ***********************************************************************/

/*
 * DLSW subsystem header
 */
SUBSYS_HEADER(dlsw, DLSW_MAJVERSION, DLSW_MINVERSION, DLSW_EDITVERSION,
              dlsw_init, SUBSYS_CLASS_PROTOCOL,
              "seq: cls, srb, iphost",
              "req: cls, srb, iphost");


/*
 * dlsw_init
 * Get dlsw process running.  Called from sub system init.
 * enabling parser to call dlsw.
 */
void dlsw_init (subsystype *subsys)
{
    if (dlsw_running)
        return;

    dlsw_running = TRUE;
    dlsw_started = FALSE;
    dlsw_debug_init();
}

/* 
 * dlsw_default_get_sap
 *
 * The default routine to get the dlsw SAP
 */

static ClsUSapIdT dlsw_default_get_sap (hwidbtype *idb)
{
    sap_entry_t	*sap_entry;

    sap_entry = sap_get_bridging_entry(idb->firstsw);
    
    if (sap_entry) {
	return ((ClsUSapIdT)sap_entry->usapid);
    } else {
	return (NULL);
    }
}

/*
 * This routine starts the DLSw protocol processes.  These processes are
 * only ever started once.  They will continue to run until the router is
 * rebooted.  The gate on starting these processes is dlsw_started
 * forks the required processes for message processing.
 */
void dlsw_start (void)
{
    int         ii;
    union       D_primitives dp;

    if (dlsw_started) {
        csm_start_explr_delay_timer();
        queue_init(&dlsw_bgroupQ, 0);
        dlsw_parse_if_cmds();
        return;
    }

    reg_add_srt_set_srb_params(dlsw_set_srb_params, "dlsw_set_srb_params");
    reg_add_dlsw_parse_srb_if(dlsw_parse_srb_if, "dlsw_parse_srb_if");
    reg_add_dlsw_work(dlsw_work, "");
    reg_add_default_dlsw_get_ring_req(dlsw_default_get_ring_req,
                                      "dlsw_default_get_ring_req");
    reg_add_dlsw_queue_cls_message(dlsw_queue_cls_message,
                                   "dlsw_queue_cls_message");
    reg_add_default_dlsw_get_sap(dlsw_default_get_sap,
				 "dlsw_default_get_sap");
    if (!dlsw_back_pid) {
        dlsw_back_pid = cfork((forkproc *)dlsw_background, 0L,
                             1500, "DLSw Background", 0);
        /* Using default stack size will give stack low warning */
        if (dlsw_back_pid == NO_PROCESS) {
            dlsw_back_pid = 0;
            return;
        }
    }
    if (!dlsw_msg_pid) {
        dlsw_msg_pid = cfork((forkproc *)dlsw_msg_proc, 0L,
                             1500, "DLSw msg proc", 0);
        /* Using default stack size will give stack low warning */
        if (dlsw_msg_pid == NO_PROCESS) {
            dlsw_msg_pid = 0;
            return;
        }
    }

    queue_init(&dlsw_deadQ, 0);
    queue_init(&dlsw_delay_eventQ, 0);
    for (ii = 0; ii < NDLSW_HASH_SIZE; ii++) {
        queue_init(&dlsw_hashQ[ii], 0);
    }

    /*
     * Initialize all the timer chains
     */
    mgd_timer_init_parent(&dlsw_master_timer, NULL);

    queue_init(&dlsw_ringQ, 0);
    queue_init(&dlsw_bgroupQ, 0);

    /* Interface between LAN and core */
    dlsw_core_keepalive_flag = TRUE;
    dp.type = INIT_START;
    dlsw_dlu_init();

    dlsw_peer_init();
    dlsw_csm_init();
    dlsw_local_init();
    dlsw_parse_if_cmds();
    dlsw_started = TRUE;
}


/***********************************************************************
 *                  Background Processing Functions
 ***********************************************************************/


/*
 * dlsw_msgBLOCK()
 *
 * Scheduler test for 2 inputs. 
 *   - Control frames from CLSI. disp_to_fsm() provides an async
 *     call interface and this is not the only place where FSM
 *     gets invoked for CLSI messages.
 */
boolean dlsw_msgBLOCK (void)
{
   if ((QUEUEEMPTY(&dlsw_ctlQ)) && (QUEUEEMPTY(&dlsw_delay_eventQ)) )
       return(TRUE);
   else
       return(FALSE);
}

/*
 * dlsw_process_delayed_events
 *
 * pull deferred events off the queue, and
 * pass them into the fsm
*/

static inline void dlsw_process_delayed_events (void)
{
    dlsw_delay_t *delay_event;

    while ((delay_event = p_dequeue(&dlsw_delay_eventQ))) {
	(* delay_event->restart_fsm)(delay_event->context,
				     delay_event->old_state,
				     delay_event->major,
				     delay_event->minor);

	free(delay_event);
    }
}

/*
 * Main processing module to dequeue frames from the
 * clsi control Q and funnel frames to the fsm, csm, and
 * local switch.
 *
 * Also handles I-frame packets from LLC2 peer and feeds
 * them to peer_input().
 */
forktype dlsw_msg_proc (void)
{
    TCLSIMsg* clsi_msg;
    ulong ctlcount;

    edisms(systeminitBLOCK, 0);
    for (;;) {
        edisms((blockproc *)dlsw_msgBLOCK, 0);

        ctlcount = PAK_SWITCHCOUNT;
        while ( TRUE ) {

	    /* handle deferred events */
	    dlsw_process_delayed_events();

            /*
             * Process only a limited number of packets per invocation
             */
            if (--ctlcount == 0)
                break;

            clsi_msg = p_dequeue(&dlsw_ctlQ);
            if (clsi_msg == NULL )
                break;

            dlsw_pre_proc_clsi(clsi_msg);
        }
    }
}


forktype dlsw_background (void)
{
    dlsw_t *dlsw_ptr;

    edisms(systeminitBLOCK, 0);

    /* Start the explorer delay timer if configured
     * This is to give a chance for all the peers to come up before
     * we start pumping out explorers to the peers
     */
    csm_start_explr_delay_timer();

    while (TRUE) {
        csm_house_keeping();    /* RP */
        dlsw_peer_background(); /* peer manager periodic tasks */
        dlsw_local_background();

        /*
         * Clean up any connections which are in process of coming up or down
         */
        if (dlsw_core_keepalive_flag) {
            while(mgd_timer_expired(&dlsw_master_timer)) {
                mgd_timer *expired_timer;
                dlsw_t    *dlsw_ptr;
                expired_timer = mgd_timer_first_expired(&dlsw_master_timer);
                dlsw_ptr = mgd_timer_context(expired_timer);
                dlsw_fsm(dlsw_ptr, dlswma_timer, dlswmi_timer_expired);
            }
        }

        if (dlsw_deadQ.qhead) {
            while ((dlsw_ptr = p_dequeue(&dlsw_deadQ))) {
                if (dlsw_ptr->pending_clsi_msg)
                    CLSIMsgDestroy(dlsw_ptr->pending_clsi_msg);
                dlsw_ptr->del_flag = TRUE;
                dlsw_ptr->next = NULL;
                free(dlsw_ptr);
            }
        }

        process_sleep_for(ONESEC);
            }
        }

/*
 * dlsw_delayed_event
 *
 * build an event block and enqueue it for deferred input
 * into the fsm
 *
 * Inputs
 *
 * fsm		wrapper to restart the fsm.  This is used for debugging display
 * context	data structure that the fsm acts on
 * state	current state - used for debugging display
 * major, minor event codes
 *
 * Outputs
 *
 * event is queued up on the deferred event queue
 *
 * Errors:
 *
 * No error handling if malloc fails - we _really_ need that.
 */

boolean
dlsw_delayed_event (restart_fsm_t fsm, void *context, int state, int major, int minor)
{
    dlsw_delay_t	*delay_event;

    delay_event = malloc_named(sizeof(dlsw_delay_t), "DLSW-EVENT");
    
    if (delay_event == NULL) {
	return(FALSE);
    }
    delay_event->next = NULL;
    delay_event->restart_fsm  = fsm;
    delay_event->context = context;
    delay_event->old_state = state;
    delay_event->major = major;
    delay_event->minor = minor;
    p_enqueue(&dlsw_delay_eventQ, delay_event);
    return(TRUE);
}


/***********************************************************************
 *       SSP Message Processing Functions - Includes Flow Control
 ***********************************************************************/


static void fill_ssp_hdrs(ssp_hdr_t *ssp_ptr, dlsw_t *dlsw_ptr, ssp_op_t ssp_op)
{
    ssp_dlc_id_t hash_dlc_id;

    ssp_ptr->ssp_vers  = SSP_VERS;
    PUTLONG(&ssp_ptr->rem_dlc_port_id, dlsw_ptr->their_port_id);
    PUTLONG(&ssp_ptr->rem_dlc_corr, dlsw_ptr->their_dlc_corr);
    ssp_ptr->ssp_msg_type  = ssp_op;
    ssp_ptr->ssp_fc = 0;

    if (is_ssp_info_hdr(ssp_op)) {
        ssp_ptr->ssp_hdr_len  = SSP_INFO_HDR_LEN;
        return;
    }

    ssp_ptr->ssp_hdr_len  = SSP_CNTL_HDR_LEN;
    ssp_ptr->ssp_prot_id = SSP_PROTOCOL_ID;
    ssp_ptr->ssp_hdr_no = SSP_HDR_NUM;
    ssp_ptr->ssp_dirn = ((dlsw_ptr->my_ckt) ?
                          SSP_DIRN_FORWARD :
                          SSP_DIRN_BACKWARD);
    ssp_ptr->origin_dlc_port_id = dlsw_ptr->my_ckt ?
                 dlsw_ptr->our_port_id : dlsw_ptr->their_port_id;

    ssp_ptr->origin_dlc_corr = dlsw_ptr->my_ckt ?
                 dlsw_ptr->our_dlc_corr : dlsw_ptr->their_dlc_corr;

    ssp_ptr->origin_transport_id = dlsw_ptr->my_ckt ?
                 dlsw_ptr->our_transport_id : dlsw_ptr->their_transport_id;


    ssp_ptr->target_dlc_port_id = dlsw_ptr->my_ckt ?
                 dlsw_ptr->their_port_id : dlsw_ptr->our_port_id;

    ssp_ptr->target_dlc_corr = dlsw_ptr->my_ckt ?
                 dlsw_ptr->their_dlc_corr : dlsw_ptr->our_dlc_corr;

    ssp_ptr->target_transport_id = dlsw_ptr->my_ckt ?
                                   dlsw_ptr->their_transport_id :
                                   dlsw_ptr->our_transport_id;

    if ( dlsw_is_ssp_ctl_mac_hdr(dlsw_ptr, ssp_op))
        ssp_ptr->ssp_dlc_hdr_len = SSP_NB_DLC_HDR_LEN;
    else
        ssp_ptr->ssp_dlc_hdr_len = SSP_SNA_DLC_HDR_LEN;

    ssp_ptr->ssp_flags = 0;
    hash_dlc_id = dlsw_ptr->dlc_id;
    bcopy((uchar *)&hash_dlc_id, (uchar *)&(ssp_ptr->ssp_dlc_id),
          sizeof(ssp_dlc_id_t));

    if (is_ssp_lf_valid(ssp_op)) {
        if (peer_get_vendor_type(dlsw_ptr->peer_handle) == PEER_NOT_CISCO) {
            ssp_ptr->ssp_lf = bytes_to_ssp_lf_bits(dlsw_ptr->reach_mtu_size);
        } else {
            ssp_ptr->ssp_lf = bytes_to_rc_code(dlsw_ptr->reach_mtu_size);
        }
        if (dlsw_core_debug_msgs)
            buginf("\nCORE: Setting lf : bits %x : size %d",
                   ssp_ptr->ssp_lf, dlsw_ptr->reach_mtu_size);
    }

    if (ssp_ptr->target_sap == 0)
        ssp_ptr->target_sap = ssp_ptr->origin_sap;

    ssp_ptr->ssp_llc_cr = dlsw_ptr->llc_flags.cmd_rsp;
    ssp_ptr->ssp_llc_pf = dlsw_ptr->llc_flags.poll_final;
}


static paktype *dlsw_get_ssp_info_pak (dlsw_t *dlsw_ptr, ssp_op_t ssp_op,
                                uchar *payload_ptr, ulong payload_len)
{
    paktype *pak;
    ssp_hdr_t *ssp_ptr;

    if (ssp_op == SSP_OP_ifcm) {
        if ((pak = getbuffer(sizeof(ssp_info_t))) == NULL)
            return NULL;
        ssp_ptr = (ssp_hdr_t *) (pak->datagramstart);
        ssp_ptr->ssp_msg_len  = 0;
    } else {
        if (!dlsw_ptr->clsi_msg)
            return(NULL);

        pak = CLSIMsgGetPak(dlsw_ptr->clsi_msg);
        if (!pak)
            return NULL;

        dlsw_ptr->clsi_msg = NULL;
        pak->datagramstart = payload_ptr - sizeof(ssp_i_frame_t);
        ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
        PUTSHORT(&ssp_ptr->ssp_msg_len, payload_len);
    }

    fill_ssp_hdrs(ssp_ptr, dlsw_ptr, ssp_op);
    pak->datagramsize = GETSHORT(&ssp_ptr->ssp_msg_len) + sizeof(ssp_i_frame_t);
    return(pak);
}


static paktype *dlsw_get_ssp_cntl_pak (dlsw_t *dlsw_ptr, ssp_op_t ssp_op,
                                       uchar *payload_ptr, ulong payload_len)
{
    ssp_hdr_t *ssp_ptr;
    ulong dgsize = payload_len;
    paktype *pak;

    dgsize += sizeof(ssp_ctl_t);
    if (dlsw_is_ssp_ctl_mac_hdr(dlsw_ptr, ssp_op))
        dgsize += SSP_NB_DLC_HDR_LEN;

    pak = getbuffer(dgsize);
    if (!pak) {
        return(NULL);
    }

    ssp_ptr = (ssp_hdr_t *) (pak->datagramstart);
    fill_ssp_hdrs(ssp_ptr, dlsw_ptr, ssp_op);
    ssp_ptr->ssp_msg_len = payload_len;
    if ((is_ssp_need_data(ssp_op)) && (payload_len > 0)) {
        bcopy(payload_ptr, ((uchar *) ssp_ptr) + sizeof(ssp_ctl_t),
              payload_len);
    }

    pak->datagramsize = sizeof(ssp_control_frame_t) + ssp_ptr->ssp_msg_len;
    return(pak);
}

boolean dlsw_cantput (word UCepId, boolean queueempty)
{
    dlsw_t *dlsw_ptr;

    /*
     * The "cantput" feature creates one call for all DLSw circuits.
     * This includes local and remote.The CEP is not a remote core
     * circuit, then accept the i-frame for the local switch code.
     */ 
    dlsw_ptr = (dlsw_t *) UCepId;

    if (dlsw_ptr->magic != DLSW_MAGIC_ID)
        return(FALSE);

    if (dlsw_ptr != (dlsw_t *) dlsw_ptr->u_cep_id) {
        dlsw_ptr->fc_data_ind_queued = FALSE;
        return(FALSE);
    }

    if (dlsw_ptr->fc_send_granted_units > 0) {
        dlsw_ptr->fc_send_granted_units--;
        FLOW_D("\nDLSw: %u decr s - s:%d so:%d r:%d ro:%d",
               dlsw_ptr->our_dlc_corr,
               dlsw_ptr->fc_send_granted_units,
               dlsw_ptr->fc_send_owe_fcack,
               dlsw_ptr->fc_recv_granted_units,
               dlsw_ptr->fc_recv_owe_fcack);
        if (queueempty)
          dlsw_ptr->fc_data_ind_queued = FALSE;
        return(FALSE);
    }

    FLOW_D("\nDLSw: %u can not accept i-frame - s:%d so:%d r:%d ro:%d",
           dlsw_ptr->our_dlc_corr,
           dlsw_ptr->fc_send_granted_units,
           dlsw_ptr->fc_send_owe_fcack,
           dlsw_ptr->fc_recv_granted_units,
           dlsw_ptr->fc_recv_owe_fcack);

    dlsw_ptr->fc_data_ind_queued = TRUE;
    return(TRUE);
}


static void insert_fca (dlsw_t *dlsw_ptr, ssp_hdr_t *ssp_hdr)
{
    /*
     * If we owe an FCA, piggy-back it.
     */
    if (is_ssp_fc_op(ssp_hdr->ssp_msg_type)) {
        if (dlsw_ptr->fc_send_owe_fcack) {
            ssp_hdr->ssp_fc |= SSP_FLOW_CONTROL_FCA;
            dlsw_ptr->fc_send_owe_fcack = FALSE;

            FLOW_D("\nDLSw: %u sent FCA on %s",
                   dlsw_ptr->our_dlc_corr,
                   dlsw_sspop2str(ssp_hdr->ssp_msg_type));
        }
    }
}


static void insert_fci (dlsw_t *dlsw_ptr, ssp_hdr_t *ssp_hdr)
{
    ssp_hdr->ssp_fc = 0;

    /*
     * If we are awaiting an FCA (to a previously sent FCI), then we
     * can't send another FCI. Otherwise, if receiver's granted
     * units are less than one full window, then grant a new window.
     */
    if (dlsw_ptr->fc_recv_owe_fcack == FALSE) {
        /*
         * Update receiver's next window accordingly.
         */
        switch (dlsw_ptr->fc_recv_grant_oper) {
          case SSP_FLOW_CONTROL_RWO:
            FLOW_D("\nDLSw: sent RWO");
            break;
          case SSP_FLOW_CONTROL_IWO:
            FLOW_D("\nDLSw: sent IWO");
            dlsw_ptr->fc_recv_curr_window++;
            if (dlsw_ptr->fc_recv_curr_window > 
                             dlsw_ptr->fc_largest_recv_granted) {
                dlsw_ptr->fc_largest_recv_granted = 
                             dlsw_ptr->fc_recv_curr_window;
            }
            break;
          case SSP_FLOW_CONTROL_DWO:
            FLOW_D("\nDLSw: sent DWO");
            if (dlsw_ptr->fc_recv_curr_window > 1)
                dlsw_ptr->fc_recv_curr_window--;
            break;
          case SSP_FLOW_CONTROL_ZWO:
            FLOW_D("\nDLSw: sent ZWO");
            dlsw_ptr->fc_recv_curr_window = 0;
            ++dlsw_ptr->fc_reset_window_sents;
            break;
          case SSP_FLOW_CONTROL_HWO:
            FLOW_D("\nDLSw: sent HWO");
            if (dlsw_ptr->fc_recv_curr_window > 1)
                dlsw_ptr->fc_recv_curr_window =
                                  dlsw_ptr->fc_recv_curr_window / 2;
            ++dlsw_ptr->fc_half_window_sents;
            break;
        }

        ssp_hdr->ssp_fc |= SSP_FLOW_CONTROL_FCI | dlsw_ptr->fc_recv_grant_oper;
        dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_RWO;
        dlsw_ptr->fc_recv_owe_fcack = TRUE;
        dlsw_ptr->fc_recv_granted_units += dlsw_ptr->fc_recv_curr_window;
    }

    FLOW_D("\nDLSw: %u sent FCI %x on %s - s:%d so:%d r:%d ro:%d",
           dlsw_ptr->our_dlc_corr,
           ssp_hdr->ssp_fc, dlsw_sspop2str(ssp_hdr->ssp_msg_type),
           dlsw_ptr->fc_send_granted_units, dlsw_ptr->fc_send_owe_fcack,
           dlsw_ptr->fc_recv_granted_units, dlsw_ptr->fc_recv_owe_fcack);
}


static void send_ssp_ifcack (dlsw_t *dlsw_ptr)
{
    paktype             *pak;
    peer_msg_u          peer_msg;
    ssp_hdr_t		*ssp_hdr;

    if (!dlsw_ptr->peer_handle)
        return;

    pak = dlsw_get_ssp_info_pak(dlsw_ptr, SSP_OP_ifcm, NULL, 0);
    if (pak == NULL) {
        return;
    }

    FLOW_D("\nDLSw: %u sent FCA on %s - s:%d so:%d r:%d ro:%d",
           dlsw_ptr->our_dlc_corr, "IFCM", 
           dlsw_ptr->fc_send_granted_units, dlsw_ptr->fc_send_owe_fcack,
           dlsw_ptr->fc_recv_granted_units, dlsw_ptr->fc_recv_owe_fcack);

    ssp_hdr = (ssp_hdr_t *) pak->datagramstart;
    ssp_hdr->ssp_fc = SSP_FLOW_CONTROL_FCA;
    dlsw_ptr->fc_send_owe_fcack = FALSE;

    peer_msg.type = PEER_PUT;
    peer_msg.p_put.flags = PEER_SSP_TYPE | PEER_INFO_FRAME;
    peer_msg.put_handle = dlsw_ptr->peer_handle;
    peer_msg.put_handle = dlsw_ptr->peer_handle;
    peer_msg.p_put.pak_cat = PEER_CIR_ADMIN;

    peer_msg.p_put.lan_port = dlsw_ptr->port_id_dgra;
    peer_msg.p_put.pak = pak;

    if (dlsw_ptr->my_ckt) {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.target_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.origin_mac_address;
    } else {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.origin_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.target_mac_address;
    }

    core_to_peer(&peer_msg);
}


static void send_ssp_ifcind (dlsw_t *dlsw_ptr)
{
    paktype             *pak;
    peer_msg_u          peer_msg;
    ssp_hdr_t		*ssp_hdr;

    if (!dlsw_ptr->peer_handle)
        return;

    pak = dlsw_get_ssp_info_pak(dlsw_ptr, SSP_OP_ifcm, NULL, 0);
    if (pak == NULL) {
        return;
    }

    ssp_hdr = (ssp_hdr_t *) pak->datagramstart;
    insert_fci(dlsw_ptr, ssp_hdr);
    insert_fca(dlsw_ptr, ssp_hdr);

    peer_msg.type = PEER_PUT;
    peer_msg.p_put.flags = PEER_SSP_TYPE | PEER_INFO_FRAME;
    peer_msg.put_handle = dlsw_ptr->peer_handle;
    peer_msg.put_handle = dlsw_ptr->peer_handle;
    peer_msg.p_put.pak_cat = PEER_CIR_ADMIN;

    peer_msg.p_put.lan_port = dlsw_ptr->port_id_dgra;
    peer_msg.p_put.pak = pak;

    if (dlsw_ptr->my_ckt) {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.target_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.origin_mac_address;
    } else {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.origin_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.target_mac_address;
    }

    core_to_peer(&peer_msg);
}


static void do_wan_inbound_fc (dlsw_t *dlsw_ptr, ssp_hdr_t *ssp_hdr)
{
    ssp_op_t ssp_op;
    uchar fci_op;

    ssp_op = ssp_hdr->ssp_msg_type;

    /*
     * Count this message against the receiver's granted units
     * if it is a flow controlled message.
     */
    if (is_ssp_fc_ctr_op(ssp_op)) {
        if (dlsw_ptr->fc_recv_granted_units > 0) {
            dlsw_ptr->fc_recv_granted_units--;
            FLOW_D("\nDLSw: %u decr r - s:%d so:%d r:%d ro:%d",
                   dlsw_ptr->our_dlc_corr,
                   dlsw_ptr->fc_send_granted_units,
                   dlsw_ptr->fc_send_owe_fcack,
                   dlsw_ptr->fc_recv_granted_units,
                   dlsw_ptr->fc_recv_owe_fcack);
        }
    }

    /*
     * Does this frame contain an FCA.
     */
    if (ssp_hdr->ssp_fc & SSP_FLOW_CONTROL_FCA) {
        dlsw_ptr->fc_recv_owe_fcack = FALSE;

        /*
         * By the time we got the ack, if our partner's granted
         * units fell below the lower limit of our desired fc
         * oscillating range, then increment the next window.
         */
        if (dlsw_ptr->fc_recv_granted_units <
                      dlsw_ptr->fc_recv_curr_window) {
            if (dlsw_ptr->fc_recv_curr_window >= lpeer->max_pacing_window)
                dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_RWO;
            else
                dlsw_ptr->fc_recv_grant_oper = SSP_FLOW_CONTROL_IWO;
        }

        FLOW_D("\nDLSw: %u recv FCA on %s - s:%d so:%d r:%d ro:%d",
               dlsw_ptr->our_dlc_corr, dlsw_sspop2str(ssp_op),
               dlsw_ptr->fc_send_granted_units,
               dlsw_ptr->fc_send_owe_fcack,
               dlsw_ptr->fc_recv_granted_units,
               dlsw_ptr->fc_recv_owe_fcack);
    }

    /*
     * If the FCI bit is set, then the receiver is telling
     * us what to do with our next window grant.
     */
    if (ssp_hdr->ssp_fc & SSP_FLOW_CONTROL_FCI) {
        fci_op = ssp_hdr->ssp_fc & SSP_FLOW_CONTROL_MASK;

        FLOW_D("\nDLSw: %u recv FCI %x - s:%d so:%d r:%d ro:%d",
               dlsw_ptr->our_dlc_corr, fci_op,
               dlsw_ptr->fc_send_granted_units,
               dlsw_ptr->fc_send_owe_fcack,
               dlsw_ptr->fc_recv_granted_units,
               dlsw_ptr->fc_recv_owe_fcack);

        switch (fci_op) {
          case SSP_FLOW_CONTROL_RWO:
            FLOW_D("\nDLSw: recv RWO");
            dlsw_ptr->fc_send_granted_units +=
                                      dlsw_ptr->fc_send_curr_window;
            dlsw_ptr->fc_send_owe_fcack = TRUE;
            dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_RESUME_FLOW);
            break;
          case SSP_FLOW_CONTROL_IWO:
            FLOW_D("\nDLSw: recv IWO");
            dlsw_ptr->fc_send_curr_window++;
            dlsw_ptr->fc_send_granted_units +=
                                      dlsw_ptr->fc_send_curr_window;
            dlsw_ptr->fc_send_owe_fcack = TRUE;
            if (dlsw_ptr->fc_send_granted_units > 
                                dlsw_ptr->fc_largest_send_granted) {
                dlsw_ptr->fc_largest_send_granted = 
                                dlsw_ptr->fc_send_granted_units;
            }

            dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_RESUME_FLOW);
            break;
          case SSP_FLOW_CONTROL_DWO:
            FLOW_D("\nDLSw: recv DWO");
            if (dlsw_ptr->fc_send_curr_window > 1) {
                dlsw_ptr->fc_send_curr_window--;
                dlsw_ptr->fc_send_granted_units +=
                                          dlsw_ptr->fc_send_curr_window;
            }
            dlsw_ptr->fc_send_owe_fcack = TRUE;
            dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_RESUME_FLOW);
            break;
          case SSP_FLOW_CONTROL_ZWO:
            FLOW_D("\nDLSw: recv ZWO");
            dlsw_ptr->fc_send_curr_window = 0;
            dlsw_ptr->fc_send_granted_units = 0;
            ++dlsw_ptr->fc_reset_window_rcvds;

            /*
             * Receipt of reset window indication requires fcack.
             */
            send_ssp_ifcack(dlsw_ptr);
            break;
          case SSP_FLOW_CONTROL_HWO:
            FLOW_D("\nDLSw: recv HWO");
            if (dlsw_ptr->fc_send_curr_window > 1) {
                dlsw_ptr->fc_send_curr_window = 
                                      dlsw_ptr->fc_send_curr_window / 2;
            }
            dlsw_ptr->fc_send_granted_units +=
                                      dlsw_ptr->fc_send_curr_window;
            ++dlsw_ptr->fc_half_window_rcvds;
            dlsw_ptr->fc_send_owe_fcack = TRUE;
            dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_RESUME_FLOW);
            break;
        }
    }

    /*
     * If the receiver's granted units are below 
     * DLSW_CKT_WINDOW_LOW_THRESHOLD, this indicates that we haven't
     * sent an SSP op that we could piggy back an FCI on. Send
     * an independent FCI. Make sure that we don't send 2 FCI
     * without receiving an FCA in between.
     */
    if ((dlsw_ptr->fc_recv_owe_fcack == FALSE) &&
        (dlsw_ptr->fc_recv_init_needed == FALSE)) {
        if (dlsw_ptr->fc_recv_granted_units <=
                      DLSW_CKT_WINDOW_LOW_THRESHOLD(dlsw_ptr)) {
            send_ssp_ifcind(dlsw_ptr);
        }
    }
}


static void do_wan_outbound_fc (dlsw_t *dlsw_ptr, ssp_hdr_t *ssp_hdr)
{
    ssp_op_t ssp_op;

    ssp_op = ssp_hdr->ssp_msg_type;

    if (is_ssp_fc_op(ssp_op)) {
        if (dlsw_ptr->fc_recv_init_needed) {
            /*
             * No flow control indication have been sent.
             */
            dlsw_ptr->fc_recv_init_needed = FALSE;
            insert_fci(dlsw_ptr, ssp_hdr);
            return;
        }

        /*
         * If we are awaiting an FCA (to our FCI), then we
         * can't send another FCI. Otherwise, if receiver's granted
         * units are less than one full window, then grant a new window.
         */
        if (dlsw_ptr->fc_recv_owe_fcack == FALSE) {
            if (dlsw_ptr->fc_recv_granted_units < dlsw_ptr->fc_recv_curr_window)
                insert_fci(dlsw_ptr, ssp_hdr);
        }
        insert_fca(dlsw_ptr, ssp_hdr);
    }
}
              

void dlsw_send_ssp (dlsw_t *dlsw_ptr, ssp_op_t ssp_op,
                    uchar *payload_ptr, ulong payload_len)
{
    paktype             *pak;
    peer_msg_u          peer_msg;
    core2peer_ret_t     ret_code;
    char                *str;

    if (!dlsw_ptr->peer_handle)
        return;

    str = string_getnext();

    if (is_ssp_info_hdr(ssp_op))
        pak = dlsw_get_ssp_info_pak(dlsw_ptr, ssp_op, payload_ptr, payload_len);
    else
        pak = dlsw_get_ssp_cntl_pak(dlsw_ptr, ssp_op, payload_ptr, payload_len);

    if (pak == NULL) {
        return;
    }

    do_wan_outbound_fc(dlsw_ptr, (ssp_hdr_t *) pak->datagramstart);

    peer_msg.type = PEER_PUT;
    peer_msg.p_put.flags = PEER_SSP_TYPE | PEER_INFO_FRAME;
    if (ssp_op == SSP_OP_canureach)
      peer_msg.p_put.flags |= PEER_CKT_START;
    peer_msg.put_handle = dlsw_ptr->peer_handle;

    if (ssp_op == SSP_OP_infoframe)
        peer_msg.p_put.pak_cat = PEER_USER_INFO;
    else
        peer_msg.p_put.pak_cat = PEER_CIR_ADMIN;

    peer_msg.p_put.lan_port = dlsw_ptr->port_id_dgra;
    peer_msg.p_put.pak = pak;

    if (dlsw_ptr->my_ckt) {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.target_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.origin_mac_address;
    } else {
        peer_msg.p_put.ssap = dlsw_ptr->dlc_id.target_link_sap;
        peer_msg.p_put.dsap = dlsw_ptr->dlc_id.origin_link_sap;
        peer_msg.p_put.dmac = dlsw_ptr->dlc_id.origin_mac_address;
        peer_msg.p_put.smac = dlsw_ptr->dlc_id.target_mac_address;
    }

    ret_code = core_to_peer(&peer_msg);
    switch (ret_code) {
      case PEER_SUCCESS_AND_BUSY :
        dlsw_mark_peerckts(dlsw_ptr->peer_handle, DLSW_PEER_CONG_HIGH);
 
        /* Intentional fall through */
      case PEER_SUCCESS :
        /*
         * If fc data unit op, determine if lan needs throttling.
         */
        if (is_ssp_fc_ctr_op(ssp_op)) {
            if (dlsw_ptr->fc_send_granted_units == 0)
                dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_STOP_FLOW);
        }

        if (dlsw_core_debug_msgs) {
            errmsg(DLSWC_ERR_SENDSSP, ssp_op,
                   dlsw_sspop2str(ssp_op), "",
                   peer_handle_to_str(peer_msg.put_handle, str),
                   "success");
        }
        break;

      case PEER_UNREACHABLE :
      case PEER_INVALID_HANDLE :
      case PEER_NO_CONNECTIONS :
      case PEER_FILTERED :
      default :
        MSG_D("\nDLSw: %s unreachable - reason code %d",
              peer_handle_to_str(peer_msg.put_handle, str), ret_code);

        /*
         * Circuit needs to be torn down, but can't recursively
         * call fsm. Queue wan_fail event on the dlsw_delay_eventQ
         * to be processed in the background. Zero'ing the peer_handle
         * avoids a race condition between the core finding out
         * a peer is dead through core_to_peer() and the peer
         * code calling peer_to_core() with PEER_KILL_HANDLE.
         */
	if (dlsw_delayed_event(dlsw_restart_fsm, dlsw_ptr, dlsw_ptr->state,
			       dlswma_admin, dlswmi_admin_wan_fail)) {
            if (dlsw_is_peer_being_removed(dlsw_ptr->peer_handle))
                dlsw_ptr->peer_handle = 0;
        }
        break;
    }
}


void dlsw2clsi_flow_action (dlsw_t *dlsw_ptr, ushort action)
{
    union D_primitives    dp;

    /*
     * It is a bad thing to check state outside of the
     * fsm. This one is difficult to do any other way.
     */
    if (dlsw_ptr->state != dlswst_connected)
        return;

    dp.type = D_FLOW_REQ;
    dp.disp_flow_req.pCepId = dlsw_ptr->p_cep_id;
    dp.disp_flow_req.action = action;

    if (action == CLS_ACTION_RESUME_FLOW) {
        /* Do not resume if PEER congestion is high. */
        if (dlsw_ptr->fc_peer_flowlevel == DLSW_PEER_CONG_HIGH) 
            return;

        /*
         * Only resume if we were previously stopped or
         * dlsw_cantput has caused data_ind's to be queued and
         * we need to flush them. This accounts for the case where
         * dlsw_cantput returned TRUE and a FCI message was received
         * before CLS_ACTION_STOP_FLOW was sent.
         */
        if ((dlsw_ptr->fc_stop_flow_sent) || (dlsw_ptr->fc_data_ind_queued)) {
            FLOW_D("\nDLSw: Flow Resume to CLSI");
            fsm_to_disp(&dp);
            dlsw_ptr->fc_stop_flow_sent = FALSE;
        }
    } else { /* CLS_ACTION_STOP_FLOW */
        /* only stop if not already stopped */
        if (!dlsw_ptr->fc_stop_flow_sent) {
            FLOW_D("\nDLSw: Flow Stop to CLSI");
            fsm_to_disp(&dp);
            dlsw_ptr->fc_stop_flow_sent = TRUE;
        }
    }
}


/*
 * Scans the list of circuits going the
 * peer that got busy and marks the circuits to make it busy
 */

static void dlsw_mark_peerckts (peerhandle_t peer_handle, ulong cong_level)
{
    int n;
    dlsw_t *dlsw_ptr;
    char   *str;

    str = string_getnext();
    if (dlsw_core_debug_flow)
        buginf("\n DLSw-FC : %s congestion level %s",
               peer_handle_to_str(peer_handle, str),
               (cong_level & DLSW_PEER_CONG_HIGH) ? "High" :
               (cong_level & DLSW_PEER_CONG_LOW) ? "Low" : "unknown");
   
    for (n = 0; n < NDLSW_HASH_SIZE; n++) {
        dlsw_ptr = (dlsw_t *) dlsw_hashQ[n].qhead;
        for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
            if (dlsw_ptr->peer_handle != peer_handle)
                continue;
            dlsw_ptr->fc_peer_flowlevel = cong_level;
            /*
             * It is a bad thing to check state outside of the
             * fsm. This one is difficult to do any other way.
             */
            if (dlsw_ptr->state == dlswst_connected) {
                if (cong_level & DLSW_PEER_CONG_LOW) {
                    FLOW_D("\nDLSw: %u DLSW_PEER_CONG_LOW",
                           dlsw_ptr->our_dlc_corr);
                    dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_RESUME_FLOW);
                } else if (cong_level & DLSW_PEER_CONG_HIGH) {
                    FLOW_D("\nDLSw: %u DLSW_PEER_CONG_HIGH",
                           dlsw_ptr->our_dlc_corr);
                    dlsw2clsi_flow_action(dlsw_ptr, CLS_ACTION_STOP_FLOW);
                }
            }
        }
    }
}


/***********************************************************************
 *                    CSM Message Processing Functions
 ***********************************************************************/

static csm_rc_t new_ckt_from_clsi (TCLSIMsg *clsi_msg, dlsw_ring_t *port_id,
                               peerhandle_t peer_handle, ulong mtu_size)
{
    uchar *saddr, *daddr;
    uchar ssap, dsap;
    uchar rif[SRB_MAXRIF];
    dlsw_t *dlsw_ptr;
    ushort cls_op;
    int    rif_rc;
    int    rc;

    saddr = get_saddr_from_clsi(clsi_msg);
    daddr = get_daddr_from_clsi(clsi_msg);
    dsap = get_dsap_from_clsi(clsi_msg);
    ssap = get_ssap_from_clsi(clsi_msg);
    rif_rc = get_rif_from_clsi(clsi_msg, rif);
    if (rif_rc < 0) {
        /* problem with the rif... discard message */
        CLSIMsgDestroy(clsi_msg);
        return PROC_OK;
    }
    rif[1] ^= RC_D; /* make the direction forward */
    rif[0] &= ~RC_RTMASK; /* clear broadcast bits */

    if (dlsw_core_debug_msgs)
        buginf("\nDLSw: new_ckt_from_clsi(): %s %e:%x->%e:%x",
               port_id->swidb->hwptr->hw_namestring,
               saddr, ssap, daddr, dsap);

    dlsw_ptr = dlsw_find_ckt(saddr, daddr, ssap, dsap);
    if (dlsw_ptr) {
        /*
         * This should not happen. CSM received a *_STN
         * clsi message and we have a local circuit.
         * Kill the circuit, drop the clsi msg, and start over.
         */
        MSG_D("\nDLSw: new_ckt_from_clsi(): ckt already exists %e:%x->%e:%x",
              saddr, ssap, daddr, dsap);
        dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
        CLSIMsgDestroy(clsi_msg);
    } else {
        /*
         * before creating a circuit call to start the peer
         * if it is a pod peer, not connected, return
         * csm will handle retries.
         */
        rc = peer_new_ckt (peer_handle);
        if (rc == PEER_WILL_START) {
            return POD_PEER_NOT_READY;
        }

        /*
         * If the peer did not connect, peer is removed. Indicate this to
         * CSM, so it does not retry.
         */
        if (rc == PEER_INVALID_PEER)
          return POD_PEER_INVALID;

        /*
         * dlsw_create_ckt() will save clsi_msg.
         * When ReqOpnStn.Cnf is received, it will be processed.
         */
        dlsw_ptr = dlsw_create_ckt(port_id, peer_handle,
                                   saddr, daddr, ssap, dsap, rif,
                                   mtu_size);
        if (dlsw_ptr) {
            cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
            dlsw_ptr->my_ckt = TRUE;
            dlsw_ptr->clsi_msg = clsi_msg;
            dlsw_fsm(dlsw_ptr, dlswma_dlc, cls_op);
        } else {
            CLSIMsgDestroy(clsi_msg);
        }
    }
    return PROC_OK;
}


static void new_ckt_from_wan (paktype *pak, dlsw_ring_t *port_id,
                              peerhandle_t peer_handle, uchar *rif,
                              ulong mtu_size)
{
    dlsw_t *dlsw_ptr;
    ssp_op_t ssp_op;
    ssp_hdr_t *ssp_hdr;
    int rc;

    ssp_hdr = (ssp_hdr_t *)pak->datagramstart;
    ssp_op = ssp_hdr->ssp_msg_type;

    /*
     * Only create wan initiated circuits on CUR_cs.
     * This applies for SNA and NetBIOS.
     */
    if (ssp_op != SSP_OP_canureach) {
        datagram_done(pak);
        return;
    }

    dlsw_ptr = dlsw_find_ckt(ssp_hdr->origin_mac, ssp_hdr->target_mac,
                             ssp_hdr->origin_sap, ssp_hdr->target_sap);
    if (dlsw_ptr) {
        /*
         * CSM received a SSP_OP_canureach message and we have a circuit.
         * This can happen when end stations send SABME/XID at the same
         * time. In this case, existing circuit will be in the "circuit
         * start" state. As per RFC1795, either drop CUR_cs or existing
         * circuit depending on origin and target mac address compare.
         */
        MSG_D("\nDLSw: new_ckt_from_wan(): ckt already exists %e:%x->%e:%x",
              ssp_hdr->origin_mac, ssp_hdr->origin_sap,
              ssp_hdr->target_mac, ssp_hdr->target_sap);
        if (dlsw_ptr->state == dlswst_ckt_start) {
            rc = ieee_compare(ssp_hdr->origin_mac,
                             dlsw_ptr->dlc_id.origin_mac_address);
            if (rc > 0) {
                /*
                 * Drop existing circuit and accept ssp cur_cs.
                 */
                MSG_D("\nDLSw: Ckt collision - drop existing ckt %e:%x->%e:%x",
                      dlsw_ptr->dlc_id.origin_mac_address,
                      dlsw_ptr->dlc_id.origin_link_sap,
                      dlsw_ptr->dlc_id.target_mac_address,
                      dlsw_ptr->dlc_id.target_link_sap);
                dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
            } else if (rc < 0) {
                /*
                 * Drop CUR_cs. Return here, do not create new circuit below.
                 */
                MSG_D("\nDLSw: Ckt collision - drop CUR_cs : %e:%x->%e:%x",
                      ssp_hdr->origin_mac, ssp_hdr->origin_sap,
                      ssp_hdr->target_mac, ssp_hdr->target_sap);
                datagram_done(pak);
                return;
            } else {
                /*
                 * We sent CUR_cs and received one from the peer with matching
                 * origin mac address. Something is wrong. Clean up and restart.
                 * Return here so as to not create new circuit below.
                 */
                MSG_D("\nDLSw: Ckt collision error : %e:%x->%e:%x",
                      ssp_hdr->origin_mac, ssp_hdr->origin_sap,
                      ssp_hdr->target_mac, ssp_hdr->target_sap);
                dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
                datagram_done(pak);
                return;
            }
         } else {
            MSG_D("\nDLSw: CUR_cs received in state %s : %e:%x->%e:%x",
                  dlsw_print_state(dlsw_ptr->state),
                  ssp_hdr->origin_mac, ssp_hdr->origin_sap,
                  ssp_hdr->target_mac, ssp_hdr->target_sap);
            dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_stop);
            datagram_done(pak);
            return;
        }
    }

    dlsw_ptr = dlsw_create_ckt(port_id, peer_handle,
                               ssp_hdr->origin_mac, ssp_hdr->target_mac,
                               ssp_hdr->origin_sap, ssp_hdr->target_sap,
                               rif, mtu_size);
    if (dlsw_ptr) {
        dlsw_ptr->wan_pak = pak;
        dlsw_ptr->my_ckt = FALSE;
        dlsw_fsm(dlsw_ptr, dlswma_wan, dlsw_ssp_classify(ssp_op));
        if (dlsw_ptr->wan_pak)
            datagram_done(pak);
    } else {
        datagram_done(pak);
    }
}


csm_rc_t csm_to_core (csm_op_t op_type, u_portid_type_t *prp_id,
                      u_msg_ptr_t *msg_ptr, reach_entry_t *reach_entry)
{
    csm_rc_t       rc;

    if ((dlsw_disable) || (!dlsw_peer_active(0,0,0))) {
        CLSIMsgDestroy(msg_ptr->clsi_msg);
        return(PROC_OK);
    }

    if (op_type == CLSI_START_NEWDL) {
        rc = new_ckt_from_clsi(msg_ptr->clsi_msg, prp_id->port_id,
                          reach_entry->prp_id.peer_id,
                          reach_entry->mtu_size);
        return(rc);
    } else if (op_type == SSP_START_NEWDL) {
        new_ckt_from_wan(msg_ptr->pak, reach_entry->prp_id.port_id,
                         prp_id->peer_id, reach_entry->rif,
                         reach_entry->mtu_size);
        return(PROC_OK);
    } else {
        if (dlsw_core_debug_msgs)
            buginf("\ncsm_to_core(): invalid op %d from csm", op_type);
        return(FRAME_REJ);
    }
}


/***********************************************************************
 *                    CLSI MSG Processing Functions
 ***********************************************************************/


void disp_to_fsm(int msg_type,  TCLSIMsg *clsi_msg)
{
    ushort      cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
    dlsw_t      *dlsw_ptr;
    char        *str;
    DataInd_t	*data_ind;

    str = string_getnext();
    if (dlsw_core_debug_msgs)
        buginf("\n DLSW Received-disp : %s ",
               CLSIMsgSprintfShort(clsi_msg, str));

    /*
     * Check if this is a local switched circuit.
     */
    if (dlsw_is_local_ckt(clsi_msg))
        return;

    /*
     * Is the message for an existing core circuit?
     */
    dlsw_ptr = clsi2dlsw(clsi_msg);
    if (dlsw_ptr == NULL) {
        if (dlsw_core_debug_msgs)
            buginf("\nDLSw: disp_to_fsm() can't find circuit");
        CLSIMsgDestroy(clsi_msg);
        return;
    }

    switch (cls_op) {
      case CLS_UDATA | CLS_INDICATION:
        dlsw_ptr->clsi_msg = clsi_msg;
        dlsw_fsm(dlsw_ptr, dlswma_dlc, cls_op);
        if (dlsw_ptr->clsi_msg) {
            CLSIMsgDestroy(dlsw_ptr->clsi_msg);
            dlsw_ptr->clsi_msg = NULL;
        }
        break;

      case CLS_DATA | CLS_INDICATION:
        data_ind = (DataInd_t*) clsi_msg;
        dlsw_ptr->clsi_msg = clsi_msg;
        dlsw_send_ssp(dlsw_ptr, SSP_OP_infoframe,
                      data_ind->fData + data_ind->fReserveLength,
                      data_ind->fDataLength);
        /*
         * Above call should consume clsi_msg, but check anyway.
         */
        if (dlsw_ptr->clsi_msg) {
            CLSIMsgDestroy(dlsw_ptr->clsi_msg);
            dlsw_ptr->clsi_msg = NULL;
        }
        break;

      case CLS_FLOW | CLS_INDICATION:
        dlsw_ptr->clsi_msg = clsi_msg;
        dlsw_fsm(dlsw_ptr, dlswma_dlc, cls_op);
        if (dlsw_ptr->clsi_msg) {
            CLSIMsgDestroy(dlsw_ptr->clsi_msg);
            dlsw_ptr->clsi_msg = NULL;
        }
        break;

      default :
        errmsg(DLSWC_ERR_BADCLSIIND, CLSIMsgSprintfShort(clsi_msg, str),
               "disp_to_fsm", "");
        CLSIMsgDestroy(clsi_msg);
        break;
    }
}


static void dlsw_pre_proc_clsi (TCLSIMsg* clsi_msg)
{
    csm_rc_t            csm_ret_code;
    char                *str;
    dlsw_ring_t         *dlsw_ring_tmp;
    dlsw_t		*dlsw_ptr;
    ushort		cls_op;

    str = string_getnext();

    if (dlsw_core_debug_msgs)
        buginf("\n DLSW Received-ctlQ : %s ",
               CLSIMsgSprintfShort(clsi_msg, str));

    /*
     * Check if this is a local switched circuit.
     */
    if (dlsw_is_local_ckt(clsi_msg))
        return;

    /*
     * Is the message for an existing core circuit?
     */
    dlsw_ptr = clsi2dlsw(clsi_msg);
    if (dlsw_ptr) {
        dlsw_core_proc_clsi(dlsw_ptr, clsi_msg);
        return;
    }

    /*
     * Is the message for CSM?
     */
    if (dlsw_is_cstm_clsi_msg(clsi_msg)) {
        dlsw_ring_tmp = usapid2dgra(clsi_msg->fIdentifier);
        if (dlsw_ring_tmp == NULL) {
            errmsg(DLSWC_ERR_BADCLSISAP,
                   CLSIMsgSprintfShort(clsi_msg, str),
                   clsi_msg->fIdentifier, "dlsw_pre_proc_clsi");
          CLSIMsgDestroy(clsi_msg);
          return;
        } else if (dlsw_ring_tmp->enabled == FALSE) {
          CLSIMsgDestroy(clsi_msg);
          return;
        }

	csm_ret_code = csm_clsi_handler(dlsw_ring_tmp, clsi_msg);

        if (csm_ret_code != PROC_OK) {
            CLSIMsgDestroy(clsi_msg);
        }
        return;
    }

    /*
     * Is this a port initialization op?
     */
    cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
    if (dlsw_is_init_clsi_op(cls_op))  {
        dlsw_init_clsi_op_proc(clsi_msg);
        CLSIMsgDestroy(clsi_msg);
        return;
    }

    CLSIMsgDestroy(clsi_msg);
    return;
}


static void dlsw_core_proc_clsi (dlsw_t *dlsw_ptr, TCLSIMsg* clsi_msg)
{
    ushort cls_op;
    ulong clsi_cnf_ret_code;

    cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
    if ((cls_op & CLS_TYPE_MASK) == CLS_CONFIRM) {
        clsi_cnf_ret_code = CLSIMsgGetReturnCode(clsi_msg);
        if (clsi_cnf_ret_code != CLS_OK) {
            /*
             * Ignore XID response timeouts.
             */
            if (clsi_cnf_ret_code != CLS_NO_XID_RSP)
                dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_error);

            CLSIMsgDestroy(clsi_msg);
            return;
        }
    }

    dlsw_ptr->clsi_msg = clsi_msg;
    dlsw_fsm(dlsw_ptr, dlswma_dlc, cls_op);
    if (dlsw_ptr->clsi_msg) {
        CLSIMsgDestroy(dlsw_ptr->clsi_msg);
        dlsw_ptr->clsi_msg = NULL;
    }
}


/***********************************************************************
 *                    WAN MSG Processing Functions
 ***********************************************************************/


/*
 * Scans the list of circuits going the
 * just died peer and throw all the connections.
 * Do a graceful shut down of dlc side
 * so all llc/CLSI data strutures are nuked properly
 */
static void fsm_destroy_peer (peerhandle_t peer_handle)
{
    int n;
    dlsw_t *dlsw_ptr;

    for (n = 0; n < NDLSW_HASH_SIZE; n++) {
        dlsw_ptr = (dlsw_t *) dlsw_hashQ[n].qhead;
        for (; dlsw_ptr; dlsw_ptr = dlsw_ptr->next) {
            if (dlsw_ptr->peer_handle != peer_handle)
                continue;
            /*
             * Zero out the peer_handle to avoid the core using the handle
             * after the peer structure is freed.
             */
            dlsw_ptr->peer_handle = 0;
            dlsw_fsm(dlsw_ptr, dlswma_admin, dlswmi_admin_wan_fail);
        }
    }
}


static void dlsw_pre_proc_dlx (paktype *pak, peerhandle_t peer_handle)
{
    csm_rc_t            csm_ret_code;

    /*
     * Always assume DLX freames are for CSM, until core is DLX capable.
     */
    if (pak != NULL) {
	csm_ret_code = csm_dlx_handler(peer_handle, pak); /* consumes pak */
	if (csm_ret_code != PROC_OK) {
            buginf("\ndlsw_pre_proc_dlx(): csm_dlx_handler() failed %d",
		   csm_ret_code);
            datagram_done(pak);
	}
    }
    else {
	if (dlsw_csm_debug)
            buginf("\nError in DLX Message pak - NULL ptr");	    
    }
}


static void dlsw_pre_proc_ssp (paktype *pak, peerhandle_t peer_handle)
{
    dlsw_t		*dlsw_ptr;
    ssp_op_t            ssp_op;
    ssp_hdr_t           *ssp_hdr;
    csm_rc_t            csm_ret_code;
    boolean             isexpl;
    uchar               *str;
    ushort		cls_op;

    pak->enc_flags |= ENC_DLSW;
    str = string_getnext();
    ssp_hdr = (ssp_hdr_t *) pak->datagramstart;

    /*
     * Sanity checking of SSP Header.
     */
    if (!ssp_sanity_check_inline(ssp_hdr)) {
        datagram_done(pak);
        return;
    }

    ssp_op =  ssp_hdr->ssp_msg_type;
    isexpl = is_ssp_info_hdr(ssp_op) ? 0 : ssp_hdr->ssp_flags & SSP_FLAGS_EX;

    if (dlsw_core_debug_msgs) {
      errmsg(DLSWC_ERR_RECVSSP, ssp_op, dlsw_sspop2str(ssp_op),
             isexpl ? "-explorer" : "",
             peer_handle_to_str(peer_handle, str));
    }

    if (dlsw_is_cstm_ssp_op(ssp_hdr)) {
	if (pak != NULL) {
	    csm_ret_code = csm_ssp_handler (peer_handle, pak);
            if (csm_ret_code != PROC_OK) 
                datagram_done(pak);
        }
	else {
	    if (dlsw_csm_debug)
	        buginf("\nError in SSP Message pak - NULL ptr");
        }
	return;
    }	

    dlsw_ptr = remdlc2dlsw(ssp_hdr->rem_dlc_corr);
    if (dlsw_ptr) {
	/*
         * Echo Back SSP_OP_test_ckt_req for backward compatibility.
         * Don't add this to the fsm.
         */
        if (ssp_op == SSP_OP_test_ckt_req) {
            dlsw_send_ssp(dlsw_ptr, SSP_OP_test_ckt_rsp, NULL, 0);
            datagram_done(pak);
            return;
        }

        /*
         * Since 90% of ssp meaages can have flow control indicators,
         * it makes sense to handle this outside of the dlsw_fsm().
         */ 
        do_wan_inbound_fc(dlsw_ptr, (ssp_hdr_t *) pak->datagramstart);
      
        dlsw_ptr->wan_pak = pak;
        dlsw_fsm(dlsw_ptr, dlswma_wan, dlsw_ssp_classify(ssp_op));
        if (dlsw_ptr->wan_pak)
            datagram_done(pak);

        /*
         * This is a little ugly, but other alternatives are worse.
         * If we have reached the circuit established state
         * (reach ack back from partner), then it's time to process
         * any pending messages.
         *
         * This is better than the fsm calling itself or trying to
         * manage multiple "pending" events in one state.
         */
        if (dlsw_ptr->state == dlswst_established) {
            if (dlsw_ptr->pending_clsi_msg) {
                dlsw_ptr->clsi_msg = dlsw_ptr->pending_clsi_msg;
                dlsw_ptr->pending_clsi_msg = NULL;
                cls_op = CLSIMsgGetPrimitiveCode(dlsw_ptr->clsi_msg);
                dlsw_fsm(dlsw_ptr, dlswma_dlc, cls_op);

                if (dlsw_ptr->clsi_msg) {
                    CLSIMsgDestroy(dlsw_ptr->clsi_msg);
                    dlsw_ptr->clsi_msg = NULL;
                }
            }
        }
    } else {
        if (dlsw_core_debug_msgs)
            buginf("\nDLSw: dlsw_pre_proc_ssp() can't find circuit");
        datagram_done(pak);
    }
}


peer2core_ret_t peer_to_core(peer_msg_u *peer_msg)
{
    switch (peer_msg->p_recv.type) {
      case PEER_RECV_PAK :
        if (peer_msg->p_recv.header_type == PEER_SSP_TYPE)
            dlsw_pre_proc_ssp(peer_msg->p_recv.pak,
                              peer_msg->p_recv.peer_handle);
        else
            dlsw_pre_proc_dlx(peer_msg->p_recv.pak,
                              peer_msg->p_recv.peer_handle);
        break;
      case PEER_KILL_HANDLE :
        fsm_destroy_peer(peer_msg->p_kill_handle.peer_handle);
        csm_destroy_peer(peer_msg->p_kill_handle.peer_handle);
        break;
      case PEER_DEL_CACHE :
	csm_delete_cache(peer_msg->p_del_cache.peer_handle);
	break;
      case PEER_FLOW_CTRL_INDICATION :
        dlsw_mark_peerckts(peer_msg->p_flow_ind.peer_handle,
                           peer_msg->p_flow_ind.cong_level);
        break;
      default:
        errmsg(DLSWC_ERR_BADPEEROP, peer_msg->type);
        break;
    }
    return(CORE_SUCCESS);
}


/***********************************************************************
 *          DLC Initializion CLSi Msg Processing Functions
 ***********************************************************************/


static void dlsw_send_activate_ring (TCLSIMsg *clsi_msg)
{
  union         D_primitives dp;
  dlsw_ring_t   *dlsw_ring_tmp;
  dlsw_ring_t   *orig_dlsw_ring_tmp;
  boolean       orig_enabled;
  dlsw_sap_t    *dlsw_sap_tmp;
  ushort        port_type;
  ushort        trans_bridge_group;
  CLSPortID     port_id;
  hwidbtype     *hw_idb;
  idbtype       *sw_idb;
  char   *str;
  srb_triplet_t srb_triplet;
  dlsw_bgroup_t *dlsw_bgroup;

  str = string_getnext();

  port_id   = ((EnableCnf_t *)clsi_msg)->fPortId;
  port_type   = CLSIPortIdGetPortType(&port_id);

  sw_idb = CLSIPortIdGetSwIdb(&port_id);
  if (sw_idb == NULL ) {
      errmsg(DLSWC_ERR_BADCLSI,
             CLSIMsgSprintfShort(clsi_msg, str),
             "Sending Act Ring", "sw_idb is NULL");

      return;
  }

  if (port_type == CLS_TBRIDGE) { /* stash idb for deconfigure */
      dlsw_bgroup = dlsw_get_bgroup(sw_idb->span_index);
      if (dlsw_bgroup == NULL ) { /* dlsw_bgroup is allocated on config */
          errmsg(DLSWC_ERR_BADCLSI,
                 CLSIMsgSprintfShort(clsi_msg, str),
                 "Sending Act Ring", "dlsw_bgroup is NULL ");
          return;
      }

      dlsw_bgroup->tbridge_idb = sw_idb;
      trans_bridge_group = sw_idb->span_index;
      if (sw_idb->hwptr) {
         dlsw_bgroup->tbridge_idb->hwptr->sap_prilist = 
                                         dlsw_bgroup->sap_prilist_num;
         dlsw_bgroup->tbridge_idb->hwptr->sna_la_prilist = 
                                         dlsw_bgroup->sna_la_prilist_num;
         dlsw_assign_bgroup_llc2_params(sw_idb, dlsw_bgroup);
      }
  } else {
      trans_bridge_group = 0;
  }

  hw_idb = sw_idb->hwptr;
  if (hw_idb == NULL ) {
      errmsg(DLSWC_ERR_BADCLSI,
             CLSIMsgSprintfShort(clsi_msg, str),
             "Sending Act Ring", "hw_idb is NULL ");
      return;
  }
  
  srbV.srb_sb_get_triplet(sw_idb, &srb_triplet);
  dlsw_ring_tmp = dlsw_get_ring(&port_id, trans_bridge_group);

  /*
   * Each QLLC Sap is a DLSW ring, and we can have multiple QLLC Saps per
   * swidb, so always allocate this port structure if the DLC is QLLC.
   */
  orig_dlsw_ring_tmp = dlsw_ring_tmp = dlsw_get_ring(&port_id, 
                                                     trans_bridge_group);
  if (orig_dlsw_ring_tmp)
      orig_enabled = dlsw_ring_tmp->enabled;
  else
      orig_enabled = FALSE;

  if ((port_type == CLS_QLLC) || (dlsw_ring_tmp == NULL)) {
      dlsw_ring_tmp = malloc_named(sizeof(dlsw_ring_t), "DLSW-RING");
      if (!dlsw_ring_tmp) {
          printf(nomemory);
          return;
      }
      enqueue(&dlsw_ringQ, dlsw_ring_tmp);
  }

  dlsw_sap_tmp = dlsw_get_sap(dlsw_ring_tmp, DLSW_RING_ACTIVE);
  if (!dlsw_sap_tmp) {
    dlsw_sap_tmp = malloc_named(sizeof(dlsw_sap_t), "DLSW-SAP");
    if (!dlsw_sap_tmp) {
      return;
    }
    dlsw_sap_tmp->sap = DLSW_RING_ACTIVE;
    dlsw_sap_tmp->enabled = FALSE;
    enqueue(&(dlsw_ring_tmp->dlsw_sapQ), dlsw_sap_tmp);
  }
  dlsw_ring_tmp->ring_number = srb_triplet.localRing;

  /*
   * because the srb_targetring and srb_bridge_num will be used by RSRB,
   * DLSw needs to use different target ring and bridge num.
   */
  if (port_type == CLS_RSRB) {
      dlsw_ring_tmp->bridge_number = reg_invoke_cls_get_rsrb_bridge(srb_triplet.localRing);
      dlsw_ring_tmp->ring_group = reg_invoke_cls_get_rsrb_ring(srb_triplet.localRing);
  } else {
      dlsw_ring_tmp->bridge_number = srb_triplet.bridgeNum;
      dlsw_ring_tmp->ring_group    = srb_triplet.remoteRing;
  }
  dlsw_ring_tmp->port_type   = port_type;
  dlsw_ring_tmp->trans_bridge_group = trans_bridge_group;
  if (port_type == CLS_TBRIDGE) {
      if (dlsw_core_debug_state)
          buginf("\nCore: CLS_TBRIDGE mtu %d, setting to 1500",
                  dlsw_ring_tmp->mtu_size);
      dlsw_ring_tmp->mtu_size = RC_LF1500_VAL;
  } else if ((port_type == CLS_QLLC) ||
             (port_type == CLS_SDLC) ||
             (port_type == CLS_VDLC)) {
      dlsw_ring_tmp->mtu_size   = RC_LF4472_VAL;
  } else {
      dlsw_ring_tmp->mtu_size   = llc_mtu(hw_idb);
  }
  
  dlsw_ring_tmp->swidb = sw_idb;

  /*
   * If this port type does not negotiate an lf (non-rif media is most
   * likely scenario), then set port flag so CSM can provide appropriate
   * lf checking.
   */
  if (dlsw_ignore_port_lf_type(port_type))
      dlsw_ring_tmp->ignore_port_lf = TRUE;
  else
      dlsw_ring_tmp->ignore_port_lf = FALSE;

  if ((port_type != CLS_QLLC) && orig_dlsw_ring_tmp && orig_enabled) {
      /*
       * The port was already enabled. We've already recorded the new
       * port parms, do not send a duplicate activate ring.
       */
      if (dlsw_core_debug_state)
          buginf("\nDLSw-CORE : Not sending duplicate activate ring to %s",
                 sw_idb->hwptr->hw_namestring);
      return;
  }

  dlsw_ring_tmp->enabled = FALSE; /* set to TRUE when cnf comes back */
  csm_activate_static(dlsw_ring_tmp); /* activate static CSM entries */

  dp.type = D_ACTIVATERING_REQ;
  dp.disp_activatering_req.u_sap_id = (uint)dlsw_ring_tmp;
  /*
   * because the srb_targetring and srb_bridge_num will be used by RSRB,
   * DLSw needs to use different target ring and bridge num.
   */
  if (port_type == CLS_RSRB) {
  dp.disp_activatering_req.source_ring   = srb_triplet.localRing;
  dp.disp_activatering_req.bridge_number =
                      reg_invoke_cls_get_rsrb_bridge(srb_triplet.localRing);
  dp.disp_activatering_req.target_ring   =
                      reg_invoke_cls_get_rsrb_ring(srb_triplet.localRing);
  } else {
      dp.disp_activatering_req.source_ring   = srb_triplet.localRing;
      dp.disp_activatering_req.bridge_number = srb_triplet.bridgeNum;
      dp.disp_activatering_req.target_ring   = srb_triplet.remoteRing;
  }
  dp.disp_activatering_req.port_type     = port_type;
  dp.disp_activatering_req.swidb         = sw_idb;
  if (dlsw_core_debug_state)
    buginf("\nDLSw-CORE : Sending activate ring to %s",
           sw_idb->hwptr->hw_namestring);
  fsm_to_disp(&dp);
}


static void dlsw_init_clsi_op_proc (TCLSIMsg* clsi_msg)
{
    ushort              cls_op = CLSIMsgGetPrimitiveCode(clsi_msg);
    ulong               clsi_cnf_ret_code = CLSIMsgGetReturnCode(clsi_msg);
    char                *str;
    union D_primitives  dp;
    dlsw_ring_t         *dlsw_ring_tmp;
    dlsw_sap_t		*dlsw_sap_tmp;

    if ((cls_op & CLS_TYPE_MASK) != CLS_CONFIRM)
        return;

    str = string_getnext();
    if (clsi_cnf_ret_code != CLS_OK) {
        if (cls_op == (CLS_ACTIVATE_RING | CLS_CONFIRM)) {
            dlsw_ring_tmp = usapid2dgra(clsi_msg->fIdentifier);
            if (!dlsw_ring_tmp)
                return;

            /* When CSCdi58393 is fixed, we should simply call
             * csm_deactivate_ring(dlsw_ring_tmp) instead of 
             * setting the port and sap enabled fields to FALSE.
             */
            dlsw_ring_tmp->enabled = FALSE;
            dlsw_sap_tmp = dlsw_get_sap(dlsw_ring_tmp, DLSW_RING_ACTIVE);
            if (dlsw_sap_tmp)
                dlsw_sap_tmp->enabled = FALSE;
            return;
        }
        errmsg(DLSWC_ERR_BADCLSIRET,
               CLSIMsgSprintfShort(clsi_msg, str),
               clsi_cnf_ret_code, "init_clsi_op_proc", "");

        /* Cleanup missing!!! */
        return;
    }

    switch (cls_op) {
      case CLS_ENABLE  | CLS_CONFIRM :
        dlsw_send_activate_ring(clsi_msg);
        break;

      case CLS_DISABLE  | CLS_CONFIRM :
        if (dlsw_core_debug_state)
            buginf("\nDLSw-CORE : Received Disable confirm");
        break;

      case CLS_ACTIVATE_RING | CLS_CONFIRM :
        dlsw_ring_tmp = usapid2dgra(clsi_msg->fIdentifier);
        if (dlsw_ring_tmp) {
            dlsw_ring_tmp->p_sap_id =
                           ((ActRngCnf_t *)clsi_msg)->fCLSIHdr.fPSapId;
            dlsw_ring_tmp->enabled = TRUE; /* successfully enabled */
            if (dlsw_core_debug_state)
                buginf("\nDLSw Received ActRingcnf from %s",
                       dlsw_ring_tmp->swidb->hwptr->hw_namestring);
            dlsw_sap_tmp = dlsw_get_sap(dlsw_ring_tmp, DLSW_RING_ACTIVE);
            if (dlsw_sap_tmp)
                dlsw_sap_tmp->enabled = TRUE;
        } else
            errmsg(DLSWC_ERR_BADCLSISAP,
                   CLSIMsgSprintfShort(clsi_msg, str),
                   clsi_msg->fIdentifier, "dlsw_pre_proc_clsi - ActRngCnf");
        break;

      case CLS_DEACTIVATE_RING | CLS_CONFIRM :
        /*
         * Do not dequeue the dlsw_ring_t ever.
         * CSM depends on the availability of these structures
         * and they are stored somewhere.
         * The next time around when this sw_idb is enabled
         * it will get reused.
         */
        dlsw_ring_tmp = usapid2dgra(clsi_msg->fIdentifier);
        if (dlsw_ring_tmp == NULL) {
            errmsg(DLSWC_ERR_BADCLSISAP,
                   CLSIMsgSprintfShort(clsi_msg, str),
                   clsi_msg->fIdentifier,
                   "dlsw_pre_proc_clsi- DeActRngCnf");
            return;
        }
        if (dlsw_core_debug_state)
            buginf("\nDLSw-CORE : Received Deactivate ring confirm from %s",
                   dlsw_ring_tmp->swidb->hwptr->hw_namestring);
        dlsw_ring_tmp->enabled = FALSE;
        dlsw_sap_tmp = dlsw_get_sap(dlsw_ring_tmp, DLSW_RING_ACTIVE);
        if (dlsw_sap_tmp)
            dlsw_sap_tmp->enabled = FALSE;
        dp.type = D_DISABLE_REQ;
        dp.disp_disable_req.port_type = dlsw_ring_tmp->port_type;
        dp.disp_disable_req.swidb     = dlsw_ring_tmp->swidb;
        fsm_to_disp(&dp);
        break;

      default :
        errmsg(DLSWC_ERR_BADCLSICNF,
               CLSIMsgSprintfShort(clsi_msg, str),
               "init_clsi_op_proc - default", "");
        break;
    }
}

int dlsw_get_circuit_creates ()
{
    return dlsw_circuit_creates;
}

int dlsw_get_active_circuits ()
{
    return dlsw_active_circuits;
}
