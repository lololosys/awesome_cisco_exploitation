/* $Id: platform.h,v 3.4.12.2 1996/06/19 04:43:19 smackie Exp $
 * $Source: /release/112/cvs/Xsys/h/platform.h,v $
 *------------------------------------------------------------------
 * platform.h - Platform specific indirected variables
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform.h,v $
 * Revision 3.4.12.2  1996/06/19  04:43:19  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/05/17  11:16:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.1  1996/04/26  07:52:59  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4  1996/02/13  20:38:33  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.3  1995/12/03  17:56:31  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  09:13:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/15  18:53:40  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  20:36:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Defines
 *
 */

#define ALL_DISABLE        all_disable
#define NETS_DISABLE       nets_disable
#define TTY_DISABLE        tty_disable
#define ALL_ENABLE         all_enable
#define FAMTYPE            famtype
#define MAXLINES           maxlines

/*
 * Externs
 */

extern const uint all_disable;
extern const uint tty_disable;
extern const uint nets_disable;
extern const uint all_enable;
extern const uint famtype;
extern const uint maxlines;


/*
 * Platform API
 *
 * Routines supported by all platforms and used to obtain platform
 * specific variables and call initialization routines.
 *
 * DO NOT ADD TO THIS WITHOUT CONSULTING THE AUTHOR OF THIS FILE.
 *
 */

/*
 * platform_main
 *
 * Support routine called from main() to allow platforms to do basic
 * things like turn on LEDs
 */

extern void platform_main(void);

/*
 * platform_exception_init
 *
 * Support routine called by main() to allow platforms to initialize
 * exception handlers and stacks
 */

extern void platform_exception_init(void);

/*
 * platform_memory_init
 *
 * Support routine called by memory initialization code to enable
 * platforms declare memory regions and create pools
 */

extern void platform_memory_init(void);

/*
 * platform_buffer_init
 *
 * Support routine called by buffer initialization code to tune
 * platform specific buffer parameters
 */

typedef enum platform_buffer_init_type_ {
    PLATFORM_BUFFER_BOOTSTRAP,
    PLATFORM_BUFFER_NORMAL,
    PLATFORM_BUFFER_ROUTING,
} platform_buffer_init_type;

extern void platform_buffer_init(platform_buffer_init_type type);

/*
 * platform_interface_init
 *
 * Support routine used to allow platforms to start miscellaneous
 * functions and features for interfaces
 */

extern void platform_interface_init(void);

/*
 * platform_interface_ordering
 *
 * Support function to aid in determining the lexical ordering
 * of interfaces.
 */
 
extern boolean (*platform_interface_ordering)(hwidbtype *, hwidbtype *);

/*
 * platform_line_init
 *
 * Support routine used to allow platforms to declare console
 * drivers and other async serial interfaces
 */

extern void platform_line_init(void);

/*
 * platform_get_string
 *
 * Support routine used to allow platforms to return commonly used
 * textual strings
 */

typedef enum platform_string_type_ {
    PLATFORM_STRING_NOM_DU_JOUR,
    PLATFORM_STRING_DEFAULT_HOSTNAME,
    PLATFORM_STRING_PROCESSOR,
    PLATFORM_STRING_PROCESSOR_REVISION,
    PLATFORM_STRING_HARDWARE_REVISION,
    PLATFORM_STRING_HARDWARE_SERIAL,
    PLATFORM_STRING_VENDOR,
    PLATFORM_STRING_LAST_RESET,
    PLATFORM_STRING_HARDWARE_REWORK,
    PLATFORM_STRING_BOOTROM_OR_BOOTFLASH,
    PLATFORM_STRING_PCMCIA_CONTROLLER

} platform_string_type;

extern char *platform_get_string (platform_string_type type);

/*
 * platform_get_value
 *
 * Support routine used to allow platforms to return commonly used
 * values
 */

typedef enum platform_value_type_ {
    PLATFORM_VALUE_SERVICE_CONFIG,
    PLATFORM_VALUE_FEATURE_SET,
    PLATFORM_VALUE_HARDWARE_REVISION,
    PLATFORM_VALUE_HARDWARE_SERIAL,
    PLATFORM_VALUE_VENDOR,
    PLATFORM_VALUE_CPU_TYPE,
    PLATFORM_VALUE_FAMILY_TYPE,
    PLATFORM_VALUE_REFRESH_TIME,
    PLATFORM_VALUE_LOG_BUFFER_SIZE,

} platform_value_type;

extern uint platform_get_value (platform_value_type type);

/*
 * platform_verify_config
 *
 * Support routine used to allow platforms to validate
 * hardware/software compatability.
 */

extern void platform_verify_config(void);
