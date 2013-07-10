#define LIST_LEVELS         0
#define LIST_PERMISSIONS    1
#define LIST_CREATE_FOOD    2
#define LIST_LS_SPECIES     3
#define LIST_LS_REAGENT     4
#define LIST_FF_SPECIES     5
#define LIST_FF_REAGENT     6
#define LIST_RA_SPECIES     7
#define LIST_RA_REAGENT     8
#define LIST_FM_SPECIES     9
#define LIST_FM_REAGENT    10
#define LIST_CE_SPECIES    11
#define LIST_CE_REAGENT    12
#define LIST_CG_SPECIES    13
#define LIST_CG_REAGENT    14
#define MAX_LIST           15


extern int list_value [ MAX_LIST ][ 30 ];


void   save_lists   ( void );
void   load_lists   ( void );











