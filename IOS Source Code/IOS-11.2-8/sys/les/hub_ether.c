/* $Id: hub_ether.c,v 3.7.8.3 1996/08/31 00:39:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_ether.c,v $
 *------------------------------------------------------------------
 * hub_ether.c -- ethernet hub general functions
 *
 * March 1994, Steve Garcia
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_ether.c,v $
 * Revision 3.7.8.3  1996/08/31  00:39:05  snyder
 * CSCdi67771:  Engineer confessed to having declared an array without
 *              const in a previous life.  Assist him dealing with his
 *              past sin.
 *              276 out of data, 20 from image.
 * Branch: California_branch
 *
 * Revision 3.7.8.2  1996/08/28  12:56:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.1  1996/03/18  20:40:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:11:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:05:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:08:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/28  09:33:22  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.6  1996/02/01  06:06:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/22  06:37:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/06  03:22:25  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/28  03:58:49  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:37:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:13:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  13:11:18  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:32:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "logger.h"
#include "../les/msg_hub.c"		/* Not a typo, see logger.h */
#include "ieee.h"
#include "../ui/setup.h"
#include "hub.h"
#include "hub_ether.h"
#include "if_les.h"
#include "if_les_lance.h"
#include "../os/timer_generic.h"
#include "sr_rptrmib.h"
#include "sr_rptrmibdefs.h"
#include "hub_registry.h"
#include "parser_defs_hub.h"
#include "../../boot/src-68-c3000/pcmap.h"


static int source_addr_pid = 0;
static int trap_pid = 0;
static void hub_mac_security(himib_port_info_t *port_info);
static forktype hub_illegal_addr_trap_process(void);
static void hub_send_illegal_addr_trap(himib_port_info_t *port_info, int portIndex);
static boolean hub_illegal_addr_trap_ctrlBLOCK(void);
static const uchar himib_access_arr[HIMIB_ACCESS_MAX_ROW+1][HIMIB_ACCESS_MAX_COL+1]
    = {
	/* array format:
	 * Rows indexed by access code indicating what himib data is
	 * desired.
	 * Column format:
	 *    col 0: HIMIB p register;
	 *    col 1: HIMIB r register;
	 *    col 2: number bytes to read/write from HIMIB;
	 *    col 3: IMR+ si data
	 *    col 4: HIMIB/IMR+ access control byte
	 */
	/* repeater registers */
	{0x00, 0xea, 6, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x00, 0xec, 4, 0x00, HIMIB_CTRL_READ},
	{0x00, 0xed, 4, 0x00, HIMIB_CTRL_READ},
	{0x00, 0xf0, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x00, 0xfc, 1, 0x00, HIMIB_CTRL_READ},
	{0x00, 0xfe, 1, 0x00, HIMIB_CTRL_WRITE},
	{0x00, 0xff, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	/* port status registers */
	{0x01, 0xe0, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe1, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe2, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe3, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe4, 0, 0x00, HIMIB_CTRL_RESERVED},
	{0x01, 0xe5, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe6, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe7, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe8, 1, 0x00, HIMIB_CTRL_READ},
	{0x01, 0xe9, 1, 0x00, HIMIB_CTRL_READ},
	/* port control registers */
	{0x02, 0xe0, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe1, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe2, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe3, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe4, 0, 0x00, HIMIB_CTRL_RESERVED},
	{0x02, 0xe5, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe6, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	{0x02, 0xe7, 1, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_WRITE},
	/* attribute registers */
	{0x10, 0xe0, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe1, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe2, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe3, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe4, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe5, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe6, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe7, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe8, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xe9, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xea, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xeb, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xec, 4, 0x00, HIMIB_CTRL_READ | HIMIB_CTRL_PORT},
	{0x10, 0xed, 0, 0x00, HIMIB_CTRL_RESERVED},
	{0x10, 0xee, 6, 0x00,
	     HIMIB_CTRL_READ | HIMIB_CTRL_WRITE | HIMIB_CTRL_PORT},
	/* IMR+ "get" codes */
	{0x00, 0xff, 1, 0x8f, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0x80, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0xa0, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0xd0, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0xe0, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0xf0, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	{0x00, 0xff, 1, 0xff, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_READ},
	/* IMR+ "set" codes */
	{0x00, 0xfe, 1, 0x2f, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x3f, HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x20,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x30,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x40,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x50,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x60,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE},
	{0x00, 0xfe, 1, 0x70,
	     HIMIB_CTRL_IMR_DIRECT | IMR_CTRL_PORT | IMR_CTRL_WRITE}
    };


/*
 * =====================================================================
 * hub_read_himib()
 *
 * Description:
 * Fetch requested data from AMD79C987 "HIMIB" (or IMR+) device.
 *
 * Parameters:
 * access  - pointer to Hub access structure
 *
 * Returns:
 * number of bytes of HIMIB data 
 * HIMIB data in access struct buffer
 * =====================================================================
 */
int hub_read_himib (hub_access_t *access)
{
    uchar p_reg_value;
    uchar *data_ptr;
    int read_count;
    hub_ether_t *hub_ether;
    leveltype SR;

    /*
     * Check if read is allowed.
     */
    if ( !(himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
	      (HIMIB_CTRL_READ | IMR_CTRL_READ)) ) {
         errmsg(&msgsym(READERR, HUB), access->access_code);
         return(0);
    }

    /*
     * Build value for HIMIB P register.
     * If this access is for a port specific value, then need
     * to include the port number in the P register data.
     */
    if (himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
	      HIMIB_CTRL_PORT) {
        /* OR port number into value for P register */
        p_reg_value = himib_access_arr[access->access_code][HIMIB_P_REG] |
            access->phys_port;
    } else {
        /* just use P reg value straight from array */
        p_reg_value = himib_access_arr[access->access_code][HIMIB_P_REG];
    }

    /* turn off level 2 interrupts */
    SR = raise_interrupt_level(LEVEL2_DISABLE);

    /*
     * Select HIMIB port and register by writing to control reg.
     */
    hub_ether = access->device->hub_ether;

    *(hub_ether->ether_himib_control) = p_reg_value;
    *(hub_ether->ether_himib_control) =
	  himib_access_arr[access->access_code][HIMIB_R_REG];

    /*
     * If going directly to IMR+, load HIMIB data reg.
     */
    if (himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
            HIMIB_CTRL_IMR_DIRECT) {
        *(hub_ether->ether_himib_data) =
            himib_access_arr[access->access_code][IMR_SI];
    }

    /*
     * Read right number of data bytes from HIMIB data register.
     */
     for (read_count = 0;
          read_count < himib_access_arr[access->access_code][HIMIB_NUM_BYTES];
          read_count++) {
         data_ptr = hub_ether->ether_himib_data;
         access->buffer[read_count] = *data_ptr;
    }

    reset_interrupt_level(SR);
    return(read_count);

}   /* hub_read_himib */


/*
 * =====================================================================
 * hub_write_himib()
 *
 * Description:
 * Write data to AMD79C987 "HIMIB" (or IMR+) device.
 *
 * Parameters:
 * access - pointer to Hub access structure
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_write_himib (hub_access_t *access)
{
    uchar p_reg_value;
    uchar si_reg_value;
    uchar *data_ptr;
    int write_count;
    hub_ether_t *hub_ether;
    leveltype SR;

    /*
     * Check if write to target register is allowed.
     */
    if ( !(himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
	    (HIMIB_CTRL_WRITE | IMR_CTRL_WRITE)) ) {
        errmsg(&msgsym(WRITEERR, HUB), access->access_code);
        return;
    }

    /*
     * Build value for HIMIB P register.
     * If this access is for a port specific value, then need
     * to include the port number in the P register data.
     */
    if (himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
        HIMIB_CTRL_PORT) {
        /* OR port number into value for P register */
        p_reg_value = himib_access_arr[access->access_code][HIMIB_P_REG] |
            access->phys_port;
    } else {
        /* just use P reg value straight from array */
        p_reg_value = himib_access_arr[access->access_code][HIMIB_P_REG];
    }

    /* turn off level 2 interrupts */
    SR = raise_interrupt_level(LEVEL2_DISABLE);

    /*
     * Select HIMIB port and register by writing to control reg.
     */
    hub_ether = access->device->hub_ether;

    *(hub_ether->ether_himib_control) = p_reg_value;
    *(hub_ether->ether_himib_control) =
        himib_access_arr[access->access_code][HIMIB_R_REG];

    /*
     * If going directly to IMR+
     *   load data for IMR+'s SI register into HIMIB data reg before
     *   writing HIMIB P and R registers.
     * else
     *   write HIMIB data byte(s) to HIMIB data reg.
     */
    if (himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
	  HIMIB_CTRL_IMR_DIRECT) {

      /* must go IMR+ direct */
      /*
       * Build value for to be sent to IMR+ SI register via the HIMIB's
       * D register. If this access is for a port specific value, then
       * need to include the port number in the D reg value.
       */
      if (himib_access_arr[access->access_code][HIMIB_ACCESS_CTRL] &
          IMR_CTRL_PORT) {
          /* OR port number into value for d register */
          si_reg_value = himib_access_arr[access->access_code][IMR_SI] |
              access->phys_port;
      } else {
          /* just use SI reg value straight from array */
          si_reg_value = himib_access_arr[access->access_code][IMR_SI];
      }
      /* write IMR+'s SI value to HIMIB data reg */
      *(hub_ether->ether_himib_data) = si_reg_value;
    } else {
      /* NOT IMR+ direct */
      /*
       * Write right number of data bytes to HIMIB
       */
      for (write_count = 0;
           write_count < himib_access_arr[access->access_code][HIMIB_NUM_BYTES];
	   write_count++) {
	  data_ptr = hub_ether->ether_himib_data;
	  *data_ptr = access->buffer[write_count];
      }

    }

    reset_interrupt_level(SR);

}   /* hub_write_himib */


/*
 * =====================================================================
 * hub_create_ether_device()
 *
 * Description:
 * Create a new ethernet hub device structure and setup the appropriate
 * addresses depending on mother or daughter card.  Initialize the hub
 * device structure.
 *
 * Parameters:
 * daughter - if true, put daughtercard addresses in structure.
 *            else,  use motherboard addresses
 * hub      - pointer to "parent" hub structure
 *
 * Returns:
 * pointer to newly formed hub device data structure
 * =====================================================================
 */
hub_device_t *hub_create_ether_device (boolean daughter, hub_t *hub)
{
    hub_device_t *device;
    hub_ether_t *hub_ether;

    /* malloc device struct */
    device = malloc(sizeof(hub_device_t));
    if (!device) {
	return(NULL);
    }

    /*
     * Initialize all fields in device struct.
     */
    device->next_hub_device = NULL;
    device->parent_hub = hub;
    /*
     * Cabrillo (1E2T1Bhub) only has 14 ports. 
     * (6 on motherboard and 8 on daughterboard)
     */
    if ((cookie->interfaces == INTERFACE_1E2T1Bhub) && (!daughter)) {
        device->num_ports = PORTS_PER_DEVICE_6;
    } else {
        device->num_ports = PORTS_PER_DEVICE_8;
    }

    /* malloc ethernet hub structure */
    device->hub_ether = malloc(sizeof(hub_ether_t));
    if (!(device->hub_ether)) {
        free(device);
        return(NULL);
    }
    hub_ether = device->hub_ether;
    if (daughter) {
        /* use daughterboard addresses */
        hub_ether->ether_himib_control = (uchar *)ADRSPC_GAL_HIMIB_1_CTRL;
        hub_ether->ether_himib_data = (uchar *)ADRSPC_GAL_HIMIB_1_DATA;
    } else {
        /* use motherboard addresses */
        hub_ether->ether_himib_control = (uchar *)ADRSPC_GAL_HIMIB_0_CTRL;
        hub_ether->ether_himib_data = (uchar *)ADRSPC_GAL_HIMIB_0_DATA;
    }

    /* 
     * initialize hub structure to default:
     *  - indicate all ports enabled 
     *  - indicate link test enabled 
     *  - indicate automatic receiver polarity reversal enabled 
     *  - indicate source address change control disabled 
     *  - indicate no source address configured or learned
     */
    device->enabled = ALL_TP_PORTS_MASK;   
    hub_ether->link_test = ALL_TP_PORTS_MASK;
    hub_ether->automatic_polarity = ALL_TP_PORTS_MASK;
    hub_ether->source_addr_change_int = 0;
    hub_ether->source_addr_config = 0;
    hub_ether->source_addr_learn = 0;
    hub_ether->source_addr_snmp_pending = 0;
    hub_ether->source_addr_illegal = 0;

    /* malloc general repeater info structure for counters */
    hub_ether->gen_info = malloc(sizeof(himib_repeater_info_t));
    if (!(hub_ether->gen_info)) {
        free(device->hub_ether);
        free(device);
        return(NULL);
    }

    /* malloc port info structure for counters */
    hub_ether->port_info =
        malloc(sizeof(himib_port_info_t)*(device->num_ports+1));

    if (!(hub_ether->port_info)) {
        free(device->hub_ether->gen_info);
        free(device->hub_ether);
        free(device);
        return(NULL);
    }

    /*
     * Add ports to global ether port count.
     */
    netherhubports += device->num_ports;

    return(device);

}   /* hub_create_ether_device */


/*
 * =====================================================================
 * hub_get_val_uint()
 * 
 * Description:
 * Get requested statistic for target port.  Convert the value into
 * uint format. 
 *
 * Parameters:
 * access -- hub access structure with device, phys_port, and
 *           access_code fields complete (Struct operated upon by
 *           hub_map_port).
 *
 * Returns:
 * uint value requested for the target port
 * =====================================================================
 */
uint hub_get_val_uint (hub_access_t *access)
{
    uint num_bytes, i;
    uint value = 0;

    /* fetch value needed, then convert himib byte format to uint */
    num_bytes = hub_read_himib(access);
    for (i = 0; i < num_bytes; i++) {
        value += access->buffer[i] << (i*8); /* prepend each buffer byte */
    }
    return(value);

}  /* hub_get_val_uint */


/*
 * =====================================================================
 * hub_reset_port_counters()
 *
 * Descriptions:
 * Reset individual port counter values
 *
 * Parameters:
 * phys_port - physical port number of counters to clear 
 * access    - pointer to hub access structure
 *
 * Returns:
 * None
 * =====================================================================
 */ 
void hub_reset_port_counters (int phys_port, hub_access_t *access)
{
    himib_port_info_t *port_info;
  
    /*
     * get current value of each himib counter and store
     * for future use in zeroing "show" counters.
     * (see zero_base_counter() )
     */
    port_info = &(access->device->hub_ether->port_info[phys_port]);

    /* frames_count_base */
    access->access_code = READABLE_FRAMES_CODE;
    port_info->frames_count_base = hub_get_val_uint(access);

    /* octets_count_base */
    access->access_code = READABLE_OCTETS_CODE;
    port_info->octets_count_base = hub_get_val_uint(access);

    /* fcs_count_base */
    access->access_code = FCS_ERR_CODE;
    port_info->fcs_count_base = hub_get_val_uint(access);

    /* alignment_errors_count_base */
    access->access_code = ALIGNMENT_ERR_CODE;
    port_info->align_err_count_base = hub_get_val_uint(access);

    /* frames_too_long_count_base */
    access->access_code = FRAMES_TOO_LONG_CODE;
    port_info->long_count_base = hub_get_val_uint(access);

    /* short_events_count_base */
    access->access_code = SHORT_EVENTS_CODE;
    port_info->short_count_base = hub_get_val_uint(access);

    /* runts_count_base */
    access->access_code = RUNTS_CODE;
    port_info->runts_count_base = hub_get_val_uint(access);

    /* collisions_count_base */
    access->access_code = COLLISIONS_CODE;
    port_info->collisions_count_base = hub_get_val_uint(access);

    /* late_events_count_base */
    access->access_code = LATE_EVENTS_CODE;
    port_info->lates_count_base = hub_get_val_uint(access);

    /* very_long_events_count_base */
    access->access_code = VERY_LONG_EVENTS_CODE;
    port_info->long_events_count_base = hub_get_val_uint(access);

    /* rate_missmatch_count_base */
    access->access_code = DATA_RATE_MISMATCHES_CODE;
    port_info->mismatch_count_base = hub_get_val_uint(access);

    /* auto_partitions_count_base */
    access->access_code = AUTO_PARTITIONS_CODE;
    port_info->partitions_count_base = hub_get_val_uint(access);
}


/*
 * =====================================================================
 * reset_himib_counters()
 *
 * Descriptions:
 * "Reset" counters for ethernet hub ports.
 * For each port in a hub, load device->info.ether_hub->port_info[port] 
 * with current values from himib.  The values will be used in zero
 * correcting the himib "show hub" values.  (himib on-chip counters
 * are not clearable and are non-zero at reset/powerup.)
 *
 * Parameters:
 * hub_type     - hub type to look for 
 * hub_number   - hub number to look for
 * logical_port - if zero, clear counters for all ports in the
 *                specified hub, and clear general repeater counters;
 *                otherwise clear <logical_port> counters.
 *
 * Returns:
 * None
 * =====================================================================
 */
void reset_himib_counters (int hub_type, int hub_number, int logical_port)
{
    hub_t *hub;
    hub_access_t access;
    int start_port;
    int end_port;
    int port;
    int num_ports;
    boolean okay;
    static boolean system_init = TRUE;
    hub_ether_t *hub_ether;

    /* determine which hub */
    hub = hub_get_hub_ptr (hub_type, hub_number);
    if (hub == NULL) {
        errmsg(&msgsym(BADHUB, HUB), hub_type, hub_number);
        return;
    }

    access.hub_number = hub_number;
    access.hub_type = hub_type;

    /* set limits on which ports' counters to clear */
    if (logical_port != 0) {
        /* clear counters only for requested port */
        start_port = logical_port;
        end_port = logical_port;
    } else {
	/* clear counters for all ports */
        start_port = 1;
        end_port = hub_count_ports(hub_type, hub_number);

        /*
         * Go reset the himib repeater general counters.
         *
         * Only needs to be done for one himib device in an ether hub.  
         * Just make sure it's the device used in show_ether_hub_general.
         */

        /*
         * map logical port onto physical port/device combination
         */
        okay = hub_map_port(start_port, &access);
        if (!okay) {
            errmsg(&msgsym(BADUNIT, HUB), start_port);
            return;
	}

        hub_ether = access.device->hub_ether;
        num_ports = access.device->num_ports;

        /* get current general counter values */
	/* total_octets_count_base */
	access.access_code = TOTAL_OCTETS_CODE;
	hub_ether->gen_info->total_octets_count_base =
	    hub_get_val_uint(&access);

	/* tx_collisions_count_base */
	access.access_code = TRANSMIT_COLLISIONS_CODE;
	hub_ether->gen_info->tx_collisions_count_base =
	    hub_get_val_uint(&access);

        /* reset "clear hub" counter */
        hub->resets = 0;

        /* go reset the AUI port counters */
        access.phys_port = HIMIB_AUI_PORT;
        hub_reset_port_counters(num_ports, &access);

        /* 
         * reset the general repeater and AUI port last clear time 
         */
        if (!system_init) { 
          GET_TIMESTAMP(hub->last_clear_time);
          GET_TIMESTAMP(hub_ether->port_info[num_ports].last_clear_time);
        }
    }

    /* 
     * now go reset the counters for the TP port(s)
     */
    for (port = start_port; port <= end_port; port++) {
        /*
         * map logical port onto physical port/device combination
         */
        okay = hub_map_port(port, &access);
        if (!okay) {
            errmsg(&msgsym(BADUNIT, HUB), port);
            return;
        }

        /* go reset the port counters */
        hub_reset_port_counters(access.phys_port, &access);

        /* reset the last clear time of individual port */
        hub_ether = access.device->hub_ether;
        if (!system_init) {
          GET_TIMESTAMP(hub_ether->port_info[access.phys_port].last_clear_time);
        }
    }  /* for each logical port in hub */

    system_init = FALSE;

}   /* reset_himib_counters */


/*
 * =====================================================================
 * zero_base_counter()
 *
 * Description:
 * Zero correct the himib "show hub" values.
 * (See reset_himib_counters.)  Used by the hub show routines.
 *
 * Parameters:
 * raw_number - value read from the hub hardware
 * base       - value read & saved from hub hardware at last
 *              "clearing" of hub counters.
 * 
 * Returns:
 * uint counter value
 * =====================================================================
 */
uint zero_base_counter (uint raw_number, uint base)
{

    if (raw_number >= base) {
        return(raw_number - base);
    } else {
        return(MAX_HIMIB_VAL - base + raw_number + 1);
    }

}   /* zero_base_counter */


/*
 * =====================================================================
 * hub_init_mac_security (cur_hub);
 *
 * Descriptions:
 * Initialize port parameters which related to mac security
 * For each port in a hub, load device->info.ether_hub->port_info[port] 
 *
 * Parameters:
 * cur_hub     - current hub ptr
 
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_init_mac_security(hub_t *hub)
{
    hub_device_t *device;
    int num_port;
    int port;
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;

    /* determine which hub */
    device = hub->hub_device;
    while (device) {

        num_port = device->num_ports;
        hub_ether = device->hub_ether;
    /* 
     * now init port parameters for mac security
     */
       for (port = 0; port < num_port; port++) {
    
            port_info = &(hub_ether->port_info[port]);

            memset (port_info->last_illegal_source_addr, 0, 
                         sizeof(port_info->last_illegal_source_addr));
            memset (port_info->current_illegal_source_addr, 0, 
                         sizeof(port_info->current_illegal_source_addr));
            memset (port_info->learned_addr, 0, 
                         sizeof(port_info->learned_addr));
            port_info->last_mac_security_state = VALID_MAC;
            port_info->current_mac_security_state = VALID_MAC;
            port_info->illegal_addr_trap_acked = NO_ACKED_NO_SENDING; 
            port_info->illegal_addr_trap_enabled = TRAP_ENABLED_FALSE;
            port_info->illegal_addr_first_heard = 0;
            port_info->illegal_addr_last_heard = 0;
            port_info->last_illegal_addr_count = 0; /* last illegal_addr count on this port */
            port_info->illegal_addr_total_count = 0; /* tatal count of all the illegal addr on this port */
            port_info->illegal_addr_trap_count = 0;
            port_info->illegal_addr_countdown_started = FALSE;
            port_info->illegal_addr_sleep_time = 0;
        }
	device = device->next_hub_device;
    }	
}   /* hub_init_mac_security */


/*
 * =====================================================================
 * hub_ether_config_port()
 *
 * Description:
 * Enable/disable requested TP port feature (shutdown, link test,
 * and automatic polarity reversal).
 *
 * Parameters:
 * enable         - boolean: if true, enable the feature for TP port
 *                  (NOTE: for shutdown of hub port, enable == FALSE)
 * access         - pointer to hub access structure which specifies hub 
 *                  number, type, device, and physical port to [en/dis]able.
 * feature_config - HUB_SHUTDOWN, HUB_LINK_TEST, HUB_AUTOMATIC_POLARITY
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_config_port (boolean enable, hub_access_t *access, 
                            uint feature_config)
{
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;
    uchar port_mask;

    hub_ether = access->device->hub_ether;
    port_info = &(hub_ether->port_info[access->phys_port]);
    port_mask = GET_PORT_MASK(access->phys_port);

    /* depending upon sense of command,
     *   set/clear enabled bit in device struct
     *   set access code for HIMIB
     *   and send command to HIMIB
     */
    switch (feature_config) {
    case HUB_SHUTDOWN:
        /* NOTE:
         * The following is broken for case of AUI port, but the user doesn't
         * have the facility to shutdown/enable the AUI port.  So far, the
         * code has no need to (no)shut the AUI port.
         */
        
        /* 
         * for shutdown, the enable flag would be TRUE to shutdown
         * and FALSE, to enable the TP port.
         */ 
        if (enable) {
            access->device->enabled &= ~port_mask;
            access->access_code = TP_PORT_DISABLE_CODE;
        } else {
            /*
             * reprogram the source address register before enabling port
             * if source address control has been enabled.
             */
            if (hub_ether->source_addr_change_int & port_mask) {
                if ((hub_ether->source_addr_config & port_mask) ||
                    (hub_ether->source_addr_learn & port_mask)) {
                    ieee_copy(port_info->source_addr, access->buffer);
                    access->access_code = LAST_SOURCE_ADDR_CODE;
                    hub_write_himib(access);
                }
            }
            /*
             * now re-enable the port
             */
            access->device->enabled |= port_mask;
            access->access_code = TP_PORT_ENABLE_CODE;
        }
        break;
    case HUB_LINK_TEST: 
        if (enable) {
            hub_ether->link_test |= port_mask;
            access->access_code = TP_PORT_ENABLE_LINK_TEST_FUNCTION_CODE;
        } else {
            hub_ether->link_test &= ~port_mask;
            access->access_code = TP_PORT_DISABLE_LINK_TEST_FUNCTION_CODE;
        }
        break;
    case HUB_AUTOMATIC_POLARITY:
        if (enable) {
            hub_ether->automatic_polarity |= port_mask;
            access->access_code = TP_PORT_ENABLE_RXR_POLARITY_REVERS_CODE;
        } else {
            hub_ether->automatic_polarity &= ~port_mask;
            access->access_code = TP_PORT_DISABLE_RXR_POLARITY_REVERS_CODE;
        }
        break;
    default:
        /* should not come here */
        return;
    }
    
    hub_write_himib(access);
}   /* hub_ether_config_port */


/*
 * =====================================================================
 * hub_ether_setup_int()
 *
 * Description:
 * Handle setup for a hub.
 *
 * Parameters:
 * sp - pointer to setup structure
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_setup_int (setup_struct *sp)
{
    int hub_number;
    int num_ports;
    int port;
    boolean yes;
    hwidbtype *hwidb;

    hwidb = sp->idb->hwptr;

    /* 
     * check if this is an ethernet interface  
     * and whether there's a hub connected 
     */
    if (hwidb->type == IDBTYPE_LANCE) {
        if (hwidb->hub == NULL) {
            /* no hub connected to this ethernet interface */
            return;
        }
    } else {
        /* this is not an ethernet interface */
        return; 
    }

    /* 
     * retrieve hub number and figure out how many ports this hub has 
     */
    hub_number = hwidb->hub->hub_number;
    num_ports = hub_count_ports(HUB_ETHER, hub_number);

    if (yes_or_no("  Enable all hub ports on this interface", TRUE, TRUE)) {
        /*
         * For each port to be enabled, map that port onto a
         * physical port/device pair, then call to enable/disable.
         * As default, link test function and auto-polarity are enabled.
         */
        for (port = 1; port <= num_ports; port++) {
	    setup_printf(sp,
			 "\n!"
			 "\nhub ether %d %d"
			 "\nlink-test"
			 "\nauto-polarity"
			 "\nno shutdown", 
			 hub_number, port);
        }
    } else if (yes_or_no("  Enable some hub ports on this interface", TRUE, TRUE)) {
        for (port = 1; port <= num_ports; port++) {
            printf("\n  Configuring hub port %d:", port);
            yes = yes_or_no("    Is this port in use", TRUE, TRUE);
	    setup_printf(sp, 
			 "\n!"
			 "\nhub ether %d %d"
			 "\nlink-test"
			 "\nauto-polarity",
			 hub_number, port);
           if (yes) {
	       setup_printf(sp, "\nno shutdown");
           } else {
	       setup_printf(sp, "\nshutdown");
	   } 
        }
    } else {
        for (port = 1; port <= num_ports; port++) {
	    setup_printf(sp,
			 "\n!"
			 "\nhub ether %d %d"
			 "\nlink-test"
			 "\nauto-polarity"
			 "\nshutdown",
			 hub_number, port);
	}
    }
}   /* hub_ether_setup_int */


/*
 * =====================================================================
 * hub_ether_reset()
 *
 * Description:
 * Reset the ethernet hub by toggling the reset bit in hub misc register
 * and (en/dis)able features according to config.
 *
 * Parameters:
 * access - pointer to hub access structure
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_reset (hub_access_t *access)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    uint port;
    ushort *hub_misc_reg;
    hub_ether_t *hub_ether;
    uchar port_mask = 0;
    ulong delay_4_usec;
    int ix;

    /*
     * check to make sure there is an ether hub  
     */
    cur_hub = hub_get_hub_ptr(access->hub_type, access->hub_number);
    if (!cur_hub) {
        errmsg(&msgsym(BADHUB, HUB), access->hub_type, access->hub_number);
        return;
    }

    hub_misc_reg = (ushort *)ADRSPC_GAL_HUB_MISC_REG;

    /* 
     * Reset the ether hub by setting reset bit to high for 4 usec.
     * Update those snmp fields that are relevant to reset so that
     * they indicate reset in process.  (Don't forget to update them
     * again at end of hub reset.)
     */
    *hub_misc_reg |= ETHER_HUB_RESET;
    cur_hub->management->oper_status = D_rptrOperStatus_other;
    GET_TIMESTAMP(cur_hub->management->grp_last_oper_stat_change);
    cur_hub->management->health_text = HEALTHTEXT_INITIALIZING;
    delay_4_usec = timer_calibrate(4, FALSE);
    WASTETIME(delay_4_usec);

    /* now, set the reset bit to low to take it out of reset */
    *hub_misc_reg &= ~ETHER_HUB_RESET;

    /*
     * Indicate all current HIMIB LSA values are currently invalid.
     * (HIMIB will report FF.FF.FF.FF as LSA until the first packet
     * comes along.  We use LSA_correct to know whether to believe
     * the HIMIB reported LSA.)  Can't do this in device loop below,
     * since it must be done before enabling interrupts (to prevent
     * race condition of LSA_correct against first packet arrival).
     *
     * Indicate no source address mismatched. 
     * Indicate no source address learned.
     * Indicate no illegal source address yet.
     */
    cur_device = cur_hub->hub_device;
    while (cur_device != NULL) {
	hub_ether = cur_device->hub_ether;
	hub_ether->LSA_correct = 0;
        hub_ether->source_addr_mismatch = 0;
        hub_ether->source_addr_learn = 0;
        hub_ether->source_addr_illegal = 0;
        cur_device = cur_device->next_hub_device;
    }

    /*
     * need to enable all interrupts since resetting
     * the hub would disable all interrupts and lose our
     * interrupt enable settings.
     */
    hub_ether_enable_interrupts(cur_hub);

    /*
     * Traverse all the hub devices' ports and check to see which
     * hub port, test-link, auto-partition needs to be enabled since
     * resetting the hub would reset them to IMR+ default settings.
     */
    cur_device = cur_hub->hub_device;
    while (cur_device != NULL) {
	hub_ether = cur_device->hub_ether;
	for (port = 0; port < cur_device->num_ports; port++) {
            /*
             * setup access structure
             */
            access->device = cur_device;
            access->phys_port = port;

            /*
             * initalize the LSA register of himib to FF's
             */
            for (ix = 0; ix < HW_DATA_BUFFER; ix++) {
                access->buffer[ix] = 0xff;
            }
            access->access_code = LAST_SOURCE_ADDR_CODE;
            hub_write_himib(access);

            /* retrieve port mask */
            port_mask = GET_PORT_MASK(port);

	    /* (en/dis)abled port according to config */
	    hub_ether_config_port(!(cur_device->enabled & port_mask), 
				  access, HUB_SHUTDOWN);   

	    /* (en/dis)abled link-test according to config */
	    hub_ether_config_port(hub_ether->link_test & port_mask, 
				  access, HUB_LINK_TEST);   

	    /* (en/dis)abled auto-polarity according to config */
	    hub_ether_config_port(hub_ether->automatic_polarity & port_mask, 
				  access, HUB_AUTOMATIC_POLARITY);   

	}   /* for each port in device */
	cur_device = cur_device->next_hub_device;
    }    /* for each device in hub */

    /* increment hub reset counter */
    cur_hub->resets++;

    /* Update those snmp fields that are relevant to reset so that
     * they indicate reset no longer in process.
     */
    cur_hub->management->oper_status = D_rptrOperStatus_ok;
    GET_TIMESTAMP(cur_hub->management->grp_last_oper_stat_change);
    cur_hub->management->health_text = HEALTHTEXT_OPERATIONAL;

}  /* hub_ether_reset */


/*
 * =====================================================================
 * hub_ether_enable_interrupts()
 *
 * Description:
 * Enable ether hub interrupts.
 *
 * Parameters:
 * cur_hub - pointer to hub structure  
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_enable_interrupts (hub_t *cur_hub)
{
    hub_access_t access;
    hub_device_t *cur_device;

    cur_device = cur_hub->hub_device;
    access.hub_number = cur_hub->hub_number;
    access.hub_type = cur_hub->hub_type;

    while (cur_device != NULL) {
        access.device = cur_device;

        /* enable interrupts */
        access.access_code = CONFIG_REG_CODE;
        access.buffer[0] = HIMIB_ENABLE_INT | HIMIB_IF_ERR_INT;
        hub_write_himib(&access);

        /* turn on Partition status change */
        access.access_code = TP_PART_CHANGE_INT_ENABLE_CODE;
        access.buffer[0] = ALL_TP_PORTS_MASK;
        hub_write_himib(&access);

        access.access_code = AUI_PART_CHANGE_INT_ENABLE_CODE;
        access.buffer[0] = AUI_PORT_MASK;
        hub_write_himib(&access);

        /* turn on link status change */
        access.access_code = TP_LINK_STATUS_CHANGE_INT_ENABLE_CODE;
        access.buffer[0] = ALL_TP_PORTS_MASK;
        hub_write_himib(&access);

        /* turn on source address change */
        /* 
         * Enable it so we can tell when packets have been received on a port.
         * If user has not enabled source address control, it will be disabled
         * by the interrupt handler.
         */
        access.access_code = TP_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE;
        access.buffer[0] = ALL_TP_PORTS_MASK;
        hub_write_himib(&access);

        access.access_code = AUI_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE;
        access.buffer[0] = AUI_PORT_MASK;
        hub_write_himib(&access);

        cur_device = cur_device->next_hub_device;
    }
}  /* hub_ether_enable_interrupts */


/*
 * =====================================================================
 * hub_ether_interrupt()
 * 
 * Description:
 * This interrupt handler for ether hub checks the status register to 
 * determine if the interrupt is caused by its (mother or daughter)
 * devices.  If so, go query the different himib registers to determine
 * the cause of the interrupt and go service it.
 *
 * Note: This isn't very efficient for multiple hubs since it traverses
 * the hub list to determine who causes the interrupt.  This works now
 * since there is only one hub.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_interrupt (void)
{
    hub_access_t access;
    hub_t *cur_hub;
    hub_device_t *cur_device;
    uchar status_reg;
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;
    boolean mom;
    int port = 0;
    uchar port_mask;
    uchar sac_status;

    cur_hub = hub_list;
    while (cur_hub != NULL) {
      if (cur_hub->hub_type == HUB_ETHER) {
        /* 
         * first, check motherboard device
         */
        mom = TRUE;
        /* 
         * check to see if this device causes the interrupt
         * if so, check what causes the interrupt and service it
         */
        cur_device = cur_hub->hub_device;
        while (cur_device != NULL) {
	    hub_ether = cur_device->hub_ether;
	    status_reg = *(hub_ether->ether_himib_control);
	    if (status_reg & (HIMIB_ENABLE_INT | HIMIB_IF_ERR_INT)) {
		access.hub_type = cur_hub->hub_type;
		access.hub_number = cur_hub->hub_number;
		access.device = cur_device;

		/*
		 * Check for TPSourceAddressChange interrupt
		 */
		access.access_code = TP_SOURCE_ADDR_CHANGE_CODE;
		hub_read_himib(&access);
                sac_status = access.buffer[0]; 
		if (sac_status) {
                    /*
                     * for each port interrupting
                     * - if user has configured source address control, disable
                     *   the port that has the mismatched source address.
                     * - indicate that HIMIB LSA data is now correct.  (HIMIB
                     *   reports incorrect LSA for a port until a packet has
                     *   been received on that port.)
                     */
                    for (port = 0; port < cur_device->num_ports; port++) {
                      port_info = &(hub_ether->port_info[port]);
                      port_mask = GET_PORT_MASK(port);
                      /* if this port interrupted */
                      if (sac_status & port_mask) {
                          if (hub_ether->source_addr_change_int & port_mask) {
                              /*
                               * if a source address was configured for port or
                               * that we have already learned a source-address,
                               * the port needs to be disable and mac security
			       * should be excuted since it doesn't
                               * match either the configured or the learned
                               * source address.
                               */
                              if ((hub_ether->source_addr_config & port_mask)||
                                  (hub_ether->source_addr_learn & port_mask)) {

                                  port_info->last_mac_security_state = 
                                            port_info->current_mac_security_state;
                                  port_info->current_mac_security_state = INVALID_MAC; 
                                  memcpy (port_info->last_illegal_source_addr, 
                                            port_info->current_illegal_source_addr,
                                            sizeof (port_info->last_illegal_source_addr));
                                  access.phys_port = port;
                                  access.access_code = LAST_SOURCE_ADDR_CODE;
                                  hub_read_himib(&access);
                                  ieee_copy(access.buffer, 
                                            port_info->current_illegal_source_addr);
				  
				  if (cur_hub->management->illegal_addr_trap_algorithm
				        == ONCE && port_info->illegal_addr_trap_count >= 1) {
				      if (!(port_info->illegal_addr_trap_acked == ACKED)) {
				          port_info->illegal_addr_trap_acked = NO_ACKED_NO_SENDING;
			              }
				  }
				  else {
				      if (port_info->illegal_addr_trap_acked == NO_ACKED_NO_SENDING) {
					  port_info->illegal_addr_trap_acked = NO_ACKED_SENDING;
				      }	  
				  }    
                                  hub_mac_security(port_info);

                                  hub_ether->source_addr_illegal |= port_mask;
                                  access.access_code = TP_PORT_DISABLE_CODE;
                                  hub_write_himib(&access);
                                  hub_ether->source_addr_mismatch |= port_mask;
                              } else {
                                  /*
                                   * save a copy of the learned LSA 
                                   * since mac-address wasn't configured
                                   */
                                  access.phys_port = port;
                                  access.access_code = LAST_SOURCE_ADDR_CODE;
                                  hub_read_himib(&access);
                                  ieee_copy(access.buffer,
                                            port_info->source_addr);
                                  hub_ether->source_addr_learn |= port_mask;
                              }
                          }

                          /*
                           * print out debug messages only if source
                           * address control is configured or for first
                           * packet received on port.
                           */
                          if (hub_debug &&
                              ((hub_ether->source_addr_change_int & port_mask)
                                || !(hub_ether->LSA_correct & port_mask))) {
                              buginf("\n%%HUB-ETHER-%d_%s: TP source address changed for port %d",
                                 access.hub_number, mom?"MOM":"DGTR", port+1);
                          }

                          /*
                           * indicate that we received a valid packet
                           * on port and that the LSA register contains
                           * a valid source address.
                           */
                          hub_ether->LSA_correct |= port_mask;

                          /*
                           * if user has not configured source address control,
                           * disable source addr change interrupts for port
                           * which just interrupted.
                           */
                          if (!(hub_ether->source_addr_change_int & port_mask)) {
                              /* read current SAC int enable */
                              access.access_code = 
                                  TP_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE;
		              hub_read_himib(&access);
                              /* reset the bit */
		              access.buffer[0] &= ~port_mask; 
		              hub_write_himib(&access);
                          } 
                      }
                    }
		}
		
                /*
                 * Check for AUISourceAddressChange interrupt
                 */
		access.access_code = AUI_SOURCE_ADDR_CHANGE_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: AUI source address changed = %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}

		/*
		 * Check for TPPartitionStatusChange interrupt
		 */
		access.access_code = TP_PARTITION_STATUS_CHANGE_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: TP Partition status changed: %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}
		
		/*
		 * Check for AUIPartitionStatusChange interrupt
		 */
		access.access_code = AUI_PARTITION_STATUS_CHANGE_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: AUI Partition status changed = %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}
		
		/*
		 * Check for TPLinkStatusChange interrupt
		 */
		access.access_code = TP_LINK_STATUS_CHANGE_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: TP Link status changed = %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}

		/*
		 * Check for TPSourceAddressMatchStatus interrupt
		 */
		access.access_code = TP_SOURCE_ADDR_MATCH_STATUS_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: TP source address match status = %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}
			
		/*
		 * Check for AUISourceAddressMatchStatus interrupt
		 */
		access.access_code = AUI_SOURCE_ADDR_MATCH_STATUS_CODE;
		hub_read_himib(&access);
		if (hub_debug && access.buffer[0]) {
		    buginf("\n%%HUB-ETHER-%d_%s: AUI source address match status = %#x",
			access.hub_number, mom?"MOM":"DGTR", access.buffer[0]);
		}
	    }
	    /*
	     * check daughterboard device, if one exists
	     */
	    cur_device = cur_device->next_hub_device;
	    mom = FALSE;
        }
    }
    cur_hub = cur_hub->next_hub;
  }
} /* hub_ether_interrupt */


/*
 * =====================================================================
 * hub_mac_security( port_info)
 *
 * Description:
 * Perform MAC security for SNMP user
 *
 * Parameters:
 * None.
 *
 * Returns:
 * 
 * =====================================================================
 */
static void hub_mac_security (himib_port_info_t *port_info)
{
    if (port_info->current_mac_security_state == VALID_MAC) {
        if (port_info->last_mac_security_state == INVALID_MAC) {
            port_info->last_mac_security_state = VALID_MAC;
            port_info->illegal_addr_trap_acked = NO_ACKED_NO_SENDING;
        }
    }
 
    if (port_info->current_mac_security_state == INVALID_MAC &&
        port_info->last_mac_security_state == INVALID_MAC) {
        if (memcmp(port_info->current_illegal_source_addr,
                port_info->last_illegal_source_addr, 
                sizeof(port_info->current_illegal_source_addr)) == 0) {
            port_info->illegal_addr_last_heard = GetTimeNow();
            port_info->last_illegal_addr_count++;
            port_info->illegal_addr_total_count++;
        } else { /*current_illegal_source_addr is not the same as  last_illegal_source_addr */  
            port_info->illegal_addr_first_heard = GetTimeNow();
            port_info->illegal_addr_last_heard = GetTimeNow();
            port_info->last_illegal_addr_count = 1;
            port_info->illegal_addr_total_count++; 
            port_info->illegal_addr_trap_count = 0;
          }
    }

    if (port_info->current_mac_security_state == INVALID_MAC &&
        port_info->last_mac_security_state == VALID_MAC) {
            port_info->illegal_addr_first_heard = GetTimeNow();
            port_info->illegal_addr_last_heard = GetTimeNow();
            port_info->last_illegal_addr_count = 1;
            port_info->illegal_addr_total_count++; 
            port_info->illegal_addr_trap_count = 0; 
            port_info->last_mac_security_state = INVALID_MAC;
    }
}


/*
 * =====================================================================
 * hub_illegal_addr_trap_process_creation ()
 *
 * Description:
 * Crate a trap process 
 *
 * Parameters:
 * none
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_illegal_addr_trap_process_creation (void)

{
 /*
  * create a trap process to generate illegal src address trap
 */
  if (!(trap_pid)) {
      trap_pid = process_create(hub_illegal_addr_trap_process,
				"Illegal Addr Trap Process",
				LARGE_STACK,
				PRIO_NORMAL);
  }
  if (trap_pid == NO_PROCESS)
        trap_pid = 0;

}   /* hub_illegal_addr_trap_process_creation */

 
/*
 * =====================================================================
 * hub_illegal_addr_trap_process ()
 *
 * Description:
 * Background process to send illegal addr trap 
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static forktype hub_illegal_addr_trap_process (void)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;
    int port = 0;
    uchar port_mask;
    int portIndex = 0;
    boolean mom;
    int mom_num_ports = 0;
    hub_access_t access;
    
    while (TRUE) {
        edisms((blockproc *)hub_illegal_addr_trap_ctrlBLOCK, 0);

        process_sleep_for( ONEMIN );
        cur_hub = hub_list;
        while (cur_hub != NULL) {
            if (cur_hub->hub_type == HUB_ETHER) {
                cur_device = cur_hub->hub_device;
		mom = TRUE; /* first is the mother board */
                while (cur_device != NULL) {
	            hub_ether = cur_device->hub_ether;
                    for (port = 0; port < cur_device->num_ports; port++) {
             	        port_info = &(cur_device->hub_ether->port_info[port]);
			port_mask =  GET_PORT_MASK(port); 
               	        if ((port_info->illegal_addr_trap_enabled == TRAP_ENABLED_TRUE) &&
			      (port_info->illegal_addr_trap_acked == NO_ACKED_SENDING) &&
                              (hub_ether->source_addr_illegal & port_mask) && 
                              (hub_ether->source_addr_change_int & port_mask)) {
                            if (cur_hub->management->illegal_addr_trap_algorithm == ONCE &&
                                   port_info->illegal_addr_trap_count >= 1 ) {
                                port_info->illegal_addr_trap_acked = NO_ACKED_NO_SENDING;
                            }  
                            if ((cur_hub->management->illegal_addr_trap_algorithm == ONCE &&
                                  port_info->illegal_addr_trap_count == 0 ) ||
			          (cur_hub->management->illegal_addr_trap_algorithm == DECAY)) {
				if ( mom ) {
				    portIndex = port + 1;
      				} else {/* daughter board's port */
					/*
                                         * Cabrillo (1E2T1Bhub) only has 14 ports. 
                                         * (6 on motherboard and 8 on daughterboard)
                                         */
                                    if (cookie->interfaces == INTERFACE_1E2T1Bhub) {
                                        mom_num_ports = PORTS_PER_DEVICE_6;
                                    } else {
                                        mom_num_ports = PORTS_PER_DEVICE_8;
                                    }
				    portIndex = port + mom_num_ports + 1;
				}     
			        hub_send_illegal_addr_trap(port_info, portIndex);
			    }
                        }
		        /*Read from himib the learned src addr. */
		        access.hub_type = cur_hub->hub_type;
		        access.hub_number = cur_hub->hub_number;
		        access.device = cur_device;
                        access.phys_port = port;
                        access.access_code = LAST_SOURCE_ADDR_CODE;
                        hub_read_himib(&access);
                        ieee_copy(access.buffer, port_info->learned_addr);
	            }
		    cur_device = cur_device->next_hub_device;
		    mom = FALSE; /* next round is the daughter board */
		}    
            }
	    cur_hub = cur_hub->next_hub;
	}
    }	
} /* hub_illegal_addr_trap_process */

 
/*
 * =====================================================================
 * hub_send_illegal_addr_trap (*port_info, portIndex)
 *
 * Description:
 * Send this trap
  *
 * Parameters:
 * port_info
 * portIndex Index for accessing MIB port table
 *
 * Returns:
 * none
 * =====================================================================
 */
    
static void hub_send_illegal_addr_trap (himib_port_info_t *port_info, int portIndex)
{
    int shift_bit = 0;

    if ( ! port_info->illegal_addr_countdown_started ) {
        if (port_info->illegal_addr_trap_count == 0) {
	    port_info->illegal_addr_sleep_time = 0;
        } else {
	      shift_bit = port_info->illegal_addr_trap_count - 1; 
              port_info->illegal_addr_sleep_time = min( 32, 1 << shift_bit) - 1;
	                                          /* process already slept one minute */
	      port_info->illegal_addr_countdown_started = TRUE;
        }
    } else {
 	if (port_info->illegal_addr_sleep_time == 0) {
	    port_info->illegal_addr_countdown_started = FALSE;
	}
    }	
    if (port_info->illegal_addr_sleep_time == 0) {
        if (port_info->illegal_addr_trap_enabled == TRAP_ENABLED_TRUE &&
	    port_info->illegal_addr_trap_acked == NO_ACKED_SENDING ) {
                reg_invoke_generate_illegal_addr_trap(RPTR_ILLEGAL_ADDR_TRAPNUM, portIndex);
                port_info->illegal_addr_trap_count++;
		port_info->illegal_addr_countdown_started = FALSE;
	}
    } else {
          port_info->illegal_addr_sleep_time--;
    }
}

 
/*
 * =====================================================================
 * hub_illegal_addr_trap_ctrlBLOCK ()
 *
 * Description:
 * Block hub illegal addr trap process if sending the trap is not required.
 *
 * Parameters:
 * None.
 *
 * Returns:
 * boolean: TRUE to block process
 * =====================================================================
 */
static boolean hub_illegal_addr_trap_ctrlBLOCK (void)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;
    int port = 0;
    uchar port_mask;

    cur_hub = hub_list;
    while (cur_hub != NULL) {
        if (cur_hub->hub_type == HUB_ETHER) {
            cur_device = cur_hub->hub_device;
            while (cur_device != NULL) {

	        hub_ether = cur_device->hub_ether;
		for (port = 0; port < cur_device->num_ports; port++) {
                    port_info = &(cur_device->hub_ether->port_info[port]);
		    port_mask =  GET_PORT_MASK(port);
		        
		    if ((ieee_equal(port_info->current_illegal_source_addr,
				    port_info->source_addr)) ||
			(ieee_equal(port_info->learned_addr, port_info->source_addr))) {
			      port_info->illegal_addr_trap_acked = NO_ACKED_NO_SENDING;
		    } else {
                        if ((port_info->illegal_addr_trap_enabled == TRAP_ENABLED_TRUE) &&
			    (port_info->illegal_addr_trap_acked == NO_ACKED_SENDING) &&
                            (hub_ether->source_addr_illegal & port_mask) &&
			    (hub_ether->source_addr_change_int & port_mask)) {
                                return(FALSE);
              		} 
                    }			   
                }
                cur_device = cur_device->next_hub_device;
            }
        }
        cur_hub = cur_hub->next_hub;
    }
    return(TRUE);
}


/*
 * =====================================================================
 * hub_ether_src_addr_ctrlBLOCK ()
 *
 * Description:
 * Block hub source address process if no work is available by checking
 * whether there is a source address mismatch for all ether hub devices.
 *
 * Parameters:
 * None.
 * =====================================================================
 */
boolean hub_ether_src_addr_ctrlBLOCK (void)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;

    cur_hub = hub_list;
    while (cur_hub != NULL) {
        if (cur_hub->hub_type == HUB_ETHER) {
            cur_device = cur_hub->hub_device;
            while (cur_device != NULL) {
                if (cur_device->hub_ether->source_addr_mismatch) {
                    return(FALSE);
                }
                cur_device = cur_device->next_hub_device;
            }
        }
        cur_hub = cur_hub->next_hub;
    }
    return(TRUE);
}


/*
 * =====================================================================
 * hub_ether_src_addr_ctrl_process ()
 *
 * Description:
 * Background process to re-enable a port that was disabled previously
 * by a source address mismatch.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
forktype hub_ether_src_addr_ctrl_process(void)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    hub_ether_t *hub_ether;
    hub_access_t access;

    int port = 0;
    uchar port_mask = 0;

    while (TRUE) {
        edisms((blockproc *)hub_ether_src_addr_ctrlBLOCK, 0);
        /*
         * ok, there is a port disabled due to source address mismatch.
         * Wait for one minute and re-enable the disabled port.
         */
        process_sleep_for(ONEMIN);
        cur_hub = hub_list;
        while (cur_hub != NULL) {
            if (cur_hub->hub_type == HUB_ETHER) {
                cur_device = cur_hub->hub_device;
                while (cur_device != NULL) {
                    hub_ether = cur_device->hub_ether;

                    if (hub_ether->source_addr_mismatch) {
                        /*
                         * re-enable the port that has mismatch
                         */
                        access.device = cur_device;
                        for (port = 0; port < cur_device->num_ports; port++) {
                            port_mask = GET_PORT_MASK(port);
                            if (hub_ether->source_addr_mismatch & port_mask) {
                                /* 
                                 * reset the mismatch bit, and go re-enable
                                 * the port if configured to be enabled.
                                 */
                                hub_ether->source_addr_mismatch &= ~port_mask;
                                if (cur_device->enabled & port_mask) {
                                    /*
                                     * retrieve the source address allowed, and
                                     * write it to himib LSA register.
                                     */
                                    ieee_copy(hub_ether->port_info[port].source_addr,
                                              access.buffer);
                                    access.phys_port = port;
                                    access.access_code = LAST_SOURCE_ADDR_CODE;
                                    hub_write_himib(&access);
                                    /*
                                     * now re-enable the port
                                     */
                                    access.access_code = TP_PORT_ENABLE_CODE;
                                    hub_write_himib(&access);
				}    
                            }
                        }
                    }
                    cur_device = cur_device->next_hub_device;
                }
            }
            cur_hub = cur_hub->next_hub;
        }
    }
}


/*
 * =====================================================================
 * hub_ether_src_addr_ctrl ()
 *
 * Description:
 * Enable/disable source address control feature for an ether hub port.
 *
 * If the feature is to be enable and a user enters a mac-address,
 * the port would be enabled only when a device with the config
 * mac-address is connected to the port.  Otherwise, if no mac-address
 * was entered, the hub will learn the mac-address of the first packet
 * it receives and only enable the port for the device with the learned
 * mac-address.
 *
 * Parameters:
 * enable         - boolean: if true, enable the feature for TP port
 * access         - pointer to hub access structure which specifies hub
 *                  number, type, device, physical port to [en/dis]able,
 *                  and the mac-address if user entered one.
 * macaddr_config - boolean: if true, user has entered a mac-address
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_ether_src_addr_ctrl (boolean enable, hub_access_t *access,
                       boolean macaddr_config)
{
    hub_ether_t *hub_ether;
    himib_port_info_t *port_info;
    uchar port_mask;

    hub_ether = access->device->hub_ether;
    port_mask = GET_PORT_MASK(access->phys_port);

    if (enable) {
        /*
         * toggle bit that say we want source address
         * changes to cause interrupts
         */
        hub_ether->source_addr_change_int |= port_mask;
        /*
         * if a mac-address was configured, then set the
         * appropriate bit in hub device structure, save
         * a copy of the mac-address, and write it to the
         * himib last source address register.
         */
        port_info = &(hub_ether->port_info[access->phys_port]);
        if (macaddr_config) {
            hub_ether->source_addr_config |= port_mask;
            ieee_copy(access->buffer, port_info->source_addr);
            access->access_code = LAST_SOURCE_ADDR_CODE;
            hub_write_himib(access);
        } else {
            /*
             * else, no source address configured, use the current
             * source address in the LSA register in himib if it
             * is valid.
             */
            hub_ether->source_addr_config &= ~port_mask;
            if (hub_ether->LSA_correct & port_mask) {
                hub_ether->source_addr_learn |= port_mask;
                access->access_code = LAST_SOURCE_ADDR_CODE;
                hub_read_himib(access);
                ieee_copy(access->buffer, port_info->source_addr);
            }
        }
       
        /*
         * writing new source address to himib by above code
         * will create a source address change interrupt;
         * clear out pending source address change interrupt
         */
        access->access_code = TP_SOURCE_ADDR_CHANGE_CODE;
        hub_read_himib(access);

        /*
         * This could be a reconfig of the source-address command so
         * check if device is currently disabled because of source
         * address mismatch.  If so, re-enable the port if it's
         * configured to be enabled so that we don't have to wait
         * one minute.
         */
        if ((hub_ether->source_addr_mismatch & port_mask) &&
            (access->device->enabled & port_mask)) {
            hub_ether->source_addr_mismatch &= ~port_mask;
            hub_ether_config_port(FALSE, access, HUB_SHUTDOWN);
        }

        /*
         * retrieve the current status of Source Address
         * Change Interrupt Enable Register
         */
        access->access_code = TP_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE;
        hub_read_himib(access);
        /*
         * enable the appropriate port of the Source Address Change
         * Interrupt Enable Register and write it back to himib
         */
        access->buffer[0] |= port_mask;
        hub_write_himib(access);

        /*
         * fork off a background process to see if a port should be
         * enable after a period of time due to source address mismatch
         */
        if (!(source_addr_pid)) {
            source_addr_pid =
                cfork((forkproc *)hub_ether_src_addr_ctrl_process, 0L, 0, "Hub Background", 0);
        }
    } else {
        /*
         * turn off bits that say we want source address
         * changes to cause interrupts and that a mac-address
         * was configured or learned for the port.
         */
        hub_ether->source_addr_change_int &= ~port_mask;
        hub_ether->source_addr_config &= ~port_mask;
        hub_ether->source_addr_learn &= ~port_mask;

        /*
         * retrieve the current status of Source Address
         * Change Interrupt Enable Register
         */
        access->access_code = TP_SOURCE_ADDR_CHANGE_INT_ENABLE_CODE;
        hub_read_himib(access);
        /*
         * turn off the appropriate bit of the Source Address Change
         * Interrupt Enable Register and write it back to himib
         */
        access->buffer[0] &= ~port_mask;
        hub_write_himib(access);
        /*
         * Source address control has been disabled for this port so 
         * check if device is currently disabled because of source
         * address mismatch.  If so, re-enable the port if it's
         * configured to be enable so that we don't have to wait
         * one minute.
         */
        if ((hub_ether->source_addr_mismatch & port_mask) &&
            (access->device->enabled & port_mask)) {
            hub_ether->source_addr_mismatch &= ~port_mask;
            hub_ether_config_port(FALSE, access, HUB_SHUTDOWN);
        }
    }

    /*
     * source address control has either been enabled or disabled
     * via config terminal or snmp set, so reset source address 
     * illegal and snmp pending flags for port.
     */
    hub_ether->source_addr_illegal &= ~port_mask;
    hub_ether->source_addr_snmp_pending &= ~port_mask;

}   /* hub_ether_src_addr_ctrl */




























