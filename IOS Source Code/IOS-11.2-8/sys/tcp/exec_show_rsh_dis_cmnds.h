/* $Id: exec_show_rsh_dis_cmnds.h,v 3.2.28.1 1996/08/12 16:08:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/exec_show_rsh_dis_cmnds.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ R S H _ D I S _ C M N D S
 *
 * 6/94 Paul Traina/Andy Heffernan
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: 
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 * show rsh-disable-commands
 */
EOLNS	(show_rsh_dis_cmnds_eol, show_rsh_disabled_commands);

KEYWORD (show_rsh_dis_cmnds, show_rsh_dis_cmnds_eol, ALTERNATE,
	 "rsh-disable-commands", NULL, PRIV_USER|PRIV_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_rsh_dis_cmnds
