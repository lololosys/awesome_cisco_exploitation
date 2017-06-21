/* $Id: if_as5200_dsx1_framer.h,v 3.1.68.2 1996/09/07 22:36:59 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dsx1_framer.h,v $
 *------------------------------------------------------------------
 * if_as5200_dsx1_framer.h - Common framer defines for any E1/T1 interface.
 *
 * Nov 1995, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dsx1_framer.h,v $
 * Revision 3.1.68.2  1996/09/07  22:36:59  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.1.68.1  1996/06/16  21:14:36  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/09  23:55:55  jliu
 * Create placeholder for new files
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

#define MAX_GAUGE_COUNT 0x7fffffff
#define ADD_TO_GAUGE(a,b) {a += b; if ( a > MAX_GAUGE_COUNT ) a = MAX_GAUGE_COUNT;}

struct dsx1_instance {

    struct dsx1info_ *dsx1_ds;     /* DSX1 interface info data structure */

    short          carrier_status; /*TRUE for carrier state UP*/
    uchar          alive_count;    /*10 secs count to detect Neat processor*/
                                   /*is still alive or not?*/
    uchar          secs_for_frame;
    uchar          one_sec_timer;
    uchar          appliquetype;

    short          frame_type;
    short          clock_type;
    short          loopback_type;
    
    uchar          slot;
    uchar          unit;
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
    ushort         loopback_act;

    boolean        unavailable_state;
    boolean        available;
    boolean        m32_errmsg_print;
    boolean        dsx1_noloop_trap;
    boolean        runts_recovery;

    void           (*framer_remote_alarm)(volatile short *, boolean);
};

struct dtmf_instance {
     uchar	modem_id;
     uchar      *phone_digits;
     uchar      unit;
};

/*
 * Prototypes
 */
void framer_interrupt(uint slot);
void carrier_interrupt(void);
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

extern queuetype t1_sec_updateQ;
extern queuetype e1_sec_updateQ;
