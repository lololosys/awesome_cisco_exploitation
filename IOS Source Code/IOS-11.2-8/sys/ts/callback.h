/* $Id: callback.h,v 3.2 1995/11/17 18:52:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/callback.h,v $
 *------------------------------------------------------------------
 * Callback function & data structure definitions
 *
 * April 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: callback.h,v $
 * Revision 3.2  1995/11/17  18:52:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/13  16:37:25  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.1  1995/06/07  23:08:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#define CALLBACK_MAX_WAIT_NODSR     30*ONESEC /* Max wait for DSR to go down */
#define CALLBACK_DEFAULT_WAIT_NODSR 5*ONESEC  /* Max time waiting for DSR to go down */
#define CALLBACK_WAIT_DSR 10*ONESEC   /* Max time waiting for DSR to come up */
#define CALLBACK_RETRY_ON_DTR  3    /* Number of times onhook script retried */
#define CALLBACK_WAIT_PROCESS 5*ONESEC /* Max Callback time */ 
#define CALLBACK_DEFAULT_FORCEDWAIT_SEC  4  /* Default forced wait in seconds*/
#define CALLBACK_MAX_FORCEDWAIT_SEC     30  /* Max forced wait in seconds */

#define PRINTF_IF_ALLOWED(message)\
    if ( (stdio == incoming_tty) &&\
         (callback_type == ACF_CALLBACK_EXEC) &&\
         !(stdio->capabilities & MUSTSLIP) &&\
         !(stdio->capabilities & MUSTARAP)){\
	 printf(message);\
         startoutput(stdio);\
         tty_wait_output(stdio, MAX_TTY_OUTPUT_WAIT);\
    }

#define CALLBACK_SUPPORTED_LINES(tty)\
    ((tty->type == TTY_LINE) || (tty->type == AUX_LINE)) 


extern void callback_parser_init(void);

#endif /* _CALLBACK_H_ */
