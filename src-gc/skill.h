/*
 *   PRACTICES
 */


int  total_pracs  ( char_data* );


inline int expected_pracs( char_data* ch )
{
  return 10+2*ch->shdata->level*(10+ch->Wisdom( )
    +ch->Intelligence( ))/3;
}


/*
 *  TRAINER HEADER
 */


class Trainer_Data
{
 public:
  trainer_data*   next;
  room_data*      room;
  char_data*       mob;
  int          trainer;
  int            skill  [ MAX_TRAIN ];

  Trainer_Data( ) {
    record_new( sizeof( trainer_data ), MEM_TRAINER );
    vzero( skill, MAX_TRAIN );
    mob  = NULL;
    next = NULL;
    return;
    }  

  ~Trainer_Data( ) {
    record_delete( sizeof( trainer_data ), MEM_TRAINER );
    return;
    }
};


void   load_trainers    ( void );
void   save_trainers    ( void );
void   set_trainer      ( mob_data*, room_data* );












