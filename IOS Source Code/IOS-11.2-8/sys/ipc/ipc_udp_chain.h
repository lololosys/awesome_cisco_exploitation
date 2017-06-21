/* $Id: ipc_udp_chain.h,v 3.2.60.1 1996/08/12 16:03:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_udp_chain.h,v $
 *------------------------------------------------------------------
 * Mainline placeholder file for IPC system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_udp_chain.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:29:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:03:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- UDP CONFIG COMMANDS --------
 */

/******************************************************************
 * ipc master <IP_ADDRESS>
 * 
 * OBJ(paddr,1) = <address>
 */

EOLNS    (config_ipc_master_eol, ipc_config_master_udp);

IPADDR  (config_ipc_master_addr, config_ipc_master_eol, no_alt,
	 OBJ(paddr,1), "IP address of master IPC Server");

NOPREFIX(config_ipc_master_no_prefix, config_ipc_master_addr, 
	 config_ipc_master_eol);

NVGENNS  (config_ipc_master_nvgens, config_ipc_master_no_prefix,
	 ipc_config_master_udp);

KEYWORD (config_ipc_master, config_ipc_master_nvgens, no_alt,
	 "master", "Set master server's address", PRIV_CONF);

KEYWORD_MM (config_ipc, config_ipc_master, no_alt,
	    "ipc", "Configure IPC system", PRIV_CONF, 3);

LINK_POINT (config_ipc_commands, config_ipc); 

