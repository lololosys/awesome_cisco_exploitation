/* $Id: chat_debug_flags.h,v 3.1.62.1 1996/05/17 12:15:16 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ts/chat_debug_flags.h,v $
 *------------------------------------------------------------------
 * Chat scripts debugging flags defined here
 *
 * May'94, Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *
 *------------------------------------------------------------------
 *
 */

/*
 * We get included (multiple times) from chat code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(chat_debug_arr)

DEBUG_FLAG(chat_debug,DEBUG_CHAT,"Chat scripts activity")

DEBUG_ARRDONE
