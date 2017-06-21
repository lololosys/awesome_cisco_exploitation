/* $Id: if_trip.h,v 3.3.60.4 1996/09/10 19:30:54 cmurphy Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_trip.h,v $
 *------------------------------------------------------------------
 * if_trip.h -- defines for the cBus Token Ring Interface Card
 *
 * August 1991, David R. Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved. 
 *
 *------------------------------------------------------------------
 * $Log: if_trip.h,v $
 * Revision 3.3.60.4  1996/09/10  19:30:54  cmurphy
 * CSCdi63700:  Token-ring int inserts into ring with bad
 * ring#, no warning.  Spyglass parameter structure is being
 * padded for alignment on RSP, causing us to read the
 * wrong values.  Fixed the alignment problem.
 *
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/06/01  06:15:30  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/05/29  03:46:03  skufer
 * CSCdi55758:  800E on TRIP steals MEMD from global queue, tql > tx queue
 * limit
 * Branch: California_branch
 *         TRIP on C7000 may advance tql when noisy ring causes
 *         frequent beacons.  Result is performance degradation.  When
 *         resetting the trip, new code prevents advancing the tql.  It
 *         is only a problem on c7000, not c7500 series.
 *
 * Revision 3.3.60.1  1996/03/18  19:42:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/14  00:56:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.26.1  1996/02/20  14:19:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:19:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:01:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/05  20:48:42  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.1  1995/06/07 20:42:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_TRIP_H__
#define __IF_TRIP_H__

/*
 * FCI complex, generic commands (0 - 31).  These are accessed by talking
 * to the cBus controller card directly.
 *
 * *** see if_fci.h ***
 */

/*
 * FCI complex, CTR specific commands (0 - 31).  These are accessed by
 * using the FCI_CTRLR_CMD and FCI_CTRLR_ARGUMENT commands to the cBus
 * controller card.
 */
#define FCI_CTR_VERSION			0
#define FCI_CTR_RESET			1
#define FCI_CTR_SELECT_INT		2
#define FCI_CTR_TX_START		7
#define FCI_CTR_MEMD_WRITE		9	/* Write */
#define FCI_CTR_MEMD_READ		10	/* Read */
#define FCI_CTR_MEMB_SELECT		11	/* Memb select */
#define FCI_CTR_MEMB_C_WRITE		12	/* Contention Write */
#define FCI_CTR_MEMB_C_READ		13	/* Contention Read */
#define FCI_CTR_POKE_CUETRC_ARG0	14	/*  */
#define FCI_CTR_MEMB_NC_WRITE		15	/* Non-Contention Write */
#define FCI_CTR_MEMB_NC_READ		16	/* Non-Contention Read */
#define FCI_CTR_MEMA_SELECT	        17	/* Select */
#define FCI_CTR_MEMA_WRITE		18	/* Write */
#define FCI_CTR_MEMA_READ		19	/* Read */
#define FCI_CTR_INIT			24	/* Spyglass RAM Test & Init */
#define FCI_CTR_SPYG_MEM_SELECT		25	/* Spyglass Memb select */
#define FCI_CTR_SPYG_MEM_WRITE		26	/* Spyglass Memb Write */
#define FCI_CTR_SPYG_MEM_READ		27	/* Spyglass Memb Read */
#define FCI_CTR_INIT_COMPLETE		28	/* Spyglass Init Cmplt Code */
# define FCI_INIT_RAM_ERR		0x8000	/* RAM Test vs. Initialize */
#define FCI_CTR_POKE_CUETRC_CMD		29	/* Command in SRB */
#define FCI_CTR_GET_CHECK_PTR		30	/* Get the Adapter Check Pointer */
#define FCI_CTR_READ_RING_STAT		31	/* Get the Spyglass Ring Status Word */

/*
 * FCI complex, CTR specific commands (32 - 63).  These are accessed by
 * giving the command directly to the cBus controller card, and letting
 * it forward the command on to the daughter card.
 */

#define FCI_CTR_STATUS			32	/* read and write controller status */
# define FCI_CTR_STATUS_THROTTLE	5
# define FCI_CTR_STATUS_UNTHROTTLE	6
#define FCI_CTR_STATION			33
#define FCI_CTR_AF_OR			39

#define FCI_CTR_MAC_HIGH		41	/* The interface's MAC address */
#define FCI_CTR_MAC_MIDDLE		42	/* The interface's MAC address */
#define FCI_CTR_MAC_LOW			43	/* The interface's MAC address */
#define FCI_CTR_SPYG_MEM_READ_RSLT	44	/* The result of a command 27 */

#define FCI_CTR_SET_PH_BIT		51	/* Turn on a PH bit */
#define FCI_CTR_RESET_PH_BIT		52	/* Turn off a PH bit */
#define FCI_CTR_AF_OUTPUT		53	/* Diag.  Set AF Output to this */
#define FCI_CTR_ACCEPT_MAC		54	/* Filter or receive MAC frames */
#define   MAC_PITCH  0
#define   MAC_ACCEPT 1
#define FCI_CTR_AF_SIZE			55	/* get size of address filter */
#define FCI_CTR_OPEN_RESULT		58	/* Result of TRC open command */

/*
 * Address Filter Definitions
 */
#define CTR_AF_PAGES 32
#define CTR_AF_WORDS (CTR_AF_PAGES * 0x100)

/*
 * Definitions for word returned by FCI_CMD_CONDITIONS 
 */
#define CTR_COND_DCD	       0x4000	/* carrier detect changed state */
#define CTR_COND_INIT_DONE     0x2000	/* Initialize command completed */
#define CTR_COND_SRB_COMPLETE  0x1000   /* open command has completed   */
#define CTR_COND_RING_STAT     0x0800   /* ring status has changed      */
#define CTR_COND_ADAPTER_CHECK 0x0400   /* self-forced spyglass crash      */
#define CTR_COND_ENABLE_COMP   0x0200   /*      */
#define CTR_COND_UCODE_CHECK   0x0100   /* Microcode dected a fatal error  */
#define CTR_COND_MASK	       0x0001	/* count of TX buffers posted done */

/*
 * RXSTATUS REGISTER
 *
 * The output of the address filter is what is returned in the upper half
 * of the RXSTATUS register.  See if_fci.h for these values (FCI_???_MASK).
 *
 * The lower half of this register is the same as the lower half of the
 * Spyglass Receive Status Register.
 */
#define CTR_RXSTAT_ROUTED 0xff00
#define CTR_RXSTAT_FS     0x00CC
#define CTR_RXSTAT_MAC    0x0010
#define CTR_RXSTAT_EXT    0x0002 /* External Match - source route bridged */
#define CTR_RXSTAT_INT    0x0001 /* Internal Match - one of our addresses */

#define CTR_RXSTAT_BOTH   0x0003
#define CTR_RXSTAT_SRB    0x0002 /* rif match or an explorer */
#define CTR_RXSTAT_MINE   0x0001 /* uni or broadcast mac match, (not multi?) */
#define CTR_RXSTAT_TB     0x0000


/**********************************************************************
 *
 *		       Spyglass data structures
 *
 **********************************************************************/

/*
 * The generic Token Ring Chip (Spyglass) to cisco micro engine data structure.
 */
typedef struct trc_cue_t_ {
    ushort valid;
    ushort command;
    ushort param0;
    ushort param1;
    ushort param2;
    ushort param3;
    ushort param4;
    ushort param5;
} trc_cue_t;

#define TRC_TO_CUE_ADDRESS 0x0900	/* bytes */
#define CUE_TO_TRC_ADDRESS 0x0910	/* bytes */
#define SPY_CMDBLK_START   0x0900	/* bytes */
#define SPY_CMDBLK_END     0x0920	/* bytes */
#define CTR_MEMB_SIZE      0x1000	/* bytes */

/*
 * The same data structure when initialization is completed.
 */
typedef struct trc_cue_init_t_ {
    ushort valid;
    ushort command;
    ushort completion;
    ushort extended_error;
    ushort srb_address;
    ushort arb_address;
    ushort expressb_address;
    ushort mac_txmt_address;
} trc_cue_init_t;

/*
 * The completion codes.
 */
#define SPYG_INIT_SUCCESS     0x0000
#define SPYG_INIT_FIRST	      0x0020
#define SPYG_INIT_PARITY      0x0020
#define SPYG_INIT_ROS	      0x0022
#define SPYG_INIT_RAM	      0x0024
#define SPYG_INIT_INSTRUCTION 0x0026
#define SPYG_INIT_INTERRUPT   0x0028
#define SPYG_INIT_PH	      0x002c
#define SPYG_INIT_LAST	      0x002c

/*
 * The same data structure when ram test is completed.
 */
typedef struct trc_cue_ramtest_t_ {
    ushort valid;
    ushort command;
    ushort completion;
    ushort reserved1;
    ushort reserved2;
    ushort reserved3;
    ushort reserved4;
    ushort reserved5;
} trc_cue_ramtest_t;

/*
 * The completion codes.
 */
#define SPYG_RAMTEST_SUCCESS     0x0000
#define SPYG_RAMTEST_INV_ADDR    0x0001
#define SPYG_RAMTEST_ADDR_ERR	 0x0002
#define SPYG_RAMTEST_PRTY_ERR	 0x0003
#define SPYG_RAMTEST_LAST	 0x0004

/**********************************************************************/

/*
 * The generic SRB
 */
typedef struct spyg_srb_t_ {
    uchar command;
    uchar reserved;
    uchar response;
} spyg_srb_t;

#define SPYG_SRB_INIT_COMP      0x80
#define SPYG_SRB_SET_SIGNAL	0x00
#define SPYG_SRB_READ_ERROR_LOG 0x02
#define SPYG_SRB_OPEN_ADAPTER	0x03
#define SPYG_SRB_CLOSE_ADAPTER  0x04
#define SPYG_SRB_SET_FUNCTIONAL 0x05
#define SPYG_SRB_SET_GROUP      0x06
#define SPYG_SRB_SET_BRIDGE     0x07
#define SPYG_SRB_START_TBRIDGE  0x08
#define SPYG_ARB_RING_STATUS    0xC0 /* phony */

#define SPYG_SRB_RETURN         0xFE

typedef struct spyg_srb_init_t_ {
    uchar  command;
    uchar  reserved[3];
    ushort revision_address;
    ushort adapter_address;
    ushort parms_address;
} spyg_srb_init_t;

typedef struct spyg_srb_signal_t_ {
    uchar  command;
    uchar  reserved;
    uchar  returnval;
} spyg_srb_signal_t;

typedef struct spyg_srb_errlog_t_ {
    uchar  command;
    uchar  reserved1;
    uchar  returnval;
    uchar  reserved3;
    uchar  error_log[14];
} spyg_srb_errlog_t;

typedef struct spyg_srb_open_t_ {
    uchar  command;
    uchar  reserved1;
    uchar  returnval;
    uchar  faulure;
    ushort options;
    uchar  address[6];
    uchar  product_id[18];
} spyg_srb_open_t;

#define SPYG_OPEN_WRAP           0x8000
#define SPYG_OPEN_NOHARD	 0x4000
#define SPYG_OPEN_NOSOFT	 0x2000
#define SPYG_OPEN_PASS_ADAPTER	 0x1000
#define SPYG_OPEN_PASS_ATTENTION 0x0800
#define SPYG_OPEN_CONTENDER      0x0100
#define SPYG_OPEN_PASS_BEACON    0x0080
#define SPYG_OPEN_NO_ETR         0x0010

#define SPYG_OPEN_OPTIONS        0x1900

typedef struct spyg_srb_close_t_ {
    uchar  command;
    uchar  reserved;
    uchar  returnval;
} spyg_srb_close_t;

typedef struct spyg_srb_functional_t_ {
    uchar  command;
    uchar  reserved1;
    uchar  returnval;
    uchar  reserved3;
    uchar  functional[4];
} spyg_srb_functional_t;

typedef struct spyg_srb_group_t_ {
    uchar  command;
    uchar  reserved1;
    uchar  returnval;
    uchar  reserved3;
    uchar  group[6];
} spyg_srb_group_t;

typedef struct spyg_srb_bridge_t_ {
    uchar  command;
    uchar  reserved1;
    uchar  returnval;
    uchar  reserved3;
    ushort bridge;		/* 0x0001 - 0x000F */
    ushort source_ring;		/* 0x0001 - 0x0FFF */
    ushort target_ring;		/* 0x0001 - 0x0FFF */
} spyg_srb_bridge_t;

typedef struct spyg_srb_transparent_t_ {
    uchar  command;
    uchar  reserved;
    uchar  returnval;
} spyg_srb_transparent_t;

#define SPYG_ERR_SUCCESS	0x00
#define SPYG_ERR_INV_CMD        0x01
#define SPYG_ERR_NOT_RUNNING    0x02
#define SPYG_ERR_CLOSED         0x03
#define SPYG_ERR_OPEN           0x04
#define SPYG_ERR_OPEN_FAILED    0x07
#define SPYG_ERR_INV_ADDR       0x08
#define SPYG_ERR_INV_GROUP      0x09
#define SPYG_ERR_INV_BRIDGE     0x0a
#define SPYG_ERR_TARGET_RING    0x0b
#define SPYG_ERR_SOURCE_RING    0x0c
#define SPYG_ERR_LAST           0x0d

/**********************************************************************/

/*
 * Spyglass Receive Command Register Bits
 */

#define SPYG_RCR_NRESET		0x8000	  
#define SPYG_RCR_NFRAQ		0x4000	  
#define SPYG_RCR_INSERT		0x2000	  
#define SPYG_RCR_NWRAP		0x1000	  
#define SPYG_RCR_ACCLB		0x0800	/* enable spanning */
#define SPYG_RCR_ACCRI		0x0400	/* enable bridging */
#define SPYG_RCR_RESERVED	0x0200	  
#define SPYG_RCR_RXEN		0x0100	  
#define SPYG_RCR_NOCHAIN	0x0080	  
#define SPYG_RCR_IGNCTL		0x0040	  
#define SPYG_RCR_ROPT0		0x0020  /* copy all frames */
#define SPYG_RCR_RPOT1		0x0010	/* copy only express MAC */
#define SPYG_RCR_IGN0		0x0008	  
#define SPYG_RCR_IGN1		0x0004	  
#define SPYG_RCR_IGN2		0x0002	  
#define SPYG_RCR_IGN3		0x0001	  
#define SPYG_RCR_IGN_MASK	0x000F	  

/**********************************************************************/

/*
 * Various data structures as maintained in the spyglass.
 */

typedef struct spyg_addrs_t_ {
    uchar mac[6];
    uchar group[6];
    ulong functional;
} spyg_addrs_t;

typedef struct spyg_parms_t_ {
    ulong  physdrop;		   /* adapters physical address          */
    uchar  una[6];		   /* NAUN specific address		 */
    ulong  PACKED(una_physdrop);   /* NAUN physical address		 */
    uchar  last_poll[6];	   /* Last Poll specific address	 */
    uchar  rsrvd[2];
    ushort txmt_pri; 		   /* maximum transmit access priority 	 */
    ushort auth_class;		   /* source class authorization	 */
    ushort last_attn;		   /* last attention code		 */
    uchar  last_src[6];		   /* last source address		 */
    ushort last_beacon_type;	   /* last beacon type			 */
    ushort last_major_vector;	   /* last major vector			 */
    ushort ring_status;		   /* current ring status		 */
    ushort soft_err_timer;	   /* soft error timer value		 */
    ushort ring_xfc_errors;	   /* front end error counter		 */
    ushort local_ring;		   /* ring number			 */
    ushort monitor_error_code;	   /* monitor error code		 */
    ushort beacon_txmt_type;	   /* beacon transmit type		 */
    ushort beacon_rcv_type;	   /* beacon receive type		 */
    ushort frame_corr;		   /* frame correlator save area	 */
    uchar  beacon_una[6];	   /* beaconing station NAUN		 */
    uchar  rsrvd2[4];
    ulong  PACKED(beacon_una_physdrop);	   /* beaconing station physical address */
} spyg_parms_t;

typedef struct spyg_error_log_t_ {
    uchar line;
    uchar internal;
    uchar burst;
    uchar arifci;
    uchar abort;
    uchar reserved6;
    uchar lostframe;
    uchar rcvcongestion;
    uchar copy;
    uchar freq;
    uchar token;
    uchar reserved11;
    uchar reserved12;
    uchar reserved13;
} spyg_error_log_t;

/**********************************************************************/

#define SPYG_VERS_LEN 10
#define SPYG_PRODUCTID_LEN 18

enum INIT_PHASE {
    INIT_NONE,
    INIT_DISABLE,
    INIT_INIT,
    INIT_ENABLE,
    INIT_OPEN,
    INIT_OPEN_COMPLETE
};
    
struct spyglass_data_t_ {
    ushort  last_cmd;	     /* last command from whatever */
    ushort  last_rslt;	     /* last rslt from whatever  */
    ulong   last_p1;	     /* param, usually failure code */
    sys_timestamp init_timer;    /* TRIP initialization timeout timer */

    ushort  srb_addr;
    ushort  arb_addr;
    ushort  expressb_addr;
    ushort  mac_tcb_addr;
    ushort  revision_addr;
    ushort  adapter_addr;
    ushort  parms_addr;

    uchar   spyglass_rev[SPYG_VERS_LEN];
    spyg_addrs_t addrs;
    spyg_parms_t smt;

    boolean cmd_in_progress;
    sys_timestamp cmd_timeout;	/* timer-in-the-future */
    boolean set_functional_needed;
    boolean set_functional_only;
    ushort  last_open_options;
    ushort  conditions;
    int     init_phase;

    ulong   err_line;
    ulong   err_internal;
    ulong   err_burst;
    ulong   err_arifci;
    ulong   err_abort;
    ulong   err_reserved6;
    ulong   err_lostframe;
    ulong   err_rcvcongestion;
    ulong   err_copy;
    ulong   err_freq;
    ulong   err_token;
    ulong   err_reserved11;
    ulong   err_reserved12;
    ulong   err_reserved13;

    ulong prev_runts;
    ulong prev_giants;
    ulong prev_input_overrun;
    ulong prev_input_resource;
    ulong prev_output_underrun;
    ulong prev_parity;

    ulong mac_giants;	            /* no. mac too large packets received */
    ulong mac_ignored;	            /* no. mac dropped because MAC hw busy */
    ulong rx_rfed;		    /* no. frames with EDI bit set */
    ulong rx_redi;		    /* no. frames with EDI bit set */
    ulong rx_null_rcp;	    	    /* no. times RCP went to zero (IBM chip) */
    ulong rx_recover_rcp;	    /* no. times RCP went to zero (ucode) */
    ulong rx_explicit_abort;        /* no. explicit aborts seen on ring */
    ulong rx_implicit_abort;        /* no. implicit aborts seen on ring */
    ulong tx_sfed;
    ulong tx_sedi;
    ulong tx_false_token;
    ulong tx_dma_parity;	    /* no. PH DMA parity errors on transmit */
    ulong tx_next_buffer;	    /* no. times TCP went to zero */
    ulong tx_strip_abort;
    ulong tx_ptt_expired;

    ulong prev_mac_giants;	    /* no. mac too large packets received */
    ulong prev_mac_ignored;	    /* no. mac dropped because MAC hw busy */
    ulong prev_rx_rfed;		    /* no. frames with EDI bit set */
    ulong prev_rx_redi;		    /* no. frames with EDI bit set */
    ulong prev_rx_null_rcp;	    /* no. times RCP went to zero (IBM chip) */
    ulong prev_rx_recover_rcp;	    /* no. times RCP went to zero (ucode) */
    ulong prev_rx_explicit_abort;   /* no. explicit aborts seen on ring */
    ulong prev_rx_implicit_abort;   /* no. implicit aborts seen on ring */
    ulong prev_tx_sfed;
    ulong prev_tx_sedi;		
    ulong prev_tx_false_token;
    ulong prev_tx_dma_parity;	    /* no. PH DMA parity errors on transmit */
    ulong prev_tx_next_buffer;	    /* no. times TCP went to zero */
    ulong prev_tx_strip_abort;
    ulong prev_tx_ptt_expired;
    ulong prev_tx_cumbytes;	    /* total bytes sent */
    ulong prev_rx_cumbytes;	    /* total bytes received */	
};

#define TR_POLLING_OK		0
#define TR_POLLING_TMOUT 	1
#define TR_POLLING_PERIOD	8 	/* eight seconds */

#define SPY_MAX_CMD_TIME	(30*ONESEC) /* time to allow cmds to complete */
#define SPY_SRB_MAX_OPEN_TIME	(60*ONESEC) /* time to allow SRB to complete */
#define SPY_MAX_INIT_TIME       (90*ONESEC) /* time to allow init to complete */

/*
 * Protocol Handler definitions
 */
#define SPYGLASS_DEADMAN_TIMER_ERROR_ENABLE 0x4000
#define SPYGLASS_PH_INTCTL 0xfe94	/* Interrupt Control Register */


/**********************************************************************
 *
 * Function prototypes for functions in if_trip.c
 *
 **********************************************************************/
boolean ctr_set_addresses(hwidbtype *);
void ctr_get_mib_stats(hwidbtype *);
boolean ctr_show_controller(hwidbtype *, parseinfo *);
void cbus_token_periodic(hwidbtype *);
boolean cbus_token_reset(hwidbtype *);
void cbus_token_shutdown(hwidbtype *, int);
void ctr_conditions(hwidbtype *, ushort);
void ctr_deadman_poll(hwidbtype *);
void ctr_modify(hwidbtype *, ulong, ulong);
void ctr_setup(hwidbtype *);
void ctr_update_prev_stats(hwidbtype *);

extern void ctr_memb_setup(hwidbtype *, ushort );
extern void trip_cbus_mci_txlimit(hwidbtype *);
extern void trip_platform_init(subsystype*);
extern void trip_restart_init(hwidbtype *);
extern ushort platform_trip_anybody_out_there(hwidbtype *);
extern void ctr_polling_failed (hwidbtype *);


#endif   /* __IF_TRIP_H__ */
