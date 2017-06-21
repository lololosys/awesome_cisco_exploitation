/* $Id: dspu_notification.c,v 3.2.58.2 1996/05/17 10:47:50 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_notification.c,v $
 *------------------------------------------------------------------
 * DSPU Notification support.
 *
 * September 1995, Bob Batz
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_notification.c,v $
 * Revision 3.2.58.2  1996/05/17  10:47:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.46.2  1996/04/22  03:48:26  lcox
 * CSCdi55251:  change DspuMalloc(of Pu)/malloc to malloc_named call
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.46.1  1996/04/03  14:02:46  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.58.1  1996/03/18  19:33:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/09  05:04:39  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.24.1  1996/02/20  13:47:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:04:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:56:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  14:26:04  rbatz
 * Placeholder for Ark_Ibm_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "dtypes.h"
#include "dspu_notification.h"
#include "dspu_pu.h"
#include "dspu_lu.h"
#include "dspu_fp.h"
#include "dspu.h"
#include "dspu_puutil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "mv_sr.h"
#include "../h/nv_msgid.h"
#include "dspu_itimer.h"

/*  DSPU can generate a burst of traps due to:
 *
 *  1) PU and LU operational state changes (especially LU).
 *  2) LU Activation Failures due to misconfiguration.
 *
 *  The usual way to send traps is to invoke snmp_trap which will
 *  do an SNMP Get of the VarBind parameters and will send the
 *  trap.  For each SNMP management station defined, there is a queue 
 *  which can hold 10 traps and which sends 1 trap every 1/4 second.
 *  DSPU can easily overflow this queue, causing the loss of our own
 *  traps and, worse yet, other traps.
 *
 *  So we have created two queues within DSPU to handle this.
 *  The first queue is a statically-allocated array of size MAX_DSPU_TRAPS.  
 *  All traps except LU State Changes go in here.
 *
 *  The second queue is a dynamically-allocated array of size 
 *  MAX_LU_STATE_TRAPS which is allocated when the notification level
 *  is set to HI and freed when the notification level is set to
 *  OFF, LOW, or MEDIUM.  This queue holds the LU State Change Traps
 *  only.
 *
 *  Traps can still get lost (no matter how big the queue); however, 
 *  DSPU sends MAX_DSPU_PERMIT traps every second to prevent the
 *  SNMP queues from being overflowed by DSPU traps.
 */

#define MAX_DSPU_PERMIT 3
#define MAX_DSPU_TRAPS 24
#define MAX_LU_STATE_TRAPS 96

static int readTrap;
static int writeTrap;
static int notificationPermits;
static struct dspuTrap dspuTrapArray[MAX_DSPU_TRAPS];
static unsigned char dspuNotifyLevel = DSPU_NOTIFY_LEVEL_DEFAULT;
static Itimer dspuPermitTimer;
static void * dumHandle;

static int readLuStateTrap;
static int writeLuStateTrap;
static struct dspuTrap * luStateTrapArray = NULL;

static uchar puMgtBuff[80];
static struct mto_name_subvect cpNameVect, puNameVect;
static struct mto_selfDefText_subvect nauSv1, nauSv2, nauSv3;


/*=====================================================================*
* SetDspuNotificationLevel
*======================================================================*
*
* Description:
*   Set the Notification level for DSPU.
**/
void SetDspuNotificationLevel (int level)
{
    dspuNotifyLevel = level;

    if (level == DSPU_NOTIFY_LEVEL_HI) {
        if (luStateTrapArray == NULL) {
            luStateTrapArray = (struct dspuTrap *) 
                  malloc_named(
                    sizeof(struct dspuTrap) * MAX_LU_STATE_TRAPS,
                    "DSPU TA");
        }
    }
    else {  /* Lu State Change Traps not allowed */
        readLuStateTrap = writeLuStateTrap = 0;
        if (luStateTrapArray) {
            free(luStateTrapArray);
            luStateTrapArray = NULL;
        }
    }
    return;
}

/*=====================================================================*
* GetDspuNotificationLevel
*======================================================================*
*
* Description:
*   Get the Notification level for DSPU.
**/
int GetDspuNotificationLevel ()
{
    return dspuNotifyLevel;
}


/*=====================================================================*
* DspuPermitTimeout
*======================================================================*
*
* Description:
*   This function is called when the Notification Permit Timer pops.
**/
void DspuPermitTimeout (void * dummy)
{
    /* Restore the permit */
    notificationPermits = MAX_DSPU_PERMIT;

    /* Set the scheduler flag if work to do */
    if ( (readTrap != writeTrap) || (readLuStateTrap != writeLuStateTrap) )
        FpHasDataToSend();
}

/*=====================================================================*
* initDspuNotificationQ
*======================================================================*
*
* Description:
*   Initialize the data structures for the Notification Queue.
**/
void initDspuNotificationQ ()
{
    readTrap = 0;
    readLuStateTrap = 0;
    writeTrap = 0;
    writeLuStateTrap = 0;
    notificationPermits = MAX_DSPU_PERMIT;
    InitialiseItimer(&dspuPermitTimer, FALSE);
    return;
}
    
/*=====================================================================*
* DspuStartPermitTimer
*======================================================================*
*
* Description:
*   This function starts the Notification Permit Timer.
**/
void DspuStartPermitTimer ()
{
    StartItimer(&dspuPermitTimer, 1*SECONDS, DspuPermitTimeout, dumHandle);
}

/*=====================================================================*
* ProcessDspuNotificationQ
*======================================================================*
*
* Description:
*   Return TRUE if the Notification Queue is Empty.
**/
void ProcessDspuNotificationQ ()
{
    boolean startTimerFlag = FALSE;

    /* Can I send anything? */
    if (!notificationPermits) return;

    /* Send one from the regular notification queue */
    if (readTrap != writeTrap) {
        SendDspuNotification(&dspuTrapArray[readTrap]);
        readTrap = (++readTrap) % MAX_DSPU_TRAPS;
        if (notificationPermits == MAX_DSPU_PERMIT)
            startTimerFlag = TRUE;
        --notificationPermits;
    }

    /* Send from the LU State Change notification queue */
    if (notificationPermits && (readLuStateTrap != writeLuStateTrap)) {
        if (luStateTrapArray)
            SendDspuNotification(&luStateTrapArray[readLuStateTrap]);
        readLuStateTrap = (++readLuStateTrap) % MAX_LU_STATE_TRAPS;
        if (notificationPermits == MAX_DSPU_PERMIT)
            startTimerFlag = TRUE;
        --notificationPermits;
    }
   
    if (startTimerFlag)
        DspuStartPermitTimer();

    if ( notificationPermits &&
       ((readTrap != writeTrap) || (readLuStateTrap != writeLuStateTrap)) ) {
        FpHasDataToSend();
    }
}
  
/*=====================================================================*
* SendDspuNotification
*======================================================================*
*
* Description:
*   Send a queued notification as an SNMP Traps and/or an SNA Message
*   to Operator.
**/
void SendDspuNotification (struct dspuTrap * pTrap)
{
    /* Send the SNMP Trap */
    reg_invoke_generateDSPUtrap(pTrap);

    /* Send the Message To Operator if desired */
    switch (pTrap->trapType) {
        case PU_STATECHANGE:
            if (pTrap->linkType == Downstream)
                PuStateChangeMsgToOper(pTrap);
            break;
        case PU_ACTFAIL:
            PuActFailureMsgToOper(pTrap);
            break;
        case LU_ACTFAIL:
            LuActFailureMsgToOper(pTrap);
            break;
        default:
            break;
    }
}

/*=====================================================================*
* DspuNetMgmtNotification
*======================================================================*
*
* Description:
*   Queue a notification into the DSPU Array of Traps.
*   Used for SNA Messages to Operator as well.
**/
void DspuNetMgmtNotification (void * puluP, 
                              DspuNotification trapType,
                              int stateOrRsn,
                              LinkType linkType)
{ 
    struct Pu *puP;
    struct Lu *luP;
    struct dspuTrap *pTrap;

    /* Is the queue full? */
    if (trapType == LU_STATECHANGE) {
        if ( (writeLuStateTrap == (readLuStateTrap - 1)) || 
            ((writeLuStateTrap == MAX_LU_STATE_TRAPS-1) && !readLuStateTrap) ||
             !luStateTrapArray ) {
              return;
        }
        pTrap = &luStateTrapArray[writeLuStateTrap];
    }
    else {
        if ( (writeTrap == (readTrap - 1)) || 
            ((writeTrap == MAX_DSPU_TRAPS-1) && !readTrap) ) {
              return;
        }
        pTrap = &dspuTrapArray[writeTrap];
    }

    /* Queue it or Drop it? */
    switch (trapType) {
        case PU_STATECHANGE:
            if (dspuNotifyLevel < DSPU_NOTIFY_LEVEL_MEDIUM)  
                return;
            puP = (struct Pu *) puluP;
            pTrap->puIdx = (int) GetPuDspuMibPuIdx(puP);
            bcopy(GetPuName(puP), pTrap->puName, MAX_ENTRYNAME_LN);
            break;

        case LU_STATECHANGE:
            if (dspuNotifyLevel < DSPU_NOTIFY_LEVEL_HI)  
                return;
            luP = (struct Lu *) puluP;
            if (linkType == Upstream) {
                pTrap->puIdx = (int) GetPuDspuMibPuIdx(luP->upPuP);
                pTrap->luIdx = (int) luP->upLuNumber;
                bcopy(GetPuName(luP->upPuP), pTrap->puName, MAX_ENTRYNAME_LN);
            }
            else {
                pTrap->puIdx = (int) GetPuDspuMibPuIdx(luP->downPuP);
                pTrap->luIdx = (int) luP->downLuNumber;
                bcopy(GetPuName(luP->downPuP), pTrap->puName, MAX_ENTRYNAME_LN);
            }
            break;

        case PU_ACTFAIL:
            if (dspuNotifyLevel < DSPU_NOTIFY_LEVEL_LOW) 
                return; 
            puP = (struct Pu *) puluP;
            pTrap->puIdx = (int) GetPuDspuMibPuIdx(puP);
            bcopy(GetPuName(puP), pTrap->puName, MAX_ENTRYNAME_LN);
            IncrPuActFailCnt(puP);
            SetPuLastActFailRsn(puP, stateOrRsn);
            break;

        case LU_ACTFAIL:
            if (dspuNotifyLevel < DSPU_NOTIFY_LEVEL_LOW)  
                return;
            luP = (struct Lu *) puluP;
            pTrap->puIdx = (int) GetPuDspuMibPuIdx(luP->downPuP);
            pTrap->luIdx = (int) luP->downLuNumber;
            bcopy(GetPuName(luP->downPuP), pTrap->puName, MAX_ENTRYNAME_LN);
            SetLuLastActFailRsn(luP, stateOrRsn);
            break;
        default:
            break;
    }

    pTrap->stateOrReason = (int) stateOrRsn;
    pTrap->trapType = trapType;
    pTrap->linkType = linkType;

    if (trapType == LU_STATECHANGE) 
        writeLuStateTrap = (++writeLuStateTrap) % MAX_LU_STATE_TRAPS;
    else
        writeTrap = (++writeTrap) % MAX_DSPU_TRAPS;

    FpHasDataToSend();
}

/*=====================================================================*
* PuStateChangeMsgToOper
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator when dspuPuOperState changes.
*
**/
void PuStateChangeMsgToOper (struct dspuTrap * pTrap)
{
  uchar* cpName = 0;

  /* Fill in the Resource Hierarchy Names */
  mto_fill_nameVector(&puNameVect, pTrap->puName);
  puNameVect.next = &cpNameVect;

  /* Now do the cpName */
  if (strcmp(hostname, "Router") == 0)
    cpName = (uchar *) GetFocalPuName();
  if (cpName == 0)
    cpName = hostname;
  mto_fill_nameVector(&cpNameVect, cpName);

  /* Now fill in the Self Defined Text */
  if (pTrap->linkType == Downstream) {
      if (pTrap->stateOrReason) {
          sprintf(puMgtBuff, "%s %s DOWNSTREAM PU %s ACTIVE", 
                  NVMSG_DspuPuDownActiv, cpName, pTrap->puName);
      }
      else {
          sprintf(puMgtBuff, "%s %s DOWNSTREAM PU %s INACTIVE", 
                  NVMSG_DspuPuDownInact, cpName, pTrap->puName);
      }
  }
  else
      return;

  /* Format the text into a self-defined Text Vector */
  mto_fill_selfDefTextVector(&nauSv1, puMgtBuff);

  MV_MessageToOperator (NV_OPER,       /* Application Name */
                        &puNameVect,   /* Name Sub Vector */
                        &nauSv1);       /* Text Sub Vector */

}  /* End PuStateChange_MsgToOper */

/*=====================================================================*
* PuActFailureMsgToOper
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator on Downstream PU activation failure.
*
**/
void PuActFailureMsgToOper (struct dspuTrap * pTrap)
{
  uchar* cpName = 0;

  /* Fill in the Resource Hierarchy Names */
  mto_fill_nameVector(&puNameVect, pTrap->puName);
  puNameVect.next = &cpNameVect;

  /* Now do the cpName */
  if (strcmp(hostname, "Router") == 0)
    cpName = (uchar *) GetFocalPuName();
  if (cpName == 0)
    cpName = hostname;
  mto_fill_nameVector(&cpNameVect, cpName);

  /* Now fill in the Self Defined Text */ 
  sprintf(puMgtBuff, "%s DOWNSTREAM PU FAILED ACTIVATION", 
          NVMSG_DspuPuFailAct1);
  mto_fill_selfDefTextVector(&nauSv1, puMgtBuff); 
  nauSv1.next = &nauSv2;

  sprintf(puMgtBuff, "%s %s DOWNSTREAM PU %s", 
                      NVMSG_DspuPuFailAct2, hostname, pTrap->puName);
  mto_fill_selfDefTextVector(&nauSv2, puMgtBuff); 
  nauSv2.next = &nauSv3;

  switch (pTrap->stateOrReason) {
      case PuActFailRsn_InternalError:
          sprintf(puMgtBuff, "%s INTERNAL ERROR", 
                  NVMSG_DspuPuFailActRsn2); 
          break;
      case PuActFailRsn_ConfigError:
          sprintf(puMgtBuff, "%s CONFIGURATION ERROR", 
                  NVMSG_DspuPuFailActRsn3); 
          break;
      case PuActFailRsn_NegativeRsp:
          sprintf(puMgtBuff, "%s NEGATIVE RESPONSE", 
                  NVMSG_DspuPuFailActRsn4); 
          break;
      case PuActFailRsn_AlreadyActive:
          sprintf(puMgtBuff, "%s ALREADY ACTIVE", 
                  NVMSG_DspuPuFailActRsn5); 
          break;
      case PuActFailRsn_None: 
      case PuActFailRsn_Other:
      default:
          sprintf(puMgtBuff, "%s UNKNOWN OR UNDEFINED", 
                  NVMSG_DspuPuFailActRsn1); 
          break;
  }

  mto_fill_selfDefTextVector(&nauSv3, puMgtBuff); 

  MV_MessageToOperator(NV_OPER,       /* Operator Name   */
                       &puNameVect,   /* Name Sub Vector */
                       &nauSv1);      /* Text Sub Vector */

}  /* End PuActFailure_MsgToOper */


/*=====================================================================*
* LuActFailureMsgToOper
*======================================================================*
*
* Description:
*   Generates SNA Message To Operator on Downstream LU activation failure.
*
**/
void LuActFailureMsgToOper (struct dspuTrap * pTrap)
{
  uchar* cpName = 0;

  /* Fill in the Resource Hierarchy Names - must be downstream PU/LU */
  mto_fill_nameVector(&puNameVect, pTrap->puName);
  puNameVect.next = &cpNameVect;

  /* Now do the cpName */
  if (strcmp(hostname, "Router") == 0)
    cpName = (uchar *) GetFocalPuName();
  if (cpName == 0)
    cpName = hostname;
  mto_fill_nameVector(&cpNameVect, cpName);

  /* Now fill in the Self Defined Text */ 
  sprintf(puMgtBuff, "%s DOWNSTREAM LU FAILED ACTIVATION", 
                      NVMSG_DspuLuFailAct1);
  mto_fill_selfDefTextVector(&nauSv1, puMgtBuff); 
  nauSv1.next = &nauSv2;

  sprintf(puMgtBuff, "%s %s DOWNSTREAM PU %s LOCADDR %d", 
                      NVMSG_DspuLuFailAct2, hostname, 
                      pTrap->puName, pTrap->luIdx);
  mto_fill_selfDefTextVector(&nauSv2, puMgtBuff); 
  nauSv2.next = &nauSv3;
 
  switch (pTrap->stateOrReason) {
      case LuActFailRsn_NegativeRsp:
           sprintf(puMgtBuff, "%s NEGATIVE RESPONSE", 
                  NVMSG_DspuLuFailActRsn2); 
           break;
      case LuActFailRsn_None: 
      case LuActFailRsn_Other:
      default:
           sprintf(puMgtBuff, "%s UNKNOWN OR UNDEFINED", 
                  NVMSG_DspuLuFailActRsn1); 
           break;
  }

  mto_fill_selfDefTextVector(&nauSv3, puMgtBuff); 

  MV_MessageToOperator(NV_OPER,       /* Operator Name   */
                       &puNameVect,   /* Name Sub Vector */
                       &nauSv1);      /* Text Sub Vector */
  return;
}


