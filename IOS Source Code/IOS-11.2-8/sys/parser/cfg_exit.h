/* $Id: cfg_exit.h,v 3.2 1995/11/17 18:42:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_exit.h,v $
 *------------------------------------------------------------------
 * C F G _ E X I T . H
 *
 * exit	- exit configuration mode 
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_exit.h,v $
 * Revision 3.2  1995/11/17  18:42:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:02:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exit
 *
 * exits from config mode
 */

EOLS	(act_exit_configure, exit_config_command, CMD_EXIT);

KEYWORD	(exit_configure, act_exit_configure, ALTERNATE, 
	 "exit", "Exit from configure mode", PRIV_MIN);

#undef	ALTERNATE
#define	ALTERNATE	exit_configure
