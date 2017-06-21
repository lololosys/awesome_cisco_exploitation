/* $Id: ci_dbus.h,v 3.2 1995/11/17 18:46:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-rsp/ci_dbus.h,v $
 *------------------------------------------------------------------
 * ci_dbus.h
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * defines CI card dbus interface
 *------------------------------------------------------------------
 * $Log: ci_dbus.h,v $
 * Revision 3.2  1995/11/17  18:46:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * chassis interface occupies dbus slot 31
 */
#define CHASSIS_INTERFACE_SLOT	31
#define DBUS_NUM_SLOTS		32
#define SLOTS_PER_BYTE		 8

/*
 * private data types
 */
typedef struct chasint_dbus_ {			/* byte offset */
    uchar	DBus_Slot;			/* 0x00 */
    uchar	Last_Cmd;
    uchar	DBus_Status;			/* non-standard usage */
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
    uchar	PS_ID;
    uchar	Int_Status;
    uchar	Int_Mask;
    uchar	DBUS[DBUS_NUM_SLOTS /
		     SLOTS_PER_BYTE];		/* 0x14 */
    uchar	Power_Ctl;			/* 0x18 */
    uchar	Power_Status;
    uchar	AD_Ctl;
    uchar	AD_Result;
    uchar	:8;				/* 0x1c */
    uchar	:8;
    uchar	:8;
    uchar	:8;
    /* defined in sys/hes/dgbus.h */
    dgbus_eeprom_t eeprom;			/* 0x20 */
} chasint_dbus_t;

#define DBUS_STATUS_CI_ERR(x)		(((x) & 0x40) != 0)
#define DBUS_STATUS_CI_COND(x)		((x) & 0x0f)

#define BPLANE_ID_ARB_ID(x)		((x) >> 4)
#define BPLANE_ID_BACKPLANE_ID(x)	((x) & 0x07)
#define   BACKPLANE_7506		0
#define   BACKPLANE_7510		1
#define   BACKPLANE_7520		2
#define   BACKPLANE_7500		4
#define   BACKPLANE_TOKTEST		7
#define   BACKPLANE_UNKNOWN		8

#define PS_ID_PS1_ID(x)			((x) & 0x07)
#define PS_ID_PS2_ID(x)			((x) >> 4)
#define   BP7520PS1200WAC		1
#define   BP7520PS1200WDC		2

#define INT_STATUS_TEMP(x)		(((x) & 0x80) != 0)
#define INT_STATUS_PS1_FAIL(x)		(((x) & 0x40) != 0)
#define INT_STATUS_PS2_FAIL(x)		(((x) & 0x20) != 0)
#define INT_STATUS_BFAIL(x)		(((x) & 0x10) != 0)
#define INT_STATUS_BLOWERNUM(x)		((x) & 0x0f)

#define INT_MASK_TEMP			0x80
#define INT_MASK_PS1FAIL		0x40
#define INT_MASK_PS2FAIL		0x20
#define INT_MASK_BFAIL			0x10
#define INT_MASK_PS_SHUTDOWN(x)		(((x) & 0x08) != 0)
#define INT_MASK_BOARD_SHUTDOWN(x)	(((x) & 0x04) != 0)
#define INT_MASK_TEMP_WARN(x)		(((x) & 0x02) != 0)
#define INT_MASK_RESTART_OK(x)		(((x) & 0x01) != 0)
#define INT_TRIP_POINTS(x)		((x) & 0x0f)

static inline boolean
dbus_present_inline (uchar *dbus, uint slot)
{
    return ((dbus[slot / SLOTS_PER_BYTE] &
	     (1 << (slot & (SLOTS_PER_BYTE - 1)))) != 0);
}

#define DBUS_PRESENT(ci, slot)		dbus_present_inline(ci,slot)

#define POWER_CTL_OVERRIDE		0x80
#define POWER_CTL_BP_SHUT		0x40
#define POWER_CTL_PS2_SHUT		0x20
#define POWER_CTL_PS1_SHUT		0x10
#define POWER_CTL_TEMPR			0x08
#define POWER_CTL_TEMPSEL		0x04
#define POWER_CTL_SDA			0x02
#define POWER_CTL_SCL			0x01

#define POWER_STATUS_TRIP_POINT(x)	((x) >> 4)
#define POWER_STATUS_CYCLE_COUNT(x)	((x) & 0x0f)

#define AD_CTL_INPUT_SELECT(x)		((x) & 0x1f)
#define   MUX_RSP1_INLET_TEMP		0
#define   MUX_RSP1_HOT_SPOT		1
#define   MUX_RSP1_EXHAUST		2
#define   MUX_RSP2_INLET_TEMP		3
#define   MUX_RSP2_HOT_SPOT		4
#define   MUX_RSP2_EXHAUST		5
#define   MUX_PLUS12V_MONITOR		6
#define   MUX_PLUS5V_MONITOR		7
#define   MUX_MINUS12V_MONITOR		8
#define   MUX_PLUS24V_MONITOR		9
#define   MUX_VREF_2POINT5		10
#define   MUX_PS1_PLUS5V_CURRENT	11
#define   MUX_PS2_PLUS5V_CURRENT	12
#define   MUX_PS1_PLUS12V_CURRENT	13
#define   MUX_PS2_PLUS12V_CURRENT	14
#define   MUX_PS1_MINUS12V_CURRENT	15
#define   MUX_PS2_MINUS12V_CURRENT	16

#define CIMUX_TEMP(x)		(((x) * 977 / 1000) - 148)
#define CIMUX_mV_PLUS5(x)	(((x) * 195 +  25000) / 10)
#define CIMUX_mV_PLUS12(x)	(((x) * 468 +  59900) / 10)
#define CIMUX_mV_MINUS12(x)	 ((x) * -47 -   6010)
#define CIMUX_mV_PLUS24(x)	(((x) * 944 + 120700) / 10)
#define CIMUX_mV_2POINT5(x)	(((x) * 977 + 125000) / 100)

/*
 * for 7520 power supply types 1 & 2
 */
#define CIMUX_mAMP_P5_7520(x)	((x) *  5*1000*120 / 255)
#define CIMUX_mAMP_P12_7520(x)	((x) * 12*1000*35  / 255)
#define CIMUX_mAMP_M12_7520(x)	((x) * 12*1000*3   / 255)
























