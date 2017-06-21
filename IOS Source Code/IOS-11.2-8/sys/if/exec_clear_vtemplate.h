/* $Id: exec_clear_vtemplate.h,v 3.1.2.2 1996/08/12 16:02:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_clear_vtemplate.h,v $
 *------------------------------------------------------------------
 * exec_clear_vtemplate.h - Virtual template clear command
 *
 * July 1996, Perry Leung
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_vtemplate.h,v $
 * Revision 3.1.2.2  1996/08/12  16:02:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/07/04  02:23:36  perryl
 * CSCdi61809:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Add "clear vtemplate" command to reset the vtemplate command history
 * buffer.
 *
 * Revision 3.1  1996/07/04  01:23:52  perryl
 * Placeholder for California_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear vtemplate
 *
 */
EOLNS	(clear_vtemplate_eol, vtemplate_clear_cmdbuf);
KEYWORD (clear_vtemplate, clear_vtemplate_eol, ALTERNATE,
	 "vtemplate", NULL /*"Clear virtual template's command buffer"*/,
	 PRIV_OPR|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	clear_vtemplate
