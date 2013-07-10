#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   ASSIST ROUTINE
 */


void do_assist( char_data* ch, char* argument )
{
  char_data*    victim;
  char_data*  opponent;

  if( ( victim = one_character( ch, argument, "assist", 
    ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "You can't assist yourself.\n\r" );
    return;
    }

  if( ( opponent = victim->fighting ) == NULL ) {
    send( ch, "%s isn't fighting anyone.\n\r", victim );
    return;
    }

  if( victim->species != NULL ) {
    send( ch, "You can only assist players.\n\r" );
    return;
    }

  if( opponent == ch ) {
    send( ch, "Perhaps you should let the fight finish on its own.\n\r" );
    return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "You are already in battle.\n\r" );
    return;
    }

  if( !can_kill( ch, opponent ) )
    return;

  check_killer( ch, opponent );
  ch->fighting = opponent;
  react_attack( ch, opponent );

  remove_bit( &ch->status, STAT_WIMPY );

  fight_round( ch );
}


/*
 *   RESCUE ROUTINE
 */


void do_rescue( char_data* ch, char* argument )
{
  char_data*  victim;
  char_data*     rch;
  char_array    list;

  if( is_mob( ch ) )
    return;

  if( ( victim = one_character( ch, argument, "rescue",
    ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "What about fleeing instead?\n\r" );
    return;
    }

  if( victim->species != NULL && !is_set( &victim->status, STAT_PET ) ) { 
    send( ch, "You are unable to rescue monsters.\n\r" );
    return;
    }

  if( ch->fighting == victim ) {
    send( ch, "You can't rescue someone you are attacking!\n\r" );
    return;
    }

  for( int i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL
      && rch->fighting == victim ) {
      if( !can_kill( ch, rch ) )
        return;
      list += rch;
      }

  if( is_empty( list ) ) {
    send( ch, "Noone is attacking %s.\n\r", victim );
    return;
    }

  ch->improve_skill( SKILL_RESCUE );
  
  if( !ch->check_skill( SKILL_RESCUE ) ) {
    send( ch, "You fail to rescue %s.\n\r", victim );
    set_delay( ch, 10 );
    return;
    }

  send( ch, "You rescue %s!\n\r", victim );
  send( victim, "++ %s rescues you! ++\n\r", ch );
  send( *ch->array, "%s rescues %s!\n\r", ch, victim );

  if( victim->fighting != NULL ) {
    ch->fighting     = victim->fighting;
    victim->fighting = NULL;
    }
  else {
    ch->fighting = list[0];
    }

  for( int i = 0; i < list; i++ )
    list[i]->fighting = ch;

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );
  set_delay( ch, 10 );
}


/*
 *   BERSERK ROUTINE
 */


void do_berserk( char_data* ch, char* ) 
{
  if( is_mob( ch ) )
    return;

  if( ch->fighting == NULL ) {
    send( ch, "You aren't in battle.\n\r" );
    return;
    }

  if( ch->pcdata == NULL || ch->species != NULL 
    || ch->shdata->skill[ SKILL_BERSERK ] == 0 ) {
    send( ch, "You are unable to incite yourself into a further rage.\n\r" );
    return;
    }

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "You are already in a frenzy beyond control.\n\r" );
    return;
    }

  set_bit( &ch->status, STAT_BERSERK );

  ch->improve_skill( SKILL_BERSERK );
  
  send( ch, "You scream a cry of war, and attack with renewed fury!!\n\r" );
  fsend( *ch->array,
    "%s screams a cry of war, and attacks with renewed fury!!", ch );
}
    










