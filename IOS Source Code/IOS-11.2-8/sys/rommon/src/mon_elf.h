/* $Id: mon_elf.h,v 3.2 1995/11/17 18:42:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/mon_elf.h,v $
 *------------------------------------------------------------------
 * mon_elf.h
 *
 * January 1994, Rob CLevenger
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * ELF file defines for the Monitor.
 *------------------------------------------------------------------
 * $Log: mon_elf.h,v $
 * Revision 3.2  1995/11/17  18:42:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MON_ELF_H__
#define __MON_ELF_H__

#ifndef UNIX
typedef unsigned int size_t;  /* define for elf.h */
typedef int FILE;             /* kludge for elf.h */
#endif  /* UNIX */


#include "../../h/elf.h"
#define ELFMAGIC        0x7f454c46      /* The first longword of the file */
#define ELFCOMPRESSMAGIC 0x1f9d907f     /* magic number for a compressed ELF file */


#endif  /* __MON_ELF_H__ */
