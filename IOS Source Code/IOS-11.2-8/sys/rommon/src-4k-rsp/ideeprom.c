/* $Id: ideeprom.c,v 3.4.28.1 1996/03/21 23:32:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/ideeprom.c,v $
 *------------------------------------------------------------------
 * ideeprom.c
 *
 * Mar 1995, Steve J. Zhang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Dump out the content of ID EEPROM
 *------------------------------------------------------------------
 * $Log: ideeprom.c,v $
 * Revision 3.4.28.1  1996/03/21  23:32:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:58:53  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:07  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.4  1996/01/17  23:31:39  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.3  1996/01/10  03:30:40  tkam
 * CSCdi34476:  r4k ROM monitor doesnt save crashed system version
 * Bug fixes and adding RSP7000 support.
 *
 * Revision 3.2  1995/11/17  18:46:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dgbus.h"
#include "cookie.h"

#define ALPHA_MASK  0xF8
#define ALPHA_SHIFT 3
#define NUM_MASK    0x7
#define MIN_ALPHA_REV  0xa
#define ROUTER_MASK 0x80
#define ROUTER_SHIFT 0x7

#define MAX_CONTROLLER_TYPE 25
#define MAX_FAILCODE 0x17
#define MAX_ROUTER_TYPE 1

char *controllers[MAX_CONTROLLER_TYPE + 1] = {
                       "EIP", "Unknown", "SIP", "HIP", "Unknown", "TRIP", "FIP", 
		       "FSIP", "AIP","MIP", "Unknown", "SP", "RP", "CIP", "RSP1",
		       "RSP2", "CI", "SRS-TRIP", "SRS-FIP", "FEIP", "Unknown",
		       "Unknown", "Unknown", "Unknown", "RSP7000", "Unknown"};

char *failcode[MAX_FAILCODE + 1] = {
                    "NO Fail", "Bent Pin", "Component Damaged", "Component Failure",
		    "Cut Trace", "Damaged Board", "Damaged Component", "Down Rev",
		    "ECO Error", "Hosed", "Mis-configured Board", "Missing Component",
		    "Mis-inserted Component", "Other", "Pass", "Software Bug", 
		    "Solder Missing", "Solder Short", "Trace Open", "Trace Short",
		    "Vendor Repair", "Warped Board", "Wrong Component", "Not from the Field"};

char *boardtype[MAX_ROUTER_TYPE + 1] = {"ROUTER", "HUB"};
/* 
 * Name: dump_ideeprom
 *  Dump out the content of ID EEPROM
 */

dump_ideeprom()
{
    id_eeprom_t *id;

    id = (id_eeprom_t *)KSEG1_ADRSPC_ID_EEPROM;

    /* 
     * Display the content 
     */
    printf("EEPROM Version: %d\n", id->eeprom_version);

    if (id->controller_type > MAX_CONTROLLER_TYPE)
	printf("Controller Type: Unknown\n");
    else
	printf("Controller Type: %s\n", controllers[id->controller_type]);

    printf("RSP Hardware Version: %d.%d\n", id->hardware_version_high, id->hardware_version_low);

    printf("Serial Number: %d\n", (id->serial_number_31_24 << 24) + 
	                          (id->serial_number_23_16 << 16) +
	                          (id->serial_number_15_8 << 8) + (id->serial_number_7_0));

    printf("Part Number: %.2d-%.4d-%.2d\n", id->part_number_31_24,
	                                    (id->part_number_23_16 << 8) + id->part_number_15_8,
	                                    id->part_number_7_0);

    if (id->test_history > MAX_FAILCODE)
	printf("Test History: Unknown Code\n");
    else
	printf("Test History: %s\n", failcode[id->test_history]);

    printf("RMA Number: %d\n", (id->rma_number_23_16 << 16) +
	                       (id->rma_number_15_8 << 8) +
	                       id->rma_number_7_0);

    if (((id->board_revision & ALPHA_MASK) >> ALPHA_SHIFT) < MIN_ALPHA_REV)
        printf("Revision Number : %.2d\n", id->board_revision);
    else
        printf("Revision Number: %c%d\n",
               'A'+ ((id->board_revision & ALPHA_MASK) >> ALPHA_SHIFT) - MIN_ALPHA_REV,
               id->board_revision & NUM_MASK);

    printf("Deviation Number: %d\n", (id->deviation_number_15_8 << 8) + id->deviation_number_7_0);

    printf("Board Configuration Value: %d\n", id->board_config);

/*
    printf("Magic Number: %d\n", id->magic);

    printf("MAC Address 0000.0C%x.%x%x\n", id->macaddr[0], id->macaddr[1], id->macaddr[2]);

    if (((id->router_sign & ROUTER_MASK) >> ROUTER_SHIFT) > MAX_ROUTER_TYPE)
	printf("Router Type: Unknown\n");
    else
	printf("Router Type: %s\n", boardtype[(id->router_sign & ROUTER_MASK) >> ROUTER_SHIFT]);
*/ 

    printf("DPU ASIC Version: %d\n", id->DPU_rev);
    
}

static struct cookie virtual_cookie;

struct cookie *get_cookie(void) 
{
    strcpy((char *)&virtual_cookie, "I am fine!");
    return (&virtual_cookie);
}

void
set_platform_string(char *platform_string, unsigned short size)
{
    id_eeprom_t *id;

    id = (id_eeprom_t *)KSEG1_ADRSPC_ID_EEPROM;
    if (id->controller_type > MAX_CONTROLLER_TYPE)
	strcpy(platform_string, "Unknown");
    else if (sizeof(controllers[id->controller_type]) > size-1) {
	strncpy(platform_string, controllers[id->controller_type], size-1);
	platform_string[size-1] = '\0';
	}
    else
	strcpy(platform_string, controllers[id->controller_type]);

#ifdef DEBUG
    printf("sizeof(controllers[id->controller_type]) is %d\n", sizeof(controllers[id->controlle
 r_type]));
    printf("controller_type id is %d\n", id->controller_type);
    printf("platform_string is %s\n", platform_string);
#endif

    return;
}
