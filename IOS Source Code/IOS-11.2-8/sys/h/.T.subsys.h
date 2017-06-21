
subsys.h,894
#define __SUBSYS_H__63,2450
#define SUBSYS_MAGIC_NUMBER 73,2769
#define SUBSYS_MAGIC_NUMBER_PT1 74,2841
#define SUBSYS_HEADER_VERSION 75,2908
#define SUBSYS_CLASS_MIN 77,2944
#define SUBSYS_CLASS_KERNEL 78,2983
#define SUBSYS_CLASS_LIBRARY 79,3022
#define SUBSYS_CLASS_DRIVER 80,3061
#define SUBSYS_CLASS_PROTOCOL 81,3100
#define SUBSYS_CLASS_MANAGEMENT 82,3139
#define SUBSYS_CLASS_REGISTRY 83,3178
#define SUBSYS_CLASS_SYSINIT 84,3217
#define SUBSYS_CLASS_MAX 85,3256
#define SUBSYS_STATUS_INACTIVE 87,3296
#define SUBSYS_STATUS_ACTIVE 88,3335
#define SUBSYS_PROPERTIES_MAX 90,3375
typedef void (*subsys_entry)subsys_entry92,3415
typedef void (*subsys_entry)(subsystype subsystype92,3415
typedef ushort subsys_class;subsys_class93,3459
typedef ushort subsys_type;subsys_type94,3488
struct subsystype_ subsystype_96,3517
#define SUBSYS_HEADER(SUBSYS_HEADER118,4022
