/* $Id: strerror.c,v 3.1.10.1 1996/03/18 20:49:32 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/string/strerror.c,v $
 *------------------------------------------------------------------
 * strerror.c -- convert error number to string
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: strerror.c,v $
 * Revision 3.1.10.1  1996/03/18  20:49:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:40:00  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***
**** CAUTION!!! KEEP DOC CONSISTENT---if you change text of a message
****            here, change two places:
****            1) the leading doc section (alphabetized by macro)
****            2) the real text inside switch(errnum)
***/

/*
FUNCTION
        strerror---convert error number to string

ANSI_SYNOPSIS
        #include <string.h>
        char *strerror(int errnum);

DESCRIPTION
        strerror converts the error number errnum into a string.  The
        value of errnum is usually a copy of errno.  If errnum is not
        a known error number, the result points to an empty string.

        This implementation of strerror prints out the following
        strings for each of the values defined in `errno.h':

        E2BIG
        Arg list too long

        EACCES
        Permission denied

        EADV
        Advertise error

        EAGAIN
        No more processes

        EBADF
        Bad file number

        EBADMSG
        Bad message

        EBUSY
        Device or resource busy

        ECHILD
        No children

        ECOMM
        Communication error

        EDEADLK
        Deadlock

        EEXIST
        File exists

        EDOM
        Math argument

        EFAULT
        Bad address

        EFBIG
        File too large

        EIDRM
        Identifier removed

        EINTR
        Interrupted system call

        EINVAL
        Invalid argument

        EIO
        I/O error

        EISDIR
        Is a directory

        ELIBACC
        Cannot access a needed shared library

        ELIBBAD
        Accessing a corrupted shared library

        ELIBEXEC
        Cannot exec a shared library directly

        ELIBMAX
        Attempting to link in more shared libraries than system limit

        ELIBSCN
        .lib section in a.out corrupted

        EMFILE
        Too many open files

        EMLINK
        Too many links

        EMULTIHOP
        Multihop attempted

        ENFILE
        File table overflow

        ENODEV
        No such device

        ENOENT
        No such file or directory

        ENOEXEC
        Exec format error

        ENOLCK
        No lock

        ENOLINK
        Virtual circuit is gone

        ENOMEM
        Not enough space

        ENOMSG
        No message of desired type

        ENONET
        Machine is not on the network

        ENOPKG
        No package

        ENOSPC
        No space left on device

        ENOSR
        No stream resources

        ENOSTR
        Not a stream

        ENOTBLK
        Block device required

        ENOTDIR
        Not a directory

        ENOTTY
        Not a character device

        ENXIO
        No such device or address

        EPERM
        Not owner

        EPIPE
        Broken pipe

        EPROTO
        Protocol error

        ERANGE
        Result too large

        EREMOTE
        Resource is remote

        EROFS
        Read-only file system

        ESPIPE
        Illegal seek

        ESRCH
        No such process

        ESRMNT
        Srmount error

        ETIME
        Stream ioctl timeout

        ETXTBSY
        Text file busy

        EXDEV
        Cross-device link

RETURNS
        This function returns a pointer to a string.  Your application must
        not modify that string.

PORTABILITY
        ANSI C requires strerror, but does not specify the strings
        used for each error number.

        Although this implementation of strerror is reentrant, ANSI C
        declares that subsequent calls to strerror may overwrite the
        result string; therefore portable code cannot depend on the
        reentrancy of this subroutine.

        strerror requires no supporting OS subroutines.
*/

#include <errno.h>
#include <string.h>

char *
strerror (int errnum)
{
    char *error;

    switch (errnum) {
#ifdef EPERM
    case EPERM:
        error = "Not owner";
        break;
#endif /* EPERM */
#ifdef ENOENT
    case ENOENT:
        error = "No such file or directory";
        break;
#endif /* ENOENT */
#ifdef ESRCH
    case ESRCH:
        error = "No such process";
        break;
#endif /* ESRCH */
#ifdef EINTR
    case EINTR:
        error = "Interrupted system call";
        break;
#endif /* EINTR */
#ifdef EIO
    case EIO:
        error = "I/O error";
        break;
#endif /* EIO */
#ifdef ENXIO
    case ENXIO:
        error = "No such device or address";
        break;
#endif /* ENXIO */
#ifdef E2BIG
    case E2BIG:
        error = "Arg list too long";
        break;
#endif /* E2BIG */
#ifdef ENOEXEC
    case ENOEXEC:
        error = "Exec format error";
        break;
#endif /* ENOEXEC */
#ifdef EBADF
    case EBADF:
        error = "Bad file number";
        break;
#endif /* EBADF */
#ifdef ECHILD
    case ECHILD:
        error = "No children";
        break;
#endif /* ECHILD */
#ifdef EAGAIN
    case EAGAIN:
        error = "No more processes";
        break;
#endif /* EAGAIN */
#ifdef ENOMEM
    case ENOMEM:
        error = "Not enough space";
        break;
#endif /* ENOMEM */
#ifdef EACCES
    case EACCES:
        error = "Permission denied";
        break;
#endif /* EACCES */
#ifdef EFAULT
    case EFAULT:
        error = "Bad address";
        break;
#endif /* EFAULT */
#ifdef ENOTBLK
    case ENOTBLK:
        error = "Block device required";
        break;
#endif /* ENOTBLK */
#ifdef EBUSY
    case EBUSY:
        error = "Device or resource busy";
        break;
#endif /* EBUSY */
#ifdef EEXIST
    case EEXIST:
        error = "File exists";
        break;
#endif /* EEXIST */
#ifdef EXDEV
    case EXDEV:
        error = "Cross-device link";
        break;
#endif /* EXDEV */
#ifdef ENODEV
    case ENODEV:
        error = "No such device";
        break;
#endif /* ENODEV */
#ifdef ENOTDIR
    case ENOTDIR:
        error = "Not a directory";
        break;
#endif /* ENOTDIR */
#ifdef EISDIR
    case EISDIR:
        error = "Is a directory";
        break;
#endif /* EISDIR */
#ifdef EINVAL
    case EINVAL:
        error = "Invalid argument";
        break;
#endif /* EINVAL */
#ifdef ENFILE
    case ENFILE:
        error = "File table overflow";
        break;
#endif /* ENFILE */
#ifdef EMFILE
    case EMFILE:
        error = "Too many open files";
        break;
#endif /* EMFILE */
#ifdef ENOTTY
    case ENOTTY:
        error = "Not a character device";
        break;
#endif /* ENOTTY */
#ifdef ETXTBSY
    case ETXTBSY:
        error = "Text file busy";
        break;
#endif /* ETXTBSY */
#ifdef EFBIG
    case EFBIG:
        error = "File too large";
        break;
#endif /* EFBIG */
#ifdef ENOSPC
    case ENOSPC:
        error = "No space left on device";
        break;
#endif /* ENOSPC */
#ifdef ESPIPE
    case ESPIPE:
        error = "Illegal seek";
        break;
#endif /* ESPIPE */
#ifdef EROFS
    case EROFS:
        error = "Read-only file system";
        break;
#endif /* EROFS */
#ifdef EMLINK
    case EMLINK:
        error = "Too many links";
        break;
#endif /* EMLINK */
#ifdef EPIPE
    case EPIPE:
        error = "Broken pipe";
        break;
#endif /* EPIPE */
#ifdef EDOM
    case EDOM:
        error = "Math argument";
        break;
#endif /* EDOM */
#ifdef ERANGE
    case ERANGE:
        error = "Result too large";
        break;
#endif /* ERANGE */
#ifdef ENOMSG
    case ENOMSG:
        error = "No message of desired type";
        break;
#endif /* ENOMSG */
#ifdef EIDRM
    case EIDRM:
        error = "Identifier removed";
        break;
#endif /* EIDRM */
#ifdef EDEADLK
    case EDEADLK:
        error = "Deadlock";
        break;
#endif /* EDEADLK */
#ifdef ENOLCK
    case ENOLCK:
        error = "No lock";
        break;
#endif /* ENOLCK */
#ifdef ENOSTR
    case ENOSTR:
        error = "Not a stream";
        break;
#endif /* ENOSTR */
#ifdef ETIME
    case ETIME:
        error = "Stream ioctl timeout";
        break;
#endif /* ETIME */
#ifdef ENOSR
    case ENOSR:
        error = "No stream resources";
        break;
#endif /* ENOSR */
#ifdef ENONET
    case ENONET:
        error = "Machine is not on the network";
        break;
#endif /* ENONET */
#ifdef ENOPKG
    case ENOPKG:
        error = "No package";
        break;
#endif /* ENOPKG */
#ifdef EREMOTE
    case EREMOTE:
        error = "Resource is remote";
        break;
#endif /* EREMOTE */
#ifdef ENOLINK
    case ENOLINK:
        error = "Virtual circuit is gone";
        break;
#endif /* ENOLINK */
#ifdef EADV
    case EADV:
        error = "Advertise error";
        break;
#endif /* EADV */
#ifdef ESRMNT
    case ESRMNT:
        error = "Srmount error";
        break;
#endif /* ESRMNT */
#ifdef ECOMM
    case ECOMM:
        error = "Communication error";
        break;
#endif /* ECOMM */
#ifdef EPROTO
    case EPROTO:
        error = "Protocol error";
        break;
#endif /* EPROTO */
#ifdef EMULTIHOP
    case EMULTIHOP:
        error = "Multihop attempted";
        break;
#endif /* EMULTIHOP */
#ifdef EBADMSG
    case EBADMSG:
        error = "Bad message";
        break;
#endif /* EBADMSG */
#ifdef ELIBACC
    case ELIBACC:
        error = "Cannot access a needed shared library";
        break;
#endif /* ELIBACC */
#ifdef ELIBBAD
    case ELIBBAD:
        error = "Accessing a corrupted shared library";
        break;
#endif /* ELIBBAD */
#ifdef ELIBSCN
    case ELIBSCN:
        error = ".lib section in a.out corrupted";
        break;
#endif /* ELIBSCN */
#ifdef ELIBMAX
    case ELIBMAX:
        error = "Attempting to link in more shared libraries than system limit";
        break;
#endif /* ELIBMAX */
#ifdef ELIBEXEC
    case ELIBEXEC:
        error = "Cannot exec a shared library directly";
        break;
#endif /* ELIBEXEC */
    default:
        error = "";
        break;
    }

    return error;
}
