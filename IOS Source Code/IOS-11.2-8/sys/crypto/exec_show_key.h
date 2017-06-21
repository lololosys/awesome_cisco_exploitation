/* $Id: exec_show_key.h,v 3.3.62.1 1996/08/12 16:01:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/crypto/exec_show_key.h,v $
 *------------------------------------------------------------------
 * Global key show commands
 *
 * September 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_key.h,v $
 * Revision 3.3.62.1  1996/08/12  16:01:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/18  01:59:53  ahh
 * CSCdi42411:  Key commands not NVGENed properly
 *
 * Revision 3.2  1995/11/17  08:57:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:14:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:35:26  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * show key chain [<name>]
 *
 * OBJ(string,1) -- keychain name
 */

EOLNS    (show_key_chain_eol, show_key_command);

STRING  (show_key_chain_name, show_key_chain_eol, show_key_chain_eol,
	 OBJ(string,1), "Keychain name");

KEYWORD (show_key_chain, show_key_chain_name, NONE,
	 "chain", 
	 "Keychain information", PRIV_CONF);

KEYWORD (show_key, show_key_chain, ALTERNATE,
	 "key", 
	 "Key information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	show_key
