/* $Id: fslib_dev_ram.c,v 3.2.60.1 1996/03/18 19:35:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fslib_dev_ram.c,v $
 *------------------------------------------------------------------
 * Flash file system driver for RAM cards (or any byte read/write device)
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_ram.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(STANDALONE)
#include "master.h"
#endif

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_print.h"
#include "fslib_dev_ram.h"


/******************************************************************************
 * Name: RAM_read_fn
 *
 * Description: read function for RAM devices
 * 
 * Input:
 *   dev_num -- the number of the device from which to read
 *   src -- a pointer into device memory from which to start reading
 *   dst -- a pointer to local memory in which to put the data
 *   length -- the number of bytes to read from the device
 *
 * Return Value:
 *   actual -- the actual number of bytes copied from the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/
int RAM_read_fn (int dev_num, fslib_ptr src, char *dst, ulong length)
{
    int actual;

    actual = ralib_read(dev_num, src, dst, length);

    PRT(P10,("RAM_read_fn(%d,%lx,%lx,%d) = %d\n",dev_num,src,dst,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: RAM_write_fn
 *
 * Description: write function for RAM devices
 * 
 * Input:
 *   dev_num -- the number of the device to which to write
 *   src -- a pointer to local memory buufer that has data to write
 *   dst -- a pointer into device memory at which to start writing
 *   length -- the number of bytes to write to the device
 *
 * Return Value:
 *   actual -- the actual number of bytes written to the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/
int RAM_write_fn (int dev_num, char *src, fslib_ptr dst, ulong length)
{
    int actual;

    /* XXX jack need to guard against odd aligned writes ? */

    actual = ralib_write(dev_num, src, dst, length);

    PRT(P11,("RAM_write_fn(%d,%lx,%lx,%d) = 0\n",dev_num,src,dst,length));

    return(actual);
}


/******************************************************************************
 * Name: RAM_erase_xx_fn
 *
 * Description: erase function for RAM devices that erase to 0x00
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *   sector -- a pointer into device memory at which to erase
 *   length -- the number of bytes to erase (must be sector size)
 *   erase_value -- the value to which to erase the device (0x00 or 0ff)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
static int RAM_erase_xx_fn (int dev_num, fslib_ptr sector, ulong length, uchar erase_value)
{
    int i;
    int rc;
    int write_length;
    int actual;
    static char erase_buffer[64];

    /*
     * set the erase buffer to the erased value
     */
    for (i = 0; i < sizeof(erase_buffer); i++) {
	erase_buffer[i] = erase_value;
    }
    
    /* 
     * Now loop writing the buffer until the whole sector is filled
     * with the erased value.
     */
    rc = FSLIB_OK;
    while ((length) && (rc == FSLIB_OK)) {
	/* 
	 * calculate how much to write
	 */
	if (length > sizeof(erase_buffer)) {
	    write_length = sizeof(erase_buffer);
	} else {
	    write_length = length;
	}
	 
	/* 
	 * write it to the sector
	 */
	actual = ralib_write(dev_num,erase_buffer,sector,write_length);

	/* 
	 * check the result and/or update the counters
	 */
	if (actual != write_length) {
	    rc = FSLIB_ERROR_ERASING_SECTOR;
	} else {
	    length -= actual;
	}
    }


    PRT(P12,("RAM_erase_xx_fn(%d,%lx,%x,%d) = %d\n",dev_num,sector,length,erase_value,rc));

    return(rc);
}


/******************************************************************************
 * Name: RAM_erase_00_fn
 *
 * Description: erase function for RAM devices that erase to 0x00
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *   sector -- a pointer into device memory at which to erase
 *   length -- the number of bytes to erase (must be sector size)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int RAM_erase_00_fn (int dev_num, fslib_ptr sector, ulong length)
{
    int rc;

    rc = RAM_erase_xx_fn(dev_num,sector,length,0x00);
    return(rc);
}


/******************************************************************************
 * Name: RAM_erase_ff_fn
 *
 * Description: erase function for RAM devices that erase to 0xff
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *   sector -- a pointer into device memory at which to erase
 *   length -- the number of bytes to erase (must be sector size)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int RAM_erase_ff_fn (int dev_num, fslib_ptr sector, ulong length)
{
    int rc;

    rc = RAM_erase_xx_fn(dev_num,sector,length,0xff);
    return(rc);
}


/******************************************************************************
 * Name: RAM_init_fn
 *
 * Description: init function for generic RAM cards -- nothing to do
 * 
 * Input:
 *   dev_num -- the number of the device to init
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int RAM_init_fn (int dev_num)
{
    return(FSLIB_OK);
}


/******************************************************************************
 * Name: RAM_show_info_fn
 *
 * Description: show info function for generic RAM cards -- no info
 * 
 * Input:
 *   dev_num -- the number of the device
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int RAM_show_info_fn (int dev_num)
{
    printf("This card is being treated as a generic RAM card\n");
    return(FSLIB_OK);
}

