/* $Id: as5200_flash.c,v 1.1.70.2 1996/09/10 03:22:38 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/as5200_flash.c,v $
 *------------------------------------------------------------------
 * as5200_flash.c - Platform specific Flash routines for AS5200
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_flash.c,v $
 * Revision 1.1.70.2  1996/09/10  03:22:38  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/06/16  21:19:26  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:11  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/flash_cmnds.h"
#include "../dev/flash_dvr_les.h"
#include "../les/flash_fs_les.h"
#include "../dev/flash_partition.h"
#include "../src-68-as5200/as5200_flash.h"
#include "../../boot/src-68-c3000/pcmap.h"
#include "../src-68-as5200/as5200_pcmap.h"
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
#include "../../boot/cpu.h"
#include "../../boot/flh.h"
#include "../../boot/emt.h"

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

static dev_info_t *as5200_flash_devices[MAX_DEV_INIT_ENTRIES];


/**************************************************************************
 *               Flash functions for Brasil only
 *************************************************************************/
/*
 * Device dependent pre-open function for device.
 */
static void as5200_flash_open (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we are opening for WRITE or ERASE, see that VPP is turned
     * ON and cache turned OFF.
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_ON);
        if (devcons->dev_cache_fn)
            (*devcons->dev_cache_fn)(dev, CACHE_OFF);
    }
}

/*
 * Device dependent post-close function for device.
 */
static void as5200_flash_close (dev_info_t *dev, ulong flag)
{
    dev_cons_info_t *devcons;
    devcons = dev->devcons;
    if (devcons == NULL)
       return ;
    /*
     * If we had opened for WR or ERASE access, turn VPP OFF
     * and reenable CACHE.
     */
    if (flag & (DEV_FLAGS_RW | DEV_FLAGS_ERASE)) {
        if (devcons->dev_vpp_fn)
            (*devcons->dev_vpp_fn)(VPP_OFF);
        if (devcons->dev_cache_fn)
            (*devcons->dev_cache_fn)(dev, CACHE_ON);
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
static void as5200_flash_access (dev_info_t *dev)
{
    global_dev_gate_closed = TRUE;
}

/*
 *                Flash VPP Control
 * as5200_boot|system_flash_assert_vpp() :
 *     Assert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * as5200_boot|system_flash_deassert_vpp() :
 *     Deassert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * as5200_boot|system_check_vpp() :
 *     Returns TRUE if VPP asserted, FALSE otherwise.
 */
static boolean as5200_system_flash_check_vpp (void)
{
    return ((*(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG) & SYSTEM_FLASH_VPP)
			? TRUE : FALSE );
}
 
static boolean as5200_system_flash_assert_vpp (void)
{
    *(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG)  |= SYSTEM_FLASH_VPP;
    DELAY(VPP_RAMP_UP);
    return (as5200_system_flash_check_vpp());
}
 
static boolean as5200_system_flash_deassert_vpp (void)
{
    /*
     * Be sure to keep the flash in Brasil mode!
     */
    *(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG)&= (~SYSTEM_FLASH_VPP &
						BRASIL_FLASH_VPP_MODE);
    DELAY(VPP_RAMP_DOWN);
    return (as5200_system_flash_check_vpp());
}
 
static boolean as5200_system_flash_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (as5200_system_flash_assert_vpp());
    else       /* Turn Vpp OFF */
        return (as5200_system_flash_deassert_vpp());
}

static boolean as5200_boot_flash_check_vpp (void)
{
    return ((*(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG) & BOOT_FLASH_VPP)
			? TRUE : FALSE );
}
 
static boolean as5200_boot_flash_assert_vpp (void)
{
    *(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG)  |= BOOT_FLASH_VPP;
    DELAY(VPP_RAMP_UP);
    return (as5200_boot_flash_check_vpp());
}
 
static boolean as5200_boot_flash_deassert_vpp (void)
{
    /*
     * Be sure to keep the flash in Brasil mode!
     */
    *(ushort *)(ADRSPC_BRASIL_MB_CTRL2_REG)&= (~BOOT_FLASH_VPP &
						BRASIL_FLASH_VPP_MODE);
    DELAY(VPP_RAMP_DOWN);
    return (as5200_boot_flash_check_vpp());
}
 
static boolean as5200_boot_flash_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (as5200_boot_flash_assert_vpp());
    else       /* Turn Vpp OFF */
        return (as5200_boot_flash_deassert_vpp());
}

/*
 * Brasil system flash sizing
 */
static void as5200_system_flash_size (dev_info_t *dp)
{
    dev_cons_info_t *devcons;
    ulong flash_size;
    register ushort ctrl3, simm_size;
 
    devcons = dp->devcons;
    ctrl3 = *(ushort *)ADRSPC_BRASIL_MB_CTRL3_REG;  /* Read ctrl register */
    flash_size = 0;
    if (!(ctrl3 & FLASH_SIMM1)) {  /* Flash SIMM 1 present */
        simm_size = (ctrl3 >> FLASH_SIMM1_SIZE_SHIFT) & FLASH_SIMM_SIZE_MASK;
        switch (simm_size) {
        case FLASH_ONE_MEG :
            flash_size = ONE_MB;
            break;
        case FLASH_TWO_MEG :
            flash_size = 2 * ONE_MB;
            break;
        case FLASH_FOUR_MEG :
            flash_size = 4 * ONE_MB;
            break;
        case FLASH_EIGHT_MEG :
            flash_size = 8 * ONE_MB;
            break;
        case FLASH_SIXTEEN_MEG :
            flash_size = 16 * ONE_MB;
            break;
        default:
            break;
        }
    }
    devcons->dev_simm_size = flash_size;
    dp->dev_size = flash_size;
    devcons->dev_size = dp->dev_size;
    dp->dev_end  = dp->dev_addr + dp->dev_size;
    devcons->dev_end = dp->dev_end;
    dp->dev_width = BW_32BITS;
    devcons->dev_width = dp->dev_width;
    dp->dev_wmask = BW_32BITS_MASK;
    devcons->dev_wmask = dp->dev_wmask;
}

/*
 * Brasil boot flash sizing
 */
static void as5200_boot_flash_size (dev_info_t *dp)
{
    dev_cons_info_t *devcons;
    ulong flash_size;
    register ushort ctrl3, simm_size;
    register ushort asic_reg;
 
    devcons = dp->devcons;
    ctrl3 = *(ushort *)ADRSPC_BRASIL_MB_CTRL3_REG;  /* Read ctrl register */
    asic_reg = *(ushort *)ADRSPC_ASIC_REG;  /* Read ASIC register */
    flash_size = 0;
    if (!(asic_reg & CANCUN_FLASH_SIMM_1)) {  /* Bootflash SIMM present */
	simm_size = (ctrl3 >> FLASH_SIMM0_SIZE_SHIFT) & FLASH_SIMM_SIZE_MASK;
	switch (simm_size) {
	    case FLASH_ONE_MEG :
		flash_size = ONE_MB;
		break;
	    case FLASH_TWO_MEG :
		flash_size = 2 * ONE_MB;
		break;
	    case FLASH_FOUR_MEG :
		flash_size = 4 * ONE_MB;
		break;
	    case FLASH_EIGHT_MEG :
		flash_size = 8 * ONE_MB;
		break;
	    case FLASH_SIXTEEN_MEG :
		flash_size = 16 * ONE_MB;
		break;
	    default:
		break;
	}
    }
    devcons->dev_simm_size = flash_size;
    dp->dev_size = flash_size;
    devcons->dev_size = dp->dev_size;
    dp->dev_end  = dp->dev_addr + dp->dev_size;
    devcons->dev_end = dp->dev_end;
    dp->dev_width = BW_32BITS;
    devcons->dev_width = dp->dev_width;
    dp->dev_wmask = BW_32BITS_MASK;
    devcons->dev_wmask = dp->dev_wmask;
}

/* data cache control:
 *
 * first time cache off is called:
 *    determine type of cache control:
 *      NONE - xx
 *      OLD - pre set/reset type emt's
 *      NEW - set/reset emt's
 *
 */
static ulong as5200_dcache_off (dev_info_t *dev)
{
    ulong cstatus;

    switch (dev->dev_cache_set) {
      case CACHE_NOT_INIT:
	if ((cstatus = mon_setcacr(CLEAR_DCACHE)) == (ulong)-1) {
	    printf("\n%s%s%s%s%s\n\n",
                   "\n*****************************************************",
		   "\nFlash: System Bootstrap 4.6(0.11) or greater required",
                   "\n       for proper data cache control.",
		   "\n       Please upgrade boot proms.",
                   "\n*****************************************************");
	    mon_disable_dcache();
	    dev->dev_cache_set = CACHE_OLD_CACHE;
	    return(cstatus);
	} else {
	    dev->dev_cache_set = CACHE_NEW_CACHE;  /* new cache controls */
	    return(cstatus);
	}
      case CACHE_NONE:
	return(0);
      case CACHE_OLD_CACHE:
	mon_disable_dcache();
	return(1);
      case CACHE_NEW_CACHE:
	return(mon_setcacr(CLEAR_DCACHE));
      default:
	printf("\n%%ERR: %s cache control error\n", dev->dev_name);
	return(1);
    }
}

static void as5200_dcache_restore (dev_info_t *dev, ulong cstatus)
{

    switch (dev->dev_cache_set) {
      case CACHE_NONE:
	return;
      case CACHE_OLD_CACHE:
	mon_enable_dcache();
	return;
      case CACHE_NEW_CACHE:
	mon_setcacr(cstatus);
	return;
      default:
        printf("\n%%ERR: %s cache control error\n", dev->dev_name);
    }
}

static boolean as5200_flash_cache (dev_info_t *dp, boolean mode)
{

    if (mode == CACHE_OFF)
        dp->dev_cache_sts = as5200_dcache_off(dp);
    else
        as5200_dcache_restore(dp, dp->dev_cache_sts);
    return (TRUE);
}

/* as5200_flash_timer: calibrate the 6 and 10 usec flash timers */

static void as5200_flash_timer (ushort *v6, ushort *v10, boolean print_timer)
{
    *v6  = timer_calibrate(6, print_timer);
    *v10 = timer_calibrate(10, print_timer);
}

static boolean as5200_data_bus_reversed (void)
{
    return (TRUE);  /* data bus reversed on PAN products */
}

/*
 * This updates the entry in the array as5200_flash_devices after dynamic
 * re-partitioning has been done.
 */
static void as5200_replace_flash_devices_entry (char *devnm, void *dev)
{
    int indx;
    dev_info_t *workptr;
    
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++) {
        workptr = as5200_flash_devices[indx];
        if (workptr == NULL) /* Skip empty slots */
            continue;
        if (strcmp(workptr->dev_alias, devnm) == 0)
            break;
    }
    if (indx < MAX_DEV_INIT_ENTRIES)
        as5200_flash_devices[indx] = (dev_info_t *)dev;
}

/*
 * This function determines whether partitioning of flash devices is to be
 * allowed for this platform. For the 5200 platform flash partitioning will
 * always be allowed.
 */
static boolean as5200_allow_flash_partitioning (void)
{
    return (TRUE);
}



/* ------------------- registry functions ------------------------- */

static boolean RFF_recommended_system (void)
{
    switch (cpu_type) {

	/* RFF systems */
	case CPU_CANCUN:
	    return (TRUE);
	/* non RFF systems */
	default:
	    return (FALSE);
    }
}

static char *as5200_default_flash_device (void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &as5200_flash_devices[0];
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
static void as5200_flash_print_hardware(void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &as5200_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        dev_info((*dev)->dev_alias, SHOW_VER_FLASH_SIZE);
        dev++;
        i++;
    }
}

/* Function to get flh version */
static int as5200_flash_flh_version (void)
{
    int flh_ver;

    /*
     * Until Brasil supports the flh this emt call 
     * is merely a boilerplate.
     */
    flh_ver = emt_call(EMT_FLASH_LOAD_HELPER, FLH_GET_VERSION);
    return (++flh_ver);
}

/*
 * aout_image_reloc_test()
 * take a pointer to an a.out header and look to see if this is a
 * relocatable image.  Return TRUE if so else return false
 */

static boolean aout_image_reloc_test(SUN *mptr)
{
    struct special_header *shptr;
    /*
     * we're given a pointer to the a.out header, so increment past
     * the a.out header and look at the potential rel image header
     */
    mptr++;                     /* should increment size of a.out */
                                /* header thereby pointing to rel image hdr */
    shptr = (struct special_header *)mptr;
    if ((shptr->r_dynamic) && (shptr->r_toolversion == SUN_TOOLVER) &&
        (shptr->r_machtype == TYPE_SUN) && (shptr->r_magic == RELOC_MAGIC1)){

        return(TRUE);
    }
    return(FALSE);
}

/*
 * as5200_image_family_check
 * verify the image being downloaded/netbooted is fine for this platform
 * Return value 0 = image correct for this box
 * return value -1 = invalid image.
 */
int as5200_image_family_check(char *buff)
{
    SUN *aoutptr = (SUN *)buff;
    struct special_header *shptr; /* pointer to reloc special header */
 
    /*
     * Do image validation on reloc images.
     */
    shptr = (struct special_header *)(aoutptr+1);
    if (shptr->r_magic == RELOC_MAGIC1) {
	if (shptr->r_platform_family != FAMILY_BRASIL) {
	    return(-1);
	}
    }
    return(0);
}

/*
 * Include file that contains code for validating SUN a.out executables.
 */

#include "../dev/flash_aout.h"

/*
 * Table of Flash devices for Brasil
 */
static const dev_table_t dev_table[] = {
    { 0, FLASH_PRIMARY, "System flash", SF, FLASH_BASE, 0,
      as5200_system_flash_vpp, as5200_system_flash_check_vpp, as5200_flash_cache, 
      as5200_system_flash_size, as5200_flash_open, as5200_flash_close, 
      as5200_flash_access },

    { 0, FLASH_FOR_BOOTSTRAP, "Boot flash", BF, BOOT_FLASH_BASE, 0,
      as5200_boot_flash_vpp, as5200_boot_flash_check_vpp, as5200_flash_cache, 
      as5200_boot_flash_size, as5200_flash_open, as5200_flash_close, 
      as5200_flash_access },
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

    dev = &as5200_flash_devices[0];
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
#define	ALTERNATE	NONE
#include "../dev/exec_copy_flash.h"
LINK_POINT(flash_exec_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for Flash show commands
 */
#define	ALTERNATE	NONE
#include "../dev/exec_show_flash.h"
LINK_POINT(flash_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for verify and erase exec commands. 
 */
#define	ALTERNATE	NONE
#include "../dev/exec_erase_cmnds.h"
LINK_POINT(exec_erase_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "../dev/exec_verify_cmnds.h"
LINK_POINT(exec_verify_commands, ALTERNATE);
#undef	ALTERNATE

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
static void as5200_flash_parser_init (void)
{
    if (!system_loading) {
        /* copy flash ... */
        parser_add_link_point(PARSE_ADD_COPY_FLASH_CMD,
                              "copy flash ...",
                              &pname(exec_copy_flash_extend_here));
 
        /* copy bootflash ... */
        parser_add_link_point(PARSE_ADD_COPY_BFLASH_CMD,
                              "copy bootflash ...",
                              &pname(exec_copy_bflash_extend_here));
    }

    parser_add_command_list(system_flash_chain_table, "system flash");
}

/*
 * Init function for this sub-system.
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
     * Set the flash addressing mode to Brasil mode.
     */
    *(ushort *)ADRSPC_BRASIL_MB_CTRL2_REG = BRASIL_FLASH_VPP_MODE;

    reg_add_flh_version(as5200_flash_flh_version, "FLH version");
    /*
     * Do one-time initializations.
     */
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++)
        as5200_flash_devices[indx] = NULL;
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
            devcons->dev_timer_fn = as5200_flash_timer;
            devcons->dev_data_bus_reversed = as5200_data_bus_reversed;
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
                    as5200_flash_devices[count++] = dev; 

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
        as5200_flash_parser_init();
        reg_add_flash_download_verify(flash_download_verify,
                                      "flash download verification");
        reg_add_flash_boot_verify(flash_boot_verify,
                                  "flash boot verification");
        reg_add_RFF_exec_check(aout_image_RFF_exec_checks, 
                               "RFF execution check");
        reg_add_flash_print_hardware(as5200_flash_print_hardware, 
                                     "print flash hardware");
	reg_add_classify_image(sniff_image_type,
			       "classify image type");
	reg_add_check_rel_image(aout_image_check_rel_image,
				"validate relocatable image");
        reg_add_replace_flash_devices_entry(as5200_replace_flash_devices_entry,
                                "replace flash devices entry");
        reg_add_allow_flash_partitioning(as5200_allow_flash_partitioning,
                                         "as5200_allow_flash_partitioning");
	reg_add_image_family_check(as5200_image_family_check,
				"as5200_image_family_check");
    }
    reg_add_flash_device_default(as5200_default_flash_device, 
                                 "default flash device");
    reg_add_flash_alias_match(flash_alias_match, "flash_alias_match");
}

/*
 *************** This function should actually be moved out from here ***
 */
void test_leds (parseinfo *csb)
{
    ushort control;
    int i;

    control = *(volatile u_short *)ADRSPC_CONTROL;
    for (i = 0; i < 30; i++) {
        /*
         * Turn all LEDs off
         */
        *(volatile u_short *)ADRSPC_CONTROL = control & (~CONTROL_RUNLAMP);
        process_sleep_for(100);         /* Let other processes run again */
        /*
         * Turn all LEDs on
         */
        *(volatile u_short *)ADRSPC_CONTROL = control | CONTROL_RUNLAMP;
        process_sleep_for(100);         /* Let other processes run again */
    }
}

/*
 * Flash hardware subsystem header
 */
#define FLASH_MAJVERSION	1
#define FLASH_MINVERSION	0
#define FLASH_EDITVERSION	1

SUBSYS_HEADER(flash_devices, FLASH_MAJVERSION, FLASH_MINVERSION,
              FLASH_EDITVERSION, flash_init, SUBSYS_CLASS_DRIVER,
              "seq: les_flash_driver, les_flash_file_system",
              NULL);
