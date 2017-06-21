/* $Id: loadprog.c,v 3.2.58.4 1996/07/08 22:42:41 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/loadprog.c,v $
 *------------------------------------------------------------------
 * loadprog.c
 *
 * November 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Load a program.  Supports ELF files, with conditional support
 * for a.out and COFF.
 *------------------------------------------------------------------
 * $Log: loadprog.c,v $
 * Revision 3.2.58.4  1996/07/08  22:42:41  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/16  21:18:17  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.2  1996/04/09  23:09:24  szhang
 * CSCdi53689:  Open() failed in PCMCIA monlib
 * Branch: California_branch
 * Monitor shouldn't reset the PCMCIA controller after the MONLIB
 * is launched.
 *
 * Revision 3.2.58.1  1996/03/21  23:27:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/01/04  03:15:29  wfried
 * Branch: ELC_branch
 * Don't print debugging messages unless necessary.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:29  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/23  05:57:59  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/06/07  22:37:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Load a program for execution and launch it.
** Handles ELF, COFF, and a.out files.
** Return -1 on loadprog failure.  Return the return value
** of the launched program if program is to be launched.
** Return 1 for successful loadonly.
*/
#include "monitor.h"
#include "sh.h"
#include "signal_4k.h"
#include "mon_boot.h"
#include "fsio.h"
#include "monfsio.h"
#include "proto.h"
#include "fstat.h"
#include "monlib.h"
#include "mon_elf.h"
#include "dev.h"
#include "mon_defs.h"
#include "extern.h"
#include "monfsio.h"
#include "queue.h"

#include "../../../boot/cpu.h"

#ifdef COFFSUPPORT
#include "a.out.h"
#endif   /* COFFSUPPORT */

#ifdef BRASIL
#include "../../os/reloc.h"
#ifndef ASMINCLUDE
#define ASMINCLUDE
#include "../../machine/target_cisco.h"
#endif
#endif /* include relocatable image support */

long loadsize;  /* size of loaded program */

#ifdef DEBUG
char *scntypstr[] = {   /* array of section type strings */
    "Section header table entry unused (NULL)",
    "Program specific (private) data (PROGBITS)",
    "Link editing symbol table (SYMTAB)",
    "A string table (STRTAB)",
    "Relocation entries with addends (RELA)",
    "A symbol hash table (HASH)",
    "Information for dynamic linking (DYNAMIC)",
    "Information that marks file (NOTE)",
    "Section occupies no space in file (NOBITS)",
    "Relocation entries, no addends (REL)",
    "Reserved, unspecified semantics (SHLIB)",
    "Dynamic linking symbol table (DYNSYM)",
};
#define MAXSCNTYPSTR (sizeof(scntypstr) / sizeof(char *))
#endif /* DEBUG */

struct loadinfo {
    int fptr;    /* file pointer to section */
    int size;    /* size of section */
    unsigned long paddr;  /* physical link address of section */
};
/* at least the num of scns specified in case statement below */
#define LOADTBLENTRIES 8
#if defined(FUTURE)
#if !defined(CHECK_INTERFACE)
#define CHECK_INTERFACE(x)	(0)
#endif /* defined(CHECK_INTERFACE) */
#endif /* defined(FUTURE) */
extern int readline(int fd, char *bufp);  /* forward declaration */

/*
 * The Brasil montior uses imageaddr and spl_hdr to determine
 * the correct load point for netbooting a relocatable image.
 */
ulong imageaddr;
struct special_header *spl_hdr;  /* Special header for reloc images - reloc.h */

static char fhmsg[] = "loadprog: cannot read file's \"%s\" header\n";
static char errmsg[] = "loadprog: error - %s\n";
static char invmsg[] = "loadprog: error - Invalid image for platform\n";
static char novalidmsg[] = "loadprog: WARNING! - Unable to validate image for this platform\n";

int
loadprog(char *loadname, int loadonly, int argc, char *argv[])
{
    long entry;  /* entry point of program */
    int fmagic;  /* file magic number */
    int i, j, fd, size;
    int loadp;
    struct plb plb;
    struct pib pib;
    int nscns, numread;
    long freememstart, ciaddr;
    struct shstuff fsh;  /* use for a shell file */
    char *bufp, *phys_fptr;
    struct stat_ fstatbuf;
    struct loadinfo loadtbl[LOADTBLENTRIES];
    char buffer[128];
    struct family_check *ic_ptr;

#ifdef DEBUG
    printf("\nentering loadprog(), name = %s\n", loadname);
#endif

    bufp = buffer;
    if((fd = open(loadname, O_RDONLY)) < 0) {
	printf(errmsg, "on file open");
	return(-1);
    }

    if((numread = read(fd, bufp, sizeof(long))) != sizeof(long)) {
	printf(errmsg, "trouble reading file magic number");

#ifdef DEBUG
printf("read system call returned %d, got magic %#x, stack area %#x\n", 
       numread, *(long *)bufp, bufp);
#endif

badload:
	close(fd);
	return(-1);
    }

    lseek(fd, 0, SEEK_SET);  /* rewind the file to the beginning */

    pib.mifp = mifp;
    pib.argc = argc;
    pib.argv = argv;
    freememstart = getfreememstart();
    fmagic = ismagic((unsigned long *)bufp);
    switch(fmagic) {
#ifdef COFFSUPPORT
    case MIPSEBMAGIC_3:  /* mips "ecoff" magic number */

	/* read in the file header and extract info */
	if((numread = read(fd, bufp, sizeof(struct filehdr))) !=
		sizeof(struct filehdr)) {
	    printf(fhmsg, "mips ecoff file");
	    goto badload;
	}
	nscns = ((struct filehdr *)bufp)->f_nscns;

	/* read in a.out header and extract info */
	if((numread = read(fd, bufp, sizeof(AOUTHDR)))
	   != sizeof(AOUTHDR)) {
	    printf(fhmsg, "mips ecoff a.out (optional) header");
	    goto badload;
	}
	entry = ((AOUTHDR *)bufp)->entry;
	
	/* process the section headers and build load table */
	for(i=0, j=0; i<nscns; i++) {
	    if((numread = read(fd, bufp, sizeof(struct scnhdr))) !=
	       sizeof(struct scnhdr)) {
		printf(fhmsg, "mips ecoff section");
		goto badload;
	    }
	    switch (((struct scnhdr *)bufp)->s_flags ) {
	    case STYP_TEXT:
	    case STYP_RDATA:
	    case STYP_DATA:
		if(j >= LOADTBLENTRIES) {
		    printf(errmsg, "too many loadable sections");
		    goto badload;
		}
		loadtbl[j].fptr = ((struct scnhdr *)bufp)->s_scnptr;
		loadtbl[j].size = ((struct scnhdr *)bufp)->s_size;
		loadtbl[j].paddr = ((struct scnhdr *)bufp)->s_paddr;
		j++;
	    }
	}

	/* check section info for sanity */
	for(i=0; i<j; i++) {
	    if(loadtbl[i].paddr < freememstart) {
		printf(errmsg, "program section below free mem start");
		goto badload;
	    }
	    /*
	    ** The launch() routine  will establish a stack at the
	    ** start address of this process.
	    */
	    if(loadtbl[i].paddr < (freememstart + 4096)) {
		printf("loadprog warning: only %#x stack size\n",
		       loadtbl[i].paddr - freememstart);
	    }
	    /*
	    ** Check to see if we will clobber monlib here
	    */
	}

	/* load the sections */
	size = 0;
	for(i=0; i<j; i++) {
	    if(lseek(fd, loadtbl[i].fptr, SEEK_SET) < 0) {
		printf(errmsg, "on seek during program load");
		goto badload;
	    }
	    if((numread = read(fd, (char *)loadtbl[i].paddr, loadtbl[i].size)) !=
	       loadtbl[i].size) {
		printf(errmsg, "on read during program load");
		goto badload;
	    }
	    size += loadtbl[i].size;
	}
	/*
	** File is now loaded.
	** A mechanism is needed here to facilitate loading a
	** program image that will clobber monlib.
	*/
	close(fd);  /* load is complete, close our file */
	break;
#endif   /* COFFSUPPORT */

    case ELFMAGIC:  /* ELF file magic number */
	/* Read the ELF header */
	if((numread = read(fd, bufp, ELFEHDRSZ)) != ELFEHDRSZ) {
	    printf(fhmsg, "ELF");
	    goto badload;
	}

	if(((Elf32_Ehdr *)bufp)->e_type != ET_EXEC && !loadonly) {
	    printf(errmsg, "not an executable ELF file");
#ifdef DEBUG
	    printf("buffer is at: %#x\n",bufp);
#endif
	    goto badload;
	}
	
	if(((Elf32_Ehdr *)bufp)->e_machine != cpu_type) {
	    printf(invmsg);
	    printf("e_machine = %d, cpu_type = %d \n", 
		   ((Elf32_Ehdr *)bufp)->e_machine, cpu_type);
	    goto badload; 
	}
	nscns = ((Elf32_Ehdr *)bufp)->e_shnum;
	entry = ((Elf32_Ehdr *)bufp)->e_entry;

	/* Check that we only have one program header */
	if(((Elf32_Ehdr *)bufp)->e_phnum > 1) {
	    printf(errmsg, "too many ELF program segments");
	    goto badload;
	}

	/* Seek to the the section headers */
	if(lseek(fd, ((Elf32_Ehdr *)bufp)->e_shoff, SEEK_SET) < 0) {
	    printf(errmsg, "ELF file header lseek");
	    goto badload;
	}

	/* process the section headers and build load table */
	for(i=0, j=0; i<nscns; i++) {
	    if((numread = read(fd, bufp, ELFSHDRSZ)) != ELFSHDRSZ) {
		printf(fhmsg, "ELF section");
		goto badload;
	    }

#ifdef DEBUG
	    printf("\nELF section header number %d\n\
section name, index in string tbl: %d\n\
miscellaneous section attributes: %#x\n\
section virtual addr at execution: %#x, size of section in bytes: %#x\n\
entry size if section holds table: %#x, (link) index of another section: %d\n\
additional section information: %#x, section file offset: %#x\n\
section alignment: %#x\n",
i, ((Elf32_Shdr *)bufp)->sh_name, ((Elf32_Shdr *)bufp)->sh_flags,
((Elf32_Shdr *)bufp)->sh_addr, ((Elf32_Shdr *)bufp)->sh_size,
((Elf32_Shdr *)bufp)->sh_entsize, ((Elf32_Shdr *)bufp)->sh_link,
((Elf32_Shdr *)bufp)->sh_info, ((Elf32_Shdr *)bufp)->sh_offset,
((Elf32_Shdr *)bufp)->sh_addralign);

            printf("type of section: ");
	    if(((Elf32_Shdr *)bufp)->sh_type < MAXSCNTYPSTR)
                printf("%s\n", scntypstr[((Elf32_Shdr *)bufp)->sh_type]);
	    else printf("%#x\n", ((Elf32_Shdr *)bufp)->sh_type);

            printf("section flags:\n");
	    if(((Elf32_Shdr *)bufp)->sh_flags & SHF_ALLOC)
                printf("    occupies mem during execution\n");
	    if(((Elf32_Shdr *)bufp)->sh_flags & SHF_WRITE)
                printf("    writable during execution\n");
	    if(((Elf32_Shdr *)bufp)->sh_flags & SHF_EXECINSTR)
                printf("    code\n");
#endif   /* DEBUG */

            if(((Elf32_Shdr *)bufp)->sh_flags & SHF_ALLOC &&
                  ((Elf32_Shdr *)bufp)->sh_size &&
                  ((Elf32_Shdr *)bufp)->sh_type == SHT_PROGBITS) {
		if(j >= LOADTBLENTRIES) {
		    printf(errmsg, "(ELF) too many loadable sections");
		    goto badload;
		}

#ifdef DEBUG
                printf("adding this section to the load table\n");
#endif
		loadtbl[j].fptr = ((Elf32_Shdr *)bufp)->sh_offset;
		loadtbl[j].size = ((Elf32_Shdr *)bufp)->sh_size;
		loadtbl[j].paddr = ((Elf32_Shdr *)bufp)->sh_addr;

                /*
		** Sanity check the beginning and ending address of each
		** loadable section to make sure it does not bus error on
		** read.  Do not check for buserr on write unless you save
		** the contents of the location and write it back - you may
		** corrupt a netbooted image sitting in memory if it
		** overlaps its destination load area.
		*/
                if((loadtbl[j].paddr & 3) ||
		   chkberr((volatile unsigned long *)loadtbl[j].paddr,BW_32BITS,1) ||
		   chkberr((volatile unsigned long *)(loadtbl[j].paddr + loadtbl[j].size),
			   BW_32BITS,1)) {
		    printf(errmsg, "program section linked to illegal address");
		    if(boot_verbose) {
			printf("load address:%#x, size: %#x\n", loadtbl[j].paddr,
			       loadtbl[j].size);
		    }
		    goto badload;
		}

		j++;

	    }
	}
        /*
	** We may want to do further checking of the image sections here
	** to make sure we don't clobber our stack, etc.
	*/


        /* 
         * Check to see if we will clobber monlib here
	 */
         for (i=0; i<j; i++) {
	     if (loadtbl[i].paddr < monlib_upper_bound)
                 break;
         }

         if (i < j) { /* loaded file will clobber the monlib */
	   lseek(fd, 0, SEEK_SET);     /* position the file pointer to the start of file */
           if (fstat(fd, &fstatbuf) < 0) {
              printf(errmsg, "cannot stat file");
              goto badload;
           }
           size = fstatbuf.st_size;
#ifdef DEBUG
           printf("fstatbuf = 0x%x\n", &fstatbuf);
           printf("reading the file into memory...\n");
#endif
	   printf("\n");
           ciaddr = (mainmemstart() + sizemainmem());
           ciaddr -= ((size + 3) & ~3);
           
           if (read(fd, (char *)ciaddr, size) != size) {
              printf(errmsg, "on load of image from file system with monlib");
              goto badload;
           }
           /*
            * File is now in the main memory.
            * Call ourself recursively to handle ELF processing.
            * Loadname arg must now reflect the fact that the image is
            * in main memory
            */
           sprintf(buffer, "%s%x", DEV_KMEM1_ID, ciaddr);
           /* 
            * reset the monlib_upper_bound so that we can recursively
	    * call laodprog.
            */
           monlib_upper_bound = 0;
           return(loadprog(buffer, loadonly, argc, argv));
        } else {
	    /* load the sections */
	    size = 0;
	    for(i=0; i<j; i++) {
              if(lseek(fd, loadtbl[i].fptr, SEEK_SET) < 0) {
		  printf(errmsg, "on seek during ELF program load");
		  goto badload;
              }

            if(boot_verbose)
              printf("loading section to address %#x from file position %#x, size is %#x\n",
                   loadtbl[i].paddr,loadtbl[i].fptr,loadtbl[i].size);

            if((numread = read(fd, (char *)loadtbl[i].paddr, loadtbl[i].size)) !=
	        loadtbl[i].size) {
		  printf(errmsg, "on read during ELF program load");
		  printf("requested %d (%#x) bytes, got %d (%#x)\n",
		       loadtbl[i].size,loadtbl[i].size,numread,numread);
		  goto badload;
	        }
	    size += loadtbl[i].size;
	   }
        }
        /*
	** File is now loaded.
	*/
        break;

     case COMPRESSMAGIC:  /* handle a compressed file */
        if(fstat(fd, &fstatbuf) < 0) {
	    printf(errmsg, "cannot stat file");
            goto badload;
        }
	size = fstatbuf.st_size;
        ciaddr = (mainmemstart() + sizemainmem()) - UNCOMPRESS_DATASIZE;
	plb.plb_ipaddr = ciaddr;  /* tell uncompress where the work area is */
        ciaddr -= ((size + 3) & ~3);
        if(read(fd, (char *)ciaddr, size) != size) {
	    printf(errmsg, "on load of compressed image\n");
            goto badload;
        }
#ifdef UNCOMPRESS_BUILTIN
#define UNCOMPRESS_ADDR SIERRA_FREE_MEM
        size = uncompress((unsigned char *)ciaddr, UNCOMPRESS_ADDR, size);
        if(size == 0) {
	    printf(errmsg,"on uncompress");
            goto badload;
        }
        /*
	** File is now uncompressed in main memory.
	** Call ourself recursively to handle ELF (or other file) processing.
	** Loadname arg must now reflect the fact that the image is
	** in main memory.
	*/
        sprintf(buffer, "%s%x", mon_main_mem_id, UNCOMPRESS_ADDR);
        return(loadprog(buffer, loadonly, argc, argv));
#else
#define UNCOMPRESS_FILENAME "uncompress"

        /*
	** Load the uncompress routine from EPROM.
	*/
        sprintf(buffer, "%s%s", DEV_EPROM_ID, UNCOMPRESS_FILENAME);
        if(loadprog(buffer, 1, 0, (char **)0) < 0) {  /* load only */
	    printf(errmsg, "loading the uncompress program");
            goto badload;
        }

        /*
	** Launch the uncompress routine with info about our image
        */
	plb.plb_str = loadname;  /* point to the name of image to uncompress */
        plb.plb_addr = (char *)ciaddr;
        plb.plb_size = size;

	i = launch(PROM2_BOOTLOAD, &plb, &pib, entrypt);

#ifdef DEBUG
printf("loadprog: returned from uncompress routine, retcode %#x, size: %#x\n",
             i, plb.plb_size);
if(i && plb.plb_size) {
    printf("checksumming image after decompression (location %#x, size %#x) ...",
    plb.plb_addr, plb.plb_size);
    printf("\nchecksum is %#x\n",
	   u_chksum((unsigned char *)plb.plb_addr, plb.plb_size));
}
#endif

        if(!i || !plb.plb_size) {
	    printf(errmsg, "uncompress failed");
            goto badload;
	}
        /*
        ** File is now uncompressed in main memory.
	** Call ourself recursively to handle ELF (or other file) processing.
	** Loadname arg must now reflect the fact that the image is
	** in main memory.
	*/
        sprintf(buffer, "%s%x", mon_main_mem_id, plb.plb_addr);
        return(loadprog(buffer, loadonly, argc, argv));
#endif  /* UNCOMPRESS_BUILTIN */

#ifndef  __mips
    case OMAGIC:  /* 68k cisco_exec (a.out) magic number */
	if((numread = read(fd, bufp, sizeof(struct cisco_exec))) !=
	   sizeof(struct cisco_exec)) {  /* read in header */
	    printf(fhmsg,"a.out");
	    goto badload;
        }
        /* 
	** Check to see if this image is made for this platform?
	*/
        /* need to check cpu_type and interface type here */

#if defined(FUTURE)
	ic_ptr = (struct family_check *) 
                  &((struct cisco_exec *)bufp)->a_trsize;
        if (ic_ptr->magic == FAMILY_CHECK_MAGIC) {
             if ((ic_ptr->family != cpu_type) ||
                  CHECK_INTERFACE(ic_ptr->interface,loadname)) 
                       printf(invmsg);
                       goto badload;
             }
        }
#endif /* defined(FUTURE) */
	entry = ((struct cisco_exec *)bufp)->a_entry;
	size = ((struct cisco_exec *)bufp)->a_text +
           ((struct cisco_exec *)bufp)->a_data;

#ifdef BRASIL
 
#ifdef DEBUG
        printf("\nimageaddr: 0x%x, entry: 0x%x\n", imageaddr, entry);
#endif /* DEBUG */
 
        /*
        ** Check to see if we have a reloc header.
        ** If so we can do image validation.
        */
        spl_hdr = (struct special_header *) (entry);
	if (addr2devnum(imageaddr) != addr2devnum(entry)) {
	    /*
	     * We're netbooting and our netboot image is in RAM now.
	     * Fixup pointer to special header and pick up entry point.
	     */
	    spl_hdr = (struct special_header *) (imageaddr +
			   sizeof (struct cisco_exec));
	    if(spl_hdr->r_magic == RELOC_MAGIC1) {  /* have reloc header */
		entry = spl_hdr->r_def_ram_start;
	    } else {
		/* Fix up pointer */
		spl_hdr = (struct special_header *) (entry);
	    }
        }
	if (spl_hdr->r_magic == RELOC_MAGIC1) {
	    /*
	     * FIXME: put in some type of cpu type check here.
	     */
	    if (spl_hdr->r_platform_family != PLATFORM_FAMTYPE) {
		printf(invmsg);
		goto badload;
	    }
	} else {
	    printf(novalidmsg);
	}
#ifdef DEBUG
        printf("\nentry: 0x%x, size: 0x%x, spl_hdr: 0x%x\n",
				entry, size, spl_hdr);
#endif /* DEBUG */
 
#endif /* BRASIL */

	/*
	** The following read actually puts the program in memory
	** where it is to be run.
	** This, of course, assumes that the entry point of the image
	** is the first instruction.
	*/
	if((numread = read(fd, (char *)entry, size)) != size) {
	    printf("loadprog: trouble reading a.out file\n");
#ifdef DEBUG
printf("expected read size is %#x, returned is %#x\n", size, numread);
#endif
	    goto badload;
	}
	break;
#endif  /* __mips */

    case MONSH_MAGIC:  /* monitor shell script file */
	if(loadonly) {
	    printf(errmsg, "cannot load a script file");
	    goto badload;
	}
	readline(fd, buffer);  /* throw away the rest of this line */
        /* protect against files getting closed by invoke() */
        hkeepflags |= H_SCRIPT;
	while(1) {  /* do till end of file */
	    size = readline(fd, buffer);
	    if(size < 0) break;      /* end of file */
	    if(size == 0) continue;  /* for efficiency skip empty lines */
	    fsh.cmdptr = buffer;
	    shcmdline(&fsh);
	}
        hkeepflags &= ~H_SCRIPT;
	close(fd);
	/* return the rc from the last command from script */
	return(fsh.retval);
    default:
	if(loadonly) {  /* we can load ANYTHING */
	    bufp = (char *)freememstart;
	    while(read(fd, bufp, 1) == 1) bufp++;
	    entry = -1;  /* illegal entry point */
	    size = (int)(bufp - freememstart);
	} else {
	    printf("loadprog: bad file magic number: %#8x\n", fmagic);
	    goto badload;
	}
	break;
    }
    close(fd);  /* load is complete, close our file */
    entrypt = entry;
    loadsize = size;
#if !defined(RSP) && !defined(RSP2) && !defined(PREDATOR) && !defined(SVIP)
    printf("program load complete, entry point: %#x, size: %#x\n",
	   entry, size);
#endif

#ifdef DEBUG
printf("checksumming image at launch location (no file header) ...");
printf("\nchecksum is %#x\n",
       u_chksum((unsigned char *)entry, size));
#endif

    if(!loadonly) {
	/* the pib structure is already set up */
	return(launch(image_launchcode, (struct plb *)0, &pib, entry));
    }
    return(1);  /* success */
}

/*
** Support for monitor shell scripts
** Read a line from file (newline is the delimiter)
** Return the count or -1 on EOF
*/
int
readline(int fd, char *bufp)
{
    int count = 0;  /* init */

    while(1) {
	if(read(fd,bufp,1) != 1) {  /* end of file */
	    if(count) {         /* there is something in the buffer */
termbuf:
		*bufp = '\0';     /* terminate the buffer */
		return(count);
	    } else return(-1);  /* end of file */
	}
	if(*bufp == '\n') goto termbuf;
	count++;
	bufp++;
    }
    return(0);
}
