/* $Id: sd.h,v 3.2.60.1 1996/05/06 16:07:24 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/sd.h,v $
 *------------------------------------------------------------------
 * sd.h - Session Directory header file
 *
 * February, 1995.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sd.h,v $
 * Revision 3.2.60.1  1996/05/06  16:07:24  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:30:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  08:29:00  dino
 * CSCdi37181:  Need to show active multicast sources - show ip mroute
 * active.
 *
 * Revision 2.2  1995/07/12  21:55:07  dino
 * CSCdi36995:  Timestamp cached sd advertisments.
 *
 * Revision 2.1  1995/06/07  21:04:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SD_H__
#define __SD_H__

/*
 * General constants.
 */
enum {
    SD_MEDIA_UNDEFINED = -1,
    SD_MEDIA_VIDEO,
    SD_MEDIA_WB,
    SD_MEDIA_MAX
};

/*
 * Sd header - as sd_listen knows it. No source available to put meaningful
 * fields in it.
 */
typedef struct sdhdrtype_ {
    ulong      unknown;
    ipaddrtype source;
    uchar      data[0];
} sdhdrtype;

#define SD_MAX_MEDIA 4         /* We cache a max of 4 medias */
#define SD_MAX_ATTR  8         /* We cache a max of 8 attributes per media */
#define SD_IN_IP4    "IN IP4 " /* IP version 4 address format follows */

/*
 * Sd cache entry.
 */
typedef struct sdtype_ {
    struct sdtype_ *next;   
    sys_timestamp  uptime;     /* How long entry has been cached */
    sys_timestamp  last_heard; /* Last time announcement has been heard */
    ipaddrtype     group;      /* Group address */
    ushort         ttl;        /* TTL for advertisement */
    ushort         num_groups; /* Number of consecutive groups */
    ulong          lifetime_start;
    ulong          lifetime_stop;
    ipaddrtype     source;     /* Source from sd header */
    uchar          *name;      /* Session name */
    uchar          *person;    /* Creator of session */
    uchar          *descrip;   /* Conference description */
    uchar          *url;       /* URL refereince with conference */
    uchar          *email;     /* Email address of creator */
    uchar          *phone;     /* Phone number of creator */
    struct {
	uchar      *media_string;  /* Media type, audio, video, whiteboard */
	ushort      media_type;
	ushort      port;	/* Transport port */
	uchar      *format[SD_MAX_ATTR]; /* Media specific format */
	ipaddrtype group;
	ushort     ttl;
    } media[SD_MAX_MEDIA];
} sdtype;

#define SD_BUGINF if (sd_debug) buginf

/*
 * Externs in sd.c
 */
extern char const * const sd_media_strings[];

extern void sd_show_command(parseinfo *);
extern void sd_clear_command(parseinfo *);
extern sdtype *sd_get_session(ipaddrtype);
extern boolean sd_get_media_port(ipaddrtype, ushort, ulong *);

#endif __SD_H__
