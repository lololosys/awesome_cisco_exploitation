/* $Id: exec_show_tdm.h,v 3.1.40.2 1996/08/12 16:04:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/les/exec_show_tdm.h,v $
 *------------------------------------------------------------------
 * exec_show_tdm.h - TDM show routines.
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_tdm.h,v $
 * Revision 3.1.40.2  1996/08/12  16:04:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/06/16  21:14:25  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:11  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show tdm [<connections>|<data>] [<slot and num>|<motherboard>]
 *
 * OBJ(int,1) = <connections> | <data>
 * OBJ(int,2) = <motherboard> | <slot> | all
 * OBJ(int,3) = <slot-num>
 */
EOLNS    (show_tdm_eol, show_tdm_command);
KEYWORD_ID(show_tdm_motherboard, show_tdm_eol, show_tdm_eol,
           OBJ(int,2), SHOW_TDM_MOTHERBOARD,
           "motherboard", "show motherboard TDM connection information",
           PRIV_USER);
NUMBER (show_tdm_slot_number, show_tdm_eol,
        no_alt, OBJ(int,3), 0, 2, "Slot number");
KEYWORD_ID(show_tdm_slot, show_tdm_slot_number, show_tdm_motherboard,
           OBJ(int,2), SHOW_TDM_SLOT,
           "slot", "show slot specific TDM connection information",
           PRIV_USER);
KEYWORD_ID (show_tdm_data, show_tdm_slot, no_alt,
           OBJ(int,1), SHOW_TDM_DATA,
           "data", "TDM data information", PRIV_USER);
KEYWORD_ID (show_tdm_connections, show_tdm_slot, show_tdm_data,
           OBJ(int,1), SHOW_TDM_CONNECTIONS,
           "connections", "TDM connection information", PRIV_USER);
KEYWORD    (show_tdm_kwd, show_tdm_connections, no_alt,
           "tdm", "TDM connection information", PRIV_USER);
 
#undef  ALTERNATE
#define ALTERNATE       show_tdm_kwd
