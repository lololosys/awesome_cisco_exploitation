/* $Id: if_c4000_ce1.h,v 3.3.62.2 1996/09/05 17:57:31 wmay Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_ce1.h,v $
 *------------------------------------------------------------------
 * if_c4000_ce1.h - C4XXX E1 framer defines. 
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_ce1.h,v $
 * Revision 3.3.62.2  1996/09/05  17:57:31  wmay
 * CSCdi68168:  ce1 firmware data is in data segment - move to text
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/06/16  21:15:23  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3  1995/11/17  17:39:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/07  22:58:22  wmay
 * CSCdi36810:  CE1 nim does not pass Austalian Homologation
 *
 * Revision 2.3  1995/06/09  17:43:50  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.2  1995/06/08  22:05:42  jliu
 * CSCdi35359:  Rename files for new hardware changes
 *
 * Revision 2.1  1995/06/07  21:34:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* The Channelized E1 hardware in Spaten project is different from
 * T1 hardware by adding 8-bit 68000 microprocessor dedicating for
 * physical layer process. So there is mailbox type protocol used
 * between main CPU and NIM processor(68000). The MPU used in definitions
 * below means main CPU. 
 */

/*
 * Offset of the external registers on CORE board for E1 controller
 */
#define CE1_LED_OFFSET     0x000e  /* offset to E1 external LED register */

#define CE1_NP_CONTROL     0x0080  /* offset to NIM processor control reg */
/* bit definition for NIM processor control regsiter */
#define  CE1_NP_RESET      0x01    /* bring NIM processor to reset state */
#define  CE1_MPU_INT       0x04    /* interrupt pending from NIM processor */
#define  CE1_NP_INT        0x08    /* cause an interrupt to NIM processor */
#define  CE1_NP_HALT       0x10    /* NIM processor is in halt condition */
#define  CE1_NP_INT_ENABLE 0x20    /* force MPU interrupt will be enabled */

#define CE1_PAGE_OFFSET    0x0082  /* offset to external page register */
#define CE1_INDEX_OFFSET   0x0084  /* offset to external index register */
#define CE1_DATA_OFFSET    0x0086  /* offset to external data register */

#define CE1_MAIL_FROM_NP   0x7ff0  /* NIM processor's ram address contains */
                                  /* mail from NIM processor */
/* bit definition for mail from NIM processor */
#define  MSG_TYPE_NONE           0x00
#define  MSG_TYPE_CARRIER        0x01
#define  MSG_TYPE_FULL_MESSAGE   0x02

#define CE1_CARRIER_AVAIL  0x7ff1  /* NIM processor's ram address contains */
                                  /* carrier available byte */
#define CE1_BOARD_STATUS   0x7ff2  /* NIM processor's ram address contains */
                                  /* E1 line status */

#define CE1_OTHER_STATUS   0x7ff3  /* NIM processor's ram address contains */
                                  /* some MIB error  information */
/* bit definition for MIB error information from NIM processor */
#define  MSG_ES_OCCURRED         0x01
#define  MSG_BES_OCCURRED        0x02
#define  MSG_SES_OCCURRED        0x04
#define  MSG_CSS_OCCURRED        0x08
#define  MSG_FR_LOSS_OCCURRED    0x10
#define  MSG_LES_OCCURRED        0x20

#define BPV_COUNT          0x7ff4  /* BPV error counts */
#define PCV_COUNT          0x7ff6  /* PCV error counts */
#define CRC_COUNT          0x7ff8  /* CRC error counts */
#define FBE_COUNT          0x7ffa  /* FBE error counts */

#define CE1_MAIL_TO_NP     0x7fff  /* NIM processor's ram address contains */
                                   /* mail to NIM processor */
/* bit definition for mail sent to NIM processor from MPU */
#define  MPU_MSG_DISABLED	0x01
#define  MPU_MSG_FRAME		0x02
#define  MPU_MSG_LINECODE	0x04
#define  MPU_MSG_LOOPBACK	0x08
#define  MPU_MSG_SEND_RAI	0x10
#define  MPU_MSG_HOMOL_TYPE	0x20


#define  CE1_CABLE_TYPE    0x8201  /* cable impedance, 1 for unbalanced(75 ohm) */
                                   /* 0 for balanced(120 ohm) */
#define  UNBALANCE         0x02

#define  MAX_UPDATE_SIZE   0x10    /* buffer size to store full message from */
                                   /* Nymph processor */

struct mpu_instance {
    volatile short *np_control;    /*point to nim processor control register */
    volatile short *page_ram;      /*point to the local ram  page register*/
    volatile short *index_ram;     /*point to the local ram index register*/
    volatile short *data_ram;      /*point to the local ram data register*/
    volatile short *ext_led;       /*point to the LED display*/

    uchar    carrier_status;       /*TRUE for carrier state UP*/
    uchar    alive_count;          /*10 secs count to detect Nymph processor*/
                                   /*is still alive or not?*/
};

extern const int ce1_firmware_bytes;
extern const unsigned char ce1_firmware[];

/*
 * Prototypes
 */
void ce1_analyze_mcn_port_adaptor(int, int, int);
void ce1_txAIS(uint, boolean);
void ce1_linecode(uint, short);
void ce1_local_up(uint);
void ce1_no_loop(uint);
void ce1_framing(uint, short);
void ce1_homologation_type(uint, enum dsx1_homologation_type);
void ce1_interrupt(dsx1_instance_t *);
void ce1_sec_update(dsx1_instance_t *, paktype *);
void ce1_carrier_update(dsx1_instance_t *);
