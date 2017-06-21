/* $Id: snmp_trap_fe.c,v 3.5.8.4 1996/08/28 13:09:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/snmp/snmp_trap_fe.c,v $
 *------------------------------------------------------------------
 * SNMP trap front end process. This program provides a simple interface for
 * user to send a trap, either at interrupt or process level. 
 * 
 * Jan 1995, Barry Chan(Metaplex)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_trap_fe.c,v $
 * Revision 3.5.8.4  1996/08/28  13:09:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.8.3  1996/08/15  22:47:37  ramk
 * CSCdi65392:  Link up/down traps generated before system configured
 * Branch: California_branch
 * removed "noisy" error messages
 *
 * Revision 3.5.8.2  1996/07/31  00:10:08  ramk
 * CSCdi57452:  SNMP Trap overflow on boot-up
 * Branch: California_branch
 * Trap queue implemented using buffers allocated from system buffer pool.
 * Buginfs changed to errmsgs.
 *
 * Revision 3.5.8.1  1996/03/18  21:48:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:30:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/07  12:33:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  17:13:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/28  09:33:39  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.4  1996/02/12  06:58:19  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.3  1995/12/14  08:27:31  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  18:58:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/17  04:24:37  bchan
 * CSCdi36821:  SNMP trap process started without SNMP being configured
 *
 * Revision 2.3  1995/07/24  18:24:20  sdurham
 * CSCdi37557:  snmp stops responding
 *                 snmp processes (ip and trap) need to process all queue
 *                 elements when they get called.
 *
 * Revision 2.2  1995/07/17  07:58:21  bchan
 * CSCdi36821:  SNMP trap process started without SNMP being configured
 *
 * Revision 2.1  1995/06/07  22:48:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "packet.h"
#include <ciscolib.h>
#include "sr_snmpd.h"
#include "logger.h"
#include "msg_snmp.c"			/* Not a typo, see logger.h */

#include "snmp_registry.h"
#include "snmp_trap_fe.h"

/*
 * Note: 
 * A condition that generates a trap (e.g. link down) might change
 * after the trap has been generated and enqueued, but before 
 * it has been dequeued and processed by the snmp trap process. 
 * In such cases (e.g. when the link comes up before the 
 * corresponding linkDown trap is processed), the original 
 * (linkDown) trap will be sent, but the status will indicate the 
 * current condition (i.e. "link is up").
 */


/* 
 * MAXINDEX defines the max length of the instance field,
 * which stores the instanceOID for an snmp trap.
 * This value can be increased, if necessary.
 */
#define MAXINDEX 8

/*Max traps processed before relinquishing CPU */
#define SNMP_TRAP_SWITCHCOUNT 100

/* structure to save trap parameters to pass from interrupt to process level */
typedef struct SnmpTrapParm_ {
                     const OID* vblist;        /* varbind list */
                     ulong  instance[MAXINDEX];         /* maximum MAXINDEX
							   indexes */
                     const OID*   enterprise;
                     const char*  v2TrapOID;
                     unsigned short instanceLength;
                     unsigned short varbindCount;
                     int          genericTrapNum;
                     int          specificTrapNum;
		  }SnmpTrapParm;

static pid_t snmp_trap_pid = NO_PROCESS;
static watched_queue *SnmpTrapQueue = NULL;

/* 
 * This is a generic Snmp interface. This will make the calling of the
 * trap a lot simpler and can be called during interrupt
 */
void SnmpSendTrap_Generic (const OID* vblist,        /* varbind list */
                     const OID*   enterprise,
                     const char*  v2TrapOID,
                     const OID*   instanceOID,         /* index of traps */
                     unsigned short varbindCount,
                     int          genericTrapNum,
                     int          specificTrapNum)
{
  SnmpTrapParm* trapBuffer;
  paktype* trap_pkt;
  
  /* 
   * If SnmpTrapProcess is not created, silently drop
   * trap and return.
   */
  if (snmp_trap_pid == NO_PROCESS) {
    return;
  }

  /* 
   * Get buffer from pool.
   * If buffer is not allocated, drop trap and return 
   */
  trap_pkt = getbuffer(sizeof(SnmpTrapParm));
  if (!trap_pkt) {
    return;
  }

  trapBuffer = (SnmpTrapParm *) trap_pkt->data_area;
  /* 
   * Assumption - No check is made to test if trapBuffer is NULL.
   * It is assumed that the trap_pkt->data_area value is non-NULL
   * and uncorrupted 
   */
  trapBuffer->vblist = vblist;
  trapBuffer->enterprise = enterprise;
  trapBuffer->v2TrapOID = v2TrapOID;
  trapBuffer->specificTrapNum = specificTrapNum;
  trapBuffer->genericTrapNum = genericTrapNum;
  trapBuffer->instanceLength = instanceOID->length;
  trapBuffer->varbindCount = varbindCount;
  
  if (trapBuffer->instanceLength > MAXINDEX) {
    errmsg(&msgsym(BADVALUE, SNMP));
    trapBuffer->instanceLength = MAXINDEX;      /* maximum index
						   supported = MaxIndex */
  }

  bcopy(instanceOID->oid_ptr, trapBuffer->instance,
	sizeof(ulong) * trapBuffer->instanceLength);
  
  /* Enqueue the trap, and wake up process */
  if (process_enqueue(SnmpTrapQueue, trap_pkt)) {
    return;
  } else {
    /* 
     * Possibly no snmp_trap process - free allocated buffer 
     */
    datagram_done(trap_pkt);
    errmsg(&msgsym(TRAPDROP, SNMP),
	   "Could not enqueue snmp trap - dropped trap");
    return;
  }
}

/* 
 * This process is used to handle send trap enqueued at
 * interrupt level 
*/
static process SnmpTrapProcess (void)
{
  paktype *trap_pkt;
  SnmpTrapParm *trapBuffer;
  int trap_cnt;
  int  index;
  OID **vbList;
  OID  instanceOID;

  process_watch_queue(SnmpTrapQueue,ENABLE,RECURRING);
  for (;;) {
    process_wait_for_event();
    /* 
     * trap count is used to make sure that the SnmpTrapProcess does:
     *   1. not hog the cpu
     *   2. frees buffers quickly
     * The aim is to let a max of SNMP_TRAP_SWITCHCOUNT trap packets be 
     * processed consecutively, before relinquishing the CPU. This way, 
     * a max of 100 buffers are freed at one stretch of cpu usage. Then
     * the process will execute a process_may suspend() command to
     * allow more critical processes to run. This should also ensure
     * that the process does not "thrash"
     */
    trap_cnt = SNMP_TRAP_SWITCHCOUNT; 

    /* 
     * WARNING!  :
     * Decrement trap_cnt BEFORE dequeueing a packet from the queue.
     * If this sequence of events is reversed,
     * a trap could be dequeued, but trap_cnt could be zero.
     * thus this packet could be lost forever - never processed
     * and never reclaimed
     */
    while ((trap_cnt-- > 0) &&
	    ((trap_pkt = process_dequeue(SnmpTrapQueue)) != NULL)) {
      trapBuffer = (SnmpTrapParm *) trap_pkt->data_area;
      
      /* Create the instance OID for index */
      instanceOID.oid_ptr = trapBuffer->instance;
      instanceOID.length = trapBuffer->instanceLength;
      
      /* 
       * Create the VarBind list. Allocate space for
       * NULL ptr (last element) also - this accounts for the "+ 1"
       * in the malloc call.
       * If malloc returns NULL, free packet buffer and
       * go to next loop cycle quietly - no point screaming out 
       * an error mesg
       */
      vbList = malloc(sizeof(OID *) * (trapBuffer->varbindCount + 1));
      if (vbList == NULL) {
	datagram_done(trap_pkt);
      } else {
	for (index = 0; index < trapBuffer->varbindCount; index++) {
	  vbList[index] = CatOID((OID *)&trapBuffer->vblist[index], 
				 &instanceOID);
	}
	
	vbList[index] = NULL;
	
	/* Generate the TRAP */
	snmp_trap(trapBuffer->genericTrapNum, trapBuffer->specificTrapNum, 
		  vbList,	(OID *)trapBuffer->enterprise, 
		  (char*)trapBuffer->v2TrapOID);
	
	/* Free the VarBind list */
	while (index) {
	  FreeOID(vbList[--index]);
	}
	
	/* Free allocated memory for vbList & pkt buffer */
	free(vbList);
	datagram_done(trap_pkt);
      }
      process_may_suspend();
    }
  }
}



void init_snmp_trap (void)
{
  reg_add_SnmpTrapFeProcess(SnmpTrapFeProcess, "SnmpTrapFeProcess");
}



/* snmp trap front end initialisation routine */
void SnmpTrapFeProcess (boolean createflag)
{
  paktype* trap_pkt;
  
  /* 
   * If createflag == FALSE, kill the process if it exists, and also
   * free any buffers on the SnmpTrapQueue, and delete the Queue.
   *
   * If createflag == TRUE, create the SnmpTrapProcess and create
   * SnmpTrapQueue.
   */
  if (!createflag) {
    if (snmp_trap_pid != NO_PROCESS) {
      process_kill(snmp_trap_pid);
      snmp_trap_pid = NO_PROCESS;
      
      if (SnmpTrapQueue) {
	while ((trap_pkt = process_dequeue(SnmpTrapQueue)) != NULL) {
	  datagram_done(trap_pkt);
	}
	delete_watched_queue(&SnmpTrapQueue);
      }
    }
  } else { 
    /* 
     * createflag == TRUE 
     */
    if (snmp_trap_pid == NO_PROCESS) {
      snmp_trap_pid = process_create(SnmpTrapProcess, "SNMP Traps",
				     NORMAL_STACK, PRIO_NORMAL);
      if (snmp_trap_pid != NO_PROCESS) {
	process_set_crashblock(snmp_trap_pid, TRUE);

	/* 
	 * Process created. Create watched_queue for process.
	 * If creation fails, kill process, print errmsg and return
	 */
	SnmpTrapQueue = create_watched_queue("SnmpTrapQueue",0,0);
	if (!SnmpTrapQueue) {
	  /* creation failed */
	  process_kill(snmp_trap_pid);
	  snmp_trap_pid = NO_PROCESS;
	  errmsg(&msgsym(TRAPDROP, SNMP), "snmp trap process not created"); 
	}
      }
    }
  }
}
