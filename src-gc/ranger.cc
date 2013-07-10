#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *   SKIN FUNCTION
 */


void do_skin( char_data* ch, char* argument )
{
  mprog_data*         mprog;
  obj_data*          corpse;
  thing_data*         thing;
  thing_array*         list;
  species_data*     species;
  char_data*            rch;

  if( is_confused_pet( ch ) )
    return;

  if( ( thing = one_thing( ch, argument,
    "skin", ch->array ) ) == NULL )
    return;

  if( ( corpse = object( thing ) ) == NULL ) {
    fsend( ch, "Skinning %s alive would be cruel and unusual.\n\r", thing );
    return;
    } 

  if( corpse->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "You can only skin corpses.\n\r" );
    return;
    }

  if( corpse->pIndexData->vnum == OBJ_CORPSE_PC ) {
    send( ch, "You can't [yet] skin player corpses!\n\r" );
    return;
    }

  if( ( species = get_species( corpse->value[1] ) ) == NULL ) {
    send( ch, "Corpses without a species cannot be skinned.\n\r" );
    return;
    }

  for( mprog = species->mprog; mprog != NULL; mprog = mprog->next )
    if( mprog->trigger == MPROG_TRIGGER_SKIN ) {
      var_obj  = corpse;
      var_ch   = ch;    
      var_room = ch->in_room;
      execute( mprog ); 
      return;
      }

  list = get_skin_list( species );

  if( list == (thing_array*) -1 ) {
    send( ch, "You cannot skin %s.\n\r", corpse );
    return;
    }

  if( list == NULL ) {
    fsend( ch, "You skin %s, but it is too mangled to be of any use.",
      corpse );
    fsend( *ch->array,
      "%s skins %s but is unable to extract anything of value.",
      ch, corpse );
    }
  else {
    fsend( ch,
      "You skin %s producing %s.\n\r",
      corpse, list );

    for( int i = 0; i < *ch->array; i++ )  
      if( ( rch = character( ch->array->list[i] ) ) != NULL
         && rch != ch && ch->Seen( rch ) )
         fsend( rch,
           "%s skins %s producing %s.\n\r",
           ch, corpse, list );

    for( int i = 0; i < *list; i++ ) {
      list->list[i]->To( ch );
      consolidate( (obj_data*) list->list[i] );
      }

    delete list;
    }
  
  drop_contents( corpse ); 
  corpse->Extract( 1 );
}


/*
 *   SCAN FUNCTIONS
 */


bool scan_room( char_data* ch, room_data* room, const char* word,
  bool need_return )
{
  char           tmp  [ MAX_STRING_LENGTH ];
  const char*   name;
  char_data*     rch;
  bool         found  = FALSE;
  int         length  = 0; 

  room->distance = 0;

  select( room->contents, ch );
  rehash( ch, room->contents );

  for( int i = 0; i < room->contents; i++ ) {
    if( ( rch = character( room->contents[i] ) ) == NULL
      || rch->shown == 0
      || !rch->Seen( ch ) || ch == rch
      || ( rch->species != NULL
      && is_set( &rch->species->act_flags, ACT_MIMIC ) ) )
      continue;

    name = rch->Seen_Name( ch, rch->shown );

    if( !found ) {
      if( need_return )
        send( ch, "\n\r" );
      sprintf( tmp, "%12s : %s", word, name );
      *tmp   = toupper( *tmp );
      length = strlen( tmp );
      found  = TRUE;
      }
   else {
      length += strlen( name )+2;
      if( length > 75 ) {
        length = strlen( name )+7;
        sprintf( tmp+strlen( tmp ), ",\n\r              %s", name );
        }
      else
        sprintf( tmp+strlen(tmp), ", %s", name );
      }
    }

  if( found ) {
    strcat( tmp, "\n\r" );
    send( ch, tmp );
    }

  return found;
} 


void do_scan( char_data* ch, char* argument )
{
  char             tmp  [ ONE_LINE ];
  room_data*      room  = ch->in_room;
  room_data*     room1;
  room_data*     room2;
  bool        anything  = FALSE;
  bool         is_auto  = !strcmp( argument, "shrt" );

  if( !ch->in_room->Seen( ch ) ) {
    if( !is_auto )
      send( ch, "The room is too dark to scan.\n\r" );
    return;
    }

  if( !is_auto ) 
    anything = scan_room( ch, room, "[Here]", FALSE );

  room->distance = 0;

  for( int i = 0; i < room->exits; i++ ) {
    if( is_set( &room->exits[i]->exit_info, EX_CLOSED ) )
      continue;
    room1     = room->exits[i]->to_room;
    anything |= scan_room( ch, room1,
      dir_table[room->exits[i]->direction].name,
      !anything && is_auto );
    if( ch->get_skill( SKILL_SCAN ) != 0 ) {
      for( int j = 0; j < room1->exits; j++ ) {
        if( is_set( &room1->exits[j]->exit_info, EX_CLOSED ) )
          continue;
        room2 = room1->exits[j]->to_room;
        if( room2->distance != 0 ) {
          sprintf( tmp, "%s %s",
            room->exits[i]->direction == room1->exits[j]->direction
            ? "far" : dir_table[ room->exits[i]->direction ].name,
            dir_table[ room1->exits[j]->direction ].name );
          anything |= scan_room( ch, room2, tmp, !anything
            && is_auto );
          }
        }  
      }
    } 

  if( !anything && !is_auto ) 
    send( ch, "You see nothing in the vicinity.\n\r" );

  /*--  CLEANUP DISTANCE --*/

  room->distance = MAX_INTEGER;

  for( int i = 0; i < room->exits; i++ ) { 
    room1 = room->exits[i]->to_room;
    room1->distance = MAX_INTEGER;
    for( int j = 0; j < room1->exits; j++ )
      room1->exits[j]->to_room->distance = MAX_INTEGER;
    }
}


/*
 *   SPELLS
 */


bool spell_tame( char_data* ch, char_data* victim, void*, int level, int )
{
  if( null_caster( ch, SPELL_TAME ) )
    return TRUE;

  if( is_set( &victim->status, STAT_PET ) ) {
    send( ch, "%s is already tame.\n\r", victim );
    return TRUE;
    }

  if( victim->species == NULL
    || !is_set( &victim->species->act_flags, ACT_CAN_TAME )
    || makes_save( victim, ch, RES_MIND, SPELL_TAME, level )
    || victim->leader != NULL 
    || victim->shdata->level > ch->shdata->level ) {
    send( ch, "%s ignores you.\n\r", victim );
    send( *ch->array, "%s ignores %s.\n\r", victim, ch );
    return TRUE;
    }

  if( victim->shdata->level > ch->shdata->level-pet_levels( ch ) ) {
    send( ch, "You fail as you are unable to control more animals.\n\r" );
    return TRUE;
    }

  if( is_set( &victim->species->act_flags, ACT_MOUNT ) && has_mount( ch ) )
    return TRUE;

  if( ch->leader == victim )
    stop_follower( ch );

  set_bit( &victim->status, STAT_PET );
  set_bit( &victim->status, STAT_TAMED );

  remove_bit( &victim->status, STAT_AGGR_ALL );
  remove_bit( &victim->status, STAT_AGGR_GOOD );
  remove_bit( &victim->status, STAT_AGGR_EVIL );

  add_follower( victim, ch );

  return TRUE;
}


bool spell_barkskin( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_BARKSKIN, AFF_BARKSKIN );

  return TRUE;
}


bool spell_thorn_shield( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_THORN_SHIELD,
    AFF_THORN_SHIELD );

  return TRUE;
}


/*
 *   HEALING SPELLS
 */


bool spell_balm( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_BALM, level ) );

  return TRUE;
}


bool spell_surcease( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_SURCEASE, level ) );

  return TRUE;
}


bool spell_poultice( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_POULTICE, level ) );

  return TRUE;
}














