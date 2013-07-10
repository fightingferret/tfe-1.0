#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool get_trigger    ( char_data*, obj_data* );
bool put_trigger    ( char_data*, obj_data*, obj_data* );
  

/*
 *
 */


obj_data* find_type( thing_array& array, int type )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ )
    if( ( obj = object( array[i] ) ) != NULL
      && obj->pIndexData->item_type == type ) {
      obj->selected = 1;
      obj->shown    = 1;
      return obj;
      }

  return NULL;
}


obj_data* find_vnum( thing_array& array, int vnum )
{
  obj_data* obj;

  for( int i = 0; i < array; i++ )
    if( ( obj = object( array[i] ) ) != NULL
      && obj->pIndexData->vnum == vnum )
      return obj;

  return NULL;
}


/*
 *   OBJECT MANLIPULATION ROUTINES
 */


void do_inventory( char_data* ch, char* )
{
  char        long_buf  [ MAX_STRING_LENGTH ];
  char             buf  [ MAX_STRING_LENGTH ];
  char          string  [ MAX_STRING_LENGTH ];
  thing_data*    thing;
  obj_data*        obj;
  bool         nothing  = TRUE;
  const char*     name;
  int             wght  = 0;
  int           i, col;

  if( is_confused_pet( ch ) )
    return;

  if( ch->species != NULL
    && !is_set( &ch->species->act_flags, ACT_CAN_CARRY )
    && get_trust( ch ) < LEVEL_APPRENTICE ) {
    send( ch, "You are unable to carry items.\n\r" );
    return;
    }

  *long_buf = '\0';

  for( i = 0; i < ch->contents; i++ ) {
    thing = ch->contents[i];
    if( ( obj = object( thing ) ) != NULL
      && obj->pIndexData->item_type == ITEM_MONEY ) { 
      obj->selected = 0;
      wght += obj->Weight( );
      }
    else 
      thing->selected = thing->number;
    }

  rehash_weight( ch, ch->contents );

  page( ch, "Coins: %d = [%s ]    Weight: %.2f lbs\n\r\n\r",
    get_money( ch ), coin_phrase( ch ), float( wght/100. ) );

  strcpy( string, "Item                          Num  Wgt" );
  page( ch, "%s%s   %s\n\r", bold_v( ch ), string, string );
  strcpy( string, "----                          ---  ---");
  page( ch, "%s   %s%s\n\r", string, string, normal( ch ) );

  for( col = i = 0; i < ch->contents; i++ ) {
    thing = ch->contents[i];
 
    if( thing->shown == 0 )
      continue;

    name    = thing->Name( ch );
    nothing = FALSE;

    if( strlen( name ) < 30 ) {
      sprintf( buf, "%-30s%3s%5s%s", name,
        int3( thing->shown ), float3( thing->temp/100. ),
        ++col%2 == 0 ? "\n\r" : "   " );
      page( ch, buf );
      }
    else {
      sprintf( long_buf+strlen( long_buf ), "%-71s%3s%5s\n\r",
        name, int3( thing->shown ), float3( thing->temp/100. ) );
      }
    }

  if( col%2 == 1 )
    page( ch, "\n\r" );

  if( *long_buf != '\0' ) {
    if( col != 0 )
      page( ch, "\n\r" );
    page( ch, long_buf );
    }

  if( nothing ) 
    page( ch, "< empty >\n\r" ); 

  i = ch->get_burden( );

  page( ch, "\n\r  Carried: %6.2f lbs   (%s%s%s)\n\r",
    float( ch->contents.weight/100. ),
    color_scale( ch, i ), burden_name[i], normal( ch ) );
  page( ch, "     Worn: %6.2f lbs\n\r",
    float( ch->wearing.weight/100. ) );
  page( ch, "   Number: %6d       ( Max = %d )\n\r",
    ch->contents.number, ch->can_carry_n( ) );
}


/*
 *   JUNK ROUTINE
 */


const char* empty_msg = "You beg and plead to Caer the goddess of munchkins\
 but only attract a following of off duty mail daemons who taunt you that\
 the goddess has nothing to return you.";

const char* junk_undo_msg = "You abase yourself before Caer, the goddess of\
 munchkins in fervent prayer for your lost items.  Fortunately it seems your\
 whining is noticed.";


thing_data* junk( thing_data* thing, char_data*, thing_data* )
{
  return thing;
}


void execute_junk( event_data* event )
{
  player_data* pc = (player_data*) event->owner;

  extract( pc->junked );
  event->owner->events -= event;

  delete event;
}
  

void do_junk( char_data* ch, char* argument )
{
  thing_array*  array;
  thing_array   subset  [ 2 ];
  thing_func*     func  [ 2 ]  = { cursed, junk };
  player_data*      pc         = player( ch );
  event_data*    event;
  obj_data*        obj;

  page_priv( ch, NULL, empty_string );

  if( !strcasecmp( argument, "undo" ) ) {
    if( pc == NULL ) {
      send( ch, "Only player may junk undo.\n\r" );
      return;
      }
    if( is_empty( pc->junked ) ) {
      fsend( ch, empty_msg );
      return;
      }
    fpage( ch, junk_undo_msg );
    page( ch, "\n\r" );

    page_priv( ch, &pc->junked, NULL, NULL,
      "appears in a flash of light",
      "appear in a flash of light" );

    for( int i = pc->junked-1; i >= 0; i-- ) {
      pc->junked[i]->From( pc->junked[i]->number ); 
      pc->junked[i]->To( ch );
      }

    stop_events( ch, execute_junk );
    return;
    }

  if( ( array = several_things( ch, argument,
    "junk", &ch->contents ) ) == NULL ) 
    return;

  sort_objects( ch, *array, NULL, 2, subset, func );

  page_priv( ch, &subset[0], "can't let go of" );
  page_publ( ch, &subset[1], "junk" );

  if( !is_empty( subset[1] ) ) {
    if( pc != NULL ) {
      stop_events( ch, execute_junk );
      extract( pc->junked );
      for( int i = 0; i < subset[1]; i++ ) {
        obj = (obj_data*) subset[1][i];
        obj = (obj_data*) obj->From( obj->selected );
        obj->To( &pc->junked );
        }
      event        = new event_data( execute_junk, ch );
      add_queue( event, 2000 );
      }
    else 
      extract( subset[1] );
    }

  delete array;
}


/*
 *   DROP ROUTINES
 */


thing_data* drop( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj;

  thing = thing->From( thing->selected );

  if( ( obj = object( thing ) ) != NULL )
    set_owner( obj, NULL, ch );

  thing->To( ch->array );

  return thing;
}


void do_drop( char_data* ch, char* argument )
{
  thing_array*  array;
  thing_array   subset  [ 2 ];
  thing_func*     func  [ 2 ]  = { cursed, drop };

  if( newbie_abuse( ch ) )
    return; 

  if( ( array = several_things( ch, argument,
    "drop", &ch->contents ) ) == NULL ) 
    return;
 
  sort_objects( ch, *array, NULL, 2, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't let go of" );
  page_publ( ch, &subset[1], "drop" );

  consolidate( subset[1] );

  delete array;
}











