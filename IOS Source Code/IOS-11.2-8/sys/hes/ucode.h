/* $Id: ucode.h,v 3.7.6.2 1996/07/08 22:25:49 mwu Exp $
 * $Source: /release/112/cvs/Xsys/hes/ucode.h,v $
 *------------------------------------------------------------------
 * ucode.h - downloadable microcode support
 *
 *  William H. Palmer, Mon Jun 29 20:05:19 PDT 1992
 *
 *  Copyright (c) 1992-1997 by cisco Systems, Inc.  
 *  All rights reserved. 
 *------------------------------------------------------------------
 * $Log: ucode.h,v $
 * Revision 3.7.6.2  1996/07/08  22:25:49  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.7.6.1  1996/03/18  19:44:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  06:52:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/02/20  00:53:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/02  23:00:26  dbath
 * CSCdi50584:  mis-specified micro file results in disabled board
 * Fix broken logic whereby even after we'd successfully loaded
 * the system bundle ucode as a backup, we'd fail to realise we'd
 * done it, and so wouldn't start the board.  While the bonnet is
 * up, tidy up some enums, and shuffle some code to enable other
 * modules to manipulate the ucode database in the future.
 *
 * Revision 3.6  1996/02/16  15:26:31  banderse
 * CSCdi47098:  CIP ucode sneaks into slot0:   NOT !!
 * Change wording on default flash images; add indication of file presence.
 *
 * Revision 3.5  1996/02/01  06:02:55  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.12.1  1996/01/24  21:48:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/07  13:51:26  banderse
 * CSCdi45351:  show microcode should display CIP flash default
 * if_cip.c - cleanup up printout.
 * ucode.c,
 * ucode.h  - handle IP flash default from mkbndl FLASH keyword.
 *
 * Revision 3.3  1995/11/29  18:45:34  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.2  1995/11/17  09:21:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:01:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/21  07:40:06  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.3  1995/07/31 21:28:52  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.2  1995/07/17  08:21:28  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  20:45:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */



#ifndef __UCODE_H__
#define __UCODE_H__

typedef enum UCODE_CTRLR_TYPE {	    /* order must match ctrlr_names[] in ucode.c */
    UCODE_EIP,
    UCODE_FIP,
    UCODE_FSIP,
    UCODE_HIP,
    UCODE_SIP,
    UCODE_SP,
    UCODE_SSP,
    UCODE_TRIP,
    UCODE_AIP,
    UCODE_MIP,
    UCODE_CIP,
    UCODE_FEIP,
    UCODE_RVIP,
    UCODE_SVIP,
    UCODE_RSP2,
    UCODE_POSIP,
    NUCODE_CTRLR_TYPES
} ucode_ctrlr_type_t;

typedef enum UCODE_SRC {
    UCODE_FLASH,
    UCODE_ROM,
    UCODE_SYSTEM,
    UCODE_NONE,			/* no valid microcode loaded */
} ucode_src_t;

/* struct indexed by dbus->controller_type */
struct ucodetype_ {
    ucode_ctrlr_type_t cardtype;  /* card type */
    ucode_src_t src;		  /* ucode source */
    char *filename;		  /* flash filename */
};

#define UCODE_BUFFER_SIZE (120*1024) 

#define UNIX_COMPRESS   1

/* struct for ucode bundle table of contents */
typedef struct {
    ulong  offset;              /* index into the ucode bundle */
    ulong  size;                /* size of ucode               */
    ushort fw_rev;              /* firmware revision           */
    ushort req_hw_ver;          /* required hardware version   */
    uchar  hw_type;             /* card type                   */
    uchar  desc[25];            /* description of ucode        */
    boolean compressed;         /* is the stuff compressed     */
    int    compress_type;       /* what kind of compress       */
} bundle_toc_t;

/* defines for ucode flash default table of contents */
#define MAX_LEN_FLASH_VERSION   14
#define MAX_LEN_FLASH_FILENAME  60

#define UCODE_FLASH_DEFAULT_DEVICE "slot0"

/* struct for ucode flash default table of contents */
typedef struct {
    u_char version[MAX_LEN_FLASH_VERSION];
    u_char filename[MAX_LEN_FLASH_FILENAME];
} flash_toc_t;

typedef struct {
    uint  slot;
    uint  card_type;
    ulong ucode_size;
    ulong chunks_done;
    ulong downloaded_bytes;
    uchar old_status;
    uchar old_control;
    ushort checksum;
    ulong download_time;
    uchar *filename;
    boolean wait;
} ucode_downloadtype;

/*------------------------------------------------------------------------- */
/*   IP microcode "multi-segment object" file changes 9/95                  */

#define IP_KERNEL_NAME "kernel_hw"
#define IP_KERNEL_NAME_SIZE sizeof(IP_KERNEL_NAME)

/*
 * Incoming IPC message structure for IP requests
 */
struct microcode_msg {
    int msg_version;
    int request_code;
    int data[1];
};

/*
 * Microcode message request codes - must keep in sync with IP's
 */
#define MICROCODE_PORT_NAME "Microcode Server"

#define MICROCODE_MSG_VERSION 1               /* protocol version ID */

#define MICROCODE_GET_PREFIX      1           /* IP requesting file prefix */
#define MICROCODE_GET_IOS_VERSION 2           /* IP requesting IOS version */

/*
 * Define to be the largest response sent to IP
 */
#define MICROCODE_BUFSIZE FLASH_NAME_LEN

/* ----------------------------------------------------------------------- */

extern const uchar bundle[];             /* ucode bundle                   */
extern const bundle_toc_t bundle_toc[];  /* ucode bundle table of contents */
extern const int bundled_files;          /* no# of files in bundle         */
extern const ulong max_ucode_uncompressed; /* uncompressed size of biggest */
					 /* compressed ucode file          */
extern const flash_toc_t flash_toc[];    /* flash default tbl of contents  */
extern const int flash_default_files;    /* # of files in flash default    */


/*
 * Prototypes 
 */

extern void dbus_load_ucode(int);
extern boolean dbus_load_ucode_nowaited(int);
extern void dbus_load_ucode_all_specific(boolean[]);
extern const char *get_ucode_src(int);
extern char *get_ucode_src_file(int);
extern boolean ucode_version_ck(uint ctrlr_type, uint major, uint minor);
extern inline boolean dbus_no_rom_ucode(uint);
extern boolean check_microcode_header(uchar**, ulong*, char*, int, boolean);
extern boolean rom_ucode_fallback(uint pslot);
extern void ucode_loader_wakeup(uint msec);
extern void set_ucode_source(ucode_src_t, ucode_ctrlr_type_t, boolean, char*);

#endif				/* __UCODE_H__ */
