#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void do_kick( char_data* ch, char* argument )
{
  char_data* victim;

  if(  is_confused_pet( ch )
    || !is_humanoid( ch )
    || is_mounted( ch )
    || is_entangled( ch, "kick" ) )
    return;

  if( ( victim = get_victim( ch, argument, "Kick who?\n\r" ) ) == NULL )
    return;

  if( victim == ch ) {
    send( ch, "Kicking yourself is pointless.\n\r" );
    return;
    }

  if( ch->pcdata != NULL && ch->shdata->skill[ SKILL_KICK ] == UNLEARNT ) {
    send( ch, "You are untrained in the art of kicking.\n\r" );
    return;
    }

  if( victim->mount != NULL ) {
    send( ch, "You can't kick a mounted person.\n\r" );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );

  ch->improve_skill( SKILL_KICK );
  attack( ch, victim, "kick", NULL, roll_dice( 2,4 ), 0 );

  react_attack( ch, victim );
  add_queue( &ch->active, 20 );
}



void do_charge( char_data* ch, char* argument )
{
  char_data*  victim;
  obj_data*      obj;
  int           roll;
  int          skill  = ch->get_skill( SKILL_CHARGE );
  /*
  if( ch->mount != NULL ) {
    send( ch, "Charging while mounted is beyond your skill.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Charge whom?\n\r" );
    return;
    }

  if( ( victim = get_char_room( ch, argument, TRUE ) ) == NULL ) 
    return;

  if( ( obj = get_eq_char( ch, WEAR_HELD_R ) ) == NULL ) {
    send( "You must wield a weapon to charge at someone.\n\r", ch );
    return;
    }

  if( opponent( ch ) != NULL ) {
    send( "You are already fighting someone.\n\r", ch );
    return;
    }

  if( victim == ch ) {
    send( "How can you charge against yourself?\n\r", ch );
    return;
    }

  if( skill == 0 ) {
    send( "Charging is not part of your repertoire.\n\r", ch );
    return;
    }

  if( opponent( ch ) != NULL ) {
    send( ch, "You are unable to charge while fighting someone.\n\r" );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;


  check_killer( ch, victim );
  start_fight( ch, victim );
  set_attack( ch, victim, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );


  roll = number_range( 0, 20 )+ 
    +(ch->shdata->dexterity-victim->shdata->dexterity)*2+
    2*(ch->shdata->intelligence-victim->shdata->intelligence);

  if( roll < 10  ) {
      send( ch, "You attempt to charge %s, but miss and fall down.\n\r",
        victim );
      send( victim,
        "%s attempts to charge you, but %s misses and falls down.\n\r",
        ch, ch->He_She( victim ) );
      send( *ch->array,
        "%s attempts to charge %s, but %s misses and falls down.\n\r",
        ch, victim, ch->He_She( ) );
      ch->position = POS_RESTING;
    }
  else
    attack( ch, victim, "charge", obj, -1, 0 );
  */
  return;
}


