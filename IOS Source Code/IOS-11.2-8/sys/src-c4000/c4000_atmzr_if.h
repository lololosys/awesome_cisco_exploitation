/* $Id: c4000_atmzr_if.h,v 3.3.20.1 1996/05/22 22:34:26 hxu Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_atmzr_if.h,v $
 *------------------------------------------------------------------
 * c4000_atmzr_if.h - Public SRAM Definition shared by the ATM NIM
 * system code and ATMizer firmware.
 *
 * November 1994, Walter Friedrich and Mohsen Mortazavi
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c4000_atmzr_if.h,v $
 * Revision 3.3.20.1  1996/05/22  22:34:26  hxu
 * CSCdi56054:  4500 uses 18K buffer pool even for a 9180 MTU
 * Branch: California_branch
 * Adding a private 9180-byte buffer pool to accommodate 9180 MTU request.
 *
 * Revision 3.3  1996/01/29  21:38:55  mohsen
 * CSCdi44192:  Add Adaptation layer 3/4 support for 4500 ATM NIM
 * Implement AAL3/4 (ITU-T I.363)
 *
 * Revision 3.2  1995/11/17  18:44:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  00:31:00  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  22:56:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*******************************************************************
                Public and Private SRAM addresses
 *******************************************************************/
#ifdef ATMNIM
#define ADRSPC_PUB_SRAM         0xA0C00000  /* start of 2 port SRAM */
#define ADRSPC_PRIV_SRAM        0xA0C80000  /* start of 1 port SRAM */
#else
#define ADRSPC_PUB_SRAM         0xA0810000  /* start of 2 port SRAM */
#define ADRSPC_PRIV_SRAM        0xA0D80000  /* start of 1 port SRAM */
#endif /* ATMNIM */


#ifndef ASMINCLUDE 


/*******************************************************************
                Global Definitions
 *******************************************************************/
#define MAX_ATMZR_VPXVC         8192            /* #VP*#VC = VpiVci tbl size */
#define MAX_ATMZR_VCIDS         1024            /* Max # actv VCs (Id=0 resv) */
#ifdef ATMNIM
#define MAX_ATMZR_AAL34_VCS        4            /* Max # of AAL34 VCs */
#else
#define MAX_ATMZR_AAL34_VCS        1            /* Max # of AAL34 VCs */
#endif
#define MAX_ATMZR_MIDS          (MAX_ATMZR_AAL34_VCS * 4)
#define ATMZR_RATEQ_NO             4            /* # ATMizer rate queues */
 


/*******************************************************************
                Basic Data Types
 *******************************************************************/
 
typedef volatile unsigned char          vchar;
typedef volatile unsigned short         vshort;
typedef volatile unsigned long          vlong;


/*******************************************************************
                Global Data Types
 *******************************************************************/
 
typedef struct  atmhdr_t_ {
    ulong               gfc             :4;     /* GFC */
    ulong               vpi             :8;     /* VPI */
    ulong               vci             :16;    /* VCI */
    ulong               pt              :3;     /* Payload Type */
    ulong               clp             :1;     /* Cell Loss Priority */
} atmhdr_t;
typedef vshort                          indx_t;


/*******************************************************************
                Public SRAM Definitions
 *******************************************************************/

/*
 * Receive free queues
 */
typedef enum {
    SMALL_FREEQ,
    MID_FREEQ,
    BIG_FREEQ,
    LARGE_FREEQ,
    SEMI_HUGE_FREEQ,
    HUGE_FREEQ,
    NUM_FREEQ
} freeq_t;
#define	SMALL_FREEQ_MTU		64		/* Supports raw cells only */
#define MID_FREEQ_MTU		500		/* */
#define BIG_FREEQ_MTU		1500		/* Supports Ethernet packets */
#define LARGE_FREEQ_MTU		4500		/* Supports FDDI packets */
#define SEMI_HUGE_FREEQ_MTU	9180		/* Supports SMDS pkts */
#define HUGE_FREEQ_MTU		18000		/* Supports Token Ring pkts */
#define ATMZR_BAD_POOL		NUM_FREEQ	/* Bad pool ID error */

/*
 * Queue sizes
 */
#define	CMDRSPQSZ		128		/* # Cmd/Rsp mailbox entries */
#define	PKTDESCRTSZ		128		/* # PktD <= INDXQSZ (0=unus) */
#define	INDXQSZ			128		/* Index queue size = 2**N */
#define	CMDRSPDATASZ		64		/* # Cmd/Rsp data sz (bytes) */
#define	EVENTQSZ		64		/* # Event Q entries */
#define	MAX_PURAMSZ		65536		/* Max Public SRAM size */
#define	RXPKTDLIMIT		((PKTDESCRTSZ*3)/4) /* Max # PktD for recv */
#define	TXPKTDLIMIT		(PKTDESCRTSZ/4)	/* Max # PktD for transmit */

/*
 * Packet Descriptor.
 * note: "stat" and "optstat_vc" fields are defined further down
 *       in this file in order to be shared by C/assmebly code.
 */
typedef union pktd_t_ {
    struct {
	vchar		*start;			/* Datagram start addr */
	vlong		res[7];			/* Reserved fields */
    } free;
    struct {
	vchar		*dma;			/* DMA pointer */
	vshort		mtu;			/* Temporary MTU storage */
	vshort		size;			/* Datagram size field */
	vlong		crc32;			/* CRC32 */
	vshort		pktid;			/* Packet ID */
	vshort		congest;                /* congestion flag */
	union pktd_t_	*next;			/* Next packet descr in list */
	union pktd_t_	*prev;			/* Prev packet descr in list */
	vlong		reastout;		/* Reassembly time-out stamp */
	vlong		res1;			/* Reserved field */
    } rxng;
    struct {
	vlong		res1;			/* Reserved field */
	vshort		stat_vc;		/* Rx status/VCID field */
	vshort		size;			/* Datagram size field */
        vlong           res2;                   /* reserved field */
        vshort          res3;                   /* reserved field */
	vshort		congest;                /* congestion flag */
	vlong		res[4];			/* Reserved fields */
    } rxrdy;
    struct {
	vchar		*start;			/* Datagram start addr */
	vshort		opt_vc;			/* Tx option/stat/VCID field */
	vshort		size;			/* Datagram size field */
	vlong		res[6];			/* Reserved fields */
    } txrdy;
    struct {
	vchar		*dma;			/* DMA pointer */
	vshort		opt_mid;		/* Tx option/MID field */
	vshort		left;			/* Bytes left to transmit */
	vlong		crc32;			/* CRC32 */
	union pktd_t_	*next;			/* Next packet descr in list */
	vlong		res[4];			/* Reserved fields */
    } txng;
    struct {
	vchar		*dma;			/* DMA pointer */
	vshort		stat_mid;		/* Tx stat/MID field */
	vshort		size;			/* Datagram size field */
	vlong		res[6];			/* Reserved fields */
    } txdone;
} pktd_t;
#define	free_start		free.start
#define	rxrdy_stat_vc		rxrdy.stat_vc
#define	rxrdy_size		rxrdy.size
#define	txrdy_start		txrdy.start
#define	txrdy_opt_vc    	txrdy.opt_vc
#define	txrdy_size		txrdy.size
#define	txng_next		txng.next
#define	txdone_stat_mid		txdone.stat_mid

/*
 * Command/Response Queue.
 * The command/Response queue is a circular queue where each entry
 * contains a command mailbox, a response mailbox and a common data
 * area.
 */
typedef struct cmd_t_ {
    vchar		cmd;			/* Command code */
    vchar		res;			/* Reserved field */
    vshort		param;			/* Parameter */
} cmd_t;

typedef struct rsp_t_ {
    vchar		rsp;			/* Response code */
    vchar		stat;			/* Response status */
    vshort		param;			/* Parameter */
} rsp_t;

typedef struct cmdrsp_t_ {
    cmd_t		cmd;			/* Command mailbox */
    rsp_t		rsp;			/* Response mailbox */
    vchar		data[CMDRSPDATASZ];	/* Data area */
} cmdrsp_t;
#define	CMDRSP_SIZE		sizeof (cmdrsp_t)

/* Command/Response_Status Codes */
#define	CMD_SYNC		0x80		/* Synchronous command */
#define	CMD_ASYNC		0x00		/* Asynchronous command */

#define	CMD_OBJ_VC		0x08		/* Object = VC */
#define	CMD_OBJ_PRMEM		0x10		/* Object = Private memory */
#define	CMD_OBJ_CFG		0x18		/* Object = Configuration */
#define	CMD_OBJ_RATEQ		0x20		/* Object = Rate Queue */
#define	CMD_OBJ_MSK		0x78		/* Object field mask */

#define	CMD_OPR_0		0x00		/* Operation = 0 */
#define	CMD_OPR_1		0x01		/* Operation = 1 */
#define	CMD_OPR_2		0x02		/* Operation = 2 */
#define	CMD_OPR_3		0x03		/* Operation = 3 */
#define	CMD_OPR_4		0x04		/* Operation = 4 */
#define	CMD_OPR_5		0x05		/* Operation = 5 */
#define	CMD_OPR_6		0x06		/* Operation = 6 */
#define	CMD_OPR_7		0x07		/* Operation = 7 */
#define	CMD_OPR_MSK		0x07		/* Operation field mask */

#define	CMD_VC_OPEN		(CMD_OBJ_VC | CMD_OPR_0)
#define	CMD_VC_CLOSE		(CMD_OBJ_VC | CMD_OPR_1)
#define	CMD_VC_READ		(CMD_OBJ_VC | CMD_OPR_2)
#define	CMD_VC_WRITE		(CMD_OBJ_VC | CMD_OPR_3)
#define	CMD_PRMEM_RDBLK		(CMD_OBJ_PRMEM | CMD_OPR_0)
#define	CMD_PRMEM_WRBLK		(CMD_OBJ_PRMEM | CMD_OPR_1)
#define	CMD_PRMEM_FILLBLK	(CMD_OBJ_PRMEM | CMD_OPR_2)
#define	CMD_CFG_SET		(CMD_OBJ_CFG | CMD_OPR_0)
#define	CMD_RATEQ_CREATE	(CMD_OBJ_RATEQ | CMD_OPR_0)
#define	CMD_RATEQ_DESTROY	(CMD_OBJ_RATEQ | CMD_OPR_1)
#define	CMD_RATEQ_READ		(CMD_OBJ_RATEQ | CMD_OPR_2)
#define	CMD_MSK			0x7F		/* Command Obj/Opr field mask */

#define	RSP_STAT_OK		0x00		/* Response status = OK */
#define	RSP_STAT_FULL		0x01		/* Cmd/Rsp queue full */
#define	RSP_STAT_TOOBIG		0x02		/* Cmd/Rsp data blk too big */
#define	RSP_STAT_TOUT		0x03		/* Response time-out */
#define	RSP_STAT_CMDINVAL	0x04		/* Invalid command code */
#define RSP_STAT_NOTRDY         0x0F            /* NIM not ready */
#define	RSP_STAT_VCIDINVAL	0x10		/* Invalid VC index */
#define	RSP_STAT_VCIDINUSE	0x11		/* VC index in use */
#define	RSP_STAT_VCIDUNUSED	0x12		/* Unused VC index */
#define RSP_STAT_RATEQINACTIVE  0x13            /* Tried to use inactv RateQ */
#define RSP_STAT_BUFQIDINVAL    0x14            /* Tried to use invalid BufQ */
#define RSP_STAT_NOAAL34SLOT    0x15            /* no more AAL34 VC slots */
#define	RSP_STAT_ADDRINVAL	0x20		/* Invalid 2nd port mem addr */
#define	RSP_STAT_CFGINVAL	0x30		/* Invalid configuration data */
#define	RSP_STAT_RATEQINVAL	0x40		/* Invalid Rate Queue index */
#define	RSP_STAT_RATEQINUSE	0x41		/* Rate Queue index in use */
#define	RSP_STAT_MSK		0xFF		/* Response status field mask */

/*
 * Commands/Responses parameter data types.
 * This structure types define the data passed in the command parameter
 * field.
 */

/* Rate Queue command data type */
typedef struct rqdata_t_ {
    vchar		rqid		:2;	/* Rate queue index */
    vchar		rqclk		:1;	/* PRU clk (0=CLK,1=PRU_CLK) */
    vchar		unused		:1;	/* Unused field */
    vshort		prucnt		:12;	/* PRU initial count */
} rqdata_t;

/*
 * Commands/Responses parameters data area structure types.
 * This structure types define the data structures used to format the
 * information passed in the Cmd/Rsp data area. There is one type per
 * command/response that uses the data area.
 */

/* VC commands data type */
typedef struct vcdata_t_ {
    struct vcdata_t_	*vc_next;		/* Next VC in doubl-link list */
    struct vcdata_t_	*vc_prev;		/* Prev VC in doubl-link list */
    ushort		vcid;			/* VC index */
    ushort		encaps;			/* VC encapsulation */
    union {
        struct {
            ushort aal			:3;	/* AAL type (1=AAL5,2=AAL34) */
            ushort rateq		:2;	/* Rate queue index */
            ushort bufq			:3;	/* Buf q (0=Small..4=Huge) */
            ushort options		:8;	/* Options undefined */
        } misc_fields;
        ushort misc;
    } vc_info;
    ushort		mtu		:16;	/* MTU size (bytes) */
    ushort		idlew;			/* Idle transmit window */
    ushort		burstw;			/* Burst transmit window */
    atmhdr_t		atmhdr;			/* ATM cell header (B0-3) */
    union {
        struct {
            ulong unused1		:2;	/* Unused field */
            ulong midhi			:10;	/* MID high limit */
            ulong midlo			:10;	/* MID low limit */
            ulong midnxt		:10;	/* Next MID field value */
        } mid_fields;
        ulong mid;
    } mid_info;
    ulong		idletm;			/* Next Idle wind start time */
    ulong		bursttm;		/* Next Burst wind start time */
    ulong		unused2[2];		/* Unused fields */
    ushort		aal34heccrcer;		/* HCS or CRC errors */
    ushort		aal34payloadlther;	/* Payload length errors */
    ushort		aal34seqnber;		/* Sequence number errors */
    ushort		aal34midactver;		/* MID currently active er */
    ushort		aal34bommider;		/* BOM or SSM MID errors */
    ushort		aal34eommider;		/* EOM MID errors */
    pktd_t		*first_pktptr;		/* VC's first packet descr */
    pktd_t		*last_pktptr;		/* VC's last packet descr */
} vcdata_t;
#define vc_misc_info      vc_info.misc
#define vc_aal            vc_info.misc_fields.aal
#define vc_rateq          vc_info.misc_fields.rateq
#define vc_bufq           vc_info.misc_fields.bufq
#define vc_options        vc_info.misc_fields.options
 
#define vc_mid_info       mid_info.mid
#define vc_midhi          mid_info.mid_fields.midhi
#define vc_midlo          mid_info.mid_fields.midlo
#define vc_midnxt         mid_info.mid_fields.midnxt

/* Private Memory command data type */ 
typedef struct prmemdata_t_ {
    ulong		*start;			/* ATmizer mem blk start addr */
    ulong               *end;                   /* ATmizer mem blk end addr */
    ulong		data[14];		/* Memory data */
} prmemdata_t;

/* Initial Configuration command data type */ 
typedef struct cfgdata_t_ {
    vshort		small_rxq_off;		/* Small rx free buf q offset */
    vshort		mid_rxq_off;		/* Mid rx free buf q offset */
    vshort		big_rxq_off;		/* Big rx free buf q offset */
    vshort		large_rxq_off;		/* Large rx free buf q offset */
    vshort		semi_huge_rxq_off;	/* Semi_huge rx free buf q offset */
    vshort		huge_rxq_off;		/* Huge rx free buf q offset */
    vshort		rxrdyq_off;		/* Rx rdy queue offset */
    vshort		txrdyq_off;		/* Tx rdy queue offset */
    vshort		txdoneq_off;		/* Tx done queue offset */
    vshort		indexq_size;		/* Index queue size */
    vshort		cmdrspq_off;		/* Cmd/Rsp queue offset */
    vshort		cmdrspq_size;		/* Cmd/Rsp queue size */
    vshort		excq_off;		/* Exception queue offset */
    vshort		excq_size;		/* Exception queue size */
    vshort		pktdt_off;		/* Packet descr table offset */
    vshort		vcpervp;		/* Number of VCs per VP */
} cfgdata_t;

/* Public SRAM fixed structures memory map */
typedef struct pusram_t_ {
    vchar               reserved[0x200];
    indx_t		small_rxfreeQ[INDXQSZ];
    indx_t		mid_rxfreeQ[INDXQSZ];
    indx_t		big_rxfreeQ[INDXQSZ];
    indx_t		large_rxfreeQ[INDXQSZ];
    indx_t		semi_huge_rxfreeQ[INDXQSZ];
    indx_t		huge_rxfreeQ[INDXQSZ];
    indx_t		rxrdyQ[INDXQSZ];
    indx_t		txrdyQ[INDXQSZ];
    indx_t		txdoneQ[INDXQSZ];
    cmdrsp_t		cmdrspQ[CMDRSPQSZ];
    vshort		eventQ[EVENTQSZ];
    pktd_t		pktdT[PKTDESCRTSZ];
    ulong               debug_area[1024];
} pusram_t;

/* Public SRAM non-fixed structures (intr ack) memory map */
typedef struct intr_t_ {
    vshort              rsp_rdy;           /* rsp ready intr, apu->cpu */
    vshort              event;             /* event intr, apu->cpu  */
    vshort              tx_rdy;            /* transmit ready intr, cpu->apu */
    vshort              cmd_rdy;           /* command ready intr, cpu->apu */
} intr_t;


/*******************************************************************
                Macros
 *******************************************************************/

/*
 * Index queue macros
 */
#define PRODINDEX(ptr,ndx)                                              \
        *(ptr) = (ndx);                                                 \
        (ulong)(ptr) = (((ulong)ptr) & ~(INDXQSZ*sizeof(indx_t)-1)) |	\
                       ((ulong)(++(ptr)) & (INDXQSZ*sizeof(indx_t)-1));
 
#define CONSINDEX(ptr,ndx)                                              \
        (ndx) = *(ptr);                                                 \
        (ulong)(ptr) = (((ulong)ptr) & ~(INDXQSZ*sizeof(indx_t)-1)) |	\
                       ((ulong)(++(ptr)) & (INDXQSZ*sizeof(indx_t)-1));
 
#define CLRINDEX(ptr)                                                   \
        *(ptr) = 0;                                                     \
        (ulong)(ptr) = (((ulong)ptr) & ~(INDXQSZ*sizeof(indx_t)-1)) |	\
                       ((ulong)(++(ptr)) & (INDXQSZ*sizeof(indx_t)-1));
 

#else /* ASMINCLUDE */


/*******************************************************************
                Global Definitions
 *******************************************************************/
/*
 * Packet Descriptor table Receive entry (public SRAM)
 */
#define PKT_RX_DMAPTR           0x00    /* dma pointer */
#define PKT_RX_MTU              0x04    /* mtu size */
#define PKT_RX_STAT             0x04    /* status (reused) */
#define PKT_RX_LEN              0x06    /* current packet length */
#define PKT_RX_CRC32            0x08    /* crc32 value partial,final */
#define PKT_RX_ID               0x0C    /* packet id, not currently used */
#define PKT_RX_CONGST           0x0E    /* congestion flag, 1 = congested */
#define PKT_RX_NEXT             0x10    /* next packet in rx timeout Q */
#define PKT_RX_PREV             0x14    /* prev packet in rx timeout Q */
#define PKT_RX_REAS_TIME        0x18    /* reassemly timeout time stamp */
#define PKT_RX_UNUSED           0x1C    /* unused */
 
/*
 * Packet Descriptor table Transmit entry (public SRAM)
 */
#define PKT_TX_DMAPTR           0x00    /* dma pointer */
#define PKT_TX_MISC             0x04    /* misc fields, see below */
#define PKT_TX_PKTLEN           0x06    /* packet length */
#define PKT_TX_CRC32            0x08    /* CRC32 value partial/final */
#define PKT_TX_NEXTPTR          0x0C    /* next packet to transmit on the vc */
 
/* MISC field */
#define PKT_TX_PTI_MASK    0xE0000000   /* PTI mask */
#define PKT_TX_EOM         0x10000000   /* EOM bit setting in ATM header */
#define PKT_TX_OAM         0x08000000   /* OAM bit setting in ATM header */
#define PKT_TX_1ST_CELL    0x04000000   /* 1st cell (new packet) flag */
#define PKT_TX_34_STATE    0x08000000   /* aal34 MID segmentation state */
#define PKT_TX_VCID_MASK   0x03FF0000   /* vc desc index, ready for transmit */
#define PKT_TX_LEN_MASK    0x0000FFFF   /* packet len, decrements in transmit*/
#define PKT_TX_MID_MASK    0x03FF0000   /* aal34 mid */
#define PKT_TX_STAT_MASK   0xFC000000   /* tx status on completion */

/*
 * For AAL34, the upper 4 bits of STAT field is used for 
 * storing the MID sequence number while packet is being
 * transmitted. To reduce the number of instructions used
 * to manipulate this field and the MID number field, all
 * are defined as 16 bit.  
 */
#define PKT_TX_34_SN_MASK      0xF000   /* aal34 next seq. no. on this MID */
#define PKT_TX_34_SN_SHIFT         12   /* to shift SN left into position */
#define PKT_TX_34_MID_MASK     0x03FF   /* mask to get the MID number */

#endif /* ASMINCLUDE */

/*
 * Packet Descriptor's transmit options field (optstat_vc)
 */
#define	TXOPT_MSK		0xFC00		/* Tx options field mask */
#define	TXOPT_PTI_USRNOCONG0	0x0000		/* User data, no cong, SDU 0 */
#define	TXOPT_PTI_USRNOCONG1	0x2000		/* User data, no cong, SDU 1 */
#define	TXOPT_PTI_USRCONG0	0x4000		/* User data, cong, SDU 0 */
#define	TXOPT_PTI_USRCONG1	0x6000		/* User data, cong, SDU 1 */
#define	TXOPT_PTI_SEGMENTF5	0x8000		/* Segment OAM F5 flow */
#define	TXOPT_PTI_END2ENDF5	0xA000		/* End-to-end OAM F5 flow */
#define	TXOPT_PTI_MSK		0xE000		/* PTI field mask */
#define	TXOPT_EOMEN		0x1000		/* EOM bit setting enable */
#define	TXOPT_OAM		0x0800		/* OAM cell */

/*
 * Packet's Descriptor's receive status field codes (stat)
 */
#define	RXSTAT_OK		0x0000		/* No error = OK */
#define	RXSTAT_MSK		0xFC00		/* Receive status field mask */
#define RXSTAT_SHIFT            10              /* Receive status field shift */
#define	RXSTAT_CRC		0x0400		/* CRC error */
#define	RXSTAT_LENGTH		0x0800		/* Length error */
#define	RXSTAT_GIANT		0x0C00		/* Oversized SDU error */
#define	RXSTAT_REASTOUT		0x1000		/* Reassembly time-out error */
#define	RXSTAT_AAL34_MID	0x2000		/* AAL3/4 incorrect MID error */
#define	RXSTAT_AAL34_BUFOVFL	0x2400		/* AAL3/4 buffer ovfl error */
#define	RXSTAT_AAL34_SEQNUM	0x2800		/* AAL3/4 incorrect seq nb er */
#define	RXSTAT_AAL34_P_ABORT	0x2C00		/* AAL3/4 CPCS abort msg err */
#define	RXSTAT_AAL34_U_ABORT	0x3000		/* AAL3/4 user abort msg err */
#define	RXSTAT_AAL5_FORMAT	0x4000		/* AAL5 invalid format error */
#define	RXSTAT_F5_SEGMENT       0x8000  	/* OAM F5 segment */
#define	RXSTAT_F5_END2END       0x8400  	/* OAM F5 end-to-end */
#define	NUM_RXERR		64		/* Max number of rx errors */
#define	TXSTAT_OK		0x0000		/* No error = OK */
#define	TXSTAT_ABORTED		0x0400		/* Transmission aborted */
#define	TXSTAT_MSK		0xFC00		/* Transmit status field mask */
#define TXSTAT_SHIFT            10              /* Transmit status field shft */

/* Other fields */
#define RXVCID_MSK              0x03FF          /* Receive vcid field mask */

/*
 * VC descriptor's vc_aal field 
 */
#define VC_NO_AAL                  0    /* no aal type assigned */
#define VC_AAL5                    1    /* aal5 type */
#define VC_AAL34                   2    /* aal34 type */

/*
 * Public SRAM fixed structures memory map
 */
#define PUB_RESERVED            0x0
#define PUB_SMALL_RXFREE        0x200
#define PUB_MID_RXFREE          0x300
#define PUB_BIG_RXFREE          0x400
#define PUB_LARGE_RXFREE        0x500
#define PUB_SEMI_HUGE_RXFREE    0x600
#define PUB_HUGE_RXFREE         0x700
#define PUB_RXRDY_Q             0x800
#define PUB_TXRDY_Q             0x900
#define PUB_TXDONE_Q            0xA00
#define PUB_CMDRSP_Q            0xB00
#define PUB_EVENT_Q             0x2F00
#define PUB_PKTDSC_TBL          0x2F80
#define PUB_DEBUG_AREA          0x3F80

#define PUB_RXFREE_Q_SZ         0x100
#define PUB_RXRDY_Q_SZ          0x100
#define PUB_TXRDY_Q_SZ          0x100
#define PUB_TXDONE_Q_SZ         0x100
#define PUB_CMDRSP_Q_SZ         0x100
#define PUB_EVENT_Q_SZ          0x100

/*
 * Public SRAM non-fixed structures (intr ack) memory map
 */
#define PUB_RSPRDY_INTR         0x00
#define PUB_EVENT_INTR          0x02
#define PUB_TXRDY_INTR          0x04
#define PUB_CMDRDY_INTR         0x06

#ifdef EVAL
/*
 * LSI's Eval board GPINT_TST and GPINT_AUTO defines.
 */
#define EVAL_SETCMD_ADDR        0xA0E0003B      /* Set Command address */
#define EVAL_RSTCMD_ADDR        0xA0E0003F      /* Reset Command address */
#define EVAL_INTR_CLEAR_ALL     0x3F            /* to clear all output pins */ 
#define EVAL_INTR_AUTO          0x02            /* Assert AUTO interrupt */
#define EVAL_INTR_TST           0x01            /* Assert TST interrupt */
#define EVAL_CLR_AUTO           0x04            /* Deassert AUTO interrupt */
#define EVAL_CLR_TST            0x03            /* Deassert TST interrupt */
#endif /* EVAL */

/*
 * Event Queue codes 
 */
#define EVENTQ_INIT_DONE          0x01
#define EVENTQ_BADMEM_PUBSRAM     0x21
#define EVENTQ_BADMEM_PRVSRAM     0x22
#define EVENTQ_BADMEM_IRAM        0x23
#define EVENTQ_BADMEM_VCR         0x24
#define EVENTQ_RQ_OVERLOAD        0x41
#define EVENTQ_BADCHK_PUBSRAM     0x51
#define EVENTQ_BADCHK_PRVSRAM     0x52
#define EVENTQ_BADCHK_IRAM        0x53
#define EVENTQ_EXCEPTION          0x61


