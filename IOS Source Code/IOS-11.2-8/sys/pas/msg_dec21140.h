/* $Id: msg_dec21140.h,v 3.3.10.3 1996/08/28 13:07:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_dec21140.h,v $
 *------------------------------------------------------------------
 * msg_dec21140.h - Support for DEC21140 driver messages
 *
 * February 1995, Steve Jacobson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dec21140.h,v $
 * Revision 3.3.10.3  1996/08/28  13:07:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.10.2  1996/05/07  19:24:08  mbeesley
 * CSCdi56937:  Update DEC21140 errmsgs to include slot/subunit
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/03/21  23:24:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:29:18  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/03/07  00:19:21  mbeesley
 * CSCdi50649:  DEC21140 in Store-and-Forward mode
 *         o Put device in tx cut-thru, 512 byte tx fifo threshold
 *         o Increase rx pool, eliminate use of fallback pool
 *         o Handle rx fifo overrun and error interrupt correctly
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/02/23  20:02:20  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.2  1995/11/17  18:55:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
