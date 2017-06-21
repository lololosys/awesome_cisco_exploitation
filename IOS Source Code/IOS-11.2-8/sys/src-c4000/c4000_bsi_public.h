/* $Id: c4000_bsi_public.h,v 3.2.62.1 1996/08/28 13:12:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/c4000_bsi_public.h,v $
 *------------------------------------------------------------------
 * External prototypes for BSI FDDI driver
 *
 * May 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: c4000_bsi_public.h,v $
 * Revision 3.2.62.1  1996/08/28  13:12:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:45:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C4000_BSI_PUBLIC_H__
#define __C4000_BSI_PUBLIC_H__

/*
 * fddi_c4000_chain.h
 */
extern void fddi_c4000_parser_init(void);

/*
 * c4000_bsi.c
 */
extern const char msgtxt_fddi_illegal[];

#endif __C4000_BSI_PUBLIC_H__

