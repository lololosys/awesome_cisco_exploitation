/* $Id: if_as5200_dummy.c,v 3.1.68.1 1996/06/16 21:14:41 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dummy.c,v $
 *------------------------------------------------------------------
 * if_as5200_dummy.c -- Brasil branch dummy file
 *
 * November, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dummy.c,v $
 * Revision 3.1.68.1  1996/06/16  21:14:41  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/06  19:12:29  jchlin
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "../h/master.h"
#include "../os/old_timer_callbacks.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "linktype.h"
#include "passive_timers.h"
#include "../ui/debug.h"
#include "config.h"
#include "logger.h"
#include "../if/network.h"
#include "../h/ttysrv.h"
#include "../os/async.h"
#include "../h/ctype.h"
#include "../h/globs.h"
#include "../les/isdn_bri.h"
#include "../isdn/gendef.h"
#if defined(XX) | defined(SIERRA)
#   include "if_c4000_mcn.h"
#endif
 
#if defined(BRASIL)
#   include "if_as5200_mcn.h"
#endif
#include "if_c4000_m32.h"

#undef   PUBLIC
#undef   ARGS
#define  PUBLIC      extern
#define  ARGS(x)     x
#include "../isdn/ccie.h"
 
#include "../ui/command.h"
#include "../dev/flash_defines.h"
#include "if_as5200_tdm.h"

#include "if_as5200_modem.h"
#include "if_as5200_dummy.h"
#include "csm_registry.h"

