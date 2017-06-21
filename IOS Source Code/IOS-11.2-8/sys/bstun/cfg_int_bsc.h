/* $Id: cfg_int_bsc.h,v 3.3 1996/02/13 21:27:29 rbatz Exp $
 * $Source: /release/111/cvs/Xsys/bstun/cfg_int_bsc.h,v $
 *------------------------------------------------------------------
 * cfg_int_bsc.h - BSC Interface configuration commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_bsc.h,v $
 * Revision 3.3  1996/02/13  21:27:29  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.2  1995/11/17  08:49:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/03  01:04:11  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.1  1995/07/04  09:35:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/16  06:23:10  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  22:03:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*----------------------------------------------------------------------------
 * The following bi-synchronous commands are defined for use, when
 * configuring a serial interface:
 *
 * bsc fdx
 * bsc pause <tenths-of-secs>
 * bsc servlim <servlim-count>
 * bsc poll-timeout <tenths-of-secs>
 * bsc host-timeout <tenths-of-secs>
 * bsc retries <retry-count>
 * bsc spec-poll
 * bsc primary
 * bsc secondary
 * bsc contention
 * bsc char-set [ebcdic|ascii]
 * 
 ----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * [no] bsc fdx
 *
 */

EOLS	(ci_bsc_fdx_eol, bscif_command, BSC_CMD_FDX);
NVGENS	(ci_bsc_fdx_nvgen, NONE, bscif_command, BSC_CMD_FDX);
KEYWORD	(ci_bsc_fdx, ci_bsc_fdx_eol, no_alt,
         "fdx",
         "Define this interface as a full-duplex line",
         PRIV_CONF | PRIV_HIDDEN);

/*----------------------------------------------------------------------------
 * [no] bsc pause <tenths-of-secs>
 */

EOLS    (ci_bsc_pause_eol, bscif_command, BSC_CMD_PAUSE);
NOPREFIX(ci_bsc_pause_noprefix, ci_bsc_pause_eol, ci_bsc_pause_eol );
NUMBER  (ci_bsc_pause_val, ci_bsc_pause_noprefix, no_alt, OBJ(int,1), 
		1, 1000, "Pause timeout value in tenths of seconds" );
NVGENS  (ci_bsc_pause_nvgens, ci_bsc_pause_val, bscif_command, 
		BSC_CMD_PAUSE);
KEYWORD (ci_bsc_pause, ci_bsc_pause_nvgens, ci_bsc_fdx, "pause", 
		"Specify pause timeout in tenths of seconds", PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc servlim <value>
 */

EOLS    (ci_bsc_servlim_eol, bscif_command, BSC_CMD_SERVLIM);
NOPREFIX(ci_bsc_servlim_noprefix, ci_bsc_servlim_eol, ci_bsc_servlim_eol );
NUMBER  (ci_bsc_servlim_val, ci_bsc_servlim_noprefix, no_alt, OBJ(int,1), 
		1, 100, "Servlim value as a number");
NVGENS  (ci_bsc_servlim_nvgens, ci_bsc_servlim_val, bscif_command, 
		BSC_CMD_SERVLIM);
KEYWORD (ci_bsc_servlim, ci_bsc_servlim_nvgens, ci_bsc_pause, "servlim", 
		"Specify servlim (active v's inactive end-station poll ratio) as a value", PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc poll-timeout <tenths-of-secs>
 */

EOLS    (ci_bsc_polltimeout_eol, bscif_command, BSC_CMD_POLL_TIMEOUT);
NOPREFIX(ci_bsc_polltimeout_noprefix, ci_bsc_polltimeout_eol, ci_bsc_polltimeout_eol );
NUMBER  (ci_bsc_polltimeout_val, ci_bsc_polltimeout_noprefix, no_alt, OBJ(int,1), 
		1, 1000, "Poll timeout value in tenths of seconds" );
NVGENS  (ci_bsc_polltimeout_nvgens, ci_bsc_polltimeout_val, bscif_command, 
		BSC_CMD_POLL_TIMEOUT);
KEYWORD (ci_bsc_polltimeout, ci_bsc_polltimeout_nvgens, ci_bsc_servlim, "poll-timeout", 
		"Specify poll-timeout in tenths of seconds", PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc host-timeout <decisecs>
 */

EOLS    (ci_bsc_hosttimeout_eol, bscif_command, BSC_CMD_HOST_TIMEOUT);
NOPREFIX(ci_bsc_hosttimeout_noprefix, ci_bsc_hosttimeout_eol, ci_bsc_hosttimeout_eol );
NUMBER  (ci_bsc_hosttimeout_val, ci_bsc_hosttimeout_noprefix, no_alt, OBJ(int,1), 
		30, 3000, "Host inactivity timeout value in deciseconds" );
NVGENS  (ci_bsc_hosttimeout_nvgens, ci_bsc_hosttimeout_val, bscif_command, 
		BSC_CMD_HOST_TIMEOUT);
KEYWORD (ci_bsc_hosttimeout, ci_bsc_hosttimeout_nvgens, ci_bsc_polltimeout, "host-timeout", 
         "inactivity time for polls from the host in deciseconds", PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc retries <retry-count>
 */

EOLS    (ci_bsc_retries_eol, bscif_command, BSC_CMD_RETRIES);
NOPREFIX(ci_bsc_retries_noprefix, ci_bsc_retries_eol, ci_bsc_retries_eol );
NUMBER  (ci_bsc_retries_val, ci_bsc_retries_noprefix, no_alt, OBJ(int,1), 
		1, 100, "Number of retries" );
NVGENS  (ci_bsc_retries_nvgens, ci_bsc_retries_val, bscif_command, 
		BSC_CMD_RETRIES);
KEYWORD (ci_bsc_retries, ci_bsc_retries_nvgens, ci_bsc_hosttimeout, "retries", 
		"Specify number of retries", PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc spec-poll
 *
 */

EOLS	(ci_bsc_specpoll_eol, bscif_command, BSC_CMD_SPECPOLL);
NVGENS	(ci_bsc_specpoll_nvgen, NONE, bscif_command, BSC_CMD_SPECPOLL);
KEYWORD	(ci_bsc_specpoll, ci_bsc_specpoll_eol, ci_bsc_retries,
         "spec-poll",
         "Inform BSC local-ack to handle specific polls",
         PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc primary
 *
 */

EOLS	(ci_bsc_primary_eol, bscif_command, BSC_CMD_PRIMARY);
NVGENS	(ci_bsc_primary_nvgen, NONE, bscif_command,
	 BSC_CMD_PRIMARY);
KEYWORD	(ci_bsc_primary, ci_bsc_primary_eol, ci_bsc_specpoll,
         "primary",
         "Define this link to be the primary Bi-Synchronous station",
         PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc secondary
 *
 */

EOLS	(ci_bsc_secondary_eol, bscif_command, BSC_CMD_SECONDARY);
NVGENS	(ci_bsc_secondary_nvgen, NONE, bscif_command,
	 BSC_CMD_SECONDARY);
KEYWORD	(ci_bsc_secondary, ci_bsc_secondary_eol, ci_bsc_primary,
         "secondary",
         "Define this link to be the secondary Bi-Synchronous station",
         PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc contention
 *
 */

EOLS	(ci_bsc_contention_eol, bscif_command, BSC_CMD_CONTENTION);
NVGENS	(ci_bsc_contention_nvgen, NONE, bscif_command,
	 BSC_CMD_CONTENTION);
KEYWORD	(ci_bsc_contention, ci_bsc_contention_eol, ci_bsc_secondary,
         "contention",
         "Define this link to be a point-to-point station",
         PRIV_CONF);

/*----------------------------------------------------------------------------
 * [no] bsc char-set [ebcdic | ascii]
 *
 */

EOLS	(ci_bsc_charset_eol, bscif_command, BSC_CMD_CHARSET);
KEYWORD_ID(ci_bsc_charset_ebcdic, ci_bsc_charset_eol, no_alt,
	   OBJ(int,1), 0,
	   "ebcdic", "EBCDIC Character Set", PRIV_CONF);
KEYWORD_ID(ci_bsc_charset_ascii, ci_bsc_charset_eol,
           ci_bsc_charset_ebcdic,
	   OBJ(int,1), 1,
	   "ascii", "ASCII Character Set", PRIV_CONF);
NOPREFIX (ci_bsc_charset_noprefix, ci_bsc_charset_ascii,
	  ci_bsc_charset_eol);
NVGENS	(ci_bsc_charset_nvgen, ci_bsc_charset_noprefix,
         bscif_command, BSC_CMD_CHARSET);
KEYWORD	(ci_bsc_charset, ci_bsc_charset_nvgen, ci_bsc_contention,
	 "char-set",
	 "Defines the character set to be used on this interface",
         PRIV_CONF);

/*----------------------------------------------------------------------------
 * bsc
 */

KEYWORD (ci_bsc, ci_bsc_charset, NONE, "bsc",
        "BSC interface subcommands",PRIV_CONF);

ASSERT (ci_bsc_test, ci_bsc, ALTERNATE, 
	csb->interface->hwptr->status & IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	ci_bsc_test
