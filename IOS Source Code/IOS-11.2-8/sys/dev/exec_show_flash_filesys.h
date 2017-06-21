/* $Id: exec_show_flash_filesys.h,v 3.1 1995/11/09 11:18:42 shaker Exp $ 
 *
 *------------------------------------------------------------------
 * Add filesys option to original show flash command.
 *
 * Sept. 15, 1995, Irene Zhu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	show flash filesys for 7000
 */	

/* show flash filesys */
KEYWORD_ID(show_flash_filesys, show_flash_eol, ALTERNATE,
           OBJ(int, 8), TRUE,
           "filesys", "Show file system information", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_flash_filesys
