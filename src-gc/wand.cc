#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


void do_brandish( char_data *ch, char* )
{
  send( "Disabled\n\r", ch );
  return;
}


void do_zap( char_data* ch, char* argument )
{
  char             arg  [ MAX_STRING_LENGTH ];
  char_data*    victim;
  obj_data*       wand;
  cast_data*      cast;
  obj_data*        obj;
  room_data*      room;
  int            level;
  int         duration;
  int            spell;
  /*
  argument = one_argument( argument, arg );

  if( ( wand = get_obj_inv( ch, arg ) ) == NULL
    && ( wand = get_obj_wear( ch, arg ) ) == NULL ) {
    send( ch, "You do not have that wand.\n\r" );
    return;
    }

  if( wand->wear_loc == -1 ) {
    send( ch, "You need to be holding an item to zap it.\n\r" );
    return;
    }

  if( wand->pIndexData->item_type != ITEM_WAND ) {
    send( ch, "You can only zap wands.\n\r" );
    return;
    }

  if( ( spell = wand->pIndexData->value[0] ) < 0
    || spell >= MAX_SPELL ) {
    send( ch, "That wand contains a non-existent spell.\n\r" );
    bug( "Do_Zap: wand value out of range" );
    return;
    }

  cast        = new cast_data;
  cast->spell = spell;

  if( !get_target( ch, cast, argument ) ) {
    delete cast;
    return;
    }

  victim  = cast->victim;
  obj     = cast->obj;
  room    = cast->room;

  delete cast;

  if( wand->value[3] <= 0 ) {
    send( ch, "You zap %s, but nothing happens.\n\r", wand );
    send_seen( ch, "%s zaps %s, but nothing happens.\n\r", ch, wand );
    return;
    }

  if( spell_table[ spell ].type == STYPE_OFFENSIVE ) {
    check_killer( ch, victim );
    start_fight( victim, ch );
    }

  wand->value[3]--;

  if( victim == NULL ) {
    send( ch, "You zaps %s.\n\r", wand );
    send_seen( ch, "%s zaps %s.\n\r", ch, wand );
    }
  else if( victim != ch ) {
    send( ch, "You zap %s at %s.\n\r", wand, victim );
    send_seen( ch, "%s zaps %s at %s.\n\r", ch, wand, victim );
    if( ch->Seen( victim ) )
      send( victim, "%s zaps %s at you!\n\r", ch, wand ); 
    }
  else {
    send( ch, "You zap %s at yourself.\n\r", wand );
    send_seen( ch, "%s zaps %s at %sself.",
      ch, wand, ch->Him_Her( ) );
    }

  remove_bit( &ch->status, STAT_HIDING );
  remove_bit( ch->affected_by, AFF_HIDE );
  remove_bit( &ch->status, STAT_CAMOUFLAGED );
  remove_bit( ch->affected_by, AFF_CAMOUFLAGE );
  remove_bit( ch->affected_by, AFF_INVISIBLE );
  strip_affect( ch, AFF_INVISIBLE );

  set_delay( ch, 20 );

  level    = wand->value[1];
  duration = wand->value[2];

  ( *spell_table[ spell ].function )( ch, victim,
    room == NULL ? obj : room, level, duration ); 
  */
  return;
}


/*
 *   POTION FUNCTIONS
 */


void do_quaff( char_data* ch, char* argument )
{
  obj_data*    obj;
  int        level;
  int     duration;
  int        spell;

  if( *argument == '\0' ) {
    send( ch, "Quaff what?\n\r" );
    return;
    }

  if( ( obj = one_object( ch, argument, "quaff", &ch->contents ) ) == NULL )
    return;

  if( obj->pIndexData->item_type != ITEM_POTION ) {
    send( ch, "You can quaff only potions.\n\r" );
    return;
    }

  if( ( spell = obj->pIndexData->value[0] ) < 0
    || spell >= MAX_SPELL ) {
    send( ch, "That potion contains a non-existent spell.\n\r" );
    bug( "Quaff: Spell out of Range." );
    bug( "-- Potion = %s.", obj->Seen_Name( NULL ) );
    return;
    }

  send( ch, "You quaff %s.\n\r", obj );
  send_seen( ch, "%s quaffs %s.\n\r", ch, obj );

  if( ( level = obj->value[1] ) < 1 || level > 25 ) {
    bug( "Quaff: Level out of range." );
    bug( "-- Potion = %s", obj->Seen_Name( NULL ) );
    level = 1;
    }

  if( ( duration = obj->value[2] ) < 1 ) {
    bug( "Quaff: Duration out of range." );
    bug( "-- Potion = %s", obj->Seen_Name( NULL ) );
    duration = 1;
    }  

  set_delay( ch, 10 );
  obj->Extract( 1 );

  ( *spell_table[ spell ].function )( NULL, ch, NULL,
    level, duration );

  return;
}


/*
 *   SCROLL ROUTINES
 */


void do_recite( char_data* ch, char* argument )
{
  char            arg  [ MAX_STRING_LENGTH ];
  cast_data*     cast;
  obj_data*    scroll;
  int           level;
  int        duration;
  int           spell;

  if( not_player( ch ) )
    return;

  if( is_set( &ch->status, STAT_BERSERK ) ) {
    send( ch, "Your mind is on killing, not reading scrolls.\n\r" );
    return;
    }

  argument = one_argument( argument, arg );

  if( ( scroll = one_object( ch, arg, "recite", &ch->contents ) ) == NULL )
    return;

  if( scroll->pIndexData->item_type != ITEM_SCROLL ) {
    send( ch, "%s isn't a scroll.\n\r", scroll );
    return;
    }

  if( !is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT )
    && ( is_set( ch->affected_by, AFF_BLIND )
    || ch->in_room->is_dark( ) ) ) {
    send( ch, "It is too dark for you read any writing.\n\r" );
    return;
    }

  if( ( spell = scroll->pIndexData->value[0] ) < 0
    || spell >= MAX_SPELL ) {
    send( ch, "That scroll contains a non-existent spell.\n\r" );
    bug( "Recite: Scroll spell out of range." );
    bug( "-- Scroll = %s", scroll->Seen_Name( NULL ) );
    return;
    }

  cast        = new cast_data;
  cast->spell = spell;

  if( !get_target( ch, cast, argument ) ) {
    delete cast;
    return;
    }

  thing_data*  target  = cast->target;
  char_data*   victim  = character( target );

  delete cast;

  if( spell_table[ spell ].type == STYPE_OFFENSIVE ) {
    check_killer( ch, victim );
    ch->fighting = victim;
    react_attack( ch, victim );
    }

  if( spell == SPELL_RECALL-SPELL_FIRST 
    && is_set( &ch->in_room->room_flags, RFLAG_NO_RECALL ) ) {
    send( ch, "You attempt to pronounce the words on the scroll but\
 absolutely nothing\n\rhappens.\n\r" );
    return;
    }

  send( ch, "You recite %s.\n\r", scroll );
  send_seen( ch, "%s recites %s.\n\r", ch, scroll );

  remove_bit( &ch->status, STAT_HIDING );
  remove_bit( ch->affected_by, AFF_HIDE );
  remove_bit( &ch->status, STAT_CAMOUFLAGED );
  remove_bit( ch->affected_by, AFF_CAMOUFLAGE );
  strip_affect( ch, AFF_INVISIBLE );

  if( ( level = scroll->value[1] ) < 1 || level > 25 ) {
    bug( "Recite: Level out of range." );
    bug( "-- Scroll = %s", scroll->Seen_Name( NULL ) );
    level = 1;
    }

  if( ( duration = scroll->value[2] ) < 1 ) {
    bug( "Recite: Duration out of range." );
    bug( "-- Scroll = %s", scroll->Seen_Name( NULL ) );
    bug( "--   Vnum = %d", scroll->pIndexData->vnum );
    duration = 1;
    }  

  set_delay( ch, 20 );

  ( *spell_table[ spell ].function )( ch, victim,
    target, level, duration );

  if( scroll->Is_Valid( ) )
    scroll->Extract( 1 );
}



