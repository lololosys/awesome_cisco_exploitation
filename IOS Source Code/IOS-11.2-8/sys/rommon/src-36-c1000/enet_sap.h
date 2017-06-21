/* $Id: enet_sap.h,v 3.2 1995/11/17 18:43:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/enet_sap.h,v $
 *------------------------------------------------------------------
 * enet_sap.h
 *
 * December 1994, Yvette Yuen
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for Sapphire Ethernet diagnostic
 *------------------------------------------------------------------
 * $Log: enet_sap.h,v $
 * Revision 3.2  1995/11/17  18:43:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ENET_SAP_H__
#define __ENET_SAP_H__

#define QSCC_LAN               0       /* ethernet is available on SCC1 */
#define ENET_VECT              0x1E    /* CPM interrupt vector */
#define CICR_VB_MASK           0xE0    /* vector base */

/* Ethernet-specific parameters values */
#define ENET_PRAM_C_PRES       0xffffffff
#define ENET_PRAM_C_MASK       0xdebb20e3
#define ENET_PRAM_PADS         0x8888
#define ENET_PRAM_RET_LIM      0x000f
#define ENET_PRAM_MFLR         0x05ee
#define ENET_PRAM_MINFLR       0x0040
#define ENET_PRAM_MAXD1        0x05ee
#define ENET_PRAM_MAXD2        0x05ee
#define ENET_PRAM_PADDR_H      0x5548   /* station address is */
#define ENET_PRAM_PADDR_M      0x3322   /* 00 19 22 33 48 55  */ 
#define ENET_PRAM_PADDR_L      0x1900   /* 00 is the low order byte */
#define ENET_PRAM_P_PER        0x0
#define ENET_RX_BUF_SIZE       0x5f0     /* 1520 bytes */

/* Ethernet related register values */
#define ENET_DSR_DEFAULT       0xd555
#define QUICC_ENET_SCCE_CLEAR  0xffff

/* Ethernet Tx/RX buffer info */
#define	LAN_RX_BD_BASE	       0x0420	 /* 1 Rx BD */
#define	LAN_TX_BD_BASE	       0x0440	 /* 1 Tx BD */
#define LAN_RX_BD              (ADRSPC_QUICC_DPRAM + LAN_RX_BD_BASE)
#define LAN_TX_BD              (ADRSPC_QUICC_DPRAM + LAN_TX_BD_BASE)
#define ENET_BUF_SIZE          0x800
typedef char enet_buf_t[ENET_BUF_SIZE];

/* error flags */
#define POLL_TX_FAIL           0x001
#define POLL_TX_ERR            0x002
#define POLL_RX_FAIL           0x004
#define POLL_RX_ERR            0x008
#define INTR_TX_FAIL           0x040
#define INTR_TX_ERR            0x080
#define INTR_RX_FAIL           0x010
#define INTR_RX_ERR            0x020
#define DATA_MISMATCH          0x100
#define RX_BD_ERR_MASK  (QUICC_BDSTAT_RX_FLV | QUICC_BDSTAT_RX_NOFA |\
                         QUICC_BDSTAT_RX_SHORT | QUICC_BDSTAT_RX_OVRRUN |\
			 QUICC_BDSTAT_RX_COLLIS)
#define TX_BD_ERR_MASK  (QUICC_BDSTAT_TX_LCOL | QUICC_BDSTAT_TX_RLIM |\
			 QUICC_BDSTAT_TX_UNRRUN | QUICC_BDSTAT_TX_CSLOS)

/* Loopback modes */
#define LAN_SCC_LOOPBACK  1
#define LAN_TXVR_LOOPBACK 2
#define LAN_EXT_LOOPBACK  3

/* frame info */
#define SCC1_FIFO_SIZE 32    /* SCC1 TX/RX FIFO size */
#define MIN_PAK_SIZE   14    /* 6:dest addr, 6:src addr, 2:enet proto type */
#define PROTOCOL_TYPE  0x800 /* DOD IP */
#define THREE_WORDS    3     /* address length = 6 bytes/3words */

#define POLL_MODE      0
#define INTR_MODE      1
#define MAX_MODE       2

#define	LEVEL1	1
#define	LEVEL2	2
#define	LEVEL3	3
#define	LEVEL4	4
#define	LEVEL5	5
#define LEVEL6	6
#define LEVEL7  7

/* static function proto type */
static void cleanup();
static unsigned short rand_num(unsigned long num);
static unsigned long filldata(unsigned short data_size, 
			      unsigned short * data_ptr, 
			      unsigned long seed);
static int tx_status(unsigned short *status);
static void set_heading(unsigned short *data_ptr, unsigned short *dest_addr, 
			unsigned short *src_addr, unsigned short proto_type);
static void set_test_mode (int mode);
static void setup_tx_bd(quicc_bd_t * bd_ptr, int mode);
static void setup_rx_bd(quicc_bd_t * bd_ptr, int mode);
static unsigned int check_intr_event();
static unsigned int check_rx_status(quicc_bd_t *rx_bd_ptr); 
static unsigned int check_tx_status(quicc_bd_t *tx_bd_ptr);
static char* gen_err_msg(char *errbufptr, unsigned int error,
			 quicc_bd_t *tx_bd_ptr, quicc_bd_t *rx_bd_ptr);

/* function proto type */
extern void lan_interrupt();
extern int init_enet_scc(); 
extern int setup_enet_loopback(int optype); 
extern int lan_loopback_test(int loopback_mode);

#endif __ENET_SAP_H__
