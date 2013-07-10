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
    send( ch, "You can't assist yourself.\r\n" );
    return;
    }

  if( ( opponent = victim->fighting ) == NULL ) {
    send( ch, "%s isn't fighting anyone.\r\n", victim );
    return;
    }

  if( victim->species != NULL ) {
    send( ch, "You can only assist players.\r\n" );
    return;
    }

  if( opponent == ch ) {
    send( ch, "Perhaps you should let the fight finish on its own.\r\n" );
    return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "You are already in battle.\r\n" );
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
  int i;

  if( is_mob( ch ) )
    return;

  if( ( victim = one_character( ch, argument, "rescue",
    ch->array ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "What about fleeing instead?\r\n" );
    return;
    }

  if( victim->species != NULL && !is_set( &victim->status, STAT_PET ) ) { 
    send( ch, "You are unable to rescue monsters.\r\n" );
    return;
    }

  if( ch->fighting == victim ) {
    send( ch, "You can't rescue someone you are attacking!\r\n" );
    return;
    }

  for( i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL
      && rch->fighting == victim ) {
      if( !can_kill( ch, rch ) )
        return;
      list += rch;
      }

  if( is_empty( list ) ) {
    send( ch, "Noone is attacking %s.\r\n", victim );
    return;
    }

  ch->improve_skill( SKILL_RESCUE );
  
  if( !ch->check_skill( SKILL_RESCUE ) ) {
    send( ch, "You fail to rescue %s.\r\n", victim );
    set_delay( ch, 10 );
    return;
    }

  send( ch, "You rescue %s!\r\n", victim );
  send( victim, "++ %s rescues you! ++\r\n", ch );
  send( *ch->array, "%s rescues %s!\r\n", ch, victim );

  if( victim->fighting != NULL ) {
    ch->fighting     = victim->fighting;
    victim->fighting = NULL;
    }
  else {
    ch->fighting = list[0];
    }

  for( i = 0; i < list; i++ )
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
    send( ch, "You aren't in battle.\r\n" );
    return;
    }

  if( ch->pcdata == NULL || ch->species != NULL 
    || ch->shdata->skill[ SKILL_BERSERK ] == 0 ) {
    send( ch, "You are unable to incite yourself into a further rage.\r\n" );
    return;
    }

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "You are already in a frenzy beyond control.\r\n" );
    return;
    }

  set_bit( &ch->status, STAT_BERSERK );

  ch->improve_skill( SKILL_BERSERK );
  
  send( ch, "You scream a cry of war, and attack with renewed fury!!\r\n" );
  fsend( *ch->array,
    "%s screams a cry of war, and attacks with renewed fury!!", ch );
}
    










