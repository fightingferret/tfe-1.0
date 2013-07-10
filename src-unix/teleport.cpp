#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*      
 *   ASTRAL GATE
 */


void make_gate( room_data* from, room_data* to )
{
  obj_data* gate;

  gate           = create( get_obj_index( OBJ_ASTRAL_GATE ) );
  gate->value[1] = to->vnum;
  gate->To( from );

  send( from->contents, "%s slowly rises from the ground.\n\r", gate );

  return;
}


bool spell_astral_gate( char_data* ch, char_data*, void* vo, int, int )
{
  room_data*  room  = (room_data*) vo;

  if( null_caster( ch, SPELL_ASTRAL_GATE ) )
    return TRUE;

  if( room == NULL ) {
    bug( "Astral_Gate: Null room." );
    return TRUE;
    }

  make_gate( room, ch->in_room );
  make_gate( ch->in_room, room );

  return TRUE;
}


/*      
 *   BLINK
 */


bool spell_blink( char_data* ch, char_data* victim, void*, int level, int )
{
  exit_data*  exit;
  room_data*  room;

  if( ch == NULL )
    ch = victim;

  if( ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "Blink only works in a room.\n\r" );
    return FALSE;
    } 

  for( int i = 0; i < level; i++ ) {
    if( ( exit = random_open_exit( room ) ) == NULL )
      break;
    if( ch->array->where != exit->to_room
      && exit->to_room->area->status == AREA_OPEN )
      room = exit->to_room;
    }

  if( room == ch->array->where ) {
    send( ch, "You are trapped!\n\r" );
    return TRUE;
    }

  send( *ch->array, "%s vanishes in the blink of an eye!\n\r", ch );
  send( ch, "You disappear and suddenly find yourself elsewhere.\n\r\n\r" );

  ch->From( );
  ch->To( room ); 

  send( *ch->array, "%s suddenly appears in a flash of white light!\n\r", ch );
  show_room( ch, room, FALSE, FALSE );

  return TRUE;
}


/*
 *   PASSDOOR
 */


bool spell_pass_door( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PASS_DOOR,
    AFF_PASS_DOOR );

  return TRUE;
}


/*
 *   RECALL
 */


bool spell_recall( char_data* ch, char_data* victim, void* vo, int, int )
{
  room_data *room = (room_data*) vo;

  if( !consenting( victim, ch, "recalling" ) ) 
    return TRUE;

  if( room == NULL ) {
    if( victim->species != NULL ) {
      if( !is_set( &victim->status, STAT_PET ) || victim->leader == NULL ) {
        send( ch, "Nothing happens.\n\r" );
        return TRUE;
        }
      room = get_temple( victim->leader );
      }
    else
      room = get_temple( victim );
    }

  send( *victim->array, "%s disappears in a flash of light.\n\r", victim );

  victim->From( );
  victim->To( room );

  send( "\n\r", victim );
  do_look( victim, "" );

  send( *victim->array, "%s appears in a flash of light.\n\r", victim );

  return TRUE;
}


/*
 *   SUMMON
 */


bool spell_summon( char_data* ch, char_data* victim, void*, int, int )
{
  thing_array   list;
  char_data*     rch;

  if( null_caster( ch, SPELL_SUMMON ) )
    return TRUE;
 
  if( victim->in_room == ch->in_room ) {
    send( ch, "Nothing happens.\n\r" );
    return TRUE;
    }

  if( victim->shdata->level >= LEVEL_BUILDER
    || victim->species != NULL
    || is_set( &victim->in_room->room_flags, RFLAG_NO_RECALL ) ) {
    send( ch, "You fail to summon them.\n\r" );
    send( victim,
      "The world comes back to focus as the summoning fails.\n\r" );
    return TRUE;
    }

  if( is_set( victim->pcdata->pfile->flags, PLR_NO_SUMMON ) ) {
    send( ch, "%s has no.summon set.\n\r", victim );
    return TRUE;
    }

  if( !consenting( victim, ch, "summoning" ) ) 
    return TRUE;

  /* MAKE LIST */

  list += victim;

  for( int i = 0; i < victim->in_room->contents; i++ ) 
    if( ( rch = character( victim->in_room->contents[i] ) ) != NULL
      && rch->leader == victim && rch->species != NULL ) 
      list += rch;

  /* TRANSFER CHARACTERS */

  send_seen( victim, "%s slowly fade%s out of existence.\n\r",
    &list, list > 1 ? "" : "s" );

  for( int i = 0; i < list; i++ ) {
    list[i]->From( );
    list[i]->To( ch->in_room );
    }

  send( victim,
    "\n\r** You feel yourself pulled to another location. **\n\r\n\r" );

  show_room( victim, ch->in_room, FALSE, FALSE );
  send_seen( victim, "%s slowly fade%s into existence.\n\r",
    &list, list > 1 ? "" : "s" );

  return TRUE;
}


/*
 *   TRANSFER
 */


bool spell_transfer( char_data* ch, char_data*, void*, int, int )
{
  if( null_caster( ch, SPELL_TRANSFER ) )
    return TRUE;
 
  return TRUE;
}









