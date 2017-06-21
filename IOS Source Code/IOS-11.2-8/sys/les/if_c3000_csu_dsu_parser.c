/* $Id: if_c3000_csu_dsu_parser.c,v 3.2.2.3 1996/07/02 17:46:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_csu_dsu_parser.c,v $
 *------------------------------------------------------------------
 * if_c3000_csu_dsu_parser.c 
 *
 * March 1996, Johnny Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_csu_dsu_parser.c,v $
 * Revision 3.2.2.3  1996/07/02  17:46:51  snyder
 * CSCdi61472:  rewrite if_c3000_csu_dsu_parser.c sections
 * Branch: California_branch
 *              put in code review comments from sagarwal
 *
 * Revision 3.2.2.2  1996/06/27  18:01:55  snyder
 * CSCdi61472:  rewrite if_c3000_csu_dsu_parser.c sections
 * Branch: California_branch
 *              poor use of output strings - saves 628 bytes
 *
 * Revision 3.2.2.1  1996/06/13  19:07:46  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.2  1996/03/29  03:49:48  sagarwal
 * CSCdi46172:  Add Boa support to 11.1 mainline
 *
 * Revision 3.1  1996/03/28  20:06:27  sagarwal
 * Placeholder files for 2500 Derivative with integrated CSU/DSU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "logger.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "exception.h"
#include "ttysrv.h"
#include "../h/bitlogic.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_csu_dsu.h"
#include "if_les.h"
#include "init_les.h"
#include "if_les_hd64570.h"
#include "if_c3000_csu_dsu.h"
#include "../os/timer_generic.h"

/* 
 * csu_dsu_show_cable_state
 */
boolean csu_dsu_show_cable_state (ushort cable_type, boolean print_flag)
{
    char *s;

    s = NULL;
    switch (cable_type) {
      case BOA_SW56_2W_ADTRAN_DSU_ID:
            s = "56k 2-wire";
        break;
      case BOA_SW56_4W_ADTRAN_DSU_ID:
            s = "56k 4-wire";
        break;
      case BOA_FT1_DIGITAL_LINK_DSU_ID:
            s = "T1";
        break;
      case BOA_NO_MODULE:
        if (print_flag)
            printf("No module present ");
        return(TRUE);
        break;
      default:
        return(FALSE);
        break;
    }
    if (print_flag) 
        printf("Integrated %s CSU/DSU module ", s);

    return (TRUE);
}

/*
 * csu_dsu_show_info
 */
void csu_dsu_show_info (hwidbtype *hwidb)
{
    char revision_cksum_fields[6];
    uchar dsu_reg_char1, dsu_reg_char2;
    ushort dsu_reg_short;
    ulong dsu_reg_long;
    csu_dsu_instance_t *cd_info;
    char *s;

    cd_info = csu_dsu_get_instance(hwidb);

    if (!hwidb || !cd_info)
	return;

    /*
     * Display module type, revisions and image checksums
     */
    switch (cd_info->module_type) {
	case CSU_DSU_SW56_2_WIRE:
	    s = "2-wire Switched 56";
	    break;
	case CSU_DSU_SW56_4_WIRE:
	    s = "4-wire Switched 56";
	    break;
	case CSU_DSU_FT1:	
	    s = "T1/fractional";
	    break;
	default:
	    /* Should never come here */
	    printf("\nModule type is %s", "unknown");
	    return;
            break;
    }
    printf("\nModule type is %s", s);
	
    /* Hardware revision & Sw revision */
    if (csu_dsu_register_RW(cd_info, DSU_HW_REVISION_REG,
                            DSU_HW_REVISION_LEN, (char *)revision_cksum_fields,
                            READ_REGISTER) == DSU_REQUEST_COMPLETE)
        printf("\n    Hardware revision is %s, ", revision_cksum_fields);
    else
	goto register_RW_failure;

    if (csu_dsu_register_RW(cd_info, DSU_SW_REVISION_REG,
                            DSU_SW_REVISION_LEN, (char *)revision_cksum_fields,
                            READ_REGISTER) == DSU_REQUEST_COMPLETE)
	printf("Software revision is %s,", revision_cksum_fields);
    else
	goto register_RW_failure;

    /* Image checksum */
    if (csu_dsu_register_RW(cd_info, DSU_IMAGE_CKSUM_REG,
                            DSU_IMAGE_CKSUM_LEN, (char *) &dsu_reg_long,
                            READ_REGISTER) == DSU_REQUEST_COMPLETE)
	printf("\n    Image checksum is 0x%x, ", dsu_reg_long);
    else
	goto register_RW_failure;

    /* The module-router interface protocol revision field */
     if (csu_dsu_register_RW(cd_info, DSU_INT_PROTOCOL_REV_REG,
                             DSU_INT_PROTOCOL_REV_LEN, (char *) &dsu_reg_short, 
                             READ_REGISTER) == DSU_REQUEST_COMPLETE) 
	printf("Protocol revision is %d.%d ", (dsu_reg_short >> 8), 
					      (dsu_reg_short & 0x00ff));
     else
	goto register_RW_failure;
    /* Check if this version of image recognises this protocol */
    if (dsu_reg_short > cd_info->mod_highest_int_protocol_rev)
	printf("(unrecognized).");

    /* module-specific code will display network status */
    if (cd_info->dsu_show_network_status)
	(*cd_info->dsu_show_network_status)(cd_info);

    /* Now show any loopback states in progress */
    if (csu_dsu_register_RW(cd_info, DSU_TEST_TYPE_SELECT_REG,
                            DSU_TEST_TYPE_SELECT_LEN,
                            (char *) &dsu_reg_char1,
                            READ_REGISTER) != DSU_REQUEST_COMPLETE) 
	goto register_RW_failure;

    if (csu_dsu_register_RW(cd_info, DSU_TEST_PATTERN_SELECT_REG,
                            DSU_TEST_PATTERN_SELECT_LEN,
                            (char *) &dsu_reg_char2, 
			    READ_REGISTER) != DSU_REQUEST_COMPLETE)
	goto register_RW_failure;

    csu_dsu_show_loopback_status(cd_info, dsu_reg_char1, dsu_reg_char2);

    /* module-specific configuration details */
    if (cd_info->dsu_show_module_config)
	(*cd_info->dsu_show_module_config)(cd_info);

    /* last user loopback performed */
    if (cd_info->last_loopback_command != DSU_LOOPBACK_NONE) {
	printf("\nLast user loopback performed:\n    ");
	csu_dsu_show_last_lpback(cd_info);
    }
	
    /* self test results */
    printf("\nLast module self-test (done ");
    if (!TIMER_RUNNING(cd_info->selftest_start_time))
	printf("at startup): "); 
    else {
	print_dhms(cd_info->selftest_start_time);
	printf("): ");
    }

    csu_dsu_selftest_results(cd_info, TRUE);

    /* show the alarm counters */
    if (cd_info->dsu_show_alarm_counters) {
	printf("\nLast clearing of alarm counters ");
	print_dhms(cd_info->last_counter_reset);
	(*cd_info->dsu_show_alarm_counters)(cd_info);
    }
	
    /* for T1 module show performance data */
    if (cd_info->dsu_show_performance_stats)
	(*cd_info->dsu_show_performance_stats)(cd_info);
 
    return;

  register_RW_failure:
    printf("\nMODULE READ FAILED");

}

/*
 * csu_dsu_show_perf_stat
 */
void csu_dsu_show_perf_stat (hwidbtype *idb, int start_ind, int stop_ind)
{
    csu_dsu_instance_t *cd_info;

    if (!idb || !(cd_info = csu_dsu_get_instance(idb)))
	return;

    if ((start_ind == 0) && (stop_ind == 0)) {
      /*
       * No interval range was specified by user
       */
      start_ind = 1;
      stop_ind  = T1_MAX_PERFORMANCE_INTERVALS;
    }

    if ((start_ind <= 0) || (stop_ind <= 0) ||
	(stop_ind < start_ind)) {
	printf("\nInvalid range specified");
        return;
    }

    if (cd_info->dsu_show_full_performance_stats)
	(*cd_info->dsu_show_full_performance_stats)(cd_info, start_ind, stop_ind);
}

/*
 * csu_dsu_show_command
 */
void csu_dsu_show_command (parseinfo *csb)
{
    hwidbtype *idb, *tmpidb;

    automore_enable(NULL);

    /* Check show option */
    if (GETOBJ(int,1)) {
        if ((GETOBJ(int,2) == CSU_DSU_SHOW_DEF)) {
            idb = hwidb_or_null(GETOBJ(idb,1));
            csu_dsu_show_info(idb);
        } else {
   	     idb = hwidb_or_null(GETOBJ(idb,1));
	     csu_dsu_show_perf_stat(idb, GETOBJ(int,3),GETOBJ(int,4));
        }
    } else {
       /*
        * Display information about all installed CSU/DSUs
        */
       FOR_ALL_HWIDBS(tmpidb)
           if (csu_dsu_csu_dsu_installed(CHK_ANY_CSU_DSU, tmpidb))
              csu_dsu_show_info(tmpidb);
    }
	
    automore_disable();
}

/*
 * csu_dsu_clear_command
 */
void csu_dsu_clear_command (parseinfo *csb)
{
    hwidbtype *idb;
    csu_dsu_instance_t *cd_info;
  
    idb = hwidb_or_null(GETOBJ(idb,1));

    if (idb == NULL) {
        printf("\n%% Invalid interface selected.\n");
    } else {
        cd_info = csu_dsu_get_instance(idb);
        reset_csu_dsu(cd_info, TRUE);
    }
}

/*
 * csu_dsu_test_command
 */
void csu_dsu_test_command (parseinfo *csb)
{
    hwidbtype *idb;
    csu_dsu_instance_t *cd_info;

    idb = hwidb_or_null(GETOBJ(idb,1));

    if (idb == NULL) {
        printf("\n%% Invalid interface selected.\n");
	return;
    } 

    cd_info = csu_dsu_get_instance(idb);
    csu_dsu_loopback_command(cd_info, CSU_DSU_SELFTEST, 0, 0, 0); 
}

/*
 * csu_dsu_make_bit_field
 * create a bit field with the passed bits set.
 */
ulong csu_dsu_make_bit_field (int start, int end)
{
    ulong retval;

    for (retval = 0 ; start <= end ; start++)
        retval |= 1 << (start - 1);
    return(retval);
}

/*
 * csu_dsu_validate_usr_pat
 * validate the user defined test pattern for the "loopback remote" command.
 */
ulong csu_dsu_validate_usr_pat (char *user_pat) 
{
    int length;
    int index;
    ulong bitmap;
    boolean valid_str;

    bitmap = 0;
    length = strlen(user_pat);

    valid_str = TRUE;
    if ((length <= 0) || (length > CSU_DSU_MAX_USR_PAT_BITS)) {
        valid_str = FALSE;
    } else {
        for (index = length - 1; index >= 0; index--) {
            if (user_pat[index] == '0') {
	        continue;    
            } else if (user_pat[index] == '1') {
                bitmap = bitmap | csu_dsu_make_bit_field(index+1, index+1);
	    } else {
	        valid_str = FALSE;
	        break;
	    }
        }
    }

    if (valid_str) {
	/*
	 * CSU/DSU needs bits length of user pattern to repeat
	 * as 4th byte of bitmap field
	 */
	bitmap |= (length << CSU_DSU_MAX_USR_PAT_BITS);
	return (bitmap);
    } else {
        printf("\n%% Invalid command: User pattern %s is invalid, binary bits pattern (up to 24 bits) is expected.\n", user_pat);
	return (CSU_DSU_INVALID_USER_PAT);
    }
}

/*
 * csu_dsu_show_user_pat
 * Convert from bit field to user defined pattern
 */
void csu_dsu_show_user_pat (char *u_pat, ulong bmap, int len)
{
    int index;

    *u_pat = '\0';

    for (index = 1; index <= len; index++) {
        if (CSU_DSU_BIT_SET_CHK(bmap, index))
		strcat(u_pat, "1");
	else
		strcat(u_pat, "0");
    }
    return;
}

/*
 * csu_dsu_show_ts_map
 * Convert from bit field to act string.
 */
void csu_dsu_show_ts_map (char *comline, ulong ts_range)
{
    int ts;
    boolean first;

    first = FALSE;
    *comline = '\0';

    for (ts = 1 ; ts <= CSU_DSU_MAX_TIMESLOTS; first = TRUE) {
        while ((ts <= CSU_DSU_MAX_TIMESLOTS) && 
	       (!(CSU_DSU_BIT_SET_CHK(ts_range, ts)))) 
            ts++;

        if (ts > CSU_DSU_MAX_TIMESLOTS) {
            break;
        }

        /*
         * If we already have 1, put a comma
         */
        if (first)
            strcat(comline, ",");

        if (ts == CSU_DSU_MAX_TIMESLOTS) {
            sprintf(comline + strlen(comline), "%d", ts);
            ts++; /* ugly, but should work */
        } else if (CSU_DSU_BIT_SET_CHK(ts_range, ts + 1)) {
            /* we have a range */
            sprintf(comline + strlen(comline), "%d-", ts);
            while((++ts <= CSU_DSU_MAX_TIMESLOTS) && 
		  (CSU_DSU_BIT_SET_CHK(ts_range, ts)));
            sprintf(comline + strlen(comline), "%d", ts - 1);
        } else {
            sprintf(comline + strlen(comline), "%d", ts);
            ts += 2;    /* we know ts + 1 is not valid */
        }
    }
    return;
}

/*
 * csu_dsu_csu_dsu_installed
 * Check what type of CSU/DSU module is installed, if any.
 */
boolean csu_dsu_csu_dsu_installed (int module_type, hwidbtype *idb)
{
    hwidbtype *tmpidb;
    int mod_chk;

    switch (module_type) { 
    	case CHK_ANY_CSU_DSU:
	    mod_chk = CSU_DSU_FT1 | CSU_DSU_SW56;
	    break;
    	case CHK_5IN1_CSU_DSU:
	    mod_chk = CSU_DSU_SERIAL_5IN1;
	    break;
    	case CHK_FT1_CSU_DSU:
	    mod_chk = CSU_DSU_FT1;
	    break;
    	case CHK_SW56_CSU_DSU:
	    mod_chk = CSU_DSU_SW56;
	    break;
    	case CHK_SW56_2W_CSU_DSU:
	    mod_chk = CSU_DSU_SW56_2_WIRE;
	    break;
    	case CHK_SW56_4W_CSU_DSU:
	    mod_chk = CSU_DSU_SW56_4_WIRE;
	    break;
    	case CHK_FT1_OR_SW56_4W_CSU_DSU:
	    mod_chk = CSU_DSU_FT1 | CSU_DSU_SW56_4_WIRE;
	    break;
	default:
	    mod_chk = 0;
	    break;
    }

    /* Check the system if no specific interface is defined */
    if (idb == NULL) {
        FOR_ALL_HWIDBS(tmpidb) {
            if ((tmpidb->status & IDB_SERIAL) && (tmpidb->csu_dsu_type & mod_chk))
	        return (TRUE);
        }
    } else {
	if ((idb->status & IDB_SERIAL) && (idb->csu_dsu_type & mod_chk))
            return (TRUE);
    }

    return (FALSE);
}

/* 
 * csu_dsu_cfg_print_error
 * Display parser error messages
 */
void csu_dsu_cfg_print_error(uchar error_type)
{
    char *s;

    if (error_type == DSU_REQUEST_COMPLETE)
        return;
    switch (error_type) {
	case DSU_REQUEST_TIMEOUT:
	    s = "REQUEST TIMEOUT";
	    break;
	case DSU_BAD_INSTANCE:
	    s = "BAD INSTANCE";
	    break;
	case DSU_BEING_RESET:
	    s = "CSU/DSU IS BEING RESET";
	    break;
	case DSU_LOCK_OBTAIN_TIMEOUT :
	    s = "LOCK OBTAIN TIMEOUT";
	    break;
	case DSU_NO_MEMORY:
	    s= "NO_MEMORY";
	    break;
	case DSU_WRONG_MODULE_TYPE:
	    s = "WRONG MODULE TYPE";
	    break;
	case DSU_WRONG_FORMAT:
	    s = "WRONG FORMAT";
	    break;
	case DSU_INTERMEDIATE_REG_RW_FAILURE:
	    s = "INTERMEDIATE REG R/W FAILURE";
	    break;
	case DSU_MAX_REG_ACCESS_RESULTS:
	    s = "MAX REG ACCESS RESULTS";
	    break;
	default:
            s = "Unknown error";
	    printf("\n%% %s = %x.", s, error_type);
	    break;
    }
    printf("\n%% Service module configuration command failed: %s.\n", s);
    return;
}

/*
 * csu_dsu_service_module_command
 * Used to configure service module (CSU/DSU) on a serial interface.
 */
void csu_dsu_service_module_command (parseinfo *csb)
{
    hwidbtype *idb;
    idbtype *swidb;
    csu_dsu_instance_t *ds;
    boolean cnfg_flag;
    char *comline;
    int cur_csu_dsu_type;
    uchar result;
    uchar user_option1;
    uchar user_option2;
    ulong user_option3;
    uchar cnfg_param0;
    uchar cnfg_param1;
    uchar cnfg_param2;
    ulong cnfg_param3;
    uchar current_test;
    ulong user_option_bmap;
    ulong cnfg_param_bmap;
    boolean cnfg_param1_flag;
    boolean loopback_idle;

    cnfg_flag= FALSE;
    result = 0;
    cnfg_param1 = 0;
    cnfg_param2 = 0;

    swidb = csb->interface;
    idb = hwidb_or_null(swidb);
    if (idb == NULL) {
	return;
    }

    /* Get DSU-CSU's instance */
    ds = csu_dsu_get_instance(idb);

    if (!((idb->csu_dsu_type & CSU_DSU_FT1) || 
          (idb->csu_dsu_type & CSU_DSU_SW56)))
    {
	if (!csb->nvgen) 
	    printf("\n%% Service module must be installed before using configuration subcommands\n");
        return;
    }

    if (csb->nvgen) {
	switch (csb->which) {
            case CSU_DSU_CLK_SRC:
		if (ds->usr_cfg_clock_source == CSU_DSU_CLK_SRC_INT) 
        	    nv_write(TRUE, "%s internal", csb->nv_command);
	        break;

            case CSU_DSU_DCODE:
		if ((ds->usr_cfg_data_coding) &&
		    (ds->usr_cfg_data_coding != CSU_DSU_DCODE_NORMAL)) {
            	    nv_write(TRUE, "%s", csb->nv_command);
                    switch (ds->usr_cfg_data_coding) {
                        case CSU_DSU_DCODE_INVERTED:
     	      	            nv_add(TRUE, " inverted");
    		            break;
    		        case CSU_DSU_DCODE_SCRAMBLED:
     	      	            nv_add(TRUE, " scrambled");
    		            break;
    		        default:
    		    	    break;
    		    }
		}
	        break;

	    case CSU_DSU_LOOPBACK_DTE:
		if (ds->usr_cfg_loopback_dte_type) {
        	    nv_write(TRUE, "%s", csb->nv_command);
		    switch (ds->usr_cfg_loopback_dte_pat_opt) {
		        case CSU_DSU_LPBACK_PAT_NOSEL:
			    /* Do nothing */
			    break;
		        case CSU_DSU_LPBACK_PAT_2047:
 	      	            nv_add(TRUE, " 2047");
			    break;
		        case CSU_DSU_LPBACK_PAT_511:
 	      	            nv_add(TRUE, " 511");
			    break;
		        case CSU_DSU_LPBACK_PAT_STR_PAT:
 	      	            nv_add(TRUE, " stress-pattern %u", 
			           ds->usr_cfg_loopback_dte_pattern);
			    break;
		        default:
		    	    break;
		    }
		}
	        break;

	    case CSU_DSU_LOOPBACK_LINE:
		if (ds->usr_cfg_loopback_line_type) {
           	    /*
         	     * Make sure loopback tests is active 
         	     */
                    loopback_idle = TRUE;
        	    if (csu_dsu_register_RW(ds, DSU_TEST_TYPE_SELECT_REG,
                        DSU_TEST_TYPE_SELECT_LEN,
                        (char *) &current_test,
                        READ_REGISTER) == DSU_REQUEST_COMPLETE)  {
            		if (current_test != DSU_LOOPBACK_NONE)
                	    loopback_idle = FALSE;
			else {
			    /*
			     * Tell the driver there are no more loopbacks 
			     */
			    csu_dsu_loopback_command(ds, CSU_DSU_LPBACK_NONE, 0, 0, 0);
			} 
        	    }
		    if (loopback_idle) {
	                ds->usr_cfg_loopback_line_type = CSU_DSU_LPBACK_TYPE_NOSEL;
		    } else {
        	        nv_write(TRUE, "%s", csb->nv_command);
	                if (ds->usr_cfg_loopback_line_type == CSU_DSU_LPBACK_TYPE_FR)
 	                    nv_add(TRUE, " payload");
		    }
		}
	        break;

	    case CSU_DSU_LOOPBACK_REMOTE:
		if (ds->usr_cfg_loopback_remote_type) {
        	    nv_write(TRUE, "%s", csb->nv_command);
                    switch (ds->usr_cfg_loopback_remote_type) {
		        case CSU_DSU_LPBACK_TYPE_NOSEL:
			    break;
		        case CSU_DSU_LPBACK_TYPE_DEF:
                            if (idb->csu_dsu_type & CSU_DSU_FT1)  
 	      	            	nv_add(TRUE, " full");
			    break;
		        case CSU_DSU_LPBACK_TYPE_FR:
 	      	            nv_add(TRUE, " payload");
			    break;
		        case CSU_DSU_LPBACK_TYPE_SJ:
 	      	            nv_add(TRUE, " smart-jack");
			    break;
		        default:
		    	    break;
		    }
		    switch (ds->usr_cfg_loopback_remote_pat_opt) {
			case CSU_DSU_LPBACK_PAT_NOSEL:
			    /* Do nothing */
			    break;
			case CSU_DSU_LPBACK_PAT_QRW:
 	  	                nv_add(TRUE, " qrw");
			    break;
			case CSU_DSU_LPBACK_PAT_1IN8:
 	  	                nv_add(TRUE, " lin8");
			    break;
			case CSU_DSU_LPBACK_PAT_3in24:
 	  	                nv_add(TRUE, " 3in24");
			    break;
			case CSU_DSU_LPBACK_PAT_1IN2:
 	  	                nv_add(TRUE, " 1in2");
			    break;
			case CSU_DSU_LPBACK_PAT_1IN1:
 	  	                nv_add(TRUE, " 1in1");
			    break;
			case CSU_DSU_LPBACK_PAT_0IN1:
 	  	                nv_add(TRUE, " 0in1");
			    break;
			case CSU_DSU_LPBACK_PAT_1IN3:
 	  	                nv_add(TRUE, " 1in3");
			    break;
			case CSU_DSU_LPBACK_PAT_1IN5:
 	  	                nv_add(TRUE, " 1in5");
			    break;
			case CSU_DSU_LPBACK_PAT_UP:
                            comline = string_getnext();
                            csu_dsu_show_user_pat(comline, ds->usr_cfg_loopback_remote_pat_bmap,
						  ds->usr_cfg_loopback_remote_pat_len);
                            nv_add(TRUE, " user-pattern %s", comline);
			    break;
			case CSU_DSU_LPBACK_PAT_2047:
 	  	                nv_add(TRUE, " 2047");
			    break;
			case CSU_DSU_LPBACK_PAT_511:
 	  	                nv_add(TRUE, " 511");
			    break;
			case CSU_DSU_LPBACK_PAT_STR_PAT:
 	  	                nv_add(TRUE, " stress-pattern %u", 
			           ds->usr_cfg_loopback_remote_pat_bmap);
			    break;
			default:
			        break;
		    }
		}
	        break;

            case CSU_DSU_NWKTYPE:
		if (ds->usr_cfg_network_type == CSU_DSU_NWKTYPE_SWITCHED) 
        	    nv_write(TRUE, "%s switched", csb->nv_command);
	        break;

            case CSU_DSU_SW_CARRIER:
		if ((ds->usr_cfg_switch_carrier) &&
		    (ds->usr_cfg_switch_carrier != CSU_DSU_SW_CARRIER_ATT)) {
        	    nv_write(TRUE, "%s", csb->nv_command);
                    switch (ds->usr_cfg_switch_carrier) {
                        case CSU_DSU_SW_CARRIER_SPRINT:
 	      	            nv_add(TRUE, " sprint");
			    break;
                        case CSU_DSU_SW_CARRIER_OTHER:
 	      	            nv_add(TRUE, " other");
			    break;
		        default:
			    break;
		    }
		}
	        break;

            case CSU_DSU_RMT_LPBACK_SW56:
		if (ds->usr_cfg_sw56_remote_loopback_mode == CSU_DSU_RMT_LPBACK_MODE_NOSEL) 
        	    nv_write(TRUE, "no %s", csb->nv_command);
		break;

            case CSU_DSU_RMT_LPBACK_T1_FULL:
		if (ds->usr_cfg_t1_full_remote_loopback_type) {
                    if (ds->usr_cfg_t1_full_remote_loopback_type == 
			CSU_DSU_RMT_LPBACK_TYPE_ALT) 
    	                nv_write(TRUE, "%s alternate", csb->nv_command);
		} else {
        	    nv_write(TRUE, "no %s", csb->nv_command);
	        }
		break;

            case CSU_DSU_RMT_LPBACK_T1_FR:
		if (ds->usr_cfg_t1_fr_remote_loopback_type) {
                    if (ds->usr_cfg_t1_fr_remote_loopback_type == 
			CSU_DSU_RMT_LPBACK_TYPE_ALT) {
    	                nv_write(TRUE, "%s alternate", csb->nv_command);
		    } else if (ds->usr_cfg_t1_fr_remote_loopback_type == 
			    CSU_DSU_RMT_LPBACK_TYPE_V54) { 
    	                nv_write(TRUE, "%s v54", csb->nv_command);
		    }
		} else {
        	    nv_write(TRUE, "no %s", csb->nv_command);
	        }
	        break;

            case CSU_DSU_FRAMING:
		if (ds->usr_cfg_framing == CSU_DSU_FRAMING_SF) 
        	    nv_write(TRUE, "%s sf", csb->nv_command);
	        break;

            case CSU_DSU_LINECODE:
		if (ds->usr_cfg_linecode == CSU_DSU_LINECODE_AMI) 
        	    nv_write(TRUE, "%s ami", csb->nv_command);
	        break;

            case CSU_DSU_LBO:
		if ((ds->usr_cfg_lbo) &&
		    (ds->usr_cfg_lbo != CSU_DSU_LBO_NONE)) {
        	    nv_write(TRUE, "%s", csb->nv_command);
                    switch (ds->usr_cfg_lbo) {
		        case CSU_DSU_LBO_7_5_DB:
 	      	            nv_add(TRUE, " -7.5db");
		            break;
		        case CSU_DSU_LBO_15DB:
 	      	            nv_add(TRUE, " -15db");
		            break;
		        default:
		            break;
		    }
		}
  	        break;

            case CSU_DSU_TIMESLOTS:
		if (ds->usr_cfg_timeslot_type) {
                    switch (ds->usr_cfg_timeslot_type) {
		        case CSU_DSU_TS_TYPE_ALL:
			    if (ds->usr_cfg_timeslot_speed == CSU_DSU_TS_SPEED_56)
        	                nv_write(TRUE, "%s all", csb->nv_command);
		            break;
		        case CSU_DSU_TS_TYPE_RANGE:
        	            nv_write(TRUE, "%s", csb->nv_command);
                            comline = string_getnext();
			    csu_dsu_show_ts_map(comline, ds->usr_cfg_timeslot_range);
 	      	            nv_add(TRUE, " %s", comline);
		            break;
		        default:
		            break;
		    }

		    /* If there is a non default speed defined */
		    if (ds->usr_cfg_timeslot_speed == CSU_DSU_TS_SPEED_56) 
 	      	        nv_add(TRUE, " speed 56");
		}
	        break;

            case CSU_DSU_REMOTE_ALRM_ENA:
        	nv_write(ds->usr_cfg_yellow_alarm_enable, "%s", csb->nv_command);
	        break;

            case CSU_DSU_CLK_RATE:
		if ((ds->usr_cfg_clock_rate) &&
		    (ds->usr_cfg_clock_rate != CSU_DSU_CLK_RATE_56)) {
        	    nv_write(TRUE, "%s", csb->nv_command);
                    switch (ds->usr_cfg_clock_rate) {
		        case CSU_DSU_CLK_RATE_AUTO:
 	      	            nv_add(TRUE, " auto");
		            break;
		        case CSU_DSU_CLK_RATE_2_4:
 	      	            nv_add(TRUE, " 2.4");
		            break;
		        case CSU_DSU_CLK_RATE_4_8:
 	      	            nv_add(TRUE, " 4.8");
		            break;
		        case CSU_DSU_CLK_RATE_9_6:
 	      	            nv_add(TRUE, " 9.6");
		            break;
		        case CSU_DSU_CLK_RATE_19_2:
 	      	            nv_add(TRUE, " 19.2");
		            break;
		        case CSU_DSU_CLK_RATE_38_4:
 	      	            nv_add(TRUE, " 38.4");
		            break;
		        case CSU_DSU_CLK_RATE_64:
 	      	            nv_add(TRUE, " 64");
		            break;
		        default:
		            break;
		    }
		}
	        break;

            default:
	        bad_parser_subcommand(csb, csb->which);
	        break;
	}
	return;
    }

    cur_csu_dsu_type = GETOBJ(int,1);
    user_option1 = GETOBJ(int,2);
    switch (csb->which) {
        case CSU_DSU_CLK_SRC:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_CLK_SRC_LINE;
		user_option1 = CSU_DSU_CLK_SRC_NOSEL;
	    }

            if (cur_csu_dsu_type == CSU_DSU_FT1_OPT) 
	        result = t1_set_clock_source(ds, cnfg_param1); 
	    else 
	        result = sw56_set_clock_source(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_clock_source = user_option1;

	    break;

        case CSU_DSU_DCODE:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_DCODE_NORMAL;
		user_option1 = CSU_DSU_DCODE_NOSEL;
	    }

            if (cur_csu_dsu_type == CSU_DSU_FT1_OPT) 
	        result = t1_set_data_coding(ds, cnfg_param1); 
	    else 
	        result = sw56_set_data_coding(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_data_coding = user_option1;

	    break;

        case CSU_DSU_LOOPBACK_DTE:
 	    user_option2 = GETOBJ(int,3);
 	    user_option3 = GETOBJ(int,4);
            if (csb->sense) {
  		cnfg_param0 = CSU_DSU_LOOPBACK_DTE;
  		cnfg_param1 = user_option1;
  		cnfg_param2 = user_option2;
  		cnfg_param3 = user_option3;
	    } else {
                cnfg_param0 = CSU_DSU_LPBACK_NONE;
                cnfg_param1 = 0;
                cnfg_param2 = 0;
                cnfg_param3 = 0;
	        user_option1 = CSU_DSU_LPBACK_TYPE_NOSEL;
	        user_option2 = CSU_DSU_LPBACK_PAT_NOSEL;
	        user_option3 = 0;
	    }

	    result = csu_dsu_loopback_command(ds, cnfg_param0, cnfg_param1, 
					      cnfg_param2, cnfg_param3);
	
	    if (result == DSU_REQUEST_COMPLETE) {
		ds->usr_cfg_loopback_dte_type = user_option1;
		ds->usr_cfg_loopback_dte_pat_opt = user_option2;
		ds->usr_cfg_loopback_dte_pattern = user_option3;
	    }

	    break;

        case CSU_DSU_LOOPBACK_LINE:
            if (csb->sense) {
	        cnfg_param0 = CSU_DSU_LOOPBACK_LINE;
	        cnfg_param1 = user_option1;
	    } else { 
	        cnfg_param0 = CSU_DSU_LPBACK_NONE;
	        cnfg_param1 = 0;
	        user_option1 = CSU_DSU_LPBACK_TYPE_NOSEL;
	    }

            result = csu_dsu_loopback_command(ds, cnfg_param0, cnfg_param1, 0, 0);

	    if (result == DSU_REQUEST_COMPLETE) 
		ds->usr_cfg_loopback_line_type = user_option1;

	    break;

        case CSU_DSU_LOOPBACK_REMOTE:
 	    user_option2 = GETOBJ(int,3);
 	    user_option3 = GETOBJ(int,5);

            /* 
	     * If we are processing the invalid user pattern case,
	     * just exit.
	     */
            if (user_option3 == CSU_DSU_INVALID_USER_PAT) {
		result = DSU_REQUEST_COMPLETE;
		break;
	    }

            if (csb->sense) {
  		cnfg_param0 = CSU_DSU_LOOPBACK_REMOTE;
  		cnfg_param1 = user_option1;
  		cnfg_param2 = user_option2;
  		cnfg_param3 = user_option3;
	    } else {
                cnfg_param0 = CSU_DSU_LPBACK_NONE;
                cnfg_param1 = 0;
                cnfg_param2 = 0;
                cnfg_param3 = 0;
	        user_option1 = CSU_DSU_LPBACK_TYPE_NOSEL;
	        user_option2 = CSU_DSU_LPBACK_PAT_NOSEL;
	        user_option3 = 0;
	    }

	    result = csu_dsu_loopback_command(ds, cnfg_param0, cnfg_param1, 
					      cnfg_param2, cnfg_param3);
	
	    if (result == DSU_REQUEST_COMPLETE) {
		ds->usr_cfg_loopback_remote_type = user_option1;
		ds->usr_cfg_loopback_remote_pat_opt = user_option2;
		ds->usr_cfg_loopback_remote_pat_len = strlen(GETOBJ(string,4));;
		ds->usr_cfg_loopback_remote_pat_bmap = user_option3;
	    }

	    break;

        case CSU_DSU_NWKTYPE:

	    cnfg_param2 = ds->usr_cfg_switch_carrier;
	    if (cnfg_param2 == CSU_DSU_SW_CARRIER_NOSEL)
	        cnfg_param2 = CSU_DSU_SW_CARRIER_ATT;

            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_NWKTYPE_DDS;
		user_option1 = CSU_DSU_NWKTYPE_NOSEL;
	    }

	    result = sw56_set_network_type(ds, cnfg_param1, cnfg_param2); 

	    if (result == DSU_REQUEST_COMPLETE)
		ds->usr_cfg_network_type = user_option1;

	    break;

        case CSU_DSU_SW_CARRIER:
	    cnfg_param1 = ds->usr_cfg_network_type;
	    if (cnfg_param1 == CSU_DSU_NWKTYPE_NOSEL) {
		if (csu_dsu_csu_dsu_installed(CHK_SW56_4W_CSU_DSU, idb))
		    cnfg_param1 = CSU_DSU_NWKTYPE_DDS;
		else
		    cnfg_param1 = CSU_DSU_NWKTYPE_SWITCHED;
	    }

            if (csb->sense) { 
		cnfg_param2 = user_option1;
	    } else {
		if (csu_dsu_csu_dsu_installed(CHK_SW56_4W_CSU_DSU, idb))
	            cnfg_param2 = CSU_DSU_SW_CARRIER_ATT;
		else
	            cnfg_param2 = CSU_DSU_SW_CARRIER_SPRINT;

		user_option1 = CSU_DSU_SW_CARRIER_NOSEL;
	    }

            if (cur_csu_dsu_type == CSU_DSU_SW56_OPT) 
	        result = sw56_set_network_type(ds, cnfg_param1, cnfg_param2); 
	    else	
		result = DSU_REQUEST_COMPLETE;

	    if (result == DSU_REQUEST_COMPLETE)
		ds->usr_cfg_switch_carrier = user_option1;

	    break;

        case CSU_DSU_RMT_LPBACK_SW56:
	    if (csb->sense) {
		cnfg_flag = TRUE;
	    } else {
		cnfg_flag = FALSE;
		user_option1 = CSU_DSU_RMT_LPBACK_MODE_NOSEL;
	    }
	
	    result = sw56_set_RDL_enable(ds, cnfg_flag);

            if (result == DSU_REQUEST_COMPLETE) 
		ds->usr_cfg_sw56_remote_loopback_mode = user_option1;

	    break;

        case CSU_DSU_RMT_LPBACK_T1_FULL:
        case CSU_DSU_RMT_LPBACK_T1_FR:
	    user_option2 = GETOBJ(int,3);

            if (csb->sense) { 
		cnfg_param1 = user_option1;
		cnfg_param2 = user_option2;
	    } else {
	        cnfg_param1 = user_option1;
	        cnfg_param2 = CSU_DSU_RMT_LPBACK_TYPE_DISABLE;

		user_option2 = CSU_DSU_RMT_LPBACK_TYPE_NOSEL;
	    }

	    result = t1_set_remote_loopcode(ds, cnfg_param1, cnfg_param2); 

            if (result == DSU_REQUEST_COMPLETE) {
                if (cnfg_param1 == CSU_DSU_RMT_LPBACK_MODE_FULL)
                    ds->usr_cfg_t1_full_remote_loopback_type = user_option2;
                else
                    ds->usr_cfg_t1_fr_remote_loopback_type = user_option2;
            }

	    break;

        case CSU_DSU_FRAMING:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_FRAMING_ESF;
		user_option1 = CSU_DSU_FRAMING_NOSEL;
	    }

	    result = t1_set_line_framing(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_framing = user_option1;

	    break;

        case CSU_DSU_LINECODE:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_LINECODE_B8ZS;
		user_option1 = CSU_DSU_LINECODE_NOSEL;
	    }

	    result = t1_set_line_code(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_linecode = user_option1;

	    break;

        case CSU_DSU_LBO:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_LBO_NONE;
		user_option1 = CSU_DSU_LBO_NOSEL;
	    }

	    result = t1_set_line_buildout(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_lbo = user_option1;

	    break;

        case CSU_DSU_TIMESLOTS:
            user_option_bmap = GETOBJ(int,3);
	    user_option2 = GETOBJ(int,4);

            if (csb->sense) { 
		if (user_option1 == CSU_DSU_TS_TYPE_ALL)
		    cnfg_param_bmap = CSU_DSU_TS_ALL_BMASK;
		else
		    cnfg_param_bmap = user_option_bmap;
		 
		if (user_option2 == CSU_DSU_TS_SPEED_NOSEL)
		    cnfg_param1 = CSU_DSU_TS_SPEED_64;
		else
		    cnfg_param1 = user_option2;
		
	    } else {
		cnfg_param_bmap = CSU_DSU_TS_ALL_BMASK;
		cnfg_param1 = CSU_DSU_TS_SPEED_64;
		user_option1 = CSU_DSU_TS_TYPE_NOSEL;
		user_option2 = CSU_DSU_TS_SPEED_NOSEL;
	    }

	    result = t1_set_timeslots(ds, cnfg_param_bmap, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE) {
	        ds->usr_cfg_timeslot_type = user_option1;
	        ds->usr_cfg_timeslot_range = cnfg_param_bmap;
	        ds->usr_cfg_timeslot_speed = user_option2;
	    }

	    break;

        case CSU_DSU_REMOTE_ALRM_ENA:
            if (csb->sense)  
		cnfg_param1_flag = TRUE;
	    else 
	        cnfg_param1_flag = FALSE;

	    result = t1_yellow_alarm_enable(ds, cnfg_param1_flag); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_yellow_alarm_enable = cnfg_param1_flag;

	    break;

        case CSU_DSU_CLK_RATE:
            if (csb->sense) { 
		cnfg_param1 = user_option1;
	    } else {
	        cnfg_param1 = CSU_DSU_CLK_RATE_56;
		user_option1 = CSU_DSU_LBO_NOSEL;
	    }

	    result = sw56_set_clock_rate(ds, cnfg_param1); 

	    if (result == DSU_REQUEST_COMPLETE)
	        ds->usr_cfg_clock_rate = user_option1;

	    break;

        default:
	    bad_parser_subcommand(csb, csb->which);
	    result = DSU_REQUEST_COMPLETE;
	    break;
    }

    if (result != DSU_REQUEST_COMPLETE)
	csu_dsu_cfg_print_error(result);

    return;
}
