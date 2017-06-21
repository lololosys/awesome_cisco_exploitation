/* $Id: cfg_modem_busyout.h,v 3.1 1996/01/12 20:03:24 sjackson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ts/cfg_modem_busyout.h,v $
 *------------------------------------------------------------------
 * cfg_modem_busyout.h  -- parser stuff for modem busyout
 *
 * January 1996, Scott Jackson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_modem_busyout.h,v $
 * Revision 3.1  1996/01/12  20:03:24  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 *  [no] modem busyout
 *
 */

LINK_TRANS(line_modem_busyout_extend_here, no_alt);

EOLS    (line_modem_busyout_eol, modem_busyout_command,
                                        PARSER_LINE_MODEM_BUSYOUT);

KEYWORD (line_modem_busyout, line_modem_busyout_eol,
            line_modem_busyout_extend_here,
           "busyout", "Block calls to and from the modem", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	line_modem_busyout
