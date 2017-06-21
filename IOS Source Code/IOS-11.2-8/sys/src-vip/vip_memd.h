/* $Id: vip_memd.h,v 3.8.4.10 1996/07/17 01:45:28 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_memd.h,v $
 *------------------------------------------------------------------
 * vip_memd.h - VIP memd support defines
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_memd.h,v $
 * Revision 3.8.4.10  1996/07/17  01:45:28  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.8.4.9  1996/06/03  18:04:53  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.8.4.8  1996/05/26  00:17:47  hwang
 * CSCdi58206:  srb testing locks up svip
 * make vip_message2buffer available to functions outside vip_memd.c
 * Branch: California_branch
 *
 * Revision 3.8.4.7  1996/04/23  03:10:44  kao
 * CSCdi53527:  Need EOIR support on VIP2 [fix one compiler bug in one
 * image
 *              whose compiler has more type checking than others]
 *
 * Branch: California_branch
 *
 * Revision 3.8.4.6  1996/04/20  17:32:24  getchell
 * CSCdi54568:  FE/VIP2 perf. is severely impacted by presence of second
 * FE.
 * Branch: California_branch
 *
 * Revision 3.8.4.5  1996/04/12  17:44:45  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.8.4.4  1996/04/09  04:11:24  ondiaye
 * CSCdi52394:  svip support for merged qe codes header file
 * Branch: California_branch
 *
 * Revision 3.8.4.3  1996/04/08  13:45:06  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.8.4.2  1996/04/04  19:45:33  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.8.4.1  1996/03/21  23:52:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.7  1996/03/18  23:36:50  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.6  1996/03/07  17:30:08  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/21  20:38:09  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Also integrated patch from dlobete that allows dfs across mic rel
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/02/10  00:22:37  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/02/08  09:31:18  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/16  02:36:21  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/19  23:32:46  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.8  1996/03/09  03:18:42  kao
 * CSCdi51172:  RVIP crashes when PA bay 0 is empty
 *
 * Revision 3.7  1996/03/04  07:14:42  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.6  1996/02/18  19:42:47  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.5  1996/02/14  02:56:00  hwang
 * CSCdi48861:  rvip malu command queue full
 * Enlarge malu command queue to 256 element.
 *
 * Revision 3.4  1996/01/19  03:03:38  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.3  1995/12/26  23:11:42  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.2  1995/11/17  18:52:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VIP_MEMD_H__
#define __VIP_MEMD_H__

#if !defined(ASMINCLUDE)

/*
 * typedefs
 */
typedef struct vip_memd_ifcntl_t_ vip_memd_ifcntl_t;
typedef volatile struct vip_bhdr_t_ vip_bhdr_t;

/*
 * Include RP/SP and RSP support header files but no static inlines
 */
#include "rpsp_memd.h"
#include "rsp_memd.h"

#endif /* ASMINCLUDE */

/*
 * Usefull definitions
 */
#define VIP_BUFFERIN_OFFSET	0x96	/* location of SP bufferin word */
#define VIP_SP_BUFFERIN_SUPPORT	0x8100	/* code for bufferin support */

#define BUFFER_HEADER_SPACE 	16384 	/* Four pages in 16 bit words */
#define MALU_ATTN_CMD_DEPTH	2048	/* 2048 posted commands */
#define CCB_LENGTH 		12    	/* byte length of CCB */

#define BH_PARK_LIMIT	3		/* max park limit */

/*
 * VIP interface states
 */
#define VIP_DISABLE   		0
#define VIP_ENABLE    		1
#define VIP_QUIESCE   		2
#define VIP_UNQUIESCE 		3

/*
 * VIP control flags for the entire VIP in vip_control_flags
 */
#define VIP_QUIESCE_FLAG	0x1

#if !defined(ASMINCLUDE)

/*
 * Note: Currently there is no mechanism for VIP to know the size of
 *       MEMD in RP/RSP (stored in memd_size). VIP hardcodes MEMD size to
 *       2 MB. When MEMD size is changed, change ADRSPC_CBUS_SIZE accordingly.
 *
 * Check whether it is within 2 MB MEMD address range starting ADRSPC_CBUS
 * although the theoretical size of MEMD is 0x30000000 to 0x31FFFFFF
 * the real MEMD size will be still 2 MB for as far as we know
 * This macro is used to check whether a read bus error is caused
 * by accessing "valid" MEMD address. If yes, it is assumed that
 * CBUS stall is in progress and no exception handling is performed.
 */
#define ADRSPC_CBUS_SIZE        0x200000        /* memd address size */
#define MEMD_CBUS_MASK          (~(ADRSPC_CBUS_SIZE-1)) /* MEMD address mask */

static inline boolean is_memd_addr (int addr)
{
    return((addr & MEMD_CBUS_MASK) == ADRSPC_CBUS);
}
#endif /* ASMINCLUDE */

#if defined(ASMINCLUDE)

/*
 * Note: Keep the definitions for the asm code consistant with
 * the typedefs for C code.
 */
#define MEMD_CNTL_CCB		0	/* Offset to ccbptr */
#define MEMD_CNTL_SELECT	4	/* Offset to selected_interface */
#define MEMD_CNTL_MEMD_ADDR	8	/* Offset to memd_pointer */
#define MEMD_CNTL_LAST_SYNC	12	/* Sync magic number for ccb cmds */
#define MEMD_CNTL_SWVER		16	/* Offset to sw_ver_done */
#define MEMD_CNTL_VPLDVER       20      /* Offset to vpld version | done */
#define MEMD_CNTL_ROMVER        24      /* Offset to rom version | done */
#define MEMD_CNTL_NOQERR	28	/* Offset to no_qerr_count */
#define MEMD_CNTL_SYNC_COUNT	32	/* Offset to sync_count */
#define MEMD_CNTL_MAX_LV_LTR	36	/* Offset to max_lv_ltr_done */
#define MEMD_CNTL_PA_INT_MASK   40      /* PA interrupt mask */

#define MEMD_CNTL_PASCB_TABLE	44	/* Offset to pascb table */

#define MEMD_CNTL_IFTYPE	\
    (MEMD_CNTL_PASCB_TABLE + (MAX_PA_BAYS * MAX_INTERFACES_PER_PA * 4))

#define MEMD_CNTL_RXCOUNT	\
    (MEMD_CNTL_IFTYPE + (MAX_PA_BAYS * MAX_INTERFACES_PER_PA * 4))

#define MEMD_CNTL_IFCARRIER	\
    (MEMD_CNTL_RXCOUNT + (MAX_PA_BAYS * MAX_INTERFACES_PER_PA * 4))

#define MEMD_CMD_PASCB		0	/* Offset to pascb */
#define MEMD_CMD_CMDARG0	4	/* Offset to cmd_arg0 */
#define MEMD_CMD_ARG1ARG2	8	/* Offset to args 1 and 2 */
#define MEMD_CMD_ARG3		12	/* Offset to arg3 */
#define MEMD_CMD_DRIVER_CMD	14	/* Offset to driver_cmd */

#define MEMD_CMDQ_READ		0	/* Offset to read */
#define MEMD_CMDQ_WRITE		4	/* Offset to write */
#define MEMD_CMDQ_CMDQ		8	/* Offset to cmd_q */
 
#else 	/* C source code */

/*
 * Note: Keep the definitions for the asm code consistant with
 * the typedef for C code.
 */
typedef struct memd_cntl_t_ {

    ccbtype *ccbptr;			/* pointer to ccb in memd */
    ulong   selected_interface;		/* presently selected interface */
    ulong   memd_pointer;		/* memd pointer (in bytes) */
    ulong   last_sync_value;		/* ccb command sync value */
    ulong   sw_ver_done;		/* sw version (plus done) */
    ulong   vpld_ver_done;		/* vpld version (plus done) */
    ulong   rom_ver_done;		/* rom version (plus done) */
    ulong   no_qerr_count;		/* Count of no Q entries */
    ulong   sync_count;			/* number of sync commands */
    ulong   max_lv_ltr_done;		/* max love letter (plus done) */
    ushort  pa_int_mask;                /* PA interrupt mask */
    ushort  unused;

    /*
     * Array of pas_cbtype pointers (indexed by selected_interface)
     */
    pas_cbtype *pascb_table[MAX_PA_BAYS * MAX_INTERFACES_PER_PA];

    /*
     * Array of fcitypes plus done (indexed by selected_interface)
     */
    ulong if_fcitype[MAX_PA_BAYS * MAX_INTERFACES_PER_PA];

    /*
     * Array of local free queue buffer counters
     */
    ulong rx_count[MAX_PA_BAYS * MAX_INTERFACES_PER_PA];

    /*
     * Array of carrier state info plus done
     */
    ulong if_carrier[MAX_PA_BAYS * MAX_INTERFACES_PER_PA];

    /*
     * Informational variables
     */
    ulong queue_checked;		/* number of times Q was checked */
    ulong processed;			/* total queued commands proc. */
    ulong errors;			/* total errors */
    ulong sys_commands;			/* system commands */
    ulong driver_commands;		/* driver commands */

} memd_cntl_t;

/*
 * Define the malu attention command structure and queue
 */
typedef struct malu_cmd_t_ {
    pas_cbtype *pascb;
    ushort  cmd;
    ushort  arg0;
    ushort  arg1;
    ushort  arg2;
    ushort  arg3;
    ushort  driver_cmd;
} malu_cmd_t;

typedef struct malu_q_t_ {
    volatile int read;
    volatile int write;
    malu_cmd_t cmd_q[MALU_ATTN_CMD_DEPTH];
} malu_q_t;

/*
 * Malu attention dispatch vector
 */
typedef void (*cmd_dispatch_t)(malu_cmd_t *cmd);

/*
 * MEMD buffer header type 
 */
struct vip_bhdr_t_ {

    /*
     * ulong 0 : RP/SP	: ushort 0 - link
     *			: ushort 1 - rxbytes_txicb
     *
     *		 RSP	: ushort 0 - rxstatus
     *		  	: ushort 1 - rxbytes
     */
    union {
        struct {
            ushort short0;
            ushort short1;
        } sw01;
        ulong w0;
    } u0;

    /*
     * ulong 1 : RP/SP	: ushort 0 - rxsts_txstart
     *			: ushort 1 - tx_bytes
     *
     *		 RSP	: ushort 0 - rxstatus2
     *		  	: ushort 1 - queptr
     */
    union {
        struct {
            ushort short0;
            ushort short1;
        } sw23;
        ulong w1;
    }u1;

    /*
     * ulong 2 : RP/SP	: ushort 0 - rxicb
     *			: ushort 1 - rifsts
     *
     *		 RSP	: ushort 0 - reserved
     *		  	: ushort 1 - bcastid
     */
    union {
        struct {
            ushort short0;
            ushort short1;
        } sw45;
        ulong w2;
    } u2;

    /*
     * ulong 3 : RP/SP	: ushort 0 - rxicb
     *			: ushort 1 - rifsts
     *
     *		 RSP	: ushort 0/1 - bufptr
     */
    union {
        struct {
            ushort short0;
            ushort short1;
        } sw67;
        ulong w3;
    } u3;
};

/*
 * Buffer header, ulong 0
 */
#define bf_rpsp_link		u0.sw01.short0
#define bf_rsp_rxstatus		u0.sw01.short0
#define bf_rsp_evnt_code	u0.sw01.short0

#define bf_rpsp_txicb		u0.sw01.short1
#define bf_rpsp_rxbytes		u0.sw01.short1
#define bf_rsp_rxbytes		u0.sw01.short1
#define bf_rsp_evnt_id		u0.sw01.short1

#define bf_word0		u0.w0
#define bf_rsp_txptr		u0.w0
#define bf_rsp_evnt_code_and_id	u0.w0

/*
 * Buffer header, ulong 1
 */
#define bf_rpsp_txstart      	u1.sw23.short0
#define bf_rpsp_rxsts        	u1.sw23.short0
#define bf_rsp_rxstatus2	u1.sw23.short0
#define bf_rsp_txbytes		u1.sw23.short0
#define bf_rsp_evnt_len		u1.sw23.short0

#define bf_rpsp_txbytes      	u1.sw23.short1
#define bf_rsp_freeq		u1.sw23.short1
#define bf_rsp_qptr		u1.sw23.short1
#define bf_rsp_evnt_fq		u1.sw23.short1

#define bf_word1        	u1.w1
#define bf_rsp_evnt_len_and_fq	u1.w1
#define bf_rsp_txlen_and_fq	u1.w1

/*
 * Buffer header, ulong 2
 */
#define bf_rpsp_rxicb		u2.sw45.short0

#define bf_rpsp_rifsts		u2.sw45.short1
#define bf_rsp_bcastid		u2.sw45.short1

#define bf_word2		u2.w2

/*
 * Buffer header, ulong 3
 */
#define bf_rpsp_pointer		u3.sw67.short0
#define bf_rpsp_page		u3.sw67.short1

#define bf_rpsp_ptr_page	u3.w3
#define bf_rsp_bufptr		u3.w3
#define bf_rsp_evnt_buf		u3.w3
#define bf_word3		u3.w3

/*
 * vip_rpsp_icb_t:
 * Typedef for interface control block in MEMD on an RP/SP system
 */
typedef volatile struct vip_rpsp_icb_t_ {

    union {
        struct {
            ushort type;
            ushort freeq;
        } sw01;
        ulong w0;
    } u0;

    union {
        struct {
            ushort txque;
            ushort reserved0;
        } sw23;
        ulong w1;
    } u1;

    union {
        struct {
            ushort reserved1;
            ushort ipcque;
        } sw45;
        ulong w2;
    } u2;

} vip_rpsp_icb_t;
 
#define icb_rpsp_type          u0.sw01.type
#define icb_rpsp_freeq         u0.sw01.freeq
#define icb_rpsp_txque         u1.sw23.txque
#define icb_rpsp_txque_word    u1.w1
#define icb_rpsp_reserved0     u1.sw23.reserved0
#define icb_rpsp_ipcque        u2.sw45.ipcque

/*
 * VIP system vector type
 */
typedef struct vsys_vector_t_ {

    int		vip_slot;		/* slot that the vip is installed */
    boolean	bufferin_enabled_sp;	/* TRUE if SP running bufferin */

    boolean 	rpsp_host;		/* in an RP/SP system */
    boolean     viper;                  /* software running on viper */

    /*
     * Global fields for RP/SP memd control
     */
    ushort		rpsp_rx_last_length;

    vip_bhdr_t		*rpsp_rx_qtail;		/* tx_qtail */
    vip_bhdr_t		*rpsp_rx_qhead;		/* rx_qhead */
    vip_bhdr_t		*rpsp_tx_qtail;		/* tx_qtail */
    vip_bhdr_t		*rpsp_tx_qhead;		/* tx_qhead */

    volatile ulong	*rpsp_q_rethead_tail;	/* cxq->q_retque */
    volatile ushort	*rpsp_q_rettail;	/* cxq->q_rettail */

    volatile ushort	*rpsp_q_rawtail;    	/* cxq->q_rawtail */
    volatile ulong 	*rpsp_q_rawhead_tail;	/* cxq->q_rawque */

    /*
     * Global fields for RSP memd control
     */
    volatile ulong	*rsp_global_raw_q;	/* raw_q */
    volatile ulong	*rsp_event_response_q;	/* ev_resp_q */
    volatile ulong	*rsp_event_free_q;	/* ev_free_q */
    volatile ulong	*rsp_loveletter_free_q;	/* ltr_free_q */
    volatile ulong	*rsp_ipc_free_q;	/* ipc_free_q */
    volatile ulong	*rsp_ipc_rx_q;		/* ipc_rx_q */
    volatile ulong	*rsp_ipc_tx_q;		/* ipc_tx_q */

    /*
     * Interface command dispatch vector, for malu attention
     */
    cmd_dispatch_t 	cmd_dispatch[MAX_VIP_INTERFACES];

    /*
     * Lovenote / Loveletter accounting
     */
    ulong		lovenote_count;
    ulong		loveletter_count;

    /*
     * Pointer to ipc pascb structure 
     */
    pas_cbtype         *ipc_pascb;

    /* 
     * Bus stall error statistics
     */
     int                bus_stall_read_err;
     int                bus_stall_write_err;
     int                bus_stall;

    /*
     * vip_memd_ifcntrl pointer to interface associated with current
     * posted read operation using posted read register one
     */
    vip_memd_ifcntl_t  *pr1_ifc;

    /*
     * accumulator number associated with current
     * posted read operation using posted read register two
     */
    ulong               pr2_tx_acc;  
    /*
     * particle size chosen dynamically on VIP2
     */
    ulong               vip_particle_size;

    /*
     * PA config_type: 1= two fast(fe,fddi,hssi) PA's, 0 else
     */
    ushort pa_conf_type;

} vsys_vector_t;

/*
 * VIP memd interface control
 */
struct vip_memd_ifcntl_t_ {

    /*
     * Hardware IDB pointer
     */
    hwidbtype		*idb;

    /*
     * Interface state control
     */
    ushort		vip_if_status;
    ushort		old_vip_if_status;
    ulong		rx_offset;

    /*
     * Buffer header stack
     */
    ulong 		bh_stack_top;
    vip_bhdr_t		*(bh_stack[BH_PARK_LIMIT]);

    /*
     * MEMD free queue limits and pointer to rx_count accumulator found in
     * memd_cntl 
     */
    ulong              rx_limits_high;
    ulong              rx_limits_low;
    ulong              *rx_count_ptr;

    /*
     * RP/SP memd buffer queues
     */
    vip_bhdr_t	   	*rpsp_local_free_q;   	/* freeq_array */
    vip_bhdr_t	    	*rpsp_local_freeq_head;	/* localhead_array */
    vip_bhdr_t	    	*rpsp_scraped_txq;	/* scraped_tx_que */
    vip_bhdr_t	    	*rpsp_loveq;		/* loveq_array */

    /*
     * RP/SP icb pointers
     */
    vip_rpsp_icb_t	*icb_ptr;		/* CPU format */
    ushort		icb_ptr_memd;		/* MEMD format */

    /*
     * RSP memd buffer queues
     */
    volatile ulong	*rsp_local_free_q;	/* lfree_q */
    volatile ulong	*rsp_global_free_q;	/* gfree_q */
    volatile ulong	*rsp_tx_q;		/* tx_q */
    volatile short	*rsp_tx_acc;		/* tx_acc */

    ushort              status_word_bit_mask;   /* RSP style status */
    ushort              status_word_register;   /* Viper specific */

    boolean             memd_poll_suspend_queued;   /* MEMD polling */
};

/*
 * Definitions
 */
#define MAX_BUFFER_HEADERS		4096
#define MEMD_BHDR_ADDR_MASK             0xffff
#define MEMD_ADDR_MASK                  0x1fffff      /* 2 Mbytes */

/*
 * Externals
 */
extern uchar 			*(vip_bhcache[MAX_BUFFER_HEADERS]);
extern volatile ushort  	*(vip_rsp_txcached_q[MAX_BUFFER_HEADERS]);
extern vip_rpsp_icb_t		*(vip_rpsp_icb_array[MAX_VIP_INTERFACES+1]);
extern vip_memd_ifcntl_t	vip_ifcntl_array[MAX_VIP_INTERFACES+1];
extern vsys_vector_t 		vsys_vec;
extern memd_cntl_t 		memd_cntl;
extern void                     init_ifc_queues_caches(vip_memd_ifcntl_t *ifc);
extern int                      vip_control_flags;


/*
 * Prototypes
 */

/* src-vip/vip_memd.c */

extern void     r4k_update_timestamp(int ms_loss);
extern boolean  check_clock_loss(void);
extern void     vip_init_memd_support(boolean is_viper);

extern pas_cbtype *get_one_exist_pascb(void);
extern void	vip_init_memd_support(boolean);
extern void	vip_init_ipc(void);
extern void 	vip_kernel_ready(void);
extern void     vip_unsupported_cmd(malu_cmd_t *cmd);
extern void	vip_send_interface_stats(hwidbtype *, malu_cmd_t *);
extern void	vip_control_interface(hwidbtype *, malu_cmd_t *);
extern void	vip_interface_init(hwidbtype *);
extern void	install_cmd_dispatch(hwidbtype *, cmd_dispatch_t);
extern void	vip_lineproto_change(hwidbtype *, boolean);
extern void	vip_memd_return_periodic(void);
extern void	vip_network_chores(void);
extern void	vip_dmaq_consumer(void);
extern void	vip_malu_attn_queue_check(void);
extern boolean	vip_send_ipc_message(pas_cbtype *, uchar *, ushort);
extern boolean 	vip_send_loveletter(pas_cbtype *pascb, uchar *buf, 
				    ushort bytes);
extern paktype  *vip_message2buffer (hwidbtype *, uchar *, ushort);

/* src-vip/rpsp_memd.c */

extern vip_rpsp_icb_t	*vip_rpsp_icb_address(pas_cbtype *);

/* src-rvip/rvip_dma.c */

extern void		vip_flush_dmaq(void);
extern boolean		vip_poll_tx_queues(void);

/* platform_ [..].c */

extern boolean  	vip_set_tx_pointers(malu_cmd_t *);
extern boolean          vip_set_ipc_queues(malu_cmd_t *);
extern mempool		*vip_dma_buffer_mempool(void);
extern ushort		vip_sw_version(void);
extern ushort		vip_vpld_version(void);
extern ushort		vip_rom_version(void);

/* src-[sr]vip/[sr]vip*.c */
extern void             vip_quiesce_all_interfaces(void);
extern void             vip_unquiesce_all_interfaces(void);
extern void             vip_quiesce_one_interface(pas_cbtype *pascb);
extern void             vip_unquiesce_one_interface(pas_cbtype *pascb);
extern boolean		vip_pak_dfs(hwidbtype *, paktype *);
extern boolean		vip_pak_to_host(hwidbtype *, pas_cbtype *, paktype *);
extern boolean		vip_pak_switch(vip_memd_ifcntl_t *, paktype *,
					vip_bhdr_t *, volatile ulong *);



/* asm-mips/asm_4k_vip_cbus.S */

extern boolean  	vip_probe_memd(void);
extern boolean 		vip_memdread8(volatile uchar *, uchar *);
extern boolean		vip_memdread16(volatile ushort *, ushort *);
extern boolean  	vip_memdread32(volatile ulong *, ulong *);
extern vip_bhdr_t	*vip_rpsp_memd_readbh(volatile ushort *);
extern vip_bhdr_t	*vip_rsp_memd_readbh(volatile ulong *);
extern void		start_memd_read_handlers(void);
extern void		end_memd_read_handlers(void);


/**********************************************************************
 * Static inlines
 **********************************************************************/

/*
 * Include RP/SP and RSP support header files but this time 
 * pick up only the static inlines
 */
#define VIP_MEMD_STATIC_INLINES

#include "rpsp_memd.h"
#include "rsp_memd.h"

#undef VIP_MEMD_STATIC_INLINES

static inline boolean is_cbus_quiesced (void)
{
    return(vip_control_flags & VIP_QUIESCE_FLAG);
}

static inline void set_cbus_quiesced (void)
{
    vip_control_flags |= VIP_QUIESCE_FLAG;
}

static inline void clr_cbus_quiesced (void)
{
    vip_control_flags &= ~VIP_QUIESCE_FLAG;
}

static inline void vip_pak_vector_init (pas_cbtype *pascb)
{
    if (!RPSP_SELECT)
	pascb->pak_switch = (pak_switch_t)vip_pak_switch;
    pascb->pak_dfs = vip_pak_dfs;
    pascb->pak_to_host = vip_pak_to_host;
}

/*
 * vip_dequeue_bh_stack:
 * Try and dequeue a bh from the private buffer header stack
 * for the interface.
 */
static inline vip_bhdr_t *vip_dequeue_bh_stack (vip_memd_ifcntl_t *ifc)
{
    vip_bhdr_t *bh;

    if (ifc->bh_stack_top <= 0) {
	return(NULL);
    }
    ifc->bh_stack_top -= 1;
    bh = ifc->bh_stack[ifc->bh_stack_top];
    return(bh);
}

/*
 * vip_enqueue_bh_stack:
 * enqueue a bh onto the private buffer header stack for the interface.
 */
static inline void vip_enqueue_bh_stack (vip_memd_ifcntl_t *ifc, 
					 vip_bhdr_t *bhptr)
{
    ifc->bh_stack[(ifc->bh_stack_top++)] = bhptr;
}

/*
 * vip_empty_bh_stack:
 * Return TRUE if the buffer header stack is empty.
 */
static inline boolean vip_empty_bh_stack (vip_memd_ifcntl_t *ifc)
{
    return (ifc->bh_stack_top <= 0);
}

/*
 * vip_rxbh_setup:
 * Setup a receive buffer header
 */
static inline void vip_rxbh_setup (vip_memd_ifcntl_t *ifc, 
				   vip_bhdr_t *bh, 
				   paktype *pak)
{
    struct {
	ulong upper;
	ulong lower;
    } split64;

    if (RPSP_SELECT) {
	if (pak->mci_status) {
	    bh->bf_word1 = (pak->mci_status << 16);
	}

#if 0	/* FIXME : How is this going to work ?? */
	if (pak->rif_status) {
	    bh->bf_rpsp_rifsts = pak->rif_status;
	}
#endif

    } else {
	split64.upper = (pak->mci_status << 16) | pak->datagramsize;
	split64.lower = RSP_VIRTUAL2MEMD(ifc->rsp_local_free_q) |
                        pak->rif_status;
	move64((u64*)&split64, (u64*)bh);
    }
}

#if defined(SVIP)
#include "../src-svip/svip_memd.h"
#endif
#if defined(RVIP)
#include "../src-rvip/rvip_memd.h"
#endif

/*
 * vip_dfs_bh_allocate :
 * Allocate a buffer header for a DFS packet. This routine returns
 * a VIP addressing format buffer header pointer from the local
 * freeq of the inbound interface or from the global freeq if the local
 * queue is empty and we are allowed take one from the global freeq.
 * Note: length is provided in case we want to do best-fit.
 *
 */
static inline vip_bhdr_t *vip_dfs_bh_allocate (pas_cbtype *pascb, int length)
{
    return(vip_allocate_memd_bh(pascb));
}

/*
 * This is the first step towards an equivalent to the lowend
 * fastsend vector.  We're not there yet due to the fact that
 * we need the input idb in order to allocate memd buffers.
 * That requirement is really stupid, though, since we should be
 * allocating global first-fit buffers, not using input buffers.
 *
 * By name, this inline belongs in rsp_memd.h, but it was impossible 
 * to put it there due to ordering constraints (needs to call routines
 * in vip_memd.h but rsp_memd.h must come first)
 */
static inline boolean vip_rsp_memd_fastsend (void *tx_q_ptr, 
					     paktype *pak,
					     hwidbtype *input,
					     vip_bhdr_t *bhptr)
{
    pas_cbtype *pascb = INSTANCE(input);
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    particletype *mp = pak->particlequeue.qhead;
    struct {
	ulong upper;
	ulong lower;
    } split64;

    mp->data_start = pak->datagramstart;

    /*
     * This is the equivalent of vip_rxbh_setup for normal
     * rx path. 
     */
    split64.upper = (ulong)vip_rsp_lookup_bhcache(bhptr) & 0xFFFFFF;
    split64.lower = (pak->datagramsize << 16) |
                           RSP_VIRTUAL2MEMD(ifc->rsp_local_free_q);
    move64((u64*)&split64, (u64*)bhptr);
    
    return(pascb->pak_switch(ifc, pak, bhptr, (ulong *)tx_q_ptr));
}
#endif /* ASMINCLUDE */

#endif /* !__VIP_MEMD_H__ */

/* end of file */
