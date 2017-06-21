/* $Id: msg_appn.h,v 3.2.60.1 1996/08/28 12:35:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/appn/msg_appn.h,v $
 *------------------------------------------------------------------
 * Log messages for APPN
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_appn.h,v $
 * Revision 3.2.60.1  1996/08/28  12:35:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  08:37:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:03:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MSG_APPN_H__
#define __MSG_APPN_H__

/*
 * This first chunk of stuff is cloned from logger.h so we don't need
 * to include logger.h into appn. In order to invoke log messages, APPN
 * needs access to two things:
 *
 * 1) the function prototype for errmsg()
 * 2) the macro msgsym
 *
 * These 2 things are the binary api to the IOS logging facility
 */

/*
 * The msgsym() macro is used to create a global symbol for a message. This
 * is used so that the naming convention can be changed without changing
 * any of the references.
 *
 * The msgsym() macro is used both in declaring the message as an external,
 * and in actually calling the message facility. For example:
 *
 *	extern message msgsym(SCARLET, DEAD);
 *			.
 *			.
 *			.
 *	errmsg(&msgsym(SCARLET, DEAD), idb->namestring);
 *
 * Note the use of the & operator in the second use of msgsym(). This is
 * required since errmsg() expects a *pointer* to the message structure.
 */

#ifndef msgsym
#define msgsym(msg, fac) msg_ ## fac ## _ ## msg
#endif

extern void errmsg(const message *, ...);

/*
 * End of stuff cloned from logger.h
 *
 * The rest of this is specific to appn
 */

extern boolean appn_msg_debug;


/* The following defines are for the 1st parameter (i.e., *msg) of
 *    errmsg(message *msg, ...);
 */
#define MSG_APPNEMERG	(&msgsym(APPNEMERG, APPN))
#define MSG_APPNALERT	(&msgsym(APPNALERT, APPN))
#define MSG_APPNCRIT	(&msgsym(APPNCRIT, APPN))
#define MSG_APPNERR	(&msgsym(APPNERR, APPN))
#define MSG_APPNWARNING	(&msgsym(APPNWARNING, APPN))
#define MSG_APPNNOTICE	(&msgsym(APPNNOTICE, APPN))
#define MSG_APPNINFO	(&msgsym(APPNINFO, APPN))
#define MSG_APPNDEBUG	(&msgsym(APPNDEBUG, APPN))

#define MSG_APPNLOGMSG     (&msgsym(APPNLOGMSG, APPN))
#define MSG_APPNSENDEVENT  (&msgsym(APPNSENDEVENT, APPN))
#define MSG_APPNSENDMSG    (&msgsym(APPNSENDMSG, APPN))
#define MSG_APPNSENDREQ    (&msgsym(APPNSENDREQ, APPN))
#define MSG_APPNETERROR    (&msgsym(APPNETERROR, APPN))
#define MSG_APPNETLOG      (&msgsym(APPNETLOG, APPN))

extern const message msgsym(APPNEMERG, APPN);
extern const message msgsym(APPNALERT, APPN);
extern const message msgsym(APPNCRIT, APPN);
extern const message msgsym(APPNERR, APPN);
extern const message msgsym(APPNWARNING, APPN);
extern const message msgsym(APPNNOTICE, APPN);
extern const message msgsym(APPNINFO, APPN);
extern const message msgsym(APPNDEBUG, APPN);

extern const message msgsym(APPNLOGMSG, APPN);
extern const message msgsym(APPNSENDEVENT, APPN);
extern const message msgsym(APPNSENDMSG, APPN);
extern const message msgsym(APPNSENDREQ, APPN);
extern const message msgsym(APPNETERROR, APPN);
extern const message msgsym(APPNETLOG, APPN);

#endif /* __MSG_APPN_H__ **/
