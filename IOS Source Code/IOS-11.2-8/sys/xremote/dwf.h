/* $Id: dwf.h,v 3.2 1995/11/17 19:26:29 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/dwf.h,v $
 *------------------------------------------------------------------
 * $Log: dwf.h,v $
 * Revision 3.2  1995/11/17  19:26:29  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:31:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Copyright 1988, 1989, 1990 Network Computing Devices, Inc.  All rights reserved. */

#ident "$NCDId: @(#)dwf.h,v 14.2 1990/09/27 15:33:05 dc Exp $"

/*
 * dwf header
 */

#define B8
#define B16

typedef struct {
    INT16 leftSideBearing B16,
          rightSideBearing B16,
          characterWidth B16,
          ascent B16,
          descent B16;
    CARD16 attributes B16;
} dwfxCharInfo;
/*#define sz_dwfxCharInfo 12*/
#define sz_dwfxCharInfo 11	/* attr seems to occupy a byte */

/* 
 * Same as an xCharInfo, excluding the attribute bytes.
 */
typedef struct {
    INT16 leftSideBearing B16,
          rightSideBearing B16,
          characterWidth B16,
          ascent B16,
          descent B16;
    CARD16 attributes B16;
} dwfxCharInfoNoAttr;
#define sz_dwfxCharInfoNoAttr 10

/* 
 * Same as an xCharInfo, only in 8-bit byte form (instead of 16-bit form).
 */
typedef struct {
    INT8 leftSideBearing B8,
          rightSideBearing B8,
          characterWidth B8,
          ascent B8,
          descent B8;
    CARD8 attributes B8;
} dwfbCharInfo;
#define sz_dwfbCharInfo 6

/* 
 * Same as an bCharInfo, excluding the attribute bytes.
 */
typedef struct {
    INT8 leftSideBearing B8,
          rightSideBearing B8,
          characterWidth B8,
          ascent B8,
          descent B8;
} dwfbCharInfoNoAttr;
#define sz_dwfbCharInfoNoAttr 5

typedef struct {
    union {
	CARD32	value;
	CARD32	offset;
    } propAtom;
    union {
	CARD32	value;
	CARD32	offset;
    } v;
    CARD16	indirectAtom;
    CARD16	indirectValue;
} PropInfoRec;

typedef struct {
    CARD32	FirstGlyphAddress;	/* abs seek address of first glyph */
    CARD32	BytesOfPointersAndGlyph;/* # bytes of glyph information */
} GlyphHeader;

/*
 * Holds the offset from the start of the glyph pointers to the
 * start of a glyph, if a 16 bit item is big enough.
 */
typedef struct {
    CARD16	offset;
} ShortGlyphPointer;
#define NULLSHORTGLYPHPOINTER 0xffff

/*
 * Holds the offset from the start of the glyph pointers to the
 * start of a glyph, as a 32 bit quantity.
 */
typedef struct {
    CARD32	offset;
} LongGlyphPointer;
#define NULLLONGGLYPHPOINTER 0xffffffff

/*
 * For two-dimensional representations, there is an indirect
 * table for getting to the glyph pointers themselves.
 * The entries in this table appear to be the byte offset
 * within the file for the glyph pointers for a column of glyphs,
 * and the length (in bytes) of that portion of the file.
 */
typedef struct {
    CARD32 colPtr;
    CARD32 length;
} ColInfo;

#define DWF_MAGIC 0x1df5e76
typedef struct {
    CARD32	magic;		/* offs: 0x00 */
    INT16	headerLength;	/* usually == sizeof(dwfInfo) */
    INT16	nameLength;	/* length of font name */
    INT16	propLength;	/* length of properties area */
    INT16	propStringsLength;	/* length of properties strings */
} dwfPrefix;

typedef union {
    CARD32	word;
    struct {
	/* byte 0 */
	unsigned int f00 : 1;
	unsigned int f01 : 1;
	unsigned int f02 : 1;
	unsigned int f03 : 1;
	unsigned int f04 : 1;
	unsigned int f05 : 1;
	unsigned int f06 : 1;
	unsigned int allExist : 1;
	/* byte 1 */
	unsigned int f10 : 1;
	unsigned int f11 : 1;
	unsigned int f12 : 1;
	unsigned int f13 : 1;
	unsigned int shortGlyphPointer : 1;	/* size of pointer to glyphs:1=16bit */
	unsigned int f15 : 1;
	unsigned int byteInfo : 1;	/* CharInfo item size:1=bytes */
	unsigned int f17 : 1;
	/* byte 2 */
	unsigned int f20 : 1;
	unsigned int f21 : 1;
	unsigned int f22 : 1;
	unsigned int f23 : 1;
	unsigned int f24 : 1;
	unsigned int f25 : 1;
	unsigned int f26 : 1;
	unsigned int f27 : 1;
	/* byte 3 */
	unsigned int f30 : 1;
	unsigned int f31 : 1;
	unsigned int f32 : 1;
	unsigned int f33 : 1;
	unsigned int f34 : 1;
	unsigned int f35 : 1;
	unsigned int f36 : 1;
	unsigned int f37 : 1;
    } b;
} dwfFlags;

typedef struct {
    INT16	len2;		/* == sizeof(dwfInfo) */
    INT16	blank1;
    INT16	constv1;	/* offs: 0x10 */
    INT16	constv6;
    INT16	asc1;
    INT16	asc2;
    INT16	blank2;
    INT16	blank3;
    CARD32	ascent;
    CARD32	descent;	/* offs: 0x20 */
    dwfxCharInfo	minBounds;
    dwfxCharInfo	maxBounds;
    INT16	unk5;
    INT16	charInfosHaveAttrs;
    CARD32	minrow;		/* offs: 0x40 */
    CARD32	mincol;
    CARD32	maxrow;
    CARD32	maxcol;
    dwfFlags	flags;		/* offs: 0x50 */
    CARD16	numProps;
    CARD16	unk9[5];
} dwfInfo;

/*
 * encodings of the property info records
 */
#define DWF_PROP_X_HEIGHT	0x38
#define DWF_PROP_QUAD_WIDTH	0x39
#define DWF_PROP_SET_WIDTH_NAME	0x3a
#define DWF_PROP_POINT_SIZE	0x3b
#define DWF_PROP_RESOLUTION	0x3c
#define DWF_PROP_COPYRIGHT	0x3d
#define DWF_PROP_FAMILY_NAME	0x40
#define DWF_PROP_FULL_NAME	0x41
#define DWF_PROP_CAP_HEIGHT	0x42
