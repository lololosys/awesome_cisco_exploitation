/* $Id: nvram.h,v 1.1.4.1 1996/05/09 15:01:14 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/nvram.h,v $
 *------------------------------------------------------------------
 * nvram.h -- 
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc XDI
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nvram.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:14  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:22  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:41  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
	The first 4k is reserved for BOOT ROM information.
	The next 2k is for any special FLASH image batch commands.
	The next 2k is for SMTMIB variables.
	The next 8k is reserved for CRESMIB variables.
	The next 8k is reserved for future use (routing etc).
	The last 8k is reserved for system log.
*/


#define BOOTNVRAM_START		LCPNVRAM
#define BOOTNVRAM_SIZE		(4*1024)
#define	BOOTNVRAM_STARTUP_SIZE	((2*1024)-2)
#define FLASHNVRAM_START	(BOOTNVRAM_START + BOOTNVRAM_SIZE)
#define FLASHNVRAM_SIZE		(2*1024)
#define FLASHNVRAM_STARTUP_SIZE (FLASHNVRAM_SIZE-2)
#define SMTMIB_START		(FLASHNVRAM_START + FLASHNVRAM_SIZE)
#define SMTMIB_SIZE		(2*1024)
#define CRESMIB_START		(SMTMIB_START + SMTMIB_SIZE)
/*
** AC
** increase cresmib to 16k.
**
#define CRESMIB_SIZE		(8*1024)
#define	NVRAM_RESERVED_START	(CRESMIB_START + CRESMIB_SIZE)
#define	NVRAM_RESERVED_SIZE	(8*1024)
#define	LOGNVRAM_START		(NVRAM_RESERVED_START + NVRAM_RESERVED_SIZE)
**
*/
#define CRESMIB_SIZE		(16*1024)
#define	LOGNVRAM_START		(CRESMIB_START + CRESMIB_SIZE)

#define	LOGNVRAM_SIZE		(8*1024)
#define MAGIC_PATTERN		0xfeedface

struct bootNVRAM {
	uChar startup[BOOTNVRAM_STARTUP_SIZE];
	uInt16 startup_cksum;
	uInt32 ipaddr;
	uInt32 ipmask;
	uInt32 ipbroad;
	uInt16 baudRate;
	uInt16 halt;
	uInt16 invokeDiag;
	uInt16 modemControl;
	uInt32 dmp_feedface;
	uInt32 dmpcodestart;
	uInt32 dmpcodesize;
	uInt32 clearConfig;
	uChar empty[2010];
	uInt16 cksum;
};
extern struct bootNVRAM *bootarea;

struct flashNVRAM {
	uChar startup[FLASHNVRAM_STARTUP_SIZE];
	uInt16 cksum;
};

struct smtmibNVRAM {
	uChar data[SMTMIB_SIZE - 2];
	uInt16 cksum;
};

struct cresmibNVRAM {
	uChar data[CRESMIB_SIZE - 2];
	uInt16 cksum;
};

#define	NUM_RESET_ENTRIES	10
struct logNVRAM {
	uInt32	data_valid;
	uInt16	reset_count;
	uInt16	bootrom_failures;
	uInt16	uart_failures;
	uInt16	nmp_flash_cksum_failures;
	uInt16	nmp_flash_pgm_failures;
	uInt16	nmp_dram_failures;
	uInt16	dmp_flash_cksum_failures;
	uInt16	dmp_flash_pgm_failures;
	uInt16	dmp_sram_failures;
	uInt16	dmp_dram_failures;
	uInt16	nmp_exceptions;
	uInt16	reserved[3];
	uInt16	nmp_exception_info[80];
	struct {
	   uInt32	data_valid;
	   uInt16	num_entries;
	   uInt16	curr_entry;
	   char		reset_time[NUM_RESET_ENTRIES][24];
	   char		last_nmp_exception_time[24];
	   uInt32	dmp_exceptions;
	   uInt32	dmp_exception_info[36];
	   char		last_dmp_exception_time[24];
	}	extended_info;
	struct {
	   uInt32	data_valid;
	   uInt32	entx;
	   uInt32	enrx;
	   uInt32	fdditx;
	   uInt32	fddirx;
	   uInt32	encheck_data_valid;
	   uInt32	encheck;
	   uInt32	altlearn_data_valid;
	   uInt32	enaltlearn;
	   uInt32	fddialtlearn;
	}	dmp_tweak_info;
	struct {
	   uInt32	data_valid;
	   uInt32	length;
	   uInt16	version;
	   uInt16	revision;
	   uInt16	nmp_stack_info[80];
	}		nmp_exception_stack;
	struct {
	   uInt32	data_valid;
	   uInt32	length;
	   uInt16	version;
	   uInt16	revision;
	   uInt32	dmp_stack_info[0x40];
	}		dmp_exception_stack;
};

