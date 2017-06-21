
gdb_68.h,1525
#define NUMREGBYTES 33,1075
#define NUMREGS 34,1099
#define REGBYTES 36,1132
typedef void (*SIG_PF)SIG_PF41,1212
typedef struct FrameStructFrameStruct46,1311
} Frame;Frame55,1689
typedef struct cpu_context_ cpu_context_61,1789
} cpu_context_t;cpu_context_t64,1916
#define BREAKPOINT(BREAKPOINT69,1998
enum regnames regnames72,2044
enum regnames {D0,72,2044
enum regnames {D0,D1,72,2044
enum regnames {D0,D1,D2,72,2044
enum regnames {D0,D1,D2,D3,72,2044
enum regnames {D0,D1,D2,D3,D4,72,2044
enum regnames {D0,D1,D2,D3,D4,D5,72,2044
enum regnames {D0,D1,D2,D3,D4,D5,D6,72,2044
enum regnames {D0,D1,D2,D3,D4,D5,D6,D7,72,2044
               A0,73,2085
               A0,A1,73,2085
               A0,A1,A2,73,2085
               A0,A1,A2,A3,73,2085
               A0,A1,A2,A3,A4,73,2085
               A0,A1,A2,A3,A4,A5,73,2085
               A0,A1,A2,A3,A4,A5,A6,73,2085
               A0,A1,A2,A3,A4,A5,A6,A7,73,2085
               PS,74,2126
               PS,PC,74,2126
               FP0,75,2148
               FP0,FP1,75,2148
               FP0,FP1,FP2,75,2148
               FP0,FP1,FP2,FP3,75,2148
               FP0,FP1,FP2,FP3,FP4,75,2148
               FP0,FP1,FP2,FP3,FP4,FP5,75,2148
               FP0,FP1,FP2,FP3,FP4,FP5,FP6,75,2148
               FP0,FP1,FP2,FP3,FP4,FP5,FP6,FP7,75,2148
               FPCONTROL,76,2196
               FPCONTROL,FPSTATUS,76,2196
               FPCONTROL,FPSTATUS,FPIADDR76,2196
	       ,FPCODE,77,2238
	       ,FPCODE,FPFLAGS77,2238
