/* $Id: parser_defs_tdm.h,v 3.1.40.1 1996/06/16 21:16:46 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/parser_defs_tdm.h,v $
 *------------------------------------------------------------------
 * parser_defs_tdm.h -- Parser definitions for tdm commands.
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_tdm.h,v $
 * Revision 3.1.40.1  1996/06/16  21:16:46  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1.2.2  1996/05/09  22:33:45  jturner
 * CSCdi57181:  tdm code review
 * Branch: Brasil_branch
 *
 * Revision 3.1.2.1  1996/02/06  17:20:02  jturner
 * Branch: Brasil_branch
 * First commit of tdm parser commands.
 * Cleaned up some switching details.
 *
 * Revision 3.1  1996/02/02  22:50:19  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


enum {
    SHOW_TDM_ALL,
    SHOW_TDM_DATA,
    SHOW_TDM_CONNECTIONS,
    SHOW_TDM_MOTHERBOARD,
    SHOW_TDM_SLOT
};

