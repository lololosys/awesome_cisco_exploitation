/*
 *	P A R S E R _ D E F S _ S R B . H
 *
 * $Id: parser_defs_srb.h,v 3.2 1996/03/11 14:05:49 rbatz Exp $
 * $Source: /release/111/cvs/Xsys/srt/parser_defs_srb.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_srb.h,v $
 * Revision 3.2  1996/03/11  14:05:49  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.1  1996/02/01  04:59:19  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:52:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:31:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Command table for config command "source-bridge"
 */

enum {
    SRB_ACTIVE = 1,
    SRB_CONTMO,
    SRB_COS,
    SRB_ENABLE_80D5,
    SRB_FRAMESIZE,
    SRB_FST_PEERNAME,
    SRB_INADDRESS,
    SRB_INLSAP,
    SRB_INTYPE,
    SRB_LACK_CON_TIMEOUT,
    SRB_LACK_PASSTHRU,
    SRB_MAXHOPS,
    SRB_MAXRD,
    SRB_OLDSNA,
    SRB_OUTADDRESS,
    SRB_OUTLSAP,
    SRB_OUTTYPE,
    SRB_PROXY,
    SRB_PROXY_DSAP,
    SRB_REMPR,
    SRB_REMPR_FST,
    SRB_REMPR_INT,
    SRB_REMPR_TCP,
    SRB_REMPR_FR,
    SRB_REMPR_FTCP,
    SRB_RINGGR,
    SRB_ROUTEC,
    SRB_SAP_80D5,
    SRB_SDLLC_LAK,
    SRB_QLLC_LAK,
    SRB_SPAN,
    SRB_SSE,
    SRB_TCPQMAX,
    SRB_TRANSPARENT,
    SRB_TRANSPARENT_90COMPAT,
    SRB_TRANSPARENT_CISCO,
    SRB_TRANSPARENT_STANDARD,
    SRB_PROXY_NETBIOS,
    SRB_EXPLORERQ_DEPTH,
    SRB_REMPR_KEEP,
    SRB_MAXOHOPS,
    SRB_MAXIHOPS,
    SRB_EXPLORER_FASTSWITCH,
    SRB_EXPLORER_MAXRATE,
    SRB_EXPLORER_DUP_ARE_FILTER
};

/*
 * Flags for "source-bridge route-cache" command
 */
#define	SRB_ROUTEC_CBUS	1
#define	SRB_ROUTEC_SSE	2
