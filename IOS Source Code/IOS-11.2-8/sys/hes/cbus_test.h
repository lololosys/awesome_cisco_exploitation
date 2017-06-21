/* $Id: cbus_test.h,v 3.3 1995/11/17 09:15:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/cbus_test.h,v $
 *------------------------------------------------------------------
 * cbus_test.h -- cbus/cbus2 diagnostic routines definitions
 *
 * December 1991 - David R. Hampton
 * 
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbus_test.h,v $
 * Revision 3.3  1995/11/17  09:15:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:25:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:37:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:38:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* cbus_test.c */

extern int default_interface;

ushort cbus_select(mcitype *, int);
void cbus_address(ulong *, ulong *, ulong);
void dci_command(mcitype *, ulong *, ulong *, int *);
void fddi_psiclear(void);
void fddi_psiend(void);
void fddi_psistart(void);

/* cbus_test_chain.c */
extern void cbus_parser_test_init(void);

/* ctr_test.c */
extern void ctr_specific(mcitype *);

/* cip_test.c */
extern void channel_specific(mcitype*);
extern void logger_drain(int);
extern void console_drain(int);
