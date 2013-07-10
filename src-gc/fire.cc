#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


bool fireball_effect    ( char_data*, char_data*, int );


/*
 *   SMOKE COMMAND
 */


void do_smoke( char_data* ch, char* argument )
{
  obj_data*      pipe;
  obj_data*   tobacco;
  oprog_data*   oprog;

  if( *argument == '\0' ) {
    send( ch, "What do you want to smoke?\n\r" );
    return;
    }

  if( ( pipe = one_object( ch, argument,
    "smoke", &ch->contents ) ) == NULL )
    return;

  if( pipe->pIndexData->item_type == ITEM_TOBACCO ) {
    send( ch, "You need to put that in a pipe to smoke it.\n\r" );
    return;
    }

  if( pipe->pIndexData->item_type != ITEM_PIPE ) {
    send( ch, "That is not an item you can smoke.\n\r" );
    return;
    }

  if( is_empty( pipe->contents ) ) {
    send( ch, "%s contains nothing to smoke.\n\r", pipe );
    return;
    }

  tobacco = object( pipe->contents[0] );

  fsend( ch, "You smoke %s, inhaling the aroma from %s.",
    pipe, tobacco );
  fsend( *ch->array, "%s smokes %s.", ch, pipe );

  for( oprog = tobacco->pIndexData->oprog; oprog != NULL;
    oprog = oprog->next )
    if( oprog->trigger == OPROG_TRIGGER_USE ) {
      var_ch        = ch;
      var_room      = ch->in_room;
      var_obj       = tobacco;
      var_container = pipe;
      if( !execute( oprog ) )
        return;
      }

  tobacco->Extract( );
}


/*
 *   IGNITE
 */


void do_ignite( char_data* ch, char* argument )
{
  affect_data  affect;
  obj_data*       obj;
  obj_data*    source  = NULL;
  bool          found  = FALSE;
 
  if( ch->shdata->race == RACE_TROLL ) {
    send( ch, "Due to the natural attraction of flame and troll flesh and\
 the associated\n\rchildhood nightmares burning things is not one of your\
 allowed hobbies.\n\r" );
    return;
    }

  if( ( obj = one_object( ch, argument, "ignite",
    &ch->wearing, &ch->contents, ch->array ) ) == NULL )
    return;

  if( is_set( obj->extra_flags, OFLAG_BURNING ) ) {
    send( ch, "%s is already burning.\n\r", obj );
    return;
    }

  for( int i = 0; !found && i < *ch->array; i++ )
    if( ( source = object( ch->array->list[i] ) ) != NULL
      && is_set( source->extra_flags, OFLAG_BURNING ) ) 
      found = TRUE;

  for( int i = 0; !found && i < ch->contents; i++ )
    if( ( source = object( ch->contents[i] ) ) != NULL
      && is_set( source->extra_flags, OFLAG_BURNING ) ) 
      found = TRUE;

  if( !found ) {
    send( ch, "You have nothing with which to ignite %s.\n\r", obj );
    return;
    }

  if( obj->vs_fire( ) > 90 ) {
    send( ch, "Depressingly %s doesn't seem inclined to burn.\n\r", obj );
    return;
    }
  
  send( ch, "You ignite %s using %s.\n\r", obj, source );
  send( *ch->array, "%s ignites %s using %s.\n\r", ch, obj, source );
  
  affect.type      = AFF_BURNING;
  affect.duration  = 1;
  affect.level     = 1;
  affect.leech     = NULL;

  add_affect( obj, &affect );
}


/*
 *   FIRE DAMAGE ROUTINES
 */


index_data fire_index [] = 
{
  { "singes",                "singe",                  3 },
  { "scorches",              "scorch",                 7 },
  { "toasts",                "toast",                 15 },
  { "cooks",                 "cook",                  30 },
  { "fries",                 "fry",                   50 },
  { "SEARS",                 "SEAR",                  75 },
  { "CHARS",                 "CHAR",                 100 },
  { "* IMMOLATES *",         "* IMMOLATE *",         140 },
  { "* VAPORIZES *",         "* VAPORIZE *",         200 },
  { "** INCINERATES **",     "** INCINERATE **",     300 },  
  { "** CREMATES **",        "** CREMATE **",        400 },
  { "*** DISINTEGRATES ***", "*** DISINTEGRATE ***",  -1 }
};


bool damage_fire( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  damage *= 100-victim->Save_Fire( );
  damage /= 100;

  dam_message( victim, ch, damage, string,
    lookup( fire_index, damage, plural ) );
  
  return inflict( victim, ch, damage, "fire" );
}


int obj_data :: vs_fire( )
{
  int save  = 100;
  int    i;

  for( i = 0; i < MAX_MATERIAL; i++ ) 
    if( is_set( &pIndexData->materials, i ) )
      save = min( save, material_table[i].save_fire );

  if( pIndexData->item_type != ITEM_ARMOR 
    || pIndexData->item_type != ITEM_WEAPON ) 
    return save;

  return save+value[0]*(100-save)/(value[0]+2);
}


/* 
 *   FIRE BASED SPELLS
 */


bool spell_resist_fire( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_FIRE,
    AFF_RESIST_FIRE );
  
  return TRUE;
}


bool spell_fire_shield( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  if( is_submerged( victim ) ) {
    fsend_all( victim->in_room, "The water around %s bubbles briefly.\n\r",
      victim );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_FIRE_SHIELD,
    AFF_FIRE_SHIELD );

  return TRUE;
}


bool spell_ignite_weapon( char_data* ch, char_data*, void* vo,
  int level, int )
{
  affect_data  affect;
  obj_data*       obj  = (obj_data*) vo;

  if( null_caster( ch, SPELL_IGNITE_WEAPON ) )
    return TRUE;

  if( is_set( &obj->pIndexData->materials, MAT_WOOD ) ) {
    fsend( ch,
      "%s you are carrying bursts into flames which quickly consume it.",
      obj );
    fsend( *ch->array,
      "%s %s is carrying bursts into flames which quickly consume it.",
      obj, ch );
    obj->Extract( 1 );
    return TRUE;
    }

  affect.type      = AFF_FLAMING;
  affect.duration  = level;
  affect.level     = level;
  affect.leech     = NULL;

  add_affect( obj, &affect );

  return TRUE;
}

 
/*
 *   DAMAGE SPELLS
 */


bool spell_burning_hands( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_BURNING_HANDS ) ) 
    return TRUE;

  damage_fire( victim, ch, spell_damage( SPELL_BURNING_HANDS, level ),
    "*the burst of flame" );

  return TRUE; 
}


bool spell_flame_strike( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_FLAME_STRIKE ) ) 
    return TRUE;

  damage_fire( victim, ch, spell_damage( SPELL_FLAME_STRIKE, level ),
    "*An incandescent spear of flame" );

  return TRUE;
}



bool spell_conflagration( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_CONFLAGRATION ) ) 
    return TRUE;

  damage_fire( victim, ch, spell_damage( SPELL_CONFLAGRATION, level ),
    "*A raging inferno" );

  return TRUE;
}


/*
 *   FIREBALL
 */


bool spell_fireball( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_FIREBALL ) )
    return TRUE; 

  if( fireball_effect( ch, victim, level ) )
    return TRUE;

  if( victim->in_room != ch->in_room )
    return TRUE;

  if( victim->mount != NULL ) 
    fireball_effect( ch, victim->mount, level );

  if( victim->rider != NULL )
    fireball_effect( ch, victim->rider, level );

  return TRUE;
}


bool fireball_effect( char_data *ch, char_data *victim, int level )
{
  room_data*     dest;
  int               i;

  if( damage_fire( victim, ch, spell_damage( SPELL_FIREBALL,level ),
    "*The raging fireball" ) )
    return TRUE;

  /*
  if( victim->mount != NULL )
    if( number_range( 0, 12 ) > victim->get_skill( SKILL_RIDING ) ) {
      send( "The blast throws you from your mount!\n\r", victim );
      fsend_seen( victim, "%s is thrown from his mount by the blast.",
        victim );
      victim->mount->rider = NULL;
      victim->mount = NULL;
    return FALSE;
    }   

  i = number_range( 0, 20 );

  if( number_range( 0, SIZE_HORSE ) > victim->Size( )
    && i < 6 && victim->Can_Move( i ) ) {
    send( victim, "The blast throws you from the room!\n\r" );
    fsend_seen( victim, "The blast throws %s from the room!", victim );
    dest = victim->in_room->exit[i]->to_room;
    char_from_room( victim );
    char_to_room( victim, dest );
    send( "\n\r", victim );
    do_look( victim, "");
    }
  */

  return FALSE;
}
















