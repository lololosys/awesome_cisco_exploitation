/* $Id: fslib_dev_intel_series_2plus.c,v 3.2.60.2 1996/04/03 15:43:50 wfried Exp $
 * $Source: /release/112/cvs/Xsys/filesys/fslib_dev_intel_series_2plus.c,v $
 *------------------------------------------------------------------
 * Flash file system device driver for Intel Series 2+ flash cards
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_dev_intel_series_2plus.c,v $
 * Revision 3.2.60.2  1996/04/03  15:43:50  wfried
 * CSCdi52833:  PCMCIA Flash File System not working
 * Branch: California_branch
 * Fix Predator PCMCIA flash file system support. Major issues are byte
 * swapping, and PCMCIA controller timing.
 *
 * Revision 3.2.60.1  1996/03/18  19:35:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/27  21:52:52  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.3  1995/07/07  22:57:08  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.2  1995/06/17  00:16:47  yanke
 * perform a reset after each flash chip access to avoid subsequent
 * flash wirte/read failure.
 *
 * CSCdi35872:  sh fl chips corrupts card, then crashes router.
 *
 * Revision 2.1  1995/06/07  20:32:28  hampton
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
#include "fslib_dev_intel_series_2plus.h"

/* XXX jack  change this back to static after debug */
#define STATIC


/******************************************************************************
 * Name: intel_series_2plus_read_fn
 *
 * Description: read function for Intel Series 2+ flash cards.
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
int intel_series_2plus_read_fn (int dev_num, fslib_ptr src, char *dst,
				ulong length)
{
    int		actual;
    int		cmd_actual;
    ushort	read_cmd;
    ushort	old_dcl_asic_reg;

    PRT(P10E,("intel_series_2plus_read_fn(%d,%lx,%lx,%d)\n",dev_num,src,dst,length));

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    read_cmd = IFLASH_CMD_READ_ARRAY | (IFLASH_CMD_READ_ARRAY << 8);

    /*
     * issue the read command to the flash chips (16 bits wide)
     */
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

    PRT(P10X,("intel_series_2plus_read_fn(%d,%lx,%lx,%d) = %d\n",dev_num,src,dst,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: wait_for_ready
 *
 * Description: waits for the flash card to be ready
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *   offset -- the offset into the device where the last operation was
 *     done that we are waiting to complete
 *   mask -- which bytes to check for ready (0xff00, 0xffff, 0x00ff)
 *   microsecond_delay -- the number of microseconds to delay between
 *     checks of the device status.
 *
 * Return Value:
 *   status -- FSLIB_OK if device is ready, otherwise an error code.
 *
 ******************************************************************************/
STATIC int wait_for_ready (int dev_num, fslib_ptr offset, ushort mask,
			   int microsecond_delay)
{
    int status;
    int count;

    /*
     * Small delay to ensure that quick operations won't put
     * us to sleep. This is not the case for the erase operation
     * for instance.
     */
    usecdelay(RDY_DELAY);

    for (count = 0; count < 1000000L; count++) {
	status = FSLIB_ralib->status(dev_num);
	if (status != RALIB_BUSY)
	    break;
	ralib_yield(microsecond_delay);
    }
    if (status == RALIB_OK) {
	status = FSLIB_OK;
    } else {
	if (status == RALIB_BUSY) {
	    status = FSLIB_ERROR;
	} else {
	    status = convert_ralib_error_code(dev_num);
	}
    }

    return(status);
}


/******************************************************************************
 * Name: turn_vpp_on
 *
 * Description: turns on the programming voltage (Vpp) in the card
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *
 * Return Value:
 *   status -- FSLIB_OK if Vpp was turned on successfully, otherwise an error code.
 *
 ******************************************************************************/
STATIC int turn_vpp_on (int dev_num)
{
    uchar	voltage_cmd;
    uchar	voltage_status;
    int		voltage_actual;
    int		count;
    int		status;
    ushort	old_dcl_asic_reg;

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * We turn on the VPP programming voltage using RALIB_control.
     * If that is successful, we don't have to do anything more.
     * The C7000 works this way.
     */
    status = FSLIB_ralib->control(dev_num,RALIB_VPGM_12V);
    if (status == RALIB_OK) {
	status = FSLIB_OK;
    } else {
	
	/*
	 * We couldn't turn on the Vpp for this device using the
         * RALIB, so we need to use the internal Vpp supply.
	 * The RSP works this way.
	 * Turn on the programming voltage and wait for it to get up to
	 * the required voltage.
	 */
	voltage_cmd = IFLASH_VOLTCTRL_VPP_GEN;
	voltage_actual = FSLIB_ralib->attrib_write(dev_num,
						   &voltage_cmd,
						   IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
						   sizeof(voltage_cmd));
	if (voltage_actual != sizeof(voltage_cmd)) {
	    PRT(PERR,("ERROR turning on internal Vpp.  voltage_actual=%d\n",voltage_actual));
	    status = FSLIB_ERROR;
	} else {

	    count = 0;
	    status = FSLIB_ERROR_DEVICE_IS_READ_ONLY;
	    for (count = 0; count < 100; count ++) {

		/*
		 * Restore the previous IO write buffer state
		 * before suspending
		 */
		rsp_restore_io_write_buffer(old_dcl_asic_reg);

		/* 
		 * Let other processes run while we wait for the
		 * voltage to be OK (it has to get up to 12V)
		 */
		ralib_yield(VPP_DELAY);

		/*
		 * Disable write buffer again
		 */
		old_dcl_asic_reg = rsp_disable_io_write_buffer();

		voltage_actual =
		    FSLIB_ralib->attrib_read(dev_num,
					     IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
					     &voltage_status,
					     sizeof(voltage_status));
		if (voltage_actual != sizeof(voltage_cmd)) {
		    PRT(PERR,("ERROR reading internal voltage status.  voltage_actual=%d\n",voltage_actual));
		    status = convert_ralib_error_code(voltage_actual);
		    break;
		} else {
		    if (voltage_status & IFLASH_VOLTCTRL_VPP_VALID) {
			status = FSLIB_OK;
			break;
		    }
		}
	    }
	}
    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    return(status);
}


/******************************************************************************
 * Name: turn_vpp_off
 *
 * Description: turns off the programming voltage (Vpp) in the card
 * 
 * Input:
 *   dev_num -- the number of the device to check/wait
 *
 * Return Value: 
 *   return code -- FSLIB_OK = success, otherwise error code
 *
 ******************************************************************************/
STATIC int turn_vpp_off (int dev_num)
{
    uchar	voltage_cmd;
    int		voltage_actual;
    uchar	voltage_status;
    int		count;
    int		status;
    ushort	old_dcl_asic_reg;

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * We turn off the Vpp programming voltage using RALIB_control.
     * If this is successful, we are done.
     * The C7000 works this way.
     * If not we try to turn off Vpp using the on-card function.
     */
    status = FSLIB_ralib->control(dev_num,RALIB_VPGM_OFF);
    if (status == RALIB_OK) {
	status = FSLIB_OK;
    } else {

	/*
	 * If we didn't turn off the Vpp using the control, we assume
	 * that Vpp is done using the on-card Vpp source for this
	 * platform, so we turn off the Vpp on the card.
	 * The RSP works this way.
	 * Turn off the programming voltage and wait for it to get up to
	 * the required voltage.
	 */
	voltage_cmd = 0;
	voltage_actual = FSLIB_ralib->attrib_write(dev_num,
						   &voltage_cmd,
						   IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
						   sizeof(voltage_cmd));
	if (voltage_actual != sizeof(voltage_cmd)) {
	    PRT(PERR,("ERROR turning off internal voltage.  voltage_actual=%d\n",voltage_actual));
	    status = convert_ralib_error_code(voltage_actual);
	} else {
	    
	    /* 
	     * wait for the voltage drop (it has to go below 11.4  volts)
	     */
	    
	    count = 0;
	    status = FSLIB_ERROR;
	    for (count = 0; count < 1000000L; count ++) {
		
		/*
		 * Restore the previous IO write buffer state before suspending
		 */
		rsp_restore_io_write_buffer(old_dcl_asic_reg);
		
		/* 
		 * Let other processes run while we wait
		 */
		ralib_yield(VPP_DELAY);
		
		/*
		 * Disable write buffer again
		 */
		old_dcl_asic_reg = rsp_disable_io_write_buffer();
		
		voltage_actual =
		    FSLIB_ralib->attrib_read(dev_num,
					     IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
					     &voltage_status,
					     sizeof(voltage_status));
		if (voltage_actual != sizeof(voltage_cmd)) {
		    PRT(PERR,("ERROR reading voltage status (off).  voltage_actual=%d\n",voltage_actual));
		    status = convert_ralib_error_code(voltage_actual);
		    break;
		} else {
		    if ((voltage_status & IFLASH_VOLTCTRL_VPP_VALID) == 0) {
			status = FSLIB_OK;
			break;
		    }
		}
	    }
	}
    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    return(status);
}


/******************************************************************************
 * Name: get_device_operation_status
 *
 * Description: gets the device operation status from the general status register
 * 
 * Input:
 *   dev_num -- the number of the device
 *   offset -- the offset into the flash card at which to get the device
 *     operation status.  There are different status registers for each flash
 *     chip in the flash card.
 *   mask -- the mask of the bytes that we are supposed to check the
 *     device operation status for (either 0x00ff, 0xffff, or 0xff00)
 *
 * Return Value: 
 *   return code -- FSLIB_OK = device operation OK, FSLIB_ERROR = device operation ERROR
 *
 ******************************************************************************/
STATIC int get_device_operation_status (int dev_num, fslib_ptr offset,
					ushort mask)
{
    int rc;
    ushort status;
    ulong bank_base;
    ulong status_offset;
    int status_actual;

    bank_base = offset - (offset % IFLASH_BANK_SIZE);
    status_offset = bank_base + IFLASH_GLOBAL_STATUS_OFFSET;
    status_actual = FSLIB_ralib->read(dev_num,status_offset,&status,sizeof(status));
    if (status_actual != sizeof(status)) {
	PRT(PERR,("ERROR reading dev op status.  status_actual=%d\n",status_actual));
	printf("\nRead status error:");
	printf("\n	device: %d", dev_num);
	printf("\n	status offset: %x", status_offset);
	printf("\n	bytes read: %d", status_actual);
	printf("\n	status read: %x", status);
	rc = FSLIB_ERROR;
    } else {
	if (status 
	    & (IFLASH_GLOBAL_STATUS_DOS | (IFLASH_GLOBAL_STATUS_DOS << 8)) 
	    & mask) {
	    PRT(PERR,("ERROR DOS bit set in GSR.  status=%x\n",status));
	    printf("\nRead error status:");
	    printf("\n	device: %d", dev_num);
	    printf("\n	status offset: %x", status_offset);
	    printf("\n	bytes read: %d", status_actual);
	    printf("\n	status read: %x", status);
	    /*
	     *	Perform a hardware reset to the pcmcia
	     *	card in an attempt to recover from the problem.
	     */
	    FSLIB_ralib->control(dev_num, RALIB_RESET_DEVICE);
	    rc = FSLIB_ERROR;
	} else {
	    rc = FSLIB_OK;
	}
    }
    PRT(P11,("get_device_operation_status(%d,%lx) = %d(%x)\n",dev_num,offset,rc,status));

    return(rc);
}


/******************************************************************************
 * Name: reset_device_operation_status
 *
 * Description: issues a reset status command
 * 
 * Input:
 *   dev_num -- the number of the device
 *   offset -- the offset into the flash card at which to reset the device
 *     operation status.  There are different status registers for each flash
 *     chip in the flash card.
 *
 * Return Value: None
 *
 ******************************************************************************/
STATIC void reset_device_operation_status (int dev_num, int addr)
{
    ushort cmd;
    
    cmd = IFLASH_CMD_CLEAR_STATUS_REG | (IFLASH_CMD_CLEAR_STATUS_REG << 8);
    FSLIB_ralib->write(dev_num,&cmd,addr,sizeof(cmd));

}


/******************************************************************************
 * Name: check_device_operation_status
 *
 * Description: gets the dev op status and reset it if there was an error
 * 
 * Input:
 *   dev_num -- the number of the device
 *   offset -- the offset into the flash card at which to get the device
 *     operation status.  There are different status registers for each flash
 *     chip in the flash card.
 *   mask -- the mask of the bytes that we are supposed to check the
 *     device operation status for (either 0x00ff, 0xffff, or 0xff00)
 *
 * Return Value: 
 *   return code -- FSLIB_OK = device operation OK, FSLIB_ERROR = device operation ERROR
 *
 ******************************************************************************/
STATIC int check_device_operation_status (int dev_num, fslib_ptr offset,
					  ushort mask)
{
    int rc;
    
    rc = get_device_operation_status(dev_num,offset,mask);
    if (rc == FSLIB_ERROR) {
	reset_device_operation_status(dev_num,offset);
    }
    return(rc);
}


/******************************************************************************
 * Name: intel_series_2plus_write_fn
 *
 * Description: write function for Intel Series 2+ flash cards.
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
int intel_series_2plus_write_fn (int dev_num, char *src, fslib_ptr dst,
				 ulong length)
{
    int		actual;
    int		cmd_actual;
    int		data_actual;
    int		status;
    ushort	write_cmd;
    ushort	read_cmd;
    ushort	write_data;
    ushort	dev_op_mask;
    fslib_ptr	first_dst;
    fslib_ptr	initial_dst;
    int		write_count;
    ushort	old_dcl_asic_reg;

    PRT(P11E,("intel_series_2plus_write_fn(%d,%lx,%lx,%d)\n",dev_num,src,dst,length));
    
    actual = 0;

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * Turn on the programming voltage and wait for it to get up to
     * the required voltage.
     */
    status = turn_vpp_on(dev_num);
    dev_op_mask = 0;
    if (status == FSLIB_OK) {
	initial_dst = dst;
	/*
	 * Check for odd byte alignment at the start of the write (dst is odd)
	 */
	if (dst & 1) {
	    dev_op_mask |= 0x00ff;
	    /*
	     * Make a command that has a read in the upper byte and write
	     * in the lower byte.
	     */
	    first_dst = dst - 1;
	    write_cmd =  (IFLASH_CMD_READ_EXT_STATUS_REG << 8) | IFLASH_CMD_BYTE_WRITE;
	    cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,first_dst,sizeof(write_cmd));
	    if (cmd_actual != sizeof(write_cmd)) {
		PRT(PERR,("ERROR writing command (first).  cmd_actual=%d\n",cmd_actual));
		if (cmd_actual >= 0)
		    actual = FSLIB_ERROR;
		else
		    actual = convert_ralib_error_code(cmd_actual);
	    } else {
		/*
		 * Make the data word that has a read command in the upper
		 * byte and the data for the first byte in the lower byte.
		 */
		write_data = (IFLASH_CMD_READ_EXT_STATUS_REG << 8) | *src;
		cmd_actual = FSLIB_ralib->write(dev_num,&write_data,first_dst,sizeof(write_data));
		if (cmd_actual != sizeof(write_data)) {
		    PRT(PERR,("ERROR writing data (first).  cmd_actual=%d\n",cmd_actual));
		    if (cmd_actual >= 0)
			actual = FSLIB_ERROR;
		    else
			actual = convert_ralib_error_code(cmd_actual);
		} else {
		    /*
		     * Wait for the device to become ready
		     */
		    status = wait_for_ready(dev_num,first_dst,0x00ff,1);
		    if (status != FSLIB_OK) {
			PRT(PERR,("ERROR waiting for ready (first).  status=%d\n",status));
			actual = status;
		    } else {
			src++;
			dst++;
			actual++;
			length--;
		    }
		}
	    }
	}

	/*
	 * now do as many 16-bit writes to the device as we can:
	 * - issue the write command,
	 * - write the data
	 * - wait for the device to complete the write (become read again)
	 */
	write_count = 0;
	write_cmd = (IFLASH_CMD_BYTE_WRITE << 8) | IFLASH_CMD_BYTE_WRITE;
	while ((length >= 2) && (actual >= 0)) {

	    write_count++;
	    if (write_count >= FLASH_OP_COUNT) {
		ralib_yield(0);
		write_count = 0;
	    }

	    dev_op_mask |= 0xffff;
	    cmd_actual =
		FSLIB_ralib->write(dev_num,&write_cmd,dst,sizeof(write_cmd));
	    if (cmd_actual != sizeof(write_cmd)) {
		PRT(PERR,("ERROR writing command (loop).  cmd_actual=%d\n",cmd_actual));
		if (cmd_actual >= 0)
		    actual = FSLIB_ERROR;
		else
		    actual = cmd_actual;
	    } else {
		data_actual = FSLIB_ralib->write(dev_num,src,dst,sizeof(ushort));
		if (data_actual != sizeof(ushort)) {
		    PRT(PERR,("ERROR writing data (loop).  cmd_actual=%d\n",cmd_actual));
		    if (data_actual >= 0)
			actual = FSLIB_ERROR;
		    else
			actual = data_actual;
		} else {
		    status = wait_for_ready(dev_num,dst,0xffff,1);
		    if (status != FSLIB_OK) {
			PRT(PERR,("ERROR waiting for ready (first).  status=%d\n",status));
			actual = status;
		    } else {
			src += sizeof(ushort);
			dst += sizeof(ushort);
			actual += sizeof(ushort);
			length -= sizeof(ushort);
		    }
		}
	    }
	}

	/*
	 * Check if there is one more odd byte left to write
	 */
	if ((length == 1) && (actual >= 0)) {
	    dev_op_mask |= 0xff00;
	    /*
	     * Make a command that has a write in the upper byte and read
	     * in the lower byte.
	     */
	    write_cmd = (IFLASH_CMD_BYTE_WRITE << 8) | IFLASH_CMD_READ_EXT_STATUS_REG;
	    cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,dst,sizeof(write_cmd));
	    if (cmd_actual != sizeof(write_cmd)) {
		PRT(PERR,("ERROR writing command (last).  cmd_actual=%d\n",cmd_actual));
		if (cmd_actual >= 0)
		    actual = FSLIB_ERROR;
		else
		    actual = convert_ralib_error_code(cmd_actual);
	    } else {
		/*
		 * Make the data word that has the data in the upper
		 * byte and a read command in the lower byte.
		 */
		write_data = (*src << 8) | IFLASH_CMD_READ_EXT_STATUS_REG;
		cmd_actual = FSLIB_ralib->write(dev_num,&write_data,dst,sizeof(write_data));
		if (cmd_actual != sizeof(write_data)) {
		    PRT(PERR,("ERROR writing data (last).  cmd_actual=%d\n",cmd_actual));
		    if (cmd_actual >= 0)
			actual = FSLIB_ERROR;
		    else
			actual = convert_ralib_error_code(cmd_actual);
		} else {
		    /*
		     * Wait for the device to become ready
		     */
		    status = wait_for_ready(dev_num,dst,0xff00,1);
		    if (status != FSLIB_OK) {
			PRT(PERR,("ERROR waiting for ready (last).  status=%d\n",status));
			actual = status;
		    } else {
			src++;
			dst++;
			actual++;
			length--;
		    }
		}
	    }
	}

	/* 
 	 * check the status of the write state machine
	 */
	status = check_device_operation_status(dev_num,initial_dst,dev_op_mask);
	if (status != FSLIB_OK) {
	    PRT(PERR,("ERROR in dev op status.  status=%d\n",status));
	    actual = FSLIB_ERROR_WRITING_DEVICE;
	}
	
	/* 
 	 * turn off the programming voltage
	 */
	turn_vpp_off(dev_num);

	/*
	 * issue the read command to the flash chips to leave them in
	 * read mode (16 bits wide)
	 */
	read_cmd = IFLASH_CMD_READ_ARRAY | (IFLASH_CMD_READ_ARRAY << 8);
	cmd_actual = FSLIB_ralib->write(dev_num,&read_cmd,initial_dst,sizeof(read_cmd));

    } else
	actual = status;

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    PRT(P11X,("intel_series_2plus_write_fn(%d,%lx,%lx,%d) = %d\n",dev_num,src,dst,length,actual));

    return(actual);
}


/******************************************************************************
 * Name: intel_series_2plus_erase_fn
 *
 * Description: erase function for Intel Series 2+ flash cards.
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
int intel_series_2plus_erase_fn (int dev_num, fslib_ptr sector, ulong length)
{
    int		cmd_actual;
    ushort	write_cmd;
    ushort	read_cmd;
    int		rc;
    int		status;
    ushort	old_dcl_asic_reg;

    PRT(P12E,("intel_series_2plus_erase_fn(%d,%lx,%d)\n",dev_num,sector,length));

    /*
     * Disable the IO write buffer:
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    /*
     * turn on the programming voltage
     */
    rc = turn_vpp_on(dev_num);
    if (rc == FSLIB_OK) {
	rc = FSLIB_ERROR;
	/*
	 * Make a command that has a write in the upper byte and read
	 * in the lower byte.
	 */
	write_cmd = (IFLASH_CMD_BLOCK_ERASE << 8) | IFLASH_CMD_BLOCK_ERASE;
	cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,sector,sizeof(write_cmd));
	if (cmd_actual != sizeof(write_cmd)) {
	    if (cmd_actual >= 0) {
		rc = FSLIB_ERROR;
	    } else {
		rc = convert_ralib_error_code(cmd_actual);
	    }
	} else {
	    write_cmd = (IFLASH_CMD_ERASE_CONFIRM << 8) | IFLASH_CMD_ERASE_CONFIRM;
	    cmd_actual = FSLIB_ralib->write(dev_num,&write_cmd,sector,sizeof(write_cmd));
	    if (cmd_actual != sizeof(write_cmd)) {
		if (cmd_actual >= 0) {
		    rc = FSLIB_ERROR;
		} else {
		    rc = convert_ralib_error_code(cmd_actual);
		}
	    } else {
		/*
		 * wait for the erase to complete and the card to be ready.
		 */
		rc = wait_for_ready(dev_num,sector,0xffff,8000);
	    }
	}

	/* 
 	 * check the status of the erase state machine
	 */
	status = check_device_operation_status(dev_num,sector,0xffff);
	if (status != FSLIB_OK) {
	    rc = FSLIB_ERROR_ERASING_DEVICE;
	}
	
	/*
	 * make sure we turn off the programming voltage
	 */
	turn_vpp_off(dev_num);

	/*
	 * issue the read command to the flash chips to leave them in
	 * read mode (16 bits wide)
	 */
	read_cmd = IFLASH_CMD_READ_ARRAY | (IFLASH_CMD_READ_ARRAY << 8);
	cmd_actual = FSLIB_ralib->write(dev_num,&read_cmd,sector,sizeof(read_cmd));

    }

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    PRT(P12X,("intel_series_2plus_erase_fn(%d,%lx,%d) = %d\n",dev_num,sector,length,rc));

    return(rc);
}


/******************************************************************************
 * Name: intel_series_2plus_init_fn
 *
 * Description: init function for Intel Series 2+ flash cards.
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int intel_series_2plus_init_fn (int dev_num)
{
    int rc;
    uchar unlock_cmd;
    
    PRT(P12E,("intel_series_2plus_init_fn(%d)\n",dev_num));
    
    rc = 0;
    /*
     * Read write protect switch
     * Write a 0 to BLKEN to unlock any locked blocks
     * Upload block lock status information (9797 d0d0)
     */
    unlock_cmd = 0;
    ralib_attrib_write(dev_num, &unlock_cmd,
	IFLASH_WRITE_PROTECT_ATTRIB_OFFSET, sizeof(unlock_cmd));

    return(rc);
}


#if !defined(C7000)
/******************************************************************************
 * Name: print_attrib_reg
 *
 * Description: Prints an attribute memory register value
 * 
 * Input:
 *   description -- pointer to the character string describing the register
 *   attrib_offset -- offset into attrib memory space of the register
 *
 * Return Value: None
 *
 ******************************************************************************/
static void print_attrib_reg(int dev_num, char *description, ulong attrib_offset)
{
    uchar reg8;
    int actual;

    printf("%s (%x): ",description,attrib_offset);
    actual = FSLIB_ralib->attrib_read(dev_num,attrib_offset,&reg8,sizeof(reg8));
    if (actual != sizeof(reg8)) {
	printf("ERROR READING\n");
    } else {
	printf("%x\n",reg8);
    }

}
#endif

/******************************************************************************
 * Name: intel_series_2plus_show_info_fn
 *
 * Description: dumps the registers for the Intel Series 2+ flash cards.
 * 
 * Input:
 *   dev_num -- the number of the device to erase (partial erase)
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ******************************************************************************/
int intel_series_2plus_show_info_fn (int dev_num)
{
    int rc;
    ushort cmd;
    int cmd_actual;
    ushort status;
    int status_actual;
    ulong status_offset;
    int id_actual;
    ulong id_codes;
    int sector;
    int num_sectors;
    int bank;
    ulong bank_base;
    int num_banks;
    ushort old_dcl_asic_reg;

    /*
     * disable the write buffers so they don't confuse the I/O devices
     */
    old_dcl_asic_reg = rsp_disable_io_write_buffer();

    rc = FSLIB_OK;
    /*
     * Initialize the device (may not be necessary, but won't hurt)
     */
    FSLIB_ralib->device_init(dev_num);

#if !defined(C7000)

    /*
     * Attribute Memory registers
     */
    printf("\n******** Intel Series 2+ Status/Register Dump ********\n");
    printf("\nATTRIBUTE MEMORY REGISTERS:\n");

    print_attrib_reg(dev_num,"  Config Option Reg",IFLASH_CONFIG_OPTION_ATTRIB_OFFSET);
    print_attrib_reg(dev_num,"  Config Status Reg",IFLASH_CONFIG_STATUS_ATTRIB_OFFSET);
    print_attrib_reg(dev_num,"  Card Status   Reg",IFLASH_CARD_STATUS_ATTRIB_OFFSET);
    print_attrib_reg(dev_num,"  Write Protect Reg",IFLASH_WRITE_PROTECT_ATTRIB_OFFSET);
    print_attrib_reg(dev_num,"  Voltage Cntrl Reg",IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET);
    print_attrib_reg(dev_num,"  Rdy/Busy Mode Reg",IFLASH_READY_BUSY_ATTRIB_OFFSET);
#endif

    num_banks = 5;

    for (bank = 0; bank < num_banks; bank++) {
	bank_base = IFLASH_BANK_SIZE * bank;
	/*
	 * Common memory status registers
	 */
	printf("\nCOMMON MEMORY REGISTERS: Bank %d\n",bank);
	
	/*
	 * read the intelligent identifier codes
	 */
	printf("  Intelligent ID Code  : ");
	cmd = IFLASH_CMD_INTELLIGENT_ID | (IFLASH_CMD_INTELLIGENT_ID << 8);
	cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	if (cmd_actual != sizeof(cmd)) {
	    printf("ERROR WRITING ID COMMAND\n");
	    rc = FSLIB_ERROR;
	} else {
	    id_actual = FSLIB_ralib->read(dev_num,bank_base,&id_codes,sizeof(id_codes));
	    if (id_actual != sizeof(id_codes)) {
		printf("ERROR READING IID CODE\n");
		rc = FSLIB_ERROR;
	    } else {
		printf("%lx\n",id_codes);
		if ((id_codes & 0xffff0000) != 0x89890000) {
		    printf("    IID Not Intel -- assuming bank not populated\n");
		    continue;
		}
	    }
	}
	
	printf("  Compatible Status Reg: ");
	cmd = IFLASH_CMD_READ_STATUS_REG | (IFLASH_CMD_READ_STATUS_REG << 8);
	cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	if (cmd_actual != sizeof(cmd)) {
	    printf("ERROR WRITING READ STATUS REG COMMAND\n");
	    rc = FSLIB_ERROR;
	} else {
	    status_actual = FSLIB_ralib->read(dev_num,bank_base,&status,sizeof(status));
	    if (status_actual != sizeof(status)) {
		printf("ERROR READING STATUS\n");
		rc = FSLIB_ERROR;
	    } else {
		printf("%x\n",status);
	    }
	}

	printf("  Global     Status Reg: ");
	cmd = IFLASH_CMD_UPLOAD_STATUS_BITS | (IFLASH_CMD_UPLOAD_STATUS_BITS << 8);
	cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	if (cmd_actual != sizeof(cmd)) {
	    printf("ERROR WRITING UPLOAD STATUS BITS COMMAND\n");
	    rc = FSLIB_ERROR;
	} else {
	    cmd = IFLASH_CMD_READ_EXT_STATUS_REG | (IFLASH_CMD_READ_EXT_STATUS_REG << 8);
	    cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	    if (cmd_actual != sizeof(cmd)) {
		printf("ERROR WRITING READ EXTENDED STATUS COMMAND\n");
		rc = FSLIB_ERROR;
	    } else {
		wait_for_ready(dev_num,bank_base,0xffff,0);
		status_offset = bank_base + IFLASH_GLOBAL_STATUS_OFFSET;
		status_actual = FSLIB_ralib->read(dev_num,status_offset,&status,sizeof(status));
		if (status_actual != sizeof(status)) {
		    printf("ERROR READING EXTENDED STATUS\n");
		    printf("status_actual = %d\n",status_actual);
		    rc = FSLIB_ERROR;
		} else {
		    printf("%x\n",status);
		    
		    printf("  Block Status Regs:\n");
		    num_sectors = IFLASH_BANK_SIZE / IFLASH_SECTOR_SIZE;
		    
		    for (sector = 0; sector < num_sectors; sector++) {
			if ((sector % 8) == 0) 
			    printf("    %3d:",sector);
			status_offset = bank_base + (sector * IFLASH_SECTOR_SIZE) + IFLASH_BLOCK_STATUS_OFFSET;
			status_actual = FSLIB_ralib->read(dev_num,status_offset,&status,sizeof(status));
			if (status_actual != sizeof(status)) {
			    printf("  ****");
			} else {
			    printf("  %4x",status);
			}
			if ((sector % 8) == 7)
			    printf("\n");
		    }
		}
	    }
	}
	reset_device_operation_status ( dev_num, bank_base);
	cmd = IFLASH_CMD_READ_ARRAY | (IFLASH_CMD_READ_ARRAY << 8);
	FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
    }

    /*
     * The access of the flash register somehow causes
     * the subsequent flash write to fail. Reason to
     * be investigated. For now, a reset of the flash
     * solves the problem.
     */
    FSLIB_ralib->control(dev_num, RALIB_RESET_DEVICE);

    /*
     * Restore the previous IO write buffer state:
     */
    rsp_restore_io_write_buffer(old_dcl_asic_reg);

    return(rc);
}

