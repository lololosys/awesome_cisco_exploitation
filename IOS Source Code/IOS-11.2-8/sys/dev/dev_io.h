/* $Id: dev_io.h,v 3.2.60.2 1996/09/10 03:22:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/dev_io.h,v $
 *------------------------------------------------------------------
 * dev_io.h - Generic device i-o control routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: dev_io.h,v $
 * Revision 3.2.60.2  1996/09/10  03:22:11  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/07/17  03:15:59  echeng
 * CSCdi54639:  Flashless 3000 does not boot
 * Branch: California_branch
 * Free up entry in both the two init_table; delete the useless flag
 * value DEV_UNCRT_CLEAR
 *
 * Revision 3.2  1995/11/17  09:00:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:54:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/25  21:40:01  sagarwal
 * CSCdi41798:  Flash mib problems on the 4500
 * o Remove check for device subunit to prevent passing of corrupt data
 * o Init chipinfo array from beginning
 * o Make SNMP chip info static (and reupdate on partitioning or insertion
 *   of PCMCIA card) to make getnext retrievals faster
 *
 * Revision 2.3  1995/09/24  19:15:32  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.2  1995/06/26  20:29:33  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.1  1995/06/07  20:23:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DEV_IO_H__
#define __DEV_IO_H__

/* 
 * Data structure for passing generic device info to upper layers.
 */
typedef struct {
    int dev_part_cnt;	       /* Number of partitions */
    int	dev_rwcnt;	       /* Number of RW devices */
    int	dev_rocnt;	       /* Number of RO devices */
    int dev_first_rw;	       /* First R/W partition */
    int dev_subunit;	       /* Sub-device number, starting from 1 */
    char *dev_name;            /* device name */
    char *dev_alias;           /* alias for refering to device */
    char *dev_sub_alias;       /* alias for referring to sub-device */
    char *dev_controller;      /* controller name */
    sys_timestamp dev_init_time;  /* Time when the device was initialized */
    ulong dev_state;           /* device state - RDONLY, RW, etc */
    boolean dev_removable;     /* PCMCIA Flash Device? */
    ulong dev_addr;            /* device physical address */
    ulong dev_size;            /* device size */
    ulong dev_width;           /* device width */
    ulong dev_banksize;        /* size of one bank of device */
    int dev_vpp_status;
    ulong dev_flags;
} devstat_t;

/*
 * Data structure for passing chip related information for a device
 * to higher layers.
 */
typedef struct {
    ushort    dev_chips;                /* total number of chips */
    ushort    dev_start_chip;           /* Starting physical chip number */
    ushort    dev_chip_wtries;          /* write retries */
    ushort    dev_chip_etries;          /* erase retries */
    char      *dev_chip_name[MAX_CHIPS]; /* names of device chips */
    ushort    dev_rom_id[MAX_CHIPS];     /* entry for each possible rom */
    ulong     dev_chip_wp[MAX_CHIPS];    /* number of write retries */
    ulong     dev_chip_ep[MAX_CHIPS];    /* number of erase retries */

    /* array that stores the chip boundaries of flash partitions */
    int             dev_partn_chip_bounds[FLASH_MAX_PART_CNT+1];
} dev_chipstat_t;

/* 
 * Master data structure for a device. Used only by code
 * that deals with generic device i-o control (dev_io.c).
 */
#define DEV_INFO  struct dev_info
#define DEV_CONS_INFO struct dev_cons_info
typedef DEV_INFO {
    int   dev_unit;            /* Sequence number, starting from 0 */
    int   dev_subunit;         /* Sub-device number, starting from 1 */
    int   dev_type;            /* Type - embedded Flash, Flash card, .. */
    char  *dev_name;           /* Any nice name for easy identification */
    char  *dev_alias;          /* Alias for device */
    char  dev_sub_alias[FLASH_DEV_NAME_LEN]; /* Alias for sub-device */
    ulong dev_state;           /* Initialized, RW, RDONLY, etc */
    ulong dev_ocnt;            /* Open count */
    ulong dev_flags;           /* Flags for EXCL, NODELAY, READ, WRITE, etc */
    ulong dev_addr;            /* Physical start address of device */
    ulong dev_size;            /* Device total size in bytes */
    ulong dev_end;             /* End address of device */
    ulong dev_start_bank;      /* Start bank number for device */
    ulong dev_width;           /* Width in terms of bytes */
    ulong dev_wmask;           /* Width mask */
    ulong dev_banksize;        /* Size of a bank */
    int   dev_cache_set;       /* Whether cache commands used */
    ulong dev_cache_sts;       /* Last cache status */
    void  *dev_geom;           /* Detailed device specific info */

    /*
     * File system related info.
     */
    void *fs_gen_info;    /* Pointer to fs generic info */
    void *fs_spec_info;   /* Pointer to fs specific info */
    
    /*
     * Pointer to consolidated device information.
     */
    DEV_CONS_INFO *devcons;

    /*
     * Pointers to structs above and below this one in the list.
     */
    DEV_INFO *next_partition;
    DEV_INFO *prev_partition;
    int   dev_sock;            /* PCMCIA Socket if applicable.*/

} dev_info_t;

/*
 * Master data structure that serves as a central place to hold
 * consolidated information about the device.
 */
typedef DEV_CONS_INFO {
    int   dev_type;            /* Type - embedded Flash, Flash card, .. */
    char  *dev_name;           /* Any nice name for easy identification */
    char  *dev_alias;          /* Alias for device */
    char  *dev_controller;     /* controller name, if any */
    sys_timestamp dev_init_time;  /* Time when the device was initialized */
    boolean dev_removable;     /* PCMCIA Flash Device? */
    boolean dev_wr_prot;       /* Defines if hardware write protect detected */
    ulong dev_cons_flags;      /* Flags that refer to the whole device */
    ulong dev_addr;            /* Physical start address of device */
    ulong dev_size;            /* Device total size in bytes */
    ulong dev_simm_size;       /* Size of a SIMM, where known. Default=0 */
    ulong dev_end;             /* End address of device */
    ulong dev_width;           /* Width in terms of bytes */
    ulong dev_wmask;           /* Width mask */
    ulong dev_banksize;        /* Size of a bank */
    int   dev_part_cnt;        /* Number of partitions on device */
    int	  dev_rwcnt;	       /* Number of RW devices */
    int	  dev_rocnt;	       /* Number of RO devices */
    int   dev_first_rw;	       /* First R/W partition */
    DEV_INFO *devinfo;       /* Pointer to the first devinfo structure */
    dev_chipstat_t *dev_chipinfo;  /* Pointer to device-level chip info struct   */
                                   /* updated on repartition or PCMCIA insertion */

    /*
     * Device specific functions
     */
    /* Device vpp fn */
    boolean (*dev_vpp_fn)(boolean);   /* Vpp ON or OFF function */
    /* Device vpp check fn */
    boolean (*dev_vpp_status_fn)(void);
    /* Device cache ON-OFF fn */
    boolean (*dev_cache_fn)(DEV_INFO *, int); /* Cache ON or OFF function */
    /* Device sizing fn */
    void    (*dev_size_fn)(DEV_INFO *);  /* Device sizing function */
    /* Device read fn */
    ulong (*dev_read_fn)(DEV_INFO *, uchar *, uchar *, ulong);
    /* Device write fn */
    ulong (*dev_write_fn)(DEV_INFO *, uchar *, uchar *, ulong);
    /* Device erase fn */
    ulong (*dev_erase_fn)(DEV_INFO *, uchar *, ulong);
    /* Device open fn */
    void  (*dev_open_fn)(DEV_INFO *, ulong);
    /* Device close fn */
    void  (*dev_close_fn)(DEV_INFO *, ulong);
    /* Device info display fn */
    int   (*dev_info_fn)(DEV_INFO *, int);
    /* Device access check fn */
    void  (*dev_access_fn)(DEV_INFO *);
    /* device short delay function */
    void  (*dev_timer_fn)(ushort *, ushort *, boolean);
    /* device chip identification fn */
    boolean  (*dev_data_bus_reversed)(void);
    /* long delay routine */
    void  (*dev_delay_fn)(ulong);
    /* function to relink dev_geom for all the partitions */
    void  (*dev_geom_relink_fn)(DEV_INFO *, int *, ushort);
    /* function to chk for removeable devices being present */
    int  (*dev_present_fn)(dev_info_t *);
    void (*dev_remove_fn)(dev_info_t *); /* mark device as empty */
    void (*dev_insert_fn)(dev_info_t *); /* re-fill device struct with data*/
    /* Wrapper functions for setting up bus error handlers.                */
    /* dev_read_fn will setup bus error hander, then call dev_read_wrap_fn */
    ulong (*dev_read_wrap_fn)(DEV_INFO *, uchar *, uchar *, ulong);
    ulong (*dev_write_wrap_fn)(DEV_INFO *, uchar *, uchar *, ulong);
    ulong (*dev_erase_wrap_fn)(DEV_INFO *, uchar *, ulong);
    int   (*dev_info_wrap_fn)(DEV_INFO *, int);
} dev_cons_info_t;

/*
 * Device table for specifying list of devices. This table will be
 * platform specific.
 */
typedef struct {
    int   dev_unit;         /* Sequence number, staring from 0 */
    int   dev_type;         /* Type - embedded Flash, Flash card, .. */
    char  *dev_name;        /* Any nice name for easy identification */
    char  *dev_alias;       /* Alias used while opening device */
    ulong dev_addr;         /* Physical start address of device */
    ulong size_cap;         /* Limit on device size */

    boolean (*dev_vpp_fn)(boolean);   /* Vpp ON or OFF function */
    boolean (*dev_vpp_status_fn)(void); /* Is Vpp ON or OFF */
    boolean (*dev_cache_fn)(DEV_INFO *, int); /* Cache ON or OFF function */
    void    (*dev_size_fn)(DEV_INFO *);  /* Device sizing function */
    void    (*dev_open_fn)(DEV_INFO *, ulong);
    void    (*dev_close_fn)(DEV_INFO *, ulong);
    void    (*dev_access_fn)(DEV_INFO *);   /* Check device access */
} dev_table_t;
    
/* 
 * Data structure for info about each opened device.
 */
typedef struct {
    dev_info_t  *dev;          /* Open device pointer */
    ulong dev_tty;             /* User's tty */
    ulong dev_flags;           /* Flags for EXCL, NODELAY, READ, WRITE, etc */
    ulong cur_ptr;             /* Current position in device */
    int   dev_subunit;         /* Device partition number */
} dev_open_instance_t;
    

/* Possible values for dev_state field */
enum DEV_STATE {
    DEV_STATE_NODEV      =  0,     /* Device not present        */
    DEV_STATE_INIT       =  1,     /* Device init routine OK    */
    DEV_STATE_RDONLY     =  2,     /* Device in READ-ONLY mode  */
    DEV_STATE_RFF        =  3,     /* Running from device; RD-ONLY */
    DEV_STATE_RW         =  4,     /* Device in READ-WRITE mode */
    DEV_STATE_REMOVED    =  5      /* was there but not now     */
};

/* Possible values for dev_flags field */
#define DEV_FLAGS_EXCL        1    /* Exclusive open access            */
#define DEV_FLAGS_VERBOSE     8    /* Be verbose                       */
#define DEV_FLAGS_PROMPT      0x10 /* Prompt if necessary; implies verbose */
#define DEV_FLAGS_FORCE       0x20 /* Force open even if read mode */
#define DEV_FLAGS_REOPEN      0x40 /* Reopen device */
#define DEV_FLAGS_REOPEN_ERASE 0x80 /* Reopen to purge device */
#define DEV_FLAGS_NOERASE     0x0100 /* erase not allowed */
#define DEV_FLAGS_RDONLY      0x1000
#define DEV_FLAGS_RW          0x2000
#define DEV_FLAGS_ERASE       0x4000
#define DEV_FLAGS_OPENMASK    0x7000

/*
 * Possible uncreate modes.
 */
#define DEV_UNCRT_TOTAL		0x1
#define DEV_UNCRT_PARTIAL	0x2

/*
 * Possible device relink modes.
 */
#define DEV_CLONE		0x1
#define DEV_RECREATE		0x2

#define DEV_FS_SPEC_INFO        0x1
#define DEV_FS_GEN_INFO         0x2
/*
 * Error codes for device routines.
 * NOTE: these codes index into *dev_errmsg[] in dev_io.c
 */
#define DEV_OK              0      /* OK Status                      */
#define DEVERR_TBLOVER     -1      /* maximum # of drivers exceeded  */
#define DEVERR_NOINIT      -2      /* init routine not present       */
#define DEVERR_INIT        -3      /* init routine not present       */
#define DEVERR_NODEV       -4      /* device not found in dev table  */
#define DEVERR_EXCL        -5      /* already exclusive open         */
#define DEVERR_BAD_IOCTL   -6      /* unsupported ioctl command      */
#define DEVERR_NOMEM       -7      /* no memory during init          */
#define DEVERR_INV_MODE    -8      /* invalid mode flag specified    */
#define DEVERR_RDONLY      -9      /* device not programmable        */
#define DEVERR_ALREADY_OPEN    -10 /* device is already open    */
#define DEVERR_ERASE           -11 /* flash erase op failed     */

#define DEV_CLOSED 0
#define DEV_OPEN   1

#define DEV_START  1
#define DEV_CURR   2
#define DEV_END    3

#define MAX_DEV_INIT_ENTRIES 8
#define MAX_DEV_OPEN_ENTRIES 32

#define DEVICE_LES_FLASH	0
#define DEVICE_HES_FLASH	1
#define DEVICES_MAX		20

#define DEV_PRESENT 	0x01
#define DEV_CHANGED	0x02

/*
 * Externs
 */

extern boolean global_dev_gate_closed;

/*
 * prototypes
 */
extern void dev_io_init(void);
extern boolean dev_chk(dev_info_t *);
extern dev_info_t *dev_create(const dev_table_t *);
extern dev_info_t *dev_info_struct(const char *devnm, int* partition);
extern void dev_clone(dev_info_t *, int *);
extern void dev_uncreate(dev_info_t *, ulong);
extern dev_cons_info_t *dev_move(char *);
extern void dev_unmove(void);
extern dev_info_t *dev_re_create(dev_cons_info_t *);
extern void dev_gather_config_info(char *, int *);
extern int dev_open(const char *, int);
extern int dev_iopen(int, int, int);
extern void dev_close(int);
extern ulong dev_read(int, uchar *, ulong);
extern ulong dev_write(int, uchar *, ulong);
extern ulong dev_erase(int, uchar *, ulong);
extern int dev_seek(int, int, int);
extern int dev_stat(const char *, devstat_t *);
extern int dev_chip_stat(const char *, dev_chipstat_t **);
extern int dev_fs_stat(const char *, fsstat_t *);
extern void dev_init (int);
extern int dev_info(const char *, int);
extern void dev_delay(int, ulong);
extern const char *device_alias(int);
extern int dev_subunit(int);
extern boolean dev_set_fs_spec_info(int, void *);
extern void *dev_get_fs_spec_info(int);
extern boolean dev_set_fs_gen_info(int, void *);
extern void *dev_get_fs_gen_info(int);
extern const char *dev_error_msg(int);
extern int dev_get_num_flash_part(void);
extern int dev_get_flash_devices(char **);
extern void dev_clear_init_table_slot (int indx);

#endif  /* __DEV_IO_H__ */
