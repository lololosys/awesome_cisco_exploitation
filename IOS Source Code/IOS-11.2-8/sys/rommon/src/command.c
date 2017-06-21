/* $Id: command.c,v 3.3.30.2 1996/07/08 22:42:32 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/command.c,v $
 *------------------------------------------------------------------
 * command.c
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: command.c,v $
 * Revision 3.3.30.2  1996/07/08  22:42:32  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.30.1  1996/03/21  23:26:40  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:57:42  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:50  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:28:55  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:09  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dbus.h"
#include "command.h"
#include "dbstatus.h"
#include "compress_lzw.h"

/* 
 * Download philosophy:
 * Code Download
 * 
 * 	Code Download is controlled through the DBUS command/data register,
 * register 0x07.  In other IPs, the download is handled through several
 * wcs opcodes, lengths, parities and commands, averaging 6 control bytes for
 * every 4 data bytes.  This goal of this download was to use fewer
 * control bytes.
 * 
 * 	Each image has an image header which starts off with 0x1025.
 * This is the only WCS sequence.  The 0x10 is the opcode to load the
 * WCS opcode.  0x25 is the WCS command which tells the CIP card that 
 * the next several bytes make up the wcs_image_header which is
 * outlined in command.h.  The fields in the header are described in
 * command.h.  There is an addr to indicate where the code will
 * be placed after it has been received.  The length indicates the
 * length of the data portion of the image.  The start_addr, if non-zero,
 * indicates where to jump to run the code that has been recieved.
 * The direction field indicates whether this is a image download to
 * the CIP or an upload to the RP of a memory dump from the CIP.
 * The compression byte indicates if the image being transferred is
 * compressed.  The code_type indicates what type of code is being
 * transferred in this image: vpld, firmware, or a secondary dbus
 * handler.  The version field is used to allow future changes to the 
 * image header.  The cksum field comes after the data and is a
 * 16 bit checksum of the data, carries added back into the 
 * short word.
 * 
 * Uncompressed images:
 * 
 * 	Normally, an uncompressed download will contain three
 * separate images sent in sequence without any breaks as in the
 * following real-life example:
 * 		
 * 		wcs_image_header for VPLD image
 * 		VPLD data (Length specified in image_header
 * 		checksum (16 bits)
 * 
 * 		wcs_image_header for firmware to load into SRAM
 * 		SRAM firmware data
 * 		checksum (16 bits)
 * 
 * 		wcs_image_header for firmware to load into DRAM
 * 		DRAM firmware data
 * 		checksum (16 bits)
 * 
 * 
 * 	Flow:
 * 	
 * dbus_handler()  called for each byte received over the dbus
 * 	command()  called for each byte to register 7
 * 		After 0x10, 0x25 has been recieved, then image_transfer 
 * 		will be called with each byte.
 * 		image_transfer() Processes header bytes and determines 
 * 			what type of transfer this is.  If this is 
 * 			a VPLD tranfer, then each byte is sent to the
 * 			VPLDs through push_vpld().  If this is
 * 			firmware or a secondary handler, each byte
 * 			is stored into memory sequentially, starting at the
 * 			addr field in the image header. 
 * 			When the entire length of data has been processed,
 * 			then the checksum is compared to the one received
 * 			from the RP.  If there is an error, an internal
 * 			error is declared.
 * 
 * Compressed Image:
 * 
 * Normally, the standard three part image described above will be 
 * compressed using a 16 bit Unix compress program.  That compressed
 * image is then given a single wcs_image_header:
 * 
 * 	wcs_image_header,  comp = 1
 * 	compressed data (3 part image compressed as a unit by 16 bit 
 * 		Unix compression)
 * 	checksum
 * 
 * The flow looks something like this:
 * 
 * dbus_handler()
 * 	command()  After 0x10, 0x25 is received, it calls image_transfer()
 * 		image_transfer()  After receiving the header and seeing that
 * 			this is a compressed image, call decomp_image_mode()
 * 
 * decomp_image_mode()  Called only once for an entire set of compressed data
 * 	config_decompress()  Called once for setup
 * 		decompress()  This contains the main loop for
 * 			decompression.  This calls getcode() as needed
 * 			to get the next code of data.  It then decodes
 * 			the codes using the code tables.  For each
 * 			uncompressed byte, it calls put_byte_uncompressed().
 * 			get_code() Get next code.
 * 				get_byte_comp() Take byte from circular queue
 * 					If there is no byte available, this
 * 					polls the dbus interrupt until there is
 * 					a new byte in the queue.
 * 			put_byte_uncompressed()
 * 				image_transfer() Processes uncompressed
 * 					images byte by byte as explained
 * 					above.  The only difference, is
 * 					that the initial 0x10, 0x25 is
 * 					dropped.
 * 
 * While all of this is going on, periodic calls to check_for_data() are
 * made from the moment decomp_image_mode() is called.  These calls to
 * check_for_data() poll the cause register to see if there is a
 * character available from the RP.  This is done because the downloaded
 * compressed image is being decompressed on the fly.  There is not
 * enough time to process the just received character and then return
 * control to the standard handler () to get the next character.
 * Therefore the retrieval of compressed bytes off of the DBUS is
 * interleaved with the decompression of the image.  If there is a
 * character available, then that character is read from the DBUS and
 * placed in the circular queue.  The circular queue is managed by the
 * comp_r_offset and comp_w_offset variables.  The comp_r_offset is the
 * offset of the next byte to be read.  The comp_w_offset is the offset
 * of the next position to write data into.  When the offsets are equal,
 * then the queue is empty.  If incrementing the comp_w_offset by one
 * would make the two equal, then an overrun of the queue has occured.
 * get_byte_comp() gets its data from this queue.
 * 
 * The circular queue is found in the compress_private structure.  The
 * code table is also found in this structure.  The size of this stucture
 * could greatly be reduced if more data could be decompressed while 
 * characters were being received over the dbus.  It could also be
 * reduced in size if 12 bit compression were used.  This would increase
 * the size of the downloaded image by about 10%.
 * 
 * Secondary Handler:
 * 
 * When a secondary handler is downloaded, it is prepended as
 * a separate wcs_image in front of the other image(s).  The 
 * secondary handler cannot be compressed.  The image(s) following
 * the handler can be compressed.  Example:
 * 
 * 	wcs_image_header , code_type = secondary handler
 * 	secondary handler data
 * 	checksum
 * 
 * 	wcs_image_header, compressed
 * 	compressed data
 * 	checksum
 * 
 * After the secondary image is downloaded, the start_addr is called, and
 * the remaining image(s) is downloaded using the newly installed handler.
 * 
 */

/* local static functions */
static boolean memory_range_ok(void);


/* 
 * Inline routine to calculate even parity of single byte.  Parity
 * variable is then shifted left once and new bit is stored in 1st bit
 * position. 
 */
static inline void update_parity (signed char c)
{
    int temp;   /* temp parity for this byte */
    for(temp = 0; c; c<<=1) {
	if (c<0)
	    temp++;
    }
    
    dbuscore_p->db.wcs_parity <<=1; /* move saved partiy over one to make
				  room for new bit */
    if (temp & 1)  /* if temp partiy is odd, */
	dbuscore_p->db.wcs_parity |= 0x01; /* turn on bit        */
}


/*
 * update_checksum (ch, dg_ptr)  updates 16 bit checksum byte by byte
 *     input:  ch 
 *             dg_ptr  pointer to current dbus frame
 */
void update_checksum (uchar ch, dbus_globals *dg_ptr) 
{   
    /* If this is the odd byte, shift it left by one byte  */
    /*  Add the result to the running checksum */
    dg_ptr->wcs_cksum += (ushort)
	(dg_ptr->cksum_add++ % 2 ? 
	       (((ushort) ch ) << 8) : ch);

    /* If there was a carry, subtract 0xffff */
    if(dg_ptr->wcs_cksum > 0xffff)
	dg_ptr->wcs_cksum -= 0xffff;
	    
}

/*
 * Validates memory arguments for memory dump to rp.
 * rc:  TRUE  Memory range is OK
 *      FALSE  Memory range is not valid
 */
static boolean memory_range_ok(void)
{

    uint start, length;
    boolean ksegok = FALSE, range = FALSE;
    
    start = dbuscore_p->db.wh.addr;
    length = dbuscore_p->db.wh.length;
    
    /* Use if in kseg0 */
    if( ( ( start           & ADRSPC_K0BASE) == ADRSPC_K0BASE) &&
        ( ((start + length) & ADRSPC_K0BASE) == ADRSPC_K0BASE) )
	ksegok = TRUE;

    /* Use if in kseg1 */
    if( ( ( start           & ADRSPC_K1BASE) == ADRSPC_K1BASE) &&
        ( ((start + length) & ADRSPC_K1BASE) == ADRSPC_K1BASE) )
	ksegok = TRUE;
	
    /* Use if in kuseg, but change to kseg0 */
    if( ( ( start           & ADRSPC_KUBASE) == ADRSPC_KUBASE) &&
        ((((start | ADRSPC_K0BASE) + length) & ADRSPC_K0BASE) == ADRSPC_K0BASE) ) {
	/* BAD ADDRESS.  They probably wanted kseg0.
	   We will change it to kseg0 */
	dbuscore_p->db.wh.addr |= ADRSPC_K0BASE;
	ksegok = TRUE;
    }

    start &=  PHYSICAL_MASK;   /* turn off mode bits */
    length &=  PHYSICAL_MASK;  /* turn off mode bits */
    if(start < PHY_ADRSPC_RAM_END) {
	range = TRUE;
    } 

    if(range && ksegok)
	return(TRUE);
    else
	return(FALSE);
}


/* 
 *  image_transfer(ch)  routine to process each byte of image download
 *                      or upload
 *    ch -- character variable for byte to be processed
 *    returns:  0          image transfer is complete
 *              non-zero   more image to be read
 */
uint image_transfer (uchar ch, dbus_globals *dg_ptr)
{
    /* 
     * There are three sections:
     *     header:    wcs_count < offset of checksum
     *     data:      wcs_count == OFFSETOF checksum &&
     *                    wcs_data_count < length
     *     checksum:  wcs_data_count == length  &&
     *                    wcs_check_count < 2
     * where length is the length of the data, not the overall packet.
     */
    if(dg_ptr->wcs_head_count < OFFSETOF(wcs_image_header,cksum)) {
	/*
	 * header section
	 * Copy header information into current header storage 
	 */
	
	* (((char *) &dg_ptr->wh) +
	   dg_ptr->wcs_head_count) = ch;
	* ((char *)( (  ( (uint) &dg_ptr->wh) +
	   dg_ptr->wcs_head_count++) | ADRSPC_K1BASE)) = ch;

	if(dg_ptr->wcs_head_count == 2 &&
	   *((ushort *) &dg_ptr->wh) == MAGIC_NUMBERS) 
	{
	    
	    /* This is the beginning of an uncompressed section of
	       a compressed image, reset the head_count to zero,
	       hackkkkkk */
	    dg_ptr->wstate = image_mode;
	    dg_ptr->wcs_head_count = 0;
	    dg_ptr->wcs_data_count = 0;
	    dg_ptr->wcs_check_count = 0;
	    dg_ptr->cksum_add = 1;      /* start with high
						 byte of checksum */
	    dg_ptr->wcs_cksum = 0;
	}

	if(dg_ptr->wcs_head_count > OFFSETOF(wcs_image_header,code_type) && 
	   dg_ptr->wh.code_type == VPLD_TYPE && !dg_ptr->wh.comp) {
	    internal_error();
	}
	    
	if(dg_ptr->wcs_head_count == OFFSETOF(wcs_image_header,cksum) ) {
	    /* 
	     * The header has been received, 
	     * prepare for the data.
	     */
	    if(dg_ptr->wh.comp) {  
		/* This is the beginning of a compressed transfer   */

		write_status();       /* Ack the last byte received */
		decomp_image_mode();  /* Start compress processing  */
		reset_buffer();       /* comp image received, reset */
		return(0);            /* transfer is over           */
	    }
		
	    /* 
	     * Otherwise, this is uncompressed data, switch on the
	     * type 
	     */
	    switch(dg_ptr->wh.code_type) {
	    case VPLD_TYPE:    
		/* vpld data */
		internal_error();
		break;
	    case DBUS_TYPE:
		/* secondary dbus handler is being sent */
/*		led_init(DOWNLOADING_2ND_DBUS); */
		break;
	    case UCODE_TYPE:
		/* operational code is being sent */
		if(dbuscore_p->db.wh.direction == TRANSFER_TO_RSP)  {
		    /* memory transfer to rsp */
		    if(!memory_range_ok()) {
			/* Reflect error to RP in status register;
			 * will cause RP to discontinue this request */  
			dbus_invalid_request_error(FALSE);
			dbuscore_p->db.wstate = awaiting_wcs_opcode;
		    }
		} else { 
/*		    led_init(DOWNLOADING_FIRMWARE); */
		}
		break;
	    default: /* should not happen */
		internal_error();
	    }
	}
	    
    } else if (dg_ptr->wcs_data_count < dg_ptr->wh.length){ 
 
	/****** data section*******/

	if(dg_ptr->wh.direction == TRANSFER_FROM_RSP){
	/* Normal transfer, to our card, copy to memory	 */
	    *((uchar *)((dg_ptr->wh.addr + dg_ptr->wcs_data_count) |
			ADRSPC_K1BASE))= ch;
	    *((uchar *) (dg_ptr->wh.addr + dg_ptr->wcs_data_count++)) = ch;
	} else {
	/* Memory dump to RP */
	    ch = dg_ptr->r.regs.cmd =  *((uchar *) (dg_ptr->wh.addr +
					 dg_ptr->wcs_data_count++));
	}
	update_checksum( ch, dg_ptr);

    } else if (dg_ptr->wcs_check_count < sizeof(ushort)) {

	/****** checksum section ***/

	if(dg_ptr->data_port.ad.addrs & PORT_DBUS_READ) { 
	    /* Memory dump */
	    dg_ptr->wh.cksum = ~dg_ptr->wcs_cksum;
	    ch = dg_ptr->r.regs.cmd =
		*( (char *) &dg_ptr->wh.cksum + dg_ptr->wcs_check_count++);
	    
	    if (dg_ptr->wcs_check_count == sizeof(ushort) ) {
		/* This is the last byte of the checksum */
		return 0;
	    } else {
		/* There is one more checksum byte */
		return 1;
	    }
	} else {
	    /* Code download */
	    *(((char *) &dg_ptr->wh.cksum) +
	      dg_ptr->wcs_check_count++) = ch; /* copy cksum byte */

	    /* If this is the last byte of the checksum */
	    if (dg_ptr->wcs_check_count == sizeof(ushort) ) {
		/* transfer is complete */
		if( dg_ptr->wh.cksum !=(ushort) ~dg_ptr->wcs_cksum ) {
		    /* chksums do not match */
		    if(!dg_ptr->wh.comp) {
			/* 
			 * If this is a subsection, declare an error.
			 * We do not check overall checksum
			 */
			wcs_parity_error(); 
		    }
		} else { 
		    /* checksums do match */
		    switch(dg_ptr->wh.code_type) {
		    case DBUS_TYPE:
			/* secondary dbus handler */
			if(!dg_ptr->wh.comp) {
			    dbuscore_p->db.dbus_handler_p =
				dg_ptr->wh.start_addr;
			}
			break;
		    case UCODE_TYPE:
			if( !dg_ptr->wh.comp) {
			    /* ucode type, not compressed */
			    dbuscore_p->db.start_addr = dg_ptr->wh.start_addr;
			    dbuscore_p->software_version =
				dg_ptr->wh.version;
			}
                        break;
		    default:	
			internal_error();
		    }
		}
		dg_ptr->wh.length = 0;      /* clear length,
						     transfer over */
/*
 		flush_data_cache((uint)dbuscore_p,
				 (uint)&dbuscore_p->interrupt_routine);
*/
		return(0); /* transfer is complete */
	    } /* end of if */
	}  /* end of else */
    } else  {  
	/* length has exceeded all expectations, declare error */
	wcs_parity_error();
	return(0);
    }
    return(1);  /* transfer is not complete */
}

static int data_pending = 0;
static uchar *load_address = NULL;

/*
 * Routine to handle writes to the dbus register 0x07, command
 * register.  Called from dbus_handler()
 */
void ucode_command_data (void)
{
    uchar opcode;
    uint logcount, count;
    uchar ch=0;
    void (* diag_call)(data_port_type *);
    
    /*
     * Set up state machine with three states.  Initial state is to wait for
     * the next WCS command.
     */


    if(dbuscore_p->db.data_port.ad.addrs & PORT_DBUS_READ) { /* Read request */
	if(dbuscore_p->db.wstate != image_mode) {
	    /* Reads should only get this far if we are in transfer
	       mode, just return the last thing written */
	    return;
	}
    }

    dbuscore_p->db.r.regs.cmd = ch = dbuscore_p->db.data_port.dbus_data;

    if(dbuscore_p->db.r.regs.status & DBUS_ERROR_MASK) {
	return;
    }


    /* WCS state machine */
    switch(dbuscore_p->db.wstate) {
    case awaiting_wcs_opcode:
	/* copy the opcode to the local command variable */
	opcode = return_opcode(ch);

	/* Get logcount and calculate count.  Error if count > 16 */
	logcount = return_logcount(ch);
	count = 1 << logcount;
	if(count > 16){
	    wcs_instr_error();
	    return;
	}

	switch(opcode) {
	case WCS_WR_CMD_REG:
	    if(count != 1) {
		wcs_instr_error();
	    } else {
		dbuscore_p->db.wstate = loading_cmd_reg;
	    }
	    break;
        case WCS_WR_DAT_REG:
	    dbuscore_p->db.wstate = awaiting_data;
	    data_pending = count;
	    dbuscore_p->db.wcs_parity = 0;
	    break;
	case WCS_LOADED:
	    if(count != 1) {
		wcs_instr_error();
	    } else {
		dbuscore_p->db.r.regs.ctrl |= CNTL_WCS_LOADED;       /* Set Loaded bit in
							control Register */
		dbuscore_p->db.r.regs.ctrl &= ~ ((uchar) CNTL_WCS_LOADING); /* Loading cannot be set at
							the same time */
	    }
	    break;
        case WCS_RD_DAT_REG:
	    dbuscore_p->db.wstate = reading_data;
	    data_pending = count;
	    dbuscore_p->db.wcs_parity = 0;
	    break;
	/* The following have not been implemented */
        case WCS_RESET_ROM_ADDRESS:
        case WCS_INCREMENT_ROM_ADDRESS:
        case WCS_RD_CMD_REG:
        case WCS_READ_ROM:
	    wcs_instr_error();
	    break;
	default:
	    wcs_instr_error();
	} /* end opcode switch */
	
	break;
    case loading_cmd_reg:
	    /* save the command in the command register, await data  */
	    switch(ch) {
	    case WCS_IMAGE_TRANSFER:
		dbuscore_p->db.r.regs.status = 0;    /* clear status */
		dbuscore_p->db.com_reg = ch;
		dbuscore_p->db.wstate = image_mode;
		dbuscore_p->db.wcs_head_count = 0;
		dbuscore_p->db.wcs_data_count = 0;
		dbuscore_p->db.wcs_check_count = 0;
		dbuscore_p->db.cksum_add = 1;      /* start with high
						     byte of checksum */
		dbuscore_p->db.wcs_cksum = 0;
		/* led_init(DOWNLOADING_FIRMWARE); */
		break;
	    case WCS_DIAG:
		dbuscore_p->db.com_reg = ch;
		dbuscore_p->db.wstate = awaiting_data;
		break;
	    case WCS_INIT_CNT:
	    case WCS_WR_RAM:
	    case WCS_WR_PAR:
	    case WCS_RD_RAM:
	    case WCS_RD_PAR:
		/* Other commands we understand */
		dbuscore_p->db.com_reg = ch;
		dbuscore_p->db.wstate = awaiting_wcs_opcode;
		break;
	    default:
		/* We do not understand this command, signal an error */
		wcs_instr_error();
		dbuscore_p->db.wstate = awaiting_wcs_opcode;		
	    }
	break;

    /* We are receiving or sending an image.  When it is complete,
       return to awaiting_wcs_opcode state */
    case image_mode:
	status_rsp_not_ready(FALSE);
	if(image_transfer(ch, &dbuscore_p->db) == 0) {
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;
	    if(dbuscore_p->db.wh.code_type != DBUS_TYPE) {
#if defined(SVIP)
		status_rsp_ready(TRUE); 
#endif
#if defined(RSP2)
		status_rsp_ready(FALSE); 
#endif
	    }
	}
    	break;

    /* We are receiving data. data_pending is non-zero */
    case awaiting_data:
	switch (dbuscore_p->db.com_reg) {
	case WCS_WR_RAM:
	    *load_address++ = ch;
	    break;
	case WCS_WR_PAR:
	    if (dbuscore_p->db.wcs_parity !=  ch)
		wcs_parity_error();
	    break;
	case WCS_INIT_CNT:
	    load_address = (char *) (((uint) load_address) << 8);
	    load_address = (char *)  ((uint) load_address | (uint) ch);
	    break;
	case WCS_DIAG:
/*
	    diag_call = (void (*)(data_port_type *)) dbuscore_p->sram_diag_ep;
	    diag_call(&dbuscore_p->db.data_port);
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;
*/
	    break;
	default:
	    wcs_instr_error();
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;		
	}  /* end com_reg switch while writing data to CIP */
	data_pending--;
	update_parity((signed char) ch);
	if(data_pending <= 0)
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;
	break;

    /* Data is being read from our memory.  data_pending is non-zero */
    case reading_data:
	switch (dbuscore_p->db.com_reg) {
	case WCS_RD_RAM:
	    db_hw->data_port.ad.data_out = ch = *load_address++;
	    break;
	case WCS_RD_PAR:
	    db_hw->data_port.ad.data_out = dbuscore_p->db.wcs_parity;
	    break;
	default:
	    wcs_instr_error();
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;		
	}  /* end com_reg switch while reading data from CIP */

	data_pending--;
	update_parity((signed char) ch);
	if(data_pending <= 0)
	    dbuscore_p->db.wstate = awaiting_wcs_opcode;
	break;
    default:
	wcs_instr_error();
	dbuscore_p->db.wstate = awaiting_wcs_opcode;		
    } /* end state switch */
}


/*
 * The ucode_command_data() function maintains state information
 * which has to be initialized when we get reset by the RP
 */
void command_initialization(void) 
{
    data_pending = 0;
    load_address = NULL;
}








