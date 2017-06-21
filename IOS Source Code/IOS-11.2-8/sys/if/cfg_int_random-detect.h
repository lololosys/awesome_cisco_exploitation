/* $Id: cfg_int_random-detect.h,v 3.1.4.1 1996/04/17 13:40:05 fred Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_random-detect.h,v $
 *------------------------------------------------------------------
 * Interface specific configuration commands for fair queueing
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_random-detect.h,v $
 * Revision 3.1.4.1  1996/04/17  13:40:05  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *      random-detect <1-16>
 */
EOLS (ci_redq_eol, prioritygroup_command, QUEUE_RED);
SET (ci_redq_default, ci_redq_eol, OBJ (int, 1), RED_STANDARD_SHIFT);

NUMBER (ci_redq_threshold, ci_redq_eol, ci_redq_default, OBJ (int, 1),
	MINIMUM_RED_SHIFT, MAXIMUM_RED_SHIFT,
	"integer in 1..16, used in weighted average to mean 2^number");

NVGENS (ci_redq_nvgen, ci_redq_threshold, prioritygroup_command, QUEUE_RED);
KEYWORD (ci_redq, ci_redq_nvgen, ALTERNATE,
	 "random-detect", "Enable Random Early Detection (RED) on an Interface", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_redq
