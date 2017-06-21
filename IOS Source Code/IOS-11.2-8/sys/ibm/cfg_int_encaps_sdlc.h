/* $Id: cfg_int_encaps_sdlc.h,v 3.2 1995/11/27 16:23:40 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/cfg_int_encaps_sdlc.h,v $
 *------------------------------------------------------------------
 * SDLC interface encapsulation commands
 *
 * November 1995, Robert Widmer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_encaps_sdlc.h,v $
 * Revision 3.2  1995/11/27  16:23:40  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.1  1995/11/22  22:21:10  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Initial version
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


LINK_EXIT(cfg_encap_sdlc_exit, no_alt);

/**********************************************************************
 * [no] encapsulation sdlc-secondary
 */
KEYWORD_ID_MM(config_encap_sdlcsec1, cfg_encap_sdlc_exit, NONE,
	      OBJ(int,1), ET_SDLCS,
	      "sdlc-secondary", "SDLC (secondary)", PRIV_CONF, 6);

/**********************************************************************
 * [no] encapsulation sdlc-primary
 */
KEYWORD_ID_MM(config_encap_sdlcprim1, cfg_encap_sdlc_exit,
	      config_encap_sdlcsec1,
	      OBJ(int,1), ET_SDLCP,
	      "sdlc-primary", "SDLC (primary)", PRIV_CONF, 6);
TEST_IDBSTATUS(config_encap_sdlcprim, config_encap_sdlcprim1,
	       NONE, ALTERNATE, IDB_SYNC);

/**********************************************************************
 * [no] encapsulation sdlc
 */
ENCAPS	(config_encap_sdlc, cfg_encap_sdlc_exit, config_encap_sdlcprim,
	 OBJ(int,1), ET_SDLC,
	 "sdlc", "SDLC",
	 IDB_SYNC);

#undef	ALTERNATE
#define	ALTERNATE	config_encap_sdlc
