/* $Id: keyman.h,v 3.3.44.1 1996/04/19 14:56:12 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/keyman.h,v $
 *------------------------------------------------------------------
 * Key management
 *
 * September 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: keyman.h,v $
 * Revision 3.3.44.1  1996/04/19  14:56:12  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1996/01/23  04:59:12  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.3  1996/01/03  03:31:57  ahh
 * CSCdi45528:  Move key manager out of core
 *
 * Revision 3.2  1995/11/17  08:57:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:14:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1.2.1  1995/10/12  20:54:35  gchristy
 * Branch: Arkansas_branch
 * Commit RRI2 tree to Arkansas_branch
 *
 * Revision 2.1  1995/10/11  19:35:29  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __KEYMAN_H__
#define __KEYMAN_H__

/*
 * Lifetime types
 */

enum key_lifetime_type {
    KEY_ACCEPT_LIFETIME,
    KEY_SEND_LIFETIME
};

#endif /* __KEYMAN_H__ */
