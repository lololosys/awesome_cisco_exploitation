/* $Id: g_dev.h,v 3.1.60.2 1996/08/02 22:53:02 smackie Exp $
 * $Source: /release/112/cvs/Xsys/dev/g_dev.h,v $
 *------------------------------------------------------------------
 * pan_dev.h -- Generic device driver structs and defines
 *
 * 27-June-1991, Darren Kerr
 * 12-Mar 1992, Dit Morse (stolen)
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * In order that processes may dynamically determine if
 * a device exists (knowing only the device name, not any
 * global variable names), a device struct and OS entrypoints
 * are here defined.
 *
 * Support installable drivers if practical.
 *------------------------------------------------------------------
 * $Log: 
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef MORE_DEV_PROTECTION
#define DEV_NAME_MASK (((((('d')<<8)|'e')<<8)|'v')<<8)
#define DEV_FLASH_MCI (DEV_NAME_MASK | 0)
#define DEV_FLASH_ENV (DEV_NAME_MASK | 1)
#endif

#define DEV_NUL 0
#define DEV_ENV 1               /* ENV must be before MCI            */
#define DEV_MCI 2
#define DEV_MAX 2
#define DEV_CNT DEV_MAX+1

#define DEV_MAXNAMLEN      16
#define DEV_MAXDRIVERS      3   /* Maximum number of device drivers */
#define LEVEL_MAX           6   /* Maximum interrupt level for drivers */
/*
 * Function table to call routines through device table.
 */
typedef struct {
    int     (*df_init)(void);      /* Init device if present          */
    int     (*df_open)(int flags); 
                                   /* Chk access & return file handle */
    int     (*df_close)(flash_dev_t *);  /* Back to init state              */
    void    (*df_intr)(void);      /* Device interrupt handler        */
    int     (*df_ioctl)(flash_dev_t *,int, uchar *);   
                                   /* Special cmds to I/O device      */
    ulong   (*df_read)(flash_dev_t *, uchar *, uchar *, uint);
                                   /* Device's read entrypoint        */
    ulong   (*df_write)(flash_dev_t *, uchar *, uchar *, ulong);
                                   /* Device's write entrypoint       */
} devfun_t;

/*
 * Device structure.
 */ 

struct flash_dev_t_ {
    char      *dev_name;          /* String tag used in open   */
    ulong     dev_state;          /* NODEV, INIT, OPEN, ...         */
    int       dev_ocnt;           /* Open count                     */
    int       dev_flags;          /* EXCL, NDLY, ...                */
    ulong     dev_level;          /* interrupt level                */
    ulong     dev_tty;            /* current controlling user's tty */
    devfun_t *dev_fun;            /* dev driver entrypoints         */
};

/* Possible values for dev_state field */
#define DEV_STATE_NODEV    0    /* Device not present               */
#define DEV_STATE_INIT     1    /* Device init routine OK           */
#define DEV_STATE_OPEN     2    /* Device opened                    */
#define DEV_STATE_WOPEN    3    /* Device waiting to open           */

/* Possible values for dev_flags field */
#define DEV_FLAGS_EXCL     1    /* Exclusive open access            */
#define DEV_FLAGS_NDLY     2    /* Don't wait/sleep on open         */
#define DEV_FLAGS_FORCE    4    /* Force open, even if minor errors */
#define DEV_FLAGS_QUIET    8    /* Don't print error if open-fail   */

#define TEST_OPEN  (DEV_FLAGS_EXCL | DEV_FLAGS_NDLY | DEV_FLAGS_FORCE)
#define FLASH_OPEN (DEV_FLAGS_EXCL | DEV_FLAGS_NDLY)

/*
 * Error codes for device routines.
 * NOTE: these codes index into *dev_errmsg[] in dev.c
 */
#define DEV_OK              0      /* OK Status                      */
#define DEVERR_TBLOVER     -1      /* maximum # of drivers exceeded  */
#define DEVERR_NOINIT      -2      /* init routine not present       */
#define DEVERR_INIT        -3      /* init routine not present       */
#define DEVERR_BADNAME     -4      /* dev str null or too big        */
#define DEVERR_STILL_OPEN  -5      /* can't install: still opened    */
#define DEVERR_NODEV       -6      /* device not found in dev table  */
#define DEVERR_EXCL        -7      /* already exclusive open         */
#define DEVERR_OPEN_STS    -8      /* open returned unknown status   */
#define DEVERR_ALLOC       -9      /* bad alloc                      */
#define DEVERR_LEVEL      -10      /* interrupt level is invalid     */
#define DEVERR_CTLR       -11      /* controller error caused failure*/
#define DEVERR_BUSY       -12      /* busy doing something else      */
#define DEVERR_BAD_IOCTL  -13      /* unsupported ioctl command      */
/* 
 * added for NVRAM driver  -  7/25/91 hstraus
 */
#define DEVERR_ALREADY_OPEN    -20 /* device is already open    */
#define DEVERR_CLOSED          -21 /* device is closed          */
#define DEVERR_EOF             -22 /* device at end of data     */
#define DEVERR_ERASE           -23 /* flash erase op failed     */
#define DEVERR_INSUFF_MEMORY   -24 /* no shadow RAM available   */
#define DEVERR_INVALID_OP      -25 /* invalid IOCTL operation   */
#define DEVERR_NO_FLASH        -26 /* no flash memory present   */
#define DEVERR_NO_NVRAM        -27 /* no NVRAM available        */
#define DEVERR_SEEK_PAST_EOM   -28 /* seek past end of mem      */
#define DEVERR_WRITE_PAST_EOM  -29 /* write past end of mem     */
#define DEVERR_WRITE_FLASH     -30 /* error programming flash   */

#define DEV_CLOSED 0
#define DEV_OPEN   1

extern int dev_init(flash_dev_t *dp);
extern int dev_open(flash_dev_t *dp, int flags);
extern int dev_close(flash_dev_t *dp);
extern int dev_ioctl(flash_dev_t *dp, int cmd, void *buf);
extern ulong dev_read(flash_dev_t *dp, uchar *buf, uchar *addr, ulong len);
extern ulong dev_write(flash_dev_t *dp, uchar *buf, uchar *addr, ulong len);
