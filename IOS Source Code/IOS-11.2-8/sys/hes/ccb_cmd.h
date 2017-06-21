/* $Id: ccb_cmd.h,v 3.2.56.6 1996/08/29 02:23:17 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/ccb_cmd.h,v $
 *------------------------------------------------------------------
 * ccb_cmd.h -- RP => SP/IP Command definitions
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccb_cmd.h,v $
 * Revision 3.2.56.6  1996/08/29  02:23:17  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.2.56.5  1996/08/20  00:34:12  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.2.56.4  1996/07/17  01:44:54  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.2.56.3  1996/07/08  22:24:58  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/05/22  23:25:49  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 * Pass serial encap to serial driver in VIP
 *
 * Revision 3.2.56.1  1996/03/21  22:39:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/23  04:39:49  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2  1995/11/17  09:15:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:59:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/17  20:50:14  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.5  1995/08/31  22:38:43  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.4  1995/08/09  00:19:20  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.3  1995/07/28  19:40:14  mcieslak
 * Added AIP specific commands to "test cbus" as part of:
 * CSCdi35575:  test cbus needs IP-specific commands
 *
 * Revision 2.2  1995/07/16  22:30:34  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.1  1995/06/07  20:38:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CCB_CMD_H__
#define __CCB_CMD_H__

/****************************************************************
 *
 * Constants for the done field in the CCB
 *
 ***************************************************************/
#define CCB_CMDARG0              0       /* Offset to cmd_arg0 */
#define CCB_ARG1RES1             4       /* Offset to args 1 and 2 */
#define CCB_RES0DONE             8       /* Offset to result and done */
#define CCB_DONE                 0xA     /* Offset to done */
 
#define CCB_NOT_DONE             0
#define CCB_DONE_OK              1
#define CCB_DONE_ERROR_BIT  0x8000
#define CCB_OUT_OF_RANGE    0xfffe
#define CCB_UNKNOWN         0xffff
#define CCB_DONE_ERROR    ((CCB_DONE_ERROR_BIT << 16) + CCB_DONE_OK)
#define CCB_DONE_OUT_OF_RANGE ((CCB_OUT_OF_RANGE << 16) + CCB_DONE_OK)
#define CCB_DONE_UNKNOWN      ((CCB_UNKNOWN << 16) + CCB_DONE_OK)

#ifndef ASMINCLUDE
 
/****************************************************************
 *
 * Structures for accessing the CCB's efficiently
 *
 ***************************************************************/
typedef struct cmd_arg0_ {
    volatile ushort cmd;
    volatile ushort arg0;
}cmd_arg0_t;
 
typedef struct arg1_res1_ {
    volatile ushort arg1;
    volatile ushort res1;
}arg1_res1_t;
 
typedef struct res0_done_ {
    volatile ushort res0;
    volatile ushort done;
}res0_done_t;
 
typedef struct  diag0_diag1_ {
    volatile ushort diag0;
    volatile ushort diag1;
}diag0_diag1_t;

/****************************************************************
 *
 * Command Control Block (CCB)
 *
 ***************************************************************/
 
struct ccbtype_ {
    union {
        cmd_arg0_t cmdarg0;
        volatile ulong ulong0;
    } w0;
    union {
        arg1_res1_t arg1res1;
        volatile ulong ulong1;
    } w1;
    union {
        res0_done_t res0done;
        volatile ulong ulong2;
    } w2;
    union {
        diag0_diag1_t diag0diag1;
        volatile ulong ulong3;
    } w3;
};
 
#define ccb_cmd       w0.cmdarg0.cmd
#define ccb_arg0      w0.cmdarg0.arg0
#define ccb_cmdarg0   w0.cmdarg0
 
#define ccb_arg1      w1.arg1res1.arg1
#define ccb_res1      w1.arg1res1.res1
#define ccb_arg1res1  w1.arg1res1
 
#define ccb_res0      w2.res0done.res0
#define ccb_done      w2.res0done.done
#define ccb_res0done  w2.res0done
#define ccb_word2     w2.ulong2
 
#define ccb_diag0      w3.diag0diag1.diag0
#define ccb_diag1      w3.diag0diag1.diag1
#define ccb_diag0diag1 w3.ulong3
 
#endif   /* ifndef ASMINCLUDE */

/******************************************************************************
 * Commands supported by IP type
 *
 * +--+--------+--------+--------+--------+--------+--------+--------+--------+
 * |  |  EIP   |  TRIP  |  FIP   |  HIP   |  FSIP  |  MIP   |  AIP   |  CIP   |
 * +--+--------+--------+--------+--------+--------+--------+--------+--------+
 * |  |        |        |        |        |        |        |        |        |
 * | 0|VERSION |VERSION |VERSION |VERSION |VERSION |VERSION |VERSION |VERSION |
 * | 1|RESET   |RESET   |RESET   |RESET   |RESET   |RESET   |RESET   |RESET   |
 * | 2|SELECT  |SELECT  |SELECT  |SELECT  |SELECT  |SELECT  |SELECT  |SELECT  |
 * | 3|PAGE SEL|PAGE SEL|PAGE SEL|PAGE SEL|PAGE SEL|PAGE SEL|PAGE SEL|PAGE SEL|
 * | 4|        |REG SEL |REG SEL |        |        |        |        |        |
 * | 5|        |REG WRIT|REG WRIT|        |        |        |        |        |
 * | 6|        |REG READ|REG READ|        |        |        |        |        |
 * | 7|        |        |        |        |        |        |        |        |
 * | 8|MEMD SEL|MEMD SEL|MEMD SEL|MEMD SEL|MEMD SEL|MEMD SEL|MEMD SEL|MEMD SEL|
 * | 9|MEMD WR |MEMD WR |MEMD WR |MEMD WR |MEMD WR |MEMD WR |MEMD WR |MEMD WR |
 * |10|MEMD RD |MEMD RD |MEMD RD |MEMD RD |MEMD RD |MEMD RD |MEMD RD |MEMD RD |
 * |11|        |MEMB SEL|        |        |        |        |        |        |
 * |12|        |MEMB CWR|        |        |        |        |        |        |
 * |13|        |MEMB CRD|        |        |        |        |        |        |
 * |14|        |SPYGLASS|PAGE SEL|        |        |        |        |        |
 * |  |        |SETUP   |        |        |        |        |        |        |
 * |15|        |MEMB WR |RETURN  |        |        |        |        |        |
 * |16|        |MEMB RD |        |        |        |        |        |        |
 * |17|MEMA SEL|MEMA SEL|MEMA SEL|MEMA SEL|MEMA SEL|MEMA SEL|        |        |
 * |18|MEMA WR |MEMA WR |MEMA WR |MEMA WR |MEMA WR |MEMA WR |        |        |
 * |19|MEMA RD |MEMA RD |MEMA RD |MEMA RD |MEMA RD |MEMA RD |        |        |
 * |20|        |XLNX ARG|        |        |        |        |        |        |
 * |21|        |XLNX CMD|        |        |        |        |        |        |
 * |22|        |TX DMA T|        |        |        |        |        |        |
 * |23|        |RX DMA T|        |        |        |        |        |        |
 * |24|        |        |        |        |        |        |        |        |
 * |25|        |        |        |        |        |        |        |        |
 * |26|        |        |        |        |        |        |        |        |
 * |27|        |        |        |        |        |        |        |        |
 * |28|        |SPG RES |CAM CMD |        |        |        |        |        |
 * |29|        |SPG SRB |CAM RD A|        |        |        |        |        |
 * |30|        |ADAP CHK|CAM WR A|        |        |        |        |        |
 * |31|        |ARB VAL |MEMD HMR|        |        |        |        |        |
 * |32|STATUS  |STATUS  |STATUS  |STATUS  |STATUS  |STATUS  |STATUS  |STATUS  |
 * |33|STATION |STATION |STATION |STATION |        |        |        |        |
 * |34|AF SEL  |AF SEL  |AF SEL  |        |        |        |        |        |
 * |35|AF RD   |AF RD   |AF RD   |        |        |        |        |        |
 * |36|AF WR   |AF WR   |AF WR   |        |        |        |        |        |
 * |37|CARRIER |CARRIER |CARRIER |CARRIER |CARRIER |CARRIER |CARRIER |        |
 * |38|ERRORS  |ERRORS  |ERRORS  |ERRORS  |ERRORS  |        |ERRORS  |        |
 * |39|        |        |        |        |        |        |        |        |
 * |40|        |        |        |        |        |        |        |        |
 * |41|        |        |        |        |        |        |        |        |
 * |42|        |        |        |        |        |        |        |        |
 * |43|        |        |        |        |        |        |        |        |
 * |44|        |        |        |        |        |        |        |        |
 * |45|        |        |        |        |        |        |        |        |
 * |46|        |        |        |        |        |        |        |        |
 * |47|        |        |        |        |        |        |        |        |
 * |48|RX BYTES|        |        |        |        |        |        |        |
 * |49|RXOFFSET|        |        |        |        |        |        |        |
 * |50|        |        |        |        |        |        |        |        |
 * |51|        |        |        |        |        |        |        |        |
 * |52|        |        |        |        |        |        |        |        |
 * |53|        |        |        |        |        |        |        |        |
 * |54|        |        |        |        |        |        |        |        |
 * |55|        |        |        |        |        |        |        |        |
 * |56|        |SPG INIT|        |        |        |        |        |        |
 * |57|        |SPG SEL |        |        |        |        |        |        |
 * |58|        |SPG WR  |        |        |        |        |        |        |
 * |59|        |SPG RD  |        |        |        |        |        |        |
 * |60|        |        |        |        |        |        |        |        |
 * |61|        |        |        |        |        |        |        |        |
 * |62|        |        |        |        |        |        |        |        |
 * |63|        |        |        |        |        |        |        |        |
 * |64|        |        |        |        |        |        |        |        |
 * |65|        |        |        |        |        |        |        |        |
 * |66|        |        |        |        |        |        |        |        |
 * |67|        |        |        |        |        |        |        |        |
 * |68|        |        |        |        |        |        |        |        |
 * |69|        |        |        |        |        |        |        |        |
 * |70|        |        |        |        |        |        |        |        |
 * |71|        |        |        |        |        |        |        |        |
 * |72|        |        |        |        |        |        |        |        |
 * |73|        |        |        |        |        |        |        |        |
 * |74|        |        |        |        |        |        |        |        |
 * |75|        |        |        |        |        |        |        |        |
 * |76|        |        |        |        |        |        |        |        |
 * |77|        |        |        |        |        |        |        |        |
 * |78|        |        |        |        |        |        |        |        |
 * |79|        |        |        |        |        |        |        |        |
 * |80|        |        |        |        |        |        |        |        |
 * +--+--------+--------+--------+--------+--------+--------+--------+--------+
 ******************************************************************************/

/*
 * CCB Global command codes.  These commands do not require a
 *   CCB_CMD_SELECT -- they operate on the Interface Processor
 *   Controller card as a whole.
 */
#define CCB_CMD_VERSION                 0       /* read version */
#  define CCB_VERSION_SW                    0   /* get ucode version number */
#  define CCB_VERSION_HW                    1   /* get what the f/w thinks */
                                                /* the hardware is */
#define CCB_CMD_RESET                   1       /* reset controller */
#  define CCB_RESET_HARD                    0   /* argument to CCB_CMD_RESET */
#  define CCB_RESET_SOFT                    1   /* argument to CCB_CMD_RESET */
#define CCB_CMD_SELECT                  2       /* select an interface */
#define CCB_CMD_REG_SELECT              4       /* select an ALU register */
#define CCB_CMD_REG_WRITE               5       /* write the selected register */
#define CCB_CMD_REG_READ                6       /* read the selected register */
#define CCB_CMD_MEMD_SELECT             8       /* select memd address */
#define CCB_CMD_MEMD_WRITE              9       /* write to the selected MEMD address */
#define CCB_CMD_MEMD_READ               10      /* read from the selected MEMD address*/
#define CCB_CMD_MEMA_SELECT             17      /* select MEMA address */
#define CCB_CMD_MEMA_WRITE              18      /* write selected MEMA address */
#define CCB_CMD_MEMA_READ               19      /* read selected MEMA address */

/*
 * Interface specific command codes. Valid after a CCB_CMD_SELECT.
 */
#define CCB_CMD_CTRLR_VERSION            0      /* Query version command */
#  define CCB_VERSION_PLD                   2   /* get pld version */
#  define CCB_VERSION_ROM                   3   /* get rom version */
#define CCB_CMD_RETURN_BUFFER           24      /*  */
#define CCB_CMD_TX_PTRS                 25      /*  */
#define CCB_CMD_RX_LIMITS               26      /*  */
#define CCB_CMD_RAW_QUEUE_PTR           27      /*  */

#define CCB_CMD_STATUS                  32      /* read and write controller status */
#  define CCB_STATUS_IF_ENABLE           0      /* enable the interface */
#  define CCB_STATUS_IF_DISABLE          1      /* disable the interface */
#  define CCB_STATUS_IF_LOOPLOCAL        2
#  define CCB_STATUS_IF_NOLOOP           3
#  define CCB_STATUS_IF_QUIESCE          4
#  define CCB_STATUS_IF_UNQUIESCE        5
#  define CCB_STATUS_IF_THROTTLE         6      /* throttle the interface */
#  define CCB_STATUS_IF_UNTHROTTLE       7
#  define CCB_STATUS_IF_LOOPREMOTE      10

#define CCB_CMD_STATION                 33      /* read Ethernet station address */
#define CCB_CMD_AF_SELECT               34      /* select address filter address*/
#define CCB_CMD_AF_READ                 35      /* read address filter mask */
#define CCB_CMD_AF_WRITE                36      /* write address filter mask */
#define CCB_CMD_CARRIER                 37      /* read current DCD status */
#define CCB_CMD_ERRORS                  38      /* read and reset error counter */

#define CCB_CMD_BUFFER_PARK		40	/* Tell IP to stoke a buffer */

#define CCB_CMD_APPLIQUE                42      /* write and read serial applique */
#define CCB_CMD_CLOCKRATE               43      /* bit rate for generating clock */
#define CCB_CMD_CLOCKTYPE               44      /* external vs generated clock */
#define CCB_CMD_DELAY                   45      /* transmitter delay counter */
#define CCB_CMD_FREE_QUEUE_PTRS         46      /*  */
#define CCB_CMD_GLOBAL_PTRS             47      /*  */
#define CCB_CMD_RXERROR                 47      /* RPSP love note queue pointer */
#define CCB_CMD_RXBYTES                 48      /* max bytes IP are allowed to
						   receive from the wire */
#define CCB_CMD_RX_OFFSET               49      /* offset from the beginning
						   of the buffers to start
						   writing the packets */

#define CCB_CMD_AF_SIZE                 50      /*  */

#define CCB_CMD_SERIAL_CRC_32           63      /* Used by FSIP and HIP */

/*----------------------------------------------------------------------------*
 * EIP commands
 *----------------------------------------------------------------------------*/
/* no special EIP commands exist */


/*----------------------------------------------------------------------------*
 * TRIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_TRIP_MEMB_SELECT        11      /* select MEMB address */
#define CCB_CMD_TRIP_MEMB_CONT_WRITE    12      /* contention write selected MEMB address */
#define CCB_CMD_TRIP_MEMB_CONT_READ     13      /* contention read selected MEMB address */
#define CCB_CMD_TRIP_SPYGLASS_SETUP     14      /*  */
#define CCB_CMD_TRIP_MEMB_WRITE         15      /* write selected MEMB address */
#define CCB_CMD_TRIP_MEMB_READ          16      /* read selected MEMB address */
#define CCB_CMD_TRIP_XILINX_ARG         20      /*  */
#define CCB_CMD_TRIP_XILINX_CMD         21      /*  */
#define CCB_CMD_TRIP_TX_DMA_TEST        22      /*  */
#define CCB_CMD_TRIP_RX_DMA_TEST        23      /*  */
#define CCB_CMD_TRIP_SPYGLASS_RESULT    28      /*  */
#define CCB_CMD_TRIP_POKE_CUETRC        29      /*  */
#define CCB_CMD_TRIP_GET_CHECK_PTR      30      /*  */
#define CCB_CMD_TRIP_READ_RING_STAT     31      /*  */
#define CCB_CMD_TRIP_AF_OR              39      /*  */
#define CCB_CMD_TRIP_BUFFER_PARK        40      /* Bufferin only??? */
#define CCB_CMD_TRIP_SET_MAC_HI         41      /*  */
#define CCB_CMD_TRIP_SET_MAC_MID        42      /*  */
#define CCB_CMD_TRIP_SET_MAC_LO         43      /*  */
#define CCB_CMD_TRIP_SPYGLASS_READ_VAL  44      /*  */
#define CCB_CMD_TRIP_SET_PH_BIT         51      /*  */
#define CCB_CMD_TRIP_RESET_PH_BIT       52      /*  */
#define CCB_CMD_TRIP_ACCEPT_MAC         54      /*  */
#define CCB_CMD_TRIP_BRIDGE_MODE        55      /*  */
#define CCB_CMD_TRIP_INIT               56      /*  */
#define CCB_CMD_TRIP_SPYGLASS_SELECT    57      /*  */
#define CCB_CMD_TRIP_SPYGLASS_WRITE     58      /*  */
#define CCB_CMD_TRIP_SPYGLASS_READ      59      /*  */


/*----------------------------------------------------------------------------*
 * FIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_FIP_HOLD_REG            11      /*  */
#define CCB_CMD_FIP_HEARSELF_MASK       12      /*  */
#define CCB_CMD_FIP_BURST_MODE          13      /*  */
#define CCB_CMD_FIP_PAGE_SELECT         14      /*  */
#define CCB_CMD_FIP_RETURN              15      /* XXX ?  */
#define CCB_CMD_FIP_CAM_CMD             28      /*  */
#define CCB_CMD_FIP_CAM_RD_ARG          29      /*  */
#define CCB_CMD_FIP_CAM_WR_ARG          30      /*  */
#define CCB_CMD_FIP_MEMD_HAMMER         31      /* XXX ?  */
#define CCB_CMD_FIP_FM_SELECT           39      /* select the formac */
#define CCB_CMD_FIP_FM_WRITE            40      /* write to the formac */
#define CCB_CMD_FIP_FM_READ             41      /* read the formac */
#define CCB_CMD_FIP_EN_SELECT           42      /* select the endec */
#define CCB_CMD_FIP_EN_WRITE            43      /* write the endec */
#define CCB_CMD_FIP_EN_READ             44      /* read the endec */
#define CCB_CMD_FIP_APPLIQUE_SELECT     CCB_CMD_FIP_EN_SELECT
#define CCB_CMD_FIP_APPLIQUE_WRITE      CCB_CMD_FIP_EN_WRITE
#define CCB_CMD_FIP_APPLIQUE_READ       CCB_CMD_FIP_EN_READ
#define CCB_CMD_FIP_CLAIM_BEACON        45      /* station management functions ? */
#define CCB_CMD_FIP_CMT_ARM_A           51      /*  */
#define CCB_CMD_FIP_CMT_ARM_B           52      /*  */
#define CCB_CMD_FIP_PHY_A_INT_MASK      CCB_CMD_FIP_CMT_ARM_A
#define CCB_CMD_FIP_PHY_B_INT_MASK      CCB_CMD_FIP_CMT_ARM_B
#define CCB_CMD_FIP_CMT_LEM_A           53      /*  */
#define CCB_CMD_FIP_CMT_LEM_B           54      /*  */
#define CCB_CMD_FIP_PHY_A_LEM           CCB_CMD_FIP_CMT_LEM_A
#define CCB_CMD_FIP_PHY_B_LEM           CCB_CMD_FIP_CMT_LEM_B
#define CCB_CMD_FIP_START_TRACE         55      /*  */
#define CCB_CMD_FIP_GET_STATE           56      /*  */
#define CCB_CMD_FIP_GET_RVAL            57      /*  */
#define CCB_CMD_FIP_GET_TVAL            58      /*  */
#define CCB_CMD_FIP_SET_TL_MIN          59      /*  */
#define CCB_CMD_FIP_KICK_PHY            60      /*  */
#define CCB_CMD_FIP_CMT_MODE            61      /*  */
#define CCB_CMD_FIP_GET_PY_LS           62      /*  */


/*----------------------------------------------------------------------------*
 * HIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_HIP_TA                  20      /*  */
#define CCB_CMD_HIP_ICLK                21      /*  */
#define CCB_CMD_HIP_LOOPBACK            39      /*  */
#define CCB_CMD_HIP_READ_LOOP           41      /*  */
#define CCB_CMD_HIP_XMIT                42      /*  */
#define CCB_CMD_HIP_CRC_HACK            44      /*  */
#define CCB_CMD_HIP_LC_INHIBIT          56      /*  */
#define CCB_CMD_HIP_SEND_BREAK          57      /*  */


/*----------------------------------------------------------------------------*
 * FSIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_FSIP_TX_INVERT_CLK      33      /*  */
#define CCB_CMD_FSIP_NRZI               34      /*  */
#define CCB_CMD_FSIP_HALFDUPLEX         35      /*  */
#define CCB_CMD_FSIP_DTECLOCK           36      /*  */
#define CCB_CMD_FSIP_SIP_ADDR           51      /*  */
#define CCB_CMD_FSIP_SIP_READ           52      /*  */
#define CCB_CMD_FSIP_SIP_WRITE          53      /*  */
#define CCB_CMD_FSIP_DOHC_ADDR          54      /*  */
#define CCB_CMD_FSIP_ZILOG_ADDR         57      /*  */
#define CCB_CMD_FSIP_ZILOG_READ         58      /*  */
#define CCB_CMD_FSIP_ZILOG_WRITE        59      /*  */
#define CCB_CMD_FSIP_SIGNAL_STS         61      /*  */
#define CCB_CMD_FSIP_DTE_CLOCKTICKS     62      /*  */
#define CCB_CMD_FSIP_CRC32              63      /*  */
#define CCB_CMD_FSIP_SETUP_G703         64      /*  */
#define CCB_CMD_FSIP_READ_G703          65      /*  */
#define CCB_CMD_FSIP_START_SLOT_G703    66      /*  */
#define CCB_CMD_FSIP_STOP_SLOT_G703     67      /*  */
#define CCB_CMD_FSIP_CRC4               68      /*  */
#define CCB_CMD_FSIP_TS16               69      /*  */
#define CCB_CMD_FSIP_CLOCKSOURCE        70      /*  */
#define CCB_CMD_FSIP_IGNORE_DCD         86

#define CCB_CMD_MAX_MTU_SUPPORTED       93      /* 0x5d */


/*----------------------------------------------------------------------------*
 * AIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_AIP_PROC_SELECT         66      /* Select the TX or RX processor */
#define CCB_CMD_AIP_ADDR_SELECT         67      /* Select AIP addr. */
#define CCB_CMD_AIP_MEM_WRITE           68      /* Write into AIP */
#define CCB_CMD_AIP_MEM_READ            69      /* Read AIP mem. */
#define CCB_CMD_AIP_CMDBLOCK            70      /* Get the AIP MEMA command addr. */
#define CCB_CMD_AIP_CMDPARSE            71      /* Parse the given AIP command */
#define CCB_CMD_AIP_FRAMING             72      /* AIP framing mode */
#define CCB_CMD_AIP_SONET               73      /* AIP SONET mode */
#define	CCB_CMD_AIP_DEBUG_WR		74	/* Write debug data to AIP mem */
#define	CCB_CMD_AIP_DEBUG_GETRX		75	/* Get Rx read mem data */
#define	CCB_CMD_AIP_DS3SCRAMBLE		76	/* AIP Scramble option */
#define	CCB_CMD_AIP_SETVER		77	/* H/W bug work-around */


/*----------------------------------------------------------------------------*
 *  MIP commands
 *----------------------------------------------------------------------------*/
#define CCB_CMD_MIP_VC_SELECT           64      /* select a virtual ciruit */
#define CCB_CMD_MIP_VC_STATUS           65      /* same params as CCB_CMD_MIP_STATUS*/
#define CCB_CMD_MIP_VC_ERRORS           66      /*  */
#define CCB_CMD_MIP_VC_MAP              67      /* map a circuit disabled */
#define CCB_CMD_MIP_RESET_APPLIQUE      68      /*  */
#define CCB_CMD_MIP_VC_UNMAP_ALL        69      /*  */
#define CCB_CMD_MIP_FRAME_TYPE          70      /*  */
#define CCB_CMD_MIP_VC_SUBRATE          71      /*  */
#define CCB_CMD_MIP_LINECODE_TYPE       72      /*  */
#define CCB_CMD_MIP_CHANNEL_STATUS      73      /* status command for the full T1 */
#define CCB_CMD_MIP_MIPCARRIER          74      /*  */
#define CCB_CMD_MIP_CABLE_LENGTH        75      /*  */
#define CCB_CMD_MIP_VC_ENCAP            76      /* the encapsulation byte we want */
#define CCB_CMD_MIP_VC_MAPENABLE        77      /* map and enable a circuit */
#define CCB_CMD_MIP_VC_MTU              78      /*  */
#define CCB_CMD_MIP_VC_TX_LIMIT         79      /*  */
#define CCB_CMD_MIP_CLR_COUNTS          80      /* clear onboard counters */
#define CCB_CMD_NO_LOVELETTERS  	81      /* stop sending love letters */
#define CCB_CMD_OK_LOVELETTERS  	82      /* ok to send love letters */
#define CCB_CMD_ALT_IDLE        	83      /* ok to use alt idle pattern */
#define CCB_CMD_TXQLENGTH               84
#define CCB_CMD_SHORT_TXQ_LOCALBUF      85      /* shorten tx local
						   buffer on an interface */


/*-------------------------------------------------------------------------
 * CIP specific cbus commands 
 *--------------------------------------------------------------------------
 */

#define CCB_CMD_CIP_DAUGHTER_TYPE       66   /* Is it PCA or ECA */
#define CCB_CMD_CIP_SET_PCA_BW          67   /* Set PCA data rate */
#define CCB_CMD_CIP_ADDR_SELECT         70   /* Select an address */
#define CCB_CMD_CIP_MEM_WRITE           71   /* Write data */
#  define FCI_CMD_CIP_MEM_READ          72   /* Read data */
#define CCB_CMD_CIP_CFGBLOCK            73   /* get address of cfg block */
#define CCB_CMD_CIP_CFGPARSE            74   /* tell CIP to parse cfg block */
#  define FCI_CMD_CIP_DIAGNOSTIC        75   /* run diagnostics */
#define CCB_CMD_CIP_STATS_INTERVAL      76   /* set stats interval */
#define CCB_CMD_CIP_PLD_VERSION          2   /* get pld version */
#define CCB_CMD_CIP_ROM_VERSION          3   /* get rom version */
#define CCB_CMD_CIP_CTRLR_VERSION        0   /* get rom version */
#define CCB_CMD_NO_LOVELETTERS  	81   /* stop sending love letters */
#define CCB_CMD_OK_LOVELETTERS  	82   /* ok to send love letters */
#define CCB_CMD_ALT_IDLE        	83   /* ok to use alt idle pattern */

/*--------------------------------------------------------------------------
 * FEIP specific cbus commands
 *--------------------------------------------------------------------------
 */
#define CCB_CMD_FEIP_SPECIFIC           61
# define CCB_CMD_FEIP_LOOP_NONE          0   /* Loopback modes */
# define CCB_CMD_FEIP_LOOP_MOTOROLA      1
# define CCB_CMD_FEIP_LOOP_DEC           2
# define CCB_CMD_FEIP_LOOP_MICRO_LINEAR  3
# define CCB_CMD_FEIP_RJ45               4   /* media selection */
# define CCB_CMD_FEIP_MII                5
# define CCB_CMD_FEIP_FULL_DUPLEX        6   /* duplex mode */
# define CCB_CMD_FEIP_HALF_DUPLEX        7


/*
 * POSIP commands...why are these replicated in if_fci.h????
 */

# define CCB_CMD_POSIP_SPECIFIC               62

# define CCB_POSIP_LOOP_NONE                   0    /* Loopback modes */
# define CCB_POSIP_LOOP_INTERNAL               1
# define CCB_POSIP_LOOP_EXTERNAL               2
# define CCB_POSIP_CLOCK_EXTERNAL              3
# define CCB_POSIP_CLOCK_INTERNAL              4
# define CCB_POSIP_SDH_ON                      5
# define CCB_POSIP_SDH_OFF                     6
# define CCB_POSIP_GET_MEM_SIZE                7

/*--------------------------------------------------------------------------*
 * VIP 4R specific cbus commands
 *--------------------------------------------------------------------------*/
#define CCB_CMD_VIP_4R_TEST              3
#define CCB_CMD_VIP_4R_START_BLOCK      11
#define CCB_CMD_VIP_4R_WRITE_BLOCK      12
#define CCB_CMD_VIP_4R_END_BLOCK        13
#define CCB_CMD_VIP_4R_REQUEST_STATS    14

/*--------------------------------------------------------------------------*
 * VIP 1F specific cbus commands
 *--------------------------------------------------------------------------*/
#define CCB_CMD_VIP_1F_BRIDGE            100
#define CCB_CMD_VIP_1F_TVX               101
#define CCB_CMD_VIP_1F_TRT               102
#define CCB_CMD_VIP_1F_STN_ADD           103
#define CCB_CMD_VIP_1F_INTERESTING_ADD   104
#define CCB_CMD_VIP_1F_ALL_MULTICAST_ADD 105
#define CCB_CMD_VIP_1F_GET_MIB_INFO      106
#define CCB_CMD_VIP_1F_SRB               107
#define CCB_CMD_VIP_CAM_POPULATE         125
#define CCB_CMD_VIP_CAM_DEPOPULATE       126

/*--------------------------------------------------------------------------*
 * VIP serial specific cbus commands
 *--------------------------------------------------------------------------*/
#define CCB_CMD_SERIAL_ENCAP             87

/*--------------------------------------------------------------------------*
 * VIP specific cbus commands
 *--------------------------------------------------------------------------*/
#define CCB_CMD_VIP_GET_CONFIG           120

/*--------------------------------------------------------------------------
 * Malu Attenton command resync commands
 *--------------------------------------------------------------------------
 */
#define CCB_CMD_SYNC_COMMAND	       128   /* resync cbus commands */

/*--------------------------------------------------------------------------
 * IPC mechanism cbus commands
 *--------------------------------------------------------------------------
 */
#define CCB_CMD_IPC_QUEUE	       129   /* Init IPC queue (RSP only) */
#define CCB_CMD_IPC_INIT	       130   /* Init IPC zone, seat */

#define CCB_CMD_MAX_COMMANDS           131  /* (0 - 130) PLEASE UPDATE! */
#endif  /* __CCB_CMD_H__ */


