#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "math.h"
#include "define.h"
#include "struct.h"


int death_exp( char_data* victim, char_data* )
{
  return exp_for_level( victim )/5;
}


int exp_for_level( int level )
{
  int exp;

  exp  = level*(110*level*level+400*level-411);
  exp += int( pow( level*level*level*level, 1.11 ) ); 

  return exp;
}


/*
 *   EXP CALCULATION SUB-ROUTINES
 */


int base_exp( char_data* mob )
{
  species_data*  species  = mob->species;
  double              xp;
  
  if( species->shdata->deaths == 0
    || species->rounds == 0 )
    return 0;

  xp = species->damage*( mob->max_hit
    +species->damage_taken/species->shdata->deaths )/(2.*species->rounds);
  xp = int( pow( xp, 1.07 ) )+5.; 

  return (int) xp;
}


int base_exp( species_data* species )
{
  double       xp;
  dice_data  dice;

  dice = species->hitdice;
  
  if( species->shdata->deaths == 0
    || species->rounds == 0 )
    return 0;

  xp = species->damage*( average( dice )+species->damage_taken
    /species->shdata->deaths )/(2.*species->rounds);
  xp = pow( double( xp ), double( 1.07 ) )+5.;

  return (int) xp;
}


int death_exp( species_data* species, int base )
{
  float x;

  x = float( species->shdata->kills/(2+species->shdata->deaths) );

  if( x > 1.5 )
    return (int) 1.5*base;

  return (int) x*base;
}


int special_exp( species_data* species, int base )
{
  if( species->rounds == 0 )
    return 0;

  return base*species->special/species->rounds/2;
}


int level_exp( species_data* species, int base )
{
  return base*species->shdata->level/100;
}


int modify_exp( species_data* species, int base )
{
  if( is_set( &species->act_flags, ACT_AGGR_ALL ) )
    return base/10;

  return 0;
}


int modify_exp( char_data* mob, int base )
{
  if( is_set( &mob->status, STAT_AGGR_ALL ) ) 
    return base/10;

  return 0;
}


/*
 *   EXP DISPLAY FUNCTION
 */


void do_exp( char_data* ch, char* argument )
{
  dice_data         dice;
  int               base;
  int              death;
  int               spec;
  int              level;
  int              modif;
  int              total;
  species_data*  species;

  if( ( species = get_species( atoi( argument ) ) ) == NULL ) {
    send( ch, "No mob has that vnum.\n\r" );
    return;
    }

  base  = base_exp( species );
  death = death_exp( species, base );
  spec  = special_exp( species, base );
  modif = modify_exp( species, base );
  total = base+death+spec+modif;

  level  = level_exp( species, total );
  total += level;

  page_title( ch, "Exp for %s", species->Name( ) );

  if( species->shdata->deaths > 0 ) 
    page( ch, "       Ave Exp: %d\n\r",
      species->exp/species->shdata->deaths );

  dice   = species->hitdice;

  page( ch, "        Damage: %-10d Rounds: %-11d Dam/Rnd: %.2f\n\r ",
    species->damage, species->rounds,
    float( species->damage )/float( species->rounds ) ); 

  page( ch, "      Avg. Hp: %-7d Dmg_Taken: %-9d Avg_Taken: ",
    average( dice ), species->damage_taken );

  if( species->shdata->deaths == 0 )
    page( ch, "??\n\r" );
  else
    page( ch, "%d\n\r", species->damage_taken/species->shdata->deaths );

  page( ch,"         Kills: %-10d Deaths: %-13d Level: %d\n\r",
    species->shdata->deaths, species->shdata->kills,
    species->shdata->level );
  page( ch, "       Special: %-8d Spec/Rnd: %.2f\n\r\n\r",
    species->special, float( species->special )/species->rounds );
   
  page( ch, "   Base Exp: %7d\n\r", base  );
  page( ch, "  Death Exp: %7d\n\r", death );
  page( ch, "    Special: %7d\n\r", spec  );
  page( ch, "  Modifiers: %7d\n\r", modif );
  page( ch, "      Level: %7d\n\r", level );
  page( ch, "             -------\n\r" );
  page( ch, "      Total: %7d\n\r", total );

  return;
}


/*
 *   EXP COMPUTATION FUNCTION
 */

 
int xp_compute( char_data* victim )
{
  obj_data*        wield;
  species_data*  species  = victim->species;
  int                 xp;
  int             damage;

  if( victim->species != NULL ) {
    xp  = base_exp( species );
    xp += death_exp( species, xp )+special_exp( species, xp );
    xp += level_exp( species, xp );
    xp += modify_exp( victim, xp );
    }
  else {
    if( ( wield = victim->Wearing( WEAR_HELD_R ) ) != NULL 
      && wield->pIndexData->item_type == ITEM_WEAPON ) 
      damage = wield->value[1]*(wield->value[2]+1)/2;
    else
      damage = 2;  
    damage += (int) victim->Damroll( wield );
    xp = damage*victim->max_hit/2;
    xp = min( xp, victim->exp/2 );
    }

  xp = max( 0, xp );
 
  return xp;
}


/*
 *   GROUP EXP FUNCTION
 */


void gain_exp( char_data* ch, char_data* victim, int gain, const char* msg )
{
  if( ch->species != NULL )
    return;

  int level = exp_for_level( ch );

  if( gain > 0 ) 
    gain = int( gain/sqr(1.+sqrt(1.*gain/level)) );

  add_exp( ch, gain, msg );

  /* MODIFY REPUTATION */

  if( victim == NULL )
    return;

  player_data* pc  = (player_data*) ch;

  pc->reputation.alignment[ victim->shdata->alignment ] += gain;

  if( victim->species != NULL )
    pc->reputation.nation[ victim->species->nation ]++;
}


void add_exp( char_data* ch, int gain, const char* msg )
{
  if( ch->species != NULL )
    return;

  if( msg != NULL )
    send( ch, msg, abs( gain ), abs( gain ) == 1 ? "" : "s" );

  int level = exp_for_level( ch );

  ch->exp += gain;

  for( ; ch->shdata->level > 1 && ch->shdata->level <= LEVEL_HERO
    && ch->exp < 0; ) {
    lose_level( ch );
    ch->exp += exp_for_level( ch ); 
    }
 
  ch->exp = max( 0, ch->exp );
    
  if( ch->exp >= level && ch->shdata->level < LEVEL_HERO
    && ch->pcdata->quest_pts >= 0 && gain > 0 ) {
    if( !is_set( ch->pcdata->pfile->flags, PLR_APPROVED )
      && ch->shdata->level >= 5 ) {
      send( ch,
        "To gain levels past 5th you must have an approved appearance.\n\r" );
      } 
    else {
      ch->exp -= level;
      advance_level( ch, TRUE );
      }
    }

  update_score( ch ); 
}


void disburse_exp( char_data* victim )
{
  char_data*       ch;
  int      tot_damage;
  float           xpc;

  victim->shdata->deaths++;

  if( victim->pcdata != NULL )
    return;

  xpc = float( xp_compute( victim ) );
  
  if( victim->species != NULL )
    victim->species->exp += int( xpc );

  tot_damage = 0;

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( ch = player( victim->array->list[i] ) ) != NULL 
      && ch != victim ) {
      remove_bit( &ch->status, STAT_GAINED_EXP );      
      tot_damage += max( 0, damage_done( ch, victim ) );
      }
    }

  xpc = xpc/float( max( tot_damage, victim->max_hit ) );

  for( int i = 0; i < *victim->array; i++ ) 
    if( ( ch = player( victim->array->list[i] ) ) != NULL 
      && ch != victim
      && !is_set( &ch->status, STAT_GAINED_EXP ) )
      group_gain( victim, ch, xpc );
}


void group_gain( char_data* victim, char_data* ch, float xpc )
{
  player_data*      gch;
  player_array     list;
  int           members  = 0;
  int        tot_damage  = 0;
  int            damage;
  float              xp;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( gch = player( ch->array->list[i] ) ) == NULL 
      || !is_same_group( gch, ch ) )
      continue;
    damage = damage_done( gch, victim );
    if( damage >= 0 || gch->fighting != NULL ) {
      tot_damage  += damage;
      members     += cube( gch->shdata->level+5 );
      list        += gch;
      }
    set_bit( &gch->status, STAT_GAINED_EXP );
    }

  for( int i = 0; i < list; i++ ) {
    if( ( xp = float( tot_damage )*cube( list[i]->shdata->level+5 )
      *xpc/members ) > 0 ) {   
      gain_exp( list[i], victim, int( xp ),
        "You receive %d experience point%s.\n\r" );
      }
    list[i]->shdata->fame = min( list[i]->shdata->fame
      +victim->shdata->level, 1000 );
    }
}


void zero_exp( species_data* species )
{
  species->rounds         = 1;
  species->damage         = 0;
  species->damage_taken   = 0;
  species->shdata->deaths = 0;
  species->shdata->kills  = 0;
  species->exp            = 0;
  species->special        = 0;
}













