/* $Id: pred_plat_defs.h,v 3.1.68.4 1996/07/18 17:48:59 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-c7100/pred_plat_defs.h,v $
 *------------------------------------------------------------------
 * pred_plat_defs.h -- Predator ROM monitor defintions
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_plat_defs.h,v $
 * Revision 3.1.68.4  1996/07/18  17:48:59  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010CSCdi63193: Teach the rom
 * monitor
 * about the incompatibilities between rev1 and rev2 gt64010s :
 *         o Addressing flip for bytes accessed thru PCI IO space
 *         o Internal PCI config register addressing differences
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/04/30  17:25:13  szhang
 * CSCdi55355:  fault tolerant boot strategy causes boot cycle
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/04/04  17:05:59  szhang
 * CSCdi52979:  Bootloader hang after OIR HW change
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:31:44  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.9  1996/02/28  01:00:40  szhang
 * CSCdi50105:  C7100 ROMMON code cleanup
 * Branch: ELC_branch
 *
 * Revision 3.1.2.8  1996/02/26  18:43:33  szhang
 * CSCdi49349:  Predator ROM monitor needs to support packed NVRAM
 * Branch: ELC_branch
 *
 * Revision 3.1.2.7  1996/02/13  00:48:59  mbeesley
 * CSCdi48771:  Predator should use fastest DRAM timing possible
 * Branch: ELC_branch
 *
 * Revision 3.1.2.6  1996/02/12  19:12:41  szhang
 * Branch: ELC_branch
 * Move the GT64010 register initial values to pred_plat_defs.h.
 * Add the pcmap display into sizemem.c
 *
 * Revision 3.1.2.5  1996/01/20  01:22:57  szhang
 * Branch: ELC_branch
 * Make the parity detection code work on the Predator platform.
 *
 * Revision 3.1.2.4  1995/12/05  23:12:30  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.1.2.3  1995/12/05  00:54:19  mbeesley
 * Branch: ELC_branch
 * Add support for the L2 cache.
 *
 * Revision 3.1.2.2  1995/12/01  04:23:57  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:50  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:55:14  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PRED_PLAT_DEFS_H__
#define __PRED_PLAT_DEFS_H__

/*
 * General defines.
 */
#define ALARM_FREQUENCY 	(1000)       		/* 1ms */

#define FALSE 0
#define TRUE  1


#define PARITY_NO   0
#define PARITY_YES  1


/*
 * defines for R4K timer
 */
#define COUNT2NANOSEC(count)       	(20 * count)
#define NANOSEC2COUNT(nanosec)     	(nanosec/20)
#define TIMER_EXPIRED(current, timer) 	((current) > (timer))

#define MON_TIMER_VAL_MAX   		(2 * 1000 * 1000 * 1000)

/*
 * The default value for the virtual config register
 */
#define BOOT_DEFAULT		0x0001

/* #define DEFAULT_CONFIG	(DISABLE_BREAK | BOOT_DEFAULT) */

#define DEFAULT_CONFIG		DISABLE_BREAK

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

#define SCOPE_TRIGGER_ADDR 	((unsigned long)&KSEG1_PRED_FPGA->led_reg)

/*
 * Define the initial values for the system controller gt64010.
 */

/*
 * CPU Interface config : 
 * 	- L2 cache present, tagop connected to Addr35:34
 *	- Big Endian
 *	- Pipelined writes
 */
#define CPU_INTERFACE_CONF_INIT            0x80030000

/*
 * The initial values defined herewith are map to the maximum
 * configuration so that mem sizing can be performed 
 */
#define RAS10_LOW_INIT                     0x00000000
#define RAS10_HIGH_INIT                    0x1f000000

#define RAS32_LOW_INIT                     0x20000000
#define RAS32_HIGH_INIT                    0x3f000000

#define CS20_LOW_INIT                      0xd0000000
#define CS20_HIGH_INIT                     0x74000000

#define PCI_IO_LOW_INIT                    0x00080000
#define PCI_IO_HIGH_INIT                   0x00000000
 
#define PCI_MEM_LOW_INIT                   0x00020000
#define PCI_MEM_HIGH_INIT                  0x7F000000
 
#define CS3_BOOT_LOW_INIT                  0xf8000000
#define CS3_BOOT_HIGH_INIT                 0x7e000000

#define RAS0_LOW_INIT                      0x00000000
#define RAS0_HIGH_INIT                     0x1f000000

#define RAS1_LOW_INIT                      0x20000000
#define RAS1_HIGH_INIT                     0x3f000000

#define RAS2_LOW_INIT                      0x40000000
#define RAS2_HIGH_INIT                     0x5f000000

#define RAS3_LOW_INIT                      0x60000000
#define RAS3_HIGH_INIT                     0x7f000000

#define DRAM_CONFIG_INIT                   0x0c030200   /* 0x2030c */
#define DRAM_BANK0_PARAM_INIT              0x70000000
#define DRAM_BANK1_PARAM_INIT              0x70000000
#define DRAM_BANK2_PARAM_INIT              0x70000000
#define DRAM_BANK3_PARAM_INIT              0x70000000

#define CS0_LOW_INIT                       0xa0000000
#define CS0_HIGH_INIT                      0xbf000000
#define CS1_LOW_INIT                       0xe8000000
#define CS1_HIGH_INIT                      0xe8000000
#define CS2_LOW_INIT                       0xe0000000
#define CS2_HIGH_INIT                      0xe0000000
#define CS3_LOW_INIT                       0xf0000000
#define CS3_HIGH_INIT                      0xfb000000
#define BOOT_CS_LOW_INIT                   0xfc000000
#define BOOT_CS_HIGH_INIT                  0xfc000000

#define DEVICE_BANK0_PARAM_INIT            0xffff2f02 
#define DEVICE_BANK1_PARAM_INIT            0xb2b72402
#define DEVICE_BANK2_PARAM_INIT_REV1       0xffff6f02	/* rev 1 gt64010 */
#define DEVICE_BANK2_PARAM_INIT		   0xffff0f02   /* rev 2 gt64010 */

#ifndef ASMINCLUDE

#define GT64010_CLASS_REV_ID_ADDR     0x08000080

#define GT64010_PCI_MEMMAPBASE_INIT   0x00400100

#define EXC_SW_NMI      	13	/* S/W NMI */
#define EXC_UNKNOWN_RESET 	99    	/* Unknown Reset */

#define PCI_PCMCIA_DEV_NUM	5	/* DevSel of PCMCIA controller */

#define PCI_COMMAND_REG_OFFSET		0x04		/* Command register */
#define PCI_IO_BASE_OFFSET		0x10		/* IO base register */

#define CLPD6279_COMMAND_SETTINGS	0x000001C3	/* PCMCIA command */

#define CACHE_SIZE		0x4000

struct simParm {
    ulong rowsize;
    ulong wordwidth;
    ulong size;
    uchar simma_parity;
    uchar simmb_parity;
};

/*
 * declare platform specific functions
 */

/* init.c */
extern void save_reset_reason();
extern unsigned char get_reset_reason();

/* rsp_ralib.c */
extern void monlib_vector_init(void);

/*
 * External functions
 */
extern int 	promscan();
extern int	slot0_size(), slot1_size();
extern long     sizemainmem(void), getflashsiz(void), bflash_siz();

/*
 * External Variables
 */
extern long vec_start, vec_end;

extern unsigned char bank_parity;    /* used to store parity flags for IOS */

/*
 * Function Prototype 
 */
int lw_instr(unsigned long *addr);

/*
 * sizemem.c
 */
int check_dram_config();
 
#endif /* ASMINCLUDE */

#endif  /* __PRED_PLAT_DEFS_H__ */



