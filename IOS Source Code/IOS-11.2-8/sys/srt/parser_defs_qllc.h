/* $Id: parser_defs_qllc.h,v 3.1 1996/02/01 04:59:18 hampton Exp $
 * $Source: /release/111/cvs/Xsys/srt/parser_defs_qllc.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * April, 1994		Phil Morton
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Define configuration, show, and debug constants
 *------------------------------------------------------------------
 * $Log: parser_defs_qllc.h,v $
 * Revision 3.1  1996/02/01  04:59:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/19  20:47:43  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add constant for 'qllc dlsw' command
 *
 * Revision 2.1  1995/06/07  22:30:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* qllc parser commands */ 

enum {
    LNX_SRB,			/* source route bridging                     */
    LNX_PRTNR,			/* partner                                   */
    LNX_XID,			/* sna xid                                   */
    LNX_PASS,			/* password - used for qllc/sdlc conversion  */
    LNX_QLLCLF,			/* largest frame or largest sna data unit    */
    LNX_SAP,			/* enables use of non-default sap values     */
    LNX_POLL,			/* lets lnx poll the LAN workstation	     */
    QLLC_CFG_DLSW,		/* Configure QLLC support for DLSw+	     */
} ;
