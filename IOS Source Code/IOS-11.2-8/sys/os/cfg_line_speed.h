/* $Id: cfg_line_speed.h,v 3.2 1995/11/17 18:45:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_speed.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ S P E E D . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_speed.h,v $
 * Revision 3.2  1995/11/17  18:45:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * speed <num>
 * no speed
 *
 * rxspeed <num>
 * no rxspeed
 *
 * txspeed <num>
 * no txspeed
 */
PARAMS_KEYONLY(line_txspeed, ALTERNATE, "txspeed",
	       OBJ(int,1), 0, -1,
	       txspeed_command, PARSER_LINE_TXSPEED,
	       "Set the transmit speeds",
	       "Transmit speed", PRIV_USER);
PARAMS_KEYONLY(line_rxspeed, line_txspeed, "rxspeed",
	       OBJ(int,1), 0, -1,
	       rxspeed_command, PARSER_LINE_RXSPEED,
	       "Set the receive speed",
	       "Receive speed", PRIV_USER);
PARAMS_KEYONLY(line_speed, line_rxspeed, "speed",
	       OBJ(int,1), 0, -1,
	       speed_command, PARSER_LINE_SPEED,
	       "Set the transmit and receive speeds",
	       "Transmit and receive speeds", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_speed
