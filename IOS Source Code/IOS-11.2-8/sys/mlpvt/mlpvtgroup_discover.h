/* $Id: mlpvtgroup_discover.h,v 1.1.42.2 1996/08/28 13:00:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvtgroup_discover.h,v $
 *------------------------------------------------------------------
 * 
 * mlpvtgroup_discover.h -- MLPVT group discovery via hellos
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvtgroup_discover.h,v $
 * Revision 1.1.42.2  1996/08/28  13:00:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:54  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.2  1996/04/22  00:31:25  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:51  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:23:01  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MLPVT_DISCOVER_H__
#define __MLPVT_DISCOVER_H__

/*
 * XXX - these need discussing :-)
 */
#define MLPVT_DISCOVER_PORT	9900
#define MLPVT_MAX_DGRAM_SIZE	1472

/*
 * Hello message type
 */

#define MLPVT_CHALLENGE         1
#define MLPVT_CHALLENGE_RESP    2

#define MLPVT_DUMMY_PASSWD "unsecure"

#endif
