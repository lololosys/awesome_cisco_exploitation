/* $Id: cfg_int_transmit-buffers.h,v 3.2.60.1 1996/08/12 16:01:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/hes/cfg_int_transmit-buffers.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T R A N S M I T - B U F F E R S . H
 *
 * Feb 8, 1995, Chris Shaker
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_transmit-buffers.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:15:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/02  01:41:11  shaker
 * CSCdi32715:  no transmit-buffers command is urecognized by parser.
 * Change parser code so it is clear that 'backing-store' must follow
 * 'transmit-buffers', instead of implying that '<Return>' would be
 * acceptable.
 *
 * Revision 2.1  1995/07/02  10:51:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:07:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * [no] transmit-buffers backing-store
 */

EOLNS	(ci_transmit_buffers_eol, transmit_buffers_command);

KEYWORD (ci_transmit_buffers_backing_store, ci_transmit_buffers_eol,
	 no_alt, "backing-store",
	 "copy tx packets to hold queue when hw tx queue full",
	 PRIV_CONF);

KEYWORD	(ci_transmit_buffers, ci_transmit_buffers_backing_store, ALTERNATE,
	 "transmit-buffers", "configure transmit-buffers policies",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_transmit_buffers
