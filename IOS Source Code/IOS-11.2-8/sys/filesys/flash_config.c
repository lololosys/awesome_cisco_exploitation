/* $Id: flash_config.c,v 3.4.20.4 1996/07/24 22:05:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/filesys/flash_config.c,v $
 *------------------------------------------------------------------
 * Redirection of configuration to flash
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_config.c,v $
 * Revision 3.4.20.4  1996/07/24  22:05:54  widmer
 * CSCdi60592:  Error: command buffer too small [OK]  Cant save to nvram
 * Branch: California_branch
 * Malloc buffer instead of using stack
 * Improve intuitivness of logic
 *
 * Revision 3.4.20.3  1996/05/21  09:46:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.20.2  1996/05/09  14:12:46  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.20.1.18.1  1996/04/27  06:34:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.20.1.8.2  1996/04/25  20:05:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Incorporate gstovall's comments:
 * - filesys/flash_config.c
 *         - Don't include globs.h, master.h already includes it.
 * - h/interface.h
 *         - Extra "," after IDBTYPE_SYNALC_BIGA.
 * - if/interface.c
 *         - Why did you include network.h?
 *
 * Revision 3.4.20.1.8.1  1996/04/08  01:45:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.20.1  1996/03/18  19:35:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.1  1996/03/25  02:18:42  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.4.2  1996/03/22  05:25:09  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.4.1  1996/03/20  23:45:35  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/31  02:13:51  snyder
 * CSCdi47864:  spellink errors
 *              sould -> should
 *
 * Revision 3.3  1995/12/17  18:26:34  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:07:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  00:49:47  yanke
 * Placeholder file.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdlib.h>		/* for atoi */
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "logger.h"
#include "../dev/monitor1.h"
#include "ttysrv.h"
#include "config.h"
#include "../os/nv.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "filesys.h"
#include "flh_config.h"

#define	DISTILLED_VERSION	"ff.ff"	/* to indicate distill nv config */

/*
 * flash config record and pointer to it.
 */
static flash_config_t flash_config_rec;
static flash_config_t *flashp = &flash_config_rec;

/*
 * Store config file name and boot loader file name.
 */
char config_file_id[MAX_CONFIG_FILE_ID + 1];

/*
 * XXX - to be defined in h/globs.h and os/nv.h
 */
extern void nv_write_raw(boolean, char *, ...);
extern void nv_done_raw(nvtype *ptr);

/*
 * Return true to indicate we have flash config.
 * For systems not registered this function,
 * config is written directly to nvram.
 */
static boolean check_flash_config_enable (void)
{
    return TRUE;
}

/*
 * Check if the config in buffer is distilled version.
 * Return true if yes.
 */
boolean distilled_config (char *bufp)
{
    /*
     * Expect "version" command is the first one.
     */
    bufp++;		/* skip "\n" */
    if (*bufp == '!')
	bufp += 2;	/* skip "!" and "\n" */
    if (strncmp(bufp, "version", strlen("version")) == 0) {
	bufp += strlen("version");
	while (*bufp == ' ')
	    bufp++;
	if (strncmp(bufp, DISTILLED_VERSION, strlen(DISTILLED_VERSION)) == 0)
	    return TRUE;
    }
    return FALSE;
}

/*
 * Returns:
 *  1	if config in nvram is distilled,
 *  0	if nvram config is not distilled.
 * -1	if can not determine.
 */

int distilled_config_nv (void)
{
    nvtype *nvp;
    boolean retcode;

    nvp = nv_getptr_raw();
    if (!nvp) {
	/* non-vol is in use cant go look. */
	return -1;
    }
    if (nv_check_getptr(nvp) == 0) {
	/*
	 * Nvram is invalid.
	 */
	nv_done_raw(nvp);
	return -1;
    }
    retcode = distilled_config(nvp->textbase);
    nv_done_raw(nvp);
    return ((retcode == TRUE) ? 1 : 0);
}

/*
 * Fetech the current configuration version
 * from the no-op command version.
 */
static int get_system_version (char *bufp)
{
    int version;
    int major;
    int minor;
    char *ptr;

    /*
     * Expect "version" command is the first one.
     */
    bufp++;		/* skip "\n" */
    if (*bufp == '!')
	bufp += 2;	/* skip "!" and "\n" */
    if (strncmp(bufp, "version", strlen("version")) == 0) {
	bufp += strlen("version");
	while (*bufp == ' ')
	    bufp++;
	ptr = strchr(bufp, '.');
	if (ptr)
	    *ptr = '\n';
	major = atoi(bufp);
	if (ptr) {
	    ptr++;
	    minor = atoi(ptr);
	    ptr--;
	    *ptr = '.';
	} else
	    minor = 0;
	version = 256*major + minor;
    } else
	version = SYSTEM_VERSION;

    return version;
}

/*
 * This is a registry function called from a particular
 * nvram routine to indicate the type of the subsequent
 * access (read/write/erase). It must be called before
 * nv_getptr() so the latter one knows how to open the nvram.
 */
static void access_flash_config (int flag)
{
    if (flash_config_on == FALSE)
	return;

    if (flag == SET_WRITE_FLASH_CONFIG ||
	flag == SET_READ_FLASH_CONFIG ||
	flag == SET_ERASE_FLASH_CONFIG)
    {
	/*
	 * Passed flags value are ok.
	 */
	if ((flashp->flag & FLASH_CONFIG_IN_USE) == 0) {
	    /*
	     * flash rec is available, init it.
	     */
	    memset(flashp, 0, sizeof(flash_config_t));
	    flashp->flag |= FLASH_CONFIG_IN_USE;
	    flashp->access = flag;
	    flashp->fd = -1;
	}
    }
}

static boolean write_to_slave_config = FALSE;

/*
 * Called from nv_getptr() this routine sets up flash file rec
 * for read and and write according to CONFIG_FILE variable.
 *
 * Return false if nvram to be used for config, or writing flash config
 * Caller continue to prepare nvram for access.
 *
 * Return true if flash to be used for config read, in this case nv_ptr
 * points to nv rec, or NULL if errors. Or Flash to be used
 * for config write with errors, in this case, nv_ptr is NULL.
 * Caller should abort operation and return nv_ptr.
 */
boolean open_flash_config (nvtype **nv_ptr)
{
    char *file_id;
    char dev_id[FS_DEV_ID_MAX + 1];
    boolean retcode = FALSE;
    int fd;
    nvtype *nvp = NULL;
    int flags;
    int size;
    char *bufp;
    int cnt;
    stat_t status;
    int fsize;
    uint dev_type;

    if (flash_config_on == FALSE)
	return FALSE;

    *nv_ptr = NULL;

    /*
     * If we are writing config, write it to slave
     * as well if possible.
     */
    if (flashp->access == SET_WRITE_FLASH_CONFIG) {
	if (reg_invoke_open_slave_config(FALSE) == TRUE)
	    write_to_slave_config = TRUE;
    }

    /*
     * Get config file file if specified,
     * or default to nvram.
     */
    file_id = config_file_id;

    if (*file_id == 0) {
	/*
	 * CONFIG_FILE variable specifies null string, default to nvram.
	 */
	flashp->flag &= ~FLASH_CONFIG_IN_USE;
	return FALSE;
    }

    fileid2devid(file_id, dev_id, FS_DEV_ID_MAX);
    if (*dev_id == 0) {
	/*
	 * CONFIG_FILE variable not specified, default to nvram.
	 */
	flashp->flag &= ~FLASH_CONFIG_IN_USE;
	return TRUE;
    }

    /*
     *	Check config file device, return if nvram or invalid.
     */
    FS_ioctl(dev_id, IOCTL_GET_DEV_TYPE, &dev_type);
    if ((dev_type & IOCTL_DEV_TYPE_FLASH) == 0) {
	/*
	 * It's not flash device, check if it's nvram.
	 */
	if ((dev_type & IOCTL_DEV_TYPE_NV) == 0) {
	    /*
	     * Invalid device is specified. Abort.
	     */
	    flashp->flag &= ~FLASH_CONFIG_IN_USE;
	    return TRUE;
	} else {
	    flashp->flag &= ~FLASH_CONFIG_IN_USE;
	    return FALSE;
	}
    }

    /*
     *	We come here because flash device/file is specified.
     */

    if ((flashp->flag & FLASH_CONFIG_IN_USE) == 0) {
	/*
	 * Flash access mode is not set up.
	 * This means caller didn't call access_flash_config
	 * before calling this. We take no default and
	 * abort on this error situation.
	 */
	return TRUE;
    }

    if (flashp->access == SET_WRITE_FLASH_CONFIG) {
	/*
	 * Open the flash config file for write.
	 * Check if current nvram config is distilled or not.
	 * If not, we need to confirm the user before overwrite
	 * it with distilled config.
	 */
	if (distilled_config_nv() != 1) {
	    if (confirm("\nOverwrite nvram configuration? ") == FALSE) {
		flashp->flag &= ~FLASH_CONFIG_IN_USE;
		return TRUE;
	    }
	}
	/*
	 * Open the specified flash config file.
	 */
	flags = O_WRONLY | O_APPEND | O_TRUNC | O_CREAT;
	fd = FS_open(file_id, flags, 0);
	if (fd >= 0) {
	    flashp->fd = fd;

	    if (flashp->cfg_file_id)
		free(flashp->cfg_file_id);
	    setstring(&flashp->cfg_file_id, file_id);

	    flashp->flag |= FLASH_CONFIG_ON;
	    /*
	     * The flash_config_only flag is
	     * used to indicate if a particular
	     * config is saved to flash only.
	     * It is set by parser action routine.
	     * Make sure it's cleared to start with.
	     */
	    flashp->flag &= ~FLASH_CONFIG_ONLY;
	    /*
	     * Get the nvram location
	     * so we can differentiate
	     * dummy nv_write() later.
	     */
	    flashp->nvp = nv_getptr_raw();
	    if (flashp->nvp == NULL) {
		/*
		 * nvram is busy, abort.
		 */
		flashp->flag &= ~FLASH_CONFIG_IN_USE;
		FS_close(fd);
		return TRUE;
	    }
	    nv_done_raw(flashp->nvp);
	    /*
	     * Set the file type right.
	     */
	    FS_settype(file_id, IOCTL_FILE_TYPE_CONFIG);
	    /*
	     * Let caller continue nvram preparation
	     * for saving of distilled config.
	     */
	    retcode = FALSE;
	} else {
	    /*
	     * Flash file can not be opend for write.
	     * Abort operation.
	     */
	    flashp->flag &= ~FLASH_CONFIG_IN_USE;
	    printf("\nError: Unable to open config file %s", file_id);
	    retcode = TRUE;
	}
    } else if (flashp->access == SET_READ_FLASH_CONFIG) {
	/*
	 * Open flash config for read.
	 * Caller will not continue in any case.
	 */
	retcode = TRUE;
	/*
	 * Open the config file for read.
	 */
	fd = FS_open(file_id, O_RDONLY, 0);
	if (fd >= 0) {
	    flashp->fd = fd;
	    /*
	     * Get buffer and read flash config into it.
	     * Buffer must be as big as nvram for
	     * crc validation, and as big as actual
	     * config file. Propblem: anything beyond
	     * nvsize will not be crc protected.
	     */
	    if (FS_fstat(fd, &status) != FS_OK) {
		printf("\nError get file size");
		return retcode;
	    }
	    fsize = status.st_size;
	    if (nvsize >= fsize + sizeof(nvtype))
		size = nvsize;		/* size of needed buffer */
	    else
		size = fsize + sizeof(nvtype);
	    bufp = malloc(size + 32);
	    if (bufp) {
		flush();

		flashp->buffer = bufp;
		nvp = (nvtype *)bufp;

		bufp += sizeof(nvtype);	/* where config command starts */
		cnt = FS_read(fd, bufp, fsize);
		if (cnt < fsize) {
		    printf("\nRead config cmd incomplete, is %d, should be %d",
			cnt, fsize);
		}
		/*
		 * Set up the fake nvtype rec from what we have
		 * read from flash file.
		 */
		memset(nvp, 0, sizeof(nvtype));
		nvp->magic = NVMAGIC;
		nvp->version = NVVERSION;
		nvp->system_version = get_system_version(bufp);
		nvp->textbase = bufp;
		nvp->textsize = cnt;
		nvp->checksum = 0;
		nvp->checksum = ipcrc((ushort *)nvp, nvsize);
		/*
		 * Give it to caller.
		 */
		*nv_ptr = nvp;
	    } else {
		/*
		 * No more memory available for the buffer.
		 */
		FS_close(fd);
		flashp->flag &= ~FLASH_CONFIG_IN_USE;
		printf("\nError: Out of memory for config buffer");
	    }
	} else {
	    /*
	     * Flash file can not be opened for read.
	     */
	    flashp->flag &= ~FLASH_CONFIG_IN_USE;
	    printf("\nError: Unable to open flash file %s", file_id);
	}
    } else if (flashp->access == SET_ERASE_FLASH_CONFIG) {
	/*
	 * Erase flash config file.
	 * Let caller continue.
	 */
	FS_delete(file_id);
	retcode = FALSE;
    } else {
	/*
	 * Invalid access specified (should not happen).
	 * Abort caller operation.
	 */
	flashp->flag &= ~FLASH_CONFIG_IN_USE;
	retcode = TRUE;
    }
    return retcode;
}

/*
 * This is a registry function called from nv_write()
 * to save config into flash, in case of CONFIG_FILE points
 * to flash file. It also instructs the caller wether to
 * continue to save it to nvram (see below).
 *
 * Return false if the particular config passed over should
 * also be saved into nvram (as part of distilled version).
 * Return true if the passed config is flash only,
 * and not saved to nvram.
 */
static boolean write_flash_config (boolean newline, const char *string,
				   va_list args)
{
    int bytes;
    int size;
    char *buffer;

    /*
     * This is to allow raw nvram access.
     */
    if (flash_config_on == FALSE) {
	return(FALSE);
    }

    /*
     * Put config command[s] into buffer.
     */
    bytes = _doprnt(NULL, NULLTTY, string, args, FALSE);
    buffer = malloc(bytes + 1 + (newline ? 1 : 0));
    if (!buffer) {
	return(FALSE);
    }

    if (newline) {
	buffer[0] = '\n';
	/*
	 * Now write the actual config cmd
	 * to flash, it's ok to be truncated
	 * for whatever reason.
	 */
	bytes = _doprnt(buffer+1, NULL, string, args, FALSE);
	bytes++;	/* include the first newline */
    } else {
	bytes = _doprnt(buffer, NULL, string, args, FALSE);
    }

    /*
     * Write config to slave too. Stop if failed.
     */
    if (write_to_slave_config == TRUE) {
	if (reg_invoke_write_slave_config(buffer, bytes) == FALSE) {
	    write_to_slave_config = FALSE;
	    reg_invoke_close_slave_config();
	}
    }

    /*
     * Do nothing if flash config is not activated.
     */
    if ((flashp->flag & FLASH_CONFIG_IN_USE) == 0) {
	free(buffer);
	return(FALSE);
    }

    if ((flashp->flag & FLASH_CONFIG_ON) == 0) {
	free(buffer);
	return(FALSE);
    }

    /*
     * In nv_common.c nvptr may be set to dummy to cause
     * output discarded, we need to handle it correspondingly
     * here.
     */
#if !defined(C7000)
    if (flashp->nvp != nvptr) {
	free(buffer);
	return(FALSE);
    }
#endif

    /*
     * Need to save config to flash file, whose
     * file id is saved in flashp->fd.
     */
    if (flashp->fd < 0) {
	free(buffer);
	return(FALSE);
    }

    /*
     * Write the config buffer to flash file.
     */
    size = FS_write(flashp->fd, buffer, bytes);
    if (size < bytes) {
	printf("\nWarning: config save incomplete, is %d, should be %d",
		size, bytes);
    }
    free(buffer);

    /*
     * This flag is set by parser action function.
     * We return true if set to indicate no
     * nvram writing for this config cmd.
     */
    if (flashp->flag & FLASH_CONFIG_ONLY) {
	return(TRUE);
    }
    return(FALSE);
}

/*
 * Called from nv_done() this routine close flash config file,
 * free all resources, and make flash rec available for next time.
 */
boolean close_flash_config (void)
{
    if (flash_config_on == FALSE)
	return FALSE;

    /*
     * If we have successfully written config to
     * slave, close it now.
     */
    if (write_to_slave_config == TRUE) {
	write_to_slave_config = FALSE;
	reg_invoke_close_slave_config();
    }

    /*
     * Close the file, free all resource
     * and set flash rec to be available.
     */
    if (flashp->flag & FLASH_CONFIG_IN_USE) {
	if (flashp->fd >= 0) {
	    FS_close(flashp->fd);
	    flashp->fd = -1;
	    if (flashp->access == SET_WRITE_FLASH_CONFIG) {
		/*
		 * Set to config type in case file is newly created.
		 */
		FS_settype(flashp->cfg_file_id, IOCTL_FILE_TYPE_CONFIG);
	    }
	}
	if (flashp->buffer) {
	    free(flashp->buffer);
	    flashp->buffer = NULL;
	}
	flashp->flag &= ~FLASH_CONFIG_IN_USE;
	if (flashp->access == SET_READ_FLASH_CONFIG)
	    return TRUE;
    }
    return FALSE;
}

/*
 * Write distilled verstion ff.ff into nvram
 * if we are generating distilled version into it.
 */
static void generate_nv_distilled_version (parseinfo *csb)
{
    if (flashp->flag & FLASH_CONFIG_ON) {
	/*
	 * Disable flash config tempararily to force
	 * write to nvram, then restore original setting.
	 */
	nv_write_raw(TRUE, "%s %s", csb->nv_command, DISTILLED_VERSION);
    }
}

static void clear_flash_config (parseinfo *csb)
{
    flashp->flag &= ~FLASH_CONFIG_ONLY;
}

transition pname(flash_only_point) =
    {NULL, NULL, (trans_func) clear_flash_config, NULL};

/*
 * Set or clear flash only config.
 */
static void proc_flash_config (parseinfo *csb, uint priv_flag)
{
    if (priv_flag & PRIV_DISTILLED) {
	/*
	 * Set flash config only flag.
	 * Push terminate node onto parser stack.
	 */
	flashp->flag |= FLASH_CONFIG_ONLY;
	push_node(csb, &pname(flash_only_point));
    }
}

/*
 * Initialization of the flash configuration.
 */
void flash_config_init (void)
{
    char *bufp;

    memset(flashp, 0, sizeof(flash_config_t));

    /*
     * Update config file environment variable
     * and bootldr variable.
     */
    bufp = reg_invoke_monvar_get(ROMMON_CONFIG_VAR);
    if (bufp && *bufp)
	strcpy(config_file_id, bufp);

    /*
     *	Register all functions related with flash configuration.
     */
    reg_add_flash_config_enabled(check_flash_config_enable,
	"check flash config enable");

    reg_add_access_flash_config(access_flash_config, "access_flash_config");
    reg_add_write_flash_config(write_flash_config,
	"write_flash_config");

    reg_add_proc_flash_config(proc_flash_config, "proc_flash_config");

    reg_add_generate_nv_distilled_version(generate_nv_distilled_version,
	"generate_nv_distilled_version");
}
