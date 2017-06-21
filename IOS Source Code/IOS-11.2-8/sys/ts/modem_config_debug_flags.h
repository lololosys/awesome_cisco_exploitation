/* $Id: modem_config_debug_flags.h,v 3.1 1996/02/23 04:12:06 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/ts/modem_config_debug_flags.h,v $
 *------------------------------------------------------------------
 * Modem config debug flags
 *
 * Feburary 1996, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_config_debug_flags.h,v $
 * Revision 3.1  1996/02/23  04:12:06  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * $Endlog$
 */
/*
 * We get included (multiple times) from autoconfig code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(modem_config_debug_arr)

DEBUG_FLAG(confmodem_debug,DEBUG_MODEMCAP,"Modem Configuration Database")

DEBUG_ARRDONE
