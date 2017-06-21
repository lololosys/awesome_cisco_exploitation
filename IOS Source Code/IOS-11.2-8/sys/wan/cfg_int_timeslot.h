/* $Id: cfg_int_timeslot.h,v 3.4.12.2 1996/08/12 16:09:33 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_timeslot.h,v $
 *------------------------------------------------------------------
 * cfg_int_timeslot.h
 *
 * 08-June-1994, Rob Haragan
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cfg_int_timeslot.h,v $
 * Revision 3.4.12.2  1996/08/12  16:09:33  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.12.1  1996/03/18  22:47:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  07:54:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/13  02:11:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  21:49:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/14  23:44:34  snyder
 * CSCdi49032:  fix typo in serial help text
 *              with the typos fixed, 3 help messages match and the
 *              optimizer deals with placing only one copy in the .o
 *
 * Revision 3.3  1995/11/17  18:01:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:07:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * [no] timeslot <range_entry>
 *
 * <range_entry>:: <number> '-' <number>
 *
 *	OBJ(int,1) = start timeslot
 *	OBJ(int,2) = stop timeslot
 */
EOLNS	(ci_ts_eol, serial_timeslot_command);

GENERAL_NUMBER(ci_ts_num3, ci_ts_eol, NONE, OBJ(int,2),
	       0, 31, "Timeslots for the G.703 interface",
	       NUMBER_DEC | NUMBER_WS_OK | NO_NUMBER | NUMBER_HELP_CHECK);

CHARACTER(ci_ts_num2, ci_ts_num3, NONE, '-');

GENERAL_NUMBER(ci_ts_num1, ci_ts_num2, NONE, OBJ(int,1),
	       0, 31, "Timeslots for the G.703 interface",
	       NUMBER_DEC | NUMBER_NO_WS_OK | NO_NUMBER | NUMBER_HELP_CHECK);

NOPREFIX (ci_ts_noprefix, ci_ts_num1, ci_ts_eol);

NVGENNS (ci_ts_nvgen, ci_ts_noprefix, serial_timeslot_command);

KEYWORD	(ci_timeslot, ci_ts_nvgen, NONE, "timeslot",
	 "Timeslots for the G.703 interface",
	 PRIV_CONF);

ASSERT (config_timeslot_test, ci_timeslot, ALTERNATE, 
	is_g703(csb->interface->hwptr)); 

#undef ALTERNATE
#define ALTERNATE config_timeslot_test
