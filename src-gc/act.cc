#include "ctype.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   ACT
 */


void act( char_data* to, const char* text, visible_data* ch,
  visible_data* obj, const char* string1, const char* string2 )
{
  char  tmp  [ 3*MAX_STRING_LENGTH ];
  char  buf  [ 3*MAX_STRING_LENGTH ];

  if( to == NULL || to->link == NULL || *text == '\0' )
    return;

  act_print( buf, text, ch, NULL, obj, NULL, string1, string2, to );   
  convert_to_ansi( to, buf, tmp );
  send( to, tmp );
}


void act( char_data* to, const char* text, visible_data* ch,
  visible_data* victim, visible_data* obj1, visible_data* obj2 )
{
  char  tmp  [ 3*MAX_STRING_LENGTH ];
  char  buf  [ 3*MAX_STRING_LENGTH ];

  if( to == NULL || to->link == NULL || *text == '\0' )
    return;

  act_print( buf, text, ch, victim, obj1, obj2, NULL, NULL, to );   
  convert_to_ansi( to, buf, tmp );
  send( to, tmp );
}


/*
 *   ACT_AREA
 */


void act_area( const char* text, char_data* ch, char_data* victim,
  visible_data* obj )
{
  room_data*  room;
  char_data*   rch;

  if( ( room = Room( ch->array->where ) ) == NULL )
    return;

  for( room = room->area->room_first; room != NULL; room = room->next ) 
    if( room != ch->array->where )
      for( int i = 0; i < room->contents; i++ ) 
        if( ( rch = character( room->contents[i] ) ) != NULL
          && rch->position > POS_SLEEPING )
          act( rch, text, ch, victim, obj );
}


/*
 *   ACT_NOTCHAR
 */


void act_notchar( const char* text, char_data* ch, visible_data* obj1,
  visible_data* obj2 )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING 
      && rch->Accept_Msg( ch ) )
      act( rch, text, ch, obj1, obj2 );
}


void act_notchar( const char* text, char_data* ch, const char* string1,
  const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch != ch && rch->position > POS_SLEEPING  
      && rch->Accept_Msg( ch ) )
      act( rch, text, ch, NULL, string1, string2 );   
}


void act_notchar( const char* text, char_data* ch, visible_data* obj,
  const char* string1, const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING  
      && rch->Accept_Msg( ch ) )
      act( rch, text, ch, obj, string1, string2 );   
}


/* 
 *   ACT_ROOM
 */


void act_room( room_data* room, const char* text, const char* string1,
  const char* string2 )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ ) 
    if( ( rch = character( room->contents[i] ) ) != NULL
      && rch->position > POS_SLEEPING )
      act( rch, text, NULL, NULL, string1, string2 );   
}


void act_room( room_data* room, const char* text, 
  visible_data* obj1, visible_data* obj2 )
{
  char_data* rch;

  for( int i = 0; i < room->contents; i++ ) 
    if( ( rch = character( room->contents[i] ) ) != NULL
      && rch->position > POS_SLEEPING )
      act( rch, text, obj1, obj2 );
}


/*
 *   ACT_NEITHER
 */


void act_neither( const char* text, char_data* ch, char_data* victim,
  visible_data* obj1, visible_data* obj2 )
{
  char_data* rch;
 
  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch != victim && rch->position > POS_SLEEPING
      && rch->Accept_Msg( ch ) )
      act( rch, text, ch, victim, obj1, obj2 );
}


/*
 *  MAIN PRINT ROUTINE FOR ACT
 */


void act_print( char* out, const char* in, visible_data* ch,
  visible_data* victim, visible_data* obj1, visible_data* obj2,
  const char* string1, const char* string2, char_data* to )
{
  char_data*  pers;
  visible_data*  obj3;
  const char*     sub;
  char*         start;

  start = out;

  for( ; ; ) {
    if( *in != '$' ) {
      *out++ = *in;
      }
    else {
      pers = character( islower( *(++in) ) ? ch : victim );

      switch( toupper( *in ) ) {
       default:
        sub = "[BUG]";
        break;

       case '$':
        sub = "$";
        break;       

       case '1':
        sub = ( ch == NULL ? "[BUG]" : ch->Name( to, ch->selected ) );
        break;

       case '2':
        sub = ( victim == NULL ? "[BUG]" :
          victim->Name( to, victim->selected ) );
        break;

       case '3':
        sub = ( obj1 == NULL ? "[BUG]" : obj1->Name( to, obj1->selected ) );
        break;

       case 'P':
        obj3 = ( *in == 'p' ? obj1 : obj2 );
        sub = ( obj3 == NULL ? "[BUG]" : obj3->Name( to, obj3->selected ) );
        break;

       case 'D' :
       case 'T' :
        sub = ( *in == 't' ? string1 : string2 );
        if( sub == NULL )
          sub = "[BUG]";
        break;

       case 'N':
        sub = ( pers == NULL ? "[BUG]" : pers->Name( to ) );
        break;

       case 'E':
        sub = ( pers == NULL ? "[BUG]" : pers->He_She( to ) );
        break;

       case 'M':
        sub = ( pers == NULL ? "[BUG]" : pers->Him_Her( to ) );
        break;
 
       case 'S':
        sub = ( pers == NULL ? "[BUG]" : pers->His_Her( to ) );
        break;
        }
      strcpy( out, sub );
      out += strlen( sub );
      }
    if( *in++ == '\0' )
      break;
    }

  *start = toupper( *start );
  out--;

  if( out != start ) {
    if( *(out-1) == '\r' ) 
      return;
    }

  strcpy( out, "\n\r" );
}



