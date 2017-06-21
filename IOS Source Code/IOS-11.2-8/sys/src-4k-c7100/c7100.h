/* $Id: c7100.h,v 3.2.56.17 1996/08/29 07:31:54 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100.h,v $
 *------------------------------------------------------------------
 * c7100.h - Predator support
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100.h,v $
 * Revision 3.2.56.17  1996/08/29  07:31:54  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.2.56.16  1996/08/28  07:46:05  abaindur
 * CSCdi67069:  Data cache use needs to be turned ON for c7200 IP flow
 * switching
 * Branch: California_branch
 *  Fix mixup of cached and uncached region pointers. Fix cases where
 *  converted addresses were not restored if flow switching failed.
 *
 * Revision 3.2.56.15  1996/08/20  00:10:27  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.2.56.14  1996/08/09  22:42:28  abaindur
 * CSCdi65580:  Flow and optimum switching does not work on C7200 with
 * FDDI PA
 * Branch: California_branch
 * Temporarily back out change to use CPU data cache for flow/optimum
 * switching.
 *
 * Revision 3.2.56.13  1996/07/26  23:54:25  ssangiah
 * CSCdi63705:  Need to add particle-based XNS fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.2.56.12  1996/07/24  20:46:13  ssangiah
 * CSCdi63706:  Need to add particle-based CLNS fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.2.56.11  1996/07/23  20:16:50  mbeesley
 * CSCdi63541:  Improve optimum switching performance on c7200
 * Branch: California_branch
 *
 * Revision 3.2.56.10  1996/07/03  00:11:32  ssangiah
 * CSCdi58629: Need to add particle-based decnet fast-switching support.
 * Branch: California_branch
 *
 * Revision 3.2.56.9  1996/06/26  18:40:12  mbeesley
 * CSCdi61260:  Make output of show c7200 more usefull to manufacturing
 * Branch: California_branch
 *
 * Revision 3.2.56.8  1996/05/31  23:44:19  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.2.56.7  1996/05/24  20:33:03  ssangiah
 * CSCdi58375:  Need to add ISL support for new platform.
 * Add ISL support for the new c7200 platform.
 * Branch: California_branch
 *
 * Revision 3.2.56.6  1996/05/24  01:37:58  mbeesley
 * CSCdi57393:  GT64010 DMA engines can hog MB0 bandwidth
 * Branch: California_branch
 *
 * Revision 3.2.56.5  1996/05/21  20:44:05  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.56.4  1996/05/07  00:24:01  mbeesley
 * CSCdi56157:  Enable IP Flow/Optimum on Ethernets
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/05/01  13:28:17  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.56.2  1996/04/23  19:21:51  mbeesley
 * CSCdi55360:  Enable cached, write thru view of packet memory
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/03/21  23:47:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.11  1996/03/08  21:36:25  mbeesley
 * CSCdi51090:  Need to support rev 2 CPU card
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/03/01  18:28:35  gstovall
 * CSCdi50508:  Particle based vines fastswitching not supported
 * Branch: ELC_branch
 * Add vines to the list of protocols that support particle based
 * fastswitching.
 *
 * Revision 3.2.2.9  1996/02/19  23:24:19  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.2.2.8  1996/02/14  02:30:39  wfried
 * CSCdi48945:  Implement particle-based Appletalk fastswitching
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/13  03:44:53  sthormod
 * CSCdi48780:  Allow coalesced fastswitching on tokenring
 * Branch: ELC_branch
 * Coalesce the packet before fastswitching if we don't support particle
 * based fastswitching for a given protocol.
 *
 * Revision 3.2.2.6  1996/02/01  06:48:05  mbeesley
 * Branch: ELC_branch
 * Add macro to control particle based fastswitching. Also, set the
 * gt64010 into sync mode.
 *
 * Revision 3.2.2.5  1996/01/26  22:10:34  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.2.2.4  1996/01/23  02:18:10  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.2.2.3  1995/12/12  04:50:46  mbeesley
 * Branch: ELC_branch
 * Add an enumeration for the IO card 1fe device number. Also fix some
 * DRAM PCI configuration in the GT64010.
 *
 * Revision 3.2.2.2  1995/11/30  02:13:50  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:03  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Define CPU card types
 */
#define C7100_CPU_UNKNOWN	0	/* Don't know what it is */
#define C7100_CPU_NPE100	1	/* no packet SRAM */
#define C7100_CPU_NPE150	2	/* packet SRAM */
#define C7100_CPU_NPE200	3	/* 200Mhz P4 */

/*
 * Define revision numbers
 */
#define C7100_VERSION_UNKNOWN	0	/* unprogrammed CPU eeprom */
#define C7100_VERSION_REV1	1	/* no pci-sram connection */
#define C7100_VERSION_REV2	2	/* pci-sram connection */

/*
 * Particle sizes
 */
#define C7100_PARTICLE_BYTES	512	/* 512 bytes of data */

/*
 * Port Adaptor Bays
 */
#define MAX_PA_BAYS		7     	/* # of PA's on Predator */
#define MAX_INTERFACES_PER_PA  	8 	/* no of interfaces per PA */

#define MAX_C7100_INTERFACES  	(MAX_INTERFACES_PER_PA * MAX_PA_BAYS)

/*
 * Define PCMCIA device number
 */
#define PCI_PCMCIA_DEV_NUM	5	/* Device 5 on bus 0 */

/*
 * Define the IO card 1fe device number
 */
#define PCI_IO1FE_DEV_NUM	6	/* Device 6 on bus 0 */

/*
 * Define parameters foir PCI config of packet SRAM
 */
#define PCI_SRAM_BUS0		4       /* On bus 4, mb1 */
#define PCI_SRAM_BUS1		10	/* On bus 10, mb2 */

#define SRAM_DEV_NUM		0    	/* SRAM is device 0 on both buses */

#define PCI_SRAM_VENDOR_ID	0x00051137	/* Vendor/Device ID */
#define PCI_SRAM_COMMAND	0x00000142	/* Turn on the PCI port */

/*
 * Define minimum PA NMI prorities
 */
#define PA_NMI_PRIOR_HI    	3 	/* Highest priority */
#define PA_NMI_PRIOR_MED   	2 	/* Medium priority */
#define PA_NMI_PRIOR_LOW   	1 	/* Low priority */
#define PA_NMI_PRIOR_ZERO  	0 	/* Priority of unused slots */

/*
 * Fine granularity scheduler interval processing constants
 */
#define NETINT_MASK_MIN		100	/* min usecs with net ints off */
#define NETINT_MASK_DEFAULT	200	/* default usecs with net ints off */
#define NETINT_MASK_MAX		4000	/* max usecs with net ints off */
#define NETINT_MIN		400	/* min usecs in net io handler */
#define NETINT_DEFAULT		4000	/* default usecs in net io handler */
#define NETINT_MAX		60000	/* max usecs in net io handler */

#define NETINT_THROTTLE_DEFAULT	FALSE	/* default state of throttling */

#define NETWORK_SW_INTERRUPT 	0x200

/*
 * Number fo DMA entries for coalescing dma engine
 */
#define GT64010_DMA0_ENTRIES	256

#ifndef ASMINCLUDE

/*
 * Enum for the ID eeprom test routines
 */
enum {
    TEST_C7100_EEPROM_READ = 1,
    TEST_C7100_EEPROM_WRITE,
};

#define HIT_INVALIDATE_DATA		"17"
#define HIT_WRITEBACK_INVALIDATE_DATA	"21"
#define CREATE_DIRTY_EXCLUSIVE_DATA	"13"

/*
 * invalidate_data_cache_line:
 * Invalidate one cache line from internal data cache
 */
static inline void invalidate_data_cache_line (uchar *cline_ptr)
{
    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    /* Invalidate cache line */

    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)"
		 : /* no output */
		 : "r" (cline_ptr));

    asm volatile(".set mips2");
    asm volatile(".set reorder");
}

/*
 * invalidate_data_lines:
 * Invalidate some data cache lines
 */
static inline void invalidate_data_lines (uchar *cline_ptr, int lines)
{
    /*
     * Align the pointer to a cache line boundary
     */
    cline_ptr = (uchar *)((ulong)cline_ptr & ~(C7100_DCACHE_LINE_SIZE-1));

    /*
     * Invalidate the lines
     */
    do {
	invalidate_data_cache_line(cline_ptr);
	cline_ptr += C7100_DCACHE_LINE_SIZE;
	lines -= 1;
    } while (lines > 0);
}

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
 * typedef for post-coalesce callback routine
 */
typedef void		(*c7100_coalesce_v)(hwidbtype *, paktype *);

/*
 * Externs
 */
extern boolean	c7100_debug;
extern boolean	c7100_has_packet_sram;
extern boolean  oir_init_mode;
extern leveltype oir_level;

/*
 * Prototypes
 */
extern void	c7100_read_cpueeprom(void *);
extern void 	c7100_write_cpueeprom(void *);
extern void 	c7100_show_cpueeprom(void);
extern void	c7100_dump_ideeprom(uchar *);
extern int	c7100_cpucard_version(void);
extern int	c7100_cpucard_type(void);
extern void	c7100_exception_init(void);
extern void 	c7100_pci_init(void);
extern boolean	c7100_pci_enable_packet_sram(void);
extern void 	c7100_setup_hardware_registers(void);
extern boolean 	c7100_verify_sysconfig(uint pa_bay);
extern boolean	c7100_map_iomemory(int mem, int iomem);
extern boolean	c7100_bay2vaddr(uint pa_bay, boolean inout, boolean valid);
extern void	c7100_netint_throttle_show(void);
extern boolean 	c7100_oir_interrupt(ulong oir_event);
extern boolean 	c7100_envmon_interrupt(ulong envmon_event);
extern void	c7100_oir_throttle(boolean);
extern boolean	c7100_pak_coalesce(hwidbtype *, paktype *, c7100_coalesce_v);
extern boolean	c7100_dma_pak_coalesce(hwidbtype *, paktype *, 
				       void *, uchar *);

extern hwidbtype* c7100_get_matching_idb(int slot, int interface,
					 int pa_type, enum IDBTYPE idb_type);
extern void     c7100_remove_hwidbs(slottype *);
extern void	sched_alloc_command(parseinfo *csb);

#endif /* ASMINCLUDE */

/*
 * Debug macro to enable more effective logic analyzer
 * debugging. Write codes to DRAM uncached.
 *
 * define DAS_DEBUG(a) ((*(volatile ulong *)(R4K_K1BASE + 0x24C)) = (ulong)(a))
 */
#define DAS_DEBUG(a)

/*
 * C7100 deals with traffic shaping
 */
#define TRAFFIC_SHAPING(pak, shape)	\
        (shape = traffic_shape_required_inline(pak->if_output->traffic_shape,\
					       pak))

/*
 * Control whether IP Optimum/Flow switching uses the internal 
 * CPU's data cache. Use it as it makes the box go lots faster.
 */
#define C7100_FLOW_THRU_DCACHE	TRUE

/*
 * Define a macro to tell which protocols are fastswitchable while
 * the packet is still in particles. This is a macro instead of
 * an inline to prevent all modules that include this file from having
 * to include interface_private.h first.
 */
#define c7100_scattered_fs_protocol(proto)			\
    ((proto == RXTYPE_DODIP) 		||			\
     (proto == RXTYPE_SNAP_DODIP)       ||                      \
     (proto == RXTYPE_NOVELL1) 		||			\
     (proto == RXTYPE_NOVELL2) 		||			\
     (proto == RXTYPE_SNAP_NOV1) 	||			\
     (proto == RXTYPE_SAP_NOVELL)	||			\
     (proto == RXTYPE_APPLE)	 	||			\
     (proto == RXTYPE_SNAP_APPLE)	||			\
     (proto == RXTYPE_VINES)		||			\
     (proto == RXTYPE_SNAP_VINES)	||			\
     (proto == RXTYPE_SAP_VINES)	||			\
     (proto == RXTYPE_ISL)		||			\
     (proto == RXTYPE_DECNET)		||			\
     (proto == RXTYPE_CLNS)		||			\
     (proto == RXTYPE_XNS)		||			\
     (proto == RXTYPE_SNAP_XNS)		||			\
     (proto == RXTYPE_IEEE_802_10))

/* end of file */
