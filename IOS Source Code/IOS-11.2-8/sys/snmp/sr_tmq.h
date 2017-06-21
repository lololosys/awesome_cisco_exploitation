/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_tmq.h,v 3.4 1996/01/18 16:38:01 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_tmq.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_tmq.h,v $
 * Revision 3.4  1996/01/18  16:38:01  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * move the rs232 mib, image mib, and terminal services mib into separate
 * subsystems.  move the cisco interfaces mib into the interfaces mib
 * subsystem.  consolidate the snmp timer subsystem into the snmp
 * subsystem since it really is core snmp technology.
 *
 * Revision 3.3  1995/11/17  19:01:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:35  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/03  17:32:41  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _TMQ_H_
#define _TMQ_H_


typedef struct _TimeOutDescriptor TimeOutDescriptor;
struct _TimeOutDescriptor {
    long            TimeOutType;
    long            TimerId;
    void            (*CallBack) (TimeOutDescriptor *tdp);
    long            UserData1;
    void           *UserData2;
};

#define CurTimeOfDay snmp_sysUpTime
#define TimerCurTime snmp_sysUpTime
static inline boolean CanSetTimeout(void) { return TRUE; };

int SetPeriodicTimeout (unsigned long when, unsigned long period,
			TimeOutDescriptor *info);
int SetOneshotTimeout (unsigned long when, TimeOutDescriptor *info);
int CancelTimeout (int id);
int TimerFreeInfo (long id, void (*tifree) (void *p));

void init_snmp_timers (void);

#endif				/* _TMQ_H_ */
