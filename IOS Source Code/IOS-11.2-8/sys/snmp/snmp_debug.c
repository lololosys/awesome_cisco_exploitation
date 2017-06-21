/* $Id: snmp_debug.c,v 3.4.24.1 1996/07/25 09:54:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_debug.c,v $
 *------------------------------------------------------------------
 * snmp_debug.c:  Debugging routines for SNMP
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_debug.c,v $
 * Revision 3.4.24.1  1996/07/25  09:54:23  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/08  22:01:48  jjohnson
 * CSCdi48221:  Software forced crash in RMON image
 * debug messages for lengthy octet strings, such as those associated with
 * rmon packet capture objects, can overflow the logger buffer, so
 * truncate them to a reasonable albeit arbitrary length
 *
 * Revision 3.3  1996/01/20  02:36:03  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1996/01/17  19:46:13  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.1  1996/01/17  17:44:59  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sr_snmpd.h"
#include "sys_registry.h"
#include "snmp_registry.h"
#include "../ui/debug.h"
#include "snmp_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "snmp_debug_flags.h"


/*
 * snmp_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the SNMP debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

static void
snmp_debug_all (boolean flag)
{
    generic_debug_all(snmp_debug_arr, flag);
}

/*
 * snmp_debug_show is called to display the values of all the SNMP
 * debugging variables.
 */

static void
snmp_debug_show (void)
{
    debug_show_flags(&(snmp_debug_arr[0]), "SNMP");
}

/*
 * snmp_debug_status is called to get the status of a debug flag.
 * typicially called from snmp transport subsystems via the service
 * point reg_invoke_snmp_debug
 */

static boolean
snmp_debug_status (int flag)
{
    return *(snmp_debug_arr[flag].var);
}


/*
 * snmp_debug_init is called at SNMP subsystem startup. It registers
 * the routines to maintain and display the SNMP debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when SNMP is started.
 */

void
snmp_debug_init (void)
{
    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(snmp_debug_all, "snmp_debug_all");
    reg_add_debug_show(snmp_debug_show, "snmp_debug_show");
    reg_add_snmp_debug(snmp_debug_status, "snmp_debug_status");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when SNMP is initialized, we want to turn on
     * all the SNMP debugging right now.
     */
    snmp_debug_all(debug_all_p());
}

void
snmp_print_message (Pdu *message)
{
    char *buffer = malloc(MAX_OCTET_STRING_SIZE);
    VarBind *vb;

    /*
     * first identify the mesaage type
     */
    switch(message->type) {
    case GET_REQUEST_TYPE:
	buginf("\nSNMP: Get request");
	break;
    case GET_NEXT_REQUEST_TYPE:
	buginf("\nSNMP: Get-next request");
	break;
    case GET_RESPONSE_TYPE:
	buginf("\nSNMP: Response");
	break;
    case SET_REQUEST_TYPE:
	buginf("\nSNMP: Set request");
	break;
#ifdef  SR_SNMPv1
    case TRAP_TYPE:
	buginf("\nSNMP: V1 Trap");
	break;
#endif                          /* SR_SNMPv1 */
#ifdef  SR_SNMPv2
    case GET_BULK_REQUEST_TYPE:
	buginf("\nSNMP: Get-bulk request");
	break;
    case INFORM_REQUEST_TYPE:
	buginf("\nSNMP: Inform request");
	break;
    case SNMPv2_TRAP_TYPE:
	buginf("\nSNMP: V2 Trap");
	break;
#endif                          /* SR_SNMPv2 */
    default:
	buginf("\nSNMP: Unknown operation");
	free(buffer);
	return;
    }

    /*
     * second, append pdu-type specific information
     */
    switch(message->type) {
    case GET_REQUEST_TYPE:
    case GET_NEXT_REQUEST_TYPE:
    case GET_RESPONSE_TYPE:
    case SET_REQUEST_TYPE:
#ifdef  SR_SNMPv2
    case INFORM_REQUEST_TYPE:
    case SNMPv2_TRAP_TYPE:
#endif                          /* SR_SNMPv2 */
	buginf(", reqid %d, errstat %d, erridx %d",
	       message->u.normpdu.request_id,
	       message->u.normpdu.error_status,
	       message->u.normpdu.error_index);
	break;

#ifdef  SR_SNMPv1
    case TRAP_TYPE:
	if (buffer)
	    MakeDotFromOID(message->u.trappdu.enterprise, buffer);
	buginf(", ent %s, addr %i, gentrap %d, spectrap %d",
	       buffer ? buffer : "oid overflow",
	       OctetStringToIP(message->u.trappdu.agent_addr),
	       message->u.trappdu.generic_trap,
	       message->u.trappdu.specific_trap);
	break;
#endif                          /* SR_SNMPv1 */

#ifdef  SR_SNMPv2
    case GET_BULK_REQUEST_TYPE:
	buginf(", reqid %d, nonrptr %d, maxreps %d",
	       message->u.bulkpdu.request_id,
	       message->u.bulkpdu.non_repeaters,
	       message->u.bulkpdu.max_repetitions);
	break;
#endif                          /* SR_SNMPv2 */
    }

    /*
     * third, dump the varbind list
     */
    if (buffer == NULL) {
	buginf(" \n (not enough memory to display variable bindings)");
    } else {
	for (vb = message->var_bind_list; vb; vb = vb->next_var) {
	    SPrintVarBind(vb, buffer);
	    /*
	     * long debug lines can corrupt memory when written to the
	     * debug log, so limit them to a reasonable length
	     */
	    if (strlen(buffer) > 256)
		strcpy(&buffer[253], "...");
	    buginf(" \n %s", buffer);
	}
	free(buffer);
    }
}
