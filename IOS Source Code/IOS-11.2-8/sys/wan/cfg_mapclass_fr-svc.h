/* $Id: cfg_mapclass_fr-svc.h,v 3.1.2.9 1996/07/22 10:23:55 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_mapclass_fr-svc.h,v $
 *------------------------------------------------------------------
 * Frame Relay map class configuration commands
 *
 * February 1995, Shankar Natarajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mapclass_fr-svc.h,v $
 * Revision 3.1.2.9  1996/07/22  10:23:55  fowler
 * CSCdi63651:  FR BE values dont match customer needs
 *              Expand the range of accepted values at the lower end
 * Branch: California_branch
 *
 * Revision 3.1.2.8  1996/07/03  23:03:36  fowler
 * CSCdi62066:  Some frame relay parameters need lower minimums
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/03  22:02:56  fowler
 * CSCdi62066:  Some frame relay parameters need lower minimums
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/27  21:38:13  skoh
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/22  19:27:34  shankar
 * CSCdi61102:  Parameter Negotiation for SVCs needs a little more checking
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/11  23:42:36  skoh
 * CSCdi59477:  Do not allow the customer to configure max fmif
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/22  20:13:29  skoh
 * CSCdi58382:  Correction needed to the range of bc and be
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/20  05:37:52  skoh
 * CSCdi56032:  New SVC commands in 11.2 (partial)
 * Branch: California_branch
 * Syntax change in fr svc map-class subcommands.
 *
 * Revision 3.1.2.1  1996/05/16  04:48:16  fowler
 * CSCdi57841:  Frame relay VCQ+CIR merge dropped some map-class commands
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/02  22:09:46  ccpoe
 * cfg_mapclass_fr-svc.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] frame-relay fmif {in|out} <size, byte>
 */
EOLS	(cmc_fr_fmif_eol, fr_mapclass_command, FR_MAP_CLASS_FMIF);
PARAMS_KEYONLY(cmc_fr_out_fmif, cmc_fr_fmif_eol, "out", OBJ(int,1),
	       1, 8096, fr_mapclass_command, FR_MAP_CLASS_OUT_FMIF,
	       "Outgoing Frame Size", "Bytes", PRIV_CONF | PRIV_HIDDEN);
PARAMS_KEYONLY(cmc_fr_in_fmif, cmc_fr_out_fmif, "in", OBJ(int,1),
	       1, 8096, fr_mapclass_command, FR_MAP_CLASS_IN_FMIF,
	       "Incoming Frame Size", "Bytes", PRIV_CONF | PRIV_HIDDEN);
NUMBER  (cmc_fr_fmif_num, cmc_fr_fmif_eol, cmc_fr_in_fmif,
	 OBJ(int,1), 1, 8096, "Applied to both Incoming/Outgoing Frame size, Bytes");
KEYWORD	(cmc_fr_fmif, cmc_fr_fmif_num, no_alt, "fmif", 
	 "Max. Frame Size, Default = 1500 bytes", PRIV_CONF | PRIV_HIDDEN);
/***************************************************************
 * [no] frame-relay holdq <number>
 */
PARAMS_KEYONLY(cmc_fr_holdq, cmc_fr_fmif, "holdq", OBJ(int,1),
	       1, 40, fr_mapclass_command, FR_MAP_CLASS_HOLDQ,
	       "Hold queue size for SVC, Default = 40", 
	       "Size", PRIV_CONF | PRIV_HIDDEN);
/***************************************************************
 * [no] frame-relay idle-timer <number, second>
 */
PARAMS_KEYONLY(cmc_fr_idle_timer, cmc_fr_holdq, "idle-timer", OBJ(int,1),
	       0, 2000000, fr_mapclass_command, FR_MAP_CLASS_IDLE_TIMER,
	       "Idle timeout for a SVC, Default = 120 sec", 
	       "Seconds", PRIV_CONF);

/***************************************************************
 * [no] frame-relay mincir {in|out} <rate, bps>
 */
EOLS	(cmc_fr_min_cir_eol, fr_mapclass_command, FR_MAP_CLASS_MIN_CIR);

NUMBER  (cmc_fr_min_cir_num, cmc_fr_min_cir_eol, no_alt,
	 OBJ(int,1), 2000, 45000000,
	 "Applied to both Incoming/Outgoing CIR, Bits per second");

PARAMS_KEYONLY(cmc_fr_out_mincir, cmc_fr_min_cir_num, "out", OBJ(int,1),
	       2000, 45000000, fr_mapclass_command, FR_MAP_CLASS_MIN_OUT_CIR,
	       "Outgoing minimum acceptable CIR", 
	       "Bits per second", PRIV_CONF);

PARAMS_KEYONLY(cmc_fr_min_incir, cmc_fr_out_mincir, "in", OBJ(int,1),
	       2000, 45000000, fr_mapclass_command, FR_MAP_CLASS_MIN_IN_CIR,
	       "Incoming minimum acceptable CIR",
	       "Bits per second", PRIV_CONF);

KEYWORD	(cmc_fr_min_cir, cmc_fr_min_incir, cmc_fr_idle_timer, "mincir",
	"Minimum acceptable CIR, Default = 56000 bps",
	 PRIV_CONF);
/***************************************************************/


#undef  ALTERNATE
#define ALTERNATE       cmc_fr_min_cir
