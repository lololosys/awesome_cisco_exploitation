/* $Id: if_pas_ibm2692_common.h,v 3.4.10.2 1996/03/28 02:00:17 sthormod Exp $
 * $Source: /release/111/cvs/Xsys/pas/if_pas_ibm2692_common.h,v $
 *------------------------------------------------------------------
 * if_pas_ibm2692_common.h - Common header file for 4R port adapter.
 *
 * August 1995, HC Wang & Stig Thormodsrud
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_ibm2692_common.h,v $
 * Revision 3.4.10.2  1996/03/28  02:00:17  sthormod
 * CSCdi52811:  4R SMT stats not displayed
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/03/21  23:23:36  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:28:59  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/06  19:51:47  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.2.2.1  1995/12/19  23:32:37  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.4  1996/02/23  01:43:48  sthormod
 * CSCdi49614:  VIP/4R: clean up dead code
 * Remove #ifdef'd out code
 *
 * Revision 3.3  1996/02/12  23:06:53  hwang
 * CSCdi46480:  4R failed hot standby test
 * Add configuration mechanism to allow 4r driver to configure
 * new functional address.
 *
 * Revision 3.2  1995/11/17  18:54:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  05:35:33  sthormod
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note: several of the data structure listed in this file and in
 *       if_pas_ibm2692.h are defined to map directly to the chip's
 *       memory.  See IBM "LAN Technical Reference, Token Ring Network
 *       LANStreamer Adapters".  SC30-3708-00 
 */

/*
 * Configuration data send to the 4R
 */
typedef struct ibmtr_cfg_data_ {
    ulong       config_type;       /* complete or partial config required */
    uchar	hardware[6];       /* mac address */
    uchar       pad1[2];           /* pad for long word alignment */
    ulong	tr_functional;     /* functional address */
    ulong	tr_group;          /* group addres */
    uint 	tr_ring_mode;      /* bridging flags */
    ushort	tr_ring_speed;     /* ring speed */
    ushort	srb_thisring;      /* this ring number */
    ushort	srb_targetring;    /* target ring number */
    uchar	srb_bridge_num;    /* bridge number */
    uchar       pad2[1];           /* pad for long word alignment */
} ibmtr_cfg_data_t;

#define CFG_DATA_SIZE btow(sizeof(ibmtr_cfg_data_t))

#define	INITIAL_CONFIG		1
#define	FUNCTIONAL_ADDR_CHANGE	2

/*
 * types for CCB_CMD_VIP_4R_REQUEST_STATS
 */
#define  VIP_4R_SMT_STATS             1
#define  VIP_4R_ERROR_STATS           2


typedef struct {
    ushort bring_up_code;
    uchar  version[10];
} init_t;

typedef struct {
    ushort ret_code;
    ushort open_error_code;
    uchar  version[10];
} open_t;

typedef struct {
    ushort ret_code;
} cfg_brg_t;

typedef struct {
    ushort status;
    ushort data[4];
} adp_chk_t;

typedef struct {
    ushort lan_status;
} lan_status_change_t;

/*
 * Data structure for 4R error message love letter
 */
typedef struct {
    ulong ll_type;
    ulong err_type;
    union {
	init_t               init;
	open_t               open;
	cfg_brg_t            cfg_brg;
	lan_status_change_t  lan_status_change;
	adp_chk_t            adp_chk;
    } type;
} vip_4r_err_ll;

/*
 * Data structure for reading SMT data from MPC chip
 *
 * LAN Techical Reference 7-7
 */
typedef struct {
    ulong  phys_addr;             /* Adapter physical address */
    uchar  up_node_addr[6];       /* Next active upstream node address */
    ulong  PACKED(up_phys_addr);  /* Next active upstream physical address */
    uchar  poll_addr[6];          /* Last poll address */
    uchar  reserve1[2];           /* Reserved */
    ushort acc_priority;          /* Transmit access priority */
    ushort auth_source_class;     /* Source class authorization */
    ushort att_code;              /* Last attention code */
    uchar  source_addr[6];        /* Last source address */
    ushort PACKED(beacon_type);   /* Last beacon type */
    ushort major_vector;          /* Last major vector */
    ushort lan_status;            /* Network status */
    ushort soft_error_time;       /* Soft error timer value */
    ushort fe_error;              /* Front end error counter */
    ushort local_ring;            /* Ring number */
    ushort mon_error;             /* Monitor error code */
    ushort beacon_transmit;       /* Beacon transmit type */
    ushort beacon_receive;        /* Beacon receive type */
    ushort frame_correl;          /* Frame correlator save */
    uchar  beacon_naun[6];        /* Beaconing station NAUN */
    uchar  reserve2[4];           /* Reserved */
    ulong  PACKED(beacon_phys);   /* Beaconing station physical address */
} smt_parms_table;

/*
 * Data stucture for reading address table from MPC chip
 *
 * LAN Techical Reference 7-6
 */
typedef struct {
    uchar node_addr[6];             /* Adapter node address */
    uchar reserved[4];              /* Reserved */
    ulong PACKED(functional_addr);  /* Adapter Functional Address */
} addr_table;


/*
 * data structure used by loveletter type VIP_LL_4R_SMT_STATS  
 */
typedef struct {
    ulong           ll_type;  
    smt_parms_table smt;
    uchar           pad1[2];
    addr_table      addr_tbl;
    ushort          PACKED(ring_util);
    uchar           ucode_version[10];
} vip_4r_smt_ll;



/*
 * MPC error log
 */
typedef struct {
    ulong err_line;
    ulong err_internal;
    ulong err_burst;
    ulong err_arifci;
    ulong err_abort;
    ulong err_lostframe;
    ulong err_rcvcongestion;
    ulong err_copy;
    ulong err_token;
} mpc_errors;


/*
 * TX errors from TX descriptor
 */
typedef struct {
    ulong tx_pci_read_error;		/* pci read error */
    ulong tx_icd_read_parity_error;	/* ICDB read parity error */
    ulong tx_icd_write_parity_error;	/* ICDB write parity error */
    ulong tx_icd_addr_parity_error;	/* ICDB address parity error */
    ulong tx_stat_write_error;		/* status write error */
    ulong tx_fifo_underrun;		/* tx fifo underrun error */
    ulong tx_fifo_parity_error;		/* tx fifo parity error */
} tx_errors;

/* 
 * RX error from descriptors
 */
typedef struct {
    ulong rx_pci_read_parity_error;	/* pci data read parity error */
    ulong rx_int_read_parity_error;	/* internal parity error read desc */
    ulong rx_int_write_parity_error;	/* internal parity error write desc */
    ulong rx_int_pci_address_bfr;	/* internal parity error pci addr bfr */
    ulong rx_fifo_parity_error;		/* fifo parity error */
    ulong rx_no_data_bfr;		/* no rx data bfr available */
    ulong rx_pci_write_error;		/* bad completion of pci write */
    ulong rx_fifo_overrun;		/* rx fifo overrun error */
    ulong rx_parity_error;		/* receive parity error */
    ulong rx_mpc_error;		        /* receive mpc error */
    ulong rx_imp_abrt_frm;	        /* receive implicit aborted frame */
    ulong rx_exp_abrt_frm;	        /* receive explicit aborted frame */
} rx_errors;

typedef struct {
    ulong   ll_type;
    mpc_errors mpc_err;
    tx_errors  tx_err;
    rx_errors  rx_err;
} vip_4r_errors_ll;


struct vip4r_data_t_ {
    smt_parms_table  smt;
    uchar            pad1[2];
    addr_table       addr_tbl;
    mpc_errors       mpc_err;
    tx_errors        tx_err;
    tx_errors        tx_prev;
    rx_errors        rx_err;
    rx_errors        rx_prev;
    sys_timestamp    init_timer;
    ulong            ring_util;
    uchar            ucode_version[10];
};

#define INSTANCE_4R_STATS(idb)      ((idb)->vip4r_info)

/* end of file */
