/* $Id: exec_show_flash.h,v 3.2 1995/11/17 09:00:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/exec_show_flash.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ F L A S H . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_flash.h,v $
 * Revision 3.2  1995/11/17  09:00:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/27  21:53:05  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.1  1995/07/04  01:52:39  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:23:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __EXEC_SHOW_FLASH_H__
#define __EXEC_SHOW_FLASH_H__

/******************************************************************
 * "show flash [detailed | chips | all | err]"  : For main system flash
 * "show flash partition <no> [detailed | chips | all |err]" and "show flash
 * "summary": enabled only if partitioning is supported on the system.
 * "show bootflash [detailed | chips | all | err]" : For bootstrap flash
 * "show bootflash partition <no> [detailed | chips | all |err]" and "show
 * "bootflash summary": enabled only if partitioning is supported on the system.
 *
 */

/*---------------------** show flash commands **------------------------*/
EOLS    (show_flash_eol, show_flash, SHOW_FLASH);

/* show flash chips */
KEYWORD_ID(show_flash_chips, show_flash_eol, show_flash_eol,
           OBJ(int,5), TRUE,
           "chips", "show flash chip information",
           PRIV_USER);

/* extend "show flash filesys" from here */
LINK_TRANS(show_flash_filesys_extend_here, show_flash_chips);

/* show flash detailed */
KEYWORD_ID(show_flash_det, show_flash_eol, show_flash_filesys_extend_here,
           OBJ(int,4), TRUE,
           "detailed", "show flash detailed directory",
           PRIV_USER);
/* show flash err */
KEYWORD_ID(show_flash_err, show_flash_eol, show_flash_det,
           OBJ(int,3), TRUE,
           "err", "show flash chip erase and write retries",
           PRIV_USER);
/* show flash all */
KEYWORD_ID(show_flash_all, show_flash_eol, show_flash_err,
           OBJ(int,2), TRUE,
           "all", "show all possible flash info", PRIV_USER);
/* show flash summary */
KEYWORD_ID(show_flash_summary, show_flash_eol, show_flash_all,
           OBJ(int,6), TRUE,
           "summary", "show flash partition summary",
           PRIV_USER);
/* show flash partition */
NUMBER  (show_flash_partition_num, show_flash_all, NONE,
         OBJ(int,7), 1, FLASH_MAX_PART_CNT, "flash partition number" );
KEYWORD_ID(show_flash_partition, show_flash_partition_num, show_flash_summary,
           OBJ(int,1), TRUE,
           "partition", "show flash partition information",
           PRIV_USER);
IFELSE  (show_flash, show_flash_partition, show_flash_all,
         flash_has_multiple_partitions(SF));
KEYWORD (show_flash_kwd, show_flash, ALTERNATE,
         SF, "System Flash information", PRIV_USER|PRIV_USER_HIDDEN);
/*---------------------** show flash commands **------------------------*/

/*--------------------- show bootflash commands ------------------------*/
EOLS    (show_bflash_eol, show_flash, SHOW_BOOTFLASH);
/* show bootflash chips */
KEYWORD_ID(show_bflash_chips, show_bflash_eol, show_bflash_eol,
           OBJ(int,5), TRUE,
           "chips", "show boot flash chip information",
           PRIV_USER);
/* show bootflash detailed */
KEYWORD_ID(show_bflash_det, show_bflash_eol, show_bflash_chips,
           OBJ(int,4), TRUE,
           "detailed", "show boot flash detailed directory",
           PRIV_USER);
/* show bootflash err */
KEYWORD_ID(show_bflash_err, show_bflash_eol, show_bflash_det,
           OBJ(int,3), TRUE,
           "err", "show boot flash chip erase and write retries",
           PRIV_USER);
/* show bootflash all */
KEYWORD_ID(show_bflash_all, show_bflash_eol, show_bflash_err,
           OBJ(int,2), TRUE,
           "all", "show all possible boot flash info", PRIV_USER);
/* show bootflash summary */
KEYWORD_ID(show_bflash_summary, show_bflash_eol, show_bflash_all,
           OBJ(int,6), TRUE,
           "summary", "show boot flash partition summary",
           PRIV_USER);
/* show bootflash partition */
NUMBER        (show_bflash_partition_num, show_bflash_all, NONE,
       OBJ(int,7), 1, FLASH_MAX_PART_CNT, "boot flash partition number" );
KEYWORD_ID(show_bflash_partition, show_bflash_partition_num,
           show_bflash_summary,
           OBJ(int,1), TRUE,
           "partition", "show boot flash partition information",
           PRIV_USER);
IFELSE  (show_bflash, show_bflash_partition, show_bflash_all,
         flash_has_multiple_partitions(BF));

KEYWORD (show_bflash_kwd, show_bflash_all, show_flash_kwd,
         BF, "Boot Flash information", PRIV_USER);
/*-------------------** show bootflash commands **----------------------*/

IFELSE  (ifelse_show_flash, show_bflash_kwd, show_flash_kwd,
         flash_alias_match(BF));

#undef	ALTERNATE
#define	ALTERNATE	ifelse_show_flash

#endif /* __EXEC_SHOW_FLASH_H__ */
