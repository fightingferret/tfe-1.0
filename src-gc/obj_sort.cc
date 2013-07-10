#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *
 */


thing_data* corpse( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj = object( thing );

  if( obj->pIndexData->item_type == ITEM_CORPSE )
    return NULL;

  for( int i = 0; i < obj->contents; i++ )
    if( !corpse( obj->contents[i], ch ) )
      return NULL;

  return obj;
}


thing_data* stolen( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj = object( thing );

  if( !obj->Belongs( ch ) )
    return NULL;

  for( int i = 0; i < obj->contents; i++ )
    if( !stolen( obj->contents[i], ch ) )
      return NULL;

  return obj;
}


thing_data* same( thing_data* obj, char_data*, thing_data* container )
{
  return( obj != container ? obj : NULL );
}


thing_data* cursed( thing_data* thing, char_data*, thing_data* )
{ 
  obj_data* obj = object( thing );

  return( obj->droppable( ) ? obj : NULL );
}


thing_data* cant_take( thing_data* thing, char_data* ch, thing_data* )
{
  obj_data* obj;

  if( ( obj = object( thing ) ) == NULL )
    return NULL;
 
  if( obj->array != ch->array 
    || can_wear( obj, ITEM_TAKE ) )
    return thing;

  return NULL;
}


thing_data* sat_on( thing_data* thing, char_data*, thing_data* )
{ 
  return thing;
}


thing_data* heavy( thing_data* thing, char_data* ch, thing_data* )
{
  int  n  = thing->Weight( thing->selected );
  int  m  = ch->Capacity( );

  if( m >= n || n <= 0 )
    return thing;
  
  if( ( m = thing->selected*m/n ) <= 0 )
    return NULL;
  
  content_array* where = thing->array;

  thing->selected -= m;
  thing = thing->From( m );
  thing->To( where );

  return thing;
}


thing_data* many( thing_data* thing, char_data* ch, thing_data* )
{
  int  n  = thing->Number( thing->selected );
  int  m  = ch->can_carry_n( )-ch->contents.number;

  if( m >= n || n <= 0 )
    return thing;
  
  if( ( m = thing->selected*m/n ) <= 0 )
    return NULL;

  content_array* where = thing->array;

  thing->selected -= m;
  thing = thing->From( m );
  thing->To( where );

  return thing;
}


/*
 *   REHASH  
 */


void select( thing_array& list )
{
  for( int i = 0; i < list; i++ )
    list[i]->selected = list[i]->number;
}  


void select( thing_array& list, char_data* ch )
{
  for( int i = 0; i < list; i++ )
    list[i]->selected = ( list[i]->Seen( ch ) ? list[i]->number : 0 );
}  


void rehash( char_data* ch, thing_array& list )
{
  for( int i = 0; i < list; i++ )  
    list[i]->shown = list[i]->selected;

  for( int i = 0; i < list-1; i++ )
    if( list[i]->shown > 0 ) 
      for( int j = i+1; j < list; j++ )
        if( ch->look_same( list[i], list[j] ) ) {
          list[i]->shown += list[j]->shown;
          list[i]->temp  += list[j]->temp;    
          list[j]->shown  = 0; 
          }
}


void rehash_weight( char_data* ch, thing_array& list )
{
  thing_data* t1;
  thing_data* t2;

  for( int i = 0; i < list; i++ ) {  
    t1        = list[i];
    t1->shown = t1->selected;
    t1->temp  = t1->Weight( t1->shown );
    }

  for( int i = 0; i < list-1; i++ ) 
    if( ( t1 = list[i] )->shown > 0 ) 
      for( int j = i+1; j < list; j++ ) 
        if( ch->look_same( t1, t2 = list[j] ) ) {
          t1->shown += t2->shown;
          t1->temp  += t2->temp; 
          t2->shown  = 0; 
          }
}


/*
 *   SORT ROUTINE
 */


void sort_objects( char_data* ch, thing_array& array, thing_data* container,
  int n, thing_array* subset, thing_func** func )
{
  thing_data*         thing;
  thing_data*      unsorted;

  for( int i = 0; i < array; i++ ) {
    thing = array[i];
    for( int j = 0; ; j++ ) {
      unsorted = ( func[j] == NULL ? thing 
        : ( *func[j] )( thing, ch, container ) );
      if( j == n-1 ) {
        subset[j] += unsorted;
        break;
        }      
      if( unsorted != thing ) 
        subset[j] += thing;
      if( ( thing = unsorted ) == NULL )
        break;
      }
    }
}


/*
 *   LIST ROUTINES
 */


bool first_list;
bool prev_long;


void page_priv( char_data* ch, thing_array* array, const char* msg1,
  thing_data* container, const char* msg2, const char* msg3 )
{
  thing_data* thing;

  if( array == NULL ) {
    first_list = TRUE;
    prev_long  = FALSE;
    return;
    }

  if( is_empty( *array ) )
    return; 

  rehash( ch, *array );

  if( ( thing = one_shown( *array ) ) != NULL ) {
    if( prev_long )
      page( ch, "\n\r" );
    if( container == NULL ) {
      if( msg1 == NULL )
        fpage( ch, "%s %s.", thing, 
          thing->shown == 1 ? msg2 : msg3 );
      else if( msg2 != empty_string )
        fpage( ch, "%s %s %s.\n\r", msg1,
          thing, msg2 ); 
      else
        fpage( ch, "You %s %s.", msg1, thing );
      }
    else if( container == thing ) {
      fpage( ch, "You %s %s %s.", msg1,
        thing, msg2 );
      }
    else if( msg2 != empty_string ) {
      fpage( ch, "You %s %s %s %s.", msg1, thing, msg2, container );
      }
    else {
      fpage( ch, "%s %s %s.", container, msg1, thing );
      }
    first_list = FALSE;
    prev_long  = FALSE;
    return;
    }

  if( !first_list ) 
    page( ch, "\n\r" );

  first_list = FALSE;
  prev_long  = TRUE;
   
  if( container == NULL ) {
    if( msg1 == NULL )
      page( ch, "%s:\n\r", msg3 );
    else if( msg2 != empty_string )
      page( ch, "%s %s:\n\r", msg1, msg2 ); 
    else
      page( ch, "You %s:\n\r", msg1 );
    }
  else if( msg2 != empty_string ) {
    page( ch, "You %s %s %s:\n\r", msg1, msg2, container );
    }
  else {
    page( ch, "%s %s:\n\r", container, msg1 );
    }

  for( int i = 0; i < *array; i++ ) {
    thing = array->list[i];
    if( thing->shown > 0 ) 
      page( ch, "  %s\n\r", thing );
    }
}


void page_publ( char_data* ch, thing_array* array, const char* msg1,
  thing_data* container, const char* msg2, const char* msg3 )
{
  char_data*  rch;
  thing_data*   thing;

  if( is_empty( *array ) )
    return; 

  /* TO CHARACTER */

  rehash( ch, *array );

  if( ( thing = one_shown( *array ) ) != NULL ) { 
    if( prev_long )
      page( ch, "\n\r" ); 
    if( !strcmp( msg3, "for" ) ) 
      fpage( ch, "You %s %s %s %s for %d cp.",
        msg1, thing, msg2, container, thing->temp );
    else if( container != NULL ) 
      fpage( ch, "You %s %s %s %s%s.",
        msg1, thing, msg2, container, msg3 );
    else 
      fpage( ch, "You %s %s.", msg1, thing );
    }
  else {
    if( !first_list )
      page( ch, "\n\r" );
    if( !strcmp( msg3, "for" ) )
      page( ch, "You %s %s %s:\n\r",
        msg1, msg2, container );
    else if( container != NULL ) 
      page( ch, "You %s %s %s%s:\n\r",
        msg1, msg2, container, msg3 );
    else 
      page( ch, "You %s:\n\r", msg1 );
    for( int i = 0; i < *array; i++ ) {
      thing = array->list[i];
      if( thing->shown > 0 )  
        if( !strcmp( msg3, "for" ) )
          page( ch, "  %s for %d cp\n\r",
            thing, thing->temp );
        else
          page( ch, "  %s\n\r", thing );
      }
    }

  first_list = FALSE;

  /* TO ROOM OCCUPANTS */

  for( int i = 0; i < *ch->array; i++ ) { 
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch == ch || rch->pcdata == NULL
      || !rch->in_room->Seen( rch )  
      || !rch->Accept_Msg( ch ) ) 
      continue; 

    rehash( rch, *array );

    if( ( thing = one_shown( *array ) ) != NULL ) { 
      if( !strcmp( msg3, "for" ) ) 
        fsend( rch, "%s %ss %s %s %s.",
          ch, msg1, thing, msg2, container );
      else if( container != NULL ) 
        fsend( rch, "%s %ss %s %s %s%s.",
          ch, msg1, thing, msg2,
          container == rch ? "you" : container->Name( rch ),
          msg3 );
      else 
        fsend( rch, "%s %ss %s.", ch, msg1, thing );
      continue;
      }

    if( !is_set( &rch->pcdata->message, MSG_MULTIPLE_ITEMS ) ) {
      if( !strcmp( msg3, "for" ) )
        fsend( rch, "%s %ss several items %s %s.",
          ch, msg1, msg2, container );
      else if( container != NULL ) 
        fsend( rch, "%s %ss several items %s %s%s.",
          ch, msg1, msg2,
          container == rch ? "you" : container->Name( rch ),
          msg3 );
      else   
        fsend( rch, "%s %ss several items.", ch, msg1 );
      continue;
      }

    if( !strcmp( msg3, "for" ) )
      send( rch, "%s %ss %s %s:\n\r",
        ch, msg1, msg2, container );
    else if( container != NULL ) 
      send( rch, "%s %ss %s %s%s:\n\r",
        ch, msg1, msg2,
        container == rch ? "you" : container->Name( rch ),
        msg3 );
    else 
      send( rch, "%s %ss:\n\r", ch, msg1 );

    for( int j = 0; j < *array; j++ ) {
      thing = array->list[j];
      if( thing->shown > 0 )  
        send( rch, "  %s\n\r", thing );
      }
    }

  return;
}


/*
 *   SEND_PRIV
 */


void send_priv( char_data* ch, thing_array* array, const char* msg1,
  thing_data* container )
{
  thing_data* thing;

  rehash( ch, *array );

  if( none_shown( *array ) ) {
    send( ch, "%s %s nothing.\n\r", container, msg1 );
    return;
    }

  if( ( thing = one_shown( *array ) ) != NULL ) {
    fsend( ch, "%s %s %s.", container, msg1, thing );
    return;
    }

  send( ch, "%s %s:\n\r", container, msg1 );

  for( int i = 0; i < *array; i++ ) {
    thing = array->list[i];
    if( thing->shown > 0 ) 
      send( ch, "  %s\n\r", thing );
    }
}


/*
 *   SEND_PUBL
 */


void send_publ( char_data* ch, thing_array* array, const char* msg1,
  const char* msg2 )
{
  char_data*     rch;
  thing_data*  thing;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch == ch )
      continue;

    select( *array, rch );
    rehash( rch, *array );

    if( none_shown( *array ) ) {
      send( rch, "%s %s.\n\r", ch, msg1 );
      }
  
    else if( ( thing = one_shown( *array ) ) != NULL ) {
      fsend( rch, "%s %s, %s %s.", ch, msg1, msg2, thing );
      }

    else {
      send( rch, "%s %s, %s:\n\r", ch, msg1, msg2 );
      for( int j = 0; j < *array; j++ ) {
        thing = array->list[j];
        if( thing->shown > 0 ) 
          send( rch, "  %s\n\r", thing );
        }
      }
    }
}


/* 
 *   SPECIALIZED LIST ROUTINES
 */


void list_wear( char_data* ch, thing_array* array )
{
  thing_data* thing;

  if( is_empty( *array ) )
    return; 

  rehash( ch, *array );

  if( ( thing = one_shown( *array ) ) != NULL ) {
    if( prev_long )
      page( ch, "\n\r" );
    page( ch, "You %s %s.\n\r", wear_verb[ thing->position ], thing );
    }
  else {
    if( !first_list )
      page( ch, "\n\r" ); 
    page( ch, "You:\n\r" );
    for( int i = 0; i < *array; i++ ) {
      thing = (obj_data*) array->list[i];
      if( thing->shown > 0 )
        page( ch, "  %s %s.\n\r", wear_verb[ thing->position ], thing );
      }
    }
}












