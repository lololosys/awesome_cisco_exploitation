/* $Id: cfg_int_fr_svc.h,v 3.1.2.2 1996/07/04 17:05:37 chifei Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_fr_svc.h,v $
 *------------------------------------------------------------------
 * Frame Relay SVC layer 3 interface configuration commands
 *
 * July 1996, Chifei Cheng
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_fr_svc.h,v $
 * Revision 3.1.2.2  1996/07/04  17:05:37  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Moved command handling out of frame_relay_command()
 *
 * Revision 3.1.2.1  1996/07/03  01:57:54  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1  1996/07/03  00:51:17  chifei
 * a placeholder for modularizing FR SVC commands.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/**************************************************************
 * [no] frame-relay svc
 *	- enable Frame Relay SVC functionality for the interface,
 *	- no form disables it.
 */
EOLS  (ci_fr_svc_enable_eol, fr_svc_enable_cmd, FRAME_RELAY_SVC_ENABLE);
NOPREFIX (ci_fr_svc_enable_noprefix, ci_fr_svc_enable_eol, ci_fr_svc_enable_eol);
KEYWORD_ID (ci_fr_svc_b2b, ci_fr_svc_enable_noprefix, ci_fr_svc_enable_noprefix,
		OBJ(int,1), FRAME_RELAY_SVC_B2B_ENABLE, "back-to-back", 
		"Run back-back SVCs, LAB USE ONLY", PRIV_CONF);
NVGENS (ci_fr_svc_nvgen, ci_fr_svc_b2b, fr_svc_enable_cmd,
        FRAME_RELAY_SVC_ENABLE);
KEYWORD (ci_FR_svc, ci_fr_svc_nvgen, NONE, "svc",
        "Enable frame relay SVCs", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	ci_FR_svc
