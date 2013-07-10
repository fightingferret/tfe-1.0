#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   SWITCH/RETURN
 */


void do_switch( char_data* ch, char* argument )
{
  /*
  char_data*     victim;
  link_data*       link;
  player_data*   pc;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( ch->shdata->level < LEVEL_BUILDER ) {
    if( ( victim = pc->familiar ) == NULL ) {
      send( ch, "You don't have a familiar to switch to!\n\r" );
      return;
      }
    }
  else {
    if( *argument == '\0' ) {
      send( ch, "Syntax: switch <mob>\n\r" );
      return;
      }

    if( ( victim = get_char_world( ch, argument ) ) == NULL ) {
      send( ch, "They aren't here.\n\r" );
      return;
      }

    if( victim->type( ) != MOB_DATA ) {
      send( ch, "You cannot switch to players.\n\r" );
      return;
      }

    if( victim->link != NULL ) {
      send( ch, "Character in use.\n\r" );
      return;
      }
    }

  link = ch->link;

  link->character  = victim;
  pc->switched = victim;
  pc->link     = NULL;
  victim->link     = link;
  victim->pcdata   = pc->pcdata;
  victim->timer    = current_time;

  send( victim, "Ok.\n\r" );
  */
  return;
}


void do_return( char_data* ch, char* )
{
  link_data*       link;
  player_data*   pc;

  if( not_player( ch ) )
    return;

  if( ch->link == NULL || ch->link->player == ch ) {
    send( ch, "You aren't switched.\n\r" );
    return;
    }

  send( ch, "You return to your original body.\n\r" );

  link             = ch->link;
  pc           = link->player;
  link->character  = pc;
  pc->link     = ch->link; 
  ch->link         = NULL;
  ch->pcdata       = NULL;
  pc->switched = NULL;

  return;
}


/*
 *   SUMMONING SPELLS
 */


void find_familiar( char_data* ch, obj_data* obj, int level,
  int species_list, int obj_list )
{
  char_data*      familiar;
  player_data*      pc;
  species_data*    species;
  int                    i;

  if( obj == NULL ) {
    bug( "Find_Familiar: Null Object as reagent!?" );
    return;
    }

  if( ( pc = player( ch ) ) == NULL )
    return;

  if( ch->shdata->level < LEVEL_APPRENTICE
    && is_set( ch->pcdata->pfile->flags, PLR_FAMILIAR ) ) {
    send( ch, "Nothing happens.\n\r" );
    send( ch, "You can only summon one familiar per level.\n\r" );
    return;
    }

  if( pc->familiar != NULL ) {
    send( ch, "Nothing happens.\n\r" );
    send( ch, "You can only have one familiar at a time.\n\r" );
    return;
    }

  for( i = 0; i < 10; i++ )
    if( obj->pIndexData->vnum
      == list_value[ obj_list ][ 10*(ch->shdata->alignment%3)+i ] )
      break;

  if( i == 10 ) {
    send( ch, "Nothing happens.\n\r" );
    return;
    }

  send( *ch->array, "%s disintegrates in a burst of blue flame.\n\r", obj );
  obj->Extract( 1 );

  if( number_range( 0,100 ) < 50-7*level+10*i ) {
    send( ch, "You feel the summoning fail.\n\r" );
    return;
    }

  i = list_value[ species_list ][ 10*(ch->shdata->alignment%3)+i ];

  if( ( species = get_species( i ) ) == NULL ) {
    bug( "Find_familiar: unknown species." );
    return;
    }

  familiar         = create_mobile( species );
  familiar->reset  = NULL;
  pc->familiar = familiar;

  set_bit( &familiar->status, STAT_PET );
  set_bit( &familiar->status, STAT_FAMILIAR );
  set_bit( ch->pcdata->pfile->flags, PLR_FAMILIAR );

  remove_bit( &familiar->status, STAT_AGGR_ALL );
  remove_bit( &familiar->status, STAT_AGGR_GOOD );
  remove_bit( &familiar->status, STAT_AGGR_EVIL );

  familiar->To( ch->array );

  send( ch,
    "%s comes to your summons, stepping from the shadows.\n\r",
    familiar );

  add_follower( familiar, ch );

  return;
}


bool spell_lesser_summoning( char_data* ch, char_data*, void* vo,
  int level, int )
{
  if( null_caster( ch, SPELL_LESSER_SUMMONING ) ) 
    return FALSE;

  find_familiar( ch, (obj_data*) vo, level,
    LIST_LS_SPECIES, LIST_LS_REAGENT );

  return TRUE;
}


bool spell_find_familiar( char_data* ch, char_data*, void* vo,
  int level, int )
{
  if( null_caster( ch, SPELL_FIND_FAMILIAR ) ) 
    return FALSE;

  find_familiar( ch, (obj_data*) vo, level,
    LIST_FF_SPECIES, LIST_FF_REAGENT );

  return TRUE;
}


bool spell_request_ally( char_data* ch, char_data*, void* vo,
  int level, int )
{
  if( null_caster( ch, SPELL_REQUEST_ALLY ) ) 
    return FALSE;

  find_familiar( ch, (obj_data*) vo, level,
    LIST_RA_SPECIES, LIST_RA_REAGENT );

  return TRUE;
}


void find_buddy( char_data* ch, obj_data* obj, int level,
  int species_list, int obj_list )
{
  char_data*         buddy;
  player_data*      pc;
  species_data*    species;
  int                    i;

  if( obj == NULL ) {
    bug( "Find_Buddy: Null Object as reagent!?" );
    return;
    }

  if( ( pc = player( ch ) ) == NULL )
    return;

  for( i = 0; i < 10; i++ )
    if( obj->pIndexData->vnum
      == list_value[ obj_list ][ 10*(ch->shdata->alignment%3)+i ] )
      break;

  if( i == 10 ) {
    send( ch, "Nothing happens.\n\r" );
    return;
    }

  send( *ch->array, "%s disintegrates in a burst of blue flame.\n\r", obj );
  obj->Extract( 1 );

  if( number_range( 0,100 ) < 50-7*level+10*i ) {
    send( ch, "You feel the summoning fail.\n\r" );
    return;
    }

  i = list_value[ species_list ][ 10*(ch->shdata->alignment%3)+i ];

  if( ( species = get_species( i ) ) == NULL ) {
    bug( "Find_buddy: unknown species." );
    return;
    }

  buddy = create_mobile( species );

  set_bit( &buddy->status, STAT_PET );

  remove_bit( &buddy->status, STAT_AGGR_ALL );
  remove_bit( &buddy->status, STAT_AGGR_GOOD );
  remove_bit( &buddy->status, STAT_AGGR_EVIL );

  buddy->To( ch->array );

  send( ch,
    "%s comes to your summons, stepping from the shadows.\n\r",
    buddy );

  add_follower( buddy, ch );

  return;
}


bool spell_construct_golem( char_data* ch, char_data*, void* vo, 
  int level, int )
{

  if( null_caster( ch, SPELL_CONSTRUCT_GOLEM ) ) 
    return FALSE;
  
  if( has_elemental( ch ) ) {
    send( ch, "You can have only one golem at a time.\n\r" );
    return FALSE;
    }
  
  find_buddy( ch, (obj_data*) vo, level,
    LIST_CG_SPECIES, LIST_CG_REAGENT );

  return TRUE;
}


bool spell_conjure_elemental( char_data* ch, char_data*, void* vo,
  int level, int )
{
  if( null_caster( ch, SPELL_CONJURE_ELEMENTAL ) ) 
    return FALSE;

  if( has_elemental( ch ) ) {
    send( ch, "You can have only one elemental at a time.\n\r" );
    return FALSE;
    }

  find_buddy( ch, (obj_data*) vo, level,
    LIST_CE_SPECIES, LIST_CE_REAGENT );

  return TRUE;
}


bool spell_find_mount( char_data* ch, char_data*, void* vo,
  int level, int )
{
  if( null_caster( ch, SPELL_FIND_MOUNT ) ) 
    return FALSE;

  if( has_mount( ch ) )  {
    send( ch, "You can only have one mount at a time.\n\r" );
    return FALSE;
    }

  find_buddy( ch, (obj_data*) vo, level,
    LIST_FM_SPECIES, LIST_FM_REAGENT );

  return TRUE;
}

