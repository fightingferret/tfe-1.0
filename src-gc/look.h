/*
 *   DESCRIPTION STRUCTURE
 */


class Extra_Data : public Visible_Data
{
 public:
  char*        keyword;        
  char*           text;

  Extra_Data( ) {
    record_new( sizeof( extra_data ), MEM_EXTRA );
    }

  ~Extra_Data( ) {
    record_delete( sizeof( extra_data ), MEM_EXTRA );
    free_string( keyword, MEM_EXTRA );
    free_string( text, MEM_EXTRA );
    }

  int Type ( ) { return EXTRA_DATA; }
  
  void         Look_At    ( char_data* );
  const char*  Keywords   ( char_data* );
};


bool  obj_descr     ( char_data*, thing_array&, char* );
bool  room_descr    ( char_data*, extra_data*, char* );
void  show_extras   ( char_data*, extra_array& );
void  edit_extra    ( extra_array&, wizard_data*, int, char*, char* );
void  read_extra    ( FILE*, extra_array& );
void  write_extras  ( FILE*, extra_array& );


/*
 *   LOOK ROUTINES
 */


void   glance        ( char_data*, char_data* );
void   look_in       ( char_data*, obj_data* );
void   look_at       ( char_data*, obj_data* );
void   show_room     ( char_data*, room_data*, bool, bool );


/*
 *   NOISE
 */


inline bool can_hear( char_data* ch )
{
  if( ch->position <= POS_SLEEPING )
    return FALSE;

  return TRUE;
}


/*
 *   WHO ROUTINES
 */


inline bool can_see_who( char_data* ch, char_data* victim )
{
  wizard_data* imm;

  if( ( imm = wizard( victim ) ) != NULL
    && is_set( victim->pcdata->pfile->flags, PLR_WIZINVIS )
    && get_trust( ch ) < imm->wizinvis )
    return FALSE;

  return TRUE;
}


inline const char* who_name( char_data* ch, char_data* victim )
{
  return can_see_who( ch, victim ) ? victim->Seen_Name( ch ) : "someone";
}





