#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void char_bash   ( char_data*, char_data* );
void obj_bash    ( char_data*, obj_data*, obj_data* );


void do_bash( char_data* ch, char* argument )
{
  char            arg  [ MAX_INPUT_LENGTH ];
  char_data*   victim;
  int            door;
  obj_data*     chest;
  obj_data*  bludgeon;

  if(  is_confused_pet( ch )
    || is_mounted( ch )
    || is_entangled( ch, "bash" ) )
    return;

  if( *argument == '\0' ) {
    if( ( victim = opponent( ch ) ) == NULL ) {
      send( ch, "Who or what do you want to bash?\n\r" );
      return;
      }
    char_bash( ch, victim );
    return;
    }
  /*  
  if( ( victim = get_victim( ch, argument, empty_string ) ) != NULL ) {
    char_bash( ch, victim );
    return;
    }

  if( ( door = find_door( ch, argument, FALSE ) ) >= 0 ) {
    door_bash( ch, door );
    return;
    }

  if( door == -1 ) {
    send( ch, "There isn't a door there for you to bash.\n\r" );
    return;
    }

  argument = one_argument( argument, arg );

  if( ( chest = one_object( ch, arg,
    "bash", ch->array ) ) == NULL )
    return;
  
  if( *argument == '\0' ) {
    send( ch, "With what do you wish to bash %s?\n\r", chest );
    return;
    }

  if( ( bludgeon = one_object( ch, argument,
    "use as a bludgeon", &ch->contents ) ) == NULL )
    return;

  obj_bash( ch, chest, bludgeon );
  */
  return;
}


/*
 *   CHARACTER BASH FUNCTION
 */ 


void char_bash( char_data* ch, char_data* victim )
{
  int delay;

  if( victim == ch ) {
    send( ch, "Bashing yourself is not very productive.\n\r" );
    return;
    }

  if( victim->species != NULL ) { 
    if( is_set( &victim->species->act_flags, ACT_NO_BASH ) ) {
      send( ch, "Bashing that does not make sense.\n\r" );
      return;
      }
    if( is_set( &victim->species->act_flags, ACT_GHOST ) ) {
      send( ch, "Bashing a ghost is a completely futile exercise.\n\r" );
      return;
      }
    }

  if( victim->position < POS_FIGHTING ) {
    send( ch, "Your victim is already on the ground!\n\r" );
    return;
    }

  if( victim->Size( ) > ch->Size( )+1 ) {
    send( ch, "%s is way too large for you to successfully bash %s.\n\r",
      victim, victim->Him_Her( ) );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  ch->fighting = victim;
  react_attack( ch, victim );  

  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_BASH );

  delay = bash_attack( ch, victim );
  add_queue( &ch->active, delay );
}


int bash_attack( char_data* ch, char_data* victim )
{
  int roll;

  roll = number_range( 0, 20 )-2*(victim->Size( )-ch->Size( )) 
    -(victim->shdata->dexterity-ch->shdata->dexterity)/2 -
    (victim->shdata->level-ch->shdata->level)/5;

  if( ch->species == NULL )  
    roll += ch->get_skill( SKILL_BASH )/2;
  else
    roll += 3;

  if( roll < 4 ) {  
    if( number_range( 0, 26 ) < ch->shdata->dexterity ) {
      send( ch, "You attempt to bash %s, but miss and fall down.\n\r",
        victim );
      send( victim,
        "%s attempts to bash you, but %s misses and falls down.\n\r",
        ch, ch->He_She( victim ) );
      send( *ch->array,
        "%s attempts to bash %s, but %s misses and falls down.\n\r",
        ch, victim, ch->He_She( ) );
      ch->position = POS_RESTING;
      }
    else {
      send( ch, "You attempt to bash %s but are unsuccessful.\n\r", victim );
      send( victim, "%s attempts to bash you but is unsuccessful.\n\r", ch );
      send( *ch->array,
        "%s attempts to bash %s but is unsuccessful.\n\r",
        ch, victim );
      }
    return 32;
    }

  if( roll > 20 ) {
    send( ch, "You send %s sprawling!!\n\r", victim );
    send( victim, "%s sends you sprawling!!\n\r", ch );
    send( *ch->array, "%s sends %s sprawling!!\n\r", ch, victim );
    disrupt_spell( victim ); 
    set_delay( victim, 32 );
    victim->position = POS_RESTING;
    return 20;
    }

  if( roll < 15 ) {
    send( ch, "You attempt to bash %s but fail.\n\r", victim );
    send( victim, "%s attempts to bash you but fails.\n\r", ch );
    send( *ch->array, 
      "%s attempts to bash %s but fails.\n\r", ch, victim );
    return 20;
    }
  
  send( ch, "You attempt to bash %s, but are knocked down yourself.\n\r",
    victim );
  send( victim, "%s attempts to bash you, but you knock %s down instead.\n\r",
    ch, ch->Him_Her( ) );
  send( *ch->array,
    "Attempting to bash %s, %s is knocked down.\n\r", victim, ch ); 

  ch->position = POS_RESTING;

  return 32;
}


/*
 *   OBJECT BASH ROUTINE
 */


void obj_bash( char_data* ch, obj_data*, obj_data* )
{
  send( ch, "Bashing objects does nothing useful yet.\n\r" );
  return;
}








