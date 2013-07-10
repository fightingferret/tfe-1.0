#include "define.h"
#include "struct.h"



void do_energize( char_data* ch, char* argument )
{
  /*
  char_data*  victim;
  int           heal;
  int           mana;
  double        cure;

  if( ch->species != NULL || ch->pcdata->clss != CLSS_MONK ) {
    send( ch, "Only monks can transfer energy.\r\n" );
    return;
    }

  if( ch->shdata->skill[SKILL_TRANSFER_ENERGY] == 0 ) {
    send( ch, "You don't know that skill.\r\n" );
    return;
    } 

  if( *argument == '\0' ) {
    victim = ch;
    }
  else {
    if( ( victim = get_char_room( ch, argument ) ) == NULL ) {
      send( "They aren't here.\r\n", ch );
      return;
      }
    }

  if( ch->fighting != NULL ) {
    send( ch, "You can't transfer energy while fighting!\r\n" );
    return;
    }
    
  if( victim->fighting != NULL ) {
    send( ch,
      "You can't transfer energy to someone who is fighting!\r\n" );
    return;
    }

  if( ch->mana < 2 ) {
    send( ch, "You don't have enough energy to do that.\r\n" );
    return;
    }

  if( ( mana = victim->max_mana-victim->mana ) == 0 ) {
    if( ch == victim ) 
      send( ch, "You cannot transfer energy to yourself.\r\n" );
    else 
      send( ch, "%s is at full mana.\r\n", victim );
    return;
    }

  if( victim == ch ) {
    send( ch, "You cannot transfer energy to yourself.\r\n" );
    }
  else {    
    send( ch, "You transfer energy to %s.\r\n", victim );
    send( victim, "%s transfers energy to you.\r\n", ch );
    send_seen( ch, "%s transfers energy to %s.\r\n", ch, victim );
    }

  cure = ch->shdata->skill[SKILL_TRANSFER_ENERGY]*0.1; 
  heal = number_range( 70, 120 );

  if( ch->mana-heal > 0 ) {
    victim->mana   = victim->mana+heal*cure;
    ch->mana = ch->mana-heal; }
  else {
    victim->mana = victim->mana+ch->mana*cure;
    ch->mana = 0;
  }

  victim->mana = min( victim->max_mana, victim->mana );

  update_pos( victim );

  ch->improve_skill( SKILL_TRANSFER_ENERGY );

  */  
  return;
}

void do_polymorph( char_data*, char* )
{
  /*
  char_data*        mob;
  obj_data*         obj;
  player_data*   player;

  return;
  if( ch->link == NULL )
    return;

  if( ch->link->original != NULL ) {
    send_to_char( "You can only polymorph in human form.\r\n", ch );
    return;
    }

  send_to_char( "You transform into a tiger.\r\n", ch );

  mob = create_mobile( get_species( 690 ) );
  char_to_room( mob, ch->in_room );

  act( "The air around $n fades into a strange array of swirling colors.",
    ch, NULL, NULL, TO_ROOM );
  act( "$e is enveloped by the colors and when they recede you see $N.",
    ch, NULL, mob, TO_ROOM );
  if( ch->contents != NULL ) {
    send_to_char( "You equipment clatters to the floor.\r\n", ch );
    for( ; ch->contents != NULL; ) {
      obj = remove( ch->contents, ch->contents->number );
      put_obj( obj, ch->in_room );
      }
    act( "$n's equipment lies scattered on the ground.",
      ch, NULL, NULL, TO_ROOM );
    } 
 
  char_from_room( ch ); 

  ch->link->character = mob;
  ch->link->original  = player;
  mob->link           = ch->link;
  ch->link            = NULL;
  mob->pcdata         = ch->pcdata;
  mob->timer          = 0;

  SET_BIT( ch->status, STAT_POLYMORPH );
  */
  return;
}


void monk_return( char_data* )
{
  return;
  /*
  REMOVE_BIT( ch->link->original->status, STAT_POLYMORPH );

  send_to_char( "You transform back to human form.\r\n", ch );

  ch->link->character       = ch->link->original;
  ch->link->original        = NULL;
  ch->link->character->link = ch->link; 

  char_to_room( ch->link->character, ch->in_room );
  ch->link                  = NULL;
  ch->pcdata                = NULL;
  ch->Extract( );
  */
  return;
}

 
