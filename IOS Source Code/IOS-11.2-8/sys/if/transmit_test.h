/* $Id: transmit_test.h,v 3.2.58.1 1996/03/18 20:14:56 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/transmit_test.h,v $
 *------------------------------------------------------------------
 * transmit_test.h -- Packet generator for testing purposes
 *
 * December 1991 - David R. Hampton
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: transmit_test.h,v $
 * Revision 3.2.58.1  1996/03/18  20:14:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.1  1996/03/07  09:44:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  09:31:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:52:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:55:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define	BUFLEN	80

#define READ_OR_EXIT 		\
    if (!rdtty(buf, BUFLEN)) {	\
	if (!pak)		\
	    retbuffer(pak);	\
	*pakp = NULL;		\
	return;			\
    }				\
    args = deblank(buf);	\
    c = toupper(args[0]);	\
    if (c == 'Q') {		\
	if (!pak)		\
	    retbuffer(pak);	\
	*pakp = NULL;		\
	return;			\
    }

#define READ_OR_EXIT2 		\
    if (!rdtty(buf, BUFLEN)) {	\
	if (!pak)		\
	    retbuffer(pak);	\
	return;			\
    }				\
    args = deblank(buf);	\
    c = toupper(args[0]);	\
    if (c == 'Q') {		\
	if (!pak)		\
	    retbuffer(pak);	\
	return;			\
    }

#define READ_OR_EXIT3 		\
    if (!rdtty(buf, BUFLEN)) {	\
	return(0);		\
    }				\
    args = deblank(buf);	\
    c = toupper(args[0]);	\
    if (c == 'Q') {		\
	return(0);		\
    }

#define READ_POP 		\
    if (!rdtty(buf, BUFLEN)) {	\
	if (!pak)		\
	    retbuffer(pak);	\
	return;			\
    }				\
    args = deblank(buf);	\
    c = toupper(args[0]);	\
    if (c == 'Q')		\
	break;
