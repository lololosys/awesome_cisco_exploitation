/* $Id: quicc_lib.h,v 3.3.44.3 1996/06/25 17:31:56 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36/quicc_lib.h,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_lib.h,v $
 * Revision 3.3.44.3  1996/06/25  17:31:56  snyder
 * CSCdi61230:  move format string out of #define, save code space
 * Branch: California_branch
 *              Instead of 4 copies of this text, how about just 1, saves
 *              ~200 bytes
 *
 * Revision 3.3.44.2  1996/06/12  19:56:51  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.3.44.1  1996/03/18  21:56:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.1  1996/01/24  22:49:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/21  16:59:52  hampton
 * Move a couple of externs between files.  [CSCdi45965]
 *
 * Revision 3.2  1995/11/17  18:39:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:26:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/24  23:10:11  henniger
 * CSCdi38744:  BRI Fair-queueing on c1000 slows performance & breaks
 * compression
 * Increased buf-desc tx ringsize to '2' for fair-q-ing.
 *
 * Revision 2.1  1995/06/07  22:52:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUICC_LIB_H__
#define __QUICC_LIB_H__

/*
 * The following defines should be used whenever setting
 * or clearing pins (i.e. LED's, Chip selects).  These
 * defines should not be used directly but mapped to more
 * descriptive names (see below).
 *
 * #include "quicc_lib.h"
 *
 * #define TP3420_ASSERT	QUICC_CLR_PB4
 * #define TP3420_NEGATE	QUICC_SET_PB4
 *
 *	TP3420_ASSERT(dpr);
 *	do 3420 type stuff.
 *	TP3420_NEGATE(dpr);
 *
 * Note: QUICC_SET_PXX will write a one to the register and
 * QUICC_CLR_PXX will write a 0.  In the example above the
 * TP3420 chip select is active low.
 */
#define QUICC_SET_PA0(dpr)	((dpr)->regs.pio_padat |=  0x1)
#define QUICC_CLR_PA0(dpr)	((dpr)->regs.pio_padat &= ~0x1)
#define QUICC_SET_PA1(dpr)	((dpr)->regs.pio_padat |=  0x2)
#define QUICC_CLR_PA1(dpr)	((dpr)->regs.pio_padat &= ~0x2)
#define QUICC_SET_PA2(dpr)	((dpr)->regs.pio_padat |=  0x4)
#define QUICC_CLR_PA2(dpr)	((dpr)->regs.pio_padat &= ~0x4)
#define QUICC_SET_PA3(dpr)	((dpr)->regs.pio_padat |=  0x8)
#define QUICC_CLR_PA3(dpr)	((dpr)->regs.pio_padat &= ~0x8)
#define QUICC_SET_PA4(dpr)	((dpr)->regs.pio_padat |=  0x10)
#define QUICC_CLR_PA4(dpr)	((dpr)->regs.pio_padat &= ~0x10)
#define QUICC_SET_PA5(dpr)	((dpr)->regs.pio_padat |=  0x20)
#define QUICC_CLR_PA5(dpr)	((dpr)->regs.pio_padat &= ~0x20)
#define QUICC_SET_PA6(dpr)	((dpr)->regs.pio_padat |=  0x40)
#define QUICC_CLR_PA6(dpr)	((dpr)->regs.pio_padat &= ~0x40)
#define QUICC_SET_PA7(dpr)	((dpr)->regs.pio_padat |=  0x80)
#define QUICC_CLR_PA7(dpr)	((dpr)->regs.pio_padat &= ~0x80)
#define QUICC_SET_PA8(dpr)	((dpr)->regs.pio_padat |=  0x100)
#define QUICC_CLR_PA8(dpr)	((dpr)->regs.pio_padat &= ~0x100)
#define QUICC_SET_PA9(dpr)	((dpr)->regs.pio_padat |=  0x200)
#define QUICC_CLR_PA9(dpr)	((dpr)->regs.pio_padat &= ~0x200)
#define QUICC_SET_PA10(dpr)	((dpr)->regs.pio_padat |=  0x400)
#define QUICC_CLR_PA10(dpr)	((dpr)->regs.pio_padat &= ~0x400)
#define QUICC_SET_PA11(dpr)	((dpr)->regs.pio_padat |=  0x800)
#define QUICC_CLR_PA11(dpr)	((dpr)->regs.pio_padat &= ~0x800)
#define QUICC_SET_PA12(dpr)	((dpr)->regs.pio_padat |=  0x1000)
#define QUICC_CLR_PA12(dpr)	((dpr)->regs.pio_padat &= ~0x1000)
#define QUICC_SET_PA13(dpr)	((dpr)->regs.pio_padat |=  0x2000)
#define QUICC_CLR_PA13(dpr)	((dpr)->regs.pio_padat &= ~0x2000)
#define QUICC_SET_PA14(dpr)	((dpr)->regs.pio_padat |=  0x4000)
#define QUICC_CLR_PA14(dpr)	((dpr)->regs.pio_padat &= ~0x4000)
#define QUICC_SET_PA15(dpr)	((dpr)->regs.pio_padat |=  0x8000)
#define QUICC_CLR_PA15(dpr)	((dpr)->regs.pio_padat &= ~0x8000)
#define QUICC_SET_PB0(dpr)	((dpr)->regs.pip_pbdat |=  0x1)
#define QUICC_CLR_PB0(dpr)	((dpr)->regs.pip_pbdat &= ~0x1)
#define QUICC_SET_PB1(dpr)	((dpr)->regs.pip_pbdat |=  0x2)
#define QUICC_CLR_PB1(dpr)	((dpr)->regs.pip_pbdat &= ~0x2)
#define QUICC_SET_PB2(dpr)	((dpr)->regs.pip_pbdat |=  0x4)
#define QUICC_CLR_PB2(dpr)	((dpr)->regs.pip_pbdat &= ~0x4)
#define QUICC_SET_PB3(dpr)	((dpr)->regs.pip_pbdat |=  0x8)
#define QUICC_CLR_PB3(dpr)	((dpr)->regs.pip_pbdat &= ~0x8)
#define QUICC_SET_PB4(dpr)	((dpr)->regs.pip_pbdat |=  0x10)
#define QUICC_CLR_PB4(dpr)	((dpr)->regs.pip_pbdat &= ~0x10)
#define QUICC_SET_PB5(dpr)	((dpr)->regs.pip_pbdat |=  0x20)
#define QUICC_CLR_PB5(dpr)	((dpr)->regs.pip_pbdat &= ~0x20)
#define QUICC_SET_PB6(dpr)	((dpr)->regs.pip_pbdat |=  0x40)
#define QUICC_CLR_PB6(dpr)	((dpr)->regs.pip_pbdat &= ~0x40)
#define QUICC_SET_PB7(dpr)	((dpr)->regs.pip_pbdat |=  0x80)
#define QUICC_CLR_PB7(dpr)	((dpr)->regs.pip_pbdat &= ~0x80)
#define QUICC_SET_PB8(dpr)	((dpr)->regs.pip_pbdat |=  0x100)
#define QUICC_CLR_PB8(dpr)	((dpr)->regs.pip_pbdat &= ~0x100)
#define QUICC_SET_PB9(dpr)	((dpr)->regs.pip_pbdat |=  0x200)
#define QUICC_CLR_PB9(dpr)	((dpr)->regs.pip_pbdat &= ~0x200)
#define QUICC_SET_PB10(dpr)	((dpr)->regs.pip_pbdat |=  0x400)
#define QUICC_CLR_PB10(dpr)	((dpr)->regs.pip_pbdat &= ~0x400)
#define QUICC_SET_PB11(dpr)	((dpr)->regs.pip_pbdat |=  0x800)
#define QUICC_CLR_PB11(dpr)	((dpr)->regs.pip_pbdat &= ~0x800)
#define QUICC_SET_PB12(dpr)	((dpr)->regs.pip_pbdat |=  0x1000)
#define QUICC_CLR_PB12(dpr)	((dpr)->regs.pip_pbdat &= ~0x1000)
#define QUICC_SET_PB13(dpr)	((dpr)->regs.pip_pbdat |=  0x2000)
#define QUICC_CLR_PB13(dpr)	((dpr)->regs.pip_pbdat &= ~0x2000)
#define QUICC_SET_PB14(dpr)	((dpr)->regs.pip_pbdat |=  0x4000)
#define QUICC_CLR_PB14(dpr)	((dpr)->regs.pip_pbdat &= ~0x4000)
#define QUICC_SET_PB15(dpr)	((dpr)->regs.pip_pbdat |=  0x8000)
#define QUICC_CLR_PB15(dpr)	((dpr)->regs.pip_pbdat &= ~0x8000)
#define QUICC_SET_PB16(dpr)	((dpr)->regs.pip_pbdat |=  0x10000)
#define QUICC_CLR_PB16(dpr)	((dpr)->regs.pip_pbdat &= ~0x10000)
#define QUICC_SET_PB17(dpr)	((dpr)->regs.pip_pbdat |=  0x20000)
#define QUICC_CLR_PB17(dpr)	((dpr)->regs.pip_pbdat &= ~0x20000)
#define QUICC_SET_PC0(dpr)	((dpr)->regs.pio_pcdat |=  0x1)
#define QUICC_CLR_PC0(dpr)	((dpr)->regs.pio_pcdat &= ~0x1)
#define QUICC_SET_PC1(dpr)	((dpr)->regs.pio_pcdat |=  0x2)
#define QUICC_CLR_PC1(dpr)	((dpr)->regs.pio_pcdat &= ~0x2)
#define QUICC_SET_PC2(dpr)	((dpr)->regs.pio_pcdat |=  0x4)
#define QUICC_CLR_PC2(dpr)	((dpr)->regs.pio_pcdat &= ~0x4)
#define QUICC_SET_PC3(dpr)	((dpr)->regs.pio_pcdat |=  0x8)
#define QUICC_CLR_PC3(dpr)	((dpr)->regs.pio_pcdat &= ~0x8)
#define QUICC_SET_PC4(dpr)	((dpr)->regs.pio_pcdat |=  0x10)
#define QUICC_CLR_PC4(dpr)	((dpr)->regs.pio_pcdat &= ~0x10)
#define QUICC_SET_PC5(dpr)	((dpr)->regs.pio_pcdat |=  0x20)
#define QUICC_CLR_PC5(dpr)	((dpr)->regs.pio_pcdat &= ~0x20)
#define QUICC_SET_PC6(dpr)	((dpr)->regs.pio_pcdat |=  0x40)
#define QUICC_CLR_PC6(dpr)	((dpr)->regs.pio_pcdat &= ~0x40)
#define QUICC_SET_PC7(dpr)	((dpr)->regs.pio_pcdat |=  0x80)
#define QUICC_CLR_PC7(dpr)	((dpr)->regs.pio_pcdat &= ~0x80)
#define QUICC_SET_PC8(dpr)	((dpr)->regs.pio_pcdat |=  0x100)
#define QUICC_CLR_PC8(dpr)	((dpr)->regs.pio_pcdat &= ~0x100)
#define QUICC_SET_PC9(dpr)	((dpr)->regs.pio_pcdat |=  0x200)
#define QUICC_CLR_PC9(dpr)	((dpr)->regs.pio_pcdat &= ~0x200)
#define QUICC_SET_PC10(dpr)	((dpr)->regs.pio_pcdat |=  0x400)
#define QUICC_CLR_PC10(dpr)	((dpr)->regs.pio_pcdat &= ~0x400)
#define QUICC_SET_PC11(dpr)	((dpr)->regs.pio_pcdat |=  0x800)
#define QUICC_CLR_PC11(dpr)	((dpr)->regs.pio_pcdat &= ~0x800)

/***************************************************************************
 *                    Hardware Timer (4) Timer Definitions
 *
 ***************************************************************************/

/*
 * Which of the 4 QUICC Timers
 */
typedef enum {
    QUICC_TMR_1,
    QUICC_TMR_2,
    QUICC_TMR_3,
    QUICC_TMR_4
} quicc_timer_t;

typedef struct quicc_info {
    ulong  port_no;
    ushort rx_enable_sccm_mask;
    ushort tx_enable_sccm_mask;
    
    ushort rx_disable_sccm_mask;					     
    ushort tx_disable_sccm_mask;
    
    ushort rx_clear_scce_event;
    ushort tx_clear_scce_event;
} quicc_info_t;

#define QUICC_TMR_TRR_MASK  0xFFFF /* Timer Reference Reg mask */

/*
 * Prototypes
 */
void quicc_install_cpm_exception(quicc_dpr_t *, void (*)(void), ushort);
void quicc_install_avec_exception(void (*)(void), uchar, boolean);
void quicc_avec_enable(uchar);
void quicc_avec_disable(uchar);
boolean quicc_pit_init(quicc_dpr_t *, ushort, void (*)(void), ushort);
void quicc_write_protect_nvram(void);
void quicc_write_enable_nvram (void);
void quicc_set_cimr(ulong);
void quicc_start_timer(quicc_timer_t);
void quicc_stop_timer(quicc_timer_t);
void quicc_init_timer(quicc_timer_t, ushort, boolean, ushort, ushort);
void qassert(char *, ulong);
ushort quicc_version_number(quicc_dpr_t *);
ushort quicc_port_to_channel(ushort);
boolean quicc_send_risc_cmd(quicc_dpr_t *, ushort, ushort);
void quicc_scc_enable_rx(quicc_dpr_t *, quicc_info_t *);
void quicc_scc_disable_rx(quicc_dpr_t *, quicc_info_t *);
void quicc_scc_enable_tx(quicc_dpr_t *, quicc_info_t *);
void quicc_scc_disable_tx(quicc_dpr_t *, quicc_info_t *);
boolean quicc_scc_start(quicc_dpr_t *, quicc_info_t *, boolean, boolean);
void quicc_scc_stop(quicc_dpr_t *, quicc_info_t *, boolean, boolean);
void quicc_spi_init(quicc_dpr_t *);
void quicc_spi_acquire(void);
void quicc_spi_release(void);
boolean quicc_spi_op(quicc_dpr_t *, uchar *, uchar *, ulong);
void quicc_scc_set_listen_addr(quicc_dpr_t *, void *);
void quicc_hdlc_param_init(quicc_dpr_t *, ushort, ushort);
void quicc_uart_param_init (quicc_dpr_t *, ushort , ushort);
void quicc_scc_ethernet_param_init(quicc_dpr_t *, ushort, void *);
boolean quicc_tdma_running(quicc_dpr_t *);
void quicc_tdma_enable(quicc_dpr_t *);
void quicc_tdma_disable(quicc_dpr_t *);
boolean quicc_scc_status(quicc_dpr_t *, ushort, boolean, boolean);
void *quicc_rxbd_alloc(ushort);
void *quicc_txbd_alloc(ushort);
void quicc_scc_set_signal(quicc_dpr_t *, ushort);
void quicc_scc_clear_signal(quicc_dpr_t *, ushort);
boolean quicc_scc_tset_psmr(quicc_dpr_t *, ushort, ushort);
void quicc_scc_write_psmr(quicc_dpr_t *, ushort, ushort);
void quicc_scc_clear_psmr(quicc_dpr_t *, ushort, ushort);
void quicc_scc_write_gsmr(quicc_dpr_t *, ushort, ulong, ulong);
void quicc_scc_clear_gsmr(quicc_dpr_t *, ushort, ulong, ulong);
void quicc_set_ethernet_pins(quicc_dpr_t *);
void quicc_set_bri_pins(quicc_dpr_t *);

quicc_dpr_t *getmbar(void);
void quicc_scc1_interrupt(void);
void quicc_scc2_interrupt(void);
void quicc_scc3_interrupt(void);
void quicc_scc4_interrupt(void);
void quicc_tp3420_interrupt(void);
void quicc_timer_t3(void);
void quicc_timer_t4(void);
void quicc_timer_2ms(void);

#if defined (DEBUG)
#define QASSERT(expr) { if (!(expr)) { qassert(__FILE__, __LINE__); }}
#else
extern const char assertion_failed_str[];   /* in quicc_lib.c */
#define QASSERT(expr) { if (!(expr)) { buginf (assertion_failed_str, __FILE__, __LINE__); flush(); }}
#endif

/*
 * The following table shows the layout of buffer descriptors
 * in dualport ram.  Each section can have its own size.  The
 * relative positions are fixed (using the macros below).  To
 * shift the whole table in memory just change QUICC_BD_OFFSET.
 * A pad area can be added between each area for debugging (
 * see BDDEBUG below.  If this structure is changed then please
 * make sure all the _OFFSET_ values are divisible by 8.
 *
 * RXBD_OFFSET_SMC1	|-------------------------------|
 *		 	|  SMC1 Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SMC1	|-------------------------------|
 *			|  SMC1 Tx Buffer Descriptors.	|
 *	 		|-------------------------------|
 * RXBD_OFFSET_SPI	|-------------------------------|
 *		 	|  SPI Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SPI	|-------------------------------|
 *			|  SPI Tx Buffer Descriptors.	|
 *			|-------------------------------|
 * RXBD_OFFSET_SCC1	|-------------------------------|
 *			|  SCC1 Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SCC1	|-------------------------------|
 *			|  SCC1 Tx Buffer Descriptors.	|
 *			|-------------------------------|
 * RXBD_OFFSET_SCC2	|-------------------------------|
 *			|  SCC2 Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SCC2	|-------------------------------|
 *			|  SCC2 Tx Buffer Descriptors.	|
 *     			|-------------------------------|
 * RXBD_OFFSET_SCC3	|-------------------------------|
 *			|  SCC3 Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SCC3	|-------------------------------|
 *		 	|  SCC3 Tx Buffer Descriptors.	|
 *			|-------------------------------|
 * RXBD_OFFSET_SCC4	|-------------------------------|
 *			|  SCC4 Rx Buffer Descriptors.	|
 *			|-------------------------------|
 * TXBD_OFFSET_SCC4	|-------------------------------|
 *			|  SCC4 Tx Buffer Descriptors.	|
 *			|-------------------------------|
 */

#define QUICC_BD_OFFSET		0x400	/* Start at MBAR + 0x400 */
#define QUICC_BD_SIZE(x)	((x) << 3)
#if defined (BDDEBUG)
#define QUICC_BD_PADDING	8
#else
#define QUICC_BD_PADDING	0
#endif

#define QUICC_RXBD_NUM_SMC1	1
#define QUICC_TXBD_NUM_SMC1	1
#define QUICC_RXBD_OFFSET_SMC1	(QUICC_BD_OFFSET + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SMC1	(QUICC_RXBD_OFFSET_SMC1 +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SMC1) + QUICC_BD_PADDING)

#define QUICC_RXBD_NUM_SPI	1
#define QUICC_TXBD_NUM_SPI	1
#define QUICC_RXBD_OFFSET_SPI	(QUICC_TXBD_OFFSET_SMC1 +\
				 QUICC_BD_SIZE(QUICC_TXBD_NUM_SMC1) + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SPI	(QUICC_RXBD_OFFSET_SPI +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SPI) + QUICC_BD_PADDING)

#define QUICC_RXBD_NUM_SCC1	16
#define QUICC_TXBD_NUM_SCC1	8
#define QUICC_RXBD_OFFSET_SCC1	(QUICC_TXBD_OFFSET_SPI +\
				 QUICC_BD_SIZE(QUICC_TXBD_NUM_SMC1) + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SCC1	(QUICC_RXBD_OFFSET_SCC1 +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SCC1) + QUICC_BD_PADDING)

#define QUICC_RXBD_NUM_SCC2	8
#define QUICC_TXBD_NUM_SCC2	4
#define QUICC_RXBD_OFFSET_SCC2	(QUICC_TXBD_OFFSET_SCC1 +\
				 QUICC_BD_SIZE(QUICC_TXBD_NUM_SCC1) + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SCC2	(QUICC_RXBD_OFFSET_SCC2 +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SCC2) + QUICC_BD_PADDING)

#define QUICC_RXBD_NUM_SCC3	2
#define QUICC_TXBD_NUM_SCC3	2 /* Perf. suffers if only one bd used. */
#define QUICC_RXBD_OFFSET_SCC3	(QUICC_TXBD_OFFSET_SCC2 +\
				 QUICC_BD_SIZE(QUICC_TXBD_NUM_SCC2) + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SCC3	(QUICC_RXBD_OFFSET_SCC3 +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SCC3) + QUICC_BD_PADDING)

#define QUICC_RXBD_NUM_SCC4	16
#define QUICC_TXBD_NUM_SCC4	8
#define QUICC_RXBD_OFFSET_SCC4	(QUICC_TXBD_OFFSET_SCC3 +\
				 QUICC_BD_SIZE(QUICC_TXBD_NUM_SCC3) + QUICC_BD_PADDING)
#define QUICC_TXBD_OFFSET_SCC4	(QUICC_RXBD_OFFSET_SCC4 +\
				 QUICC_BD_SIZE(QUICC_RXBD_NUM_SCC4) + QUICC_BD_PADDING)


/*
 * Interrupt and event masks.
 */

/*
 *  HDLC SCCE and SCCM definitions
 */
#define QUICC_RX_HDLC_ENABLE_SCCM_MASK	(QUICC_SCCM_RXFRAME | \
					 QUICC_SCCM_RXBUF |   \
					 QUICC_SCCM_BSY)

#define QUICC_RX_HDLC_DISABLE_SCCM_MASK  QUICC_RX_HDLC_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_HDLC_ENABLE_SCCM_MASK	(QUICC_SCCM_TXBUF |   \
					 QUICC_SCCM_TXERR)

#define QUICC_TX_HDLC_DISABLE_SCCM_MASK	(QUICC_TX_HDLC_ENABLE_SCCM_MASK | \
					 QUICC_SCCM_GSTOP)

#define QUICC_RX_HDLC_CLEAR_SCCE_EVENT QUICC_RX_HDLC_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_HDLC_CLEAR_SCCE_EVENT (QUICC_TX_HDLC_ENABLE_SCCM_MASK | \
					  QUICC_SCCM_GSTOP)


/*
 * Ethernet SCCE and SCCM definitions
 */
#define QUICC_RX_ETHER_ENABLE_SCCM_MASK	(QUICC_SCCM_RXFRAME | \
					 QUICC_SCCM_RXBUF |   \
					 QUICC_SCCM_BSY)

#define QUICC_RX_ETHER_DISABLE_SCCM_MASK  QUICC_RX_HDLC_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_ETHER_ENABLE_SCCM_MASK	(QUICC_SCCM_TXBUF |   \
					 QUICC_SCCM_TXERR)

#define QUICC_TX_ETHER_DISABLE_SCCM_MASK (QUICC_TX_ETHER_ENABLE_SCCM_MASK | \
					 QUICC_SCCM_GSTOP)

#define QUICC_RX_ETHER_CLEAR_SCCE_EVENT QUICC_RX_ETHER_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_ETHER_CLEAR_SCCE_EVENT (QUICC_TX_ETHER_ENABLE_SCCM_MASK | \
					  QUICC_SCCM_GSTOP)



/*
 * BRI SCCE and SCCM definitions
 */
#define QUICC_RX_BRI_ENABLE_SCCM_MASK	(QUICC_SCCM_RXFRAME | \
					 QUICC_SCCM_RXBUF |   \
					 QUICC_SCCM_BSY)

#define QUICC_RX_BRI_DISABLE_SCCM_MASK  QUICC_RX_HDLC_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_BRI_ENABLE_SCCM_MASK	(QUICC_SCCM_TXBUF |   \
					 QUICC_SCCM_TXERR)

#define QUICC_TX_BRI_DISABLE_SCCM_MASK (QUICC_TX_BRI_ENABLE_SCCM_MASK | \
					 QUICC_SCCM_GSTOP)

#define QUICC_RX_BRI_CLEAR_SCCE_EVENT QUICC_RX_BRI_ENABLE_SCCM_MASK
					
					
#define QUICC_TX_BRI_CLEAR_SCCE_EVENT (QUICC_TX_BRI_ENABLE_SCCM_MASK | \
					  QUICC_SCCM_GSTOP)


/*
 * UART SCCE and SCCM definitions
 */
#define QUICC_RX_UART_ENABLE_SCCM_MASK	(QUICC_SCCM_RXBUF | \
					 QUICC_SCCM_BSY   | \
					 QUICC_SCCM_BRKS  | \
					 QUICC_SCCM_AUTO_BAUD)

#define QUICC_RX_UART_DISABLE_SCCM_MASK  QUICC_RX_UART_ENABLE_SCCM_MASK


#define QUICC_TX_UART_ENABLE_SCCM_MASK	 QUICC_SCCM_TXBUF
					 

#define QUICC_TX_UART_DISABLE_SCCM_MASK	(QUICC_TX_UART_ENABLE_SCCM_MASK | \
					 QUICC_SCCM_GSTOP)

#define QUICC_RX_UART_CLEAR_SCCE_EVENT   QUICC_RX_UART_ENABLE_SCCM_MASK
					
#define QUICC_TX_UART_CLEAR_SCCE_EVENT  (QUICC_TX_UART_ENABLE_SCCM_MASK | \
					  QUICC_SCCE_GSTOP)


#define QUICC_SCC1_DEFAULT_GSMRH	0
#define QUICC_SCC2_DEFAULT_GSMRH	0
#define QUICC_SCC3_DEFAULT_GSMRH	0
#define QUICC_SCC4_DEFAULT_GSMRH	0

#define QUICC_SCC1_DEFAULT_GSMRL	0x1088000C
#define QUICC_SCC2_DEFAULT_GSMRL	0
#define QUICC_SCC3_DEFAULT_GSMRL	0
#define QUICC_SCC4_DEFAULT_GSMRL	0

#define QUICC_SCC1_DEFAULT_PSMR		0x080A
#define QUICC_SCC2_DEFAULT_PSMR		0
#define QUICC_SCC3_DEFAULT_PSMR		0
#define QUICC_SCC4_DEFAULT_PSMR		0

#define QUICC_SCC1_DEFAULT_DSR		0xD555
#define QUICC_SCC2_DEFAULT_DSR		0x7E7E
#define QUICC_SCC3_DEFAULT_DSR		0x7E7E
#define QUICC_SCC4_DEFAULT_DSR		0x7E7E

#define QUICC_SCC1_DEFAULT_SCCM_RX	(QUICC_SCCM_RXFRAME | \
                                         QUICC_SCCM_RXBUF)
#define QUICC_SCC2_DEFAULT_SCCM_RX	(QUICC_SCCM_RXFRAME | \
                                         QUICC_SCCM_RXBUF)
#define QUICC_SCC3_DEFAULT_SCCM_RX	(QUICC_SCCM_RXFRAME | \
                                         QUICC_SCCM_RXBUF)
#define QUICC_SCC4_DEFAULT_SCCM_RX	(QUICC_SCCM_RXFRAME | \
                                         QUICC_SCCM_RXBUF)

#define QUICC_SCC1_DEFAULT_SCCM_TX	(QUICC_SCCM_TXBUF |   \
                                         QUICC_SCCM_TXERR)
#define QUICC_SCC2_DEFAULT_SCCM_TX	(QUICC_SCCM_TXBUF |   \
                                         QUICC_SCCM_TXERR)
#define QUICC_SCC3_DEFAULT_SCCM_TX	(QUICC_SCCM_TXBUF |   \
                                         QUICC_SCCM_TXERR)

/* Take out TXERR for UART mode */
#define QUICC_SCC4_DEFAULT_SCCM_TX	 QUICC_SCCM_TXBUF 
                             

#define QUICC_SCC1_DEFAULT_SCCM_MISC	QUICC_SCCM_BSY
#define QUICC_SCC2_DEFAULT_SCCM_MISC	QUICC_SCCM_BSY
#define QUICC_SCC3_DEFAULT_SCCM_MISC	QUICC_SCCM_BSY
#define QUICC_SCC4_DEFAULT_SCCM_MISC	QUICC_SCCM_BSY

#define QUICC_NUMBER_SCC		4

extern const ulong quicc_scc_intr_mask[];
#endif
