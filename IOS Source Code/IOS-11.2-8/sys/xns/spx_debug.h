/* $Id: spx_debug.h,v 3.2 1995/11/17 19:24:49 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/spx_debug.h,v $
 *------------------------------------------------------------------
 * spx_debug.h   Sequenced Packet Exchange Protocol  (debug header
 *               include file)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx_debug.h,v $
 * Revision 3.2  1995/11/17  19:24:49  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:40:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:16  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void spx_debug_command(parseinfo *);
extern void spx_debug_all(boolean);
extern void spx_debug_show(void);
extern void spx_debug_init(void);

extern boolean spx_debug;

/*
 * The actual debugging flags are defined in spx_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../xns/spx_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../xns/spx_debug_flags.h"
