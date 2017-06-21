
glue_elf.c,849
#define TRUE 50,1328
#define FALSE 51,1344
#define ZIP_COMMAND 53,1361
#define UNZIP_COMMAND	54,1391
#define ELFPHDRSZ 59,1450
#define ELF_EHDR(ELF_EHDR60,1494
#define ELF_PHDR(ELF_PHDR62,1544
#define ELF_SHDR(ELF_SHDR66,1668
#define ELF_SEGMENT(ELF_SEGMENT70,1792
#define SHDR_NOBIT(SHDR_NOBIT73,1884
#define SHDR_PROGBITS(SHDR_PROGBITS76,1992
#define NOT_LONGWORD_ALIGNED(NOT_LONGWORD_ALIGNED79,2085
} MagicHdrType;MagicHdrType90,2332
} FileInfoType;FileInfoType98,2457
} CommandType;CommandType105,2535
void usage 125,2854
CommandType command_line 142,3424
void read_image 179,4435
void save_image 238,5702
void verify_elf_file 348,8737
unsigned long image_checksum 382,9519
void extract_zip_image 406,10099
void squish_elf_file 499,12600
void unzip_image 743,21583
void zip_image 790,22767
int main 866,25000
