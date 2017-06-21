/* $Id: quicc_serial.h,v 3.2 1995/11/17 18:44:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/quicc_serial.h,v $
 *------------------------------------------------------------------
 * quicc_serial.h -- definiton for 68360 QUICC serial diagnostic code
 *
 * August 22, 1995, John Ng
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_serial.h,v $
 * Revision 3.2  1995/11/17  18:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/22  20:30:20  jng
 * Place holder file for Catwoman 11.0
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SERIAL_TEST_ON  1
#define SERIAL_TEST_OFF 0
#define MFLR   0x1000                      /* maximum frame length */
#define BUF_SIZE  0x100
#define MRBLR BUF_SIZE                     /* maximum receive buffer length */
#define TX_LENGTH BUF_SIZE-2               /* number of bytes transmitted, 
				              2 bytes reserved for CRC */
#define CPAI  0                            /* CPAI loop back mode */
#define SCCLP 1                            /* SCC loopback mode */
#define MAXCLOCKS 11                       /* maximum # of clocks used
					      in the test */

#define SERIAL_RX_BD_MEM   0X200a000       /* Serial channel Rx */
#define SERIAL_TX_BD_MEM   0x200b000       /* Serial channel Tx */

/* SERIAL related register values */
#define QUICC_SERIAL_PSMR_SETTING      0x00
#define QUICC_SERIAL_REG_CLEAR         0xffff

/* SERIAL related parameters */

#define SERIAL_PRAM_C_PRES   0x0000FFFF;   /* 16 bit CRC preset */
#define SERIAL_PRAM_C_MASK   0xF0B8;       /* 16 bit CRC */

#define SERIAL_SCC4_RX_BD_IDX  0x20        /* RxBD for the Serial channel */
#define SERIAL_SCC4_TX_BD_IDX  0x22        /* TxBD for the Serial channel */

#define CTRL_BITS_MASK  (~(CAT_QU_PT_B_DAT_T_CTRL0 | \
			   CAT_QU_PT_B_DAT_T_CTRL1 | \
			   CAT_QU_PT_B_DAT_T_CTRL2 | \
			   CAT_QU_PT_B_DAT_T_CTRL3 | \
			   CAT_QU_PT_B_DAT_T_CTRL4 ))

/* Catwoman Serial port specific port pin values */

#define CAT_QU_PT_A_PAR_T_BTXC   0x4000    /* Serial I/F TXC */
#define CAT_QU_PT_A_PAR_T_BRXC   0x1000    /* Serial I/F RXC */
#define CAT_QU_PT_A_PAR_T_BTXD   0x0080    /* Serial I/F TXD */
#define CAT_QU_PT_A_PAR_T_BRXD   0x0040    /* Serial I/F RXD */

#define CAT_QU_PT_B_DIR_T_CTRL0  0x0001    /* Direction bit for T_CTRL0 */
#define CAT_QU_PT_B_DIR_T_CTRL1  0x0002    /* Direction bit for T_CTRL1 */
#define CAT_QU_PT_B_DIR_T_CTRL2  0x0004    /* Direction bit for T_CTRL2 */
#define CAT_QU_PT_B_DIR_T_CTRL3  0x0008    /* Direction bit for T_CTRL3 */
#define CAT_QU_PT_B_DIR_T_CTRL4  0x0010    /* Direction bit for T_CTRL4 */
 
#define CAT_QU_PT_B_DAT_T_CTRL0  0x0001    /* Data bit for T_CTRL0 */
#define CAT_QU_PT_B_DAT_T_CTRL1  0x0002    /* Data bit for T_CTRL1 */
#define CAT_QU_PT_B_DAT_T_CTRL2  0x0004    /* Data bit for T_CTRL2 */
#define CAT_QU_PT_B_DAT_T_CTRL3  0x0008    /* Data bit for T_CTRL3 */
#define CAT_QU_PT_B_DAT_T_CTRL4  0x0010    /* Data bit for T_CTRL4 */

#define CAT_QU_PT_C_PAR_T_RTS    0x0008    /* PAR value for T_RTS  */
#define CAT_QU_PT_C_DIR_T_BDTR   0x0004    /* PAR value for T_BDTR  */

#define CAT_QU_PT_C_SO_T_CTS     0x0004    /* SO value for T_CTS */
#define CAT_QU_PT_C_SO_T_DCD     0x0008    /* SO value for T_DCD */
