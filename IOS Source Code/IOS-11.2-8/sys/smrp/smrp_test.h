/* $Id: smrp_test.h,v 3.2 1995/11/17 18:54:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/smrp_test.h,v $
 *------------------------------------------------------------------
 * Support routines for SMRP test commands.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_test.h,v $
 * Revision 3.2  1995/11/17  18:54:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/01  00:23:13  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.2  1995/06/28  09:30:30  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  22:45:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __SMRP_TEST_H__
#define __SMRP_TEST_H__

/*
 *------------------------------------------------------------------
 * SMRP Test data structure
 *
 *------------------------------------------------------------------
 */
#define SMRP_DATAPRI_OFFSET     1
#define SMRP_DATAFILTER_OFFSET  1
#define SMRP_DATAHDR_SIZE       (SMRP_HEADER_SIZE + SMRP_DATAPRI_OFFSET + \
                                 SMRP_DATAFILTER_OFFSET)

typedef struct SMRPTestGroup_ {
    struct SMRPTestGroup_*	Next;
    SMRPPortID                  TestPortID;
    SMRPGroupID		        TestGroupAddress;
} SMRPTestGroup;

SMRPTestGroup*                  TestGroupTable;

/*
 *------------------------------------------------------------------
 * SMRP Internal Entry Points defined by smrp_test.
 *------------------------------------------------------------------
 */

void smrp_test_init(subsystype *subsys);

#endif __SMRP_TEST_H__
