/* $Id: dtimer.c,v 1.1.4.1 1996/05/09 15:01:01 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/dtimer.c,v $
 *------------------------------------------------------------------
 * dtimer.c -- Timer Interrupt Functions for diagnostics
 *
 * 1993, rlb
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dtimer.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:01  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:59  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:10  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:54  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:29  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains procedures related to "Timer Interrupt Functions" that  */
/* are included as part of CALYPSO Diagnostics.                               */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

#include	"types.h"
#include	"constant.h"
#include	"clock.h"
#include	"mfp.h"
#include	"hw.h"

#define  ATM

extern	struct realtime sys_clock;
#ifdef DIAGTEST
extern	int	diag_test_mode;
#endif /* DIAGTEST */
extern	int	ctrl_reg38_img;
extern	int	sys_clock_update;
extern	int	diag_virtual_uart_mode;

#ifndef BOOT
extern	int	sys_eeprom_valid;
extern	char	sys_oem_string[], sys_model[];
extern	uChar	sys_mac_base[];
extern	uInt32	sys_serial_no;
extern	uInt16	sys_hw_version_high, sys_hw_version_low;
#endif /* BOOT */

	volatile uInt32	system_time, curr_system_time, system_tick_time;

#define	PHY_PORT_A	1
#define	PHY_PORT_B	2

void c_dtimer_A_hdlr()
{
	register int	i;

	system_time++; system_tick_time++;

	/* Punch Watch Dog */
	*(uChar *)CNTL_REG_17 = 1;
	*(uChar *)CNTL_REG_17 = 0;

#if  0   /* ATM */

	if (sys_clock_update == TRUE) {
	   *((uInt32 *) COMM_NMP_SYSTIME) = system_time;
	   *((uInt32 *) COMM_NMP_TICK_TIME) = system_tick_time;
	}
	
#ifdef DIAGTEST
	if (diag_test_mode == FALSE) {
#endif /* DIAGTEST */
	   if (diag_virtual_uart_mode == TRUE) {
	      if (system_time%0x100 == 0) {
                 /* blink LED */
	         ctrl_reg38_img ^= STATUS_GREEN_LED_ON_MASK; 
                 *(uChar *)CNTL_REG_38 = ctrl_reg38_img;
              }
	   }
	   else {
	      if (system_time%0x400 == 0) {
	         ctrl_reg38_img ^= STATUS_GREEN_LED_ON_MASK; 
                 *(uChar *)CNTL_REG_38 = ctrl_reg38_img;
              }
	   }
#ifdef DIAGTEST
        }
	else {
	   if (system_time%0x40 == 0) {
	      ctrl_reg38_img ^= STATUS_GREEN_LED_ON_MASK; 
              *(uChar *)CNTL_REG_38 = ctrl_reg38_img;
           }
        }
#endif /* DIAGTEST */

	/* Punch Watch Dog */
	*(uChar *)CNTL_REG_17 = WATCH_DOG_PUNCH_MASK;
	*(uChar *)CNTL_REG_17 = ~WATCH_DOG_PUNCH_MASK;

#ifndef BOOT

	if (*((uInt32 *) COMM_DMP_TRAP) == 0xdeadface) {
	   handleDMPtrap();
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_BYPASS) {
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_A) {
	      configure_phy_in_Bypass_mode(PHY_PORT_A);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_A;
	   }
	   else
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_B) {
	      configure_phy_in_Bypass_mode(PHY_PORT_B);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_B;
	   }
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_BYPASS;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_LMLOOP) {
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_A) {
	      configure_phy_in_LM_Loopback_mode(PHY_PORT_A);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_A;
	   }
	   else
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_B) {
	      configure_phy_in_LM_Loopback_mode(PHY_PORT_B);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_B;
	   }
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_LMLOOP;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_EBLOOP) {
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_A) {
	      configure_phy_in_EB_Loopback_mode(PHY_PORT_A);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_A;
	   }
	   else
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_B) {
	      configure_phy_in_EB_Loopback_mode(PHY_PORT_B);
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_B;
	   }
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_EBLOOP;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_XTLOOP) {
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_A) {
	      if (configure_phy_in_XT_Loopback_mode(PHY_PORT_A))
	         *((uInt32 *) COMM_STATUS) |= COMM_STATUS_MEDIA_NOT_PRESENT;
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_A;
	   }
	   else
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_B) {
	      if (configure_phy_in_XT_Loopback_mode(PHY_PORT_B))
	         *((uInt32 *) COMM_STATUS) |= COMM_STATUS_MEDIA_NOT_PRESENT;
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_B;
	   }
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_XTLOOP;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_PMDLOOP) {
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_A) {
	      if (configure_phy_in_PMD_Loopback_mode(PHY_PORT_A))
	         *((uInt32 *) COMM_STATUS) |= COMM_STATUS_MEDIA_NOT_PRESENT;
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_A;
	   }
	   else
	   if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_PHY_B) {
	      if (configure_phy_in_PMD_Loopback_mode(PHY_PORT_B))
	         *((uInt32 *) COMM_STATUS) |= COMM_STATUS_MEDIA_NOT_PRESENT;
	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_B;
	   }
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_PHY_PMDLOOP;
	}
	else
#endif /* BOOT */
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_DISABLE_ETHERNET_LINKS) {
	   disable_ethernet_links();
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_DISABLE_ETHERNET_LINKS;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_ENABLE_ETHERNET_LINKS) {
	   enable_ethernet_links();
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_ENABLE_ETHERNET_LINKS;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_RESET_ETHERNET_BANK_A) {
	   reset_ethernet_bank_a();
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_RESET_ETHERNET_BANK_A;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_RESET_ETHERNET_BANK_B) {
	   reset_ethernet_bank_b();
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_RESET_ETHERNET_BANK_B;
	}
#ifndef BOOT
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_GET_OEM_STRING) {
	   if (sys_eeprom_valid)
	      strcpy((uChar *) COMM_OEM_STRING, sys_oem_string);
	   else
	      *((uInt32 *) COMM_STATUS) |= COMM_STATUS_EEPROM_NOT_VALID;
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_GET_OEM_STRING;
	}
	else
	if (*((uInt32 *) COMM_STATUS) & COMM_STATUS_GET_EEPROM_DATA) {
	   if (sys_eeprom_valid) {
	      strcpy((char *) COMM_MODEL_STRING, sys_model);
	      for (i = 0; i < 6; i++)
	         *((uChar *) (COMM_MAC_BASE+i)) = sys_mac_base[i];
	      *((uInt32 *) COMM_SERIAL_NO) = sys_serial_no;
	      *((uInt16 *) COMM_HW_VERSION_HIGH) = sys_hw_version_high;
	      *((uInt16 *) COMM_HW_VERSION_LOW) = sys_hw_version_low;
	   }
	   else
	      *((uInt32 *) COMM_STATUS) |= COMM_STATUS_EEPROM_NOT_VALID;
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_GET_EEPROM_DATA;
	}
#endif /* BOOT */

#endif  /* ATM */
}

void dtimer_init()
{
	int		hour, minute, second;

	*((uInt32 *) COMM_YEAR)   = sys_clock.year;
	*((uInt32 *) COMM_MONTH)  = sys_clock.month;
	*((uInt32 *) COMM_DAY)    = sys_clock.date;

	hour = sys_clock.hour;
	minute = sys_clock.minute;
	second = sys_clock.second;
	system_time = curr_system_time = hour*3600000 +
	                                 minute*60000 +
	                                 second*1000;
	*((uInt32 *) COMM_NMP_SYSTIME) = system_time;

	system_tick_time = 0;
	*((uInt32 *) COMM_NMP_TICK_TIME) = system_tick_time;

#ifndef SCC_NEW
#define	SYSTEM_TIMER_INTERVAL	246
#else /* SCC_NEW */
#define	SYSTEM_TIMER_INTERVAL	230
#endif /* SCC_NEW */
	/* Init Timer A */
	*((uChar *) (MFP_BASE + MFP_TACR)) = TIMER_AB_RESET;
	*((uChar *) (MFP_BASE + MFP_TADR)) = SYSTEM_TIMER_INTERVAL;
#ifndef SCC_NEW
	*((uChar *) (MFP_BASE + MFP_TACR)) = TIMER_DELAY_MODE_10;
#else /* SCC_NEW */
	*((uChar *) (MFP_BASE + MFP_TACR)) = TIMER_DELAY_MODE_16;
#endif /* SCC_NEW */
	*((uChar *) (MFP_BASE + MFP_IPRA)) &= ~EN_TIMER_A;
	*((uChar *) (MFP_BASE + MFP_IMRA)) |= EN_TIMER_A;
}	

void dtimer_term()
{
	*((uChar *) (MFP_BASE + MFP_TACR)) = TIMER_AB_RESET;
	*((uChar *) (MFP_BASE + MFP_TADR)) = 0;
	*((uChar *) (MFP_BASE + MFP_IMRA)) &= ~EN_TIMER_A;
	*((uChar *) (MFP_BASE + MFP_IPRA)) &= ~EN_TIMER_A;
}	


