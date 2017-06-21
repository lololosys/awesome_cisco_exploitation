/* $Id: cfg_int_traffic-shape.h,v 3.1.4.7 1996/08/12 16:02:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_traffic-shape.h,v $
 *------------------------------------------------------------------
 * Interface specific configuration commands for fair queueing
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_traffic-shape.h,v $
 * Revision 3.1.4.7  1996/08/12  16:02:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.4.6  1996/08/07  22:12:31  fred
 * CSCdi63525:  traceback on RSVP beta image, pointing to ip fastswitching
 *         introduce 'traffic-shape group' command
 *         remove traffic in shaping queue from input-hold queue.
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/07/20  03:18:31  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/06/04  23:12:44  fred
 * CSCdi58101:  <traffic shaping adaptive> crashes router on ethenet
 * interface
 *         Fix NULL dereference on entry to traffic_shape_adaptive(), and
 *         implement traffic-shape adaptive <lower bound>. The lower bound
 *         was left out.
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/06/04  19:36:45  fred
 * CSCdi58705:  traffic-shape command not on fr point to point interface
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/18  01:20:35  fred
 * CSCdi57567:  traffic shaping needs units changed on commands.
 *         This was done at the request of the Documentation folks,
 *         to normalize the parameters for per-interface traffic shaping,
 *         per-dlci traffic-shaping, and per-svc traffic-shaping.
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/17  13:40:06  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *	traffic-shape fecn-create
 */
EOLNS(ci_shape_fecn_create_eol, shape_command_fecn_create);
KEYWORD(ci_shape_fecn_create, ci_shape_fecn_create_eol, no_alt, "fecn-create",
	"Enable setting of FECN on shaped traffic",
	PRIV_HIDDEN | PRIV_CONF | PRIV_SUBIF);

ASSERT(ci_shape_fecn_create_check, ci_shape_fecn_create, no_alt,
	is_frame_relay(csb->interface->hwptr));

/**********************************************************************
 *	traffic-shape fecn-adapt
 */
EOLNS(ci_shape_fecn_adapt_eol, shape_command_fecn_adapt);
KEYWORD(ci_shape_fecn_adapt, ci_shape_fecn_adapt_eol, no_alt, "fecn-adapt",
	"Enable Traffic Shaping reflection of FECN as BECN",
	PRIV_CONF | PRIV_SUBIF);

ASSERT(ci_shape_fecn_adapt_check, ci_shape_fecn_adapt,
	ci_shape_fecn_create_check,
	is_frame_relay(csb->interface->hwptr));

/**********************************************************************
 *      traffic-shape adaptive
 */
EOLNS(ci_shape_adaptive_eol, shape_command_adaptive);
NUMBER(ci_shape_lower_bound, ci_shape_adaptive_eol, ci_shape_adaptive_eol,
	OBJ(int,1), 1, 100000000,
       "Lower Bound Target Bit Rate (bits per second)");
NVGENNS(ci_shape_nvgena, ci_shape_lower_bound, shape_command_adaptive);
NOPREFIX(ci_shape_noprefixa, ci_shape_nvgena, ci_shape_adaptive_eol);
KEYWORD(ci_shape_adaptive, ci_shape_noprefixa, ci_shape_fecn_adapt_check,
	"adaptive", "Enable Traffic Shaping adaptation to BECN",
	PRIV_CONF | PRIV_SUBIF);

ASSERT(ci_shape_adaptive_limit, ci_shape_adaptive, no_alt,
       is_frame_relay(csb->interface->hwptr));

/**********************************************************************
 *      traffic-shape group <access-list> <target-bit-rate>
 *      		[[bits-per-interval>]
 *      		 [excess-bits-first-interval>]]
 *
 *	OBJ(int,1) = target bit rate (bits per second)
 *	OBJ(int,2) = sustained bits per interval
 *	OBJ(int,3) = excess bits in first interval
 *	OBJ(int,4) = access list that applies
 */
EOLS(cj_shape_eol, shape_command, TRAFFIC_SHAPE_GROUP);

SET(cj_shape_default_Be, cj_shape_eol, OBJ(int,3), MAXUINT);
SET(cj_shape_default_Bc, cj_shape_default_Be, OBJ(int,2), MAXUINT);

NUMBER(cj_shape_Be, cj_shape_eol,
       cj_shape_default_Be, OBJ(int,3), 0, 100000000,
       "bits per interval, excess in first interval");
NUMBER(cj_shape_Bc, cj_shape_Be,
       cj_shape_default_Bc, OBJ(int,2), 0, 100000000,
       "bits per interval, sustained");
NUMBER(cj_shape_cjr, cj_shape_Bc, no_alt, OBJ(int,1), 1, 100000000,
       "Target Bit Rate (bits per second)");

NOPREFIX(cj_shape_noprefix, cj_shape_cjr, cj_shape_eol);
NUMBER (cj_shape_acl_list, cj_shape_noprefix, no_alt,
        OBJ(int,4), 1, MAXACCESSGROUP, "selecting Access list");
NVGENS(cj_shape_nvgen, cj_shape_acl_list, shape_command, TRAFFIC_SHAPE_GROUP);
KEYWORD(ci_shape_group, cj_shape_nvgen, ci_shape_adaptive_limit, "group",
	"configure token bucket: group <access-list> "
	"CIR (bps) [Bc (bits) [Be (bits)]] ", PRIV_CONF | PRIV_SUBIF);

/**********************************************************************
 *      traffic-shape rate <target-bit-rate>
 *      		[[bits-per-interval>]
 *      		 [excess-bits-first-interval>]]
 *
 *	OBJ(int,1) = target bit rate (bits per second)
 *	OBJ(int,2) = sustained bits per interval
 *	OBJ(int,3) = excess bits in first interval
 *	OBJ(int,4) = 0
 */

EOLS(ci_shape_eol, shape_command, TRAFFIC_SHAPE_RATE);

SET(ci_shape_default_Be, ci_shape_eol, OBJ(int,3), MAXUINT);
SET(ci_shape_default_Bc, ci_shape_default_Be, OBJ(int,2), MAXUINT);

NUMBER(ci_shape_Be, ci_shape_eol,
       ci_shape_default_Be, OBJ(int,3), 0, 100000000,
       "bits per interval, excess in first interval");
NUMBER(ci_shape_Bc, ci_shape_Be,
       ci_shape_default_Bc, OBJ(int,2), 0, 100000000,
       "bits per interval, sustained");
NUMBER(ci_shape_cir, ci_shape_Bc, no_alt, OBJ(int,1), 1, 100000000,
       "Target Bit Rate (bits per second)");

NVGENS(ci_shape_nvgen, ci_shape_cir, shape_command, TRAFFIC_SHAPE_RATE);
NOPREFIX(ci_shape_noprefix, ci_shape_nvgen, ci_shape_eol);
KEYWORD(ci_shape_rate, ci_shape_noprefix, ci_shape_group, "rate",
	"configure token bucket: CIR (bps) [Bc (bits) [Be (bits)]] ",
	PRIV_CONF | PRIV_SUBIF);

KEYWORD(ci_shape, ci_shape_rate, ALTERNATE, "traffic-shape",
	"Enable Traffic Shaping on an Interface or Sub-Interface",
	PRIV_CONF | PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_shape
