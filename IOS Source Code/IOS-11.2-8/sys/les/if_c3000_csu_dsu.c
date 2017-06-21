/* $Id: if_c3000_csu_dsu.c,v 3.2.2.3 1996/08/28 12:57:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_csu_dsu.c,v $
 *------------------------------------------------------------------
 * if_c3000_csu_dsu.c - Integrated CSU/DSU generic driver code
 *
 * March 1996, Shalabh Agarwal
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_csu_dsu.c,v $
 * Revision 3.2.2.3  1996/08/28  12:57:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.2.2  1996/08/07  09:01:15  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.2.1  1996/06/13  19:07:44  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2  1996/03/29  03:49:46  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:24  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../les/msg_csu_dsu.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "../if/network.h"
#include "mgd_timers.h"
#include "../les/if_les.h"
#include "init_les.h"
#include "../les/if_les_hd64570.h"
#include "../os/timer_generic.h"
#include "../les/if_c3000_csu_dsu.h"
#include "../parser/parser_defs_parser.h"

/*
 * Global variables
 */

static boolean	first_time = TRUE;	/* for one-time initialisation */
csu_dsu_instance_t *csu_dsu_int_array[MAX_SERIAL_PORTS] = 
		    {NULL, NULL};
watched_boolean		*alarm_process_awaken;	/* awaken background procees */	
mgd_timer 		parent_rem_lpback_timer;
static char *const      self_test_results_messages[DSU_MAX_SELF_TESTS] = {
        "flash chksum", "eerom chksum", "ram test", "rom chksum",
        "internal pattern test", "internal pattern test"
};

/* Forward prototypes */
void csu_dsu_clear_alarm_data(csu_dsu_instance_t *);

/*
 * csu_dsu_wan_module_type
 * Obtains what module is plugged in the serial slot
 */
static int csu_dsu_wan_module_type (hwidbtype *idb, int which, boolean cable_type)
{
    int module_id;
    uchar *wan_misc_reg;

    /* Get the first three bits of id */
    module_id = hd64570_get_cable_type(idb);

    /* Get fourth id bit from the WAN miscellanous register */
    wan_misc_reg = (uchar *) (which == 0 ? ADRSPC_BOA_WAN0_MISC : 
					   ADRSPC_BOA_WAN1_MISC);
    module_id |= ((*wan_misc_reg) & BOA_MISC_WAN_MOD_ID3) >> MISC_MOD_TYPE_ID3_SHIFT;
    module_id &= BOA_MOD_TYPE_ID_MASK;

    if (cable_type)	
    	return (module_id);

    switch (module_id) {
        case CANCUN_CABLE_NONE:
        case CANCUN_CABLE_232:
        case CANCUN_CABLE_449: 
        case CANCUN_CABLE_V35:
        case CANCUN_CABLE_X21:
        case CANCUN_CABLE_RS530:
                return (CSU_DSU_SERIAL_5IN1);
                break;
        case BOA_SW56_2W_ADTRAN_DSU_ID:
                return (CSU_DSU_SW56_2_WIRE);
                break;
        case BOA_SW56_4W_ADTRAN_DSU_ID:
                return (CSU_DSU_SW56_4_WIRE);
                break;
        case BOA_FT1_DIGITAL_LINK_DSU_ID:
                return (CSU_DSU_FT1);
                break;
        case BOA_NO_MODULE:
                return (CSU_DSU_NO_MODULE);
		break;
        default:
                return (CSU_DSU_UNRECOGNISED);
    }

}

/*
 * Displays the current serial interface module configuration
 * Called from Print_hardware()
 */
static void serial_show_int_enhanced (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if (idb->type == IDBTYPE_HD64570) {	 
 	    switch (csu_dsu_wan_module_type(idb, idb->unit, FALSE)) {
	        case CSU_DSU_SERIAL_5IN1:
		    printf("5-in-1 module for Serial Interface %d\n", idb->unit);
		    break;
		case CSU_DSU_NO_MODULE:
		    printf("No module installed for Serial Interface %d\n", idb->unit);
		    break;
		case CSU_DSU_SW56_2_WIRE:
		    printf("56k 2-wire CSU/DSU for Serial Interface %d\n", idb->unit);
		    break;
		case CSU_DSU_SW56_4_WIRE:
		    printf("56k 4-wire CSU/DSU for Serial Interface %d\n", idb->unit);
		    break;
		case CSU_DSU_FT1:
		    printf("FT1 CSU/DSU for Serial Interface %d\n", idb->unit);
		    break;
		default:
		    break;
	    }
        }
    }
}
		 	
/*
 * csu_dsu_get_status_value - get status value for CSU/DSU
 */
static void csu_dsu_get_status_value (hwidbtype *idb, void *ds, ushort *status)
{
    /*
     * If we have a CSU/DSU module, we need more information
     */
    if (((hd64570_instance_t *)ds)->csu_dsu_info != NULL) 
	*status = (ushort) csu_dsu_wan_module_type(idb, idb->unit, TRUE);
}

/*
 * csu_dsu_init_instance
 * Mallocs the driver data structure and initializes the fields
 */
static csu_dsu_instance_t *csu_dsu_init_instance (hwidbtype *idb,
                                                  int which, int module_type)
{
    csu_dsu_instance_t *cd_info = NULL;

    if (!(cd_info = malloc(sizeof(csu_dsu_instance_t))))
        return (NULL);

    if (which == 0) {
        cd_info->wan_misc_reg = (uchar *) ADRSPC_BOA_WAN0_MISC;
        cd_info->command_reg =  (uchar *) ADRSPC_BOA_WAN0_CMD_REG;
        cd_info->data_reg =     (uchar *) ADRSPC_BOA_WAN0_DATA_REG;
    }
    else if (which == 1) {
        cd_info->wan_misc_reg = (uchar *) ADRSPC_BOA_WAN1_MISC;
        cd_info->command_reg =  (uchar *) ADRSPC_BOA_WAN1_CMD_REG;
        cd_info->data_reg =     (uchar *) ADRSPC_BOA_WAN1_DATA_REG;
    }
    else {
        free(cd_info);
        return (NULL);
    }
    cd_info->idb = idb;
    csu_dsu_int_array[which] = cd_info;
    cd_info->module_type = module_type;
    cd_info->interface_number = which;
    cd_info->num_module_resets = 0;
    cd_info->dsu_reg_access_timeouts = 0;
    cd_info->current_loopback_state = DSU_NO_LPBACK;
    cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
    cd_info->last_loopback_command = CSU_DSU_LPBACK_NONE;

    /* Function called by "clear counters" command to reset T1 performance stats */
    idb->clear_support = csu_dsu_clear_counters;

    /*
     * Set user configurable CSU/DSU options to defaults
     */
    switch (module_type) {
	case CSU_DSU_FT1:
            t1_set_default_values(cd_info);
	    break;
	case CSU_DSU_SW56_4_WIRE:
	case CSU_DSU_SW56_2_WIRE:
            sw56_set_default_values(cd_info);
	    break;
	default:
	    break;
    }

    return (cd_info);
}

/*
 * Called after reload to do a software reset of CSU/DSU
 */
static boolean csu_dsu_set_default_state (csu_dsu_instance_t *cd_info)
{
    uchar software_reset = DSU_DEFAULTS_ENABLE;

    if (!cd_info)
	return (FALSE);

    if (csu_dsu_register_RW(cd_info, DSU_POWER_ON_DEFAULT_REG,
                            DSU_POWER_ON_DEFAULT_LEN,
                            &software_reset,
                            WRITE_REGISTER) != DSU_REQUEST_COMPLETE)
        return (FALSE);

    return (TRUE);
}

boolean csu_dsu_selftest_results (csu_dsu_instance_t *cd_info, boolean show_command)
{
    uchar selftest_results;
    uchar bitmask = 0x01;
    char mesg[100];
    char *selftest_mesg;
    int i;

    if (!cd_info)
       	return (FALSE);

    if (!show_command)
        buginf("\nSERVICE_MODULE(%d): self test finished: ", cd_info->interface_number);

    selftest_mesg = mesg;
    if  (csu_dsu_register_RW(cd_info, DSU_SELF_TEST_RESULTS_REG,
	 		     DSU_SELF_TEST_RESULTS_LEN, 
			     &selftest_results, 
			     READ_REGISTER) != DSU_REQUEST_COMPLETE) 
        selftest_mesg += sprintf(selftest_mesg, "results unretrievable");
    else { 
        if (selftest_results == DSU_SELF_TEST_PASSED) 
	    selftest_mesg += sprintf(selftest_mesg, "Passed");
        else {
	    selftest_mesg += sprintf(selftest_mesg, "Failed - ");
	
            for (i=DSU_FLASH_CHKSUM_FAILED; i<DSU_MAX_SELF_TESTS; 
				            i++, bitmask <<= 1) 
	        if (bitmask & selftest_results) 
	            selftest_mesg += sprintf(selftest_mesg, "%s,", 
					     self_test_results_messages[i]);
         }
    }

    if (!show_command)
	buginf("%s", mesg);
    else
	printf("%s", mesg);
  
    return (!selftest_results);
}

/*
 * Checks for module errors during CSU/DSU reset
 */
static void csu_dsu_sanity_check (csu_dsu_instance_t *cd_info)
{  
    uchar unit_type;
    int internal_unit_type = 0;

    /* First check the unit register for module type */
    if (csu_dsu_register_RW(cd_info, DSU_UNIT_TYPE_REG, DSU_UNIT_TYPE_LEN,
			    &unit_type, READ_REGISTER) == DSU_REQUEST_COMPLETE) {
	switch (unit_type) {
  	    case DIGITAL_LINK_FT1_MODULE:
		internal_unit_type = CSU_DSU_FT1;
		break;
	    case ADTRAN_SW56_4W_MODULE:
		internal_unit_type = CSU_DSU_SW56_4_WIRE;
                break;
	    case ADTRAN_SW56_2W_MODULE:
		internal_unit_type = CSU_DSU_SW56_2_WIRE;
                break;
	    default:
		/* Should never come here */
		buginf("Unrecognised module type\n");
		break;
	}

	if (cd_info->module_type != internal_unit_type)
	    errmsg(&msgsym(BADTYPE, SERVICE_MODULE), cd_info->interface_number,
		   unit_type);  
     }
     else if (csu_dsu_debug)
	buginf("\nSERVICE_MODULE(%d): Couldn't retrieve unit type", 
	       cd_info->interface_number);

     if (csu_dsu_register_RW(cd_info, DSU_INT_PROTOCOL_REV_REG, 
			     DSU_INT_PROTOCOL_REV_LEN, 
			     &cd_info->module_int_protocol_rev, 
			     READ_REGISTER) == DSU_REQUEST_COMPLETE) {
          if (cd_info->module_int_protocol_rev > cd_info->mod_highest_int_protocol_rev) {
	      cd_info->module_int_protocol_rev = cd_info->mod_highest_int_protocol_rev;
	      errmsg(&msgsym(OLDIMAGE, SERVICE_MODULE), cd_info->interface_number);
	  }
      }
      else {
	  if (csu_dsu_debug)
	      buginf("\nSERVICE_MODULE(%d): Couldn't retrieve module version number-assuming lowest",
		     cd_info->interface_number);
	  cd_info->module_int_protocol_rev = DSU_INT_PROTOCOL_LOWEST_REV;
      }

      /*
       * Now check self test results
       */
      csu_dsu_selftest_results(cd_info, FALSE);
}

static void csu_dsu_bringup_procedure (csu_dsu_instance_t *cd_info, 
				boolean (*set_state_func)(csu_dsu_instance_t *csu_dsu_info))
{
    /*
     * Set bringup flag so register access timeouts don't cause further resets
     */
    cd_info->module_bringup = TRUE;

    csu_dsu_sanity_check(cd_info);
    if (set_state_func)
	(*set_state_func)(cd_info);

    /* clear ALM counters */
    csu_dsu_clear_alarm_data(cd_info);

    BOA_ENABLE_ALM_INTERRUPT(cd_info->wan_misc_reg);

    cd_info->module_bringup = FALSE;
}

static inline void csu_dsu_hardware_reset (csu_dsu_instance_t *cd_info) 
{
    if (!cd_info)
	return;

    MODULE_RESET(cd_info->wan_misc_reg);
    if (get_interrupt_level())
	DELAY(DSU_RESET_HOLD_TIME);
    else
        process_sleep_for(DSU_RESET_HOLD_TIME);
    MODULE_UNRESET(cd_info->wan_misc_reg);
}
    
/*
 * csu_dsu_reset_process
 * Waits for the CSU/DSU to come up after a reset
 * Then does sanity checking and reconfiguration of non default parameters
 */
static process csu_dsu_reset_process (void)
{
    csu_dsu_instance_t *cd_info;
    int seconds_counter = 0;

    if (!process_get_arg_ptr((void **) &cd_info))
	process_kill(THIS_PROCESS);

    if ((cd_info == NULL) || (!cd_info->module_init))
	process_kill(THIS_PROCESS);

    /*
     * CSU/DSU s/w timedelay between actually asserting RESET line and
     * bringing the READY signal inactive 
     */
    process_sleep_for(DSU_RESET_WAIT_TIME);
    while (!RDY_SIGNAL_HIGH(cd_info->wan_misc_reg)) {
	process_sleep_for(ONESEC);	
	seconds_counter ++;
	if (seconds_counter/DSU_MAX_POWERCYCLE_TIME == DSU_MAX_MODULE_RESET_RETRIES) {
	   errmsg(&msgsym(REPEATEDRESET, SERVICE_MODULE), cd_info->interface_number,
		  DSU_MAX_MODULE_RESET_RETRIES);
	   cd_info->module_init = FALSE; /* to allow retry module reset */
	   process_kill(THIS_PROCESS);
	} 
	else if (seconds_counter%DSU_MAX_POWERCYCLE_TIME == 0) {
	   csu_dsu_hardware_reset(cd_info);
	   if (csu_dsu_debug)
	       buginf("\nSERVICE_MODULE(%d) - Still not up after %d secs - Resetting again", 
		      cd_info->interface_number, seconds_counter);
	}
    }

    if (csu_dsu_debug)
        buginf("\nSERVICE_MODULE(%d): reset took %d seconds", cd_info->interface_number, seconds_counter); 
    cd_info->module_init = FALSE;
    csu_dsu_bringup_procedure(cd_info, cd_info->dsu_set_current_config);
    process_kill(THIS_PROCESS);
}

/*
 * The Module reset routine
 */
void reset_csu_dsu (csu_dsu_instance_t *cd_info, boolean obtain_module_lock)
{
    pid_t bringup_pid;
 
    if (cd_info == NULL)
	return;
    
    if ((cd_info->module_bringup) || (cd_info->module_init))
	return;

    /* Inform the user module is being reset */
    errmsg(&msgsym(RESET, SERVICE_MODULE), cd_info->interface_number);

    cd_info->module_init = TRUE;		/* mark module being reset */

    /*
     * If reset issued by user, we need to let other processes
     * currently using the CSU/DSU to exit cleanly
     */
    if (obtain_module_lock) {
	if (!process_lock_semaphore(cd_info->csu_dsu_lock, CSU_DSU_LOCK_WAITTIME)) {
	    if (csu_dsu_debug)
	        buginf("\nCannot reset module - module busy");
	    cd_info->module_init = FALSE;
	    return;
	}
	/* 
	 * Now unlock the semaphore - this seemingly pointless exercise
	 * will ensure no processes were/are left waiting for a register access
	 */
	process_unlock_semaphore(cd_info->csu_dsu_lock);
     }

     BOA_MASK_ALM_RDY_INTERRUPT(cd_info->wan_misc_reg);
     cd_info->num_module_resets ++;
     cd_info->dsu_reg_access_timeouts = 0;
     GET_TIMESTAMP(cd_info->time_module_reset);

     /* Powercycle the module */
     csu_dsu_hardware_reset(cd_info);

     bringup_pid = process_create(csu_dsu_reset_process,
				  "Service-module initialization", 
				  NORMAL_STACK, PRIO_NORMAL);
     if (bringup_pid == NO_PROCESS) {
	/*
	 * Since module was reset, and cannot create process, warn 
	 * user that any non-default values configured may be lost
	 */
	errmsg(&msgsym(RESETFAILURE, SERVICE_MODULE), cd_info->interface_number);
	cd_info->module_init = FALSE;	/* to allow user to keep trying */
	return;
     }
 
     process_set_arg_ptr(bringup_pid, cd_info);
}

int csu_dsu_register_RW (csu_dsu_instance_t* cd_info, uchar register_addr,
                         int register_length, void *register_data,
                         boolean write)
{
    int data_length, RDY_wait_counter;
    int dsu_post_cmd_wait_counter;
    int dsu_interbyte_wait_counter;	
    int dsu_interbyte_wait_time;
    char *data_ptr = register_data;
    ulong post_cmd_sleep_time = 0;
    boolean interrupts_blocked;

    if (cd_info == NULL)
        return (DSU_BAD_INSTANCE);

    if (cd_info->module_init)           /* Module is being reset */
        return (DSU_BEING_RESET);

    /*
     * Suspend until we obtain exclusive control of the CSU/DSU I/O data bus
     */
    if (!process_lock_semaphore(cd_info->csu_dsu_lock, CSU_DSU_LOCK_WAITTIME)) {
	errmsg(&msgsym(REQUESTOVERLOAD, SERVICE_MODULE), cd_info->interface_number);
        return (DSU_LOCK_OBTAIN_TIMEOUT);
    }

    if (cd_info->module_init) {
	/*
         * We check this condition again to let the user command 
         * "clear service-module" gain fast access to the CSU/DSU 
	 */
        process_unlock_semaphore(cd_info->csu_dsu_lock);
        return (DSU_BEING_RESET);
    }

    BOA_MASK_READY_INTERRUPT(cd_info->wan_misc_reg);
    interrupts_blocked = get_interrupt_level();
    data_length = register_length;
    dsu_interbyte_wait_time = write ?
			      cd_info->dsu_interwrite_wait_time :
			      cd_info->dsu_interread_wait_time;

    /*
     * Phase 1 - Wait till RDY signal is high, to write register addr
     */
    RDY_wait_counter = 0;
    while (!RDY_SIGNAL_HIGH(cd_info->wan_misc_reg)) {

	if (interrupts_blocked)
  	    DELAY(DSU_WAIT_RDY_SIGNAL_HIGH);
	else
            process_sleep_for(DSU_WAIT_RDY_SIGNAL_HIGH);

	if (RDY_wait_counter ++ > DSU_MAX_RDY_SIGNAL_RETRIES) {
	    errmsg(&msgsym(NOTREADY, SERVICE_MODULE), cd_info->interface_number);
	    goto register_RW_failure;
	}
    }

    /*
     * Supply address of register to read/write to CSU/DSU
     */
    *cd_info->command_reg = (uchar) register_addr;

    /*
     * For most registers, a minimum of 5msecs wait is recommended
     * For a few, the length of the registers dictates a longer time which
     * can be specified by the calling process
     */
    if (cd_info->dsu_specify_reg_wait_time) 
	post_cmd_sleep_time = (*cd_info->dsu_specify_reg_wait_time)(register_addr);
    post_cmd_sleep_time = post_cmd_sleep_time ? post_cmd_sleep_time : 
					        DSU_POST_COMMAND_FIXED_WAIT;

    /*
     * Phase 2 - Now wait for CSU/DSU to prepare it's output FIFO 
     *		 for supplying/storing the register contents
     */
    dsu_post_cmd_wait_counter = 0;
    do {
        /*
         * If this is being done during init with network interrupts blocked
         * we cannot disms - instead we waste cpu cycles!
         */
        if (interrupts_blocked)
            DELAY(post_cmd_sleep_time);
        else
            process_sleep_for(post_cmd_sleep_time);

	if (++dsu_post_cmd_wait_counter > DSU_MAX_POST_CMD_WAIT_RETRIES) { 
	    errmsg(&msgsym(COMMANDFAILED, SERVICE_MODULE), cd_info->interface_number,
		   register_addr);
	    goto register_RW_failure;
	}
    } while (!RDY_SIGNAL_HIGH(cd_info->wan_misc_reg));

    /*
     * Phase 3 - write/read the contents of register one by one
     */
    while (data_length > 0) {
	dsu_interbyte_wait_counter = 0;
	while (!RDY_SIGNAL_HIGH(cd_info->wan_misc_reg)) {
	    WASTETIME(dsu_interbyte_wait_time);
	    if (++dsu_interbyte_wait_counter > DSU_MAX_INTERBYTE_RDY_RETRIES) {
		errmsg(&msgsym(INTERBYTETIMEOUT, SERVICE_MODULE), 
		       cd_info->interface_number, write ? "writing" : "reading", 
		       data_length, register_length, register_addr);
		goto register_RW_failure;
	    }
	}

	if (write)
	    *cd_info->data_reg = *data_ptr ++;
	else
	    *data_ptr ++ = *cd_info->data_reg;

	data_length --;
    }

    cd_info->dsu_reg_access_timeouts = 0;
    process_unlock_semaphore(cd_info->csu_dsu_lock);
    return (DSU_REQUEST_COMPLETE);

  register_RW_failure:
    if (++cd_info->dsu_reg_access_timeouts > DSU_NUM_REG_TIMEOUTS_RESET) {
	reset_csu_dsu(cd_info, FALSE);
	cd_info->dsu_reg_access_timeouts = 0;
    }

    process_unlock_semaphore(cd_info->csu_dsu_lock);
    return (DSU_REQUEST_TIMEOUT);
}

/*
 * The level 3 CSU/DSU Alarm Interrupt Handler routine
 */
void csu_dsu_ALM_int (csu_dsu_instance_t *cd_info) 
{


    if (!cd_info) 
        return;

    if (!ALM_SIGNAL_HIGH(cd_info->wan_misc_reg)) {
	if (csu_dsu_debug)
            buginf("\nSERVICE_MODULE(%d): Spurious alarm interrupt", 
		   cd_info->interface_number);
        return;
    }

    cd_info->alarm_asserted = TRUE;
    GET_TIMESTAMP(cd_info->alarm_event_timestamp);
    process_set_boolean(alarm_process_awaken, TRUE);
    BOA_MASK_ALM_INTERRUPT(cd_info->wan_misc_reg);
    return;
}

static inline ulong power10_x (int n)
{
    int i;
    ulong x;

    for (i = n, x = 1; i > 0; x *= 10, i--);
    return(x);
}

static inline int log10_x (ulong n)
{
    int i;
    ulong x;

    for (i = 0, x = n; x >= 10; x /= 10, i++);
    return (i);
}

void csu_dsu_calculate_bert_test (csu_dsu_instance_t *cd_info)
{
    ulong lpback_msecs_duration;
    ulong bit_errors;
    ulong line_speed;
    ulong bits_xmitted;
    int log_bits_xmitted, log_bit_errors;

    if (!cd_info || (cd_info->current_loopback_command != CSU_DSU_LOOPBACK_REMOTE))
        return;

    /*
     * If no test pattern was being sent, don't need to calculate bert 
     */
    if (cd_info->dsu_loopback_test_pattern == CSU_DSU_LPBACK_PAT_NOSEL) {

        lpback_msecs_duration = ELAPSED_TIME(cd_info->loopback_start_time);
        cd_info->bert_mantissa = 0;
        cd_info->bert_exponent = 0;
	bit_errors = cd_info->dsu_loopback_bit_errors;

	/*
	 * Calculate bert using the line speed set at start loopback 
	 */
	if (cd_info->dsu_current_line_bandwidth)
	    line_speed = (*cd_info->dsu_current_line_bandwidth)(cd_info, FALSE);
	else
	    return;

	bits_xmitted = line_speed * (ELAPSED_TIME(cd_info->loopback_start_time)/ONESEC);
        log_bits_xmitted = log10_x(bits_xmitted);
        log_bit_errors =   log10_x(bit_errors);
	
	if ((bit_errors == 0) || (bits_xmitted == 0))
	    return;

	/*
	 * First normalize denominator to power of 5 
	 */
	if (log_bits_xmitted > 6)
	    bits_xmitted /= power10_x(log_bits_xmitted - 6);
	else
	    bits_xmitted *= power10_x(6 - log_bits_xmitted);
 
	/*
	 * Now normalize numerator to power of 8 giving 3 bytes of precision
	 */
	bit_errors *= power10_x(8 - log_bit_errors);

	cd_info->bert_mantissa = bit_errors/bits_xmitted;
	cd_info->bert_exponent = log_bit_errors - log_bits_xmitted;
    }

    return;
}

void csu_dsu_loopback_finished (csu_dsu_instance_t *cd_info)
{
    if (!cd_info)
        return;

    if (cd_info->current_loopback_command == CSU_DSU_LPBACK_NONE)
	return;

    cd_info->idb->loopback = FALSE;
    cd_info->last_loopback_command = cd_info->current_loopback_command;
    cd_info->dsu_last_loopback_type = cd_info->dsu_loopback_type;
    cd_info->dsu_last_loopback_pattern = cd_info->dsu_loopback_test_pattern;
    cd_info->dsu_last_loopback_user_pattern = cd_info->dsu_loopback_user_pattern;
    cd_info->dsu_last_loopback_bit_errors = cd_info->dsu_loopback_bit_errors;
    cd_info->dsu_last_loopback_synced_pattern =
					cd_info->dsu_loopback_synced_on_pattern;
    cd_info->dsu_last_remote_loopback_passed = TRUE;

    /* tell parser no more loopbacks in progress */
    cd_info->usr_cfg_loopback_dte_type = CSU_DSU_LPBACK_TYPE_NOSEL;
    cd_info->usr_cfg_loopback_line_type = CSU_DSU_LPBACK_TYPE_NOSEL;
    cd_info->usr_cfg_loopback_remote_type = CSU_DSU_LPBACK_TYPE_NOSEL;
 
    COPY_TIMESTAMP(cd_info->loopback_start_time,
                   cd_info->last_loopback_start_time);
    COPY_TIMESTAMP(cd_info->loopback_pattern_sync_duration,
		   cd_info->last_loopback_pattern_sync_duration);
    GET_TIMESTAMP(cd_info->last_loopback_end_time);
}

static ulong csu_dsu_decide_rem_lpback_state (csu_dsu_instance_t *cd_info)
{
    int new_lpback_state;
    ulong new_timeout = 0;

    if (!cd_info || (cd_info->current_loopback_command != CSU_DSU_LOOPBACK_REMOTE))
	return (0);

    if (!cd_info->dsu_remote_lpback_state)
	return (0);

    new_lpback_state = (*cd_info->dsu_remote_lpback_state)(cd_info);
    if (new_lpback_state == DSU_TEST_STATUS_UNREADABLE) {
	/* Try back in a second */
	return (ONESEC);
    }

    new_timeout = 0;
    switch (cd_info->current_loopback_state) {
	case DSU_NO_LPBACK:
	    /* how did we get here ? */
	    cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
	    cd_info->usr_cfg_loopback_remote_type = CSU_DSU_LPBACK_TYPE_NOSEL;
	    break;
	case DSU_IN_LOOPBACK:
	    /* if we're already in loopback, what're we doing here? */
	    if (new_lpback_state == NO_REMOTE_TEST) {
		/* How did loopback get cancelled - module misbehaving? */
		if (csu_dsu_debug)
		    buginf("\nSERVICE_MODULE(%d): Remote loopback terminated for unknown reason", 
			   cd_info->interface_number);
		csu_dsu_calculate_bert_test(cd_info);
		csu_dsu_loopback_finished(cd_info);
                cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
                cd_info->current_loopback_state = DSU_NO_LPBACK;
	    }
	    break;
	case DSU_ATTEMPTING_LOOPUP:
	    switch (new_lpback_state) {
	        case ATTEMPTING_REMOTE_TEST:
	            /* Loopback still being attempted - check back in a sec */
	            new_timeout = ONESEC;
		    break;
		case PASSED_REMOTE_TEST:
		    cd_info->current_loopback_state = DSU_IN_LOOPBACK;
		    errmsg(&msgsym(LOOPUPREMOTE, SERVICE_MODULE), 
			   cd_info->interface_number);
		    /* update the bert calculation stuff */
		    GET_TIMESTAMP(cd_info->loopback_start_time);
		    break;
		case FAILED_REMOTE_TEST:
		case NO_REMOTE_TEST:
		    csu_dsu_loopback_finished(cd_info);
		    cd_info->dsu_last_remote_loopback_passed = FALSE;
		    cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
		    cd_info->current_loopback_state = DSU_NO_LPBACK;
		    cd_info->usr_cfg_loopback_remote_type = CSU_DSU_LPBACK_TYPE_NOSEL;
		    errmsg(&msgsym(LOOPUPFAILED, SERVICE_MODULE), 
			   cd_info->interface_number);
		    break;
		default:
		    break;
	    }
	    break;
	case DSU_ATTEMPTING_LOOPDOWN:
	    switch (new_lpback_state) {
                case ATTEMPTING_REMOTE_TEST:
                    /* Loopdown still being attempted - check back in a sec */
                    new_timeout = ONESEC;
                    break;
                case PASSED_REMOTE_TEST:
		case NO_REMOTE_TEST:
		    csu_dsu_calculate_bert_test(cd_info);
		    csu_dsu_loopback_finished(cd_info);
		    cd_info->dsu_last_remote_loopback_passed = TRUE;
		    cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
                    cd_info->current_loopback_state = DSU_NO_LPBACK;
		    errmsg(&msgsym(LOOPDOWNREMOTE, SERVICE_MODULE),
			   cd_info->interface_number);
                    break;
                case FAILED_REMOTE_TEST:
		    errmsg(&msgsym(LOOPDOWNFAILED, SERVICE_MODULE),
			   cd_info->interface_number);
                    cd_info->current_loopback_state = DSU_IN_LOOPBACK;
                    break;
                default:
                    break;
            }
            break;
	default:
	    break;
    }
 
    return (new_timeout);
}

static ulong csu_dsu_diagnostic_status (csu_dsu_instance_t *cd_info)
{

    if (!cd_info) 
        return (0);

    switch (cd_info->current_loopback_command) {
	case CSU_DSU_SELFTEST:
	    /*
	     * Retrieve self test results and print to user
	     */
	    cd_info->current_loopback_command = CSU_DSU_LPBACK_NONE;
	    csu_dsu_selftest_results(cd_info, FALSE);
    	    break;
	case CSU_DSU_LOOPBACK_REMOTE:
	    return (csu_dsu_decide_rem_lpback_state(cd_info));
	    break;
	default:
	    break;	
    }
	
    return (0);
}

static void csu_dsu_alarm_begin (csu_dsu_instance_t *cd, int alarm_bit)
{
    if (!cd || (alarm_bit >= MAX_ALARM_EVENTS))
       return;

    cd->alarm_counts[alarm_bit].count ++;
    COPY_TIMESTAMP(cd->alarm_event_timestamp,
    		   cd->alarm_counts[alarm_bit].current_duration);
    if (csu_dsu_debug)
       buginf("\nSERVICE_MODULE(%d): detects %s", cd->interface_number,
              cd->dsu_alarm_messages[alarm_bit]);
}

static void csu_dsu_alarm_cleared (csu_dsu_instance_t *cd, int alarm_bit)
{
    char duration_str[40];
   
    if (!cd || (alarm_bit >= MAX_ALARM_EVENTS))
       return;

    COPY_TIMESTAMP(cd->alarm_counts[alarm_bit].current_duration,
                   cd->alarm_counts[alarm_bit].last_occurrence);
    cd->alarm_counts[alarm_bit].last_duration =
       ELAPSED_TIME(cd->alarm_counts[alarm_bit].current_duration);
    TIMER_STOP(cd->alarm_counts[alarm_bit].current_duration);

    if (csu_dsu_debug) {
        sprint_ticks_in_dhms(duration_str, 
			     cd->alarm_counts[alarm_bit].last_duration);        
        buginf("\nSERVICE_MODULE(%d): %s ended after duration %s",
               cd->interface_number, cd->dsu_alarm_messages[alarm_bit],
	       duration_str);
    }
}

static boolean csu_dsu_alarm_processor_routine (csu_dsu_instance_t *cd_info, 
					 boolean alarms_init)
{
    ushort bitmask;
    ushort alarm_transitions, alarm_current_status;
    int i, lo_alarm_index, hi_alarm_index;
    boolean asserted;

    if (!cd_info || !cd_info->dsu_get_alarm_status)
        return (FALSE);

    if (!(*cd_info->dsu_get_alarm_status)(cd_info, &lo_alarm_index, 
 				          &hi_alarm_index, 
					  &alarm_transitions,
					  &alarm_current_status))
        return (FALSE);

    /*
     * Now process all alarm transition changes
     * If initializing alarms (CSU/DSU bringup) ignore this
     */
    if (!alarms_init) {
        for (bitmask = 0x0001, i = lo_alarm_index; 
	     i < hi_alarm_index; i ++, bitmask <<= 1) {
            if (bitmask & cd_info->dsu_alarm_mask) /* alarm masked out */
                continue;
            if (alarm_transitions & bitmask) {
                /* change in alarm status */
                if (cd_info->dsu_network_alarm_status & bitmask) {
                    /* If alarm was previously asserted, now it has cleared */
                    cd_info->dsu_network_alarm_status &= ~bitmask;
                    csu_dsu_alarm_cleared(cd_info, i);
                }
                else {
                    /* If alm was previously inactive, now it has happened */
                    cd_info->dsu_network_alarm_status |= bitmask;
                    csu_dsu_alarm_begin(cd_info, i);
                }
            }
        }
    }
    else {
	/* 
	 * If initializing alarms, reset cached driver alarm value
	 * so that router doesn't 'remember' about old state before module reset
	 */
	cd_info->dsu_network_alarm_status = DSU_NO_ALARMS;
    }
 
    /*
     * Process the real time status now
     * This reflects any changes in network condns in the time it took to
     * read the alarm register - it also represents the base for reporting
     * the next alarm transition changes
     */
    for (bitmask = 0x0001, i = lo_alarm_index;
         i < hi_alarm_index; i ++, bitmask <<= 1) {
        if (bitmask & cd_info->dsu_alarm_mask) /* alarm masked out */
            continue;
        if ((cd_info->dsu_network_alarm_status & bitmask) !=
                (asserted = (alarm_current_status & bitmask))) {
            if (asserted)
                csu_dsu_alarm_begin(cd_info, i);
            else
                csu_dsu_alarm_cleared(cd_info, i);
        }
    }

    /* Save the new network alarm status */
    cd_info->dsu_network_alarm_status = alarm_current_status;

    /*
     * Inform manager of new network status via SNMP trap
     */
    reg_invoke_csu_dsu_send_snmp_loop_status_trap(cd_info->module_type, 
                                                  alarm_current_status, 
                                                  cd_info->idb->snmp_if_index);

    return (TRUE);
}

void csu_dsu_clear_alarm_data (csu_dsu_instance_t * cd_info)
{

    /* Make all alarm counts zero */
    bzero(&cd_info->alarm_counts, MAX_ALARM_EVENTS * sizeof(dsu_alarm_data_t));

    /* Clear any performance counters on module */
    if (cd_info->dsu_clear_alarm_data)
	(*cd_info->dsu_clear_alarm_data)(cd_info);

    /*
     * Update the timestamp to make the alarm initialization code
     * think that an alarm event just started (normally, the interrupt
     * updates this, but at this point alarms are masked off)
     */
    GET_TIMESTAMP(cd_info->alarm_event_timestamp);
    GET_TIMESTAMP(cd_info->last_counter_reset); 
    cd_info->alarm_asserted = FALSE;

    /*
     * Initialize the new network alarm status and print debugs of
     * existing alarms
     */
    csu_dsu_alarm_processor_routine(cd_info, TRUE);
}

void csu_dsu_clear_counters (hwidbtype *idb) 
{
    csu_dsu_instance_t *cd_info;

    if (!(cd_info = csu_dsu_get_instance(idb))) 
	return;

    BOA_MASK_ALM_INTERRUPT(cd_info->wan_misc_reg);
    csu_dsu_clear_alarm_data(cd_info);
    BOA_ENABLE_ALM_INTERRUPT(cd_info->wan_misc_reg);
}


/*
 * The Background alarm processing routine
 */
static process csu_dsu_ALM_background_process (void)
{
    csu_dsu_instance_t *cd_info;
    mgd_timer *expired_timer;
    ulong major, minor;
    ushort timer_number;
    ulong new_delay;
    int i;

    /* If there are no CSU/DSUs, no reason to live */
    if (!csu_dsu_int_array[0] && !csu_dsu_int_array[1]) 
	process_kill(THIS_PROCESS);

    process_watch_boolean(alarm_process_awaken, ENABLE, RECURRING);
    process_watch_mgd_timer(&parent_rem_lpback_timer, ENABLE);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
		case BOOLEAN_EVENT:
		    if ((minor != W_BOOLEAN_ALM_TYPE) ||
			(!process_get_boolean(alarm_process_awaken)))
 		        break;

		    /*
	             * Note - setting watched boolean to FALSE may conflict
	 	     * with interrupts - its purpose is only for awakening us 
	 	     * cd_info->alarm_asserted is true measure of active alarm 
	 	      */
		    process_set_boolean(alarm_process_awaken, FALSE);
	            for (i = 0; i < MAX_SERIAL_PORTS; i++) {
	                if (!(cd_info = csu_dsu_int_array[i]))
	                    continue;
	                if (cd_info->alarm_asserted) {
		            cd_info->alarm_asserted = FALSE;
			    if (csu_dsu_alarm_processor_routine(cd_info, FALSE))
				BOA_ENABLE_ALM_INTERRUPT(cd_info->wan_misc_reg); 
	    		}
		    }
		    break;
		case TIMER_EVENT:
		    while (mgd_timer_expired(&parent_rem_lpback_timer)) {
			expired_timer = 
			    mgd_timer_first_expired(&parent_rem_lpback_timer);
			timer_number = mgd_timer_type(expired_timer);
			cd_info = mgd_timer_context(expired_timer);
			if (timer_number <= WAN_PORT_1) {
			    if (cd_info != csu_dsu_int_array[--timer_number]) {
				if (csu_dsu_debug)
				    buginf("\nUnknown module attempting loopback");
				mgd_timer_stop(expired_timer);
				continue;
			    }
			}
			else {
			    if (csu_dsu_debug)
			        buginf("\nSERVICE_MODULE(%d): Bad timer %d Context %x awoke ALM proc", 
				       cd_info->interface_number, timer_number, 
				       cd_info);
			    /* Stop phantom timer to prevent reawakening */
			    mgd_timer_stop(expired_timer);
			    continue;
			}
			mgd_timer_stop(&cd_info->dsu_diagnostic_timer);
			new_delay = csu_dsu_diagnostic_status(cd_info);
			if (new_delay != 0) 
			    mgd_timer_start(&cd_info->dsu_diagnostic_timer, new_delay);
		    }		
		    break;
		default:
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		    break;
	     }
	     process_may_suspend();
        }       
    }
}		

/*
 * csu_dsu_decode_loopback - deciphers type of loopback currently in
 *                           progress
 */
static uchar csu_dsu_decode_loopback (uchar loopback_command, uchar loopback_type, 
			              uchar loopback_pattern)
{
    switch (loopback_command) {
        case CSU_DSU_LOOPBACK_DTE:
            return (DSU_DTE_LOOPBACK);
        case CSU_DSU_LOOPBACK_LINE:
	    if (loopback_type == CSU_DSU_LPBACK_TYPE_FR)
		return (DSU_LOCAL_PAYLOAD_LOOPBACK);
	    else
                return (DSU_LOCAL_NET_LOOPBACK);
        case CSU_DSU_SELFTEST:
            return (DSU_SELF_TEST);
        case CSU_DSU_LOOPBACK_REMOTE:
            switch (loopback_type) {
                case CSU_DSU_LPBACK_TYPE_DEF:
                case CSU_DSU_LPBACK_TYPE_FR:
                    return ((loopback_pattern ==
                                   CSU_DSU_LPBACK_PAT_NOSEL) ?
                             DSU_REMOTE_LOOPUP :
                             DSU_REMOTE_LOOPUP_SEND_PATTERN);
                    break;
                case CSU_DSU_LPBACK_TYPE_SJ:
                    return ((loopback_pattern ==
                                   CSU_DSU_LPBACK_PAT_NOSEL) ?
                             DSU_SMART_JACK_LOOPUP:
                             DSU_SMART_JACK_LOOPUP_SEND_PATTERN);
                    break;
                default:
                    break;
            }
            return (DSU_REMOTE_LOOPUP);
            break;
        default:
           return (DSU_LOOPBACK_NONE);
    }
}


/*
 * csu_dsu_show_loopback_type - displays loopback strings in
 *                         show service-module command
 */
static boolean csu_dsu_show_loopback_type (uchar loopback_command, 
					   uchar loopback_fraction)
{
    switch (loopback_command) {
        case DSU_LOOPBACK_NONE:
            printf("no loopback ");
            return (FALSE);
            break;
        case DSU_DTE_LOOPBACK:
            printf("dte loopback ");
            break;
        case DSU_LOCAL_PAYLOAD_LOOPBACK:
            printf("line loopback (payload) ");
            break;
        case DSU_LOCAL_NET_LOOPBACK:
            printf("line loopback ");
            break;
        case DSU_REMOTE_LOOPUP:
        case DSU_REMOTE_LOOPUP_SEND_PATTERN:
        case DSU_REMOTE_LOOPDOWN:
            printf("remote loopback ");
            if (loopback_fraction == T1_TEST_FRACTION_ALLOCATED_FRACTION)
                printf("(payload) ");
            break;
        case DSU_SELF_TEST:
            printf("self test ");
            break;
        case DSU_SMART_JACK_LOOPUP:
        case DSU_SMART_JACK_LOOPUP_SEND_PATTERN:
        case DSU_SMART_JACK_LOOPDOWN:
            printf("smart-jack loopback ");
            break;
        default:
            printf("unknown loopback ");
            return (FALSE);
            break;
    }

    return (TRUE);
}

void csu_dsu_show_last_lpback (csu_dsu_instance_t *cd_info)
{
    uchar last_loopback;

    if (!cd_info)
	return;

    last_loopback = csu_dsu_decode_loopback(cd_info->last_loopback_command, 
					    cd_info->dsu_last_loopback_type,
					    cd_info->dsu_last_loopback_pattern);

    csu_dsu_show_loopback_type(last_loopback, ((cd_info->dsu_last_loopback_type ==
                                              CSU_DSU_LPBACK_TYPE_DEF) ?
                                              T1_TEST_FRACTION_FULL_BW :
                                              T1_TEST_FRACTION_ALLOCATED_FRACTION));

    if ((cd_info->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE) &&
      !(cd_info->dsu_last_remote_loopback_passed)) {
      printf("\n    Failed to loopup remote");
      return;
    }

    printf("\n    duration ");
    print_ticks_in_dhms(CLOCK_DIFF_UNSIGNED(cd_info->last_loopback_start_time,
                                            cd_info->last_loopback_end_time));

    if ((cd_info->last_loopback_command == CSU_DSU_LOOPBACK_REMOTE) &&
      (cd_info->dsu_last_loopback_pattern != CSU_DSU_LPBACK_PAT_NOSEL)) {
        if (cd_info->dsu_last_loopback_synced_pattern) {
            printf("    Bit errors = %d", cd_info->dsu_last_loopback_bit_errors);
            if (cd_info->module_type == CSU_DSU_FT1) {
                printf(", sync duration ");
                print_ticks_in_dhms(CLOCK_DIFF_UNSIGNED(cd_info->last_loopback_pattern_sync_duration,
				    cd_info->last_loopback_end_time));
            }
        }
 	else 
	    printf("    Out of sync with test-pattern");
    }
}

/*
 * dsu_show_loopback_status - shows current loopback status for
 *                           show service-module command
 */
void csu_dsu_show_loopback_status (csu_dsu_instance_t *cd_info,
                              uchar loopback_command, uchar test_pattern)
{
    uchar test_bandwidth = T1_TEST_FRACTION_FULL_BW;

    if (!cd_info)
	return;

    if (loopback_command == DSU_LOOPBACK_NONE)
        return;

    if (cd_info->module_type == CSU_DSU_FT1) {
        if (csu_dsu_register_RW(cd_info, T1_TEST_FRACTION_REG,
                                T1_TEST_FRACTION_LEN, &test_bandwidth,
                                READ_REGISTER) != DSU_REQUEST_COMPLETE)
            return;
    }

    printf("\nUnit is currently in test mode:");
    printf("\n    ");

    if (!csu_dsu_show_loopback_type(loopback_command, test_bandwidth))
        return;

    /* if we don't know about the test, must be remotely initiated */
    if (cd_info->current_loopback_command == DSU_LOOPBACK_NONE)
        printf("(remotely initiated) ");

    switch (loopback_command) {
        case DSU_DTE_LOOPBACK:
        case DSU_SELF_TEST:
        case DSU_LOCAL_PAYLOAD_LOOPBACK:
        case DSU_LOCAL_NET_LOOPBACK:
            printf("is in progress ");
            break;
        case DSU_REMOTE_LOOPDOWN:
        case DSU_SMART_JACK_LOOPDOWN:
            if ((test_pattern != 0) ||
                 (cd_info->dsu_loopback_test_pattern != 0))
                printf("with test-pattern ");
            printf("is currently terminating");
            break;
        case DSU_REMOTE_LOOPUP_SEND_PATTERN:
        case DSU_SMART_JACK_LOOPUP_SEND_PATTERN:
            printf("with test-pattern ");
        /* WARNING - fall through case */
        case DSU_REMOTE_LOOPUP:
        case DSU_SMART_JACK_LOOPUP:
            switch (cd_info->current_loopback_state) {
                case DSU_ATTEMPTING_LOOPDOWN:
                    /* This case should never happen */
                    printf("is terminated ");
                    break;
                case DSU_ATTEMPTING_LOOPUP:
                    printf("is being initiated ");
                    break;
                case DSU_IN_LOOPBACK:
                    printf("is in progress ");
                    break;
                default:
                    printf("is in unknown state ");
                    break;
            }
            break;
        default:
            return;
    }
}

int csu_dsu_loopback_command (csu_dsu_instance_t *cd_info, int loopback_command,
			      int loopback_type, int test_pattern, 
			      ulong user_pattern)

{
    int result;
    uchar dsu_loopback_command;
    uchar current_test;

    if (!cd_info)
	return (DSU_BAD_INSTANCE);

    if (loopback_command != CSU_DSU_LPBACK_NONE) {
	/*
	 * Check if any user or remotely initiated tests active
	 */
        if (cd_info->current_loopback_command != CSU_DSU_LPBACK_NONE) {
            printf("\nLoopback already in progress");
            return (DSU_WRONG_FORMAT);
        }
	
	if ((cd_info->dsu_remote_tests) && 
		(*cd_info->dsu_remote_tests)(cd_info)) {
	    printf("\nRemotely-initiated loopback already in progress");
	    return (DSU_WRONG_FORMAT);
        }

	/*
	 * For certain types of loopbacks, need to ensure connection
	 * is active
	 */
	if ((cd_info->dsu_check_connection_status) &&	
	   !(*cd_info->dsu_check_connection_status)(cd_info, loopback_command, 
						    loopback_type)) {
	    printf("\nNeed active connection for this type of loopback");
	    return (DSU_WRONG_FORMAT);
	}

        /*
  	 * Make sure no tests active that we don't know about
	 */
        if (csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                                DSU_TEST_TYPE_SELECT_LEN,
                                &current_test,
                                READ_REGISTER) == DSU_REQUEST_COMPLETE)  
	    if (current_test != DSU_LOOPBACK_NONE) {
		printf("\nTest already in progress - source unknown");
		return (DSU_WRONG_FORMAT);
	    }
    }


    switch (loopback_command) {
	case CSU_DSU_LOOPBACK_DTE:
	    dsu_loopback_command = DSU_DTE_LOOPBACK;
	    /* 
	     * If we are a T1 module, we want to do a DTE loopback at the
	     * full 1.544Mbps bandwidth - so select full bw test fraction
	     */
	    if (cd_info->module_type == CSU_DSU_FT1) 
		t1_set_test_fraction(cd_info, T1_TEST_FRACTION_FULL_BW);
	    break;
	case CSU_DSU_LOOPBACK_LINE:
	    if (loopback_type == CSU_DSU_LPBACK_TYPE_FR)
		dsu_loopback_command = DSU_LOCAL_PAYLOAD_LOOPBACK;
	    else
	        dsu_loopback_command = DSU_LOCAL_NET_LOOPBACK;
	    break;
	case CSU_DSU_SELFTEST:
	    dsu_loopback_command = DSU_SELF_TEST;
	    break;
	case CSU_DSU_LPBACK_NONE:
	    if (cd_info->current_loopback_command != 
		 			CSU_DSU_LOOPBACK_REMOTE) {
		dsu_loopback_command = DSU_LOOPBACK_NONE;
		if (cd_info->current_loopback_command != CSU_DSU_LPBACK_NONE) 
		    csu_dsu_loopback_finished(cd_info);
		break;
	    }
	    /* WARNING - fall through case for remote loopdown case */
	case CSU_DSU_LOOPBACK_REMOTE:

            /*
             * If we're already attempting a loopup or loopdown
             * then reject this new command
             */
            switch (cd_info->current_loopback_state) {
                case DSU_ATTEMPTING_LOOPUP:
                case DSU_ATTEMPTING_LOOPDOWN:
                    printf("\nModule attempting loop%s currently - try later",
                           (cd_info->current_loopback_state == DSU_ATTEMPTING_LOOPUP) ?
                           "up" : "down");
                    return (DSU_WRONG_FORMAT);
                default:
                    break;
            }

	    if (cd_info->dsu_remote_loopback_command) {
		result = (*cd_info->dsu_remote_loopback_command)(cd_info,
			   loopback_command, loopback_type, test_pattern,
			   user_pattern);
		return (result);
	    }
	    else {
		printf("\nService-module remote loopbacks not supported");
		return (DSU_WRONG_FORMAT);
	    }
	    break;
	default:
	    printf("\nLoopback undefined/not supported");
	    return (DSU_WRONG_FORMAT);
    }

    /*
     * Tell CSU/DSU to perform loopback test 
     */
    if ((result = csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                                      DSU_TEST_TYPE_SELECT_LEN, 
				      &dsu_loopback_command,
                                      WRITE_REGISTER)) == DSU_REQUEST_COMPLETE)  {
	cd_info->current_loopback_command = loopback_command;

	switch (loopback_command) {
	    case CSU_DSU_SELFTEST:
	        printf("\nSERVICE_MODULE(%d): Performing service-module self test", 
		       cd_info->interface_number);
	        GET_TIMESTAMP(cd_info->selftest_start_time);

	        /* Start timer to awaken background process */
	        if (!mgd_timer_running(&cd_info->dsu_diagnostic_timer)) 
		    mgd_timer_start(&cd_info->dsu_diagnostic_timer, DSU_SELFTEST_COMPLETION);
		break;
	    case CSU_DSU_LPBACK_NONE:
		break;
	    default:
		cd_info->idb->loopback = TRUE;
	        GET_TIMESTAMP(cd_info->loopback_start_time);
	        cd_info->dsu_loopback_type = loopback_type;
		cd_info->dsu_loopback_test_pattern = CSU_DSU_LPBACK_PAT_NOSEL;
	        printf("\nLoopback in progress");
		break;
        }
    }
    else {
	switch (loopback_command) {
	    case CSU_DSU_LPBACK_NONE: 
	        printf("\nFailed to exit loopback test - test still in progress");
		break;
	    case CSU_DSU_SELFTEST:
	        printf("\nFailed to perform service-module selftest");
		break;
	    default:
	        printf("\nLoopback test failed");
		break;	
	}
    }

    return (result);
}

/*
 * csu_dsu_init - If an integrated CSU/DSU is present, this initializes the
 *		  driver and returns a pointer to its data structure
 *		- If not, NULL is returned
 */
static void *csu_dsu_init (hwidbtype *idb, int which) 
{
    csu_dsu_instance_t *csu_dsu_info;
    hd64570_instance_t *ds;
    boolean power_cycle = FALSE;
    int module_type;
    int seconds_counter = 0;
    pid_t alarm_process_pid;

    if ((cookie->interfaces != INTERFACE_1E1B2W) && 
        (cookie->interfaces != INTERFACE_1R1B2W)) {
	/* used by the parser chain */
	idb->csu_dsu_type = CSU_DSU_NO_MODULE;
	return (NULL);
    }

    ds = INSTANCE;

     /*
      * If this is a Boa, add on the command to show current modular
      * WAN slots configuration
      */
    module_type = csu_dsu_wan_module_type(idb, which, FALSE);
    switch (module_type) {
	case CSU_DSU_SERIAL_5IN1:
	    idb->typestring = "HD64570 with 5-in-1 module";
	    idb->csu_dsu_type = module_type;
	    return (NULL);
	    break;
	case CSU_DSU_NO_MODULE:
	    return (NULL);
	    break;
	case CSU_DSU_SW56_2_WIRE:
	    idb->typestring = "HD64570 with 56k 2-wire CSU/DSU";
	    idb->csu_dsu_type = module_type | CSU_DSU_SW56;
	    break;   
	case CSU_DSU_SW56_4_WIRE:
	    idb->typestring = "HD64570 with 56k 4-wire CSU/DSU";
	    idb->csu_dsu_type = module_type | CSU_DSU_SW56;
	    break;
	case CSU_DSU_FT1:
	    idb->typestring = "HD64570 with FT1 CSU/DSU";
	    idb->csu_dsu_type = module_type;
	    break;
	case CSU_DSU_UNRECOGNISED:
	default:
	    buginf("\nUnrecognised module in serial %d", which);
	    return (NULL);
    }

    if (!(csu_dsu_info = csu_dsu_init_instance(idb, which, module_type))) 
	return (NULL);

    csu_dsu_info->module_init = TRUE;
    BOA_MASK_ALM_RDY_INTERRUPT(csu_dsu_info->wan_misc_reg);

    /*
     * Perform generic initialisation
     */
    if (first_time) {
       /*
        * Initialise the auto vectored interrupt and interrupt stack if necessary
        */

	*((uchar *) ADRSPC_BOA_IVR) = BOA_CSU_DSU_VECTOR;
   	if (!level_exists(BOA_DSU_IRQ_LEVEL)) {
	    /* 
             * Should never come to this as the interrupt stack for the modem
	     * control state change has been already created
	     */
	    createlevel(BOA_DSU_IRQ_LEVEL, NULL, "Integrated CSU-DSU interrupts");
	}
	exceptionHandler(BOA_CSU_DSU_VECTOR, level3_boa_csu_dsu_int);

	/* 
   	 * Set up the CSU/DSU alarm handler process common to both WAN slots
	 */
	alarm_process_pid = process_create(csu_dsu_ALM_background_process,
					   "Service-module alarm handler",
					   NORMAL_STACK, PRIO_NORMAL);

	/* Initialize w_boolean used to awaken alarm handler process */
	alarm_process_awaken = create_watched_boolean("Service-module alarm", 
						      W_BOOLEAN_ALM_TYPE);

	if ((alarm_process_pid == NO_PROCESS) || !alarm_process_awaken) {
	    errmsg(&msgsym(INITFAILURE, SERVICE_MODULE), which); 
	    crashdump(1);
	}

	/* Initialize the parent managed timer for remote loopbacks */
	mgd_timer_init_parent(&parent_rem_lpback_timer, NULL);

	first_time = FALSE;
     }

     /* Init scheduler semaphore that serializes requests to CSU/DSU */
     csu_dsu_info->csu_dsu_lock = create_watched_semaphore("Service-module lock", 
							   (which == 0) ? 
							   SEMAPHORE_DSU0_TYPE :
							   SEMAPHORE_DSU1_TYPE);

     if (!csu_dsu_info->csu_dsu_lock) {
	 errmsg(&msgsym(INITFAILURE, SERVICE_MODULE), which);
	 crashdump(1);
     }
    
     /*
      * Init the leaf timer used for initiating remote loopbacks
      */
     mgd_timer_init_leaf(&csu_dsu_info->dsu_diagnostic_timer, 
			 &parent_rem_lpback_timer, 
			 (which == 0) ? WAN_PORT_0 : WAN_PORT_1,  
			 csu_dsu_info, FALSE);
 
     /*
      * If this was power-cycle, prevent further hardware RESETS to the CSU/DSU
      * by setting a bit in the wan miscellanous register
      */
     if (!(*csu_dsu_info->wan_misc_reg & BOA_MISC_DSU_HW_RST_MASK)) 
	power_cycle = TRUE;

     *csu_dsu_info->wan_misc_reg |=  BOA_MISC_DSU_HW_RST_MASK; 

     /*
      * At this point we need to wait for the module ready to perform reg reads
      */
     while (!RDY_SIGNAL_HIGH(csu_dsu_info->wan_misc_reg)) {
         DELAY(ONESEC);
	 if (++seconds_counter >= DSU_MAX_POWERCYCLE_TIME) {
	    csu_dsu_info->module_init = FALSE;
	    reset_csu_dsu(csu_dsu_info, FALSE);
	    return (csu_dsu_info);
	 }
     }

     csu_dsu_info->module_init = FALSE;

     if (power_cycle) 
         csu_dsu_bringup_procedure(csu_dsu_info, NULL);
     else
	 csu_dsu_bringup_procedure(csu_dsu_info, csu_dsu_set_default_state);

     return (csu_dsu_info);
}   

/*
 * CSU-DSU subsystem initialisation entry point
 */
void csu_dsu_subsys_init (subsystype *subsys)
{
    /*
     * Add registry called during idb creation of HD64570 serial interfaces
     * to initialize CSU/DSU driver
     */
    reg_add_csu_dsu_init(csu_dsu_init, "service-module init task");

    /*
     * For efficiency, register these functions only we're a Boa
     */
    if ((cookie->interfaces == INTERFACE_1E1B2W) ||
	 (cookie->interfaces == INTERFACE_1R1B2W)) {
	reg_add_show_int_enhanced(serial_show_int_enhanced,
                                  "serial_show_int_enhanced");
        reg_add_csu_dsu_installed(csu_dsu_csu_dsu_installed,
                                  "csu_dsu_csu_dsu_installed");
        reg_add_csu_dsu_loopback_command(csu_dsu_service_module_command,
                                         "csu_dsu_loopback_command");
        reg_add_csu_dsu_validate_usr_pat(csu_dsu_validate_usr_pat,
                                         "csu_dsu_validate_usr_pat");
        reg_add_csu_dsu_show_cable_state(csu_dsu_show_cable_state,
                                         "csu_dsu_show_cable_state");
        reg_add_csu_dsu_get_status_value(csu_dsu_get_status_value,
                                         "csu_dsu_get_status_value");
        csu_dsu_parser_init();
        csu_dsu_debug_init();
    }
}

#define CSU_DSU_MAJVERSION	1
#define CSU_DSU_MINVERSION	0
#define CSU_DSU_EDITVERSION	1

SUBSYS_HEADER(integrated_csu_dsu, CSU_DSU_MAJVERSION, CSU_DSU_MINVERSION, 
	      CSU_DSU_EDITVERSION, csu_dsu_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL, "req: hd64570");
