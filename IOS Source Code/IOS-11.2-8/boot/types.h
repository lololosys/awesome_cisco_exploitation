/* $Id: types.h,v 3.3 1995/11/17 07:42:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/types.h,v $
 *------------------------------------------------------------------
 * Basic system types.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: types.h,v $
 * Revision 3.3  1995/11/17  07:42:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  22:48:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  09:05:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef unsigned char	uchar;		/* System V compatibility */
typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
typedef unsigned long	ulong;		/* System V compatibility */
typedef int		boolean;
typedef unsigned long long ulonglong;

typedef unsigned char tinybool;
typedef int pid_t;

/*
 * charlong data structure
 * Used for manipulating longs, shorts, and bytes.
 */

typedef struct charlong_ {
	union {
	    uchar byte[4];
	    ulong lword;
	    ushort sword[2];
	} d;
} charlong;

typedef struct sys_timestamp_ {
    union {
	ulonglong value;
	struct {
	    ulong high;
	    ulong low;
	} p;
    } u;
} sys_timestamp;
