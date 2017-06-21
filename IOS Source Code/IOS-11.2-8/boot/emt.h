/* $Id: emt.h,v 3.5.12.3 1996/05/10 23:16:52 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/emt.h,v $
 *------------------------------------------------------------------
 * emt.h -- emulator trap definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: emt.h,v $
 * Revision 3.5.12.3  1996/05/10  23:16:52  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.5.12.2  1996/04/05  02:41:45  yoko
 * CSCdi53681:  add new cpu types
 * Branch: California_branch
 * define new cpu types and emt calls
 *
 * Revision 3.5.12.1  1996/03/26  21:43:44  szhang
 * CSCdi52667:  Add new emt call for DRAM parity
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/17  05:29:56  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checksum
 * validation to fail
 *
 * Revision 3.4  1996/02/12  19:25:42  mbeesley
 * CSCdi48696:  Need EMT calls for fault history
 *
 * Revision 3.3  1996/01/30  21:10:58  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.2  1995/11/17  07:41:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/08  17:52:19  dbath
 * CSCdi38506:  Dual RSP2 master/slave arbitration needs implementing
 * Make sure the new EMT codes used in the VIP_branch are also reserved
 * in the mainline, lest someone else grab them.
 *
 * Revision 2.2  1995/09/08  00:48:10  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.1  1995/06/07  19:11:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	EMT_TICKS		0 /* clock counter */
#define	EMT_PUTCHAR		1 /* putchar(c) */
#define EMT_VERSION		2 /* rom version */
#define EMT_GETCHAR		3 /* getchar() */
#define	EMT_GETMEMSIZE		4 /* return memory size */
#define EMT_SETECHO		5 /* set console echoing */
#define	EMT_GETCONFIG		6 /* return configuration register */
#define EMT_PROCTYPE    	7 /* return processor type */
#define	EMT_RELOAD		8 /* halt and reload */
#define EMT_BOOTIPADDR		9 /* return IP of host we booted from */
#define EMT_VSTRING		10 /* return pointer to version string */
#define	EMT_CLOCKADDR		11 /* return address of clock in locore */
#define	EMT_TOUCH		12 /* return TRUE if an address exists */
#define	EMT_TOUCHR		12 /* return TRUE if read access exists */
#define	EMT_LASTERROR		13 /* return pointer to last error data */
#define	EMT_ROMADDR		14 /* return address of rom beginning */
#define	EMT_HCLOCKADDR		15 /* return address of upper clock long */
#define	EMT_TOUCHW		16 /* return TRUE if write access exists */
#define EMT_CONSOLE		17 /* perform console port functions (STS10) */
#define	EMT_GETCAP		18 /* return pointer to read capabilities */
#define EMT_SETCAP		19 /* set capabilities (expects pointer) */
#define EMT_GETCHAR_RAW		20 /* get character from console, unmodified */
#define EMT_RESET_IO		21 /* reset IO devices to quiescent state */

/* IGS only */
#define EMT_GET_IOBASE		22 /* get IO RAM base address */
#define EMT_GET_REV		23 /* get the processor revision byte */
#define EMT_GET_NVRAM_SIZE	24 /* Return size of NVRAM */

/* Processor independant definitions */

#define EMT_PROCNAME		25 /* Return processor type name */
#define EMT_GET_BOOTINFO	26 /* Get bootstrap information pointer */

/* XX and IGS only */
#define EMT_GET_IOSIZE          27 /* return size of io memory */

/* Processor independant definitions */

#define EMT_BOOT_CONFREG	28 /* Get boot-time configuration register */

/* IGS and XX only */
#define EMT_DISABLE_DCACHE      29 /* turn the cache off */
#define EMT_ENABLE_DCACHE       30 /* turn the cache on  */

/* All 68020/68030/68040 processors */
#define EMT_SETCACR             31 /* set the Cache Control Register */

/* RP1 only */
#define EMT_GET_POWERFAIL_ADDR  32 /* get the powerfail struct addr */

/* Cristal/Cancun only */
#define EMT_GET_COOKIE          33 /* get the cookie address */

/* all platforms */
#define EMT_VSTRING_SYS		34 /* get ptr to previous system version */
#define EMT_VSTRING_SYS_SET	35 /* remember currently running version */
#define EMT_VSTRING_SYS_ROM	36 /* get ptr to ROM system version string */
#define EMT_VSTRING_SYS_ROM_SET	37 /* set system version string */

/* All 68020/68030/68040 processors */
#define EMT_WRITE_BAD_MAIN_MEMORY_PARITY 38 /* Write data to main memory   */
                                            /*  location with bad parity   */

#define EMT_SIMBRK		39 /* cause out-of-band break emulation */

#define EMT_NVCOMPRESS_SUPPORT  40 /* No long used.  Subject to be reused */
#define EMT_SWCONFREG_SUPPORT   41 /* Does boot ROM support S/W config-reg */

/* R4k based boxes only */
#define EMT_EXCEPTION_TABLE_ADDR   42 /* exception table address
                                         in EPROM */
#define EMT_CPU_EXCEPTION          43 /* unnatural return to the monitor */
#define EMT_GET_COOKIE_COPY        44 /* Copy cookie into system image buffer*/
#define EMT_SETVAR                 45 /* Set environment variable */
#define EMT_GETVAR                 46 /* Get environment variable */
#define EMT_MON_NVRAM_BYTES        47 /* get no. of bytes of NVRAM used by
                                         monitor */
#define EMT_SETCONFIG              48 /* Set the config register */
#define EMT_FLASH_LOAD_HELPER      49 /* Flash load helper support */
#define EMT_RESET_REASON           50 /* return RSP reset reason register */

#define EMT_CMD_BOOT_SYS_ROM       51 /* Tell monitor to boot system rom */

/* RSP2 only */
#define EMT_SETMS                  52 /* set/clear the default_master variable */
#define EMT_READMS                 53 /* read default_master variable */

/*
 * Fault history management
 */
#define EMT_FAULT_HISTORY_ADDR	   54 /* Fault history buffer address */
#define EMT_FAULT_HISTORY_BYTES    55 /* Size of fault history buffer */
#define EMT_WRITE_FAULT_HISTORY	   56 /* Write fault history to NVRAM */
#define EMT_SYS_CHKSUM_DISABLE	   57 /*
				       * Point to chk sum validiton diable rtn
				       * in system image.
				       */
/*
 * Return parity setting for DRAM on C7100
 */
#define EMT_DRAM_PARITY            58 /* DRAM parity setting */

/*
 * C3500 only
 */
#define EMT_SET_IO_PERCENT         59 /* set percentage of DRAM to be
                                         used for packet memory */
#define EMT_GET_IO_PERCENT         60 /* get percentage of DRAM to be
                                         used for packet memory */
 
/*
 * Console port speed
 */
#define EMT_SET_CON_BAUD           61 /* set console speed in config
                                         register */
#define EMT_GET_CON_BAUD           62 /* get console speed from config
                                         register */

/*
 * This call was introduced by the c1600 (volcano). It
 * returns miscelaneous platform specific characteristics
 * (e.g CPU speed, whether parity is enabled etc).
 */
#define EMT_SYS_CHARS		   63


/*
 * NITRO use
 */
#define EMT_CHASSIS_SLOT           65
#define EMT_BOARD_TYPE             66
#define EMT_MAC_ADDR               67
