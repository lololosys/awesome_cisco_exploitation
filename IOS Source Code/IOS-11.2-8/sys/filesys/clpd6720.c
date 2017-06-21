/* $Id: clpd6720.c,v 3.2.60.1 1996/03/18 19:35:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/clpd6720.c,v $
 *------------------------------------------------------------------
 * CLPD6720 PCMCIA controller chip driver
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clpd6720.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:56:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/05  01:52:05  mcieslak
 * Removed redundant include's.
 * CSCdi38262:  Need to clean-up code review issues found in alaska merge
 *
 * Revision 2.2  1995/07/07  22:56:58  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.1  1995/06/07  20:31:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../h/master.h"

#if defined(RSP_ROM)
#include "mon_ralib.h"
#endif

#include "../src-rsp/rsp_pcmap.h"
#include "clpd6720.h"
#include "clpd6720_internals.h"
#include "ralib_internals.h"
#include "fslib_print.h"
#include "../hes/dgbus.h"
#include "../hes/if_fci.h"
#include "../src-rsp/rsp_flashmib.h"


/*----------------------------------------------------------------------------*
 * Declarations of the pointers to the register read and write
 * function for the CLPD6720.  They are different for RSP/1 and RSP/2
 * and are set to point to the correct routine for the platform at
 * initialization time.
 *----------------------------------------------------------------------------*/
void (*clpd6720_reg_write)(clpd6720_socket_t socket, uchar reg_index, uchar value);
uchar (*clpd6720_reg_read)(clpd6720_socket_t socket, uchar reg_index);


/******************************************************************************
 * Name: clpd6720_reg_write_rsp1
 *
 * Description: writes to a register on the Cirrus CLPD6720
 *
 * Input:
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   reg_index -- the register number
 *   value -- the 8-bit value to write to the register
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_reg_write_rsp1 (clpd6720_socket_t socket, uchar reg_index, uchar value)
{
    uchar index;

    if (socket == CLPD6720_SOCKET_0) {
        index = reg_index + SOCKET_0_START_INDEX;
    } else {
        index = reg_index + SOCKET_1_START_INDEX;
    }

    /*
     * This is the way the registers MUST be read -- do not change
     * this code unless you fully understand the quirks of the hardware.
     * See note above for details.
     */
    *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_INDEX_REG_RSP1)) = index;
    *((volatile ushort *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_DATA_REG_RSP1)) = (ushort)value;


    PRT(P23INT,("CL_reg_write(s=%d, reg %2x, val = %2x)\n",socket,reg_index,value));
}


/******************************************************************************
 * Name: clpd6720_reg_read_rsp1
 *
 * Description: reads from a register on the Cirrus CLPD6720
 *
 * Input:
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   reg_index -- the register number
 *
 * Return Value:
 *   value read from the register
 *
 *******************************************************************************/
uchar clpd6720_reg_read_rsp1 (clpd6720_socket_t socket, uchar reg_index)
{
    uchar value;
    uchar index;

    if (socket == CLPD6720_SOCKET_0) {
        index = reg_index + SOCKET_0_START_INDEX;
    } else {
        index = reg_index + SOCKET_1_START_INDEX;
    }

    /*
     * This is the way the registers MUST be read -- do not change
     * this code unless you fully understand the quirks of the hardware.
     * See note above for details.
     */
    *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_INDEX_REG_RSP1)) = index;
    value = (uchar)(*((volatile ushort *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_DATA_REG_RSP1)));

    PRT(P23INT,("CL_reg_read(s=%d, reg %2x) = %2x)\n",socket,reg_index,value));

    return(value);
}


/******************************************************************************
 * Name: clpd6720_reg_write_rsp2
 *
 * Description: writes to a register on the Cirrus CLPD6720
 *
 * Input:
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   reg_index -- the register number
 *   value -- the 8-bit value to write to the register
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_reg_write_rsp2 (clpd6720_socket_t socket, uchar reg_index, uchar value)
{
    uchar index;

    if (socket == CLPD6720_SOCKET_0) {
        index = reg_index + SOCKET_0_START_INDEX;
    } else {
        index = reg_index + SOCKET_1_START_INDEX;
    }

    /*
     * This is the way the registers MUST be read -- do not change
     * this code unless you fully understand the quirks of the hardware.
     * See note above for details.
     */
    *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_INDEX_REG_RSP2)) = index;
    *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_DATA_REG_RSP2)) = value;

    PRT(P23INT,("CL_reg_write(s=%d, reg %2x, val = %2x)\n",socket,reg_index,value));
}


/******************************************************************************
 * Name: clpd6720_reg_read_rsp2
 *
 * Description: reads from a register on the Cirrus CLPD6720
 *
 * Input:
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   reg_index -- the register number
 *
 * Return Value:
 *   value read from the register
 *
 *******************************************************************************/
uchar clpd6720_reg_read_rsp2 (clpd6720_socket_t socket, uchar reg_index)
{
    uchar value;
    uchar index;

    if (socket == CLPD6720_SOCKET_0) {
        index = reg_index + SOCKET_0_START_INDEX;
    } else {
        index = reg_index + SOCKET_1_START_INDEX;
    }

    /*
     * This is the way the registers MUST be read -- do not change
     * this code unless you fully understand the quirks of the hardware.
     * See note above for details.
     */
    *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_INDEX_REG_RSP2)) = index;
    value = *((volatile uchar *) KUSEG_TO_KSEG1(ADRSPC_PCMCIA_DATA_REG_RSP2));

    PRT(P23INT,("CL_reg_read(s=%d, reg %2x) = %2x)\n",socket,reg_index,value));

    return(value);
}


/******************************************************************************
 * Name: clpd6720_init
 *
 * Description: initializes the CLPD6720 for operation
 *
 * Input: None
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_init (void)
{
    clpd6720_socket_t  socket;
    uchar interface_status;
    boolean rsp2;
    dgbus_eeprom_t *idprom;
 
    /* 
     * Find out if we are running on an RSP1 or an RSP2 and
     * point at the correct low level register read and write
     * functions.
     */
    idprom = (dgbus_eeprom_t*) KUSEG_TO_KSEG1(ADRSPC_IDPROM);

    rsp2 = ((idprom->eeprom_version == 1) &&
         (idprom->controller_type == FCI_RSP2_CONTROLLER));
    if (rsp2) {
	clpd6720_reg_read = clpd6720_reg_read_rsp2;
	clpd6720_reg_write = clpd6720_reg_write_rsp2;
    } else {
	clpd6720_reg_read = clpd6720_reg_read_rsp1;
	clpd6720_reg_write = clpd6720_reg_write_rsp1;
    }

    /* 
     * do this for both sockets.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) {
	/*
	 * set up the CLPD6720 stuff that never changes, and does not
	 * depend on a window
	 */
	clpd6720_reg_write(socket, POWER_CTRL_IDX, VPP1_POWER_OFF |
			   CARD_ENABLE | VCC_POWER_ENABLE);
	
	clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX,
			   MGMT_INT_ON_INTR | CARD_RESET_RELEASE);

/*
 *	Enable card insert/remove interrupt only.
 *
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX,
			   CARD_DETECT_CHANGE_ENABLE |
			   READY_CHANGE_ENABLE);
 *
 */
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX,
			   CARD_DETECT_CHANGE_ENABLE);

	clpd6720_reg_write(socket, MISC_CTRL_1_IDX,
			   VCC_5V | MGMT_INT_LEVEL | SPEAKER_DISABLE | INPACK_IGNORED);
	
	clpd6720_reg_write(socket, MISC_CTRL_2_IDX, 
			   BYPASS_FREQ_SYNTH |
			   RUN_CLOCK_ALWAYS | 
			   NORMAL_POWER_MODE | 
			   VDD_5V_CORE_MODE |
			   IRQ12_NORMAL_MODE |
			   BIT7_NORMAL_MODE |
			   NON_DMA_MODE |
			   IRQ15_NORMAL_MODE);
	
	clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, MAP_ALL_DISABLE);


	/* 
	 * Get the "ready" and "present" status of the card(s)
	 */
	interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);

	/*
	 * Check the card detect status.
	 */
	if ((interface_status & CD_MASK) == CD_CARD_PRESENT) {
	    ralib_event_card_inserted(socket);
	} else {
	    ralib_event_card_removed(socket);
	}

	/*
	 * Check if the card is ready
	 */
	if (interface_status & READY_MASK) {
	    ralib_event_card_ready(socket);
	} else {
	    ralib_event_card_busy(socket);
	}
    }
}


/******************************************************************************
 * Name: clpd6720_mgmt_intr_handler
 *
 * Description: handles the CLPD6720 management interrupt
 *
 * Input:  None
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_mgmt_intr_handler (void)
{
    clpd6720_socket_t  socket;
    uchar status_change;
    uchar interface_status;
#if !defined(FSLIB) && !defined(RSP_ROM)
    leveltype level = 0;

    if (onintstack() == 0)
	level = raise_interrupt_level(ALL_DISABLE);
#endif

    /* 
     * find out what kind of event occurred
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) {
	status_change = clpd6720_reg_read(socket, CARD_STATUS_CHANGE_IDX);
	interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);

	/*
	 * XXX Jack, this is where we should be doing things based on
	 * the status change register, but that doesn't seem to work
	 * right.
	 * if (status_change & READY_CHANGE_MASK) {
	 *
	 * Check to see if the card ready status has changed.  If so,
         * find out what the present ready status is, and act on it.
	 */
	if (interface_status & READY_MASK) {
	    ralib_event_card_ready(socket);
	} else {
	    ralib_event_card_busy(socket);
	}

	if (interface_status & SOCKET_WRITE_PROTECT_MASK) {
	    ralib_event_card_protected(socket);
	} else {
	    ralib_event_card_writable(socket);
	}

	/*
	 * XXX Jack, this is where we should be doing things based on
	 * the status change register, but that doesn't seem to work
	 * right.
	 * if (status_change & CARD_DETECT_CHANGE_MASK) {
	 *
	 * Check to see if the card detect status has changed.  If so,
         * find out what the present card detect status is, and act on it.
	 */
	if ((interface_status & CD_MASK) == CD_CARD_PRESENT) {
	    /* 
	     * a card has been inserted
	     */
	    ralib_event_card_inserted(socket);
	} else {
	    /*
	     * card status has made one of the following transitions
	     *     PRESENT --> PARTIALLY_PRESENT
	     *     PRESENT --> REMOVED
	     *     PARTIALLY_PRESENT --> REMOVED
	     *     PARTIALLY_PRESENT --> PARTIALLY_PRESENT (other state)
	     * In any case, the card is not present, we just don't
	     * really know that is was present before the change.
	     */
	    ralib_event_card_removed(socket);
	}
    }

#if !defined(FSLIB) && !defined(RSP_ROM)
    if (onintstack() == 0)
	reset_interrupt_level(level);
#endif
}



/******************************************************************************
 * Name: clpd6720_set_window
 *
 * Description: sets a window to the address and length specified
 *
 * Input: 
 *   socket -- the socket identifier
 *   window -- the window number 
 *   local_addr -- the address in local memory of the start of the window
 *                 (0 to 16 MBytes -- the base address is automatically added)
 *                 This value can contain the upper bits of the address, but
 *                 all bits above the 16 MByte line are ignored.
 *   pcmcia_addr -- the address in PCMCIA memory of the start of the window
 *                  (0 to 64 MBytes)
 *   length -- the length (in bytes) of the window (0 to 16 MBytes)
 *   pcmcia_type -- device memory or attribute memory 
 *   window_width -- the width of the data for the window (8 or 16 bit)
 *   write_protect -- whether to write-protect the window 
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_set_window (clpd6720_socket_t socket, 
			  clpd6720_window_t window, 
			  ulong local_addr, 
			  ulong pcmcia_addr, 
			  ulong length, 
			  clpd6720_pcmcia_type_t pcmcia_type, 
			  clpd6720_window_width_t window_width, 
			  clpd6720_write_protect_t write_protect)
{

    ulong start_addr;
    ulong end_addr;
    int offset;
    uchar start_addr_lo;
    uchar start_addr_lo_idx;
    uchar start_addr_hi;
    uchar start_addr_hi_idx;
    uchar end_addr_lo;
    uchar end_addr_lo_idx;
    uchar end_addr_hi;
    uchar end_addr_hi_idx;
    uchar offset_lo;
    uchar offset_lo_idx;
    uchar offset_hi;
    uchar offset_hi_idx;

#if DEBUG
    if (length > CLPD6720_MAX_WINDOW_LENGTH) {
	/* XXX error */ 
    }

    if ((local_addr + length) > CLPD6720_MAX_LOCAL_ADDR) {
	/* XXX error */ 
    }

    if (local_addr & ~CLPD6720_LOCAL_ADDR_MASK) {
	/* XXX error */ 
    }

    if ((pcmcia_addr + length) > CLPD6720_MAX_PCMCIA_ADDR) {
	/* XXX error */ 
    }

    if (pcmcia_addr & ~CLPD6720_PCMCIA_ADDR_MASK) {
	/* XXX error */ 
    }

#endif

    /* 
     * find the base address of the register for the window selected.
     */
    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0:
	start_addr_lo_idx = MAP_0_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_1:
	start_addr_lo_idx = MAP_1_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_2:
	start_addr_lo_idx = MAP_2_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_3:
	start_addr_lo_idx = MAP_3_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_4:
	start_addr_lo_idx = MAP_4_START_LO_IDX;
	break;

    }
    /*
     * This relies on the numerical relationship of the register indicies,
     * but they are fixed for this part, so this is an efficient way to do
     * this.
     */
    start_addr_hi_idx = start_addr_lo_idx + 1;
    end_addr_lo_idx = start_addr_lo_idx + 2;
    end_addr_hi_idx = start_addr_lo_idx + 3;
    offset_lo_idx = start_addr_lo_idx + 4;
    offset_hi_idx = start_addr_lo_idx + 5;

    /*
     * Mask off the parts of the addresses that don't matter
     */
    local_addr &= LOCAL_ADDR_MASK;
    pcmcia_addr &= PCMCIA_ADDR_MASK;

    /*
     * convert the local addr and pcmcia addr to start, end and offset for
     * the controller chip
     */
    start_addr = local_addr;
    end_addr = start_addr + length - 1;
    offset = pcmcia_addr - local_addr;
    
    /*
     * separate the start, end and offset into hi and lo  parts for the 
     * controller
     */
    start_addr_lo = (start_addr & WINDOW_ADDR_LO_MASK) >> WINDOW_ADDR_LO_SHIFT;
    start_addr_hi = (start_addr & WINDOW_ADDR_HI_MASK) >> WINDOW_ADDR_HI_SHIFT;
    end_addr_lo = (end_addr & WINDOW_ADDR_LO_MASK) >> WINDOW_ADDR_LO_SHIFT;
    end_addr_hi = (end_addr & WINDOW_ADDR_HI_MASK) >> WINDOW_ADDR_HI_SHIFT;
    offset_lo = (offset & WINDOW_OFFSET_LO_MASK) >> WINDOW_OFFSET_LO_SHIFT;
    offset_hi = (offset & WINDOW_OFFSET_HI_MASK) >> WINDOW_OFFSET_HI_SHIFT;
    
    /* 
     * set the extra control bits in the register values, and preserve the
     * timing values from the current setting
     */
    if (pcmcia_type == CLPD6720_ATTRIB_MEM)
	offset_hi |= ATTRIBUTE_MEM;
    
    if (write_protect == CLPD6720_READ_ONLY)
	offset_hi |= WRITE_PROTECT;
    
    if (window_width == CLPD6720_WIDTH_16_BIT)
	start_addr_hi |= MEM_WINDOW_DATA_SIZE_16_BIT;
    
    end_addr_hi |= (clpd6720_reg_read(socket, end_addr_lo_idx) & CARD_TIMER_SELECT_MASK);
    
    /*
     * write the values to the registers
     */
    clpd6720_reg_write(socket, start_addr_hi_idx, start_addr_hi);
    clpd6720_reg_write(socket, start_addr_lo_idx, start_addr_lo);
    clpd6720_reg_write(socket, end_addr_hi_idx, end_addr_hi);
    clpd6720_reg_write(socket, end_addr_lo_idx, end_addr_lo);
    clpd6720_reg_write(socket, offset_hi_idx, offset_hi);
    clpd6720_reg_write(socket, offset_lo_idx, offset_lo);

    PRT(P22,("CL_set_window(s=%d, w=%d, la=%lx, pa=%lx, l=%lx, t=%s, w=%d, wp=%s)\n",
	     socket, window, local_addr, pcmcia_addr, length, 
	     (pcmcia_type == CLPD6720_COMMON_MEM) ? "C": "A", 
	     (window_width == CLPD6720_WIDTH_8_BIT) ? 8: 16,
	     (write_protect == CLPD6720_READ_ONLY) ? "R/O": "R/W"));

}


/******************************************************************************
 * Name: clpd6720_set_window_timing
 *
 * Description: assigns a window one of the sets of timing parameters
 *
 * Input: 
 *   socket -- the socket identifier (SOCKET_0 or SOCKET_1)
 *   window -- the window number (WINDOW_0 to WINDOW_4)
 *   timing_set -- the timing set to use for the window indicated (0 or 1) 
 *                    (TRUE=read-only, FALSE=writable)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_set_window_timing (clpd6720_socket_t socket, 
				 clpd6720_window_t window, 
				 clpd6720_timing_t timing)
{
    uchar timing_select_idx;
    uchar timing_select;

    /*
     * Find the register that has the timing bits for the window
     */
    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0:
	timing_select_idx = MAP_0_END_HI_IDX;
	break;
    case CLPD6720_WINDOW_1:
	timing_select_idx = MAP_1_END_HI_IDX;
	break;
    case CLPD6720_WINDOW_2:
	timing_select_idx = MAP_2_END_HI_IDX;
	break;
    case CLPD6720_WINDOW_3:
	timing_select_idx = MAP_3_END_HI_IDX;
	break;
    case CLPD6720_WINDOW_4:
	timing_select_idx = MAP_4_END_HI_IDX;
	break;
    }

    /*
     * Read the register and mask off the timing select bits,
     * preserve the rest of the register, and OR in the timing
     * select bits to select the timing parameter set.
     */
    timing_select = clpd6720_reg_read(socket, timing_select_idx);
    timing_select &= ~CARD_TIMER_SELECT_MASK;
    if (timing == CLPD6720_TIMING_0) {
	timing_select |= CARD_TIMER_SELECT_0;
    } else {
	timing_select |= CARD_TIMER_SELECT_1;
    }

    /*
     * write the new (modified) value back to the register
     */
    clpd6720_reg_write(socket, timing_select_idx, timing_select);

    PRT(P22,("CL_set_window_timing(s=%d, w=%d, t=%d)\n",socket, window,timing));

}


/******************************************************************************
 * Name: clpd6720_set_timing
 *
 * Description: assign values to the set of timing parameters indicated
 *
 * Input: 
 *   socket -- the socket identifier (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   timing_set -- the timing set to use for the window indicated (0 or 1) 
 *   setup -- the value to use for the set-up timing (see manual)
 *   command -- the value to use for the command timing (see manual)
 *   recovery -- the value to use for the recovery timing (see manual)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_set_timing (clpd6720_socket_t socket, 
			  clpd6720_timing_t timing_set, 
			  uchar setup, 
			  uchar command, 
			  uchar recovery)
{

    if (timing_set == CLPD6720_TIMING_0) {
	clpd6720_reg_write(socket, SETUP_TIMING_0_IDX, setup);
	clpd6720_reg_write(socket, COMMAND_TIMING_0_IDX, command);
	clpd6720_reg_write(socket, RECOVERY_TIMING_0_IDX, recovery);
    } else {
	clpd6720_reg_write(socket, SETUP_TIMING_1_IDX, setup);
	clpd6720_reg_write(socket, COMMAND_TIMING_1_IDX, command);
	clpd6720_reg_write(socket, RECOVERY_TIMING_1_IDX, recovery);
    }

    PRT(P22,("CL_set_timing(s=%d, t=%d, s=%2x, c=%2x, r=%2x)\n",
	     socket,timing_set,setup,command,recovery));

}


/******************************************************************************
 * Name: clpd6720_reset_socket
 *
 * Description: issues a reset to the card in the socket
 *
 * Input: 
 *   socket -- the socket identifier (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_reset_socket (clpd6720_socket_t socket)
{
    uchar value;

    /*
     * read, modify, write the register to reset card
     */
    value = clpd6720_reg_read(socket, INTERRUPT_CTRL_IDX);
    value &= ~CARD_RESET_RELEASE;
    clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX, value);
    value |= CARD_RESET_RELEASE;
    clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX, value);

    PRT(P22,("CL_reset_socket(s=%d)\n",socket));

}


/******************************************************************************
 * Name: clpd6720_enable_window
 *
 * Description: enables a window (allows mapping)
 *
 * Input: 
 *   socket -- the socket identifier (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   window -- the window number (CLPD6720_WINDOW_1 to CLPD6720_WINDOW_5, or 
 *                                CLPD6720_IO_WINDOW_1 or CLPD6720_IO_WINDOW_2)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_enable_window (clpd6720_socket_t socket, clpd6720_window_t window)
{
    uchar value;
    uchar mask;

    /*
     * get the mask for the window in question
     */
    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0: mask = MAP_0_MASK; break;
    case CLPD6720_WINDOW_1: mask = MAP_1_MASK; break;
    case CLPD6720_WINDOW_2: mask = MAP_2_MASK; break;
    case CLPD6720_WINDOW_3: mask = MAP_3_MASK; break;
    case CLPD6720_WINDOW_4: mask = MAP_4_MASK; break;
    }

    /*
     * read, modify, write the register
     */
    value = clpd6720_reg_read(socket, MAPPING_ENABLE_IDX);
    value |= mask;
    clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, value);

    PRT(P22,("CL_enable_window(s=%d, w=%d)\n",socket,window));

}


/******************************************************************************
 * Name: clpd6720_disable_window
 *
 * Description: disables a window (turns off any mapping
 *
 * Input: 
 *   socket -- the socket identifier (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   window -- the window number (CLPD6720_WINDOW_1 to CLPD6720_WINDOW_5, or 
 *                                CLPD6720_IO_WINDOW_1 or CLPD6720_IO_WINDOW_2)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_disable_window (clpd6720_socket_t socket, clpd6720_window_t window)
{
    uchar value;
    uchar mask;

    /*
     * get the mask for the window in question
     */
    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0: mask = MAP_0_MASK; break;
    case CLPD6720_WINDOW_1: mask = MAP_1_MASK; break;
    case CLPD6720_WINDOW_2: mask = MAP_2_MASK; break;
    case CLPD6720_WINDOW_3: mask = MAP_3_MASK; break;
    case CLPD6720_WINDOW_4: mask = MAP_4_MASK; break;
    }

    /*
     * read, modify, write the register
     */
    value = clpd6720_reg_read(socket, MAPPING_ENABLE_IDX);
    value &= ~mask;
    clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, value);

    PRT(P22,("CL_disable_window(s=%d, w=%d)\n",socket,window));

}


/******************************************************************************
 * Name: clpd6720_write_enable_socket
 *
 * Description: turns on the programming power and write enable to a socket
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_write_enable_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);
    interface_status &= ~SOCKET_WRITE_PROTECT_MASK;
    clpd6720_reg_write(socket, INTERFACE_STATUS_IDX, interface_status);

    PRT(P22,("CL_write_enable_socket(s=%d)\n",socket));
}


/******************************************************************************
 * Name: clpd6720_write_protect_socket
 *
 * Description: turns off the programming power and write protects a socket
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_write_protect_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);
    interface_status |= SOCKET_WRITE_PROTECT_MASK;
    clpd6720_reg_write(socket, INTERFACE_STATUS_IDX, interface_status);

    PRT(P22,("CL_write_protect_socket(s=%d)\n",socket));
}



/******************************************************************************
 * Name: clpd6720_write_enable_window
 *
 * Description: turns on the programming power and write enable to a window
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   window -- the window number (CLPD6720_WINDOW_1 to CLPD6720_WINDOW_5)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_write_enable_window (clpd6720_socket_t socket,
				   clpd6720_window_t window)
{
    uchar write_protect_idx;
    uchar write_protect;

    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0:
	write_protect_idx = MAP_0_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_1:
	write_protect_idx = MAP_1_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_2:
	write_protect_idx = MAP_2_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_3:
	write_protect_idx = MAP_3_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_4:
	write_protect_idx = MAP_4_OFFSET_HI_IDX;
	break;
    }

    /*
     * Read the register and mask off the timing select bits,
     * preserve the rest of the register, and OR in the timing
     * select bits to select the timing parameter set.
     */
    write_protect = clpd6720_reg_read(socket, write_protect_idx);
    write_protect &= ~WRITE_PROTECT_MASK;
    write_protect |= WRITE_ENABLE;
    
    /*
     * write the new (modified) value back to the register
     */
    clpd6720_reg_write(socket, write_protect_idx, write_protect);

    PRT(P22,("CL_write_enable_window(s=%d,w=%d)\n",socket,window));
}


/******************************************************************************
 * Name: clpd6720_write_protect_window
 *
 * Description: turns off the programming power and write protects a window
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *   window -- the window number (CLPD6720_WINDOW_1 to CLPD6720_WINDOW_5)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_write_protect_window (clpd6720_socket_t socket,
				    clpd6720_window_t window)
{
    uchar write_protect_idx;
    uchar write_protect;

    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0:
	write_protect_idx = MAP_0_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_1:
	write_protect_idx = MAP_1_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_2:
	write_protect_idx = MAP_2_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_3:
	write_protect_idx = MAP_3_OFFSET_HI_IDX;
	break;
    case CLPD6720_WINDOW_4:
	write_protect_idx = MAP_4_OFFSET_HI_IDX;
	break;
    }

    /*
     * Read the register and mask off the timing select bits,
     * preserve the rest of the register, and OR in the timing
     * select bits to select the timing parameter set.
     */
    write_protect = clpd6720_reg_read(socket, write_protect_idx);
    write_protect &= ~WRITE_PROTECT_MASK;
    write_protect |= WRITE_PROTECT;
    
    /*
     * write the new (modified) value back to the register
     */
    clpd6720_reg_write(socket, write_protect_idx, write_protect);

    PRT(P22,("CL_write_protect_window(s=%d,w=%d)\n",socket,window));
}



