/* $Id: flash_command.c,v 3.11.4.11 1996/09/06 21:00:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/filesys/flash_command.c,v $
 *------------------------------------------------------------------
 * Flash command routines.
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_command.c,v $
 * Revision 3.11.4.11  1996/09/06  21:00:46  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.11.4.10  1996/08/30  02:10:37  rolsen
 * CSCdi47580:  micro reload prints from interrupt level (SYS-3-INTPRINT)
 * Branch: California_branch
 *              make check of interrupt level before doing printf
 *
 * Revision 3.11.4.9  1996/08/28  12:42:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.8  1996/06/18  15:02:17  widmer
 * CSCdi57010:  Copy flash to running-config doesnt work.
 * Branch: California_branch
 *
 * Revision 3.11.4.7  1996/05/22  18:08:26  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * opend -> opened
 *
 * Revision 3.11.4.6  1996/05/21  09:46:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.4.5  1996/05/07  00:39:30  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.11.4.4  1996/04/29  19:22:37  ssangiah
 * CSCdi55648:  The boot flash command does not work.
 *   o Get the "boot flash" command to work again on RSP and c7200.
 *   o Fix an erroneous log message.
 * Branch: California_branch
 *
 * Revision 3.11.4.3  1996/04/15  21:13:31  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/04/15  14:57:28  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.11.4.1  1996/03/18  19:35:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.6.3  1996/03/16  06:42:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.6.2  1996/03/07  09:35:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.6.1  1996/02/20  00:47:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  07:13:39  tboyle
 * CSCdi44459:  rsp BOOTLDR image is worthless, in some cases
 *
 * Revision 3.10  1996/02/22  14:32:56  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/02/01  06:02:08  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/29  07:27:29  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.7  1996/01/09  21:28:12  yanke
 * Update to next image in the list when checking boot image list.
 *
 * CSCdi44152:  Force Reload if attempts to del PCMCIA File,Running image
 * or NOT
 *
 * Revision 3.6  1995/12/17  18:26:33  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/11/29  18:44:35  banderse
 * CSCdi43438:  CIP needs default microcode flash image
 * Added release specific default flash image; changes to ensure flash file
 * system is initialized completely before IP download.
 *
 * Revision 3.4  1995/11/25  22:51:24  yanke
 * Add show device list. Fix a bug in copy command.
 *
 * CSCdi42314:  slave sync config command doesnt update slave config
 *
 * Revision 3.3  1995/11/24  22:48:57  yanke
 * Take keywords in 'copy file-id' family.
 *
 * CSCdi39749:  Tftp parameters in Copy tftp xx is inconsistent for device
 * or file
 *
 * Revision 3.2  1995/11/17  09:07:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/22  21:59:24  yanke
 * initial version.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "config_register.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "interface.h"
#include "../dev/monitor1.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"
#include "../os/nv.h"
#include "../os/boot.h"

#include "filesys.h"
#include "fslib_print.h"
#include "flash_command.h"
#include "../dev/flash_dvr_spec.h"
#include "flh_config.h"
#include "filesys_debug.h"

#include "config.h"
#include "parser.h"
#include "file.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"

#include "../ui/command.h"
#include "../snmp/config_history.h"

#if defined(C7000)
#include "rp_fc.h"
#endif

/*
 * Used for new command "copy file-id file-id" command.
 */
#define	COPY_FILE_RCONFIG_SCONFIG	1
#define	COPY_FILE_RCONFIG_ID		2
#define	COPY_FILE_SCONFIG_RCONFIG	3
#define	COPY_FILE_SCONFIG_ID		4
#define	COPY_FILE_ID_RCONFIG		5
#define	COPY_FILE_ID_SCONFIG		6
#define	COPY_FILE_ID_ID			7
#define	COPY_FILE_ID_TFTP		8
#define	COPY_FILE_ID_RCP		9
#define	COPY_FILE_ID_FLASH		10

#define TFTP_DEV_NAME	"tftp"
#define RCP_DEV_NAME	"rcp"
#define NV_DEV_NAME	"nvram"
#if defined(C7000)
#define FLASH_DEV_NAME	"flash"
#endif

/*
 * Used when writing to NVRAM. nv_private_fileid[] is given as an argument
 * to FS_open() who modifies it, thus NV_PRIVATE_FILE_ID can't be used directly.
 */
#define NV_FILE_ID        "nvram:"
#define NV_PRIVATE_FILE_ID "nvram:private"
#define NV_MATCH_PRIVATE_FILE_ID(ptr) (!strcmp(ptr, NV_PRIVATE_FILE_ID))
static char nv_private_fileid[] = NV_PRIVATE_FILE_ID;

/*
 *	Used for extending "copy flash/tftp/rcp/mop file-id" commands.
 */
#define	COPY_FLASH_FILE_ID	100
#define	COPY_TFTP_FILE_ID	101
#define	COPY_RCP_FILE_ID	102
#define	COPY_MOP_FILE_ID	103

#define	STARTUP_CONFIG	"startup-config"
#define	RUNNING_CONFIG	"running-config"

/*
 * Max length config file name
 */
#define	MAX_CONFIG_FILE_ID	128

typedef struct df_dev_rec_
{
    int ttynum;
    struct df_dev_rec_ *next;
    char df_dev[FSLIB_DEV_ID_MAX + 1];
} df_dev_rec_t;

static df_dev_rec_t *df_dev_q = NULL;

static char *get_config_file_id (void);
static char *fetch_config_file_id (void);
static char *current_config_to_buf(int *);
static char *config_file_to_buf(const char *, int *, addrtype *);
static int copy_file_to_file(const char *, const char *);
static int write_buffer_to_file(const char *, char *, int);
static int buf_to_current_config(char *, int);
static int buf_to_startup_config(char *, int, boolean);
static boolean file_protection_check(const char *);
static boolean device_protection_check(const char *);
static boolean check_file_exist(const char *);
static char *trunc_image_id_from_list(char *);
static boolean match_filename(const char *, const char *);
static char *read_file_to_buf(int, int *);
static void set_default_dev_id(const char *);
int get_whole_file_id(const char *, char *, int);
static int construct_whole_file_id(const char *, char *, int, const char *);
static boolean is_IP_multi_segment_file(const char *src_file_id, int sfd);

/*
 * Store boot loader file name.
 */
static char rxboot_file_id[MAX_CONFIG_FILE_ID+1];

boolean flash_config_print = FALSE;	/* for print debug messages */

extern const uchar monlib_bundle[];
extern int monlib_bundle_size;

static boolean is_flash_device (const char *file_id)
{
    uint dev_type;
    int retcode;

    retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
    return (retcode >= 0 && (dev_type & IOCTL_DEV_TYPE_FLASH));
}

static boolean is_nv_device (const char *file_id)
{
    uint dev_type;
    int retcode;

    retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
    return (retcode >= 0 && (dev_type & IOCTL_DEV_TYPE_NV));
}

static int get_file_size (int fd)
{
    stat_t status;
    int size = -1;

    if (FS_fstat(fd, &status) == FS_OK)
	size = status.st_size;

    if (filesys_debug)
	printf("\nFile %d size = %d", fd, size);
    return size;
}

static int get_file_type (const char *file_id)
{
    stat_t status;
    int value;
    int type = -1;

    if (is_flash_device(file_id) == TRUE) {
	value = FS_stat(file_id, &status);
	if (value >= 0)
	    type = status.type;
    }
    return type;
}

static int config_buffer_to_startup (char *bufp, uint len, boolean vocal)
{
    int retcode;

    retcode = buf_to_startup_config(bufp, len, vocal);
    if (retcode < 0)
	return 0;
    return len;
}

static const char *error_code2text (int code)
{
    static char error_buffer[FS_ERR_BUF_MAX + 1];

    FS_geterrortext(code, error_buffer, FS_ERR_BUF_MAX);
    return (const char *)error_buffer;
}

static const char *get_device_names(void)
{
    static char dev_name_buffer[FS_DEV_NAMES_MAX + 1];

    FS_getdevicenames(dev_name_buffer, FS_DEV_NAMES_MAX);
    return (const char *)dev_name_buffer;
}

/*
 * Return TRUE if router configuration is indicated
 * to a file on flash device, and the file doesn't actually exist.
 */
static boolean config_file_not_exist (void)
{
    char *file_id;
    uint dev_type;
    int retcode;
    stat_t status;

    file_id = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (file_id == NULL || *file_id == 0)
	return FALSE;
	
    retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
    if (retcode < 0 || (dev_type & (IOCTL_DEV_TYPE_NV | IOCTL_DEV_TYPE_FLASH)) == 0)
	return TRUE;

    if (dev_type & IOCTL_DEV_TYPE_NV)
	    return FALSE;

    retcode = FS_stat(file_id, &status);
    if (retcode == FS_OK)
	return FALSE;

    printf("\nConfig file %s not found, proceed? ", file_id);
    if (confirm(NULL) == TRUE)
	return FALSE;
    return TRUE;
}

/*
 * Return reason string if this device is busy.  Return NULL
 * otherwise, or if the device name is invalid.
 */
static const char * get_busy_string (const char *dev_id)
{
    int retcode;
    int busy;
    const char *busy_strings[FS_BUSY_MAX + 1] =
    {
	"device not busy",
	"device being squeezed"
    };
    
    retcode = FS_ioctl(dev_id, IOCTL_GET_BUSY_INFO, &busy);
    if ((retcode == FS_OK) && (busy != FS_BUSY_IDLE))
 	return (busy_strings[busy]);
    else
	return (NULL);
}

#define CONTROL_C  3
/*
 * "show file <file-id>" command.
 */
void show_file_command (parseinfo *csb)
{
    char *file_id;
    int fd;
    char *bufp;
    int bufsize;
    char *cp;
    int cnt;
    char ext_file_id[FS_FILE_ID_MAX + 1];
    int retcode;
    const char *msg;
    int first;

    file_id = GETOBJ(string, 1);
    retcode = get_whole_file_id(file_id, &ext_file_id[0], FS_FILE_ID_MAX);
    if (retcode != 0) {
	printf("\nInvalid file id %s", file_id);
	return;
    }

    /* 
     * Read the file and display it.
     */
    
    file_id = &ext_file_id[0];
    bufsize = 1024;
    bufp = malloc(bufsize);
    if (bufp == NULL) {
	printf("\nNo memory for buffer");
	return;
    }

    fd = FS_open(file_id, O_RDONLY | O_CRC_OFF, 0);
    if (fd < 0) {
	msg = error_code2text(fd);
	printf("\nFile %s can not be opened (%s)", file_id, msg);
	free(bufp);
	return;
    }

    first = 1;
    automore_enable(NULL);
    psion(stdio);
    for (;;) {
        char inputchar;
        boolean control_c_seen;
	cnt = FS_read(fd, bufp, bufsize);
	if (cnt < 0) {
	    msg = error_code2text(cnt);
	    printf("\nRead file %s error (%s)", file_id, msg);
			break;
	} else if (cnt == 0)
			break;

	if (first) {
	    /*
	     * Indicate if a distilled config file
	     */
	    if (distilled_config(bufp) == TRUE)
		printf("\nConfiguration is distilled version");
	}
	cp = bufp;
	while (cnt--) {
            printc(*cp++);  
            /* 
             *  soak up chars from the console so that interrupt
             *  sequence can set the flag used by psipending(),
             *  and quit if psipending() notices the interrupt sequence,
             *  or if the user has typed control/c.
             *  also, quickly toss the remaining output if user
             *  wants to quit via automore.
             */
            control_c_seen = FALSE;
            while ((inputchar=getc(stdio)) != -1) { 
                if (inputchar == CONTROL_C) 
                  control_c_seen = TRUE;
            }
            if (control_c_seen || psipending() || automore_quit()) {
                psioff(stdio);
                automore_disable();
                FS_close(fd);
                free(bufp);
                return;
            }
        }
    }
    psioff(stdio);
    automore_disable();
    FS_close(fd);
    free(bufp);
}

/*
 * concatenate dev_id and file_name into file_id.
 */
void build_whole_file_id (char *file_id, const char *dev_id, const char *file_name)
{
    char *ptr;
    file_sys_stat_t status;
    int retcode;
    boolean remotefile;

    /*
     * If the incoming file_name already has a VALID device name in it,
     * we don't use dev_id (CSCdi88334).  For things like "c:micrsoft.sux",
     * we still need to tack dev_id on the front (CSCdi78276).
     * Use dev_id regardless for remote copy (CSCdj09479).
     */
    ptr = strchr(file_name, ':');
    remotefile = dev_id && (!strcmp(dev_id, TFTP_DEV_NAME) ||
                 !strcmp(dev_id, RCP_DEV_NAME));
    if (ptr && !remotefile) {
	*ptr = '\0';
	retcode = FS_fsstat(file_name, &status);
	*ptr = ':';
	if (retcode != FS_ERROR_UNKNOWN_DEVICE) {
	    if (filesys_debug)
		printf("\nName already file id %s", file_name);
	    strcpy(file_id, file_name);
	    return;
	}
    }
    ptr = file_id;
    strcpy(ptr, dev_id);
    ptr += strlen(file_id);
    *ptr++ = ':';
    strcpy(ptr, file_name);
}

/*
 * Construct a full file id from the given key word.
 * file_id: buffer to hold the constructed file id
 *   must be size of FS_FILE_ID_MAX + 1.
 * file_name: return pointer to start of file name if not NULL.
 * prompt: string used to prompt the user.
 * dev_id: device string to attatch to file name
 *   to form complete file id.
 * default_name: default file name to use if user
 *   elect to (by hit return). NULL if user input is mandatory.
 */
static boolean convert_key_to_file_id (char *file_id, char **file_name,
	const char *prompt, const char *dev_id, const char *default_name)
{
    char file_id_buf[FS_FILE_ID_MAX + 1];
    boolean retcode;

    /*
     *	Build complete file id.
     */
    memset(file_id_buf, 0, FS_FILE_ID_MAX + 1);
    if (default_name)
	printf("\n%s [%s]: ", prompt, default_name);
    else
	printf("\n%s: ", prompt);

    retcode = rdtty(file_id_buf, FS_FILE_ID_MAX);
    if (retcode == FALSE) {
	printf("\n?Bad file name");
	return FALSE;
    }

    /*
     * Check if user input is complete,
     * use default value if possible.
     */
    if (null(file_id_buf)) {
	if (default_name) {
	    strcpy(file_id_buf, default_name);
	} else {
	    printf("\nFile name needed");
	    return FALSE;
	}
    } else {
	if (get_file_name_by_file_id(file_id_buf) == NULL) {
	    if (default_name) {
		strcat(file_id_buf, default_name);
	    } else {
		printf("\nFile name needed");
		return FALSE;
	    }
	}
    }

    if (filesys_debug)
	printf("\nFile name = %s", file_id_buf);

    memset(file_id, 0, FS_FILE_ID_MAX + 1);
    if (dev_id)
	build_whole_file_id(file_id, dev_id, file_id_buf);
    else
	build_whole_file_id(file_id, get_default_dev_id(), file_id_buf);
    if (file_name)
	*file_name = strchr(file_id, ':') + 1;
    return TRUE;
}

/*
 * "copy file-id file-id" command.
 *
 * Possible source
 * startup-config, running-config, nvram:,
 * flash:filename, slot0:filename, slot1:filename,
 * tftp:filename, rcp:filename, and mop:filename (to be done).
 *
 * Possible destination
 * startup-config, running-config, nvram:,
 * flash:filename, slot0:filename, slot1:filename,
 * tftp:filename, rcp:filename, and mop:filename (to be done).
 *
 * Wildcard to be supported.
 */
void copy_file_command (parseinfo *csb)
{
    char *src_file_id = NULL;
    char *dst_file_id = NULL;
    char *bufp = NULL;
    int size;
    int retcode;
    char *file_id;
    char ext_src_file_id[FS_FILE_ID_MAX + 1];
    char ext_dst_file_id[FS_FILE_ID_MAX + 1];
    const char *file_name;
    addrtype adrp;
    int source, dest;
    const char *dev_id;
    char *src_file_name;
    uint dev_type;
    char *dst_ptr;


    if (csb->which == COPY_FILE_RCONFIG_SCONFIG) {
	/*
	 * invoke write_mem() directly.
	 */
	write_memory(csb);
	return;
    }

    /*
     * If a device is specified, validate it before proceeding.
     * Determine if a device was specified by get_dev_id_by_file_id.
     * Determine if the device was valid by FS_ioctl.
     */

     if ((csb->which == COPY_FILE_ID_FLASH) ||
	 (csb->which == COPY_FILE_ID_TFTP)  ||
         (csb->which == COPY_FILE_ID_RCP))
     {
	 dst_ptr = GETOBJ(string,1);
	 dev_id = get_dev_id_by_file_id(dst_ptr);
	 if (dev_id != NULL) {
             retcode = FS_ioctl(dst_ptr, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
	     if (retcode < 0) {
	        printf("\nInvalid device id [%s]", dst_ptr);
	        return;
	     }
	 }
     }

    /*
     * Get whole source file id and dst file id
     * by prefix with default device if needed.
     */
    if (csb->which == COPY_FILE_ID_SCONFIG ||
	csb->which == COPY_FILE_ID_RCONFIG ||
	csb->which == COPY_FILE_ID_TFTP ||
	csb->which == COPY_FILE_ID_RCP ||
	csb->which == COPY_FILE_ID_FLASH ||
	csb->which == COPY_FILE_ID_ID) {
	src_file_id = GETOBJ(string, 1);
	file_id = &ext_src_file_id[0];
	retcode = get_whole_file_id(src_file_id, file_id, FS_FILE_ID_MAX);
	if (retcode != 0) {
	    printf("\nInvalid file id %s", src_file_id);
	    return;
	}
	if (get_file_name_by_file_id(src_file_id) == NULL) {
	    dev_id = get_dev_id_by_file_id(src_file_id);

	    /* Prompt the user for the File Name they didn't enter */
	    retcode = convert_key_to_file_id(&ext_src_file_id[0],
					     &src_file_name,
					     "Enter source file name",
					     dev_id, NULL);

	    if (retcode == FALSE) {
		printf("\nInvalid file id %s", src_file_id);
		return;
	    }

	   /*
	    * We now have a valid string. We need to copy this to file_id
	    * so it will be picked up by src_file_id below
	    */ 

	    file_id = &ext_src_file_id[0];
	}
	if (filesys_debug)
	    printf("\nSource file = (%s, %s)", src_file_id, file_id);
	src_file_id = file_id;
    }

    if (csb->which == COPY_FILE_SCONFIG_ID ||
	csb->which == COPY_FILE_RCONFIG_ID ||
	csb->which == COPY_FILE_ID_ID) {
	/*
	 * Check for invalid "copy run run/copy start start" command.
	 */
	dst_file_id = GETOBJ(string, 2);

	if ((csb->which == COPY_FILE_RCONFIG_ID &&
	    strncmp(dst_file_id, "running-config", strlen(dst_file_id)) == 0)
		||
	    (csb->which == COPY_FILE_SCONFIG_ID &&
	    strncmp(dst_file_id, "startup-config", strlen(dst_file_id)) == 0))
	{
	    printf("\nSource/Destination file may not be the same");
	    return;
	}

	/*
	 * Expand file id by default device if needed.
	 * And use src file name if file name is missing
	 * (except for nvram).
	 */
	file_id = &ext_dst_file_id[0];
	file_name = get_file_name_by_file_id(src_file_id);
	if (csb->which == COPY_FILE_ID_ID) {
	    retcode = construct_whole_file_id(dst_file_id, file_id,
		FS_FILE_ID_MAX, file_name);
	} else {
	    retcode = get_whole_file_id(dst_file_id, file_id,
				FS_FILE_ID_MAX);
	}
	if (retcode != 0) {
	    printf("\nInvalid file id %s", dst_file_id);
	    return;
	}
	if (filesys_debug)
	    printf("\nDestination file = (%s, %s)", dst_file_id, file_id);
	dst_file_id = file_id;
    } else {
	/*
	 * Keyword follows the source file id.
	 * Need to construct the file id from the keyword.
	 */
	const char *dev_id = NULL;
	boolean retcode;

	if (csb->which == COPY_FILE_ID_TFTP)
	    dev_id = TFTP_DEV_NAME;
	else if (csb->which == COPY_FILE_ID_RCP)
	    dev_id = RCP_DEV_NAME;
	else if (csb->which == COPY_FILE_ID_FLASH)
	    dev_id = get_default_dev_id();

	if (dev_id) {
	    retcode = convert_key_to_file_id(ext_dst_file_id, NULL,
					     "Enter destination file name",
					     dev_id,
					get_file_name_by_file_id(src_file_id));
	    if (retcode == FALSE) {
		return;
	    }
	    dst_file_id = ext_dst_file_id;
	}
    }

    /*
     * Source and destination may not be the same.
     */
    switch (csb->which) {
	case COPY_FILE_SCONFIG_ID:
	    file_id = get_config_file_id();
	    if (file_id && strcmp(file_id, dst_file_id) == 0) {
		printf("\nStartup-config file the same as %s", file_id);
		return;
	    }
	    break;

	case COPY_FILE_ID_SCONFIG:
	    file_id = get_config_file_id();
	    if (file_id && strcmp(file_id, src_file_id) == 0) {
		printf("\nStartup-config file the same as %s", file_id);
		return;
	    }
	    break;

	case COPY_FILE_ID_ID:
	    if (strcmp(src_file_id, dst_file_id) == 0) {
		printf("\nSource/Destination file may not be the same");
		return;
	    }
	    break;
    }

    /*
     * Make sure target file is ok to be overwritten.
     * 1. What about distilled config if target is startup-config?
     * 2. What about startup-config if target is distilled config?
     */
    if (csb->which == COPY_FILE_RCONFIG_ID ||
	csb->which == COPY_FILE_ID_ID)
    {
	if (file_protection_check(dst_file_id) == FALSE)
	    return;
    }

    switch (csb->which) {

	case COPY_FILE_RCONFIG_ID:
	    bufp = current_config_to_buf(&size);
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nCan not get running config to buffer");
		return;
	    }
	    break;

	case COPY_FILE_SCONFIG_RCONFIG:
	case COPY_FILE_SCONFIG_ID:
	    bufp = startup_config_to_buf(&size);
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nCan not get startup config to buffer");
		return;
	    }
	    break;

	case COPY_FILE_ID_SCONFIG:
	case COPY_FILE_ID_RCONFIG:
	    /*
	     * We make one assumption here that
	     * such file smaller enough we can
	     * handle with one buffer.
	     */
	    memset(&adrp, 0, sizeof(addrtype));
	    bufp = config_file_to_buf(src_file_id, &size, &adrp);
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nCan not get file to buffer");
		return;
	    }
	    /*
	     * Make sure we don't copy distilled config
	     * to startup/running config.
	     */
	    if (distilled_config(bufp) == TRUE) {
		printf("\nFile %s is distilled config, proceed? ", src_file_id);
		if (confirm(NULL) == FALSE)
		    return;
	    }
	    break;
		
	case COPY_FILE_ID_ID:
	case COPY_FILE_ID_TFTP:
	case COPY_FILE_ID_RCP:
	case COPY_FILE_ID_FLASH:
	    /*
	     * For this, we have to do one chunk at a time.
	     */
	    retcode = copy_file_to_file(src_file_id, 
					    dst_file_id);
		if (retcode < 0) {
		    if (filesys_debug) {
			printf("\nCan not copy from %s to %s",
			    src_file_id, dst_file_id);
		    }
		    return;
		}

	  default:
	    return;
	}

    /*
     *	Copy buffer of data to file.
     */
    switch (csb->which) {

	case COPY_FILE_SCONFIG_RCONFIG:
	case COPY_FILE_ID_RCONFIG:
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nNo buffer to copy from");
		break;
	    }
	    buf_to_current_config(bufp, size);
	    if (csb->which == COPY_FILE_ID_RCONFIG) {
		if (adrp.type) {
		    char source[50];
		    char tuba_buf[50];
		    char *ptr;
		    ptr = &tuba_buf[0];
		    sprintf(ptr, "%i", adrp.ip_addr);
		    sprintf(source, "from %s", ptr);
		    errmsg(&msgsym(CONFIG, SYS), src_file_id,
			user_on_stdio_str(), source);
		} else {
		    errmsg(&msgsym(CONFIG, SYS), src_file_id,
			user_on_stdio_str());
		}
	    } else {
		char *file_id;
		file_id = get_config_file_id();
		errmsg(&msgsym(CONFIG, SYS), file_id, user_on_stdio_str());
	    }
	    break;

	case COPY_FILE_ID_SCONFIG:
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nNo buffer to copy from");
		break;
	    }
	    retcode = buf_to_startup_config(bufp, size, TRUE);
	    if (retcode < 0) {
		if (filesys_debug)
		    printf("\nCan not save to startup config");
	    }
	    break;

	case COPY_FILE_RCONFIG_ID:
	case COPY_FILE_SCONFIG_ID:
	    if (bufp == NULL) {
		if (filesys_debug)
		    printf("\nNo buffer to copy from");
		break;
	    }
	    retcode = write_buffer_to_file(dst_file_id, bufp, size);
	    if (retcode < 0) {
		if (filesys_debug)
		    printf("\nCan not write buffer to file");
	    }
	    FS_settype(dst_file_id, IOCTL_FILE_TYPE_CONFIG);
	    break;
    }

    if (bufp)
	free(bufp);

    source = CONFIG_MEDIUM_LOCAL;
    dest = CONFIG_MEDIUM_LOCAL;

    switch (csb->which) {
      case COPY_FILE_SCONFIG_ID:
	source = CONFIG_MEDIUM_STARTUP;
	break;
      case COPY_FILE_RCONFIG_ID:
	source = CONFIG_MEDIUM_RUNNING;
	break;
      case COPY_FILE_ID_SCONFIG:
	dest = CONFIG_MEDIUM_STARTUP;
	break;
      case COPY_FILE_ID_RCONFIG:
	dest = CONFIG_MEDIUM_RUNNING;
	break;
      default:
	return;
    }

    reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
				    source, dest, 
				    0, NULL, NULL);
    return;
}

/*
 * Copy an individual file to individual file.
 */
static void copy_file_id_to_file_id (const char *src_file_id, const char *dst_file_id)
{
    int value;

    if (file_protection_check(dst_file_id) == FALSE)
	return;
    value = copy_file_to_file(src_file_id, dst_file_id);
}

/*
 * Used for "copy flash/tftp/rcp flash/tftp/rcp command".
 * Plus "copy flash/tftp/rcp/mop file-id".
 */
void rsp_flash_copy_command (parseinfo *csb)
{
    const char *dev_id;
    char *src_file_name;
    char src_file_id[FS_FILE_ID_MAX + 1];
    char dst_file_id[FS_FILE_ID_MAX + 1];
    boolean retcode;

    uint dev_type;
    char *dst_ptr;



    if (csb->which == COPY_MOP_FLASH || csb->which == COPY_MOP_FILE_ID) {
	printf("\nCopy from MOP server not supported");
	return;
    }

    if (csb->which == COPY_ERASE_FLASH || csb->which == COPY_VERIFY_FLASH) {
	printf("\nCommand not supported");
	return;
    }

    /*
     * If a device is specified, validate it before proceeding.
     * Determine if a device was specified by get_dev_id_by_file_id.
     * Determine if the device was valid by FS_ioctl.
     */

    if ((csb->which == COPY_FLASH_FILE_ID) ||
	(csb->which == COPY_TFTP_FILE_ID)  ||
	(csb->which == COPY_RCP_FILE_ID))
    {
        dst_ptr = GETOBJ(string,1);
	dev_id = get_dev_id_by_file_id(dst_ptr);
	if (dev_id != NULL) {
	    retcode = FS_ioctl(dst_ptr, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
	    if (retcode < 0) {
               printf("\nInvalid device id [%s]", dst_ptr);
	       return;
	    }
        }
    }

    switch (csb->which) {
	case COPY_FLASH_TFTP:
	case COPY_FLASH_RCP:
	case COPY_FLASH_FILE_ID:
	    dev_id = get_default_dev_id();
	    break;

	case COPY_TFTP_FLASH:
	case COPY_TFTP_FILE_ID:
	    dev_id = TFTP_DEV_NAME;
	    break;

	case COPY_RCP_FLASH:
	case COPY_RCP_FILE_ID:
	    dev_id = RCP_DEV_NAME;
	    break;

	default:
	    return;
    }
    retcode = convert_key_to_file_id(src_file_id, &src_file_name,
	"Enter source file name", dev_id, NULL);
    if (retcode == FALSE)
	return;

    /*
     * Build complete destination file id.
     * From user if needed.
     */
    if (csb->which == COPY_FLASH_FILE_ID ||
	csb->which == COPY_TFTP_FILE_ID ||
	csb->which == COPY_RCP_FILE_ID)
    {
	char *dst_file;
	int value;
	/*
	 * File id is given as dest, extend with default if needed.
	 */
	dst_file = GETOBJ(string,1);
	value = construct_whole_file_id(dst_file, dst_file_id,
	    FS_FILE_ID_MAX, get_file_name_by_file_id(src_file_name));
	if (value != 0) {
	    if (filesys_debug)
		printf("\nInvalid file id %s", dst_file);
	    return;
	}
    } else {
	/*
	 * Key word is given as dst.
	 */
	switch (csb->which) {
	    case COPY_TFTP_FLASH:
	    case COPY_RCP_FLASH:
		dev_id = get_default_dev_id();
		break;

	    case COPY_FLASH_TFTP:
		dev_id = TFTP_DEV_NAME;
		break;

	    case COPY_FLASH_RCP:
		dev_id = RCP_DEV_NAME;
		break;

	    default:
		return;
	}
	retcode = convert_key_to_file_id(dst_file_id, NULL,
	    "Enter destination file name", dev_id, 
	    get_file_name_by_file_id(src_file_name));
	if (retcode == FALSE)
	    return;
    }

    copy_file_id_to_file_id(src_file_id, dst_file_id);
    return;
}

/*
 * "delete <file-id>" command.
 */
void delete_command (parseinfo *csb)
{
    char *file_id;
    char ext_file_id[FS_FILE_ID_MAX + 1];
    int retcode;
    const char *msg;
    boolean fdev;

    file_id = GETOBJ(string, 1);
    retcode = get_whole_file_id(file_id, &ext_file_id[0], FS_FILE_ID_MAX);
    if (retcode != 0) {
	printf("\nInvalid file id %s", file_id);
	return;
    }
    file_id = &ext_file_id[0];
    /*
     * Allowalbe devices are nvram, flash, slot0/slot1.
     * File name must be present except nvram.
     */
    fdev = is_flash_device(file_id);
    if (fdev == FALSE && is_nv_device(file_id) == FALSE) {
	printf("\nCan not delete file on the device");
	return;
    }
    if (fdev == TRUE && get_file_name_by_file_id(file_id) == NULL) {
	printf("\nFile name missing");
	return;
    }

    if (file_protection_check(file_id) == FALSE) {
	if (filesys_debug)
	    printf("\nFile %s not deleted", file_id);
	return;
    }

    retcode = FS_delete(file_id);
    if (retcode < 0) {
	msg = error_code2text(retcode);
	printf("\nFile delete error (%s)", msg);
    }
    return;
}

/*
 * Search for a default device entry in the list
 */
static df_dev_rec_t *get_df_dev_entry (int ttynum)
{
    df_dev_rec_t *dfp;

    for (dfp=df_dev_q; dfp; dfp=dfp->next) {
	if (dfp->ttynum == ttynum)
	    return dfp;
    }
    return NULL;
}

/*
 * Put a default device entry into the list.
 */
static void put_df_dev_entry (df_dev_rec_t *dfp)
{
    dfp->next = df_dev_q;
    df_dev_q = dfp;
}

/*
 * Remove the named default device entry from the list.
 */
static boolean rem_df_dev_entry (int ttynum)
{
    df_dev_rec_t *dfp;
    df_dev_rec_t *prv = NULL;

    for (dfp=df_dev_q; dfp; dfp=dfp->next) {
	if (dfp->ttynum == ttynum) {
	    if (prv == NULL)
		df_dev_q = dfp->next;
	    else
		prv->next = dfp->next;
	    free(dfp);
	    return TRUE;
	}
	prv = dfp;
    }
    return FALSE;
}

/*
 * Obtain current default device.
 */
const char *get_default_dev_id (void)
{
    int ttynum;
    df_dev_rec_t *dfp;

    ttynum = stdio->ttynum;
    dfp = get_df_dev_entry(ttynum); /* see if we have entry in list */
    if (dfp == NULL) {
	dfp = malloc(sizeof(df_dev_rec_t)); /* allocate new entry if not */
	if (dfp == NULL) {
	    errmsg(&msgsym(NOMEMORY, SYS), "set default device");
	    return reg_invoke_default_flash_device();
	}
	memset(dfp, 0, sizeof(df_dev_rec_t));
	dfp->ttynum = ttynum;
	strcpy(dfp->df_dev, reg_invoke_default_flash_device());
	put_df_dev_entry(dfp);
    }
    return dfp->df_dev;
}

/*
 * Change current default device.
 */
void set_default_dev_id (const char *dev_id)
{
    df_dev_rec_t *dfp;
    int ttynum;

    if (dev_id == NULL)
		return;

    ttynum = stdio->ttynum;
    dfp = get_df_dev_entry(ttynum);
    if (dfp == NULL) {
	dfp = malloc(sizeof(df_dev_rec_t));
	if (dfp == NULL) {
	    errmsg(&msgsym(NOMEMORY, SYS), "get default device");
	    return;
	}
	memset(dfp, 0, sizeof(df_dev_rec_t));
	dfp->ttynum = ttynum;
	put_df_dev_entry(dfp);
    }
    sstrncpy(dfp->df_dev, dev_id, FS_DEV_ID_MAX);
}

/*
 * Destroy current default device.
 */
static void df_dev_clean (tt_soc *tty)
{
    if (tty)
	rem_df_dev_entry(tty->ttynum);
}

/*
 * "undelete <index> [dev-id]" command.
 */
void undelete_command (parseinfo *csb)
{
    const char *dev_id;
    int file_index;
    int value;
    const char *msg;

    /*
     * Get the device id and make sure it's allowable.
     */
    dev_id = GETOBJ(string, 1);
    if (*dev_id == 0)
	dev_id = get_default_dev_id();

    /*
     * Get the file index.
     */
    file_index = GETOBJ(int, 1);

    value = FS_undelete(dev_id, file_index, UNDELETE_UNIQUE);
    if (value < 0) {
	msg = error_code2text(value);
	printf("\nFile undelete error (%s)", msg);
    }
    return;
}

static void list_device_files (const char *dev_id, const char *file_name, int flag)
{
    char dev_id_buf[FS_DEV_ID_MAX + 1];
    int fd;
    const char *msg;
    boolean long_format;
    int first;
    int fcnt;
    int actual;
    char dbuffer[sizeof(dirent_t)];
    dirent_t *dirp;
    file_sys_stat_t status;
    int retcode;
    const char *busy_string;

    /*
     * Construct the device id.
     */
    if (dev_id)
	strcpy(dev_id_buf, dev_id);
    else {
	dev_id = get_default_dev_id();
	strcpy(dev_id_buf, dev_id);
    }

    strcat(dev_id_buf, ":");
    if (check_monlib_mismatch(dev_id,FALSE))
	printf("device not bootable, device format is from a different router type\n");
    strcat(dev_id_buf, ".");

    /*
     * Check to see if the device is busy.  If so, then print a message
     * and skip the directory listing altogether - because some operations
     * (format and squeeze) can take minutes to complete, and this command
     * would hang in "open".
     */
    if ((busy_string = get_busy_string(dev_id)) != NULL) {
	printf("\nDirectory listing is unavailable: %s", busy_string);
	return;
    }
    
    /*
     * List the directory.
     */

    fd = FS_open(dev_id_buf, O_RDONLY, 0);
    if (fd < 0) {
	msg = error_code2text(fd);
	printf("\nOpen device %s failed (%s)", dev_id, msg);
	return;
    }

    long_format = (flag == DIR_NORMAL) ? FALSE : TRUE;

    dirp = (dirent_t *)&dbuffer[0];
    automore_enable(NULL);
    for (first=1, fcnt=0; ; ) {
	actual = FS_getdents(fd, (char *)dirp, sizeof(dirent_t));
	if (actual == 0)
	    break;
	if (actual != sizeof(dirent_t)) {
	    if (filesys_debug)
		printf("\nGet directory error (%d)", actual);
	    break;
	}
	if (dirp->valid == 0) {
	    /*
	     *	Display invalid files only with /all.
	     */
	    if (flag != DIR_ALL)
		continue;
	} else if (dirp->deleted) {
	    /*
	     *	Display deleted files with /all or /deleted.
	     */
	    if (flag != DIR_ALL && flag != DIR_DELETED)
		continue;
	} else {
	    /*
	     *	Normal file.
	     */
	    if (flag == DIR_DELETED)
		continue;
	}
	if (file_name == NULL ||
	    match_filename(dirp->d_name, file_name) == TRUE)
	{
	    if (first) {
		if (long_format == TRUE)
		    print_dirent_header();
		else
		    print_dirent_header_short();
		first = 0;
	    }
	    fcnt++;
	    if (long_format == TRUE)
		print_dirent(dirp);
	    else
		print_dirent_short(dirp);
	}
    }
    FS_close(fd);
    if (fcnt == 0) {
	if (file_name == NULL || *file_name == 0)
	    printf("\nNo files on device");
	else
	    printf("\nNo such file");
	printf("\n");
    }

    /*
     * Show how much space available.
     */
    retcode = FS_fsstat(dev_id, &status);
    if (retcode == FS_OK) {
	printf("\n%d bytes available (%d bytes used)",
	    status.bytes_available, status.bytes_used);
    } else {
	msg = error_code2text(retcode);
	printf("\nError getting file system status (%s)", msg);
    }
    automore_disable();
}

/*
 * "dir" command.
 */
void dir_command (parseinfo *csb)
{
    int flag;
    char *file_id;
    const char *dev_id;
    const char *file_name;

    flag = GETOBJ(int, 1);
    if (flag != DIR_LONG && flag != DIR_ALL && flag != DIR_DELETED)
	flag = DIR_NORMAL;

    file_id = GETOBJ(string, 1);
    dev_id = get_dev_id_by_file_id(file_id);
    file_name = get_file_name_by_file_id(file_id);

    list_device_files(dev_id, file_name, flag);
}

/*
 * check if dev_id followed by ":" and null.
 */
static boolean check_device_id_colon (char *dev_id)
{
    char *ptr;

    ptr = strchr(dev_id, ':');
    if (ptr == NULL) {
	printf("\nMissing colon for device id %s", dev_id);
	return FALSE;
    }
    if (*++ptr) {
	printf("\nFile name not allowed %s", dev_id);
	return FALSE;
    }
    *--ptr = 0;
    return TRUE;
}

/*
 * "squeeze <dev-id>" command
 */
void squeeze_command (parseinfo *csb)
{
    char *dev_id;
    int retcode;
	
    dev_id = GETOBJ(string, 1);
    if (check_device_id_colon(dev_id) == FALSE)
	return;

    /*
     * Confirm with user.
     */
    printf("\nAll deleted files will be removed, proceed? ");
    if (confirm(NULL) == FALSE)
	return;
    printf("\nSqueeze operation may take a while, proceed? ");
    if (confirm(NULL) == FALSE)
	return;
    retcode = FS_squeeze(dev_id, SQUEEZE_NORMAL);
    if (retcode < 0) {
	printf("\nSqueeze device %s failed (%s)", dev_id,
	    error_code2text(retcode));
    }
    return;
}

/*
 * "format <dev-id>" command.
 */
void format_command (parseinfo *csb)
{
    int spare;
    format_parm_t fm_rec;
    format_parm_t *fmp = &fm_rec;
    char *dev_id;
    char *bufp;
    boolean retcode;
    int error;

    spare = GETOBJ(int, 1) ? GETOBJ(int, 2) : 0;
    fmp->spare = spare;

    /*
     * Get device id and check if it's valid.
     */
    dev_id = GETOBJ(string, 1);
    bufp = strchr(dev_id, ':');
    if (bufp == NULL) {
	printf("\nMissing colon for device id %s", dev_id);
	return;
    }
    *bufp = 0;

    /*
     * Be careful of erasing important files.
     */
    if (device_protection_check(dev_id) == FALSE)
	return;

    /*
     * One more warning.
     */
    printf("\nAll sectors will be erased, proceed? ");
    flush();
    if (confirm(NULL) == FALSE)
	return;

    /*
     * Get volumne id up front. Once get it, we block
     * out other access and proceed formatting.
     */
    printf("\nEnter volume id (up to 30 characters): ");
    flush();
    retcode = rdtty(fmp->volume_id, IOCTL_FORMAT_VOLUME_MAX);
    if (retcode == FALSE) {
	printf("\n?Bad volume id");
	return;
    }

    /*
     * Get possible user specified monlib image.
     */
    fmp->monlib = GETOBJ(string, 2);
    fmp->monlib_bndl = (char *)&monlib_bundle[0];
    fmp->monlib_bndl_size = monlib_bundle_size;

    error = FS_ioctl(dev_id, IOCTL_FORMAT_FLASH, (void *)fmp);
    if (error < 0) {
	printf("\nFormat device %s failed (%s)",
	    dev_id, error_code2text(error));
    }
}

/*
 * "cd [<dev-id>]" command.
 */
void cd_command (parseinfo *csb)
{
    char *dev_id;

    dev_id = GETOBJ(string, 1);

    if (dev_id == NULL || *dev_id == 0) {
	/*
	 * Default device.
	 */
	set_default_dev_id(reg_invoke_default_flash_device());
    } else {
	if (check_device_id_colon(dev_id) == FALSE)
	    return;
	if (is_flash_device(dev_id) == FALSE) {
	    printf("\nInvalid device");
	    return;
	}
	set_default_dev_id(dev_id);
    }
}

/*
 * "pwd" command.
 */
void pwd_command (parseinfo *csb)
{
    printf("\n%s", get_default_dev_id());
}

/*
 * "boot config <file-id>" config command.
 */
void boot_config_file_command (parseinfo *csb)
{
    char *file_id;
    uint dev_type;
    char *bufp;
    stat_t status;
    const char *file_name;
    int retcode;
    const char *msg;

    if (csb->nvgen) {
	/*
	 * write this config command to nvram.
	 */
	nv_write(config_file_id[0], "%s ", csb->nv_command);
	nv_add(config_file_id[0], "%s", config_file_id);
	/*
	 * Update environment varialbe with new value.
	 */
	bufp = reg_invoke_monvar_buff(ROMMON_CONFIG_VAR);
	if (bufp) {
	    bufp += strlen(bufp);
	    sprintf(bufp, "%s", &config_file_id[0]);
	}
	return;
    }

    if (csb->sense == FALSE) {
	/*
	 * Wipe out the config file variable.
	 * This effectively default config to nvram.
	 */
	memset(&config_file_id[0], 0, MAX_CONFIG_FILE_ID);
	return;
    }

    file_id = GETOBJ(string, 1);

    if (get_dev_id_by_file_id(file_id) == NULL) {
	printf("\nMissing device id");
	return;
    }
    retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
    if (retcode < 0) {
	printf("\nInvalid device id");
	return;
    }
    if (!(dev_type & IOCTL_DEV_TYPE_LOCAL)) {
        printf("\nSpecified device must be local (not slave)");
        return;
    }
    file_name = get_file_name_by_file_id(file_id);

    /*
     *	Device must be flash or nvram.
     *	And check validity in each case.
     */
    if (dev_type & IOCTL_DEV_TYPE_FLASH) {
	if (file_name == NULL) {
	    printf("\nMissing file name");
	    return;
	}
	retcode = FS_stat(file_id, &status);
	if (retcode != FS_OK) {
	    msg = error_code2text(retcode);
	    printf("\nFile %s not found (%s)", file_id, msg);
	    return;
	}
	if (status.type == FILE_TYPE_SYS_IMAGE) {
	    printf("\nFile %s is an image file", file_id);
	    return;
	}
    } else if (dev_type & IOCTL_DEV_TYPE_NV) {
	if (file_name) {
	    printf("\nNo file name allowed");
	    return;
	}
	/*
	 * Wipe out the config file variable.
	 * This effectively default config to nvram.
	 */
	memset(&config_file_id[0], 0, MAX_CONFIG_FILE_ID);
	return;
    } else {
	printf("\nInvalid device id");
	return;
    }
    /*
     * Verified ok, save the config file.
     */
    strcpy(config_file_id, file_id);
    return;
}

/*
 * "boot bootldr <file-id>" config command.
 */
void boot_helper_file_command (parseinfo *csb)
{
    char *bufp;
    stat_t status;
    char *file_name;
    int retcode;
    const char *msg;

    if (csb->nvgen) {
	/*
	 * write this config command to nvram.
	 */
	nv_write(rxboot_file_id[0], "%s ", csb->nv_command);
	nv_add(rxboot_file_id[0], "%s", rxboot_file_id);
	/*
	 * Update environment varialbe with new value.
	 */
	bufp = reg_invoke_monvar_buff(ROMMON_BOOTLDR_VAR);
	if (bufp) {
	    bufp += strlen(bufp);
	    sprintf(bufp, "%s", rxboot_file_id);
	}
	return;
    }

    if (csb->sense == FALSE) {
	/*
	 * Wipe out the config file variable.
	 * This effectively default config to nvram.
	 */
	memset(&rxboot_file_id[0], 0, MAX_CONFIG_FILE_ID);
	return;
    }

    file_name = GETOBJ(string, 1);

    /*
     * Device must be flash
     * And check validity in each case.
     */
    if (is_flash_device(file_name)) {
	if (get_file_name_by_file_id(file_name) == NULL) {
	    printf("\nFile name missing");
	    return;
	}
	retcode = FS_stat(file_name, &status);
	if (retcode != FS_OK) {
	    msg = error_code2text(retcode);
	    printf("\nFile %s not found (%s)", file_name, msg);
	    return;
	}
	if (status.type == FILE_TYPE_SYS_CONFIG) {
	    printf("\nFile %s is a config file", file_name);
	    return;
	}
    } else {
	printf("\nInvalid device id");
	return;
    }
    /*
     * Verified ok, save the config file.
     */
    strcpy(rxboot_file_id, file_name);
    return;
}

/*
 * "show boot" command.
 */
void show_boot_command (parseinfo *csb)
{
    char *str;
    ushort new_configreg;

    str = reg_invoke_monvar_get(ROMMON_BOOT_VAR);
    if (str)
	printf("\nBOOT variable = %s", str);
    else
	printf("\nBOOT variable does not exist");

    str = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (str)
	printf("\nCONFIG_FILE variable = %s", str);
    else
	printf("\nCONFIG_FILE variable does not exist");
    printf("\nCurrent CONFIG_FILE variable = %s", &config_file_id[0]);

    str = reg_invoke_monvar_get(ROMMON_BOOTLDR_VAR);
    if (str)
	printf("\nBOOTLDR variable = %s", str);
    else
	printf("\nBOOTLDR variable does not exist");

    printf("\nConfiguration register is %#x", (ushort)configregister);
    new_configreg = (ushort) ~mon_getconfig();
    if (configregister != new_configreg) {
	printf(" (will be %#x at next reload)\n", new_configreg);
    } else {
	printf("\n");
    }

    reg_invoke_show_slave_boot();
}

/*
 * "verify <file-id>" command.
 */
void verify_file_command (parseinfo *csb)
{
    char *file_id;
    char ext_file_id[FS_FILE_ID_MAX + 1];
    int retcode;
    int fd;
    const char *msg;

    file_id = GETOBJ(string, 1);

    retcode = get_whole_file_id(file_id, &ext_file_id[0], FS_FILE_ID_MAX);
    if (retcode != 0) {
	printf("\nInvalid file id %s", file_id);
	return;
    }

    file_id = &ext_file_id[0];

    /*
     * Allowalbe devices are flash, slot0/slot1.
     * File name must be present.
     */
    if (is_flash_device(file_id) == FALSE) {
	printf("\nCan not verify file on device");
	return;
    }
    if (get_file_name_by_file_id(file_id) == NULL) {
	printf("\nFile name missing");
	return;
    }
    fd = FS_open(file_id, O_RDONLY, 0);
    if (fd > 0) {
	FS_close(fd);
	printf("\nFile %s verified OK", file_id);
	return;
    }
    msg = error_code2text(fd);
    printf("\nFile verification failed (%s)", msg);
}

boolean rsp_flash_tftp_server (ipsocktype *soc, const char *filename)
{
    boolean result;
    int blknum;
    int nbytes;
    int fd;
    char buffer[TFTP_MAXDATA];

    fd = FS_open(filename, O_RDONLY | O_VERBOSE_OFF | O_CRC_OFF, 0);
    if (fd < 0) {
	if (filesys_debug)
	    printf("\nOpen file %s error (%d) as server", filename, fd);
	return FALSE;
    }
    result = TRUE;
    for (blknum = 1; result; blknum++) {
	nbytes = FS_read(fd, buffer, TFTP_MAXDATA);
	if (nbytes < 0) {
	    if (filesys_debug)
		printf("\nRead file %s error (%d) as server", filename,nbytes);
	    FS_close(fd);
	    return FALSE;
	}
	result = reg_invoke_ip_tftp_sendblock(soc, blknum, buffer,
	    nbytes, TRUE);
	/*
	 * If we read less than specified from flash,
	 * we must have reached the end.
	 */
	if (nbytes < TFTP_MAXDATA)
	    break;
    }
    FS_close(fd);
    return result;
}

#define	DEVICE_LIST_LEN		128

/*
 * list all file system devices
 */
static int list_all_devices (void)
{
    char device_list[DEVICE_LIST_LEN];
    int retcode;
    char *dp;
    char *tp;
    int count;

    retcode = FS_getdevicenames(device_list, DEVICE_LIST_LEN);
    if (retcode != FS_OK) {
	return retcode;
    }
    if (null(device_list) == TRUE)
	return FS_OK;
    printf("\n");
    count = 0;
    for (dp = device_list;;) {
	tp = strchr(dp, ' ');
	if (tp)
	    *tp = 0;
	if (null(dp))
	    break;
	if (dp != device_list)
	    printf(", ");
	if (count >= 40) {
	    printf("\n");
	    count = 0;
	}
	count += printf("%s", dp);
	if (tp == NULL)
	    break;
	dp = ++tp;
    }
    return FS_OK;
}

void show_flash_file_command (parseinfo *csb)
{
    int flag;
    char *dev_id;
    file_sys_stat_t status;
    int retcode;
    const char *msg;
    const char *busy_string;

    flag = GETOBJ(int, 1);
    dev_id = GETOBJ(string, 1);
    if (dev_id == NULL || *dev_id == 0)
	dev_id = (char *)get_default_dev_id();
    else if (check_device_id_colon(dev_id) == FALSE)
	return;

    if (flag != SHOW_FLASH_FILE_FILESYS && flag != SHOW_FLASH_FILE_CHIPS &&
	flag != SHOW_FLASH_FILE_ALL && flag != SHOW_FLASH_FILE_DEV)
    {
	flag = SHOW_FLASH_FILE_NORMAL;
    }

    /*
     * Quit if device is busy, because the subsequent calls
     * (open, fsstat, ioctl) will not work properly.
     */
    if ((busy_string = get_busy_string(dev_id)) != NULL) {
	printf("\nShow flash is unavailable: %s", busy_string);
	return;
    }
    
    /*
     * Show files on device if need to.
     */
    if (flag == SHOW_FLASH_FILE_NORMAL || flag == SHOW_FLASH_FILE_ALL)
	list_device_files(dev_id, NULL, DIR_ALL);

    automore_enable(NULL);
    /*
     * Show file system info if need to.
     */
    if (flag == SHOW_FLASH_FILE_FILESYS || flag == SHOW_FLASH_FILE_ALL) {
	/*
	 * Print file sys info on this device.
	 */
	retcode = FS_fsstat(dev_id, &status);
	if (retcode != FS_OK) {
	    msg = error_code2text(retcode);
	    printf("\nError getting file system status (%s)", msg);
	    automore_disable();
	    return;
	}
	printf("\n\n");
	print_file_sys_stat(&status);
    }

    /*
     * Show flash chips info if need to.
     */
    if (flag == SHOW_FLASH_FILE_CHIPS || flag == SHOW_FLASH_FILE_ALL) {
	retcode = FS_ioctl(dev_id, IOCTL_PRINT_CHIP, NULL);
	if (retcode) {
	    msg = error_code2text(retcode);
	    printf("\nPrint chip info failed (%s)", msg);
	}
    }

    if (flag == SHOW_FLASH_FILE_DEV || flag == SHOW_FLASH_FILE_ALL) {
	retcode = list_all_devices();
	if (retcode != FS_OK) {
	    msg = error_code2text(retcode);
	    printf("\nError getting device names (%s)", msg);
	}
    }

    automore_disable();
}

/****************************************************************************
 *
 *	Utility routine.
 *
 ****************************************************************************/

/*
 * Fetch current running configuration into a buffer.
 */
char *current_config_to_buf (int *size)
{
    char *bufp;
    int bufsize;
    int cnt;

    /*
     *	XXX, this may not big enough.
     */
    bufsize = nv_get_config_size();

    bufp = malloc(bufsize);
    if (bufp == NULL) {
	return NULL;
    }

    cnt = nv_current_to_buffer(bufp, bufsize, TRUE,
			       CUR_PRIV(stdio->privilege), FALSE);
    *size = cnt;
    return bufp;
}

/*
 * Fetch startup configuration into a buffer.
 */
char *startup_config_to_buf (int *size)
{
    char *file_id;
    char *bufp;
    int bufsize;
    int cnt;
    int fd;
    const char *msg;

    file_id = fetch_config_file_id();
    if (file_id == NULL) {
	printf("\nCan not get config file name");
	return NULL;
    }
    fd = FS_open(file_id, O_RDONLY, 0);
    if (fd < 0) {
	printf("\nCan not open config file %s", file_id);
	return NULL;
    }

    bufsize = get_file_size(fd);
    if (bufsize < 0) {
	printf("\nCan not get size of config file %s", file_id);
	return NULL;
    }

    bufp = malloc(bufsize);
    if (bufp == NULL) {
	printf("\nNo memory for config buffer");
	FS_close(fd);
	return NULL;
    }

    cnt = FS_read(fd, bufp, bufsize);
    if (cnt < 0) {
	msg = error_code2text(cnt);
	printf("\nStartup config file %s read error (%s)", file_id, msg);
	free(bufp);
	FS_close(fd);
	return NULL;
    } else if (cnt < bufsize)
	printf("\nRead config %d bytes instead of %d", cnt, bufsize);

    *size = cnt;
    FS_close(fd);
    return bufp;
}

/*
 * Fetch startup private configuration into a buffer. 
 */
char *startup_private_config_to_buf (int *size)
{
    addrtype adrp;

    return config_file_to_buf(nv_private_fileid, size, &adrp);
}

char *get_config_file_id (void)
{
    char *file_id;
    uint dev_type;
    int retcode;

    file_id = &config_file_id[0];

    if (*file_id) {
	retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
	if (retcode >= 0 && (dev_type & (IOCTL_DEV_TYPE_NV | IOCTL_DEV_TYPE_FLASH))) {
	    return file_id;
	}
    }
    return NULL;
}

/*
 * Default config file variable to nvram if current
 * is not valid (wrong device) or non-existing.
 * Return pointer to variable.
 */
static char *fetch_config_file_id (void)
{
    char *file_id;
    char *ptr;

    file_id = get_config_file_id();
    if (file_id == NULL) {
	file_id = &config_file_id[0];
	strcpy(file_id, NV_DEV_NAME);
	ptr = file_id + strlen(file_id);
	*ptr++ = ':';
	*ptr = 0;
    }
    return file_id;
}

/*
 *	This makes max file of 8MB able to read.
 */
#define	MEM_ARRAY_CNT	64
#define	MEM_BLOCK_SIZE	128*1024

/*
 * Read a file into a buffer.
 * If size is not given, use memory array.
 */
char *read_file_to_buf (int fd, int *size)
{
    char *bufp;
    const char *msg;
    int cnt;
    int total;
    int count;
    char *mem_array[MEM_ARRAY_CNT];
    char *buffer;
    int index;
    char *src;
    char *dst;

    if (*size > 0) {
	bufp = malloc(*size);
	if (bufp == NULL) {
	    printf("\nNo memory to read file");
	    return NULL;
	}
	cnt = FS_read(fd, bufp, *size);
	if (cnt < 0) {
	    msg = error_code2text(cnt);
	    printf("\nRead file error (%s)", msg);
	    free(bufp);
	    return NULL;
	}
	*size = cnt;
	return bufp;
    }

    total = 0;
    memset(&mem_array[0], 0, MEM_ARRAY_CNT*sizeof(char *));
    for (cnt=0; cnt<MEM_ARRAY_CNT; cnt++) {
	bufp = malloc(MEM_BLOCK_SIZE);
	if (bufp == NULL) {
	    printf("\nNo memory for read file");
	    /*
	     * Free all allocated buffers.
	     */
	    for (index=0; index<cnt; index++) {
		bufp = mem_array[index];
		free(bufp);
	    }
	    return NULL;
	}
	mem_array[cnt] = bufp;
	count = FS_read(fd, bufp, MEM_BLOCK_SIZE);
	if (count < 0) {
	    msg = error_code2text(count);
	    printf("\nRead file error (%s)", msg);
	    /*
	     * Free all allocated buffers.
	     */
	    for (index=0; index<=cnt; index++) {
		bufp = mem_array[index];
		free(bufp);
	    }
	    return NULL;
	} else if (count < MEM_BLOCK_SIZE) {
	    /*
	     * This is the last buffer, we are done.
	     * Group all smaller buffers into one big one
	     * if we have to.
	     */
	    if (cnt == 0) {
		*size = count;
		return bufp;
	    }
	    total += count;
	    buffer = malloc(total);
	    if (buffer == NULL) {
		printf("\nNo memory for read file");
		for (index=0; index<=cnt; index++) {
		    bufp = mem_array[index];
		    free(bufp);
		}
		return NULL;
	    }
	    /*
	     * Copy all smaller buffers into big one.
	     */
	    for (index=0, dst=buffer; index<=cnt;
		index++, dst+=MEM_BLOCK_SIZE)
	    {
		src = mem_array[index];
		if (index == cnt) {
		    memcpy(dst, src, count);
		    free(src);
		    *size = total;
		    return buffer;
		}
		memcpy(dst, src, MEM_BLOCK_SIZE);
		free(src);
	    }
	    break;
	}
	total += MEM_BLOCK_SIZE;
    }
    /*
     * We come here because array is full.
     */
    printf("\nFile is too big to read");
    for (index=0; index<=MEM_ARRAY_CNT; index++) {
	bufp = mem_array[index];
	free(bufp);
    }
    return NULL;
}

/*
 * Copy content of file into a buffer.
 * Also return address in case of remote server.
 */
char *config_file_to_buf (const char *file_id, int *size, addrtype *adrp)
{
    int fsize;
    char *bufp;
    int fd;
    stat_t status;
    int retcode;
    const char *msg;
    boolean is_private_file_id = FALSE;
   
   if (NV_MATCH_PRIVATE_FILE_ID(file_id)) {
    	retcode = FS_ioctl(file_id, IOCTL_SET_PRIVATE_RW, (void *)TRUE);
    	if (retcode) {
	    msg = error_code2text(retcode);
	    printf("\nCan not set attributes for file %s status (%s)",
								file_id, msg);
	    return NULL;
	}
	is_private_file_id = TRUE;
    }

    fd = FS_open(file_id, O_RDONLY, 0);
    if (fd < 0) {
	if (is_private_file_id) {
	    (void) FS_ioctl(file_id, IOCTL_SET_PRIVATE_RW, (void *)FALSE);
	} else {
	    msg = error_code2text(fd);
	    printf("\nConfig file %s can not be opened (%s)", file_id, msg);
	}
	return NULL;
    }

    memset(&status, 0, sizeof(stat_t));
    retcode = FS_fstat(fd, &status);
    if (retcode < 0) {
	msg = error_code2text(retcode);
	printf("\nCan not get file %s status (%s)", file_id, msg);
	FS_close(fd);
	return NULL;
    }

    /*
     * We won't load the file to config buffer if its
     * type is specifically set to IMAGE.
     */
    if (status.type == FILE_TYPE_SYS_IMAGE) {
	printf("\nFile type (%d) is not configuration", status.type);
	FS_close(fd);
	return NULL;
    }

    if (status.server_addr)
	address_copy(adrp, status.server_addr);
    /*
     * Allocate the buffer according to size of file.
     */
    fsize = status.st_size;
    bufp = read_file_to_buf(fd, &fsize);

    FS_close(fd);
    *size = fsize;
    return bufp;
}

/*
 * Return true if the file is the current startup config file.
 */
static boolean is_startup_config_file (const char *file_id)
{
    char *cfg_file_id;

    cfg_file_id = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (cfg_file_id == NULL || *cfg_file_id == 0) {
	/*
	 * startup config is effectively nvram:
	 */
	if (is_nv_device(file_id) == TRUE)
	    return TRUE;
    } else {
	if (strcmp(cfg_file_id, file_id) == 0)
	    return TRUE;
    }
    return FALSE;
}

static boolean check_device_space (const char *dev_id,
    int *space_left, boolean to_confirm)
{
    int value;
    const char *msg;
    file_sys_stat_t status;

    *space_left = -1;

    if (is_flash_device(dev_id) == FALSE)
	return TRUE;

    value = FS_fsstat(dev_id, &status);
    if (value != FS_OK) {
	msg = error_code2text(value);
	printf("\nError getting file system status (%s)", msg);
	return FALSE;
    }

    value = status.bytes_available - status.file_hdr_size;
    if (value < 0) {
	printf("\nFile system status corrupted (%d)", value);
	return FALSE;
    }
    *space_left = value;

    if (to_confirm) {
	printf("\n%d bytes available on device %s, proceed? ", value, dev_id);
	if (confirm(NULL) == FALSE)
	    return FALSE;
    }
    return TRUE;
}

/*
 * IP_header_getword
 *
 * copies s to t thru NULL (until max)
 * strips NULLS off end of string
 */
static char *IP_header_getword (char *s,char *t,int max) 
{
    do {
	*t++ = *s;
    }
    while ((*s++ != 0) && (--max));

    while(*s == 0)
	s++; 
    return(s);
}

/*
 * IP_header_parse
 */
static int IP_header_parse (char *p, char *name, int *size_p) 
{
    char size_s[40];

    if (*p == 0)                /* empty header */
	return(-1);
    p = IP_header_getword(p+2,name,101);  /* start two chars in (skip "./") */
    p = IP_header_getword(p,size_s,40);	  /* throw away */
    p = IP_header_getword(p,size_s,40);	  /* throw away */
    p = IP_header_getword(p,size_s,40);	  /* throw away */
    p = IP_header_getword(p,size_s,40);   /* the size (in octal, no less !)*/

    *size_p = atoo(size_s);
    return(0);
}

static boolean is_IP_multi_segment_file (const char * src_file_id, int sfd) {

    char *bufp;
    const char *msg;
    int cnt;
    boolean ret_code = FALSE;

    /*
     * Read in first "block" of source and check for image header
     */
    bufp = malloc(IP_IMAGE_HEADER_SIZE);
    if (bufp == NULL) {
	printf(nomemory);
	return(ret_code);
    }

    cnt = FS_read(sfd, bufp, IP_IMAGE_HEADER_SIZE );
    if (cnt < 0) {
	msg = error_code2text(cnt);
	printf("\nRead file %s error (%s)", src_file_id, msg);
    }
    else if (strncmp("./",bufp,2) == 0)
	/*
	 * IP image files in tar format begin with a header which 
	 * starts with the file name (./filename).
	 */
	ret_code = TRUE;
    /*
     * Put things back where they were
     */
    cnt = FS_lseek(sfd, 0, SEEK_SET);
    if (cnt < 0) {
	msg = error_code2text(cnt);
	printf("\nLseek file %s error (%s)", src_file_id, msg);
    }
    free(bufp);
    return (ret_code);
}

/*
 * Copy file one chunk at a time.
 */
int copy_file_to_file (const char *src_file_id, const char *dst_file_id)
{
    char *bufp;
    int sfd;
    int dfd;
    int flags;
    int cnt;
    int bufsize;
    int total;
    int num;
    int first;
    int type;
    const char *msg;
    int size;
    const char *dev_id;
    int space_left = -1;
    uint dev_type;
    uint dev_type2;
    int retcode;
    char *private_bufp;
    int private_size;
    addrtype adrp;

    char name[FLASH_NAME_LEN + 1]; 
    char *name_p;              /* ptr to name array             */

    bufsize = 4*1024;
    bufp = malloc(bufsize);
    if (bufp == NULL) {
	printf("\nNo more memory for buffer");
	return -1;
    }

    /*
     * Check how much space left on device,
     * and confirm with user.
     */
    dev_id = get_dev_id_by_file_id(dst_file_id);
    if (check_device_space(dev_id, &space_left, TRUE) == FALSE) {
	free(bufp);
	return -1;
    }

    sfd = FS_open(src_file_id, O_RDONLY, 0);
    if (sfd < 0) {
	msg = error_code2text(sfd);
	printf("\nCan not open source file %s (%s)", src_file_id, msg);
	free(bufp);
	return -1;
    }

    /*
     * If we know source file size, check if
     * we have enough space left.
     */
    size = get_file_size(sfd);
    if (size > 0 && space_left > 0) {
	if (size > space_left) {
	    dev_id = get_dev_id_by_file_id(dst_file_id);
	    printf("\nNot enough space left on device %s", dev_id);
	    FS_close(sfd);
	    free(bufp);
	    return -1;
	}
    }

    /*
     * Check first for special IP file.
     */
    if (!is_IP_multi_segment_file(src_file_id, sfd)) {
    
	/*
	 * If the config is in the nvram: device, save the private config
	 * before writing the config from the buffer. Restore it afterwords.
	 */
	if (!strcmp(dst_file_id, NV_FILE_ID)) {
	    private_bufp = 
		    config_file_to_buf(nv_private_fileid, &private_size, &adrp);
	} else {
	    private_bufp = NULL;
	}

	flags = O_WRONLY | O_APPEND | O_TRUNC | O_CREAT;
	dfd = FS_open(dst_file_id, flags, 0);
	if (dfd < 0) {
	    msg = error_code2text(dfd);
	    printf("\nCan not open destination file %s (%s)", dst_file_id, msg);
	    FS_close(sfd);
	    free(bufp);
	    return -1;
	}

	/*
	 * Destination rcp device needs to know
	 * the size of file.
	 */
	retcode = FS_ioctl(dst_file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
	if (retcode == FS_OK && (dev_type & IOCTL_DEV_TYPE_RCP)) {
	    if (size <= 0) {
		printf("\nCan not copy to rcp device");
		FS_close(sfd);
		FS_close(dfd);
		FS_delete(dst_file_id);	/* remove uncomplete file */
		free(bufp);
		return -1;
	    }
	    FS_ioctl(dst_file_id, IOCTL_SET_FILE_SIZE, (void *)size);
	}

	/*
	 * keep reading one chunk from
	 * source file and write it to
	 * destination file.
	 */
	first = 1;
	total = 0;
	for (;;) {
	    cnt = FS_read(sfd, bufp, bufsize);
	    if (cnt < 0) {
                retcode = FS_ioctl(dst_file_id, IOCTL_SET_VERBOSE,
                                   (void *)FALSE);
		msg = error_code2text(cnt);
		printf("\nRead file %s error (%s)", src_file_id, msg);
		FS_close(sfd);
		FS_close(dfd);
		free(bufp);
		FS_delete(dst_file_id);	/* remove uncomplete file */
		return -1;
	    } else if (cnt == 0)
		break;

	    /*
	     * Make sure we are not copying distilled config to
	     * a file which happen to be startup config by accident.
	     */
	    if (first) {
		first = 0;
		if (distilled_config(bufp) == TRUE &&
		    is_startup_config_file(dst_file_id) == TRUE)
		    {
			printf("\nCopy distilled config to startup config? ");
			if (confirm(NULL) == FALSE) {
			    FS_close(sfd);
			    FS_close(dfd);
			    free(bufp);
			    FS_delete(dst_file_id); /* remove uncomplete file */
			    return -1;
			}
		    }
	    }

	    /*
	     * Let other processes run between chunks
	     */
	    process_suspend();

	    /*
	     * Make sure we have enough space left on
	     * destination device.
	     */
	    if (space_left >= 0) {
		if (space_left < cnt) {
                    retcode = FS_ioctl(dst_file_id, IOCTL_SET_VERBOSE,
                                       (void *)FALSE);
		    dev_id = get_dev_id_by_file_id(dst_file_id);
		    printf("\nDevice %s is full", dev_id);
		    FS_close(sfd);
		    FS_close(dfd);
		    free(bufp);
		    FS_delete(dst_file_id); /* remove uncomplete file */
		    return -1;
		}
		space_left -= cnt;
	    }

	    num = FS_write(dfd, bufp, cnt);
	    if (num != cnt) {
                retcode = FS_ioctl(dst_file_id, IOCTL_SET_VERBOSE,
                                   (void *)FALSE);
		if (num < 0) {
		    msg = error_code2text(num);
		    printf("\nWrite file %s error (%s)", dst_file_id, msg);
		} else {
		    printf("\nWrite %d bytes instead of %d", num, cnt);
		}
		FS_close(sfd);
		FS_close(dfd);
		free(bufp);
		FS_delete(dst_file_id);	/* remove uncomplete file */
		return -1;
	    }

	    total += num;
	    /*
	     * Print a "c" every 8K if we don't copy from/to
	     * tftp/rcp devices.
	     */
	    if ((total & 0x1FFF) == 0) {
		retcode = FS_ioctl(src_file_id, IOCTL_GET_DEV_TYPE,
				   (void *)&dev_type2);
		if (retcode == FS_OK &&
		    (dev_type & (IOCTL_DEV_TYPE_TFTP | IOCTL_DEV_TYPE_RCP)) == 0 &&
		    (dev_type2 & (IOCTL_DEV_TYPE_TFTP | IOCTL_DEV_TYPE_RCP)) == 0)
		    {
			printf("c");
			flush();
		    }
	    }
	    process_suspend();

	} /* for */

	/*
	 * Free all resources.
	 */
	FS_close(sfd);
	FS_close(dfd);
	free(bufp);
   
	if (private_bufp) {
	    cnt = 
	      write_buffer_to_file(nv_private_fileid,private_bufp,private_size);
	    if (cnt < 0) {
		printf("\nRestore of private config failed.");
	    }
	    free(private_bufp);
	}

	/*
	 * Keep file type the same.
	 */
	type = get_file_type(src_file_id);
	if (type >= 0)
	    FS_settype(dst_file_id, type);

	return total;
    }
    else {

	/*
	 * Begin the full filename with the ip image file prefix
	 */
	if (strlen(dst_file_id) >= FLASH_NAME_LEN - IP_IMAGE_SUFFIX_LEN) {
	    printf("\nDestination name too long %s ", dst_file_id);
	    free(bufp);
	    FS_close(sfd);
	    return -1;
	}

	strcpy(name, dst_file_id);
	name_p = name + strlen(name);

	/*
	 * Create a directory file for the multi-segments
	 */
	if ( (dfd = FS_open(dst_file_id, 
			    O_WRONLY | O_CREAT | O_APPEND | O_TRUNC,0)) < 0) {

	    msg = error_code2text(dfd);
	    printf("\nCan not open destination file %s (%s)", dst_file_id,msg);
	    free(bufp);
	    FS_close(sfd);
	    return -1;
	}
	num = FS_write(dfd, bufp, 1);
	if (num != 1) {
	    msg = error_code2text(num);
	    printf("\nWrite file %s error (%s)", dst_file_id, msg);
	}
	FS_close(dfd);


	/*
	 * Loop reading image headers
	 */
	while ( FS_read(sfd, bufp, IP_IMAGE_HEADER_SIZE) > 0) {
	    /*
	     * Ideally, we should be checking the return code.  If negative,
	     * alerting and removing the dst file - not just ending the loop.
	     */
	    if (IP_header_parse(bufp, name_p, &size) < 0) {
		free(bufp);
		FS_close(sfd);
		return -1;
	    }
	
	    printf("\n--- expanding multi-segment file --- \n%s size = %d\n",name, size);

	    if ( (dfd = FS_open(name, 
				O_WRONLY | O_CREAT | O_APPEND | O_TRUNC,0)) < 0) {

		msg = error_code2text(dfd);
		printf("\nCan not open destination file %s (%s)", name, msg);
		FS_close(sfd);
		free(bufp);
		return -1;
	    }

	    /*
	     * The basic process is to loop on large chunks 
	     * reading - writing, then when exhausted,
	     * read - write the image block size.
	     */
	    while (size >= bufsize) {
		if ( (cnt = FS_read(sfd, bufp, bufsize)) != bufsize) {
		    msg = error_code2text(cnt);
		    printf("\nRead file %s error (%s)", src_file_id, msg);
		    free(bufp);
		    FS_close(sfd);
		    FS_close(dfd);
		    FS_delete(name); /* remove incomplete file */
		    return -1;
		}

		num = FS_write(dfd, bufp, bufsize);
		if (num != bufsize) {
		    if (num < 0) {
			msg = error_code2text(num);
			printf("\nWrite file %s error (%s)", name, msg);
		    } else {
			dev_id = get_dev_id_by_file_id(dst_file_id);
			printf("\nWrite %d bytes instead of %d (Device %s is full)",
			       num, bufsize, dev_id);
		    }
		    FS_close(sfd);
		    FS_close(dfd);
		    free(bufp);
		    FS_delete(name); /* remove incomplete file */
		    return -1;
		}

		size  -= bufsize;

		/*
		 * Let other processes run between chunks
		 */
		process_suspend();

	    }
	    while (size > 0) {
		cnt = IP_IMAGE_HEADER_SIZE;
		if (FS_read(sfd, bufp, cnt) != cnt) {
		    printf("Error - file read failure\n");
		    free(bufp);
		    FS_close(sfd);
		    FS_close(dfd);
		    FS_delete(name); /* remove incomplete file */
		    return -1;
		}

		cnt = min(cnt,size);
		num = FS_write(dfd, bufp, cnt);
		if (num != cnt) {
		    if (num < 0) {
			msg = error_code2text(num);
			printf("\nWrite file %s error (%s)", name, msg);
		    } else {
			dev_id = get_dev_id_by_file_id(dst_file_id);
			printf("\nWrite %d bytes instead of %d (Device %s is full)",
			       num, cnt, dev_id);
		    }
		    FS_close(sfd);
		    FS_close(dfd);
		    FS_delete(name); /* remove incomplete file */
		    free(bufp);
		    return -1;
		}

		size  -= cnt;

		/*
		 * Let other processes run between chunks
		 */
		process_suspend();
	    }

	    FS_close(dfd);

	    /*
	     *	Keep file type the same.
	     */
	    type = get_file_type(src_file_id);
	    if (type >= 0)
		FS_settype(name, type);
	}


	free(bufp);
	FS_close(sfd);
	return 0;
    }
}

/*
 * Execute the config commands in buffer.
 */
int buf_to_current_config (char *bufp, int size)
{
    parse_configure(bufp, TRUE, RES_CONFIG, CUR_PRIV(stdio->privilege));
    return 0;
}

/*
 * Copy the config commands into config file,
 * and generate the distilled config to nvram.
 */
int buf_to_startup_config (char *bufp, int size, boolean vocal)
{
    char *file_id;
    int flags;
    int cnt;
    int fd;
    const char *msg;
    const char *dev_id;
    int space_left;
    char *private_bufp;
    addrtype adrp;
    int private_size;

    if (vocal)
	printf("\nWarning: distilled config is not generated\n");

    file_id = fetch_config_file_id();
    dev_id = get_dev_id_by_file_id(file_id);
    if (check_device_space(dev_id, &space_left, FALSE) == FALSE)
	return -1;
    if (space_left >=0) {
	if (space_left < size) {
	    printf("\nNot enough space left on device %s", dev_id);
	    return -1;
	}
    }

    /*
     * If the config is in the nvram: device, save the private config
     * before writing the config from the buffer. Restore it afterwords.
     * Note that the file_id is modified by called routines, so it can't be
     * constant.
     */
    if (!strcmp(dev_id, NV_DEV_NAME)) {
	private_bufp =config_file_to_buf(nv_private_fileid,&private_size,&adrp);
    } else {
	private_bufp = NULL;
    }

    flags = O_WRONLY | O_APPEND | O_TRUNC | O_CREAT;
    fd = FS_open(file_id, flags, 0);
    if (fd < 0) {
	msg = error_code2text(fd);
	if (vocal)
	    printf("\nConfig file %s can not be opened (%s)", file_id, msg);
	return -1;
    }

    cnt = FS_write(fd, bufp, size);
    if (cnt != size) {
	if (cnt < 0) {
	    msg = error_code2text(cnt);
	    if (vocal)
		printf("\nWrite to file %s error (%s)", file_id, msg);
	} else {
	    if (vocal) {
		printf("\nWrite % bytes to %s instead of %d",
		    cnt, file_id, size);
	    }
	}
	FS_close(fd);
	FS_delete(file_id);	/* remove the error file */
	return -1;
    }
    FS_close(fd);

    if (private_bufp) {
	cnt = write_buffer_to_file(nv_private_fileid,private_bufp,private_size);
	if (cnt < 0) {
	    if (vocal) {
		printf("\nRestore of private config failed.");
	    }
	}
    }

    /*
     * Write a copy of the config and private config to slave as well.
     */
    reg_invoke_sync_slave_config(FALSE, bufp, size);
    reg_invoke_sync_slave_private_config(FALSE, private_bufp, private_size);
    free(private_bufp);

    return 0;
}

/*
 * Write the buffer to the named file.
 * Return bytes actually written.
 */
int write_buffer_to_file (const char *file_id, char *bufp, int size)
{
    int fd;
    int flags;
    int cnt;
    const char *dev_id;
    const char *msg;
    int space_left;
    int retcode;
    uint dev_type;
    boolean is_private_file_id = FALSE;

    dev_id = get_dev_id_by_file_id(file_id);
    if (check_device_space(dev_id, &space_left, FALSE) == FALSE)
	return -1;
    if (space_left >=0) {
	if (space_left < size) {
	    printf("\nNot enough space left on device %s", dev_id);
	    return -1;
	}
    }
    
    /*
     * Required to write private configuration.
     */
    if (NV_MATCH_PRIVATE_FILE_ID(file_id)) {
    	retcode = FS_ioctl(file_id, IOCTL_SET_PRIVATE_RW, (void *)TRUE);
    	if (retcode) {
	    msg = error_code2text(retcode);
	    printf("\nCan not set attributes for file %s status (%s)", 
								file_id, msg);
  	    return -1;
	}
	is_private_file_id = TRUE;
    }

    flags = O_WRONLY | O_APPEND | O_TRUNC | O_CREAT;
    fd = FS_open(file_id, flags, 0);
    if (fd < 0) {
	if (is_private_file_id) {
	    (void) FS_ioctl(file_id, IOCTL_SET_PRIVATE_RW, (void *)FALSE);
	} else {
	    msg = error_code2text(fd);
	    printf("\nfile %s can not be opened for write (%s)", file_id, msg);
	}
	return -1;
    }

    /*
     * Required for rcp device.
     */
    retcode = FS_ioctl(file_id, IOCTL_GET_DEV_TYPE, (void *)&dev_type);
    if (retcode == FS_OK && (dev_type & IOCTL_DEV_TYPE_RCP))
	FS_ioctl(file_id, IOCTL_SET_FILE_SIZE, (void *)size);

    cnt = FS_write(fd, bufp, size);
    if (cnt < 0) {
	msg = error_code2text(cnt);
	printf("\nWrite file %s error (%s)", file_id, msg);
	FS_close(fd);
	return -1;
    } else if (cnt < size)
	printf("\nWrite %d bytes instead of %d", cnt, size);

    FS_close(fd);
    return cnt;
}

/*
 * Check if the file is possibly a config file.
 */
static boolean is_config_file (const char *file_id)
{
    const char *dev_id;
    stat_t status;

    dev_id = get_dev_id_by_file_id(file_id);
    if (dev_id == NULL)
	return FALSE;

    if (is_nv_device(file_id) == TRUE)
	return TRUE;

    if (FS_stat(file_id, &status) == FS_OK) {
	if (status.type == FILE_TYPE_SYS_CONFIG ||
	    status.type == FILE_TYPE_SYS_UNKNOWN)
	{
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * Check if the file is possibly an image file.
 */
static boolean is_image_file (const char *file_id)
{
    stat_t status;

    if (FS_stat(file_id, &status) == FS_OK) {
	if (status.type == FILE_TYPE_SYS_IMAGE ||
	    status.type == FILE_TYPE_SYS_UNKNOWN)
	{
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * Check if ok to overwrite/delete a file.
 * This provides file protection for image
 * and config file.
 */
boolean file_protection_check (const char *file_id)
{
    char *cfg_file_id;
    char *image_id;
    char *ptr;
    boolean retcode = TRUE;
    boolean inlist = FALSE;
    boolean to_confirm = FALSE;
    char buffer[32];

    ptr = strchr(file_id, ':');
    if (ptr == NULL)
	return retcode;

    if (is_config_file(file_id) == TRUE) {
	/*
	 * This maybe a config file. Confirm user
	 * if it's pointed to by startup config variable
	 * or same as running config file.
	 */
	ptr = &buffer[0];
	memset(ptr, 0, 32);
	cfg_file_id = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
	if (cfg_file_id && strcmp(file_id, cfg_file_id) == 0) {
	    to_confirm = TRUE;
	    strcpy(ptr, "startup");
	}
	if (strcmp(file_id, &config_file_id[0]) == 0) {
	    to_confirm = TRUE;
	    if (*ptr) {
		ptr += strlen(ptr);
		*ptr++ = '/';
	    }
	    strcpy(ptr, "running");
	}
	if (to_confirm) {
	    printf("\nOverwrite %s config file? ", buffer);
	    if (confirm(NULL) == FALSE)
		retcode = FALSE;
	}
    }

    if (is_image_file(file_id) == TRUE) {
	/*
	 * This may be an image file.
	 * Confirm user if it's
	 * the only valid image in boot image list.
	 */
	image_id = reg_invoke_monvar_get(ROMMON_BOOT_VAR);
	while (image_id) {
	    ptr = trunc_image_id_from_list(image_id);
	    if (strcmp(image_id, file_id) == 0) {
		inlist = TRUE;
	    } else {
		if (check_file_exist(image_id) == TRUE)
		    break;
	    }
	    image_id = ptr;
	}
	if (inlist == TRUE && image_id == NULL) {
	    printf("\nOverwrite image file? ");
	    if (confirm(NULL) == FALSE)
		retcode = FALSE;
	}
	/*
	 * Confirm with user if we are deleting rxboot image.
	 */
	image_id = reg_invoke_monvar_get(ROMMON_BOOTLDR_VAR);
	if (image_id) {
	    if (strcmp(image_id, file_id) == 0) {
		printf("\nOverwrite boot loader image? ");
		if (confirm(NULL) == FALSE)
		    retcode = FALSE;
	    }
	}
    } else {
	if (filesys_debug)
	    printf("\nUnknown file type/or can not determined");
    }
    return retcode;
}

/*
 * Find the first file on the given device with
 * given file type or with unknown file type.
 */
static char *find_first_file (const char *dev_id, int type)
{
    char dbuffer[sizeof(dirent_t)];
    dirent_t *dirp;
    int fd;
    char *file_name;
    static char dev_id_buf[FS_FILE_ID_MAX + 1];
    int actual;
    char *ptr;

    if (dev_id == NULL)
	return NULL;

    memset(dev_id_buf, 0, FS_DEV_ID_MAX + 1);
    strcpy(dev_id_buf, dev_id);
    strcat(dev_id_buf, ":");
    strcat(dev_id_buf, ".");

    fd = FS_open(dev_id_buf, O_RDONLY, 0);
    if (fd < 0)
	return NULL;

    file_name = NULL;
    dirp = (dirent_t *)&dbuffer[0];
    for (;;) {
	actual = FS_getdents(fd, (char *)dirp, sizeof(dirent_t));
	if (actual != sizeof(dirent_t))
	    break;

	if (dirp->deleted == 0 && dirp->valid &&
	    (dirp->type == type || dirp->type == FILE_TYPE_SYS_UNKNOWN))
	{
	    file_name = dirp->d_name;
		break;
	}
    }
    FS_close(fd);

    if (file_name) {
	ptr = strchr(dev_id_buf, '.');
	memcpy(ptr, file_name, strlen(file_name));
	return dev_id_buf;
    }
    return NULL;
}

/*
 * Check if this device contains config file or
 * all the valid images in the boot image list.
 */
boolean device_protection_check (const char *dev_id)
{
    char *file_id;
    const char *this_dev_id;
    char *image_id;
    char *next;
    int image_cnt = 0;
    char *sep;
    char buffer[32];
    boolean to_confirm = FALSE;

    if (is_flash_device(dev_id) == FALSE)
	return TRUE;	/* don't check for non-flash device */

    /*
     * Check config file on this device
     */
    next = &buffer[0];
    memset(next, 0, 32);
    file_id = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (file_id && *file_id) {
	this_dev_id = get_dev_id_by_file_id(file_id);
	if (this_dev_id && strcmp(this_dev_id, dev_id) == 0) {
	    to_confirm = TRUE;
	    strcpy(next, "Startup");
	}
    }
    file_id = &config_file_id[0];
    if (file_id && *file_id) {
	this_dev_id = get_dev_id_by_file_id(file_id);
	if (this_dev_id && strcmp(this_dev_id, dev_id) == 0) {
	    to_confirm = TRUE;
	    if (*next) {
		next += strlen(next);
		*next++ = '/';
	    }
	    strcpy(next, "Running");
	}
    }
    if (to_confirm) {
	printf("\n%s config file on this device, proceed? ", buffer);
	if (confirm(NULL) == FALSE)
	    return FALSE;
    }

    file_id = find_first_file(dev_id, FILE_TYPE_SYS_IMAGE);
    if (file_id == NULL)
	return TRUE;		/* this device has no image file */

    /*
     * Confirm with user if this device contains rxboot image.
     */
    image_id = reg_invoke_monvar_get(ROMMON_BOOTLDR_VAR);
    if (image_id) {
	this_dev_id = get_dev_id_by_file_id(image_id);
	if (this_dev_id && strcmp(this_dev_id, dev_id) == 0) {
	    printf("\nBoot loader image on this device, continue? ");
	    if (confirm(NULL) == FALSE)
		return FALSE;
	}
    }

    image_id = reg_invoke_monvar_get(ROMMON_BOOT_VAR);
    if (image_id == NULL || *image_id == 0)
	return TRUE;

    /*
     * Go through the image list and check how many of them
     * are on this device, and how many on other devices.
     * We confirm with the user only if there are images on
     * this device and none are on other devices.
     */
    while (image_id) {
	next = trunc_image_id_from_list(image_id);
	sep = strchr(image_id, ',');
	if (sep)
	    *sep = 0;
	this_dev_id = get_dev_id_by_file_id(image_id);

	if (this_dev_id == NULL && is_flash_device(this_dev_id) == FALSE) {
	    image_id = next;	/* skip non-flash device */
	    continue;
	}

	if (strcmp(this_dev_id, dev_id) == 0) {
	    if (get_file_name_by_file_id(image_id)) {
		if (check_file_exist(image_id) == TRUE)
		    image_cnt++;
	    } else
		image_cnt++;
	} else {
	    if (get_file_name_by_file_id(image_id) == NULL) {
		if (find_first_file(this_dev_id, FILE_TYPE_SYS_IMAGE))
		    return TRUE;
	    } else {
		if (check_file_exist(image_id) == TRUE)
		    return TRUE;
	    }
	}
	image_id = next;	/* move to next device on the list */
    }

    if (image_cnt == 0)
	return TRUE;

    printf("\nAll 'boot system' images are on this device, continue? ");
    if (confirm(NULL) == 0)
	return FALSE;
    return TRUE;
}

/*
 * Check if a give file does exist.
 */
boolean check_file_exist (const char *file_id)
{
    stat_t status;

    if (FS_stat(file_id, &status) == FS_OK) {
	return TRUE;
    } else {
	return FALSE;
    }
}

/*
 * Make image_list a pointer to file_id, return
 * the pointer to possible next file_id in the list.
 */
char *trunc_image_id_from_list (char *image_list)
{
    char *ptr = NULL;
    char *tmp;

    if (image_list && *image_list) {
	ptr = strchr(image_list, ';');
	if (ptr)
	    *ptr = 0;
	tmp = strchr(image_list, ',');
	if (tmp)
	    *tmp = 0;
	if (ptr) {
	    if (*++ptr == 0)
		ptr = NULL;
	}
    }
    return ptr;
}

/*
 * Check if file_name matches name
 * String in file_name after "*" matches all.
 */
boolean match_filename (const char *name, const char *file_name)
{
    char *ptr;
    boolean match;
    int len;

    if (file_name && *file_name) {
	ptr = strchr(file_name, '*');
	if (ptr) {
	    len = ptr - file_name;
	    if (strncmp(name, file_name, len) == 0)
		match = TRUE;
	    else
		match = FALSE;
	} else {
	    if (strcmp(name, file_name) == 0)
		match = TRUE;
	    else
		match = FALSE;
	}
    } else
	match = TRUE;

    return match;
}

/****************************************************************************
 *
 * Command linking routines.
 *
 ****************************************************************************/

/*
 * Link "copy file-id file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_file.h"
LINK_POINT(exec_copy_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy running-config file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_run_conf.h"
LINK_POINT(exec_copy_running_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy startup-config file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_start_conf.h"
LINK_POINT(exec_copy_startup_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy flash file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_flash_conf.h"
LINK_POINT(exec_copy_flash_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy tftp file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_tftp_conf.h"
LINK_POINT(exec_copy_tftp_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy rcp file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_rcp_conf.h"
LINK_POINT(exec_copy_rcp_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "copy mop file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_copy_mop_conf.h"
LINK_POINT(exec_copy_mop_file_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Link "verify file-id" command.
 */
#define	ALTERNATE	NONE
#include "exec_verify_file_cmnds.h"
LINK_POINT(exec_verify_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "show file file-id" commands.
 */
#define	ALTERNATE	NONE
#include "exec_show_file.h"
LINK_POINT(exec_show_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "dir, format, delete, undelete, squeeze ..." commands.
 */
#define	ALTERNATE	NONE
#include "exec_file.h"
LINK_POINT(exec_file_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "boot config" command.
 */
#define	ALTERNATE	NONE
#include "cfg_boot_config.h"
LINK_POINT(cfg_boot_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Link "boot helper" command.
 */
#define	ALTERNATE	NONE
#include "cfg_boot_helper.h"
LINK_POINT(cfg_boot_helper_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for File System debug commands
 */
#define ALTERNATE       NONE
#include "exec_debug_filesys.h"
LINK_POINT(exec_filesys_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Link "show boot" command.
 */
#define	ALTERNATE	NONE
#include "exec_show_boot.h"
LINK_POINT (exec_show_boot_commands, ALTERNATE);
#undef ALTERNATE

static parser_extension_request file_chain_init_table[] = {
    /*
     * These parse chains should get hooked into PARSE_ADD_COPY_CMD,
     * etc, but since the TEST_MULTIPLE_FUNCS() disambiguator only
     * works when it is last on the alternate chain, separate link
     * point exists for these parse chains only.
     */
    { PARSE_ADD_COPYFILE_CMD, &pname(exec_copy_file_commands) },

    { PARSE_ADD_COPY_RUNNING_FILE_CMD,
	  &pname(exec_copy_running_file_commands) },
    { PARSE_ADD_COPY_STARTUP_FILE_CMD,
	  &pname(exec_copy_startup_file_commands) },

    { PARSE_ADD_COPY_FLASH_FILE_CMD,
	  &pname(exec_copy_flash_file_commands) },

    { PARSE_ADD_COPY_TFTP_FILE_CMD,
	  &pname(exec_copy_tftp_file_commands) },
    { PARSE_ADD_COPY_RCP_FILE_CMD,
	  &pname(exec_copy_rcp_file_commands) },
    { PARSE_ADD_COPY_MOP_FILE_CMD,
	  &pname(exec_copy_mop_file_commands) },

    { PARSE_ADD_VERIFY_FILE_CMD, &pname(exec_verify_file_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(exec_show_file_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_file_commands) },

    { PARSE_ADD_CFG_BOOT_CONFIG_CMD, &pname(cfg_boot_config_commands) },
    { PARSE_ADD_CFG_BOOT_CONFIG_CMD, &pname(cfg_boot_helper_commands) },

    { PARSE_ADD_DEBUG_CMD, &pname(exec_filesys_debug_commands) },

    { PARSE_ADD_SHOW_CMD, &pname(exec_show_boot_commands) },
    { PARSE_LIST_END, NULL }
};

static void file_parser_init (void)
{
    if (!system_loading)
	parser_add_command_list(file_chain_init_table, "file");
}

/*
 * Extend file id with default device if needed.
 * Checking for possible buffer overflown.
 */
int get_whole_file_id (const char *file_id, char *ext_file_id, int size)
{
    char *ptr;
    const char *dev_id;

    ptr = ext_file_id;
    if (get_dev_id_by_file_id(file_id) == NULL) {
	/*
	 * Extend the device-less file id only
	 * if they are not "startup-config" and
	 * "running-config".
	 */
	if (strcmp(file_id, STARTUP_CONFIG) != 0 &&
	    strcmp(file_id, RUNNING_CONFIG) != 0)
	{
	    dev_id = get_default_dev_id();
	    /*
	     * We need to insert default device id, make sure
	     * we have at least 1 byte space for file name,
	     * which will be truncated if needed.
	     */
	    if (size <= strlen(dev_id) + 2)
		return -1;
	    memset(ptr, 0, size);
	    strcpy(ptr, dev_id);
	    ptr += strlen(ptr);
	    *ptr++ = ':';
	    size -= strlen(ext_file_id) - 2;
	}
    }
    sstrncpy(ptr, file_id, size);
    return 0;
}

/*
 * Extend file id with default device, and add given file name
 * if needed. check for possibly buffer overflown.
 */
int construct_whole_file_id (const char *file_id, char *ext_file_id, int size,
    const char *file_name)
{
    const char *src_file_name;

    memset(ext_file_id, 0, size);

    if (get_whole_file_id(file_id, ext_file_id, size) < 0)
	return -1;

    if (strchr(ext_file_id, ':') == NULL)
	return -1;

    src_file_name = get_file_name_by_file_id(ext_file_id);
    if (src_file_name)	/* we already have a file name */
	return 0;

    if (is_nv_device(ext_file_id) == TRUE)
	return 0;	/* do not need file name for nvram device */

    /* we need to use the given file name */
    if (strlen(ext_file_id) + strlen(file_name) >= size)
	return -1;

    strcat(ext_file_id, file_name);
    return 0;
}

/***************************************************************************
 *
 * Redirect routines from original flash devices.
 *
 **************************************************************************/

static boolean check_flash_file_name (const char *file_name)
{
    int len;

    len = strlen(file_name);
    if (0 < len && len <= FS_FILE_NAME_MAX)
	return TRUE;
    else
	return FALSE;
}

static boolean rsp_flash_file_system (void)
{
    return TRUE;
}

void rsp_flash_file_parse (char *file_name, parseinfo *csb)
{
    char *bufptr;

    if (csb->nvgen) {
	nv_add(TRUE, "%s %s", "flash", file_name);
	/*
	 * Now add boot command to ROM monitor BOOT variable.
	 */
	bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
	if (bufptr) {
	    bufptr += strlen(bufptr);
	    sprintf(bufptr, "%s,%d;", file_name, 1);
	}
    }
}

/*
 * rsp_flash_first_bootable_file()
 * 	Find the first bootable file on all possible system flash 
 * devices. Note, in particular, that the bootflash will not be 
 * searched for candidate bootable files. This function is called 
 * in the auto-boot path and there we desire to boot a system image. 
 * If there are no system images we rely on the ROM Monitor to boot 
 * the image in bootflash, if necessary.
 */
char *rsp_flash_first_bootable_file (void)
{
    const char *all_dev_id;
    int len;
    char *bufp;
    char *dev_id;
    char *ptr;
    char *file_id = NULL;

    all_dev_id = get_device_names();
    if (all_dev_id == NULL)
	return NULL;

    len = strlen(all_dev_id);
    bufp = malloc(len + 1);
    if (bufp == NULL)
	return NULL;
    memcpy(bufp, all_dev_id, len);

    dev_id = bufp;
    while (*dev_id) {
	ptr = strchr(dev_id, ' ');
	if (ptr) {
	    *ptr = 0;
	    ptr++;
	}
        /*
         * Don't search the bootflash for bootable files.
         */
        if (strstr(dev_id, "bootflash") == NULL)
	    file_id = find_first_file(dev_id, FILE_TYPE_SYS_IMAGE);
	if (file_id) {
	    free(bufp);
	    return file_id;
	}
	if (ptr == NULL)
	    break;
	dev_id = ptr;
    }
    free(bufp);
    return NULL;
}

boolean rsp_file_boot_in_place (const char *file_id, uchar **bufp, ulong *size)
{
    const char *dev_id;
    stat_t status;
    int retcode;

    if (get_file_name_by_file_id(file_id) == NULL) {
	/*
         * No file name given. Try to boot the first bootable file
         * on specified device if device name is given. If not, try
         * to boot the first bootable file in one of the system flash
         * devices.
	 */
	dev_id = get_dev_id_by_file_id(file_id);
        if (dev_id == NULL) { /* No device name specified */
            file_id = rsp_flash_first_bootable_file(); 
        }
        else {  /* Device name was specified */
	    file_id = find_first_file(dev_id, FILE_TYPE_SYS_IMAGE);
        }
	if (file_id == NULL) {
	    *size = 0;
	    return FALSE;
	}
    }

    retcode = FS_stat(file_id, &status);
    if (retcode < 0) {
	printf("\nBoot in place status error");
	*size = 0;
	return FALSE;
    }
    if  ((status.type != FILE_TYPE_SYS_IMAGE &&
	status.type != FILE_TYPE_SYS_UNKNOWN) || status.st_size > *size)
    {
	printf("\nFile type = %x", status.type);
	printf("\nFile size = %d, buff size = %d",
	    status.st_size, *size);
	*size = 0;
	return FALSE;
    }

    setstring(&sysconfg.filename, file_id);
    *size = 1;
    return FALSE;
}

int rsp_flash_file_read (const char *file_name, char *buffer, int size)
{
    int fd;
    int value;

    if (check_flash_file_name(file_name) == FALSE) {
	return -1;
    }

    if (get_interrupt_level() == 0)
        fd = FS_open(file_name, O_RDONLY, 0);
    else
        fd = FS_open(file_name, O_RDONLY | O_VERBOSE_OFF, 0);

    if (fd < 0) {
	printf("\nCan not open file %s", file_name);
	return -1;
    }

    if (buffer == NULL) {
	printf("\nNo buffer is given");
	FS_close(fd);
	return -1;
    }

    value = FS_read(fd, buffer, size);
    if (value < 0) {
	printf("\nFile %s read error (%d)", file_name, value);
	FS_close(fd);
	return -1;
    }

    if (get_interrupt_level() == 0)
	printf("\nRead %d bytes from file %s\n", value, file_name);
    FS_close(fd);
    return value;
}

boolean rsp_flash_file_exists (const char *file_name)
{
    if (check_flash_file_name(file_name) == TRUE)
	return check_file_exist(file_name);
    return FALSE;
}

boolean rsp_flash_block_server (const char *file_name, char *buffer, int size,
    callback_func_t func, void *func_arg)
{
    int fd;
    int total;
    int blocksize;
    int count;
    int value;
    boolean retcode = TRUE;

    if (check_flash_file_name(file_name) == FALSE) {
	return FALSE;
    }

    if (buffer == NULL) {
	return FALSE;
    }

    fd = FS_open(file_name, O_RDONLY | O_VERBOSE_OFF | O_CRC_OFF, 0);
    if (fd < 0) {
	return FALSE;
    }

    total = get_file_size(fd);
    if (total < 0) {
	FS_close(fd);
	return FALSE;
    }

    blocksize = min(TFTP_MAXDATA, size);

    count = 0;
    for (;;) {
	value = FS_read(fd, buffer, blocksize);
	if (value < 0) {
	    FS_close(fd);
	    return FALSE;
	}
	total -= value;
	count += value;
	if (func) {
	    retcode = (*func)((uchar **)&buffer, (long *)&total,value,func_arg);
	    if (retcode == FALSE)
		break;
	}
	if (value < blocksize)	/* last block read */
	    break;
    }

    FS_close(fd);
    return retcode;
}

#if !defined(C7000)

void rsp_flash_file_source (char *file_name, uchar *buffer)
{
    const char *dev_id;

    dev_id = get_dev_id_by_file_id(file_name);
    if (dev_id)
	strcpy(buffer, dev_id);
}


/*
 * Return file length of the flash file.
 */
int flash_file_length (char* name)
{
    int value;
    stat_t status;

    if (check_flash_file_name(name) == TRUE) {
	value = FS_stat(name, &status);
	if (value >= 0)
	    return status.st_size;
    }
    return -1;
}

/*
 * Read flash file into given buffer.
 */
int flash_read_file (char* name, uchar* bufp, ulong* sizep, boolean verbose)
{
    int fd;
    int size;
    int fsize;
    const char *msg;

    /*
     * Make sure buffer is big enough.
     */
    fsize = flash_file_length(name);
    if (fsize < 0) {
	if (verbose) {
	    printf("\nSize of file %s can not be determined", name);
	    flush();
	}
	return 1;
    } else if (fsize > *sizep) {
	if (verbose) {
	    printf("\nBuffer (%d) is not big enough (%d)", *sizep, fsize);
	    flush();
	}
	return 1;
    }
    /*
     * Open the file.
     */
    fd = FS_open(name, O_RDONLY, 0);
    if (fd < 0) {
	if (verbose) {
	    msg = error_code2text(fd);
	    printf("\nFile %s can not be opened (%s)", name, msg);
	    flush();
	}
	return 1;
    }
    size = FS_read(fd, bufp, *sizep);
    if (size < 0) {
	if (verbose) {
	    printf("\nFile %s read error (%d)", name, size);
	    flush();
	}
	FS_close(fd);
	return 1;
    }
    if (size != fsize) {
	if (verbose) {
	    printf("\nRead file %s inconsistent", name);
	    flush();
	}
	FS_close(fd);
	return 1;
    }

    if (verbose) {
	printf("\n[OK - %d/%d bytes]\n", fsize, *sizep);
	flush();
	setstring(&sysconfg.filename, name);
    }

    FS_close(fd);
    return ERROR_OK;
}
#endif

/*
 * Initialize flash commands.
 */
void init_flash_command (void)
{
    char *bufp;

    /*
     * Register all functions related with flash configuration.
     */
    reg_add_config_buffer_to_startup(config_buffer_to_startup,
	"config buffer to startup");
    reg_add_abort_reload(config_file_not_exist, "config file not exist");

    /*
     * Register command redirection functions.
     */
    reg_add_rsp_flash_file_system(rsp_flash_file_system,
	"rsp flash file sys");

    /*
     * Registery function called from death() to clean
     * up device device list when exec process exit.
     */
    reg_add_death(df_dev_clean, "df_dev_clean");

    /*
     * Add flash config file new commands.
     */
    file_parser_init();

    /*
     * Update config file environment variable
     * and bootldr variable.
     */
    bufp = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (bufp && *bufp)
	strcpy(&config_file_id[0], bufp);

    bufp = reg_invoke_monvar_get(ROMMON_BOOTLDR_VAR);
    if (bufp && *bufp)
	strcpy(&rxboot_file_id[0], bufp);

#if defined(C7000)
    fs_init_rp_flash();
#endif

    filesys_debug_init();
}
