/* $Id: cfg_slave_exception.h,v 3.2 1995/11/17 18:45:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_slave_exception.h,v $
 *------------------------------------------------------------------
 * C F G _ _ S L A V E _ E X C E P T I O N . H
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_slave_exception.h,v $
 * Revision 3.2  1995/11/17  18:45:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:13:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/******************************************************************************
 * [no] exception-slave dump { <ipaddr> | <hostname> }
 */
EOLS	(sl_exception_dump_eol, sl_exception_config, EXCEPTION_DUMP);
IPADDR_NAME (sl_exception_dump_host, sl_exception_dump_eol, no_alt,
	 OBJ(paddr,1), "Hostname or address of dump host");
NOPREFIX(sl_exception_dump_noprefix, sl_exception_dump_host, 
	 sl_exception_dump_eol);
NVGENS	(sl_exception_dump_nvgen, sl_exception_dump_noprefix,
	 sl_exception_config, EXCEPTION_DUMP);
KEYWORD	(sl_exception_dump, sl_exception_dump_nvgen, no_alt,
	 "dump", "", PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
 * [no] exception-slave protocol { tftp | rcp }
 * OBJ(int,1) = FILE_ACCESS_*
 */
EOLS	(sl_exception_proto_eol, sl_exception_config, EXCEPTION_PROTO);
KEYWORD_ID(sl_exception_proto_rcp, sl_exception_proto_eol, no_alt,
	 OBJ(int,1), FILE_ACCESS_RCP,
	 "rcp", "RCP protocol", PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID(sl_exception_proto_tftp, sl_exception_proto_eol, 
	 sl_exception_proto_rcp, OBJ(int,1), FILE_ACCESS_TFTP,
	 "tftp", "TFTP protocol", PRIV_CONF|PRIV_HIDDEN);
NOPREFIX(sl_exception_proto_noprefix, sl_exception_proto_tftp,
	 sl_exception_proto_eol);
NVGENS	(sl_exception_proto_nvgen, sl_exception_proto_noprefix,
	 sl_exception_config, EXCEPTION_PROTO);
KEYWORD	(sl_exception_proto, sl_exception_proto_nvgen, sl_exception_dump,
	 "protocol", "", PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
 * [no] exception-slave core-file <file-name>
 */
EOLS	(sl_exception_core_eol, sl_exception_config, EXCEPTION_CORE);
STRING	(sl_exception_core_file, sl_exception_core_eol, no_alt,
	 OBJ(string,1), "Name of the core file");
NOPREFIX(sl_exception_core_noprefix, sl_exception_core_file, 
	sl_exception_core_eol);
NVGENS	(sl_exception_core_nvgen, sl_exception_core_noprefix,
	 sl_exception_config, EXCEPTION_CORE);
KEYWORD	(sl_exception_core, sl_exception_core_nvgen, sl_exception_proto,
	 "core-file", "", PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
 * exception-slave
 */
KEYWORD_MM (sl_exception, sl_exception_core, ALTERNATE, "exception-slave",
	 "", PRIV_CONF|PRIV_HIDDEN, 11);

#undef  ALTERNATE
#define ALTERNATE	sl_exception
