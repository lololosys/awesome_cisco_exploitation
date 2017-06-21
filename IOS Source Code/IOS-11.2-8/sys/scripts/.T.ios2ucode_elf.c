
ios2ucode_elf.c,797
#define OUTPUT_DIMAGE 90,2325
#define OUTPUT_VPLD 91,2353
#define OUTPUT_DBUS 92,2381
#define DLU_HDR_SKIP	94,2410
#define DLU_HDR_VER	95,2438
#define NONE 98,2487
#define UNIX 99,2502
#define ZIP 100,2517
#define MAX_PHDRS 102,2533
#define MAX_SECTIONS	103,2562
#define TRUE 105,2589
#define FALSE 106,2606
typedef int boolean;boolean108,2624
} dlu_hdr_t;dlu_hdr_t121,2847
} dlu_trlr_t;dlu_trlr_t125,2902
typedef struct INPUT_SEC INPUT_SEC127,2917
} INPUT_SEC;INPUT_SEC132,3038
unsigned short longcrc 169,3773
unsigned short add_to_cksum(212,5123
unsigned short update_cksum(222,5474
int set_load_flag 243,5999
void process_elf 266,6425
void process_addit 526,13309
void process_mkdlx 584,14914
void usage 648,16582
void process_args 667,17133
void main 737,18382
