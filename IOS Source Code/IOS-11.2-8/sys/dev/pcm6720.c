/* $Id: pcm6720.c,v 3.2 1995/11/17 09:01:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/pcm6720.c,v $
 *------------------------------------------------------------------
 * PCMCIA controller accessing routines
 *
 * Jan   95    Mick Henniger (stolen from Jack Jenney)
 *
 * Copyright (c) 1994-1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcm6720.c,v $
 * Revision 3.2  1995/11/17  09:01:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/29  22:33:06  henniger
 * CSCdi36569:  Boot Load shudnt xsum files twice
 * bootload should not checksum file while loading
 * PCMCIA attribute search should be limited
 *
 * Revision 2.1  1995/06/07  20:25:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
Name:

 Description:  pcmcia controller routines for Cirrus Logic 672x chip.
	       Allows init, windows config, timing setup, etc.

 Design Notes:


 ******************************************************************************/

#include "master.h"
#include "../dev/flash_defines.h"
#include "dev_io.h"
#include "../src-36-c1000/c1000_flash.h"
#include "pcm6720.h"
#include "pcm6720_internals.h"

#define PRT(level,params) {}


/*----------------------------------------------------------------------------*
 *  Local prototypes 
 *----------------------------------------------------------------------------*/

static int clpd6720_init (void);
static void pcmcia_timing_init(void);
static void clpd6720_set_window(clpd6720_socket_t,clpd6720_window_t,
        		ulong , ulong , ulong , clpd6720_pcmcia_type_t ,
        		clpd6720_window_width_t,clpd6720_write_protect_t);
static void clpd6720_set_window_timing (clpd6720_socket_t socket,
                                        clpd6720_window_t window,
                                        clpd6720_timing_t timing);
static void clpd6720_set_timing(clpd6720_socket_t,clpd6720_timing_t,
			uchar,uchar,uchar);
static void clpd6720_enable_window(clpd6720_socket_t, clpd6720_window_t);
static void clpd6720_enable_socket (clpd6720_socket_t);
static int  bincmp(uchar *, uchar *, int);



/**** these are defined but not currently used. ****************************/

       void clpd6720_disable_window(clpd6720_socket_t,clpd6720_window_t);
       void clpd6720_write_enable_socket (clpd6720_socket_t socket);
       void clpd6720_write_enable_window (clpd6720_socket_t,
			clpd6720_window_t);
       void clpd6720_write_protect_window (clpd6720_socket_t,
			clpd6720_window_t);


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
 ******************************************************************************/
void clpd6720_reg_write (clpd6720_socket_t socket, uchar reg_index, uchar value)
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

    CLPD6720_INDEX_REGISTER = index;
    CLPD6720_DATA_REGISTER = value;
    PRT(PX,("CL_reg_write(s=%d, reg %2.2x, val = %2.2x)\n",socket,reg_index,value));

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
    value = (CLPD6720_DATA_REGISTER);
    PRT(PX,("CL_reg_read(s=%d, reg %2.2x) = %2.2x)\n",socket,reg_index,value));

    return(value);
}
/****************************************************************************
 * pcmcia checking routines.
 * Around each section of code which references the pcmcia card,
 * a pcm_chk_entry / pcm_chk_exit pair should be called.
 * Either will return FALSE if something is wrong, or
 *                    TRUE  if everything appears okay.
 * Initially, this will just check for card present.
 * Eventually, this will enable a bus error monitor, and check
 * for bus error's, then report error if any bus errors occurred
 * so that the calling routine knows to return an error.
 ****************************************************************************/

boolean pcmcia_card_present (dev_info_t *dev)
{
    clpd6720_socket_t socket = dev->dev_sock;
    uchar value = clpd6720_reg_read(socket,INTERFACE_STATUS_IDX);

    value &= CD_MASK;
    return(value == CD_CARD_PRESENT ? TRUE : FALSE);
}

uchar pcmcia_status_change (dev_info_t *dev)
{
    uchar value;
    clpd6720_socket_t socket = dev->dev_sock;
    value = clpd6720_reg_read(socket,CARD_STATUS_CHANGE_IDX);
    return(value);
}

/****************************************************************************
 * pcm_get_reg_addr(socket)  -> gets the base address of the attribute 
 * space for the socket given.  since only one socket is used now,
 * is relatively trivial.
 ****************************************************************************/
static inline uchar * pcm_get_reg_addr (clpd6720_socket_t socket)
{
  return((uchar*)ADRSPC_PCMCIA_ATTR);
}

/****************************************************************************
 *  bincmp(uchar*p,uchar *q,len)                                            *
 *  Before tip-toeing through the tuples, check to make sure that the       *
 *  attribute space really is different from the common memory space.       *
 *  If a card has no  attribute space, it will probably ignore the "-REG"   *
 *  pin, and therefore the two memory spaces are identical.  use this to    *
 *  determine if attribute space exists on the card.                        *
 ***************************************************************************/
static int bincmp (uchar *p, uchar *q, int len)
{
    int retval = 0;
    while (len--) {
        if (*p++ != *q++) {
	    retval = -1;
	    break;
        } 
    } 
    return(retval);
}

/****************************************************************************
 * pcm_get_tuple(dev_info_t *dev,tuple,offset)
 * gets the tuple from attribute space of the specified socket.
 * returns -1 if tuple not found, or attribute space corrupt/ non-existent
 * returns the 8 bit uchar of the byte in the tuple defined by offset.
 * offset of 0 means first byte of tuple.
 * Note: Bug here- if card is pulled after pcmcia_card_present() call, and
 * before accessing tuples, unprotected bus error can occur.
 ****************************************************************************/
static int pcm_get_tuple (dev_info_t *dev,uchar tuple,int ofs)
{
    uchar *p;
    ushort tuple_cnt = 0;
    uchar tuple_type, tuple_link;
    int val = -1;

    if (      (dev)  &&
              (dev->dev_sock) &&
	      (pcmcia_card_present(dev)) &&
	      (ofs >= 0) && (ofs < 0xff) ) {

        p = pcm_get_reg_addr(dev->dev_sock);
        if (bincmp(p,(uchar*)dev->dev_addr,16)) {
	    while ( (p >= (uchar*)ADRSPC_PCMCIA_ATTR) &&  /* fix if 2 socks*/
		     (p <  (uchar*)(ADRSPC_PCMCIA_ATTR + 
				    ADRSPC_PCMCIA_ATSPC_SIZ+4+ofs*2)) &&
		     (tuple_cnt++ < MAX_TUPLES)  ) {
    
                tuple_type = *p;
	        p += 2;
                tuple_link = *p;
	        p += 2;
	        if (tuple_type == tuple) {
		    p += ofs * 2;
		    val = *p;
		    break;
                }
    
	        if (   (tuple_type == CISTPL_END) ||
		       (tuple_link == CISTPL_END) ) {
		    break;
                }
	        p += tuple_link * 2;
            }
        }
    }
    return (val);
}

/****************************************************************************
 * pcm_flash_chips(dev)
 * determines the size of the flash card, using tuples
 * 
 ****************************************************************************/
ushort pcm_flash_chips(dev_info_t *dev)
{
    int    mfg,nfo;
    ushort retval=0;

    if (dev) {
        if (dev->dev_sock) {
	    if (pcmcia_card_present(dev)) {
	        mfg = pcm_get_tuple(dev,CISTPL_JEDEC_C,0);
	        nfo = pcm_get_tuple(dev,CISTPL_JEDEC_C,1);
	        if ( (mfg >= 0) && (mfg != 0xff) ) {
	            if ( (nfo >= 0) && (nfo != 0xff) ) {
                        retval = mfg << 8 | nfo;
                    }
                }
	    }
        }
    }
    return (retval);
}

/****************************************************************************
 * pcm_flash_size(dev)
 * determines the size of the flash card, using tuples
 * 
 ****************************************************************************/
ulong pcm_flash_size(dev_info_t *dev)
{
    ulong  size = 0L;
    ulong  sizebank=0L;
    ushort banks=0;
    ushort banksize=0;
    int    val,ofs;

    if (  (dev) && (dev->dev_sock) && (pcmcia_card_present(dev)) ) {
        val = pcm_get_tuple(dev,CISTPL_DEVICE,0);
        if ( (val >= 0) && (val != 0xff) ) {
            ofs = 1;
	    if ((val & CISTPL_DEV_SPEED_MASK) == CISTPL_DEV_SPEED_MASK){
	        while (1) {
                    val = pcm_get_tuple(dev,CISTPL_DEVICE,0);
                    ofs++;
                    if ( (val >= 0) && (val != 0xff) ) {
			break;
                    }
		    if ( (val & CISTPL_DEV_SPEED_EXT_MASK) == 0) {
			break;
                    }
                }
	    }
            val = pcm_get_tuple(dev,CISTPL_DEVICE,ofs);
	    if ( (val >= 0) && (val != 0xff) ) {
		    banks    = (val & CISTPL_DEV_SIZE_CNT_MASK) >> 
                                      CISTPL_DEV_SIZE_CNT_SHIFT;
		    banksize = val & CISTPL_DEV_SIZE_UNIT_MASK;
		    switch (banksize) {
			case CISTPL_DEV_SIZE_UNIT_512   : sizebank = 512L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_2K    : sizebank = 2   *1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_8K    : sizebank = 8   *1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_32K   : sizebank = 32  *1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_128K  : sizebank = 128 *1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_512K  : sizebank = 512 *1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_2M    : sizebank=2*1024*1024L;
                                                          break;
			case CISTPL_DEV_SIZE_UNIT_UNK   :
			default                         : sizebank = 0L;
                                                          break;
                    }
		    size = sizebank * (banks+1);
	    }
        }
    }
    return (size);
}

/****************************************************************************
 * pcm_device_type(dev)
 * determines type of card: flash, ram, etc. and returns as a uchar.
 * 
 ****************************************************************************/
uchar pcm_device_type (dev_info_t *dev)
{
    clpd6720_devtype_t type = PCM_DEV_UNKNOWN;
    int val;

    if (dev) {
        if (dev->dev_sock) {
	    if (pcmcia_card_present(dev)) {
	        val = pcm_get_tuple(dev,CISTPL_DEVICE,0);
		if (val < 0) {
		    val = CISTPL_DEV_NO_ATTR;
                } else {
		    val &= CISTPL_DEV_TYPE_MASK;
                }
		switch (val) {
		    case CISTPL_DEV_TYPE_FLASH :  type = PCM_DEV_FLASH;
						  break;
		    case CISTPL_DEV_TYPE_SRAM  :  type = PCM_DEV_SRAM;
						  break;
		    case CISTPL_DEV_NO_ATTR    :  type = PCM_DEV_NO_ATTR;
						  break;
		    default                    :  type = PCM_DEV_UNKNOWN;
						  break;
                }
	    }
        }
    }
    return (type);
}


void pcmcia_init (dev_info_t *dev)
{
    clpd6720_init();	/* power off, misc1 and 2, and mgmt intr. */
    pcmcia_timing_init();

    /* setup the common 'data' memory space to full 16 megabytes.          */
    /* this sets the memory window fields, including the write protect bit */
    /* the bus size, and whether it's common or attribute memory space.    */

    clpd6720_set_window(dev->dev_sock,         /* use hardcoded socket    */
                        PCM_FLASH_DWIN,         /* this window is all data */
                        ADRSPC_PCMCIA_MEM,      /* This local address maps */
                        0,			/* to loc 0 of flash data  */
                        ADRSPC_PCMCIA_MSPC_SIZ, /* Set size to max window  */
                        CLPD6720_COMMON_MEM,    /* common (not attribute)  */
                        CLPD6720_WIDTH_16_BIT,  /* width of data for window*/
                        CLPD6720_READ_WRITE     /* make it writable-not Vpp*/
                        );

    /* setup the attribute memory space to the full 16 megabytes.          */

    clpd6720_set_window(dev->dev_sock,         
                        PCM_FLASH_AWIN,        
                        ADRSPC_PCMCIA_ATTR,    
                        0,		
                        ADRSPC_PCMCIA_ATSPC_SIZ,
                        CLPD6720_ATTRIB_MEM,  
                        CLPD6720_WIDTH_16_BIT,
                        CLPD6720_READ_WRITE 
                        );

    clpd6720_enable_socket(dev->dev_sock);               /* Vpp  if inserted*/
    clpd6720_enable_window(dev->dev_sock,PCM_FLASH_AWIN);
    clpd6720_enable_window(dev->dev_sock,PCM_FLASH_DWIN);/* turn window on  */

     
	 
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
	
	clpd6720_reg_write(socket, INTERRUPT_CTRL_IDX, CARD_RESET);
	
	clpd6720_reg_write(socket, MGMT_INT_CFG_IDX,CARD_DETECT_CHANGE_ENABLE);
	
	clpd6720_reg_write(socket, MISC_CTRL_1_IDX,
			   VCC_5V |           /* use 5V on card (not 3) */
                           MGMT_INT_LEVEL |   /* intrs. are level trig. */
                           SPEAKER_DISABLE |  /* tristate SPKR_OUT      */
                           INPACK_IGNORED);   /* inpack pin ignored     */
	
	clpd6720_reg_write(socket, MISC_CTRL_2_IDX, 
			   BYPASS_FREQ_SYNTH |/* Clk = Clk input freq.  */
			   RUN_CLOCK_ALWAYS | /* dont power conserve    */
			   NORMAL_POWER_MODE |/* disable low power      */
			   VDD_5V_CORE_MODE | /* thresholds @ 2.5V      */
			   IRQ12_NORMAL_MODE |/* IRQ12, not LED output  */
			   BIT7_NORMAL_MODE | /* not a floppy drive.    */
			   NON_DMA_MODE |     /* don't use DMA.         */
			   IRQ15_NORMAL_MODE);/* IRQ15 is not Ring Ind. */
	
	clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, DISABLE_ALL);
	
    }

	return(0);
}



void pcmcia_timing_init ()
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;


    /*
     * Set up the timing parameters for the windows.
     * Each Socket has two timing sets. [set,command,and recovery time]
     * Each of the 5 windows for a socket have a bit to say which to use.
     * The convention here is to use one timing set for fast accesses,
     * and the other for slow accesses (when the specifics of the card 
     * are not known.)
     * Initialization is done to setup the slow and fast timing sets for
     * each of the sockets, and then set all 5 * 2 windows to use the 
     * fast timing set.
     * 
     */
    clpd6720_set_timing(CLPD6720_SOCKET_0,
                        FAST_WINDOW_TIMING_SET,
                        FAST_WINDOW_SETUP_TIME,
                        FAST_WINDOW_COMMAND_TIME,
                        FAST_WINDOW_RECOVERY_TIME);

    clpd6720_set_timing(CLPD6720_SOCKET_0,
                        SLOW_WINDOW_TIMING_SET,
                        SLOW_WINDOW_SETUP_TIME,
                        SLOW_WINDOW_COMMAND_TIME,
                        SLOW_WINDOW_RECOVERY_TIME);

    clpd6720_set_timing(CLPD6720_SOCKET_1,
                        FAST_WINDOW_TIMING_SET,
                        FAST_WINDOW_SETUP_TIME,
                        FAST_WINDOW_COMMAND_TIME,
                        FAST_WINDOW_RECOVERY_TIME);

    clpd6720_set_timing(CLPD6720_SOCKET_1,
                        SLOW_WINDOW_TIMING_SET,
                        SLOW_WINDOW_SETUP_TIME,
                        SLOW_WINDOW_COMMAND_TIME,
                        SLOW_WINDOW_RECOVERY_TIME);


    /*
     * Set all the windows to use the fast timing set.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) {
      for (window = CLPD6720_WINDOW_0; window <= CLPD6720_WINDOW_4; window++) {
        clpd6720_set_window_timing(socket,window,FAST_WINDOW_TIMING_SET);
      }
    }

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
 * Note: Need to add support for IO memory space eventually.
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
    uchar start_addr_lo_idx = MAP_0_START_LO_IDX;
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
    if (length <=0)
	length = 1;
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
    offset_lo = ((-start_addr)&WINDOW_OFFSET_LO_MASK) >> WINDOW_OFFSET_LO_SHIFT;
    offset_hi = ((-start_addr)&WINDOW_OFFSET_HI_MASK) >> WINDOW_OFFSET_HI_SHIFT;
    
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

    PRT(PX,("CL_set_window(s=%d, w=%d, la=%6.6lx, pa=%6.6lx, l=%6.6lx, t=%c, w=%d, wp=%s)\n",
	     socket, window, local_addr, pcmcia_addr, length, 
	     pcmcia_type == CLPD6720_COMMON_MEM ? 'C': 'A', 
	     window_width == CLPD6720_WIDTH_8_BIT ? 8: 16,
	     write_protect == CLPD6720_READ_ONLY ? "R/O": "R/W"));

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
    uchar timing_select_idx = MAP_0_END_HI_IDX;
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

    PRT(PX,("CL_set_window_timing(s=%d, w=%d, t=%d)\n",socket, window,timing));

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
 ******************************************************************************/
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

    PRT(PX,("CL_set_timing(s=%d, t=%d, s=%2.2x, c=%2.2x, r=%2.2x)\n",
             socket,timing_set,setup,command,recovery));

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
 *****************************************************************************/
static void clpd6720_enable_window (clpd6720_socket_t socket, 
				   clpd6720_window_t window)
{

    uchar value;
    uchar mask = MAP_0_MASK;

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

    PRT(PX,("CL_enable_window(s=%d, w=%d)\n",socket,window));

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
       void clpd6720_disable_window (clpd6720_socket_t socket, 
				    clpd6720_window_t window)
{
    uchar value;
    uchar mask = MAP_0_MASK;

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
    value &= ~mask;
    clpd6720_reg_write(socket, MAPPING_ENABLE_IDX, value);

    PRT(PX,("CL_set_timing(s=%d, t=%d, s=%2.2x, c=%2.2x, r=%2.2x)\n",
	     socket,timing_set,setup,command,recovery));

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
 *****************************************************************************/
       void clpd6720_write_enable_socket (clpd6720_socket_t socket)
{
    uchar interface_status;

    interface_status = clpd6720_reg_read(socket, INTERFACE_STATUS_IDX);
    interface_status &= ~SOCKET_WRITE_PROTECT_MASK;
    clpd6720_reg_write(socket, INTERFACE_STATUS_IDX, interface_status);

    PRT(PX,("CL_write_enable_socket(s=%d)\n",socket));
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

    PRT(PX,("CL_write_protect_socket(s=%d)\n",socket));
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
    uchar write_protect_idx = MAP_0_OFFSET_HI_IDX;
    uchar write_protect;

    switch (window) {
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

    PRT(PX,("CL_write_enable_window(s=%d,w=%d)\n",socket,window));
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
    uchar write_protect_idx = MAP_0_OFFSET_HI_IDX;
    uchar write_protect;

    switch (window) {
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

    PRT(PX,("CL_ write_protect_window(s=%d,w=%d)\n",socket,window));
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
static void clpd6720_enable_socket (clpd6720_socket_t socket)
{
    clpd6720_reg_write(socket, POWER_CTRL_IDX, 
            CARD_ENABLE |		/* Activate power lines.             */
            AUTO_POWER_ENABLE |         /* power on if card is in.           */
	    VPP1_POWER_VCC    |         /* power if on is Vcc not Vpp.       */
	    VCC_POWER_ENABLE            /* Turn above selected pwr leven on  */
		);

    PRT(PX,("CL_enable_socket(s=%d)\n",socket));
}

