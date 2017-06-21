/* $Id: rsp_fslib_test.c,v 3.3.54.1 1996/03/18 19:36:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/rsp_fslib_test.c,v $
 *------------------------------------------------------------------
 * Test driver for flash file system debug
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_fslib_test.c,v $
 * Revision 3.3.54.1  1996/03/18  19:36:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.2  1996/03/07  09:36:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  13:50:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/21  19:09:54  snyder
 * CSCdi44272:  more spellink errors
 *              acutal -> actual
 *
 * Revision 3.2  1995/11/17  09:08:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:57:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/18  23:14:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:33:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdlib.h>		/* for atoi */
#include <string.h>
#include <ciscolib.h>

#include "ttysrv.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_dev.h"
#include "fslib_dev_intel_series_2plus.h"
#include "fslib_dev_rsp_internal_flash.h"
#include "fslib_print.h"
#include "clpd6720.h"
#include "ralib_internals.h"

#define strncpy sstrncpy
#define strncat sstrncat

static char *(device_id[]) = {"slot0","slot1","bootflash"};
static char inbuf[128];
static char inbuf_copy[128];

/******************************************************************************
 * Name: isspace
 *
 *******************************************************************************/
static int isspace (char c) {

    if (  (c == ' ') 
       || (c == '\t')
       || (c == '\r')
       || (c == '\n')
       || (c == '\v')
       || (c == '\f') ) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/******************************************************************************
 * Name: format_inbuf
 *
 *******************************************************************************/
static void format_inbuf(char *inbuf) {
    char *in_ptr;
    char *out_ptr;

    in_ptr = inbuf;
    out_ptr = inbuf;
    /*
     * remove all leading spaces
     */
    while (isspace(*in_ptr)) {
	in_ptr++;
    }

    /*
     * main copy loop
     */
    while (*in_ptr != '\0') {
	/* 
	 * If the char is not whitespace, just move it and continue
	 */
	if (!isspace(*in_ptr)) {
	    *out_ptr = *in_ptr;
	    in_ptr++;
	    out_ptr++;
	} else {
	    /*
	     * skip all the white-space chars you find
	     */
	    in_ptr++;
	    while (isspace(*in_ptr)) {
		in_ptr++;
	    }
	    /*
	     * if all white space to the end of the string
	     * we are done (a null is put on later)
	     */
	    if (*in_ptr == '\0') {
		break;
	    } else {
		/*
		 * more non-white chars -- put a single space in
		 * and continue
		 */
		*out_ptr = ' ';
		out_ptr++;
	    }
	}
    }
    /*
     * put a null at the end of the string
     */
    *out_ptr = '\0';
}

/******************************************************************************
 * Name: rdtty_confirmed
 *
 *******************************************************************************/
static int rdtty_confirmed(char *prompt) {
    int rc;
    int parsed;

    rc = FALSE;
    while (1) {
	if (prompt == NULL) {
	    printf("Do you want to continue? [y/n]: ");
	} else {
	    printf("%s [y/n]: ",prompt);
	}
	parsed = rdtty(inbuf, sizeof(inbuf));
	if (parsed == 0)
	    continue;
	format_inbuf(inbuf);
	/*
	 * check for confirmation
	 */
	if ((inbuf[0] == 'Y') || (inbuf[0] == 'y')) {
	    rc = TRUE;
	    break;
	} else if ((inbuf[0] == 'N') || (inbuf[0] == 'n')) {
	    rc = FALSE;
	    break;
	} else {
	    printf("\nPlease answer Y or N\n");
	}
    }
    return(rc);
}

/******************************************************************************
 * Name: rdtty_dec
 *
 *******************************************************************************/
static int rdtty_dec(char *prompt, int low, int high) {
    int dec;
    int parsed;

    while (1) {
	if ((low == 0) && (high == 0)) {
	    printf("%s: ",prompt);	    
	} else {
	    printf("%s (%d to %d): ",prompt,low,high);
	}
	parsed = rdtty(inbuf, sizeof(inbuf));
	if (parsed == 0)
	    continue;
	format_inbuf(inbuf);
	/*
	 * do the conversion the brute force way since there is no
	 * atoi()
	 */
	dec = atoi(inbuf);
	
	if (((low == 0) && (high == 0)) || ((dec >= low) && (dec <= high)))
	    break;
	printf("\nINVALID NUMBER: %d\n",dec);
    }
    return(dec);
}

/******************************************************************************
 * Name: ascii_to_hex
 *
 *******************************************************************************/
static int ascii_to_hex(char *ascii_string) {
    char *as;
    int invert_flag;
    int converted;
    int num_converted;
    int total;
    
    as = ascii_string;
    invert_flag = FALSE;

    /*
     * if the string starts with a minus sign, flip the answer when we
     * are done, and skip the char
     */
    if (*as == '-') {
	invert_flag = TRUE;
	as++;
    }

    /*
     * if the string starts with a plus sign, just skip it
     */
    if (*as == '+') {
	as++;
    }

    /*
     * if the string starts with "0x" or "0X" just skip them
     */
    if ((*as == '0') && ((*(as+1) == 'x') || (*(as+1) == 'X'))) {
	as++;
	as++;
    }

    total = 0;
    converted = TRUE;
    num_converted = 0;
    while (converted && (num_converted <= 8)) {
	switch (*as) {
	case '0': total *= 16;              break;
	case '1': total *= 16; total += 1;  break;
	case '2': total *= 16; total += 2;  break;
	case '3': total *= 16; total += 3;  break;
	case '4': total *= 16; total += 4;  break;
	case '5': total *= 16; total += 5;  break;
	case '6': total *= 16; total += 6;  break;
	case '7': total *= 16; total += 7;  break;
	case '8': total *= 16; total += 8;  break;
	case '9': total *= 16; total += 9;  break;
	case 'A':
	case 'a': total *= 16; total += 10; break;
	case 'B':
	case 'b': total *= 16; total += 11; break;
	case 'C':
	case 'c': total *= 16; total += 12; break;
	case 'D':
	case 'd': total *= 16; total += 12; break;
	case 'E':
	case 'e': total *= 16; total += 14; break;
	case 'F':
	case 'f': total *= 16; total += 15; break;
	default:
	    converted = FALSE;
	    break;
	}
	if (converted) {
	    num_converted++;
	    as++;
	}
    }

    /*
     * if the string started with a minus sign, flip the answer now that we
     * are done
     */
    if (invert_flag) {
	total = -total;
    }

    return(total);

}

/******************************************************************************
 * Name: rdtty_hex
 *
 *******************************************************************************/
static ulong rdtty_hex (char *prompt, int low, int high) {
    ulong hex;
    int parsed;

    while (1) {
	if ((low == 0) && (high == 0)) {
	    printf("%s(hex): ",prompt);
	} else {
	    printf("%s (%lx to %lx)(hex): ",prompt,low,high);
	}
	parsed = rdtty(inbuf, sizeof(inbuf));
	if (parsed == 0)
	    continue;
	format_inbuf(inbuf);
	/*
	 * do the conversion the brute force way since there is no
	 * atoi()
	 */
	hex = ascii_to_hex(inbuf);
	if (((low == 0) && (high == 0)) || ((hex >= low) && (hex <= high)))
	    break;
	printf("\nINVALID NUMBER: %d\n",hex);
    }
    return(hex);
}

/******************************************************************************
 * Name: rdtty_socket
 *
 *******************************************************************************/
static int rdtty_socket(void) {
    int socket;

    socket = rdtty_dec("\nEnter SOCKET number",CLPD6720_SOCKET_0,CLPD6720_SOCKET_1);
    return(socket);
}

/******************************************************************************
 * Name: rdtty_window
 *
 *******************************************************************************/
static int rdtty_window(void) {
    int window;

    window = rdtty_dec("\nEnter WINDOW number",CLPD6720_WINDOW_0,CLPD6720_WINDOW_4);
    return(window);
}

/******************************************************************************
 * Name: rdtty_file_id
 *
 *******************************************************************************/
static void rdtty_file_id (char *prompt, char *buffer, int buffer_length) {
    int parsed;

    while (1) {
	printf("%s (<device id>:<file name>): ",prompt);
	parsed = rdtty(buffer, buffer_length);
	if (parsed == 0)
	    continue;
	format_inbuf(buffer);
	break;
    }
}

/******************************************************************************
 * Name: rdtty_dev_id
 *
 *******************************************************************************/
static char *rdtty_dev_id (void) {
    int i;
    int parsed;

    while (1) {
	printf("Enter device ID (slot0,slot1,bootflash): ");
	parsed = rdtty(inbuf, sizeof(inbuf));
	if (parsed == 0)
	    continue;
	format_inbuf(inbuf);
	for (i = 0; i < (sizeof(device_id)/sizeof(device_id[0])); i++) {
	    if (strcmp(inbuf,device_id[i]) == 0) {
		return(device_id[i]);
	    }
	}
    }
}

/******************************************************************************
 * Name: rdtty_dev_num
 *
 *******************************************************************************/
static int rdtty_dev_num (void) {
    char *dev_id;
    int dev_num;

    dev_id = rdtty_dev_id();
    dev_num = ralib_get_dev_num(dev_id);
    return(dev_num);
}


/******************************************************************************
 * Name: do_dir_1
 *
 * Description:
 *
 ******************************************************************************/
static void do_dir_1(char *dev_id)
{
    char dbuffer[sizeof(dirent_t)];
    int dfd;
    int actual;
    char file_name[32];

    strcpy(file_name,dev_id);
    strcat(file_name,":.");

    dfd = FS_open(file_name,O_RDONLY,0);
    if (dfd <= 0) {
	printf("Error opening directory\n");
    } else {
	print_dirent_header();
	do {
	    actual = FS_getdents(dfd,dbuffer,sizeof(dbuffer));
	    if (actual == sizeof(dbuffer)) {
		print_dirent((dirent_t *)dbuffer);
	    }
	} while (actual == sizeof(dbuffer));
	FS_close(dfd);
    }
}

/******************************************************************************
 * Name: do_dir_all
 *
 * Description:
 *
 ******************************************************************************/
static void do_dir_all(char *dev_id)
{
    char *dbuffer;
    int dbuffer_size;
    int dfd;
    int actual;
    dirent_t *de;
    char file_name[32];

    dbuffer_size = sizeof(dirent_t) * 1000;
    dbuffer = malloc(dbuffer_size);
    if (dbuffer == NULL) {
	printf(nomemory);
    } else {
	strcpy(file_name,dev_id);
	strcat(file_name,":.");
	
	dfd = FS_open(file_name,O_RDONLY,0);
	if (dfd <= 0) {
	    printf("Error opening directory\n");
	} else {
	    print_dirent_header();
	    actual = FS_getdents(dfd,dbuffer,dbuffer_size);
	    if (actual < 0) {
		printf("Error getting dirents (actual = %d)\n",actual);
	    } else {
		de = (dirent_t *)dbuffer;
		while (actual >= sizeof(dirent_t)) {
		    print_dirent((dirent_t *)de);
		    actual -= de->d_reclen;
		    de = (dirent_t *)((char *)de + de->d_reclen);
		}		
	    }
	    FS_close(dfd);
	}
	free(dbuffer);
    }
}

/******************************************************************************
 * Name: fslib_test_main
 *
 *******************************************************************************/

/*
 * Make this a common routine? - XXX
 */
static const char *error_code2text (int code)
{
    static char error_buffer[FS_ERR_BUF_MAX + 1];

    FS_geterrortext(code, error_buffer, FS_ERR_BUF_MAX);
    return (const char *)error_buffer;
}

void fslib_test_main(void)
{
    int i = 0;
    int rc = 0;
    int actual = 0;
#define RDWR_BUFFER_LENGTH 4096
    ulong *wr_buffer;
    ulong *rd_buffer;
    int parsed = 0;
    stat_t stat;
    stat_t *st;

    st = &stat;

    rd_buffer = malloc(RDWR_BUFFER_LENGTH * sizeof(ulong));
    wr_buffer = malloc(RDWR_BUFFER_LENGTH * sizeof(ulong));

    if (rd_buffer != NULL) {
	for (i = 0; i < RDWR_BUFFER_LENGTH; i++) {
	    rd_buffer[i] = 0;
	}
    }

    if (wr_buffer != NULL) {
	for (i = 0; i < RDWR_BUFFER_LENGTH; i++) {
	    wr_buffer[i] = i + (i << 16);
	}
    }

    while (1) {
	printf("\nFILE SYSTEM TEST ==> ");

	parsed = rdtty(inbuf, sizeof(inbuf));
	if (parsed == 0) {
	    printf("parsed == 0\n");
	    continue;
	}

	format_inbuf(inbuf);

	if (strcmp(inbuf,".") == 0) {
	    strncpy(inbuf,inbuf_copy,sizeof(inbuf));
	} else {
	    strncpy(inbuf_copy,inbuf,sizeof(inbuf_copy));
	}

	/*------------------------------------------------------------*
	 * Basic stuff
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"exit") == 0) {
	    break;
	}

	if (strcmp(inbuf,"quit") == 0) {
	    break;
	}

	if (strcmp(inbuf,"term") == 0) {
	    printf("NOT IMPLEMENTED\n");
	    continue;
	}

	if (strcmp(inbuf,"remove card") == 0) {
	    int dev_num;

	    dev_num = rdtty_dev_num();
	    ralib_remove_device(dev_num);
	    continue;
	}

	if (strcmp(inbuf,"insert card") == 0) {
	    int dev_num;

	    dev_num = rdtty_dev_num();
	    ralib_insert_device(dev_num);
	    continue;
	}

	/*------------------------------------------------------------*
	 * Print stuff
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"print off") == 0) {
	    global_print = FALSE;
	    continue;
	}

	if (strcmp(inbuf,"print on") == 0) {
	    global_print = TRUE;
	    continue;
	}

	if (strcmp(inbuf,"set print level") == 0) {
	    int print_level;
	    
	    print_level = rdtty_dec("Enter print level",0,0);
	    global_print_level = print_level;
	    if (print_level == 0) {
		global_print = FALSE;
	    } else {
		global_print = TRUE;
	    }
	    continue;
	}

	if (strcmp(inbuf,"set print") == 0) {
	    ulong value;
	    ulong print_mask;
	    
	    printf("Print Status:\n");
	    printf("  Print %s\n",global_print ?"ENABLED":"DISABLED");
	    printf("  Print mask  = %8x\n",global_print_mask);
	    printf("  Print level = %d\n",global_print_level);

	    print_mask = 0;
	    value = rdtty_dec("0 -- FSLIB main entry point routines",0,1);
	    print_mask |= value << 0;
	    value = rdtty_dec("1 -- high level read utilities or utilities specific to a single function",0,1);
	    print_mask |= value << 1;
	    value = rdtty_dec("2 -- high level write utilities or utilities specific to a single function",0,1);
	    print_mask |= value << 2;
	    value = rdtty_dec("3 -- middle level read utilities or utilities called seldom",0,1);
	    print_mask |= value << 3;
	    value = rdtty_dec("4 -- middle level write utilities or utilities called seldom",0,1);
	    print_mask |= value << 4;
	    value = rdtty_dec("5 -- low level read utilities or utilities called often",0,1);
	    print_mask |= value << 5;
	    value = rdtty_dec("6 -- low level write utilities or utilities called often",0,1);
	    print_mask |= value << 6;
	    value = rdtty_dec("7 -- dev_read ",0,1);
	    print_mask |= value << 7;
	    value = rdtty_dec("8 -- dev_write",0,1);
	    print_mask |= value << 8;
	    value = rdtty_dec("9 -- erase_sector",0,1);
	    print_mask |= value << 9;
	    value = rdtty_dec("10 -- device read_fn",0,1);
	    print_mask |= value << 10;
	    value = rdtty_dec("11 -- device write_fn",0,1);
	    print_mask |= value << 11;
	    value = rdtty_dec("12 -- device erase_fn",0,1);
	    print_mask |= value << 12;
	    value = rdtty_dec("13 -- write utilities",0,1);
	    print_mask |= value << 13;
	    value = rdtty_dec("14 -- comments and errors",0,1);
	    print_mask |= value << 14;
	    value = rdtty_dec("15 -- interrupt functions",0,1);
	    print_mask |= value << 15;
	    value = rdtty_dec("16 -- main RALIB functions",0,1);
	    print_mask |= value << 16;
	    value = rdtty_dec("17 -- RALIB read",0,1);
	    print_mask |= value << 17;
	    value = rdtty_dec("18 -- RALIB write",0,1);
	    print_mask |= value << 18;
	    value = rdtty_dec("19 -- low level read functions",0,1);
	    print_mask |= value << 19;
	    value = rdtty_dec("20 -- low level write functions",0,1);
	    print_mask |= value << 20;
	    value = rdtty_dec("22 -- CLPD6720 high level functions",0,1);
	    print_mask |= value << 22;
	    value = rdtty_dec("23 -- CLPD6720_reg_read and CLPD6720_reg_write",0,1);
	    print_mask |= value << 23;
	    value = rdtty_dec("24 -- Read Data",0,1);
	    print_mask |= value << 24;
	    value = rdtty_dec("25 -- Write Data",0,1);
	    print_mask |= value << 25;

	    global_print_mask = print_mask;
	    printf("Print mask  = %8x\n",global_print_mask);

	    continue;
	}

	if (strcmp(inbuf,"show print") == 0) {
	    
	    printf("Print Status:\n");
	    printf("  Print %s\n",global_print ?"ENABLED":"DISABLED");
	    printf("  Print mask  = %8x\n",global_print_mask);
	    printf("  Print level = %d\n",global_print_level);
	    
	    if (global_print_mask & (1 << 0))
		printf("0 -- FSLIB main entry point routines\n");
	    if (global_print_mask & (1 << 1))
		printf("1 -- high level read utilities or utilities specific to a single function\n");
	    if (global_print_mask & (1 << 2))
		printf("2 -- high level write utilities or utilities specific to a single function\n");
	    if (global_print_mask & (1 << 3))
		printf("3 -- middle level read utilities or utilities called seldom\n");
	    if (global_print_mask & (1 << 4))
		printf("4 -- middle level write utilities or utilities called seldom\n");
	    if (global_print_mask & (1 << 5))
		printf("5 -- low level read utilities or utilities called often\n");
	    if (global_print_mask & (1 << 6))
		printf("6 -- low level write utilities or utilities called often\n");
	    if (global_print_mask & (1 << 7))
		printf("7 -- dev_read \n");
	    if (global_print_mask & (1 << 8))
		printf("8 -- dev_write\n");
	    if (global_print_mask & (1 << 9))
		printf("9 -- erase_sector\n");
	    if (global_print_mask & (1 << 10))
		printf("10 -- device read_fn\n");
	    if (global_print_mask & (1 << 11))
		printf("11 -- device write_fn\n");
	    if (global_print_mask & (1 << 12))
		printf("12 -- device erase_fn\n");
	    if (global_print_mask & (1 << 13))
		printf("13 -- write utilities\n");
	    if (global_print_mask & (1 << 14))
		printf("14 -- comments and errors\n");
	    if (global_print_mask & (1 << 15))
		printf("15 -- interrupt functions\n");
	    if (global_print_mask & (1 << 16))
		printf("16 -- main RALIB functions\n");
	    if (global_print_mask & (1 << 17))
		printf("17 -- RALIB read\n");
	    if (global_print_mask & (1 << 18))
		printf("18 -- RALIB write\n");
	    if (global_print_mask & (1 << 19))
		printf("19 -- low level read functions\n");
	    if (global_print_mask & (1 << 20))
		printf("20 -- low level write functions\n");
	    if (global_print_mask & (1 << 22))
		printf("22 -- CLPD6720 high level functions\n");
	    if (global_print_mask & (1 << 23))
		printf("23 -- CLPD6720_reg_read and CLPD6720_reg_write\n");
	    if (global_print_mask & (1 << 24))
		printf("24 -- Read Data\n");
	    if (global_print_mask & (1 << 25))
		printf("25 -- Write Data\n");
	    continue;
	}

	/*------------------------------------------------------------*
	 * Diagnostic stuff
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"print files") == 0) {
	    print_file_recs(1);
	    continue;
	}

	if (strcmp(inbuf,"print dev") == 0) {
	    int dev_num;

	    dev_num = rdtty_dev_num();
	    print_dev(&FSLIB_device_table[dev_num],1);
	    continue;
	}

	/*------------------------------------------------------------*
	 * File operations
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"open file") == 0) {
	    int fd;
	    int open_flags;
	    char file_id[32];

	    rdtty_file_id("Enter file ID",file_id,sizeof(file_id));
	    open_flags = rdtty_dec("Enter open flags (0=RDONLY,1=CREATE,2=WRONLY)",0,2);
	    switch (open_flags) {
	    default:
	    case 0:
		open_flags = O_RDONLY;
		break;
	    case 1:
		open_flags = O_WRONLY|O_APPEND|O_TRUNC|O_CREAT;
		break;
	    case 2:
		open_flags = O_WRONLY|O_APPEND|O_TRUNC;
		break;
	    }
	    fd = FS_open(file_id,open_flags,0);
	    if (fd < 0) {
		printf("ERROR opening file -- rc = %d(%s)\n",
		    fd, error_code2text(fd));
	    } else {
		printf("File %s opened.  File descriptor = %d\n",file_id,fd);
	    }
	    continue;
	}

	if (strcmp(inbuf,"close file") == 0) {
	    int fd;

	    fd = rdtty_dec("Enter file descriptor",0,0);
	    FS_close(fd);
	    continue;
	}

	if (strcmp(inbuf,"read file") == 0) {
	    int length;
	    int fd;

	    if (rd_buffer == NULL) {
		printf(nomemory);
	    } else {
		fd = rdtty_dec("Enter file descriptor",0,0);
		length = rdtty_hex("Enter number of bytes to read",0,sizeof(rd_buffer));
		actual = FS_read(fd,rd_buffer,length);
		ulong_dump(rd_buffer,actual);
	    }
	    continue;
	}

	if (strcmp(inbuf,"write file") == 0) {
	    int length;
	    int fd;

	    if (wr_buffer == NULL) {
		printf(nomemory);
	    } else {
		fd = rdtty_dec("Enter file descriptor",0,0);
		length = rdtty_hex("Enter number of bytes to write",0,sizeof(wr_buffer));
		FS_write(fd,wr_buffer,length);
	    }
	    continue;
	}

	if (strcmp(inbuf,"delete file") == 0) {
	    char file_id[32];

	    rdtty_file_id("Enter file ID",file_id,sizeof(file_id));
	    FS_delete(file_id);
	    continue;
	}

	if (strcmp(inbuf,"undelete file") == 0) {
	    char *dev_id;
	    int file_index;
	    int mode;

	    dev_id = rdtty_dev_id();
	    file_index = rdtty_dec("Enter file index",0,0);
	    mode = rdtty_dec("Enter undelete mode (AUTO=0, UNIQUE=1)",0,1);
	    FS_undelete(dev_id,file_index,mode);
	    continue;
	}

	if (strcmp(inbuf,"set filetype") == 0) {
	    int filetype;
	    char file_id[32];

	    rdtty_file_id("Enter file ID",file_id,sizeof(file_id));
	    filetype = rdtty_hex("Enter file type",0,0);
	    FS_settype(file_id,filetype);
	    continue;
	}

	if (strcmp(inbuf,"stat") == 0) {
	    char file_id[32];

	    rdtty_file_id("Enter file ID",file_id,sizeof(file_id));
	    rc = FS_stat(file_id,st);
	    if (rc != FSLIB_OK) {
		printf("Error getting status (rc = %d)\n",rc);
	    } else {
		print_stat(st);
	    }
	    continue;
	}

	if (strcmp(inbuf,"fstat") == 0) {
	    int fd;

	    fd = rdtty_dec("Enter file descriptor",0,0);
	    rc = FS_fstat(fd,st);
	    if (rc != FSLIB_OK) {
		printf("Error getting status (rc = %d)\n",rc);
	    } else {
		print_stat(st);
	    }
	    continue;
	}

	if (strcmp(inbuf,"istat") == 0) {
	    char *dev_id;
	    int file_index;

	    dev_id = rdtty_dev_id();
	    file_index = rdtty_dec("Enter file index",0,0);
	    rc = FS_istat(dev_id,file_index,st);
	    if (rc != FSLIB_OK) {
		printf("Error getting status (rc = %d)\n",rc);
	    } else {
		print_stat(st);
	    }
	    continue;
	}

	if (strcmp(inbuf,"squeeze") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    FS_squeeze(dev_id,SQUEEZE_NORMAL);
	    continue;
	}

	if (strcmp(inbuf,"squeeze nolog") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    FS_squeeze(dev_id,SQUEEZE_NO_LOG);
	    continue;
	}

	if (strcmp(inbuf,"dir 1") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    do_dir_1(dev_id);
	    continue;
	}

	if (strcmp(inbuf,"dir") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    do_dir_all(dev_id);
	    continue;

	}

	if (strcmp(inbuf,"fsstat") == 0) {
	    file_sys_stat_t  file_sys_stat;
	    file_sys_stat_t *fss;
	    char *dev_id;

	    fss = &file_sys_stat;
	    dev_id = rdtty_dev_id();
	    rc = FS_fsstat(dev_id,fss);
	    if (rc != FSLIB_OK) {
		printf("Error getting file system status (rc=%d)\n",rc);
	    } else {
		print_file_sys_stat(fss);
	    }
	    continue;
	}

	/*------------------------------------------------------------*
	 * Dumping the squeeze log
	 *------------------------------------------------------------*/
	
	if (strcmp(inbuf,"dump squeeze log") == 0) { 
	    file_sys_stat_t  file_sys_stat;
	    file_sys_stat_t *fss;
	    char *dev_id;
	    int dev_num;
	    fslib_squeeze_log_rec_t squeeze_log_rec;
	    int rc;
	    ulong squeeze_log_offset;
	    ulong squeeze_log_end;
	    ulong squeeze_log_ptr;
	    int actual;
	    
	    fss = &file_sys_stat;
	    dev_id = rdtty_dev_id();
	    dev_num = ralib_get_dev_num(dev_id);
	    rc = FS_fsstat(dev_id,fss);
	    if (rc != FSLIB_OK) {
		printf("Error getting file system status (rc=%d)\n",rc);
	    } else {
		squeeze_log_offset = fss->dib.squeeze_log_offset;
		squeeze_log_end = squeeze_log_offset + fss->dib.squeeze_log_length;
		squeeze_log_ptr = squeeze_log_offset;
		do {
		    actual = ralib_read(dev_num,
					squeeze_log_ptr,
					&squeeze_log_rec,
					sizeof(squeeze_log_rec));
		    if (actual != sizeof(squeeze_log_rec)) {
			printf("Error reading squeeze log record (actual=%d)\n",actual);
			break;
		    } else {
			if (squeeze_log_rec.flags == 0xff) {
			    printf("Squeeze log ends at %lx\n",squeeze_log_ptr);
			    break;
			} else {
			    print_squeeze_log_rec(&squeeze_log_rec);
			    squeeze_log_ptr += sizeof(squeeze_log_rec);
			    if (squeeze_log_ptr >= squeeze_log_end) {
				printf("Squeeze log is FULL\n");
				break;
			    }
			}
		    }
		} while (TRUE);
	    }
	    continue;
	}
	    
	/*------------------------------------------------------------*
	 * Utilities for formatting
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"erase all sectors") == 0) { 
	    ulong sector;
	    fslib_ptr sector_ptr; 
	    ulong sector_size; 
	    int num_sectors;
	    int dev_num; 
	    int rc;
	    int confirmed;
	    int last_sector_to_erase;

            dev_num = rdtty_dev_num();
	    if (dev_num == 2) {
		sector_size = 256 * 1024;
		num_sectors = 32;
	    } else {
		sector_size = 128 * 1024;
		num_sectors = 160;
	    }
	    last_sector_to_erase = rdtty_hex("Enter sector number of last sector to erase: ",0,num_sectors - 1);
	    printf("Erasing sectors 0 to %d\n",last_sector_to_erase);
	    confirmed = rdtty_confirmed(NULL);
	    if (confirmed) {
		for (sector = 0; sector <= last_sector_to_erase; sector++) {
		    sector_ptr = sector * sector_size;
		    if (dev_num == 2) {
			rc = rsp_internal_flash_erase_fn(dev_num,sector_ptr,sector_size);
		    } else {
			rc = intel_series_2plus_erase_fn(dev_num,sector_ptr,sector_size);
		    }
		    printf(" %d",sector);
		    flush();
		    if (rc != FSLIB_OK) {
			break;
		    }
		}
		if (rc == FSLIB_OK) {
		    printf("\nSuccess!\n");
		} else {
		    printf("\nERROR -- ALL SECTORS NOT ERASED!\n");
		}
	    }
	    continue;
	}

	if (strcmp(inbuf,"erase sector") == 0) {
	    ulong sector;
	    fslib_ptr sector_ptr;
	    ulong sector_size;
	    int dev_num;
	    int num_sectors;
	    int rc;
	    int confirmed;

	    dev_num = rdtty_dev_num();
	    if (dev_num == 2) {
		sector_size = 256 * 1024;
		num_sectors = 32;
	    } else {
		sector_size = 128 * 1024;
		num_sectors = 160;
	    }
	    sector = rdtty_hex("Enter sector number",0,num_sectors - 1);
	    sector_ptr = sector * sector_size;
	    printf("initializing RALIB device\n");
            rc = ralib_device_init(dev_num);
	    printf("initializing RALIB device -- rc = %d\n",rc);
	    printf("Erasing sector %d (%8x) on device %d\n",sector,sector_ptr,dev_num);
	    confirmed = rdtty_confirmed(NULL);
	    if (confirmed) {
		if (dev_num == 2) {
		    rc = rsp_internal_flash_erase_fn(dev_num,sector_ptr,sector_size);
		} else {
		    rc = intel_series_2plus_erase_fn(dev_num,sector_ptr,sector_size);
		}
		printf("Erase done -- return code = %d\n",rc);
	    }
	    continue;
	}

	if (strcmp(inbuf,"format") == 0) {
	    fslib_device_info_block_t device_info_block;
	    fslib_device_info_block_t *dib;
	    ulong sector_size;
	    ulong num_sectors;
	    ulong num_spare_sectors;
	    ulong monlib_length;
	    ulong prog_algorithm;
	    ulong erased_state;
	    char volume_id[FSLIB_VOLUME_ID_MAX];
	    int actual;
	    int dev_num;
	    int confirmed;

	    dib = &device_info_block;

	    dev_num = rdtty_dev_num();
	    sector_size = rdtty_hex("Enter sector size",0,0x100000);
	    num_sectors = rdtty_dec("Enter number of sectors",0,4096);
	    num_spare_sectors = rdtty_dec("Enter number of spare sectors",0,16);
	    monlib_length = 0;
	    if (dev_num == 2) {
		prog_algorithm = FSLIB_PROG_RSP_INTERNAL_FLASH;
	    } else {
		prog_algorithm = FSLIB_PROG_INTEL_SERIES_2PLUS_CARD;
	    }
	    erased_state = 0xffffffff;
	    printf("Enter volume ID (up to %d chars): ",sizeof(dib->volume_id));
	    rdtty(volume_id, sizeof(volume_id));

	    printf("building DIB\n");
	    FSLIB_build_dib(dib,
			    sector_size,
			    num_sectors,
			    num_spare_sectors,
			    monlib_length,
			    prog_algorithm,
			    erased_state,
			    volume_id);

	    print_dib(dib);
	    confirmed = rdtty_confirmed(NULL);
	    if (confirmed) {
		printf("initializing RALIB device\n");
		rc = ralib_device_init(dev_num);
		printf("initializing RALIB device -- rc = %d\n",rc);
		
		printf("assuming entire device is erased!\n");
		printf("writing DIB to device\n");
		if (dev_num == 2) {
		    actual = rsp_internal_flash_write_fn(dev_num,
							 (char *)dib,
							 0,
							 sizeof(fslib_device_info_block_t));
		} else {
		    actual = intel_series_2plus_write_fn(dev_num,
							 (char *)dib,
							 0,
							 sizeof(fslib_device_info_block_t));
		}
		if (actual == sizeof(fslib_device_info_block_t)) {
		    printf("Success!\n");
		} else {
		    printf("error writing DIB -- actual = %d (should be %d)\n",actual,sizeof(fslib_device_info_block_t));
		}
	    }
	    continue;
	}

	/*------------------------------------------------------------*
	 * PCMCIA utilities
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"pcmcia cmd") == 0) {
	    int dev_num;
	    ushort cmd;
	    ulong offset;
	    ulong pcmcia_type;

	    dev_num = rdtty_dec("Enter device (0=slot0,1=slot1)",0,1);
	    offset = rdtty_hex("Enter offset into PCMCIA address space",0,0);
	    cmd = rdtty_hex("Enter command",0,0);
	    pcmcia_type = rdtty_dec("Enter type (0=Common,1=Attrib)",
				    CLPD6720_COMMON_MEM,CLPD6720_ATTRIB_MEM);
	    if (pcmcia_type == CLPD6720_COMMON_MEM) {
		ralib_write(dev_num,&cmd,offset,sizeof(ushort));
	    } else {
		ralib_attrib_write(dev_num,&cmd,offset,sizeof(ushort));
	    }
	    continue;
	}

	if (strcmp(inbuf,"pcmcia dump") == 0) {
	    int socket;
	    int window;
	    ulong window_size;
	    ulong local_addr;
	    ulong local_base_addr;
	    ulong pcmcia_addr;
	    ulong pcmcia_base;
	    ulong pcmcia_offset;
	    ulong socket_offset;
	    ulong length;
	    clpd6720_pcmcia_type_t pcmcia_type;
	    
            socket = rdtty_socket();
	    window = CLPD6720_WINDOW_3;
	    window_size = 0x00100000;
	    pcmcia_addr = rdtty_hex("Enter PCMCIA ADDR",0,0x03ffffff);
	    pcmcia_base = pcmcia_addr & 0x03f00000;
	    pcmcia_offset = pcmcia_addr & 0x000fffff;
	    length = rdtty_hex("Enter LENGTH",0,0x000fffff);
	    if ((length + pcmcia_offset) > window_size) {
		length = window_size - pcmcia_offset;
	    }
	    pcmcia_type = rdtty_dec("Enter type (0=Common,1=Attrib)",
				    CLPD6720_COMMON_MEM,CLPD6720_ATTRIB_MEM);
	    if (socket == CLPD6720_SOCKET_0) {
		socket_offset = 0x100000;
	    } else {
		socket_offset = 0x900000;
	    }
	    clpd6720_set_window(socket,
				window,
				socket_offset, /* local addr (ISA)
						  offset = 1MB or 9MB */
				pcmcia_base,
				0x00100000, /* length = 1MB */
				pcmcia_type,
				CLPD6720_WIDTH_16_BIT,
				CLPD6720_READ_ONLY);
	    clpd6720_enable_window((uchar)socket,window);
	    printf("Dumping: skt=%d, PCMCIA Addr=%lx, len=%lx\n",socket,pcmcia_addr,length);
	    printf("  Using window 3\n");
  	    local_base_addr = ralib_get_pcmcia_base_local_addr(socket);
            if (! local_base_addr)
            {
                printf("ralib_get_pcmcia_base_local_addr(%d) returned 0.  "
                       "Unable to dump\n",
                       socket);
                continue;
            }
            local_addr = local_base_addr + socket_offset + pcmcia_offset;
	    ushort_dump((ushort *)local_addr,length);
	    continue;
	}

	if (strcmp(inbuf,"isa data dump") == 0) {
	    ulong length;
	    ulong local_addr;
	    ulong local_base_addr;
	    int socket;

            socket = rdtty_socket();
            
	    local_base_addr = ralib_get_pcmcia_base_local_addr(socket);
            if (! local_base_addr)
            {
                printf("ralib_get_pcmcia_base_local_addr(%d) returned 0.  "
                       "Unable to dump\n",
                       socket);
                continue;
            }
	    local_addr = rdtty_hex("Enter LOCAL (ISA) ADDR",0,0x00ffffff);
	    local_addr &= 0x00fffffe;
	    local_addr |= local_base_addr;
	    length = rdtty_hex("Enter LENGTH",0,0x00ffffff);
	    length = ((length + 1) / 2) * 2;
	    printf("Dumping ISA Address %lx (%lx bytes)\n",local_addr,length);
	    ushort_dump((ushort *)local_addr, length);
	    continue;
	}

	if (strcmp(inbuf,"flash dump") == 0) {
	    ulong flash_offset;
	    ulong flash_addr;
	    ulong length;
	    
	    flash_offset = rdtty_hex("Enter FLASH OFFSET",0,0x03ffffff);
	    flash_offset &= 0xfffffffc;
	    flash_addr = flash_offset + ralib_get_flash_base_local_addr();
	    length = rdtty_hex("Enter LENGTH",0,0x0003fffff);
	    length = (length + 3) & 0xfffffffc;
	    printf("Dumping: Flash Addr=%lx, len=%lx\n",flash_addr,length);
	    ulong_dump((ulong *)flash_addr,length);
	    continue;
	}

	if (strcmp(inbuf,"flash cmd") == 0) {
	    int dev_num;
	    ulong cmd;
	    ulong offset;

	    dev_num = 2;
	    offset = rdtty_hex("Enter offset into flash address space",0,0);
	    cmd = rdtty_hex("Enter command (32-bit)",0,0);
	    ralib_write(dev_num,&cmd,offset,sizeof(cmd));
	    continue;
	}

	/*------------------------------------------------------------*
	 * CLPD6720 utilities
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"clpd reg dump") == 0) {
	    int socket;
	    uchar r;

	    socket = rdtty_socket();
	    for (r = 0; r <= 0x3f; r++) {
		volatile uchar value;
		value = clpd6720_reg_read((uchar)socket,r);
	    }
	    
	    continue;
	}

	if (strcmp(inbuf,"clpd data dump") == 0) {

	    printf("NOT IMPLEMENTED\n");
	    continue;
	}

	if (strcmp(inbuf,"clpd reg read") == 0) {
	    int socket;
	    int index;
	    int value;

	    socket = rdtty_socket();
	    index = rdtty_dec("Enter register index in decimal",0,31);
	    value = clpd6720_reg_read(socket,(uchar)index);
	    printf("Socket %d, Index %d = %x\n",socket, index, value);
	    continue;
	}

	if (strcmp(inbuf,"clpd reg write") == 0) {
	    int socket;
	    int index;
	    int value;

	    socket = rdtty_socket();
	    index = rdtty_dec("Enter register index in decimal",0,31);
	    value = rdtty_hex("Enter register value in hex",0,0x000000ff);
	    clpd6720_reg_write(socket,(uchar)index,(uchar)value);
	    continue;
	}

	if (strcmp(inbuf,"clpd interrupt") == 0) {
	    clpd6720_mgmt_intr_handler();
	    continue;
	}

	if (strcmp(inbuf,"clpd init") == 0) {
	    clpd6720_init();
	    continue;
	}

	if (strcmp(inbuf,"clpd reset socket") == 0) {
	    int socket;

	    socket = rdtty_socket();
	    clpd6720_reset_socket(socket);
	    continue;
	}

	if (strcmp(inbuf,"clpd set window") == 0) {
	    int socket;
	    int window;
	    ulong local_addr;
	    ulong pcmcia_addr;
	    ulong length;
	    clpd6720_pcmcia_type_t pcmcia_type;
	    
            socket = rdtty_socket();
            window = rdtty_window();
	    local_addr = rdtty_hex("Enter LOCAL (ISA) ADDR",0,0x00ffffff);
	    pcmcia_addr = rdtty_hex("Enter PCMCIA ADDR",0,0x03ffffff);
	    length = rdtty_hex("Enter LENGTH",0,0x00ffffff);
	    pcmcia_type = rdtty_dec("Enter type (0=Common,1=Attrib)",
				    CLPD6720_COMMON_MEM,CLPD6720_ATTRIB_MEM);
	    clpd6720_set_window(socket,
				window,
				local_addr,
				pcmcia_addr,
				length,
				pcmcia_type,
				CLPD6720_WIDTH_16_BIT,
				CLPD6720_READ_ONLY);
	    continue;
	}

	if (strcmp(inbuf,"clpd enable window") == 0) {
	    int socket;
	    int window;
	    
	    socket = rdtty_socket();
	    window = rdtty_window();
	    clpd6720_enable_window((uchar)socket,(uchar)window);
	    continue;
	}

	if (strcmp(inbuf,"clpd disable window") == 0) {
	    int socket;
	    int window;
	    
	    socket = rdtty_socket();
	    window = rdtty_window();
	    clpd6720_disable_window((uchar)socket,(uchar)window);
	    continue;
	}

	/*------------------------------------------------------------*
	 * Card utilities
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"reset socket") == 0) {
	    int socket;
	    int rc;

	    socket = rdtty_socket();
	    rc = ralib_control(socket,RALIB_RESET_DEVICE);
	    if (rc == RALIB_OK)
		printf("Reset successful\n");
	    else
		printf("Reset ERROR -- return code = %d\n",rc);

	    continue;
	}

	/*------------------------------------------------------------*
	 * Bootflash utilities
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"bootflash reg dump") == 0) {
	    int dev_num;

	    dev_num = 2;
	    rsp_internal_flash_show_info_fn(dev_num);
	    continue;
	}

	/*------------------------------------------------------------*
	 * Intel Flash card utilities
	 *------------------------------------------------------------*/

	if (strcmp(inbuf,"intel reg dump") == 0) {
	    int dev_num;

	    dev_num = rdtty_dec("Enter device (0=slot0,1=slot1)",0,1);
	    intel_series_2plus_show_info_fn(dev_num);
	    continue;
	}

	if (strcmp(inbuf,"intel turn vpp on") == 0) {
	    uchar voltage_cmd;
	    int voltage_actual;
	    
	    /*
	     * Turn on the programming voltage and wait for it to get up to
	     * the required voltage.
	     */
	    voltage_cmd = IFLASH_VOLTCTRL_VPP_GEN;
	    voltage_actual = FSLIB_ralib->attrib_write(0,  /* socket 0 */
						       &voltage_cmd,
						       IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
						       sizeof(voltage_cmd));
	    if (voltage_actual != sizeof(voltage_cmd)) {
		printf("Error writing to PCMCIA card to turn Vpp on -- actual=%d\n",voltage_actual);
	    } else {
		printf("OK\n");
	    }
	    continue;
	}

	if (strcmp(inbuf,"intel turn vpp off") == 0) {
	    uchar voltage_cmd;
	    int voltage_actual;
	    
	    /*
	     * Turn on the programming voltage and wait for it to get up to
	     * the required voltage.
	     */
	    voltage_cmd = 0;
	    voltage_actual = FSLIB_ralib->attrib_write(0,  /* socket 0 */
						       &voltage_cmd,
						       IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
						       sizeof(voltage_cmd));
	    if (voltage_actual != sizeof(voltage_cmd)) {
		printf("Error writing to PCMCIA card to turn Vpp off -- actual=%d\n",voltage_actual);
	    } else {
		printf("OK\n");
	    }
	    continue;
	}

	if (strcmp(inbuf,"intel poll vpp") == 0) {
	    uchar voltage_cmd;
	    uchar voltage_status;
	    int   voltage_actual;

	    voltage_cmd = 0;
	    voltage_actual = FSLIB_ralib->attrib_read(voltage_cmd,
					       IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,
					       &voltage_status,
					       sizeof(voltage_status));
	    if (voltage_actual != sizeof(voltage_cmd)) {
		printf("Error reading Vpp status -- actual=%d\n",voltage_actual);
	    } else {
		printf("Vpp status word at %x = %x\n",IFLASH_VOLTAGE_CONTROL_ATTRIB_OFFSET,voltage_status);
		if (voltage_status & IFLASH_VOLTCTRL_VPP_VALID) {
		    printf("Vpp voltage is ON\n");
		} else {
		    printf("Vpp voltage is OFF\n");
		}
	    }
	    continue;
	}

	if (strcmp(inbuf,"intel unlock blocks") == 0) {
	    uchar unlock_cmd;
	    int dev_num;

	    dev_num = rdtty_dec("Enter device (0=slot0,1=slot1",0,1);
	    unlock_cmd = 0;
	    ralib_attrib_write(dev_num,&unlock_cmd,IFLASH_WRITE_PROTECT_ATTRIB_OFFSET,sizeof(unlock_cmd));
	    continue;
	}

	/*------------------------------------------------------------*
	 * Yan's utilities
	 *------------------------------------------------------------*/

	{
	    extern int flash_config_print;
	    
	    if (strcmp(inbuf, "debug flash config on") == 0) {
		flash_config_print = TRUE;
		continue;
	    } else if (strcmp(inbuf, "debug flash config off") == 0) {
		flash_config_print = FALSE;
		continue;
	    }
	}
	
	/* 
	 * if the command is not recognized, print a message
	 */
	printf("UNRECOGNIZED COMMAND\n");

    }

    free(rd_buffer);
    free(wr_buffer);


}





