/* $Id: hub.h,v 3.2.62.2 1996/08/28 12:56:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/hub.h,v $
 *------------------------------------------------------------------
 * hub.h -- definitions for hub
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub.h,v $
 * Revision 3.2.62.2  1996/08/28  12:56:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/03/18  20:40:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:22:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:37:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:13:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  06:47:07  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:31:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef __HUB_H__
#define __HUB_H__

/*
 *  Hub Miscellaneous Register
 */
#define BIT1_UNUSED                   0x0001
#define BIT2_UNUSED                   0x0002
#define BIT3_UNUSED                   0x0004
#define BIT4_UNUSED                   0x0008
#define BIT5_UNUSED                   0x0010
#define BIT6_UNUSED                   0x0020
#define BIT7_UNUSED                   0x0040
#define BIT8_UNUSED                   0x0080
#define ETHER_DAUGHTER_CARD           0x0100
#define ETHER_LOOPBACK_DGTR_TEST      0x0200
#define ETHER_LOOPBACK_MOM_TEST       0x0400
#define ETHER_HUB_RESET               0x0800
#define ETHER_MDI_MODE                0x1000
#define BIT14_UNUSED                  0x2000
#define BIT15_UNUSED                  0x4000
#define ETHER_THERMAL_SHUTDOWN        0x8000

#define PORTS_PER_DEVICE_6 6
#define PORTS_PER_DEVICE_8 8

#define HW_DATA_BUFFER 6

/*
 * Use this macro to get mask of a port
 */
#define GET_PORT_MASK(port) (0x01 << (port))

/*
 * Hardware addresses - see boot/src-68-c3000/pcmap.h
 */


/*
 * Hub data structures
 */

/*
 * Set of network management data -- general per hub data.
 */
struct hub_mgmt_t_ {
    /* ether snmp fields */
    /* snmp_grouptable_items  -- some rptr_group values used here */
    int grp_description;        /* enum RPTR_SNMP_TEXT */
    sys_timestamp grp_last_oper_stat_change;/* sysuptime at last oper change */
    /* snmp_rptr_group */
    int health_text;            /* enum RPTR_SNMP_TEXT */
    uint oper_status;           /* operational status */
    uint illegal_addr_trap_algorithm;  /* it's either once or decay */
};

/*
 * Set of general counters held for ether hub.
 * Required since ethernet hub hardware counters cannot be cleared.
 */
struct himib_repeater_info_t_ {
    uint total_octets_count_base;
    uint tx_collisions_count_base;
};

/*
 * Set of counters held for each port.  Required since ethernet hub
 * hardware counters cannot be cleared.
 */
struct himib_port_info_t_ {
    sys_timestamp last_clear_time;
    uint frames_count_base;
    uint octets_count_base;
    uint fcs_count_base;
    uint align_err_count_base;
    uint long_count_base;
    uint short_count_base;
    uint runts_count_base;
    uint collisions_count_base;
    uint lates_count_base;
    uint long_events_count_base;
    uint mismatch_count_base;
    uint partitions_count_base;
    uchar source_addr[HW_DATA_BUFFER];
    uchar learned_addr[HW_DATA_BUFFER];
    uchar last_illegal_source_addr[HW_DATA_BUFFER];
    uchar current_illegal_source_addr[HW_DATA_BUFFER];
    uint last_mac_security_state;
    uint current_mac_security_state;
    uint illegal_addr_trap_acked; 
    ulong illegal_addr_first_heard;
    uint illegal_addr_trap_enabled;
    ulong illegal_addr_last_heard;
    uint last_illegal_addr_count; /* last illegal_addr count on this port */
    uint illegal_addr_total_count; /* tatal count of all the illegal addr on this port */
    uint illegal_addr_trap_count;
    boolean illegal_addr_occured;
    boolean illegal_addr_countdown_started;
    uint illegal_addr_sleep_time;
    
};

/*
 *  Structure for ethernet specific portion of device structure.
 */
struct hub_ether_t_ {
    uchar *ether_himib_control; /* ptr to himib control register */
    uchar *ether_himib_data;    /* ptr to himib data register */
    uchar link_test;            /* one bit per port, set if test enabled */
    uchar automatic_polarity;   /* one bit per port, set if enabled
				   (cable polarity reversal accepted) */
    uchar LSA_correct;          /* one bit per port, set if HIMIB LSA
                                   reflects reality */
    uchar source_addr_mismatch; /* one bit per port, set if port has been
                                   disabled due to source address mismatch */
    uchar source_addr_config;   /* one bit per port, set if user entered
                                   a mac-address to control */
    uchar source_addr_learn;    /* one bit per port, set if port has learned
                                   a mac-address to control */
    uchar source_addr_snmp_pending; /* one bit per port, set if snmp entered
                                   a mac-address to control */
    uchar source_addr_illegal;  /* one bit per port, set if port has been
                                   disabled at least once by mac-address ctrl */

    uchar source_addr_change_int;
                                /* one bit per port, set if int
                                   enabled by user */
    uchar partition_state;      /* one bit per port, set if partitioned */
    himib_repeater_info_t *gen_info;
                                /* structure for general repeater
                                   counter values */
    himib_port_info_t *port_info;
                                /* structure for individual repeater
				   port counter values */
};

/*
 * Defines a single device that is (part of) this "hub".
 */
struct hub_device_t_ {
    hub_device_t *next_hub_device;
                                /* link all devices in this "hub" */
    hub_t *parent_hub;          /* device is part of this hub */
    hub_ether_t *hub_ether;     /* ptr to ethernet hub repeater structure */
    int num_ports;              /* number of ports on this device */
    uchar enabled;              /* one bit per port. set if enabled */
};

/*
 * Defines a single "hub" attached to an internal LAN port.
 */
struct hub_t_ {
    hub_t *next_hub;            /* link all hubs in box */
    int hub_type;               /* ether, tring, etc. */
    int hub_number;             /* usually 0 */
    struct hwidbtype_ *host_hwidb;
                                /* pointer to internally connected interface */
    uint resets;                /* number of times hub gets reset */
    sys_timestamp last_clear_time; /* time of last clear/reset */
    hub_device_t *hub_device;   /* list of devices comprising this hub */
    hub_mgmt_t *management;     /* ptr to general hub management data */
};

/*
 * Defines struct needed to access a hub device.
 */
struct hub_access_t_ {
    int hub_number;             /* target hub */
    int hub_type;               /* target hub type */
    hub_device_t *device;       /* target hub device */
    int phys_port;              /* target port */
    int access_code;            /* designate access */
    uchar buffer[HW_DATA_BUFFER]; /* buffer to hold data */
};


/*
 * Globals
 */
hub_t *hub_list;


/*
 * External declarations
 */
extern boolean hub_debug;

/*
 * Function prototypes
 */
extern int hub_count_ports (int, int);
extern hub_t *hub_get_hub_ptr (int, int);
extern void hub_parser_init (void);
extern void hub_debug_init (void);
extern void hub_if_final_init (void);
extern boolean hub_map_port (int, hub_access_t *);
extern void hub_reset (hub_t *hub);
extern boolean hub_platform (void);

#endif










