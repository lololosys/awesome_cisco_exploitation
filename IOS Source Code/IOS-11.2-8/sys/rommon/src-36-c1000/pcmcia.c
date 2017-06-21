/* $Id: pcmcia.c,v 3.2.58.1 1996/03/21 23:29:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/pcmcia.c,v $
 *------------------------------------------------------------------
 * pcmcia.c
 *
 * December 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Sapphire PCMCIA initialization code.
 *------------------------------------------------------------------
 * $Log: pcmcia.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:14  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * PCMCIA operations
 *
 * usage: pcmcia <cmd> <arguments>
 *    possible commands:
 *         init  <base addr for 6720 accesses>
 *
 */

#include "monitor.h"
#include "error.h"
#include "proto.h"
#include "mon_defs.h"
#include "clpd6720.h"

static void pcmcia_usage(void);
static void pcmcia_set_usage(void);
static int pcmcia_bktest();
int pcmcia_init(void);
static int pcmcia_enable();
static int pcmcia_dump();
static int pcmcia_help_help();
static int pcmcia_set(), pcmcia_help();

static struct cmdstruct {
    char *command;
    int (*procedure)();
    char *description;
    int flags;
} cmd[] = {  /* in alphabetical order */
    "bktest",pcmcia_bktest,"bktest",0,
    "dump",pcmcia_dump,"dump the 6720 controller registers",0,
    "enable",pcmcia_enable,"enable window",0,
    "init",pcmcia_init,"init the 6720 controller",0,
    "help",pcmcia_help,"help",0,
    "set",pcmcia_set,"set  window | set timing",0
};

#define N_CMD (sizeof(cmd)/sizeof(struct cmdstruct))

int
pcmcia_cmds (int argc, char *argv[])
{
    
    int i,retval;
    struct cmdstruct *cmdptr;

    if (argc < 2)  {
	pcmcia_usage();
	return(1);
    }
    
    printf("argv1 %s\n", argv[1]);
    for(i=0, cmdptr=cmd; i<N_CMD; i++, cmdptr++) {
	if(strcmp(argv[1],cmdptr->command) == 0) {  /* match */
	    retval = (*cmdptr->procedure)(argc,argv);
	    return(retval);
	}
    }
    printf("%s not found\n",argv[1]);
    return(3);
    
}

#define FAST_WINDOW_TIMING_SET     CLPD6720_TIMING_0
#define FAST_WINDOW_SETUP_TIME     0x04
#define FAST_WINDOW_COMMAND_TIME   0x0a
#define FAST_WINDOW_RECOVERY_TIME  0x04
#define SLOW_WINDOW_TIMING_SET     CLPD6720_TIMING_1
#define SLOW_WINDOW_SETUP_TIME     0x20
#define SLOW_WINDOW_COMMAND_TIME   0x48
#define SLOW_WINDOW_RECOVERY_TIME  0x20

int
pcmcia_init ()
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;
    
    clpd6720_init();
    
    /*
     * Set up the timing parameters for the windows.
     */
    clpd6720_set_timing(CLPD6720_SOCKET_0,
			FAST_WINDOW_TIMING_SET,
			FAST_WINDOW_SETUP_TIME,
			FAST_WINDOW_COMMAND_TIME,
			FAST_WINDOW_RECOVERY_TIME);
    
    clpd6720_set_timing(CLPD6720_SOCKET_0,
			SLOW_WINDOW_TIMING_SET,
			SLOW_WINDOW_SETUP_TIME,
			SLOW_WINDOW_COMMAND_TIME,
			SLOW_WINDOW_RECOVERY_TIME);
    
    clpd6720_set_timing(CLPD6720_SOCKET_1,
			FAST_WINDOW_TIMING_SET,
			FAST_WINDOW_SETUP_TIME,
			FAST_WINDOW_COMMAND_TIME,
			FAST_WINDOW_RECOVERY_TIME);
    
    clpd6720_set_timing(CLPD6720_SOCKET_1,
			SLOW_WINDOW_TIMING_SET,
			SLOW_WINDOW_SETUP_TIME,
			SLOW_WINDOW_COMMAND_TIME,
			SLOW_WINDOW_RECOVERY_TIME);
    
    
    /*
     * Set all the windows to use the fast timing set.
     */
    for (socket = CLPD6720_SOCKET_0; socket <= CLPD6720_SOCKET_1; socket++) { 
	for (window = CLPD6720_WINDOW_0; window <= CLPD6720_WINDOW_4; window++) { 
	    clpd6720_set_window_timing(socket,window,FAST_WINDOW_TIMING_SET);
	}
    }
    
}

pcmcia_bktest (int argc, char *argv[])
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;
    long temp;
    int soc, win;
    
    if (argc != 4) {
    usage:
	printf("usage: %s %s <socket> <window>\n", argv[0], argv[1]);
	return(1);
	
    }
    
    if(!getnum(argv[2], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    if(!getnum(argv[3], 10, &temp)) goto usage;
    window = (clpd6720_window_t)temp;
    
    pcmcia_init();
    
    clpd6720_set_window(socket, 
			window, 
			0x0000000, /* start */
			0xff0000,  /* offset */
			0x400000,  /* length */
			CLPD6720_COMMON_MEM, 
			CLPD6720_WIDTH_16_BIT, 
			CLPD6720_READ_WRITE
			);
    clpd6720_enable_window(socket, window);
    clpd6720_enable_socket(socket);
    return(0);
}

pcmcia_enable (int argc, char *argv[])
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;
    long temp;
    
    if (argc != 4) {
    usage:
	printf("usage: %s %s <socket> <window>\n", argv[0], argv[1]);
	return(1);
	
    }
    
    if(!getnum(argv[2], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    if(!getnum(argv[3], 10, &temp)) goto usage;
    window = (clpd6720_window_t)temp;
    
    clpd6720_enable_socket(socket);
    clpd6720_enable_window(socket, window);
    return(0);
}

pcmcia_dump (int argc, char *argv[])
{
    int i;
    clpd6720_socket_t socket;
    long temp;
    
    if (argc != 3) {
    usage:
	printf("usage: %s %s <socket>\n", argv[0], argv[1]);
	return(1);
    }
    
    if(!getnum(argv[2], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    
    for (i=0; i<64; i++)
	clpd6720_reg_read(socket, i);
    return(0);
}

static int pcmcia_set_help();
static int pcmcia_set_timing();
static int pcmcia_set_window_timing();
static int pcmcia_set_window();

static struct cmdstruct set_cmd[] = {  /* in alphabetical order */
    "help",pcmcia_set_help,"set help",0,
    "timing",pcmcia_set_timing,"set timing",0,
    "window_timing",pcmcia_set_window_timing,"set window_timing",0,
    "window",pcmcia_set_window,"set window",0
    };

#define N_SET_CMD (sizeof(set_cmd)/sizeof(struct cmdstruct))

static int
    pcmcia_set (int argc, char *argv[])
{
    
    int i,retval;
    struct cmdstruct *cmdptr;
    
    if (argc < 3)  {
	pcmcia_set_usage();
	return(1);
    }
    
    for(i=0, cmdptr=set_cmd; i<N_SET_CMD; i++, cmdptr++) {
	if(strcmp(argv[2],cmdptr->command) == 0) {  /* match */
	    retval = (*cmdptr->procedure)(argc,argv);
	    return(retval);
	}
    }
    printf("%s %s not found\n",argv[1], argv[2]);
    return(3);
}

static int
    pcmcia_set_timing (int argc, char *argv[])
{
    clpd6720_socket_t socket;
    clpd6720_timing_t timing_set;
    uchar setup;
    uchar command;
    uchar recovery;
    long temp;
    
    if (argc != 8) {
    usage:
	printf("usage: %s %s %s <socket> <timing_set>\n", argv[0], argv[1], argv[2]);
	printf("       <setup> <command> <recovery>\n");
	return(1);
    }
    
    if(!getnum(argv[3], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    if(!getnum(argv[4], 10, &temp)) goto usage;
    timing_set = (clpd6720_timing_t)temp;
    if(!getnum(argv[5], 10, &temp)) goto usage;
    setup = (uchar)temp;
    if(!getnum(argv[6], 10, &temp)) goto usage;
    command = (uchar)temp;
    if(!getnum(argv[7], 10, &temp)) goto usage;
    recovery = (uchar)temp;
    
    printf("socket<%x> timing_set<%x> setup<%x> command<%x> recovery<%x>\n",
	   socket,
	   timing_set,
	   setup,
	   command,
	   recovery
	   );
    
    clpd6720_set_timing(socket, 
			timing_set, 
			setup, 
			command, 
			recovery
			);
    
    return(0);
}

static int
    pcmcia_set_window_timing (int argc, char *argv[])
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;
    clpd6720_timing_t timing;
    long temp;
    
    if (argc != 6) {
    usage:
	printf("usage: %s %s %s <socket> <window> <timing>\n", argv[0], argv[1], argv[2]);
	return(1);
    }
    
    if(!getnum(argv[3], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    if(!getnum(argv[4], 10, &temp)) goto usage;
    window = (clpd6720_window_t)temp;
    if(!getnum(argv[5], 10, &temp)) goto usage;
    timing = (clpd6720_timing_t)temp;
    
    printf("socket<%x> window<%x> timing<%x>\n", socket, window, timing);
    
    clpd6720_set_window_timing(socket, window, timing);
    
    return(0);
}

static int
    pcmcia_set_window (int argc, char *argv[])
{
    clpd6720_socket_t socket;
    clpd6720_window_t window;
    ulong local_addr;
    ulong pcmcia_addr;
    ulong length;
    clpd6720_pcmcia_type_t pcmcia_type;
    clpd6720_window_width_t window_width;
    clpd6720_write_protect_t write_protect;
    char u_pcmcia_type;
    int u_window_width;
    int u_write_protect;
    long temp;
    
    if (argc != 11) {
    usage:
	printf("usage: %s %s %s <socket> <window> <local_addr> <pcmcia_addr> <length>\n", argv[0], argv[1], argv[2]);
	printf("       <pcmcia_type 'c' or 'a'> <window_width 8 or 16> <write_protect 0 or 1>\n");
	return(1);
    }
    
    if(!getnum(argv[3], 10, &temp)) goto usage;
    socket = (clpd6720_socket_t)temp;
    if(!getnum(argv[4], 10, &temp)) goto usage;
    window = (clpd6720_window_t)temp;
    if(!getnum(argv[5], 10, &temp)) goto usage;
    local_addr = (ulong)temp;
    if(!getnum(argv[6], 10, &temp)) goto usage;
    pcmcia_addr = (ulong)temp;
    if(!getnum(argv[7], 10, &temp)) goto usage;
    length = (ulong)temp;
    u_pcmcia_type = *argv[8];
    if(!getnum(argv[8], 10, &temp)) goto usage;
    u_window_width = temp;
    if(!getnum(argv[9], 10, &temp)) goto usage;
    u_write_protect = temp;
    
    printf("socket<%x> window<%x> local_addr<%x> pcmcia_addr<%x>\n",
	   socket,
	   window,
	   local_addr,
	   pcmcia_addr
	   );
    
    printf("length<%x> pcmcia_type<%c> window_width<%x> write_protect<%x>\n",
	   length,
	   u_pcmcia_type,
	   u_window_width,
	   u_write_protect
	   );
    
    if (u_pcmcia_type == 'c')
	pcmcia_type = CLPD6720_COMMON_MEM;
    else if (u_pcmcia_type == 'a')
	pcmcia_type = CLPD6720_ATTRIB_MEM;
    else {
	printf("pcmcia_type<%c> ERROR\n", u_pcmcia_type);
	return(1);
    }
    
    if (u_window_width == 16)
	window_width = CLPD6720_WIDTH_16_BIT;
    else if (u_window_width == 8)
	window_width = CLPD6720_WIDTH_8_BIT;
    else {
	printf("window_width<%d> ERROR\n", u_window_width);
	return(1);
    }
    if (u_write_protect)
    	write_protect = CLPD6720_READ_ONLY;
    else
    	write_protect = CLPD6720_READ_WRITE;
    
    clpd6720_set_window(socket, 
			window, 
			local_addr, 
			pcmcia_addr, 
			length, 
			pcmcia_type, 
			window_width, 
			write_protect
			);
    
    return(0);
}

static int
    pcmcia_set_help (int argc, char *argv[])
{
    return( pcmcia_help_help(set_cmd, N_SET_CMD) );
}

static int
    pcmcia_help (int argc, char *argv[])
{
    return( pcmcia_help_help(cmd, N_CMD) );
}

static int
    pcmcia_help_help (cmdptr, n_cmds)
struct cmdstruct *cmdptr;
int n_cmds;
{
    int i;
    
    for(i=0; i<n_cmds; i++, cmdptr++)
	printf("%s\n", cmdptr->description);
}

static void
    pcmcia_set_usage (void)
{
    printf("usage: pcmcia set <arguments>; pcmcia ? lists commands\n");
    return;
}

static void
    pcmcia_usage (void)
{
    printf("usage: pcmcia <cmd> <arguments>; pcmcia ? lists commands\n");
    return;
}

/* end of module */
