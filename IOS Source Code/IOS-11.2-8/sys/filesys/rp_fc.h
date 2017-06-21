/* $Id: rp_fc.h,v 3.2 1995/11/17 09:08:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/rp_fc.h,v $
 *------------------------------------------------------------------
 * RP specific PCMCIA flash card definitions
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_fc.h,v $
 * Revision 3.2  1995/11/17  09:08:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 7000 flash card status register -- byte value at ADRSPC_FCSTATUS
 *
 *    bit 7 - not used  
 *    bit 6 - not used  
 *    bit 5 - not used  
 *    bit 4 - not used  
 *    bit 3 - not used  
 *    bit 2 - flash card external write protect switch
 *            0 not write protected
 *            1 write protected
 *    bit 1 - Card Detected Signal 2 on the PCMCIA socket
 *            0 card detected (inserted)
 *            1 card not detected (not inserted)
 *    bit 0 - Card Detected Signal 1 on the PCMCIA socket
 *            0 card detected (inserted)
 *            1 card not detected (not inserted)
 */

#include "../../boot/src-68-c7000/pcmap.h"

/* flash card is not inserted in the socket */
#define FLASH_CARD_NOT_PRESENT     \
             ((*(volatile uchar *)ADRSPC_FCSTATUS) & 0x03)

/* flash card is write protected */
#define FLASH_CARD_WRITE_PROT \
             ((*(volatile uchar *)ADRSPC_FCSTATUS) & 0x04)

#define INTEL_2P_FLASH_CARD_ID  	0x89A0
#define INTEL_2P_FLASH_CARD_SECTOR_SIZE	0x20000
#define INTEL_2P_FLASH_CARD_BANK_SIZE  	0x400000
#define INTEL_2P_FLASH_CARD_READY  	0x8080

#define RP_FLASH_CARD_MAX_SIZE  	0x1000000

/* bundled MONLIB label in memory */
extern uchar bndl_monlib_start, bndl_monlib_end;
extern ulong bndl_monlib_crc;

extern char *copy_bndl_monlib_to_buffer(int *size);
extern void rp_dir_flash(void);


