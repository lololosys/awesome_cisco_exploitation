/* $Id: config_register.h,v 3.2 1995/11/17 09:11:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/config_register.h,v $
 *------------------------------------------------------------------
 * config_register.h - Definitions for configuration register
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config_register.h,v $
 * Revision 3.2  1995/11/17  09:11:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Configuration Register Bits
 */

#ifndef ASMINCLUDE
extern unsigned short configregister;
#endif ASMINCLUDE

#define CFG_DIAGNOSTIC  0x8000		/* diagnostic mode */
#define CFG_SUBNET	0x4000
#define CFG_LANCONFIG	0x2000
#define CFG_ZEROS	0x0400
#define CFG_SECONDARY	0x0200
#define CFG_BREAK	0x0100		/* If set, BREAK is disabled */
#define CFG_OEM		0x0080		/* OEM mode (no cisco copyright) */
#define CFG_NOCONFIG	0x0040		/* Ignore NVRAM */
#define CFG_FILE	0x000F

#define CFG_DEFAULT	(CFG_BREAK | 1)	/* Default configuration */


