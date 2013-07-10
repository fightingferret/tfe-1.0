#define POS_DELETED                -3
#define POS_FOLLOWING              -2
#define POS_EXTRACTED              -1    
#define POS_DEAD                    0
#define POS_MORTAL                  1
#define POS_INCAP                   2
#define POS_STUNNED                 3
#define POS_SLEEPING                4
#define POS_MEDITATING              5
#define POS_RESTING                 6
#define POS_FIGHTING                7
#define POS_STANDING                8
#define POS_FLYING                  9
#define MAX_POSITION               10


/*
 *   MOUNT FUNCTIONS
 */


bool   is_mounted         ( char_data* );
void   dismount           ( char_data* );


