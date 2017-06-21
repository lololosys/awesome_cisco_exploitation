/* $Id: appn_sysutils.c,v 3.6.10.1 1996/03/18 18:49:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/appn/appn_sysutils.c,v $
 *------------------------------------------------------------------
 * Sytem Utilities required in APPN
 *
 * May 1995, Leo Pereira
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appn_sysutils.c,v $
 * Revision 3.6.10.1  1996/03/18  18:49:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/16  06:24:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.2  1996/03/07  08:27:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:15:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/27  10:51:10  dcalia
 * APPN/RSRB needs a configurable bridge number (CSCdi42257)
 *
 * Revision 3.5  1996/02/13  20:49:18  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.4  1996/01/17  11:53:56  dcalia
 * APPN isr performance not adequate (CSCdi32469)
 *
 * Revision 3.3  1995/11/21  07:26:02  lpereira
 * CSCdi44255:  APPN build is broken with the PortReady changes
 * Added function appn_get_timestamp32().
 *
 * Revision 3.2  1995/11/17  08:37:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/08/12  11:47:28  dcalia
 * Rate limit APPN alerts. Provide API to CLSReturnCodeToString and
 * string_getnext (CSCdi28882)
 *
 * Revision 2.11  1995/08/11  02:22:35  dmccowan
 * CSCdi30561:  need to support APING on our router
 *
 * Revision 2.10  1995/08/02  00:36:35  vtseng
 * CSCdi29693:  no appn activate should not stop appn
 *
 * Revision 2.9  1995/07/27  22:07:48  mregan
 * CSCdi36463:  redo appn_get_current_time: APPN images are broken
 * add timezone support for APPN
 *
 * Revision 2.8  1995/07/26  21:52:18  mregan
 * CSCdi33023:  appn show command CPUHOG message
 * fix CPUMOG messages on large show outputs
 *
 * Revision 2.7  1995/07/17  21:30:34  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Another missed directory.  :-(
 *
 * Revision 2.6  1995/07/15  02:36:24  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.5  1995/06/27  20:38:44  mregan
 * CSCdi36463:  redo appn_get_current_time: APPN images are broken
 * do the appn_get_current_time() right this time.
 *
 * Revision 2.4  1995/06/26  21:51:44  mregan
 * CSCdi33791:  APPN MS TP can starve other processes when activated
 * implement get_current_time in APPN code.
 *
 * Revision 2.3  1995/06/19  18:40:14  pleung
 * CSCdi36057:  system make broke
 *
 * replacing GET_SECONDS() with system_uptime_seconds()
 *
 * Revision 2.2  1995/06/09  12:58:10  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:03:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "ieee.h"
#include "ttysrv.h"
#include "address.h"            /* ADDR_IP                                   */
#include "file.h"               /* filetype                                  */
#include "sys_registry.h"       /* reg_invoke_file_{read,write}              */
#include "../os/clock.h"
#include "../ibm/ibm_types.h"
#include "../cls/clsi_msg.h"
#include "appn_sysutils.h"
#include "interface_private.h"
#include "../srt/rsrb.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"

long atol (char *num_string) 
{
   boolean ok;

   return(parse_decimal_long(num_string,&ok));
}

ulong appn_get_seconds (void)
{
    return(system_uptime_seconds());
}

void appn_edisms (void *func, ulong parm)
{
   edisms((blockproc *)func, parm);
}

void appn_tdisms (ulong deltat)
{
   process_sleep_for(deltat);
}

void appn_s_killp (int pid)
{
   process_kill(pid);
}


int appn_cfork(void *padd, long pp, int stacksize, char *name, int ttynum)
{
   return(cfork((forkproc *)padd, pp, stacksize, name, ttynum));
}

/******************************************************************************
**
**   FUNCTION NAME:  disable
**
**   FUNCTION:  This function performs the logical equivalent of disabling
**              the system from accepting interrupts.  If this is a 
**              multi-processor system, that may entail spinning on a 
**              lock field, besides physically disabling.
**
**   INPUT:     None
**
**   OUTPUT:    None
**
******************************************************************************/
static leveltype appn_interrupt_status;
 
void c_disable(void)
{
   appn_interrupt_status = raise_interrupt_level(ALL_DISABLE);
}
 
/******************************************************************************
**
**   FUNCTION NAME:  c_enable
**
**   FUNCTION:  This function re-enables for interrupts.
**
**   INPUT:     None
**
**   OUTPUT:    None
**
******************************************************************************/
 
void c_enable(void)
{
   reset_interrupt_level(appn_interrupt_status);
}

/*
 * Memory allocation stuff
 */
void *appn_malloc(uint size)
{
   return(malloc(size));
}

void appn_free(void *addr)
{
   free(addr);
}

boolean appn_yes_or_no(char *prompt, boolean def, boolean usedef)
{
    return(yes_or_no(prompt, def, usedef));
}

/* 
 * Manipulating ieee 48 bit addresses
 */
void appn_ieee_copy (uchar const *src, uchar *dst)
{
   ieee_copy(src, dst);
}

void appn_ieee_swap (uchar const *src, uchar *dst)
{
   ieee_swap(src, dst);
}


/*
 * Timestamp stuff
 */
ulong appn_get_timestamp32(void);
sys_timestamp appn_get_timestamp(void);
ulong appn_elapsed_time(sys_timestamp timestamp);
ulong appn_get_seconds(void);

/*
 * printing and debugging
 */
int appn_printf(const char *fmt, ...)
{
   va_list ap;
   int rc;

   va_start(ap, fmt);
   rc = _doprnt(NULL, stdio, fmt, ap, FALSE);
   va_end(ap);
   return(rc);
}

void appn_buginf(const char *fmt, ...)
{
   va_list ap;
   int len;
   char fmtarea[FMTAREA_SIZE];

   va_start(ap, fmt);
   len = _doprnt(fmtarea, stdio, fmt, ap, FALSE);
   fmtarea[len] = '\0';
   if (len > FMTAREA_SIZE) {
      buginf("appn_buginf: message length too long.");
   }
   buginf("%s", fmtarea);
   va_end(ap);
}

boolean appn_breakpoint(void)
{
   return(breakpoint());
}


sys_timestamp appn_get_timestamp(void)
{
   sys_timestamp time;

   GET_TIMESTAMP(time);
   return(time);
}

ulong appn_get_timestamp32(void)
{
   ulong time;

   GET_TIMESTAMP32(time);
   return(time);
}

void appn_get_secs_and_msecs(word *secs, word *msecs)
{
   clock_epoch curtime;

   *secs = system_uptime_seconds();
   clock_get_time(&curtime);
   *msecs = (curtime.epoch_frac / (MAXUINT / 1000));
}

ulong appn_elapsed_time(sys_timestamp timestamp)
{
   return(ELAPSED_TIME(timestamp));
}

void appn_get_current_time(appn_timeval *atv)
{
   clock_epoch ce;
   timeval     tv;

   clock_get_time(&ce);
   clock_epoch_to_timeval(&ce, &tv, 0); /* Calendar runs on UTC */
   
   if (clock_is_probably_valid()) {
      atv->year = tv.year;
      atv->month = tv.month;
      atv->day = tv.day;
      atv->hour = tv.hour;
      atv->minute = tv.minute;
      atv->second = tv.second;
   } else {
      memset(atv,'\0',sizeof(appn_timeval));
   }
}


int appn_tftp_file_write (char *filename, ipaddrtype tftp_host, char *bfr_p, int bfr_len)
{
    filetype   file;
    
    file.proto = FILE_ACCESS_TFTP;
    file.filename = filename;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.ps.tuba.boothost.ip_addr = tftp_host;
    
    if ((reg_invoke_file_write(file.proto, &file, bfr_p, bfr_len)) == ERROR_OK)
	return (TFTP_OK);
    else
	return (TFTP_ERR);    
}


int appn_tftp_file_read (char *filename, ipaddrtype tftp_host, char *bfr_p, ulong bfr_len)
{
    filetype   file;

    file.proto = FILE_ACCESS_TFTP;
    file.filename = filename;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.ps.tuba.boothost.ip_addr = tftp_host;

    if ((reg_invoke_file_read(file.proto, &file, bfr_p, &bfr_len, FALSE, NULL, NULL)) == ERROR_OK)
	return (TFTP_OK);
    else
	return (TFTP_ERR);
    
}

boolean appn_automore_quit (void)
{
   return(automore_quit());
}

int appn_clock_timezone_offset (void)
{
   return(clock_timezone_offset());
}

void appn_flush (tt_soc *tty)
{
   ttyflush(tty);
}

char *appn_CLSReturnCodeToString(hword RetCode)
{
  return(CLSReturnCodeToString(RetCode));
}

char *appn_string_getnext(void)
{
  return(string_getnext());
}
/*
 *
 */
tr_vgrouptype *appn_vrfind_ringgroup(ushort vrn)
{
  return(vrfind_ringgroup(vrn));;
}

/*
 *  Send APPN Alert to Host via SSCP-PU session
 *
 *  Input: buffer is formatted as Major Vector  (see struct MV )
 */

void appn_send_alert_to_host(uchar *buffer)
{
    reg_invoke_MV_SendAlert((MV *)buffer);
}
