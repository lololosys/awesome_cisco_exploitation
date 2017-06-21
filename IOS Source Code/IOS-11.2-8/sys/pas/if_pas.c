/* $Id: if_pas.c,v 3.4.44.4 1996/07/30 01:09:12 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas.c,v $ 
 *------------------------------------------------------------------
 * if_pas.c - PCI Adaptor support routines
 *
 * January 1995 - David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas.c,v $
 * Revision 3.4.44.4  1996/07/30  01:09:12  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.4.44.3  1996/04/05  19:55:56  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.4.44.2  1996/04/03  18:25:18  dcarroll
 * CSCdi53341:  clean up tables after oir removal
 * Branch: California_branch
 * Add routines to uninstall tables.
 *
 * Revision 3.4.44.1  1996/03/21  23:23:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.6  1996/03/01  20:42:51  wfried
 * CSCdi50196:  Need to remove CPU/MP ID eeprom workarround
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/01/16  01:42:35  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.4  1996/01/03  06:10:00  wfried
 * Branch: ELC_branch
 * Add temporary HW bug workaround to allow correct PCI arbitration and
 * selection of MP EEPROM device.
 *
 * Revision 3.2.2.3  1995/12/08  05:33:28  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.2.2.2  1995/12/05  06:35:47  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.2.2.1  1995/11/22  21:41:46  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.4  1995/12/26  23:10:09  dlobete
 * CSCdi42282:  chassis mib incomplete for VIP
 * Add support for PA MIB information to be relayed from VIP to RP/RSP
 * via love-letter.
 * Rework some of the 4T PA's data structures as a result of above.
 *
 * Revision 3.3  1995/12/19  20:24:18  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.2  1995/11/17  18:54:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:32:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "packet.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/particle.h"

#include "../hes/dgbus.h"
#include "../ts/nonvol_x2444.h"

#include "if_pas.h"
#include "pas_id.h"

/*
 * Local data storage
 */
static dgbus_eeprom_t *eeprom_data_ptr[MAX_PA_BAYS];

/*
 * pas_allocate_fspak:
 * Allocate a static fastswitch paktype header
 */
paktype *pas_allocate_fspak (void)
{
    if (!fspak_pool) {
	return(NULL);
    }
    return(pool_getbuffer(fspak_pool));
}

/*
 * pas_fspak_item_ret:
 * Return an item to the FS Pak pool. Do not actually return the
 * paktype. Just return all particles and re-init the structure.
 */
static void pas_fspak_item_ret (pooltype *pool, void *item)
{
    paktype *pak = item;

    /*
     * Release all particles
     */
    particle_retbuffer(pak);

    /*
     * Zero out paktype fields
     */
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
    pak->particlequeue.qhead = pak->particlequeue.qtail = NULL;
    pak->particlequeue.count = 0;
}

/*
 * pas_init_fspak_pool: 
 * Initialize the pool of static fastswitching paktype headers
 */
void pas_init_fspak_pool (void)
{
    int pool_group;

    pool_group = pool_create_group();
    fspak_pool = pak_pool_create("FS Header", pool_group, 0,
		 POOL_DYNAMIC | POOL_PUBLIC_DUPLICATE | POOL_INVISIBLE,
		 NULL);
    if (fspak_pool) {
	pool_adjust(fspak_pool, 16, 128, 16, TRUE);
	fspak_pool->item.ret = pas_fspak_item_ret;
    }
}

/*
 * pas_instance_init_common:
 * Initialize common fields in the common part of every device's
 * instance data structure
 */
void pas_instance_init_common (pas_cbtype *pascb, hwidbtype *idb)
{
    pascb->idb = idb;
    pascb->pa_bay = idb->slot;
    pascb->pa_device = idb->subunit;
    pascb->pa_ifindex = (pascb->pa_bay * MAX_INTERFACES_PER_PA) + 
	pascb->pa_device;
}

/*
 * pas_eeprom_clock :
 * Control the clock bit (assume positive polarity)
 */
static void pas_eeprom_clock (pas_management_t *pa, boolean onoff)
{
    volatile ushort *sreg;
    volatile ulong *lreg;
    volatile uchar *creg;

    if (pa->clk_reg_width == sizeof(ushort)) {
	sreg = (volatile ushort *)pa->clk_reg;
	if (onoff) {
	    *sreg |= pa->clk_mask;		/* assert the bit */
	} else {
	    *sreg &= ~(pa->clk_mask);		/* de-assert the bit */
	}
    } else if (pa->clk_reg_width == sizeof(ulong)) {
	lreg = (volatile ulong *)pa->clk_reg;
	if (onoff) {
	    *lreg |= pa->clk_mask;		/* assert the bit */
	} else {
	    *lreg &= ~(pa->clk_mask);		/* de-assert the bit */
	}
    } else if (pa->clk_reg_width == sizeof(uchar)) {
	creg = (volatile uchar *)pa->clk_reg;
	if (onoff) {
	    *creg |= pa->clk_mask;		/* assert the bit */
	} else {
	    *creg &= ~(pa->clk_mask);		/* de-assert the bit */
	}
    }
    usecdelay(PAS_EEPROM_DELAY);
}

/*
 * pas_eeprom_clock_cycle :
 * Send one clock cycle to the ID EEPROM
 */
static void pas_eeprom_clock_cycle (pas_management_t *pa)
{
    pas_eeprom_clock(pa, TRUE);
    pas_eeprom_clock(pa, FALSE);
}

/*
 * pas_eeprom_datain :
 * Control the data-in bit (assume positive polarity)
 */
static void pas_eeprom_datain (pas_management_t *pa, boolean onoff)
{
    volatile ushort *sreg;
    volatile ulong *lreg;
    volatile uchar *creg;

    if (pa->datain_reg_width == sizeof(ushort)) {
	sreg = (volatile ushort *)pa->datain_reg;
	if (onoff) {
	    *sreg |= pa->datain_mask;		/* assert the bit */
	} else {
	    *sreg &= ~(pa->datain_mask);	/* de-assert the bit */
	}
    } else if (pa->datain_reg_width == sizeof(ulong)) {
	lreg = (volatile ulong *)pa->datain_reg;
	if (onoff) {
	    *lreg |= pa->datain_mask;		/* assert the bit */
	} else {
	    *lreg &= ~(pa->datain_mask);	/* de-assert the bit */
	}
    } else if (pa->datain_reg_width == sizeof(uchar)) {
	creg = (volatile uchar *)pa->datain_reg;
	if (onoff) {
	    *creg |= pa->datain_mask;		/* assert the bit */
	} else {
	    *creg &= ~(pa->datain_mask);	/* de-assert the bit */
	}
    }
    usecdelay(PAS_EEPROM_DELAY);
}

/*
 * pas_eeprom_select :
 * Control the select bit (assume positive polarity)
 */
static void pas_eeprom_select (pas_management_t *pa, boolean onoff)
{
    volatile ushort *sreg;
    volatile ulong *lreg;
    volatile uchar *creg;

    usecdelay(PAS_EEPROM_DELAY);
    if (pa->select_reg_width == sizeof(ushort)) {
	sreg = (volatile ushort *)pa->select_reg;
	if (onoff) {
	    *sreg |= pa->select_mask;		/* assert the bit */
	} else {
	    *sreg &= ~(pa->select_mask);	/* de-assert the bit */
	}
    } else if (pa->select_reg_width == sizeof(ulong)) {
	lreg = (volatile ulong *)pa->select_reg;
	if (onoff) {
	    *lreg |= pa->select_mask;		/* assert the bit */
	} else {
	    *lreg &= ~(pa->select_mask);	/* de-assert the bit */
	}
    } else if (pa->select_reg_width == sizeof(uchar)) {
	creg = (volatile uchar *)pa->select_reg;
	if (onoff) {
	    *creg |= pa->select_mask;		/* assert the bit */
	} else {
	    *creg &= ~(pa->select_mask);	/* de-assert the bit */
	}
    }
    usecdelay(PAS_EEPROM_DELAY);
}

/*
 * pas_eeprom_dataout :
 * Read the dataout bit (assume positive polarity)
 */
static int pas_eeprom_dataout (pas_management_t *pa)
{
    volatile ushort *sreg;
    volatile ulong *lreg;
    volatile uchar *creg;

    usecdelay(PAS_EEPROM_DELAY);
    if (pa->dataout_reg_width == sizeof(ushort)) {
	sreg = (volatile ushort *)pa->dataout_reg;
	return(*sreg & pa->dataout_mask);
    } else if (pa->dataout_reg_width == sizeof(ulong)) {
	lreg = (volatile ulong *)pa->dataout_reg;
	return(*lreg & pa->dataout_mask);
    } else if (pa->dataout_reg_width == sizeof(uchar)) {
	creg = (volatile uchar *)pa->dataout_reg;
	return(*creg & pa->dataout_mask);
    }
    return(0);
}

/*
 * pas_eeprom_read_sixteen : 
 * Read sixteen bits of info from the eeprom
 */
static ushort pas_eeprom_read_sixteen (pas_management_t *pa, 
				       pas_eeprom_type eeprom)
{
    ushort value = 0;
    int i;

    if (eeprom == PAS_EEPROM_X2444) {
	/*
	 * X2444 device
	 */
	pas_eeprom_datain(pa, TRUE);		/* set data in */
	for (i=0; i<16; i++) {
	    if (pas_eeprom_dataout(pa)) {	/* read dataput */
		value |= (1 << i);
	    }
	    pas_eeprom_clock(pa, TRUE);		/* activate clock bit */
	    pas_eeprom_clock(pa, FALSE);	/* de-activate the clock bit */
	}
	return(value);

    } else {
	/*
	 * NMC93C46 device
	 */
	for (i=0; i<16; i++) {
	    pas_eeprom_clock(pa, TRUE);		/* enable clock */
	    if (pas_eeprom_dataout(pa)) {	/* read dataout */
		value |= (1 << i);
	    }
	    pas_eeprom_clock(pa, FALSE);	/* disable clock */
	}
	return(value);
    }
}

/*
 * pas_eeprom_io :
 * Perform low-level commands on the EEPROM
 */
static boolean pas_eeprom_io (pas_management_t *pa, 
			      pas_eeprom_type eeprom,
			      int cmd, 
			      int write_bits, 
			      ushort *read_ptr)
{
    int i;

    /*
     * Enable the EEPROM (common across both X2444 and NMC93C46)
     */
    pas_eeprom_clock(pa, FALSE);		/* clear clock */
    pas_eeprom_datain(pa, FALSE);		/* clear datain  */
    pas_eeprom_select(pa, TRUE);		/* set chip select */

    /*
     * Actually send the command to the device
     */
    for (i=0; i<write_bits; i++) {
	pas_eeprom_datain(pa, (cmd & 0x01));	/* send the command bit */
	pas_eeprom_clock_cycle(pa);		/* pulse the clock */
	cmd >>= 1;
    }

    /*
     * If the read_ptr is Null, then there is no read-back
     */
    if (read_ptr == NULL) {
	pas_eeprom_select(pa, FALSE);		/* clear chip select */
	return(TRUE);
    }

    /*
     * We have done something that requires a read back. Read back 16 bits
     * and stick the answer in read_ptr
     */
    *read_ptr = pas_eeprom_read_sixteen(pa, eeprom);
    pas_eeprom_select(pa, FALSE);	      	/* de-select the device */
    return(TRUE);
}

/*                                    
 * pas_eeprom_read :
 * Attempt to read 32 bytes from the PA's ID eeprom
 */
static boolean pas_eeprom_read (pas_management_t *pa, void *buf,
				pas_eeprom_type eeprom, ushort read_count,
                                ushort read_offset)
{
    ushort *ptr = (ushort *)buf;
    int i;
    int cmd;

    /* X2444 EEPROM case */
    if (eeprom == PAS_EEPROM_X2444) {
	
	/*
	 * Recall the data
	 */
	pas_eeprom_io(pa, eeprom, X2444_CMD_RCL, 8, NULL);
	usecdelay(PAS_X2444_DELAY);

	/*
	 * Read the data 16 bits at a time
	 */
	for (i=0; i<read_count; i++, ptr++) {
            cmd = X2444_CMD_READ | ((i + read_offset) * 2);
	    if (!pas_eeprom_io(pa, eeprom, cmd, 8, ptr)) {
		return(FALSE);
	    }
	}
	return(TRUE);
    }

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {
	/*
	 * Read the data 16 bits at a time
	 */
	for (i=0; i<read_count; i++, ptr++) {
            cmd = NMC93C46_CMD_READ | ((i + read_offset) << 3);
	    if (!pas_eeprom_io(pa, eeprom, cmd, 9, ptr)) {
		return(FALSE);
	    }
	}
	return(TRUE);
    }
    
    /* Unknown EEPROM type */
    return(FALSE);
}

/*
 * pas_eeprom_write :
 * Attempt to write 32 bytes to the PA's ID eeprom
 */
static boolean pas_eeprom_write (pas_management_t *pa, void *buf,
        			 pas_eeprom_type eeprom, ushort write_count,
                                 ushort write_offset)
{
    ushort *ptr = (ushort *)buf;
    int i;
    int cmd;

    /* X2444 EEPROM case */
    if (eeprom == PAS_EEPROM_X2444) {

	/*
	 * Put x2444 in Write Enable mode
	 */
	pas_eeprom_io(pa, eeprom, X2444_CMD_WREN, 8, NULL);
	usecdelay(PAS_X2444_DELAY);

	/*
	 * Write the data 16 bits at a time
	 */
	for (i=0; i<write_count; i++, ptr++) {
            cmd = X2444_CMD_WRITE | ((i + write_offset) * 2) | (*ptr << 8);
	    if (!pas_eeprom_io(pa, eeprom, 
			       cmd, 
			       24, NULL)) {
		return(FALSE);
	    }
	}

	if (!pas_eeprom_io(pa, eeprom, X2444_CMD_STO, 8, NULL)) {
	    return(FALSE);
	}
	usecdelay(PAS_X2444_DELAY);
	if (!pas_eeprom_io(pa, eeprom, X2444_CMD_WRDS, 8, NULL)) {
	    return(FALSE);
	}
	return(TRUE);
    }

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {

	/*
	 * Set the part into Write Enable mode
	 */
	pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WREN, 9, NULL);

	/*
	 * Write the data 16 bits at a time
	 */
	for (i=0; i<write_count; i++, ptr++) {
            cmd = NMC93C46_CMD_WRITE | ((i + write_offset) << 3) | (*ptr << 9);
	    if (!pas_eeprom_io(pa, eeprom, cmd, 25, NULL)) { 
		return(FALSE);
	    }
	    usecdelay(PAS_NMC93C46_WRITE_DELAY);
	}

	/*
	 * Disable Write mode
	 */
	pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WRDS, 9, NULL);
	return(TRUE);
    }
    
    /* Unknown EEPROM type */
    return(FALSE);
}

/*
 * pas_eeprom_erase :
 * Attempt to erase the PA's ID eeprom
 */
static boolean pas_eeprom_erase (pas_management_t *pa, void *buf,
				 pas_eeprom_type eeprom)
{
    ushort temp[16];
    int i;

    /* X2444 EEPROM case : do it by hand */
    if (eeprom == PAS_EEPROM_X2444) {
	for (i=0; i<16; i++) {
	    temp[i] = 0xffff;
	}
	return(pas_eeprom_write(pa, temp, eeprom, 16, 0));
    }

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {

	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WREN, 9, NULL)) {
	    return(FALSE);
	}
	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_ERASE_ALL, 9, NULL)) {
	    return(FALSE);
	}
	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WRDS, 9, NULL)) {
	    return(FALSE);
	}
	return(TRUE);
    }

    /* Unknown EEPROM type */
    return(FALSE);
}

/*
 * pas_eeprom_zero :
 * Attempt to zero the PA's ID eeprom
 */
static boolean pas_eeprom_zero (pas_management_t *pa, void *buf,
				pas_eeprom_type eeprom)
{
    ushort temp[16];
    int i;

    /* X2444 EEPROM case : do it by hand */
    if (eeprom == PAS_EEPROM_X2444) {
	for (i=0; i<16; i++) {
	    temp[i] = 0x00;
	}
	return(pas_eeprom_write(pa, temp, eeprom, 16, 0));
    }

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {

	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WREN, 9, NULL)) {
	    return(FALSE);
	}
	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WRITE_ALL, 25, NULL)) {
	    return(FALSE);
	}
	if (!pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WRDS, 9, NULL)) {
	    return(FALSE);
	}
	return(TRUE);
    }

    /* Unknown EEPROM type */
    return(FALSE);
}

/*
 * pas_access_ideeprom :
 * Access the ID eeprom on a port adaptor
 */
boolean pas_access_ideeprom (int pa_bay, void *buf,
			     pas_eeprom_type eeprom,
			     pas_eeprom_cmd_t cmd,
			     ushort read_count)
{
    pas_management_t *pa;

    /*
     * Get the management data structure and verify the PA exists
     */
    if (pa_bay >= MAX_PA_BAYS) {
	return(FALSE);
    }
    pa = pas_management_list[pa_bay];
    if (!pa || !pas_pa_present(pa_bay)) {
	return(FALSE);
    }

    /*
     * Switch off of the command, calling the access routine
     */
    switch (cmd) {

    case PAS_EEPROM_CMD_READ: 
	return(pas_eeprom_read(pa, buf, eeprom, read_count, 0));
	
    case PAS_EEPROM_CMD_WRITE:
	return(pas_eeprom_write(pa, buf, eeprom, 16, 0));

    case PAS_EEPROM_CMD_ERASE:
	return(pas_eeprom_erase(pa, buf, eeprom));

    case PAS_EEPROM_CMD_ZERO:
	return(pas_eeprom_zero(pa, buf, eeprom));
    }
   
    /*
     * Unrecognized command, return failure
     */
    return(FALSE);
}

/*
 * pas_access_sys_eeprom :
 * Access an EEPROM not associated with a bay
 */
boolean pas_access_sys_eeprom (pas_management_t *pa,
			       void *buf,
			       pas_eeprom_type eeprom,
			       pas_eeprom_cmd_t cmd,
			       ushort read_count)
{
    /*
     * Switch off of the command, calling the access routine
     */
    switch (cmd) {

    case PAS_EEPROM_CMD_READ: 
	return(pas_eeprom_read(pa, buf, eeprom, read_count, 0));
	
    case PAS_EEPROM_CMD_WRITE:
	return(pas_eeprom_write(pa, buf, eeprom, 16, 0));

    case PAS_EEPROM_CMD_ERASE:
	return(pas_eeprom_erase(pa, buf, eeprom));

    case PAS_EEPROM_CMD_ZERO:
	return(pas_eeprom_zero(pa, buf, eeprom));
    }
   
    /*
     * Unrecognized command, return failure
     */
    return(FALSE);
}

/*
 * pas_access_sys_eeprom_partial :
 * Access to a portion of an EEPROM not associated with a bay.
 * Read and write are the only commands supported.
 * Note count and offset are in 2-byte words.
 */
boolean pas_access_sys_eeprom_partial (pas_management_t *pa,
			               void *buf,
			               pas_eeprom_type eeprom,
			               pas_eeprom_cmd_t cmd,
			               ushort count,
                                       ushort offset)
{
    /*
     * Switch off of the command, calling the access routine
     */
    switch (cmd) {
    case PAS_EEPROM_CMD_READ: 
	return(pas_eeprom_read(pa, buf, eeprom, count, offset));
	                                                     
    case PAS_EEPROM_CMD_WRITE:                               
	return(pas_eeprom_write(pa, buf, eeprom, count, offset));    
    }
   
    /*
     * Unrecognized command, return failure
     */
    return(FALSE);
}

/*
 * pas_get_eeprom_data : 
 * Read the ID eeprom, either as an NMC93C46 or X2444
 */
void pas_get_eeprom_data (uint pa_bay, void *ptr,
				 ushort read_count)
{
    dgbus_eeprom_t *dg_ptr=ptr;

    /*
     * Try and read the EEPROM data as an NMC93C46 device
     */
    pas_access_ideeprom(pa_bay, dg_ptr, PAS_EEPROM_NMC93C46, 
			PAS_EEPROM_CMD_READ, read_count);
    if (dg_ptr->controller_type >= PA_NUM_HARDWARE_TYPES) {
	/*
	 * Failed to access it as an NMC93C46. Try accessing
	 * it as an X2444 device
	 */
	pas_access_ideeprom(pa_bay, dg_ptr, PAS_EEPROM_X2444, 
			    PAS_EEPROM_CMD_READ, read_count);
    }
}

/*
 * get_pa_type :
 * Get the PA type of the specified port using the onboard eeprom
 */
ushort get_pa_type (uint pa_bay)
{
    dgbus_eeprom_t *ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	return(PA_HARDWARE_UNKNOWN);
    }
    ptr = eeprom_data_ptr[pa_bay];

    if (!ptr) {
	ptr = malloc(sizeof(dgbus_eeprom_t));
	if (!ptr) {
	    return(PA_HARDWARE_UNKNOWN);
	}
	eeprom_data_ptr[pa_bay] = ptr;

	/*
	 * To save init time, we will only read one 16 bit value from
	 * the PA ID eeprom
	 */
	pas_get_eeprom_data(pa_bay, ptr, 1);
    }
    return(ptr->controller_type);
}

/*
 * get_pa_version :
 * Get the PA version of the specified port using the onboard cookie
 */
ushort get_pa_version (uint pa_bay)
{
    dgbus_eeprom_t *ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	return(0xffff);
    }
    ptr = eeprom_data_ptr[pa_bay];
    if (!ptr) {
	ptr = malloc(sizeof(dgbus_eeprom_t));
	if (!ptr) {
	    return(0xffff);
	}
	eeprom_data_ptr[pa_bay] = ptr;
	pas_get_eeprom_data(pa_bay, ptr, 8);
    }
    return((ptr->hardware_version_high<<8) | ptr->hardware_version_low);
}

/*
 * pas_oir_cleanup:
 * Clean up the eeprom copy, the jumptable 
 * and the nmi handler table.
 */
void pas_oir_cleanup (uint pa_bay)
{
    dgbus_eeprom_t *eeprom_ptr;

    if (pa_bay >= MAX_PA_BAYS) {
	return;
    }

    /*
     * Clear and free the copy of the eeprom data.
     */
    eeprom_ptr = eeprom_data_ptr[pa_bay];
    if (!eeprom_ptr) {
	return;
    }
    free(eeprom_ptr);
    eeprom_data_ptr[pa_bay] = NULL;

    /*
     * Reset jumptable and nmi handler table.
     */
    pas_uninstall_jumptable_entry(pa_bay);
    pas_uninstall_pa_nmi_handler(pa_bay);
}

/*
 * show_pci:
 * Print out details of the system pci controller
 */
void show_pci (parseinfo *csb)
{
    /*
     * Print out PCI asic/controller summary
     */
    pas_pci_controller_summary();
}

/* end of file */

