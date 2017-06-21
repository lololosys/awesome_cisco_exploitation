/* $Id: xx_flash.c,v 3.5.10.3 1996/09/10 03:22:43 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx_flash.c,v $
 *------------------------------------------------------------------
 * xx_flash.c - Platform specific Flash routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_flash.c,v $
 * Revision 3.5.10.3  1996/09/10  03:22:43  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/05/21  10:03:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.10.1  1996/04/15  21:23:04  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/22  14:38:19  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.4  1996/01/06  03:23:02  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/17  18:37:56  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:43:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/04  01:57:28  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/28  09:30:53  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/26  20:30:06  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.1  1995/06/07  22:55:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
 *               Flash functions for XX only
 *************************************************************************/

#include "master.h"
#include "../ui/common_strings.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/flash_cmnds.h"
#include "../dev/flash_dvr_les.h"
#include "../les/flash_fs_les.h"
#include "../dev/flash_partition.h"
#include "../src-68-c4000/xx_flash.h"
#include "../../boot/src-68-c4000/pcmap.h"
#include "../os/timer_generic.h"
#include "../dev/monitor1.h"
#include "../os/boot.h"
#include "config.h"
#include "address.h"
#include "file.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "../os/reloc.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"


/*
 * Define flash device aliases
 */
#define SF "flash"
#define BF "bootflash"

static dev_info_t *xx_flash_devices[MAX_DEV_INIT_ENTRIES];

/**************************************************************************
 *               Flash functions for XX only
 *************************************************************************/

/*
 * Device dependent pre-open function for device.
 */
static void xx_flash_open (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we are opening for WRITE or ERASE, see that VPP is turned
     * ON. 
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_ON);
    }
}

/*
 * Device dependent post-close function for device.
 */
static void xx_flash_close (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we had opened for WR or ERASE access, turn VPP OFF.
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_OFF);
    }
}

/*
 * Global device access control gate. Used to block write access
 * to all other devices (controlled by this code) during write
 * access to this device.
 * Eg., we do not want boot flash writes during system flash writes
 * to ensure that both devices are not left erased or half-written
 * on account of system or power failure.
 */
static void xx_flash_access (dev_info_t *dev)
{
    global_dev_gate_closed = TRUE;
}

/*
 *                Flash VPP Control
 * xx_flash_assert_vpp() :
 *     Assert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * flash_deassert_vpp()
 *     Deassert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * check_vpp
 *     Returns TRUE if VPP asserted, FALSE otherwise.
 */

static boolean xx_flash_check_vpp (void)
{
    return((*(ulong *)(ADRSPC_CONTROL) & CONTROL_FLASH_VPP) ? TRUE : FALSE );
}

static boolean xx_flash_assert_vpp (void)
{
    *(ulong *)(ADRSPC_CONTROL) |=  CONTROL_FLASH_VPP;
    DELAY(VPP_RAMP_UP);
    return (xx_flash_check_vpp());
}

static boolean xx_flash_deassert_vpp (void)
{
    *(ulong *)(ADRSPC_CONTROL) &= ~CONTROL_FLASH_VPP;
    DELAY(VPP_RAMP_DOWN);
    return (xx_flash_check_vpp());
}

static boolean xx_flash_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (xx_flash_assert_vpp());
    else       /* Turn Vpp OFF */
        return (xx_flash_deassert_vpp());
}

static void xx_flash_size (dev_info_t *dp)
{
    uchar    *addr;
    leveltype status;
    uint      i;
    dev_cons_info_t *devcons;

    devcons = dp->devcons;
    /* 
     * touch flash at FLASH_BASE for an existence check 
     * sizing code goes nicely here when needed 
     * and sets fg.fg_size
     * if 16MB reported (wraparound), set size to 0
     */
    
    addr = (uchar *)dp->dev_addr;
    status = raise_interrupt_level(ALL_DISABLE);
    for (i = 0; i < 16; i++, addr += ONE_MB) 
        if (! touch_device(addr))
            break;
    reset_interrupt_level(status);
    
    dp->dev_size = (ulong)addr - dp->dev_addr;
    devcons->dev_size = dp->dev_size;
    dp->dev_end  = dp->dev_addr + dp->dev_size;
    devcons->dev_end = dp->dev_end;
    dp->dev_width = BW_32BITS;
    devcons->dev_width = dp->dev_width;
    dp->dev_wmask = BW_32BITS_MASK;
    devcons->dev_wmask = dp->dev_wmask;
}

static void xx_flash_timer (ushort *v6, ushort *v10, boolean print_timer)
{
    *v6  = timer_calibrate(6, print_timer);
    *v10 = timer_calibrate(10, print_timer);
}

static boolean xx_data_bus_reversed (void)
{
    return (TRUE);  /* data bus reversed on XX products */
}
/*
 * Dummy function.
 */
static boolean xx_flash_cache (dev_info_t *dp, boolean mode)
{

    dp->dev_cache_set = CACHE_NONE;
    dp->dev_cache_sts = 0;
    return (TRUE);
}

/*
 * This updates the entry in the array xx_flash_devices after dynamic
 * re-partitioning has been done.
 */
static void xx_replace_flash_devices_entry (char *devnm, void *dev)
{
    int indx;
    dev_info_t *workptr;

    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++) {
        workptr = xx_flash_devices[indx];
        if (workptr == NULL) /* Skip empty slots */
            continue;
        if (strcmp(workptr->dev_alias, devnm) == 0)
            break;
    }
    if (indx <MAX_DEV_INIT_ENTRIES)
        xx_flash_devices[indx] = (dev_info_t *)dev;
}

/* This function determines whether partitioning of flash devices is to be 
 * allowed for this platform. For the 4000, partitioning will always 
 * be allowed.
 */
static boolean xx_allow_flash_partitioning (void)
{
    return (TRUE);
}


/* ------------------- registry functions ------------------------- */

static boolean RFF_recommended_system (void)
{
    /* 4000 is not a RFF system */
    return (FALSE);
}

static char *xx_default_flash_device (void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &xx_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        if ((*dev)->dev_type == FLASH_PRIMARY)
            return ((*dev)->dev_alias);
        dev++;
        i++;
    }
    return (NULL);
}

/*
 * Function called by "show version" to display one-liner about flash
 * found on the system.
 */
static void xx_flash_print_hardware(void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &xx_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        dev_info((*dev)->dev_alias, SHOW_VER_FLASH_SIZE);
        dev++;
        i++;
    }
}

static boolean aout_image_reloc_test(SUN *mptr)
{
    /*
     * take a pointer to an a.out header and look to see if 
     * this is a relocatable image.
     * The 4000 product does not support relocatable images yet
     * igs-x-l images should not be relocated
     */
    return(FALSE);
}

/*
 * Include file that contains code for validating SUN a.out executables.
 */

#include "../dev/flash_aout.h"

/*
 * Table of flash devices on XX.
 */
static const dev_table_t dev_table[] = {
    { 0, FLASH_PRIMARY, "System flash", SF, FLASH_BASE, 0,
      xx_flash_vpp, xx_flash_check_vpp, xx_flash_cache,  
      xx_flash_size, xx_flash_open, xx_flash_close, 
      xx_flash_access },

    { -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

/*********************************************************************
 *                  Parser stuff for system flash commands
 ********************************************************************/
static boolean flash_has_multiple_partitions(char *dev_name)
{
    devstat_t devinfo;

    dev_stat(dev_name, &devinfo); 
    if (devinfo.dev_part_cnt > 1)
	return (TRUE);
    else 
	return (FALSE);
}
static boolean flash_alias_match (char *alias)
{
    dev_info_t **dev;
    int i = 0;

    dev = &xx_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        if (strcmp(alias, (*dev)->dev_alias) == 0) /* matched */
            return (TRUE);
        dev++;
        i++;
    }
    return (FALSE);
}

/*
 * Parse chains for Flash copy commands
 */
#define ALTERNATE       NONE
#include "../dev/exec_copy_flash.h"
LINK_POINT(flash_exec_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for Flash show commands
 */
#define ALTERNATE       NONE
#include "../dev/exec_show_flash.h"
LINK_POINT(flash_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for verify and erase exec commands.
 */
#define ALTERNATE       NONE
#include "../dev/exec_erase_cmnds.h"
LINK_POINT(exec_erase_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "../dev/exec_verify_cmnds.h"
LINK_POINT(exec_verify_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE       NONE
#include "../dev/cfg_flash_partition.h"
LINK_POINT(flash_partition_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for Flash
 */
static parser_extension_request system_flash_chain_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(flash_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(flash_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_erase_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_verify_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(flash_partition_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * flash_parser_init - Initialize Flash parser support
 */
static void xx_flash_parser_init (void)
{
    if (!system_loading) {
	/* copy flash ... */
	parser_add_link_point(PARSE_ADD_COPY_FLASH_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_extend_here));
    }

    parser_add_command_list(system_flash_chain_table, "system flash");
}

/*
 * Init function for this subsystem.
 */
void flash_init (subsystype *subsys)
{
    const dev_table_t *dev_tbl_ptr;
    dev_info_t *dev, *workptr;
    dev_cons_info_t *devcons;
    int count = 0, indx, i;
    fs_bounds_t fsbounds[FLASH_MAX_PART_CNT];
    int part_bounds[FLASH_MAX_PART_CNT+2];
    leveltype level;

    /*
     * Do one-time initializations.
     */
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++)
        xx_flash_devices[indx] = NULL;
    dev_io_init();  /* dev_init_table and dev_open_table */
    file_io_init(); /* file_open_table */

    level = set_interrupt_level(ALL_ENABLE);
    for (dev_tbl_ptr = &dev_table[0];
         dev_tbl_ptr->dev_unit != -1;
         dev_tbl_ptr++) {
        /* Initialize the fsbounds array */
        for (indx = 0; indx < FLASH_MAX_PART_CNT; indx++) {
            fsbounds[indx].start_bank = -1;
            fsbounds[indx].end_bank   = -1;
            for (i = 0; i < FLASH_FILE_CNT; i++)
                fsbounds[indx].exception_bank[i] = -1;
        }
        /*
         * Create data structure for device. Get back pointer.
         * Assume sub-device number of 1 since we don't support
         * sub-devices yet.
         */
        if ((dev = dev_create(dev_tbl_ptr)) != NULL) {
            /*
             * device creation successful.
             */
            devcons = dev->devcons;
            dev->dev_flags |= DEV_FLAGS_VERBOSE;
            dev->dev_cache_set = CACHE_NOT_INIT;
            devcons->dev_timer_fn = xx_flash_timer;
            devcons->dev_data_bus_reversed = xx_data_bus_reversed;
            devcons->dev_wr_prot   = !(*devcons->dev_vpp_fn)(VPP_ON);
				      (*devcons->dev_vpp_fn)(VPP_OFF);

	    /*
             * See if specified device is actually present.
             */
            if (devcons->dev_size_fn)
                (*devcons->dev_size_fn)(dev);
            if (dev->dev_size != 0) { /* Device sized and present. */
		regiontype *flash_region;

		/*
		 * Add a region for this flash memory
		 */
		flash_region = region_create(NULL, dev->dev_alias,
					     (void *)dev->dev_addr,
					     dev->dev_size,
					     REGION_CLASS_FLASH,
					     REGION_FLAGS_DEFAULT);
		region_set_media(flash_region, REGION_MEDIA_READONLY);

                /*
                 * We sized device. Now go and do device specific
                 * initialization to identify chips and hence the
                 * erase-write routines to use.
                 */
                reg_invoke_flash_device_driver_init(DEVICE_LES_FLASH, (void *)dev);

                if (dev->dev_state > DEV_STATE_INIT) {
                    /*
                     * Device can be read. Sniff file systems on device.
                     */
                    reg_invoke_file_system_sniff(DEVICE_LES_FLASH,
                                     dev->dev_alias, (void *)fsbounds);

                    /*
                     * Figure out how to partition the device.
                     */
                    devcons->dev_part_cnt = decide_partitions(dev,
                                                       fsbounds, part_bounds);

                    /* Now clone dev_info_t structures for all partitions */
                    dev_clone(dev, part_bounds);

                    /* Read the filesystems on all partitions */
                    workptr = dev;
                    for(indx = 1; workptr && (indx <= devcons->dev_part_cnt); 
                                                                      indx++) {
                        reg_invoke_flash_device_file_system(DEVICE_LES_FLASH,
                                                 workptr->dev_sub_alias, TRUE);
                            workptr = workptr->next_partition;
                    }

                    /* save dev info pointer */
                    xx_flash_devices[count++] = dev;

                    /* record initialization time for device */
                    GET_TIMESTAMP(devcons->dev_init_time);

                    /* clear verbose bit set earlier */
                    dev->dev_flags &= (~DEV_FLAGS_VERBOSE);
                    continue;
                }
            }
            /* problem with device. Undo everything */
            dev_uncreate(dev, DEV_UNCRT_TOTAL);
        }
    }  /* end for loop */
    reset_interrupt_level(level);

    if (count > 0) {  /* atleast one flash device found and initialized */
        xx_flash_parser_init();
        reg_add_flash_download_verify(flash_download_verify,
                                      "flash download verification");
        reg_add_flash_boot_verify(flash_boot_verify,
                                  "flash boot verification");
        reg_add_RFF_exec_check(aout_image_RFF_exec_checks,
                               "RFF execution check");
        reg_add_flash_print_hardware(xx_flash_print_hardware,
                                     "print flash hardware");
	reg_add_classify_image(sniff_image_type,
			       "classify image type");
	reg_add_check_rel_image(aout_image_check_rel_image,
				"validate relocatable image");
	reg_add_replace_flash_devices_entry(xx_replace_flash_devices_entry,
                                "replace flash devices entry");
        reg_add_allow_flash_partitioning(xx_allow_flash_partitioning,
                                "xx_allow_flash_partitioning");
    }
    reg_add_flash_device_default(xx_default_flash_device,
                                 "default flash device");
    reg_add_flash_alias_match(flash_alias_match, "flash_alias_match");
}

/*
 * Flash hardware subsystem header
 */
#define FLASH_MAJVERSION        1
#define FLASH_MINVERSION        0
#define FLASH_EDITVERSION       1

SUBSYS_HEADER(flash_devices, FLASH_MAJVERSION, FLASH_MINVERSION,
              FLASH_EDITVERSION, flash_init, SUBSYS_CLASS_DRIVER,
              "seq: les_flash_driver, les_flash_file_system",
              NULL);

