/* $Id: clpd6729.c,v 3.1.66.4 1996/07/18 17:57:42 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/filesys/clpd6729.c,v $
 *------------------------------------------------------------------
 * clpd6729.c - CLPD6729 PCMCIA controller chip driver
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clpd6729.c,v $
 * Revision 3.1.66.4  1996/07/18  17:57:42  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.1.66.3  1996/04/22  18:58:11  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/04/03  15:43:48  wfried
 * CSCdi52833:  PCMCIA Flash File System not working
 * Branch: California_branch
 * Fix Predator PCMCIA flash file system support. Major issues are byte
 * swapping, and PCMCIA controller timing.
 *
 * Revision 3.1.66.1  1996/03/21  22:36:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/01/03  05:56:54  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration and isolate Bootflash control
 * software from PCMCIA control software.
 *
 * Revision 3.1.2.3  1995/12/08  04:58:14  mbeesley
 * Branch: ELC_branch
 * Make data register access be uchar, not ushort.
 *
 * Revision 3.1.2.2  1995/12/01  02:19:55  mbeesley
 * Branch: ELC_branch
 * Update .cvsignore. Also allow the monitor to share the PCMCIA
 * driver and raw access library.
 *
 * Revision 3.1.2.1  1995/11/22  20:25:46  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  21:36:38  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"

#if defined(PREDATOR)
#include "../src-4k-c7100/c7100_pcmap.h"
#include "../src-4k-c7100/c7100_flashmib.h"
#endif /* PREDATOR */

#include "clpd6720.h"
#include "clpd6720_internals.h"
#include "clpd6729.h"
#include "ralib_internals.h"
#include "fslib_print.h"

/*--------------------------------------------------------------------------*
 * Declarations of the pointers to the register read and write
 * function for the CLPD6720. 
 *--------------------------------------------------------------------------*/
void (*clpd6720_reg_write)(clpd6720_socket_t socket, uchar reg_index, 
			   uchar value);
uchar (*clpd6720_reg_read)(clpd6720_socket_t socket, uchar reg_index);

/****************************************************************************
 * Name: clpd6720_reg_write_pci
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
 *****************************************************************************/
static void clpd6720_reg_write_pci (clpd6720_socket_t socket, 
				    uchar reg_index, uchar value)
{
    uchar index, offset;

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
    offset = clpd6729_pcmcia_regs_offset();
    if (offset) {
	*((volatile uchar *)(ADRSPC_PCMCIA_INDEX_REG + 3)) = index;
	*((volatile uchar *)(ADRSPC_PCMCIA_DATA_REG + 1)) = value;
    } else {
	*((volatile uchar *)(ADRSPC_PCMCIA_INDEX_REG)) = index;
	*((volatile uchar *)(ADRSPC_PCMCIA_DATA_REG)) = value;
    }
    PRT(P23INT,("CL_reg_write(s=%d, reg %2x, val = %2x)\n",socket,
		reg_index,value));
}

/****************************************************************************
 * Name: clpd6720_reg_read_pci
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
 *****************************************************************************/
static uchar clpd6720_reg_read_pci (clpd6720_socket_t socket, uchar reg_index)
{
    volatile uchar *data_ptr;
    uchar value, index, offset;
    
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
    offset = clpd6729_pcmcia_regs_offset();
    if (offset) {
	data_ptr = (volatile uchar *)(ADRSPC_PCMCIA_DATA_REG + 1);
	*((volatile uchar *)(ADRSPC_PCMCIA_INDEX_REG + 3)) = index;

    } else {
	data_ptr = (volatile uchar *)(ADRSPC_PCMCIA_DATA_REG);
	*((volatile uchar *)(ADRSPC_PCMCIA_INDEX_REG)) = index;
    }
    value = *data_ptr;
    PRT(P23INT,("CL_reg_read(s=%d, reg %2x) = %2x)\n",socket,reg_index,value));

    return(value);
}

/****************************************************************************
 * Name: clpd6720_init
 *
 * Description: initializes the CLPD6720 for operation
 *
 * Input: None
 *
 * Return Value: None
 *
 *****************************************************************************/
void clpd6720_init (void)
{
    clpd6720_socket_t  socket;
    uchar interface_status;

    /*
     * setup the function pointers
     */
    clpd6720_reg_read = clpd6720_reg_read_pci;
    clpd6720_reg_write = clpd6720_reg_write_pci;

    /* 
     * Init both sockets.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) {
	/*
	 * set up the CLPD6720 stuff that never changes, and does not
	 * depend on a window
	 */
	clpd6720_reg_write(socket, POWER_CTRL_IDX, VPP1_POWER_OFF |
			   CARD_ENABLE | VCC_POWER_ENABLE);
	
	clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX, CARD_RESET_RELEASE);

	/*
	 * Set management interrupt output level
	 */
	clpd6720_reg_write(socket, EXT_IDX_IDX, EXT_CTRL_1_IDX);
	clpd6720_reg_write(socket, EXT_DT_IDX, CTL_1_MGMT_INT_LOW);

/*
 *	Enable card insert/remove interrupt only.
 *
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX,
			   CARD_DETECT_CHANGE_ENABLE |
			   READY_CHANGE_ENABLE);
 *
 */
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX,
			   CARD_DETECT_CHANGE_ENABLE | MGMT_IRQ3);

	clpd6720_reg_write(socket, MISC_CTRL_1_IDX,
			   VCC_5V | MGMT_INT_LEVEL | SPEAKER_DISABLE | 
			   INPACK_IGNORED);
	
	clpd6720_reg_write(socket, MISC_CTRL_2_IDX, 
			   NORMAL_CLOCK |
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

/****************************************************************************
 * Name: clpd6720_mgmt_intr_handler
 *
 * Description: handles the CLPD6720 management interrupt
 *
 * Input:  None
 *
 * Return Value: None
 *
 *****************************************************************************/
void clpd6720_mgmt_intr_handler (void)
{
    clpd6720_socket_t  socket;
    uchar status_change;
    uchar interface_status;

#if !defined(FSLIB) && !defined(C7100_ROM)
    leveltype level;

    if (!clpd6729_pcmcia_supported()) {
	return;
    } else {
	level = raise_interrupt_level(ALL_DISABLE);
    }
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

#if !defined(FSLIB) && !defined(C7100_ROM)
    if (onintstack()) {
	c7100_flashmib_adjust();
    }
    reset_interrupt_level(level);
#endif
}

/****************************************************************************
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
 *****************************************************************************/
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
    uchar win_up_addr_idx;

    /*
     * Select Upper and Base Address register for the given window.
     */
    switch (window) {
    default: /* to keep the compiler quiet */
    case CLPD6720_WINDOW_0:
        win_up_addr_idx = MAP_0_UPPER_IDX;
	start_addr_lo_idx = MAP_0_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_1:
        win_up_addr_idx = MAP_1_UPPER_IDX;
	start_addr_lo_idx = MAP_1_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_2:
        win_up_addr_idx = MAP_2_UPPER_IDX;
	start_addr_lo_idx = MAP_2_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_3:
        win_up_addr_idx = MAP_3_UPPER_IDX;
	start_addr_lo_idx = MAP_3_START_LO_IDX;
	break;
    case CLPD6720_WINDOW_4:
        win_up_addr_idx = MAP_4_UPPER_IDX;
	start_addr_lo_idx = MAP_4_START_LO_IDX;
	break;
    }

    /*
     * write the values to the registers
     */
    clpd6720_reg_write(socket, EXT_IDX_IDX, win_up_addr_idx);
    clpd6720_reg_write(socket, EXT_DT_IDX, (local_addr >> 24));

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
    
    end_addr_hi |= (clpd6720_reg_read(socket, end_addr_lo_idx) & 
		    CARD_TIMER_SELECT_MASK);
    
    /*
     * write the values to the registers
     */
    clpd6720_reg_write(socket, start_addr_hi_idx, start_addr_hi);
    clpd6720_reg_write(socket, start_addr_lo_idx, start_addr_lo);
    clpd6720_reg_write(socket, end_addr_hi_idx, end_addr_hi);
    clpd6720_reg_write(socket, end_addr_lo_idx, end_addr_lo);
    clpd6720_reg_write(socket, offset_hi_idx, offset_hi);
    clpd6720_reg_write(socket, offset_lo_idx, offset_lo);

    PRT(P22,("CL_set_window(s=%d, w=%d, la=%lx, pa=%lx, l=%lx, t=%s, "
	     "w=%d, wp=%s)\n",
	     socket, window, local_addr, pcmcia_addr, length, 
	     (pcmcia_type == CLPD6720_COMMON_MEM) ? "C": "A", 
	     (window_width == CLPD6720_WIDTH_8_BIT) ? 8: 16,
	     (write_protect == CLPD6720_READ_ONLY) ? "R/O": "R/W"));
}

/****************************************************************************
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
 *****************************************************************************/
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

    PRT(P22,("CL_set_window_timing(s=%d, w=%d, t=%d)\n",socket, 
	     window,timing));
}

/****************************************************************************
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
 *****************************************************************************/
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

/****************************************************************************
 * Name: clpd6720_reset_socket
 *
 * Description: issues a reset to the card in the socket
 *
 * Input: 
 *   socket -- the socket identifier (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *****************************************************************************/
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

/****************************************************************************
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
 *****************************************************************************/
void clpd6720_enable_window (clpd6720_socket_t socket, 
			     clpd6720_window_t window)
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

/****************************************************************************
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
 *****************************************************************************/
void clpd6720_disable_window (clpd6720_socket_t socket, 
			      clpd6720_window_t window)
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

/****************************************************************************
 * Name: clpd6720_write_enable_socket
 *
 * Description: turns on the programming power and write enable to a socket
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *****************************************************************************/
void clpd6720_write_enable_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);
    interface_status &= ~SOCKET_WRITE_PROTECT_MASK;
    clpd6720_reg_write(socket, INTERFACE_STATUS_IDX, interface_status);

    PRT(P22,("CL_write_enable_socket(s=%d)\n",socket));
}

/****************************************************************************
 * Name: clpd6720_write_protect_socket
 *
 * Description: turns off the programming power and write protects a socket
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *****************************************************************************/
void clpd6720_write_protect_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);
    interface_status |= SOCKET_WRITE_PROTECT_MASK;
    clpd6720_reg_write(socket, INTERFACE_STATUS_IDX, interface_status);

    PRT(P22,("CL_write_protect_socket(s=%d)\n",socket));
}

/****************************************************************************
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
 *****************************************************************************/
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

/****************************************************************************
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
 *****************************************************************************/
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

/****************************************************************************
 * Name: clpd6729_print_sock_regs
 *
 * Description: Print CLPD6729 Socket Specific Registers
 *
 * Input: None
 *
 * Return Value: None
 *
 *****************************************************************************/
static void clpd6729_print_sock_regs (clpd6720_socket_t socket)
{
    uchar offset;

    /*
     * Select right extended register address based on socket
     */
    offset = SOCKET_0_START_INDEX;
    if (socket == CLPD6720_SOCKET_1)
	offset = SOCKET_1_START_INDEX;

    /*
     * Print Socket Registers
     */
    printf("\n Socket %d:\n", (socket == CLPD6720_SOCKET_0) ? 0 : 1);
    printf("  (0x%02X) Interface Status = 0x%02x\n",
	INTERFACE_STATUS_IDX + offset,
	clpd6720_reg_read(socket, INTERFACE_STATUS_IDX));
    printf("  (0x%02X) Power Control = 0x%02x\n",
	POWER_CTRL_IDX + offset,
	clpd6720_reg_read(socket, POWER_CTRL_IDX));
    printf("  (0x%02X) Interrupt and General Control = 0x%02x\n",
	INTERRUPT_CTRL_IDX + offset,
	clpd6720_reg_read(socket, INTERRUPT_CTRL_IDX));
    printf("  (0x%02X) Card Status Change = 0x%02x\n",
	CARD_STATUS_CHANGE_IDX + offset,
	clpd6720_reg_read(socket, CARD_STATUS_CHANGE_IDX));
    printf("  (0x%02X) Management Interrupt Configuration = 0x%02x\n",
	MGMT_INT_CFG_IDX + offset,
	clpd6720_reg_read(socket, MGMT_INT_CFG_IDX));
    printf("  (0x%02X) Mapping Enable = 0x%02x\n",
	MAPPING_ENABLE_IDX + offset,
	clpd6720_reg_read(socket, MAPPING_ENABLE_IDX));
    printf("  (0x%02X) I/O Window Control = 0x%02x\n",
	IO_WINDOW_CTRL_IDX + offset,
	clpd6720_reg_read(socket, IO_WINDOW_CTRL_IDX));
    printf("  (0x%02X) System I/O Map 0 Start Address Low = 0x%02x\n",
	IO_MAP_0_START_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_START_LO_IDX));
    printf("  (0x%02X) System I/O Map 0 Start Address High = 0x%02x\n",
	IO_MAP_0_START_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_START_HI_IDX));
    printf("  (0x%02X) System I/O Map 0 End Address Low = 0x%02x\n",
	IO_MAP_0_END_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_END_LO_IDX));
    printf("  (0x%02X) System I/O Map 0 End Address High = 0x%02x\n",
	IO_MAP_0_END_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_END_HI_IDX));
    printf("  (0x%02X) System I/O Map 1 Start Address Low = 0x%02x\n",
	IO_MAP_1_START_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_START_LO_IDX));
    printf("  (0x%02X) System I/O Map 1 Start Address High = 0x%02x\n",
	IO_MAP_1_START_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_START_HI_IDX));
    printf("  (0x%02X) System I/O Map 1 End Address Low = 0x%02x\n",
	IO_MAP_1_END_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_END_LO_IDX));
    printf("  (0x%02X) System I/O Map 1 End Address High = 0x%02x\n",
	IO_MAP_1_END_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_END_HI_IDX));
    printf("  (0x%02X) System Memory Map 0 Start Address Low = 0x%02x\n",
	MAP_0_START_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_START_LO_IDX));
    printf("  (0x%02X) System Memory Map 0 Start Address High = 0x%02x\n",
	MAP_0_START_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_START_HI_IDX));
    printf("  (0x%02X) System Memory Map 0 End Address Low = 0x%02x\n",
	MAP_0_END_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_END_LO_IDX));
    printf("  (0x%02X) System Memory Map 0 End Address High = 0x%02X\n",
	MAP_0_END_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_END_HI_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, MAP_0_UPPER_IDX);
    printf("  (0x05 EXT) System Memory Map 0 Upper Address = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    printf("  (0x%02X) Card Memory Map 0 Offset Address Low = 0x%02x\n",
	MAP_0_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_OFFSET_LO_IDX));
    printf("  (0x%02X) Card Memory Map 0 Offset Address High = 0x%02x\n",
	MAP_0_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_0_OFFSET_HI_IDX));
    printf("  (0x%02X) Misc Control 1 = 0x%02x\n",
	MISC_CTRL_1_IDX + offset,
	clpd6720_reg_read(socket, MISC_CTRL_1_IDX));
    printf("  (0x%02X) FIFO Control = 0x%02x\n",
	FIFO_CTRL_IDX + offset,
	clpd6720_reg_read(socket, FIFO_CTRL_IDX));
    printf("  (0x%02X) System Memory Map 1 Start Address Low = 0x%02x\n",
	MAP_1_START_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_START_LO_IDX));
    printf("  (0x%02X) System Memory Map 1 Start Address High = 0x%02x\n",
	MAP_1_START_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_START_HI_IDX));
    printf("  (0x%02X) System Memory Map 1 End Address Low = 0x%02x\n",
	MAP_1_END_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_END_LO_IDX));
    printf("  (0x%02X) System Memory Map 1 End Address High = 0x%02X\n",
	MAP_1_END_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_END_HI_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, MAP_1_UPPER_IDX);
    printf("  (0x06 EXT) System Memory Map 1 Upper Address = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    printf("  (0x%02X) Card Memory Map 1 Offset Address Low = 0x%02x\n",
	MAP_1_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_OFFSET_LO_IDX));
    printf("  (0x%02X) Card Memory Map 1 Offset Address High = 0x%02x\n",
	MAP_1_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_1_OFFSET_HI_IDX));
    printf("  (0x%02X) System Memory Map 2 Start Address Low = 0x%02x\n",
	MAP_2_START_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_START_LO_IDX));
    printf("  (0x%02X) System Memory Map 2 Start Address High = 0x%02x\n",
	MAP_2_START_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_START_HI_IDX));
    printf("  (0x%02X) System Memory Map 2 End Address Low = 0x%02x\n",
	MAP_2_END_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_END_LO_IDX));
    printf("  (0x%02X) System Memory Map 2 End Address High = 0x%02X\n",
	MAP_2_END_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_END_HI_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, MAP_2_UPPER_IDX);
    printf("  (0x07 EXT) System Memory Map 2 Upper Address = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    printf("  (0x%02X) Card Memory Map 2 Offset Address Low = 0x%02x\n",
	MAP_2_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_OFFSET_LO_IDX));
    printf("  (0x%02X) Card Memory Map 2 Offset Address High = 0x%02x\n",
	MAP_2_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_2_OFFSET_HI_IDX));
    printf("  (0x%02X) ATA Control = 0x%02x\n",
	ATA_CTRL_IDX + offset,
	clpd6720_reg_read(socket, ATA_CTRL_IDX));
    printf("  (0x%02X) System Memory Map 3 Start Address Low = 0x%02x\n",
	MAP_3_START_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_START_LO_IDX));
    printf("  (0x%02X) System Memory Map 3 Start Address High = 0x%02x\n",
	MAP_3_START_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_START_HI_IDX));
    printf("  (0x%02X) System Memory Map 3 End Address Low = 0x%02x\n",
	MAP_3_END_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_END_LO_IDX));
    printf("  (0x%02X) System Memory Map 3 End Address High = 0x%02X\n",
	MAP_3_END_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_END_HI_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, MAP_3_UPPER_IDX);
    printf("  (0x08 EXT) System Memory Map 3 Upper Address = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    printf("  (0x%02X) Card Memory Map 3 Offset Address Low = 0x%02x\n",
	MAP_3_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_OFFSET_LO_IDX));
    printf("  (0x%02X) Card Memory Map 3 Offset Address High = 0x%02x\n",
	MAP_3_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_3_OFFSET_HI_IDX));
    printf("  (0x%02X) System Memory Map 4 Start Address Low = 0x%02x\n",
	MAP_4_START_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_START_LO_IDX));
    printf("  (0x%02X) System Memory Map 4 Start Address High = 0x%02x\n",
	MAP_4_START_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_START_HI_IDX));
    printf("  (0x%02X) System Memory Map 4 End Address Low = 0x%02x\n",
	MAP_4_END_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_END_LO_IDX));
    printf("  (0x%02X) System Memory Map 4 End Address High = 0x%02X\n",
	MAP_4_END_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_END_HI_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, MAP_4_UPPER_IDX);
    printf("  (0x09 EXT) System Memory Map 4 Upper Address = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    printf("  (0x%02X) Card Memory Map 4 Offset Address Low = 0x%02x\n",
	MAP_4_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_OFFSET_LO_IDX));
    printf("  (0x%02X) Card Memory Map 4 Offset Address High = 0x%02x\n",
	MAP_4_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, MAP_4_OFFSET_HI_IDX));
    printf("  (0x%02X) Card I/O Map 0 Offset Address Low = 0x%02x\n",
	IO_MAP_0_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_OFFSET_LO_IDX));
    printf("  (0x%02X) Card I/O Map 0 Offset Address High = 0x%02x\n",
	IO_MAP_0_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_0_OFFSET_HI_IDX));
    printf("  (0x%02X) Card I/O Map 1 Offset Address Low = 0x%02x\n",
	IO_MAP_1_OFFSET_LO_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_OFFSET_LO_IDX));
    printf("  (0x%02X) Card I/O Map 1 Offset Address High = 0x%02x\n",
	IO_MAP_1_OFFSET_HI_IDX + offset,
	clpd6720_reg_read(socket, IO_MAP_1_OFFSET_HI_IDX));
    printf("  (0x%02X) Setup Timing 0 = 0x%02x\n",
	SETUP_TIMING_0_IDX + offset,
	clpd6720_reg_read(socket, SETUP_TIMING_0_IDX));
    printf("  (0x%02X) Command Timing 0 = 0x%02x\n",
	COMMAND_TIMING_0_IDX + offset,
	clpd6720_reg_read(socket, COMMAND_TIMING_0_IDX));
    printf("  (0x%02X) Recovery Timing 0 = 0x%02x\n",
	RECOVERY_TIMING_0_IDX + offset,
	clpd6720_reg_read(socket, RECOVERY_TIMING_0_IDX));
    printf("  (0x%02X) Setup Timing 1 = 0x%02x\n",
	SETUP_TIMING_1_IDX + offset,
	clpd6720_reg_read(socket, SETUP_TIMING_1_IDX));
    printf("  (0x%02X) Command Timing 1 = 0x%02x\n",
	COMMAND_TIMING_1_IDX + offset,
	clpd6720_reg_read(socket, COMMAND_TIMING_1_IDX));
    printf("  (0x%02X) Recovery Timing 1 = 0x%02x\n",
	RECOVERY_TIMING_1_IDX + offset,
	clpd6720_reg_read(socket, RECOVERY_TIMING_1_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, EXT_CTRL_1_IDX);
    printf("  (0x03 EXT) Extension Control 1 = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
    clpd6720_reg_write(socket, EXT_IDX_IDX, EXT_CTRL_2_IDX);
    printf("  (0x0B EXT) Extension Control 2 = 0x%02X\n",
	clpd6720_reg_read(socket, EXT_DT_IDX));
}

/****************************************************************************
 * Name: clpd6729_print_regs
 *
 * Description: Print CLPD6729 Registers
 *
 * Input: None
 *
 * Return Value: None
 *
 *****************************************************************************/
void clpd6729_print_regs (void)
{
    clpd6720_socket_t socket;

    printf("\nCLPD6729 registers:\n");
 
    /*
     * Chip General Information
     */
    printf(" (0x00) Chip Revision = 0x%02x\n",
           clpd6720_reg_read(CLPD6720_SOCKET_0, CHIP_REVISION_IDX));
    printf(" (0x1E) Misc Control 2 = 0x%02x\n",
           clpd6720_reg_read(CLPD6720_SOCKET_0, MISC_CTRL_2_IDX));
    printf(" (0x1F) Chip Information = 0x%02x\n",
           clpd6720_reg_read(CLPD6720_SOCKET_0, CHIP_INFO_IDX));

    /*
     * Now Print Socket Specific Registers
     */
    socket = CLPD6720_SOCKET_0;
    clpd6729_print_sock_regs(socket);
    socket = CLPD6720_SOCKET_1;
    clpd6729_print_sock_regs(socket);
}

/* end of file */
