/* $Id: flash_partition.h,v 3.2 1995/11/17 09:01:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/flash_partition.h,v $
 *------------------------------------------------------------------
 * flash_partition.h - Flash partitioning code.
 *
 * April 1995, Suresh Sangiah
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_partition.h,v $
 * Revision 3.2  1995/11/17  09:01:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Default number of flash partitions */
#define DEFAULT_PART_CNT 1

extern int decide_partitions(dev_info_t *dev, fs_bounds_t *fsbounds,
                                                             int *part_bounds);
extern void partition_flash_command(parseinfo *csb);
