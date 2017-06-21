/* $Id: command.h,v 3.2.62.2 1996/04/15 15:00:31 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/command.h,v $
 *------------------------------------------------------------------
 * command.h  -- Command Support Prototypes
 *  
 * February 1991, David Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: command.h,v $
 * Revision 3.2.62.2  1996/04/15  15:00:31  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.2.62.1  1996/03/18  22:28:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/14  01:35:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2  1995/11/17  17:47:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:10:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

/*
 * Locking a configuration
 *
 * This is to prevent overlapping write (configuring) and reading
 * (write mem/net/term)
 *
 * At the moment we make no distinction between read and write locks
 * That would only be necessary if we need to allow overlapping
 * read access.  For now it's OK if a write term has to wait for
 * another one to complete
 */

#define CONFIG_LOCK_SECS	10
#define CONFIG_LOCK_TIMEOUT	(CONFIG_LOCK_SECS * ONESEC)

typedef enum config_locktype_ {
    READ_LOCK,
    WRITE_LOCK,
} config_locktype;

typedef struct config_lock_t_ {
    watched_semaphore *semaphore;
    config_locktype read_write;
} config_lock_t;

extern config_lock_t *lock_config(config_locktype read_write);
extern void unlock_config(void);

/*
 * command1.c
 */
extern boolean exit_command(boolean);
extern boolean validlogicalname(char *, tt_soc *, conntype *);
extern void host_banner(void);
extern void lognamerr(int);
extern void logout_banner(tt_soc *);
extern void show_systat(parseinfo *);
extern void escape_command(parseinfo *);

/*
 * command2.c
 */
extern void enable_command(parseinfo *);
extern boolean askpassword(tt_soc *, char *, uint);
extern boolean askstring(tt_soc *, char *, int, char *, int, int, boolean);
extern boolean ping_common(long *, long *, long, long, long *);
extern char *user_on_stdio_str(void);
extern void notify_config(char *);
extern boolean ping_get_sweep(ulong *, ulong *, ulong *);
extern void sendmessage(int);
extern void terminate_line(tt_soc *);
extern void zap_line(parseinfo *);
extern void nvram_config_dirty_init(void);
extern void write_memory(parseinfo *);

extern uchar *write_terminal_nv_fallback;

/*
 * exec.c
 */
extern forktype exec(tt_soc *);

/*
 * trace.c
 */
extern boolean trace_common(char *buff, ulong *, ushort *, ushort *,
			    ushort *, int);

#endif __COMMAND_H__
