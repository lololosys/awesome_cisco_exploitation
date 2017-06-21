/* $Id: tring.h,v 3.7.4.8 1996/08/13 02:19:13 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/if/tring.h,v $
 *- -----------------------------------------------------------------
 * tring.h -- 802.5 Token Ring generic definitions.
 * 
 * 23-Jan-1989, Eric B. Decker
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tring.h,v $
 * Revision 3.7.4.8  1996/08/13  02:19:13  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.4.7  1996/08/07  09:00:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.4.6  1996/07/22  01:32:55  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.7.4.5  1996/05/20  06:05:39  gcox
 * Branch: California_branch
 * Remove garbage comments introduced from IbuMod branch
 *
 * Revision 3.7.4.4  1996/05/17  11:23:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.8  1996/05/05  23:10:30  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.6.7  1996/04/26  15:20:44  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.6  1996/04/26  07:55:33  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.6.5  1996/04/03  14:39:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.6.4  1996/03/20  17:54:29  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.6.6.3  1996/03/19  02:04:46  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.6.6.2  1996/03/17  17:51:38  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.6.1  1996/03/16  12:09:37  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.7.4.3  1996/05/09  14:30:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.4.2.4.1  1996/04/27  07:04:13  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.4.2  1996/04/22  18:13:27  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.7.4.1.8.1  1996/04/08  23:03:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Messed up formats from TR-LANE commit.
 *
 * Revision 3.7.4.1  1996/03/18  20:15:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.5  1996/03/16  06:56:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.4  1996/03/14  01:05:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.2.3  1996/03/09  05:06:23  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.2.2  1996/02/20  00:57:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/07  06:34:39  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.6  1996/01/31  18:55:12  rhubbard
 * CSCdi44171:  test-p to virt. mac addr. cause rsrb peer to drop
 *
 * Revision 3.5.2.1  1996/01/24  22:19:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/18  03:22:49  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.4  1995/12/06  18:30:17  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  09:31:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:53:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:03:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/14  22:35:31  kmoberg
 * CSCdi40399:  token_decode_encapsulation_inline() still broken. Get
 * the right rif length.
 *
 * Revision 2.4  1995/09/09  00:53:00  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.3  1995/08/07  05:36:59  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.2  1995/08/01  09:21:45  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.1  1995/06/07  20:55:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TRING_H__
#define __TRING_H__

#include "ieee.h"
#include "tring_common.h"
#include "interface.h"

/*
 * Token Ring (802.5, 16Mb/s or 4Mb/s) mac header consists of the
 * following: all sizes in bytes, Access Control (AC 1), Frame Control
 * (FC 1), Dest Addr (DA 6), Src Addr (SA 6), Routing Information (RI,
 * optional 18 max [IBM] to 30 max [802.5]).  Currently we will only do
 * IBM.  This is controlled by the TRSRB_MAX_MAXRD.
 */
typedef struct tring_hdr_ {
    uchar ac;
    uchar fc;
    uchar daddr[IEEEBYTES];
    uchar saddr[IEEEBYTES];
    uchar data[0];
} tring_hdr;

#define TRING_ENCAPBYTES      14	/* total bytes of MAC encapsulation header*/
#define TRING_8025_ENCAPBYTES 14	/* total bytes of MAC encapsulation header*/
#define TRING_8025_ENCAPWORDS (TRING_8025_ENCAPBYTES / 2)
#define TRING_SAP_ENCAPBYTES  17        /* total bytes of ISO1 encapsulation*/
#define TRING_SNAP_ENCAPBYTES 22        /* total bytes of ISO2 encapsulation*/
#define TRING_SNAP_ENCAPWORDS (TRING_SNAP_ENCAPBYTES / 2)

/*
 * Token-ring multicast address bit -- same as Ethernet, but bit-reversed.
 * Most significant bit of most significant byte.
 */
#define TRING_MAC_MULTICAST     0x80

/*
 * 4M Parameters
 */
#define TR_MAXLLC       4472
#define TR_MAXINFO      (TR_MAXLLC-SNAP_HDRBYTES) /* max proto data (no LLC encap) */
#define TR_MAXDATA      (TR_MAXENCSIZE + TR_MAXINFO + TR_FCS_SIZE)
#define TR_MAXPAK	(TR_MAXENCSIZE + TR_MAXINFO)
#define TR_MTU          TR_MAXINFO

/*
 * 16M Parameters
 */
#define TR16_DEFLLC     8144
#define TR16_DEFINFO    (TR16_DEFLLC - SNAP_HDRBYTES) /* max proto data (no LLC encap) */
#define TR16_DEFDATA    (TR_MAXENCSIZE + TR16_DEFINFO + TR_FCS_SIZE)
#define TR16_MTU        TR16_DEFINFO

#define TR16_MAXLLC     17800
#define TR16_MAXINFO    (TR16_MAXLLC - SNAP_HDRBYTES) /* max proto data (no LLC encap) */
#define TR16_MAXDATA    (TR_MAXENCSIZE + TR16_MAXINFO + TR_FCS_SIZE)
#define TR16_MAXPAK	(TR_MAXENCSIZE + TR16_MAXINFO)

#define TR_ACFC_SIZE 2
#define TR_DA_OFFSET 2
#define TR_SA_OFFSET 8
#define TR_RC_SHORTS 8

#define MAXTRSIZE 17800

#define TR_MINDATA        TR_MINENCSIZE /* ISO2 without Routing Info Field */
#define TR_MINENCSIZE     TRING_ENCAPBYTES
#define TR_MAXENCSIZE     (TRING_ENCAPBYTES+SRB_MAXRIF+SNAP_HDRBYTES)
#define TR_OVERHEAD       (TRING_ENCAPBYTES+SRB_MAXRIF+TR_FCS_SIZE)

struct tr_mac_llc_t_ {
    tring_hdr mac; 
    snap_hdr llc;
};

typedef struct _trrif_t {
    uchar ac;
    uchar fc;
    uchar daddr[6];
    uchar saddr[6];
    uchar rc_blth;
    uchar rc_dlf;
    ushort rd[2];
} trrif_t;

/*
 * Access Control
 */
#define AC_PRI_MASK     0xe0
#define AC_PRI0         0x00
#define AC_PRI1         0x20
#define AC_PRI2         0x40
#define AC_PRI3         0x60
#define AC_PRI4         0x80
#define AC_PRI5         0xa0
#define AC_PRI6         0xc0
#define AC_PRI7         0xe0
#define AC_NOT_TOKEN    0x10  /* NEVER SET THIS BIT!!!!  Both the TI and IBM */
                              /* chipsets die when they see this bit in      */
                              /* conjunction with a non-zero priority field. */
#define AC_MC           0x08
#define AC_RES_MASK     0x07

#define AC_BADBITS      (AC_NOT_TOKEN | AC_MC)

/*
 * Frame Control
 */
#define FC_TYPE_MASK    0xc0
#define FC_TYPE_MAC     0x00
#define FC_TYPE_LLC     0x40
#define FC_TYPE_RSVD1   0x80
#define FC_TYPE_RSVD2   0xc0
#define FC_TYPE_MAC_EXP 0x01

#define PCF_ATTN_MASK   0x0f
#define PCF_EXPRESS     0x01
#define PCF_BEACON      0x02
#define PCF_CLAIMTOKEN  0x03
#define PCF_RINGPURGE   0x04
#define PCF_AMP         0x05
#define PCF_SMP         0x06

/*
 * Misc. Functional Address definitions.  These are defined by the spec.
 * They are 32 bit quantities.  The upper 16 bits (to make the 48 bit
 * address) is 0xc000.
 */

#define TRFUNC_LOCAL      FIXLSWAP(0x0000011b)
#define TRFUNC_MON        FIXLSWAP(0x00000001)
#define TRFUNC_RPS        FIXLSWAP(0x00000002)
#define TRFUNC_REM        FIXLSWAP(0x00000008)
#define TRFUNC_CRS        FIXLSWAP(0x00000010)
#define TRFUNC_NETBIOS    FIXLSWAP(0x00000080)
#define TRFUNC_BRIDGE     FIXLSWAP(0x00000100)
#define TRFUNC_CAU        FIXLSWAP(0x00001000)  /* Controlled Access Unit */
#define TRFUNC_LANMGR     FIXLSWAP(0x00002000)
#define TRFUNC_IPMCAST    FIXLSWAP(0x00040000)  /* Per RFC 1469 */
#define TRFUNC_ESIS_ES    FIXLSWAP(0x00084000)
#define TRFUNC_ESIS_IS    FIXLSWAP(0x00108000)
#define TRFUNC_ISIS_IS    FIXLSWAP(0x00008000)
#define TRFUNC_STANDBY    FIXLSWAP(0x00070000)
#define TRFUNC_STANDBY_0  FIXLSWAP(0x00010000)
#define TRFUNC_STANDBY_1  FIXLSWAP(0x00020000)
#define TRFUNC_STANDBY_2  FIXLSWAP(0x00040000)
#define TRFUNC_NOVELL     FIXLSWAP(0x00800000)
#define TRFUNC_LAT        FIXLSWAP(0x10000000)
#define TRFUNC_ATALK      FIXLSWAP(0x40000000)
#define TRFUNC_C_DN_NODE  FIXLSWAP(0x20000000)
#define TRFUNC_C_DN_ROUTE FIXLSWAP(0x40000000)
#define TRFUNC_D_DN_NODE  FIXLSWAP(0x08000000)
#define TRFUNC_D_DN_ROUTE FIXLSWAP(0x10000000)
#define TRFUNC_CDP        FIXLSWAP(0x08000000)
#define TRFUNC_LRM        FIXLSWAP(0x80000000)
#define TRFUNC_UPPER      FIXSWAP(0xC000)

/*
 * Various byte values for the high order byte of the
 * destination address (Indiv./Group and Univ/Local).  Also
 * the high order bit of src address which is the Routing
 * Information Indicator.
 *
 * TR_GROUP     Indiv./Group    1st byte Daddr
 * TR_LOCAL     Univ./Local     1st byte Daddr
 * TR_FAI       Func.Addr.Indic.3rd byte Daddr
 * TR_RII       RtInfIndic.     1st byte Saddr
 */
#define TR_GROUP        0x80
#define TR_LOCAL        0x40
#define TR_FAI          0x80
#define TR_RII          0x80

#define TR_FCS_SIZE     4
/*
 * Frame Status
 */
#define FS_ARIX         0x80
#define FS_FCIX         0x40
#define FS_ARIY         0x08
#define FS_FCIY         0x04
#define FS_ARIFCI_MASK  0xcc
#define FS_BOTH         0xcc
#define FS_ARI          0x88
#define FS_FCI          0x44
#define FS_NONE         0x00


#define TR_ARP_GRATUITOUS_HOLD  30
/*
 * This is only defined on the main machine.  This file
 * is also used by the Token Ring monitor code also.
 */

#if (CPU_68K || CPU_MIPS || CPU_SPARC)
extern uchar const bit_swaps[];
#define BITSWAP(x) (bit_swaps[(x)])
#endif 

/*
 * Structure used to hold ring status information.
 */

typedef struct tr_rstattab_t_ {
    ushort bit;
    char *msg;
} tr_rstattab_t;

typedef struct tring_mac_frame_ {
    uchar ac;
    uchar fc;
    uchar daddr[IEEEBYTES];
    uchar saddr[IEEEBYTES];
    ushort length;
    uchar  class;
    uchar  command;
} tring_mac_frame;


enum {
    TOKEN_HW_STATUS_TIME=IDB_TYPE_BASE_TOKEN+1,
    TOKEN_HW_LINEUP,
    TOKEN_HW_LINEUPTIMER,
    TOKEN_HW_GROUP,
    TOKEN_HW_FUNCTIONAL,
    TOKEN_HW_RING_STATUS,
    TOKEN_HW_MBUS_TIMER,
    TOKEN_HW_TEST_TIMER,
    TOKEN_HW_LAST_PANIC,
    TOKEN_HW_LAST_PANIC_TIME,
    TOKEN_HW_FAIL_COUNT,
    TOKEN_HW_LAST_FAIL,
    TOKEN_HW_LAST_FAIL_CODE,
    TOKEN_HW_LAST_FAIL_TIME,
    TOKEN_HW_NEXT_BUGINF,
    TOKEN_HW_POLLING_COUNT,
    TOKEN_HW_POLLING_FAILURE,
    TOKEN_HW_POLLING_STATE,

    TOKEN_HW_SIGNAL_LOSS,
    TOKEN_HW_HARD_ERRS,
    TOKEN_HW_SOFT_ERRS,
    TOKEN_HW_TX_BEACON,
    TOKEN_HW_WIRE_FAULT,
    TOKEN_HW_RMT_REMOVAL,
    TOKEN_HW_SINGLE_STATION,
    TOKEN_HW_RING_RECOVERY,

    TOKEN_HW_PREV_SIGNAL_LOSS,
    TOKEN_HW_PREV_HARD_ERRS,
    TOKEN_HW_PREV_SOFT_ERRS,
    TOKEN_HW_PREV_TX_BEACON,
    TOKEN_HW_PREV_WIRE_FAULT,
    TOKEN_HW_PREV_RMT_REMOVAL,
    TOKEN_HW_PREV_SINGLE_STATION,
    TOKEN_HW_PREV_RING_RECOVERY,
    TOKEN_HW_PREV_BAD_ACBITS,

    TOKEN_HW_PREV_ERR_LINE,
    TOKEN_HW_PREV_ERR_INTERNAL,
    TOKEN_HW_PREV_ERR_BURST,
    TOKEN_HW_PREV_ERR_ARIFCI,
    TOKEN_HW_PREV_ERR_ABORT,
    TOKEN_HW_PREV_ERR_LOSTFRAME,
    TOKEN_HW_PREV_ERR_RCVCONGESTION,
    TOKEN_HW_PREV_ERR_COPY,
    TOKEN_HW_PREV_ERR_FREQ,
    TOKEN_HW_PREV_ERR_TOKEN,
    TOKEN_HW_NAUN,
    TOKEN_HW_BEACON_NAUN,
    TOKEN_HW_LAST_BEACON_TYPE,
    TOKEN_HW_RINGOPENSTATUS
};
    
        

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
#define TR_OPN_PHASES 6
#define TR_OPN_ERRORS 11
#define TR_RSTAT_BUF_SIZE 256
/************************************************
 *    Cls_Is_Idb_Virtual(idb)
 *    ----------------------
 *   This macro is simple test for unique field
 *   that determines the vidb link type . In
 *   all cases link for CLS have vidb_link to -1 
 *   see dlc_new_vidb() .
 ************************************************/

#define Cls_Is_Idb_Virtual(idb) ((idb)->vidb_link == (struct hwidbtype_ *) RSRB_INVALID_ADDRESS)

extern char trprint_rifstr[];
extern char *const tr_panic_types[];
extern uchar trbaddr2[];
extern uchar tr_amp[];
extern uchar tr_crs[];

#include "../h/packet.h"
#include "../if/rif.h"

int getRifLfTrh(trrif_t *rif);
boolean setRifLfTrh(trrif_t *trh, int lfValue);
boolean using_multiring(idbtype *, int);
fuzzy tr_bcast(hwidbtype *, paktype *);
void set_new_ringmode(hwidbtype *, int);
boolean tr_print_packet(hwidbtype *, char *, tring_hdr *, ulong *, ushort,
                        int);
void tr_process_xid(hwidbtype *, paktype *, boolean, boolean);
void tr_process_test(hwidbtype *idb, paktype *pak, boolean usesa);
void tr_functional_init(hwidbtype *);
boolean tr_extract_addr(paktype *pak, hwaddrtype *address, enum SRCDST which);
/* move tring_parse_packet to interface_types.h where iqueue_t is defined 
 * gcox 96/3/28
iqueue_t tring_parse_packet(hwidbtype *iadb, paktype *pak, uchar *data);
*/
void tring_idb_enqueue(hwidbtype *hwidb);
void tring_idb_dequeue(hwidbtype *hwidb);
boolean tr_vencap(paktype *, long);
long tring_getlink(paktype *);
boolean multicast_explorer_can_be_bridged(idbtype *, ushort, sap_hdr*);
extern boolean rif_loop_check(hwidbtype *, trrif_t *, riftype *);

extern void fs_srb_setup(void);

#endif /* __TRING_H__ */

