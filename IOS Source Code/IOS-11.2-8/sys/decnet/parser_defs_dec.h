/*
 *	P A R S E R _ D E F S . H
 *
 * $Id: parser_defs_dec.h,v 3.1.40.1 1996/05/30 23:42:56 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/parser_defs_dec.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_dec.h,v $
 * Revision 3.1.40.1  1996/05/30  23:42:56  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:57:25  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:50  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:50:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/20  19:03:34  asastry
 * CSCdi41453:  Request for new knob: no decnet split-horizon (for frame
 * relay)
 *
 * Revision 2.2  1995/07/24  06:17:46  asastry
 * CSCdi36662:  Add a knob to override the behavior of the "L2 attached
 * bit". This is needed to allow L2 routers that are also doing IV/V
 * conversion to advertise the route to the "nearest L2 router" even when
 * they have no Phase IV area reachability.
 *
 * Revision 2.1  1995/06/07  22:29:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Decnet connect item definitions
 */
#define DNID_EQ 1
#define DNSTR_EQ    "eq"

#define DNID_NEQ 2
#define DNSTR_NEQ    "neq"

#define DNID_LT 3
#define DNSTR_LT    "lt"

#define DNID_GT 4
#define DNSTR_GT    "gt"
/* DECnet stuff */

#define DEC_NODETYPE_1		 1	/* Level 1 router */
#define DEC_NODETYPE_2		 2	/* Level 2 router */
#define DEC_PS_NORMAL		 1	/* Path split normal */
#define DEC_PS_INTERIM		 2	/* Path split iterim */

/*
 * Mop commands
 */

#define	MOP_CFG_DEVICE_CODE	 1
#define	MOP_CFG_ENABLED		 2
#define	MOP_CFG_RETRANSMIT	 3
#define	MOP_CFG_RETRIES		 4
#define	MOP_CFG_SYSID		 5

/* DECNET defines from cisco sources */
#define DC_ADDRESS               0
#define DC_AREA_MAX_COST         (DC_ADDRESS+1)
#define DC_AREA_MAX_HOPS         (DC_AREA_MAX_COST+1)
#define DC_MAP                   (DC_AREA_MAX_HOPS+1)
#define DC_MAX_ADDRESS           (DC_MAP+1)
#define DC_MAX_AREA              (DC_MAX_ADDRESS+1)
#define DC_MAX_COST              (DC_MAX_AREA+1)
#define DC_MAX_HOPS              (DC_MAX_COST+1)
#define DC_MAX_PATHS             (DC_MAX_HOPS+1)
#define DC_MAX_VISITS            (DC_MAX_PATHS+1)
#define DC_NODE_TYPE            (DC_MAX_VISITS+1)
#define DC_PATH_SPLIT_MODE      (DC_NODE_TYPE+1)
#define DC_ROUTER_PRIORITY      (DC_PATH_SPLIT_MODE+1)
#define DC_ROUTING              (DC_ROUTER_PRIORITY+1)
#define DC_CONVERSION           (DC_ROUTING+1)
#define DC_ACCESS_GROUP         (DC_CONVERSION+1)
#define DC_COST                 (DC_ACCESS_GROUP+1)
#define DC_HELLO_TIME           (DC_COST+1)
#define DC_IN_FILTER            (DC_HELLO_TIME+1)
#define DC_OUT_FILTER           (DC_IN_FILTER+1)
#define DC_ROUTE_TIME           (DC_OUT_FILTER+1)
#define DC_ROUTE_CACHE          (DC_ROUTE_TIME+1)
#define DC_ENCAPSULATION        (DC_ROUTE_CACHE+1)
#define DNE_CISCO               (DC_ENCAPSULATION+1)
#define DNE_DEC                 (DNE_CISCO+1)
#define DC_CONGEST_THRESH       (DNE_DEC+1)
#define DC_ADVERTISE		(DC_CONGEST_THRESH+1)
#define DC_MULTI_MAP            (DC_ADVERTISE+1)
#define DNE_ALLROUTERS		(DC_MULTI_MAP+1)
#define DNE_ALLPRIMEROUTERS	(DNE_ALLROUTERS+1)
#define DNE_ALLENDNODES		(DNE_ALLPRIMEROUTERS+1)
#define DC_HOST		        (DNE_ALLENDNODES+1)
#define DC_ROUTE	        (DC_HOST+1)
#define DC_ROUTE_NAME	        (DC_ROUTE+1)
#define DC_ROUTE_DISCARD	(DC_ROUTE_NAME+1)
#define DC_ROUTE_INTERFACE      (DC_ROUTE_DISCARD+1)
#define DC_PROPAGATE            (DC_ROUTE_INTERFACE+1)
#define DC_ATTACH_OVERRIDE      (DC_PROPAGATE+1)
#define DC_SPLIT_HORIZON        (DC_ATTACH_OVERRIDE+1)
#define NDECNETCMDS		(DC_SPLIT_HORIZON+1)

#define CONVERGENCE_IGRP        1

#define DNN_AREA                 0
#define DNN_ROUTING_IV           1
#define DNN_NKEYS                2

#define DNM_NORMAL               0
#define DNM_INTERIM              1
#define DNM_NORMAL               0
#define DNM_INTERIM              1
#define DNM_NKEYS                2

#define DNNAM_EQ                 1
#define DNNAM_NEQ                2
#define DNNAM_LT                 3
#define DNNAM_GT                 4

/* DECNET clear commands */
enum {CLEAR_DECNET,
      CLEAR_DECNET_COUNTERS,
};

/* DECNET show commands */
enum {SHOW_DECNET,
      SHOW_DECNET_ACCESS,
      SHOW_DECNET_DEBUG,
      SHOW_DECNET_INTERFACE,
      SHOW_DECNET_MAP,
      SHOW_DECNET_NEIGHBORS,
      SHOW_DECNET_ROUTE,
      SHOW_DECNET_STATIC,
      SHOW_DECNET_TRAFFIC,
      SHOW_DECNET_GLOBAL,
};
