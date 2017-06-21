/* $Id: if_c4000_m32.h,v 3.4.28.3 1996/09/07 22:37:14 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_m32.h,v $
 *------------------------------------------------------------------
 * if_c4000_m32.h
 *
 * 13 December 1993, Michael Hubenthal
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Header file for the Munich32 HDLC controller. This file contains
 * chip specific structures and definitons for this controller.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_m32.h,v $
 * Revision 3.4.28.3  1996/09/07  22:37:14  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.4.28.2  1996/06/16  21:15:39  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.28.1  1996/05/23  23:46:38  hrobison
 * CSCdi50628:  BRI interface stops sending frames on B or D channels on
 * 4x00
 * Branch: California_branch
 * Under unknown circumstances, a 4x00 BRI can stop transmitting.  Appears
 * to be related to CSCdi37200, so choose the lesser of two evils, either
 * have it hang, or have it require a ton of memory just to change the
 * speed of the interface.  Back out CSCdi37200 until we can figure out
 * what is happening.
 *
 * Revision 3.4  1996/01/19  23:09:48  hrobison
 * CSCdi37200:  MBRI requires significant memory to set b channel speed
 * The MBRI driver no longer frees and reallocates ring buffers and
 * descriptors on every dialer call that changes bandwidth.  Set the
 * timeslot bitrate in the M32 on every call.
 *
 * Fix problem (software workaround for probable HW bug) with the M32
 * chip where the device RX descriptor pointer either does not increment
 * or jumps to the start of the ring with an RX interrupt
 *
 * Revision 3.3  1995/11/17  17:39:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  07:18:39  jchlin
 * CSCdi35650 (transmit delay bug) and CSCdi35651 (remote loopback bug)
 * are fixed in this commit. Now, the hdlc flags will be used to
 * implement the transmit delay. The interface remote loopback will
 * send out the DDS CSU loopback patterns to activate and deactivate
 * CSU remote loopback.
 * CSCdi35650:  The interface transmit delay for T1/E1/PRI doesnt work
 *
 * Revision 2.2  1995/06/20  20:28:01  jchlin
 * Fix the channel throttling routines so that the rx channel
 * won't get stucked after throttling.
 * Fix the rx channel descriptor ring handling so that rx
 * channel won't get stucked after the ring is full and
 * gets processed during the single interrupt.
 * CSCdi34370:  4000 crashed w/BADMAGIC during stress test
 *
 * Revision 2.1  1995/06/07  21:34:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prevent multiple inculsion.
 */
#ifndef __IF_C4000_M32_H__
#define __IF_C4000_M32_H__

/*
 * ----------------------------------------
 * definitions, masks
 * ----------------------------------------
 */

/*
 * general Munich32 definitions.
 */
#define M32_NUM_CHANNELS 32
#define M32_NUM_TIMESLOTS 32
#define M32_MAX_FRAME_SIZE 8191 
#define M32_LONG_WORD_ALIGN 4
#define M32_IDLE_CHNL_UNASSIGNED M32_NUM_CHANNELS

/*
 * states that a munich32 channel can be in
 */
enum m32_channel_state_t {
    M32_CHANNEL_ILLEGAL = 0,  /* illegal state meant to catch if the */
                              /* state gets mistakenly set to zero */
    M32_CHANNEL_UNUSED,       /* never been initialized */
    M32_CHANNEL_IDLE,         /* idle channel, all unused timeslots are */
                              /* assigned to this channel */
    M32_CHANNEL_DOWN,         /* initialized but rx/tx off */
                              /* no buffers assigned */
    M32_CHANNEL_STANDBY,      /* rx/tx abort done, buffers assigned */
                              /* requires only clear to bring channel up */
    M32_CHANNEL_UP,           /* channel can transmit and receive */
    M32_CHANNEL_THROTTLED     /* channel being throttled */
};

/*
 * value used by action request code to mean 
 * configure all channels.
 */
#define M32_AR_ALL_CHANNELS (M32_NUM_CHANNELS + 1)
/*
 * max times to spin wait waiting for an action
 * request to finish while polling.
 */
#define M32_AR_SPIN_WAIT_ITERATIONS 5000
#define M32_AR_SPIN_WAIT_USEC 100
/*
 * values for the action specification long word
 */
#define M32_AR_PCM_T1    0x80000000
#define M32_AR_PCM_E1    0xa0000000
#define M32_AR_IN            0x8000
#define M32_AR_ICO           0x4000
#define M32_AR_RES             0x40
#define M32_AR_EXTERNAL_LINE_LOOP         0x10
#define M32_AR_EXTERNAL_CHANNEL_LOOP      0x30
#define M32_AR_INTERNAL_LINE_LOOP         0x08
#define M32_AR_INTERNAL_CHANNEL_LOOP      0x28
#define M32_AR_IA               0x4
#define M32_AR_MFL_SHIFT 16
#define M32_AR_MFL (M32_MAX_FRAME_SIZE << M32_AR_MFL_SHIFT)
#define M32_AR_CHANNEL_SHIFT 8
/*
 * masks for setting up the action request word.
 */
#define M32_AR_BASE_MASK (M32_AR_MFL) 
#define M32_AR_ONE_CHNL_MASK (M32_AR_BASE_MASK | M32_AR_IN) 
#define M32_AR_ALL_CHNL_MASK (M32_AR_BASE_MASK | M32_AR_RES) 
#define M32_AR_STARTUP_MASK (M32_AR_BASE_MASK | M32_AR_RES | M32_AR_IA) 

/*
 * values to enable and clear Munich32 interrupts
 */
#define M32_REG_INT_DISABLE 0x0
#define M32_REG_INT_ENABLE  0x1
#define M32_REG_INT_CLEAR   0x0
#define M32_REG_INT_PENDING 0x1

/*
 * number of long word blocks of elements in the interrupt
 * queue. each block is 16 long words. this is how the chip
 * wants the number of elements represented.
 */
#define M32_INTQ_BLOCKS   255
#define M32_INTQ_ELEMENTS ((M32_INTQ_BLOCKS + 1) * 16)

/*
 * process this many events on the interrupt queue
 * for every interrupt.
 */
#define M32_INTS_TO_PROCESS M32_INTQ_ELEMENTS
 
/*
 * masks for the interrupt queue element bits
 */
#define M32_INT_INT 0x80000000
#define M32_INT_VERSION 0x38000000
#define M32_INT_ARACK   0x8000
#define M32_INT_ARF     0x4000
#define M32_INT_HI      0x2000
#define M32_INT_FI      0x1000
#define M32_INT_IFC      0x800
#define M32_INT_SF       0x400
#define M32_INT_ERR      0x200
#define M32_INT_FO       0x100
#define M32_INT_RT        0x20
#define M32_INT_CHANNEL   0x1f

#define M32_INT_VER_SHIFT 27
#define M32_INT_VER_21    1
#define M32_INT_VER_22    2
#define M32_INT_VER_32    3
#define M32_INT_VER_33    4
/*
 * values for the interrupt queue element bits
 */
#define M32_INT_RT_XMIT  0x0
#define M32_INT_RT_RECV 0x20
/*
 * munich32 bitrates are always multiples of 8
 */
#define M32_BITRATE_DIVISOR 8
#define M32_MAX_FILLMASK    8
/*
 * whether to use the most or least significant bits when
 * subrating a channel. if less then the full 64Kb of a timeslot
 * is being used, some number less than the full 8 bits are 
 * sent. these values determine how to send out the bits.
 */
#define M32_TIMESLOT_USE_MSB 0
#define M32_TIMESLOT_USE_LSB 1
/*
 * timeslot masks for the control structure
 */
#define M32_TIMESLOT_CHANNEL 0x1f00
#define M32_TIMESLOT_BITRATE   0xff
#define M32_TIMESLOT_INHIBIT 0x2000
#define M32_TIMESLOT_CHNL_SHIFT 8
#define M32_TIMESLOT_BITRATE_64KBPS 0xff
/*
 * masks for the the first long word of the channel specification.
 * values are used for the m32_channel_info field of the 
 * channel control structure
 */
#define M32_CHANNEL_INT_MASK 0x30000000
#define M32_CHANNEL_NITBS      0x800000
#define M32_CHANNEL_CS            0x100
#define M32_CHANNEL_CRC            0x40
#define M32_CHANNEL_FA              0x8
#define M32_CHANNEL_MODE            0x6
#define M32_CHANNEL_IFTF            0x1
#define M32_CHANNEL_TMA_MASK        ~((ulong) M32_CHANNEL_MODE)
/*
 * masks for recv/xmit commands to the chip
 */
#define M32_CHANNEL_TX_RX_CMD_MASK 0x7b0000 /* rx/tx command mask */
#define M32_CHANNEL_ABORT_CMD 0x1b0000 /* rx/tx abort command */
#define M32_CHANNEL_CLEAR_CMD      0x0 /* rx/tx clear command */
#define M32_CHANNEL_INIT_CMD  0x690000 /* rx/tx init command */
#define M32_CHANNEL_OFF_CMD    0x60000 /* recv. off cmd, xmit hold cmd */

/*
 * standard masks for startup and channel initialization
 */
#define M32_CHANNEL_BASE_MASK (M32_CHANNEL_INT_MASK | M32_CHANNEL_MODE)
#define M32_CHANNEL_INIT_MASK (M32_CHANNEL_BASE_MASK | M32_CHANNEL_NITBS | M32_CHANNEL_INIT_CMD)
#define M32_CHANNEL_ABORT_MASK (M32_CHANNEL_BASE_MASK | M32_CHANNEL_ABORT_CMD)
#define M32_CHANNEL_CLEAR_MASK (M32_CHANNEL_BASE_MASK | M32_CHANNEL_CLEAR_CMD)
#define M32_CHANNEL_DOWN_MASK (M32_CHANNEL_BASE_MASK | M32_CHANNEL_OFF_CMD)

/*
 * recv descriptor masks for params field 
 */
#define M32_RECV_DESC_HOLD   0x40000000
#define M32_RECV_DESC_HI     0x20000000
#define M32_RECV_DESC_NO     0x1fff0000
#define M32_RECV_DESC_NO_SHIFT 16
/*
 * recv descriptor masks for status field 
 */
#define M32_RECV_DESC_FE     0x80000000
#define M32_RECV_DESC_C      0x40000000
#define M32_RECV_DESC_BNO    0x1fff0000
#define M32_RECV_DESC_STATUS     0xff00
#define M32_RECV_DESC_BNO_SHIFT 16
/*
 * recv descriptor status masks, these bits give
 * more detail about an error that occured while
 * receiving a frame. 
 */
#define M32_RECV_STATUS_SF   0x4000
#define M32_RECV_STATUS_LOSS 0x2000
#define M32_RECV_STATUS_CRC  0x1000
#define M32_RECV_STATUS_NOB   0x800
#define M32_RECV_STATUS_LFD   0x400
#define M32_RECV_STATUS_RA    0x200
#define M32_RECV_STATUS_ROF   0x100

#define M32_RECV_PARAMS_INITIAL M32_RECV_DESC_HI
#define M32_RECV_STATUS_INITIAL M32_RECV_DESC_FE
/*
 * xmit descriptor masks 
 */
#define M32_XMIT_DESC_FE      0x80000000
#define M32_XMIT_DESC_HOLD    0x40000000
#define M32_XMIT_DESC_HI      0x20000000
#define M32_XMIT_DESC_NO      0x1fff0000
#define M32_XMIT_DESC_CSM          0x800
#define M32_XMIT_DESC_FNUM             0
#define M32_XMIT_DESC_FNUM_MAX     0x7ff
#define M32_XMIT_DESC_NO_SHIFT 16
#define M32_MAX_INTERFRAME_FLAGS 64

#define M32_XMIT_DESC_INITIAL (M32_XMIT_DESC_FE | M32_XMIT_DESC_HI | M32_XMIT_DESC_FNUM)

/*
 * masks used to represent bits in the interrupt status and
 * clear register. the inverse of these masks are used to clear 
 * the bits. when the register is written, a 0 in a bit position
 * clears that bit, a 1 leaves the bit unchanged.
 */
#define M32_HOST_INT 0x1
#define M32_AR_INT   0x2
#define M32_CLEAR_HOST_INT ~(M32_HOST_INT)
#define M32_CLEAR_AR_INT   ~(M32_AR_INT)


#define M32_RUNT_THRESHOLD      30     /* The threshold to start calculating new */
                                       /* error rate */
#define M32_ERR_HIGH_MARK       300    /* The threshold for Munich32 runts error */
                                       /* rate */
#define M32_INTERVAL            3      /* The threshold to determinate displaying*/
                                       /* the error message */
#define CHAN_INTERVAL           1      /* The threshold to determinate */
                                       /* dropping the call in B channel */
#define M32_ERR_LOW_MARK        05     /* The low threshold for Munich32 runts */
                                       /* error rate */

/*
 * data patterns for enabling and disabling CSU remote loopback switch
 */
#define REM_LOOP_PATTERN_MATCHED(ch, pat) ((ch & 0x7f) == pat)
#define REM_LOOP_TIP_BYTES       0x3a
#define REM_LOOP_SELECT          0x31
#define REM_LOOP_LBE_BYTES       0x56
#define REM_LOOP_FEV_BYTES       0x5a
 
#define REM_LOOPUP_TIP_CNT     40
#define REM_LOOPUP_SELECT_CNT  40
#define REM_LOOPUP_LBE_CNT    120
#define REM_LOOPUP_FEV_COUNT  (80*10)   /* 100 ms data per time slot */
#define REM_LOOPDOWN_TIP_CNT  (80*10)   /* 100 ms data per time slot */
#define REM_LOOPUP_PREAMBLE_CNT (REM_LOOPUP_TIP_CNT+REM_LOOPUP_SELECT_CNT+REM_L\
OOPUP_LBE_CNT)
#define REM_LOOP_FRAME_CNT     50
 
/*
 * types of loopback the chip can be in
 */
enum m32_loopback_t {
    M32_NO_LOOPBACK = 0,
    M32_LOCAL_LOOPBACK,
    M32_REMOTE_LOOPBACK_WAIT_UP,
    M32_REMOTE_LOOPBACK_ACTIVE,
    M32_REMOTE_LOOPBACK_WAIT_DOWN,
};
 
/*
 * errors returned by Munich32 driver code
 */
enum m32_error_t {
    M32_NOERR = 0,
    M32_ERR_NO_TIMESLOT,
    M32_ERR_TIMESLOT_IN_USE,
    M32_ERR_ILLEGAL_CHANNEL,
    M32_ERR_AR_TIMEOUT,
    M32_ERR_AR_FAILURE,
    M32_ERR_BAD_STATE,
    M32_ERR_BAD_BITRATE,
    M32_ERR_BAD_INTQ,
    M32_ERR_MEM_ALLOC
};


/*
 * ----------------------------------------
 * Structures
 * ----------------------------------------
 */

/*
 *------------------------------------------------------
 * These structures are defined by the Munich32 chip.
 * Do not change them unless the hardware requires it.
 * Many of these definitions are comprised of bit fields.
 * Refer to the Munich32 data sheet for details.
 *------------------------------------------------------
*/

/* 
 * points to the interrupt queue, stored in the control struct.
 */
typedef struct m32_intq m32_intq_t;
struct m32_intq {
    ulong *intq_addr;
    ulong num_q_elements;
};

/* 
 * used to map a timeslot to a data channel.
 * data channels can have multiple timeslots.
 * also used to set the bandwidth of a timeslot
 * by specifying how many bits will be used.
 */
typedef struct m32_timeslot m32_timeslot_t;
struct m32_timeslot {
    ushort recv_timeslot;
    ushort xmit_timeslot;
};

/* 
 * receive and transmit descriptor structures
 */
typedef struct m32_recv_desc m32_recv_desc_t;
struct m32_recv_desc {
    ulong recv_params;
    ulong recv_status;
    uchar *recv_data_ptr;
    m32_recv_desc_t *next_recv_desc;
}; 

typedef struct m32_xmit_desc m32_xmit_desc_t;
struct m32_xmit_desc {
    ulong xmit_params;
    uchar *xmit_data_ptr;
    m32_xmit_desc_t *next_xmit_desc;
};

/* 
 * Various fields dealing with defining the channel.
 */
typedef struct m32_channel_ctrl m32_channel_ctrl_t;
struct m32_channel_ctrl {
    ulong channel_info;
    m32_recv_desc_t *first_recv_desc;
    m32_xmit_desc_t *first_xmit_desc;
    ulong channel_itbs; /* no. of bytes in internal xmit buffer */
};

/* 
 * main configuration and control structure for the Munich32 chip
 * many of this fields have bit fields, use the Munich32 data sheet
 * for details of all fields.
 */
typedef struct m32_control m32_control_t;
struct m32_control {
    /* 
     * action specification fields 
     */
    volatile ulong ctrl_action; 
    /* 
     * address of the interrupt queue and the number of elements 
     */
    volatile m32_intq_t ctrl_intq;  
    /* 
     * recv. & xmit timeslots
     */
    volatile m32_timeslot_t ctrl_timeslots[M32_NUM_CHANNELS]; 
    /* 
     * various channel related commnands, see data sheet for details 
     */ 
    volatile m32_channel_ctrl_t ctrl_channels[M32_NUM_CHANNELS];
    /* 
     * array of ptr.s to current receive descriptor for every channel 
     */
    volatile m32_recv_desc_t *ctrl_curr_recv_desc[M32_NUM_CHANNELS];
    /* 
     * array of ptr.s to current xmit descriptor for every channel 
     */
    volatile m32_xmit_desc_t *ctrl_curr_xmit_desc[M32_NUM_CHANNELS];
};

/*
 * end of chip defined structures
 */

/*
 * an element in a pak ring. pak rings mirror the xmit and
 * recv rings. they allow a pointer to the begining of a 
 * pak structure to be accessed at interrupt level when
 * only the encaps/data portion of a pak is known.
 */
typedef struct m32_pak_ring m32_pak_ring_t;
struct m32_pak_ring {
    paktype *pak;
    m32_pak_ring_t *next_pak;
};

/* 
 * adaptor vectors used by the m32 driver
 */
typedef void (*m32_recv_handler_t)(hwidbtype *, paktype *);
typedef void (*m32_xmit_handler_t)(hwidbtype *, paktype *);
typedef void (*m32_disable_adaptor_t)(hwidbtype *);
typedef void (*m32_enable_adaptor_t)(hwidbtype *);
typedef void (*m32_down_connection_t)(hwidbtype *);

/* 
 * parameters used to define a single channel
 */

typedef struct m32_channel_params m32_channel_params_t;
struct m32_channel_params {
    /*
     * general channel parameters
     */
    ulong channel_num;
    hwidbtype *channel_idb;  /* idb associated w/ channel */
    /*
     * bandwidth of this channel
     */
    ulong bandwidth;
    /*
     * bitfield of timeslots assigned to this channel
     */
    ulong timeslot_bitfield;
    /*
     * The saved bitrate fillmask for the timeslots
     */
    ulong fillmask;
    /*
     * whether it's ok to throttle this channel and throttle
     * and re-enable counters
     */
    boolean throttle_channel;
    ulong throttle_count;
    ulong enable_count;
    enum m32_loopback_t loopback;
    ulong rloopback_frame_sent;
    ulong rloopback_frame_cnt;
    /*
     * number of interrupts received for unused or idle channels.
     */
    ulong ints_unused_channel;
    /*
     * number of bytes between the begining of the 
     * encapsulation space and the actual encapsulation
     */
    ulong data_offset;

    hwidbtype *idb_drop;
    /* runt errors in certain period of time */
    ulong runts_errors;
    /* Time of the runt error started */
    ulong runt_started;
    /* times of the runt error rate over threshold */
    ushort runt_interval;

    /*
     * transmit and receive handlers for this channel
     * also, a vector for handling the case when no 
     * receive buffer is available.
     */
    m32_xmit_handler_t xmit_handler;
    m32_recv_handler_t recv_handler;
    m32_xmit_handler_t xmit_handler_saved;
    m32_recv_handler_t recv_handler_saved;

    m32_disable_adaptor_t disable_adaptor;
    m32_enable_adaptor_t enable_adaptor;
    m32_down_connection_t down_connection;
    /*
     * channel state variables
     */
    enum m32_channel_state_t initial_state;
    enum m32_channel_state_t current_state;
    /*
     * general ring parameters
     */
    ulong rx_ring_size;            /* no. of rx ring entries */
    ulong tx_ring_size;            /* no. of tx ring entries */
    ulong tx_ring_use_count;       /* count of tx descriptors in use */

    /*
     * recieve and transmit parameters for the DESCRIPTOR ring
     */
    m32_recv_desc_t *rx_ring_head;  /* head of rx descriptor ring */
    m32_recv_desc_t *rx_ring_tail;  /* tail of rx descriptor ring */
    m32_recv_desc_t *rx_desc_throttle_hold; /* the descriptor marked with
                                               M32_RECV_DESC_HOLD when 
                                               throttled 
                                               */
    m32_xmit_desc_t *tx_ring_head;  /* head of tx descriptor ring */
    m32_xmit_desc_t *tx_ring_tail;  /* tail of tx descriptor ring */
    /*
     * recieve and transmit parameters for the PACKET ring
     */
    m32_pak_ring_t *rx_pak_first; /* first entry of rx pak ring */
    m32_pak_ring_t *rx_pak_head;  /* head of rx pak ring */
    m32_pak_ring_t *rx_pak_tail;  /* tail of rx pak ring */
    m32_pak_ring_t *tx_pak_first; /* first entry of tx pak ring */
    m32_pak_ring_t *tx_pak_head;  /* head of tx pak ring */
    m32_pak_ring_t *tx_pak_tail;  /* tail of tx pak ring */

    boolean tx_pending;  /* Transmit pending for channel */
    boolean drop_call;   /* if true, check we need to drop call because of */
                         /* excessive errors from M32 B channel */
    boolean first_start; /* The start of the every error checking interval */
                         /* if it is TRUE */
    boolean ar_failed;
};

/*
 * structure passed in by port adaptor code to 
 * initialize a channel.
 */
typedef struct m32_channel_init_info m32_channel_init_info_t;
struct m32_channel_init_info {
    ulong channel_num;
    hwidbtype *channel_idb;
    ulong data_offset; /* offset from begining of encaps area */
    ulong timeslot_bitfield; /* list of timeslots for this channel */
    ulong bitrate; /* bitrate of a timeslot, max 64Kb min 8Kb */
    ulong bitrate_which_bits; /* use LSB or MSB if subrating */
    boolean throttle_channel; /* if TRUE, it's ok to throttle this channel */
    enum m32_loopback_t loopback;
    enum m32_channel_state_t initial_state;
    ulong rx_ring_size;
    ulong tx_ring_size;
    m32_xmit_handler_t xmit_handler; /* xmit interrupt handler */
    m32_recv_handler_t recv_handler; /* recv interrupt handler */
    m32_disable_adaptor_t disable_adaptor; /* vectors to disable and */
    m32_enable_adaptor_t enable_adaptor; /* enable adaptor chips */
    m32_down_connection_t down_connection; /* vector to down connection */
    ulong internal_buf_size; /* count of internal m32 bufs to use */
};

/* 
 * munich32 registers. this structure must be mapped onto 
 * a platform specific address space. this struct assumes
 * the registers will be contiguous.
 */
typedef struct m32_regs m32_regs_t;
struct m32_regs {
    volatile m32_control_t *control_start_reg;
    volatile ushort int_status_reg;
    volatile ushort int_enable_reg;
    volatile ushort action_request_reg;
};

/*
 * instance structure for the Munich32. one instance struct is
 * allocated for every Munich32 chip in the box. the typedef for
 * this struct should be declared in a platform specific header file.
 */
struct m32_instance {
    /*
     * ptr. to the m32 configuration and control struct
     */
    m32_control_t *ctrl;
    /*
     * Munich32 chip version number
     */
    ulong chip_version;
    /*
     * format of the pcm highway. specifies number of channels
     * and the speed of the pcm bus.
     */
    ulong pcm_format;
    /*
     * action request in use flag and success/failure counters.
     */
    ulong ar_in_progress;
    ulong ar_success;
    ulong ar_timeout;
    ulong ar_failure;
    /*
     * index into interrupt queue array
     */
    ulong intq_index;
    /*
     * the idle channel as the channel all timeslots that
     * are not in use are assigned to. this allows timeslots
     * that are not in use to transmit idle flags. idle timeslots
     * is a bitfield of all unused timeslots.
     */
    ulong idle_channel;
    ulong idle_timeslots;
    hwidbtype *idle_idb;
    /*
     * array of channel info structs. parameters for each
     * channel are kept here.
     */
    m32_channel_params_t channel_params[M32_NUM_CHANNELS];
    /* 
     * pointer to the munich32 register set
     */
    m32_regs_t *regs;
    /*
     * count of bad interrupt queue elements
     */
    ulong bad_intq_element_count;
    hwidbtype *idb_runts;
    ulong   runts_count;
    boolean m32_pro_kill;
    boolean en_runts_check;
    boolean no_auto_check;
    boolean set_calldrop;
    boolean waiting_arack_in_interrupt;
    void (*runts_handle) (hwidbtype *);
};


/*
 * ----------------------------------------
 * function prototypes and inlines
 * ----------------------------------------
 */
void m32_fastsend(hwidbtype *idb, paktype *pak);
m32_regs_t *m32_get_regs(ulong slot, ulong subcont);
void m32_set_vector(ulong slot);
void m32_reset_chip(ulong slot);

/*
 * Externs
 */
extern m32_instance_t *m32_startup(uint slot, ulong subcont, ulong pcm_format);
extern enum m32_error_t m32_init_channel(m32_instance_t *m32_ds,
                       m32_channel_init_info_t *m32_channel_init);
extern enum m32_error_t m32_release_channel(m32_instance_t *m32_ds,
					    ulong channel);
extern enum m32_error_t m32_reset_channel(m32_instance_t *m32_ds,
					  ulong channel);
extern enum m32_error_t m32_down_channel(m32_instance_t *m32_ds,
					 ulong channel);
extern enum m32_error_t m32_up_channel(m32_instance_t *m32_ds,
				       ulong channel);
extern ulong m32_get_channel_bw_inline(m32_instance_t *m32_ds,
				       ulong channel);
extern hwidbtype *m32_get_channel_idb_inline(m32_instance_t *m32_ds,
					     ulong channel);
extern enum m32_channel_state_t m32_get_channel_state_inline(
                               m32_instance_t *m32_ds, ulong channel);
extern enum m32_error_t m32_create_idle_channel(m32_instance_t *m32_ds);
extern void m32_tx_start(hwidbtype *idb);
extern void m32_print_rings(m32_instance_t *m32_ds, ulong channel);
extern void m32_enable(m32_instance_t *m32_ds, ulong channel, 
		       enum m32_channel_state_t state);
extern void m32_interrupt(ulong slot, ulong subcont);
extern void dsx1_pro_add(int);
extern watched_boolean  *m32print_process_awaken;
extern watched_boolean  *calldrop_process_awaken;
extern enum m32_error_t m32_tristate_isdn_b_channel (m32_instance_t *m32_ds,
                                                     ulong channel);
#endif
