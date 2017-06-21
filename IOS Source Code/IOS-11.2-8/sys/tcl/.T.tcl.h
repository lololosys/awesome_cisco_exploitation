
tcl.h,4574
#define _TCL74,2882
#define TCL_VERSION 77,2897
#define TCL_MAJOR_VERSION 78,2923
#define TCL_MINOR_VERSION 79,2951
#define TCL_GENERIC_ONLY84,3074
#define NO_STRING_H86,3105
#define NO_LIMITS_H87,3125
#define NO_STDLIB_H88,3145
#define NO_FLOAT_H89,3165
#define TCL_NO_MATH91,3191
#define TCL_LIBRARY	92,3211
#define TCL_MEM_DEBUG93,3234
#   define _USING_PROTOTYPES_ 103,3513
#   define _ANSI_ARGS_(_ANSI_ARGS_104,3545
#   define CONST 105,3573
#       define VARARGS 107,3618
#       define VARARGS 109,3656
#   define _ANSI_ARGS_(_ANSI_ARGS_112,3711
#   define CONST113,3740
#   define EXTERN 117,3784
#   define EXTERN 119,3819
#       define VOID 130,4035
#       define VOID 132,4069
#define NULL 142,4233
    typedef void *ClientData;ClientData147,4306
    typedef int *ClientData;ClientData149,4345
#define _CLIENTDATA151,4399
#define TCL_DSTRING_STATIC_SIZE 160,4599
typedef struct Tcl_DString Tcl_DString161,4635
} Tcl_DString;Tcl_DString171,5076
#define Tcl_DStringLength(Tcl_DStringLength173,5092
#define Tcl_DStringValue(Tcl_DStringValue174,5143
typedef struct Tcl_Interp{Tcl_Interp184,5522
} Tcl_Interp;Tcl_Interp199,6262
typedef int *Tcl_Trace;Tcl_Trace201,6277
typedef struct Tcl_AsyncHandler_ *Tcl_AsyncHandler;Tcl_AsyncHandler202,6301
#define TCL_OK	223,7144
#define TCL_ERROR	224,7162
#define TCL_RETURN	225,7182
#define TCL_BREAK	226,7203
#define TCL_CONTINUE	227,7223
#define TCL_RESULT_SIZE 229,7247
typedef enum {TCL_INT, TCL_EITHER} Tcl_ValueType;Tcl_ValueType235,7352
typedef struct Tcl_Value Tcl_Value236,7402
} Tcl_Value;Tcl_Value240,7560
typedef int (Tcl_AsyncProc)Tcl_AsyncProc246,7617
typedef int (Tcl_AsyncProc) _ANSI_ARGS_(_ANSI_ARGS_246,7617
typedef void (Tcl_CmdDeleteProc)Tcl_CmdDeleteProc248,7714
typedef void (Tcl_CmdDeleteProc) _ANSI_ARGS_(_ANSI_ARGS_248,7714
typedef int (Tcl_CmdProc)Tcl_CmdProc249,7785
typedef int (Tcl_CmdProc) _ANSI_ARGS_(_ANSI_ARGS_249,7785
typedef void (Tcl_CmdTraceProc)Tcl_CmdTraceProc251,7894
typedef void (Tcl_CmdTraceProc) _ANSI_ARGS_(_ANSI_ARGS_251,7894
typedef void (Tcl_FreeProc)Tcl_FreeProc254,8081
typedef void (Tcl_FreeProc) _ANSI_ARGS_(_ANSI_ARGS_254,8081
typedef void (Tcl_InterpDeleteProc)Tcl_InterpDeleteProc255,8140
typedef void (Tcl_InterpDeleteProc) _ANSI_ARGS_(_ANSI_ARGS_255,8140
typedef int (Tcl_MathProc)Tcl_MathProc257,8235
typedef int (Tcl_MathProc) _ANSI_ARGS_(_ANSI_ARGS_257,8235
typedef char *(Tcl_VarTraceProc)Tcl_VarTraceProc259,8360
typedef char *(Tcl_VarTraceProc) _ANSI_ARGS_(_ANSI_ARGS_259,8360
typedef struct Tcl_CmdInfo Tcl_CmdInfo267,8577
} Tcl_CmdInfo;Tcl_CmdInfo274,8929
#define TCL_MAX_PREC 282,9149
#define TCL_DOUBLE_SPACE 283,9173
#define TCL_BRACKET_TERM	291,9375
#define TCL_DONT_USE_BRACES	299,9596
#define TCL_NO_EVAL	306,9720
#define TCL_VOLATILE	313,9851
#define TCL_STATIC	314,9894
#define TCL_DYNAMIC	315,9934
#define TCL_GLOBAL_ONLY	321,10041
#define TCL_APPEND_VALUE	322,10068
#define TCL_LIST_ELEMENT	323,10095
#define TCL_TRACE_READS	324,10122
#define TCL_TRACE_WRITES	325,10152
#define TCL_TRACE_UNSETS	326,10182
#define TCL_TRACE_DESTROYED	327,10212
#define TCL_INTERP_DESTROYED	328,10245
#define TCL_LEAVE_ERR_MSG	329,10280
#define TCL_LINK_INT	335,10352
#define TCL_LINK_BOOLEAN	336,10376
#define TCL_LINK_STRING	337,10403
#define TCL_LINK_READ_ONLY	338,10430
#define TCL_FILE_READABLE	344,10502
#define TCL_FILE_WRITABLE	345,10530
#  define ckalloc(ckalloc364,11185
#  define ckfree(ckfree365,11243
#  define ckrealloc(ckrealloc366,11300
#  define ckalloc(ckalloc370,11378
#  define ckfree(ckfree371,11409
#  define ckrealloc(ckrealloc372,11438
#  define Tcl_DumpActiveMemory(Tcl_DumpActiveMemory373,11476
#  define Tcl_ValidateAllMemory(Tcl_ValidateAllMemory374,11510
#define Tcl_FreeResult(Tcl_FreeResult382,11627
typedef struct Tcl_HashEntry Tcl_HashEntry408,12315
} Tcl_HashEntry;Tcl_HashEntry428,13223
#define TCL_SMALL_HASH_TABLE 436,13429
typedef struct Tcl_HashTable Tcl_HashTable437,13460
} Tcl_HashTable;Tcl_HashTable464,14619
typedef struct Tcl_HashSearch Tcl_HashSearch471,14740
} Tcl_HashSearch;Tcl_HashSearch477,15024
#define TCL_STRING_KEYS	484,15093
#define TCL_ONE_WORD_KEYS	485,15120
#define Tcl_GetHashValue(Tcl_GetHashValue491,15220
#define Tcl_SetHashValue(Tcl_SetHashValue492,15266
#define Tcl_GetHashKey(Tcl_GetHashKey493,15342
#define Tcl_FindHashEntry(Tcl_FindHashEntry502,15592
#define Tcl_CreateHashEntry(Tcl_CreateHashEntry504,15677
#define Tcl_Return 628,21255
