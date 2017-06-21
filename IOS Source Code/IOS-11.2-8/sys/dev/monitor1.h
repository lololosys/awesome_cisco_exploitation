/* $Id: monitor1.h,v 3.5.12.1 1996/03/21 22:29:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dev/monitor1.h,v $
 *------------------------------------------------------------------
 * Header file for monitor functions
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: monitor1.h,v $
 * Revision 3.5.12.1  1996/03/21  22:29:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.2.3  1996/03/18  22:46:16  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.2  1996/02/16  02:22:10  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/12/11  18:32:21  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.5  1996/02/17  05:47:40  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checsum
 * validation to fail
 *
 * Revision 3.4  1995/12/05  07:02:55  dbath
 * CSCdi43178:  rommon boot info not kept in sync on slave
 * Ship the three boot related environment variables, and the config
 * register setting, across to the slave whenever autosync is enabled,
 * or whenever the user manually syncs the slave config.
 *
 * Revision 3.3  1995/11/17  09:01:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:20:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/08  00:45:17  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.1  1995/06/07  20:25:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ROMMON_BOOT_VAR               0
#define ROMMON_CONFIG_VAR             1
#define ROMMON_BOOTLDR_VAR            2

#define MAX_BOOTVAR_LEN               1024

/*
 * ROM variable name strings involved in booting
 */
#define BOOT_VAR_NAME_STRING          "BOOT"
#define CONFIG_VAR_NAME_STRING        "CONFIG_FILE"
#define BOOTLDR_VAR_NAME_STRING       "BOOTLDR"


/*
 * Arguments supplied to the ROM monitor's 'setms' command (and EMT).
 */
#define ROM_MS_SLAVE 0
#define ROM_MS_MASTER 1

#define MON_UNIMPL	(0xFFFFFFFFUL)

extern void	mon_reload(void);
extern uchar	*mon_get_iobase(void);
extern char	*mon_vstring(void);
extern int	mon_write_bad_main_memory_parity(ulong, volatile uint *);
extern uint	mon_setcacr(uint);
extern void	*mon_get_powerfail_addr(void);
extern ulong	mon_getmemsize(void);
extern boolean	mon_touchr(void *);
extern void	mon_disable_dcache(void);
extern void	mon_enable_dcache(void);
extern uchar	mon_get_rev(void);
extern void	*mon_get_cookie(void);
extern void	mon_get_cookie_copy(void *);
extern long	mon_get_nvram_size(void);
extern long	mon_get_mon_nvram_bytes(void);
extern int	mon_get_iosize(void);
extern ushort	mon_version(void);
extern void	*mon_get_bootinfo(void);
extern ipaddrtype mon_bootipaddr(void);
extern char	*mon_procname(void);
extern long	*mon_lasterror(void);
extern int	mon_getchar_raw(void);
extern int	mon_boot_confreg(void);
extern long	mon_boot_sys_rom_flg_set(void);
extern long	mon_sys_chksum_disable(void);
extern ushort	mon_getconfig(void);
extern int	mon_set_confreg(ushort);
extern void     mon_set_ms(uint);
extern uint     mon_get_ms(void);
extern void	mon_vstring_sys_rom_set(char *);
extern char	*mon_vstring_sys(void);
extern void	mon_vstring_sys_set(char *);
extern ulong	mon_proctype(void);
extern ulong	*mon_clockaddr(void);
extern ulong	*mon_hclockaddr(void);
extern void	*mon_romaddr(void);
extern char	*mon_vstring_sys_rom(void);
extern int	mon_simbrk(void);
extern ulong	*mon_exception_addr(void);
extern int	mon_cpu_exception(int sig, int code, void *);
extern boolean	mon_nvcompress_support(void);
extern boolean	mon_swconfreg_support(void);
extern int	mon_set_var(char *);
extern int	mon_get_var(char *, char *, int);
extern uchar	mon_reset_reason(void);
extern char	*mon_fault_history_addr(void);
extern int	mon_fault_history_bytes(void);
extern void	mon_write_fault_history(void);
extern boolean platform_nvvar_support(int);
