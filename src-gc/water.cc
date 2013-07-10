#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


bool water_logged( room_data* room )
{
  if(  room->sector_type == SECT_WATER_SURFACE 
    || room->sector_type == SECT_UNDERWATER   
    || room->sector_type == SECT_RIVER 
    || room->sector_type == SECT_SHALLOWS )  
    return TRUE;

  return FALSE;
}


bool deep_water( room_data* room )
{
  if(  room->sector_type == SECT_WATER_SURFACE 
    || room->sector_type == SECT_UNDERWATER   
    || room->sector_type == SECT_RIVER ) 
    return TRUE;

  return FALSE;
}


bool is_submerged( char_data* ch )
{
  room_data* room;

  if( ( room = Room( ch->array->where ) ) == NULL )
    return FALSE;

  if( room->sector_type == SECT_UNDERWATER )
    return TRUE;

  if( ( room->sector_type == SECT_WATER_SURFACE 
    || room->sector_type == SECT_RIVER )
    && !ch->can_fly( ) && !ch->can_float( ) )
    return TRUE;

  return FALSE;
}


void enter_water( char_data* ch )
{
  char         tmp  [ TWO_LINES ];
  char        list  [ 5 ][ ONE_LINE ];
  int        count  = 0;
  obj_data*    obj;
  room_data*  room;

  if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) ) 
    strip_affect( ch, AFF_FIRE_SHIELD );

  if( ( room = Room( ch->array->where ) ) != NULL
    && room->sector_type == SECT_UNDERWATER 
    && ( ch->species == NULL
    || !is_set( ch->species->affected_by, AFF_WATER_BREATHING ) ) )
    add_queue( new event_data( execute_drown, ch ),
      number_range( 50, 75 ) );

  /*
  for( obj = get_eq_char( ch, WEAR_LIGHT ); obj != NULL;
    obj = obj->next_list ) {
    if( !is_set( obj->extra_flags, OFLAG_WATER_PROOF ) ) {
      sprintf( list[count++], obj->Name( ch ) );
      unequip_char( ch, obj );
      }
    }  
  */

  if( count == 0 )
    return;

  *tmp = '\0';
  for( int i = 0; i < count; i++ ) 
    sprintf( tmp+strlen( tmp ), "%s%s",
      i == 0 ? "" : ( i == count-1 ? "and " : ", " ), list[i] );

  send( ch, "As you enter the water %s you are carrying %s quickly\
 extinguished.\n\r", tmp, count == 1 ? "is" : "are" );
}


/*
 *   WATER BASED SPELLS
 */


bool spell_water_breathing( char_data* ch, char_data* victim, void*, 
  int level, int duration )
{

  spell_affect( ch, victim, level, duration, SPELL_WATER_BREATHING,
    AFF_WATER_BREATHING );

  return TRUE;
}
