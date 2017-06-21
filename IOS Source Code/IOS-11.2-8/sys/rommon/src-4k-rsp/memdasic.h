/* $Id: memdasic.h,v 3.3 1996/01/17 23:31:40 mansonw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/rommon/src-4k-rsp/memdasic.h,v $
 *------------------------------------------------------------------
 * memdasic.h
 *
 * Mar 1995, Steve J.Zhang
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines MEMD controller register structure
 *------------------------------------------------------------------
 * $Log: memdasic.h,v $
 * Revision 3.3  1996/01/17  23:31:40  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:46:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define PAD unsigned char

typedef struct {
    /* Physical Base Address is 0xE10000 */
    volatile unsigned char PMT_slot0reg0_hi;     /*pagemap table slot0 reg0 hi */
    PAD pad1[7];
    volatile unsigned char PMT_slot0reg0_lo;     /*pagemap table slot0 reg0 lo */
    PAD pad2[7];
    volatile unsigned char PMT_slot0reg1_hi;     /*pagemap table slot0 reg1 hi */
    PAD pad3[7];
    volatile unsigned char PMT_slot0reg1_lo;     /*pagemap table slot0 reg1 lo */
    PAD pad4[8135];
    volatile unsigned char PMT_slot15reg30_hi;     /*pagemap table slot15 reg30 hi */
    PAD pad5[7];
    volatile unsigned char PMT_slot15reg30_lo;     /*pagemap table slot15 reg30 lo */
    PAD pad6[7];
    volatile unsigned char PMT_slot15reg31_hi;     /*pagemap table slot15 reg31 hi */
    PAD pad7[7];
    volatile unsigned char PMT_slot15reg31_lo;     /*pagemap table slot15 reg31 lo */
    PAD pad8[8199];
    volatile unsigned char pagemap_hi_reg;     /*pagemap config register(rd/wr) */
    PAD pad9[7];
    volatile unsigned char pagemap_lo_reg;     /*pagemap config register(rd/wr) */
    PAD pad10[23];                          /* there is reserved area here */
    volatile unsigned char memdconfig_reg;  /*memd config register(rd/wr) */
    PAD pad11[7];
    volatile unsigned char int_stat_reg;   /*interrupt lvl-6 status(Rd)*/
    PAD pad12[7];                        
    volatile unsigned char QA_cmd_reg;      /*QA Cmd Int Mask Reg(rd/wr) */
    PAD pad13[7];
    volatile unsigned char QA_network_reg;  /*QA network Int Mask Reg(rd/wr) */
    PAD pad14[7];
    volatile unsigned char memd_diags_reg;  /*MEMD_control diags Reg(rd/wr) */
    PAD pad15[7];
    volatile unsigned char rp_err_status_reg;  /*RP err status register(rd)*/
    PAD pad16[7];
    volatile unsigned short cybus_data_par_reg; /*Cybus Data Par flags (rd)*/
    PAD pad17[6];
    volatile unsigned char cybus_virtual_addr_reg;  /*CyBus Virtual addr reg(rd)*/
    PAD pad18[7];
    volatile unsigned char cybus_phy_addr_reg;  /*CyBus physical addr reg(rd) */
    PAD pad19[7];
    volatile unsigned char cybus_misc_reg;  /* cybus misc error register(rd) */
    PAD pad20[7];
    volatile unsigned char cybus_slotcmd_reg;  /*cybus slot and cmd register(rd)*/
    PAD pad21[7];
    volatile unsigned char cybus_err_type_reg;  /*cybus error type register(rd)*/
#ifdef RSP2
    PAD pad22[1159];
/*  RSP2 registers start at XXe14500 */
    volatile unsigned char cy1_vaddr_err_reg;  /*cy1 virtual address error register*/
    PAD pad23[7];
    volatile unsigned char cy1_phy_addr_err_reg;  /*cy1 phy address error register*/
    PAD pad24[7];
    volatile unsigned char cy1_misc_err_reg;  /*cy1 misc phy address error register*/
    PAD pad25[7];
    volatile unsigned char cy1_noslot_command_reg;  /*cy1 Noslot and Command register*/
    PAD pad26[7];
    volatile unsigned char cybus1_err_reg;  /*cybus1 error register*/
    PAD pad27[7];
    volatile unsigned char cybus_master_err_reg;  /*cybus master register*/
    PAD pad28[7];
    volatile unsigned char mc_err_int_status_reg;  /*MC Error Interrupt Status register*/
    PAD pad29[7];
    volatile unsigned char mc_int_mask_reg;  /*MC Interrupt Mask register*/
    PAD pad30[7];
    volatile unsigned char qa_status_range_reg;  /*QA status range register */
    PAD pad31[7];
    volatile unsigned char qa_range_reg;  /*QA RANGE register*/
    PAD pad32[7];
    volatile unsigned char BH_range_reg;  /*Buffer Header Range register*/
    PAD pad33[7];
    volatile unsigned char buff_range_reg;  /*Buffer Range register*/
    PAD pad34[7];
    volatile unsigned char reserved;
    PAD pad35[7];
    volatile unsigned char cybus_det_table_0_7_reg;  /*CyBus Determination Table 0-7  register*/
    PAD pad36[7];
    volatile unsigned char cybus_det_table_8_f_reg;  /*CyBus Determination Table 8-15 register*/
    PAD pad37[7];
    volatile unsigned char cybus_loopback_reg;  /*CyBus Diag Loopback register*/
#endif
} MEMD_ASIC_STRUCT;

#undef PAD

/*
 * Define a pointer to the MEMD CONTROLLER ASIC
 */
#define MEMD_ASIC      ((MEMD_ASIC_STRUCT *)(KSEG1_ADRSPC_MEMD_ASIC))


/* 
 * Define the bit value for MEMD configuration register 
 */
#define MEMD2M		0x1


/*
 * Define the bit value for QA_cmd_reg
 * and QA_network_reg
 */
#define QA_NETWORK_MASK   0x1
#define QA_CMD_MASK       0x1


/* 
 * Define the bit value for MEMD control diagnostic register 
 */
#define ERR_CHK_EN      0x1


/*
 * Define the bit value for MEMD interrupt 6 status register 
 */
#define CYBUS_MEMD_ERR  0x20
#define QA_ASIC_ERR     0x40
#define POWER_FAIL      0x80


/* end of module */












