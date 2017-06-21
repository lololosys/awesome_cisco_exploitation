/* $Id: smrp_show.h,v 3.2 1995/11/17 18:54:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_show.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP show commands.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_show.h,v $
 * Revision 3.2  1995/11/17  18:54:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:45:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_SHOW_H__
#define __SMRP_SHOW_H__

#include "../util/tree.h"


/*
 *==================================================================
 * SMRP Internal Entry Points defined by smrp_route.
 *==================================================================
 */
void smrp_show_commands (parseinfo*);
extern void smrp_ShowProto(idbtype *);

#endif __SMRP_SHOW_H__
