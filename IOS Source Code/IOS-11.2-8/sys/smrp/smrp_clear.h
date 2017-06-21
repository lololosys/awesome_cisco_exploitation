/* $Id: smrp_clear.h,v 3.2 1995/11/17 18:53:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_clear.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP clear commands.
 *
 * April 1995, Kent Leung
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_clear.h,v $
 * Revision 3.2  1995/11/17  18:53:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:45:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_CLEAR_H__
#define __SMRP_CLEAR_H__



/*
 *==================================================================
 * SMRP Internal Entry Points defined by smrp_route.
 *==================================================================
 */
extern void smrp_ClearNeighbor(parseinfo *);
extern void smrp_ClearTraffic(void);
extern void smrp_clear_commands(parseinfo *);


#endif __SMRP_CLEAR_H__
