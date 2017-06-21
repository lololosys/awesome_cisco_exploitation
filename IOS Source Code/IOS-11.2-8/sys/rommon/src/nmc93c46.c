/* $Id: nmc93c46.c,v 3.1.68.2 1996/04/20 01:10:51 szhang Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/nmc93c46.c,v $
 *------------------------------------------------------------------
 * nmc93c46.c -- NMC93C46 EEPROM Support
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nmc93c46.c,v $
 * Revision 3.1.68.2  1996/04/20  01:10:51  szhang
 * CSCdi55174:  Reset Reason Reg is not read correctly
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:27:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.3  1996/01/17  05:53:43  szhang
 * Branch: ELC_branch
 * Fixed the problem with chip select to mid-plane eeprom.
 *
 * Revision 3.1.2.2  1995/12/01  04:20:46  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:08  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:54:58  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "monitor.h"
#include "nmc93c46.h"

#define TRUE	1
#define FALSE	0
#define NULL	0

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
    wastetime(PAS_EEPROM_DELAY);
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
    wastetime(PAS_EEPROM_DELAY);
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

    wastetime(PAS_EEPROM_DELAY);
    if (pa->select_reg_width == sizeof(ushort)) {
	sreg = (volatile ushort *)pa->select_reg;
	if (onoff) {
	    *sreg |= pa->select_mask;	        /* assert the bit */
	} else {
	    *sreg &= ~(pa->select_mask);	/* de-assert the bit */
	}
    } else if (pa->select_reg_width == sizeof(ulong)) {
	lreg = (volatile ulong *)pa->select_reg;
	if (onoff) {
	    *lreg |= pa->select_mask;	        /* assert the bit */
	} else {
	    *lreg &= ~(pa->select_mask);	/* de-assert the bit */
	} 
    } else if (pa->select_reg_width == sizeof(uchar)) {
	creg = (volatile uchar *)pa->select_reg;
	if (onoff) {
	    *creg |= pa->select_mask;	        /* assert the bit */
	} else {
	    *creg &= ~(pa->select_mask);	/* de-assert the bit */
	}
    }
    wastetime(PAS_EEPROM_DELAY);
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

    wastetime(PAS_EEPROM_DELAY);
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
				pas_eeprom_type eeprom, ushort read_count)
{
    ushort *ptr = (ushort *)buf;
    int i;

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {
	/*
	 * Read the data 16 bits at a time
	 */
	for (i=0; i<read_count; i++, ptr++) {
	    if (!pas_eeprom_io(pa, eeprom, 
			       (NMC93C46_CMD_READ | (i << 3)), 9, ptr)) {
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
				 pas_eeprom_type eeprom)
{
    ushort *ptr = (ushort *)buf;
    int i;

    /* NMC93C46 EEPROM case */
    if (eeprom == PAS_EEPROM_NMC93C46) {

	/*
	 * Set the part into Write Enable mode
	 */
	pas_eeprom_io(pa, eeprom, NMC93C46_CMD_WREN, 9, NULL);

	/*
	 * Write the data 16 bits at a time
	 */
	for (i=0; i<16; i++, ptr++) {
	    if (!pas_eeprom_io(pa, eeprom,
			       (NMC93C46_CMD_WRITE | (i << 3) |
			       (*ptr << 9)), 25, NULL)) { 
		return(FALSE);
	    }
	    wastetime(PAS_NMC93C46_WRITE_DELAY);
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
     * Switch off of the command, calling the access routine
     */
    switch (cmd) {

    case PAS_EEPROM_CMD_READ: 
	return(pas_eeprom_read(pa, buf, eeprom, read_count));
	
    case PAS_EEPROM_CMD_WRITE:
	return(pas_eeprom_write(pa, buf, eeprom));

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
	return(pas_eeprom_read(pa, buf, eeprom, read_count));
	
    case PAS_EEPROM_CMD_WRITE:
	return(pas_eeprom_write(pa, buf, eeprom));

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

/* end of file */
