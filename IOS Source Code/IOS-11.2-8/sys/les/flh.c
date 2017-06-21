/* $Id: flh.c,v 3.8.12.8 1996/09/10 03:22:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/flh.c,v $
 *------------------------------------------------------------------
 * flh.c -- routines to handle flh buffer logging/subsystem
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flh.c,v $
 * Revision 3.8.12.8  1996/09/10  03:22:20  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.8.12.7  1996/09/06  21:01:02  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.8.12.6  1996/08/28  12:56:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.5  1996/07/12  02:01:20  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.8.12.4  1996/07/04  02:07:01  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/06/28  23:23:38  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/06/17  08:33:30  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.8.12.1  1996/03/18  20:40:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.6  1996/03/16  07:11:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.5  1996/03/14  01:08:55  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.2.4  1996/03/09  05:07:13  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.2.3  1996/03/07  10:05:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.2  1996/02/20  01:07:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/19  17:28:26  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.7  1996/02/13  19:13:25  jquan
 * CSCdi45337:  wall() function used by flh is bogus. Replace fprintf by
 * ttyline_sendmessage in wall() to avoid hanging system. Change comments
 * regarding wall() function. Remove datagram_done() after
 * ttyline_sendmessage() since the background tty output process will free
 * the buffer.
 *
 * Revision 3.6  1996/01/24  20:18:01  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5.2.1  1996/01/24  22:22:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/22  06:37:26  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:32:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  17:37:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/25  11:44:27  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.5  1995/06/28 09:28:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/23  18:48:13  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/19  06:46:54  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:31:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "config.h"
#include "config_register.h"
#include "file.h"
#include "logger.h"
#include "ttysrv.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../../boot/reason.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/flash_registry.h"
#include "../dev/monitor1.h"
#include "../if/ether.h"         /* for parse_ether_address */
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../les/flh.h"
#include "../os/boot.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

static char *system_flash = NULL;
static  struct _flh_log_routines flh_log_routines = {0};
static  int have_flh_logging = -1;
static addrtype hostip_flh;    /* Server address */

static const char config_reg_err[] =
             "\nERR: Config register boot bits set for manual booting";
static const char *const flh_warning[] = {
"\n                         ****  NOTICE  ****",
"\nFlash load helper v1.0",
"\nThis process will accept the copy options and then terminate",
"\nthe current system image to use the ROM based image for the copy.",
"\nRouting functionality will not be available during that time.",
"\nIf you are logged in via telnet, this connection will terminate.",
"\nUsers with console access can see the results of the copy operation.",
"\n                         ---- ******** ----",
""
};

/*
 * flh_buffer_init
 * This routine initializes a buffer in memory (at the top of
 * Processor memory) for the use of holding console output during
 * flh/bootstrap operations.
 */
static void flh_buffer_init (void)
{

    switch (cpu_type) {

      case CPU_CANCUN:
      case CPU_CRISTAL:
      case CPU_IGS_BRUT:
      case CPU_MERLOT:
      case CPU_BASS:
	have_flh_logging = FLH_LOGROUTINES(&flh_log_routines);
	
      default:
	break;
    }
}

/*
 * flh_putc -- put characters into buffer for later printing.
 */

static void flh_putc (char c)
{
    switch (cpu_type) {

      case CPU_CANCUN:
      case CPU_CRISTAL:
      case CPU_IGS_BRUT:
      case CPU_MERLOT:
      case CPU_BASS:
	if (FLH_LOGGING_ACTIVE) {
	    if (flh_log_routines.flh_write_char) {
		(*flh_log_routines.flh_write_char)(c);
	    }
	}
	break;
      default:
	break;
    }
}

/*
 * Decompress the FLH log into a buffer for display or
 * transfer (via TFTP) to a server
 */
static char *flhlog_to_buffer (void)
{
    char *buf;
   
    if (FLH_LOGGING_ACTIVE) {
	buf = malloc(FLH_DISPLAY_BUFSIZE);
	if (buf) {
	    /* space for \0 at end of buff for printing */
	    (*flh_log_routines.flh_print_output)(buf,(FLH_DISPLAY_BUFSIZE-1));
	    return (buf);
	}
    }
    return (NULL);
}

/*
 * Function for "show flh-log"
 * Print out the contents of our FLH log history.
 */
void flhbuf_print (parseinfo *csb)
{
    char *buf;

    if (FLH_LOGGING_ACTIVE) {
	if ((buf = flhlog_to_buffer()) != NULL) {
	    automore_enable("");
	    printf("%s",buf);
	    automore_disable();
	    free(buf);
	}
    } else {
	printf("\nFLH logging not supported by boot proms");
    }
}

/*
 * Clear out FLH logging buffer
 */

static void flh_buffer_clear (void)
{
    if (FLH_LOGGING_ACTIVE) {
	(*flh_log_routines.flh_clear_buff)();
    }
}

/*
 * Check boot bits in the config register.
 * Return FALSE if boot bits are 0 and user is on a virtual console.
 * This is to disallow the FLH use in the above case so as to prevent
 * the possibility of the system going down to the ROM MON prompt in
 * the event of a failure.
 */
static boolean flh_boot_bit_check (void)
{
    ushort config_reg;

    config_reg = ~mon_getconfig();
    if ((config_reg & CFG_FILE) == 0) { /* boot bits are zero */
        if (stdio->type != CTY_LINE) {  /* Not on the console */
            printf(config_reg_err);
            return (FALSE);
        }
    }
    return (TRUE);
}

static int flh_version (void)
{
    int flh_ver;

    flh_ver = FLH_ROMVERSION;
    /*
     * In 9.14, the version number starts from 0.
     * This is bad for the modular trees such as this 10.0 tree
     * because the reg_invoke_flh_version() call will return 0
     * in the default case where no callback function has been
     * registered. Sooo, we will increment the version number by
     * 1. The calling function will check for >0 for a valid
     * version number.
     * a) if callback function has been registered but boot ROMs
     *    don't support FLH, then the returned value of -1 will
     *    become 0 after the increment
     * b) if callback function has been registered and boot ROMs
     *    support FLH, then the returned value of 0 will become 1
     * c) if callback function hasn't been registered, then the
     *    calling function will get the value 0 because we won't
     *    come here
     */
    return (++flh_ver);
}

static boolean flh_ok_check (ulong prompt)
{
    int cnt;
    /*
     * make sure that the config register boot bits are non-zero
     * if user is not on the console.
     */
    if (!flh_boot_bit_check())
        return (FALSE);

    if (prompt) {
        for (cnt = 0; *flh_warning[cnt]; cnt++)
            printf("%s", flh_warning[cnt]);
        if (wall(NULL))   /* TTY connection(s) active */
            printf("\n[There are active users logged into the system]");
        if (!confirm("\nProceed? "))
            return (FALSE);
    }
    return (TRUE);  /* AB: OK ?? */
}

static void flh_invoke (void *flh_parms)
{
    flash_copy_parms_t *fparms = flh_parms;
    struct flash_load_helper helper;
    uint flags;
    char *tmpptr;
    char partition_num = 0;
    long *boot_data;

    flags = fparms->erase ? HELPER_FLASH_ERASE : 0;
    switch (fparms->copy_option) {
    case FILE_ACCESS_TFTP:
    default:
        flags |= HELPER_TFTP_COPY_FLASH;
        strcpy(fparms->host, address_string(&fparms->addr));
        break;
    case FILE_ACCESS_RCP:
        flags |= HELPER_RCP_COPY_FLASH;
        strcpy(fparms->host, address_string(&fparms->addr));
        break;
    case FILE_ACCESS_MOP:
        flags |= HELPER_MOP_COPY_FLASH;
        strcpy(fparms->host, "");
        break;
    }

    helper.host  = fparms->host;
    helper.src_filename  = fparms->sfname;

    /* 
     * check to see if we need to include destination file name...
     */

    /*
     * fparms->path at this point is either "flash" in a single partitioned
     * system or flash:X (where X is partition number) in a multipartitioned
     * system
     */
    if ((tmpptr = strchr(fparms->path,':')) != NULL ) {
	partition_num = *(tmpptr + 1); /* incr one char and grab ASCII */
				/* partition_number. */
    }

    fparms->buff[0] = '\0';	/* init buff to be null string */
    if (partition_num && partition_num != '1') { /* if we are not */
				/* copying to partition 1 then we need */
				/* partition number in the dest filename. */
	sprintf(fparms->buff, "%c:", partition_num);
    }

    if (strcmp(fparms->dfname, fparms->sfname) != 0) {
	/* 
	 * strings are *not* equal so we must use dest file name 
	 */
	strcat(fparms->buff, fparms->dfname);
    }

    helper.dst_filename = fparms->buff;
    helper.flags = flags;

    /*
     * Let's send an output message to all active TTY line
     * connections except stdio (this terminal)
     */
    wall("\n** System going down for Flash upgrade **\n");
    errmsg(&msgsym(RELOAD, SYS));
    reg_invoke_will_reload();
    flush();
    process_sleep_for( ONESEC * 3 );
    reg_invoke_hardware_shutdown();

    flh_buffer_clear();
    boot_data = mon_lasterror();
    if (boot_data && (boot_data != (long *)-1))
	boot_data[BOOT_REASON] = EXC_RELOAD;
    FLH_START(&helper);
}
/*
 * FLH copy function. Will get invoked only in the bootstrap.
 */
static int auto_copy_command (flash_copy_parms_t *fparms)
{
    int cnt, tyme;
    boolean sts;
    flash_copy_parms_t *temp_parms;

    if (system_flash == NULL) {
	system_flash = reg_invoke_flash_device_default();
	if (system_flash == NULL)
	    return (0);
    }

    /* Parse the destination filename to get the device name and the 
     * filename.  Since the parse routine stores the filename in 
     * sfname, we'll copy sfname to dfname.
     */
    temp_parms = reg_invoke_flash_filename_parse(fparms->dfname);
    if (temp_parms == NULL)  /* no memory */
	return (0);
    strcpy(fparms->devnm, temp_parms->devnm);
    strcpy(fparms->dfname, temp_parms->sfname);
    free(temp_parms);

    fparms->prompt  = 0;
    fparms->verbose = FILE_FLAGS_VERBOSE;
    if (fparms->erase)
        fparms->erase = FILE_FLAGS_ERASE;

    /*
     * On success, everthing's fine. On error, retry upto 3 times.
     * Retry only if returned status indicates that retries are
     * worthwhile.
     */
    cnt = 0;
    do {
        switch (cnt) {
        case 0:  /* Initial try */
            break;
        case 1:   /* first retry; immediate */
            printf("\n%%FLH: retry #1");
            flush();
            break;
        case 2:  /* second retry after 30 secs */
        default:
            tyme = 30;
            printf("\n%%FLH: Idling for %d secs before retry #%d",
                   tyme, cnt);
            flush();
            process_sleep_for(tyme * ONESEC); /* since we have to pass msecs */
            break;
        }

        fparms->sts = 0;

        switch (fparms->option) {
        case BOOTCOPYTFTPFLASH:
        case (BOOTCOPYTFTPFLASH |  BOOTERASEFLASH):
            printf("\n%%FLH: %s from %s to flash ... %s",
                   fparms->sfname, fparms->host, cnt ? "(retry)" : "");
            flush();
            sts = copy_tftp_flash(fparms);
            break;
        case BOOTCOPYRCPFLASH:
        case (BOOTCOPYRCPFLASH |  BOOTERASEFLASH):
            printf("\n%%FLH: %s from %s to flash ... %s",
                   fparms->sfname, fparms->host, cnt ? "(retry)" : "");
            flush();
            sts = copy_rcp_flash(fparms);
            break;
        case BOOTCOPYMOPFLASH:
        case (BOOTCOPYMOPFLASH |  BOOTERASEFLASH):
            printf("\n%%FLH: %s from MOP server to flash ... %s",
                   fparms->sfname, cnt ? "(retry)" : "");
            flush();
            sts = copy_mop_flash(fparms);
            break;
        default:
            return (FALSE);
        }
        if (sts) /* success */
            return (1);
        if (fparms->sts >= 0)  /* failure but retries not possible */
            return (0);
        cnt++;
    } while (cnt < FLH_RETRY_COUNT);
    return (-1);
}

#define DEFAULT_HOST_ADDRESS "255.255.255.255"

static int flash_load_helper (void *parm)
{
    struct plb_t *plb = parm;
    flash_copy_parms_t *fparms;
    char *cptr;
    int sts;

    fparms = malloc(sizeof(flash_copy_parms_t));
    if (fparms == NULL) {  /* no memory */
        return (-1);
    }
    fparms->option = plb->plb_option;
    fparms->erase = (plb->plb_option & BOOTERASEFLASH);
    /*
     * Parse out the Flash load helper command for the source file
     * name (name on server), server address, and destination file
     * name (name in flash).
     * syntax : flh "<src-file> <address> <dest-file>"  for TFTP/RCP
     *          flh "<src-file> <address> <interface> <dest-file>"  for MOP
     */
    cptr = plb->plb_str;
    cptr = gettoken(cptr, fparms->sfname, FLASH_FILE_NAME_LEN-1, WORD);
    cptr = gettoken(cptr, fparms->host, FLASH_SERVER_NAME_LEN-1, WORD);
    cptr = gettoken(cptr, fparms->dfname, FLASH_FILE_NAME_LEN-1, WORD);

    switch (plb->plb_option) {
    case BOOTCOPYTFTPFLASH:
    case (BOOTCOPYTFTPFLASH | BOOTERASEFLASH):
    case BOOTCOPYRCPFLASH:
    case (BOOTCOPYRCPFLASH | BOOTERASEFLASH):
        if (!fparms->host[0]) { /* no host address */
            strcpy(fparms->host, DEFAULT_HOST_ADDRESS);
        }
        fparms->addr.type = ADDR_IP;
        if (!reg_invoke_parse_address(ADDR_IP, fparms->host, &fparms->addr)) {
            strcpy(fparms->host, DEFAULT_HOST_ADDRESS);
            reg_invoke_parse_address(ADDR_IP, fparms->host, &fparms->addr);
        }
        break;
    case BOOTCOPYMOPFLASH:
    case (BOOTCOPYMOPFLASH | BOOTERASEFLASH):
        if (fparms->host[0]) {  /* Something present. See if it's a MAC addr */
            if (!parse_ether_address(fparms->host, fparms->buff)) { /* Nope */
                strcpy(fparms->dfname, fparms->host); /* Must be dest file nm */
                fparms->host[0] = 0; /* NULL out host address field */
            }
        }
        break;
    default:
        free(fparms);
        return (0);
    }
    if (!fparms->dfname[0])  /* No dest file name */
        strcpy(fparms->dfname, fparms->sfname); /* Set = src file name */

    sts = auto_copy_command((void *)fparms);

    /*
     * Now modify plb_str to fake a 'boot flash' command.
     * The actual boot will depend on the value of the boot bits
     * and any 'boot system' config commands.
     *
     */
    free(fparms);
    if (sts > 0)   /* Flash download succeeded */
        printf("\n%sRe-booting system after download", "%FLH: ");
    else  /* Failure  */
        printf("\n%sFlash download failed", "%FLH: ");
    flush();
    return (sts);
}

static void flhlog_copy_server_addr_init (addrtype *host)
{
    host->type    = ADDR_IP;
    host->ip_addr = -1L;
}

/*
 * Copy flh-log tp a TFTP server.
 */
static boolean copy_flhlog_tftp (void *copy_parms)
{
    ipsocktype *soc;
    char *bufptr, *tmptr, dname[80];
    boolean result, done = FALSE;
    flash_copy_parms_t *fparms;
    ulong nbytes, length;
    int blknum;

    fparms = (flash_copy_parms_t *)copy_parms;
    fparms->copy_option = FILE_ACCESS_TFTP;
    /*
     * Get IP/NSAP address of tftp server
     */
    if (fparms->addr.ip_addr == IPADDR_ZERO)
        address_copy(&fparms->addr, &hostip_flh);
    if (!flash_copy_get_server(&fparms->addr, (char *)fparms->buff,
                               hostip_flh, fparms->prompt)) {
        fparms->snmp_sts = SNMP_INVALID_SRVR_ADDR;
        return (FALSE);
    }
    /* save host address for next time default */
    address_copy(&hostip_flh, &fparms->addr);

    sprintf(dname, "Destination file name [%s-flh-log]? ", hostname);
    if (!flash_copy_file_name(fparms->dfname, (FLASH_FILE_NAME_LEN - 1),
			      fparms->prompt ? dname : NULL, FALSE)) {
        sprintf(fparms->dfname, "%s-flh-log", hostname);/* set default value */
    }
    if (!(soc = reg_invoke_ip_tftp_open_socket(&fparms->addr, SOCK_ANYSRC))) {
        fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
        return (FALSE);
    }

    /*
     * reg_invoke_ip_tftp_WRQ(ip socket, source filename, data transfer mode,
     *                        boolean server : FALSE - display file transfer
     *                                                 progress
     *                                         TRUE  - console silent transfer)
     */
    result = reg_invoke_ip_tftp_WRQ(soc, fparms->dfname, "octet",
                                    fparms->verbose ? FALSE : TRUE);
    if (result != TRUE) {
        reg_invoke_ip_close_socket(soc);
        fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
        return (FALSE);
    }

    bufptr = tmptr = flhlog_to_buffer();

    GET_TIMESTAMP(fparms->start_time); /* note start time */

    result = TRUE;
    length = strlen(tmptr);
    for (blknum = 1; result && !done; blknum++) {
        if (length >= TFTP_MAXDATA) {
            nbytes = TFTP_MAXDATA;
        }
        else {
            nbytes = length;
            done = TRUE;
        }
        result = reg_invoke_ip_tftp_sendblock(soc, blknum, tmptr,
                                              nbytes, TRUE);
        tmptr = tmptr + nbytes;
        length -= nbytes;
    }

    reg_invoke_ip_close_socket(soc);
    fparms->elapsed_time = ELAPSED_TIME(fparms->start_time);
    free(bufptr);

    if (!result) {
        fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
        if (fparms->verbose)
            printf("\nError during upload of FLH log file to server");
        return (FALSE);
    }
    else {
        if (fparms->verbose)
            printf("\nUpload of FLH log file to server done");
        return (TRUE);
    }
}

void copy_flhlog_tftp_wrapper (parseinfo *csb)
{
    flash_copy_parms_t *fparms;
    fparms = malloc(sizeof(flash_copy_parms_t));
    if (fparms == NULL) { /* no memory :-( */
        printf(nomemory);
        return;
    }
    fparms->prompt  = FILE_FLAGS_PROMPT;
    fparms->verbose = FILE_FLAGS_VERBOSE;

    psion(stdio);  /* Enable user interrupts */
    copy_flhlog_tftp((void *)fparms);
    free(fparms);
    psioff(stdio);  /* Disable user interrupts */
}


/* 
 * check parms passed to flh and make sure they don't overrun a buffer
 * src_file will be an ascii string containing the source file name
 * dst_file is the destination file and will be in the form 
 * "flash:filename" or
 * "flash:part:filename" (part is a number partition number)
 * copy_type contains COPY_TFTP, COPY_MOP or COPY_RCP
 */
static boolean flh_check_parms (char *src_file,char *dst_file,
				int verbose, int copy_type)
{
	int src_file_len;
	int dst_file_len;
	char *tmpptr;
	int rom_avail_buf_len;

        rom_avail_buf_len = FLH_PARMBUFSIZE;
	if (rom_avail_buf_len < 0) { /* call not supported in rom mon */
	    rom_avail_buf_len = FLH_SRCDST_ROM_BUFF_SIZE;
	}

	switch (copy_type) {
	  case FILE_ACCESS_TFTP:
	  case FILE_ACCESS_RCP:
	    /* 
	     * these two cases require space be reserved for the worst case
	     * ip address -- XXX.XXX.XXX.XXX
	     */
	    rom_avail_buf_len -= FLH_MAX_IP_LENGTH; 
	    break;
	  case FILE_ACCESS_MOP:
	  default:
	    break;
	}
	if (rom_avail_buf_len > 0) { /* just sanity check here */
	    /* reserve space for null terminator to string */
	    rom_avail_buf_len--;
	}
        src_file_len = strlen(src_file);
	/* 
	 * get length of dest file.  First check if it is
	 * the same as src filename.  If so then don't count it.
	 * else count the entire string
	 */
	dst_file_len = 0;
	/* 
	 * go through some hoops to allow for file that doesn't have the 
	 * form "flash:filename" or "flash:X:filename" and just has "filename"
	 */
	
	if ((tmpptr = strchr(dst_file,':')) != NULL ) {
	    char *tmpptr2 = tmpptr; /* is this necessary  I bet not */

	    /* 
	     * at this point we have a pointer to the first : of "flash:??????"
	     * If flash is paritition for a single bank then the next test
	     * will fail,  otherwise  we'll find the second :
	     *
	     * now test for flash:X:filename
	     */
	    if ((tmpptr2 = strchr(tmpptr+1,':')) != NULL) {
		if ( *(tmpptr2-1) != '1') { /* if we dont have flash:1:file */
				/* then we have (at least)"flash:2:filename"*/ 
				/* in the dest filename */
		    dst_file_len += 2; /* count up 2 characters "X:" */
		}
		tmpptr = tmpptr2; /* found anouther : so point to it */
	    }
	    /* 
	     * tmpptr points to  : in string, bump it up one char to point
	     * at filename
	     */
	    tmpptr++;
	} else {
	    tmpptr = dst_file;	/* allow for neither flash: nor flash:X: in */
				/* dest filename */
	}
	/* 
	 * at this point tmpptr points to the dest filename 
	 * without flash:[X:] header
	 * check now to see if src file and dst file are the same
	 */
	if (strcmp(tmpptr, src_file) != 0) {
	    /* strings are *not* equal so count dest file name */
	    dst_file_len += strlen(tmpptr);
	}
	if ((src_file_len + dst_file_len) > rom_avail_buf_len) {
	    if (verbose) {
		printf("\nSrc file and Dest file length too large for FLH"
		       "\nBuffer exceeded by %d byte(s)",
		       (src_file_len + dst_file_len - 
		       rom_avail_buf_len));
	    }
	    return (FALSE);
	}
	return(TRUE);
}

static void flh_init (subsystype *subsys)
{
    flhlog_copy_server_addr_init(&hostip_flh);
    flh_buffer_init();
    flh_parser_init();
    /* do registry stuff */
    reg_add_flh_log_putc(FLH_OP, flh_putc, "flh_putc");
    reg_add_flh_call(flash_load_helper, "flash_load_helper");
    reg_add_flh_version(flh_version, "flh_version");
    reg_add_flh_ok_check(flh_ok_check, "flh_ok_check");
    reg_add_flh_invoke(flh_invoke, "flh_invoke");
    reg_add_copy_flhlog_tftp(copy_flhlog_tftp, "copy_flhlog_tftp");
    reg_add_flh_check_parms(flh_check_parms, "flh_check_parms");
}

#define FLH_MAJVERSION 1
#define FLH_MINVERSION 0
#define FLH_EDITVERSION  1

SUBSYS_HEADER(flh, FLH_MAJVERSION, FLH_MINVERSION, FLH_EDITVERSION,
              flh_init, SUBSYS_CLASS_DRIVER,
              "seq: flash_devices",
              NULL);

