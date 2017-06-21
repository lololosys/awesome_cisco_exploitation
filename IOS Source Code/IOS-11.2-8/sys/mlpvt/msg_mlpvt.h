/* $Id: msg_mlpvt.h,v 1.1.42.7 1996/08/28 13:00:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/msg_mlpvt.h,v $
 *------------------------------------------------------------------
 * msg_mlpvt.h -- MLPVT messages
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: msg_mlpvt.h,v $
 * Revision 1.1.42.7  1996/08/28  13:00:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.6  1996/06/03  18:02:55  syiu
 * CSCdi59320:  SGBP has too many errmsg facdefs
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/05/22  02:24:14  perryl
 * CSCdi56889:  MLPVT Hellos detection of misconfiguration
 * Branch: California_branch
 * Print the possible misconfigurated member name in the
 * error message
 *
 * Revision 1.1.42.4  1996/05/18  22:35:14  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.3  1996/05/06  21:49:58  perryl
 * CSCdi56532:  MLPVT does not properly break tie on equal high bids
 * Branch: California_branch
 * Randomize the tie breaker
 *
 * Revision 1.1.42.2  1996/05/01  02:06:55  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for L2F and MLPVT to do CHAP style authenication with the new AAA API.
 *
 * Revision 1.1.42.1  1996/04/27  06:34:56  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.6  1996/04/22  00:31:27  perryl
 * Branch: Dial1_branch
 * Added CHAP style authenication to MLPVT.
 * Used AAA to obtain password, eliminate "mlpvt password"
 * command.
 * Fixed misc bugs and enhanced debug messages.
 *
 * Revision 1.1.2.5  1996/04/03  22:59:03  abaindur
 * Branch: Dial1_branch
 *  Enhance bid generation scheme for MLP bundle mastership. Provide
 * a means of controlling mastership based on either locality of a call
 * or load-sharing.
 *
 * Revision 1.1.2.4  1996/03/09  00:48:01  abaindur
 * Branch: Dial1_branch
 *  Some state m/c fixes, cleanup, and documentation.
 *
 * Revision 1.1.2.3  1996/03/05  02:47:07  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.2  1996/03/01  21:43:20  abaindur
 * Branch: Dial1_branch
 *  o fix the query simulator; a casualty of the integration with MLP/VPN
 *  o enhance query state m/c to handle case where a peer goes down just
 *  when we're sending it a query msg. The TCP write blocks and a peer loss
 *  event blows the peer state from beneath.
 *
 * Revision 1.1.2.1  1996/01/26  20:28:52  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:23:03  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
