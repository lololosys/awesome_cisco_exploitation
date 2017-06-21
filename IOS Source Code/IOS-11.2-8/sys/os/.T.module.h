
module.h,1030
#define __MODULE_H__27,827
#define CPU_LONG_ALIGN	42,1199
#define MODULE_MAJVER 48,1306
#define MODULE_MINVER 49,1372
#define MODULE_MAGIC 51,1439
#define MODULE_TYPE_RESERVED	53,1470
#define MODULE_TYPE_SECTION	54,1522
#define MODULE_TYPE_RELOC	55,1573
#define MODULE_TYPE_DATA	56,1623
#define MODULE_NAMEMAX 63,1800
#define MODULE_SECTIONMAX 64,1861
#define MODULE_68000	73,2114
#define MODULE_CPU32	74,2186
#define MODULE_68020	75,2250
#define SECTION_READONLY	81,2347
#define SECTION_FAST	82,2410
typedef struct section_header_ section_header_92,2713
} section_header;section_header96,2890
typedef struct section_prologue_ section_prologue_107,3221
} section_prologue;section_prologue112,3454
typedef struct relocation_prologue_ relocation_prologue_125,3851
} relocation_prologue;relocation_prologue130,4065
typedef struct data_prologue_ data_prologue_143,4482
} data_prologue;data_prologue149,4735
typedef struct module_header_ module_header_155,4801
} module_header;module_header171,5687
