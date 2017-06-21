/* $Id: c7100_envm.c,v 3.2.56.7 1996/08/28 13:11:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_envm.c,v $
 *------------------------------------------------------------------
 * c7100_envm.c - Predator Environmental Monitor support
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_envm.c,v $
 * Revision 3.2.56.7  1996/08/28  13:11:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.56.6  1996/06/11  14:27:37  dcarroll
 * CSCdi59664:  enable power supply fail interrupts
 * Branch: California_branch
 * Turn on power supply interrupts.
 *
 * Revision 3.2.56.5  1996/06/06  00:04:00  dcarroll
 * CSCdi58996:  envmon show commands need init state info
 * Branch: California_branch
 * Fix up envmon display messages.
 *
 * Revision 3.2.56.4  1996/05/23  19:50:04  dcarroll
 * CSCdi58439:  Disable fan checking
 * Branch: California_branch
 * Disable fan checking
 *
 * Revision 3.2.56.3  1996/05/21  20:44:09  dcarroll
 * CSCdi55856:  %ENVM-3-NOFORK: Could not start environment polling process
 * Branch: California_branch
 * Turn the envmon process back on.
 *
 * Revision 3.2.56.2  1996/05/21  10:03:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.56.1  1996/03/21  23:47:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/02/15  19:51:56  dcarroll
 * CSCdi49083:  Fix c7100 envm temp sensor support
 * Branch: ELC_branch
 * Add volage, temperature and snmp fixes.
 *
 * Revision 3.2.2.7  1996/02/08  09:26:49  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/16  02:16:45  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.5  1995/12/15  18:42:56  dcarroll
 * Branch: ELC_branch
 * Remove envmon code from boot image. Use common
 * envmon parser code.
 *
 * Revision 3.2.2.4  1995/12/11  19:23:05  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.3  1995/12/09  22:52:12  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.2.2.2  1995/12/08  19:21:24  dcarroll
 * Branch: ELC_branch
 * OIR and ENVMON improvements.
 *
 * Revision 3.2.2.1  1995/11/30  02:14:02  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "logger.h"
#include "../src-4k-c7100/msg_c7100.c"	/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "sched.h"
#include "interface_private.h"
#include "ctype.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "c7100_envm.h"
#include "../ui/debug_macros.h"
#include "../ui/debug.h"

#include "../dev/monitor1.h"
#include "../hes/sr_envmibdefs.h"
#include "../hes/envmib_registry.h"

#include "../snmp/sr_old_lchassismib.h"
#include "../snmp/sr_old_lchassismibdefs.h"

#include "c7100.h"
#include "../hes/if_fci.h"
#include "../hes/envmon_parser.h"
#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../pas/pas_id.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "c7100_fpga.h"
#include "c7100_pcmap.h"
#include "c7100_macaddr.h"

pid_t c7100_envmon_pid;

boolean envm_debug_a2d = FALSE;
boolean envm_debug_ps = FALSE;
boolean envm_debug_fan = FALSE;
boolean envm_debug_convert = TRUE;

/*****************
 * Common
 *****************/
/*
 * Shutdown reason codes
 */
typedef enum {
    REASON_PS = 0,
    REASON_TEMP,
    REASON_VOLT,
    REASON_TEST,
    REASON_UNDEFINED,
} env_reason_t;

env_reason_t shutdown_reason, shutdown_reason_last;

static char* const shutdown_reason_name_array[] = {
    "power supply shutdown",
    "critical temperature",
    "critical voltage",
    "test shutdown",
    "shutdown undefined",};

/*
 * Envirnomental States
 */
typedef enum {
    ENV_STATE_NORMAL = 0,
    ENV_STATE_WARNING,
    ENV_STATE_CRITICAL,
    ENV_STATE_SHUTDOWN,
    ENV_STATE_NOT_PRESENT,
} env_state_t;

static char* const env_state_name_array[] = {
    "normal",
    "warning",
    "critical",
    "shutdown",
    "not present",};

typedef char env_name_buf[C7100_MAX_NAME_LEN];

static env_name_buf inlet_name, outlet0_name;
static env_name_buf outlet1_name, outlet2_name;
static env_name_buf p3v_name, p5v_name, p12v_name, n12v_name;
static env_name_buf ps0_name, ps1_name;
static env_name_buf fan0_name, fan1_name, fan2_name;

/*****************
 * Temperatures
 *****************/
/* 
 * struct for temperature sensors
 */
typedef struct temptab_ {
    char *name;
    uint warn_high;
    uint crit_high;
    uint shut_high;
    uint current_val;
    uint last_val;
    env_state_t state;
    sys_timestamp warn_suppress;
    sys_timestamp crit_suppress;
} temptab_t;

static temptab_t temp_table_npe150[C7100_TEMP_SENSORS] = {
    {inlet_name,
	 NPE150_INLET_WARN_HIGH,
	 NPE150_INLET_CRIT_HIGH,
	 NPE150_INLET_SHUT_HIGH},
    {outlet0_name,
	 NPE150_OUTLET0_WARN_HIGH,
	 NPE150_OUTLET0_CRIT_HIGH,
	 NPE150_OUTLET0_SHUT_HIGH},
    {outlet1_name,
	 NPE150_OUTLET1_WARN_HIGH,
	 NPE150_OUTLET1_CRIT_HIGH,
	 NPE150_OUTLET1_SHUT_HIGH,},
    {outlet2_name,
	 NPE150_OUTLET2_WARN_HIGH,
	 NPE150_OUTLET2_CRIT_HIGH,
	 NPE150_OUTLET2_SHUT_HIGH,}};
         
static temptab_t temp_table_npe100[C7100_TEMP_SENSORS] = {
    {inlet_name,
	 NPE100_INLET_WARN_HIGH,
	 NPE100_INLET_CRIT_HIGH,
	 NPE100_INLET_SHUT_HIGH},
    {outlet0_name,
	 NPE100_OUTLET0_WARN_HIGH,
	 NPE100_OUTLET0_CRIT_HIGH,
	 NPE100_OUTLET0_SHUT_HIGH},
    {outlet1_name,
	 NPE100_OUTLET1_WARN_HIGH,
	 NPE100_OUTLET1_CRIT_HIGH,
	 NPE100_OUTLET1_SHUT_HIGH,},
    {outlet2_name,
	 NPE100_OUTLET2_WARN_HIGH,
	 NPE100_OUTLET2_CRIT_HIGH,
	 NPE100_OUTLET2_SHUT_HIGH,}};
         
static temptab_t *temp_table;

/*****************
 * Voltages
 *****************/
typedef uint voltage_format_t(char* buf, uint value);
typedef int voltage_convert_t(uint value);

/* 
 * struct for voltage sensing
 */
typedef struct volttab_ {
    char *name;
    uchar warn_low;
    uchar warn_high;
    uchar crit_low;
    uchar crit_high;
    uchar shut_low;
    uchar shut_high;
    uchar mux_select;
    voltage_format_t* format;
    voltage_convert_t* convert;
    uint current_val;
    uint last_val;
    env_state_t state;
    sys_timestamp warn_suppress;
    sys_timestamp crit_suppress;
} volttab_t;

static voltage_format_t c7100_p3v_format, c7100_p5v_format;
static voltage_format_t c7100_p12v_format, c7100_n12v_format;
static voltage_convert_t c7100_p3v_convert, c7100_p5v_convert;
static voltage_convert_t c7100_p12v_convert, c7100_n12v_convert;

static volttab_t volt_table[C7100_VOLT_SENSORS] = {
    {p3v_name,
	 P3V_WARN_LOW,
	 P3V_WARN_HIGH,
	 P3V_CRIT_LOW,
	 P3V_CRIT_HIGH,
	 P3V_SHUT_LOW,
	 P3V_SHUT_HIGH,
	 P3V_MUX_SELECT,
         c7100_p3v_format,
         c7100_p3v_convert,},
    {p5v_name,
	 P5V_WARN_LOW,
	 P5V_WARN_HIGH,
	 P5V_CRIT_LOW,
	 P5V_CRIT_HIGH,
	 P5V_SHUT_LOW,
	 P5V_SHUT_HIGH,
	 P5V_MUX_SELECT,
         c7100_p5v_format,
         c7100_p5v_convert,},
    {p12v_name,
	 P12V_WARN_LOW,
	 P12V_WARN_HIGH,
	 P12V_CRIT_LOW,
	 P12V_CRIT_HIGH,
	 P12V_SHUT_LOW,
	 P12V_SHUT_HIGH,
	 P12V_MUX_SELECT,
         c7100_p12v_format,
         c7100_p12v_convert,},
    {n12v_name,
	 N12V_WARN_LOW,
	 N12V_WARN_HIGH,
	 N12V_CRIT_LOW,
	 N12V_CRIT_HIGH,
	 N12V_SHUT_LOW,
	 N12V_SHUT_HIGH,
	 N12V_MUX_SELECT,
         c7100_n12v_format,
         c7100_n12v_convert,}};

/*****************
 * Power Supplies
 *****************/
typedef enum {
    PSTYPE_EMPTY = 0,
    PSTYPE_1,
    PSTYPE_2,
    PSTYPE_3,
    PSTYPE_4,
    PSTYPE_5,
    PSTYPE_6,
    PSTYPE_7,
    PSTYPE_8,
    PSTYPE_9,
    PSTYPE_UNKNOWN,
} pstype_t;

/*
 * struct for the power supplies
 */
typedef struct pstab_ {
    char *name;
    uchar mux_select;
    uint current_val;
    uchar last_val;
    pstype_t pstype;
    env_state_t state;
    boolean init;
} pstab_t;

static char* const pstype_name_array[] = {
    "removed",
    "power supply type 1",
    "power supply type 2",
    "power supply type 3",
    "power supply type 4",
    "power supply type 5",
    "Zytek AC Power Supply",
    "Zytek DC Power Supply",
    "power supply type 8",
    "power supply type 9",
    "unknown",};

static pstab_t ps_table[C7100_POWER_SUPPLIES] = {
    {ps0_name,
	 PS0_MUX_SELECT,},
    {ps1_name,
	 PS1_MUX_SELECT,}};
	 
/*****************
 * Fans
 *****************/
/*
 * struct for the fans
 */
typedef struct fantab_ {
    char *name;
    uchar max_delta;
    int delta;
    uint current_val;
    uchar last_val;
    env_state_t state;
    sys_timestamp warn_suppress;
} fantab_t;

static fantab_t fan_table[C7100_FANS] = {
    {fan0_name,
	 FAN0_MAX_DELTA,},
    {fan1_name,
	 FAN1_MAX_DELTA,},
    {fan2_name,
	 FAN2_MAX_DELTA,}};
	 
/*****************
 * Other globals
 *****************/
boolean envmib_load = FALSE;
static sys_timestamp warmup_timer;
static boolean warm;
static boolean lastenv_failure, lastenv_available;
static boolean c7100_broken_ps = FALSE;
static boolean c7100_include_fans = FALSE;

/*****************************************
 * UTILITY ROUTINES
 ****************************************/
/*
 * ds1620_2_celcius
 *
 * Take a reading from the dallas DS1620 part and convert it to 
 * a temperature in Celcius. The input is a 9 bit reading from the
 * dallas part. Its format is the following:
 *
 *     N T T T T T T T T
 *
 * N = Negative temperature
 * T = Temperature in 1/2 degree increments
 *
 * Examples: 0x032 = +25 C
 *           0x1ce = -25 C
 *
 */

static int ds1620_2_celcius (int in_temp) {
int tmp;

    if (in_temp & 0x100) {
	in_temp &= 0xff;
	in_temp = in_temp ^ 0xff;
	in_temp++;
	in_temp *= -1;
    }
    tmp = in_temp >> 1;
    return (tmp);
}

/*
 * c7100_temp_format
 *
 * Format a string with the temperature in celcius
 * and Farhenheit.
 *
 */

static uint c7100_temp_format (char* buf, int val) {
    int cel, fahr;

    cel = ds1620_2_celcius(val);
    fahr = CEL2FAHR(cel);
    return sprintf(buf, "%dC/%dF", cel, fahr);
}

/*
 * c7100_p3v_convert
 *
 * Convert an a2d reading to a 3v measurement.
 *
 */

static int c7100_p3v_convert (uint value) {
    return C7100_3V_2_MV(value);
}

/*
 * c7100_p3v_format
 *
 * Convert an a2d reading to a 3v measurement
 * then format it to be printf'd.
 *
 */

static uint c7100_p3v_format (char* buf, uint value) {
    int mv;

    mv = C7100_3V_2_MV(value);
    return sprintf(buf, "+%d.%02u", MV_TO_UNITS(mv), MV_TO_HUNDREDTHS(mv));
}

/*
 * c7100_p5v_convert
 *
 * Convert an a2d reading to a 5v measurement.
 *
 */

static int c7100_p5v_convert (uint value) {
    return C7100_5V_2_MV(value);
}

/*
 * c7100_p5v_format
 *
 * Convert an a2d reading to a 5v measurement
 * then format it to be printf'd.
 *
 */

static uint c7100_p5v_format (char* buf, uint value) {
    int mv;
    mv = C7100_5V_2_MV(value);
    return sprintf(buf, "+%d.%02u", MV_TO_UNITS(mv), MV_TO_HUNDREDTHS(mv));
}

/*
 * c7100_p12v_convert
 *
 * Convert an a2d reading to a +12v measurement.
 *
 */

static int c7100_p12v_convert (uint value) {
    return C7100_P12V_2_MV(value);
}

/*
 * c7100_n12v_convert
 *
 * Convert an a2d reading to a -12v measurement.
 *
 */

static int c7100_n12v_convert (uint value) {
    return -(C7100_N12V_2_MV(value));
}

/*
 * c7100_p12v_format
 *
 * Convert an a2d reading to a +12v measurement
 * then format it to be printf'd.
 *
 */

static uint c7100_p12v_format (char* buf, uint value) {
    int mv;

    mv = C7100_P12V_2_MV(value);
    return sprintf(buf, "+%d.%02u", MV_TO_UNITS(mv), MV_TO_HUNDREDTHS(mv));
}

/*
 * c7100_n12v_format
 *
 * Convert an a2d reading to a -12v measurement
 * then format it to be printf'd.
 *
 */

static uint c7100_n12v_format (char* buf, uint value) {
    int mv;

    mv = C7100_N12V_2_MV(value);
    return sprintf(buf, "-%d.%02u", MV_TO_UNITS(mv), MV_TO_HUNDREDTHS(mv));
}

/*************************************
 * SENSOR ACCESS ROUTINES
 ************************************/
/*
 * write_ds1620_bit
 *
 * Write a bit of data to the thermostats
 *
 */

static void write_ds1620_bit (int data_bits) {
    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_LOW;
    C7100_IO_FPGA->temp_data_write = data_bits;
    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_HIGH;
}

/*
 * read_ds1620_bits
 *
 * Read a bit from each of the thermostats
 *
 */

static uint read_ds1620_bits (void) {
    uint temp;

    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_LOW;
    temp = C7100_IO_FPGA->temp_data_read;
    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_HIGH;
    return(temp);
}

/*
 * write_ds1620_byte   
 *
 * write a byte of data to the thermostats
 *
 */

static void write_ds1620_byte (uint command, uint write_data, boolean config) {
    int i, data_bits;
    
    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_HIGH;
    for (i = 0; i < DS1620_WRITE_SIZE; i++) {
	if (command & 1) {
	    data_bits = DS1620_DATA_HIGH;
	}
	else {
	    data_bits = DS1620_DATA_LOW;
	}
	write_ds1620_bit(data_bits);
	command = command >> 1;
    }
    for (i = 0; i < DS1620_WRITE_SIZE; i++) {
	if (write_data & 1) {
	    data_bits = DS1620_DATA_HIGH;
	}
	else {
	    data_bits = DS1620_DATA_LOW;
	}
	write_ds1620_bit(data_bits);
	write_data = write_data >> 1;
    }
    if (!config)
	C7100_IO_FPGA->temp_ctrl = DS1620_RESET_ON;
}

/*
 * write_ds1620_cmd
 *
 * write a command to the thermostats
 *
 */

static void write_ds1620_cmd (uint command) {
    int i, data_bits;
    
    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_HIGH;
    for (i = 0; i < DS1620_WRITE_SIZE; i++) {
	if (command & 1) {
	    data_bits = DS1620_DATA_HIGH;
	}
	else {
	    data_bits = DS1620_DATA_LOW;
	}
	write_ds1620_bit(data_bits);
	command = command >> 1;
    }
    C7100_IO_FPGA->temp_ctrl = DS1620_RESET_ON;
}

/*
 * read_ds1620_byte
 *
 * Actually, it reads as many bits as you specify. It is
 * usually 8 or 9 bits since the ds1620 configuration register
 * is 8 bits and a temperature reading is 9 bits.
 *
 * This routine must also demux the data from the 4 
 * thermostats and put them in the right place.
 *
 */

static void read_ds1620_byte (uint command, int *array, int count) {
    int i, j, data_bits;
    uint temp, *temp_array;

    C7100_IO_FPGA->temp_ctrl = DS1620_CLK_HIGH;
    for (i = 0; i < DS1620_WRITE_SIZE; i++) {
	if (command & 1) {
	    data_bits = DS1620_DATA_HIGH;
	}
	else {
	    data_bits = DS1620_DATA_LOW;
	}
	write_ds1620_bit(data_bits);
	command = command >> 1;
    }

    C7100_IO_FPGA->temp_data_write = DS1620_ENABLE_READ;
    for (i = 0; i < C7100_TEMP_SENSORS; i++) {
	temp_array = array + i;
	*temp_array = 0;
    }
    for (i = 0; i < count; i++) {
	temp = read_ds1620_bits();
	for (j = 0; j < C7100_TEMP_SENSORS; j++) {
	    temp_array = array + j;
	    *temp_array = ((((temp >> j) & 1) << i) | *temp_array);
	}
    }
    C7100_IO_FPGA->temp_ctrl = DS1620_RESET_ON;
}

/*
 * select_mux
 *
 * Chose the desired input to the a2d part.
 *
 */
static void select_mux (uchar select) {
    if (envm_debug_a2d) {
	buginf("\nselected mux = %x", select);
    }
    C7100_IO_FPGA->temp_data_write = (uchar) select;
}

/*
 * read_a2d
 *
 * Get the data from the a2d part.
 *
 */
static uint read_a2d (void) {
    uint data, ix;

    data = 0;
    for (ix = 0; ix < C7100_A2D_SAMPLES; ix++) {
	data += C7100_IO_FPGA->env_ad_converter;
	process_sleep_for(C7200_ENVMON_POLL_INTERVAL);
    }
    data = data / C7100_A2D_SAMPLES;
    if (envm_debug_a2d) {
	buginf("\n read a2d = %d", data);
    }
    return(data);
}

/***********************************************
 * ENVIRONMENT HEALTH DETERMINATION
 **********************************************/

/*
 * save_last_envm_stats
 *
 * Store the environmental stats to the midplane eeprom.
 *
 * Note: calling this function is fairly expensive (about 200
 *       milliseconds).
 */

static void save_last_envm_stats (void) {
    int ix;
    envm_stats_t stats;
    boolean write_ok;

    stats.magic = ENVM_STATS_MAGIC;

    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
        stats.temp[ix] = temp_table[ix].current_val;
    }

    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
        stats.volt[ix] = volt_table[ix].current_val;
    }

    for (ix = 0; ix < C7100_POWER_SUPPLIES; ix++) {
        stats.ps[ix] = ps_table[ix].current_val;
    }

    stats.shutdown_reason = shutdown_reason;

    stats.xsum = 0;
    stats.xsum = ipcrc((ushort *)&stats, sizeof(stats) / 2);

    write_ok = c7100_write_mpeeprom_partial(&stats,
                                            sizeof(envm_stats_t) / 2,
                                            ENVM_STATS_EEPROM_OFFSET);

    if (! write_ok) {
	if (!lastenv_failure) {
	    errmsg(&msgsym(LASTENV, ENVM), "Cannot save environmental data");
	    lastenv_failure = TRUE;
	}
    }
}

/*
 * monitor_shutdown
 * 
 * Initiate a monitor shutdown of both power suppies.
 *
 */
static void monitor_shutdown (env_reason_t reason) {
 
    shutdown_reason = reason;
    
    /*
     * Print a message saying the box is being shutdown
     */
    errmsg(&msgsym(SHUT, ENVM));
    
    /*
     * We're about to drop the box, so flush any pending error
     * messages.
     */
    logger_flush();
    
    /*
     * Save the environmental stats (viewed by "show env last").
     */
    save_last_envm_stats();
    
    reg_invoke_envmib_shutdown_trap();
    
    /*
     * Turn off the power supplies
     */
    if (!c7100_broken_ps) {
        C7100_MP_FPGA->pa_tst_port_nsw &= ~POWER_SUPPLY_0_ENABLE;
        C7100_MP_FPGA->pa_tst_port_nsw &= ~POWER_SUPPLY_1_ENABLE;
    }
}

/*
 * get_new_ds1620_temps()
 * 
 * Get sensed temperatures from the ds1620 thermostats
 * in the Predator system. Convert them into Celcius
 * temperatures, ready to be used to determine system
 * health.
 *
 */
static void get_new_ds1620_temps (void) {
    int ix;
    uint temp_array[4];

    process_sleep_for(C7200_ENVMON_POLL_INTERVAL);
    /* 
     * Now get the temperatures.
     */
    read_ds1620_byte(DS1620_READ_TEMP, temp_array, DS1620_DATA_READ_SIZE);
    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	temp_table[ix].current_val = temp_array[ix];
    }
}

/*
 * check_ds1620_temps
 * 
 * Determine the system thermal state.
 *
 */
static void check_ds1620_temps (void) {
    int ix;
    uint val;
    char envbuf[MAX_ENVBUF_LEN];

    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	val = temp_table[ix].current_val;
	if (TIMER_RUNNING_AND_AWAKE(temp_table[ix].warn_suppress))
	    TIMER_STOP(temp_table[ix].warn_suppress);
	if (TIMER_RUNNING_AND_AWAKE(temp_table[ix].crit_suppress))
	    TIMER_STOP(temp_table[ix].crit_suppress);

	if ((val >= temp_table[ix].shut_high) &&
	    !(val & DS1620_NEG_TEMP)) {
	    temp_table[ix].state = ENV_STATE_SHUTDOWN;
	} else if ((val >= temp_table[ix].crit_high) &&
	    !(val & DS1620_NEG_TEMP)) {
	    temp_table[ix].state = ENV_STATE_CRITICAL;
	} else if ((val >= temp_table[ix].warn_high) &&
		   !(val & DS1620_NEG_TEMP)) {
	    temp_table[ix].state = ENV_STATE_WARNING;
	} else {
	    temp_table[ix].state = ENV_STATE_NORMAL;
	}
	switch (temp_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    break;
	  case ENV_STATE_WARNING:
	    if (!TIMER_RUNNING(temp_table[ix].warn_suppress)) {
		c7100_temp_format(envbuf, val);
		errmsg(&msgsym(ENVWARN, ENVM), temp_table[ix].name, envbuf);
		reg_invoke_envmib_temperature_trap(ix+1);
		TIMER_START(temp_table[ix].warn_suppress, C7100_ENV_WARN_INTERVAL);
	    }
	    break;
	  case ENV_STATE_CRITICAL:
	    if (!TIMER_RUNNING(temp_table[ix].crit_suppress)) {
		c7100_temp_format(envbuf, val);
		errmsg(&msgsym(ENVCRIT, ENVM), temp_table[ix].name, envbuf);
		reg_invoke_envmib_temperature_trap(ix+1);
		TIMER_START(temp_table[ix].crit_suppress, C7100_ENV_CRIT_INTERVAL);
	    }
	    break;
	  case ENV_STATE_SHUTDOWN:
	    /*
	     * Shut off power supply. It is too hot in here.
	     */
	    if (envm_debug_ps) {
		buginf("\nPS SHUTDOWN called from check temps");
	    }
            monitor_shutdown(REASON_TEMP);
	    break;
	  default:
	    break;
	}
    }
}

/*
 * measure_deltas
 *
 * Record the temperature deltas inside the chassis.
 * The deltas are measured between the inlet temp
 * and the outlet temps.
 *
 * delta1 = outlet1 - inlet
 * delta2 = outlet2 - inlet
 * delta3 = outlet3 - inlet
 *
 * The deltas are used to check fan status. If the
 * deltas grow larger over time, then it can be
 * assumed that a fan has died or the outlet is 
 * blocked.
 */

static void measure_deltas (void) {

    fan_table[FAN0].delta =
	temp_table[TEMP_OUTLET0].current_val -
	    temp_table[TEMP_INLET].current_val;

    fan_table[FAN1].delta =
	temp_table[TEMP_OUTLET1].current_val -
	    temp_table[TEMP_INLET].current_val;

    fan_table[FAN2].delta =
	temp_table[TEMP_OUTLET2].current_val -
	    temp_table[TEMP_INLET].current_val;
}

static void print_fan_table(void) {
    int ix;

    for (ix = 0; ix < C7100_FANS; ix++) {
	printf("\n%s max = %d delta = %d",
	       fan_table[ix].name, fan_table[ix].max_delta, fan_table[ix].delta);
    }
}

/*
 * check_fan_status
 * 
 * Determine the state of the fans. This is done in an
 * indirect manner. After the ambient temperature has 
 * reached a stable level, the deltas between the 
 * inlet sensor and the outlet sensors are measured.
 * If the deltas drift out of range, then
 * it is most likely because a fan has failed.
 *
 */

static void check_fan_status (void) {
    int ix;
    /* 
     * Check for fan failures.
     */
    measure_deltas();
    if (envm_debug_fan) {
	print_fan_table();
    }
    for (ix = 0; ix < C7100_FANS; ix++) {
	if (TIMER_RUNNING_AND_AWAKE(fan_table[ix].warn_suppress))
	    TIMER_STOP(fan_table[ix].warn_suppress);
	if (fan_table[ix].delta >= fan_table[ix].max_delta) {
	    fan_table[ix].state = ENV_STATE_WARNING;
	}
	switch (fan_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    break;
	  case ENV_STATE_WARNING:
	    if (!TIMER_RUNNING(fan_table[ix].warn_suppress)) {
		errmsg(&msgsym(BLOWER, ENVM), fan_table[ix].name);
		reg_invoke_envmib_fan_trap(ix+1);
		TIMER_START(fan_table[ix].warn_suppress, C7100_ENV_WARN_INTERVAL);
	    }
	    break;
	  default:
	    break;
	}
    }
}

/* 
* get_new_voltage()
 * 
 * Get sensed midplane voltages of the Predator system. 
 * Convert them into the proper format so that the code
 * can determine system health.
 *
 */
static void get_new_voltage (int index) {
    volttab_t *volttab;

    volttab = &volt_table[index];
    /*
     * Select the desired input to the a2d 
     * and go away for a while.
     */
    select_mux(volttab->mux_select);
    process_sleep_for(C7200_ENVMON_POLL_INTERVAL);

    /* 
     * Now get the a2d output.
     */
    volttab->current_val = read_a2d();
}

/*
 * check_voltages
 * 
 * Make the comparisons to determine if any of the
 * midplane voltages are out of spec.
 *
 */
static void check_voltages (void) {
    int ix;
    uint val;
    char envbuf[MAX_ENVBUF_LEN];

    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	val = volt_table[ix].current_val;
	if (TIMER_RUNNING_AND_AWAKE(volt_table[ix].warn_suppress))
	    TIMER_STOP(volt_table[ix].warn_suppress);
	if (TIMER_RUNNING_AND_AWAKE(volt_table[ix].crit_suppress))
	    TIMER_STOP(volt_table[ix].crit_suppress);

	if (val >= volt_table[ix].shut_high) {
	    volt_table[ix].state = ENV_STATE_SHUTDOWN;
	} else if (val >= volt_table[ix].crit_high) {
	    volt_table[ix].state = ENV_STATE_CRITICAL;
	} else if (val >= volt_table[ix].warn_high) {
	    volt_table[ix].state = ENV_STATE_WARNING;
	} else if (val <= volt_table[ix].shut_low) {
	    volt_table[ix].state = ENV_STATE_SHUTDOWN;
	} else if (val <= volt_table[ix].crit_low) {
	    volt_table[ix].state = ENV_STATE_CRITICAL;
	} else if (val <= volt_table[ix].warn_low) {
	    volt_table[ix].state = ENV_STATE_WARNING;
	} else {
	    volt_table[ix].state = ENV_STATE_NORMAL;
	}
	switch (volt_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    break;
	  case ENV_STATE_WARNING:
	    if (!TIMER_RUNNING(volt_table[ix].warn_suppress)) {
		volt_table[ix].format(envbuf, val);
		errmsg(&msgsym(ENVWARN, ENVM), volt_table[ix].name, envbuf);
		reg_invoke_envmib_voltage_trap(ix+1);
		TIMER_START(volt_table[ix].warn_suppress, C7100_ENV_WARN_INTERVAL);
	    }
	    break;
	  case ENV_STATE_CRITICAL:
	    if (!TIMER_RUNNING(volt_table[ix].crit_suppress)) {
		volt_table[ix].format(envbuf, val);
		errmsg(&msgsym(ENVCRIT, ENVM), volt_table[ix].name, envbuf);
		reg_invoke_envmib_voltage_trap(ix+1);
		TIMER_START(volt_table[ix].crit_suppress, C7100_ENV_CRIT_INTERVAL);
	    }
	    break;
	  case ENV_STATE_SHUTDOWN:
	    /*
	     * Turn off power supplies because of out of range voltage condition.
	     */
	    if (envm_debug_ps) {
		buginf("\nPS SHUTDOWN called from check volts");
	    }
            monitor_shutdown(REASON_VOLT);
	    break;
	  default:
	    break;
	}
    }
}

/*
 * get_ps_type
 *
 * Convert an a2d reading from a power
 * supply id voltage into a pstype. Need this since
 * a2d readings can drift a bit.
 *
 */

static pstype_t get_ps_type (int val) {

    if ((val >= PSTYPE_6_LOW) && (val <= PSTYPE_6_HIGH))
	return (PSTYPE_6);
    if ((val >= PSTYPE_7_LOW) && (val <= PSTYPE_7_HIGH))
	return (PSTYPE_7);
    return (PSTYPE_EMPTY);
}

/*
 * get_ps_state
 *
 * Take a power supply type and a 
 * power supply type and determine the
 * state of the supply (Empty, Off, On).
 *
 */

static env_state_t get_ps_state (int fail, pstype_t type) {
    if (type == PSTYPE_EMPTY) {
	return (ENV_STATE_NOT_PRESENT);
    } 
    if (fail) {
	return (ENV_STATE_SHUTDOWN);
    }
    return (ENV_STATE_NORMAL);
}

/*
 * check_ps_id
 *
 * Determine if either of the power supplies have
 * changed state.
 *
 */
static void check_ps_id (int index) {
    uint new_val;
    uint new_fail;
    pstype_t new_type;
    env_state_t new_state;
    pstab_t *pstab;
    leveltype level;

    pstab = &ps_table[index];

    /*
     * Select the desired input to the a2d 
     * and go away for a while.
     */
    select_mux(pstab->mux_select);
    process_sleep_for(C7200_ENVMON_POLL_INTERVAL);

    /* 
     * Now get the a2d output. Multiply it by 
     * (5v expected) / (5v Actual). This brings
     * the measured ps id voltage closed to the
     * expected value.
     */
    new_val = read_a2d();
    new_type = get_ps_type(new_val);

    /*
     * Read failure bit here.
     */
    if (envm_debug_ps) {
	buginf("\nIn ps id check 1 MGMT INT STAT LSW = %x, PA TEST PORT NSW = %x", 
	       C7100_MP_FPGA->mgmt_int_stat_lsw, C7100_MP_FPGA->pa_tst_port_nsw);
	C7100_MP_FPGA->mgmt_int_stat_lsw |= 0x30;
	buginf("\nIn ps id check 2 MGMT INT STAT LSW = %x, PA TEST PORT NSW = %x", 
	       C7100_MP_FPGA->mgmt_int_stat_lsw, C7100_MP_FPGA->pa_tst_port_nsw);
    }

    /*
     * Raise interrupts while modifying ps state.
     */
    level = raise_interrupt_level(ALL_DISABLE);

    C7100_MP_FPGA->mgmt_int_stat_lsw |= 1 << (index + 4);
    new_fail = C7100_MP_FPGA->pa_tst_port_nsw & (1 << (index + 2));
    new_state = get_ps_state(new_fail, new_type);
    /*
     * Now determine if the state of the supply has changed.
     * Switch on the previous state then switch on the new
     * state.
     */
    if (envm_debug_ps) {
	buginf("\nPS %d: current state = %d, new state = %d",
	       index, pstab->state, new_state);
    }
    switch (pstab->state) {
      case ENV_STATE_NOT_PRESENT:
	switch (new_state) {
	  case ENV_STATE_NOT_PRESENT:
	    break;
	  case ENV_STATE_SHUTDOWN:
	    if (pstab->init) {
		errmsg(&msgsym(PSCHANGE, ENVM), pstab->name,
		       pstype_name_array[pstab->pstype], pstype_name_array[new_type]);
	    }
	    break;
	  case ENV_STATE_NORMAL:
	    if (pstab->init) {
		errmsg(&msgsym(PSCHANGE, ENVM), pstab->name,
		       pstype_name_array[pstab->pstype], pstype_name_array[new_type]);
	    }
	    /*
	     * Enable power fail interrupt for this supply.
	     */
	    C7100_MP_FPGA->mgmt_int_stat_lsw |= 1 << (index + 4);
	    if (!c7100_broken_ps) {
		C7100_MP_FPGA->mgmt_int_enbl_lsw |= 1 << (index + 4);
	    }
	    break;
	  default:
	    break;
	}
	break;
      case ENV_STATE_SHUTDOWN:
	switch (new_state) {
	  case ENV_STATE_NOT_PRESENT:
	    errmsg(&msgsym(PSCHANGE, ENVM), pstab->name,
		   pstype_name_array[pstab->pstype], pstype_name_array[new_type]);
	    break;
	  case ENV_STATE_SHUTDOWN:
	    break;
	  case ENV_STATE_NORMAL:
	    errmsg(&msgsym(PSLEV, ENVM), pstab->name, env_state_name_array[pstab->state], env_state_name_array[new_state]);
	    /*
	     * Enable power fail interrupt for this supply.
	     */
	    C7100_MP_FPGA->mgmt_int_stat_lsw |= 1 << (index + 4);
	    if (!c7100_broken_ps) {
		C7100_MP_FPGA->mgmt_int_enbl_lsw |= 1 << (index + 4);
	    }
	    break;
	  default:
	    break;
	}
	break;
      case ENV_STATE_NORMAL:
	switch (new_state) {
	  case ENV_STATE_NOT_PRESENT:
	    errmsg(&msgsym(PSCHANGE, ENVM), pstab->name,
		   pstype_name_array[pstab->pstype], pstype_name_array[new_type]);
	    break;
	  case ENV_STATE_SHUTDOWN:
	    errmsg(&msgsym(PSLEV, ENVM), pstab->name, env_state_name_array[pstab->state], env_state_name_array[new_state]);
	    break;
	  case ENV_STATE_NORMAL:
	    break;
	  default:
	    break;
	}
      default:
	break;
    }
    pstab->current_val = new_val;
    pstab->state = new_state;
    pstab->pstype = new_type;
    pstab->init = TRUE;

    /*
     * Restore interrupts.
     */
    reset_interrupt_level(level);
}

/*
 * get_last_envm_stats
 *
 * Retrive the environmental stats from the midplane eeprom.
 *
 */

static boolean get_last_envm_stats (void) {

    int ix;
    envm_stats_t stats;
    boolean read_ok;
    ushort xsum;

    read_ok = c7100_read_mpeeprom_partial(&stats,
                                          sizeof(envm_stats_t) / 2,
                                          ENVM_STATS_EEPROM_OFFSET);
    if (! read_ok) {
        return FALSE;
    }

    if (stats.magic != ENVM_STATS_MAGIC) {
        return FALSE;
    }

    xsum = stats.xsum;
    stats.xsum = 0;
    if (xsum != ipcrc((ushort *)&stats, sizeof(stats) / 2)) {
        return FALSE;
    }

    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
        temp_table[ix].last_val = stats.temp[ix];
    }

    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
        volt_table[ix].last_val = stats.volt[ix];
    }
    for (ix = 0; ix < C7100_POWER_SUPPLIES; ix++) {
        ps_table[ix].last_val = stats.ps[ix];
    }

    shutdown_reason_last = stats.shutdown_reason;

    return TRUE;
}

/*
 * c7100_envmon_interrupt
 *
 * Handle an environmental monitor interrupt.
 * The only event that causes envm interrupts
 * is a power supply asserting its "power
 * fail" bit. Determine which supply failed and
 * whether there is another supply to keep
 * things going.
 *
 */

boolean c7100_envmon_interrupt (ulong envmon_event) {

    uint ix, up_count, old_state;
    pstab_t* pstab;
    
    if (c7100_broken_ps)
	return(FALSE);

    up_count = 2;
    for (ix = 0; ix < C7100_POWER_SUPPLIES; ix++) {
	if (envmon_event & (1 << (ix + 4))) {
	    pstab = &ps_table[ix];
	    old_state = pstab->state;
	    if (old_state != ENV_STATE_SHUTDOWN) {
		C7100_MP_FPGA->mgmt_int_stat_lsw |= (1 << (ix + 4));
		pstab->state = ENV_STATE_SHUTDOWN;
	    }
	    up_count--;
	}
    }
    return(FALSE);
}	

/*
 * c7100_envmon_daemon
 *
 * This is the periodic checks of the predator environmentals 
 * (temperature, voltage, power supply ids and fan status) to 
 * make sure they stay within spec. Otherwise, shutdown the system!
 *
 */
forktype c7100_envmon_daemon (void) {
    int ix;
    pstype_t ps0_id, ps1_id;
    int save_last_cnt;

    save_last_cnt = ENVM_SAVE_LAST_CNT;

    for (;;) {
	get_new_ds1620_temps();
	check_ds1620_temps();
	
	for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	    get_new_voltage(ix);
	}
	check_voltages();
	
	/*
	 * Check the power supply IDs to see if a power supply has changed state.
	 */
	ps0_id = ps_table[PS0].pstype;
	ps1_id = ps_table[PS1].pstype;
	for (ix = 0; ix < C7100_POWER_SUPPLIES; ix++) {
	    check_ps_id(ix);
	}
	
	/* 
	 * Check if the warmup period is over. If so, then use the current temperatures
	 * to calculate the deltas between the temperatures at the inlet and outlets.
	 * We use these deltas to deduce fan stability.
	 */
	if (!warm) {
	    if (TIMER_RUNNING_AND_AWAKE(warmup_timer)) {
		if (c7100_include_fans) {
		    warm = TRUE;
		}
		TIMER_STOP(warmup_timer);
	    }
	} else {
	    check_fan_status();
	}
        /*
         * Each pass through this loop currently takes about 1 minute.
         * Thus stats are saved once every ENVM_SAVE_LAST_CNT minutes.
         */
        if (save_last_cnt >= ENVM_SAVE_LAST_CNT) {
            /*  
             * Save last environmental statistic for the command "show env last"
             */ 
            save_last_envm_stats();
            save_last_cnt = 0;
        } else {
            save_last_cnt++;
        }
    }
}

/*
 * c7100_envm_midplane_test
 *
 * Check the midplane version to determine what the
 * hardware is capable of doing.
 */

static void c7100_envmon_midplane_test (void)
{
    c7100_mpeeprom_copy_t c;

    c7100_read_mpeeprom(&c);
    c7100_broken_ps = FALSE;
    if ((c.hardware_version_high < 1) || (c.hardware_version_high > 100)) {
	c7100_broken_ps = TRUE;
    }
    if ((c.hardware_version_high == 1) && (c.hardware_version_low < 3)) {
	c7100_broken_ps = TRUE;
    }
}

/*
 * c7100_envmon_init
 *
 * Initialize the predator environmental monitor and
 * kick off the envmon daemon.
 *
 */

static void c7100_envmon_init (subsystype *subsys)
{
    int ix;
    int ds1620_config_status_array[C7100_TEMP_SENSORS];
    sys_timestamp init_timer;
    int cpu_card;
    
    cpu_card = c7100_cpucard_type();

    switch (cpu_card) {

        case C7100_CPU_NPE150:
            temp_table = temp_table_npe150;
            break;

        case C7100_CPU_NPE100:
            temp_table = temp_table_npe100;
            break;

        default:
            /*
             * Don't know what type of CPU card is installed.
             * Print an error message, and default to NPE-150.
             */
            errmsg(&msgsym(CPUCARD, ENVM), cpu_card);
            temp_table = temp_table_npe150;
            break;

    }   /* switch (cpu_card) */
    
    sprintf(inlet_name, "chassis inlet");
    sprintf(outlet0_name, "chassis outlet 1");
    sprintf(outlet1_name, "chassis outlet 2");
    sprintf(outlet2_name, "chassis outlet 3");
    sprintf(p3v_name, "+3.45 V");
    sprintf(p5v_name, "+5.15 V");
    sprintf(p12v_name, "+12.15 V");
    sprintf(n12v_name, "-11.95 V");
    sprintf(ps0_name, "Power Supply 1");
    sprintf(ps1_name, "Power Supply 2");
    sprintf(fan0_name, "Fan 1");
    sprintf(fan1_name, "Fan 2");
    sprintf(fan2_name, "Fan 3");
    
    temp_table[TEMP_INLET].current_val = 0;
    temp_table[TEMP_OUTLET0].current_val = 0;
    temp_table[TEMP_OUTLET1].current_val = 0;
    temp_table[TEMP_OUTLET2].current_val = 0;
    volt_table[P3V].current_val = 0;
    volt_table[P5V].current_val = 0;
    volt_table[P12V].current_val = 0;
    volt_table[N12V].current_val = 0;
    fan_table[FAN0].current_val = 0;
    fan_table[FAN1].current_val = 0;
    fan_table[FAN2].current_val = 0;
    ps_table[PS0].pstype = PSTYPE_EMPTY;
    ps_table[PS1].pstype = PSTYPE_EMPTY;
    ps_table[PS0].state = ENV_STATE_NOT_PRESENT;
    ps_table[PS1].state = ENV_STATE_NOT_PRESENT;
    ps_table[PS0].init = FALSE;
    ps_table[PS1].init = FALSE;

    /* 
     * Check the configuration of the temp sensors. We only want
     * to program them if they haven't been programmed before,
     * since the parts have a max of 50K writes to the config
     * register.
     */
    read_ds1620_byte(DS1620_READ_CONFIG, 
		     ds1620_config_status_array, 
		     DS1620_CONFIG_READ_SIZE);

    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	if (ds1620_config_status_array[ix] && DS1620_CONFIG_STATUS_CPU)
	    continue;
	else {
	    /*
	     * Need to write the config data then wait
	     * for 100 ms while the temp sensor's reset
	     * line is off.
	     */
	    errmsg(&msgsym(NOACK, ENVM), temp_table[ix].name);
	    write_ds1620_byte(DS1620_WRITE_CONFIG, 
			      DS1620_CONFIG_STATUS_CPU, 
			      TRUE);
	    TIMER_START(init_timer, 20);
	    while (SLEEPING(init_timer))
		;
	    TIMER_STOP(init_timer);
	    C7100_IO_FPGA->temp_ctrl = DS1620_RESET_ON;
	    break;
	}
    }

    /*
     * Start the temp sensors. They will now continuously
     * be sensing the temperature. But you have to wait a 
     * second first.
     */
    write_ds1620_cmd(DS1620_START_CONVT);
    TIMER_START(init_timer, ONESEC);
    while (SLEEPING(init_timer))
	;
    TIMER_STOP(init_timer);

    TIMER_START(warmup_timer, DS1620_WARMUP_TIME);
    warm = FALSE;
    lastenv_failure = FALSE;
    /*
     * Unless otherwise specified, it is always the power
     * supply's fault that the system is being shutoff.
     * If it were the temperature or voltage causing the
     * shutdown, the reason will be changed at that time.
     */
    shutdown_reason = REASON_PS; 
    shutdown_reason_last = REASON_UNDEFINED; 

    /*
     * Start the EnvMon process.
     */
     c7100_envmon_pid = process_create(c7100_envmon_daemon,
				      "EnvMon",
				      NORMAL_STACK,
				      PRIO_NORMAL);

    if (c7100_envmon_pid == NO_PROCESS) {
	errmsg(&msgsym(NOFORK, ENVM));
    }
    lastenv_available = get_last_envm_stats();
    envmon_parser_init();
    envmib_load = TRUE;
    c7100_envmon_midplane_test();
    c7100_include_fans = FALSE;

    if (envm_debug_ps) {
	buginf("\nBefore init MGMT INT STAT LSW = %x, PA TEST PORT NSW = %x", 
	       C7100_MP_FPGA->mgmt_int_stat_lsw, C7100_MP_FPGA->pa_tst_port_nsw);
    }
    C7100_MP_FPGA->mgmt_int_stat_lsw |= MGMT_INTR_STATUS_PS0_FAIL;
    C7100_MP_FPGA->mgmt_int_stat_lsw |= MGMT_INTR_STATUS_PS1_FAIL;
    if (envm_debug_ps) {
	buginf("\nAfter init MGMT INT STAT LSW = %x, PA TEST PORT NSW = %x", 
	       C7100_MP_FPGA->mgmt_int_stat_lsw, C7100_MP_FPGA->pa_tst_port_nsw);
    }

    return;
}

#define ENVMON_MAJVERSION 1
#define ENVMON_MINVERSION 0
#define ENVMON_EDITVERSION 1

SUBSYS_HEADER(envmon, 
	      ENVMON_MAJVERSION, 
	      ENVMON_MINVERSION, 
	      ENVMON_EDITVERSION,
              c7100_envmon_init,
	      SUBSYS_CLASS_KERNEL,
              NULL,
              NULL);

/************************************
 * SHOW ENV ROUTINES
 ***********************************/
/*
 * "show env all" - show all possible information
 */

static void
show_env_all (void)
{
    int ix;
    
    /*
     * Something here for midplane/io/cpu IDs
     */

    printf("\nPower Supplies:\n");
    for (ix = 0; ix < C7100_POWER_SUPPLIES; ix++) {
	if (!ps_table[ix].init) {
	    printf("\tPower supply %d is unmeasured.\n", (ix + 1));
	    continue;
	}

	printf("\tPower supply %d is %s.",
	       (ix + 1), 
	       (ps_table[ix].state == ENV_STATE_NOT_PRESENT) ? 
	       "empty" :
	       pstype_name_array[ps_table[ix].pstype]);
	switch(ps_table[ix].state) {
	  case ENV_STATE_NOT_PRESENT:
	    printf("\n");
	    break;
	  case ENV_STATE_SHUTDOWN:
	    printf(" Unit is off.\n");
	    break;
	  case ENV_STATE_NORMAL:
	    printf(" Unit is on.\n");
	    break;
	  default:
	    break;
	}
    }

    printf("\nTemperature readings:\n");
    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	char buf[10];

	if (!temp_table[ix].current_val) {
	    printf("\t%16s is unmeasured\n", temp_table[ix].name);
	    continue;
	}
	    
	c7100_temp_format(buf, temp_table[ix].current_val);
	printf("\t%16s measured at %s ", temp_table[ix].name, buf);
	switch (temp_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    printf("\n");
	    break;
	  case ENV_STATE_WARNING:
	    printf(":Temperature in Warning range!\n");
	    break;
	  case ENV_STATE_CRITICAL:
	    printf(":Temperature in Critical range!\n");
	    break;
	  default:
	    printf("\n");
	    break;
	}
    }

    printf("\nVoltage readings:\n");
    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	char buf[MAX_ENVBUF_LEN];

	if (!volt_table[ix].current_val) {
	    printf("\t%7s is unmeasured\n", volt_table[ix].name);
	    continue;
	}
	    
	volt_table[ix].format(buf, volt_table[ix].current_val);
	printf("\t%7s measured at %s V ", volt_table[ix].name, buf);
	switch (volt_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    printf("\n");
	    break;
	  case ENV_STATE_WARNING:
	    printf(":Voltage in Warning range!\n");
	    break;
	  case ENV_STATE_CRITICAL:
	    printf(":Voltage in Critical range!\n");
	    break;
	  default:
	    printf("\n");
	    break;
	}
    }

    if (c7100_include_fans) {
	printf("\nFans:");
	if (warm) {
	    for (ix = 0; ix < C7100_FANS; ix++) {
		printf("\n\t%s is believed to be %s.",
		       fan_table[ix].name, 
		       (fan_table[ix].delta >= fan_table[ix].max_delta) ? "off" : "on");
	    }
	} else {
	    printf("\n\tStill warming up. Fan deltas not available.");
	}
    }
}

/*
 * "show env" - just give thumbs up/down
 */
static void
show_env_default (void)
{
    int ix;
    boolean warned;

    warned = FALSE;
    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	int cel, fahr;
	boolean warning;

	warning = FALSE;
	switch (temp_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    continue;
	  case ENV_STATE_WARNING:
	    warning = TRUE;
	    break;
	  case ENV_STATE_CRITICAL:
	    break;
	  default:
	    break;
	}
	warned = TRUE;
	cel = ds1620_2_celcius(temp_table[ix].current_val);
	fahr = CEL2FAHR(cel);
	printf("\n\t%s: %s measured at %dC/%dF",
	       (warning) ? "Warning" : "CRITICAL"
	       , temp_table[ix].name, cel, fahr);
    }
    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	boolean warning;
	char buf[10];

	warning = FALSE;
	switch (volt_table[ix].state) {
	  case ENV_STATE_NORMAL:
	    continue;
	  case ENV_STATE_WARNING:
	    warning = TRUE;
	    break;
	  case ENV_STATE_CRITICAL:
	    break;
	  default:
	    break;
	}
	warned = TRUE;
	volt_table[ix].format(buf, volt_table[ix].current_val);
	printf("\n\t%s: %s measured at %s V",
	       (warning) ? "Warning" : "CRITICAL"
	       , volt_table[ix].name, 
	       buf);
    }
    if (c7100_include_fans) {
	for (ix = 0; ix < C7100_FANS; ix++) {
	    switch (fan_table[ix].state) {
	      case ENV_STATE_NORMAL:
		continue;
	      case ENV_STATE_WARNING:
		break;
	      default:
		break;
	    }
	    warned = TRUE;
	    printf("\n\tWarning: %s is believed to be off",
		   fan_table[ix].name);
	}
    }
    if (!warned)
	printf("\nAll measured values are normal");
        
    return;
}

/*
 * display table of environmental values
 */
static void
show_env_table (void)
{
    int ix;
    char* empty = "";
    char buf[10];

    printf("\n%18s%15s%15s%15s%15s", "Sample Point",
	   "LowCritical", "LowWarning", "HighWarning", "HighCritical");

    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	printf("\n%18s%15s%15s", temp_table[ix].name, empty, empty);
	c7100_temp_format(buf, temp_table[ix].warn_high);
	printf("%15s", buf);
	c7100_temp_format(buf, temp_table[ix].crit_high);
	printf("%s", buf);
    }
    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	char buf[10];
	printf("\n%18s", volt_table[ix].name);
	volt_table[ix].format(buf, volt_table[ix].crit_low);
	printf("\%15s", buf);
	volt_table[ix].format(buf, volt_table[ix].warn_low);
	printf("\%15s", buf);
	volt_table[ix].format(buf, volt_table[ix].warn_high);
	printf("\%15s", buf);
	volt_table[ix].format(buf, volt_table[ix].crit_high);
	printf("\%15s", buf);
    }
    c7100_temp_format(buf, temp_table[TEMP_INLET].shut_high);
    printf("\nSystem shutdown for %s %s", temp_table[TEMP_OUTLET0].name, buf);
    c7100_temp_format(buf, temp_table[TEMP_OUTLET2].shut_high);
    printf("\nSystem shutdown for %s %s", temp_table[TEMP_OUTLET2].name, buf);
}

/*
 * show environmental stats from shortly before last reload
 */
static void
show_env_last (void)
{
    int ix;

    if (!lastenv_available) {
	printf("\n\tData not available");
	return;
    }
    for (ix = 0; ix < C7100_TEMP_SENSORS; ix++) {
	char buf[10];

	c7100_temp_format(buf, temp_table[ix].last_val);
	printf("\n  %18s previously measured at %s", 
	       temp_table[ix].name, buf);
    }
    for (ix = 0; ix < C7100_VOLT_SENSORS; ix++) {
	char buf[10];

	volt_table[ix].format(buf, volt_table[ix].last_val);
	printf("\n  %18s previously measured at %s", 
	       volt_table[ix].name, buf);
    }
    printf("\n  last shutdown reason - %s", 
	   shutdown_reason_name_array[shutdown_reason_last]);
}

/*
 * "show env"
 *
 * OBJ(int,1) = { default | all | last | table }
 */
void show_env (parseinfo *csb)
{
    automore_enable(NULL);
    switch (GETOBJ(int, 1)) {
      case SHOW_ENV_ALL:
	show_env_all();
	break;

      case SHOW_ENV_DEFAULT:
	show_env_default();
	break;

      case SHOW_ENV_LAST:
	show_env_last();
	break;

      case SHOW_ENV_TABLE:
	show_env_table();
	break;

      default:
	printf("%%not implemented");
	break;
    }
    automore_disable();

    return;
}

/***************************************
 * MIB CODE SUPPORT 
 **************************************/
boolean c7100_volt_valid (uint index) {
    if (index > C7100_VOLT_SENSORS || index < 1)
	return FALSE;
    --index;
    if (volt_table[index].current_val == 0)
	return FALSE;
    return TRUE;
}

char* c7100_volt_descr (uint index) {
    --index;
    return volt_table[index].name;
}

int c7100_volt_value (uint index) {
    volttab_t* vtab;

    --index;
    vtab = &volt_table[index];
    return (vtab->convert(vtab->current_val));
}

uint c7100_volt_lowthresh (uint index) {
    volttab_t* vtab;

    --index;
    vtab = &volt_table[index];
    return vtab->convert(vtab->crit_low);
}

uint c7100_volt_highthresh (uint index) {
    volttab_t* vtab;

    --index;
    vtab = &volt_table[index];
    return vtab->convert(vtab->crit_high);
}

uint c7100_volt_lastvalue (uint index) {
    volttab_t* vtab;

    --index;
    vtab = &volt_table[index];
    return vtab->convert(vtab->last_val);
}

uint c7100_volt_state (uint index) {
    --index;
    switch (volt_table[index].state) {
      case ENV_STATE_NORMAL:	return D_ciscoEnvMonVoltageState_normal;
      case ENV_STATE_WARNING:	return D_ciscoEnvMonVoltageState_warning;
      case ENV_STATE_CRITICAL:	return D_ciscoEnvMonVoltageState_critical;
      default:
	break;
    }
    return D_ciscoEnvMonVoltageState_notPresent;
}

boolean c7100_temp_valid (uint index) {
    if (index > C7100_TEMP_SENSORS || index < 1)
	return FALSE;
    --index;
    if (temp_table[index].current_val == 0)
	return FALSE;
    return TRUE;
}

char* c7100_temp_descr (uint index) {
    --index;
    return temp_table[index].name;
}

uint c7100_temp_value (uint index) {
    uint val;

    --index;
    val = ds1620_2_celcius(temp_table[index].current_val);
    return (val);
}

uint c7100_temp_thresh (uint index) {
    uint val;

    --index;
    val = ds1620_2_celcius(temp_table[index].crit_high);
    return (val);
}

uint c7100_temp_lastvalue (uint index) {
    uint val;

    --index;
    val = ds1620_2_celcius(temp_table[index].last_val);
    return (val);
}

uint c7100_temp_state (uint index)
{
    --index;
    switch (temp_table[index].state) {
      case ENV_STATE_NORMAL:	return D_ciscoEnvMonTemperatureState_normal;
      case ENV_STATE_WARNING:	return D_ciscoEnvMonTemperatureState_warning;
      case ENV_STATE_CRITICAL:	return D_ciscoEnvMonTemperatureState_critical;
      default:
	break;
    }
    return D_ciscoEnvMonTemperatureState_notPresent;
}

boolean c7100_ps_valid (uint index) {
    if (index > C7100_POWER_SUPPLIES || index < 1 || !ps_table[--index].init)
	return FALSE;
    return TRUE;
}

char* c7100_ps_descr (uint index) {
    --index;
    if (index >= C7100_POWER_SUPPLIES) {
        return pstype_name_array[PSTYPE_UNKNOWN];
    }
    return pstype_name_array[ps_table[index].pstype];
}

uint c7100_ps_state (uint index) {
    --index;
    switch (ps_table[index].state) {
      case ENV_STATE_NORMAL:	return D_ciscoEnvMonSupplyState_normal;
      case ENV_STATE_SHUTDOWN:	return D_ciscoEnvMonSupplyState_shutdown;
      case ENV_STATE_NOT_PRESENT: return D_ciscoEnvMonSupplyState_notPresent;
      default:
	break;
    }
    return D_ciscoEnvMonSupplyState_notPresent;
}
uint c7100_ps_source (uint index) {
    --index;
    if (index >= C7100_POWER_SUPPLIES) {
        return D_ciscoEnvMonSupplySource_unknown;
    }
    switch (ps_table[index].pstype) {
      case PSTYPE_6: return D_ciscoEnvMonSupplySource_ac;
      case PSTYPE_7: return D_ciscoEnvMonSupplySource_dc;
      default:
	break;
    }
    return D_ciscoEnvMonSupplySource_unknown;
}

boolean c7100_fan_valid (uint index) {
    if (index > C7100_FANS || index < 1)
	return FALSE;
    if (c7100_include_fans) {
	return TRUE;
    } else {
	return FALSE;
    }
}

char* c7100_fan_descr (uint index) {
    --index;
    return fan_table[index].name;
}

uint c7100_fan_state (uint index) {
    --index;
    switch (fan_table[index].state) {
      case ENV_STATE_NORMAL:	  return D_ciscoEnvMonFanState_normal;
      case ENV_STATE_SHUTDOWN:	  return D_ciscoEnvMonFanState_shutdown;
      case ENV_STATE_WARNING:	  return D_ciscoEnvMonFanState_shutdown;
      case ENV_STATE_CRITICAL:	  return D_ciscoEnvMonFanState_shutdown;
      case ENV_STATE_NOT_PRESENT: return D_ciscoEnvMonFanState_shutdown;
    }
    return D_ciscoEnvMonSupplyState_notPresent;
}
