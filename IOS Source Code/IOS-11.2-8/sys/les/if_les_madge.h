/* $Id: if_les_madge.h,v 3.4.12.2 1996/08/28 12:58:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_madge.h,v $
 *------------------------------------------------------------------
 * if_les_madge.c - low level code to deal with madge firware and tms380
 *
 * May 1993       - Dit Morse
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * originally in : if_les_tms380.h - Low end madge specific defs
 *
 *       March 1992, Marciano Pitargue
 *       hacked from if_pan_tms380.h if_xx_tms380.h
 *
 * See code pulled from if_les_tms380.c (1.33), and if_px_tms380.c (1.34)
 *------------------------------------------------------------------
 * $Log: if_les_madge.h,v $
 * Revision 3.4.12.2  1996/08/28  12:58:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.12.1  1996/03/18  20:42:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/16  07:12:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.1  1996/01/24  22:24:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/14  23:22:51  jcchan
 * CSCdi48756:  Race condition for TMS380 input driver
 *
 * Revision 3.3  1995/11/17  17:40:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:36:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define TMS380_MAX4LLC  4472	/*  4MB parameter */
#define TMS380_MAX4INFO (TMS380_MAX4LLC - SNAP_HDRBYTES) /* max proto data (no LLC encap) */
#define TMS380_MTU4     TMS380_MAX4INFO

#define TMS380_DEFLLC  8144	/* 16MB parameter */
#define TMS380_MAXLLC  17800	/* 16MB parameter */
#define TMS380_DEFINFO (TMS380_DEFLLC - SNAP_HDRBYTES) /* max proto data (no LLC) */
#define TMS380_MAXINFO (TMS380_MAXLLC - SNAP_HDRBYTES) /* max proto data (no LLC) */
#define TMS380_DEFDATA TR_MAXENCSIZE + TMS380_DEFINFO + TR_FCS_SIZE
#define TMS380_MAXDATA TR_MAXENCSIZE + TMS380_MAXINFO + TR_FCS_SIZE

#define TMS380_MTU16     TMS380_DEFINFO
#define TMS380_MTUTOBUFSIZ (TR_MAXENCSIZE + SNAP_HDRBYTES + TR_FCS_SIZE)

/*
 * xmit timeout value -- if no packets are transmitted in this time,
 * assume the board is dead, and reset it.
 */
#ifdef DEBUG
#define TMS380_XMITTIMEOUT (3600*ONESEC)
#else
#define TMS380_XMITTIMEOUT (2*ONESEC)
#endif

/*
 * Timeout values:
 */
#define TMS380_INIT_TIMEOUT		6	/* 6 * 0.5 sec = 3 sec */
#define TMS380_OPEN_TIMEOUT		30	/* sec */


/***************************************************************************/
/* Beginning of code leveraged from David Hampton's sbe16/madge/if_madge.h */
/***************************************************************************/
/***********************************************************************
 *
 * if_les_madge.h
 *
 * Describe fundamental constants of the TI380 chipset.  Almost all data
 * found in this file was obtained from the Texas Instruments "TMS380
 * Second Generaion Token Ring" specification, and the Madge FASTMAC
 * Specification.
 *
 * Hacked for Merlot tokenring
 * June 1991, Chris Shaker
 *
 * January 1991, David R. Hampton
 *
 ***********************************************************************/

/***********************************************************************
 *
 *		  TMS380C 16/04 Internal Memory Map
 *
 * 00.0000 - 00.000D	Burned In address
 * 00.000E - 00.7FFF	Code
 * 00.8000 - 00.FFFF	MAC Code
 *
 * 01.0000 - 01.007E	Interrupt Vectors
 * 01.0080 - 01.0FFF	Unused
 * 01.0100 - 01.01FF	TMS380C 16/04 Registers
 * 01.0200 - 01.03FF	Reserved for external address checker
 * 01.0400 - 1F.FFFF	Data Area
 *
 ***********************************************************************/

#define ADDR_CHPT0		0x0000
#define ADDR_CHPT1		0x0001
#define INITBLK_ADDR		0x0a00
#define TI_INTERNAL_ADDR	0x0a00
#define MADGE_INTERNAL_ADDR	0x1000
/*
 * madge defined...
 */
#define   SRB_PTR       0x1000
#define   SSB_PTR       0x1002
#define   ARB_PTR       0x1004
#define   STB_PTR       0x1006
#define   IPB_PTR       0x1008
#define   CHECK_PTR     0x1014

/******************************************************************
 *
 *		TI380 direct I/O interface definition
 *
 ******************************************************************/
typedef struct _sifreg_t {	/* TI manual names: */
    volatile ushort data;	/* SIFDAT */
    volatile ushort datainc;	/* SIFDAT/INC */
    volatile ushort old_addr;	/* SIFADR */
    volatile ushort intreg;	/* SIFCMD/SIFSTS */
    volatile ushort actl;	/* SIFACL */
    volatile ushort adr;	/* SIFADR */
    volatile ushort adrx;	/* SIFADX */
    volatile ushort dmalen;	/* DMALEN */
} sifreg_t;

/**********************************************************************
 *
 *                Adapter Control Register Values
 *
 * This register allows the attach processor to control and reconfigure
 * the adapter under software control.
 *
 **********************************************************************/
/* TI bits 0-3 are indeterminate */
#define TRACTL_SWHLDA   0x0800	/* do not use */
#define TRACTL_SWDDIR   0x0400	/* read only */
#define TRACTL_SWHRQ    0x0200	/* read only */
#define TRACTL_PSDMAEN  0x0100  /* pseodu DMA enable */
#define TRACTL_ARESET	0x0080	/* reset the adapter */
#define TRACTL_CPHALT	0x0040	/* halt the CP */
#define TRACTL_BOOT	0x0020	/* RAM based */
/* TI bit 11 is reserved. Must be set to zero */
#define TRACTL_SINTEN	0x0008  /* enable adapter to 020 interrupts */
#define TRACTL_PEN	0x0004  /* enable parity within adapter */
#define TRACTL_NSEL0  0x0002  /* network select; used to set ring speed */
#define TRACTL_NSEL1  0x0001  /* network select; used to set ring speed */


#define TRACTL_HARDRESET \
	(TRACTL_ARESET | TRACTL_CPHALT | TRACTL_BOOT | \
	 TRACTL_PEN)
#define TRACTL_DOWNLOAD \
	(TRACTL_CPHALT | TRACTL_BOOT | TRACTL_PEN)
#define TRACTL_CONTINUE \
	(TRACTL_BOOT | TRACTL_PEN)

/*
 * Ring speed defs for Campeche.  Campeche 2r2t uses the SIFACL reg for
 * setting the ring speed (Bits 14 and 15)
 * TI does bit ordering backwards.
 */
#define RING_SPEED_4   0x0003 /* Ring speed 4MB for Campeche */
#define RING_SPEED_16  0x0001 /* Ring speed 16MB for Campeche */
 
/**********************************************************************
 *
 *                Interrupt Register Definitions
 *
 * SYSINT being zero on a write will clear the SYSINT bit
 * coming from the adapter.
 *
 **********************************************************************/

#define WRITE_SIF(regptr,val)	*(regptr) = (val)
#define READ_SIF(regptr)	*(regptr)

#define WRITE_INT(val)          sif->intreg = val
#define READ_INT                sif->intreg

/*
 * TI Read/Write register definitions
 */
#define TRINT_ADAPTER	0x8000
#define TRINT_RESET	0x4000
#define TRINT_SSBCLR	0x2000
#define TRINT_EXECUTE	0x1000
#define TRINT_SCBREQ	0x0800
#define TRINT_RXCONT	0x0400
#define TRINT_RXVALID	0x0200
#define TRINT_TXVALID	0x0100
#define TRINT_SYSINT	0x0080

/*
 * TI Signals: common combinations of bits
 *
 * #define SIGNAL_SOFTRESET  0xFF00
 * #define SIGNAL_RXVALID    (TRINT_ADAPTER|TRINT_RXVALID|TRINT_SYSINT)
 * #define SIGNAL_TXVALID    (TRINT_ADAPTER|TRINT_TXVALID|TRINT_SYSINT)
 * #define CLR_SSBINT        (TRINT_ADAPTER|TRINT_SSBCLR)
 */

/*
 * TI Read only interrupt register meanings.
 */
#define TRINT_INIT	0x0040
#define TRINT_TEST	0x0020
#define TRINT_ERROR	0x0010
#define TRINT_CODEMASK	0x000f

/*
 * TI interrupt code (TRINT_CODEMASK) values
 *
 * #define TRINT_CHECK	   0x0
 * #define TRINT_RINGSTAT  0x4
 * #define TRINT_LLCSTAT   0x5
 * #define TRINT_SCBCLR	   0x6
 * #define TRINT_TIMER	   0x7
 * #define TRINT_CMDSTAT   0x8
 * #define TRINT_RXSTAT	   0xa
 * #define TRINT_TXSTAT	   0xc
 * #define TRINT_RXPEND	   0xe
 */



/*
 * Madge Read/Write register definitions
 */
#define MADGE_ADAPTER	   0x8000
#define MADGE_SSB_FREE	   0x4000
#define MADGE_SRB_CMD      0x2000
#define MADGE_ARB_FREE	   0x1000
#define MADGE_INIT_ONLY	   0x0800
#define MADGE_CLR_SSB_RESP 0x0400
#define MADGE_CLR_SRB_FREE 0x0200
#define MADGE_CLR_ARB_REQ  0x0100
#define MADGE_SYSINT	   0x0080

/*
 * MADGE Signals: common combinations of bits.  These all include
 * MADGE_SYSINT so that the interrupt bit from the madge code doesn't
 * accidentally get reset.
 */
#define SIGNAL_SOFTRESET 0xFF00
#define SIGNAL_SRB_CMD  (MADGE_ADAPTER | MADGE_SRB_CMD | MADGE_SYSINT)
#define SIGNAL_SRB_FREE (MADGE_ADAPTER | MADGE_CLR_SRB_FREE | MADGE_SYSINT)
#define SIGNAL_ARB_FREE (MADGE_ADAPTER | MADGE_ARB_FREE | MADGE_CLR_ARB_REQ | \
			 MADGE_SYSINT)
#define CLEAR_INTERRUPT 0x0000

/*
 * Madge Read only interrupt register meanings.
 */
#define MADGE_CHECK	0x0008
#define MADGE_SSB_RESP	0x0004
#define MADGE_SRB_FREE	0x0002
#define MADGE_ARB_REQ	0x0001

/*
 * Madge Masks.  In order to test for a condition, the interrupt but must
 * be set, and the corresponding free bit must be clear.
 */
#define MADGE_RESP_MASK (MADGE_CLR_SSB_RESP | MADGE_SSB_RESP)
#define MADGE_SRB_MASK  (MADGE_CLR_SRB_FREE | MADGE_SRB_FREE)
#define MADGE_ARB_MASK  (MADGE_CLR_ARB_REQ  | MADGE_ARB_REQ)
#define MADGE_CMD_MASK  (MADGE_CHECK | MADGE_SSB_RESP | \
			 MADGE_SRB_FREE | MADGE_ARB_REQ)

/***********************************************************************
 *
 *		      Initilization Block Values
 *
 * The intialization block must be written to the SIF by using DIO.
 * This is because this command is what supplies the SIF with the
 * locations of the command and status block.
 *
 * The structure below is never used in the system, it is simply to get a
 * clear picture of what is being written to the SIF.  The code pokes the
 * values directly into the DIO space instead of building the structure.
 *
 ***********************************************************************/

typedef struct STRUCT_SMT_LOAD_PARMS_HDR {
    ushort length;		/* of whole block, in bytes	*/
    ushort signature;		/* this module			*/
    ushort reserved;	        /* module to bind to		*/
    ushort version;		/* of the init block itself	*/
} madge_hdr;
    
typedef struct STRUCT_SMT_LOAD_PARMS {
    madge_hdr fastmac_hdr;
    ushort    debug_active;
    uchar     permanent_address[6];
    ushort    rxtx_buffer_size;
    ulong     PACKED(dma_buffer_addr);
    ushort    dma_buffers_size;
    ushort    max_buffer_ram;
    ushort    min_buffer_ram;
    ushort    sif_burst_size;
} smtmac_hdr;

typedef struct _init_blk_t {
    /*
     * Madge FASTMAC Specific Parameters
     */
    madge_hdr fastmac_hdr;
    ushort    feature_flags;
    ushort    int_flags;
    ushort    open_options;
    uchar     node_address[6];
    ulong     group_address;
    ulong     functional_address;	/* only if auto=true */
    ulong     rx_buf_physaddr;
    ushort    rx_buf_size;
    ushort    rx_buf_space;
    ulong     tx_buf_physaddr;
    ushort    tx_buf_size;
    ushort    tx_buf_space;
    ushort    max_frame_size;		/* for both rx and tx */
    ushort    size_rxdesc_queue;
    ushort    max_rx_dma;
    ushort    group_root_address;
    ushort    rx_bufs;                  /* Number of rx bufs */
    ushort    tx_bufs;                  /* Number of tx bufs */
    ushort    tms380_rx_slots;          /* Number of rx slots*/
    ushort    tx_slots;                 /* Number of tx slots*/
    ushort    tx_ahead;

} init_blk_t;

/*
 * Bits for INIT_OPTIONS:
 */
#define INIT_OPT_MUST_BE_SET		0x8000
#define INIT_OPT_PARITY_ENABLE		0x6000
#define INIT_OPT_BURST_SCB_SSB		0x1000
#define INIT_OPT_BURST_LIST		0x0800
#define INIT_OPT_BURST_LIST_STATUS	0x0400
#define INIT_OPT_BURST_RECEIVE_DATA	0x0200
#define INIT_OPT_BURST_TRANSMIT_DATA	0x0100
#define INIT_OPT_LLC_ENABLE		0x0040

#define INIT_OPT_VAL	(INIT_OPT_MUST_BE_SET | INIT_OPT_BURST_SCB_SSB | \
			 INIT_OPT_BURST_LIST | INIT_OPT_BURST_LIST_STATUS | \
			 INIT_OPT_BURST_RECEIVE_DATA | \
			 INIT_OPT_BURST_TRANSMIT_DATA)
#define INIT_VEC	0x0
#define INIT_RX_BURST	256
#define INIT_TX_BURST	256

#define INIT_RX_TX_BURST 0
/* 
 * note:
 * INIT_RX_BURST_BASS and INIT_TX_BURST_BASS  are used for both BASS 
 * and CANCUN.  Please don't change them until you understand the
 * reprocussions for BOTH platforms
 */
#define INIT_RX_BURST_BASS	8  /* DMA burst size for BASS */
#define INIT_TX_BURST_BASS	8  /* DMA burst size for BASS */
#define INIT_DMA_THRES	0x0505
#define INIT_UNUSED     0

#define INIT_MIN_BUFFER_RAM 2


/*
 * Madge version numbers. These are the data structure version numbers,
 * not the release version numbers.
 *
 *  v1.00 == first version
 *  v1.01 == sif_burst_size added
 *
 *  v3.00 == receive descriptors
 *  v?.?? == added CRC passing
 *
 *  Remember, the most significant bit must be set. This is a magic
 *  bit that is not documented and will not be documented by Madge.
 *
 */
#define VERNUM_FASTMAC_MODULE    0x8200
#define VERNUM_SMTMAC_LOAD_PARMS 0x0101

/*
 * These values must be changed if the size of the structures they are
 * associated with changes.  Using sizeof() a stucture that describes
 * hardware is a bad idea, as different compilers may give different 
 * answers. No, really.
 */
#define SMT_LEN_MAC              30 /* size of smtmac_hdr */
#define SMT_LEN_FASTMAC          62 /* size of init_blk_t */
#ifdef OLD_RELESE
#define SMT_LEN_FASTMAC          60 /* size of init_blk_t */
#endif

/*
 * Madge Feature Flags
 */
#define FF_MAGIC_BIT         0x8000   /* need this to run fastmac 4.28 */
#define FF_USE_NEW_INIT_BLK  0x0400
#define FF_ONE_INT_PER_RX    0x0200   /* madge hack to not interrupt if
                                       * only 1 packet in rx frame buffer */
#define FF_DELAY_RX          0x0100
#define FF_TRUNCATE_DMA      0x0080	/* don't dma in frame data */
#define FF_DISCARD_BEACON_TX 0x0040
#define FF_WATCHDOG_TIMER    0x0020
#define FF_RESERVED          0x0010	/* was FF_AUTO_PHYS */
#define FF_PASS_RX_CRC       0x0008
#define FF_SELL_BY_DATE      0x0004
#define FF_NOVELL            0x0002
#define FF_AUTO_OPEN         0x0001

#define FF_USED (FF_DISCARD_BEACON_TX | FF_PASS_RX_CRC | FF_MAGIC_BIT)

/*
 * Madge Interrupt Flags
 */
#define INT_RX_INT           0x10
#define INT_TX_LARGE_DMA     0x08
#define INT_RING_STATUS_ARB  0x04
#define INT_TIMER_TICK_ARB   0x02
#define INT_TX_BUF_EMPTY     0x01
#define INT_USED             (INT_RING_STATUS_ARB | INT_RX_INT)

/*
 * Magic values that should be in the SCB/SSB after initialization.
 * These are written there by the SIF and should be checked to make sure
 * that the SIF initialized properly.  With the Madge code, this is the
 * *only* use that is made of the SCB/SSB area.
 */
#define SCBINIT0	0x0
#define SCBINIT1	0xc1e2
#define SCBINIT2	0xd48b
#define SSBINIT0	0xffff
#define SSBINIT1	0xd1d7
#define SSBINIT2	0xc5d9
#define SSBINIT3	0xc3d4

/**********************************************************************
 *
 * These are the values for the Madge module signature word.  Each module
 * has a header file which defines the details of is initialization and
 * status structures.  The names of these header files are given here.
 *
 **********************************************************************/

#define SMT_MOD_SIGNATURE_ROOT    0
#define SMT_MOD_SIGNATURE_LLC     1
#define SMT_MOD_SIGNATURE_LLC_API 2
#define SMT_MOD_SIGNATURE_NB      3
#define SMT_MOD_SIGNATURE_IPX     4
#define SMT_MOD_SIGNATURE_FASTMAC 5
#define SMT_MOD_SIGNATURE_TEST    6
#define SMT_MOD_SIGNATURE_MAC     7
#define SMT_MOD_SIGNATURE_HOUSE   8

/*
 * The following are error codes used for initialization errors and also
 * for "user adapter check reason codes.
 */

#define SMTERR_WATCHDOG 0x0001
#define SMTERR_INTERNAL_ERR 0x0002
#define SMTERR_OUT_OF_MEMORY 0x0003
#define SMTERR_OUT_OF_CODE_MEMORY 0x0004
#define SMTERR_OUT_OF_BUFFER_MEMORY 0x0005
#define SMTERR_VERSION_PROBLEM 0x0006
#define SMTERR_TOO_MANY_MODULES 0x0007
#define SMTERR_BAD_DELAYED_INIT 0x0008
#define SMTERR_FAILED_RELOCATE 0x0009
#define SMTERR_BIND_FAILED 0x000A
#define SMTERR_ADAPTER_IS_OPEN 0x000B	
#define SMTERR_REG_DLC_USER_FAILED 0x000C
	

#define CRC_LEN         4

#define LONG_ALIGN(x) (((x) + sizeof(long) - 1) & ~(sizeof(long) - 1))

/*
 * Address match and CRC included bits.  These are contained in the
 * reserved field of the AC byte.
 */
#define AC_AM_NONE	0x0
#define AC_AM_ADDRMATCH	0x1
#define AC_AM_EXTMATCH	0x2
#define AC_AM_BOTH	0x3
#define AC_CRC_INCLUDED 0x8	/* madge defined */

/***********************************************************************
 *
 *		  TI System Control and Status Blocks
 *
 *			 unused by madge code
 *
 ***********************************************************************/

typedef struct _scbssb_t {
    ushort scb_cmd;
    union {
	uchar PACKED(*addr);
	struct {
	    ushort p0;
	    ushort p1;
	} param;
    } scb;
    ushort fill0;
    ushort ssb_cmd;
    ushort ssb_p0;
    ushort ssb_p1;
    ushort ssb_p2;
} scbssb_t;

/***********************************************************************
 *
 *		Madge System Request and Status Blocks
 *
 ***********************************************************************/

#define DIR_INTERRUPT              0x00
#define DIR_MODIFY_OPEN_PARMS	   0x01
#define DIR_RESTORE_OPEN_PARMS	   0x02
#define DIR_OPEN_ADAPTER	   0x03
#define DIR_CLOSE_ADAPTER	   0x04
#define DIR_SET_GROUP_ADDRESS	   0x06
#define DIR_SET_FUNCTIONAL_ADDRESS 0x07
#define DIR_READ_LOG		   0x08
#define DIR_SET_BRIDGE_PARMS	   0x09
#define DIR_CONFIG_BRIDGE	   0x0c

/*
 * The following structure is used for the header of all of the request
 * and status blocks (SRB, SSB, ARB and ASB).
 */


typedef struct struct_srb_hdr {
    uchar  function;
    uchar  correlator;
    uchar  return_code;
    uchar  reserved;
} srb_hdr;

/**********************************************************************
 *
 *           S R B   R E Q U E S T   S T R U C T U R E S
 *           -------------------------------------------
 *
 * The following structures are used to pass requests from the host to
 * the adapter card.  The function codes are the same as the equivalent
 * CCB function codes defined in LLCCON.CON.
 *
 **********************************************************************/

typedef struct struct_srb_config_bridge {
    srb_hdr hdr;
    uchar   reserved[4];
    ushort  xmit_ram_size;	
} srb_config_bridge;

typedef struct struct_srb_config_br_response {
    srb_hdr hdr;
    uchar   reserved[4];
    ushort  xmit_control_offset;
    ushort  srb_offset;
} srb_config_br_response;

/*********************************************************************
 *
 *           S S B   R E S P O N S E   S T R U C T U R E S
 *           ---------------------------------------------
 *
 * The following structures define the parameter areas for all of the
 * SSB commands which are used to pass responses from the adapter back
 * to the host.
 *
 **********************************************************************/

typedef struct struct_ssb_transmit {
    srb_hdr hdr;
    ushort  station_id;
    uchar   transmit_fs;
} struct_ssb_transmit;


/***********************************************************************
 *
 *			     Open Command
 *
 ***********************************************************************/

typedef struct struct_srb_open_adapter {
    srb_hdr hdr;
    uchar   reserved_1[2];		/* 4 */
    ushort  open_error;
    ushort  open_options;		/* valid */
    uchar   address[6];			/* valid */
    ulong   group_address;		/* valid */
    ulong   functional_address;		/* valid */
    ushort  number_of_rx_buffers;
    ushort  receive_buffer_length;
    ushort  transmit_buffer_length;
    uchar   number_of_tx_buffers;
    uchar   reserved_2;
    uchar   dlc_parms[10];
    uchar   product_id[18];		/* valid */
} srb_open_adapter;

#define SRB_OPEN_ADAPTER_SIZE	60	/* Size of srb_open_adapter struct */

#define OFFSET_OPEN_RESPONSE 2 
#define OFFSET_OPEN_ERROR    6

/*
 * Open option values
 */
#define OPN_WRAP	  0x8000
#define OPN_NOSOFT	  0x4000       /* 0x2000 for TI */
#define OPN_NOHARD	  0x2000       /* 0x4000 for TI */
#define OPN_PASS_ADPT_MAC 0x1000
#define OPN_PASS_ATTN_MAC 0x0800
#define OPN_PAD_ROUTE	  0x0400	/* unsupported */
#define OPN_FRAME_HOLD	  0x0200	/* unsupported */
#define OPN_CONTENDER	  0x0100
#define OPN_PASS_BEACON	  0x0080
#define OPN_RPL           0x0020        /* madge only - ??? */
#define OPN_EARLY_RELEASE 0x0010	/* unsupported */
#define OPN_COPY_ALL_MAC  0x0004	/* unsupported */
#define OPN_COPY_NON_MAC  0x0002	/* supported ??? - not in llccon.h */
#define OPN_PASS_FIRST	  0x0001	/* unsupported */

#define OPEN_OPT_VAL	  0x0100

/***********************************************************************
 *
 *			     Close Command
 *
 ***********************************************************************/

typedef srb_hdr srb_close_adapter;

#define OFFSET_CLOSE_RESPONSE 2 

/***********************************************************************
 *
 *		     Modify Open Options Command
 *
 ***********************************************************************/

typedef struct struct_srb_modify_open {
    srb_hdr  hdr;
    ushort  open_options;
} srb_modify_open;

#define OFFSET_MODIFY_RESPONSE 2 

/***********************************************************************
 *
 *			Set Bridge Params Command
 *
 ***********************************************************************/

/*
 * For IBM SRA functionality.
 */
typedef struct struct_srb_set_ibm_bridge_parms {
    srb_hdr hdr;
    uchar   reserved[2];
    ushort  this_ring;
    ushort  that_ring;	
    ushort  bridge_num;
    uchar   bridge_bits;
    uchar   single_route;
    uchar   tx_priority;
} srb_set_ibm_bridge_parms;

/*
 * For TI SRA functionality.
 */
typedef struct struct_srb_set_ti_bridge_parms {
    srb_hdr hdr;
    ushort  sngrtebcast:1;
    ushort  reserved:5;
    ushort  maxrif:6;
    ushort  partlen:4;
    ushort  this_ring;
    ushort  that_ring;	
    ushort  bridge_num;
} srb_set_ti_bridge_parms;

#define SRB_SET_TI_BRIDGE_PARMS_SIZE	12 	/* size of srb_set_ti_bridge_parms */


#define OFFSET_SETBRPARAM_RESPONSE 2 

/***********************************************************************
 *
 *                   Set Group/Functional Address Command
 *
 ***********************************************************************/
typedef struct struct_srb_set_multi_addr {
    srb_hdr hdr;
    ushort  reserved;
    ulong   PACKED(address);
} srb_set_multi_addr;

#define OFFSET_SET_ADDRESS_RESPONSE 2 

/***********************************************************************
 *
 *                   Read Error Log
 *
 ***********************************************************************/
typedef struct struct_srb_dir_read_log {
    srb_hdr hdr;                /* 0 */
    ushort  reserved;           /* 4 */
    uchar   line_errors;        /* 6 */
    uchar   internal_errors;	
    uchar   burst_errors;
    uchar   a_c_error;
    uchar   abort_delimiter;
    uchar   res1;
    uchar   lost_frame;
    uchar   rx_congestion;
    uchar   fr_copied_errors;
    uchar   freq_errors;
    uchar   token_errors;
    uchar   res2;
    uchar   res3;
    uchar   res4;
} srb_dir_read_log;

#define SRB_DIR_READ_LOG_SIZE	20	/* size of srb_dir_read_log struct */

#define OFFSET_READ_LOG_RESPONSE 2 
#define OFFSET_READ_LOG_COUNTERS 6

/**********************************************************************
 *
 *           A R B   R E Q U E S T   S T R U C T U R E S
 *           -------------------------------------------
 *
 * The following structures define the parameter areas for all of the
 * ARB commands which are used to pass requests from the adapter card to
 * the host.  Also the command function codes are defined here.
 **********************************************************************/

#define ARB_CMD_RX_DATA 	0x81	/* unimplemented  */
#define ARB_CMD_GET_TX_DATA	0x82	/* unimplemented  */
#define ARB_CMD_DLC_STATUS	0x83	/* unimplemented  */
#define ARB_CMD_RING_STATUS	0x84	/* Ring Status    */
#define ARB_CMD_RX_BRIDGE_DATA	0x85	/* unimplemented  */
#define ARB_CMD_TIMER_TICK	0x86    /* Periodic Timer */

typedef srb_hdr arb_hdr;

typedef struct struct_arb_ring_status {
    arb_hdr hdr;
    uchar   res[2];
    ushort  status;
} arb_ring_status;

typedef struct struct_arb_timer_tick {
    arb_hdr hdr;
    ushort  interval;
} arb_timer_tick;

#define OFFSET_RETURN_CODE	2
#define OFFSET_ARB_RING_STATUS	6

/**********************************************************************
 *
 *                         Madge Error Codes
 *
 **********************************************************************/
/*
 * These are all the errors that can be produced by the Direct and DLC
 * Interfaces.
 */


#define E_00_SUCCESS                0x00 /* Operation completed successfully */
#define E_01_INVALID_COMMAND	    0x01 /* Invalid command code             */
#define E_02_DUPLICATE_COMMAND	    0x02 /* Dup command - one outstanding    */
#define E_03_ADAPTER_OPEN	    0x03 /* Adapter open - should be closed  */
#define E_04_ADAPTER_CLOSED	    0x04 /* Adapter closed - should be open  */
#define E_05_PARMS_MISSING	    0x05 /* Required parameter(s) missing    */
#define E_06_INVALID_OPTIONS	    0x06 /* Option(s) invalid/incompatible   */
#define E_07_CMD_CANCELLED_FAIL	    0x07 /* Cancelled - unrecoverable fail   */
#define E_08_BAD_PRIORITY	    0x08 /* Unauthorised access priority     */
#define E_09_NOT_INITIALIZED	    0x09 /* Adapter not initialized          */
#define E_0A_CMD_CANCELLED_USER	    0x0A /* Cancelled by user                */
#define E_0B_CMD_CANCELLED_CLOSED   0x0B /* Cancelled bec closed in progress */
#define E_0C_CMD_OK_BUT_CLOSED	    0x0C /* Completed but adapter closed     */

#define E_10_OPEN_BUT_NO_NETBIOS    0x10 /* Adapter open - NETBIOS not op    */
#define E_11_TIMER_ERROR	    0x11 /* SET or CANCEL error              */
#define E_12_OUT_OF_WORK_AREA	    0x12 /* Available work area exceeded     */
#define E_13_INVALID_LOG_ID	    0x13 /* Invalid LOG.ID                   */
#define E_14_INVALID_SHARED_RAM	    0x14 /* Invalid shared RAM seg or size   */
#define E_15_LOST_LOG_DATA	    0x15 /* Lost log data - log reset        */
#define E_16_BUFFER_SIZE_TOO_BIG    0x16 /* Req buffer size exceeds pool     */
#define E_17_CMD_INVALID_NO_NETBIOS 0x17 /* Command invalid - NETBIOS not op */
#define E_18_INVALID_BUFFER_LENGTH  0x18 /* Invalid buffer length            */
#define E_19_NOT_ENOUGH_BUFFERS	    0x19 /* Inadequate buffers available     */
#define E_1A_USER_LENGTH_TOO_BIG    0x1A /* User length too large for buffer */
#define E_1B_CCB_PARM_TAB_INVALID   0x1B /* The CCB_PARM_TAB pointer invalid */
#define E_1C_CCB_PARM_PTR_INVALID   0x1C /* A ptr in CCB parm table invalid  */
#define E_1D_INVALID_ADAPTER_NUMBER 0x1D /* Invalid CCB_ADAPTER value */

#define E_20_LOST_RX_DATA_NO_BUFFERS   0x20 /* receive data lost - no buffs */
#define E_21_LOST_RX_DATA_TOO_FEW_BUFS 0x21 /* receive data lost - no space */
#define E_22_TRANSMIT_CHECK_FS         0x22 /* transmist error - check FS   */
#define E_23_TRANSMIT_STRIP	       0x23 /* transmit or strip error      */
#define E_24_UNAUTHORISED_MAC_FRAME    0x24 /* Unauthorised MAC frame       */
#define E_25_MAX_CMDS_EXCEEDED	       0x25 /* Maximum commands exceeded    */
#define E_26_BAD_CMD_CORRELATOR	       0x26 /* Unrecognised cmd correlator  */
#define E_27_LINK_NOT_SENDING_I_FRAMES 0x27 /* Link not trans I frames      */
#define E_28_INVALID_FRAME_LENGTH      0x28 /* Invalid transmit length      */

#define E_30_NO_RX_BUFFERS	      0x30 /* Inadequate receive buffers   */

#define E_32_INVALID_NODE_ADDRESS     0x32 /* Invalid NODE_ADDRESS           */
#define E_33_INVALID_RX_BUFFER_LENGTH 0x33 /* Invalid receive buffer length  */
#define E_34_INVALID_TX_BUFFER_LENGTH 0x34 /* Invalid transmit buffer length */

#define E_40_INVALID_STATION_ID	      0x40 /* Invalid STATION_ID             */
#define E_41_PROTOCOL_ERROR	      0x41 /* Protocol error                 */
#define E_42_PARAMETER_TOO_BIG	      0x42 /* Parameter exceeded maximum     */
#define E_43_INVALID_SAP_VALUE	      0x43 /* Invalid SAP_VALUE or in use    */
#define E_44_INVALID_ROUTE_LENGTH     0x44 /* Invalid RIF length             */
#define E_45_SAP_IS_NOT_A_GROUP	      0x45 /* Req member in non-exist group  */
#define E_46_RESOURCES_NOT_AVAILABLE  0x46 /* Resources not available        */
#define E_47_LINK_STATIONS_STILL_OPEN 0x47 /* SAP cannot close - links open  */
#define E_48_OTHER_SAPS_STILL_OPEN    0x48 /* G-SAP cant close - I-SAP open  */
#define E_49_GROUP_SAP_FULL	      0x49 /* Group SAP has max membership   */
#define E_4A_SEQUENCE_ERROR_BAD_CMD   0x4A /* Seq error: incompat cmd active */
#define E_4B_STATION_CLOSED_NO_ACK    0x4B /* Stn closed without remote ack  */
#define E_4C_SEQUENCE_ERROR_DLC_TO_GO 0x4C /* Seq error: DLC cmd outstand    */
#define E_4D_LINK_CONNECT_FAILURE     0x4D /* Unsuccessful link open attempt */
#define E_4E_MEMBER_SAP_NOT_FOUND     0x4E /* Member SAP not found           */
#define E_4F_INVALID_REMOTE_ADDRESS   0x4F /* Invalid remote address         */

#define E_80_ADAPTER_ALREADY_REG      0x80	
#define E_81_NETBIOS_ALREADY_REG      0x81	
#define E_82_NETBIOS_ALREADY_REG      0x82
#define E_83_INTARB_LOAD_ERROR	      0x83

#define E_FF_COMMAND_NOT_COMPLETE     0xFF /* Command not complete */

/**********************************************************************
 *
 *		     Madge compressed code format
 *
 **********************************************************************/

typedef struct magde_fw_ {
    ushort length;
    ushort type;
    uchar  data[0];
} madge_fw;

typedef struct magde_fw_code_ {
    ushort length;
    ushort type;
    ushort address;
    ushort count;
    uchar  data[0];
} madge_fw_code;

typedef struct magde_fw_execute_ {
    ushort length;
    ushort type;
    ushort address;
} madge_fw_execute;

typedef struct magde_fw_wait_ {
    ushort length;
    ushort type;
    ushort value;
    ushort timeout;
} madge_fw_wait;

typedef struct magde_fw_32data_ {
    ushort length;
    ushort type;
    ushort address_l;
    ushort address_h;
    ushort count;
    ushort data[0];
} madge_fw_32data;

typedef struct magde_fw_32block_ {
    ushort length;
    ushort type;
    ushort address_l;
    ushort address_h;
    ushort count;
    ushort fill;
} madge_fw_32block;

typedef struct magde_fw_phdis_ {
    ushort length;
    ushort type;
} madge_fw_phdis;

#define MADGE_DL_CODE    0x01
#define MADGE_DL_EXECUTE 0x02
#define MADGE_DL_WAIT    0x03
#define MADGE_DL_32DATA  0x04
#define MADGE_DL_32BLOCK 0x05
#define MADGE_DL_PHDIS   0x10
#define MADGE_DL_HEADER  0x12

/**********************************************************************
 *
 *                        Madge Status Block
 *
 **********************************************************************/
typedef struct struct_fastmac_status {
    madge_hdr hdr;
    ushort    adapter_open;	/* the order of fields in this       */
    ushort    open_error;	/* structure must be kept the same   */
    ushort    tx_adap_ptr;	/* because several programs read     */
    ushort    tx_host_ptr;	/* values out of it by using the     */
    ushort    tx_wraparound;	/* data increment register, and not  */
    ushort    rx_adap_ptr;	/* using the fields of the structure.*/
    ushort    rx_wraparound;
    ushort    rx_host_ptr;
    uchar     perm_addr[6];
    uchar     open_addr[6];
    ushort    tx_dma_count;
    ushort    timestamp;
    ushort    rx_internal_buffer_size;
    ushort    rx_total_buffers_avail;
    ushort    rx_buffers_in_use;
    ushort    rx_frames_lost;
    ushort    watchdog_timer;
    ushort    ring_status;
    ushort    tx_discarded;
    ushort    tms380_rx_slot_start;         /* ptr to 1st rx slot */
    ushort    tx_slot_start;                /* ptr to 1st tx slot */
    ushort    tx_frames_repaired;           /* # of underruns */
    ushort    rxdesc_host_ptr;
    ulong     rxdesc_queue;

} fastmac_status;

#define OFFSET_RX_SLOT_PTR           50
#define OFFSET_TX_SLOT_PTR           52
#define OFFSET_TX_FRAMES_REPAIRED    54
 
#define FM_RX_MSW_BUFFER_ADDR         4
#define FM_RX_STATUS                  8
#define FM_RX_NEXT_SLOT_OFFSET       10

#define FM_RX_FRAME_STATUS           0x5E00
#define FM_RX_DELIMITER_ERROR        0x40
#define FM_RX_IMPLICIT_ERROR         0x04
#define FM_RX_EXPLICIT_ERROR         0x06
#define FM_RX_DMA_OVERRUN            0x10
#define FM_RX_BUFFER_RUNOUT          0x30
 
#define FM_TX_SMALL_BUF_LEN           2
#define FM_TX_LARGE_BUF_LEN           4
#define FM_TX_SMALL_BUF_ADDR_MSW     10
#define FM_TX_SMALL_BUF_ADDR_LSW     12
#define FM_TX_NEXT_SLOT_OFFSET       14
#define FM_TX_LARGE_BUF_ADDR_MSW     16
#define FM_TX_LARGE_BUF_ADDR_LSW     18
 
#define TMS380_NO_SMALL_BUFFER_FOR_TX 0x8000
 
#define TMS380_RX_NEXT_PACKET    \
    /*                                                                   \
     * get next slot & see if next slot has                              \
     * traffic ready to be received.                                     \
     */                                                                  \
    sif_regs->adr = rx_slot->fm_rx_space;                                \
    sif_regs->data = 0;                                                  \
                                          				 \
    ds->tms380_rx_slot++;						 \
    if (ds->tms380_rx_slot == ds->rx_size)                               \
        ds->tms380_rx_slot = 0;                                          \
    rx_slot = &ds->rx_p[ds->tms380_rx_slot];                             \
                                                                         \
    sif_regs->adr = rx_slot->fm_rx_space;                                \
    size = sif_regs->data;                                               \
    sif_regs->adr = rx_slot->fm_rx_space + FM_RX_STATUS;                 \
    receive.status = sif_regs->data;					 
 
#define OFFSET_TX_ADAP_PTR  8
#define OFFSET_TX_HOST_PTR 10
#define OFFSET_TX_WRAP_PTR 12
#define OFFSET_RX_ADAP_PTR 14
#define OFFSET_RX_WRAP_PTR 16
#define OFFSET_RX_HOST_PTR 18
#define OFFSET_TX_DMA_COUNT 38
#define OFFSET_RX_INTERNAL_BUF_SIZ 36

/***************************************************************************/
/* End of code leveraged from David Hampton's sbe16/madge/if_madge.h */
/***************************************************************************/

/***************************************************************************/
/* Beginning of code leveraged from David Hampton's sbe16/madge/sca.h      */
/***************************************************************************/
/***********************************************************************
 *
 *		      Transmitting and Receiving
 *
 ***********************************************************************
 *
 * 			       TRANSMIT
 *
 * The system writes buffers directly into the linked list used by the TI
 * chipset.  It is responsible for setting the frame size length
 * properly, and for setting the CSTAT bit the indicate whether the FCS
 * and Source addresses should be passed or regenerated.
 *
 * When the system has written a buffer and set the cstat valid bit, it
 * sets the tx_munged semaphore and sends an interrupt to the SBE card.
 * The SBE notices the tx_munged bit and immediately sends a transmit
 * valid interrupt to the TI chipset ithout performing any processing on
 * the buffer.  When the TI finishes transmission, it sets the cstst
 * complete bit and interrupts the SBEs 68020 processor.  The 20 then
 * performs post processint on the buffer, and resets the flags to
 * indicate to the main system that the buffer may be used again.
 *
 * To determine who "owns" a transmit buffer at any time, you must look
 * at both the cstat and buf_flags fields in the buffer header.  The
 * combinations are:
 *      cstat VALID - TI chipset
 *      cstat CMPLT - SBE 68020
 *      buf_flags VALID - main system
 * These flags are used exclusively.  Any state where one of these flags
 * is not set is a transient state and the buffer should not be touched
 * by any processor except the one that is already in the process of
 * modifying the headers.
 *
 * The tx_kickme semaphore is used by the system when it wants to be
 * notified of a transmit completion.  The system uses this bit when it
 * has filled all the transmit buffers and needs to be notified that a
 * buffer has become available.  This allows the system to process other
 * things and only come back to this interface when it is guaranteed to
 * have a slot free for transmitting.
 *
 * The cstat flags are set by the main system to tell the chipset how to
 * perform the actuall transmission this packet, and whether to pass the
 * source address and/or the frame checksum.  There are only three values
 * that will be used by the system code.  They are:
 *
 * 	CSTAT_BASIC	normal   - generate source and fcs
 * 	CSTAT_BRIDGE    bridge   - pass source and fcs
 * 	CSTAT_TBRIDGE   srt      - pass source, generate fcs - from other media
 * 	CSTAT_EXPLORER  explorer - pass source, generate fcs
 *
 *
 *
 *
 * 			       RECEIVE
 *
 * Receive is very similar to transmit.  The TI chipset copies packets
 * directly into shared memory, and sends an interrupt to the SBE
 * processor.  The SBE then performs packet classification, and sets a
 * "processed" bit in the rtlist header.  It then sets the shared
 * rx_avail cell and sends an interrupt to the main system.  The main
 * system sees this flag and begins processing receive buffers.  A buffer
 * mey be processed any time that the cstat frame complete bit is set.
 * Each time a buffer is processed, the system sets the rx_munged cell in
 * the shared area, clears the SBE's processed bit, sets the valid flag
 * in the cstate header, and then sends an interrupt to the SBE card.
 * The system will continue processing until it finds a frame that has
 * the frame complete bit off, or it wraps the receive window.
 *
 * To determine who "owns" a receive buffer at any time, you must look
 * at both the cstat and buf_flags fields in the buffer header.  The
 * combinations are:
 *      cstat VALID - TI chipset
 *      cstat CMPLT - SBE 68020
 *      buf_flags VALID - main system
 *      buf_flags IGNORE - main system
 * These flags are used exclusively.  Any state where one of these flags
 * is not set is a transient state and the buffer should not be touched
 * by any processor except the one that is already in the process of
 * modifying the headers.
 *
 * When the system wraps the receive window, it sets the rx_window shared
 * cell, and sends an interrupt to the SBE card.  The SBE then rotates
 * the receive window to the next page of receive buffers.  If the first
 * buffer in the new window has already been processed, the SBE card
 * immediately sets the rx_avail cell and posts an interrupt back to the
 * main system.
 */


/*
 * The rtlist_t structure and flags are found in if_380.h since they
 * are copied from the TMS380 manual.  The extensions are listed here
 * since they are more oriented to the system level.
 */

#define CSTAT_BASIC	(CSTAT_VALID | CSTAT_SOF | CSTAT_EOF | CSTAT_FRMINT)
#define CSTAT_BRIDGE    (CSTAT_BASIC | CSTAT_PASS_CRC | CSTAT_PASS_SRC)
#define CSTAT_TBRIDGE   (CSTAT_BASIC | CSTAT_PASS_SRC)
#define CSTAT_EXPLORER  (CSTAT_BASIC | CSTAT_PASS_SRC)
#define CSTAT_RECEIVEOK	(CSTAT_VALID | CSTAT_FRMINT | CSTAT_PASS_CRC)

/***********************************************************************
 *
 * Statistics
 *
 * These are stored in a local array, and are only copied into the
 * shared array upon demand.  This prevents the SBE card from changing
 * counts while the main system is copying them from the local memory.
 *
 ***********************************************************************/

/*
 * Local counters
 */
typedef struct sbe_counters_t_ {
    ulong  sca_ints;		/* number of ints from sca */
    ulong  bad_state;		/* bad state detects */
    ulong  proto_err;		/* bad proto state detects */
    ulong  sca_ghosts;		/* number of ghost interrupts */
    ulong  bozo_ints;		/* number of bozo interrupts seen */
    ulong  last_bozo;		/* what the last one was */

    ulong  rx_sent;		/* number of Received packets given to Mom */
    ulong  rx_bytes_sent;	/* number of Received bytes given to Mom */
    ulong  rx_small;		/* num rcvd <= MAX_FRAME/2 */
    ulong  rx_large;		/* num rcvd > MAX_FRAME/2 */
    ulong  rx_giants;		/* frames larger than MAX_FRAME */
    ulong  rx_multiframe;	/* multi-frame receieves. not allowed. */
    ulong  rx_runts;		/* frames that were too small */
    ulong  rx_local;		/* given to mom because local acceptor */
    ulong  rx_bridge;		/* given to mom because of bridge acceptor */
    ulong  rx_promiscuous;	/* number we had to look at total */
    ulong  rx_rif_toolarge;	/* not local and rif limit exceeded */
    ulong  rx_srb_rnum_mismatch; /* last ring not ours */

    ulong  tx_sent;		/* number of Transmitted packets from Mom */
    ulong  tx_bytes_sent;	/* number of Transmitted bytes from Mom */
    ulong  tx_small;		/* num xmitted <= MAX_FRAME/2 */
    ulong  tx_large;		/* num xmitted > MAX_FRAME/2 */
    ulong  tx_toolarge;
    ulong  tx_toosmall;
    ulong  tx_nobodyhome;	/* address not recognized on ring */
    ulong  tx_congested;	/* address recognized but not copied */
    ulong  tx_bad_fs;		/* invalid frame status when stripped */
    ulong  tx_siferr;		/* sif complained in cstat field */
    ulong  bad_acbits;		/* number of bad acbits detected */
} sbe_counters_t;

/*
 * Data blocks read from the TMS380 local memory.
 */
typedef struct sbe_smt_stats_t_ {
    ushort addr[6];			/* chipset pointers */

    /*
     * Burned in address
     */
    uchar  bia[6];			/* burned in addr, should be zero */

    /*
     * Adapter software level
     */
    uchar  ti380_version[MAC_VERS_LEN];	/* ti chipset version in EBCDIC */

    /*
     * Current Adapter Addresses
     */
    uchar  mac[6];			/* current address from chipset */
    ulong  PACKED(group);		
    ulong  PACKED(functional);
    
    /*
     * Current Adapter Parameters
     */
    ulong  PACKED(physdrop);			/* physical drop number */
    uchar  una[6];			/* upstream */
    ulong  una_physdrop;		/* upstream phys drop */
    uchar  last_poll[6];
    uchar  rsrvd[2];
    ushort txmt_pri;			/* authorized access priority */
    ushort auth_class;			/* authorized functional classes */
    ushort last_attn;
    uchar  last_src[6];
    ushort last_beacon_type;
    ushort last_major_vector;
    ushort ring_status;
    ushort soft_err_timer;		/* 10ms unit TER */
    ushort ring_xfc_errors;
    ushort local_ring;			/* ring's idea of real ring num */
    ushort monitor_error_code;
    ushort beacon_txmt_type;
    ushort beacon_rcv_type;
    ushort frame_corr;
    uchar  beacon_una[6];
    uchar  rsrvd2[4];
    ulong  PACKED(beacon_una_physdrop);

    /*
     * Pointer to MAC buffer.  We never use this.
     */

    /*
     * Interface speed
     */
    ulong  PACKED(speed);			/* 0 = 4Mbps, ~0 = 16Mbps */

    ulong  PACKED(total_ram);

} sbe_smt_stats_t;
#define SMT_TI_ADDRESS 14
#define SMT_TI_PARMS   68

/*
 * Error Counters read from the TMS380 local memory.
 */
typedef struct madge_smt_errors_t_ {
    ulong  err_line;			/* ti380 logged errors */
    ulong  err_internal;
    ulong  err_burst;
    ulong  err_arifci;
    ulong  err_abort;

    ulong  err_lostframe;
    ulong  err_rcvcongestion;
    ulong  err_copy;
    ulong  err_freq;
    ulong  err_token;
} madge_smt_errors_t;


/*
 * Interface States
 */

#define	IF_DOWN			0
#define	IF_UP			1
#define	IF_CHECK		2
#define	IF_CLOSING		3
#define	IF_OPENING		4
#define	IF_AUTOFIX		5

#define	SMT_STATE_IDLE		0
#define	SMT_STATE_BIA		1
#define	SMT_STATE_SWLEV		2
#define	SMT_STATE_ADDR		3
#define	SMT_STATE_PARM		4
#define	SMT_STATE_SPEED		5
#define	SMT_STATE_RAM		6
#define	SMT_STATE_DONE		7
#define	SMT_STATE_SWLEV_DIRECT	8


#define MAX_BRIDGE_PROTOS 32

/*
 * Transparent Bridging definitions
 */
#define MAX_ADDRS 5

#define MK_BUF_PTR(win, ptr) (win + (((uint)ptr) & 0xffff))

#define SYSAREABASEADDR 0x10000
/* #define sys_area ((struct _sbe_sys_area_t *)SYSAREABASEADDR) */

/*
 * The Flag cell denotes binary information about the Monitor.
 * FLAG_OPERATE is set if the board is operational.  If it crashes
 * at some later date it will get cleared.  FLAG_HARD_FAIL denotes
 * that the Monitor has crashed and sca_fail is valid.
 */

#define FLAG_PRESENT		0x80
#define FLAG_OPERATE		0x40
#define FLAG_HARD_FAIL		0x20

/*
 * Misc.
 */
#define trbuginf if (tokenring_debug) buginf 
#define trebuginf if (tokenevent_debug || tokenring_debug) buginf 

/*
 * Porting Note : The data array below needs to be specified
 * as being 10 elements big to stop the R4k compiler from thinking
 * that timagic is in the .sdata section when -G 4 or -G 8 is specified
 */
typedef struct ucodehdr_ {
    ulong magic;
    uchar data[10];
} ucodehdr;

#define MADGE_UCODE_MAGIC	0x07301963	/* microcode magic number */

extern ucodehdr timagic;			/* in asm_les_madge.S */

#define MAX_PARA        32      /* Allow for up to 2 MB of RAM */
#define TEST_LOC        0x0800  /* Address in paragraph for presence test */
#define BYTES_PER_PARA  0x10000 /* 64k bytes per paragraph */
