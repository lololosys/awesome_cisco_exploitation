/* $Id: region.h,v 3.2 1995/11/17 18:50:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/region.h,v $
 *------------------------------------------------------------------
 * region.h - Prototypes and defines for Region Manager
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: region.h,v $
 * Revision 3.2  1995/11/17  18:50:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:19:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  23:14:14  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  21:57:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __REGION_H__
#define __REGION_H__

/*
 * Defines
 */

typedef enum region_class_ {
    REGION_CLASS_ANY,
    REGION_CLASS_LOCAL,
    REGION_CLASS_IOMEM,
    REGION_CLASS_PCIMEM,
    REGION_CLASS_FAST,
    REGION_CLASS_MULTIBUS,
    REGION_CLASS_CBUS,
    REGION_CLASS_FLASH,
    REGION_CLASS_NVRAM,
    REGION_CLASS_FILESYSTEM,
    REGION_CLASS_CONFIG,
    REGION_CLASS_IMAGETEXT,
    REGION_CLASS_IMAGEDATA,
    REGION_CLASS_IMAGEBSS,
    REGION_CLASS_UNKNOWN
} region_class;

typedef enum region_status_ {
    REGION_STATUS_ANY,
    REGION_STATUS_PARENT,
    REGION_STATUS_CHILD,
    REGION_STATUS_ALIAS
} region_status;

typedef enum region_media_ {
    REGION_MEDIA_ANY,
    REGION_MEDIA_READWRITE,
    REGION_MEDIA_READONLY,
    REGION_MEDIA_WRITEONLY,
    REGION_MEDIA_UNKNOWN
} region_media;

/*
 * Region flags
 *
 * Miscellaneous flags
 */

#define REGION_FLAGS_IGNORE            0x00000001
#define REGION_FLAGS_INHERIT_CLASS     0x00000002
#define REGION_FLAGS_INHERIT_MEDIA     0x00000004
#define REGION_FLAGS_EXCEPTION_DUMP    0x00000008

#define REGION_FLAGS_DEFAULT           (REGION_FLAGS_INHERIT_MEDIA)


/*
 * Structures
 */

struct regiontype_ {
    list_element        element;
    list_element        mempool_element;
    struct regiontype_ *parent;
    struct regiontype_ *alias;
    char               *name;
    uchar              *start;
    uchar              *end;
    uint                size;
    uint                flags;
    region_class        class;
    region_status       status;
    region_media        media;
    void 	       *context;   /* IP socket for core dumping (for now)*/
    mempool            *mempool;   /* back pointer to mempool */
};


/*
 * Inlines
 */

static inline boolean region_is_parent (regiontype *region)
{
    return(region && (region->status == REGION_STATUS_PARENT));
}

static inline boolean region_is_child (regiontype *region)
{
    return(region && (region->status == REGION_STATUS_CHILD));
}

static inline boolean region_is_alias (regiontype *region)
{
    return(region && (region->status == REGION_STATUS_ALIAS));
}

/*
 * region_attribute_match
 *
 * Returns true if the attributes passed in match the current region
 */

static inline boolean region_attribute_match (regiontype   *region,
					      region_class  class,
					      region_status status,
					      region_media  media)
{
    return(!(region->flags & REGION_FLAGS_IGNORE)                        &&
	   ((class == REGION_CLASS_ANY) || (region->class == class))     &&
	   ((status == REGION_STATUS_ANY) || (region->status == status)) &&
	   ((media == REGION_MEDIA_ANY) || (region->media == media)));
}

/*
 * region_addr_match
 *
 * Returns true if the address passed in matches the current region
 */

static inline boolean region_addr_match (regiontype   *region,
					 void         *addr)
{
    return(!(region->flags & REGION_FLAGS_IGNORE) &&
	   ((uchar *)addr >= region->start)       &&
	   ((uchar *)addr <  region->end));
}


/*
 * Prototypes
 */

extern void         region_init(void);
extern regiontype  *region_create(regiontype *region, char *name,
				  void *start, uint size,
				  region_class class, uint flags);
extern boolean      region_add_alias(regiontype*region, regiontype *alias);
extern regiontype  *region_find_by_attributes(region_class class,
					      region_status status,
					      region_media media);
extern regiontype  *region_find_next_by_attributes(regiontype *region,
						   region_class class,
						   region_status status,
						   region_media media);
extern regiontype  *region_find_by_class(region_class class);
extern regiontype  *region_find_next_by_class(regiontype *region,
					      region_class class);
extern regiontype  *region_find_by_addr(void *address);
extern uint         region_get_size_by_attributes(region_class class,
						  region_status status,
						  region_media media);
extern uint         region_get_size_by_class(region_class class);
extern void         region_set_class(regiontype *region, region_class class);
extern region_class region_get_class(regiontype *region);
extern region_status region_get_status(regiontype *region);
extern void         region_set_media(regiontype *region, region_media media);
extern region_media region_get_media(regiontype *region);
extern boolean      region_exists(region_class class);
extern void         region_insert(regiontype *region);

#endif

