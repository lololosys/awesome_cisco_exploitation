/* $Id: if_c4000_lance.h,v 3.3.62.2 1996/04/06 00:50:12 vdukki Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_lance.h,v $
 *------------------------------------------------------------------
 * C4000 lance driver header file
 *
 * 4/92, Steve Southam, Marciano Pitargue
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: if_c4000_lance.h,v $
 * Revision 3.3.62.2  1996/04/06  00:50:12  vdukki
 * CSCdi48472:  Sustained process switched throughput degraded 28%
 * Branch: California_branch
 * Modified the lance_stop() function to add 100 microseconds delay
 * only for 6E NIM.
 *
 * Revision 3.3.62.1  1996/03/18  20:41:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:23:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  16:38:39  heluthra
 * CSCdi37135:  DEC BRUT Router does not recognise ethernet (10baseT) down
 * Disconnecting the 10Base2 cable on Brut gives the excessive collision
 * error and does not show the line protocol as down. Added detection of
 * RTRY error (excessive collision) to bring the line protocol down.
 *
 * Revision 2.2  1995/08/09  00:21:10  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.1  1995/06/07  21:34:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * if_xx_lance.h -- definitions for AMD AM7990 (Lance) chip for xx
 *
 *	Based on "if_lance.h", June 1988, Bill Westfield
 *	Modified for xx, March 1991, Marciano Pitargue
 *
 */


/*
 * Prototypes for instance vectors.
 */
typedef boolean (* get_squelch_t) (hwidbtype *);
typedef void (* set_media_type_t) (hwidbtype *);
typedef void (* set_squelch_t) (hwidbtype *, int);

typedef struct xx_eim_nim_regs xx_eim_nim_regs_t;
struct xx_eim_nim_regs {
    volatile ushort id_prom[2];
    volatile ushort pad_1fe_4[254];
    volatile ushort xx_csr0;
    volatile ushort xx_csr1;
};

typedef struct xx_eim_lance_regs xx_eim_lance_regs_t;
struct xx_eim_lance_regs {
    volatile ushort csr;
    volatile ushort rap;
};

/*
 * Dual EIM defines
 */
#define DEIM_PORT_A_OFFSET       0x0000
#define DEIM_PORT_B_OFFSET       0x0800
#define EIM_LANCE_REGS_OFFSET    0x0400

/*
 * xx_csr0 bits:
 */
#define EIM_RESET               1
#define EIM_MEDIA_TYPE          2
#define EIM_SQUELCH             4
#define EIM_10BASET_LINK_STATUS 8

/* 
 * subunits for old xx box 
 */
#define OLD_XX_MAX_ETHER_SUBUNITS 2
#define OLD_XX_MAX_ETHER_ADDRESSES 6

struct lance_instance {

    /*
     * Driver defined structure
     * Allocate in CPU RAM and keep long words long word aligned for speed.
     */
    struct xx_eim_nim_regs   *eim_regaddr;   /* Pointer to base of EIM regs */
    struct xx_eim_lance_regs *lance_regaddr; /* Pointer to lance registers */

    lance_initblock_t *ib;	 /* Pointer to Hardware initialization block */

    short rx_size, tx_size;	 /* RX and TX ring sizes */

    uchar *rx_base;		    /* Address of malloced RX ring */
    lance_rx_ring_t *rx_ra;	    /* Adjusted address of RX ring */
    paktype *rx_p[MAX_RING];	    /* Copy of RX ring packet pointers */
    lance_rx_descriptor_t *rx_head; /* Pointer to next RX ring entry */
    paktype **head_rxp;		    /* Pointer to RX packet for rx_head */
    lance_rx_descriptor_t *rx_max;  /* Pointer to last RX ring entry */

    uchar *tx_base;		    /* Address of malloced tx ring */
    lance_tx_ring_t *tx_ra;	    /* Aligned address of tx ring */
    paktype *tx_p[MAX_RING];	    /* Copy of TX ring packet pointers */
    lance_tx_descriptor_t *tx_head; /* Next ring entry we expect transmitted */
    paktype **head_txp;		    /* Pointer to TX packet for tx_head */
    lance_tx_descriptor_t *tx_tail; /* Last ring entry we expect transmitted */
    paktype **tail_txp;		    /* Pointer to TX packet for tx_tail */
    lance_tx_descriptor_t *tx_max;  /* Pointer to last TX ring entry */

    ulong tx_count;		/* Count of packets currently in TX Q */

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
     * use the lance functionality. The following vectors are valid
     * for all platforms in low-end/midrange using the LANCE.
     */
    get_regaddr_t 		get_regaddr;
    dev_reset_t  		reset;
    set_vectors_t 		set_vectors;
    show_structure_info_t   	show_structure_info;
    pre_init_controller_t 	ctrlr_pre_init;
    post_init_controller_t      ctrlr_post_init;
    delay_vector_t              delay_vector;
 
    /*
     * The following vectors are used only by the C4XXX platforms.
     */
    set_media_type_t 	set_media_type;
    get_squelch_t 	get_squelch;
    set_squelch_t 	set_squelch;

    /*
     * flag to distinguish presidio from lance.
     */
    boolean mode;
};

/*
 * Prototypes
 */
void lance_get_default_hw_address(hwidbtype *idb, uchar addr[IEEEBYTES]);
void lance_show_structure_info(hwidbtype *idb);

/*
 * Externs
 */
extern void lance_init(hwidbtype *, boolean);
extern void lance_check_version(int slot);


/*
 * Product specific externals
 */
extern void lance_reset(hwidbtype *idb);
extern void lance_get_regaddr(hwidbtype *idb);
extern void lance_set_vectors(hwidbtype *idb);


