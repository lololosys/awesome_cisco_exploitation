/* $Id: atm_oam.h,v 3.2.58.1 1996/08/03 23:17:35 fox Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_oam.h,v $
 *------------------------------------------------------------------
 * A T M _ O A M . H
 *
 * January 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_oam.h,v $
 * Revision 3.2.58.1  1996/08/03  23:17:35  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2  1995/11/17  08:45:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/10  00:59:55  awu
 * CSCdi37887:  ATM OAM cells are transmitted and received incorrect.
 *
 * Revision 2.1  1995/06/07  20:11:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	AIP_DM_PKT_OAM_VC	0x0200		/* OAM info. from VC table */
#define	AIP_DM_PKT_OAM_DM	0x0300		/* OAM info. from PTI */

/*
 * Define the Common OAM cell format - F4 & F5 cells
 * 	For F4 cells:
 *		VPI == User VPI
 *		VCI == (3 == Segment), (4 == End-to-End)
 *		OAM_TYPE = (0001 == Fault management)
 *		OAM_FUNC == (0000 == AIS, 0001 == FERF, 0010 == loopback)
 *	For F5 cells:
 *		VPI == User VPI
 *		VCI == User VCI
 *		PT == (100 == Segment, 101 == End-to-End)
 *		OAM_TYPE = (0001 == Fault management)
 *		OAM_FUNC == (0000 == AIS, 0001 == FERF, 1000 == loopback)
 */
#define	ATM_OAM_FAULT_MGMT	0x1		/* OAM Fault mgmt. code */

#define	ATM_OAM_AIS_FUNC	0x0		/* AIS function type */
#define	ATM_OAM_FERF_FUNC	0x1		/* FERF function type */
#define	ATM_OAM_LOOP_FUNC	0x8		/* Loopback function type */
#define ATM_OAM_LOOPIND		0x1	 	/* OAM LOOPBACK INDICATION */

#define	ATM_OAM_F5_SEGMENT	0x4		/* Segment function */
#define	ATM_OAM_F5_ENDTOEND	0x5		/* End-to-End function */
#define	ATM_OAM_F4_SEGMENT	0x3		/* Segment function */
#define	ATM_OAM_F4_ENDTOEND	0x4		/* End-to-End function */

typedef struct	atm_oam_hdr_t_ {
    unsigned	oam_gfc:4;			/* GFC */
    unsigned	oam_vpi:8;			/* VPI */
    unsigned    oam_vci_ms:4;		  /* VCI (Most Significant Bits) */

    unsigned	oam_vci_ls:12;  	  /* VCI (Least Significant Bits) */
    unsigned	oam_pt:3;			/* Payload Type */
    unsigned	oam_clp:1;			/* Cell Loss Priority */
    uchar	data[0];
} atm_oam_hdr_t;

typedef struct	atm_oam_type_func_t_ {
    uchar	oam_type:4;
    uchar	oam_func:4;
    uchar	data[0];
} atm_oam_type_func_t;

#define ATM_VCI_MS_SHFT		12
#define ATM_VCI_LS_MASK		0x0FFF
#define ATM_GET_VCI_MS(vci)	 ((vci)>>ATM_VCI_MS_SHFT)
#define ATM_GET_VCI_LS(vci)	((vci)&ATM_VCI_LS_MASK)
#define ATM_CONSTRUCT_VCI(vci_ms, vci_ls) \
    (((vci_ms)<<ATM_VCI_MS_SHFT) | (vci_ls))

#define	ATM_OAM_CELL_SIZE	53
#define ATM_CELLHDR_HEC_BYTES	1
#define	ATM_CELL_ENCAPBYTES	4		/* CELL header bytes-1 */
#define	ATM_OAM_ENCAPBYTES	1		/* OAM header bytes */

#define	ATM_OAM_FILL		0x6A		/* UNI3.x specifies this */
#define	ATM_OAM_FILLBYTES	16		/* No. of 6A FILL bytes */

#define	ATM_OAM_UNUSEDBYTES	12		/* No. of FF UNUSED bytes */

/*
 * Define the functions specific field breakdown
 */
#define	ATM_AIS_FAIL		0x6A
typedef	struct	atm_ais_ferf_func_t_ {
    uchar	failure;			/* Failure type */
    uchar	fail_loc[9];			/* Failure location */
} atm_ais_ferf_t;


typedef struct	atm_loopback_func_t_ {
    uchar	loop_ind;			/* Loopback indication */
    uchar	ctag[4];			/* Correlation tagg */
    uchar	loc_id[12];			/* Loopback location ID */
    uchar	src_id[12];			/* Source ID */
    uchar	oam_fill[16];			/* Fill area == 0x6A */
} atm_loopback_t;

/*
 * EXTERNs
 */
extern void atm_oam_init (void);
extern void atm_oam_enable_timer(vc_info_t *, uint);
extern void atm_oam_disable_timer(vc_info_t *);

extern watched_queue *atm_oam_packetQ;       /* OAM packets */
