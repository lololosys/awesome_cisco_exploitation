/* $Id: cfg_int_fddi_cbus.h,v 3.2 1995/11/17 09:15:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/cfg_int_fddi_cbus.h,v $
 *------------------------------------------------------------------
 * cfg_int_fddi_cbus.h - parse chain definitions for cBus FDDI
 *
 * November 1993, Scott Mackie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_fddi_cbus.h,v $
 * Revision 3.2  1995/11/17  09:15:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:05:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * fddi tb-min <num>
 * no fddi tb-min
 *
 */
PARAMS_KEYONLY(fddi_tbmin, no_alt, "tb-min",
	       OBJ(int,1), 0, 0xffff,
	       fddi_config_commands, FDDI_INTFC_TB_MIN,
	       "Timer values in cmt state machine",
	       "Microseconds", PRIV_CONF );

/******************************************************************
 * fddi t-out <number>
 * no fddi t-out
 */
PARAMS_KEYONLY(fddi_tout, fddi_tbmin, "t-out",
	       OBJ(int,1), 0, 0xffff,
	       fddi_config_commands, FDDI_INTFC_T_OUT,
	       "Timer values in cmt state machine",
	       "Milliseconds", PRIV_CONF );

/******************************************************************
 * [no] fddi smt-frames
 *
 */
EOLS	(fddi_smt_eol, fddi_config_commands, FDDI_INTFC_SMT_FRAMES);
KEYWORD	(fddi_smt, fddi_smt_eol, fddi_tout,
	 "smt-frames", "Enable handling of SMT frames", PRIV_CONF );

/******************************************************************
 * [no] fddi duplicate-address-check
 *
 */
EOLS	(fddi_duplicate_eol, fddi_config_commands, FDDI_INTFC_DUP_CHECK);
KEYWORD	(fddi_duplicate, fddi_duplicate_eol, fddi_smt,
	 "duplicate-address-check", "Enable duplicate address checking",
	 PRIV_CONF );

/******************************************************************
 * fddi cmt-signal-bits <signal-bits> { phy-a | phy-b }
 *
 * OBJ(int,1) = signal bits
 * OBJ(int,3) = FDDI_PHY_A, FDDI_PHY_B, or FDDI_PHY_BOTH
 */
EOLS	(fddi_cmtsigbits_eol, fddi_config_commands, FDDI_INTFC_CMT_S_BITS);
SET	(fddi_cmtsigbits_default, fddi_cmtsigbits_eol, OBJ(int,3),
	 FDDI_PHY_BOTH );
KEYWORD_ID (fddi_phyb, fddi_cmtsigbits_eol, fddi_cmtsigbits_default,
	 OBJ(int,3), FDDI_PHY_B,
	 "phy-b", "Select the PHY B physical sublayer", PRIV_CONF);
KEYWORD_ID (fddi_phya, fddi_cmtsigbits_eol, fddi_phyb,
	 OBJ(int,3), FDDI_PHY_A,
	 "phy-a", "Select the PHY A physical sublayer", PRIV_CONF);

ZEROXHEXNUM(fddi_cmtsigbits_hex, fddi_phya, no_alt,
	    OBJ(int,1), "Signal bits (in hex)");

NVGENS	(fddi_cmtsigbits_nvgen, fddi_cmtsigbits_hex,
	 fddi_config_commands, FDDI_INTFC_CMT_S_BITS);
KEYWORD	(fddi_cmtsigbits, fddi_cmtsigbits_nvgen, 
	 fddi_duplicate, "cmt-signal-bits",
	 "Control information transmitted during CMT", PRIV_CONF);

/******************************************************************
 * fddi c-min <num>
 * no fddi c-min
 */
PARAMS_KEYONLY(fddi_cmin, fddi_cmtsigbits, "c-min",
	       OBJ(int,1), 0, 0xffff,
	       fddi_config_commands, FDDI_INTFC_C_MIN,
	       "Timer values in cmt state machine",
	       "Microseconds", PRIV_CONF );

/******************************************************************
 * fddi burst-count <number>
 * no fddi burst_count
 */
PARAMS_KEYONLY(fddi_burst, fddi_cmin, "burst-count",
	       OBJ(int,1), 1, 10,
	       fddi_config_commands, FDDI_INTFC_BURST_COUNT,
	       "Pre-allocate receive buffers for bursty traffic",
	       "Number of buffers, default = 3", PRIV_CONF );

TEST_IDBSTATUS(config_fddi_hes, fddi_burst, NONE, ALTERNATE, IDB_FDDI);

#undef	ALTERNATE
#define	ALTERNATE	config_fddi_hes

