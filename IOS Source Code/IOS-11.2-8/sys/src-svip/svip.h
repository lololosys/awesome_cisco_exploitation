/* $Id: svip.h,v 3.1.62.8 1996/08/29 07:32:06 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip.h,v $
 *------------------------------------------------------------------
 * svip.h
 * Include file for SVIP definitions and externals
 *
 * Sept, 1995  David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: svip.h,v $
 * Revision 3.1.62.8  1996/08/29  07:32:06  abaindur
 * CSCdi67514:  Spurious accesses while local switching on VIP
 * Branch: California_branch
 *  pak->if_output was not being set for the traffic shaping check.
 *  Bypass traffic shaping on the VIP for now.
 *
 * Revision 3.1.62.7  1996/07/10  02:38:07  getchell
 * Added crash context mechanism which is shown via diag command and test
 * cbus (vc command)
 * CSCdi60415:  VIP difficult to debug in the field
 * Branch: California_branch
 *
 * Revision 3.1.62.6  1996/06/03  18:04:46  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/05/18  17:54:23  kao
 * CSCdi54117:  vip2 looses it with serial encaps other than HDLC
 * cbus stall handling plus two s/w work-around for CYA h/w bugs
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/04/12  17:44:07  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/06  21:56:26  ondiaye
 * CSCdi52394:  svip support for merged qe codes header file
 *              Also takes care of CSCdi52731 and the DRQ dealock.
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/04  19:45:37  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:20  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.3  1996/03/07  17:29:38  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/16  22:33:55  getchell
 * CSCdi49237:  svip kernel and asm files out of whack
 * cache invalidate before reading instead of after
 * add some accounting useful for debugging
 * change function definitions for drq/dtq consumer (no longer static)
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:21:55  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:34  getchell
 * Placeholder files for Viper development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_H__
#define __SVIP_H__

#define PA_FAST            	        1
#define PA_SLOW            	        0
#define CSR2_TURBO_MODE                 0x4

/*
 * Particle sizes
 */
#define VIP_PARTICLE_BYTES	vsys_vec.vip_particle_size

#define MIN_GLOBAL_PARTICLES    16
#define MAX_GLOBAL_PARTICLES    16
#define PERM_GLOBAL_PARTICLES   16

/*
 * Port Adaptor Bays
 */
#define MAX_PA_BAYS		2     	/* # of PA's on Checkers Core Board */
#define PA0             	0     	/* Port Adaptor 0 */
#define PA1             	1      	/* Port Adaptor 1 */
#define MAX_INTERFACES_PER_PA  	8 	/* no of interfaces per PA */
#define MAX_VIP_INTERFACES      MAX_INTERFACES_PER_PA * MAX_PA_BAYS

#define INTERFACE_PA_SHIFT	3	/* shift value for flat numbering */

#define IPC_INTERFACE_NUMBER	MAX_PA_BAYS * MAX_INTERFACES_PER_PA
 
#define FLAT_INTERFACE_NUMBER(dcbptr) \
        (((dcbptr)->port<<INTERFACE_PA_SHIFT) | (dcbptr)->interface)

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
 * Fine granularity scheduler interval processing constants
 */
#define NETINT_MASK_MIN         100     /* min usecs with net ints off */
#define NETINT_MASK_DEFAULT     200     /* default usecs with net ints off */
#define NETINT_MASK_MAX         4000    /* max usecs with net ints off */
#define NETINT_MIN              400     /* min usecs in net io handler */
#define NETINT_DEFAULT          4000    /* default usecs in net io handler */
#define NETINT_MAX              60000   /* max usecs in net io handler */
 
#define NETINT_THROTTLE_DEFAULT TRUE    /* default state of throttling */

#define NETWORK_SW_INTERRUPT 0x200

/*
 * Cbus Stall timers
 */
#define BRIEF_BUS_STALL_TIME  500      /* 0.5 milliseconds */


#ifndef ASMINCLUDE

#define HIT_INVALIDATE_DATA             "17"
#define HIT_WRITEBACK_INVALIDATE_DATA   "21"
#define CREATE_DIRTY_EXCLUSIVE_DATA     "13"

static inline
void hit_invalidate_cache_line_inline (void *ptr)
{
    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)" :
	/* no output */ : "r" (ptr));

    asm volatile(".set mips2");
    asm volatile(".set reorder");
}

/*
 * Debug macro to enable more effective logic analyzer
 * debugging. Write codes to DRAM uncached.
 */
/*
#define DAS_DEBUG(a) ((*(volatile ulong *)(R4K_K1BASE + 0x24C)) = (ulong)(a)) 
#define STATISTICS(a) (a)
*/
#define DAS_DEBUG(a)
#define STATISTICS(a)

#define SVIP_DEBUG_SUPPORT            FALSE
#define MEMD_POSTED_READ_SUPPORT      TRUE

/*
 * Host selection - Viper only runs on RSP
 */
#define RPSP_SELECT                   FALSE

/*
 * SVIP does not deal with traffic shaping
 */
#define TRAFFIC_SHAPING(pak, shape)   FALSE

/*
 * Prototypes
 */
extern void     vip_context_dump(int, int, void *);

/* src-vip/vip_dbus.c */
extern void	vip_init_dbus_parser(void);

/* src-svip/svip.c */
typedef void (*vector)(ulong);
extern void	svip_reload(int sig, int code, void *context);
extern ulong	svip_reset_io(void);
extern void     svip_netint_throttle_init(void);
extern void     sched_alloc_command(parseinfo *);
extern void     vip_netint_throttle_show(void);
extern void	install_cbus_handler(void);
extern void	install_cbus_stall_handler(void);
extern void     install_svip_error_handler(void);
extern void     get_pa_conf_type(void);
extern boolean  is_channelized(ushort);

/* src-svip/svip_dma.c */
extern void	install_dma_handler(void);
extern boolean  dmaq_consumer(void);
extern boolean 	vip_ipc_tx_poll(void);

/* asm-mips/asm_4k_svip.S */
extern boolean 	pending_cbus_dbus_intrpt(void);

#endif /* ASMINCLUDE */

#endif /* __SVIP_H__ */
