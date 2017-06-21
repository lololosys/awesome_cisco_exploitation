/* $Id: parser_defs_isis.h,v 3.4.12.5 1996/08/11 15:13:38 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/parser_defs_isis.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ I S I S . H
 *
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_isis.h,v $
 * Revision 3.4.12.5  1996/08/11  15:13:38  dkatz
 * CSCdi58739:  ISIS needs to be able to limit flooding
 * Branch: California_branch
 * Add phase I of mesh group support.  Allow an interface to be
 * configured to be part of a mesh group, or to be blocked for
 * flooding.  Allow CSNP transmission to be configured on point-to-
 * point interfaces as a minimal robustness mechanism.  Redefine
 * the SRM, etc., bits as an array of ulongs rather than bytes (to
 * speed up masking operations).  Change the interface number field
 * from a short to a long in order to shrink the code size by a few
 * hundred bytes.
 *
 * Revision 3.4.12.4  1996/08/02  02:48:00  hsmit
 * CSCdi64256:  Add knob to ISIS to generate LSPs with longer lifetime
 * Branch: California_branch
 * Implemented lsp-refresh-interval and max-lsp-lifetime for ISIS. Changed
 * the
 * "show clns traffic" command to show the number of changed and refreshed
 * LSPs.
 *
 * Revision 3.4.12.3  1996/06/18  16:51:01  dkatz
 * CSCdi59965:  ISIS retransmits too quickly
 * Branch: California_branch
 * Add a retransmission throttling mechanism.
 *
 * Revision 3.4.12.2  1996/05/26  06:32:19  dkatz
 * CSCdi58433:  ISIS/NLSP can starve input queue
 * Branch: California_branch
 * Do a clear_if_input on incoming packets before requeueing them for the
 * Update process.  Depth-limit the secondary queue to avoid buffer
 * starvation.
 *
 * Revision 3.4.12.1  1996/05/07  17:15:58  dkatz
 * CSCdi54576:  ISIS LSP pacing rate needs to be configurable
 * Branch: California_branch
 * Provide a knob to alter the LSP pacing timer;  the timer is already
 * there.
 *
 * Revision 3.4  1996/02/19  21:32:41  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.3  1996/01/29  07:00:32  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  08:54:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/23  21:38:01  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Calculate routes from only changed LSPs if the inter-router topology
 * stays constant.  Don't gratuitously install IP routes that will
 * be immediately replaced, as this causes large-scale cache invalidation.
 * Eliminate multiple IP forwarding table traversals.
 *
 * Revision 2.2  1995/08/25  16:02:50  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 *
 * Revision 2.1  1995/08/25  14:59:47  dkatz
 * CSCdi38139:  ISIS needs better control of adjacency parameters
 *
 * Revision 2.3  1995/08/25  05:13:46  dkatz
 * CSCdi38138:  ISIS needs adjacency state tracing
 *
 * Revision 2.2  1995/08/23  18:01:19  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 *
 * Revision 2.1  1995/06/07  22:30:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * isis commands
 */
enum {
    ISIS_PRIO_CMD = 1,
    ISIS_PW_CMD,
    ISIS_METRIC_CMD,
    ISIS_CIRTYPE_CMD,
    ISIS_HELLO_CMD,
    ISIS_HELLO_MULT_CMD,
    ISIS_CSNP_CMD,
    ISIS_RETRANS_CMD,
    ISIS_MIN_REXMIT_CMD,
    ISIS_LSPIVL_CMD,
    ISIS_AREA_PW_CMD,
    ISIS_DOMAIN_PW_CMD,
    ISIS_IP_SUM_ADDR_CMD,
    ISIS_IP_ROUTER_CMD,
    ISIS_ADJFILTER_CMD,
    ISIS_PART_AVOID_CMD,
    ISIS_FORCE_HIPPITY_CMD,
    ISIS_IGN_ATT_BIT_CMD,
    ISIS_NEVER_SET_ATT_CMD,
    ISIS_IPX_AREA_CMD,
    ISIS_SPF_IVL_CMD,
    ISIS_PRC_IVL_CMD,
    ISIS_LSP_GEN_IVL_CMD,
    ISIS_IGN_LSP_ERR_CMD,
    ISIS_LOG_ADJ_CHG_CMD,
    ISIS_LSP_MTU_CMD,
    ISIS_UPQ_DEPTH_CMD,
    ISIS_DSP_RT_DET_CMD,
    ISIS_MAXAGE_CMD,
    ISIS_MAX_LSP_GEN_CMD,
    ISIS_MESHGRP_CMD
};

/*
 * LSP Generation rate limiting
 */
#define ISIS_NLSP_LSP_GEN_DEFAULT		5 /* seconds */
#define ISIS_NLSP_LSP_GEN_MAX			120

/*
 * SPF Interval limiting
 */
#define ISIS_NLSP_SPF_IVL_DEFAULT	5   /* Seconds default */
#define ISIS_NLSP_SPF_IVL_MAX		120 /* Maximum */

/*
 * Partial Route Calculation (PRC) Interval limiting
 */
#define ISIS_NLSP_PRC_IVL_DEFAULT	5   /* Seconds default */
#define ISIS_NLSP_PRC_IVL_MAX		120 /* Maximum */

/*
 * LSP aging and refreshing
 */
#define ISIS_MAXAGE_DEFAULT             1200      /* 20 minutes per spec */
#define ISIS_MAXAGE_MIN                 1         /* 1 sec for test purpose */
#define ISIS_MAXAGE_MAX                 MAXUSHORT /* As high as possible */
#define ISIS_MAX_LSP_GEN_DEFAULT        900       /* 15 minutes per spec */
#define ISIS_MAX_LSP_GEN_MIN            1         /* Same as NLSP */
#define ISIS_MAX_LSP_GEN_MAX            ISIS_MAXAGE_MAX /* 18.7 hours */

/*
 * ISIS LSP MTU sizes
 */
#define ISIS_LSP_MTU_MIN	128	/* Arbitrary numbers, */
#define ISIS_LSP_MTU_MAX	4352	/* But fairly reasonable. */

/*
 * LSP hashing algorithm definitions:
 */
#define ISIS_NLSP_RT_FRAGS_DEFAULT   64 /* Stuff routes in 0..63 */
#define ISIS_NLSP_SVC_FRAGS_DEFAULT 128 /* Stuff svcs in 64..127 */

/* ISIS show commands */
enum {
    SHOW_ISIS_DATABASE,
    SHOW_ISIS_ROUTE,
    SHOW_ISIS_SPFLOG,
    SHOW_ISIS_TIMERS,
    SHOW_ISIS_MESHGRPS,
    SHOW_NLSP_DATABASE,
    SHOW_NLSP_SPFLOG,
    SHOW_NLSP_TIMERS};
