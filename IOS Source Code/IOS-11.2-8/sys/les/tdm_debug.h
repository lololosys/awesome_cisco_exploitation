/* $Id: tdm_debug.h,v 3.1.40.1 1996/06/16 21:16:57 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/tdm_debug.h,v $
 *------------------------------------------------------------------
 * tdm_debug.h -- Header file for TDM debugging routines.
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tdm_debug.h,v $
 * Revision 3.1.40.1  1996/06/16  21:16:57  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:15  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prevent multiple inclusion.
 */
#ifndef __TDM_DEBUG_H__
#define __TDM_DEBUG_H__

/* Function to fire up tdm debugging */
extern void tdm_debug_init(void);
extern void tdm_parser_init(void);

/*
 * The actual debugging flags are defined in tdm_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../les/tdm_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../les/tdm_debug_flags.h"

#endif
