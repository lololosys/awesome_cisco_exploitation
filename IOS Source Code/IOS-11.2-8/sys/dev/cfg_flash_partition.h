/* $Id: cfg_flash_partition.h,v 3.2.60.1 1996/08/12 16:01:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dev/cfg_flash_partition.h,v $
 *------------------------------------------------------------------
 * cfg_flash_partition.h : Flash partitioning command
 *
 * Oct 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_flash_partition.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:59:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:52:36  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:16:28  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:03:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * partition <dev> <number> <size1> <size2> ... <size8>
 *
 * There should be only as many size parameters as the number of partitoons
 * specified.
 * It is also acceptable to have no size parameters, and just the parameter
 * indicating the number of partitions. In this case, an attempt will be
 * made to partition the flash into equal sized partitions.
 * If the config command has the 'no' prefix, all parameters entered after
 * the "flash" keyword will be ignored.
 */

EOLNS    (partition_flash_eol, partition_flash_command);

NUMBER  (partition_flash_size8, partition_flash_eol, partition_flash_eol,
         OBJ(int, 9), 1, 64, "Size of partition 8");

NUMBER  (partition_flash_size7, partition_flash_size8, partition_flash_eol,
         OBJ(int, 8), 1, 64, "Size of partition 7");

NUMBER  (partition_flash_size6, partition_flash_size7, partition_flash_eol,
         OBJ(int, 7), 1, 64, "Size of partition 6");

NUMBER  (partition_flash_size5, partition_flash_size6, partition_flash_eol,
         OBJ(int, 6), 1, 64, "Size of partition 5");

NUMBER  (partition_flash_size4, partition_flash_size5, partition_flash_eol,
         OBJ(int, 5), 1, 64, "Size of partition 4");

NUMBER  (partition_flash_size3, partition_flash_size4, partition_flash_eol,
         OBJ(int, 4), 1, 64, "Size of partition 3");

NUMBER  (partition_flash_size2, partition_flash_size3, partition_flash_eol,
         OBJ(int, 3), 1, 64, "Size of partition 2");

NUMBER  (partition_flash_size1, partition_flash_size2, partition_flash_eol,
         OBJ(int, 2), 1, 64, "Size of partition 1");

NUMBER  (partition_flash_num, partition_flash_size1, no_alt,
         OBJ(int, 1), 1, 8, "Number of partitions in device");

NOPREFIX (partition_flash_noprefix, partition_flash_num, partition_flash_eol);

NVGENNS  (partition_flash_nvgen, partition_flash_noprefix,
         partition_flash_command);

KEYWORD (partition_flash, partition_flash_nvgen, no_alt, "flash",
         "Partition system flash", PRIV_CONF);

KEYWORD (sys_partition, partition_flash, NONE,
	 "partition", "Partition device", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       sys_partition
