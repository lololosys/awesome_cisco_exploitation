/* $Id: mc68302.h,v 3.2 1995/11/17 17:41:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/mc68302.h,v $
 *------------------------------------------------------------------
 * mc68302.h  MC68302 register definitions, commands, etc.
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: mc68302.h,v $
 * Revision 3.2  1995/11/17  17:41:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:16:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       mc68302.h                                               
DESCRIPTION:  MC68302 register definitions, commands, etc.            
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************


GENERAL NOTES

First, some memory map constants are defined (e.g. BAR is at 0xf2,
RAM_BASE302 is the beginning of RAM, etc). Then macros are defined for
accessing the registers.

*************************************************************************
*************************************************************************
*/


#ifndef     MC68302_H                     /* prevent nested inclusions */
#define     MC68302_H                                   /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/


/* 68302 internal interrupt vectors
*/
#define  ERR_VECT       (VECT_BASE302+0x00)            /* Error Vector */
#define  PB8_VECT       (VECT_BASE302+0x01)   /* GP int 0 (PB8) Vector */
#define  SMC2_VECT      (VECT_BASE302+0x02)             /* SMC2 Vector */
#define  SMC1_VECT      (VECT_BASE302+0x03)             /* SMC1 Vector */
#define  TMR3_VECT      (VECT_BASE302+0x04)          /* Timer 3 Vector */
#define  SCP_VECT       (VECT_BASE302+0x05)              /* SCP Vector */
#define  TMR2_VECT      (VECT_BASE302+0x06)          /* Timer 2 Vector */
#define  PB9_VECT       (VECT_BASE302+0x07)   /* GP int 1 (PB9) Vector */
#define  SCC3_VECT      (VECT_BASE302+0x08)             /* SCC3 Vector */
#define  TMR1_VECT      (VECT_BASE302+0x09)          /* Timer 1 Vector */
#define  SCC2_VECT      (VECT_BASE302+0x0a)             /* SCC2 Vector */
#define  IDMA_VECT      (VECT_BASE302+0x0b)             /* IDMA Vector */
#define  SERR_VECT      (VECT_BASE302+0x0c)   /* SDMA bus error Vector */
#define  SCC1_VECT      (VECT_BASE302+0x0d)             /* SCC1 Vector */
#define  PB10_VECT      (VECT_BASE302+0x0e)  /* GP int 2 (PB10) Vector */
#define  PB11_VECT      (VECT_BASE302+0x0f)  /* GP int 3 (PB11) Vector */

/* define configuration control registers,
* located in exception vector table
*/
#define  BAR_ADR        0xf2                  /* base address register */
#define  SCR_ADR        0xf4                /* system control register */

/* Define register offsets.
*/

/* SCC buffer descriptor bases.
*/
#define  SCC1_BD_BASE   (RAM_BASE302+0x400)
#define  SCC2_BD_BASE   (RAM_BASE302+0x500)
#define  SCC3_BD_BASE   (RAM_BASE302+0x600)

/* SMC buffer descriptors.
*/
#define  SMC1_ADR       (RAM_BASE302+0x666)
#define  SMC2_ADR       (RAM_BASE302+0x66a)

/* SCP buffer descriptor.
*/
#define  SCP_ADR        (RAM_BASE302+0x67a)

/* BERR bus error channel number.
*/
#define  BERR_ADR       (RAM_BASE302+0x67e)

/* Internal registers.
*/

/* IDMA.
*/
#define  CMR_ADR        (RAM_BASE302+0x802)
#define  SAPR_ADR       (RAM_BASE302+0x804)
#define  DAPR_ADR       (RAM_BASE302+0x808)
#define  BCR_ADR        (RAM_BASE302+0x80c)
#define  CSR_ADR        (RAM_BASE302+0x80e)
#define  FCR_ADR        (RAM_BASE302+0x810)

/* Int Controller.
*/
#define  GIMR_ADR       (RAM_BASE302+0x812)
#define  IPR_ADR        (RAM_BASE302+0x814)
#define  IMR_ADR        (RAM_BASE302+0x816)
#define  ISR_ADR        (RAM_BASE302+0x818)

/* PIO.
*/
#define  PACNT_ADR      (RAM_BASE302+0x81e)
#define  PADDR_ADR      (RAM_BASE302+0x820)
#define  PADAT_ADR      (RAM_BASE302+0x822)
#define  PBCNT_ADR      (RAM_BASE302+0x824)
#define  PBDDR_ADR      (RAM_BASE302+0x826)
#define  PBDAT_ADR      (RAM_BASE302+0x828)

/* CSx.
*/
#define  BR0_ADR        (RAM_BASE302+0x830)
#define  OR0_ADR        (RAM_BASE302+0x832)
#define  BR1_ADR        (RAM_BASE302+0x834)
#define  OR1_ADR        (RAM_BASE302+0x836)
#define  BR2_ADR        (RAM_BASE302+0x838)
#define  OR2_ADR        (RAM_BASE302+0x83a)
#define  BR3_ADR        (RAM_BASE302+0x83c)
#define  OR3_ADR        (RAM_BASE302+0x83e)

/* Timers.
*/
#define  TMR1_ADR       (RAM_BASE302+0x840)
#define  TRR1_ADR       (RAM_BASE302+0x842)
#define  TCR1_ADR       (RAM_BASE302+0x844)
#define  TCN1_ADR       (RAM_BASE302+0x846)
#define  TER1_ADR       (RAM_BASE302+0x849)
#define  WRR_ADR        (RAM_BASE302+0x84a)
#define  WCN_ADR        (RAM_BASE302+0x84c)
#define  TMR2_ADR       (RAM_BASE302+0x850)
#define  TRR2_ADR       (RAM_BASE302+0x852)
#define  TCR2_ADR       (RAM_BASE302+0x854)
#define  TCN2_ADR       (RAM_BASE302+0x856)
#define  TER2_ADR       (RAM_BASE302+0x859)

/* CR command register.
*/
#define  CR_ADR         (RAM_BASE302+0x860)

/* SCC configuration bases:
* start at the first reserved offset (e.g. 0x880)
* just because that is the way it is in the manual
*/
#define  SCC1_CON_BASE  (RAM_BASE302+0x880)
#define  SCC2_CON_BASE  (RAM_BASE302+0x890)
#define  SCC3_CON_BASE  (RAM_BASE302+0x8a0)
#define  SPMODE_ADR     (RAM_BASE302+0x8b0)

/* SI.
*/

#define  SIMASK_ADR     (RAM_BASE302+0x8b2)
#define  SIMODE_ADR     (RAM_BASE302+0x8b4)

/* Command register command definitions.
*/
#define  CR_RST         0x80
#define  CR_GCI_ABORT   0x00
#define  CR_GCI_TIMEOUT 0x10
#define  CR_SCC1        0x00
#define  CR_SCC2        0x02
#define  CR_SCC3        0x04
#define  CR_HUNT        0x20
#define  CR_FLG         0x01

/* CMR register values
*/
#define  CMR_RST        0x0002

/* GIMR register values
*/
#define  GIMR_VECT      0x00e0                     /* vector base bits */
#define  GIMR_MOD       0x8000                  /* GIMR interrupt mode */
#define  GIMR_IV7       0x4000                        /* IV 7 external */
#define  GIMR_IV6       0x2000                        /* IV 6 external */
#define  GIMR_IV1       0x1000                        /* IV 1 external */

/* SIMODE register values
*/
#define  SI_NMSI_MODE   0x0000
#define  SI_IDL_MODE    0x0002
#define  SI_D_ON_SCC1   0x0010
#define  SI_D_ON_SCC2   0x0020
#define  SI_D_ON_SCC3   0x0030
#define  SI_B1_ON_SCC1  0x0040
#define  SI_B1_ON_SCC2  0x0080
#define  SI_B1_ON_SCC3  0x00c0
#define  SI_B2_ON_SCC1  0x0100
#define  SI_B2_ON_SCC2  0x0200
#define  SI_B2_ON_SCC3  0x0300
#define  SI_NMSI_SCC2   0x0004
#define  SI_NMSI_SCC3   0x0008

/* SIMASK register values
*/
#define  SIMASK_ALL     0xffff

/* PIO A values
*/
#define  PA_SCC2_PINS   0x00ff
#define  PA_SCC3_PINS   0x1f00

/* TX Buffer Descriptor values
*/
#define  TX_BD_INIT_H   0x5c00              /* Initial value: X I L TC */
#define  TX_BD_INIT_U   0x5000                   /* Initial value: X I */
#define  TX_BD_WRAP     0x2000                             /* Wrap bit */
#define  TX_BD_R        0x8000                              /* Rdy bit */
#define  TX_BD_STAT     0x0003                       /* BD status bits */

/* RX Buffer Descriptor values
*/
#define  RX_BD_INIT     0xd000                 /* Initial value: E X I */
#define  RX_BD_WRAP     0x2000                             /* Wrap bit */
#define  RX_BD_E        0x8000                            /* Empty bit */
#define  RX_BD_STAT_H   0x002f                  /* BD HDLC status bits */
#define  RX_BD_STAT_U   0x003b                  /* BD UART status bits */
#define  RX_BD_BAD_H    0x001f       /* BD bad HDLC status, all but LG */

/* SCON register values
*/
#define  SCON_LB_CLOCK  0x007e               /* Loopback clock divider */
#define  SCON_EXTC      0x4000                /* External clock source */
#define  SCON_TCS       0x2000                /* Transmit clock source */
#define  SCON_RCS       0x1000                /* Receive  clock source */
#define  SCON_B9600     0x00d8         /* 9600 baud for 16.67Mhz clock */

/* C_MASK_x register values
*/
#define  C_MASK_CRC16   0xf0b8                           /* 16-bit CRC */

/* CCR control character register values
*/
#define  CCR_END        0x8000                         /* end of table */

/* SCCE register values (also used for SCCM register)
*/
#define  SCCE_ALL       0xff                    /* clear all, any mode */
#define  SCCE_HDLC_TXE  0x10
#define  SCCE_HDLC_RXF  0x08
#define  SCCE_HDLC_TXB  0x02
#define  SCCE_HDLC_RXB  0x01
#define  SCCE_UART_RX   0x01
#define  SCCE_UART_TX   0x02

/* IMR register values (can also be used for ISR and IPR register)
*/
#define  IMR_ALL        0xffff                         /* IMR all bits */
#define  IMR_PB11       0x8000                          /* enable PB11 */
#define  IMR_PB10       0x4000                          /* enable PB10 */
#define  IMR_SCC1       0x2000                          /* enable SCC1 */
#define  IMR_SDMA       0x1000                /* enable SDMA bus error */
#define  IMR_IDMA       0x0800                          /* enable IDMA */
#define  IMR_SCC2       0x0400                          /* enable SCC2 */
#define  IMR_TMR1       0x0200                       /* enable timer 1 */
#define  IMR_SCC3       0x0100                          /* enable SCC3 */
#define  IMR_PB9        0x0080                           /* enable PB9 */
#define  IMR_TMR2       0x0040                       /* enable timer 2 */
#define  IMR_SCP        0x0020                           /* enable SCP */
#define  IMR_TMR3       0x0010                       /* enable timer 3 */
#define  IMR_SMC1       0x0008                          /* enable SMC1 */
#define  IMR_SMC2       0x0004                          /* enable SMC2 */
#define  IMR_PB8        0x0002                           /* enable PB8 */

/* SCC Mode Register values
*/
#define  SCM_NOF3       0x8000
#define  SCM_NOF2       0x4000
#define  SCM_NOF1       0x2000
#define  SCM_NOF0       0x1000
#define  SCM_DIAG0      0x0010
#define  SCM_ENR        0x0008
#define  SCM_ENT        0x0004
#define  SCM_HDLC_RTE   0x0100
#define  SCM_HDLC_FLG   0x0080

/* UART init: no parity, 8 data bits, 1 stop bit, UART mode
*/
#define  SCM_UART_INIT  0x0181

/* TMRx register values
*/
#define  TMR_PS_VALUE   0x0900                /* timer prescaler value */
#define  TMR_CE3        0x00c0
#define  TMR_CE2        0x0080
#define  TMR_CE1        0x0040
#define  TMR_OM         0x0020
#define  TMR_ORI        0x0010
#define  TMR_FRR        0x0008
#define  TMR_ICLK3      0x0006
#define  TMR_ICLK2      0x0004
#define  TMR_ICLK1      0x0002
#define  TMR_RST        0x0001

/* TRRx register values
*/
#define  TRR_REF_VALUE  10000

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/

/*
* It has been attempted to make register
* accesses look like simple C assignment statements - e.g.
*   UINT32 x;  REG(base) = x;
*  or
*   UINT16 x;  x = REG;
*/

/*
* macro to install interrupt vectors in the 68000 EVT:
*  x is the vector number
*  y is the ISR function address
* e.g. INSTALL_VECTOR(64, example_isr) will install the
* function address example_isr() at EVT locations 256-259
*/

#define  INSTALL_VECTOR(x,y) (*(void **)((x) * 4) = (void **) (y))

/* BAR and SCR register access macros.
*/
#define  BAR                 (*(UINT16 *)(BAR_ADR))
#define  SCR                 (*(UINT32 *)(SCR_ADR))

/* SCC buffer descriptor access macros
* add the BD number * 8 (i.e. n<<3) to the base
*/
#define  SCC_RxBD_SC(b,n)    (*(UINT16 *)(b+((n)<<3)))
#define  SCC_RxBD_LEN(b,n)   (*(UINT16 *)(b+((n)<<3)+0x02))
#define  SCC_RxBD_ADDR(b,n)  (*(BYTE  **)(b+((n)<<3)+0x04))
#define  SCC_TxBD_SC(b,n)    (*(UINT16 *)(b+((n)<<3)+0x40))
#define  SCC_TxBD_LEN(b,n)   (*(UINT16 *)(b+((n)<<3)+0x42))
#define  SCC_TxBD_ADDR(b,n)  (*(BYTE  **)(b+((n)<<3)+0x44))

/* SMC buffer descriptor access macros.
*/
#define  SMC1_RxBD   (*(UINT16 *)(SMC1_ADR))
#define  SMC1_TxBD   (*(UINT16 *)(SMC1_ADR+0x02))
#define  SMC2_RxBD   (*(UINT16 *)(SMC2_ADR))
#define  SMC2_TxBD   (*(UINT16 *)(SMC2_ADR+0x02))

/* SCP buffer descriptor access macro.
*/
#define  SCP_BD      (*(UINT16 *)(SCP_ADR))

/* BERR access macro.
*/
#define  BERR        (*(UINT16 *)(BERR_ADR))

/* internal registers.
*/

/* IDMA access macros.
*/
#define  CMR         (*(UINT16 *)(CMR_ADR))
#define  SAPR        (*(BYTE  **)(SAPR_ADR))
#define  DAPR        (*(BYTE  **)(DAPR_ADR))
#define  BCR         (*(UINT16 *)(BCR_ADR))
#define  CSR         (*(BYTE   *)(CSR_ADR))
#define  FCR         (*(BYTE   *)(FCR_ADR))

/* Int Controller.
*/
#define  GIMR        (*(UINT16 *)(GIMR_ADR))
#define  IPR         (*(UINT16 *)(IPR_ADR))
#define  IMR         (*(UINT16 *)(IMR_ADR))
#define  ISR         (*(UINT16 *)(ISR_ADR))

/* PIO
*/
#define  PACNT       (*(UINT16 *)(PACNT_ADR))
#define  PADDR       (*(UINT16 *)(PADDR_ADR))
#define  PADAT       (*(UINT16 *)(PADAT_ADR))
#define  PBCNT       (*(UINT16 *)(PBCNT_ADR))
#define  PBDDR       (*(UINT16 *)(PBDDR_ADR))
#define  PBDAT       (*(UINT16 *)(PBDAT_ADR))

/* CSx
*/
#define  BR0         (*(UINT16 *)(BR0_ADR))
#define  OR0         (*(UINT16 *)(OR0_ADR))
#define  BR1         (*(UINT16 *)(BR1_ADR))
#define  OR1         (*(UINT16 *)(OR1_ADR))
#define  BR2         (*(UINT16 *)(BR2_ADR))
#define  OR2         (*(UINT16 *)(OR2_ADR))
#define  BR3         (*(UINT16 *)(BR3_ADR))
#define  OR3         (*(UINT16 *)(OR3_ADR))

/* timers
*/
#define  TMR1        (*(UINT16 *)(TMR1_ADR))
#define  TRR1        (*(UINT16 *)(TRR1_ADR))
#define  TCR1        (*(UINT16 *)(TCR1_ADR))
#define  TCN1        (*(UINT16 *)(TCN1_ADR))
#define  TER1        (*(BYTE   *)(TER1_ADR))
#define  WRR         (*(UINT16 *)(WRR_ADR))
#define  WCN         (*(UINT16 *)(WCN_ADR))
#define  TMR2        (*(UINT16 *)(TMR2_ADR))
#define  TRR2        (*(UINT16 *)(TRR2_ADR))
#define  TCR2        (*(UINT16 *)(TCR2_ADR))
#define  TCN2        (*(UINT16 *)(TCN2_ADR))
#define  TER2        (*(BYTE   *)(TER2_ADR))

/* CR command register
*/
#define  CR          (*(BYTE   *)(CR_ADR))

/* SCC
* add the base to the offset
* remember the base is the first RES
*/
#define  SCON(b)     (*(UINT16 *)(b+0x02))
#define  SCM(b)      (*(UINT16 *)(b+0x04))
#define  DSR(b)      (*(UINT16 *)(b+0x06))
#define  SCCE(b)     (*(BYTE   *)(b+0x08))
#define  SCCM(b)     (*(BYTE   *)(b+0x0a))
#define  SCCS(b)     (*(BYTE   *)(b+0x0c))

/* SPMODE
*/
#define  SPMODE      (*(UINT16 *)(SPMODE_ADR))

/* SI
*/
#define  SIMASK      (*(UINT16 *)(SIMASK_ADR))
#define  SIMODE      (*(UINT16 *)(SIMODE_ADR))

/* SCC parameter RAM
*/
#define  RFCR(b)     (*(BYTE   *)(b+0x80))
#define  TFCR(b)     (*(BYTE   *)(b+0x81))
#define  MRBLR(b)    (*(UINT16 *)(b+0x82))
#define  RBD(b)      (*(BYTE   *)(b+0x86))
#define  TBD(b)      (*(BYTE   *)(b+0x92))

/* HDLC parameter RAM
*/
#define  C_MASK_L(b) (*(UINT16 *)(b+0xa0))
#define  C_MASK_H(b) (*(UINT16 *)(b+0xa2))
#define  DISFC(b)    (*(UINT16 *)(b+0xa8))
#define  CRCEC(b)    (*(UINT16 *)(b+0xaa))
#define  ABTSC(b)    (*(UINT16 *)(b+0xac))
#define  NMARC(b)    (*(UINT16 *)(b+0xae))
#define  RETRC(b)    (*(UINT16 *)(b+0xb0))
#define  MFLR(b)     (*(UINT16 *)(b+0xb2))
#define  HMASK(b)    (*(UINT16 *)(b+0xb6))
#define  HADDR1(b)   (*(UINT16 *)(b+0xb8))
#define  HADDR2(b)   (*(UINT16 *)(b+0xba))
#define  HADDR3(b)   (*(UINT16 *)(b+0xbc))
#define  HADDR4(b)   (*(UINT16 *)(b+0xbe))

/* UART parameter RAM
*/
/* Control characters - start with RCCR, which is 0xae+0, so
*  CHARACTER1 is CCR(b,1), CHARACTER2 is CCR(b,2), etc.
*/
#define  CCR(b,r) (*(UINT16 *)(b+0xae+((r) << 1)))

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/

/************************************************************************
*                F U N C T I O N   D E C L A R A T I O N S              *
*************************************************************************
*/

#endif                                                    /* MC68302_H */
