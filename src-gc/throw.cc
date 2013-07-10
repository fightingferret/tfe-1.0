#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void throw_drink_con  ( char_data*, char_data*, obj_data* );
void throw_potion     ( char_data*, char_data*, obj_data* );
bool throw_liquid     ( char_data*, char_data*, obj_data* );
void throw_message    ( char_data*, char_data*, obj_data*, const char*,
                        const char* );


void do_throw( char_data* ch, char* argument )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  char_data*   victim;
  obj_data*       obj;

  argument = one_argument( argument, arg );

  if( !strncasecmp( argument, "at ", 3 ) )
    argument += 3;

  if( *arg == '\0' ) {
    send( "Throw what?\n\r", ch );
    return;
    }
  /*
  if( ( obj = get_obj_inv( ch, arg ) ) == NULL ) {
    send( "You do not have that item.\n\r", ch );
    return;
    }

  if( obj->selected != 1 || obj->next_list != NULL ) {
    send( "You can only throw one item at a time.\n\r", ch );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Throw %s at whom?\n\r", obj );
    return;
    }

  if( ( victim = get_char_room( ch, argument ) ) == NULL ) {
    send( "They aren't here.\n\r", ch );
    return;
    }

  if( victim == ch ) {
    send( "You can't throw something at yourself!\n\r", ch ); 
    return;
    } 

  if( weight( obj ) > 10*ch->Strength( ) ) {
    send( ch, "%s is too heavy for you to throw any distance.\n\r", obj );
    return;
    }

  if( obj->pIndexData->item_type == ITEM_FOOD ) {
    send( ch, "You throw %s at %s - food fight!!\n\r", obj, victim );
    send( victim, "%s throws %s at you - food fight!!\n\r", ch, obj );
    send( *ch->array, "%s throws %s at %s - food fight!!\n\r", ch, obj, victim );
    obj->Extract( 1 );
    return;
    }

  if( obj->pIndexData->item_type != ITEM_DRINK_CON 
    && obj->pIndexData->item_type != ITEM_POTION ) {
    send( ch,
      "That is not something you can throw with any result yet.\n\r" ); 
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  remove_bit( &ch->status, STAT_WIMPY );

  check_killer( ch, victim );
  start_fight( ch, victim );

  if( obj->pIndexData->item_type == ITEM_DRINK_CON ) 
    throw_drink_con( ch, victim, obj );
  else 
    throw_potion( ch, victim, obj );

  set_delay( ch, 32 );
  */
  return;
} 


bool throw_liquid( char_data* ch, char_data* victim, obj_data* obj )
{
  int roll  = number_range( -50,50 )+ch->Dexterity( );

  if( roll < 0 ) {
    throw_message( ch, victim, obj,
      "%s throw%s %s at %s, but it misses %s completely!",
      victim->Him_Her( ) );
    return FALSE;
    }

  roll -= victim->Dexterity( );

  if( roll < 0 ) {
    throw_message( ch, victim, obj,
      "%s throw%s %s at %s, but %s adriotly dodge%s it!",
      victim->He_She( ) );
    return FALSE;
    }

  throw_message( ch, victim, obj,
    "%s throw%s %s at %s, splashing the contents all over %s!",
    victim->Him_Her( ) );

  return TRUE;
}


/*
 *   ITEM TYPE THROW ROUTINES
 */


void throw_potion( char_data* ch, char_data* victim, obj_data* obj )
{
  int     level;
  int  duration;

  if( throw_liquid( ch, victim, obj ) ) {
    if( ( level = obj->value[1] ) < 1 || level > 10 ) {
      bug( "Throw: level out of range." );
      level = 1;
      }

    if( ( duration = obj->value[2] ) < 1 ) {
      bug( "Throw: duration out of range." );
      duration = 2;
      }  

    if( obj->value[0] < 0 || obj->value[0] >= MAX_SPELL ) {
      bug( "Throw: spell out of range." );
      return;
      }

    duration = 1+duration/3;
    level    = 1+level/2;

    ( *spell_table[ obj->value[0] ].function )( ch, victim, obj,
      level, duration );
    }

  obj->Extract( 1 );

  return;
}


void throw_drink_con( char_data* ch, char_data* victim, obj_data* obj )
{
  int  liquid  = obj->value[2];  
  int   spell;

  if( obj->value[1] == 0 ) {
    obj->value[1] = -2;
    send( ch,
      "%s is empty - you need to go fill it with soup first.\n\r", obj );
    obj->value[1] = 0;
    return;
    }

  if( liquid >= table_max[ TABLE_LIQUID ] || liquid < 0 ) {
    bug( "Throw_Drink_Con: bad liquid number. (Obj# %d, Liq# %d)",
      obj->pIndexData->vnum, liquid );
    liquid = obj->value[2] = 0;
    }

  obj->value[1] = -2;
  spell         = liquid_table[liquid].spell;

  if( throw_liquid( ch, victim, obj ) && spell != -1 ) {
    if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
      bug( "Throw_Drink_Con: Liquid with non-spell skill." );
      return;
      }
    ( *spell_table[spell-SPELL_FIRST].function )( ch, victim,
      NULL, 10, 5 ); 
    }

  obj->value[1] = 0;    

  return;
}


/*
 *   THROW MESSAGE
 */
 

void throw_message( char_data* ch, char_data* victim, obj_data* obj,
  const char* text, const char* word )
{
  char         tmp  [ THREE_LINES ];
  char_data*   rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) != NULL ) {
      sprintf( tmp, text,
        rch == ch ? "You" : ch->Name( rch ),
        rch == ch ? "" : "s", obj->Name( rch ),
        rch == victim ? "you" : victim->Name( rch ),
        rch == victim ? "you" : word,
        rch == victim ? "" : "s" );
      fsend( rch, tmp );
      }
    }

  return;
}






