/* $Id: dspu_notification.h,v 3.2.60.1 1996/04/03 00:11:52 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_notification.h,v $
 *------------------------------------------------------------------
 * DSPU Notifications Definitions and Declarations
 *
 * September 1995, Bob Batz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_notification.h,v $
 * Revision 3.2.60.1  1996/04/03  00:11:52  jjohnson
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Implement a different mechanism for creating trap varbind lists
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:04:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  15:01:11  rbatz
 * Placeholder file for Ark_Ibm_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef DSPU_TRAP_ALERT_H
#define DSPU_TRAP_ALERT_H

#include "dtypes.h"
#include "dspu_pu.h"
#include "dspu_lu.h"
#include "../snmp/snmp_api.h"

#define DSPU_NOTIFY_LEVEL_OFF 0
#define DSPU_NOTIFY_LEVEL_LOW 1
#define DSPU_NOTIFY_LEVEL_MEDIUM 2
#define DSPU_NOTIFY_LEVEL_HI 3
#define DSPU_NOTIFY_LEVEL_DEFAULT 1

typedef enum {
    PU_STATECHANGE,
    PU_ACTFAIL,
    LU_STATECHANGE,
    LU_ACTFAIL
} DspuNotification;

struct dspuTrap {
    DspuNotification trapType;   
    int puIdx;
    int luIdx;
    char puName[MAX_ENTRYNAME_LN];
    int stateOrReason;
    LinkType linkType;
};

/*=====================================================================*
* SetDspuNotificationLevel()
*======================================================================*
*
* Description:
*   Set the Notification level for DSPU.
**/
void SetDspuNotificationLevel(int);

/*=====================================================================*
* GetDspuNotificationLevel
*======================================================================*
*
* Description:
*   Get the Notification level for DSPU.
**/
int GetDspuNotificationLevel(void);

/*=====================================================================*
* DspuPermitTimeout()
*======================================================================*
*
* Description:
*   This function is called when the Notification Permit Timer pops.
**/
void DspuPermitTimeout(void *);

/*=====================================================================*
* DspuStartPermitTimer()
*======================================================================*
*
* Description:
*   This function starts the Notification Permit Timer.
**/
void DspuStartPermitTimer(void);

/*=====================================================================*
* initDspuNotificationQ()
*======================================================================*
*
* Description:
*   Initialize the data structures for the Notification Queue.
**/
void initDspuNotificationQ(void);

/*=====================================================================*
* ProcessDspuNotificationQ()
*======================================================================*
*
* Description:
*   Return TRUE if the Notification Queue is Empty.
**/
void ProcessDspuNotificationQ(void);

/*=====================================================================*
* SendDspuNotification()
*======================================================================*
*
* Description:
*   Send a queued notification as an SNMP Traps and/or an SNA Message
*   to Operator.
**/
void SendDspuNotification(struct dspuTrap *);

/*=====================================================================*
* DspuNetMgmtNotification
*======================================================================*
*
* Description:
*   Queue a notification into the DSPU Array of Traps.
*   Used for SNA Messages to Operator as well.
**/
void DspuNetMgmtNotification(void *, DspuNotification, int, LinkType);

/*=====================================================================*
* PuStateChangeTrap()
*======================================================================*
*
* Description:
*   Generates SNMP trap when dspuPuOperState changes.
*
**/
void PuStateChangeTrap(struct dspuTrap *);

/*=====================================================================*
* PuStateChangeMsgToOper()
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator when dspuPuOperState changes.
*
**/
void PuStateChangeMsgToOper(struct dspuTrap *);

/*=====================================================================*
* PuActFailureTrap()
*======================================================================*
*
* Description:
*   Generates SNMP trap on PU activation failure
*
**/
void PuActFailureTrap(struct dspuTrap *);

/*=====================================================================*
* PuActFailureMsgToOper()
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator on Downstream PU activation failure.
*
**/
void PuActFailureMsgToOper(struct dspuTrap *);

/*=====================================================================*
* LuStateChangeTrap()
*======================================================================*
*
* Description:
*   Generates SNMP trap when LuOperState changes
*
**/
void LuStateChangeTrap (struct dspuTrap *);

/*=====================================================================*
* LuActFailureTrap()
*======================================================================*
*
* Description:
*   Generates SNMP trap on LU activation failure
*
**/
void LuActFailureTrap(struct dspuTrap *);

/*=====================================================================*
* LuActFailureMsgToOper()
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator on Downstream LU activation failure.
*
**/
void LuActFailureMsgToOper(struct dspuTrap *);

#endif

      
