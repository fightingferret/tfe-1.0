#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


void   room_info          ( char_data*, room_data* );
void   show_secret        ( char_data* );
void   show_tracks        ( char_data* );
bool   show_characters    ( char_data* );
void   show               ( char_data*, thing_data* );

void   look_in            ( char_data*, char* );
void   look_in            ( char_data*, obj_data* );

void   look_at_character  ( char_data*, char_data* );
void   show_equipment     ( char_data*, char_data* );
void   show_inventory     ( char_data*, char_data* );
void   show_description   ( char_data*, char_data* );


/*
 *   CAN_SEE ROUTINES
 */


bool Char_Data :: Can_See( bool msg )
{
  if( position <= POS_SLEEPING || is_set( affected_by, AFF_BLIND ) ) {
    if( msg )
      send( this, "You can't see a thing!\n\r" );
    return FALSE;
    }

  return TRUE;
}


bool Room_Data :: Seen( char_data* ch )
{
  if( !ch->Can_See( ) )
    return FALSE;

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return TRUE;
}


bool Char_Data :: Seen( char_data* ch )
{
  wizard_data* imm;

  if( !In_Game( ) )
    return FALSE;

  if( this == ch || ch == NULL )
    return TRUE;

  if( ( imm = wizard( this ) ) != NULL
    && is_set( pcdata->pfile->flags, PLR_WIZINVIS )
    && get_trust( ch ) < imm->wizinvis )
    return FALSE;

  if( !in_character )
    return TRUE;

  if( !ch->Can_See( ) )
    return FALSE;
    
  /*
  if( in_room->is_dark( )
    && !is_set( affected_by, AFF_SANCTUARY )
    && ( !is_set( ch->affected_by, AFF_INFRARED ) || ( species != NULL
    && !is_set( &species->act_flags, ACT_WARM_BLOODED ) ) ) )
    return FALSE;
  */

  if( is_set( affected_by, AFF_INVISIBLE )
    && !ch->Sees_Invis( ) )
    return FALSE;

  /*
  if( ( is_set( affected_by, AFF_CAMOUFLAGE )
    || is_set( &status, STAT_CAMOUFLAGED ) )
    && !is_set( ch->affected_by, AFF_CAMOUFLAGE ) )
    return FALSE;
  */

  if( is_set( &status, STAT_HIDING )
    && !ch->Sees_Hidden( )
    && !includes( seen_by, ch ) )
    return FALSE;

  return TRUE;
}


bool Obj_Data :: Seen( char_data* ch )
{
  if( array == &ch->contents
    || array == &ch->wearing )
    return TRUE;

  if( !ch->Can_See( ) )
    return FALSE;

  /*
  if( in_room->is_dark( ) )
    return FALSE;
  */

  if( is_set( extra_flags, OFLAG_IS_INVIS ) && !ch->Sees_Invis( ) )
    return FALSE;

  return TRUE;
}


/*
 *   ATTRIBUTES
 */


bool char_data :: detects_evil( )
{
  return( is_set( affected_by, AFF_TRUE_SIGHT ) 
    || is_set( affected_by, AFF_DETECT_EVIL ) );
}


bool char_data :: detects_good( )
{
  return( is_set( affected_by, AFF_TRUE_SIGHT ) 
    || is_set( affected_by, AFF_DETECT_GOOD ) );
}


bool Char_Data :: Sees_Invis( )
{
  if( pcdata != NULL
    && is_set( pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return( is_set( affected_by, AFF_SEE_INVIS ) 
    || is_set( affected_by, AFF_TRUE_SIGHT ) );
}


bool Char_Data :: Sees_Hidden( )
{
  if( pcdata != NULL
    && is_set( pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  return( is_set( affected_by, AFF_DETECT_HIDDEN ) 
    || is_set( affected_by, AFF_SENSE_LIFE ) );
}


/* 
 *   SHOW OBJECT ROUTINES
 */


void page( char_data* ch, thing_array& array )
{
  thing_data*   thing;
  bool        nothing  = TRUE;

  select( array );
  rehash( ch, array );

  for( int i = 0; i < array; i++ ) {
    thing = array[i];
    if( thing->shown > 0 ) {
      nothing = FALSE;
      page( ch, "  %s\n\r", thing );
      }
    }

  if( nothing )
    page( ch, "  nothing\n\r" );
}


void send( char_data* ch, thing_array& array )
{
  thing_data*   thing;
  bool        nothing  = TRUE;

  select( array );
  rehash( ch, array );

  for( int i = 0; i < array; i++ ) {
    thing = array[i];
    if( thing->shown > 0 ) {
      nothing = FALSE;
      send( ch, "  %s\n\r", thing );
      }
    }

  if( nothing )
    send( ch, "  nothing\n\r" );
}


/* 
 *   TRACK AND SEARCH ROUTINES
 */


void show_secret( char_data* ch ) 
{
  exit_data*  exit;

  if( ch->pcdata == NULL
    || !is_set( ch->pcdata->pfile->flags, PLR_SEARCHING )  
    || ch->move == 0 )
    return; 

  ch->move--;

  for( int i = 0; i < ch->in_room->exits; i++ ) {
    exit = ch->in_room->exits[i];
    if(  is_set( &exit->exit_info, EX_SEARCHABLE )
      && is_set( &exit->exit_info, EX_CLOSED )
      && is_set( &exit->exit_info, EX_SECRET ) 
      && !includes( ch->seen_exits, exit )
      && ch->check_skill( SKILL_SEARCHING ) ) {
      send( ch, "\n\r%s>> You detect something unusual %s. <<%s\n\r",
        bold_v( ch ), dir_table[ exit->direction ].name, normal( ch ) );
      ch->seen_exits += exit;
      }
    }
}


/*
 *   LOOK AT
 */


void Char_Data :: Look_At( char_data* ch )
{
  known_by += ch;

  if( ch != this ) {
    if( ch->Seen( this ) )
      send( this, "%s looks at you.\n\r", ch );
    send_seen( ch, "%s looks at %s.\n\r", ch, this );
    }
  else {
    send_seen( ch, "%s looks at %sself.\n\r",
      ch, ch->Him_Her( ) );
    }
  
  show_description( ch, this );
  page( ch, scroll_line[0] );
  glance( ch, this );
  page( ch, "\n\r" );
  show_equipment( ch, this );
}


void Exit_Data :: Look_At( char_data* ch )
{
  if( !is_set( &exit_info, EX_ISDOOR ) ) {
    send( ch, "To the %s is %s.\n\r",
       dir_table[ direction ].name,
       to_room->name );
    }
  else {   
    send( ch, "The %s is %s.\n\r", name,
      is_set( &exit_info, EX_CLOSED ) ? "closed" : "open" );
    }
}  


void Extra_Data :: Look_At( char_data* ch )
{
  char tmp [ MAX_STRING_LENGTH ];

  convert_to_ansi( ch, text, tmp );
  send( ch, tmp );
}

   


/* 
 *   SHOW CHARACTER FUNCTIONS
 */


void do_peek( char_data* ch, char* argument )
{
  char_data *victim;

  if( !ch->Can_See( TRUE ) )
    return;

  if( ( victim = one_character( ch, argument, "peek at",
    ch->array ) ) == NULL )
    return;

  show_equipment( ch, victim );
  page( ch, "\n\r" );
  show_inventory( ch, victim );

  ch->improve_skill( SKILL_PEEK );

  return;
}


void do_qlook( char_data *ch, char *argument )
{
  char_data* victim;

  if( !ch->Can_See( TRUE ) )
    return;

  if( *argument == '\0' ) {
    send( ch, "Look quickly at whom?\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "look quickly at",
    ch->array ) ) == NULL )
    return;

  show_description( ch, victim );

  return;
}


void show_inventory( char_data* ch, char_data* victim )
{
  if( ch == victim ) 
    page( ch, "You are carrying:\n\r" );
  else 
    page( ch, "%s is carrying:\n\r", victim );

  page( ch, victim->contents );

  return;
}


void show_description( char_data* ch, char_data* victim )
{
  char tmp  [ 3*MAX_STRING_LENGTH ];
 
  if( *victim->descr->complete != '\0' ) {
    convert_to_ansi( ch, victim->descr->complete, tmp );
    page( ch, tmp );
    }
  else {
    fpage( ch, "You see nothing special about %s.  In fact, you doubt\
 %s has any special distinguishing characteristics at all.",
      victim->Him_Her( ), victim->He_She( ) );
    }

  return;
}


/*
 *   EQUIPMENT 
 */


void do_equipment( char_data* ch, char* )
{
  show_equipment( ch, ch );
}


void show_equipment( char_data* ch, char_data* victim )
{
  char*       format  = "%-22s %-42s %s\n\r";
  char*          tmp  = static_string( );
  bool         found  = FALSE;
  obj_data**    list  = (obj_data**) victim->wearing.list;
  int           i, j;

  for( i = 0; i < victim->wearing; i = j ) { 

    if( victim != ch )  
      for( j = i+1; j < victim->wearing
        && list[i]->position == list[j]->position; j++ )
        if( is_set( list[j]->pIndexData->extra_flags, OFLAG_COVER ) )
          i = j;

    for( j = i; j < victim->wearing
      && list[i]->position == list[j]->position; j++ ) {
      if( ch == victim || list[j]->Seen( ch ) ) {
        if( !found ) {
          page_centered( ch, "+++ Equipment +++" );
          sprintf( tmp, format, "Body Location", "Item", "Condition" );
          page_underlined( ch, tmp );
          found = TRUE;
          }
        page( ch, format,
          j == i ? where_name[ list[i]->position ] : "", 
          list[j]->Name( ch, 1, TRUE ),
          list[j]->condition_name( ch, TRUE ) );
        }
      }
    }

  if( !found ) {
    if( ch == victim )
      page( ch, "You have nothing equipped.\n\r" );
    else
      page( ch, "%s has nothing equipped.\n\r", victim );
    }
  else {
    if( ch == victim ) 
      page( ch, "\n\rWeight: %.2f lbs\n\r", 
        (float) ch->wearing.weight/100 );
      }
}

/*
 *   LOOK IN OBJECT
 */


thing_data* cant( thing_data* thing, char_data*, thing_data* )
{
  return object( thing );
}


thing_data* notcontainer( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  switch( obj->pIndexData->item_type ) {
    case ITEM_KEYRING   :
    case ITEM_CONTAINER :
    case ITEM_CORPSE    :
      return obj;
    }

  return NULL;
}


thing_data* closed( thing_data* thing, char_data*, thing_data* )
{
  obj_data* obj = (obj_data*) thing;

  if( obj->pIndexData->item_type == ITEM_CONTAINER 
    && is_set( &obj->value[1], CONT_CLOSED ) )
    return NULL;

  return obj;
}
 
    
thing_data* empty( thing_data* thing, char_data*, thing_data* )
{
  return( is_empty( thing->contents ) ? NULL : thing );
}


thing_data* lookin( thing_data* thing, char_data*, thing_data* )
{
  return thing;
}


void look_in( char_data* ch, char* argument )
{
  thing_array* array;

  if( *argument == '\0' ) {
    send( ch, "Look in what?\n\r" );
    return;
    }

  if( ( array = several_things( ch, argument,
    "look in", ch->array, &ch->contents,
    &ch->wearing ) ) == NULL ) 
    return;

  thing_array   subset  [ 5 ];
  thing_func*     func  [ 5 ]  = { cant, notcontainer, closed,
                                   empty, lookin };

  sort_objects( ch, *array, NULL, 5, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't look in" );
  page_priv( ch, &subset[1], NULL, NULL,
    "isn't a container", "aren't containers" );
  page_priv( ch, &subset[2], NULL, NULL, "is closed", "are closed" );
  page_priv( ch, &subset[3], NULL, NULL, "is empty", "are empty" );

  for( int i = 0; i < subset[4]; i++ ) 
    look_in( ch, (obj_data*) subset[4].list[i] );

  delete array;
}


void look_in( char_data* ch, obj_data* obj )
{
  switch( obj->pIndexData->item_type ) {
    default:
      page( ch, "%s is not a container.\n\r", obj );
      return;

    case ITEM_SPELLBOOK :
      page( ch, "The spellbook is blank.\n\r" );
      return;

    case ITEM_DRINK_CON:
      page( ch, "It's %sfull of %s.\n\r",
        obj->value[1] < obj->pIndexData->value[0] / 2 ? "less than half "
        : ( obj->value[1] < 3* obj->pIndexData->value[0] / 4
        ? "more than half " : "" ),
        obj->value[2] < 0 || obj->value[2] >= table_max[TABLE_LIQUID]
        ? "[BUG]" : liquid_table[obj->value[2]].color );
      return;

    case ITEM_KEYRING :
    case ITEM_CONTAINER:
    case ITEM_CORPSE :
      break; 
    }

  page( ch, "%s contains:\n\r", obj );
  page( ch, obj->contents );
}


/*
 *   MAIN LOOK ROUTINE
 */


void do_look( char_data* ch, char* argument )
{
  visible_data* visible;

  if( ch->link == NULL || !ch->Can_See( TRUE ) )
    return;

  if( *argument == '\0' ) {
    show_room( ch, ch->in_room, FALSE, FALSE );
    return;
    }

  if(  !strncasecmp( argument, "in ", 3 ) 
    || !strncasecmp( argument, "i ", 2 ) ) {
    argument += ( argument[1] == 'n' ? 3 : 2 );
    look_in( ch, argument );
    return;
    }

  if( !strncasecmp( argument, "at ", 3 ) )
    argument += 3;

  if( ( visible = one_visible( ch, argument, "look at",
    (visible_array*) ch->array,
    (visible_array*) &ch->contents,
    (visible_array*) &ch->wearing,  
    (visible_array*) &ch->in_room->extra_descr,
    (visible_array*) &ch->in_room->exits ) ) == NULL )
    return;

  visible->Look_At( ch );
}


void show_room( char_data* ch, room_data* room, bool brief, bool scan )
{
  char           tmp  [ 3*MAX_STRING_LENGTH ];
  thing_data*  thing;
  obj_data*      obj;

  if( ch->pcdata == NULL )
    return;

  room_info( ch, room );
   
  if( !brief || !is_set( ch->pcdata->pfile->flags, PLR_BRIEF ) ) {
    if( room->Seen( ch ) ) {
      convert_to_ansi( ch, room->description, tmp );
      send( ch, tmp );
      }
    else {
      send( ch,
        "The area is very dark and you can make out no details.\n\r" );
      }
    if( is_builder( ch ) && *room->comments != '\0' ) {
      send( ch, "%37s-----\n\r", "" );
      send( ch, room->comments ); 
      }
    }

  show_secret( ch );
  show_tracks( ch );

  /* SHOW CONTENTS */

  select( room->contents, ch );

  for( int i = 0; i < room->contents; i++ ) 
    if( ( obj = object( room->contents[i] ) ) != NULL  
      && is_set( obj->extra_flags, OFLAG_NOSHOW ) )
      room->contents[i]->selected = 0; 

  rehash( ch, room->contents );

  bool found = FALSE;

  for( int i = 0; i < room->contents; i++ ) {
    thing = room->contents[i];
    if( thing->shown > 0 && thing != ch ) {
      if( !found ) {
        found = TRUE;
        send( ch, "\n\r" );
        }
      show( ch, thing );
      }
    }

  /* SCAN */

  int level = level_setting( &ch->pcdata->pfile->settings,
    SET_AUTOSCAN );

  if( scan && level != 0
    && ( level == 3 || !is_set( &room->room_flags, RFLAG_NO_AUTOSCAN ) ) )
    do_scan( ch, "shrt" );       
}


void show( char_data* ch, thing_data* thing )
{
  send( ch, "%s\n\r", thing->Show( ch, thing->shown ) );
}


/*
 *   ROOM INFO BOX
 */


char* room_flags( room_data* room )
{
  char* tmp = static_string( );

  if( is_set( &room->room_flags, RFLAG_SAFE ) )
    sprintf( tmp, "safe" );
  else
    *tmp = '\0';

  if( is_set( &room->room_flags, RFLAG_NO_MOB ) ) 
    sprintf( tmp+strlen( tmp ), "%s%s",
      *tmp == '\0' ? "" : ", ", "no.mob" );

  if( is_set( &room->room_flags, RFLAG_INDOORS ) ) 
    sprintf( tmp+strlen( tmp ), "%s%s",
      *tmp == '\0' ? "" : ", ", "inside" );

  if( *tmp == '\0' )
    return "--";

  return tmp;
}


void room_info( char_data* ch, room_data* room )
{
  char*    tmp  = static_string( );
  char*   name;
  bool can_see  = room->Seen( ch );
  int     term  = ch->pcdata->terminal;
  int   detail;
  int        i;

  name   = can_see ? ch->in_room->name : "DARKNESS";
  detail = level_setting( &ch->pcdata->pfile->settings, SET_ROOM_INFO );

  if( detail < 2 ) {
    if( is_apprentice( ch ) ) {
      send_color( ch, COLOR_ROOM_NAME, "#%d : %s\n\r",
        room->vnum, name );
      }
    else {
      send_color( ch, COLOR_ROOM_NAME, "%s\n\r", name );
      }
    if( detail == 1 && can_see )
      autoexit( ch );
    send( ch, "\n\r" );
    return;
    }

  if( term != TERM_DUMB ) {
    sprintf( tmp, "%%%ds%s%%s%s\n\r",
      40-strlen( name )/2,
      term_table[term].codes( ch->pcdata->color[ COLOR_ROOM_NAME ] ),
      normal( ch ) );
    send( ch, tmp, "", name );
    }
  else {
    send_centered( ch, name );
    }

  send( ch, scroll_line[2] );

  send( ch, "|   Lighting: %-15s Time: %-16s  Terrain: %-13s |\n\r",
    light_name( room->Light( ) ),
    is_set( &room->room_flags, RFLAG_INDOORS ) ? "???" : sky_state( ),
    terrain[ room->sector_type ].name );

  i = exits_prompt( tmp, ch );
  add_spaces( tmp, 12-i );

  send( ch, "|      Exits: %s Weather: %-15s Room Size: %-13s |\n\r",
    tmp, is_set( &room->room_flags, RFLAG_INDOORS ) ? "???" : "Clear",
    size_name[room->size] );

  if( is_apprentice( ch ) )
    send( ch, "|       Vnum: %-14d Flags: %-40s |\n\r",
      room->vnum, room_flags( room ) );
  
  send( ch, scroll_line[2] );
}











