#include "define.h"
#include "struct.h"


#define RECOG_INTRODUCE       16
#define RECOG_RECOGNIZE       17
#define RECOG_BEFRIEND        18
#define RECOG_FILTER          19
#define RECOG_CONSENT         20


bool in_character = TRUE;


/*
 *   CLASS KNOWN_DATA
 */


Recognize_Data :: Recognize_Data( int i )
{
  record_new( sizeof( recognize_data ), MEM_RECOGNIZE );
  record_new( i*sizeof( int ), -MEM_RECOGNIZE );

  size = i;
  list = new int[i];
};


Recognize_Data :: ~Recognize_Data( )
{
  record_delete( sizeof( recognize_data ), MEM_RECOGNIZE );
  record_delete( size*sizeof( int ), -MEM_RECOGNIZE );

  delete [] list;
};


/*
 *   LOCAL FUNCTIONS
 */


bool  display               ( char_data*, int, const char* );
bool  is_recognize_set      ( recognize_data*, int, int );
bool  remove_recognize_bit  ( recognize_data*&, int, int );
void  set_recognize_bit     ( char_data*, int, int );

void  sort                  ( recognize_data* );
void  insert                ( recognize_data*&, int, int );


/*
 *   SEARCH ROUTINES
 */


void sort( recognize_data* recognize )
{
  int*   list;
  int    size;
  int       i;
  bool  found;

  if( recognize == NULL )
    return;

  list = recognize->list;
  size = recognize->size;

  found = TRUE;
  for( ; found; ) { 
    found = FALSE;
    for( i = 0; i < size-1; i++ ) {
      if( ( list[i] & 0xffff ) > ( list[i+1] & 0xffff ) ) {
        swap( list[i], list[i+1] );
        found = TRUE;
        }
      }
    }
}


void insert( recognize_data*& recognize, int value, int pos )
{
  if( recognize == NULL ) {
    recognize = new recognize_data( 1 );
    recognize->list[0] = value;
    return;
    }

  insert( recognize->list, recognize->size, value, pos );
  record_new( sizeof( int ), -MEM_RECOGNIZE );
}


void remove( recognize_data*& recognize, int pos )
{
  if( pos < 0 )
    return;

  record_delete( sizeof( int ), -MEM_RECOGNIZE );

  if( recognize->size == 1 ) {
    delete recognize;   
    recognize = NULL;
    return;
    }

  remove( recognize->list, recognize->size, pos );
}  


int search( recognize_data* recognize, int ident )
{
  int element;
  int min; 
  int max;
  int value;

  if( recognize == NULL )
    return -1;

  min = 0;
  max = recognize->size-1;

  for( ; ; ) {
    if( max < min )
      return -min-1;

    element  = (max+min)/2;
    value    = ( recognize->list[element] & 0xffff );

    if( ident == value )
      break;

    if( ident > value )
      min = element+1;
    else 
      max = element-1;
    }

  return element;
}

  
/*
 *   ALTERING KNOWLEDGE ROUTINES
 */


bool is_recognize_set( recognize_data* recognize, int ident, int bit )
{
  int pos;

  pos = search( recognize, ident );

  return( pos >= 0 && IS_SET( recognize->list[pos], bit ) );
}


bool remove_recognize_bit( recognize_data*& recognize, int ident, int bit )
{
  int pos;

  pos = search( recognize, ident );
 
  if( pos >= 0 && is_set( &recognize->list[pos], bit ) ) {
    remove_bit( &recognize->list[pos], bit );
    if( ( recognize->list[pos] & 0xffff0000 ) == 0 )
      remove( recognize, pos );
    return TRUE;
    }

  return FALSE;
} 


void set_recognize_bit( char_data* ch, int ident, int bit )
{
  recognize_data*  recognize  = ch->pcdata->recognize;
  int                    pos; 
    
  pos = search( recognize, ident );

  if( pos >= 0 ) {
    set_bit( &recognize->list[pos], bit );
    return;
    }

  set_bit( &ident, bit );
  insert( ch->pcdata->recognize, ident, -pos-1 );

  return;
} 


/*
 *   LOGIN ROUTINE
 */


void reconcile_recognize( char_data* ch )
{
  recognize_data*    recog_ch;
  recognize_data*  recog_vict;
  player_data*         victim;
  int                  pos_ch;
  int                pos_vict;
  int                 flag_ch;
  int               flag_vict;
  int                       i;

  if( ( recog_ch = ch->pcdata->recognize ) != NULL ) {
    for( i = recog_ch->size-1; i >= 0; i-- ) 
      if( get_pfile( recog_ch->list[i] & 0xffff ) == NULL ) 
        remove( recog_ch, i );
    sort( recog_ch );
    }

  for( i = 0; i < player_list; i++ ) {
    victim = player_list[i];

    if( !victim->Is_Valid( ) || victim == ch )
      continue;

    recog_vict  = victim->pcdata->recognize;
    pos_ch      = search( recog_ch, victim->pcdata->pfile->ident );
    pos_vict    = search( recog_vict, ch->pcdata->pfile->ident );

    flag_ch     = ( pos_ch >= 0 ? recog_ch->list[pos_ch] : 0 );
    flag_vict   = ( pos_vict >= 0 ? recog_vict->list[pos_vict] : 0 );

    if( ( is_set( &flag_ch, RECOG_RECOGNIZE )
      != is_set( &flag_vict, RECOG_INTRODUCE ) )
      || ( is_set( &flag_vict, RECOG_RECOGNIZE )
      != is_set( &flag_ch, RECOG_INTRODUCE ) ) ) {
      remove( recog_ch, pos_ch );
      remove( victim->pcdata->recognize, pos_vict );
      }
    }

  ch->pcdata->recognize = recog_ch;
}


/*
 *   DISPLAY LIST WITH BIT SET
 */


bool display( char_data* ch, int bit, const char* title )
{
  recognize_data*  recognize  = ch->pcdata->recognize;
  pfile_data*          pfile;
  int                   i, j;
  bool                 found  = FALSE;

  if( recognize == NULL )
    return FALSE;

  for( i = j = 0; i < recognize->size; i++ ) {
    if( ( pfile = get_pfile( recognize->list[i] & 0xffff ) ) == NULL
      || !is_set( &recognize->list[i], bit ) )
      continue;
    if( !found ) { 
      page( ch, "%s:\r\n", title );
      found = TRUE;
      }
    page( ch, "%17s%s", pfile->name, ++j%4 == 0 ? "\r\n" : "" );
    }  

  if( j%4 != 0 )
    page( ch, "\r\n" );
  
  return found;
}


pfile_data* remove_name( recognize_data*& recognize, char* name, int bit )
{
  pfile_data* pfile;

  if( ( pfile = find_pfile( name ) ) != NULL
    && remove_recognize_bit( recognize, pfile->ident, bit ) )
    return pfile;

  return NULL;
}


/*
 *   INTRODUCTION FUNCTIONS
 */


bool char_data :: Recognizes( char_data *victim )
{
  if( pcdata == NULL )
    return FALSE;

  if( victim->species != NULL ) {
    if( victim->descr == victim->species->descr || victim->leader != this
      || victim->descr->singular[0] == '\0' )
      return FALSE;
    return TRUE;
    }

  if( victim == this || shdata->level >= LEVEL_APPRENTICE
    || victim->shdata->level >= LEVEL_APPRENTICE ) 
    return TRUE;

  return is_recognize_set( pcdata->recognize,
    victim->pcdata->pfile->ident, RECOG_RECOGNIZE );
}


void do_introduce( char_data *ch, char *argument )
{
  char_data*  victim;

  if( is_confused_pet( ch ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "Who do you want to introduce yourself to?\r\n" );
    return;
    }

  if( ( victim = one_character( ch, argument, "introduce",
    ch->array ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "You unfortunately already recognize yourself.\r\n" );
    return;
    }

  if( victim->fighting != NULL ) {
    send( ch,
      "They are too busy fighting for you to introduce yourself.\r\n" );
    return;
    }

  if( victim->position < POS_RESTING ) {
    send( ch, "They aren't in a good state for introductions.\r\n" );
    return;
    }

  if( victim->species != NULL ) {
    send( ch, "Introducing yourself to npcs will do nothing useful.\r\n" );
    return;
    }

  if( !associate( ch, victim ) ) 
    return;

  if( victim->Recognizes( ch ) ) {
    send( ch, "%s already recognizes you.\r\n", victim );
    return;
    }

  if( !ch->Seen( victim ) ) {
    send( ch, "%s cannot see you.\r\n", victim );
    return;
    }

  if( !can_talk( ch, "introduce yourself" ) ) 
    return;

  leave_shadows( ch );
  leave_camouflage( ch );

  if( skill_language( victim, ch->pcdata->speaking ) == 0 ) {
    fsend( ch, "You attempt to introduce yourself to %s, but %s\
 does not seem to understand a word you are saying.\r\n",
      victim, victim->He_She( ) );
    return;
    }

  fsend( ch, "You introduce yourself to %s.\r\n", victim );
  fsend( victim, "%s greets you and introduces %sself as '%s'.\r\n",
    ch, ch->Him_Her( ), ch->descr->name );
  fsend_seen( ch, "%s introduces %sself to %s.\r\n",
    ch, ch->Him_Her( ), victim );

  set_recognize_bit( victim, ch->pcdata->pfile->ident, RECOG_RECOGNIZE ); 
  set_recognize_bit( ch, victim->pcdata->pfile->ident, RECOG_INTRODUCE );
}


/*
 *   BEFRIEND FUNCTIONS
 */


bool Char_Data :: Befriended( pfile_data* pfile )
{
  if( pcdata == NULL )
    return FALSE; 

  return is_recognize_set( pcdata->recognize,
    pfile->ident, RECOG_BEFRIEND );
}


bool Char_Data :: Befriended( char_data* ch )
{
  if( pcdata == NULL || ch->pcdata == NULL )
    return FALSE;
 
  return is_recognize_set( pcdata->recognize,
    ch->pcdata->pfile->ident, RECOG_BEFRIEND );
}


void do_befriend( char_data* ch, char* argument )
{
  char                   tmp  [ MAX_INPUT_LENGTH ];
  char_data*          victim;
  pfile_data*          pfile;
 
  if( is_confused_pet( ch ) ) 
    return;

  if( argument[0] == '\0' ) {
    if( !display( ch, RECOG_BEFRIEND, "Befriend List" ) )
      send( "You have befriended no one.\r\n", ch );
    return;
    }

  if( ( pfile = remove_name( ch->pcdata->recognize, argument,
    RECOG_BEFRIEND ) ) != NULL ) {
    sprintf( tmp, "You no longer consider %s a friend.\r\n", pfile->name );
    send( tmp, ch );
    return; 
    }
        
  if( ( victim = one_character( ch, argument, "befriend",
    ch->array ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "Befriending yourself is a waste of time.\r\n" );
    return;
    }

  if( IS_NPC( victim ) ) {
    send( ch, "Befriending npcs will do nothing useful.\r\n" );
    return;
    }

  if( !ch->Recognizes( victim ) ) {
    fsend( ch, "You cannot befriend %s without first knowing %s name.",
      victim, victim->His_Her( ) );
    return;
    }

  if( !victim->Recognizes( ch ) ) {
    fsend( ch, 
      "You cannot befriend %s until you have introduced yourself to %s.",
      victim, victim->Him_Her( ) );
    return;
    }

  if( ch->Befriended( victim ) ) {
    send( ch, "You already think %s is your friend.\r\n", victim );
    return;
    }

  send( ch, "You now consider %s a friend.\r\n", victim );

  set_recognize_bit( ch, victim->pcdata->pfile->ident, RECOG_BEFRIEND );

  return;
}


/*
 *   IGNORE ROUTINES
 */


bool Char_Data :: Ignoring( char_data* victim )
{
  if( pcdata == NULL || victim->pcdata == NULL 
    || victim->shdata->level >= LEVEL_APPRENTICE )
    return FALSE;

  if( !is_set( pcdata->pfile->flags, PLR_APPROVED ) 
    && has_permission( victim, PERM_APPROVE ) )
    return FALSE;

  int level = level_setting( &pcdata->pfile->settings, SET_IGNORE );

  switch( level ) {
    case 0 : return FALSE;
    case 1 : return !victim->Recognizes( this );
    case 2 : return !Befriended( victim );
    }

  return TRUE;
}


bool Char_Data :: Filtering( char_data *victim )
{
  if( this == NULL || pcdata == NULL || victim->pcdata == NULL )
    return FALSE;

  return is_recognize_set( pcdata->recognize,
    victim->pcdata->pfile->ident, RECOG_FILTER );
}


void do_filter( char_data* ch, char* argument )
{
  char_data*      victim;
  pfile_data*      pfile;

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( !display( ch, RECOG_FILTER, "Filter List" ) ) 
      send( ch, "You are filtering noone.\r\n" );
    return;
    }

  if( ( pfile = remove_name( ch->pcdata->recognize,
    argument, RECOG_FILTER ) ) != NULL ) {
    send( ch, "You will no longer filter %s.\r\n", pfile->name );
    return;
    }

  in_character = FALSE;

  if( ( victim = one_player( ch, argument, "filter",
    (thing_array*) &player_list ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( ch, 
      "Ignoring yourself might be wise, but it is also pointless.\r\n" );
    }
  else if( victim->shdata->level >= LEVEL_APPRENTICE ) {
    send( ch, "You may not filter immortals.\r\n" );
    }
  else if( ch->Filtering( victim ) ) {
    send( ch, "You are already filtering them.\r\n" );
    }
  else {
    set_recognize_bit( ch, victim->pcdata->pfile->ident, RECOG_FILTER );
    send( ch, "You are now filtering %s.\r\n", victim );
    }
}


/*
 *    ROUTINES
 */


bool consenting( char_data* victim, char_data* ch, const char* word )
{
  if( ch == victim || ch->shdata->level >= LEVEL_APPRENTICE )
    return TRUE;

  if( ch->pcdata != NULL && victim->pcdata != NULL
    && is_recognize_set( victim->pcdata->recognize,
    ch->pcdata->pfile->ident, RECOG_CONSENT ) )
    return TRUE;

  if( word != empty_string ) {
    fsend( ch, "Since %s does not automatically consent to your actions\
 the %s fails.", victim, word );
    send( victim, "You automatically disrupt %s.\r\n", word );
    } 

  return FALSE;
}


void do_consent( char_data* ch, char* argument )
{
  char_data*          victim;
  pfile_data*          pfile;

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( !display( ch, RECOG_CONSENT, "Consent List" ) ) 
      send( ch, "You have consented noone.\r\n" );
    return;
    }

  if( ( pfile = remove_name( ch->pcdata->recognize,
    argument, RECOG_CONSENT ) ) != NULL ) {
    send( ch, "You no longer automatically consent %s.\r\n", pfile->name );
    return;
    }

  if( ( victim = one_player( ch, argument, "consent",
    ch->array, (thing_array*) &player_list ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "That you agree with your own actions is assumed.\r\n" );
    return;
    }

  if( !ch->Recognizes( victim ) ) {
    send( ch, "You may only consent someone whom you recognize.\r\n" );
    return;
    } 

  if( victim->pcdata == NULL ) {
    send( ch, "Consent only matters with players.\r\n" );
    return;
    }

  if( consenting( ch, victim ) ) {
    send( ch, "%s is already listed for automatic consent.\r\n", victim );
    return;
    }

  set_recognize_bit( ch, victim->pcdata->pfile->ident, RECOG_CONSENT );

  send( ch, "You now automatically consent to the actions of %s.\r\n",
    victim );
}










