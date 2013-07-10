#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool can_assist( char_data* ch, char_data* victim )
{
  if( ch == NULL || victim == NULL || ch->species != NULL )
    return TRUE;

  if( ch->pcdata->clss != CLSS_CLERIC
    && ch->pcdata->clss != CLSS_PALADIN )
    return TRUE;

  if( abs( ch->shdata->alignment%3-victim->shdata->alignment%3 ) != 2 )
    return TRUE;

  send( ch, "You feel the energy drain from you, but for some reason the\
 spirits refuse\n\ryour request.\n\r" );

  return FALSE;
}


/*
 *   HEALING SPELLS
 */


void heal_victim( char_data* ch, char_data* victim, int hp )
{
  const char *heal_message[] = {
    "You were not wounded so were not healed.\n\r",
    "$n was not wounded so was not healed.",

    "You are completely healed.\n\r",
    "$n is completely healed.",

    "Most of your wounds are gone.\n\r",
    "Most of $n's wounds disappear.",

    "Your wounds feel quite a bit better.\n\r",
    "$n looks quite a bit less injured.",

    "You are slightly healed.\n\r",
    "$n looks a little bit better."
    };

  int damage = victim->max_hit-victim->hit;
  int i;

  if( !can_assist( ch, victim ) )
    return;

  victim->hit = min( victim->max_hit, victim->hit+hp );

  i = ( damage <= hp ? ( damage <= 0 ? 0 : 1 ) : ( 2*damage < 3*hp
    ? 2 : ( damage < 3*hp ? 3 : 4 ) ) );
 
  send( victim, heal_message[2*i] );
  act_notchar( heal_message[2*i+1], victim );

  update_pos( victim );
  update_max_move( victim );

  return;
}


bool spell_cure_light( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_LIGHT, level ) );

  return TRUE;
}


bool spell_cure_serious( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_SERIOUS, level ) );

  return TRUE;
}


bool spell_group_serious( char_data* ch, char_data*, void*,
  int level, int )
{
  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = player( ch->array->list[i] ) ) != NULL
      && is_same_group( gch, ch ) )
      heal_victim( ch, gch, spell_damage( SPELL_GROUP_SERIOUS, level ) );

  return TRUE;
}


bool spell_cure_critical( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_CURE_CRITICAL, level ) );

  return TRUE;
}



bool spell_group_critical( char_data* ch, char_data*, void*,
  int level, int )
{
  char_data* gch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( gch = player( ch->array->list[i] ) ) != NULL
      && is_same_group( gch, ch ) )
      heal_victim( ch, gch, spell_damage( SPELL_GROUP_CRITICAL, level ) );

  return TRUE;

}

bool spell_heal( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_HEAL, level ) );

  return TRUE;
}

bool spell_restoration( char_data* ch, char_data* victim, void*,
  int level, int )
{
  heal_victim( ch, victim, spell_damage( SPELL_RESTORATION, level ) );

  return TRUE;
}


/*
 *   REMOVE CURSE
 */


bool spell_remove_curse( char_data* ch, char_data* victim, void*,
  int level, int )
{
  obj_data*   obj;
  int       count  = 0;

  if( !consenting( victim, ch, "curse removal" ) ) 
    return TRUE;

  for( int i = victim->wearing.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->wearing[i] ) ) == NULL
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) ) 
      continue;

    if( count++ > level/5 ) 
      return TRUE;

    send( victim, "%s which you are wearing turns to dust.\n\r", 
      obj );
    send_seen( victim, "%s which %s is wearing turns to dust.\n\r",
      obj, victim );
    obj->Extract( );
    }

  for( int i = victim->contents.size-1; i >= 0; i-- ) {
    if( ( obj = object( victim->contents[i] ) ) == NULL
      || ( !is_set( obj->extra_flags, OFLAG_NODROP )
      && ( !is_set( obj->extra_flags, OFLAG_NOREMOVE ) ) ) ) 
      continue;

    if( count++ > level/5 ) 
      return TRUE;

    send( victim, "%s which you are carrying turns to dust.\n\r", obj );
    send_seen( victim, "%s which %s is carrying turns to dust.\n\r",
      obj, victim );
    obj->Extract( );
    }

  if( count == 0 ) 
    if( victim == ch ) 
      send( ch, "You weren't carrying anything cursed.\n\r" );
    else 
      send( ch, "%s wasn't carrying anything cursed.\n\r", victim );

  return TRUE;
}


/*
 *   POISON/DISEASE
 */


bool spell_cure_disease( char_data* ch, char_data* victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_PLAGUE ) ) {
    if( ch != victim )
      send( ch, "%s wasn't diseased.\n\r", victim );
    return TRUE;
    }

  strip_affect( victim, AFF_PLAGUE );

  if( is_set( victim->affected_by, AFF_PLAGUE ) ) {
    send( ch, "%s is still diseased!?\n\r", victim );
    send( victim, "You are still diseased!?" );
    }

  return TRUE;
}


bool spell_cure_poison( char_data* ch, char_data* victim, void*, int, int )
{
  bool is_drunk;

  is_drunk = ( victim->species == NULL &&
    ( victim->pcdata->condition[COND_ALCOHOL] > 0
    || victim->pcdata->condition[COND_DRUNK] > 0 ) );

  if( !is_drunk && !is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch != victim )
      send( ch, "%s wasn't poisoned.\n\r", victim );
    return TRUE;
    }

  if( victim->pcdata != NULL ) {
    victim->pcdata->condition[ COND_ALCOHOL ] = 0;
    victim->pcdata->condition[ COND_DRUNK ]   = 0;
    }

  strip_affect( victim, AFF_POISON );

  if( is_set( victim->affected_by, AFF_POISON ) ) {
    if( ch != victim ) 
      send( ch, "%s is still poisoned!?\n\r", victim );
    send( victim, "You are still poisoned!?\n\r" );
    }

  return TRUE;
}


/*
 *   SILENCE SPELL
 */


bool spell_gift_of_tongues( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_GIFT_OF_TONGUES,
    AFF_TONGUES );

  return TRUE;
}



bool spell_silence( char_data* ch, char_data* victim, void*, int level, int duration )
{
  if( makes_save( victim, ch, RES_MAGIC, SPELL_SILENCE, level ) )
    return TRUE;

  spell_affect( ch, victim, level, duration, SPELL_SILENCE, AFF_SILENCE );

  return TRUE;
}


bool spell_augury( char_data* ch, char_data*, void* vo, int, int )
{
  obj_data*  obj  = (obj_data*) vo;

  if( ch == NULL || obj == NULL )
    return TRUE;

  if( !obj->Belongs( ch ) ) {
    fsend( ch, "%s glows black and you sense the true owner is %s.",
      obj, obj->owner->name );
    }
  else {
    send( ch, "Nothing happens.\n\r" );
    }

  return TRUE;
}


bool spell_true_sight( char_data* ch, char_data* victim, void*, 
  int level, int duration )
{

  spell_affect( ch, victim, level, duration, SPELL_TRUE_SIGHT,
    AFF_TRUE_SIGHT );

  return TRUE;
}


bool spell_protect_life( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_LIFE,
    AFF_LIFE_SAVING ); 

  return TRUE;
}


bool spell_sense_life( char_data* ch, char_data* victim, void*, 
  int level, int duration )
{

  spell_affect( ch, victim, level, duration, SPELL_SENSE_LIFE,
    AFF_SENSE_LIFE );

  return TRUE;

}


/*
 *   CREATION SPELLS
 */


bool spell_create_water( char_data* ch, char_data* victim, void* vo,
  int level, int duration )
{
  obj_data*         obj  = (obj_data*) vo;
  content_array*  where;
  int            liquid;
  int             metal;

  /* DIP */

  if( duration == -3 ) {
    if( ( metal = obj->metal( ) ) != 0 && number_range( 0,3 ) == 0 ) {
      if( obj->rust == 3 ) {
        send( *ch->array, "%s disintegrates into worthless pieces.\n\r", obj );
        obj->Extract( 1 );
        } 
      else {
        send( *ch->array, "%s %s.\n\r",
          obj, material_table[ metal ].rust_name );
        obj->rust++;
        }  
      }
    return FALSE;
    }      

  /* THROWING, REACTING */ 

  if( ch == NULL || victim != NULL )
    return FALSE;

  /* CAST */

  liquid = LIQ_WATER;
  where  = obj->array;

  if( number_range( 0, 100 ) < 2 ) {
    for( ; ; ) {
      if( ( liquid = number_range( 0, MAX_ENTRY_LIQUID-1 ) ) == LIQ_WATER
        || liquid_table[ liquid ].create )
        break;
      }
    }

  obj = (obj_data*) obj->From( 1 );

  obj->value[1] += 100*level;
  obj->value[2]  = liquid;
  obj->selected  = 1;

  include_empty  = FALSE;
  include_liquid = FALSE;

  if( obj->value[1] >= obj->value[0] ) {
    send( *ch->array, "%s fills to overflowing with %s.\n\r",
      obj, liquid_table[ liquid ].name );
    obj->value[1] = obj->value[0];
    }
  else {
    send( *ch->array, "%s fills partially up with %s.\n\r",
      obj, liquid_table[ liquid ].name );
    }

  include_empty  = TRUE;
  include_liquid = TRUE;

  set_bit( obj->extra_flags, OFLAG_KNOWN_LIQUID );
  obj->To( where );
  react_filled( ch, obj, liquid );

  return TRUE;    
}


/*
 *   OBJECT SPELLS
 */

bool spell_purify( char_data* ch, char_data* victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_SLEEP )  &&
    !is_set( victim->affected_by, AFF_SLOW ) &&
    !is_set( victim->affected_by, AFF_CURSE ) ) {
    if( ch != victim )
      send( ch, "%s is not affected by anything which can be purified.\n\r",
        victim );
    else
      send( victim, 
        "You are not affected by anything which can be purified.\n\r" );
    return TRUE;
    }

  if( is_set( victim->affected_by, AFF_SLEEP ) ) {
    if( ch != victim )
      send( ch, "%s is no longer affected by magical sleep.\n\r", victim );
    else
      send( victim, "You are no longer affected by magical sleep.\n\r" );
    strip_affect( victim, AFF_SLEEP );
    }

  if( is_set( victim->affected_by, AFF_CURSE ) ) {
    if( ch != victim )
      send( ch, "%s is no longer cursed.\n\r", victim );
    else
      send( victim, "You are no longer cursed.\n\r" );
    strip_affect( victim, AFF_CURSE );
    }

  if( is_set( victim->affected_by, AFF_SLOW ) ) {
    if( ch != victim )
      send( ch, "%s is no longer slowed.\n\r", victim );
    else
      send( victim, "You are no longer slowed.\n\r" );

    strip_affect( victim, AFF_SLOW );
    }

  return TRUE;
}


bool spell_sanctify( char_data* ch, char_data*, void *vo, int level, int )
{
  obj_data *obj = (obj_data*) vo;
  int dam;

  dam = obj->value[0]*(obj->value[1]+1);

  if( is_set( obj->extra_flags, OFLAG_SANCT ) ) {
    send( ch, "Nothing happens.\n\r" );
    return TRUE;
    }

  if( dam > 3*level ) {
    fsend( *ch->array,
      "%s glows with a pale blue light which quickly fades.", 
      obj );
    return TRUE;
    }

  fsend( *ch->array, "%s glows with a pale blue light.\n\rThis\
 slowly fades as if being absorbed into the item.", obj );

  set_bit( obj->extra_flags, OFLAG_SANCT );

  return TRUE;
};


/*
 *   PROTECTION SPELLS
 */


bool spell_armor( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_ARMOR, AFF_ARMOR );

  return TRUE;
}


bool spell_bless( char_data* ch, char_data* victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_BLESS, AFF_BLESS );

  return TRUE;
}


bool spell_protect_good( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_GOOD,
    AFF_PROTECT_GOOD );

  return TRUE;
}


bool spell_protect_evil( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROTECT_EVIL,
    AFF_PROTECT_EVIL );

  return TRUE;
}


/*
 *   WRATH
 */


bool spell_holy_wrath( char_data *ch, char_data *victim, void*, int level,
  int duration )
{
  if( ch != NULL
    && ch->shdata->alignment%3 != victim->shdata->alignment%3 ) {
    send( ch, "You are unable to incite %s into a holy rage.\n\r", victim );
    send( victim, "%s is unable to incite you into a holy rage.\n\r", ch );
    return TRUE;
    }

  spell_affect( ch, victim, level, duration, SPELL_HOLY_WRATH, AFF_WRATH );

  return TRUE;
}


/*
 *   GOLEM SPELLS
 */


bool spell_animate_clay( char_data* ch, char_data*, void*, int, int )
{
  char_data*       golem;
  species_data*  species;

  if( null_caster( ch, SPELL_ANIMATE_CLAY ) )
    return TRUE;

  for( int i = 0; i < ch->followers; i++ ) { 
    golem = ch->followers[i];
    if( is_set( &golem->status, STAT_PET ) 
      && golem->shdata->race == RACE_GOLEM ) {
      send( ch, "The incarnation fails as you can only control one golem\
 at a time.\n\r" );
      return TRUE;
      }
    }

  if( ( species = get_species( MOB_CLAY_GOLEM ) ) == NULL ) {
    bug( "Animate_Clay: NULL species." );
    return TRUE;
    }

  golem = create_mobile( species );

  golem->To( ch->array );
  add_follower( golem, ch );
  set_bit( &golem->status, STAT_PET );

  return TRUE;
}


/*
 *   DEATH
 */


bool spell_resurrect( char_data*, char_data*, void*, int, int )
{
  return TRUE;
}


/*
 *   FEAR
 */


bool spell_fear( char_data* ch, char_data* victim, void*, int level, int )
{
  /*
  if( ch == NULL ) {
    send( victim, "Tell merior she forgot about fear potions.\n\r" );
    return TRUE; 
    }
  */

  if( makes_save( victim, ch, RES_MAGIC, SPELL_FEAR, level ) ) {
    send( ch, "You are unable to make %s afraid!\n\r", victim );
    send( victim, "%s is unable to make you afraid!\n\r", ch );
    send( *ch->array, "%s is unable to make %s afraid.\n\r",
      ch, victim );
    return TRUE;
    }

  send( victim, "%s causes you to cringe in fear!\n\r", ch );
  send( *ch->array, "%s cringes in fear!\n\r", victim );
 
  disrupt_spell( victim );
  set_delay( victim, 40+level );

  return TRUE;
}


