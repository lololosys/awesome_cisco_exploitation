/* $Id: c1000_flash.c,v 3.7.10.5 1996/09/10 03:22:31 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/c1000_flash.c,v $
 *------------------------------------------------------------------
 * Sapphire PCMCIA flash platform specific code.
 * c1000_flash.c - Platform specific Flash routines (for 1000 and under).
 *
 * Dec  1994, Mick Henniger  created from c3000_flash.c
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000_flash.c,v $
 * Revision 3.7.10.5  1996/09/10  03:22:31  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/21  10:02:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.3  1996/04/23  00:09:19  dcoli
 * CSCdi44581:  Warn user when copying enterprise image to ciscoPro router
 * flash
 * Branch: California_branch
 * Stamp a.out header with a bit identifying the image as a ciscoPro
 * image.
 *
 * Revision 3.7.10.2  1996/04/15  21:22:52  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/18  21:57:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  07:32:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/09  05:11:25  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.1  1996/01/24  22:49:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.7  1996/02/23  17:16:08  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.6  1996/02/22  14:38:00  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/19  17:28:37  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.4  1996/01/06  03:22:52  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/17  18:37:29  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:39:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/09/24  19:16:36  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.9  1995/09/23  05:58:11  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.8  1995/09/22  04:36:38  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.7  1995/07/18  06:48:32  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.6  1995/07/04 01:57:16  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.5  1995/06/30  23:02:32  agh
 * CSCdi36138:  C1000 flash has malloc failure test backward
 * Fix incorrect check after malloc failure.
 *
 * Revision 2.4  1995/06/28  09:30:38  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/26  20:29:54  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.2  1995/06/09  00:32:59  hampton
 * Remove more useless includes of obsolete files.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 22:52:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-------------------------------------------------------------------------
 * 01/09/95 - mrh - check for unknown Pcmcia devices, flash with no attrib.
 *                  fix ram_size to check for aliasing, 64k banks.
 *                  use slow window timing by default, flash speeds it up.
 *-------------------------------------------------------------------------*/
#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "../../boot/src-68-c3000/cookie.h" /* for cookie_t */
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/flash_dvr_les.h"
#include "../les/flash_fs_les.h"
#include "../dev/flash_partition.h"
#include "../src-36-c1000/c1000_flash.h"
#include "c1000.h"
#include "../os/boot.h"
#include "../os/signal.h"
#include "config.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "../os/reloc.h"
#include "cisco_a.out.h"
#include "../les/flh.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"
#include "../dev/pcm6720.h"
#include "../dev/pcm6720_internals.h"
#include "../les/if_table.h"


/*
 * Define flash device aliases
 */
#define SF "flash"
#define BF "bootflash"

static dev_info_t *c1000_flash_devices[MAX_DEV_INIT_ENTRIES];
static dev_info_t *dev_pending_delete;
static quicc_dpr_t *dpr; 
static volatile ushort *c1000_vpp_control_reg;
static ushort c1000_vpp_control_mask;


static void  c1000_initialize_dev(dev_info_t *dev);
static boolean c1000_flash_check_vpp(void);

/**************************************************************************
 *               Flash functions for c1000 only
 *************************************************************************/

/*--------------------------------------------------------------------------*/
/*  Bus error handling stuff.                                               */
/*--------------------------------------------------------------------------*/


static jmp_buf berr_buf;

static void c1000_handle_buserror (int signal,int subcode,void *info,
				   char *bad_addr)
{
   longjmp(&berr_buf,1);
}




/*--------------------------------------------------------------------------*/
/*  Wrappers for dev_[read,write,etc.]_fn to setup for catching bus errors  */
/*  before the call to the actual function, so if the function causes a bus */
/*  error, it will just trap to an error here.                              */
/*  Each routine gets called from devcons->dev_*_fn and it then will:       */
/*   register the bus error handler (with interrupts off )                  */
/*   make the call to setjmp to store away the state info                   */
/*   call the normal function to perform the operation.                     */
/*   else clause handles bus error by dev_chk ing and returing an error.    */
/*   before returning, it cleans up its buss error handler.                 */
/*--------------------------------------------------------------------------*/


static ulong c1000_dev_read (dev_info_t *dev, uchar *buf,uchar *addr,ulong len)
{
    ulong i=0;
    dev_cons_info_t *devcons;
    signal_handler oh;

    if (dev != NULL) {
        if (dev_chk(dev)) {
	    if (dev->devcons) {
                devcons = dev->devcons;
	        if (devcons->dev_read_wrap_fn) {
                    oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
                    if( setjmp(&berr_buf) == 0) {
                        i=(*devcons->dev_read_wrap_fn)(dev,buf,addr,len);
                    } else {
	                i = 0;
	                dev_chk(dev);
                    }
                    signal_oneshot(SIGBUS, oh);
                }
            }
        }
    }
    return(i);
}

static ulong c1000_dev_write (dev_info_t *dev, uchar *buf,uchar *addr,ulong len)
{
    ulong i=0;
    dev_cons_info_t *devcons;
    signal_handler oh;
    
    if (dev != NULL) {
        if (dev_chk(dev)) {
	    if (dev->devcons) {
                if (c1000_flash_check_vpp()) {
                    devcons = dev->devcons;
	            if (devcons->dev_write_wrap_fn) {
                        oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
                        if( setjmp(&berr_buf) == 0) {
                            i=(*devcons->dev_write_wrap_fn)(dev,buf,addr,len);
                        } else {
	                    i = 0;
	                    dev_chk(dev);
                        }
                        signal_oneshot(SIGBUS, oh);
                    }
                }
            }
        }
    }
    return(i);
}

static ulong c1000_dev_erase (dev_info_t *dev ,uchar *addr,ulong len)
{
    ulong i=0;
    dev_cons_info_t *devcons;
    signal_handler oh;

    if (dev != NULL) {
        if (dev_chk(dev)) {
	    if (dev->devcons) {
                if (c1000_flash_check_vpp()) {
                    devcons = dev->devcons;
	            if (devcons->dev_erase_wrap_fn) {
                        oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
                        if( setjmp(&berr_buf) == 0) {
                            i=(*devcons->dev_erase_wrap_fn)(dev,addr,len);
                        } else {
	                    i = 0;
	                    dev_chk(dev);
                        }
                        signal_oneshot(SIGBUS, oh);
                    }
                }
            }
        }
    }
    return(i);
}

static int c1000_dev_info  (dev_info_t *dev ,int flag)
{
    ulong i=0;
    dev_cons_info_t *devcons;
    signal_handler oh;

    if (dev != NULL) {
        if (dev_chk(dev)) {
	    if (dev->devcons) {
                devcons = dev->devcons;
	        if (devcons->dev_info_wrap_fn) {
                    oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
                    if( setjmp(&berr_buf) == 0) {
                        i=(*devcons->dev_info_wrap_fn)(dev,flag);
                    } else {
	                i = 0;
	                dev_chk(dev);
                    }
                    signal_oneshot(SIGBUS, oh);
                }
            }
        }
    }
    return(i);
}


/*
 * Device dependent pre-open function for device.
 */
static void c1000_flash_open (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we are opening for WRITE or ERASE, see that VPP is turned
     * ON and cache turned OFF.
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_ON);
        if (devcons->dev_cache_fn)
            (*devcons->dev_cache_fn)(dev, CACHE_OFF);
    }
}

/*
 * Device dependent post-close function for device.
 */
static void c1000_flash_close (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we had opened for WR or ERASE access, turn VPP OFF
     * and reenable CACHE.
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_OFF);
        if (devcons->dev_cache_fn)
            (*devcons->dev_cache_fn)(dev, CACHE_ON);
    }
}

/*
 * Global device access control gate. Used to block write access
 * to all other devices (controlled by this code) during write
 * access to this device.
 * Eg., we do not want boot flash writes during system flash writes
 * to ensure that both devices are not left erased or half-written
 * on account of system or power failure.
 */
static void c1000_flash_access (dev_info_t *dev)
{
    global_dev_gate_closed = TRUE;
}

/*
 *                Flash VPP Control
 * c1000_flash_assert_vpp() :
 *     Assert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * c1000_flash_deassert_vpp()
 *     Deassert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * check_vpp
 *     Returns TRUE if VPP asserted, FALSE otherwise.
 */
static boolean c1000_flash_check_vpp (void)
{
    uchar reg, wp;
    reg = clpd6720_reg_read(PCM_FLASH_SOCK, POWER_CTRL_IDX);
    wp  = clpd6720_reg_read(PCM_FLASH_SOCK, INTERFACE_STATUS_IDX);
    reg = ( (wp & SOCKET_WRITE_PROTECT_MASK) ? VPP1_POWER_OFF : reg );

    return( (reg & VPP1_POWER_MASK) == VPP1_POWER_12V ? TRUE : FALSE );
}

static boolean c1000_flash_fixed_check_vpp (void)
{
    uchar vendor = cookie->vendor;

    switch(vendor){
    case VENDOR_NEC:    /* UMI */
      return( (*c1000_vpp_control_reg & c1000_vpp_control_mask) ? FALSE : TRUE);
    case VENDOR_OKI:    /* HANA */
      return( (*c1000_vpp_control_reg & c1000_vpp_control_mask) ? TRUE : FALSE);
    default:
      return (FALSE);
    }
}


static void c1000_flash_set_volt (uchar value)
{
    uchar reg;
    reg = clpd6720_reg_read(PCM_FLASH_SOCK, POWER_CTRL_IDX);
    reg &= ~VPP1_POWER_MASK;
    reg |= (value & VPP1_POWER_MASK);
    clpd6720_reg_write(PCM_FLASH_SOCK, POWER_CTRL_IDX,reg);
    DELAY(VPP_RAMP_UP);
}

static void c1000_flash_fixed_set_volt (void)
{
    uchar vendor = cookie->vendor;

    switch(vendor){
      case VENDOR_NEC: /* UMI */
        *c1000_vpp_control_reg &= ~c1000_vpp_control_mask;
        break;
      case VENDOR_OKI: /* HANA */
        *c1000_vpp_control_reg |= c1000_vpp_control_mask;
        break;
    }
    DELAY(VPP_RAMP_UP);
}


static boolean c1000_flash_assert_vpp (void)
{
    c1000_flash_set_volt(VPP1_POWER_12V);
    return (c1000_flash_check_vpp());
}

static boolean c1000_flash_fixed_assert_vpp (void)
{
    c1000_flash_fixed_set_volt();
    return (c1000_flash_fixed_check_vpp());
}


static boolean c1000_flash_deassert_vpp (void)
{
    c1000_flash_set_volt(VPP1_POWER_VCC);
    DELAY(VPP_RAMP_DOWN);
    return (c1000_flash_check_vpp());
}

static boolean c1000_flash_fixed_deassert_vpp (void)
{
    uchar vendor = cookie->vendor;

    switch(vendor){
      case VENDOR_NEC: /* UMI */
        *c1000_vpp_control_reg |= c1000_vpp_control_mask;
        break;
      case VENDOR_OKI: /* HANA */
        *c1000_vpp_control_reg &= ~c1000_vpp_control_mask;
        break;
    }
    DELAY(VPP_RAMP_DOWN);
    return (c1000_flash_fixed_check_vpp());
}


static boolean c1000_flash_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (c1000_flash_assert_vpp());
    else       /* Turn Vpp OFF */
        return (c1000_flash_deassert_vpp());
}

static boolean c1000_flash_fixed_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (c1000_flash_fixed_assert_vpp());
    else       /* Turn Vpp OFF */
        return (c1000_flash_fixed_deassert_vpp());
}


/*
 * This code will find end of flash memory in case the hardware     *
 * will not return a buserror at the end of flash memory.           *
 * The requirements are                                             *
 * The flash should support returning manufacture and device ids    *
 * The flash memory should be contiguous                            *
 * Inputs: startaddr - Beginning of flash                           *
 *         next addr - This is the address to be checked for flash  *
 *         flash_width - Bus width of the system                    *
 *         code - Code returned by the first bank = manu << 8 | dev *
 * Output: True if flash has ended and wrap around detected         *
 *         False if another bank found at the address               *
 */

static boolean  check_flash_end (uchar *startaddr, uchar *nextaddr,
				 int flash_width, ushort code)
{
    uchar manu[4],device[4];
    ushort id_code;
 
    switch (flash_width) {

    case BW_16BITS:

        *(volatile ushort *)startaddr = (ushort)reset_cmd;
        *(volatile ushort *)nextaddr = (ushort)reset_cmd;  /* reset chips */
            ;
        *(volatile ushort *)nextaddr = (ushort)read_id;    /* Read chip IDs */
            ;
        /* get manuf code */
        *(ushort *)manu   = *(volatile ushort *)startaddr;
        startaddr += sizeof(short);
        /* get device code */
        *(ushort *)device = *(volatile ushort *)startaddr;
        *(volatile ushort *)nextaddr = (ushort)reset_cmd;
        break;
 
    case BW_32BITS:
 
        *(volatile ulong *)startaddr = (ulong)reset_cmd;
        *(volatile ulong *)nextaddr = (ulong)reset_cmd;  /* reset chips */
            ;
        *(volatile ulong *)nextaddr = (ulong)read_id;    /* Read chip IDs */
            ;
        /* get manuf code */
        *(ulong *)manu   = *(volatile ulong*)startaddr;
        startaddr += sizeof(long);
        /* get device code */
        *(ulong *)device = *(volatile ulong*)startaddr;
        *(volatile ulong *)nextaddr = (ulong)reset_cmd;
        break;
 
    case BW_8BITS:
    default:
 
        *(volatile uchar *)startaddr = (uchar)reset_cmd;
        *(volatile uchar *)nextaddr = (uchar)reset_cmd;  /* reset chips */
            ;
        *(volatile uchar *)nextaddr = (uchar)read_id;    /* Read chip IDs */
            ;
        /* get manuf code */
        *(uchar *)manu   = *(volatile uchar *)startaddr;
        startaddr += sizeof(uchar);
        /* get device code */
        *(uchar *)device = *(volatile uchar *)startaddr;
        *(volatile uchar *)nextaddr = (uchar)reset_cmd;
        break;
    } 

    id_code = (manu[0] << 8) | device[0];
 
    if (id_code == code)              /* wrap around */
        return (1);
    return (0);
}

/**************************************************************************/
/***   Flash Bus Width sizing code.                                       */
/***   This routine attempts to get a valid mfg / device id from the      */
/***   device assuming it is 8 32 or 16 bits.  If more than one give a    */
/***   valid ID, the precedence is given as follows: 16, 32, 8.           */
/***                                                                      */
/***   Input:  dev->dev_geom    <- geometry struct for flash              */
/***           dev->dev_addr     <- base address of flash memory          */
/***           dev->dev_state    <- uses DEV_STATE_RFF for beesley stuff  */
/***                                                                      */
/***   Output: dev->dev_width, dev->dev_wmask  - indicates bus size.      */
/***           fg->fg_chip_code                - indicates which chiptype */
/***           returns chiptype.                                          */
/**************************************************************************/

static ushort calc_flash_width (dev_info_t *dev,ulong bankaddr)
{
    uchar     mfg[8],dv[8];
    ushort    chip_code;
    flash_geom_t *fg;
    int id;
    int chip;

    fg = (flash_geom_t *)dev->dev_geom;

    if (fg == NULL)
      return(0);
    for (chip = 0; chip < MAX_CHIPS_IN_BANK; chip++) 
       fg->fg_chip_code[chip] = 0;          /* mark it as unknown for now. */

    dev->dev_width = BW_16BITS;     /* assume chips are 16 bits wide.*/
    dev->dev_wmask = BW_16BITS_MASK;
    fg->fg_width =  dev->dev_width;
    fg->fg_wmask =  dev->dev_wmask;
    flash_bank_query_wrapper(dev, bankaddr, (ulong *)&mfg[0],(ulong *)&dv[0]);
    chip_code = ((mfg[0] << 8) | dv[0]);
    id = flash_chip_identify(chip_code);
    if (id != 0)
    {
        for (chip = 0; chip < fg->fg_width; chip++)
            fg->fg_chip_code[chip] = chip_code;
        return(fg->fg_chip_code[0]);
    }
    dev->dev_width = BW_32BITS;     /* assume chips are 32 bits wide.*/
    dev->dev_wmask = BW_32BITS_MASK;
    fg->fg_width =  dev->dev_width;
    fg->fg_wmask =  dev->dev_wmask;
    flash_bank_query_wrapper(dev,bankaddr,(ulong *)&mfg[0],(ulong *)&dv[0]);
    chip_code = ((mfg[0] << 8) | dv[0]);
    id = flash_chip_identify(chip_code);
    if (id != 0)
    {
        for (chip = 0; chip < fg->fg_width; chip++)
            fg->fg_chip_code[chip] = chip_code;
        return(fg->fg_chip_code[0]);
    }
    dev->dev_width = BW_8BITS;     /* assume chips are 8 bits wide.*/
    dev->dev_wmask = BW_8BITS_MASK;
    fg->fg_width =  dev->dev_width;
    fg->fg_wmask =  dev->dev_wmask;
    /* try to identify the chip */
    flash_bank_query_wrapper(dev, bankaddr, (ulong *)&mfg[0],(ulong *)&dv[0]);
    chip_code = ((mfg[0] << 8) | dv[0]);
    id = flash_chip_identify(chip_code);
    if (id != 0)
    {
        for (chip = 0; chip < fg->fg_width; chip++)
            fg->fg_chip_code[chip] = chip_code;
        return(fg->fg_chip_code[0]);
    }
    dev->dev_width = BW_16BITS;     /* Default if chips are unknown. */
    dev->dev_wmask = BW_16BITS_MASK;
    fg->fg_width =  dev->dev_width;
    fg->fg_wmask =  dev->dev_wmask;
    return(0);
}

/**************************************************************************/
/***   RAM Sizing Code.                                                   */
/***   assumes rams come in increments of 1megabytes.                     */
/***                                                                      */
/***                                                                      */
/***                                                                      */
/**************************************************************************/

static uint ram_size (dev_info_t *dev)
{
    uint i;
    volatile ushort *addr,*first_addr;
    ushort pattern,chk_val,first_val,tmp_val;
    ushort save_first;
    int status;
    uint ofs=0;
    flash_geom_t *fg;


    if (dev != NULL) {
        fg = (flash_geom_t *)dev->dev_geom;
        if (fg != NULL) {
	    if (fg->fg_status == FR_RAM) {
                addr = (ushort *)dev->dev_addr;
		first_addr = addr;
                status = raise_interrupt_level(ALL_DISABLE);
                pattern = 0x0101; 
                save_first = *addr;
                *addr = pattern;
                for (i = 0; i < 255; i++, pattern += 0x0101, 
                                     addr += (64*1024)/sizeof(ushort)) {
                    tmp_val =  *addr;	/* save so we can put it back     */
                    *addr = pattern;	/* write pattern at beg. of block */
		    chk_val = *addr;    /* see if we read what we wrote   */
		    chk_val = *addr;    /* see if we read what we wrote   */
		    first_val = *first_addr;  /* chk if we wrote over 1st */
                    *addr = tmp_val;    /* put the orig. val back         */
		    if ( (chk_val != pattern) || (first_val != 0x0101) )
			break;
		    ofs += 64*1024;
                }
                *first_addr = save_first;	/* put back first word */
		reset_interrupt_level(status);
            }
        }
    }
    return (ofs);
}

static int c1000_present_fn (dev_info_t *dev)
{
    int present=0;
    if (dev != NULL)
    {
        if (pcmcia_card_present(dev) == TRUE )
            present |= DEV_PRESENT;
        if (pcmcia_status_change(dev) & CARD_DETECT_CHANGE_MASK) {
            present |= DEV_CHANGED;
        }
    }
    return(present);
}

/*---------------------------------------------------------------*/
/* query the device and create its partitions, etc.              */
/*---------------------------------------------------------------*/
static void c1000_insert_fn (dev_info_t *dev)
{
    if (dev != NULL) {
        printf("\nFlash card inserted. Reading filesystem...");
        flush();
        c1000_initialize_dev(dev);
        dev->dev_flags = 0;
        dev->dev_ocnt  = 0;
        if (dev->devcons)
            dev->devcons->dev_cons_flags = 0;

        printf("done.\n");
        flush();
    }
}

/*---------------------------------------------------------------*/
/* throws away non-first partition stuff into a holding bin, and */
/* marks the first partition/ devcons/geom to indicate empty.    */
/*---------------------------------------------------------------*/
static void c1000_remove_fn (dev_info_t *dev)
{
    dev_cons_info_t *devcons;
    flash_geom_t    *ptr_geom;
    flash_t         * fs_spec;
    dev_info_t      *dev_tmp_ptr;

    int             i;



    if (dev != NULL) {
        /*--------------------------------------------------------- */
        /*  First clean up the devcons to look removed.             */
        /*--------------------------------------------------------- */
        devcons = dev->devcons;
        if (devcons  != NULL) {
            devcons->dev_addr       = (ulong)dev;
            devcons->dev_size       = 0;
            devcons->dev_end        = devcons->dev_addr;
            devcons->dev_width      = 0;
            devcons->dev_wmask      = 0;
            devcons->dev_banksize   =1;
            devcons->dev_part_cnt   =0;
            devcons->dev_rwcnt      =0;
            devcons->dev_rocnt      =0;
            devcons->dev_first_rw   =0;
	    TIMER_STOP(devcons->dev_init_time);
	    devcons->dev_wr_prot    =TRUE;
        }
        /*--------------------------------------------------------- */
        /*  Then clean up all partitions of a device.               */
        /*--------------------------------------------------------- */
        while (dev->prev_partition) { /* do all partitions.         */
            dev = dev->prev_partition;
        }

        /*--------------------------------------------------------- */
	/*  For insertion/removal, we wish to move the non-first    */
	/*  partitions to dev_pending_delete, but before we can do  */
	/*  that, we must make sure dev_pending_delete has nothing  */
	/*  on it.  If it does, then clear it out first.            */
	/*  Since the 2nd thru nth partition is here, only the dev, */
        /*  fs_spec/gen, and geom struct will actually be           */ 
	/*  deleted, since the first dev and its devcons, etc. are  */
	/*  reused from one insertion to the next.                  */
        /*--------------------------------------------------------- */
	
	while (dev_pending_delete != NULL)
	{
	    /* Free up memory used for fs specific information */
            if (dev_pending_delete->fs_spec_info)
                reg_invoke_file_system_mem_free(DEVICE_LES_FLASH,
                  (void *)dev_pending_delete->fs_spec_info, DEV_FS_SPEC_INFO);

            /* Free up memory used for fs generic information */
            if (dev_pending_delete->fs_gen_info)
                reg_invoke_file_system_mem_free(DEVICE_LES_FLASH,
                  (void *)dev_pending_delete->fs_gen_info, DEV_FS_GEN_INFO);

            if (dev_pending_delete->dev_geom)
                free(dev_pending_delete->dev_geom);

	    dev_tmp_ptr = dev_pending_delete;
	    dev_pending_delete = dev_pending_delete->next_partition;
	    free(dev_tmp_ptr);
        }
	dev_tmp_ptr = dev;

        while (dev)                 /* clean up dev_info_t          */
        {
            dev->dev_state      = DEV_STATE_REMOVED;
            dev->dev_addr       = (ulong)dev;
            dev->dev_size       = 0;
            dev->dev_end        = dev->dev_addr;
            dev->dev_start_bank = 0;
            dev->dev_width      = 0;
            dev->dev_wmask      = 0;
            dev->dev_banksize   = 1;

            /*----------------------------------------------------- */
            /*  If there is not geom create one, then say empty.    */
            /*----------------------------------------------------- */
            ptr_geom = (flash_geom_t *)dev->dev_geom;
            if (ptr_geom == NULL) {
                ptr_geom = malloc(sizeof(flash_geom_t));
                if (ptr_geom != NULL) {
                    dev->dev_geom             = ptr_geom;
                    ptr_geom->fg_cmnd_set     = 0;
                    ptr_geom->fg_card         = "processor board";
                    ptr_geom->fg_ctlr         = "Cirrus 6720";
                    for ( i = 0; i < MAX_CHIPS; i++) {
                        ptr_geom->fg_chip_code[i] = 0;
                        ptr_geom->fg_chip_id[i]   = "    ";
                        ptr_geom->fg_rom_id[i]  = 0;
                        ptr_geom->fg_bank_no    = 0;
                        ptr_geom->fg_chip_name[i]  = "Unknown";
                    }
                }
            }
            if (ptr_geom != NULL) {
                ptr_geom->fg_status = FR_REMOVED;
                ptr_geom->fg_chips = 0;
                ptr_geom->fg_chipsize = 0;
                ptr_geom->fg_banksize = 0;
                ptr_geom->fg_sectors=0;
                ptr_geom->fg_sectorsize=0;
                ptr_geom->fg_cmnd_set=0;
                ptr_geom->fg_width=0;
                ptr_geom->fg_wtries=0;
                ptr_geom->fg_etries=0;
            } else {
	       printf("\nCannot initialize flash subsystem due to memory");
	       printf(" exhaustion.\n");
            }
            fs_spec = (flash_t *)dev->fs_spec_info;
            if (fs_spec != NULL)
            {
                /*------------------------------------------------- */
                /* if an fs specific info exists, mark as empty.    */
                /*------------------------------------------------- */
                fs_spec->dev_part_cnt  = 0;
                fs_spec->dev_size      = 0;
                fs_spec->dev_banksize  = 1;
                fs_spec->fs_size       = 0;
                fs_spec->fs_status     = dev->dev_state;
                fs_spec->f_free_offset = 0;
                fs_spec->f_filecnt     = 0;
            }

            dev = dev->next_partition;
            if (dev)
                dev_clear_init_table_slot(dev->dev_unit);
        }
	/* once all partitions have been zero'ed, then drop then  */
	/* onto deathrow, and remove them from the main dev       */
	if (dev_tmp_ptr) { 
	    dev_pending_delete = dev_tmp_ptr->next_partition;
	    dev_tmp_ptr->next_partition = NULL;
        }
    }
}

/**************************************************************************/
/***   Flash Sizing code.                                                 */
/***   The fixed  flash window is set to 16 megabytes.  The flash memory  */
/***   is usually smaller than that, and the unused part of the 16 mb     */
/***   space will not bus error, but just return wraps.  The algorithm    */
/***   here is :   if memory is not present, size = 0.                    */
/***               else calculate the end of flash memory                 */ 
/***   Then set the OR register correctly to give buserror                */ 
/**************************************************************************/


static void c1000_flash_fixed_size (dev_info_t *dev)
{

    volatile  ulong           min_flash_size;
    volatile  ulong           max_flash_size;
    volatile  int             max_flash_banks;
    volatile  ulong           ofs=0;
    volatile  int             id;
    volatile  signal_handler  oh;
    volatile  dev_cons_info_t *devcons;
    volatile  flash_geom_t    *fg;
              flash_geom_t    *fg_tmp;
              int             chip;
    volatile ulong mask;
 
    /* Kludge: To force the compiler to not place the following variables */
    /*         into registers (which could then get clobbered by setjmp)  */
    /*         we have to take the address of them, making them ineligible*/
    /*         as register variables.  Yeeeeech!!!!                       */
 
    devcons = (dev_cons_info_t *)&fg;
 
    devcons = dev->devcons;
    max_flash_size = ADRSPC_PCMCIA_MSPC_SIZ;   /* 16 megabytes */
    max_flash_banks= 8;                        /* Assume  */
    min_flash_size = 2L * ONE_MB;           /* assume minimum 2MB*/
 
 
    /*
     * Malloc a flash_geom_t structure and initialize it.
     * Attach it to the dev_info_t structure.
     */
    if (dev->dev_geom == NULL) {
        fg = malloc(sizeof(flash_geom_t));
        if (!fg) {  /* malloc failure */
            return;
        }
        dev->dev_geom = (void *)fg;
    }
    else
        fg = (flash_geom_t *)dev->dev_geom;
 
    /*
     * Initialize the geometry fields. Make a whole lot of
     * initial assumptions since we haven't queried the chips.
     */
    fg->fg_status       = FR_NOT_QUERIED;  /* not yet queried */
 
    dev->dev_size    = 0;
    dev->dev_width   = BW_16BITS;     /* assume chips are 16 bits wide.*/
    dev->dev_wmask   = BW_16BITS_MASK;
    fg->fg_width     =  dev->dev_width;
    fg->fg_chips     =  dev->dev_width;
    fg->fg_wmask     =  dev->dev_wmask;
    for (chip=0; chip < MAX_CHIPS_IN_BANK; chip++)
        fg->fg_chip_code[chip] = 0;
    fg->fg_chipsize  = ONE_MB;
    fg->fg_banksize  = fg->fg_chipsize * fg->fg_width;
    fg_tmp = (flash_geom_t *)fg;
    if ( (devcons) && (devcons->dev_timer_fn))
        (*devcons->dev_timer_fn)(&fg_tmp->fg_usec6,
                                 &fg_tmp->fg_usec10, FALSE);
 
 
    oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
    if ( setjmp(&berr_buf) == 0) {
 
        (*devcons->dev_vpp_fn)(VPP_ON);
         while ( (max_flash_banks) || (ofs < min_flash_size)) {
             if (max_flash_banks) {
                 max_flash_banks--;
             }
             if (fg->fg_chip_code[0] == 0) {
                 if ( calc_flash_width(dev,ofs) == 0) {
                        break;
                 }
             }
             else {
                 if (check_flash_end( (unsigned char *)dev->dev_addr,
                     (unsigned char *)(dev->dev_addr)+ofs,
                     dev->dev_width, (ushort) fg->fg_chip_code[0]))
                      break;
             }
 
             id = flash_chip_identify(fg->fg_chip_code[0]);
             fg->fg_chipsize    = flash_chip_size(id);
             fg->fg_banksize    = fg->fg_chipsize * fg->fg_width;
             ofs += fg->fg_banksize;
             if ((ofs>max_flash_size) || (fg->fg_banksize==0)){
                 ofs -= fg->fg_banksize;
                     break;
             }
         }
         (*devcons->dev_vpp_fn)(VPP_OFF);
    } else {     /* Bus error signal ! */
        (*devcons->dev_vpp_fn)(VPP_OFF);
    }
    signal_oneshot(SIGBUS, oh);

    /* adjust the OR5 register according to the size */
    mask = (~(ofs -1)) & 0x00f00000;    /* 1 MB resolution */
    dpr->regs.memc_or5 |= mask ;
 
    dev->dev_size = ofs;
    devcons->dev_size = dev->dev_size;
    dev->dev_end  = dev->dev_addr + dev->dev_size;
    devcons->dev_end = dev->dev_end;
    devcons->dev_width = dev->dev_width ;
    devcons->dev_wmask = dev->dev_wmask ;
}

/**************************************************************************/
/***   Flash Sizing code.                                                 */
/***   The pcmcia memory window is set to 16 megabytes.  The flash cards  */
/***   are usually smaller than that, and the unused part of the 16 mb    */
/***   space will not bus error, but just return 'ff's.  The algorithm    */
/***   here is :   if card is not present, size = 0.                      */
/***               else assume cards start at chip boundary, and use the  */
/***               mfg id's of the chips on the card to determine size.   */
/***               keep going until no mfg id is found.                   */
/**************************************************************************/


static void c1000_flash_size (dev_info_t *dev)
{

    volatile  ulong           min_flash_size;
    volatile  ulong           max_flash_size;
    volatile  int             max_flash_banks;
    volatile  ulong           ofs=0;
    volatile  ulong           flash_size=0;
    volatile  int             id;
    volatile  signal_handler  oh;
    volatile  dev_cons_info_t *devcons;
    volatile  flash_geom_t    *fg;
    volatile  uchar           device_type = pcm_device_type(dev);
              flash_geom_t    *fg_tmp;
              int             chip;

    /* Kludge: To force the compiler to not place the following variables */
    /*         into registers (which could then get clobbered by setjmp)  */
    /*         we have to take the address of them, making them ineligible*/
    /*         as register variables.  Yeeeeech!!!!                       */

    devcons = (dev_cons_info_t *)&fg;

    devcons = dev->devcons;
    max_flash_size = ADRSPC_PCMCIA_MSPC_SIZ;   /* 16 megabytes */
    max_flash_banks= 1;			       /* if no attrib space */
    min_flash_size = 2L * ONE_MB;	       /* assume all cards are >= 2MB*/


    /*
     * Malloc a flash_geom_t structure and initialize it.
     * Attach it to the dev_info_t structure.
     */
    if (dev->dev_geom == NULL) {
        fg = malloc(sizeof(flash_geom_t));
        if (!fg) {  /* malloc failure */
            return;
        }
	dev->dev_geom = (void *)fg;
    }
    else
        fg = (flash_geom_t *)dev->dev_geom;

    /*
     * Initialize the geometry fields. Make a whole lot of
     * initial assumptions since we haven't queried the chips.
     */
    fg->fg_status       = FR_NOT_QUERIED;  /* not yet queried */

    dev->dev_size    = 0;
    dev->dev_width   = BW_16BITS;     /* assume chips are 16 bits wide.*/
    dev->dev_wmask   = BW_16BITS_MASK;
    fg->fg_width     =  dev->dev_width;
    fg->fg_chips     =  dev->dev_width;
    fg->fg_wmask     =  dev->dev_wmask;
    for (chip = 0; chip < MAX_CHIPS_IN_BANK; chip++)
        fg->fg_chip_code[chip] = 0; 
    fg->fg_chipsize  = ONE_MB;
    fg->fg_banksize  = fg->fg_chipsize * fg->fg_width;
    fg_tmp = (flash_geom_t *)fg;
    if ( (devcons) && (devcons->dev_timer_fn))
        (*devcons->dev_timer_fn)(&fg_tmp->fg_usec6, 
                                 &fg_tmp->fg_usec10, FALSE);

    if (dev_chk(dev)) {
        oh = signal_oneshot(SIGBUS, c1000_handle_buserror);
        if ( setjmp(&berr_buf) == 0) {
    	    if ( (device_type == PCM_DEV_FLASH )  ||
    	         (device_type == PCM_DEV_SRAM  ) ) {
                /*-------------------------------------------------------*/
                /*---Size memory cards which have attribute space--------*/
                /*-------------------------------------------------------*/
    	        flash_size = pcm_flash_size(dev);
    	        if ( flash_size > 0) {
    		    max_flash_banks = 9999; /* more than are on the card */
    		    if (flash_size <= max_flash_size) {
    		        max_flash_size = flash_size;
                    }
                }
            }
    	    if (device_type != PCM_DEV_UNKNOWN ) {
                for (chip = 0; chip < fg->fg_width; chip++)
                    fg->fg_chip_code[chip] = pcm_flash_chips(dev);
    	        if (devcons->dev_wr_prot == 0) {
                /*----------------------------------------------------------*/
                /*-For any memory card that is not write protected         -*/
                /*----------------------------------------------------------*/
                    flash_probe_memory_type(dev);
                    if (fg->fg_status == FR_RAM) {
                        ofs = ram_size(dev);
                    } else {
                        (*devcons->dev_vpp_fn)(VPP_ON);
        	        while ( (max_flash_banks) || (ofs < min_flash_size)) {
        	            if (max_flash_banks) {
        	                max_flash_banks--;
                            }
                            if (fg->fg_chip_code[0] == 0) {
        	                if ( calc_flash_width(dev,ofs) == 0) {
        	                    break;
                                }
                            }
                            id = flash_chip_identify(fg->fg_chip_code[0]);
                            fg->fg_chipsize    = flash_chip_size(id);
                            fg->fg_banksize    = fg->fg_chipsize * fg->fg_width;
                            ofs += fg->fg_banksize;
        		    if ((ofs>max_flash_size) || (fg->fg_banksize==0)){
                                ofs -= fg->fg_banksize;
        		        break;
        		    }
        	        }
                        (*devcons->dev_vpp_fn)(VPP_OFF);
                    }
                } else  {
                    /*------------------------------------------------------*/
                    /*-For any memory card that is write protected.        -*/
                    /*------------------------------------------------------*/
		    dev->dev_state = DEV_STATE_RDONLY;
                    if (flash_size) {
                        fg->fg_banksize  = ofs = flash_size;
                        if (fg->fg_chip_code[0]) {
                            id = flash_chip_identify(fg->fg_chip_code[0]);
                            fg->fg_chipsize    = flash_chip_size(id);
                            fg->fg_banksize    = fg->fg_chipsize * fg->fg_width;
                        }
                    } else {
                        fg->fg_banksize  = ofs = min_flash_size;
                    }
                    fg->fg_chipsize  = fg->fg_banksize / fg->fg_width;
                }
            }
        } else {     /* Bus error signal ! */
            ofs = 0;
	    dev->dev_addr = (ulong)dev;
        }
        signal_oneshot(SIGBUS, oh);
    }
    dev->dev_size = ofs;
    devcons->dev_size = dev->dev_size;
    dev->dev_end  = dev->dev_addr + dev->dev_size;
    devcons->dev_end = dev->dev_end;
    devcons->dev_width = dev->dev_width;
    devcons->dev_wmask = dev->dev_wmask;
    devcons->dev_simm_size = flash_size;
}

static void c1000_flash_timer (ushort *v6, ushort *v10, boolean print_timer)
{
   /* Note: timer_calibrate would normally be used, here, but since */
   /* we have hardware support in quicc, its not needed.   just     */
   /* return want we get.                                           */
    *v6  = 6;     
    *v10 = 10;
}

static boolean c1000_data_bus_reversed (void)
{
    return  (FALSE); 
}


/*
 * This updates the entry in the array c1000_flash_devices after dynamic
 * re-partitioning has been done.
 */
static void c1000_replace_flash_devices_entry (char *devnm, void *dev)
{
    int indx;
    dev_info_t *workptr;
  
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++) {
        workptr = c1000_flash_devices[indx];
        if (workptr == NULL) /* Skip empty slots */
            continue;
        if (strcmp(workptr->dev_alias, devnm) == 0)
            break;
    }
    if (indx < MAX_DEV_INIT_ENTRIES)
        c1000_flash_devices[indx] = (dev_info_t *)dev;
}

/* This function determines whether partitioning of flash devices is to be 
 * allowed for this platform. The 1000 platform will always support
 * partitioning.
 */
static boolean c1000_allow_flash_partitioning (void)
{
    return (TRUE);
}

/*
 * get_interface_val
 * return the bit defns representating the system interfaces
 * based on the cookie interface value
 */
static int get_interface_val (void)
{
    return(interface_bit_table[cookie->interfaces]);
}

/*
 * c1000_image_family_check
 * verify the image being downloaded/netbooted is fine for this platform
 * Return value 0 = image correct for this box
 * return value -1 = invalid image.
 */
static int c1000_image_family_check (char *buff)
{
    SUN *aoutptr = (SUN *)buff;
    struct family_check *fc_ptr;
    int interface = get_interface_val();

    fc_ptr = (struct family_check *)&aoutptr->a_trsize;
    if (aoutptr->a_trsize) {
	/*
	 * If a.out header is not stamped with FAMILY_CHECK_MAGIC,
	 * we cannot make any assumptions about the type of image
	 * we are dealing with; ie. could be an old image.
	 * Assume the image is valid.
	 */
	if (fc_ptr->magic == FAMILY_CHECK_MAGIC) {
	    if (fc_ptr->family == cpu_family){
		if (fc_ptr->interface == interface) {
		    /*
		     * If this is a ciscoPro platform, then issue a warning
		     * message if the a.out header is NOT stamped with
		     * CISCOPRO_IMAGE_FLAG. CiscoPro platforms can only run
		     * CiscoPro images.
		     */
		     if (ciscopro_platform) {
			if (fc_ptr->flags & CISCOPRO_IMAGE_FLAG)
			    return(0);
			else
			    return(-1);
		     }
		    else return(0);
		}
		return(-1);
	    }
	    return(-1);
	}
    }
    return(0);
}


/* ------------------- registry functions ------------------------- */

static boolean RFF_recommended_system (void)
{

    /*
     * For Sapphire this must be false ..
     */
    return (FALSE);
}

static char *c1000_default_flash_device (void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &c1000_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        if ((*dev)->dev_type == FLASH_PRIMARY)
            return ((*dev)->dev_alias);
        dev++;
        i++;
    }
    return (NULL);
}

/*
 * Function called by "show version" to display one-liner about flash
 * found on the system.
 */
static void c1000_flash_print_hardware (void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &c1000_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        dev_info((*dev)->dev_alias, SHOW_VER_FLASH_SIZE);
        dev++;
        i++;
    }
}

static boolean aout_image_reloc_test (SUN *mptr)
{
    /*
     * take a pointer to an a.out header and look to see if
     * this is a relocatable image.
     * The 1000 product does not support relocatable images yet
     * igs-x-l images should not be relocated
     */ 
    return(FALSE);
}

/*
 * Include file that contains code for validating SUN a.out executables.
 */
#include "../dev/flash_aout.h"


/*
 * Table of Flash devices for 1000/2000 (IGS) products.
 */

static const dev_table_t dev_table_pcmcia[] = {
    { 0, FLASH_PRIMARY,"PCMCIA flash", SF, FLASH_BASE, 0,
      c1000_flash_vpp, c1000_flash_check_vpp, 0, c1000_flash_size,
      c1000_flash_open, c1000_flash_close, c1000_flash_access },
 
    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

static const dev_table_t dev_table_fixed[] = {
    { 0, FLASH_PRIMARY,"System flash", SF, FLASH_BASE_FIXED, 0,
      c1000_flash_fixed_vpp, c1000_flash_fixed_check_vpp, 0, 
      c1000_flash_fixed_size, c1000_flash_open, c1000_flash_close, 
      c1000_flash_access },

    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

/*********************************************************************
 *                  Parser stuff for system flash commands
 ********************************************************************/
static boolean flash_has_multiple_partitions (char *dev_name)
{
    devstat_t devinfo;

    dev_stat(dev_name, &devinfo); 
    if (devinfo.dev_part_cnt > 1)
	return (TRUE);
    else 
	return (FALSE);
}
static boolean flash_alias_match (char *alias)
{
    dev_info_t **dev;
    int i = 0;

    dev = &c1000_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        if (strcmp(alias, (*dev)->dev_alias) == 0) /* matched */
            return (TRUE);
        dev++;
        i++;
    }
    return (FALSE);
}

/*
 * Parse chains for Flash copy commands
 */
#define	ALTERNATE	NONE
#include "../dev/exec_copy_flash.h"
LINK_POINT(flash_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Flash show commands
 */
#define	ALTERNATE	NONE
#include "../dev/exec_show_flash.h"
LINK_POINT(flash_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for verify and erase exec commands. 
 */
#define	ALTERNATE	NONE
#include "../dev/exec_erase_cmnds.h"
LINK_POINT(exec_erase_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "../dev/exec_verify_cmnds.h"
LINK_POINT(exec_verify_commands, ALTERNATE);
#undef	ALTERNATE

#define ALTERNATE       NONE
#include "../dev/cfg_flash_partition.h"
LINK_POINT(flash_partition_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for Flash
 */
static parser_extension_request system_flash_chain_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(flash_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(flash_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_erase_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_verify_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(flash_partition_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * flash_parser_init - Initialize Flash parser support
 */
static void c1000_flash_parser_init (void)
{
    if (!system_loading) {
	/* copy flash ... */
	parser_add_link_point(PARSE_ADD_COPY_FLASH_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_extend_here));
    }

    parser_add_command_list(system_flash_chain_table, "system flash");
}
/*
 * c1000_flash_needs_buffer
 * Status if we can do device checks directly on flash device or whether
 * we need to copy data to be checked into a buffer.  This is done to 
 * work around the H/W limitation of only being able to read a 
 * ushort at a time from the PCMCIA device.
 */
static boolean c1000_flash_needs_buffer (void)
{
    return(TRUE);
}

/************************************************************
* Platform dependent function to support Initialization     *
* Currently defined for Sapphire with on-board flash        *
* Modify if needed for newer platforms                      *
************************************************************/

static void c1000_flash_fixed_hw_register_init (void)
{
    uchar vendor = cookie->vendor;
    dpr = (quicc_dpr_t *) getmbar();

    dpr->regs.memc_or5 = QUICC_FIXED_OR5_VAL;
    dpr->regs.memc_br5 = QUICC_FIXED_BR5_VAL;

    switch(vendor){
      case VENDOR_NEC: /* UMI */
        dpr->regs.pio_pcpar &= ~QU_PT_C_FIXED_VPP_CTRL_UMI;
        dpr->regs.pio_pcdir |= QU_PT_C_FIXED_VPP_CTRL_UMI;
        dpr->regs.pio_pcso  &= ~QU_PT_C_FIXED_VPP_CTRL_UMI;
        dpr->regs.pio_pcdat |= QU_PT_C_FIXED_VPP_CTRL_UMI;
        c1000_vpp_control_reg = &(dpr->regs.pio_pcdat);
        c1000_vpp_control_mask = QU_PT_C_FIXED_VPP_CTRL_UMI;
        break;
      case VENDOR_OKI: /* HANA */
        dpr->regs.pio_pcpar &= ~QU_PT_C_FIXED_VPP_CTRL_HANA;
        dpr->regs.pio_pcdir |= QU_PT_C_FIXED_VPP_CTRL_HANA;
        dpr->regs.pio_pcso  &= ~QU_PT_C_FIXED_VPP_CTRL_HANA;
        dpr->regs.pio_pcdat |= QU_PT_C_FIXED_VPP_CTRL_HANA;
        c1000_vpp_control_reg = &(dpr->regs.pio_pcdat);
        c1000_vpp_control_mask = QU_PT_C_FIXED_VPP_CTRL_HANA;
        break;
      default:
        break;
    }
}

/*
 * Init function for this sub-system.
 */
static void flash_init (subsystype *subsys)
{
    const dev_table_t *dev_tbl_ptr;
    dev_info_t *dev;
    int count = 0, indx;
    leveltype level;
    boolean device_pcmcia;

    /*
     * Do one-time initializations.
     */
    dev_pending_delete=NULL;

    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++)
        c1000_flash_devices[indx] = NULL;
    dev_io_init();  /* dev_init_table and dev_open_table */
    file_io_init(); /* file_open_table */

    if (touch_device((void *)ADRSPC_PCMCIA_IO)) {
        device_pcmcia = TRUE;
        dev_tbl_ptr = &dev_table_pcmcia[0];
    } else {
        device_pcmcia = FALSE;
        dev_tbl_ptr = &dev_table_fixed[0];
    }

    level = set_interrupt_level(ALL_ENABLE);
    for ( ;
         dev_tbl_ptr->dev_unit != -1;
         dev_tbl_ptr++) {
        /*
         * Create data structure for device. Get back pointer.
         * Assume sub-device number of 1 since we don't support
         * sub-devices yet.
         */
        if ((dev = dev_create(dev_tbl_ptr)) != NULL) {
            count++;
            (dev->devcons)->dev_removable = device_pcmcia;
            c1000_initialize_dev(dev);
        }
    }  /* end for loop */
    reset_interrupt_level(level);

    if (count > 0) {  /* atleast one flash device found and initialized */
        c1000_flash_parser_init();
        reg_add_flash_download_verify(flash_download_verify,
                                      "flash download verification");
        reg_add_flash_boot_verify(flash_boot_verify,
                                  "flash boot verification");
        reg_add_RFF_exec_check(aout_image_RFF_exec_checks, 
                               "RFF execution check");
        reg_add_flash_print_hardware(c1000_flash_print_hardware, 
                                     "print flash hardware");
	reg_add_classify_image(sniff_image_type,
			       "classify image type");
	reg_add_check_rel_image(aout_image_check_rel_image,
				"validate relocatable image");
	reg_add_flash_needs_buffer(c1000_flash_needs_buffer,
				   "flash_needs_buffer");
	reg_add_replace_flash_devices_entry(c1000_replace_flash_devices_entry,
					    "replace flash devices entry");
        reg_add_allow_flash_partitioning(c1000_allow_flash_partitioning,
                                         "c1000_allow_flash_partitioning");
	reg_add_image_family_check(c1000_image_family_check,
				   "c100__image_family_check");
    }
    reg_add_flash_device_default(c1000_default_flash_device, 
                                 "default flash device");
    reg_add_flash_alias_match(flash_alias_match, "flash_alias_match");
}
static void c1000_initialize_dev (dev_info_t *dev)
{
    dev_info_t *workptr;
    dev_cons_info_t *devcons;
    int count = 0, indx, i;
    fs_bounds_t *fsbounds;
    int part_bounds[FLASH_MAX_PART_CNT+2];
    ulong save_flags     = dev->dev_flags;
    ulong save_ocnt;

    /* Initialize the fsbounds array */
    fsbounds = malloc(sizeof(fs_bounds_t) * FLASH_MAX_PART_CNT);
    if (!fsbounds)
        return;
    for (indx = 0; indx < FLASH_MAX_PART_CNT; indx++) {
        fsbounds[indx].start_bank = -1;
        fsbounds[indx].end_bank   = -1;
        for (i = 0; i < FLASH_FILE_CNT; i++)
            fsbounds[indx].exception_bank[i] = -1;
    }
    devcons = dev->devcons;
    /* the device may be locked, open, or accessed when this init
     * request comes in (as a result of an unsoliticed insert).
     * since the operations required here for initialization require
     * opening for exclusive access, the flags are stowed away,
     * and restored after initialization.             
     */
    dev->dev_flags |= DEV_FLAGS_VERBOSE;

    dev->dev_cache_set = CACHE_NOT_INIT;
    devcons->dev_timer_fn = c1000_flash_timer;
    devcons->dev_data_bus_reversed = c1000_data_bus_reversed;
 
    if (devcons->dev_removable) {
        dev->dev_sock = PCM_FLASH_SOCK;
        devcons->dev_addr       = (ulong)FLASH_BASE;
        dev->dev_addr           = devcons->dev_addr;
        devcons->dev_present_fn = c1000_present_fn;
        devcons->dev_insert_fn = c1000_insert_fn;
        devcons->dev_remove_fn = c1000_remove_fn;
        dev->dev_state = DEV_STATE_NODEV; 

    /*
     * See if specified device is actually present.
     */
        pcmcia_init(dev);
        devcons->dev_wr_prot   = !(*devcons->dev_vpp_fn)(VPP_ON);
        (*devcons->dev_vpp_fn)(VPP_OFF);
        if (devcons->dev_size_fn)
            (*devcons->dev_size_fn)(dev);
    /*
     * We sized device. Now go and do device specific
     * initialization to identify chips and hence the
     * erase-write routines to use.
     */
        reg_invoke_flash_device_driver_init(DEVICE_LES_FLASH, (void *)dev);
    /*----------------------------------------------------------------*/
    /*  we assume that flash_device_driver_init has already updated   */
    /*  the devcons functions to access the flash, and now it is safe */
    /*  to put wrappers around them.  devcons->dev_[*]_fn() will call */
    /*  the new wrapper routine here which sets up for bus error      */
    /*  handling, and calls the original functions.                   */
    /*----------------------------------------------------------------*/
        if (devcons->dev_read_fn) {
            devcons->dev_read_wrap_fn = devcons->dev_read_fn;
            devcons->dev_read_fn      = c1000_dev_read;
        }
        if ((devcons->dev_write_fn)&&(devcons->dev_write_fn!=c1000_dev_write)){
            devcons->dev_write_wrap_fn = devcons->dev_write_fn;
            devcons->dev_write_fn      = c1000_dev_write;
        }
        if ((devcons->dev_erase_fn)&&(devcons->dev_erase_fn!=c1000_dev_erase)){
            devcons->dev_erase_wrap_fn = devcons->dev_erase_fn;
            devcons->dev_erase_fn      = c1000_dev_erase;
        }
        if ((devcons->dev_info_fn)&&(devcons->dev_info_fn!=c1000_dev_info)){
            devcons->dev_info_wrap_fn = devcons->dev_info_fn;
            devcons->dev_info_fn      = c1000_dev_info;
        }
    } else {
        c1000_flash_fixed_hw_register_init(); /* platform specific init */
        devcons->dev_addr       = (ulong)FLASH_BASE_FIXED;
        dev->dev_addr           = devcons->dev_addr;
        devcons->dev_wr_prot   = !(*devcons->dev_vpp_fn)(VPP_ON);
        (*devcons->dev_vpp_fn)(VPP_OFF);
        if (devcons->dev_size_fn)
            (*devcons->dev_size_fn)(dev);
    /*
     * We sized device. Now go and do device specific
     * initialization to identify chips and hence the
     * erase-write routines to use.
     */
        reg_invoke_flash_device_driver_init(DEVICE_LES_FLASH, (void *)dev);
 
    } 

    if (dev->dev_state > DEV_STATE_INIT) {
        /*------------------------------------------------------------*/
        /* halfway through init, dismiss to prevent cpuhog.           */
        /*------------------------------------------------------------*/
        if (devcons->dev_delay_fn)
            (*devcons->dev_delay_fn)(10);
	/*
         * Device can be read. Sniff file systems on device.
         */
        reg_invoke_file_system_sniff(DEVICE_LES_FLASH,
                         dev->dev_alias, (void *)fsbounds);

        /*
         * Figure out how to partition the device.
         */

        devcons->dev_part_cnt = decide_partitions(dev, fsbounds, part_bounds); 

        /* Now clone dev_info_t structures for all partitions */
        dev_clone(dev, part_bounds);

        /* Read the filesystems on all partitions */
        workptr = dev;
        for(indx = 1; workptr && (indx <= devcons->dev_part_cnt); 
                                                          indx++) {
            /* read in info and update it for a given partition */
            /* Kludge here to save away dev_flags / ocnt, since */
	    /* the device removal/insertion may have occurred in*/
	    /* the middle of an access. This call needs to have */
	    /* exclusive access to the part.                    */
            
            save_flags     = workptr->dev_flags;
            save_ocnt      = workptr->dev_ocnt;

            workptr->dev_flags = DEV_FLAGS_VERBOSE;
            workptr->dev_ocnt  = 0;

            reg_invoke_flash_device_file_system(DEVICE_LES_FLASH,
                                         workptr->dev_sub_alias, TRUE);
            workptr->dev_flags = save_flags;
            workptr->dev_ocnt  = save_ocnt;

            workptr = workptr->next_partition;
        }

        /* save dev info pointer */
        c1000_flash_devices[count++] = dev;

	/* record initialization time for device */
	GET_TIMESTAMP(devcons->dev_init_time);
 

    }
    dev->dev_flags = save_flags; /* turn of verbose, restore flags */
    dev_chk(dev);
    free(fsbounds);
}


/*
 * Flash hardware subsystem header
 */
#define FLASH_MAJVERSION	1
#define FLASH_MINVERSION	1
#define FLASH_EDITVERSION	1

SUBSYS_HEADER(flash_devices, FLASH_MAJVERSION, FLASH_MINVERSION,
              FLASH_EDITVERSION, flash_init, SUBSYS_CLASS_DRIVER,
              "seq: les_flash_driver, les_flash_file_system",
              NULL);
