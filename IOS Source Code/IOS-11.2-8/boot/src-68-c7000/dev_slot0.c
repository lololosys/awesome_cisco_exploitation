/* $Id: dev_slot0.c,v 3.1.58.1 1996/03/18 18:34:47 gstovall Exp $
 * $Source: /release/111/cvs/Xboot/src-68-c7000/dev_slot0.c,v $
 *------------------------------------------------------------------
 * Flash card support functions for 7000 ROM monitor
 *
 * December 1994, Szewei Ju
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dev_slot0.c,v $
 * Revision 3.1.58.1  1996/03/18  18:34:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.26.1  1996/03/16  06:10:30  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1995/11/09  09:10:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:08  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.2  1995/06/05  06:10:26  smackie
 * Sync Arizona to Arizona_baseline_950604
 * Branch: Arizona_branch
 *
 * Revision 1.1.4.1  1995/05/02  23:22:29  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:36  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../reason.h" 
#include "../extern.h"
#include "../../sys/filesys/filesys.h"
#include "../../sys/filesys/ralib.h"
#include "../../sys/filesys/fslib.h"
#include "../../sys/rommon/src/mon_boot.h"
#include "../../sys/les/flash_fs_les.h"
#include "../../sys/h/cisco_a.out.h"
#include "monlib.h"
#include "dev_slot0.h"
#include "dev_flash.h"
#include "mon_fslib.h"
#include "nvmonvars.h"

struct  monlib_iface ml_iface;
ulong   monlib_entry;

/*
 * The rom monitor is VERY limited in the amount of memory it can use
 * for variable.  On RP1, RAM begins at 0, but the 1st chuck of memory
 * is used by the exception vectors.  The rom monitor data is linked
 * at 0x120 and our stack grows downward from 0x1000.  So we have less
 * than 4K for both stack & data (not to mention that the system
 * scheduler task runs on the rom monitor stack also).  So as a
 * hack^h^h^h^h work around we will reuse part of bufu.  Be VERY
 * careful when changing/adding routines that use "share_buff".
 */
char    *share_buff = bufu.bufh.buf2;   

dirent_t common_dirent;

/*************************************************************************
 *     A file ID is made up of two parts: a device id and a file
 *     name.  They are separated by a colon (:).  Device ID's can be up to
 *     seven characters long.  It is defined by the raw access library 
 *     (RALIB).  The 7000 has only one device defined in RALIB which is
 *     "slot0" (case sensitive).  
 *
 *     The file system eventually uses the RALIB to figure out
 *     if a valid device ID was used.  The entire file ID should be
 *      null terminated.  So the format looks like this:
 *
 *       <device ID>:<file name><null>
 */
char flash_card_dev_id[] = "slot0:";
char *fc_insert_status_msg[] = { "OK",
         "Flash card is half inserted in RP's PCMCIA slot",
         "Flash card is half inserted in RP's PCMCIA slot",
         "No flash card in RP's PCMCIA slot or it's not plugged in properly",
};


/************************************************************************
 * NAME: flash_card_check_id
 */
ushort flash_card_check_id (ulong dev_addr)
{
    ushort id;
    volatile ushort *lp;

    lp = (volatile ushort *)dev_addr;
    *lp = READ_ID_CODES | (READ_ID_CODES << 8);
    delay(10);
    id = (*lp & 0xff) << 8;
    id += *(lp+1) & 0xff;
    *lp = RESET | (RESET << 8);
    return( id);
}

/************************************************************************
 * NAME: flash_card_valid
 */
boolean flash_card_valid (void)
{
    ushort fc_id;
    ulong  fc_size;
    uchar  fc_status;

    if ((fc_status=((*(volatile uchar *)ADRSPC_FCSTATUS) & 3)) != 0) {
        printf("%s\n", fc_insert_status_msg[fc_status]);
	return FALSE;
    }
    if ((fc_id=flash_card_check_id(ADRSPC_EXT_FLASH)) != INTEL_2P_FLASH_CARD_ID) {
        printf("Invalid device: ID Manu code = 0x%2x, device code = 0x%2x\n",
	       fc_id >>8, fc_id & 0xff);
	return FALSE;
    }
    return TRUE;
}

/************************************************************************
 * fc_copy_to_ram
 *     Raw copy function used by ROM Monitor to read DIB and MONLIB 
 *     from flash card into RAM.
 *     It is *not* used to read files from flash card.
 * INPUT: fc_offset -- offset from the beginning of the flash card
 *                     It is assumed to start at a even address.
 *        ram_addr -- ram address to copy data into.
 *        size -- number of bytes to copy
 * RETURN: number of bytes copied.
 */
boolean fc_copy_to_ram (ulong fc_offset, char *ram_addr, ulong size)
{
    ushort *p1, *p2;
    ulong  i, words;

    if (fc_offset > RP_FLASH_CARD_MAX_SIZE)
	return FALSE;
    
    p1 = (ushort *)(fc_offset+ADRSPC_EXT_FLASH);
    p2 = (ushort *)ram_addr;
    words = size >> 1;
    *p1 = RESET | (RESET << 8);
    delay(10);
    for (i = 0; i < words; i++) {
        if ((ulong)p1 % INTEL_2P_FC_BANK_SIZE == 0) {
            *p1 = RESET | (RESET << 8);
            delay(10);
        }
        *p2++ = *p1++;
    }
    if (size & 1) 
        ram_addr[size-1] = (char)(*p1 >> 8);	
    return TRUE;
}

/****************************************************************************
 * load_monlib_from_fc --  load MONLIB from flash card into high memory
 *                         at the entry point specified in a.out header
 * INPUT:  entry -- pointer to a location which holds the monlib entry point
 * RETURN: TRUE -- succeed.
 *         FALSE -- failed.
 * 
 *  Flash Card Format
 * 
 *    +--------------------------------+
 *    | DIB                            |
 *    +--------------------------------+
 *    | BSM                            |
 *    +--------------------------------+ <--- monlib_offset
 *    | MONLIB - monlib is in simple   |
 *    |          filesystem format.    |
 *    +--------------------------------+ <--- file_system_offset
 *    |                                |
 *    | File System                    |
 *    |                                |
 *    +--------------------------------+
 *    | Misc                           | 
 *    +--------------------------------+
 */
boolean load_monlib_from_fc (ulong *entry, boolean vocal)
{
    fslib_device_info_block_t *dib;
    ulong i, size, bsize;
    struct exec exec;
    uchar *bstart, *foffset;
    flash_fhdr_t fhdr;
    char *monlib_string;
    ushort *ptr, *end, chksum;

    dib = (fslib_device_info_block_t *)share_buff;  /* hack alert */

    monlib_string = (char *)getvar(nvram->evartbl, "MONLIB");
    if (!monlib_string)
	monlib_string = DEFAULT_MONLIB;

    if (!fc_copy_to_ram(0, (char *)dib, sizeof(fslib_device_info_block_t))) {
        printf("\nError: Read DIB from the flash card.");
        return FALSE;
    }
    if (dib->magic != FSLIB_DEVICE_INFO_BLOCK_MAGIC) {
        printf("\nError: Unrecognized DIB on flash card.");
        return FALSE;
    }
    if (dib->monlib_length == 0) {
        printf("\nError: No MONLIB on flash card.");
        return FALSE;
    }

    foffset = (uchar *)flash_align((ulong)dib->monlib_offset);
    while (1) {
	if (!fc_copy_to_ram((ulong)foffset, (char *)&fhdr, sizeof(fhdr))) {
	    printf("\nError: Reading monlib fhdr.");
	    return FALSE;
	}
	if (fhdr.ffh_magic != FLASH_FHDR_MAGIC) {
	    printf("\nError: No monlib found\n");
	    return FALSE;
	}
	if (strcmp(monlib_string, fhdr.ffh_name) == 0) 
	    break;
	foffset = (uchar *)flash_align(foffset + sizeof(fhdr) + fhdr.ffh_len);
    }

    foffset = (uchar *)flash_align(foffset + sizeof(fhdr));
    
    if (!fc_copy_to_ram((ulong)foffset, (char *)&exec, EXEC_HDRSIZ)) {
	printf("\nError: Reading exec header.");
	return FALSE;
    }
    
    if (exec.a_magic != CISCOMAGIC) {
	printf("Error: bad exec magic 0x%x\n", exec.a_magic);
	return FALSE;
    }

    if (exec.a_machtype != CISCOTYPE) {
	printf("Error: bad exec machtype %d\n", exec.a_machtype);
	return FALSE;
    }

    *entry = exec.a_entry;
    size = fhdr.ffh_len;   

    ptr = (ushort *)(*entry - EXEC_HDRSIZ);
    end = (ushort *)((uchar *)ptr + size);

    if (*entry & 3 || !touch_device(ptr) || !touch_device(end)) {
	printf("Error: monlib linked to invalid address (0x%x, %d)\n",
	       ptr, size);
	return FALSE;
    }

    end[0] = 0;
    end[1] = 0;
    if (!fc_copy_to_ram((ulong)foffset, (char *)((*entry)-EXEC_HDRSIZ), size)) {
        printf("\nError: Copy MONLIB to high memory.");
        return FALSE;
    }

    chksum = 0;
    chksum = update_cksum(chksum, (uchar *)ptr, (ulong)size);
    chksum = ~chksum;
    if (chksum != fhdr.ffh_checksum) {
	printf("Error: fslib chksum mismatch, 0x%x != 0x%x\n", 
	       chksum, fhdr.ffh_checksum);
	return FALSE;
    } 

    bsize = exec.a_bss;
    bstart = (uchar *)(*entry + (size - EXEC_HDRSIZ));
    bzero(bstart, bsize);  /* fill the bss segment with zero */

    /* 
     * this data is probably in the data cache, and before we try to 
     * execute that "data" we need to make sure it has actually been 
     * written back to physical memory.
     */
    asm(".word	0xf4f8");		/* flush caches "CPUSH IC/DC" */

    if (vocal) 
	printf("\nMONLIB is successfully loaded from flash card\n"
	       "into high memory at 0x%X, size 0x%X (%d).\n", 
	       *entry, size, size);

    return TRUE;
}

/***************************************************************************
 * NAME: init_monlib 
 *    Load MONLIB from internal flash or flash card
 *    Call the MONLIB entry point to initialize MONLIB
 *    (pass the ralib_ptr to MONLIB and get back the fslib vector pointer
 *    from MONLIB)
 * INPUT: 
 *      name -- pointer to the name of the monlib (only used when loading
 *            monlib from internal flash)
 * RETURN: TRUE -- succeeded.
 *         FALSE -- failed.
 */
boolean init_monlib (boolean vocal)
{
    struct plb proc_load_block;
    struct pib proc_inface_block;
    struct mon_iface    *m_iface; 
    extern int printf();

    if (!load_monlib_from_fc(&monlib_entry, vocal))
        return FALSE;

    m_iface = (struct mon_iface *)share_buff;
    m_iface->printf = &printf;
    /* 
     * Setup pointer to the monlib_iface in the mon_iface
     * MONLIB will put the fslib functions in the monlib_iface structure
     */
    m_iface->mlifp = &ml_iface;
    proc_inface_block.mifp = m_iface;

    call_monlib(monlib_entry, MONLIB_INIT, &proc_load_block, &proc_inface_block);
    /*
     * 2nd call is a hack to turn off debug flags that are turned on
     * by default.
     */
    call_monlib(monlib_entry, MONLIB_NODEBUG, NULL, NULL);

    if (vocal) 
	printf("\n%s initialized.\n", ml_iface.monlib_vers);

    return TRUE;
}

/***************************************************************************
 * NAME: change_monlib_flags
 *     Call the MONLIB entry point with new flags value
 * INPUT: 
 * RETURN: TRUE -- succeeded.
 *         FALSE -- failed.
 */
void change_monlib_flags (void)
{
    char  *a_value;
    long  value;
 
    if (!monlib_initialized) {
	if (init_monlib(TRUE))
	    monlib_initialized = TRUE;
	else {
	    printf("Error: Failed to initialize monlib\n");
	    return;
	}
    }

    if (debug(DEBUG_MONLIB)) {
	a_value = debug_value(DEBUG_MONLIB);
	if (!a_value || a_value[0] == 0) {
	    /*
	     * No value defined, use default
	     */
	    value = MONLIB_PRINTALL;
	} else {
	    value = gethexnum(a_value);
	    if (value == -1)  {
		value = MONLIB_PRINTALL;
		printf("Invalid hex number [%s], using %x\n", 
		       a_value, value);
	    }
	}  
    } else { 
	/* 
	 * No debug flags.
	 */
	value = MONLIB_NODEBUG;
    }
    printf("Reseting monlib flags to 0x%x\n", value);
    call_monlib(monlib_entry, value, NULL, NULL);
}



/******************************************************************************
 * Name: flash_card_print_dirent
 *
 * Description: prints a dirent (directory entry)
 *
 * Input:
 *   de -- pointer to the directory entry to print
 *
 *******************************************************************************/
inline void flash_card_print_dirent (dirent_t *de)
{
    int i;

    i = printf("%-10d bytes (%#x)", de->size, de->size);
    while (i++ < 30) putchar(' ');  /* pad with spaces */
    printf("0x%8x  ", de->crc);
    printf("%s", de->d_name);
    if (de->deleted) printf(" (deleted)");
    putchar('\n');
}

/**************************************************************************
 * NAME:  show_flash_card
 *        Show the directory entry on the flash card
 *  Note: The ROM Monitor has the knowledge of the directory entry format
 *        which is independent of the file header format in the
 *        file system.
 *  Note: ROM Monitor should use the "d_reclen" to jump to the next entry
 *        in the fc_dirent_buffer.  So, the ROM Monitor can still
 *        interpret those previously defined fields when new fields are
 *        added later.
 */
void show_flash_card (boolean all)
{
    int dfd, actual;
    dirent_t *dent_buffer = &common_dirent;
    char *file_id = share_buff;

    if (!monlib_initialized && sys_dirty) {
	printf("\nCan't load monlib while a system image is in memory\n");
	return;
    }
    
    if (!flash_card_valid()) {
        /* flash_card_valid() will print the error message if invalid */
	return;
    }
    if (!monlib_initialized) {
	if (init_monlib(FALSE))
	    monlib_initialized = TRUE;
	else {
	    printf("Error: Failed to initialize monlib\n");
	    return;
	}
    }
    /*
     * Fix up the special directory file ID for FS_open()
     */
    sprintf(file_id, "%s.", flash_card_dev_id);

    if ((dfd = FS_open(file_id, O_RDONLY, 0, 0, 0)) <= 0) {
        printf("Error: show_flash_card, FS_open (%s,%d)\n", file_id, dfd);
    } else {
	puts("         File size           Checksum     File name\n");
	for (;;) {
	    actual = FS_getdents(dfd, dent_buffer, sizeof(dirent_t));
	    if (actual == 0)
		break;
	    if (actual != sizeof(dirent_t)) {
		printf("Get directory error (%d)\n", actual);
		break;
	    }
	    if (all || !dent_buffer->deleted) 
		flash_card_print_dirent(dent_buffer);
	}
	FS_close(dfd);
    }
}

/**************************************************************************
 * NAME:  flash_card_find_file
 *
 * INPUT:  filename -- pointer to the name of the file
 *        
 * RETURN: NULL, if the file is not found.  Otherwise, a pointer 
 *         to the directory entry structure of the file found.
 *         The directory entry contains all the information (size, crc,
 *         type, time, ..) about the file found.
 *
 * NOTE 1: The returned pointer must be passed to flash_card_copy_to_ram()
 *         to copy the found file into memory. 
 *
 * NOTE 2: The returned directory entry pointer is only good before next time
 *         the flash_card_find_file() is called.
 */
char *flash_card_find_file (char *filename)
{
    int dfd, actual;
    boolean first_file = FALSE;
    dirent_t *de = &common_dirent;
    char     *file_id = share_buff;    /* hack alert */
    
    if (!flash_card_valid()) {
        /* flash_card_valid() will print the error message if invalid */
	return FALSE;
    }
    if (!monlib_initialized) {
        if (init_monlib(FALSE))
            monlib_initialized = TRUE;
        else {
            printf("Error: Failed to initialize monlib\n");
            return FALSE;
        }
    }
    if (filename == NULL || *filename == '\0') 
        first_file = TRUE;

    /*
     * Fix up the special directory file ID for FS_open()
     */
    sprintf(file_id, "%s.", flash_card_dev_id);

    if ((dfd = FS_open(file_id, O_RDONLY,0,0,0)) <= 0) {
        printf("Error: flash_card_find_file, FS_open (%s,%d)\n", file_id, dfd);
    } else {
        do {
            actual = FS_getdents(dfd, de, sizeof(dirent_t));
            if (actual == sizeof(dirent_t)) {
                /* 
                 * We don't want to boot a deleted file, so let's
		 * skip it.
                 */
		if (de->deleted)
		    continue;
		if (de->valid == FALSE)
		    continue;
		/*
		 * It would be nice if we could tell for sure that
		 * this file was a bootable IOS image, but since we
		 * can't, let's at least skip config files.
		 */
		if (de->type == FILE_TYPE_SYS_CONFIG)
		    continue;

                if (first_file || strcmp(filename, de->d_name) == 0) {
		    FS_close(dfd);
		    return((char *)de);
	       }
            } 
        } while (actual == sizeof(dirent_t));
        FS_close(dfd);
    }
    return(NULL);
}

ulong flash_card_file_size (dirent_t *de)
{
    if (!de)
	return 0;

    return de->size;
}

char *flash_card_file_name (dirent_t *de)
{
    if (!de)
	return NULL;

    return de->d_name;
}

/**************************************************************************
 * NAME:  flash_card_copy_to_ram
 * INPUT: dirent -- pointer to the directory entry structure of 
 *                  the file to be copied.
 *                  To save the caller from having to know the 
 *                  directory entry structure, the pointer is
 *                  is passed as "void *".
 *        ram_addr -- memory address to copy the file into
 *        size -- size of the buffer
 * RETURN: TRUE -- file successfully copied into memory
 *         FALSE -- failed to copy the file into memory
 */
boolean flash_card_copy_to_ram (char *dirent, ulong ram_addr, ulong size)
{
    dirent_t *de;
    int fd, actual;
    ulong crc, cur_addr, bytes_left, read_len;
    char *file_id = share_buff;  /* hack alert */

    if (!flash_card_valid())
	return FALSE;

    de = (dirent_t *)dirent;
    if (de->size > size) {
        printf("\nCan't read file from flash card into memory,"
	       " buffer too small, (%d, %d)", size, de->size);
        return FALSE;
    }
    if (de->d_namlen > MAXNAMELEN) {
        printf("\nFile name too long, %d, %s", de->d_namlen, de->d_name);
        return FALSE;
    }

    /*
     * Fix up the file ID from the file name and flash_card_dev_id
     * for FS_open().
     */
    sprintf(file_id, "%s%s", flash_card_dev_id, de->d_name);
    if ((fd = FS_open(file_id, O_RDONLY,0,0,0)) > 0) {
	bytes_left = de->size;
	cur_addr = ram_addr;
	while (bytes_left > 0) {
	    read_len = bytes_left > FC_READ_LEN ? FC_READ_LEN : bytes_left;
	    actual = FS_read(fd, (char *)cur_addr, read_len);
	    if (actual != read_len) {
		printf("\nError: flash_card_copy_to_ram FS_read (%s, %d, %d, %d)", 
		       file_id, fd, de->size, actual);
		FS_close(fd);
		return FALSE;
	    }
	    cur_addr += read_len;
	    bytes_left -= read_len;
	    putchar('R');
	}
        FS_close(fd);
        return TRUE;
    }
    printf("\nError: flash_card_copy_to_ram, FS_open(%s, %s, %d) = %d\n", 
	   file_id, "O_RDONLY", 0, fd);
    return FALSE;
}




