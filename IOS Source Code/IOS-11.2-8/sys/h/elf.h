/* $Id: elf.h,v 3.2 1995/11/17 09:12:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/elf.h,v $
 *------------------------------------------------------------------
 * elf.h - ELF object file header file
 *
 * January 1994, Michael Beesley
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: elf.h,v $
 * Revision 3.2  1995/11/17  09:12:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:58:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:34:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file is copyrighted by FSF. We need to add their disclaimer
 * here once the legal issues get resolved
 */

/*
 * Elf data structures and values
 */
typedef unsigned short	Elf32_Half;
typedef unsigned int	Elf32_Word;
typedef signed int	Elf32_Sword;
typedef unsigned int	Elf32_Off;
typedef unsigned int	Elf32_Addr;
typedef unsigned char	Elf_Char;

#define EI_NIDENT	16

typedef struct {
    Elf_Char	e_ident[EI_NIDENT];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    Elf32_Word	sh_name;
    Elf32_Word	sh_type;
    Elf32_Word	sh_flags;
    Elf32_Addr	sh_addr;
    Elf32_Off	sh_offset;
    Elf32_Word	sh_size;
    Elf32_Word	sh_link;
    Elf32_Word	sh_info;
    Elf32_Word	sh_addralign;	/* power 2 ?? */
    Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct {
  Elf32_Word	p_type;			/* Identifies program segment type */
  Elf32_Off	p_offset;		/* Segment file offset */
  Elf32_Addr	p_vaddr;		/* Segment virtual address */
  Elf32_Addr	p_paddr;		/* Segment physical address */
  Elf32_Word	p_filesz;		/* Segment size in file */
  Elf32_Word	p_memsz;		/* Segment size in memory */
  Elf32_Word	p_flags;		/* Segment flags */
  Elf32_Word	p_align;		/* Segment alignment, file & memory */
} Elf32_Phdr;

typedef struct {
    Elf32_Word	st_name;
    Elf32_Addr	st_value;
    Elf32_Word	st_size;
    Elf_Char	st_info;
    Elf_Char	st_other;
    Elf32_Half	st_shndx;
} Elf32_Sym;

/* we will use these given nothing else exists (they are rather big !!)
 */
typedef struct {
    Elf32_Addr		r_offset;
    Elf32_Word		r_info;
    Elf32_Sword		r_addend;
} Elf32_Rela;

typedef struct {
    Elf32_Addr		r_offset;
    Elf32_Word		r_info;
} Elf32_Rel;

/* these are the external file sizes (we hope) 
 */
#define ELFEHDRSZ	sizeof(Elf32_Ehdr)
#define ELFPHDRSZ       sizeof(Elf32_Phdr)
#define ELFSHDRSZ	sizeof(Elf32_Shdr)
#define ELFSYMSZ	sizeof(Elf32_Sym)
#define ELFRELASZ	sizeof(Elf32_Rela)
#define ELFRELSZ	sizeof(Elf32_Rel)

#if !defined(OBJECT_4K)

size_t elfEhdrRead(FILE *,Elf32_Ehdr *);

size_t elfEhdrWrite(FILE *,const Elf32_Ehdr *);

size_t elfShdrRead(FILE *,const Elf32_Ehdr *, Elf32_Shdr *,size_t);

size_t elfShdrWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,size_t);

size_t elfSymRead(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		  Elf32_Sym *,size_t);
size_t elfSymWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Sym *,size_t);

size_t elfRelaWrite(FILE *,const Elf32_Ehdr *,const Elf32_Shdr *,
		   const Elf32_Rela *,size_t);

#endif /* OBJECT_4K */

/* various special values */

/* Fields in e_ident[] */

#define EI_MAG0		0		/* File identification byte 0 index */
#define ELFMAG0		0x7F		/* Magic number byte 0 */

#define EI_MAG1		1		/* File identification byte 1 index */
#define ELFMAG1		'E'		/* Magic number byte 1 */

#define EI_MAG2		2		/* File identification byte 2 index */
#define ELFMAG2		'L'		/* Magic number byte 2 */

#define EI_MAG3		3		/* File identification byte 3 index */
#define ELFMAG3		'F'		/* Magic number byte 3 */

#define EI_CLASS	4		/* File class */
#define ELFCLASSNONE	0		/* Invalid class */
#define ELFCLASS32	1		/* 32-bit objects */
#define ELFCLASS64	2		/* 64-bit objects */

#define EI_DATA		5		/* Data encoding */
#define ELFDATANONE	0		/* Invalid data encoding */
#define ELFDATA2LSB	1		/* 2's complement, little endian */
#define ELFDATA2MSB	2		/* 2's complement, big endian */

#define EI_VERSION	6		/* File version */

#define EI_PAD		7		/* Start of padding bytes */

/* Values for e_type, which identifies the object file type */

#define ET_NONE		0		/* No file type */
#define ET_REL		1		/* Relocatable file */
#define ET_EXEC		2		/* Executable file */
#define ET_DYN		3		/* Shared object file */
#define ET_CORE		4		/* Core file */
#define ET_LOPROC	0xFF00		/* Processor-specific */
#define ET_HIPROC	0xFFFF		/* Processor-specific */

/* Values for e_machine, which identifies the architecture */

#define EM_NONE		0		/* No machine */
#define EM_M32		1		/* AT&T WE 32100 */
#define EM_SPARC	2		/* SUN SPARC */
#define EM_386		3		/* Intel 80386 */
#define EM_68K		4		/* Motorola m68k family */
#define EM_88K		5		/* Motorola m88k family */
#define EM_860		7		/* Intel 80860 */
#define EM_MIPS		8		/* MIPS R3000  */

/* Values for e_version */

#define EV_NONE		0		/* Invalid ELF version */
#define EV_CURRENT	1		/* Current version */

/* Values for program header, p_type field */

#define	PT_NULL		0		/* Program header table entry unused */
#define PT_LOAD		1		/* Loadable program segment */
#define PT_DYNAMIC	2		/* Dynamic linking information */
#define PT_INTERP	3		/* Program interpreter */
#define PT_NOTE		4		/* Auxiliary information */
#define PT_SHLIB	5		/* Reserved, unspecified semantics */
#define PT_PHDR		6		/* Entry for header table itself */
#define PT_LOPROC	0x70000000	/* Processor-specific */
#define PT_HIPROC	0x7FFFFFFF	/* Processor-specific */

/* Program segment permissions, in program header p_flags field */

#define PF_X		(1 << 0)	/* Segment is executable */
#define PF_W		(1 << 1)	/* Segment is writable */
#define PF_R		(1 << 2)	/* Segment is readable */
#define PF_MASKPROC	0xF0000000	/* Processor-specific reserved bits */

/* Values for section header, sh_type field */

#define SHT_NULL	0		/* Section header table entry unused */
#define SHT_PROGBITS	1		/* Program specific (private) data */
#define SHT_SYMTAB	2		/* Link editing symbol table */
#define SHT_STRTAB	3		/* A string table */
#define SHT_RELA	4		/* Relocation entries with addends */
#define SHT_HASH	5		/* A symbol hash table */
#define SHT_DYNAMIC	6		/* Information for dynamic linking */
#define SHT_NOTE	7		/* Information that marks file */
#define SHT_NOBITS	8		/* Section occupies no space in file */
#define SHT_REL		9		/* Relocation entries, no addends */
#define SHT_SHLIB	10		/* Reserved, unspecified semantics */
#define SHT_DYNSYM	11		/* Dynamic linking symbol table */
#define SHT_LOPROC	0x70000000	/* Processor-specific semantics, lo */
#define SHT_HIPROC	0x7FFFFFFF	/* Processor-specific semantics, hi */
#define SHT_LOUSER	0x80000000	/* Application-specific semantics */
#define SHT_HIUSER	0x8FFFFFFF	/* Application-specific semantics */

/* Values for section header, sh_flags field */

#define SHF_WRITE	(1 << 0)	/* Writable data during execution */
#define SHF_ALLOC	(1 << 1)	/* Occupies memory during execution */
#define SHF_EXECINSTR	(1 << 2)	/* Executable machine instructions */
#define SHF_MASKPROC	0xF0000000	/* Processor-specific semantics */

/* Values of note segment descriptor types for core files. */

#define NT_PRSTATUS	1		/* Contains copy of prstatus struct */
#define NT_FPREGSET	2		/* Contains copy of fpregset struct */
#define NT_PRPSINFO	3		/* Contains copy of prpsinfo struct */

/* These three macros disassemble and assemble a symbol table st_info field,
   which contains the symbol binding and symbol type.  The STB_ and STT_
   defines identify the binding and type. */

#define ELF_ST_BIND(val)		(((unsigned int)(val)) >> 4)
#define ELF_ST_TYPE(val)		((val) & 0xF)
#define ELF_ST_INFO(bind,type)		(((bind) << 4) | ((type) & 0xF))

#define STB_LOCAL	0		/* Symbol not visible outside obj */
#define STB_GLOBAL	1		/* Symbol visible outside obj */
#define STB_WEAK	2		/* Like globals, lower precedence */
#define STB_LOPROC	13		/* Application-specific semantics */
#define STB_HIPROC	15		/* Application-specific semantics */

#define STT_NOTYPE	0		/* Symbol type is unspecified */
#define STT_OBJECT	1		/* Symbol is a data object */
#define STT_FUNC	2		/* Symbol is a code object */
#define STT_SECTION	3		/* Symbol associated with a section */
#define STT_FILE	4		/* Symbol gives a file name */
#define STT_LOPROC	13		/* Application-specific semantics */
#define STT_HIPROC	15		/* Application-specific semantics */

/* Special section indices, which may show up in st_shndx fields, among
   other places. */

#define SHN_UNDEF	0		/* Undefined section reference */
#define SHN_LORESERV	0xFF00		/* Begin range of reserved indices */
#define SHN_LOPROC	0xFF00		/* Begin range of appl-specific */
#define SHN_HIPROC	0xFF1F		/* End range of appl-specific */
#define SHN_ABS		0xFFF1		/* Associated symbol is absolute */
#define SHN_COMMON	0xFFF2		/* Associated symbol is in common */
#define SHN_HIRESERVE	0xFFFF		/* End range of reserved indices */

#define ELF32_R_SYM(i)		((i) >> 8)
#define ELF32_R_TYPE(i)		((i) & 0xff)
#define ELF32_R_INFO(s,t)	(((s) << 8) | ((t) & 0xff))

#define ELF32_ST_BIND(i)	((i) >> 4)
#define ELF32_ST_TYPE(i)	((i) & 0xf)
#define ELF32_ST_INFO(b,t)	(((b) << 4) | ((t) & 0xf))

/* End of file */

