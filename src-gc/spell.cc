#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool spell_amnesia( char_data *ch, char_data*, void*, int, int )
{
  cast_data*  cast;
  int            i;

  send( ch, "You feel very confused!\n\r" );
  send( ch, "Your memory seems to have gone blank!\n\r" );

  if( ch->species != NULL )
    return TRUE;

  ch->pcdata->practice = 9*total_pracs( ch )/10;

  for( i = 0; i < MAX_SKILL; i++ )
    ch->shdata->skill[i] = 0;

  ch->pcdata->speaking = LANG_HUMANIC+ch->shdata->race;
  ch->shdata->skill[ LANG_HUMANIC+ch->shdata->race ] = 10;
  ch->shdata->skill[ LANG_PRIMAL ] = 10; 

  for( ; ; ) {
    if( ( cast = ch->prepare ) == NULL )
      break;
    ch->prepare = cast->next;
    delete cast;
    }

  return TRUE;
}


bool spell_magic_mapping( char_data* ch, char_data*, void*, int, int )
{
  show_map( ch, 60, 20 );

  return TRUE;
}


bool spell_youth( char_data* ch, char_data* victim, void* vo, int,
  int duration )
{
  player_data*  pc;
  obj_data*        obj  = (obj_data*) vo;

  if( duration == -4 )
    return TRUE;

  if( duration == -3 ) {
    if( obj->age > 0 ) {
      send( *ch->array, "%s appears less deteriorated by age.\n\r", obj );
      obj->age = max( 0, obj->age-10 );
      }
    return TRUE;
    }

  if( ( pc = player( victim ) ) == NULL )
    return TRUE;

  if( pc->Age( ) > plyr_race_table[
    victim->shdata->race ].start_age ) {
    pc->base_age--;
    send( victim, "You feel younger!\n\r" );
    }

  return TRUE;
} 

