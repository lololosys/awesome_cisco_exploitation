/* $Id: cfg_configreg.h,v 3.2.62.1 1996/08/12 16:05:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_configreg.h,v $
 *------------------------------------------------------------------
 * C F G _ C O N F I G R E G . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_configreg.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:42:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:49:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:02:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(VIRTUAL_CONFIG_HIDDEN)
#define VIRTUAL_CONFIG_HIDDEN	0
#endif

/***************************************************************
 * config-register <number>
 * no config-register [<number>]
 */

EOLNS	(cfg_configreg_eol, configreg_command);
ZEROXHEXNUM	(cfg_configreg_num, cfg_configreg_eol, no_alt,
	 OBJ(int,1), "Config register number");
NOPREFIX(cfg_configreg_noprefix, cfg_configreg_num, cfg_configreg_eol);
NVGENNS  (cfg_configreg_nvgen, cfg_configreg_noprefix,
	 configreg_command );

KEYWORD	(cfg_configreg, cfg_configreg_nvgen, ALTERNATE, "config-register",
	 "Define the configuration register",
	 PRIV_CONF|VIRTUAL_CONFIG_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	cfg_configreg
