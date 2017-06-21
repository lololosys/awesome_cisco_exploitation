
/* $Id: svip_mon_plat_defs.h,v 1.1.6.1 1996/07/08 22:43:09 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_mon_plat_defs.h,v $
 *------------------------------------------------------------------
 * svip_mon_plat_defs.h
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_mon_plat_defs.h,v $
 * Revision 1.1.6.1  1996/07/08  22:43:09  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:23:02  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MON_PLAT_DEFS_H__
#define __MON_PLAT_DEFS_H__
/*
** General defines.
*/

#ifndef ASMINCLUDE

#define ALARM_FREQUENCY (1000)       		/* 1ms */

#define TRUE 1
#define FALSE 0

/*
** The default value for the virtual config register
*/
#define BOOT_DEFAULT	0x0001
/* #define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT) */
#define DEFAULT_CONFIG	DISABLE_BREAK

/*
** Macros for NVRAM write protection routines
** NVRAM is not write protected on this platform so these
** are left empty
*/
#define WR_ENA_NVRAM
#define WR_PROT_NVRAM

/*
 * Macro for nop's surround accesses that may cause bus errors
 */
#ifdef __mips

#define BUSERR_NOP_X4()                  	\
    asm volatile (".set noreorder");         	\
    asm volatile ("nop; nop; nop; nop");        \
    asm volatile (".set reorder"); 

/*
 * Define a write buffer flush macro
 */
#define FLUSH_BUFFERS	flush_all_wb()

/*
 * Define macros for low level primitives
 */
#define LD_ACCESS(a, t, tu)	ld_access(a, t, tu)
#define LDR_ACCESS(a, t, tu)	ldr_access(a, t, tu)
#define LDL_ACCESS(a, t, tu)	ldl_access(a, t, tu)
#define LWR_ACCESS(a, t)	lwr_access(a, t)
#define LWL_ACCESS(a, t)	lwl_access(a, t)
#define LW_ACCESS(a, t)		lw_access(a, t)
#define LH_ACCESS(a, t)		lh_access(a, t)
#define LB_ACCESS(a, t)		lb_access(a, t)

#define SD_ACCESS(a, t, tu)	sd_access(a, t, tu)
#define SDR_ACCESS(a, t, tu)	sdr_access(a, t, tu)
#define SDL_ACCESS(a, t, tu)	sdl_access(a, t, tu)
#define SWR_ACCESS(a, t)	swr_access(a, t)
#define SWL_ACCESS(a, t)	swl_access(a, t)

#endif /* __mips */

#define SCOPE_TRIGGER_ADDR ((unsigned long)&REG_FPGA->led_reg)

/*
 * declare platform specific functions
 */

/* init.c */
extern void save_reset_reason();
extern unsigned char get_reset_reason();

/* rsp_ralib.c */
extern void monlib_vector_init(void);

#if defined(RSP2)
void handle_ms_int();
void enable_ms_int();
#endif

#else	/* ASMINCLUDE */

#define PHY_ADRSPC_LOW_SRAM	0x00000000	/* SRAM heap */
#define PHY_ADRSPC_RAM_END      0x08000000      /* End of RAM area (up to 128 MB) */
#define PHY_ADRSPC_RAM		0x00000000	/* code, data memory */
#define PHY_ADRSPC_PROM_END     0x1FFFFFFF	/* EPROM end */
#define PHY_ADRSPC_PCI_IO	0x10000000	/* PCI IO space */
/*
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1. ROM Monitor use 
 * the KSEG1 most of the time except its stack space
 */
#define KSEG1_ADRSPC_LOW_SRAM        PHY_TO_KSEG1(PHY_ADRSPC_LOW_SRAM)
#define KSEG1_ADRSPC_RAM             PHY_TO_KSEG1(PHY_ADRSPC_RAM)
#define KSEG1_ADRSPC_RAM_END         PHY_TO_KSEG1(PHY_ADRSPC_RAM_END)
#define KSEG1_ADRSPC_CYA_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_CYA_ASIC)
#define KSEG1_ADRSPC_CBUS            PHY_TO_KSEG1(PHY_ADRSPC_CBUS)
#define KSEG1_ADRSPC_NEV_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_NEV_ASIC)
#define KSEG1_ADRSPC_PMA_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_PMA_ASIC)
#define KSEG1_ADRSPC_DBUS            PHY_TO_KSEG1(PHY_ADRSPC_DBUS)
#define KSEG1_ADRSPC_IO_WBERR        PHY_TO_KSEG1(PHY_ADRSPC_IO_WBERR)
#define KSEG1_ADRSPC_PROM            PHY_TO_KSEG1(PHY_ADRSPC_PROM)
#define KSEG1_ADRSPC_PROM_END        PHY_TO_KSEG1(PHY_ADRSPC_PROM_END)
#define KSEG1_ADRSPC_PCIMEM          PHY_TO_KSEG1(PHY_ADRSPC_PCIMEM)
 
#define PHY_ADRSPC_MEMD    	     0x08000000 /* Start of MEMD */
#define KSEG1_ADRSPC_MEMD            PHY_TO_KSEG1(PHY_ADRSPC_MEMD)
 
/* these are virtual device, not physical ones in the SVIP */
#define PHY_ADRSPC_NVRAM             0x00006000
#define KSEG1_ADRSPC_NVRAM           PHY_TO_KSEG1(PHY_ADRSPC_NVRAM)
#define PHY_ADRSPC_VIP_VFPGA         0x00007000
#define KSEG1_ADRSPC_VIP_VFPGA       PHY_TO_KSEG1(PHY_ADRSPC_VIP_VFPGA)
#endif


#endif  /* __MON_PLAT_DEFS_H__ */
