/* $Id: smrp_parse.h,v 3.2 1995/11/17 18:54:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_parse.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP parser commands.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_parse.h,v $
 * Revision 3.2  1995/11/17  18:54:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:45:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_PARSE_H__
#define __SMRP_PARSE_H__

/*
 *==================================================================
 * SMRP Parse Specific Definitions
 *==================================================================
 */

/*
 *------------------------------------------------------------------
 * SMRPShowOp
 *
 * The SMRPShowOp structure is built when parsing a show command.  
 * It permits limiting SHOW output to specific information without
 * having to write special handling code in each show routine. The
 * routines simply call a function with the current show data and
 * the parsed ShowOp structure and it returns TRUE if the data is
 * desired otherwise the data should be ignored.
 *------------------------------------------------------------------
 */
typedef struct {
    idbtype*		idb;
    SMRPNetRange	range;
    SMRPPortID		port;
    UnicastAddr		node;
    SMRPGroupID 	group;

    tinybool		hasIDB, hasRange, hasPort, hasNode, hasGroup;
} SMRPShowOp;


extern void smrp_parse_Initialize(void);

#endif __SMRP_PARSE_H__
