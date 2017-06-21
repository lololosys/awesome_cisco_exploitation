/* $Id: cfg_modem_autoconfigure.h,v 3.2 1995/11/17 18:54:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_modem_autoconfigure.h,v $
 *------------------------------------------------------------------
 * cfg_modem_autoconfigure.h  -- parser stuff for modem auto-configuration
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_modem_autoconfigure.h,v $
 * Revision 3.2  1995/11/17  18:54:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:32:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/27  04:18:55  tkolar
 * Files for the modemcap and modem_discovery subsystems.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 *  modem autoconfigure type [modemname]
 *
 */

LINK_TRANS(line_modem_autoconfigure_extend_here, no_alt);

EOLS    (line_modem_autoconfigure_eol, modem_autoconfigure_command,
                                        PARSER_LINE_MODEM_AUTOCONFIGURE);

STRING  (line_modem_autoconfigure_typestring, line_modem_autoconfigure_eol, no_alt,
            OBJ(string,1), "Modem type");

KEYWORD_ID (line_modem_autoconfigure_type, line_modem_autoconfigure_typestring,
				line_modem_autoconfigure_extend_here,
           OBJ(int,1), MODEM_AUTOCONFIGURE_TYPE,
           "type", "Specify modem type", PRIV_CONF);

NOPREFIX (line_modem_autoconfigure_no, line_modem_autoconfigure_type,
                line_modem_autoconfigure_eol);

NVGENS  (line_modem_autoconfigure_nvgen, line_modem_autoconfigure_no,
         modem_autoconfigure_command, PARSER_LINE_MODEM_AUTOCONFIGURE);

KEYWORD (line_modem_autoconfigure, line_modem_autoconfigure_nvgen,
            no_alt,
           "autoconfigure", "Automatically configure modem on line", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	line_modem_autoconfigure
