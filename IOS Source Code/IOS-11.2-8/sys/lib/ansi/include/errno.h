/* $Id: errno.h,v 3.1.10.1 1996/03/18 20:48:40 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/include/errno.h,v $
 *------------------------------------------------------------------
 * errno.h -- Error codes
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: errno.h,v $
 * Revision 3.1.10.1  1996/03/18  20:48:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:04:06  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

/* Define error codes that we pass to the NCD for file read errors */

#define ENOENT          2       /* No such file or directory */
#define EIO             5       /* I/O error */
#define ENOMEM          12      /* Not enough memory */
#define EFBIG           27      /* File too large */

/* The ETIMEDOUT macro is NOT a POSIX standard macro value */
#if 0
#define ETIMEDOUT       60      /* Connection timed out */
#endif
#define ETIMEDOUT       145     /* Connection timed out */

/* math software */
#define EDOM            33      /* Argument too large */
#define ERANGE          34      /* Result too large */

/* POSIX mandated values */
#define EPERM           1       /* Not super-user                       */
#define ESRCH           3       /* No such process                      */
#define EINTR           4       /* interrupted system call              */
#define ENXIO           6       /* No such device or address            */
#define E2BIG           7       /* Arg list too long                    */
#define ENOEXEC         8       /* Exec format error                    */
#define EBADF           9       /* Bad file number                      */
#define ECHILD          10      /* No children                          */
#define EAGAIN          11      /* Resource temporarily unavailable     */
#define EACCES          13      /* Permission denied                    */
#define EFAULT          14      /* Bad address                          */
#define EBUSY           16      /* Device busy                          */
#define EEXIST          17      /* File exists                          */
#define EXDEV           18      /* Cross-device link                    */
#define ENODEV          19      /* No such device                       */
#define ENOTDIR         20      /* Not a directory                      */
#define EISDIR          21      /* Is a directory                       */
#define EINVAL          22      /* Invalid argument                     */
#define ENFILE          23      /* File table overflow                  */
#define EMFILE          24      /* Too many open files                  */
#define ENOTTY          25      /* Inappropriate ioctl for device       */
#define ENOSPC          28      /* No space left on device              */
#define ESPIPE          29      /* Illegal seek                         */
#define EROFS           30      /* Read only file system                */
#define EMLINK          31      /* Too many links                       */
#define EPIPE           32      /* Broken pipe                          */
#define EDEADLK         45      /* Deadlock condition.                  */
#define ENOLCK          46      /* No record locks available.           */
#define ENAMETOOLONG    78      /* path name is too long                */
#define ENOSYS          89      /* Unsupported file system operation    */
#define ENOTEMPTY       93      /* directory not empty                  */

#define errno (*__errno())
int *__errno(void);             /* reentrant version */

#endif /* __ERRNO_H__ */
