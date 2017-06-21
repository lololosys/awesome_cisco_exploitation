/* $Id: srb.h,v 3.8.4.4 1996/08/03 23:38:28 fox Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb.h,v $
 *------------------------------------------------------------------
 * srb.h -- Common definitions for SRB/RSRB code.
 *
 * March 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srb.h,v $
 * Revision 3.8.4.4  1996/08/03  23:38:28  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.8.4.3  1996/07/18  20:29:49  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.8.4.2  1996/03/28  16:47:43  psandova
 * CSCdi47456:  All srb explorers being flushed on one tok interface
 * Branch: California_branch
 *
 * Make the flush timer interface specific.  Also, count flush
 * data rate after packet has passed all filtering checks.
 *
 * Revision 3.8.4.1  1996/03/18  22:11:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/16  07:38:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.2  1996/03/13  02:02:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.1  1996/02/20  18:49:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/03/11  14:05:51  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.7  1996/03/07  23:32:49  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.6  1996/02/29  00:19:46  kchiu
 * CSCdi46631:  lanmgr reporting erroneous Non-Broadcast frame counters
 *
 * Revision 3.5  1995/12/18  15:03:10  motto
 * When multiple i/f's are in same ring-group and do not share
 * the same bridge number, then explorers can get forwarded
 * with incorrect RIFs.
 * CSCdi45373:  Explorers not forwarded to target ring
 *
 * Revision 3.4  1995/12/11  10:20:39  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.3  1995/11/17  18:57:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:30:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/26  00:30:09  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert tbridge and srb to use chunk-based SMF.
 *
 * Revision 2.6  1995/08/08  16:50:05  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/08/01 22:07:06  motto
 * Changed fast explorer vector routine to keep track of drops instead
 * of the caller.
 * Added support for CIP internal LAN fast explorers.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.4  1995/08/01  09:23:09  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.3  1995/06/21  02:06:53  alam
 * CSCdi36191:  IBM spanning tree doesnt form with SRB/FDDI
 * Insert IEEE spanning tree multicast address into SMF when AST over
 * SRB/FDDI is enabled.
 *
 * Revision 2.2  1995/06/15  23:00:56  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.1  1995/06/07  23:02:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__SRB_H__
#define	__SRB_H__

/*
 * Source Route Bridging external variables
 */
extern boolean global_proxy_explorer_enable;
extern boolean srb_running;
extern watched_queue *srb_explorerQ;
extern boolean	  srb_debug;
extern boolean	  srbevent_debug;
extern boolean	  srberr_debug;

/*
 * Source Route Bridging prototypes
 */
extern hwidbtype *find_ringidb(ushort);
extern void set_srb_interfaces(void);
extern void srb_enq(watched_queue *, paktype *pak);
extern void srb_init(subsystype *subsys);
extern void srb_list(int, hwidbtype *, int *);
extern boolean srb_multicast_enq(idbtype *, paktype *, boolean);
extern void start_srb_background(void);
extern void srb_common(paktype *, hwidbtype *, int);
extern boolean srb_in_check(uchar *, ulong, hwidbtype *, int);
extern boolean srb_out_check(uchar *, ulong, hwidbtype *, int);
extern void set_vring_fast_entry(hwidbtype *);
extern void set_vring_fast_peer(tr_vpeertype *, hwidbtype *);
extern void srb_media_registry(void);
extern void do_bridge_number_setup(hwidbtype *, hwidbtype *);
extern void rsrb_filter_command(parseinfo *csb);
extern void srb_media_conv_local_ack(boolean, boolean *, int);
extern void srb_span_command(parseinfo *, hwidbtype *);
extern boolean lnm_srb_span_command(hwidbtype *, hwidbtype *, boolean);
extern int get_srb_span_index(int);
extern boolean srb_flush_explorer(hwidbtype *, ushort);
extern boolean explorer_enq(paktype *);
extern boolean fs_explorer_check(boolean, hwidbtype *, hwidbtype *);
extern void fs_explorer_setup(void);
extern boolean srb_fast_explorers(paktype *, hwidbtype *, int);
extern void activate_ring(hwidbtype *idb, ushort target_ring, ushort local_ring, ushort bridge_num);
extern void deactivate_ring(hwidbtype *idb, ushort target_ring, ushort local_ring, ushort bridge_num);
extern boolean sbridge_smf_update(uchar *mac_addr, idbtype *swidb, ulong result);
extern int get_max_srb_ports(void);
/*
 * srb_chain.c
 */
extern void srb_parser_init(void);

/*
 * bridge_setup.c
 */
extern void bridge_setup_global(setup_struct *);
extern void bridge_setup_int(setup_struct *);
extern void bridge_setup_end(setup_struct *);

/*
 * CLS over Local-ack
 * Doesn't really belong here, but local-ack isn't yet a 
 * subsystem.  
 */
extern void cls_CreateLak (llctype *llc, paktype *pak);
extern void cls_remove_vring_interface (idbtype *idb);

/*
 * global explorer fast-switching control block.
 */

#define FS_FLAG_CXBUS  0x0004  /* Hardware includes CXBUS */
#define EXPL_RIF_UPDATED    1  /* Common part of rif modified.          */
#define LES_PAK_MODIFIED    2  /* Lo-end specific part of pak modified. */
#define CXBUS_PAK_MODIFIED  4  /* Hi-end specific part of pak modified. */

struct fs_explorer_cb_ {
    ulong total;
    ulong flush;
    ulong data_maxrate;
    ulong data_maxrate_dflt;
    ulong flags;
    paktype *newpak;
    fs_hdrtype *hcb;
    boolean enabled;
    boolean prune;
    boolean ARE_dup_filter;
};

/*
 * packet specific explorer fast-switch control block.
 */
struct fs_hdrtype_ {
   boolean valid;
   struct _trrif_t *paktrh;
   int     pakriflen;
   uchar   *pakdataptr;
   int     pakdatasize;
   int     dgsize;
   struct _trrif_t *newtrh;
   int     newriflen;
   uchar   *newdataptr;
   hwidbtype *iidb;             /* input interface pointer */
   struct _srbrd_t *vrg_rd;     /* route desc. ptr for virtual ring */
};

/*
 * idb specific explorer fast-switch control block.
 */
struct fs_expl_que_ {
    fs_expl_que *next;
    hwidbtype *idb;
    ulong count;
    ulong storms;
    ulong out_drops;
    sys_timestamp data_timer;
    ulong data_rate;
    sys_timestamp queue_timer;
    ulong queue_depth;
    boolean prune;
    boolean congested;
};

/*
 * defines for pak->fcs_type
 * used to identify the type of check sum appended to the pak data
 *  if any.
 *  (NOTE check sum size not included in pak->datagramsize).
 * this stuff might be of use to tbridging.
 */
#define FCS_NONE    0        /* no fcs appended */
#define FCS_TRING   1        /* token ring fcs appended */
#define FCS_FDDI    2        /* fddi fcs appended */

#define FCS_SIZE    4

#define SRB_POSSIBLE_ON_IDB(idb) ((idb->status & IDB_TR) || 	\
				  (idb->type & IDBTYPE_FDDIT))

/*
 * SRB route cache flags used in idb->srb_routecache. Really bit positions.
 * Taken out of h/interface_private.h and put here in 11.2.
 */
# define SRB_FAST_ROUTECACHE		0x00000001
# define SRB_CBUS_ROUTECACHE		0x00000002
# define SRB_SSE_ROUTECACHE		0x00000004

#endif	__SRB_H__
