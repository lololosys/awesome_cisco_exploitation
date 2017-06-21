/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * C F G _ L I N E _ M O T D - B A N N E R . H
 *
 * April 1997, Edward Cheng
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log$
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * motd-banner
 * no motd-banner
 */
EOLS	(cl_motdbanner_eol, line_command, LINE_MOTD_BANNER);
KEYWORD	(cl_motdbanner_kwd, cl_motdbanner_eol, NONE,
	 "motd-banner", "Enable the display of the MOTD banner", PRIV_CONF);
TEST_MULTIPLE_FUNCS(cl_motdbanner, cl_motdbanner_kwd, NONE, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	cl_motdbanner
