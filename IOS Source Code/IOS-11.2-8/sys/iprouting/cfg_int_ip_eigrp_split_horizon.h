/* $Id: cfg_int_ip_eigrp_split_horizon.h,v 3.2 1996/11/12 04:35:37 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/cfg_int_ip_eigrp_split_horizon.h,v $
 *------------------------------------------------------------------
 * IP EIGRP split horizion interface command
 *
 * November 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_eigrp_split_horizon.h,v $
 * Revision 3.2  1996/11/12 04:35:37  dkatz
 * CSCdi76003:  EIGRP isnt modular
 * Clean up the tendrils reaching into EIGRP from the generic IP routing
 * code.
 *
 * Revision 3.1  1996/11/12 03:46:52  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] ip split-horizon eigrp <as-number>
 *
 * OBJ(pdb,1) = pdb for the process
 */
EOLS (ci_ip_igrp2_sh_eols, ipigrp2_if_commands, IPIGRP2_SPLIT);
GENERAL_ROUTER(ci_ip_igrp2_sh_router, ci_ip_igrp2_sh_eols,
	       no_alt, router_name, (PROC_IGRP2 | RMATCH_IP | RMATCH_ARGS));
NVGENS (ci_ip_igrp2_sh_nvgen, ci_ip_igrp2_sh_router, ipigrp2_if_commands, 
	IPIGRP2_SPLIT);

#undef  ALTERNATE
#define	ALTERNATE	ci_ip_igrp2_sh_nvgen
