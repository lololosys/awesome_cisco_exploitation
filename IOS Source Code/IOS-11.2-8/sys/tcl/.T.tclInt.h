
tclInt.h,2132
#define _TCLINT80,3058
typedef struct VarTrace VarTrace143,4414
} VarTrace;VarTrace153,4898
typedef struct ActiveVarTrace ActiveVarTrace163,5239
} ActiveVarTrace;ActiveVarTrace173,5669
typedef struct ArraySearch ArraySearch181,5850
} ArraySearch;ArraySearch198,6606
typedef struct Var Var207,6869
} Var;Var247,8706
#define VAR_ARRAY	272,9583
#define VAR_UPVAR	273,9604
#define VAR_UNDEFINED	274,9625
#define VAR_TRACE_ACTIVE	275,9650
typedef struct Arg Arg289,10036
} Arg;Arg300,10563
typedef struct Proc Proc308,10732
} Proc;Proc320,11274
typedef struct Trace Trace327,11433
} Trace;Trace333,11760
typedef struct DeleteCallback DeleteCallback340,11902
} DeleteCallback;DeleteCallback346,12179
typedef struct CallFrame CallFrame354,12384
} CallFrame;CallFrame375,13321
} HistoryEvent;HistoryEvent387,13676
typedef struct HistoryRev HistoryRev403,14198
} HistoryRev;HistoryRev413,14665
typedef struct OpenFile OpenFile428,15080
} OpenFile;OpenFile448,15949
#define MAX_MATH_ARGS 463,16303
typedef struct MathFunc MathFunc464,16327
} MathFunc;MathFunc471,16674
typedef struct Command Command483,17129
} Command;Command491,17487
#define CMD_SIZE(CMD_SIZE493,17499
typedef struct Interp Interp495,17575
#define NUM_REGEXPS NUM_REGEXPS601,21978
} Interp;Interp651,23965
#define DELETED	675,24901
#define ERR_IN_PROGRESS	676,24921
#define ERR_ALREADY_LOGGED	677,24948
#define ERROR_CODE_SET	678,24977
#define EXPR_INITIALIZED	679,25003
#define DEFAULT_PD_PREC 685,25111
#define DEFAULT_PD_FORMAT 686,25137
typedef struct ParseValue ParseValue704,25807
} ParseValue;ParseValue716,26315
#define CHAR_TYPE(CHAR_TYPE726,26577
#define TCL_NORMAL	744,27151
#define TCL_SPACE	745,27173
#define TCL_COMMAND_END	746,27194
#define TCL_QUOTE	747,27221
#define TCL_OPEN_BRACKET	748,27242
#define TCL_OPEN_BRACE	749,27269
#define TCL_CLOSE_BRACE	750,27295
#define TCL_BACKSLASH	751,27322
#define TCL_DOLLAR	752,27347
#define TCL_RECORD_BOUNDS	764,27728
#define MAX_NESTING_DEPTH	771,27873
#define UCHAR(UCHAR779,28076
#define TCL_ALIGN(TCL_ALIGN787,28336
