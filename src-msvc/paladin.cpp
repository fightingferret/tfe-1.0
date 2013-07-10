#include "define.h"
#include "struct.h"


void do_hands( char_data* ch, char* argument )
{
  char_data*  victim;
  int           heal;
  int             hp;
 
  if( ch->species != NULL || ch->pcdata->clss != CLSS_PALADIN ) {
    send( ch, "Only paladins can lay hands to heal.\r\n" );
    return;
    }

  if( ch->shdata->skill[SKILL_LAY_HANDS] == 0 ) {
    send( ch, "You don't know that skill.\r\n" );
    return;
    } 

  if( *argument == '\0' ) {
    victim = ch;
    }
  else {
    if( ( victim = one_character( ch, argument, "lay hands on",
      ch->array ) ) == NULL )
      return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "You can't lay hands while fighting!\r\n" );
    return;
    }
    
  if( victim->fighting != NULL ) {
    send( ch, "You can't lay hands on someone who is fighting!\r\n" );
    return;
    }

  if( ch->mana < 2 ) {
    send( ch, "You don't have enough energy to do that.\r\n" );
    return;
    }

  if( ( hp = victim->max_hit-victim->hit ) == 0 ) {
    if( ch == victim ) 
      send( ch, "You aren't hurt.\r\n" );
    else 
      send( ch, "%s is not hurt.\r\n", victim );
    return;
    }

  if( victim == ch ) {
    send( ch, "You lay hands on yourself.\r\n" );
    send_seen( ch, "%s lays hands on %sself.\r\n",
      ch, ch->Him_Her( ) );
    }
  else {    
    send( ch, "You lay hands on %s.\r\n", victim );
    send( victim, "%s lays hands on you.\r\n", ch );
    send_seen( ch, "%s lays hands on %s.\r\n", ch, victim );
    }
  
  heal = min( hp/2, ch->mana );

  victim->hit = (int) (victim->hit + heal * (0.5 + ch->shdata->skill[SKILL_LAY_HANDS] * 0.05 + ch->shdata->skill[SKILL_REGENERATION] * 0.1));
  
  if( hp > heal ) 
    ch->mana -= heal;
  else 
    ch->mana -= (int) (hp*(ch->shdata->skill[SKILL_LAY_HANDS]*0.05)* (ch->shdata->skill[SKILL_REGENERATION]*0.1));
       

  victim->hit = min( victim->max_hit, victim->hit );

  update_pos( victim );

  if( number_range( 0, 1 ) == 0 )
    ch->improve_skill( SKILL_LAY_HANDS );
  else
    if( ch->shdata->skill[SKILL_REGENERATION] > 0 )
      ch->improve_skill( SKILL_REGENERATION );

  return;
}















