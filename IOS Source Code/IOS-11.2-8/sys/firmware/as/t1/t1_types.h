/* $Id: types.h,v 1.12.6.1 1997/02/21 07:41:09 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/types.h,v $
 *------------------------------------------------------------------
 * types.h  NEAT data types
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: types.h,v $
 * Revision 1.12.6.1  1997/02/21 07:41:09  jliu
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *    o Resolved CSCdi85835 bug and fix a problem in make.rule file
 *
 * Revision 1.12  1996/07/13 05:46:28  towu
 * o Changed T1InfoType to include fields to support transmit RBS status.
 *
 * Revision 1.11  1996/07/10  06:44:25  jliu
 *     	o Created another timer(250ms) for framing process and one second
 *           DSX1 performance data statistics in order to pass ACCUNET test
 * 	o Code changed for both AT&T and ANSI fdl test
 *         o Removed frame_mode in T1InfoType by using FrameType in frm_info
 *
 * Revision 1.10  1996/06/07  21:40:35  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.9  1996/05/09  21:25:57  towu
 * Added new feature Robbed bit signals.
 *
 * Revision 1.8  1996/05/06  06:01:09  jliu
 *     o Cleaned some code for LED on and off
 *     o Set DSX1_XMT_AIS in dsx1LineStatus after sending AIS
 *
 * Revision 1.7  1996/04/29  03:35:10  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.6  1996/04/20  02:37:45  jliu
 *     o Changed timer inetrrupt resolution from 50ms to 1 ms
 *
 * Revision 1.5  1996/04/14  20:49:53  jliu
 *     o clean up some comments
 *     o made some changes foe phone digits collection
 *
 * Revision 1.4  1996/03/02  03:51:25  towu
 * The basecode to support both mailbox and message queue for ipc.
 *
 * Revision 1.3  1996/02/26  06:25:58  jliu
 *      o The error counters should use the same type as the messages passed
 *        back to core CPU
 *
 * Revision 1.2  1996/02/22  22:27:52  jliu
 *     o update thge current T1 firmware for starting channelized T1 coding
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_TYPES_H
#define HAVE_TYPES_H


typedef unsigned short  SmGauge;
typedef unsigned long	Gauge;
typedef unsigned int	size_t;

typedef enum { FALSE = 0, TRUE }    bool;

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
#define CLEAR_COUNTER(a)	((a) = 0)
#define FORWARD_COUNTER(a,q)	((a) += (q))
#define COUNTER_EXPIRED(a,b)	((a) >= (b))
#define HAVE_COUNTER(a)		((a) > 0)
#define BACKWARD_COUNTER(a,q)	((a) -= (q))

#define FRAME_PROCESS           5   /* use it for 250ms frame process */
#define FRAMER_TIME	        1
#define FRAMER_TIMES_PER_SEC	4
#define T1_REM_ALARM_TIME	(2 * FRAMER_TIMES_PER_SEC)

#define QUARTER_HOURS_IN_DAY 96
#define MAX_GAUGE_COUNT 0xffff
#define MAX_SMGAUGE_COUNT 0xff
#define ADD_TO_GAUGE(a,b) { a += b; if ( a > MAX_GAUGE_COUNT ) a = MAX_GAUGE_COUNT; }

#define ADD_TO_SMGAUGE(a,b) { a += b; if ( a > MAX_SMGAUGE_COUNT ) a = MAX_SMGAUGE_COUNT; }

/*
 *  The following is really 2.5 * FRAMER_TIMES_PER_SEC, but avoids floating
 *  point math
 */
#define T1_FRAME_LOSS_TIME ((2*FRAMER_TIMES_PER_SEC)+(FRAMER_TIMES_PER_SEC/2))

/*
 * Do not move the definitions below to other place. You will get compile
 * error since the following data structures might rely on these definitions
 */
#define MAX_REQ_MESSAGE 30
#define MAX_RLY_MESSAGE 30
#define MAX_REPLY_DATA  230

typedef struct FDLRequestType {
    uchar has_message;    /* 1 means this buffer has FDL request message */
    uchar in_process;     /* 1 means this buffer is being processed */
    uchar length;         /* the length of the incoming FDL request message */
    uchar address;
    uchar control;
    uchar cmd;
    uchar org_unit_addr;
    uchar unit_addr;
    uchar request;
    ushort fcs;
} FDLRequestType;

typedef struct FDLReplyType {
    uchar *data_start;   /* the pointer always points to the next reply */
                         /* message byte to be sent */
    uchar has_message;   /* 1 means this buffer has FDL request message */
    uchar in_process;    /* 1 means this buffer is being processed */
    uchar which_half;    /* 0x0 means lower half, 0x80 means upper half */
    uchar length;
    uchar address;
    uchar control;
    uchar cmd;
    uchar status;
    uchar org_unit_addr;
    uchar unit_addr;
    uchar request;
    uchar cur_status;
    uchar data[MAX_REPLY_DATA];
} FDLReplyType;

/*
 *	Data structure containing information on the two framers 
 */
typedef struct frm_info {
    
    uint        rly_free_head;	   /*this point to next element in DL_Request*/
                                   /*array to store the incoming fdl request*/
    uint        rly_message_head;  /*this points to the element in DL_Request*/
                                   /*array which has fdl request and need to*/
    uint        req_free_head;	   /*this point to next element in DL_Request*/
                                   /*array to store the incoming fdl request*/
    uint        req_message_head;  /*this points to the element in DL_Request*/
                                   /*array which has fdl request and need to*/
                                   /*be served first*/

    FDLRequestType  DL_Request[MAX_REQ_MESSAGE]; /* FDL request array for */
                                                 /* storing request messages */
                                                 /* from external network */
    FDLReplyType    DL_Reply[MAX_RLY_MESSAGE];   /* FDL reply array for */
                                                 /* replying request messages */
    vushort     *framer;		/* point to the framer's registers */
    uchar	FrameType;		/* type of frame set */
    uchar	SecsForFrame;
    ushort      OtherErrors;
    ushort	SecsForMinute;
    ushort	BESinfailed;
    ushort	ESinfailed;
    ushort	BESlastinterval;
    ushort	ESlastinterval;
    ushort	SEScount;	   /* count of ses for unavail */

    ulong	CountsNoRemAlarm;
    ulong	CountsUntilFrame;
    ulong	CountsTowardLoss;
    ulong	CountsUntilLossClear;

    Gauge	BPVinMin;
    Gauge	CRCinMin;
    Gauge	BPVneededinMin;
    Gauge	CRCneededinMin;
    volatile ushort	FBEErrs;
    volatile ushort	PCVErrs;
    volatile ushort	CRCErrs;
    volatile ushort	BPVErrs;

    bool	IntrptLOS;
    bool	IntrptOOF;
    bool	HasLOS;			/* TRUE for LOS	*/
    bool	HasAIS;			/* TRUE for blue alarm	*/
    bool	HasFrame;
    bool	FrameAcquired;
    bool	FrameLossOccurred;
    bool	TxingRemAlarm;
    bool	TxingAis;
    bool	UnavailableState;
    bool	RxingRemAlarm;
    
} frm_info;


/************************************************************************
 *  types defined for the MIB - error counters and such			*
 ************************************************************************/


typedef struct dsx1ConfigType {
    int         dsx1LineIndex;                /* Not used */
    int         dsx1IfIndex;
    int	        dsx1TimeElapsed;
    int         dsx1ValidIntervals;
    int         dsx1LineType;
    int         dsx1LineCoding;
    int         dsx1SendCode;                 /* Not used */
    uchar       dsx1CircuitIdentifier[256];   /* Not used */
    int         dsx1LoopbackConfig;
    LineStatus  dsx1LineStatus;
    int         dsx1SignalMode;               /* Not used */
    int         dsx1TransmitClockSource;
    int         dsx1Fdl;                      /* Not used */
} dsx1ConfigType;

typedef struct dsx1TableType {
    SmGauge	dsx1Interval;
    SmGauge	dsx1ESs;
    SmGauge	dsx1UASs;
    SmGauge	dsx1BESs;
    SmGauge	dsx1SESs;
    SmGauge	dsx1CSSs;
    SmGauge	dsx1LOFCs;
} dsx1TableType;

typedef struct dsx1TotalTableType {
    Gauge   dsx1ESs;
    Gauge   dsx1UASs;
    Gauge   dsx1BESs;
    Gauge   dsx1SESs;
    SmGauge dsx1CSSs;
    SmGauge dsx1LOFCs;
} dsx1TotalTableType;

typedef struct dsx1MIBDataType {
    int                 CurrentIndex;   /* to see if indexes match with mip */
    dsx1ConfigType	Config;
    dsx1TableType	CurrentData;
    dsx1TotalTableType	TotalData;
    dsx1TableType       Intervals[QUARTER_HOURS_IN_DAY];
} dsx1MIBDataType;

typedef struct ansi_fdl_info {
    uchar       sapi;
    uchar       tei;
    uchar       control;
    uchar	message0_T;
    uchar	message1_T;
    uchar	message0_T1;
    uchar	message1_T1;
    uchar	message0_T2;
    uchar	message1_T2;
    uchar	message0_T3;
    uchar	message1_T3;
} ansi_fdl_info;

typedef struct T1InfoType {
    frm_info	       Fr;
    ansi_fdl_info      Ansi;
    dsx1MIBDataType    *Mib;
    vushort            *MailFromMPU;  /* This word memory includes 6 commands */
				      /* which are shown in message.h file */ 
    neat2mpu_mail_type *MailToMPU;    /* Messages reply to core CPU */
    carrier_mail_type  *Carrier;      /* report carrier state changed to core */
                                      /* CPU */
    vushort            *LIU_Control;  /* LIU controle register */

    ushort             MsgToSendToMPU;
    ushort             MsgReceivedFromMPU;
    ushort             LoopbackAct;

    int		unit;                 /* T1 interface unit number */
    uchar	clk_source;
    bool        frame_pro_need;	      /* need to do frame process in main loop */
    bool        att_lineloop;         /* True if AT&T line loopback activate */
                                      /* request was detected */
    bool        ansi_lineloop;        /* True if ANSI line loopback activate */
                                      /* request was detected */
    bool	InRemLoop;
    bool        Available;            /* carrier is available */
    uchar       RAI_resend;           /* resend  RAI */
    uchar       fdl_std;              /* AT&T TR54016 or ANSI T1.403 */
    uchar       fdl_resend_RAI;       /* if true, need to resend RAI after FDL */
                                      /* reply message sent was completed */
    uchar	RemoteLoopCount;
    LoopStatusType  RemoteLoopState;
    uint        crc_count;            /* This CRC count is for AT&T 54016 */
    ulong	ct1_map;	      /* bit map of channelized T1 channels */	
    bool	rx_rbs_report;	      /* Indicate we have to send rx rbs status
 					 back to mpu */
    bool	rx_rbs_changed;	      /* Indicate rx rbs status changed */
    bool	tx_rbs_report;	      /* Indicate we have to send tx rbs status
 					 back to mpu */
    bool	tx_rbs_changed;	      /* Indicate tx rbs status changed */
    
} T1InfoType;


typedef struct DTMFInfoType {
    dtmfmail_from_mpu 	*DTMFFromMPU;
    dtmfmail_to_mpu 	*DTMFToMPU;
    ushort      digit_1st_timer;      /* timer for first digit collection */
    ushort      digit_rx_done_timer;  /* timer for remainning digits */
                                      /* collection */
    bool        digit_col_req;        /* digits collected request from core */
                                      /* CPU */
    bool        digit_1st_timeout;    /* timeout before first digit received */
    bool        digit_1st_received;   /* first digit has been received */
    bool        digit_rx_done;        /* all digits were received */
    bool        dv_search;            /* if true, start checking dv bit */
}DTMFInfoType;


/*
 * This section of code describes all the internal registers.
 * The format is taken from the QUICC Users Manual and is as
 * follows:
 *
 *	Block_name;	description
 *
 * Thus the SIM moduale Configuration Register (mcr) would be
 * sim_mcr (see below).  Please do not violate this namespace.
 */
typedef struct QuiccInternalRegsType {
    /*
     * SIM - System Integration Module
     */
    volatile ulong	sim_mcr;	/* Module Configuration Register */
    volatile uchar	sim_res_0[0x4];	/* Reserved */
    volatile uchar	sim_avr;	/* Autovector Register */
    volatile uchar	sim_rsr;	/* Reset Status Register */
    volatile uchar	sim_res_1[0x2];	/* Reserved */
    volatile uchar	sim_clkocr;	/* CLKO Control Register */
    volatile uchar	sim_res_2[0x3];	/* Reserved */
    volatile ushort	sim_pllcr;	/* PLL Control Register */
    volatile uchar	sim_res_3[0x2];	/* Reserved */
    volatile ushort	sim_cdvcr;	/* Clock Divider Control Register */
    volatile ushort	sim_pepar;	/* Port E Pin Assignment Register */
    volatile uchar	sim_res_4[0xA];	/* Reserved */
    volatile uchar	sim_sypcr;	/* System Protection Control */
    volatile uchar	sim_swiv;	/* Software Interrupt Vector */
    volatile uchar	sim_res_5[0x2];	/* Reserved */
    volatile ushort	sim_picr;	/* Periodic Interrupt Control Register*/
    volatile uchar	sim_res_6[0x2];	/* Reserved */
    volatile ushort	sim_pitr;	/* Periodic Interrupt Timing Register */
    volatile uchar	sim_res_7[0x3];	/* Reserved */
    volatile uchar	sim_swsr;	/* Software Service Register */
    volatile ulong	sim_bkar;	/* Breakpoint Address Register */
    volatile ulong	sim_bkcr;	/* Breakpoint Control Register */
    volatile uchar	sim_res_8[0x8];	/* Reserved */
    /*
     * MEMC - Memory Controller
     */
    volatile ulong	memc_gmr;	/* Global Memory Register */
    volatile ushort	memc_mstat;	/* Memory Controller Status Register */
    volatile uchar	memc_res_0[0xA]; /* Reserved */
    volatile ulong	memc_br0;	/* Base Register 0 */
    volatile ulong	memc_or0;	/* Option Register 0 */
    volatile uchar	memc_res_1[0x8]; /* Reserved */
    volatile ulong	memc_br1;	/* Base Register 1 */
    volatile ulong	memc_or1;	/* Option Register 1 */
    volatile uchar	memc_res_2[0x8]; /* Reserved */
    volatile ulong	memc_br2;	/* Base Register 2 */
    volatile ulong	memc_or2;	/* Option Register 2 */
    volatile uchar	memc_res_3[0x8]; /* Reserved */
    volatile ulong	memc_br3;	/* Base Register 3 */
    volatile ulong	memc_or3;	/* Option Register 3 */
    volatile uchar	memc_res_4[0x8]; /* Reserved */
    volatile ulong	memc_br4;	/* Base Register 4 */
    volatile ulong	memc_or4;	/* Option Register 4 */
    volatile uchar	memc_res_5[0x8]; /* Reserved */
    volatile ulong	memc_br5;	/* Base Register 5 */
    volatile ulong	memc_or5;	/* Option Register 5 */
    volatile uchar	memc_res_6[0x8]; /* Reserved */
    volatile ulong	memc_br6;	/* Base Register 6 */
    volatile ulong	memc_or6;	/* Option Register 6 */
    volatile uchar	memc_res_7[0x8]; /* Reserved */
    volatile ulong	memc_br7;	/* Base Register 7 */
    volatile ulong	memc_or7;	/* Option Register 7 */
    volatile uchar	memc_res_8[0x438]; /* Reserved */
    /*
     * IDMA1 - Independent Direct Memory Access
     */
    volatile ushort	idma1_iccr;	/* Channel Configuration Register */
    volatile uchar	idma1_res_0[0x2]; /* Reserved */
    volatile ushort	idma1_cmr;	/* IDMA1 Mode Register */
    volatile uchar	idma1_res_1[0x2]; /* Reserved */
    volatile ulong	idma1_sapr;	/* IDMA1 Source Address Pointer */
    volatile ulong	idma1_dapr;	/* IDMA1 Destination Address Pointer*/
    volatile ulong	idma1_bcr;	/* IDMA1 Byte Count Register */
    volatile uchar	idma1_fcr;	/* IDMA1 Function Code Register */
    volatile uchar	idma1_res_2[0x1]; /* Reserved */
    volatile uchar	idma1_cmar;	/* Channel Mask Register */
    volatile uchar	idma1_res_3[0x1]; /* Reserved */
    volatile uchar	idma1_csr;	/* IDMA1 Channel Status Register */
    volatile uchar	idma1_res_4[0x3]; /* Reserved */
    /*
     * SDMA - Serial Direct Memory Access
     */
    volatile uchar	sdma_sdsr;	/* Status Register */
    volatile uchar	sdma_res_0[0x1]; /* Reserved */
    volatile ushort	sdma_sdcr;	/* Configuration Register */
    volatile ulong	sdma_sdar;	/* Address Register */
    /*
     * IDMA2 - Independent Direct Memory Access
     */
    volatile uchar	idma2_res_0[0x2]; /* Reserved */
    volatile ushort	idma2_cmr;	/* IDMA2 Mode Register */
    volatile ulong	idma2_sapr;	/* IDMA2 Source Address Pointer */
    volatile ulong	idma2_dapr;	/* IDMA2 Destination Address Pointer */
    volatile ulong	idma2_bcr;	/* IDMA2 Byte Count Register */
    volatile uchar	idma2_fcr;	/* IDMA2 Function Code Register */
    volatile uchar	idma2_res_1[0x1]; /* Reserved */
    volatile uchar	idma2_cmar;	/* Channel Mask Register */
    volatile uchar	idma2_res_2[0x1]; /* Reserved */
    volatile uchar	idma2_csr;	/* IDMA2 Channel Status Register */
    volatile uchar	idma2_res_3[0x7]; /* Reserved */
    /*
     * CPIC - Communications Processor Interrupt Controller
     */
    volatile ulong	cpic_cicr;	/* CP Interrupt Configuration Register*/
    volatile ulong	cpic_cipr;	/* CP Interrupt Pending Register */
    volatile ulong	cpic_cimr;	/* CP Interrupt Mask Register */
    volatile ulong	cpic_cisr;	/* CP Interrupt In-Service Register */
    /*
     * Parallel I/O
     */
    volatile ushort	pio_padir;	/* Port A Data Direction Register */
    volatile ushort	pio_papar;	/* Port A Pin Assignment Register */
    volatile ushort	pio_paodr;	/* Port A Open Drain Register */
    volatile ushort	pio_padat;	/* Port A Data Register */
    volatile uchar	pio_res_0[0x8];	/* Reserved */
    volatile ushort	pio_pcdir;	/* Port C Data Direction Register */
    volatile ushort	pio_pcpar;	/* Port C Pin Assignment Register */
    volatile ushort	pio_pcso;	/* Port C Special Options */
    volatile ushort	pio_pcdat;	/* Port C Data Register */
    volatile ushort	pio_pcint;	/* Port C Interrupt Control Register */
    volatile uchar	pio_res_1[0x16]; /* Reserved */
    /*
     * TIMER
     */
    volatile ushort	tmr_tgcr;	/* Timer Global Configuration Register*/
    volatile uchar	tmr_res_0[0xE];	/* Reserved */
    volatile ushort	tmr_tmr1;	/* Timer 1 Mode Register */
    volatile ushort	tmr_tmr2;	/* Timer 2 Mode Register */
    volatile ushort	tmr_trr1;	/* Timer 1 Reference Register */
    volatile ushort	tmr_trr2;	/* Timer 2 Reference Register */
    volatile ushort	tmr_tcr1;	/* Timer 1 Capture Register */
    volatile ushort	tmr_tcr2;	/* Timer 2 Capture Register */
    volatile ushort	tmr_tcn1;	/* Timer 1 Counter Register */
    volatile ushort	tmr_tcn2;	/* Timer 2 Counter Register */
    volatile ushort	tmr_tmr3;	/* Timer 3 Mode Register */
    volatile ushort	tmr_tmr4;	/* Timer 4 Mode Register */
    volatile ushort	tmr_trr3;	/* Timer 3 Reference Register */
    volatile ushort	tmr_trr4;	/* Timer 4 Reference Register */
    volatile ushort	tmr_tcr3;	/* Timer 3 Capture Register */
    volatile ushort	tmr_tcr4;	/* Timer 4 Capture Register */
    volatile ushort	tmr_tcn3;	/* Timer 3 Counter Register */
    volatile ushort	tmr_tcn4;	/* Timer 4 Counter Register */
    volatile ushort	tmr_ter1;	/* Timer 1 Event Register */
    volatile ushort	tmr_ter2;	/* Timer 2 Event Register */
    volatile ushort	tmr_ter3;	/* Timer 3 Event Register */
    volatile ushort	tmr_ter4;	/* Timer 4 Event Register */
    volatile uchar	tmr_res_1[0x8];	/* Reserved */
    /*
     * CP - Communications Processor
     */
    volatile ushort	cp_cr;		/* Command Register */
    volatile uchar	cp_res_0[0x2];	/* Reserved */
    volatile ushort	cp_rccr;	/* Main Configuration Register */
    volatile uchar	cp_res_1[0x10];	/* Reserved */
    volatile ushort	cp_rter;	/* RISC Timers Event Register */
    volatile uchar	cp_res_2[0x2];	/* Reserved */
    volatile ushort	cp_rtmr;	/* RISC Timers Mask Register */
    volatile uchar	cp_res_3[0x14];	/* Reserved */
    /*
     * BRG - Baud Rate Generator
     */
    volatile ulong	brg_brgc1;	/* BRG1 Configuration Register */
    volatile ulong	brg_brgc2;	/* BRG2 Configuration Register */
    volatile ulong	brg_brgc3;	/* BRG3 Configuration Register */
    volatile ulong	brg_brgc4;	/* BRG4 Configuration Register */
    /*
     * SCC - Serial Communications Controller
     */
    struct scc_regs {
	volatile ulong	scc_gsmrl;	/* SCC General Mode Register */
	volatile ulong	scc_gsmrh;	/* SCC General Mode Register */
	volatile ushort	scc_psmr;	/* Protocol Specific Mode Register */
	volatile uchar	scc_res_0[0x2];	/* Reserved */
	volatile ushort	scc_todr;	/* SCC Transmit on Demand */
	volatile ushort	scc_dsr;	/* SCC Data Sync Register */
	volatile ushort	scc_scce;	/* SCC Event Register */
	volatile uchar	scc_res_1[0x2];	/* Reserved */
	volatile ushort	scc_sccm;	/* SCC Mask Register */
	volatile uchar	scc_res_2[0x1];	/* Reserved */
	volatile uchar	scc_sccs;	/* SCC Status Register */
	volatile uchar	scc_res_3[0x8];	/* Reserved */
    } scc[0x4];
    /*
     * SMC - Management Controller
     */
    struct smc_regs {
	volatile uchar	smc_res_0[0x2];	/* Reserved */
	volatile ushort	smc_smcmr;	/* SMC Mode Register */
	volatile uchar	smc_res_1[0x2];	/* Reserved */
	volatile uchar	smc_smce;	/* SMC Event Register */
	volatile uchar	smc_res_2[0x3];	/* Reserved */
	volatile uchar	smc_smcm;	/* SMC Mask Register */
	volatile uchar	smc_res_3[0x5];	/* Reserved */
    } smc_regs[0x2];
    /*
     * SPI - Serial Peripheral Interface
     */
    volatile ushort	spi_spmode;	/* SPI Mode Register */
    volatile uchar	spi_res_0[0x4];	/* Reserved */
    volatile uchar	spi_spie;	/* SPI Event Register */
    volatile uchar	spi_res_1[0x3];	/* Reserved */
    volatile uchar	spi_spim;	/* SPI Mask Register */
    volatile uchar	spi_res_2[0x2];	/* Reserved */
    volatile uchar	spi_spcom;	/* SPI Command Register */
    volatile uchar	spi_res_3[0x4];	/* Reserved */
    /*
     * PIP - Parallel Interface Port
     */
    volatile ushort	pip_pipc;	/* PIP Configuration Register */
    volatile uchar	pip_res_0[0x2];	/* Reserved */
    volatile ushort	pip_ptpr;	/* PIP Timing Parameters Register */
    volatile ulong	pip_pbdir;	/* Port B Data Direction Register */
    volatile ulong	pip_pbpar;	/* Port B Pin Assignment Register */
    volatile uchar	pip_res_1[0x2];	/* Reserved */
    volatile ushort	pip_pbodr;	/* Port B Open Drain Register */
    volatile ulong	pip_pbdat;	/* Port B Data Register */
    volatile uchar	pip_res_2[0x18]; /* Reserved */
    /*
     * SI - Serial Interface
     */
    volatile ulong	si_simode;	/* SI Mode Register */
    volatile uchar	si_sigmr;	/* SI Global Mode Register */
    volatile uchar	si_res_0[0x1];	/* Reserved */
    volatile uchar	si_sistr;	/* SI Status Register */
    volatile uchar	si_sicmr;	/* SI Command Register */
    volatile uchar	si_res_1[0x4];	/* Reserved */
    volatile ulong	si_sicr;	/* SI Clock Routing */
    volatile uchar	si_res_3[0x2];	/* Reserved */
    volatile ushort	si_sirpt;	/* SI RAM Pointer Transmit */
    volatile ushort	si_sirpr;	/* SI RAM Pointer Receive */
    volatile uchar	si_res_4[0xA];	/* Reserved */
    volatile ushort	si_siram[0x80];	/* SI Routing RAM */
} QuiccInternalRegsType;

typedef struct SCCTransParamType {
    ushort      rbase;                  /* Rx BD Base Address */
    ushort      tbase;                  /* Tx BD Base Address */
    uchar       rfcr;                   /* Rx Function Code */
    uchar       tfcr;                   /* Tx Function Code */
    ushort      mrblr;                  /* Rx Buffer Length */
    ulong       rstate;                 /* Rx Internal State */
    ulong       rptr;                   /* Rx Internal Data Pointer */
    ushort      rbptr;                  /* Rb BD Pointer */
    ushort      rcount;                 /* Rx Internal Byte Count */
    ulong       rtemp;                  /* Rx Temp */
    ulong       tstate;                 /* Tx Internal State */
    ulong       tptr;                   /* Tx Internal Data Pointer */
    ushort      tbptr;                  /* Tx BD Pointer */
    ushort      tcount;                 /* Tx Byte Count */
    ulong       ttemp;                  /* Tx Temp */
} SCCTransParamType;

/*
 * quicc buffer descriptors.  These buffer descriptors are used
 * for all interfaces.
 */
typedef struct QuiccBdType {
    volatile ushort status;             /* Status and Control */
    volatile ushort length;             /* Length of Data in buffer */
    volatile uchar *buf_ptr;            /* Pointer to Data buffer */
} QuiccBdType;

#endif  /* HAVE_TYPES_H */
