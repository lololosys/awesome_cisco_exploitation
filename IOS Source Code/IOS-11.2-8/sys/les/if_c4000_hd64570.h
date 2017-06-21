/* $Id: if_c4000_hd64570.h,v 3.3.62.1 1996/08/28 12:58:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_hd64570.h,v $
 *------------------------------------------------------------------
 * if_c4000_hd64570.h - header file for HD64570 serial controller (4T NIM).
 *
 * March 1994, Mohsen Mortazavi
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_hd64570.h,v $
 * Revision 3.3.62.1  1996/08/28  12:58:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:39:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:47:26  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:34:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Secondary wrappers.
 */
extern void qsim_t0(void), qsim_0t(void);
extern void qsim_thd0(void), qsim_0thd(void);
extern void qsim_tt(void), qsim_tthd(void);
extern void qsim_thdt(void), qsim_thdthd(void);
extern void qsim_thd0_bsc(void), qsim_0thd_bsc(void);
extern void qsim_thd_bsc_thd_bsc(void);
extern void qsim_thd0_bsc_t(void), qsim_t_0thd_bsc(void);
extern void qsim_thd0_bsc_0thd(void), qsim_thd0_0thd_bsc(void);

/*
 * For each port pair on the 4T NIM  there 
 * exists one of these register regions.
 */

struct qsim_regs {
    volatile ushort id_prom[2];
    volatile ushort ch_A_mdm;     	/* port 0 or 2 modem control */
    volatile ushort ch_A_int;		/* port 0 or 2 interrupt control */
    volatile ushort ch_B_mdm; 	        /* port 1 or 3 modem control */
    volatile ushort ch_B_int;		/* port 1 or 3 interrupt control */
    volatile ushort vector_base;	/* interrupt vector base */
    volatile ushort vector;             /* SCA register access control */
};

/* SCA register offsets. */
#define QSIM_PORT23_OFFSET	0x10000
#define QSIM_SCA_REG_OFFSET	0x100

/* 
 * DMA channel priority setting.
 */
#define HD64570_DMA_PRIORITY    (HD64570_PCR_CCC | HD64570_PCR_TX)  

/*
 * Fifo threshold for transmit and receive channels.
 */
/* Transmit channels for Port 0 and 2 */
#define HD64570_P0_TX_HIGH_MARKER       0x0B00
#define HD64570_P0_TX_LOW_MARKER        0x0008

#define HD64570_P2_TX_HIGH_MARKER       HD64570_P0_TX_HIGH_MARKER
#define HD64570_P2_TX_LOW_MARKER        HD64570_P0_TX_LOW_MARKER

/* Transmit channel for port 1 */
#define HD64570_P1_TX_HIGH_MARKER       0x0F00
#define HD64570_P1_TX_LOW_MARKER        0x0008

/* Transmit channel for port 3 */
#define HD64570_P3_TX_HIGH_MARKER       0x1F00
#define HD64570_P3_TX_LOW_MARKER        0x0017

/* Receive channel for all ports */
#define HD64570_RX_MARKER                 0x03
#define HD64570_RX_BSC_MARKER             0x00

/*
 * Modem Control Register Bits
 */
#define QSIM_MDM_BDSR		0x0001
#define QSIM_MDM_BCTS		0x0002
#define QSIM_MDM_BDCD		0x0004
#define QSIM_MDM_BRTS		0x0008
#define QSIM_MDM_BDTR		0x0010
#define QSIM_MDM_LOOP		0x0020
#define QSIM_MDM_SENSE0         0x0040
#define QSIM_MDM_SENSE1	        0x0080
#define QSIM_MDM_SENSE2        	0x0100
#define QSIM_MDM_RESET		0x0200
#define QSIM_MDM_CTRL0		0x0400
#define QSIM_MDM_CTRL1		0x0800
#define QSIM_MDM_CTRL2		0x1000
#define QSIM_MDM_CTRL3		0x2000
#define QSIM_MDM_CTRL4		0x4000

/*
 * Interrupt Control Register Bits
 */
#define QSIM_INT_SENSE2_MASK          0x0100
#define QSIM_INT_SCA_BYTE_MODE        0x0200
#define QSIM_INT_SCA_A0               0x0400
#define QSIM_INT_MODEM_INTR_ENABLE    0x0800
#define QSIM_INT_MODEM_INTR_CLEAR     0x1000
#define QSIM_INT_MODEM_INTR	      0x2000
#define QSIM_INT_SCA_INT 	      0x4000
#define QSIM_INT_RESET_SCA	      0x8000

/* 
 * 5/1 control bits
 */
#define PA_CTRL0    QSIM_MDM_CTRL0
#define PA_CTRL1    QSIM_MDM_CTRL1
#define PA_CTRL2    QSIM_MDM_CTRL2
#define PA_CTRL3    QSIM_MDM_CTRL3
#define PA_CTRL4    QSIM_MDM_CTRL4

#define PA_CONTROL_BITS (PA_CTRL0 | PA_CTRL1 | PA_CTRL2 | \
		      PA_CTRL3 | PA_CTRL4)
#define PA_CONTROL_SHIFT	10

/*
 *  5/1 Port Adapter bits in DTE mode.
 */
#define QSIM_DTE_DSR		QSIM_MDM_BDSR
#define QSIM_DTE_CTS		QSIM_MDM_BCTS
#define QSIM_DTE_DCD		QSIM_MDM_BDCD
#define QSIM_DTE_RTS		QSIM_MDM_BRTS
#define QSIM_DTE_DTR		QSIM_MDM_BDTR

/*
 *  5/1 Port Adapter bits in DCE mode.
 *
 *  *** NOTE ***
 *  DSR is not defined, since it is controlled as follows:
 *  On output: DSR is asserted when DCD is asserted.
 *  On input:  DSR is asserted when DTR is asserted.
 *   
 */
#define QSIM_DCE_CTS	        QSIM_MDM_BRTS
#define QSIM_DCE_DCD    	QSIM_MDM_BDTR
#define QSIM_DCE_RTS	        QSIM_MDM_BCTS
#define QSIM_DCE_DTR    	QSIM_MDM_BDCD

#define PORT_PRESENT(idb)    ((idb == (hwidbtype *)BAD_ADDRESS) ? 0 : 1)

struct hd64570_instance {
    qsim_regs_t *regaddr;            /* pointer to 4T NIM registers */ 
    hd64570_reg_t *hd64570_reg;      /* Pointer to HD64570 registers */
    hd64570_msci_reg_t *msci_reg;    /* Pointer to MSCI for this channel */
    hd64570_dmac_reg_t *dmac_rx_reg; /* Pointer to DMAC RX register */
    hd64570_dmac_reg_t *dmac_tx_reg; /* Pointer to DMAC TX register */
    volatile ushort *mdm_reg;        /* pointer to modem control register */
    volatile ushort *int_reg;        /* pointer to interrupt control regsiter*/
    ushort rx_size;        	     /* RX ring sizes */
    ushort tx_size;	             /* TX ring sizes */
    ushort tx_count;		   /* Count of packets currently in TX queue */

    uchar *rx_base;		     /* Address of malloced rx ring */
    hd64570_buf_ring_t *rx_ra;	     /* Aligned address of rx ring */
    hd64570_buf_descr_t *rx_last;    /* The last packet we processed */

    ulong shift_tx_cpb;           /* Shifted version of tx cpb */
    uchar *tx_base;		  /* Address of malloced tx ring */
    hd64570_buf_ring_t *tx_ra;	  /* Aligned address of tx ring */
    hd64570_buf_descr_t *tx_last; /* Last buffer we processed */

    uchar port_id;                /* Port Adapter ID from 5/1 adapter. */
    uchar adapter_flags;          /* Port Adapter current settings . */
    ushort up_signal;             /* signal to use as link up/down indicator */

    ulong residual_bit;           /* Residual bit errors */

    ulong hdx_state;              /* State of the half duplex conection */

    /* Flap Protection */
    boolean flap_enabled;       /* Whether we've throttled interrupts */
    uint    flap_backoff;       /* Number of backoffs after throttling */
    sys_timestamp flap_time;    /* Time of last flap */
    uint    flap_count;         /* Number of interface flaps */
    uint    flap_rate;          /* Maximum rate of flaps per second */

    appl_periodic_t appl_periodic; /* used by G.703 applique only */

    /* G.703 applique stuff. */
    boolean g703_loop_line;       /* G.703 loop line status */
    ushort g703_setup;            /* current settings on G.703 adapter */
    uchar g703_signals;           /* G.703 applique signals. */ 
};

#define QSIM_MAX_PORTS   4


/* Defines used during the initialization of the secondary wrappers. */
#define HALF_DUPLEX_SDLC_INDEX    0x01
#define FULL_DUPLEX_SDLC_INDEX    0x04
#define HALF_DUPLEX_BSC_INDEX     0x10

#define HDX_SDLC_P0               (HALF_DUPLEX_SDLC_INDEX << 1)
#define HDX_SDLC_P1               HALF_DUPLEX_SDLC_INDEX
#define FDX_SDLC_P0               (FULL_DUPLEX_SDLC_INDEX << 1)
#define FDX_SDLC_P1               FULL_DUPLEX_SDLC_INDEX
#define HDX_BSC_P0                (HALF_DUPLEX_BSC_INDEX << 1)
#define HDX_BSC_P1                HALF_DUPLEX_BSC_INDEX

/*
 * 5/1 Port Adapter IDs. (should be in a more generic header file). 
 */
#define PA_NONE                 0
#define PA_NO_CABLE		1
#define PA_RS232_DTE		2
#define PA_RS232_DCE		3
#define PA_V35_DTE		4
#define PA_V35_DCE		5
#define PA_RS449_DTE		6
#define PA_RS449_DCE		7
#define PA_X21_DTE		8
#define PA_X21_DCE		9
#define PA_RS232_NRZI_DTE  	10	
#define PA_RS232_NRZI_DCE  	11	
#define PA_G703_DTE		12
#define PA_G703_DCE		13
#define PA_RS530_DTE		14
#define PA_RS530_DCE		15

/*
 * 5/1 Port adapter settings.
 */
#define PA_LOOP                0x01
#define PA_INVERT_TXC          0x02

/*
 * G.703 applique Status register bits
 */
#define G703_AIS               0x01
#define G703_RAI               0x02
#define G703_LOS               0x04
#define G703_LOF               0x08
#define G703_BER               0x10
#define G703_NELR              0x20
#define G703_FELR              0x40
#define G703_SIGNALS           0x70

/*
 * G.703 applique control register bits
 */
#define G703_CRC4         0x0001
#define G703_STOP_SLOT    0x003e
#define G703_START_SLOT   0x07c0
#define G703_TS16         0x0800
#define G703_REMOTE_LOOP  0x1000
#define G703_LOOP_UP      0x2000
#define G703_LOOP_DOWN    0x4000
#define G703_LINE         0x6000
#define G703_CLOCK_SRC    0x8000

#define G703_DATA         QSIM_MDM_CTRL0
#define G703_CLOCK        QSIM_MDM_CTRL2
#define G703_ENHANCED     QSIM_MDM_CTRL3

/*
 * Platform specific read port id. 
 */
#define HD64570_PLATFORM_READ_PORT_ID(idb)  hd64570_read_port_id(idb) 

/*
 * Prototypes
 */
void hd64570_timer_interrupt(hwidbtype *port1_idb, hwidbtype *port0_idb);
void hd64570_dmer_pcr_write(hd64570_instance_t *ds, boolean even_flag,
			    u_char value);
void hd64570_show_serial_state(hwidbtype *idb, char *leader);

/*
 * Product specific externals
 */
extern void *hd64570_get_wrapper_base(uint slot);
extern void hd64570_set_vectors(hwidbtype *idb);
extern void hd64570_check_version(int slot);
extern void hd64570_read_port_id(hwidbtype *idb);
