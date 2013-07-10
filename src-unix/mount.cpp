#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


bool is_mounted( char_data* ch )
{
  if( ch->mount == NULL )
    return FALSE;

  send( "You can't do that while mounted.\n\r", ch );

  return TRUE;
}


void dismount( char_data* ch )
{
  if( ch->mount != NULL ) {
    ch->mount->rider = NULL;
    ch->mount = NULL;
    }

  set_bit( &ch->status, STAT_STUNNED );

  return;
}


void do_mount( char_data* ch, char* argument )
{
  char_data* victim;

  if( IS_NPC( ch ) || ch->shdata->skill[SKILL_RIDING] == 0 ) {
    send( ch, "You don't know how to ride.\n\r" );
    return;
    }

  if( ch->mount != NULL ) {
    send( ch, "You are already riding %s.\n\r", ch->mount );
    return;
    }

  if( ( victim = one_character( ch, argument, "mount",
    ch->array ) ) == NULL )
    return;

  if( IS_SET( ch->in_room->room_flags, RFLAG_NO_MOUNT ) ) {
    send( "You can't mount here.\n\r", ch );
    return;
    }

  if( victim->rider != NULL ) {
    send( ch, "Someone else is already riding them.\n\r" );
    return;
    } 

  if( victim->leader != ch || !is_set( &victim->status, STAT_PET )
    || !is_set( &victim->species->act_flags, ACT_MOUNT ) ) {
    send( ch, "%s refuses to let you mount %s.\n\r",
      victim, victim->Him_Her( ) );
    return;
    }

  if( victim->fighting != NULL ) {
    send( ch, "It is impossible to mount a fighting beast.\n\r" );
    return;
    }

  if( victim->Size( ) < ch->Size( )+1 ) {
    send( ch, "%s is too small to carry your weight.\n\r", victim );
    return;
    }

  if( victim->position != POS_STANDING ) {
    send( ch, "You can only mount a beast while it is standing.\n\r" );
    return;
    }

  leave_shadows( ch );

  if( ch->pcdata != NULL ) 
    remove_bit( ch->pcdata->pfile->flags, PLR_SNEAK );

  send( ch, "You mount %s.\n\r", victim );
  send( *ch->array, "%s mounts %s.\n\r", ch, victim );

  ch->mount     = victim;
  victim->rider = ch;

  ch->improve_skill( SKILL_RIDING );

  return;
}


void do_dismount( char_data *ch, char* )
{
  if( ch->mount == NULL ) {
    send( "You aren't mounted.\n\r", ch );
    return;
    }  

  send( ch, "You dismount from %s.\n\r", ch->mount );
  send_seen( ch, "%s dismounts from %s.\n\r", ch, ch->mount );

  ch->position = POS_STANDING;
  ch->mount->rider = NULL;
  ch->mount = NULL;

  return;
}


void check_mount( char_data* ch )
{
  if( ch->rider != NULL )
    ch = ch->rider;

  if( ch->mount == NULL )
    return;

  if( number_range( -10, 20 ) < ch->shdata->skill[SKILL_RIDING]
    +ch->shdata->skill[SKILL_MOUNTED_FIGHTING] )
    return;

  send( ch, "You are thrown from your mount.\n\r" );
  fsend_seen( ch, "%s is thrown from %s which %s was riding.",
    ch, ch->mount, ch->He_She( ) );

  ch->mount->rider = NULL;
  ch->mount = NULL;
  ch->position = POS_RESTING;

  return;
}

