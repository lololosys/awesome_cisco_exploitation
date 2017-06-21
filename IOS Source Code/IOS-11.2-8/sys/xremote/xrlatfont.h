/* $Id: xrlatfont.h,v 3.2 1995/11/17 19:27:17 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xrlatfont.h,v $
 *------------------------------------------------------------------
 * xrlatfont.h - Definitions for Xremote LAT font service
 *  
 * May 27, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xrlatfont.h,v $
 * Revision 3.2  1995/11/17  19:27:17  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:02:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XRLATFONT_H__
#define __XRLATFONT_H__

/*
 * Define the LAT font load protocol
 */

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 strlen;
    CARD16 reserved2;
} lat_font_list_req;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 filenum;
    CARD16 reserved2;
} lat_font_get_prop_req;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 filenum;
    CARD16 reserved2;
    CARD16 reserved3;
    CARD8 reserved4;
    CARD8 mustbe1;
} lat_font_get_glyph_req;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 filenum;
    CARD16 namelen;
} lat_font_list_rep;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
} lat_font_attr_rep;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 atom;
    CARD16 namelen;
    CARD16 value;
    CARD16 valuelen;
} lat_font_property_rep;

typedef struct {
    CARD8 opcode;
    CARD8 reserved1;
    CARD16 reqlen;
    CARD16 charnum;
} lat_font_glyph_rep;

enum LAT_FONT_PROTOCOL {
    LAT_FONT_VERSION = 0x01,
    LAT_FONT_READY = 0x02,
    LAT_FONT_LIST_OPCODE = 0x03,
    LAT_FONT_GET_PROPERTIES = 0x04,
    LAT_FONT_GET_GLYPHS = 0x05,
    LAT_FONT_DIRECTORY = 0x0a,
    LAT_FONT_ATTRIBUTES = 0x0b,
    LAT_FONT_PROPERTIES = 0x0c,
    LAT_FONT_GLYPHS = 0x0d
};

/*
 * Define our data structures
 */

/*
 * Define layout of font server context block
 */


enum font_state { FONT_NOTOPEN = 0, FONT_OPEN_WAIT, FONT_VERSION_WAIT,
		    FONT_READY_WAIT, FONT_READY, FONT_LIST,
		    FONT_GET_ATTRIBUTES, FONT_GET_PROPERTIES, FONT_GET_GLYPHS,
		    FONT_DISCONNECT };

typedef struct lat_font_context_ {
    struct lat_font_context_ *next; /* Link to next */
    enum font_state state;	/* Current state of state machine */
    sb_ptr sb;			/* Pointer to sesison block */
    struct buffer *buf;		/* Pointer to I/O buffer */
    xrcontext *xrctx;		/* Pointer to XRemote context, if any */
    int incarnation;		/* Incarnation number */
    int bytesread;		/* Total bytes read */
    int filesread;		/* Total files read */
    int protoerrors;		/* Protocol errors */
    int memerrors;		/* Buffer overflows */
    char *directory;		/* Pointer to font directory */
    char *dirfill;		/* Directory fill pointer */
    int dirfiles;		/* Number of files in directory */
    char *header;		/* Pointer to current file header */
    char *property;		/* Pointer to property info start */
    char *propstr;		/* Pointer to property string start */
    int propnum;		/* Number of next property */
    int propoff;		/* Current property string offset */
    char *glyphs;		/* Pointer to start of glyph ptrs */
    int min_glyph;		/* Minimum glyph number */
    int max_glyph;		/* Maximum glyph number */
    int glyphoff;		/* Current glyph offset */
    int charinfosize;		/* Size of charInfo records */
    byte service[SV_STR];	/* Service name */
    boolean newdata;		/* TRUE if new data on connection */
    boolean error;		/* TRUE if an error occured reading file */
} lat_font_context;

/*
 * Define directory list entry header
 */

typedef struct {
    ushort	len;		/* Length of string value to follow */
    ushort	num;		/* File number */
} lat_font_dir_hdr;

/*
 * Constants
 */

#define XRLAT_OUTBUFSIZE 64	/* Output buffer size - largest request
				 * we can send */
#define XRLAT_INBUFSIZE 512	/* Input buffer size - largest request
				 * we can receive */


/*
 * Function prototypes
 */

extern void xremote_lat_font_session(sb_ptr, int, boolean *);
extern void xremote_lat_font_get_char(sb_ptr, of_byte, int, int *);
extern void xremote_lat_font_put_char(sb_ptr, if_byte, int);
extern void xremote_lat_font_show_sessions(void);

#endif __XRLATFONT_H__
