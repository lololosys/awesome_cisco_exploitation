/* $Id: mlp.h,v 3.3.18.10 1996/09/10 07:32:46 dblair Exp $
 * $source$
 *------------------------------------------------------------------
 * mlp.h - Multilink PPP Protocol Defs.
 *
 * 28-June- 1995, Dana Blair
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlp.h,v $
 * Revision 3.3.18.10  1996/09/10  07:32:46  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.3.18.9  1996/09/10  07:17:48  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.3.18.8  1996/08/21  23:12:50  dblair
 * CSCdi66129:  bus error at PC _mlp_add_link, address 0x4AFC4B04
 * Branch: California_branch
 * Don't allow reset while changing the multilink bundle interface
 * bandwidth
 *
 * Revision 3.3.18.7  1996/08/14  14:19:09  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * The multilink half-fastswitching performance improvement
 *
 * Revision 3.3.18.6  1996/08/03  23:41:54  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.18.5  1996/07/08  15:57:57  syiu
 * CSCdi62107:  Multichassis config conflict with Dialer
 * Branch: California_branch
 *
 * Revision 3.3.18.4  1996/05/19  23:31:36  perryl
 * CSCdi56888:  Bundle partitioning should reset and display warning
 * Branch: California_branch
 *
 * Revision 3.3.18.3  1996/05/19  05:48:58  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.3.18.2  1996/05/01  02:49:37  dblair
 * CSCdi43824:  MLP Lost Fragment on async lines.
 * Branch: California_branch
 * Increase the maximum size of the unassigned fragment list
 *
 * Revision 3.3.18.1  1996/04/27  06:36:57  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.16.8  1996/04/17  00:01:30  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.16.7  1996/04/06  21:26:53  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.2.16.6  1996/04/03  22:59:14  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 3.2.16.5  1996/03/09  07:18:50  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 3.2.16.4  1996/03/08  17:30:33  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.2.16.3  1996/02/07  20:23:13  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 3.2.16.2  1996/02/04  06:29:40  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.2.16.1  1996/01/16  15:45:20  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.3  1996/02/07  19:46:09  dblair
 * CSCdi47994:  Router crashes when low on memory
 * Handle getbuffer fail correctly and clean up multilink data structures
 * when bundle goes down
 *
 * Revision 3.2  1995/11/29  01:59:08  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.1  1995/11/09  13:50:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/20  17:14:10  dblair
 * CSCdi40641:  Bridging over MLP ISDN PRI broken
 *         Bridge code assumed 6 byte header.  Multilink now completes
 *         header when receiving and encapsulated bridge packet
 *
 * Revision 2.1  1995/09/20  22:39:02  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__MLP_H__
#define __MLP_H__

/* Multilink PPP header masks  */
#define MLP_SEQ_MASK_24BITS   0xFFFFFFL
#define MLP_SEQ_RANGE_24BITS  MLP_SEQ_MASK_24BITS + 1
#define MLP_BEGBIT_MASK       0x80
#define MLP_ENDBIT_MASK       0x40

#define MAX_MLP_BUNDLES 64

#define MLP_LOST_FRAG_TIME  1 * ONESEC

#define MLP_UPDATE_DIALER_IDLE_INTERVAL   20 * ONESEC
#define MLP_MAX_IDLE_TIMEOUT              1000 * ONESEC
#define MLP_DEFAULT_LOAD_ANALYZE_TIME     2 * ONESEC
#define MLP_DEFAULT_WAIT_FOR_ADD_TIME    30 * ONESEC
#define MLP_DEFAULT_WAIT_BEFORE_DISCONNECT_TIME  30 * ONESEC
#define MLP_WAIT_BUNDLE_APPOINTMENT      10 * ONESEC

/*
 *  Lost fragment algorithms and constants
 */ 

#define MLP_LOST_FRAG_TIMER         0
#define MLP_LOST_FRAG_RFC           1
#define MLP_LOST_FRAG_UNA_GT_LINKS  2
#define MLP_LOST_FRAG_MAX_BUFFERS   3

/*
 *  maximum number of fragments in the unassigned list is:
 *  max(MLP_DEF_MIN_UNA_SIZE, (# of links in bundle) * 2^MLP_LOST_FRAG_EXP)
 */
#define MLP_LOST_FRAG_EXP           3
#define MLP_DEF_MIN_UNA_SIZE        30


#define MLP_SEQ_WINDOW_SIZE_24BITS  0x8000

#define MLP_DEFAULT_MIN_FRAG_SIZE   30

#define MLP_INIT_SEQ  0x0L

#define DEF_MULTILINK_MRRU  1524
#define MLP_24BITSEQ_HDR_SIZE 4  /* 4 = 8 total - 4 bytes for ppp header */

enum MLP_BUNDLE_STATE {
    MLP_BUNDLE_DOWN,             /* bundle is not operational */
    MLP_BUNDLE_ACTIVE,           /* bundle is in normal operation */
    MLP_BUNDLE_REMOVE,           /* bundle needs to be removed  */
    MLP_BUNDLE_CREATING,         /* bundle structures are being created */
				 /*   and linked  */
    MLP_BUNDLE_APPOINTED
};

struct mlplinktype_ {
    struct mlplinktype_ *next;
    struct mlplinktype_ *prior;
    mlpbundletype *bundle;        /* bundle this link is a member of */
    hwidbtype *idb;               /* hwidb this link is associated with */ 
    ulong frag_size;              /* size of the next fragment for this link */
    int num_frags;
    boolean rcv_out_of_order_frag; /* last fragment rcv'ed was out of order */
};

/*
 *  Multilink Bundle structure
 *   define info for bundles
 */

struct mlpbundletype_ {
    struct mlpbundletype_ *next;
    struct mlpbundletype_ *prior;
    mlplinktype *master_link;
    mlplinktype *first_link;
    mlplinktype *last_link;
    mlplinktype *next_xmit_link;     /* next link for transmit data  */
    enum MLP_BUNDLE_STATE state;
    int nlinks;                 /* number of links in the bundle */
    int min_links;              /* config:  min links required in bundle */ 
    int max_links;              /* config:  max links allowed in bundle */
    int max_frags;              /* config: max number of fragments per pak */ 
    boolean is_frag;            /* flag: fragmentation enable/disable */
    boolean is_lost_frag;       /* flag: TRUE indicates rcv frag lost */
    ulong mrru;                 
    ulong min_frag_size;        
    ulong send_seq;             /* transmit sequence number */
    ulong rcv_seq;              /* expected receive sequence number */
    ulong seq_window_size;      /* size of the range of expected seq numbers */
    ulong seq_window_threshold; 
    ulong lost_fragments;       
    ulong discarded_fragments;
    ulong received_lost;        /* # of fragments though lost but recieved later */
    ulong out_of_order;         
    ulong unassigned;        /* # of fragments queued that have not been reassembled */
    uchar load;              /* actual load used to test against threshold */ 
    paktype *una_frag_list;  /* list (sorted by seq )of unreassembled frags  */
    paktype *una_frag_end;   /* last frag on the list.  has lowest seq */
    uchar *working_pak_ptr;  /* points to location for next frag in working pak */
    paktype *working_pak;    /* packet used for reassembling frags */
    boolean rcved_end_bit;   /* flag: indicates that last frag rcved had end bit */
    uchar add_link_threshold;
    uchar delete_link_threshold;
    enum MULTILINK_THRESHOLD_DIRECTION thresh_direction;
    sys_timestamp lost_frag_timer; /* waiting for expected sequence number */
    int lost_frag_algo;         /* algorithm # for detecting lost fragment */
    sys_timestamp load_analyze_timer;  
    ulong load_analyze_ticks;  /* how long to wait before testing load */
    sys_timestamp wait_for_add_timer;  
    ulong wait_for_add_ticks;  
    sys_timestamp wait_before_disconnect_timer;
    ulong wait_before_disconnect_ticks;
    sys_timestamp appointment_timer;   /* mlp_remove_bundle if timer expires */
    boolean are_links_equal;   /* TRUE is all links have similar BW  */
    enum CALLBACK_TYPE callbacktype;
    char name[MAXSTR];	/* remote name bundle is connected to */
};

inline static ulong mlp_get_pak_seq (paktype *pak)
{
    return(GETLONG(pak->network_start) & MLP_SEQ_MASK_24BITS);
}

inline static boolean mlp_get_beg_bit (paktype *pak)
{
    
    return((*pak->network_start) & MLP_BEGBIT_MASK); 
    
}

inline static boolean mlp_get_end_bit (paktype *pak)
{
    
    return((*pak->network_start) & MLP_ENDBIT_MASK); 
    
}

inline static void mlp_set_beg_and_end_bits (char *pchar)
{
    
    *pchar = *pchar | (MLP_ENDBIT_MASK | MLP_BEGBIT_MASK);
    
}

inline static void mlp_set_beg_bit (char *pchar)
{
    
    *pchar = *pchar | MLP_BEGBIT_MASK;
    
}

inline static void mlp_set_end_bit (char *pchar)
{
    
    *pchar = *pchar | MLP_ENDBIT_MASK;
    
}

inline static ulong mlp_inc_sequence (ulong sequence)
{
    
    return((sequence + 1) & MLP_SEQ_MASK_24BITS);
    
}

inline static ulong mlp_add_sequence (ulong sequence, ulong offset)
{
    
    return((sequence + offset) & MLP_SEQ_MASK_24BITS);
    
}

/*
 * mlp_put_frag_seq 
 * put the fragment sequence number in the fragment
 */
inline static void mlp_put_frag_seq (paktype *fragment, ulong seq)
{
    uchar *p;
    p = fragment->datagramstart + fragment->encsize;
    seq = seq | GETLONG(p);
    PUTLONG(p, seq);
}

/*
 *  Returns the number of bytes in the sequence and beg/end bit fields
 */

inline static ushort mlp_get_seq_size (void)
{
    /* only support 4 byte sequence */
    return(MLP_24BITSEQ_HDR_SIZE);
}

/*
 * Loop for all bundles
 */

#define FOR_ALL_BUNDLES(bundle) \
    for ((bundle) = (mlp_bundle_list_head); (bundle); \
	 (bundle) = (bundle)->next)

/*
 * Loop for all links in a bundle
 */

#define FOR_ALL_LINKS_IN_BUNDLE(bundle, link) \
    for ((link) = (bundle)->first_link; (link); \
	 (link) = (link)->next)

/*
 *  Loop for all fragments in a packet
 */
#define FOR_ALL_FRAGMENTS_IN_PACKET(pak1, frag1) \
    for ((frag1) = (pak1); (frag1); \
	 (frag1) = (frag1)->mlp_next_frag)


/*
 *  is_mlp_links_greater_than_one(idb)
 *  return TRUE is numbers of members in multilink bundle is greater than one.
 */

static inline boolean is_mlp_links_greater_than_one (hwidbtype *idb)
{
  if (is_multilink(idb) && (idb->mlp_link->bundle->nlinks > 1))
      return(TRUE);
  else
      return(FALSE);
}

/*
 *  mlp_get_master_idb
 *  return the hwidb of the master link
 */
static inline hwidbtype * mlp_get_master_idb (hwidbtype *idb)
{
    return(idb->mlp_link->bundle->master_link->idb);
}

/*
 *  Finds the distance in the sequence space from seq1 to seq 2
 */
					  
inline static ulong mlp_frag_distance (ulong seq1, ulong seq2) {

    if (seq1 < seq2) {
	return(seq2 - seq1);
    } else if (seq1 > seq2) {
	return((seq2 + MLP_SEQ_RANGE_24BITS) - seq1);
    } else {
	return(0);
    }
    
    
}

/*
 * This inline checks to see if the incoming packet is outside the
 *   window of expected packet sequences.  This may occur when
 *   connected to a hostile peer who witholds packets for long
 *   periods or in extreme and massive fragment loss we may
 *   assign a fragment as lost and then receive it.
 * return(TRUE) if sequence is outside the window
 * otherwise return(FALSE)
 */

inline static boolean mlp_is_frag_outside_expected_seq (ulong rcv_seq, 
							ulong seq_window_size,
							ulong sequence)
{
    ulong distance;

    distance = mlp_frag_distance(rcv_seq, sequence);
    if (distance > seq_window_size)
	return(TRUE);
    return(FALSE);
}

/*
 *  Extern prototypes
 */
extern void mlp_subsys_init(void);
extern void mlp_setup(hwidbtype *);
extern boolean multilink_up(hwidbtype *);
extern void multilink_down(hwidbtype *);
extern char pppdebugbuffer[];
extern hwidbtype *mlp_create_bundle_interface(hwidbtype *, mlpbundletype *);
extern boolean mlp_holdq_enqueue(hwidbtype *, paktype *, enum HEADTAIL);
extern boolean mlp_xmit_fragments(hwidbtype *, paktype *, long);
extern boolean mlp_fragment(paktype *, mlpbundletype *);
extern void mlp_bundle_interface_up(hwidbtype *);
extern void mlp_bundle_reset (hwidbtype *);
extern void mlp_raw_enqueue(hwidbtype *, paktype *);
extern void mlp_bind_interface (hwidbtype *);
extern void mlp_unbind_interface (hwidbtype *);
extern int mlp_search_bundle(char *, int);
extern void mlp_appoint_bundle(char *);
extern int mlp_get_vtemplate_num(void);
extern void mlp_get_bundle_list(int* ,char **, int*);
extern boolean mlp_reset_bundle(char *);
extern hwidbtype *mlp_find_bundle_netidb(char *, hwidbtype **);
extern void mlp_process_pending_ncps(hwidbtype *, boolean);
extern queuetype mlp_pending_ncpQ;
#endif	__MLP_H__
