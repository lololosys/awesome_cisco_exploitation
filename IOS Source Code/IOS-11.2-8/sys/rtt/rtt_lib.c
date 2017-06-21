/* $Id: rtt_lib.c,v 1.1.4.7 1996/08/28 13:08:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_lib.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Library.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor MIB libraries.
 *------------------------------------------------------------------
 * $Log: rtt_lib.c,v $
 * Revision 1.1.4.7  1996/08/28  13:08:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.6  1996/07/29  22:31:04  snyder
 * CSCdi64483:  declare arrays const in sub__rtt_mon
 *              788 out of data, 12 out of image
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/07  22:26:14  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/04  18:28:44  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.3  1996/05/27  05:48:54  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/05/22  17:51:40  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Occurances -> Occurrences
 * Occurences -> Occurrences
 * hundreths  -> hundredths
 * "unknown -> unkown" -> unknown
 * Varify -> Verify
 * Currrent -> Current
 * Occured -> Occurred
 * Sequece -> Sequence
 * millliseconds -> milliseconds
 * unkown -> unknown
 * soure -> source
 * Unkown -> Unknown
 *
 * Revision 1.1.4.1  1996/05/17  11:41:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:43  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:41  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:40  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:30  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../rtt/msg_rtt.c"		/* Not a typo, see logger.h */
#include "rtt_lib.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "rtt_dump.h"

/*
 * The following are the definitions for the overall
 *   tables in the mib.
 */
static rttMonAppl_t rttMonApplEntry;
static rttMonApplSupportedRttTypesEntry_t
    rttMonApplSupportedRttTypesTable[MAX_RTT_TYPES];
static rttMonApplSupportedProtocolsEntry_t
    rttMonApplSupportedProtocolsTable[MAX_RTT_PROTOCOLS];
static boolean rttMonApplSupportedProtocolsTablePopulated;

#define TMPBUFF_SIZE 80
static char *tmpOutBuff;

static void updateRttMonApplTimeOfLastSet (unsigned long currentTime)
{
    rttMonApplEntry.rttMonApplTimeOfLastSet =
	currentTime;
}				

/* 
 * NOTE:
 *   All locking must be done at the adminQ entry level.
 */

/*
 * Starting point for whole DataBase
 */

static rttMonCtrlAdminQHead baseRttMonCtrlAdminQHead;

/* 
 *  Global Semahore used by all library access to the MIB DB 
 */

static watched_semaphore *rttMonAccessSemaphore;

static const char *const rtt_named_malloc_table[] =
{
    "RTR Entry 0",
    "RTR Entry 1",
    "RTR Entry 2",
    "RTR Entry 3",
    "RTR Entry 4",
    "RTR Entry 5",
    "RTR Entry 6",
    "RTR Entry 7",
    "RTR Entry 8",
    "RTR Entry 9",
    "RTR Entry 10",
    "RTR Entry 11",
    "RTR Entry 12",
    "RTR Entry 13",
    "RTR Entry 14",
    "RTR Entry 15",
    "RTR Entry 16",
    "RTR Entry 17",
    "RTR Entry 18",
    "RTR Entry 19",
    "RTR Entry 20",
    "RTR Entry 21",
    "RTR Entry 22",
    "RTR Entry 23",
    "RTR Entry 24",
    "RTR Entry 25",
    "RTR Entry 26",
    "RTR Entry 27",
    "RTR Entry 28",
    "RTR Entry 29",
    "RTR Entry 30",
    "RTR Entry 31",
    "RTR Entry 32",
    "RTR Entry 33",
    "RTR Entry 34",
    "RTR Entry 35"
    };

const unsigned char rtt_arr_eyecather_str[] =
{
    0x57,				/* W */
    0x78,				/* x */
    0x59,				/* Y */
    0x7A				/* Z */
    };

static const unsigned char rttAsciiToEbcdic[] =
{
    0x40,				/* SP */
    0x5A,				/* ! */
    0x7F,				/* " */
    0x7B,				/* # */
    0x5B,				/* $ */
    0x6C,				/* % */
    0x50,				/* & */
    0x7d,				/* ' */
    0x4D,				/* ( */
    0x5D,				/* ) */
    0x5C,				/* * */
    0x4E,				/* + */
    0x6B,				/* ' */
    0x60,				/* - */
    0x4B,				/* . */
    0x61,				/* / */
    0xF0,				/* 0 */
    0xF1,				/* 1 */
    0xF2,				/* 2 */
    0xF3,				/* 3 */
    0xF4,				/* 4 */
    0xF5,				/* 5 */
    0xF6,				/* 6 */
    0xF7,				/* 7 */
    0xF8,				/* 8 */
    0xF9,				/* 9 */
    0x7A,				/* : */
    0x5E,				/* ; */
    0x4C,				/* < */
    0x7E,				/* = */
    0x6E,				/* > */
    0x6F,				/* ? */
    0x7C,				/* @ */
    0xC1,				/* A */
    0xC2,				/* B */
    0xC3,				/* C */
    0xC4,				/* D */
    0xC5,				/* E */
    0xC6,				/* F */
    0xC7,				/* G */
    0xC8,				/* H */
    0xC9,				/* I */
    0xD1,				/* J */
    0xD2,				/* K */
    0xD3,				/* L */
    0xD4,				/* M */
    0xD5,				/* N */
    0xD6,				/* O */
    0xD7,				/* P */
    0xD8,				/* Q */
    0xD9,				/* R */
    0xE2,				/* S */
    0xE3,				/* T */
    0xE4,				/* U */
    0xE5,				/* V */
    0xE6,				/* W */
    0xE7,				/* X */
    0xE8,				/* Y */
    0xE9,				/* Z */
    0x4D,				/* [ -> ( */
    0xE9,				/* \ */
    0x5D,				/* ] -> ) */
    0x5F,				/* > */
    0x6D,				/* - */
    0x79,				/* */
    0x81,				/* a */
    0x82,				/* b */
    0x83,				/* c */
    0x84,				/* d */
    0x85,				/* e */
    0x86,				/* f */
    0x87,				/* g */
    0x88,				/* h */
    0x89,				/* i */
    0x91,				/* j */
    0x92,				/* k */
    0x93,				/* l */
    0x94,				/* m */
    0x95,				/* n */
    0x96,				/* o */
    0x97,				/* p */
    0x98,				/* q */
    0x99,				/* r */
    0xA2,				/* s */
    0xA3,				/* t */
    0xA4,				/* u */
    0xA5,				/* v */
    0xA6,				/* w */
    0xA7,				/* x */
    0xA8,				/* y */
    0xA9				/* z */
    };



/*
   Generic Libraries
 */
void rttMon_get_boot_epoch (clock_epoch *boot_epoch)
{
  unsigned long currentTime;

  /*
   * Get the current snmp uptime and boot time as close to the
   *   same as possible.  Then convert uptime to seconds and subtract
   *   from current epoch seconds.  Thus we have when the router
   *   booted.  Now the only worry I have is that the two calls
   *   to get time may be a little different between the two and
   *   the results of this can vary a little.  But the way this
   *   is used, I should not compare the results of this with
   *   the results of this exactly or at all.
   */

  currentTime = snmp_sysUpTime();
  clock_get_time(boot_epoch);

  /*
   * currrentTime is in centiseconds eg 500 csec = 5 sec
   */
  boot_epoch->epoch_secs -= currentTime / SYSUPTIME_UNITS;

}


void rttMonConvertAsciiToEbcdic (const unsigned char *ascii,
				 unsigned char *ebcdic,
				 short length)
{
    unsigned char c;

    while (length-- > 0)
	*ebcdic++ = (c = *ascii++) < ' ' ? 0x15 : rttAsciiToEbcdic[c - ' '];

    return;
}			

/*
 * Convert an IP Address (in the form of an unsigned long) to
 * an OctetString.
 */ 

void rttMonIPToOctetString (unsigned long ipAddr,
			    OctetString * OSBuff)
{
    if (OSBuff != NULL) {
	OSBuff->octet_ptr[0] = (ipAddr >> 24) & 0xff;
	OSBuff->octet_ptr[1] = (ipAddr >> 16) & 0xff;
	OSBuff->octet_ptr[2] = (ipAddr >> 8) & 0xff;
	OSBuff->octet_ptr[3] = (ipAddr >> 0) & 0xff;

	OSBuff->length = 4;
    }
    return;
} 

void rttMonFreeOctetStringBuffer (OctetString * tmpAddressBuff)
{
    if (tmpAddressBuff != NULL) {
	if (tmpAddressBuff->octet_ptr != NULL) {
	    free(tmpAddressBuff->octet_ptr);
	}
	free(tmpAddressBuff);
    }
    return;
} 

static void updateOctetsInUse (rttMonCtrlAdminEntry_t
			       * ctrlAdminEntry,
			       int size)
{
    ctrlAdminEntry->rttMonCtrlOperOctetsInUse +=
	size;
    return;
} 

static void *rttMalloc (long rttMonCtrlAdminIndex,
			unsigned int size)
{
    void *return_ptr;
    long tmpIndex;

    /* Index 35 is the garbage can */
    if (rttMonCtrlAdminIndex > 35) {
	tmpIndex = 35;
    } else {
	tmpIndex = rttMonCtrlAdminIndex;
    }			
    /* 
     *  The following passes lint for systems
     *  that do not use the second parameter
     *  to malloc_named.
     */ 
    tmpIndex = tmpIndex;

    return_ptr =
	malloc_named(size,
		     rtt_named_malloc_table[tmpIndex]);

    return (return_ptr);
} 

static OctetString *preMallocOctetString (long rttMonCtrlAdminIndex,
					  long length)
{
    OctetString *os_ptr;

    if (length > MAX_OCTET_STRING_SIZE) {
	sprintf(tmpOutBuff,
		"Bad octetstring length: %d",
		length);
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      tmpOutBuff);
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in preMallocOctetString");
	return (NULL);
    }
    if ((os_ptr = rttMalloc(rttMonCtrlAdminIndex,
			    sizeof(OctetString))) == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "Failed os_ptr malloc");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in preMallocOctetString");
	return (os_ptr);
    }
    os_ptr->octet_ptr = NULL;

    if ((os_ptr->octet_ptr = rttMalloc(rttMonCtrlAdminIndex,
				       MAX(length, 1))) == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "Failed octet_ptr malloc");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in preMallocOctetString");
	rttMonFreeOctetStringBuffer(os_ptr);
	os_ptr = NULL;
	return (os_ptr);
    }
    memset(os_ptr->octet_ptr, 0, length);
    os_ptr->length = length;

    return (os_ptr);

} 

OctetString *rttMonPreMallocOctetString (long length)
{
    return (preMallocOctetString(0, length));
} 

void populateOctetString (long rttMonCtrlAdminIndex,
			  OctetString * os_str,
			  unsigned char *string,
			  long length)
{
    if (os_str == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "os_str == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in populateOctetString");
	return;
    }
    if (os_str->octet_ptr == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "os_str->octet_ptr == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in populateOctetString");

	return;
    }
    memcpy(os_str->octet_ptr, string, length);

    os_str->length = length;

    return;

} 

void appendOctetString (long rttMonCtrlAdminIndex,
			OctetString * os_str,
			unsigned char *string,
			long length)
{
    if (os_str == NULL) {

	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "os_str == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in appendOctetString");
	return;
    }
    if (os_str->octet_ptr == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "os_str->octet_ptr == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in appendOctetString");
	return;
    }
    memcpy(&os_str->octet_ptr[os_str->length],
	   string, length);

    os_str->length += length;

    return;

} 

static void cloneToPreAllocatedOctetString (long rttMonCtrlAdminIndex,
					    OctetString * target_os_str,
					    OctetString * orig_os_str)
{
    if (orig_os_str == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "orig_os_str == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in cloneToPreAllocatedOctetString");
	return;
    }
    if (target_os_str == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "target_os_str == NULL");
	RTT_BUGINF2(rttMonCtrlAdminIndex,
		    "\t in cloneToPreAllocatedOctetString");
	return;
    }
    memcpy(target_os_str->octet_ptr, orig_os_str->octet_ptr,
	   orig_os_str->length);

    target_os_str->length = orig_os_str->length;

    return;

} 

static void scrubOctetString (OctetString * inOctetString)
{
    if (inOctetString == NULL) {
	return;
    }
    memset(inOctetString->octet_ptr, 0,
	   inOctetString->length);

    return;
}			

/*
 * RttCmpOS: Compares two Octet Strings.
 *
 * Returns:   0 if contents of ptr1 have the same length and values
 *              as the contents of ptr2,
 *          < 0 if contents of ptr1 are less than (or smaller) than
 *              the contents of ptr2,
 *          > 0 otherwise.
 *
 */ 

static int RttCmpOS (OctetString * ptr1,
		     OctetString * ptr2)
{
    int i;
    int min;

    min = (int)
	((ptr1->length < ptr2->length) ? ptr1->length : ptr2->length);

    /* check for mismatched values */
    for (i = 0; i < min; i++) {
	/*
	 * First check if not equal.  This prevents two 
	 * comparisons on each interation.
	 */
	if (ptr1->octet_ptr[i] != ptr2->octet_ptr[i]) {
	    if (ptr1->octet_ptr[i] > ptr2->octet_ptr[i]) {
		return (1);
	    }
	    if (ptr1->octet_ptr[i] < ptr2->octet_ptr[i]) {
		return (-1);
	    }
	}
    }
    /*
     * equal for as long as the shorter one holds out.  
     *  The longer should be considered bigger
     */ 
    if (ptr1->length > ptr2->length)
	return (1);
    if (ptr1->length < ptr2->length)
	return (-1);
    /* they are equal for their entire mutual lengths */
    return (0);
} 

static long getMaxRttMonAddressSize (long rttMonEchoAdminProtocol)
{
    long return_size;

    /*
     *  Return the size of memory that is needed to store each
     *   protocol, depending on the protocol type.
     */

    switch (rttMonEchoAdminProtocol) {
    case D_rttMonApplSupportedProtocols_notApplicable:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_ipIcmpEcho:
	return_size = MAX_rttMonTargetAddress_ipIcmpEcho;
	break;
    case D_rttMonApplSupportedProtocols_ipUdpEchoAppl:
	return_size = MAX_rttMonTargetAddress_ipUdpEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_snaRUEcho:
	return_size = MAX_rttMonTargetAddress_snaRUEcho;
	break;
    case D_rttMonApplSupportedProtocols_snaLU0EchoAppl:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_snaLU2EchoAppl:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_snaLU62Echo:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_snaLU62EchoAppl:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_appleTalkEcho:
	return_size = MAX_rttMonTargetAddress_appleTalkEcho;
	break;
    case D_rttMonApplSupportedProtocols_appleTalkEchoAppl:
	return_size = MAX_rttMonTargetAddress_appleTalkEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_decNetEcho:
	return_size = MAX_rttMonTargetAddress_decNetEcho;
	break;
    case D_rttMonApplSupportedProtocols_decNetEchoAppl:
	return_size = MAX_rttMonTargetAddress_decNetEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_ipxEcho:
	return_size = MAX_rttMonTargetAddress_ipxEcho;
	break;
    case D_rttMonApplSupportedProtocols_ipxEchoAppl:
	return_size = MAX_rttMonTargetAddress_ipxEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_isoClnsEcho:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_isoClnsEchoAppl:
	return_size = MAX_rttMonTargetAddress;
	break;
    case D_rttMonApplSupportedProtocols_vinesEcho:
	return_size = MAX_rttMonTargetAddress_vinesEcho;
	break;
    case D_rttMonApplSupportedProtocols_vinesEchoAppl:
	return_size = MAX_rttMonTargetAddress_vinesEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_xnsEcho:
	return_size = MAX_rttMonTargetAddress_xnsEcho;
	break;
    case D_rttMonApplSupportedProtocols_xnsEchoAppl:
	return_size = MAX_rttMonTargetAddress_xnsEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_apolloEcho:
	return_size = MAX_rttMonTargetAddress_apolloEcho;
	break;
    case D_rttMonApplSupportedProtocols_apolloEchoAppl:
	return_size = MAX_rttMonTargetAddress_apolloEchoAppl;
	break;
    case D_rttMonApplSupportedProtocols_netbiosEchoAppl:
	return_size = MAX_rttMonTargetAddress_netbiosEchoAppl;
	break;
    default:
	return_size = MAX_rttMonTargetAddress;
    } 
    return (return_size);
} 

static void RTT_Increment (long *incounter,
			   long maxVal)
{
    if ((*incounter) >= maxVal) {
	return;
    }
    (*incounter)++;

    return;

}


/*
 *  rttMonReactTriggerAdminQ
 */ 

static void freeRttMonReactTriggerAdminQItem (long rttMonCtrlAdminIndex,
					      rttMonReactTriggerAdminQItem
					      * rttMonReactTriggerAdminQItem)
{
    if (rttMonReactTriggerAdminQItem == NULL) {
	return;
    }
    if (rttMonReactTriggerAdminQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonReactTriggerAdminQItem->next not NULL");
    }
    if (rttMonReactTriggerAdminQItem->reactTriggerAdminEntry != NULL) {
	free(rttMonReactTriggerAdminQItem->reactTriggerAdminEntry);
    }
    free(rttMonReactTriggerAdminQItem);

    return;

} 

static void freeRttMonReactTriggerAdminQ (rttMonReactTriggerAdminQHead
					  * inRttMonReactTriggerAdminQHead)
{
    rttMonReactTriggerAdminQItem *tmpTriggerAdminQItem;
    rttMonReactTriggerAdminQItem *tmpTriggerAdminQItemNext;

    if (inRttMonReactTriggerAdminQHead == NULL) {
	return;
    }
    for (tmpTriggerAdminQItem =
	 inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ.qhead;
	 tmpTriggerAdminQItem != NULL;
	 tmpTriggerAdminQItem = tmpTriggerAdminQItemNext) {
	tmpTriggerAdminQItemNext = tmpTriggerAdminQItem->next;
	unqueue(&(inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ),
		tmpTriggerAdminQItem);
	tmpTriggerAdminQItem->next = NULL;
	if ((inRttMonReactTriggerAdminQHead->
	     lastSearchedReactTriggerAdminQItem != NULL) &&
	    (inRttMonReactTriggerAdminQHead->
	     lastSearchedReactTriggerAdminQItem ==
	     tmpTriggerAdminQItem)) {
	    inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem = NULL;
	}
	freeRttMonReactTriggerAdminQItem(tmpTriggerAdminQItem->
					 reactTriggerAdminEntry->
					 rttMonCtrlAdminIndex,
					 tmpTriggerAdminQItem);
    } 
    free(inRttMonReactTriggerAdminQHead);

    return;
} 

static rttMonReactTriggerAdminQItem *newRttMonReactTriggerAdminQItem (long rttMonCtrlAdminIndex,
								      long
								      *rttMallocOctetsAllocated)
{
    rttMonReactTriggerAdminQItem *newRttMonReactTriggerAdminQItem;

    newRttMonReactTriggerAdminQItem =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(rttMonReactTriggerAdminQItem));
    if (newRttMonReactTriggerAdminQItem == NULL) {
	return (newRttMonReactTriggerAdminQItem);
    }
    memset(newRttMonReactTriggerAdminQItem, 0,
	   sizeof(rttMonReactTriggerAdminQItem));

    newRttMonReactTriggerAdminQItem->reactTriggerAdminEntry =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(rttMonReactTriggerAdminEntry_t));
    if (newRttMonReactTriggerAdminQItem->reactTriggerAdminEntry == NULL) {
	freeRttMonReactTriggerAdminQItem(rttMonCtrlAdminIndex,
					 newRttMonReactTriggerAdminQItem);
	return (NULL);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonReactTriggerAdminQItem);
    *rttMallocOctetsAllocated +=
	sizeof(rttMonReactTriggerAdminEntry_t);

    return (newRttMonReactTriggerAdminQItem);

} 

static rttMonReactTriggerAdminQHead *newRttMonReactTriggerAdminQ (long rttMonCtrlAdminIndex,
								  long
								  *rttMallocOctetsAllocated)
{
    rttMonReactTriggerAdminQHead *returnQHead;
    rttMonReactTriggerAdminQItem *newTriggerAdminQItem;
    long i;
    long initialSize;

    returnQHead = rttMalloc(rttMonCtrlAdminIndex,
			    sizeof(rttMonReactTriggerAdminQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0, sizeof(rttMonReactTriggerAdminQHead));

    queue_init(&(returnQHead->rttMonReactTriggerAdminQ), 0);

    /*
     * The Maximum number of entries in each Trigger Q is
     *  the Maximum number of entries that can be pointed
     *  toward.  Thus for each Trigger Q, you can have 
     *  SYS_MAX_rttMonApplNumCtrlAdminEntry entries that
     *  are pointed to.
     */
    initialSize = SYS_MAX_rttMonApplNumCtrlAdminEntry;

    for (i = 0; i < initialSize; i++) {
	newTriggerAdminQItem =
	    newRttMonReactTriggerAdminQItem(rttMonCtrlAdminIndex,
					    rttMallocOctetsAllocated);
	if (newTriggerAdminQItem == NULL) {
	    freeRttMonReactTriggerAdminQ(returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	enqueue(&(returnQHead->rttMonReactTriggerAdminQ),
		newTriggerAdminQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonReactTriggerAdminQHead);

    return (returnQHead);

} 

static rttMonReactTriggerAdminQItem
*getUnusedRttMonReactTriggerAdminQItem (rttMonReactTriggerAdminQHead
					* inRttMonReactTriggerAdminQHead)
{
    rttMonReactTriggerAdminQItem *returnTriggerAdminQItem;

    for (returnTriggerAdminQItem =
	 inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ.qhead;
	 returnTriggerAdminQItem != NULL;
	 returnTriggerAdminQItem = returnTriggerAdminQItem->next) {
	if (!VALID(I_queue_item_in_use, returnTriggerAdminQItem->valid)) {
	    return (returnTriggerAdminQItem);
	}
    } 
    return (NULL);
} 

static rttMonReactTriggerAdminQItem *findRttMonReactTriggerAdminQItem (rttMonReactTriggerAdminQHead
								       * inRttMonReactTriggerAdminQHead,
								       long
								       rttMonReactTriggerAdminRttMonCtrlAdminIndex)
{
    rttMonReactTriggerAdminQItem *returnTriggerAdminQItem, *startTriggerAdminQItem;
    boolean found = FALSE;

    if (inRttMonReactTriggerAdminQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonReactTriggerAdminQHead->
	 lastSearchedReactTriggerAdminQItem != NULL) &&
	(inRttMonReactTriggerAdminQHead->
	 lastSearchedReactTriggerAdminQItem->
	 reactTriggerAdminEntry->
	 rttMonReactTriggerAdminRttMonCtrlAdminIndex ==
	 rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	return (inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem);
    } else if ((inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem != NULL) &&
	       (inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		rttMonReactTriggerAdminRttMonCtrlAdminIndex <
		rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	startTriggerAdminQItem =
	    inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem;
    } else {
	startTriggerAdminQItem =
	    inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ.qhead;
    } 
    for (returnTriggerAdminQItem = startTriggerAdminQItem;
	 returnTriggerAdminQItem != NULL;
	 returnTriggerAdminQItem = returnTriggerAdminQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnTriggerAdminQItem->valid))
	    && (returnTriggerAdminQItem->reactTriggerAdminEntry->
		rttMonReactTriggerAdminRttMonCtrlAdminIndex ==
		rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonReactTriggerAdminQHead->
	    lastSearchedReactTriggerAdminQItem = returnTriggerAdminQItem;
	return (returnTriggerAdminQItem);
    } else {
	return (NULL);
    }
} 

static rttMonReactTriggerAdminQItem
*findNextRttMonReactTriggerAdminQItem (rttMonReactTriggerAdminQHead
				       * inRttMonReactTriggerAdminQHead,
				       long
				       rttMonReactTriggerAdminRttMonCtrlAdminIndex)
{
    rttMonReactTriggerAdminQItem *returnTriggerAdminQItem, *startTriggerAdminQItem;
    boolean found = FALSE;

    if (inRttMonReactTriggerAdminQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonReactTriggerAdminQHead->
	 lastSearchedReactTriggerAdminQItem != NULL) &&
	(inRttMonReactTriggerAdminQHead->
	 lastSearchedReactTriggerAdminQItem->
	 reactTriggerAdminEntry->
	 rttMonReactTriggerAdminRttMonCtrlAdminIndex ==
	 rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	return (inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem);
    } else if ((inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem != NULL) &&
	       (inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		rttMonReactTriggerAdminRttMonCtrlAdminIndex <
		rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	startTriggerAdminQItem =
	    inRttMonReactTriggerAdminQHead->
		lastSearchedReactTriggerAdminQItem;
    } else {
	startTriggerAdminQItem =
	    inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ.qhead;
    } 
    for (returnTriggerAdminQItem = startTriggerAdminQItem;
	 returnTriggerAdminQItem != NULL;
	 returnTriggerAdminQItem = returnTriggerAdminQItem->next) {
	if ((VALID(I_queue_item_in_use, returnTriggerAdminQItem->valid))
	    && (returnTriggerAdminQItem->reactTriggerAdminEntry->
		rttMonReactTriggerAdminRttMonCtrlAdminIndex >=
		rttMonReactTriggerAdminRttMonCtrlAdminIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonReactTriggerAdminQHead->
	    lastSearchedReactTriggerAdminQItem =
		returnTriggerAdminQItem;
	return (returnTriggerAdminQItem);
    } else {
	return (NULL);
    }
} 

static int updateRttMonReactTriggerAdminQItem (rttMonReactTriggerAdminQHead
					       * inRttMonReactTriggerAdminQHead,
					       rttMonReactTriggerAdminQItem
					       * inRttMonReactTriggerAdminQItem)
{
    long tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex;
    rttMonReactTriggerAdminQItem *prevItem;
    rttMonReactTriggerAdminQItem *currItem;

    unqueue(&(inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ),
	    inRttMonReactTriggerAdminQItem);
    tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex =
	inRttMonReactTriggerAdminQItem->reactTriggerAdminEntry->
	    rttMonReactTriggerAdminRttMonCtrlAdminIndex;
    prevItem = NULL;
    currItem = inRttMonReactTriggerAdminQHead->
	rttMonReactTriggerAdminQ.qhead;
    while (currItem != NULL) {
	if (tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex <
	    currItem->reactTriggerAdminEntry->
	    rttMonReactTriggerAdminRttMonCtrlAdminIndex) {
	    break;
	}
	prevItem = currItem;
	currItem = currItem->next;
    } 
    insqueue(&(inRttMonReactTriggerAdminQHead->rttMonReactTriggerAdminQ),
	     inRttMonReactTriggerAdminQItem, prevItem);

    SET_VALID(I_queue_item_in_use,
	      inRttMonReactTriggerAdminQItem->valid);

    return (D_Return_Successfull);

}


/*
 * rttMonStatsCaptureDistQ
 */ 

static void freeRttMonStatsCaptureDistQItem (long rttMonCtrlAdminIndex,
					     rttMonStatsCaptureDistQItem
					     * inRttMonStatsCaptureDistQItem)
{
    if (inRttMonStatsCaptureDistQItem == NULL) {
	return;
    }
    if (inRttMonStatsCaptureDistQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonStatsCaptureDistQItem->next not NULL");
    }
    if (inRttMonStatsCaptureDistQItem->shortStatsCaptureEntry != NULL) {
	free(inRttMonStatsCaptureDistQItem->shortStatsCaptureEntry);
    }
    free(inRttMonStatsCaptureDistQItem);

    return;

} 

static void scrubRttMonStatsCaptureDistQItem (rttMonStatsCaptureDistQItem
					      * inRttMonStatsCaptureDistQItem)
{
    long tmpRttMonStatsCaptureDistIndex;

    if (inRttMonStatsCaptureDistQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonStatsCaptureDistQItem->
	       valid);
    inRttMonStatsCaptureDistQItem->
	accumulation = 0;
    tmpRttMonStatsCaptureDistIndex =
	inRttMonStatsCaptureDistQItem->
	    shortStatsCaptureEntry->
		rttMonStatsCaptureDistIndex;
    memset(inRttMonStatsCaptureDistQItem->
	   shortStatsCaptureEntry, 0,
	   sizeof(short_rttMonStatsCaptureEntry_t));
    inRttMonStatsCaptureDistQItem->
	shortStatsCaptureEntry->
	    rttMonStatsCaptureDistIndex =
		tmpRttMonStatsCaptureDistIndex;
    inRttMonStatsCaptureDistQItem->
      shortStatsCaptureEntry->
	rttMonStatsCaptureCompletionTimeMin = MAXINT;

    return;

} 

static void freeRttMonStatsCaptureDistQ (long rttMonCtrlAdminIndex,
					 rttMonStatsCaptureDistQHead
					 * inRttMonStatsCaptureDistQHead)
{
    rttMonStatsCaptureDistQItem *tmpDistQItem, *tmpDistQItemNext;

    if (inRttMonStatsCaptureDistQHead == NULL) {
	return;
    }
    for (tmpDistQItem = inRttMonStatsCaptureDistQHead->
	 rttMonStatsCaptureDistQ.qhead;
	 tmpDistQItem != NULL;
	 tmpDistQItem = tmpDistQItemNext) {
	tmpDistQItemNext = tmpDistQItem->next;
	unqueue(&(inRttMonStatsCaptureDistQHead->
		  rttMonStatsCaptureDistQ), tmpDistQItem);
	tmpDistQItem->next = NULL;
	if ((inRttMonStatsCaptureDistQHead->
	     lastSearchedStatsCaptureDistQItem != NULL) &&
	    (inRttMonStatsCaptureDistQHead->
	     lastSearchedStatsCaptureDistQItem ==
	     tmpDistQItem)) {
	    inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem = NULL;
	}
	freeRttMonStatsCaptureDistQItem(rttMonCtrlAdminIndex,
					tmpDistQItem);
    } 
    free(inRttMonStatsCaptureDistQHead);

    return;
} 

static void scrubRttMonStatsCaptureDistQ (rttMonStatsCaptureDistQHead
					  * inRttMonStatsCaptureDistQHead)
{
    rttMonStatsCaptureDistQItem *tmpDistQItem;

    if (inRttMonStatsCaptureDistQHead == NULL) {
	return;
    }
    inRttMonStatsCaptureDistQHead->
	lastSearchedStatsCaptureDistQItem = NULL;

    for (tmpDistQItem = inRttMonStatsCaptureDistQHead->
	 rttMonStatsCaptureDistQ.qhead;
	 tmpDistQItem != NULL;
	 tmpDistQItem = tmpDistQItem->next) {
	scrubRttMonStatsCaptureDistQItem(tmpDistQItem);
    } 
    return;
} 

static rttMonStatsCaptureDistQItem *newRttMonStatsCaptureDistQItem (long rttMonCtrlAdminIndex,
								    long
								    *rttMallocOctetsAllocated)
{
    rttMonStatsCaptureDistQItem *newRttMonStatsCaptureDistQItem;

    newRttMonStatsCaptureDistQItem =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureDistQItem));
    if (newRttMonStatsCaptureDistQItem == NULL) {
	return (newRttMonStatsCaptureDistQItem);
    }
    memset(newRttMonStatsCaptureDistQItem, 0,
	   sizeof(rttMonStatsCaptureDistQItem));

    newRttMonStatsCaptureDistQItem->shortStatsCaptureEntry =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(short_rttMonStatsCaptureEntry_t));
    if (newRttMonStatsCaptureDistQItem->shortStatsCaptureEntry == NULL) {
	freeRttMonStatsCaptureDistQItem(rttMonCtrlAdminIndex,
					newRttMonStatsCaptureDistQItem);
	return (newRttMonStatsCaptureDistQItem);
    }
    memset(newRttMonStatsCaptureDistQItem->shortStatsCaptureEntry,
	   0, sizeof(short_rttMonStatsCaptureEntry_t));

    newRttMonStatsCaptureDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureCompletionTimeMin = MAXINT;

    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureDistQItem);
    *rttMallocOctetsAllocated +=
	sizeof(short_rttMonStatsCaptureEntry_t);

    return (newRttMonStatsCaptureDistQItem);
} 

static rttMonStatsCaptureDistQHead *newRttMonStatsCaptureDistQ (rttMonCtrlAdminEntry_t
								* ctrlAdminEntry,
								long
								*rttMallocOctetsAllocated)
{
    rttMonStatsCaptureDistQHead *returnQHead;
    rttMonStatsCaptureDistQItem *newDistQItem;
    long i;
    long initialSize;

    returnQHead = 
	rttMalloc(ctrlAdminEntry->
		  rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureDistQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonStatsCaptureDistQHead));

    queue_init(&(returnQHead->rttMonStatsCaptureDistQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonStatisticsAdminNumDistBuckets;

    for (i = 0; i < initialSize; i++) {
	newDistQItem = 
	    newRttMonStatsCaptureDistQItem(ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   rttMallocOctetsAllocated);
	if (newDistQItem == NULL) {
	    freeRttMonStatsCaptureDistQ(ctrlAdminEntry->
					rttMonCtrlAdminIndex,
					returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newDistQItem->shortStatsCaptureEntry->
	    rttMonStatsCaptureDistIndex = i + 1;

	enqueue(&(returnQHead->rttMonStatsCaptureDistQ),
		newDistQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureDistQHead);

    return (returnQHead);

} 

static rttMonStatsCaptureDistQItem *findRttMonStatsCaptureDistQItem (rttMonStatsCaptureDistQHead
								     * inRttMonStatsCaptureDistQHead,
								     long rttMonStatsCaptureDistIndex)
{
    rttMonStatsCaptureDistQItem *returnDistQItem, *startDistQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureDistQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCaptureDistQHead->
	 lastSearchedStatsCaptureDistQItem != NULL) &&
	(inRttMonStatsCaptureDistQHead->
	 lastSearchedStatsCaptureDistQItem->
	 shortStatsCaptureEntry->
	 rttMonStatsCaptureDistIndex ==
	 rttMonStatsCaptureDistIndex)) {
	return (inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem);
    } else if ((inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem != NULL) &&
	       (inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem->
		shortStatsCaptureEntry->
		rttMonStatsCaptureDistIndex <
		rttMonStatsCaptureDistIndex)) {
	startDistQItem =
	    inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem;
    } else {
	startDistQItem =
	    inRttMonStatsCaptureDistQHead->
		rttMonStatsCaptureDistQ.qhead;
    } 
    for (returnDistQItem = startDistQItem;
	 returnDistQItem != NULL;
	 returnDistQItem = returnDistQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnDistQItem->valid))
	    && (returnDistQItem->shortStatsCaptureEntry->
		rttMonStatsCaptureDistIndex ==
		rttMonStatsCaptureDistIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureDistQHead->
	    lastSearchedStatsCaptureDistQItem =
		returnDistQItem;
	return (returnDistQItem);
    } else {
	return (NULL);
    }
} 

static rttMonStatsCaptureDistQItem
*findNextRttMonStatsCaptureDistQItem (rttMonStatsCaptureDistQHead
				      * inRttMonStatsCaptureDistQHead,
				      long rttMonStatsCaptureDistIndex)
{
    rttMonStatsCaptureDistQItem *returnDistQItem, *startDistQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureDistQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCaptureDistQHead->
	 lastSearchedStatsCaptureDistQItem != NULL) &&
	(inRttMonStatsCaptureDistQHead->
	 lastSearchedStatsCaptureDistQItem->
	 shortStatsCaptureEntry->
	 rttMonStatsCaptureDistIndex ==
	 rttMonStatsCaptureDistIndex)) {
	return (inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem);
    } else if ((inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem != NULL) &&
	       (inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem->
		shortStatsCaptureEntry->
		rttMonStatsCaptureDistIndex <
		rttMonStatsCaptureDistIndex)) {
	startDistQItem =
	    inRttMonStatsCaptureDistQHead->
		lastSearchedStatsCaptureDistQItem;
    } else {
	startDistQItem =
	    inRttMonStatsCaptureDistQHead->
		rttMonStatsCaptureDistQ.qhead;
    } 
    for (returnDistQItem = startDistQItem;
	 returnDistQItem != NULL;
	 returnDistQItem = returnDistQItem->next) {
	if ((VALID(I_queue_item_in_use, returnDistQItem->valid))
	    && (returnDistQItem->shortStatsCaptureEntry->
		rttMonStatsCaptureDistIndex >=
		rttMonStatsCaptureDistIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureDistQHead->
	    lastSearchedStatsCaptureDistQItem =
		returnDistQItem;
	return (returnDistQItem);
    } else {
	return (NULL);
    }
}



/*
 * rttMonStatsCollectEntry
 */ 

void freeRttMonStatsCollectEntry (rttMonStatsCollectEntry_t
				  * rttMonStatsCollectEntry)
{
    if (rttMonStatsCollectEntry == NULL) {
	return;
    }
    if (rttMonStatsCollectEntry->rttMonStatsCollectAddress !=
	NULL) {
	rttMonFreeOctetStringBuffer(rttMonStatsCollectEntry->
				    rttMonStatsCollectAddress);
    }
    free(rttMonStatsCollectEntry);

    return;

} 

static void freeShortRttMonStatsCollectEntry (short_rttMonStatsCollectEntry_t
					      * shortRttMonStatsCollectEntry)
{
    if (shortRttMonStatsCollectEntry == NULL) {
	return;
    }
    if (shortRttMonStatsCollectEntry->
	rttMonStatsCollectAddressShared == FALSE) {
	if (shortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddress !=
	    NULL) {
	    rttMonFreeOctetStringBuffer(shortRttMonStatsCollectEntry->
					rttMonStatsCollectAddress);
	}
    }
    free(shortRttMonStatsCollectEntry);

    return;

} 

static void scrubShortRttMonStatsCollectEntry (short_rttMonStatsCollectEntry_t
					       * shortRttMonStatsCollectEntry)
{
    OctetString *tmpRttMonStatsCollectAddress;
    boolean tmpRttMonStatsCollectAddressShared;

    if (shortRttMonStatsCollectEntry == NULL) {
	return;
    }
    tmpRttMonStatsCollectAddressShared =
	shortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddressShared;
    tmpRttMonStatsCollectAddress =
	shortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddress;
    memset(shortRttMonStatsCollectEntry, 0,
	   sizeof(short_rttMonStatsCollectEntry_t));
    shortRttMonStatsCollectEntry->
	rttMonStatsCollectAddress =
	    tmpRttMonStatsCollectAddress;
    shortRttMonStatsCollectEntry->
	rttMonStatsCollectAddressShared =
	    tmpRttMonStatsCollectAddressShared;

    if (shortRttMonStatsCollectEntry->
	rttMonStatsCollectAddressShared == FALSE) {
	scrubOctetString(shortRttMonStatsCollectEntry->
			 rttMonStatsCollectAddress);
    }
    return;

}


/*
 *  rttMonStatsCaptureHopQ
 */ 

static void freeRttMonStatsCaptureHopQItem (long rttMonCtrlAdminIndex,
					    rttMonStatsCaptureHopQItem
					    * inRttMonStatsCaptureHopQItem)
{
    if (inRttMonStatsCaptureHopQItem == NULL) {
	return;
    }
    if (inRttMonStatsCaptureHopQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonStatsCaptureHopQItem->next not NULL");
    }
    if (inRttMonStatsCaptureHopQItem->statsCaptDistQHead != NULL) {
	freeRttMonStatsCaptureDistQ(rttMonCtrlAdminIndex,
				    inRttMonStatsCaptureHopQItem->
				    statsCaptDistQHead);
    }
    if (inRttMonStatsCaptureHopQItem->shortStatsCollectEntry != NULL) {
	freeShortRttMonStatsCollectEntry(inRttMonStatsCaptureHopQItem->
					 shortStatsCollectEntry);
    }
    free(inRttMonStatsCaptureHopQItem);

    return;

} 

static void scrubRttMonStatsCaptureHopQItem (rttMonStatsCaptureHopQItem
					     * inRttMonStatsCaptureHopQItem)
{
    if (inRttMonStatsCaptureHopQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonStatsCaptureHopQItem->
	       valid);

    scrubShortRttMonStatsCollectEntry(inRttMonStatsCaptureHopQItem->
				      shortStatsCollectEntry);

    scrubRttMonStatsCaptureDistQ(inRttMonStatsCaptureHopQItem->
				 statsCaptDistQHead);

    return;

} 

static void freeRttMonStatsCaptureHopQ (long rttMonCtrlAdminIndex,
					rttMonStatsCaptureHopQHead
					* inRttMonStatsCaptureHopQHead)
{
    rttMonStatsCaptureHopQItem *tmpHopQItem, *tmpHopQItemNext;

    if (inRttMonStatsCaptureHopQHead == NULL) {
	return;
    }
    for (tmpHopQItem = inRttMonStatsCaptureHopQHead->
	 rttMonStatsCaptureHopQ.qhead;
	 tmpHopQItem != NULL;
	 tmpHopQItem = tmpHopQItemNext) {
	tmpHopQItemNext = tmpHopQItem->next;
	unqueue(&(inRttMonStatsCaptureHopQHead->
		  rttMonStatsCaptureHopQ), tmpHopQItem);
	tmpHopQItem->next = NULL;
	if ((inRttMonStatsCaptureHopQHead->
	     lastSearchedStatsCaptureHopQItem != NULL) &&
	    (inRttMonStatsCaptureHopQHead->
	     lastSearchedStatsCaptureHopQItem ==
	     tmpHopQItem)) {
	    inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem = NULL;
	}
	freeRttMonStatsCaptureHopQItem(rttMonCtrlAdminIndex,
				       tmpHopQItem);
    } 
    free(inRttMonStatsCaptureHopQHead);

    return;
} 

static void scrubRttMonStatsCaptureHopQ (rttMonStatsCaptureHopQHead
					 * inRttMonStatsCaptureHopQHead)
{
    rttMonStatsCaptureHopQItem *tmpHopQItem;

    if (inRttMonStatsCaptureHopQHead == NULL) {
	return;
    }
    inRttMonStatsCaptureHopQHead->
	lastSearchedStatsCaptureHopQItem = NULL;

    for (tmpHopQItem = inRttMonStatsCaptureHopQHead->
	 rttMonStatsCaptureHopQ.qhead;
	 tmpHopQItem != NULL;
	 tmpHopQItem = tmpHopQItem->next) {
	scrubRttMonStatsCaptureHopQItem(tmpHopQItem);
    } 
    return;
} 

static short_rttMonStatsCollectEntry_t
*newShortRttMonStatsCollectEntry (rttMonCtrlAdminEntry_t
				  * ctrlAdminEntry,
				  rttMonEchoAdminEntry_t
				  * echoAdminEntry,
				  long
				  *rttMallocOctetsAllocated,
				  long
				  protocolType)
{
    short_rttMonStatsCollectEntry_t *newShortRttMonStatsCollectEntry;

    newShortRttMonStatsCollectEntry =
	rttMalloc(ctrlAdminEntry->
		  rttMonCtrlAdminIndex,
		  sizeof(short_rttMonStatsCollectEntry_t));
    if (newShortRttMonStatsCollectEntry == NULL) {
	return (newShortRttMonStatsCollectEntry);
    }
    memset(newShortRttMonStatsCollectEntry, 0,
	   sizeof(short_rttMonStatsCollectEntry_t));

    if (ctrlAdminEntry->rttMonCtrlAdminRttType ==
	D_rttMonCtrlAdminRttType_pathEcho) {
	newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddress =
		preMallocOctetString(ctrlAdminEntry->rttMonCtrlAdminIndex,
				     getMaxRttMonAddressSize(protocolType));
	if (newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddress ==
	    NULL) {
	    freeShortRttMonStatsCollectEntry(newShortRttMonStatsCollectEntry);
	    return (newShortRttMonStatsCollectEntry);
	}
	newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddressShared = FALSE;
    } else {
	newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddress =
		echoAdminEntry->
		    rttMonEchoAdminTargetAddress;
	newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddressShared = TRUE;
    } 
    if (ctrlAdminEntry->rttMonCtrlAdminIndex != 0) {
	*rttMallocOctetsAllocated +=
	    sizeof(short_rttMonStatsCollectEntry_t);
	if (newShortRttMonStatsCollectEntry->
	    rttMonStatsCollectAddressShared == FALSE) {
	    *rttMallocOctetsAllocated +=
		getMaxRttMonAddressSize(protocolType);
	}
    }
    return (newShortRttMonStatsCollectEntry);
} 

static rttMonStatsCaptureHopQItem *newRttMonStatsCaptureHopQItem (rttMonCtrlAdminEntry_t
								  * ctrlAdminEntry,
								  rttMonEchoAdminEntry_t
								  * echoAdminEntry,
								  long
								  *rttMallocOctetsAllocated,
								  long
								  protocolType)
{
    rttMonStatsCaptureHopQItem *newRttMonStatsCaptureHopQItem;

    newRttMonStatsCaptureHopQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureHopQItem));
    if (newRttMonStatsCaptureHopQItem == NULL) {
	return (newRttMonStatsCaptureHopQItem);
    }
    memset(newRttMonStatsCaptureHopQItem, 0,
	   sizeof(rttMonStatsCaptureHopQItem));

    newRttMonStatsCaptureHopQItem->statsCaptDistQHead =
	newRttMonStatsCaptureDistQ(ctrlAdminEntry,
				   rttMallocOctetsAllocated);
    if (newRttMonStatsCaptureHopQItem->statsCaptDistQHead
	== NULL) {
	freeRttMonStatsCaptureHopQItem(ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       newRttMonStatsCaptureHopQItem);
	newRttMonStatsCaptureHopQItem = NULL;
	return (newRttMonStatsCaptureHopQItem);
    }
    newRttMonStatsCaptureHopQItem->shortStatsCollectEntry =
	newShortRttMonStatsCollectEntry(ctrlAdminEntry,
					echoAdminEntry,
					rttMallocOctetsAllocated,
					protocolType);
    if (newRttMonStatsCaptureHopQItem->shortStatsCollectEntry ==
	NULL) {
	freeRttMonStatsCaptureHopQItem(ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       newRttMonStatsCaptureHopQItem);
	newRttMonStatsCaptureHopQItem = NULL;
	return (newRttMonStatsCaptureHopQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureHopQItem);

    return (newRttMonStatsCaptureHopQItem);
} 

static rttMonStatsCaptureHopQHead *newRttMonStatsCaptureHopQ (rttMonCtrlAdminEntry_t
							      * ctrlAdminEntry,
							      rttMonEchoAdminEntry_t
							      * echoAdminEntry,
							      long
							      *rttMallocOctetsAllocated,
							      long
							      protocolType)
{
    rttMonStatsCaptureHopQHead *returnQHead;
    rttMonStatsCaptureHopQItem *newHopQItem;
    long i;
    long initialSize;

    returnQHead = 
	rttMalloc(ctrlAdminEntry->
		  rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureHopQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonStatsCaptureHopQHead));

    queue_init(&(returnQHead->rttMonStatsCaptureHopQ), 0);

    initialSize = ctrlAdminEntry->rttMonStatisticsAdminNumHops;

    for (i = 0; i < initialSize; i++) {
	newHopQItem =
	    newRttMonStatsCaptureHopQItem(ctrlAdminEntry,
					  echoAdminEntry,
					  rttMallocOctetsAllocated,
					  protocolType);
	if (newHopQItem == NULL) {
	    freeRttMonStatsCaptureHopQ(ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newHopQItem->rttMonStatsCaptureHopIndex = i + 1;

	enqueue(&(returnQHead->rttMonStatsCaptureHopQ),
		newHopQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureHopQHead);

    return (returnQHead);

} 

static rttMonStatsCaptureHopQItem
*findRttMonStatsCaptureHopQItemByAddress (rttMonStatsCaptureHopQHead *
					  captureHopQHead,
					  OctetString * rttMonAddress)
{
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    boolean found;

    found = FALSE;
    tmpHopQItem =
	captureHopQHead->rttMonStatsCaptureHopQ.qhead;
    while (!found && (tmpHopQItem != NULL)) {
	if (!VALID(I_queue_item_in_use, tmpHopQItem->valid)) {
	    tmpHopQItem = NULL;
	} else if (RttCmpOS(rttMonAddress,
			    tmpHopQItem->shortStatsCollectEntry->
			    rttMonStatsCollectAddress) == 0) {
	    found = TRUE;
	} else {
	    tmpHopQItem = tmpHopQItem->next;
	}
    } 
    return (tmpHopQItem);
} 

static rttMonStatsCaptureHopQItem *findRttMonStatsCaptureHopQItem (rttMonStatsCaptureHopQHead
								   * inRttMonStatsCaptureHopQHead,
								   long rttMonStatsCaptureHopIndex)
{
    rttMonStatsCaptureHopQItem *returnHopQItem, *startHopQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureHopQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCaptureHopQHead->
	 lastSearchedStatsCaptureHopQItem != NULL) &&
	(inRttMonStatsCaptureHopQHead->
	 lastSearchedStatsCaptureHopQItem->
	 rttMonStatsCaptureHopIndex ==
	 rttMonStatsCaptureHopIndex)) {
	return (inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem);
    } else if ((inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem != NULL) &&
	       (inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem->
		rttMonStatsCaptureHopIndex <
		rttMonStatsCaptureHopIndex)) {
	startHopQItem =
	    inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem;
    } else {
	startHopQItem =
	    inRttMonStatsCaptureHopQHead->
		rttMonStatsCaptureHopQ.qhead;
    } 
    for (returnHopQItem = startHopQItem;
	 returnHopQItem != NULL;
	 returnHopQItem = returnHopQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnHopQItem->valid))
	    && (returnHopQItem->rttMonStatsCaptureHopIndex ==
		rttMonStatsCaptureHopIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureHopQHead->
	    lastSearchedStatsCaptureHopQItem =
		returnHopQItem;
	return (returnHopQItem);
    } else {
	return (NULL);
    }
} 

static rttMonStatsCaptureHopQItem *findNextRttMonStatsCaptureHopQItem (rttMonStatsCaptureHopQHead
								       * inRttMonStatsCaptureHopQHead,
								       long rttMonStatsCaptureHopIndex)
{
    rttMonStatsCaptureHopQItem *returnHopQItem, *startHopQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureHopQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCaptureHopQHead->
	 lastSearchedStatsCaptureHopQItem != NULL) &&
	(inRttMonStatsCaptureHopQHead->
	 lastSearchedStatsCaptureHopQItem->
	 rttMonStatsCaptureHopIndex ==
	 rttMonStatsCaptureHopIndex)) {
	return (inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem);
    } else if ((inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem != NULL) &&
	       (inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem->
		rttMonStatsCaptureHopIndex <
		rttMonStatsCaptureHopIndex)) {
	startHopQItem =
	    inRttMonStatsCaptureHopQHead->
		lastSearchedStatsCaptureHopQItem;
    } else {
	startHopQItem =
	    inRttMonStatsCaptureHopQHead->
		rttMonStatsCaptureHopQ.qhead;
    } 
    for (returnHopQItem = startHopQItem;
	 returnHopQItem != NULL;
	 returnHopQItem = returnHopQItem->next) {
	if ((VALID(I_queue_item_in_use, returnHopQItem->valid))
	    && (returnHopQItem->rttMonStatsCaptureHopIndex >=
		rttMonStatsCaptureHopIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureHopQHead->
	    lastSearchedStatsCaptureHopQItem =
		returnHopQItem;
	return (returnHopQItem);
    } else {
	return (NULL);
    }
}


/*
 *  Path List
 */ 

static void freePathList (pathList * inPathList)
{
    pathList *tmpPathListItem, *tmpPathListItemNext;

    if (inPathList == NULL) {
	return;
    }
    for (tmpPathListItem = inPathList;
	 tmpPathListItem != NULL;
	 tmpPathListItem = tmpPathListItemNext) {
	tmpPathListItemNext = tmpPathListItem->next;
	if (tmpPathListItem->hop != NULL) {
	    rttMonFreeOctetStringBuffer(tmpPathListItem->hop);
	}
	free(tmpPathListItem);
    } 
    return;

} 

static pathList *newRttMonPathListItem (rttMonCtrlAdminEntry_t
					* ctrlAdminEntry,
					long
					*rttMallocOctetsAllocated,
					long
					protocolType)
{
    pathList *newPathListItem;

    newPathListItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(pathList));
    if (newPathListItem == NULL) {
	return (newPathListItem);
    }
    memset(newPathListItem, 0,
	   sizeof(pathList));

    newPathListItem->hop =
	preMallocOctetString(ctrlAdminEntry->
			     rttMonCtrlAdminIndex,
			     getMaxRttMonAddressSize(protocolType));
    if (newPathListItem->hop == NULL) {
	free(newPathListItem);
	return (NULL);
    }
    *rttMallocOctetsAllocated +=
	sizeof(pathList);
    *rttMallocOctetsAllocated +=
	getMaxRttMonAddressSize(protocolType);

    return (newPathListItem);

}


/*
 * rttMonStatsCapturePathQ
 */ 

static void freeRttMonStatsCapturePathQItem (long rttMonCtrlAdminIndex,
					     rttMonStatsCapturePathQItem
					     * inRttMonStatsCapturePathQItem)
{
    if (inRttMonStatsCapturePathQItem == NULL) {
	return;
    }
    if (inRttMonStatsCapturePathQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonStatsCapturePathQItem->next not NULL");
    }
    if (inRttMonStatsCapturePathQItem->statsCaptureHopQHead !=
	NULL) {
	freeRttMonStatsCaptureHopQ(rttMonCtrlAdminIndex,
				   inRttMonStatsCapturePathQItem->
				   statsCaptureHopQHead);
    }
    free(inRttMonStatsCapturePathQItem);

    return;
} 

static void scrubRttMonStatsCapturePathQItem (rttMonStatsCapturePathQItem
					      * inRttMonStatsCapturePathQItem)
{
    if (inRttMonStatsCapturePathQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonStatsCapturePathQItem->
	       valid);

    scrubRttMonStatsCaptureHopQ(inRttMonStatsCapturePathQItem->
				statsCaptureHopQHead);

    return;
} 

static void freeRttMonStatsCapturePathQ (long rttMonCtrlAdminIndex,
					 rttMonStatsCapturePathQHead
					 * inRttMonStatsCapturePathQHead)
{
    rttMonStatsCapturePathQItem *tmpPathQItem, *tmpPathQItemNext;

    if (inRttMonStatsCapturePathQHead == NULL) {
	return;
    }
    for (tmpPathQItem = inRttMonStatsCapturePathQHead->
	 rttMonStatsCapturePathQ.qhead;
	 tmpPathQItem != NULL;
	 tmpPathQItem = tmpPathQItemNext) {
	tmpPathQItemNext = tmpPathQItem->next;
	unqueue(&(inRttMonStatsCapturePathQHead->
		  rttMonStatsCapturePathQ), tmpPathQItem);
	tmpPathQItem->next = NULL;
	if ((inRttMonStatsCapturePathQHead->
	     lastSearchedStatsCapturePathQItem != NULL) &&
	    (inRttMonStatsCapturePathQHead->
	     lastSearchedStatsCapturePathQItem ==
	     tmpPathQItem)) {
	    inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem = NULL;
	}
	freeRttMonStatsCapturePathQItem(rttMonCtrlAdminIndex,
					tmpPathQItem);
    } 
    free(inRttMonStatsCapturePathQHead);
} 

static void scrubRttMonStatsCapturePathQ (rttMonStatsCapturePathQHead
					  * inRttMonStatsCapturePathQHead)
{
    rttMonStatsCapturePathQItem *tmpPathQItem;

    if (inRttMonStatsCapturePathQHead == NULL) {
	return;
    }
    inRttMonStatsCapturePathQHead->
	rttMonStatsCapturePathQFull = FALSE;
    inRttMonStatsCapturePathQHead->
	currentStatsCapturePathQItem = NULL;
    inRttMonStatsCapturePathQHead->
	lastSearchedStatsCapturePathQItem = NULL;

    for (tmpPathQItem = inRttMonStatsCapturePathQHead->
	 rttMonStatsCapturePathQ.qhead;
	 tmpPathQItem != NULL;
	 tmpPathQItem = tmpPathQItem->next) {
	scrubRttMonStatsCapturePathQItem(tmpPathQItem);
    } 
    return;
} 

static rttMonStatsCapturePathQItem *newRttMonStatsCapturePathQItem (rttMonCtrlAdminEntry_t
								    * ctrlAdminEntry,
								    rttMonEchoAdminEntry_t
								    * echoAdminEntry,
								    long
								    *rttMallocOctetsAllocated,
								    long
								    protocolType)
{
    rttMonStatsCapturePathQItem
	* newRttMonStatsCapturePathQItem;

    newRttMonStatsCapturePathQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCapturePathQItem));
    if (newRttMonStatsCapturePathQItem == NULL) {
	return (newRttMonStatsCapturePathQItem);
    }
    memset(newRttMonStatsCapturePathQItem, 0,
	   sizeof(rttMonStatsCapturePathQItem));

    newRttMonStatsCapturePathQItem->statsCaptureHopQHead =
	newRttMonStatsCaptureHopQ(ctrlAdminEntry,
				  echoAdminEntry,
				  rttMallocOctetsAllocated,
				  protocolType);
    if (newRttMonStatsCapturePathQItem->
	statsCaptureHopQHead == NULL) {
	freeRttMonStatsCapturePathQItem(ctrlAdminEntry->
					rttMonCtrlAdminIndex,
					newRttMonStatsCapturePathQItem);
	newRttMonStatsCapturePathQItem = NULL;
	return (newRttMonStatsCapturePathQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCapturePathQItem);

    return (newRttMonStatsCapturePathQItem);

} 

static rttMonStatsCapturePathQHead *newRttMonStatsCapturePathQ (rttMonCtrlAdminEntry_t
								* ctrlAdminEntry,
								rttMonEchoAdminEntry_t
								* echoAdminEntry,
								long
								*rttMallocOctetsAllocated,
								long
								protocolType)
{
    long initialSize, i;
    rttMonStatsCapturePathQHead *returnQHead;
    rttMonStatsCapturePathQItem *newPathQItem;

    returnQHead = 
	rttMalloc(ctrlAdminEntry->
		  rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCapturePathQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonStatsCapturePathQHead));

    returnQHead->rttMonStatsCapturePathQFull = FALSE;

    queue_init(&(returnQHead->rttMonStatsCapturePathQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonStatisticsAdminNumPaths;

    for (i = 0; i < initialSize; i++) {
	newPathQItem =
	    newRttMonStatsCapturePathQItem(ctrlAdminEntry,
					   echoAdminEntry,
					   rttMallocOctetsAllocated,
					   protocolType);
	if (newPathQItem == NULL) {
	    freeRttMonStatsCapturePathQ(ctrlAdminEntry->
					rttMonCtrlAdminIndex,
					returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newPathQItem->rttMonStatsCapturePathIndex = i + 1;

	enqueue(&(returnQHead->rttMonStatsCapturePathQ),
		newPathQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCapturePathQHead);

    return (returnQHead);

} 

static rttMonStatsCapturePathQItem
*getUnusedRttMonStatsCapturePathQItem (rttMonStatsCapturePathQHead
				       * inRttMonStatsCapturePathQHead)
{
    rttMonStatsCapturePathQItem *returnPathQItem;

    for (returnPathQItem = inRttMonStatsCapturePathQHead->
	 rttMonStatsCapturePathQ.qhead;
	 returnPathQItem != NULL;
	 returnPathQItem = returnPathQItem->next) {
	if (!VALID(I_queue_item_in_use, returnPathQItem->valid)) {
	    return (returnPathQItem);
	}
    } 
    return (NULL);
} 

static rttMonStatsCapturePathQItem *findRttMonStatsCapturePathQItem (rttMonStatsCapturePathQHead
								     * inRttMonStatsCapturePathQHead,
								     long rttMonStatsCapturePathIndex)
{
    rttMonStatsCapturePathQItem *returnPathQItem, *startPathQItem;
    boolean found = FALSE;

    if (inRttMonStatsCapturePathQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCapturePathQHead->
	 lastSearchedStatsCapturePathQItem != NULL) &&
	(inRttMonStatsCapturePathQHead->
	 lastSearchedStatsCapturePathQItem->
	 rttMonStatsCapturePathIndex ==
	 rttMonStatsCapturePathIndex)) {
	return (inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem);
    } else if ((inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem != NULL) &&
	       (inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem->
		rttMonStatsCapturePathIndex <
		rttMonStatsCapturePathIndex)) {
	startPathQItem =
	    inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem;
    } else {
	startPathQItem =
	    inRttMonStatsCapturePathQHead->
		rttMonStatsCapturePathQ.qhead;
    } 
    for (returnPathQItem = startPathQItem;
	 returnPathQItem != NULL;
	 returnPathQItem = returnPathQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnPathQItem->valid))
	    && (returnPathQItem->
		rttMonStatsCapturePathIndex ==
		rttMonStatsCapturePathIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCapturePathQHead->
	    lastSearchedStatsCapturePathQItem =
		returnPathQItem;
	return (returnPathQItem);
    } else {
	return (NULL);
    }
} 

static rttMonStatsCapturePathQItem
*findNextRttMonStatsCapturePathQItem (rttMonStatsCapturePathQHead
				      * inRttMonStatsCapturePathQHead,
				      long rttMonStatsCapturePathIndex)
{
    rttMonStatsCapturePathQItem *returnPathQItem, *startPathQItem;
    boolean found = FALSE;

    if (inRttMonStatsCapturePathQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonStatsCapturePathQHead->
	 lastSearchedStatsCapturePathQItem != NULL) &&
	(inRttMonStatsCapturePathQHead->
	 lastSearchedStatsCapturePathQItem->
	 rttMonStatsCapturePathIndex ==
	 rttMonStatsCapturePathIndex)) {
	return (inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem);
    } else if ((inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem != NULL) &&
	       (inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem->
		rttMonStatsCapturePathIndex <
		rttMonStatsCapturePathIndex)) {
	startPathQItem =
	    inRttMonStatsCapturePathQHead->
		lastSearchedStatsCapturePathQItem;
    } else {
	startPathQItem =
	    inRttMonStatsCapturePathQHead->
		rttMonStatsCapturePathQ.qhead;
    } 
    for (returnPathQItem = startPathQItem;
	 returnPathQItem != NULL;
	 returnPathQItem = returnPathQItem->next) {
	if ((VALID(I_queue_item_in_use, returnPathQItem->valid))
	    && (returnPathQItem->rttMonStatsCapturePathIndex >=
		rttMonStatsCapturePathIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCapturePathQHead->
	    lastSearchedStatsCapturePathQItem =
		returnPathQItem;
	return (returnPathQItem);
    } else {
	return (NULL);
    }
}


/*
 * rttMonStatsCaptureStartTimeQ
 */ 

static void freeRttMonStatsCaptureStartTimeQItem (long rttMonCtrlAdminIndex,
						  rttMonStatsCaptureStartTimeQItem
						  * rttMonStatsCaptureStartTimeQItem)
{
    if (rttMonStatsCaptureStartTimeQItem == NULL) {
	return;
    }
    if (rttMonStatsCaptureStartTimeQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonStatsCaptureStartTimeQItem->next not NULL");
    }
    if (rttMonStatsCaptureStartTimeQItem->statsTotalsEntry !=
	NULL) {
	free(rttMonStatsCaptureStartTimeQItem->statsTotalsEntry);
    }
    if (rttMonStatsCaptureStartTimeQItem->
	statsCapturePathQHead != NULL) {
	freeRttMonStatsCapturePathQ(rttMonCtrlAdminIndex,
				    rttMonStatsCaptureStartTimeQItem->
				    statsCapturePathQHead);
    }
    free(rttMonStatsCaptureStartTimeQItem);

    return;
} 

static void scrubRttMonStatsCaptureStartTimeQItem (rttMonStatsCaptureStartTimeQItem
						   * inRttMonStatsCaptureStartTimeQItem)
{
    if (inRttMonStatsCaptureStartTimeQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonStatsCaptureStartTimeQItem->
	       valid);
    inRttMonStatsCaptureStartTimeQItem->
	rttMonStatsCaptureStartTimeIndex = 0;
    memset(inRttMonStatsCaptureStartTimeQItem->
	   statsTotalsEntry, 0,
	   sizeof(rttMonStatsTotalsEntry_t));
    scrubRttMonStatsCapturePathQ(inRttMonStatsCaptureStartTimeQItem->
				 statsCapturePathQHead);

    return;
} 

static void freeRttMonStatsCaptureStartTimeQ (long rttMonCtrlAdminIndex,
					      rttMonStatsCaptureStartTimeQHead
					      * inRttMonStatsCaptureStartTimeQHead)
{
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem, *tmpStartTimeQItemNext;

    if (inRttMonStatsCaptureStartTimeQHead == NULL) {
	return;
    }
    for (tmpStartTimeQItem =
	 inRttMonStatsCaptureStartTimeQHead->
	 rttMonStatsCaptureStartTimeQ.qhead;
	 tmpStartTimeQItem != NULL;
	 tmpStartTimeQItem = tmpStartTimeQItemNext) {
	tmpStartTimeQItemNext = tmpStartTimeQItem->next;
	unqueue(&(inRttMonStatsCaptureStartTimeQHead->
		  rttMonStatsCaptureStartTimeQ),
		tmpStartTimeQItem);
	tmpStartTimeQItem->next = NULL;
	if ((inRttMonStatsCaptureStartTimeQHead->
	     lastSearchedStatsCaptureStartTimeQItem != NULL) &&
	    (inRttMonStatsCaptureStartTimeQHead->
	     lastSearchedStatsCaptureStartTimeQItem ==
	     tmpStartTimeQItem)) {
	    inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem = NULL;
	}
	freeRttMonStatsCaptureStartTimeQItem(rttMonCtrlAdminIndex,
					     tmpStartTimeQItem);
    } 
    free(inRttMonStatsCaptureStartTimeQHead);

    return;
} 

static void scrubRttMonStatsCaptureStartTimeQ (rttMonStatsCaptureStartTimeQHead
					       * inRttMonStatsCaptureStartTimeQHead)
{
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;

    if (inRttMonStatsCaptureStartTimeQHead == NULL) {
	return;
    }
    inRttMonStatsCaptureStartTimeQHead->
	currentStatsCaptureStartTimeQItem = NULL;
    inRttMonStatsCaptureStartTimeQHead->
	lastSearchedStatsCaptureStartTimeQItem = NULL;
    inRttMonStatsCaptureStartTimeQHead->
	lastAddedStatsCaptureStartTimeQItem = NULL;

    for (tmpStartTimeQItem =
	 inRttMonStatsCaptureStartTimeQHead->
	 rttMonStatsCaptureStartTimeQ.qhead;
	 tmpStartTimeQItem != NULL;
	 tmpStartTimeQItem = tmpStartTimeQItem->next) {
	scrubRttMonStatsCaptureStartTimeQItem(tmpStartTimeQItem);
    } 
    return;
} 

static rttMonStatsCaptureStartTimeQItem
*newRttMonStatsCaptureStartTimeQItem (rttMonCtrlAdminEntry_t
				      * ctrlAdminEntry,
				      rttMonEchoAdminEntry_t
				      * echoAdminEntry,
				      long
				      *rttMallocOctetsAllocated,
				      long
				      protocolType)
{
    rttMonStatsCaptureStartTimeQItem
	* newRttMonStatsCaptureStartTimeQItem;

    newRttMonStatsCaptureStartTimeQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureStartTimeQItem));
    if (newRttMonStatsCaptureStartTimeQItem == NULL) {
	return (newRttMonStatsCaptureStartTimeQItem);
    }
    memset(newRttMonStatsCaptureStartTimeQItem, 0,
	   sizeof(rttMonStatsCaptureStartTimeQItem));

    newRttMonStatsCaptureStartTimeQItem->
	statsTotalsEntry =
	    rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		      sizeof(rttMonStatsTotalsEntry_t));
    if (newRttMonStatsCaptureStartTimeQItem->
	statsTotalsEntry == NULL) {
	freeRttMonStatsCaptureStartTimeQItem(ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     newRttMonStatsCaptureStartTimeQItem);
	newRttMonStatsCaptureStartTimeQItem = NULL;
	return (newRttMonStatsCaptureStartTimeQItem);
    }
    memset(newRttMonStatsCaptureStartTimeQItem->
	   statsTotalsEntry, 0,
	   sizeof(rttMonStatsTotalsEntry_t));

    newRttMonStatsCaptureStartTimeQItem->
	statsCapturePathQHead =
	    newRttMonStatsCapturePathQ(ctrlAdminEntry,
				       echoAdminEntry,
				       rttMallocOctetsAllocated,
				       protocolType);
    if (newRttMonStatsCaptureStartTimeQItem->
	statsCapturePathQHead == NULL) {
	freeRttMonStatsCaptureStartTimeQItem(ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     newRttMonStatsCaptureStartTimeQItem);
	newRttMonStatsCaptureStartTimeQItem = NULL;
	return (newRttMonStatsCaptureStartTimeQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureStartTimeQItem);
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsTotalsEntry_t);

    return (newRttMonStatsCaptureStartTimeQItem);

} 

static rttMonStatsCaptureStartTimeQHead
*newRttMonStatsCaptureStartTimeQ (rttMonCtrlAdminEntry_t
				  * ctrlAdminEntry,
				  rttMonEchoAdminEntry_t
				  * echoAdminEntry,
				  long
				  *rttMallocOctetsAllocated,
				  long
				  protocolType)
{
    rttMonStatsCaptureStartTimeQHead *returnQHead;
    rttMonStatsCaptureStartTimeQItem *newStartTimeQItem;
    long i;
    long initialSize;

    returnQHead =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonStatsCaptureStartTimeQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonStatsCaptureStartTimeQHead));

    queue_init(&(returnQHead->rttMonStatsCaptureStartTimeQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonStatisticsAdminNumHourGroups;

    for (i = 0; i < initialSize; i++) {
	newStartTimeQItem =
	    newRttMonStatsCaptureStartTimeQItem(ctrlAdminEntry,
						echoAdminEntry,
						rttMallocOctetsAllocated,
						protocolType);
	if (newStartTimeQItem == NULL) {
	    freeRttMonStatsCaptureStartTimeQ(ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	enqueue(&(returnQHead->rttMonStatsCaptureStartTimeQ),
		newStartTimeQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonStatsCaptureStartTimeQHead);

    return (returnQHead);

} 

static rttMonStatsCaptureStartTimeQItem
*findRttMonStatsCaptureStartTimeQItem (rttMonStatsCaptureStartTimeQHead
				       * inRttMonStatsCaptureStartTimeQHead,
				       unsigned long
				       rttMonStatsCaptureStartTimeIndex)
{
    rttMonStatsCaptureStartTimeQItem *returnStartTimeQItem, *startStartTimeQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureStartTimeQHead == NULL) {
	return (NULL);
    }
    /* Comparing SysUptime  */
    if ((inRttMonStatsCaptureStartTimeQHead->
	 lastSearchedStatsCaptureStartTimeQItem != NULL) &&
	(inRttMonStatsCaptureStartTimeQHead->
	 lastSearchedStatsCaptureStartTimeQItem->
	 rttMonStatsCaptureStartTimeIndex ==
	 rttMonStatsCaptureStartTimeIndex)) {
	return (inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem);
    } else if ((inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem != NULL) &&
	       (inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem->
		rttMonStatsCaptureStartTimeIndex <
		rttMonStatsCaptureStartTimeIndex)) {
	startStartTimeQItem =
	    inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem;
    } else {
	startStartTimeQItem =
	    inRttMonStatsCaptureStartTimeQHead->
		rttMonStatsCaptureStartTimeQ.qhead;
    } 
    for (returnStartTimeQItem = startStartTimeQItem;
	 returnStartTimeQItem != NULL;
	 returnStartTimeQItem = returnStartTimeQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnStartTimeQItem->valid))
	    && (returnStartTimeQItem->
		rttMonStatsCaptureStartTimeIndex ==
		rttMonStatsCaptureStartTimeIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureStartTimeQHead->
	    lastSearchedStatsCaptureStartTimeQItem =
		returnStartTimeQItem;
	return (returnStartTimeQItem);
    } else {
	return (NULL);
    }
} 

static rttMonStatsCaptureStartTimeQItem
*findNextRttMonStatsCaptureStartTimeQItem (rttMonStatsCaptureStartTimeQHead
					   * inRttMonStatsCaptureStartTimeQHead,
					   unsigned long
					   rttMonStatsCaptureStartTimeIndex)
{
    rttMonStatsCaptureStartTimeQItem *returnStartTimeQItem, *startStartTimeQItem;
    boolean found = FALSE;

    if (inRttMonStatsCaptureStartTimeQHead == NULL) {
	return (NULL);
    }
    /* Comparing SysUptime  */ 
    if ((inRttMonStatsCaptureStartTimeQHead->
	 lastSearchedStatsCaptureStartTimeQItem != NULL) &&
	(inRttMonStatsCaptureStartTimeQHead->
	 lastSearchedStatsCaptureStartTimeQItem->
	 rttMonStatsCaptureStartTimeIndex ==
	 rttMonStatsCaptureStartTimeIndex)) {
	return (inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem);
    } else if ((inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem != NULL) &&
	       (inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem->
		rttMonStatsCaptureStartTimeIndex <
		rttMonStatsCaptureStartTimeIndex)) {
	startStartTimeQItem =
	    inRttMonStatsCaptureStartTimeQHead->
		lastSearchedStatsCaptureStartTimeQItem;
    } else {
	startStartTimeQItem =
	    inRttMonStatsCaptureStartTimeQHead->
		rttMonStatsCaptureStartTimeQ.qhead;
    } 
    for (returnStartTimeQItem = startStartTimeQItem;
	 returnStartTimeQItem != NULL;
	 returnStartTimeQItem = returnStartTimeQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnStartTimeQItem->valid))
	    && ((returnStartTimeQItem->
		 rttMonStatsCaptureStartTimeIndex ==
		 rttMonStatsCaptureStartTimeIndex) ||
		(returnStartTimeQItem->
		 rttMonStatsCaptureStartTimeIndex >
		 rttMonStatsCaptureStartTimeIndex))) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonStatsCaptureStartTimeQHead->
	    lastSearchedStatsCaptureStartTimeQItem =
		returnStartTimeQItem;
	return (returnStartTimeQItem);
    } else {
	return (NULL);
    }
}			

/*
 *  NOTES:
 *  The following MUST be valid:
 *  rttMonStatsCaptureStartTimeQItem->
 *  rttMonStatsCaptureStartTimeIndex
 */ 

static int updateRttMonStatsCaptureStartTimeQItem (long rttMonCtrlAdminIndex,
						   rttMonStatsCaptureStartTimeQHead
						   * inRttMonStatsCaptureStartTimeQHead,
						   rttMonStatsCaptureStartTimeQItem
						   * inRttMonStatsCaptureStartTimeQItem)
{
    unsigned long tmpRttMonStatsCaptureStartTimeIndex;
    rttMonStatsCaptureStartTimeQItem *prevItem, *currItem;

    if (inRttMonStatsCaptureStartTimeQHead == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonStatsCaptureStartTimeQHead is NULL");
	return (D_InvalidParameter);
    }
    if (inRttMonStatsCaptureStartTimeQItem == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonStatsCaptureStartTimeQItem is NULL");
	return (D_InvalidParameter);
    }
    unqueue(&(inRttMonStatsCaptureStartTimeQHead->
	      rttMonStatsCaptureStartTimeQ),
	    inRttMonStatsCaptureStartTimeQItem);
    /*  don't need to update the last searched pointer
     *   because we are placing the queue into lexigraphical
     *   order and thus it will still be valid
     */
    tmpRttMonStatsCaptureStartTimeIndex =
	inRttMonStatsCaptureStartTimeQItem->
	    rttMonStatsCaptureStartTimeIndex;
    prevItem = NULL;
    currItem = inRttMonStatsCaptureStartTimeQHead->
	rttMonStatsCaptureStartTimeQ.qhead;
    while (currItem != NULL) {
	if (tmpRttMonStatsCaptureStartTimeIndex <
	    currItem->rttMonStatsCaptureStartTimeIndex) {
	    break;
	}
	prevItem = currItem;
	currItem = currItem->next;
    } 
    insqueue(&(inRttMonStatsCaptureStartTimeQHead->
	       rttMonStatsCaptureStartTimeQ),
	     inRttMonStatsCaptureStartTimeQItem, prevItem);

    SET_VALID(I_queue_item_in_use,
	      inRttMonStatsCaptureStartTimeQItem->valid);

    return (D_Return_Successfull);

}


/*
 *  rttMonHistoryCollectionEntry
 */ 

void freeRttMonHistoryCollectionEntry (rttMonHistoryCollectionEntry_t
				       * rttMonHistoryCollectionEntry)
{
    if (rttMonHistoryCollectionEntry == NULL) {
	return;
    }
    if (rttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddress != NULL) {
	rttMonFreeOctetStringBuffer(rttMonHistoryCollectionEntry->
				    rttMonHistoryCollectionAddress);
    }
    if (rttMonHistoryCollectionEntry->
	rttMonHistoryCollectionSenseDescription != NULL) {
	rttMonFreeOctetStringBuffer(rttMonHistoryCollectionEntry->
				    rttMonHistoryCollectionSenseDescription);
    }
    free(rttMonHistoryCollectionEntry);

    return;

} 

static void freeShortRttMonHistoryCollectionEntry (short_rttMonHistoryCollectionEntry_t
						   * shortRttMonHistoryCollectionEntry)
{
    if (shortRttMonHistoryCollectionEntry == NULL) {
	return;
    }
    if (shortRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddressShared == FALSE) {
	if (shortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddress != NULL) {
	    rttMonFreeOctetStringBuffer(shortRttMonHistoryCollectionEntry->
					rttMonHistoryCollectionAddress);
	}
    }
    free(shortRttMonHistoryCollectionEntry);

    return;

} 

static void scrubShortRttMonHistoryCollectionEntry (short_rttMonHistoryCollectionEntry_t
						    * shortRttMonHistoryCollectionEntry)
{
    OctetString *tmpRttMonHistoryCollectionAddress;
    long tmpRttMonHistoryCollectionSampleIndex;
    boolean tmpRttMonHistoryCollectionAddressShared;

    if (shortRttMonHistoryCollectionEntry == NULL) {
	return;
    }
    tmpRttMonHistoryCollectionSampleIndex =
	shortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionSampleIndex;
    tmpRttMonHistoryCollectionAddressShared =
	shortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddressShared;
    tmpRttMonHistoryCollectionAddress =
	shortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddress;
    memset(shortRttMonHistoryCollectionEntry, 0,
	   sizeof(short_rttMonHistoryCollectionEntry_t));
    shortRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddressShared =
	    tmpRttMonHistoryCollectionAddressShared;
    shortRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddress =
	    tmpRttMonHistoryCollectionAddress;
    if (shortRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddressShared == FALSE) {
	scrubOctetString(shortRttMonHistoryCollectionEntry->
			 rttMonHistoryCollectionAddress);
    }
    shortRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionSampleIndex =
	    tmpRttMonHistoryCollectionSampleIndex;

    return;
}


/*
 *  rttMonHistoryCollectionSampleQ
 */ 

static void freeRttMonHistoryCollectionSampleQItem (long rttMonCtrlAdminIndex,
						    rttMonHistoryCollectionSampleQItem
						    * rttMonHistoryCollectionSampleQItem)
{
    if (rttMonHistoryCollectionSampleQItem == NULL) {
	return;
    }
    if (rttMonHistoryCollectionSampleQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonHistoryCollectionSampleQItem->next not NULL");
    }
    if (rttMonHistoryCollectionSampleQItem->
	shortHistoryCollectionEntry != NULL) {
	freeShortRttMonHistoryCollectionEntry(rttMonHistoryCollectionSampleQItem->
					      shortHistoryCollectionEntry);
    }
    free(rttMonHistoryCollectionSampleQItem);

    return;
} 

static void scrubRttMonHistoryCollectionSampleQItem (rttMonHistoryCollectionSampleQItem
						     * rttMonHistoryCollectionSampleQItem)
{
    if (rttMonHistoryCollectionSampleQItem == NULL) {
	return;
    }
    ZERO_VALID(rttMonHistoryCollectionSampleQItem->
	       valid);
    if (rttMonHistoryCollectionSampleQItem->
	shortHistoryCollectionEntry != NULL) {
	scrubShortRttMonHistoryCollectionEntry(rttMonHistoryCollectionSampleQItem->
					       shortHistoryCollectionEntry);
    }
    return;
} 

static void freeRttMonHistoryCollectionSampleQ (long rttMonCtrlAdminIndex,
						rttMonHistoryCollectionSampleQHead
						* inRttMonHistoryCollectionSampleQHead)
{
    rttMonHistoryCollectionSampleQItem *tmpSampleQItem, *tmpSampleQItemNext;

    if (inRttMonHistoryCollectionSampleQHead == NULL) {
	return;
    }
    for (tmpSampleQItem =
	 inRttMonHistoryCollectionSampleQHead->
	 rttMonHistoryCollectionSampleQ.qhead;
	 tmpSampleQItem != NULL;
	 tmpSampleQItem = tmpSampleQItemNext) {
	tmpSampleQItemNext = tmpSampleQItem->next;
	unqueue(&(inRttMonHistoryCollectionSampleQHead->
		  rttMonHistoryCollectionSampleQ),
		tmpSampleQItem);
	tmpSampleQItem->next = NULL;
	if ((inRttMonHistoryCollectionSampleQHead->
	     lastSearchedHistoryCollectionSampleQItem != NULL) &&
	    (inRttMonHistoryCollectionSampleQHead->
	     lastSearchedHistoryCollectionSampleQItem ==
	     tmpSampleQItem)) {
	    inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem = NULL;
	}
	freeRttMonHistoryCollectionSampleQItem(rttMonCtrlAdminIndex,
					       tmpSampleQItem);
    } 
    free(inRttMonHistoryCollectionSampleQHead);

    return;
} 

static void scrubRttMonHistoryCollectionSampleQ (rttMonHistoryCollectionSampleQHead
						 * inRttMonHistoryCollectionSampleQHead)
{
    rttMonHistoryCollectionSampleQItem *tmpSampleQItem;

    if (inRttMonHistoryCollectionSampleQHead == NULL) {
	return;
    }
    for (tmpSampleQItem =
	 inRttMonHistoryCollectionSampleQHead->
	 rttMonHistoryCollectionSampleQ.qhead;
	 tmpSampleQItem != NULL;
	 tmpSampleQItem = tmpSampleQItem->next) {
	scrubRttMonHistoryCollectionSampleQItem(tmpSampleQItem);
    } 
    return;
} 

static short_rttMonHistoryCollectionEntry_t
*newShortRttMonHistoryCollectionEntry (rttMonCtrlAdminEntry_t
				       * ctrlAdminEntry,
				       rttMonEchoAdminEntry_t
				       * echoAdminEntry,
				       long
				       *rttMallocOctetsAllocated,
				       long
				       protocolType)
{
    short_rttMonHistoryCollectionEntry_t
	* newShortRttMonHistoryCollectionEntry;

    newShortRttMonHistoryCollectionEntry =
	rttMalloc(ctrlAdminEntry->
		  rttMonCtrlAdminIndex,
		  sizeof(short_rttMonHistoryCollectionEntry_t));
    if (newShortRttMonHistoryCollectionEntry == NULL) {
	return (newShortRttMonHistoryCollectionEntry);
    }
    memset(newShortRttMonHistoryCollectionEntry, 0,
	   sizeof(short_rttMonHistoryCollectionEntry_t));

    if (ctrlAdminEntry->rttMonCtrlAdminRttType
	== D_rttMonCtrlAdminRttType_pathEcho) {
	newShortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddress =
		preMallocOctetString(ctrlAdminEntry->
				     rttMonCtrlAdminIndex,
				     getMaxRttMonAddressSize(protocolType));
	if (newShortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddress == NULL) {
	    freeShortRttMonHistoryCollectionEntry(newShortRttMonHistoryCollectionEntry);
	    return (NULL);
	}
	newShortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddressShared = FALSE;
    } else {
	newShortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddress =
		echoAdminEntry->
		    rttMonEchoAdminTargetAddress;
	newShortRttMonHistoryCollectionEntry->
	    rttMonHistoryCollectionAddressShared = TRUE;
    } 
    if (ctrlAdminEntry->rttMonCtrlAdminIndex != 0) {
	if (ctrlAdminEntry->rttMonCtrlAdminRttType
	    == D_rttMonCtrlAdminRttType_pathEcho) {
	    *rttMallocOctetsAllocated +=
		getMaxRttMonAddressSize(protocolType);
	}
	*rttMallocOctetsAllocated +=
	    sizeof(short_rttMonHistoryCollectionEntry_t);
    }
    return (newShortRttMonHistoryCollectionEntry);

} 

static rttMonHistoryCollectionSampleQItem
*newRttMonHistoryCollectionSampleQItem (rttMonCtrlAdminEntry_t
					* ctrlAdminEntry,
					rttMonEchoAdminEntry_t
					* echoAdminEntry,
					long
					*rttMallocOctetsAllocated,
					long
					protocolType)
{
    rttMonHistoryCollectionSampleQItem
	* newRttMonHistoryCollectionSampleQItem;

    newRttMonHistoryCollectionSampleQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonHistoryCollectionSampleQItem));
    if (newRttMonHistoryCollectionSampleQItem == NULL) {
	return (newRttMonHistoryCollectionSampleQItem);
    }
    memset(newRttMonHistoryCollectionSampleQItem, 0,
	   sizeof(rttMonHistoryCollectionSampleQItem));

    newRttMonHistoryCollectionSampleQItem->
	shortHistoryCollectionEntry =
	    newShortRttMonHistoryCollectionEntry(ctrlAdminEntry,
						 echoAdminEntry,
						 rttMallocOctetsAllocated,
						 protocolType);
    if (newRttMonHistoryCollectionSampleQItem->
	shortHistoryCollectionEntry == NULL) {
	freeRttMonHistoryCollectionSampleQItem(ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       newRttMonHistoryCollectionSampleQItem);
	newRttMonHistoryCollectionSampleQItem = NULL;
	return (newRttMonHistoryCollectionSampleQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionSampleQItem);

    return (newRttMonHistoryCollectionSampleQItem);

} 

static rttMonHistoryCollectionSampleQHead
*newRttMonHistoryCollectionSampleQ (rttMonCtrlAdminEntry_t
				    * ctrlAdminEntry,
				    rttMonEchoAdminEntry_t
				    * echoAdminEntry,
				    long
				    *rttMallocOctetsAllocated,
				    long
				    protocolType)
{
    rttMonHistoryCollectionSampleQHead *returnQHead;
    rttMonHistoryCollectionSampleQItem *newSampleQItem;
    long i;
    long initialSize;

    returnQHead =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonHistoryCollectionSampleQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonHistoryCollectionSampleQHead));

    queue_init(&(returnQHead->rttMonHistoryCollectionSampleQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonHistoryAdminNumSamples;

    for (i = 0; i < initialSize; i++) {
	newSampleQItem = 
	    newRttMonHistoryCollectionSampleQItem(ctrlAdminEntry,
						  echoAdminEntry,
						  rttMallocOctetsAllocated,
						  protocolType);
	if (newSampleQItem == NULL) {
	    freeRttMonHistoryCollectionSampleQ(ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newSampleQItem->shortHistoryCollectionEntry->
	    rttMonHistoryCollectionSampleIndex = i + 1;

	enqueue(&(returnQHead->rttMonHistoryCollectionSampleQ),
		newSampleQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionSampleQHead);

    return (returnQHead);

} 

static rttMonHistoryCollectionSampleQItem
*findRttMonHistoryCollectionSampleQItem (rttMonHistoryCollectionSampleQHead
					 * inRttMonHistoryCollectionSampleQHead,
					 long
					 rttMonHistoryCollectionSampleIndex)
{
    rttMonHistoryCollectionSampleQItem *returnSampleQItem,  *startSampleQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionSampleQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionSampleQHead->
	 lastSearchedHistoryCollectionSampleQItem != NULL) &&
	(inRttMonHistoryCollectionSampleQHead->
	 lastSearchedHistoryCollectionSampleQItem->
	 shortHistoryCollectionEntry->
	 rttMonHistoryCollectionSampleIndex ==
	 rttMonHistoryCollectionSampleIndex)) {
	return (inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem);
    } else if ((inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem != NULL) &&
	       (inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionSampleIndex <
		rttMonHistoryCollectionSampleIndex)) {
	startSampleQItem =
	    inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem;
    } else {
	startSampleQItem =
	    inRttMonHistoryCollectionSampleQHead->
		rttMonHistoryCollectionSampleQ.qhead;
    } 
    for (returnSampleQItem = startSampleQItem;
	 returnSampleQItem != NULL;
	 returnSampleQItem = returnSampleQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnSampleQItem->valid))
	    && (returnSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionSampleIndex ==
		rttMonHistoryCollectionSampleIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionSampleQHead->
	    lastSearchedHistoryCollectionSampleQItem =
		returnSampleQItem;
	return (returnSampleQItem);
    } else {
	return (NULL);
    }
} 

static rttMonHistoryCollectionSampleQItem
*findNextRttMonHistoryCollectionSampleQItem (rttMonHistoryCollectionSampleQHead
					     * inRttMonHistoryCollectionSampleQHead,
					     long
					     rttMonHistoryCollectionSampleIndex)
{
    rttMonHistoryCollectionSampleQItem *returnSampleQItem, *startSampleQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionSampleQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionSampleQHead->
	 lastSearchedHistoryCollectionSampleQItem != NULL) &&
	(inRttMonHistoryCollectionSampleQHead->
	 lastSearchedHistoryCollectionSampleQItem->
	 shortHistoryCollectionEntry->
	 rttMonHistoryCollectionSampleIndex ==
	 rttMonHistoryCollectionSampleIndex)) {
	return (inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem);
    } else if ((inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem != NULL) &&
	       (inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionSampleIndex <
		rttMonHistoryCollectionSampleIndex)) {
	startSampleQItem =
	    inRttMonHistoryCollectionSampleQHead->
		lastSearchedHistoryCollectionSampleQItem;
    } else {
	startSampleQItem =
	    inRttMonHistoryCollectionSampleQHead->
		rttMonHistoryCollectionSampleQ.qhead;
    } 
    for (returnSampleQItem = startSampleQItem;
	 returnSampleQItem != NULL;
	 returnSampleQItem = returnSampleQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnSampleQItem->valid))
	    && (returnSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionSampleIndex >=
		rttMonHistoryCollectionSampleIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionSampleQHead->
	    lastSearchedHistoryCollectionSampleQItem =
		returnSampleQItem;
	return (returnSampleQItem);
    } else {
	return (NULL);
    }
}


/*
 * rttMonHistoryCollectionBucketQ
 */
 
static void freeRttMonHistoryCollectionBucketQItem (long rttMonCtrlAdminIndex,
						    rttMonHistoryCollectionBucketQItem
						    * inRttMonHistoryCollectionBucketQItem)
{
    if (inRttMonHistoryCollectionBucketQItem == NULL) {
	return;
    }
    if (inRttMonHistoryCollectionBucketQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonHistoryCollectionBucketQItem->next is not NULL");
    }
    if (inRttMonHistoryCollectionBucketQItem->
	historyCollectionSampleQHead != NULL) {
	freeRttMonHistoryCollectionSampleQ(rttMonCtrlAdminIndex,
					   inRttMonHistoryCollectionBucketQItem->
					   historyCollectionSampleQHead);
    }
    free(inRttMonHistoryCollectionBucketQItem);

    return;

} 

static void scrubRttMonHistoryCollectionBucketQItem (rttMonHistoryCollectionBucketQItem
						     * inRttMonHistoryCollectionBucketQItem)
{
    if (inRttMonHistoryCollectionBucketQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonHistoryCollectionBucketQItem->
	       valid);
    inRttMonHistoryCollectionBucketQItem->
	rttMonHistoryCollectionBucketFull = FALSE;

    scrubRttMonHistoryCollectionSampleQ(inRttMonHistoryCollectionBucketQItem->
					historyCollectionSampleQHead);

    return;

} 

static void freeRttMonHistoryCollectionBucketQ (long rttMonCtrlAdminIndex,
						rttMonHistoryCollectionBucketQHead
						* inRttMonHistoryCollectionBucketQHead)
{
    rttMonHistoryCollectionBucketQItem *tmpBucketQItem, *tmpBucketQItemNext;

    if (inRttMonHistoryCollectionBucketQHead == NULL) {
	return;
    }
    for (tmpBucketQItem =
	 inRttMonHistoryCollectionBucketQHead->
	 rttMonHistoryCollectionBucketQ.qhead;
	 tmpBucketQItem != NULL;
	 tmpBucketQItem = tmpBucketQItemNext) {
	tmpBucketQItemNext = tmpBucketQItem->next;
	unqueue(&(inRttMonHistoryCollectionBucketQHead->
		  rttMonHistoryCollectionBucketQ),
		tmpBucketQItem);
	tmpBucketQItem->next = NULL;
	if ((inRttMonHistoryCollectionBucketQHead->
	     lastSearchedHistoryCollectionBucketQItem != NULL) &&
	    (inRttMonHistoryCollectionBucketQHead->
	     lastSearchedHistoryCollectionBucketQItem ==
	     tmpBucketQItem)) {
	    inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem = NULL;
	}
	freeRttMonHistoryCollectionBucketQItem(rttMonCtrlAdminIndex,
					       tmpBucketQItem);
    } 
    free(inRttMonHistoryCollectionBucketQHead);

    return;
} 

static void scrubRttMonHistoryCollectionBucketQ (rttMonHistoryCollectionBucketQHead
						 * inRttMonHistoryCollectionBucketQHead)
{
    rttMonHistoryCollectionBucketQItem *tmpBucketQItem;

    if (inRttMonHistoryCollectionBucketQHead == NULL) {
	return;
    }
    inRttMonHistoryCollectionBucketQHead->
	lastSearchedHistoryCollectionBucketQItem = NULL;
    inRttMonHistoryCollectionBucketQHead->
	currentHistoryCollectionBucketQItem = NULL;

    for (tmpBucketQItem =
	 inRttMonHistoryCollectionBucketQHead->
	 rttMonHistoryCollectionBucketQ.qhead;
	 tmpBucketQItem != NULL;
	 tmpBucketQItem = tmpBucketQItem->next) {
	scrubRttMonHistoryCollectionBucketQItem(tmpBucketQItem);
    } 
    return;
} 

static rttMonHistoryCollectionBucketQItem
*newRttMonHistoryCollectionBucketQItem (rttMonCtrlAdminEntry_t
					* ctrlAdminEntry,
					rttMonEchoAdminEntry_t
					* echoAdminEntry,
					long
					*rttMallocOctetsAllocated,
					long
					protocolType)
{
    rttMonHistoryCollectionBucketQItem
	* newRttMonHistoryCollectionBucketQItem;

    newRttMonHistoryCollectionBucketQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonHistoryCollectionBucketQItem));
    if (newRttMonHistoryCollectionBucketQItem == NULL) {
	return (newRttMonHistoryCollectionBucketQItem);
    }
    memset(newRttMonHistoryCollectionBucketQItem, 0,
	   sizeof(rttMonHistoryCollectionBucketQItem));

    newRttMonHistoryCollectionBucketQItem->
	rttMonHistoryCollectionBucketFull =
	    D_rttMonHistoryCollectionBucketFull_False;

    newRttMonHistoryCollectionBucketQItem->
	historyCollectionSampleQHead =
	    newRttMonHistoryCollectionSampleQ(ctrlAdminEntry,
					      echoAdminEntry,
					      rttMallocOctetsAllocated,
					      protocolType);
    if (newRttMonHistoryCollectionBucketQItem->
	historyCollectionSampleQHead == NULL) {
	freeRttMonHistoryCollectionBucketQItem(ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       newRttMonHistoryCollectionBucketQItem);
	newRttMonHistoryCollectionBucketQItem = NULL;
	return (newRttMonHistoryCollectionBucketQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionBucketQItem);

    return (newRttMonHistoryCollectionBucketQItem);

} 

static rttMonHistoryCollectionBucketQHead
*newRttMonHistoryCollectionBucketQ (rttMonCtrlAdminEntry_t
				    * ctrlAdminEntry,
				    rttMonEchoAdminEntry_t
				    * echoAdminEntry,
				    long
				    *rttMallocOctetsAllocated,
				    long
				    protocolType)
{
    rttMonHistoryCollectionBucketQHead *returnQHead;
    rttMonHistoryCollectionBucketQItem *newBucketQItem;
    long i;
    long initialSize;

    returnQHead = rttMalloc(ctrlAdminEntry->
			    rttMonCtrlAdminIndex,
			    sizeof(rttMonHistoryCollectionBucketQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonHistoryCollectionBucketQHead));

    queue_init(&(returnQHead->rttMonHistoryCollectionBucketQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonHistoryAdminNumBuckets;

    for (i = 0; i < initialSize; i++) {
	newBucketQItem =
	    newRttMonHistoryCollectionBucketQItem(ctrlAdminEntry,
						  echoAdminEntry,
						  rttMallocOctetsAllocated,
						  protocolType);
	if (newBucketQItem == NULL) {
	    freeRttMonHistoryCollectionBucketQ(ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newBucketQItem->rttMonHistoryCollectionBucketIndex = i + 1;

	enqueue(&(returnQHead->rttMonHistoryCollectionBucketQ),
		newBucketQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionBucketQHead);

    return (returnQHead);

} 

static rttMonHistoryCollectionBucketQItem
*findRttMonHistoryCollectionBucketQItem (rttMonHistoryCollectionBucketQHead
					 * inRttMonHistoryCollectionBucketQHead,
					 long rttMonHistoryCollectionBucketIndex)
{
    rttMonHistoryCollectionBucketQItem *returnBucketQItem, *startBucketQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionBucketQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionBucketQHead->
	 lastSearchedHistoryCollectionBucketQItem != NULL) &&
	(inRttMonHistoryCollectionBucketQHead->
	 lastSearchedHistoryCollectionBucketQItem->
	 rttMonHistoryCollectionBucketIndex ==
	 rttMonHistoryCollectionBucketIndex)) {
	return (inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem);
    } else if ((inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem != NULL) &&
	       (inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem->
		rttMonHistoryCollectionBucketIndex <
		rttMonHistoryCollectionBucketIndex)) {
	startBucketQItem =
	    inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem;
    } else {
	startBucketQItem =
	    inRttMonHistoryCollectionBucketQHead->
		rttMonHistoryCollectionBucketQ.qhead;
    } 
    for (returnBucketQItem = startBucketQItem;
	 returnBucketQItem != NULL;
	 returnBucketQItem = returnBucketQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnBucketQItem->valid))
	    && (returnBucketQItem->
		rttMonHistoryCollectionBucketIndex ==
		rttMonHistoryCollectionBucketIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionBucketQHead->
	    lastSearchedHistoryCollectionBucketQItem =
		returnBucketQItem;
	return (returnBucketQItem);
    } else {
	return (NULL);
    }
} 

static rttMonHistoryCollectionBucketQItem
*findNextRttMonHistoryCollectionBucketQItem (rttMonHistoryCollectionBucketQHead
					     * inRttMonHistoryCollectionBucketQHead,
					     long
					     rttMonHistoryCollectionBucketIndex)
{
    rttMonHistoryCollectionBucketQItem *returnBucketQItem, *startBucketQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionBucketQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionBucketQHead->
	 lastSearchedHistoryCollectionBucketQItem != NULL) &&
	(inRttMonHistoryCollectionBucketQHead->
	 lastSearchedHistoryCollectionBucketQItem->
	 rttMonHistoryCollectionBucketIndex ==
	 rttMonHistoryCollectionBucketIndex)) {
	return (inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem);
    } else if ((inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem != NULL) &&
	       (inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem->
		rttMonHistoryCollectionBucketIndex <
		rttMonHistoryCollectionBucketIndex)) {
	startBucketQItem =
	    inRttMonHistoryCollectionBucketQHead->
		lastSearchedHistoryCollectionBucketQItem;
    } else {
	startBucketQItem =
	    inRttMonHistoryCollectionBucketQHead->
		rttMonHistoryCollectionBucketQ.qhead;
    } 
    for (returnBucketQItem = startBucketQItem;
	 returnBucketQItem != NULL;
	 returnBucketQItem = returnBucketQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnBucketQItem->valid))
	    && (returnBucketQItem->
		rttMonHistoryCollectionBucketIndex >=
		rttMonHistoryCollectionBucketIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionBucketQHead->
	    lastSearchedHistoryCollectionBucketQItem =
		returnBucketQItem;
	return (returnBucketQItem);
    } else {
	return (NULL);
    }
}


/*
 * rttMonHistoryCollectionLifeQ
 */ 

static void freeRttMonHistoryCollectionLifeQItem (long rttMonCtrlAdminIndex,
						  rttMonHistoryCollectionLifeQItem
						  * inRttMonHistoryCollectionLifeQItem)
{
    if (inRttMonHistoryCollectionLifeQItem == NULL) {
	return;
    }
    if (inRttMonHistoryCollectionLifeQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonHistoryCollectionLifeQItem->next is not NULL");
    }
    if (inRttMonHistoryCollectionLifeQItem->
	historyCollectionBucketQHead != NULL) {
	freeRttMonHistoryCollectionBucketQ(rttMonCtrlAdminIndex,
					   inRttMonHistoryCollectionLifeQItem->
					   historyCollectionBucketQHead);
    }
    free(inRttMonHistoryCollectionLifeQItem);

    return;

} 

static void scrubRttMonHistoryCollectionLifeQItem (rttMonHistoryCollectionLifeQItem
						   * inRttMonHistoryCollectionLifeQItem)
{
    if (inRttMonHistoryCollectionLifeQItem == NULL) {
	return;
    }
    ZERO_VALID(inRttMonHistoryCollectionLifeQItem->
	       valid);
    scrubRttMonHistoryCollectionBucketQ(inRttMonHistoryCollectionLifeQItem->
					historyCollectionBucketQHead);

    return;

} 

static void freeRttMonHistoryCollectionLifeQ (long rttMonCtrlAdminIndex,
					      rttMonHistoryCollectionLifeQHead
					      * inRttMonHistoryCollectionLifeQHead)
{
    rttMonHistoryCollectionLifeQItem *tmpLifeQItem, *tmpLifeQItemNext;

    if (inRttMonHistoryCollectionLifeQHead == NULL) {
	return;
    }
    for (tmpLifeQItem = inRttMonHistoryCollectionLifeQHead->
	 rttMonHistoryCollectionLifeQ.qhead;
	 tmpLifeQItem != NULL;
	 tmpLifeQItem = tmpLifeQItemNext) {
	tmpLifeQItemNext = tmpLifeQItem->next;
	unqueue(&(inRttMonHistoryCollectionLifeQHead->
		  rttMonHistoryCollectionLifeQ), tmpLifeQItem);
	tmpLifeQItem->next = NULL;
	if ((inRttMonHistoryCollectionLifeQHead->
	     lastSearchedHistoryCollectionLifeQItem != NULL) &&
	    (inRttMonHistoryCollectionLifeQHead->
	     lastSearchedHistoryCollectionLifeQItem ==
	     tmpLifeQItem)) {
	    inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem = NULL;
	}
	freeRttMonHistoryCollectionLifeQItem(rttMonCtrlAdminIndex,
					     tmpLifeQItem);
    } 
    free(inRttMonHistoryCollectionLifeQHead);

    return;
} 

static void scrubRttMonHistoryCollectionLifeQ (rttMonHistoryCollectionLifeQHead
					       * inRttMonHistoryCollectionLifeQHead)
{
    rttMonHistoryCollectionLifeQItem *tmpLifeQItem;

    if (inRttMonHistoryCollectionLifeQHead == NULL) {
	return;
    }
    inRttMonHistoryCollectionLifeQHead->
	currentHistoryCollectionLifeQItem = NULL;
    inRttMonHistoryCollectionLifeQHead->
	lastSearchedHistoryCollectionLifeQItem = NULL;
    inRttMonHistoryCollectionLifeQHead->
	lastAddedHistoryCollectionLifeQItem = NULL;

    for (tmpLifeQItem = inRttMonHistoryCollectionLifeQHead->
	 rttMonHistoryCollectionLifeQ.qhead;
	 tmpLifeQItem != NULL;
	 tmpLifeQItem = tmpLifeQItem->next) {
	scrubRttMonHistoryCollectionLifeQItem(tmpLifeQItem);
    } 
    return;
} 

static rttMonHistoryCollectionLifeQItem
*newRttMonHistoryCollectionLifeQItem (rttMonCtrlAdminEntry_t
				      * ctrlAdminEntry,
				      rttMonEchoAdminEntry_t
				      * echoAdminEntry,
				      long
				      *rttMallocOctetsAllocated,
				      long
				      protocolType)
{
    rttMonHistoryCollectionLifeQItem
	* newRttMonHistoryCollectionLifeQItem;

    newRttMonHistoryCollectionLifeQItem =
	rttMalloc(ctrlAdminEntry->rttMonCtrlAdminIndex,
		  sizeof(rttMonHistoryCollectionLifeQItem));
    if (newRttMonHistoryCollectionLifeQItem == NULL) {
	return (newRttMonHistoryCollectionLifeQItem);
    }
    memset(newRttMonHistoryCollectionLifeQItem, 0,
	   sizeof(rttMonHistoryCollectionLifeQItem));

    newRttMonHistoryCollectionLifeQItem->
	historyCollectionBucketQHead =
	    newRttMonHistoryCollectionBucketQ(ctrlAdminEntry,
					      echoAdminEntry,
					      rttMallocOctetsAllocated,
					      protocolType);
    if (newRttMonHistoryCollectionLifeQItem->
	historyCollectionBucketQHead == NULL) {
	freeRttMonHistoryCollectionLifeQItem(ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     newRttMonHistoryCollectionLifeQItem);
	newRttMonHistoryCollectionLifeQItem = NULL;
	return (newRttMonHistoryCollectionLifeQItem);
    }
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionLifeQItem);

    return (newRttMonHistoryCollectionLifeQItem);
} 

static rttMonHistoryCollectionLifeQHead
*newRttMonHistoryCollectionLifeQ (rttMonCtrlAdminEntry_t
				  * ctrlAdminEntry,
				  rttMonEchoAdminEntry_t
				  * echoAdminEntry,
				  long
				  *rttMallocOctetsAllocated,
				  long
				  protocolType)
{
    rttMonHistoryCollectionLifeQHead *returnQHead;
    rttMonHistoryCollectionLifeQItem *newLifeQItem;
    long i;
    long initialSize;

    returnQHead = rttMalloc(ctrlAdminEntry->
			    rttMonCtrlAdminIndex,
			    sizeof(rttMonHistoryCollectionLifeQHead));
    if (returnQHead == NULL) {
	return (returnQHead);
    }
    memset(returnQHead, 0,
	   sizeof(rttMonHistoryCollectionLifeQHead));

    queue_init(&(returnQHead->
		 rttMonHistoryCollectionLifeQ), 0);

    initialSize =
	ctrlAdminEntry->rttMonHistoryAdminNumLives;

    for (i = 0; i < initialSize; i++) {
	newLifeQItem = 
	    newRttMonHistoryCollectionLifeQItem(ctrlAdminEntry,
						echoAdminEntry,
						rttMallocOctetsAllocated,
						protocolType);
	if (newLifeQItem == NULL) {
	    freeRttMonHistoryCollectionLifeQ(ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	newLifeQItem->rttMonHistoryCollectionLifeIndex = i + 1;

	enqueue(&(returnQHead->
		  rttMonHistoryCollectionLifeQ),
		newLifeQItem);
    } 
    *rttMallocOctetsAllocated +=
	sizeof(rttMonHistoryCollectionLifeQHead);

    return (returnQHead);

} 

static rttMonHistoryCollectionLifeQItem
*findRttMonHistoryCollectionLifeQItem (rttMonHistoryCollectionLifeQHead
				       * inRttMonHistoryCollectionLifeQHead,
				       long rttMonHistoryCollectionLifeIndex)
{
    rttMonHistoryCollectionLifeQItem *returnLifeQItem, *startLifeQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionLifeQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionLifeQHead->
	 lastSearchedHistoryCollectionLifeQItem != NULL) &&
	(inRttMonHistoryCollectionLifeQHead->
	 lastSearchedHistoryCollectionLifeQItem->
	 rttMonHistoryCollectionLifeIndex ==
	 rttMonHistoryCollectionLifeIndex)) {
	return (inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem);
    } else if ((inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem != NULL) &&
	       (inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem->
		rttMonHistoryCollectionLifeIndex <
		rttMonHistoryCollectionLifeIndex)) {
	startLifeQItem =
	    inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem;
    } else {
	startLifeQItem =
	    inRttMonHistoryCollectionLifeQHead->
		rttMonHistoryCollectionLifeQ.qhead;
    } 
    for (returnLifeQItem = startLifeQItem;
	 returnLifeQItem != NULL;
	 returnLifeQItem = returnLifeQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnLifeQItem->valid))
	    && (returnLifeQItem->
		rttMonHistoryCollectionLifeIndex ==
		rttMonHistoryCollectionLifeIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionLifeQHead->
	    lastSearchedHistoryCollectionLifeQItem =
		returnLifeQItem;
	return (returnLifeQItem);
    } else {
	return (NULL);
    }
} 

static rttMonHistoryCollectionLifeQItem
*findNextRttMonHistoryCollectionLifeQItem (rttMonHistoryCollectionLifeQHead
					   * inRttMonHistoryCollectionLifeQHead,
					   long rttMonHistoryCollectionLifeIndex)
{
    rttMonHistoryCollectionLifeQItem *returnLifeQItem, *startLifeQItem;
    boolean found = FALSE;

    if (inRttMonHistoryCollectionLifeQHead == NULL) {
	return (NULL);
    }
    if ((inRttMonHistoryCollectionLifeQHead->
	 lastSearchedHistoryCollectionLifeQItem != NULL) &&
	(inRttMonHistoryCollectionLifeQHead->
	 lastSearchedHistoryCollectionLifeQItem->
	 rttMonHistoryCollectionLifeIndex ==
	 rttMonHistoryCollectionLifeIndex)) {
	return (inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem);
    } else if ((inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem != NULL) &&
	       (inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem->
		rttMonHistoryCollectionLifeIndex <
		rttMonHistoryCollectionLifeIndex)) {
	startLifeQItem =
	    inRttMonHistoryCollectionLifeQHead->
		lastSearchedHistoryCollectionLifeQItem;
    } else {
	startLifeQItem =
	    inRttMonHistoryCollectionLifeQHead->
		rttMonHistoryCollectionLifeQ.qhead;
    }
    for (returnLifeQItem = startLifeQItem;
	 returnLifeQItem != NULL;
	 returnLifeQItem = returnLifeQItem->next) {
	if ((VALID(I_queue_item_in_use,
		   returnLifeQItem->valid))
	    && (returnLifeQItem->
		rttMonHistoryCollectionLifeIndex >=
		rttMonHistoryCollectionLifeIndex)) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	inRttMonHistoryCollectionLifeQHead->
	    lastSearchedHistoryCollectionLifeQItem =
		returnLifeQItem;
	return (returnLifeQItem);
    } else {
	return (NULL);
    }
}
			
/*
 *  NOTES:
 *  The following MUST be valid:
 *  rttMonHistoryCollectionLifeQItem->
 *  rttMonHistoryCollectionLifeIndex
 */ 

static int updateRttMonHistoryCollectionLifeQItem (long rttMonCtrlAdminIndex,
						   rttMonHistoryCollectionLifeQHead
						   * inRttMonHistoryCollectionLifeQHead,
						   rttMonHistoryCollectionLifeQItem
						   * inRttMonHistoryCollectionLifeQItem)
{
    long tmpRttMonHistoryCollectionLifeIndex;
    rttMonHistoryCollectionLifeQItem *prevItem, *currItem;

    if (inRttMonHistoryCollectionLifeQHead == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonHistoryCollectionLifeQHead is NULL");
	return (D_InvalidParameter);
    }
    if (inRttMonHistoryCollectionLifeQItem == NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "inRttMonHistoryCollectionLifeQItem is NULL");
	return (D_InvalidParameter);
    }
    unqueue(&(inRttMonHistoryCollectionLifeQHead->
	      rttMonHistoryCollectionLifeQ),
	    inRttMonHistoryCollectionLifeQItem);
    /*
     *  Don't need to update the last searched pointer
     *   because we are reordering into lexigrahpical 
     *   order and thus it will still be valid
     */
    tmpRttMonHistoryCollectionLifeIndex =
	inRttMonHistoryCollectionLifeQItem->
	    rttMonHistoryCollectionLifeIndex;
    prevItem = NULL;
    currItem = inRttMonHistoryCollectionLifeQHead->
	rttMonHistoryCollectionLifeQ.qhead;
    while (currItem != NULL) {
	if (tmpRttMonHistoryCollectionLifeIndex <
	    currItem->rttMonHistoryCollectionLifeIndex) {
	    break;
	}
	prevItem = currItem;
	currItem = currItem->next;
    } 
    insqueue(&(inRttMonHistoryCollectionLifeQHead->
	       rttMonHistoryCollectionLifeQ),
	     inRttMonHistoryCollectionLifeQItem,
	     prevItem);

    SET_VALID(I_queue_item_in_use,
	      inRttMonHistoryCollectionLifeQItem->valid);

    return (D_Return_Successfull);

}


/*
 *  connectHandleList
 */ 

static void freeConnectionHandleListItem (long rttMonCtrlAdminIndex,
					  connectHandleList
					  * connHandleListItem)
{
    if (connHandleListItem == NULL) {
	return;
    }
    if (connHandleListItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "connHandleListItem->next is not NULL");
    }
    if (connHandleListItem->connAddress != NULL) {
	rttMonFreeOctetStringBuffer(connHandleListItem->connAddress);
    }
    free(connHandleListItem);

    return;
} 

void rttMonFreeConnectionHandleList (long rttMonCtrlAdminIndex,
				     connectHandleList
				     * connHandleList)
{
    connectHandleList *tmpListItem, *tmpNextListItem;

    tmpListItem = connHandleList;

    while (tmpListItem != NULL) {
	tmpNextListItem = tmpListItem->next;
	tmpListItem->next = NULL;
	freeConnectionHandleListItem(rttMonCtrlAdminIndex,
				     tmpListItem);
	tmpListItem = tmpNextListItem;
    } 
    return;
} 

static connectHandleList *newConnectionHandleListItem (long rttMonCtrlAdminIndex,
						       long
						       *rttMallocOctetsAllocated,
						       long
						       protocolType)
{
    connectHandleList *tmpConnHandleListItem;

    tmpConnHandleListItem = (connectHandleList *)
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(connectHandleList));
    if (tmpConnHandleListItem == NULL) {
	return (NULL);
    }
    memset(tmpConnHandleListItem, 0, sizeof(connectHandleList));
    tmpConnHandleListItem->connected = FALSE;
    tmpConnHandleListItem->connAddress =
	preMallocOctetString(rttMonCtrlAdminIndex,
			     getMaxRttMonAddressSize(protocolType));
    if (tmpConnHandleListItem->connAddress == NULL) {
	freeConnectionHandleListItem(rttMonCtrlAdminIndex,
				     tmpConnHandleListItem);
	return (NULL);
    }
    if (rttMonCtrlAdminIndex != 0) {
	*rttMallocOctetsAllocated += sizeof(connectHandleList);
	*rttMallocOctetsAllocated +=
	    getMaxRttMonAddressSize(protocolType);
    }
    return (tmpConnHandleListItem);
} 

static connectHandleList *newConnectionHandleListParent (rttMonCtrlAdminQItem
							 * inCtrlAdminQItem,
							 long
							 rttMonCtrlAdminIndex,
							 long
							 *rttMallocOctetsAllocated,
							 long
							 protocolType)
{
    connectHandleList *tmpListItem;
    connectHandleList *newListHead;
    long listSize;

    /* 
     *  I cannot predict how many will be in the
     *  list at any one time, so I will use 3 times
     *  the maximum number hops the user configured
     */
    if (inCtrlAdminQItem->ctrlAdminEntry->
	rttMonStatisticsAdminNumHops >
	inCtrlAdminQItem->ctrlAdminEntry->
	rttMonHistoryAdminNumSamples) {
	listSize = inCtrlAdminQItem->ctrlAdminEntry->
	    rttMonStatisticsAdminNumHops * 3;
    } else {
	listSize = inCtrlAdminQItem->ctrlAdminEntry->
	    rttMonHistoryAdminNumSamples * 3;
    } 
    if (listSize <= 10) {
	listSize = 12;
    }
    newListHead = 
	newConnectionHandleListItem(rttMonCtrlAdminIndex,
				    rttMallocOctetsAllocated,
				    protocolType);
    if (newListHead == NULL) {
	return (NULL);
    }
    tmpListItem = newListHead;
    listSize--;

    while (listSize > 0) {
	tmpListItem->next = 
	    newConnectionHandleListItem(rttMonCtrlAdminIndex,
					rttMallocOctetsAllocated,
					protocolType);
	tmpListItem = tmpListItem->next;
	if (tmpListItem == NULL) {
	    rttMonFreeConnectionHandleList(rttMonCtrlAdminIndex,
					   newListHead);
	    return (NULL);
	}
	listSize--;
    } 
    return (newListHead);
} 

static connectHandleList *getUnusedConnectionHandleItem (connectHandleList
							 * inConnectionHandleList)
{
    connectHandleList *tmpConnHandle;

    tmpConnHandle = inConnectionHandleList;
    while (VALID(I_queue_item_in_use,
		 tmpConnHandle->valid) &&
	   (tmpConnHandle != NULL)) {
	tmpConnHandle = tmpConnHandle->next;
    } 
    return (tmpConnHandle);

} 

static connectHandleList *findConnectionHandle (connectHandleList
						* inConnectionHandleList,
						OctetString
						* handleAddress)
{
    connectHandleList *tmpConnHandle;
    boolean found;

    found = FALSE;
    tmpConnHandle = inConnectionHandleList;
    while (!found &&
	   (tmpConnHandle != NULL)) {
	if (RttCmpOS(handleAddress,
		     tmpConnHandle->connAddress) == 0) {
	    found = TRUE;
	} else {
	    tmpConnHandle = tmpConnHandle->next;
	}
    } 
    return (tmpConnHandle);

} 

static long collectConnectHandleList (connectHandleList
				      * outConnHandleList,
				      connectHandleList
				      * inConnHandleList)
{
    long return_code;
    connectHandleList *tmpOutListItem,
    *tmpInListItem;

    return_code = D_Return_Successfull;

    if ((outConnHandleList == NULL) ||
	(inConnHandleList == NULL)) {
	return (D_Conn_Handle_List_Invalid);
    }
    tmpOutListItem = outConnHandleList;
    tmpInListItem = inConnHandleList;
    while ((tmpOutListItem != NULL) &&
	   (tmpInListItem != NULL)) {
	tmpOutListItem->connHandle =
	    tmpInListItem->connHandle;
	tmpOutListItem->connected =
	    tmpInListItem->connected;

	tmpOutListItem = tmpOutListItem->next;
	tmpInListItem = tmpInListItem->next;

    } 
    return (return_code);
}


/*
 * rttMonCtrlAdminQ
 */ 

static void copyEchoAdminEntry (rttMonEchoAdminEntry_t
				* targetEchoAdminEntryBuff,
				rttMonEchoAdminEntry_t
				* sourceEchoAdminEntryBuff)
{
    OctetString *holdRttMonEchoAdminTargetAddress;

    holdRttMonEchoAdminTargetAddress =
	targetEchoAdminEntryBuff->
	    rttMonEchoAdminTargetAddress;
    cloneToPreAllocatedOctetString(sourceEchoAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonEchoAdminTargetAddress,
				   sourceEchoAdminEntryBuff->
				   rttMonEchoAdminTargetAddress);

    memcpy(targetEchoAdminEntryBuff,
	   sourceEchoAdminEntryBuff,
	   sizeof(rttMonEchoAdminEntry_t));

    targetEchoAdminEntryBuff->
	rttMonEchoAdminTargetAddress =
	    holdRttMonEchoAdminTargetAddress;

    return;
} 

static void copyCtrlAdminEntry (rttMonCtrlAdminEntry_t
				* targetCtrlAdminEntryBuff,
				rttMonCtrlAdminEntry_t
				* sourceCtrlAdminEntryBuff)
{
    OctetString *holdRttMonCtrlAdminOwner,
    *holdRttMonCtrlAdminTag,
    *holdRttMonCtrlOperDiagText,
    *holdRttMonLatestRttOperSenseDescription,
    *holdRttMonLatestRttOperAddress;

    holdRttMonCtrlAdminOwner =
	targetCtrlAdminEntryBuff->rttMonCtrlAdminOwner;
    cloneToPreAllocatedOctetString(sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonCtrlAdminOwner,
				   sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminOwner);
    holdRttMonCtrlAdminTag =
	targetCtrlAdminEntryBuff->rttMonCtrlAdminTag;
    cloneToPreAllocatedOctetString(sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonCtrlAdminTag,
				   sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminTag);
    holdRttMonCtrlOperDiagText =
	targetCtrlAdminEntryBuff->rttMonCtrlOperDiagText;
    cloneToPreAllocatedOctetString(sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonCtrlOperDiagText,
				   sourceCtrlAdminEntryBuff->
				   rttMonCtrlOperDiagText);
    holdRttMonLatestRttOperSenseDescription =
	targetCtrlAdminEntryBuff->
	    rttMonLatestRttOperSenseDescription;
#if defined(RTT_SCRIPTS)
    cloneToPreAllocatedOctetString(sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonLatestRttOperSenseDescription,
				   sourceCtrlAdminEntryBuff->
				   rttMonLatestRttOperSenseDescription);
#else
    holdRttMonLatestRttOperSenseDescription->octet_ptr[0] =
	0;
    holdRttMonLatestRttOperSenseDescription->length = 0;
#endif
    holdRttMonLatestRttOperAddress =
	targetCtrlAdminEntryBuff->rttMonLatestRttOperAddress;
    cloneToPreAllocatedOctetString(sourceCtrlAdminEntryBuff->
				   rttMonCtrlAdminIndex,
				   holdRttMonLatestRttOperAddress,
				   sourceCtrlAdminEntryBuff->
				   rttMonLatestRttOperAddress);

    memcpy(targetCtrlAdminEntryBuff,
	   sourceCtrlAdminEntryBuff,
	   sizeof(rttMonCtrlAdminEntry_t));

    targetCtrlAdminEntryBuff->rttMonCtrlAdminOwner =
	holdRttMonCtrlAdminOwner;
    targetCtrlAdminEntryBuff->rttMonCtrlAdminTag =
	holdRttMonCtrlAdminTag;
    targetCtrlAdminEntryBuff->rttMonCtrlOperDiagText =
	holdRttMonCtrlOperDiagText;
    targetCtrlAdminEntryBuff->
	rttMonLatestRttOperSenseDescription =
	    holdRttMonLatestRttOperSenseDescription;
    targetCtrlAdminEntryBuff->rttMonLatestRttOperAddress =
	holdRttMonLatestRttOperAddress;

    return;
} 

static rttMonCtrlAdminQItem *findRttMonCtrlAdminQItem (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem,
    *tmpAdminQItemNext,
    *startAdminQItem;
    boolean found = FALSE;

    if ((baseRttMonCtrlAdminQHead.
	 lastSearchedCtrlAdminQItem != NULL) &&
	(baseRttMonCtrlAdminQHead.
	 lastSearchedCtrlAdminQItem->
	 ctrlAdminEntry->rttMonCtrlAdminIndex ==
	 rttMonCtrlAdminIndex)) {
	return (baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem);
    } else if ((baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem != NULL) &&
	       (baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonCtrlAdminIndex <
		rttMonCtrlAdminIndex)) {
	startAdminQItem =
	    baseRttMonCtrlAdminQHead.lastSearchedCtrlAdminQItem;
    } else {
	startAdminQItem =
	    baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ.qhead;
    } 
    for (tmpAdminQItem = startAdminQItem;
	 tmpAdminQItem != NULL;
	 tmpAdminQItem = tmpAdminQItemNext) {
	tmpAdminQItemNext = tmpAdminQItem->next;
	if (tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminIndex ==
	    rttMonCtrlAdminIndex) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	baseRttMonCtrlAdminQHead.lastSearchedCtrlAdminQItem =
	    tmpAdminQItem;
	return (tmpAdminQItem);
    } else {
	return (NULL);
    }
} 

static rttMonCtrlAdminQItem *findNextRttMonCtrlAdminQItem (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem, *tmpAdminQItemNext, *startAdminQItem;
    boolean found = FALSE;

    if ((baseRttMonCtrlAdminQHead.
	 lastSearchedCtrlAdminQItem != NULL) &&
	(baseRttMonCtrlAdminQHead.
	 lastSearchedCtrlAdminQItem->
	 ctrlAdminEntry->
	 rttMonCtrlAdminIndex ==
	 rttMonCtrlAdminIndex)) {
	return (baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem);
    } else if ((baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem != NULL) &&
	       (baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonCtrlAdminIndex <
		rttMonCtrlAdminIndex)) {
	startAdminQItem =
	    baseRttMonCtrlAdminQHead.lastSearchedCtrlAdminQItem;
    } else {
	startAdminQItem =
	    baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ.qhead;
    } 
    for (tmpAdminQItem = startAdminQItem;
	 tmpAdminQItem != NULL;
	 tmpAdminQItem = tmpAdminQItemNext) {
	tmpAdminQItemNext = tmpAdminQItem->next;
	if (tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminIndex >=
	    rttMonCtrlAdminIndex) {
	    found = TRUE;
	    break;
	}
    } 
    if (found == TRUE) {
	baseRttMonCtrlAdminQHead.lastSearchedCtrlAdminQItem =
	    tmpAdminQItem;
	return (tmpAdminQItem);
    } else {
	return (NULL);
    }
} 

void initBaseRttMonCtrlAdminQ ()
{
    queue_init(&(baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ), 0);
} 

void freeRttMonCtrlAdminEntry (rttMonCtrlAdminEntry_t
			       * ctrlAdminEntry)
{
    if (ctrlAdminEntry == NULL) {
	return;
    }
    if (ctrlAdminEntry->rttMonCtrlAdminOwner != NULL) {
	rttMonFreeOctetStringBuffer(ctrlAdminEntry->
				    rttMonCtrlAdminOwner);
    }
    if (ctrlAdminEntry->rttMonCtrlAdminTag != NULL) {
	rttMonFreeOctetStringBuffer(ctrlAdminEntry->
				    rttMonCtrlAdminTag);
    }
    if (ctrlAdminEntry->rttMonCtrlOperDiagText != NULL) {
	rttMonFreeOctetStringBuffer(ctrlAdminEntry->
				    rttMonCtrlOperDiagText);
    }
    if (ctrlAdminEntry->rttMonLatestRttOperSenseDescription != NULL) {
	rttMonFreeOctetStringBuffer(ctrlAdminEntry->
				    rttMonLatestRttOperSenseDescription);
    }
    if (ctrlAdminEntry->rttMonLatestRttOperAddress != NULL) {
	rttMonFreeOctetStringBuffer(ctrlAdminEntry->
				    rttMonLatestRttOperAddress);
    }
    free(ctrlAdminEntry);

    return;

} 

static rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntryParent (long rttMonCtrlAdminIndex,
							      long *rttMallocOctetsAllocated,
							      long protocolType)
{
    rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntry;

    newRttMonCtrlAdminEntry =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(rttMonCtrlAdminEntry_t));
    if (newRttMonCtrlAdminEntry == NULL) {
	return (newRttMonCtrlAdminEntry);
    }
    memset(newRttMonCtrlAdminEntry, 0,
	   sizeof(rttMonCtrlAdminEntry_t));

    newRttMonCtrlAdminEntry->rttMonCtrlAdminOwner =
	preMallocOctetString(rttMonCtrlAdminIndex,
			     MAX_rttMonCtrlAdminOwner);
    if (newRttMonCtrlAdminEntry->rttMonCtrlAdminOwner ==
	NULL) {
	freeRttMonCtrlAdminEntry(newRttMonCtrlAdminEntry);
	return (newRttMonCtrlAdminEntry);
    }
    newRttMonCtrlAdminEntry->rttMonCtrlAdminTag =
	preMallocOctetString(rttMonCtrlAdminIndex,
			     MAX_rttMonCtrlAdminTag);
    if (newRttMonCtrlAdminEntry->rttMonCtrlAdminTag ==
	NULL) {
	freeRttMonCtrlAdminEntry(newRttMonCtrlAdminEntry);
	return (newRttMonCtrlAdminEntry);
    }
    newRttMonCtrlAdminEntry->rttMonCtrlOperDiagText =
	preMallocOctetString(rttMonCtrlAdminIndex,
			     MAX_rttMonCtrlOperDiagText);
    if (newRttMonCtrlAdminEntry->rttMonCtrlOperDiagText ==
	NULL) {
	freeRttMonCtrlAdminEntry(newRttMonCtrlAdminEntry);
	return (newRttMonCtrlAdminEntry);
    }
    newRttMonCtrlAdminEntry->
	rttMonLatestRttOperSenseDescription =
	    preMallocOctetString(rttMonCtrlAdminIndex,
				 MAX_rttMonLatestRttOperSenseDescription);
    if (newRttMonCtrlAdminEntry->
	rttMonLatestRttOperSenseDescription == NULL) {
	freeRttMonCtrlAdminEntry(newRttMonCtrlAdminEntry);
	return (newRttMonCtrlAdminEntry);
    }
    newRttMonCtrlAdminEntry->rttMonLatestRttOperAddress =
	preMallocOctetString(rttMonCtrlAdminIndex,
			     getMaxRttMonAddressSize(protocolType));
    if (newRttMonCtrlAdminEntry->rttMonLatestRttOperAddress ==
	NULL) {
	freeRttMonCtrlAdminEntry(newRttMonCtrlAdminEntry);
	return (newRttMonCtrlAdminEntry);
    }
    if (rttMonCtrlAdminIndex != 0) {
	*rttMallocOctetsAllocated +=
	    sizeof(rttMonCtrlAdminEntry_t);
	*rttMallocOctetsAllocated +=
	    MAX_rttMonCtrlAdminOwner;
	*rttMallocOctetsAllocated +=
	    MAX_rttMonCtrlAdminTag;
	*rttMallocOctetsAllocated +=
	    MAX_rttMonCtrlOperDiagText;
#if defined(RTT_SCRIPTS)
	*rttMallocOctetsAllocated +=
	    MAX_rttMonLatestRttOperSenseDescription;
#endif
	*rttMallocOctetsAllocated +=
	    getMaxRttMonAddressSize(protocolType);
    }
    return (newRttMonCtrlAdminEntry);

} 

rttMonCtrlAdminEntry_t *newRttMonCtrlAdminEntry ()
{
    return (newRttMonCtrlAdminEntryParent(0, NULL,
					  0));
} 

void freeRttMonEchoAdminEntry (rttMonEchoAdminEntry_t
			       * echoAdminEntry)
{
    if (echoAdminEntry == NULL) {
	return;
    }
    if (echoAdminEntry->rttMonEchoAdminTargetAddress != NULL) {
	rttMonFreeOctetStringBuffer(echoAdminEntry->
				    rttMonEchoAdminTargetAddress);
    }
    free(echoAdminEntry);

    return;

} 

static rttMonEchoAdminEntry_t *newRttMonEchoAdminEntryParent (long rttMonCtrlAdminIndex,
							      long
							      *rttMallocOctetsAllocated,
							      long
							      protocolType)
{
    rttMonEchoAdminEntry_t *newRttMonEchoAdminEntry;

    newRttMonEchoAdminEntry =
	rttMalloc(rttMonCtrlAdminIndex,
		  sizeof(rttMonEchoAdminEntry_t));
    if (newRttMonEchoAdminEntry == NULL) {
	return (newRttMonEchoAdminEntry);
    }
    memset(newRttMonEchoAdminEntry, 0,
	   sizeof(rttMonEchoAdminEntry_t));

    newRttMonEchoAdminEntry->
	rttMonEchoAdminTargetAddress =
	    preMallocOctetString(rttMonCtrlAdminIndex,
				 getMaxRttMonAddressSize(protocolType));
    if (newRttMonEchoAdminEntry->
	rttMonEchoAdminTargetAddress == NULL) {
	freeRttMonEchoAdminEntry(newRttMonEchoAdminEntry);
	return (newRttMonEchoAdminEntry);
    }
    if (rttMonCtrlAdminIndex != 0) {
	*rttMallocOctetsAllocated +=
	    sizeof(rttMonEchoAdminEntry_t);
	*rttMallocOctetsAllocated +=
	    getMaxRttMonAddressSize(protocolType);
    }
    return (newRttMonEchoAdminEntry);
} 

rttMonEchoAdminEntry_t *newRttMonEchoAdminEntry ()
{
    return (newRttMonEchoAdminEntryParent(0,
					  NULL, 0));
} 

static void freeRttMonFileIOAdminEntry (rttMonFileIOAdminEntry_t
					* fileIOAdminEntry)
{
    if (fileIOAdminEntry == NULL) {
	return;
    }
    if (fileIOAdminEntry != NULL) {
	rttMonFreeOctetStringBuffer(fileIOAdminEntry->
				    rttMonFileIOAdminFilePath);
    }
    free(fileIOAdminEntry);

    return;
} 

static void freeRttMonScriptAdminEntry (rttMonScriptAdminEntry_t
					* scriptAdminEntry)
{
    if (scriptAdminEntry == NULL) {
	return;
    }
    if (scriptAdminEntry->rttMonScriptAdminName != NULL) {
	rttMonFreeOctetStringBuffer(scriptAdminEntry->
				    rttMonScriptAdminName);
    }
    if (scriptAdminEntry->rttMonScriptAdminCmdLineParams != NULL) {
	rttMonFreeOctetStringBuffer(scriptAdminEntry->
				    rttMonScriptAdminCmdLineParams);
    }
    free(scriptAdminEntry);

    return;

} 

static void freeRttMonCtrlAdminQItemMem (rttMonCtrlAdminQItem
					 * rttMonCtrlAdminQItem)
{
    long rttMonCtrlAdminIndex;

    if (rttMonCtrlAdminQItem == NULL) {
	return;
    }
    rttMonCtrlAdminIndex =
	rttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlAdminIndex;

    if (rttMonCtrlAdminQItem->next != NULL) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonCtrlAdminQItem->next is not NULL");
    }
    if (rttMonCtrlAdminQItem->ctrlAdminEntry != NULL) {
	freeRttMonCtrlAdminEntry(rttMonCtrlAdminQItem->
				 ctrlAdminEntry);
    }
    if (rttMonCtrlAdminQItem->echoAdminEntry != NULL) {
	freeRttMonEchoAdminEntry(rttMonCtrlAdminQItem->
				 echoAdminEntry);
    }
    if (rttMonCtrlAdminQItem->fileIOAdminEntry != NULL) {
	freeRttMonFileIOAdminEntry(rttMonCtrlAdminQItem->
				   fileIOAdminEntry);
    }
    if (rttMonCtrlAdminQItem->scriptAdminEntry != NULL) {
	freeRttMonScriptAdminEntry(rttMonCtrlAdminQItem->
				   scriptAdminEntry);
    }
    if (rttMonCtrlAdminQItem->reactTriggerAdminQHead != NULL) {
	freeRttMonReactTriggerAdminQ(rttMonCtrlAdminQItem->
				     reactTriggerAdminQHead);
    }
    if (rttMonCtrlAdminQItem->statsCaptureStartTimeQHead
	!= NULL) {
	freeRttMonStatsCaptureStartTimeQ(rttMonCtrlAdminQItem->
					 ctrlAdminEntry->
					 rttMonCtrlAdminIndex,
					 rttMonCtrlAdminQItem->
					 statsCaptureStartTimeQHead);
    }
    if (rttMonCtrlAdminQItem->historyCollectionLifeQHead
	!= NULL) {
	freeRttMonHistoryCollectionLifeQ(rttMonCtrlAdminQItem->
					 ctrlAdminEntry->
					 rttMonCtrlAdminIndex,
					 rttMonCtrlAdminQItem->
					 historyCollectionLifeQHead);
    }
    /* free path list */ 
    if (rttMonCtrlAdminQItem->tempPath != NULL) {
	freePathList(rttMonCtrlAdminQItem->tempPath);
    }
    /* free connection list */ 
    if (rttMonCtrlAdminQItem->connHandleList != NULL) {
	rttMonFreeConnectionHandleList(rttMonCtrlAdminQItem->
				       ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       rttMonCtrlAdminQItem->
				       connHandleList);
    }
    if (rttMonCtrlAdminQItem->pid != 0) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonCtrlAdminQItem->pid is not 0");
    }
    if (rttMonCtrlAdminQItem->inTheProcessOfDying != FALSE) {
	dumpRttMonProcessHeaderString(rttMonCtrlAdminIndex,
				      "rttMonCtrlAdminQItem->inTheProcessOfDying is not FALSE");
    }
    if (rttMonCtrlAdminQItem->pendingBoolean != NULL) {
	delete_watched_boolean(&(rttMonCtrlAdminQItem->pendingBoolean));
    }
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer Stop (ageoutTimer)");
	RTT_BUGINF3(0,
		    "\t in freeRttMonCtrlAdminQItemMem for index %u\n",
		    rttMonCtrlAdminIndex);
    }
    mgd_timer_stop(&(rttMonCtrlAdminQItem->ageoutTimer));

    free(rttMonCtrlAdminQItem);

} 

static int freeRttMonCtrlAdminQItemByIndexInternal (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    tmpAdminQItem = 
	findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
    if (tmpAdminQItem != NULL) {
	unqueue(&(baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ),
		tmpAdminQItem);
	tmpAdminQItem->next = NULL;
	if ((baseRttMonCtrlAdminQHead.
	     lastSearchedCtrlAdminQItem != NULL) &&
	    (baseRttMonCtrlAdminQHead.
	     lastSearchedCtrlAdminQItem ==
	     tmpAdminQItem)) {
	    baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem = NULL;
	}
	freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	return_code = D_Return_Successfull;
    } else {
	return_code = D_InvalidParameter;
    } 
    return (return_code);
} 

int freeRttMonCtrlAdminQItemByIndex (long rttMonCtrlAdminIndex)
{
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	return_code =
	    freeRttMonCtrlAdminQItemByIndexInternal(rttMonCtrlAdminIndex);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
} 

int freeRttMonCtrlAdminQItem (rttMonCtrlAdminQItem
			      * inRttMonCtrlAdminQItem)
{
    if ((inRttMonCtrlAdminQItem == NULL) ||
	(inRttMonCtrlAdminQItem->ctrlAdminEntry
	 == NULL)) {
	return (D_InvalidParameter);
    }
    return (freeRttMonCtrlAdminQItemByIndex(inRttMonCtrlAdminQItem->
					    ctrlAdminEntry->
					    rttMonCtrlAdminIndex));
}				
/*
 *  NOTES:
 *  The following MUST be valid:
 *  rttMonCtrlAdminQItem->ctrlAdminEntry
 */ 

static int rttMonCtrlAdminQinsert (rttMonCtrlAdminQItem
				   * inRttMonCtrlAdminQItem)
{

    long tmpCtrlAdminIndex;
    rttMonCtrlAdminQItem *prevItem;
    rttMonCtrlAdminQItem *currItem;

    if (inRttMonCtrlAdminQItem == NULL) {
	return (D_InvalidParameter);
    }
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry == NULL) {
	return (D_DataFieldIsInvalid);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpCtrlAdminIndex =
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonCtrlAdminIndex;
	prevItem = NULL;
	currItem = baseRttMonCtrlAdminQHead.
	    rttMonCtrlAdminQ.qhead;
	while (currItem != NULL) {
	    if (tmpCtrlAdminIndex < currItem->ctrlAdminEntry->
		rttMonCtrlAdminIndex) {
		break;
	    }
	    prevItem = currItem;
	    currItem = currItem->next;
	} 
	insqueue(&(baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ),
		 inRttMonCtrlAdminQItem,
		 prevItem);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (D_Return_Successfull);

} 

static pathList *newTempPathList (rttMonCtrlAdminEntry_t
				  * ctrlAdminEntry,
				  long
				  *rttMallocOctetsAllocated,
				  long
				  protocolType)
{
    long i;
    long initialHopSize;
    pathList *lastPathListItem, *newPathListItem, *returnQHead;

    lastPathListItem = NULL;
    if (ctrlAdminEntry->rttMonStatisticsAdminNumHops >
	ctrlAdminEntry->
	rttMonHistoryAdminNumSamples) {
	initialHopSize =
	    ctrlAdminEntry->rttMonStatisticsAdminNumHops;
    } else {
	initialHopSize =
	    ctrlAdminEntry->rttMonHistoryAdminNumSamples;
    } 
    returnQHead = NULL;

    for (i = 0; i < initialHopSize; i++) {
	newPathListItem =
	    newRttMonPathListItem(
				  ctrlAdminEntry,
				  rttMallocOctetsAllocated,
				  protocolType);
	if (newPathListItem == NULL) {
	    freePathList(returnQHead);
	    returnQHead = NULL;
	    return (returnQHead);
	}
	if (returnQHead == NULL) {
	    returnQHead = newPathListItem;
	} else {
	    lastPathListItem->next = newPathListItem;
	} 
	lastPathListItem = newPathListItem;

    } 
    return (returnQHead);
} 

static void updateModificationTime (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem)
{
    unsigned long currentTime;

    currentTime = snmp_sysUpTime();

    inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperModificationTime =
	    currentTime;
    updateRttMonApplTimeOfLastSet(currentTime);
}				

/* 
 *  The following values must be valid:
 *   ctrlAdminEntry:
 *     rttMonCtrlAdminIndex
 *     rttMonCtrlAdminRttType
 *   specificAdminEntryBuff:
 *     rttMonCtrlAdminIndex
 */
 
int newRttMonCtrlAdminQItem (rttMonCtrlAdminEntry_t
			     * ctrlAdminEntryBuff,
			     void
			     *specificAdminEntryBuff)
{
    long protocolType;
    rttMonCtrlAdminQItem *newRttMonCtrlAdminQItem = NULL;
    long rttMallocOctetsAllocated = 0;

    protocolType =
	D_rttMonApplSupportedProtocols_notApplicable;

    if ((ctrlAdminEntryBuff == NULL) ||
	(specificAdminEntryBuff == NULL)) {
	return (D_InvalidParameter);
    }
    /* Create the new entry */ 
    newRttMonCtrlAdminQItem =
	rttMalloc(ctrlAdminEntryBuff->
		  rttMonCtrlAdminIndex,
		  sizeof(rttMonCtrlAdminQItem));
    if (newRttMonCtrlAdminQItem == NULL) {
	return (D_MemoryAllocationFailed);
    }
    memset(newRttMonCtrlAdminQItem, 0,
	   sizeof(rttMonCtrlAdminQItem));
    newRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
    newRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
    newRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
    newRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;

    if ((ctrlAdminEntryBuff->rttMonCtrlAdminRttType ==
	 D_rttMonCtrlAdminRttType_echo) ||
	(ctrlAdminEntryBuff->rttMonCtrlAdminRttType ==
	 D_rttMonCtrlAdminRttType_pathEcho)) {
	protocolType = ((rttMonEchoAdminEntry_t *)
			specificAdminEntryBuff)->
			    rttMonEchoAdminProtocol;
	newRttMonCtrlAdminQItem->
	    echoAdminEntry =
		newRttMonEchoAdminEntryParent(ctrlAdminEntryBuff->
					      rttMonCtrlAdminIndex,
					      &rttMallocOctetsAllocated,
					      protocolType);
	if (newRttMonCtrlAdminQItem->
	    echoAdminEntry == NULL) {
	    freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	    return (D_MemoryAllocationFailed);
	}
	copyEchoAdminEntry(newRttMonCtrlAdminQItem->
			   echoAdminEntry,
			   (rttMonEchoAdminEntry_t *) specificAdminEntryBuff);
    } else if (ctrlAdminEntryBuff->rttMonCtrlAdminRttType ==
	       D_rttMonCtrlAdminRttType_fileIO) {
	/*
	 *  Currently Not Supported
	 *  copyFileIOAdminEntry(newRttMonCtrlAdminQItem->
	 *  FileIOAdminEntry,
	 *  (rttMonFileIOAdminEntry_t *)specificAdminEntryBuff);
	 *  Create a dummy echo entry and set the targetAddress to
	 *  point to FileIOAdminEntry ?? address
	 */
	dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
				      rttMonCtrlAdminIndex,
				      "Illegal rttMonCtrlAdminRttType (fileIO)");
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_DataFieldIsInvalid);
    } else if (ctrlAdminEntryBuff->rttMonCtrlAdminRttType ==
	       D_rttMonCtrlAdminRttType_script) {
	/*
	 *  Currently Not Supported
	 *  copyScriptAdminEntry(newRttMonCtrlAdminQItem->
	 *  ScriptAdminEntry, 
	 *  (rttMonScriptAdminEntry_t *)specificAdminEntryBuff);
	 *  Create a dummy echo entry and set the targetAddress to
	 *  point to ScriptAdminEntry ?? address
	 */
	dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
				      rttMonCtrlAdminIndex,
				      "Illegal rttMonCtrlAdminRttType (script)");
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_DataFieldIsInvalid);
    } else {
	dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
				      rttMonCtrlAdminIndex,
				      "Illegal rttMonCtrlAdminRttType (unknown)");
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_DataFieldIsInvalid);
    } 
    newRttMonCtrlAdminQItem->
	ctrlAdminEntry =
	    newRttMonCtrlAdminEntryParent(ctrlAdminEntryBuff->
					  rttMonCtrlAdminIndex,
					  &rttMallocOctetsAllocated,
					  protocolType);
    if (newRttMonCtrlAdminQItem->
	ctrlAdminEntry == NULL) {
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_MemoryAllocationFailed);
    }
    copyCtrlAdminEntry(newRttMonCtrlAdminQItem->
		       ctrlAdminEntry,
		       ctrlAdminEntryBuff);

    if (newRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlAdminRttType ==
	D_rttMonCtrlAdminRttType_echo) {
	if (newRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonStatisticsAdminNumPaths != 1) {
	    dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
					  rttMonCtrlAdminIndex,
					  "rttMonStatisticsAdminNumPaths should be 1, adjusting");
	    newRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonStatisticsAdminNumPaths = 1;
	}
	if (newRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonStatisticsAdminNumHops != 1) {
	    dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
					  rttMonCtrlAdminIndex,
					  "rttMonStatisticsAdminNumHops should be 1, adjusting");
	    newRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonStatisticsAdminNumHops = 1;
	}
	if (newRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonHistoryAdminNumSamples != 1) {
	    dumpRttMonProcessHeaderString(ctrlAdminEntryBuff->
					  rttMonCtrlAdminIndex,
					  "rttMonHistoryAdminNumSamples should be 1, adjusting");
	    newRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonHistoryAdminNumSamples = 1;
	}
    }
    /* pass the newly created and copied to data areas */ 
    newRttMonCtrlAdminQItem->reactTriggerAdminQHead =
	newRttMonReactTriggerAdminQ(newRttMonCtrlAdminQItem->
				    ctrlAdminEntry->rttMonCtrlAdminIndex,
				    &rttMallocOctetsAllocated);
    if (newRttMonCtrlAdminQItem->reactTriggerAdminQHead == NULL) {
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_MemoryAllocationFailed);
    }
    newRttMonCtrlAdminQItem->pendingBoolean =
	create_watched_boolean("RTR Probe Wakeup",
			       newRttMonCtrlAdminQItem->
			       ctrlAdminEntry->
			       rttMonCtrlAdminIndex);
    if (newRttMonCtrlAdminQItem->pendingBoolean == NULL) {
	freeRttMonCtrlAdminQItemMem(newRttMonCtrlAdminQItem);
	return (D_MemoryAllocationFailed);
    }
    process_set_boolean(newRttMonCtrlAdminQItem->
			pendingBoolean, FALSE);

    rttMallocOctetsAllocated +=
	sizeof(rttMonCtrlAdminQItem);

    updateOctetsInUse(newRttMonCtrlAdminQItem->ctrlAdminEntry,
		      rttMallocOctetsAllocated);

    updateModificationTime(newRttMonCtrlAdminQItem);

    /*
     * Start the Scheduler.  The call will make sure
     *   it is not started many times.
     */
    start_rtt_scheduler();

    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer init leaf (ageoutMaster)");
	RTT_BUGINF3(0,
		    "\t in newRttMonCtrlAdminQItem for index %u\n",
		    newRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    mgd_timer_init_leaf(&(newRttMonCtrlAdminQItem->
			  ageoutTimer),
			&(baseRttMonCtrlAdminQHead.ageoutMaster),
			0, NULL, FALSE);

    if (newRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonScheduleAdminConceptRowAgeout != 0) {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start (ageoutTimer)");
	    RTT_BUGINF4(0,
			"\t in newRttMonCtrlAdminQItem for index %u,\n\t\t set to %u (milliseconds)\n",
			newRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			newRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonScheduleAdminConceptRowAgeout * 1000L);
	}
	mgd_timer_start(&(newRttMonCtrlAdminQItem->
			  ageoutTimer),
			newRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonScheduleAdminConceptRowAgeout * 1000L);
    } else {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start skipped (ageoutTimer = 0)");
	}
    } 
    return (rttMonCtrlAdminQinsert(newRttMonCtrlAdminQItem));

} 

static int createRttMonCtrlAdminQItemSecondaryMemoryInternal (rttMonCtrlAdminQItem
							      * tmpAdminQItem)
{
    long rttMallocOctetsAllocated = 0;

    /* Check to see if secondary memory already exists */
    if ((tmpAdminQItem->statsCaptureStartTimeQHead != NULL) &&
	(tmpAdminQItem->historyCollectionLifeQHead != NULL) &&
	(tmpAdminQItem->tempPath != NULL) &&
	(tmpAdminQItem->connHandleList != NULL)) {
	return (D_Return_Successfull);
    } else {
	if (tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminRttType ==
	    D_rttMonCtrlAdminRttType_echo) {
	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonStatisticsAdminNumPaths != 1) {
		dumpRttMonProcessHeaderString(tmpAdminQItem->ctrlAdminEntry->
					      rttMonCtrlAdminIndex,
					      "rttMonStatisticsAdminNumPaths should be 1, adjusting");
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonStatisticsAdminNumPaths = 1;
	    }
	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonStatisticsAdminNumHops != 1) {
		dumpRttMonProcessHeaderString(tmpAdminQItem->ctrlAdminEntry->
					      rttMonCtrlAdminIndex,
					      "rttMonStatisticsAdminNumHops should be 1, adjusting");
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonStatisticsAdminNumHops = 1;
	    }
	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonHistoryAdminNumSamples != 1) {
		dumpRttMonProcessHeaderString(tmpAdminQItem->ctrlAdminEntry->
					      rttMonCtrlAdminIndex,
					      "rttMonHistoryAdminNumSamples should be 1, adjusting");
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonHistoryAdminNumSamples = 1;
	    }
	}
	if (tmpAdminQItem->statsCaptureStartTimeQHead != NULL) {
	    freeRttMonStatsCaptureStartTimeQ(tmpAdminQItem->
					     ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     tmpAdminQItem->
					     statsCaptureStartTimeQHead);
	    dumpRttMonProcessHeaderString(tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "StartTimeQHead Not NULL");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t in createRttMonCtrlAdminQItemSecondaryMemoryInternal\n");
	}
	tmpAdminQItem->statsCaptureStartTimeQHead =
	    newRttMonStatsCaptureStartTimeQ(tmpAdminQItem->
					    ctrlAdminEntry,
					    tmpAdminQItem->
					    echoAdminEntry,
					    &rttMallocOctetsAllocated,
					    tmpAdminQItem->
					    echoAdminEntry->
					    rttMonEchoAdminProtocol);
	if (tmpAdminQItem->
	    statsCaptureStartTimeQHead ==
	    NULL) {
	    freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    return (D_MemoryAllocationFailed);
	}
	if (tmpAdminQItem->historyCollectionLifeQHead != NULL) {
	    freeRttMonHistoryCollectionLifeQ(tmpAdminQItem->
					     ctrlAdminEntry->
					     rttMonCtrlAdminIndex,
					     tmpAdminQItem->
					     historyCollectionLifeQHead);
	    dumpRttMonProcessHeaderString(tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "historyCollectionLifeQHead Not NULL");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t in createRttMonCtrlAdminQItemSecondaryMemoryInternal\n");
	}
	tmpAdminQItem->historyCollectionLifeQHead =
	    newRttMonHistoryCollectionLifeQ(tmpAdminQItem->
					    ctrlAdminEntry,
					    tmpAdminQItem->
					    echoAdminEntry,
					    &rttMallocOctetsAllocated,
					    tmpAdminQItem->
					    echoAdminEntry->
					    rttMonEchoAdminProtocol);
	if (tmpAdminQItem->historyCollectionLifeQHead ==
	    NULL) {
	    freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    return (D_MemoryAllocationFailed);
	}
	if (tmpAdminQItem->tempPath != NULL) {
	    freePathList(tmpAdminQItem->
			 tempPath);
	    dumpRttMonProcessHeaderString(tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "tempPath Not NULL");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t in createRttMonCtrlAdminQItemSecondaryMemoryInternal\n");
	}
	tmpAdminQItem->tempPath =
	    newTempPathList(tmpAdminQItem->
			    ctrlAdminEntry,
			    &rttMallocOctetsAllocated,
			    tmpAdminQItem->
			    echoAdminEntry->
			    rttMonEchoAdminProtocol);
	if (tmpAdminQItem->tempPath ==
	    NULL) {
	    freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    return (D_MemoryAllocationFailed);
	}
	/* new connection list */ 
	if (tmpAdminQItem->connHandleList != NULL) {
	    rttMonFreeConnectionHandleList(tmpAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   tmpAdminQItem->
					   connHandleList);
	    dumpRttMonProcessHeaderString(tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "connHandleList Not NULL");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t in createRttMonCtrlAdminQItemSecondaryMemoryInternal\n");
	}
	tmpAdminQItem->connHandleList =
	    newConnectionHandleListParent(tmpAdminQItem,
					  tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  &rttMallocOctetsAllocated,
					  tmpAdminQItem->
					  echoAdminEntry->
					  rttMonEchoAdminProtocol);
	if (tmpAdminQItem->connHandleList
	    == NULL) {
	    freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    return (D_MemoryAllocationFailed);
	}
	updateOctetsInUse(tmpAdminQItem->ctrlAdminEntry,
			  rttMallocOctetsAllocated);
    } 
    return (D_Return_Successfull);
} 

int createRttMonCtrlAdminQItemSecondaryMemory (long rttMonCtrlAdminIndex)
{
    int return_code;
    rttMonCtrlAdminQItem *tmpAdminQItem;

    return_code = D_InvalidParameter;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    return_code =
		createRttMonCtrlAdminQItemSecondaryMemoryInternal(tmpAdminQItem);
	} /* else of if (tmpAdminQItem == NULL) */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int rttMonCtrlAdminQItemSecondaryMemory (long rttMonCtrlAdminIndex,
					 boolean * exists)
{
    int return_code;
    rttMonCtrlAdminQItem *tmpAdminQItem;

    return_code = D_InvalidParameter;
    *exists = FALSE;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    if ((tmpAdminQItem->statsCaptureStartTimeQHead != NULL) &&
		(tmpAdminQItem->historyCollectionLifeQHead != NULL) &&
		(tmpAdminQItem->tempPath != NULL) &&
		(tmpAdminQItem->connHandleList != NULL)) {
		*exists = TRUE;
	    }
	    return_code = D_Return_Successfull;
	} /* else of if (tmpAdminQItem == NULL) */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

static void setOperState (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
			  long desiredState)
{
    if ((desiredState != D_rttMonCtrlOperState_active) &&
	(desiredState != D_rttMonCtrlOperState_reset) &&
	(desiredState != D_rttMonCtrlOperState_orderlyStop) &&
	(desiredState != D_rttMonCtrlOperState_immediateStop)) {
	if (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonScheduleAdminConceptRowAgeout != 0) {
	    if (RTT_TRACE(0)) {
		dumpRttMonProcessHeaderString(0, "Timer start (ageoutTimer)");
		RTT_BUGINF4(0,
			    "\t in setOperState for index %u, set to %u (milliseconds)\n",
			    inRttMonCtrlAdminQItem->ctrlAdminEntry->
			    rttMonCtrlAdminIndex,
			    inRttMonCtrlAdminQItem->
			    ctrlAdminEntry->
			    rttMonScheduleAdminConceptRowAgeout * 1000L);
	    }
	    mgd_timer_start(&(inRttMonCtrlAdminQItem->ageoutTimer),
			    inRttMonCtrlAdminQItem->
			    ctrlAdminEntry->
			    rttMonScheduleAdminConceptRowAgeout * 1000L);
	} else {
	    if (RTT_TRACE(0)) {
		dumpRttMonProcessHeaderString(0, "Timer start skipped (ageoutTimer = 0)");
	    }
	}
    } else {
	/*
	 *  For reset, and ..stop's
	 *    we will stop the timer here, but it will get
	 *    restarted once the probe realizes that it should
	 *    do something about these values.  At that point 
	 *    the operState will transition to pending or inactive 
	 *    (restarting this timer).
	 */
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer stop (ageoutTimer)");
	    RTT_BUGINF3(0,
			"\t in setOperState for index %u\n",
			inRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex);
	}
	mgd_timer_stop(&(inRttMonCtrlAdminQItem->ageoutTimer));
    } 
    if (desiredState == D_rttMonCtrlOperState_inactive) {
	/* 
	 * if the process is running let the scheduler know to
	 *   finish it off.
	 */
	if (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonCtrlOperState !=
	    D_rttMonCtrlOperState_inactive) {
	    /* 
	     * Wakeup the scheduler
	     */
	    process_set_boolean(baseRttMonCtrlAdminQHead.aProcessDied,
				TRUE);
	    inRttMonCtrlAdminQItem->inTheProcessOfDying = TRUE;
	}
    } else if (desiredState == D_rttMonCtrlOperState_reset) {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperResetTime = snmp_sysUpTime();
    }
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "OperState Change");
	RTT_BUGINF4(0,
		    "\t in setOperState for index %u, to %s\n",
		    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    getRttMonCtrlOperStateString(desiredState));
    }
    inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	    rttMonCtrlOperState = desiredState;

    return;
}				

/*
 *  New states:
 *      D_rttMonCtrlOperState_transition_not_active
 *      D_rttMonCtrlOperState_transition_to_pending
 *      D_rttMonCtrlOperState_transition_to_inactive
 */
/*  
 *   return codes are:
 */ 
static long transitionOperState (rttMonCtrlAdminQItem
				 * inRttMonCtrlAdminQItem,
				 long desiredState)
{
    long return_code;

    return_code = D_Return_Successfull;

    if (inRttMonCtrlAdminQItem->operTransitionState !=
	D_rttMonCtrlOperState_transition_not_active) {
	return_code = D_OperStateInTransition;
    } else {
	switch (inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonCtrlOperState) {
	case D_rttMonCtrlOperState_pending:
	    setOperState(inRttMonCtrlAdminQItem,
			 desiredState);
	    /* wake up the Probe */
	    process_set_boolean(inRttMonCtrlAdminQItem->
				pendingBoolean, TRUE);
	    break;
	case D_rttMonCtrlOperState_inactive:
	    if (desiredState ==
		D_rttMonCtrlOperState_reset) {
		return_code = D_Illegal_Transition;
	    } else if ((desiredState == D_rttMonCtrlOperState_orderlyStop) ||
		       (desiredState ==
			D_rttMonCtrlOperState_immediateStop)) {
	      /*
	       *  To keep the logic more readable, we do nothing since
	       *   transitioning a probe to stop that is already
	       *   stopped will not do anything, but leave the state
	       *   of an inactive probe not inactive, thus hung.
	       */
	    } else {
		setOperState(inRttMonCtrlAdminQItem,
			     desiredState);
	    } 
	    break;
	case D_rttMonCtrlOperState_active:
	    setOperState(inRttMonCtrlAdminQItem,
			 desiredState);
	    break;
	default:
	    return_code = D_Illegal_Transition;
	}
    } 
    return (return_code);
} 

void freeRttMonCtrlAdminQ ()
{
    rttMonCtrlAdminQItem *tmpAdminQItem,
    *tmpAdminQItemNext;
    int return_code, oldAdminStatus;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	for (tmpAdminQItem = baseRttMonCtrlAdminQHead.
	     rttMonCtrlAdminQ.qhead;
	     tmpAdminQItem != NULL;
	     tmpAdminQItem = tmpAdminQItemNext) {
	    tmpAdminQItemNext = tmpAdminQItem->next;
	    if ((tmpAdminQItem->ctrlAdminEntry->
		 rttMonCtrlOperState !=
		 D_rttMonCtrlOperState_inactive) &&
		tmpAdminQItem->inTheProcessOfDying ==
		FALSE) {
	        oldAdminStatus = tmpAdminQItem->
		  ctrlAdminEntry->rttMonCtrlAdminStatus;
		/* 
		 *  Tell the probe to kill itself and
		 *    have the scheduler clean up.
		 */ 
		tmpAdminQItem->
		    ctrlAdminEntry->rttMonCtrlAdminStatus =
			D_rttMonCtrlAdminStatus_destroy;
		/*
		 *  The probe could be in a transition state,
		 *    thus we need to keep trying until we 
		 *    succeed. 
		 */
		return_code = transitionOperState(tmpAdminQItem,
						  D_rttMonCtrlOperState_immediateStop);
		if (return_code != D_Return_Successfull) {
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminStatus =
		      oldAdminStatus;
		    if (RTT_TRACE(0)) {
		      dumpRttMonProcessHeaderString(0, "Operational State Change Failed");
		      RTT_BUGINF3(0,
				  "\t in freeRttMonCtrlAdminQ for index %u, to immediateStop\n",
				  tmpAdminQItem->ctrlAdminEntry->
				  rttMonCtrlAdminIndex);
		    }
		} else {
		    if (RTT_TRACE(0)) {
		      dumpRttMonProcessHeaderString(0, "AdminStatus Change");
		      RTT_BUGINF3(0,
				  "\t in freeRttMonCtrlAdminQ for index %u, to destroy\n",
				  tmpAdminQItem->ctrlAdminEntry->
				  rttMonCtrlAdminIndex);
		    }
		}
	    } else if (tmpAdminQItem->inTheProcessOfDying ==
		       FALSE) {
		/*
		 *  Just cleanup the inactive entry.
		 */
		unqueue(&(baseRttMonCtrlAdminQHead.
			  rttMonCtrlAdminQ),
			tmpAdminQItem);
		tmpAdminQItem->next = NULL;
		if ((baseRttMonCtrlAdminQHead.
		     lastSearchedCtrlAdminQItem != NULL) &&
		    (baseRttMonCtrlAdminQHead.
		     lastSearchedCtrlAdminQItem ==
		     tmpAdminQItem)) {
		    baseRttMonCtrlAdminQHead.
			lastSearchedCtrlAdminQItem = NULL;
		}
		freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    }
	}
	/* 
	 *  Tell Scheduler to die.
	 */ 
	process_set_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf, TRUE);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;

}


/*
 * getRttMonCtrlAdminQSize
 */
 
int getRttMonCtrlAdminQSize ()
{
    return (QUEUESIZE(&(baseRttMonCtrlAdminQHead.
			rttMonCtrlAdminQ)));
}


/*
 *  getRttMonCtrlAdminEntry
 */ 

int getRttMonCtrlAdminEntry (long rttMonCtrlAdminIndex,
			     rttMonCtrlAdminEntry_t * outCtrlAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    copyCtrlAdminEntry(outCtrlAdminEntryBuff,
			       tmpAdminQItem->ctrlAdminEntry);
	    /*
	     *  Fix up the value of OperRttLife
	     */
	    if ((tmpAdminQItem->actualStartTime != 0) &&
		(tmpAdminQItem->ctrlAdminEntry->
		 rttMonScheduleAdminRttLife !=
		 DEF_rttMonScheduleAdminRttLife_FOREVER)) {
		outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife =
			tmpAdminQItem->
			    ctrlAdminEntry->
				rttMonScheduleAdminRttLife -
				    ((snmp_sysUpTime() -
				      tmpAdminQItem->
				      actualStartTime) / 100L);
		if (outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife < 0) {
		    outCtrlAdminEntryBuff->
			rttMonCtrlOperRttLife = 0;
		}
	    } else if ((tmpAdminQItem->actualStartTime != 0) &&
		       (tmpAdminQItem->ctrlAdminEntry->
			rttMonScheduleAdminRttLife ==
			DEF_rttMonScheduleAdminRttLife_FOREVER)) {
		outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife =
			DEF_rttMonScheduleAdminRttLife_FOREVER;
	    }
	    return_code = D_Return_Successfull;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int getNextRttMonCtrlAdminEntry (long rttMonCtrlAdminIndex,
				 rttMonCtrlAdminEntry_t * outCtrlAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_EntryNotFound;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = findNextRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_EntryNotFound;
	} else {
	    copyCtrlAdminEntry(outCtrlAdminEntryBuff,
			       tmpAdminQItem->ctrlAdminEntry);
	    /*
	     *  Fix up the value of OperRttLife
	     */
	    if ((tmpAdminQItem->actualStartTime != 0) &&
		(tmpAdminQItem->ctrlAdminEntry->
		 rttMonScheduleAdminRttLife !=
		 DEF_rttMonScheduleAdminRttLife_FOREVER)) {
		outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife =
			tmpAdminQItem->
			    ctrlAdminEntry->
				rttMonScheduleAdminRttLife -
				    ((snmp_sysUpTime() -
				      tmpAdminQItem->
				      actualStartTime) / 100L);
		if (outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife < 0) {
		    outCtrlAdminEntryBuff->
			rttMonCtrlOperRttLife = 0;
		}
	    } else if ((tmpAdminQItem->actualStartTime != 0) &&
		       (tmpAdminQItem->ctrlAdminEntry->
			rttMonScheduleAdminRttLife ==
			DEF_rttMonScheduleAdminRttLife_FOREVER)) {
		outCtrlAdminEntryBuff->
		    rttMonCtrlOperRttLife =
			DEF_rttMonScheduleAdminRttLife_FOREVER;
	    }
	    return_code = D_Return_Successfull;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int getRttMonEchoAdminEntry (long rttMonCtrlAdminIndex,
			     rttMonEchoAdminEntry_t * outEchoAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = 
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    copyEchoAdminEntry(outEchoAdminEntryBuff,
			       tmpAdminQItem->echoAdminEntry);
	    return_code = D_Return_Successfull;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int getNextRttMonEchoAdminEntry (long rttMonCtrlAdminIndex,
				 rttMonEchoAdminEntry_t * outEchoAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_EntryNotFound;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem = 
	    findNextRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_EntryNotFound;
	} else {
	    copyEchoAdminEntry(outEchoAdminEntryBuff,
			       tmpAdminQItem->echoAdminEntry);
	    return_code = D_Return_Successfull;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}


/*
 *  getRttMonCtrlAdminQItem
 */ 

rttMonCtrlAdminQItem *getRttMonCtrlAdminQItem (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (tmpAdminQItem);
}


/*
 * updateRttMonCtrlAdminEntry
 */ 

int updateRttMonCtrlAdminEntry (rttMonCtrlAdminEntry_t
				* ctrlAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_Return_Successfull;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(ctrlAdminEntryBuff->
				     rttMonCtrlAdminIndex);
	/* 
	 *  If the AdminStatus is active, then don't
	 *    allow this to update.  For NotInService, let
	 *    SNMP handle that, because it has two states,
	 *    one of which updates are still possible.
	 */
	if ((tmpAdminQItem != NULL) &&
	    (tmpAdminQItem->
	     ctrlAdminEntry->
	     rttMonCtrlAdminStatus !=
	     D_rttMonCtrlAdminStatus_active)) {
	    /* 
	     * Place the values that are not to change into
	     *  the ctrlAdminEntryBuff
	     */
	    cloneToPreAllocatedOctetString(ctrlAdminEntryBuff->rttMonCtrlAdminIndex,
					   tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminOwner,
					   ctrlAdminEntryBuff->rttMonCtrlAdminOwner);
	    cloneToPreAllocatedOctetString(ctrlAdminEntryBuff->rttMonCtrlAdminIndex,
					   tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminTag,
					   ctrlAdminEntryBuff->rttMonCtrlAdminTag);
	    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminRttType =
		ctrlAdminEntryBuff->rttMonCtrlAdminRttType;
	    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminThreshold =
		ctrlAdminEntryBuff->rttMonCtrlAdminThreshold;
	    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminFrequency =
		ctrlAdminEntryBuff->rttMonCtrlAdminFrequency;
	    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminTimeout =
		ctrlAdminEntryBuff->rttMonCtrlAdminTimeout;
	    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminVerifyData =
		ctrlAdminEntryBuff->rttMonCtrlAdminVerifyData;
	    tmpAdminQItem->ctrlAdminEntry->rttMonStatisticsAdminNumHourGroups =
		ctrlAdminEntryBuff->rttMonStatisticsAdminNumHourGroups;
	    tmpAdminQItem->ctrlAdminEntry->rttMonStatisticsAdminNumPaths =
		ctrlAdminEntryBuff->rttMonStatisticsAdminNumPaths;
	    tmpAdminQItem->ctrlAdminEntry->rttMonStatisticsAdminNumHops =
	      ctrlAdminEntryBuff->rttMonStatisticsAdminNumHops;
	    tmpAdminQItem->ctrlAdminEntry->rttMonStatisticsAdminNumDistBuckets =
		ctrlAdminEntryBuff->rttMonStatisticsAdminNumDistBuckets;
	    tmpAdminQItem->ctrlAdminEntry->rttMonStatisticsAdminDistInterval =
		ctrlAdminEntryBuff->rttMonStatisticsAdminDistInterval;
	    tmpAdminQItem->ctrlAdminEntry->rttMonHistoryAdminNumLives =
		ctrlAdminEntryBuff->rttMonHistoryAdminNumLives;
	    tmpAdminQItem->ctrlAdminEntry->rttMonHistoryAdminNumBuckets =
		ctrlAdminEntryBuff->rttMonHistoryAdminNumBuckets;
	    tmpAdminQItem->ctrlAdminEntry->rttMonHistoryAdminNumSamples =
		ctrlAdminEntryBuff->rttMonHistoryAdminNumSamples;
	    tmpAdminQItem->ctrlAdminEntry->rttMonHistoryAdminFilter =
		ctrlAdminEntryBuff->rttMonHistoryAdminFilter;

	    updateModificationTime(tmpAdminQItem);

	    if (tmpAdminQItem->
		ctrlAdminEntry->
		rttMonScheduleAdminConceptRowAgeout != 0) {
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start (ageoutTimer)");
		    RTT_BUGINF4(0,
				"\t in updateRttMonCtrlAdminEntry for index %u,\n\t\t set to %u (milliseconds)\n",
				tmpAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				tmpAdminQItem->
				ctrlAdminEntry->
				rttMonScheduleAdminConceptRowAgeout * 1000L);
		}
		mgd_timer_start(&(tmpAdminQItem->
				  ageoutTimer),
				tmpAdminQItem->
				ctrlAdminEntry->
				rttMonScheduleAdminConceptRowAgeout * 1000L);
	    } else {
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start skipped (ageoutTimer = 0)");
		}
	    } 
	    return_code = D_Return_Successfull;
	} else if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int updateRttMonEchoAdminEntry (long rttMonCtrlAdminIndex,
				rttMonEchoAdminEntry_t * echoAdminEntryBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (rttMonCtrlAdminIndex !=
	echoAdminEntryBuff->rttMonCtrlAdminIndex) {
	return (return_code);
    }
    return_code = D_Return_Successfull;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	/* 
	 *  If the AdminStatus is active, then don't
	 *    allow this to update.  For NotInService, let
	 *    SNMP handle that, because it has two states,
	 *    one of which updates are still possible.
	 */
	if ((tmpAdminQItem != NULL) &&
	    (tmpAdminQItem->
	     ctrlAdminEntry->
	     rttMonCtrlAdminStatus !=
	     D_rttMonCtrlAdminStatus_active)) {
	    copyEchoAdminEntry(tmpAdminQItem->echoAdminEntry,
			       echoAdminEntryBuff);

	    updateModificationTime(tmpAdminQItem);

	    if (tmpAdminQItem->
		ctrlAdminEntry->
		rttMonScheduleAdminConceptRowAgeout != 0) {
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start (ageoutTimer)");
		    RTT_BUGINF4(0,
				"\t in updateRttMonEchoAdminEntry for index %u,\n\t\t set to %u (milliseconds)\n",
				tmpAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				tmpAdminQItem->
				ctrlAdminEntry->
				rttMonScheduleAdminConceptRowAgeout * 1000L);
		}
		mgd_timer_start(&(tmpAdminQItem->
				  ageoutTimer),
				tmpAdminQItem->
				ctrlAdminEntry->
				rttMonScheduleAdminConceptRowAgeout * 1000L);
	    } else {
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start skipped (ageoutTimer = 0)");
		}
	    } 
	    return_code = D_Return_Successfull;
	} else if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

static void startProbe (rttMonCtrlAdminQItem
			* inRttMonCtrlAdminQItem)
{
    sys_timestamp tmp_timestamp;

    /* create Timers */
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer Work in startProbe");
	RTT_BUGINF3(0,
		    "\t for index %u\n",
		    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
	RTT_BUGINF2(0,
		    "\t   init leaf oneHourTimer parent oneHourMaster\n");
	RTT_BUGINF2(0,
		    "\t   init leaf endLifeTimer parent lifeTimeMaster\n");
	RTT_BUGINF2(0,
		    "\t   init leaf startTimer parent master\n");
    }
    mgd_timer_init_leaf(&(inRttMonCtrlAdminQItem->
			  oneHourTimer),
			&(baseRttMonCtrlAdminQHead.oneHourMaster),
			0, NULL, FALSE);
    mgd_timer_init_leaf(&(inRttMonCtrlAdminQItem->
			  endLifeTimer),
			&(baseRttMonCtrlAdminQHead.lifeTimeMaster),
			0, NULL, FALSE);
    mgd_timer_init_leaf(&(inRttMonCtrlAdminQItem->
			  startTimer),
			&(baseRttMonCtrlAdminQHead.master),
			0, NULL, FALSE);


    /* 
     * If it has taken longer time to start the
     *  probe than the start time, start it now
     *  without pending.
     * To check for wrap of the currentTime, we
     *  just need to look at the restartPending
     *  flag.  If the flag is true then start
     *  pending, else see what the current time
     *  has to say.
     *
     * The special Value of 1 (always start active)
     *  will never be set when restartPending is 
     *  true.
     */
    if (inRttMonCtrlAdminQItem->
	restartPending) {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "OperState Change in startProbe");
	    RTT_BUGINF3(0,
			"\t for index %u, to pending\n",
			inRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex);
	}
	/* set the OperState directly so we don't
	 *   reset the ageoutTimer.
	 */ 
	inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlOperState =
		    D_rttMonCtrlOperState_pending;
	/* Since this is a restart or intial start 
	 *  with the special value to start pending, 
	 *  the value of rttMonScheduleAdminRttStartTime
	 *  has expired once or will not expire
	 *  and will not expire again
	 *  so we will not set the expireTime.  
	 *
	 *  The function of restart allow multiple
	 *    triggers to restart the entry.
	 */
    } else if ((snmp_sysUpTime() > inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonScheduleAdminRttStartTime) ||
	       (inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonScheduleAdminRttStartTime == 1)) {
	setOperState(inRttMonCtrlAdminQItem,
		     D_rttMonCtrlOperState_active);
    } else {
	/* The start time has not expired yet.
	 *   So, start pending and set a timer
	 *     to tell the scheduler to activate
	 *     the probe at a later time.
	 */
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "OperState Change in startProbe");
	    RTT_BUGINF3(0,
			"\t for index %u, to pending\n",
			inRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex);
	}
	/* set the OperState directly so we don't
	 *   reset the ageoutTimer.
	 */ 
	inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlOperState =
		    D_rttMonCtrlOperState_pending;
	/* 
	 * This is almost the same as the code to perform 
	 *   mgd_timer_start, with the excption of a call
	 *   to TIMER_START_ABSOLUTE versus TIMER_START.
	 * TIMER_START_ABSOLUTE needs the milliseconds since
	 *   the router booted and not the centiseconds as
	 *   stored in the rttMonScheduleAdminRttStartTime.
	 *   eg 5 sec = 500 csec = 5000 msec
	 */
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer set exptime (startTimer)");
	    RTT_BUGINF4(0,
			"\t in startProbe for index %u, set to %u (milliseconds)\n",
			inRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonScheduleAdminRttStartTime * 10L);
	}
	TIMER_START_ABSOLUTE(tmp_timestamp,
			     (inRttMonCtrlAdminQItem->
			      ctrlAdminEntry->
			      rttMonScheduleAdminRttStartTime * 10L));
	mgd_timer_set_exptime(&(inRttMonCtrlAdminQItem->
				startTimer),
			      &tmp_timestamp);
    } /* set operRttLife to schedulerRttLife */ 
    inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	    rttMonCtrlOperRttLife =
		inRttMonCtrlAdminQItem->
		    ctrlAdminEntry->
			rttMonScheduleAdminRttLife;

    /* 
     * set operTransitionState to 
     * D_rttMonCtrlOperState_transition_not_active
     */
    inRttMonCtrlAdminQItem->
	operTransitionState =
	    D_rttMonCtrlOperState_transition_not_active;

    reg_invoke_rtt_echo_activate(inRttMonCtrlAdminQItem->
				 echoAdminEntry->
				 rttMonEchoAdminProtocol,
				 inRttMonCtrlAdminQItem);

    if (inRttMonCtrlAdminQItem->pid == NO_PROCESS) {
	setOperState(inRttMonCtrlAdminQItem,
		     D_rttMonCtrlOperState_inactive);
    }
    return;
}


/*
 * setRttMonCtrlAdminStatus
 */ 

int setRttMonCtrlAdminStatus (long rttMonCtrlAdminIndex,
			      long rttMonCtrlAdminStatus)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code, oldAdminStatus;
    sys_timestamp tmp_timestamp;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonCtrlAdminStatus !=
		rttMonCtrlAdminStatus) {
		/*
		 * Logic to start task, etc...
		 */
		if ((rttMonCtrlAdminStatus ==
		     D_rttMonCtrlAdminStatus_active) &&
		    (tmpAdminQItem->ctrlAdminEntry->
		     rttMonCtrlOperState ==
		     D_rttMonCtrlOperState_inactive)) {
		    /*
		     * If the special (always run) value is not
		     *   set and startTime is smaller than
		     *   the current upTime, start pending
		     */
		    if ((tmpAdminQItem->
			 ctrlAdminEntry->
			 rttMonScheduleAdminRttStartTime != 1) &&
			(snmp_sysUpTime() > tmpAdminQItem->
			 ctrlAdminEntry->
			 rttMonScheduleAdminRttStartTime)) {
			tmpAdminQItem->
			    restartPending = TRUE;
		    }
		    startProbe(tmpAdminQItem);
		    if (RTT_TRACE(0)) {
			dumpRttMonProcessHeaderString(0, "AdminStatus Change");
			RTT_BUGINF4(0,
				    "\t in setRttMonCtrlAdminStatus for index %u, to %s\n",
				    tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex,
				    getRttMonAdminStatusString(rttMonCtrlAdminStatus));
		    }
		    tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminStatus =
			    rttMonCtrlAdminStatus;
		    updateModificationTime(tmpAdminQItem);
		    return_code = D_Return_Successfull;
		} else if ((rttMonCtrlAdminStatus ==
			    D_rttMonCtrlAdminStatus_active) &&
			   (tmpAdminQItem->ctrlAdminEntry->
			    rttMonCtrlOperState ==
			    D_rttMonCtrlOperState_pending)) {
		    /*  
		     *  If the probe is already running just
		     *    set the timer that the scheduler uses
		     *    to wake the probe to the new start 
		     *    time.  If the probe has not been
		     *    started the timer will be set by
		     *    the probe startup.
		     */

		    /* Start Time of 1 is a special value to 
		     *  be always active.
		     */
		    if (tmpAdminQItem->ctrlAdminEntry->
			rttMonScheduleAdminRttStartTime == 1) {
			transitionOperState(tmpAdminQItem,
					    D_rttMonCtrlOperState_active);
		    } else if (tmpAdminQItem->ctrlAdminEntry->
			       rttMonScheduleAdminRttStartTime >=
			       snmp_sysUpTime()) {
			/* 
			 * This is almost the same as the code to perform 
			 *   mgd_timer_start, with the excption of a call
			 *   to TIMER_START_ABSOLUTE versus TIMER_START
			 * TIMER_START_ABSOLUTE needs the milliseconds since
			 *   the router booted and not the centiseconds as
			 *   stored in the rttMonScheduleAdminRttStartTime.
			 *   eg 5 sec = 500 csec = 5000 msec
			 */
			if (RTT_TRACE(0)) {
			    dumpRttMonProcessHeaderString(0, "Timer set exptime (startTimer)");
			    RTT_BUGINF4(0,
					"\t in setRttMonCtrlAdminStatus for index %u,\n\t\t set to %u (milliseconds)\n",
					tmpAdminQItem->ctrlAdminEntry->
					rttMonCtrlAdminIndex,
					tmpAdminQItem->ctrlAdminEntry->
					rttMonScheduleAdminRttStartTime * 10L);
			}
			TIMER_START_ABSOLUTE(tmp_timestamp,
					     (tmpAdminQItem->ctrlAdminEntry->
					      rttMonScheduleAdminRttStartTime * 10L));
			mgd_timer_set_exptime(&(tmpAdminQItem->
						startTimer),
					      &tmp_timestamp);
		    }
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminStatus =
		      rttMonCtrlAdminStatus;
		} else if (rttMonCtrlAdminStatus ==
			   D_rttMonCtrlAdminStatus_destroy) {
		    /*
		     *  Check for inTheProcessOfDying because
		     *    the probe could have been requested to 
		     *    destroy, and has transitioned to
		     *    inactive, but the scheduler has
		     *    not finished cleaning it up.  The
		     *    scheduler will clean it up, so don't
		     *    do it here.
		     */
		    if ((tmpAdminQItem->ctrlAdminEntry->
			 rttMonCtrlOperState ==
			 D_rttMonCtrlOperState_inactive) &&
			(tmpAdminQItem->inTheProcessOfDying ==
			 FALSE)) {
			/* 
			 * When the probe is not active, the
			 *  row needs to be deleted immediately
			 */
			freeRttMonCtrlAdminQItemByIndexInternal(tmpAdminQItem->ctrlAdminEntry->
								rttMonCtrlAdminIndex);
			/* 
			 * Need to tell the scheduler that we have
			 *   cleaned up one of its processes.  But
			 *   first check to see if the scheduler is
			 *   not in reset, ie the aProcessDied 
			 *   watched boolean is NULL.
			 */
			if (baseRttMonCtrlAdminQHead.aProcessDied !=
			    NULL) {
			    process_set_boolean(baseRttMonCtrlAdminQHead.
						aProcessDied,
						TRUE);
			}
		    } else if (tmpAdminQItem->inTheProcessOfDying ==
			       FALSE) {
			oldAdminStatus = tmpAdminQItem->ctrlAdminEntry->
			  rttMonCtrlAdminStatus;
			/*
			 *  Tell the probe to kill itself and
			 *    the scheduler will clean up the 
			 *    mess.
			 */ 
			tmpAdminQItem->ctrlAdminEntry->
			    rttMonCtrlAdminStatus =
				rttMonCtrlAdminStatus;
			updateModificationTime(tmpAdminQItem);
			return_code = transitionOperState(tmpAdminQItem,
							  D_rttMonCtrlOperState_immediateStop);
			if (return_code != D_Return_Successfull) {
			    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminStatus =
			        oldAdminStatus;
			    if (RTT_TRACE(0)) {
			      dumpRttMonProcessHeaderString(0, "Operational State Change Failed");
			      RTT_BUGINF3(0,
					  "\t in setRttMonCtrlAdminStatus for index %u, to immediateStop\n",
					  tmpAdminQItem->ctrlAdminEntry->
					  rttMonCtrlAdminIndex);
			    }
			} else {
			    if (RTT_TRACE(0)) {
			        dumpRttMonProcessHeaderString(0, "AdminStatus Change");
				RTT_BUGINF4(0,
					    "\t in setRttMonCtrlAdminStatus for index %u, to %s\n",
					    tmpAdminQItem->ctrlAdminEntry->
					    rttMonCtrlAdminIndex,
					    getRttMonAdminStatusString(rttMonCtrlAdminStatus));
			    }
			}
		    }
		} else if (tmpAdminQItem->ctrlAdminEntry->
			   rttMonCtrlOperState !=
			   D_rttMonCtrlOperState_active) {
		    if (RTT_TRACE(0)) {
			dumpRttMonProcessHeaderString(0, "AdminStatus Change");
			RTT_BUGINF4(0,
				    "\t in setRttMonCtrlAdminStatus for index %u, to %s\n",
				    tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex,
				    getRttMonAdminStatusString(rttMonCtrlAdminStatus));
		    }
		    tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminStatus =
			    rttMonCtrlAdminStatus;
		    updateModificationTime(tmpAdminQItem);
		    return_code = D_Return_Successfull;
		} else {
		    return_code = D_BadOperState;
		}
	    } else {
		updateModificationTime(tmpAdminQItem);
		return_code = D_Return_Successfull;
	    }
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonScheduleAdminRttLife (long rttMonCtrlAdminIndex,
				   long rttMonScheduleAdminRttLife)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonScheduleAdminRttLife =
		    rttMonScheduleAdminRttLife;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonScheduleAdminRttStartTime (long rttMonCtrlAdminIndex,
					long rttMonScheduleAdminRttStartTime)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonScheduleAdminRttStartTime =
		    rttMonScheduleAdminRttStartTime;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonScheduleAdminConceptRowAgeout (long rttMonCtrlAdminIndex,
					    long rttMonScheduleAdminConceptRowAgeout)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonCtrlOperState !=
		D_rttMonCtrlOperState_active) {
		if (rttMonScheduleAdminConceptRowAgeout != 0) {
		    /*
		     * RowAgeout is in seconds, so multiply 1000 to
		     *   get milliseconds.
		     */
		    if (RTT_TRACE(0)) {
			dumpRttMonProcessHeaderString(0, "Timer start (ageoutTimer)");
			RTT_BUGINF4(0,
				    "\t in setRttMonScheduleAdminConceptRowAgeout for index %u,\n\t\t set to %u (milliseconds)\n",
				    tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex,
				    rttMonScheduleAdminConceptRowAgeout * 1000L);
		    }
		    mgd_timer_start(&(tmpAdminQItem->
				      ageoutTimer),
				    rttMonScheduleAdminConceptRowAgeout * 1000L);
		} else {
		    if (RTT_TRACE(0)) {
			dumpRttMonProcessHeaderString(0, "Timer start skipped (ageoutTimer = 0)");
		    }
		}
	    }
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonScheduleAdminConceptRowAgeout =
		    rttMonScheduleAdminConceptRowAgeout;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminConnectionEnable (long rttMonCtrlAdminIndex,
					 long rttMonReactAdminConnectionEnable)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminConnectionEnable =
		    rttMonReactAdminConnectionEnable;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminTimeoutEnable (long rttMonCtrlAdminIndex,
				      long rttMonReactAdminTimeoutEnable)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminTimeoutEnable =
		    rttMonReactAdminTimeoutEnable;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminThresholdType (long rttMonCtrlAdminIndex,
				      long rttMonReactAdminThresholdType)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminThresholdType =
		    rttMonReactAdminThresholdType;
	    /* 
	     * Clear all current threshold values and 
	     *   start over 
	     */
	    memset(&(tmpAdminQItem->reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminThresholdFalling (long rttMonCtrlAdminIndex,
					 long rttMonReactAdminThresholdFalling)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminThresholdFalling =
		    rttMonReactAdminThresholdFalling;
	    /* 
	     * Clear all current threshold values and 
	     *   start over 
	     */
	    memset(&(tmpAdminQItem->reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminThresholdCount (long rttMonCtrlAdminIndex,
				       long rttMonReactAdminThresholdCount)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminThresholdCount =
		    rttMonReactAdminThresholdCount;
	    /* 
	     * Clear all current threshold values and 
	     *   start over 
	     */
	    memset(&(tmpAdminQItem->reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminThresholdCount2 (long rttMonCtrlAdminIndex,
					long rttMonReactAdminThresholdCount2)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminThresholdCount2 =
		    rttMonReactAdminThresholdCount2;
	    /* 
	     * Clear all current threshold values and 
	     *   start over 
	     */
	    memset(&(tmpAdminQItem->reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



int setRttMonReactAdminActionTypes (long rttMonCtrlAdminIndex,
				    long rttMonReactAdminActionType)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    tmpAdminQItem->ctrlAdminEntry->
		rttMonReactAdminActionType =
		    rttMonReactAdminActionType;
	    /* 
	     * Clear all current threshold values and 
	     *   start over 
	     */
	    memset(&(tmpAdminQItem->reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    tmpAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    updateModificationTime(tmpAdminQItem);
	    return_code = D_Return_Successfull;
	} else {
	    return_code = D_InvalidParameter;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
} 

long getCurrentFrequency (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    long returnFrequency;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    returnFrequency =
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminFrequency;
	} else {
	    returnFrequency = 0;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (returnFrequency);
} 

long getCurrentOperState (long rttMonCtrlAdminIndex)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    long returnOperState;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    returnOperState =
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlOperState;
	} else {
	    returnOperState =
		D_rttMonCtrlOperState_inactive;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (returnOperState);
}



int setRttMonCtrlOperState (long rttMonCtrlAdminIndex,
			    long rttMonCtrlOperState)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem != NULL) {
	    /*
	     * Perform proper State Change.
	     */
	    if ((rttMonCtrlOperState == D_rttMonCtrlOperState_reset) ||
		(rttMonCtrlOperState == D_rttMonCtrlOperState_orderlyStop) ||
		(rttMonCtrlOperState == D_rttMonCtrlOperState_immediateStop)) {
	        return_code = transitionOperState(tmpAdminQItem,
						  rttMonCtrlOperState);
	        updateModificationTime(tmpAdminQItem);
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}



rttMonStatsCollectEntry_t *newRttMonStatsCollectEntry ()
{
    rttMonStatsCollectEntry_t *newRttMonStatsCollectEntry;

    newRttMonStatsCollectEntry =
	rttMalloc(0,
		  sizeof(rttMonStatsCollectEntry_t));
    if (newRttMonStatsCollectEntry == NULL) {
	return (newRttMonStatsCollectEntry);
    }
    memset(newRttMonStatsCollectEntry, 0,
	   sizeof(rttMonStatsCollectEntry_t));

    newRttMonStatsCollectEntry->rttMonStatsCollectAddress =
	preMallocOctetString(0,
			     getMaxRttMonAddressSize(0));
    if (newRttMonStatsCollectEntry->rttMonStatsCollectAddress ==
	NULL) {
	freeRttMonStatsCollectEntry(newRttMonStatsCollectEntry);
	return (newRttMonStatsCollectEntry);
    }
    return (newRttMonStatsCollectEntry);

}			

/*
 * getRttMonStatsCollectEntry
 */ 

static void copyStatsCollectEntry (long rttMonCtrlAdminIndex,
				   rttMonStatsCollectEntry_t
				   * targetStatsCollectionEntryBuff,
				   short_rttMonStatsCollectEntry_t
				   * sourceShortStatsCollectionEntryBuff)
{
    OctetString *holdRttMonStatsCollectAddress;

    holdRttMonStatsCollectAddress =
	targetStatsCollectionEntryBuff->
	    rttMonStatsCollectAddress;
    cloneToPreAllocatedOctetString(rttMonCtrlAdminIndex,
				   holdRttMonStatsCollectAddress,
				   sourceShortStatsCollectionEntryBuff->
				   rttMonStatsCollectAddress);

    targetStatsCollectionEntryBuff->
	rttMonStatsCollectNumDisconnects =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectNumDisconnects;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectTimeouts =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectTimeouts;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectBusies =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectBusies;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectNoConnections =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectNoConnections;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectDrops =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectDrops;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectSequenceErrors =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectSequenceErrors;
    targetStatsCollectionEntryBuff->
	rttMonStatsCollectVerifyErrors =
	    sourceShortStatsCollectionEntryBuff->
		rttMonStatsCollectVerifyErrors;

    targetStatsCollectionEntryBuff->
	rttMonStatsCollectAddress =
	    holdRttMonStatsCollectAddress;

    return;
} 

int getRttMonStatsCollectEntry (long rttMonCtrlAdminIndex,
				unsigned long rttMonStatsCaptureStartTimeIndex,
				long rttMonStatsCapturePathIndex,
				long rttMonStatsCaptureHopIndex,
				rttMonStatsCollectEntry_t * outCollectBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (outCollectBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else if (QUEUESIZE(&(tmpAdminQItem->
			       statsCaptureStartTimeQHead->
			       rttMonStatsCaptureStartTimeQ)) == 0) {
	    return_code = D_DataNotBeingStored;
	} else {
	    tmpStartTimeQItem =
		findRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
						     statsCaptureStartTimeQHead,
						     rttMonStatsCaptureStartTimeIndex);
	    if (tmpStartTimeQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		tmpPathQItem =
		    findRttMonStatsCapturePathQItem(tmpStartTimeQItem->
						    statsCapturePathQHead,
						    rttMonStatsCapturePathIndex);
		if (tmpPathQItem == NULL) {
		    return_code = D_InvalidParameter;
		} else {
		    tmpHopQItem =
			findRttMonStatsCaptureHopQItem(tmpPathQItem->
						       statsCaptureHopQHead,
						       rttMonStatsCaptureHopIndex);
		    if (tmpHopQItem == NULL) {
			return_code = D_InvalidParameter;
		    } else {
			copyStatsCollectEntry(tmpAdminQItem->
					      ctrlAdminEntry->
					      rttMonCtrlAdminIndex,
					      outCollectBuff,
					      tmpHopQItem->
					      shortStatsCollectEntry);
			outCollectBuff->rttMonCtrlAdminIndex =
			    tmpAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex;
			outCollectBuff->rttMonStatsCaptureStartTimeIndex =
			    tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
			outCollectBuff->rttMonStatsCapturePathIndex =
			    tmpPathQItem->rttMonStatsCapturePathIndex;
			outCollectBuff->rttMonStatsCaptureHopIndex =
			    tmpHopQItem->rttMonStatsCaptureHopIndex;
			return_code = D_Return_Successfull;
		    }
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/*
 *  getNextRttMonStatsCollectEntry
 */ 

int getNextRttMonStatsCollectEntry (long rttMonCtrlAdminIndex,
				    unsigned long rttMonStatsCaptureStartTimeIndex,
				    long rttMonStatsCapturePathIndex,
				    long rttMonStatsCaptureHopIndex,
				    rttMonStatsCollectEntry_t * outCollectBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    int return_code;
    boolean endSearch = FALSE;
    long localRttMonCtrlAdminIndex;
    unsigned long localRttMonStatsCaptureStartTimeIndex;
    long localRttMonStatsCapturePathIndex;
    long localRttMonStatsCaptureHopIndex;

    return_code = D_InvalidParameter;
    if (outCollectBuff == NULL) {
	return (return_code);
    }
    localRttMonCtrlAdminIndex = rttMonCtrlAdminIndex;
    localRttMonStatsCaptureStartTimeIndex =
	rttMonStatsCaptureStartTimeIndex;
    localRttMonStatsCapturePathIndex =
	rttMonStatsCapturePathIndex;
    localRttMonStatsCaptureHopIndex =
	rttMonStatsCaptureHopIndex;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	while (endSearch == FALSE) {
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	    if (tmpAdminQItem == NULL) {
		return_code = D_EntryNotFound;
		endSearch = TRUE;
	    } else {
		/*
		 *  If we have incremented this index, then the other 
		 *  indicies need to start over.
		 */
		if (localRttMonCtrlAdminIndex != 
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex) {
		    localRttMonStatsCaptureStartTimeIndex = 0;
		    localRttMonStatsCapturePathIndex = 0;
		    localRttMonStatsCaptureHopIndex = 0;
		}
		tmpStartTimeQItem =
		    findNextRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
							     statsCaptureStartTimeQHead,
							     localRttMonStatsCaptureStartTimeIndex);
		if (tmpStartTimeQItem == NULL) {
		    /*
		     *  When a table is sparse, continue looking from
		     *  the last found entry.
		     */
		    localRttMonCtrlAdminIndex = tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		    localRttMonCtrlAdminIndex++;
		    localRttMonStatsCaptureStartTimeIndex = 0;
		    localRttMonStatsCapturePathIndex = 0;
		    localRttMonStatsCaptureHopIndex = 0;
		} else {
		    /*
		     *  If we have incremented this index, then the other 
		     *  indicies need to start over.
		     */
		    if (localRttMonStatsCaptureStartTimeIndex !=
			tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex) {
			localRttMonStatsCapturePathIndex = 0;
			localRttMonStatsCaptureHopIndex = 0;
		    }
		    tmpPathQItem =
			findNextRttMonStatsCapturePathQItem(tmpStartTimeQItem->
							    statsCapturePathQHead,
							    localRttMonStatsCapturePathIndex);
		    if (tmpPathQItem == NULL) {
			/*
			 *  When a table is sparse, continue looking from
			 *  the last found entry.
			 */
			localRttMonStatsCaptureStartTimeIndex = tmpStartTimeQItem->
			    rttMonStatsCaptureStartTimeIndex;
			/* I am assuming the Timer will rap as needed */
			localRttMonStatsCaptureStartTimeIndex++;
			localRttMonStatsCapturePathIndex = 0;
			localRttMonStatsCaptureHopIndex = 0;
		    } else {
			/*
			 *  If we have incremented this index, then the other 
			 *  indicies need to start over.
			 */
			if (localRttMonStatsCapturePathIndex !=
			    tmpPathQItem->rttMonStatsCapturePathIndex) {
			    localRttMonStatsCaptureHopIndex = 0;
			}
			tmpHopQItem =
			    findNextRttMonStatsCaptureHopQItem(tmpPathQItem->
							       statsCaptureHopQHead,
							       localRttMonStatsCaptureHopIndex);
			if (tmpHopQItem == NULL) {
			    /*
			     *  When a table is sparse, continue looking from
			     *  the last found entry.
			     */
			    localRttMonStatsCapturePathIndex =
				tmpPathQItem->rttMonStatsCapturePathIndex;
			    localRttMonStatsCapturePathIndex++;
			    localRttMonStatsCaptureHopIndex = 0;
			} else {
			    copyStatsCollectEntry(tmpAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  outCollectBuff,
						  tmpHopQItem->
						  shortStatsCollectEntry);
			    outCollectBuff->rttMonCtrlAdminIndex =
				tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex;
			    outCollectBuff->rttMonStatsCaptureStartTimeIndex =
				tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
			    outCollectBuff->rttMonStatsCapturePathIndex =
				tmpPathQItem->rttMonStatsCapturePathIndex;
			    outCollectBuff->rttMonStatsCaptureHopIndex =
				tmpHopQItem->rttMonStatsCaptureHopIndex;
			    return_code = D_Return_Successfull;
			    endSearch = TRUE;
			}
		    }
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}



void freeRttMonReactTriggerAdminEntry (rttMonReactTriggerAdminEntry_t
				       * reactTriggerAdminEntry)
{
    free(reactTriggerAdminEntry);
} 

rttMonReactTriggerAdminEntry_t *newRttMonReactTriggerAdminEntry ()
{
    rttMonReactTriggerAdminEntry_t *return_buff;

    return_buff = rttMalloc(0,
			    sizeof(rttMonReactTriggerAdminEntry_t));
    return_buff->
	rttMonReactTriggerOperState =
	    D_rttMonReactTriggerOperState_pending;

    return (return_buff);
} 

int addUpdateRttMonReactTriggerAdminEntry (long rttMonCtrlAdminIndex,
					   rttMonReactTriggerAdminEntry_t
					   * inTriggerAdminBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonReactTriggerAdminQItem *tmpReactQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (inTriggerAdminBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    tmpReactQItem =
		findRttMonReactTriggerAdminQItem(tmpAdminQItem->
						 reactTriggerAdminQHead,
						 inTriggerAdminBuff->
						 rttMonReactTriggerAdminRttMonCtrlAdminIndex);
	    if (tmpReactQItem == NULL) {
		tmpReactQItem =
		    getUnusedRttMonReactTriggerAdminQItem(tmpAdminQItem->
							  reactTriggerAdminQHead);
		if (tmpReactQItem == NULL) {
		    return_code = D_QueueFull;
		} else {
		    memcpy(tmpReactQItem->reactTriggerAdminEntry,
			   inTriggerAdminBuff,
			   sizeof(rttMonReactTriggerAdminEntry_t));
		    tmpReactQItem->reactTriggerAdminEntry->
			rttMonReactTriggerOperState =
			    D_rttMonReactTriggerOperState_pending;
		    return_code = 
			updateRttMonReactTriggerAdminQItem(tmpAdminQItem->
							   reactTriggerAdminQHead, 
							   tmpReactQItem);
		}
	    } else {
		memcpy(tmpReactQItem->reactTriggerAdminEntry,
		       inTriggerAdminBuff,
		       sizeof(rttMonReactTriggerAdminEntry_t));
		return_code = updateRttMonReactTriggerAdminQItem(
								 tmpAdminQItem->reactTriggerAdminQHead, tmpReactQItem);
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
} 

int setRttMonReactTriggerAdminStatus (long rttMonCtrlAdminIndex,
				      long rttMonReactTriggerAdminRttMonCtrlAdminIndex,
				      long rttMonReactTriggerAdminStatus)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonReactTriggerAdminQItem *tmpReactQItem;
    int return_code;

    return_code = D_InvalidParameter;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    tmpReactQItem =
		findRttMonReactTriggerAdminQItem(tmpAdminQItem->
						 reactTriggerAdminQHead,
						 rttMonReactTriggerAdminRttMonCtrlAdminIndex);
	    if (tmpReactQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		tmpReactQItem->reactTriggerAdminEntry->
		    rttMonReactTriggerAdminStatus =
			rttMonReactTriggerAdminStatus;
		if (rttMonReactTriggerAdminStatus ==
		    D_rttMonReactTriggerAdminStatus_destroy) {
		    CLR_VALID(I_queue_item_in_use,
			      tmpReactQItem->valid);
		}
		return_code = D_Return_Successfull;
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}				

/*
 * getRttMonReactTriggerAdminEntry
 */ 

int getRttMonReactTriggerAdminEntry (long rttMonCtrlAdminIndex,
				     long rttMonReactTriggerAdminRttMonCtrlAdminIndex,
				     rttMonReactTriggerAdminEntry_t * outTriggerAdminBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonReactTriggerAdminQItem *tmpReactQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (outTriggerAdminBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else {
	    tmpReactQItem =
		findRttMonReactTriggerAdminQItem(tmpAdminQItem->
						 reactTriggerAdminQHead,
						 rttMonReactTriggerAdminRttMonCtrlAdminIndex);
	    if (tmpReactQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		memcpy(outTriggerAdminBuff,
		       tmpReactQItem->reactTriggerAdminEntry,
		       sizeof(rttMonReactTriggerAdminEntry_t));
		outTriggerAdminBuff->rttMonCtrlAdminIndex =
		    tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		return_code = D_Return_Successfull;
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/*
 *  getNextRttMonReactTriggerAdminEntry
 *  
 *  The rttMonReactTriggerAdminRttMonCtrlAdminIndex has already
 *  been incremented.
 * 
 */ 

int getNextRttMonReactTriggerAdminEntry (long rttMonCtrlAdminIndex,
					 long rttMonReactTriggerAdminRttMonCtrlAdminIndex,
					 rttMonReactTriggerAdminEntry_t * outTriggerAdminBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonReactTriggerAdminQItem *tmpReactQItem;
    int return_code;
    boolean endSearch = FALSE;
    long localRttMonCtrlAdminIndex;
    long localRttMonReactTriggerAdminRttMonCtrlAdminIndex;

    return_code = D_InvalidParameter;
    if (outTriggerAdminBuff == NULL) {
	return (return_code);
    }
    localRttMonCtrlAdminIndex = rttMonCtrlAdminIndex;
    localRttMonReactTriggerAdminRttMonCtrlAdminIndex =
	rttMonReactTriggerAdminRttMonCtrlAdminIndex;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	while (endSearch == FALSE) {
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	    if (tmpAdminQItem == NULL) {
		return_code = D_EntryNotFound;
		endSearch = TRUE;
	    } else {
		/*
		 *  If we have incremented this index, then the other 
		 *  indicies need to start over.
		 */
		if (localRttMonCtrlAdminIndex !=
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex) {
		    localRttMonReactTriggerAdminRttMonCtrlAdminIndex = 0;
		}
		tmpReactQItem =
		    findNextRttMonReactTriggerAdminQItem(tmpAdminQItem->
							 reactTriggerAdminQHead,
							 localRttMonReactTriggerAdminRttMonCtrlAdminIndex);
		if (tmpReactQItem == NULL) {
		    /*
		     *  When a table is sparse, continue looking from
		     *  the last found entry.
		     */
		    localRttMonCtrlAdminIndex = tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		    /*
		     * Increment AdminEntry
		     */
		    localRttMonCtrlAdminIndex++;
		    localRttMonReactTriggerAdminRttMonCtrlAdminIndex = 0;
		} else {
		    memcpy(outTriggerAdminBuff, tmpReactQItem->
			   reactTriggerAdminEntry,
			   sizeof(rttMonReactTriggerAdminEntry_t));
		    outTriggerAdminBuff->rttMonCtrlAdminIndex =
			tmpAdminQItem->ctrlAdminEntry->
			    rttMonCtrlAdminIndex;
		    return_code = D_Return_Successfull;
		    endSearch = TRUE;
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}



void freeRttMonStatsCaptureDistEntry (rttMonStatsCaptureEntry_t
				      * statsCaptureEntry)
{
    free(statsCaptureEntry);
} 

rttMonStatsCaptureEntry_t *newRttMonStatsCaptureDistEntry ()
{
    return (rttMalloc(0,
		      sizeof(rttMonStatsCaptureEntry_t)));
}				

/*
 *  getRttMonStatsCaptureDistEntry
 */ 

int getRttMonStatsCaptureDistEntry (long rttMonCtrlAdminIndex,
				    unsigned long rttMonStatsCaptureStartTimeIndex,
				    long rttMonStatsCapturePathIndex,
				    long rttMonStatsCaptureHopIndex,
				    long rttMonStatsCaptureDistIndex,
				    rttMonStatsCaptureEntry_t * outDistBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    rttMonStatsCaptureDistQItem *tmpDistQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (outDistBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else if (QUEUESIZE(&(tmpAdminQItem->
			       statsCaptureStartTimeQHead->
			       rttMonStatsCaptureStartTimeQ)) == 0) {
	    return_code = D_DataNotBeingStored;
	} else {
	    tmpStartTimeQItem =
		findRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
						     statsCaptureStartTimeQHead,
						     rttMonStatsCaptureStartTimeIndex);
	    if (tmpStartTimeQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		tmpPathQItem =
		    findRttMonStatsCapturePathQItem(tmpStartTimeQItem->
						    statsCapturePathQHead,
						    rttMonStatsCapturePathIndex);
		if (tmpPathQItem == NULL) {
		    return_code = D_InvalidParameter;
		} else {
		    tmpHopQItem =
			findRttMonStatsCaptureHopQItem(tmpPathQItem->
						       statsCaptureHopQHead,
						       rttMonStatsCaptureHopIndex);
		    if (tmpHopQItem == NULL) {
			return_code = D_InvalidParameter;
		    } else {
			tmpDistQItem =
			    findRttMonStatsCaptureDistQItem(tmpHopQItem->
							    statsCaptDistQHead,
							    rttMonStatsCaptureDistIndex);
			if (tmpDistQItem == NULL) {
			    return_code = D_InvalidParameter;
			} else {
			    outDistBuff->rttMonStatsCaptureCompletions =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureCompletions;
			    outDistBuff->rttMonStatsCaptureOverThresholds =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureOverThresholds;
			    outDistBuff->rttMonStatsCaptureSumCompletionTime =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureSumCompletionTime;
			    outDistBuff->rttMonStatsCaptureSumCompletionTime2Low =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureSumCompletionTime2Low;
			    outDistBuff->rttMonStatsCaptureSumCompletionTime2High =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureSumCompletionTime2High;
			    outDistBuff->rttMonStatsCaptureCompletionTimeMax =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureCompletionTimeMax;
			    if (tmpDistQItem->shortStatsCaptureEntry->
				rttMonStatsCaptureCompletionTimeMin == MAXINT) {
				outDistBuff->rttMonStatsCaptureCompletionTimeMin = 0;
			    } else {
				outDistBuff->rttMonStatsCaptureCompletionTimeMin =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureCompletionTimeMin;
			    } 
			    outDistBuff->rttMonStatsCaptureDistIndex =
				tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureDistIndex;
			    outDistBuff->rttMonCtrlAdminIndex =
				tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex;
			    outDistBuff->rttMonStatsCaptureStartTimeIndex =
				tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
			    outDistBuff->rttMonStatsCapturePathIndex =
				tmpPathQItem->rttMonStatsCapturePathIndex;
			    outDistBuff->rttMonStatsCaptureHopIndex =
				tmpHopQItem->rttMonStatsCaptureHopIndex;
			    return_code = D_Return_Successfull;
			}
		    }
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/* 
 *  getNextRttMonStatsCaptureDistEntry
 */ 

int getNextRttMonStatsCaptureDistEntry (long rttMonCtrlAdminIndex,
					unsigned long rttMonStatsCaptureStartTimeIndex,
					long rttMonStatsCapturePathIndex,
					long rttMonStatsCaptureHopIndex,
					long rttMonStatsCaptureDistIndex,
					rttMonStatsCaptureEntry_t * outDistBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    rttMonStatsCaptureDistQItem *tmpDistQItem;
    int return_code;
    boolean endSearch = FALSE;
    long localRttMonCtrlAdminIndex;
    unsigned long localRttMonStatsCaptureStartTimeIndex;
    long localRttMonStatsCapturePathIndex;
    long localRttMonStatsCaptureHopIndex;
    long localRttMonStatsCaptureDistIndex;

    return_code = D_InvalidParameter;
    if (outDistBuff == NULL) {
	return (return_code);
    }
    localRttMonCtrlAdminIndex = rttMonCtrlAdminIndex;
    localRttMonStatsCaptureStartTimeIndex =
	rttMonStatsCaptureStartTimeIndex;
    localRttMonStatsCapturePathIndex =
	rttMonStatsCapturePathIndex;
    localRttMonStatsCaptureHopIndex =
	rttMonStatsCaptureHopIndex;
    localRttMonStatsCaptureDistIndex =
	rttMonStatsCaptureDistIndex;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	while (endSearch == FALSE) {
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	    if (tmpAdminQItem == NULL) {
		return_code = D_EntryNotFound;
		endSearch = TRUE;
	    } else {
		/*
		 *  If we have incremented this index, then the other 
		 *  indicies need to start over.
		 */
		if (localRttMonCtrlAdminIndex !=
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex) {
		    localRttMonStatsCaptureStartTimeIndex = 0;
		    localRttMonStatsCapturePathIndex = 0;
		    localRttMonStatsCaptureHopIndex = 0;
		    localRttMonStatsCaptureDistIndex = 0;
		}
		tmpStartTimeQItem =
		    findNextRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
							     statsCaptureStartTimeQHead,
							     localRttMonStatsCaptureStartTimeIndex);
		if (tmpStartTimeQItem == NULL) {
		    /*
		     *  When a table is sparse, continue looking from
		     *  the last found entry.
		     */
		    localRttMonCtrlAdminIndex = tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		    localRttMonCtrlAdminIndex++;
		    localRttMonStatsCaptureStartTimeIndex = 0;
		    localRttMonStatsCapturePathIndex = 0;
		    localRttMonStatsCaptureHopIndex = 0;
		    localRttMonStatsCaptureDistIndex = 0;
		} else {
		    /*
		     *  If we have incremented this index, then the other 
		     *  indicies need to start over.
		     */
		    if (localRttMonStatsCaptureStartTimeIndex !=
			 tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex) {
			localRttMonStatsCapturePathIndex = 0;
			localRttMonStatsCaptureHopIndex = 0;
			localRttMonStatsCaptureDistIndex = 0;
		    }
		    tmpPathQItem =
			findNextRttMonStatsCapturePathQItem(tmpStartTimeQItem->
							    statsCapturePathQHead,
							    localRttMonStatsCapturePathIndex);
		    if (tmpPathQItem == NULL) {
			/*
			 *  When a table is sparse, continue looking from
			 *  the last found entry.
			 */
			localRttMonStatsCaptureStartTimeIndex = tmpStartTimeQItem->
			    rttMonStatsCaptureStartTimeIndex;
			/* I am assuming the timer will wrap as needed */
			localRttMonStatsCaptureStartTimeIndex++;
			localRttMonStatsCapturePathIndex = 0;
			localRttMonStatsCaptureHopIndex = 0;
			localRttMonStatsCaptureDistIndex = 0;
		    } else {
			/*
			 *  If we have incremented this index, then the other 
			 *  indicies need to start over.
			 */
			if (localRttMonStatsCapturePathIndex !=
			    tmpPathQItem->rttMonStatsCapturePathIndex) {
			    localRttMonStatsCaptureHopIndex = 0;
			    localRttMonStatsCaptureDistIndex = 0;
			}
			tmpHopQItem =
			    findNextRttMonStatsCaptureHopQItem(tmpPathQItem->
							       statsCaptureHopQHead,
							       localRttMonStatsCaptureHopIndex);
			if (tmpHopQItem == NULL) {
			    /*
			     *  When a table is sparse, continue looking from
			     *  the last found entry.
			     */
			    localRttMonStatsCapturePathIndex =
				tmpPathQItem->rttMonStatsCapturePathIndex;
			    localRttMonStatsCapturePathIndex++;
			    localRttMonStatsCaptureHopIndex = 0;
			    localRttMonStatsCaptureDistIndex = 0;
			} else {
			    /*
			     *  If we have incremented this index, then the other 
			     *  indicies need to start over.
			     */
			    if (localRttMonStatsCaptureHopIndex !=
				tmpHopQItem->rttMonStatsCaptureHopIndex) {
				localRttMonStatsCaptureDistIndex = 0;
			    }
			    tmpDistQItem =
				findNextRttMonStatsCaptureDistQItem(tmpHopQItem->
								    statsCaptDistQHead,
								    localRttMonStatsCaptureDistIndex);
			    if (tmpDistQItem == NULL) {
				/*
				 *  When a table is sparse, continue looking from
				 *  the last found entry.
				 */
				localRttMonStatsCaptureHopIndex =
				    tmpHopQItem->rttMonStatsCaptureHopIndex;
				localRttMonStatsCaptureHopIndex++;
				localRttMonStatsCaptureDistIndex = 0;
			    } else {
				outDistBuff->rttMonStatsCaptureCompletions =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureCompletions;
				outDistBuff->rttMonStatsCaptureOverThresholds =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureOverThresholds;
				outDistBuff->rttMonStatsCaptureSumCompletionTime =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureSumCompletionTime;
				outDistBuff->rttMonStatsCaptureSumCompletionTime2Low =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureSumCompletionTime2Low;
				outDistBuff->rttMonStatsCaptureSumCompletionTime2High =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureSumCompletionTime2High;
				outDistBuff->rttMonStatsCaptureCompletionTimeMax =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureCompletionTimeMax;
				if (tmpDistQItem->shortStatsCaptureEntry->
				    rttMonStatsCaptureCompletionTimeMin == MAXINT) {
				    outDistBuff->rttMonStatsCaptureCompletionTimeMin = 0;
				} else {
				    outDistBuff->rttMonStatsCaptureCompletionTimeMin =
					tmpDistQItem->shortStatsCaptureEntry->
					    rttMonStatsCaptureCompletionTimeMin;
				} 
				outDistBuff->rttMonStatsCaptureDistIndex =
				    tmpDistQItem->shortStatsCaptureEntry->
					rttMonStatsCaptureDistIndex;
				outDistBuff->rttMonCtrlAdminIndex =
				    tmpAdminQItem->ctrlAdminEntry->
					rttMonCtrlAdminIndex;
				outDistBuff->rttMonStatsCaptureStartTimeIndex =
				    tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
				outDistBuff->rttMonStatsCapturePathIndex =
				    tmpPathQItem->rttMonStatsCapturePathIndex;
				outDistBuff->rttMonStatsCaptureHopIndex =
				    tmpHopQItem->rttMonStatsCaptureHopIndex;
				return_code = D_Return_Successfull;
				endSearch = TRUE;
			    }
			}
		    }
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}



void freeRttMonStatsTotalsEntry (rttMonStatsTotalsEntry_t
				 * statsTotalsEntry)
{
    free(statsTotalsEntry);
}				

/*
 *  newRttMonStatsTotalsEntry  
 */ 

rttMonStatsTotalsEntry_t *newRttMonStatsTotalsEntry ()
{
    return (rttMalloc(0,
		      sizeof(rttMonStatsTotalsEntry_t)));
} 

int getRttMonStatsTotalsEntry (long rttMonCtrlAdminIndex,
			       unsigned long rttMonStatsCaptureStartTimeIndex,
			       rttMonStatsTotalsEntry_t * outTotalsBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (outTotalsBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else if (QUEUESIZE(&(tmpAdminQItem->
			       statsCaptureStartTimeQHead->
			       rttMonStatsCaptureStartTimeQ)) == 0) {
	    return_code = D_DataNotBeingStored;
	} else {
	    tmpStartTimeQItem =
		findRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
						     statsCaptureStartTimeQHead,
						     rttMonStatsCaptureStartTimeIndex);
	    if (tmpStartTimeQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		memcpy(outTotalsBuff,
		       tmpStartTimeQItem->
		       statsTotalsEntry,
		       sizeof(rttMonStatsTotalsEntry_t));
		outTotalsBuff->rttMonCtrlAdminIndex =
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex;
		outTotalsBuff->rttMonStatsCaptureStartTimeIndex =
		    tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
		if (outTotalsBuff->rttMonStatsTotalsElapsedTime == 0) {
		    outTotalsBuff->rttMonStatsTotalsElapsedTime =
			snmp_sysUpTime() - outTotalsBuff->
			    rttMonStatsCaptureStartTimeIndex;
		}
		return_code = D_Return_Successfull;
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

int getNextRttMonStatsTotalsEntry (long rttMonCtrlAdminIndex,
				   unsigned long rttMonStatsCaptureStartTimeIndex,
				   rttMonStatsTotalsEntry_t * outTotalsBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonStatsCaptureStartTimeQItem *tmpStartTimeQItem;
    boolean endSearch = FALSE;
    long localRttMonCtrlAdminIndex;
    unsigned long localRttMonStatsCaptureStartTimeIndex;
    int return_code;

    return_code = D_InvalidParameter;
    if (outTotalsBuff == NULL) {
	return (return_code);
    }
    localRttMonCtrlAdminIndex = rttMonCtrlAdminIndex;
    localRttMonStatsCaptureStartTimeIndex =
	rttMonStatsCaptureStartTimeIndex;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	while (endSearch == FALSE) {
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	    if (tmpAdminQItem == NULL) {
		return_code = D_EntryNotFound;
		endSearch = TRUE;
	    } else {
		/*
		 *  If we have incremented this index, then the other 
		 *  indicies need to start over.
		 */
		if (localRttMonCtrlAdminIndex !=
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex) {
		    localRttMonStatsCaptureStartTimeIndex = 0;
		}
		tmpStartTimeQItem =
		    findNextRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
							     statsCaptureStartTimeQHead,
							     localRttMonStatsCaptureStartTimeIndex);
		if (tmpStartTimeQItem == NULL) {
		    /*
		     *  When a table is sparse, continue looking from
		     *  the last found entry.
		     */
		    localRttMonCtrlAdminIndex = tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		    localRttMonCtrlAdminIndex++;
		    localRttMonStatsCaptureStartTimeIndex = 0;
		} else {
		    memcpy(outTotalsBuff,
			   tmpStartTimeQItem->statsTotalsEntry,
			   sizeof(rttMonStatsTotalsEntry_t));
		    outTotalsBuff->rttMonCtrlAdminIndex =
			tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex;
		    outTotalsBuff->rttMonStatsCaptureStartTimeIndex =
			tmpStartTimeQItem->rttMonStatsCaptureStartTimeIndex;
		    if (outTotalsBuff->rttMonStatsTotalsElapsedTime == 0) {
			outTotalsBuff->rttMonStatsTotalsElapsedTime =
			    snmp_sysUpTime() - outTotalsBuff->
				rttMonStatsCaptureStartTimeIndex;
		    }
		    return_code = D_Return_Successfull;
		    endSearch = TRUE;
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}



rttMonHistoryCollectionEntry_t *newRttMonHistoryCollectionEntry ()
{
    rttMonHistoryCollectionEntry_t
	* newRttMonHistoryCollectionEntry;

    newRttMonHistoryCollectionEntry =
	rttMalloc(0,
		  sizeof(rttMonHistoryCollectionEntry_t));
    if (newRttMonHistoryCollectionEntry == NULL) {
	return (newRttMonHistoryCollectionEntry);
    }
    memset(newRttMonHistoryCollectionEntry, 0,
	   sizeof(rttMonHistoryCollectionEntry_t));

    newRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddress =
	    preMallocOctetString(0,
				 getMaxRttMonAddressSize(0));
    if (newRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionAddress == NULL) {
	freeRttMonHistoryCollectionEntry(newRttMonHistoryCollectionEntry);
	return (NULL);
    }
    newRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionSenseDescription =
	    preMallocOctetString(0,
				 MAX_rttMonHistoryCollectionSenseDescription);
    if (newRttMonHistoryCollectionEntry->
	rttMonHistoryCollectionSenseDescription == NULL) {
	freeRttMonHistoryCollectionEntry(newRttMonHistoryCollectionEntry);
	return (NULL);
    }
    return (newRttMonHistoryCollectionEntry);

} 

static void copyHistoryCollectionEntry (long rttMonCtrlAdminIndex,
					rttMonHistoryCollectionEntry_t
					* targetHistoryCollectionEntryBuff,
					short_rttMonHistoryCollectionEntry_t
					* sourceShortHistoryCollectionEntryBuff)
{
    OctetString *holdRttMonHistoryCollectionAddress;
    OctetString *holdRttMonHistoryCollectionSenseDescription;

    holdRttMonHistoryCollectionAddress =
	targetHistoryCollectionEntryBuff->
	    rttMonHistoryCollectionAddress;
    cloneToPreAllocatedOctetString(rttMonCtrlAdminIndex,
				   holdRttMonHistoryCollectionAddress,
				   sourceShortHistoryCollectionEntryBuff->
				   rttMonHistoryCollectionAddress);
    holdRttMonHistoryCollectionSenseDescription =
	targetHistoryCollectionEntryBuff->
	    rttMonHistoryCollectionSenseDescription;
#if defined(RTT_SCRIPTS)
    cloneToPreAllocatedOctetString(rttMonCtrlAdminIndex,
				   holdRttMonHistoryCollectionSenseDescription,
				   sourceShortHistoryCollectionEntryBuff->
				   rttMonHistoryCollectionSenseDescription);
#endif
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionSampleIndex =
	    sourceShortHistoryCollectionEntryBuff->
		rttMonHistoryCollectionSampleIndex;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionSampleTime =
	    sourceShortHistoryCollectionEntryBuff->
		rttMonHistoryCollectionSampleTime;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionCompletionTime =
	    sourceShortHistoryCollectionEntryBuff->
		rttMonHistoryCollectionCompletionTime;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionSense =
	    sourceShortHistoryCollectionEntryBuff->
		rttMonHistoryCollectionSense;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionAddress =
	    holdRttMonHistoryCollectionAddress;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionSenseDescription =
	    holdRttMonHistoryCollectionSenseDescription;
    targetHistoryCollectionEntryBuff->
	rttMonHistoryCollectionApplSpecificSense =
	    MAX_ApplSpecSense;

    return;
}				

/*
 *  getRttMonHistoryCollectionSampleEntry
 */ 

int getRttMonHistoryCollectionEntry (long rttMonCtrlAdminIndex,
				     long rttMonHistoryCollectionLifeIndex,
				     long rttMonHistoryCollectionBucketIndex,
				     long rttMonHistoryCollectionSampleIndex,
				     rttMonHistoryCollectionEntry_t * outSampleBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonHistoryCollectionLifeQItem *tmpLifeQItem;
    rttMonHistoryCollectionBucketQItem *tmpBucketQItem;
    rttMonHistoryCollectionSampleQItem *tmpSampleQItem;
    int return_code;

    return_code = D_InvalidParameter;
    if (outSampleBuff == NULL) {
	return (return_code);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpAdminQItem =
	    findRttMonCtrlAdminQItem(rttMonCtrlAdminIndex);
	if (tmpAdminQItem == NULL) {
	    return_code = D_InvalidParameter;
	} else if (QUEUESIZE(&(tmpAdminQItem->
			       historyCollectionLifeQHead->
			       rttMonHistoryCollectionLifeQ)) == 0) {
	    return_code = D_DataNotBeingStored;
	} else {
	    tmpLifeQItem =
		findRttMonHistoryCollectionLifeQItem(tmpAdminQItem->
						     historyCollectionLifeQHead,
						     rttMonHistoryCollectionLifeIndex);
	    if (tmpLifeQItem == NULL) {
		return_code = D_InvalidParameter;
	    } else {
		tmpBucketQItem =
		    findRttMonHistoryCollectionBucketQItem(tmpLifeQItem->
							   historyCollectionBucketQHead,
							   rttMonHistoryCollectionBucketIndex);
		if (tmpBucketQItem == NULL) {
		    return_code = D_InvalidParameter;
		} else {
		    tmpSampleQItem =
			findRttMonHistoryCollectionSampleQItem(tmpBucketQItem->
							       historyCollectionSampleQHead,
							       rttMonHistoryCollectionSampleIndex);
		    if (tmpSampleQItem == NULL) {
			return_code = D_InvalidParameter;
		    } else {
			copyHistoryCollectionEntry(tmpAdminQItem->
						   ctrlAdminEntry->
						   rttMonCtrlAdminIndex,
						   outSampleBuff,
						   tmpSampleQItem->
						   shortHistoryCollectionEntry);
			outSampleBuff->rttMonCtrlAdminIndex =
			    tmpAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex;
			outSampleBuff->
			    rttMonHistoryCollectionLifeIndex =
				tmpLifeQItem->
				    rttMonHistoryCollectionLifeIndex;
			outSampleBuff->
			    rttMonHistoryCollectionBucketIndex =
				tmpBucketQItem->
				    rttMonHistoryCollectionBucketIndex;
			return_code = D_Return_Successfull;
		    }
		}
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/*
 * getNextRttMonHistoryCollectionSampleEntry
 */ 

int getNextRttMonHistoryCollectionEntry (long rttMonCtrlAdminIndex,
					 long rttMonHistoryCollectionLifeIndex,
					 long rttMonHistoryCollectionBucketIndex,
					 long rttMonHistoryCollectionSampleIndex,
					 rttMonHistoryCollectionEntry_t * outSampleBuff)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonHistoryCollectionLifeQItem *tmpLifeQItem;
    rttMonHistoryCollectionBucketQItem *tmpBucketQItem;
    rttMonHistoryCollectionSampleQItem *tmpSampleQItem;
    int return_code;
    boolean endSearch = FALSE;
    long localRttMonCtrlAdminIndex;
    long localRttMonHistoryCollectionLifeIndex;
    long localRttMonHistoryCollectionBucketIndex;
    long localRttMonHistoryCollectionSampleIndex;

    return_code = D_InvalidParameter;
    if (outSampleBuff == NULL) {
	return (return_code);
    }
    localRttMonCtrlAdminIndex = rttMonCtrlAdminIndex;
    localRttMonHistoryCollectionLifeIndex =
	rttMonHistoryCollectionLifeIndex;
    localRttMonHistoryCollectionBucketIndex =
	rttMonHistoryCollectionBucketIndex;
    localRttMonHistoryCollectionSampleIndex =
	rttMonHistoryCollectionSampleIndex;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	while (endSearch == FALSE) {
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	    if (tmpAdminQItem == NULL) {
		return_code = D_EntryNotFound;
		endSearch = TRUE;
	    } else {
		/*
		 *  If we have incremented this index, then the other 
		 *  indicies need to start over.
		 */
		if (localRttMonCtrlAdminIndex !=
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminIndex) {
		    localRttMonHistoryCollectionLifeIndex = 0;
		    localRttMonHistoryCollectionBucketIndex = 0;
		    localRttMonHistoryCollectionSampleIndex = 0;
		}
		tmpLifeQItem =
		    findNextRttMonHistoryCollectionLifeQItem(tmpAdminQItem->
							     historyCollectionLifeQHead,
							     localRttMonHistoryCollectionLifeIndex);
		if (tmpLifeQItem == NULL) {
		    /*
		     *  When a table is sparse, continue looking from
		     *  the last found entry.
		     */
		    localRttMonCtrlAdminIndex = tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex;
		    localRttMonCtrlAdminIndex++;
		    localRttMonHistoryCollectionLifeIndex = 0;
		    localRttMonHistoryCollectionBucketIndex = 0;
		    localRttMonHistoryCollectionSampleIndex = 0;
		} else {
		    /*
		     *  If we have incremented this index, then the other 
		     *  indicies need to start over.
		     */
		    if (localRttMonHistoryCollectionLifeIndex !=
			tmpLifeQItem->rttMonHistoryCollectionLifeIndex) {
			localRttMonHistoryCollectionBucketIndex = 0;
			localRttMonHistoryCollectionSampleIndex = 0;
		    }
		    tmpBucketQItem =
			findNextRttMonHistoryCollectionBucketQItem(tmpLifeQItem->
								   historyCollectionBucketQHead,
								   localRttMonHistoryCollectionBucketIndex);
		    if (tmpBucketQItem == NULL) {
			/*
			 *  When a table is sparse, continue looking from
			 *  the last found entry.
			 */
			localRttMonHistoryCollectionLifeIndex =
			    tmpLifeQItem->rttMonHistoryCollectionLifeIndex;
			localRttMonHistoryCollectionLifeIndex++;
			localRttMonHistoryCollectionBucketIndex = 0;
			localRttMonHistoryCollectionSampleIndex = 0;
		    } else {
			/*
			 *  If we have incremented this index, then the other 
			 *  indicies need to start over.
			 */
			if (localRttMonHistoryCollectionBucketIndex !=
			    tmpBucketQItem->rttMonHistoryCollectionBucketIndex) {
			    localRttMonHistoryCollectionSampleIndex = 0;
			}
			tmpSampleQItem =
			    findNextRttMonHistoryCollectionSampleQItem(tmpBucketQItem->
								       historyCollectionSampleQHead,
								       localRttMonHistoryCollectionSampleIndex);
			if (tmpSampleQItem == NULL) {
			    /*
			     *  When a table is sparse, continue looking from
			     *  the last found entry.
			     */
			    localRttMonHistoryCollectionBucketIndex =
				tmpBucketQItem->rttMonHistoryCollectionBucketIndex;
			    localRttMonHistoryCollectionBucketIndex++;
			    localRttMonHistoryCollectionSampleIndex = 0;
			} else {
			    copyHistoryCollectionEntry(tmpAdminQItem->
						       ctrlAdminEntry->
						       rttMonCtrlAdminIndex,
						       outSampleBuff,
						       tmpSampleQItem->
						       shortHistoryCollectionEntry);
			    outSampleBuff->rttMonCtrlAdminIndex =
				tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex;
			    outSampleBuff->
				rttMonHistoryCollectionLifeIndex =
				    tmpLifeQItem->
					rttMonHistoryCollectionLifeIndex;
			    outSampleBuff->
				rttMonHistoryCollectionBucketIndex =
				    tmpBucketQItem->
					rttMonHistoryCollectionBucketIndex;
			    return_code = D_Return_Successfull;
			    endSearch = TRUE;
			}
		    }
		}
	    }
	} /* while (endSearch == FALSE) */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

static void doEndLife (rttMonCtrlAdminQItem
		       * inRttMonCtrlAdminQItem)
{
    unsigned long currentTime;

    currentTime = snmp_sysUpTime();
    if (inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
	lastAddedStatsCaptureStartTimeQItem != NULL) {
	inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
	    lastAddedStatsCaptureStartTimeQItem->
		statsTotalsEntry->
		    rttMonStatsTotalsElapsedTime =
			currentTime -
			    inRttMonCtrlAdminQItem->
				statsCaptureStartTimeQHead->
				    currentStatsCaptureStartTimeQItem->
					rttMonStatsCaptureStartTimeIndex;
    }
    /* 
     * Stop the life timer and update the Oper
     *  life time to reflect when it stoped.
     *  It may already be expired...
     */ 
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer stop (endLifeTimer)");
	RTT_BUGINF3(0,
		    "\t in doEndLife for index %u\n",
		    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    mgd_timer_stop(&(inRttMonCtrlAdminQItem->
		     endLifeTimer));
    /*
     * Life is really the number of elapsed
     *  seconds, so find the elapsed time
     *  in hundreths of seconds and divide
     *  by 100 to get seconds.
     */
    inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	    rttMonCtrlOperRttLife =
		inRttMonCtrlAdminQItem->
		    ctrlAdminEntry->
			rttMonScheduleAdminRttLife -
			    ((currentTime -
			      inRttMonCtrlAdminQItem->
			      actualStartTime) / 100L);
    inRttMonCtrlAdminQItem->
	actualStartTime = 0;
    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlOperRttLife < 0) {
	inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlOperRttLife = 0;
    }
    return;
} 

static void clearRttMonReactTriggerAdminEntries (rttMonCtrlAdminQItem
						 * inRttMonCtrlAdminQItem)
{
    long tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex;
    rttMonReactTriggerAdminQItem *tmpReactTriggerAdminQItem;
    rttMonCtrlAdminQItem *tmpCtrlAdminQItem;
    long tmpRttMonCtrlAdminIndex;

    /*
     * Clear all entries this row point at.
     */
    tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex = 0;
    tmpReactTriggerAdminQItem =
	findNextRttMonReactTriggerAdminQItem(inRttMonCtrlAdminQItem->
					     reactTriggerAdminQHead,
					     tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    while (tmpReactTriggerAdminQItem != NULL) {
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex =
	    tmpReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		    rttMonReactTriggerAdminRttMonCtrlAdminIndex;
	if (tmpReactTriggerAdminQItem->
	    reactTriggerAdminEntry->
	    rttMonReactTriggerOperState ==
	    D_rttMonReactTriggerOperState_active) {
	    tmpReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		    rttMonReactTriggerOperState =
			D_rttMonReactTriggerOperState_pending;
	}
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex++;
	tmpReactTriggerAdminQItem =
	    findNextRttMonReactTriggerAdminQItem(inRttMonCtrlAdminQItem->
						 reactTriggerAdminQHead,
						 tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    }		

    /*
     * Clear all entries pointing at this entry
     */ 
    tmpRttMonCtrlAdminIndex = 0;
    tmpCtrlAdminQItem =
	findNextRttMonCtrlAdminQItem(tmpRttMonCtrlAdminIndex);
    while (tmpCtrlAdminQItem != NULL) {
	tmpRttMonCtrlAdminIndex =
	    tmpCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlAdminIndex;
	tmpReactTriggerAdminQItem =
	    findRttMonReactTriggerAdminQItem(tmpCtrlAdminQItem->
					     reactTriggerAdminQHead,
					     tmpRttMonCtrlAdminIndex);
	if ((tmpReactTriggerAdminQItem != NULL) &&
	    (tmpReactTriggerAdminQItem->
	     reactTriggerAdminEntry->
	     rttMonReactTriggerOperState ==
	     D_rttMonReactTriggerOperState_active)) {
	    tmpReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		    rttMonReactTriggerOperState =
			D_rttMonReactTriggerOperState_pending;
	}
	tmpRttMonCtrlAdminIndex++;
	tmpCtrlAdminQItem =
	    findNextRttMonCtrlAdminQItem(tmpRttMonCtrlAdminIndex);
    }
    return;
}				

/* 
 *  This function Assumes that the StartTimeQ is completely 
 *  utilized (All Entries are VALID) and it will return
 *  the oldest StartTimeQItem.
 */ 

static rttMonStatsCaptureStartTimeQItem
*oldestStatsCaptureStartTimeQItem (rttMonStatsCaptureStartTimeQHead
				   * startTimeQHead)
{
    return (startTimeQHead->rttMonStatsCaptureStartTimeQ.qhead);
} 

static rttMonHistoryCollectionLifeQItem
*oldestHistoryCollectionLifeQItem (rttMonHistoryCollectionLifeQHead
				   * lifeQHead)
{
    return (lifeQHead->rttMonHistoryCollectionLifeQ.qhead);
} 

static long doStartLife (rttMonCtrlAdminQItem
			 * inRttMonCtrlAdminQItem)
{
    long return_code;
    unsigned long currentTime;

    return_code = D_Return_Successfull;

    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperState !=
	D_rttMonCtrlOperState_active) {
	return_code = D_BadOperState;
    } else {
	currentTime = snmp_sysUpTime();
	inRttMonCtrlAdminQItem->
	    actualStartTime =
		currentTime;
	/* 
	 * Start the timer that specifies 
	 *  that specifies the end of life
	 */
	if (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonCtrlOperRttLife !=
	    DEF_rttMonScheduleAdminRttLife_FOREVER) {
	    /* 
	     *  RttLife is in seconds so multiply by
	     *   1000 to get milliseconds of the timers
	     */
	    /*
	     * Adjust OperTimerChunk to be used as part of 
	     *   whole life.
	     */
	    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonCtrlOperRttLife < DEF_OperRttLifeTimeChunk) {
	        inRttMonCtrlAdminQItem->operRttLifeTimerChunk =
		    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		      rttMonCtrlOperRttLife;
		inRttMonCtrlAdminQItem->ctrlAdminEntry->
		      rttMonCtrlOperRttLife = 0;
	    } else {
	        inRttMonCtrlAdminQItem->operRttLifeTimerChunk =
		    DEF_OperRttLifeTimeChunk;
		inRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlOperRttLife -= DEF_OperRttLifeTimeChunk;
	    }
	    if (RTT_TRACE(0)) {
		dumpRttMonProcessHeaderString(0, "Timer start (endLifeTimer)");
		RTT_BUGINF4(0,
			    "\t in doStartLife for index %u, set to %u (milliseconds)\n",
			    inRttMonCtrlAdminQItem->ctrlAdminEntry->
			    rttMonCtrlAdminIndex,
			    inRttMonCtrlAdminQItem->
			    operRttLifeTimerChunk * 1000L);
	    }
	    mgd_timer_start(&(inRttMonCtrlAdminQItem->
			      endLifeTimer),
			    inRttMonCtrlAdminQItem->
			    operRttLifeTimerChunk * 1000L);
	}
	process_may_suspend();
	if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			statsCaptureStartTimeQHead->
			rttMonStatsCaptureStartTimeQ)) != 0) {
	    if (inRttMonCtrlAdminQItem->
		statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem == NULL) {
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem =
			    inRttMonCtrlAdminQItem->
				statsCaptureStartTimeQHead->
				    rttMonStatsCaptureStartTimeQ.qhead;
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem->
			    rttMonStatsCaptureStartTimeIndex =
				currentTime;
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
		    RTT_BUGINF4(0,
				"\t in doStartLife for index %u, set to %u (milliseconds)\n",
				inRttMonCtrlAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				ONEHOUR);
		}
		mgd_timer_start(&(inRttMonCtrlAdminQItem->
				  oneHourTimer), ONEHOUR);
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			lastAddedStatsCaptureStartTimeQItem =
			    inRttMonCtrlAdminQItem->
				statsCaptureStartTimeQHead->
				    currentStatsCaptureStartTimeQItem;
	    } else {
		inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem =
			inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
			    lastAddedStatsCaptureStartTimeQItem->next;
		if (inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem == NULL) {
		    /*
		     * This is called the first time through and
		     *   when the queue is full.
		     */
		    inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem =
			    oldestStatsCaptureStartTimeQItem(inRttMonCtrlAdminQItem->
							     statsCaptureStartTimeQHead);
		    /* 
		     *  Scrub old entry
		     */
		    process_may_suspend();
		    scrubRttMonStatsCaptureStartTimeQItem(inRttMonCtrlAdminQItem->
							  statsCaptureStartTimeQHead->
							  currentStatsCaptureStartTimeQItem);
		}
		/* requeue in order */ 
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem->
			    rttMonStatsCaptureStartTimeIndex = currentTime;
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
		    RTT_BUGINF4(0,
				"\t in doStartLife for index %u, set to %u (milliseconds)\n",
				inRttMonCtrlAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				ONEHOUR);
		}
		mgd_timer_start(&(inRttMonCtrlAdminQItem->
				  oneHourTimer), ONEHOUR);
		process_may_suspend();
		return_code = 
		    updateRttMonStatsCaptureStartTimeQItem(inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonCtrlAdminIndex,
							   inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead,
							   inRttMonCtrlAdminQItem->
							   statsCaptureStartTimeQHead->
							   currentStatsCaptureStartTimeQItem);
		inRttMonCtrlAdminQItem->statsCaptureStartTimeQHead->
		    lastAddedStatsCaptureStartTimeQItem =
			inRttMonCtrlAdminQItem->
			    statsCaptureStartTimeQHead->
				currentStatsCaptureStartTimeQItem;
	    } 
	    SET_VALID(I_queue_item_in_use,
		      inRttMonCtrlAdminQItem->
		      statsCaptureStartTimeQHead->
		      currentStatsCaptureStartTimeQItem->valid);
	}
	process_may_suspend();
	if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			historyCollectionLifeQHead->
			rttMonHistoryCollectionLifeQ)) != 0) {
	    if (inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		currentHistoryCollectionLifeQItem == NULL) {
		inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
			currentHistoryCollectionLifeQItem =
			    inRttMonCtrlAdminQItem->
				historyCollectionLifeQHead->
				    rttMonHistoryCollectionLifeQ.qhead;
		inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
			lastAddedHistoryCollectionLifeQItem =
			    inRttMonCtrlAdminQItem->
				historyCollectionLifeQHead->
				    currentHistoryCollectionLifeQItem;
	    } else {
		inRttMonCtrlAdminQItem->historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem =
			inRttMonCtrlAdminQItem->historyCollectionLifeQHead->
			    lastAddedHistoryCollectionLifeQItem->next;
		if (inRttMonCtrlAdminQItem->historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem == NULL) {
		    /*
		     *  This is called when the queue is full.
		     */
		    inRttMonCtrlAdminQItem->historyCollectionLifeQHead->
			currentHistoryCollectionLifeQItem =
			    oldestHistoryCollectionLifeQItem(inRttMonCtrlAdminQItem->
							     historyCollectionLifeQHead);
		    /*
		     * Scrub old Entry.
		     */
		    process_may_suspend();
		    scrubRttMonHistoryCollectionLifeQItem(inRttMonCtrlAdminQItem->
							  historyCollectionLifeQHead->
							  currentHistoryCollectionLifeQItem);
		}
		/* requeue in order */ 
		inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
			currentHistoryCollectionLifeQItem->
			    rttMonHistoryCollectionLifeIndex =
				inRttMonCtrlAdminQItem->
				    historyCollectionLifeQHead->
					lastAddedHistoryCollectionLifeQItem->
					    rttMonHistoryCollectionLifeIndex + 1;
		if (inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
		    rttMonHistoryCollectionLifeIndex >
		    MAX_rttMonHistoryCollectionLifeIdx) {
		    inRttMonCtrlAdminQItem->
			historyCollectionLifeQHead->
			    currentHistoryCollectionLifeQItem->
				rttMonHistoryCollectionLifeIndex =
				    MIN_rttMonHistoryCollectionLifeIdx;
		}
		process_may_suspend();
		return_code = 
		    updateRttMonHistoryCollectionLifeQItem(inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonCtrlAdminIndex,
							   inRttMonCtrlAdminQItem->historyCollectionLifeQHead,
							   inRttMonCtrlAdminQItem->
							   historyCollectionLifeQHead->
							   currentHistoryCollectionLifeQItem);
		process_may_suspend();
		inRttMonCtrlAdminQItem->historyCollectionLifeQHead->
		    lastAddedHistoryCollectionLifeQItem =
			inRttMonCtrlAdminQItem->
			    historyCollectionLifeQHead->
				currentHistoryCollectionLifeQItem;
	    } 
	    SET_VALID(I_queue_item_in_use,
		      inRttMonCtrlAdminQItem->
		      historyCollectionLifeQHead->
		      currentHistoryCollectionLifeQItem->valid);
	}
    } 
    return (return_code);
}



/* Response Time Library Calls */ 

long rttMonStartLife (rttMonCtrlAdminQItem
		      * inRttMonCtrlAdminQItem)
{
    long return_code;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	return_code = doStartLife(inRttMonCtrlAdminQItem);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long getRttMonCtrlAdminRttType (rttMonCtrlAdminQItem
				* inRttMonCtrlAdminQItem)
{
    long ctrlAdminRttType;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	ctrlAdminRttType = inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminRttType;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (ctrlAdminRttType);

} 

long getRttMonEchoAdminProtocol (rttMonCtrlAdminQItem
				 * inRttMonCtrlAdminQItem)
{
    long echoAdminProtocol;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	echoAdminProtocol =
	    inRttMonCtrlAdminQItem->echoAdminEntry->
		rttMonEchoAdminProtocol;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (echoAdminProtocol);

} 

OctetString *rttMonNewAddressBuffer ()
{
    OctetString *tmpAddressBuff;

    tmpAddressBuff = preMallocOctetString(0,
					  getMaxRttMonAddressSize(0));
    return (tmpAddressBuff);
} 

OctetString *getRttMonEchoAdminTargetAddress (rttMonCtrlAdminQItem
					      * inRttMonCtrlAdminQItem,
					      OctetString
					      * echoAdminTargetAddressBuff)
{
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
				       echoAdminEntry->
				       rttMonCtrlAdminIndex,
				       echoAdminTargetAddressBuff,
				       inRttMonCtrlAdminQItem->
				       echoAdminEntry->
				       rttMonEchoAdminTargetAddress);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (echoAdminTargetAddressBuff);

} 

long getRttMonEchoAdminPktDataRequestSize (rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem)
{
    long echoAdminPktDataReqSize;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	echoAdminPktDataReqSize =
	    inRttMonCtrlAdminQItem->
		echoAdminEntry->
		    rttMonEchoAdminPktDataRequestSize;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (echoAdminPktDataReqSize);

} 

long getRttMonEchoAdminPktDataResponseSize (rttMonCtrlAdminQItem
					    * inRttMonCtrlAdminQItem)
{
    long echoAdminPktDataResponseSize;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	echoAdminPktDataResponseSize =
	    inRttMonCtrlAdminQItem->
		echoAdminEntry->
		    rttMonEchoAdminPktDataResponseSize;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (echoAdminPktDataResponseSize);

} 

void setRttMonSchedulerPid (pid_t pid)
{
    baseRttMonCtrlAdminQHead.schedulerPid = pid;
    if (pid == NO_PROCESS) {
	baseRttMonCtrlAdminQHead.schedulerActive = FALSE;
    } else {
	baseRttMonCtrlAdminQHead.schedulerActive = TRUE;
    } 
    return;

} 

void rttMonSchedulerInit (void)
{
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer Work in rttMonSchedulerInit");
	RTT_BUGINF2(0,
		    "\t init parent master\n");
	RTT_BUGINF2(0,
		    "\t init parent ageoutMaster parent master\n");
	RTT_BUGINF2(0,
		    "\t init parent oneHourMaster parent master\n");
    }
    mgd_timer_init_parent(&(baseRttMonCtrlAdminQHead.master),
			  NULL);
    mgd_timer_init_parent(&(baseRttMonCtrlAdminQHead.ageoutMaster),
			  &(baseRttMonCtrlAdminQHead.master));
    mgd_timer_init_parent(&(baseRttMonCtrlAdminQHead.oneHourMaster),
			  &(baseRttMonCtrlAdminQHead.master));
    mgd_timer_init_parent(&(baseRttMonCtrlAdminQHead.lifeTimeMaster),
			  &(baseRttMonCtrlAdminQHead.master));
    baseRttMonCtrlAdminQHead.aProcessDied =
	create_watched_boolean("rttMonProcessDied", 0);
    baseRttMonCtrlAdminQHead.schedulerResetSelf =
	create_watched_boolean("rttMonSchedulerResetSelf", 0);

    /* Initialize the watched booleans */
    process_set_boolean(baseRttMonCtrlAdminQHead.aProcessDied,
			FALSE);
    process_set_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf,
			FALSE);

    process_watch_mgd_timer(&(baseRttMonCtrlAdminQHead.master),
			    ENABLE);
    process_watch_boolean(baseRttMonCtrlAdminQHead.aProcessDied,
			  ENABLE, RECURRING);
    process_watch_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf,
			  ENABLE, RECURRING);

    return;

} 

void rttMonSchedulerCleanup (void)
{
    /* free all managed timers linked to the master */
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer stop (master)");
	RTT_BUGINF2(0,
		    "\t in rttMonSchedulerCleanup\n");
    }
    mgd_timer_stop(&(baseRttMonCtrlAdminQHead.
		     master));
    delete_watched_boolean(&(baseRttMonCtrlAdminQHead.
			     aProcessDied));
    delete_watched_boolean(&(baseRttMonCtrlAdminQHead.
			     schedulerResetSelf));
    return;
} 

static void rttMonCleanupProcessDeath (rttMonCtrlAdminQItem
				       * tmpAdminQItem)
{
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Process Death Cleanup");
	RTT_BUGINF3(0,
		    "\t in rttMonCleanupProcessDeath, aProcessDidDie Probe %u\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    tmpAdminQItem->inTheProcessOfDying = FALSE;
    tmpAdminQItem->pid = 0;
    if (tmpAdminQItem->ctrlAdminEntry->
	rttMonCtrlAdminStatus ==
	D_rttMonCtrlAdminStatus_destroy) {
	if (RTT_TRACE(0)) {
	    RTT_BUGINF2(0,
			"\t   Destroying Entry\n");
	}
	freeRttMonCtrlAdminQItemByIndexInternal(tmpAdminQItem->ctrlAdminEntry->
						rttMonCtrlAdminIndex);
    } else if (tmpAdminQItem->
	       restartAfterDeath) {
	if (RTT_TRACE(0)) {
	    RTT_BUGINF2(0,
			"\t   Restarting Probe\n");
	}
	tmpAdminQItem->
	    restartAfterDeath = FALSE;
	startProbe(tmpAdminQItem);
    } else {
	if (RTT_TRACE(0)) {
	    RTT_BUGINF2(0,
			"\t   just cleaning up flags\n");
	}
    } 
    return;
} 

static void rttMonProcessEndLife (rttMonCtrlAdminQItem
				  * tmpAdminQItem)
{
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Process End Life");
	RTT_BUGINF3(0,
		    "\t in rttMonProcessEndLife, endLifeTimer popped for Probe %u\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    /*
     * Adjust OperTimerChunk to be used as part of 
     *   whole life.
     */
    if (tmpAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperRttLife > 0) {
        if (tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperRttLife < DEF_OperRttLifeTimeChunk) {
            tmpAdminQItem->operRttLifeTimerChunk =
	      tmpAdminQItem->ctrlAdminEntry->
	      rttMonCtrlOperRttLife;
            tmpAdminQItem->ctrlAdminEntry->
	      rttMonCtrlOperRttLife = 0;
        } else {
            tmpAdminQItem->operRttLifeTimerChunk =
	      DEF_OperRttLifeTimeChunk;
            tmpAdminQItem->ctrlAdminEntry->
	      rttMonCtrlOperRttLife -= DEF_OperRttLifeTimeChunk;
        }
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start (endLifeTimer) new chunk");
	    RTT_BUGINF4(0,
			"\t in rttMonProcessEndLife for index %u, set to %u (milliseconds)\n",
			tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			tmpAdminQItem->
			operRttLifeTimerChunk * 1000L);
	}
	mgd_timer_start(&(tmpAdminQItem->
			  endLifeTimer),
			tmpAdminQItem->
			operRttLifeTimerChunk * 1000L);
	return;
    }

    /*
     * When the probe actually transitions out of life, 
     *  the doEndLife will be called to close the stats.
     *  It needs to be done then, because we cannot 
     *  be sure the probe is done with this current
     *  statistics group.
     */ 
    if (mgd_timer_expired(&(tmpAdminQItem->
			    oneHourTimer))) {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
	    RTT_BUGINF2(0,
			"\t in rttMonProcessEndLife\n");
	    RTT_BUGINF2(0,
			"\t   Restarted, so the probe can end it when it finishes.\n");
	    RTT_BUGINF4(0,
			"\t   for index %u, set to %u (milliseconds)\n",
			tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			ONEHOUR);
	}
	mgd_timer_start(&(tmpAdminQItem->
			  oneHourTimer), ONEHOUR);
    }
    /* 
     * Restart the endLifeTimer, to give us time to allow the probe to die
     *   gracefully, and clear the timer. 
     */ 
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer start (endLifeTimer)");
	RTT_BUGINF2(0,
		    "\t in rttMonProcessEndLife\n");
	RTT_BUGINF2(0,
		    "\t   Restarted, so the probe can end it when it finishes.\n");
	RTT_BUGINF4(0,
		    "\t   for index %u, set to %u (milliseconds)\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    tmpAdminQItem->
		    operRttLifeTimerChunk * 1000L);
    }
    mgd_timer_start(&(tmpAdminQItem->
		      endLifeTimer),
		    tmpAdminQItem->
		    operRttLifeTimerChunk * 1000L);

    /*
     * Again the probe will stop this timer when it
     *  transitions out of life.
     *  If the transition fails, it is currently in
     *  transition, which can be reset, or stop, either
     *  will clear this probe.
     */
    transitionOperState(tmpAdminQItem,
			D_rttMonCtrlOperState_immediateStop);
    clearRttMonReactTriggerAdminEntries(tmpAdminQItem);
    if (tmpAdminQItem->
	restartPending) {
	tmpAdminQItem->
	    restartAfterDeath = TRUE;
    }
    return;
} 

static void rttMonProcessAgeOut (rttMonCtrlAdminQItem
				 * tmpAdminQItem)
{
    boolean freeTheMem;
    long tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex;
    rttMonReactTriggerAdminQItem *tmpReactTriggerAdminQItem;
    int return_code, oldAdminStatus;

    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Process Age Out");
	RTT_BUGINF3(0,
		    "\t in rttMonProcessAgeOut, ageoutTimer popped for Probe %u\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    /*
     *  Delete the entry.
     *    This will clear the timer.
     */ 
    freeTheMem = FALSE;
    if (tmpAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperState !=
	D_rttMonCtrlOperState_inactive) {
	oldAdminStatus = tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminStatus;
	tmpAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminStatus =
		D_rttMonCtrlAdminStatus_destroy;
	/*
	 * If the probe cannot ageout, it is in the process of
	 *  changing state, thus ageout is in the process of
	 *  being reset.
	 */
	return_code = transitionOperState(tmpAdminQItem,
					  D_rttMonCtrlOperState_immediateStop);
	if (return_code != D_Return_Successfull) {
		    tmpAdminQItem->ctrlAdminEntry->rttMonCtrlAdminStatus =
		      oldAdminStatus;
	    if (RTT_TRACE(0)) {
	        RTT_BUGINF2(0,
			    "\t  Operational State Change Failed\n");
	    }
	} else {
	    if (RTT_TRACE(0)) {
	        RTT_BUGINF2(0,
			    "\t  Setting Admin Status to Destroy\n");
		RTT_BUGINF2(0,
			    "\t    and transitionOperState to tell probe to stop\n");
	    }
	}
    } else if (tmpAdminQItem->inTheProcessOfDying ==
	       FALSE) {
	if (RTT_TRACE(0)) {
	    RTT_BUGINF2(0,
			"\t   Entry Already inactive, deleting it\n");
	}
	/*
	 *  Just cleanup the inactive entry.
	 */ 
	unqueue(&(baseRttMonCtrlAdminQHead.
		  rttMonCtrlAdminQ),
		tmpAdminQItem);
	tmpAdminQItem->next = NULL;
	if ((baseRttMonCtrlAdminQHead.
	     lastSearchedCtrlAdminQItem != NULL) &&
	    (baseRttMonCtrlAdminQHead.
	     lastSearchedCtrlAdminQItem ==
	     tmpAdminQItem)) {
	    baseRttMonCtrlAdminQHead.
		lastSearchedCtrlAdminQItem = NULL;
	}
	freeTheMem = TRUE;
    } /* else if (...inTheProcessOfDying... */
    /*
     *  Clear Reaction Table of entries for 
     *    this conceptual row.
     */ 
    tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex = 0;
    tmpReactTriggerAdminQItem =
	findNextRttMonReactTriggerAdminQItem(tmpAdminQItem->
					     reactTriggerAdminQHead,
					     tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    while (tmpReactTriggerAdminQItem != NULL) {
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex =
	    tmpReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		    rttMonReactTriggerAdminRttMonCtrlAdminIndex;
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex++;
	CLR_VALID(I_queue_item_in_use,
		  tmpReactTriggerAdminQItem->valid);
	tmpReactTriggerAdminQItem =
	    findNextRttMonReactTriggerAdminQItem(tmpAdminQItem->
						 reactTriggerAdminQHead,
						 tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    }
    if (freeTheMem) {
	freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
    }
    return;
} 

static void rttMonStartProcessing (rttMonCtrlAdminQItem
				   * tmpAdminQItem)
{
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Process Start Time");
	RTT_BUGINF3(0,
		    "\t in rttMonStartProcessing, startTimer popped for Probe %u\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
	RTT_BUGINF2(0,
		    "\t   transitioningOperState to active");
	dumpRttMonProcessHeaderString(0, "Timer stop (startTimer)");
	RTT_BUGINF2(0,
		    "\t in rttMonStartProcessing\n");
    }
    mgd_timer_stop(&(tmpAdminQItem->
		     startTimer));
    transitionOperState(tmpAdminQItem,
			D_rttMonCtrlOperState_active);
    return;
} 

/*
 * Forward Declarations
 */
static void rttMonStartUpdatePathInternal (rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem);
static void fillPathQItem (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem);
/*
 *  Create a source to target path.  Remember to terminate the
 *   currently stored temp path.  The probe could be in the
 *   middle of an operation, and thus will need the temp
 *   path length to be the same as the source to target path
 *   length.
 */
static void rttMonCreateSourceToTargetVirgin (rttMonCtrlAdminQItem
					    * tmpAdminQItem)
{
  /* 
   * Clear tmpAdminQ path info
   */
  rttMonStartUpdatePathInternal(tmpAdminQItem);
  /*
   * Set the target address into the first item
   */
  cloneToPreAllocatedOctetString(tmpAdminQItem->
				 ctrlAdminEntry->
				 rttMonCtrlAdminIndex,
				 tmpAdminQItem->
				 currPathItem->hop,
				 tmpAdminQItem->
				 echoAdminEntry->
				 rttMonEchoAdminTargetAddress);
  SET_VALID(I_queue_item_in_use,
	    tmpAdminQItem->
	    currPathItem->valid);
  /*
   * Set the path as full.  It will get reset
   *  to the whole tempPath at the next call to
   *  StartUpdatePath.
   */
  tmpAdminQItem->
    currPathItem = NULL;
  /*
   * Set the current PathQItem to the first empty path Q
   *  Item.  It is assumed that this is a virgin entry and
   *  atleast one will exist.
   */
  tmpAdminQItem->
    statsCaptureStartTimeQHead->
    currentStatsCaptureStartTimeQItem->
    statsCapturePathQHead->
    currentStatsCapturePathQItem =
    getUnusedRttMonStatsCapturePathQItem(tmpAdminQItem->
					 statsCaptureStartTimeQHead->
					 currentStatsCaptureStartTimeQItem->
					 statsCapturePathQHead);
  /*
   *  Fill in the path (source to target) and setup
   *   the distribution queues for this path
   */
  fillPathQItem(tmpAdminQItem);
  tmpAdminQItem->
    statsCaptureStartTimeQHead->
    currentStatsCaptureStartTimeQItem->
    statsCapturePathQHead->
    rttMonStatsCapturePathQFull =
    FALSE;

  return;
}

static void rttMonPerformEndHourProcessing (rttMonCtrlAdminQItem
					    * tmpAdminQItem,
					    unsigned long
					    currentTime)
{
    long tmpReturn_code;

    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Doing EndHour Processing");
	RTT_BUGINF3(0,
		    "\t in rttMonPerformEndHourProcessing, oneHourTimer popped for Probe %u\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    /* perform the end of hour processing */ 
    if (tmpAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperState !=
	D_rttMonCtrlOperState_active) {
	if (RTT_TRACE(0)) {
	    RTT_BUGINF2(0,
			"\t   this probe is not active, so don't end hour\n");
	}
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
	    RTT_BUGINF4(0,
			"\t in endHourProcessing for index %u, set to %u (milliseconds)\n",
			tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			ONEHOUR);
	}
	mgd_timer_start(&(tmpAdminQItem->
			  oneHourTimer), ONEHOUR);
	return;
    }
    if (QUEUESIZE(&(tmpAdminQItem->
		    statsCaptureStartTimeQHead->
		    rttMonStatsCaptureStartTimeQ)) == 0) {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "No Statistics to Update");
	    RTT_BUGINF2(0,
			"\t in rttMonPerformEndHourProcessing\n");
	}
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
	    RTT_BUGINF4(0,
			"\t in endHourProcessing, for index %u, set to %u (milliseconds)\n",
			tmpAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			ONEHOUR);
	}
	mgd_timer_start(&(tmpAdminQItem->
			  oneHourTimer), ONEHOUR);
	return;
    }
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Doing statistics end hour processing");
    }
    if ((tmpAdminQItem->statsCaptureStartTimeQHead->
	 currentStatsCaptureStartTimeQItem == NULL) ||
	(mgd_timer_expired(&(tmpAdminQItem->
			     oneHourTimer)))) {
	if (tmpAdminQItem->statsCaptureStartTimeQHead->
	    lastAddedStatsCaptureStartTimeQItem != NULL) {
	    tmpAdminQItem->statsCaptureStartTimeQHead->
		lastAddedStatsCaptureStartTimeQItem->
		    statsTotalsEntry->
			rttMonStatsTotalsElapsedTime =
			    currentTime -
				tmpAdminQItem->
				    statsCaptureStartTimeQHead->
					currentStatsCaptureStartTimeQItem->
					    rttMonStatsCaptureStartTimeIndex;
	}
	tmpAdminQItem->statsCaptureStartTimeQHead->
	    currentStatsCaptureStartTimeQItem =
		tmpAdminQItem->statsCaptureStartTimeQHead->
		    lastAddedStatsCaptureStartTimeQItem->next;
	if (tmpAdminQItem->statsCaptureStartTimeQHead->
	    currentStatsCaptureStartTimeQItem == NULL) {
	    /*
	     * This is called the first time through and
	     *  when the queue is full.
	     */
	    tmpAdminQItem->statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem =
		    oldestStatsCaptureStartTimeQItem(tmpAdminQItem->
						     statsCaptureStartTimeQHead);
	    /* 
	     * Scrub old entry
	     */
	    scrubRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->
						  statsCaptureStartTimeQHead->
						  currentStatsCaptureStartTimeQItem);
	}
	/* requeue in order */ 
	SET_VALID(I_queue_item_in_use,
		  tmpAdminQItem->
		  statsCaptureStartTimeQHead->
		  currentStatsCaptureStartTimeQItem->valid);
	tmpAdminQItem->
	    statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem->
		    rttMonStatsCaptureStartTimeIndex = currentTime;
	tmpReturn_code =
	    updateRttMonStatsCaptureStartTimeQItem(tmpAdminQItem->ctrlAdminEntry->
						   rttMonCtrlAdminIndex,
						   tmpAdminQItem->statsCaptureStartTimeQHead,
						   tmpAdminQItem->
						   statsCaptureStartTimeQHead->
						   currentStatsCaptureStartTimeQItem);
	if (tmpReturn_code != D_Return_Successfull) {
	    dumpRttMonProcessHeaderString(tmpAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "Bad Return Code");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t from updateRttMonStatsCaptureStartTimeQItem,\n");
	    RTT_BUGINF2(tmpAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t in rttMonPerformEndHourProcessing via SchedulerUpdate\n");
	}
	tmpAdminQItem->statsCaptureStartTimeQHead->
	    lastAddedStatsCaptureStartTimeQItem =
		tmpAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem;
    }
    /* 
     *  Set the path to the source to target
     *    path to catch any errors.  This is important,
     *    because a probe could be in the middle of an
     *    operation, and it needs a place to store the
     *    current information that is outstanding on
     *    the network.
     */
    rttMonCreateSourceToTargetVirgin(tmpAdminQItem);

    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "Timer start (oneHourTimer)");
	RTT_BUGINF4(0,
		    "\t in endHourProcessing,  for index %u, set to %u (milliseconds)\n",
		    tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    ONEHOUR);
    }
    mgd_timer_start(&(tmpAdminQItem->
		      oneHourTimer), ONEHOUR);
    return;
} 

boolean rttMonSchedulerUpdate (void)
{
    rttMonCtrlAdminQItem *tmpAdminQItem;
    rttMonCtrlAdminQItem *tmpAdminQItemNext;
    unsigned long currentTime;
    boolean aProcessDidDie;
    boolean shouldIResetSelf;
    clock_epoch tmp_time;

    if (RTT_TRACE(0)) {
	clock_get_time(&tmp_time);
	dumpRttMonProcessHeaderString(0, "rttMonSchedulerUpdate called");
	RTT_BUGINF4(0, "\t at %CC", "%v%B\n",
		    &tmp_time);
    }
    shouldIResetSelf =
	process_get_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf);
    /* 
     * Probes run at a lower Priority level than the scheduler,
     *   thus we need to allow them to run and cleanup, 
     *   before the scheduler can cleanup itself.
     */
    if (shouldIResetSelf) {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "schedulerResetSelf TRUE, so will sleep ONESEC");
	}
	process_sleep_for(ONESEC);	/* 1 Second */
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	/*
	 *  Return Die = TRUE, after the scheduler has finished its job
	 *    of cleaning up all child processes (qhead == NULL).
	 */
	if (shouldIResetSelf &&
	    (baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ.
	     qhead == NULL)) {
	    if (RTT_TRACE(0)) {
		dumpRttMonProcessHeaderString(0, "schedulerResetSelf set");
		RTT_BUGINF2(0,
			    "\t in rttMonSchedulerUpdate, returning TRUE to die\n");
	    }
	    process_unlock_semaphore(rttMonAccessSemaphore);
	    return (TRUE);
	}
	currentTime = snmp_sysUpTime();
	aProcessDidDie =
	    process_get_boolean(baseRttMonCtrlAdminQHead.aProcessDied);
	for (tmpAdminQItem =
	     baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ.
	     qhead;
	     tmpAdminQItem != NULL;
	     tmpAdminQItem = tmpAdminQItemNext) {
	    tmpAdminQItemNext = tmpAdminQItem->next;

	    process_may_suspend();

	    /*
	     * Order is important for the following 
	     *  activities.
	     */

	    /* Get rid of processes that did die */
	    if (aProcessDidDie &&
		(tmpAdminQItem->inTheProcessOfDying == TRUE) &&
		(tmpAdminQItem->ctrlAdminEntry->
		 rttMonCtrlOperState ==
		 D_rttMonCtrlOperState_inactive)) {

		rttMonCleanupProcessDeath(tmpAdminQItem);
		/* 
		 *  Don't continue, because we have
		 *   just cleaned up a process.
		 */
		continue;
	    } /* if (aProcessDidDie... */ 
	    process_may_suspend();

	    /* 
	     * EndLife Processing :  This will close
	     *  all statistics and deal with all timers
	     *  that are used while a life is active.
	     *  Thus the oneHourTimer will be handled.
	     */
	    if (mgd_timer_expired(&(tmpAdminQItem->endLifeTimer))) {
		rttMonProcessEndLife(tmpAdminQItem);
	    } else if (mgd_timer_expired(&(tmpAdminQItem->
					   oneHourTimer))) {
		/*
		 *  Check to see if this entry's hour is
		 *    up.  If endLifeTimer has expired don't
		 *    call this routine, because the endLife
		 *    processing will close out all stats.  If
		 *    this were called new stats would be setup
		 *    and possible wrap over good data.
		 */

		rttMonPerformEndHourProcessing(tmpAdminQItem,
					       currentTime);
	    }
	    process_may_suspend();

	    /* Ageout Processing */
	    if (mgd_timer_expired(&(tmpAdminQItem->ageoutTimer))) {
		rttMonProcessAgeOut(tmpAdminQItem);
	    } /* Ageout Processing */ 
	    process_may_suspend();

	    /* Start Processing */
	    if (mgd_timer_expired(&(tmpAdminQItem->
				    startTimer)) &&
		tmpAdminQItem->ctrlAdminEntry->
		rttMonCtrlAdminStatus ==
		D_rttMonCtrlAdminStatus_active) {
		rttMonStartProcessing(tmpAdminQItem);
	    } /* Start Processing */ 
	} /* for */ 
	process_may_suspend();

	/*
	 * Only one flag exists for the death of a process.  The
	 *  above loop checked each entry, so clear this boolean.
	 */
	if (aProcessDidDie) {
	    process_set_boolean(baseRttMonCtrlAdminQHead.aProcessDied, FALSE);
	    if (QUEUESIZE(&baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ)
		== 0) {
		/*
		 *  Return Die = TRUE, after the scheduler has finished its job
		 *    of cleaning up all child processes (qhead == NULL).
		 */
		if ((baseRttMonCtrlAdminQHead.rttMonCtrlAdminQ.
		     qhead == NULL) &&
		    process_get_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf)) {
		    if (RTT_TRACE(0)) {
			dumpRttMonProcessHeaderString(0, "All probes are dead and schedulerResetSelf set");
			RTT_BUGINF2(0,
				    "\t in rttMonSchedulerUpdate returning TRUE\n");
		    }
		    process_unlock_semaphore(rttMonAccessSemaphore);
		    return (TRUE);
		}
	    }
	}
    }
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (FALSE);

} 

static void rttMonUpdateHistoryBucketCurrPtr (rttMonCtrlAdminQItem
					      * inRttMonCtrlAdminQItem)
{
    rttMonHistoryCollectionBucketQHead *tmpBucketQHead;

    if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
		    rttMonHistoryCollectionLifeQ)) != 0) {
	tmpBucketQHead =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead;
	/*
	 * Set the current bucket pointer to a
	 *  new bucket.
	 */
	if (tmpBucketQHead->
	    currentHistoryCollectionBucketQItem ==
	    NULL) {
	    tmpBucketQHead->
		currentHistoryCollectionBucketQItem =
		    tmpBucketQHead->
			rttMonHistoryCollectionBucketQ.qhead;
	    SET_VALID(I_queue_item_in_use,
		      tmpBucketQHead->
		      currentHistoryCollectionBucketQItem->
		      valid);
	} else if (tmpBucketQHead->
		   currentHistoryCollectionBucketQItem->
		   rttMonHistoryCollectionBucketIndex >=
		   inRttMonCtrlAdminQItem->
		   ctrlAdminEntry->rttMonHistoryAdminNumBuckets) {
	    tmpBucketQHead->
		currentHistoryCollectionBucketQItem->
		    rttMonHistoryCollectionBucketFull =
			D_rttMonHistoryCollectionBucketFull_True;
	} else {
	    if (tmpBucketQHead->
		currentHistoryCollectionBucketQItem->
		next ==
		NULL) {
		tmpBucketQHead->
		    currentHistoryCollectionBucketQItem->
			rttMonHistoryCollectionBucketFull =
			    D_rttMonHistoryCollectionBucketFull_True;
	    }
	    /*
	     *  If the current sample has been used, move the
	     *   current bucket forward else don't waste the
	     *   current bucket.
	     */ 
	    else if ((tmpBucketQHead->
		      currentHistoryCollectionBucketQItem->
		      historyCollectionSampleQHead->
		      currentHistoryCollectionSampleQItem !=
		      NULL) &&
		     (VALID(I_queue_item_in_use,
			    tmpBucketQHead->
			    currentHistoryCollectionBucketQItem->
			    historyCollectionSampleQHead->
			    currentHistoryCollectionSampleQItem->
			    valid))) {
		tmpBucketQHead->
		    currentHistoryCollectionBucketQItem =
			tmpBucketQHead->
			    currentHistoryCollectionBucketQItem->next;
		SET_VALID(I_queue_item_in_use,
			  tmpBucketQHead->
			  currentHistoryCollectionBucketQItem->
			  valid);
		/*
		 * Set the current Sample Pointer to NULL
		 */
		tmpBucketQHead->
		    currentHistoryCollectionBucketQItem->
			historyCollectionSampleQHead->
			    currentHistoryCollectionSampleQItem = NULL;
	    } else {
		if (RTT_TRACE(inRttMonCtrlAdminQItem->
			      ctrlAdminEntry->
			      rttMonCtrlAdminIndex)) {
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "Bucket Unused, will reuse");
		}
	    }
	}
    } /* QUEUSIZE */ 
} 

static long rttMonUpdateHistorySampleCurrPtr (rttMonCtrlAdminQItem
					      * inRttMonCtrlAdminQItem,
					      OctetString * rttMonAddress)
{
    long return_code;
    rttMonHistoryCollectionBucketQHead *tmpBucketQHead;
    rttMonHistoryCollectionSampleQHead *tmpSampleQHead;

    return_code = D_Return_Successfull;

    if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
		    rttMonHistoryCollectionLifeQ)) != 0) {
	tmpBucketQHead =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead;

	tmpSampleQHead =
	    tmpBucketQHead->
		currentHistoryCollectionBucketQItem->
		    historyCollectionSampleQHead;

	if (tmpBucketQHead->
	    currentHistoryCollectionBucketQItem->
	    rttMonHistoryCollectionBucketFull ==
	    D_rttMonHistoryCollectionBucketFull_True) {
	    return_code = D_History_Buckets_Full;
	    /* 
	     *  Reset the current sample to the beginning of the queue
	     *  and update the address and valid flag.
	     */
	} else if (tmpSampleQHead->
		   currentHistoryCollectionSampleQItem ==
		   NULL) {
	    tmpSampleQHead->
		currentHistoryCollectionSampleQItem =
		    tmpSampleQHead->
			rttMonHistoryCollectionSampleQ.qhead;
	    if (tmpSampleQHead->
		currentHistoryCollectionSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionAddressShared ==
		FALSE) {
		cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					       ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       tmpSampleQHead->
					       currentHistoryCollectionSampleQItem->
					       shortHistoryCollectionEntry->
					       rttMonHistoryCollectionAddress,
					       rttMonAddress);
	    }
	    SET_VALID(I_queue_item_in_use,
		      tmpSampleQHead->
		      currentHistoryCollectionSampleQItem->
		      valid);


	    /* if current is already pointing to an unused entry
	     *  the start or end update path has been called
	     *  properly and fill the current entry
	     */
	} else if (!VALID(I_queue_item_in_use,
			  tmpSampleQHead->
			  currentHistoryCollectionSampleQItem->
			  valid)) {
	    if (tmpSampleQHead->
		currentHistoryCollectionSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionAddressShared ==
		FALSE) {
		cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					       ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       tmpSampleQHead->
					       currentHistoryCollectionSampleQItem->
					       shortHistoryCollectionEntry->
					       rttMonHistoryCollectionAddress,
					       rttMonAddress);
	    }
	    SET_VALID(I_queue_item_in_use,
		      tmpSampleQHead->
		      currentHistoryCollectionSampleQItem->
		      valid);
	}
	/* 
	 * if current points to the last entry and it has been
	 *  filled we are full 
	 */ 
	else if ((tmpSampleQHead->
		  currentHistoryCollectionSampleQItem->
		  shortHistoryCollectionEntry->
		  rttMonHistoryCollectionSampleIndex >=
		  inRttMonCtrlAdminQItem->
		  ctrlAdminEntry->
		  rttMonHistoryAdminNumSamples) &&
		 (VALID(I_queue_item_in_use,
			tmpSampleQHead->
			currentHistoryCollectionSampleQItem->
			valid))) {
	    return_code = D_Path_Too_Long;
	} else {			/* move current forward, fill it in */
	    tmpSampleQHead->
		currentHistoryCollectionSampleQItem =
		    tmpSampleQHead->
			currentHistoryCollectionSampleQItem->next;
	    if (tmpSampleQHead->
		currentHistoryCollectionSampleQItem->
		shortHistoryCollectionEntry->
		rttMonHistoryCollectionAddressShared ==
		FALSE) {
		cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					       ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       tmpSampleQHead->
					       currentHistoryCollectionSampleQItem->
					       shortHistoryCollectionEntry->
					       rttMonHistoryCollectionAddress,
					       rttMonAddress);
	    }
	    SET_VALID(I_queue_item_in_use,
		      tmpSampleQHead->
		      currentHistoryCollectionSampleQItem->
		      valid);
	}
    }
    return (return_code);
} 

static void updateOper (rttMonCtrlAdminQItem
			* inRttMonCtrlAdminQItem,
			OctetString * rttMonAddress,
			unsigned long response_time,
			int command_code)
{
    cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
				   ctrlAdminEntry->
				   rttMonCtrlAdminIndex,
				   inRttMonCtrlAdminQItem->
				   ctrlAdminEntry->
				   rttMonLatestRttOperAddress,
				   rttMonAddress);
    inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonLatestRttOperSense = command_code;
    inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonLatestRttOperCompletionTime =
	    response_time;

    return;

} 

static long updateHistory (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem,
			   OctetString * rttMonAddress,
			   unsigned long response_time,
			   int command_code,
			   boolean * value_saved)
{
    rttMonHistoryCollectionSampleQItem *tmpSampleQItem;
    long return_code = D_Return_Successfull;

    *value_saved = FALSE;
    if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
		    historyCollectionLifeQHead->
		    rttMonHistoryCollectionLifeQ)) == 0) {
	return (D_Return_Successfull);
    }
    switch (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonHistoryAdminFilter) {
    case D_rttMonHistoryAdminFilter_none:
	return (D_Return_Successfull);
	break;
    case D_rttMonHistoryAdminFilter_failures:
	if ((command_code == D_rttMonHistoryCollectionSense_ok) ||
	    (command_code ==
	     D_rttMonHistoryCollectionSense_overthreshold)) {
	    return (D_Return_Successfull);
	}
	break;
    case D_rttMonHistoryAdminFilter_overThreshold:
	if (response_time <
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonCtrlAdminThreshold) {
	    return (D_Return_Successfull);
	}
	break;
    default:
    } 
    return_code =
	rttMonUpdateHistorySampleCurrPtr(inRttMonCtrlAdminQItem,
					 rttMonAddress);
    if (return_code != D_Return_Successfull) {
	return (return_code);
    }
    tmpSampleQItem =
	inRttMonCtrlAdminQItem->
	    historyCollectionLifeQHead->
		currentHistoryCollectionLifeQItem->
		    historyCollectionBucketQHead->
			currentHistoryCollectionBucketQItem->
			    historyCollectionSampleQHead->
				currentHistoryCollectionSampleQItem;

    *value_saved = TRUE;
    tmpSampleQItem->shortHistoryCollectionEntry->
	rttMonHistoryCollectionSampleTime =
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonLatestRttOperTime;
    tmpSampleQItem->shortHistoryCollectionEntry->
	rttMonHistoryCollectionCompletionTime = response_time;
    tmpSampleQItem->shortHistoryCollectionEntry->
	rttMonHistoryCollectionSense = command_code;

    return (D_Return_Successfull);

} 

/* response time starting at 1 */ 

static rttMonStatsCaptureDistQItem *findStatsCaptureDistQItem (rttMonStatsCaptureDistQHead
							       * distQHead,
							       unsigned long response_time,
							       unsigned long distInterval)
{
    unsigned long distBucket;
    rttMonStatsCaptureDistQItem *tmpDistQItem;

    distBucket = response_time / distInterval;
    distBucket++;			/* starting with bucket of 1 */

    if (QUEUESIZE(&(distQHead->
		    rttMonStatsCaptureDistQ))
	<= distBucket) {
	return (distQHead->rttMonStatsCaptureDistQ.
		qtail);
    }
    tmpDistQItem = distQHead->rttMonStatsCaptureDistQ.
	qhead;
    while (distBucket > 1) {
	tmpDistQItem = tmpDistQItem->next;
	distBucket--;
    } 
    return (tmpDistQItem);

} 

static long updateStatsCaptureEntry (rttMonCtrlAdminQItem
				     * inRttMonCtrlAdminQItem,
				     unsigned long response_time,
				     OctetString * rttMonAddress,
				     int command_code)
{
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    rttMonStatsCaptureDistQItem *tmpDistQItem;

#if defined (NOLONGLONG)

    unsigned long mask32 = 0xffffffff;

#else

    unsigned long long mask32 = 0xffffffff;

#endif

    if (inRttMonCtrlAdminQItem->
	statsCaptureStartTimeQHead->
	currentStatsCaptureStartTimeQItem->
	statsCapturePathQHead->
	rttMonStatsCapturePathQFull == TRUE) {
	return (D_All_Paths_In_Use);
    }
    tmpHopQItem =
	findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
						statsCaptureStartTimeQHead->
						currentStatsCaptureStartTimeQItem->
						statsCapturePathQHead->
						currentStatsCapturePathQItem->
						statsCaptureHopQHead,
						rttMonAddress);
    if (tmpHopQItem == NULL) {
	return (D_Hop_Not_Found);
    }
    tmpDistQItem =
	findStatsCaptureDistQItem(tmpHopQItem->statsCaptDistQHead,
				  response_time,
				  inRttMonCtrlAdminQItem->
				  ctrlAdminEntry->
				  rttMonStatisticsAdminDistInterval);

    if (command_code ==
	D_rttMonHistoryCollectionSense_overthreshold) {
	RTT_Increment(&(tmpDistQItem->shortStatsCaptureEntry->
			rttMonStatsCaptureOverThresholds),
		      MAX_rttMonStatsCaptureOverThresholds);
    }
    RTT_Increment(&(tmpDistQItem->shortStatsCaptureEntry->
		    rttMonStatsCaptureCompletions),
		  MAX_rttMonStatsCaptureCompletions);

    tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureSumCompletionTime +=
	    response_time;

    /* 
     * 64 arithmetic for rttMonStatsCaptureSumCompletionTime2Low 
     *   and rttMonStatsCaptureSumCompletionTime2High 
     */

#if defined (NOLONGLONG)

    tmpDistQItem->accumulation += (unsigned long)
	(response_time * response_time);

#else

    tmpDistQItem->accumulation += (unsigned long long)
	(response_time * response_time);
#endif

    tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureSumCompletionTime2Low =
	    (unsigned long) (tmpDistQItem->accumulation & mask32);

#if defined (NOLONGLONG)

    tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureSumCompletionTime2High =
	    (unsigned long) (tmpDistQItem->accumulation >> 16);

#else

    tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureSumCompletionTime2High =
	    (unsigned long) (tmpDistQItem->accumulation >> 32);

#endif

    if (response_time <
	tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureCompletionTimeMin) {
	tmpDistQItem->shortStatsCaptureEntry->
	    rttMonStatsCaptureCompletionTimeMin =
		response_time;
    }
    if (response_time >
	tmpDistQItem->shortStatsCaptureEntry->
	rttMonStatsCaptureCompletionTimeMax) {
	tmpDistQItem->shortStatsCaptureEntry->
	    rttMonStatsCaptureCompletionTimeMax =
		response_time;
    }
    return (D_Return_Successfull);
} 

static void generateRttThresholdTrap (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				      boolean saved_in_history)
{
    long tmpRttMonHistoryCollectionLifeIndex;
    long tmpRttMonHistoryCollectionBucketIndex;
    long tmpRttMonHistoryCollectionSampleIndex;

    if (!saved_in_history ||
	(QUEUESIZE(&(inRttMonCtrlAdminQItem->
		     historyCollectionLifeQHead->
		     rttMonHistoryCollectionLifeQ))
	 == 0)) {
	tmpRttMonHistoryCollectionLifeIndex = 0;
	tmpRttMonHistoryCollectionBucketIndex = 0;
	tmpRttMonHistoryCollectionSampleIndex = 0;
    } else {
	tmpRttMonHistoryCollectionLifeIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			rttMonHistoryCollectionLifeIndex;
	tmpRttMonHistoryCollectionBucketIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				rttMonHistoryCollectionBucketIndex;
	tmpRttMonHistoryCollectionSampleIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				historyCollectionSampleQHead->
				    currentHistoryCollectionSampleQItem->
					shortHistoryCollectionEntry->
					    rttMonHistoryCollectionSampleIndex;
    } 
    reg_invoke_rttMonThresholdNotification_activate(inRttMonCtrlAdminQItem->
						    ctrlAdminEntry->
						    rttMonCtrlAdminIndex,
						    tmpRttMonHistoryCollectionLifeIndex,
						    tmpRttMonHistoryCollectionBucketIndex,
						    tmpRttMonHistoryCollectionSampleIndex,
						    inRttMonCtrlAdminQItem->
						    ctrlAdminEntry->
						    rttMonCtrlAdminTag,
						    inRttMonCtrlAdminQItem->
						    ctrlAdminEntry->
						    rttMonLatestRttOperAddress,
						    inRttMonCtrlAdminQItem->
						    ctrlAdminEntry->
						    rttMonCtrlOperOverThresholdOccurred);
    return;
} 

static void generateRttThresholdNmvt (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				      boolean saved_in_history)
{
    inRttMonCtrlAdminQItem = inRttMonCtrlAdminQItem;
    saved_in_history = saved_in_history;

    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlOperOverThresholdOccurred ==
	D_rttMonCtrlOperOverThresholdOccurred_true) {
	reg_invoke_Rtt_Alert(rttMonThresholdNotification_Alert,
			     inRttMonCtrlAdminQItem->
			     ctrlAdminEntry->
			     rttMonCtrlAdminIndex,
			     (void *) &(inRttMonCtrlAdminQItem->
					reactCtrlValues.
					uniqueOverThresholdToken));
    } else {
	reg_invoke_Rtt_Resolve(rttMonThresholdNotification_Resolve,
			       inRttMonCtrlAdminQItem->
			       ctrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       (void *) &(inRttMonCtrlAdminQItem->
					  reactCtrlValues.
					  uniqueOverThresholdToken));
    } 
    return;
} 

static void generateRttConnectionTrap (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				       boolean saved_in_history)
{
    long tmpRttMonHistoryCollectionLifeIndex;
    long tmpRttMonHistoryCollectionBucketIndex;
    long tmpRttMonHistoryCollectionSampleIndex;

    if (!saved_in_history ||
	(QUEUESIZE(&(inRttMonCtrlAdminQItem->
		     historyCollectionLifeQHead->
		     rttMonHistoryCollectionLifeQ))
	 == 0)) {
	tmpRttMonHistoryCollectionLifeIndex = 0;
	tmpRttMonHistoryCollectionBucketIndex = 0;
	tmpRttMonHistoryCollectionSampleIndex = 0;
    } else {
	tmpRttMonHistoryCollectionLifeIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			rttMonHistoryCollectionLifeIndex;
	tmpRttMonHistoryCollectionBucketIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				rttMonHistoryCollectionBucketIndex;
	tmpRttMonHistoryCollectionSampleIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				historyCollectionSampleQHead->
				    currentHistoryCollectionSampleQItem->
					shortHistoryCollectionEntry->
					    rttMonHistoryCollectionSampleIndex;
    } 
    reg_invoke_rttMonConnectionChangeNotification_activate(inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonCtrlAdminIndex,
							   tmpRttMonHistoryCollectionLifeIndex,
							   tmpRttMonHistoryCollectionBucketIndex,
							   tmpRttMonHistoryCollectionSampleIndex,
							   inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonCtrlAdminTag,
							   inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonLatestRttOperAddress,
							   inRttMonCtrlAdminQItem->
							   ctrlAdminEntry->
							   rttMonCtrlOperConnectionLostOccurred);
    return;
} 

static void generateRttConnectionNmvt (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				       boolean saved_in_history)
{
    inRttMonCtrlAdminQItem = inRttMonCtrlAdminQItem;
    saved_in_history = saved_in_history;

    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlOperConnectionLostOccurred ==
	D_rttMonCtrlOperConnectionLostOccurred_true) {
	reg_invoke_Rtt_Alert(rttMonConnectionChangeNotification_Alert,
			     inRttMonCtrlAdminQItem->
			     ctrlAdminEntry->
			     rttMonCtrlAdminIndex,
			     (void *) &(inRttMonCtrlAdminQItem->
					reactCtrlValues.
					uniqueConnectLossToken));
    } else {
	reg_invoke_Rtt_Resolve(rttMonConnectionChangeNotification_Resolve,
			       inRttMonCtrlAdminQItem->
			       ctrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       (void *) &(inRttMonCtrlAdminQItem->
					  reactCtrlValues.
					  uniqueConnectLossToken));
    } 
    return;
} 

static void generateRttTimeoutTrap (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				    boolean saved_in_history)
{
    long tmpRttMonHistoryCollectionLifeIndex;
    long tmpRttMonHistoryCollectionBucketIndex;
    long tmpRttMonHistoryCollectionSampleIndex;

    if (!saved_in_history ||
	(QUEUESIZE(&(inRttMonCtrlAdminQItem->
		     historyCollectionLifeQHead->
		     rttMonHistoryCollectionLifeQ))
	 == 0)) {
	tmpRttMonHistoryCollectionLifeIndex = 0;
	tmpRttMonHistoryCollectionBucketIndex = 0;
	tmpRttMonHistoryCollectionSampleIndex = 0;
    } else {
	tmpRttMonHistoryCollectionLifeIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			rttMonHistoryCollectionLifeIndex;
	tmpRttMonHistoryCollectionBucketIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				rttMonHistoryCollectionBucketIndex;
	tmpRttMonHistoryCollectionSampleIndex =
	    inRttMonCtrlAdminQItem->
		historyCollectionLifeQHead->
		    currentHistoryCollectionLifeQItem->
			historyCollectionBucketQHead->
			    currentHistoryCollectionBucketQItem->
				historyCollectionSampleQHead->
				    currentHistoryCollectionSampleQItem->
					shortHistoryCollectionEntry->
					    rttMonHistoryCollectionSampleIndex;
    } 
    reg_invoke_rttMonTimeoutNotification_activate(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  tmpRttMonHistoryCollectionLifeIndex,
						  tmpRttMonHistoryCollectionBucketIndex,
						  tmpRttMonHistoryCollectionSampleIndex,
						  inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminTag,
						  inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonLatestRttOperAddress,
						  inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlOperTimeoutOccurred);
    return;
} 

static void generateRttTimeoutNmvt (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				    boolean saved_in_history)
{
    inRttMonCtrlAdminQItem = inRttMonCtrlAdminQItem;
    saved_in_history = saved_in_history;

    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlOperTimeoutOccurred ==
	D_rttMonCtrlOperTimeoutOccurred_true) {
	reg_invoke_Rtt_Alert(rttMonTimeoutNotification_Alert,
			     inRttMonCtrlAdminQItem->
			     ctrlAdminEntry->
			     rttMonCtrlAdminIndex,
			     (void *) &(inRttMonCtrlAdminQItem->
					reactCtrlValues.
					uniqueTimeoutToken));
    } else {
	reg_invoke_Rtt_Resolve(rttMonTimeoutNotification_Resolve,
			       inRttMonCtrlAdminQItem->
			       ctrlAdminEntry->
			       rttMonCtrlAdminIndex,
			       (void *) &(inRttMonCtrlAdminQItem->
					  reactCtrlValues.
					  uniqueTimeoutToken));
    } 
    return;
} 

static void generateRttTrigger (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem)
{
    long tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex;
    rttMonReactTriggerAdminQItem *tmpReactTriggerAdminQItem;
    rttMonCtrlAdminQItem *toTriggerCtrlAdminQItem;

    tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex = 0;
    tmpReactTriggerAdminQItem =
	findNextRttMonReactTriggerAdminQItem(inRttMonCtrlAdminQItem->
					     reactTriggerAdminQHead,
					     tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    while (tmpReactTriggerAdminQItem != NULL) {
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex =
	    tmpReactTriggerAdminQItem->
		reactTriggerAdminEntry->
		    rttMonReactTriggerAdminRttMonCtrlAdminIndex;
	if ((tmpReactTriggerAdminQItem->
	     reactTriggerAdminEntry->
	     rttMonReactTriggerOperState ==
	     D_rttMonReactTriggerOperState_pending) &&
	    (tmpReactTriggerAdminQItem->
	     reactTriggerAdminEntry->
	     rttMonReactTriggerAdminStatus ==
	     D_rttMonReactTriggerAdminStatus_active)) {
	    toTriggerCtrlAdminQItem =
		findRttMonCtrlAdminQItem(tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
	    if ((toTriggerCtrlAdminQItem != NULL) &&
		(toTriggerCtrlAdminQItem->
		 ctrlAdminEntry->
		 rttMonCtrlAdminStatus ==
		 D_rttMonCtrlAdminStatus_active) &&
		(toTriggerCtrlAdminQItem->
		 ctrlAdminEntry->
		 rttMonCtrlOperState ==
		 D_rttMonCtrlOperState_pending)) {
		transitionOperState(toTriggerCtrlAdminQItem,
				    D_rttMonCtrlOperState_active);
		tmpReactTriggerAdminQItem->
		    reactTriggerAdminEntry->
			rttMonReactTriggerOperState =
			    D_rttMonReactTriggerOperState_active;
	    }
	}
	tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex++;
	tmpReactTriggerAdminQItem =
	    findNextRttMonReactTriggerAdminQItem(inRttMonCtrlAdminQItem->
						 reactTriggerAdminQHead,
						 tmpRttMonReactTriggerAdminRttMonCtrlAdminIndex);
    } 
    return;
} 

static void reactToConnectionLost (rttMonCtrlAdminQItem
				   * inRttMonCtrlAdminQItem,
				   boolean saved_in_history)
{
    switch (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminActionType) {
    case D_rttMonReactAdminActionType_trapOnly:
	generateRttConnectionTrap(inRttMonCtrlAdminQItem,
				  saved_in_history);
	break;
    case D_rttMonReactAdminActionType_nmvtOnly:
	generateRttConnectionNmvt(inRttMonCtrlAdminQItem,
				  saved_in_history);
	break;
    case D_rttMonReactAdminActionType_triggerOnly:
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapAndNmvt:
	generateRttConnectionTrap(inRttMonCtrlAdminQItem,
				  saved_in_history);
	generateRttConnectionNmvt(inRttMonCtrlAdminQItem,
				  saved_in_history);
	break;
    case D_rttMonReactAdminActionType_trapAndTrigger:
	generateRttConnectionTrap(inRttMonCtrlAdminQItem,
				  saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_nmvtAndTrigger:
	generateRttConnectionNmvt(inRttMonCtrlAdminQItem,
				  saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapNmvtAndTrigger:
	generateRttConnectionTrap(inRttMonCtrlAdminQItem,
				  saved_in_history);
	generateRttConnectionNmvt(inRttMonCtrlAdminQItem,
				  saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    default:
	break;
    } 
    return;
} 

static void toggleConnectionLost (rttMonCtrlAdminQItem
				  * inRttMonCtrlAdminQItem,
				  boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperConnectionLostOccurred ==
	D_rttMonCtrlOperConnectionLostOccurred_true) {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperConnectionLostOccurred =
		D_rttMonCtrlOperConnectionLostOccurred_false;
    } else {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperConnectionLostOccurred =
		D_rttMonCtrlOperConnectionLostOccurred_true;
    } 
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonReactAdminConnectionEnable ==
	D_rttMonReactAdminConnectionEnable_true) {
	reactToConnectionLost(inRttMonCtrlAdminQItem,
			      saved_in_history);
    }
    return;
} 

static void setConnectionLost (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
			       boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperConnectionLostOccurred ==
	D_rttMonCtrlOperConnectionLostOccurred_false) {
	toggleConnectionLost(inRttMonCtrlAdminQItem,
			     saved_in_history);
    }
    return;
} 

static void clearConnectionLost (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				 boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperConnectionLostOccurred ==
	D_rttMonCtrlOperConnectionLostOccurred_true) {
	toggleConnectionLost(inRttMonCtrlAdminQItem,
			     saved_in_history);
    }
    return;
} 

static void reactToThreshold (rttMonCtrlAdminQItem
			      * inRttMonCtrlAdminQItem,
			      boolean saved_in_history)
{
    switch (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminActionType) {
    case D_rttMonReactAdminActionType_trapOnly:
	generateRttThresholdTrap(inRttMonCtrlAdminQItem,
				 saved_in_history);
	break;
    case D_rttMonReactAdminActionType_nmvtOnly:
	generateRttThresholdNmvt(inRttMonCtrlAdminQItem,
				 saved_in_history);
	break;
    case D_rttMonReactAdminActionType_triggerOnly:
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapAndNmvt:
	generateRttThresholdTrap(inRttMonCtrlAdminQItem,
				 saved_in_history);
	generateRttThresholdNmvt(inRttMonCtrlAdminQItem,
				 saved_in_history);
	break;
    case D_rttMonReactAdminActionType_trapAndTrigger:
	generateRttThresholdTrap(inRttMonCtrlAdminQItem,
				 saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_nmvtAndTrigger:
	generateRttThresholdNmvt(inRttMonCtrlAdminQItem,
				 saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapNmvtAndTrigger:
	generateRttThresholdTrap(inRttMonCtrlAdminQItem,
				 saved_in_history);
	generateRttThresholdNmvt(inRttMonCtrlAdminQItem,
				 saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    default:
	break;
    } 
    return;
} 

static boolean checkConsecutiveThreshold (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
					  unsigned long response_time)
{
    boolean toggleOverThreshold;

    toggleOverThreshold = FALSE;
    if (inRttMonCtrlAdminQItem->reactCtrlValues.
	rttMonConsecutiveFalling) {
	if (response_time < inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->rttMonReactAdminThresholdFalling) {
	    inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds++;
	    if (inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds >=
		inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonReactAdminThresholdCount) {
		toggleOverThreshold = TRUE;
		inRttMonCtrlAdminQItem->reactCtrlValues.
		    rttMonNumberOfConsecutiveOverThresholds = 0;
		inRttMonCtrlAdminQItem->reactCtrlValues.
		    rttMonConsecutiveFalling = FALSE;
	    }
	} else {
	    inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds = 0;
	}
    } else {
	if (response_time > inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->rttMonCtrlAdminThreshold) {
	    inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds++;
	    if (inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds >=
		inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonReactAdminThresholdCount) {
		toggleOverThreshold = TRUE;
		inRttMonCtrlAdminQItem->reactCtrlValues.
		    rttMonNumberOfConsecutiveOverThresholds = 0;
		inRttMonCtrlAdminQItem->reactCtrlValues.
		    rttMonConsecutiveFalling = TRUE;
	    }
	} else {
	    inRttMonCtrlAdminQItem->reactCtrlValues.
		rttMonNumberOfConsecutiveOverThresholds = 0;
	}
    } 
    return (toggleOverThreshold);
} 

static int rttIncrementSubscriptXofY (int value)
{
    value++;
    /* subscript 0..MAX_ -1 */
    if (value > MAX_ThresholdXofYArrayBuckets - 1) {
	return (0);
    } else {
	return (value);
    }
} 

static int rttIncrementSubscriptAve (int value)
{
    value++;
    /* subscript 0..MAX_ -1 */
    if (value > MAX_ThresholdAveArrayBuckets - 1) {
	return (0);
    } else {
	return (value);
    }
} 

static int numberOfEntriesXofY (int currentSub,
				int startSub)
{
    int return_val;

    /* 
     * If wrap then return (zero to current) +
     *    (start to end)
     * (subscript 0..MAX_ -1)
     */
    if (startSub > currentSub) {
	return_val = (currentSub + 1) +
	    (startSub + (MAX_ThresholdXofYArrayBuckets - 1) + 1);
    } else {
	return_val = currentSub - startSub + 1;
    } 
    return (return_val);
} 

static int numberOfEntriesAve (int currentSub,
			       int startSub)
{
    int return_val;

    /* 
     * If wrap then return (zero to current) +
     *    (start to end)
     * (subscript 0..MAX_ -1)
     */
    if (startSub > currentSub) {
	return_val = (currentSub + 1) +
	    (startSub + (MAX_ThresholdAveArrayBuckets - 1) + 1);
    } else {
	return_val = currentSub - startSub + 1;
    } 
    return (return_val);
} 

static int countXofYOccurred (int currentSub,
			      int startSub,
			      rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem)
{
    int return_val;
    int tmp_startSub;
    boolean not_done;

    return_val = 0;
    not_done = TRUE;
    tmp_startSub = startSub;
    while (not_done) {
	/* (subscript 0..MAX_ -1) */
	if (tmp_startSub > (MAX_ThresholdXofYArrayBuckets - 1)) {
	    tmp_startSub = 0;
	}
	if (inRttMonCtrlAdminQItem->
	    reactCtrlValues.
	    rttMonXofYArray[tmp_startSub]
	    == TRUE) {
	    return_val++;
	}
	if (tmp_startSub ==
	    currentSub) {
	    not_done = FALSE;
	}
	tmp_startSub++;

    } 
    return (return_val);
} 

static boolean checkXofYThreshold (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				   unsigned long response_time)
{
    boolean toggleOverThreshold;

    toggleOverThreshold = FALSE;
    /* check valid values */
    if (inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonReactAdminThresholdCount >
	inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonReactAdminThresholdCount2) {
	return (toggleOverThreshold);
    }
    /* in Reset Mode move both */ 
    if ((inRttMonCtrlAdminQItem->
	 reactCtrlValues.
	 rttMonCurrentValueOfXofY != -1) &&
	(inRttMonCtrlAdminQItem->
	 reactCtrlValues.
	 rttMonStartValueOfXofY != -1)) {
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonCurrentValueOfXofY =
		    rttIncrementSubscriptXofY(inRttMonCtrlAdminQItem->
					      reactCtrlValues.
					      rttMonCurrentValueOfXofY);

	/* 
	 * Move the back end of the sliding 
	 * window 
	 */
	if (numberOfEntriesXofY(inRttMonCtrlAdminQItem->
				reactCtrlValues.
				rttMonCurrentValueOfXofY,
				inRttMonCtrlAdminQItem->
				reactCtrlValues.
				rttMonStartValueOfXofY) >
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminThresholdCount2) {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfXofY =
			rttIncrementSubscriptXofY(inRttMonCtrlAdminQItem->
						  reactCtrlValues.
						  rttMonStartValueOfXofY);
	}
    } else {
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonCurrentValueOfXofY = 0;
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonStartValueOfXofY = 0;
    } 
    if (inRttMonCtrlAdminQItem->
	reactCtrlValues.
	rttMonXofYFalling) {
	if (response_time <
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminThresholdFalling) {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYArray[inRttMonCtrlAdminQItem->
				    reactCtrlValues.
				    rttMonCurrentValueOfXofY]
					= TRUE;
	} else {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYArray[inRttMonCtrlAdminQItem->
				    reactCtrlValues.
				    rttMonCurrentValueOfXofY]
					= FALSE;
	} 
	if ((numberOfEntriesXofY(inRttMonCtrlAdminQItem->
				 reactCtrlValues.
				 rttMonCurrentValueOfXofY,
				 inRttMonCtrlAdminQItem->
				 reactCtrlValues.
				 rttMonStartValueOfXofY) ==
	     inRttMonCtrlAdminQItem->
	     ctrlAdminEntry->
	     rttMonReactAdminThresholdCount2) &&
	    (countXofYOccurred(inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonCurrentValueOfXofY,
			       inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonStartValueOfXofY,
			       inRttMonCtrlAdminQItem) >=
	     inRttMonCtrlAdminQItem->
	     ctrlAdminEntry->
	     rttMonReactAdminThresholdCount)) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonCurrentValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYFalling = FALSE;
	}
    } else {
	if (response_time >
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonCtrlAdminThreshold) {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYArray[inRttMonCtrlAdminQItem->
				    reactCtrlValues.
				    rttMonCurrentValueOfXofY]
					= TRUE;
	} else {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYArray[inRttMonCtrlAdminQItem->
				    reactCtrlValues.
				    rttMonCurrentValueOfXofY]
					= FALSE;
	}
	if ((numberOfEntriesXofY(inRttMonCtrlAdminQItem->
				 reactCtrlValues.
				 rttMonCurrentValueOfXofY,
				 inRttMonCtrlAdminQItem->
				 reactCtrlValues.
				 rttMonStartValueOfXofY) ==
	     inRttMonCtrlAdminQItem->
	     ctrlAdminEntry->
	     rttMonReactAdminThresholdCount2) &&
	    (countXofYOccurred(inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonCurrentValueOfXofY,
			       inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonStartValueOfXofY,
			       inRttMonCtrlAdminQItem) >=
	     inRttMonCtrlAdminQItem->
	     ctrlAdminEntry->
	     rttMonReactAdminThresholdCount)) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonCurrentValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonXofYFalling = TRUE;
	}
    } 
    return (toggleOverThreshold);
} 

static long averageOfEntries (int currentSub,
			      int startSub,
			      rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem)
{
    long long return_val;
    int tmp_startSub;
    boolean not_done;

    return_val = 0;
    not_done = TRUE;
    tmp_startSub = startSub;
    while (not_done) {
	/* (subscript 0..MAX_ -1) */
	if (tmp_startSub > (MAX_ThresholdAveArrayBuckets - 1)) {
	    tmp_startSub = 0;
	}
	return_val +=
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonAverageNominatorArray[tmp_startSub];
	if (tmp_startSub ==
	    currentSub) {
	    not_done = FALSE;
	}
	tmp_startSub++;

    } 
    return_val = return_val /
	inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonReactAdminThresholdCount;

    return ((long)return_val);
} 

static boolean checkAveThreshold (rttMonCtrlAdminQItem * inRttMonCtrlAdminQItem,
				  unsigned long response_time)
{
    boolean toggleOverThreshold;

    toggleOverThreshold = FALSE;
    if ((inRttMonCtrlAdminQItem->
	 reactCtrlValues.
	 rttMonCurrentValueOfAverage != -1) &&
	(inRttMonCtrlAdminQItem->
	 reactCtrlValues.
	 rttMonStartValueOfAverage != -1)) {
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonCurrentValueOfAverage =
		    rttIncrementSubscriptAve(inRttMonCtrlAdminQItem->
					     reactCtrlValues.
					     rttMonCurrentValueOfAverage);
	if (numberOfEntriesAve(inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonCurrentValueOfAverage,
			       inRttMonCtrlAdminQItem->
			       reactCtrlValues.
			       rttMonStartValueOfAverage) >
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminThresholdCount) {
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfAverage =
			rttIncrementSubscriptAve(inRttMonCtrlAdminQItem->
						 reactCtrlValues.
						 rttMonStartValueOfAverage);
	}
    } else {
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonCurrentValueOfAverage = 0;
	inRttMonCtrlAdminQItem->
	    reactCtrlValues.
		rttMonStartValueOfAverage = 0;
    } 
    inRttMonCtrlAdminQItem->
	reactCtrlValues.
	    rttMonAverageNominatorArray[inRttMonCtrlAdminQItem->
					reactCtrlValues.
					rttMonCurrentValueOfAverage]
					    = response_time;

    if (numberOfEntriesAve(inRttMonCtrlAdminQItem->
			   reactCtrlValues.
			   rttMonCurrentValueOfAverage,
			   inRttMonCtrlAdminQItem->
			   reactCtrlValues.
			   rttMonStartValueOfAverage) !=
	inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonReactAdminThresholdCount) {
	return (toggleOverThreshold);
    }
    if (inRttMonCtrlAdminQItem->
	reactCtrlValues.
	rttMonAverageFalling) {
	if (averageOfEntries(inRttMonCtrlAdminQItem->
			     reactCtrlValues.
			     rttMonCurrentValueOfAverage,
			     inRttMonCtrlAdminQItem->
			     reactCtrlValues.
			     rttMonStartValueOfAverage,
			     inRttMonCtrlAdminQItem) <
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminThresholdFalling) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonCurrentValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonAverageFalling = FALSE;
	}
    } else {
	if (averageOfEntries(inRttMonCtrlAdminQItem->
			     reactCtrlValues.
			     rttMonCurrentValueOfAverage,
			     inRttMonCtrlAdminQItem->
			     reactCtrlValues.
			     rttMonStartValueOfAverage,
			     inRttMonCtrlAdminQItem) >
	    inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonCtrlAdminThreshold) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonCurrentValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonStartValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		    rttMonAverageFalling = TRUE;
	}
    } 
    return (toggleOverThreshold);

} 

static void checkOverThreshold (rttMonCtrlAdminQItem
				* inRttMonCtrlAdminQItem,
				unsigned long response_time,
				boolean saved_in_history)
{
    boolean toggleOverThreshold;

    toggleOverThreshold = FALSE;
    switch (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminThresholdType) {
    case D_rttMonReactAdminThresholdType_never:
	break;
    case D_rttMonReactAdminThresholdType_immediate:
	if ((inRttMonCtrlAdminQItem->
	     reactCtrlValues.
	     rttMonImmediateFalling) &&
	    (response_time <
	     inRttMonCtrlAdminQItem->
	     ctrlAdminEntry->
	     rttMonReactAdminThresholdFalling)) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		rttMonImmediateFalling = FALSE;
	} else if (response_time >
		   inRttMonCtrlAdminQItem->
		   ctrlAdminEntry->
		   rttMonCtrlAdminThreshold) {
	    toggleOverThreshold = TRUE;
	    inRttMonCtrlAdminQItem->
		reactCtrlValues.
		rttMonImmediateFalling = TRUE;
	}
	break;
    case D_rttMonReactAdminThresholdType_consecutive:
	toggleOverThreshold =
	    checkConsecutiveThreshold(inRttMonCtrlAdminQItem,
				      response_time);
	break;
    case D_rttMonReactAdminThresholdType_xOfy:
	toggleOverThreshold =
	    checkXofYThreshold(inRttMonCtrlAdminQItem,
			       response_time);
	break;
    case D_rttMonReactAdminThresholdType_average:
	toggleOverThreshold =
	    checkAveThreshold(inRttMonCtrlAdminQItem,
			      response_time);
	break;
    default:
	dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
				      ctrlAdminEntry->
				      rttMonCtrlAdminIndex,
				   "Illegal rttMonReactAdminThresholdType");
	RTT_BUGINF2(inRttMonCtrlAdminQItem->
		    ctrlAdminEntry->
		    rttMonCtrlAdminIndex,
		    "\t in checkOverThreshold");
    } 
    if (toggleOverThreshold) {
	if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperOverThresholdOccurred ==
	    D_rttMonCtrlOperOverThresholdOccurred_true) {
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonCtrlOperOverThresholdOccurred =
		    D_rttMonCtrlOperOverThresholdOccurred_false;
	} else {
	    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		rttMonCtrlOperOverThresholdOccurred =
		    D_rttMonCtrlOperOverThresholdOccurred_true;
	}
	reactToThreshold(inRttMonCtrlAdminQItem,
			 saved_in_history);
    }
    return;
} 

static void reactToTimeout (rttMonCtrlAdminQItem
			    * inRttMonCtrlAdminQItem,
			    boolean saved_in_history)
{
    switch (inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
	    rttMonReactAdminActionType) {
    case D_rttMonReactAdminActionType_trapOnly:
	generateRttTimeoutTrap(inRttMonCtrlAdminQItem,
			       saved_in_history);
	break;
    case D_rttMonReactAdminActionType_nmvtOnly:
	generateRttTimeoutNmvt(inRttMonCtrlAdminQItem,
			       saved_in_history);
	break;
    case D_rttMonReactAdminActionType_triggerOnly:
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapAndNmvt:
	generateRttTimeoutTrap(inRttMonCtrlAdminQItem,
			       saved_in_history);
	generateRttTimeoutNmvt(inRttMonCtrlAdminQItem,
			       saved_in_history);
	break;
    case D_rttMonReactAdminActionType_trapAndTrigger:
	generateRttTimeoutTrap(inRttMonCtrlAdminQItem,
			       saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_nmvtAndTrigger:
	generateRttTimeoutNmvt(inRttMonCtrlAdminQItem,
			       saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    case D_rttMonReactAdminActionType_trapNmvtAndTrigger:
	generateRttTimeoutTrap(inRttMonCtrlAdminQItem,
			       saved_in_history);
	generateRttTimeoutNmvt(inRttMonCtrlAdminQItem,
			       saved_in_history);
	generateRttTrigger(inRttMonCtrlAdminQItem);
	break;
    default:
	break;
    } 
    return;
} 

static void toggleTimeOut (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem,
			   boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperTimeoutOccurred ==
	D_rttMonCtrlOperTimeoutOccurred_true) {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperTimeoutOccurred =
		D_rttMonCtrlOperTimeoutOccurred_false;
    } else {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlOperTimeoutOccurred =
		D_rttMonCtrlOperTimeoutOccurred_true;
    } 
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonReactAdminTimeoutEnable ==
	D_rttMonReactAdminTimeoutEnable_true) {
	reactToTimeout(inRttMonCtrlAdminQItem,
		       saved_in_history);
    }
    return;
} 

static void setTimeOut (rttMonCtrlAdminQItem
			* inRttMonCtrlAdminQItem,
			boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperTimeoutOccurred ==
	D_rttMonCtrlOperTimeoutOccurred_false) {
	toggleTimeOut(inRttMonCtrlAdminQItem,
		      saved_in_history);
    }
    return;
} 

static void clearTimeOut (rttMonCtrlAdminQItem
			  * inRttMonCtrlAdminQItem,
			  boolean saved_in_history)
{
    if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	rttMonCtrlOperTimeoutOccurred ==
	D_rttMonCtrlOperTimeoutOccurred_true) {
	toggleTimeOut(inRttMonCtrlAdminQItem,
		      saved_in_history);
    }
    return;
} 

static int setOverThreshold (rttMonCtrlAdminQItem
			     * inRttMonCtrlAdminQItem,
			     unsigned long response_time,
			     int command_code)
{
    if ((command_code != D_rttMonHistoryCollectionSense_ok) &&
	(command_code != D_rttMonHistoryCollectionSense_overthreshold)) {
	return (command_code);
    }
    if (response_time >
	inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	rttMonCtrlAdminThreshold) {
	return (D_rttMonHistoryCollectionSense_overthreshold);
    }
    return (command_code);
} 

long rttMonUpdateResponse (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem,
			   unsigned long response_time,
			   OctetString * rttMonAddress,
			   int command_code,
			   boolean initialAttempt)
{
    long return_code;
    long tmp_return_code;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    boolean collectStats;
    boolean saved_in_history;

    return_code = D_Return_Successfull;

    collectStats = TRUE;
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	command_code =
	    setOverThreshold(inRttMonCtrlAdminQItem,
			     response_time,
			     command_code);
	updateOper(inRttMonCtrlAdminQItem,
		   rttMonAddress,
		   response_time,
		   command_code);
	tmp_return_code =
	    updateHistory(inRttMonCtrlAdminQItem,
			  rttMonAddress,
			  response_time,
			  command_code,
			  &saved_in_history);
	process_may_suspend();
	/* if tmp_return_code bad, log internal Error */
	if (tmp_return_code != D_Return_Successfull) {
	    RTT_BUGINF2(inRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t Bad Return Code for updateHistory");
	    dumpRttMonLibReturnCode(inRttMonCtrlAdminQItem->
				    ctrlAdminEntry->
				    rttMonCtrlAdminIndex,
				    tmp_return_code);
	}
	if (initialAttempt ||
	    (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			 statsCaptureStartTimeQHead->
			 rttMonStatsCaptureStartTimeQ)) == 0)) {
	    collectStats = FALSE;
	    return_code = D_Return_Successfull;
	}
	switch (command_code) {
	case D_rttMonHistoryCollectionSense_ok:
	case D_rttMonHistoryCollectionSense_overthreshold:
	    if (collectStats) {
		return_code =
		    updateStatsCaptureEntry(inRttMonCtrlAdminQItem,
					    response_time,
					    rttMonAddress,
					    command_code);
	    }
	    process_may_suspend();
	    if (RttCmpOS(inRttMonCtrlAdminQItem->
			 echoAdminEntry->
			 rttMonEchoAdminTargetAddress,
			 rttMonAddress) == 0) {
		checkOverThreshold(inRttMonCtrlAdminQItem,
				   response_time,
				   saved_in_history);
		process_may_suspend();
		clearConnectionLost(inRttMonCtrlAdminQItem,
				    saved_in_history);
		process_may_suspend();
		clearTimeOut(inRttMonCtrlAdminQItem,
			     saved_in_history);
		process_may_suspend();
	    }
	    break;
	case D_rttMonHistoryCollectionSense_disconnected:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating disconnected error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectNumDisconnects),
				  MAX_rttMonStatsCollectNumDisconnects);
		}
	    }
	    if (RttCmpOS(inRttMonCtrlAdminQItem->
			 echoAdminEntry->
			 rttMonEchoAdminTargetAddress,
			 rttMonAddress) == 0) {
		process_may_suspend();
		setConnectionLost(inRttMonCtrlAdminQItem,
				  saved_in_history);
		process_may_suspend();
	    }
	    break;
	case D_rttMonHistoryCollectionSense_timeout:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating timeout error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectTimeouts),
				  MAX_rttMonStatsCollectTimeouts);
		}
	    }
	    if (RttCmpOS(inRttMonCtrlAdminQItem->
			 echoAdminEntry->
			 rttMonEchoAdminTargetAddress,
			 rttMonAddress) == 0) {
		process_may_suspend();
		setTimeOut(inRttMonCtrlAdminQItem,
			   saved_in_history);
		process_may_suspend();
	    }
	    break;
	case D_rttMonHistoryCollectionSense_busy:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating busy error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectBusies),
				  MAX_rttMonStatsCollectBusies);
		}
	    }
	    break;
	case D_rttMonHistoryCollectionSense_notconnected:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating not connected error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectNoConnections),
				  MAX_rttMonStatsCollectNoConnections);
		}
	    }
	    if (RttCmpOS(inRttMonCtrlAdminQItem->
			 echoAdminEntry->
			 rttMonEchoAdminTargetAddress,
			 rttMonAddress) == 0) {
		process_may_suspend();
		setConnectionLost(inRttMonCtrlAdminQItem,
				  saved_in_history);
		process_may_suspend();
	    }
	    break;
	case D_rttMonHistoryCollectionSense_dropped:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating dropped error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectDrops),
				  MAX_rttMonStatsCollectDrops);
		}
	    }
	    break;
	case D_rttMonHistoryCollectionSense_sequenceerror:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating sequence error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectSequenceErrors),
				  MAX_rttMonStatsCollectSequenceErrors);
		}
	    }
	    break;
	case D_rttMonHistoryCollectionSense_verifyerror:
	    if (collectStats) {
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    rttMonStatsCapturePathQFull == TRUE) {
		    /* 
		     * When end update path found the path Q Full
		     *   it reset the path to a source to target
		     *   path.
		     */
		    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						  ctrlAdminEntry->
						  rttMonCtrlAdminIndex,
						  "All Paths Full");
		    RTT_BUGINF2(inRttMonCtrlAdminQItem->
				ctrlAdminEntry->
				rttMonCtrlAdminIndex,
				"\t Updating verify error in Source-Target Path\n");
		    /*
		     *  This is a warning return code that can be overridden by a more
		     *    important error return code.
		     */
		    return_code = D_All_Paths_In_Use;
		}
		tmpHopQItem =
		    findRttMonStatsCaptureHopQItemByAddress(inRttMonCtrlAdminQItem->
							    statsCaptureStartTimeQHead->
							    currentStatsCaptureStartTimeQItem->
							    statsCapturePathQHead->
							    currentStatsCapturePathQItem->
							    statsCaptureHopQHead,
							    rttMonAddress);
		if (tmpHopQItem == NULL) {
		    return_code = D_Hop_Not_Found;
		} else {
		    RTT_Increment(&(tmpHopQItem->shortStatsCollectEntry->
				    rttMonStatsCollectVerifyErrors),
				  MAX_rttMonStatsCollectVerifyErrors);
		}
	    }
	    break;
	default:
	    /* This is used for the initial operation */
	} /* switch */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

boolean rttMonCollectPath (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem)
{
    long collectPathReturnValue;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminRttType ==
	    D_rttMonCtrlAdminRttType_pathEcho) {
	    collectPathReturnValue = TRUE;
	} else {
	    collectPathReturnValue = FALSE;
	}
    }
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (collectPathReturnValue);

} 

static void rttMonStartUpdatePathInternal (rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem)
{
    pathList *tmpPathListItem;

    /* 
     * Set all current tempPath list items to INVALID,
     *  then set the currPathItem to the beginning of
     *  the list.
     */
    tmpPathListItem = inRttMonCtrlAdminQItem->
	tempPath;
    while (tmpPathListItem != NULL) {
	CLR_VALID(I_queue_item_in_use,
		  tmpPathListItem->valid);
	tmpPathListItem = tmpPathListItem->next;
    } 
    inRttMonCtrlAdminQItem->currPathItem =
	inRttMonCtrlAdminQItem->tempPath;

    return;

} 

void rttMonStartUpdatePath (rttMonCtrlAdminQItem
			    * inRttMonCtrlAdminQItem)
{
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	rttMonStartUpdatePathInternal(inRttMonCtrlAdminQItem);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;

} 

static long rttMonUpdatePathInternal (rttMonCtrlAdminQItem
				      * inRttMonCtrlAdminQItem,
				      OctetString * rttMonAddress)
{
    long return_code;

    return_code = D_Return_Successfull;

    if (inRttMonCtrlAdminQItem->
	currPathItem == NULL) {
	return_code = D_Path_Too_Long;
    } else {
	cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
				       ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonCtrlAdminQItem->
				       currPathItem->hop,
				       rttMonAddress);
	SET_VALID(I_queue_item_in_use,
		  inRttMonCtrlAdminQItem->
		  currPathItem->valid);
	inRttMonCtrlAdminQItem->
	    currPathItem =
		inRttMonCtrlAdminQItem->
		    currPathItem->next;
    } /* else */ 
    return (return_code);

}				

/* 
 *  D_History_Buckets_Full and D_Path_Too_Long 
 *  is a warning and not a serious error.
 */ 

long rttMonUpdatePath (rttMonCtrlAdminQItem
		       * inRttMonCtrlAdminQItem,
		       OctetString * rttMonAddress)
{
    long return_code;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	return_code = rttMonUpdatePathInternal(inRttMonCtrlAdminQItem,
					       rttMonAddress);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

static void setupDistQs (rttMonCtrlAdminQItem
			 * inRttMonCtrlAdminQItem)
{
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    rttMonStatsCaptureDistQItem *tmpDistQItem;

    tmpHopQItem =
	inRttMonCtrlAdminQItem->
	    statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
			currentStatsCapturePathQItem->
			    statsCaptureHopQHead->
				rttMonStatsCaptureHopQ.qhead;
    while ((tmpHopQItem != NULL) &&
	   VALID(I_queue_item_in_use,
		 tmpHopQItem->valid)) {
	process_may_suspend();
	tmpDistQItem = tmpHopQItem->
	    statsCaptDistQHead->
		rttMonStatsCaptureDistQ.qhead;
	while (tmpDistQItem != NULL) {
	    SET_VALID(I_queue_item_in_use,
		      tmpDistQItem->valid);
	    tmpDistQItem = tmpDistQItem->next;

	} /* while (tmpDistQItem != NULL) */ 
	tmpHopQItem = tmpHopQItem->next;

    } /* while ((tmpHopQItem != NULL) */ 
}				

/* 
 *  Fill in a new pathQItem's hops etc.   
 */

static void fillPathQItem (rttMonCtrlAdminQItem
			   * inRttMonCtrlAdminQItem)
{
    pathList *tmpPathListItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;

    tmpPathQItem =
	inRttMonCtrlAdminQItem->
	    statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
			currentStatsCapturePathQItem;
    SET_VALID(I_queue_item_in_use,
	      tmpPathQItem->valid);
    tmpPathListItem =
	inRttMonCtrlAdminQItem->tempPath;
    tmpHopQItem =
	tmpPathQItem->statsCaptureHopQHead->
	    rttMonStatsCaptureHopQ.qhead;

    /*
     *  File to the max size of the HopQ.  If
     *   the hopQ is smaller than the temp
     *   path, then dump extra paths.  This only
     *   ocurrs when the History Samples (hops)
     *   is smaller than the Statistics Hops, since
     *   only one temp path is kept for both.
     */
    while ((tmpPathListItem != NULL) &&
	   VALID(I_queue_item_in_use,
		 tmpPathListItem->valid) &&
	   tmpHopQItem != NULL) {
	process_may_suspend();
	cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
				       ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       tmpHopQItem->shortStatsCollectEntry->
				       rttMonStatsCollectAddress,
				       tmpPathListItem->hop);
	SET_VALID(I_queue_item_in_use,
		  tmpHopQItem->valid);
	tmpPathListItem = tmpPathListItem->next;
	tmpHopQItem = tmpHopQItem->next;
    } 
    setupDistQs(inRttMonCtrlAdminQItem);

    return;
} 

static rttMonStatsCapturePathQItem *findPathQItem (rttMonCtrlAdminQItem
						   * inRttMonCtrlAdminQItem)
{
    pathList *tmpPathListItem;
    rttMonStatsCapturePathQItem *tmpPathQItem;
    rttMonStatsCaptureHopQItem *tmpHopQItem;
    boolean found,
    stillEqual,
    atLeastOneEqual;

    tmpPathQItem =
	inRttMonCtrlAdminQItem->
	    statsCaptureStartTimeQHead->
		currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
			rttMonStatsCapturePathQ.qhead;
    found = FALSE;
    while ((!found) &&
	   (tmpPathQItem != NULL) &&
	   VALID(I_queue_item_in_use,
		 tmpPathQItem->valid)) {
	process_may_suspend();
	tmpPathListItem =
	    inRttMonCtrlAdminQItem->tempPath;
	tmpHopQItem =
	    tmpPathQItem->statsCaptureHopQHead->
		rttMonStatsCaptureHopQ.qhead;
	stillEqual = TRUE;
	atLeastOneEqual = FALSE;
	while ((tmpPathListItem != NULL) &&
	       (tmpHopQItem != NULL) &&
	       VALID(I_queue_item_in_use,
		     tmpPathListItem->valid) &&
	       VALID(I_queue_item_in_use,
		     tmpHopQItem->valid) &&
	       stillEqual) {
	    process_may_suspend();
	    if (RttCmpOS(tmpPathListItem->hop,
			 tmpHopQItem->shortStatsCollectEntry->
			 rttMonStatsCollectAddress) == 0) {
		atLeastOneEqual = TRUE;
		tmpHopQItem = tmpHopQItem->next;
		tmpPathListItem = tmpPathListItem->next;
	    } else {
		stillEqual = FALSE;
	    }
	} /* while */
	/* 
	 * if stillEqual we got to the end of the 
	 *  logical list and the two lists are still 
	 *  equal and both logical lists are the same
	 *  size.  The atLeastOneEqual prevent two 
	 *  logical lists of the same size from being
	 *  equal. 
	 */ 
	if (stillEqual && atLeastOneEqual &&
	    (tmpPathListItem == NULL ||
	     !VALID(I_queue_item_in_use,
		    tmpPathListItem->valid)) &&
	    (tmpHopQItem == NULL ||
	     !VALID(I_queue_item_in_use,
		    tmpHopQItem->valid))) {
	    found = TRUE;
	} else {
	    tmpPathQItem = tmpPathQItem->next;
	}
    } /* while (!found) */ 
    if (found) {
	return (tmpPathQItem);
    } else {
	return (NULL);
    }
} 

long rttMonEndUpdatePath (rttMonCtrlAdminQItem
			  * inRttMonCtrlAdminQItem)
{
    long return_code;
    rttMonStatsCapturePathQItem *tmpPathQItem;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			statsCaptureStartTimeQHead->
			rttMonStatsCaptureStartTimeQ)) != 0) {
	    /* 
	     * Search for a path that already exists with
	     *  the same tempPath items.  If one is found
	     *  set the currentStatsCapturePathQItem to
	     *  this path, else setup a new path (and 
	     *  VALIDate) and set the 
	     *  currentStatsCapturePathQItem to this new
	     * path.
	     */

	    tmpPathQItem = findPathQItem(inRttMonCtrlAdminQItem);
	    if (tmpPathQItem != NULL) {
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem->
			    statsCapturePathQHead->
				currentStatsCapturePathQItem =
				    tmpPathQItem;
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem->
			    statsCapturePathQHead->
				rttMonStatsCapturePathQFull = FALSE;
	    } else {
		inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
			currentStatsCaptureStartTimeQItem->
			    statsCapturePathQHead->
				currentStatsCapturePathQItem =
				    getUnusedRttMonStatsCapturePathQItem(inRttMonCtrlAdminQItem->
									 statsCaptureStartTimeQHead->
									 currentStatsCaptureStartTimeQItem->
									 statsCapturePathQHead);
		if (inRttMonCtrlAdminQItem->
		    statsCaptureStartTimeQHead->
		    currentStatsCaptureStartTimeQItem->
		    statsCapturePathQHead->
		    currentStatsCapturePathQItem == NULL) {
		    inRttMonCtrlAdminQItem->
			statsCaptureStartTimeQHead->
			    currentStatsCaptureStartTimeQItem->
				statsCapturePathQHead->
				    rttMonStatsCapturePathQFull = TRUE;
		    return_code = D_All_Paths_In_Use;
		    /* 
		     *  Set the path to the source to target
		     *    path to catch any errors.
		     */
		    rttMonStartUpdatePathInternal(inRttMonCtrlAdminQItem);
		    rttMonUpdatePathInternal(inRttMonCtrlAdminQItem,
					     inRttMonCtrlAdminQItem->
					     echoAdminEntry->
					     rttMonEchoAdminTargetAddress);
		    tmpPathQItem = findPathQItem(inRttMonCtrlAdminQItem);
		    if (tmpPathQItem != NULL) {
			inRttMonCtrlAdminQItem->
			    statsCaptureStartTimeQHead->
				currentStatsCaptureStartTimeQItem->
				    statsCapturePathQHead->
					currentStatsCapturePathQItem =
					    tmpPathQItem;
		    } else {
			/*
			 *  This is a development error!  The Source to Target
			 *    entry must be created for every life.
			 */
			dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
						      ctrlAdminEntry->
						      rttMonCtrlAdminIndex,
						      "source->target path not setup");
			RTT_BUGINF2(inRttMonCtrlAdminQItem->
				    ctrlAdminEntry->
				    rttMonCtrlAdminIndex,
				    "\t in rttMonEndUpdatePath, this is a fatal error");
		    }
		} else {
		    fillPathQItem(inRttMonCtrlAdminQItem);
		    inRttMonCtrlAdminQItem->
			statsCaptureStartTimeQHead->
			    currentStatsCaptureStartTimeQItem->
				statsCapturePathQHead->
				    rttMonStatsCapturePathQFull = FALSE;
		}
	    } /* if (found) else */ 
	} /* QUEUESIZE */ 
	if (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			historyCollectionLifeQHead->
			rttMonHistoryCollectionLifeQ)) != 0) {
	    /* 
	     * Setup the History Bucket for this path.
	     */
	    rttMonUpdateHistoryBucketCurrPtr(inRttMonCtrlAdminQItem);
	} /* QUEUESIZE */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonGetFirstStoredHopAddress (rttMonCtrlAdminQItem
				     * inRttMonCtrlAdminQItem,
				     OctetString * rttMonAddressBuff)
{
    long return_code;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	/*   
	 *  Move the currPathItem pointer to the
	 *   first item in the tempPath list.
	 *  Clone the Address at that location
	 *   into the output buffer.
	 */
	inRttMonCtrlAdminQItem->currPathItem =
	    inRttMonCtrlAdminQItem->tempPath;
	if (!VALID(I_queue_item_in_use,
		   inRttMonCtrlAdminQItem->currPathItem->
		   valid)) {
	    return_code = D_Hop_Not_Found;
	} else {
	    cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					   ctrlAdminEntry->
					   rttMonCtrlAdminIndex,
					   rttMonAddressBuff,
					   inRttMonCtrlAdminQItem->
					   currPathItem->hop);
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
} 

long rttMonGetNextStoredHopAddress (rttMonCtrlAdminQItem
				    * inRttMonCtrlAdminQItem,
				    OctetString * rttMonAddressBuff)
{
    long return_code;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	/*   
	 *  Move the currPathItem pointer to the
	 *   next item in the tempPath list.
	 *  Clone the Address at that location
	 *   into the output buffer.
	 */
	/* 
	 *  If the caller ignored the D_Hop_Not_Found
	 *   last time and calls again the currPathItem
	 *   will be NULL.  Check for this!
	 */
	if (inRttMonCtrlAdminQItem->currPathItem
	    == NULL) {
	    return_code = D_Hop_Not_Found;
	} else {
	    inRttMonCtrlAdminQItem->currPathItem =
		inRttMonCtrlAdminQItem->currPathItem->next;
	    if ((inRttMonCtrlAdminQItem->currPathItem ==
		 NULL) ||
		(!VALID(I_queue_item_in_use,
			inRttMonCtrlAdminQItem->currPathItem->
			valid))) {
		return_code = D_Hop_Not_Found;
	    } else {
		cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					       ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       rttMonAddressBuff,
					       inRttMonCtrlAdminQItem->
					       currPathItem->hop);
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);
}				

/* 
 *  The sequence number is assumed to be in Network
 *    Byte Order.
 *  The dataSizeofReceivedBuffer is only used on a echo response.
 *  It is the size of the data portion read from the 
 *    client.
 */ 

long rttMonSetupASCIIDataPortion (rttMonCtrlAdminQItem
				  * inRttMonCtrlAdminQItem,
				  void *data,
				  long dataSizeofReceivedBuffer,
				  long *dataSizeOfSendBuffer,
				  unsigned short sequenceNumber,
				  long command)
{
    long return_code;
    long requestDataSize;
    long responseDataSize;
    long byteLoc;
    unsigned char byteVal;
    unsigned char *tmpData;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (data == NULL) {
	    return_code = D_Invalid_DataPtr;
	} else {
	    tmpData = (unsigned char *) data;
	    switch (command) {
	    case D_rttMonDataPortionCommand_logoff:
		memcpy(data, RTT_ARR_EYECATCHER, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_LOGOFF;
		*dataSizeOfSendBuffer = 6;
		break;
	    case D_rttMonDataPortionCommand_echoRequest:
		requestDataSize =
		    inRttMonCtrlAdminQItem->
			echoAdminEntry->
			    rttMonEchoAdminPktDataRequestSize;
		*dataSizeOfSendBuffer = requestDataSize + ARR_HEADER_SIZE;
		responseDataSize =
		    inRttMonCtrlAdminQItem->
			echoAdminEntry->
			    rttMonEchoAdminPktDataResponseSize;
		memcpy(data, RTT_ARR_EYECATCHER, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_ECHO_REQUEST;
		PUTSHORT(&tmpData[6], htons(sequenceNumber));
		PUTLONG(&tmpData[8], htonl(responseDataSize));
		byteLoc = 12;
		byteVal = 'A';
		while (requestDataSize > 0) {
		    tmpData[byteLoc] = byteVal;
		    byteLoc++;
		    byteVal++;
		    if (byteVal > 'Z') {
			byteVal = 'A';
		    }
		    requestDataSize--;
		} 
		break;
	    case D_rttMonDataPortionCommand_echoResponse:
		if (dataSizeofReceivedBuffer < 12) {
		    return_code = D_Invalid_DataSize;
		} else {
		    tmpData[5] = RTT_ARR_ECHO_RESPONSE;
		    responseDataSize = ntohl(GETLONG(&tmpData[8]));
		    *dataSizeOfSendBuffer = responseDataSize + ARR_HEADER_SIZE;
		    if (dataSizeofReceivedBuffer <
			(responseDataSize + ARR_HEADER_SIZE)) {
			/* byteLoc is to append onto data sent */
			byteLoc = dataSizeofReceivedBuffer;
			/* byteVal continues with what is already in the buffer */
			/*  % is remainder */
			byteVal = 'A' + ((dataSizeofReceivedBuffer - 12) % 26);
			responseDataSize -= (dataSizeofReceivedBuffer - 12);
			while (responseDataSize > 0) {
			    tmpData[byteLoc] = byteVal;
			    byteLoc++;
			    byteVal++;
			    if (byteVal > 'Z') {
				byteVal = 'A';
			    }
			    responseDataSize--;
			}
		    } else {
			byteLoc = responseDataSize + ARR_HEADER_SIZE;
			while (byteLoc < dataSizeofReceivedBuffer) {
			    tmpData[byteLoc] = 0x00;
			    byteLoc++;
			}
		    }
		} /* else invalid data size */ 
		break;
	    case D_rttMonDataPortionCommand_softwareVersionReq:
		memcpy(data, RTT_ARR_EYECATCHER, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_SW_VERS_REQ;
		*dataSizeOfSendBuffer = 6;
		break;
	    case D_rttMonDataPortionCommand_softwareVersionResp:
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_SW_VERS_RSP;
		*dataSizeOfSendBuffer = 6;
		break;
	    default:
		return_code = D_Invalid_Command;
	    } /* switch */ 
	} /* else */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/*
 *  The sequence number is assumed to be in Network
 *    Byte Order.
 *  The dataSizeofReceivedBuffer is only used on a echo response.
 *  It is the size of the data portion read from the 
 *    client.
 */ 

long rttMonSetupEBCDICDataPortion (rttMonCtrlAdminQItem
				   * inRttMonCtrlAdminQItem,
				   void *data,
				   long dataSizeofReceivedBuffer,
				   long *dataSizeOfSendBuffer,
				   unsigned short sequenceNumber,
				   long command)
{
    long return_code;
    long requestDataSize;
    long responseDataSize;
    long byteLoc;
    unsigned char byteVal;
    unsigned char *tmpData;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (data == NULL) {
	    return_code = D_Invalid_DataPtr;
	} else {
	    tmpData = (unsigned char *) data;
	    switch (command) {
	    case D_rttMonDataPortionCommand_logoff:
		memcpy(tmpData, RTT_ARR_EYECATCHER, 4);
		rttMonConvertAsciiToEbcdic(tmpData,
					   tmpData, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_LOGOFF;
		*dataSizeOfSendBuffer = 6;
		break;
	    case D_rttMonDataPortionCommand_echoRequest:
		requestDataSize =
		    inRttMonCtrlAdminQItem->
			echoAdminEntry->
			    rttMonEchoAdminPktDataRequestSize;
		responseDataSize =
		    inRttMonCtrlAdminQItem->
			echoAdminEntry->
			    rttMonEchoAdminPktDataResponseSize;
		*dataSizeOfSendBuffer = requestDataSize + ARR_HEADER_SIZE;
		memcpy(tmpData, RTT_ARR_EYECATCHER, 4);
		rttMonConvertAsciiToEbcdic(tmpData,
					   tmpData, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_ECHO_REQUEST;
		PUTSHORT(&tmpData[6], htons(sequenceNumber));
		PUTLONG(&tmpData[8], htonl(responseDataSize));

		byteLoc = 12;
		byteVal = 'A';
		while (requestDataSize > 0) {
		    tmpData[byteLoc] = rttAsciiToEbcdic[byteVal - ' '];
		    byteLoc++;
		    byteVal++;
		    if (byteVal > 'Z') {
			byteVal = 'A';
		    }
		    requestDataSize--;
		} 
		break;
	    case D_rttMonDataPortionCommand_echoResponse:
		if (dataSizeofReceivedBuffer < 12) {
		    return_code = D_Invalid_DataSize;
		} else {
		    tmpData[5] = RTT_ARR_ECHO_RESPONSE;
		    responseDataSize = ntohl(GETLONG(&tmpData[8]));
		    if (dataSizeofReceivedBuffer <
			(responseDataSize + ARR_HEADER_SIZE)) {
			/* byteLoc is to append onto data sent */
			byteLoc = dataSizeofReceivedBuffer;
			/* byteVal continues with what is already in the buffer */
			/*  % is remainder */
			byteVal = 'A' + ((dataSizeofReceivedBuffer - 12) % 26);
			responseDataSize -= (dataSizeofReceivedBuffer - 12);
			*dataSizeOfSendBuffer = responseDataSize + ARR_HEADER_SIZE;
			while (responseDataSize > 0) {
			    tmpData[byteLoc] = rttAsciiToEbcdic[byteVal - ' '];
			    byteLoc++;
			    byteVal++;
			    if (byteVal > 'Z') {
				byteVal = 'A';
			    }
			    responseDataSize--;
			}
		    } else {
			byteLoc = responseDataSize + ARR_HEADER_SIZE;
			*dataSizeOfSendBuffer = responseDataSize + ARR_HEADER_SIZE;
			while (byteLoc < dataSizeofReceivedBuffer) {
			    tmpData[byteLoc] = 0x00;
			    byteLoc++;
			}
		    }
		} /* else invalid data size */ 
		break;
	    case D_rttMonDataPortionCommand_softwareVersionReq:
		memcpy(tmpData, RTT_ARR_EYECATCHER, 4);
		rttMonConvertAsciiToEbcdic(tmpData,
					   tmpData, 4);
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_SW_VERS_REQ;
		*dataSizeOfSendBuffer = 6;
		break;
	    case D_rttMonDataPortionCommand_softwareVersionResp:
		tmpData[4] = RTT_ARR_VERSION_NUM;
		tmpData[5] = RTT_ARR_SW_VERS_RSP;
		*dataSizeOfSendBuffer = 6;
		break;
	    default:
		return_code = D_Invalid_Command;
	    } /* switch */ 
	} /* else */ 
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonVerifyASCIIDataPortion (rttMonCtrlAdminQItem
				   * inRttMonCtrlAdminQItem,
				   unsigned short sequenceNumber,
				   void *data)
{
    long return_code;
    long responseDataSize;
    long byteLoc;
    unsigned char byteVal;
    unsigned char *tmpData;


    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpData = (unsigned char *) data;
	responseDataSize =
	    inRttMonCtrlAdminQItem->
		echoAdminEntry->
		    rttMonEchoAdminPktDataResponseSize;
	if (GETSHORT(&tmpData[6]) != htons(sequenceNumber)) {
	    return_code = D_Bad_Sequence_Number;
	} else if ((memcmp(RTT_ARR_EYECATCHER, data, 4) != 0) ||
		   (tmpData[4] != RTT_ARR_VERSION_NUM) ||
		   (tmpData[5] != RTT_ARR_ECHO_RESPONSE) ||
		   (GETLONG(&tmpData[8]) != htonl(responseDataSize))) {
	    return_code = D_Bad_Data_Portion;
	} else {
	    byteLoc = 12;
	    byteVal = 'A';
	    while (responseDataSize > 0) {
		if (tmpData[byteLoc] != byteVal) {
		    return_code = D_Bad_Data_Portion;
		    responseDataSize = 0;
		    continue;
		}
		byteLoc++;
		byteVal++;
		if (byteVal > 'Z') {
		    byteVal = 'A';
		}
		responseDataSize--;
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonVerifyEBCDICDataPortion (rttMonCtrlAdminQItem
				    * inRttMonCtrlAdminQItem,
				    unsigned short sequenceNumber,
				    void *data)
{
    long return_code;
    long responseDataSize;
    long byteLoc;
    unsigned char byteVal;
    unsigned char tmpData[5];
    unsigned char *tmpDataTwo;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpDataTwo = (unsigned char *) data;
	responseDataSize =
	    inRttMonCtrlAdminQItem->
		echoAdminEntry->
		    rttMonEchoAdminPktDataResponseSize;
	memcpy(tmpData, RTT_ARR_EYECATCHER, 4);
	rttMonConvertAsciiToEbcdic(tmpData,
				   tmpData, 4);
	if (GETSHORT(&tmpDataTwo[6]) != htons(sequenceNumber)) {
	    return_code = D_Bad_Sequence_Number;
	} else if ((memcmp(tmpData, tmpDataTwo, 4) != 0) ||
		   (tmpDataTwo[4] != RTT_ARR_VERSION_NUM) ||
		   (tmpDataTwo[5] != RTT_ARR_ECHO_RESPONSE) ||
		   (GETLONG(&tmpDataTwo[8]) != htonl(responseDataSize))) {
	    return_code = D_Bad_Data_Portion;
	} else {
	    byteLoc = 12;
	    byteVal = 'A';
	    while (responseDataSize > 0) {
		if (tmpDataTwo[byteLoc] !=
		    rttAsciiToEbcdic[byteVal - ' ']) {
		    return_code = D_Bad_Data_Portion;
		    responseDataSize = 0;
		    continue;
		}
		byteLoc++;
		byteVal++;
		if (byteVal > 'Z') {
		    byteVal = 'A';
		}
		responseDataSize--;
	    }
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long getRttMonCtrlAdminFrequency (rttMonCtrlAdminQItem
				  * inRttMonCtrlAdminQItem)
{
    long ctrlAdminFrequency;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	ctrlAdminFrequency = inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlAdminFrequency;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (ctrlAdminFrequency);

} 

long getRttMonCtrlAdminTimeout (rttMonCtrlAdminQItem
				* inRttMonCtrlAdminQItem)
{
    long ctrlAdminTimeout;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	ctrlAdminTimeout = inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->rttMonCtrlAdminTimeout;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (ctrlAdminTimeout);

}				

/*  
 *   return codes are:
 *       The current operState values
 */ 

static long transitionTransitionState (rttMonCtrlAdminQItem
				       * inRttMonCtrlAdminQItem)
{
    long return_code;

    return_code = inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	    rttMonCtrlOperState;

    if (inRttMonCtrlAdminQItem->operTransitionState !=
	D_rttMonCtrlOperState_transition_not_active) {
	switch (inRttMonCtrlAdminQItem->operTransitionState) {
	case D_rttMonCtrlOperState_transition_to_pending:
	    inRttMonCtrlAdminQItem->operTransitionState =
		D_rttMonCtrlOperState_transition_not_active;
	    scrubRttMonStatsCaptureStartTimeQ(inRttMonCtrlAdminQItem->
					      statsCaptureStartTimeQHead);
	    scrubRttMonHistoryCollectionLifeQ(inRttMonCtrlAdminQItem->
					      historyCollectionLifeQHead);
	    /*
	     * Clear threshold values
	     */
	    memset(&(inRttMonCtrlAdminQItem->
		     reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    clearRttMonReactTriggerAdminEntries(inRttMonCtrlAdminQItem);
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperRttLife =
			inRttMonCtrlAdminQItem->
			    ctrlAdminEntry->
				rttMonScheduleAdminRttLife;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperNumRtts = 0;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperConnectionLostOccurred =
			D_rttMonCtrlOperConnectionLostOccurred_false;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperTimeoutOccurred =
			D_rttMonCtrlOperTimeoutOccurred_false;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperOverThresholdOccurred =
			D_rttMonCtrlOperOverThresholdOccurred_false;
	    if (RTT_TRACE(0)) {
	      dumpRttMonProcessHeaderString(0, "Timer stop (endLifeTimer)");
	      RTT_BUGINF3(0,
			  "\t in transitionTransitionState for index %u\n",
			  inRttMonCtrlAdminQItem->ctrlAdminEntry->
			  rttMonCtrlAdminIndex);
	    }
	    mgd_timer_stop(&(inRttMonCtrlAdminQItem->
			     endLifeTimer));
	    inRttMonCtrlAdminQItem->
	      actualStartTime = 0;
	    setOperState(inRttMonCtrlAdminQItem,
			 D_rttMonCtrlOperState_pending);
	    return_code =
		D_rttMonCtrlOperState_pending;
	    break;
	case D_rttMonCtrlOperState_transition_to_inactive:
	    inRttMonCtrlAdminQItem->operTransitionState =
		D_rttMonCtrlOperState_transition_not_active;
	    /*
	     *  Clear threshold values
	     */
	    memset(&(inRttMonCtrlAdminQItem->
		     reactCtrlValues), 0,
		   sizeof(rttMonReactCtrlValues));
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfXofY = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonStartValueOfAverage = -1;
	    inRttMonCtrlAdminQItem->reactCtrlValues.rttMonCurrentValueOfAverage = -1;
	    doEndLife(inRttMonCtrlAdminQItem);
	    clearRttMonReactTriggerAdminEntries(inRttMonCtrlAdminQItem);
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperConnectionLostOccurred =
			D_rttMonCtrlOperConnectionLostOccurred_false;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperTimeoutOccurred =
			D_rttMonCtrlOperTimeoutOccurred_false;
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperOverThresholdOccurred =
			D_rttMonCtrlOperOverThresholdOccurred_false;
	    if (RTT_TRACE(0)) {
		dumpRttMonProcessHeaderString(0, "Timer stop (oneHourTimer)");
		RTT_BUGINF3(0,
			    "\t in transitionTransitionState for index %u\n",
			    inRttMonCtrlAdminQItem->ctrlAdminEntry->
			    rttMonCtrlAdminIndex);
	    }
	    mgd_timer_stop(&(inRttMonCtrlAdminQItem->oneHourTimer));
	    setOperState(inRttMonCtrlAdminQItem,
			 D_rttMonCtrlOperState_inactive);
	    return_code =
		D_rttMonCtrlOperState_inactive;
	    break;
	default:
	    dumpRttMonProcessHeaderString(inRttMonCtrlAdminQItem->
					  ctrlAdminEntry->
					  rttMonCtrlAdminIndex,
					  "bad operTransitionState");
	    RTT_BUGINF2(inRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlAdminIndex,
			"\t In transitionTransitionState");
	}
    }
    return (return_code);
}				

/*
 *  This is called to clear a transition state.
 */ 

long rttMonTransitionTransitionState (rttMonCtrlAdminQItem
				      * inRttMonCtrlAdminQItem)
{
    long transitionState;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	transitionState =
	    transitionTransitionState(inRttMonCtrlAdminQItem);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (transitionState);
} 

long getRttMonOperState (rttMonCtrlAdminQItem
			 * inRttMonCtrlAdminQItem)
{
    long operState;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	switch (inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		rttMonCtrlOperState) {
	case D_rttMonCtrlOperState_reset:
	    inRttMonCtrlAdminQItem->
		operTransitionState =
		    D_rttMonCtrlOperState_transition_to_pending;
	    operState =
		D_rttMonCtrlOperState_transition_to_pending;
	    break;
	case D_rttMonCtrlOperState_orderlyStop:
	case D_rttMonCtrlOperState_immediateStop:
	    inRttMonCtrlAdminQItem->
		operTransitionState =
		    D_rttMonCtrlOperState_transition_to_inactive;
	    operState =
		D_rttMonCtrlOperState_transition_to_inactive;
	    break;
	default:
	    operState = inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlOperState;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (operState);

} 

void rttMonTimeStampEchoStart (rttMonCtrlAdminQItem
			       * inRttMonCtrlAdminQItem)
{
    sys_timestamp currentTimeMs;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonLatestRttOperTime = snmp_sysUpTime();
	GET_TIMESTAMP(currentTimeMs);
	inRttMonCtrlAdminQItem->currentRttStartTime =
	    currentTimeMs;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;
} 

unsigned long rttMonCalculateResponseTime (rttMonCtrlAdminQItem
					   * inRttMonCtrlAdminQItem)
{
    sys_timestamp currentTimeMs;
    unsigned long responseTime;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	GET_TIMESTAMP(currentTimeMs);
	responseTime = 
	    CLOCK_DIFF_UNSIGNED(inRttMonCtrlAdminQItem->
				currentRttStartTime,
				currentTimeMs);
	if (responseTime == 0) {
	    responseTime = 1;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (responseTime);
} 

void rttMonProbePanicInternal (rttMonCtrlAdminQItem
			       * inRttMonCtrlAdminQItem)
{
    if (RTT_TRACE(0)) {
	dumpRttMonProcessHeaderString(0, "OperState Change");
	RTT_BUGINF3(0,
		    "\t in rttMonProbePanicInternal, for index %u, to inactive\n",
		    inRttMonCtrlAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex);
    }
    inRttMonCtrlAdminQItem->
	ctrlAdminEntry->
	    rttMonCtrlOperState =
		D_rttMonCtrlOperState_inactive;
    setOperState(inRttMonCtrlAdminQItem,
		 D_rttMonCtrlOperState_inactive);
    return;
} 

void rttMonProbePanic (rttMonCtrlAdminQItem
		       * inRttMonCtrlAdminQItem)
{
    /* 
     * Set the processes state to inactive via the 
     *  operState variable.  Then call setOperState
     *  to set internal state to inactive, so the 
     *  ageout timer gets started.
     */
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (RTT_TRACE(0)) {
	    dumpRttMonProcessHeaderString(0, "OperState Change");
	    RTT_BUGINF3(0,
			"\t in rttMonProbePanic, for index %u, to inactive\n",
			inRttMonCtrlAdminQItem->ctrlAdminEntry->
			rttMonCtrlAdminIndex);
	}
	inRttMonCtrlAdminQItem->
	    ctrlAdminEntry->
		rttMonCtrlOperState =
		    D_rttMonCtrlOperState_inactive;
	setOperState(inRttMonCtrlAdminQItem,
		     D_rttMonCtrlOperState_inactive);

    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;

} 

void rttMonUpdateNumberOfEchosAttempted (rttMonCtrlAdminQItem
					 * inRttMonCtrlAdminQItem,
					 boolean
					 initialAttempt)
{
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if ((!initialAttempt) &&
	    (QUEUESIZE(&(inRttMonCtrlAdminQItem->
			 statsCaptureStartTimeQHead->
			 rttMonStatsCaptureStartTimeQ)) != 0)) {
	    RTT_Increment(&(inRttMonCtrlAdminQItem->
			    statsCaptureStartTimeQHead->
			    currentStatsCaptureStartTimeQItem->
			    statsTotalsEntry->
			    rttMonStatsTotalsInitiations),
			  MAX_rttMonStatsTotalsInitiations);
	}
	RTT_Increment(&(inRttMonCtrlAdminQItem->
			ctrlAdminEntry->
			rttMonCtrlOperNumRtts),
		      MAX_rttMonCtrlOperNumRtts);

    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;

} 

boolean rttMonHopConnected (rttMonCtrlAdminQItem
			    * inRttMonCtrlAdminQItem,
			    OctetString * rttMonAddress)
{
    boolean returnHopConnected;
    connectHandleList *tmpConnHandle;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpConnHandle =
	    findConnectionHandle(inRttMonCtrlAdminQItem->
				 connHandleList,
				 rttMonAddress);
	if (tmpConnHandle == NULL) {
	    returnHopConnected = FALSE;
	} else {
	    returnHopConnected =
		tmpConnHandle->connected;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (returnHopConnected);

} 

long rttMonSetHopConnectedState (rttMonCtrlAdminQItem
				 * inRttMonCtrlAdminQItem,
				 OctetString * rttMonAddress,
				 boolean state)
{
    long return_code;
    connectHandleList *tmpConnHandle;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpConnHandle =
	    findConnectionHandle(inRttMonCtrlAdminQItem->
				 connHandleList,
				 rttMonAddress);
	if (tmpConnHandle == NULL) {
	    return_code = D_Hop_Not_Found;
	} else {
	    tmpConnHandle->connected = state;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonSetConnectionHandle (rttMonCtrlAdminQItem
				* inRttMonCtrlAdminQItem,
				OctetString * rttMonAddress,
				void *connHandle)
{
    long return_code;
    connectHandleList *tmpConnHandle;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpConnHandle =
	    findConnectionHandle(inRttMonCtrlAdminQItem->
				 connHandleList,
				 rttMonAddress);
	if (tmpConnHandle == NULL) {
	    tmpConnHandle =
		getUnusedConnectionHandleItem(inRttMonCtrlAdminQItem->
					      connHandleList);
	    if (tmpConnHandle == NULL) {
		return_code = D_Conn_Handle_List_Full;
	    } else {
		tmpConnHandle->connHandle = connHandle;
		cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
					       ctrlAdminEntry->
					       rttMonCtrlAdminIndex,
					       tmpConnHandle->connAddress,
					       rttMonAddress);
		SET_VALID(I_queue_item_in_use,
			  tmpConnHandle->valid);
	    }
	} else {
	    tmpConnHandle->connHandle = connHandle;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonGetConnectionHandle (rttMonCtrlAdminQItem
				* inRttMonCtrlAdminQItem,
				OctetString * rttMonAddress,
				void **connHandle)
{
    connectHandleList *tmpConnHandle;
    long return_code;

    return_code = D_Return_Successfull;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	tmpConnHandle =
	    findConnectionHandle(inRttMonCtrlAdminQItem->
				 connHandleList,
				 rttMonAddress);
	if (tmpConnHandle != NULL) {
	    *connHandle = tmpConnHandle->connHandle;
	} else {
	    return_code = D_Hop_Not_Found;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

}				

/* 
 *  This helper routine allocates memory and the
 *  owner of the memory is the caller.
 *  After freeing the connection Handles the
 *  caller must call rttMonFreeConnectionHandleList
 *  to free the list.
 */ 

connectHandleList *rttMonNewConnectionHandleList (rttMonCtrlAdminQItem
						  * inRttMonCtrlAdminQItem)
{
    connectHandleList *connHandleList;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	connHandleList =
	    newConnectionHandleListParent(inRttMonCtrlAdminQItem,
					  0,
					  NULL,
					  0);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (connHandleList);
} 

long rttMonGetConnectionHandleList (rttMonCtrlAdminQItem
				    * inRttMonCtrlAdminQItem,
				    connectHandleList
				    * connHandleListBuff)
{
    long return_code;

    return_code = D_Return_Successfull;

    if (connHandleListBuff == NULL) {
	return (D_Conn_Handle_List_Invalid);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	return_code =
	    collectConnectHandleList(connHandleListBuff,
				     inRttMonCtrlAdminQItem->
				     connHandleList);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

long rttMonSetDiagText (rttMonCtrlAdminQItem
			* inRttMonCtrlAdminQItem,
			OctetString
			* diagTextOctetString)
{
    long return_code;

    return_code = D_Return_Successfull;

    if (diagTextOctetString == NULL) {
	return (D_Invalid_DataPtr);
    }
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	cloneToPreAllocatedOctetString(inRttMonCtrlAdminQItem->
				       ctrlAdminEntry->
				       rttMonCtrlAdminIndex,
				       inRttMonCtrlAdminQItem->
				       ctrlAdminEntry->
				       rttMonCtrlOperDiagText,
				       diagTextOctetString);
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (return_code);

} 

boolean rttMonGetVerifyDataFlag (rttMonCtrlAdminQItem
				 * inRttMonCtrlAdminQItem)
{
    boolean verifyDataFlag;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	if (inRttMonCtrlAdminQItem->ctrlAdminEntry->
	    rttMonCtrlAdminVerifyData ==
	    D_rttMonCtrlAdminVerifyData_true) {
	    verifyDataFlag = TRUE;
	} else {
	    verifyDataFlag = FALSE;
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (verifyDataFlag);

} 

watched_boolean *rttMonGetPendingBoolean (rttMonCtrlAdminQItem
					  * inRttMonCtrlAdminQItem)
{
    watched_boolean *pendingBoolean;

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	pendingBoolean = inRttMonCtrlAdminQItem->
	    pendingBoolean;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return (pendingBoolean);

} 

void setRttMonCtrlAdminEntryDefaults (rttMonCtrlAdminEntry_t
				      * inRttMonCtrlAdminEntry)
{
    /*
     * The index is already set
     */
    if (inRttMonCtrlAdminEntry->rttMonCtrlAdminOwner ==
	NULL) {
	inRttMonCtrlAdminEntry->rttMonCtrlAdminOwner =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonCtrlAdminOwner));
    }
    populateOctetString(inRttMonCtrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminEntry->
			rttMonCtrlAdminOwner,
			DEF_rttMonCtrlAdminOwner,
			strlen(DEF_rttMonCtrlAdminOwner));
    SET_VALID(I_rttMonCtrlAdminOwner,
	      inRttMonCtrlAdminEntry->valid);
    if (inRttMonCtrlAdminEntry->rttMonCtrlAdminTag ==
	NULL) {
	inRttMonCtrlAdminEntry->rttMonCtrlAdminTag =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonCtrlAdminTag));
    }
    populateOctetString(inRttMonCtrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminEntry->
			rttMonCtrlAdminTag,
			DEF_rttMonCtrlAdminTag,
			strlen(DEF_rttMonCtrlAdminTag));
    SET_VALID(I_rttMonCtrlAdminTag,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminRttType =
	DEF_rttMonCtrlAdminRttType;
    SET_VALID(I_rttMonCtrlAdminRttType,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminThreshold =
	DEF_rttMonCtrlAdminThreshold;
    SET_VALID(I_rttMonCtrlAdminThreshold,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminFrequency =
	DEF_rttMonCtrlAdminFrequency;
    SET_VALID(I_rttMonCtrlAdminFrequency,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminTimeout =
	DEF_rttMonCtrlAdminTimeout;
    SET_VALID(I_rttMonCtrlAdminFrequency,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminVerifyData =
	DEF_rttMonCtrlAdminVerifyData;
    SET_VALID(I_rttMonCtrlAdminVerifyData,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlAdminStatus =
	DEF_rttMonCtrlAdminStatus;
    SET_VALID(I_rttMonCtrlAdminStatus,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonScheduleAdminRttLife =
	DEF_rttMonScheduleAdminRttLife;
    SET_VALID(I_rttMonScheduleAdminRttLife,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonScheduleAdminRttStartTime =
	DEF_rttMonScheduleAdminRttStartTime;
    SET_VALID(I_rttMonScheduleAdminRttStartTime,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonScheduleAdminConceptRowAgeout =
	DEF_rttMonScheduleAdminConceptRowAgeout;
    SET_VALID(I_rttMonScheduleAdminConceptRowAgeout,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminConnectionEnable =
	DEF_rttMonReactAdminConnectionEnable;
    SET_VALID(I_rttMonReactAdminConnectionEnable,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminTimeoutEnable =
	DEF_rttMonReactAdminTimeoutEnable;
    SET_VALID(I_rttMonReactAdminTimeoutEnable,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminThresholdType =
	DEF_rttMonReactAdminThresholdType;
    SET_VALID(I_rttMonReactAdminThresholdType,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminThresholdFalling =
	DEF_rttMonReactAdminThresholdFalling;
    SET_VALID(I_rttMonReactAdminThresholdFalling,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount =
	DEF_rttMonReactAdminThresholdCount;
    SET_VALID(I_rttMonReactAdminThresholdCount,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount2 =
	DEF_rttMonReactAdminThresholdCount2;
    SET_VALID(I_rttMonReactAdminThresholdCount2,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonReactAdminActionType =
	DEF_rttMonReactAdminActionType;
    SET_VALID(I_rttMonReactAdminActionType,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumHourGroups =
	DEF_rttMonStatisticsAdminNumHourGroups;
    SET_VALID(I_rttMonStatisticsAdminNumHourGroups,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumPaths =
	DEF_rttMonStatisticsAdminNumPaths;
    SET_VALID(I_rttMonStatisticsAdminNumPaths,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumHops =
	DEF_rttMonStatisticsAdminNumHops;
    SET_VALID(I_rttMonStatisticsAdminNumHops,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonStatisticsAdminNumDistBuckets =
	DEF_rttMonStatisticsAdminNumDistBuckets;
    SET_VALID(I_rttMonStatisticsAdminNumDistBuckets,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonStatisticsAdminDistInterval =
	DEF_rttMonStatisticsAdminDistInterval;
    SET_VALID(I_rttMonStatisticsAdminDistInterval,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonHistoryAdminNumLives =
	DEF_rttMonHistoryAdminNumLives;
    SET_VALID(I_rttMonHistoryAdminNumLives,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonHistoryAdminNumBuckets =
	DEF_rttMonHistoryAdminNumBuckets;
    SET_VALID(I_rttMonHistoryAdminNumBuckets,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonHistoryAdminNumSamples =
	DEF_rttMonHistoryAdminNumSamples;
    SET_VALID(I_rttMonHistoryAdminNumSamples,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonHistoryAdminFilter =
	DEF_rttMonHistoryAdminFilter;
    SET_VALID(I_rttMonHistoryAdminFilter,
	      inRttMonCtrlAdminEntry->valid);

    /*
     *  Non user writeable defaults
     */

    inRttMonCtrlAdminEntry->rttMonCtrlOperModificationTime = 0;
    SET_VALID(I_rttMonCtrlOperModificationTime,
	      inRttMonCtrlAdminEntry->valid);
    if (inRttMonCtrlAdminEntry->rttMonCtrlOperDiagText ==
	NULL) {
	inRttMonCtrlAdminEntry->rttMonCtrlOperDiagText =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonCtrlOperDiagText));
    }
    populateOctetString(inRttMonCtrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminEntry->
			rttMonCtrlOperDiagText,
			DEF_rttMonCtrlOperDiagText,
			strlen(DEF_rttMonCtrlOperDiagText));
    SET_VALID(I_rttMonCtrlOperDiagText,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperResetTime = 0;
    SET_VALID(I_rttMonCtrlOperResetTime,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperOctetsInUse = 0;
    SET_VALID(I_rttMonCtrlOperOctetsInUse,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperConnectionLostOccurred =
	DEF_rttMonCtrlOperConnectionLostOccurred;
    SET_VALID(I_rttMonCtrlOperConnectionLostOccurred,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperTimeoutOccurred =
	DEF_rttMonCtrlOperTimeoutOccurred;
    SET_VALID(I_rttMonCtrlOperTimeoutOccurred,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperOverThresholdOccurred =
	DEF_rttMonCtrlOperOverThresholdOccurred;
    SET_VALID(I_rttMonCtrlOperOverThresholdOccurred,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperNumRtts = 0;
    SET_VALID(I_rttMonCtrlOperNumRtts,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperRttLife = 0;
    SET_VALID(I_rttMonCtrlOperRttLife,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonCtrlOperState =
	DEF_rttMonCtrlOperState;
    SET_VALID(I_rttMonCtrlOperState,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonLatestRttOperCompletionTime = 0;
    SET_VALID(I_rttMonLatestRttOperCompletionTime,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonLatestRttOperSense = 0;
    SET_VALID(I_rttMonLatestRttOperSense,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonLatestRttOperApplSpecificSense = 0;
    SET_VALID(I_rttMonLatestRttOperApplSpecificSense,
	      inRttMonCtrlAdminEntry->valid);
    if (inRttMonCtrlAdminEntry->rttMonLatestRttOperSenseDescription ==
	NULL) {
	inRttMonCtrlAdminEntry->rttMonLatestRttOperSenseDescription =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonLatestRttOperSenseDescription));
    }
    populateOctetString(inRttMonCtrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminEntry->
			rttMonLatestRttOperSenseDescription,
			DEF_rttMonLatestRttOperSenseDescription,
			strlen(DEF_rttMonLatestRttOperSenseDescription));
    SET_VALID(I_rttMonLatestRttOperSenseDescription,
	      inRttMonCtrlAdminEntry->valid);
    inRttMonCtrlAdminEntry->rttMonLatestRttOperTime = 0;
    SET_VALID(I_rttMonLatestRttOperTime,
	      inRttMonCtrlAdminEntry->valid);
    if (inRttMonCtrlAdminEntry->rttMonLatestRttOperAddress ==
	NULL) {
	inRttMonCtrlAdminEntry->rttMonLatestRttOperAddress =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonEchoAdminTargetAddress));
    }
    populateOctetString(inRttMonCtrlAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonCtrlAdminEntry->
			rttMonLatestRttOperAddress,
			DEF_rttMonEchoAdminTargetAddress,
			strlen(DEF_rttMonEchoAdminTargetAddress));
    SET_VALID(I_rttMonEchoAdminTargetAddress,
	      inRttMonCtrlAdminEntry->valid);

    return;
} 

void setRttMonEchoAdminEntryDefaults (rttMonEchoAdminEntry_t
				      * inRttMonEchoAdminEntry)
{
    /*
     * The index is already set
     */
    /*
     * Don't set rttMonEchoAdminProtocol and
     *   rttMonEchoAdminTargetAddress valid
     */
    inRttMonEchoAdminEntry->rttMonEchoAdminProtocol =
	DEF_rttMonEchoAdminProtocol;
    if (inRttMonEchoAdminEntry->rttMonEchoAdminTargetAddress ==
	NULL) {
	inRttMonEchoAdminEntry->rttMonEchoAdminTargetAddress =
	    preMallocOctetString(0,
				 strlen(DEF_rttMonEchoAdminTargetAddress));
    }
    populateOctetString(inRttMonEchoAdminEntry->
			rttMonCtrlAdminIndex,
			inRttMonEchoAdminEntry->
			rttMonEchoAdminTargetAddress,
			DEF_rttMonEchoAdminTargetAddress,
			strlen(DEF_rttMonEchoAdminTargetAddress));
    inRttMonEchoAdminEntry->rttMonEchoAdminPktDataRequestSize =
	DEF_rttMonEchoAdminPktDataRequestSize;
    SET_VALID(I_rttMonEchoAdminPktDataRequestSize,
	      inRttMonEchoAdminEntry->valid);
    inRttMonEchoAdminEntry->rttMonEchoAdminPktDataResponseSize =
	DEF_rttMonEchoAdminPktDataResponseSize;
    SET_VALID(I_rttMonEchoAdminPktDataResponseSize,
	      inRttMonEchoAdminEntry->valid);

    return;
} 

rttMonAppl_t *getRttMonApplPtr (void)
{
    return (&rttMonApplEntry);
}

void setRttMonAppPreConfigedReset (void)
{
    updateRttMonApplTimeOfLastSet(snmp_sysUpTime());

    /* 
     * we don't keep this table
     */

    return;
} 

void setRttMonApplReset (void)
{
    long localRttMonCtrlAdminIndex;
    rttMonCtrlAdminQItem *tmpAdminQItem;
    int return_code, oldAdminStatus;

    updateRttMonApplTimeOfLastSet(snmp_sysUpTime());

    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	/*
	 * For each entry, either:
	 *   If a probe exists or is dying, mark the 
	 *     entry to be destroyed and tell the probe 
	 *     to die
	 *   else just delete the entry
	 */
	localRttMonCtrlAdminIndex = 0;
	tmpAdminQItem =
	    findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	while (tmpAdminQItem != NULL) {
	    localRttMonCtrlAdminIndex =
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminIndex;
	    localRttMonCtrlAdminIndex++;

	    if (tmpAdminQItem->ctrlAdminEntry->
		rttMonCtrlOperState !=
		D_rttMonCtrlOperState_inactive) {
		tmpAdminQItem->
		    restartPending = FALSE;
		/* 
		 *  Tell the probe to kill itself and
		 *    have the scheduler clean up.
		 */ 
		oldAdminStatus = tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminStatus;
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminStatus =
			D_rttMonCtrlAdminStatus_destroy;
		return_code = transitionOperState(tmpAdminQItem,
						  D_rttMonCtrlOperState_immediateStop);
		if (return_code != D_Return_Successfull) {
		    tmpAdminQItem->ctrlAdminEntry->
		      rttMonCtrlAdminStatus = oldAdminStatus;
		    if (RTT_TRACE(0)) {
		        dumpRttMonProcessHeaderString(0, "Operational State Change Failed");
			RTT_BUGINF3(0,
				    "\t in setRttMonApplReset, for index %u, to immediateStop\n",
				    tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex);
		    }
		} else {
		    if (RTT_TRACE(0)) {
		        dumpRttMonProcessHeaderString(0, "AdminStatus Change");
			RTT_BUGINF3(0,
				    "\t in setRttMonApplReset, for index %u, to destroy\n",
				    tmpAdminQItem->ctrlAdminEntry->
				    rttMonCtrlAdminIndex);
		    }
		}
	    } else if (tmpAdminQItem->inTheProcessOfDying ==
		       FALSE) {
		/*
		 *  Just cleanup the inactive entry.
		 */
		unqueue(&(baseRttMonCtrlAdminQHead.
			  rttMonCtrlAdminQ),
			tmpAdminQItem);
		tmpAdminQItem->next = NULL;
		if ((baseRttMonCtrlAdminQHead.
		     lastSearchedCtrlAdminQItem != NULL) &&
		    (baseRttMonCtrlAdminQHead.
		     lastSearchedCtrlAdminQItem ==
		     tmpAdminQItem)) {
		    baseRttMonCtrlAdminQHead.
			lastSearchedCtrlAdminQItem = NULL;
		}
		freeRttMonCtrlAdminQItemMem(tmpAdminQItem);
	    } else if (tmpAdminQItem->inTheProcessOfDying ==
		       TRUE) {
		tmpAdminQItem->
		    restartPending = FALSE;
		if (RTT_TRACE(0)) {
		    dumpRttMonProcessHeaderString(0, "AdminStatus Change");
		    RTT_BUGINF3(0,
				"\t in setRttMonApplReset, for index %u, to destroy\n",
				tmpAdminQItem->ctrlAdminEntry->
				rttMonCtrlAdminIndex);
		}
		/* 
		 * Make sure the entry is cleaned up when
		 *  the probe dies.
		 */ 
		tmpAdminQItem->ctrlAdminEntry->
		    rttMonCtrlAdminStatus =
			D_rttMonCtrlAdminStatus_destroy;
	    }
	    tmpAdminQItem =
		findNextRttMonCtrlAdminQItem(localRttMonCtrlAdminIndex);
	}
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    if (baseRttMonCtrlAdminQHead.schedulerActive) {
	process_set_boolean(baseRttMonCtrlAdminQHead.schedulerResetSelf, TRUE);
    }
    rttMonApplEntry.rttMonApplReset =
	D_rttMonApplReset_reset;

    return;
} 

rttMonApplSupportedRttTypesEntry_t
*getRttMonApplSupportedRttTypesEntryPtr (long
					 rttMonApplSupportedRttTypes)
{
    if ((rttMonApplSupportedRttTypes < 1) ||
	(rttMonApplSupportedRttTypes >= MAX_RTT_TYPES)) {
	return (NULL);
    }
    return (&rttMonApplSupportedRttTypesTable[rttMonApplSupportedRttTypes]);
} 

rttMonApplSupportedRttTypesEntry_t
*getNextRttMonApplSupportedRttTypesEntryPtr (long
					     rttMonApplSupportedRttTypes)
{
    if (rttMonApplSupportedRttTypes < 1) {
	rttMonApplSupportedRttTypes = 1;
    }
    return (getRttMonApplSupportedRttTypesEntryPtr(rttMonApplSupportedRttTypes));
} 

static void populateSupportedProtocols (void)
{
    int i;

    for (i = D_rttMonApplSupportedProtocols_notApplicable;
	 i < MAX_RTT_PROTOCOLS;
	 i++) {
	/* if by default it is not valid, skip it */
	if (rttMonApplSupportedProtocolsTable[i].
	    rttMonApplSupportedProtocolsValid ==
	    D_rttMonApplSupportedProtocolsValid_true) {
	    if (reg_invoke_rtt_echo_exists(i) ==
		FALSE) {
		rttMonApplSupportedProtocolsTable[i].
		    rttMonApplSupportedProtocolsValid =
			D_rttMonApplSupportedProtocolsValid_false;
	    }
	} /* rttMonApplSupportedProtocolsTable[i].valid */ 
    }
} 

rttMonApplSupportedProtocolsEntry_t
*getRttMonApplSupportedProtocolsEntryPtr (long rttMonApplSupportedProtocols)
{
    if ((rttMonApplSupportedProtocols < 1) ||
	(rttMonApplSupportedProtocols >= MAX_RTT_PROTOCOLS)) {
	return (NULL);
    }
    if (rttMonApplSupportedProtocolsTablePopulated) {
	return (&rttMonApplSupportedProtocolsTable[rttMonApplSupportedProtocols]);
    } else {
	populateSupportedProtocols();
	rttMonApplSupportedProtocolsTablePopulated = TRUE;
	return (&rttMonApplSupportedProtocolsTable[rttMonApplSupportedProtocols]);
    }
} 

rttMonApplSupportedProtocolsEntry_t
*getNextRttMonApplSupportedProtocolsEntryPtr (long
					      rttMonApplSupportedProtocols)
{
    if (rttMonApplSupportedProtocols < 1) {
	rttMonApplSupportedProtocols = 1;
    }
    return (getRttMonApplSupportedProtocolsEntryPtr(rttMonApplSupportedProtocols));
} 

static void initRttMonApplEntry (unsigned long currentTime)
{
    rttMonApplEntry.rttMonApplVersion =
	preMallocOctetString(0, strlen(RTT_VERSION));
    populateOctetString(0,
			rttMonApplEntry.rttMonApplVersion,
			RTT_VERSION,
			strlen(RTT_VERSION));
    SET_VALID(I_rttMonApplVersion, rttMonApplEntry.valid);
    rttMonApplEntry.rttMonApplMaxPacketDataSize =
	MAX_rttMonApplMaxPacketDataSize;
    SET_VALID(I_rttMonApplMaxPacketDataSize, rttMonApplEntry.valid);
    rttMonApplEntry.rttMonApplTimeOfLastSet = currentTime;
    SET_VALID(I_rttMonApplTimeOfLastSet, rttMonApplEntry.valid);
    rttMonApplEntry.rttMonApplNumCtrlAdminEntry =
	SYS_MAX_rttMonApplNumCtrlAdminEntry;
    SET_VALID(I_rttMonApplNumCtrlAdminEntry, rttMonApplEntry.valid);
    rttMonApplEntry.rttMonApplReset = D_rttMonApplReset_ready;
    SET_VALID(I_rttMonApplReset, rttMonApplEntry.valid);
    rttMonApplEntry.rttMonApplPreConfigedReset =
	D_rttMonApplPreConfigedReset_ready;
    SET_VALID(I_rttMonApplPreConfigedReset, rttMonApplEntry.valid);

    return;
} 

static void initRttMonApplSupportedRttTypesTable (void)
{
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_echo].
	rttMonApplSupportedRttTypes =
	    D_rttMonApplSupportedRttTypes_echo;
    SET_VALID(I_rttMonApplSupportedRttTypes,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_echo].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_echo].
	rttMonApplSupportedRttTypesValid =
	    D_rttMonApplSupportedRttTypesValid_true;
    SET_VALID(I_rttMonApplSupportedRttTypesValid,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_echo].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_pathEcho].
	rttMonApplSupportedRttTypes =
	    D_rttMonApplSupportedRttTypes_pathEcho;
    SET_VALID(I_rttMonApplSupportedRttTypes,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_pathEcho].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_pathEcho].
	rttMonApplSupportedRttTypesValid =
	    D_rttMonApplSupportedRttTypesValid_true;
    SET_VALID(I_rttMonApplSupportedRttTypesValid,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_pathEcho].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_fileIO].
	rttMonApplSupportedRttTypes =
	    D_rttMonApplSupportedRttTypes_fileIO;
    SET_VALID(I_rttMonApplSupportedRttTypes,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_fileIO].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_fileIO].
	rttMonApplSupportedRttTypesValid =
	    D_rttMonApplSupportedRttTypesValid_false;
    SET_VALID(I_rttMonApplSupportedRttTypesValid,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_fileIO].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_script].
	rttMonApplSupportedRttTypes =
	    D_rttMonApplSupportedRttTypes_script;
    SET_VALID(I_rttMonApplSupportedRttTypes,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_script].valid);
    rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_script].
	rttMonApplSupportedRttTypesValid =
	    D_rttMonApplSupportedRttTypesValid_false;
    SET_VALID(I_rttMonApplSupportedRttTypesValid,
	      rttMonApplSupportedRttTypesTable[D_rttMonApplSupportedRttTypes_script].valid);

    return;
} 

static void initRttMonApplSupportedProtocolsTable (void)
{
    rttMonApplSupportedProtocolsTablePopulated = FALSE;

    /* fill in the defaults */
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_notApplicable].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_notApplicable;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_notApplicable].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_notApplicable].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_notApplicable].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipIcmpEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_ipIcmpEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipIcmpEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipIcmpEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_true;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipIcmpEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipUdpEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_ipUdpEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipUdpEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipUdpEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipUdpEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaRUEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_snaRUEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaRUEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaRUEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_true;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaRUEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU0EchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_snaLU0EchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU0EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU0EchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_true;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU0EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU2EchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_snaLU2EchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU2EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU2EchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_true;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU2EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62Echo].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_snaLU62Echo;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62Echo].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62Echo].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62Echo].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62EchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_snaLU62EchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62EchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_snaLU62EchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_appleTalkEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_appleTalkEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_appleTalkEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_decNetEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_decNetEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_decNetEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_ipxEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_ipxEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_ipxEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_isoClnsEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_isoClnsEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_isoClnsEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_vinesEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_vinesEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_vinesEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_xnsEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_xnsEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_xnsEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEcho].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_apolloEcho;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEcho].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEcho].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_apolloEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_apolloEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_netbiosEchoAppl].
	rttMonApplSupportedProtocols =
	    D_rttMonApplSupportedProtocols_netbiosEchoAppl;
    SET_VALID(I_rttMonApplSupportedProtocols,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_netbiosEchoAppl].valid);
    rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_netbiosEchoAppl].
	rttMonApplSupportedProtocolsValid =
	    D_rttMonApplSupportedProtocolsValid_false;
    SET_VALID(I_rttMonApplSupportedProtocolsValid,
	      rttMonApplSupportedProtocolsTable[D_rttMonApplSupportedProtocols_netbiosEchoAppl].valid);

    return;
} 

void initGlobalRttInfo (void)
{
    /* The following is created and never deleted */
    rttMonAccessSemaphore =
	create_watched_semaphore("rttMonAccessSemaphore",
				 0);

    initRttMonApplEntry(snmp_sysUpTime());
    initRttMonApplSupportedRttTypesTable();
    initRttMonApplSupportedProtocolsTable();
    baseRttMonCtrlAdminQHead.schedulerActive = FALSE;

    /* The following is created and never deleted */
    tmpOutBuff = malloc(TMPBUFF_SIZE);
    if (tmpOutBuff == NULL) {
	errmsg(&msgsym(GlobalInitFailed, RTT),
	       "initGlobalRttInfo");
    }
    return;
} 

void rttMonSetSelfID (long *IDofCaller,
		      rttMonCtrlAdminQItem
		      * inRttMonCtrlAdminQItem)
{
    process_lock_semaphore(rttMonAccessSemaphore, MAXULONG);
    {
	*IDofCaller =
	    inRttMonCtrlAdminQItem->
		ctrlAdminEntry->
		    rttMonCtrlAdminIndex;
    } 
    process_unlock_semaphore(rttMonAccessSemaphore);

    return;
}
