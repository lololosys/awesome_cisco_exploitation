/* $Id: if_bri_quicc.h,v 3.3 1995/11/17 18:39:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-36/if_bri_quicc.h,v $
 *------------------------------------------------------------------
 * if_bri_quicc.h - ISDN BRI parameter specific to the QUICC Chip.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_bri_quicc.h,v $
 * Revision 3.3  1995/11/17  18:39:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  04:36:34  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:51:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_BRI_QUICC_H__
#define __IF_BRI_QUICC_H__

/*
 * BRI instance structure
 */
typedef struct bri_instance {

    /*
     * Driver defined data structure.
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    COMMON_DRIVER_INSTANCE

    /*
     * Fields common across interface types (but controller
     * specific).
     */

    QUICC_DRIVER_INSTANCE

    /* 
     * Bri specific fields.
     */
    uchar last_sid;                /* Last status seen in SID */
    boolean nt_one;		   /* integrated NT1 for this interface */
} bri_instance_t;

/*
 * QUICC buffer descriptor flags for BRI.  They do not
 * include the wrap or ready/empty bits.
 */
#define QUICC_BRI_TX_FLAGS	(QUICC_BDSTAT_TX_INT|QUICC_BDSTAT_TX_LAST| \
				 QUICC_BDSTAT_TX_TRCRC)
#define QUICC_BRI_RX_FLAGS	QUICC_BDSTAT_RX_INT

#define QUICC_BRI_RX_ERR   	(QUICC_BDSTAT_RX_DERR |  	\
				 QUICC_BDSTAT_RX_FLV |   	\
				 QUICC_BDSTAT_RX_NOFA |  	\
				 QUICC_BDSTAT_RX_ABORT | 	\
				 QUICC_BDSTAT_RX_CRCERR | 	\
				 QUICC_BDSTAT_RX_OVRRUN	|	\
				 QUICC_BDSTAT_RX_CDLOST)

#define QUICC_BRI_RX_MASK	(QUICC_BDSTAT_RX_PKT | QUICC_BRI_RX_ERR)

#define TP3420_CMD_LEN		0x0001

/*
 * More descriptive names for TP3420 chip select.
 */
#define TP3420_ASSERT		QUICC_CLR_PB4
#define TP3420_NEGATE		QUICC_SET_PB4

/*
 * ISDN bandwidths.
 */
#define QUICC_BANDWIDTH_64	64
#define QUICC_BANDWIDTH_56	56
#define QUICC_BANDWIDTH_16	16

#define QUICC_TP3420_AVEC4	28

#define QUICC_HDLC_CRC_SIZE	2

/*
 * Defines for BRI timers
 */
#define QUICC_TIME_8S    49080  /* 8 second timeout period */
#define QUICC_TIME_600MS  3680  /* 600 ms timeout period */
#define QUICC_TIME_2MS      12  /* 2 ms timeout period */

/*
 * Prototypes
 */
uchar quicc_access_sid(quicc_dpr_t *, uchar);
void quicc_bri_set_bandwidth(quicc_dpr_t *, ulong, ulong);
void quicc_init_bri(hwidbtype *);
void quicc_t3_timer_clear(quicc_dpr_t *);
void quicc_t4_timer_clear(quicc_dpr_t *);
void quicc_2ms_timer_clear(quicc_dpr_t *);
boolean quicc_bri_show_controller (hwidbtype *, ulong);
void quicc_3420_intr_clear(quicc_dpr_t *);
#endif
