/* $Id: if_pas_mif68840.h,v 3.1.60.9 1996/07/17 01:45:05 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_mif68840.h,v $
 *------------------------------------------------------------------
 * if_pas_mif68840.h - 1F MIF68840 based PCI port adaptor support
 *                  (Platform independent components)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_mif68840.h,v $
 * Revision 3.1.60.9  1996/07/17  01:45:05  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.1.60.8  1996/06/07  06:59:11  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.1.60.7  1996/05/31  01:26:22  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.1.60.6  1996/05/19  03:18:41  fsunaval
 * CSCdi55239:  FDDI driver needs to correctly mimic AF status bits
 * settings
 * Branch: California_branch
 *
 * Revision 3.1.60.5  1996/04/30  07:01:09  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.1.60.4  1996/04/11  04:39:43  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.60.3  1996/03/28  03:33:28  fsunaval
 * CSCdi50796:  SW-VIP1: Transparent Bridge not taking fast-switching
 * Branch: California_branch
 *
 * Revision 3.1.60.2  1996/03/28  02:49:23  fsunaval
 * CSCdi51945:  SW-VIP1: fddi pa not showing neigbor mac-address.
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/03/22  19:49:59  fsunaval
 * CSCdi50796:  SW-VIP1: Transparent Bridge not taking fast-switching
 * Branch: ELC_branch
 *
 * Revision 3.1.4.7  1996/03/09  22:14:39  fsunaval
 * CSCdi50760:  Particle-based Appletalk fast-switching broken on Fddi.
 * Branch: ELC_branch
 *
 * Revision 3.1.4.6  1996/02/26  04:39:22  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.1.4.5  1996/02/17  05:54:33  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.1.4.4  1996/02/05  07:00:12  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.3  1996/01/25  12:10:08  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.2  1995/12/26  19:56:44  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:27  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:57:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:27  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_PAS_MIF68840_H__
#define __IF_PAS_MIF68840_H__

/*
 * Define rx/tx ring entries. Must be a power of 2!!
 */
#define MIF68840_RX_LLC_RING_ENTRIES        128
#define MIF68840_RX_SMT_RING_ENTRIES         32 
#define MIF68840_TX_RING_ENTRIES            256 

/*
 * Define rx ring attribute
 */
#define MIF68840_RX_RING_ATTRIBUTE TLB_UNCACHED

/*
 * Define the minibuffer size used by MIF68840
 */
#define MIF68840_PARTICLE_SIZE              512 

/*
 * Define Tx/Rx interrupt loop limits
 */
#define MIF68840_RX_MAX_SPIN                MIF68840_RX_LLC_RING_ENTRIES
#define MIF68840_TX_MAX_SPIN                MIF68840_TX_RING_ENTRIES

/*
 * Define address conversion macro's
 */
#define MIF68840_BUF_ADDR(x)        ((ulong)(x))
#define MIF68840_CNTL_ADDR(x)       ((ulong)(x))


#define FCR_IS_FREE                                                 \
while (!(ds->mif68840_regaddr->ifddi[SR1] & CONTROL_REG_FREE)) {    \
    usecdelay(1);                                                   \
    continue;                                                       \
}                                                                   \

#define CMR_IS_FREE                                                 \
while (!(ds->mif68840_regaddr->ifddi[SR1] & COMMAND_DONE)) {        \
    usecdelay(1);                                                   \
    continue;                                                       \
}                                                                   \

/*
 * PCI PLX9060ES defines
 */
#define     MIF68840_DEVICE_VENDOR_ID           0x906e10b5

/*
 * Defines for PCI (PLX9060ES) configuration register address offsets
 */
#define    MIF68840_PCI_CFID_REG_OFFSET        0x00
#define    MIF68840_PCI_CFCS_REG_OFFSET        0x04
#define             MIF68840_CFCS_IO_SPACE_ACCESS       0x0001
#define             MIF68840_CFCS_MEM_SPACE_ACCESS      0x0002
#define             MIF68840_CFCS_MASTER_OPERATION      0x0004
#define    MIF68840_PCI_CFRV_REG_OFFSET        0x08
#define    MIF68840_PCI_CFLT_REG_OFFSET        0x0C
#define             MIF68840_CFLT_LATENCY_TIMER         0x0000FF00
#define    MIF68840_PCI_CBMEM_REG_OFFSET           0x10
#define    MIF68840_PCI_CBIO_REG_OFFSET        0x14
#define    MIF68840_PCI_CBLOCAL_REG_OFFSET     0x18
#define    MIF68840_PCI_CFIT_REG_OFFSET        0x3C



/* PLX9060ES PCI register definition */
/* Not used */
typedef struct plx9060_pci_regs_t {
    volatile ulong cfid;
    volatile ulong cfcs; 
    volatile ulong cfrv;
    volatile ulong cflt; 
    volatile ulong cbmem;
    volatile ulong cbio;
    volatile ulong cblocal; 
    volatile ulong reserved[8];
    volatile ulong cfit;  
} plx9060_pci_regs;

/* PLX9060ES Local configuration register definitions */
typedef struct plx9060_local_regs_t {
    volatile ulong range_pci_local;
    volatile ulong remap_pci_local;
    volatile ulong local_arb_reg;
    volatile ulong big_little_endian_reg;
    volatile ulong range_pci_rom;
    volatile ulong remap_pci_rom;
    volatile ulong bus_descriptors_pci_local;
    volatile ulong range_local_pci;
    volatile ulong cb_local_pci; 
    volatile ulong cb_local_pci_io; 
    volatile ulong remap_local_pci;
    volatile ulong cfg_addr_reg_local_pci_io;
    volatile uchar reserved[0x38];
    volatile ulong interrupt_control_status;
    volatile ulong eeprom_control;
} plx9060_local_regs;

/* MIF68840's register definition */
typedef struct mif68840_regs_t {

    volatile uchar  pad1[0x400];        /*0x0 - 0x3ff*/
    volatile ulong  csr;                /*0x400 - 0x403 
                                         *short in reality
                                         */

    volatile uchar pad2[0x3fc];         /*0x404 - 0x7ff*/
    volatile ulong cam[0xa];            /*0x800 - 0x827
                                         *short in reality
                                         */

    volatile uchar pad3[0x7d8];         /*0x828 - 0xfff*/
    volatile ulong ifddi[0x10];         /*0x1000 - 0x103f*/

    volatile uchar pad4[0x3c0];         /*0x1040 - 0x13ff*/
    volatile ulong elmb[0x20];          /*0x1400 - 0x147f
                                         *short in reality
                                         */

    volatile uchar pad5[0x80];          /*0x1480 - 0x14ff*/
    volatile ulong mac[0x35];           /*0x1500 - 0x15d3
                                         *short in reality
                                         */

    volatile uchar pad6[0x2c];          /*0x15d2 - 0x15ff*/
    volatile ulong camel[0x15];         /*0x1600 - 0x1653
                                         *short in reality
                                         */

    volatile uchar pad7[0x1ac];         /*0x1652 - 0x17ff*/
    volatile ulong elma[0x1b];          /*0x1800 - 0x186b*/

} mif68840_regs;


typedef struct mif68840_rx_desc_t {
    volatile ulong rx_status_length;
    volatile ulong rx_buffer_addr;      /* pointer to minibuffer */
} mif68840_rx_desc;

#define MIF68840_RX_EAC_MASK                0x001c0000

#define MIF68840_RX_DA_MASK                 0x0000c000
#define     MIF68840_RX_PROMISCUOUS         0x00004000
#define     MIF68840_RX_CAM_MATCH           0x00008000
#define     MIF68840_RX_LOCAL_MATCH         0x0000c000

/*
 * 0x02 is the address filter result
 * set to a broadcast
 */
#define MIF68840_RX_SMT_FRAME_STATUS        0x000a 

/*
 * OWN Bit=1 and Buffer Length=512 bytes.
 * Default value for all new RX Buffers 
 */
#define MIF68840_RX_STATUS_LENGTH_DEFAULT 0x80000100 

/* Receive frame Normal Indications */
#define MIF68840_RX_OWN     0x80000000 /* Buffer is owned by device (Orion) */
#define MIF68840_RX_FIRST   0x20000000 /* Buffer is first of frame */
#define MIF68840_RX_LAST    0x10000000 /* Buffer is last byte of frame */
#define MIF68840_RX_LENGTH  0x00001fff /* Mask to obtain length of receive frame */

/* definitions for receive errors */
#define MIF68840_RX_ERROR   0x0f000000 /* Received frame has some/any error */

#define MIF68840_RX_PORT    0x08000000 /* Received frame has some port error */
#   define MIF68840_RX_PORT_ERROR_MASK  0x0000000f

#define MIF68840_RX_OVR_PAR 0x04000000 /* Received frame has OVR/PAR error */
#   define MIF68840_RX_OVR  0x02000000 /* Received frame has overrun error */
#   define MIF68840_RX_PAR  0x01000000 /* Received frame has parity error */

#define MIF68840_RX_CRC     0x01000000 /* Received frame has CRC error */

typedef struct mif68840_tx_desc_t {
    volatile ulong tx_status_length;    /* tx_status and length bits */
    volatile ulong tx_buffer_addr;      /* pointer to minibuffer */
} mif68840_tx_desc;

typedef struct mif68840_tx_shadow_t {
    paktype *pak;           /* pointer to pak */
    particletype *mp;       /* pointer to particle */
} mif68840_tx_shadow;

/* 3 bytes before each transmit frame, the first 00 is for PUTLONG  */
#define PREMAC              0x00303800

/* Transmit frame Normal Indications */
#define MIF68840_TX_OWN     0x80000000 /* Buffer is owned by device (Orion) */
#define MIF68840_TX_LAST    0x10000000 /* Last buffer of transmitted frame */

/* Transmit frame control/status information */
#define MIF68840_TX_FIRST_LAST_BUFFER   0xb0000000
#define MIF68840_TX_FIRST_BUFFER        0xa0000000
#define MIF68840_TX_LAST_BUFFER         0x90000000
#define MIF68840_TX_INTERMEDIATE_BUFFER 0x80000000
#define MIF68840_MAGIC_PAK_PTR          ((paktype *)0x03)

/* definitions for transmit errors */
#define MIF68840_TX_ERROR       0x00040000 /* Transmitted frame had been aborted */
#define MIF68840_TX_PORT        0x00380000 /* Transmitted frame had port error */
#define MIF68840_TX_UNDERRUN    0x00020000 /* Transmitted frame had underrun error */
#define MIF68840_TX_PARITY      0x00010000 /* Transmitted frame had parity error */

/* Structure of the ds instantiation */
typedef struct mif68840_instance_t {

    pas_cbtype pacb;            /* Universal PA device control */

    /* Ptr to mif68840 control/status  registers */
    mif68840_regs *mif68840_regaddr;
 
    volatile ulong csr;          /* csr register (type, mode, etc.) */

    /*
     * Receive llc ring information 
     */
    mif68840_rx_desc *rxr_llc_malloc;  /* Rx llc ring pointer,usable for free */
    mif68840_rx_desc *rxr_llc;         /* Rx llc ring pointer */
    ulong rx_llc_head, rx_llc_tail;    /* Indexes into rx llc ring */
    particletype **rxr_llc_shadow;     /* Rx llc ring shadow */
    paktype *rx_pak_llc;               /* Rx temp llc paktype pointer */

    /*
     * Receive smt ring information 
     */
    mif68840_rx_desc *rxr_smt_malloc;  /* Rx smt ring pointer,usable for free */
    mif68840_rx_desc *rxr_smt;         /* Rx smt ring pointer */
    uint rx_smt_head, rx_smt_tail;     /* Indexes into rx smt ring */
    particletype **rxr_smt_shadow;     /* Rx smt ring shadow */
    paktype *rx_pak_smt;               /* Rx temp smt paktype pointer */

    boolean rx_discard;                /* Rx temp flag for discarding frame */

    /*
     * Miscellaneous Rx info
     */
    ulong llc_inputs;           /* # of llc inputs */
    ulong smt_inputs;           /* # of smt inputs */
    ulong llc_mr_drops;         /* # of times llc couldn't get rep. particle */
    ulong llc_bh_drops;         /* # of times llc couldn't get bh in memd */
    ulong smt_mr_drops;         /* # of times smt couldn't get rep. particle */
    ulong smt_bh_drops;         /* # of times smt couldn't get bh in memd */
    ulong rx_no_llc_ep;         /* # of times eof got lost on llc */
    ulong rx_no_smt_ep;         /* # of times eof got lost on smt */
    ulong rx_llc_mis;           /* # of times llc miss happened */
    ulong rx_smt_mis;           /* # of times smt miss happened */

    /* RX errors */
    ulong rx_overrun_errors;      /* # of overruns */
    ulong rx_macif_parity_errors; /* # of parity errors detected by the MACIF */
    ulong rx_port_errors;         /* # of FSI receive port errors */

    /*
     * Transmit ring information 
     */
    mif68840_tx_desc *txr_malloc;  /* Tx ring pointer,usable for free */
    mif68840_tx_desc *txr;         /* Tx ring pointer */
    ulong tx_head, tx_tail;        /* Indexes into tx ring */
    uint tx_count;                 /* Count of Tx ring entries used */
    boolean tx_limited;            /* Limit one tx pak at a time */
    
    mif68840_tx_shadow *txr_shadow;/* Tx ring shadow */

    /* TX errors */
    ulong tx_port_errors;       /* # of FSI transmit port errors */
    ulong tx_parity_errors;     /* # of FSI transmit parity errors */
    ulong tx_abort_errors;      /* # of FSI transmit abort errors */

    /* 
     ***********************
     * Additional info needed for proper chip operation
     ***********************
     */
    ushort  bridge;             /* Go promiscuous */
    boolean promiscuous_llc;    /* Go promiscuous llc */
    uchar   interesting_add[6]; /* Some interesting address to be added */
    uchar   srbridge_on_idb;    /* Is SRB configured on the interface */
    boolean rxovr_4;            /* Ring4 overran. Ignore interrupt disabled*/
    ulong   rxovr_llc_cnt;      /* #times llc ring has overrun */
    ulong   rxovr_smt_cnt;      /* #times smt ring has overrun */

    /*
     **********************
     *  MIF68840 MAC information 
     **********************
     */

    boolean invalidClaim;   /* Flag used for ValidClaimReceived().  not used */
    boolean beaconing;      /* true if we be direct beaconing */
    ulong  curBeaconType;   /* current beacon frame type (if any) */ 
    uchar *bcn_ptr;         /* Beacon frame pointer */
    ulong bcn_ptr_size;     /* Size of beacon frame */
    mif68840_tx_desc *bcn_malloc;  /* Beacon pointer, usable for free */
    mif68840_tx_desc *bcn_rd_ptr;  /* Beacon read pointer */
    mif68840_tx_desc *bcn_wr_ptr;  /* Beacon write pointer */


    /* 
     **********************
     * MIF68840 Port information
     **********************
     */
    ushort remaining[MAX_PORT_COUNT];   /* remaining count to link failure */

    /*
     **********************
     * MIF68840 CAM information
     *********************
     */
    uchar hwver_hi;
    uchar hwver_lo;
    boolean cam_present;

    /*
     **********************
     *  MIF68840 Errors and other counters information 
     **********************
     */

    ulong np_err;           /* # of node processor errors */
    ulong ebuf_err;         /* # of elasticity buffer errors */
    ulong throttled;        /* # of interface throttles */

    /*
     * Function pointers (vectors) for the VIP platforms.
     */

    void (* init_controller) (hwidbtype *);
	pooltype	*ll_pool_ptr;
	/*
	 * Pointer to the pak currently being filled-in with SMT messages.
	 * Each pak will be sent to the RSP host as one love letter.
	 */
	paktype	*waitq_pak;
	boolean	pak_on_watchq; /* Is the pak above enqueued on watchq? */
	uchar	waitmsg_count; /* No. of SMT msgs currently held in pak */

} mif68840_instance;


/*************************************************************************/
/* defines for control and status register (csr) */

/* Status Register (read only) */

#define HIGH_BCUR_PHYB          0x2000
#define HIGH_BCUR_PHYA          0x1000
#define OPTICAL_BYPASS_PRESENT  0x0800
#define FDDI_DUAL               0x0400
#define MULTIMODE_PHYB          0x0200
#define MULTIMODE_PHYA          0x0100
#define MULTIMODE_PHYB_PHYA     0x0300

/* Control Register (R/W) */

#define EN_OPTICAL_BYPASS_SEC   0x0010
#define EN_OPTICAL_BYPASS_PRI   0x0008
#define LED_DUALHOME            0x0004
#define LED_PHYB                0x0002
#define LED_PHYA                0x0001

/*************************************************************************/
/* defines for registers of CAM */

#define CSR                         0x0
#define DCR                         0x1
#define DDR                         0x2
#define LSAR                        0x3
#define MSAR                        0x4
#define USAR                        0x5
#define CCF                         0x6
#define CDF                         0x7
#define SRD                         0x8
#define CRD                         0x9

/*
 * Nasty definitions that CAM code must recognize.
 * CAM operations must be done only on the following FDDI-PA revisions:
 *     PA-F-SM/MM/FD-MM/FD-SM with 28-1605-02 fab version 1.2 or greater
 *     PA-F-SM/MM/FD-MM/FD-SM with 28-1605-03 fab version 1.12 or greater
 */
#define FAB02_WITH_CAM_SUPPORT      2
#define FAB03_BASE                  11
#define FAB03_WITH_CAM_SUPPORT      12

#define MAX_CAM_ENTRIES             1024

#define CAM_ENABLE_MATCH_FLAG       0x9ffc
#define CAM_DISABLE_MATCH_FLAG      0xbffc

#define CAM_ENABLE_FULL_FLAG        0xe7fc
#define CAM_DISABLE_FULL_FLAG       0xeffc

#define CAM_INPUT_NOT_SCRAMBLED     0xf9fc
#define CAM_INPUT_SCRAMBLED         0xfbfc

#define CAM_64_RAM_0                0xfe3c
#define CAM_48_RAM_16               0xfe7c
#define CAM_32_RAM_32               0xfebc
#define CAM_16_RAM_48               0xfefc
#define CAM_0_RAM_64                0xff3c

#define CAM_COMPARE_MASK_DISABLE    0xffcc
#define CAM_ENABLE_ADDRESS_INC      0xfff0

#define CAM_DST_CNTR_START01_END11 0x3fff
#define CAM_SRC_CNTR_START01_END11 0xf9ff
#define CAM_LOAD_DST_SEG_CNTR_01   0xffcf
#define CAM_LOAD_SRC_SEG_CNTR_01   0xfff9
#define CAM_COMMAND(instruction)        rp->cam[CCF] = instruction;
#define CAM_COMMAND_DIRECT(instruction) rp->cam[DCR] = instruction;
#define CAM_DATA_WR(data)               rp->cam[CDF] = data;
#define CAM_DATA_WR_DIRECT(data)        rp->cam[DDR] = data;

#define VBC_M_ALM_E        CAM_COMMAND(0x043d)

/* COMMANDS VIA FIFO */

#define SPD_CR          CAM_COMMAND(0x0100)
#define SPS_M_DIRECT(addr)  CAM_COMMAND_DIRECT(0x0804) CAM_COMMAND_DIRECT(addr)
#define SPS_CR_DIRECT            CAM_COMMAND_DIRECT(0x0000)
#define MOV_M_NF_CR_V            CAM_COMMAND(0x0334)
#define TCO_DS_DIRECT            CAM_COMMAND_DIRECT(0x0228)
#define TCO_CT_DIRECT            CAM_COMMAND_DIRECT(0x0200)
#define TCO_PA_DIRECT            CAM_COMMAND_DIRECT(0x0208)
#define TCO_SC_DIRECT            CAM_COMMAND_DIRECT(0x0210)

/* CAM CSR definitions */
#define CAM_CSR_SA_MISS 0x1000
#define CAM_CSR_SA_MISS_INTR_EN    0x0008
#define CAM_CSR_FIFO_CLEAR         0x0040
#define CAM_CSR_FIFO_GO            0x8000
#define CAM_CSR_CAM_COMPARE_ENABLE 0x0001
#define CAM_CSR_CAM_COMPARE        0x0080
#define CAM_STATUS_MATCH_FLAG          0x0001

/*************************************************************************/

/*****************************************************************
 * Static inlines
 *****************************************************************/
   
/*
 * mif68840_rxring_flush :
 * Flush the Rx ring from the data cache
 */
static inline void mif68840_rxring_flush (mif68840_instance *ds)
{
    if (MIF68840_RX_RING_ATTRIBUTE == TLB_UNCACHED) {
        return;
    }
}

/*
 * mif68840_flush_pak:
 * NULL out the relevant entries in the paktype
 */
static inline void mif68840_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
}

/*
 * mif68840_cleanup_pak:
 * Return any particles and then NULL out relevant entries in the paktype
 */
static inline void mif68840_cleanup_pak (paktype *pak)
{ 
    particletype *mp;
 
    while (mp = dequeue_inline(&pak->particlequeue), mp) {
        retparticle(mp);
    }
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
}


/*
 * Prototypes
 */

/* pas/if_pas_mif68840.c */
extern hwidbtype *mif68840_idb_create(uint slot, uint port);
extern void     mif68840_deallocate(hwidbtype *hwidb);
extern void     mif68840_get_default_hw_address(hwidbtype *idb, uchar *addr);
extern boolean  mif68840_platform_init(hwidbtype *idb);
extern boolean  mif68840_init_buffer_pools(hwidbtype *idb);
extern void     mif68840_lineproto_change(hwidbtype *idb,
                            boolean state);
extern pa_wrapper_t     mif68840_eim_wrapper;
extern void     mif68840_tx_start(hwidbtype *hwidb);
extern void     mif68840_fastsend(hwidbtype *idb, paktype *pak);
extern void     mif68840_system_configured(hwidbtype *idb);
extern process  mif68840_CSPTimer_process(void);
extern void     mif68840_cache_threshold(pooltype *pool);

/* pas/if_isr_mif68840.c */
extern void     mif68840_rx_llc_interrupt(hwidbtype *idb);
extern void     mif68840_rx_smt_interrupt(hwidbtype *idb);
extern void     mif68840_rx_error(ulong rmd_status_length,
                            hwidbtype *idb, particletype *mp);
extern void     mif68840_tx_error(ulong tmd_status, hwidbtype *idb);
extern void     mif68840_throttle(hwidbtype *idb);
extern uchar    read_fcr(mif68840_instance *ds, ulong value);
extern void     mif68840_fddi_cam_populate(hwidbtype *, uchar *);
extern void     mif68840_fddi_cam_depopulate(hwidbtype *, uchar *, hwidbtype *);

#if defined(RVIP) || defined(SVIP)
#include "if_vip_mif68840.h"
#endif
 
#if defined(PREDATOR)
#include "if_c7100_mif68840.h"
#endif

#endif __IF_PAS_MIF68840_H__
