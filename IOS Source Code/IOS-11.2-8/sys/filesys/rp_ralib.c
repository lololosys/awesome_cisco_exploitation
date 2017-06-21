/* $Id: rp_ralib.c,v 3.2.60.3 1996/08/28 12:42:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/filesys/rp_ralib.c,v $
 *------------------------------------------------------------------
 * Raw Access Library for the RP PCMCIA socket
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_ralib.c,v $
 * Revision 3.2.60.3  1996/08/28  12:42:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.2  1996/04/22  18:58:21  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:36:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/14  19:48:40  izhu
 * CSCdi37094:  write to flash card fails w/ device read-only msg when it
 * is not
 * Report the correct status for card write protection checking
 *
 * Revision 2.3  1995/06/16  04:26:47  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:04:02  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:33:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>

#if !defined(FSLIB)
#include "../os/clock.h"
#include "logger.h"
#define delay(ms)        process_sleep_for(ms)
#include "../dev/msg_flash.c"		/* Not a typo, see logger.h */
#endif

#include "../../boot/src-68-c7000/pcmap.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "fslib_print.h"
#include "fslib_dev_intel_series_2plus.h"
#include "rp_fc.h"

#define STATIC
#define MAX_YIELD_TIME		0x1000

static ulong total_microseconds;

/*----------------------------------------------------------------------------*
 * DEVICE numbers
 *----------------------------------------------------------------------------*/
#define RALIB_MAX_DEV_NUM 0  /* only devices 0 */

/*----------------------------------------------------------------------------*
 * RALIB vector table to return when initializing.
 *----------------------------------------------------------------------------*/
ralib_vector_table_t ralib_vector_table;

/*----------------------------------------------------------------------------*
 * RALIB DEVICE TABLE
 * device_id -- the text of the device (without the trailing ":")
 * device_type -- PCMCIA or INTERNALFLASH
 * base_addr -- (for INTERNALFLASH) the address of the start of the flash bank
 * status_addr -- the device address to read the status
 * valid -- is this entry in the device table valid (TRUE = VALID)
 * present -- is the flash is inserted into the PCMCIA socket
 * initialized -- indicates whether the device has been initialized (by
 *   calling ralib_init_device()) since the last time it was removed.
 *   (used by all devices, but really only applies to PCMCIA)
 * ready -- indicates if the device is ready.
 *   (used by all devices, but really only applies to PCMCIA)
 *----------------------------------------------------------------------------*/
typedef struct ralib_dev_rec_ {
    char  device_id[32];
    int   device_type;
    ulong base_addr;
    ulong status_addr;
    ushort last_cmd;
    ushort last_data;
    int   valid;
    int   present;
    int   initialized;
    int   ready;
} ralib_dev_rec_t;

ralib_dev_rec_t  ralib_dev_table[RALIB_MAX_DEV_NUM + 1];

#define DEVICE_TYPE_UNKNOWN          0
#define DEVICE_TYPE_PCMCIA           1


/******************************************************************************
 * Name: ralib_init_device_table
 *
 * Description: initializes the device table for the RALIB
 *
 * Input: None
 *
 * Return Value: None
 *
 *******************************************************************************/
void ralib_init_device_table (void)
{
    ralib_dev_rec_t *dr;

    /*
     * PCMCIA slot 0
     */
    dr = &ralib_dev_table[0];
    strcpy(dr->device_id,"slot0");
    dr->device_type = DEVICE_TYPE_PCMCIA;
    dr->base_addr = ADRSPC_EXT_FLASH;
    dr->status_addr = 0;
    dr->valid = TRUE;
    dr->present = FALSE;
    dr->initialized = FALSE;
    dr->ready = FALSE;

    PRT(P16,("ralib_init_device_table() = done\n"));
}

/******************************************************************************
 * Name: ralib_init
 *
 * Description: initializes the raw access library
 *
 * Input: None
 *
 * Return Value:
 *   a pointer to the RALIB vector table for this RALIB.
 *
 *******************************************************************************/
ralib_vector_table_t *ralib_init (void)
{
    ralib_vector_table_t *ralib;

    /*
     * set up the ralib vector table with pointers to the functions
     */
    ralib = &ralib_vector_table;
    ralib->read = ralib_read;
    ralib->write = ralib_write;
    ralib->status = ralib_status;
    ralib->control = ralib_control;
    ralib->yield = ralib_yield;
    ralib->time = ralib_time;
    ralib->device_init = ralib_device_init;
    ralib->device_reset = ralib_device_reset;
    ralib->get_dev_num = ralib_get_dev_num;
    ralib->attrib_read = ralib_attrib_read;
    ralib->attrib_write = ralib_attrib_write;

    /*
     * setup the platform-dependant device stuff
     */
    ralib_init_device_table();

    total_microseconds = 0;              /* accumulated yield time */

    PRT(P16,("ralib_init() = done\n"));

    return(ralib);
}


/******************************************************************************
 * Name: ralib_read
 *
 * Description: reads from a file system (issues read cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   flash_ptr -- offset into the file system device from which to read
 *     (this is the source pointer)
 *   buffer_ptr -- address of the buffer in which to put the data 
 *     (this is the destination pointer)
 *   length -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read, or an error code.  All
 *     error codes are negative.
 *
 *******************************************************************************/
STATIC int ralib_read (int dev_num, ulong flash_ptr, void *buffer_ptr, int length)
{
    int actual;
    ralib_dev_rec_t *dr;
    volatile ushort *src_ptr, *dest_ptr, data;

    PRT(P17E,("ralib_read (%d, 0x%X, 0x%X, %d)",dev_num,flash_ptr,buffer_ptr,length));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
            dr->present = FLASH_CARD_NOT_PRESENT ? FALSE : TRUE;
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
		    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
                        dest_ptr = (ushort *)buffer_ptr;
                        actual = 0;
    /*
     * Handle the first byte copy if it is an unaligned flash address.
     * Read the 16-bit value from the aligned address, and then copy
     * the low byte to the buffer and increment all the pointers.
     */
                        if (flash_ptr & 1) {
                            src_ptr = (volatile ushort *)((flash_ptr& 0xfffffffe)+dr->base_addr);
                            data = *src_ptr;
                            *(uchar *)dest_ptr = (uchar)(data & 0x00FF);
                            (uchar *)dest_ptr += sizeof(uchar);
                            src_ptr++;
                            actual += sizeof(uchar);
                        } else {
                            src_ptr = (volatile ushort *)(flash_ptr+dr->base_addr);
                        }
    /*
     * Main copy loop: copy 16-bit values until there are 0 or 1 bytes left
     * to copy.  The flash pointer is now aligned to a 16-bit boundary.
     */
                        while( length >= (actual+sizeof(ushort)) ) {
                            *dest_ptr++ = *src_ptr++;
                            actual += sizeof(ushort);
                        }
    /*
     * Handle the last byte copy if there is still one byte left.
     * Read the 16 bit value and copy only the MSB to the buffer;
     */
                        if ( length > actual) {
                            data = *src_ptr;
                            *(uchar *)dest_ptr = (uchar)(data >> 8);
                            actual += sizeof(uchar);
                        }
		    }
		}
	    }
	}
    }
    PRT(P17X,(" actual = %d, 0x%X\n",actual,actual));
    return(actual);
}


/******************************************************************************
 * Name: ralib_write
 *
 * Description: writes to a file system (issues write cycles)
 *
 * Input: 
 *   dev_num -- the device number of the file system from which to read
 *   buffer_ptr -- address of the buffer in which to get the data 
 *     (this is the source pointer)
 *   flash_ptr -- offset into the file system device to which to write 
 *     (this is the destination pointer)
 *   length -- the number of bytes to write 
 *
 * Return Value:
 *   actual -- the number of bytes actually written.  If there was an error while
 *     writing the file, an error code is returned.  All error codes are
 *     negative, and all valid values for actual are positive.
 *
 *******************************************************************************/

STATIC int ralib_write (int dev_num, void *buffer_ptr, ulong flash_ptr, int length)
{
    int   actual;
    ralib_dev_rec_t *dr;
    volatile ushort *dest_ptr, *src_ptr;

    /*
     * ralib_write() assume the buffer_ptr are aligned 
     * at word boundary and length is multiple of 2
     * The write function in the file system handles odd byte for us
     */
    PRT(P18E,("ralib_write(%d, 0x%X, 0x%X, len %d, data %X)",dev_num,
                   buffer_ptr,flash_ptr,length, *(ushort *)buffer_ptr));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	actual = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    actual = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
            dr->present = FLASH_CARD_NOT_PRESENT ? FALSE : TRUE;
	    if (dr->present == FALSE) {
		actual = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
		    actual = RALIB_ERROR_DEVICE_NEEDS_INIT;
		} else {
                    if (dr->ready == FALSE) {
			actual = RALIB_ERROR_DEVICE_NOT_READY;
		    } else {
                        actual = 0;
                        src_ptr = (ushort *)buffer_ptr;
                        dest_ptr= (volatile ushort *)(flash_ptr+dr->base_addr);
                        while (length > actual) {
                            dr->status_addr = (ulong)dest_ptr;
			    dr->last_cmd = dr->last_data;
			    dr->last_data = *src_ptr;
                            *dest_ptr++ = *src_ptr++;
                            actual += sizeof(ushort);
                        }
		    }
		}
	    }
	}
    }
    PRT(P18X,("actual = %d\n", actual));
    return(actual);
}


/******************************************************************************
 * Name: ralib_status
 *
 * Description: returns the ready/busy status of the card
 *
 * Input:
 *   dev_num -- the device number of the file system from which to read
 *
 * Return Value:
 *   status -- status of the device.  Possible values are
 *     RALIB_OK
 *     RALIB_BUSY
 *     RALIB_ERROR_INVALID_DEV_NUM
 *     RALIB_ERROR_NO_DEVICE
 *     RALIB_ERROR_DEVICE_NEEDS_INIT
 *
 ******************************************************************************i
 */
STATIC int ralib_status (int dev_num)
{
    int status;
    ralib_dev_rec_t *dr;
    volatile ushort csr;
    ushort csr_mask=0;

    if (dev_num > RALIB_MAX_DEV_NUM) {
        status = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
        dr = &ralib_dev_table[dev_num];
        if (dr->valid == FALSE) {
            status = RALIB_ERROR_INVALID_DEV_NUM;
        } else {
            dr->present = FLASH_CARD_NOT_PRESENT ? FALSE : TRUE;
            if (dr->present == FALSE) {
                status = RALIB_ERROR_NO_DEVICE;
            } else {
                if (dr->initialized == FALSE) {
                    status = RALIB_ERROR_DEVICE_NEEDS_INIT;
                } else {
		    if ((dr->last_cmd & 0x00FF) == IFLASH_CMD_BYTE_WRITE)
			csr_mask |= 0x00FF;
		    if ((dr->last_cmd & 0xFF00) == (IFLASH_CMD_BYTE_WRITE << 8))
			csr_mask |= 0xFF00;
		    if ((dr->last_cmd & 0x00FF) == IFLASH_CMD_BLOCK_ERASE)
			csr_mask |= 0x00FF;
		    if ((dr->last_cmd & 0xFF00) == (IFLASH_CMD_BLOCK_ERASE << 8))
			csr_mask |= 0xFF00;

                    csr = *(volatile ushort *)(dr->status_addr);
                    if ( (csr & (INTEL_2P_FLASH_CARD_READY & csr_mask)) != 
                                (INTEL_2P_FLASH_CARD_READY & csr_mask)) {
                        /* ready bit(s) not set yet */
                        dr->ready = FALSE;
                        status = RALIB_BUSY;
                    } else {
                        dr->ready = TRUE;
                        if ( (csr & csr_mask) == 
                             (INTEL_2P_FLASH_CARD_READY & csr_mask)) {
                            /* only ready bit(s) set, no error bits */
                            status = RALIB_OK;
                        } else {
                            status = csr & csr_mask;
                        }
                    }
                }
            }
        }
    }

    PRT(P16,("ralib_status(%x) = %x, csr = %x\n",dev_num,status,csr));
    return(status);
}


/******************************************************************************
 * Name: ralib_control
 *
 * Description:
 *
 * Input:
 *   dev_num -- the device number of the file system from which to read
 *   mode --
 *
 * Return Value:
 *
 *******************************************************************************/
STATIC int ralib_control (int dev_num, int mode)
{
    int rc=0;

    if (dev_num > RALIB_MAX_DEV_NUM) {
        rc = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
        switch ( mode) {
            case RALIB_RESET_DEVICE:
                 rc = RALIB_OK;
                 break;

#if !defined(FSLIB)
            case RALIB_VPGM_OFF:
                *(volatile ushort *)ADRSPC_CONTROL &= ~CONTROL_FLASH_VPP;
                delay(100);
                rc = RALIB_OK;
                break;

            case RALIB_VPGM_VCC:
                rc = RALIB_OK;
                break;

            case RALIB_VPGM_12V:
                *(volatile ushort *)ADRSPC_CONTROL |= CONTROL_FLASH_VPP;
                delay(100);
                rc = RALIB_OK;
                break;
#endif
 
	    case RALIB_WRITE_PROTECT:
		rc = FLASH_CARD_WRITE_PROT;
		break;

            default:
                rc = RALIB_ERROR_INVALID_MODE;
                break;
        }
    }
    PRT(P16,("ralib_control(%d,%d) = %d\n",dev_num,mode,rc));
    return(rc);
}


/******************************************************************************
 * Name: ralib_yield
 *
 * Description: allows other processes to run (no-op here)
 *
 * Input: 
 *   microseconds -- number of microseconds to yield for 
 *
 * Return Value: None
 *
 *******************************************************************************/
STATIC void ralib_yield (int microseconds)
{
#if !defined(FSLIB) && !defined(RSP_ROM)
    ulong milliseconds;

    /*
     * If non-zero, convert microseconds to milliseconds and round
     * off to the ceiling. Dismiss and let other processes run.
     */
    if (get_interrupt_level() == 0) {
        if (microseconds) {
            total_microseconds += microseconds;
            if (total_microseconds > MAX_YIELD_TIME) {
                milliseconds = ((total_microseconds + 999)/1000);
                total_microseconds = 0; 
                process_sleep_for(milliseconds);
            }
        }
        else
            process_suspend();
    }
#endif

    PRT(P16,("ralib_yield(%d)\n",microseconds));
    return;
}


/******************************************************************************
 * Name: ralib_time
 *
 * Description: returns the time of day
 *
 * Input: None
 *
 * Return Value:
 *   time -- the time of day (in Unix time?)
 *
 *******************************************************************************/
STATIC ulong ralib_time (void)
{
    ulong now;

#if defined(STANDALONE) || defined(FSLIB)
    now = 0;
#else
    now = unix_time();
#endif

    PRT(P16,("ralib_get_time() = %d\n",now));

    return( now);
}


/******************************************************************************
 * Name: ralib_device_init
 *
 * Description: initializes a device
 *
 * Input: 
 *   dev_num -- the device number of the device to initialize
 *
 * Return Value:
 *   return code -- returns the state of the device.
 *
 *******************************************************************************/
int ralib_device_init (int dev_num)
{
    int   rc;
    ralib_dev_rec_t *dr;
    volatile ushort csr;
 
    PRT(P16E,("ralib_device_init(%d)\n",dev_num));

    if (dev_num > RALIB_MAX_DEV_NUM) {
	rc = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
	dr = &ralib_dev_table[dev_num];
	if (dr->valid == FALSE) {
	    rc = RALIB_ERROR_INVALID_DEV_NUM;
	} else {
            dr->present = FLASH_CARD_NOT_PRESENT ? FALSE : TRUE;
	    if (dr->present == FALSE) {
		rc = RALIB_ERROR_NO_DEVICE;
	    } else {
		if (dr->initialized == FALSE) {
                    /* 
                     * Clear the device status register
                     */
                    *((volatile ushort *)dr->base_addr) = 
               (IFLASH_CMD_CLEAR_STATUS_REG << 8) | IFLASH_CMD_CLEAR_STATUS_REG;
                    /* 
                     * reset the device -- make sure it's in read data mode 
                     * this is because the file system assumes the device
                     * is in reset mode the first time it is inserted.
                     */
                    *((volatile ushort *)dr->base_addr) = 
                           (IFLASH_CMD_READ_ARRAY << 8) | IFLASH_CMD_READ_ARRAY;

		    dr->initialized = TRUE;
		    dr->ready = TRUE;
		    rc = RALIB_DEVICE_FIRST_OK;
		} else {
		    if (dr->ready == FALSE) {
                        /* 
                         * Read the status register and update ready flag
                         */
                        *((volatile ushort *)dr->status_addr) = 
                (IFLASH_CMD_READ_STATUS_REG << 8) | IFLASH_CMD_READ_STATUS_REG;
                        csr = *(volatile ushort *)(dr->status_addr);
                        if ( (csr & INTEL_2P_FLASH_CARD_READY) == INTEL_2P_FLASH_CARD_READY) {
                            dr->ready = TRUE;
                        }
                        *((volatile ushort *)dr->status_addr) = 
                           (IFLASH_CMD_READ_ARRAY << 8) | IFLASH_CMD_READ_ARRAY;
		    }
		    rc = RALIB_DEVICE_STILL_OK;
		}
	    }
	}
    }
    PRT(P16X,("ralib_device_init(%d) = %d\n",dev_num,rc));
    return(rc);
}


/***************************************************************************
 * Name: ralib_device_reset
 *
 * Description: resets the device status so device_init returns FSLIB_FIRST_
OK
 *
 * Input:
 *   dev_num -- the device number of the device to reset
 *
 * Return Value:
 *   return code -- returns the state of the device.
 *
 ***************************************************************************
*/
int ralib_device_reset (int dev_num)
{
    int   rc;
    ralib_dev_rec_t *dr;

    PRT(P16E,("ralib_device_reset(%d)\n",dev_num));

    if (dev_num > RALIB_MAX_DEV_NUM) {
        rc = RALIB_ERROR_INVALID_DEV_NUM;
    } else {
        dr = &ralib_dev_table[dev_num];
        if (dr->valid == FALSE) {
            rc = RALIB_ERROR_INVALID_DEV_NUM;
        } else {
            dr->present = FLASH_CARD_NOT_PRESENT ? FALSE : TRUE;
            if (dr->present == FALSE) {
                rc = RALIB_ERROR_NO_DEVICE;
            } else {
                dr->initialized = FALSE;
                rc = RALIB_OK;
            }
        }
    }

    PRT(P16X,("ralib_device_reset(%d) = %d\n",dev_num,rc));

    return(rc);
}


/******************************************************************************
 * Name: ralib_get_dev_num
 *
 * Description: gets a device number, given a device ID
 *
 * Input: 
 *   device_id -- a pointer to the null-terminated character string that is
 *     the device ID
 *
 * Return Value:
 *   dev_num -- the device number for the device, or an error code.  All error
 *     codes are negative, and all valid device numbers are non-negative.
 *
 *******************************************************************************/
int ralib_get_dev_num (const char *device_id)
{
    int i;
    int dev_num;

    PRT(P16E,("ralib_get_dev_num(%s)\n",device_id));

    dev_num = RALIB_ERROR_INVALID_DEVICE_ID;
    for (i = 0; i <= RALIB_MAX_DEV_NUM; i++) {
	if (ralib_dev_table[i].valid == TRUE) {
	    if (strcmp(ralib_dev_table[i].device_id,device_id) == 0) {
		dev_num = i;
		break;
	    } 
	}
    }

    PRT(P16X,("ralib_get_dev_num(%s,%s) = %d\n",device_id,&(ralib_dev_table[0].device_id), dev_num));

    return(dev_num);
}


/***************************************************************************
 * Name: ralib_get_device_names
 *
 * Description: gets a string that contains all the valid device names
 *
 * Input: None
 *
 * Return Value:
 *   device_name_ptr -- a pointer to a character string that contains
 *     the names of the supported device names (i.e the name of the
 *     devices that cause get_dev_num() to return a valid device
 *     number).  The names do not contain the colon (:) and are
 *     concatenated with separating spaces and null terminated.
 *     The device names are in no specific order.
 *     The character string is a const, and must not be modified.
 *
 *     Example of return value.
 *       "slot0 slot1 flash"
 *
 ***************************************************************************
 */
const char *ralib_get_device_names (void)
{
    /*
     * this method could be improved upon, but this is easiest to implement
     * see rsp_ralib.c
     */
    return("slot0");
}


/***************************************************************************
 * Name: ralib_attrib_read -- NOT SUPPORTED on 7000
 *
 * Description: reads from attribute memory for PCMCIA devices 
 ***************************************************************************
 */
int STATIC ralib_attrib_read (int dev_num, ulong flash_ptr, void *buffer_ptr, int length)
{
#if !defined(FSLIB)
    errmsg(&msgsym(ACCESSATTRIBONRP, FLASH),
            "Cannot read flash card attribute memory on 7000");
#endif

    PRT(P17,("ERROR: ralib_attrib_read(%d,%lx,%lx,%d)\n",dev_num,flash_ptr,buffer_ptr,length));
    return( RALIB_ERROR);
}

/***************************************************************************
 * Name: ralib_attrib_write  -- NOT SUPPORTED on 7000
 *
 * Description: writes to attribute memory for PCMCIA devices 
 ***************************************************************************
 */
STATIC int ralib_attrib_write (int dev_num, void *buffer_ptr, ulong flash_ptr, int length)
{
#if !defined(FSLIB)
    errmsg(&msgsym(ACCESSATTRIBONRP, FLASH),
             "Cannot write flash card attribute memory on 7000");
#endif

    PRT(P18,("ERROR: ralib_attrib_write(%d,%lx,%lx,%d)\n",dev_num,buffer_ptr,flash_ptr,length));
    return( RALIB_ERROR);
}

/***************************************************************************
 * stub to satisfy fslib_dev_intel_series_2plus.c
 ***************************************************************************
 */
ushort rsp_disable_io_write_buffer (void)
{
    return( 0);
}

/***************************************************************************
 * stub to satisfy fslib_dev_intel_series_2plus.c
 ***************************************************************************
 */
void rsp_restore_io_write_buffer (ushort dcl_asic_reg)
{
}

