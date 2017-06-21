/* $Id: rvip.h,v 3.4.6.4 1996/08/29 07:32:00 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip.h,v $
 *------------------------------------------------------------------
 * rvip.h - Include file for RVIP (aka Checkers, VIP1)
 *
 * December 94, David Getchell and Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rvip.h,v $
 * Revision 3.4.6.4  1996/08/29  07:32:00  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.4.6.3  1996/07/10  02:38:03  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.4.6.2  1996/06/03  18:04:39  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/03/21  23:50:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:35:51  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  17:29:21  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:19:00  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.4  1996/03/06  21:24:46  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.3  1996/03/06  01:47:52  jwelder
 * CSCdi50800:  Show Diagbus changed to show VIP1 Dram Size
 *
 * Revision 3.2  1995/11/17  18:50:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RVIP_H__
#define __RVIP_H__

/*
 * Particle sizes
 */
#define VIP_PARTICLE_BYTES	512	/* 512 bytes of data */

/*
 * Port Adaptor Bays
 */
#define MAX_PA_BAYS		2     	/* # of PA's on Checkers Core Board */
#define PA0             	0     	/* Port Adaptor 0 */
#define PA1             	1      	/* Port Adaptor 1 */
#define MAX_INTERFACES_PER_PA  	8 	/* no of interfaces per PA */

#define MAX_VIP_INTERFACES      (MAX_INTERFACES_PER_PA * MAX_PA_BAYS)

#define INTERFACE_PA_SHIFT	3	/* shift value for flat numbering */

/*
 * The RP/SP does support more than 8 interfaces per cbus slot.
 * So set the max number of usable interfaces per PA bay to
 * the max number of interfaces per PA bay
 */
#define MAX_USABLE_INTF_PER_PA	MAX_INTERFACES_PER_PA

/*
 * Define minimum PA NMI prorities
 */
#define PA_NMI_PRIOR_HI    	3 	/* Highest priority */
#define PA_NMI_PRIOR_MED   	2 	/* Medium priority */
#define PA_NMI_PRIOR_LOW   	1 	/* Low priority */
#define PA_NMI_PRIOR_ZERO  	0 	/* Priority of unused slots */

/*
 * Define how we break up DRAM
 */
#define RVIP_DRAM_MEMSIZE_8	0x800000	/* 8 meg as proc. memory */
#define RVIP_DRAM_MEMSIZE_32	0x2000000	/* 32 meg as proc. memory */
#define RVIP_DRAM_MEMSIZE_128	0x4000000	/* 128 meg as proc. memory */
#define RVIP_PCIMEM_OFFSET	0x600000	/* Offset into DRAM for PCI */
#define RVIP_PCI_MEMSIZE_8	0x200000	/* 2 meg for PCI memory  */
#define RVIP_PCI_MEMSIZE_32	0x400000	/* 4 meg for PCI memory  */
#define RVIP_PCI_MEMSIZE_128	0x1C00000	/* 28 meg for PCI memory  */

#define NETWORK_SW_INTERRUPT 	0x200

#ifndef ASMINCLUDE

/*
 * assert_sw_interrupt
 */
static inline void assert_sw_interrupt (ulong sw_intr)
{
    asm volatile (".set noreorder");
    asm volatile ("mtc0 %0,$13"
		      :                       /* no outputs */
		      : "r" (sw_intr));       /* input */
    asm volatile (".set reorder");
}

/*
 * Debug macro to enable more effective logic analyzer
 * debugging. Write codes to DRAM uncached.
#define DAS_DEBUG(a) ((*(volatile ulong *)(R4K_K1BASE + 0x24C)) = (ulong)(a))
 *
 */

#define DAS_DEBUG(a)

/*
 * Host platform support
 */
#define RPSP_SELECT	(vsys_vec.rpsp_host)

/*
 * RVIP does not deal with traffic shaping
 */
#define TRAFFIC_SHAPING(pak, shape)   FALSE

/*
 * Prototypes
 */
extern void     vip_context_dump(int, int, void *);

/* src-vip/vip_memd.c */
extern void	install_cbus_handler(void);

/* src-rvip/rvip_dbus.c */
extern void	install_rvip_dbus_handler(void);
extern void	vip_init_dbus_parser(void);

/* src-rvip/rvip.c */
extern void	rvip_reload(int sig, int code, void *context);
extern ulong	rvip_reset_io(void);
extern ulong 	rvip_exception_table_addr(void);
extern void     sched_alloc_command(parseinfo *);
extern void     vip_netint_throttle_show(void);
extern boolean  rvip_map_iomemory(int mem, int iomem);

/* src-rvip/rvip_dma.c */
extern void	install_dma_handler(void);
extern boolean  dmaq_consumer(void);
extern boolean 	vip_ipc_tx_poll(void);

/* asm-mips/asm_4k_rvip.S */
extern boolean 	pending_cbus_dbus_intrpt(void);

#endif /* ASMINCLUDE */

#endif /* __RVIP_H__ */
