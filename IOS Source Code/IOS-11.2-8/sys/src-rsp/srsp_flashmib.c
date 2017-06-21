/* $Id: srsp_flashmib.c,v 3.2.64.1 1996/08/28 13:14:35 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/srsp_flashmib.c,v $
 * ------------------------------------------------------------------
 * RSP slave-side flash MIB support code
 *
 * October 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: srsp_flashmib.c,v $
 * Revision 3.2.64.1  1996/08/28  13:14:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:50:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/18  01:22:54  schaefer
 * placeholders for 7500 flash MIB
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-rsp/rsp_flashmib.h"

void
rsp_flashmib_init (void)
{   
    return;
}
