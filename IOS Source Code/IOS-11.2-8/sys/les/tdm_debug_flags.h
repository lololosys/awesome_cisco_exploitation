/* $Id: tdm_debug_flags.h,v 3.1.40.1 1996/06/16 21:16:59 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/tdm_debug_flags.h,v $
 *------------------------------------------------------------------
 * tdm_debug_flags.h
 *
 * February 1996, Joe Turner
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tdm_debug_flags.h,v $
 * Revision 3.1.40.1  1996/06/16  21:16:59  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/02/02  22:50:16  jturner
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) to define debugging flags.
 * For this, we use some macros that get defined by debug_macros.h
 * What definitions the macros get depends on which magic flags
 * have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(tdm_debug_arr)

DEBUG_FLAG(tdm_debug,DEBUG_TDM,"TDM connection")

DEBUG_ARRDONE
