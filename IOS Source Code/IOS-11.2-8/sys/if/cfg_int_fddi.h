/* $Id: cfg_int_fddi.h,v 3.2 1995/11/17 09:26:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/cfg_int_fddi.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ F D D I . H
 *
 * fddi token-rotation-time <microseconds>
 * fddi valid-transmission-time <microseconds>
 * fddi tl-min-time <microseconds>
 * fddi cmt-signal-bits <signal-bits> {phy-a | phy-b}
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_fddi.h,v $
 * Revision 3.2  1995/11/17  09:26:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:04:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_int_fddi_extend_here, no_alt);

/*****************************************************************
 * [no] fddi encapsulate
 */
EOLS	(fddi_encap_eol, fddi_config_commands, FDDI_INTFC_ENCAP);
KEYWORD (fddi_encap, fddi_encap_eol, cfg_int_fddi_extend_here,
	 "encapsulate", "Enable FDDI Encapsulation bridging",
	 PRIV_CONF);

/*****************************************************************
 * fddi valid-transmission-time <microseconds>
 *
 * OBJ(int,1) = microseconds
 */
PARAMS_KEYONLY(fddi_xmittime, fddi_encap, "valid-transmission-time",
	       OBJ(int,1), DEFAULT_T_TVX_TIME, 0x7fffffff,
	       fddi_config_commands, FDDI_INTFC_TVX,
	       "Set transmission valid timer",
	       "Microseconds", PRIV_CONF);

/******************************************************************
 * fddi token-rotation-time <microseconds>
 *
 * OBJ(int,1) = microseconds
 */
PARAMS_KEYONLY(fddi_rottime, fddi_xmittime, "token-rotation-time",
	       OBJ(int,1), DEFAULT_T_MIN_TIME, DEFAULT_T_MAX_TIME,
	       fddi_config_commands, FDDI_INTFC_TRT,
	       "Set the token rotation timer",
	       "Microseconds", PRIV_CONF);

/******************************************************************
 * fddi tl-min-time <microseconds>
 *
 * OBJ(int,1) = microseconds
 */
PARAMS_KEYONLY(fddi_tlmintime, fddi_rottime, "tl-min-time",
	       OBJ(int,1), 0, -1,
	       fddi_config_commands, FDDI_INTFC_TL_MIN,
	       "Line state transmission time",
	       "Microseconds", PRIV_CONF);

KEYWORD	(config_fddi, fddi_tlmintime, NONE,
	 "fddi", "FDDI interface parameters", PRIV_CONF);

TEST_IDBSTATUS(config_fddi_test, config_fddi, NONE, ALTERNATE, IDB_FDDI);

#undef	ALTERNATE
#define	ALTERNATE	config_fddi_test
