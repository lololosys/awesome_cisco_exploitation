/* $Id: if_fci.h,v 3.1 1995/11/09 09:05:45 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/if_fci.h,v $
 *------------------------------------------------------------------
 * if_fci.h -- FAST Communications Interface Hardware Definitions.
 *
 * 1-August-1989, Robert Michaels
 *
 * Copyright (c) 1989-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_fci.h,v $
 * Revision 3.1  1995/11/09  09:05:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:42  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define FCISTART (ADRSPC_MULTIIO+0xC000)
#define FCI_INTLEVEL                    3
#define FCI_MAX_CTRLR_NUMBER            15
#define FCI_MAX_DCI_CTRLRS              4       /* 4 daughter boards */
#define FCI_MAX_INTERFACES              24      /* max number of interfaces */
#define FCI_MAX_CTRLR_INTERFACES        (FCI_MAX_INTERFACES / \
                                         FCI_MAX_DCI_CTRLRS)

/*
 * Controller type(s) supported by the Fast Communications Interface.
 */
#define FCI_ETHER_CONTROLLER    0
#define FCI_FDDI_CONTROLLER     1
#define FCI_SERIAL_CONTROLLER   2
#define FCI_DS3_CONTROLLER      3


#define FDDI_RECEIVE_OFFSET             12
#define FDDI_RECEIVE_WORD_OFFSET        (FDDI_RECEIVE_OFFSET & 0XFFFE)
#define FDDI_RECEIVE_BYTE_OFFSET        ((FDDI_RECEIVE_OFFSET * 2) + 1)

#define FDDI_FRAME_SIZE         4500
#define FDDI_PREAMBLE           2
#define FDDI_SD                 1
#define FDDI_FC                 1
#define FDDI_DA                 6
#define FDDI_SA                 6    
#define FDDI_FCS                4
#define FDDI_EFS                2
#define FDDI_INFO_SIZE          (FDDI_FRAME_SIZE  - (FDDI_PREAMBLE + \
                                                    FDDI_SD +   \
                                                    FDDI_FC +   \
                                                    FDDI_DA +   \
                                                    FDDI_SA +   \
                                                    FDDI_FCS +  \
                                                    FDDI_EFS))  

#define FDDI_BUFFERSIZE_BYTES   (FDDI_INFO_SIZE + (FDDI_FC + \
                                                   FDDI_DA + \
                                                   FDDI_SA + \
                                                   FDDI_FCS + \
                                                   1))

#define FDDI_BUFFERSIZE_WORDS   ((((FDDI_BUFFERSIZE_BYTES + 1) - \
                                 ((MCI_OFFSETSIZE * 2) - \
                                  FDDI_RECEIVE_BYTE_OFFSET)) / 2) + 1) 

#define FDDI_MAXDGRAM           (FDDI_FRAME_SIZE - (FDDI_PREAMBLE + \
                                                    FDDI_SD + \
                                                    FDDI_FCS + \
                                                    FDDI_EFS))

#define FDDI_MINDGRAM           16


/*
 * Buffer pools in the FCI.
 */

#define FCI_BUF_POOL_0          0
#define FCI_BUF_POOL_1          1
#define FCI_BUF_POOL_2          2
#define FCI_BUF_POOL_3          3
#define FCI_BUF_POOL_4          4
#define FCI_BUF_POOL_5          5
#define FCI_BUF_POOL_6          6
#define FCI_BUF_POOL_7          7

#define FCI_RX_ERROR_POOL               FCI_BUF_POOL_7
#define FDDI_CLAIM_BEACON_POOL          FCI_BUF_POOL_6
#define FDDI_BUFFER_POOL                FCI_BUF_POOL_1
#define ETHER_BUFFER_POOL               FCI_BUF_POOL_0
#define SERIAL_BUFFER_POOL              FCI_BUF_POOL_0    
#define SYSTEM_BUFFER_POOL              FCI_BUF_POOL_2
/*
 * FCI Global command codes
 */
#define FCI_CMD_VERSION         0       /* read version */
#define FCI_CMD_RESET           1       /* reset controller */
# define FCI_HARD_RESET         0       /* argument to FCI_CMD_RESET */
# define FCI_SOFT_RESET         1       /* argument to FCI_CMD_RESET */
#define FCI_CMD_MEMSIZE         2       /* read memory size in words */
#define FCI_CMD_SELECT          3       /* select an interface */
#define FCI_CMD_MEMD_SELECT     4       /* select memd address */
#define FCI_CMD_RX_SELECT       5       /* select offset into RX */
#define FCI_CMD_TX0_SELECT      6       /* select offset into transmit 0 */
#define FCI_CMD_TX1_SELECT      7       /* select offset into transmit 1 */
#define FCI_CMD_RX_SETUP        8       /* setup RX window */
#define FCI_CMD_ASR             9       /* attention summary bit mask */
#define FCI_CMD_CONDITIONS      10      /* error, DCD, and TX done summary */
#define FCI_CMD_RX_MOVE         11      /* move RX buffer to TX1 window */
#define FCI_CMD_INTERRUPT       12      /* define interrupt level */
#define FCI_CMD_RX_FLUSH        13      /* move or flush current RX buffer */
#define FCI_CMD_TX0_RESERVE     14      /* reserve a buffer in transmit 0 */
#define FCI_CMD_TX1_RESERVE     15      /* reserve a buffer in transmit 1 */
#define FCI_CMD_RX_CHECK        16      /* validate special RX packet */
#define FCI_CMD_CAPABILITY      17      /* read controller capabilities */
#define FCI_CMD_MEMA_READ       18      /* read word from mema */
#define FCI_CMD_MEMA_WRITE      19      /* write word to mema */
#define FCI_CMD_MEMA_SELECT     20      /* select mema address */
#define FCI_CMD_OVERHEAD        21      /* read and write buffer overhead */
#define FCI_CMD_ARGUMENT        22      /* set buffer size for TX or RX */
# define FCI_CMD_BUFFERSIZE     22      
#define FCI_CMD_TX_ALLOCATE     23      /* allocate a transmit buffer */
#define FCI_CMD_LIGHTS          24      /* write LED's */
#define FCI_CMD_BRIDGE          25      /* FSA bit mask for bridging */
#define FCI_CMD_BRIDGEPROTOCOL  26      /* protocol specific bridging ctl */
#define FCI_CMD_MEMX_SELECT_HIGH 27     /* high 16 bits of memx address */
#define FCI_CMD_MEMX_SELECT_LOW 28      /* low 16 bits of memx address */
#define FCI_CMD_READ_REGISTER   29      /* read cBus register */
# define FCI_CCBPTR             0
# define FCI_ICBPTR             1
/*
 * Interface specific command codes. Valid after a FCI_CMD_SELECT, 
 * FCI_CMD_CONDITIONS or FCI_CMD_RX_MOVE.
 */
#define FCI_CMD_STATUS          32      /* read and write controller status */
#define FCI_CMD_STATION         33      /* read Ethernet station address */
#define FCI_CMD_AF_SELECT       34      /* select address filter address*/
#define FCI_CMD_AF_READ         35      /* read address filter mask */
#define FCI_CMD_AF_WRITE        36      /* write address filter mask */
#define FCI_CMD_CARRIER         37      /* read current DCD status */
#define FCI_CMD_ERRORS          38      /* read and reset error counter */
#define FCI_CMD_TX0_START       39      /* start transmitting TX0 */
#define FCI_CMD_TX1_START       40      /* start transmitting TX1 */
#define FCI_CMD_EXECUTE         41      /* execute the argreg CMD */
# define FCI_ASSIGN_POOLINDEX   0
# define FCI_MAX_BUFFERSIZE     1
# define FCI_RX_OFFSET          2
# define FCI_RX_BUFFER_LIMIT    3
# define FCI_RX_ERROR           4
#define FCI_CMD_APPLIQUE        42      /* select applique register */
#define FCI_CMD_APP_WRITE       43      /* write applique register */
#define FCI_CMD_APP_READ        44      /* read applique register */
#define FCI_CMD_DELAY           45      /* transmitter delay counter */
#define FCI_CMD_QUEUE           46      /* maximum output queue length */
#define FCI_CMD_SLIMIT          47      /* min. queue length when switching */
#define FDDI_CMD_FM_SELECT      48      /* select the formac */
#define FDDI_CMD_FM_WRITE       49      /* write to the formac */
#define FDDI_CMD_FM_READ        50      /* read the formac */
#define FDDI_CMD_EN_SELECT      51      /* select the endec */
#define FDDI_CMD_EN_WRITE       52      /* write the endec */
#define FDDI_CMD_EN_READ        53      /* read the endec */
#define FDDI_CMD_CLAIM_BEACON   54      /* station management functions ? */
#define FCI_CMD_AF_SIZE         55      /* get size of address filter */
#define FCI_CMD_TX0_ENQUEUE     56      /* enqueue -- don't start */

#define FDDI_CMD_APPLIQUE_SELECT   FDDI_CMD_EN_SELECT   /* select applique */
#define FDDI_CMD_APPLIQUE_WRITE    FDDI_CMD_EN_WRITE    /* write applique */
#define FDDI_CMD_APPLIQUE_READ     FDDI_CMD_EN_READ     /* read appilque */

/*
 * Controller commands.
 */
#define FCI_CMD_CTRLR_SELECT            0x8000
#define FCI_CTRLR_CMD                   0x8002
#define FCI_CTRLR_ARGUMENT              0x8003
#define FCI_CTRLR_WHO                   0x8004
#define FCI_CTRLR_LOAD                  0x8005
#define FCI_CTRLR_RESET                 0x8006

#define FCI_FDDI_VERSION                0
#define FCI_FDDI_RESET                  1
#define FCI_FDDI_SELECT_INT             2

#define FCI_FDDI_REG_SELECT             4
#define FCI_FDDI_REG_WRITE              5    
#define FCI_FDDI_REG_READ               6    
#define FCI_CTRLR_TX_START              7
#define FCI_CTRLR_MEMD_SELECT           8
#define FCI_CTRLR_MEMD_WRITE            9
#define FCI_CTRLR_MEMD_READ             10
#define FCI_FDDI_HOLD_REG               11
# define FDDI_FULL_DUPLEX               (1 << 4)
#define FCI_FDDI_HEARSELF               12
#define FCI_FDDI_PHY_A_INT_MASK         51
#define FCI_FDDI_PHY_B_INT_MASK         52

#define FCI_HSCI_BUFFER_PARK            40
#define FCI_HSCI_CRC_HACK               44

/*
 * Counter registers in the FDDI -- used by SMT get get the frame count.
 */
#define FDDI_REG_FRINC                  063

/*
 * Registers available in the FORMAC.
 */
#define FDDI_FM_REG_0           0       
#define FDDI_FM_REG_1           1
#define FDDI_FM_REG_2           2
#define FDDI_FM_REG_3           3
#define FDDI_FM_REG_4           4
#define FDDI_FM_REG_5           5
#define FDDI_FM_REG_6           6
#define FDDI_FM_REG_7           7
#define FDDI_FM_REG_8           8
#define FDDI_FM_REG_9           9
#define FDDI_FM_REG_10          10
#define FDDI_FM_REG_11          11
#define FDDI_FM_REG_12          12
#define FDDI_FM_REG_13          13
#define FDDI_FM_REG_14          14
#define FDDI_FM_REG_15          15

/*
 * Write only FORMAC registers i.e. FDDI_FM_CMD_WRITE
 */
#define FDDI_FM_W_RESET         FDDI_FM_REG_0
#define FDDI_FM_W_IDLE_LISTEN   FDDI_FM_REG_1
#define FDDI_FM_W_CLAIM_LISTEN  FDDI_FM_REG_2
#define FDDI_FM_W_BEACON_LISTEN FDDI_FM_REG_3
#define FDDI_FM_W_RESTRICT_CTRL FDDI_FM_REG_7
#define FDDI_FM_W_SEND_NOW      FDDI_FM_REG_9
#define FDDI_FM_W_RESET_ADDRPTR FDDI_FM_REG_12

/*
 * Read only FORMAC registers i.e. FDDI_FM_CMD_READ
 */
#define FDDI_FM_R_LSBS          FDDI_FM_REG_0
#define FDDI_FM_R_TNEG          FDDI_FM_REG_1
#define FDDI_FM_R_MIR1          FDDI_FM_REG_2
#define FDDI_FM_R_MIR0          FDDI_FM_REG_3
#define FDDI_FM_R_STATES        FDDI_FM_REG_9
#define FDDI_FM_R_STATUS        FDDI_FM_REG_12
#define FDDI_FM_R_TVX_TIMER     FDDI_FM_REG_14

/*
 * Read/write address registers i.e FDDI_FM_CMD_{READ WRITE}
 */

#define FDDI_FM_RW_THT          FDDI_FM_REG_4
#define FDDI_FM_RW_TRT          FDDI_FM_REG_5
#define FDDI_FM_RW_TMAX         FDDI_FM_REG_6
#define FDDI_FM_RW_TVX          FDDI_FM_REG_8
#define FDDI_FM_RW_MODE_REG     FDDI_FM_REG_10
# define FM_MODE_ON_LINE                0x6000
# define FM_MODE_OFF_LINE               0x0000
# define FM_MODE_DA_EQ_MA               0x0000
# define FM_MODE_DA_AND_SA_EQ_MA        0x0100  
# define FM_MODE_PROMISCUOUS            0x0200
# define FM_MODE_RB_BUS                 0x0000    
# define FM_MODE_RA_BUS                 0x0020
#define FDDI_FM_RW_INT_MASK     FDDI_FM_REG_11
#define FDDI_FM_RW_ADDR_REG     FDDI_FM_REG_13
#define FDDI_FM_RW_TPRI         FDDI_FM_REG_15

/* 
 * Registers available in the ENDEC 
 */
#define ENDECA                  0
#define ENDECB                  1

#define ENDEC_CMD               1
#define ENDEC_DATA              0

#define BYPASS_SWITCH           2
#define APPLIQUE_BYPASS         3
#define BYPASS_BYPASS           FALSE
#define APPLIQUE_CONNECT        7
#define BYPASS_CONNECT          TRUE

#define FDDI_EN_REG_0           0
#define FDDI_EN_REG_1           1
#define FDDI_EN_REG_2           2
#define FDDI_EN_REG_3           3
#define FDDI_EN_REG_4           4

#define FDDI_EN_RW_CR0          FDDI_EN_REG_0
# define ENDEC_FORCE_QUIET              0x00
# define ENDEC_FORCE_MASTER             0x01
# define ENDEC_FORCE_HALT               0X02
# define ENDEC_FORCE_IDLE               0X03
# define ENDEC_ENABLE_REPEAT_FILTER     0x04
# define ENDEC_FORCE_JKILS              0x05
# define ENDEC_FORCE_IQ                 0x06
# define ENDEC_UNFILTERED_ENCODE        0x07
#define FDDI_EN_RW_CR1          FDDI_EN_REG_1
# define ENDEC_TB_BUS                   0x00
# define ENDEC_TA_BUS                   0x04
# define ENDEC_THROUGH                  0X00
# define ENDEC_LOOPBACK                 0x01
# define ENDEC_SHORT_LOOPBACK           0x02
# define ENDEC_REPEAT                   0x03
#define FDDI_EN_RW_CR2          FDDI_EN_REG_2
# define ENDEC_PARCON                   0x01
# define ENDEC_FORCE_ODD                0x02
# define ENDEC_RESET                    0x04
/* #define FDDI_EN_R_STATUS     FDDI_EN_REG_3 */
#define FDDI_EN_RW_CR3          FDDI_EN_REG_4
# define ENDEC_SMRESET                  0x01
# define ENDEC_EXTENSION1               0x02
# define ENDEC_EXTENSION2               0x04

/*
 * Response codes returned by reading command register
 */
#define FCI_RSP_OKAY           0        /* successful operation */
#define FCI_RSP_ERR_UNK   0x8000        /* unknown error */
#define FCI_RSP_ERR_ILR   0x8001        /* illegal register read */
#define FCI_RSP_ERR_ILW   0x8002        /* illegal register write */
#define FCI_RSP_ERR_UNIT  0x8003        /* impossible unit number */
#define FCI_RSP_ERR_NPRS  0x8004        /* unit not present */
#define FCI_RSP_ERR_FNV   0x8005        /* function not valid */
#define FCI_RSP_ERR_IVO   0x8006        /* invalid offset */
#define FCI_RSP_ERR_NBF   0x8007        /* no available buffers */
#define FCI_RSP_ERR_CMD   0x8008        /* illegal command code */
#define FCI_RSP_ERR_MEM   0x8009        /* no more memory */
#define FCI_RSP_ERR_VLD   0x800A        /* RX_CHECK failed */
#define FCI_RSP_ERR_CHK   0x800B        /* RX_CHECK failed, bad checksum */
#define FCI_RSP_ERR_TTL   0x800C        /* RX_CHECK failed, TTL exceeded */
#define FCI_RSP_ERR_BFR   0x800D        /* buffer already in window */
#define FCI_RSP_ERR_QFL   0x800E        /* output queue full */
#define FCI_RSP_ERR_NCP   0x800F        /* no capability present */

/*
 * Arguments for FCI_CMD_VERSION
 */
#define FCI_SW_VERSION          0       /* microcode version */
#define FCI_HW_VERSION          1       /* hardware type code */

/*
 * Hardware version on FCI board -- response to FCI_HW_VERSION/FCI_CMD_VERSION.
 */
#define MCI_HARDWARE            1       /* returned from MCI */
#define SCI_HARDWARE            2       /* returned from SCI */
#define FCI_HARDWARE            3       /* returned from FCI */
#define FCI2_HARDWARE           6       /* returned from FCI2 */

/*
 * Arguments for FCI_CMD_MEMSIZE
 */
#define FCI_MEMSIZE_MEMA        0       /* cache memory */
#define FCI_MEMSIZE_MEMD        1       /* main buffer memory */

/*
 * Argument for FCI_CMD_STATUS
 */
#define FCI_IF_THROTTLE         6

/*
 * Types of interfaces supported by cBus controller.
 */
#define FCI_TYPE_ETHER          0
#define FCI_TYPE_SERIAL         1
#define FCI_TYPE_FDDI           2
#define FCI_TYPE_ULTRA          3
#define FCI_TYPE_DS3            4

/*
 * Definitions for word returned by FCI_CMD_CONDITIONS 
 */
#define FCI_COND_ERROR  0x8000          /* error counters need reading */
#define FCI_COND_DCD    0x4000          /* carrier detect changed state */
#define FCI_COND_PHY_A  0x1000          /* status change on phy_a */
#define FCI_COND_PHY_B  0x2000          /* status change on phy_b */
#define FCI_COND_MASK   0x00FF          /* count of TX buffers posted done */

/*
 * Argument to the FCI_CMD_ERRORS command.
 */
#define FCI_ERR_GIANTS          0
#define FCI_ERR_NOBUFFERS       1
#define FCI_ERR_ACQUIRE         2
#define FCI_ERR_COLLISIONS      3
#define FCI_ERR_RX_OVERRUN      4
#define FCI_ERR_CRC             5
#define FCI_ERR_FRAME           6
#define FCI_ERR_RUNT            7
#define FCI_ERR_TX_UNDERFLOW    8

/*
 * MEC address filter constants -- really bits within an octet.
 */
#define FCI_STATION_ADDRESS     0
#define FCI_MULTICAST_ADDRESS   1
#define FCI_BRIDGING_ADDRESS    2
#define FCI_UNUSED_ADDRESS      3
#define FCI_DOD_IP_PROTOCOL     4
#define FCI_DECNET_PROTOCOL     5
#define FCI_XNS_PROTOCOL        6
#define FCI_CLNS_PROTOCOL       7

#define FCI_STATION_MASK                (1 << FCI_STATION_ADDRESS)
#define FCI_MULTICAST_MASK              (1 << FCI_MULTICAST_ADDRESS)
#define FCI_BRIDGING_MASK               (1 << FCI_BRIDGING_ADDRESS)
#define FCI_UNUSED_MASK                 (1 << FCI_UNUSED_ADDRESS)
#define FCI_DOD_IP_PROTOCOL_MASK        (1 << FCI_DOD_IP_PROTOCOL)
#define FCI_DECNET_PROTOCOL_MASK        (1 << FCI_DECNET_PROTOCOL)
#define FCI_XNS_PROTOCOL_MASK           (1 << FCI_XNS_PROTOCOL)
#define FCI_CLNS_PROTOCOL_MASK          (1 << FCI_CLNS_PROTOCOL)


