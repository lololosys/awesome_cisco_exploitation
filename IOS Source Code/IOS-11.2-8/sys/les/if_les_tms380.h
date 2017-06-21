/* $Id: if_les_tms380.h,v 3.3.62.4 1996/08/28 12:59:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_tms380.h,v $
 *------------------------------------------------------------------
 * if_les_tms380.h - Pancake and xx Network Tokenring interface driver
 *
 * March 1992, Marciano Pitargue
 * hacked from if_pan_tms380.h if_xx_tms380.h
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_tms380.h,v $
 * Revision 3.3.62.4  1996/08/28  12:59:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.3  1996/08/22  05:15:36  echeng
 * CSCdi62388:  Many bugs in the code handling TokenRing error counters
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/08/13  02:20:46  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.62.1  1996/03/18  20:42:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:05:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  14:42:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:41:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/05  20:49:46  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.1  1995/06/07 21:37:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

typedef struct tms380_instance tms380_instance_t;

#include "if_les_madge.h"

#if defined(XX) || defined(SIERRA)
#include "if_c4000_tms380.h"
#endif 

#if defined(PAN)
#include "if_c3000_tms380.h"
#endif /* PAN */

/* The page mapping bits are no longer used */
/* csr0 - bit 0:     1 - assert reset, 0 - de-assert reset
 *        bit 1:     tokenring speed: 1 = 16mb, 0 = 4mb
 *        bit 3,2:   remapping size
 *	             0x   - 64K blocks (00 or 04)
 *	             10   - 32K blocks (08)
 *	             11   - 16K blocks (0C)
 * 2Rnim bit 4       1 - enable burst mode, 0 - disable (normal for 1R)
 */

/* for c16 bit 4 can be used to set speed */
/* defaults to 16 mbit ring speed */
#define C16_SPEED_SET_BIT	0x0010  /* 1 - 4MB, 0 - 16MB */
#define C16_SPEED_SELECT_BIT	0x0020  /* if set, bit 4 controls speed */


/*
 * The number of buffers in the cache has to be equal to the number of 
 * slots in the receive ring. If not we will run out of buffers really
 * fast.
 */

#define TMS380_NUM_BUF(rng_size)   ((rng_size) + (rng_size>>1)) 
#define TMS380_NUM_CACHE(rng_size) (rng_size>>1)

#define TMS380_LARGE_BUF_SIZE   1016

/* enable burst mode - 2Rnim only */
#define TMS380_2RNIM_BURST_MODE               0x0010


/***********************************************************************
 *
 *                     Frame Buffer organization
 *
 * Madge frame buffers are implemented as a sequence of bytes.  There is
 * a small header, followed immediately by the data.  The start of the
 * next frame may be found by rounding up to the next longword boundary.
 * This architecture allows any size packets to be transmitted in any
 * order without waste of space in memory.  If a frame cannot fit
 * entirely in the space remaining in the 64K buffer, then the buffer is
 * wrapped, and the entire frame placed at the beginning of the buffer.
 *
 * Three pointers are kept into the receive buffer area; the host
 * pointer, the adapter pointer, and the wrap pointer.  As the adapter
 * receives frames, it advances the adapter pointer.  As the host chews
 * these frames, it advances the host pointer.  This means that new
 * frames can be found by looking in the area between the host pointer
 * and the adapter pointer.  When the adapter reaches the top of the
 * buffer, it sets the wrap pointer to it's current location, and resets
 * the adapter pointer back to the start of the buffer.  The only catch
 * in all of this is when the host and adapter pointers point to the same
 * location, the wrap pointer must checked to see if the buffer is empty
 * or full.
 *
 *         +---------+  <-------- end of receive buffer
 *         |         |
 *         |  empty  |
 *         |         |
 *         +---------+  <--------------- RX_WRAP_PTR
 *         | frame 2 |
 *         +---------+
 *         |         |
 *         | frame 1 |
 *         |         |
 *         +---------+  <--------------- RX_HOST_PTR
 *         |         |
 *         |         |
 *         |  empty  |
 *         |         |
 *         |         |
 *         +---------+  <--------------- RX_ADAPTER_PTR
 *         |         |
 *         | frame 3 |
 *         |         |
 *         +---------+  <-------- start of receive buffer
 *
 *
 * Inside of each received 'frame' is the following information according to
 * the Madge FastMAC specification:
 *
 *  FASTMAC receive frame format:
 *     BYTE    space[rx_space]	   Fixed size defined in load parms.
 *     WORD    frame_length	   Includes CRC if present
 *     WORD    rx_timestamp
 *     BYTE    ac_byte		   bit 0 => internally matched
 * 				   bit 1 => externally matched
 * 				   bit 2 => CRC included
 *     BYTE    fc_byte
 *     BYTE    rest_of_frame[nn]
 *     DWORD   crc		   Only present if AC_BYTE bit-2 set
 *
 *
 * The 'space' area is used for the monitor code to pass preprocessing
 * information.
 *
 ********************
 *
 * The same method is used for the transmit buffer, only the adapter
 * pointer follows the host pointer.
 *
 *         +---------+  <-------- end of receive buffer
 *         |         |
 *         |  empty  |
 *         |         |
 *         +---------+  <--------------- TX_WRAP_PTR
 *         |         |
 *         | frame 1 |
 *         |         |
 *         +---------+  <--------------- TX_ADAPTER_PTR
 *         |         |
 *         |         |
 *         |  empty  |
 *         |         |
 *         |         |
 *         +---------+  <--------------- TX_HOST_PTR
 *         |         |
 *         | frame 2 |
 *         |         |
 *         +---------+  <-------- start of receive buffer
 *
 *
 *
 *  FASTMAC transmit frame format:
 *     BYTE    space[tx_space]	   Fixed size defined in load parms.
 *     WORD    data_length	   Includes CRC if present
 *     WORD    tx_timestamp
 *     BYTE    ac_byte		   bit 2 => CRC included
 *     BYTE    fc_byte
 *     BYTE    rest_of_frame[nn]
 *
 *  Note:  If the CRC-included bit is set then the last four bytes of
 *         data will be interpreted as the CRC whether they occur in
 *         the immediate data or the indirect buffer.
 *
 ***********************************************************************/
#define MAX_FRAME	idb->max_pak_size
#define MIN_FRAME	14
#define FM_BUF_ALIGN	4

/*
 * for dosxx, we will use tx descriptors.  there will be one buffer
 * for tx.  for receive, we will *NOT* use rx descriptors and there
 * will be several (TMS380_PAGES) receive buffers used, however used one
 * at a time. the tx descriptors will point to the correct part of the
 * receiver buffer or pak (if it is not fast switched).  there is only
 * one tx buffer since we are only putting fm_transmit_t info in there.
 * there are several rx buffers to handle bursts better so that in the
 * event that the madge needs more space, it can grab another page.
 * the pad field (see data structure below) of the first packet of page 
 * is important in 2 ways.  the most significant bit is set if the page
 * is not the current page being used for receive.  this is set in the
 * case that some other driver might have a packet still inside the
 * frame, w/ the page still being the current page.
 * this bit will tell us to put the page in the free queue of pages
 * or not.  the 2nd important part of pad is that it will be a counter
 * of how many packets inside the frame are still outstanding.  if it
 * is 0, there are no packets in the page being used currently.  however
 * just this information alone cannot guarentee the page is not being
 * used currently as the receive buffer page.  (henceforth the msb being
 * set if not current receive page.)
 */

#define USE_TRANSMIT_DESC 0x8000

struct fm_receive_t_ {
    ushort buf_flags;           /* Type of packet */
    ushort type;                /* Packet classification on rcv */
    ushort status;              /* status given by madge */
    ushort rif_length;          /* Length of the RIF field */
    ushort rd_offset;           /* RD that matched our config */
    ushort llc_offset;          /* Start of llc */
    ushort data_offset;         /* Start of data */
 
    uchar  *data;
    ushort data_length;
    paktype *pak;
} ;

#define MADGE_LONG_ALIGN(a) \
    ((a + sizeof(fm_transmit_t) - 1 - 4) & (~(sizeof(ulong)-1)))

struct fm_transmit_t_ {
    /* Defined by us. Length must be long word multiple. */
#define FM_TX_SPACE     16
    idbtype *if_input;		/* idb this packet came in on */
    ulong flags;		/* all good data structs has a flags field */
    ushort buf_flags;           /* Type of packet */
    ushort refcount;		/* for shared packet */
    paktype *pak_addr;	        /* the packet buffer for this descriptor */

    /* Defined by Madge */
    ushort length;         	/* always zero if using descriptors */
    ushort tx_timestamp;
    ushort pakpad;              /* copy data starting here */
    ushort data_length;		/* Includes CRC if present*/
    ulong  data_start;
};

/* time stamped debugging stuff - dit */

#ifdef DEBUG_TIMES
#define TIME_BASE \
    sys_timestamp base_380 = 0; \
    uint indent_380 = 0;\
    char *spaces_380[] \
        = {\
	   "******",\
	   " ","  ","   ","    ","     ",  /* 1 ..5 */\
	   "      ","       ", "        ","         ","          ", /* 6- */\
	   "           " /* 11- */ \
	   }
    
extern sys_timestamp base_380;
extern uint indent_380;
extern char *spaces_380[];

#define BEG_TIME_380(a) \
    if (dbgflags & DBG_TRACE) { \
    indent_380++;\
    ttyprintf(CONTTY,"\n%5d.%3d: tr(%d)%s beg %s", \
	      ELAPSED_TIME(base_380)/1000,\
	      ELAPSED_TIME(base_380)%1000,\
	      idb->unit, \
	      indent_380 > 11 ? "*********"  : spaces_380[indent_380], \
	      a); \
	   }
#define END_TIME_380(a) \
    if (dbgflags & DBG_TRACE) { \
    ttyprintf(CONTTY,"\n%5d.%3d: tr(%d)%s end %s", \
	      ELAPSED_TIME(base_380)/1000,\
	      ELAPSED_TIME(base_380)%1000,\
	      idb->unit, \
	      indent_380 > 11 ? "*********"  : spaces_380[indent_380], \
	      a);\
    indent_380--; \
	   }
#define MSG_TIME_380(a,b) \
    if (dbgflags & DBG_TRACE) { \
    ttyprintf(CONTTY,"\n%5d.%3d: tr(%d)%s    " a "", \
	      ELAPSED_TIME(base_380)/1000,\
	      ELAPSED_TIME(base_380)%1000,\
	      idb->unit, \
	     (indent_380 + 1) > 11 ? "*********"  : spaces_380[indent_380+1], \
	      b); \
	   }

#define ENTER_380(a) \
    if (dbgflags & DBG_TRACE) { \
    ttyprintf(CONTTY,"\n%5d.%3d:      %s    %s ", \
	      ELAPSED_TIME(base_380)/1000,\
	      ELAPSED_TIME(base_380)%1000,\
	      (indent_380 + 1) > 11 ? "*********"  : spaces_380[indent_380+1], \
	      a); \
	}

#define ENTER_RSRB(a) \
    if (dbgflags & DBG_RSRB) { \
    ttyprintf(CONTTY,"\n%5d.%3d:      %s    %s ", \
	      ELAPSED_TIME(base_380)/1000,\
	      ELAPSED_TIME(base_380)%1000,\
	      (indent_380 + 1) > 11 ? "*********"  : spaces_380[indent_380+1], \
	      a); \
	   }

#else

#define TIME_BASE

#define BEG_TIME_380(a) 
#define END_TIME_380(a) 
#define MSG_TIME_380(a,b) 
#define ENTER_380(a)
#define ENTER_RSRB(a) 

#endif DEBUG_TIMES


/* use DEBUG_XX for XX */
#ifdef DEBUG

extern ulong dbgflags;

#define DBG_BASE(a) ulong dbg_flags = a;

#define DBGBEG(flag)  if(flag & dbgflags) {
#define DBGELSE } else {
#define DBGEND }

#define DTRACE(name) \
    if (DBG_TRACE & dbgflags) \
    ttyprintf(CONTTY,"TR(%d): %s: idb = 0x%x\n", \
	      idb->unit, name, idb);
#define DBG(f,s,a) \
    if (f & dbgflags) \
    ttyprintf(CONTTY,s,a)
#define DBG0(f,s) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s);
#define DBG1(f,s,a1) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s,a1);
#define DBG2(f,s,a1,a2) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s,a1,a2);
#define DBG3(f,s,a1,a2,a3) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s,a1,a2,a3);
#define DBG4(f,s,a1,a2,a3,a4) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s,a1,a2,a3,a4);
#define DBG8(f,s,a1,a2,a3,a4,a5,a6,a7,a8) \
    if (f & dbgflags)  \
    ttyprintf(CONTTY,s,a1,a2,a3,a4,a5,a6,a7,a8);

#else /* DEBUG */

#define DBG_BASE(a) 

#define DBGBEG(flag)  if(FALSE) {
#define DBGELSE } else {
#define DBGEND }

#define DTRACE(name) 
#define DBG(f,s,a)
#define DBG0(f,s) 
#define DBG1(f,s,a1) 
#define DBG2(f,s,a1,a2) 
#define DBG3(f,s,a1,a2,a3)
#define DBG4(f,s,a1,a2,a3,a4)
#define DBG8(f,s,a1,a2,a3,a4,a5,a6,a7,a8)
#endif /* DEBUG */


#define DBG_TRACE     0x0001
#define DBG_PAK       0x0002
#define DBG_ADAPT     0x0004
#define DBG_MADGE     0x0008
#define DBG_MADGE_IN  0x0010
#define DBG_MADGE_OUT 0x0020
#define DBG_SRB       0x0040
#define DBG_ARB       0x0080
#define DBG_ADAPT_CHK 0x0100
#define DBG_CRC       0x0200
#define DBG_FST       0x0400
#define DBG_RSRB      0x0800

#define DBG_MADGE_TX  0x1000
#define DBG_TIMER     0x2000
#define DBG_XX        0x4000
#define DBG_2RNIM     0x8000

#define DBG_NOREM     0x00100000
#define DBG_NOLOC     0x00200000
#define DBG_DLSW      0x00400000

/*
 * rvalues for tms380_tx_pak
 */
enum TXRV {
    TXRV_DROPPED,	/* packet too big */
    TXRV_QUEUE_IT,	/* no room on Tx ring */
    TXRV_SENT		/* pkt sent */
    };



/*
 * Prototypes
 */
boolean init_tms380_idb_common(hwidbtype *idb);
boolean tms380_initialize(hwidbtype *idb);
void tms380_certain_death(hwidbtype *idb, uint fail, ulong fail_code);
void tms380_sif_handler(hwidbtype *idb, volatile ushort *sifint_p,
			ushort intval);
boolean tms380_srb_common(hwidbtype *idb, tring_hdr *trh, paktype *pak,
			  uint buf_flags, int riflen, int rdoffset,
			  sap_hdr *llc1);
void tms380_pnt_ptrs(hwidbtype *idb);
void tms380_possibly_display_ringstat(hwidbtype *idb, uint ring_stat);
void tms380_add_counters(hwidbtype *idb);
paktype *tms380_getbuf(hwidbtype *idb, boolean cache_ok);


/*
 * Externs
 */
extern void tms380_subsys_init(subsystype *);
extern void  arb_req(hwidbtype *idb, tms380_instance_t *ds,
		     struct _sifreg_t *sif_regs, volatile ushort *sifint_p,
		     ushort intval);

/* from if_les_madge.c */
extern boolean init380(hwidbtype *idb, ulong *fail_p);
extern void tms380_adapter_check(hwidbtype *idb, int intval, int where);
extern boolean hardreset380(hwidbtype *idb, ulong *fail_p);
extern boolean softreset380(hwidbtype *idb, ulong *fail_p);
extern void tms380_disable_host_interrupts(hwidbtype *idb);
extern void tms380_enable_host_interrupts(hwidbtype *idb);
extern boolean open380(hwidbtype *idb, ulong *fail_p);
extern void close380(hwidbtype *idb);
extern boolean chgmode380(hwidbtype *idb, ulong *fail_p);
extern boolean cfgbridge380(hwidbtype *idb, boolean enable, ulong *fail_p);
extern boolean set_address_380(hwidbtype *idb, ushort command, 
			       ulong address, ulong *fail_p);
extern boolean tms380_get_stats(hwidbtype *idb);
extern boolean tms380_get_stats_copy(hwidbtype *idb);
extern void grab_380_smt_stats(hwidbtype *idb);

/* defined in either if_c4000_tms380.c or if_c3000_tms380.c */
extern void tms380_get_default_hw_address(hwidbtype *idb, 
					  uchar addr[IEEEBYTES]);
extern void tms380_set_ringspeed(hwidbtype *idb);
extern void tms380_reset(hwidbtype *idb);
extern void init_tms380_dev(hwidbtype *idb);
extern void tms380_vendor_init(hwidbtype *idb);
extern void tms380_print_nim_id(hwidbtype *idb);
