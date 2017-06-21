/* $Id: module.c,v 3.2 1995/11/17 18:49:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/module.c,v $
 *------------------------------------------------------------------
 * module.c - Loadable modules
 *
 * November 11, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This routine is responsible for loading and managing loadable software
 * modules.
 *
 *------------------------------------------------------------------
 * $Log: module.c,v $
 * Revision 3.2  1995/11/17  18:49:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  08:56:16  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  21:56:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "module.h"
#include "module_externs.h"
#include "module_private.h"
#include "core.h"
#include "address.h"
#include "config.h"
#include "../os/boot.h"
#include "file.h"
#include "../os/export.h"

/*
 * Local storage
 */

queuetype moduleQ;		/* List of defined modules */
core_vector coreV = {
    CORE_MAJVER, /* Major version number */
    CORE_MINVER, /* Minor version number */
    sizeof(core_vector), /* Size of vector */
    export_import /* Import handler */
  };

/*
 * Local function prototypes
 */

static boolean module_header_check(module_stream *, module_header *);
static void module_free_sections(module_info *);
static boolean module_allocate_sections(module_info *);
static boolean module_load_section_prologue(module_stream *,
					    section_prologue *,
					    int);
static void module_free_relocation_data(relocation_prologue **);
static boolean module_load_relocation_data(module_stream *,
					   module_info *,
					   relocation_prologue **,
					   ulong);
static boolean module_load_data_segment(module_stream *,
					ulong,
					module_info *);
static void module_apply_relocation(section_prologue *,
				       module_info *,
				       relocation_prologue **);
static boolean module_load_sections(module_stream *, module_info *);

static void module_close(module_stream *);
static module_stream *module_open(char *);
static boolean module_read(module_stream *, void *, ulong);

/*
 * High-level routines
 */

/*
 * module_load_file - Load a module into memory from a file
 * 
 * This routine is used to load a module into memory from a file. We
 * are passed a filename, and we load the module, relocate it, and call
 * it at its init entry point. If the process is successful, we add this
 * module to the current load list.
 */

boolean module_load_file (char *filename)
{
    module_stream *stream;
    module_info *modinfo;
    boolean (*entry)(enum MODULE_FUNCTION, core_vector *);

    /*
     * Attempt to allocate a module information block
     */

    modinfo = malloc(sizeof(module_info)); /* Get a module information block */
    if (!modinfo) {
	printf(nomemory);
	return(FALSE);
    }

    /*
     * Open the input file
     */

    stream = module_open(filename); /* Attempt to open the module */
    if (!stream) {
	free(modinfo);
	return(FALSE);		/* If we can't, fail */
    }

    /*
     * Validate the image
     */

    if (!module_header_check(stream, &modinfo->header)) {
	free(modinfo);
	module_close(stream);
	return(FALSE);
    }

    /*
     * Allocate memory space for each section
     */

    if (!module_allocate_sections(modinfo)) {
	free(modinfo);
	module_close(stream);
	return(FALSE);
    }

    /*
     * Now process each section
     */

    if (!module_load_sections(stream, modinfo)) {
	module_free_sections(modinfo);
	free(modinfo);
	module_close(stream);
	return(FALSE);
    }

    /*
     * Close the input stream
     */

    module_close(stream);

    /*
     * Calculate the entry point
     */

    entry = (boolean (*)(enum MODULE_FUNCTION, core_vector *))
	(modinfo->baseaddr[modinfo->header.mod_entrysec] +
	 modinfo->header.mod_entryoff);


    /*
     * Call the module we just loaded to see if it wants to run
     */

    if (!((*entry)(MODULE_INIT, &coreV))) {
	module_free_sections(modinfo); /* Free the module we loaded */
	free(modinfo);		/* Free the header structure */
	printf("\n%% Load module is not compatible with this system");
	return(FALSE);
    }

    /*
     * Success, link this module into the module list
     */

    enqueue(&moduleQ, modinfo);

    return(TRUE);		/* All done */
}

/*
 * module_find_id - Find a module by canonical ID
 */

module_info *module_find_id (ulong id)
{
    module_info *ptr;

    for (ptr = (module_info *)moduleQ.qhead; ptr; ptr = ptr->next) {
	if (ptr->header.mod_id == id)
	  return(ptr);
    }
    return(NULL);		/* Module not found */
}

/*
 * Helper routines to read the various image sections
 */

/*
 * module_header_check - Read header and do a validity check
 */

static boolean
module_header_check (module_stream *stream, module_header *header)
{
    module_info *othermod;
    boolean status;

    /*
     * Read in the header
     */

    status = module_read(stream, header, sizeof(module_header));
    if (!status)
      return(FALSE);

    /*
     * Check the magic number. This must be an exact match.
     */

    if (header->hdr_magic != MODULE_MAGIC) {
	printf("\n%% bad magic number %x in load module",
	       header->hdr_magic);
	return(FALSE);
    }

    /*
     * Check the header major version number. This must be an exact
     * match. Backwards compatible changes should be done as a change
     * to the minor version number.
     */

    if (header->hdr_majver != MODULE_MAJVER) {
	printf("\n%% Unsupported major header version %d in load module",
	       header->hdr_majver);
	return(FALSE);
    }

    /*
     * Check the header minor version number. If the minor version of
     * this module exceeds our minor version, then we should not load
     * this image.
     *
     * Backwards compatibility should be handled here. If the version
     * of this module is *less* than the version that we directly support,
     * then update (in memory) the header structure here.
     */

    if (header->hdr_minver > MODULE_MINVER) {
	printf("\n%% Unsupported minor header version %d in load module",
	       header->hdr_minver);
	return(FALSE);
    }

    /*
     * Check the core major version number for compatibility. This must be
     * an exact match, because a change of the core version number indicates
     * that all of the module's imports from core will be invalid.
     */

    if (header->core_majver != CORE_MAJVER) {
	printf("\n%% Unsupported major core version %d in load module",
	       header->core_majver);
	return(FALSE);
    }

    /*
     * Check the core minor version number for compatibility. If the minor
     * version of the expected core exceeds our minor version, then we should
     * not load this module, because its import vector will contain
     * pointers that are not defined in this core. However, we can always
     * load modules that require older core versions.
     *
     * Note that unlike older header versions, there are no backwards
     * compatibility issues to deal with.
     */

    if (header->core_minver > CORE_MINVER) {
	printf("\n%% Unsupported minor core version %d in load module",
	       header->core_minver);
	return(FALSE);
    }

    /*
     * Check the section maximum. This shouldn't exceed our maximum ever,
     * but check anyway so that a bad file doesn't cause us to crash.
     */

    if ((header->sec_cnt > MODULE_SECTIONMAX) ||
	(header->sec_cnt == 0)) {
	printf("\n%% Unsupported section count %d in load module",
	       header->sec_cnt);
	return(FALSE);
    }

    /*
     * Check the entry point section number
     */

    if (header->mod_entrysec > header->sec_cnt) {
	printf("\n%% Entry section %d exceeds maximum %d in load module",
	       header->mod_entrysec, header->sec_cnt);
	return(FALSE);
    }

    /*
     * Check the entry point offset
     */

    if (header->mod_entryoff > header->sec_info[header->mod_entrysec].size) {
	printf("\n%% Entry point offset %x exceeds section %d size %x",
	       header->mod_entryoff, header->mod_entrysec,
	       header->sec_info[header->mod_entrysec].size);
	return(FALSE);
    }

    /*
     * Check the length of the module name.
     */

    if ((header->mod_namelen > MODULE_NAMEMAX) ||
	(header->mod_namelen == 0)) {
	printf("\n%% Unsupported section count %d in load module",
	       header->sec_cnt);
	return(FALSE);
    }
	
    /*
     * See if there is another module with this module ID already loaded
     */

    othermod = module_find_id(header->mod_id);
    if (othermod) {
	printf("\n%% Another module with id %d (%*s) already loaded",
	       header->mod_id, othermod->header.mod_namelen,
	       othermod->header.mod_name);
	return(FALSE);
    }

    return(TRUE);		/* Looks good from here */
}

/*
 * module_load_sections - Load all of the sections in the input file
 *
 * This is the high-level controller responsible for loading and relocating
 * all sections in the input file.
 */

static boolean
module_load_sections (module_stream *stream, module_info *modinfo)
{
    int section;
    int reloc;
    int data;
    section_prologue secinfo;
    relocation_prologue *reloc_table[MODULE_SECTIONMAX];

    for (section = 0; section < modinfo->header.sec_cnt; section++) {

	/*
	 * Load and check the section prologue
	 */

	if (!module_load_section_prologue(stream, &secinfo, section))
	  return(FALSE);

	/*
	 * Zero all of the relocation table pointers
	 */

	for (reloc = 0; reloc < MODULE_SECTIONMAX; reloc++)
	  reloc_table[reloc] = NULL;

	/*
	 * Read all of the relocation tables into buffers
	 */

	for (reloc = 0; reloc < secinfo.relcnt; reloc++) {
	    if (!module_load_relocation_data
		(stream, modinfo, reloc_table, secinfo.secnum)) {
		module_free_relocation_data(reloc_table);
		return(FALSE);
	    }
	}

	/*
	 * Process all of the data segments
	 */

	for (data = 0; data < secinfo.datacnt; data++) {
	    if (!module_load_data_segment
		(stream, section, modinfo )) {
		module_free_relocation_data(reloc_table);
		return(FALSE);
	    }
	}

	/*
	 * Process the relocation information
	 */

	module_apply_relocation(&secinfo, modinfo, reloc_table);

	/*
	 * Free up the relocation information
	 */

	module_free_relocation_data(reloc_table);

    }
    return(TRUE);
}

/*
 * module_load_section_prologue - Load and check a section prologue
 *
 * This routine loads and checks the section prologue.
 */

static boolean
module_load_section_prologue (
    module_stream *stream,
    section_prologue *secinfo,
    int section)
{
    boolean status;

    /*
     * Read and validate the section header
     */

    status = module_read(stream, secinfo, sizeof(section_prologue));
    if (!status)
      return(FALSE);

    if (secinfo->type != MODULE_TYPE_SECTION) {
	printf("\n%% Unexpected type %x when reading section %x header",
	       secinfo->type, section);
	return(FALSE);
    }

    if (secinfo->secnum != section) {
	printf("\n%% Unexpected section number %x - expecting %x",
	       secinfo->secnum, section);
	return(FALSE);
    }

    if (secinfo->relcnt > MODULE_SECTIONMAX) {
	printf("\n%% Unexpected relocation count %x in section %x",
	       secinfo->relcnt, section);
	return(FALSE);
    }
    return(TRUE);		/* Success */
}


/*
 * module_load_relocation_data - Load and check the relocation data
 */

static boolean
module_load_relocation_data (
    module_stream *stream,
    module_info *modinfo,
    relocation_prologue **reloc_table,
    ulong secnum)
{
    boolean status;
    relocation_prologue relinfo;
    relocation_prologue *reldata;
    int relitem;

    /*
     * Read the relocation prologue
     */

    status = module_read(stream, &relinfo, sizeof(relocation_prologue));
    if (!status)
      return(FALSE);

    /*
     * Check the prologue contents
     */

    if (relinfo.type != MODULE_TYPE_RELOC) {
	printf("\n%% Unexpected type %x reading relocation for section %x",
	       relinfo.type, secnum);
	return(FALSE);
    }

    if (relinfo.secnum >= modinfo->header.sec_cnt) {
	printf("\n%% Relocation section number %x exceeds module maximum %x",
	       relinfo.secnum, modinfo->header.sec_cnt);
	return(FALSE);
    }	

    if (reloc_table[relinfo.secnum]) {
	printf("\n%% Duplicate relocation information (section %x)"
	       "for section %x", relinfo.secnum, secnum);
	return(FALSE);
    }

    if (relinfo.count == 0) {
	printf("\n%% Relocation block with zero items in section %x",
	       secnum);
	return(FALSE);
    }

    /*
     * Allocate a buffer to hold the prologue and the data
     */

    reldata = malloc(sizeof(relocation_prologue) +
		     (relinfo.count  * sizeof(void *)));

    if (!reldata)
      return(FALSE);
    
    /*
     * Save the prologue away in the buffer
     */

    bcopy(&relinfo, reldata, sizeof(relocation_prologue));

    /*
     * Now read the relocation data itself
     */

    status = module_read(stream, reldata->items,
			 relinfo.count * sizeof(ulong));
    if (!status) {
	free(reldata);
	return(FALSE);
    }

    /*
     * Ensure that the fixup items are sorted and aligned
     */

    for (relitem = 0; relitem < relinfo.count; relitem++) {

	/*
	 * Check for proper alignment
	 */

	if ((reldata->items[relitem] & (CPU_LONG_ALIGN - 1)) != 0) {
	    printf("\n%% Section %x/%x relocation item %d invalid alignment, "
		   "fixup = %x", secnum, relinfo.secnum,
		   relitem, reldata->items[relitem]);
	    free(reldata);
	    return(FALSE);
	}

	/*
	 * Ensure that the fixup addresses are within range 
	 */

	if (reldata->items[relitem] > modinfo->header.sec_info[secnum].size) {
	    printf("\n%% Relocation address %x out of range - section %x/%x",
		   reldata->items[relitem], secnum, relinfo.secnum);
	    free(reldata);
	    return(FALSE);
	}

	/*
	 * Ensure that this list is sorted
	 */

	if (relitem == 0)
	  continue;		/* Skip sort check for first item */

#ifdef NOTYET
	if (reldata->items[relitem] < reldata->items[relitem - 1]) {
	    printf("\n%% Relocation item %d is out of sequence",
		   relitem);
	    free(reldata);
	    return(FALSE);
	}
#endif NOTYET
    }

    reloc_table[relinfo.secnum] = reldata; /* Save pointer to data */

    return(TRUE);		/* Success */
}

/*
 * module_load_data_segment - Load a data segment
 *
 * This routine loads a data segment into memory. Note that this does not
 * perform relocation here - while this will be necessary for FLASH, it makes
 * the code much more complex so it is not being done in this first pass.
 */

static boolean
module_load_data_segment (
    module_stream *stream,
    ulong secnum,
    module_info *modinfo)
{
    data_prologue datainfo;
    boolean status;

    /*
     * Read the data prologue
     */

    status = module_read(stream, &datainfo, sizeof(data_prologue));
    if (!status)
      return(FALSE);

    /*
     * Check the prologue contents
     */

    /*
     * Check that this is the proper type code
     */

    if (datainfo.type != MODULE_TYPE_DATA) {
	printf("\n%% Unexpected type %x looking for data segment %x",
	       datainfo.type, secnum);
	return(FALSE);
    }

    /*
     * Check that this is the section we expect to see data for
     */

    if (datainfo.secnum != secnum) {
	printf("\n%% Found section %x when expecting %x",
	       datainfo.secnum, secnum);
	return(FALSE);
    }

    /*
     * Ensure that the start address is within the section
     */

    if (datainfo.addr_offset > modinfo->header.sec_info[secnum].size) {
	printf("\n%% Start address %x exceeds segment %x length %x",
	       datainfo.addr_offset, secnum,
	       modinfo->header.sec_info[secnum].size);
	return(FALSE);
    }

    /*
     * Ensure that the end address is within the section
     */

    if ((datainfo.addr_offset + datainfo.count) >
	modinfo->header.sec_info[secnum].size) {
	printf("\n%% End address %x exceeds segment %x length %x",
	       datainfo.addr_offset + datainfo.count, secnum,
	       modinfo->header.sec_info[secnum].size);
	return(FALSE);
    }

    /*
     * Everything checks out - read the data into the buffer
     */

    status = module_read(stream, 
			 modinfo->baseaddr[secnum] + datainfo.addr_offset,
			 datainfo.count);
    if (!status)
      return(FALSE);

    return(TRUE);		/* Success */
}

/*
 * module_apply_relocation - Apply the relocation information
 *
 * This routine is used to apply the saved relocation information to the
 * segment that we just read. Note that this is a simplification - we
 * should process the relocation information on the fly since this is
 * necessary for FLASH programming (no random-access flash writes).
 */

static void
module_apply_relocation (
    section_prologue *secinfo,
    module_info *modinfo,
    relocation_prologue **reloc_table)
{
    int section;
    int reloc;
    relocation_prologue *relinfo;

    for (section = 0; section < modinfo->header.sec_cnt; section++) {
	relinfo = reloc_table[section];	/* Get pointer to reloc info */
	if (!relinfo)
	  continue;		/* No relocation information for section */

	/*
	 * Perform all of the relocation entries
	 */

	for (reloc = 0; reloc < relinfo->count; reloc++) {
	    *(ulong *)&(modinfo->baseaddr[secinfo->secnum]
		       [relinfo->items[reloc]]) +=
			 (modinfo->baseaddr[relinfo->secnum] -
			  modinfo->header.sec_info[relinfo->secnum].linkaddr);
	}
    }	      
}

/*
 * Memory management routines
 */

/*
 * module_allocate_sections - Allocate memory for a module's sections
 *
 * This routine is responsible for allocating the memory used for each
 * of the sections in a module being loaded.
 */

static boolean
module_allocate_sections (module_info *modinfo)
{
    int section;

    for (section = 0; section < modinfo->header.sec_cnt; section++) {
	if (!modinfo->header.sec_info[section].size)
	  continue;		/* Skip empty sections */

	if (modinfo->header.sec_info[section].flags & SECTION_FAST) {
	    modinfo->baseaddr[section] =
	      malloc_fast(modinfo->header.sec_info[section].size);
	} else {
	    modinfo->baseaddr[section] =
	      malloc(modinfo->header.sec_info[section].size);
	}
	if (!modinfo->baseaddr[section]) {
	    printf(nomemory);
	    module_free_sections(modinfo); /* Free the sections */
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * module_free_sections - Free memory for a module's sections
 *
 * This routine is used to free the memory used for a load module.
 */

static void 
module_free_sections (module_info *modinfo)
{
    int section;

    for (section = 0; section < modinfo->header.sec_cnt; section++) {
	if (modinfo->baseaddr[section])
	  free(modinfo->baseaddr[section]);
    }
}

/*
 * module_free_relocation_data - Free the volatile relocation data
 */

static void
module_free_relocation_data (relocation_prologue **reloc_table)
{
    int reloc;

    for (reloc = 0; reloc < MODULE_SECTIONMAX; reloc++) {
	if (reloc_table[reloc])
	  free(reloc_table[reloc]);
	reloc_table[reloc] = NULL;
    }
}

/*
 * Temporary I/O routines
 *
 * In order to make things simple, we just use TFTP to read the entire
 * file into the buffer at "open" time, and then deliver parts of it at
 * read time.
 *
 * We really need to have efficient, device-independent (TFTP, MOP, FLASH,
 * ROM) file I/O for this to work well.
 */

/*
 * module_open - Open a load module
 */

static module_stream *
module_open (char *filename)
{
    module_stream *stream;
    addrtype reqhost = { ADDR_IP, ADDRLEN_IP, (ipaddrtype) { 0xFFFFFFFF } };
    filetype file;

    stream = malloc(sizeof(module_stream)); /* Allocate the I/O buffer */

    if (!stream) {
	printf(nomemory);
	return(NULL);
    }

    stream->filesize = MODULE_MAX_FILESIZE; /* Set maximum filesize */

    /*
     * Set up a file descriptor, then read the file.
     */
    file.proto = FILE_ACCESS_TFTP;
    bcopy(&reqhost, &file.ps.tuba.boothost, sizeof(addrtype));
    file.filename = filename;
    if (reg_invoke_file_read(FILE_ACCESS_TFTP,
			     (filetype*)&file, stream->buffer,
			     &stream->filesize, TRUE, NULL, NULL) != ERROR_OK) {
	free(stream);
	return(NULL);
    }

    return(stream);		/* Success! */
}

static boolean
module_read (module_stream *stream, void *buf, ulong len)
{
    if (stream->offset + len > stream->filesize) {
	printf("\n%% Module load error - read past EOF");
	return(FALSE);
    }

    bcopy(&stream->buffer[stream->offset], buf, len); /* Move the data */
    stream->offset += len;	/* Adjust the read offset for next time */
    return(TRUE);
}

static void
module_close (module_stream *stream)
{
    free(stream);		/* That's easy enough to do... */
}
