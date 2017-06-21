/* $Id: arap_mnp4.h,v 3.2 1995/11/17 08:38:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_mnp4.h,v $
 *------------------------------------------------------------------
 * Declarations
 *
 * January 1997, Bill Thomas & Arun Tirumalai
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_mnp4.h,v $
 * Revision 3.2  1995/11/17  08:38:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/18  06:45:38  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 20:04:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef MNP4_H__
#define MNP4_H__

typedef uchar mod_cntr;

/* mnp4 modulo constants */
#define MNP4_EXP        8
#define MNP4_MOD        (1 << MNP4_EXP)
#define MNP4_MSK        (MNP4_MOD - 1)
 
/* max transmission window constants */
#define MNP4_EXP_VK     (MNP4_EXP - 5)
#define MNP4_MOD_VK     (1 << MNP4_EXP_VK)
#define MNP4_RESEND_SIZE MNP4_MOD_VK
#define MNP4_MOD_RESEND  (MNP4_RESEND_SIZE - 1)
 
/* general case
 * #define MOD_SUB(a,b) (((a+MOD)-b)&MSK)
 * #define MOD_ADD(a,b) ((a+b)&MSK)
 */ 

#define MNP4_MOD_SUB(a,b) ((mod_cntr)((mod_cntr)a-(mod_cntr)b))
#define MNP4_MOD_ADD(a,b) ((mod_cntr)((mod_cntr)a+(mod_cntr)b))

/* WARNING:
 *   don't do:
 *	if (MOD4_MOD_SUB(a, b) == 0)
 *	if (MOD4_MOD_SUB(a, b) < 0)
 *	if (MOD4_MOD_SUB(a, b) > 0)
 *   instead do
 *	if (a == b)
 *	if (a != b)
 */
 
#define MNP4_LR_LD_WAIT_FACTOR	      20  /* multiply T401 factor for LR & LD*/
#define MNP4_DEFAULT_RESEND_TIMEOUT    3*ONESEC  /* T401 default time not LR's*/
#define MNP4_DEFAULT_ACK_DELAY         MNP4_DEFAULT_RESEND_TIMEOUT/2
#define MNP4_MAX_RESENDS 12	/*N400 max burst resends without recv an ack*/
 
#define MNP4_RCV_RESERVE 4      /*reserve's for receive LA's & loop delays*/
#define MNP4_RVC_CASHE_SIZE MNP4_MOD_VK + MNP4_RCV_RESERVE
#define MNP4_MAX_RESEND_BUMPS 10	/*max resend timer bumps*/

typedef struct mnp4stats {
    int packets_received;
    int packets_sent;
    int garbled_received;
    int out_of_order;
    int packets_resent;
    int nobuffers;
} mnp4_stats;

#define MNP4_LOG_REC_FLAG 0x80000000
#define MNP4_LOG_XMT_FLAG 0x40000000
#define MNP4_LOG_MSK      (MNP4_LOG_REC_FLAG | MNP4_LOG_XMT_FLAG) 
#define MNP4_LOG_TIME_MSK (~MNP4_LOG_MSK)
#define MNP4_LOG_ITEM_SIZE 4
#define MNP4_LOG_SIZE 300
/*
 * some models of the mac get booged down and their ack delays
 * cause the router to do many resends.  here we measure the
 * mac's ack delay and adjust T401 to reduce the number of
 * resends.
 *
 * avrg_delay = exp(-constant)*(avrg_delay - ack_delay) + ack_delay;
 *
 * slow way:
 *   after 15 iterations want 95% of final value (if ack_delay were constant).
 *   constant = 3/15 = 1/5. 
 *   .819 = exp(-1/5) or 819/1000
 *   avrg_delay = 819*(avrg_delay - ack_delay)/1000 + ack_delay;
 *
 *   division is too slow for real time.
 *
 * fast way:
 *   avrg_delay = .8125*(avrg_delay - ack_delay) + ack_delay;  
 *
 *   .8125 turns out to be faster than .819 and instead of 15 iterations
 *   to get 95% its 14.4 iterations.
 *
 *   .8125 = 1/2 + 1/4 + 1/16
 *
 *      -0.208 = ln(.8125)
 *
 *      95% of final value at 3 time constants; 0.95 = 1 - exp(-3)
 *
 *      14.4 = -3/(-0.208
 *      14.4 iterrations required to get 95% of final value.
 *
 *   then:
 *     avrg_delay = .8125*(avrg_delay - ack_delay) + ack_delay; 
 *     term = avrg_delay - ack_delay
 *     p = term/2 + term/4 + term/16
 *
 *     p = term >> 1
 *     temp = p >> 1
 *     avrg_delay =  p + temp + (temp >> 2) + ack_delay;
 *
 *    or
 *     avrg_delay = (avrg_delay - ack_delay) >> 1
 *     temp = avrg_delay >> 1
 *     avrg_delay += temp + (temp >> 2) + ack_delay;
 *
 */
#define MNP4_AVRG_FACT1(x) ((x) >> 1)
#define MNP4_AVRG_FACT2(x) ((x) >> 2)
#define MNP4_AVRG_VALID 15	/* 14.4 iterrations required to get 95% */

typedef struct {
    ulong time;
    uchar data[MNP4_LOG_ITEM_SIZE];
} mnp4_item;

typedef struct {
    mnp4_item item[MNP4_LOG_SIZE];
} mnp4_log;

typedef struct {
    paktype *pak;		/*pointer to pak to resend */
    boolean resending;		/*this is a resend of pak  */
    sys_timestamp time;		/*time pak sent            */
} resend_t;

struct mnp4type_ {
    paktype *mnp4_inpak;	/* current MNP4 input packet */
    paktype *mnp4_outpak;	/* current MNP4 output packet */
    paktype *pak_ack;		/* fixed ack pak - free'ed by mnp4_disable() */
    resend_t resend[MNP4_RESEND_SIZE];  /*resend structure*/
    int istate;			/* MNP4 input state */
    int ostate;			/* MNP4 output state */
    tt_soc *tty;

    boolean do_resend;          /* do resends*/
    boolean send_ack;           /* send an ack ie, LA*/
    boolean free_pak_on_send;   /* resend pak to be free'ed by mnp4_sndchar()*/

    sys_timestamp timer_resend;		/*timer used to start resends LT's*/
    sys_timestamp timer_ack_delay;      /*timer to delay sending an ack   */
    sys_timestamp wait_timer;		/*wait timer for remote reply */
    sys_timestamp timer_disconnect;	/*tear down disconnect time out */

    long t401_base;		/* base timer_resend computed from bps rate */ 
    long t401_max;		/* max timer_resend computed from bps rate */ 
    long t401;			/* timer_resend adjusted by avrg_delay */ 
    long t402;			/* value for timer_ack_delay */
    long avrg_delay;		/* average ack delay in mil seconds */

    mnp4_stats stats;
    mnp4_log *log_ptr;		/* pointer to log struct */
    queuetype MNP4InQ;		/* input Queue */
    queuetype recv_ackQ;	/* queue of received ack's*/
    queuetype send_ltQ;		/* tty send LT queue - numbered */
    queuetype send_lr_ldQ;	/* tty send LR, LD un-numbered queue */
    queuetype bufferQ;		/* recv pak cashe */

    ushort lt_ack_cntr;		/* don't ack on 1st previous LT - counter */
    ushort resend_cntr;		/* cleared on acked frame or on sending LT*/
    short avrg_delay_cntr;	/* delays use of avrg_delay till valid */
    short log_index;		/* put index for log_ptr */
    uchar mnp4_syn;
    uchar mnp4_dle;
    uchar link_state;		/* link state of the mnp4 session*/
#define MNP4_CONNECTING 0	/* before LR LA sequence*/
#define MNP4_DATA_PHASE 1	/* after LR LA sequence*/
#define MNP4_DISCONNECTING 2	/* on send/recv LD     */

    mod_cntr resend_r;          /* index into resend[]*/
    mod_cntr rvs;               /* next expected received min value of Ns*/
    mod_cntr ack_vs;            /* last Nr == (rvs - 1) sent*/
    mod_cntr rvr;               /* received Nr + 1*/
    mod_cntr rvk;               /* received Nk*/
    mod_cntr vs;                /* next Ns to be sent*/
    mod_cntr vk;                /* last allocation sent Nk*/
    mod_cntr vs_queued;		/* Ns counter assigned at output queue time*/
}; 


/******************************************************************************

MNP4 ASYNC FRAMING used in mnp4_gotchar() & mnp4_sndchar(): 

   INPUT                 STATES
            ___________________________________
           |IDLE |DLE1 |DATA |DLE3 |CRC4 |CRC5 |
           |     |     |     |     |     |     |
   --------|-----|-----|-----|-----|-----|-----|
    SYN    |DLE1 |IDLE |DATA |IDLE |CRC5 |IDLE |
           |     |     |STORE|     |CHECK|CHECK|
   --------|-----|-----|-----|-----|-----|-----|
    DLE    |IDLE |IDLE |DLE3 |DATA |CRC5 |IDLE |
           |     |     |     |STORE|CHECK|CHECK|
   --------|-----|-----|-----|-----|-----|-----|
    STX    |IDLE |DATA |DATA |IDLE |CRC5 |IDLE |
           |     |     |STORE|     |CHECK|CHECK|
   --------|-----|-----|-----|-----|-----|-----|
    OTHER  |IDLE |IDLE |DATA |IDLE |CRC5 |IDLE |
           |     |     |STORE|     |CHECK|CHECK|
   --------|-----|-----|-----|-----|-----|-----|
    ETX    |IDLE |IDLE |DATA |CRC4 |CRC5 |IDLE |
           |     |     |STORE|     |CHECK|CHECK|
   --------------------------------------------+


   NOTE: CRC4 & CRC5 done by arap_crc16.c & arap_crc16.h 

MNP4 FRAME TYPES:

 LA (Acknowledge LT)

   [0] == 3 or 7 length (3 optimized, 7 non-optimized)
   [1] == 5 LA type
   [2] == Nr; ACK's up to and including LT - Ns frames.
   [3] == N(k); credit ie, 0 can't send, 1 == send one ..etc, 

 LD (Disconnect)

   [0] == 4 or 7 length
   [1] == 2 LD type
   [2] == reason
   [3] == user

 LT (Transmit data)
 
   [0] == 2 or 4 length (2 optimized, 4 non-optimized)
   [1] == 4 LT type
   [2] == Ns; Ns <-- Vs++; init 1, range 0 - 255.

 LN (Attention) NOT SUPPORTED!

   [0] == 7 length
   [1] == 6 LN type
   [2] == Nsa; Nsa <-- Vsa++; init 1, range 0 - 255.
   [3] == Attention type
		D == Destructive break ie, flush everything.
		E == Expedited break
			0x1 == D & E
			0x2 == non-D & E
			0x3 == non-D & non-E

 LNA (Acknowledge LN) NOT SUPPORTED!

   [0] == 4 length
   [1] == 7 LNA type
   [2] == Nra; ACK's up to and including LN - Nsa frames.

NOTE: ARAP MNP4 ignores LN's & LNA's ie, they are not used.
	     
 LR (Link request)
   See V.42 recommedations page 53.

-------------------------------------------------------------------------------

      LT [Ns]
      LA [Nr][Nk]

      Ns is the send sequence number
      Nr is the acked sequence number (of last received frame)
   
      Nk is the allocation i.e., receive buffers available.
            Nk must be <= MOD/2
            Nk is not a MOD value i.e. its the number of
            packets which can be sent.  Nk == 0 means no packets
            can be sent i.e., flow-controled.
  
 internal variables usage:

   rvs is next expected received min value of Ns
   ack_vs is last Nr == (rvs - 1) sent
   rvr is received Nr + 1
   rvk is received Nk
   vs == next Ns to be sent (see note below for vs_queued).
   vk == last allocation sent Nk

   NOTE: other protocols ack with an Nr of their next expected
	 received frame.  mnp4's Nr is that of the last received
	 frame.  the above internal variables shift Nr to that
	 of the senders (of the ack) next expected receive frame. 

   NOTE: only LT's can be in the resend[] array, because only
	 it has a Ns (LN's are not supported).

   NOTE: for LT's we assign Ns in mnp4_output() so we can do the crc16.
         we use vs_queued for this purpose.  however, vs is updated
         as we send the LT in prime_mnp4_outpak().

 overview:

   the major sections of the mnp4 logic are:

	A. transmission - mnp4_sndchar()    
		which picks from four possible frame sources
		in accordance with the following priorities:

			1.  send's of LR's or LD's
			2.  ack's LA's
			3.  resend's of LT's
			4.  send's of LT's

		this split of frame sources insures that ack's
		can get ahead of the rest of the traffic, and
		that they ack (Nr) the most recent received frames. 

		transmission of LT's which are not due to resend's
		reset the resend counter.

		transmission of LT's restarts the resend timer.

		during session startup (link state == connecting), LA's
		are un-numbered (Ns = 0), and the sending of
		any LT's is not allowed.  link state data phase begins
		after the following:

			1. an LR is sent and a LR is received.
			   we respond with an LA and then enter the
			   data phase.

			2. an LR is received.  we respond
			   with a LR and then (after a wait) receive
			   an LA.  after which we enter the data phase.

		upon sending or receiving an LD link state is set to
		disconnecting, and LA's and LT's are ignored. 

	  NOTE:  from a conceptual point of view the above is correct.
		 but because of the logic of the "PT" (see comments in
		 mnp4_packets_pending()) the real mnp4_sndchar() HAS to use
		 prime_mnp4_outpak() to pick the pak to send.  the
		 comments in the code should help explain this some
		 what twisted logic - all to avoid a CPU HOG situation
		 in vty_asyncBLOCK(). 

	B. receive mnp4_gotchar() to mnp4_input()
		the ack's are processed from their queue, rcv_ackQ.
		and the regular frames are processed from queue,
		MNP4InQ.  this gives the receive ack's higher
		priority than all other traffic.

		mnp4_process_ack() removes ack'ed frames from the resend[] 
		array and updates rvr and rvk.  which is then immediatley
		available for ack transmissions in mnp4_sndchar().  note
		there is NO ack transmission queue ie, ALL DELAYS have
		been eliminated.  well almost, there is the delay of
		the rcv_ackQ.  the trade off would have been to
		process the receive ack's in interrupt routine
		mnp4_gotchar().  but that would have resulted in
		the crc16 computation and mnp4_process_ack() being
		done at receive interrupt level. 

		NOTE: the older mnp4 logic didn't seperate the ack's
		      and the resulting delays resulted in what
		      that author termed ack loop's.

		mnp4_process_ack() is also responsible for re-starting
		the transmit interrupts (via a startoutput() to mnp4_sndchar(). 
		re-starting is required in the case where mnp4_sndchar()
		was flow controlled.
		
		regular receive frame processing is simple with the exception
		of when to send an ack.  the goal is to send as few ack's
		but without stalling the remote end.  here we send an ack
		if we have received half the window, or else start the
		ack_delay timer, so as to assure transmitting an ack in the
		case where only a few frames are received.  if a frame is
		received which is outside the window then an ack is sent
		immediately because most likly a previous transmitted ack
		was not received by the remote end.  note, the ack 
		transmissions are driven by the remotes resend timer. 

	C. timers arapBLOCK() to forktype arap()
		its here where the resend timer causes a resend event
		to occur.  the actual resends are done by mnp4_sndchar()
		right out of the resend[] array.  arap() only needs
		to turn on the resend boolean (do_resend) and sent the 
		index (resend_r) of the resend[] array.

		but before the resend burst is initiated, a check for
		too many consecutive resend burst is made.  the max
		allowed is three at which point a disconnect will
		occur.

		also experation of the ack_delay timer will initate 
		transmission of AN ack by mnp4_sndchar() via the
		boolean send_ack.

******************************************************************************/

#define MNP4_MAXPACKETSIZE 259

/* MNP4 packet types */
#define MNP4_LR	1
#define MNP4_LD	2
#define MNP4_LT	4
#define MNP4_LT_H       0x84    
#define MNP4_LA	5
#define MNP4_LN	6
#define MNP4_LNA	7

/* MNP4 special characters */
#define ARAP1_MNP4_SYN     0x16
#define ARAP1_MNP4_DLE     0x10
#define ARAP2_MNP4_SYN     0x01
#define ARAP2_MNP4_DLE     0x1B
#define MNP4_STX     0x02
#define MNP4_ETX     0x03

/* MNP4 input/output state machine states */
#define MNP4_RXIDLE		1
#define MNP4_START_STAGE0	2
#define MNP4_START_STAGE1	3
#define MNP4_START_STAGE2	4
#define MNP4_RECEIVE		5
#define MNP4_DLE_RECEIVED	6
#define MNP4_CHECKSUM1		7
#define MNP4_CHECKSUM2		8
#define MNP4_RXERROR		9
#define MNP4_SEND		10
#define MNP4_END_STAGE		11
#define MNP4_DLE_SENT		12
#define MNP4_TXIDLE		13

/* these receive states are used to retain sync in the event of a 
 * getbuffer() failure 
 */
#define MNP4_RECEIVE_SHADOW		14
#define MNP4_DLE_RECEIVED_SHADOW	15
#define MNP4_CHECKSUM1_SHADOW		16
#define MNP4_CHECKSUM2_SHADOW		17

/* MNP4 packet fields and values */
#define MNP4_LENGTH 0
#define MNP4_TYPE 1

#define MNP4_LA_PAKLEN   4
#define MNP4_LD_PAKLEN   5
#define MNP4_LR_PAKLEN	 30
#define MNP4_LR_LA_BIT_SIZE (10*(2*7 + MNP4_LR_PAKLEN + MNP4_LA_PAKLEN))
#define MNP4_LR_LR_BIT_SIZE (10*(2*(7 + MNP4_LR_PAKLEN)))

/* (see V.42 Annex A page 66)
 * T401 = (2(k/2*10(7+259)+10(7+4)))/bps + latency    
 *      = (2(40(266)+110)/bps + latency                  
 *      = 21500/bps + latency
 */ 
/* DIDN'T USE the round trip delays of LR's & LA's to determine T401
 * but lets keep these comments anyway
 *
 * bps_la  = MNP4_LR_LA_BIT_SIZE/elapsed_time_sec
 * bps_lr  = MNP4_LR_LR_BIT_SIZE/elapsed_time_sec
 * 
 * T401sec_la = elapsed_time_sec*(21500+fudge)/MNP4_LR_LA_BIT_SIZE
 * T401sec_la = elapsed_time_sec*(45+fudge)
 * T401mil_la = elapsed_time_mil*56
 *
 * T401sec_lr = elapsed_time_sec*(21500+fudge)/MNP4_LR_LR_BIT_SIZE
 * T401sec_lr = elapsed_time_sec*(29+fudge)
 * T401mil_lr = elapsed_time_mil*37
 */

#define MNP4_T401_BAUD_CONSTANT (21500*ONESEC)	/* 21500*1000 in mil sec */
#define MNP4_T401_LATENCY ((ONESEC*3)/4)	/* 750 mil sec */
#define MNP4_MAX_BAUD 56000			/* max baud of modems */
#define MNP4_MAX_BAUD_FACT 8

#define MNP4_LT_SENDSEQ 2

#define MNP4_LD_TYPE    2
#define MNP4_LD_LEN     3
#define MNP4_LD_REASON  4
#define MNP4_LD_USER    255

#define MNP4_LA_RECVSEQ  2
#define MNP4_LA_RECVCRED 3

#define MNP4_MAXSENTQSIZE 60
#define MNP4_INITIATOR_RETRIES 4

#define MNP4_RESPONDER_RETRIES 2

#define MNP4_PUT_CHAR(ch) { *p++ = (ch); len += 1; }

/* Prototypes */
mnp4type *mnp4_setup(tt_soc *);
void mnp4_disable(tt_soc *);
void mnp4_destroy_setup(mnp4type *);
void mnp4_gotchar(tt_soc *, uchar *, int);
void mnp4_internal_output(mnp4type *, paktype *);
void mnp4_output(mnp4type *, queuetype *);
uint mnp4_sndchar(tt_soc *, uchar *, int);
boolean mnp4_input(mnp4type *, queuetype *);
boolean mnp4_overflowed(mnp4type *);
boolean mnp4BLOCK(mnp4type *);
int wait_for_mnp4_remote(mnp4type *, int);

extern void death(tt_soc *);
extern boolean mnp4_timers(mnp4type * mnp4);
extern void mnp4_send_ack(mnp4type *mnp4);
extern void mnp4_startoutput (mnp4type * mnp4);

extern boolean mnp4_logging;

/*
 * inlines
 */
/* mnp4_packets_pending()
 * used by protocol translation in vty_asyncBLOCK() /pt/vtyasync.c
 * which activates pt_manage_vtyasync() pt/vtyasync.c. which in turn
 * calls rx_tx_handler() /os/ttycon.c, and finally calls
 * len = (*(vty->txservice))(vty, buf, sizeof(buf))
 *
 * this was done for speed (instead of the modularity).
 * 
 * so one can say rx_tx_handler() is doing a psuedo interrupt at process level.
 * however, rx_tx_handler() can quite while mnp4_sndchar() still has output!
 *
 * HOWEVER, mnp4_sndchar() can be flow controlled! so depending on
 * the logic in mnp4_packets_pending() could return TRUE for quite 
 * some time and be a CPU HOG,
 *   ie, {
 *           if (QUEUESIZE(&mnp4->send_not_ltQ) || QUEUESIZE(&mnp4->send_ltQ)) 
 *		return (TRUE)
 *           if (mnp4->send_ack || mnp4->do_resend)
 *              return (TRUE)
 *       }
 *
 * instead mnp4_sndchar() & mnp4_startoutput() use prime_mnp4_outpak()
 * which puts a pak in mnp4->mnp4_outpak if there is output pending and
 * we are not flow controlled.
 *
 * so we have the tail wagging the dog situation here.
 */
static inline boolean mnp4_packets_pending (mnp4type *mnp4)
{
    if (mnp4->mnp4_outpak)
	return (TRUE);
    else
	return (FALSE);
}

#endif	/* MNP4_H__ */
