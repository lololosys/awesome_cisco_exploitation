/* $Id: sr_rmonmibuser.h,v 3.3 1995/11/17 18:39:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_rmonmibuser.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmonmibuser.h,v $
 * Revision 3.3  1995/11/17  18:39:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:03:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* This file contains definitions for the user part of the rmon mib data
 * structures */

#ifndef _rmonmibuser_h
#define	_rmonmibuser_h

#define	U_historyControlEntry	long TimerId; \
				void *NextBucketTimer; \
				struct _etherHistoryEntry_t *oldest; \
				struct _etherHistoryEntry_t *newest; \
				struct _etherHistoryEntry_t *current; \
				struct _etherHistoryEntry_t *freelist;

#define	U_etherHistoryEntry	struct _etherHistoryEntry_t *next; \
				struct _etherHistoryEntry_t *prev;

#define	U_eventEntry		long TimerId; \
				struct _logEntry_t *first; \
				struct _logEntry_t *last;

#define U_filterEntry		long TimerId;

#define U_etherStatsEntry	long TimerId;

#define	U_logEntry		struct _logEntry_t *next; \
				struct _logEntry_t *prev;

#define	U_alarmEntry		long TimerId; \
				void *alarmCallbackTimer; \
				long lastValue; \
				long lastDelta; \
				long flags;

#define	U_channelEntry		long TimerId; \
				long active;

#define	U_bufferControlEntry	long TimerId; \
				long active; \
				long bufferCount; \
				long currentIndex; \
				long errorSeen; \
				sys_timestamp startTime; \
				struct _captureBufferEntry_t *oldest; \
				struct _captureBufferEntry_t *newest; \
				struct _captureBufferEntry_t *freelist; \
				int bufferSize;

#define	U_captureBufferEntry	unsigned char *data; \
				struct _captureBufferEntry_t *next; \
				struct _captureBufferEntry_t *prev;

#define	U_hostControlEntry	long TimerId; \
				struct _rmonTableS *rtp; \
				struct _hostTimeEntry_t **htp; \
				unsigned long curseqno; \
				unsigned long tableid;


#ifdef SR_FTL
#define	U_hostEntry		unsigned long seqno; \
				unsigned long topNRate; \
				long topNTaken; \
				OctetString addrOS; \
				unsigned char addr[6];

#define	U_hostTimeEntry		unsigned long seqno; \
				unsigned long topNRate; \
				long topNTaken; \
				OctetString addrOS; \
				unsigned char addr[6];
#else	/* SR_FTL */
#define	U_hostEntry		unsigned long seqno; \
				unsigned long topNRate; \
				long topNTaken;

#define	U_hostTimeEntry		unsigned long seqno; \
				unsigned long topNRate; \
				long topNTaken;
#endif	/* SR_FTL */

#define	U_matrixControlEntry	long TimerId; \
				struct _rmonTableS *rtp; \
				struct _matrixDSEntry_t **dstp; \
				unsigned long curseqno;

#ifdef SR_FTL
#define	U_matrixSDEntry		unsigned long seqno; \
				OctetString srcAddrOS; \
				OctetString dstAddrOS; \
				unsigned char srcaddr[6]; \
				unsigned char dstaddr[6];

#define	U_matrixDSEntry		unsigned long seqno; \
				OctetString srcAddrOS; \
				OctetString dstAddrOS; \
				unsigned char srcaddr[6]; \
				unsigned char dstaddr[6];
#else	/* SR_FTL */
#define	U_matrixSDEntry		unsigned long seqno;

#define	U_matrixDSEntry		unsigned long seqno;
#endif	/* SR_FTL */

#define	U_hostTopNControlEntry	long TimerId; \
				struct _hostTopNEntry_t **tntp; \
				struct _hostEntry_t **htp; \
				long numhosts; \
				void *topNCallbackTimer; \
				long newReport; \
				unsigned long hostTableId; \
				unsigned long lastCheck;

#define	U_hostTopNEntry		long validData;

#endif	/* _rmonmibuser_h */

