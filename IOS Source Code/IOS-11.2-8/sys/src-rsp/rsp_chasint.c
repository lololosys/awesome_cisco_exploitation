/* $Id: rsp_chasint.c,v 3.15.8.9 1996/08/28 13:13:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_chasint.c,v $
 *------------------------------------------------------------------
 * Chassis Interface support code
 *
 * October 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Code supporting the "chassis interface" card in the
 * 75xx chassis.  Monitors environmental, power supply, 
 * and fan status.
 *------------------------------------------------------------------
 * $Log: rsp_chasint.c,v $
 * Revision 3.15.8.9  1996/08/28  13:13:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.8.8  1996/07/26  01:11:09  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.15.8.7  1996/07/09  21:15:14  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.15.8.6  1996/06/09  07:58:59  cheilek
 * CSCdi52427:  VIP1/VIP2 cardType enumeration labels should be vip/vip2
 * Branch: California_branch
 * Change MIBS: rvip to vip; svip to vip2.
 *
 * Revision 3.15.8.5  1996/05/21  10:04:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.8.4  1996/05/21  06:40:25  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.15.8.3  1996/05/14  02:57:47  feisal
 * CSCdi56797:  Card type for the 5EFL PA not added properly to the chasis
 * MIB
 * Branch: California_branch
 *
 * Revision 3.15.8.2  1996/04/29  09:46:15  jjohnson
 * CSCdi55599:  System restart when querying CISCO-ENVMON-MIB
 * Branch: California_branch
 *
 * Revision 3.15.8.1  1996/03/18  22:01:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.5  1996/03/16  07:36:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.4  1996/03/14  01:26:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.11.2.3  1996/03/07  10:44:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.2  1996/02/20  17:59:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/02/28  09:33:50  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.14  1996/02/15  00:43:56  dbath
 * CSCdi38455:  HSA needs SNMP support
 * Call card_type_and_descr() to determine the card type and
 * card description for RSP controllers, rather than have our
 * own switch here.  Also, when returning OperStatus, take into
 * consideration the slave as well as the master.
 *
 * Revision 3.13  1996/02/02  22:58:13  schaefer
 * CSCdi48075: work around misprogrammed CI EEPROM
 *
 * Revision 3.12  1996/01/23  16:12:33  schaefer
 * CSCdi47221: fix rsp7000 temp thresholds
 *
 * Revision 3.11.2.1  1996/01/24  23:03:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11  1996/01/21  17:28:06  schaefer
 * CSCdi47221: fix rsp7000 temp thresholds
 *
 * Revision 3.10  1996/01/17  02:21:20  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.9  1996/01/16  06:39:51  schaefer
 * CSCdi45761: support RSP in new chassis
 * add temperature thresholds
 *
 * Revision 3.8  1996/01/15  18:05:06  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.7  1995/12/28  08:24:46  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.6  1995/12/17  18:38:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/15  03:16:11  rpratt
 * CSCdi45651:  Chassis MIB enhancements made Port Ready
 *
 * Revision 3.4  1995/12/01  19:04:43  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.3  1995/11/24  16:41:50  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  18:47:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:28:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/17  20:55:19  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.8  1995/08/11  08:47:07  schaefer
 * CSCdi38464: get RSP1/7505 slot right
 *
 * Revision 2.7  1995/08/10  22:18:39  schaefer
 * CSCdi37459: spurious fan failure messages on 7513
 * also, fix +5, +12, +24 threshold values
 *
 * Revision 2.6  1995/08/10  19:11:58  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.5  1995/07/18  18:01:33  mcieslak
 * Added some additional Master/Slave support for Dual RSP2's.
 * Ensure that the Master/Slave interrupts are masked during
 * initialization.
 * CSCdi37206:  Dual processor support needs work
 *
 * Revision 2.4  1995/07/17  08:39:50  schaefer
 * CSCdi36180: medley of chassis interface fixes
 * 7513 variable speed blower, shutdown after 7505 fan failure
 * 7507 support, 7513 fan failure warnings
 *
 * Revision 2.3  1995/06/28  21:17:17  sdurham
 * CSCdi36261:  CISCO-ENVMON-MIB and OLD-CISCO-ENV-MIB being loaded on ags
 *              improperly.
 *
 * Revision 2.2  1995/06/09  13:21:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:57:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include "logger.h"
#include "../src-rsp/msg_chasint.c"	/* Not a typo, see logger.h */
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_qa.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/dgbus.h"
#include "../hes/dbus.h"
#include "../hes/if_fci.h"
#include "../hes/if_hes_common.h"
#include "rsp_chasint.h"
#include "rsp_pcmap.h"
#include "rsp_macaddr.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../h/interface_private.h"
#include "../if/ether.h"
#include "../dev/monitor1.h"
#include "../dev/flash_registry.h"
#include "../hes/sr_envmib.h"
#include "../hes/envmib_registry.h"
#include "../hes/envmon_parser.h"
#include "rsp_init.h"
#include "../util/itemlist.h"
#include "rsp_if.h"

/*
 * generate all possible sysObjectID values for this platform
 */
#define SNMPPART_DECL
#define N_cisco7505
#define N_cisco7506
#define N_cisco7507
#define N_cisco7513
#define N_cisco7000
#define N_cisco7010
#define N_ciscoGatewayServer
#include "../snmp/sr_lsmi.h"
#undef SNMPPART_DECL

/*
 * array of booleans indicating which slots we should pretend to be empty
 */
slot_mask_t slot_present_mask;

/*
 * chassis interface occupies dbus slot 31
 */
#define CHASSIS_INTERFACE_SLOT	31
#define DBUS_NUM_SLOTS		32
#define SLOTBITS_PER_BYTE	 8

/*
 * clear soft power failure count after this amount of time
 */
#define CI_CYCLE_CLEAR_MINUTES	(6*60)

/*
 * this number of software power failures result in a hard power failure
 * (must fit in four bits)
 */
#define CI_CYCLE_TRIP_POINT	15

typedef volatile uchar v_uchar;

/*
 * private data types
 */
typedef struct chasint_dbus_ {			/* byte offset */
    uchar	DBus_Slot;			/* 0x00 */
    uchar	Last_Cmd;
    uchar	DBus_Status;			/* note: non-standard usage */
    uchar	:8;
    uchar	DBus_LSlot;			/* 0x04 - not used by CI */
    uchar	DBus_Cntrl;			/* also not used by CI */
    uchar	:8;
    uchar	:8;
    uchar	:8;				/* 0x08 */
    uchar	:8;
    uchar	:8;
    uchar	:8;
    uchar	:8;				/* 0x0c */
    uchar	:8;
    uchar	:8;
    uchar	:8;
    uchar	BPlane_ID;			/* 0x10 */
    v_uchar	PS_ID;
    v_uchar	Int_Status;
    v_uchar	Int_Mask;
    uchar	DBUS[DBUS_NUM_SLOTS /
		     SLOTBITS_PER_BYTE];	/* 0x14 */
    v_uchar	Power_Ctl;			/* 0x18 */
    uchar	Power_Status;
    v_uchar	AD_Ctl;
    v_uchar	AD_Result;
    uchar	:8;				/* 0x1c */
    uchar	:8;
    uchar	:8;
    uchar	:8;
    dgbus_eeprom_t eeprom;			/* 0x20 */
} chasint_dbus_t;

#define DBUS_STATUS_CI_ERR(x)		(((x) & 0x40) != 0)
#define DBUS_STATUS_CI_COND(x)		((x) & 0x0f)

#define BPLANE_ID_ARB_ID(x)		((x) >> 4)
#define BPLANE_ID_BACKPLANE_ID(x)	((x) & 0x07)
#define   BACKPLANE_7506		0
#define   BACKPLANE_7505		1
#define   BACKPLANE_7513		2
#define   BACKPLANE_7507		4
#define   BACKPLANE_7000		5	/* RSP/7000 in 7000 or 7010 */
#define   BACKPLANE_TOKTEST		7
#define   BACKPLANE_UNKNOWN		8

/*
 * in the 7507 chassis, slots 0-6 are in the CI's dbus_present bits 4-10
 */
#define BP7507_OFFSET		4
#define FANS_7507		1
#define PS_7507			2

/*
 * 7505 has six fans, but they draw air over different parts of the
 * chassis, so losing even one is reason for shutting the box down.
 */
#define FANS_7505		6
#define FAN_SHUTDOWN_7505	(2*ONEMIN)
#define PS_7505			1

/*
 * 7513 fans are:
 *	#1 - main blower
 *	#2 - PS1 fan
 *	#3 - PS2 fan
 */
#define FANS_7513		3
#define BP7513_FAN_MAIN		1
#define BP7513_FAN_PS1		2
#define BP7513_FAN_PS2		3
#define PS_7513			2

/*
 * number of slots per backplane type
 */
#define SLOTS_7506		7
#define SLOTS_7505		5
#define SLOTS_7513		13
#define SLOTS_7507		7
#define SLOTS_TOKTEST		10

#define PS_ID_PS1_ID(x)			((x) & 0x07)
#define PS_ID_PS2_ID(x)			((x) >> 4)
#define   BP7513PS1200WAC		1
#define   BP7513PS1200WDC		2
#define   BP7505PS600WAC		1
#define   BP7505PS600WDC		2
#define   BP7507PS850W			1
#define   BP7507PS700W			2

#define BP7513_1200W_P5_CAPACITY	(200 * 1000)	/* milliamps */
#define BP7513_1200W_P12_CAPACITY	( 35 * 1000)
#define BP7513_1200W_N12_CAPACITY	(  3 * 1000)
#define BP7513_1200W_P24_CAPACITY	(  8 * 1000)

#define INT_STATUS_TEMP_BIT		0x80
#define INT_STATUS_PS1_BIT		0x40
#define INT_STATUS_PS2_BIT		0x20
#define INT_STATUS_BFAIL_BIT		0x10
#define INT_STATUS_BMASK		0x0f

#define INT_STATUS_TEMP(x)		(((x) & INT_STATUS_TEMP_BIT)  != 0)
#define INT_STATUS_PS1_FAIL(x)		(((x) & INT_STATUS_PS1_BIT)   != 0)
#define INT_STATUS_PS2_FAIL(x)		(((x) & INT_STATUS_PS2_BIT)   != 0)
#define INT_STATUS_PSX_FAIL(stat,unit)	\
  (((stat) & ((unit == 1) ? INT_STATUS_PS1_BIT : INT_STATUS_PS2_BIT)) != 0)
#define INT_STATUS_BFAIL(x)		(((x) & INT_STATUS_BFAIL_BIT) != 0)
#define INT_STATUS_BLOWERNUM(x)		((x) & INT_STATUS_BMASK)

#define INT_MASK_TEMP			0x80		/* enables interrupt */
#define INT_MASK_PS1FAIL		0x40		/* enables interrupt */
#define INT_MASK_PS2FAIL		0x20		/* enables interrupt */
#define INT_MASK_BFAIL			0x10		/* enables interrupt */
#define INT_MASK_PS_SHUTDOWN(x)		(((x) & 0x08) != 0)
#define INT_MASK_BOARD_SHUTDOWN(x)	(((x) & 0x04) != 0)
#define INT_MASK_TEMP_WARN(x)		(((x) & 0x02) != 0)
#define INT_MASK_RESTART_OK(x)		(((x) & 0x01) != 0)
#define INT_TRIP_POINTS(x)		((x) & 0x0f)

#define POWER_CTL_OVERRIDE		0x80
#define POWER_CTL_PS2_SHUT		0x20
#define POWER_CTL_PS1_SHUT		0x10
#define POWER_CTL_TEMPR			0x08
#define POWER_CTL_TEMPSEL		0x04
#define POWER_CTL_SDA			0x02
#define POWER_CTL_SCL			0x01

#define POWER_STATUS_TRIP_POINT(x)	((x) >> 4)
#define POWER_STATUS_CYCLE_COUNT(x)	((x) & 0x0f)

#define AD_CTL_INPUT_SELECT(x)		((x) & 0x1f)
#define   MUX_RSPu0_INLET_TEMP		0
#define   MUX_RSPu0_HOT_SPOT		1
#define   MUX_RSPu0_EXHAUST		2
#define   MUX_RSPu1_INLET_TEMP		3
#define   MUX_RSPu1_HOT_SPOT		4
#define   MUX_RSPu1_EXHAUST		5
#define   MUX_PLUS12V_MONITOR		6
#define   MUX_PLUS5V_MONITOR		7
#define   MUX_MINUS12V_MONITOR		8
#define   MUX_PLUS24V_MONITOR		9
#define   MUX_VREF_2POINT5		10
#define   MUX_PS1_PLUS5V_CURRENT	11
#define   MUX_PS2_PLUS5V_CURRENT	12
#define   MUX_PS1_PLUS12V_CURRENT	13
#define   MUX_PS2_PLUS12V_CURRENT	14
#define   MUX_DO_NOT_USE		15
#define   MUX_PS1_MINUS12V_CURRENT	16
#define   MUX_PS2_MINUS12V_CURRENT	17

#define CIMUX_TEMP(x)		((((x) * 97656 + 50000) / 100000) - 148)

#define DEGREES_C2F(x)		((x) * 9/5 + 32)

#define CIMUX_mV_PLUS5(x)	(((x) * 195 +  25000) / 10)
#define CIMUX_mV_PLUS12(x)	(((x) * 468 +  59900) / 10)
#define CIMUX_mV_MINUS12(x)	 ((x) * -47 -   6010)
#define CIMUX_mV_PLUS24(x)	(((x) * 944 + 120700) / 10)
#define CIMUX_mV_2POINT5(x)	(((x) * 977 + 125000) / 100)

#define TRIP_TEMP_CELSIUS(x)	((((x) * 12500 / X9241_MAXVAL) - 2315) / 100)

#define MILLI_TO_UNITS(mv)	((mv) / 1000)
#define HUNDREDTHS(mv)		(((mv) % 1000 + 5) / 10)

#define A2D_MAXVAL		255
#define WARN_LOW_DISABLE	0
#define WARN_HIGH_DISABLE	A2D_MAXVAL

/*
 * for 7513 power supply types 1 & 2
 */
#define CIMUX_mAMP_P5_7513(x)	((x) * 1000*200 / A2D_MAXVAL)
#define CIMUX_mAMP_P12_7513(x)	((x) * 1000*35  / A2D_MAXVAL)
#define CIMUX_mAMP_N12_7513(x)	((x) * 1000*3   / A2D_MAXVAL)

/*
 * simple tests describing which test points are available on
 * which chassis
 */
#define measure_current()	(backplane_id == BACKPLANE_7513)

#define dual_cpu_capable()	((backplane_id == BACKPLANE_7513) || \
				 (backplane_id == BACKPLANE_7507))

#define monitor_power_supply()	(backplane_id != BACKPLANE_TOKTEST)

#define variable_blower()	(backplane_id == BACKPLANE_7513)

#define soft_shutdown_works()	(real_backplane_id != BACKPLANE_7000)

/*
 * saved environmental data
 */
static tinybool lastenv_available;
static uchar lastenv_ps1_id, lastenv_ps2_id, lastenv_bp_id;

#define KEY_MAGIC_VALUE		0xeb

enum {
    KEY_MAGIC = DS1201_CI,
    KEY_BACKPLANE_ID,
    KEY_PS1_ID,
    KEY_PS2_ID,

    KEY_TEMP_RSP0_IN,
    KEY_TEMP_RSP0_HOT,
    KEY_TEMP_RSP0_EX,
    KEY_TEMP_RSP1_IN,
    KEY_TEMP_RSP1_HOT,
    KEY_TEMP_RSP1_EX,
    KEY_VOLT_P12,
    KEY_VOLT_P5,
    KEY_VOLT_N12,
    KEY_VOLT_P24,
    KEY_AMP_PS1_P5,
    KEY_AMP_PS2_P5,
    KEY_AMP_PS1_P12,
    KEY_AMP_PS2_P12,
    KEY_AMP_PS1_N12,
    KEY_AMP_PS2_N12,
    KEY_INVALID = 0xff,
};

/*
 * message state
 */
typedef enum {
    ENVSTATE_NORMAL = 0,
    ENVSTATE_WARN_LOW,
    ENVSTATE_WARN_HIGH,
    ENVSTATE_CRIT_LOW,
    ENVSTATE_CRIT_HIGH,
} warn_state_t;

/*
 * table of polled values
 */
typedef struct poll_t_ {
    char *name;
    uchar a2d_select;
    uchar lastenv_key;
    uchar current_val;
    uchar lastenv_val;
} poll_t;

#define SET_SKIP_POLL	'\0'
#define SKIP_POLL(ptr)	((ptr)->name[0] == SET_SKIP_POLL)

typedef uint ci_format_t(char* buf, uchar value);
typedef int ci_convert_t(uchar value);

typedef struct env_t_ {
    poll_t poll;			/* collection data */
    ci_format_t* format;		/* format the value into the buffer */
    ci_convert_t* convert;		/* return numeric value for SNMP */
    uchar warn_low;			/* warn if at or below this */
    uchar warn_high;			/* warn if at or above this */
    uchar crit_low;			/* critical if at or below this */
    uchar crit_high;			/* critical if at or above this */
    sys_timestamp warn_suppress;	/* rate-limit warning messages */
    sys_timestamp crit_suppress;	/* rate-limit critical alerts */
    warn_state_t state;
} env_t;

#define MAX_ENVSTAT_FORMAT	32

static ci_convert_t ci_temp_celsius, ci_volt_p12_mv, ci_volt_p5_mv,
			ci_volt_n12_mv, ci_volt_p24_mv, ci_volt_2p5_mv;

static ci_format_t ci_temp, ci_volt_p12, ci_volt_p5,
			ci_volt_n12, ci_volt_p24, ci_volt_2p5;

typedef char rspbuf[sizeof("RSP(0) Hotpoint")];
static rspbuf rsp0_inlet, rsp0_hotpoint, rsp0_exhaust,
		rsp1_inlet, rsp1_hotpoint, rsp1_exhaust;

/*
 * convert +12V to a2d value with:
 *
 * define w(x) {
 *    return (((x*1000 * 10) - 59900) / 468)
 * }
 */
#define P12_WARN_LOW	120			/* 12.2  -5% */
#define P12_WARN_HIGH	    146			/* 12.2  +5% */
#define P12_CRIT_LOW		105		/* 12.2 -10% */
#define P12_CRIT_HIGH		    158		/* 12.2 +10% */

/*
 * convert +5V to a2d value with:
 *
 * define v(x) {
 *     return(((x*1000 * 10) - 25000) / 195)
 * }
 */
#define P5_WARN_LOW	125			/* 5.2  -5% */
#define P5_WARN_HIGH	    152			/* 5.2  +5% */
#define P5_CRIT_LOW		108		/* 5.2 -10% */
#define P5_CRIT_HIGH		    164		/* 5.2 +10% */

/*
 * convert -12V to a2d value with:
 *
 * define y(x) {
 * 	return ((x*1000 - 6010) / 47)
 * }
 */
#define N12_WARN_LOW	101			/* -12  -5% */
#define N12_WARN_HIGH	    154			/* -12  +5% */
#define N12_CRIT_LOW		 88		/* -12 -10% */
#define N12_CRIT_HIGH		    167		/* -12 +10% */

/*
 * convert +24V to a2d value with:
 *
 * define z(x) {
 *     return (((x*1000 * 10) - 120700) / 944)
 * }
 */
#define P24_WARN_LOW	100			/* 24 -10% */
#define P24_WARN_HIGH	    152			/* 24 +10% */
#define P24_CRIT_LOW		 88		/* 24 -15% */
#define P24_CRIT_HIGH		    165		/* 24 +15% */

/*
 * convert +2.5V to a2d value with:
 *
 * define u(x) {
 *     return (((x*1000 * 100) - 125000) / 977)
 * }
 */
#define P25_WARN_LOW	121			/* <= 2.43 */
#define P25_WARN_HIGH		129		/* >= 2.52 */

/*
 * table to drive polling of temperature & voltage values, and
 * warning thresholds.  Temp thresholds are filled-in from the
 * temp_thresh table.
 */
static env_t env_table[] = {
    { { rsp0_inlet,		MUX_RSPu0_INLET_TEMP,	KEY_TEMP_RSP0_IN, },
	  ci_temp, ci_temp_celsius, },
    { { rsp0_hotpoint,		MUX_RSPu0_HOT_SPOT,	KEY_TEMP_RSP0_HOT, },
	  ci_temp, ci_temp_celsius, },
    { { rsp0_exhaust,		MUX_RSPu0_EXHAUST,	KEY_TEMP_RSP0_EX, },
	  ci_temp, ci_temp_celsius, },
    { { rsp1_inlet,		MUX_RSPu1_INLET_TEMP,	KEY_TEMP_RSP1_IN, },
	  ci_temp, ci_temp_celsius, },
    { { rsp1_hotpoint,		MUX_RSPu1_HOT_SPOT,	KEY_TEMP_RSP1_HOT, },
	  ci_temp, ci_temp_celsius, },
    { { rsp1_exhaust,		MUX_RSPu1_EXHAUST,	KEY_TEMP_RSP1_EX, },
	  ci_temp, ci_temp_celsius, },
    { { "+12 Voltage",		MUX_PLUS12V_MONITOR,	KEY_VOLT_P12, },
	  ci_volt_p12, ci_volt_p12_mv,
	  P12_WARN_LOW, P12_WARN_HIGH, P12_CRIT_LOW, P12_CRIT_HIGH, },
    { { "+5 Voltage",		MUX_PLUS5V_MONITOR,	KEY_VOLT_P5, },
	  ci_volt_p5, ci_volt_p5_mv,
	  P5_WARN_LOW, P5_WARN_HIGH, P5_CRIT_LOW, P5_CRIT_HIGH, },
    { { "-12 Voltage",		MUX_MINUS12V_MONITOR,	KEY_VOLT_N12, },
	  ci_volt_n12, ci_volt_n12_mv,
	  N12_WARN_LOW, N12_WARN_HIGH, N12_CRIT_LOW, N12_CRIT_HIGH, },
    { { "+24 Voltage",		MUX_PLUS24V_MONITOR,	KEY_VOLT_P24, },
	  ci_volt_p24, ci_volt_p24_mv,
	  P24_WARN_LOW, P24_WARN_HIGH, P24_CRIT_LOW, P24_CRIT_HIGH, },
    { { "2.5 Reference",	MUX_VREF_2POINT5,	KEY_INVALID, },
	  ci_volt_2p5, ci_volt_2p5_mv,
	  P25_WARN_LOW, P25_WARN_HIGH, WARN_LOW_DISABLE, WARN_HIGH_DISABLE, },
};

#define ENVTABLE_SIZE	(sizeof(env_table)/sizeof(env_table[0]))

#define temp_table		(&env_table[0])
#define rsp0_inlet_env		(&env_table[0])
#define rsp0_hotpoint_env	(&env_table[1])
#define rsp0_exhaust_env	(&env_table[2])
#define rsp1_inlet_env		(&env_table[3])
#define rsp1_hotpoint_env	(&env_table[4])
#define rsp1_exhaust_env	(&env_table[5])
#define TEMP_TABLE_SIZE		6

#define volt_table		(&env_table[TEMP_TABLE_SIZE])
#define VOLT_TABLE_SIZE		(ENVTABLE_SIZE - TEMP_TABLE_SIZE)

static uchar plus12v_value;

#define PLUS12V_5PERCENT_HIGH	P12_WARN_HIGH
#define PLUS12V_5PERCENT_LOW	P12_WARN_LOW

typedef int ci_mAmp_t(uchar value, uchar bp_id, uchar ps_id);

static ci_mAmp_t ci_mAmp_p5, ci_mAmp_p12, ci_mAmp_n12;

typedef struct amp_t_ {
    poll_t poll;			/* collection data */
    ci_mAmp_t* convert;			/* convert the value into an integer */
    int voltage;			/* voltage for this test point */
    uchar* lastenv_ps_id;		/* ptr to ps_id for this saved data */
    uchar unit;				/* unit number */
    tinybool fully_loaded;		/* saved ps_fully_loaded() */
    int mAmps;				/* converted milliamp value */
    sys_timestamp maxout_suppress;	/* rate-limit MAXOUTPUT msg */
    sys_timestamp sysload_suppress;	/* rate-limit SYSLOAD msg */
    sys_timestamp unbalance_suppress;	/* rate-limit UNBALANCE msg */
} amp_t;

/*
 * table to drive polling of current data, and control warning messages
 */
static amp_t amp_table[] = {
    { { "PS1 +5V Current",	MUX_PS1_PLUS5V_CURRENT,    KEY_AMP_PS1_P5, },
	  ci_mAmp_p5,    5, &lastenv_ps1_id, 1, },
    { { "PS1 +12V Current",	MUX_PS1_PLUS12V_CURRENT,   KEY_AMP_PS1_P12, },
	  ci_mAmp_p12,  12, &lastenv_ps1_id, 1, },
    { { "PS1 -12V Current",	MUX_PS1_MINUS12V_CURRENT,  KEY_AMP_PS1_N12, },
	  ci_mAmp_n12, -12, &lastenv_ps1_id, 1, },
    { { "PS2 +5V Current",	MUX_PS2_PLUS5V_CURRENT,	   KEY_AMP_PS2_P5, },
	  ci_mAmp_p5,    5, &lastenv_ps2_id, 2, },
    { { "PS2 +12V Current",	MUX_PS2_PLUS12V_CURRENT,   KEY_AMP_PS2_P12, },
	  ci_mAmp_p12,  12, &lastenv_ps2_id, 2, },
    { { "PS2 -12V Current",	MUX_PS2_MINUS12V_CURRENT,  KEY_AMP_PS2_N12, },
	  ci_mAmp_n12, -12, &lastenv_ps2_id, 2, },
};

#define AMPTABLE_SIZE	(sizeof(amp_table)/sizeof(amp_table[0]))

#define ps1_p5_amp	(&amp_table[0])
#define ps1_p12_amp	(&amp_table[1])
#define ps1_n12_amp	(&amp_table[2])
#define ps2_p5_amp	(&amp_table[3])
#define ps2_p12_amp	(&amp_table[4])
#define ps2_n12_amp	(&amp_table[5])

#define ps1_table	(&amp_table[0])
#define PS1TABLE_SIZE	3

#define ps2_table	(&amp_table[3])
#define PS2TABLE_SIZE	3

static ushort dual_ps_status;

#define DUALPS_PS1_SYSLOAD	0x0001
#define DUALPS_PS2_SYSLOAD	(DUALPS_PS1_SYSLOAD << PS1TABLE_SIZE)
#define DUALPS_UNBALANCE	(DUALPS_PS2_SYSLOAD << PS2TABLE_SIZE)

#define LOAD_DIFF_PERCENT	10
#define OUTPUT_WARN_PERCENT	95

/*
 * X9241 EEPOT definitions
 *
 * resistor array output selected by WCR, four array/WCR pairs per EEPOT
 * registers are non-volatile (100,000 write cycles), four registers per WCR
 */

#define X9241_SLAVE_ADDR(id)	(0x50 | (id))
#define X9241_SLAVE_ADDR_BITS	8

#define NO_WCR			0
#define NO_REG			0

#define X9241_CMD_BITS		8
#define X9241_CMD_SHIFT		4
#define X9241_WCR_SHIFT		2
#define X9241_CMD(cmd,wcr,reg)	((cmd) << X9241_CMD_SHIFT | \
				 (wcr) << X9241_WCR_SHIFT | (reg))
#define   READ_WCR		0x9	/* args: wcr; 1byte returned */
#define   WRITE_WCR		0xa	/* args: wcr + 1byte data */
#define   READ_REG		0xb	/* args: wcr, reg; 1byte returned */
#define   WRITE_REG		0xc	/* args: wcr, reg + 1byte data */
#define   XFR_REG2WCR		0xd	/* args: wcr, reg */
#define   XFR_WCR2REG		0xe	/* args: wcr, reg */
#define   XFR_REG2ALLWCRS	0x1	/* args: reg */
#define   XFR_ALLWCRS2REG	0x8	/* args: reg */
#define   INCDEC_WCR		0x2	/* args: wcr */
#define   DISABLE_POT		0x10	/* pseudo command, used in parsing */

#define X9241_CASCADE_WIPER	0x80	/* enable cascade mode */
#define X9241_DISABLE_WIPER	0x40	/* disable wiper (terminal floats) */

#define X9241_MIN_CLOCK_PERIOD	10	/* max clock freq == 100KHz */
#define X9241_MIN_CLOCK_LOW	5	/* 4700ns ~= 5 usec */
#define X9241_MIN_CLOCK_HIGH	4	/* 4000ns == 4 usec */
#define X9241_TSU_STA		5	/* start setup 4700ns < 5 usec */
#define X9241_THD_STA		4	/* start hold  4000ns == 4 usec */
#define X9241_TSU_DAT		1	/* data in setup 250ns < 1 usec */
#define X9241_TSU_ACKOUT	4	/* ack out hold 3500ns < 4 usec */
#define X9241_THD_DATAOUT	1	/* data out hold 300ns < 1 usec */
#define X9241_TSU_STO		5	/* stop setup 4700ns < 5 usec */
#define X9241_TBUF		5	/* bus free  4700ns < 5 usec */
#define X9241_TWR		10	/* millisec after nv write stop */
					/* before next start */
#define X9241_TSTPWV		500	/* WCR response from stop (usec) */
#define X9241_TCLWV		500	/* WCR response from inc/dec (usec) */


#define X9241_MAXVAL		63

#define X9241_NUMPOTS		4

#define POT_TEMP_RESTART_OK	0		/* X9241 0, pot 0 */
#define POT_TEMP_INTERRUPT	1		/* X9241 0, pot 1 */
#define POT_TEMP_BOARD_SHUT	2		/* X9241 0, pot 2 */
#define POT_TEMP_PS_SHUT	3		/* X9241 0, pot 3 */
#define POT_TEMPSEL0		4		/* X9241 1, pot 0 */
#define POT_TEMPSEL1		5		/* X9241 1, pot 1 */
#define POT_BLOWER_SPEED	6		/* X9241 1, pot 2 */
#define POT_NC			7		/* X9241 1, pot 3 */

static const char
	inlet_str[] = "inlet",
	hotpoint_str[] = "hotpoint",
	exhaust_str[] = "exhaust";

static const char* const tempsel_name[] = {
    inlet_str, hotpoint_str, exhaust_str, "invalid",
};

#define TEMPSOURCE_INLET	0
#define TEMPSOURCE_HOTPOINT	1
#define TEMPSOURCE_EXHAUST	2

#define TEMPSEL_BIT0(val)	(((val) & 1) ? X9241_MAXVAL : 0)
#define TEMPSEL_BIT1(val)	(((val) & 2) ? X9241_MAXVAL : 0)

/* forward declarations of parser action functions */
static void test_ci_x9241(parseinfo*);
static void test_ci_override(parseinfo*);
static void test_ci_tempsource(parseinfo*);

#define ALTERNATE       NONE
#include "rsp_ci_test.h"
LINK_POINT(ci_test_command, ALTERNATE);
#undef  ALTERNATE

/*
 * private data objects
 */
static uchar real_backplane_id;		/* backplane ID from ci->BPlane_ID */
static uchar backplane_id;		/* "virtual" backplane ID:
					 *   7000/7010 mapped to 7506/7505 */

static uchar fan_count;			/* number of fans in chassis */
static uchar ps_count;			/* number of power supplies this
					 *  chassis can hold */
static ushort postinit_minutes;		/* number of minutes since boot */

static chasint_dbus_t *chasint;		/* ptr to CI's dbus registers */

static const char *slothdr;		/* slot present header */

static const char *temp_source;		/* name for temp source card */

/*
 * snmp platform api support
 */
static chassis_type_t chassis_type;	/* chassisType of the chassis */
static card_type_t processor_type;	/* cardType of the cpu card */
static int chassis_slots;		/* number of slots in chassis */
static OID  system_id;			/* sysObjectID of the chassis */

static inline boolean
dbus_present_inline (uchar *dbus, uint slot)
{
    if (backplane_id == BACKPLANE_7507)
	slot += BP7507_OFFSET;
    return ((dbus[slot / SLOTBITS_PER_BYTE] &
	     (1 << (slot & (SLOTBITS_PER_BYTE - 1)))) != 0);
}

#define DBUS_PRESENT(dba, slot)		dbus_present_inline((dba), (slot))

/*
 * power supply name arrays
 */
static char* const
psarray_7507[] = { "unknown", "850W",     "700W",     "removed", };

static char* const
psarray_7505[] = { "unknown", "600W AC",  "600W DC",  "error", };

static char* const
psarray_7513[] = {
    "TBD",
    "1200W AC",
    "1200W DC",
    "TBD",
    "TBD",
    "TBD",
    "TBD",
    "removed",
};

#define BP7513_PSREMOVED	7
#define BP7507_PSREMOVED	3

/*
 * convert degrees C to A2D value:
 *
 * define t(x) {
 *     return((x + 148) * 1000 / 977)
 * }
 */
#define C7505_IN_WARN	197	/* 44 degrees C */
#define C7505_IN_CRIT	203	/* 50 degrees C */
#define C7505_HOT_WARN	207	/* 54 degrees C */
#define C7505_HOT_CRIT	213	/* 60 degrees C */

#define C7000_IN_WARN	197	/* 44 degrees C */
#define C7000_IN_CRIT	203	/* 50 degrees C */
#define C7000_HOT_WARN	207	/* 54 degrees C */
#define C7000_HOT_CRIT	213	/* 60 degrees C */

/*
 * convert degrees C to WCR value:
 *
 * define p(x) {
 *     return(((x * 100) + 2315) * 63 / 12500)
 * }
 *
 * the interrupt level is never used by the sw but is a valid part
 * of the hw state machine, so specify it as one less than the
 * board shutdown level.
 */
#define C7505_RSP1_RESTART	32	/* 40 degrees C */
#define C7505_RSP1_INTERRUPT	(C7505_RSP1_BOARD_SHUT - 1)
#define C7505_RSP1_BOARD_SHUT	47	/* 70 degrees C */
#define C7505_RSP1_PS_SHUT	50	/* 76 degrees C */

/*
 * for RSP7000, RESTART is not operational, but to keep the CI state
 * machine sane it needs to be less than PS_SHUT
 * 
 * The CI won't respond to dbus accesses when BOARD_SHUT is true, so
 * make sure the CI transitions immediately from BOARD_SHUT to PS_SHUT
 */
#define C7000_RESTART		(C7000_PS_SHUT - 1)
#define C7000_INTERRUPT		(C7000_BOARD_SHUT - 1)
#define C7000_BOARD_SHUT	47	/* 70 degrees C */
#define C7000_PS_SHUT		(C7000_BOARD_SHUT - C7000_PS_BOARD_DIFF)
#define C7000_PS_BOARD_DIFF	1

/*
 * CI7000 lives in next higher slot than RSP7000
 */
#define RSP7000_TO_CI7000(slot)	((slot) + 1)

/*
 * table of temperature threshold values for different RSP models in
 * different chassis
 */
static const struct {
    uchar inlet_warn, inlet_crit,
    	  hotpoint_warn, hotpoint_crit,
    	  exhaust_warn, exhaust_crit,
    	  restart_ok, interrupt, board_shut, ps_shut,
    	  tempsel;
} temp_thresh[] = {
#define THRESH_DEFAULT	 0		/* maximum value thresholds */
			    { A2D_MAXVAL, A2D_MAXVAL,
				  A2D_MAXVAL, A2D_MAXVAL,
				  A2D_MAXVAL, A2D_MAXVAL,
				  X9241_MAXVAL,
				  X9241_MAXVAL,
				  X9241_MAXVAL,
				  X9241_MAXVAL,
			  	  TEMPSOURCE_HOTPOINT, },
#define THRESH_RSP1_7505 1
			    { C7505_IN_WARN, C7505_IN_CRIT,
				  C7505_HOT_WARN, C7505_HOT_CRIT,
				  A2D_MAXVAL, A2D_MAXVAL,
				  C7505_RSP1_RESTART,
				  C7505_RSP1_INTERRUPT,
				  C7505_RSP1_BOARD_SHUT,
				  C7505_RSP1_PS_SHUT,
				  TEMPSOURCE_HOTPOINT, },
#define THRESH_RSP7000	2
			    { C7000_IN_WARN, C7000_IN_CRIT,
				  C7000_HOT_WARN, C7000_HOT_CRIT,
				  A2D_MAXVAL, A2D_MAXVAL,
				  C7000_RESTART,
				  C7000_INTERRUPT,
				  C7000_BOARD_SHUT,
				  C7000_PS_SHUT,
				  TEMPSOURCE_HOTPOINT, },
};

boolean envmib_load = FALSE;     /* global to tell mibcode ok to load */


/************************** X9241 interface code *************************/

/*
 * DRAM copy of CI's Power_Ctl reg with SDA & SCL clear
 * useful only if Power_Ctl isn't changed from interrupt level
 * can change if thread suspends
 */
static uchar Power_Ctl;

/*
 * START is a transition of data from high to low with clock high
 */
static void
x9241_start (void)
{
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SCL | POWER_CTL_SDA;
    chasint->Power_Ctl;
    usecdelay(X9241_TSU_STA);
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SCL; /* SDA == 0 */
    chasint->Power_Ctl;
    usecdelay(X9241_THD_STA);
    chasint->Power_Ctl = Power_Ctl; /* SDA == 0, SCL == 0 */
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_LOW);
    return;
}

/*
 * STOP is a transition of data from low to high with clock high
 */
static void
x9241_stop (void)
{
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SCL; /* SDA == 0 */
    chasint->Power_Ctl;
    usecdelay(X9241_TSU_STO);
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SCL | POWER_CTL_SDA;
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_HIGH);
    chasint->Power_Ctl = Power_Ctl; /* SDA == 0, SCL == 0 */
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_LOW);
    return;
}

/*
 * send N bits on X9241 bus
 */
static void
x9241_sendbits (uchar val, uint nbits)
{
    uchar bit;

    while (nbits > 0) {
	--nbits;
	bit = val & (1 << nbits);
	if (bit)
	    bit = POWER_CTL_SDA;
	chasint->Power_Ctl = Power_Ctl | bit | POWER_CTL_SCL;
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_HIGH);
	chasint->Power_Ctl = Power_Ctl | bit;	/* SCL == 0 */
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_LOW);
    }
    return;
}

/*
 * read N bits from X9241 bus
 */
static void
x9241_readbits (uchar *val, uint nbits)
{
    uchar bit, bits;

    bits = 0;
    while (nbits > 0) {
	--nbits;
	chasint->Power_Ctl = Power_Ctl | POWER_CTL_SDA | POWER_CTL_SCL;
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_HIGH);
	bit = chasint->Power_Ctl & POWER_CTL_SDA;
	chasint->Power_Ctl = Power_Ctl | POWER_CTL_SDA;	/* SCL == 0 */
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_LOW);
	if (bit)
	    bits |= (1 << nbits);
    }
    *val = bits;
    return;
}

/*
 * check for ACK from X9241
 */
static boolean
x9241_read_ack (void)
{
    uchar ack;

    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SDA;
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_LOW);
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SDA | POWER_CTL_SCL;
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_HIGH);
    if (chasint->Power_Ctl & POWER_CTL_SDA)
	ack = 0;
    else
	ack = 1;
    chasint->Power_Ctl = Power_Ctl | POWER_CTL_SDA; /* SCL == 0 */
    chasint->Power_Ctl;
    usecdelay(X9241_MIN_CLOCK_LOW);
    return ack;
}

/************************* EEPOT access routines *************************/

typedef enum {
    EEPOT_OKAY = 1,
    EEPOT_ACK_ADDR,		/* no address ACK */
    EEPOT_ACK_CMD,		/* no cmd ACK */
    EEPOT_ACK_WRDATA,		/* no write data ACK */
} eepot_return_t;

/*
 * issue a X9241 command and return status
 */
static eepot_return_t
eepot_cmd_quiet (uchar addr, uchar cmd, uchar *data)
{
    eepot_return_t retv;
    uchar initial_power_ctl;
    static tinybool wcr_write_waiting = FALSE;
    static sys_timestamp wcr_write_wait;

    if (wcr_write_waiting) {
	while (XSLEEPING(wcr_write_wait, X9241_TWR))
	    /* spin */;
	wcr_write_waiting = FALSE;
    }
    initial_power_ctl = chasint->Power_Ctl;
    chasint->Power_Ctl = (initial_power_ctl | POWER_CTL_OVERRIDE);

    Power_Ctl = chasint->Power_Ctl & ~(POWER_CTL_SDA | POWER_CTL_SCL);

    retv = EEPOT_OKAY;
    x9241_start();
    x9241_sendbits(X9241_SLAVE_ADDR(addr), X9241_SLAVE_ADDR_BITS);
    if (!x9241_read_ack()) {
	retv = EEPOT_ACK_ADDR;
    } else {
	x9241_sendbits(cmd, X9241_CMD_BITS);
	if (!x9241_read_ack()) {
	    retv = EEPOT_ACK_CMD;
	} else {
	    switch (cmd >> X9241_CMD_SHIFT) {
	      case READ_WCR:
	      case READ_REG:
		x9241_readbits(data, 8 /* always returns one octet */);
		break;

	      case WRITE_WCR:
	      case WRITE_REG:
		x9241_sendbits(*data, 8 /* always write one octet */);
		if (!x9241_read_ack())
		    retv = EEPOT_ACK_WRDATA;
		break;
	    }
	}
    }
    x9241_stop();
    switch (cmd >> X9241_CMD_SHIFT) {
      case WRITE_REG:
      case XFR_WCR2REG:
      case XFR_ALLWCRS2REG:
	TIMER_START(wcr_write_wait, X9241_TWR);
	wcr_write_waiting = TRUE;
	break;

      case WRITE_WCR:
      case XFR_REG2WCR:
      case XFR_REG2ALLWCRS:
	usecdelay(X9241_TSTPWV);
	/* fallthrough... */

      default:
	wcr_write_waiting = FALSE;
	break;
    }

    if ((initial_power_ctl & POWER_CTL_OVERRIDE) == 0)
	chasint->Power_Ctl &= ~POWER_CTL_OVERRIDE;

    return retv;
}

/*
 * issue a X9241 command and call errmsg on failure
 */
static boolean
eepot_cmd (uchar addr, uchar cmd, uchar *data)
{
    char *errstr = "";

    switch (eepot_cmd_quiet(addr, cmd, data)) {
      case EEPOT_OKAY:
	return TRUE;

      case EEPOT_ACK_ADDR:
	errstr = "address";
	break;

      case EEPOT_ACK_CMD:
	errstr = "command";
	break;

      case EEPOT_ACK_WRDATA:
	errstr = "write data";
	break;
    }
    errmsg(&msgsym(NOACK, CI), errstr, cmd, addr, *data);
    return FALSE;
}

/*
 * check the value in the WCR, and if it doesn't match the supplied value,
 * write both the WCR and register 0
 */
static void
x9241_check_or_set (uchar x9241_pot, uchar val)
{
    uchar x9241, pot;

    pot = x9241_pot % X9241_NUMPOTS;
    x9241 = x9241_pot / X9241_NUMPOTS;
    for (;;) {
	uchar data;

	if (!eepot_cmd(x9241, X9241_CMD(READ_WCR, pot, NO_REG), &data))
	    break;
	if (data == val)
	    break;
	data = val;
	if (!eepot_cmd(x9241, X9241_CMD(WRITE_WCR, pot, NO_REG), &data))
	    break;
	eepot_cmd(x9241, X9241_CMD(XFR_WCR2REG, pot, 0), &data);
	break;
    }
    return;
}

/*
 * write a new value into the WCR
 */
static void
write_pot (uchar x9241_pot, uchar val)
{
    uchar x9241, pot;

    pot = x9241_pot % X9241_NUMPOTS;
    x9241 = x9241_pot / X9241_NUMPOTS;
    eepot_cmd(x9241, X9241_CMD(WRITE_WCR, pot, NO_REG), &val);
    return;
}

/*
 * read the value in the WCR
 */
static uchar
read_pot (uchar x9241_pot)
{
    uchar val, x9241, pot;

    pot = x9241_pot % X9241_NUMPOTS;
    x9241 = x9241_pot / X9241_NUMPOTS;
    eepot_cmd(x9241, X9241_CMD(READ_WCR, pot, NO_REG), &val);
    return val;
}

/******************************* A2D Access ******************************/

#define SAMPLE_COUNT	16	/* oversample converted value */
#define A2D_DELAY_MS	10	/* delay to allow A/D to complete */

typedef enum {
    A2D_IDLE = 1,
    A2D_BACKGROUND,
    A2D_INTERACTIVE,
} a2d_state_t;

static a2d_state_t a2d_state;

/*
 * select a2d mux value
 */
static inline void
select_a2d (uchar input_select, uint dismiss, boolean interactive)
{
    a2d_state_t state;

    state = (interactive) ? A2D_INTERACTIVE : A2D_BACKGROUND;
    for (;;) {
	switch (a2d_state) {
	  case A2D_IDLE:
	    break;

	  case A2D_BACKGROUND:
	    if (interactive) {
		a2d_state = A2D_INTERACTIVE;
		break;
	    }
	    /* fall through to... */

	  case A2D_INTERACTIVE:
	    process_sleep_for(ONESEC/10);
	    if (dismiss > (ONESEC/10 + A2D_DELAY_MS))
		dismiss -= ONESEC/10;
	    continue;		/* 'for' loop */
	}
	a2d_state = state;
	chasint->AD_Ctl = input_select;
	process_sleep_for(dismiss);
    	if (a2d_state == state)
	    break;
    }
    return;
}

/*
 * return value read from a2d result register
 */
static inline uchar
read_a2d (void)
{
    int samples;
    ushort val_sum;

    for (val_sum = 0, samples = 0; samples < SAMPLE_COUNT; ++samples)
	val_sum += chasint->AD_Result;
    a2d_state = A2D_IDLE;
    return val_sum / SAMPLE_COUNT;
}

/*
 * read value from A2D registers (don't call this from interrupt level)
 */
static uchar
ci_read_a2d (uchar input_select)
{
    uchar val;

    select_a2d(input_select, A2D_DELAY_MS, TRUE);
    val = read_a2d();
    return val;
}

/*************************** "test ci" routines ***************************/

/*
 * read & write X9241 devices from command line
 */
static void
test_ci_x9241 (parseinfo *csb)
{
    uchar addr, cmd, wcr, reg, data;

    addr = GETOBJ(int,1);
    cmd  = GETOBJ(int,2);
    wcr  = GETOBJ(int,3);
    reg  = GETOBJ(int,4);
    data = GETOBJ(int,5);

    if (cmd == DISABLE_POT) {
	cmd  = WRITE_WCR;
	data = X9241_DISABLE_WIPER;
    }

    switch (eepot_cmd_quiet(addr, X9241_CMD(cmd, wcr, reg), &data)) {
      static char noack_fmt[] = "%%%s not acknowledged";

      case EEPOT_OKAY:
	if ((cmd == READ_WCR) || (cmd == READ_REG))
	    printf("decimal value: %u", data);
	break;

      case EEPOT_ACK_ADDR:
	printf(noack_fmt, "address");
	break;

      case EEPOT_ACK_CMD:
	printf(noack_fmt, "command");
	break;

      case EEPOT_ACK_WRDATA:
	printf(noack_fmt, "write data");
	break;
    }
    return;
}

/*
 * inhibit/enable CI-initiated power shutdown
 */
static void
test_ci_override (parseinfo *csb)
{
    if (GETOBJ(int,1))	/* turn override "off" */
	chasint->Power_Ctl &= ~POWER_CTL_OVERRIDE;
    else
	chasint->Power_Ctl |= POWER_CTL_OVERRIDE;
    return;
}

/*
 * return slot number for temp sensor unit 0 or 1
 */
static uint
rsp_slot (uint rsp)
{
    switch (real_backplane_id) {
      case BACKPLANE_7505:	rsp += RSPSLOT_7505;		break;
      case BACKPLANE_7506:	rsp += RSPSLOT_7506;		break;
      case BACKPLANE_7507:	rsp += RSPSLOT_7507_PROC1;	break;
      case BACKPLANE_7513:
      case BACKPLANE_TOKTEST:	rsp += RSPSLOT_7513_PROC1;	break;
      case BACKPLANE_7000:
	switch (backplane_id) {
	  case BACKPLANE_7505:	rsp += RSP7000_TO_CI7000(RSPSLOT_7010);	break;
	  case BACKPLANE_7506:	rsp += RSP7000_TO_CI7000(RSPSLOT_7000);	break;
	}
    }
    return rsp;
}

/*
 * control from which RSP testpoint the CI's temperature measurement
 * is sourced.  
 */
static void
test_ci_tempsource (parseinfo *csb)
{
    int slot;
    uchar initial_power_ctl;

    initial_power_ctl = chasint->Power_Ctl;
    chasint->Power_Ctl = (initial_power_ctl | POWER_CTL_OVERRIDE);
    for (;;) {
	slot = GETOBJ(int,1);
	if (rsp_slot(0) == slot)
	    chasint->Power_Ctl &= ~POWER_CTL_TEMPSEL;
	else if (rsp_slot(1) == slot)
	    chasint->Power_Ctl |= POWER_CTL_TEMPSEL;
	else {
	    printf("%% invalid slot");
	    break;
	}
	write_pot(POT_TEMPSEL0, TEMPSEL_BIT0(GETOBJ(int,2)));
	write_pot(POT_TEMPSEL1, TEMPSEL_BIT1(GETOBJ(int,2)));
	break;
    }
    usecdelay(X9241_TSTPWV);
    if ((initial_power_ctl & POWER_CTL_OVERRIDE) == 0)
	chasint->Power_Ctl &= ~POWER_CTL_OVERRIDE;

    return;
}

/**************************** "show env" routines ************************/

/*
 * convert backplane ID into text string
 */
static char*
bp_name (uint bp_id)
{
    switch (bp_id) {
      case BACKPLANE_7506:	return "7506";
      case BACKPLANE_7505:	return "7505";
      case BACKPLANE_7513:	return "7513";
      case BACKPLANE_TOKTEST:	return "Tokamak_Test";
      case BACKPLANE_7507:	return "7507";
      case BACKPLANE_7000:	return "70x0";
    }
    return "unknown";
}

#define NELEMENTS(x)	(sizeof(x)/sizeof(*x))

/*
 * convert power supply ID into text string
 */
static char*
ps_name (uint ps_id, uint bp_id)
{
    switch (bp_id) {
      case BACKPLANE_7507:
      case BACKPLANE_7506:
	if (ps_id < NELEMENTS(psarray_7507))
	    return psarray_7507[ps_id];
	break;

      case BACKPLANE_7505:
	if (ps_id < NELEMENTS(psarray_7505))
	    return psarray_7505[ps_id];
	break;

      case BACKPLANE_7513:
	if (ps_id < NELEMENTS(psarray_7513))
	    return psarray_7513[ps_id];
	break;
    }
    return "unknown";
}

/*
 * display temperature values for an RSP
 */
static void
rsptemp (int rsp)
{
    int inlet, hotspot, exhaust;
    char inbuf[MAX_ENVSTAT_FORMAT];
    char hotbuf[MAX_ENVSTAT_FORMAT];
    char exbuf[MAX_ENVSTAT_FORMAT];

    if (rsp == 0) {
	inlet   = MUX_RSPu0_INLET_TEMP;
	hotspot = MUX_RSPu0_HOT_SPOT;
	exhaust = MUX_RSPu0_EXHAUST;
    } else {
	inlet   = MUX_RSPu1_INLET_TEMP;
	hotspot = MUX_RSPu1_HOT_SPOT;
	exhaust = MUX_RSPu1_EXHAUST;
    }

    inlet   = ci_read_a2d(inlet);
    hotspot = ci_read_a2d(hotspot);
    exhaust = ci_read_a2d(exhaust);

    if ((inlet == 0) && (hotspot == 0) && (exhaust == 0))
	return;

    ci_temp(inbuf, inlet);
    ci_temp(hotbuf, hotspot);
    ci_temp(exbuf, exhaust);

    printf("\n%-3s(%u)   %9s    %9s    %9s",
	   temp_source, rsp_slot(rsp), inbuf, hotbuf, exbuf);

    return;
}

/*
 * determine whether there's a power supply installed
 */
static boolean
ps_removed (uchar ps_id)
{
    switch (backplane_id) {
      case BACKPLANE_7506:
      case BACKPLANE_7507: return (ps_id == BP7507_PSREMOVED);
      case BACKPLANE_7513: return (ps_id == BP7513_PSREMOVED);
    }
    return TRUE;
}

/*
 * determine whether or not to believe reports of fan failure
 */
static boolean
believe_7513fan (uchar fan)
{
    uchar ps_id;

    switch (fan) {
      default:
	return FALSE;

      case BP7513_FAN_MAIN:
	return TRUE;

      case BP7513_FAN_PS1:
	ps_id = PS_ID_PS1_ID(chasint->PS_ID);
	break;

      case BP7513_FAN_PS2:
	ps_id = PS_ID_PS2_ID(chasint->PS_ID);
	break;
    }
    return !ps_removed(ps_id);
}

/*
 * get fault status, adjusted for backplane type
 */
static uchar
get_int_status (chasint_dbus_t* ci)
{
    uchar int_stat;

    int_stat = ci->Int_Status;
    switch (backplane_id) {
      case BACKPLANE_TOKTEST:
	int_stat &= ~(INT_STATUS_PS1_BIT | INT_STATUS_PS2_BIT |
		      INT_STATUS_BFAIL_BIT | INT_STATUS_BMASK);
	break;

      case BACKPLANE_7505:
	int_stat &= ~(INT_STATUS_PS2_BIT | INT_STATUS_PS1_BIT);
	break;

      case BACKPLANE_7513:
	if (INT_STATUS_BFAIL(int_stat)) {
	    uchar fan;

	    fan = INT_STATUS_BLOWERNUM(int_stat);
	    if (!believe_7513fan(fan))
		int_stat &= ~(INT_STATUS_BFAIL_BIT | INT_STATUS_BMASK);
	}
	goto ps_chk;

      case BACKPLANE_7506:
      case BACKPLANE_7507:
	int_stat &= ~(INT_STATUS_BFAIL_BIT | INT_STATUS_BMASK);
	if (real_backplane_id == BACKPLANE_7000) {
	    uchar stat;

	    /*
	     * the CI7000 has these bits swapped
	     */
	    stat = int_stat & ~(INT_STATUS_PS2_BIT | INT_STATUS_PS1_BIT);
	    if (int_stat & INT_STATUS_PS1_BIT)
		stat |= INT_STATUS_PS2_BIT;
	    if (int_stat & INT_STATUS_PS2_BIT)
		stat |= INT_STATUS_PS1_BIT;
	    int_stat = stat;
	}
ps_chk:	if (INT_STATUS_PS1_FAIL(int_stat) &&
	    ps_removed(PS_ID_PS1_ID(ci->PS_ID)))
	    int_stat &= ~INT_STATUS_PS1_BIT;
	if (INT_STATUS_PS2_FAIL(int_stat) &&
	    ps_removed(PS_ID_PS2_ID(ci->PS_ID)))
	    int_stat &= ~INT_STATUS_PS2_BIT;
	break;
    }
    return int_stat;
}

/*
 * print voltage info
 */
static void
rsp_voltage (int *p5_mv, int *p12_mv, int *n12_mv)
{
    uchar val;
    char vbuf[MAX_ENVSTAT_FORMAT];

    ci_volt_p12(vbuf, val = ci_read_a2d(MUX_PLUS12V_MONITOR));
    printf("\n+12V measured at %s", vbuf);
    *p12_mv = ci_volt_p12_mv(val);

    ci_volt_p5(vbuf, val = ci_read_a2d(MUX_PLUS5V_MONITOR));
    printf("\n +5V measured at %s", vbuf);
    *p5_mv = ci_volt_p5_mv(val);

    ci_volt_n12(vbuf, val = ci_read_a2d(MUX_MINUS12V_MONITOR));
    printf("\n-12V measured at %s", vbuf);
    *n12_mv = ci_volt_n12_mv(val);

    ci_volt_p24(vbuf, ci_read_a2d(MUX_PLUS24V_MONITOR));
    printf("\n+24V measured at %s", vbuf);

    ci_volt_2p5(vbuf, ci_read_a2d(MUX_VREF_2POINT5));
    printf("\n+2.5 reference is %s", vbuf);

    return;
}

/*
 * format a milliamp value for output
 */
static uint
ci_mAmp (char* buf, uint ma)
{
    return sprintf(buf, "%u.%02u", MILLI_TO_UNITS(ma), HUNDREDTHS(ma));
}

/*
 * given the current backplane type, name the indicated fan
 */
static char*
fan_name (uint fan_num)
{
    switch (backplane_id) {
      case BACKPLANE_7505: {
	  static char fan_name_7505[sizeof("#6")];

	  if ((fan_num >= 1) && (fan_num <= FANS_7505)) {
	      sprintf(fan_name_7505, "#%u", fan_num);
	      return fan_name_7505;
	  }
	  break;
      }

      case BACKPLANE_7506:
      case BACKPLANE_7507:
	if (fan_num == 1)
	    return "main";
	break;

      case BACKPLANE_7513:
	switch (fan_num) {
	  case BP7513_FAN_MAIN:	return "main";
	  case BP7513_FAN_PS1:	return "ps1";
	  case BP7513_FAN_PS2:	return "ps2";
	}
	break;
    }
    return "unknown";
}

/*
 * return power supply capacity (in milliamps)
 */
static uint
ps_capacity (uchar bp_id, uchar ps_id, int voltage)
{
    switch (bp_id) {
      case BACKPLANE_7513:
	switch (ps_id) {
	  case BP7513PS1200WAC:
	  case BP7513PS1200WDC:
	    switch (voltage) {
	      case   5: return BP7513_1200W_P5_CAPACITY;
	      case  12: return BP7513_1200W_P12_CAPACITY;
	      case -12: return BP7513_1200W_N12_CAPACITY;
	      case  24: return BP7513_1200W_P24_CAPACITY;
	    }
	    break;
	}
	break;
    }
    return 0;
}

/*
 * returns TRUE if current output is near rated capacity
 */
static boolean
ps_fully_loaded (amp_t* amp, uchar ps_id)
{
    uint ps_cap;

    ps_cap = ps_capacity(backplane_id, ps_id, amp->voltage);
    if (ps_cap > 0) {
	amp->mAmps = amp->convert(amp->poll.current_val, backplane_id, ps_id);
	if ((amp->mAmps * 100 / ps_cap) >= OUTPUT_WARN_PERCENT)
	    return TRUE;
    }
    return FALSE;
}

/*
 * print current measurement, return milliamps
 */
static uint
rsp_current (amp_t* amp, uchar ps_id)
{
    uchar val;
    int ma, cap_ma;
    char envbuf[MAX_ENVSTAT_FORMAT];

    val = ci_read_a2d(amp->poll.a2d_select);
    ma = amp->convert(val, backplane_id, ps_id);
    if (ma < 0)
	return 0;
    ci_mAmp(envbuf, ma);
    printf("\n%18s measured at %s A", amp->poll.name, envbuf);
    cap_ma = ps_capacity(backplane_id, ps_id, amp->voltage);
    if (cap_ma > 0) {
	printf(" (capacity %u A)", MILLI_TO_UNITS(cap_ma));
    }
    return ma;
}

/*
 * "show env all" - show all possible information
 */
static void
show_env_all (void)
{
    int mv_p5, mv_p12, mv_n12;
    uchar val, ps1_id, ps2_id, power_ctl, int_stat;

    printf("Arbiter type %u, backplane type %s (id %u)",
	   1 + BPLANE_ID_ARB_ID(chasint->BPlane_ID),
	   bp_name(real_backplane_id), real_backplane_id);

    ps1_id = PS_ID_PS1_ID(chasint->PS_ID);
    ps2_id = PS_ID_PS2_ID(chasint->PS_ID);
    if (backplane_id != BACKPLANE_TOKTEST) {
	printf("\nPower supply #1 is %s (id %u)",
	       ps_name(ps1_id, backplane_id), ps1_id);
	if (ps_count > 1)
	    printf(", power supply #2 is %s (id %u)",
		   ps_name(ps2_id, backplane_id), ps2_id);
    }

    int_stat = get_int_status(chasint);
    printf("\nActive fault conditions: ");
    if (int_stat == 0)
	printf("none");
    else {
	if (INT_STATUS_TEMP(int_stat))
	    printf("Temperature ");
	if (INT_STATUS_PS1_FAIL(int_stat))
	    printf("PS1 ");
	if (INT_STATUS_PS2_FAIL(int_stat))
	    printf("PS2 ");
	if (INT_STATUS_BFAIL(int_stat))
	    printf("Fan(%s)", fan_name(INT_STATUS_BLOWERNUM(int_stat)));
    }

    if (variable_blower()) {
	val = read_pot(POT_BLOWER_SPEED);
	printf("\nFan transfer point: %u%%",
	       (1 + val) * 100 / (1 + X9241_MAXVAL));
    }

    val = chasint->Int_Mask;
    printf("\nActive trip points: ");
    if (INT_TRIP_POINTS(val) == 0)
	printf("none");
    else {
	if (INT_MASK_RESTART_OK(val))
	    printf("Restart_Inhibit ");
	if (INT_MASK_TEMP_WARN(val))
	    printf("Temperature_Warning ");
	if (INT_MASK_BOARD_SHUTDOWN(val))
	    printf("Board_Shutdown ");
	if (INT_MASK_PS_SHUTDOWN(val))
	    printf("PS_Shutdown ");
    }

    if (soft_shutdown_works()) {
	uchar power_status;

	power_status = chasint->Power_Status;
	printf("\n%u of %u soft shutdowns remaining before hard shutdown",
	       (POWER_STATUS_TRIP_POINT(power_status) -
		POWER_STATUS_CYCLE_COUNT(power_status)),
	       POWER_STATUS_TRIP_POINT(power_status));
    }

    power_ctl = chasint->Power_Ctl;
    if (power_ctl & POWER_CTL_OVERRIDE)
	printf("\n\nWARNING: Automatic power-shutdown is inhibited!");

    printf("\n\n"
	   "            %s\n"
	   "Dbus slots: ", slothdr);
    for (val = 0; val < MAX_SLOTS; ++val) {
	if (DBUS_PRESENT(chasint->DBUS, val))
	    printf("X");
	else
	    printf(" ");
    }

    printf("\n\n"
	   " card     inlet       hotpoint     exhaust");
    rsptemp(0);
    rsptemp(1);

    val = (((read_pot(POT_TEMPSEL1)) ? 2 : 0) |
	   ((read_pot(POT_TEMPSEL0)) ? 1 : 0));
    printf("\n\n"
	   "Shutdown temperature source is '%s' on %s(%u), requested %s(%u)\n",
	   tempsel_name[val],
	   temp_source, rsp_slot((power_ctl & POWER_CTL_TEMPR)   != 0),
	   temp_source, rsp_slot((power_ctl & POWER_CTL_TEMPSEL) != 0));

    rsp_voltage(&mv_p5, &mv_p12, &mv_n12);

    printf("\n");

    if (measure_current()) {
	uint ps_power;

	if (!ps_removed(ps1_id) && !INT_STATUS_PS1_FAIL(int_stat)) {
	    ps_power  =  mv_p5  * rsp_current(ps1_p5_amp,  ps1_id);
	    ps_power +=  mv_p12 * rsp_current(ps1_p12_amp, ps1_id);
	    ps_power += -mv_n12 * rsp_current(ps1_n12_amp, ps1_id);
	    printf("\nPS%u output is %u W\n", 1, ps_power / 1000000);
	}
	if (!ps_removed(ps2_id) && !INT_STATUS_PS2_FAIL(int_stat)) {
	    ps_power  =  mv_p5  * rsp_current(ps2_p5_amp,  ps2_id);
	    ps_power +=  mv_p12 * rsp_current(ps2_p12_amp, ps2_id);
	    ps_power += -mv_n12 * rsp_current(ps2_n12_amp, ps2_id);
	    printf("\nPS%u output is %u W\n", 2, ps_power / 1000000);
	}
    }

    return;
}

/*
 * "show env" - just give thumbs up/down
 */
static void
show_env_default (void)
{
    int ix;
    uchar val;
    boolean warned;

    warned = FALSE;
    for (ix = 0; ix < ENVTABLE_SIZE; ++ix) {
	env_t* ev;
	char* level;
	char envbuf[MAX_ENVSTAT_FORMAT];

	ev = &env_table[ix];
	val = ev->poll.current_val;
	if (val == 0)
	    continue;
	switch (ev->state) {
	  case ENVSTATE_WARN_LOW:
	  case ENVSTATE_WARN_HIGH:	level = "Warning";	break;
	  case ENVSTATE_CRIT_LOW:
	  case ENVSTATE_CRIT_HIGH:	level = "CRITICAL";	break;
	  default:						continue;
	}
	warned = TRUE;
	ev->format(envbuf, val);
	printf("\n  %s - %18s measured at %s", level, ev->poll.name, envbuf);
    }
    if (measure_current()) {
	for (ix = 0; ix < AMPTABLE_SIZE; ++ix) {
	    amp_t* amp;

	    amp = &amp_table[ix];
	    val = amp->poll.current_val;
	    if (val == 0)
		continue;
	    if (amp->fully_loaded) {
		warned = TRUE;
		printf("\n  Warning - %18s is at or near rated output",
		       amp->poll.name);
	    }
	}
	for (ix = 0; ix < PS1TABLE_SIZE; ++ix) {
	    if (dual_ps_status & (DUALPS_PS1_SYSLOAD << ix)) {
		warned = TRUE;
		printf("\n  Warning - %dV load exceeds PS%u capacity",
		       ps1_table[ix].voltage, 1);
	    }
	    if (dual_ps_status & (DUALPS_PS2_SYSLOAD << ix)) {
		warned = TRUE;
		printf("\n  Warning - %dV load exceeds PS%u capacity",
		       ps2_table[ix].voltage, 2);
	    }
	    if (dual_ps_status & (DUALPS_UNBALANCE << ix)) {
		warned = TRUE;
		printf("\n  Warning - large %dV output difference "
		       "between power supplies", ps1_table[ix].voltage);
	    }
	}
    }
    if (!warned)
	printf("\nAll measured values are normal");
    return;
}

/*
 * display EEPOT temperature thresholds
 */
static void
show_x9241_temps (void)
{
    uchar val;
    uint degreesC;

    if (real_backplane_id != BACKPLANE_7000) {
	val = read_pot(POT_TEMP_BOARD_SHUT);
	degreesC = TRIP_TEMP_CELSIUS(val);
	printf("\n%28s %dC/%dF", "Shutdown boards at",
	       degreesC, DEGREES_C2F(degreesC));
    }

    val = read_pot(POT_TEMP_PS_SHUT);
    if (real_backplane_id == BACKPLANE_7000)
	val += C7000_PS_BOARD_DIFF;
    degreesC = TRIP_TEMP_CELSIUS(val);
    printf("\n%28s %dC/%dF", "Shutdown power supplies at",
	   degreesC, DEGREES_C2F(degreesC));

    if (real_backplane_id != BACKPLANE_7000) {
	val = read_pot(POT_TEMP_RESTART_OK);
	degreesC = TRIP_TEMP_CELSIUS(val);
	printf("\n%28s %dC/%dF", "Restart after shutdown below",
	       degreesC, DEGREES_C2F(degreesC));
    }

    return;
}

/*
 * display table of environmental values
 */
static void
show_env_table (void)
{
    int ix;

    printf("\n%18s%15s%15s%15s%15s", "Sample Point",
	   "LowCritical", "LowWarning", "HighWarning", "HighCritical");
    for (ix = 0; ix < ENVTABLE_SIZE; ++ix) {
	env_t* ev;
	char* ptr;
	char envbuf[MAX_ENVSTAT_FORMAT];

	ev = &env_table[ix];
	if (SKIP_POLL(&ev->poll))
	    continue;

	printf("\n%18s", ev->poll.name);

	if (ev->crit_low != WARN_LOW_DISABLE)
	    ev->format(ptr = envbuf, ev->crit_low);
	else
	    ptr = "";
	printf("%15s", ptr);

	if (ev->warn_low != WARN_LOW_DISABLE)
	    ev->format(ptr = envbuf, ev->warn_low);
	else
	    ptr = "";
	printf("%15s", ptr);

	if (ev->warn_high != WARN_HIGH_DISABLE)
	    ev->format(ptr = envbuf, ev->warn_high);
	else
	    ptr = "";
	printf("%15s", ptr);

	if (ev->crit_high != WARN_HIGH_DISABLE)
	    ev->format(ptr = envbuf, ev->crit_high);
	else
	    ptr = "";
	printf("%15s", ptr);
    }
    show_x9241_temps();
    return;
}

/*
 * show environmental stats from shortly before last reload
 */
static void
show_env_last (void)
{
    int ix;
    uchar val;
    char envbuf[MAX_ENVSTAT_FORMAT];

    if (!lastenv_available) {
	printf("\n%% data not available");
	return;
    }
    for (ix = 0; ix < ENVTABLE_SIZE; ++ix) {
	env_t* ev;

	ev = &env_table[ix];
	val = ev->poll.lastenv_val;
	if (SKIP_POLL(&ev->poll) || (val == 0))
	    continue;
	ev->format(envbuf, val);
	printf("\n  %18s previously measured at %s", ev->poll.name, envbuf);
    }
    for (ix = 0; ix < AMPTABLE_SIZE; ++ix) {
	int ma;
	amp_t* amp;

	amp = &amp_table[ix];
	val = amp->poll.lastenv_val;
	if (val == 0)
	    continue;
	ma = amp->convert(val, lastenv_bp_id, *amp->lastenv_ps_id);
	if (ma > 0) {
	    ci_mAmp(envbuf, ma);
	    printf("\n  %18s previously measured at %s",
		   amp->poll.name, envbuf);
	}
    }
    return;
}

/*
 * "show env"
 *
 * OBJ(int,1) = { default | all | last | table }
 */
void show_env (parseinfo *csb)
{
    if (chasint == NULL) {
	printf("%%Chassis Interface not available");
	return;
    }

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

/***************************** CI system support ****************************/

/*
 * handle CI interrupts
 * potential enhancement - enable fan & power-supply interrupts
 *   emit message & SNMP trap, then mask the interrupt.  reenable from
 *   ci_daemon after condition has gone away and debounce time expired
 */
int
chasint_interrupt (void)
{
    uchar int_stat;

    int_stat = 0;
    if (chasint) {
	int_stat = chasint->Int_Status;
	chasint->Int_Mask = 0;
    }
    errmsg(&msgsym(INT, CI), int_stat);
    return 1;			/* unexpected interrupt, crash */
}

/*
 * invoked every minute, it clears the soft power-down cycle count and
 * unregisters itself after the router has been up "long enough"
 */
static void
rsp_ci_noyoyo (void)
{
    ++postinit_minutes;

    if (postinit_minutes > CI_CYCLE_CLEAR_MINUTES) {
	if (chasint)
          chasint->Power_Status &= ~0x0f;       /* clear CycleCount */
	reg_delete_onemin(rsp_ci_noyoyo);
    }
    return;
}

/*********************** format an unconverted value ***********************/

static uint
ci_temp (char* buf, uchar value)
{
    int degreesC;

    degreesC = CIMUX_TEMP(value);
    return sprintf(buf, "%dC/%dF", degreesC, DEGREES_C2F(degreesC));
}

static int
ci_temp_celsius (uchar value)
{
    return CIMUX_TEMP(value);
}

static uint
ci_volt_p12 (char* buf, uchar value)
{
    int mv;

    mv = CIMUX_mV_PLUS12(value);
    return sprintf(buf, "%d.%02u", MILLI_TO_UNITS(mv), HUNDREDTHS(mv));
}

static int
ci_volt_p12_mv (uchar value)
{
    return CIMUX_mV_PLUS12(value);
}

static uint
ci_volt_p5 (char* buf, uchar value)
{
    int mv;

    mv = CIMUX_mV_PLUS5(value);
    return sprintf(buf, "%d.%02u", MILLI_TO_UNITS(mv), HUNDREDTHS(mv));
}

static int
ci_volt_p5_mv (uchar value)
{
    return CIMUX_mV_PLUS5(value);
}

static uint
ci_volt_n12 (char* buf, uchar value)
{
    int mv;

    mv = CIMUX_mV_MINUS12(value);
    return sprintf(buf, "%d.%02u", MILLI_TO_UNITS(mv), HUNDREDTHS(-mv));
}

static int
ci_volt_n12_mv (uchar value)
{
    return CIMUX_mV_MINUS12(value);
}

static uint
ci_volt_p24 (char* buf, uchar value)
{
    int mv;

    mv = CIMUX_mV_PLUS24(value);
    return sprintf(buf, "%d.%02u", MILLI_TO_UNITS(mv), HUNDREDTHS(mv));
}

static int
ci_volt_p24_mv (uchar value)
{
    return CIMUX_mV_PLUS24(value);
}

static uint
ci_volt_2p5 (char* buf, uchar value)
{
    int mv;

    mv = CIMUX_mV_2POINT5(value);
    return sprintf(buf, "%d.%02u", MILLI_TO_UNITS(mv), HUNDREDTHS(mv));
}

static int
ci_volt_2p5_mv (uchar value)
{
    return CIMUX_mV_2POINT5(value);
}

static int
ci_mAmp_p5 (uchar value, uchar bp_id, uchar ps_id)
{
    if ((bp_id == BACKPLANE_7513) &&
	((ps_id == BP7513PS1200WAC) || (ps_id == BP7513PS1200WDC))) {
	return CIMUX_mAMP_P5_7513(value);
    }
    return -1;
}

static int
ci_mAmp_p12 (uchar value, uchar bp_id, uchar ps_id)
{
    if ((bp_id == BACKPLANE_7513) &&
	((ps_id == BP7513PS1200WAC) || (ps_id == BP7513PS1200WDC))) {
	return CIMUX_mAMP_P12_7513(value);
    }
    return -1;
}

static int
ci_mAmp_n12 (uchar value, uchar bp_id, uchar ps_id)
{
    if ((bp_id == BACKPLANE_7513) &&
	((ps_id == BP7513PS1200WAC) || (ps_id == BP7513PS1200WDC))) {
	return CIMUX_mAMP_N12_7513(value);
    }
    return -1;
}

/*
 * read an environmental measurement from the ds1201
 */
static inline uchar
lastenv_read (uchar addr)
{
    if (addr == KEY_INVALID)
	return 0;
    return ds1201_read_byte(addr);
}

/*
 * write to the unprotected region of the ds1201
 */
static inline void
lastenv_write (uchar addr, uchar value)
{
    if (addr == KEY_INVALID)
	return;
    ds1201_write_byte(addr, value);
    return;
}

/*
 * retrieve environmental measurements from before last reboot.
 * returns false if info is not available.
 */
static boolean
get_lastenv (void)
{
    int ix;
    poll_t* poll;
    boolean retval;

    retval = FALSE;
    lastenv_bp_id = BACKPLANE_UNKNOWN;
    lastenv_ps1_id = (uchar) -1;
    lastenv_ps2_id = (uchar) -1;
    if (lastenv_read(KEY_MAGIC) == KEY_MAGIC_VALUE) {
	retval = TRUE;
	lastenv_bp_id = lastenv_read(KEY_BACKPLANE_ID);
	lastenv_ps1_id = lastenv_read(KEY_PS1_ID);
	lastenv_ps2_id = lastenv_read(KEY_PS2_ID);

	for (ix = 0; ix < ENVTABLE_SIZE; ++ix) {
	    poll = &env_table[ix].poll;
	    poll->lastenv_val = lastenv_read(poll->lastenv_key);
	}

	if (measure_current()) {
	    for (ix = 0; ix < AMPTABLE_SIZE; ++ix) {
		poll = &amp_table[ix].poll;
		poll->lastenv_val = lastenv_read(poll->lastenv_key);
	    }
	}
    }

    /*
     * poison the data
     */
    lastenv_write(KEY_MAGIC, (uchar) ~KEY_MAGIC_VALUE);

    return retval;
}

/*
 * send a temperature or voltage trap
 */
static void
env_trap (env_t* env)
{
    if ((env >= temp_table) && (env < &temp_table[TEMP_TABLE_SIZE]))
	reg_invoke_envmib_temperature_trap(env - temp_table);
    if ((env >= volt_table) && (env < &volt_table[VOLT_TABLE_SIZE]))
	reg_invoke_envmib_voltage_trap(env - volt_table);
    return;
}

#define ENV_WARN_INTERVAL	ONEHOUR
#define ENV_CRIT_INTERVAL	(5*ONEMIN)

/*
 * emit warning if necessary
 */
static void
env_warn (env_t* env)
{
    uchar val;
    char envbuf[MAX_ENVSTAT_FORMAT];

    if (TIMER_RUNNING_AND_AWAKE(env->warn_suppress))
	TIMER_STOP(env->warn_suppress);
    if (TIMER_RUNNING_AND_AWAKE(env->crit_suppress))
	TIMER_STOP(env->crit_suppress);

    val = env->poll.current_val;
    if (val >= env->crit_high)
	env->state = ENVSTATE_CRIT_HIGH;
    else if (val >= env->warn_high)
	env->state = ENVSTATE_WARN_HIGH;
    else if (val <= env->crit_low)
	env->state = ENVSTATE_CRIT_LOW;
    else if (val <= env->warn_low)
	env->state = ENVSTATE_WARN_LOW;
    else
	env->state = ENVSTATE_NORMAL;

    switch (env->state) {
      case ENVSTATE_NORMAL:
	break;

      case ENVSTATE_WARN_LOW:
      case ENVSTATE_WARN_HIGH:
	if (!TIMER_RUNNING(env->warn_suppress)) {
	    env->format(envbuf, val);
	    errmsg(&msgsym(ENVWARN, CI), env->poll.name, envbuf);
	    env_trap(env);
	    TIMER_START(env->warn_suppress, ENV_WARN_INTERVAL);
	}
	break;

      case ENVSTATE_CRIT_LOW:
      case ENVSTATE_CRIT_HIGH:
	if (!TIMER_RUNNING(env->crit_suppress)) {
	    env->format(envbuf, val);
	    errmsg(&msgsym(ENVCRIT, CI), env->poll.name, envbuf);
	    env_trap(env);
	    TIMER_START(env->crit_suppress, ENV_CRIT_INTERVAL);
	}
	break;
    }
    return;
}

/*
 * log a warning if a power supply is close to rated output
 */
static void
amp_warn (amp_t* amp, uchar ps_id)
{
    if (TIMER_RUNNING_AND_AWAKE(amp->maxout_suppress))
	TIMER_STOP(amp->maxout_suppress);

    amp->mAmps = amp->convert(amp->poll.current_val, backplane_id, ps_id);

    amp->fully_loaded = ps_fully_loaded(amp, ps_id);
    if (amp->fully_loaded && !TIMER_RUNNING(amp->maxout_suppress)) {
	errmsg(&msgsym(MAXOUTPUT, CI), amp->unit, amp->voltage);
	TIMER_START(amp->maxout_suppress, ENV_WARN_INTERVAL);
    }
    return;
}

/*
 * warn about unbalanced output, or non-redundant operation
 */
static void
dual_ps_warnings (uchar ps1_id, uchar ps2_id)
{
    int ix, shift;
    amp_t *ps1, *ps2;

    dual_ps_status = 0;

    /*
     * disable these warnings until the current measurements have
     * been validated
     */
    return;

    for (ix = 0; ix < AMPTABLE_SIZE; ++ix) {
	if (TIMER_RUNNING_AND_AWAKE(amp_table[ix].sysload_suppress))
	    TIMER_STOP(amp_table[ix].sysload_suppress);
	if (TIMER_RUNNING_AND_AWAKE(amp_table[ix].unbalance_suppress))
	    TIMER_STOP(amp_table[ix].unbalance_suppress);
    }

    /* skip these warnings if one PS has failed or is missing */
    if (chasint->Int_Status & (INT_STATUS_PS1_BIT | INT_STATUS_PS2_BIT))
	return;

    for (ps1 = ps1_table, ps2 = ps2_table, shift = 0;
	 ps1 < ps2_table;
	 ++ps1, ++ps2, ++shift) {
	int total_load, load_diff;

	if ((ps1->mAmps < 0) || (ps2->mAmps < 0))
	    continue;
	total_load = ps1->mAmps + ps2->mAmps;
	if (total_load > ps_capacity(backplane_id, ps1_id, ps1->voltage)) {
	    dual_ps_status |= (DUALPS_PS1_SYSLOAD << shift);
	    if (!TIMER_RUNNING(ps1->sysload_suppress)) {
		errmsg(&msgsym(SYSLOAD, CI), ps1->voltage, ps1->unit);
		TIMER_START(ps1->sysload_suppress, ENV_WARN_INTERVAL);
	    }
	}
	if (total_load > ps_capacity(backplane_id, ps2_id, ps2->voltage)) {
	    dual_ps_status |= (DUALPS_PS2_SYSLOAD << shift);
	    if (!TIMER_RUNNING(ps2->sysload_suppress)) {
		errmsg(&msgsym(SYSLOAD, CI), ps2->voltage, ps2->unit);
		TIMER_START(ps2->sysload_suppress, ENV_WARN_INTERVAL);
	    }
	}
	if (total_load == 0)
	    return;			/* geez... */
	load_diff = ps1->mAmps - ps2->mAmps;
	if (load_diff < 0)
	    load_diff = (-load_diff);
	if ((load_diff * 200 / total_load) > LOAD_DIFF_PERCENT) {
	    dual_ps_status |= (DUALPS_UNBALANCE << shift);
	    if (!TIMER_RUNNING(ps1->unbalance_suppress)) {
		errmsg(&msgsym(UNBALANCE, CI), ps1->voltage);
		TIMER_START(ps1->unbalance_suppress, ENV_WARN_INTERVAL);
	    }
	}
    }
    return;
}

/*
 * check a power supply for something worth reporting
 */
static void
check_ps (uchar int_stat, uchar ps_id, uchar unit, sys_timestamp* timer)
{
    if (INT_STATUS_PSX_FAIL(int_stat, unit) && !ps_removed(ps_id) &&
	!TIMER_RUNNING(*timer)) {
	errmsg(&msgsym(PSFAIL, CI), unit);
	reg_invoke_envmib_redundant_supply_trap(unit);
	TIMER_START(*timer, ENV_CRIT_INTERVAL);
    } else {
	if (TIMER_RUNNING_AND_AWAKE(*timer))
	    TIMER_STOP(*timer);
    }
    return;
}

static sys_timestamp blower_suppress;

static tinybool bfail_7505_will_reload;
static uchar    bfail_7505_warn_count;

/*
 * alert the user during countdown to fan-failure shutdown
 */
static void
bfail_7505_msg (emergency_msg_time_type msg_type)
{
    switch (msg_type) {
      case EMERGENCY_LINE_BANNER:
      case EMERGENCY_SHOW_VERSION:
	printf("\nEMERGENCY: shutdown due to blower failure in %d seconds\n",
	       TIME_LEFT_SLEEPING(blower_suppress) / ONESEC);
	break;

      default:
	break;
    }
    return;
}

/*
 * called every second when counting down to fan-failure shutdown
 */
static void
bfail_7505 (void)
{
    int seconds_left;

    if (!INT_STATUS_BFAIL(get_int_status(chasint))) {
	if (bfail_7505_will_reload)
	    reload_system(RELOAD_QUICK);
	reg_delete_onesec(bfail_7505);
	reg_delete_emergency_message(bfail_7505_msg);
	TIMER_STOP(blower_suppress);
	return;
    }
    seconds_left = TIME_LEFT_SLEEPING(blower_suppress) / ONESEC;
    if ((bfail_7505_warn_count++ % 15) == 0)
	errmsg(&msgsym(BLOWSHUT, CI), seconds_left);
    if ((seconds_left < 5) && !bfail_7505_will_reload) {
	reg_invoke_will_reload();
	bfail_7505_will_reload = TRUE;
    }
    if (seconds_left <= 0) {
	chasint->Power_Ctl |= (POWER_CTL_PS1_SHUT | POWER_CTL_PS2_SHUT);
	for (;;)
	    /* ever */;
    }
    return;
}

/*
 * check the fan for something worth reporting
 */
static void
check_fan (uchar int_stat, sys_timestamp* timer)
{
    if (INT_STATUS_BFAIL(int_stat) && !TIMER_RUNNING(*timer)) {
	uchar fan_number;

	fan_number = INT_STATUS_BLOWERNUM(int_stat);
	if (fan_number > fan_count)
	    return;
	errmsg(&msgsym(BLOWER, CI), fan_name(fan_number));
	reg_invoke_envmib_fan_trap(fan_number);
	if (backplane_id == BACKPLANE_7505) {
	    reg_add_onesec(bfail_7505, "bfail_7505");
	    reg_add_emergency_message(bfail_7505_msg, "bfail_7505_msg");
	    TIMER_START(*timer, FAN_SHUTDOWN_7505);
	    bfail_7505_will_reload = FALSE;
	    bfail_7505_warn_count = 0;
	} else {
	    TIMER_START(*timer, ENV_CRIT_INTERVAL);
	}
    } else {
	if (TIMER_RUNNING_AND_AWAKE(*timer))
	    TIMER_STOP(*timer);
    }
    return;
}

/*
 * initialize saved environmental measurements
 */
static void
setup_lastenv (uchar ps1_id, uchar ps2_id)
{
    lastenv_write(KEY_BACKPLANE_ID, backplane_id);
    lastenv_write(KEY_PS1_ID, ps1_id);
    lastenv_write(KEY_PS2_ID, ps2_id);
    return;
}

/*
 * simple check that the CI is still accessible
 */
static void
ci_validate (void)
{
    uchar slot_and_comp;
    slot_and_comp = chasint->DBus_Slot;
    if ((slot_and_comp >> 4) != (~slot_and_comp & 0x0f)) {
	chasint = NULL;
	for (;;) {
	    errmsg(&msgsym(NOTFOUND, CI));
	    process_sleep_for(ENV_WARN_INTERVAL);
	}
    }
    return;
}

/*
 * poll one value this often (get them about every minute)
 */
static int poll_interval;

/*
 * periodic polling of CI stats
 */
static void
ci_daemon (void)
{
    chasint_dbus_t *ci;
    uchar ps_id, ps1_id, ps2_id, val;
    sys_timestamp ps1fail_suppress, ps2fail_suppress;
    tinybool lastenv_complete;

    ci = chasint;
    ps_id = ci->PS_ID;
    ps1_id = PS_ID_PS1_ID(ps_id);
    ps2_id = PS_ID_PS2_ID(ps_id);
    TIMER_STOP(ps1fail_suppress);
    TIMER_STOP(ps2fail_suppress);
    TIMER_STOP(blower_suppress);
    setup_lastenv(ps1_id, ps2_id);
    lastenv_complete = FALSE;
    for (;;) {
	int ix;

	ci_validate();
	if (ps_count > 1) {
	    ps_id = ci->PS_ID;
	    val = PS_ID_PS1_ID(ps_id);
	    if (val != ps1_id) {
		errmsg(&msgsym(PSCHANGE, CI), 1,
		       ps_name(ps1_id, backplane_id),
		       ps_name(val, backplane_id));
		ps1_id = val;
		lastenv_write(KEY_PS1_ID, ps1_id);
	    }
	    val = PS_ID_PS2_ID(ps_id);
	    if (val != ps2_id) {
		errmsg(&msgsym(PSCHANGE, CI), 2,
		       ps_name(ps2_id, backplane_id),
		       ps_name(val, backplane_id));
		ps2_id = val;
		lastenv_write(KEY_PS2_ID, ps2_id);
	    }
	}

	val = get_int_status(ci);
	check_ps(val, ps1_id, 1, &ps1fail_suppress);
	check_ps(val, ps2_id, 2, &ps2fail_suppress);
	check_fan(val, &blower_suppress);

	for (ix = 0; ix < ENVTABLE_SIZE; ++ix) {
	    uchar select;

	    if (SKIP_POLL(&env_table[ix].poll))
		continue;
	    select = env_table[ix].poll.a2d_select;
	    select_a2d(select, poll_interval, FALSE);
	    val = read_a2d();
	    env_table[ix].poll.current_val = val;
	    if (val > 0)
		env_warn(&env_table[ix]);
	    lastenv_write(env_table[ix].poll.lastenv_key,
			  env_table[ix].poll.current_val);
	    if (select == MUX_PLUS12V_MONITOR)
		plus12v_value = val;
	}
	if (measure_current()) {
	    for (ix = 0; ix < AMPTABLE_SIZE; ++ix) {
		amp_t* amp;

		amp = &amp_table[ix];
		select_a2d(amp->poll.a2d_select, poll_interval, FALSE);
		val = read_a2d();
		amp->poll.current_val = val;
		if (val > 0)
		    amp_warn(amp, (amp->unit == 1) ? ps1_id : ps2_id);
		lastenv_write(amp_table[ix].poll.lastenv_key,
			      amp_table[ix].poll.current_val);
	    }
	    dual_ps_warnings(ps1_id, ps2_id);
	}
	if (!lastenv_complete) {
	    lastenv_write(KEY_MAGIC, KEY_MAGIC_VALUE);
	    lastenv_complete = TRUE;
	}
	if (variable_blower()) {
	    int speed, new_speed;

	    speed = read_pot(POT_BLOWER_SPEED);
	    if (INT_MASK_RESTART_OK(ci->Int_Mask)) {
		new_speed = speed + (X9241_MAXVAL / 10);
		if (new_speed > X9241_MAXVAL)
		    new_speed = X9241_MAXVAL;
	    } else {
		new_speed = speed - (X9241_MAXVAL / 10);
		if (new_speed < 0)
		    new_speed = 0;
	    }
	    if (new_speed != speed)
		write_pot(POT_BLOWER_SPEED, new_speed);
	}
    }
}

/*
 * complain that there isn't a CI card installed
 */
static void
ci_notfound (emergency_msg_time_type msg_type)
{
    switch (msg_type) {
      case EMERGENCY_SYS_STARTUP:
	errmsg(&msgsym(NOTFOUND, CI));
	break;

      case EMERGENCY_SHOW_VERSION:
	printf("\nWARNING: Chassis Interface not present\n");
	break;

      default:
	break;
    }
    return;
}

/******************************* CI Init ******************************/

#define CI_VERSION 	0x07	/* some CIs were misprogrammed with this */

/* 
 * discover the CI
 */
static chasint_dbus_t*
ci_analyze (boolean complain)
{
    chasint_dbus_t* ci;
    uchar slot_and_comp, ctrlr_type;

    ci = (chasint_dbus_t*) pslot_to_dbus(CHASSIS_INTERFACE_SLOT);
    if (!touch_device(ci))
	return NULL;

    slot_and_comp = ci->DBus_Slot;
    if ((slot_and_comp >> 4) != (~slot_and_comp & 0x0f)) {
	if (complain)
	    errmsg(&msgsym(SLOTCOMP, CI), "CI", slot_and_comp);
	return NULL;
    }

    ctrlr_type = ci->eeprom.controller_type;
    if (((ci->eeprom.eeprom_version != IDPROM_VERSION) &&
	 (ci->eeprom.eeprom_version != CI_VERSION)) ||
	((ctrlr_type != FCI_CI_CONTROLLER) &&
	 (ctrlr_type != FCI_CI7000_CONTROLLER))) {
	if (complain)
	    errmsg(&msgsym(CTRLRTYPE, CI), ctrlr_type);
	return NULL;
    }

    return ci;
}

/*
 * for known dual-RSP backplane types, find the RSP/1 slot.
 * there's only one, since if_init() crashdumps otherwise.
 */
static uchar
rsp1_dualbus_slot (uchar bp_id)
{
    switch (bp_id) {
      case BACKPLANE_7507:
	if (slot_present(RSPSLOT_7507_PROC1))
	    return RSPSLOT_7507_PROC1;
	if (slot_present(RSPSLOT_7507_PROC2))
	    return RSPSLOT_7507_PROC2;
	break;

      case BACKPLANE_7513:
      case BACKPLANE_TOKTEST:
	if (slot_present(RSPSLOT_7513_PROC1))
	    return RSPSLOT_7513_PROC1;
	if (slot_present(RSPSLOT_7513_PROC2))
	    return RSPSLOT_7513_PROC2;
	break;
    }
    return MAX_SLOTS;
}

/*
 * figure out what slot we're in
 */
uchar
compute_rsp_slot (void)
{
    uchar slot, bp_id;

    chasint = ci_analyze(FALSE /* don't complain */);
    if (chasint)
	bp_id = BPLANE_ID_BACKPLANE_ID(chasint->BPlane_ID);
    else
	bp_id = BACKPLANE_UNKNOWN;
    switch (bp_id) {
      case BACKPLANE_7506:
	return RSPSLOT_7506;

      case BACKPLANE_7505:
	return RSPSLOT_7505;

      case BACKPLANE_7507:
      case BACKPLANE_7513:
      case BACKPLANE_TOKTEST:
	slot = rsp1_dualbus_slot(bp_id);
	if (slot != MAX_SLOTS)
	    return slot;
	break;

      case BACKPLANE_7000:
	if (chasint) {
	    if (slot_present(RSPSLOT_7000))
		return RSPSLOT_7000;
	    if (slot_present(RSPSLOT_7010))
		return RSPSLOT_7010;
	}
	break;
    }
    /*
     * if backplane type is unknown, assume the first dbus_present bit
     * without corresponding dbus registers is the RSP
     */
    for (slot = 0; slot < MAX_SLOTS; ++slot) {
	if (slot_present(slot) &&
	    !touch_device(pslot_to_dbus(slot)))
	    break;
    }
    return slot;
}

/*
 * the 7505 backplane has a bug - the slot number pins
 * in the RSP slot indicate slot 6 instead of slot 4.
 */
uchar
adjust_rsp_slot (uchar slot)
{
    chasint_dbus_t* ci;

    ci = ci_analyze(FALSE	/* don't complain */);
    if (ci &&
	(BACKPLANE_7505 == BPLANE_ID_BACKPLANE_ID(ci->BPlane_ID))) {
	slot = RSPSLOT_7505;
    }
    return slot;
}

/*
 * register ourself with the chassis MIB
 */
static void
self2chassismib (dgbus_eeprom_t *idprom)
{
    int slot;
    ulong serial;
    char* card_descr;
    card_type_t card_type;
    char version_buf[16];
    char* hw_version;
    char* sw_version;
    char* cp;

    card_type_and_descr(idprom->controller_type, 0, &processor_type, 
			&card_descr);

    serial = (((((idprom->serial_number_31_24  << 8) + 
		  idprom->serial_number_23_16) << 8) +
	          idprom->serial_number_15_8)  << 8) +
		  idprom->serial_number_7_0;

    cp = version_buf;
    cp += sprintf(hw_version = cp, "%u.%u",
		  idprom->hardware_version_high, idprom->hardware_version_low);
    cp += sprintf(sw_version = ++cp, "%u.%u",
		  SYS_MAJORVERSION, SYS_MINORVERSION);

    if (get_rsp_self_slot() == RSP_NO_SLOT)
	slot = NO_SLOTNUMBER;
    else
	slot = get_rsp_self_slot();
    reg_invoke_chassismib_add_card_entry(processor_type,
					NO_HOTSWAP /* can't hotswap ourself */,
					card_descr, serial, hw_version,
					sw_version, slot);

    /*
     * now, register the CI itself
     */

    idprom = &chasint->eeprom;

    serial = (((((idprom->serial_number_31_24  << 8) + 
		  idprom->serial_number_23_16) << 8) +
	          idprom->serial_number_15_8)  << 8) +
		  idprom->serial_number_7_0;

    sprintf(hw_version = version_buf, "%u.%u",
	    idprom->hardware_version_high, idprom->hardware_version_low);

    if (processor_type == D_cardType_rsp7000) {
	card_type = D_cardType_chassisInterface7000;
	if (slot != NO_SLOTNUMBER)
	    slot = RSP7000_TO_CI7000(slot);
    } else {
	card_type = D_cardType_chassisInterface;
	slot = NO_SLOTNUMBER;
    }
    reg_invoke_chassismib_add_card_entry(card_type, NO_HOTSWAP,
					 "Environmental Monitor", serial,
					 hw_version, "", slot);
    return;
}

/*
 * initialize non-default values for all 5-slot chassis
 */
static void
chassis_init_5slot (void)
{
    fan_count = FANS_7505;
    ps_count = PS_7505;
    slothdr = "01234";
    chassis_slots = SLOTS_7505;
    return;
}

/*
 * initialize non-default values for all 7-slot chassis
 */
static void
chassis_init_7slot (void)
{
    fan_count = FANS_7507;
    ps_count = PS_7507;
    slothdr = "0123456";
    chassis_slots = SLOTS_7506;
    return;
}

/*
 * initialize non-default values for all 13-slot chassis
 */
static void
chassis_init_13slot (void)
{
    fan_count = FANS_7513;
    ps_count = PS_7513;
    chassis_slots = SLOTS_7513;
    return;
}

/*
 * initialize chassis-specific info
 */
static void
ci_chassis_init (uchar real_bp_id, uchar virt_bp_id)
{
    slothdr = "          1\n"
	      "            0123456789012";
    fan_count = 0;
    ps_count = 0;
    chassis_slots = -1;		/* unknown value */
    system_id.oid_ptr = (unsigned long *) IDciscoGatewayServer;
    system_id.length = LNciscoGatewayServer;
    switch (real_bp_id) {
      case BACKPLANE_7505:
	chassis_init_5slot();
        system_id.oid_ptr = (unsigned long *) IDcisco7505;
        system_id.length = LNcisco7505;
	chassis_type = D_chassisType_c7505;
	break;

      case BACKPLANE_7506:
 	chassis_init_7slot();
	system_id.oid_ptr = (unsigned long *) IDcisco7506;
	system_id.length = LNcisco7506;
	chassis_type = D_chassisType_c7506;
	break;

      case BACKPLANE_7507:
	chassis_init_7slot();
	system_id.oid_ptr = (unsigned long *) IDcisco7507;
	system_id.length = LNcisco7507;
	chassis_type = D_chassisType_c7507;
	break;

      case BACKPLANE_7513:
	chassis_init_13slot();
	system_id.oid_ptr = (unsigned long *) IDcisco7513;
	system_id.length = LNcisco7513;
	chassis_type = D_chassisType_c7513;
	break;

      case BACKPLANE_7000:
	switch (virt_bp_id) {
	  case BACKPLANE_7505:
	    chassis_init_5slot();
	    system_id.oid_ptr = (unsigned long *) IDcisco7010;
	    system_id.length = LNcisco7010;
	    chassis_type = D_chassisType_c7010;
	    break;

	  case BACKPLANE_7506:
	    chassis_init_7slot();
	    system_id.oid_ptr = (unsigned long *) IDcisco7000;
	    system_id.length = LNcisco7000;
	    chassis_type = D_chassisType_c7000;
	    break;
	}
	break;

      case BACKPLANE_TOKTEST:
	chassis_slots = SLOTS_TOKTEST;
	chassis_type = D_chassisType_unknown;
	break;
    }
    return;
}

/*
 * return index of temperature threshold table entry appropriate for
 * the given RSP and backplane type
 */
static uchar
temp_thresh_ix (uchar ctrlr, uchar real_bp_id, uchar virt_bp_id)
{
    switch (ctrlr) {
      case FCI_RSP1_CONTROLLER:
      case FCI_RSP2_CONTROLLER:
				switch (virt_bp_id) {
				  case BACKPLANE_7506:
				  case BACKPLANE_7507:
				  case BACKPLANE_7513:
				  case BACKPLANE_7505: return THRESH_RSP1_7505;
				}
				break;
      case FCI_RSP7000_CONTROLLER:
				switch (real_bp_id) {
				  case BACKPLANE_7000: return THRESH_RSP7000;
				}
				break;
    }
    return THRESH_DEFAULT;
}

/*
 * discover and initialize the CI
 */
static void
chasint_subsys_init (subsystype* subsys)
{
    int ix, samples;
    chasint_dbus_t *ci;
    dgbus_eeprom_t *idprom;
    uchar initial_power_ctl, tt_ix;

    chasint = NULL;
    backplane_id = BACKPLANE_UNKNOWN;

    if (system_loading)
	return;

    lastenv_available = get_lastenv();

    ci = ci_analyze(TRUE /* complain */);
    if (ci == NULL) {
	reg_add_emergency_message(ci_notfound, "ci_notfound");
	return;
    }

    /*
     * set the override bit while we may change things
     */
    initial_power_ctl = ci->Power_Ctl;
    ci->Power_Ctl = (initial_power_ctl | POWER_CTL_OVERRIDE);

    /*
     * select temperature from RSP "zero";
     */
    ci->Power_Ctl &= ~POWER_CTL_TEMPSEL;

    /*
     * mask all interrupts
     */
    ci->Int_Mask = 0;

    real_backplane_id = backplane_id = BPLANE_ID_BACKPLANE_ID(ci->BPlane_ID);
    if ((backplane_id != chassis_model) && (chassis_model != NO_CHASSIS_MODEL))
	errmsg(&msgsym(BPLANE, CI), backplane_id, chassis_model);

    temp_source = "RSP";
    if (backplane_id == BACKPLANE_7000) {
	if (get_rsp_self_slot() == RSPSLOT_7000)
	    backplane_id = BACKPLANE_7506;
	else if (get_rsp_self_slot() == RSPSLOT_7010)
	    backplane_id = BACKPLANE_7505;
	temp_source = "CI";
    }

    /*
     * check and correct soft failure trip point
     * Set up chasint for normal operation, reg_add_onemin
     * will access chasint global.
     */
    chasint = ci;
    if (soft_shutdown_works()) {
	uchar power_status, dbus_status;

	/*
	 * check if we're restarting from a soft power failure
	 */
	dbus_status = ci->DBus_Status;
	if (DBUS_STATUS_CI_ERR(dbus_status)) {
	    errmsg(&msgsym(COND, CI), DBUS_STATUS_CI_COND(dbus_status));
	    postinit_minutes = 0;
	    reg_add_onemin(rsp_ci_noyoyo, "rsp_ci_noyoyo");
	}
	power_status = ci->Power_Status;
	if (POWER_STATUS_TRIP_POINT(power_status) != CI_CYCLE_TRIP_POINT) {
	    ci->Power_Status = ((CI_CYCLE_TRIP_POINT << 4) |
				POWER_STATUS_CYCLE_COUNT(power_status));
	}
    }

    samples = (ENVTABLE_SIZE + AMPTABLE_SIZE);

    sprintf(rsp0_inlet,    "%s(%u) Inlet",    temp_source, rsp_slot(0));
    sprintf(rsp0_hotpoint, "%s(%u) Hotpoint", temp_source, rsp_slot(0));
    sprintf(rsp0_exhaust,  "%s(%u) Exhaust",  temp_source, rsp_slot(0));
    if (dual_cpu_capable()) {
	sprintf(rsp1_inlet,    "%s(%u) Inlet",    temp_source, rsp_slot(1));
	sprintf(rsp1_hotpoint, "%s(%u) Hotpoint", temp_source, rsp_slot(1));
	sprintf(rsp1_exhaust,  "%s(%u) Exhaust",  temp_source, rsp_slot(1));
    } else {
	*rsp1_inlet    = SET_SKIP_POLL;
	*rsp1_hotpoint = SET_SKIP_POLL;
	*rsp1_exhaust  = SET_SKIP_POLL;
	samples -= 3;
    }

    /*
     * set up for normal system operation
     */
    a2d_state = A2D_IDLE;

    /*
     * initialize chassis-specific data, including the snmp platform api
     */
    idprom = (dgbus_eeprom_t*) ADRSPC_IDPROM;
    self2chassismib(idprom);
    ci_chassis_init(real_backplane_id, backplane_id);

    tt_ix = temp_thresh_ix(idprom->controller_type,
			   real_backplane_id, backplane_id);
    rsp1_inlet_env->warn_high =
	rsp0_inlet_env->warn_high = temp_thresh[tt_ix].inlet_warn;
    rsp1_inlet_env->crit_high =
	rsp0_inlet_env->crit_high = temp_thresh[tt_ix].inlet_crit;
    rsp1_hotpoint_env->warn_high =
	rsp0_hotpoint_env->warn_high = temp_thresh[tt_ix].hotpoint_warn;
    rsp1_hotpoint_env->crit_high =
	rsp0_hotpoint_env->crit_high = temp_thresh[tt_ix].hotpoint_crit;
    rsp1_exhaust_env->warn_high =
	rsp0_exhaust_env->warn_high = temp_thresh[tt_ix].exhaust_warn;
    rsp1_exhaust_env->crit_high =
	rsp0_exhaust_env->crit_high = temp_thresh[tt_ix].exhaust_crit;

    /*
     * clear any in-progress X9241 transaction
     */
    Power_Ctl = chasint->Power_Ctl & ~(POWER_CTL_SDA | POWER_CTL_SCL);
    x9241_stop();				/* clear inc/dec */
    for (ix = 0; ix < 32; ++ix) {		/* clear 2- or 3-byte cmds */
	chasint->Power_Ctl = Power_Ctl | POWER_CTL_SCL;
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_HIGH);
	chasint->Power_Ctl = Power_Ctl;		/* SCL == 0 */
	chasint->Power_Ctl;
	usecdelay(X9241_MIN_CLOCK_LOW);
    }
    x9241_stop();

    /*
     * reset both EEPOTs to power-on state
     */
    eepot_cmd(0, X9241_CMD(XFR_REG2ALLWCRS, NO_WCR, NO_REG), "");
    eepot_cmd(1, X9241_CMD(XFR_REG2ALLWCRS, NO_WCR, NO_REG), "");

    /*
     * check/correct restart/warn/shutdown levels
     */
    x9241_check_or_set(POT_TEMP_RESTART_OK, temp_thresh[tt_ix].restart_ok);
    x9241_check_or_set(POT_TEMP_INTERRUPT, temp_thresh[tt_ix].interrupt);
    x9241_check_or_set(POT_TEMP_BOARD_SHUT, temp_thresh[tt_ix].board_shut);
    x9241_check_or_set(POT_TEMP_PS_SHUT, temp_thresh[tt_ix].ps_shut);

    /*
     * select hot spot temperature source
     */
    write_pot(POT_TEMPSEL0, TEMPSEL_BIT0(temp_thresh[tt_ix].tempsel));
    write_pot(POT_TEMPSEL1, TEMPSEL_BIT1(temp_thresh[tt_ix].tempsel));

    /*
     * start with low speed
     */
    x9241_check_or_set(POT_BLOWER_SPEED, 0);

    /*
     * X9241 address 1, pot 3 isn't used, so disable it
     */
    for (;;) {
	uchar data;

	data = X9241_DISABLE_WIPER;
	if (!eepot_cmd(1, X9241_CMD(WRITE_WCR, 3, NO_REG), &data))
	    break;
	if (!eepot_cmd(1, X9241_CMD(READ_REG, 3, 0), &data))
	    break;
	if (data != 0)
	    eepot_cmd(1, X9241_CMD(XFR_WCR2REG, 3, 0), &data);
	break;
    }

    usecdelay(X9241_TSTPWV);
    if ((initial_power_ctl & POWER_CTL_OVERRIDE) == 0)
	ci->Power_Ctl &= ~POWER_CTL_OVERRIDE;

    plus12v_value = 0;

    poll_interval = (ONEMIN / samples);
    process_create(ci_daemon, "Chassis Daemon", SMALL_STACK, PRIO_LOW);

    dual_ps_status = 0;

    parser_add_commands(PARSE_ADD_TEST_CMD, &pname(ci_test_command), "ci");
    envmon_parser_init();
    /*
     * ok to load envmib since ci card is present
     */
    envmib_load = TRUE;

    return;
}

SUBSYS_HEADER(chassis,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      chasint_subsys_init,	/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      /* follows "rsp" because "rsp" initializes the ds1201 */
	      "seq: dbus rsp",		/* sequence list */
	      "req: dbus"		/* prerequisite list */
	      );

/*************************** snmp platform api ***************************/

OID *
snmp_platform_get_oid (snmp_platform_oid_type selector)
{
    switch (selector) {
    case SNMP_PLATFORM_OID_SYS_OBJECT_ID:
        return &system_id;
    default:
        return NULL;
    }
}

uint
snmp_platform_get_value (snmp_platform_value_type selector)
{
    switch (selector) {
    case SNMP_PLATFORM_CHASSIS_TYPE:
        return chassis_type;
    case SNMP_PLATFORM_PROCESSOR_TYPE:
        return processor_type;
    case SNMP_PLATFORM_SLOTS:
        return chassis_slots;
    default:
        return -1;
    }
}

int
snmp_platform_get_card_value (uint cardIndex,
			      snmp_platform_card_value_type selector)
{
    card_entry *cep;
    long card_index;
 
    card_index = cardIndex;
    if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
        return(-1);
 
    switch (selector) {
    case SNMP_PLATFORM_CARD_OPER_STATUS:
        switch (cep->card_type) {
        case D_cardType_rsp1:
        case D_cardType_rsp2:
        case D_cardType_rsp7000:
	    /*
	     * If the RSP of interest is us, then we're obviously up,
	     * otherwise we wouldn't be here, otherwise it's our slave,
	     * so return a result based on his state.  Note that a healthy
	     * slave is indicated by the 'standby' state.
	     */
	    if(cep->card_slot_number == get_rsp_self_slot())
		return(D_cardOperStatus_up);
	    else if(rsp_slave_state() == SLAVE_RUNNING)
		return(D_cardOperStatus_standby);
	    else
		return(D_cardOperStatus_down);
        case D_cardType_chassisInterface:
        case D_cardType_chassisInterface7000:
            return(D_cardOperStatus_not_specified);
        case D_cardType_pa_1fe:
        case D_cardType_pa_1fe_tx_isl:
        case D_cardType_pa_1fe_fx_isl:
        case D_cardType_pa_1fe_tx_nisl:
        case D_cardType_pa_4e:
        case D_cardType_pa_5e:
        case D_cardType_pa_8e:
        case D_cardType_pa_4t:
        case D_cardType_pa_4r:
            /* Add in support for pas when pa_info is made available */
            return(D_cardOperStatus_not_specified);
        default:
            return((slot_enabled(cep->card_slot_number)?
                        D_cardOperStatus_up:D_cardOperStatus_down));
        }
    default:
        return -1;
    }
}

int
snmp_platform_get_if_value (hwidbtype *hwidb,
			    snmp_platform_if_value_type selector)
{
    card_entry *cep;
    long card_index;
    ONE_ITEM_LIST(tmp, ETHER_HW_MEDIA_TYPE, sizeof(ulong), 0);
 
    if (hwidb == NULL)
        return(-1);

    switch (selector) {
    case SNMP_PLATFORM_IF_CARD_INDEX:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
            return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
            return(-1);
        switch (cep->card_type) {
        case D_cardType_vip:
        case D_cardType_vip2:
            return(reg_invoke_chassismib_find_sub_card_index(card_index,
                                (hwidb->slotunit >> PA_VIRTUAL_SLOT_BITS)));
        default:
            return((uint) card_index);
        }
    case SNMP_PLATFORM_IF_SLOT_NUMBER:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
            return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
            return(-1);
        switch (cep->card_type) {
        case D_cardType_vip:
        case D_cardType_vip2:
            /*
             * cardIfSlotNumber use should be phased out. PA interfaces set
             * to -1 to allow derivation of old cardIfTable.
             */
            return(-1);
        default:
            return(hwidb->slot);
        }
    case SNMP_PLATFORM_IF_PORT_NUMBER:
        if ((card_index = 
	     reg_invoke_chassismib_find_slot_card_index(hwidb->slot)) == -1)
            return(-1);
        if ((cep = reg_invoke_chassismib_get_cep(EXACT,&card_index)) == NULL)
            return(-1);
        switch (cep->card_type) {
        case D_cardType_vip:
        case D_cardType_vip2:
            return((hwidb->slotunit) & (PA_VIRTUAL_SLOT_BOUNDARY-1));
        default:
            return(hwidb->slotunit);
        }
    case SNMP_PLATFORM_IF_CONNECTOR_TYPE_ENABLED:
        switch (hwidb->type) {
        case IDBTYPE_FEIP:
	    /*
	     * Get the media type
	     */
	    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);

	    switch (tmp_item.u.l_value) {
	      case ETHER_MEDIA_TYPE_FEIP_RJ45:
		return(D_cardIfConnectorTypeEnabled_rj_45);
	      case ETHER_MEDIA_TYPE_FEIP_MII:
		return(D_cardIfConnectorTypeEnabled_db_40);
	      default:
		return(D_cardIfConnectorTypeEnabled_none);
	    }
        default:
            return(D_cardIfConnectorTypeEnabled_not_specified);
        }
    default:
        return(-1);
    }
}



/**************************** dbus code **********************************/

/*
 * if CI is present read its dbus present bits, otherwise use touch_device
 * these bits indicate a board is inserted, not that it is powered up
 * return FALSE for RSP slot
 */
static inline boolean
slot_present_inline (uint slot)
{
    if (slot_present_mask.data[slot])
	return FALSE;
    if ((slot == get_rsp_self_slot()))
	return FALSE;
    if (chasint) {
	if (DBUS_PRESENT(chasint->DBUS, slot)) {
	    if (!hotstall_flag)
		return TRUE;
	} else {
	    return FALSE;
	}
    }
    return touch_device(pslot_to_dbus(slot));
}

boolean
slot_present (uint slot)
{
    return slot_present_inline(slot);
}

boolean
dbus_present (dgtype* dbus)
{
    return slot_present_inline(dbus_to_pslot(dbus));
}

/*
 * return slot number for route processor
 */
int dbus_system_rp_slot (void) 
{
    return get_rsp_self_slot();
}

/*
 * does the specified contain an RSP?
 */
boolean
dbus_cpu_slot (uint slot)
{
    tinybool present;

    if (chasint)
	present = DBUS_PRESENT(chasint->DBUS, slot);
    else
	present = touch_device(pslot_to_dbus(slot));
    if (!present)
	return FALSE;
    return ((slot == get_rsp_self_slot()) ||
	    (slot == get_rsp_peer_slot()));
}

/************************** ENVMON MIB support *******************************/

boolean
ci_present (void)
{
    return (chasint != NULL);
}

/*
 * routines to fill-in the voltage table
 */

boolean
ci_volt_valid (uint index)
{
    if (chasint == NULL)
	return FALSE;
    if (index < 1)
	return FALSE;
    if (index > VOLT_TABLE_SIZE)
	return FALSE;
    --index;
    if (volt_table[index].poll.current_val == 0)
	return FALSE;
    return TRUE;
}

char*
ci_volt_descr (uint index)
{
    --index;
    return volt_table[index].poll.name;
}

uint
ci_volt_value (uint index)
{
    env_t* env;

    --index;
    env = &volt_table[index];
    return env->convert(env->poll.current_val);
}

uint
ci_volt_lowthresh (uint index)
{
    env_t* env;

    --index;
    env = &volt_table[index];
    return env->convert(env->crit_low);
}

uint
ci_volt_highthresh (uint index)
{
    env_t* env;

    --index;
    env = &volt_table[index];
    return env->convert(env->crit_high);
}

uint
ci_volt_lastvalue (uint index)
{
    env_t* env;

    --index;
    env = &volt_table[index];
    if (env->poll.lastenv_key == KEY_INVALID)
	return 0;
    return env->convert(env->poll.lastenv_val);
}

uint
ci_volt_state (uint index)
{
    --index;
    switch (volt_table[index].state) {
      case ENVSTATE_NORMAL:	return D_ciscoEnvMonVoltageState_normal;
      case ENVSTATE_WARN_LOW:
      case ENVSTATE_WARN_HIGH:	return D_ciscoEnvMonVoltageState_warning;
      case ENVSTATE_CRIT_LOW:
      case ENVSTATE_CRIT_HIGH:	return D_ciscoEnvMonVoltageState_critical;
    }
    return D_ciscoEnvMonVoltageState_notPresent;
}

/*
 * routines to fill-in the temperature table
 */

boolean
ci_temp_valid (uint index)
{
    if (chasint == NULL)
	return FALSE;
    if (index < 1)
	return FALSE;
    if (index > TEMP_TABLE_SIZE)
	return FALSE;
    --index;
    if (temp_table[index].poll.current_val == 0)
	return FALSE;
    return TRUE;
}

char*
ci_temp_descr (uint index)
{
    --index;
    return temp_table[index].poll.name;
}

uint
ci_temp_value (uint index)
{
    env_t* env;

    --index;
    env = &temp_table[index];
    return env->convert(env->poll.current_val);
}

uint
ci_temp_thresh (uint index)
{
    env_t* env;

    --index;
    env = &temp_table[index];
    return env->convert(env->crit_high);
}

uint
ci_temp_lastvalue (uint index)
{
    env_t* env;

    --index;
    env = &temp_table[index];
    return env->convert(env->poll.lastenv_val);
}

uint
ci_temp_state (uint index)
{
    --index;
    switch (temp_table[index].state) {
      case ENVSTATE_NORMAL:	return D_ciscoEnvMonTemperatureState_normal;
      case ENVSTATE_WARN_LOW:
      case ENVSTATE_WARN_HIGH:	return D_ciscoEnvMonTemperatureState_warning;
      case ENVSTATE_CRIT_LOW:
      case ENVSTATE_CRIT_HIGH:	return D_ciscoEnvMonTemperatureState_critical;
    }
    return D_ciscoEnvMonTemperatureState_notPresent;
}

/*
 * routines to fill-in the fan table
 */

boolean
ci_fan_valid (uint index)
{
    if (chasint == NULL)
	return FALSE;
    if (index < 1)
	return FALSE;
    if (index > fan_count)
	return FALSE;
    if (backplane_id == BACKPLANE_7513)
	return believe_7513fan(index);
    return TRUE;
}

char*
ci_fan_descr (uint index)
{
    return fan_name(index);
}

uint
ci_fan_state (uint index)
{
    uchar int_stat;

    int_stat = get_int_status(chasint);
    if (INT_STATUS_BFAIL(int_stat) &&
	(index == INT_STATUS_BLOWERNUM(int_stat)))
	return D_ciscoEnvMonFanState_warning;
    return D_ciscoEnvMonFanState_normal;
}

/*
 * routines to fill-in the power-supply table
 */

boolean
ci_ps_valid (uint index)
{
    if (chasint == NULL)
	return FALSE;
    if (index < 1)
	return FALSE;
    if (index > ps_count)
	return FALSE;
    return TRUE;
}

char*
ci_ps_descr (uint index)
{
    uchar ps_id;

    if (index == 1)
	ps_id = PS_ID_PS1_ID(chasint->PS_ID);
    else
	ps_id = PS_ID_PS2_ID(chasint->PS_ID);
    return ps_name(ps_id, backplane_id);
}

uint
ci_ps_state (uint index)
{
    uchar ps_id;

    if (index == 1)
	ps_id = PS_ID_PS1_ID(chasint->PS_ID);
    else
	ps_id = PS_ID_PS2_ID(chasint->PS_ID);
    if (ps_removed(ps_id))
	return D_ciscoEnvMonSupplyState_notPresent;
    if (INT_STATUS_PSX_FAIL(get_int_status(chasint), index))
	return D_ciscoEnvMonSupplyState_warning;
    return D_ciscoEnvMonSupplyState_normal;
}
uint
ci_ps_source (uint index)
{
    uchar ps_id;
    uint ps_src;
    ps_src = D_ciscoEnvMonSupplySource_unknown;
    if (index == 1)
	ps_id = PS_ID_PS1_ID(chasint->PS_ID);
    else
	ps_id = PS_ID_PS2_ID(chasint->PS_ID);
    if (ps_removed(ps_id))
	return D_ciscoEnvMonSupplySource_unknown;
    switch (BACKPLANE_7505) {
      case BACKPLANE_7507:
      case BACKPLANE_7506:
        switch (ps_id) {
          case BP7507PS850W:
          case BP7507PS700W:
            ps_src = D_ciscoEnvMonSupplySource_unknown;
            break;
        }
	break;
      case BACKPLANE_7505:
        switch (ps_id) {
          case BP7505PS600WAC:
            ps_src = D_ciscoEnvMonSupplySource_ac;
            break;
          case BP7505PS600WDC:
            ps_src = D_ciscoEnvMonSupplySource_dc;
            break;
        }
	break;
      case BACKPLANE_7513:
        switch (ps_id) {
          case BP7513PS1200WAC:
            ps_src = D_ciscoEnvMonSupplySource_ac;
            break;
          case BP7513PS1200WDC:
            ps_src = D_ciscoEnvMonSupplySource_dc;
            break;
        }
	break;
    }
    return ps_src;
}
      
