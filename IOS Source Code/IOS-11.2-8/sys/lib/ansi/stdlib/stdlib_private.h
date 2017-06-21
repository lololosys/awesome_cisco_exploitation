/* $Id: stdlib_private.h,v 3.1.10.1 1996/03/18 20:49:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/stdlib/stdlib_private.h,v $
 *------------------------------------------------------------------
 * stdlib_private.h -- private macros
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdlib_private.h,v $
 * Revision 3.1.10.1  1996/03/18  20:49:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:22:48  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STDLIB_PRIVATE_H__
#define __STDLIB_PRIVATE_H__

#define Isspace(c)      ((c == ' ') || (c == '\t') || (c=='\n') || (c=='\v') \
                         || (c == '\r') || (c == '\f'))
#define Isdigit(c)      ((c <= '9') && (c >= '0'))

#endif /* __STDLIB_PRIVATE_H__ */
