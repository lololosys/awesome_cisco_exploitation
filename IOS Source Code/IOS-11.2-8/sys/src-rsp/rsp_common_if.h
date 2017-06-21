/* $Id: rsp_common_if.h,v 3.2.58.1 1996/07/08 22:52:37 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_common_if.h,v $
 *------------------------------------------------------------------
 * rsp_common_if.h -- Prototypes for rsp_common_if.c
 *
 * September, 1989,  Robert Fletcher
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_common_if.h,v $
 * Revision 3.2.58.1  1996/07/08  22:52:37  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:48:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__RSP_COMMON_IF_H__)
#define __RSP_COMMON_IF_H__

/*
 * REQUIRED INCLUDES:
 *     if_fci.h
 *     interface.h
 */

#define RSP_MIP_MIN_TQL (RSP_RES_PROC_TX + 2)

extern hwidbtype* rsp_get_matching_idb(slottype* slot, int ifc_num,
				       int fci_type, enum IDBTYPE idb_type);

extern void rsp_common_ctrlr_init(slottype*);
extern void rsp_common_if_init(hwidbtype*);
extern void serial_ctrlr_init (slottype *);
extern void serial_if_init (hwidbtype *);
extern void platform_serial_reset_encsize (hwidbtype *);

extern void rsp_serial_set_rxoffset (hwidbtype *);
extern ushort calculate_cbus_txlimit(hwidbtype*);

#endif /* !defined(__RSP_COMMON_IF_H__) */



