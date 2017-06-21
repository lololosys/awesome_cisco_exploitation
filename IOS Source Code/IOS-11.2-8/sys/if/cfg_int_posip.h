/* $Id: cfg_int_posip.h,v 3.1.2.2 1996/08/12 16:02:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_posip.h,v $
 *------------------------------------------------------------------
 * cfg_int_posip.h - POSIP Parser support
 *
 * January 1996, Robert Broberg
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_posip.h,v $
 * Revision 3.1.2.2  1996/08/12  16:02:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/07/08  22:30:57  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/06/07  05:31:30  rbroberg
 * Add placeholders for new project
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *	transmitter-delay { <flags> }
 *	no transmitter-delay [ <flags> }]
 */
PARAMS_KEYONLY(ci_transdelay, NONE,
	       "transmitter-delay", OBJ(int,1), 0, 0x3FF,
	       transmitter_command, 0,
	       "Set flags to insert after transmitting a datagram",
	       "Number of flags<0-1023>",
	       PRIV_CONF);


/******************************************************************
 * posi internal-clock
 * no posi internal-clock
 */
EOLNS	(ci_posi_internal_eol, posi_clock_cmd);

KEYWORD	(ci_posi_internal, ci_posi_internal_eol, ci_transdelay,
	 "internal-clock", "Use internal clock for transmit clock source",
	 PRIV_CONF);

/******************************************************************
 * posi framing-sdh
 * no posi framing-sdh
 */
EOLNS	(ci_posi_framing_eol, posi_framing_cmd);

KEYWORD	(ci_posi_framing, ci_posi_framing_eol, ci_posi_internal,
	 "framing-sdh", "select sdh framing", PRIV_CONF);

KEYWORD	(ci_posi, ci_posi_framing, NONE,
	 "posi", "Modify POSI parameters", PRIV_CONF);

TEST_IDBSTATUS(ci_posi_test, ci_posi, NONE, ALTERNATE, IDB_POS);

#undef	ALTERNATE
#define	ALTERNATE	ci_posi_test
