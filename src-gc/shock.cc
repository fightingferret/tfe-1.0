#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


index_data electric_index [] = 
{
  { "tingles",         "tingle",            7 },
  { "zaps",            "zap",              15 },
  { "charges",         "charge",           30 },
  { "jolts",           "jolt",             50 },
  { "SHOCKS",          "SHOCK",            75 },
  { "ELECTRIFIES",     "ELECTRIFY",       100 },
  { "* ELECTROCUTES *","* ELECTROCUTE *", 140 },
  { "* IONIZES *",     "* IONIZE *",      200 },
  { "** ATOMIZES **",  "** ATOMIZE **",    -1 }
};


void water_shock( char_data* ch, int spell, int level )
{
  char_data*  victim;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( victim = character( ch->array->list[i] ) ) != NULL
      && can_kill( ch, victim ) )
      damage_shock( victim, ch, 2*spell_damage( spell, level ),
        "*The water shock" );
    
  return;
}


bool damage_shock( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  damage *= 100-victim->Save_Shock( );
  damage /= 100;

  dam_message( victim, ch, damage, string,
    lookup( electric_index, damage, plural ) );  

  return inflict( victim, ch, damage, "shock" );
}


/* 
 *   ELECTRICITY BASED SPELLS
 */


bool spell_resist_shock( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_RESIST_SHOCK,
    AFF_RESIST_SHOCK );
  
  return TRUE;
}

bool spell_ion_shield( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_ION_SHIELD,
    AFF_ION_SHIELD );

  return TRUE;
}

bool spell_call_lightning( char_data* ch, char_data* victim, void*,
  int level, int )
{

  if( null_caster( ch, SPELL_CALL_LIGHTNING ) )
    return TRUE;

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    water_shock( ch, SPELL_CALL_LIGHTNING, level );
    return TRUE;
    }

  damage_shock( victim, ch, spell_damage( SPELL_CALL_LIGHTNING, level ),
    "*A bolt of lightning" );
  return TRUE;

}


bool spell_lightning_bolt( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_LIGHTNING_BOLT ) )
    return TRUE;

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    water_shock( ch, SPELL_LIGHTNING_BOLT, level );
    return TRUE;
    }

  damage_shock( victim, ch, spell_damage( SPELL_LIGHTNING_BOLT, level ),
    "*The brilliant bolt of lightning" );

  return TRUE;
}


bool spell_chain_lightning( char_data* ch, char_data* victim, void*,
  int level, int )
{
  room_data*  room;

  if( null_caster( ch, SPELL_CHAIN_LIGHTNING ) ) 
    return TRUE;

  room = ch->in_room;

  if( room->sector_type == SECT_UNDERWATER ) {
    water_shock( ch, SPELL_CHAIN_LIGHTNING, level );
    return TRUE;
    }
 
  for( ; victim != NULL; ) {
    damage_shock( victim, ch, spell_damage( SPELL_CHAIN_LIGHTNING, level ),
      "*The bifurcating lightning bolt" );
    if( number_range( 0, 3 ) == 0
      || ( victim = random_pers( room ) ) == ch 
      || !can_kill( ch, victim ) )
      break;
    }

  return TRUE;
}


bool spell_shock( char_data* ch, char_data* victim, void*,
  int level, int )
{
  if( null_caster( ch, SPELL_SHOCK ) )
    return TRUE;

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    water_shock( ch, SPELL_SHOCK, level );
    return TRUE;
    }

  damage_shock( victim, ch, spell_damage( SPELL_SHOCK, level ),
    "*The blue arcs of energy", TRUE );

  return TRUE;
}





