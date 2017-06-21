
generic_objects.h,784
#define generic_objects_h77,2543
typedef int bool;bool79,2570
typedef struct Index *IndexPtr;IndexPtr114,4282
typedef int (*CMP)CMP117,4351
typedef int (*CMP) (void *key, void *elem)elem117,4351
typedef struct List *ListPtr;ListPtr250,9451
typedef struct ListEntryListEntry251,9481
} ListEntry, *ListEntryPtr;ListEntryPtr255,9564
#define List_pop	363,12818
#define List_get	364,12846
#define List_write	366,12875
#define List_append	367,12909
#define List_prepend	369,12945
#define List_push	370,12983
#define List_first	372,13019
#define List_last	373,13049
#define List_moveToFront(List_moveToFront375,13079
#define List_moveToEnd(List_moveToEnd381,13207
#define List_moveAfter(List_moveAfter387,13331
#define List_moveBefore(List_moveBefore393,13471
