/* $Id: cfg_nasi.h,v 3.2 1995/11/17 19:21:06 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/cfg_nasi.h,v $
 *------------------------------------------------------------------
 * cfg_nasi.h    Parse chains for top level "nasi" command
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_nasi.h,v $
 * Revision 3.2  1995/11/17  19:21:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  21:10:16  irfan
 * Parser chains for top level nasi command and nasi line subcommand
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 ***************************************************************
 * Config command:
 *
 *  [no]  nasi start
 ***************************************************************
 */


EOLS(cfg_nasi_eol, nasi_enable_command, NASI_CMD_START);

KEYWORD(cfg_nasi_start, cfg_nasi_eol,
	no_alt, "enable", "Start up NASI services",
	PRIV_CONF);

KEYWORD(cfg_nasi, cfg_nasi_start, ALTERNATE,
	"nasi-server",
	"Netware Asynchronous Services Interface config commands",
	PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cfg_nasi

