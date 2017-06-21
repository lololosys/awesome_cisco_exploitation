/* $Id: at_api.h,v 3.2 1995/11/17 08:41:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_api.h,v $
 *------------------------------------------------------------------
 * at_api.h -- definitions for Appletalk API interface
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_api.h,v $
 * Revision 3.2  1995/11/17  08:41:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_API_H__
#define __AT_API_H__

typedef enum {
    apiSysInfo=1,
    apiTraffic=2,
    apiTrafficInfo=3,
    apiPortInfo=4
} apiFunctions;

typedef struct apiCommand_ {
    ushort func;
    ushort vers;
    ulong reqid;
    ulong  arg1;
    ulong  arg2;
} apiCommand;

typedef struct apiSysInfoReply_ {
    apiCommand cmd;
    ushort numberOfPorts;
    ushort majorVersion;
    ushort minorVersion;
    ushort trafficSize;
    str32 hostname;
    str32 version;
    boolean checksumming;	/* if ddp checksums desired */
    boolean strictRTMP;	/* if strict checks are used on RTMP */
    boolean testToolCompat;	/* if supporting test tool */
    boolean ignoreVerifyErrs;	/* if kludge for dumb admins is desired */
    boolean permitPartialZones; /* If enabled then propogate a zone if any net permits */
    boolean routeZones;	/* if enabled then routes need zones */
    ulong rtmpUpdateTime;	/* timers used for routing */
    ulong rtmpValidTime;
    ulong rtmpInvalidTime;
    ulong zipQueryInterval;
    boolean extendedInternet; /* if we have seen range > 1 OR >1 zone */
    boolean obsoleteRouterActive; /* if we have a phase 1 neighbor */
    boolean obsoleteRouterDetected; /* if we had a phase 1 neighbor */
} apiSysInfoReply;

typedef struct apiTrafficReply_ {
    apiCommand cmd;
    ulong version;
    ulong uptime;
    ulong data[ATALK_MAXTRAFFIC];
} apiTrafficReply;

typedef struct apiTrafficInfoReply_ {
    apiCommand cmd;
    ulong version;
    ulong uptime;
    ulong value;
    ushort trafficGroup;
    ushort trafficReserved;
    str32 name;
} apiTrafficInfoReply;

typedef struct apiPortInfoReply_ {
    apiCommand cmd;
    ulong uptime;
    str32 name;				/* Physical name of port */
    ushort hwtype;			/* Physical hardware type */
    str32 atalk_PrimaryZone;		/* Primary zone for port */
    boolean atalk_enabled;		/* TRUE if AppleTalk enabled */
    boolean atalk_operational; 		/* TRUE if operational */
    boolean atalk_fastokay;		/* TRUE if fast switching enabled */
    boolean atalk_discovery;		/* TRUE if discovery is desired */
    boolean atalk_ifinit;		/* TRUE if idb->reset() done */
    boolean atalk_compatibility;	/* TRUE if ATp1 RTMP should be used */
    boolean atalk_sendroutes;		/* TRUE if we want routes sent */
    boolean atalk_glean; 		/* TRUE if we want gleaning */
    ulong atalk_inputerrs;		/* Number of interface input errors */
    ulong atalk_outputerrs;		/* Number of interface output errors */
    int atalk_routecache; 		/* !0 if fast switching desired */
    int atalk_enctype;			/* encaps to use for AppleTalk */
    int atalk_accesslist;	        /* Access list id */
    short atalk_gzlfilter; 		/* Access list for GetZoneList filtering */
    short atalk_route_filter;		/* Access list RTMP in */
    short atalk_route_filter_in;	/* Access list RTMP out */
    int atalk_status;			/* code why we are not operational */
    ushort atalk_cablestart;		/* Start of cable range */
    ushort atalk_cableend;		/* End of cable range, 0 = phase 1 */
    ataddrtype atalk_address;		/* This nodes address */
    ataddrtype atalk_confaddr;		/* Address of config/verify host */
    ushort atalk_zonecount;	        /* count of zone for interface */
    uchar atalk_zone_multicast[6]; 	/* zone multicast address */
    int aarp_enctype;			/* encaps to use for Apple AARP */
    int aarp_tentative;			/* state of address assignment */
}

#endif __AT_API_H__
