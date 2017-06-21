/* $Id: exec_pas_show_diag.h,v 3.1.2.2 1996/08/12 16:06:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/exec_pas_show_diag.h,v $
 *------------------------------------------------------------------
 * exec_pas_show_diag.h - The "show diag" command for port adapters.
 *
 * April 1996, Suresh Sangiah
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_pas_show_diag.h,v $
 * Revision 3.1.2.2  1996/08/12  16:06:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/29  17:57:12  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/29  17:09:21  ssangiah
 * Placeholder file for new platform related code.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show diag [slot]
 * OBJ(int,1) = [slot], if a slot is specified
 * OBJ(int,1) = -1, if no slot is specified
 */
EOLNS   (pas_show_diag_eol, pas_show_diag);
 
SET    (pas_show_diag_noslot, pas_show_diag_eol, OBJ(int,1), MAXUINT);
 
NUMBER (pas_show_diag_slot, pas_show_diag_eol, pas_show_diag_noslot,
        OBJ(int,1), 0, 6, "Slot number");
 
KEYWORD(pas_show_diag, pas_show_diag_slot, ALTERNATE,
        "diag", "Show diagnostic information for port adapters/modules",
        PRIV_USER | PRIV_USER_HIDDEN);
 
#undef  ALTERNATE
#define ALTERNATE       pas_show_diag

