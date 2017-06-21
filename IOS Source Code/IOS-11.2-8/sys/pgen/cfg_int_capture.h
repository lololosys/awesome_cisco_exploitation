/* $Id: cfg_int_capture.h,v 3.2.62.1 1996/08/12 16:06:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pgen/cfg_int_capture.h,v $
 *------------------------------------------------------------------
 * Enables Pagent Capture mode on the specified interface
 * Jan 1995, skoh
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_capture.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:56:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/14  01:51:56  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.1  1995/06/07  22:33:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*******************************************************************
 * syntax: interface <interface>
 *         capture <number of packets to be captured, 1-30> 
 *
 * descr:  This command enables capturing mode on the specified interface.
 *         Max number of packets to be captured is 30 and default is 10.
 *
 * OBJ(int, 1) = <number of packets to be captured>
 *
 */


PARAMS_KEYONLY(ci_capture, ALTERNATE, "capture",
	       OBJ(int,1), 1, 100,
	       capture_command, 0,
	       "Put the selected interface into capture mode",
	       "Number of packets to be captured", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_capture
