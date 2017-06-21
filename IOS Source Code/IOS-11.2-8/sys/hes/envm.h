/* $Id: envm.h,v 3.3.60.1 1996/04/30 20:20:05 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/hes/envm.h,v $
 *------------------------------------------------------------------
 * envm.h - Environmental Monitor private definitions 
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: envm.h,v $
 * Revision 3.3.60.1  1996/04/30  20:20:05  gstovall
 * CSCdi56097:  7000 envm reports higher inlet temperature
 * Branch: California_branch
 * Make sure to use the right test points when getting the inlet
 * temperature.
 *
 * Revision 3.3  1995/11/17  09:15:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/21  07:40:00  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.2  1995/06/16 04:27:32  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:39:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/*
 * Env constants
 */
#define FORMAT_MSK              0x20
#define POLARITY_MSK            0x10
#define ENV_SHUTDOWN            0xC0
#define ENV_REFRESH             60   /* time between env. updates in sec. */
#define WARN_REFRESH            240  /* time between warning errmsgs in min. */
#define CELSIUS_BASELINE	151
#define ENV_MAGIC               0xf1e2
#define ENV_NV_VERSION          0x0005

/*
 * ENV_NVMAXBYTES is the number of bytes set aside in nvram for storing
 * enviromental monitor data.  Be VERY careful about changing the
 * structures that get stored to NVRAM.  Writing outside the defined
 * area could destroy the rom monitor environment variables.
 *
 * The 128K of NVRAM on the 70x0 is carved up as follows:
 *
 *   configuration data           127 Kbytes    11140000
 *   envm data                    602 bytes     1115FC00
 *   alignment padding              2 bytes     1115FE5A
 *   new rommon vars              415 bytes     1115FE5C
 *   confreg magic                  2 bytes     1115FFFB
 *   confreg value                  2 bytes     1115FFFD
 */
#define ENV_NVMAXBYTES          602

#define ENV_VALID		0
#define ENV_DIRTY		1
#define ENV_INVALID		2
#define NOT_INSTALLED		0x3
#define T_DESCRIPT_LEN          9   /* length of test point description     */
#define MAX_PWR_INSTALLED       2   /* max number of installed pwr supplies */

/* Valid ENVM states */
#define NORMAL		0
#define WARNING		1
#define CRITICAL	2
#define SHUTDOWN	3
#define NOT_PRESENT     4

/* ENVM Test Points (referenced by SHOW struct)                     */
#define ECTEST_TEST      0  /* Test                                 */
#define ECTEST_V_P12     1  /* +12 Volts                            */
#define ECTEST_V_P5      2  /* +5 Volts                             */
#define ECTEST_V_N12     3  /* -12 Volts                            */
#define ECTEST_V_P24     4  /* +24 Volts                            */
#define ECTEST_RES_1     5  /* Reserved                             */
#define ECTEST_RES_2     6  /* Reserved                             */
#define ECTEST_T_EXHAUST 7  /* Temperature Ambient (exhaust)        */
#define ECTEST_FAN_0     8  /* Status of fan 0                      */
#define ECTEST_FAN_1     9  /* Status of fan 1                      */
#define ECTEST_FAN_2     10 /* Status of fan 2                      */
#define ECTEST_FAN_3     11 /* Status of fan 3                      */
#define ECTEST_FAN_4     12 /* Status of fan 4                      */
#define ECTEST_FAN_5     13 /* Status of fan 5                      */
#define ECTEST_RES_3     14 /* Reserved                             */
#define ECTEST_T_INLET   15 /* Temperature Ambient (inlet)          */
#define ECTEST_NTESTS    ECTEST_T_INLET+1

#define CELSIUS(reading)	(reading - CELSIUS_BASELINE)
#define SENSE_BITS(reading)         ((reading >> 8) & 0xff)
#define PWR_FAIL_BIT(reading,num)   ((reading >> (18 + num)) & 0x1)

#define ENV_VOLT_LOW  0
#define ENV_VOLT_HIGH 1

/*
 * Power supply types
 */
#define C7000_PS_TYPE_700W_DC       1
#define C7000_PS_TYPE_700W_AC       2
#define C7010_PS_TYPE_600W_AC       1
#define C7010_PS_TYPE_600W_DC       2
typedef struct {
	char   t_description[T_DESCRIPT_LEN]; /* What's measured, "+5 Volts" */
	ulong  t_measured;    /* Measured value, "5.013(V)"                  */
        sys_timestamp t_clock;/* snapshot of msclock after warning condition */
        uchar  t_state;       /* State of measurement  ie. nor, warn, crit   */
        uchar  t_orig_state;  /* State at first indication of warning        */
} ectest_t;

typedef struct {
	char   powerfail_status;   /* shutdown status of power supply     */
	ulong  type;               /* power supply present bit            */
	char   *type_str;          /* power supply 1 string               */
	sys_timestamp clock;       /* snapshot of msclock after pwr-fail  */
} ecps_t;

/* ECSHOW Structure used for "show env" and SNMP                          */
struct ecshow_t_ {
    ecps_t ps[MAX_PWR_INSTALLED];
    sys_timestamp  ms_snapshot;  	    /* snapshot of msclock                 */
    char   date_time_snap[CURRENT_TIME_STRING_MINBUF]; /* date/time snapshot       */
    uchar  who_shutdown;                    /* index of sense that caused shutdown */
    uint   int_powerfail_count;             /* intermintent powerfail count        */
    char   pf_date_time_snap[CURRENT_TIME_STRING_MINBUF]; /* snapshot of calendar at int pwrfail */
    sys_timestamp int_pwr_fail_time;        /* time since last int.   pwr_fail     */
    ectest_t s_test[ECTEST_NTESTS];         /* There are many testpoints for ENVM  */
    uchar  last_fan_state;                  /* Cumulative state of 6 spinnaker fans*/
};

/* 
 * Warning to those who would change ecshow_t_ or ec_t.  Make sure
 * the combined size of ecshow_t_ and ec_t are not greater than
 * ENV_NVMAXBYTES
 */

/* EC struct */
typedef struct {
    ushort   magic;              /* Magic number                     */
    ushort   version;            /* version of ENVM data structure   */
    chassis_type_t chassis;      /* chassis type that wrote the data */
    ushort   checksum;           /* checksum                         */
    uchar    invalid;            /* valid indicator                  */
    uchar    *textbase;          /* pointer to ecshow_t              */
    ecshow_t *ec_show;           /* Info Strings for SHOW and SNMP   */
} ec_t;

typedef struct {
    uint   normal_low;     /* Lower bound for normal voltage */
    uint   normal_high;    /* Upper bound for normal voltage */
    uint   warning_low;    /* Lower bound for warning voltage */
    uint   warning_high;   /* Upper bound for warning voltage */
    uint   critical_low;   /* Lower bound for critical voltage */
    uint   critical_high;  /* Upper bound for critical voltage */
} voltage_bounds_t;

typedef struct {
    uint   normal_low;   /* Lower bound for normal temperature */
    uint   warning_low;  /* Lower bound for warning temperature */
    uint   critical_low; /* Lower bound for critical temperature */
    uint   shutdown_low; /* Lower bound for shutdown temperature */
} temp_bounds_t;

typedef struct {
    uint   voltage_line; /* Which voltage sense */
    ulong  lower_bound;  /* Lower bound we'll record */
    uint   range;        /* Spread in volts reported */
    uint   divisions;    /* # of 0.5(V) divisions recorded */
    uint   resolution;   /* (Hwd. resolution of measurement * 100) */
} print_bounds_t ;

ec_t    *ecp;                    /* Ptr to alloc'd Env. struct    */
ecshow_t *last_showp;            /* Ptr to last showp struct      */
ecshow_t *last_showp2;           /* Ptr to last showp struct      */
ecshow_t *lshowp;                /* Ptr to last showp struct      */
uchar   *last_env_data;          /* Ptr to last alloc'd Env. data */
uchar   *last_env_data2;         /* Ptr to last alloc'd Env. data */
boolean  ec_present;             /* Used by PrintHardware()       */

/*
 * Prototypes
 */
void show_env_table(ecshow_t *);
uint env_temp_bounds(int, int);
int convert_millivolts(u_long, int);
int getenvVoltThreshold(int, boolean);
int getenvSupplyState(int);
int getenvSupplySource(int);








