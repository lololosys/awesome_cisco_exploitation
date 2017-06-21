/* $Id: dsx1.h,v 3.5.12.6 1996/08/27 01:22:02 vdukki Exp $
 * $Source: /release/112/cvs/Xsys/if/dsx1.h,v $
 *----------------------------------------------------------------
 * dsx1.h - Support for Channelized T1/E1, Primary rate ISDN
 *
 * 29-August-1994, Bill May (from if_mip.h)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dsx1.h,v $
 * Revision 3.5.12.6  1996/08/27  01:22:02  vdukki
 * CSCdi52688:  After clearing pri counters, sho controller shows loss of
 * signal
 * Branch: California_branch
 * Created dsx1_clear_mib_data function to clear the dsx1 MIB table. It
 * will be called when "clear counters" command is run, instead of
 * dsx1_init_mib_data().
 *
 * Revision 3.5.12.5  1996/08/22  21:47:48  vdukki
 * CSCdi65777:  Channelized E1 interface doesnt respond to ds1 MIB table
 * requests
 * Branch: California_branch
 * Modified the code to look for E1 interfaces, in addition to T1
 * interfaces, while accessing the ds1 table. Also did some code
 * cleanups.
 *
 * Revision 3.5.12.4  1996/08/07  19:05:03  wmay
 * CSCdi62173:  Setting pri-group timeslots 1-24 causes all pri calls to
 * drop - check if timeslots change before making the change
 * Branch: California_branch
 *
 * Revision 3.5.12.3  1996/07/15  20:43:06  towu
 * CSCdi61208:  show cont t1 for cas channel does not display transmit RBS
 * status
 * Branch: California_branch
 *         o Added feature to show rbs xmit status when serial debug turn
 *         on.
 *         o Added new field to collect rbs xmit status in dsx1info
 *         structure.
 *
 * Revision 3.5.12.2  1996/07/11  22:39:23  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *            o Added new fdl command for t1 controller in order to support
 *              CSU function
 *
 * Revision 3.5.12.1  1996/06/16  21:12:50  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.5  1996/02/15  07:46:35  smackie
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net. (CSCdi49069)
 *
 * Revision 3.4  1996/02/13  08:12:46  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/11/17  09:26:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/04  19:37:42  wmay
 * CSCdi40145:  DS1 MIB returns bad values - initialize to good values
 *
 * Revision 2.2  1995/06/19  21:51:00  wmay
 * CSCdi36002:  CE1 layer 1 fails Australia Homologation - add Australia
 * option to framing controller E1 command.
 *
 * Revision 2.1  1995/06/07  20:51:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __DSX1_H__
#define __DSX1_H__
/*
 *	Channels are numbered from 0, timeslots from 1.  What a pain.
 */
#define	T1_MAXCHAN	23
#define	E1_MAXCHAN	30
#define	T1_MAXTS	24
#define	E1_MAXTS	31

#define DSX1_MAX_CHAN	E1_MAXCHAN
#define DSX1_MAX_TS	E1_MAXTS

#define E1_ISDN_D_CHANNEL	15
#define E1_ISDN_D_TIMESLOT	16
#define T1_ISDN_D_CHANNEL	T1_MAXCHAN
#define T1_ISDN_D_TIMESLOT	T1_MAXTS

#define ISDN_TS_TO_CHAN_GROUP(x) (x - 1)

#define FOR_ALL_DSX1_CHANNELS(ix,dsx1) \
        for (ix = 0; ix <= dsx1->max_channels; ix++)

#define FOR_ALL_DSX1_TIMESLOTS(ix,dsx1) \
        for (ix = 1; ix <= dsx1->max_timeslots; ix++)

/*
 * Defines to enable the correct use of DSX1 parser chains.
 */
#ifdef SLOT_BASED_CDB_PLATFORM
#define DEFAULT_DSX1_CTRLR_SLOTS 		MAX_SLOTS
#define DEFAULT_DSX1_CTRLR_INTERFACES        MAX_CTRLR_INTERFACES
#define DEFAULT_DSX1_CTRLR_UNITS             MAX_CTRLR_INTERFACES
#else
#define DEFAULT_DSX1_CTRLR_SLOTS 		3
#define DEFAULT_DSX1_CTRLR_INTERFACES        3
#define DEFAULT_DSX1_CTRLR_UNITS             3 /* Current Maximum */
#endif

#define UNUSED_CHAN -1

/* these enums are carefully crafted to match the MIP's magic numbers */
enum appl_type {
    APPL_T1 = 1,         /* T1 with DSX1 interface */
    APPL_E1_UNBAL = 2,
    APPL_E1_BAL = 3,
    APPL_6B = 4, 
    APPL_T1_CSU = 5,     /* T1 with integrated CSU */
    APPL_NONE = 0xf,
};

/*
 * DSX1 enums.  Values are defined in RFC 1406.
 */
enum dsx1_framing_type {
    DSX1_FRM_ESF = 2,
    DSX1_FRM_SF = 3,
    DSX1_FRM_E1 = 4,
    DSX1_FRM_E1_CRC = 5,
};
enum dsx1_clocking_type {
    DSX1_CLK_LINE = 1,
    DSX1_CLK_INTERNAL = 2,
    DSX1_CLK_THRU = 3,
    DSX1_CLK_FREE = 4,	/* for cisco hidden command */
};
enum dsx1_linecode_type {
    DSX1_LC_B8ZS = 2,
    DSX1_LC_HDB3 = 3,
    DSX1_LC_AMI = 5,
};
enum dsx1_speed_type {
    DSX1_SPEED_8  = 0x01,
    DSX1_SPEED_16 = 0x03,
    DSX1_SPEED_24 = 0x07,
    DSX1_SPEED_32 = 0x0f,
    DSX1_SPEED_40 = 0x1f,
    DSX1_SPEED_48_I460 = 0x3f,
    DSX1_SPEED_48 = 0x7e,	/* command to the MIP */
    DSX1_SPEED_56 = 0x7f,	/* shifted to be or'd into the MAP */
    DSX1_SPEED_64 = 0xff,	/* this are the masks to use */
};
/* for dsx1LoopbackConfig */
enum dsx1_loopback_config_type {
    DSX1_NO_LOOP = 1,
    DSX1_PAYLOAD_LOOP = 2,
    DSX1_LINE_LOOP = 3,
    DSX1_OTHER_LOOP = 4,
};
enum dsx1_loop_type {
    DSX1_LOOP_NONE = 1,  
    DSX1_LOOP_LOCAL = 3, 
    DSX1_LOOP_REMOTE = 4,
};
enum dsx1_cable_length {
    DSX1_LENGTH_133 = 1,
    DSX1_LENGTH_266 = 2,
    DSX1_LENGTH_399 = 3,
    DSX1_LENGTH_533 = 4,
    DSX1_LENGTH_655 = 5,
};

enum dsx1_clocksource_type {
    DSX1_PRIMARY = 1,
    DSX1_SECONDARY = 2,
    DSX1_THIRD = 3,
    DSX1_FOURTH = 4,
    DSX1_FIFTH = 5,
    DSX1_SIXTH = 6,
    DSX1_SEVENTH = 7,
    DSX1_EIGHTH = 8,
};

enum dsx1_pulse_gain_type {
    DSX1_0DB = 1,
    DSX1_75DB = 2,
    DSX1_15DB = 3,
    DSX1_225DB = 4,
};

enum dsx1_length_type {
    DSX1_SHORT = 1,
    DSX1_LONG = 2,
};

enum dsx1_gain_type {
    DSX1_GAIN26 = 1,
    DSX1_GAIN36 = 2,
};

enum dsx1_sig_type {
    DSX1_SIGTYPE_NOSIG = 0,
    DSX1_SIGTYPE_EM,
};

/*
 * homologation types are layed out so that the mip can get the
 * commands directly.  This means add to the end, dummy
 */
enum dsx1_homologation_type {
    DSX1_HOMOL_NORMAL,
    DSX1_HOMOL_AUSTRALIA,
};
enum dsx1_fdl_type {
    DSX1_FDL_ANSI = 2,
    DSX1_FDL_ATT = 4,
    DSX1_FDL_NONE = 8,
};

typedef unsigned long	Gauge;
typedef unsigned short	SmGauge;

#define MAX_GAUGE_COUNT 0x7fffffff

static inline void ADD_TO_GAUGE (Gauge *a, Gauge b)
{
    *a += b;
    if ( *a > MAX_GAUGE_COUNT )
	*a = MAX_GAUGE_COUNT;
}

/*
 * Caution: The following structure is actually "passed over the cbus",
 * so it must be defined in such a way that it will be padded identically
 * on the RP1's 68K, the MIP's 68K, and the RSP's Orion
 * (see also if_mip.h).
 */
typedef struct dsx1TableType_ {
    Gauge	dsx1PCVs;
    Gauge	dsx1LCVs;
    SmGauge	dsx1Interval;
    SmGauge	dsx1ESs;
    SmGauge	dsx1SESs;
    SmGauge	dsx1SEFSs;
    SmGauge	dsx1UASs;
    SmGauge	dsx1CSSs;
    SmGauge	dsx1LESs;
    SmGauge	dsx1BESs;
    SmGauge	dsx1DMs;
    SmGauge     dummy;
} dsx1TableType;

typedef struct dsx1TotalTableType_ {
    Gauge dsx1ESs;
    Gauge dsx1SESs;
    Gauge dsx1SEFSs;
    Gauge dsx1UASs;
    Gauge dsx1CSSs;
    Gauge dsx1PCVs;
    Gauge dsx1LESs;
    Gauge dsx1BESs;
    Gauge dsx1DMs;
    Gauge dsx1LCVs;
} dsx1TotalTableType;

/*
 * Now the data that we store on the RP
 */

typedef struct dsx1ConfigTable_  {
    int dsx1LineIndex;					/* Not used */
    int dsx1IfIndex;					/* Not used */
    int dsx1TimeElapsed;
    int dsx1ValidIntervals;
    int dsx1LineType;
    int dsx1LineCoding;
    int dsx1SendCode;					/* Not used */
    uchar dsx1CircuitIdentifier[256];			/* Not used */
    int dsx1LoopbackConfig;
    int dsx1LineStatus;
    int dsx1SignalMode;					/* Not used */
    int dsx1TransmitClockSource;
    int dsx1Fdl;					/* Not used */
} dsx1ConfigTable;

#define DSX1_NO_ALARM		0x001
#define DSX1_RX_REM_ALARM	0x002
#define DSX1_TX_REM_ALARM	0x004
#define DSX1_RX_AIS		0x008
#define DSX1_TX_AIS		0x010
#define DSX1_RX_LOF		0x020
#define DSX1_RX_LOS		0x040
#define DSX1_LOOPED		0x080
#define DSX1_TS16_AIS		0x100
#define DSX1_RX_LOMF		0x200
#define DSX1_TX_LOMF		0x400
#define DSX1_HAVE_ERRORS        0x1ffe


#define DSX1_ALL_RX (DSX1_RX_REM_ALARM | DSX1_RX_AIS | DSX1_RX_LOF | \
		     DSX1_RX_LOS |DSX1_TS16_AIS | DSX1_RX_LOMF )
#define DSX1_ALL_TX ( DSX1_TX_REM_ALARM | DSX1_TX_AIS | DSX1_TX_LOMF )

/*
 * This is defined here because it is used in the RFC 1406 defines.
 * Things are stored in 15 minute buckets for 24 hours
 */
#define QUARTER_HOURS_IN_DAY 96

typedef struct dsx1MIBDataTable_ {
    int   		CurrentIndex;	/* to see if indexes match with mip */
    dsx1ConfigTable	C;
    dsx1TableType	CurrentData;	/* dsx1Interval not used */
    dsx1TotalTableType	TotalData;	/* same here */
    dsx1TableType	Intervals[QUARTER_HOURS_IN_DAY];
} dsx1MIBDataTable;

/*
 * Here's the info that we need for a DSX1 interface
 */
typedef struct dsx1info_ {
    hwidbtype		*hwidb[DSX1_MAX_CHAN + 1];	/* the vc's hwidb */
    hwidbtype		*isdn_d_chan_hwidb;
    struct cdbtype_	*cdb;
    int			unit;
    enum pif_types_	pif_type;
    enum appl_type	applique_type;

    enum dsx1_loop_type		loop;
    enum dsx1_framing_type	framing;
    enum dsx1_clocking_type	clocking;
    enum dsx1_clocksource_type  clocksource;
    enum dsx1_linecode_type	linecode;
    enum dsx1_fdl_type	        fdl;
    enum dsx1_cable_length	cablelength;
    enum dsx1_pulse_gain_type   pulse;
    enum dsx1_length_type       length;
    enum dsx1_gain_type         gain;
    enum dsx1_homologation_type	homologation_type;
    enum dsx1_speed_type	speed[DSX1_MAX_CHAN + 1]; /* by channel */
    /*
     * Channel numbers run 0 to MAX_CHAN (23 or 30)
     * Timeslots run 1 to 24 or 32
     * isdn_d_chan has the channel number of the isdn d channel
     * isdn_d_timeslot has the timeslot.  This should be a
     * chan = timeslot - 1 relationship, but might change
     */
    ushort max_channels;
    ushort max_timeslots;
    ushort isdn_d_chan;
    ushort isdn_d_timeslot;
    ulong  pri_timeslot_bitmap;  /* bitmap for checking for changes */
    
    short timeslots[DSX1_MAX_TS + 1];  /* channel ts assigned to, -1 = none) */

    dsx1MIBDataTable	MIB;		/* MIB data from above */
    /*
     * The next 2 are routines that we need to convert from
     * the way the isdn code handles numbering to the way that
     * the dsx1 code handles numbering
     */
    ulong (*bchan_bitfield_to_dsx1_bitfield)(ulong);
    enum Chan (*timeslot_to_bchan)(ushort);

    int    pool_group[DSX1_MAX_CHAN + 1]; /* by  channel */
    enum dsx1_sig_type signal_type[DSX1_MAX_CHAN]; /* Only availabe if the channel is cas */
    uchar  rx_rbs[DSX1_MAX_CHAN/2];  /* robbed bits signals on rx; 4 bits per channel */
    uchar  tx_rbs[DSX1_MAX_CHAN/2];  /* robbed bits signals on tx line */

}dsx1info;

static inline boolean is_t1_cas(dsx1info *dsx, int chan)
{
    return (dsx && (dsx->signal_type[chan] != DSX1_SIGTYPE_NOSIG));
}

static inline boolean is_t1_digit_channel(dsx1info *dsx, int chan)
{
    return (dsx && (dsx->signal_type[chan] == DSX1_SIGTYPE_NOSIG));
}

/* definitions for RBS signal bits */
#define DSX1_RBS_ESF_BITA     0x08
#define DSX1_RBS_ESF_BITB     0x04
#define DSX1_RBS_ESF_BITC     0x02
#define DSX1_RBS_ESF_BITD     0x01

#define DSX1_RBS_SF_BITA      0x02
#define DSX1_RBS_SF_BITB      0x01


extern dsx1info **dsx1_list;
extern int dsx1_max_dsx1s;
extern boolean dsx1_cas_supp;
static inline dsx1info *dsx1_find_by_unit (int unit)
{
    if (unit >= dsx1_max_dsx1s)
	return(NULL);
    return dsx1_list[unit];
}

void init_dsx1_subsystem(int);
dsx1info *dsx1_create_context(cdbtype *, int, boolean);
void dsx1_preallocate(boolean, ushort);
int make_bit_field(int, int);
void dsx1_reset(cdbtype *);
char *dsx1_applique_name(int);
int dsx1_count_timeslots(dsx1info *, int);
boolean dsx1_show_timeslots(char *, dsx1info *, int);
void dsx1_change_idb_state(hwidbtype *, enum IDB_STATES);
void dsx1_change_all_idbs_state(cdbtype *, enum IDB_STATES);
void dsx1_init_mib_data(dsx1info *);
void dsx1_clear_mib_data(dsx1info *);
dsx1MIBDataTable *dsx1_find_SNMP(long *, boolean);
enum IDBTYPE dsx1_get_idbtype(cdbtype *cdb);

/* From dsx1_chain.c */
extern void add_t1_commands(void);
extern void add_e1_commands(void);
extern const char *get_dsx1_framing(enum dsx1_framing_type);
extern const char *get_dsx1_homologation_type(enum dsx1_homologation_type);
#endif /* __DSX1_H__ */




