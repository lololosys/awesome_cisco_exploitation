/* $Id: mlpvtgroup_connect.h,v 1.1.42.2 1996/08/28 13:00:33 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvtgroup_connect.h,v $
 *------------------------------------------------------------------
 * mlpvtgroup_connect.h -- MLPVT group connection setup
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvtgroup_connect.h,v $
 * Revision 1.1.42.2  1996/08/28  13:00:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:51  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.3  1996/04/22  00:31:23  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.2  1996/03/05  02:47:03  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:49  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:58  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVTGROUP_CONNECT_H__
#define __MLPVTGROUP_CONNECT_H__

#define MLPVT_CONNECT_PORT		9900
#define MLPVT_KEEPALIVE_TIMER           (2 * ONESEC)
#define MLPVT_KEEPALIVE_RETRIES         3

void mlpvt_close_pipe(mlpvtmember_t *);

#endif
