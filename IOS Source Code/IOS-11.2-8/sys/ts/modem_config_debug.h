/* $Id: modem_config_debug.h,v 3.1 1996/02/23 04:12:04 tkolar Exp $
 * $Source: /release/111/cvs/Xsys/ts/modem_config_debug.h,v $
 *------------------------------------------------------------------
 * Modem Autoconfig debug definitions
 *
 * Febuary 1996, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: modem_config_debug.h,v $
 * Revision 3.1  1996/02/23  04:12:04  tkolar
 * CSCdi49648:  Modem autoconfig break modularity rules
 * Move confmodem_debug out of the commserver subsystem.
 *
 * $Endlog$
 */
#ifndef _MODEMCONF_DEBUG_H_
#define _MODEMCONF_DEBUG_H_

extern void modem_config_debug_init (void);

/*
 * The actual debugging flags are defined in modem_config_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ts/modem_config_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ts/modem_config_debug_flags.h"

#endif /* _MODEMCONF_DEBUG_H_ */
