#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *   ANIMATE DEAD SPELLS
 */


bool can_animate( char_data* ch, species_data* species,
  species_data* undead )
{
  if( species->shdata->race == RACE_UNDEAD ) {
    send( ch, "That corpse has died twice already - it is too destroyed to\
 be animated\n\ragain.\n\r" );
    return FALSE;
    }

  if( undead == NULL ) {   
    send( ch, "You fail to animate the corpse as it does not seem to be from\
 a species that\n\ryou can resurrect.\n\r" );
    return FALSE;
    }
  
  if( undead->shdata->level > ch->shdata->level/2 ) {
    send( ch, "You are not powerful enough to control the spirit of that\
 creature.\n\r" );
    return FALSE;
    }

  if( undead->shdata->level + pet_levels( ch ) > 
      ch->shdata->level ) {
    send( ch, "Controlling that spirit in conjunction with the undead you\
 have already\n\rraised is beyond your abilities.\n\r" );
    return FALSE;
    }

  return TRUE;
}


bool spell_animate_dead( char_data* ch, char_data*, void* vo,
  int level, int )
{
  species_data*    zombie;
  species_data*   species;
  char_data*      remains;
  obj_data*        corpse  = (obj_data*) vo;

  if(  null_caster( ch,     SPELL_ANIMATE_DEAD )
    || null_corpse( corpse, SPELL_ANIMATE_DEAD ) )
    return TRUE;

  if( ( species = get_species( corpse->value[1] ) ) == NULL ) {
    bug( "Animate Dead: illegal corpse species." );
    return TRUE;
    }

  if( !can_animate( ch, species, zombie = get_species( species->zombie ) ) )
    return TRUE;

  remains = create_mobile( zombie );
  corpse->Extract( 1 );

  set_bit( &remains->status, STAT_PET );
  set_bit( &remains->status, STAT_TAMED );
  remains->To( ch->array );
  add_follower( remains, ch, FALSE );

  send( ch, "%s slowly stands up and starts to follow you.\n\r", remains );
  fsend_seen( remains, "%s slowly stands up and starts to follow %s.",
    remains, ch );

  return TRUE;
}


bool spell_greater_animation( char_data* ch, char_data*, void* vo,
  int level, int )
{
  species_data*    species;
  species_data*   skeleton; 
  char_data*       remains;
  obj_data*         corpse  = (obj_data*) vo;

  if(  null_caster( ch,     SPELL_GREATER_ANIMATION )
    || null_corpse( corpse, SPELL_GREATER_ANIMATION ) )
    return TRUE;

  if( ( species = get_species( corpse->value[1] ) ) == NULL ) {
    bug( "Greater Animation: Corpse of unknown species." );
    return TRUE;
    }

  if( !is_set( &species->act_flags, ACT_HAS_SKELETON ) ) {
    fsend( ch, "%s did not contain a skeleton and so trying to animate it\
 was doomed to failure.  All that remains on the ground is a few ashes.",
      corpse );
    return TRUE;
    }

  if( !can_animate( ch, species,
    skeleton = get_species( species->skeleton ) ) )
    return TRUE;

  if( skeleton->shdata->level > ch->shdata->level-pet_levels( ch ) ) {
    send( ch, "You can not control that many pets.\n\r" );
    return TRUE;
  }

  remains = create_mobile( skeleton );
  corpse->Extract( 1 );

  set_bit( &remains->status, STAT_PET );
  set_bit( &remains->status, STAT_TAMED );
  remains->To( ch->array );
  add_follower( remains, ch );

  fsend( ch, "The skeleton of %s slowly rises from the ground and starts\
 to follow you.", species->descr->name );
  fsend_seen( remains, "The skeleton of %s slowly rises from the ground\
 and starts to follow %s.", species->descr->name, ch );

  return TRUE;
}


/*
 *   BANISHMENT SPELLS 
 */


bool spell_turn_undead( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( victim->shdata->race != RACE_UNDEAD ) 
    return TRUE;
  
  damage_physical( victim, ch, spell_damage( SPELL_TURN_UNDEAD, level ),
    "*the enchantment" );

  return TRUE;
}


bool spell_banishment( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( victim->shdata->race != RACE_UNDEAD ) 
    return TRUE;
  
  damage_physical( victim, ch, spell_damage( SPELL_BANISHMENT, level ),
    "*the enchantment" );

  return TRUE;
}

