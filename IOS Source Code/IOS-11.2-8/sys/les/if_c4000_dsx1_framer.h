/* $Id: if_c4000_dsx1_framer.h,v 3.3 1995/11/17 17:39:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_c4000_dsx1_framer.h,v $
 *------------------------------------------------------------------
 * if_c4000_dsx1_framer.h - Common framer defines for any E1/T1 interface. 
 *
 * Nov 27 1994, Joe Liu
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_dsx1_framer.h,v $
 * Revision 3.3  1995/11/17  17:39:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/09  01:36:54  jliu
 * CSCdi39941:  loopback LED on CT1 nim always on under normal operation
 * Put extern queuetype to .h file instead
 *
 * Revision 2.1  1995/06/07  21:34:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define DSX1_OTHER_FRAME  1

typedef enum {
    REM_LOOP_NOT_SRCH = 0, REM_LOOP_ON_SRCH_CODE,
    REM_LOOP_OFF_SRCH_CODE, REM_LOOP_OFF_WAIT_NO_CODE,
    REM_LOOP_IN_LOOP,REM_LOOP_ON_SRCH_BUT_STOPPED
} LoopStatusType;

/*
 *  Note that we rely on the fact that these counters never counting high
 *  enough to wrap.  If you use these with a long count (more than
 *  the variable can hold before wrapping (or going negative if signed))
 *  then you will likely introduce a bug.
 */
#define CLEAR_COUNTER(a)        ((a) = 0)
#define FORWARD_COUNTER(a,q)    ((a) += (q))
#define COUNTER_EXPIRED(a,b)    ((a) >= (b))
#define HAVE_COUNTER(a)         ((a) > 0)
#define BACKWARD_COUNTER(a,q)   ((a) -= (q))

#define FRAMER_TIME     1
#define FRAMER_TIMES_PER_SEC    4
#define T1_REM_ALARM_TIME       (2 * FRAMER_TIMES_PER_SEC)
#define E1_REM_ALARM_TIME       (FRAMER_TIMES_PER_SEC / 2)

/*
 *  The following is really 2.5 * FRAMER_TIMES_PER_SEC, but avoids floating
 *  point math
 */
#define T1_FRAME_LOSS_TIME ((2*FRAMER_TIMES_PER_SEC)+(FRAMER_TIMES_PER_SEC/2))
#define E1_FRAME_LOSS_TIME (FRAMER_TIMES_PER_SEC/2)

/*
 * External LED indicator register
 */
#define DSX1_LED_RLOOP       0x0004        /* 0 for trun on remote loopback LED */
#define DSX1_LED_LOCALARM    0x0002        /* 0 for trun on local alarm LED */
#define DSX1_LED_REMALARM    0x0001        /* 0 for trun on remote alarm LED */

#define DSX1_INT_ENABLE      0x0001        /* 1 for framer interrupt enable*/

#define MAX_GAUGE_COUNT 0x7fffffff
#define ADD_TO_GAUGE(a,b) {a += b; if ( a > MAX_GAUGE_COUNT ) a = MAX_GAUGE_COUNT;}

typedef struct mpu_instance  mpu_instance_t;

struct dsx1_instance {
    volatile short *framer;       /* framer's registers */
    volatile short *ram;          /* framer's ram area */
    volatile ushort *ext_ctl;     /* DSX1 interface control register */
    volatile ushort *ext_status;  /* DSX1 interface status register */
    volatile ushort *ext_int_ctl; /* DSX1 interrupt enable regsiter */
    volatile ushort *ext_led;     /* DSX1 interface LED */

    struct dsx1info_ *dsx1_ds;    /* DSX1 interface info data structure */

    mpu_instance_t   *mpu;        /* E1 info data structure */

    uchar       secs_for_frame;
    uchar       one_sec_timer;
    uchar       appliquetype;

    boolean        has_los;                 /* TRUE for LOS */
    boolean        has_ais;                 /* TRUE for blue alarm  */
    boolean        has_frame;
    boolean        frame_acquired;
    boolean        frame_loss_occurred;
    boolean        txing_rem_alarm;
    boolean        txing_ais;
    boolean        unavailable_state;
    boolean        rxing_rem_alarm;
    ulong          count_no_remalarm;
    ulong          count_until_frame;
    ulong          count_toward_loss;
    ulong          count_until_loss_clear;
    boolean        intrpt_los;
    boolean        intrpt_oof;
    short          frame_type;
    short          clock_type;
    
    ulong          bpv_in_min;
    ulong          pcv_in_min;
    ulong          crc_in_min;
    ulong          bpvneeded_in_min;
    ulong          pcvneeded_in_min;
    ulong          crcneeded_in_min;
    short          secs_for_min;
    short          bes_in_failed;
    short          es_in_failed;
    short          bes_last_interval;
    short          es_last_interval;
    ushort         ses_count;               /* count of ses for unavail */
    
    boolean        available;
    boolean        m32_errmsg_print;
    boolean        runts_recovery;
    
    LoopStatusType rem_loop_state;
    uchar          rem_loop_count;
    boolean        in_rem_loop;
    
    void           (*framer_remote_alarm)(volatile short *, boolean);
};

/*
 * Prototypes
 */
void framer_uas(dsx1_instance_t *, dsx1info *, ushort, ushort, boolean,
		boolean, boolean, dsx1TableType *);
void framer_interrupt(uint slot);
void framer_send_ais(cdbtype *, boolean);
void framer_reg_add(void);
void framer_led_on(dsx1_instance_t *, ushort);
void framer_led_off(dsx1_instance_t *, ushort);
void framer_set_MIB_ls(int *, ushort);
void framer_clr_MIB_ls(int *, ushort);
void framer_frame_loss(dsx1_instance_t *, dsx1info *);
void framer_send_remalarm(dsx1_instance_t *, boolean);
forktype framer_background_process(void);
void framer_check_available(dsx1_instance_t *);
void framer_clr_dsx1table(dsx1TableType *);

extern queuetype ce1_sec_updateQ;
