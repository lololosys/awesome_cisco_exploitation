/* $Id: tring_common.h,v 3.3.60.1 1996/03/18 20:15:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/tring_common.h,v $
 *------------------------------------------------------------------
 * tring_common.h - definition of common commands/responses to the
 * various on-board drivers.
 *
 * November 1990, David R. Hampton & Eric B. Decker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tring_common.h,v $
 * Revision 3.3.60.1  1996/03/18  20:15:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:06:26  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  14:24:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:31:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:53:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:55:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TRING_COMMON_H__
#define __TRING_COMMON_H__

/*
 * Define swapping macros.  These are needed because the definitions in
 * this file is referenced by code for three separate processors, two of
 * which are Motorola 68020s, and one Intel 80186.  They produce the
 * correct word and long word format for a processor, based upon compile
 * time defines.
 */
#ifndef FIXSWAP
#if (CPU_68K || CPU_MIPS || CPU_SPARC)
#define FIXSWAP(x)	x
#define FIXLSWAP(x)	x
#else
#define FIXSWAP(x)	((((x) & 0xff) << 8) | ((x) >> 8))
#define FIXLSWAP(x)	((((ulong) (x) & 0x000000ff) << 24) | \
			 (((ulong) (x) & 0x0000ff00) <<  8) | \
			 (((ulong) (x) & 0x00ff0000) >>  8) | \
			 (((ulong) (x) & 0xff000000) >> 24))
# endif
#endif

/*
 * Command Block Definition.
 *
 * A command block is the holder of the basic message being passed back
 * and forth between the Main CPU and the Tra.
 *
 * The current protocol is simple.  When a command is completed an
 * appropriate result code in placed in the result cell of the command
 * block and a completion interrupt is sent to the other side.
 *
 * There is also a class of commands called immediate commands.  These
 * commands do not generate completion interrupts but complete immediately.
 * Immediate commands are completly decoupled from the regular kind of
 * commands.  There can be an immediate command executed at the same time
 * a regular command is pending.  Currently the only class that uses
 * immediate commands are debugger commands.  Immediate commands are
 * unidirectional.  Only Mother can send them.
 *
 * At most one command of each type is outstanding at a time in each
 * direction.  This implements an implicit queue for all data buffer traffic
 * messages and error messages.  Error messages do not have a higher priority
 * than other traffic.  This also has the side effect of preserving temporal
 * information about the error being reported.  This is almost true.  If
 * the adapter (Tra) becomes congested exact temporal relationship is
 * lost.  Is this important?  Currently not important enough to really
 * complicate things more.
 *
 * Cmd and Result are ushorts to avoid having to deal with the nastiness
 * imposed by bus byteorder.  The 68000 was set up to access the intel
 * multibus in a rather interesting fashion.  When accessing bytes on even
 * addresses (in the 68K address space) it will actually access an odd byte
 * in the intel space.  Odd 68K bytes actually map into even intel bytes.
 * To avoid this nonsense 16 bit quantities are used that don't exhibit
 * this phenomena.
 *
 * The Intel side (netronix card only) is responsible for all byte swapping
 * to insure that the 68K side sees things straight.
 */

typedef struct _cmd_block_t {
    volatile ushort cmd_flag;
    volatile ushort rslt_flag;
    volatile ushort cmd;
    volatile ushort rslt;
    volatile ulong p1;			/* parameter 1 */
    volatile ulong p2;			/* parameter 2 */
} cmd_block_t;

/*
 * Cmd Fifo type looks just like a cmd_block only it has cmd_flag redefined
 * to be fifo_flag for use internally. It is used when a cmd block
 * can't go immediatly into the real command block but must be queued on a
 * fifo for later transmission.
 *
 * This fifo works exactly like the Rlist and Tlist fifos.
 */

typedef struct _cmd_fifo_t {
    ushort fifo_flags;
    ushort rslt_flag;
    ushort cmd;
    ushort rslt;
    ulong  p1;
    ulong  p2;
} cmd_fifo_t;

/*
 * CRITICAL must be non-zero and NONCRITICAL must be 0
 */
#define CFIFO_VALID   0x8000
#define CFIFO_CRITICAL        0X0001
#define CFIFO_NONCRITICAL 0




/*
 * Command Block definitions and formats.
 *
 * ->  denotes information flowing from Mother to Tra
 * <-  denotes information flowing from Tra to Mother
 * <-> denotes a bi-directional field.
 *
 * M2T command block.
 *
 * NOP
 *
 * cmd/rsp flags	4
 * CB_NOP		2	->
 * RSLT_OK		2	<-
 *
 * M2T_SOFT_RESET
 * 	undefined
 *
 * M2T_START
 *
 * cmd/rsp flags	4
 * CB_M2T_START		2	->
 * RSLT_OK		2	<-	RSLT_indicator	2	<-
 * start_blk pntr (p1)	4	->	fail reason		<-
 * ring_mode (p2, high)	2	->
 * fill			2	->
 *
 * M2T_START_BRIDGE
 *
 * cmd/rsp flags	4
 * CB_M2T_START_BRIDGE	2	->
 * RSLT_OK		2	<-	RSLT_indicator	2	<-
 * start_blk pntr (p1)	4	->	fail reason		<-
 * ring_mode (p2, high)	2	->
 * fill (p2, low)	2	->
 *
 *
 * A START can fail for a number of reasons.  Exactly how is indicated
 * by the RSLT_indicator and fail reason.  Below are the list of
 * possibilities:
 *
 * RSLT_START_FAIL_RESET   0x0000<intval>  hardware failed?, intval
 * 					   contains bits that could tell
 * 					   why.
 *
 * RSLT_START_FAIL_INIT	   0x0000<intval>  hardware failed?, intval
 * 					   contains bits that could tell
 * 					   why.
 *
 * 			   0x0000ffff	   scbssb dma didn't work.  scb0
 * 			   0x0000fffe	   scbssb dma didn't work.  scb1
 * 			   0x0000fffd	   scbssb dma didn't work.  scb2
 * 			   0x0000fffc	   scbssb dma didn't work.  ssb0
 * 			   0x0000fffb	   scbssb dma didn't work.  ssb1
 * 			   0x0000fffa	   scbssb dma didn't work.  ssb2
 * 			   0x0000fff9	   scbssb dma didn't work.  ssb3
 *
 * RSLT_START_FAIL_ADDR	   0x03004000	   node address error, either node
 * 					   address passed in bogus or BIA 0.
 *
 * RSLT_START_FAIL_OPEN	   0x01<cmd><flgs> <cmd> is 8 bits, <flgs> is 16.
 * 					   A cmd in the open process was
 * 					   rejected by the chip set.
 * 			   0x030002<flgs>  <flgs> is 8 bits.  Open error.
 * 					   A general failur.  <flgs> is two
 * 					   4 bit fields that tells which open
 * 					   phase and the kind of failure.
 * 			   0x0300<flgs>	   <flgs> is 16 bits.  The upper 7
 * 					   bits denote the kind of failure.
 *
 * RSLT_START_FAIL	   0x00000000	   It either works or it doesn't.
 * 					   Failure denotes a f/w problem.
 *
 * start_blk must always be present.  On input start_blk contains the
 * node_addr to use.  If the node_addr is 0 then Tra will attempt to use
 * the burned in address contained in the on board prom.  On return
 * node_addr will have been filled by Tra with the actual address being
 * used.
 *
 * The group and functional fields of the start_blk are used to set up
 * a group and/or functional address for the ring interface to pay attention
 * to.  The group address is essentially a single multi-cast address.  It
 * is limited to one by the TI chipset.  The functional address is a weird
 * but functional beast.  Each bit denotes a particular function that a
 * machine may perform.  If you set the bit on open you receive the packet
 * if it has the bit set in its dest address field if the address is a
 * functional address as defined by 802.5.  0 in either says use none.
 *
 * Start Bridge and Start are used to actually turn the interface on.  Start
 * uses node_addr through functional in the start_blk and Start Bridge uses
 * these plus max_rd, bridge_num, thisring, targetring, and the proto_block.
 * This is fully backward compatible with earlier implementations because the
 * offsets of previously defined cells have not changed.
 *
 * A Start_Bridge given to a pre-Bridge implementation will fail with
 * RSLT_BAD_CMD.  What is implemented can be determined by using the Async
 * command GET_CONFIG.  This will tell whether it is a basic configuration
 * (unimpl or all zero return) or if bridging and other advance features
 * are present.
 */




/*
 * Ring Mode definitions.  These are values passed from the main
 * system to the adapter boards indicating which mode the interface
 * should use.  They control whether or not certain function bits are
 * turned on, and the command flags used when actually inserting into
 * the token ring.
 */
#define RINGMODE_LOOPBACK	0x8000
#define RINGMODE_NO_RINGSTAT	0x4000
#define RINGMODE_ALL_FRAMES	0x2000
#define RINGMODE_ALL_LLC	0x1000
#define RINGMODE_BRIDGE		0x0800	/* status only */
#define RINGMODE_REM		0x0400	/* be Ring Error Monitor */
#define RINGMODE_RPS		0x0200	/* be Ring Parameter Server */
#define RINGMODE_NETMGR		0x0100	/* be Config Report Server */
#define RINGMODE_TBRIDGE	0x0080	/* be a transparent bridge */
#define RINGMODE_CONTENDER	0x0040	/* be a contender for AMP */
#define RINGMODE_RS		0x0020	/* listen to ring maintenance MAC frames */
#define RINGMODE_ALL_MAC	0x0010	/* listen to all MAC frames */
#define RINGMODE_ETR		0x0008	/* Early Token Release */

#define RINGMODE_NEED_MAC	0x0730	/* Needs MAC frames */
/*
 * Failure codes returned by the TI Chipset when attempting an open
 * command.  The phase code tells which of the five phases in opening the
 * ring was ocurring when the error happened.  The error code tells which
 * error occurred during that phase of insertion.
 */

#define OPN_PHASE_MASK	0x00F0
#define OPN_CODE_MASK	0x000F

/*
 * Buffer Flags - These flags are passed from the adapter card to the
 * main system with each packet received from the token ring.  They are
 * used to indicate what type of packet was received by the adapter card.
 * This is preclassification that is performed by the adapter.
 */

#define BF_NONE		0x0000
#define BF_LOCAL	0x0001
#define BF_BRIDGE	0x0002
#define BF_MAC_PKT	0x0004
#define	BF_MYMAC	0x0008
#define BF_MYGROUP	0x0010
#define BF_MYFUNC	0x0020
#define BF_BROAD	0x0040
#define BF_EXPLORER	0x0080
#define BF_MULTI	0x0100
#define	BF_XIDTEST	0x0200		/* only if BF_BRIDGE set */
#define BF_SBECHECKED	0x0400
#define BF_TBRIDGE	0x0800		/* Candatate for transparent bridge */
#define BF_EAVES	0x8000
#define BF_MYMULTI	(BF_MYGROUP | BF_MYFUNC | BF_BROAD)

#define SBE_INVALID	0x0000		/* ready for adapter */
#define SBE_IGNORE	0x4040		/* TI screwup, skip this */
#define SBE_VALID	0x8080		/* ready for main system */


#define RNG_SIGNAL_LOSS	FIXSWAP(0x8000)
#define RNG_HARD_ERROR	FIXSWAP(0x4000)
#define RNG_SOFT_ERROR	FIXSWAP(0x2000)
#define RNG_BEACON	FIXSWAP(0x1000)
#define RNG_WIRE_FAULT	FIXSWAP(0x0800)
#define RNG_HW_REMOVAL	FIXSWAP(0x0400)
#define RNG_RMT_REMOVAL	FIXSWAP(0x0100)
#define RNG_CNT_OVRFLW	FIXSWAP(0x0080)
#define RNG_SINGLE	FIXSWAP(0x0040)
#define RNG_RECOVERY	FIXSWAP(0x0020)
#define RNG_UNDEFINED	FIXSWAP(0x021F)

#define RNG_FATAL	FIXSWAP(0x0d00)
#define RNG_AUTOFIX	FIXSWAP(0x0c00)
#define RNG_UNUSEABLE	FIXSWAP(0xdd00)	/* may still be open */





#define	CB_CMD_VALID		0x3535
#define CB_CMD_INVALID		0x0

#define CB_NOP			0x0000
#define CB_M2T_SOFT_RESET	0x0101
#define CB_M2T_START		0x0202
#define	CB_M2T_STOP		0x0303
#define CB_M2T_START_BRIDGE	0x0404

#define CB_M2T_SMT_STATS	0x2626
#define CB_M2T_MEMTEST		0x2828	/* sbe only */

/*
 * Command Block definitions.  T2M (Token to Mother)
 */

#define CB_T2M_RING_STAT	0x3131
#define CB_T2M_DEBUG_INFO	0x3232
#define CB_T2M_PANIC_INFO	0x3333

/* Async only */

#define CB_ASYNC_GENERAL	0x5050
#define CB_ASYNC_NOP50		0x5050
#define CB_ASYNC_GET_VERSION	0x5151
#define CB_ASYNC_MODIFY		0x5858
#define CB_ASYNC_UPDATE_COUNTS	0x5c5c	/* sbe only */
#define CB_ASYNC_START_GDB	0x5d5d	/* sbe only */
#define CB_ASYNC_BLAST		0x5e5e	/* sbe only */
#define CB_ASYNC_SOFT_RESET	0x5f5f	/* sbe only */
#define CB_ASYNC_SET_FUNCTIONAL	0x6060
#define CB_ASYNC_SET_GROUP	0x6161
#define CB_ASYNC_SET_OPTIONS	0x6262
#define CB_ASYNC_ENABLE 	0x6363	/* sbe only */
#define CB_ASYNC_READL_MEMX 	0x7070	/* str only */
#define CB_ASYNC_WRITEL_MEMX 	0x7171	/* str only */
#define CB_ASYNC_READS_MEMX 	0x7272	/* str only */
#define CB_ASYNC_WRITES_MEMX 	0x7373	/* str only */
#define	CB_ASYNC_STOP		0x8080  /* str only */

/*
 * Response codes.  Used by both T2M and M2T.
 */

#define	RSLT_VALID		0x4343
#define	RSLT_PENDING		0xffff
#define	RSLT_INVALID		0x0

#define RSLT_NONE		0x0000
#define RSLT_OK			0x0101
#define RSLT_ABUSE		0x0202
#define RSLT_TOOSMALL		0x0303
#define RSLT_TOOBIG		0x0404
#define RSLT_ADDR_RANGE		0x0505
#define RSLT_ADDR_ALIGN		0x0606
#define RSLT_ADDR_ERR		0x0707
#define RSLT_CMD_FORMAT		0x0808
#define RSLT_UNIMPL		0x0909
#define RSLT_BAD_CMD		0x0a0a
#define RSLT_ERR_INTERNAL	0x0b0b
#define RSLT_TBUF_COPIED	0x0c0c
#define	RSLT_RBUF_COPIED	0x0d0d
#define RSLT_NO_RBUFS		0x0e0e
#define RSLT_ALRDY_SET		0x0f0f
#define RSLT_NOT_SET		0x1010
#define RSLT_START_FAIL		0x1111
#define RSLT_START_FAIL_RESET	0x1212
#define RSLT_START_FAIL_INIT	0x1313
#define RSLT_START_FAIL_OPEN	0x1414
#define RSLT_START_FAIL_ADDR	0x1515
#define RSLT_MODIFY_BAD_CELL	0x1616
#define RSLT_MODIFY_BAD_DATA	0x1717
#define RSLT_MODIFY_BRIDGE_OFF	0x1818
#define RSLT_BRIDGE_BAD_PARAM	0x1919
#define RSLT_MODE_FAILED	0x1a1a
#define RSLT_BAD_FUNCTIONAL	0x1b1b
#define RSLT_BUSY		0x1c1c
#define RSLT_CFGBR_FAILED	0x1d1d	/* sbe only */
#define RSLT_FAIL_SOFT_RESET	0x1e1e	/* sbe only */
#define RSLT_FAIL_MEMTEST	0x1f1f	/* sbe only */
#define RSLT_FAIL_BOARD_RESET	0x2020	/* sbe only */
#define RSLT_FAIL_NOMEMORY	0x2121
#define RSLT_FAIL_HARD_RESET	0x2222	/* sbe only */

#define RSLT_NOTSTOPPED		0x4040
#define RSLT_NOTRUNNING		0x4141
#define RSLT_NOTFOUND		0x4242
#define RSLT_DBG_RUNNING	0x4343
#define RSLT_DBG_NOTACTIVE	0x4444

/*
 * Catistrophic failure codes.  These show up in the
 * sca_fail cell if TRA has died horribly.
 */

#define SCA_FAIL_NONE		0
#define SCA_FAIL_BAD_EXIT	1
#define SCA_FAIL_STK_OVERFLOW	2
#define	SCA_FAIL_STK_UNDERFLOW	3
#define SCA_FAIL_STK_GUARD	4
#define SCA_FAIL_UNEXPECT_INT	5
#define	SCA_FAIL_PARITY		6
#define SCA_FAIL_TIMEOUT	7
#define SCA_FAIL_MBACCESS	8
#define SCA_FAIL_SCAPROTO	9
#define SCA_FAIL_ALIGN		0xa
#define SCA_FAIL_SEGWRAP	0xb
#define	SCA_FAIL_ADDRRANGE	0xc
#define	SCA_FAIL_DBGPROTO	0xd
#define SCA_FAIL_MEMTRASH	0xe
#define	SCA_FAIL_T2MTRASH	0xf
#define SCA_FAIL_T2MSEQ		0x10
#define SCA_FAIL_T2MCLOG	0x11
#define SCA_FAIL_T2MBUSY	0x12
#define SCA_FAIL_BADRBUF_CMPLT	0x13
#define SCA_FAIL_RLIST_TRASH	0x14
#define SCA_FAIL_TLIST_TRASH	0x15
#define SCA_FAIL_BAD380INT	0x16
#define SCA_FAIL_380CMDFAIL	0x17
#define SCA_FAIL_380CHECK	0x18
#define SCA_FAIL_380BADRECV	0x19
#define SCA_FAIL_380BADTXMT	0x1a
#define SCA_FAIL_380BADCLOSE	0x1b
#define SCA_FAIL_380TXINT	0x1c
#define SCA_FAIL_380RXINT	0x1d
#define SCA_FAIL_RXPTR_TRASH	0x1e
#define SCA_FAIL_TXPTR_TRASH	0x1f
#define SCA_FAIL_MBMEM_TEST	0x20
#ifdef notdef
#define SCA_FAIL_RECOVERY	0x21
#define SCA_RECOVERY_INFO	0x22
#endif
#define SCA_FAIL_TIUCODE	0x23	/* sbe only */
#define SCA_FAIL_BADARBCMD	0x24	/* sbe only */
#define SCA_FAIL_BADSRBCMD	0x25	/* sbe only */
#define SCA_FAIL_BADSSBCMD	0x26	/* sbe only */
#define SCA_FAIL_OPEN_TIMEOUT		0x27	/* tms380 only */
#define SCA_FAIL_CHGMODE_TIMEOUT	0x28	/* tms380 only */
#define SCA_FAIL_CFGBRIDGE_TIMEOUT	0x29	/* tms380 only */
#define SCA_FAIL_SET_ADDR_TIMEOUT	0x2a	/* tms380 only */
#define SCA_FAIL_READ_ERROR_LOG_TIMEOUT	0x2b	/* tms380 only */
#define LAST_SCA_FAIL			0x2c

/*
 * Diagnostic codes
 */
#define DI_AUTOFIXING		0x1L
#define DI_AUTOFIXED		0x2L
#define DI_PANIC		0x3L


/*
 * Codes used to modify ring parameters for source route bridging
 */
#define TR_MODIFY_NONE  0
#define TR_MODIFY_MAXRD 1
#define TR_MODIFY_BNUM  2
#define TR_MODIFY_RING  3
#define TR_MODIFY_TRING 4
#define TR_MODIFY_SPAN  5


/*
 * Other Miscellanea
 */
#define MAC_VERS_LEN 10

/*
 * Mac Frame Major Vector - Class defines
 */

#define MVID_DEST_CLASS_MASK 0xf0

#define RNG_STATION    0x0
#define LLC_MGR        0x1
#define NET_MGR        0x4
#define RNG_PARAM_SVR  0x5
#define RNG_ERR_MON    0x6


#endif /* __TRING_COMMON_H__ */
