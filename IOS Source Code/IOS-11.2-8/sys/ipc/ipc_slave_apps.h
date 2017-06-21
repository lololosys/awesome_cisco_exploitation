/* $Id: ipc_slave_apps.h,v 3.1.2.1 1996/06/10 04:52:04 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_slave_apps.h,v $
 *------------------------------------------------------------------
 * Header file for VIP IPC slave applications.
 *
 * June 1996, Don Lobete
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_slave_apps.h,v $
 * Revision 3.1.2.1  1996/06/10  04:52:04  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.1  1996/06/08  02:32:01  dlobete
 * Placeholder for California_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Defines/declarations required for Slave utilities.
 */
/*
 * Port names - slave utilities.
 */
#define IPC_SLAVECORE_PORT0_NAME        "Slot0: Slave Core"
#define IPC_SLAVECORE_PORT1_NAME        "Slot1: Slave Core"
#define IPC_SLAVECORE_PORT2_NAME        "Slot2: Slave Core"
#define IPC_SLAVECORE_PORT3_NAME        "Slot3: Slave Core"
#define IPC_SLAVECORE_PORT4_NAME        "Slot4: Slave Core"
#define IPC_SLAVECORE_PORT5_NAME        "Slot5: Slave Core"
#define IPC_SLAVECORE_PORT6_NAME        "Slot6: Slave Core"
#define IPC_SLAVECORE_PORT7_NAME        "Slot7: Slave Core"
#define IPC_SLAVECORE_PORT8_NAME        "Slot8: Slave Core"
#define IPC_SLAVECORE_PORT9_NAME	"Slot9: Slave Core"
#define IPC_SLAVECORE_PORT10_NAME       "Slot10: Slave Core"
#define IPC_SLAVECORE_PORT11_NAME       "Slot11: Slave Core"
#define IPC_SLAVECORE_PORT12_NAME       "Slot12: Slave Core"
 
#define IPC_SLAVELOG_PORT0_NAME         "Slot0: Slave Log"
#define IPC_SLAVELOG_PORT1_NAME         "Slot1: Slave Log"
#define IPC_SLAVELOG_PORT2_NAME         "Slot2: Slave Log"
#define IPC_SLAVELOG_PORT3_NAME         "Slot3: Slave Log"
#define IPC_SLAVELOG_PORT4_NAME         "Slot4: Slave Log"
#define IPC_SLAVELOG_PORT5_NAME         "Slot5: Slave Log"
#define IPC_SLAVELOG_PORT6_NAME         "Slot6: Slave Log"
#define IPC_SLAVELOG_PORT7_NAME         "Slot7: Slave Log"
#define IPC_SLAVELOG_PORT8_NAME         "Slot8: Slave Log"
#define IPC_SLAVELOG_PORT9_NAME         "Slot9: Slave Log"
#define IPC_SLAVELOG_PORT10_NAME        "Slot10: Slave Log"
#define IPC_SLAVELOG_PORT11_NAME        "Slot11: Slave Log"
#define IPC_SLAVELOG_PORT12_NAME        "Slot12: Slave Log"


#define MAX_SLAVE_SLOTS			13
#define MAX_SLAVE_PNAME			20

static uchar slavelog_port_name_table [MAX_SLAVE_SLOTS][MAX_SLAVE_PNAME] = {
{IPC_SLAVELOG_PORT0_NAME},
{IPC_SLAVELOG_PORT1_NAME},
{IPC_SLAVELOG_PORT2_NAME},
{IPC_SLAVELOG_PORT3_NAME},
{IPC_SLAVELOG_PORT4_NAME},
{IPC_SLAVELOG_PORT5_NAME},
{IPC_SLAVELOG_PORT6_NAME},
{IPC_SLAVELOG_PORT7_NAME},
{IPC_SLAVELOG_PORT8_NAME},
{IPC_SLAVELOG_PORT9_NAME},
{IPC_SLAVELOG_PORT10_NAME},
{IPC_SLAVELOG_PORT11_NAME},
{IPC_SLAVELOG_PORT12_NAME}
};

#define IPC_SLAVECORE_TFTP_OPEN         0x01
#define IPC_SLAVECORE_TFTP_CLOSE        0x02
#define IPC_SLAVECORE_TFTP_WRITE        0x04
 
extern void ipc_open_slave_core_ports(void);
extern void ipc_open_slave_log_ports(void);
extern void ipc_close_slave_core_ports(void);
extern void ipc_close_slave_log_ports(void);

/*
 * Convert slot number to slave log port name.
 */
static inline uchar * generate_slavelog_port_name (ushort slot)
{
    if (slot >= MAX_SLAVE_SLOTS)
        return(NULL);
 
    return(&slavelog_port_name_table[slot][0]);
}
