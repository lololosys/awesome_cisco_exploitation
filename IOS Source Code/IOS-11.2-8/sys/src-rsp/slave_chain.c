/* $Id: slave_chain.c,v 3.12.6.4 1996/08/28 13:14:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/slave_chain.c,v $
 *------------------------------------------------------------------
 * slave_chain.c - parser command support for managing the slave
 *
 * August 1995, David Bath
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: slave_chain.c,v $
 * Revision 3.12.6.4  1996/08/28  13:14:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.6.3  1996/06/17  23:37:27  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.12.6.2  1996/05/21  10:04:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.12.6.1  1996/03/18  22:02:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.3  1996/03/16  07:36:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.12.2  1996/03/07  10:45:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.12.1  1996/02/20  18:02:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/06  08:30:15  dbath
 * CSCdi50824:  Should stop user from specifying slave image on slave
 * device
 * Inhibit the user from specifying slave devices as a source for
 * the slave image.  We can't get to slave devices while
 * downloading the slave.
 *
 * Revision 3.11  1996/03/05  01:25:14  dbath
 * CSCdi47229:  HSA slave reload and slave <image> commands need to work
 * Allow non-bundled slave images to be used in the slave.
 *
 * Revision 3.10  1996/02/22  14:38:33  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/01/23  04:56:17  dbath
 * CSCdi44891:  slave sync conf command takes hours when HSA slave is
 * crashed
 * Don't even think about opening up the slave's config file, if the
 * slave isn't running.  Simplify the existing conditional tests
 * while in the process.
 *
 * Revision 3.8  1995/12/18  07:09:15  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.7  1995/12/17  18:38:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/14  07:42:20  dbath
 * CSCdi45563:  Slave filesystem timestamps are wrong.
 * Ship the current time over to the slave every minute.
 *
 * Revision 3.5  1995/12/11  04:09:24  dbath
 * CSCdi45341:  Cant get crash stack on HSA systems.
 * Add client and server code to gather slave crash stack details,
 * to the master/slave exchange mechanism.
 *
 * Revision 3.4  1995/12/05  07:06:34  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.3  1995/11/22  03:43:54  dbath
 * CSCdi43201:  Unecessary errmsgs need to be collapsed into existing
 * errmsg
 * Rationalise error messages as promised in the doco review.
 *
 * Revision 3.2  1995/11/17  18:49:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/07  16:54:09  dbath
 * Placeholders for new development in VIP_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "sys_registry.h"
#include "../os/clock.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../hes/ucode.h"
#include "../hes/dbus.h"
#include "../hes/if_fci.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_spec.h"
#include "../hes/slot.h"
#include "rsp_chasint.h"
#include "rsp_init.h"
#include "rsp_if.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "../filesys/filesys.h"
#include "../filesys/flash_command.h"
#include "../filesys/remote_filesys.h"
#include "../filesys/flh_config.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "ms_xchng_private.h"
#include "ms_xchng.h"

extern pid_t nv_write_pid; /* PID of the process who last set nv_writeflag */

static int lowest_dual_slot;
static int highest_dual_slot;
static int current_default_slave;
static int boottime_default_slave;



/*
 * Slave config commands 
 */
#define ALTERNATE NONE
#include        "cfg_slave.h"
LINK_POINT      (slave_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Slave sync commands (exec)
 */
#define	ALTERNATE	NONE
#include "exec_slave_sync.h"
LINK_POINT(exec_slave_sync_commands, ALTERNATE);
#undef ALTERNATE

static parser_extension_request slave_parse_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(slave_config_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_slave_sync_commands) },
    { PARSE_LIST_END, NULL},
};


/*
 * Pass the supplied argument to the local ROM monitor via an EMT call
 * If 'interactive' is true, then errors should be reported via printfs
 * as we're running in the context of a user config command (bit of
 * a mute point in this case, since we don't generate any errors).
 * Returns true if successful, otherwise FALSE, but it's always successful.
 */
static boolean set_self_rom_ms (uint m_or_s, boolean interactive)
{
    mon_set_ms(m_or_s);
    return TRUE;
}

/*
 * Return the current ms setting from our own ROM register.
 * Returns true if successful, false if not.  In the case of
 * true, the result can be found in the passed parameter.
 * This routine is somewhat trivial, and always returns successs,
 * however, it's sister routine, get_peer_rom_ms() does take full
 * advantage of the interface.
 */
static boolean get_self_rom_ms (uint *result)
{
    *result = mon_get_ms();
    return TRUE;
}

/*
 * Pass the supplied argument to the peer ROM monitor via an rpc call.
 * If 'interactive' is true, then errors should be reported via printfs
 * as we're running in the context of a user config command.
 * Returns true if successful, otherwise FALSE.
 */
static boolean set_peer_rom_ms (uint m_or_s, boolean interactive)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *req;

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	if (interactive) printf("\n%% Cannot communicate setting to slave");
	errmsg(&msgsym(IPC, RSP), "master noticed slave is not RUNNING", "");
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	if (interactive) printf("\n%% Cannot communicate setting to slave");
	errmsg(&msgsym(IPC, RSP), "master could not locate slave port", "");
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size, and stash
     * the provided master/slave setting in it.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_SETMS_REQ);
    if (!req_msg) {
	if (interactive) printf("\n%% Cannot communicate setting to slave");
	errmsg(&msgsym(IPC, RSP), 
	       "master could not allocate SET request msg", "");
	return FALSE;
    }
    req = req_msg->data;
    req->ms_setting = m_or_s;

    /*
     * Launch the RPC.  Nothing useful comes back in the response,
     * but we need to grab it and free it as part of the rpc protocol.
     */
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	if (interactive) 
	    printf("\n%% Cannot communicate default slave setting to slave");
	errmsg(&msgsym(IPC, RSP), "master detected rpc failure",
	       ipc_decode_error(ec));
	return FALSE;
    }
    ipc_return_message(rsp_msg);
    return TRUE;
}

/*
 * Return the current ms setting from the slave's ROM register.
 * Returns true if successful, false if not.  In the case of
 * true, the result can be found in the passed parameter. Can
 * also be used as a benign way of determining slave health.
 */
static boolean get_peer_rom_ms (uint *result)
{
    static ipc_port_id slaverom_port;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    slaverom_t *rsp;

    /*
     * Firstly, do a quick sanity check to see if we even think the
     * slave is running.  If we know it's not, it's better to shortcut
     * this now, then to wait for rpc timeouts.
     */
    if (rsp_slave_state() != SLAVE_RUNNING) {
	return FALSE;
    }


    /*
     * Next,  find the slave's message port if we haven't already.
     */
    if (!slaverom_port)
	slaverom_port = ipc_locate_port(SLAVEROM_PORT_NAME);

    if (!slaverom_port) {
	return FALSE;
    }

    /*
     * Next, allocate a message of the correct type and size.
     */
    req_msg = ipc_get_message(sizeof(slaverom_t), slaverom_port, 
			      SLAVEROM_GETMS_REQ);
    if (!req_msg) {
	return FALSE;
    }

    /*
     * Launch the probe.
     */
    rsp_msg = ipc_send_rpc(req_msg, &ec);
    if ((ec != IPC_OK) || (!rsp_msg)) {
	return FALSE;
    }

    rsp = rsp_msg->data;
    *result = rsp->ms_setting;

    ipc_return_message(rsp_msg);
    return TRUE;
}

/*
 * sanitise_ms_settings()
 *
 * Called after the slave signs in.  We take this opportunity to check
 * the both the slave's ROM ms setting, and our own, against what we 
 * think they should be. If either is in error, we'll fix it now.
 */
void sanitise_ms_settings (void)
{
    uint slave_setting;
    uint self_setting;

    if (!get_peer_rom_ms(&slave_setting) || !get_self_rom_ms(&self_setting)) {
	/*
	 * Hmmm... slave isn't running anymore, so we can't do much
	 * fixing here.  Just silently forget about it.
	 */
	return;
    }

    if (get_rsp_peer_slot() == boottime_default_slave) {
	if (slave_setting != ROM_MS_SLAVE) {
	    set_peer_rom_ms(ROM_MS_SLAVE, FALSE);
	    errmsg(&msgsym(MSFIX, RSP), "slave", "slave");
	}
	if (self_setting != ROM_MS_MASTER) {
	    set_self_rom_ms(ROM_MS_MASTER, FALSE);
	    errmsg(&msgsym(MSFIX, RSP), "master", "master");
	}

    } else {
	if (slave_setting != ROM_MS_MASTER) {
	    set_peer_rom_ms(ROM_MS_MASTER, FALSE);
	    errmsg(&msgsym(MSFIX, RSP), "slave", "master");
	}
	if (self_setting != ROM_MS_SLAVE) {
	    set_self_rom_ms(ROM_MS_SLAVE, FALSE);
	    errmsg(&msgsym(MSFIX, RSP), "master", "slave");
	}
    }
}


/*
 * Given the currently configured default slave, and my slot number, go ahead 
 * and make the necessary calls to set up the ROM monitor variables.
 * If 'interactive' is TRUE, then error conditions should generate
 * printfs, since we're running in the context of a user command.
 * The peer setting is most likely to fail, so we'll see if it works
 * before we go ahead and set our own.  If one fails, better not to 
 * touch the other, so that we end up in a slightly more consistent state.
 * If we do succeed, then it's time to update our boottime_default_slave
 * variable to reflect the new setting.
 */
static void set_roms_ms (boolean interactive)
{
    /*
     * If the current setting is no different from the boottime
     * setting, don't do anything.  This avoids annoying
     * "slave is dead" messages when nothing has changed anyway.
     * XXX Does this work in the 'config mem' case?
     */
    if (current_default_slave == boottime_default_slave)
	return;

    if (get_rsp_self_slot() == current_default_slave) {
	if (set_peer_rom_ms(ROM_MS_MASTER, interactive)) {
	    set_self_rom_ms(ROM_MS_SLAVE, interactive);
	    boottime_default_slave = current_default_slave;
	}
    } else {
	if (set_peer_rom_ms(ROM_MS_SLAVE, interactive)) {
	    set_self_rom_ms(ROM_MS_MASTER, interactive);
	    boottime_default_slave = current_default_slave;
	}
    }
}


/*
 * Handle user requests for tinkering with the default slave.
 * Remember, the default, default slave is the right hand slot,
 * i.e. highest_dual_slot.  Also note, this config command has
 * no effect until 'write mem' time, when it is sent down to the
 * ROM monitor.  For now, we just capture what they've requested.
 */
void rsp_slave_slot_cmd (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write((current_default_slave != highest_dual_slot),
		 "%s %d", csb->nv_command, current_default_slave);
    } else {
	uint dont_care;

	/*
	 * If they're typing this command interactively, then now is
	 * a good time to check the health of the slave.  Even though
	 * we don't send the information to the slave until 'write mem'
	 * time, by checking now, we can whinge while it's fresh on their
	 * minds (and ignore their command).
	 */
	if ((csb->resolvemethod == RES_MANUAL) && 
	    !get_peer_rom_ms(&dont_care)) {

	    printf("\n%% Cannot communicate setting to slave");
	    return;                                  /* Don't act on it */
	}

	/*
	 * Go ahead and interpret the new setting.
	 */
	if (csb->sense) {
	    current_default_slave = GETOBJ(int,1);
	} else {
	    current_default_slave = highest_dual_slot;
	}

	/*
	 * If this is not being typed in interactively, then update our
	 * boottime setting records.   XXX 'config mem' case?
	 */
	if (csb->resolvemethod != RES_MANUAL) {
	    boottime_default_slave = current_default_slave;
	}
    }
}

/*
 * slave_parser_init ()
 *
 * Chain the 'slave' commands into the the main config parse tree.
 */
void slave_parser_init (void)
{
    /*
     * Firstly, determine what the slave slot ranges are for this chassis.
     * This is to facilitate the "slave default-slot ?" config command.
     * Can be <2,3> on a 7507, and <6,7> on a 7513.
     */
    if (get_rsp_self_slot() < get_rsp_peer_slot()) {
	lowest_dual_slot = get_rsp_self_slot();
	highest_dual_slot = get_rsp_peer_slot();
    } else {
	lowest_dual_slot = get_rsp_peer_slot();
	highest_dual_slot = get_rsp_self_slot();
    }
    /*
     * Next, initialise our local idea of the current default
     * slave, and boottime_default_slave.
     * The default, default slave, is the right hand slot.
     */
    boottime_default_slave = current_default_slave = highest_dual_slot;

    /*
     * Add in our routine to communicate the settings down to the ROM
     * monitor at 'write mem' time.
     */
    reg_add_master_slave_set(set_roms_ms, "set_roms_ms");

    /*
     * Finally, chain in the slave management parse tree.
     */
    parser_add_command_list(slave_parse_init_table, "slave management");
}

/*-------------------------------------------------------------------------
 *
 * Routines to control slave image and reload
 *
 *------------------------------------------------------------------------*/
static boolean non_bundled_slave_requested = FALSE;
static char *requested_filename;

/*
 * Generate any config commands for slave image control.  If the
 * user has requested an image other than the bundle, we need to not
 * only generate the image command, but then we must also generate a
 * slave reload to ensure it gets used on the next boot.
 */
void rsp_slave_image_nvgen (parseinfo *csb)
{
    nv_write(non_bundled_slave_requested, "%s flash %s",
	     csb->nv_command, requested_filename);
    nv_write(non_bundled_slave_requested, "slave reload");
}

/*
 * Specify slave image source
 */
void rsp_slave_image_cmd (parseinfo *csb)
{
    char *filename = GETOBJ(string,1);
    uint dev_type;
    int retcode;

    /*
     * Take a local note of what the user has requested, so that we'll be
     * able to nvgen it later.
     */
    if (csb->sense) {
	switch (csb->which) {
	case UCODE_SYSTEM:
	    non_bundled_slave_requested = FALSE;
	    break;
	case UCODE_FLASH:
	    /*
	     * First, check that they've specified a fully qualified
	     * device/filename, and make sure that the device is a
	     * local one.  We can't download the slave with an image
	     * that lives on the slave's devices.  If any of these
	     * conditions are not met, bail early.
	     */
	    retcode = FS_ioctl(filename, IOCTL_GET_DEV_TYPE, &dev_type);
	    if (retcode < 0) {
		printf("\n%% Unrecognised, or unspecified device name");
		return;
	    }
	    if (!(dev_type & IOCTL_DEV_TYPE_LOCAL)) {
		printf("\n%% Device must be local, slave devices not allowed");
		return;
	    }

	    non_bundled_slave_requested = TRUE;
	    setstring(&requested_filename, filename);
	    break;
	}
    } else {
        non_bundled_slave_requested = FALSE;
    }

    /*
     * Pass all the info over to ucode.c where all the real work happens.
     * XXX RSP4 alert, either call routine for both entries, or
     * introduce the concept of a "family" of controller types.
     */
    set_ucode_source(csb->which, UCODE_RSP2, csb->sense, filename);
}

/*
 * Reload the slave processor.  We never nvgen anything, we leave that
 * up to rsp_slave_image_nvgen().  If they attempt to remove the 
 * "slave reload" command, make them remove the "slave image flash"
 * command instead.
 */

void rsp_slave_reload_cmd (parseinfo *csb)
{
    if (!csb->nvgen) {
	if (csb->sense) {
	    /*
	     * If there's a slave installed, trick the oir code
	     * into thinking it was just inserted, and let it do
	     * it's stuff.
	     */
	    if (rsp_slave_state() != SLAVE_UNPLUGGED) {
		uchar slave_slot = get_rsp_peer_slot();
		slots[slave_slot].ucode_loaded = FALSE;
		ucode_loader_wakeup(0);
	    } else {
		printf("\n%% No slave installed");
	    }
	} else if (non_bundled_slave_requested) {
	    printf("\n%% Remove 'slave image flash' command.");
	}
    }
}

/*-------------------------------------------------------------------------
 *
 * Routines to sync slave rsp config with master rsp.
 *
 *------------------------------------------------------------------------*/
static boolean slave_config_inuse = FALSE;
static int slave_config_fd = -1;
static int slave_private_config_fd = -1;
static boolean slave_auto_sync_mode = TRUE;

/*
 * Turn on/off slave auto sync mode.
 */
void rsp_slave_auto_sync_cmd (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(!slave_auto_sync_mode, "no %s", csb->nv_command);
    } else {
	slave_auto_sync_mode = csb->set_to_default || csb->sense;
    }
}

static boolean slave_config_busy (void)
{
    if (slave_config_inuse == TRUE) {
	printf("\n%% Slave config is being used\n");
	return TRUE;
    }
    return FALSE;
}

static boolean slave_config_valid (void)
{
    if (slave_config_fd < 0) {
	printf("\n%% Slave config is not opened\n");
	return FALSE;
    }
    return TRUE;
}

static boolean slave_private_config_valid (void)
{
    if (slave_private_config_fd < 0) {
	printf("\n%% Slave private config is not opened\n");
	return FALSE;
    }
    return TRUE;
}


#define	SLAVE_SYNC_CONFIG_MSG	"\n%% Slave configuration not updated\n"

/*
 * open_slave_config_common is used to open both the slave config and the 
 * slave private config, depending on the private_config argument.
 */
boolean open_slave_config_common (boolean force, boolean private_config)
{
    char *file_id;
    char *str;
    int len;
    int flags;
    slave_state_t state;
    int private_config_ioctl;
    pid_t current_pid; 

    /*
     * Do not bother if there is no slave
     * in the chassis.
     */
    state = rsp_slave_state();
    if (state == SLAVE_UNPLUGGED)
	return FALSE;

    if (slave_config_busy() == TRUE)
	return FALSE;

    /*
     * CSCdj16985: Before we proceed, check if we are the
     * process who was going to write and who set the  nv_writeflag.
     * There is a protection violation in flashp->access and we should
     * not have got called called from open_flash_config in the first place.
     */
    process_get_pid(&current_pid);
    if (current_pid != nv_write_pid)
	return FALSE;
    

    /* 
     * If the user doesn't want us to do this, or, the slave isn't running, 
     * give them a warning to remind them, and exit.
     */
    if ((!force && !slave_auto_sync_mode) || (state != SLAVE_RUNNING)) {
	printf(SLAVE_SYNC_CONFIG_MSG);
	return FALSE;
    }

    /*
     * Whenever we write to the slave's config file, we demand that it
     * be in the same location as the current master's.  Hence we can
     * simply use config_file_id[] to determine where to write.
     */
    len = strlen(slave_prefix);
    file_id = malloc(MAX_BOOTVAR_LEN + len + 1);
    if (file_id == NULL)
	return FALSE;

    /*
     * Copy "slave" prefix and append normal file id
     * after it.
     */
    strcpy(file_id, slave_prefix);
    str = file_id + len;

    if (private_config) {
    	strcpy(str, "nvram:private");
    } else {
	if (null(config_file_id)) {
	    /*
	     * Use the default config: nvram.
	     */
	    strcpy(str, "nvram:");
	} else {
	    strcpy(str, config_file_id);
	}
    }

    flags = O_WRONLY | O_APPEND | O_TRUNC | O_CREAT;
    if (private_config) {
    	private_config_ioctl = 
			FS_ioctl(file_id, IOCTL_SET_PRIVATE_RW, (void *)TRUE);
        if (private_config_ioctl < 0) {
	    printf("\n%% Slave private config ioctl failed (%d)\n", 
	           private_config_ioctl);
	    free(file_id);
	    return FALSE;
    	}
	slave_private_config_fd = FS_open(file_id, flags, 0);
    	if (slave_private_config_fd < 0) {
	    printf("\n%% Slave private config open failed (%d)\n", 
		    slave_private_config_fd);
	    free(file_id);
	    return FALSE;
    	}
        slave_config_inuse = TRUE;
    } else {
	slave_config_fd = FS_open(file_id, flags, 0);
	if (slave_config_fd < 0) {
	    printf("\n%% Slave config open failed (%d)\n", slave_config_fd);
	    free(file_id);
	    return FALSE;
	}
        slave_config_inuse = TRUE;
    }
    free(file_id);
    return TRUE;
}

boolean open_slave_config (boolean force)
{
    return open_slave_config_common(force, FALSE);
}

boolean open_slave_private_config (boolean force)
{
    return open_slave_config_common(force, TRUE);
}

boolean close_slave_config (void)
{
    int retcode;

    if (slave_config_valid() == TRUE) {
	retcode = FS_close(slave_config_fd);
	slave_config_fd = -1;
	slave_config_inuse = FALSE;
	if (retcode < 0) {
	    printf("\n%% Slave config close failed (%d)\n", retcode);
	    return FALSE;
	}
	return TRUE;
    }
    return FALSE;
}

boolean close_slave_private_config (void)
{
    int retcode;

    if (slave_private_config_valid() == TRUE) {
	retcode = FS_close(slave_private_config_fd);
	slave_private_config_fd = -1;
	slave_config_inuse = FALSE;
	if (retcode < 0) {
	    printf("\n%% Slave private config close failed (%d)\n", retcode);
	    return FALSE;
	}
	return TRUE;
    }
    return FALSE;
}

boolean write_slave_config (char *buf, int size)
{
    int retcode;

    if (slave_config_valid() == TRUE) {
	retcode = FS_write(slave_config_fd, buf, size);
	if (retcode != size) {
	    if (retcode < 0) {
		printf("\n%% Slave config write error (%d)\n", retcode);
	    } else {
		printf("\n%% Slave config write incomplete (%d)\n", retcode);
	    }
	    close_slave_config();
	    return FALSE;
	}
	return TRUE;
    }
    return FALSE;
}

boolean write_slave_private_config (char *buf, int size)
{
    int retcode;

    if (slave_private_config_valid() == TRUE) {
	retcode = FS_write(slave_private_config_fd, buf, size);
	if (retcode != size) {
	    if (retcode < 0) {
		printf("\n%% Slave private config write error (%d)\n", retcode);
	    } else {
		printf("\n%% Slave private config write incomplete (%d)\n", 
			retcode);
	    }
	    close_slave_private_config();
	    return FALSE;
	}
	return TRUE;
    }
    return FALSE;
}

void sync_slave_config (boolean force, char *buf, int size)
{
    if (open_slave_config(force) == TRUE) {
	write_slave_config(buf, size);
	close_slave_config();
    }
}

void sync_slave_private_config (boolean force, char *buf, int size)
{
    if (open_slave_private_config(force) == TRUE) {
	write_slave_private_config(buf, size);
	close_slave_private_config();
    }
}

void slave_sync_command (parseinfo *csb)
{
    char *bufp;
    int size;

    bufp = startup_config_to_buf(&size);
    if (bufp == NULL)
	return;
    sync_slave_config(TRUE, bufp, size);
    free(bufp);
    bufp = startup_private_config_to_buf(&size);
    if (bufp) {
	sync_slave_private_config(TRUE, bufp, size);
	free(bufp);
    }
    manual_set_slave_monvars(TRUE);
}

boolean get_slave_auto_sync_mode (void)
{
    return slave_auto_sync_mode;
}
