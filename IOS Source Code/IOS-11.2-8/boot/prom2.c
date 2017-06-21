/* $Id: prom2.c,v 3.4.20.3 1996/05/30 23:32:40 jwelder Exp $
 * $Source: /release/112/cvs/Xboot/prom2.c,v $
 *------------------------------------------------------------------
 * prom2.c -- linkage to upper ROM software
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prom2.c,v $
 * Revision 3.4.20.3  1996/05/30  23:32:40  jwelder
 * CSCdi54450:  Local Timeout Error On rommonitor boot
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/10  23:17:08  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.4.20.1  1996/05/09  14:00:02  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.4.48.1  1996/04/27  06:13:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.38.1  1996/04/08  01:34:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.26.1  1996/03/22  09:06:53  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4.10.1  1996/03/03  21:18:59  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.8.1  1996/03/02  01:06:38  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.4  1996/01/30  21:11:02  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.3  1996/01/19  23:51:19  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2.28.1  1996/02/29  07:41:56  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:41:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:38  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:37  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:12:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "prom2.h"
#include "stdio.h"
#include "ctype.h"
#include "reason.h"
#include "extern.h"
#include "cpu.h"
#ifdef RP1
#include "../sys/rommon/src/mon_elf.h"
boolean setup_elf(char *, long);
#endif

#ifdef  SYNALC
#include "mc68ec030_defs.h"
#endif

#ifdef PAN
extern cookie_t *cookie;
#endif

extern void restore_vectors();
extern void strcpy();
extern void lowercase();

unsigned long get_rom_entry();

char notsupp[] = "Entry type not supported\n";

/*
 * bootload
 * Perform network booting by invoking the system software.
 */


long bootload(filename, flags)
    char *filename;
    int flags;
{
    char *c;
    long size;
    struct plb_t plb, *p;
    int result;
    int rom_system_entry = PROM2_BOOTLOAD;
    int net_system_entry = PROM2_BOOTLOAD;
    int bootstrap_entry = PROM2_BOOTLOAD;
    boolean try_rom = FALSE;
    ulong rom_start;

    /*
     * Make sure we found a bootable ROM system.
     */
    if (!prom2) {
	puts(noprom2);
	return(0); 
    }

    /* Check for any command switches */

    if (!(flags & (BOOTCOPYTFTPFLASH | BOOTCOPYMOPFLASH | BOOTCOPYRCPFLASH)) 
        && (filename != NULL) 
        && (*filename == '/')) {
	filename++;

	switch(*filename++) {
	  case 'A' :	case 'a':		/* Debug all */
	    bootstrap_entry = PROM2_BOOTDEBUG;
	    rom_system_entry = PROM2_BOOTDEBUG;
	    net_system_entry = PROM2_BOOTDEBUG;
	    break;
	  case 'B' :	case 'b':		/* Debug bootstrap image */
	    bootstrap_entry = PROM2_BOOTDEBUG;
	    break;
	  case 'M' :	case 'm':		/* Debug the monitor */
	    c = filename;		/* Copy command pointer */
	    while (*c && isspace(*c))	/* Skip past any spaces */
	      c++;
	    if (*c) {			/* If we are booting from the net */
		net_system_entry = PROM2_DEBUG; /* Debug using net image */
	    } else {
		rom_system_entry = PROM2_DEBUG; /* Debug using ROM image */
	    }
	    break;
	  case 'N' :	case 'n':		/* Debug netbooted system image */
	    net_system_entry = PROM2_BOOTDEBUG;
	    break;
	  case 'R' :	case 'r':		/* Debug ROM system image */
	    rom_system_entry = PROM2_BOOTDEBUG;
	    break;
	  case 'Z':     case 'z':
	    try_rom = TRUE;	/* Try to run from RAM */
	    break;
	  default:
	    puts("Unknown boot switch\n");
	    return(0);
	}

	while (*filename && isspace(*filename))
	  filename++;			/* Skip spaces after switch */
    }

    /*
     * If no filename was specified, we're booting a ROM image.
     * Otherwise, construct a boot load block for bootload() in
     * the system image:
     */
    bootaddr = 0;
    if ((filename == NULL) || (*filename == '\0')) {
	p = NULL;
    } else {
	p = &plb;
	p->plb_str = filename;
	p->plb_addr = (uchar *)0;
	p->plb_size = 0;
	p->plb_ipaddr = 0;
	p->plb_magic = PLBMAGIC;
	p->plb_option = flags & ~BOOTRUNIMAGE; /* os/boot.c doesn't know */
	                                       /* about this flag */
	p->plb_result = PLB_OK;
	
    }

#ifdef RP1
   if (sys_dirty) {
       printf("Please reset before booting\n");
       return(0);
   }
   /*
    * Invoke RP1 specific booting code
    */
   result = bootload_rp1(filename, flags, rom_system_entry, p, FALSE);
   if (debug(DEBUG_BOOT))
       printf("boot: bootload_rp1() returned %x\n", result);
   if (!result)
       return result;
    sys_dirty = TRUE;

#elif defined (SYNALC)
   /*
    * Invoke SYNALC specific booting code (adapted from PROM2_MAGIC3 case).
    */
    size = (long) ALC_LoadAcpCode ((u_char *) prom2, (u_char *) baseaddr,
				   (u_long) flags) ;
    if (! size)
	return(0) ;
    if (!setup(baseaddr, size))
	return(0);
    if (flags & BOOTRUNIMAGE || p) {
	restore_vectors();
	system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
		/* Flush I/D caches before jump. */
	Sys_ModCpuCACR (CPU030_FLUSH_CACHES, CPU030_FLUSH_CACHES) ;
	result = ram_dispatch(rom_system_entry,p,baseaddr);
    } else
	return((long)baseaddr);

#else
    /*
     * Run from ROM (MAGIC1), uncompress to RAM (MAGIC2), or copy to RAM
     * (MAGIC3), then netboot a file if (p != NULL).  If loading from the
     * bootstrap monitor ('l' command), then do not netboot.
     */

    switch (*prom2) {
    case PROM2_MAGIC1:
	restore_vectors();
	system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
	result = prom2_dispatch(rom_system_entry,p);
	break;
    case PROM2_MAGIC2:
	if (!(size = uncompress(prom2, baseaddr)))
	    return(0);
	if (!setup(baseaddr, size))
	    return(0);
	if (flags & BOOTRUNIMAGE || p) {
	    restore_vectors();
	    system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
	    result = ram_dispatch(rom_system_entry,p,baseaddr);
	} else
	    return((long)baseaddr);
	break;
    case PROM2_MAGIC3:
	if (try_rom) {
	    rom_start = get_rom_entry();
	    if (!rom_start) {
		printf("\nImage does not support run from ROM");
		return(0);
	    }
	    if (flags & BOOTRUNIMAGE || p) {
		system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
		result = ram_dispatch(rom_system_entry,p,rom_start);
		break;
	    } else
	      return((long)rom_start);
	}

	if (!(size = unpack(prom2, baseaddr)))
	    return(0);
	if (!setup(baseaddr, size))
	    return(0);
	if (flags & BOOTRUNIMAGE || p) {
	    restore_vectors();
	    system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
	    result = ram_dispatch(rom_system_entry,p,baseaddr);
	} else
	    return((long)baseaddr);
	break;
    default:
	printf("\nBad magic number - %x", *prom2);
	return(0);
    }
#endif

    if (result) {		/* If we supported this entry type */

	/*
	 * Now we have a netbooted image sitting in RAM.  Relocate and start.
	 * If we loaded a secondary bootstrap image, we need to dispatch it
	 * to load in the system image:
	 */

	reset_io();		/* Reset IO devices */
	sil(7);			/* Disable maskable interrupts */

	if (p->plb_size == 0) {  /* Nope, there was an error */
            /* If we haven't come back here to retry the FLH
             * operation, display plb_str. It will contain an error
             * message. 
             * Else, it will contain the FLH command string.
             */
            if (!((p->plb_magic == PLBMAGIC) && 
                  (p->plb_result == PLB_ERROR_RETRY)))
	        printf("\n%s", p->plb_str);
	    return(0);
	}

	if (setup(p->plb_addr, p->plb_size)) {

#if ! defined(PAN) && ! defined(XX) && ! defined(SYNALC)
	    if (config.bootstrap) {
#endif
		/*
		 * We loaded a runnable system image:
		 */
		bootaddr = p->plb_ipaddr;
#if ! defined(PAN) && ! defined(XX) && ! defined(SYNALC)
	    } else {
		/*
		 * We loaded a secondary bootstrap image. Start it again
		 * to load in the system image. Note that we should not
		 * clear plb_ipaddr here because this will make the system
		 * image think that the secondary boostrap is the default
		 * ROM system image:
		 */
		p->plb_str = filename;
		p->plb_addr = (uchar *)0;
		p->plb_size = 0;
		restore_vectors();
		result = ram_dispatch(bootstrap_entry, p, baseaddr);
		reset_io();		/* Restore vectors */
		sil(7);			/* Mask interrupts */
		if (result) {
		    if (p->plb_size == 0) {
			printf("\n%s\n", p->plb_str);
			system_state = SYSTATE_MONITOR;
			return(0);
		    } else {
			if (setup(p->plb_addr, p->plb_size)) {
			    bootaddr = p->plb_ipaddr;
			} else {
			    system_state = SYSTATE_MONITOR;
			    return(0);
			}
		    }
		}
	    }
#endif /* PAN */
	    if (result) {
		if (flags & BOOTRUNIMAGE) {
		    restore_vectors();
		    system_state = SYSTATE_RUNNING;
		    result = ram_dispatch(net_system_entry, NULL, baseaddr);
		} else {
		    system_state = SYSTATE_MONITOR;
		    return((long)baseaddr);
		}
	    }
	} else {
	    system_state = SYSTATE_MONITOR;
	    return(FALSE);		/* Setup failed */
	}
    }
    reset_io();
    sil(7);
    system_state = SYSTATE_MONITOR;

    if (!result) {
	printf(notsupp);
    }
    return(0);
}

/*
 * setup
 * Decode the filhdr field
 */

int setup (addr, size)
    char *addr;
    long size;
{
    char *bdata, *p;
    int i;
    char processor_version;
    ushort *sdata;
    long *ldata;
    char *type;
    int tsize, dsize, bsize, ssize, offset, aentry;
    struct cdata *data;
    struct cload *load;

    baseaddr = (long *)ADRSPC_CODE_START; /* default load address */
    sdata = (ushort *)addr;
    ldata = (long *)addr;
    if (ldata[0] == 0407) {		/* do the long compare first */
	type = "F2";			/* MIT cc68 */
	offset = 32;
	tsize = ldata[1];
	dsize = ldata[2];
	bsize = ldata[3];
	ssize = ldata[4];
    } else if (sdata[1] == 0x107 && sdata[0] == 0x20C) {
	type = "F1";			/* HP-UX 9000 or Sun */
	offset = 64;			/* sizeof(exec) */
	tsize = ldata[3];
	dsize = ldata[4];
	bsize = ldata[5];
	ssize = ldata[9];
    } else if (sdata[1] == 0x107 && sdata[0] == 2) {
	type = "F3";			/* Sun */
	offset = 32;			/* sizeof(exec) */
	tsize = ldata[1];
	dsize = ldata[2];
	bsize = ldata[3];
	ssize = ldata[4];
        aentry = ldata[5];  /* get entry point */

#ifdef SYNALC
	if (((u_long)addr >= ADRSPC_FLASH_BASE) &&
	    ((u_long)addr < ADRSPC_FLASH_END))		/* addr is in flash */
                baseaddr = (long *)(addr + offset);
#endif
#ifdef PAN
        if ( cpu_type != CPU_XX )  /* check for RFF; XX is always RFR */
            if ((long)addr >= ADRSPC_FLASH_BASE) /* addr is in flash */
                baseaddr = (long *)(addr + offset);
#endif
#ifdef RP1
	/* 
	 * Verify that the size copied into RAM is at least as big as
	 * the text & data segment claim to be.
	 */
	if (size < offset + tsize + dsize) {
	    printf("\nImage error: size %d, text %d, data %d, bss %d\n", 
		   size, tsize, dsize, bsize);
	    return (FALSE);
	}
#endif
        if (baseaddr == ((long *)(addr + offset))) { /* image in place */
            printf("\n%s: %d+%d+%d at %#x\n", type, tsize, dsize, 
                   bsize, baseaddr);
            return (TRUE);
        }
    } else if (sdata[0] == 0xa800 && sdata[1] == 0x3000) {
	/* MOP image - call ourselves recursively to process it */
	return(setup(addr + 512, size - 512));

    } else if (sdata[0] == 0x1F9D || sdata[0] == 0x1F9E) {  /* compressed */
	/*
	 * Build fake prelude
	 */
	data = (struct cdata *)(addr - (sizeof(struct cdata) +
		sizeof(struct cload)));
	data->magic = PROM2_MAGIC2;
	data->number = 1;
	data->cksum = 0;
	load = (struct cload *)(data + 1);
	load->addr = (u_long)addr;
	load->len = size;
	if (!(size = uncompress(data, baseaddr)))
	    return(FALSE);
	/*
	 * Recursive call to place unpacked binary
	 */
	return setup(baseaddr, size);
#ifdef RP1
    } else if (ldata[0] == ELFMAGIC) {
	return setup_elf(addr, size);
#endif
    } else {
	puts("Undefined load module -- ignoring\n");
	return(FALSE);
    }
#ifdef	SYNALC
    PUNCH_WD ;
#endif
    p = (char *)baseaddr;
    bdata = (char *)addr + offset;
    bcopy(bdata, p, tsize);
#ifdef	SYNALC
    PUNCH_WD ;
#endif
    p += tsize;
    bdata += tsize;
    bcopy(bdata, p, dsize);
#ifdef	SYNALC
    PUNCH_WD ;
#endif
    p += dsize;
    for (i = 0; i < bsize; i++)
	*p++ = '\0';
#ifdef	SYNALC
    PUNCH_WD ;
#endif
    printf("%s: %d+%d+%d at %#x\n", type, tsize, dsize, bsize, baseaddr);

    /*
     * If this is a 68040, we must push the data cache at the end of setup()
     * in order to be sure there are no instructions (dirty data) still in the
     * data cache once we jump to RAM and start executing code.
     */

#ifdef RP1
       asm("     .word     0xf4f8");    /* cpush bc ; push & then inval. */
                                        /* both caches in 68040          */
#else
#ifdef CSC2
    processor_version = sys_status_reg->version;
    if (processor_version == VER_CSC4) 
       asm("     .word     0xf4f8");    /* cpush bc ; push & then inval. */
                                        /* both caches in 68040          */
#endif
#endif
#ifdef	lint
    ssize = ssize;
#endif

#ifdef	SYNALC
    PUNCH_WD ;
#endif

    return(TRUE);
}

/*
 * makebfile
 * Make a netboot file name
 */

char *makebfile (n)
    int n;
{
    if (n == 0 || n == 1)
	return (char *)NULL;

#ifdef PAN
    /*
     * Synoptics IGS boot file name:
     */
    if (cookie->vendor == VENDOR_SYNOPTICS)
	sprintf(bootfilename, SYNOPTICS_IGS_FORMAT, n);
    else
#endif
#ifdef RP1
    {
 	char *filename;

	filename = (char *)autoboot();
	if (filename)
	    return filename;
	/*
	 * Otherwise do it the old way.
	 */
    }
#endif
    sprintf(bootfilename,
	    (!config.oem) ? OEM_FORMAT : CISCO_FORMAT,
	    n, cputypes[cpu_type]);
    
    lowercase(bootfilename);
    return(bootfilename);
}

/*
 * unpack
 * Unpack an uncompressed image into RAM and return its size.
 */

int unpack (startaddr, endaddr)
    u_char *startaddr, *endaddr;
{
    int size, i;
    struct cdata *cdata;
    struct cload *cload;

    cdata = (struct cdata *)startaddr;
    cload = (struct cload *)(cdata + 1);
    size = 0;
    for (i = 0; i < cdata->number; i++) {
	if (cload->len != 0) {
	    bcopy(cload->addr, endaddr, cload->len);
	    endaddr += cload->len;
	    size += cload->len;
	    cload++;
	}
    }
    return(size);
}

/*
 * get_rom_entry - See if an image can run from ROM
 *
 */

unsigned long get_rom_entry ()
{
    ushort *sdata;
    struct cdata *cdata;
    struct cload *cload;

    cdata = (struct cdata *)prom2;
    cload = (struct cload *)(cdata + 1);

    if (cdata->number != 1)
      return(0);		/* Must be single image */

    sdata = (ushort *)cload->addr; /* Get load address */

    if ((sdata[1] == 0x107) && (sdata[0] == 2) && (sdata[16] == 0x6006) &&
	(sdata[17] == 0xdead))
      return((ulong)&sdata[16]);
    return(0);
}

#ifdef RP1

struct loadinfo {
    int fptr;    /* file pointer to section */
    int size;    /* size of section */
    unsigned long paddr;  /* physical link address of section */
};
/* at least the num of scns specified in case statement below */
#define LOADTBLENTRIES 8

/*
 * setup_elf
 * 
 * unpack an elf executable
 */
boolean setup_elf (char *addr, long size)
{
    Elf32_Ehdr *ehdr;
    Elf32_Shdr *shdr;
    long sh_off;
    int i, j, shnum, tsize, dsize, bsize;
    struct loadinfo loadtbl[LOADTBLENTRIES];
    boolean debug_elf;

    debug_elf = debug(DEBUG_ELF);

    if (debug_elf)
	printf("elf image at 0x%x, %d\n", addr, size);

    tsize = dsize = bsize = 0;
    ehdr = (Elf32_Ehdr *)addr;
    if (ehdr->e_type != ET_EXEC) {
	printf("Error: not an executable ELF file\n");
	return FALSE;
    } 

    if (ehdr->e_machine != EM_68K) {
	printf("Error: wrong image for this platform\n");
	return FALSE;
    } 

    if (ehdr->e_phnum > 1) {
	printf("Error: too many ELF program segments\n");
	return FALSE;
    } 

    shnum = ehdr->e_shnum;
    sh_off = ehdr->e_shoff;
    if (debug_elf)
	printf("elf: e_shoff 0x%x, e_shnum %d",ehdr->e_shoff, shnum);
    shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff);

    for (i = 0, j = 0; i < shnum; i++) {
	if (debug_elf) 
	    printf("\nelf: section %-3d @ 0x%x, type %d, flags 0x%x", 
		   i, shdr, shdr->sh_type, shdr->sh_flags); 
	/*
	 * Check if this section should be loaded.  Load values into a
	 * loadtable before coping to avoid clobbering section
	 * headers. 
	 */
	if (shdr->sh_type == SHT_PROGBITS && 
	    shdr->sh_flags & SHF_ALLOC &&
	    shdr->sh_size) {
	    if (j >= LOADTBLENTRIES) {
		printf("(ELF) too many loadable sections");
		return FALSE;
	    }
	    if (debug_elf) {
		printf("\n     adding to load table [%d]", j);
		printf("\n     offset 0x%x, addr 0x%x, size %d",
		       shdr->sh_offset, shdr->sh_addr, shdr->sh_size);
	    }
	    loadtbl[j].fptr = shdr->sh_offset;
	    loadtbl[j].size = shdr->sh_size;
	    loadtbl[j].paddr = shdr->sh_addr;
	    j++;
	    
	    if (shdr->sh_flags & SHF_EXECINSTR)
		tsize = shdr->sh_size;
	    else 
		dsize += shdr->sh_size;
	} else if (shdr->sh_type == SHT_NOBITS) {
	    bsize = shdr->sh_size;
	    if (debug_elf)
		printf("\n     bss 0x%x, size %d", 
		       shdr->sh_addr, shdr->sh_size);
	}
	shdr++;
    }
    
    if (debug_elf)
	printf("\nelf: relocating %d sections\n", j);

    for (i = 0; i < j; i++) {
	if (debug_elf)
	    printf("    # %d bcopy(0x%x, 0x%x, %d)\n", 
		   i, addr + loadtbl[i].fptr, loadtbl[i].paddr,
		   loadtbl[i].size);   
	if ((loadtbl[i].paddr & 3) ||
	    !touch_device(loadtbl[i].paddr) ||
	    !touch_device(loadtbl[i].paddr + loadtbl[i].size)) {
	    printf("elf section linked to illegal address (0x%x, %d)\n", 
		   loadtbl[i].paddr, loadtbl[i].size);
	    return FALSE;
	}
	bcopy(addr + loadtbl[i].fptr, loadtbl[i].paddr, loadtbl[i].size); 
    }
    
    printf("ELF: %d+%d+%d at %#x\n", tsize, dsize, bsize, baseaddr);
    asm(".word     0xf4f8");    /* cpush bc ; push & then inval. */
    return TRUE;
}
#endif
