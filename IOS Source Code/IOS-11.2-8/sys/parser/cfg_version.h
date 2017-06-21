/* $Id: cfg_version.h,v 3.2.62.1 1996/08/12 16:05:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_version.h,v $
 *------------------------------------------------------------------
 * C F G _ V E R S I O N . H
 *
 * version - generate the software version
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_version.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:14:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * version <string>
 *
 * generates the software version, only used during NV generation
 *
 * OBJ(int,1) = major version
 * OBJ(int,2) = minor version
 */

EOLNS	(cfg_version_eol, version_command);

TEXT	(cfg_version_text, cfg_version_eol, cfg_version_eol,
	 OBJ(string,1), NULL);

TEST_MULTIPLE_FUNCS(cfg_version_number, cfg_version_text,
		    no_alt, NONE);

IDECIMAL (cfg_version_minor, cfg_version_text, cfg_version_number,
	  OBJ(int, 2), 1, MAXINT);
CHARACTER (cfg_version_dot, cfg_version_minor, cfg_version_number, '.');
IDECIMAL (cfg_version_major, cfg_version_dot, cfg_version_number,
	  OBJ(int, 1), 1, MAXINT);
NVGENNS	(cfg_version_nvgen, cfg_version_major,
	 version_command);

KEYWORD	(cfg_version, cfg_version_nvgen, ALTERNATE, 
	 "version", NULL, PRIV_CONF|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	cfg_version
