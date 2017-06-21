/* $Id: c3000_flash.c,v 3.6.10.4 1996/09/10 03:22:41 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/c3000_flash.c,v $
 *------------------------------------------------------------------
 * c3000_flash.c - Platform specific Flash routines (for 3000 and under).
 *
 * July 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c3000_flash.c,v $
 * Revision 3.6.10.4  1996/09/10  03:22:41  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  10:03:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/04/15  21:23:01  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/03/18  21:59:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/16  07:34:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.1  1996/03/09  05:12:12  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6  1996/02/22  14:38:15  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/19  17:28:42  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.4  1996/01/06  03:23:00  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/17  18:37:51  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:43:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/24  19:17:00  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.5  1995/07/04  01:57:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.4  1995/06/28  09:30:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/26  20:30:02  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.2  1995/06/09  13:19:04  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:54:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "../../boot/cpu.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "../dev/flash_cmnds.h"
#include "../dev/flash_dvr_les.h"
#include "../les/flash_fs_les.h"
#include "../dev/flash_partition.h"
#include "../src-68-c3000/c3000_flash.h"
#include "../../boot/src-68-c3000/pcmap.h"
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
#include "../les/flh.h"

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

static dev_info_t *c3000_flash_devices[MAX_DEV_INIT_ENTRIES];


/**************************************************************************
 *               Flash functions for c3000 only
 *************************************************************************/
/*
 * Device dependent pre-open function for device.
 */
static void c3000_flash_open (dev_info_t *dev, ulong flag)
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
static void c3000_flash_close (dev_info_t *dev, ulong flag)
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
static void c3000_flash_access (dev_info_t *dev)
{
    global_dev_gate_closed = TRUE;
}

/*
 *                Flash VPP Control
 * c3000_flash_assert_vpp() :
 *     Assert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * c3000_flash_deassert_vpp()
 *     Deassert VPP and wait for the voltage to stabilize.
 *     Returns 1 if VPP ON, else 0.
 *
 * check_vpp
 *     Returns TRUE if VPP asserted, FALSE otherwise.
 */
static boolean c3000_flash_check_vpp (void)
{
    return ((*(ushort *)(ADRSPC_CONTROL2) & CONTROL2_VPPEN) ? TRUE : FALSE );
}

static boolean c3000_flash_assert_vpp (void)
{
    *(ushort *)(ADRSPC_CONTROL2)  |= CONTROL2_VPPEN;
    DELAY(VPP_RAMP_UP);
    return (c3000_flash_check_vpp());
}

static boolean c3000_flash_deassert_vpp (void)
{
    *(ushort *)(ADRSPC_CONTROL2)&= ~CONTROL2_VPPEN;
    DELAY(VPP_RAMP_DOWN);
    return (c3000_flash_check_vpp());
}

static boolean c3000_flash_vpp (boolean mode)
{
    if (mode)  /* Turn Vpp ON */
        return (c3000_flash_assert_vpp());
    else       /* Turn Vpp OFF */
        return (c3000_flash_deassert_vpp());
}

/* Flash sizing :
 *    Note the odd cases:
 *    merlot:
 *       ROM Monitor sets the size bits to 2 meg -
 *       the only size currently supported by merlot.
 *    moosehead:
 *       hardware sets size bits from info on the simms
 *       bug: no simm is reported as 16 megs.
 */
static void cancun_main_flash_size (dev_info_t *dp)
{
    dev_cons_info_t *devcons;
    ulong flash_size;
    register ushort asic_reg, simm_size;

    devcons = dp->devcons;
    asic_reg = *(ushort *)ADRSPC_ASIC_REG;  /* Read cancun ASIC register */
    flash_size = 0;
    if (!(asic_reg & CANCUN_FLASH_SIMM_1)) {  /* Flash SIMM present */
        simm_size = (asic_reg & CANCUN_FLASH_SIMM_SIZE) >> 9;
        switch (simm_size) {
        case 0 :
            flash_size = ONE_MB;
            break;
        case 1 :
            flash_size = 2 * ONE_MB;
            break;
        case 2 :
            flash_size = 4 * ONE_MB;
            break;
        case 3 :
            flash_size = 8 * ONE_MB;
            break;
        default:
            break;
        }
	devcons->dev_simm_size = flash_size;
        if (!(asic_reg &  CANCUN_FLASH_SIMM_2))
            flash_size += flash_size;
    }
    dp->dev_size = flash_size;
    devcons->dev_size = dp->dev_size;
    dp->dev_end  = dp->dev_addr + dp->dev_size;
    devcons->dev_end = dp->dev_end;
    dp->dev_width = BW_32BITS;
    devcons->dev_width = dp->dev_width;
    dp->dev_wmask = BW_32BITS_MASK;
    devcons->dev_wmask = dp->dev_wmask;
}

#ifdef CANCUN_BOOT_FLASH
static void cancun_boot_flash_size (dev_info_t *dp)
{
    dev_cons_info_t *devcons;
    ulong flash_size;
    register ushort asic_reg, scr2_reg, boot_size;

    devcons = dp->devcons;
    scr2_reg = *(ushort *)ADRSPC_CONTROL2;  /* Read control 2 register */
    asic_reg = *(ushort *)ADRSPC_ASIC_REG;  /* Read cancun ASIC register */
    if (asic_reg & CANCUN_PCMCIA_PRESENT) /* PCMCIA actually NOT present */
        dp->dev_addr = ADRSPC_PROM; /* Adjust start addr. PCMCIA card absent */
    flash_size = 0;
    boot_size = (scr2_reg & CONTROL2_BROMSIZE_MASK) >> CONTROL2_BROMSIZE_SHIFT;
    switch (boot_size) {
    case 0 :
        flash_size = 2 * ONE_MB;
        break;
    case 1 :
        flash_size = ONE_MB;
        break;
    case 2 :
        flash_size = ONE_MB / 2;
        break;
    case 3 :
        flash_size = 4 * ONE_MB;
        break;
    default:
        break;
    }
    devcons->dev_simm_size = flash_size;
    dp->dev_size = flash_size;
    devcons->dev_size = dp->dev_size;
    dp->dev_end  = dp->dev_addr + dp->dev_size;
    devcons->dev_end = dp->dev_end;
    dp->dev_width = ((asic_reg & CANCUN_BOOT_WIDTH) ? BW_16BITS : BW_8BITS);
    devcons->dev_width = dp->dev_width;
    dp->dev_wmask = ((asic_reg & CANCUN_BOOT_WIDTH) ?
                      BW_16BITS_MASK : BW_8BITS_MASK);
    devcons->dev_wmask = dp->dev_wmask;
}

static void c3000_bootflash_size (dev_info_t *dp)
{
    if (cpu_type == CPU_CANCUN)
        cancun_boot_flash_size(dp);
}

#endif  /*  CANCUN_BOOT_FLASH */

static void c3000_flash_size (dev_info_t *dp)
{
    dev_cons_info_t *devcons;
    uchar    *addr;
    leveltype status;
    uint      i;

    devcons = dp->devcons;
    /*
     * For Cancun, size differently. Use the size bits in the
     * ASIC register.
     */
    if (cpu_type == CPU_CANCUN) {
        cancun_main_flash_size(dp);
        return;
    }
    /*
     * touch flash at FLASH_BASE for an existence check
     * sizing code goes nicely here when needed .
     * if 16MB reported (wraparound), set size to 0
     */

    addr = (uchar *)dp->dev_addr;
    status = raise_interrupt_level(ALL_DISABLE);
    for (i = 0; i < 16; i++, addr += ONE_MB)
        if (! touch_device(addr))
            break;
    reset_interrupt_level(status);

    /* moosehead hardware glitch - no simm
     * note that this prevents using 16 mbytes on other
     * platforms
     */

    if (i == 16)
        addr = (uchar *)dp->dev_addr;

    dp->dev_size = (ulong)addr - dp->dev_addr;
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
static ulong c3000_dcache_off (dev_info_t *dev)
{
    ulong cstatus;

    switch (dev->dev_cache_set) {
      case CACHE_NOT_INIT:
	if ((cstatus = mon_setcacr(CLEAR_DCACHE)) == MON_UNIMPL) {
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

static void c3000_dcache_restore (dev_info_t *dev, ulong cstatus)
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

static boolean c3000_flash_cache (dev_info_t *dp, boolean mode)
{

    if (mode == CACHE_OFF)
        dp->dev_cache_sts = c3000_dcache_off(dp);
    else
        c3000_dcache_restore(dp, dp->dev_cache_sts);
    return (TRUE);
}

/* c3000_flash_timer: calibrate the 6 and 10 usec flash timers */

static void c3000_flash_timer (ushort *v6, ushort *v10, boolean print_timer)
{
    *v6  = timer_calibrate(6, print_timer);
    *v10 = timer_calibrate(10, print_timer);
}

static boolean c3000_data_bus_reversed (void)
{
    return (TRUE);  /* data bus reversed on PAN products */
}

/*
 * This updates the entry in the array c3000_flash_devices after dynamic
 * re-partitioning has been done.
 */
static void c3000_replace_flash_devices_entry (char *devnm, void *dev)
{
    int indx;
    dev_info_t *workptr;
    
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++) {
        workptr = c3000_flash_devices[indx];
        if (workptr == NULL) /* Skip empty slots */
            continue;
        if (strcmp(workptr->dev_alias, devnm) == 0)
            break;
    }
    if (indx < MAX_DEV_INIT_ENTRIES)
        c3000_flash_devices[indx] = (dev_info_t *)dev;
}

/* 
 * This function determines whether partitioning of flash devicces is to be
 * allowed for this platform. For the 2500 platform flash partitioning will
 * be allowed only if the boot rom version is recent enough to support dual
 * flash banks.
 */
static boolean c3000_allow_flash_partitioning (void)
{
    int boot_rom_version = -1;
    
    boot_rom_version = reg_invoke_flh_version();
    if (boot_rom_version < FLASH_BASIC_FLH_SUPPORT) {
        /* Boot roms do not support dual flash banks */
        return (FALSE);
    }

    return (TRUE);
}



/* ------------------- registry functions ------------------------- */

static boolean RFF_recommended_system (void)
{
    switch (cpu_type) {

    /* known RFF systems */
    case CPU_CANCUN:
    case CPU_BASS:
    case CPU_IGS_BRUT:
        return (TRUE);

    /*
     * Other low-end systems such as cristal, merlot, moosehead, IGS ..
     */
    default:
        return (FALSE);
    }
}

static char *c3000_default_flash_device (void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &c3000_flash_devices[0];
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
static void c3000_flash_print_hardware(void)
{
    dev_info_t **dev;
    int i = 0;

    dev = &c3000_flash_devices[0];
    while ((i < MAX_DEV_INIT_ENTRIES) && (*dev != NULL)) {
        dev_info((*dev)->dev_alias, SHOW_VER_FLASH_SIZE);
        dev++;
        i++;
    }
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
 * Include file that contains code for validating SUN a.out executables.
 */

#include "../dev/flash_aout.h"

/*
 * Table of Flash devices for 3000/2000 (IGS) products.
 */
static const dev_table_t dev_table[] = {
    { 0, FLASH_PRIMARY, "System flash", SF, FLASH_BASE, 0,
      c3000_flash_vpp, c3000_flash_check_vpp, c3000_flash_cache, 
      c3000_flash_size, c3000_flash_open, c3000_flash_close, 
      c3000_flash_access },

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

    dev = &c3000_flash_devices[0];
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
static void c3000_flash_parser_init (void)
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
     * Do one-time initializations.
     */
    for (indx = 0; indx < MAX_DEV_INIT_ENTRIES; indx++)
        c3000_flash_devices[indx] = NULL;
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
            devcons->dev_timer_fn = c3000_flash_timer;
            devcons->dev_data_bus_reversed = c3000_data_bus_reversed;
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
                    c3000_flash_devices[count++] = dev; 

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
        c3000_flash_parser_init();
        reg_add_flash_download_verify(flash_download_verify,
                                      "flash download verification");
        reg_add_flash_boot_verify(flash_boot_verify,
                                  "flash boot verification");
        reg_add_RFF_exec_check(aout_image_RFF_exec_checks, 
                               "RFF execution check");
        reg_add_flash_print_hardware(c3000_flash_print_hardware, 
                                     "print flash hardware");
	reg_add_classify_image(sniff_image_type,
			       "classify image type");
	reg_add_check_rel_image(aout_image_check_rel_image,
				"validate relocatable image");
        reg_add_replace_flash_devices_entry(c3000_replace_flash_devices_entry,
                                "replace flash devices entry");
        reg_add_allow_flash_partitioning(c3000_allow_flash_partitioning,
                                "c3000_allow_flash_partitioning");
    }
    reg_add_flash_device_default(c3000_default_flash_device, 
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
