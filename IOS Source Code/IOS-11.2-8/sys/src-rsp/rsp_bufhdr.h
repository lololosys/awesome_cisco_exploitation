/* $Id: rsp_bufhdr.h,v 3.5.4.2 1996/08/15 15:32:01 skufer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_bufhdr.h,v $
 *------------------------------------------------------------------
 * BufferinII buffer headers in MEMD
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_bufhdr.h,v $
 * Revision 3.5.4.2  1996/08/15  15:32:01  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.5.4.1  1996/07/17  01:45:13  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.5  1996/03/11  11:09:27  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.4  1996/01/09  03:57:33  skufer
 * CSCdi37013:  Unused macros should be removed
 * Removed unused macros like BUFHDR_NUM_TO_ADDR().
 *
 * Revision 3.3  1995/11/26  08:18:51  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              ISL vLAN support for the RSP platform.
 *
 * Revision 3.2  1995/11/17  18:47:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/17  20:55:13  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/06/07  22:57:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __RSP_BUFHDR_H__
#define __RSP_BUFHDR_H__

#define MEMD_BUFHDR_OFFSET	8192      /* start of hdrs byte off */
#define MEMD_BUFDATA_OFFSET	(64*1024)

#define MEMD_BUFHDR_SIZE	(MEMD_BUFDATA_OFFSET - MEMD_BUFHDR_OFFSET)


/*
 * Note that the FIP buffer header defines are here because the FIP
 * hardware implements this scheme, so you should see it before you
 * touch the buffer header
 */

typedef struct bufhdr_t_ {		/* MEMD buffer header */
    union {
	struct {
	    ushort Status;		/* Dst/Src hash on FIP */
	    ushort Bytes;		/* Also info bits on FIP */
	} Rx;
	ulong Tx_Ptr;			/* MEMD relative */
    } bf_lword0;
    ushort bf_rxstatus2_or_txbytes_or_fipstatus;
    ushort bf_queue_ptr;		/* MEMD relative */
    union {
	struct {
	    ushort Free_Queue_Ptr;	/* original free queue */
	    ushort Fipsrb_or_Bcastid;		/*  */
	} Ret;
	ulong Ret_Info;
    } bf_lword2;
    ulong bf_buffer_ptr;		/* MEMD relative */
} bufhdr_t;

#define bf_rx_status	bf_lword0.Rx.Status
#define bf_rx_bytes	bf_lword0.Rx.Bytes
#define bf_tx_ptr	bf_lword0.Tx_Ptr
#define bf_rx_status2	bf_rxstatus2_or_txbytes_or_fipstatus
#define bf_tx_bytes	bf_rxstatus2_or_txbytes_or_fipstatus
#define bf_fip_status	bf_rxstatus2_or_txbytes_or_fipstatus
#define bf_fip_srb	    bf_lword2.Ret.Fipsrb_or_Bcastid

#define bf_bcast_id	bf_lword2.Ret.Fipsrb_or_Bcastid
#define bf_idb_info	bf_bcast_id
#define bf_freeq_ptr	bf_lword2.Ret.Free_Queue_Ptr
#define bf_unused       bf_freeq_ptr	
#define bf_return_info	bf_lword2.Ret_Info


#define TX_PTR_MASK		0x00ffffff
#define TX_PTR_HEARSELF_EIP	0x80000000 /* "HEARSELF" bit, EIP-only */
#define TX_PTR_QUEUEINFO	0x0f000000 /* reserved for pri. queue info */

#define TX_PTR_DO_ISL_INNER_CRC	0x40000000 /* Used to indicate to the FEIP that
					      the transmit buffer contains an
					      ISL vLAN datagram requiring CRC
					      generation. (refer vlan/vlan.h) */


/*----------------------------------------------------------------------------*
 * Constants for doing some checking on buffer headers
 *----------------------------------------------------------------------------*/
#define MIN_BUFHDR_ADDR  MEMD_BUFHDR_BASE_ADDR
#define MAX_BUFHDR_ADDR  (MEMD_BUFHDR_BASE_ADDR + MEMD_BUFHDR_SIZE)

/*----------------------------------------------------------------------------*
 * Buffer Header Macros to convert from one format to another
 *
 * There are three formats:
 *   ADDR (address) -- the full 32-bit pointer to the buffer header
 *   OFF  (offset) -- the lower 16-bits of the address that can be
 *                    used as an offset into MEMD
 *   NUM (number) -- the ordinal number of the buffer header as if
 *                   they were allocated as an array of bufhdr_t
 *                   starting from the beginning of MEMD (note that
 *                   the first usable bufhdr_t is not located at the
 *                   start of MEMD -- the first 256 (approx) are used
 *                   to access the QA ASIC).
 *----------------------------------------------------------------------------*/
#define BUFHDR_SIZE        sizeof(bufhdr_t)
#define BUFHDR_SIZE_SHIFT  4			/* log2(BUFHDR_SIZE) */
#define BUFHDR_SIZE_MASK   ~(BUFHDR_SIZE_SHIFT - 1)

#define BUFHDR_ADDR_TO_OFF(_bha) \
	((ushort) ((ulong)(_bha) & 0x0000ffff))

/*
 * Prototypes
 */

extern bufhdr_t		*bufhdr_base;
extern uint		bufhdr_count;

/* Cache flushing and cache invalidating routines */

extern void inval_buf_hdr(bufhdr_t *);


extern bufhdr_t* get_memd_buffer(int);
extern bufhdr_t* get_ipc_memd_buffer(ulong);

#endif /* ifndef __RSP_BUFHDR_H__ */
