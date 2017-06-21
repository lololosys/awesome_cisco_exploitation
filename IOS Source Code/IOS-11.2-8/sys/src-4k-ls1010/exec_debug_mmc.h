/* $Id: exec_debug_mmc.h,v 3.1 1995/11/09 13:23:32 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-ls1010/exec_debug_mmc.h,v $
 *------------------------------------------------------------------
 * parser definition for controlling debug commands
 *
 * September 1995, Ram Bhide.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_mmc.h,v $
 * Revision 3.1  1995/11/09  13:23:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/27  14:48:55  jwilliam
 * initial creation
 *
 * Revision 2.2  1995/08/03  00:28:55  jjohnson
 * CSCdi38141:  template files contain bogus log information
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/****************************************************************************
 * debug mmc all
 *
 */
EOLS    (debug_mmc_all_eol, debug_command, DEBUG_MMC_ALL);
KEYWORD (debug_mmc_all, debug_mmc_all_eol, no_alt, "all", "MMC all", PRIV_OPR);

/***************************************************************************
 * debug mmc trace
 *
 */
EOLS    (debug_mmc_trace_eol, debug_command, DEBUG_MMC_TRACE);
KEYWORD (debug_mmc_trace, debug_mmc_trace_eol, debug_mmc_all,
         "trace", "mmc trace", PRIV_OPR);

/***************************************************************************
 * debug mmc error
 *
 */
EOLS    (debug_mmc_error_eol, debug_command, DEBUG_MMC_ERROR);
KEYWORD (debug_mmc_error, debug_mmc_error_eol, debug_mmc_trace,
         "errors", "mmc errors", PRIV_OPR);

/***************************************************************************
 * debug mmc
 *
 */
KEYWORD_DEBUG(debug_mmc, debug_mmc_error,ALTERNATE,
	      OBJ(pdb,1), mmc_debug_arr,
	      "mmc", "atm mmc", PRIV_OPR | PRIV_DUPLICATE);

#undef  ALTERNATE
#define ALTERNATE       debug_mmc
