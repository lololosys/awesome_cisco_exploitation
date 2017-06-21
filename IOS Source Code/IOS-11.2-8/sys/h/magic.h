/* $Id: magic.h,v 3.2 1995/11/17 09:12:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/magic.h,v $
 *------------------------------------------------------------------
 * magic.h:  info about HP-UX "magic numbers"
 *
 * March 1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: magic.h,v $
 * Revision 3.2  1995/11/17  09:12:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:35:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* where to find the magic number in a file and what it looks like: */
#define MAGIC_OFFSET	0L

struct magic
	 {
	  unsigned short int system_id;
	  unsigned short int file_type;
	 };
typedef struct magic MAGIC;

/* predefined (required) file types: */
#define RELOC_MAGIC	0x106		/* relocatable only */
#define EXEC_MAGIC	0x107		/* normal executable */
#define SHARE_MAGIC	0x108		/* shared executable */

#define AR_MAGIC	0xFF65

/* optional (implementation-dependent) file types: */
#define DEMAND_MAGIC	0x10B		/* demand-load executable */

/* predefined HP-UX target system machine IDs */
#define HP9000_ID	0x208
#define HP98x6_ID	0x20A
#define HP9000S200_ID	0x20C
