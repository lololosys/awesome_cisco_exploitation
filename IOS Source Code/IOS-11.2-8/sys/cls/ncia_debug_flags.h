/* $Id: ncia_debug_flags.h,v 3.1.2.1 1996/05/17 10:43:37 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_debug_flags.h,v $
 *------------------------------------------------------------------
 * ncia_debug_flags.h - Debugging flag definition for NCIA
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_debug_flags.h,v $
 * Revision 3.1.2.1  1996/05/17  10:43:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.1  1996/04/05  06:05:46  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:18:43  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from NCIA code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ncia_debug_arr)
DEBUG_FLAG(ncia_server_debug,NCIA_DEBUG_SERVER,"NCIA Debug Server")
DEBUG_FLAG(ncia_client_debug,NCIA_DEBUG_CLIENT,"NCIA Debug Client")
DEBUG_FLAG(ncia_circuit_debug,NCIA_DEBUG_CIRCUIT,"NCIA Debug Circuit")
DEBUG_ARRDONE

#define NCIA_DEBUG_ALL (NCIA_DEBUG_CIRCUIT+1)
