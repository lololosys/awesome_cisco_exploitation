/* $Id: cfg_exception.h,v 3.3 1995/11/30 00:02:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_exception.h,v $
 *------------------------------------------------------------------
 * C F G _ E X C E P T I O N . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_exception.h,v $
 * Revision 3.3  1995/11/30  00:02:52  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.2  1995/11/17  18:42:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:36:21  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/06/29  04:02:27  pst
 * CSCdi36559:  clean up hidden command help in parser
 *
 * Revision 2.1  1995/06/07  22:02:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * [no] exception spurious-interrupt <threshold>
 */

EOLS(exception_spurious_eol, exception_config, EXCEPTION_SPURIOUS);

DECIMAL(exception_spurious_threshold, exception_spurious_eol, no_alt,
	OBJ(int, 1), 1, -1, "Spurious interrupt threshold");
NOPREFIX(exception_spurious_noprefix, exception_spurious_threshold,
	 exception_spurious_eol);
NVGENS(exception_spurious_nvgen, exception_spurious_noprefix,
	exception_config, EXCEPTION_SPURIOUS);
KEYWORD(exception_spurious, exception_spurious_nvgen, no_alt,
	"spurious-interrupt",
	"Crash after a given number of spurious interrupts",
	PRIV_CONF);

/******************************************************************************
 * [no] exception memory { minimum | fragment } size
 */

PARAMS_KEYONLY(exception_memory_minimum, no_alt,
	       "minimum", OBJ(int,1), 0, -1, exception_config,
	       EXCEPTION_MEMMIN,
	       "Crash if free memory pool shrinks below limit",
	       "bytes", PRIV_CONF);

PARAMS_KEYONLY(exception_memory_fragment, exception_memory_minimum,
	       "fragment", OBJ(int,1), 0, -1, exception_config,
	       EXCEPTION_MEMFRG,
	       "Crash if we can't allocate contiguous memory below limit",
	       "bytes", PRIV_CONF);

KEYWORD(exception_memory, exception_memory_fragment, exception_spurious,
	"memory", "Memory leak debugging", PRIV_CONF);

/******************************************************************************
 * exception simulate
 */
#ifdef	GLOBAL_DEBUG
EOLS	(exception_crashdump_eol, exception_config, EXCEPTION_CRASHDUMP);
KEYWORD	(exception_crashdump, exception_crashdump_eol, exception_memory,
	 "crashdump", "Simulate a crashdump (crash router)", PRIV_CONF);
EOLS	(exception_simulate_eol, exception_config, EXCEPTION_SIMULATE);
KEYWORD	(exception_simulate, exception_simulate_eol, exception_crashdump,
	 "simulate", "Simulate an exception (crash router)", PRIV_CONF);
#else	GLOBAL_DEBUG
NOP (exception_simulate, exception_memory, NONE);
#endif	GLOBAL_DEBUG


/******************************************************************************
 * [no] exception dump { <ipaddr> | <hostname> }
 */
EOLS	(exception_dump_eol, exception_config, EXCEPTION_DUMP);
IPADDR_NAME (exception_dump_host, exception_dump_eol, no_alt,
	 OBJ(paddr,1), "Hostname or address of dump host");
NOPREFIX(exception_dump_noprefix, exception_dump_host, exception_dump_eol);
NVGENS	(exception_dump_nvgen, exception_dump_noprefix,
	 exception_config, EXCEPTION_DUMP);
KEYWORD	(exception_dump, exception_dump_nvgen, exception_simulate,
	 "dump", "Set name of host to dump to", PRIV_CONF);

/******************************************************************************
 * [no] exception region-size <size>
 */

EOLS(exception_regionsz_eol, exception_config, EXCEPTION_REGIONSIZE);

DECIMAL(exception_regionsz_value, exception_regionsz_eol, no_alt,
	OBJ(int,1), 1*1024, 64*1024, "Region size");
NOPREFIX(exception_regionsz_noprefix, exception_regionsz_value,
	exception_regionsz_eol);
NVGENS(exception_regionsz_nvgen, exception_regionsz_noprefix,
	exception_config, EXCEPTION_REGIONSIZE);
KEYWORD(exception_regionsz, exception_regionsz_nvgen, exception_dump,
	"region-size",
	"Size of region for exception-time memory pool",
	PRIV_CONF);

/******************************************************************************
 * [no] exception protocol { tftp | ... }
 * OBJ(int,1) = FILE_ACCESS_*
 */
EOLS	(exception_proto_eol, exception_config, EXCEPTION_PROTO);
LINK_TRANS(exception_proto_exit, exception_proto_eol);
LINK_TRANS(exception_proto_extend, no_alt);
KEYWORD_ID(exception_proto_tftp, exception_proto_eol, exception_proto_extend,
	 OBJ(int,1), FILE_ACCESS_TFTP,
	 "tftp", "TFTP protocol", PRIV_CONF);
NOPREFIX(exception_proto_noprefix, exception_proto_tftp, exception_proto_eol);
NVGENS	(exception_proto_nvgen, exception_proto_noprefix,
	 exception_config, EXCEPTION_PROTO);
KEYWORD	(exception_proto, exception_proto_nvgen, exception_regionsz,
	 "protocol", "Set protocol for sending core file", PRIV_CONF);

/******************************************************************************
 * [no] exception core-file <file-name>
 */
EOLS	(exception_core_eol, exception_config, EXCEPTION_CORE);
STRING	(exception_core_file, exception_core_eol, no_alt,
	 OBJ(string,1), "Name of the core file");
NOPREFIX(exception_core_noprefix, exception_core_file, exception_core_eol);
NVGENS	(exception_core_nvgen, exception_core_noprefix,
	 exception_config, EXCEPTION_CORE);
KEYWORD	(exception_core, exception_core_nvgen, exception_proto,
	 "core-file", "Set name of core dump file", PRIV_CONF);

/******************************************************************************
 * exception
 */
KEYWORD	(exception, exception_core, ALTERNATE,
	 "exception", "Exception handling", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	exception
