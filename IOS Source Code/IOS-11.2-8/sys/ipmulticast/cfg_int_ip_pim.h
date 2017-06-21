/* $Id: cfg_int_ip_pim.h,v 3.5 1996/02/09 18:09:49 lwei Exp $
 * $Source: /release/111/cvs/Xsys/ipmulticast/cfg_int_ip_pim.h,v $
 *------------------------------------------------------------------
 * PIM interface commands
 *
 * November 1993.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_pim.h,v $
 * Revision 3.5  1996/02/09 18:09:49  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 * configuration
 *
 * Revision 3.4  1996/02/08  03:39:22  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.3  1996/02/01  23:33:48  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:29:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:05:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip pim [dense-mode | sparse-mode | sparse-dense-mode]
 * no ip pim [dense-mode | sparse-mode | sparse-dense-mode]
 * no ip pim
 *
 * OBJ(int,1) = 0 for dense
 *		1 for sparse
 *		2 for sparse-dense
 */
EOLS (ci_ip_pim_sparse_dense_eol, pim_commands, PIM_S_D_MODE);
KEYWORD_ID_MM (ci_ip_pim_sparse_dense, ci_ip_pim_sparse_dense_eol,
	ci_ip_pim_sparse_dense_eol,
	OBJ(int,1), PIM_S_D_MODE,
	"sparse-dense-mode", "Enable PIM sparse-dense-mode operation",
	PRIV_CONF|PRIV_SUBIF, 8);

EOLS (ci_ip_pim_sparse_eol, pim_commands, PIM_SM_MODE);
KEYWORD_ID(ci_ip_pim_sparse, ci_ip_pim_sparse_eol, ci_ip_pim_sparse_dense,
	   OBJ(int,1), PIM_SM_MODE,
	   "sparse-mode", "Enable PIM sparse-mode operation", 
	   PRIV_CONF|PRIV_SUBIF);

EOLS (ci_ip_pim_dense_eol, pim_commands, PIM_DM_MODE);
KEYWORD_ID(ci_ip_pim_dense, ci_ip_pim_dense_eol, ci_ip_pim_sparse,
	   OBJ(int,1), PIM_DM_MODE,
	   "dense-mode", "Enable PIM dense-mode operation", 
	   PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * [no] ip pim nbma-mode
 *
 */
EOLS (ci_ip_pim_nbma_eols, pim_commands, PIM_NBMA_MODE);
KEYWORD (ci_ip_pim_nbma, ci_ip_pim_nbma_eols, ci_ip_pim_dense,
	 "nbma-mode", 
	 "Use Non-Broadcast Multi-Access (NBMA) mode on interface",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip pim query-interval <interval>
 * no ip pim query-interval
 *
 * OBJ(int,1) = <interval>
 */
EOLS (ci_ip_pim_query_eol, pim_commands, PIM_QUERY_INTVL);
NUMBER (ci_ip_pim_query_num, ci_ip_pim_query_eol, no_alt,
	OBJ(int,1), 0, 0xffff, "Query interval in seconds");
NOPREFIX (ci_ip_pim_query_no, ci_ip_pim_query_num, ci_ip_pim_query_eol);
NVGENS (ci_ip_pim_query_nvgen, ci_ip_pim_query_no, pim_commands, 
	PIM_QUERY_INTVL);
KEYWORD (ci_ip_pim_query, ci_ip_pim_query_nvgen, ci_ip_pim_nbma,
	 "query-interval", "PIM router query interval", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_pim, ci_ip_pim_query, ALTERNATE,
	 "pim", "PIM interface commands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_pim







