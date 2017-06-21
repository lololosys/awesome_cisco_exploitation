/* $Id: at_show.h,v 3.3.4.1 1996/03/23 01:27:36 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_show.h,v $
 *------------------------------------------------------------------
 * at_show.h -- definitions for Appletalk SHOW commands
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_show.h,v $
 * Revision 3.3.4.1  1996/03/23  01:27:36  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3  1996/03/11  22:55:08  kleung
 * CSCdi50981:  Cannot run multiple Cayman tunnels.
 *
 * Revision 3.2  1995/11/17  08:43:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:09:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_SHOW_H__
#define __AT_SHOW_H__

/*
 * ParseCmdFlags
 * Flags indicating what arguments are permitted for command.
 */
typedef enum {
    kNoParse =		0x40000000, /* Do not parse arguments */
    kNoArgs=		0x20000000, /* No arguments are expected */
    kAnyNode=		0x00800000, /* A node number in the form of nn.hh */
    kATPort=		0x00400000, /* Currently the same as a interface */
    kATNet=		0x00200000, /* An AppleTalk network number */
    kATRange=		0x00100000, /* An AppleTalk cable range */
    kZone=		0x00010000, /* An AppleTalk zonename */
    kKeyTable5=		0x00001000,
    kKeyTable4=		0x00000800,
    kKeyTable3=		0x00000400,
    kKeyTable2=		0x00000200,
    kKeyTable1=		0x00000100,
    kAccList=		0x00000002,
    kSmInt=		0x00000001 /* A small, one byte, integer */
} ParseCmdFlags;

/*
 * ShowEvents
 * The module routine may receive these events for processing.
 */
typedef enum {
    kInitialize,		/* Always the first (and only the first) event */
    kFirstPageLine,		/* We are at the first line of the page */
    kLastPageLine,		/* We heave reached the last line of the page (sum line) */
    kNewPage,			/* We are starting a new page */
    kUserCancel,		/* The user cancelled the remainder of the show */
    kFailure,			/* A failure occurred */
    kFinished			/* Always the last event, Clean up and release any resources */
} ShowEvents;

typedef enum {
    kFieldInt,			/* The field value is an integer */
    kFieldBoolean,		/* The field value is a boolean */
    kFieldString,		/* The field value is a c-string */
    kFieldInterface,		/* The field value is an idbtype* */
    kFieldATZone,		/* The field value is an AT zone */
    kFieldATNode,		/* The field value is an AT node */
    kFieldATFQAddr		/* The field value is a fully qualified address (net,node,sock) */
} FieldTypes;

typedef enum {
    kFieldPadLeft	=0x08000000, /* Pad the field to the left */
    kFieldPadRight      =0x04000000, /* Pad the field to the right */
    kFieldCenter        =0x02000000, /* Center the field */
    kFieldUnsigned      =0x00800000, /* Output the field as an unsigned */
    kFieldHex           =0x00400000, /* Output field in hex */
    kFieldOnOff		=0x00080000, /* Use Off/On instead of True/False */
} FieldModifiers;

/*
 * fieldDescriptor
 * Describes the field to be added to the display.
 */
typedef struct fieldDescriptor_ {
    FieldTypes ftype;
    FieldModifiers fmodifiers;
    ushort titleLen, valueLen;
    ushort fieldLen;
} fieldDescriptor, *fieldDescrPtr;

/*
 * filterDataBlock
 */
typedef struct filterDataBlock_ {
    struct filterDataBlock_* next;
    ParseCmdFlags defined;	/* Cmd flags, set if data parsed and included in filter */
    ipaddrtype node;
    idbtype* port;		/* Currently unused */
    idbtype* portIF;		/* Specified interface */
    ushort atNet;		/* AppleTalk network */
    ushort atNode;		/* AppleTalk node */
    uchar atZone[ZONELEN+1];	/* AppleTalk zone name in pascal string format */
    long value;			/* Integer value */
    long from,to;		/* Bounded value */
} filterDataBlock, *filterDBPtr;


/*
 * showDataBlock
 * The showDataBlock is a structure of data created by the show command
 * handler to the command executor.  It contains global information necessary
 * to handle display updating and object filtering.
 */
typedef struct showDataBlock_ {
    ushort cmdNum;		/* Show command ID number */
    ushort cmdErr;		/* An error ID for problems */

    int indexBytes;		/* Amount to indent this line by */

    ushort pagLen;		/* Length of a screen page */
    ushort pagWidth;		/* Number of columns in a page */

    ushort currLine;		/* Number of lines that follow */
    ushort currPos;		/* Current column */
    ushort totalLines;		/* Number of lines output so far */

    filterDBPtr showFilterData;	/* List of filters if any */

    ulong ldata;		/* Generic data value */
    void *data;			/* Generic data block */
} showDataBlock, *showDBPtr;

/*
 * showProc
 * The routine called to handle the individual items of the show
 * command.  
 */
typedef boolean (*showProc)(showDBPtr);

/*
 * showEventProc
 * The routine called to show the individual show command.
 */
typedef boolean (*showEventProc)(showDBPtr,ShowEvents);

/*
 * How many zone pointers we allow to sit on the stack.
 */
#define AT_PRINT_ROUTE_ON_STACK_ZONES (5)

/*
 * AppleTalk private prototypes
 */
void            show_appletalk_static(void);
void 		atshow_PutField(showDBPtr,fieldDescrPtr,char*,void*);
void 		show_atcache(void);
void 		show_appletalk_traffic(void);
void 		show_appletalk_globals(void);
void 		show_appletalk_map(void);
void 		display_appletalk(idbtype*);
void 		atalk_print_route(atalk_rdb_t*);
void 		atalkif_print_routes(idbtype*);
void 		show_appletalk_neighbors(ushort,uchar);
void 		show_appletalk_zones(void);
void 		show_appletalk_zonedetail(uchar*,int);
void 		atalk_show_int_brief(idbtype *);
void 		show_appletalk_route(ushort,int);
void 		atalk_show_pvt_remap(void);
void		at_print_key_address(char *, treeKey);
void		at_print_key_idb_address(char *, treeKey);
void		at_print_key_long_idb_address(char *, treeKey);
void		at_print_key_interval(char *, treeKey);

#endif __AT_SHOW_H__
