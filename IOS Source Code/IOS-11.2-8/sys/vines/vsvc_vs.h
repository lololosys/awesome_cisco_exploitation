/* $Id: vsvc_vs.h,v 3.2 1995/11/17 18:00:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vsvc_vs.h,v $
 *------------------------------------------------------------------
 * vsvc_vs.h - Support for the `VINES Security' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_vs.h,v $
 * Revision 3.2  1995/11/17  18:00:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************
 *
 *	      Vines Security Definitions
 *
 **************************************************/

#define VVS_NO_ERROR			   0
#define VVS_ERROR_SECURE		   1
#define VVS_ERROR_BAD_NONCE		1804
#define VVS_ERROR_NOT_RUNNING		1806
#define VVS_ERROR_NO_RESOURCES		1812
#define VVS_ERROR_NO_SUCH_CLIENT	1829	/* Undocumented */

/*
 *
 */
#define VVS_PERIOD	(ONEMIN)

/*
 * Definitions for client "login location" security settings.
 */
#define VVS_MAX_LOCATIONS	10


/**************************************************
 *
 *	      Externs
 *
 **************************************************/

extern void vvs_validate_send_call (vinesipcsocket *, vinesnonce *);
