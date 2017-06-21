/* $Id: if_c3000_tms380.c,v 3.3.60.3 1996/08/28 12:57:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_tms380.c,v $
 *------------------------------------------------------------------
 * if_c3000_tms380.c - C3000 specific routines for les tms380 driver
 *
 * May 1993 - Dit Morse
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_tms380.c,v $
 * Revision 3.3.60.3  1996/08/28  12:57:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.2  1996/06/13  19:08:06  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.3.60.1  1996/03/18  20:41:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.3  1996/03/13  01:49:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.24.2  1996/03/09  05:07:35  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.24.1  1996/02/20  14:41:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:38:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/07  00:28:46  ssangiah
 * CSCdi38231:  Reserve cookie value for new development
 * Knowledge of this cookie value for the new platform has also been added
 * to the init code for the appropriate drivers so that the boot ROMs from
 * this release will initialize the corresponding interfaces on the
 * platform properly.
 *
 * Revision 2.3  1995/06/21  08:55:15  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/19  06:47:07  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:33:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* 
 * #define DEBUG_TIMES
 * #define DEBUG
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "if_les.h"
#include "if_les_tms380.h"
#include "../if/network.h"


static void init_tms380_idb (int unit)
{
    hwidbtype *idb;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    /*
     * Save our unit number
     */
    idb->unit = unit;    /* unit number */

    init_tms380_idb_common(idb);

    /*
     * Initialize the hardware:
     */
    tms380_initialize(idb);
    idb->counters.resets = 0;
}


/*
 * analyze_tms380: Discover any tokenring interfaces
 */
static int analyze_tms380 (void)
{
    int num_token_int;
 
    switch (cookie->interfaces) {
      case INTERFACE_1R1T:
      case INTERFACE_1R1E:
      case INTERFACE_1R1B:
      case INTERFACE_1R1T1B:
      case INTERFACE_1R1T1I:
      case INTERFACE_1R2T:
      case INTERFACE_1R2T1B:
      case INTERFACE_1R2T8A:
      case INTERFACE_1R2T1E:
      case INTERFACE_1R2T1B2AS:
      case INTERFACE_1R1B2W:
        num_token_int = 1;
        init_tms380_idb(0);
        break;
      case INTERFACE_2R:
      case INTERFACE_2R2T:
        num_token_int = 2;
        init_tms380_idb(0);
        init_tms380_idb(1);
        break;
      default:
        num_token_int = 0;
        break;
    }
    return(num_token_int);
}


/*
 * TMS380 subsystem initialisation entry point
 */
void tms380_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the TMS380 Token Ring interfaces in system
     */
    nTRs += analyze_tms380();
}


/************************************************************************
 *
 * Interrupt handler stuff
 *
 ************************************************************************/

/*
 * Handle Tokenring interrupts
 * This routine is in the interrupt path and is coded for speed.
 */
void tms380_interrupt (register hwidbtype *idb)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    volatile ushort *sifint;
    ushort intval;
    
    ds = INSTANCE;
    
    /*
     * Do we have an interrupt?
     */
    sif_regs = SIF_REGS;
    sifint = &sif_regs->intreg;
    intval = READ_SIF(sifint);
    
    if (intval & MADGE_SYSINT)
	tms380_sif_handler(idb, sifint, intval); /* Handle the interrupt */
    else {
	ds->active_counts.bozo_ints++;
	ds->active_counts.last_bozo = intval;
    }
}

void init_tms380_dev (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;

    /*
     * Select the proper assembly language level 4 interrupt wrapper:
     */
    if (cookie->interfaces == INTERFACE_2R2T && idb->unit == 1) {
        pan_int_array[1].function = (ulong)level4_pan_r;
        pan_int_array[1].idb1 = idb;
    } else {
        pan_int_array[0].function = (ulong)level4_pan_r;
        pan_int_array[0].idb1 = idb;
    }
    
    /*
     * Find the correct register address for the TMS380:
     */
    SIF_REGS = (struct _sifreg_t *)NULL;
    if (idb->unit == 0) {
	if ((cookie->interfaces == INTERFACE_1R1T) ||
	    (cookie->interfaces == INTERFACE_1R1E) ||
	    (cookie->interfaces == INTERFACE_2R)) {
	    SIF_REGS = (struct _sifreg_t *)ADRSPC_NETWORK_A;
	    ds->reset_mask = CONTROL_RESET_NETWORK_A;
	    network_a_idb = idb; /* Save IDB ptr for level 4 int wrapper */
	} else {
	    /*
	     * Handle cristal/cancun stuff
	     */
	    if ((cookie->interfaces == INTERFACE_1R1T1B) ||
		(cookie->interfaces == INTERFACE_1R2T)   ||
		(cookie->interfaces == INTERFACE_1R2T8A) ||
                (cookie->interfaces == INTERFACE_1R1T1I) ||
		(cookie->interfaces == INTERFACE_1R2T1B) ||
                (cookie->interfaces == INTERFACE_1R2T1E) ||
                (cookie->interfaces == INTERFACE_2R2T)   ||
                (cookie->interfaces == INTERFACE_1R2T1B2AS) ||
                (cookie->interfaces == INTERFACE_1R1B2W))  {
		SIF_REGS = (struct _sifreg_t *)ADRSPC_CRISTAL_NET_A;
		ds->reset_mask = RESET_NETWORK_A;
		network_a_idb = idb; /* Save IDB ptr for level 4 int wrapper */
	    }
	}
    } else if (idb->unit == 1) {
	if (cookie->interfaces == INTERFACE_2R) {
	    SIF_REGS = (struct _sifreg_t *)ADRSPC_NETWORK_B;
	    ds->reset_mask = CONTROL_RESET_NETWORK_B;
	    network_b_idb = idb; /* Save IDB ptr for level 4 int wrapper */
	} else {
            if (cookie->interfaces == INTERFACE_2R2T) {
                SIF_REGS = (struct _sifreg_t *)ADRSPC_CAMPECHE_NET_C;
                ds->reset_mask = RESET_NETWORK_F;
                network_b_idb = idb;
            }
        }
    }
    if (SIF_REGS == NULL) {
	if (tokenevent_debug)
	    errmsg(&msgsym(BADUNIT, TR), idb->unit);
	net_cstate(idb,IDBS_DOWN);
    }
}

/*
 * tms380_get_default_hw_address
 * Get the default hardware address of network interface.
 * Note that the cookie only contains the hardware ethernet address
 * for Ethernet unit number 0. Increment the 24 bit host number for
 * each additional unit.
 */
void tms380_get_default_hw_address (hwidbtype *idb,
				    uchar addr[IEEEBYTES])
{
    charlong host;
    int i;
    uchar *ptr;

    /* Copy the ethernet 0 hardware address into addr */
    ptr = cookie->ether0_hw_addr;
    for (i = 0; i < IEEEBYTES; i++)
	addr[i] = *ptr++;
 
    if (idb->unit) {
 
        host.d.byte[0] = 0;     /* Copy the 24 bit host number from cookie */
        for (i = 1; i < 4; i++)
            host.d.byte[i] = addr[i + 2];
 
        /* Increment the host number appropriately */
        host.d.lword += idb->unit;
 
        for (i = 1; i < 4; i++) /* Copy incremented host number to addr */
            addr[i + 2] = host.d.byte[i];
    }

    /*
     * Bit swap each byte for tokenring:
     */
    ieee_swap(addr, addr);
}

void tms380_vendor_init (hwidbtype *idb)
{ }

void tms380_set_ringspeed (hwidbtype *idb)
{
    ushort control2, *addr_ctl2;
    ushort tmap1, tmap2, *addr_tmap1, *addr_tmap2;
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;

    switch (cpu_type) {
      case CPU_CRISTAL:
      case CPU_CANCUN:
	addr_ctl2 = (ushort *)ADRSPC_RESET;
	control2 = *addr_ctl2;
        if (cookie->interfaces == INTERFACE_2R2T) {
	/*
	 * The original 2R2T was developed with the understanding
	 * that the c26 would be the chip TI continued to fab -
	 * not the c16.  Do not trust TI.  The following hack is
	 * to support both original 2R2T boards (with the c26) and new
	 * 2R2T boards with the (older) c16.  In the original 2R2T
	 * design, the speed was set via the sif->acl registers for
	 * for both interfaces.  This is not available on the the c16.
	 * For the newer 2R2Ts with the c16, int token 0 speed is set
	 * like all other 2500s while int token 1 speed is set via bit 4
	 * of ADRSPC_TOKEN_CH_C_MAP_1.  Bit 5 of ADRSPC_TOKEN_CH_C_MAP_1
	 * set signifies new 2R2T.
	 * Yuck.
	 */
	    addr_tmap1 = (ushort *)ADRSPC_TOKEN_CH_C_MAP_1;
	    tmap1 = *addr_tmap1;
	    addr_tmap2 = (ushort *)ADRSPC_TOKEN_CH_C_MAP_2;
	    tmap2 = *addr_tmap2;
	    /* Must clear ADRSPC_TOKEN_CH_C_MAP_2 before reading tmap1 */
	    /* Restore ADRSPC_TOKEN_CH_C_MAP_2 after read */
	    *(ushort *)ADRSPC_TOKEN_CH_C_MAP_2 = 0x0;
            DELAY(1);	/* Let things settle down */
	    if (tmap1 & C16_SPEED_SELECT_BIT) {  /* c16 2r2t */
		*(ushort *)ADRSPC_TOKEN_CH_C_MAP_2 = tmap2;  /* Restore */
		if (idb->unit) {
		    if (idb->tr_ring_speed == 4) { 
			tmap1 |= C16_SPEED_SET_BIT;
            		*(ushort *)ADRSPC_TOKEN_CH_C_MAP_1 = tmap1;
		    } else {
			tmap1 &= ~C16_SPEED_SET_BIT;
            		*(ushort *)ADRSPC_TOKEN_CH_C_MAP_1 = tmap1;
		    }
		} else {
		    if (idb->tr_ring_speed == 4) {
		        control2 |= RESET_RING_SPEED_1;
		    } else {
		        control2 &= ~RESET_RING_SPEED_1;
		    }
		}
	    } else {  /* c26 2r2t */
		*(ushort *)ADRSPC_TOKEN_CH_C_MAP_2 = tmap2;  /* Restore */
		ds = (tms380_instance_t *)INSTANCE;
		sif_regs = SIF_REGS;
		if (idb->tr_ring_speed == 4) {
		    sif_regs->actl |= (RING_SPEED_4 | TRACTL_ARESET);
		    sif_regs->actl &= ~TRACTL_ARESET;
		} else {
		    sif_regs->actl |= (RING_SPEED_16 | TRACTL_ARESET);
		    sif_regs->actl &= ~TRACTL_ARESET;
		}
	    }
        } else {  /* c16 25XX */
            if (idb->tr_ring_speed == 4) {
                control2 |= RESET_RING_SPEED_1;
            } else {
                control2 &= ~RESET_RING_SPEED_1;
            }
        }
        break;
      default:
        addr_ctl2 = (ushort *)ADRSPC_CONTROL2;
        control2 = *addr_ctl2;
        if (idb->tr_ring_speed == 4) {
            control2 |= CONTROL2_4M;
        } else {
            control2 &= ~CONTROL2_4M;
        }
        break;
    }
    *addr_ctl2 = control2;
    DELAY(1);	/* Let things settle down */
}


/*
 * Reset a tms380:
 * May be called from interrupt path.
 */
void tms380_reset (hwidbtype *idb)
{
    struct _sifreg_t *sif_regs;
    tms380_instance_t *ds;
    leveltype level;
    
    BEG_TIME_380("tms380_reset")
	
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    
    /* Issue a hard reset command to stop the TMS380 */
    level = raise_interrupt_level(ETHER_DISABLE);
    sif_regs->actl = TRACTL_HARDRESET;
    DELAY(1);			/* Hopefully, DMA will complete cleanly */
    
    reset_interrupt_level(level);
    END_TIME_380("tms380_reset");
}

void tms380_print_nim_id (hwidbtype *idb)
{
}
