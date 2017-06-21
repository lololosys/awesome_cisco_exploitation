/* $Id: clpd6720.c,v 3.2.58.1 1996/03/21 23:28:49 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/clpd6720.c,v $
 *------------------------------------------------------------------
 * clpd6720.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Cirrus CLDP6720 chip routines
 * This module is borrowed from system code
 *------------------------------------------------------------------
 * $Log: clpd6720.c,v $
 * Revision 3.2.58.1  1996/03/21  23:28:49  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:44  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "clpd6720.h"
#include "clpd6720_internals.h"

#define PRT(level,params) printf params;

/******************************************************************************
 * Name: clpd6720_reg_write
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

static void clpd6720_reg_write (clpd6720_socket_t socket, uchar reg_index, uchar value)
{
    uchar index;

    if (socket == CLPD6720_SOCKET_0) {
	index = reg_index + SOCKET_0_START_INDEX;
    } else {
	index = reg_index + SOCKET_1_START_INDEX;
    }

    CLPD6720_DATA_16_REGISTER = (index << 8) + value;

#ifdef DEBUG
    PRT(PX,("CL_reg_write(s=%d, reg %2.2x, val = %2.2x)\n",socket,reg_index,value));
    PRT(PX,("CL_reg_write(16 bit val is #%x\n",(index << 8) + value);
#endif
}


/******************************************************************************
 * Name: clpd6720_reg_read
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

uchar clpd6720_reg_read (clpd6720_socket_t socket, uchar reg_index)
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
    CLPD6720_INDEX_REGISTER = index;
    value = (uchar)(CLPD6720_DATA_16_REGISTER);
#ifdef DEBUG
    PRT(PX,("CL_reg_read(s=%d, reg %2.2x) = %2.2x)\n",socket,reg_index,value));
#endif
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

int clpd6720_init ()
{
    clpd6720_socket_t  socket;

    /* 
     * do this for both sockets.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) {
	/*
	 * set up the CLPD6720 stuff that never changes, and does not
	 * depend on a window
	 */
	clpd6720_reg_write(socket, POWER_CTRL_IDX, VPP1_POWER_OFF);
	
	clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX, 
                                   MGMT_INT_ON_INTR | CARD_RESET);
	
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX, 0);
	
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
	
	clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, ALL_DISABLE);
	
    }

	return(0);
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

void clpd6720_set_window(clpd6720_socket_t socket, 
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


    /* 
     * find the base address of the register for the window selected.
     */
    switch (window) {
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
     * convert the local addr and pcmcia addr to start, end and offset for
     * the controller chip
     */
    start_addr = local_addr;
    end_addr = local_addr + length;
    offset = pcmcia_addr;
    
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

#ifdef DEBUG
    PRT(PX,("CL_set_window(s=%d, w=%d, la=%6.6lx, pa=%6.6lx, l=%6.6lx, t=%c, w=%d, wp=%s)\n",
	     socket, window, local_addr, pcmcia_addr, length, 
	     pcmcia_type == CLPD6720_COMMON_MEM ? 'C': 'A', 
	     window_width == CLPD6720_WIDTH_8_BIT ? 8: 16,
	     write_protect == CLPD6720_READ_ONLY ? "R/O": "R/W"));
#endif
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
clpd6720_set_window_timing(clpd6720_socket_t socket, 
			   clpd6720_window_t window, 
			   clpd6720_timing_t timing)
{
    uchar timing_select_idx;
    uchar timing_select;

    /*
     * Find the register that has the timing bits for the window
     */
    switch (window) {
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

#ifdef DEBUG
    PRT(PX,("CL_set_window_timing(s=%d, w=%d, t=%d)\n",socket, window,timing));
#endif
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

void clpd6720_set_timing(clpd6720_socket_t socket, 
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

#ifdef DEBUG
    PRT(PX,("CL_set_timing(s=%d, t=%d, s=%2.2x, c=%2.2x, r=%2.2x)\n",
	     socket,timing_set,setup,command,recovery));
#endif
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

void clpd6720_enable_window(clpd6720_socket_t socket, clpd6720_window_t window)
{
    uchar value;
    uchar mask;

    /*
     * get the mask for the window in question
     */
    switch (window) {
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

#ifdef DEBUG
    PRT(PX,("CL_enable_window(s=%d, w=%d)\n",socket,window));
#endif
}


/******************************************************************************
 * Name: clpd6720_enable_socket
 *
 * Description: enables a scoket
 *
 * Input: 
 *   socket -- which socket (CLPD6720_SOCKET_0 or CLPD6720_SOCKET_1)
 *
 * Return Value: None
 *
 *******************************************************************************/
void clpd6720_enable_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    clpd6720_reg_write(socket, POWER_CTRL_IDX, 
		CARD_ENABLE |
	    AUTO_POWER_ENABLE |
	    VPP1_POWER_12V    |
	    VCC_POWER_ENABLE
		);

#ifdef DEBUG
    PRT(PX,("CL_enable_socket(s=%d)\n",socket));
#endif
}

void clpd6720_disable_ints(void)
{
    clpd6720_reg_write(0, INTERRUPT_CTRL_IDX, 0);
    clpd6720_reg_write(0, MGMT_INT_CFG_IDX, CARD_DETECT_CHANGE_ENABLE);
    clpd6720_reg_write(1, INTERRUPT_CTRL_IDX, 0);
    clpd6720_reg_write(1, MGMT_INT_CFG_IDX, CARD_DETECT_CHANGE_ENABLE);
}
/* end of module */


