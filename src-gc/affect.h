#define APPLY_NONE                  0
#define APPLY_STR                   1
#define APPLY_DEX                   2
#define APPLY_INT                   3
#define APPLY_WIS                   4
#define APPLY_CON                   5
#define APPLY_MAGIC                 6
#define APPLY_FIRE                  7
#define APPLY_COLD                  8
#define APPLY_ELECTRIC              9
#define APPLY_MIND                 10
#define APPLY_AGE                  11 
#define APPLY_MANA                 12
#define APPLY_HIT                  13
#define APPLY_MOVE                 14
#define APPLY_AC                   17
#define APPLY_HITROLL              18
#define APPLY_DAMROLL              19
#define APPLY_MANA_REGEN           20
#define APPLY_HIT_REGEN            21
#define APPLY_MOVE_REGEN           22
#define APPLY_ACID                 23
#define APPLY_POISON               24
#define MAX_AFF_LOCATION           25


#define AFFECT_INTS                 3


class Affect_Data
{
 public:
  char_data*    leech;
  char_data*    victim;
  int           type;
  int           duration;
  int           level;
  int           location;
  int           modifier;
  int           leech_regen;
  int           leech_max;

  Affect_Data( );
  ~Affect_Data( );
};


extern const char* fake_mob [ MAX_FAKE_MOB ];


void   add_affect         ( char_data*, affect_data* );
void   add_affect         ( obj_data*, affect_data* );
int    affect_level       ( char_data*, int ); 
int    affect_duration    ( char_data*, int ); 
bool   has_affect         ( char_data*, int );
bool   is_affected        ( obj_data*, int );
void   strip_affect       ( char_data*, int );
bool   strip_affect       ( obj_data*, int );
void   remove_affect      ( char_data* );
void   remove_affect      ( char_data*, affect_data* );
void   remove_affect      ( obj_data*, affect_data*, bool = TRUE );
void   read_affects       ( FILE*, obj_clss_data* );
void   read_affects       ( FILE*, obj_data* );
void   read_affects       ( FILE*, char_data* );
void   write_affects      ( FILE*, obj_clss_data* );
void   write_affects      ( FILE*, obj_data* );
void   write_affects      ( FILE*, char_data* );

