#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


void condition_update( char_data* ch )
{
  int amount;
 
  gain_condition( ch, COND_FULL, ch->shdata->race == RACE_TROLL ? -2 : -1 );

  amount = ( ch->shdata->race == RACE_LIZARD ? number_range( -1, -2 ) : -1 );
  if( ch->in_room != NULL && ch->in_room->sector_type == SECT_DESERT )
    amount *= 2;
  gain_condition( ch, COND_THIRST, amount );

  amount = ( 3+ch->pcdata->condition[COND_ALCOHOL] )/4;
  ch->pcdata->condition[COND_ALCOHOL] -= amount;
  gain_drunk( ch, amount-1 );
}


/*
 *   DRUNK ROUTINES
 */


const char* drunk_inc [] = {
  "",
  "You feel fairly intoxicated.",
  "You stumble and realize you are roaring drunk.",
  "You feel queasy and realize you drank more than you should have.",
  "You feel very ill and the world is spinning rapidly."
  };

const  char* drunk_dec [] = {
  "You feel quite sober now.",
  "You feel less intoxicated.",
  "The alcohol begins to wear off, though the walls are still moving.",
  "You stomach settles, but you doubt you can walk.",
  "The world stops spinning right and shifts to the left.",
  };


void gain_drunk( char_data *ch, int amount )
{
  int*  condition  = ch->pcdata->condition;
  int   tolerance  = 10;
  int    new_cond  = max( 0, condition[ COND_DRUNK ]+amount );
  int   new_level;
  int       level;
 
  if( new_cond == condition[ COND_DRUNK ] )
    return;
   
  new_level = min( 4, new_cond/tolerance );
  level     = min( 4, condition[ COND_DRUNK ]/tolerance );

  condition[ COND_DRUNK ] = new_cond;

  if( level == new_level )
    return;
    
  send( ch, level > new_level ? drunk_dec[new_level]
    : drunk_inc[new_level] );
  send( ch, "\n\r" );
}


/*
 *   HUNGER/THIRST ROUTINES
 */


void gain_condition( char_data* ch, int iCond, int value )
{
  char       buf  [ MAX_INPUT_LENGTH ];
  int   old_cond;
  int   new_cond;

  if( ch->species != NULL || value == 0 )
    return;
 
  old_cond = ch->pcdata->condition[ iCond ];
  new_cond = max( -50, min( old_cond+value, 50 ) );

  ch->pcdata->condition[ iCond ] = new_cond;
    
  if( value < 0 ) {
    if( iCond == COND_FULL && new_cond < 0
      && is_set( &ch->pcdata->message, MSG_HUNGER ) ) {
      sprintf( buf, "You are %s\n\r", new_cond < -20 ? ( new_cond < -30
        ? "starving to death!" : "ravenous." ) : ( new_cond < -10
        ? "hungry." : "mildly hungry." ) );
      send( buf, ch );
      }
    if( iCond == COND_THIRST && new_cond < 0 
      && is_set( &ch->pcdata->message, MSG_THIRST ) ) {
      sprintf( buf, "You%s\n\r", new_cond < -20 ? ( new_cond < -30 
        ? " are dying of thirst!" : "r throat is parched." ) : ( new_cond
        < -10 ? " are thirsty." : "r throat feels dry." ) );
      send( buf, ch );
      }
    return;
    }

  if( iCond == COND_FULL && value != 999 ) {
    if( old_cond <= 40 && new_cond > 40 )
      send( ch, "You feel full.\n\r" );
    else if( old_cond < 0 && new_cond >= 0 )
      send( ch, "You no longer feel hungry.\n\r" );
    }

  if( iCond == COND_THIRST && value != 999 ) {
    if( old_cond <= 40 && new_cond > 40 )
      send( ch, "You can drink no more.\n\r" ); 
    else if( old_cond < 0 && new_cond >= 0 )
      send( ch, "You quench your thirst.\n\r" );
    }
    
  return;
}


/*
 *   HIT/MANA/MOVE REGENERATION
 */


void regen_update( void )
{
  char_data*          ch;
  struct timeval   start;
  
  gettimeofday( &start, NULL );

  for( int i = 0; i < mob_list; i++ ) {
    ch = mob_list[i];
    if( ch->Is_Valid( ) ) {
      if( ch->position >= POS_STUNNED )
        regenerate( ch );
      if( ch->position == POS_STUNNED )
        update_pos( ch );
      }
    }
  
  for( int i = 0; i < player_list; i++ ) {
    ch = player_list[i];
    if( ch->In_Game( ) ) {
      if( ch->position >= POS_STUNNED )
        regenerate( ch );
      if( ch->position == POS_STUNNED )
        update_pos( ch );
      if( ch->pcdata->prac_timer > 0 )
        ch->pcdata->prac_timer--;
      }
    }

  pulse_time[ TIME_REGEN ] = stop_clock( start );
}


/*
 *   REGENERATE ROUTINE
 */


inline int tenth( int value )
{
  value += sign( value )*number_range( 0,9 );

  return value/10;
}


void regenerate( char_data* ch )
{
  if( ch->hit < ch->max_hit && !is_set( ch->affected_by, AFF_POISON ) ) {
    ch->hit += tenth( ch->Hit_Regen( ) );
    if( ch->position == POS_MEDITATING && ch->mana != 0
      && ch->pcdata != NULL ) {
      ch->improve_skill( SKILL_MEDITATE );
      ch->improve_skill( SKILL_TRANCE );
      }
    if( ch->hit >= ch->max_hit ) {
      ch->hit = ch->max_hit;    
      if( ch->pcdata != NULL
        && is_set( &ch->pcdata->message, MSG_MAX_HIT ) )
        send( ch, "You are now at max hitpoints.\n\r" );
      }
    update_max_move( ch );
    }

  if( ch->mana < ch->max_mana
    || ( ch->position == POS_MEDITATING && ch->hit < ch->max_hit ) ) {
    ch->mana += tenth( ch->Mana_Regen( ) );
    if( ch->mana >= ch->max_mana ) {
      ch->mana = ch->max_mana;
      if( ch->pcdata != NULL
        && is_set( &ch->pcdata->message, MSG_MAX_ENERGY ) )
        send( ch, "You are now at max energy.\n\r" );
      }
    }

  if( ch->move < ch->max_move ) {
    ch->move += tenth( ch->Move_Regen( ) );
    if( ch->move >= ch->max_move ) {
      ch->move = ch->max_move;
      if( ch->hit == ch->max_hit && ch->pcdata != NULL
        && ( ch->position != POS_STANDING || ch->mount != NULL )
        && is_set( &ch->pcdata->message, MSG_MAX_MOVE ) )
        send( ch, "You are now at max movement.\n\r" );
      }
    }

  if( ch->mana < 0 ) {
    ch->mana = 0; 
    if( !is_empty( ch->leech_list ) ) {
      if( ch != ch->leech_list[0]->victim ) {
        fsend( ch,
          "You are unable to continue supporting the %s affect on %s.",
          aff_char_table[ch->leech_list[0]->type].name,
          ch->leech_list[0]->victim );
        fsend( ch->leech_list[0]->victim,
          "The energy supply to the %s affect on you is cut.",
          aff_char_table[ch->leech_list[0]->type].name );
        }
      else {
        fsend( ch, "You are unable to support the %s affect on yourself",
          aff_char_table[ch->leech_list[0]->type].name );
        }
      remove_leech( ch->leech_list[0] );
      }
    }

  return;
}


/*
 *   REGEN FUNCTIONS
 */


int Char_Data :: Hit_Regen( )
{
  int     full;
  int   thirst;
  int    regen  = 5+max_hit/8;

  if( shdata->race < MAX_PLYR_RACE ) 
    regen += plyr_race_table[shdata->race].hp_bonus;

  if( species == NULL ) {
    regen += clss_table[pcdata->clss].hit_bonus;

    if( ( full = pcdata->condition[ COND_FULL ] ) < 0 )
      regen = 10*regen/(10-full);

    if( ( thirst = pcdata->condition[ COND_THIRST ] ) < 0 )
      regen = 10*regen/(10-thirst);

    if( IS_DRUNK( this ) ) 
      regen += 3;

    if( position == POS_MEDITATING && mana != 0 ) {
      regen += regen*( shdata->skill[ SKILL_MEDITATE ]
        +shdata->skill[ SKILL_TRANCE ] )/15;
      }
    }      
  else {
    if( is_set( &species->act_flags, ACT_ZERO_REGEN ) ) 
      return 0;

    if( leader == NULL ) 
      regen *= 2;

    regen = min( regen, max_hit/10 );
    }

  if( position == POS_SLEEPING || position == POS_MEDITATING )
    regen *= 2;
  else if( position == POS_RESTING )
    regen = 3*regen/2;

  if( is_set( affected_by, AFF_REGENERATION ) )
    regen = 5*regen/4;

  if( is_set( affected_by, AFF_POISON ) ) 
    regen /= 4;

  regen += hit_regen;

  return regen/2;
}


int Char_Data :: Mana_Regen( )
{
  int     mana;
  int    regen;
  int     full;
  int   thirst;

  if( position == POS_MEDITATING && hit < max_hit )
    return -leech_regen( this )-60+mana_regen;

  mana  = base_mana+mod_mana+shdata->level*Intelligence( )/4;
  regen = 5+mana/12;

  if( shdata->race < MAX_PLYR_RACE )
    regen += plyr_race_table[shdata->race].mana_bonus;
  
  if( species == NULL ) {
    regen += clss_table[pcdata->clss].mana_bonus;
 
    if( ( full = pcdata->condition[ COND_FULL ] ) < 0 )
      regen = 10*regen/(10-full);

    if( ( thirst = pcdata->condition[ COND_THIRST ] ) < 0 )
      regen = 10*regen/(10-thirst);
    }

  if( position == POS_SLEEPING ) 
    regen *= 2;
  else if( position == POS_RESTING )
    regen = 3*regen/2; 

  regen += mana_regen-leech_regen( this );
   
  return regen;
}


int Char_Data :: Move_Regen( )
{
  int    regen  = 10+Dexterity( );
  int     full;
  int   thirst;

  if( shdata->race < MAX_PLYR_RACE ) 
    regen += plyr_race_table[shdata->race].move_bonus;

  if( species == NULL ) {
    full   = pcdata->condition[ COND_FULL ];
    thirst = pcdata->condition[ COND_THIRST ];
    regen += clss_table[pcdata->clss].move_bonus;

    if( full < 0 ) 
      regen = 10*regen/(10-full);

    if( thirst < 0 )
      regen = 10*regen/(10-thirst);

    if( IS_DRUNK( this ) )
      regen += 3;
    }

  switch( position ) {
   case POS_SLEEPING:
    regen *= 2;
    break;

   case POS_RESTING:
    regen = 3*regen/2;
    break;
    }

  if( is_set( affected_by, AFF_POISON ) ) 
    regen /= 2;

  regen += move_regen;

  return regen;
}




