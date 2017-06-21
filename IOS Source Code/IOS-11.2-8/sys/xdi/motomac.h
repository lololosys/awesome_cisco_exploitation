/* $Id: motomac.h,v 3.1.60.5 1996/06/10 22:21:59 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/xdi/motomac.h,v $
 *------------------------------------------------------------------
 * motomac.h - 1F MIF68840 based PCI port adaptor support
 *             ( Port adapter platforms common MAC definitions )
 *
 * December 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: motomac.h,v $
 * Revision 3.1.60.5  1996/06/10  22:21:59  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.1.60.4  1996/05/18  08:54:49  fsunaval
 * CSCdi57640:  1F - Software tweaks to augment new EPLD.
 * Branch: California_branch
 * 1.Remove 1F specific latency timer changes.
 *
 * Revision 3.1.60.3  1996/04/30  07:05:12  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 *
 * Revision 3.1.60.2  1996/04/11  04:39:33  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.60.1  1996/03/22  00:16:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/02/17  05:54:49  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.1.2.3  1996/02/05  07:19:28  fsunaval
 * Branch: ELC_branch
 *
 * Revision 3.1.2.2  1996/01/25  12:10:57  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.2.1  1995/12/23  04:42:27  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/12/19  20:36:10  fsunaval
 * Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MOTOMAC_H__
#define __MOTOMAC_H__
      
/*************************************************************************/
/* defines for register numbers of ifddi */ 

#define NOP_REG             0x0     /* No Operation */
#define SR1                 0x1     /* Status Register 1 */
#define DTR                 0x2     /* Data Register */
#define IOR                 0x3     /* Input/Output Register */
#define BADR                0x4     /* This Port's Address */
#define IMR1                0x5     /* Interupt Mask Register 1 */
#define PSR                 0x7     /* Port Status Register */
#define ADR                 0x8     /* Other Port's Address */
#define CMR                 0x9     /* Command Register */
#define PNOP                0xa 
#define CER                 0xb     /* Command Extension Register */
#define IMR2                0xc     /* Interrupt Mask Register 2 */
#define SR2                 0xd     /* Status Register 2 */
#define FCR                 0xf     /* FSI Control Register */

/*************************************************************************/

/* Bits of SR1 (Status register 1)  and IMR1 (Interrupt Mask register 1) */

#define COMMAND_DONE        0x80000000
#define CONTROL_REG_FREE    0x40000000
#define HOST_ERROR          0x20000000                  
#define INTERNAL_OP_ERROR   0x10000000
#define RECEIVE_OV_ERROR_5  0x08000000
#define RECEIVE_OV_ERROR_4  0x04000000
#define PORT_OP_ERROR_B     0x02000000
#define PORT_OP_ERROR_A     0x01000000
#define CAMEL_INTERRUPT     0x00800000
#define SMT_CNT_EXPIRED     0x00400000
#define RING_ERROR_5        0x00200000
#define RING_ERROR_4        0x00100000
#define RING_ERROR_3        0x00080000
#define RING_ERROR_2        0x00040000
#define RING_ERROR_1        0x00020000
#define RING_ERROR_0        0x00010000

#define RECEIVE_COMPLETE_5  0x00002000
#define CLEAR_RECEIVE_5     0xffff9fff  /* Complement of RECEIVE_COMPLETE_5 */

#define RECEIVE_COMPLETE_4  0x00001000
#define CLEAR_RECEIVE_4     0xffffefff  /* Complement of RECEIVE_COMPLETE_4 */

#define RING_CMD_COMPLETE_3 0x00000800
#define RING_CMD_COMPLETE_2 0x00000400
#define RING_CMD_COMPLETE_1 0x00000200
#define RING_CMD_COMPLETE_0 0x00000100
#define RING_NOT_RDY_5      0x00000020
#define RING_NOT_RDY_4      0x00000010
#define RING_NOT_RDY_3      0x00000008
#define RING_NOT_RDY_2      0x00000004
#define RING_NOT_RDY_1      0x00000002
#define RING_NOT_RDY_0      0x00000001

#define CLEAR_SR1           0x00000000

/* This is the cumulative bits of all possible errors including the STE 
 * (SMT Timer Expired) interupt, the rationale being that these occur 
 * rarely and just one check should suffice.  This check is done at 
 * the wrapper level
 */

#define ERROR_SMT_FRAME     0x3f3f2000
#define ERROR_NO_SMT_FRAME  0x3f3f0000

/*************************************************************************/

/* Bits of PSR (Port Status register) */

/* FIXME */

/*************************************************************************/

/* FCR (FSI Control Register) */

/* IRT = 0000 RING_RDY */

#define WRITE_RING_RDY_REG_0    0x00000000
#define WRITE_RING_RDY_REG_1    0x01000000
#define WRITE_RING_RDY_REG_2    0x02000000
#define WRITE_RING_RDY_REG_3    0x03000000
#define WRITE_RING_RDY_REG_4    0x04000000
#define WRITE_RING_RDY_REG_5    0x05000000

#define READ_RING_RDY_REG_0     0x80000000
#define READ_RING_RDY_REG_1     0x81000000
#define READ_RING_RDY_REG_2     0x82000000
#define READ_RING_RDY_REG_3     0x83000000
#define READ_RING_RDY_REG_4     0x84000000
#define READ_RING_RDY_REG_5     0x85000000


/*************************************************************************/

/* IRT = 0001, IRI = 000 MACIF TRANSMIT CONTROL REGISTER (MTR) */

#define WRITE_MACIF_TRANSMIT_DIS_3      0x08810000
#define WRITE_MACIF_TRANSMIT_DIS_2      0x08410000
#define WRITE_MACIF_TRANSMIT_DIS_1      0x08210000
#define WRITE_MACIF_TRANSMIT_DIS_0      0x08110000
#define WRITE_MACIF_TRANSMIT_DIS_7      0x08090000
#define WRITE_MACIF_TRANSMIT_DIS_6      0x08050000


/* enable channel 3 and disable all others */
#define WRITE_MACIF_TRANSMIT_EN_3       0x087d0000
/* enable channel 0 and disable all others */
#define WRITE_MACIF_TRANSMIT_EN_0       0x08ed0000

/* disable all */
#define WRITE_MACIF_TRANSMIT_DIS_ALL    0x08fc0000

/* global enable-bit set. */
#define WRITE_MACIF_TRANSMIT_EN         0x08010000

#define READ_MACIF_TRANSMIT_REG         0x88000000
/*************************************************************************/

/* IRT = 0001, IRI = 001 MACIF RECEIVE CONTROL REGISTER (MRR) */
/* Enable receive rings 4 and 5, no CRC, no Receive Parity Enable. */

#define WRITE_MACIF_RECEIVE_DIS_ALL     0x09800000
#define WRITE_MACIF_RECEIVE_EN_ALL      0x09830000
#define WRITE_MACIF_RECEIVE_EN_RE5      0x09820000
#define WRITE_MACIF_RECEIVE_EN_RE4      0x09810000
#define READ_MACIF_RECEIVE_REG          0x89000000

/*************************************************************************/

/* IRT = 0001, IRI = 010 IFDDI CONFIGURATION REGISTER (ICR) */
/* CAM Disable (CDS), Double Buffer Mode (DBM) */

#define WRITE_ICR                       0x0a000000
#define     CAM_DISABLE                 0x00100000
#define     DOUBLE_BUFFER_MODE          0x00010000

#define READ_ICR                        0x8a000000

/*************************************************************************/

/* IRT = 0001, IRI = 100, 101  RECEIVE FRAME TYPE REGISTERS (RFR) */
/* Ring 4 is for receiving LLC frames, Ring 5 for SMT. MAC frames are
 * not received
 */

#define WRITE_RFR_4                     0x0c180000 /* LLC (sync and async) */
#define WRITE_RFR_5                     0x0d020000 /* SMT only */
#define READ_RFR_4                      0x8c000000
#define READ_RFR_5                      0x8d000000

/*************************************************************************/

/* IRT = 0010, IRI = 000, 001 SMT TIMER LOAD VALUE REGISTERS (STL) */
/* STLO and STL1 contain the SMT timer load value. This value is the intended 
 * expiration tiome of the SMT timer measured in units of 512 BYTCLKs (40.96us 
 * at 12.5 MHz). Since writing to STL! causes the timer to start running, STLO should 
 * be written first. IRI 000 = STL0 and IRI 001 - STL1.
 * We currently program this SMT Timer to 1 second = 5f5e
 */

#define WRITE_SMT_TIMER_STL_0           0x105e0000
#define WRITE_SMT_TIMER_STL_1           0x115f0000

/*************************************************************************/

/* IRT = 0010, IRI = 010, 011 SMT TIMER READ VALUE REGISTERS (STR) */
/* STR0 and STR1 together contain the current value of the SMT Timer. These 
 * are read only registers that may be read any time. Reading either of these registers 
 * will stop the SMT Timer so that the two reads provide a consistent value
 */

#define READ_SMT_TIMER_STR_0            0x92000000
#define READ_SMT_TIMER_STR_1            0x93000000

/*************************************************************************/

/* IRT = 0011, IRI = 100, 101 RECEIVE BUFFER LENGTH REGISTER (RBR) */
/* There are two RBRs, one for each receive ring. The RBR defines the length 
 * of the receive data buffers for each ring in external memory. If the buffer
 * length is provided in the receive buffer descriptor, then this register
 * should be set to zero. (done automatically at power up).
 */ 
#define READ_RECEIVE_LENGTH_REG_4       0x9c000000
#define READ_RECEIVE_LENGTH_REG_5       0x9d000000

/*************************************************************************/

/* IRT = 0011, IRI = 110, 111 BURST LIMIT REGISTER (BLR) */
/* reg 6 is for port A which is the only port we use. 
 * It's set to maximum limit of 256 bytes
 */

#define WRITE_BURST_LIMIT_REG_A         0x1e070000
#define READ_BURST_LIMIT_REG_A          0x9e000000
#define READ_BURST_LIMIT_REG_B          0x9f000000

/*************************************************************************/

/* IRT = 0100, IRI = 000-111 FIFO WATERMARK REGISTER (FWR) */
/* There are eight FWRs, one for each ring. We use the following.
 * Ring 0   -> Transmit direct beacon
 * Ring 3   -> Transmit 
 * Ring 4   -> Receive LLC
 * Ring 5   -> Receive SMT
 * The transmit and receive watermarks are set to 480  (480/32 = 15)
 */

#define WRITE_FIFO_WATERMARK_REG_0      0x20020000
#define WRITE_FIFO_WATERMARK_REG_3      0x233f0000
#define WRITE_FIFO_WATERMARK_REG_4      0x24030000 /* 96 bytes */
#define WRITE_FIFO_WATERMARK_REG_5      0x25030000 /* 96 bytes */
#define WRITE_FIFO_WATERMARK_REG_6      0x263f0000  
#define READ_FIFO_WATERMARK_REG_0       0xa0000000
#define READ_FIFO_WATERMARK_REG_3       0xa3000000
#define READ_FIFO_WATERMARK_REG_4       0xa4000000
#define READ_FIFO_WATERMARK_REG_5       0xa5000000


/*************************************************************************/

/* IRT = 0110, IRI = 000-101 RING PARAMETER REGISTER (RPR) */
/* There are six RPRs, one for each transmit and receive channel. 
 * We program the RPR for regs 0, 4 and 5 as follows
 * Ring Parity Enable=1 (RPE), Data Parity Enable=1 (DPE), Ring Port
 * Assignment=0 (RPA)
 * Ring Data Assignment=0 (RDA), RML3-RML0 = 0110 (64 entries, max len =
 * 512 bytes)
 * This is not well understood. Might need modifications. FIXME.
 */

#define WRITE_RING_PARAMETER_REG_0          0x30000000
#define WRITE_RING_PARAMETER_REG_3          0x33080000 /* 256 desc. */
#define WRITE_RING_PARAMETER_REG_4          0x34070000 /* 128 desc. */
#define WRITE_RING_PARAMETER_REG_5          0x35050000 /*  32 desc. */
#define READ_RING_PARAMETER_REG_0           0xb0000000
#define READ_RING_PARAMETER_REG_3           0xb3000000
#define READ_RING_PARAMETER_REG_4           0xb4000000
#define READ_RING_PARAMETER_REG_5           0xb5000000

/*************************************************************************/

/* IRT = 0110, IRI = 110-111 COMMAND PARAMETER REGISTER (CPR) */
/* There are two CPRs, one for each port (IRI = 6 for port A and IRI = 7 for port B)
 * We only define for port A as follows
 * Data Parity Enable = 1 (DPE), Comman Data Assignment = 0 (CDA)
 */

#define WRITE_COMMAND_PARAMETER_REG_A       0x36000000
#define READ_COMMAND_PARAMETER_REG_A        0xb6000000
#define READ_COMMAND_PARAMETER_REG_B        0xb7000000

/*************************************************************************/

/* IRT = 1000, IRI = 110-111 PORT MEMORY PAGE RESISTER (PMP) */
/* The external page length size utilized on port A is 256 bytes (0000 0011) */

#define WRITE_PORT_MEMORY_PAGE_REG_A        0x46ff0000
#define READ_PORT_MEMORY_PAGE_REG_A         0xc6000000
#define READ_PORT_MEMORY_PAGE_REG_B         0xc7000000

/*************************************************************************/

/* IRT = 1011, IRI = 110-111 PORT CONTROL REGISTER (PCR) */
/* There are two PCRs one for each port (IRI=6 for port A and IRI=7 for port B)
 * We program port A as follows
 * Host Parity Enable = 0 ie. not enabled (HPE), Parity control = 0 odd (PC), 
 */

#define WRITE_PORT_CONTROL_DIS_REG_A        0x5e000000
#define WRITE_PORT_CONTROL_EN_REG_A         0x5e210000
#define READ_PORT_CONTROL_REG_A             0xde000000
#define READ_PORT_CONTROL_REG_B             0xdf000000

/*************************************************************************/

/* IRT = 1100, IRI = 100,101 MAXIMUM RECEIVE MEMORY SPACE REGISTER (RMR) */
/* We program this at approx 2k for each receive ring.
 * RMR = 63
 */

#define WRITE_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_4    0x645f0000 /* 3.0K */
#define WRITE_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_5    0x650f0000 /* 0.5K */
#define READ_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_4     0xe4000000
#define READ_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_5     0xe5000000

/*************************************************************************/

/* IRT = 1110, IRI = 000,111 RING STATE REGISTER (RSR) */
#define READ_RING_STATE_REG_0                       0xf0000000
#define READ_RING_STATE_REG_3                       0xf3000000
#define READ_RING_STATE_REG_4                       0xf4000000
#define READ_RING_STATE_REG_5                       0xf5000000

/*************************************************************************/

/* IRT = 1111, IRI = 000 INTERNAL ERROR STATUS REGISTER (IER) */
/* This read-only register is used to identify relevant internal 
 * operation errors. 
 */

#define READ_INTERNAL_ERROR_STATUS_REG      0xf8000000
#define     INTERNAL_OVERRUN_ERROR          0x00800000
#define     INTERNAL_UNDERRUN_ERROR         0x00400000
#define     TRANSMIT_INTERNAL_PARITY_ERROR  0x00200000
#define     MER_MAC_ERROR                   0x00100000
#define     MEMORY_OVERRUN_ERROR            0x00080000
#define     PORTB_INTERNAL_ERROR            0x00020000
#define     PORTA_INTERNAL_ERROR            0x00010000

/*************************************************************************/

/* IRT = 1111, IRI = 010 FSI REVISION REGISTER */

#define READ_FSI_REV_REG                    0xfa000000
/*************************************************************************/

/* IRT = 1111, IRI = 011 IFDDI REVISION REGISTER */

#define READ_IFDDI_REV_REG                  0xfb000000
/*************************************************************************/



/* IRT = 1111, IRI = 111 SOFTWARE RESET */

#define SOFTWARE_RESET                      0xff000000

/*************************************************************************/
/*************************************************************************/

/* defines for registers of mac */

#define CONTROL_REG_A               0x0
#define CONTROL_REG_B               0x1
#define INTERRUPT_MASK_REG_A        0x2
#define INTERRUPT_MASK_REG_B        0x3
#define INTERRUPT_MASK_REG_C        0x4
#define MY_SHORT_ADDRESS_REG        0x10
#define MY_LONG_ADDRESS_REG_A       0x11
#define MY_LONG_ADDRESS_REG_B       0x12
#define MY_LONG_ADDRESS_REG_C       0x13
#define TREQ_REG                    0x14                
#define TVX_VALUE_AND_TMAX_REG      0x15
#define REV_NO_REG                  0x1c
#define INTERRUPT_EVENT_REG_C       0x1d
#define VOID_TIME_REG               0x1e
#define TOKEN_COUNT_REG             0x1f
#define FRAME_COUNT_REG             0x20
#define LOST_AND_ERROR_COUNT_REG    0x21
#define INTERRUPT_EVENT_REG_A       0x22
#define INTERRUPT_EVENT_REG_B       0x23
#define RECEIVE_STATUS_REG          0x24
#define TRANSMIT_STATUS_REG         0x25
#define NEGOTIATED_TTRT_A_REG       0x26
#define NEGOTIATED_TTRT_B_REG       0x27
#define INFORMATION_FIELD_REG_A     0x28
#define INFORMATION_FIELD_REG_B     0x29
#define MAC_BIST_SIGNATURE_REG      0x2a
#define TVX_TIMER_REG               0x2b
#define TRT_TIMER_REG_A             0x2c
#define TRT_TIMER_REG_B             0x2d
#define THT_TIMER_REG_A             0x2e
#define SENT_COUNT_THT_TIMER_REG    0x2f
#define PACKET_REQUEST_REG          0x30
#define REC_CRC_REG_A               0x31
#define REC_CRC_REG_B               0x32
#define TRANSMIT_CRC_REG_A          0x33
#define TRANSMIT_CRC_REG_B          0x34

/*************************************************************************/

/* defines for the register bits of mac CONTROL_REG_A */
#define MAC_ON              0x8000
#define SET_BIT_5           0x4000
#define SET_BIT_4           0x2000
#define CANONICAL_FORM      0x1000
#define FLUSH_SA47          0x0800
#define COPY_IND_LLC        0x0020
#define COPY_GRP_LLC        0x0010
#define DISABLE_BRDCST      0x0008

/*************************************************************************/

/* defines for the register bits of mac CONTROL_REG_B */
#define RING_PURGE          0x8000
#define FDX_MODE            0x4000
#define BRIDGE_STRIP        0x2000
#define TXPARITY_ON         0x1000

#define MAC_RESET           0x0100
#define RESET_CLAIM         0x0300
#define RESET_BEACON        0x0200

#define FSI_BEACON          0x0080
#define IGNORE_SACAM        0x0020
#define EXT_DA_MATCH        0x0010
#define MAC_MODE_CTL        0x0004
#define RCDAT_PARITY_ON     0x0002
#define TXDAT_BAD_PAR       0x0001
/*************************************************************************/

/* defines for the register bits of mac INTERRUPT_MASK_REG_A */
#define BEACON_RCVD         0x0800
#define CLAIM_RCVD          0x0400
#define FRAME_ERR           0x0200
#define FRAME_RCVD          0x0100
#define RING_OP_CHNG        0x0040
#define TVX_EXPIR           0x0010
#define LATE_TKN            0x0008
#define RCVRY_FAIL          0x0004
#define DUPL_TKN            0x0002
#define DUPL_ADDR           0x0001
/*************************************************************************/


/* defines for the register bits of mac INTERRUPT_MASK_REG_B */
#define MY_BEACON           0x8000
#define OTHER_BEACON        0x4000
#define MY_CLAIM            0x0800
#define BAD_T_BID           0x0400
#define NOT_COPIED          0x0010
/*************************************************************************/

/* defines for the register bits of mac TRANSMIT_STATUS_REG */
#define RING_OP             0x0800

/*************************************************************************/

/* defines for the register bits of mac RECEIVE_STATUS_REG */
#define R_FLAG              0x1000

/*************************************************************************/


/* defines for registers of CAMEL  */

#define CAMEL_CONTROL_REG           0
#define CAMEL_INTERRUPT_MASK_REG    1
#define CAMEL_INTERRUPT_EVENT_REG   4
#define CAMEL_ZERO_REG_A            5
#define CAMEL_INTERRUPT_LOC_REG     6
#define CAMEL_REV_NUMBER_REG        7
#define CAMEL_ZERO_REG_B            10
#define CAMEL_ZERO_REG_C            11  
#define CAMEL_ZERO_REG_D            12
#define CAMEL_ZERO_REG_E            13
#define CAMEL_ZERO_REG_F            14

/*************************************************************************/

/* defines for the register bits of camel CAMEL_CONTROL_REG */
#define BYPASS_CTRL_ON              0x0001
/*************************************************************************/

/* basic definitions for the internal CAM */
#define CAM_ADD_PREFIX  0xb9800000
#define CAM_CMP_PREFIX  0xbd800000
#define CAM_READ_PREFIX 0xba000000

/* definitions for internal CAM indications */
#define MIF68840_INTERNAL_CAM_VALID     0x00800000
#define MIF68840_INTERNAL_CAM_MATCH     0x01000000

#endif __MOTOMAC_H__
