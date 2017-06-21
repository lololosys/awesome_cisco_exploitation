/* $Id: parser_defs_clns.h,v 3.3.20.1 1996/05/04 20:04:19 asastry Exp $
 * $Source: /release/112/cvs/Xsys/clns/parser_defs_clns.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ C L N S . H
 *
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_clns.h,v $
 * Revision 3.3.20.1  1996/05/04  20:04:19  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/29  07:00:31  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:54:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/25  14:59:47  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.1  1995/06/07  22:29:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* CLNS stuff */
#define CLNS_NONE		0
#define CLNS_NSAP		1
#define CLNS_TAG		2

#define	CLNS_ROUTE_DISCARD	 1
#define	CLNS_ROUTE_NAME		 2
#define	CLNS_ROUTE_INTERFACE	 3
#define	PROTOCOL_ISIS		 5
#define	PROTOCOL_ISO_IGRP	 6
#define	CLNS_ROUTEC		 7
#define	CLNS_CLUSTER_CMD	 8
#define	CLNS_CHECKSUM_CMD	 9
#define	CLNS_CNGST_THRESH	10
#define	CLNS_ENABLE		11
#define	CLNS_ERPDU_INTRVL	12
#define	CLNS_ES_NEIGHBOR	13
#define	CLNS_IS_NEIGHBOR	14
#define	CLNS_MTU		15
#define	CLNS_RDPDU_INTRVL	16
#define	CLNS_RDPDU_MASK		17
#define	CLNS_SEND_ERPDU		18
#define	CLNS_SEND_RDPDU		19
#define	CLNS_DEC_COMPAT		20
#define	CLNS_CONFIG_TIMER	21
#define	CLNS_HOST		22
#define	CLNS_HOLDING_TIMER	23
#define	CLNS_PACKET_LIFETIME	24
#define	CLNS_ROUTE		25
#define	CLNS_ROUTER		26
#define	CLNS_ROUTING		27
#define	CLNS_WANT_ERPDU		28
#define CLNS_NET		29
#define CLNS_ACCESS_GROUP       30
#define CLNS_SPLIT_CMD          31
#define CLNS_SECURITY_PASSTHRU  32
#define CLNS_ADJ_FILTER		33
#define CLNS_ESCT_TIME		34
#define CLNS_CACHE_INVALIDATE   35
/*
 * clns {es|is}-neighbor option flags
 */
#define	FLAG_BROAD		0x0001
#define	FLAG_OREVERSE		0x0002
#define	FLAG_IREVERSE		0x0004
#define	FLAG_WINDOW		0x0008
#define	FLAG_PACKET		0x0010
#define	FLAG_CUG		0x0020
#define	FLAG_NVC		0x0040
#define	FLAG_THROUGHPUT		0x0080
#define	FLAG_MODULO		0x0100
#define	FLAG_RPOA		0x0200
#define	FLAG_NUID		0x0400
#define	FLAG_TDELAY		0x0800
#define FLAG_NUDATA		0x1000
#define FLAG_PBP		0x2000
#define FLAG_NOPBP		0x4000

/* CLNS clear commands */
enum {CLEAR_CLNS,
      CLEAR_CLNS_CACHE,
      CLEAR_CLNS_ES_NEIGHBORS,
      CLEAR_CLNS_IS_NEIGHBORS,
      CLEAR_CLNS_NEIGHBORS,
      CLEAR_CLNS_ROUTE,
};

/* CLNS show commands */
enum {SHOW_CLNS,
      SHOW_CLNS_CACHE,
      SHOW_CLNS_ES_NEIGHBORS,
      SHOW_CLNS_FILTER_EXPR,
      SHOW_CLNS_FILTER_SET,
      SHOW_CLNS_GLOBAL,
      SHOW_CLNS_INTERFACE,
      SHOW_CLNS_IS_NEIGHBORS,
      SHOW_CLNS_NEIGHBORS,
      SHOW_CLNS_PROTOCOL,
      SHOW_CLNS_REDIRECTS,
      SHOW_CLNS_ROUTE,
      SHOW_CLNS_TRAFFIC,
};
