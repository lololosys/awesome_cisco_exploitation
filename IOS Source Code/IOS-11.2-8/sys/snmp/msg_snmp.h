/* $Id: msg_snmp.h,v 3.1.2.4 1996/08/28 13:09:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/msg_snmp.h,v $
 *------------------------------------------------------------------
 * msg_snmp.h - SNMP Error Messages
 *
 * April 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_snmp.h,v $
 * Revision 3.1.2.4  1996/08/28  13:09:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/07/31  00:10:06  ramk
 * CSCdi57452:  SNMP Trap overflow on boot-up
 * Branch: California_branch
 * Trap queue implemented using buffers allocated from system buffer pool.
 * Buginfs changed to errmsgs.
 *
 * Revision 3.1.2.2  1996/05/01  13:39:32  jjohnson
 * CSCdi40034:  spurious access in SNMP - AddOID
 * Issue an errmsg and abort if an attempt is made to encode an invalid OID
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/04/10  05:17:00  jjohnson
 * CSCdi54210:  Cannot generate SNMP traps at interrupt level
 * Detect and prevent any evil doers
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/09  16:24:20  jjohnson
 * Initial Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file obsoleted as part of the cleanup of message externs.
 */
