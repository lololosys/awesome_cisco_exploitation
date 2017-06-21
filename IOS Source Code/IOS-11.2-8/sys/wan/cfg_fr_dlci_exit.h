/* $Id: cfg_fr_dlci_exit.h,v 3.1.2.2 1996/08/12 16:09:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_fr_dlci_exit.h,v $
 *------------------------------------------------------------------
 * Frame Relay Interface-dlci mode exit
 *
 * March 1996, Charles Poe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_fr_dlci_exit.h,v $
 * Revision 3.1.2.2  1996/08/12  16:09:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/16  04:48:16  fowler
 * CSCdi57841:  Frame relay VCQ+CIR merge dropped some map-class commands
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/26  22:36:08  ccpoe
 * placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exit - exit from frame-relay interface-dlci configuration command
 * 
 */

EOLNS	(ci_FR_dlci_exit_eol, fr_dlci_exit_command);


KEYWORD	(ci_FR_dlci_exit, ci_FR_dlci_exit_eol, ci_FR_dlci_exit_eol,
         "exit", "Exit from FR dlci configuration mode", 
	 PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       ci_FR_dlci_exit
