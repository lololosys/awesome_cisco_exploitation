/* $Id: exec_slave_sync.h,v 3.3.58.1 1996/08/12 16:07:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/exec_slave_sync.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * September 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_slave_sync.h,v $
 * Revision 3.3.58.1  1996/08/12  16:07:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/24  21:33:12  yanke
 * Change slave sync command to be privileged.
 * Raw nvram access shouldn't update rom environment variables
 *
 * CSCdi42679:  new exec command should be privileged
 *
 * Revision 3.2  1995/11/17  18:46:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/03  19:39:32  yanke
 * Place holder file for new development.
 *
 * CSCdi41411:  slave config sync needs implementing
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 * slave-sync
 *
 */

EOLNS (exec_slave_sync_cfg_eol, slave_sync_command);

KEYWORD (exec_slave_sync_cfg_kwd, exec_slave_sync_cfg_eol, NONE,
	"config", "Sync master startup config to slave", PRIV_ROOT);

KEYWORD (exec_slave_sync_kwd, exec_slave_sync_cfg_kwd, NONE,
	"sync", "Sync between master and slave", PRIV_ROOT);

KEYWORD (exec_slave_kwd, exec_slave_sync_kwd, ALTERNATE,
	"slave", "Manage dual 7500 slave", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_slave_kwd
