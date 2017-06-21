/* $Id: dev_io.c,v 3.4.4.4 1996/09/10 03:22:09 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/dev_io.c,v $
 *------------------------------------------------------------------
 * dev_io.c - Generic device i-o control routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dev_io.c,v $
 * Revision 3.4.4.4  1996/09/10  03:22:09  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/08/07  08:57:54  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/07/17  03:15:57  echeng
 * CSCdi54639:  Flashless 3000 does not boot
 * Branch: California_branch
 * Free up entry in both the two init_table; delete the useless flag
 * value DEV_UNCRT_CLEAR
 *
 * Revision 3.4.4.1  1996/03/18  19:12:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/16  06:34:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.16.2  1996/03/07  08:42:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  13:45:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/09  22:54:36  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/12/07  22:28:10  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  08:59:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:53:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/25  21:40:04  sagarwal
 * CSCdi41798:  Flash mib problems on the 4500
 * o Remove check for device subunit to prevent passing of corrupt data
 * o Init chipinfo array from beginning
 * o Make SNMP chip info static (and reupdate on partitioning or insertion
 *   of PCMCIA card) to make getnext retrievals faster
 *
 * Revision 2.4  1995/09/24  19:15:30  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.3  1995/06/26  20:29:30  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.2  1995/06/18  19:40:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:23:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <stdlib.h>		/* for atoi() */
#include <ciscolib.h>
#include "ttysrv.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"

dev_info_t *dev_init_table[MAX_DEV_INIT_ENTRIES];
dev_info_t *dev_tmp_init_table[MAX_DEV_INIT_ENTRIES];
dev_open_instance_t *dev_open_table[MAX_DEV_OPEN_ENTRIES];
boolean global_dev_gate_closed = FALSE;
int tmp_part_bounds[FLASH_MAX_PART_CNT+2];

static void dev_insert(dev_info_t *dev);
static void dev_remove (dev_info_t *dev);

/*
 * Various error messages.
 * NOTE: these are indexed through error codes in dev_io.h
 */
static char *const dev_errmsg[] = {
    "device open successful",     /* DEV_OK                         */
    "table overflow",             /* DEV_TBLOVER                    */
    "device not initialized",     /* DEVERR_NOINIT                  */
    "initialization error",       /* DEVERR_INIT                    */
    "device not found",           /* DEVERR_NODEV                   */
    "device in exclusive use",    /* DEVERR_EXCL                    */
    "unknown IOCTL call",         /* DEVERR_IOCTL                   */
    "no memory",                  /* DEVERR_NOMEM                   */
    "invalid open mode",          /* DEVERR_INV_MODE                */
    "device is READ-ONLY",        /* DEVERR_RDONLY                  */
    "device in use",              /* DEVERR_ALREADY_OPEN            */
    "device erase error",         /* DEVERR_ERASE                   */
    "unknown",                    /* DEVERR_LAST                    */
};
#define DEVERR_LAST (sizeof(dev_errmsg) / sizeof(char *))

/*----------------------------------------------------------------- */
/*  dev_insert(dev)   - When a device (usually PCMCIA) is removed   */
/*  and then re-inserted, the removal caused a call to dev_remove(),*/
/*  which is followed by a call to dev_insert().  Since dev_remove  */
/*  set most fields to indicate a size of zero, the dev, devcons,   */
/*  and probably flash_geom, and fs_[gen/spec]_info still connected */
/*  to the dev, but just reflect an empty device.  This was done to */
/*  insure that the device still existed for SNMP support, as well  */
/*  as to deal with the case in which a process is accessing the    */
/*  dev (and its subordinate structures) during the removal/        */
/*  insertion.  dev_insert() will attempt to use any structures     */
/*  which are already there and just fill them with info if poss.   */
/*----------------------------------------------------------------- */
static void dev_insert (dev_info_t *dev)
{
    dev_cons_info_t *devcons;

    if ( (dev != NULL)  &&
         ((devcons = dev->devcons) != NULL) &&
	 (devcons->dev_insert_fn  != NULL) ) {
         (*devcons->dev_insert_fn)(dev);
    }
}

/* make the device appear to exist, but be removed.   */
/* in general, you can change structures that are already there,    */
/* and can add on structures to these, but you can't free structs   */
/* that exists already (cuz another thread may be accessing it and  */
/* wont appreciate it if you place their struct on the freelist.)   */

static void dev_remove (dev_info_t *dev)
{
    dev_cons_info_t *devcons;

    if ( (dev != NULL)  &&
         ( (devcons = dev->devcons) != NULL) &&
	 (devcons->dev_remove_fn  != NULL) ) {
         (*devcons->dev_remove_fn)(dev);
    }
}
  
boolean dev_chk (dev_info_t *dev)
{
    dev_cons_info_t *devcons;
    boolean state = FALSE;
    int     present,card_in;
    
    /* first assume device is not okay unless it has a dev_present_fn   */
    if (  (dev != NULL) && ((devcons = dev->devcons) != NULL) &&
	  (devcons->dev_present_fn != NULL) )
    {
        present=(*devcons->dev_present_fn)(dev);
        card_in = (present & DEV_PRESENT);
    
        if        (  card_in  && (dev->dev_state == DEV_STATE_REMOVED)  )
	{
            dev_insert(dev);
	    state = TRUE;
	} else if ( !card_in  && (dev->dev_state != DEV_STATE_REMOVED)  )
	{
            dev_remove(dev);
	    state = FALSE;
	} else {
        /*-------------------------------------------------------------*/
        /*  The card_inserted state is same as dev_state.              */
	/*  Now just check to see if pulled+pushed since last poll, or */
	/*  yanked and inserted since last pool.                       */
        /*-------------------------------------------------------------*/
    	
	    if (present & DEV_CHANGED)
	    {
		if ( card_in) {
                    dev_remove(dev);
                    dev_insert(dev);
	            state = TRUE;
	        } else {
	            state = FALSE;
		        
	        }
	    } else {
		state = TRUE;  /* nothing has changed, so do nothing */
	    }
	}
    }
    return(state);
}

void dev_clear_init_table_slot (int indx)
{
    if ((indx >= 0) && (indx < MAX_DEV_INIT_ENTRIES) )
        dev_init_table[indx]=NULL;
}

/*
 * Get the next unused slot number in dev_init_table
 */
static int dev_get_next_init_table_slot (void )
{
    int indx = 0;
    while ((dev_init_table[indx] != NULL) &&
            (indx < MAX_DEV_INIT_ENTRIES))
        indx++;
    if (indx == MAX_DEV_INIT_ENTRIES)
        return (-1); /* dev_init_table is full */
    else
        return (indx);
}
/*
 * Find out the entry number for a device in the device init table.
 * devnm is in the form <dev>:[<partition>]
 */
static int dev_get_index (const char *devnm_in, int *subunit)
{
    dev_info_t *dev;
    char *colonptr, *tmptr, *partptr, *devnm;
    ushort i;
    char dev_str[FLASH_DEV_NAME_LEN];

    *subunit = 0;  /* initialize to a default - unknown partition */
    if (devnm_in)
        strcpy(dev_str, devnm_in);
    else
        return (DEVERR_NODEV);

    /* Remove leading and trailing whitespace from input string */
    devnm = deblank(dev_str);
    if (devnm) {
        tmptr = devnm + strlen(devnm) - 1;  /* point to last char of string */
        /* skip any trailing white spaces */
        while ((tmptr >= devnm) && ((*tmptr == ' ') || (*tmptr == '\t')))
            *tmptr-- = 0;
    }

    if ((devnm == NULL) || strlen(devnm) == 0)  /* No device name */
        return (DEVERR_NODEV);

    /* Look for a colon in <dev>:<partition> */
    colonptr = strchr(devnm, FLASH_PATH_SEPARATOR);

    if (colonptr) {  /* <dev>:<partition> case */
        partptr = colonptr + 1;  /* point to <part> */
        /* skip alpha characters in <partition> */
        while ((*partptr != 0) && ((*partptr < '0') || (*partptr > '9')))
            partptr++;

        if (*partptr && t_number(partptr)) { /* numeric value found */
             *subunit = atoi(partptr);
             colonptr++;
             strcpy(colonptr, partptr); /* make it <dev>:<part#> */
        }
        else
            *colonptr = 0; /* strip off <part> and keep only <dev> */
    }

    /*
     * First match using sub-device alias that includes the
     * partition number. If that fails, try again using device
     * alias, ignoring the partition number.
     */
    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
        dev = dev_init_table[i];
        if (dev) {
            if (!strcasecmp(devnm, dev->dev_sub_alias)) {
                dev_chk(dev);     /* check to see if the device is present */
                *subunit = dev->dev_subunit;
                return (i);
            }
        }
    }
    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
        if ((dev = dev_init_table[i]) != NULL) {
            if (!strcasecmp(devnm, dev->dev_alias)) {
                dev_chk(dev);     /* check to see if the device is present */
                return (i);
            }
        }
    }
    /* Now try atleast a partial match.
     */
    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
        if ((dev = dev_init_table[i]) != NULL) {
            if (partialmatch(devnm, dev->dev_alias)) {
                dev_chk(dev);     /* check to see if the device is present */
                return (i);
            }
        }
    }

    return (DEVERR_NODEV);
}

/*
 * Validate device handle and return its open instance structure
 * if valid.
 */
static inline dev_open_instance_t  *dev_open_struct_nochk (int dh)
{
    dev_open_instance_t *doi;

    if ((dh < 0) || (dh >= MAX_DEV_OPEN_ENTRIES))  /* invalid device */
        return (NULL);

    doi = (dev_open_instance_t *)dev_open_table[dh];
    return (doi);
}


/*
 * Validate device handle and return its open instance structure
 * if valid.
 */
static dev_open_instance_t  *dev_open_struct (int dh)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;

    doi = dev_open_struct_nochk(dh);
    if (doi != NULL)
    {
        dev = doi->dev;
        if (dev != NULL)
           if (dev_chk(dev)  == FALSE)
               doi = NULL;
    }
    return (doi);
}


/*
 * Validate device name and return its info structure if valid.
 */
dev_info_t *dev_info_struct (const char *devnm, int *subunit)
{
    int dh;

    dh = dev_get_index(devnm, subunit);
    if ((dh < 0) || (dh >= MAX_DEV_INIT_ENTRIES))
        return (NULL);

    return dev_init_table[dh];
}

/*
 * This function gets used when a device is being dynamically
 * partitioned. While partitioning the device we want to be sure
 * that nobody else is using the device at that point. If the 
 * device is indeed in use, abort, but leave everything as it was.
 */
static boolean dev_lock_device (dev_info_t *dev)
{
    dev_info_t *workptr;
    dev_cons_info_t *devcons;
    int part_no, indx;

    /* First, check the flags in the consolidated device structure */
    devcons = dev->devcons;
    if (devcons->dev_cons_flags)  /* device in use */
        return (FALSE);
    else                          /* take exclusive control */
        devcons->dev_cons_flags |= DEV_FLAGS_EXCL;
   
    /* Now, go and check each of the partitions */
    workptr = dev;
    part_no = 0;
    while (workptr != NULL) {
        if (workptr->dev_ocnt || workptr->dev_flags) /* in use */
            break;
        else {      /* not in use */
            workptr->dev_ocnt++;
            workptr->dev_flags |= DEV_FLAGS_EXCL;
            part_no++;
        }
        workptr = workptr->next_partition;
        if (workptr == NULL)  /* checked all partitions */
            return (TRUE);
    }

    /* Some partition was in use. Undo the partial locking that was done */
    devcons->dev_cons_flags = 0; 
    workptr = dev;
    for (indx = 1; indx <= part_no; indx++) {
        workptr->dev_ocnt = 0;
        workptr->dev_flags = 0;
        workptr = workptr->next_partition;
    }
    return (FALSE);
}

/*
 * This function is used to undo the locking done by dev_lock_device
 */
static void dev_unlock_device (dev_info_t *dev)
{
    dev_info_t *workptr;
    dev_cons_info_t *devcons;

    devcons = dev->devcons;
    devcons->dev_cons_flags = 0;
    workptr = dev;
    while (workptr != NULL) {
        workptr->dev_ocnt = 0;
        workptr->dev_flags = 0;
        workptr = workptr->next_partition;
    }
    return;
}

    
/*
 * Some default functions.
 */
static ulong default_read_fn (dev_info_t *dev, uchar *buf,
                              uchar *addr, ulong len)
{
    return (0);
}
static ulong default_write_fn (dev_info_t *dev, uchar *buf,
                               uchar *addr, ulong len)
{
    return (0);
}
static ulong default_erase_fn (dev_info_t *dev, uchar *addr, ulong len)
{
    return (0);
}
static int default_info_fn (dev_info_t *dev, int flag)
{
    return (0);
}
static int default_present_fn (dev_info_t *dev)
{
    return (DEV_PRESENT );
}
static void default_remove_fn (dev_info_t *dev) {;}
static void default_insert_fn (dev_info_t *dev) {;}


/*************************** Global functions **********************/
/*
 * Convert device error code to error string.
 */
const char *dev_error_msg (int err)
{
    err = 0 - err;  /* make error code a positive number */
    if ((unsigned) err >= DEVERR_LAST)
         return (dev_errmsg[DEVERR_LAST - 1]);
    return (dev_errmsg[err]);
}

/*
 * Device initialization routine.
 */
void dev_io_init (void)
{
    /*
     * One time initialization.
     */
    int i;

    /*
     * Initialize tables for devices initialized and devices open.
     */
    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
        dev_init_table[i] = NULL;   /* table of initialized devices */
        dev_tmp_init_table[i] = NULL; /* our temporary table */
    }
    for (i = 0; i < MAX_DEV_OPEN_ENTRIES; i++)
        dev_open_table[i] = NULL;   /* table of opened devices */
}

/*
 * Create the data structure for a device.
 */
dev_info_t *dev_create (const dev_table_t *dev_tbl_ptr)
{
    /*
     * Per-device initialization.
     */
    dev_info_t  *dev;
    dev_cons_info_t  *devcons;
    dev_chipstat_t *chipinfo;
    int devno;

    /*
     * Malloc a structure for device info.
     * Initialize structure.
     */
    dev = malloc_named(sizeof(dev_info_t), "Device Info");
    if (dev  == NULL)
        return (NULL);
    devcons = malloc_named(sizeof(dev_cons_info_t), "Dev: Cons Info");
    if (devcons == NULL) {
        free(dev);
        return (NULL);
    }   
    chipinfo = malloc_named(sizeof(dev_chipstat_t), "Dev: Chip Info");
    if (chipinfo == NULL) {
      free(dev);
      free(devcons);
      return (NULL);
    }


    devno = dev_get_next_init_table_slot();
    dev_init_table[devno] = dev;

    /*
     * Copy over values defined in device table.
     * Start with the dev_info structure.
     */
    dev->dev_unit     = devno;                     /* sequence number */
    dev->dev_subunit  = 1;                         /* sub-device number */
    dev->dev_type     = dev_tbl_ptr->dev_type;     /* type of device */
    dev->dev_name     = dev_tbl_ptr->dev_name;     /* name */
    dev->dev_addr     = dev_tbl_ptr->dev_addr;     /* physical start addr */
    dev->dev_alias    = dev_tbl_ptr->dev_alias;    /* alias for device */
    sprintf(dev->dev_sub_alias, "%s:%d", dev->dev_alias, dev->dev_subunit);

    /*
     * Set other values.
     */
    dev->dev_state     = DEV_STATE_NODEV;
    dev->dev_ocnt      = 0;
    dev->dev_flags     = 0;
    dev->dev_size      = 0;
    dev->dev_end       = 0;
    dev->dev_width     = 0;
    dev->dev_wmask     = 0;
    dev->dev_cache_set = 0;
    dev->dev_cache_sts = 0;
    dev->fs_spec_info  = NULL;
    dev->fs_gen_info   = NULL;
    dev->dev_geom      = NULL;
    dev->next_partition = NULL;
    dev->prev_partition = NULL;
    dev->dev_sock       = 0;
    dev->devcons        = devcons;

    /*
     * Now, take care of the dev_cons_info_t structure.
     */
    devcons->dev_type     = dev_tbl_ptr->dev_type;     /* type of device */
    devcons->dev_name     = dev_tbl_ptr->dev_name;     /* name */
    devcons->dev_alias    = dev_tbl_ptr->dev_alias;  /* alias for device */
    devcons->dev_removable = FALSE;
    devcons->dev_cons_flags = 0;
    devcons->dev_addr     = dev_tbl_ptr->dev_addr;     /* physical start addr */
    devcons->dev_size      = 0;
    devcons->dev_simm_size = 0;
    devcons->dev_end       = 0;
    devcons->dev_width     = 0;
    devcons->dev_wmask     = 0;
    devcons->dev_part_cnt  = 0;    /* Number of partitions */
    devcons->dev_rwcnt	   = 0;
    devcons->dev_rocnt	   = 0;
    devcons->dev_first_rw  = 0;
    devcons->devinfo       = dev;
    devcons->dev_chipinfo  = chipinfo;
    
    /*
     * Initialize function pointers in dev_cons_info_t.
     */
    devcons->dev_vpp_fn   = dev_tbl_ptr->dev_vpp_fn;   /* Vpp handler */
    devcons->dev_vpp_status_fn = dev_tbl_ptr->dev_vpp_status_fn;
    devcons->dev_cache_fn = dev_tbl_ptr->dev_cache_fn; /* Cache handler */
    devcons->dev_size_fn  = dev_tbl_ptr->dev_size_fn;  /* Sizing function */
    devcons->dev_open_fn  = dev_tbl_ptr->dev_open_fn;  /* Open function */
    devcons->dev_close_fn = dev_tbl_ptr->dev_close_fn; /* Close function */
    devcons->dev_access_fn = dev_tbl_ptr->dev_access_fn; /* access control fn */
    devcons->dev_read_fn   = default_read_fn;
    devcons->dev_write_fn  = default_write_fn;
    devcons->dev_erase_fn  = default_erase_fn;
    devcons->dev_info_fn   = default_info_fn;
    devcons->dev_timer_fn  = NULL;
    devcons->dev_delay_fn  = NULL;
    devcons->dev_present_fn= default_present_fn;
    devcons->dev_remove_fn = default_remove_fn;
    devcons->dev_insert_fn = default_insert_fn;
    devcons->dev_data_bus_reversed = NULL;
    devcons->dev_wr_prot   = FALSE;
    return(dev);
}

/* 
 * Once the number of partitions on the device has been determined, this
 * function is called to set up the data structures needed to manage these
 * partitions. It sets up a dev_info_t structure for each partition and
 * links them all up in a doubly-linked list. All the partitions share
 * the device specific information through a single instance of the
 * dev_cons_info_t structure.
 */
void dev_clone (dev_info_t *dev, int *part_bounds)
{
    dev_info_t *workptr, *prevptr;
    dev_cons_info_t *devcons;
    int devno, part_no, part_cnt;

    devcons = dev->devcons;
    /* Reuse the existing dev_info_t for the first partition */
    dev->dev_start_bank = part_bounds[1];
    dev->dev_size = (part_bounds[2] - part_bounds[1]) * devcons->dev_banksize;
    dev->dev_end = dev->dev_addr + dev->dev_size;
    devcons->dev_cons_flags = 0;
    part_cnt = devcons->dev_part_cnt;
    prevptr = dev;
    
    /* All the other partitions will need freshly malloced structures */
    for (part_no = 2; part_no <= part_cnt; part_no++) {
        workptr = malloc(sizeof(dev_info_t));
        if (workptr == NULL)
            return;
        prevptr->next_partition = workptr;  /* Link up the prev partition */
        devno = dev_get_next_init_table_slot();
        dev_init_table[devno] = workptr;   /* Entry in dev_init_table */

        workptr->dev_unit = devno;     /* Position in dev_init_table */
        workptr->dev_subunit = part_no; /* Partition number */
        workptr->dev_type = dev->dev_type;     /* type of device */
        workptr->dev_name = dev->dev_name;     /* name */
        workptr->dev_addr = dev->dev_addr      /*Physical start address */
                           +(part_bounds[part_no] * devcons->dev_banksize); 
        workptr->dev_alias    = dev->dev_alias;    /* alias for device */
        sprintf(workptr->dev_sub_alias, "%s:%d", workptr->dev_alias, 
                                                 workptr->dev_subunit);

        /*
         * Set other values.
         */
        workptr->dev_state = dev->dev_state; /* geom_relink_fn may change this */
        workptr->dev_ocnt  = 0;
        workptr->dev_flags = 0;
	workptr->dev_start_bank = part_bounds[part_no];
        workptr->dev_size  = (part_bounds[part_no+1] - part_bounds[part_no])
                             * devcons->dev_banksize;;
        workptr->dev_end   = workptr->dev_addr + workptr->dev_size;
        workptr->dev_width     = dev->dev_width;
        workptr->dev_wmask     = dev->dev_wmask;
        workptr->dev_banksize  = dev->dev_banksize;
        workptr->dev_cache_set = dev->dev_cache_set;
        workptr->dev_cache_sts = dev->dev_cache_sts;
        workptr->fs_spec_info  = NULL;
        workptr->fs_gen_info   = NULL;
        workptr->next_partition = NULL;
        workptr->prev_partition = prevptr;
        workptr->devcons        = dev->devcons;
        workptr->dev_sock       = dev->dev_sock;

        prevptr = workptr;

    }

    /* Now we can relink the geometry structure */
    (*devcons->dev_geom_relink_fn) (dev, part_bounds, DEV_CLONE);

    /* Count the number of r/w partitions, find the first r/w partition.
     * At the start of the loop, prevptr is pointing to the last dev_info_t
     * structure.
     */
    devcons->dev_rwcnt = 0;
    devcons->dev_rocnt = 0;
    devcons->dev_first_rw = 0;
    for (part_no = 1; part_no <= part_cnt; part_no++) {
	if (prevptr->dev_state == DEV_STATE_RW) {
	    devcons->dev_rwcnt++;
	    devcons->dev_first_rw = prevptr->dev_subunit; 		
	}
	else
	    devcons->dev_rocnt++;
	prevptr = prevptr->prev_partition;
    }
    return;
}

/*
 * This function performs the task of undoing what dev_create() and 
 * dev_clone() do. It traverses the doubly linked list of dev_info_t
 * structures and systematically frees up all the memory tied up there.
 */ 
void dev_uncreate (dev_info_t *dev, ulong flags)
{
    dev_info_t *workptr, *nextptr;
    dev_cons_info_t *devcons;

    if (dev == NULL)  /* nothing to do */
        return;
    devcons = dev->devcons;
    if (devcons == NULL) /* Should not happen */
        return;

    /* First free up the consolidated device info structure */
    if (flags & DEV_UNCRT_TOTAL) {
        if (devcons) 
            free(devcons);
    }
    else
        dev->devcons->dev_cons_flags &= ~DEV_FLAGS_EXCL;
    
    /* First, get to the head of the doubly linked list */
    workptr = dev; 
    while (workptr->prev_partition != NULL)
        workptr = workptr->prev_partition;
    nextptr = workptr->next_partition;
    while (workptr != NULL) {
        /* Free up memory used for fs specific information */
        if (workptr->fs_spec_info)
            reg_invoke_file_system_mem_free(DEVICE_LES_FLASH, 
                            workptr->fs_spec_info, DEV_FS_SPEC_INFO);

        /* Free up memory used for fs generic information */
        if (workptr->fs_gen_info)
            reg_invoke_file_system_mem_free(DEVICE_LES_FLASH, 
                            workptr->fs_gen_info, DEV_FS_GEN_INFO);

        /* Free up entry in both the two init_table */
        if (dev_init_table[workptr->dev_unit] == workptr)
            dev_init_table[workptr->dev_unit] = NULL;
        if (dev_tmp_init_table[workptr->dev_unit] == workptr)
            dev_tmp_init_table[workptr->dev_unit] = NULL;

        free(workptr);
        /* Point to the next victim */
        workptr = nextptr;
        if (workptr != NULL) /* We are not done yet */
            nextptr = workptr->next_partition;
    }
}

/*
 * This function is used to move the data structures existing before
 * repartitioning is attempted into a temporary init_table. In case,
 * partitioning cannot be done, or something goes wrong, these data
 * structures are moved back. If the requuested repartitioning is 
 * possible, a pointer to the consolidated device info structure is
 * passed back.
 */
dev_cons_info_t *dev_move (char *devnm)
{
    int indx, subunit, i;
    dev_info_t *dev, *workptr;

    dev = dev_info_struct(devnm, &subunit);
    /* First lock the device */
    if (!dev_lock_device(dev))
        return (NULL); /* Device is in use */

    /* Make sure no other device is simultaneously being partitioned */
    if (dev_tmp_init_table[0] != NULL)
        return (NULL);

    /* Now, move all the data structures into temporary init_table */
    workptr = dev;
    /* remember the partition boundaries of the old structure.
     * this information is needed when we have to unmove the device */
    tmp_part_bounds[0] = workptr->devcons->dev_part_cnt;
    tmp_part_bounds[1] = 0;

    indx = 0;
    i = 1;
    while (workptr != NULL) {
	tmp_part_bounds[i+1] = (workptr->dev_size / 
				workptr->devcons->dev_banksize)
				+ tmp_part_bounds[i];
        dev_tmp_init_table[indx] = workptr;
        dev_init_table[workptr->dev_unit] = NULL; /* Clear original entry */
        workptr->dev_unit = indx;          /* Update dev_unit */
        workptr = workptr->next_partition;
        indx++;
	i++;
    }
    return (dev->devcons);
}
 
/*
 * If it is determined that the requested partitioning cannot be done, this
 * function gets called to move the original data structures back into
 * dev_init_table.
 */
void dev_unmove (void)
{
    dev_info_t *dev, *workptr;
    dev_cons_info_t *devcons;
    int part_no, devno, indx;

    dev = dev_tmp_init_table[0];
    devcons = dev->devcons; 
    devcons->devinfo = dev; /* Reset this to point back at the original */

    workptr = dev;
    part_no = 0;
    /* we need to re arrange the geom structure because it's changed by
     * dev_re_create
     */
    (*devcons->dev_geom_relink_fn)(workptr, tmp_part_bounds, DEV_CLONE);
    while (workptr != NULL) {
        devno = dev_get_next_init_table_slot();
        workptr->dev_unit = devno; /* Update dev_unit */
        dev_init_table[devno] = workptr;
        part_no++;
        workptr = workptr->next_partition;
    }
    devcons->dev_part_cnt = part_no;
  
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++)
        dev_tmp_init_table[indx] = NULL;
    dev_unlock_device(dev);
    return;
}

/*
 * This function mimics what dev_create does at system bring-up
 * except that we are using the same consolidated device info structure
 * that existed. All the device info is copied over from there into
 * the dev_info_t structure which is initialized to make the whole
 * device look like a single partition. This is done so that we can go
 * and sniff all the file systems on the device.
 */
dev_info_t *dev_re_create (dev_cons_info_t *devcons)
{
    dev_info_t  *dev, *dev_src;
    int devno;

    /*
     * Malloc a structure for device info.
     * Initialize structure.
     */
    dev = malloc(sizeof(dev_info_t));
    if (dev  == NULL)
        return (NULL);

    dev_src = dev_tmp_init_table[0];
    
    /* Copy over all fields from original structure for partition 1 */
    *dev = *dev_src;

    /* relink dev_geom structure from that for the original partition 1 */ 
    (*devcons->dev_geom_relink_fn)(dev, NULL, DEV_RECREATE);

    /* Now change fields as necessary to reflect the entire device */
    devno = dev_get_next_init_table_slot();
    dev_init_table[devno] = dev;
    dev->dev_unit     = devno;                    /* sequence number */
    dev->dev_addr     = devcons->dev_addr;        /* physical start addr */
    dev->dev_ocnt      = 0;
    dev->dev_flags     = 0;
    dev->dev_size      = devcons->dev_size;
    dev->dev_end       = devcons->dev_end;
    dev->fs_spec_info  = NULL;
    dev->fs_gen_info   = NULL;
    dev->next_partition = NULL;
    dev->prev_partition = NULL;
    dev->devcons        = devcons;
    /* Point the dev_info_t pointer in devcons to the new structure */
    devcons->devinfo = dev;
    /* Clear flags in devcons so that we can open device to sniff it */
    devcons->dev_cons_flags &= ~DEV_FLAGS_EXCL; 

    return (dev); 
}

/*
 * This function will traverse the doubly linked list of dev_info_t
 * structures and gather information about the number of partitions on
 * the device, and the size of each partition in Megabytes. This is used
 * during nv_generation in "write term" for instance.
 */
void dev_gather_config_info (char *devnm, int *part_sizes_in_MB)
{
    int subunit, part_no;
    dev_info_t *dev, *workptr;
  
    dev = dev_info_struct(devnm, &subunit);
    workptr = dev;
    part_no = 0;
    while (workptr != NULL) {
        part_no++;
        part_sizes_in_MB[part_no] = workptr->dev_size/(1024*1024);
        workptr = workptr->next_partition;
    }
    part_sizes_in_MB[0] = part_no; /* Number of partitions */
}

/*
 * Open device in mode specified by flags.
 * dev_open() opens a device by name.
 * dev_iopen() opens a device by an index (in case we don't know
 *             the name).
 */
int dev_open (const char *devnm, int flags)
{
    int   devno, subunit;

    devno = dev_get_index(devnm, &subunit);
    return (dev_iopen(devno, subunit, flags));
}

int dev_iopen (int devno, int subunit, int flags)
{
    dev_info_t *dev;
    dev_cons_info_t *devcons;
    dev_open_instance_t *doi;
    int   level, i;

    if ((devno < 0) || (devno >= MAX_DEV_INIT_ENTRIES))
        return (DEVERR_NODEV);

    if ((dev = dev_init_table[devno]) == 0)  /* invalid device number */
        return (DEVERR_NODEV);

    if((devcons = dev->devcons) == NULL)  /*No device specific info present */
        return (DEVERR_NODEV);

    if (dev->dev_size == 0) /* Device not physically present */
        return (DEVERR_NODEV);

    if (dev->dev_state <= DEV_STATE_INIT)  /* Device not fully initialized */
        return (DEVERR_NOINIT);

    switch (flags & DEV_FLAGS_OPENMASK) {
    case DEV_FLAGS_RDONLY:
        /*
         * Partition must not be already open in RW mode, ERASE mode,
         * or with EXCLUSIVE access in any mode. Also, the device 
         * itself must not be in the exclusive access mode.
         */
        if ((dev->dev_flags & (DEV_FLAGS_EXCL | DEV_FLAGS_RW)) ||
            (devcons->dev_cons_flags & DEV_FLAGS_EXCL)){
            if (flags & DEV_FLAGS_VERBOSE)
                printf("\n%%dev_open: %s already in use", dev->dev_name);
            return (DEVERR_EXCL);
        }
        /*
         * Partition already open in RD mode and we want EXCLUSIVE access
         * now. Say sorry.
         */
        if (dev->dev_ocnt && (flags & DEV_FLAGS_EXCL)) {
            if (flags & DEV_FLAGS_VERBOSE)
                printf("\n%%dev_open: %s already in use", dev->dev_name);
            return (DEVERR_EXCL);
        }
        break;
    case DEV_FLAGS_RW:
    case DEV_FLAGS_ERASE:
    case (DEV_FLAGS_RW | DEV_FLAGS_ERASE):
        /*
         * First see if device is in RW mode.
         */
        if ((dev->dev_state != DEV_STATE_RW) && (!(flags & DEV_FLAGS_FORCE))){
            if (flags & DEV_FLAGS_VERBOSE)
                printf("\n%%dev_open: %s not writable", dev->dev_name);
            return (DEVERR_RDONLY);
        }
        /*
         * Partition must not have been opened by anyone else. Also, the
         * the device itself must not be in an exclusive access mode.
         */
        if ((dev->dev_flags & DEV_FLAGS_OPENMASK) ||
            (devcons->dev_cons_flags & DEV_FLAGS_EXCL)) {
            if (flags & DEV_FLAGS_VERBOSE)
                printf("\n%%dev_open: %s already in use", dev->dev_name);
            return (DEVERR_EXCL);
        }
        if (global_dev_gate_closed) {
            if (flags & DEV_FLAGS_VERBOSE)
                printf("\n%%dev_open: %s access disallowed at this time",
                       dev->dev_name);
            return (DEVERR_EXCL);
        }
        if (devcons->dev_access_fn)
            (*devcons->dev_access_fn)(dev);
        break;
    default:   /* No mode or multiple modes */
        if (flags & DEV_FLAGS_VERBOSE)
            printf("\n%%dev_open: Invalid open mode");
        return (DEVERR_INV_MODE);
    }

    doi = malloc(sizeof(dev_open_instance_t));
    if (!doi) {
        global_dev_gate_closed = FALSE;
        return (DEVERR_NOMEM);
    }

    level = raise_interrupt_level(ALL_DISABLE); /* disable interrupts */
    for (i = 0; i < MAX_DEV_OPEN_ENTRIES; i++) {
        if (dev_open_table[i] == 0) {
            dev_open_table[i] = doi;
            dev->dev_flags |= flags;
            if (flags & DEV_FLAGS_RW)
                devcons->dev_cons_flags |= DEV_FLAGS_EXCL; 
            dev->dev_ocnt++;
            break;
        }
    }
    reset_interrupt_level(level);

    if (i >= MAX_DEV_OPEN_ENTRIES) {  /* device open table full */
        free(doi);
        global_dev_gate_closed = FALSE;
        return(DEVERR_TBLOVER);
    }

    doi->dev = dev;
    doi->dev_tty = stdio->ttynum;
    doi->dev_flags = flags;
    doi->cur_ptr = 0;
    doi->dev_subunit = subunit;

    if (devcons->dev_open_fn) /* Call the device specific open functiont */ 
       (*devcons->dev_open_fn)(dev, flags);

    return (i);  /* return device handle */
}

/*
 * External entrypoint to generic drivers' close routines.
 */
void dev_close (int dh)
{
    int   level, i;
    dev_open_instance_t *doi, *tmp_doi;
    dev_info_t  *dev;
    dev_cons_info_t * devcons;

    if ((doi = dev_open_struct(dh)) == NULL)
        return;

    dev = doi->dev;
    devcons = dev->devcons;

    level = raise_interrupt_level(ALL_DISABLE); /* disable interrupts */
    dev_open_table[dh] = 0;
    dev->dev_flags = 0;
    dev->dev_ocnt--;

    /*
     * If open count is not zero, someone else has the device open.
     * Go through the dev_open_table and see who else has device
     * open. Take their dev_flags and regenerate the device's open
     * mode flags.
     */
    if (dev->dev_ocnt) {  /* someone else has device open */
        for (i = 0; i < MAX_DEV_OPEN_ENTRIES; i++) {
            tmp_doi = dev_open_table[i];
            if (tmp_doi && (tmp_doi->dev == dev))
                dev->dev_flags |= tmp_doi->dev_flags;
        }
    }
    reset_interrupt_level(level); /* re-enable interrupts */

    if (doi->dev_flags & DEV_FLAGS_RW)
        devcons->dev_cons_flags &= ~DEV_FLAGS_EXCL;
    if (devcons->dev_close_fn)
        (*devcons->dev_close_fn)(dev, doi->dev_flags);

    global_dev_gate_closed = FALSE;

    free(doi);
}

/*
 * External entrypoint to generic drivers' read routines.
 */
ulong dev_read (int dh, uchar *buf, ulong len)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;
    dev_cons_info_t *devcons;

    if ((doi = dev_open_struct(dh)) == NULL)
        return (0);

    dev = doi->dev;
    devcons = dev->devcons;
    /*
     * Check if we are trying to read beyond the device. If so,
     * adjust the length.
     */
    if ((doi->cur_ptr + len) > dev->dev_size) {
        if (doi->cur_ptr > dev->dev_size) {
            doi->cur_ptr = dev->dev_size;
        }
        len = dev->dev_size - doi->cur_ptr;
    }
    if (len) {  /* Trying to read more than 0 bytes */
        len = (*devcons->dev_read_fn)(dev, buf, (uchar *)doi->cur_ptr, len);
        doi->cur_ptr += len;
    }
    return (len);
}

/*
 * External entrypoint to generic drivers' write routines.
 */
ulong dev_write (int dh, uchar *buf, ulong len)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;
    dev_cons_info_t *devcons;

    if ((doi = dev_open_struct(dh)) == NULL)
        return (0);

    if (!(doi->dev_flags & DEV_FLAGS_RW))
        return (0);

    dev = doi->dev;
    devcons = dev->devcons;
    
    /*
     * Check if we are trying to write beyond the device. If so,
     * adjust the length.
     */
    if ((doi->cur_ptr + len) > dev->dev_size) {
        if (doi->cur_ptr > dev->dev_size) {
            doi->cur_ptr = dev->dev_size;
        }
        len = dev->dev_size - doi->cur_ptr;
    }
    if (len) {  /* Trying to write more than 0 bytes */
        len = (*devcons->dev_write_fn)(dev, buf, (uchar *)doi->cur_ptr, len);
        doi->cur_ptr += len;
    }
    return (len);
}

/*
 * External entrypoint to generic drivers' erase routines.
 */
ulong dev_erase (int dh, uchar *addr, ulong len)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;
    dev_cons_info_t *devcons;

    if ((doi = dev_open_struct(dh)) == NULL)
        return (0);

    if (!(doi->dev_flags & DEV_FLAGS_RW))
        return (0);

    dev = doi->dev;
    devcons = dev->devcons;

    /*
     * Check if addr is equal to the current pointer. If not, seek
     * to the address, provided it's valid. If invalid, pitch it.
     */
    if ((ulong)addr != doi->cur_ptr) {
        if (((int)addr >= 0) && ((ulong)addr <= dev->dev_size))
            doi->cur_ptr = (ulong)addr;
    }
    /*
     * Check if we are trying to erase beyond the device. If so,
     * adjust the length.
     */
    if ((doi->cur_ptr + len) > dev->dev_size) {
        if (doi->cur_ptr > dev->dev_size) {
            doi->cur_ptr = dev->dev_size;
        }
        len = dev->dev_size - doi->cur_ptr;
    }
    if (len) {  /* Trying to erase more than 0 bytes */
        len = (*devcons->dev_erase_fn)(dev, (uchar *)doi->cur_ptr, len);
        doi->cur_ptr += len;
    }
    return (len);
}

/*
 * Seek to a position within a device. Offset may be relative
 * to the start, end or current position in the device. Offset
 * may be positive or negative.
 * In case of an error (invalid parameter), remain at current
 * position.
 * Return back latest position.
 */
int dev_seek (int dh, int pos, int offset)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;
    int new_off;

    if ((doi = dev_open_struct(dh)) == NULL)
        return (DEVERR_NODEV);

    dev = doi->dev;
    if (doi->cur_ptr > dev->dev_size) {
        doi->cur_ptr = dev->dev_size;
    }

    switch (pos) {
    case DEV_START:
        if (((ulong) offset <= dev->dev_size) && (offset >= 0)) {
            doi->cur_ptr = offset;
        }
        return (doi->cur_ptr);
    case DEV_CURR:
        new_off = doi->cur_ptr + offset;
        if (((ulong) new_off <= dev->dev_size) && (new_off >= 0))
            doi->cur_ptr = new_off;
        return (doi->cur_ptr);
    case DEV_END:
        new_off = dev->dev_size + offset;
        if (((ulong) new_off <= dev->dev_size) && (new_off >= 0))
            doi->cur_ptr = new_off;
        return (doi->cur_ptr);
    default:
        return (doi->cur_ptr);
    }
}
/*
 * Provide generic info about the file system on this device.
 */
int dev_fs_stat (const char *devnm, fsstat_t *ptr)
{
    int sts, subunit;
    dev_info_t *dev;

    if ((dev = dev_info_struct(devnm, &subunit)) == NULL)
        return (DEVERR_NODEV);

    /*
     * See if device being written to or erased. If so, disallow
     * access and return error code.
     *
    if (dev->dev_flags & (DEV_FLAGS_RW | DEV_FLAGS_EXCL | DEV_FLAGS_ERASE))
        return (DEVERR_EXCL);
     *
     */

    sts = reg_invoke_file_system_stat(DEVICE_LES_FLASH, dev->fs_spec_info, ptr);
    return (sts);
}


/*
 * Provide generic device info.
 */
int dev_stat (const char *devnm, devstat_t *devinfo)
{
    int subunit;
    dev_info_t *dev;
    dev_cons_info_t *devcons;

    if ((dev = dev_info_struct(devnm, &subunit)) == NULL)
        return (DEVERR_NODEV);
    devcons = dev->devcons;

    devinfo->dev_controller = devcons->dev_controller;
    if (subunit > 0) {
        devinfo->dev_part_cnt = devcons->dev_part_cnt;
	devinfo->dev_rwcnt = devcons->dev_rwcnt;
	devinfo->dev_rocnt = devcons->dev_rocnt;
	devinfo->dev_first_rw = devcons->dev_first_rw;
        devinfo->dev_init_time = devcons->dev_init_time;
        devinfo->dev_subunit = dev->dev_subunit;
        devinfo->dev_name = dev->dev_name;
        devinfo->dev_alias = dev->dev_alias;
        devinfo->dev_sub_alias = dev->dev_sub_alias;
        devinfo->dev_state = dev->dev_state;
	devinfo->dev_removable = devcons->dev_removable;
        devinfo->dev_addr = dev->dev_addr;
        devinfo->dev_size = dev->dev_size;
        devinfo->dev_width = dev->dev_width;
        devinfo->dev_banksize = devcons->dev_banksize;
	if (dev->devcons->dev_vpp_status_fn)
	    devinfo->dev_vpp_status = (*devcons->dev_vpp_status_fn)();
	else	
	    devinfo->dev_vpp_status = VPP_UNKNOWN;
	devinfo->dev_flags = dev->dev_flags;
        return (DEV_OK);
    }
    else {
        devinfo->dev_part_cnt = devcons->dev_part_cnt;
	devinfo->dev_rwcnt = devcons->dev_rwcnt;
	devinfo->dev_rocnt = devcons->dev_rocnt;
	devinfo->dev_first_rw = devcons->dev_first_rw;
        devinfo->dev_init_time = devcons->dev_init_time;
        devinfo->dev_subunit = 0;
        devinfo->dev_name  = devcons->dev_name;
        devinfo->dev_alias = devcons->dev_alias;
        devinfo->dev_state = dev->dev_state; /* state of first partition */
	devinfo->dev_removable = devcons->dev_removable;
        devinfo->dev_addr = devcons->dev_addr;
        devinfo->dev_size = devcons->dev_size;
        devinfo->dev_width = devcons->dev_width;
        devinfo->dev_banksize = devcons->dev_banksize;
	if (dev->devcons->dev_vpp_status_fn)
            devinfo->dev_vpp_status = (*devcons->dev_vpp_status_fn)();
        else
            devinfo->dev_vpp_status = VPP_UNKNOWN;
	devinfo->dev_flags = dev->dev_flags;
        return (DEV_OK);
    }
}

/*
 * Provide chip information for the device.
 */
int dev_chip_stat (const char *devnm, dev_chipstat_t **dev_chipinfo)
{
    int subunit;
    dev_info_t *dev;

    if ((dev = dev_info_struct(devnm, &subunit)) == NULL)
        return (DEVERR_NODEV);

    /*
     * Return the statically maintained chip information structure 
     */
    *dev_chipinfo = dev->devcons->dev_chipinfo;
    if ((!dev->devcons->dev_chipinfo) ||
         (dev->dev_state == DEV_STATE_REMOVED))
        return (DEVERR_NODEV);
    else
        return (DEV_OK);
}

/*
 * Display device info. This includes file system specific info
 * and device chip specific info. What is actually displayed
 * is controlled by various flags that are passed in.
 * If a valid subunit number is specified, then display info
 * about just that partition; If subunit is 0, then display info
 * about all the partitions.
 */
int dev_info (const char *devnm, int flag)
{
    int subunit;
    dev_info_t *dev, *workptr, *temp_ptr;
    dev_cons_info_t *devcons;

    if ((dev = dev_info_struct(devnm, &subunit)) == NULL)
        return (DEVERR_NODEV);
    if (subunit < 0)
        return (DEVERR_NODEV);
    if ((devcons = dev->devcons) == NULL)
        return (DEVERR_NODEV);

    workptr = temp_ptr = dev;

    /*
     * Display file system specific info, if callback fn present.
     * Then display device info, if callback fn present.
     */
    if (flag & SHOW_FILES_SUMMARY) 
	printf("\nPartition   Size    Used      Free      Bank-Size"
	       "  State\t  Copy Mode");
    if (subunit > 0) {
	/* we need to take care of the summary command separately */
	if (flag & SHOW_FILES_SUMMARY) {
	    reg_invoke_file_system_info(DEVICE_LES_FLASH, 
					workptr->fs_spec_info, flag);
	    /* disable the SHOW_FILES_SUMMARY flag after one time display */
	    flag &= ~SHOW_FILES_SUMMARY;
	}
        reg_invoke_file_system_info(DEVICE_LES_FLASH, 
                                    workptr->fs_spec_info, flag);
        if (devcons->dev_info_fn)
            (*devcons->dev_info_fn)(dev, flag);
    }
    else {
	/* If "show flash summary", we only want to display the summary info 
	 * once at the beginning. 
	 */
	if (flag & SHOW_FILES_SUMMARY) {
	    while (temp_ptr != NULL) {
		reg_invoke_file_system_info(DEVICE_LES_FLASH,
					    temp_ptr->fs_spec_info, flag);
		temp_ptr = temp_ptr->next_partition;
	    }
	    /* disable the SHOW_FILES_SUMMARY flag after one time display */
	    flag &= ~SHOW_FILES_SUMMARY;
	}
        while (workptr != NULL) {
            reg_invoke_file_system_info(DEVICE_LES_FLASH,
                                        workptr->fs_spec_info, flag);
            if (devcons->dev_info_fn)
                (*devcons->dev_info_fn)(workptr, flag);
            workptr = workptr->next_partition;
        }
    }
    
    return (DEV_OK);
}

/*
 * Initialize device. This actually means purge the file
 * system and erase device if necessary.
 */
void dev_init (int dh)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;

    if ((doi = dev_open_struct(dh)) == NULL)
        return;

    dev = doi->dev;
    reg_invoke_file_system_init(DEVICE_LES_FLASH, dh, dev->dev_flags);
}

/*
 * Long term delay routine for device.
 */
void dev_delay (int dh, ulong delay)
{
    dev_open_instance_t *doi;
    dev_info_t *dev;
    dev_cons_info_t *devcons;

    if ((doi = dev_open_struct(dh)) == NULL)
        return;

    dev = doi->dev;
    devcons = dev->devcons;
    if (devcons->dev_delay_fn)
        (*devcons->dev_delay_fn)(delay);
}

/*
 * set pointer to file system specific information structure.
 */
boolean dev_set_fs_spec_info (int dh, void *ffs)
{
    dev_open_instance_t *doi;

    if ((doi = dev_open_struct_nochk(dh)) == NULL)
        return (FALSE);

    doi->dev->fs_spec_info = ffs;
    return (TRUE);
}

/*
 * return pointer to file system specific information structure.
 */
void *dev_get_fs_spec_info (int dh)
{
    dev_open_instance_t *doi;

    if ((doi = dev_open_struct_nochk(dh)) == NULL)
        return (NULL);

    return (doi->dev->fs_spec_info);
}

/*
 * set pointer to file system generic callback functions
 */
boolean dev_set_fs_gen_info (int dh, void *ffs)
{
    dev_open_instance_t *doi;

    if ((doi = dev_open_struct_nochk(dh)) == NULL)
        return (FALSE);

    doi->dev->fs_gen_info = ffs;
    return (TRUE);
}
/*
 * return pointer to file system generic callback functions
 */
void *dev_get_fs_gen_info (int dh)
{
    dev_open_instance_t *doi;

    if ((doi = dev_open_struct_nochk(dh)) == NULL)
        return (NULL);

    return (doi->dev->fs_gen_info);
}

/*
 * Return name (or alias) for device.
 */
const char *device_alias (int type)
{
    int i;
    dev_info_t *dev;

    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
        dev = dev_init_table[i];
        if ((dev) && (dev->dev_type == type))
            return (dev->dev_alias);
    }
    return (NULL);
}

/*
 * Return device partition number.
 */
int dev_subunit (int dh)
{
    dev_open_instance_t *doi;

    if ((doi = dev_open_struct(dh)) == NULL)
        return (FALSE);

    return (doi->dev_subunit);
}

/* 
 * return number of flash partitions
 */
int dev_get_num_flash_part(void)
{
    devstat_t devinfo;

    if (dev_stat("flash",&devinfo) >= 0) {
	return(devinfo.dev_part_cnt);
    } else {
	return(1);
    }
}

/*
 * Build table of physical device names for SNMP agent
 */
int dev_get_flash_devices (char **devnm_database)
{
    int i, devs;
    dev_info_t *cur_dev;
    char *tmp_ptr;
   
    for (i = 0; i< MAX_DEV_INIT_ENTRIES; i++) 
	if (devnm_database[i]) {
	    free(devnm_database[i]);
	    devnm_database[i] = NULL; 
	}

    devs = 0;
    for (i = 0; i < MAX_DEV_INIT_ENTRIES; i++) {
	cur_dev = dev_init_table[i];
	if (cur_dev) {
	    ++devs;
            if (cur_dev->dev_subunit == 1) {
		tmp_ptr = malloc(FLASH_DEV_NAME_LEN);
		if (tmp_ptr && cur_dev->dev_alias)
		    strcpy(tmp_ptr, cur_dev->dev_alias);
		devnm_database[cur_dev->dev_unit + 1] = tmp_ptr; 
            }
	}
    }
    return devs;
}	
