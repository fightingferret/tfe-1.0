#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


const char* level_name( int i )
{
  char* tmp = static_string( );

  if( i < LEVEL_AVATAR ) 
    sprintf( tmp, "level %d", i );
  else 
    sprintf( tmp, "a%s %s",
      isvowel( *imm_title[ i-LEVEL_AVATAR ] ) ? "n" : "",
      imm_title[ i-LEVEL_AVATAR ] );

  return tmp;
}


void do_level( char_data* ch, char* argument )
{
  int i;

  if( is_confused_pet( ch ) )
    return;

  if( *argument != '\0' ) {
    if( !number_arg( argument, i ) ) 
      send( ch, "Unknown syntax - see help level.\n\r" );
    else if( i > LEVEL_HERO ) 
      send( ch, "Gaining levels beyond %d via exp is impossible.\n\r",
        LEVEL_HERO );
    else if( i < 2 ) 
      send( ch, "Levels below 2 do not require exp.\n\r" );
    else
      send( ch, "Gaining level %s costs %d exp.\n\r", 
        number_word( i, ch ), exp_for_level( i-1 ) );
    return;
    }    

  if( is_mob( ch ) ) 
    return;

  if( ch->shdata->level >= LEVEL_AVATAR ) {
    send( ch, "You are level %d.\n\r", ch->shdata->level );
    return;
    } 

  send( ch, "You need %d exp and %d quest points to level.\n\r",
    exp_for_level( ch )-ch->exp, 0 );

  send( ch, "[ You have acquired %d exp points so far. ]\n\r",
    ch->exp );
}


void advance_level( char_data* ch, bool message )
{
  char        buf  [ MAX_STRING_LENGTH ];
  int        clss  = ch->pcdata->clss;
  int      add_hp;
  int    add_mana;
  int    add_move;
  int       pracs  = 0;
  bool     regain;

  if( ch->species != NULL )
    return;

  ch->shdata->level++;

  regain = ( ch->pcdata->max_level != -1 );

  if( message ) {
    send( ch, "\n\r-=-= CONGRATS!!! =-=-\n\r\n\r" );
    send( ch, "You have %sgained level %d.\n\r",
      regain ? "re" : "",  ch->shdata->level );
    send( ch, "You need %d exp for the next level.\n\r",
      exp_for_level( ch )-ch->exp );

    sprintf( buf, "%s has %sgained level %d.", ch->descr->name,
      regain ? "re" : "", ch->shdata->level );
    info( buf, LEVEL_BUILDER, buf, IFLAG_LEVELS, regain ? 2 : 1, ch );
    }

  if( ch->pcdata->max_level == ch->shdata->level ) {
    ch->pcdata->max_level  = -1;
    ch->base_hit           = ch->pcdata->level_hit;
    ch->base_mana          = ch->pcdata->level_mana;
    ch->base_move          = ch->pcdata->level_move;
    }
  else {
    add_hp  = number_range( clss_table[ clss ].hit_min,
      clss_table[ clss ].hit_max );
    add_mana = number_range( clss_table[ clss ].mana_min,
      clss_table[ clss ].mana_max );
    add_move = number_range( clss_table[ clss ].move_min,
      clss_table[ clss ].move_max );

    ch->base_hit   += add_hp;
    ch->base_mana  += add_mana;
    ch->base_move  += add_move;

    if( ch->pcdata->max_level == -1 ) {      
      if( ( pracs = 12-ch->shdata->level ) > 0 ) {
        if( message )
          send( ch, "\n\r>> You gain %d practice point%s. <<\n\r\n\r",
            pracs, pracs == 1 ? "" : "s" );
        ch->pcdata->practice += pracs;
        }
      remove_bit( ch->pcdata->pfile->flags, PLR_FAMILIAR );
      }
    } 

  if( message ) {
    sprintf( buf, "%sained level %d.",
      regain ? "Reg" : "G", ch->shdata->level );
    player_log( ch, buf );
    }

  ch->pcdata->pfile->level = ch->shdata->level;
  calc_resist( ch );
  update_maxes( ch );
}   


void lose_level( char_data* ch )
{
  char       tmp  [ ONE_LINE ];
  int     add_hp;
  int   add_mana;
  int   add_move;

  if( ch->pcdata == NULL || ch->shdata->level < 2 )
    return;

  if( ch->pcdata->max_level == -1 ) {
    ch->pcdata->max_level  = ch->shdata->level;
    ch->pcdata->level_hit  = ch->base_hit;
    ch->pcdata->level_mana = ch->base_mana;
    ch->pcdata->level_move = ch->base_move;
    }

  ch->shdata->level--;
  ch->pcdata->pfile->level = ch->shdata->level;

  if( ch->shdata->level < LEVEL_BUILDER ) {
    sprintf( tmp, "%s has lost a level.", ch->descr->name );
    info( tmp, LEVEL_BUILDER, tmp, IFLAG_LEVELS, 2, ch );
    }

  add_hp   = ( ch->base_hit-20 )/( ch->shdata->level );
  add_mana = ( ch->base_mana-50 )/( ch->shdata->level );
  add_move = ( ch->base_move-100 )/( ch->shdata->level );

  ch->base_hit   -= add_hp;
  ch->base_mana  -= add_mana;
  ch->base_move  -= add_move;

  calc_resist( ch );
  update_maxes( ch );
 
  send( ch, "You have lost a level!\n\r" );
}   


/*
 *   EXPECTED FUNCTIONS (PLAYER) 
 */


float Player_Data :: Mean_Hp( )
{
  return 20+(shdata->level-1)*(clss_table[ pcdata->clss ].hit_min
    +clss_table[ pcdata->clss ].hit_max)/2;
}


float Player_Data :: Mean_Mana( )
{
  return 50+(shdata->level-1)*(clss_table[ pcdata->clss ].mana_min
    +clss_table[ pcdata->clss ].mana_max)/2;
}


float Player_Data :: Mean_Move( )
{
  return 100+(shdata->level-1)*(clss_table[ pcdata->clss ].move_min
    +clss_table[ pcdata->clss ].move_max)/2;
}


/*
 *   EXPECTED FUNCTIONS (MOB) 
 */


float Mob_Data :: Mean_Hp( )
{
  return 0;
}


float Mob_Data :: Mean_Mana( )
{
  return 0.;
}


float Mob_Data :: Mean_Move( )
{
  return 0.;
}



