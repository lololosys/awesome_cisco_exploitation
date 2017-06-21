/* $Id: c7100_envm.h,v 3.2.58.4 1996/06/11 14:27:41 dcarroll Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_envm.h,v $
 *------------------------------------------------------------------
 * c7100_envm.h - Predator Environmental Monitor support
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_envm.h,v $
 * Revision 3.2.58.4  1996/06/11  14:27:41  dcarroll
 * CSCdi59664:  enable power supply fail interrupts
 * Branch: California_branch
 * Turn on power supply interrupts.
 *
 * Revision 3.2.58.3  1996/06/06  00:04:06  dcarroll
 * CSCdi58996:  envmon show commands need init state info
 * Branch: California_branch
 * Fix up envmon display messages.
 *
 * Revision 3.2.58.2  1996/05/21  20:44:14  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.58.1  1996/03/21  23:47:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/02/15  19:52:01  dcarroll
 * CSCdi49083:  Fix c7100 envm temp sensor support
 * Branch: ELC_branch
 * Add volage, temperature and snmp fixes.
 *
 * Revision 3.2.2.2  1995/12/08  19:21:30  dcarroll
 * Branch: ELC_branch
 * OIR and ENVMON improvements.
 *
 * Revision 3.2.2.1  1995/11/30  02:14:09  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean c7100_volt_valid (uint index);
extern char* c7100_volt_descr (uint index);
extern int c7100_volt_value (uint index);
extern uint c7100_volt_lowthresh (uint index);
extern uint c7100_volt_highthresh (uint index);
extern uint c7100_volt_lastvalue (uint index);
extern uint c7100_volt_state (uint index);
extern boolean c7100_temp_valid (uint index);
extern char* c7100_temp_descr (uint index);
extern uint c7100_temp_value (uint index);
extern uint c7100_temp_thresh (uint index);
extern uint c7100_temp_lastvalue (uint index);
extern uint c7100_temp_state (uint index);
extern boolean c7100_ps_valid (uint index);
extern char* c7100_ps_descr (uint index);
extern uint c7100_ps_state (uint index);
extern uint c7100_ps_source (uint index);
extern boolean c7100_fan_valid (uint index);
extern char* c7100_fan_descr (uint index);
extern uint c7100_fan_state (uint index);

#define C7100_MAX_NAME_LEN 30
#define MAX_ENVBUF_LEN 32
#define C7100_TEMP_SENSORS 4
#define C7100_VOLT_SENSORS 4
#define C7100_POWER_SUPPLIES 2
#define C7100_FANS 3
#define C7100_ENVMON_POLLING_ITEMS 7

/*
 * For fan status, we compare the inlet
 * temperature to the 3 outlet temperature
 * regions. If any of these delta measurements
 * drifts above a cutoff value, then it can
 * be assumed that the fan for that region
 * is blocked or broken.
 */
#define DS1620_WARMUP_TIME 20000
#define FAN0_MAX_DELTA 16
#define FAN1_MAX_DELTA 30
#define FAN2_MAX_DELTA 50

/* 
 * Message intervals 
 */
#define C7100_ENV_WARN_INTERVAL	ONEHOUR
#define C7100_ENV_CRIT_INTERVAL	(5*ONEMIN)

/* 
 * Define the sensors
 */
#define TEMP_INLET   0
#define TEMP_OUTLET0 1
#define TEMP_OUTLET1 2
#define TEMP_OUTLET2 3
#define P3V  0
#define P5V  1
#define P12V 2
#define N12V 3
#define PS0 0
#define PS1 1
#define FAN0 0
#define FAN1 1
#define FAN2 2

/*
 * Power supply ID ranges.
 */
#define PSTYPE_EMPTY_LOW   0
#define PSTYPE_EMPTY_HIGH  2
#define PSTYPE_1_LOW   3
#define PSTYPE_1_HIGH  5
#define PSTYPE_2_LOW   7
#define PSTYPE_2_HIGH  9
#define PSTYPE_3_LOW   11
#define PSTYPE_3_HIGH  13
#define PSTYPE_4_LOW   15
#define PSTYPE_4_HIGH  56
#define PSTYPE_5_LOW   66
#define PSTYPE_5_HIGH  107
#define PSTYPE_6_LOW   117
#define PSTYPE_6_HIGH  158
#define PSTYPE_7_LOW   168
#define PSTYPE_7_HIGH  209
#define PSTYPE_8_LOW   219
#define PSTYPE_8_HIGH  250
#define PSTYPE_9_LOW   252
#define PSTYPE_9_HIGH  254

/*
 * Temperature ranges for NPE-150: 75mhz r4k with sram
 */
#define NPE150_INLET_WARN_HIGH    (40 * 2) /* inlet    warning  temp - 40 C */
#define NPE150_INLET_CRIT_HIGH    (50 * 2) /* inlet    critical temp - 50 C */
#define NPE150_INLET_SHUT_HIGH    (75 * 2) /* inlet    shutdown temp - 75 C */
#define NPE150_OUTLET0_WARN_HIGH  (43 * 2) /* outlet 0 warning  temp - 43 C */
#define NPE150_OUTLET0_CRIT_HIGH  (53 * 2) /* outlet 0 critical temp - 53 C */
#define NPE150_OUTLET0_SHUT_HIGH  (58 * 2) /* outlet 0 shutdown temp - 58 C */
#define NPE150_OUTLET1_WARN_HIGH  (75 * 2) /* outlet 1 warning  temp - n/a C */
#define NPE150_OUTLET1_CRIT_HIGH  (75 * 2) /* outlet 1 critical temp - n/a C */
#define NPE150_OUTLET1_SHUT_HIGH  (75 * 2) /* outlet 1 shutdown temp - n/a C */
#define NPE150_OUTLET2_WARN_HIGH  (55 * 2) /* outlet 2 warning  temp - 55 C */
#define NPE150_OUTLET2_CRIT_HIGH  (65 * 2) /* outlet 2 critical temp - 65 C */
#define NPE150_OUTLET2_SHUT_HIGH  (70 * 2) /* outlet 2 shutdown temp - 70 C */

/*
 * NPE-100: 75mhz r4k, no sram
 */
#define NPE100_INLET_WARN_HIGH    (40 * 2) /* inlet    warning  temp - 40 C */
#define NPE100_INLET_CRIT_HIGH    (50 * 2) /* inlet    critical temp - 50 C */
#define NPE100_INLET_SHUT_HIGH    (75 * 2) /* inlet    shutdown temp - 75 C */
#define NPE100_OUTLET0_WARN_HIGH  (43 * 2) /* outlet 0 warning  temp - 43 C */
#define NPE100_OUTLET0_CRIT_HIGH  (53 * 2) /* outlet 0 critical temp - 53 C */
#define NPE100_OUTLET0_SHUT_HIGH  (58 * 2) /* outlet 0 shutdown temp - 58 C */
#define NPE100_OUTLET1_WARN_HIGH  (75 * 2) /* outlet 1 warning  temp - n/a C */
#define NPE100_OUTLET1_CRIT_HIGH  (75 * 2) /* outlet 1 critical temp - n/a C */
#define NPE100_OUTLET1_SHUT_HIGH  (75 * 2) /* outlet 1 shutdown temp - n/a C */
#define NPE100_OUTLET2_WARN_HIGH  (50 * 2) /* outlet 2 warning  temp - 50 C */
#define NPE100_OUTLET2_CRIT_HIGH  (60 * 2) /* outlet 2 critical temp - 60 C */
#define NPE100_OUTLET2_SHUT_HIGH  (65 * 2) /* outlet 2 shutdown temp - 65 C */

/*
 * Celcius to fahrenheit conversion. Consult your
 * local highschool physics text.
 */
#define CEL2FAHR(temp)         (32 + ((temp * 9)/5))

/*
 * As defined by the conversion routines for each of
 * the voltage levels, these numbers below represent the
 * cutoff ranges.
 *
 * These currently have the 3.45, 5.15 and 12.15 set to
 *    3% for warnings
 *    5% for criticals
 *
 * For the -11.95 signal they are
 *    10% for warnings
 *    20% for criticals
 *
 * To get the correct numbers use the following equations. The reason
 * for multiplying the precentage by two is that you can only measure 
 * the input from half of its expected input to one and one half its
 * expected input. So without multiplying by 2 you would only be 
 * moving away from the expected value at half the rate.
 *
 *     low = (128 - (128 * (percent * 2)))
 *     high = (128 + (128 * (percent * 2)))
 */

#define P3V_SHUT_LOW   52
#define P3V_CRIT_LOW  114
#define P3V_WARN_LOW  120
#define P3V_WARN_HIGH 136
#define P3V_CRIT_HIGH 142
#define P3V_SHUT_HIGH 205

#define P5V_SHUT_LOW  52
#define P5V_CRIT_LOW  114
#define P5V_WARN_LOW  120
#define P5V_WARN_HIGH 136
#define P5V_CRIT_HIGH 142
#define P5V_SHUT_HIGH 206

#define P12V_SHUT_LOW  52
#define P12V_CRIT_LOW  112
#define P12V_WARN_LOW  118
#define P12V_WARN_HIGH 138
#define P12V_CRIT_HIGH 144
#define P12V_SHUT_HIGH 206

#define N12V_SHUT_LOW  23
#define N12V_CRIT_LOW  76
#define N12V_WARN_LOW  102
#define N12V_WARN_HIGH 154
#define N12V_CRIT_HIGH 180
#define N12V_SHUT_HIGH 223

/*
 * Voltage conversion definitions
 */
#define C7100_A2D_SAMPLES 9
#define C7200_ENVMON_POLL_INTERVAL (1*ONESEC)
#define P5V_EXPECTED   128
#define C7100_VOLT_ONES_DIVIDER 1000
#define C7100_VOLT_HUNDREDTHS_DIVIDER 10

/*
 * These conversion equations take an A2D output (from 0 to 255)
 * and returns the millivolts measurement it represents.
 *
 * The A2D input voltage may be from 1.25v to 3.75v. All the
 * input voltages are passed through voltage dividers to put
 * them at 2.5v if there source voltages are correct. The 
 * following diagram shows the 3.45v input. The source 
 * voltage is divided down to 2.5v. A 2.5v input to the A2D
 * will generate an output of 128 (0x80). 
 *
 *    0  +3.45v
 *    |
 *    |
 *    >
 *    <
 *    >                 +-------------+
 *    |                 |             |
 *    |    from +2.5v   |   A2D       |
 *    +-----------------+             |
 *    |    from +5.15v  |             |   A2D out
 *    >    -------------+             +-------------
 *    <    from +12.15v |             |
 *    >    -------------+             |
 *    |    from -11.95v |             |
 *    |    -------------+             |
 *    |                 |             |
 *    |                 +-------------+
 *    =
 *
 * The A2D can track input voltages down to 1.25 volts and up to 
 * 3.75 volts, which means the source voltage could drift down to 
 * one half its expected value (1.725v for the 3.45 signal) and up 
 * to one and one half its expected value (5.175v for the 3.45
 * signal).
 *
 * To convert the A2D output back to the source voltage, the following
 * equation is used:
 * 
 *   millivolts = ((((A2D * ((ref_v / 256) * 100000)) + 125000) * ((source_v / ref_v) * 100)) / 10000)
 *
 * For the 3.45 source voltage the equation would be:
 *
 *   millivolts = ((((A2D * ((2.5 / 256) * 100000)) + 125000) * ((3.45 / 2.5) * 100)) / 10000)
 *
 * These equations reduce down quite a bit and result in the following definitions:
 */

#define C7100_3V_2_MV(x)       (((((x * 977) + 125000) * 138) / 10000))
#define C7100_5V_2_MV(x)       (((((x * 977) + 125000) * 206) / 10000))
#define C7100_P12V_2_MV(x)      (((((x * 977) + 125000) * 486) / 10000))
#define C7100_N12V_2_MV(x)      (((((x * 977) + 125000) * 478) / 10000))

#define MV_TO_UNITS(x)        (x / C7100_VOLT_ONES_DIVIDER)
#define MV_TO_HUNDREDTHS(x)   ((x % C7100_VOLT_ONES_DIVIDER) / C7100_VOLT_HUNDREDTHS_DIVIDER)

/*
 * A2D MUX Select definitions.
 */
#define PS0_MUX_SELECT  0x00
#define PS1_MUX_SELECT  0x02
#define P3V_MUX_SELECT  0x04
#define P12V_MUX_SELECT 0x08
#define P5V_MUX_SELECT  0x0A
#define N12V_MUX_SELECT 0X0C
#define REF_MUX_SELECT  0x0E

/*
 * DALLAS DS1620 Command Codes
 */
#define DS1620_READ_TEMP     0xAA
#define DS1620_WRITE_TH      0x01
#define DS1620_WRITE_TL      0x02
#define DS1620_READ_TH       0xA1
#define DS1620_READ_TL       0xA2
#define DS1620_START_CONVT   0xEE
#define DS1620_STOP_CONVT    0x22
#define DS1620_WRITE_CONFIG  0x0C
#define DS1620_READ_CONFIG   0xAC

#define DS1620_NEG_TEMP 0x100
#define DS1620_POS_TEMP 0xff

#define DS1620_CONFIG_STATUS_DONE   0x80
#define DS1620_CONFIG_STATUS_THF    0x40
#define DS1620_CONFIG_STATUS_TLF    0x20
#define DS1620_CONFIG_STATUS_CPU    0x02
#define DS1620_CONFIG_STATUS_1SHOT  0x01

#define DS1620_WRITE_SIZE 8
#define DS1620_CONFIG_READ_SIZE 8
#define DS1620_DATA_READ_SIZE 9

#define DS1620_RESET_OFF 0x0f
#define DS1620_RESET_ON  0x00
#define DS1620_CLK_LOW   0x0f
#define DS1620_CLK_HIGH  0x1f

#define DS1620_DATA_HIGH 0x11
#define DS1620_DATA_LOW  0x10
#define DS1620_ENABLE_READ 0x00

/*
 * "show env last" struct.
 */
typedef struct envm_stats_t_ {
    ushort magic;                               /* 00 */
    ushort temp[C7100_TEMP_SENSORS];            /* 02 */
    ushort volt[C7100_VOLT_SENSORS];            /* 0a */
    ushort ps[C7100_POWER_SUPPLIES];            /* 12 */
    ushort shutdown_reason;                     /* 16 */
    /*
     * Add new fields here; xsum must be last.
     */
    ushort xsum;                                /* 18 */
                                                /* 1a */
} envm_stats_t;

#define ENVM_STATS_MAGIC                (('E' << 8) | ('S'))

/*
 * This define controls how often the last environmental stats
 * will be saved.  If this value is 5, then the stats will be
 * saved every 5th iteration of the main process loop
 */
#define ENVM_SAVE_LAST_CNT              5

/*
 * The first 32 bytes of the eeprom is for the system info.
 * (i.e. the struct c7100_mpeeprom_copy_t)
 */
#define ENVM_STATS_EEPROM_OFFSET  16      /* in 2-byte words */

