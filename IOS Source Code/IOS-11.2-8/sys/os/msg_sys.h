/* $Id: msg_sys.h,v 3.5.8.4 1996/08/28 13:03:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_sys.h,v $
 *------------------------------------------------------------------
 * msg_sys.h -- header for system message definitions.
 *
 * October, 1995, David S.A. Stine
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sys.h,v $
 * Revision 3.5.8.4  1996/08/28  13:03:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.8.3  1996/08/24  00:20:13  sberl
 * CSCdi66828:  process_create_common should validate start address
 * Branch: California_branch
 * Check that process start address is not NULL and it is not greater than
 * etext.
 *
 * Revision 3.5.8.2  1996/06/19  23:17:15  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.5.8.1  1996/04/04  09:37:41  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/28  09:33:25  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.4  1996/02/21  01:20:36  hampton
 * Catch bad offsets to passive timer routines.  [CSCdi49419]
 *
 * Revision 3.3  1996/01/25  11:20:28  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  00:18:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/18  08:17:56  dstine
 * Placeholders for message headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
