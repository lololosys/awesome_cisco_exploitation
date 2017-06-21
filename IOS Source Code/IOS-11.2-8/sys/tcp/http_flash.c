/* $Id: http_flash.c,v 3.1.2.5 1996/08/01 08:46:48 snijsure Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_flash.c,v $
 *------------------------------------------------------------------
 * http_flash.c -- HyperText Transfer Protocol, flash file support
 *
 * May 1996, Subodh Nijsure
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_flash.c,v $
 * Revision 3.1.2.5  1996/08/01  08:46:48  snijsure
 * CSCdi64824:  flash file retrieved via http still shows incorrect
 * contents.
 * Branch: California_branch
 * Fixed array index off by one error.
 *
 * Revision 3.1.2.4  1996/07/30  00:19:45  snijsure
 * CSCdi64235:  http sends incorrect text from files stored in flash.
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/07  19:47:56  snijsure
 * CSCdi59700:  System reloads accessing Ezsetup
 * Branch: California_branch
 * Fixed non-const literal string passing. Removed some unnecessary debug
 * messages. And code cleanup per code review comments.
 *
 * Revision 3.1.2.2  1996/06/06  06:24:42  pbopardi
 * Disbale Flash File system access for 1000 platforms
 * CSCdi59700:  System reloads accessing Ezsetup
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/22  04:45:48  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * CSCdi58178: HTTP problem with flash access
 *
 * Revision 3.1  1996/05/21  20:12:37  snijsure
 * Place holder for http_flash.c
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "ctype.h"
#include "stdarg.h"
#include "string.h"
#include "../ui/debug.h"
#include "ttysrv.h"
#include "../tcp/tcp.h"
#include "http.h"
#include "http_debug.h"
#include "../dev/file_io.h"
#include "../filesys/filesys.h"

#define FILE_IO_BUFFER_SIZE  512


/*
 * For certain platforms the flash file system access functions seems to be 
 * causing a system reload. CSCdi57527
 */
static boolean http_flash_disable_feature (void)
{
    boolean retval = FALSE;
    char *platform_string = platform_get_string(PLATFORM_STRING_NOM_DU_JOUR); 

    if (strcmp(platform_string, "RP1") == 0) {
       retval = TRUE;
    }

    return(retval);
}


/* 
 * open the file specified by 'fname' and send it.
 */
static boolean http_send_file (httpstate *state, const char *fname)
{
    char *extension, *filename, *bufp;
    int fd, cnt, error;
    stat_t fs;
    
    if ( fname != NULL )  { 
	filename = malloc(strlen(fname)+1);
	if ( filename == NULL )
	    return FALSE;
	strcpy(filename,fname);
    }
    else 
        return FALSE;

    fd = FS_open(filename, O_RDONLY,0);
    if (fd < 0) {
        if (http_uri_debug) 
            buginf("\nHTTP: File %s can not be opened.", filename); 
	free(filename);
	return FALSE;
    }

    /* We have successfully opened file */

    FS_stat(filename,&fs);
    if (http_uri_debug) 
        buginf("\nHTTP: Begin sending file %s, size (%d) bytes", 
		                         filename,fs.st_size); 
    /* 
     * Retrive the extension from uri_index and send it to 
     * http_send_prefix
     */
    if ( (extension = strchr(filename,'.')) == NULL) {
        http_send_prefix(state, NULL, fs.st_size);
    } else {
        extension++;   /* advance past '.' */
        http_send_prefix(state, extension, fs.st_size);
    }

    bufp = malloc( FILE_IO_BUFFER_SIZE  * sizeof(char));
    if ( bufp == NULL ) {
        free(filename);
        return FALSE;
    }

    for(;;) {
        cnt = FS_read(fd,bufp,FILE_IO_BUFFER_SIZE-1);
        if ( cnt <= 0 ) {
            break;
        }
	tcp_putstringblock(state->tcb,bufp,cnt,&error);
	tcp_putpush(state->tcb, TRUE, FALSE);
    }

    free(bufp);
    free(filename);

    FS_close(fd);
    http_page_complete(state);
    return TRUE;
}


/*
 * This function searches for file home.html in flash.
 * Currently the way search is done is based on a "guess".
 * 
 * i..e. we try and open file home.html or slot0:home.html or slot1:home.html
 * This is due to lack of common API to access flash file structure, which
 * will arrive soon. 
 */
boolean http_send_home_page (httpstate *state)
{
    if ( http_flash_disable_feature() ) {
	return FALSE;
    }

    /* Try different combinations of filename */ 
    if ( http_send_file(state,"home.html") ) {
	return TRUE;
    }
    else if (http_send_file(state,"flash:home.html")) {
	return TRUE;
    }
    else if (http_send_file(state,"slot0:home.html")) {
	return TRUE;
    }
    else if (http_send_file(state,"slot1:home.html")) {
	return TRUE;
    }
    return FALSE;
}



/*
 *  Open the flash file referenced by state->uri_index, and send its
 *  contents back to the requestor.
 *
 */
void http_send_flash_file (httpstate *state)
{
    char *filename; 
    state->uri_index++; /* Skip the leading '/ */


    if ( http_flash_disable_feature() ) {
        http_error(state, HTTP_STATUS_NOT_IMPLEMENTED);
	return;
    }

    if (http_send_file(state,state->uri_index) == FALSE) {
	 /* See if user specified fully qualified name, viz.
	  * slot0:filename.html, if not, add device_name: to the filename and 
	  * try and send it.
	  */
	 if (strchr(state->uri_index,':') == NULL) {

	     /* enough space for slot0:+length of uri_index */

	     filename = malloc(strlen(state->uri_index)+1+6); 
	     if (filename ==  NULL) {
                 http_error(state,HTTP_STATUS_INTERNAL_SERVER_ERROR);
		 return;
	     }

	     /* Again no portable way to figure out # of slots! */
             sprintf(filename,"slot0:%s",state->uri_index);
             if ( http_send_file(state,filename) == TRUE ) {
		 free(filename);
		 return;
	     }

             sprintf(filename,"slot1:%s",state->uri_index);
             if ( http_send_file(state,filename) == TRUE ) {
		 free(filename);
		 return;
	     }
             sprintf(filename,"flash:%s",state->uri_index);
             if ( http_send_file(state,filename) == TRUE ) {
		 free(filename);
		 return;
	     }
	     free(filename);
	  }
          http_send_not_found(state,NULL);
     }
}

