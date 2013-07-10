#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


affect_data* find_leech( char_data* ch, char* argument )
{ 
  int i;

  if( number_arg( argument, i ) ) {
    if( --i < 0 || i >= ch->leech_list ) {
      send( ch, 
        "You are not supporting any affect with that number.\n\r" );
      return NULL;
      }
    return ch->leech_list[i];
    }

  for( i = 0; i < ch->leech_list; i++ ) 
    if( fmatches( argument, aff_char_table[ ch->leech_list[i]->type ].name ) ) 
      return ch->leech_list[i];

  send( ch, "You are not supporting any such leech.\n\r" );
  return NULL;
}   


void do_leech( char_data* ch, char* argument )
{
  char             tmp  [ TWO_LINES ];
  affect_data*  affect;
  cast_data*      cast;
  int             mana;
  int         max_mana;
  int            regen;
  int        max_regen;
  int          prepare;
  int                i;

  if( *argument != '\0' ) {
    if( ( affect = find_leech( ch, argument ) ) == NULL )
      return;
    if( ch != affect->victim ) 
      fsend( ch,
        "You no longer leech %s on %s.",
          aff_char_table[affect->type].name, affect->victim->Name( ) );
    fsend( affect->victim, "The energy leech supplying %s on you is cut.",
      aff_char_table[affect->type].name );
    remove_leech( affect );
    return;
    }
 
  if( is_empty( ch->leech_list ) ) {
    send( ch, "You are not supporting any spells.\n\r" );
    return;
    }

  max_regen = ch->Mana_Regen( );
  max_mana  = ch->max_mana;
 
  mana  = max_mana;
  regen = max_regen;

  prepare = 0;
  for( cast = ch->prepare; cast != NULL; cast = cast->next )
    prepare += cast->mana*cast->times;
  max_mana += prepare;

  for( i = 0; i < ch->leech_list; i++ ) {
    affect = ch->leech_list[i];
    max_regen += affect->leech_regen;
    max_mana  += affect->leech_max;
    }

  if( prepare > 0 )
    sprintf( tmp, " ( %d used by prepare )", prepare );

  page( ch, "   Max Mana: %4d (%d max)%s\n\r", mana, max_mana,
    prepare > 0 ? tmp : "" );
  page( ch, "Regen. Rate: %4.1f (%.1f max)\n\r\n\r",
    float( regen/10. ), float( max_regen/10. ) );

  page_underlined( ch,
    "Num  Spell                Regen     Max     Character\n\r" );
 
  for( i = 0; i < ch->leech_list; i++ ) {
    affect = ch->leech_list[i];
    page( ch, "%3d  %-21s%5.1f%8d     %s\n\r",
      i+1, capitalize_words( aff_char_table[affect->type].name ),
      float( affect->leech_regen/10. ), affect->leech_max, 
      affect->victim == ch ? "[self]" : affect->victim->Name( ) );
    }

  page( ch, "\n\r[ Regen is how much supporting the spell lowers your mana regen rate\n\r  and Max is how much it lowers your maximum mana amount. ]\n\r" );

  return;
}


void sprintf_leech( char* tmp, int value )
{
  dice_data dice;

  dice = value;

  if( dice.side != 0 ) { 
    if( dice.plus == 1 ) {
      if( dice.side == 1 )
        sprintf( tmp, "%d-level", dice.number );
      else
        sprintf( tmp, "%d-%d*level", dice.number, dice.side );
      }
    else {
      if( dice.side == 1 ) 
        sprintf( tmp, "%d-level/%d", dice.number, dice.plus );
      else
        sprintf( tmp, "%d-%d*level/%d", dice.number, dice.side, dice.plus );
      }
    }
  else {
    sprintf( tmp, "%d", dice.number );
    }

  return;
} 


/*
 *   REMOVE
 */


void remove_leech( affect_data* affect )
{
  char_data*    source;
 
  if( ( source = affect->leech ) == NULL )
    return;

  source->leech_list -= affect;
  affect->leech = NULL;
  
  update_max_mana( source );

  return;
}


void remove_leech( char_data* ch )
{
  int i;

  for( i = 0; i < ch->leech_list; i++ ) 
    ch->leech_list[i]->leech = NULL;
 
  clear( ch->leech_list ); 
  update_max_mana( ch );

  return;
}
 





