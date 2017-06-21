/* $Id: if_c3000_lance.h,v 3.3.62.1 1996/04/06 00:50:08 vdukki Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_lance.h,v $
 *------------------------------------------------------------------
 * if_c3000_lance.h -- definitions for AMD AM7990 (Lance) chip
 * 
 * Based on "if_lance.h", June 1988, Bill Westfield
 * Modified for Pancake, December 1989, Chris Shaker
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_lance.h,v $
 * Revision 3.3.62.1  1996/04/06  00:50:08  vdukki
 * CSCdi48472:  Sustained process switched throughput degraded 28%
 * Branch: California_branch
 * Modified the lance_stop() function to add 100 microseconds delay
 * only for 6E NIM.
 *
 * Revision 3.3  1995/11/17  17:38:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  16:38:38  heluthra
 * CSCdi37135:  DEC BRUT Router does not recognise ethernet (10baseT) down
 * Disconnecting the 10Base2 cable on Brut gives the excessive collision
 * error and does not show the line protocol as down. Added detection of
 * RTRY error (excessive collision) to bring the line protocol down.
 *
 * Revision 2.1  1995/06/07  21:33:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct lance_regs lance_hw_reg_t;
struct lance_regs {
    volatile ushort csr;	/* the 4 control/status registers */
    volatile ushort rap;	/* select which CSR is pointed to */
};

struct lance_instance {

    /*
     * Driver defined structure
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */

    lance_hw_reg_t *lance_regaddr;	/* Pointer to lance registers */
    ushort *reset_reg;          /* Pointer to reset register */
    u_short reset_mask;		/* Reset bit in control register */
    u_short padding_1;		/* Padd back to long word boundary */

    lance_initblock_t *ib;	/* Pointer to Hardware initialization block */

    short rx_size, tx_size;	/* RX and TX ring sizes */

    uchar *rx_base;		    /* Address of malloced RX ring */
    lance_rx_ring_t *rx_ra;	    /* Adjusted address of RX ring */
    paktype *rx_p[MAX_RING];	    /* Copy of RX ring packet pointers */
    lance_rx_descriptor_t *rx_head; /* Pointer to next RX ring entry */
    paktype **head_rxp;	            /* Pointer to RX packet for rx_head */
    lance_rx_descriptor_t *rx_max;  /* Pointer to last RX ring entry */

    uchar *tx_base;		    /* Address of malloced tx ring */
    lance_tx_ring_t *tx_ra;	    /* Aligned address of tx ring */
    paktype *tx_p[MAX_RING];	    /* Copy of TX ring packet pointers */
    lance_tx_descriptor_t *tx_head; /* Next ring entry we expect transmitted */
    paktype **head_txp;		    /* Pointer to TX packet for tx_head */
    lance_tx_descriptor_t *tx_tail; /* Last ring entry we expect transmitted */
    paktype **tail_txp;		    /* Pointer to TX packet for tx_tail */
    lance_tx_descriptor_t *tx_max;  /* Pointer to last TX ring entry */
    ulong tx_count;		    /* Count of packets currently in TX Q */

    /* Count errors: */
    ulong spurious_idon;	/* # spurious init done events */
    
    /* Throttle statistics */
    ulong enabled;		/* # times we have been enabled */

    /* RX errors */
    ulong no_enp;		/* # ring entries without ENP set */
    ulong buffer_err;		/* # lance buffer errors */
    ulong overflow_err;		/* # lance overflow errors */

    /* TX errors */
    ulong tx_buff;		/* # times lance got no ENP before chain end */
    ulong tdr;			/* Saved last tdr value reported by lance    */
				/*  when last retry error was reported.      */
				/*  VALID IF idb->output_excessive_collision */
    ulong rtry_error_count;	/* # times RTRY error occured */
    boolean fatal_tx_err;	/* A fatal tx error occured. Reset lance     */
    
    /*
     * Function pointers so that the Presidio chip driver can also
     * use the lance functionality.
     */
    get_regaddr_t 		get_regaddr;
    dev_reset_t  		reset;
    set_vectors_t 		set_vectors;
    show_structure_info_t   	show_structure_info;
    pre_init_controller_t 	ctrlr_pre_init;
    post_init_controller_t      ctrlr_post_init;
    delay_vector_t              delay_vector;

    /*
     * flag to distinguish presidio from lance.
     */
    boolean mode;
};

/*
 * Prototypes
 */
void lance_get_regaddr(hwidbtype *idb);
void lance_set_vectors(hwidbtype *idb);
void lance_reset(hwidbtype *idb);
void lance_get_default_hw_address(hwidbtype *idb, uchar addr[IEEEBYTES]);
void lance_show_structure_info(hwidbtype *idb);

/*
 * Externs
 */
extern void lance_init(hwidbtype *, boolean);

