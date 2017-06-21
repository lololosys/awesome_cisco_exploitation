/* $Id: diagio.c,v 1.1.4.1 1996/05/09 15:00:59 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/diagio.c,v $
 *------------------------------------------------------------------
 * diagio.c -- I/O Utility Functions for diagnostics
 *
 * 1993, ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diagio.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:59  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:08  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:51  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:26  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains procedures related to "I/O Utility Functions" that are  */
/* included as part of CALYPSO Diagnostics.                                   */
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
#include	"scc_if.h"
#include	"clock.h"
#include	"globdef.h"
#include	"hw.h"

#define  ATM

extern	struct realtime	sys_clock;
extern	char		sys_date[];
extern	char		sys_time[];
extern	int		sys_clock_display, sys_clock_update;
extern	uInt32	system_time, curr_system_time;
extern	int		diag_virtual_uart_mode;
#ifndef BOOT
extern	int		switchtoDMP_DConsole, switchtoDMP_VConsole;
#endif /* BOOT */

#define	DMP_HEARTBEAT_TIMEOUT	1000000

/*--- function prototypes ---*/
extern int  getdbgch(int, char *);


/*--------------------------------------------------------------------
 * checkchar_p()
 *--------------------------------------------------------------------*/
int checkchar_p()
{
	char		c;
#if  1
     if (getdbgch(0, &c) < 0)
        return(0);
#else   /* !ATM */

	if (dgetch(&c) == 0)
	   return(0);
#endif  /* ATM */

	return((int) c);
}

/*--------------------------------------------------------------------
 * putchar_p()
 *--------------------------------------------------------------------*/
void putchar_p(
	char		c)
{
#ifndef SCC_NEW
	scc_output(0, &c, 1);
#else /* SCC_NEW */
	new_scc_output(0, &c, 1);
#endif /* SCC_NEW */
}

/*--------------------------------------------------------------------
 * getchar_p()
 *--------------------------------------------------------------------*/
int getchar_p()
{
	char		c;
	int		i;

/* #ifdef  ATM */
#if  1
   while (getdbgch(0,&c) < 0)
   {
      Bgfunc();
   }

#else   /* !ATM */
	while (dgetch(&c) == 0) {
	   Bgfunc();
	}
#endif  /* ATM */

	return((int) c);
}

/*--------------------------------------------------------------------
 * getchar_p1()
 *    getchar using polling for RISC processor
 *--------------------------------------------------------------------*/
int getchar_p1()
{
	char		c, c1;
	int		i;
	uInt32		addr, dummy;
extern	char		diag_fbbusarbitration[];

#if  0  /* ATM */

	while (1) {
	   while (dgetch(&c) == 0) {
#ifndef BOOT
	      if (switchtoDMP_DConsole) {
	         switchtoDMP_DConsole = FALSE; c = CTL_B; break;
	      }
	      if (switchtoDMP_VConsole) {
	         switchtoDMP_VConsole = FALSE; c = CTL_D; break;
	      }
#endif /* BOOT */
	      Bgfunc();
	   }
	   if (c != CTL_B && c != CTL_D)
	      break;

	   /* Check for DMP Heartbeat */
	   for (i = 0; i < DMP_HEARTBEAT_TIMEOUT; i++)
	      if (*((volatile uInt32 *) COMM_NMP_SYSTIME) == 
	                                *((volatile uInt32 *) COMM_DMP_SYSTIME))
	         break;

	   if (i == DMP_HEARTBEAT_TIMEOUT) {
	      put_line("\nInvoking DMP Diagnostics ...");
	      *((volatile uInt32 *) COMM_STATUS) = 0;
	      if (c == CTL_D)
	         *((volatile uInt32 *) COMM_STATUS) |= COMM_STATUS_UART_VIRTUAL;

              /* reset TX SAR, RX SAR & BIGA */
	      *(uChar *)CNTL_REG_1 = 0;
	      *(uChar *)CNTL_REG_2 = 0;
	      *(uChar *)CNTL_REG_3 = 0;
	      for (i = 0; i < 1000; i++);

	      *(uChar *)CNTL_REG_1 = SAR_RESET_MASK;
	      *(uChar *)CNTL_REG_2 = SAR_RESET_MASK;
	      *(uChar *)CNTL_REG_3 = BIGA_RESET_MASK;

	      for (i = 0; i < 1000; i++);
	      if (c == CTL_B)
	         c = '\n';
	   }
	   else {
	      if (c == CTL_D) {
	         *((volatile char *) COMM_UART_TXCHAR) = '\n';
	         *((volatile uInt32 *) COMM_STATUS) |=
	                   (COMM_STATUS_UART_VIRTUAL | COMM_STATUS_UART_TXSTAT);
	      }
	      else
	         *((volatile uInt32 *) COMM_STATUS) &= 
	                                              ~COMM_STATUS_UART_VIRTUAL;
	   }

	   if (c == CTL_D) {
	      diag_virtual_uart_mode = TRUE;
	      while (1) {
	         while (dgetch(&c) == 0) {
	            if (*((volatile uInt32 *) COMM_STATUS) & 
	                                              COMM_STATUS_UART_RXSTAT) {
	               c1 = *((volatile char *) COMM_UART_RXCHAR);
	               *((volatile uInt32 *) COMM_STATUS) &= 
	                                               ~COMM_STATUS_UART_RXSTAT;
	               putchar_p(c1);
	            }
	            if (*((volatile uInt32 *) COMM_STATUS) &
	                                          COMM_STATUS_NMP_ARBITRATION) {
	               if (diag_fbbusarbitration[0] == 'T') {
	                  for (addr = FRAMEBUF_START_ARBITRATE;
	                       addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
	                     *((volatile uInt32 *) addr) = addr;
	                     dummy = *((volatile uInt32 *) addr);
	                  }
	               }
	            }
	            if (*((volatile uInt32 *) COMM_STATUS) &
	                                            COMM_STATUS_SWITCH_TO_NMP) {
	               *((volatile uInt32 *) COMM_STATUS) &= 
	                                             ~COMM_STATUS_SWITCH_TO_NMP;
	               c = CTL_N;
	               break;
	            }
	         }
	         if (c == CTL_N) {
	            /*set_cli_screen();*/
	            c = '\n';
	            break;
	         }

	         /* Check for DMP Heartbeat */
	         for (i = 0; i < DMP_HEARTBEAT_TIMEOUT; i++)
	            if (*((volatile uInt32 *) COMM_NMP_SYSTIME) == 
	                                *((volatile uInt32 *) COMM_DMP_SYSTIME))
	               break;

	         if (i == DMP_HEARTBEAT_TIMEOUT) { /* DMP is dead */
	            c = '\n';
	            break;
	         }

	         while (*((volatile uInt32 *) COMM_STATUS) & 
	                                              COMM_STATUS_UART_TXSTAT) {
	            if (*((volatile uInt32 *) COMM_STATUS) & 
	                                              COMM_STATUS_UART_RXSTAT) {
	               c1 = *((volatile char *) COMM_UART_RXCHAR);
	               *((volatile uInt32 *) COMM_STATUS) &= 
	                                               ~COMM_STATUS_UART_RXSTAT;
	               putchar_p(c1);
	            }
	            if (*((volatile uInt32 *) COMM_STATUS) &
	                                          COMM_STATUS_NMP_ARBITRATION) {
	               if (diag_fbbusarbitration[0] == 'T') {
	                  for (addr = FRAMEBUF_START_ARBITRATE;
	                       addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
	                     *((volatile uInt32 *) addr) = addr;
	                     dummy = *((volatile uInt32 *) addr);
	                  }
	               }
	            }
	            /* Check for DMP Heartbeat */
	            for (i = 0; i < DMP_HEARTBEAT_TIMEOUT; i++)
	               if (*((volatile uInt32 *) COMM_NMP_SYSTIME) == 
	                                *((volatile uInt32 *) COMM_DMP_SYSTIME))
	                  break;

	            if (i == DMP_HEARTBEAT_TIMEOUT) { /* DMP is dead */
	               c = '\n';
	               break;
	            }
	         }

	         if (i == DMP_HEARTBEAT_TIMEOUT) { /* DMP is dead */
	            break;
	         }

	         *((volatile char *) COMM_UART_TXCHAR) = c;
	         *((volatile uInt32 *) COMM_STATUS) |= COMM_STATUS_UART_TXSTAT;
	      }
	      diag_virtual_uart_mode = FALSE;
	      *((volatile uInt32 *)COMM_STATUS) &= ~COMM_STATUS_NMP_ARBITRATION;
	   }
	   if (c == '\n')
	      break;
	}

	if (c == CTL_U)
	   sys_clock_update = ~sys_clock_update & 0x1;

#endif  /*  ATM */
	return((int) c);

}

/*--------------------------------------------------------------------
 * dgetch()
 *--------------------------------------------------------------------*/
int dgetch(
	char		*buf)
{
#ifndef SCC_NEW
	return(dgetbuf(buf, 1));
#else /* SCC_NEW */
	return(new_scc_input(0, buf, 1));
#endif /* SCC_NEW */
}

/*--------------------------------------------------------------------
 * dgetbuf()
 *--------------------------------------------------------------------*/
#ifndef SCC_NEW
int dgetbuf(
	char		*buf,
	int		cnt)
{
	extern struct sccqueue conqueue;
	struct buf	*m;
	register int	actual;
	register int	s;

	actual = 0; s = spl7();

	SCC_DEQUEUE(&conqueue, m);
	splx(s);

#if 0  /* rlb */
	if (m != 0) {
	   actual = MIN(m->b_len, cnt);
	   memcpy(buf, &m->data, actual);
	   b_free(m);
	}

#endif  /* 0 rlb */
	return (actual);
}
#endif /* SCC_NEW */

/*--------------------------------------------------------------------
 * valid_date()
 *--------------------------------------------------------------------*/
int valid_date(
	char		*date)
{
	uInt32		year, month, day;
	char		y[3], m[3], d[3];

	get_clock(&sys_clock);

	if (strlen(date) != 8)
	   return(FALSE);

	if (*(date+2) != '/' || *(date+5) != '/')
	   return(FALSE);

	y[0] = *date++; y[1] = *date++; y[2] = '\0'; date++;
	m[0] = *date++; m[1] = *date++; m[2] = '\0'; date++;
	d[0] = *date++; d[1] = *date++; d[2] = '\0';

	if (!sscanf_x(y, fmt_uInt08, &year)  ||
	    !sscanf_x(m, fmt_uInt08, &month) ||
	    !sscanf_x(d, fmt_uInt08, &day))
	   return(FALSE);

	if (year < 93 || year > 99 || month < 1 || month > 12)
	   return(FALSE);
	if (day < 1 || day > 31 || 
	   ((month==4 || month==6 || month==9 || month==11) && day == 31) ||
	   (month == 2 && day > 29) ||
	   (month == 2 && day == 29 && year != 96))
	   return(FALSE);

	sys_clock.year = (uChar) year; 
	sys_clock.month = (uChar) month; 
	sys_clock.date = (uChar) day;

#if  0   /* Adapted to ALC */
	*((volatile uInt32 *) COMM_YEAR)   = year;
	*((volatile uInt32 *) COMM_MONTH)  = month;
	*((volatile uInt32 *) COMM_DAY)    = day;
#endif  /* ATM */

	set_clock(&sys_clock);

	sprintf(sys_date, "%02u/%02u/%02u", year, month, day);
	if (sys_clock_display == TRUE)
	   display_cli_heading();

	return(TRUE);
}

/*--------------------------------------------------------------------
 * valid_time()
 *--------------------------------------------------------------------*/
int valid_time(
	char		*time)
{
	uInt32		hour, minute, second;
	char		h[3], m[3], s[3];

	get_clock(&sys_clock);

	if (strlen(time) != 8)
	   return(FALSE);

	if (*(time+2) != ':' || *(time+5) != ':')
	   return(FALSE);

	h[0] = *time++; h[1] = *time++; h[2] = '\0'; time++;
	m[0] = *time++; m[1] = *time++; m[2] = '\0'; time++;
	s[0] = *time++; s[1] = *time++; s[2] = '\0';

	if (!sscanf_x(h, fmt_uInt08, &hour)  ||
	    !sscanf_x(m, fmt_uInt08, &minute) ||
	    !sscanf_x(s, fmt_uInt08, &second))
	   return(FALSE);

	if (hour > 23 || minute > 59 || second > 59)
	   return(FALSE);

	sys_clock.hour = (uChar) hour; 
	sys_clock.minute = (uChar) minute;
	sys_clock.second = (uChar) second;

	set_clock(&sys_clock);

	system_time = hour*3600000 +
	              minute*60000 +
	              second*1000;

	return(TRUE);
}

#ifndef BOOT
int valid_source_mac(
	char		*mac)
{
	uInt32		m[6];
	uChar		base[6];
	char		m1[6][3];
	int		i;

	if (strlen(mac) != 17)
	   return(FALSE);

	if (*(mac+2) != ':' || *(mac+5) != ':' || *(mac+8) != ':' ||
	    *(mac+11) != ':' || *(mac+14) != ':')
	   return(FALSE);

	for (i = 0; i < 6; i++) {
	   m1[i][0] = *mac++; m1[i][1] = *mac++; m1[i][2] = '\0'; mac++;
	   if (!sscanf_x(m1[i], fmt_xInt08, &m[i]))
	      return(FALSE);
	}

	for (i = 0; i < 6; i++)
	   base[i] = (uChar) m[i];

	set_source_mac(base);

	return(TRUE);
}

#if  0  /* ATM */
/*--------------------------------------------------------------------
 * valid_dest_mac()
 *--------------------------------------------------------------------*/
int valid_dest_mac(
	char		*mac)
{
	uInt32		m[6];
	uChar		base[6];
	char		m1[6][3];
	int		i;

	if (strlen(mac) != 17)
	   return(FALSE);

	if (*(mac+2) != ':' || *(mac+5) != ':' || *(mac+8) != ':' ||
	    *(mac+11) != ':' || *(mac+14) != ':')
	   return(FALSE);

	for (i = 0; i < 6; i++) {
	   m1[i][0] = *mac++; m1[i][1] = *mac++; m1[i][2] = '\0'; mac++;
	   if (!sscanf_x(m1[i], fmt_xInt08, &m[i]))
	      return(FALSE);
	}

	for (i = 0; i < 6; i++)
	   base[i] = (uChar) m[i];

	set_dest_mac(base);

	return(TRUE);
}
#endif  /* ATM */

#endif /* BOOT */

/*--------------------------------------------------------------------
 * Bgfunc()
 *--------------------------------------------------------------------*/
int Bgfunc()
{
	uInt32		hour, minute, second;
	uInt32		year, month, day;

#ifndef  ATM

	if ((system_time - curr_system_time) > 1000) {
	   curr_system_time = system_time;
	   hour = curr_system_time/3600000;
	   curr_system_time -= (hour*3600000);
	   minute = curr_system_time/60000;
	   curr_system_time -= (minute*60000);
	   second = curr_system_time/1000;
	   curr_system_time = system_time;
	   if (hour >= 24) {
	      get_clock(&sys_clock);

	      year = sys_clock.year;
	      month = sys_clock.month;
	      day = sys_clock.date;
	      hour = sys_clock.hour;
	      minute = sys_clock.minute;
	      second = sys_clock.second;
	      system_time = curr_system_time = hour*3600000 +
	                                       minute*60000 +
	                                       second*1000;
	      *((volatile uInt32 *) COMM_YEAR)   = year;
	      *((volatile uInt32 *) COMM_MONTH)  = month;
	      *((volatile uInt32 *) COMM_DAY)    = day;
	      if (sys_clock_update == TRUE)
	         *((uInt32 *) COMM_NMP_SYSTIME) = system_time;
	      sprintf(sys_date, "%02u/%02u/%02u", year, month, day);
	      if (sys_clock_display == TRUE)
	         display_cli_heading();
	   }
	   sprintf(sys_time, "%02u:%02u:%02u", hour, minute, second);

	   if (sys_clock_display == TRUE)
	      display_cli_time();
	}
#endif  /* ATM */

}

