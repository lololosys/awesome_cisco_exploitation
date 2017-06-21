/* $Id: cfg_int_cpp.h,v 3.1.2.1 1996/05/10 01:44:19 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_cpp.h,v $
 *------------------------------------------------------------------
 *  C F G _ I N T _ C P P . H
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_cpp.h,v $
 * Revision 3.1.2.1  1996/05/10  01:44:19  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:35  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if 0
 /***************************************************************
  * [no] cpp multilink 
  */
EOLS(cfg_cpp_multilink_eol, 
     cpp_command, 
     cpp_MULTILINK);
 
NVGENS(cfg_cpp_multilink_nvgen, 
       cfg_cpp_multilink_eol, 
       cpp_command, 
       cpp_MULTILINK);

KEYWORD(cfg_cpp_multilink, 
 	cfg_cpp_multilink_nvgen,
	no_alt,
 	"multilink",
	"Make interface multilink capable", 
 	PRIV_CONF);
 
ASSERT (cfg_cpp_multilink_ass, cfg_cpp_multilink, no_alt,
	(if_is_ddr(csb->interface->hwptr)));
#endif
 
/***************************************************************
 * [no] cpp callback accept
 */

EOLS      (conf_cpp_callback_eol, cpp_command, CPP_CALLBACK);
KEYWORD(conf_cpp_callback_accept, conf_cpp_callback_eol, no_alt, 
	"accept", "Accept a callback request", PRIV_CONF );
KEYWORD(conf_cpp_callback, conf_cpp_callback_accept, NONE,
	"callback", "Set CPP link callback option", PRIV_CONF );
#if 0
/***************************************************************
 *[no] cpp compression predictor
 *
 */
EOLS(conf_cpp_comp_eol, cpp_ncp_command, CPP_COMPRESSION);
KEYWORD_ID(conf_cpp_comp_pred, conf_cpp_comp_eol, no_alt, OBJ(int,1),
	   cpp_COMP_PREDICTOR, "predictor", "Predictor Compression",
	   PRIV_CONF);

/***************************************************************
 *[no] cpp compression stacker
 *
 */
KEYWORD_ID(conf_cpp_comp_stack, conf_cpp_comp_eol, conf_cpp_comp_pred,
	   OBJ(int,1), cpp_COMP_STACKER, "stacker",
	   "Stacker (LZS) Compression", PRIV_CONF);
NOPREFIX(conf_cpp_comp_noprefix, conf_cpp_comp_stack, conf_cpp_comp_eol);
NVGENS(conf_cpp_comp_nvgens, conf_cpp_comp_noprefix, cpp_ncp_command,
		CPP_COMPRESSION);
KEYWORD(conf_comp_control, conf_cpp_comp_nvgens, conf_cpp_callback,
	"compression", "Enable cpp compression control negotiation",
	PRIV_CONF );
#endif
/***************************************************************
 * [no] cpp authentication
 */
EOLS  (conf_cpp_auth_eol, cpp_command, CPP_AUTH);

NVGENS  (conf_cpp_auth_nvgens, conf_cpp_auth_eol, cpp_command, CPP_AUTH);

KEYWORD(conf_cpp_auth, conf_cpp_auth_nvgens, conf_cpp_callback,
	"authentication", "Set cpp authentication to required", PRIV_CONF );

KEYWORD	(conf_cpp, conf_cpp_auth, NONE, 
	 "cpp", "Combinet Proprietary Protocol", PRIV_CONF);

ASSERT (ci_cpp_test, conf_cpp, NONE, is_isdn(csb->interface->hwptr) ||
	(csb->interface->hwptr->type == IDBTYPE_DIALER));

#undef	ALTERNATE
#define	ALTERNATE	ci_cpp_test
