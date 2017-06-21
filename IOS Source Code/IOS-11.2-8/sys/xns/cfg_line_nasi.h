/* $Id: cfg_line_nasi.h,v 3.3 1995/12/07 17:45:46 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/cfg_line_nasi.h,v $
 *------------------------------------------------------------------
 * cfg_line_nasi.h    Parser chains for "nasi" line subcommand
 *
 * Sept 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_line_nasi.h,v $
 * Revision 3.3  1995/12/07  17:45:46  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  19:21:05  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  21:10:17  irfan
 * Parser chains for top level nasi command and nasi line subcommand
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * NASI line subcommand
 *
 * NASI   authen  <authen-list>
 *
 */
EOLS	(cl_nasi_authen_eol, nasi_line_subcommand, LINE_NASI_AUTH);
SET	(cl_nasi_authen_list_set, cl_nasi_authen_eol, OBJ(int,2), 2);
STRING	(cl_nasi_authen_list_name, cl_nasi_authen_list_set, no_alt,
	 OBJ(string,1), "Use an authentication list with this name.");
TESTVAR	(cl_nasi_authen_list_term, NONE, cl_nasi_authen_list_name,
	 NONE, NONE, NONE, OBJ(int,2), 1);
KEYWORD_ID (cl_nasi_authen_list, cl_nasi_authen_eol,
	    cl_nasi_authen_list_term,
	    OBJ(int,2), 1,
	    "default", "Use the default authentication list.", PRIV_CONF);
NOPREFIX(cl_nasi_authen_nopre, cl_nasi_authen_list, cl_nasi_authen_eol);
NVGENS	(cl_nasi_authen_nvgen, cl_nasi_authen_nopre, nasi_line_subcommand,
	 LINE_NASI_AUTH);
KEYWORD	(cl_nasi_authen, cl_nasi_authen_nvgen, no_alt, "authentication",
	 "Authentication parameters.", PRIV_CONF);


KEYWORD	(cl_line_nasi, cl_nasi_authen, ALTERNATE,
	 "nasi", "Netware Asynchronous Services Interface", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_line_nasi
