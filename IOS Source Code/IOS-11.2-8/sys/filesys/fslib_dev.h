/* $Id: fslib_dev.h,v 3.2 1995/11/17 09:08:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fslib_dev.h,v $
 *------------------------------------------------------------------
 * Include file for the file system device interface
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev.h,v $
 * Revision 3.2  1995/11/17  09:08:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__fslib_dev_h__)
#define __fslib_dev_h__

/*----------------------------------------------------------------------------*
 * XXX jack, why are these defined in filesys.h too?
 * Defines for the "prog_algorithm" field in the DIB
 * These are the programming algorithms that are supported
 *   0 - invalid (used to denote the end of the list of supported
 *       types in the programming array)
 *   1 - any read only card that requires no special commands to read
 *       (this is the default set of routines that are used if a card
 *        with an unrecognized "prog_algorithm" is found).
 *   2 - RAM card: any card that requires no special commands to read
 *       or write or to switch between read and write modes.  The
 *       functions for this type of card make sure that writes which
 *       start and/or end on odd boundaries don't corrupt data.
 *       Erases to 0x00
 *   3 - RAM card: (same as above)
 *       Erases to 0xff
 *   4 - Intel Series 2+ flash cards (any size in general, but
 *       specifically the 4 and 20 MB versions).
 *   5 - Internal flash bank on the RSP
 *----------------------------------------------------------------------------*/
#define FSLIB_PROG_LIST_END                 0
#define FSLIB_PROG_ROM_CARD                 1
#define FSLIB_PROG_RAM_CARD_00              2
#define FSLIB_PROG_RAM_CARD_FF              3
#define FSLIB_PROG_INTEL_SERIES_2PLUS_CARD  4
#define FSLIB_PROG_RSP_INTERNAL_FLASH       5
#define FSLIB_PROG_MAX                      5


#endif /* !defined(__fslib_dev_h__) */


