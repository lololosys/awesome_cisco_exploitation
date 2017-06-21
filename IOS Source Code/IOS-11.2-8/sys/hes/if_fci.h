/* $Id: if_fci.h,v 3.9.12.9 1996/08/29 02:23:50 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_fci.h,v $
 *------------------------------------------------------------------
 * if_fci.h -- FAST Communications Interface Hardware Definitions.
 *
 * 1-August-1989, Robert Michaels
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_fci.h,v $
 * Revision 3.9.12.9  1996/08/29  02:23:50  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.9.12.8  1996/08/22  06:01:02  pitargue
 * CSCdi66791:  reserve fci type and idb type for Yosemite
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/08/20  00:34:55  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.9.12.6  1996/08/15  18:06:11  ptran
 * Add ESA's idb to VIP and RSP hwidb/swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/07/26  01:09:52  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.9.12.4  1996/07/08  22:25:23  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/05/22  13:51:08  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/03/21  22:40:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.12.1  1996/03/18  19:41:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/03/18  23:19:13  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/02/08  07:21:48  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.3  1996/01/15  10:19:29  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.2  1995/12/23  04:39:57  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.3.2.1  1995/12/11  18:47:28  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.9  1996/02/15  00:29:01  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Added FCI_TYPE_RSP() macro to allow code to determine if a given
 * controller type is any of the RSP family.
 *
 * Revision 3.8  1996/02/13  20:25:15  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.7.4.1  1996/01/24  21:46:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/01/15  18:04:11  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.6  1995/12/17  06:02:52  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.5  1995/12/04  20:15:55  schaefer
 * CSCdi44946: add enums for new hardware
 * add controller types for rsp/4, rsp/7000, ci/7000
 *
 * Revision 3.4  1995/11/26  08:10:50  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              Hooks for SP/SSP routing of vLAN encapsulated datagrams.
 *
 * Revision 3.3  1995/11/17  09:17:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:00:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/31  22:38:41  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.6  1995/08/09  00:19:46  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.5  1995/07/31  21:28:35  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.4  1995/07/28  01:31:12  rharagan
 * CSCdi37725:  G.703-E1 LOF, AIS, LOS, RAI not reported on c7000
 * Shovel G.703 error indicators up to the RP.
 *
 * Revision 2.3  1995/07/19  05:13:28  rharagan
 * CSCdi37295:  Mark controller types as reserved
 * Add controller types in use in branches or in development.
 *
 * Revision 2.2  1995/06/19  21:50:37  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.1  1995/06/07  20:41:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$ */

#ifndef __IF_FCI_H__
#define __IF_FCI_H__

#define FCISTART (MULTIIOBASE+0xC000)
#define FCI_INTLEVEL 			3
#define HOTSTALL_INTLEVEL		4
#define EOIR_BUFFER_RETURN_TIME		200 /* time allowed for traffic to end*/
#define FCI_MAX_CTRLR_NUMBER 		7
#define CBUS1_MAX_INTERFACES		24	/* max number of interfaces */
#define CBUS1_MAX_CTRLR_INTERFACES	(CBUS1_MAX_INTERFACES / \
					 FCI_MAX_DCI_CTRLRS)
#ifdef C7000
#define CBUS2_MAX_INTERFACES		80
#define CBUS2_NUM_EXT_INTERFACES        40  /* how many of the MAX are extended */
#define PA_MAX_BAYS                      2
#else
#define CBUS2_MAX_INTERFACES		32
#define CBUS2_NUM_EXT_INTERFACES         0  /* how many of the MAX are extended */
#define PA_MAX_BAYS                      1  
#endif
#define CBUS2_MAX_CTRLR_INTERFACES	(CBUS2_MAX_INTERFACES / \
					 FCI_MAX_DCI_CTRLRS)

#define CBUS2_NON_EXT_INTERFACES         (CBUS2_MAX_INTERFACES - \
					 CBUS2_NUM_EXT_INTERFACES)

#define PA_MAX_INTERFACES               (CBUS2_MAX_CTRLR_INTERFACES / PA_MAX_BAYS)

/*
 * Memory definitions
 */
#define CBUSII_HARD_RESET_TIME	500	/* 500 milliseconds */
#define FCI_HARD_RESET_TIME	500	/* 500 milliseconds */
#define FCI_SOFT_RESET_TIME	500	/* 500 milliseconds */

#ifdef C7000
#define FCI_MEMA_BASE		0x150   /* apparently not used */
#define FCI_MEMD_BASE   	0x240	/* start location for diagnostics */
#else
#define FCI_MEMA_BASE		0x120
#define FCI_MEMD_BASE   	0x120	/* start location for diagnostics */
#endif

/*
 * Controller type(s) supported by the Fast Communications Interface.
 * This is the number programmed into the hardware and returned by the
 * 'who' command.  Derek Brown is the current keeper of the numbers.
 * Bill Palmer and John Foster are the engineering contacts.
 */
typedef enum ctrlr_type_ {
    FCI_UNKNOWN_CONTROLLER 	= 0xffff,
    FCI_ETHER_CONTROLLER 	= 0,
    FCI_FDDI_CONTROLLER 	= 1,
    FCI_SERIAL_CONTROLLER	= 2,
    FCI_LINK_CONTROLLER		= 3,	/* ULTRANET and DS3 */
/*  FCI_DTR_CONTROLLER		= 4,	   Rest In Peace */
    FCI_CTR_CONTROLLER		= 5,
    FCI_FDDIT_CONTROLLER	= 6,
    FCI_FSIP_CONTROLLER		= 7,
    FCI_AIP_CONTROLLER		= 8,
    FCI_MIP_CONTROLLER		= 9,
    FCI_SSP_CONTROLLER		= 10,
    FCI_SP_CONTROLLER		= 11,
    FCI_RP_CONTROLLER		= 12,
    FCI_CIP_CONTROLLER		= 13,
    FCI_RSP1_CONTROLLER		= 14,
    FCI_RSP2_CONTROLLER		= 15,
    FCI_CI_CONTROLLER		= 16,
    FCI_CTR_SRS_CONTROLLER	= 17,
    FCI_FDDIT_SRS_CONTROLLER	= 18,
    FCI_FEIP_CONTROLLER         = 19,
    FCI_RVIP_CONTROLLER         = 20,
    FCI_SVIP_CONTROLLER         = 21,
    FCI_SSIP_CONTROLLER         = 22,
    FCI_SMIP_CONTROLLER         = 23,
    FCI_RSP7000_CONTROLLER	= 24,
    FCI_CI7000_CONTROLLER	= 25,
    FCI_RSP4_CONTROLLER		= 26,
    FCI_POSIP_CONTROLLER        = 27,
    FCI_C5RSP2_CONTROLLER       = 28,
    FCI_C5IP_CONTROLLER         = 29,
    FCI_NUM_CONTROLLERS,	/* not a real controller */
				/* actually (1 + last enum value) */
} ctrlr_type_t;

#define IFTYPE_BITS   4
#define IFTYPE_MASK   0xF

#define FCITYPE(x)  ((x) & IFTYPE_MASK)
#define FCI_TYPE_VIP_FETHER   ((0x01 << IFTYPE_BITS) | FCI_TYPE_ETHER)  
#define FCI_TYPE_VIP_ETHER8   ((0x02 << IFTYPE_BITS) | FCI_TYPE_ETHER)  
#define FCI_TYPE_VIP_FETHER_TX_NISL ((0x03 << IFTYPE_BITS) | FCI_TYPE_ETHER)  
#define FCI_TYPE_VIP_FETHER_TX_ISL  ((0x04 << IFTYPE_BITS) | FCI_TYPE_ETHER)  
#define FCI_TYPE_VIP_FETHER_FX_ISL  ((0x05 << IFTYPE_BITS) | FCI_TYPE_ETHER)  
#define FCI_TYPE_VIP_SERIAL4  ((0x01 << IFTYPE_BITS) | FCI_TYPE_SERIAL) 
#define FCI_TYPE_VIP_TRING4   ((0x01 << IFTYPE_BITS) | FCI_TYPE_CTR)    
#define FCI_TYPE_VIP_FDDI     ((0x01 << IFTYPE_BITS) | FCI_TYPE_FDDIT)
#define FCI_TYPE_VIP_ESA      ((0x01 << IFTYPE_BITS) | FCI_TYPE_ESA)

#define FCI_TYPE_RSP(t)  (((t) == FCI_RSP1_CONTROLLER) ||         \
			  ((t) == FCI_RSP2_CONTROLLER) ||         \
			  ((t) == FCI_RSP4_CONTROLLER) ||         \
			  ((t) == FCI_C5RSP2_CONTROLLER) ||       \
			  ((t) == FCI_RSP7000_CONTROLLER))

/*
 * HSSI Specific numbers
 */
#define HSSI_DISABLE_TIME	5	/* 5 milliseconds */

/*
 * FDDI Specific numbers
 */

/* For AGS+ and 7000 boxes */
#define FDDI_RECEIVE_OFFSET		(MCI_OFFSETSIZE - 8) 
                                        /* 32 words from zero */

#define FDDI_RECEIVE_WORD_OFFSET	(FDDI_RECEIVE_OFFSET & 0xFFFE)
#define FDDI_RECEIVE_BYTE_OFFSET	((FDDI_RECEIVE_OFFSET * 2) + 1)

/*
 * RSP specific offsets moved to src-rsp/rsp_if.h.
 */

/*
 * Value used to carve up buffers in the cBus MEMD.
 */
#define FDDI_BUFFERSIZE_WORDS	((((FDDI_BUFFERSIZE_BYTES + 1) - \
				 ((MCI_OFFSETSIZE * 2) - \
				  FDDI_RECEIVE_BYTE_OFFSET)) / 2) + 1) 

/*
 * Token Ring Specific numbers
 */
#define CTR_RECEIVE_OFFSET	(MCI_OFFSETSIZE - 8) /* 32 words from zero */
#define CTR_RECEIVE_WORD_OFFSET	(CTR_RECEIVE_OFFSET & 0XFFFE)
#define CTR_RECEIVE_BYTE_OFFSET	(CTR_RECEIVE_OFFSET * 2)

#define CTR_FRAME_PAYLOAD	4472	/* TR is defined by info size */

#define CTR_SDEL		1
#define CTR_AC			1
#define CTR_FC			1
#define CTR_DA			6
#define CTR_SA			6    
#define CTR_LLC			3
#define CTR_SNAP		5
#define CTR_FCS			4
#define CTR_EDEL		1
#define CTR_STATUS		1
#define CTR_RIF                18

#define CTR_INFO_SIZE      	CTR_FRAME_PAYLOAD

#define CTR_BAGGAGE            (CTR_LLC + CTR_SNAP)

#define CTR_BUFFERSIZE_BYTES  	(CTR_INFO_SIZE + (CTR_AC + \
						  CTR_FC + \
						  CTR_DA + \
						  CTR_SA + \
						  CTR_RIF + \
						  CTR_FCS + \
						  1))
/*
 * This is CTR_BUFFERSIZE_BYTES minus the maxmtu
 */
#define CTR_OVERHEAD_BYTES     (CTR_BUFFERSIZE_BYTES - \
				  (CTR_INFO_SIZE - \
				   (CTR_LLC + CTR_SNAP)))

#define CTR_BUFFERSIZE_WORDS	((((CTR_BUFFERSIZE_BYTES + 1) - \
				 ((MCI_OFFSETSIZE * 2) - \
				  CTR_RECEIVE_BYTE_OFFSET)) / 2) + 1) 


/*
 * Channel specific values
 */

#define CHANNEL_RECEIVE_OFFSET	(MCI_OFFSETSIZE - 2) /* 48 words from zero */
#define MCI_CHANNEL_OFFSET (MCI_OFFSETSIZE - 2) /* 4 byte encapsulation */


/*
 * Buffer pools in the FCI.
 */

#define FCI_BUF_POOL_0		0
#define FCI_BUF_POOL_1		1
#define FCI_BUF_POOL_2		2
#define FCI_BUF_POOL_3		3
#define FCI_BUF_POOL_4		4
#define FCI_BUF_POOL_5		5
#define FCI_BUF_POOL_6		6
#define FCI_BUF_POOL_7		7

#define FCI_MINI_BUF_POOL		FCI_BUF_POOL_0
#define FCI_SYSTEM_BUF_POOL		FCI_BUF_POOL_1
#define FCI_RX_POOL_0		        FCI_BUF_POOL_2
#define FCI_RX_POOL_1		        FCI_BUF_POOL_3
#define FCI_RX_POOL_2		        FCI_BUF_POOL_4
#define FCI_RX_POOL_3		        FCI_BUF_POOL_5
#define FDDI_CLAIM_BEACON_POOL		FCI_BUF_POOL_6
#define FCI_RX_ERROR_POOL		FCI_BUF_POOL_7
 
#define FCI_MINI_BUF_SIZE       256
#define FCI_MINI_BUF_COUNT       16
#define FCI_MINI_BUF_COUNT_LOW    4
#define FCI_SYSTEM_BUF_SIZE    1024
#define FCI_SYSTEM_BUF_COUNT      4

/*
 * this is the number of buffer pools left over after error pool, claim
 * beacon pool, and system pools.  
 */
#define FCI_BUF_POOL_MAX        (FCI_RX_POOL_3 +1)


/*
 * FCI Global command codes.  These commands all talk directly to the
 * Cbus Controller card.  They do not have any effect on any of the
 * daughter cards.
 */
#define FCI_CMD_VERSION		0	/* read version */
# define FCI_VERSION_SW		0	/* get ucode version number */
# define FCI_VERSION_HW		1	/* get what the f/w thinks the hardware is */
#define FCI_CMD_RESET		1	/* reset controller */
# define FCI_HARD_RESET		0	/* argument to FCI_CMD_RESET */
# define FCI_SOFT_RESET		1	/* argument to FCI_CMD_RESET */
#define FCI_CMD_MEMSIZE		2	/* read memory size in words */
#define FCI_CMD_SELECT		3	/* select an interface */
#define FCI_CMD_MEMD_SELECT	4	/* select memd address */
#define FCI_CMD_RX_SELECT	5	/* select offset into RX */
#define FCI_CMD_TX0_SELECT	6	/* select offset into transmit 0 */
#define FCI_CMD_TX1_SELECT	7	/* select offset into transmit 1 */
#define FCI_CMD_RX_SETUP	8	/* setup RX window */
#define FCI_CMD_ASR		9	/* NOT used on cBus */
# define FCI_CMD_BRIDGE_FLUSH	9	/* like RX_FLUSH, for cBus flooding */
#define FCI_CMD_CONDITIONS	10	/* error, DCD, and TX done summary */
#define FCI_CMD_RX_MOVE		11	/* move RX buffer to TX1 window */
#define FCI_CMD_INTERRUPT	12	/* define interrupt level */
#define FCI_CMD_RX_FLUSH	13	/* move or flush current RX buffer */
#define FCI_CMD_TX0_RESERVE	14	/* reserve a buffer in transmit 0 */
#define FCI_CMD_TX1_RESERVE	15	/* reserve a buffer in transmit 1 */
#define FCI_CMD_RX_CHECK	16	/* validate special RX packet */
#define FCI_CMD_CAPABILITY	17	/* read controller capabilities */
#define FCI_CMD_MEMA_READ	18	/* read word from cbus controller mema */
#define FCI_CMD_MEMA_WRITE	19	/* write word to cbus controller mema */
#define FCI_CMD_MEMA_SELECT	20	/* select cbus controller mema address */
#define FCI_CMD_OVERHEAD	21	/* read and write buffer overhead */
#define FCI_CMD_ARGUMENT	22	/* set buffer size for TX or RX */
#define FCI_CMD_BUFFERSIZE	22	
#define FCI_CMD_TX_ALLOCATE	23	/* allocate a transmit buffer */
#define FCI_CMD_LIGHTS		24	/* write LED's */
#define FCI_CMD_BRIDGE		25	/* FSA bit mask for bridging */
#define FCI_CMD_BRIDGEPROTOCOL	26	/* protocol specific bridging ctl */
#define FCI_CMD_MEMX_SELECT_HIGH 27	/* high 16 bits of memx address */
#define FCI_CMD_STATISTICS	27	/* autonomously switched counters */
#define FCI_CMD_MEMX_SELECT_LOW	28	/* low 16 bits of memx address */
#define FCI_CMD_GET_ICB		29	/* convert mci_index to icbptr */
#define FCI_CMD_SET_MTU		31	/* tell ALU about the MTU */

/*
 * The above commands are identical to those command in if_mci.h
 */

/*
 * Interface specific command codes. Valid after a FCI_CMD_SELECT or
 * FCI_CMD_RX_MOVE.  These commands are used by talking directly to the
 * Cbus Controller card, and it will forward them to the daughter cards.
 */
#define FCI_CMD_STATUS		32	/* read and write controller status */
#  define FCI_STATUS_ENABLE	0	/* enable the interface */
#  define FCI_STATUS_DISABLE	1	/* disable the interface */
#  define FCI_STATUS_LOOPLOCAL	2
#  define FCI_STATUS_NOLOOP	3
#  define FCI_STATUS_QUIESCE	4
#  define FCI_STATUS_UNQUIESCE  5
#  define FCI_STATUS_THROTTLE	6	/* throttle the interface */
#  define FCI_IF_THROTTLE	6
#  define FCI_IF_UNTHROTTLE	7
#  define FCI_STATUS_LOOPREMOTE	10	/* used by the MIP card */
#define FCI_CMD_STATION		33	/* read Ethernet station address */
#define FCI_CMD_AF_SELECT	34	/* select address filter address*/
#define FCI_CMD_AF_READ		35	/* read address filter mask */
#define FCI_CMD_AF_WRITE	36	/* write address filter mask */
#define FCI_CMD_CARRIER		37	/* read current DCD status */
#define FCI_CMD_ERRORS		38	/* read and reset error counter */
#define FCI_CMD_TX0_START	39	/* start transmitting TX0 */
#define FCI_CMD_TX1_START	40	/* start transmitting TX1 */
#define FCI_CMD_EXECUTE		41	/* execute the argreg CMD */
# define FCI_ASSIGN_POOLINDEX 	0
# define FCI_MAX_BUFFERSIZE	1
# define FCI_RX_OFFSET		2
# define FCI_RX_BUFFER_LIMIT	3
# define FCI_RX_ERROR		4
# define FCI_FDDI_HACK		5 	/* new name needed */
#define FCI_CMD_APPLIQUE	42	/* write and read serial applique */
#define FCI_CMD_CLOCKRATE	43	/* bit rate for generating clock */
#define FCI_CMD_CLOCKTYPE	44	/* external vs generated clock */
#define FCI_CMD_DELAY		45	/* transmitter delay counter */
#define FCI_CMD_QUEUE		46	/* maximum output queue length */
#define FCI_CMD_SLIMIT		47	/* min. queue length when switching */

/* 
 * The above commands are identical to those in if_mci.h -- the following
 * commands are unique to the cbus.
 */
#define FDDI_CMD_FM_SELECT	48	/* select the formac */
#define FDDI_CMD_FM_WRITE	49	/* write to the formac */
#define FDDI_CMD_FM_READ	50	/* read the formac */
#define FDDI_CMD_EN_SELECT	51	/* select the endec */
#define FDDI_CMD_EN_WRITE	52	/* write the endec */
#define FDDI_CMD_EN_READ	53	/* read the endec */
#define FDDI_CMD_CLAIM_BEACON	54	/* station management functions ? */
#define FCI_CMD_AF_SIZE		55	/* get size of address filter */
#define FCI_CMD_HS_OP		58	/* hot stall management function(s) */
#  define FCI_HS_NORMAL_MODE	0 	/* normal system operational mode */
#  define FCI_HS_DONOTHING_MODE 1	/* ignore all commands until in NORMAL mode */
#  define FCI_HS_STALL_SENSE	2 	/* poll bus stall signal */
#  define FCI_HS_OIR_RESET	5	/* soft reset (w/o ICB_TYPE trashing) */
#define FCI_CMD_READ_ICB	59	/* read ICB */
#  define FCI_ICB_IFTYPE	0 	/* ICB locations to read */
#  define FCI_ICB_RXQCOUNT	1
#  define FCI_ICB_TXQHEAD	2
#  define FCI_ICB_TXQTAIL	3
#  define FCI_ICB_TXQCOUNT	4

/*
 * AIP specific commands issued using the FCI_CTRLR_CMD & FCI_CTRLR_ARGUMENT
 */
#define	FCI_CMD_AIP_PROC_SELECT	66	/* Select the TX or RX processor */
#define	FCI_CMD_AIP_ADDR_SELECT	67	/* Select AIP addr. */
#define	FCI_CMD_AIP_MEM_WRITE	68	/* Write into AIP */
#define	FCI_CMD_AIP_MEM_READ	69	/* Read AIP mem. */
#define	FCI_CMD_AIP_CMDBLOCK	70	/* Get the AIP MEMA command addr. */
#define FCI_CMD_AIP_CMDPARSE	71	/* Parse the given AIP command */
#define	FCI_CMD_AIP_FRAMING	72	/* AIP framing mode */
#define	FCI_CMD_AIP_SONET	73	/* AIP SONET mode */
#define	FCI_CMD_AIP_DS3SCRAMBLE	76	/* AIP Scramble option */
#define	FCI_CMD_AIP_SETVER	77	/* H/W bug work-around */

/*
 *  MIP specific commands
 */
#define FCI_CMD_VC_SELECT	64	/* select a virtual ciruit */
#define	FCI_CMD_VC_STATUS	65	/* same params as FCI_CMD_STATUS*/
#define	FCI_CMD_VC_ERRORS	66
#define	FCI_CMD_VC_MAP		67	/* map a circuit disabled */
#define	FCI_CMD_HOMOL_TYPE	68
#define	FCI_CMD_VC_UNMAP_ALL	69
#define	FCI_CMD_FRAME_TYPE	70
#define	FCI_CMD_VC_SUBRATE	71
#define	FCI_CMD_LINECODE_TYPE	72
#define	FCI_CMD_CHANNEL_STATUS	73	/* status command for the full T1 */
#define	FCI_CMD_CABLE_LENGTH	75
#define	FCI_CMD_MIPCARRIER	74
#define	FCI_CMD_VC_ENCAP	76	/* the encapsulation byte we want */
#define	FCI_CMD_VC_MAPENABLE	77	/* map and enable a circuit */
#define	FCI_CMD_VC_MTU		78
#define	FCI_CMD_VC_TX_LIMIT	79
#define FCI_CMD_CLR_COUNTS	80	/* clear onboard counters */
#define	FCI_CMD_NO_LOVELETTERS	81	/* stop sending love letters */
#define	FCI_CMD_OK_LOVELETTERS	82	/* ok to send love letters */
#define	FCI_CMD_ALT_IDLE	83	/* ok to use alt idle pattern */
#define FCI_CMD_TXQLENGTH	84
#define FCI_CMD_SHORT_TXQ_LOCALBUF	85 /* shorten tx local buffer
					      on an interface */
#define FCI_CMD_VC_TXACCUM	86	/* vc accumulator address */

/*
 *  VIP 4R specific commands
 */  
#define FCI_CMD_VIP_4R_TEST              3
#define FCI_CMD_VIP_4R_START_BLOCK      11
#define FCI_CMD_VIP_4R_WRITE_BLOCK      12
#define FCI_CMD_VIP_4R_END_BLOCK        13
#define FCI_CMD_VIP_4R_REQUEST_STATS    14

/*
 *  VIP specific commands
 */  
#define FCI_CMD_VIP_REQUEST_CONFIG     120

/*
 * Debug command.
 */
#define FCI_CMD_CTRLR_MEMA_SELECT 17	/* Select daughter card mema */
#define FCI_CMD_CTRLR_MEMA_WRITE  18	/* Write daughter card mema */
#define FCI_CMD_CTRLR_MEMA_READ   19	/* Read daughter card mema */
#define FCI_CMD_TX0_ENQUEUE	  56	/* enqueue -- don't start */
#define FCI_CMD_TX1_ENQUEUE	  57	/* enqueue -- don't start */

#define FDDI_CMD_APPLIQUE_SELECT   FDDI_CMD_EN_SELECT	/* select applique */
#define FDDI_CMD_APPLIQUE_WRITE    FDDI_CMD_EN_WRITE	/* write applique */
#define FDDI_CMD_APPLIQUE_READ     FDDI_CMD_EN_READ	/* read appilque */

/*
 * Controller commands.
 */
#define FCI_CMD_CTRLR_SELECT		0x8000
#define FCI_CTRLR_CMD			0x8002
#define FCI_CTRLR_ARGUMENT		0x8003
#define FCI_CMD_CTRLR_WHO		0x8004
#define FCI_CMD_CTRLR_LOAD	        0x8005
#define FCI_CMD_CTRLR_RESET		0x8006


/*
 * Autonomous switching commands.
 */
#define	FCI_AS_CAPABILITY		0x8007
#define	FCI_CACHE_FREE_DYNAMIC		0x8008
#define	FCI_CACHE_SELECT_ABS		0x8009
#define FCI_ALLOCATE_CACHE		0x800A
#define FCI_LOOKUP_CACHE		0x800B
#define FCI_KEY_SELECT   		0x800C
#define FCI_CACHE_SELECT		0x800D
#define FCI_CACHE_ENTER 		0x800E
#define FCI_CACHE_DELETE		0x800F
#define FCI_CACHE_INVALIDATE		0x8010
#define FCI_CACHE_GET_NEXT		0x8011
#define FCI_CACHE_VERSION_UPDATE	0x8012

/*
 * Ring Numbers and Bridge Descriptors
 */
#define FCI_SRB_RING_BRIDGE_RING	0x8013	/* arg=(LR<<4)|BN, arg1=TR<<4 */
#define FCI_SRB_TARGET_INDEX		0x8014	/* output index (or -1 if virtual) */
#define   FCI_SRB_NO_TARGET		  0xFFFF
#define FCI_SRB_VRING_CREATE		0x8015	/* arg=(VRN<<4) | table(0/1) */
#define FCI_SRB_KEY_INDEX		0x8016	/* arg=(TR<<4)|BN, arg1=mci_idx */
#define FCI_SRB_VRING_DESTROY		0x8017	/* arg=vring_table (0 or 1) */
#define FCI_SRB_BRIDGING_TYPE		0x8018	/* trans, srb, srt, etc. */
#define	FCI_CACHE_ALLOC_DYNAMIC		0x8019	/* Allocate dynamic cache ent.*/
#define	FCI_SELECT_HASH_TABLE		0x801A	/* IP/IPX/AB hash table sel. */
#define	FCI_MEMA_READ_CLEAR_LONG	0x801B	/* Read & Clear MEMA */
/*
 * Autonomous bridge fields
 */
#define	FCI_BRIDGE_GROUP_NUMBER		0x801C	/* MEMA == BRIDGE grp number */
#define	FCI_BRIDGE_GROUP_NEXT		0x801D	/* MEMA == BRIDGE grp next */
#define	FCI_BRIDGE_GROUP_FLAGS		0x801E	/* MEMA == BRIDGE grp flags */
#define	FCI_BRIDGE_AUTO_FLOOD		0x801F	/* ??? */
#define	FCI_CACHE_INSERT		0x8020
#define FCI_SSE_CONTROL			0x8021  /* 0 == off, 1 == on */
#define FCI_SSE_DIAG_START		0x8022  /* start SSE on diag prog */
#define FCI_SSE_DIAG_READ		0x8023  /* read SSE result register */
#define FCI_SSE_DIAG_DONE		0x8024  /* send done to SSE */
#define FCI_SSE_BRIDGE_CONTROL		0x8025  /* 0 == off, 1 == on */
#define FCI_CTRLR_ARGUMENT2		0x8026
#define FCI_CTRLR_CMD_EXT		0x8027

/*
 * FCI_SELECT_HASH_TABLE argreg values
 */
#define	FCI_IP_HASH_TABLE		0x0
#define	FCI_IP_CACHE_UPDATE		0x0
#define	FCI_IPX_HASH_TABLE		0x1
#define	FCI_IPX_CACHE_UPDATE		0x1
#define	FCI_AB_HASH_TABLE		0x2

/*
 * FCI_AS_CAPABILITY return values. (bit mask of these)
 */
#define	FCI_CAP_IP_AUTO			0x1
#define	FCI_CAP_IPX_AUTO		0x2  /*Old style IPX AS (encap+mode)*/
#define	FCI_CAP_BRIDGE_AUTO		0x4
#define	FCI_CAP_BRIDGE_FLOOD_AUTO	0x8
#define	FCI_CAP_IPX_AUTO_NEW		0x10 /*New style IPX AS (mode+encap)*/
#define FCI_CAP_OIR_RESET		0x20

#define	FCI_IPX_AUTO(a)			((a) & (FCI_CAP_IPX_AUTO |           \
						FCI_CAP_IPX_AUTO_NEW))
#define	FCI_OTHER_AUTO(a)		((a) & ~FCI_CAP_IP_AUTO)

/*
 * Bridge Cache Entry
 */
#define BCE_MAC_OFFSET                  0x1
#define BCE_IFA_OFFSET                  0x4
#define BCE_ACTION_OFFSET               0x5  /* left half of word is encap type */
#define BCE_PERMIT_IN_OFFSET            0x6
#define BCE_PERMIT_OUT_OFFSET           0x8
#define BCE_RX_OFFSET                   0xA
#define BCE_TX_OFFSET                   0xC
#define BCE_LENGTH                      0xE
#define BCE_ACTION_RECEIVE  0x0000
#define BCE_ACTION_DISCARD  0x0001 /* 0=do_action, 1=discard */
#define BCE_ACTION_FORWARD  0x0002 /* 0=receive, 1=forward */
#define BCE_ACTION_CHKPERM  0x0004 /* 0=ignore, 1=must check permissions */

/*
 * IFA Spanning Tree states + Burb bit
 *
 */
#define IFA_SPAN_ACTION_BLOCK                0x0
#define IFA_SPAN_ACTION_LISTEN               0x1
#define IFA_SPAN_ACTION_LEARN                0x2
#define IFA_SPAN_ACTION_FORWARD              0x3
#define IFA_SPAN_ACTION_BURP                 0x4

/*
 * Token Ring Bridge Types
 */
#define FCI_BRIDGE_NONE			0
#define FCI_BRIDGE_TRANS		1	/* translational / transparent */
#define FCI_BRIDGE_SRB			2	/* SRB only */
#define FCI_BRIDGE_SRT_STRICT		3
#define FCI_BRIDGE_SRT_LOOSE		4

/*
 * Controller specific commands -- issued via the FCI_CTRL_CMD command.
 */

#define FCI_CTRLR_VERSION		0
#define FCI_CTRLR_RESET			1
#define FCI_CTRLR_SELECT_INT		2
#define FCI_CTRLR_PAGE_SELECT		3
#define FCI_CTRLR_REG_SELECT		4
#define FCI_CTRLR_REG_WRITE		5    
#define FCI_CTRLR_REG_READ		6    
#define FCI_CTRLR_TX_START		7
#define FCI_CTRLR_MEMD_SELECT		8
#define FCI_CTRLR_MEMD_WRITE		9
#define FCI_CTRLR_MEMD_READ		10
#define FCI_FDDI_HOLD_REG		11
#define FCI_FDDI_HEARSELF		12
#define FCI_FDDI_BURST_MODE		13

/*
 * FCI_CTRLR_CMD ranges
 *     0 -  63) Values have specific meaning across controller types
 *    64 - 127) Values are controller-specific
 *   128 -    ) Values have specific meaning across controller types
 */
#define FCI_CTRLR_CMDSYNC	       128    /* Resync after a timeout */

/*
 * FDDI CAM interface
 *
 * The FDDI-T CAM interface is used to read, write, and manage the
 * CAM on the FDDI-T.  This is accomplished through a simple command/data
 * command port pair that talks to the FDDI-T uCode.  There is a 5 word
 * (16 bit) argument area that is written via the FCI_FDDI_CAM_ARG command.
 * Which 16 bit word is being written is automatically incremented after
 * any read or write of the FCI_FDDI_CAM_ARG.  The pointer is reset back
 * to the beginning following the execution of any command.  The data is 
 * organized as follows:
 *
 *     camarg	NOP/CLR/LOCK/	INS	RMV	CHK	CHK	SET
 * 		UNLOCK					rd_arg
 *
 *       0	no data		maddr	maddr	maddr	msw	mod_arg
 *       1			maddr	maddr	maddr	lsw
 *       2			maddr	maddr	maddr
 *       3			ramdata
 *       4
 *
 *     camarg	RD	RD	WR	RD_CTL	RD_CTL	WR_CTL
 * 			rd_arg			rd_arg
 *
 *       0	camaddr maddr	camaddr	ctl_reg	value	ctl_reg
 *       1		maddr	maddr			value
 *       2		maddr	maddr
 *       3		ramdata	maddr
 *       4			ramdata
 *
 *     camarg	RD_DATA	RD_DATA	WR_DATA LOCK	UNLOCK	LEARN
 * 			rd_arg		CAM	CAM	MA
 *
 *       0		value	value			maddr
 *       1						maddr
 *       2						maddr
 *       3
 *       4
 *
 * Commands are issued via the command FDDI_CAM_CMD.  The following commands
 * are defined:
 *
 * 	0	NOP.  Do nothing.  Used to reset the arg pointer.
 * 	1	CLR CAM.  This removes all entries in the CAM.
 * 	2	INS ENTRY.  Insert an entry into the CAM.
 * 	3	RMV ENTRY.  Remove an entry from the CAM.
 * 	4	CHK ENTRY.   Run an associative cycle on an address
 * 	5	SET MODIFIER.  Set up the modifier for direct read/write access.
 * 	6	RD.  Read CAM directly
 * 	7	WR.  Write CAM directly
 * 	8	RD_CTL.  Read Control Register
 * 	9	WR_CTL.  Write Control Register
 * 	10	RD_DATA.  Read CAM Data port
 * 	11	WR_DATA.  Write CAM Data port
 * 	12	LOCK_CAM.  Lock out the cam for an extended period.
 * 	13	UNLOCK_CAM.  Unlock the extended lockout.
 * 	14	LEARN_MA.  Learn the mac address of this node.
 *	15	INSERT_MC. Just like insert entry but allows Multicast
 *			insertions.
 */

#define FCI_FDDI_CAM_CMD		28
#define FCI_FDDI_CAM_RD_ARG		29
#define FCI_FDDI_CAM_WR_ARG		30

#define FDDI_CAM_NOP			0
#define FDDI_CAM_CLR			1
#define FDDI_CAM_INSERT			2
#define FDDI_CAM_REMOVE			3
#define FDDI_CAM_CHECK			4
#define FDDI_CAM_SET_MODIFIER		5

#  define FCSM_DISABLE				0x00
#  define FCSM_VALID				0x04
#  define FCSM_EMPTY				0x05
#  define FCSM_SKIP				0x06
#  define FCSM_RANDOM				0x07

#  define FCSM_AR				0x20
#  define FCSM_HM				0x28
#  define FCSM_NFA				0x30

#define FDDI_CAM_RD_CAM			6
#define FDDI_CAM_WR_CAM			7
#define FDDI_CAM_RD_CTL			8
#define FDDI_CAM_WR_CTL			9
#  define FCRWC_CTL				0x00
#  define FCRWC_PA				0x08
#  define FCRWC_SEG				0x10
#  define FCRWC_NFA				0x18
#  define FCRWC_AR				0x20
#  define FCRWC_DS				0x28
#  define FCRWC_PS				0x30
#  define FCRWC_PD				0x38
#  define FCRWC_STATUS				0xFFFF
#  define FCRWC_INSTR				0xFFFF
#define FDDI_CAM_RD_DATA		10
#define FDDI_CAM_WR_DATA		11
#define FDDI_CAM_LOCK_CAM		12
#define FDDI_CAM_UNLOCK_CAM		13
#define FDDI_CAM_LEARN_MA		14
#define FDDI_CAM_INSERT_MC		15

/*
 * POSIP commands
 */

# define FCI_POSIP_SPECIFIC               62

# define POSIP_LOOP_NONE                   0    /* Loopback modes */
# define POSIP_LOOP_INTERNAL               1
# define POSIP_LOOP_EXTERNAL               2
# define POSIP_CLOCK_EXTERNAL              3
# define POSIP_CLOCK_INTERNAL              4
# define POSIP_SDH_ON                      5
# define POSIP_SDH_OFF                     6
# define POSIP_GET_MEM_SIZE                7


/*
 * FEIP commands
 */

# define FCI_FEIP_SPECIFIC               61

# define FEIP_LOOP_NONE                   0    /* Loopback modes */
# define FEIP_LOOP_MOTOROLA               1
# define FEIP_LOOP_DEC                    2
# define FEIP_LOOP_MICRO_LINEAR           3
# define FEIP_RJ45                        4    /* media selection */
# define FEIP_MII                         5
# define FEIP_FULL_DUPLEX                 6    /* duplex mode */
# define FEIP_HALF_DUPLEX                 7
# define FEIP_INIT_AF                     8    /* init address filter */
# define FEIP_RCVS_POLL0                  9    /* diagnostic code */
# define FEIP_RCVS_POLL1                 10    /* diagnostic code */
# define FEIP_TXPOLL0                    11    /* diagnostic code */
# define FEIP_TXPOLL1                    12    /* diagnostic code */

/* POSIP/FEIP sub version commands */

# define FCI_PA_READ_RST                 4
# define FCI_PA_EEPROM_READ              5

/*
 * The following command is used to instruct the Fast Ethernet ucode to
 * cache an ISL vLAN Identifier (commonly referred to as a "colour") so
 * that it can autonomously classify received ISL datagrams and match
 * the embedded ISL colour to determine whether the datagram is eligible
 * for SP/SSP switching or it should be passed to Fast Switching.
 *
 * The corresponding 10 bit ISL colour field is passed down in bits 0-9
 * of the argreg.
 */

# define FEIP_ISL_COLOUR_ENTRY           14


/*
 * FSIP commands
 */
#define FCI_FSIP_TX_INVERT_CLK           33
#define FCI_FSIP_NRZI                    34
#define FCI_FSIP_HALFDUPLEX              35
#define FCI_FSIP_SIGNAL_STS              61
#define FCI_SERIAL_SETUP_G703            64
#define FCI_SERIAL_READ_G703             65
#define FCI_SERIAL_START_SLOT_G703       66
#define FCI_SERIAL_STOP_SLOT_G703        67
#define FCI_SERIAL_CRC4                  68
#define FCI_SERIAL_TS16                  69
#define FCI_SERIAL_CLOCKSOURCE           70
#define FCI_SERIAL_IGNORE_DCD		 86

#define MAX_MTU_SUPPORTED                93 /* 0x5d, get the maximum
					       mtu size supported by
					       the local buffering */

/*
 * FSIP and HIP commands
 */
#define FCI_SERIAL_CRC_32		63

/*
 * FSIP SIGNAL MASKS
 */
#define FSIP_MASK_CTS                   0x1
#define FSIP_MASK_RTS                   0x2
#define FSIP_MASK_DTR                   0x4
#define FSIP_MASK_DSR                   0x8
#define FSIP_MASK_DCD                   0x10
#define FSIP_MASK_BER      		0x20
#define FSIP_MASK_NELR		        0x40
#define FSIP_MASK_FELR		        0x80

/*
 * AIP commands
 */
#define	FCI_AIP_PROC_SELECT		66
#define	FCI_AIP_ADDR_SELECT		67
#define	FCI_AIP_MEM_WRITE		68
#define	FCI_AIP_MEM_READ		69

/*
 * HSSI commands
 */
#define FCI_HSCI_TA                     20
#define FCI_HSCI_ICLK                   21
# define HSSI_CLK_EXTERNAL              0
# define HSSI_CLK_INTERNAL              1

/*
 * HSSI loopback modes
 */
#define FCI_HSCI_LOOPBACK               39
# define HSSI_LOOP_OFF     0
# define HSSI_LOOP_LINE    1
# define HSSI_LOOP_REMOTE  2
# define HSSI_LOOP_DTE     3

#define FCI_CBUS_BUFFER_PARK            40
#define FCI_HSCI_READ_LOOP              41
#define FCI_HSCI_CMD_XMIT               42
#define FCI_HSCI_CRC_HACK               44
# define ULTRA_CRC_ENABLE               0
# define ULTRA_CRC_DISABLE              1
#define FCI_HSCI_LC_INHIBIT             56
# define HSSI_LCI_ON                    1
# define HSSI_LCI_OFF                   0
#define FCI_HSCI_SEND_BREAK             57

#define FCI_FDDI_PHY_A_INT_MASK		51
#define FCI_FDDI_PHY_B_INT_MASK		52
#define FCI_FDDI_PHY_A_LEM		53
#define FCI_FDDI_PHY_B_LEM		54
# define FDDI_LEM_ON			1
# define FDDI_LEM_OFF			0

/*
 * Talk to the uCode CMT.  You must put the ucode into CMT
 * mode for any of this to take via SET_CMT_MODE.
 *
 * GET_RVAL returns asked for Result.  Low 10 bits valid.
 * SET_TVAL sets transmit CMT value.  Low 10 bits actual value
 * 	high order bit determines Phy-A or Phy-B.
 * SET_TL_MIN sets TL_MIN for the phys.  
 * KICK_PHY actually starts or stops the CMT process for the
 * 	specified PHY.  If high order bit set PHY-B otherwise PHY-A.
 * START_TRACE start trace -- send MLS.
 * DUP_ADDRESS initialize the DUP_ADDRESS_ENABLE cell in MEMA --
 *             control the generation of dup_address interrupt.
 */

#define FCI_FDDI_DUP_ADDRESS            16
#define FCI_FDDI_START_TRACE            55
#define FCI_FDDI_GET_STATE              56
# define FCI_PHYA_STATE                  0
# define FCI_PHYB_STATE                  1
#define FCI_FDDI_GET_RVAL		57
# define FCI_PHYA_RVAL			0
# define FCI_PHYB_RVAL			1
#define FCI_FDDI_SET_TVAL		58
# define FCI_FDDI_PHY_B			0x8000
#define FCI_FDDI_SET_TL_MIN		59
#define FCI_FDDI_KICK_PHY		60
# define FCI_FDDI_STOP_PHY		0
# define FCI_FDDI_START_PHY		1
#define FCI_FDDI_SET_CMT_MODE		61
# define CMT_MODE_MON			0
# define CMT_MODE_IMMED			1
# define CMT_MODE_CMT			2
#define FCI_FDDI_PHY_GET_LS		62	/* Get interrupt phy status */
# define GET_PHYA_LS			0
# define GET_PHYB_LS			1
#define FCI_FDDI_AF_MASK		63

/*
 * Default number of buffers to park under an hsci card.
 */
#define HSCI_BURST_COUNT_DEF            3
#define CTR_BURST_COUNT_DEF             2
#define CIP_BURST_COUNT_DEF		3

/*
 * Counter registers in the FDDI -- used by SMT get get the frame count.
 */
#define FDDI_REG_FRINC			063

/*
 * Registers available in the FORMAC.
 */
#define FDDI_FM_REG_0	 	0	
#define FDDI_FM_REG_1		1
#define FDDI_FM_REG_2		2
#define FDDI_FM_REG_3		3
#define FDDI_FM_REG_4		4
#define FDDI_FM_REG_5		5
#define FDDI_FM_REG_6		6
#define FDDI_FM_REG_7		7
#define FDDI_FM_REG_8		8
#define FDDI_FM_REG_9		9
#define FDDI_FM_REG_10		10
#define FDDI_FM_REG_11		11
#define FDDI_FM_REG_12		12
#define FDDI_FM_REG_13		13
#define FDDI_FM_REG_14		14
#define FDDI_FM_REG_15		15

/*
 * Write only FORMAC registers i.e. FDDI_FM_CMD_WRITE
 */
#define FDDI_FM_W_RESET		FDDI_FM_REG_0
#define FDDI_FM_W_IDLE_LISTEN	FDDI_FM_REG_1
#define FDDI_FM_W_CLAIM_LISTEN	FDDI_FM_REG_2
#define FDDI_FM_W_BEACON_LISTEN	FDDI_FM_REG_3
#define FDDI_FM_W_RESTRICT_CTRL	FDDI_FM_REG_7
#define FDDI_FM_W_SEND_NOW	FDDI_FM_REG_9
#define FDDI_FM_W_RESET_ADDRPTR	FDDI_FM_REG_12

/*
 * Read only FORMAC registers i.e. FDDI_FM_CMD_READ
 */
#define FDDI_FM_R_LSBS		FDDI_FM_REG_0
#define FDDI_FM_R_TNEG		FDDI_FM_REG_1
#define FDDI_FM_R_MIR1		FDDI_FM_REG_2
#define FDDI_FM_R_MIR0		FDDI_FM_REG_3
#define FDDI_FM_R_STATES	FDDI_FM_REG_9
#define FDDI_FM_R_STATUS	FDDI_FM_REG_12
#define FDDI_FM_R_TVX_TIMER	FDDI_FM_REG_14

/*
 * Read/write address registers i.e FDDI_FM_CMD_{READ WRITE}
 */

#define FDDI_FM_RW_THT		FDDI_FM_REG_4
#define FDDI_FM_RW_TRT		FDDI_FM_REG_5
#define FDDI_FM_RW_TMAX		FDDI_FM_REG_6
#define FDDI_FM_RW_TVX		FDDI_FM_REG_8
#define FDDI_FM_RW_MODE_REG	FDDI_FM_REG_10
#define FDDI_FM_RW_INT_MASK	FDDI_FM_REG_11
#define FDDI_FM_RW_ADDR_REG	FDDI_FM_REG_13
#define FDDI_FM_RW_TPRI		FDDI_FM_REG_15

/* line state values for ENDEC */
#define ENDEC_QLS	0
#define ENDEC_MLS	1
#define ENDEC_HLS	2
#define ENDEC_ILS	3


/* 
 * Registers available in the ENDEC 
 */
#define ENDECA			0
#define ENDECB			1

#define ENDEC_CMD		0x0001
#define ENDEC_DATA		0x0000

#define BYPASS_SWITCH	        2
#define APPLIQUE_BYPASS		3
#define BYPASS_BYPASS		FALSE
#define APPLIQUE_CONNECT        7
#define BYPASS_CONNECT		TRUE

#define FDDI_EN_REG_0		0
#define FDDI_EN_REG_1		1
#define FDDI_EN_REG_2		2
#define FDDI_EN_REG_3		3
#define FDDI_EN_REG_4		4

#define FDDI_EN_RW_CR0		FDDI_EN_REG_0
# define ENDEC_FORCE_QUIET		0x00
# define ENDEC_FORCE_MASTER		0x01
# define ENDEC_FORCE_HALT		0x02
# define ENDEC_FORCE_IDLE		0x03
# define ENDEC_ENABLE_REPEAT_FILTER	0x04
# define ENDEC_FORCE_JKILS		0x05
# define ENDEC_FORCE_IQ			0x06
# define ENDEC_UNFILTERED_ENCODE	0x07
#define FDDI_EN_RW_CR1		FDDI_EN_REG_1
# define ENDEC_TB_BUS			0x00
# define ENDEC_TA_BUS			0x04
# define ENDEC_THROUGH			0x00
# define ENDEC_LOOPBACK			0x01
# define ENDEC_SHORT_LOOPBACK		0x02
# define ENDEC_REPEAT			0x03
#define FDDI_EN_RW_CR2		FDDI_EN_REG_2
# define ENDEC_PARCON			0x01
# define ENDEC_FORCE_ODD		0x02
# define ENDEC_RESET			0x04
#define FDDI_EN_R_STATUS	FDDI_EN_REG_3
#define FDDI_EN_RW_CR3		FDDI_EN_REG_4
# define ENDEC_SMRESET			0x01
# define ENDEC_EXTENSION1		0x02
# define ENDEC_EXTENSION2		0x04

/*
 * Response codes returned by reading command register
 */
#define FCI_RSP_OKAY	       0	/* successful operation */
#define FCI_RSP_ERR_UNK   0x8000	/* unknown error */
#define FCI_RSP_ERR_ILR   0x8001	/* illegal register read */
#define FCI_RSP_ERR_ILW   0x8002	/* illegal register write */
#define FCI_RSP_ERR_UNIT  0x8003	/* impossible unit number */
#define FCI_RSP_ERR_NPRS  0x8004	/* unit not present */
#define FCI_RSP_ERR_FNV   0x8005	/* function not valid */
#define FCI_RSP_ERR_IVO   0x8006	/* invalid offset */
#define FCI_RSP_ERR_NBF   0x8007	/* no available buffers */
#define FCI_RSP_ERR_CMD   0x8008	/* illegal command code */
#define FCI_RSP_ERR_MEM   0x8009	/* no more memory */
#define FCI_RSP_ERR_VLD   0x800A	/* RX_CHECK failed */
#define FCI_RSP_ERR_CHK   0x800B	/* RX_CHECK failed, bad checksum */
#define FCI_RSP_ERR_TTL   0x800C	/* RX_CHECK failed, TTL exceeded */
#define FCI_RSP_ERR_BFR   0x800D	/* buffer already in window */
#define FCI_RSP_ERR_QFL   0x800E	/* output queue full */
#define FCI_RSP_ERR_NCP   0x800F	/* no capability present */
#define FCI_RSP_ERR_TMOUT 0x8010	/* timed out */
#define FCI_RSP_ERR_HSA	  0x8012 	/* hot stall active */
#define FCI_RSP_ERR_SSE   0x8015	/* SSE hung */
#define FCI_RSP_ERR_FRAG  0x8016	/* needs fragmenting */

/*
 * Arguments for FCI_CMD_VERSION
 */
#define FCI_SW_VERSION		0 	/* microcode version */
#define FCI_HW_VERSION		1	/* hardware type code */
#define FCI_PLD_VERSION		2 	/* Altera rev for SSP, etc. */
#define FCI_ROM_VERSION         3       /* eprom version */

/*
 * Hardware version on FCI board -- response to FCI_HW_VERSION/FCI_CMD_VERSION.
 * Bill Palmer is the current keeper of the numbers.
 */
#define MCI_HARDWARE		1	/* returned from MCI */
#define SCI_HARDWARE		2	/* returned from SCI */
#define CBUS_HARDWARE		3	/* returned from FCI, cbus-1 */
#define FDDI_HARDWARE		4	/* FDDI-1, non-transparent */
#define MEC_HARDWARE		5	/* 6 port Ethernet */
#define CBUSII_HARDWARE		6	/* Cbus-2, FCI-2 */
#define FDDIT_HARDWARE		7	/* FDDI-2, transparent */
#define CBUS_HW_UNUSED		8	/* Reserved, currently unused, available */
#define CTR_HARDWARE		9	/* 4 port 16/4 Cbus Token Ring */
#define ULTRA_HARDWARE		10	/* Ultranet/DS3 */

/*
 * Arguments for FCI_CMD_MEMSIZE
 */
#define FCI_MEMSIZE_MEMA	0 	/* cache memory */
#define FCI_MEMSIZE_MEMD	1	/* main buffer memory */

/*
 * Types of interfaces supported by cBus controller.  This is the number
 * returned by the select command.  It is programmed into the interface
 * microcode.  Bill Palmer is the current keeper of the numbers.
 */
#define FCI_TYPE_OUT_OF_RANGE   ((ushort) 0xfffe)
#define FCI_TYPE_UNKNOWN        ((ushort) 0xffff)
#define FCI_TYPE_BASE_FLAG	0x8000
#define FCI_TYPE_ETHER		0
#define FCI_TYPE_SERIAL		1
#define FCI_TYPE_FDDI		2
#define FCI_TYPE_ULTRA		3
#define FCI_TYPE_HSSI		4
#define FCI_TYPE_CTR		5
#define FCI_TYPE_FDDIT		6
#define	FCI_TYPE_ATM		7
#define FCI_TYPE_ESA		8
#define FCI_TYPE_POS		9
#define FCI_TYPE_C5             10      /* yosemite ip for cat5k */
#define FCI_TYPE_MAX            10      /* the highest value of an FCI_TYPE_ */

/*
 * Definitions for word returned by FCI_CMD_CONDITIONS 
 */
#define FCI_COND_PANIC  0x8000		/* unusual condition detected */
#define FCI_COND_DCD	0x4000		/* carrier detect changed state */
#define FCI_COND_PHY_B	0x2000		/* status change on phy_b */
#define FCI_COND_PHY_A	0x1000 		/* status change on phy_a */
#define FCI_COND_FDDI_PHYA_JOIN	0x800	/* FDDIT, uCode CMT Join */
#define FCI_COND_FDDI_PHYB_JOIN	0x400	/* FDDIT, uCode CMT Join */
#define FCI_COND_FDDI_PHYA_SEND_BREAK 0x200
#define FCI_COND_FDDI_PHYB_SEND_BREAK 0x100
#define FCI_COND_FDDI_DUP_ADDRESS 0x80
#define FCI_COND_LOOP   0x0800          /* loopback is being requested */
#define FCI_COND_STATUS 0x0400          /* token ring status change */
#define FCI_COND_MASK	0x0001		/* count of TX buffers posted done */

/*
 * Argument to the FCI_CMD_ERRORS command.
 */
#define FCI_ERR_GIANTS		   0
#define FCI_ERR_NOBUFFERS	   1
#define FCI_ERR_ACQUIRE		   2
#define FCI_ERR_COLLISIONS	   3
#define FCI_ERR_RX_OVERRUN	   4
#define FCI_ERR_CRC		   5
#define FCI_ERR_FRAME		   6
#define FCI_ERR_RUNT		   7
#define FCI_ERR_TX_UNDERFLOW	   8
#define FCI_ERR_FDDI_LEM_A	   9
#define FCI_ERR_FDDI_LEM_B	  10
#define FCI_ERR_APPLIQUE	  11
#define FCI_ERR_PARITY		  12
#define FCI_ERR_RX_THROTTLE	  13
#define FCI_ERR_FRAMES_SEEN	  14
#define FCI_ERR_CLAIMS		  15
#define FCI_ERR_BEACONS		  16
#define FCI_ERR_MAC_GIANTS	  17
#define FCI_ERR_MAC_IGNORED	  18
#define FCI_ERR_RX_RFED		  19
#define FCI_ERR_RX_REDI		  20
#define FCI_ERR_RX_NULL_RCP	  21
#define FCI_ERR_RX_EXPLICIT_ABORT 22
#define FCI_ERR_RX_IMPLICIT_ABORT 23
#define FCI_ERR_TX_SFED		  24
#define FCI_ERR_TX_SEDI		  25
#define FCI_ERR_TX_FALSE_TOKEN	  26
#define FCI_ERR_TX_DMA_PARITY	  27
#define FCI_ERR_TX_NEXT_BUFFER	  28
#define FCI_ERR_TX_STRIP_ABORT	  29
#define FCI_ERR_TX_PTT_EXPIRED	  30
#define FCI_ERR_RX_RECOVER_RCP	  31

/* G.703 specific errors */
  
#define FCI_ERR_RX_AIS    32
#define FCI_ERR_RX_RAI    33
#define FCI_ERR_RX_LOS    34
#define FCI_ERR_RX_LOF    35

#define FCI_ERR_AIP_TX_DROP 36

/*
 * Address Filter constants -- really bits within an octet.
 *
 * STATION_ADDRESS and MULTICAST_ADDRESS must be bits 0 and 1.
 * The FDDI-T h/w expects them to be there.
 */
#define FCI_STATION_ADDRESS	0
#define FCI_MULTICAST_ADDRESS	1
#define FCI_BRIDGING_ADDRESS	2
#define FCI_MAC_ADDRESS		3	/* On token ring */
#define FCI_HSRP_STATION_ADDR	3	/* On ethernet, fddi */
#define FCI_DOD_IP_PROTOCOL	4
#define FCI_DECNET_PROTOCOL	5
#define FCI_XNS_PROTOCOL	6
#define FCI_CLNS_PROTOCOL	7

#define FCI_STATION_MASK		(1 << FCI_STATION_ADDRESS)
#define FCI_MULTICAST_MASK		(1 << FCI_MULTICAST_ADDRESS)
#define FCI_BRIDGING_MASK		(1 << FCI_BRIDGING_ADDRESS)
#define FCI_MAC_MASK			(1 << FCI_MAC_ADDRESS)
#define FCI_HSRP_STATION_MASK		(1 << FCI_HSRP_STATION_ADDR)
#define FCI_DOD_IP_PROTOCOL_MASK	(1 << FCI_DOD_IP_PROTOCOL)
#define FCI_DECNET_PROTOCOL_MASK	(1 << FCI_DECNET_PROTOCOL)
#define FCI_XNS_PROTOCOL_MASK		(1 << FCI_XNS_PROTOCOL)
#define FCI_CLNS_PROTOCOL_MASK		(1 << FCI_CLNS_PROTOCOL)

#define FCI_ISL_VLAN_MASK		3
 
#define FCI_PROTOCOL_BIT_MASK \
	(FCI_DOD_IP_PROTOCOL_MASK | FCI_DECNET_PROTOCOL_MASK | \
	 FCI_XNS_PROTOCOL_MASK | FCI_CLNS_PROTOCOL_MASK)

/*
 * To make the Address Filter addressing more constant over the different
 * media we choose Ether as our canonical media.  Page 0 is the first byte
 * of the DA field.  For FDDI-T we need to also address the FC byte that
 * is just prior to that field.  To do so we address it as page 0xFF.  So
 * the full address of the AF for the FC byte is 0xFF00.  The interface itself
 * takes care of any addressing modifications that might need to be done when
 * AF_SELECT is used.
 *
 * The FDDI-T also has a special RIF scanner built into the AF controller.
 * The scanner is controlled by the last page of the AF (see the FDDI-T's
 * alu.doc file for more details).
 */

#define FDDIT_AF_FC_PAGE	0xff00
#define FDDIT_AF_RIF_PAGE	0x1e00
#define   AF_RIF_FLRN		0x8000	/* upper 8 bits frwd local ring num */
#define   AF_RIF_FLRBN		0x4000	/* lwr 4 bits lrn, bridge num */
#define	  AF_RIF_FTRN		0x2000	/* upper 8 bits frwd targ ring num */
#define	  AF_RIF_FTRNE		0x1000	/* lwr 4 bits trn (end) */
#define	  AF_RIF_RTRN		0x0800	/* upper 8 bits rev targ ring num */
#define   AF_RIF_RTRBN		0x0400	/* lwr 4 bits trn, bridge num */
#define	  AF_RIF_RLRN		0x0200	/* upper 8 bits rev local ring num */
#define	  AF_RIF_RLRNE		0x0100	/* lwr 4 bits rev lrn (end) */

/*
 * Defines for testing the status returned by an IP
 */
#define IPSTAT_ADDR_MASK		0xff00
#define IPSTAT_STATION_MASK		(1 << (FCI_STATION_ADDRESS + 8))
#define IPSTAT_MULTICAST_MASK		(1 << (FCI_MULTICAST_ADDRESS + 8))
#define IPSTAT_BRIDGING_MASK		(1 << (FCI_BRIDGING_ADDRESS + 8))
#define IPSTAT_MAC_MASK			(1 << (FCI_MAC_ADDRESS + 8))
#define IPSTAT_HSRP_STATION_MASK	(1 << (FCI_HSRP_STATION_ADDR + 8))
#define IPSTAT_DOD_IP_PROTO_MASK	(1 << (FCI_DOD_IP_PROTO + 8))
#define IPSTAT_DECNET_PROTO_MASK	(1 << (FCI_DECNET_PROTO + 8))
#define IPSTAT_XNS_PROTO_MASK		(1 << (FCI_XNS_PROTO + 8))
#define IPSTAT_CLNS_PROTO_MASK		(1 << (FCI_CLNS_PROTO + 8))

/*
 * Autonomous switching statistic counters -- argument to FCI_CMD_STATISTICS.
 * IP
 */
#define FCI_IP_RX_PKTS		0
#define FCI_IP_RX_BYTES		1
#define FCI_IP_TX_PKTS		2
#define FCI_IP_TX_BYTES		3
#define FCI_IP_TX_DROPS		4
#define FCI_IP_CKS_ERRS		5

/*
 * SRB
 */
#define FCI_SRB_RX_PKTS		6
#define FCI_SRB_RX_BYTES	7
#define FCI_SRB_TX_PKTS		8
#define FCI_SRB_TX_BYTES	9
#define FCI_SRB_TX_DROPS	10

/*
 * IPX
 */
#define	FCI_IPX_RX_PKTS		11
#define	FCI_IPX_RX_BYTES	12
#define	FCI_IPX_TX_PKTS		13
#define	FCI_IPX_TX_BYTES	14
#define	FCI_IPX_TX_DROPS	15
#define	FCI_IPX_CKS_ERRS	16

/*
 * Autonomous Bridging
 */ 
#define	FCI_AB_RX_BYTES		17
#define	FCI_AB_TX_BYTES		18
#define	FCI_AB_TX_DROPS		19

/*
 * CLNP 
 */
#define FCI_CLNP_RX_BYTES	20
#define FCI_CLNP_RX_PKTS	21
#define FCI_CLNP_TX_BYTES	22
#define FCI_CLNP_TX_PKTS	23
#define FCI_CLNP_TX_DROPS	24

/*
 * Flags for flag word in mcitype structure.
 */
#define CBUS		0x0001
#define CBUSII		0x0002
#define CXBUS		0x0004
#define EOIR_INIT	0x0008

/*
 * MEMD constants -- used by CMT/PCM and autonomous switching.
 */
#define CBUS1_ICB_BASE	16
#define CBUS2_ICB_BASE  96
#define CBUS1_ICB_SIZE	8
#define CBUS2_ICB_SIZE	6

/*
 * RXSRBINFO REGISTER
 *
 * This register contains the output of the cBus controllers RIF scanner.
 */
#define FCI_RXSRB_PRESENT  0x8000	/* RIF present on this frame */
#define FCI_RXSRB_MATCHED  0x4000	/* Had a RIF Match */
#define FCI_RXSRB_DIR      0x2000	/* Direction Bit. */
#define FCI_RXSRB_EXPLORER 0x1000	/* Explorer Bit. */
#define FCI_RXSRB_RDOFFSET 0x0F00	/* RD offset (IN SHORTS) */
#define FCI_RXSRB_LENGTH   0x000F	/* length of RIF field (IN SHORTS) */

/*
 * Arguments for FCI_CMD_SET_MTU
 */
#define FCI_SET_IP_MTU		0
#define FCI_SET_IPX_MTU 	1
#define FCI_SET_CLNP_MTU	2

/* 
 * Define offsets into the cbus switching data structure. Probably should
 * be in if_fci.h, when we add additional protocols.
 */
#define CACHE_LINK		0
#define CACHE_VERSION		1
#define CACHE_MACSIZE		2
#define CACHE_OUTPUT		3
#define CACHE_DSTADR		4    
#define CACHE_MODE		9  /* IPX mode flags */
#define NO_CACHE_ENTRY     	FCI_RSP_ERR_NBF
#define CBUSI_CACHE_BUCKET_MAX  40 /* max cache entries per cbus bucket */
#define CBUSII_CACHE_BUCKET_MAX 58
#define SP_CACHE_BUCKET_MAX	68

#define	SNAP_SIZE	8	/* bytes */
#define	ETHER_ISO1_SIZE	5	/* bytes */
#define	FDDI_ISO1_SIZE	3	/* bytes */
#define	ISO2_SIZE	10	/* bytes */
#define	ISO3_SIZE	4	/* bytes */

#define FDDI_SNAP_SIZE	8	/* bytes */
#define TR_SNAP_SIZE	8	/* bytes */

#define SP_CCB_WORD_OFFSET 0x0020
#define SP_LOWCORE_WORDS   0x1000

#define FCI_SEND_IPC_SLAVE 130

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 * Externs for globals, and Prototypes for routines found in the various
 * if_<platform>.c files, especially if_ags_c7000.c and if_hes_common.c
 ***********************************************************************/

extern boolean cbus_debug;
extern ulong cbus_cache_allocated;
extern ushort cbus_switching_supported;

extern void print_cxbus_controllers(void);
extern void cbus_platform_registry_init(void);
extern int cbus_analyze(boolean, boolean);
extern void cbus_cache_init(void);
extern void cbus_interrupt(void);
extern void cbus_daughter_cmd(hwidbtype *, uint, uint);

extern int  fsip_get_txqlength(hwidbtype *);

extern boolean hotstall_flag;
extern boolean hotstall_smt_flag;
extern boolean oir_debug;
extern boolean eoir_capable;
extern watched_boolean* oir_process_wakeup;

#endif /* !defined(__IF_FCI_H__) */
