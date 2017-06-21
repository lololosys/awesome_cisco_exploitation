/* $Id: pred_eeprom.h,v 3.1.68.1 1996/03/21 23:31:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c7100/pred_eeprom.h,v $
 *------------------------------------------------------------------
 * pred_eeprom.h -- Predator EEPROM support
 *
 * November 1995, Steve Zhang
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pred_eeprom.h,v $
 * Revision 3.1.68.1  1996/03/21  23:31:34  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1995/12/01  04:23:49  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.1.2.1  1995/11/23  19:14:38  szhang
 * Branch: ELC_branch
 * Initial source code commit to ELC branch for Predator (C7100) ROM
 * monitor.
 *
 * Revision 3.1  1995/11/23  00:55:10  mbeesley
 * Add placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * DRAM copy of Midplane NMC93C46 contents
 */
typedef struct c7100_mpeeprom_copy_t_ {
    uchar eeprom_version;
    uchar model;
    uchar hardware_version_high;
    uchar hardware_version_low;
    uchar serial_number[4];
    uchar part_number[4];
    uchar mac_base[6];
    uchar mac_block_size[2];
    uchar test_history;
    uchar RMA_number[3];
    uchar mfg_date;
    uchar reserved[7];
} c7100_mpeeprom_copy_t;

/*
 * DRAM copy of CPU assembly NMC93C46 contents
 */
typedef struct c7100_cpueeprom_copy_t_ {
    uchar eeprom_version;
    uchar controller_type;
    uchar hardware_version_high;
    uchar hardware_version_low;
    uchar serial_number[4];
    uchar pca_item_number[4];
    uchar test_history;
    uchar RMA_number[3];
    uchar board_revision;
    uchar deviation_number[2];
    uchar board_configuration;
    uchar mfg_date[3];
    uchar card_connector_type;
    uchar asic_revision[2];
    uchar undefined1[5];
    uchar development_engineering;
    uchar undefined2[32];
} c7100_cpueeprom_copy_t;

/* end of file */
