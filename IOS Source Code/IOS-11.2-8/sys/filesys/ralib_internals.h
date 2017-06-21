/* $Id: ralib_internals.h,v 3.2 1995/11/17 09:08:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/ralib_internals.h,v $
 *------------------------------------------------------------------
 * Include file used internally to the RALIB
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ralib_internals.h,v $
 * Revision 3.2  1995/11/17  09:08:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/07  22:57:10  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.1  1995/06/07  20:32:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__ralib_internals_h__)
#define __ralib_internals_h__


/*----------------------------------------------------------------------------*
 * External prototypes of the RALIB functions that are used internally.
 *----------------------------------------------------------------------------*/
extern void ralib_event_card_ready (clpd6720_socket_t socket);
extern void ralib_event_card_busy (clpd6720_socket_t socket);
extern void ralib_event_card_inserted (clpd6720_socket_t socket);
extern void ralib_event_card_removed (clpd6720_socket_t socket);
extern void ralib_event_card_protected (clpd6720_socket_t socket);
extern void ralib_event_card_writable (clpd6720_socket_t socket);

extern ulong ralib_get_flash_base_local_addr (void);
extern ulong ralib_get_pcmcia_base_local_addr (clpd6720_socket_t socket);
extern int ralib_device_init(int dev_num);

#endif /* !defined(__ralib_internals_h__) */

