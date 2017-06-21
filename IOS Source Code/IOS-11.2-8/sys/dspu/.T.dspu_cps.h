
dspu_cps.h,1399
#define __CPS_H31,1002
#define MAX_SIBLINGS 35,1040
#define BAD_SIBLING_INDEX 36,1064
typedef enum SiblingType SiblingType40,1130
                   } SiblingType;SiblingType43,1314
typedef Pu* Parent_t;Parent_t51,1574
typedef void* Child_t;Child_t52,1596
typedef LinkStn* Parent_t;Parent_t55,1645
typedef Lu* Child_t;Child_t56,1672
typedef Dspu* Parent_t;Parent_t59,1719
typedef Pu*  Child_t;Child_t60,1776
typedef Adaptor* Parent_t;Parent_t63,1825
typedef LinkStn*  Child_t;Child_t64,1852
typedef Dspu* Parent_t;Parent_t67,1910
typedef Sap*  Child_t;Child_t68,1934
typedef void* Parent_t;Parent_t70,1963
typedef void* Child_t;Child_t71,1987
        } SiblingIndex;SiblingIndex81,2200
        } Sibling;Sibling87,2430
        } SingleChild;SingleChild92,2638
        } DspuChild;DspuChild97,2904
typedef void (*ProcChild)ProcChild99,2972
typedef void (*ProcChild)(Child_t)Child_t99,2972
typedef void (*ProcChild1)ProcChild1100,3050
typedef void (*ProcChild1)(Child_t, hword)hword100,3050
typedef void (*ProcChild2)ProcChild2101,3130
typedef void (*ProcChild2)(Child_t,Child_t101,3130
typedef boolean (*ProcQChild)ProcQChild102,3210
typedef boolean (*ProcQChild)(Child_t)Child_t102,3210
typedef boolean (*ProcQChild1)ProcQChild1103,3250
typedef boolean (*ProcQChild1)(Child_t,Child_t103,3250
#define GetSingleChild(GetSingleChild166,6335
