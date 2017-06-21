/* $Id: unix_fslib_test.c,v 3.2.60.1 1996/03/18 19:36:21 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/unix_fslib_test.c,v $
 *------------------------------------------------------------------
 * Test driver for the Unix stand-alone flash file system test 
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_fslib_test.c,v $
 * Revision 3.2.60.1  1996/03/18  19:36:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:09:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:33:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_dev.h"
#include "fslib_dev_intel_series_2plus.h"
#include "fslib_dev_rsp_internal_flash.h"
#include "fslib_print.h"

#include "clpd6720.h"

#include <stdlib.h>

static char *(device_id[]) = {"slot0","slot1","bootflash"};

/******************************************************************************
 * Name: rdtty
 *
 *******************************************************************************/
static int rdtty (char *buffer, int size) {

    gets(buffer,size);
    return(1);

}

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
 * Name: rdtty_dec
 *
 *******************************************************************************/
static int rdtty_dec(char *prompt, int low, int high) {
    static char inbuf[128];
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
	 * do the conversion
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
    static char inbuf[128];
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
	if (((low == 0) && (high != 0)) || ((hex >= low) && (hex <= high)))
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
	printf("%s (<device id>:<file name>):",prompt);
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
    static char inbuf[128];
    int i;
    int parsed;

    while (1) {
	printf("Enter device ID (slot0,slot1,flash):");
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
 * Name: do_dir_1
 *
 * Description:
 *
 ******************************************************************************/
static void do_dir_1(char *dev_id)
{
    static char dbuffer[sizeof(dirent_t)];
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
    static char dbuffer[sizeof(dirent_t) * 1000];
    int dfd;
    int actual;
    dirent_t *de;
    char file_name[32];

    strcpy(file_name,dev_id);
    strcat(file_name,":.");

    dfd = FS_open(file_name,O_RDONLY,0);
    if (dfd <= 0) {
	printf("Error opening directory\n");
    } else {
	print_dirent_header();
	actual = FS_getdents(dfd,dbuffer,sizeof(dbuffer));
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
}

/******************************************************************************
 * Name: fslib_test_main
 *
 *******************************************************************************/
void fslib_test_main(void)
{
    int i = 0;
    int rc = 0;
    int actual = 0;
    static ulong wr_buffer[32768]; /* XXX jack make this smaller later */
    static ulong rd_buffer[32768]; /* XXX jack make this smaller later */
    static char inbuf[256];
    static char inbuf_copy[256];
    int parsed = 0;
    stat_t stat;
    stat_t *st;

    st = &stat;

    for (i = 0; i < 1024; i++) {
	wr_buffer[i] = i + (i << 16);
	rd_buffer[i] = 0;
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

	if (strcmp(inbuf,"exit") == 0) {
	    break;
	}

	if (strcmp(inbuf,"quit") == 0) {
	    break;
	}

	if (strcmp(inbuf,"init") == 0) {
	    FS_init();
#if defined(STANDALONE)
	    ralib_unix_set_file_name(2,"slot0");
	    ralib_insert_device(2);
	    ralib_unix_set_file_name(3,"slot1");
	    ralib_insert_device(3);
#endif /* defined(STANDALONE) */
	    continue;
	}

	if (strcmp(inbuf,"term") == 0) {
	    printf("NOT IMPLEMENTED\n");
	    continue;
	}

	if (strcmp(inbuf,"remove card") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    ralib_remove_device(ralib_get_dev_num(dev_id));
	    continue;
	}

	if (strcmp(inbuf,"insert card") == 0) {
	    char *dev_id;

	    dev_id = rdtty_dev_id();
	    ralib_insert_device(ralib_get_dev_num(dev_id));
	    continue;
	}

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

	if (strcmp(inbuf,"print files") == 0) {
	    print_file_recs(1);
	    continue;
	}

	if (strcmp(inbuf,"print dev") == 0) {
	    print_dev(&FSLIB_device_table[0],1);
	    continue;
	}

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
		printf("ERROR opening file -- rc = %d\n",fd);
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

	    fd = rdtty_dec("Enter file descriptor",0,0);
	    length = rdtty_hex("Enter number of bytes to read",0,sizeof(rd_buffer));
	    actual = FS_read(fd,rd_buffer,length);
	    ulong_dump(rd_buffer,actual);
	    continue;
	}

	if (strcmp(inbuf,"write file") == 0) {
	    int length;
	    int fd;

	    fd = rdtty_dec("Enter file descriptor",0,0);
	    length = rdtty_hex("Enter number of bytes to write",0,sizeof(wr_buffer));
	    FS_write(fd,wr_buffer,length);
	    continue;
	}

	if (strcmp(inbuf,"copy to file") == 0) {
	    int length;
	    int fd;
	    static char unix_file_name[128];
	    static char fslib_file_name[128];
	    static char copy_buffer[4096];
	    FILE *unix_stream;
	    int fslib_fd;
	    int error;
	    int unix_actual;
	    int fslib_actual;
	    
	    error = FALSE;
	    if (!error) {
		printf("\nEnter name of Unix file (source): ");
		rdtty(unix_file_name,sizeof(unix_file_name));
		unix_stream = fopen(unix_file_name,"r");
		if (unix_stream == NULL) {
		    printf("Could not open Unix file %s\n",unix_file_name);
		    error = TRUE;
		}
	    }
	    
	    if (!error) {
		printf("\nEnter name of FSLIB file (dest): ");
		rdtty(fslib_file_name,sizeof(fslib_file_name));
		fslib_fd = FS_open(fslib_file_name,O_WRONLY|O_APPEND|O_TRUNC|O_CREAT,0);
		if (fslib_fd < 0) {
		    printf("Could not open/create FSLIB file %s\n",fslib_file_name);
		    continue;
		    error = TRUE;
		}
	    }

	    while (!error) {
		unix_actual = fread(copy_buffer,1,sizeof(copy_buffer),unix_stream);
		fslib_actual = FS_write(fslib_fd,copy_buffer,unix_actual);
		if (unix_actual != fslib_actual) {
		    printf("**ERROR** Error copying file (unix actual=%d, fslib actual=%d\n",
			   unix_actual,fslib_actual);
		    error = TRUE;
		}
		if (unix_actual != sizeof(copy_buffer)) {
		    break;
		}
	    }

	    fclose(unix_stream);
	    FS_close(fslib_fd);
	    continue;
	}

	if (strcmp(inbuf,"copy from file") == 0) {
	    int length;
	    int fd;
	    static char unix_file_name[128];
	    static char fslib_file_name[128];
	    static char copy_buffer[4096];
	    FILE *unix_stream;
	    int fslib_fd;
	    int error;
	    int unix_actual;
	    int fslib_actual;
	    
	    error = FALSE;
	    if (!error) {
		printf("\nEnter name of FSLIB file (source): ");
		rdtty(fslib_file_name,sizeof(fslib_file_name));
		fslib_fd = FS_open(fslib_file_name,O_RDONLY,0);
		if (fslib_fd < 0) {
		    printf("Could not open/create FSLIB file %s\n",fslib_file_name);
		    continue;
		    error = TRUE;
		}
	    }

	    if (!error) {
		printf("\nEnter name of Unix file (dest): ");
		rdtty(unix_file_name,sizeof(unix_file_name));
		unix_stream = fopen(unix_file_name,"r+");
		if (unix_stream == NULL) {
		    printf("Could not open Unix file %s\n",unix_file_name);
		    error = TRUE;
		}
	    }
	    
	    while (!error) {
		fslib_actual = FS_read(fslib_fd,copy_buffer,sizeof(copy_buffer));
		unix_actual = fwrite(copy_buffer,1,fslib_actual,unix_stream);
		if (unix_actual != fslib_actual) {
		    printf("**ERROR** Error copying file (fslib actual=%d, unix actual=%d\n",
			   fslib_actual,unix_actual);
		    error = TRUE;
		}
		if (fslib_actual != sizeof(copy_buffer)) {
		    break;
		}
	    }

	    fclose(unix_stream);
	    FS_close(fslib_fd);
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

	if (strcmp(inbuf,"o1") == 0) {
	    int fd;

	    fd = FS_open("this file ID is invalid",(O_WRONLY|O_APPEND|O_TRUNC|O_CREAT),0);
	    continue;
	}

	if (strcmp(inbuf,"o2") == 0) {
	    int fd;

	    fd = FS_open("slot0:file1",(O_WRONLY|O_APPEND|O_TRUNC|O_CREAT) + 100,0);
	    continue;
	}

	if (strcmp(inbuf,"o7") == 0) {
	    int fd;

	    fd = FS_open("slot0:.",O_RDONLY,0);
	    continue;
	}

	if (strcmp(inbuf,"o8") == 0) {
	    int fd;

	    fd = FS_open("slot0:.",(O_WRONLY|O_APPEND|O_TRUNC|O_CREAT),0);
	    continue;
	}

	if (strcmp(inbuf,"o10") == 0) {
	    int fd;

	    fd = FS_open("slot0:unknown_file",(O_WRONLY|O_APPEND|O_TRUNC),0);
	    continue;
	}

	if (strcmp(inbuf,"o12") == 0) {
	    int fd;

	    fd = FS_open("slot0:new_file",(O_WRONLY|O_APPEND|O_TRUNC|O_CREAT),0);
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

	/* 
	 * if the command is not recognized, pass it to the shell if
	 * in standalone mode
	 */
#if defined(STANDALONE)	
	system(inbuf);
#else
	printf("UNRECOGNIZED COMMAND\n");
#endif

    }

}

/******************************************************************************
 * Name: main
 *
 *******************************************************************************/
main()
{
    fslib_test_main();
}
