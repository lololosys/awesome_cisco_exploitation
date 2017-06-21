/* $Id: cfg_memory.h,v 3.2.62.1 1996/08/12 16:04:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_memory.h,v $
 *------------------------------------------------------------------
 * Memory configuration parser chains
 *
 * Dave Katz, June 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_memory.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef THRASH_MEMORY

/******************************************************************************
* [no] memory thrash usage blocksize persist persec runtime
*
* OBJ(int,1) = amount of memory to eat in bytes
* OBJ(int,2) = max block size to allocate
* OBJ(int,3) = max persistence of allocated blocks in seconds
* OBJ(int,4) = max amount to allocate per second in bytes
* OBJ(int,5) = amount of time to do all this, in seconds
*/

EOLNS    (memory_thrash_eol, thrash_command);
NUMBER  (memory_thrash_runtime, memory_thrash_eol, no_alt,
	 OBJ(int,5), 1, 0x7fffffff/ONESEC,
	 NULL /*"Run time in seconds"*/);
NUMBER  (memory_thrash_maxalloc, memory_thrash_runtime, no_alt,
	 OBJ(int,4), 10, 0x7fffffff,
	 NULL /*"Max amount to allocate per second"*/);
NUMBER  (memory_thrash_persist, memory_thrash_maxalloc, no_alt,
	 OBJ(int,3), 1, 0x7fffffff/ONESEC, 
	 NULL /*"Max persistence of block in seconds"*/);
NUMBER  (memory_thrash_maxblock, memory_thrash_persist, no_alt,
	 OBJ(int,2), 10, 0x7fffffff,
	 NULL /*"Max block size to allocate"*/);
NUMBER  (memory_thrash_amount, memory_thrash_maxblock, no_alt,
	 OBJ(int,1), 0, 0x7fffffff,
	 NULL /*"Amount of memory to allocate"*/);
NVGENNS  (memory_thrash_nvgen, memory_thrash_amount, thrash_command);
NOPREFIX(memory_thrash_no, memory_thrash_nvgen, memory_thrash_eol);
KEYWORD (memory_thrash, memory_thrash_no, no_alt,
	 "thrash", NULL /*"memory allocator thrash test"*/, PRIV_CONF);

#define LAST_MEMORY_CHAIN memory_thrash

#else

#define LAST_MEMORY_CHAIN no_alt

#endif

/******************************************************************************
* [no] memory validate-checksum [nn]
*
* OBJ(int,1) = interval for codespace checksum validation (in seconds)
*
* "no" turns it off, lack of parameter sets the default value.
*/

PARAMS_KEYONLY(memory_validate, LAST_MEMORY_CHAIN, "validate-checksum",
	       OBJ(int,1), 1, 0x7fffffff/ONESEC,
	       memory_val_cmd, 0,
	       NULL /*"Code space checksum validation"*/,
	       NULL /*"Interval in seconds"*/,
	       PRIV_CONF|PRIV_HIDDEN);

/******************************************************************************
* [no] memory check-interval [nn]
*
* OBJ(int,1) = interval for memory validation (in seconds)
* "no" turns it off, lack of parameter sets the default value.
*/

PARAMS_KEYONLY(memory_check, memory_validate, "check-interval",
	       OBJ(int,1), 1, 0x7fffffff/ONESEC,
	       memory_check_cmd, 0,
	       NULL /*"Memory validation interval"*/,
	       NULL /*"Interval in seconds"*/,
	       PRIV_CONF);

/******************************************************************************
* [no] memory free-list nn
*
* OBJ(int,1) = new free list size
*/

EOLNS    (memory_free_list_eol, memory_free_list_cmd);
NUMBER  (memory_free_list_size, memory_free_list_eol, no_alt,
	 OBJ(int,1), 1, 0xfffffff0, NULL /*"Free list size in bytes"*/);
NVGENNS	(memory_free_list_nvgen, memory_free_list_size,
	 memory_free_list_cmd);
KEYWORD (memory_free_list, memory_free_list_nvgen, memory_check,
	 "free-list", NULL /*"Add a new free list"*/,
	 PRIV_CONF | PRIV_HIDDEN);

KEYWORD (memory, memory_free_list, ALTERNATE,
	 "memory", NULL /*"Configure memory management"*/,
	 PRIV_CONF | PRIV_HIDDEN);

#undef ALTERNATE
#define ALTERNATE memory

