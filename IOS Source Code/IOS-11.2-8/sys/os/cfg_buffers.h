/* $Id: cfg_buffers.h,v 3.2.62.2 1996/08/12 16:04:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_buffers.h,v $
 *------------------------------------------------------------------
 * C F G _ B U F F E R S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_buffers.h,v $
 * Revision 3.2.62.2  1996/08/12  16:04:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/07/08  22:38:15  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:45:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:50:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:02:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * buffers { small | middle | large | huge | <interface> } 
 *	{ permanent | max-free | min-free | initial} <number>
 *
 * no buffers { small | middle | large | huge | <interface> } 
 *	{ permanent | max-free | min-free | initial} <number>
 *
 *
 * buffers huge size <size>
 * no buffers huge size <size>
 *
 */
EOLNS	(conf_buf_eol, buffers_command);

/* All but 'huge' get the number of buffers */
NUMBER	(conf_buf_num, conf_buf_eol, no_alt,
	 OBJ(int,3), 0, 20480, "Number of buffers");

/* Get the new huge buffer size.  This provides a size specific help message. */
NUMBER	(conf_buf_size_getnum, conf_buf_eol, no_alt, 
	 OBJ(int,3), DEF_HUGEDATA, 100000, "Size of huge buffers");
TESTVAR	(conf_buf_size_num, conf_buf_size_getnum, conf_buf_num,
	 NONE, NONE, NONE, OBJ(int,2), BUF_CMD_SIZE);

NOPREFIX (conf_buf_noprefix, conf_buf_size_num, conf_buf_eol);
NVGENNS	(conf_buf_nvgen, conf_buf_noprefix, buffers_command);

/******************************************************************
 * { permanent | max-free | min-free | initial | size }
 * NOTE: The 'size' keyword is only valid for 'huge' buffers.
 *
 * OBJ(int,2) = BUF_CMD_PERM, BUF_CMD_MAXFREE, BUF_CMD_MINFREE, BUF_CMD_INITIAL
 */

/* Only show the 'size' keyword if 'huge' buffers were specified. */
KEYWORD_ID (conf_buf_sizekwd, conf_buf_nvgen, no_alt,
	    OBJ(int,2), BUF_CMD_SIZE,
	    "size", "Size of huge buffers", PRIV_CONF);
TESTVAR	(conf_buf_size, conf_buf_sizekwd, no_alt,
	 NONE, NONE, NONE, OBJ(int,1), BUF_CMD_HUGE);

KEYWORD_ID (conf_buf_initial, conf_buf_nvgen, conf_buf_size,
	    OBJ(int,2), BUF_CMD_INIT,
	    "initial", "Temporary buffers allocated at system reload", PRIV_CONF);
KEYWORD_ID (conf_buf_minfree, conf_buf_nvgen, conf_buf_initial,
	    OBJ(int,2), BUF_CMD_MINFREE,
	    "min-free", "Minimum number of free buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_maxfree, conf_buf_nvgen, conf_buf_minfree,
	    OBJ(int,2), BUF_CMD_MAXFREE,
	    "max-free", "Maximum number of free buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_perm, conf_buf_nvgen, conf_buf_maxfree,
	    OBJ(int,2), BUF_CMD_PERM,
	    "permanent", "Number of permanent buffers", PRIV_CONF);

/******************************************************************
 * { small | middle | big | large | huge | <interface> } 
 *
 * OBJ(int,1) = BUF_CMD_SMALL, BUF_CMD_MIDDLE, BUF_CMD_BIG, BUF_CMD_LARGE, BUF_CMD_HUGE
 */

INTERFACE  (conf_buf_interface, conf_buf_perm, no_alt, 
	    OBJ(idb,1), ~(IFTYPE_FDDI | IFTYPE_NULL)); 
KEYWORD_ID (conf_buf_huge, conf_buf_perm, conf_buf_interface,
	    OBJ(int,1), BUF_CMD_HUGE,
	    "huge", "Huge buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_large, conf_buf_perm, conf_buf_huge,
	    OBJ(int,1), BUF_CMD_LARGE,
	    "large", "Large buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_vbig, conf_buf_perm, conf_buf_large,
	    OBJ(int,1), BUF_CMD_VERYBIG,
	    "verybig", "Very Big buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_big, conf_buf_perm, conf_buf_vbig,
	    OBJ(int,1), BUF_CMD_BIG,
	    "big", "Big buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_middle, conf_buf_perm, conf_buf_big,
	    OBJ(int,1), BUF_CMD_MIDDLE,
	    "middle", "Middle buffers", PRIV_CONF);
KEYWORD_ID (conf_buf_small, conf_buf_perm, conf_buf_middle,
	    OBJ(int,1), BUF_CMD_SMALL,
	    "small", "Small buffers", PRIV_CONF);


/******************************************************************/
KEYWORD	(conf_buffers, conf_buf_small, ALTERNATE,
	 "buffers", "Adjust system buffer pool parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_buffers
