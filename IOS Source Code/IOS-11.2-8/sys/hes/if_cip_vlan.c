/* $Id: if_cip_vlan.c,v 3.13.8.9 1996/08/26 15:07:14 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_vlan.c,v $
 *------------------------------------------------------------------
 * if_cip_vlan.c  Channel Interface Processor Virtual Lan support
 *
 * April 1995, Karen Mitchell
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_cip_vlan.c,v $
 * Revision 3.13.8.9  1996/08/26  15:07:14  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.8.8  1996/08/07  08:59:06  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.8.7  1996/08/05  15:44:56  lmenditt
 * CSCdi55451:  Disallow CSNA Internal LAN configuration for FDDI and
 * ETHERNET
 * Branch: California_branch
 *
 * Revision 3.13.8.6  1996/07/09  05:51:16  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.8.5  1996/06/27  09:28:36  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.13.8.4  1996/06/06  18:17:43  motto
 * CSCdi58517:  CSNA Virtual Port x/2 hung in down/down state after
 * noShut/micro rel
 * Branch: California_branch
 *
 * Revision 3.13.8.3  1996/05/29  18:25:51  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.13.8.2  1996/05/17  11:18:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.6  1996/05/05  23:07:10  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.5  1996/04/26  15:18:46  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.4  1996/04/03  14:29:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.3  1996/03/21  06:08:03  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.11.2.2  1996/03/17  17:49:20  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.2.1  1996/03/15  05:55:38  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.13.8.1  1996/03/18  19:41:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.5  1996/03/16  06:50:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.4  1996/03/13  01:21:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.3  1996/03/07  09:39:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.2  1996/02/20  00:51:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/28  19:47:57  motto
 * CSCdi45011:  Get hung output on CIP virtual interface
 *
 * Revision 3.12  1996/02/27  21:30:55  kmitchel
 * CSCdi47478:  Parser allows more than eight chars for CIP internal
 *              adapter name
 *
 * Revision 3.11  1996/02/13  01:33:54  motto
 * Spontaneous cbus complex restarts running CIP CSNA and
 * also out stuck messages.
 * CSCdi47536:  CIP failure in 7513 - CY-Bus problems
 *
 * Revision 3.10  1996/02/07  16:11:32  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/01/31  21:15:36  motto
 * CSCdi47239:  DLSW+ doesnt forward test frames to CIP internal lan
 *
 * Revision 3.8  1996/01/30  19:45:25  jmurray
 * CSCdi46773:  local ring should not be allowed as a ring group for csna
 * Code was added to detect if an internal lan was being configured for
 * the CIP which matched a configured a ring group.  If a match is
 * noticed, the internal lan configuration is backed out.
 *
 * Revision 3.7.2.1  1996/01/24  21:46:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/18  22:47:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/12  04:26:38  motto
 * CSCdi46474:  Unable to send TEST to CIP from local SDLLC - Bad VCN
 *
 * Revision 3.5  1995/12/30  00:26:51  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.4  1995/12/05  15:51:46  ppearce
 * CSCdi42839:  LLC2 connection to CSNA internal LAN not established on RSP
 *
 * Revision 3.3  1995/11/17  09:17:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.33  1995/10/09  23:44:48  motto
 * CSCdi40680:  OIR with CIP internal LANs, Not all internal LANs get
 * added back
 *
 * Revision 2.32  1995/10/05  17:33:54  motto
 * Close window on BADVCN error message.
 * Fix SNMP query of CIP daughter boards. Was returning the virtual
 * interface as a daughter board. Remove 'debug channel trap' from
 * parser. This was not implemented to do anything.
 * CSCdi39794:  %MEMD-3-BADVCN error messages after CIP mic reload
 *
 * Revision 2.31  1995/09/15  05:57:07  ppearce
 * CSCdi40438:  Cannot config LLC parms for RSRB/LAK on CSNA virtual i/f
 *
 * Revision 2.30  1995/09/11  10:23:50  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.29  1995/09/07  03:10:11  motto
 * CSCdi39129:  Make the source-bridge config cmd on CIP ILANs work like
 * TRIP/FIP
 *
 * Revision 2.28  1995/09/05  02:58:05  ppearce
 * CSCdi39775:  Memory leak of explorer traffic from CSNA
 *   Return explorer pak to system memory if not destined for CSNA ILAN
 *
 * Revision 2.27  1995/09/01  15:58:49  ppearce
 * CSCdi39719:  CIP support for DLSw+ (TCP encapsulation only)
 *
 * Revision 2.26  1995/09/01  06:01:59  ppearce
 * CSCdi39705:  MEMD-3-BADVCN when using CSNA with RSRB/TCP local-ack
 *   Re-work how CSNA header is added before sending pak to CIP interface
 *
 * Revision 2.25  1995/09/01  05:55:30  ppearce
 * CSCdi39704:  Incorrect re-init of oqueue vectors in CIP ILAN IDB
 *   Move initialization of vectors after common init routines
 *
 * Revision 2.24  1995/08/29  23:54:52  motto
 * CSCdi39129:  Make the source-bridge config cmd on CIP ILANs work like
 * TRIP/FIP
 *
 * Revision 2.23  1995/08/25  11:43:42  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.22  1995/08/22 17:18:41  motto
 * CSCdi37937:  CIP sysmgt RPCs timeout when all interfaces shutdown
 *
 * Revision 2.21  1995/08/13  19:33:31  motto
 * CSCdi36478:  Show interface statistical info is missing for Channel
 * interface
 *
 * Revision 2.20  1995/08/12  07:08:54  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.19  1995/08/09  21:33:26  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.18  1995/08/07  17:01:45  motto
 * CSCdi38327:  Router stops forwarding explorers to the CIP LANs
 *
 * Revision 2.17  1995/08/04  14:10:13  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.16  1995/08/01  21:58:32  motto
 * Needed to add support in the srb-explorerfastswitching code for
 * the CIP interanl LAN interfaces.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.15  1995/07/25  22:18:33  motto
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.14  1995/07/23  16:25:55  motto
 * Modify CIP SRB fastswitch to allow CIPxLANa-2-CIPyLANa
 * and CIPxLANa-2-CIPxLANb.
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.13  1995/07/20  22:16:20  ppearce
 * CSCdi37431:  SR/TLB-to-CIP not working
 *
 * Revision 2.12  1995/07/20  21:31:21  ppearce
 * CSCdi37431:  SR/TLB-to-CIP not working
 *
 * Revision 2.11  1995/07/20  19:53:01  ppearce
 * CSCdi37405:  CIP-to-RSRB fastswitching broken for direct/fst/ftcp encaps
 *
 * Revision 2.10  1995/07/20  13:34:00  motto
 * CSCdi37360:  CIP SNA LLC CC state display incorrect
 *
 * Revision 2.9  1995/07/18  21:46:30  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.8  1995/07/13  19:41:55  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.7  1995/06/25  21:26:30  kmitchel
 * CSCdi35580:  CIP-SNA Virtual Lan: verbiage change - virtual to internal
 *              also includes fix for CSCdi35538 - display all CLAW lnks
 *              for offload statistics.
 *
 * Revision 2.6  1995/06/23  21:40:23  motto
 * CSCdi36373:  CIP SRB fastswitching breaks RSP and AGS images
 *
 * Revision 2.5  1995/06/22  15:34:21  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.4  1995/06/21  20:28:12  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.3  1995/06/19  07:16:12  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  00:03:27  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:41:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "../os/clock.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "../srt/srt_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "if_mci.h"                      /* XXX */
#include "if_fci.h"
#include "if_hes_common.h"
#include "if_cip.h"
#include "if_cip_vlan.h"
#include "../if/channel.h"
#include "channel_sb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "logger.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "../os/free.h"
#include "../parser/parser_defs_channel.h"
#include "../if/channel_debug.h"
#include "if_cip_sna.h"
#include "../if/if_controller.h"
#include "hes_controller.h"
#include "../if/tring.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"


/*
 * ----------------------------------------------------------------------------
 * CHANNEL VLAN DATA STRUCTURES: Virtual Lans, Virtual Adapters, LLC2
 * parameters
 * ----------------------------------------------------------------------------
 */

/*
 * LLC2 parameters used by the CIP LLC2 stack
 * 
 *        LLC MIB              CIP cisco router     IBM      CIP LLC2 stack 
 * =====================  ======================  ========  ==================
 * AckTimer               t1-time        T1_ack      T1      fAckTimer       
 * PbitTimer              tpf-time       T1_pf       T1      fPFTimer
 * RejTimer               trej-time      T1_rej      T1      fRejTimer
 * BusyTimer              tbusy-time     T1_busy     T1      fBusyTimer
 * InactTimer             idle-time      Ti          Ti      fTi
 * MaxUnackedIPDUsSend    local-window   Tw          Tw      fTw
 * MaxUnackedIPDUsRcv                    Rw          Rw      fRw    
 * MaxRetransmits         N2             N2          N2      fN2      
 * MaxPDUOctets                          N1          N1      fN1      
 * DelayAckTimer          ack-delay-time T2          T2      fT2    
 * DelayAckCount          ack-max        N3          N3      fN3      
 * Nw                     dynwind, nw    Nw          Nw      fNw      
 */ 

typedef struct vadapter_llc2_ {
    ulong llc2_T1_ack;    /* T1 Acknowledgement Timer                  */
    ulong llc2_T1_pf;     /* T1 P/F Timer                              */
    ulong llc2_T1_rej;    /* T1 Rej Timer                              */
    ulong llc2_T1_busy;   /* T1 Busy Timer                             */
    ulong llc2_Ti;        /* Ti Inactivity Timer                       */
    ulong llc2_Tw;	  /* Tw Transmit window size (MAXOUT)          */
    ulong llc2_Rw;	  /* Rw Receive  window size (MAXIN)           */
    ulong llc2_N2;        /* N2 Max retry count                        */
    ulong llc2_N1;        /* N1 Max I-field length                     */
    ulong llc2_T2;        /* T2 Ack delay timer                        */
    ulong llc2_N3;        /* N3 Ack delay count                        */
    ulong llc2_Nw;        /* Nw Congestion control slow-start interval */ 
} vadapter_llc2;

/*
 * VLAN information
 */

#define VLAN_MAX_VADAPTERS     (CSNA_VLAN_MAX_VADAPTER_NUM+1)
#define VLAN_MAX_VLANS_OF_TYPE (CSNA_VLAN_MAX_LANID_NUM+1)
#define VLAN_MAX_VLANS         (CSNA_VLAN_MAXTYPE * VLAN_MAX_VLANS_OF_TYPE)

#define VADAPTER_NAME_LEN       8
#define VADAPTER_NAME_ZLEN     (VADAPTER_NAME_LEN + 1) 

typedef struct vadapter_element_ {
    struct vadapter_element_ *next;       /* ptr to next Adapter on CIP LAN  */
    struct vlan_element_ *ilan;           /* ptr to the ILAN this adap is on */
    uchar  vadapMacaddr[IEEEBYTES];       /* macaddr                         */
    vadapter_llc2 vllc2;                  /* llc2 parameters                 */
    ushort vadapAdapno;                   /* Relative Adapter Number         */
    ushort vadapVC;                       /* Virtual Circuit Number (VC)     */
    ushort vadapVlanLanId;                /* Vlan on which this Adapter exists */
    char   vadapName[VADAPTER_NAME_ZLEN]; /* String name                     */
    ushort vadapState;                    /* Virtual Adapter state           */
} vadapter_element;

#define VADAP_STATE_UNUSED  0x0000        /* Used only to reset STATE        */
#define VADAP_STATE_INUSE   0x0001        /*                                 */
#define VADAP_STATE_CFGING  0x0002        /*                                 */
#define VADAP_STATE_CREATE  0x0004        /* Used in combination with CFGING */
#define VADAP_STATE_UPDATE  0x0008        /*                                 */
#define VADAP_STATE_DELETE  0x0010        /*                                 */
#define VADAP_STATE_PNDING  0x0020        /* Pending acknowledgement from CIP*/
#define VADAP_STATE_ACKED   0x0040        /*                                 */

typedef struct vlan_element_ {
    hwidbtype *ilan_hwidb;                 /* hwidb representing this CIP LAN */
    ushort  vlanLanAttributes;             /* see defines below              */
    ushort  vlanBridgeGrp;                 /* bridge group info              */
    ushort  vlanLocalRing;                 /* source bridge info             */
    ushort  vlanBridgeNum;                 /* source bridge info             */
    ushort  vlanTargetRing;                /* source bridge info             */
    ushort  vlanNumAdapters;               /* Count of configured vadapters  */ 
    ulong   vlanVadapterMask;              /* BitMask for claiming vadapters */
    ushort  vlanState;                     /* State flags                    */
    ushort  vlanLanType;                   /* Lan type                       */
    ushort  vlanLanId;                     /* Lan ID                         */
    vadapter_element *adapter_chain;       /* chain of adapters on this ILAN */
} vlan_element;

#define VLAN_ATTRB_SRB_OK  0x0001          /* Okay to cfg as source-bridge   */
#define VLAN_ATTRB_TB_OK   0x0002          /* Okay to cfg as tb              */
#define VLAN_ATTRB_SRB     0x0004          /* Is configured  source-bridge   */
#define VLAN_ATTRB_TB      0x0008          /* Is configured  tb              */

#define VLAN_STATE_UNUSED  0x0000          /* Used only to reset STATE       */
#define VLAN_STATE_INUSE   0x0001          /*                                */
#define VLAN_STATE_CFGING  0x0002          /*                                */
#define VLAN_STATE_CREATE  0x0004          /* Used in combination with CFGING */
#define VLAN_STATE_UPDATE  0x0008          /*                                */
#define VLAN_STATE_DELETE  0x0010          /*                                */
#define VLAN_STATE_PNDING  0x0020          /* Pending acknowledgement from CIP */
#define VLAN_STATE_ACKED   0x0040          /*                                */

typedef struct cip_vlan_type_ {
    vadapter_element vlansVadapters[VLAN_MAX_VADAPTERS];
    vlan_element     vlansVlans[VLAN_MAX_VLANS_OF_TYPE]; 
} cip_vlan_type;

typedef struct csna_vlans_ {
    ushort        vlanMaxLLC2sess; /* Restrictive Max concurrent LLC2 sessions */
    cip_vlan_type vlanTypes[CSNA_VLAN_MAXTYPE]; /* Virtual lan info Ether Token FDDI */
} csna_vlans;


/*
 * ----------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------
 */
#define MAX_VADAPTER_VC \
        (cip->vc_base + \
        ((CSNA_VLAN_MAXTYPE-1) * (VLAN_MAX_VADAPTERS)) +\
        (CSNA_VLAN_MAX_VADAPTER_NUM))

#define MIN_VADAPTER_VC \
        (cip->vc_base)

#define ASSIGN_VADAPTER_VC(lan_type, adapno)\
       (cip->vc_base + ((lan_type) * (VLAN_MAX_VADAPTERS)) + (adapno))

#define VADAPTER_FROM_VC(vcnum)\
       (((vcnum) - cip->vc_base) % (VLAN_MAX_VADAPTERS))

#define LANTYPE_FROM_VC(vcnum)\
       (((vcnum) - cip->vc_base) / (VLAN_MAX_VADAPTERS))

#define SHOWLLC2(name, curr_val, def_val)\
       (printf("%9s =%-5d  ",\
	       (name), (curr_val)))

#define NVWRITE(field, def_val)\
       (nv_write(((field) != (def_val)), "  %s %d", csb->nv_command, (field)))

/*
 * ----------------------------------------------------------------------------
 * Prototypes
 * ----------------------------------------------------------------------------
 */
static boolean csna_send_max_llc2_sess_command(idbtype *, csna_vlans *);
static boolean csna_send_vadapter_cfg(idbtype *, vlan_element *,
				      vadapter_element *);
static void csna_bad_cfg_ack(char *, vlan_element *, vadapter_element *,
			     uchar *, ulong);
static void csna_unexpected_cfg_ack(idbtype *, char *, uchar *, ulong);

static void cip_setup_srb_init(hwidbtype *idb);
static boolean cip_setup_srb_info(hwidbtype *hwidb, ushort rn, ushort br, 
				  ushort trn, boolean build);
static void cip_setup_tb_info(hwidbtype *hwidb, ushort bg, boolean build);
static hwidbtype *cip_vlan_get_idb(hwidbtype *owner_hwidb);
static void cip_vlan_free_idb(hwidbtype *hwidb, hwidbtype *ilan_idb);
static void cip_ilan_show_interface(hwidbtype *hwidb);
static ushort cip_ilan_vc_from_macaddr(hwidbtype *hwidb, uchar *dmac, uchar *smac);
static void cip_ilan_cstate( hwidbtype *hwidb);
static void cip_vlan_remove_srb(vlan_element *pVlanElement, idbtype *swidb);
static void cip_vlan_remove_tb(vlan_element *pVlanElement, idbtype *swidb);
static void cip_remove_vadapter_from_ilan(vlan_element *pVlanElement, 
                                          vadapter_element *pVadapElement);


/*
 * ---------------------------------------------------------------------------
 * Local Variables and Defines
 * ---------------------------------------------------------------------------
 */
/* Parser anchor points for User Data Block */
#define UDB_VLAN  0
#define UDB_VADAP 1

#define VADAP_PROMPT_BASE          "cfg-adap-"
#define VLAN_PROMPT_BASE           "cfg-lan-"
char VlanPrompt[MAXPROMPTLEN+1];
char VadapPrompt[MAXPROMPTLEN+1];

char *pVlanPrompt = VlanPrompt;
char *pVadapPrompt = VadapPrompt;

static char *const csna_vlan_types_short[CSNA_VLAN_MAXTYPE+1] = {
    "Ether",
    "Token",
    "FDDI",
    "Undef",
};

static char *const csna_vlan_types[CSNA_VLAN_MAXTYPE+1] = {
    "Ethernet",
    "TokenRing",
    "FDDI",
    "Undefined",
};

/*
 * Debug string for no virtual interface as output interface of ilan
 */
static const char no_vidb[] = "\nCIP ILAN(%s-%s) is not registered with"
                              " virtual interface - %s";

/*
 * This is the CIP internal LAN hwidb free queue
 */
static queuetype ilan_free_hwidbQ;
static queuetype *ilan_free_hwidbQ_ptr = &ilan_free_hwidbQ;


/*
 * ---------------------------------------------------------------------------
 * CHANNEL SNA LLC2 
 * ---------------------------------------------------------------------------
 */

static void csna_llc2_defaults (vadapter_element *pVadapElement)
{
    pVadapElement->vllc2.llc2_T1_ack    = CSNA_LLC2_DEF_T1_ACK;
    pVadapElement->vllc2.llc2_T1_pf     = CSNA_LLC2_DEF_T1_PF;
    pVadapElement->vllc2.llc2_T1_rej    = CSNA_LLC2_DEF_T1_REJ;
    pVadapElement->vllc2.llc2_T1_busy   = CSNA_LLC2_DEF_T1_BUSY;
    pVadapElement->vllc2.llc2_Ti        = CSNA_LLC2_DEF_TI;
    pVadapElement->vllc2.llc2_Tw        = CSNA_LLC2_DEF_TW;
    pVadapElement->vllc2.llc2_Rw        = CSNA_LLC2_DEF_RW;
    pVadapElement->vllc2.llc2_N2        = CSNA_LLC2_DEF_N2;
    pVadapElement->vllc2.llc2_N1        = CSNA_LLC2_DEF_N1;
    pVadapElement->vllc2.llc2_T2        = CSNA_LLC2_DEF_T2;
    pVadapElement->vllc2.llc2_N3        = CSNA_LLC2_DEF_N3;
    pVadapElement->vllc2.llc2_Nw        = CSNA_LLC2_DEF_NW;
}

static void display_this_vadap_llc2 (
    void *pThisVlan,
    void *pThisVadap)
{
    printf("\n\n  Lan %5s %-2d adapter %-3d %14e",
	   get_vlan_lan_type_name(pThisVlan, VLAN_NAME_ABBREV),
	   get_vlan_lan_id(pThisVlan),
	   get_vadapt_adapt_no(pThisVadap),
	   get_vadapt_mac_address(pThisVadap));

    printf("\n  ");
    SHOWLLC2(CSNA_LLC2_KEY_T1_ACK,
	     get_vadapt_llc2_T1_ack(pThisVadap),
	     CSNA_LLC2_DEF_T1_ACK);

    SHOWLLC2(CSNA_LLC2_KEY_T1_PF,
	     get_vadapt_llc2_T1_pf(pThisVadap),
	     CSNA_LLC2_DEF_T1_PF);

    SHOWLLC2(CSNA_LLC2_KEY_T1_REJ,
	     get_vadapt_llc2_T1_rej(pThisVadap),
	     CSNA_LLC2_DEF_T1_REJ);
    
    SHOWLLC2(CSNA_LLC2_KEY_T1_BUSY,
	     get_vadapt_llc2_T1_busy(pThisVadap),
	     CSNA_LLC2_DEF_T1_BUSY);

    printf("\n  ");
    SHOWLLC2(CSNA_LLC2_KEY_TI,
	     get_vadapt_llc2_Ti(pThisVadap),
	     CSNA_LLC2_DEF_TI);

    SHOWLLC2(CSNA_LLC2_KEY_TW,
	     get_vadapt_llc2_Tw(pThisVadap),
	     CSNA_LLC2_DEF_TW);

    SHOWLLC2(CSNA_LLC2_KEY_RW,
	     get_vadapt_llc2_Rw(pThisVadap),
	     CSNA_LLC2_DEF_RW);

    SHOWLLC2(CSNA_LLC2_KEY_N2,
	     get_vadapt_llc2_N2(pThisVadap),
	     CSNA_LLC2_DEF_N2);

    printf("\n  ");
    SHOWLLC2(CSNA_LLC2_KEY_N1,
	     get_vadapt_llc2_N1(pThisVadap),
	     CSNA_LLC2_DEF_N1);

    SHOWLLC2(CSNA_LLC2_KEY_T2,
	     get_vadapt_llc2_T2(pThisVadap),
	     CSNA_LLC2_DEF_T2);
    
    SHOWLLC2(CSNA_LLC2_KEY_N3,
	     get_vadapt_llc2_N3(pThisVadap),
	     CSNA_LLC2_DEF_N3);

    SHOWLLC2(CSNA_LLC2_KEY_NW,
	     get_vadapt_llc2_Nw(pThisVadap),
	     CSNA_LLC2_DEF_NW);
}

static inline boolean csna_llc2_val_change (
   ulong *pVadapLLC2field,
   ulong llc2_value)
{
    if (*pVadapLLC2field != llc2_value) {
	*pVadapLLC2field = llc2_value;
	return TRUE;
    }
    return FALSE;
}


/*
 * ----------------------------------------------------------------------------
 * csna_llc2_command
 * llc2 llc2-parm llc2-value
 *
 * OBJ(int,1) = llc2-value
 * csb->which = llc2-parm
 *
 */
void csna_llc2_command (
    parseinfo *csb)
{
    boolean           changed;
    vadapter_element *pVadapElement;
    
    pVadapElement = csb->userDataBlock[UDB_VADAP];

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	switch (csb->which) {
	case CSNA_LLC2_T1_ACK:
	    NVWRITE(pVadapElement->vllc2.llc2_T1_ack,
		    CSNA_LLC2_DEF_T1_ACK);
	    break;
	case CSNA_LLC2_T1_PF:
	    NVWRITE(pVadapElement->vllc2.llc2_T1_pf,
		    CSNA_LLC2_DEF_T1_PF);
	    break;
	case CSNA_LLC2_T1_REJ:
	    NVWRITE(pVadapElement->vllc2.llc2_T1_rej,
		    CSNA_LLC2_DEF_T1_REJ);
	    break;
	case CSNA_LLC2_T1_BUSY:
	    NVWRITE(pVadapElement->vllc2.llc2_T1_busy,
		    CSNA_LLC2_DEF_T1_BUSY);
	    break;
	case CSNA_LLC2_TI:
	    NVWRITE(pVadapElement->vllc2.llc2_Ti,
		    CSNA_LLC2_DEF_TI);
	    break;
	case CSNA_LLC2_TW:
	    NVWRITE(pVadapElement->vllc2.llc2_Tw,
		    CSNA_LLC2_DEF_TW);
	    break;
	case CSNA_LLC2_RW:
	    NVWRITE(pVadapElement->vllc2.llc2_Rw,
		    CSNA_LLC2_DEF_RW);
	    break;
	case CSNA_LLC2_N2:
	    NVWRITE(pVadapElement->vllc2.llc2_N2,
		    CSNA_LLC2_DEF_N2);
	    break;
	case CSNA_LLC2_N1:
	    NVWRITE(pVadapElement->vllc2.llc2_N1,
		    CSNA_LLC2_DEF_N1);
	    break;
	case CSNA_LLC2_T2:
	    NVWRITE(pVadapElement->vllc2.llc2_T2,
		    CSNA_LLC2_DEF_T2);
	    break;
	case CSNA_LLC2_N3:
	    NVWRITE(pVadapElement->vllc2.llc2_N3,
		    CSNA_LLC2_DEF_N3);
	    break;
	case CSNA_LLC2_NW:
	    NVWRITE(pVadapElement->vllc2.llc2_Nw,
		    CSNA_LLC2_DEF_NW);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    
    changed = FALSE;
    switch (csb->which) {
    case CSNA_LLC2_T1_ACK:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_T1_ack,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_T1_ACK));
	break;
    case CSNA_LLC2_T1_PF:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_T1_pf,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_T1_PF));
	break;
    case CSNA_LLC2_T1_REJ:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_T1_rej,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_T1_REJ));
	break;
    case CSNA_LLC2_T1_BUSY:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_T1_busy,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_T1_BUSY));
	break;
    case CSNA_LLC2_TI:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_Ti,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_TI));
	break;
    case CSNA_LLC2_TW:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_Tw,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_TW));
	break;
    case CSNA_LLC2_RW:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_Rw,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_RW));
	break;
    case CSNA_LLC2_N2:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_N2,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_N2));
	break;
    case CSNA_LLC2_N1:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_N1,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_N1));
	break;
    case CSNA_LLC2_T2:
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_T2,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_T2));
	break;
    case CSNA_LLC2_N3: 
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_N3,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_N3));
	break;
    case CSNA_LLC2_NW: 
	changed = csna_llc2_val_change(&pVadapElement->vllc2.llc2_Nw,
				       (csb->sense ? GETOBJ(int,1) :
					CSNA_LLC2_DEF_NW));
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }
    
    if (changed) {
	/* Change will be sent when done configing this vadapter */
	pVadapElement->vadapState |=  VADAP_STATE_UPDATE;
    } else {
	printf("\n%% Specified value matches current value.");
    }
}


/*
 * ----------------------------------------------------------------------------
 * Virtual Lan 
 * ----------------------------------------------------------------------------
 */

static boolean cip_find_vlan_type_id (
   cip_idb_infotype *cip,
   ulong             lan_type, 
   ulong             lan_id,
   vlan_element    **pVlanElement) 
{
    csna_vlans *pVlans;

    pVlans = cip->pVlanData;
    
    *pVlanElement = &pVlans->vlanTypes[lan_type].vlansVlans[lan_id];
    if ((*pVlanElement)->vlanState & VLAN_STATE_INUSE)
	return TRUE;
    return FALSE;
}

static void csna_reset_vadapter (
    vadapter_element *pVadapElement)
{
    pVadapElement->vadapState = VADAP_STATE_UNUSED;
    pVadapElement->vadapVlanLanId = VLAN_MAX_VLANS_OF_TYPE + 1;
    pVadapElement->vadapName[0] = 0;
    pVadapElement->next = NULL;
    pVadapElement->ilan = NULL;
    /* Reset llc2 values to defaults */
    csna_llc2_defaults(pVadapElement);
}

static void csna_reset_vlan (
    vlan_element *pVlanElement)
{
    pVlanElement->vlanState = VLAN_STATE_UNUSED;
    pVlanElement->vlanLanAttributes = 0;
    pVlanElement->vlanBridgeGrp     = 0;
    pVlanElement->vlanLocalRing     = 0;
    pVlanElement->vlanBridgeNum     = 0;
    pVlanElement->vlanTargetRing    = 0;
    pVlanElement->ilan_hwidb        = NULL;
    pVlanElement->adapter_chain     = NULL;
    
    pVlanElement->vlanNumAdapters = 0;
    pVlanElement->vlanVadapterMask = 0;
}

static boolean csna_disallow_if_vadapters (
    vlan_element *pVlanElement)
{
    /* Check for existing vadapters */
    if (pVlanElement->vlanNumAdapters) {
	printf("\n%% %d Adapters exist on Lan %s %d; must delete them first.",
	       pVlanElement->vlanNumAdapters,
	       csna_vlan_types[pVlanElement->vlanLanType],
	       pVlanElement->vlanLanId);
	return TRUE;
    }
    return FALSE;
}

static boolean vadap_matching_macaddr (
   uchar             *macaddr, 
   csna_vlans        *pVlans, 
   uint               lan_type,
   uint               lan_id,
   vadapter_element **pVadapElement)
{
    uint j;

    for (j = 0; j < VLAN_MAX_VADAPTERS; j++) {
	*pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[j];
	if ((*pVadapElement)->vadapState & VADAP_STATE_INUSE) {
	    if ((*pVadapElement)->vadapVlanLanId == lan_id) {
		if (!bcmp(macaddr, (*pVadapElement)->vadapMacaddr, IEEEBYTES))
		    break;
	    }
	}
    }
    
    if (j < VLAN_MAX_VADAPTERS)
	return TRUE;
    return FALSE;
    
}		 


/*
 * ----------------------------------------------------------------------------
 * csna_vadap_name_command
 *      name <name>
 * [no] name 
 *
 * OBJ(string,1) = <name>
 */
void csna_vadap_name_command (
    parseinfo *csb)
{
    vadapter_element *pVadapElement;
    
    pVadapElement = csb->userDataBlock[UDB_VADAP];

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	nv_write(pVadapElement->vadapName[0], "  %s %s", csb->nv_command,
		 pVadapElement->vadapName);
	return;
    }
    
    if (csb->sense) {
	if (strlen(GETOBJ(string,1)) > VADAPTER_NAME_LEN) { 
	    printf("\n%% '%s' exceeds max length of %d characters",
		   GETOBJ(string,1),
		   VADAPTER_NAME_LEN);
	    return;
	}	
	/* Overwrite existing name */
	strcpy(pVadapElement->vadapName, GETOBJ(string,1));
    } else {
	pVadapElement->vadapName[0] = 0;
    }
}


/* ----------------------------------------------------------------------------
 * Vlan bridging
 * ----------------------------------------------------------------------------
 */
static void csna_vlan_print_configured_bridge_stmt (
    vlan_element *pVlanElement)
{
    if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_SRB) {
	printf("\n%%\t%s %d %d %d",
	       CSNA_VLAN_KEY_SRB,
	       pVlanElement->vlanLocalRing,
	       pVlanElement->vlanBridgeNum,
	       pVlanElement->vlanTargetRing);
    }
    
    if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_TB) {
	printf("\n%%\t%s %d",
	       CSNA_VLAN_KEY_TB,
	       pVlanElement->vlanBridgeGrp);
    }		
} 


static boolean csna_validate_bridge_command (
    vlan_element *pVlanElement,
    ushort        desiredAttr)
{
    char  *pBridgeCommand;
    char  *pOtherBridgeCommand;
    ushort uAttrOk, uDisAllow;
    
    if (desiredAttr & VLAN_ATTRB_SRB) {
	pBridgeCommand      = CSNA_VLAN_KEY_SRB;  /* source-bridge */
	pOtherBridgeCommand = CSNA_VLAN_KEY_TB;   /* bridge-group */
	uAttrOk = VLAN_ATTRB_SRB_OK;
	uDisAllow = VLAN_ATTRB_TB;
    } else {
	pBridgeCommand      = CSNA_VLAN_KEY_TB;   /* bridge-group */
	pOtherBridgeCommand = CSNA_VLAN_KEY_SRB;  /* source-bridge */
	uAttrOk = VLAN_ATTRB_TB_OK;
	uDisAllow = VLAN_ATTRB_SRB;
    }
    
    /* Validate this is allowable on current Lan type */
    if (!(pVlanElement->vlanLanAttributes & uAttrOk)) {
	printf("\n%% Cannot configure %s for %s lan type",
	       pBridgeCommand,
	       csna_vlan_types[pVlanElement->vlanLanType]);
	return FALSE;
    }
    
    /* Validate current Lan is not already configured with incompatable type */
    if (pVlanElement->vlanLanAttributes & uDisAllow) {
	printf("\n%% %s is already configured for this lan (%s %d):",
	       pOtherBridgeCommand,
	       csna_vlan_types[pVlanElement->vlanLanType],
	       pVlanElement->vlanLanId);
	csna_vlan_print_configured_bridge_stmt(pVlanElement);
	return FALSE;
    }
    return TRUE;
}


/*
 * ----------------------------------------------------------------------------
 * csna_vlan_srb_command
 *	source-bridge <local-ring> <bridge> <target_ring>
 *   no source-bridge 
 *
 * OBJ(int,1) = <local-ring>
 * OBJ(int,2) = <bridge>
 * OBJ(int,3) = <target-ring>
 */
void csna_vlan_srb_command (parseinfo *csb)
{
    idbtype      *swidb;
    vlan_element *pVlanElement;
    ushort        local_ring;
    ushort        bridge_num;
    ushort        target_ring;
    channel_sb_t  *chan_sb;
    
    swidb = csb->interface;
    pVlanElement = csb->userDataBlock[UDB_VLAN];

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_SRB) {
	    nv_write(TRUE, " %s %d %d %d", csb->nv_command,
		     pVlanElement->vlanLocalRing,
		     pVlanElement->vlanBridgeNum,
		     pVlanElement->vlanTargetRing);
	}
	return;
    }
    
    if (csb->sense) {
	
	if (!csna_validate_bridge_command(pVlanElement, VLAN_ATTRB_SRB))
	    return;

	local_ring = GETOBJ(int,1);
	bridge_num = GETOBJ(int,2);
	target_ring = GETOBJ(int,3);

        if (!pVlanElement->ilan_hwidb) {	
            pVlanElement->ilan_hwidb = cip_vlan_get_idb(swidb->hwptr);
            if (!pVlanElement->ilan_hwidb) {
	        printf("\n%% No more LAN control blocks available on this interface!"); 
	        return;
            }

            chan_sb = channel_get_or_create_sb(pVlanElement->ilan_hwidb);
            if (!chan_sb) {
	        printf("\n%% Could not create channel subblock!"); 
	        return;
            }

            /*
             * If internal adapters are configured on this CIP LAN,
             * then chain them to this internal hwidb
             */
            chan_sb->channel_ilan_access_list = pVlanElement;
	    
 	    if (!cip_setup_srb_info(pVlanElement->ilan_hwidb, 
                     local_ring, bridge_num, target_ring, TRUE)) {
 		/* If we cannot add the source bridge statement, back out the 
 		   new idb and exit. */
		cip_vlan_free_idb(swidb->hwptr, pVlanElement->ilan_hwidb);
		pVlanElement->ilan_hwidb = NULL;
 		pVlanElement->vlanLanAttributes &= ~VLAN_ATTRB_SRB;
 		return;
  	    }

 	} else {

            chan_sb = channel_get_or_create_sb(pVlanElement->ilan_hwidb);
            if (!chan_sb) {
	        printf("\n%% Could not create channel subblock!"); 
	        return;
            }

 	    if(!cip_setup_srb_info(pVlanElement->ilan_hwidb, 
                   local_ring, bridge_num, target_ring, TRUE)) {
 		/* If we cannot add the source bridge statement, exit. */
 		return;
 	    }
	    
 	}
	
	/* Finally, okay to configure this as srb */
	pVlanElement->vlanLanAttributes |= VLAN_ATTRB_SRB;
	
        pVlanElement->vlanLocalRing  = local_ring;
	pVlanElement->vlanBridgeNum  = bridge_num;
	pVlanElement->vlanTargetRing = target_ring;

        /*
         * Add this LAN into the SRB switching tables
         */
        chan_sb->emulated_media = pVlanElement->vlanLanType;

    } else {
	/* Validate that this Lan IS configured for SRB */
	if (!(pVlanElement->vlanLanAttributes & VLAN_ATTRB_SRB)) {
	    printf("\n%% %s is not configured for this lan (%s %d)",
		   CSNA_VLAN_KEY_SRB,
		   csna_vlan_types[pVlanElement->vlanLanType],
		   pVlanElement->vlanLanId);
	    return;
	}
        cip_vlan_remove_srb( pVlanElement, swidb);
    }
}

		 
/*
 * ----------------------------------------------------------------------------
 * csna_vlan_bridge_command
 *      bridge-group <group> 
 * [no] bridge-group
 *
 * OBJ(int,1) = <group>
 */
void csna_vlan_bridge_command (
    parseinfo *csb)
{
    idbtype      *swidb;
    vlan_element *pVlanElement;
    channel_sb_t *chan_sb;
    ushort        bridge_grp;
    
    swidb = csb->interface;
    pVlanElement = csb->userDataBlock[UDB_VLAN];

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_TB) {
	    nv_write(TRUE, " %s %d", csb->nv_command,
		     pVlanElement->vlanBridgeGrp);
	}
	return;
    }
    
    if (csb->sense) {
	if (!csna_validate_bridge_command(pVlanElement, VLAN_ATTRB_TB))
	    return;
	
	bridge_grp = GETOBJ(int,1);
	
        if (!pVlanElement->ilan_hwidb) {	
            pVlanElement->ilan_hwidb = cip_vlan_get_idb(swidb->hwptr);
            if (!pVlanElement->ilan_hwidb) {
	        printf("\n%% No more LAN control blocks available on this interface!"); 
    	        return;
            }

            chan_sb = channel_get_or_create_sb(pVlanElement->ilan_hwidb);
            if (!chan_sb) {
                printf("\n%% Could not create channel subblock!");
                return;
            }

            /*
             * If internal adapters are configured on this CIP LAN,
             * then chain them to this internal hwidb
             */
            chan_sb->channel_ilan_access_list = pVlanElement;

	} else {

            chan_sb = channel_get_or_create_sb(pVlanElement->ilan_hwidb);
            if (!chan_sb) {
	        printf("\n%% Could not create channel subblock!"); 
	        return;
            }
        }

	/* Finally, okay to configure this as tb */
	pVlanElement->vlanLanAttributes |= VLAN_ATTRB_TB;
	
	pVlanElement->vlanBridgeGrp  = bridge_grp;

        chan_sb->emulated_media = pVlanElement->vlanLanType;

        /*
         * Add this LAN into the TB switching tables
         */
        cip_setup_tb_info(pVlanElement->ilan_hwidb, bridge_grp, TRUE);

    } else {
	/* Validate that this Lan IS configured for TB */
	if (!(pVlanElement->vlanLanAttributes & VLAN_ATTRB_TB)) {
	    printf("\n%% %s is not configured for this lan (%s %d)",
		   CSNA_VLAN_KEY_TB,
		   csna_vlan_types[pVlanElement->vlanLanType],
		   pVlanElement->vlanLanId);
	    return;
	}
        cip_vlan_remove_tb( pVlanElement, swidb);
    }
}


/* ----------------------------------------------------------------------------
 * Virtual Adapters
 * ----------------------------------------------------------------------------
 */

/*
 * ----------------------------------------------------------------------------
 * vlan_vadapter_command
 *      adapter <adapno> [<mac-address>]
 * [no] adapter <adapno>
 *
 * OBJ(int,1)   = <adapno>
 * OBJ(hwaddr,1)= <mac-address>
 * OBJ(int,2)   =  presence of <mac-adddress>
 *
 */
void vlan_vadapter_command (
    parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    vadapter_element *pVadapElement;
    vadapter_element *pVadapSameMac;
    uint              adapno;
    uint              lan_type;
    uint              lan_id;
    uchar             macaddr[IEEEBYTES];
    boolean           macaddr_specified;
    leveltype         level;
                
    
    swidb = csb->interface;
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    } 

    /* Process any queued events */
    cip_process_queued_events(hwidb, cip);

    pVlans = cip->pVlanData;
    
    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	pVadapElement = GETOBJ(udb,2);
	nv_write(TRUE, " %s %d %e", csb->nv_command,
		 pVadapElement->vadapAdapno,
		 pVadapElement->vadapMacaddr);
	if (set_mode_byname(&csb->mode, "cip-vadp", MODE_SILENT)) {
	    csb->userDataBlock[UDB_VADAP] = pVadapElement;
	    *csb->nv_command = '\0';  /* clear out before doing subcommands */
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    }

    pVlanElement = csb->userDataBlock[UDB_VLAN];
    if (!pVlanElement) {
	return;
    }
    lan_type = pVlanElement->vlanLanType;
    lan_id   = pVlanElement->vlanLanId;
    
    adapno = GETOBJ(int,1);    
    macaddr_specified = GETOBJ(int,2);
    if (macaddr_specified)
	ieee_copy((char *)GETOBJ(hwaddr,1)->addr, macaddr);
	    
    pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[adapno];
    if (pVadapElement->vadapState & VADAP_STATE_CFGING) {
	printf("\n%% Adapter %d is currently being configured",
	       adapno);
	return;
    }
    if (pVadapElement->vadapState & VADAP_STATE_PNDING) {
	printf("\n%% Configuration of Adapter %d is pending.",
	       adapno);
	return;
    }
    
    if (csb->sense) {
	/* Is it already in use by another Virtual Lan of this type? */
	if ((pVadapElement->vadapState & VADAP_STATE_INUSE) &&
	    (pVadapElement->vadapVlanLanId != pVlanElement->vlanLanId)) {
	    /* The vadapter exists on another LanId ; cannot configure it */
	    printf("\n%% Adapter %d exists on Lan %s %d",
		   adapno, csna_vlan_types[pVlanElement->vlanLanType], 
		   pVadapElement->vadapVlanLanId);
	    printf("\n%%\tcannot configure the same adapno on multiple "
		   "lans of the same type");
	    return;
	}
	/* Is it already configured on this Virtual Lan? */
	if (pVadapElement->vadapVlanLanId == pVlanElement->vlanLanId) {
	    /* reconfiguring an existing vadapter on this vlan */
	    if ((macaddr_specified) &&
		(bcmp(macaddr, pVadapElement->vadapMacaddr, IEEEBYTES))) {
		printf("\n%% Adapter %d already exists with a different "
		       "mac-address (%e)",
		       adapno,
		       pVadapElement->vadapMacaddr);
		return;
	    }
	    /* The VADAP_STATE_UPDATE bit will only be turned on if 
	     * vadapter sub commands cause a value change that must 
	     * be sent to the CIP.
	     */
	} else {
	    if (!macaddr_specified) {
		printf("\n%% A mac-address must be specified when "
		       "configuring a new adapter.");
		return;
	    }
	    /* Assure macaddr is unique across all existing macaddrs
	     * on this specific vlan 
	     */
	    if (vadap_matching_macaddr(macaddr, pVlans, lan_type, lan_id, 
				       &pVadapSameMac)) {
		printf("\n%% mac-address %e is already assigned to "
		       "Adapter %d on this Lan (%s %d)",
		       macaddr,
		       pVadapSameMac->vadapAdapno,
		       csna_vlan_types[pVlanElement->vlanLanType], 
		       pVadapSameMac->vadapVlanLanId);
		return;
	    }
	    
            /* 
             * Chain the vadapter into the CIP LAN bridging 
             * MAC address access list 
             */
            level = raise_interrupt_level(NETS_DISABLE);
            pVadapElement->next = pVlanElement->adapter_chain;
            pVlanElement->adapter_chain = pVadapElement;
            reset_interrupt_level(level);
    
	    /* It's a new one; add it */
            pVadapElement->ilan = pVlanElement;
	    pVlanElement->vlanNumAdapters++;
	    pVadapElement->vadapState |=  VADAP_STATE_INUSE;
	    pVadapElement->vadapState |=  VADAP_STATE_CREATE;

	    /* Mark it as owned by this Virtual Lan */
	    /*	pVlanElement->vlanVadapterMask |= log of adapno ; */
	    pVadapElement->vadapVlanLanId = pVlanElement->vlanLanId;
	    pVadapElement->vadapAdapno = adapno; /* silly */
	    bcopy(macaddr, pVadapElement->vadapMacaddr, IEEEBYTES);
	}
	pVadapElement->vadapState |=  VADAP_STATE_CFGING;
	sprintf(VadapPrompt,"%s%s %d-%d",
		VADAP_PROMPT_BASE,
		csna_vlan_types_short[pVlanElement->vlanLanType], 
		pVlanElement->vlanLanId,
		adapno);
	set_mode_byname(&csb->mode, "cip-vadp", MODE_VERBOSE);
	csb->userDataBlock[UDB_VADAP] = pVadapElement;
    } else {
	/* Check if it exists on this Vlan */
	if (pVadapElement->vadapVlanLanId != pVlanElement->vlanLanId) {
	    /* Cannot remove it; it doesn't exist */
	    printf("\n%% Adapter %d is not configured on this Lan",
		   adapno);
	    return;
	}
	if ((macaddr_specified) &&
	    (bcmp(macaddr, pVadapElement->vadapMacaddr, IEEEBYTES))) {
	    printf("\n%% Specified mac-address (%e) doesn't match "
		   "mac-address assigned to this Adapter (%e)",
		   macaddr,
		   pVadapElement->vadapMacaddr);
	    return;
	}
 
	/* 
         * Tear down; 
         * First fast switching; 
         * then CIP; 
         * reset to be done by cfg ack routine 
         */

        cip_remove_vadapter_from_ilan(pVlanElement, pVadapElement);
 
	/* This needs to be done in the cfg ack routine */
	pVadapElement->vadapState |= VADAP_STATE_CFGING;
	pVadapElement->vadapState |= VADAP_STATE_DELETE;

	/* Send the Tear down request to the CIP */
	pVadapElement->vadapState &= ~VADAP_STATE_CFGING;
	csna_send_vadapter_cfg(csb->interface, pVlanElement, pVadapElement);
    }
}


/*
 * ----------------------------------------------------------------------------
 * Virtual interface subcommands
 * ----------------------------------------------------------------------------
 */
/*
 * ----------------------------------------------------------------------------
 * csna_vlan_max_llc2_sess_command
 *      max-llc2-sessions <max-sessions>
 * [no] max-llc2-sessions
 *
 * OBJ(int,1) = <max-sessions>
 */
void csna_vlan_max_llc2_sess_command (
    parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    ushort            max_sessions;
    
    swidb = csb->interface;
    hwidb = swidb->hwptr;

    if (!cip_cmd_valid_on_interface(hwidb, csb->which, !csb->nvgen))
	return;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    } 

    pVlans = cip->pVlanData;

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	nv_write(pVlans->vlanMaxLLC2sess != CSNA_VLAN_DEF_LLC2_SESSIONS,
		 "%s %d", csb->nv_command,
		 pVlans->vlanMaxLLC2sess);
	return;
    }
    
    if (csb->sense) {
	max_sessions = GETOBJ(int,1);
    } else {
        max_sessions = CSNA_VLAN_DEF_LLC2_SESSIONS;
    }
    if (pVlans->vlanMaxLLC2sess == max_sessions)
	return;

    /* If new value, then update and inform the CIP */
    pVlans->vlanMaxLLC2sess = max_sessions;
    csna_send_max_llc2_sess_command(swidb, pVlans);
}


/*
 * ----------------------------------------------------------------------------
 * csna_vlan_command
 * [no] lan <lan type> <lan id>
 *
 * OBJ(int,1) = <lan type> CSNA_VLAN_ETHER | CSNA_VLAN_TOKEN | CSNA_VLAN_FDDI
 * OBJ(int,2) = <lan id>
 */
void csna_vlan_command (
    parseinfo *csb)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    uint              lan_type;
    uint              lan_id;
    vlan_element     *pVlanElement;

    swidb = csb->interface;

    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip) {
        return;
    } 

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	pVlanElement = GETOBJ(udb,1);
	nv_write(TRUE, "%s %s %d", csb->nv_command,
		 csna_vlan_types[pVlanElement->vlanLanType],
		 pVlanElement->vlanLanId);
	if (set_mode_byname(&csb->mode, "cip-vlan", MODE_SILENT)) {
	    csb->userDataBlock[UDB_VLAN] = pVlanElement;
	    /* clear out before doing subcommands */
	    *csb->nv_command = '\0';
	    push_node(csb, get_mode_nv(csb));
	}
	return;
    }
    
    lan_type = GETOBJ(int,1);
    lan_id   = GETOBJ(int,2);
    
    /* Disallow ethernet and FDDI internal LANs */
    if (lan_type !=CSNA_VLAN_TOKEN) {
       printf("\n%% Only Token-Ring internal LANs are allowed in this release\n");
       return;
    }

    /* Determine if type, id pair already exists */
    pVlanElement = NULL;
    cip_find_vlan_type_id(cip, lan_type, lan_id, &pVlanElement);

    /* Allow lans to be concurrently configured; alert user to the fact */
    if (pVlanElement->vlanState & VLAN_STATE_CFGING) {
	printf("\n%% Lan %s %d is currently being configured",
	       csna_vlan_types_short[pVlanElement->vlanLanType], 
	       pVlanElement->vlanLanId);
    }

    if (csb->sense) {
	if (pVlanElement->vlanState == VLAN_STATE_UNUSED) {
	    pVlanElement->vlanState |=  VLAN_STATE_INUSE;

	    pVlanElement->vlanLanType = lan_type;
	    pVlanElement->vlanLanId = lan_id;

	    /* Determine type of bridging allowed */
	    switch (pVlanElement->vlanLanType) {
	    case CSNA_VLAN_ETHER:
		pVlanElement->vlanLanAttributes = VLAN_ATTRB_TB_OK;
		break;
	    case CSNA_VLAN_TOKEN:
	    case CSNA_VLAN_FDDI:
		pVlanElement->vlanLanAttributes = VLAN_ATTRB_SRB_OK
		                                | VLAN_ATTRB_TB_OK;
		break;
	    }
	}
	pVlanElement->vlanState |=  VLAN_STATE_CFGING;
	sprintf(VlanPrompt,"%s%s %d",
		VLAN_PROMPT_BASE,
		csna_vlan_types_short[pVlanElement->vlanLanType], 
		pVlanElement->vlanLanId);
	set_mode_byname(&csb->mode, "cip-vlan", MODE_VERBOSE);
	csb->userDataBlock[UDB_VLAN] = pVlanElement;
    } else {
	/* Check for no such vlan */
	if (!(pVlanElement->vlanState & VLAN_STATE_INUSE)) {
	    printf("\n%% Lan %s %d does not exist.",
		   csna_vlan_types[lan_type], lan_id);
	    return;
	}
	/* Check for existing vadapters */
	if (csna_disallow_if_vadapters(pVlanElement)) {
	    return;
	}
        /* Remove Bridging */
	if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_SRB)
            cip_vlan_remove_srb( pVlanElement, swidb);
        else if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_TB)
            cip_vlan_remove_tb( pVlanElement, swidb);

	csna_reset_vlan(pVlanElement);
    }
}

/* -------------------------------------------------------
 * Exit from vlan configuration 
 * -------------------------------------------------------
 */
static void vlan_exit (
    parseinfo *csb)
{
    vlan_element *pVlanElement;

    pVlanElement = csb->userDataBlock[UDB_VLAN];
    
    if (pVlanElement) {
	pVlanElement->vlanState &= ~VLAN_STATE_CFGING;
    }
}
/*
 * vlan_exit_command
 */
void vlan_exit_command (
    parseinfo *csb)
{
    if (!csb->sense)
	return;
    
    vlan_exit(csb);
    csb->mode = interface_mode;
    csb->userDataBlock[UDB_VLAN] = NULL;
}
/* ------------------------------------------------------------------
 * Parser routines for maintaining sub command modes
 * ------------------------------------------------------------------
 */
void *vlan_set_vars (
    parseinfo *csb)
{
    vlan_element *pVlanElement;

    pVlanElement = csb->userDataBlock[UDB_VLAN];

    vlan_exit(csb);
    csb->userDataBlock[UDB_VLAN] = NULL;
    return(pVlanElement);
}

void vlan_reset_vars (
    parseinfo *csb, 
    void      *var)
{
    vlan_element *pVlanElement;

    csb->userDataBlock[UDB_VLAN] = var;
    pVlanElement = csb->userDataBlock[UDB_VLAN];
    pVlanElement->vlanState |= VLAN_STATE_CFGING;

}

/*
 * csna_vlan_subcmd
 */
void csna_vlan_subcommand (
    parseinfo  *csb,
    transition *mine,
    void       *null)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    int               i,j;

    swidb = csb->interface;
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    } 

    pVlans = cip->pVlanData;
    /*
     * Setup for the alternate transition
     */
    push_node(csb, mine->alternate);

    /* There is no specific configuration command for a VLAN, use VADAPTER */
    if (!cip_cmd_valid_on_interface(hwidb, CHANNEL_CFG_VADAPTER, !csb->nvgen))
	return;
    
    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	/* Push backwards */
	for (i = CSNA_VLAN_MAXTYPE-1; i >= 0; i--) {
	    for (j = VLAN_MAX_VLANS_OF_TYPE-1; j >= 0; j--) {
		pVlanElement = &pVlans->vlanTypes[i].vlansVlans[j];
		if (pVlanElement->vlanState & VLAN_STATE_INUSE) {
		    SETOBJ(udb,1) = pVlanElement;
		    push_node(csb, mine->accept);
		}
	    }
	}
	SETOBJ(udb,1) = NULL;
	return;
    }
    push_node(csb, mine->accept);
}


/* -------------------------------------------------------
 * Exit from vadapter configuration 
 * -------------------------------------------------------
 */
static void vadapter_exit (
    parseinfo *csb)
{
    vadapter_element *pVadapElement;
    vlan_element     *pVlanElement;

    pVadapElement = csb->userDataBlock[UDB_VADAP];
    pVlanElement = csb->userDataBlock[UDB_VLAN];
    
    if (pVadapElement && pVlanElement) {
	pVadapElement->vadapState &= ~VADAP_STATE_CFGING;
	if (!csb->nvgen) {
	    csna_send_vadapter_cfg(csb->interface, pVlanElement,
				   pVadapElement);
	}
    }    
}
/*
 * vadap_exit_command
 */
void vadap_exit_command (
    parseinfo *csb)
{
    if (!csb->sense)
	return;
    
    vadapter_exit(csb);
    set_mode_byname(&csb->mode, "cip-vlan", MODE_VERBOSE);
    csb->userDataBlock[UDB_VADAP] = NULL;
}
/* ------------------------------------------------------------------
 * Parser routines for maintaining sub command modes
 * ------------------------------------------------------------------
 */

void *vadap_set_vars (
    parseinfo *csb)
{
    vadapter_element *pVadapElement;
    pVadapElement = csb->userDataBlock[UDB_VADAP];

    vadapter_exit(csb);
    csb->userDataBlock[UDB_VADAP] = NULL;
    return(pVadapElement);
}

void vadap_reset_vars (
    parseinfo *csb,
    void      *var)
{
    vadapter_element *pVadapElement;

    csb->userDataBlock[UDB_VADAP] = var;
    pVadapElement = csb->userDataBlock[UDB_VADAP];
    pVadapElement->vadapState |= VADAP_STATE_CFGING;
}


/*
 * csna_vadap_subcmd
 */
void csna_vadap_subcommand (
    parseinfo  *csb,
    transition *mine,
    void       *null)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    vadapter_element *pVadapElement;
    vlan_element     *pVlanElement;
    int               j;
    uint              lan_type;

    swidb = csb->interface;

    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip) {
        return;
    }

    pVlans = cip->pVlanData;
    
    /*
     * Setup for the alternate transition
     */
    push_node(csb, mine->alternate);

    /* Get the current Virtual Lan for which we are processing vadapaters */
    pVlanElement = csb->userDataBlock[UDB_VLAN];

    if (!pVlanElement) {
	return;
    }
    lan_type = pVlanElement->vlanLanType;
    
    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	/* Push backwards */
	for (j = VLAN_MAX_VADAPTERS-1; j >= 0; j--) {
	    pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[j];
	    if (pVadapElement->vadapVlanLanId == pVlanElement->vlanLanId) {
		SETOBJ(udb,2) = pVadapElement;
		push_node(csb, mine->accept);
	    }
	}
	SETOBJ(udb,2) = NULL;
	return;
    }
    push_node(csb, mine->accept);
}


/*
 * ----------------------------------------------------------------------------
 * SHOW EXTENDED COMMANDS
 * ----------------------------------------------------------------------------
 */
static void display_this_vlan (
    void *pThisVlan)
{
    printf("\nLan %s %d",
	   get_vlan_lan_type_name(pThisVlan, VLAN_NAME_FULL),
	   get_vlan_lan_id(pThisVlan));
    
    switch (get_vlan_bridge_type(pThisVlan)) {
    case CSNA_VLAN_BRIDGE_TYPE_SRB:
	printf("\n\t%s %d %d %d",
	       CSNA_VLAN_KEY_SRB,
	       get_vlan_srb_local_ring(pThisVlan),
	       get_vlan_srb_bridge_num(pThisVlan),
	       get_vlan_srb_target_ring(pThisVlan));
	break;
    case CSNA_VLAN_BRIDGE_TYPE_TB:
	printf("\n\t%s %d",
	       CSNA_VLAN_KEY_TB,
	       get_vlan_tb_bridge_group(pThisVlan));
	break;
    default:
	break;
    }
}
#define BLANKS "..."
static void display_this_vadap_status (
   void   *pThisVadap)
{
    vadapter_element *pVadapElement;
    
    pVadapElement = pThisVadap;    
    
    if (channel_debug_ilan) {
	printf("%04x", pVadapElement->vadapState);
	printf(" %s %s %s %s %s %s %s",
	       (pVadapElement->vadapState & VADAP_STATE_ACKED ) ? "ACK" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_PNDING) ? "PND" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_DELETE) ? "DEL" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_UPDATE) ? "UPD" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_CREATE) ? "CRE" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_CFGING) ? "CFG" : BLANKS,
	       (pVadapElement->vadapState & VADAP_STATE_INUSE ) ? "INU" : BLANKS);
    }
}

static void display_this_vadap (
   void   *pThisVadap,
   boolean banner)
{
    
    if (banner) {
	printf("\n\t%6s %14s %8s %5s",
	       "Adapno", "Mac Address", "Name", "Vcnum");
    }
    printf("\n\t%-6d %14e %8s %-5d ",
	   get_vadapt_adapt_no(pThisVadap),
	   get_vadapt_mac_address(pThisVadap),
	   get_vadapt_adapt_name(pThisVadap),
	   get_vadapt_vc(pThisVadap));
    
    display_this_vadap_status(pThisVadap);
}


/*
 * ----------------------------------------------------------------------------
 * show extended channel [slot/port] lan [lan-type [lan-id [adapno]]]
 *
 * OBJ(int,3) = adapno   | CSNA_VLAN_MAX_VADAPTER_NUM+1
 * OBJ(int,2) = lan-id   | CSNA_VLAN_MAX_LANID_NUM+1
 * OBJ(int,1) = lan-type | CSNA_VLAN_MAXTYPE 
 * OBJ(idb,1) = <interface>
 */

void show_ext_channel_vlan_command (
    parseinfo *csb) 
{
    idbtype          *swidb;
    hwidbtype        *hwidb;
    uint              lan_type;
    uint              lan_id;
    uint              adapno;
    boolean           all_types, all_ids, all_adapno;
    boolean           exact;
    void             *pNextVlan;
    void             *pNextVadapter;
    boolean           banner;
    boolean           found = FALSE;
    vlan_element     *pVlanElement;
    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    if (!cip_interface_is_virtual(hwidb)) {
	printf("\n%% Command is not valid on current interface: %s.",
	       swidb->namestring);
	return;
    }

    lan_type = GETOBJ(int,1);
    lan_id = GETOBJ(int,2);
    adapno = GETOBJ(int,3);

    all_types = lan_type == CSNA_VLAN_MAXTYPE;
    all_ids = lan_id == CSNA_VLAN_MAX_LANID_NUM+1;
    all_adapno = adapno == CSNA_VLAN_MAX_VADAPTER_NUM+1;  

    if (all_types)
	lan_type = 0;
    if (all_ids)
	lan_id = 0;
    if (all_adapno)
	adapno = 0;
    
    if (all_types || all_ids) 
	exact = FALSE;
    else
	exact = TRUE;
    
    automore_enable(NULL);
    switch (csb->which) {
    case CHANNEL_CSNA_VLAN:
	/* All Virtual Lans of all types */
	pNextVlan = cip_get_vlan(hwidb, exact, lan_type, lan_id);
	while (pNextVlan) {
	    if (!all_types) 
		if (lan_type != get_vlan_lan_type(pNextVlan))
		    break;
	    lan_type = get_vlan_lan_type(pNextVlan);
	    lan_id = get_vlan_lan_id(pNextVlan);
	    display_this_vlan(pNextVlan);
            found = TRUE;
	    banner = TRUE;
	    pNextVadapter = cip_get_vadapt_from_adapno(hwidb, pNextVlan,
						       (!all_adapno), adapno);
	    while (pNextVadapter) {
		display_this_vadap(pNextVadapter, banner);
		banner = FALSE;
		adapno = get_vadapt_adapt_no(pNextVadapter);
		adapno++;
		pNextVadapter = cip_get_vadapt_from_adapno(hwidb, pNextVlan,
							   (!all_adapno),
							   adapno);
	    }
	    adapno = 0;
	    if (!all_ids)
		break;
	    lan_id++;
	    pNextVlan = cip_get_vlan(hwidb, exact, lan_type, lan_id);
	}
	break;
    case CHANNEL_CSNA_VLAN_INT:
	pVlanElement = (vlan_element *)cip_get_vlan(hwidb, exact, lan_type,
						    lan_id);
        if (pVlanElement && pVlanElement->ilan_hwidb) {
            show_idb(pVlanElement->ilan_hwidb->firstsw);
            found = TRUE;
	}
        break;
    default:
	break;
    }
    
    if (!found)
        printf("\n\n...No internal Lan instances found for this interface...\n");

    automore_disable();
}

/*
 * Increment the mac address
 */
static int incr_mac_addr (uchar *mac) {
    int i;
    
    for (i=IEEEBYTES;i>0;i--) {
        mac[i-1]++;
        if (mac[i-1])
            return 0;
    }
    return 1;
}


static boolean display_llc_connections (hwidbtype *hwidb, void *pNextVlan, 
                                        void *pNextVadapter,
					boolean wc_lsap_flag,
                                        boolean wc_rmac_flag, long vc, 
                                        uint lsap, uchar *rmac, uint rsap,
                                        int *count, uint show_type,
                                        int index_filter) {
    int search = SEARCH_MATCH;
    int search_size;
    int search_type;
    long vc_for_cc = vc;
    uint lsap_for_cc = lsap;
    uchar rmac_for_cc[IEEEBYTES];
    uint rsap_for_cc = rsap;
    llc_cc_stats_t  cc_stats;
    llc_cc_oper_t  cc_oper;
    void *data_ptr;
    char time_string[UNIX_TIME_STRING_MINBUF+1];
    ulong cip_start_time = 0;
    boolean header = TRUE;
    
    
    ieee_copy(rmac, rmac_for_cc);
    if (wc_lsap_flag || wc_rmac_flag || 
        (index_filter == LLC2_SHOW_EXT_CHANNEL_FILTER_RMAC))
        search = SEARCH_NEXT;
         
    switch (show_type) {
      case CSNA_SHO_EXT_TYPE_OPER:
        search_size = LLC_CC_OPER_SIZE;
        search_type = SYSMGT_LLC_CC_OPER;
        data_ptr    = &cc_oper;
        break;
      case CSNA_SHO_EXT_TYPE_STAT:
        search_size = LLC_CC_STATS_SIZE;
        search_type = SYSMGT_LLC_CC_STATS;
        data_ptr    = &cc_stats;
        break;
      default:
        return FALSE;
    }
    
    while ((get_cip_llc_cc(search, hwidb, &vc_for_cc, lsap_for_cc, rmac_for_cc, 
                           rsap_for_cc, data_ptr, search_size, search_type)) != FALSE) {
        if (vc != vc_for_cc)
            break;
        if (show_type == CSNA_SHO_EXT_TYPE_OPER) {
            lsap_for_cc = cc_oper.index.local_sap;
            bcopy( cc_oper.index.rem_mac, rmac_for_cc, IEEEBYTES);
            rsap_for_cc = cc_oper.index.rem_sap;
            if (!wc_lsap_flag) {
                if(lsap_for_cc != lsap)
                    break;
                if ((!wc_rmac_flag) && bcmp(rmac_for_cc, rmac, IEEEBYTES))
                    break;
	    }
            if ((wc_rmac_flag || !bcmp(rmac_for_cc, rmac, IEEEBYTES)) &&
                ((index_filter != LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP) ||
                 (rsap_for_cc == rsap))) {
                if (header) {
                    printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                           get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                           get_vlan_lan_id(pNextVlan),
                           get_vadapt_adapt_no(pNextVadapter),
                           get_vadapt_mac_address(pNextVadapter));
	            header = FALSE;
                }
                printf("\n    Local SAP=%02X Remote MAC=%14e Remote SAP=%02X  State=%s",
                        lsap_for_cc, rmac_for_cc, rsap_for_cc, 
                        get_llc_cc_state( &cc_oper));
                printf("\n      ");
                SHOWLLC2(CSNA_LLC2_KEY_T1_ACK, cc_oper.llcCcOperAckTimer, 0);
                SHOWLLC2(CSNA_LLC2_KEY_T1_PF, cc_oper.llcCcOperPbitTimer, 0);
                SHOWLLC2(CSNA_LLC2_KEY_T1_REJ, cc_oper.llcCcOperRejTimer, 0);
                SHOWLLC2(CSNA_LLC2_KEY_T1_BUSY, cc_oper.llcCcOperBusyTimer, 0);
                printf("\n      ");
                SHOWLLC2(CSNA_LLC2_KEY_TI, cc_oper.llcCcOperInactTimer, 0);
                SHOWLLC2(CSNA_LLC2_KEY_TW, cc_oper.llcCcOperMaxUnackedIPDUsSend, 0);
                SHOWLLC2(CSNA_LLC2_KEY_RW, cc_oper.llcCcOperMaxUnackedIPDUsRcv, 0);
                SHOWLLC2(CSNA_LLC2_KEY_N2, cc_oper.llcCcOperMaxRetransmits, 0);
                printf("\n      ");
                SHOWLLC2("N1-Send", cc_oper.llcCcOperMaxIPDUOctetsSend, 0);
                SHOWLLC2("N1-Rcv", cc_oper.llcCcOperMaxIPDUOctetsRcv, 0);
                SHOWLLC2(CSNA_LLC2_KEY_T2, cc_oper.llcCcOperDelayAckTimer, 0);
                SHOWLLC2(CSNA_LLC2_KEY_N3, cc_oper.llcCcOperDelayAckCount, 0);
                printf("\n      ");
                SHOWLLC2(CSNA_LLC2_KEY_NW, cc_oper.llcCcOperNw, 0);
                SHOWLLC2("Ww", cc_oper.llcCcOperWw, 0);
                printf("\n      Last Ww Cause = %s", 
                       get_last_ww_cause(cc_oper.llcCcOperLastWwCause));
                unix_time_string(time_string, cip_start_time + 
                                              cc_oper.llcCcOperCreateTime);
                printf("\n      Connection Time: %s", time_string);
                unix_time_string(time_string, cip_start_time + 
                                              cc_oper.llcCcOperLastModifyTime);
                printf("\n      Last modified: %s", time_string);
                (*count)++;
	    }
        } else if (show_type == CSNA_SHO_EXT_TYPE_STAT) {
            lsap_for_cc = cc_stats.index.local_sap;
            bcopy( cc_stats.index.rem_mac, rmac_for_cc, IEEEBYTES);
            rsap_for_cc = cc_stats.index.rem_sap;
            if (!wc_lsap_flag) {
                if(lsap_for_cc != lsap)
                    break;
                if ((!wc_rmac_flag) && bcmp(rmac_for_cc, rmac, IEEEBYTES))
                    break;
	    }
            if ((wc_rmac_flag || !bcmp(rmac_for_cc, rmac, IEEEBYTES)) &&
                ((index_filter != LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP) ||
                 (rsap_for_cc == rsap))) {
                if (header) {
                    printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                           get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                           get_vlan_lan_id(pNextVlan),
                           get_vadapt_adapt_no(pNextVadapter),
                           get_vadapt_mac_address(pNextVadapter));
	            header = FALSE;
                }
                printf("\n    Local SAP=%02X Remote MAC=%14e Remote SAP=%02X",
                        lsap_for_cc, rmac_for_cc, rsap_for_cc);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "LocalBusies",  cc_stats.llcCcStatsLocalBusies,
                       "RemoteBusies", cc_stats.llcCcStatsRemoteBusies);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "IFramesIn",  cc_stats.llcCcStatsIFramesIn,
                       "IFramesOut", cc_stats.llcCcStatsIFramesOut);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "IOctetsIn",  cc_stats.llcCcStatsIOctetsIn,
                       "IOctetsOut", cc_stats.llcCcStatsIOctetsOut);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "SFramesIn",  cc_stats.llcCcStatsSFramesIn,
                       "SFramesOut", cc_stats.llcCcStatsSFramesOut);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "REJsIn",  cc_stats.llcCcStatsREJsIn,
                       "REJsOut", cc_stats.llcCcStatsREJsOut);
                printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                       "RetransmitsOut",  cc_stats.llcCcStatsRetransmitsOut,
                       "WwCountChanges", cc_stats.llcCcStatsWwCount);
                (*count)++;
	    }
        }
        if (search == SEARCH_MATCH)
            break;
        rsap_for_cc++;
        if (!rsap_for_cc && incr_mac_addr( rmac_for_cc))
            if (!lsap_for_cc++)
                break;
        if (automore_quit())
            return TRUE;
    }
    return FALSE;
}


static boolean display_llc_sap (hwidbtype *hwidb, void *pNextVlan, 
                                void *pNextVadapter, boolean wc_lsap_flag,
                                long vc, uint lsap, int *count, 
                                uint show_type) {
    int search = SEARCH_MATCH;
    int search_size;
    int search_type;
    long vc_for_sap = vc;
    uint lsap_for_sap = lsap;
    llc_sap_stats_t  sap_stats;
    llc_sap_oper_t  sap_oper;
    void *data_ptr;
    boolean header = TRUE;
    
    if (wc_lsap_flag)
        search = SEARCH_NEXT;

    switch (show_type) {
      case CSNA_SHO_EXT_TYPE_OPER:
        search_size = LLC_SAP_OPER_SIZE;
        search_type = SYSMGT_LLC_SAP_OPER;
        data_ptr    = &sap_oper;
        break;
      case CSNA_SHO_EXT_TYPE_STAT:
        search_size = LLC_SAP_STATS_SIZE;
        search_type = SYSMGT_LLC_SAP_STATS;
        data_ptr    = &sap_stats;
        break;
      default:
        return FALSE;
    }
    
    while ((get_cip_llc_sap(search, hwidb, &vc_for_sap, lsap_for_sap, data_ptr,
                            search_size, search_type)) != FALSE) {
        if (vc != vc_for_sap)
            break;
        if (show_type == CSNA_SHO_EXT_TYPE_OPER) {
            lsap_for_sap = sap_oper.index.local_sap;
            if (!wc_lsap_flag && (lsap != lsap_for_sap))
                break;
            if (header) {
                printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                       get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                       get_vlan_lan_id(pNextVlan),
                       get_vadapt_adapt_no(pNextVadapter),
                       get_vadapt_mac_address(pNextVadapter));
	        header = FALSE;
            }
            printf("\n    Local SAP=%02X", lsap_for_sap);
            printf("\n%6sOpen Connections=%d", "", 
                   sap_oper.llcSapOperNumCcs);
            printf("\n%6sMax Connections Opened=%d", "", 
                   sap_oper.llcSapOperHiWaterNumCcs);
        } else if (show_type == CSNA_SHO_EXT_TYPE_STAT) {
            lsap_for_sap = sap_stats.index.local_sap;
            if (!wc_lsap_flag && (lsap != lsap_for_sap))
                break;
            if (header) {
                printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                       get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                       get_vlan_lan_id(pNextVlan),
                       get_vadapt_adapt_no(pNextVadapter),
                       get_vadapt_mac_address(pNextVadapter));
	        header = FALSE;
            }
            printf("\n    Local SAP=%02X", lsap_for_sap);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "TESTRspsIn",  sap_stats.llcSapStatsTESTResponsesIn,
                   "TESTCmdsOut", sap_stats.llcSapStatsTESTCommandsOut);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "XIDCmdsIn",  sap_stats.llcSapStatsXIDCommandsIn,
                   "XIDCmdsOut", sap_stats.llcSapStatsXIDCommandsOut);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "XIDRspsIn",  sap_stats.llcSapStatsXIDResponsesIn,
                   "XIDRspsOut", sap_stats.llcSapStatsXIDResponsesOut);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "UIFramesIn", sap_stats.llcSapStatsUIFramesIn,
                   "UIFramesOut", sap_stats.llcSapStatsUIFramesOut);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "UIOctetsIn", sap_stats.llcSapStatsUIOctetsIn,
                   "UIOctetsOut", sap_stats.llcSapStatsUIOctetsOut);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "ConnectOk", sap_stats.llcSapStatsConnectOk,
                   "ConnectFail", sap_stats.llcSapStatsConnectFail);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "DiscNorm", sap_stats.llcSapStatsDisconnect,
                   "DiscByTmr", sap_stats.llcSapStatsDisconnectTimer);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "DiscByFRMRSent", sap_stats.llcSapStatsDisconnectFRMRSend,
                   "DiscByFRMRRcvd", sap_stats.llcSapStatsDisconnectFRMRRcv);
            printf("\n%6s%14s = %-10u  %14s = %-10u", "",
                   "DMsInABM", sap_stats.llcSapStatsDMsInABM,
                   "SABMEsInABM", sap_stats.llcSapStatsSABMEsInABM);
        }
        (*count)++;
        if (search == SEARCH_MATCH)
            break;
        if (!lsap_for_sap++)
            break;
        if (automore_quit())
            return TRUE;
    }
    return FALSE;
}


static boolean display_llc_port (hwidbtype *hwidb, void *pNextVlan, 
                                 void *pNextVadapter, long vc,
                                 int *count, uint show_type) {
    llc_port_oper_t   port_oper;
    llc_port_stats_t  port_stats;

    switch (show_type) {
      case CSNA_SHO_EXT_TYPE_DFLT:
      case CSNA_SHO_EXT_TYPE_ADMN:
        (*count)++;
	display_this_vadap_llc2(pNextVlan, pNextVadapter);
        break;
      case CSNA_SHO_EXT_TYPE_OPER:
        if (get_cip_llc_port(SEARCH_MATCH, hwidb, &vc, 
                             &port_oper, LLC_PORT_OPER_SIZE, 
                             SYSMGT_LLC_PORT_OPER)) {
            (*count)++;
            printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                   get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                   get_vlan_lan_id(pNextVlan),
                   get_vadapt_adapt_no(pNextVadapter),
                   get_vadapt_mac_address(pNextVadapter));
            printf("\n%6sOpen SAPs=%d", "", 
                   port_oper.llcPortOperNumSaps);
            printf("\n%6sMax SAPs Opened=%d", "",
                   port_oper.llcPortOperHiWaterNumSaps);
        }
        break;
      case CSNA_SHO_EXT_TYPE_STAT:
        if (get_cip_llc_port(SEARCH_MATCH, hwidb, &vc, 
                             &port_stats, LLC_PORT_STATS_SIZE, 
                             SYSMGT_LLC_PORT_STATS)) {
            (*count)++;
            printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                   get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                   get_vlan_lan_id(pNextVlan),
                   get_vadapt_adapt_no(pNextVadapter),
                   get_vadapt_mac_address(pNextVadapter));
            printf("\n%6s%11s= %-10u    %11s = %-10u", "",
                   "PDUsIn",  port_stats.llcPortStatsPDUsIn,
                   "PDUsOut", port_stats.llcPortStatsPDUsOut);
            printf("\n%6s%11s= %-10u    %11s = %-10u", "",
                   "OctetsIn",  port_stats.llcPortStatsOctetsIn,
                   "OctetsOut", port_stats.llcPortStatsOctetsOut);
            printf("\n%6s%11s= %-10u    %11s = %-10u", "",
                   "TESTCmdsIn",  port_stats.llcPortStatsTESTCommandsIn,
                   "TESTRspsOut", port_stats.llcPortStatsTESTResponsesOut);
            printf("\n%6s%11s= %-10u    %11s = %-10u", "",
                   "LocalBusies", port_stats.llcPortStatsLocalBusies,
                   "UnknownSAPs", port_stats.llcPortStatsUnknownSaps);
        }
        break;
    }  /* switch (show_type) */
    return automore_quit();
}


/*
 * ----------------------------------------------------------------------------
 * show extended channel <s/p> llc2 [admin|oper|stats] [lmac [*|lsap [*|rmac [rsap]]]]
 *
 * OBJ(hwaddr,1) = lmac
 * OBJ(hwaddr,2) = rmac
 * OBJ(int,4) = lsap
 * OBJ(int,5) = rsap
 *
 * OBJ(int,6) = (FALSE) | TRUE  wild card rmac
 * OBJ(int,3) = (FALSE) | TRUE  wild card lsap
 * OBJ(int,2) = index filter
 * OBJ(int,1) = CSNA_SHO_EXT_TYPE_ADMN | 
 *              CSNA_SHO_EXT_TYPE_OPER | 
 *              CSNA_SHO_EXT_TYPE_STAT |
 *              CSNA_SHO_EXT_TYPE_DFLT 
 * OBJ(idb,1) = <interface>
 */  
void show_ext_channel_llc2_command (parseinfo *csb) {
    idbtype           *swidb;
    hwidbtype         *hwidb;    
    uint              show_type;
    uint              index_filter;
    uchar             lmac[IEEEBYTES], rmac[IEEEBYTES];
    uint              lsap, rsap;
    uint              lan_type = 0;
    uint              lan_id = 0;
    uint              adapno = 0;
    void              *pNextVlan;
    void              *pNextVadapter;
    long              vc;
    uint              count = 0;
    boolean           auto_quit_flag = FALSE;
    boolean           wc_lsap_flag, wc_rmac_flag;
    char              *err_str = "Adapters";
    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    if (!cip_interface_is_virtual(hwidb)) {
	printf("\n%% Command is not valid on current interface: %s.",
	       swidb->namestring);
	return;
    }


    show_type    = GETOBJ(int,1);
    index_filter = GETOBJ(int,2);
    wc_lsap_flag = GETOBJ(int,3);
    wc_rmac_flag = GETOBJ(int,6);
    
    lsap = 0;
    rsap = 0;
    memset( rmac, 0, IEEEBYTES);
    memset( lmac, 0, IEEEBYTES);
    
    /*
     * ****WARNING WARNING WARNING****
     * NOTICE: This switch statement is designed to fall through 
     *         at each case level. Understand the logic before
     *         making a change to it.
     */
    switch (index_filter) {
      case LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP:
        rsap = GETOBJ(int,5);
      case LLC2_SHOW_EXT_CHANNEL_FILTER_RMAC:
        if (!wc_rmac_flag)
            ieee_copy((char *)GETOBJ(hwaddr,2)->addr, rmac);
      case LLC2_SHOW_EXT_CHANNEL_FILTER_LSAP:
        if (!wc_lsap_flag)
            lsap = GETOBJ(int,4);
      case LLC2_SHOW_EXT_CHANNEL_FILTER_LMAC:
        ieee_copy((char *)GETOBJ(hwaddr,1)->addr, lmac);
      case LLC2_SHOW_EXT_CHANNEL_FILTER_NONE:
      default:
        break;
    }
    
    /*
     * Check if interface is up before querying for data
     */
    if ((hwidb->state == IDBS_ADMINDOWN) && 
        ((show_type == CSNA_SHO_EXT_TYPE_OPER) || 
         (show_type == CSNA_SHO_EXT_TYPE_STAT))) {
        printf("%s", if_shutdown);
        return;
    }
    
    automore_enable(NULL);
    
    /* 
     * Find the vadapter(s) with specified macaddr
     */
    while ((pNextVlan = cip_get_vlan(hwidb, FALSE, lan_type, lan_id)) != NULL) {
	adapno = 0;
	lan_type = get_vlan_lan_type(pNextVlan);
	lan_id = get_vlan_lan_id(pNextVlan);
	while ((pNextVadapter = cip_get_vadapt_from_adapno(hwidb, pNextVlan, 
                                                           FALSE, adapno)) != NULL) {
            /* 
             * if lmac addr matches, display it 
             */
	    if ((index_filter == LLC2_SHOW_EXT_CHANNEL_FILTER_NONE) ||
	        (!bcmp(lmac, get_vadapt_mac_address(pNextVadapter),
		       IEEEBYTES))) {
                vc = get_vadapt_vc(pNextVadapter);
                switch (index_filter) {
                  case LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP:
                  case LLC2_SHOW_EXT_CHANNEL_FILTER_RMAC:
                    err_str = "Connections";
                    auto_quit_flag = display_llc_connections(hwidb, pNextVlan, 
                                                             pNextVadapter, 
                                                             wc_lsap_flag, 
                                                             wc_rmac_flag,
                                                             vc, lsap, rmac,
							     rsap,
                                                             &count, show_type,
                                                             index_filter);
                    break;
                  case LLC2_SHOW_EXT_CHANNEL_FILTER_LSAP:
                    err_str = "SAPs";
                    auto_quit_flag = display_llc_sap( hwidb, pNextVlan,
						     pNextVadapter,
                                                      wc_lsap_flag, vc, lsap, 
                                                      &count, show_type);
                    break;
                  case LLC2_SHOW_EXT_CHANNEL_FILTER_LMAC:
                  case LLC2_SHOW_EXT_CHANNEL_FILTER_NONE:
                  default:
                    auto_quit_flag = display_llc_port( hwidb, pNextVlan,
						      pNextVadapter, 
                                                       vc, &count, show_type);
                    break;
                }  /* switch (index_filter) */
	    }  /* lmac validation */
            if (auto_quit_flag)
                break;
	    adapno = get_vadapt_adapt_no(pNextVadapter);
	    adapno++;
	}  /* while (pNextVadapter) */
        if (auto_quit_flag)
            break;
        lan_id = get_vlan_lan_id(pNextVlan);
	lan_id++;
    }  /* while (pVlan) */
    
    if (!count)
        printf("\n\n...No matching LLC2 %s found...\n", err_str);
    automore_disable();
}

  
/*
 * ------------------------------------------------------------------------
 * show extended channel [slot/port] connection-map <llc2|...>
 *
 * OBJ(idb,1) = <interface>
 */  
void show_ext_channel_csna_conn_map_command(
    parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;    
    cip_idb_infotype *cip;
    csna_llc2_conn_t csna_llc2_conn;
    llc_sap_oper_t   sap_oper;
    uint             lan_type, lan_id, adapno, lsap;
    void             *pNextVlan, *pNextVadapter;
    long             if_index;
    long             vc, search_vc;
    boolean          found = FALSE;
    boolean          map;
    uint             tot_cip_saps,
                     tot_cip_conns;
    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip || !cip_interface_is_virtual(hwidb)) {
	printf("\n%% Command is not valid on current interface: %s.",
	       swidb->namestring);
	return;
    }

    /*
     * Check if interface is up before querying for data
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
        printf("%s", if_shutdown);
        return;
    }
    
    automore_enable(NULL);
    if (cip->pVlanData) {
        switch (GETOBJ(int,1)) {
        case CSNA_CONN_MAP_LLC2:
            tot_cip_saps = tot_cip_conns = 0;
            if_index = hwidb->snmp_if_index;
	    lan_type = 0;
	    lan_id = 0;
	    pNextVlan = cip_get_vlan(hwidb, FALSE, lan_type, lan_id);
	    while (pNextVlan) {
	        adapno = 0;
	        lan_type = get_vlan_lan_type(pNextVlan);
	        lan_id = get_vlan_lan_id(pNextVlan);
	        pNextVadapter = cip_get_vadapt_from_adapno(hwidb, pNextVlan, 
                                                           FALSE, adapno);
	        while (pNextVadapter) {
                    vc = get_vadapt_vc(pNextVadapter);
                    found = TRUE;
                    printf("\n\n  LAN %5s %-2d Adapter %-3d %14e",
                       	   get_vlan_lan_type_name(pNextVlan, VLAN_NAME_ABBREV),
                       	   get_vlan_lan_id(pNextVlan),
                       	   get_vadapt_adapt_no(pNextVadapter),
                       	   get_vadapt_mac_address(pNextVadapter));
                    lsap = 0;
                    map = FALSE;
                    search_vc = vc;
                    while (get_cip_llc_sap(SEARCH_NEXT, hwidb, &vc, lsap,
                                           &sap_oper, LLC_SAP_OPER_SIZE, 
                                           SYSMGT_LLC_SAP_OPER)) {
                        if (vc != search_vc) {
                            break;
                        }
                        map = TRUE;
                        tot_cip_saps++;
                        tot_cip_conns += sap_oper.llcSapOperNumCcs;
                        printf("\n    Local SAP=%02X LLC2 Connections=%u",
                               sap_oper.index.local_sap,
                               sap_oper.llcSapOperNumCcs);  
                        if (get_llc2_conn(hwidb, sap_oper.csnaHandle, 
                                          &csna_llc2_conn)) {
                            printf("   CSNA Port=%d Path=%04X Device=%02X",
                                   csna_llc2_conn.cipCardCsnaPort,
                                   csna_llc2_conn.cipCardCsnaConnPath,
                                   csna_llc2_conn.cipCardCsnaConnDevice);
                        } else {
                            printf(" - connection map error detected...");
                        }
                        lsap = sap_oper.index.local_sap + 1;
                        if (!lsap) {
                            break;
                        }
		    }
                    if (!map)
                        printf("\n    No SAPs open on this interface");
	            adapno = get_vadapt_adapt_no(pNextVadapter);
	            adapno++;
	            pNextVadapter = cip_get_vadapt_from_adapno(hwidb, 
                                                               pNextVlan, 
                                                               FALSE, adapno);
	        }
	        /* see if macaddr exists on other Virtual Lan types */
	        lan_id++;
	        pNextVlan = cip_get_vlan(hwidb, FALSE, lan_type, lan_id);
	    }
            if (found)
                printf("\n\n  Total : SAPs opened = %5d   Connections active = %5d\n",
                       tot_cip_saps, tot_cip_conns);
            else
                printf("\n\n...No LLC information available in this interface...\n");
            break;
        default:
            break;
        }
    } else {
        printf("\n\n...No internal LANs configured on this interface...\n");
    }
    automore_disable();
}

  
/*
 * ------------------------------------------------------------------------
 * show extended channel [slot/port] max-llc2-sessions
 *
 * OBJ(idb,1) = <interface>
 */  
void show_ext_channel_max_llc2_sess_command (
    parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;    
    cip_idb_infotype *cip;
    boolean           found = FALSE;
    csna_vlans       *pVlans;
    long             slot;
    llc2_sessions_t  llc2_sess;
    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip || !cip_interface_is_virtual(hwidb)) {
	printf("\n%% Command is not valid on current interface: %s.",
	       swidb->namestring);
	return;
    }

    pVlans = cip->pVlanData;

    automore_enable(NULL);
    if (pVlans) {
        found = TRUE;
        printf("\n  Administrative max-llc2-sessions = %d",
                pVlans->vlanMaxLLC2sess);
        slot = hwidb->slot;
        if (get_llc2_sessions_info( SEARCH_MATCH, &slot, &llc2_sess)) {
            printf("\n  Operational max-llc2_sessions = %u",
                    llc2_sess.cipCardOperMaxLlc2Sessions);
            printf("\n  Highest concurrent LLC2 sessions = %u",
                    llc2_sess.cipCardStatsHiWaterLlc2Sessions);
            printf("\n  LLC2 session allocation failures = %u",
                    llc2_sess.cipCardStatsLlc2SessionAllocationErrs);
        } else {
            printf("\n  LLC2 not active on CIP");
        }
    } else {
        printf("\n\n...No LLC2 session information configured...\n");
    }
    automore_disable();
}


/*
 * --------------------------------------------------------------------------
 * SNMP APIs
 * --------------------------------------------------------------------------
 */
ushort cip_get_csna_vlan_maxllc2_sessions (hwidbtype *hwidb) {
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return CSNA_VLAN_DEF_LLC2_SESSIONS;
    }
    
    if (!cip->pVlanData) {
        return CSNA_VLAN_DEF_LLC2_SESSIONS;
    }

    pVlans = cip->pVlanData;
    return pVlans->vlanMaxLLC2sess;
}

void *cip_get_vlan (
   hwidbtype *hwidb,
   boolean    exact_match,
   ulong      lan_type,
   ulong      lan_id )
{
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    int               i,j;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return NULL;
    }
    
    if (!cip->pVlanData)
	return NULL ;
    
    pVlans = cip->pVlanData;
    if (exact_match) {
	if ((lan_id >= VLAN_MAX_VLANS_OF_TYPE) ||
	    (lan_type >= CSNA_VLAN_MAXTYPE))
	    return NULL;
	pVlanElement = &pVlans->vlanTypes[lan_type].vlansVlans[lan_id];
	if (!(pVlanElement->vlanState & VLAN_STATE_INUSE))
	    return NULL;
	return pVlanElement;
    }
    
    /* Check for increment to next Vlan type */
    if (lan_id >= VLAN_MAX_VLANS_OF_TYPE) {
	lan_type++;
	lan_id = 0;
	if (lan_type >= CSNA_VLAN_MAXTYPE) {
	    return NULL;
	}
    }
    /* Look for the next Vlan that is in use */
    for (i = lan_type; i < CSNA_VLAN_MAXTYPE; i++) {
	for (j = lan_id; j < VLAN_MAX_VLANS_OF_TYPE; j++) {
	    pVlanElement = &pVlans->vlanTypes[i].vlansVlans[j];
	    if (pVlanElement->vlanState & VLAN_STATE_INUSE) {
		return pVlanElement;
	    }
	}
	lan_id = 0;
    }
    return NULL;
}    
void *cip_get_vlan_from_vadapt (
    hwidbtype *hwidb,
    void      *vadap)
{
   cip_idb_infotype *cip;
   csna_vlans       *pVlans;
   vlan_element     *pVlanElement;
   vadapter_element *pVadapElement;
   uint              lan_type;

   if (!vadap)
       return NULL;
   
   cip = channel_sb_get_cip_info(hwidb);
   if (!cip) {
       return NULL;
   }
   
   if (!cip->pVlanData)
       return NULL;
   
   pVlans = cip->pVlanData;
   pVadapElement = vadap;

   lan_type = LANTYPE_FROM_VC(get_vadapt_vc(pVadapElement));

   pVlanElement = &pVlans->vlanTypes[lan_type].vlansVlans[pVadapElement->vadapVlanLanId];
   return(pVlanElement);
}
uchar *get_vlan_lan_type_name (
    void *vlan,
    enum_vlan_name name_options)
{
    ushort lan_type;
    
    lan_type = get_vlan_lan_type(vlan);
    if (name_options == VLAN_NAME_ABBREV)
	return csna_vlan_types_short[lan_type];
    return csna_vlan_types[lan_type];
}

lan_types_t get_vlan_lan_type ( 
    void *vlan )
{
    vlan_element     *pVlanElement;
    
    pVlanElement = vlan;
    if (!pVlanElement)
	return CSNA_VLAN_MAXTYPE;
    return (pVlanElement->vlanLanType);
}

ulong get_vlan_lan_id (
    void *vlan )
{
    vlan_element     *pVlanElement;
    
    pVlanElement = vlan;
    if (!pVlanElement)
	return CSNA_VLAN_MAX_LANID_NUM+1;
    return (pVlanElement->vlanLanId);
}

bridge_types_t get_vlan_bridge_type (
    void *vlan )
{
    vlan_element     *pVlanElement;
    
    pVlanElement = vlan;
    if (!pVlanElement)
	return CSNA_VLAN_BRIDGE_TYPE_HUH;
    
    if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_SRB) {
	return CSNA_VLAN_BRIDGE_TYPE_SRB;
    }
    
    if (pVlanElement->vlanLanAttributes & VLAN_ATTRB_TB) {
	return CSNA_VLAN_BRIDGE_TYPE_TB;
    }
    return CSNA_VLAN_BRIDGE_TYPE_HUH;
}

/* Assumes that the VLAN bridge type is appropriate to the call */
/* NO check is made to verify that the Lan has that configuration */
/* use get_vlan_bridge_type() first to determine */

ulong get_vlan_srb_local_ring (
   void *vlan )
{
   vlan_element     *pVlanElement;

   pVlanElement = vlan;
   if (!pVlanElement)
       return CSNA_VLAN_SRB_MAX_RING_NUM+1;
   return (pVlanElement->vlanLocalRing);
}

ulong get_vlan_srb_bridge_num (
   void *vlan )
{
   vlan_element     *pVlanElement;

   pVlanElement = vlan;
   if (!pVlanElement)
       return CSNA_VLAN_SRB_MAX_BRIDGE_NUM+1;
   return (pVlanElement->vlanBridgeNum);
}

ulong get_vlan_srb_target_ring (
   void *vlan )
{
   vlan_element     *pVlanElement;

   pVlanElement = vlan;
   if (!pVlanElement)
       return CSNA_VLAN_SRB_MAX_RING_NUM+1;
   return (pVlanElement->vlanTargetRing);
}

ulong get_vlan_tb_bridge_group (
   void *vlan )
{
   vlan_element     *pVlanElement;

   pVlanElement = vlan;
   if (!pVlanElement)
       return CSNA_VLAN_TB_MAX_BRIDGE_GRP+1;
   return (pVlanElement->vlanBridgeGrp);
}



/* ---------------- Virtual Adapters ---------------- */
void *cip_get_vadapt_from_adapno (
   hwidbtype *hwidb,
   void      *vlan,
   boolean    exact_match,
   ulong      adapno)
{
   cip_idb_infotype *cip;
   csna_vlans       *pVlans;
   vlan_element     *pVlanElement;
   vadapter_element *pVadapElement;
   uint              lan_type;
   uint              lan_id;
   int               k;

   cip = channel_sb_get_cip_info(hwidb);
   if (!cip) {
       return NULL;
   }
   
   if (!cip->pVlanData)
       return NULL;
   
   pVlans = cip->pVlanData;
   
   pVlanElement = vlan;
   lan_type = pVlanElement->vlanLanType;
   lan_id = pVlanElement->vlanLanId;  
   
   if (exact_match) {
       if (adapno >= VLAN_MAX_VADAPTERS)
	   return NULL;
       pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[adapno];
       if (pVadapElement->vadapVlanLanId == lan_id)
	   return pVadapElement;
       return NULL;
   }
   
   /* Look for the next Vadapter that is in use by this Virtual Lan*/
   for (k = adapno; k < VLAN_MAX_VADAPTERS; k++) {
       pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[k];
       if (pVadapElement->vadapVlanLanId == lan_id) {
	   return pVadapElement;
       }
   }
   return NULL;
}
void *cip_get_vadapt_from_vc (
   hwidbtype *hwidb,
   boolean    exact_match,
   ulong      vc)
{
   cip_idb_infotype *cip;
   csna_vlans       *pVlans;
   vadapter_element *pVadapElement;
   uint              lan_type;
   uint              adapno;
   int               i, k;

   cip = channel_sb_get_cip_info(hwidb);
   if (!cip) {
       return NULL;
   }
   
   if (!cip->pVlanData)
       return NULL;
   
   pVlans = cip->pVlanData;
   
   if (vc > MAX_VADAPTER_VC)
       return NULL;
   if (vc < MIN_VADAPTER_VC)
       vc = MIN_VADAPTER_VC;
   
   adapno = VADAPTER_FROM_VC(vc);
   lan_type = LANTYPE_FROM_VC(vc);
   
   if (exact_match) {
       pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[adapno];
       if (pVadapElement->vadapState & VADAP_STATE_INUSE) 
	   return pVadapElement;
       return NULL;
   }
   
   /* Look for the next Vadapter in use; span across Lan types */
   for (i = lan_type; i < CSNA_VLAN_MAXTYPE; i++) {
       for (k = adapno; k < VLAN_MAX_VADAPTERS; k++) {
	   pVadapElement = &pVlans->vlanTypes[i].vlansVadapters[k];
	   if (pVadapElement->vadapState & VADAP_STATE_INUSE) {
	       return pVadapElement;
	   }
       }
       adapno = 0;
   }
   return NULL;
}

ulong get_vadapt_vc (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vadapVC);
}

ulong get_vadapt_adapt_no (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vadapAdapno);
}

uchar *get_vadapt_mac_address (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vadapMacaddr);
}

uchar *get_vadapt_adapt_name (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vadapName);
}


/* ---------------- LLC2 values  ---------------- */
ulong get_vadapt_llc2_T1_ack (    
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_T1_ack);
}

ulong get_vadapt_llc2_T1_pf (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_T1_pf);
}

ulong get_vadapt_llc2_T1_rej (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_T1_rej);
}
ulong get_vadapt_llc2_T1_busy (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_T1_busy);
}

ulong get_vadapt_llc2_Ti (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_Ti);
}

ulong get_vadapt_llc2_Tw (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_Tw);
}

ulong get_vadapt_llc2_Rw (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_Rw);
}

ulong get_vadapt_llc2_N2 (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_N2);
}

ulong get_vadapt_llc2_N1 (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_N1);
}

ulong get_vadapt_llc2_T2 (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_T2);
}

ulong get_vadapt_llc2_N3 (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_N3);
}

ulong get_vadapt_llc2_Nw (
   void *vadap)
{
   vadapter_element     *pVadapElement;

   pVadapElement = vadap;
   return (pVadapElement->vllc2.llc2_Nw);
}


/*
 * ----------------------------------------------------------------------------
 * Interface with if_cip.c
 * ----------------------------------------------------------------------------
 */

/*
 * Initialize vlan structure
 */

void cip_vlan_create (hwidbtype *hwidb, cip_idb_infotype *cip)
{
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    vadapter_element *pVadapElement;
    int               i,j;

    if (!cip_interface_is_virtual(hwidb)) {
	return;
    }
    
    cip->pVlanData = malloc(sizeof(csna_vlans));
    if (!cip->pVlanData) {
	return;
    }
    
    pVlans = cip->pVlanData;
    memset(pVlans, 0, sizeof(csna_vlans));

    pVlans->vlanMaxLLC2sess = CSNA_VLAN_DEF_LLC2_SESSIONS;
    
    for (i = 0; i < CSNA_VLAN_MAXTYPE; i++) {
	for (j = 0; j < VLAN_MAX_VADAPTERS; j++) {
	    /* Reset Vadapter Element */
	    pVadapElement = &pVlans->vlanTypes[i].vlansVadapters[j];
	    csna_reset_vadapter(pVadapElement);
	    pVadapElement->vadapAdapno = j;
	    pVadapElement->vadapVC = ASSIGN_VADAPTER_VC(i,j);
	}
	for (j = 0; j < VLAN_MAX_VLANS_OF_TYPE; j++) {
	    pVlanElement = &pVlans->vlanTypes[i].vlansVlans[j];
	    csna_reset_vlan(pVlanElement);
	    pVlanElement->vlanLanType = i;
	    pVlanElement->vlanLanId = j;
	}
    }
}


/* ---------------------------------------------------------------
 * indx                               CREATE  UPDATE  DELETE  
 *   0 uchar  CHANNEL_CFG_VADAPTER       X 
 *            CHANNEL_CFG_VADAPTER_UPD           X
 *            CHANNEL_CFG_VADAPTER | x80                 X 
 *   1 uchar  align_me     [1]           X       X       X
 *   2 uchar  vadapVC      [2]           X       X       X
 *   4 uchar  vadapAdapno  [2]           X       #       #
 *   6 uchar  vadapMacaddr [6]           X       -       -
 *  12 uchar  LanType      [2]           X       #       #
 *  14 uchar  LanId        [2]           X       #       #
 *  16 uchar  llc2_T1_ack  [4]           X       X       -
 *  20 uchar  llc2_T1_pf   [4]           X       X       -
 *  24 uchar  llc2_T1_rej  [4]           X       X       -
 *  28 uchar  llc2_T1_busy [4]           X       X       -
 *  32 uchar  llc2_Ti      [4]           X       X       -
 *  36 uchar  llc2_Tw      [4]           X       X       -
 *  40 uchar  llc2_Rw      [4]           X       X       -
 *  44 uchar  llc2_N2      [4]           X       X       -
 *  48 uchar  llc2_N1      [4]           X       X       -
 *  52 uchar  llc2_T2      [4]           X       X       -
 *  56 uchar  llc2_N3      [4]           X       X       -
 *  60 uchar  llc2_Nw      [4]           X       X       -
 *  64 uchar  correlator   [2]           X       X       X
 *    -----------------------------------------------------------
 *  66 CFG_CMDBLOCK_SIZE
 *
 *  X required
 *  # sent, but not required, useful in cfg ack
 *  - sent, ignored
 */ 
#define OFFSET_CFG_CMDCODE            0
#define OFFSET_VADAPTER_VCNUM         2 
#define OFFSET_VADAPTER_ADAPNO        4
#define	OFFSET_VADAPTER_MACADDR       6
#define	OFFSET_VADAPTER_LANTYPE      12
#define	OFFSET_VADAPTER_LANID        14
#define	OFFSET_VADAPTER_LLC2_T1_ACK  16
#define	OFFSET_VADAPTER_LLC2_T1_PF   20
#define	OFFSET_VADAPTER_LLC2_T1_REJ  24
#define	OFFSET_VADAPTER_LLC2_T1_BUSY 28
#define	OFFSET_VADAPTER_LLC2_TI      32
#define	OFFSET_VADAPTER_LLC2_TW      36
#define	OFFSET_VADAPTER_LLC2_RW      40
#define	OFFSET_VADAPTER_LLC2_N2      44
#define	OFFSET_VADAPTER_LLC2_N1      48
#define	OFFSET_VADAPTER_LLC2_T2      52
#define	OFFSET_VADAPTER_LLC2_N3      56
#define	OFFSET_VADAPTER_LLC2_NW      60
#define OFFSET_VADAPTER_CORRELATOR   64
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE            66
/* ---------------------------------------------------------
 * pVadapState indicates why this command is being sent:
 * VADAP_STATE_CREATE
 * VADAP_STATE_UPDATE
 * VADAP_STATE_DELETE
 */


static boolean csna_send_vadapter_cfg (
    idbtype          *swidb,
    vlan_element     *pVlanElement,
    vadapter_element *pVadapElement)
{
    hwidbtype  *hwidb;
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    ushort      num_preQ;
    cip_idb_infotype *cip;

    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return(FALSE);
    }

    memset(&buf[0], 0, CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    if (channel_debug) 
	buginf("\n%s: adapter cfg lan %s %d adapter %d vcnum %d, State = %4x",
	       hwidb->hw_namestring,
	       csna_vlan_types[pVlanElement->vlanLanType],
	       pVlanElement->vlanLanId,
	       pVadapElement->vadapAdapno,
	       pVadapElement->vadapVC,
	       pVadapElement->vadapState);
    
    switch (pVadapElement->vadapState &
	    (VADAP_STATE_CREATE | VADAP_STATE_UPDATE | VADAP_STATE_DELETE)) {

    case VADAP_STATE_CREATE | VADAP_STATE_UPDATE:
	pVadapElement->vadapState &= ~VADAP_STATE_UPDATE;
	/* fall through */
    case VADAP_STATE_CREATE:
	buf[OFFSET_CFG_CMDCODE] = CHANNEL_CFG_VADAPTER;
	break;
    case VADAP_STATE_UPDATE:
	buf[OFFSET_CFG_CMDCODE] = CHANNEL_CFG_VADAPTER_UPD;
	break;
    case VADAP_STATE_DELETE:
	buf[OFFSET_CFG_CMDCODE] = CHANNEL_CFG_VADAPTER | CIP_TEARDOWN_VC;
	break;
    default:
	return TRUE;
    }	
    

    PUTSHORT(&buf[OFFSET_VADAPTER_VCNUM       ], pVadapElement->vadapVC);
    PUTSHORT(&buf[OFFSET_VADAPTER_ADAPNO      ], pVadapElement->vadapAdapno);

    bcopy(pVadapElement->vadapMacaddr, &buf[OFFSET_VADAPTER_MACADDR],
	  IEEEBYTES);

    PUTSHORT(&buf[OFFSET_VADAPTER_LANTYPE     ], pVlanElement->vlanLanType);
    PUTSHORT(&buf[OFFSET_VADAPTER_LANID       ], pVlanElement->vlanLanId);

    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_T1_ACK ],
	     pVadapElement->vllc2.llc2_T1_ack);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_T1_PF  ],
	     pVadapElement->vllc2.llc2_T1_pf);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_T1_REJ ],
	     pVadapElement->vllc2.llc2_T1_rej);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_T1_BUSY],
	     pVadapElement->vllc2.llc2_T1_busy);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_TI     ], pVadapElement->vllc2.llc2_Ti);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_TW     ], pVadapElement->vllc2.llc2_Tw);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_RW     ], pVadapElement->vllc2.llc2_Rw);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_N2     ], pVadapElement->vllc2.llc2_N2);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_N1     ], pVadapElement->vllc2.llc2_N1);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_T2     ], pVadapElement->vllc2.llc2_T2);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_N3     ], pVadapElement->vllc2.llc2_N3);
    PUTLONG (&buf[OFFSET_VADAPTER_LLC2_NW     ], pVadapElement->vllc2.llc2_Nw);
    
    /* Add the correlator */
    PUTSHORT(&buf[OFFSET_VADAPTER_CORRELATOR], cip->reset_correlator);

    /* Mark the configuration as PENDING acknowledgment from CIP */    
    pVadapElement->vadapState |= VADAP_STATE_PNDING;

    num_preQ = 1;
    return cip_send_or_ack (hwidb, cip, num_preQ, buf, bytes);
}


/* ---------------------------------------------------------------
 * indx                               CREATE
 *   0 uchar  CHANNEL_CFG_MAX_LLC2       X
 *   1 uchar  align_me     [1]           X      
 *   2 uchar  max_llc2_sess[2]           X      
 *   4 uchar  correlator   [2]           X      
 *    -----------------------------------------------------------
 *   6 CFG_CMDBLOCK_SIZE
 *
 *  X required
 *  # sent, but not required, useful in cfg ack
 *  - sent, ignored
 */ 

#define OFFSET_MAX_LLC2_MAXSESSIONS   2 
#define OFFSET_MAX_LLC2_CORRELATOR    4
#undef  CFG_CMDBLOCK_SIZE
#define CFG_CMDBLOCK_SIZE             6

static boolean csna_send_max_llc2_sess_command (
    idbtype    *swidb,
    csna_vlans *pVlans)
{
    hwidbtype  *hwidb;
    uchar       buf[CFG_CMDBLOCK_SIZE];
    int         bytes;
    ushort      num_preQ;
    cip_idb_infotype *cip;
    
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return(FALSE);
    }

    memset(&buf[0], 0, CFG_CMDBLOCK_SIZE);
    
    bytes = CFG_CMDBLOCK_SIZE;
    
    if (channel_debug) 
	buginf("\n%s: max-llc2-sess cfg, max=%d",
	       hwidb->hw_namestring,
	       pVlans->vlanMaxLLC2sess);
    
    buf[OFFSET_CFG_CMDCODE] = CHANNEL_CFG_MAX_LLC2;
    PUTSHORT(&buf[OFFSET_MAX_LLC2_MAXSESSIONS], pVlans->vlanMaxLLC2sess);
    
    /* Add the correlator */
    PUTSHORT(&buf[OFFSET_MAX_LLC2_CORRELATOR], cip->reset_correlator);

    num_preQ = 1;
    return cip_send_or_ack (hwidb, cip, num_preQ, buf, bytes);
}

/*
 * ---------------------------------------------------------------------
 * cip_vlan_reset_cfg
 *    Reset the current configuration states
 *
 *    This is called at process level.  
 *      - at cip_reset, after issuing a DISABLE to the CIP
 *      - in the event that cip_reset is called at interrupt
 *        level, this is called at process level
 *        (at deferred enable), prior to issuing the ENABLE
 *
 *    o 
 *
 *    o If the configuration is still PNDING, either because it
 *      got purged rather than processed, or because the CIP did
 *      not acknowledge it, then process it here and take it out of
 *      PNDING state
 *
 * ---------------------------------------------------------------------
 */
void cip_vlan_reset_cfg (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip)
{
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    vadapter_element *pVadapElement;
    int               i,j;
    uint              lan_id;    
    
    if (!cip)
	return;
    
    if (!cip->pVlanData)
	return;
    
    if (channel_debug)
	buginf("\n%s: reset_cfg",
	       hwidb->hw_namestring);
    
    pVlans = cip->pVlanData;

    /* Clear any asynchronous state change pending conditions */
    
    /* If a static route is defined on this vc, delete it */
    
    /* If any configurations are in PNDING state, reset them */

    for (i = 0; i < CSNA_VLAN_MAXTYPE; i++) {
	for (j = 0; j < VLAN_MAX_VADAPTERS; j++) {
	    /* Reset Vadapter Element */
	    pVadapElement = &pVlans->vlanTypes[i].vlansVadapters[j];
	    lan_id = pVadapElement->vadapVlanLanId;
	    pVlanElement = &pVlans->vlanTypes[i].vlansVlans[lan_id];
	    
	    if (pVadapElement->vadapState & VADAP_STATE_INUSE) {
		/* Reset any leftover state bits */
		if (pVadapElement->vadapState & VADAP_STATE_DELETE) {
		    pVadapElement->vadapState &= ~VADAP_STATE_DELETE;
		    pVlanElement->vlanNumAdapters--;
		    csna_reset_vadapter(pVadapElement);
		}
		pVadapElement->vadapState &= ~VADAP_STATE_PNDING;
	    }
	}
    }
}


/* ---------------------------------------------------------------
 * Send all vlan configuration commands.  This is called after the
 * interface has been reset.
 * ---------------------------------------------------------------
 */
void cip_vlan_send_vlan_cfg (idbtype *swidb)
{
    cip_idb_infotype *cip;
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    vadapter_element *pVadapElement;
    int               i,j;
    uint              lan_id;    

    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip) {
        return;
    }

    if (!cip->pVlanData)
	return;
    
    pVlans = cip->pVlanData;
    if (pVlans->vlanMaxLLC2sess != CSNA_VLAN_DEF_LLC2_SESSIONS)
	csna_send_max_llc2_sess_command(swidb, pVlans);
    
    for (i = 0; i < CSNA_VLAN_MAXTYPE; i++) {
	for (j = 0; j < VLAN_MAX_VADAPTERS; j++) {
	    /* Reset Vadapter Element */
	    pVadapElement = &pVlans->vlanTypes[i].vlansVadapters[j];
	    if (pVadapElement->vadapState & VADAP_STATE_INUSE) {
		/* Reset any leftover state bits */
		pVadapElement->vadapState &= VADAP_STATE_INUSE;
		pVadapElement->vadapState |= VADAP_STATE_CREATE;
		lan_id = pVadapElement->vadapVlanLanId;
		pVlanElement = &pVlans->vlanTypes[i].vlansVlans[lan_id];
		csna_send_vadapter_cfg(swidb, pVlanElement, pVadapElement);
	    }
	}
    }
}
/* ---------------------------------------------------------------
 */

/*
 * ---------------------------------------------------------------
 * A configuration has been acknowledged by the CIP 
 * --- this is called at process level ---
 *
 * NOTE: Also handle the messages (via errmsg) for all failures:
 *       if not positive ack, and locally called, then the send 
 *       failed.
 *       If not positive ack, and came from CIP , the CIP also
 *       (probably) sent up a logger message explaining why
 *
 * Values of ack_flags
 *        CIP_CFG_ACK_CIP      0x0001   CIP   acknowledgement
 *        CIP_CFG_ACK_LOCAL    0x0002   local acknowledgement
 *
 * Values of code:
 *        CIP_CFG_NACK    -- Negative response to a cfg command
 *        CIP_CFG_ACK     -- Positive response to a cfg command
 *
 * ---------------------------------------------------------------
 */
void cip_vlan_cfg_acknowledge (
    hwidbtype *hwidb,
    ushort     ack_flags,
    ulong      code,
    uchar     *buf,
    ulong      len)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    uchar             cfg_cmd;
    boolean           teardown;
    csna_vlans       *pVlans;
    vlan_element     *pVlanElement;
    vadapter_element *pVadapElement;
    ushort            adapno, lan_type, lan_id;
    boolean           positive_ack;
    ushort            correlator;
   
    swidb = hwidb->firstsw;
 
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    }

    if (!cip->pVlanData) {
	csna_unexpected_cfg_ack(swidb,"No Lan context", buf, len);
	return;
    }
    
    pVlans = cip->pVlanData;
    
    cfg_cmd = buf[OFFSET_CFG_CMDCODE];

    if (cfg_cmd & CIP_TEARDOWN_VC)
	teardown = TRUE;
    else
	teardown = FALSE;
       
    if (code == CIP_CFG_ACK)
	positive_ack = TRUE;
    else
	positive_ack = FALSE;
    
    switch (cfg_cmd) {
    case CHANNEL_NODEID:
	/* currently no state info kept on this beast           */
        /* No action for either positive or negative acknowledgment */
	return;
    case CHANNEL_CFG_MAX_LLC2:
	/* currently no state info kept on this beast           */
        /* currently no backup value to restore if cfg rejected */
	return;
    case CHANNEL_CFG_VADAPTER:
    case CHANNEL_CFG_VADAPTER_UPD:
    case CHANNEL_CFG_VADAPTER | CIP_TEARDOWN_VC:
	/* Get a pVadapElement and pVlanElement based on the fields sent */
        correlator = GETSHORT(&buf[len-sizeof(correlator)]);
        adapno = GETSHORT(&buf[OFFSET_VADAPTER_ADAPNO]);
        lan_type = GETSHORT(&buf[OFFSET_VADAPTER_LANTYPE]);
        lan_id = GETSHORT(&buf[OFFSET_VADAPTER_LANID]);
		
	pVadapElement = &pVlans->vlanTypes[lan_type].vlansVadapters[adapno];
	pVlanElement  = &pVlans->vlanTypes[lan_type].vlansVlans[lan_id];

	if (channel_debug) 
	    buginf("\n%s:(%04X,%04X) Received %s ack for %s%s %s %d %d %e",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   positive_ack ? "POSITIVE" : "NEGATIVE",
		   (teardown) ? "no " : "",
		   cip_cfg_name_str(cfg_cmd),
		   csna_vlan_types[pVlanElement->vlanLanType],
		   pVlanElement->vlanLanId,
		   pVadapElement->vadapAdapno,
		   pVadapElement->vadapMacaddr);
 	
	if (cip->reset_correlator != correlator) {
	    csna_unexpected_cfg_ack(swidb,"Correlator mismatch", 
				   buf, len);
	    return;
	}
	   
	/* Turn off the PNDING status bit */
	if (!(pVadapElement->vadapState & VADAP_STATE_PNDING)) {
	    csna_bad_cfg_ack("PNDING", pVlanElement, pVadapElement,
			     buf, len);
	    return;
	}
	pVadapElement->vadapState &= ~VADAP_STATE_PNDING;

	/* Note: the other state bits should also match the cfg code:
         * CHANNEL_CFG_VADAPTER                   VADAP_STATE_CREATE
         * CHANNEL_CFG_VADAPTER_UPD               VADAP_STATE_UPDATE
         * CHANNEL_CFG_VADAPTER | CIP_TEARDOWN_VC VADAP_STATE_DELETE
         */
	if (teardown) {
	    /* Be really picky */
	    if (!(pVadapElement->vadapState & VADAP_STATE_DELETE)) {
                csna_bad_cfg_ack("DELETE", pVlanElement, pVadapElement,
				 buf, len);
		return;
	    }
	    pVadapElement->vadapState &= ~VADAP_STATE_DELETE;
	    if (positive_ack) {
		pVlanElement->vlanNumAdapters--;
		csna_reset_vadapter(pVadapElement);
	    }
	    return;
	}
        pVadapElement->vadapState |= VADAP_STATE_ACKED;
        /* Otherwise new or update */
	if (cfg_cmd == CHANNEL_CFG_VADAPTER) {
	    /* picky picky */
	    if (!(pVadapElement->vadapState & VADAP_STATE_CREATE)) {
                csna_bad_cfg_ack("CREATE", pVlanElement, pVadapElement,
				 buf, len);
		return;
	    }
	    pVadapElement->vadapState &= ~VADAP_STATE_CREATE;
	    if (!positive_ack) {
		/* Nuke it */
		pVlanElement->vlanNumAdapters--;
                cip_remove_vadapter_from_ilan(pVlanElement, pVadapElement);
		csna_reset_vadapter(pVadapElement);
		/* errmsg  ? */
		return;
	    }
	    return;
	}
	/* Otherwise update */
	/* picky picky */
	if (!(pVadapElement->vadapState & VADAP_STATE_UPDATE)) {
	    csna_bad_cfg_ack("UPDATE", pVlanElement, pVadapElement,
			     buf, len);
	    return;
	}
	pVadapElement->vadapState &= ~VADAP_STATE_UPDATE;
	if (positive_ack) {
	    /* cool */
	    return;
	} else {
	    /* restore saved previous values, yeah, right */
	    /* errmsg  ? */
	    return;
	}
	break;
    default:
	csna_unexpected_cfg_ack(swidb,"Unexpected command code",
				buf, len);
	return;
    }
}


/* ---------------------------------------------------------------
 */
static void csna_bad_cfg_ack (
    char             *pGot,
    vlan_element     *pVlanElement,
    vadapter_element *pVadapElement,
    uchar            *buf,
    ulong             len)
{
    if (channel_debug) {
	buginf("\nGot unexpected acknowledgment: %s, VadapState = %04x",
	       pGot, pVadapElement->vadapState);
    
    if (channel_debug_blocks)
	memory_dump(buf, &buf[len-1],
		    FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
    }
}

static void csna_unexpected_cfg_ack (
    idbtype          *swidb,
    char             *pDescription,
    uchar            *buf,
    ulong             len)
{
    if (channel_debug) {
	buginf("\nGot unexpected acknowledgment: %s, %s",
	       swidb->namestring, pDescription);
	if (channel_debug_blocks)
	    memory_dump(buf, &buf[len-1],
			FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
    }
}


/*
 *
 *  Virtual lan support, innards :-) 
 *
 *
 */

/*
 * Remove this LAN from the SRB switching tables
 */
static void cip_vlan_remove_srb (vlan_element *pVlanElement, idbtype *swidb)
{
    channel_sb_t *chan_sb;

    if (!pVlanElement->ilan_hwidb) {
        return;
    }

    chan_sb = channel_get_sb(pVlanElement->ilan_hwidb);
    if (!chan_sb) {
        return;
    }

    cip_setup_srb_info(pVlanElement->ilan_hwidb, pVlanElement->vlanLocalRing, 
                       pVlanElement->vlanBridgeNum,
		       pVlanElement->vlanTargetRing,
                       FALSE);
    chan_sb->channel_ilan_access_list = NULL;
    free (pVlanElement->ilan_hwidb->firstsw->namestring);
    pVlanElement->ilan_hwidb->firstsw->namestring = NULL;
    cip_vlan_free_idb(swidb->hwptr, pVlanElement->ilan_hwidb);
    pVlanElement->ilan_hwidb = NULL;
    pVlanElement->vlanLanAttributes &= ~VLAN_ATTRB_SRB;
}

/*
 * Remove this LAN from the TB switching tables
 */
static void cip_vlan_remove_tb (vlan_element *pVlanElement, idbtype *swidb)
{
    channel_sb_t *chan_sb;

    if (!pVlanElement->ilan_hwidb) {
        return;
    }

    chan_sb = channel_get_sb(pVlanElement->ilan_hwidb);
    if (!chan_sb) {
        return;
    }

    cip_setup_tb_info(pVlanElement->ilan_hwidb,
		      pVlanElement->vlanBridgeGrp, FALSE);
    chan_sb->channel_ilan_access_list = NULL;
    cip_vlan_free_idb(swidb->hwptr, pVlanElement->ilan_hwidb);
    pVlanElement->ilan_hwidb = NULL;
    pVlanElement->vlanLanAttributes &= ~VLAN_ATTRB_TB;
}

/* 
 * Unchain the vadapter from the CIP LAN bridging 
 * MAC address access list 
 */
static void cip_remove_vadapter_from_ilan (vlan_element *pVlanElement, 
                                           vadapter_element *pVadapElement)
{
    leveltype         level;
    vadapter_element *prev_adapt_p, *vadapt_p;

    if (pVlanElement->adapter_chain) {
        level = raise_interrupt_level(NETS_DISABLE);
        prev_adapt_p = pVlanElement->adapter_chain;
        if (prev_adapt_p == pVadapElement) {
            pVlanElement->adapter_chain = pVadapElement->next;
        } else {
            for (vadapt_p = prev_adapt_p; vadapt_p; vadapt_p = vadapt_p->next) {
                if (vadapt_p == pVadapElement) {
                    prev_adapt_p->next = pVadapElement->next;
                    break;
                    }
                prev_adapt_p = vadapt_p;
            }
        }
        reset_interrupt_level(level);
        pVadapElement->next = NULL;
        pVadapElement->ilan = NULL;
    }
}


/*
 * Redirect all output on ILANs to the virtual interface.
 */
static boolean cip_vlan_holdq_enqueue (hwidbtype *output, paktype *pak,
				       enum HEADTAIL which)
{
    long    address = -1;
    hwidbtype *vidb = output->output_if_hw;

    if (!vidb) {
        if (channel_debug_ilan) {
            buginf(no_vidb, output->hw_namestring,
                   csna_vlan_types[channel_sb_get_emulated_media(output)],
                   " - queue");
        }
        datagram_done(pak);
        return(FALSE);
    }

    /*
     * Ensure packet is encap'd within CIP header before placing on outputQ
     *
     * Don't encap again if packet already has CIP header
     *   verify (pak->enctype != ET_CHANNEL)
     *
     */
    if (pak->enctype != ET_CHANNEL) {

        /*
         * Set pak->linktype for channel_vencap()
         *   channel_vencap() understands only two linktypes: 
         *     - LINK_LLC2 (if packet from CLS or RSRB/LAK)
         *     - LINK_SRB  (if packet from anywhere else  )
         *
         */
        if (pak->linktype != LINK_LLC2) {
            pak->linktype = LINK_SRB;
        } else {
            if (pak->llc2_cb) {
                address = (long) pak->llc2_cb->llc2_dte;
            } else {
                if (channel_debug_ilan) {
                  buginf("\nCIP ILAN(%s-%s): packet dropped - Null LLC",
                      output->hw_namestring,
                      csna_vlan_types[channel_sb_get_emulated_media(output)]);
                }
                datagram_done(pak);
                return(FALSE);
            }
        }  /* End if - set pak->linktype for channel_vencap() */

        if (!channel_vencap(pak, address)) {
            datagram_done(pak);
            return(FALSE);
        }

    } /* End if - Ensure packet encap'd within CIP header */

    pak->if_output = vidb->firstsw;
    return(vidb->oqueue(vidb, pak, which));
}

/*
 * Redirect all output on ILANs to the virtual interface.
 */
static void cip_vlan_soutput (hwidbtype *idb)
{
    hwidbtype *vidb = idb->output_if_hw;

    if (vidb) {
        vidb->soutput(vidb);
    } else {
        if (channel_debug_ilan) {
            buginf(no_vidb, idb->hw_namestring,
                   csna_vlan_types[channel_sb_get_emulated_media(idb)],
                   " - soutput");
        }
    }
}


/*
 * cip_ilan_interface
 */ 
static void cip_ilan_show_interface (hwidbtype *hwidb)
{
    channel_sb_t     *chan_sb;
    vlan_element     *ilan_p;
    vadapter_element *vadap_p;
    boolean found = FALSE;

    chan_sb = channel_get_sb(hwidb);
    if (!chan_sb) {
        return;
    }

    ilan_p = chan_sb->channel_ilan_access_list;
    if (!ilan_p)
        return;

    printf("\n  LAN Emulation : %s %d", 
        csna_vlan_types[chan_sb->emulated_media], ilan_p->vlanLanId);

    vadap_p = ilan_p->adapter_chain;
    while (vadap_p) {
        found = TRUE;
        printf("\n    %d  %14e %s", vadap_p->vadapAdapno, 
                                    vadap_p->vadapMacaddr, 
                                    vadap_p->vadapName);
        vadap_p = vadap_p->next;
    }
    if (!found)
        printf("\n    No adapters configured for this internal LAN");
}

/*
 * clear_ack_for_all_adapters_on_ilan
 * Clear the acked bit from the adapters on a ilan chain
 * so that frames are not bridged to the CIP for
 * those adapters. Called when the CIP virtual interface
 * is shutdown or reset.
 */
static void clear_ack_for_all_adapters_on_ilan (hwidbtype *hwidb) {
    vlan_element     *vlan_p;
    vadapter_element *vadapt_p;
    channel_sb_t     *chan_sb;

    chan_sb = channel_get_sb(hwidb);
    if (!chan_sb) {
        return;
    }
    vlan_p = chan_sb->channel_ilan_access_list;
    if (vlan_p) {
        for (vadapt_p = (vadapter_element *)vlan_p->adapter_chain; 
             vadapt_p; 
             vadapt_p = vadapt_p->next) {
            vadapt_p->vadapState &= ~VADAP_STATE_ACKED;
        }
    }
}

/*
 * cip_ilan_cstate
 * Change state of all CIP ILANs on the CIP virtual interface to the
 * state of the virtual interface. This will keep traffic from flowing
 * out to the CIP on the ILAN hwidbs when the virtual interface is down. 
 * Also, update the 'newproto' variable so state changes are not displayed
 * for the ILAN hwidbs.
 * 
 */
static void cip_ilan_cstate ( hwidbtype *hwidb)
{
    hwidbtype *ilan_idb = hwidb->next_idb_on_physint;

    if (is_cip(hwidb) && is_cip_virtual(hwidb)) {
        while(ilan_idb) {
            if (is_cip_lan(ilan_idb)) {
                ilan_idb->state = hwidb->state;
                ilan_idb->newproto = (ilan_idb->state == IDBS_UP);
                if (!(hwidb->status & IDB_NOHARDWARE)) {
                    ilan_idb->status &= ~IDB_NOHARDWARE;
		}
                if (ilan_idb->state == IDBS_ADMINDOWN) {
                    clear_ack_for_all_adapters_on_ilan(ilan_idb);
                }
	    }
            ilan_idb = ilan_idb->next_idb_on_physint;
        }
    }
}


/*
 * cip_vlan_reset_idb
 * New logic prevents buffers from being put on the 
 * ILAN hwidb holdq. However, this routine will be
 * left in for protection.
 */
void cip_vlan_reset_idb (hwidbtype *hwidb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (hwidb->output_qcount && channel_debug_ilan)
        buginf("\nCIP ILAN(%s-%s) has outputs on the holdq",
            hwidb->hw_namestring, 
            csna_vlan_types[channel_sb_get_emulated_media(hwidb)]);
    holdq_reset(hwidb);
    reset_interrupt_level(level);
    clear_ack_for_all_adapters_on_ilan(hwidb);
    return;
}

/* 
 * cip_getlink()
 *   Invoked from raw_enqueue() via (*idb->getlink)(pak)
 *
 */
static long cip_getlink (paktype *pak)
{
    return(LINK_SRB);
}


/*
 * cip_vlan_free_idb
 * Free a CIP LAN idb.
 * Input hwidb is a CIP virtual interface idb.
 */
static void cip_vlan_free_idb (hwidbtype *hwidb, hwidbtype *ilan_idb)
{
    hwidbtype *prev_hwidb, *idb;
    int	      level;
    
    prev_hwidb = hwidb;
    for (idb = hwidb->next_idb_on_physint; idb; idb = idb->next_idb_on_physint) {
        if (idb == ilan_idb) 
	    break;
        prev_hwidb = idb;
    }
    if (idb) {
        level = raise_interrupt_level(NETS_DISABLE);
        prev_hwidb->next_idb_on_physint = ilan_idb->next_idb_on_physint;
        ilan_idb->next_idb_on_physint = NULL;
        reset_interrupt_level(level);
        ilan_idb->output_if_hw = ilan_idb;
        ilan_idb->state = IDBS_ADMINDOWN;
        ilan_idb->newproto = (ilan_idb->state == IDBS_UP);
        (*ilan_idb->reset)(ilan_idb);
        idb_unlink(ilan_idb);
        p_enqueue(ilan_free_hwidbQ_ptr, ilan_idb);
    }
}

/*
 * cip_vlan_get_idb
 * Get a CIP LAN idb by finding a free one or 
 * creating a new one.
 * Input hwidb is a CIP virtual interface idb.
 */
static hwidbtype *cip_vlan_get_idb (hwidbtype *owner_hwidb)
{
    int	         status;
    cdbtype      *cdb;
    hwidbtype    *nhwidb;
    channel_sb_t *chan_sb;
    srb_vector_table_t *srbV_p;

    nhwidb = p_dequeue(ilan_free_hwidbQ_ptr);
    if (!nhwidb) {
        nhwidb = idb_create();
        if (!nhwidb)
	    return(NULL);
    }

    status = raise_interrupt_level(NETS_DISABLE);

    cdb = controller_find_by_unit(owner_hwidb->mci_index);
    cip_copy_idb_id_info(owner_hwidb, nhwidb);
    hes_idb_init_before(nhwidb);
    cip_copy_idb_id_info(owner_hwidb, nhwidb);

    nhwidb->rxoffset = CHANNEL_TOKEN_ACFC_OFFSET;

    nhwidb->status |= IDB_VIRTUAL_HW|IDB_CHANNEL|IDB_DELETED;
    nhwidb->type    = IDBTYPE_CIP_LAN_ADAPTER;

    nhwidb->show_support = cip_ilan_show_interface;
    nhwidb->enctype = ET_CHANNEL_ILAN;	 
    nhwidb->vencap = channel_vencap;
    nhwidb->oqueue = cip_vlan_holdq_enqueue;
    nhwidb->iqueue = raw_enqueue;
    cip_fs_setup(nhwidb);
    nhwidb->shutdown = (shutdown_t)return_false;
    nhwidb->reset = (reset_t) return_nothing;
    
    reg_add_setup_srb(nhwidb->type, cip_setup_srb_init, "cip_setup_srb_init");

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_enable_spanning_explorer(nhwidb->firstsw);
    }

    nhwidb->srb_routecache = TRUE;

    nhwidb->fs_expl_input = (fs_expl_que *) NULL;
    nhwidb->broadcast = (broadcast_t)return_false;
    nhwidb->linestate = (linestate_t)return_true;

    chan_sb = channel_get_or_create_sb(nhwidb); 
    if (!chan_sb) {
      printf(nomemory);
      return(NULL);
    }
    chan_sb->channel_ilan_access_list = NULL;
    chan_sb->get_vc_from_macaddr = cip_ilan_vc_from_macaddr;

    holdq_init(nhwidb, CHANNEL_HOLDQ);

    nhwidb->set_maxdgram = NULL;
    reset_interrupt_level(status);

    nhwidb->fast_switch_type = FS_CHANNEL;
    nhwidb->getlink = cip_getlink;

    nhwidb->parse_packet = tring_parse_packet;
    nhwidb->extract_snpa = tr_extract_addr;
    nhwidb->extract_hwaddr = ieee_extract_hwaddr;

    nhwidb->tr_transit_oui = DEFAULT_OUI;
    
    idb_final_hw_init(nhwidb);
    idb_final_sw_init(nhwidb->firstsw);

    hes_idb_init_after(nhwidb);

    init_new_hwidb(nhwidb);

    status = raise_interrupt_level(NETS_DISABLE);
    nhwidb->oqueue = cip_vlan_holdq_enqueue;
    nhwidb->soutput = cip_vlan_soutput;
    cip_fs_setup(nhwidb);
    reset_interrupt_level(status);

    cip_copy_idb_info(owner_hwidb, nhwidb);

    if (cdb) {
        load_idb_buffer_info(nhwidb, cdb);
        nhwidb->cbus_burst_count = cdb->cbus_burst_count;
        nhwidb->max_buffers = cdb->max_buffers;
        if (cdb->input_throttled) {
	    idb_mark_throttled(nhwidb);
	} else {
	    idb_mark_unthrottled(nhwidb);
	}
    }

    /*
     * Chain into CIP virtual hwidb
     */
    status = raise_interrupt_level(NETS_DISABLE);
    nhwidb->state = owner_hwidb->state;
    nhwidb->newproto = (nhwidb->state == IDBS_UP);
    nhwidb->output_if_hw = owner_hwidb;
    nhwidb->next_idb_on_physint = owner_hwidb->next_idb_on_physint;
    owner_hwidb->next_idb_on_physint = nhwidb;
    reset_interrupt_level(status);

    idb_enqueue(nhwidb);

    return(nhwidb);
}


/*
 * cip_ilan_vc_from_macaddr
 * Return virtual circuit given output internal LAN hwidb and mac address
 */
static ushort cip_ilan_vc_from_macaddr (hwidbtype *hwidb, uchar *dmac, uchar *smac)
{
    vadapter_element *vadapt_p;
    vlan_element     *ilan_p;

    if (is_cip_lan(hwidb)) {
        channel_sb_t *chan_sb = channel_get_sb(hwidb);
        ilan_p = chan_sb->channel_ilan_access_list;
        if (ilan_p) {
            for (vadapt_p = (vadapter_element *)ilan_p->adapter_chain; 
                vadapt_p; 
                vadapt_p = vadapt_p->next) {
                if (ieee_equal(vadapt_p->vadapMacaddr, dmac)) {
                    if (vadapt_p->vadapState & VADAP_STATE_ACKED) {
                        return(vadapt_p->vadapVC);
                    } else {
                        if (channel_debug_ilan) {
                            buginf("\n%s: ILAN %s %d - CIP internal MAC adapter not"
                                   " acknowledged DMAC(%e) SMAC(%e)",
                                   hwidb->hw_namestring, 
                                   csna_vlan_types[ilan_p->vlanLanType],
                                   ilan_p->vlanLanId, 
                                   dmac, smac);
                        }
                        return (ushort)NO_VC;
		    }
                }
            }
            if (channel_debug_ilan_filt) {
                buginf("\n%s: ILAN %s %d -  MAC filter DMAC(%e) SMAC(%e)",
                           hwidb->hw_namestring, 
                           csna_vlan_types[ilan_p->vlanLanType],
                           ilan_p->vlanLanId, 
                           dmac, smac);
	    }
	}
    }
    return(NO_CHANNEL_VC);
}

/*
 * cip_ilan_hwidb_from_vc
 * Return the CIP internal LAN hwidb for the vc specifed,
 * given the hwidb of the virtual CIP interface and the vc
 */
hwidbtype *cip_ilan_hwidb_from_vc (hwidbtype *hwidb, ushort vc)
{
    uint lan_type, adapno;
    cip_idb_infotype *cip;
    csna_vlans *pVlans;
    vlan_element *ilan;

    if (is_cip(hwidb) && is_cip_virtual(hwidb)) {

        cip = channel_sb_get_cip_info(hwidb);
        if (!cip) {
            return NULL;
        }

        pVlans = cip->pVlanData;
        if (!pVlans || (vc > MAX_VADAPTER_VC) || (vc < MIN_VADAPTER_VC))
            return NULL;
   
        adapno = VADAPTER_FROM_VC(vc);
        lan_type = LANTYPE_FROM_VC(vc);
        ilan = pVlans->vlanTypes[lan_type].vlansVadapters[adapno].ilan;
        if (ilan)
            return(ilan->ilan_hwidb);
    }
    return(NULL);
}


static void cip_reset_llc2_parms (hwidbtype *hwidb)
{
    hwidbtype *ilan_hwidb = hwidb->next_idb_on_physint;
    while (ilan_hwidb) {
        if (is_cip_lan(ilan_hwidb)) {
            llc_sb_copy(hwidb->firstsw, ilan_hwidb->firstsw);
        }
        ilan_hwidb = ilan_hwidb->next_idb_on_physint;
    }
}

static void cip_setup_srb_init (hwidbtype *hwidb) 
{
    srb_vector_table_t *srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_init_rng_br_rng_mask(hwidb->firstsw);
    }
}

static boolean cip_setup_srb_info (hwidbtype *hwidb, ushort rn, ushort bn, 
				   ushort trn, boolean build)
{
    /*
     * If this is the boot loader image, bypass
     * adding cip internal LANs to the SRB subsystem since the
     * SRB subsystem is not included in the boot loader image
     * see [CSCdi59711].
     */
    if (system_loading)
        return FALSE;

    if (srb_debug) {
	buginf("\nSRB(%s): received request to %s SR bridging from rn %d to %d via %d",
	       hwidb->hw_namestring, build ? "set up" : "tear down", rn, trn, bn);
    }	

    if (build) {
        srb_triplet_t srb_triplet;
        srb_vector_table_t *srbV_p;
	reg_invoke_srb_activate_ring(hwidb, trn, rn, bn);
  	/* If the srb activation failed, let calling routine 
	   know so that it can back config
  	   information out of config element list. */
        srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            srbV.srb_sb_get_triplet(hwidb->firstsw, &srb_triplet);
        } else {
            srb_triplet.localRing  = SRB_INVALID_RING_NUM;
            srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
            srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
        }
  	if( !( (srb_triplet.localRing  == rn) &&
  	       (srb_triplet.bridgeNum  == bn) &&
 	       (srb_triplet.remoteRing == trn) )) {
  	    return FALSE;
  	}
       reg_invoke_srt_set_srb_params(hwidb, TRUE);
    } else {
	reg_invoke_srb_deactivate_ring(hwidb, trn, rn, bn);
        reg_invoke_srt_set_srb_params(hwidb, FALSE);
    }
    return TRUE;
}


/* XXX move somewhere appropriate */
static void cip_setup_tb_info (hwidbtype *hwidb, ushort bg, boolean build)
{
    if (srb_debug) {
	buginf("\nTB(%s): received request to %s T bridging in group %d",
	       hwidb->hw_namestring, build ? "set up" : "tear down", bg);
    }

    if (build) {
    } else {
    }
}


static void cip_vlan_add_counters (hwidbtype *didb, hwidbtype *sidb)
{
    int i;
    leveltype level;
    
    level = raise_interrupt_level(NETS_DISABLE);
    didb->counters.tx_datarate += sidb->counters.tx_datarate;
    sidb->counters.tx_datarate = 0;
    didb->counters.tx_packetrate += sidb->counters.tx_packetrate;
    sidb->counters.tx_packetrate = 0;
    didb->counters.rx_datarate += sidb->counters.rx_datarate;
    sidb->counters.rx_datarate = 0;
    didb->counters.rx_packetrate += sidb->counters.rx_packetrate;
    sidb->counters.rx_packetrate = 0;
    didb->counters.tx_intbytes += sidb->counters.tx_intbytes;
    sidb->counters.tx_intbytes = 0;
    didb->counters.rx_intbytes += sidb->counters.rx_intbytes;
    sidb->counters.rx_intbytes = 0;
    didb->counters.tx_intoutputs += sidb->counters.tx_intoutputs;
    sidb->counters.tx_intoutputs = 0;
    didb->counters.rx_intinputs += sidb->counters.rx_intinputs;
    sidb->counters.rx_intinputs = 0;
    reset_interrupt_level(level);
    
    level = raise_interrupt_level(NETS_DISABLE);
    if (TIMER_LATER(sidb->lastinput, didb->lastinput))
	COPY_TIMESTAMP(sidb->lastinput, didb->lastinput);
    if (TIMER_LATER(sidb->lastoutput, didb->lastoutput))
	COPY_TIMESTAMP(sidb->lastoutput, didb->lastoutput);
    reset_interrupt_level(level);
    
    level = raise_interrupt_level(NETS_DISABLE);
    didb->counters.inputs_fast[ACCT_PROTO_SRB] += 
                                     sidb->counters.inputs_fast[ACCT_PROTO_SRB];
    sidb->counters.inputs_fast[ACCT_PROTO_SRB] = 0;
    didb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += 
                                     sidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB];
    sidb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] = 0;
    didb->counters.outputs_fast[ACCT_PROTO_SRB] += 
                                     sidb->counters.outputs_fast[ACCT_PROTO_SRB];
    sidb->counters.outputs_fast[ACCT_PROTO_SRB] = 0;
    didb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += 
                                     sidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB];
    sidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] = 0;
    didb->counters.rx_frames[ACCT_PROTO_SRB] += 
                                     sidb->counters.rx_frames[ACCT_PROTO_SRB];
    sidb->counters.rx_frames[ACCT_PROTO_SRB] = 0;
    didb->counters.rx_bytes[ACCT_PROTO_SRB] += 
                                     sidb->counters.rx_bytes[ACCT_PROTO_SRB];
    sidb->counters.rx_bytes[ACCT_PROTO_SRB] = 0;
    didb->counters.tx_frames[ACCT_PROTO_SRB] += 
                                     sidb->counters.tx_frames[ACCT_PROTO_SRB];
    sidb->counters.tx_frames[ACCT_PROTO_SRB] = 0;
    didb->counters.tx_bytes[ACCT_PROTO_SRB] += 
                                     sidb->counters.tx_bytes[ACCT_PROTO_SRB];
    sidb->counters.tx_bytes[ACCT_PROTO_SRB] = 0;
    reset_interrupt_level(level);

    level = raise_interrupt_level(NETS_DISABLE);
    didb->counters.inputs += sidb->counters.inputs;
    sidb->counters.inputs = 0;
    didb->counters.rx_cumbytes += sidb->counters.rx_cumbytes;
    sidb->counters.rx_cumbytes = 0;
    didb->counters.input_resource += sidb->counters.input_resource;
    sidb->counters.input_resource = 0;
    didb->counters.output_nobuffers += sidb->counters.output_nobuffers;
    sidb->counters.output_nobuffers = 0;
    didb->counters.input_broad += sidb->counters.input_broad;
    sidb->counters.input_broad = 0;
    didb->counters.input_error += sidb->counters.input_error;
    sidb->counters.input_error = 0;
    didb->counters.input_frame += sidb->counters.input_frame;
    sidb->counters.input_frame = 0;
    didb->counters.input_drops += sidb->counters.input_drops;
    sidb->counters.input_drops = 0;
    reset_interrupt_level(level);

    level = raise_interrupt_level(NETS_DISABLE);
    didb->counters.outputs += sidb->counters.outputs;
    sidb->counters.outputs = 0;
    didb->counters.tx_cumbytes += sidb->counters.tx_cumbytes;
    sidb->counters.tx_cumbytes = 0;
    didb->counters.output_resource += sidb->counters.output_resource;
    sidb->counters.output_resource = 0;
    didb->counters.output_error += sidb->counters.output_error;
    sidb->counters.output_error = 0;
    didb->counters.transitions += sidb->counters.transitions;
    sidb->counters.transitions = 0;
    didb->counters.output_total_drops += sidb->counters.output_total_drops;
    sidb->counters.output_total_drops = 0;
    reset_interrupt_level(level);

    level = raise_interrupt_level(NETS_DISABLE);
    didb->pak_swapped_out += sidb->pak_swapped_out;
    sidb->pak_swapped_out = 0;
    reset_interrupt_level(level);

    for (i = 0; i < PRIORITY_MAXTYPE; i++) {
       level = raise_interrupt_level(NETS_DISABLE);
       didb->counters.output_drops[i] += sidb->counters.output_drops[i];
       sidb->counters.output_drops[i] = 0;
       didb->counters.output_dequeued[i] += sidb->counters.output_dequeued[i];
       sidb->counters.output_dequeued[i] = 0;
       reset_interrupt_level(level);
    }
}

/*
 * Consolidate counters from the internal LANs to the
 * CIP virtual interface.
 * NOTICE - Know which counters need to be consolidated.
 *          Both the ILAN hwidbs and the Virtual interface
 *          hwidb support traffic.
 */
static void cip_ilan_consolidate_counters (hwidbtype *hwidb) 
{
    hwidbtype *idb;
        
    if (is_cip_virtual(hwidb)) {
        for (idb = hwidb->next_idb_on_physint; idb; idb = idb->next_idb_on_physint) {
            cip_vlan_add_counters(hwidb, idb);
	}
    }
}


/*
 * cip_vlan init routine
 */
static void cip_vlan_init (subsystype *subsys) {
    queue_init(ilan_free_hwidbQ_ptr, 0);
    reg_add_net_cstate(cip_ilan_cstate, "cip_ilan_cstate");
    reg_add_consolidate_counters(cip_ilan_consolidate_counters,
                                 "cip_ilan_consolidate_counters");
    reg_add_media_reset_llc2_parms(IDBTYPE_CIP, cip_reset_llc2_parms, 
                                   "cip_reset_llc2_parms");
}

/*
 * CIP_VLAN subsystem header
 */

#define CIP_VLAN_MAJVERSION   1
#define CIP_VLAN_MINVERSION   0
#define CIP_VLAN_EDITVERSION  0

/*---------------------------------------------------------------------------
 * Subsystem Header for CIP_VLAN Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(cip_vlan,                         /* name */
	      CIP_VLAN_MAJVERSION,		/* major version */ 
	      CIP_VLAN_MINVERSION,		/* minor version */ 
	      CIP_VLAN_EDITVERSION,		/* edit version */
	      cip_vlan_init, 	 		/* init function */
	      SUBSYS_CLASS_DRIVER,	 	/* subsystem class */
	      "seq: cip",    	                /* sequence list */
	      "req: cip"   	                /* prerequisite list */
	      );
