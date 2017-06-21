/* $Id: crypto_registry.h,v 3.1.64.1 1996/04/19 14:55:59 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_registry.h,v $
 *------------------------------------------------------------------
 * crypto_registry.h
 *
 * crypto subsystem registry file. 
 *
 * 12, 1995, Xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_registry.h,v $
 * Revision 3.1.64.1  1996/04/19  14:55:59  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.6  1996/01/09  01:01:09  xliu
 * Branch: IosSec_branch
 * Changed crypto registry to be the well known registry.
 *
 * Revision 3.1.2.5  1996/01/05  23:25:28  xliu
 * Branch: IosSec_branch
 * Add crypto_registry.regc to include.
 *
 * Revision 3.1.2.4  1996/01/04  18:47:50  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 3.1.2.3  1995/12/12  23:09:37  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 3.1.2.2  1995/12/12  03:34:44  xliu
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/11  21:34:43  xliu
 * Branch: IosSec_branch
 * Add crypto registry files.
 *
 * Revision 3.1  1995/12/11  20:57:06  xliu
 * Placeholder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * Change some registration terminology and symbol names.
 *
 * Revision 1.1  1993/05/14  00:25:54  lougheed
 * More stub trimming.  Create ip_init.c and ip_registry.h.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__CRYPTO_REGISTRY_H__
#define	__CRYPTO_REGISTRY_H__

#define CRYPTO_MAX_SWITCH_MODE 4

#include "address.h"
#include "file.h"

#include "../h/registry.h"
#include "../crypto/crypto_registry.regh"

#endif	/* __CRYPTO_REGISTRY_H__ */

