/* $Id: fslib_dev_rsp_internal_flash.c,v 3.3.54.2 1996/03/21 22:36:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fslib_dev_rsp_internal_flash.c,v $
 *------------------------------------------------------------------
 * Flash file system driver for RSP internal flash bank
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_rsp_internal_flash.c,v $
 * Revision 3.3.54.2  1996/03/21  22:36:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.54.1  1996/03/18  19:35:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.2  1995/12/11  18:45:23  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.1  1995/11/22  20:25:49  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.3  1995/11/24  22:38:25  yanke
 * Do not allow multiple access to bootlfash during write operation.
 *
 * CSCdi42786:  slave device access failures during simultaneous access to
 * slave
 *
 * Revision 3.2  1995/11/17  09:08:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(STANDALONE)
#include "master.h"
#endif

#if defined(RSP)
#include "../src-rsp/rsp_pcmap.h"
#endif /* RSP */

#if defined(PREDATOR)
#include "../src-4k-c7100/c7100_pcmap.h"
#endif /* PREDATOR */


#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_print.h"
#include "fslib_dev_rsp_internal_flash.h"


/******************************************************************************
 * Name: rsp_internal_flash_read_fn
 *
 * Description: read function for RSP Internal flash bank
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
int rsp_internal_flash_read_fn (int dev_num, fslib_ptr src, char *dst,
				ulong length)
{
    int		actual;
    int		cmd_actual;
    ulong	read_cmd;
    ushort	old_dcl_asic_reg;

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * issue the read command to the flash chips (32 bits wide)
     */
    read_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_READ_ARRAY);
    cmd_actual = FSLIB_ralib->write(dev_num,&read_cmd,src,sizeof(read_cmd));
    if (cmd_actual != sizeof(read_cmd)) {
	actual = FSLIB_ERROR;
    } else {
	/*
	 * simply read the bytes from the device.
	 */
	actual = FSLIB_ralib->read(dev_num,src,dst,length);
    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    PRT(P10,("rsp_internal_flash_read_fn(%d,%lx,%lx,%d) = %d\n",
	     dev_num,src,dst,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: turn_vpp_on
 *
 * Description: turns on the programming voltage (Vpp) to the flash bank
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *
 * Return Value:
 *   rc -- FSLIB_OK if Vpp was turned on successfully, otherwise an error code.
 *
 ******************************************************************************/
static int turn_vpp_on (int dev_num)
{
    int		rc;
    ushort	old_dcl_asic_reg;

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * We turn on the Vpp programming voltage using RALIB_control
     */
    rc = FSLIB_ralib->control(dev_num,RALIB_VPGM_12V);
    if (rc != RALIB_OK) {
	rc = convert_ralib_error_code(rc);
    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    ralib_yield(VPP_DELAY);

    return(rc);
}


/******************************************************************************
 * Name: turn_vpp_off
 *
 * Description: turns off the programming voltage (Vpp) to the flash bank
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *
 * Return Value: None
 *
 ******************************************************************************/
static int turn_vpp_off (int dev_num)
{
    int		status;
    ushort	old_dcl_asic_reg;
    
    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * We turn off the Vpp programming voltage using RALIB_control
     */
    status = FSLIB_ralib->control(dev_num,RALIB_VPGM_OFF);
    if (status != RALIB_OK) {
	status = convert_ralib_error_code(status);
    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    ralib_yield(VPP_DELAY);

    return(status);
}


/******************************************************************************
 * Name: wait_for_ready
 *
 * Description: waits for the flash card to be ready
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *   ptr -- the offset into the device that is being programmed
 *   mask -- mask value to indicate which bytes were being changed,
 *     and therefore which bytes must compare.
 *   microsecond_delay -- the number of microseconds the caller is willing
 *     to give up CPU if/when the device is found to be busy,
 *     if set to negative number, caller indicates not willing to
 *     relinquish CPU control.
 *   retry -- maximum number of retries caller is willing to perform.
 *
 * Return Value:
 *   rc -- FSLIB_OK if device is ready, otherwise an error code.
 *
 ******************************************************************************/
static int wait_for_ready (int dev_num, fslib_ptr ptr,
    ulong mask, int microsecond_delay, int retry)
{
    int rc;
    int status_actual;
    ulong status;
    ulong clear_status_cmd;
    ulong ready;
    ulong error;
    ulong erase_error;
    ulong write_error;
    ulong vpp_error;
    int count;
    ushort old_dcl_asic_reg;

    rc = FSLIB_ERROR;

    if (microsecond_delay >= 0)
	ralib_yield(microsecond_delay);
    
    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    count = 0;
    ready = RSPFLASH_CMD_32(RSPFLASH_STATUS_WSMS_READY) & mask;
    vpp_error = RSPFLASH_CMD_32(RSPFLASH_STATUS_VPPS_LOW) & mask;
    erase_error = RSPFLASH_CMD_32(RSPFLASH_STATUS_ES_ERROR) & mask;
    write_error = RSPFLASH_CMD_32(RSPFLASH_STATUS_BWS_ERROR) & mask;
    error = erase_error | write_error | vpp_error;

    do {
	/*
	 * Read the status register (since this is only called after a
	 * write or erase command, we do not need to issue a read
	 * status register command.
	 */
	status_actual = FSLIB_ralib->read(dev_num,ptr,&status,sizeof(status));
	if (status_actual != sizeof(status)) {
	    if (status_actual < 0) {
		rc = convert_ralib_error_code(status_actual);
	    } else {
		rc = FSLIB_ERROR;
	    }
	    break;
	} else {
	    /*
	     * OK, got the status register, now check to see if all
	     * the chips are done.
	     */
	    status &= mask;
	    if ((status & ready) != ready) {
		if (microsecond_delay >= 0)
		    ralib_yield(microsecond_delay);
		count++;
	    } else {
		/*
		 * OK, the operation is done, now check for errors
		 */
		if ((status & error) != 0) {
		    /*
		     * We had an error.  Determine the type and reset
		     * the status register.
		     */
		    if ((status & vpp_error) != 0) {
			rc = FSLIB_ERROR_VPP_LOW;
		    } else if ((status & write_error) != 0) {
			rc = FSLIB_ERROR_WRITING_DEVICE;
		    } else {
			rc = FSLIB_ERROR_ERASING_DEVICE;
		    }
		    /*
		     * Issue the clear status register command
		     * we can do it for all bytes, because it won't
		     * hurt to do it to a bank that did not have an error.
		     */
		    clear_status_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_CLEAR_STATUS_REG);
		    FSLIB_ralib->write(dev_num,&clear_status_cmd,ptr,sizeof(clear_status_cmd));
		} else {
		    /*
		     * else there was no error, so return a good
		     * return code and we are done
		     */
		    rc = FSLIB_OK;
		}
		break;
	    }
	}
    } while (count < retry);

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    return(rc);
}


/******************************************************************************
 * Name: rsp_internal_flash_write_fn
 *
 * Description: write function for Read-Only devices (null function)
 * 
 * This function is broken into 3 parts:
 * 
 *   1. writing the first unaligned word
 *   2. writing zero or more 32-bit aligned words
 *   3. writing the last partial word
 * 
 * Any of these parts are optional -- there are cases where only
 * one or two of them are executed depending on the alignment and 
 * length of the data to write (and, I suppose if someone calls this
 * function with a 0 length, none of these steps will be executed).
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
int rsp_internal_flash_write_fn (int dev_num, char *src, fslib_ptr dst,
				 ulong length)
{
    int		actual;
    int		cmd_actual;
    int		data_actual;
    int		status;
    ulong	dst_low_bits;
    ulong	read_cmd;
    ulong	read_cmd_dst;
    ulong	write_cmd;
    ulong	write_data;
    ulong	first_dst;
    char	*first_src;
    ulong	first_cmd;
    ulong	first_length;
    ulong	first_data;
    ulong	first_mask;
    ulong	last_cmd;
    ulong	last_data;
    ulong	last_mask;
    int		retry_abort;
    int		retry_count;
    int		write_count;
    ushort	old_dcl_asic_reg;

    actual = 0;
    read_cmd_dst = dst;
    read_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_READ_ARRAY);
    write_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_BYTE_WRITE);

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * Turn on the programming voltage
     */
    status = turn_vpp_on(dev_num);
    if (status != FSLIB_OK) {
	actual = status;
    } else {
	/*
	 * Loop to keep retrying if the programming voltage is not up
	 * to level yet.
	 */
	retry_abort = FALSE;
	retry_count = 0;
	do {
	    /*
	     * Check for non-4-byte alignment at the start of the write
	     */
	    if (dst & 3) {
		/*
		 * Handle the first write if it is an unaligned flash address
		 *
		 * the following table shows the command written to the flash bank
		 * (0x00 is the read command, 0x10 is the autoprogram command)
		 *
		 *                      +------------+------------+------------+
		 *                      |             L E N G T H              |
		 *                      +------------+------------+------------+
		 * Flash Addr MOD 4     |     1      |     2      |    >=3     |
		 * (flash_ptr_low_bits) +------------+------------+------------+
		 *                   0  | no action  | no action  | no action  |
		 *                      +------------+------------+------------+
		 *                   1  | 0x00100000 | 0x00101000 | 0x00101010 |
		 *                      +------------+------------+------------+
		 *                   2  | 0x00001000 | 0x00001010 | 0x00001010 |
		 *                      +------------+------------+------------+
		 *                   3  | 0x00000010 | 0x00000010 | 0x00000010 |
		 *                      +------------+------------+------------+
		 * the following table shows the data bytes written to the flash bank
		 * (0x00 is the read command, 0xxx is the data to write)
		 *
		 *                      +------------+------------+------------+
		 *                      |             L E N G T H              |
		 *                      +------------+------------+------------+
		 * Flash Addr MOD 4     |     1      |     2      |    >=3     |
		 * (flash_ptr_low_bits) +------------+------------+------------+
		 *                   0  | no action  | no action  | no action  |
		 *                      +------------+------------+------------+
		 *                   1  | 0x00xx0000 | 0x00xxxx00 | 0x00xxxxxx |
		 *                      +------------+------------+------------+
		 *                   2  | 0x0000xx00 | 0x0000xxxx | 0x0000xxxx |
		 *                      +------------+------------+------------+
		 *                   3  | 0x000000xx | 0x000000xx | 0x000000xx |
		 *                      +------------+------------+------------+
		 * There are several values we will need to do this
		 *
		 *   first_data = the data properly aligned in a long word
		 *     with read commands in the bytes we do not want to write
		 *   write_cmd = a ulong with read commands in the bytes we
		 *     don't want to write, and autoprogram commands in the
		 *     bytes we do want to write
		 *   first_dst = the aligned offset into flash of the ulong
		 *     word that contains the bytes we are trying to write
		 */
		first_dst = dst & 0xfffffffc;
		dst_low_bits = dst & 0x00000003;
		switch (dst_low_bits) {
		case 1:
		    switch (length) {
		    case 1: first_mask = 0x00ff0000; first_length = 1; break;
		    case 2: first_mask = 0x00ffff00; first_length = 2; break;
		    default:first_mask = 0x00ffffff; first_length = 3; break;
		    }
		    break;
		case 2:
		    switch (length) {
		    case 1: first_mask = 0x0000ff00; first_length = 1; break;
		    default:first_mask = 0x0000ffff; first_length = 2; break;
		    }
		    break;
		case 3:
		    first_mask = 0x000000ff;
		    first_length = 1;
		    break;
		default: /* to keep the compiler quiet */
		    first_mask = 0;
		    first_length = 0;
		    break;
		}
		
		first_cmd = (write_cmd & first_mask) | (read_cmd & ~first_mask);
		first_src = src - dst_low_bits;
		first_data = GETLONG((ulong *)first_src);
		first_data = (first_data & first_mask) | (read_cmd & ~first_mask);
		
		/*
		 * Now write the command, write the data, and wait for it to
		 * be done.
		 */
		cmd_actual = FSLIB_ralib->write(dev_num,&first_cmd,first_dst,sizeof(first_cmd));
		if (cmd_actual != sizeof(first_cmd)) {
		    if (cmd_actual >= 0) {
			actual = FSLIB_ERROR;
		    } else {
			actual = convert_ralib_error_code(cmd_actual);
		    }
		} else {
		    data_actual = FSLIB_ralib->write(dev_num,&first_data,first_dst,sizeof(first_data));
		    if (data_actual != sizeof(first_data)) {
			if (data_actual >= 0) {
			    actual = FSLIB_ERROR;
			} else {
			    actual = convert_ralib_error_code(data_actual);
			}
		    } else {
			status = wait_for_ready(dev_num,
			    first_dst, first_mask, -1, 100);
			if (status == FSLIB_ERROR_VPP_LOW) {
			    retry_abort = TRUE;
			    retry_count++;
			} else if (status != FSLIB_OK) {
			    actual = status;
			} else {
			    actual += first_length;
			    src += first_length;
			    dst += first_length;
			    length -= first_length;
			}
		    }
		}
	    }

	    /*
	     * now do as many 32-bit writes to the device as we can:
	     * - issue the write command,
	     * - write the data
	     * - wait for the device to complete the write (become read again)
	     */
	    write_count = 0;
	    while ((length >= sizeof(ulong)) && (actual >= 0) &&
		   (retry_abort == FALSE)) {

		write_count++;
		if (write_count >= FLASH_OP_COUNT) {
		    ralib_yield(0);
		    write_count = 0;
		}

		cmd_actual =
		    FSLIB_ralib->write(dev_num, &write_cmd, dst,
				       sizeof(write_cmd));
		if (cmd_actual != sizeof(write_cmd)) {
		    if (cmd_actual > 0)
			actual = FSLIB_ERROR;
		    else
			actual = cmd_actual;
		} else {
		    write_data = GETLONG((ulong *)src);
		    data_actual = FSLIB_ralib->write(dev_num,&write_data,dst,sizeof(ulong));
		    if (data_actual != sizeof(ulong)) {
			if (data_actual > 0)
			    actual = FSLIB_ERROR;
			else
			    actual = data_actual;
		    } else {
			status = wait_for_ready(dev_num,dst,0xffffffff,-1,100);
			if (status == FSLIB_ERROR_VPP_LOW) {
			    retry_abort = TRUE;
			    retry_count++;
			} else if (status != FSLIB_OK) {
			    actual = status;
			} else {
			    src += sizeof(ulong);
			    dst += sizeof(ulong);
			    actual += sizeof(ulong);
			    length -= sizeof(ulong);
			}
		    }
		}
	    }

	    /*
	     * Check if there are some more bytes left to write
	     */
	    if ((length != 0) && (actual >= 0) && (retry_abort == FALSE)) {
		/*
		 * Make a command that has a write in the upper byte and read
		 * in the lower byte.
		 */
		switch(length) {
		case 1: last_mask = 0xff000000; break;
		case 2: last_mask = 0xffff0000; break;
		case 3: last_mask = 0xffffff00; break;
		default: last_mask = 0; break;
		}
		last_cmd = ((write_cmd & last_mask) | (read_cmd & ~last_mask));
		last_data = GETLONG((ulong *)src);
		last_data = ((last_data & last_mask) | (read_cmd & ~last_mask));
		/*
		 * now write the cmd, write the data, and wait for ready
		 */
		cmd_actual = FSLIB_ralib->write(dev_num,&last_cmd,dst,sizeof(last_cmd));
		if (cmd_actual != sizeof(last_cmd)) {
		    if (cmd_actual > 0)
			actual = FSLIB_ERROR;
		    else
			actual = convert_ralib_error_code(cmd_actual);
		} else {
		    /*
		     * Make the data word that has the data in the upper
		     * byte and a read command in the lower byte.
		     */
		    data_actual = FSLIB_ralib->write(dev_num,&last_data,dst,sizeof(last_data));
		    if (data_actual != sizeof(last_data)) {
			if (cmd_actual > 0)
			    actual = FSLIB_ERROR;
			else
			    actual = convert_ralib_error_code(data_actual);
		    } else {
			/*
			 * Wait for the device to become ready
			 */
			status = wait_for_ready(dev_num,dst,last_mask,-1,100);
			if (status == FSLIB_ERROR_VPP_LOW) {
			    retry_abort = TRUE;
			    retry_count++;
			} else if (status != FSLIB_OK) {
			    actual = status;
			} else {
			    src += length;
			    dst += length;
			    actual += length;
			    length -= length;
			}
		    }
		}
	    }
	} while ((retry_abort == TRUE) && (retry_count < 10000));
	
	/*
	 * Turn off the programming voltage
	 */
	turn_vpp_off(dev_num);
    }

    /*
     * issue the read command to the flash chips to leave them in
     * read mode (32 bits wide)
     */
    cmd_actual = FSLIB_ralib->write(dev_num,&read_cmd,read_cmd_dst,sizeof(read_cmd));    

    PRT(P11,("rsp_internal_flash_write_fn(%d,%lx,%lx,%d) = %d\n",dev_num,src,dst,length,actual));

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    return(actual);
}


/******************************************************************************
 * Name: rsp_internal_flash_erase_fn
 *
 * Description: erase function for Read-Only devices (null function)
 * 
 * Erasing requires 2 commands to be written in order
 *
 *   1. CHIP ERASE
 *   2. AUTO BLOCK ERASE
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
int rsp_internal_flash_erase_fn (int dev_num, fslib_ptr sector, ulong length)
{
    int		cmd_actual;
    ulong	write_cmd;
    ulong	read_cmd;
    int		rc;
    int		retry_count;
    int		retry;
    ushort	old_dcl_asic_reg;
    
    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * Turn on the programming voltage
     */
    rc = turn_vpp_on(dev_num);
    if (rc == FSLIB_OK) {
	/*
	 * Loop a bunch of times if we get the "Vpp Low" error
	 * response from wait_for_ready().
	 */
	retry = TRUE;
	retry_count = 0;
	do {
	    /*
	     * Make a command that has CHIP ERASE in all 4 bytes, and write it
	     * to the flash at the sector address specified.
	     */
	    write_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_BLOCK_ERASE);
	    cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,sector,sizeof(write_cmd));
	    if (cmd_actual != sizeof(write_cmd)) {
		if (cmd_actual < 0) {
		    rc = convert_ralib_error_code(cmd_actual);
		}
	    } else {
		/*
		 * Make a command that has AUTO BLOCK ERASE in all 4 bytes,
		 * and write it to the flash at the sector address specified.
		 */
		write_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_ERASE_CONFIRM);
		cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,sector,sizeof(write_cmd));
		if (cmd_actual != sizeof(write_cmd)) {
		    if (cmd_actual < 0) {
			rc = convert_ralib_error_code(cmd_actual);
		    }
		} else {
		    /*
		     * wait for the erase to complete and the card to be
		     * ready.  We wait for the first byte in the sector for
		     * all bytes lanes (i.e. 32-bits wide) to be equal to
		     * 0xffffffff since that is the erased state of these
		     * flash chips.
		     */
		    rc = wait_for_ready(dev_num,sector,0xffffffff,1000,1000000);
		    if (rc == FSLIB_ERROR_VPP_LOW) {
			retry_count++;
		    } else {
			retry = FALSE;
		    }
		}
	    }
	} while ((retry) && (retry_count < 10000));

	/*
	 * turn off the programming voltage
	 */
	turn_vpp_off(dev_num);
    }
    /*
     * issue the read command to the flash chips to leave them in
     * read mode (32 bits wide)
     */
    read_cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_READ_ARRAY);
    cmd_actual = FSLIB_ralib->write(dev_num,&read_cmd,sector,sizeof(read_cmd));

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    PRT(P12,("rsp_internal_flash_erase_fn(%d,%lx,%d) = %d\n",dev_num,sector,length,rc));

    return(rc);
}


/******************************************************************************
 * Name: rsp_internal_flash_init_fn
 *
 * Description: init function for internal flash bank on RSP
 * 
 * Input:
 *   dev_num -- the number of the device to init
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int rsp_internal_flash_init_fn (int dev_num)
{
    return(FSLIB_OK);
}
