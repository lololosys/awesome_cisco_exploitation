/* $Id: cfg_int_fr-class.h,v 3.1.2.1 1996/05/15 05:54:33 fowler Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_fr-class.h,v $
 *------------------------------------------------------------------
 * Frame Relay Class Command
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_fr-class.h,v $
 * Revision 3.1.2.1  1996/05/15  05:54:33  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  16:49:24  ccpoe
 * frame relay traffic shaping
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * frame-relay class class-name 
 * no frame-relay class class-name
 */
EOLS	(ci_class_eol, fr_class_command, FR_DLCI_CLASS);

STRING	(ci_class_name, ci_class_eol, no_alt,
	 OBJ(string, 1), "map class name");

NVGENS	(ci_class_nvgen, ci_class_name,
	 fr_class_command, FR_DLCI_CLASS);

KEYWORD_ID(ci_FR_class, ci_class_nvgen, ALTERNATE,
	   leave_subcommand, TRUE,
	    "class", "Assign a mapclass to a dlci",
	 PRIV_CONF|PRIV_SUBIF);
    
#undef ALTERNATE
#define ALTERNATE	ci_FR_class

