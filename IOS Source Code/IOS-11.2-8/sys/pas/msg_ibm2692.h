/* $Id: msg_ibm2692.h,v 3.3.32.4 1996/08/28 13:07:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_ibm2692.h,v $
 *------------------------------------------------------------------
 * msg_ibm2692.h - Message header file for 4R IBM Token Ring PCI 
 *	 	   port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ibm2692.h,v $
 * Revision 3.3.32.4  1996/08/28  13:07:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.32.3  1996/08/22  18:42:32  dujihara
 * CSCdi65823:  Error messages dont specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.3.32.2  1996/06/07  00:18:01  hwang
 * CSCdi59533:  4r driver need to implement srb command queue
 * Branch: California_branch
 *
 * Revision 3.3.32.1  1996/04/23  02:24:03  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/11  19:39:37  hwang
 * CSCdi46480:  4R failed hot standby test
 * vip_token_classify function to return device stat in trip style.
 * Remove some unused error messages.
 *
 * Revision 3.2  1995/11/17  18:55:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
