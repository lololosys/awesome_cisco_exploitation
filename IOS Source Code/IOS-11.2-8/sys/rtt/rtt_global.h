/* $Id: rtt_global.h,v 1.1.4.3 1996/06/07 22:26:09 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_global.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor internal interface Header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * internal interface calls used by the components of the
 * RTT Monitor.  It also contains the return codes and
 * structure definitions.
 *------------------------------------------------------------------
 * $Log: rtt_global.h,v $
 * Revision 1.1.4.3  1996/06/07  22:26:09  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/04  18:28:34  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.1  1996/05/17  11:41:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:37  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:35  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:39  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:29  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _RTTGLOBAL_H_
#define _RTTGLOBAL_H_

#include "../os/clock.h"
#include "../snmp/snmp_api.h"
#include "../rtt/sr_rttmibtype.h"
#include "../rtt/rtt_defaults.h"

#define RTT_MAX_DELAY_MINUTES (MAXINT/ONEMIN)	/* Max scheduling delay in minutes */
#define RTT_MAX_DELAY_SECONDS (RTT_MAX_DELAY_MINUTES * SECONDS_PER_MINUTE)
#define RTT_MAX_DELAY_CENTISECONDS (RTT_MAX_DELAY_SECONDS * SYSUPTIME_UNITS)

/*
 *  The following are the structures used to store all
 *    data used in the Response Time Reporter Feature.
 */

typedef struct _short_rttMonStatsCaptureEntry_t {
    long rttMonStatsCaptureDistIndex;
    long rttMonStatsCaptureCompletions;
    long rttMonStatsCaptureOverThresholds;
    unsigned long rttMonStatsCaptureSumCompletionTime;
    unsigned long rttMonStatsCaptureSumCompletionTime2Low;
    unsigned long rttMonStatsCaptureSumCompletionTime2High;
    unsigned long rttMonStatsCaptureCompletionTimeMax;
    unsigned long rttMonStatsCaptureCompletionTimeMin;
} short_rttMonStatsCaptureEntry_t;

typedef struct _short_rttMonStatsCollectEntry_t {
    long rttMonStatsCollectNumDisconnects;
    long rttMonStatsCollectTimeouts;
    long rttMonStatsCollectBusies;
    long rttMonStatsCollectNoConnections;
    long rttMonStatsCollectDrops;
    long rttMonStatsCollectSequenceErrors;
    long rttMonStatsCollectVerifyErrors;
    boolean rttMonStatsCollectAddressShared;
    OctetString *rttMonStatsCollectAddress;
} short_rttMonStatsCollectEntry_t;

typedef struct _short_rttMonHistoryCollectionEntry_t {
    long rttMonHistoryCollectionSampleIndex;
    unsigned long rttMonHistoryCollectionSampleTime;
    boolean rttMonHistoryCollectionAddressShared;
    OctetString *rttMonHistoryCollectionAddress;
    unsigned long rttMonHistoryCollectionCompletionTime;
    long rttMonHistoryCollectionSense;
} short_rttMonHistoryCollectionEntry_t;

typedef struct connectHandleList_ {
    struct connectHandleList_ *next;
    boolean connected;
    OctetString *connAddress;
    void *connHandle;
    char valid[1];
} connectHandleList;

typedef struct pathList_ {
    struct pathList_ *next;
    OctetString *hop;
    char valid[1];
} pathList;

typedef struct rttMonReactTriggerAdminQItem_ {
    struct rttMonReactTriggerAdminQItem_ *next;
    rttMonReactTriggerAdminEntry_t *reactTriggerAdminEntry;
    char valid[1];
} rttMonReactTriggerAdminQItem;


typedef struct rttMonReactTriggerAdminQHead_ {
    rttMonReactTriggerAdminQItem *lastSearchedReactTriggerAdminQItem;
    queuetype rttMonReactTriggerAdminQ;
} rttMonReactTriggerAdminQHead;

#if defined(NOLONGLONG)

typedef struct rttMonStatsCaptureDistQItem_ {
    struct rttMonStatsCaptureDistQItem_ *next;
    unsigned long accumulation;
    short_rttMonStatsCaptureEntry_t *shortStatsCaptureEntry;
    char valid[1];
} rttMonStatsCaptureDistQItem;

#else

typedef struct rttMonStatsCaptureDistQItem_ {
    struct rttMonStatsCaptureDistQItem_ *next;
    unsigned long long accumulation;
    short_rttMonStatsCaptureEntry_t *shortStatsCaptureEntry;
    char valid[1];
} rttMonStatsCaptureDistQItem;

#endif

typedef struct rttMonStatsCaptureDistQHead_ {
    rttMonStatsCaptureDistQItem *lastSearchedStatsCaptureDistQItem;
    queuetype rttMonStatsCaptureDistQ;
} rttMonStatsCaptureDistQHead;

typedef struct rttMonStatsCaptureHopQItem_ {
    struct rttMonStatsCaptureHopQItem_ *next;
    long rttMonStatsCaptureHopIndex;
    rttMonStatsCaptureDistQHead *statsCaptDistQHead;
    short_rttMonStatsCollectEntry_t *shortStatsCollectEntry;
    char valid[1];
} rttMonStatsCaptureHopQItem;

typedef struct rttMonStatsCaptureHopQHead_ {
    rttMonStatsCaptureHopQItem *lastSearchedStatsCaptureHopQItem;
    queuetype rttMonStatsCaptureHopQ;
} rttMonStatsCaptureHopQHead;

typedef struct rttMonStatsCapturePathQItem_ {
    struct rttMonStatsCapturePathQItem_ *next;
    long rttMonStatsCapturePathIndex;
    rttMonStatsCaptureHopQHead *statsCaptureHopQHead;
    char valid[1];
} rttMonStatsCapturePathQItem;

typedef struct rttMonStatsCapturePathQHead_ {
    boolean rttMonStatsCapturePathQFull;
    rttMonStatsCapturePathQItem *currentStatsCapturePathQItem;
    rttMonStatsCapturePathQItem *lastSearchedStatsCapturePathQItem;
    queuetype rttMonStatsCapturePathQ;
} rttMonStatsCapturePathQHead;

typedef struct rttMonStatsCaptureStartTimeQItem_ {
    struct rttMonStatsCaptureStartTimeQItem_ *next;
    unsigned long rttMonStatsCaptureStartTimeIndex;
    rttMonStatsTotalsEntry_t *statsTotalsEntry;
    rttMonStatsCapturePathQHead *statsCapturePathQHead;
    char valid[1];
} rttMonStatsCaptureStartTimeQItem;

typedef struct rttMonStatsCaptureStartTimeQHead_ {
    rttMonStatsCaptureStartTimeQItem *currentStatsCaptureStartTimeQItem;
    rttMonStatsCaptureStartTimeQItem *lastSearchedStatsCaptureStartTimeQItem;
    rttMonStatsCaptureStartTimeQItem *lastAddedStatsCaptureStartTimeQItem;
    queuetype rttMonStatsCaptureStartTimeQ;
} rttMonStatsCaptureStartTimeQHead;

typedef struct rttMonHistoryCollectionSampleQItem_ {
    struct rttMonHistoryCollectionSampleQItem_ *next;
    short_rttMonHistoryCollectionEntry_t *shortHistoryCollectionEntry;
    char valid[1];
} rttMonHistoryCollectionSampleQItem;

typedef struct rttMonHistoryCollectionSampleQHead_ {
    rttMonHistoryCollectionSampleQItem *currentHistoryCollectionSampleQItem;
    rttMonHistoryCollectionSampleQItem *lastSearchedHistoryCollectionSampleQItem;
    queuetype rttMonHistoryCollectionSampleQ;
} rttMonHistoryCollectionSampleQHead;

#define D_rttMonHistoryCollectionBucketFull_True     1
#define D_rttMonHistoryCollectionBucketFull_False    2

typedef struct rttMonHistoryCollectionBucketQItem_ {
    struct rttMonHistoryCollectionBucketQItem_ *next;
    long rttMonHistoryCollectionBucketFull;
    long rttMonHistoryCollectionBucketIndex;
    rttMonHistoryCollectionSampleQHead *historyCollectionSampleQHead;
    char valid[1];
} rttMonHistoryCollectionBucketQItem;

typedef struct rttMonHistoryCollectionBucketQHead_ {
    rttMonHistoryCollectionBucketQItem *lastSearchedHistoryCollectionBucketQItem;
    rttMonHistoryCollectionBucketQItem *currentHistoryCollectionBucketQItem;
    queuetype rttMonHistoryCollectionBucketQ;
} rttMonHistoryCollectionBucketQHead;

typedef struct rttMonHistoryCollectionLifeQItem_ {
    struct rttMonHistoryCollectionLifeQItem_ *next;
    long rttMonHistoryCollectionLifeIndex;
    rttMonHistoryCollectionBucketQHead *historyCollectionBucketQHead;
    char valid[1];
} rttMonHistoryCollectionLifeQItem;

typedef struct rttMonHistoryCollectionLifeQHead_ {
    rttMonHistoryCollectionLifeQItem *currentHistoryCollectionLifeQItem;
    rttMonHistoryCollectionLifeQItem *lastSearchedHistoryCollectionLifeQItem;
    rttMonHistoryCollectionLifeQItem *lastAddedHistoryCollectionLifeQItem;
    queuetype rttMonHistoryCollectionLifeQ;
} rttMonHistoryCollectionLifeQHead;

#define MAX_ThresholdXofYArrayBuckets (MAX_rttMonReactAdminThresholdCount2 + 1)
#define MAX_ThresholdAveArrayBuckets (MAX_rttMonReactAdminThresholdCount + 1)

typedef struct rttMonReactCtrlValues_ {
    long rttMonNumberOfConsecutiveOverThresholds;
    boolean rttMonImmediateFalling;
    boolean rttMonConsecutiveFalling;
    int rttMonStartValueOfXofY;
    int rttMonCurrentValueOfXofY;
    boolean rttMonXofYArray[MAX_ThresholdXofYArrayBuckets];
    boolean rttMonXofYFalling;
    long rttMonAverageNominatorArray[MAX_ThresholdAveArrayBuckets];
    int rttMonStartValueOfAverage;
    int rttMonCurrentValueOfAverage;
    boolean rttMonAverageFalling;
    clock_epoch uniqueConnectLossToken;
    clock_epoch uniqueTimeoutToken;
    clock_epoch uniqueOverThresholdToken;
} rttMonReactCtrlValues;

typedef struct rttMonCtrlAdminQItem_ {
    struct rttMonCtrlAdminQItem_ *next;
    rttMonCtrlAdminEntry_t *ctrlAdminEntry;
    rttMonEchoAdminEntry_t *echoAdminEntry;
    rttMonFileIOAdminEntry_t *fileIOAdminEntry;
    rttMonScriptAdminEntry_t *scriptAdminEntry;
    rttMonReactTriggerAdminQHead *reactTriggerAdminQHead;
    rttMonReactCtrlValues reactCtrlValues;
    rttMonStatsCaptureStartTimeQHead *statsCaptureStartTimeQHead;
    rttMonHistoryCollectionLifeQHead *historyCollectionLifeQHead;
    pathList *tempPath;
    pathList *currPathItem;
    connectHandleList *connHandleList;
    sys_timestamp currentRttStartTime;
    pid_t pid;
    watched_boolean *pendingBoolean;
    long operTransitionState;
    boolean inTheProcessOfDying;
    boolean restartPending;
    boolean restartAfterDeath;
    mgd_timer ageoutTimer;
    mgd_timer oneHourTimer;
    mgd_timer startTimer;
    unsigned long actualStartTime;
    unsigned long operRttLifeTimerChunk;
    mgd_timer endLifeTimer;
    boolean modificationsHaveBeenMade;
    char valid[1];
} rttMonCtrlAdminQItem;


typedef struct rttMonCtrlAdminQHead_ {
    rttMonCtrlAdminQItem *lastSearchedCtrlAdminQItem;
    int schedulerPid;
    boolean schedulerActive;
    mgd_timer master;
    mgd_timer ageoutMaster;
    mgd_timer oneHourMaster;
    mgd_timer lifeTimeMaster;
    watched_boolean *aProcessDied;
    watched_boolean *schedulerResetSelf;
    queuetype rttMonCtrlAdminQ;
} rttMonCtrlAdminQHead;

/*==========================================================*
 *==========================================================*
 * configuration library calls 
 *==========================================================*
 *==========================================================*/

/*==========================================================*
 * populateOctetString
 *==========================================================*
 *
 * Description:
 *   This function will take the string and length
 *     and place them into os_str.  
 *   The first parameter defines which entry to assign
 *     error messages.
 */
extern void populateOctetString SR_PROTOTYPE((long rttMonCtrlAdminIndex,
					      OctetString * os_str,
					      unsigned char *string, long length));

/*==========================================================*
 * appendOctetString
 *==========================================================*
 *
 * Description:
 *   This function will take the string and length
 *     and append them to os_str.  
 *   The first parameter defines which entry to assign
 *     error messages.
 */
extern void appendOctetString SR_PROTOTYPE((long rttMonCtrlAdminIndex,
					    OctetString * os_str,
					    unsigned char *string, long length));

/*==========================================================* 
 *==========================================================*
 * The first set of library calls that control the
 *  overall RTT return pointers to static memory
 *  and thus do not free or manipulate the memory
 *==========================================================*
 *==========================================================*/


/*==========================================================*
 * getRttMonApplPtr
 *==========================================================*
 *
 * Description:
 *   This function returns the pointer to the stored
 *     rttMonAppl_t structure.  See sr_rttmibtype.h
 */
extern rttMonAppl_t *getRttMonApplPtr SR_PROTOTYPE((void));

/*==========================================================*
 * setRttMonAppPreConfigedReset
 *==========================================================*
 *
 * Description:
 *   This function resets the rttMonApplPreConfigedEntry_t
 *     table, and thus causing it to reload all new values.
 *     See sr_rttmibtype.h
 */
extern void setRttMonAppPreConfigedReset SR_PROTOTYPE((void));

/*==========================================================*
 * setRttMonApplReset
 *==========================================================*
 *
 * Description:
 *   This function resets the entire RTR feature and leaves
 *     it ready to be reconfigured.
 */
extern void setRttMonApplReset SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonApplSupportedRttTypesEntryPtr
 *==========================================================*
 *
 * Description:
 *   This function returns a pointer to a
 *     rttMonApplSupportedRttTypesEntry_t structure that 
 *     is indexed by the rttMonApplSupportedRttTypes 
 *     parameter.  If the entry does not exist then
 *     NULL is returned.
 */
extern rttMonApplSupportedRttTypesEntry_t
*   getRttMonApplSupportedRttTypesEntryPtr
    SR_PROTOTYPE((long rttMonApplSupportedRttTypes));

/*==========================================================*
 * getNextRttMonApplSupportedRttTypesEntryPtr
 *==========================================================*
 *
 * Description:
 *   This function returns a pointer to a
 *     rttMonApplSupportedRttTypesEntry_t structure that 
 *     is indexed by the rttMonApplSupportedRttTypes 
 *     parameter or the next valid 
 *     rttMonApplSupportedRttTypes index.  If the entry 
 *     does not exist then NULL is returned.
 */
extern rttMonApplSupportedRttTypesEntry_t
*   getNextRttMonApplSupportedRttTypesEntryPtr
    SR_PROTOTYPE((long rttMonApplSupportedRttTypes));

/*==========================================================*
 * getRttMonApplSupportedProtocolsEntryPtr
 *==========================================================*
 *
 * Description:
 *   This function returns a pointer to a
 *     rttMonApplSupportedProtocolsEntry_t structure that 
 *     is indexed by the rttMonApplSupportedProtocols 
 *     parameter.  If the entry does not exist then
 *     NULL is returned.
 */
extern rttMonApplSupportedProtocolsEntry_t
*   getRttMonApplSupportedProtocolsEntryPtr
    SR_PROTOTYPE((long rttMonApplSupportedProtocols));

/*==========================================================*
 * getNextRttMonApplSupportedProtocolsEntryPtr
 *==========================================================*
 *
 * Description:
 *   This function returns a pointer to a
 *     rttMonApplSupportedProtocolsEntry_t structure that 
 *     is indexed by the rttMonApplSupportedProtocols 
 *     parameter or the next valid 
 *     rttMonApplSupportedProtocols index.  If the entry 
 *     does not exist then NULL is returned.
 */
extern rttMonApplSupportedProtocolsEntry_t
*   getNextRttMonApplSupportedProtocolsEntryPtr
    SR_PROTOTYPE((long rttMonApplSupportedProtocols));

/*==========================================================* 
 *==========================================================*
 *  The following library calls initialize, create 
 *    (allocate memory) and free data base memory.
 *  All memory allocated by the caller is owned
 *    by the caller.
 *==========================================================*
 *==========================================================*/

/*==========================================================*
 * initBaseRttMonCtrlAdminQ
 *==========================================================*
 *
 * Description:
 *   This function is the first call made to initialize
 *     Queues for use by the whole library.
 */
extern void initBaseRttMonCtrlAdminQ SR_PROTOTYPE((void));

/*==========================================================*
 * initGlobalRttInfo
 *==========================================================*
 *
 * Description:
 *   This function is the second call made to initialize
 *     the access and structures for the whole library.
 */
extern void initGlobalRttInfo SR_PROTOTYPE((void));

/*==========================================================*
 * freeRttMonCtrlAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the ctrlAdminEntry and all of
 *     its secondary memory.  
 */
extern void freeRttMonCtrlAdminEntry SR_PROTOTYPE((rttMonCtrlAdminEntry_t
						   * ctrlAdminEntry));

/*==========================================================*
 * newRttMonCtrlAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function creates a rttMonCtrlAdminEntry_t structure 
 *    and all of its secondary memory.  Use this structure
 *    as a temporary structure on entry creation.  If this
 *    function fails, NULL is returned.
 */
extern rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntry SR_PROTOTYPE((void));

/*==========================================================*
 * setRttMonCtrlAdminEntryDefaults
 *==========================================================*
 *
 * Description:
 *   This function sets the defaults for the 
 *     inRttMonCtrlAdminEntry.
 */
extern void setRttMonCtrlAdminEntryDefaults SR_PROTOTYPE((rttMonCtrlAdminEntry_t
							  * inRttMonCtrlAdminEntry));

/*==========================================================*
 * freeRttMonEchoAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the echoAdminEntry and all of
 *     its secondary memory.  
 */
extern void freeRttMonEchoAdminEntry SR_PROTOTYPE((rttMonEchoAdminEntry_t
						   * echoAdminEntry));

/*==========================================================*
 * newRttMonEchoAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function creates a rttMonEchoAdminEntry_t structure 
 *    and all of its secondary memory.  Use this structure
 *    as a temporary structure on entry creation.  If this
 *    function fails, NULL is returned.
 */
extern rttMonEchoAdminEntry_t *newRttMonEchoAdminEntry SR_PROTOTYPE((void));

/*==========================================================*
 * setRttMonEchoAdminEntryDefaults
 *==========================================================*
 *
 * Description:
 *   This function sets the defaults for the 
 *     inRttMonEchoAdminEntry.
 */
extern void setRttMonEchoAdminEntryDefaults SR_PROTOTYPE((rttMonEchoAdminEntry_t
							  * inRttMonEchoAdminEntry));

/*==========================================================*
 * freeRttMonCtrlAdminQItemByIndex
 *==========================================================*
 *
 * Description:
 *   This function unqueues and frees the ctrlAdminQItem 
 *     that is indexed by rttMonCtrlAdminIndex and its
 *     secondary memory.  
 *   Possible Return Codes:
 *     D_Return_Successfull
 *     D_InvalidParameter
 */
extern int freeRttMonCtrlAdminQItemByIndex SR_PROTOTYPE((long
							 rttMonCtrlAdminIndex));

/*==========================================================*
 * freeRttMonCtrlAdminQItem
 *==========================================================*
 *
 * Description:
 *   This function unqueues and frees the ctrlAdminQItem 
 *     that is pointed to by inRttMonCtrlAdminQItem and its
 *     secondary memory.  
 *   Possible Return Codes:
 *     D_Return_Successfull
 *     D_InvalidParameter
 */
extern int freeRttMonCtrlAdminQItem SR_PROTOTYPE((rttMonCtrlAdminQItem
						  * inRttMonCtrlAdminQItem));

/*==========================================================*
 * newRttMonCtrlAdminQItem
 *==========================================================*
 *
 * Description:
 *   This function creates a rttMonCtrlAdminQItem and queue's
 *    that item into the AdminQ.  The first parameter is a
 *    buffer created with the newRttMonCtrlAdminEntry function.
 *    The second parameter is a buffer created with the
 *    newRttMonEchoAdminEntry function.  Both of these
 *    parameters require the rttMonCtrlAdminIndex values of the 
 *    buffers to be valid and the rttMonCtrlAdminRttType of
 *    the ctrlAdminEntryBuffer to be valid. Since these
 *    values are qeys, they should not be changed. 
 *    The function copies all information from the incomming
 *    buffers and the caller ownes the buffers for reuse or
 *    freeing.
 *   This function will also start the scheduler on its first
 *    call.
 *   Possible Return Codes:
 *     D_Return_Successfull
 *     D_InvalidParameter    -> NULL Buffer pointer
 *     D_MemoryAllocationFailed
 *     D_DataFieldIsInvalid  -> bad RttType
 */
extern int newRttMonCtrlAdminQItem SR_PROTOTYPE((rttMonCtrlAdminEntry_t
						 * ctrlAdminEntryBuff,
						 void
						 *specificAdminEntryBuff));

/*==========================================================*
 * createRttMonCtrlAdminQItemSecondaryMemory
 *==========================================================*
 *
 * Description:
 *  This function must be called sometime after the 
 *    newRttMonCtrlAdminQItem call, but after
 *    any updateRttMonCtrlAdminEntry or
 *    updateRttMonEchoAdminEntry calls.  
 *  This function creates the memory for tables and all
 *    stored data that will be used by a probe.
 *  On Failure the WHOLE ENTRY will be deleted 
 *    as if this were a call to the 
 *    freeRttMonCtrlAdminQItem function.  It is 
 *    assumed the caller will only use this to
 *    create secondary memory when new needs to
 *    be done in multiple steps.  The best way
 *    to use this call is to call it directly
 *    following the newRttMonCtrlAdminQItem, and
 *    do not use that call until all values are
 *    correct, and thus the update... calls will 
 *    not be needed.  (But, in SNMP, it must be 
 *    done is steps.) 
 *   Possible Return Codes:
 *     D_Return_Successfull
 *     D_InvalidParameter
 *     D_MemoryAllocationFailed
 */
extern int createRttMonCtrlAdminQItemSecondaryMemory SR_PROTOTYPE((long
								   rttMonCtrlAdminIndex));

/*==========================================================*
 * rttMonCtrlAdminQItemSecondaryMemory
 *==========================================================*
 *
 * Description:
 *  This function returns whether secondary memory already
 *    exists.  If it already exists no updates can be made
 *    to the Ctrl and Echo Admin structures and the entry
 *    must be deleted in order to change these values. 
 *   Possible Return Codes:
 *     D_Return_Successfull
 *     D_InvalidParameter
 */
extern int rttMonCtrlAdminQItemSecondaryMemory SR_PROTOTYPE((long
							     rttMonCtrlAdminIndex,
							     boolean * exists));

/*==========================================================*
 * freeRttMonCtrlAdminQ
 *==========================================================*
 *
 * Description:
 *   This function unqueues and frees every ctrlAdminQItem.
 *     and their secondary memory.  The base Queue will
 *     continue to exist, so new entries can be added 
 *     without calling initBaseRttMonCtrlAdminQ a second
 *     time.  Finally the scheduler will be stopped until
 *     it is needed again via the first
 *     newRttMonCtrlAdminQItem.
 */
extern void freeRttMonCtrlAdminQ SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonCtrlAdminQSize
 *==========================================================*
 *
 * Description:
 *   This function returns the number of entries in the 
 *     admin Queue.
 *   Possible Return Values:
 *    Any valid integer
 */
extern int getRttMonCtrlAdminQSize SR_PROTOTYPE((void));

/*==========================================================* 
 *==========================================================*
 * The following routines are used to manipulate the values of
 *  data that already have storage space and are queued.
 *==========================================================* 
 *==========================================================*/

/*==========================================================*
 * getRttMonCtrlAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outCtrlAdminEntryBuff with
 *     the entry they matches the rttMonCtrlAdminIndex value.
 *  The outCtrlAdminEntryBuff must be created with the 
 *    newRttMonCtrlAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonCtrlAdminEntry SR_PROTOTYPE((long
						 rttMonCtrlAdminIndex,
						 rttMonCtrlAdminEntry_t
						 * outCtrlAdminEntryBuff));

/*==========================================================*
 * getNextRttMonCtrlAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outCtrlAdminEntryBuff with
 *     the entry they matches the rttMonCtrlAdminIndex value
 *     or its next greatest rttMonCtrlAdminIndex value.
 *  The outCtrlAdminEntryBuff must be created with the 
 *    newRttMonCtrlAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonCtrlAdminEntry SR_PROTOTYPE((long
						     rttMonCtrlAdminIndex,
						     rttMonCtrlAdminEntry_t
						     * outCtrlAdminEntryBuff));

/*==========================================================*
 * getRttMonEchoAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outEchoAdminEntryBuff with
 *     the entry they matches the rttMonCtrlAdminIndex value.
 *  The outEchoAdminEntryBuff must be created with the 
 *    newRttMonEchoAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonEchoAdminEntry SR_PROTOTYPE((long
						 rttMonCtrlAdminIndex,
						 rttMonEchoAdminEntry_t
						 * outEchoAdminEntryBuff));

/*==========================================================*
 * getNextRttMonEchoAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outEchoAdminEntryBuff with
 *     the entry they matches the rttMonEchoAdminIndex value
 *     or its next greatest rttMonEchoAdminIndex value.
 *  The outEchoAdminEntryBuff must be created with the 
 *    newRttMonEchoAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonEchoAdminEntry SR_PROTOTYPE((long
						     rttMonCtrlAdminIndex,
						     rttMonEchoAdminEntry_t
						     * outEchoAdminEntryBuff));

/*==========================================================*
 * getRttMonCtrlAdminQItem
 *==========================================================*
 *
 * Description:
 *   This function returns a pointer to an active queue entry.
 *     If the entry is not found, NULL will be returned.
 */
extern rttMonCtrlAdminQItem *getRttMonCtrlAdminQItem SR_PROTOTYPE((long
								   rttMonCtrlAdminIndex));

/*==========================================================*
 * updateRttMonCtrlAdminEntry
 *==========================================================*
 *
 * Description:
 *  This method will update the current ctrlAdminEntry->
 *  rttMonCtrlAdminIndex item.  If the Entry does not exist 
 *  this function will fail.  The Control Admin Queue 
 *  item that holds the ctrlAdminEntry must have already 
 *  been created.
 * The paramenter passed in is the owner of the caller.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int updateRttMonCtrlAdminEntry SR_PROTOTYPE((rttMonCtrlAdminEntry_t
						    * ctrlAdminEntryBuff));

/*==========================================================*
 * updateRttMonEchoAdminEntry
 *==========================================================*
 *
 * Description:
 *  This method will update the current echoAdminEntry 
 *  indexed by rttMonCtrlAdminIndex.  If the Entry does not 
 *  exist this function will fail.  The Control Admin Queue 
 *  item that holds the echoAdminEntry must have already 
 *  been created.
 * The paramenter passed in is the owner of the caller.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int updateRttMonEchoAdminEntry SR_PROTOTYPE((long
						    rttMonCtrlAdminIndex,
						    rttMonEchoAdminEntry_t
						    * echoAdminEntryBuff));

/*==========================================================*
 * setRttMonCtrlAdminStatus
 *==========================================================*
 *
 * Description:
 *  This function will change the state of an entry.  If the
 *    state change is not different that the current state
 *    only the modification time will be updated.  Depending
 *    on the current state of the entry and the new state
 *    value, different actions will take place.  See
 *    RTTMON mib for state change values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 *    D_OperStateInTransition
 *    D_Illegal_Transition
 */
extern int setRttMonCtrlAdminStatus SR_PROTOTYPE((long
						  rttMonCtrlAdminIndex,
						  long
						  rttMonCtrlAdminStatus));

/*==========================================================*
 * setRttMonScheduleAdminRttLife
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the RttLife for
 *    the entry indexed by rttMonCtrlAdminIndex.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonScheduleAdminRttLife SR_PROTOTYPE((long
						       rttMonCtrlAdminIndex,
						       long
						       rttMonScheduleAdminRttLife));

/*==========================================================*
 * setRttMonScheduleAdminRttStartTime
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonScheduleAdminRttStartTime for
 *    the entry indexed by rttMonCtrlAdminIndex.
 *  Value of: 0 means start pending and when the life ends
 *               restart pending again
 *            1 means start now
 *           >1 means start at that sysuptime in the future,
 *               if that time has already went by, start
 *               pending
 *  This value is in hundreths of seconds.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonScheduleAdminRttStartTime SR_PROTOTYPE((long
							    rttMonCtrlAdminIndex,
							    long
							    rttMonScheduleAdminRttStartTime));

/*==========================================================*
 * setRttMonScheduleAdminConceptRowAgeout
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonScheduleAdminConceptRowAgeout for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * This value is in hundreths of seconds.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonScheduleAdminConceptRowAgeout SR_PROTOTYPE((long
								rttMonCtrlAdminIndex,
								long
								rttMonScheduleAdminConceptRowAgeout));

/*==========================================================*
 * setRttMonReactAdminConnectionEnable
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminConnectionEnable for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * This value is 1 for True and 0 for False.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminConnectionEnable SR_PROTOTYPE((long
							     rttMonCtrlAdminIndex,
							     long
							     rttMonReactAdminConnectionEnable));

/*==========================================================*
 * setRttMonReactAdminTimeoutEnable
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminTimeoutEnable for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * This value is 1 for True and 0 for False.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminTimeoutEnable SR_PROTOTYPE((long
							  rttMonCtrlAdminIndex,
							  long
							  rttMonReactAdminTimeoutEnable));

/*==========================================================*
 * setRttMonReactAdminThresholdType
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminThresholdType for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * See the RTTMON mib for possible values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminThresholdType SR_PROTOTYPE((long
							  rttMonCtrlAdminIndex,
							  long
							  rttMonReactAdminThresholdType));

/*==========================================================*
 * setRttMonReactAdminThresholdFalling
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminThresholdType for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * This value is in milliseconds.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminThresholdFalling SR_PROTOTYPE((long
							     rttMonCtrlAdminIndex,
							     long
							     rttMonReactAdminThresholdFalling));

/*==========================================================*
 * setRttMonReactAdminThresholdCount
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminThresholdCount for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * See the RTTMON mib for possible values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminThresholdCount SR_PROTOTYPE((long
							   rttMonCtrlAdminIndex,
							   long
							   rttMonReactAdminThresholdCount));

/*==========================================================*
 * setRttMonReactAdminThresholdCount2
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminThresholdCoun2t for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * See the RTTMON mib for possible values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminThresholdCount2 SR_PROTOTYPE((long
							    rttMonCtrlAdminIndex,
							    long
							    rttMonReactAdminThresholdCount2));

/*==========================================================*
 * setRttMonReactAdminActionTypes
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonReactAdminActionType for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * See the RTTMON mib for possible values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactAdminActionTypes SR_PROTOTYPE((long
							rttMonCtrlAdminIndex,
							long
							rttMonReactAdminActionType));

/*==========================================================*
 * getCurrentFrequency
 *==========================================================*
 *
 * Description:
 *  This function will return the current Frequency
 *    of the entry indexed by rttMonCtrlAdminIndex. If
 *    that entry cannot be found zero is returned.
 */
extern long getCurrentFrequency SR_PROTOTYPE((long rttMonCtrlAdminIndex));

/*==========================================================*
 * getCurrentOperState
 *==========================================================*
 *
 * Description:
 *  This function will return the current operational
 *    state of the entry indexed by rttMonCtrlAdminIndex.
 */
extern long getCurrentOperState SR_PROTOTYPE((long rttMonCtrlAdminIndex));

/*==========================================================*
 * setRttMonCtrlOperState
 *==========================================================*
 *
 * Description:
 *  This function will change the value of the 
 *    rttMonCtrlOperState for
 *    the entry indexed by rttMonCtrlAdminIndex.
 * See the RTTMON mib for possible values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 *    D_OperStateInTransition
 *    D_Illegal_Transition
 */
extern int setRttMonCtrlOperState SR_PROTOTYPE((long
						rttMonCtrlAdminIndex,
						long
						rttMonCtrlOperState));

/*==========================================================*
 * freeRttMonStatsCollectEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the statsCollectEntry structure and
 *     all of its secondary memory.
 */
extern void freeRttMonStatsCollectEntry SR_PROTOTYPE((rttMonStatsCollectEntry_t
						      * statsCollectEntry));

/*==========================================================*
 * newRttMonStatsCollectEntry
 *==========================================================*
 *
 * Description:
 *   This function creates and returns 
 *     rttMonStatsCollectEntry_t structure and all of its 
 *     secondary memory.  If it fails NULL will be returned.
 */
extern rttMonStatsCollectEntry_t *newRttMonStatsCollectEntry SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonStatsCollectEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outCollectBuff with
 *     the entry they matches the four index values.
 *  The outCollectBuff must be created with the 
 *    newRttMonStatsCollectEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonStatsCollectEntry SR_PROTOTYPE((long
						    rttMonCtrlAdminIndex,
						    unsigned long
						    rttMonStatsCaptureStartTimeIndex,
						    long
						    rttMonStatsCapturePathIndex,
						    long
						    rttMonStatsCaptureHopIndex,
						    rttMonStatsCollectEntry_t
						    * outCollectBuff));

/*==========================================================*
 * getNextRttMonStatsCollectEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outCollectBuff with
 *     the entry they matches the four index values
 *     or its next greatest index value.
 *  The outCollectBuff must be created with the 
 *    newRttMonStatsCollectEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonStatsCollectEntry SR_PROTOTYPE((long
							rttMonCtrlAdminIndex,
							unsigned long
							rttMonStatsCaptureStartTimeIndex,
							long
							rttMonStatsCapturePathIndex,
							long
							rttMonStatsCaptureHopIndex,
							rttMonStatsCollectEntry_t
							* outCollectBuff));

/*==========================================================*
 * freeRttMonReactTriggerAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the reactTriggerAdminEntry
 *     structure and all of its secondary memory.
 */
extern void freeRttMonReactTriggerAdminEntry SR_PROTOTYPE((rttMonReactTriggerAdminEntry_t
							   * reactTriggerAdminEntry));

/*==========================================================*
 * newRttMonReactTriggerAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function creates and returns 
 *     rttMonReactTriggerAdminEntry_t structure and all of its 
 *     secondary memory.  If it fails NULL will be returned.
 */
extern rttMonReactTriggerAdminEntry_t *newRttMonReactTriggerAdminEntry SR_PROTOTYPE((void));

/*==========================================================*
 * addUpdateRttMonReactTriggerAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function is used to add an entry into the Trigger 
 *     Queue. The paramenter passed in is the owner of the 
 *     caller.  The following must
 *        be VALID:
 *          inTriggerAdminBuff->rttMonCtrlAdminIndex
 *        The following must been a valid index
 *          rttMonReactTriggerAdminRttMonCtrlAdminIndex
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_QueueFull
 *    D_Return_Successfull
 */
extern int addUpdateRttMonReactTriggerAdminEntry SR_PROTOTYPE((long
							       rttMonCtrlAdminIndex,
							       rttMonReactTriggerAdminEntry_t
							       * inTriggerAdminBuff));

/*==========================================================*
 * setRttMonReactTriggerAdminStatus
 *==========================================================*
 *
 * Description:
 *  This function will change the state of a reaction Trigger
 *    entry.  
 *  See RTTMON mib for state change values.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int setRttMonReactTriggerAdminStatus SR_PROTOTYPE((long
							  rttMonCtrlAdminIndex,
							  long
							  rttMonReactTriggerAdminRttMonCtrlAdminIndex,
							  long
							  rttMonReactTriggerAdminStatus));

/*==========================================================*
 * getRttMonReactTriggerAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outTriggerAdminBuff with
 *     the entry they matches the two index values.
 *  The outTriggerAdminBuff must be created with the 
 *    newRttMonReactTriggerAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonReactTriggerAdminEntry SR_PROTOTYPE((long
							 rttMonCtrlAdminIndex,
							 long
							 rttMonReactTriggerAdminRttMonCtrlAdminIndex,
							 rttMonReactTriggerAdminEntry_t
							 * outTriggerAdminBuff));

/*==========================================================*
 * getNextRttMonReactTriggerAdminEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outTriggerAdminBuff with
 *     the entry they matches the two index values
 *     or its next greatest index value.
 *  The outTriggerAdminBuff must be created with the 
 *    newRttMonReactTriggerAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonReactTriggerAdminEntry SR_PROTOTYPE((long
							     rttMonCtrlAdminIndex,
							     long
							     rttMonReactTriggerAdminRttMonCtrlAdminIndex,
							     rttMonReactTriggerAdminEntry_t
							     * outTriggerAdminBuff));

/*==========================================================*
 * freeRttMonStatsCaptureDistEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the statsCaptureDistEntry
 *     structure and all of its secondary memory.
 */
extern void freeRttMonStatsCaptureDistEntry SR_PROTOTYPE((rttMonStatsCaptureEntry_t
							  * statsCaptureEntry));

/*==========================================================*
 * newRttMonStatsCaptureDistEntry
 *==========================================================*
 *
 * Description:
 *   This function creates and returns 
 *     rttMonStatsCaptureEntry_t structure and all of its 
 *     secondary memory.  If it fails NULL will be returned.
 */
extern rttMonStatsCaptureEntry_t *newRttMonStatsCaptureDistEntry SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonStatsCaptureDistEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outDistBuff with
 *     the entry they matches the five index values.
 *  The outDistBuff must be created with the 
 *    newRttMonStatsCaptureDistEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonStatsCaptureDistEntry SR_PROTOTYPE((long
							rttMonCtrlAdminIndex,
							unsigned long
							rttMonStatsCaptureStartTimeIndex,
							long
							rttMonStatsCapturePathIndex,
							long
							rttMonStatsCaptureHopIndex,
							long
							rttMonStatsCaptureDistIndex,
							rttMonStatsCaptureEntry_t
							* outDistBuff));

/*==========================================================*
 * getNextRttMonStatsCaptureDistEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outDistBuff with
 *     the entry they matches the two index values
 *     or its next greatest index value.
 *  The outDistBuff must be created with the 
 *    newRttMonReactTriggerAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonStatsCaptureDistEntry SR_PROTOTYPE((long
							    rttMonCtrlAdminIndex,
							    unsigned
							    long
							    rttMonStatsCaptureStartTimeIndex,
							    long
							    rttMonStatsCapturePathIndex,
							    long
							    rttMonStatsCaptureHopIndex,
							    long
							    rttMonStatsCaptureDistIndex,
							    rttMonStatsCaptureEntry_t
							    * outDistBuff));

/*==========================================================*
 * freeRttMonStatsTotalsEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the statsTotalsEntry
 *     structure and all of its secondary memory.
 */
extern void freeRttMonStatsTotalsEntry SR_PROTOTYPE((rttMonStatsTotalsEntry_t
						     * statsTotalsEntry));

/*==========================================================*
 * newRttMonStatsTotalsEntry
 *==========================================================*
 *
 * Description:
 *   This function creates and returns 
 *     rttMonStatsTotalsEntry_t structure and all of its 
 *     secondary memory.  If it fails NULL will be returned.
 */
extern rttMonStatsTotalsEntry_t *newRttMonStatsTotalsEntry SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonStatsTotalsEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outTotalsBuff with
 *     the entry they matches the two index values.
 *  The outTotalsBuff must be created with the 
 *    newRttMonStatsCaptureDistEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonStatsTotalsEntry SR_PROTOTYPE((long
						   rttMonCtrlAdminIndex,
						   unsigned long
						   rttMonStatsCaptureStartTimeIndex,
						   rttMonStatsTotalsEntry_t
						   * outTotalsBuff));

/*==========================================================*
 * getNextRttMonStatsTotalsEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outTotalsBuff with
 *     the entry they matches the two index values
 *     or its next greatest index value.
 *  The outTotalsBuff must be created with the 
 *    newRttMonReactTriggerAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonStatsTotalsEntry SR_PROTOTYPE((long
						       rttMonCtrlAdminIndex,
						       unsigned long
						       rttMonStatsCaptureStartTimeIndex,
						       rttMonStatsTotalsEntry_t
						       * outTotalsBuff));

/*==========================================================*
 * freeRttMonHistoryCollectionEntry
 *==========================================================*
 *
 * Description:
 *   This function frees the historyCollectionEntry
 *     structure and all of its secondary memory.
 */
extern void freeRttMonHistoryCollectionEntry SR_PROTOTYPE((rttMonHistoryCollectionEntry_t
							   * historyCollectionEntry));

/*==========================================================*
 * newRttMonHistoryCollectionEntry
 *==========================================================*
 *
 * Description:
 *   This function creates and returns 
 *     rttMonHistoryCollectionEntry_t structure and all of its 
 *     secondary memory.  If it fails NULL will be returned.
 */
extern rttMonHistoryCollectionEntry_t *newRttMonHistoryCollectionEntry SR_PROTOTYPE((void));

/*==========================================================*
 * getRttMonHistoryCollectionEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outSampleBuff with
 *     the entry they matches the four index values.
 *  The outSampleBuff must be created with the 
 *    newRttMonStatsCaptureDistEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_Return_Successfull
 */
extern int getRttMonHistoryCollectionEntry SR_PROTOTYPE((long
							 rttMonCtrlAdminIndex,
							 long
							 rttMonHistoryCollectionLifeIndex,
							 long
							 rttMonHistoryCollectionBucketIndex,
							 long
							 rttMonHistoryCollectionSampleIndex,
							 rttMonHistoryCollectionEntry_t
							 * outSampleBuff));

/*==========================================================*
 * getNextRttMonHistoryCollectionEntry
 *==========================================================*
 *
 * Description:
 *   This function fills the outSampleBuff with
 *     the entry they matches the four index values
 *     or its next greatest index value.
 *  The outSampleBuff must be created with the 
 *    newRttMonReactTriggerAdminEntry.
 *   Possible Return Codes:
 *    D_InvalidParameter
 *    D_EntryNotFound
 *    D_Return_Successfull
 */
extern int getNextRttMonHistoryCollectionEntry SR_PROTOTYPE((long
							     rttMonCtrlAdminIndex,
							     long
							     rttMonHistoryCollectionLifeIndex,
							     long
							     rttMonHistoryCollectionBucketIndex,
							     long
							     rttMonHistoryCollectionSampleIndex,
							     rttMonHistoryCollectionEntry_t
							     * outSampleBuff));

/*==========================================================*
 * setRttMonSchedulerPid
 *==========================================================*
 *
 * Description:
 *   This function updates the libraries with the Process
 *     ID of the Scheduler Process.  And sets the flag
 *     that tells the library if the scheduler is active.
 */
extern void setRttMonSchedulerPid SR_PROTOTYPE((pid_t pid));

/*==========================================================*
 * rttMonSchedulerInit
 *==========================================================*
 *
 * Description:
 *   This function Initializes the Scheduler's timers and
 *     booleans.
 */
extern void rttMonSchedulerInit SR_PROTOTYPE((void));

/*==========================================================*
 * rttMonSchedulerUpdate
 *==========================================================*
 *
 * Description:
 *   This function is called by the scheduler every time
 *     one of its timers and or booleans pop.  This function
 *     handles all starting/stopping, transitioning, and
 *     end of the hour processing for every probe.
 *   If the return value is True the scheduler should
 *     kill itself.
 */
extern boolean rttMonSchedulerUpdate SR_PROTOTYPE((void));


/*==========================================================*
 * rttMonSchedulerCleanup
 *==========================================================*
 *
 * Description:
 *   This function is called by the scheduler every time
 *     when it is told to kill itself.  It will cleanup
 *     all timers and booleans the scheduler is using.
 *   This function calls process_may_suspend() in many
 *     locations.
 */
extern void rttMonSchedulerCleanup SR_PROTOTYPE((void));

#endif /* _RTTGLOBAL_H_ */
      
