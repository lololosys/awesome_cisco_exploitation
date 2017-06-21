/* $Id: cfg_modem_autoconfigure_discovery.h,v 3.2 1995/11/17 18:54:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_modem_autoconfigure_discovery.h,v $
 *------------------------------------------------------------------
 * cfg_modem_autoconfigure_discovery.h -- parser for modem autorecognition
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_modem_autoconfigure_discovery.h,v $
 * Revision 3.2  1995/11/17  18:54:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:33:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:52  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/********************************************************************
 *
 * modem autoconfig discovery
 *
 */


EOLS (line_modem_autoconfig_disc_eol, modem_autoconfigure_command,
					PARSER_LINE_MODEM_AUTOCONFIGURE);

KEYWORD_ID (line_modem_autoconfig_disc, line_modem_autoconfig_disc_eol,
                                                no_alt,
            OBJ(int,1), MODEM_AUTOCONFIGURE_DISCOVERY,
           "discovery", "Attempt to determine modem type automatically",
                                                        PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	line_modem_autoconfig_disc
