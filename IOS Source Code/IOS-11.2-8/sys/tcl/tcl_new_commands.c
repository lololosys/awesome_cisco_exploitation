/* $Id: tcl_new_commands.c,v 3.2.62.1 1996/03/18 22:13:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tcl/tcl_new_commands.c,v $
 *------------------------------------------------------------------
 * tcl_new_commands.c -- Procedures for new commands.
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcl_new_commands.c,v $
 * Revision 3.2.62.1  1996/03/18  22:13:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:52:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:20:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:47:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:04:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "config.h"
#include "file.h"
#include "parser.h"
#include "name.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcl/tcl.h"
#include "../tcl/tcl_init.h"
#include "../tcl/tcl_new_commands.h"

#define FILESIZE 65536

extern unsigned long int strtoul( const char *, char **, int);

int Tcl_LoadScriptsCmd (ClientData clientData, Tcl_Interp *interp, int
			argc, char **argv)
{
    ulong bufsize;
    uchar *tftpbuf;            /* Buffer we TFTP the file into */
    filetype file;
    char *filename;
    addrtype host;
    int err;
    nametype *ptr;
    int i, j;
    char *cmd;
    int tcl_result;
    
    Tcl_DString command;     /* Used to buffer incomplete commands being
			      * read from stdin. */
    
    if ((argc < 3) || (argc > 4)){
	Tcl_AppendResult(interp, "wrong # args: should be \"",argv[0],
			 " fileName ipAddress/machineName ?fileSize?\"",
			 NULL);
	return TCL_ERROR;
    }
    
    filename = argv[1];
    if (!numeric_lookup(ADDR_IP, argv[2], &host)) {
	ptr = name_lookup(argv[2], &err, NAM_IP);
	if (ptr) {
	    address_copy(&host, &ptr->v.ip.adr[0]);
	} else {
	    Tcl_AppendResult(interp, "\n?Bad address or host name  ",
			     argv[2], NULL);
	    return TCL_ERROR;
        }
    }
    
    /*
     * Find out the size of the buffer to be allocated.
     * default is 64 K.
     */
    
    if (argc == 4) {
	bufsize = strtoul(argv[3], NULL, 10);
	if (bufsize <= 0) {
	    Tcl_AppendResult(interp, "Non numeric size  ", 
			     argv[3], NULL);
	    return TCL_ERROR;
	}
    } else {
	bufsize = FILESIZE;
    }
    
    /*
     * allocate space for the file to be tftp'ed.
     */
    tftpbuf = malloc(bufsize);
    
    /*
     * Read in the file.
     */
    file.proto = FILE_ACCESS_TFTP;
    bcopy(&host, &file.ps.tuba.boothost, sizeof(addrtype));
    file.ps.tuba.boothost.type = ADDR_IP;
    file.filename = filename;
    if (!reg_invoke_file_read(file.proto, &file, tftpbuf, &bufsize,
			      TRUE, NULL, NULL)) {
	Tcl_AppendResult(interp, "\nTFTP transfer failed", NULL);
	free(tftpbuf);
	return TCL_ERROR;
    }
    
    /*
     * source the read file into the interpreter.
     */
    tftpbuf[bufsize] = 0;
    i = 0;
    Tcl_DStringInit(&command);
    while (tftpbuf[i] != 0) {
	j = 0;
	interp->csb->line = interp->csb->line_buffer;
	do {
	    interp->csb->line_buffer[j++] = tftpbuf[i++];
	}
	while ((tftpbuf[i - 1] != 0) && (tftpbuf[i - 1] != '\n'));
	
	interp->csb->line_buffer[j] = 0;
	
	cmd = Tcl_DStringAppend(&command, interp->csb->line, -1);
	if (Tcl_CommandComplete(cmd)) {
	    tcl_result = Tcl_RecordAndEval(interp, cmd, 0 );
	    if (tcl_result != TCL_OK) {
		Tcl_AppendResult(interp, "\nError in TFTPed file", cmd,
				 NULL);
		free(tftpbuf);
		return TCL_ERROR;
	    }
	    Tcl_DStringInit(&command);
	}
    }
    Tcl_DStringInit(&command);
    free(tftpbuf);
    return TCL_OK;
}
