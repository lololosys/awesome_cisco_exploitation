/* $Id: cfg_int_autodetect.h,v 3.1.2.2 1996/08/12 16:09:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_autodetect.h,v $
 *------------------------------------------------------------------
 * cfg_int_autodetect.h   Parser chains for serial autodetect
 *
 * July 1996, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_autodetect.h,v $
 * Revision 3.1.2.2  1996/08/12  16:09:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/07/28  07:33:20  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.1  1996/07/28  07:22:12  irfan
 * files needed for serial autodetect.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 *
 * [no]  autodetect encapsulation { ppp|v120|frame-relay|x25|hdlc|smds }
 *
 * [no]  autodetect sniff-packets maximum n
 *
 * Note: not all commands are implemented at this time.
 *
 */

pdecl(cfg_autodetect_enc_types);

EOLNS	(cfg_autodetect_eol, autodetect_command);

ASSERT  (cfg_autodetect_eol_kw_test, cfg_autodetect_eol,
	 NONE,
	 (GETOBJ(int, 1) != 0));
/*
 * Frame-Relay
 */
KEYWORD_ORTEST (cfg_autodetect_fr, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_FR,
		"frame-relay",
		NULL /*"Autodetect frame-relay encapsulations on interface"*/,
		(PRIV_CONF|PRIV_HIDDEN));

ASSERT  (cfg_autodetect_fr_kw_test, cfg_autodetect_fr, NONE,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_FR)));


/*
 * SMDS
 */
KEYWORD_ORTEST (cfg_autodetect_smds, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_SMDS,
		"smds", NULL /*"Autodetect SMDS encapsulations on interface"*/,
		(PRIV_CONF|PRIV_HIDDEN));

ASSERT  (cfg_autodetect_smds_kw_test, cfg_autodetect_smds,
	 cfg_autodetect_fr_kw_test,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_SMDS)));


/*
 * X25
 */
KEYWORD_ORTEST (cfg_autodetect_x25, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_X25,
		"x25", NULL /*"Autodetect X25 encapsulations on interface"*/,
		(PRIV_CONF|PRIV_HIDDEN));

ASSERT  (cfg_autodetect_x25_kw_test, cfg_autodetect_x25,
	 cfg_autodetect_smds_kw_test,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_X25)));


/*
 * HDLC
 */
KEYWORD_ORTEST (cfg_autodetect_hdlc, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_HDLC,
		"hdlc", NULL /*"Autodetect HDLC encapsulations on interface"*/,
		(PRIV_CONF|PRIV_HIDDEN));

ASSERT  (cfg_autodetect_hdlc_kw_test, cfg_autodetect_hdlc,
	 cfg_autodetect_x25_kw_test,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_HDLC)));


/*
 * V120
 */
KEYWORD_ORTEST (cfg_autodetect_v120, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_V120,
		"v120", "Autodetect V120 encapsulations on B-channels",
		PRIV_CONF);

ASSERT  (cfg_autodetect_v120_test, cfg_autodetect_v120, NONE,
	 ((is_isdn(csb->interface->hwptr)) ||
	  (IDBTYPE_DIALER == csb->interface->hwptr->type)));

ASSERT  (cfg_autodetect_v120_kw_test, cfg_autodetect_v120_test,
	 cfg_autodetect_hdlc_kw_test,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_V120)));

/*
 * PPP
 */
KEYWORD_ORTEST (cfg_autodetect_ppp, cfg_autodetect_enc_types, NONE,
		OBJ(int,1), AUTODETECT_PARSE_PPP,
		"ppp", "Autodetect PPP encapsulations on interface",
		(PRIV_CONF));

ASSERT  (cfg_autodetect_ppp_kw_test, cfg_autodetect_ppp,
	 cfg_autodetect_v120_kw_test,
	 !((GETOBJ(int, 1) & AUTODETECT_PARSE_PPP)));


NOP     (cfg_autodetect_enc_types, cfg_autodetect_ppp_kw_test,
	 cfg_autodetect_eol_kw_test);

NOPREFIX  (cfg_autodetect_enc_noprefix, cfg_autodetect_enc_types,
	   cfg_autodetect_eol);

NVGENNS  (cfg_autodetect_enc_nvgens, cfg_autodetect_enc_noprefix,
	 autodetect_command);

KEYWORD	(cfg_autodetect_encaps, cfg_autodetect_enc_nvgens, no_alt,
	 "encapsulation", "list of allowed encapsulations for autodetect",
	 PRIV_CONF);

KEYWORD	(cfg_autodetect, cfg_autodetect_encaps, no_alt,
	 "autodetect", "Autodetect Encapsulations on Serial interface",
	 PRIV_CONF);

ASSERT (cfg_autodetect_assert, cfg_autodetect, ALTERNATE,
	(csb->interface->hwptr->status & IDB_SERIAL));

#undef	ALTERNATE
#define	ALTERNATE	cfg_autodetect_assert
