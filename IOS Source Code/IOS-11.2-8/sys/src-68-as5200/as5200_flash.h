/* $Id: as5200_flash.h,v 1.1.70.1 1996/06/16 21:19:28 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_flash.h,v $
 *------------------------------------------------------------------
 * as5200_flash.h - Platform specific Flash routines AS5200.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_flash.h,v $
 * Revision 1.1.70.1  1996/06/16  21:19:28  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:12  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PAN_FLASH_H__
#define __PAN_FLASH_H__

/*
 * Flash address defines.
 */
#define FLASH_BASE		ADRSPC_BRASIL_FLASH_BASE
#define BOOT_FLASH_BASE		ADRSPC_BOOT_FLASH_BASE

/*
 * Motherboard CONTROL REG #2 flash specific bit defines
 */
#define WRITE_SYSTEM_FLASH	0x0D
#define WRITE_BOOT_FLASH	0x13
#define BRASIL_FLASH_VPP_MODE	0x01
#define BOOT_FLASH_VPP		0x03
#define SYSTEM_FLASH_VPP	0x05

/*
 * Motherboard CONTROL REG #3 flash specific bit defines
 */
#define FLASH_SIMM_SIZE_MASK	0x07
#define FLASH_SIMM0_SIZE_SHIFT	0x0
#define FLASH_SIMM1_SIZE_SHIFT	0x03
#define FLASH_SIMM1		0x80
#define FLASH_ONE_MEG		0x4
#define FLASH_TWO_MEG		0x5
#define FLASH_FOUR_MEG		0x6
#define FLASH_EIGHT_MEG		0x7
#define FLASH_SIXTEEN_MEG	0x3

#define SUN             struct sun_exec
/* Data cache controls */
#define CLEAR_DCACHE    0x0800

#define DATA_BUS_REVERSED   1

#include "../dev/flash_codes.h"

#endif /* __PAN_FLASH_H__ */

