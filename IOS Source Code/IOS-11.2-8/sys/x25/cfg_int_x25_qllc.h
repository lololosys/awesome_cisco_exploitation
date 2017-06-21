/* $Id: cfg_int_x25_qllc.h,v 3.1.2.1 1996/04/11 23:47:40 gglick Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_x25_qllc.h,v $
 *------------------------------------------------------------------
 * X.25 QLLC client interface parsing support
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_x25_qllc.h,v $
 * Revision 3.1.2.1  1996/04/11  23:47:40  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:02  gglick
 * Initial commit of X.25 parser modularization files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 *      ...[options]
 * (QLLC map and PVC command option external linkage)
 */

LINK_EXIT(ci_x25qllc_mappvc_exit, no_alt);
NOP	(x25qllc_mappvc_opt, ci_x25qllc_mappvc_exit, NONE);

/************************************************************************
 *	(x25 pvc <#>) qllc [<QLLC-addr>] [<X.121-addr>]...
 * QLLC PVC command addresses
 *
 * For a PVC command;
 *  csb->uiarray[0] is set to the permissible PVC options
 *  csb->uiarray[1] is set to LINK_QLLC
 *  OBJ(paddr,1) is set to the QLLC address, if specified
 *  OBJ(paddr,2) is set to the remote host's X.121 address, if specified
 */

SET	(x25qllc_pvc_opt_set, x25qllc_mappvc_opt,
	 uiarray[0], X25OPT_QLLC_OPTIONS & X25OPT_PVC_OPTIONS);
X121ADDR(x25qllc_pvc_x121, x25qllc_pvc_opt_set, x25qllc_pvc_opt_set,
	 OBJ(paddr,2), "Destination host address");

QLLCADDR(x25qllc_pvc_addr, x25qllc_pvc_x121, x25qllc_pvc_x121,
	 OBJ(paddr,1), "QLLC address");
PROTONAME(x25qllc_pvc_proto, x25qllc_pvc_addr, NONE, uiarray[1], PMATCH_QLLC);

/************************************************************************
 *	(x25 map) qllc [<QLLC-addr>] <X.121-addr>...
 * QLLC map command addresses
 *
 * For a map command:
 *  csb->uiarray[0] is set to the permissible map options
 *  csb->uiarray[1] is set to LINK_QLLC
 *  OBJ(paddr,1) is set to the QLLC address, if specified
 *  OBJ(paddr,2) is set to the remote host's X.121 address
 */

SET	(x25qllc_map_opt_set, x25qllc_mappvc_opt,
	 uiarray[0], X25OPT_QLLC_OPTIONS);
X121ADDR(x25qllc_map_x121, x25qllc_map_opt_set, NONE,
	 OBJ(paddr,2), "Destination host address");

QLLCADDR(x25qllc_map_addr, x25qllc_map_x121, x25qllc_map_x121,
	 OBJ(paddr,1), "QLLC address to map");
PROTONAME(x25qllc_map_proto, x25qllc_map_addr, NONE, uiarray[1], PMATCH_QLLC);

/************************************************************************/

/* OBJ(int,1) is 0 for a map, non-zero for a PVC */
SPLIT	(x25qllc_mappvc_split, x25qllc_map_proto, x25qllc_pvc_proto,
	 OBJ(int,1), 0);

NOP	(x25qllc_mappvc_enter, x25qllc_mappvc_split, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	x25qllc_mappvc_enter
