/* $Id: cfg_downward.h,v 3.3.28.1 1996/08/12 16:08:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/cfg_downward.h,v $
 *------------------------------------------------------------------
 * cfg_downward.h
 *
 * Aug. 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_downward.h,v $
 * Revision 3.3.28.1  1996/08/12  16:08:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/16  17:01:05  widmer
 * CSCdi46957:  help for downward-compatible-config is not helpful
 *
 * Revision 3.2  1995/11/17  17:47:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/15  23:25:07  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * downward-compatible-config <major>.<minor>
 *
 * Record the version of software that we should generate a downward
 * compatible config for.
 *
 * OBJ(int,1) = major version
 * OBJ(int,2) = minor version
 */

EOLNS	(cfg_downward_eol, downward_command);

HELP	(cfg_down_help2, NONE, "<0-999999>.<0-999999>\t");
WHITESPACE(cfg_down_ws, cfg_downward_eol, cfg_down_help2);
IDECIMAL(cfg_down_minor, cfg_down_ws, cfg_down_help2, OBJ(int,2), 0, MAXINT);
CHARACTER(cfg_down_decimal, cfg_down_minor, cfg_down_help2, '.');
IDECIMAL(cfg_down_major, cfg_down_decimal, no_alt, OBJ(int,1), 0, MAXINT);
HELP	(cfg_down_help1, cfg_down_major,
	 "  <0-999999>.<0-999999>\tVersion number");

NVGENNS	(cfg_downward_nvgen, cfg_down_help1, downward_command);
NOPREFIX (cfg_downward_no, cfg_downward_nvgen, cfg_downward_eol);

KEYWORD	(cfg_downward, cfg_downward_no, ALTERNATE, 
	 "downward-compatible-config",
	 "Generate a configuration compatible with older software",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_downward
