/* $Id: fddi_private.h,v 3.1.42.3 1996/06/18 20:05:36 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/if/fddi_private.h,v $
 *------------------------------------------------------------------
 * fddi_private.h -- Fiber Distributed Data Interface private defs
 *
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi_private.h,v $
 * Revision 3.1.42.3  1996/06/18  20:05:36  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.1.42.2  1996/03/21  22:48:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.42.1  1996/03/18  20:13:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/01/24  22:18:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  23:12:37  gstovall
 * Placeholder for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FDDI_PRIVATE_H__
#define __FDDI_PRIVATE_H__

#define MAXSNAPFDDISIZE		4470

#define FDDI_DELAY		10
#define VISIBLE_FDDI_BANDWIDTH 100000

#define FDDI_HOLDQ		40

#define FDDI_ARP_GRATUITOUS_HOLD	10

#define DEFAULT_T_MIN		(T_MIN_SYMBOLS / 2)
#define DEFAULT_T_MAX		(T_MAX_SYMBOLS / 2)

#define XX_DEFAULT_TVX         3400
#define XX_DEFAULT_TRT         5000
#define XX_TVXSCALE            100        /* see tvxtable in fddi_xx.c */
#define XX_MIN_TVX             2500

#define DEFAULT_T_MIN_TIME	4000		/* 4000 microseconds */
#define DEFAULT_T_MAX_TIME	165000		/* 165000 microseconds */
#define DEFAULT_TRT_TIME	5000		/* 5000 microseconds */

#define DEFAULT_TRT	        ((FDDI_BYTES_SECOND / 1000) * \
				 (DEFAULT_TRT_TIME / 1000))
#define CISCO_DEFAULT_TMAX		((FDDI_BYTES_SECOND / 1000) * \
					 (DEFAULT_T_MAX_TIME / 1000))
#define CISCO_DEFAULT_TMIN		((FDDI_BYTES_SECOND / 1000) * \
					 (DEFAULT_T_MIN_TIME / 1000))

#if !defined(XX) & !defined(SIERRA)

/*
 * Default values for the FDDI (formac) controller.
 */
#define FDDI_FORMAC_BASE_MASK	0x1600
#define FDDIT_FORMAC_BASE_MASK	0x0000
#define FDDI_DUP_ADDR_MASK	0x0038  
#define FDDIT_DUP_ADDR_MASK     0x001e

#define FM_MODE_ON_LINE		0x6000
#define FM_MODE_OFF_LINE	0x0000
#define FM_MODE_DELADET		0x0800
#define FM_MODE_NORECV		0x0400
#define FM_MODE_DA_EQ_MA	0x0000
#define FM_MODE_DA_AND_SA_EQ_MA	0x0100  
#define FM_MODE_PROMISCUOUS	0x0200
#define FM_MODE_RB_BUS		0x0000    
#define FM_MODE_RA_BUS        	0x0020
 
#define FDDI_MODE		(FM_MODE_ON_LINE | FM_MODE_DA_EQ_MA)
#define FDDIT_MODE		(FM_MODE_ON_LINE | FM_MODE_PROMISCUOUS | \
				 FM_MODE_DELADET)

#endif

#define DEFAULT_PHY_A_TVAL	0x0008
#define DEFAULT_PHY_B_TVAL	0x020C

#define FDDI_LOOPBACK_DELAY     50
#define FDDI_RING_OP            0x8000


#define FDDI_DEFAULT_BURST_COUNT	3

/*
 * Masks to get at parts of the FC field in FDDI packet:
 */
#define FC_SYNCHRONOUS	0x80
#define FC_L_MASK	0x40
#define FC_FF_MASK	0x30
#define FC_FF_SHIFT	4
#define FC_RZZZ_MASK	0xF

/*
 * Contents of the FF (format bits).
 */ 
#define FDDI_FF_SMT_MAC_FRAME			0    
#define FDDI_FF_LLC_FRAME			1
#define FDDI_FF_RESERVED_IMPLEMENTOR_FRAME	2
#define FDDI_FF_RESERVED_FUTURE_FRAME		3		

/*
 * Constants used to constuct the claim beacon buffer.
 */
#define FDDI_CLAIM_BEACON_BUFFER_SIZE	100	/* number of bytes */

/*
 * The offsets are ALL short word offsets.
 */
#define FDDI_CLAIM_LENGTH_OFFSET	0
#define FDDI_CLAIM_LENGTH		17
#define FDDI_BEACON_LENGTH_OFFSET	20
#define FDDI_BEACON_LENGTH		17
#define FDDI_CLAIM_FRAME_CODE_OFFSET	2
#define FDDI_CLAIM_FRAME_CODE		0xC3
#define FDDI_BEACON_FRAME_CODE_OFFSET	22
#define FDDI_BEACON_FRAME_CODE		0xC2
#define FDDI_BEACON_SA_OFFSET		25
#define FDDI_DIRECT_BEACON_CODE_OFFSET  40

#define SMT_OVERHEAD		4

/*
 * SMT header type.
 */
typedef struct smthdrtype_ {
    uchar frame_class;
    uchar frame_type;
    ushort version_id;
    ulong transaction_id;
    uchar station_id[8];
    ushort pad;
    ushort info_field_length;
} smthdrtype;


#define PHY_A		0
#define PHY_B		1

#define T_OUT		100	/* milliseconds	*/
#define TB_MIN		5	/* milliseconds */
#define C_MIN		2	/* milliseconds - really s/b 1.6 ms */
#define TB_MAX		50	/* milliseconds	*/

#define TL_MIN		50 	/* microseconds */

#define LS_MIN		480 	/* nanoseconds */

struct phytype_ {
    hwidbtype *idb;
    mciregtype *regptr;
    ulong  pc_unit;
    ushort pc_state;
    ulong tval;
    ulong rval;
    boolean connect;
    ulong lem_ct;
    ushort status_offset;
    char type;
    ushort LEMThreshold;
    ushort HWLEMErrorCt;
};

/*
 * ENDEC status register values. See AMD Data Book.
 */
#define LSU_STATUS	0
#define NLS_STATUS	1
#define MLS_STATUS	2
#define ILS_STATUS	3
#define HLS_STATUS	4
#define QLS_STATUS	5
#define ALS_STATUS	6
#define OVUF_STATUS	7

/*
 * Convert status register values to bit positions.
 */
#define B_LSU	(1 << LSU_STATUS)
#define B_NLS	(1 << NLS_STATUS)
#define B_MLS	(1 << MLS_STATUS)
#define B_ILS	(1 << ILS_STATUS)
#define B_HLS	(1 << HLS_STATUS)
#define B_QLS	(1 << QLS_STATUS)
#define B_ALS	(1 << ALS_STATUS)
#define B_OVUF	(1 << OVUF_STATUS)

/*
 * The FDDIT fddi control register is similar to the
 * FDDI's hold_reg.  It is written using the Set Hold Register
 * command.
 *
 * The register looks like this:
 *
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 * |  x  |  x  |  x  | HSM | BRG |  x  | Rx1 | Rx0 |
 * +-----+-----+-----+-----+-----+-----+-----+-----+
 *
 * HSM enables hear self which we currently do not implement.
 * BRG turns on bridge stripping.
 * Rx1,0 determines the receive offset.
 * 
 */

#define FDDIT_FCTL_BRIDGE		0x08

/*
 * Phy type -- indicated via the PCM signalling process.
 */
#define PC_TYPE_MASK                    0x6

#define PC_TYPE_A                       0x0
#define PC_TYPE_B                       0x4
#define PC_TYPE_S                       0x2
#define PC_TYPE_M                       0x6

/*
 * FDDI subblock definition
 */
typedef struct fddisb_ {
    /*
     * Generic interface statistics. Note that these are duplicated
     * below for the users who like to "clear counters" for interactive
     * work. If you change/add/delete one of these, check to see if you
     * need to do the same to one of the "si_" prefixed variables below.
     * (sigh).
     */
    ulong fddi_claims;			/* no. of times claim state entered */
    ulong fddi_beacons;			/* no. of times beacon state entered */
    ulong fddi_trace_count;		/* no. of times trace state entered */
    ulong fddi_deferred_cmt;		/* CMT start is needed for this FDDI */

    /* 
     * These are the duplicates of the above for interactive work. They
     * contain the values of the variables as of the last time "clear
     * counter" was done for this interface. (The "si_" prefix stands for
     * "show interface"
     */
    ulong si_fddi_claims;		/* no. times claim state entered */
    ulong si_fddi_beacons;		/* no. times beacon state enterd */
    ulong si_fddi_trace_count;		/* no. of trace state entered */

    ulong fddi_trt;			/* requested token rotation time */
    ulong fddi_trt_neg;			/* actual (negotiated) target trt */
    int fddi_burst_count;		/* Configuration option */
    ulong fddi_frinc;			/* Frame counter -- SMT garbage */
    ulong fddi_tvx;			/* valid_transmission timer */
    sys_timestamp fddi_operational;	/* time since ring when operational */
    ushort cfm_state;			/* configuration management state */
    ushort fddi_shutdown_reasons;	/* indicates reasons for shutdown */

    /*
     * Vector to hold pre-existing state/config vector which was
     * in place before we initialized a FDDI IDB as a FDDI IDB.
     * If the FDDI state/config vector is not able to handle one
     * or more items, that itemlist should be passed to the "super"
     * function.
     */
    hwidb_state_config_t	super_state_config;
} fddisb;

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern pid_t smt_process_id;
const char *get_phy_pc_status(hwidbtype *, char);
void fddi_idb_init(hwidbtype *);
process fddi_smt_input(void);
void fddi_periodic(hwidbtype *idb);
extern void fddi_interface(hwidbtype *);
extern void fddi_parser_init(void);
extern void cmt_connect(hwidbtype *idb, int which);
extern void cmt_disconnect(hwidbtype *idb, int which);

#endif /* __FDDI_PRIVATE_H__ */
