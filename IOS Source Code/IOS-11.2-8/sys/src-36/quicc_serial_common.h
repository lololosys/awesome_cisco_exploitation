/* $Id: quicc_serial_common.h,v 3.1.6.1 1996/06/12 19:56:54 jng Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_serial_common.h,v $
 *------------------------------------------------------------------
 * quicc_serial_common.h  definition for 68360 QUICC chip use by sync/async
 *
 * March 1996, John Ng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_serial_common.h,v $
 * Revision 3.1.6.1  1996/06/12  19:56:54  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.1  1996/03/20  01:27:10  jng
 * Initial Async Catwoman placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
void quicc_set_serial_pins (quicc_dpr_t *);
void quicc_set_async_serial_pins (quicc_dpr_t *);

void quicc_modem_interrupt_mask (hwidbtype *, boolean , ulong);
void memory_pool_adjustment (hwidbtype *, boolean);
void quicc_cpic_handler_install(hwidbtype *, quicc_dpr_t  *, ushort, ushort,ulong, void (*)(hwidbtype *), void (*)(void));

void quicc_serial_dcd_interrupt(void);
void quicc_serial_onesec_periodic (hwidbtype *);
/* This macro is use to tell gsmr_write that no flags will be set.
 */
#define QUICC_GSMR_NONE	0




