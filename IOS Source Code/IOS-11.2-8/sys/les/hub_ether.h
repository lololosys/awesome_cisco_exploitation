/* $Id: hub_ether.h,v 3.2 1995/11/17 17:37:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/hub_ether.h,v $
 *------------------------------------------------------------------
 * hub_ether.h - header for ether hub general functions.
 *
 * March 1994, Steve Garcia
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_ether.h,v $
 * Revision 3.2  1995/11/17  17:37:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:14:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:32:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __HUB_ETHER_H__
#define __HUB_ETHER_H__

#define HIMIB_AUI_PORT        0x0f   /* HIMIB's AUI "port number" */
#define AUI_PORT_MASK         0x80   
#define ALL_TP_PORTS_MASK     0xff

#define MAX_HIMIB_VAL         0xffffffff /* max value for himib's counters */
#define MDI_SWITCHABLE_PORT   6          /* MDI/MDI-X switchable port */

/* HIMIB/IMR+ access control byte masks */
#define HIMIB_CTRL_RESERVED   0x00
#define HIMIB_CTRL_WRITE      0x01
#define HIMIB_CTRL_READ       0x02
#define HIMIB_CTRL_PORT       0x04
/* unused                     0x08 */
#define IMR_CTRL_WRITE        0x10
#define IMR_CTRL_READ         0x20
#define IMR_CTRL_PORT         0x40
#define HIMIB_CTRL_IMR_DIRECT 0x80

/* HIMIB configuration register to enable interrupts */
#define HIMIB_ENABLE_INT      0x80 /* Enable Interrupts */
#define HIMIB_IF_ERR_INT      0x40 /* Interface Error Interrupt Enable */
#define HIMIB_SA_MATCH_INT    0x20 /* Source Address Match Interrupt Enable */
#define HIMIB_MAC_IF_MODE_INT 0x10 /* MAC Interface Mode Enable */

/* column indeces of HIMB_access_arr */
#define HIMIB_P_REG          0
#define HIMIB_R_REG          1
#define HIMIB_NUM_BYTES      2
#define IMR_SI               3
#define HIMIB_ACCESS_CTRL    4
#define HIMIB_ACCESS_MAX_COL 4   /* pay attention here, Dudes */

#define HIMIB_ACCESS_MAX_ROW 54

/*
 * HIMIB access control.
 */

/* row indeces of HIMIB_control_arr */
/* repeater registers */
#define SOURCE_ADDR_MATCH_CODE                     0
#define TOTAL_OCTETS_CODE                          1
#define TRANSMIT_COLLISIONS_CODE                   2
#define CONFIG_REG_CODE                            3
#define VERSION_DEVICE_ID_CODE                     4
#define IMR_PLUS_SET_REG_CODE                      5
#define IMRPlusGetRegisterCode                     6
/* port status registers */
#define TP_PARTITION_STATUS_CHANGE_CODE            7
#define AUI_PARTITION_STATUS_CHANGE_CODE           8
#define TP_LINK_STATUS_CHANGE_CODE                 9
#define AUI_LOOP_BACK_ERR_CODE                    10
#define RESERVED_PORT_STATUS                      11
#define AUI_SQE_TEST_ERR_CODE                     12
#define TP_SOURCE_ADDR_CHANGE_CODE                13
#define AUI_SOURCE_ADDR_CHANGE_CODE               14
#define TP_SOURCE_ADDR_MATCH_STATUS_CODE          15
#define AUI_SOURCE_ADDR_MATCH_STATUS_CODE         16
/* port control registers */
#define TP_PART_CHANGE_INT_ENABLE_CODE            17
#define AUI_PART_CHANGE_INT_ENABLE_CODE           18
#define TP_LINK_STATUS_CHANGE_INT_ENABLE_CODE     19
#define AUI LOOPBACK_ERR_INT_ENABLE               20
#define RESERVED_PORT_CONTROL                     21
#define AUI_SQE_TEST_ERR_INT_ENABLE_CODE          22
#define TP_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE     23
#define AUI_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE    24
/* attribute registers */
#define READABLE_FRAMES_CODE                      25
#define READABLE_OCTETS_CODE                      26
#define FCS_ERR_CODE                              27
#define ALIGNMENT_ERR_CODE                        28
#define FRAMES_TOO_LONG_CODE                      29
#define SHORT_EVENTS_CODE                         30
#define RUNTS_CODE                                31
#define COLLISIONS_CODE                           32
#define LATE_EVENTS_CODE                          33
#define VERY_LONG_EVENTS_CODE                     34
#define DATA_RATE_MISMATCHES_CODE                 35
#define AUTO_PARTITIONS_CODE                      36
#define SOURCE_ADDR_CHANGES_CODE                  37
#define RESERVED_ATTRIBUTES                       38
#define LAST_SOURCE_ADDR_CODE                     39
/* IMR+ "get" codes */
#define AUI_PORT_STATUS_BSL_CODE                  40
#define TP_PORT_PARTITIONING_STATUS_CODE          41
#define TP_PORT_BIT_RATE_STATUS_CODE              42
#define TP_PORT_LINK_TEST_STATUS_CODE             43
#define TP_PORT_RECEIVE_POLARITY_STATUS_CODE      44
#define MJLP_STATUS_CODE                          45
#define IMR_VERSION_CODE                          46
/* IMR+ "set" codes */
#define AUI_PORT_DISABLE_CODE                     47
#define AUI_PORT_ENABLE_CODE                      48
#define TP_PORT_DISABLE_CODE                      49
#define TP_PORT_ENABLE_CODE                       50
#define TP_PORT_DISABLE_LINK_TEST_FUNCTION_CODE   51
#define TP_PORT_ENABLE_LINK_TEST_FUNCTION_CODE    52
#define TP_PORT_DISABLE_RXR_POLARITY_REVERS_CODE  53
#define TP_PORT_ENABLE_RXR_POLARITY_REVERS_CODE   54

/* Illegal source address trap state */
#define VALID_MAC 1
#define INVALID_MAC 2

/* The following 3 sets of defines are in the mib.
   Please refer sr_rptrmibdefs.h
 */  
/* Illegal source address trap algorithm */
#define ONCE 1
#define DECAY 2

/* Illegal source address trap acked */
#define ACKED 1
#define NO_ACKED_SENDING 2
#define NO_ACKED_NO_SENDING 3

/* Illegal source address trap enabled */
#define TRAP_ENABLED_TRUE 1
#define TRAP_ENABLED_FALSE 2

/*
 * Function prototypes
 */
extern hub_device_t *hub_create_ether_device (boolean, hub_t *);
extern void hub_ether_config_port (boolean, hub_access_t *, uint);
extern void hub_ether_enable_interrupts(hub_t *);
extern void hub_ether_interrupt(void);
extern void hub_ether_reset (hub_access_t *);
extern void hub_ether_setup_int (setup_struct *);
extern uint hub_get_val_uint (hub_access_t *);
extern int hub_read_himib (hub_access_t *);
extern void hub_write_himib (hub_access_t *);
extern void reset_himib_counters (int, int, int);
extern void hub_ether_src_addr_ctrl (boolean, hub_access_t *, boolean);
extern uint zero_base_counter (uint, uint);
extern void hub_init_mac_security(hub_t *); 
extern void hub_illegal_addr_trap_process_creation (void);

#endif
