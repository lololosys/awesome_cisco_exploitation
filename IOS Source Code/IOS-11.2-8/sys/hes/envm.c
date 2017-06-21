/* $Id: envm.c,v 3.6.46.6 1996/08/28 12:43:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/envm.c,v $
 *------------------------------------------------------------------
 * envm.c --  Environmental Monitor Function drivers
 *
 * 1-Jul-1992, Gary Kramling  adapted from ec.c 
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * - Monitor Environment in cisco 7000
 *
 *------------------------------------------------------------------
 * $Log: envm.c,v $
 * Revision 3.6.46.6  1996/08/28  12:43:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.46.5  1996/08/22  00:22:34  ramk
 * CSCdi65473:  another crash in _k_ciscoEnvMonSupplyStatusEntry_get (11.0)
 * Branch: California_branch
 *
 * Revision 3.6.46.4  1996/08/07  08:58:41  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.46.3  1996/06/28  23:06:10  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.46.2  1996/05/21  06:29:07  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.46.1  1996/03/18  19:40:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.16.2  1996/03/07  09:38:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.16.1  1996/01/24  21:45:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/07  22:28:27  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/12/01  19:03:37  sdurham
 * CSCdi44362:  envmon is non-modular w.r.t envmon mib.
 *   Remove direct calls from envmon code to mib code. Use service points.
 *
 * Revision 3.4  1995/11/24  16:38:28  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.3  1995/11/17  09:15:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/21  07:39:57  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.5  1995/06/28 21:16:49  sdurham
 * CSCdi36261:  CISCO-ENVMON-MIB and OLD-CISCO-ENV-MIB being loaded on ags
 *              improperly.
 *
 * Revision 2.4  1995/06/28  09:24:26  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  23:16:43  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  04:27:29  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:39:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/chassismib_registry.h"
#include "../os/clock.h"
#include "envm.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "../../boot/src-68-c7000/pcmap.h"
#include "config.h"
#include "powerfail.h"
#include "logger.h"
#include "envmib_registry.h"
#include "../hes/envmon_parser.h"
#include "../hes/sr_envmibdefs.h"
#include "msg_envm.c"			/* Not a typo, see logger.h */

/* Rename Address Space defines from Rom Monitor */
#define ENV_CTL                 ADRSPC_ENV_CTL
#define ENV_STS                 ADRSPC_ENV_STS
#define ENV_NVRAM               ADRSPC_ENV_NVRAM

extern uchar *nv_onboard;
extern rp1_buffer(uchar *base, uchar *dest, long size);
extern long *boot_data;

static chassis_type_t chassis;                  /* 7000 or 7010 */

static ecshow_t *read_env_nvram(uchar*);
static process env_watch_process(void);
static void env_warning_check(ecshow_t*);
static ecshow_t *env_get_show(void);
static void env_show_str_init(ecshow_t*);
static void show_power_supply(ecshow_t *);
static void show_voltage(ecshow_t *);
static void show_temperature(ecshow_t *);
static char *get_power_supply_str(ulong);
static int get_ps_bits(ulong, int);
static void show_who_shutdown(ecshow_t *);

static powerfail_info_t *pf_information;

static const voltage_bounds_t volt_bounds[5] = {
    {    0,    0,    0,    0,   0,    0},	/* ECTEST_TEST bounds data */
    {   92,  164,   92,  164,  80,  176},	/* ECTEST_V_P12 bounds data */
    {  115,  141,  103,  153,  90,  166},	/* ECTEST_V_P5 bounds data */
    {   92,  164,   92,  164,  80,  176},	/* ECTEST_V_N12 bounds data */
    {   88,  168,   80,  176,  68,  188}	/* ECTEST_V_P24 bounds data */
};

/*
 * 7000 lower temperature bounds for NORMAL, WARNING, CRITICAL, and SHUTDOWN
 */
static const uint temp_bounds_7000[2][4] = {
    {  161,  211,  221,  239},	/* ECTEST_T_EXHAUST_LOW bounds data */
    {  161,  190,  197,  215}	/* ECTEST_T_INLET_LOW bounds data */
};

/*
 * 7010 lower temperature bounds for NORMAL, WARNING, CRITICAL, and SHUTDOWN
 */
static const uint temp_bounds_7010[2][4] = {
    {  161,  221,  228,  239},	/* ECTEST_T_EXHAUST_LOW bounds data */
    {  161,  190,  197,  215}	/* ECTEST_T_INLET_LOW bounds data */
};

static char *const state_str[] = {
   "NORMAL",
   "WARNING",
   "CRITICAL",
   "SHUTDOWN"
};

static const print_bounds_t print_bounds[7] = {
    {  0,  0,  0,  0,  0},	/* Test data */
    { 12,  8, 12, 10,  5},	/* +12V data */
    {  5, 28,  4, 25,  2},	/* + 5V data */
    { 12,  8, 12, 10,  5},	/* -12V data */
    { 24,  8, 24,  5, 10},	/* +24V data */
    {  0,  0,  0,  0,  0},	/* Reserved 1*/
    {  0,  0,  0,  0,  0}	/* Reserved 2*/
};

static char *const c7000_pwr_supply_str[] = {
    "???W",
    "700W DC",
    "700W AC",
    "Not Installed"
};

static char *const spinnaker_pwr_supply_str[] = {
    "???W",
    "600W AC",
    "600W DC",
    "???W"
};

static char *const c7000_ps_location_str[] = {
    "Upper",                          /*  1 Upper Supply               */
    "Lower"                           /*  2 Lower Supply               */
};

boolean envmib_load = TRUE;     /* global to tell mibcode ok to load */

/*
 * envm_init
 * ENV init routine called by subsystem initialization
 */
static void envm_init (subsystype *subsys)
{
    ec_present = 0;             /* used by Print_Hardware */
    if (system_loading)
	return;		/* don't run environmental monitor if netbooting */
    
    if (ecp) {
	memset(ecp, 0, sizeof(*ecp));
    }
    else if (!(ecp = malloc_named(sizeof(ec_t), "ENV Controller"))) {
	return;
    }

    chassis = snmp_platform_get_value(SNMP_PLATFORM_CHASSIS_TYPE);

    /*
     * Grab (2) hunks of memory to read env parameters into from nvram
     */
    if (!(last_env_data = malloc_named(ENV_NVMAXBYTES, "ENVM Data")))
	return;
    
    if (!(last_env_data2 = malloc_named(ENV_NVMAXBYTES, "ENVM Data")))
	return;
    
    memset(ecp, 0, sizeof(*ecp));
    
    ecp->magic   = ENV_MAGIC;
    ecp->version = ENV_NV_VERSION;
    ecp->invalid = ENV_VALID;
    ecp->chassis = chassis;
    
    /* Grab last env parameters (if any) from nvram and make available
     * for [show env last] command
     */
    last_showp = read_env_nvram(last_env_data);
    
    /*
     * Now spawn the process that will do the periodic environment measurements
     */
    process_create(env_watch_process, "Env Mon", NORMAL_STACK, PRIO_NORMAL);

    envmon_parser_init();
}

/*
 * env_showp_getptr
 * Copy env data from nvram to main memory while checking the MAGIC number
 * and checksum to verify the data is good.
 * Return pointer to the recently created ecshow_t data in RAM
 */
static ecshow_t *env_showp_getptr (uchar *tmp_data)
{
    ec_t *tmp_new_nvptr;
    long env_nvsize;

    env_nvsize = (long)ENV_NVMAXBYTES;
    if (nv_onboard) {    /* We know there is nvram from nv_init() */
        rp1_buffer((uchar *)ENV_NVRAM, (uchar *)tmp_data, env_nvsize);
        tmp_new_nvptr = (ec_t *)tmp_data;
        if (tmp_new_nvptr->magic == ENV_MAGIC) {
	    if (tmp_new_nvptr->checksum ==  
		ipcrc((ushort *)(tmp_data+sizeof(ec_t)), sizeof(ecshow_t))) {
	        /* adjust to real pointer */
		tmp_new_nvptr->textbase += (ulong)tmp_data;
		return ((ecshow_t *)tmp_new_nvptr->textbase);
	    }
        }
        return (NULL);
    } else
        return (NULL);  
}

/*
 * env_nv_done
 * Copies env data from main memory to nvram.
 * Generates new checksum after computing ecshow_t offset from nvram
 * base address
 */
static void env_nv_done (ecshow_t *tmp_ecshow_ptr, ec_t *ptr_tmp_ec_t)
{
    ptr_tmp_ec_t->textbase = (uchar *)sizeof(ec_t); /* make into offset */
    ptr_tmp_ec_t->checksum = 0;
    ptr_tmp_ec_t->checksum= ipcrc((ushort *)tmp_ecshow_ptr, sizeof(ecshow_t));
    rp1_buffer((uchar *)ptr_tmp_ec_t, (uchar *)ENV_NVRAM, (long)sizeof(ec_t));
    rp1_buffer((uchar *)tmp_ecshow_ptr, (uchar *)ENV_NVRAM+sizeof(ec_t), 
	       (long)sizeof(ecshow_t));
}

/*
 * write_env_nvram
 * Check nvram is present and calls routine to copy env data to nvram.
 */
static void write_env_nvram (ecshow_t *tmp_ecshow_ptr, ec_t *ptr_tmp_ec_t)
{
    if (nv_onboard)      /* We know there is nvram from nv_init() */
	env_nv_done(tmp_ecshow_ptr, ptr_tmp_ec_t);
}

/*
 * mark_env_hdr_dirty
 * Mark env data in nvram as either DIRTY or INVALID based on status.
 * This routine is needed so outdated or bad data is not made available
 * for the [show env last] command.
 */
static void mark_env_hdr_dirty (ec_t *tmp_ecp_ptr, uchar status)
{
    tmp_ecp_ptr->invalid = status;
    tmp_ecp_ptr->textbase = (uchar *)sizeof(ec_t); /* make into offset */
    rp1_buffer((uchar *)tmp_ecp_ptr, (uchar *)ENV_NVRAM, (long)sizeof(ec_t));
    tmp_ecp_ptr->textbase += (ulong)tmp_ecp_ptr;
}

/*
 * read_env_nvram
 * Check data from nvram is good by looking at the version and dirty bit.
 * Return pointer to ecshow_t struct if good.
 * If were coming up on a reload and data had been marked dirty,
 * (ie. it has already been read) provide the data again.
 */
ecshow_t *read_env_nvram (uchar *tmp_data)
{
    ecshow_t *tmp_ecshow_ptr;
    ec_t *tmp_ecp_ptr;
    
    tmp_ecp_ptr = (ec_t *)tmp_data;
    tmp_ecshow_ptr = env_showp_getptr(tmp_data);
    if (tmp_ecshow_ptr) {
	if (tmp_ecp_ptr->chassis != chassis)
	    mark_env_hdr_dirty(tmp_ecp_ptr, (uchar)ENV_INVALID);
	if ((tmp_ecp_ptr->version == ENV_NV_VERSION) &&
	    (tmp_ecp_ptr->invalid == ENV_VALID)) {
	    mark_env_hdr_dirty(tmp_ecp_ptr, (uchar)ENV_DIRTY);
	    return (tmp_ecshow_ptr);
	} else if ((tmp_ecp_ptr->version == ENV_NV_VERSION) &&
		   (tmp_ecp_ptr->invalid == ENV_DIRTY) && 
		   (boot_data[BOOT_REASON] == EXC_RELOAD)) {
	    return (tmp_ecshow_ptr);
	} else if ((tmp_ecp_ptr->version == ENV_NV_VERSION) &&
		   (tmp_ecp_ptr->invalid == ENV_DIRTY) && 
		   (boot_data[BOOT_REASON] != EXC_RELOAD)) {
	    mark_env_hdr_dirty(tmp_ecp_ptr, (uchar)ENV_INVALID);
	}
    } else {
	return (NULL);  
    } 
    return (NULL);  
}

/*
 * handle_env_powerfail
 * This is called at non-maskable level-7 from the refresh() routine
 * to allow us to write out the current env paramaters to nvram so
 * when the router goes down, this data will still be availble upon
 * the next reboot.
 */
static void handle_env_powerfail (void)
{
    volatile ulong env_result;
    volatile ulong *env_status;
    
    env_status = (u_long *)ENV_STS;
    
    /* read last env data from nvram */
    last_showp2 = env_showp_getptr(last_env_data2);
    
    /* write out current env data to nvram because shutdown is likely */
    write_env_nvram(ecp->ec_show, ecp);
    reg_invoke_envmib_shutdown_trap();
    /* Spin... while checking the powerfail bits to see if the condition
     * ever goes away.  If it doesnt, then in approx 15ms. the power will
     * go away and we will have saved the last env data for examination
     * when we come up next time.  If we notice the powerfail condition
     * go away, then it was just an intermitant powerfail and we want
     * to back out of this driver and return control to the system.
     */
    do {
        env_result = *env_status;
    } while ((env_result & SUPPLY1_PF) && (env_result & SUPPLY2_PF));
    
    /* Got here, so it was an intermittant powerfail */
    if (pf_information) {
	pf_information->intermittent_pf_count++; /* got intermittent pf */
	GET_TIMESTAMP(pf_information->time_of_last_pf);/* time of last pwrfl */
	current_time_string(pf_information->pf_date_time_snap);
        if (last_showp2)
            /* what was in nvram previously was good, so write back out */
	    write_env_nvram(last_showp2, (ec_t *) last_env_data2);
        else
            /*
             * What was in nvram previously was bad, so invalidate new
             * env struct and write out so this data is never provided
             * via the [show env last] command.
             */
	    mark_env_hdr_dirty(ecp, (uchar)ENV_INVALID);
    }
}

/*
 * processor_shutdown
 * The software has determined that an env. measurement has reached the
 * SHUTDOWN threshold, and is shuting down the power supplies to avoid
 * any physical damage coming to the router.
 */
static void processor_shutdown (int indx)
{
    volatile ushort *env_control;
    
    reg_invoke_envmib_shutdown_trap();
    logger_flush();
    process_suspend();

    DELAY(ONESEC);
    env_control = (ushort *)ENV_CTL;
    if (pf_information) {
	pf_information->pf_addr = (ulong) 0; /* ignore further power fails */
	write_env_nvram(ecp->ec_show, ecp);  /* save last env data */
	*env_control = POLARITY_MSK | ENV_SHUTDOWN; /* shutdown */
    }
}

/*
 * update_env_stats
 * Queries for ENV voltage/temperature information from ADC.
 * Checks that all measurements fall within designated thresholds
 * and provides warnings if necessary. Stores all information in 
 * alloc'd struct to be made available to "show env" commands.
 * This routine is invoked once at initialization and periodically
 * from start_env_update(). (once per minute)
 */
static void update_env_stats (uchar *last_fan_state)
{
    ecshow_t *showp, *tmp;
    volatile ushort *env_control;
    volatile ulong *env_status;
    ulong env_result;
    uchar Power_Supply_Control = 0;
    int i, sense;
    
    env_control = (ushort *)ENV_CTL;
    env_status = (u_long *)ENV_STS;
    if ((showp = malloc(sizeof(ecshow_t)))) {
	
        /* Provide strings which describe senses */
	env_show_str_init(showp);
	
        /* 
	 * Query voltage senses 
         */
        for (sense = ECTEST_V_P12; sense <= ECTEST_V_P24; sense++) {
            *env_control = sense | POLARITY_MSK | Power_Supply_Control;
	    process_sleep_for(ONESEC/100); /* dismiss for 10 ms if scheduled */
            env_result = *env_status;
            showp->s_test[sense].t_measured = SENSE_BITS(env_result);
        }
	
        /* 
	 * Query temperature senses 
	 */
	for (sense = ECTEST_T_EXHAUST, i = 0; i < 2; i++) {
	    *env_control = sense | POLARITY_MSK | Power_Supply_Control;
	    process_sleep_for(ONESEC/100); /* dismiss for 10 ms if scheduled */
	    env_result = *env_status;
	    showp->s_test[sense].t_measured = SENSE_BITS(env_result); 
	    sense = ECTEST_T_INLET;
	}

        /* 
	 * Query type and status of power supply(s) 
	 * Spinnaker only has the first power supply 
	 */
	for (i=0; i < MAX_PWR_INSTALLED; i++) {
	    showp->ps[i].type = get_ps_bits(env_result, i);
	    showp->ps[i].powerfail_status = PWR_FAIL_BIT(env_result, i);
	    if ((showp->ps[i].type != NOT_INSTALLED) 
		&& showp->ps[i].powerfail_status) {
		if (ecp->ec_show) {
		    if (AWAKE(ecp->ec_show->ps[i].clock)) {
			/* Only send errmsg to console if timer has expired */
			errmsg(&msgsym(SUPPLY, ENVM), c7000_ps_location_str[i]);
			TIMER_START(showp->ps[i].clock, WARN_REFRESH*ONEMIN);

			/* update global early for trap send */
			ecp->ec_show->ps[i].powerfail_status= 
			    showp->ps[i].powerfail_status;

			/* indexing of power supplies by snmp starts at 1 */
			reg_invoke_envmib_redundant_supply_trap(i+1);
		    } else {
		/* Conditon already existed but timer had not expired */
			showp->ps[i].clock = ecp->ec_show->ps[i].clock;
		    }
		}
	    }
	    showp->ps[i].type_str = get_power_supply_str(showp->ps[i].type);
	}

	/* 
	 *  Query the status of the fan array if this is a Spinnaker chassis.
	 *  Also load showp->last_fan_state with the latest state of all 
	 *  6 Spinnaker fans. 
         */
	if (chassis == D_chassisType_c7010) {
	    for (sense = ECTEST_FAN_0; sense <= ECTEST_FAN_5; sense++) {
		*env_control = sense | POLARITY_MSK | Power_Supply_Control;
		process_sleep_for(ONESEC/100);
		env_result = *env_status;
		showp->s_test[sense].t_measured = SENSE_BITS(env_result);
	    }
	}
	showp->last_fan_state = *last_fan_state;
	
        current_time_string(showp->date_time_snap);
	GET_TIMESTAMP(showp->ms_snapshot);
	
        /* 
         * See if we had an intermittant powerfail since last query 
         */
	if (pf_information) {
	    showp->int_powerfail_count = pf_information->intermittent_pf_count;
	    if (showp->int_powerfail_count) {
                strcpy(showp->pf_date_time_snap,
		       pf_information->pf_date_time_snap);
            }
	    COPY_TIMESTAMP(pf_information->time_of_last_pf,
			   showp->int_pwr_fail_time);
        }
	
        /* 
         * Check to see if measurements fall within prescribed tolerances 
         */
	env_warning_check(showp);

        /*
	 * Load last_fan_state with the new state of the spinnaker fans
	 * resulting from the last update.
	 */
        *last_fan_state = showp->last_fan_state;

        tmp = ecp->ec_show;
        ecp->ec_show = showp;
        if (tmp)
	    free(tmp);

        /*
         * who_shutdown is set if any sense measured reached the SHUTDOWN
         * threshold.  Turn off power supplies to avoid physical damage
         * coming to the router.
         */
        if (ecp->ec_show->who_shutdown)
	    processor_shutdown(ecp->ec_show->who_shutdown);
    }
}

/*
 * env_watch_process
 *
 * A process that does nothing but check the environmental monitor
 * once a minute.  This cannot be run from the one minute timer hook
 * because it may dismiss.
 */
process env_watch_process (void)
{
    /*
     * last_fan_state is declared here to correct CSCdi67019.  This variable
     * must be maintained across periodic calls to update_env_stats.  It
     * will store the state of all spinnnaker 7010 fans.
     */

    uchar last_fan_state = NORMAL;   

    /* Make sure to set pf_addr after update_env_stats() is called 
     * for the first time, so ecp->ec_show is initialized before 
     * we acknowledge any powerfails
     */

    update_env_stats(&last_fan_state);
    pf_information = (powerfail_info_t*)mon_get_powerfail_addr();
    if ((long)pf_information > 0)
	pf_information->pf_addr = (ulong)handle_env_powerfail;
    else
	pf_information = 0;	/* no EMT support, assume older rom monitor */

    /*
     * Now update them once a minute
     */
    while (TRUE) {
	update_env_stats(&last_fan_state);
	process_sleep_for(ONEMIN);
    }
}

/*
 * get_ps_bits
 * get the 2 power supply type bits out of the envm result
 */
int get_ps_bits (ulong result, int ps_num)
{
    int bits;

    /*
     *  For the first p/s the type bits are bits 16 and 30,
     *  for the second p/s the type bits are bits 17 and 31
     */

    result >>= ps_num;

    bits = (result & 0x00010000) >> 16;
    bits = (bits | ((result & 0x40000000) >> 29));

    return bits;
}

/*
 * get_power_supply_str
 * return a string for the p/s type
 */
char *get_power_supply_str (ulong ps_type)
{
    if (ps_type > NOT_INSTALLED)
	return NULL;
    
    switch (chassis) {
    case D_chassisType_c7000:
	return c7000_pwr_supply_str[ps_type];
    case D_chassisType_c7010:
	return spinnaker_pwr_supply_str[ps_type];
    default:
	return NULL;
    }
}
	
/*
 * convert_volts
 * Take a raw value from ADC and use conversion table to generate a string
 * that can be used to display the voltage in floating point notation.
 */
static void convert_volts (char *voltage_tmp_str, u_long measure, int indx)
{
    int  j;
    ulong tmp_decimal;
    
    for (j = 1; j <= print_bounds[indx].range; j++) {
	if (measure < print_bounds[indx].lower_bound +
	    print_bounds[indx].divisions * 2 * j) {
	    tmp_decimal = ((measure -
			    (print_bounds[indx].lower_bound +
			     print_bounds[indx].divisions * 2 *
			     (j-1))) * print_bounds[indx].resolution);
	    sprintf(voltage_tmp_str, "%s%d.%02d",
		    (indx == ECTEST_V_N12) ? "-" : " ",
		    print_bounds[indx].voltage_line -
		    (print_bounds[indx].voltage_line / 2) + j - 1,
		    tmp_decimal);
            return;
	}
    }
}

int convert_millivolts (u_long measure, int indx)
{
    int  j;
    ulong tmp_decimal;
    
    for (j = 1; j <= print_bounds[indx].range; j++) {
        if (measure < print_bounds[indx].lower_bound +
	    print_bounds[indx].divisions * 2 * j) {
	    
            tmp_decimal = ((indx == ECTEST_V_N12) ? -1 : 1) *
		((print_bounds[indx].voltage_line -
		  (print_bounds[indx].voltage_line / 2) + j - 1) * 1000 +
		 (((measure - (print_bounds[indx].lower_bound +
			       print_bounds[indx].divisions * 2 *
			       (j-1))) * print_bounds[indx].resolution)*10));
            return(tmp_decimal);
        }
    }
    return(0);
}

/*
 * resolve_temp_warning
 * Provide errmsg to console if temperature is not within NORMAL range.
 */
static void resolve_temp_warning (ecshow_t *showp, int indx)
{
    switch (showp->s_test[indx].t_state) {
    case NORMAL:
        if (ecp->ec_show) {
	    COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			   showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = ecp->ec_show->s_test[indx].t_orig_state;
        } else {
	    TIMER_STOP(showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = 0;
        }
	break;
    case WARNING:
    case CRITICAL:
	if (ecp->ec_show) {
	    if (showp->s_test[indx].t_state > ecp->ec_show->s_test[indx].t_orig_state)
		TIMER_STOP(ecp->ec_show->s_test[indx].t_clock);
	    if (AWAKE(ecp->ec_show->s_test[indx].t_clock)) {
                /* Only send errmsg to console if timer has expired */
                errmsg(&msgsym(TEMP, ENVM),
		       showp->s_test[indx].t_description,
		       state_str[showp->s_test[indx].t_state],
		       CELSIUS(showp->s_test[indx].t_measured));
		TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
		showp->s_test[indx].t_orig_state = showp->s_test[indx].t_state;
		/* update global for trap varbinds */
		ecp->ec_show->s_test[indx].t_state = 
		    showp->s_test[indx].t_state;
		ecp->ec_show->s_test[indx].t_measured =
			showp->s_test[indx].t_measured; 

		reg_invoke_envmib_temperature_trap(indx);
	    } else {
                /* Conditon already existed but timer had not expired */
		COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			       showp->s_test[indx].t_clock);
		showp->s_test[indx].t_orig_state = 
		    ecp->ec_show->s_test[indx].t_orig_state;
	    }
	}
	break;
    case SHUTDOWN:
	if (ecp->ec_show) {
	    if (showp->s_test[indx].t_state > ecp->ec_show->s_test[indx].t_orig_state)
		TIMER_STOP(ecp->ec_show->s_test[indx].t_clock);
	    showp->who_shutdown = (uchar)indx; /* need to shutdown router */
	    if (AWAKE(ecp->ec_show->s_test[indx].t_clock)) {
                /* Only send errmsg to console if timer has expired */
                errmsg(&msgsym(SHUTDOWN, ENVM));
                errmsg(&msgsym(TEMP, ENVM),
		       showp->s_test[indx].t_description,
		       state_str[showp->s_test[indx].t_state],
		       CELSIUS(showp->s_test[indx].t_measured));
		TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
		showp->s_test[indx].t_orig_state = showp->s_test[indx].t_state;
		/* update global for trap varbinds */
		ecp->ec_show->s_test[indx].t_state = 
		    showp->s_test[indx].t_state;
		ecp->ec_show->s_test[indx].t_measured =
			showp->s_test[indx].t_measured; 
		reg_invoke_envmib_temperature_trap(indx);

	    } else {
                /* Conditon already existed but timer had not expired */
		COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			       showp->s_test[indx].t_clock);
		showp->s_test[indx].t_orig_state = ecp->ec_show->s_test[indx].t_orig_state;
	    }
	}
	break;
    }
}

/*
 * resolve_voltage_warning
 * Provide errmsg to console if voltage is not within NORMAL range.
 */
static void resolve_voltage_warning (ecshow_t *showp, int indx)
{
    char tmp_str[20];
    
    switch (showp->s_test[indx].t_state) {
    case NORMAL:
        if (ecp->ec_show) {
	    COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			   showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = ecp->ec_show->s_test[indx].t_orig_state;
        } else {
	    TIMER_STOP(showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = 0;
        }
	break;
    case WARNING:
    case CRITICAL:
	if (ecp->ec_show) {
	    if (showp->s_test[indx].t_state > ecp->ec_show->s_test[indx].t_orig_state)
		TIMER_STOP(ecp->ec_show->s_test[indx].t_clock);
	    if (AWAKE(ecp->ec_show->s_test[indx].t_clock)) {
                /* Only send errmsg to console if timer has expired */
		convert_volts(tmp_str, showp->s_test[indx].t_measured, indx);
                errmsg(&msgsym(VOLTAGE, ENVM),
		       showp->s_test[indx].t_description,
		       state_str[showp->s_test[indx].t_state],
		       tmp_str);
		TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
		showp->s_test[indx].t_orig_state = showp->s_test[indx].t_state;
		/* update global for trap varbinds */
		ecp->ec_show->s_test[indx].t_state = 
		    showp->s_test[indx].t_state; 
		ecp->ec_show->s_test[indx].t_measured =
			showp->s_test[indx].t_measured; 
		reg_invoke_envmib_voltage_trap(indx);

	    } else {
                /* Conditon already existed but timer had not expired */
		COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			       showp->s_test[indx].t_clock);
		showp->s_test[indx].t_orig_state = ecp->ec_show->s_test[indx].t_orig_state;
	    }
	}
	break;
    case SHUTDOWN: /* SHUTDOWN is currently not a valid voltage state */
	break;
    }
}

/*
 * resolve_fan_warning
 * Provide errmsg to console if fan is not within NORMAL range.
 */
static void resolve_fan_warning (ecshow_t *showp, int indx)
{
    switch (showp->last_fan_state) {
    case NORMAL:
        if (ecp->ec_show) {
	    COPY_TIMESTAMP(ecp->ec_show->s_test[indx].t_clock,
			   showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = ecp->ec_show->s_test[indx].t_orig_state;
        } else {
	    TIMER_STOP(showp->s_test[indx].t_clock);
	    showp->s_test[indx].t_orig_state = 0;
        }
	break;
    case WARNING:
	if (ecp->ec_show) {
	    errmsg(&msgsym(FAN, ENVM), 
		   state_str[showp->last_fan_state]);  /* 2 minute warning */
	    TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
	    showp->s_test[indx].t_orig_state = showp->last_fan_state;
	    /* update global for trap varbinds */
	    reg_invoke_envmib_fan_trap(indx);
	}
	break;
    case CRITICAL:
	if (ecp->ec_show) {
	    errmsg(&msgsym(FAN, ENVM), 
		   state_str[showp->last_fan_state]);  /* 1 minute warning */
	    TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
	    showp->s_test[indx].t_orig_state = showp->last_fan_state;
	    /* update global for trap varbinds */
	    reg_invoke_envmib_fan_trap(indx);
	}
	break;
    case SHUTDOWN:
	if (ecp->ec_show) {
	    showp->who_shutdown = (uchar)indx;  /* need to shutdown router */
	    errmsg(&msgsym(FAN, ENVM), 
		   state_str[showp->last_fan_state]);  /* no more warnings */
	    errmsg(&msgsym(SHUTDOWN, ENVM));
	    TIMER_START(showp->s_test[indx].t_clock, WARN_REFRESH*ONEMIN);
	    showp->s_test[indx].t_orig_state = showp->last_fan_state;
	    /* update global for trap varbinds */
	    reg_invoke_envmib_fan_trap(indx);
	}
	break;
    }
}

/*
 * env_warning_check
 * Check to see if any senses are not within prescribed tolerances.
 */
void env_warning_check (ecshow_t *showp)
{
    int i, sense;
    int fan_failure;
    
    /* 
     * Check voltages
     */
    for (sense = ECTEST_V_P12; sense <= ECTEST_V_P24; sense++) {
	if ((showp->s_test[sense].t_measured >= volt_bounds[sense].normal_low)
	    &&(showp->s_test[sense].t_measured <= volt_bounds[sense].normal_high))
	    showp->s_test[sense].t_state = NORMAL;
	else 
	    showp->s_test[sense].t_state = CRITICAL;
	resolve_voltage_warning(showp, sense);
    }
    
    /* 
     * Check temperatures
     */ 
    for (sense = ECTEST_T_EXHAUST, i = 0; i < 2; i++) {
	if (showp->s_test[sense].t_measured != 0) {
	    if (showp->s_test[sense].t_measured < env_temp_bounds(sense,NORMAL))
		showp->s_test[sense].t_state = WARNING;
	    else if (showp->s_test[sense].t_measured < env_temp_bounds(sense,WARNING))
		showp->s_test[sense].t_state = NORMAL;
	    else if(showp->s_test[sense].t_measured < env_temp_bounds(sense,CRITICAL))
		showp->s_test[sense].t_state = WARNING;
	    else if(showp->s_test[sense].t_measured < env_temp_bounds(sense,SHUTDOWN))
		showp->s_test[sense].t_state = CRITICAL;
	    else 
		showp->s_test[sense].t_state = SHUTDOWN;
	    resolve_temp_warning(showp, sense);
	}
	sense = ECTEST_T_INLET;
    }
    
    /*
     * Check for fan failure.  Store state for all 6 fans into showp->last_fan_state.
     * Reset state if all fans are OK.
     */

    if (chassis == D_chassisType_c7010) {
	for (fan_failure=0, sense=ECTEST_FAN_0; sense <= ECTEST_FAN_5; sense++) {
	    if (showp->s_test[sense].t_measured) {    
		fan_failure++;
		showp->s_test[sense].t_state = WARNING;
	    } else {
		showp->s_test[sense].t_state = NORMAL;
	    }
	}

	if (!ecp || !ecp->ec_show)
	    showp->last_fan_state = NORMAL;

        /*
	 * If any of the fans have failed, increment showp->last_fan_state to the
	 * next level (NORMAL, WARNING, CRITCAL, SHUTDOWN). 
	 */

	if (fan_failure) {
 	    ++showp->last_fan_state;         
	} else {
	    if (showp->last_fan_state != NORMAL) 
  		errmsg(&msgsym(FANOK, ENVM));  		
	    showp->last_fan_state = NORMAL;
  	}

	resolve_fan_warning(showp, ECTEST_FAN_0);

        /* 
	 * Store the cumulative fan state (NORMAL, WARNING, CRITCAL, SHUTDOWN)
	 * into ecp->ec_show->last_fan_state.
	 * ecp->ec_show->last_fan_state will be written to nvram if the 
	 * chassis shuts down.  This will allow "show env last" to 
	 * recover the proper last fan state.
	 */

        if (ecp->ec_show) {
	    ecp->ec_show->last_fan_state = showp->last_fan_state;
	}
    }
}

/*
 * show_env
 * Primary means of displaying environmental data.
 */
void show_env (parseinfo *csb)
{
    int i, sense, snapshot_age, any_warning = FALSE;
    ecshow_t *showp;
    static char *envm = "Environmental Function";
    char tmp_str[20];
    
    i = 0; /* silly compiler thinks 'i' may be used before initialized */

    if (!ecp) {
	printf("%s never started", envm);
	return;
    }
    
    /* 
     * Get the last environmental measurements 
     */
    showp = env_get_show();
    if (showp) {
	automore_enable(NULL);
        printf("\nEnvironmental Statistics\n");
        printf("  Environmental status as of %s",showp->date_time_snap);
	snapshot_age = ELAPSED_TIME(showp->ms_snapshot) / ONESEC;
	printf("\n  Data is %d second(s) old, refresh",snapshot_age);
	if (ENV_REFRESH > snapshot_age) 
	    printf(" in %d second(s)",ENV_REFRESH - snapshot_age);
        else
	    printf(" momentarily");
	
	/*
	 * For systems with redundant power supplies, check if
	 * installed, but not operational.
         */
	if (chassis == D_chassisType_c7000) {
	    for (i=0; i<MAX_PWR_INSTALLED; i++) {
		if ((showp->ps[i].type != NOT_INSTALLED) && 
		    showp->ps[i].powerfail_status) {
		    printf("\n\n  WARNING: %s Power Supply is NON-OPERATIONAL",
			   c7000_ps_location_str[i]);
		    any_warning = TRUE;
		}
	    }
	}
	
	/* 
         * Show voltage warning messages
         */
        for (sense = ECTEST_V_P12; sense <= ECTEST_V_P24; sense++) {
	    if (showp->s_test[sense].t_state > NORMAL) {
		convert_volts(tmp_str, showp->s_test[sense].t_measured, sense);
		printf("\n\n  WARNING: %s volts has reached %s level at %s(V)",
		       showp->s_test[sense].t_description,
		       state_str[showp->s_test[sense].t_state],
		       tmp_str);
		any_warning = TRUE;
	    }
        }
	
	/*
         * Show tempterature warning messages
         */
        for (sense = ECTEST_T_EXHAUST, i = 0; i < 2; i++) {
	    if (showp->s_test[sense].t_state > NORMAL) {
		printf("\n\n  WARNING: %s temperature has reached %s level at %d(C)",
		       showp->s_test[sense].t_description,
		       state_str[showp->s_test[sense].t_state],
		       CELSIUS(ecp->ec_show->s_test[sense].t_measured));
		any_warning = TRUE;
	    }
            sense = ECTEST_T_INLET;
        }
	
	/* 
	 * Show spinnaker fan warning messages
         */
	if ((chassis == D_chassisType_c7010) &&
	    (showp->last_fan_state > NORMAL)) {
	    printf("\n\n  WARNING: %s has reached %s level",
		   showp->s_test[ECTEST_FAN_0].t_description,
		   state_str[showp->last_fan_state]);
	    any_warning = TRUE;
	}
	
        if (!any_warning)
	    printf("\n\n  All Environmental Measurements are within"
		   " specifications\n");
        else
            printf("\n");
	
	switch (GETOBJ(int,1)) {
	case SHOW_ENV_ALL:
	    show_power_supply(showp);
	    show_voltage(showp);
	    show_temperature(showp);
	    break;
	case SHOW_ENV_LAST:
	    printf("\nLAST Environmental Statistics");
	    if (!last_showp) 
		printf("\n  Last shutdown reason is unknown; data is unavailable\n");
	    else {
		lshowp = last_showp;
                printf("\n  Environmental status as of %s", lshowp->date_time_snap);
		if (lshowp->who_shutdown) 
		    show_who_shutdown(lshowp);
		show_power_supply(lshowp);
		show_voltage(lshowp);
		show_temperature(lshowp);
	    }
	    break;
	case SHOW_ENV_TABLE:
	    show_env_table(showp);
	    break;
	case SHOW_ENV_DEFAULT:
	default:
	    break;
	}
    } else {
	printf("\n  Measurements not collected");
    }
    automore_disable();
    free(showp);
}

/*
 * show_power_supply
 * Prints the power supply type(s) and intermittent powerfails
 */
void show_power_supply (ecshow_t *showp)
{
    int i;

    switch (chassis) {
    case D_chassisType_c7000:
	printf("\n  ");
	for (i=MAX_PWR_INSTALLED-1; i>=0; i--) {
	    printf("%s Power Supply: %s",c7000_ps_location_str[i],
		   showp->ps[i].type_str);
	    if (showp->ps[i].type != NOT_INSTALLED)
		printf(", %13s", showp->ps[i].powerfail_status ? "OFF" : "ON");
	    else
		printf("      ");
	}
	break;
    case D_chassisType_c7010:
	printf("\n  Power Supply: %s",showp->ps[0].type_str);
	break;
    default:
    }

    if (showp->int_powerfail_count)
	printf("\n\n  Intermittent Powerfail(s): %12dLast on %s", 
	       showp->int_powerfail_count,
	       showp->pf_date_time_snap);
    else
	printf("\n\n  No Intermittent Powerfails");
    printf("\n");
}

/* 
 * show_voltage
 * Prints the measured voltage levels
 */
void show_voltage (ecshow_t *showp)
{
    int i, j;
    char tmp_str[20];
    
    for (i = ECTEST_V_P12; i <= ECTEST_V_P24; i++) {
	if (showp->s_test[i].t_measured < print_bounds[i].lower_bound)
	    printf("\n  %s volts measured at < %d.00(V)",
		   showp->s_test[i].t_description,
		   print_bounds[i].voltage_line -
		   (print_bounds[i].voltage_line / 2));
	else if (showp->s_test[i].t_measured > 
		 print_bounds[i].lower_bound +
		 (print_bounds[i].range * print_bounds[i].divisions)
		 * 2)
	    printf("\n  %s volts measured at > %d.00(V)",
		   showp->s_test[i].t_description,
		   print_bounds[i].voltage_line +
		   (print_bounds[i].voltage_line / 2));
	else {
	    for (j = 1; j <= print_bounds[i].range; j++) {
		if (showp->s_test[i].t_measured <
		    print_bounds[i].lower_bound +
		    print_bounds[i].divisions * 2 * j) {
		    convert_volts(tmp_str,showp->s_test[i].t_measured,i);
		    printf("\n  %s volts measured at %-6s(V)",
			   showp->s_test[i].t_description,
			   tmp_str);
		    break; /* Go on to next voltage sense */
		}
	    }
	}
    }
    printf("\n");
}

/* 
 * show_temperature
 * Prints out the measured temperature levels
 */
void show_temperature (ecshow_t *showp)
{
    int i, sense;
    
    for (sense = ECTEST_T_EXHAUST, i = 0; i < 2; i++) {
	if (showp->s_test[sense].t_measured) {
	    printf("\n  %7s",  showp->s_test[sense].t_description);
	    printf(" temperature measured at %2d(C)",
		   CELSIUS(showp->s_test[sense].t_measured));
	}
	sense = ECTEST_T_INLET;
    }
}

/*
 * show_who_shutdown
 */
void show_who_shutdown (ecshow_t *showp)
{
    ulong who = showp->who_shutdown;

    printf("\n  WARNING: ***INITIATED PROCESSOR SHUTDOWN***");
    switch (who) {
    case ECTEST_T_EXHAUST:
    case ECTEST_T_INLET  :    
	printf("\n  WARNING: %s temperature had reached %s level at %d(C)\n",
	       showp->s_test[who].t_description,
	       state_str[showp->s_test[who].t_state],
	       CELSIUS(showp->s_test[who].t_measured));
	break;
    case ECTEST_FAN_0:
    case ECTEST_FAN_1:
    case ECTEST_FAN_2:
    case ECTEST_FAN_3:
    case ECTEST_FAN_4:
    case ECTEST_FAN_5:
	printf("\n  WARNING: %s status had reached %s level\n",
	       showp->s_test[who].t_description,
	       state_str[showp->last_fan_state]);
	break;
    default:
	break;
    }
}

/*
 * show_env_table
 * prints out the env table for along with current values 
 */
void show_env_table (ecshow_t *showp)
{
    int i, sense;
    char tmp_str1[20], tmp_str2[20], tmp_str3[20];

    printf("\nVoltage Parameters:\n");
    printf("\n SENSE        CRITICAL                NORMAL"
	   "                CRITICAL\n");
    printf("-------|--------------------|-----------------"
	   "-------|--------------------\n");
    for (sense = ECTEST_V_P12; sense <= ECTEST_V_P24; sense++) {
	convert_volts(tmp_str1, volt_bounds[sense].normal_low, sense);
	convert_volts(tmp_str2, showp->s_test[sense].t_measured, sense);
	convert_volts(tmp_str3, volt_bounds[sense].normal_high, sense);
	printf("\n%s(V)", showp->s_test[sense].t_description);
	switch (showp->s_test[sense].t_state) {
	case NORMAL:
	    printf("%-25s%-11s(V)%-11s\n", tmp_str1, tmp_str2, tmp_str3);
	    break;
	case CRITICAL:
	    if (showp->s_test[sense].t_measured < volt_bounds[sense].normal_low)
		printf("%-13s(V)%-9s%-25s\n", tmp_str2, tmp_str1, tmp_str3);
	    else
		printf("%-25s%-25s%-9s(V)\n", tmp_str1, tmp_str3, tmp_str2);
	    break;
	}
    }

    printf("\nTemperature Parameters:\n");
    printf("\n SENSE     WARNING       NORMAL       WARNING"
	   "       CRITICAL       SHUTDOWN\n");
    printf("-------|-------------|------------|------------"
	   "-|--------------|--------------\n");
    for (sense = ECTEST_T_EXHAUST, i = 0; i < 2; i++) {
	switch (showp->s_test[sense].t_state) {
	case NORMAL:
	    printf("\n%7s%-15d%-5d(C)%-5d%-14d%-15d\n",
		   showp->s_test[sense].t_description,
		   CELSIUS(env_temp_bounds(sense,NORMAL)),
		   CELSIUS(showp->s_test[sense].t_measured),
		   CELSIUS(env_temp_bounds(sense,WARNING)),
		   CELSIUS(env_temp_bounds(sense,CRITICAL)),
		   CELSIUS(env_temp_bounds(sense,SHUTDOWN)));
	    break;
	case WARNING:
	    if (showp->s_test[sense].t_measured < env_temp_bounds(sense,NORMAL))
		printf("\n%7s%-7d(C)%-5d%-13d%-14d%-15d\n",
		       showp->s_test[sense].t_description,
		       CELSIUS(showp->s_test[sense].t_measured),
		       CELSIUS(env_temp_bounds(sense,NORMAL)),
		       CELSIUS(env_temp_bounds(sense,WARNING)),
		       CELSIUS(env_temp_bounds(sense,CRITICAL)),
		       CELSIUS(env_temp_bounds(sense,SHUTDOWN)));
	    else
		printf("\n%7s%-15d%-13d%-6d(C)%-5d%-15d\n",
		       showp->s_test[sense].t_description,
		       CELSIUS(env_temp_bounds(sense,NORMAL)),
		       CELSIUS(env_temp_bounds(sense,WARNING)),
		       CELSIUS(showp->s_test[sense].t_measured),
		       CELSIUS(env_temp_bounds(sense,CRITICAL)),
		       CELSIUS(env_temp_bounds(sense,SHUTDOWN)));
	    break;
	case CRITICAL:
	    printf("\n%7s%-15d%-13d%-14d%-6d(C)%-6d\n",
		   showp->s_test[sense].t_description,
		   CELSIUS(env_temp_bounds(sense,NORMAL)),
		   CELSIUS(env_temp_bounds(sense,WARNING)),
		   CELSIUS(env_temp_bounds(sense,CRITICAL)),
		   CELSIUS(showp->s_test[sense].t_measured),
		   CELSIUS(env_temp_bounds(sense,SHUTDOWN)));
	    break;
	case SHUTDOWN:
	    printf("\n%7s%-15d%-13d%-14d%-15d%-6d(C)\n",
		   showp->s_test[sense].t_description,
		   CELSIUS(env_temp_bounds(sense,NORMAL)),
		   CELSIUS(env_temp_bounds(sense,WARNING)),
		   CELSIUS(env_temp_bounds(sense,CRITICAL)),
		   CELSIUS(env_temp_bounds(sense,SHUTDOWN)),
		   CELSIUS(showp->s_test[sense].t_measured));
	    break;
	}
	sense = ECTEST_T_INLET;
    }
}

/*
 * env_get_show
 * Malloc's an ecshow_t buffer and copies in the latest and greatest
 * ENV measurements.
 *
 *         REMEMBER TO FREE YOUR SHOW BUF WHEN DONE !!!
 */
ecshow_t *env_get_show (void)
{
    int level;
    ecshow_t *showp;
    
    if (!ecp || !ecp->ec_show)
	return (ecshow_t *) NULL;
    
    if (!(showp = malloc(sizeof(ecshow_t)))) {
	printf(nomemory);
	return (ecshow_t *) NULL;
    }
    
    /*
     * Mask out interrupts while we copy the struct into our local
     * buffer, so that we can't get stepped under.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(ecp->ec_show, showp, sizeof(ecshow_t));
    reset_interrupt_level(level);
    
    return showp;
}

/*
 * env_show_str_init 
 * Copies in descriptions of the senses for use in the "env" displays.
 */
void env_show_str_init (ecshow_t *showp)
{
    sstrncpy(showp->s_test[ECTEST_V_P12].t_description, "+12", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_V_P5].t_description , " +5", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_V_N12].t_description, "-12", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_V_P24].t_description , "+24", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_T_EXHAUST].t_description, "Airflow", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_T_INLET].t_description, "Inlet", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_0].t_description, "Fan", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_1].t_description, "Fan", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_2].t_description, "Fan",	T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_3].t_description, "Fan", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_4].t_description, "Fan", T_DESCRIPT_LEN-1);
    sstrncpy(showp->s_test[ECTEST_FAN_5].t_description, "Fan", T_DESCRIPT_LEN-1);
}

/* 
 * env_temp_bounds
 * Returns the lower temperature bound.
 */
uint env_temp_bounds (int sense_line, int state)
{
    int indx;
    
    switch (sense_line) {
    case ECTEST_T_EXHAUST:
	indx = 0;
	break;
    case ECTEST_T_INLET:
	indx = 1;
	break;
    default:
	return 0;
    }
    
    if (state < NORMAL || state > SHUTDOWN) 
	return 0;
    
    switch (chassis) { 
    case D_chassisType_c7000:
	return temp_bounds_7000[indx][state];
    case D_chassisType_c7010:
	return temp_bounds_7010[indx][state];
    default:
	return 0;
    }
}

/*  
 *  access routine for voltage critical thresholds
 */
int getenvVoltThreshold (
	  int sense,
          boolean threshold)
{
    if (threshold == ENV_VOLT_HIGH)
	return( volt_bounds[sense].normal_high);
    else	    
	return (volt_bounds[sense].normal_low);
}


/*  
 *  access routine for power supply state
 */
int getenvSupplyState (
	  int sense)

{	
/*
 * For systems with redundant power supplies, check if
 * installed, but not operational.
 */
    if (chassis == D_chassisType_c7000) {
	if ((ecp->ec_show->ps[sense].type != NOT_INSTALLED) && 
	    ecp->ec_show->ps[sense].powerfail_status) 
	    return (CRITICAL);
	else if (ecp->ec_show->ps[sense].type == NOT_INSTALLED)
	    return (NOT_PRESENT);
    }
    else if (ecp->ec_show->ps[sense-1].powerfail_status) 
	return (SHUTDOWN);

    return (NORMAL);
}
/*
 *  mib access routine for power supply source (ac or dc)
 */
int getenvSupplySource (
	  int sense)
{
    int ps_src;
    ps_src = D_ciscoEnvMonSupplySource_unknown;
    if (sense < MAX_PWR_INSTALLED) {
	switch (chassis) {
	case D_chassisType_c7000:
	    switch (ecp->ec_show->ps[sense].type) {
	    case C7000_PS_TYPE_700W_DC:
		ps_src = D_ciscoEnvMonSupplySource_dc;
                break;
            case C7000_PS_TYPE_700W_AC:
		ps_src = D_ciscoEnvMonSupplySource_ac;
                break;
	    }
	    break;
	case D_chassisType_c7010:
	    switch (ecp->ec_show->ps[sense].type) {
	    case C7010_PS_TYPE_600W_AC:
		ps_src = D_ciscoEnvMonSupplySource_ac;
                break;
	    case C7010_PS_TYPE_600W_DC:
		ps_src = D_ciscoEnvMonSupplySource_dc;
                break;
	    }
	    break;
	}
    }
    return ps_src;
}


/*
 * SUBSYS header for environmental monitor
 */

#define ENVM_MAJVERSION 1
#define ENVM_MINVERSION 0
#define ENVM_EDITVERSION 1

SUBSYS_HEADER(envm, ENVM_MAJVERSION, ENVM_MINVERSION, ENVM_EDITVERSION,
	      envm_init, SUBSYS_CLASS_DRIVER,
	      "seq: nvram",		
	      NULL);		
      
