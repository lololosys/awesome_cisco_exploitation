/* $Id: dev_slot0.h,v 3.1 1995/11/09 09:10:10 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/dev_slot0.h,v $
 *------------------------------------------------------------------
 * Flash card support functions for 7000 ROM monitor
 *
 * December 1994, Szewei Ju
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dev_slot0.h,v $
 * Revision 3.1  1995/11/09  09:10:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:09  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:30  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:39  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MONLIB_INIT       0x0
#define MONLIB_PRINTALL   0xFFFFFFFF
#define MONLIB_NODEBUG    0x80000000

#define EXEC_HDRSIZ (sizeof(struct exec))

#define DEFAULT_MONLIB  "gs7-fslib-m"

/*
 * Intel Flash EPROM command codes:  reverse em for Merlot!
 */
#define READ_MEMORY     0
#define READ_ID_CODES   0x90
#define ERASE           0x20
#define ERASE_CONFIRM   0xD0
#define CLEAR_CSR       0x50
#define READ_CSR        0x70
#define READ_ESR        0x71
#define ERASE_VERIFY    0xA0
#define PROGRAM         0x40
#define PROGRAM_VERIFY  0xC0
#define RESET           0xFF
#define DEVICE_READY    0x80
#define INTEL_FC_READY  ((DEVICE_READY << 8) | DEVICE_READY)

/* Intel series 2+ flash card constants */
#define INTEL_2P_FLASH_CARD_ID        0x89A0
#define INTEL_2P_FC_BLOCK_SIZE        0x20000
#define INTEL_2P_FC_DEVICE_SIZE       0x200000
#define INTEL_2P_FC_BANK_SIZE         (2*INTEL_2P_FC_DEVICE_SIZE)

#define RP_FLASH_CARD_MAX_SIZE          0x1000000     /* 16 MB */
#define RP_FLASH_CARD_MAX_FILE          128

/* flash card is not inserted in the socket */
#define FLASH_CARD_NOT_PRESENT     \
             (((*(volatile uchar *)ADRSPC_FCSTATUS) & 0x03) == 0x03)

/* flash card inserted but is write protected */
#define FLASH_CARD_WRITE_PROT \
             (((*(volatile uchar *)ADRSPC_FCSTATUS) & 0x04) == 0x04)

/* flash card is inserted and writable */
#define FLASH_CARD_OK \
             (((*(volatile uchar *)ADRSPC_FCSTATUS) & 0x07) == 0)


extern boolean monlib_init(char *);
extern void show_flash_card(boolean);
extern char *flash_card_find_file(char *);
extern boolean flash_card_copy_to_ram(char *, ulong, ulong);








