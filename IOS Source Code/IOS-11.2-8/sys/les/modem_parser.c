/* $Id: modem_parser.c,v 3.1.64.8 1996/09/11 23:50:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/modem_parser.c,v $
 *------------------------------------------------------------------
 * modem_parser.c - Modem Management Parser commands
 *
 * November, 1995, Viet Nguyen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: modem_parser.c,v $
 * Revision 3.1.64.8  1996/09/11  23:50:40  snyder
 * CSCdi68893:  my final const commit, unless you make more
 *              20 data 32 image
 * Branch: California_branch
 *
 * Revision 3.1.64.7  1996/09/10  03:57:11  elem
 * CSCdi67595:  OOB port assignment incorrect when only one modem card
 * installed
 * Branch: California_branch
 * The OOB ports should be allocated to the modems based on the physical
 * ports on the carrier card, and not the total of number of modems
 * found.  Plus some minor clean up for missing modem modules.
 *
 * Revision 3.1.64.6  1996/09/09  20:56:50  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.64.5  1996/09/02  08:37:07  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.64.4  1996/08/28  12:59:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.3  1996/07/11  01:11:44  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.64.2  1996/06/28  23:23:57  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/06/16  21:16:32  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/21  23:39:37  vnguyen
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <ctype.h>
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "logger.h"
#include "../if/msg_clear.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "interface_private.h"
#include "../os/chunk.h"
#include "../os/async.h"
#include "../ts/if_async.h"
#include "../ui/debug.h"
#include "../ui/command.h"
#include "modem_debug.h"
#include "modem_parser.h"
#include "../dev/flash_defines.h"
#include "if_as5200_tdm.h"
#include "../les/isdn_bri.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"
#include "parser_defs_modem.h"

boolean modem_startup_test = FALSE;
boolean modem_fast_answer = TRUE;
int modem_buffer_size = MODEM_BUFFER_SIZE_DEFAULT;
int modem_poll_retry = MODEM_POLL_RETRY_DEFAULT;
int modem_poll_time = MODEM_POLL_TIME_DEFAULT;
static int modem_forcing_a_law = MODEM_NOT_FORCING_A_LAW;
int modem_mgmt_csm_debug = 0;

static const char modem_stats_hdr[] = {"\nMdm  Typ    Status     Tx/Rx     G  Duration  TX  RX  RTS  CTS  DSR  DCD  DTR"};

/*
 * =====================================================================
 * show_modem_stats()
 *
 * Description:
 * display the status of the modem along with all its statistics
 *
 * Parameters:
 * slot - slot # of modem
 * port - port # of modem
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_stats (int slot, int port)
{
    tt_soc *modem_tty;
    hwidbtype *group_idb;
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_statistics_t *modem_stats = NULL;
    modem_status_info_t *modem_status;
    int group;
    ulong duration;
    long tmp;
    sys_timestamp current_time;
    char firmware_buff[16];
    char boot_buff[16];
    char buff[16];

    modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
    if (!modem_info) {
        return;
    }

    modem_mgmt_info = modem_info->modem_mgmt_info;
    modem_tty = modem_info->modem_tty;

    printf(modem_stats_hdr);
    printf("\n%d/%2d", slot, port);
    
    if (modem_mgmt_info)
    {
       if (modem_mgmt_check_index((ushort)modem_mgmt_info->modem_status_info.modulation,
                                  (char *)&modulation_txt[0])) {
          printf("%6s",modulation_txt[modem_mgmt_info->modem_status_info.modulation]);
       } else {
          sprintf(buff,"ERR %d",modem_mgmt_info->modem_status_info.modulation);
          printf("%6s",buff);
       }
    } else {
       printf("%6s"," - ");
    }


    if (CSM_IS_ACTIVE_CALL(modem_info)) {
        if (CSM_IS_CONNECTED_STATE(modem_info)) {
            printf(" %6s", "Conn");
        } else {
            printf(" %6s", "Offhk");
        }
    } else if (CSM_IS_BACK2BACK_TEST(modem_info)) {
        printf(" %6s", "Test");
    } else if (CSM_IS_MODEM_RESET(modem_info)) {
        printf(" %6s", "Reset");
    } else if (CSM_IS_DOWNLOAD_FILE(modem_info)) {
        printf(" %6s", "D/L");
    } else if (CSM_IS_DOWNLOAD_FAIL(modem_info)) {
        printf(" %6s", "Bad DL");
    } else if (CSM_IS_MODEM_BAD(modem_info)) {
        /*
         * is it modem software marking it (*) as bad or user marking it?
         */
        if (modem_info->modem_config & CFG_MODEM_BAD) {
            printf(" %6s", "Bad");
        } else {
            printf(" %6s", "Bad*");
        }
    } else if (CSM_IS_BUSY_OUT(modem_info) || CSM_IS_SHUTDOWN(modem_info)) {
        printf(" %6s", "Busy");
    } else {
        printf(" %6s", "Idle");
    }

    if (modem_mgmt_info) {
        printf("%-7u/%7u", 
            modem_mgmt_info->modem_status_info.tx_speed, 
            modem_mgmt_info->modem_status_info.rx_speed);
    } else {
        printf("    -/-        ");
    };

    if (modem_info->tty_hwidb) {
        group_idb = find_async_group_leader(modem_info->tty_hwidb);
        if (group_idb && !(group_idb->status & IDB_DELETED)) {
            group = group_idb->unit;
        } else { 
            group = 0;
        }
    } else {
        group = 0;
    }
    printf("%2d", group);

    if (CSM_IS_ACTIVE_CALL(modem_info)) {
        GET_TIMESTAMP(current_time);
        duration = CLOCK_DIFF_UNSIGNED(modem_info->stat_call_duration_started,
                                       current_time);
    } else {
        duration = CLOCK_DIFF_UNSIGNED(modem_info->stat_call_duration_started,
                                       modem_info->stat_call_duration_ended);
    }
    sprint_ticks_in_dhms(buff, duration);
    printf(" %8s", buff);

    if (modem_mgmt_info) {
        printf("   %c", 
           modem_mgmt_info->modem_status_info.tx_rx_data & RS232_TX?'x':' ');
        printf("%-4c", 
           modem_mgmt_info->modem_status_info.tx_rx_data & RS232_RX?'x':' ');
        printf("%-4c", 
           modem_mgmt_info->modem_status_info.modem_rs232 & RS232_RTS?'x':' ');
        printf("%-5c", 
           modem_mgmt_info->modem_status_info.modem_rs232 & RS232_CTS?'x':' ');
        printf("%-5c", 
           modem_mgmt_info->modem_status_info.modem_rs232 & RS232_DSR?'x':' ');
        printf("%-5c", 
           modem_mgmt_info->modem_status_info.modem_rs232 & RS232_DCD?'x':' ');
        printf("%-5c", 
           modem_mgmt_info->modem_status_info.modem_rs232 & RS232_DTR?'x':' ');
    } else {
        printf("   -   -   -    -    -    -    -");
    }

    printf("\n\nModem %d/%d, ", slot, port);
    if (modem_mgmt_info) {
        if (modem_mgmt_info->modem_status_info.modem_type == BRASIL_MODEM) {
            printf("%s (Select)", MODEM_VENDOR_NAME(modem_info)); 
        } else {
            printf("Unknown Modem");
        }
    } else {
        printf("%s (Reliable)", MODEM_VENDOR_NAME(modem_info));
    }

    if (modem_info->tty_hwidb) {
        printf(", %s", modem_info->tty_hwidb->hw_namestring);
    }

    if (modem_tty) { 
        printf(", TTY%d", modem_info->modem_tty->ttynum);
    }

    if (modem_mgmt_info) {
      modem_status = &(modem_mgmt_info->modem_status_info);
      if ((modem_status->modem_firmware_rev[0] == 0) &&
	  (modem_status->modem_firmware_rev[1] == 0) &&
	  (modem_status->modem_firmware_rev[2] == 0)) {
	if (!(modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
	  strcpy(firmware_buff, "Unknown (F)");
	} else if (!(modem_info->modem_config & STATUS_POLL_ALLOWED)) {
	  strcpy(firmware_buff, "Unknown (NP)");
	} else {
	  strcpy(firmware_buff, "Unknown");
	}
      } else {
	sprintf(firmware_buff, "%d.%d(%d)",
		modem_status->modem_firmware_rev[0] ,
		modem_status->modem_firmware_rev[1] ,
		modem_status->modem_firmware_rev[2]);
      }
      if ((modem_status->modem_boot_firmware_rev[0] == 0) &&
	  (modem_status->modem_boot_firmware_rev[1] == 0) &&
	  (modem_status->modem_boot_firmware_rev[2] == 0)) {
	if (!(modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
	  strcpy(boot_buff, "Unknown (F)");
	} else if (!(modem_info->modem_config & STATUS_POLL_ALLOWED)) {
	  strcpy(boot_buff, "Unknown (NP)");
	} else {
	  strcpy(boot_buff, "Unknown");
	}
      } else {
	sprintf(boot_buff, "%d.%d(%d)",
		modem_status->modem_boot_firmware_rev[0],
		modem_status->modem_boot_firmware_rev[1],
		modem_status->modem_boot_firmware_rev[2]);
      }
    } else {
       sprintf(firmware_buff, "Unknown");
       sprintf(boot_buff, "Unknown");
    }
    printf("\nFirmware (Boot) Rev: %s (%s)", firmware_buff, boot_buff);

    if (modem_tty) {
        printf("\nModem config: ");
        switch (modem_tty->capabilities & (MODEMMASK|MODEMDTR)) {
          case MODEMIN:
          case MODEMRI:
            printf("Incoming");
            break;
          case MODEMOUT:
            printf("Outgoing");
            break;
          case MODEMINOUT:
            printf("Incoming and Outgoing");
            break;
          default:
            printf("Incoming and Outgoing");
            break;
        }
    } else { 
        printf("\nModem config: Unknown");
    }

    if (modem_mgmt_info) {
	if (modem_mgmt_check_index((ushort)modem_mgmt_info->modem_status_info.protocol,
				   (char *)&protocol_txt[0])) {
           printf("\nProtocol: %s", 
		   protocol_txt[modem_mgmt_info->modem_status_info.protocol]);
        } else {
	   printf ("\nProtocol: Invalid Protocol %d",
		   modem_mgmt_info->modem_status_info.protocol);
        }
	   
        if (modem_mgmt_check_index((ushort)modem_mgmt_info->modem_status_info.compression,
				   (char *)&compress_txt[0])) {
           printf(", Compression: %s", 
                  compress_txt[modem_mgmt_info->modem_status_info.compression]);
        } else {
	   printf(", Compression: Invalid compression %d", 
		  modem_mgmt_info->modem_status_info.compression);
        }
        if (modem_info->modem_config & CFG_MODEM_STATUS_POLL) {
            if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
                printf("\nManagement port config: Status polling and AT session");
            } else {
	       printf("\nManagement port config: Status polling and No AT session");
            }
        } else if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
            printf("\nManagement port config: AT session and No status polling");
        } else {
            printf("\nManagement port config: No status polling and No AT session");
        }
	if (modem_info->initialized == TRUE) {
	  if ((modem_mgmt_info->mgmt_port_status &
	       STATUS_POLL_ALLOWED) && (modem_info->modem_config &
					CFG_MODEM_STATUS_POLL)) { 
	    if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
	      printf("\nManagement port status: Status polling and AT session");
	    } else {
	      printf("\nManagement port status: Status polling and No AT session");
	    } 
	  } else if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
	     printf("\nManagement port status: AT session and No status polling");
	  } else {
	    printf("\nManagement port status: No status polling and No AT session");
	  }
	} else {
	  printf("\nManagement port status: No status polling and No AT"
		 " session(Uninitialized)");
	}
    } else {
      printf("\nManagement config: Not Select Modem");
    }

    if (modem_mgmt_info) {
        printf("\nTX signals: %d dBm, RX signals: %d dBm", 
           modem_mgmt_info->modem_status_info.tx_level?
               -1*(int)modem_mgmt_info->modem_status_info.tx_level:0, 
           modem_mgmt_info->modem_status_info.rx_level?
               -1*(int)modem_mgmt_info->modem_status_info.rx_level:0);
    } 

    printf("\n\n  Last clearing of \"show modem\" counters "); 
    print_dhms(modem_info->last_clear_modem_counter);
    if (modem_mgmt_info) {
        modem_stats = &(modem_mgmt_info->modem_stats);
        printf("\n    %d incoming completes, ", 
               modem_stats->modem_ic_complete -
               modem_stats->modem_ic_complete_shadow);
        tmp = (modem_stats->modem_ic_total -
               modem_stats->modem_ic_total_shadow) - 
              (modem_stats->modem_ic_complete -
               modem_stats->modem_ic_complete_shadow);
        if (tmp < 0) {
            tmp = 0; 
        }
        printf("%d incoming failures", tmp);
        printf("\n    %d outgoing completes, ", 
               modem_stats->modem_oc_complete - 
               modem_stats->modem_oc_complete_shadow);
        tmp = (modem_stats->modem_oc_total -
               modem_stats->modem_oc_total_shadow) - 
              (modem_stats->modem_oc_complete -
               modem_stats->modem_oc_complete_shadow);
        if (tmp < 0) {
            tmp = 0; 
        }
        printf("%d outgoing failures", tmp);
    } else {
        printf("\n    - incoming completes, - incoming failures");
        printf("\n    - outgoing completes, - outgoing failures");
    }
    printf("\n    %d failed dial attempts, ", modem_info->stat_dial_failure);
    printf("%d ring no answers, ", modem_info->stat_ring_no_answer);
    printf("%d busied outs", modem_info->stat_busyout);
    if (modem_mgmt_info) {
        printf("\n    %d no dial tones, ", modem_stats->modem_no_dial_tones -
               modem_stats->modem_no_dial_tones_shadow);
        printf("%d dial timeouts, ", modem_stats->modem_dial_timeouts -
               modem_stats->modem_dial_timeouts_shadow);
    } else {
        printf("\n     - no dial tones, - dial timeouts, ");
    }
    printf("%d watchdog timeouts", modem_info->wdt_timeout_count);
   
    if (modem_mgmt_info) {
        printf("\n    %d no carriers, ", modem_stats->modem_no_carriers -
               modem_stats->modem_no_carriers_shadow);
        printf("%d link failures, ", modem_stats->modem_link_failures -
               modem_stats->modem_link_failures_shadow);
    } else {
        printf("\n    - no carriers, - link failures, "); 
    }

    printf("%d resets %d recover oob", modem_info->stat_modem_reset, 
	   modem_stats->modem_reset_to_recover_oob);

    if (modem_mgmt_info) {
        printf("\n    %d protocol timeouts, ", modem_stats->modem_timeouts -
               modem_stats->modem_timeouts_shadow);
        printf("%d protocol errors, ", modem_stats->modem_protocol_errors -
               modem_stats->modem_protocol_errors_shadow);
        printf("%d lost events", modem_stats->modem_lost_events -
               modem_stats->modem_lost_events_shadow);
        printf("\n\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "75",
        "300", "600", "1200", "2400", "4800");
        printf("\n# of connections %-11lu %-8lu %-8lu %-8lu %-8lu %-8lu",
        modem_stats->modem_75_connections -
        modem_stats->modem_75_connections_shadow, 
        modem_stats->modem_300_connections -
        modem_stats->modem_300_connections_shadow,
        modem_stats->modem_600_connections -
        modem_stats->modem_600_connections_shadow,
        modem_stats->modem_1200_connections -
        modem_stats->modem_1200_connections_shadow,
        modem_stats->modem_2400_connections -
        modem_stats->modem_2400_connections_shadow,
        modem_stats->modem_4800_connections -
        modem_stats->modem_4800_connections_shadow);
        printf("\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "7200",
        "9600", "12000", "14400", "16800", "19200");
        printf("\n# of connections %-11lu %-8lu %-8lu %-8lu %-8lu %-8lu",
        modem_stats->modem_7200_connections -
        modem_stats->modem_7200_connections_shadow,
        modem_stats->modem_9600_connections -
        modem_stats->modem_9600_connections_shadow,
        modem_stats->modem_12000_connections -
        modem_stats->modem_12000_connections_shadow,
        modem_stats->modem_14400_connections -
        modem_stats->modem_14400_connections_shadow,
        modem_stats->modem_16800_connections -
        modem_stats->modem_16800_connections_shadow,
        modem_stats->modem_19200_connections -
        modem_stats->modem_19200_connections_shadow);
        printf("\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "21600",
        "24000", "26400", "28800", "31200", "33600");
        printf("\n# of connections %-11lu %-8lu %-8lu %-8lu %-8lu %-8lu",
        modem_stats->modem_21600_connections -
        modem_stats->modem_21600_connections_shadow,
        modem_stats->modem_24000_connections -
        modem_stats->modem_24000_connections_shadow,
        modem_stats->modem_26400_connections -
        modem_stats->modem_26400_connections_shadow,
        modem_stats->modem_28800_connections -
        modem_stats->modem_28800_connections_shadow,
        modem_stats->modem_31200_connections -
        modem_stats->modem_31200_connections_shadow,
        modem_stats->modem_33600_connections -
        modem_stats->modem_33600_connections_shadow);
    } else {
        printf("\n    - protocol timeouts, - protocol errors, - lost events");
        printf("\n\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "75",
        "300", "600", "1200", "2400", "4800");
        printf("\n# of connections %-11s %-8s %-8s %-8s %-8s %-8s",
        "-", "-", "-", "-", "-", "-");
        printf("\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "7200",
        "9600", "12000", "14400", "16800", "19200");
        printf("\n# of connections %-11s %-8s %-8s %-8s %-8s %-8s",
        "-", "-", "-", "-", "-", "-");
        printf("\nConnection Speeds %-10s %-8s %-8s %-8s %-8s %-8s", "21600",
        "24000", "26400", "28800", "31200", "33600");
        printf("\n# of connections %-11s %-8s %-8s %-8s %-8s %-8s",
        "-", "-", "-", "-", "-", "-");
    }
}

static const char modem_all_stats_hdr[] = {"\n                 Inc calls      Out calls   Busied   Failed  No       Succ\n  Mdm  Usage    Succ   Fail   Succ   Fail   Out      Dial    Answer   Pct."};

/*
 * =====================================================================
 * show_modem_all_stats()
 *
 * Description:
 * display summary statistics of each modem in the system 
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_all_stats (void)
{
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info=NULL;
    int slot;
    int port;
    int modem_in_slot;
    ulong total_call;
    ulong total_complete;
    long tmp;
    ulong usage;
    ulong uptime;
    ulong duration;
    sys_timestamp current_time;
    modem_statistics_t *modem_stats = NULL;

    uptime = system_uptime_seconds();
    automore_enable(NULL);
    printf(modem_all_stats_hdr);
    if (modem_pool) {
      for (slot=0; slot < modem_pool->max_slots; slot++) {
        if (modem_pool->modem_slot[slot]) {
            modem_in_slot = modem_pool->modem_slot[slot]->total_modem_num;
            for (port = 0; port < modem_in_slot; port++) {
                modem_info = CSM_MODEM_INFO(slot,port);
                if (modem_info) {
                    modem_mgmt_info = modem_info->modem_mgmt_info;
                    if (CSM_IS_ACTIVE_CALL(modem_info)) {
                        printf("\n*");
                    } else if (CSM_IS_BACK2BACK_TEST(modem_info)) {
                        printf("\nT");
                    } else if (CSM_IS_MODEM_RESET(modem_info)) {
                        printf("\nR", "Reset");
                    } else if (CSM_IS_DOWNLOAD_FILE(modem_info)) {
                        printf("\nD");
                    } else if ((CSM_IS_DOWNLOAD_FAIL(modem_info)) ||
                               (CSM_IS_MODEM_BAD(modem_info))) {
                        printf("\nB");
                    } else if (CSM_IS_BUSY_OUT(modem_info) || 
                               CSM_IS_SHUTDOWN(modem_info)) {
                        printf("\nb");
                    } else {
                        printf("\n ");
                    }

                    printf("%-2d/%2d", slot, port);
                    /*
                     * make sure we add current active call duration
                     * when calculating usage percentage
                     */
                    if (CSM_IS_ACTIVE_CALL(modem_info)) {
                        GET_TIMESTAMP(current_time);
                        duration = CLOCK_DIFF_UNSIGNED(
                                       modem_info->stat_call_duration_started,
                                       current_time);
                    } else {
                        duration = 0;
                    } 
                    usage = (((TS_VALUE(modem_info->stat_total_call_duration) 
                               + duration) * 100) /  ONESEC) / uptime;
                    printf("%-5d%%", usage);
                    if (modem_mgmt_info) {
                        modem_stats = &(modem_mgmt_info->modem_stats);
                        printf("%-8d", modem_stats->modem_ic_complete - 
                               modem_stats->modem_ic_complete_shadow);
                        tmp = (modem_stats->modem_ic_total -
                               modem_stats->modem_ic_total_shadow) -
                              (modem_stats->modem_ic_complete - 
                               modem_stats->modem_ic_complete_shadow);
                        if (tmp < 0) {
                            tmp  = 0;
                        }
                        printf("%-7d", tmp); 
                        printf("%-7d", modem_stats->modem_oc_complete -
                               modem_stats->modem_oc_complete_shadow);
                        tmp = (modem_stats->modem_oc_total -
                               modem_stats->modem_oc_total_shadow) -
                              (modem_stats->modem_oc_complete -
                               modem_stats->modem_oc_complete_shadow);
                        if (tmp < 0) {
                            tmp  = 0;
                        }
                        printf("%-7d", tmp);
                    } else {
                        printf("        -       -       -       -");
                    }
                    printf("%-8d", modem_info->stat_busyout);
                    printf("%-9d", modem_info->stat_dial_failure);
                    printf("%-8d", modem_info->stat_ring_no_answer);
                    total_call = (modem_stats->modem_ic_total -
                                  modem_stats->modem_ic_total_shadow) +
                                 (modem_stats->modem_oc_total -
                                  modem_stats->modem_oc_total_shadow);
                    total_complete = (modem_stats->modem_ic_complete -
                                      modem_stats->modem_ic_complete_shadow) + 
                                     (modem_stats->modem_oc_complete - 
                                      modem_stats->modem_oc_complete_shadow);
                    printf("%-7d%%", 
                           total_call?(total_complete*100)/total_call:0);
                }
            }
         }
      }
    }
    automore_disable();
}

/*
 * =====================================================================
 * init_counters()
 * 
 * Description: 
 * initialize modem_counters_t structure to 0
 * 
 * Parameters:
 * modem_counters_t - ptr to counter struct to init 
 * 
 * Returns:
 * None
 * =====================================================================
 */
static void init_counters (modem_counters_t *modem_counters)
{
    if (modem_counters) {
        bzero(modem_counters, sizeof(modem_counters_t));
    }
}

/*
 * =====================================================================
 * get_group_counters()
 *
 * Description:
 * traverse all the modem members in a group and sum up all the counters
 *
 * Parameters:
 * group #
 *
 * Returns:
 * modem_counters_t - structure that contains all the counter values
 * boolean - flag to know if modems are managed modems
 * =====================================================================
 */
static modem_counters_t get_group_counters (hwidbtype *hwidb, boolean *manage)
{
    int count;
    async_member_info *member;
    queuetype *memberQ;
    modem_counters_t group_counters;
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info=NULL;
    sys_timestamp current_time;
    ulong duration;
    long tmp;
    modem_statistics_t *modem_stats = NULL;
    
    init_counters(&group_counters);
    /*
     * init flag to managed modem
     */
    *manage = TRUE;

    /*
     * make sure the group isn't deleted before proceeding
     */
    if (hwidb && !(hwidb->status & IDB_DELETED)) {
        /*
         * traverses memberQ and retrieve modem info
         */
        memberQ = &hwidb->firstsw->async_groupQ;
        member = memberQ->qhead;
        for (count = 0; (member && (count < QUEUESIZE(memberQ))); count++) {
            modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
            if (modem_info) {
                modem_mgmt_info = modem_info->modem_mgmt_info;
                group_counters.modem_count++;
                if (modem_mgmt_info) {
                    modem_stats = &(modem_mgmt_info->modem_stats);
                    group_counters.stat_ic_complete +=
                        (modem_stats->modem_ic_complete - 
                         modem_stats->modem_ic_complete_shadow);
                    tmp = (modem_stats->modem_ic_total -
                           modem_stats->modem_ic_total_shadow) -
                          (modem_stats->modem_ic_complete - 
                           modem_stats->modem_ic_complete_shadow);
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    group_counters.stat_ic_failure += tmp;
                } else {
                    /*
                     * set manage flag to FALSE to let calling routines
                     * know to not display some fields in output
                     */
                     *manage = FALSE;
                }
                if (CSM_IS_IDLE_READY(modem_info) && 
                    (modem_info->modem_tty)) {
                    switch (modem_info->modem_tty->capabilities & 
                            (MODEMMASK|MODEMDTR)) {
                        case MODEMIN: 
                        case MODEMRI:
                        case MODEMDTR:
                             group_counters.stat_ic_avail++;
                             break;
                        case MODEMOUT:
                             group_counters.stat_oc_avail++;
                             break;
                        default: 
                             group_counters.stat_ic_avail++;
                             group_counters.stat_oc_avail++;
                             break;
                         }
                }
                if (modem_mgmt_info) {
                    group_counters.stat_oc_complete +=
                        (modem_stats->modem_oc_complete -
                         modem_stats->modem_oc_complete_shadow);
                    tmp = (modem_stats->modem_oc_total -
                           modem_stats->modem_oc_total_shadow) -
                          (modem_stats->modem_oc_complete -
                           modem_stats->modem_oc_complete_shadow);
                    if (tmp < 0) {
                        tmp = 0;
                    }
                    group_counters.stat_oc_failure += tmp;
                }
                group_counters.stat_busyout +=
                    modem_info->stat_busyout;
                group_counters.stat_dial_failure +=
                    modem_info->stat_dial_failure;
                group_counters.stat_ring_no_answer +=
                    modem_info->stat_ring_no_answer;

                /*
                 * make sure we add current active call duration
                 * when calculating usage percentage
                 */
                if (CSM_IS_ACTIVE_CALL(modem_info)) {
                    GET_TIMESTAMP(current_time);
                    duration = CLOCK_DIFF_UNSIGNED(
                                 modem_info->stat_call_duration_started,
                                                   current_time);
                } else {
                    duration = 0;
                }
                group_counters.stat_total_call_duration +=
                    TS_VALUE(modem_info->stat_total_call_duration)+duration;
            }
            member = member->next;
        }
    }
    return group_counters;
}

/*
 * =====================================================================
 * show_modem_group_summary()
 *
 * Description:
 * display summary counters of all members in a group
 *
 * Parameters:
 * group #
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_group_summary (int group)
{
    modem_counters_t group_counters;
    ulong total_call;
    ulong total_complete;
    ulong usage;
    ulong uptime;
    modem_group_t *tmp;
    hwidbtype *hwidb;
    boolean show_display;

    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            group_counters = get_group_counters(hwidb, &show_display);

            printf("\n%-3d", group);

            uptime = system_uptime_seconds();
            if (group_counters.modem_count && uptime) {
                usage = 100 * (group_counters.stat_total_call_duration / 
                         (ONESEC * group_counters.modem_count)) / uptime;
            } else {
                usage = 0;
            }
            printf("   %-3d%%", usage);
            if (show_display) {
                printf("%-6d", group_counters.stat_ic_complete);
                printf("%-7d", group_counters.stat_ic_failure);
            } else {
                printf("      -       -");
            }
            printf("%-7d", group_counters.stat_ic_avail);
            if (show_display) {
                printf("%-7d", group_counters.stat_oc_complete);
                printf("%-7d", group_counters.stat_oc_failure);
            } else {
                printf("       -       -");
            }
            printf("%-7d", group_counters.stat_oc_avail);
            printf("%-8d", group_counters.stat_busyout);
            printf("%-9d", group_counters.stat_dial_failure);
            printf("%-5d", group_counters.stat_ring_no_answer);
            total_call = group_counters.stat_ic_complete + 
                         group_counters.stat_ic_failure +
                         group_counters.stat_oc_complete + 
                         group_counters.stat_oc_failure;
            total_complete = group_counters.stat_ic_complete + 
                             group_counters.stat_oc_complete;
            printf("%-6d%%", total_call?(total_complete*100)/total_call:0);
        }
    }
}
        
/*
 * =====================================================================
 * show_modem_group_members()
 *
 * Description:
 * show the members in the group and the statistics of the members
 *
 * Parameters:
 * group - group number to show
 * detail - show all the group members in detail
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_group_members (int group, boolean detail)
{
    int slot;
    int port;
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int queue_size = 0, count;
    modem_info_t *modem_info;

    /* 
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /* 
             * traverses memberQ and retrieve modem info
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            queue_size = QUEUESIZE(memberQ);
            if (queue_size)
                printf("\nModem Group %d: ", group);
            member = memberQ->qhead;
            for (count = 0; (member && (count < queue_size)); count++) {
               modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
               if (modem_info) {
                   slot = CSM_MODEM_SLOT(modem_info);
                   port = CSM_MODEM_PORT(modem_info);
                   printf("%d/%d", slot, port);
                   if (count != (queue_size - 1))
                       printf(", ");
               }
               member = member->next;
            }
    
            /* need to loop twice to show individual member stats */
            if (detail) {
                /*
                 * traverses memberQ and retrieve modem info to get slot/port
                 */
                member = memberQ->qhead;
                for (count = 0; (member && (count < queue_size)); count++) {
                    modem_info = idb_get_hwsb(member->hwidb, 
                                              HWIDB_SB_MODEM_INFO);
                    if (modem_info) {
                        slot = 
                          CSM_MODEM_SLOT(modem_info);
                        port =
                          CSM_MODEM_PORT(modem_info);
                        printf("\n");
                        show_modem_stats(slot, port);
                    }
                    member = member->next;
                }
            }
        }
    }
}

static const char modem_grp_stats_hdr[] = {"\n              Incoming calls       Outgoing calls    Busied   Failed  No    Succ\nGrp  Usage  Succ   Fail  Avail   Succ   Fail  Avail  Out      Dial    Ans   Pct."};

/*
 * =====================================================================
 * show_modem_group_stats()
 *
 * Description:
 * to display the group summary stats and the members in the group
 *
 * Parameters:
 * group - group number to show
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_group_stats (int group)
{
    automore_enable(NULL);
    printf(modem_grp_stats_hdr);
    show_modem_group_summary(group);
    printf("\n");
    show_modem_group_members(group, TRUE);
    automore_disable();
}

/*
 * =====================================================================
 * show_modem_all_group_stats()
 *
 * Description:
 * to display all modem groups stats in the system
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_all_group_stats (void)
{
    modem_group_t *current_group;

    automore_enable(NULL);
    printf(modem_grp_stats_hdr);

    /*
     * first, show the summary statistics and
     * then show all the members in the group
     */
    current_group = modem_group_list;
    while (current_group) {
        if (current_group->hwidb &&
            !(current_group->hwidb->status & IDB_DELETED)) {
            show_modem_group_summary(current_group->hwidb->unit);
        }
        current_group = current_group->next;
    }
    printf("\n");
    current_group = modem_group_list;
    while (current_group) {
        if (current_group->hwidb &&
            !(current_group->hwidb->status & IDB_DELETED)) {
            show_modem_group_members(current_group->hwidb->unit, FALSE);
        }
        current_group = current_group->next;
    }
    automore_disable();
}

/*
 * =====================================================================
 * get_system_counters()
 *
 * Description:
 * sum up all the modems counters in the system
 *
 * Parameters:
 * None
 *
 * Returns:
 * modem_counters_t - a structure with all the counters to display
 * =====================================================================
 */
static modem_counters_t get_system_counters (boolean *manage)
{
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_counters_t system_counters;
    int modem_in_slot;
    int slot;
    int port;
    ulong duration;
    long tmp;
    sys_timestamp current_time;
    modem_statistics_t *modem_stats = NULL;
 
    init_counters(&system_counters);
    *manage = TRUE;
    if (modem_pool) {
      for (slot=0; slot < modem_pool->max_slots; slot++) {
        if (modem_pool->modem_slot[slot]) {
            modem_in_slot = modem_pool->modem_slot[slot]->total_modem_num;
            for (port = 0; port < modem_in_slot; port++) {
                modem_info = CSM_MODEM_INFO(slot, port);
                if (modem_info) {
                    modem_mgmt_info = modem_info->modem_mgmt_info;
                    system_counters.modem_count++;
                    if (modem_mgmt_info) {
                        modem_stats = &(modem_mgmt_info->modem_stats);
                        system_counters.stat_ic_complete +=
                            (modem_stats->modem_ic_complete -
                             modem_stats->modem_ic_complete_shadow);
                        tmp = (modem_stats->modem_ic_total -
                               modem_stats->modem_ic_total_shadow) -
                              (modem_stats->modem_ic_complete -
                               modem_stats->modem_ic_complete_shadow);
                        if (tmp < 0) {
                            tmp = 0;
                        }
                        system_counters.stat_ic_failure += tmp;
                    } else {
                        *manage = FALSE;
                    }
                    if (CSM_IS_IDLE_READY(modem_info) &&
                       (modem_info->modem_tty)) {
                       switch (modem_info->modem_tty->capabilities &
                               (MODEMMASK|MODEMDTR)) {
                         case MODEMIN:
                         case MODEMRI:
                         case MODEMDTR:
                           system_counters.stat_ic_avail++;
                           break;
                         case MODEMOUT:
                           system_counters.stat_oc_avail++;
                           break;
                         default:
                           system_counters.stat_ic_avail++;
                           system_counters.stat_oc_avail++;
                           break;
                       }
                    }

                    if (modem_mgmt_info) {
                        system_counters.stat_oc_complete +=
                            (modem_stats->modem_oc_complete - 
                             modem_stats->modem_oc_complete_shadow);
                        tmp = (modem_stats->modem_oc_total -
                               modem_stats->modem_oc_total_shadow) -
                              (modem_stats->modem_oc_complete -
                               modem_stats->modem_oc_complete_shadow);
                        if (tmp < 0) {
                            tmp = 0;
                        }
                        system_counters.stat_oc_failure += tmp;
                    }
                    system_counters.stat_busyout +=
                        modem_info->stat_busyout;
                    system_counters.stat_dial_failure +=
                        modem_info->stat_dial_failure;
                    system_counters.stat_ring_no_answer +=
                        modem_info->stat_ring_no_answer;
                    /*
                     * make sure we add current active call duration
                     * when calculating usage percentage
                     */
                    if (CSM_IS_ACTIVE_CALL(modem_info)) {
                        GET_TIMESTAMP(current_time);
                        duration = CLOCK_DIFF_UNSIGNED(
                                       modem_info->stat_call_duration_started,
                                       current_time);
                    } else {
                        duration = 0;
                    }
                    system_counters.stat_total_call_duration +=
                        TS_VALUE(modem_info->stat_total_call_duration)+duration;
                }
            }
        }
      }
    }
    return system_counters;
}


static const char modem_all_rev_hdr[] = {"\n\n\n          Modem module         Firmware           Boot\n  Mdm     Number               Rev                Rev"};


/*
 * =====================================================================
 * show_modem_version_info()
 *
 * Description:
 * display the modem version information of all system modems
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_version_info (void)
{
   modem_info_t *modem_info;
   modem_mgmt_info_t *modem_mgmt_info=NULL;
   int slot, port, module, modem_in_slot;
   int modem_in_module;
   char firmware_buff[15];
   char boot_buff[15];
   int carrier_rev_num;
   modem_status_info_t *modem_status;
   modem_cookie_t *modem_cookie;
   modem_module_cookie_t *module_cookie;
   modem_carrier_cookie_t *carrier_cookie;
   char *ch_ptr;
   char ix;
   boolean  valid_module_cookie;


   if (modem_pool) {
      automore_enable(NULL);
      printf(modem_all_rev_hdr);
      for (slot=0; slot < modem_pool->max_slots; slot++) {
         if (modem_pool->modem_slot[slot]) {
            modem_cookie = modem_pool->modem_slot[slot]->modem_cookie;
            carrier_cookie = &(modem_cookie->carrier);
            modem_in_slot = carrier_cookie->number_of_ports;
            if ( (modem_in_slot <= 0) || (carrier_cookie->max_modules <= 0) ) {
                printf("\n\n Slot %d: Modem Carrier card HW version info error\n",
                       slot);
                continue;
            } else {
               modem_in_module = modem_in_slot / carrier_cookie->max_modules; 
            }
            for (port = 0; port < modem_in_slot; port++) {
               modem_info = CSM_MODEM_INFO(slot,port);
               if (modem_info) {
                  modem_mgmt_info = modem_info->modem_mgmt_info;
                  if (modem_mgmt_info) {
		    modem_status = &(modem_mgmt_info->modem_status_info);
		    if ((modem_status->modem_firmware_rev[0] == 0) &&
			(modem_status->modem_firmware_rev[1] == 0) &&
			(modem_status->modem_firmware_rev[2] == 0)) {
		      if (!(modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
			strcpy(firmware_buff, "Unknown (F)");
		      } else if (!(modem_info->modem_config & STATUS_POLL_ALLOWED)) {
                        strcpy(firmware_buff, "Unknown (NP)");
		      } else {
			strcpy(firmware_buff, "Unknown");
		      }
		    } else {
		      sprintf(firmware_buff, "%d.%d.%d",
			      modem_status->modem_firmware_rev[0] ,
			      modem_status->modem_firmware_rev[1] ,
			      modem_status->modem_firmware_rev[2]);
		    }
		    if ((modem_status->modem_boot_firmware_rev[0] == 0) &&
			(modem_status->modem_boot_firmware_rev[1] == 0) &&
			(modem_status->modem_boot_firmware_rev[2] == 0)) {
		      if (!(modem_mgmt_info->mgmt_port_status & STATUS_POLL_ALLOWED)) {
			strcpy(boot_buff, "Unknown (F)");
		      } else if (!(modem_info->modem_config & STATUS_POLL_ALLOWED)) {
                        strcpy(boot_buff, "Unknown (NP)");
		      } else {
			strcpy(boot_buff, "Unknown");
		      }
		    } else {
		      sprintf(boot_buff, "%d.%d.%d",
			      modem_status->modem_boot_firmware_rev[0],
			      modem_status->modem_boot_firmware_rev[1],
			      modem_status->modem_boot_firmware_rev[2]);
		    }
                  } else {
                     strcpy(firmware_buff, "Unknown");
                     strcpy(boot_buff, "Unknown");
                  } 
                  if (port < modem_in_module)
		    module = 0;
                  else
		    module = 1;
                  printf("\n %-2d/%2d              %2d           %15s %15s",
                         slot, port, module, firmware_buff, boot_buff);
               }
            }
         }
      }

      printf("\n\n Modem board HW version info:");
      for (slot=0; slot < modem_pool->max_slots; slot++) {
         if (modem_pool->modem_slot[slot]) {
            carrier_rev_num = (modem_pool->modem_slot[slot]->modem_reg->mdb_rev) 
                              & 0x000f;
            modem_cookie = modem_pool->modem_slot[slot]->modem_cookie;
            carrier_cookie = &(modem_cookie->carrier);
            printf("\n\n Slot %d:", slot);
            if ( (carrier_cookie->number_of_ports <= 0) ||
                 (carrier_cookie->max_modules <= 0) ) {
               /* not a valid carrier cookie */
               printf("\n   Carrier card (HW version info error): \n     "
                      "hw version= %d, "
                      "byte 0= %#x, byte 1= %#x, byte 2= %#x",
                      carrier_rev_num,
                      carrier_cookie->number_of_ports,
                      carrier_cookie->max_modules,
                      carrier_cookie->max_oob_ports);
            } else {
               printf("\n   Carrier card: \n     hw version= %d, "
                      "number_of_ports= %d, max_modules= %d, max_oob_ports= %d",
                      carrier_rev_num,
                      carrier_cookie->number_of_ports,
                      carrier_cookie->max_modules,
                      carrier_cookie->max_oob_ports);
            }
     
            for (module=0; module < carrier_cookie->max_modules; module++) { 
               module_cookie = &(modem_cookie->module[module]);
               valid_module_cookie = FALSE;
               if (module_cookie->number_of_modems != 0xff) {
                  /* yes, there is modem module plug in */
               
                  /* now verify the cookie of modem module is valid*/
                  valid_module_cookie= TRUE;
                  ch_ptr = &(module_cookie->rev_num[0]);
                  for (ix=0; ix< MODEM_MODULE_VER_NUM_LEN; ix++) {
                     if (!isascii(*ch_ptr++)) {
                        valid_module_cookie= FALSE;
                        break;
                     }  
                  }

                  if (valid_module_cookie) {
                     ch_ptr = &(module_cookie->vendor_model_number[0]);
                     for (ix=0; ix< MODEM_MODULE_VENDER_CODE_LEN; ix++) {
                        if (!isascii(*ch_ptr++)) { 
                           valid_module_cookie= FALSE; 
                           break;
                        }  
                     }  
                  }
                  
                  if (valid_module_cookie) {
                     valid_module_cookie= 
                        is_string(&(module_cookie->vendor_banner[0]));
                  }
               }
  
               if (valid_module_cookie) {
                  /* it's a valid cookie */
                    printf("\n   Modem Module %d:\n     number_of_modems= %d,"
                           " option_bits= %d, \n     rev_num= %c%c.%c%c, "
                           "vendor_model_number= %c%c, \n     vendor_banner= %s", 
                           module, 
                           module_cookie->number_of_modems,
                           module_cookie->option_bits, 
                           module_cookie->rev_num[0],
                           module_cookie->rev_num[1],
                           module_cookie->rev_num[2],
                           module_cookie->rev_num[3],
                           module_cookie->vendor_model_number[0],
                           module_cookie->vendor_model_number[1],
                           module_cookie->vendor_banner);
	       } else if (module_cookie->number_of_modems == 0xff) {
		  printf ("\n   Modem Module %d:\n      number_of_modems= 0",
 		          module);
               } else {
                  /* it's not a valid cookie */
                  printf("\n   Modem Module %d:\n     "
                         "(Invalid hw version info -- hex dump:)\n",
                         module);
                  ch_ptr = &(module_cookie->number_of_modems);
                  for (ix=0; ix< MAX_MODEM_MODULE_COOKIE_LEN; ix++) {
                     printf("     byte %-2d= %-2x,", ix, *ch_ptr++);
                     if (!((ix+1)%4))
                        printf("\n"); 
                  }
               }
            }
         }
      }
      automore_disable();
   }
}


static const char modem_summary_hdr[] = {"\n         Incoming calls       Outgoing calls      Busied   Failed   No    Succ\nUsage  Succ   Fail  Avail   Succ   Fail  Avail    Out      Dial     Ans   Pct."};

/*
 * =====================================================================
 * show_modem_system_summary()
 *
 * Description:
 * display the summary statistics for the system
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_system_summary (void)
{
    modem_counters_t system_counters;
    ulong total_call;
    ulong total_complete;
    ulong usage;
    ulong uptime;
    boolean show_display;
 
    system_counters = get_system_counters(&show_display);
 
    uptime = system_uptime_seconds();
    if (system_counters.modem_count && uptime) {
        usage = 100 * (system_counters.stat_total_call_duration / 
            (ONESEC * system_counters.modem_count)) / uptime;
    } else {
        usage = 0;
    }

    printf(modem_summary_hdr);
    printf("\n%-4d%%", usage);
    if (show_display) {
        printf("%-6d", system_counters.stat_ic_complete);
        printf("%-7d", system_counters.stat_ic_failure);
    } else {
        printf("      -       -");
    }
    printf("%-7d", system_counters.stat_ic_avail);
    if (show_display) {
        printf("%-7d", system_counters.stat_oc_complete);
        printf("%-7d", system_counters.stat_oc_failure);
    } else {
        printf("       -       -");
    }
    printf("%-7d", system_counters.stat_oc_avail);
    printf("%-10d", system_counters.stat_busyout);
    printf("%-9d", system_counters.stat_dial_failure);
    printf("%-6d", system_counters.stat_ring_no_answer);
    total_call = system_counters.stat_ic_complete +
                 system_counters.stat_ic_failure +
                 system_counters.stat_oc_complete +
                 system_counters.stat_oc_failure;
    total_complete = system_counters.stat_ic_complete +
                     system_counters.stat_oc_complete;
    printf("%-7d%%", total_call?(total_complete*100)/total_call:0);
}

/*
 * =====================================================================
 * show_modem_log()
 *
 * Description:
 * display the event history queue for a modem
 *
 * Parameters:
 * slot, port = modem slot/port #
 *
 * Returns:
 * None
 * =====================================================================
 */

static void show_modem_log (int slot, int port)
{
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_history_event_t *ptr = NULL;
    int count;
    uchar *data_ptr = NULL;
    uchar rs232_event, rs232_changed_event;
    char buff[40], *tmp_buff;
    queuetype *tmp_eventQ = NULL;
    modem_history_event_t *tmp_ptr = NULL;
    uchar previousframe_type = 0x0;

    modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
    if (!modem_info) {
        return;
    }
    printf("\nModem %d/%d Events Log:", slot, port);
    modem_mgmt_info = modem_info->modem_mgmt_info;
    if (modem_mgmt_info && modem_mgmt_info->modem_history_eventQ) {
        /* 
         * lock eventQ and make a copy of it to display
         */
        while (!lock_semaphore(&(modem_mgmt_info->lock_eventQ))) {
            process_sleep_for(1);
        }
        tmp_eventQ = malloc(sizeof(queuetype));
        ptr = modem_mgmt_info->modem_history_eventQ->qhead;
        for (count = QUEUESIZE(modem_mgmt_info->modem_history_eventQ);
             ptr && (count > 0); count--) {
            tmp_ptr = chunk_malloc(modem_event_hdr_chunk);
            if (tmp_ptr) {
                tmp_ptr->event.frame_type = ptr->event.frame_type;
                tmp_ptr->event.event_type = ptr->event.event_type;
                tmp_ptr->event.event_length = ptr->event.event_length;
                COPY_TIMESTAMP(ptr->event_time, tmp_ptr->event_time);
                if (tmp_ptr->event.event_length < MAX_MODEM_SMALL_EVENT_SIZE) {
                    tmp_ptr->event.data_ptr = 
                        chunk_malloc(modem_small_event_chunk);
                } else {
                    tmp_ptr->event.data_ptr = 
                        chunk_malloc(modem_large_event_chunk);
                }
                if (tmp_ptr->event.data_ptr) {
                    bcopy(ptr->event.data_ptr, tmp_ptr->event.data_ptr, 
                          tmp_ptr->event.event_length); 
                }
                enqueue(tmp_eventQ, tmp_ptr);
            }
            ptr = ptr->next;        
        }

        /*
         * now unlock and process the event from the copied eventQ
         */
        unlock_semaphore(&(modem_mgmt_info->lock_eventQ));

        while ((ptr = (modem_history_event_t *)dequeue(tmp_eventQ)) != NULL) {
            sprint_dhms(buff, ptr->event_time);
            switch (ptr->event.frame_type) {
            case MCOM_STARTUP:
                data_ptr = ptr->event.data_ptr; 
                printf("\n  %8s: Startup Response: ", buff);
                if (*data_ptr == BRASIL_MODEM) {
                    printf("%s (Select)", MODEM_VENDOR_NAME(modem_info)); 
                } else {
                    printf("Unknown Modem: %d",(ushort) *data_ptr);
                }
                printf("\n\t    Modem (boot) firmware = %d.%d.%d (%d.%d.%d)",
                    *(data_ptr+MCOM_MODEM_REVISION_OFFSET), 
                    *(data_ptr+MCOM_MODEM_REVISION_OFFSET+1), 
                    *(data_ptr+MCOM_MODEM_REVISION_OFFSET+2),
                    *(data_ptr+MCOM_BOOT_REVISION_OFFSET),
                    *(data_ptr+MCOM_BOOT_REVISION_OFFSET+1),
                    *(data_ptr+MCOM_BOOT_REVISION_OFFSET+2));
                break;
            case MCOM_CONTROL:
                data_ptr = ptr->event.data_ptr; 
                printf("\n  %8s: Control Reply: 0x%02x%02x", buff, 
                    *(data_ptr+MCOM_EVENT_STATUS0_OFFSET), 
                    *(data_ptr+MCOM_EVENT_STATUS1_OFFSET));
                break;
            case MCOM_DC_SESSION:
		if (ptr->event.event_length > 1) {
		  previousframe_type = MCOM_DC_SESSION;
		  printf("\n  %8s: DC Session request: ", buff);
		  tmp_buff = chunk_malloc(modem_large_event_chunk);
		  if (tmp_buff) {
		    strncpy(tmp_buff, ptr->event.data_ptr, 
			    ptr->event.event_length);
		    /* add a null terminate to the string */
		    tmp_buff[ptr->event.event_length] = '\0';
		    printf("%s", tmp_buff);
		    chunk_free(modem_large_event_chunk, tmp_buff);
		  }
                }
                break;
            case MCOM_REFRESH_EVENT_POLL:
            case MCOM_EVENT_POLL:
                data_ptr = ptr->event.data_ptr; 
                switch (ptr->event.event_type) {
                case MCOM_TX_RX_EVENT:
                   printf("\n  %8s: TX/RX event: ", buff);
                   if (ptr->event.event_length == 1) {
                     printf("TXD=%s, RXD=%s", 
                       (*data_ptr & RS232_TX)?"On":"Off",
                       (*data_ptr & RS232_RX)?"On":"Off");
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_RS232_EVENT:
                   if (ptr->event.event_length == 2) {
                     rs232_event = *(data_ptr + MCOM_RS232_EV_OFFSET);
                     rs232_changed_event = 
                         *(data_ptr + MCOM_RS232_EV_CHANGED_OFFSET);
                     printf("\n  %8s: RS232 event: "
                            "%s%c %s%c %s%c %s%c %s%c %s%c %s%c", buff,
                        rs232_event & RS232_RTS?"RTS":"noRTS",
                        rs232_changed_event & RS232_RTS?'*':' ',
                        rs232_event & RS232_DTR?"DTR":"noDTR",
                        rs232_changed_event & RS232_DTR?'*':' ',
                        rs232_event & RS232_CTS?"CTS":"noCTS",
                        rs232_changed_event & RS232_CTS?'*':' ',
                        rs232_event & RS232_DSR?"DSR":"noDSR",
                        rs232_changed_event & RS232_DSR?'*':' ',
                        rs232_event & RS232_DCD?"DCD":"noDCD",
                        rs232_changed_event & RS232_DCD?'*':' ',
                        rs232_event & RS232_RI?"RI":"noRI",
                        rs232_changed_event & RS232_RI?'*':' ',
                        rs232_event & RS232_TST?"TST":"noTST",
                        rs232_changed_event & RS232_TST?'*':' ');
                   } else {
                     printf("Invalid length");
                   }
                   break; 
                case MCOM_MODEM_STATE_EVENT:
                   printf("\n  %8s: Modem State event: ", buff);
                   if (ptr->event.event_length == 1) {
                        if (modem_mgmt_check_index((ushort)*data_ptr,(char *)&modem_state_txt[0])) {
                                printf("%s", modem_state_txt[*data_ptr]);
                        } else {
                                printf("Unrecognized state:%d",
                                         (ushort)*data_ptr);
                        }
                   } else {
                     printf("Invalid length");
                   }
                   break; 
                case MCOM_CONNECTION_EVENT:
                   printf("\n  %8s: Connection event: ", buff);
                   if (ptr->event.event_length == 6) {
                         printf("TX/RX Speed = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET),
                                                    (char *)&modem_speed[0])) {
                                printf("%u",modem_speed[*(data_ptr+MCOM_TX_SPEED_OFFSET)]);
                         } else {
                                printf("Invalid Speed (%d)", 
                                  (ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET));
                         }
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET),
                                                    (char *)&modem_speed[0])) {
                                printf("/%u",modem_speed[*(data_ptr+MCOM_RX_SPEED_OFFSET)]);
                         } else {
                                printf("/Invalid Speed (%d)",
                                  (ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET));
                         }
                         printf(", Modulation = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_MODULATION_OFFSET),
                                                    (char *)&modulation_txt[0])) {
                                printf("%s",modulation_txt[*(data_ptr+MCOM_MODULATION_OFFSET)]);
                         } else {
                                printf("Invalid Modulation: %d", 
                                  (ushort)*(data_ptr+MCOM_MODULATION_OFFSET));
                         }
                         printf("\n\t    Direction = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_ORIG_ANSWER_OFFSET),
                                                    (char *)&direction_txt[0])) {
                                printf("%s",direction_txt[*(data_ptr+MCOM_ORIG_ANSWER_OFFSET)]);
                         } else {
                                printf("Invalid Direction: %d", 
                                  (ushort)*(data_ptr+MCOM_ORIG_ANSWER_OFFSET));
                         }
                         printf(", Protocol = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_PROTOCOL_OFFSET),
                                                    (char *)&protocol_txt[0])) {
                                printf("%s",protocol_txt[*(data_ptr+MCOM_PROTOCOL_OFFSET)]);
                         } else {
                                printf("Invalid Protocol: %d", 
                                  (ushort)*(data_ptr+MCOM_PROTOCOL_OFFSET));
                         }
                         printf(", Compression = " );
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_COMPRESSION_OFFSET),
                                                    (char *)&compress_txt[0])) {
                                printf("%s",compress_txt[*(data_ptr+MCOM_COMPRESSION_OFFSET)]);
                         } else {
                                printf("Invalid Compression: %d",
                                  (ushort)*(data_ptr+MCOM_COMPRESSION_OFFSET));
                         }
                   } else {
                     printf("Invalid length");
                   }
                   break; 
                case MCOM_CONNECTION_UPDATE_EVENT:
                   printf("\n  %8s: Connection update event: ", buff);
                   if (ptr->event.event_length == 3) {
                         printf("TX/RX Speed = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET),
                                                    (char *)&modem_speed[0])) {
                                printf("%u",modem_speed[*(data_ptr+MCOM_TX_SPEED_OFFSET)]);
                         } else {
                                printf("Invalid Speed (%d)",
                                  (ushort)*(data_ptr+MCOM_TX_SPEED_OFFSET));
                         }
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET),
                                                    (char *)&modem_speed[0])) {
                                printf("/%u",modem_speed[*(data_ptr+MCOM_RX_SPEED_OFFSET)]);
                         } else {
                                printf("/Invalid Speed (%d)",
                                  (ushort)*(data_ptr+MCOM_RX_SPEED_OFFSET));
                         } 
                         printf(", Modulation = ");
                         if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_MODULATION_OFFSET),
                                                    (char *)&modulation_txt[0])) {
                                printf("%s",modulation_txt[*(data_ptr+MCOM_MODULATION_OFFSET)]);
                         } else {
                                printf("Invalid Modulation: %d",
                                  (ushort)*(data_ptr+MCOM_MODULATION_OFFSET));
                         }
                   } else {
                     printf("Invalid length");
                   }
                   break; 
                case MCOM_CALL_PROGRESS_EVENT:
                   printf("\n  %8s: Call Progress event: ", buff);
                   if (ptr->event.event_length == 1) {
                           if (modem_mgmt_check_index((ushort)*data_ptr,(char *)&call_progress_txt[0])) {
                                  printf("%s", call_progress_txt[*data_ptr]);
                           } else {
                                  printf("Invalid Direction");
                           }
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_DTR_EVENT:
                   printf("\n  %8s: DTR event: ", buff);
                   if (ptr->event.event_length == 1) {
                     printf("%s", *data_ptr?"DTR On":"DTR Off"); 
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_MODEM_INACTIVE_EVENT:
                   printf("\n  %8s: Modem Activity event: ", buff);
                   if (ptr->event.event_length == 1) {
                     printf("%s", *data_ptr?"Data Inactive":"Data Active"); 
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_ANALOG_REPORT_EVENT:
                   printf("\n  %8s: Modem Analog signal event: ", buff);
                   if (ptr->event.event_length == 3) {
                     printf("TX = %d, RX = %d, Signal to noise = %d", 
                       *(data_ptr+MCOM_TX_LEVEL_OFFSET) * -1, 
                       *(data_ptr+MCOM_RX_LEVEL_OFFSET) * -1,
                       *(data_ptr+MCOM_SIGNAL_TO_NOISE_OFFSET)); 
                   } else {
                    printf("Invalid length");
                   }
                   break;
                case MCOM_END_CONNECTION_EVENT:
                   printf("\n  %8s: End connection event: ", buff);
                   if (ptr->event.event_length == 15) {
                     printf("Retransmits for MNP block (TX/RX) = %d/%d", 
                      *(data_ptr+MCOM_RETRANSMITS_TX_OFFSET), 
                      *(data_ptr+MCOM_RETRANSMITS_RX_OFFSET));
                     printf("\n\t    Duration = %d:%-02d:%-02d, ",
                      *(data_ptr+MCOM_DURATION_HOURS_OFFSET), 
                      *(data_ptr+MCOM_DURATION_MINUTES_OFFSET),
                      *(data_ptr+MCOM_DURATION_SECONDS_OFFSET));
                     printf("Number of TX/RX char = %d/%d",
                      GETLONG(data_ptr+MCOM_NUM_TX_CHARACTERS_OFFSET),
                      GETLONG(data_ptr+MCOM_NUM_RX_CHARACTERS_OFFSET));
                     printf("\n\t    Local Disc Reason = ");
		     if (*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET) == DISC_CONNECTED) {
			printf("Connected");
                     }
                     else if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET),
                                                     (char *)&disc_reason_txt[0])) {
                            printf("%s",disc_reason_txt[*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET)]);
                     } else {
                            printf("Undocumented Reason: %d",
                             (ushort)*(data_ptr+MCOM_DISC_REASON_LOCAL_OFFSET));
                     }
                     printf("\n\t    Remote Disc Reason = ");
                     if (*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET) == DISC_CONNECTED) {
                        printf("Connected");
                     } else if (modem_mgmt_check_index((ushort)*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET),
                                                     (char *)&disc_reason_txt[0])) {
                            printf("%s",disc_reason_txt[*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET)]);
                     } else {
                            printf("Undocumented Reason: %d",
                             (ushort)*(data_ptr+MCOM_DISC_REASON_REMOTE_OFFSET));
                     }
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_PHONE_NUMBER_EVENT:
                   printf("\n  %8s: Phone number event: ", buff);
                   if (ptr->event.event_length <= MAX_MODEM_STRING_LEN) {
                     printf("%s", data_ptr);
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_CALLER_ID_EVENT:
                   printf("\n  %8s: Caller ID event: ", buff);
                   if (ptr->event.event_length <= MAX_MODEM_STRING_LEN) {
                     printf("%s", data_ptr);
                   } else {
                     printf("Invalid length");
                   }
                   break;
                case MCOM_NO_RESPONSE_EVENT:
                    printf("\n  %8s: No poll response event: 0x%02x", buff,
                        *data_ptr);
                   break;
                case MCOM_EVENT_LOST_EVENT:
                    printf("\n  %8s: Status event lost: 0x%02x", buff,
                        *data_ptr);
                   break;
                case MCOM_PROTOCOL_ERR_EVENT:
                    printf("\n  %8s: Polling protocol error event: 0x%02x",
                        buff, *data_ptr);
                   break;
                default:
                   printf("\n  %8s: Unknown or lost event 0x%02x", 
                       buff, ptr->event.event_type);
                   break;
                } /* case poll event */
                break;
            case MCOM_DC_SESSION_EVENT_POLL:
		tmp_buff = chunk_malloc(modem_large_event_chunk);
                if (tmp_buff) {
                    strncpy(tmp_buff, ptr->event.data_ptr, 
                            ptr->event.event_length);
                    /* add a null terminate to the string */
                    tmp_buff[ptr->event.event_length] = '\0';
		    if (previousframe_type == MCOM_DC_SESSION) {
			printf("\n  %8s: DC session response: ", buff); 
			previousframe_type = MCOM_DC_SESSION_EVENT_POLL;
		    }
		    printf("%s", tmp_buff);
                    chunk_free(modem_large_event_chunk, tmp_buff);
                }
                break;
            case CSM_CALLED_NUMBER:
                if (ptr->event.event_length) {
                    tmp_buff = chunk_malloc(modem_large_event_chunk);
                    if (tmp_buff) {
                        strncpy(tmp_buff, ptr->event.data_ptr, 
                                ptr->event.event_length);
                        /* add a null terminate to the string */
                        tmp_buff[ptr->event.event_length] = '\0';
                        printf("\n  %8s: ISDN outgoing called number: %s", 
                            buff, tmp_buff);
                        chunk_free(modem_large_event_chunk, tmp_buff);
                     }
                } else {
                    printf("\n  %8s: ISDN outgoing called number: Unknown", 
                       buff);
                }
                break;
            case CSM_CALLER_NUMBER:
                if (ptr->event.event_length) {
                    tmp_buff = chunk_malloc(modem_large_event_chunk);
                    if (tmp_buff) {
                        strncpy(tmp_buff, ptr->event.data_ptr, 
                                ptr->event.event_length);
                        /* add a null terminate to the string */
                        tmp_buff[ptr->event.event_length] = '\0';
                        printf("\n  %8s: ISDN incoming caller number: %s", 
                            buff, tmp_buff);
                        chunk_free(modem_large_event_chunk, tmp_buff);
                    }
                } else {
                    printf("\n  %8s: ISDN incoming caller number: Unknown", 
                       buff);
                }
                break;
            default:
                printf("\n  %8s: Error in events log. ",buff);
                printf("Unknown frame type %d received", 
                                (ushort) ptr->event.frame_type);
                break;
            } /* case frame_type */
            if (ptr->event.event_length < MAX_MODEM_SMALL_EVENT_SIZE) {
                chunk_free(modem_small_event_chunk, ptr->event.data_ptr);
            } else {
                chunk_free(modem_large_event_chunk, ptr->event.data_ptr);
            }
            chunk_free(modem_event_hdr_chunk, ptr);
        }     /* while loop */
        free(tmp_eventQ);
    }
    printf("\n");
}


/*
 * =====================================================================
 * show_modem_group_log()
 *
 * Description:
 *
 * Parameters:
 * group - group number to show
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_group_log (int group)
{
    int slot;
    int port;
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int count;
 
    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /*
             * traverses memberQ and retrieve modem info to get slot/port
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            member = memberQ->qhead;
            for (count = 0; (member && (count < QUEUESIZE(memberQ))); count++) {
                modem_info_t *modem_info;

                modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem_info) {
                    slot = CSM_MODEM_SLOT(modem_info);
                    port = CSM_MODEM_PORT(modem_info);
                    show_modem_log(slot,port);
                }
                member = member->next;
                /* 
                 * give CPU some time to do other stuff since we
                 * can be hogging it when printing so many events
                 */
                process_sleep_for(1);
            }
        }
    }
}

/*
 * =====================================================================
 * show_modem_group_csm()
 *
 * Description:
 *
 * Parameters:
 * group - group number to show
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_group_csm (int group)
{
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int count;
    modem_info_t *modem_info;
 
    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /*
             * traverses memberQ and retrieve modem info
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            member = memberQ->qhead;
            for (count = 0; (member && (count < QUEUESIZE(memberQ))); count++) {
                modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem_info) 
                    csm_show_modem_info(modem_info);
                member = member->next;
            }
        }
    }
}

/*
 * =====================================================================
 * clear_modem()
 *
 * Description:
 * Hardware reset the modem by putting the modem in reset and
 * then take it out of reset 
 *
 * Parameters:
 * slot - slot of modem
 * port - port of modem
 * wait_for_reset - flag to pass to CSM to see if it should wait for the
 *                  reset to be finished
 *
 * Returns:
 * None
 * =====================================================================
 */
static void clear_modem (int slot, int port, boolean wait_for_reset)
{
    modem_info_t *modem_info;
 
    modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
    if (!modem_info) {
        return;
    }
 
    /*
     * Try to clear the line first
     */
    if (modem_info->modem_tty != NULL && modem_info->modem_tty != stdio) {
        terminate_line(modem_info->modem_tty);
    }
 
    /*
     * Put the reset modem request into the maintenance queue
     * This will ensure that line is cleared before trying to do
     * a reset
     */
    mai_recover_modem_from_failure(modem_info);
}

/*
 * =====================================================================
 * clear_modem_group()
 *
 * Description:
 * Do a modem hardware reset for all the modems in a group
 *
 * Parameters:
 * group - group number to show
 *
 * Returns:
 * None
 * =====================================================================
 */
static void clear_modem_group (int group)
{
    int slot;
    int port;
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int queue_size = 0, count;
 
    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /*
             * traverses memberQ and retrieve modem info to get slot/port
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            member = memberQ->qhead;
            queue_size = QUEUESIZE(memberQ);
            for (count = 0; (member && (count < queue_size)); count++) {
                modem_info_t *modem_info;

                modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem_info) {
                    slot = CSM_MODEM_SLOT(modem_info);
                    port = CSM_MODEM_PORT(modem_info);
                    clear_modem(slot, port, (count == queue_size-1));
                }
                member = member->next;
            }
        }
    }
}

/*
 * =====================================================================
 * show_modem_at_mode()
 *
 * Description:
 * display the active AT session and their TTY's 
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
static void show_modem_at_mode (void)
{
    int slot, port;
    modem_mgmt_at_mode_block *at_mode_block;

    automore_enable(NULL);
    printf("\nActive AT-MODE management sessions:");
    printf("\n   Modem    User's Terminal");
   
    at_mode_block = modem_mgmt_active_at_mode_blocks;
    while (at_mode_block) {
        slot = CSM_MODEM_SLOT(at_mode_block->modem_info);
        port = CSM_MODEM_PORT(at_mode_block->modem_info);
        switch (at_mode_block->tty->type) {
        case CTY_LINE:
            printf("\n  %-3d/%3d       %d cty %d", slot, port, 
                at_mode_block->tty->ttynum, at_mode_block->tty->ttynum);
            break;
        case TTY_LINE:
            printf("\n  %-3d/%3d       %d tty %d", slot, port, 
                at_mode_block->tty->ttynum, at_mode_block->tty->ttynum);
            break;
        case VTY_LINE:
            printf("\n  %-3d/%3d       %d vty %d", slot, port, 
                at_mode_block->tty->ttynum, at_mode_block->tty->ttynum-VTYBase);
            break;
        case AUX_LINE:
            printf("\n  %-3d/%3d       %d aux %d", slot, port, 
                at_mode_block->tty->ttynum, at_mode_block->tty->ttynum-AuxBase);
            break;
        default:
            printf("\n  %-3d/%3d          %d ???", slot, port, 
                at_mode_block->tty->ttynum);
            break;
        }

        at_mode_block = at_mode_block->next;
    }
    automore_disable();
}

/*
 * =====================================================================
 * clear_modem_stats()
 *
 * Description:
 * clear modem management counters
 *
 * Parameters:
 * slot - slot number of modem
 * port - port number of modem
 *
 * Returns:
 * None
 * =====================================================================
 */
static void clear_modem_stats (int slot, int port)
{
    modem_info_t *modem_info;
    modem_mgmt_info_t *modem_mgmt_info;
    modem_statistics_t *ms;

    modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
    if (modem_info) {
        modem_mgmt_info = modem_info->modem_mgmt_info;
        GET_TIMESTAMP(modem_info->last_clear_modem_counter);
        if (modem_mgmt_info) {
            ms = &(modem_mgmt_info->modem_stats);
            ms->modem_ic_complete_shadow       = ms->modem_ic_complete;
            ms->modem_oc_complete_shadow       = ms->modem_oc_complete;
            ms->modem_ic_total_shadow          = ms->modem_ic_total;
            ms->modem_oc_total_shadow          = ms->modem_oc_total;
            ms->modem_no_dial_tones_shadow     = ms->modem_no_dial_tones;
            ms->modem_dial_timeouts_shadow     = ms->modem_dial_timeouts;
            ms->modem_no_carriers_shadow       = ms->modem_no_carriers;
            ms->modem_link_failures_shadow     = ms->modem_link_failures;
            ms->modem_protocol_errors_shadow   = ms->modem_protocol_errors;
            ms->modem_lost_events_shadow       = ms->modem_lost_events;
            ms->modem_timeouts_shadow          = ms->modem_timeouts;
            ms->modem_75_connections_shadow    = ms->modem_75_connections;
            ms->modem_300_connections_shadow   = ms->modem_300_connections;
            ms->modem_600_connections_shadow   = ms->modem_600_connections;
            ms->modem_1200_connections_shadow  = ms->modem_1200_connections;
            ms->modem_2400_connections_shadow  = ms->modem_2400_connections;
            ms->modem_4800_connections_shadow  = ms->modem_4800_connections;
            ms->modem_7200_connections_shadow  = ms->modem_7200_connections;
            ms->modem_9600_connections_shadow  = ms->modem_9600_connections;
            ms->modem_12000_connections_shadow = ms->modem_12000_connections;
            ms->modem_14400_connections_shadow = ms->modem_14400_connections;
            ms->modem_16800_connections_shadow = ms->modem_16800_connections;
            ms->modem_19200_connections_shadow = ms->modem_19200_connections;
            ms->modem_21600_connections_shadow = ms->modem_21600_connections;
            ms->modem_24000_connections_shadow = ms->modem_24000_connections;
            ms->modem_26400_connections_shadow = ms->modem_26400_connections;
            ms->modem_28800_connections_shadow = ms->modem_28800_connections;
            ms->modem_31200_connections_shadow = ms->modem_31200_connections;
            ms->modem_33600_connections_shadow = ms->modem_33600_connections;
        }
        csm_clear_modem_info_counter(modem_info);
    }
}

/*
 * =====================================================================
 * clear_modem_group_stats()
 *
 * Description:
 * clear counters for one group
 *
 * Parameters:
 * group - clear all modems in this modem group number
 *
 * Returns:
 * None
 * =====================================================================
 */
static void clear_modem_group_stats (int group)
{
    int slot;
    int port;
    hwidbtype *hwidb = NULL;
    async_member_info *member;
    queuetype *memberQ = NULL;
    modem_group_t *tmp;
    int count;
 
    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /*
             * traverses memberQ and retrieve modem info to get slot/port
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            member = memberQ->qhead;
            for (count = 0; (member && (count < QUEUESIZE(memberQ))); count++) {
                modem_info_t *modem_info;

                modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem_info) {
                    slot = CSM_MODEM_SLOT(modem_info);
                    port = CSM_MODEM_PORT(modem_info);
                    clear_modem_stats(slot, port);
                }
                member = member->next;
            }
        }
    }
}

/*
 * =====================================================================
 * debug_modem()
 *
 * Description:
 * turn on/off modem management debugging for out of band port, csm to 
 * a modem or Modem call trace upload
 *
 * Parameters:
 * slot - slot number of modem
 * port - port number of modem
 * sense - true to turn on, false to turn off
 * which - oob, csm or modem trace debugging
 * trace_on - Modem call trace on normal, abnormal or all call termination
 *
 * Returns:
 * None
 * =====================================================================
 */
static 
void debug_modem (int slot, int port, boolean sense, int which, int trace_on)
{
   modem_info_t *modem_info;

   modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
   if (modem_info) {
       if (sense) {
		   switch (which) {
		   case DEBUG_MODEM_OOB:
               modem_info->modem_config |= CFG_MODEM_DEBUG_OOB;
				break;
			case DEBUG_MODEM_CSM: 
               if (!(modem_info->modem_config & CFG_MODEM_DEBUG_CSM)) {
                   modem_mgmt_csm_debug++;
                   modem_info->modem_config |= CFG_MODEM_DEBUG_CSM;
               }
				break;
			case DEBUG_MODEM_TRACE:
               modem_info->modem_config |= CFG_MODEM_DEBUG_TRACE;
               modem_info->trace_on = trace_on;
				break;
			default:
				break;
		   }
       } else {
		   switch (which) {
		   case DEBUG_MODEM_OOB:
               modem_info->modem_config &= ~CFG_MODEM_DEBUG_OOB;
				break;
			case DEBUG_MODEM_CSM:
               if (modem_info->modem_config & CFG_MODEM_DEBUG_CSM) {
                   modem_mgmt_csm_debug--;
                   modem_info->modem_config &= ~CFG_MODEM_DEBUG_CSM;
               }
				break;
			case DEBUG_MODEM_TRACE:
               modem_info->modem_config &= ~CFG_MODEM_DEBUG_TRACE;
               modem_info->trace_on = 0;
				break;
			default:
				break;
		   }
       }
   }
}

/*
 * =====================================================================
 * debug_modem_group()
 *
 * Description:
 * turn on/off modem management debugging for out of band port, csm
 * to a group of modems or Modem call trace upload.
 *
 * Parameters:
 * group - group #
 * sense - true to turn on, false to turn off
 * which - oob, csm or modem trace debugging
 * trace_on - Modem call trace on normal, abnormal or all call termination
 * 
 * Returns:
 * None
 * =====================================================================
 */
static 
void debug_modem_group (int group, boolean sense, int which, int trace_on)
{
    int slot;
    int port;
    hwidbtype *hwidb;
    queuetype *memberQ;
    async_member_info *member;
    modem_group_t *tmp;
    int count;

    /*
     * find the group hwidb
     */
    tmp = modem_group_list;
    while (tmp && (tmp->hwidb->unit < group)) {
        tmp = tmp->next;
    }
    if (tmp && (tmp->hwidb->unit == group)) {
        hwidb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (hwidb && !(hwidb->status & IDB_DELETED)) {
            /*
             * traverses memberQ and retrieve modem info to get slot/port
             */
            memberQ = &hwidb->firstsw->async_groupQ;
            member = memberQ->qhead;
            for (count = 0; (member && (count < QUEUESIZE(memberQ))); count++) {
                modem_info_t *modem_info;

                modem_info = idb_get_hwsb(member->hwidb, HWIDB_SB_MODEM_INFO);
                if (modem_info) {
                    slot = CSM_MODEM_SLOT(modem_info);
                    port = CSM_MODEM_PORT(modem_info);
                    debug_modem(slot, port, sense, which, trace_on);
                }
                member = member->next;
            }
        }
    }
}

/*
 * =====================================================================
 * line_modem_command()
 *
 * Description:
 * Parse the line subconfig commands for modem
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void line_modem_command (parseinfo *csb)
{
    modem_info_t *modem_info;
    int i;

    if (csb->nvgen) {
	modem_info = modem_ttynum_to_modem_info(csb->line1);
	if (!modem_info) {
            return;
	}
        switch (csb->which) {
        case MODEM_SHUTDOWN:
            nv_write(modem_info->modem_config & CFG_MODEM_SHUTDOWN, 
                     "%s", csb->nv_command);
            break;
        case MODEM_BAD:
            nv_write(modem_info->modem_config & CFG_MODEM_BAD, 
                     "%s", csb->nv_command);
            break;
        case MODEM_HOLD_RESET:
            nv_write(modem_info->modem_config & CFG_MODEM_HOLD_RESET, 
                     "%s", csb->nv_command);
            break;
        case MODEM_STATUS_POLL:
            nv_write(!(modem_info->modem_config & CFG_MODEM_STATUS_POLL), 
                     "no %s", csb->nv_command);
            break;
        case MODEM_AT_MODE_PERMIT:
            nv_write(!(modem_info->modem_config & CFG_MODEM_DC_SESSION), 
                     "no %s", csb->nv_command);
            break;
        default:
             bad_parser_subcommand(csb, csb->which);
             break;
        }

        return;
    }

    for (i = csb->line1; i < csb->line2; i++) {
        modem_info = modem_ttynum_to_modem_info(i);
	/* skip over lines that do not have modems */
	if (!modem_info) {
	    printf("\nNo Modem on this line. ");
	    printf("Command not executed for line %d.", i);
	    continue;
	}
        switch (csb->which) {
        case MODEM_SHUTDOWN:
            if (modem_shutdown(modem_info, csb->sense)) {
                if (csb->sense) {
                    modem_info->modem_config |= CFG_MODEM_SHUTDOWN;
                } else { 
                    modem_info->modem_config &= ~CFG_MODEM_SHUTDOWN;
                }
            }
            break;
        case MODEM_BAD:
            if (modem_bad(modem_info, csb->sense)) {
                if (csb->sense) {
                    modem_info->modem_config |= CFG_MODEM_BAD;
                } else { 
                   modem_info->modem_config &= ~CFG_MODEM_BAD;
                }
            } else {
               printf("\nModem is currently active. ");
               printf("Command not executed for line %d.", i);
            }
            break;
        case MODEM_HOLD_RESET:
            if (modem_reset(modem_info, csb->sense, TRUE)) {
                if (csb->sense) {
                   modem_info->modem_config |= CFG_MODEM_HOLD_RESET;
				   csm_set_modem_reset_status(modem_info);
                } else {
                   modem_info->modem_config &= ~CFG_MODEM_HOLD_RESET;
				   mai_restart_modem(modem_info);
                }
            } else {
                printf("\nModem is currently active. ");
                printf("Command not executed for line %d.", i);
            }
            break;
        case MODEM_STATUS_POLL:
            if (csb->sense) {
               modem_info->modem_config |= CFG_MODEM_STATUS_POLL;
            } else {
               modem_info->modem_config &= ~CFG_MODEM_STATUS_POLL;
            }
            break;
        case MODEM_AT_MODE_PERMIT:
            if (csb->sense) {
               modem_info->modem_config |= CFG_MODEM_DC_SESSION;
            } else {
               modem_info->modem_config &= ~CFG_MODEM_DC_SESSION;
            }
            break;
        default:
            bad_parser_subcommand(csb, csb->which);
            break;
        }
    }
}

/*
 * =====================================================================
 * forcing_a_law()
 *
 * Description:
 * force the modem and PRI board to be A-law or u-law
 *
 * Parameters:
 * type
 *
 * Returns:
 * None
 * =====================================================================
 */
static void forcing_a_law (int sense)
{
    modem_slot_t *slotinfo;
    modem_module_cookie_t *module;
    int slot, i;

    for (slot = 0; slot < modem_pool->max_slots; slot++) {
		slotinfo = modem_pool->modem_slot[slot];
		if (slotinfo == NULL)
	    	continue;
		/*
	 	* Set the codec u/A-law bit.
	 	*/
		/* First find a valid modem module cookie */
		for (i = 0, module = slotinfo->modem_cookie->module; 
			i < slotinfo->modem_cookie->carrier.max_modules; module++)
	    	if (module->number_of_modems != 0xFF)
				break;
		if (i < slotinfo->modem_cookie->carrier.max_modules) {
			modem_pool->encoding = (sense) ? A_LAW_ENCODING : U_LAW_ENCODING;
        	if (((modem_pool->encoding == A_LAW_ENCODING) &&
            	!(module->option_bits & VENDOR_CODEC_U_LAW_HIGH)) ||
            	((modem_pool->encoding != A_LAW_ENCODING) &&
            	(module->option_bits & VENDOR_CODEC_U_LAW_HIGH))) {
        		slotinfo->modem_reg->mdb_control1 |= MDB_CNTL1_CODEC_AU_LAW;
        	} else {
        		slotinfo->modem_reg->mdb_control1 &=~ MDB_CNTL1_CODEC_AU_LAW;
        	}
        }
	}
}
/*
 * =====================================================================
 * modem_mgmt_command()
 *
 * Description:
 * Parse the modem global configure commands
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_mgmt_command (parseinfo *csb)
{
    int slot, port;
    int modem_in_slot;
    modem_info_t *modem_info;
    int tmp;

    if (csb->nvgen) {
        switch (csb->which) {
	case MODEM_FAST_ANSWER:
            if (!(modem_fast_answer))
                nv_write(TRUE, "no %s", csb->nv_command);
            break;
        case MODEM_STARTUP_TEST:
            if (modem_startup_test)
                nv_write(TRUE, "%s", csb->nv_command);
            break;
        case MODEM_RECOVERY_TIME:
	    if (modem_pool) {
                tmp = modem_pool->wdt_timeout_val;
                nv_write(tmp != CSM_DEFAULT_WATCH_DOG_TIME_OUT,
                                "%s %d", csb->nv_command, tmp / ONEMIN);
            }
            break;
        case MODEM_BUFFER_SIZE:
            nv_write(modem_buffer_size != MODEM_BUFFER_SIZE_DEFAULT, 
                     "%s %d", csb->nv_command, modem_buffer_size);
            break;
        case MODEM_POLL_RETRY:
            nv_write(modem_poll_retry != MODEM_POLL_RETRY_DEFAULT, 
                     "%s %d", csb->nv_command, modem_poll_retry);
            break;
        case MODEM_POLL_TIME:
            nv_write(modem_poll_time != MODEM_POLL_TIME_DEFAULT, 
                     "%s %d", csb->nv_command, modem_poll_time / ONESEC);
            break;
        case MODEM_A_LAW:
            nv_write(modem_forcing_a_law != MODEM_NOT_FORCING_A_LAW, "%s", csb->nv_command);
            break;
        case MODEM_BUSYOUT_RBS_DSX0:
            nv_write(modem_pool->min_free_modem_threshold != DEFAULT_FREE_MODEM_THRESHOLD,
                     "%s %d", csb->nv_command, modem_pool->min_free_modem_threshold);
            break;
        default:
            bad_parser_subcommand(csb, csb->which);
            break;
        }
        return;
    }

    switch (csb->which) {
    case MODEM_STARTUP_TEST:
        if (csb->sense) {
            modem_startup_test = TRUE;
            mai_enable_poweron_b2b_test();
	}
        else
            modem_startup_test = FALSE;
        break;
    case MODEM_RECOVERY_TIME:
	if (modem_pool) {
        	if (csb->sense) {
            		modem_pool->wdt_timeout_val = GETOBJ(int,1);
	    		modem_pool->wdt_timeout_val *= ONEMIN;
	    	}
        	else {
            		modem_pool->wdt_timeout_val = CSM_DEFAULT_WATCH_DOG_TIME_OUT;
		}
	}
        break;
    case MODEM_BUFFER_SIZE:
        tmp = modem_buffer_size;
        if (csb->sense)
            modem_buffer_size = GETOBJ(int,1);
        else
            modem_buffer_size = MODEM_BUFFER_SIZE_DEFAULT;

        if (modem_buffer_size != tmp) {
            if (modem_pool) {
              for (slot=0; slot < modem_pool->max_slots; slot++) {
                if (modem_pool->modem_slot[slot]) {
                    modem_in_slot =
                        modem_pool->modem_slot[slot]->total_modem_num;
                    for (port = 0; port < modem_in_slot; port++) {
                        modem_info = CSM_MODEM_INFO(slot,port);
                        modem_mgmt_create_history_eventQ(modem_info);
                        /*
                         * give up CPU for other other processes
                         */
                        process_sleep_for(1);
                    }
                }
              }
            }
        }
        break;
    case MODEM_POLL_RETRY:
        if (csb->sense)
            modem_poll_retry = GETOBJ(int,1);
        else
            modem_poll_retry = MODEM_POLL_RETRY_DEFAULT;
        break;
    case MODEM_POLL_TIME:
        /* save the time in milliseconds for the state machines */
        if (csb->sense)
            modem_poll_time = GETOBJ(int,1) * ONESEC;
        else
            modem_poll_time = MODEM_POLL_TIME_DEFAULT;
        break;
    case MODEM_A_LAW:
        if (csb->sense)
           modem_forcing_a_law = csb->which;
        else
            modem_forcing_a_law = MODEM_NOT_FORCING_A_LAW;
		forcing_a_law(csb->sense);
        break;
    case MODEM_FAST_ANSWER:
        if (csb->sense)
            modem_fast_answer = TRUE;
        else
	    modem_fast_answer = FALSE;
        break;
    case MODEM_BUSYOUT_RBS_DSX0:
        if (csb->sense)
            modem_pool->min_free_modem_threshold = GETOBJ(int,1);
        else
            modem_pool->min_free_modem_threshold = DEFAULT_FREE_MODEM_THRESHOLD;
        break;
    default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * =====================================================================
 * show_modem_command()
 *
 * Description:
 * Parse the "show modem" command to call appropriate routines to show
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void show_modem_command (parseinfo *csb)
{
    int show_option = GETOBJ(int,1);
    int slot, port, group;
    int modem_in_slot;
    modem_info_t *modem_info;
    modem_group_t *current_group;
    hwidbtype *hwidb;

    switch (show_option) {
    case SHOW_MODEM_STATS:
    case SHOW_MODEM_CSM:
    case SHOW_MODEM_LOG:
        if (csb->which == SHOW_MODEM) {
            if (GETOBJ(int,5)) {
                /* show individual modem */
                slot = GETOBJ(int,2);
                port = GETOBJ(int,3);
                modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
                if (!modem_info) {
                    printf("\nModem %d/%d doesn't exist.", slot, port);
                    return;
                }
                switch (show_option) {
                case SHOW_MODEM_STATS:
                    show_modem_stats(slot, port);
                    break;
                case SHOW_MODEM_CSM:
                    automore_enable(NULL);
                    csm_show_modem_info(modem_info);
                    automore_disable();
                    break;
                case SHOW_MODEM_LOG:
                    automore_enable(NULL);
                    show_modem_log(slot, port);
                    automore_disable();
                    break;
                }
            } else {
                /* show all modems */
                switch (show_option) {
                case SHOW_MODEM_STATS:
                    show_modem_all_stats();
                    break;
                case SHOW_MODEM_CSM:
                    automore_enable(NULL);
                    if (modem_pool) {
                      for (slot = 0; slot < modem_pool->max_slots; slot++) {
                        if (modem_pool->modem_slot[slot]) {
                            modem_in_slot = 
                                modem_pool->modem_slot[slot]->total_modem_num;
                            for (port = 0; port < modem_in_slot; port++) {
                                modem_info = CSM_MODEM_INFO(slot, port);
                                csm_show_modem_info(modem_info);
                            }
                        }
                      }
                    }
                    automore_disable();
                    break;
                case SHOW_MODEM_LOG:
                    automore_enable(NULL);
                    if (modem_pool) {
                      for (slot = 0; slot < modem_pool->max_slots; slot++) {
                        if (modem_pool->modem_slot[slot]) {
                            modem_in_slot =
                                modem_pool->modem_slot[slot]->total_modem_num;
                            for (port = 0; port < modem_in_slot; port++) {
                               show_modem_log(slot, port);
                               /* 
                                * give CPU some time to do other stuff since we
                                * can be hogging it when printing so many events
                                */
                               process_sleep_for(1);
                            }
                        }
                      }
                    }
                    automore_disable();
                    break;
                }
            }
        } else {
            if (GETOBJ(int,6)) {
                /* show individual modem group */
                group = GETOBJ(int,4);
                /*
                 * find the group hwidb
                 */
                current_group = modem_group_list;
                while (current_group && (current_group->hwidb->unit < group)) {
                    current_group = current_group->next;
                }
                if (current_group) {
                    if (current_group->hwidb->unit != group) {
                        printf("\nModem group %d doesn't exist.", group);
                        return;
                    } else {
                        hwidb = current_group->hwidb;
                        /*
                         * check to see if the group has been deleted
                         */
                        if (hwidb && (hwidb->status & IDB_DELETED)) {
                            printf("\nModem group %d doesn't exist.", group);
                            return;
                        }
                    }
                } else {
                    printf("\nModem group %d doesn't exist.", group);
                    return;
                }

                switch (show_option) {
                case SHOW_MODEM_STATS:
                    show_modem_group_stats(group);
                    break;
                case SHOW_MODEM_CSM:
                    automore_enable(NULL);
                    show_modem_group_csm(group);
                    automore_disable();
                    break;
                case SHOW_MODEM_LOG:
                    automore_enable(NULL);
                    show_modem_group_log(group);
                    automore_disable();
                    break;
                }
            } else {
                /* show all modem groups */
                switch (show_option) {
                case SHOW_MODEM_STATS:
                    show_modem_all_group_stats();
                    break;
                case SHOW_MODEM_CSM:
                    automore_enable(NULL);
                    current_group = modem_group_list;
                    while (current_group) {
                        if (current_group->hwidb &&
                            !(current_group->hwidb->status & IDB_DELETED)) {
                            show_modem_group_csm(current_group->hwidb->unit);
                        }
                        current_group = current_group->next;
                    }
                    automore_disable();
                    break;
                case SHOW_MODEM_LOG:
                    automore_enable(NULL);
                    current_group = modem_group_list;
                    while (current_group) {
                        if (current_group->hwidb &&
                            !(current_group->hwidb->status & IDB_DELETED)) {
                            show_modem_group_log(current_group->hwidb->unit);
                        }
                        current_group = current_group->next;
                    }
                    automore_disable();
                    break;
                }
            }
        }
        break; 
    case SHOW_MODEM_SUMMARY:
        show_modem_system_summary();
        break; 
    case SHOW_MODEM_VERSION:
        show_modem_version_info();
        break;
    case SHOW_MODEM_AT_MODE:
        show_modem_at_mode();
        break; 
    default:
        bad_parser_subcommand(csb, csb->which);
        break; 
    }
}

/*
 * =====================================================================
 * clear_modem_command()
 *
 * Description:
 * Parse the "clear modem" command and call appropriate routines to
 * either clear the modem counters or reset the modem(s).
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void clear_modem_command (parseinfo *csb)
{
    int clear_option = GETOBJ(int,1);
    int slot, port, group;
    int modem_in_slot;
    char buffer[80];
    modem_group_t *current_group;
    modem_info_t *modem_info;
    hwidbtype *hwidb;

    if (csb->which == CLEAR_MODEM) {
        if (GETOBJ(int,5)) {
            slot = GETOBJ(int,2);
            port = GETOBJ(int,3);
            modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
            if (!modem_info) {
                printf("\nModem %d/%d doesn't exist.", slot, port);
                return;
            }
            if (clear_option == CLEAR_MODEM_HW) {
                printf("This command will disconnect any active calls.\n");
                printf("Clear (reset) modem %d/%d ? ",slot,port);
                if (confirm("")) {
                    /* reset individual modem */
                    clear_modem(slot, port, TRUE);
                }
            } else {
                /* clear individual modem stats */
                sprintf(buffer, "%d/%d", slot, port);
                printf("Clear \"show modem\" counters on modem %s ", buffer);
                if (confirm("")) {
                    clear_modem_stats(slot, port);
                    errmsg(&msgsym(COUNTERS, CLEAR), "modem", buffer,
                        user_on_stdio_str()); 
                }
            }
        } else {
            if (clear_option == CLEAR_MODEM_HW) {
                printf("This command will disconnect any active calls.\n");
                printf("Clear (reset) all modems? ");
                if (confirm("")) {
                    /* reset all modems */
                    if (modem_pool) {
                      for (slot = 0; slot < modem_pool->max_slots; slot++) {
                        if (modem_pool->modem_slot[slot]) {
                            modem_in_slot = 
                                modem_pool->modem_slot[slot]->total_modem_num;
                            for (port = 0; port < modem_in_slot; port++) {
                                clear_modem(slot, port, TRUE);
                            }
                        }
                      }
                    }
                }
            } else {
                /* clear all modems stats */
                if (confirm("Clear \"show modem\" counters on all modems ")) {
                    if (modem_pool) {
                      for (slot = 0; slot < modem_pool->max_slots; slot++) {
                        if (modem_pool->modem_slot[slot]) {
                            modem_in_slot =
                                modem_pool->modem_slot[slot]->total_modem_num;
                            for (port = 0; port < modem_in_slot; port++) {
                                clear_modem_stats(slot, port);
                            }
                        }
                      }
                    }
                    errmsg(&msgsym(COUNTERS, CLEAR), "all", "modems",
                        user_on_stdio_str()); 
                }
            }
        }
    } else {
        if (GETOBJ(int,6)) {
            group = GETOBJ(int,4);
            /*
             * find the group hwidb
             */
            current_group = modem_group_list;
            while (current_group && (current_group->hwidb->unit < group)) {
                current_group = current_group->next;
            }
            if (current_group) {
                if (current_group->hwidb->unit != group) {
                    printf("\nModem group %d doesn't exist.", group);
                    return;
                } else {
                    hwidb = current_group->hwidb;
                    /*
                     * check to see if the group has been deleted
                     */
                    if (hwidb && (hwidb->status & IDB_DELETED)) {
                        printf("\nModem group %d doesn't exist.", group);
                        return;
                    }
                }
            } else {
                printf("\nModem group %d doesn't exist.", group);
                return;
            }
            if (clear_option == CLEAR_MODEM_HW) {
                sprintf(buffer, "%d", group);
                printf("This command will disconnect any active calls.\n");
                printf("Clear (reset) all modems in group %s?",buffer);
                if (confirm("")) {
                    /* reset all modems in a group */
                    clear_modem_group(group);
                }
            } else {
                /* clear modems stats in group */
                sprintf(buffer, "%d", group);
                printf("Clear \"show modem\" counters for modems in group %s ",
                       buffer);
                if (confirm("")) {
                    clear_modem_group_stats(group);
                    errmsg(&msgsym(COUNTERS, CLEAR), "modems in group", buffer,
                        user_on_stdio_str()); 
                }
            }
        } else {
            if (clear_option == CLEAR_MODEM_HW) {
                printf("This command will disconnect any active calls.\n");
                printf("Clear (reset) all modem in all groups? ");
                if (confirm("")) {
                    /* reset all modems in all modem groups */
                    current_group = modem_group_list;
                    while (current_group) {
                        if (current_group->hwidb && 
                            !(current_group->hwidb->status & IDB_DELETED)) {
                            clear_modem_group(current_group->hwidb->unit);
                        }
                        current_group = current_group->next;
                    }
                }
            } else {
                /* clear modems stats in all groups */
                printf("Clear \"show modem\" counters for all modem groups ");
                if (confirm("")) {
                    current_group = modem_group_list;
                    while (current_group) {
                        if (current_group->hwidb &&
                            !(current_group->hwidb->status & IDB_DELETED)) {
                            clear_modem_group_stats(current_group->hwidb->unit);
                        }
                        current_group = current_group->next;
                    }
                    errmsg(&msgsym(COUNTERS, CLEAR), "modems", "in all groups",
                        user_on_stdio_str()); 
                }
            }
        }
    }
}

/*
 * =====================================================================
 * modem_back2back_test_command()
 *
 * Description:
 * Parse the "test modem" command and call appropriate routines to
 * put the modems in back-to-back testing.
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_back2back_test_command (parseinfo *csb)
{
    modem_info_t *modem1, *modem2;
    int slot1 = GETOBJ(int,1);
    int port1 = GETOBJ(int,2);
    int slot2 = GETOBJ(int,3);
    int port2 = GETOBJ(int,4);

    modem1 = CSM_MODEM_INFO_OR_NULL(slot1,port1);
    if (!modem1) {
        printf("\nModem %d/%d doesn't exist.", slot1, port1);
        return;
    }
    modem2 = CSM_MODEM_INFO_OR_NULL(slot2,port2);
    if (!modem2) {
        printf("\nModem %d/%d doesn't exist.", slot2, port2);
        return;
    }
    mai_back2back_test(modem1, modem2);
}

/*
 * =====================================================================
 * modem_at_mode_command()
 *
 * Description:
 * Parse the "modem at-mode" command and call appropriate routines to
 * establish DC session to the modem.
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void modem_at_mode_command (parseinfo *csb)
{
    modem_info_t *modem_info;
    int slot = GETOBJ(int,1);
    int port = GETOBJ(int,2);
 
    modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
    if (!modem_info) {
        printf("\nModem %d/%d doesn't exist.", slot, port);
        return;
    }

    if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
        if (modem_info->modem_mgmt_info && 
            !(modem_info->modem_mgmt_info->dc_session_flg)) {
    	    if (modem_info->initialized) { 
	      if (modem_info->modem_mgmt_info->clear_dc ==
		 CLEARED_ALLOW_NEW_DC)  {
		modem_mgmt_exec_at_mode(modem_info);
	      } else {
		printf("\nModem %d/%d cleanup pending for earlier DC"
		       " session", slot, port);
	      } 
	    } else {
	      printf("\nModem %d/%d oob is not initialized", slot,
		       port);
	    }
        } else {
            printf("\nAlready in Direct Connect session for modem %d/%d.", 
                   slot, port);
        }
    } else {
        printf("\nDirect Connect session not allowed for modem %d/%d.", 
                slot, port);
    }
}

/*
 * =====================================================================
 * clear_modem_at_mode_command()
 *
 * Description:
 * Parse the "clear modem at-mode" command and call appropriate routines
 * to tear down a DC session to the modem.
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void clear_modem_at_mode_command (parseinfo *csb)
{
    modem_info_t *modem_info;
    ushort slot = GETOBJ(int,1);
    ushort port = GETOBJ(int,2);

    modem_info = CSM_MODEM_INFO_OR_NULL(slot,port);
    if (!modem_info) {
        printf("\nModem %d/%d doesn't exist.", slot, port);
        return;
    }
    if (confirm("")) {
       modem_mgmt_start_closing_dc_session(FALSE, slot, port, modem_info);
    }
}

/*
 * =====================================================================
 * debug_modem_command()
 *
 * Description:
 * Parse the "debug modem" command and call appropriate routines to
 * for debugging purposes.
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void debug_modem_command (parseinfo *csb)
{
    int slot;
    int port;
    int group;
    int modem_in_slot;
    int debug_option = csb->which;
    int trace_on;
    modem_group_t *current_group;
    modem_info_t *modem_info;
    hwidbtype *hwidb;

    csb->which = GETOBJ(int,1);
    trace_on = GETOBJ(int,7);

    if (debug_option == DEBUG_MODEM) {
        if (GETOBJ(int,5)) {
            /* debug individual modem */
            slot = GETOBJ(int,2);
            port = GETOBJ(int,3);
            modem_info = CSM_MODEM_INFO_OR_NULL(slot, port);
            if (!modem_info) {
                printf("\nModem %d/%d doesn't exist.", slot, port);
                return;
            }
            debug_modem(slot, port, csb->sense, csb->which, trace_on);
        } else {
            /* debug all modems */
            if (modem_pool) {
              for (slot = 0; slot < modem_pool->max_slots; slot++) {
                if (modem_pool->modem_slot[slot]) {
                    modem_in_slot =
                        modem_pool->modem_slot[slot]->total_modem_num;
                    for (port = 0; port < modem_in_slot; port++) {
                      debug_modem(slot, port, csb->sense, csb->which, trace_on);
                    }
                }
              }
            }
        }
    } else {
        if (GETOBJ(int,6)) {
            /* debug all modems in a specified group */
            group = GETOBJ(int,4);
            /*
             * find the group hwidb
             */
            current_group = modem_group_list;
            while (current_group && (current_group->hwidb->unit < group)) {
                current_group = current_group->next;
            }
            if (current_group) {
                if (current_group->hwidb->unit != group) {
                    printf("\nModem group %d doesn't exist.", group);
                    return;
                } else {
                    hwidb = current_group->hwidb;
                    /*
                     * check to see if the group has been deleted
                     */
                    if (hwidb && (hwidb->status & IDB_DELETED)) {
                        printf("\nModem group %d doesn't exist.", group);
                        return;
                    }
                }
            } else {
                printf("\nModem group %d doesn't exist.", group);
                return;
            }

            debug_modem_group(group, csb->sense, csb->which, trace_on);
        } else {
            /* debug only modems in all modem groups */
            current_group = modem_group_list;
            while (current_group) {
                if (current_group->hwidb &&
                    !(current_group->hwidb->status & IDB_DELETED)) {
                    debug_modem_group(current_group->hwidb->unit, 
                                      csb->sense, csb->which, trace_on);
                }
		current_group = current_group->next;
            }
        }
    }

    /* Everything else is generic */
    debug_command(csb);
}


