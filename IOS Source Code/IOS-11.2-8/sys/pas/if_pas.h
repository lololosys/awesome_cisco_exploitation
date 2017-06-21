/* $Id: if_pas.h,v 3.7.20.11 1996/08/24 01:51:11 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas.h,v $
 *------------------------------------------------------------------
 * if_pas.h : PCI Port Adaptor header file
 *
 * January 1995, David Getchell and Michael Beesley
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_pas.h,v $
 * Revision 3.7.20.11  1996/08/24  01:51:11  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.7.20.10  1996/05/24  00:49:50  mbeesley
 * CSCdi58316:  Need to fully support CPQ on new interfaces
 * Branch: California_branch
 *
 * Revision 3.7.20.9  1996/05/01  13:55:54  dcarroll
 * CSCdi56037:  provide oir active check routine
 * Branch: California_branch
 * Need 'oir active check' routine.
 *
 * Revision 3.7.20.8  1996/04/12  17:43:12  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.7.20.7  1996/04/08  19:00:47  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.7.20.6  1996/04/08  13:44:53  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.7.20.5  1996/04/05  19:56:03  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.7.20.4  1996/04/05  06:30:13  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.7.20.3  1996/04/05  05:32:08  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.7.20.2  1996/04/03  18:25:22  dcarroll
 * CSCdi53341:  clean up tables after oir removal
 * Branch: California_branch
 * Add routines to uninstall tables.
 *
 * Revision 3.7.20.1  1996/03/21  23:23:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.13  1996/03/18  23:28:43  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.12  1996/02/21  02:30:49  mbeesley
 * CSCdi49422:  Turn debugging off for port adaptor drivers
 * Branch: ELC_branch
 *
 * Revision 3.2.2.11  1996/02/19  23:23:56  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/02/17  05:54:31  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.2.2.9  1996/02/14  01:58:05  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.2.2.8  1996/02/10  00:30:36  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/08  08:47:50  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/25  12:10:05  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.2.2.5  1996/01/18  05:41:47  mbeesley
 * Branch: ELC_branch
 * Optimize 1fe and 4e driver. Fix 4e fastsend bug. Patch in Tx enable
 * fix for 1fe. Debug IP fastswitching.
 *
 * Revision 3.2.2.4  1996/01/16  01:42:38  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/05  06:35:50  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.2.2.2  1995/11/30  02:04:41  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:41:50  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.7  1996/02/04  09:32:47  rharagan
 * CSCdi45869:  4T does not receive data when cabled as rs-232 DCE
 * Set no_txce control based on sensed cable type, handle rx errors.
 *
 * Revision 3.6  1996/01/12  21:52:46  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.5  1995/12/26  23:10:10  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.4  1995/12/19  20:24:20  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.3  1995/12/01  01:59:58  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:54:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * If the drivers need to be compiled up with debug code 
 * enabled, then uncomment the line below :
 *
 * #define PAS_DEBUGGING
 */

/*
 * Define debug macros and flags
 */
#if defined(PAS_DEBUGGING)

#define PAS_DEBUG(a)	a
#define PAS_DEBUG_BOOL  TRUE
#define PAS_DEBUG_FLAG

#else /* no debugging support */

#define PAS_DEBUG(a)
#define PAS_DEBUG_BOOL  FALSE
#undef  PAS_DEBUG_FLAG

#endif

/*
 * Include platform specific files
 */
#if 	defined(RVIP)
#include "../src-rvip/rvip.h"
#include "../src-rvip/rvip_pcmap.h"
#include "../src-rvip/rvip_asic.h"
#endif	/* RVIP */

#if     defined(SVIP)
#include "../src-svip/svip.h"
#include "../src-svip/svip_pcmap.h"
#include "../src-svip/svip_asic.h"
#endif /* SVIP */

#if 	defined(PREDATOR)
#include "../src-4k-c7100/c7100.h"
#include "../src-4k-c7100/c7100_pcmap.h"
#endif

/*
 * Define the instance vector off the idb
 */
#define INSTANCE(idb)		((idb)->devctl)

/*
 * PA management typedef
 */
typedef struct pas_management_t_ {

    uint pa_bay;			/* Port Adaptor bay */

    /*
     * PA ID eeprom clock register
     */
    ulong clk_reg_width;
    ulong clk_mask;
    volatile void *clk_reg;

    /*
     * PA ID eeprom select register
     */
    ulong select_reg_width;
    ulong select_mask;
    volatile void *select_reg;

    /*
     * PA ID eeprom datain register
     */
    ulong datain_reg_width;
    ulong datain_mask;
    volatile void *datain_reg;

    /*
     * PA ID eeprom dataout register
     */
    ulong dataout_reg_width;
    ulong dataout_mask;
    volatile void *dataout_reg;

} pas_management_t;

typedef int pas_eeprom_type;		/* EEPROM device type */
typedef int pas_eeprom_cmd_t;		/* EEPROM command type */

/*
 * Define the two supported ID eeproms
 */
#define PAS_EEPROM_X2444		1
#define PAS_EEPROM_NMC93C46		2

/*
 * Define the command set supported
 */
#define PAS_EEPROM_CMD_READ		1
#define PAS_EEPROM_CMD_WRITE		2
#define PAS_EEPROM_CMD_ERASE		3
#define PAS_EEPROM_CMD_ZERO		4

/*
 * Define timings constants for the EEPROM's
 */
#define PAS_EEPROM_DELAY		10	/* 10 us */
#define PAS_X2444_DELAY			5000	/* 5000 us */
#define PAS_NMC93C46_WRITE_DELAY        10000   /* 10 ms */
/* 
 * Define NMC93C46 command codes      	SB    OP    Addr[5,0]     
 */
#define NMC93C46_CMD_CONTROL   		(0x1 | 0x0)
#define NMC93C46_CMD_WRDS      		(0x1 | 0x0 | 0x00)
#define NMC93C46_CMD_ERASE_ALL 		(0x1 | 0x0 | 0x08)
#define NMC93C46_CMD_WRITE_ALL 		(0x1 | 0x0 | 0x10)
#define NMC93C46_CMD_WREN      		(0x1 | 0x0 | 0x18)
#define NMC93C46_CMD_READ      		(0x1 | 0x2)
#define NMC93C46_CMD_WRITE     		(0x1 | 0x4)
#define NMC93C46_CMD_ERASE     		(0x1 | 0x6)

/*
 * External pas_management_t list
 */
extern pas_management_t *pas_management_list[MAX_PA_BAYS];

/*
 * typedef for post-coalesce function callback
 */
typedef void 		(*pas_coalesce_v)(hwidbtype *, paktype *, iqueue_t);

/* 
 * Prototypes for pas_cbtype vectors
 */
typedef void		(*pas_deallocate_t)(hwidbtype *);

typedef boolean 	(*pak_to_host_t)(hwidbtype *, pas_cbtype *, paktype *);
typedef boolean		(*pak_switch_t)(void *, paktype *, 
					volatile void *,
					volatile ulong *);
typedef boolean 	(*pak_dfs_t)(hwidbtype *, paktype *);

/*
 * pas_cbtype data structure
 */
struct pas_cbtype_ {

    /*
     * pas_cbtype function vectors
     */
    pas_deallocate_t	pas_deallocate;	 /* deallocate interface resources */
    pak_to_host_t	pak_to_host;	 /* receive vector */
    pak_switch_t	pak_switch; 	 /* distributed fastswitching vector */
    pak_dfs_t		pak_dfs; 	 /* dfs vector */

    /*
     * Pointer to VIP memd interface control data structures and ISL
     * support variables
     */
    void		*vip_memd_ifcntl; /* VIP control data structure ptr */
    void		*vip_af;	/* VIP AF data structure ptr */

    tinybool		isl_supported;	/* ISL can be enabled */
    tinybool		isl_enabled;	/* ISL is enabled */

    ushort		vip_af_entries;	/* number of AF entries */
    ushort		vip_af_index;	/* AF data structure index */

    /*
     * VIP dma control variables
     */
    ushort 		tx_polling_high; /* Count at which to stop polling */
    ushort 		tx_polling_low;  /* Count at which to start polling */
    uint		*txring_cnt_ptr; /* pointer to counter for tx ring */
    pas_cbtype  	*next;           /* pointer to polling disabled ints */

    /*
     * Houekeeping fields
     */
    hwidbtype		*idb;		/* interfaces hwidb */
    ulong		pa_ifindex;	/* linear index, max per bay */
    ushort		pa_bay;		/* PA bay for this interface */
    ushort		pa_device;	/* Interface no. on this bay */
};

/*
 * PA wrapper typedefs
 */
typedef int (pa_wrapper_t)(void);
typedef void (pa_nmi_wrapper_t)(void *);

/*
 * Port Adaptor NMI handling typedef
 */
typedef struct pa_nmi_handler_struc {
    pa_nmi_wrapper_t *func;
    void *arg;
    ushort mask;
    ushort prior;
} pa_nmi_handler_t;

/*
 * Port Adaptor Network IO handling typedef. Note, if this changes please
 * update the definitions in the wrapper support header files !!
 */
typedef struct pa_jumptable_t_ {
    pa_wrapper_t *wrapper;		/* Wrapper pointer  */
    void  *pa_base_address;            	/* Port Adaptor base address */
    ulong pa_scratch0;			/* Scratch pad 0 for wrapper */
    ulong pa_scratch1;			/* Scratch pad 1 for wrapper */
    void *idb[MAX_INTERFACES_PER_PA];   /* Array of pointers to idbs */
    void *s_reg[MAX_INTERFACES_PER_PA]; /* Array of pointers to status regs */
} pa_jumptable_t;

/*
 * External declarations
 */
extern pa_nmi_handler_t *pa_nmi_dispatch_table[];
extern pa_wrapper_t pa_default_wrapper;
extern pa_jumptable_t pa_jumptable[];

/*
 * Defines to control PA's
 */
#define MAX_PCI_DEVICES		16

/*
 * Bit definitions for idb->oir_flags.
 * Taken from if_fci.h
 */

#define IDB_FIRST_INIT	0x0001	/* Brand new PA. Needs early init */
#define IDB_FINAL_INIT	0x0002	/* Brand new PA. Needs final init */

/*
 * Macros
 */

/*
 * pas_stuff_pak_for_holdq:
 * Copy the fields from pak to newpak that are needed by CPQ/WFQ stuff.
 *
 * Note : if_output, datagramstart and datagramsize are assumed
 *        to be setup outside of this macro
 */
#define pas_stuff_pak_for_holdq(newpak, pak)			\
    newpak->rxtype = pak->rxtype;				\
    newpak->linktype = pak->linktype;				\
    newpak->if_input = pak->if_input;				\
    newpak->network_start = pak->network_start;			\
    newpak->mac_start = pak->mac_start;				\
    newpak->if_output = pak->if_output;				\
    newpak->flags = pak->flags;                                 \
    newpak->traffic_shape = pak->traffic_shape;


/*
 * Function prototypes
 */
extern void	pas_parser_init(void);
extern void	pas_subsys_init_class(ushort);
extern void	pas_init_jumptable(void);
extern void	pas_init_network_level(void);
extern void	pas_if_final_init(void);
extern void	pas_platform_if_final_init(void);
extern void	pas_init_pa_nmi_handler(void);
extern void	pas_init_fspak_pool(void);

extern void	pas_netio_interrupts(uint, boolean);
extern void	pas_mngmt_interrupts(uint, boolean);
extern void	pas_install_wrapper(uint, pa_wrapper_t);
extern void	pas_install_port_instance(hwidbtype *, boolean);
extern void     pas_install_status_reg(hwidbtype *, void *, boolean);
extern void	pas_install_pa_nmi_handler(uint, pa_nmi_wrapper_t, void *,int);
extern void     pas_uninstall_jumptable_entry(uint);
extern void     pas_uninstall_pa_nmi_handler(uint);

extern boolean	pas_bridge_config(uint);
extern void	pas_deactivate_bay(uint);
extern void	pas_control_reset(uint, boolean);
extern void	pas_control_power(uint, boolean);
extern void	pas_pci_controller_summary(void);
extern boolean	pas_pa_present(uint);
extern void	pas_nmi_dispatcher(void);
extern void	pas_instance_init_common(pas_cbtype *, hwidbtype *);
extern pooltype *pas_interface_fallback_pool(hwidbtype *);
extern pooltype *pas_interface_header_pool(hwidbtype *);
extern paktype  *pas_allocate_fspak(void);
extern mempool	*pas_buffer_mempool(hwidbtype *);
extern boolean  pas_access_sys_eeprom(pas_management_t *, void *,
				      pas_eeprom_type, pas_eeprom_cmd_t,
				      ushort);
extern boolean  pas_access_sys_eeprom_partial(pas_management_t *, void *,
				              pas_eeprom_type, pas_eeprom_cmd_t,
				              ushort, ushort);
extern boolean 	pas_access_ideeprom(int, void *, pas_eeprom_type,
				    pas_eeprom_cmd_t, ushort);
extern void 	pas_get_eeprom_data(uint pa_bay, void *ptr,
                                 ushort read_count);

extern uchar 	get_pci_bus_no(uint);
extern void	*get_io_base_address(uint);
extern void 	*get_pci_device_base(uint, uint);
extern ulong	get_pci_device_base_offset(uint, uint);
extern ushort	get_pa_type(uint);

extern void	*pci_memaddr_convert(void *, int);

extern void     pas_add_card_entry(uint);
extern void     pas_analyze_interfaces(void);
extern void     pas_activate_bay(uint);
extern boolean  pas_pa_analyzed(uint);
extern void     pas_pa_exists(uint);
extern ushort   pas_get_pa_type(uint pa_bay);
extern void     pas_oir_cleanup(uint);
extern ushort   get_pa_version(uint);
extern void     pas_set_fci_type(hwidbtype *idb, uint slot);
extern void     pas_slots_init(void);
extern void     pas_delay(uint);
extern boolean  pas_oir_active(void);

/* end of file */
