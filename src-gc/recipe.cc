#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void do_scribe( char_data* ch, char* ) 
{
  send( ch, "Command under construction.\n\r" );

  return;
}


void do_build( char_data* ch, char* argument ) 
{
  int                  i, j;
  int                 skill;
  recipe_data*       recipe;
  obj_data*             obj;
  obj_clss_data*     ingred;
  obj_clss_data*     result;

  if( *argument == '\0' ) {
    send( ch, "What do you wish to build?\n\r" );
    return;
    }

  for( i = 0; i < MAX_ENTRY_BUILD; i++ ) 
    if( ( result = get_obj_index( build_table[i].result[0] ) ) != NULL 
      && is_name( argument, result->Keywords( ) ) )
      break;

  if( i == MAX_ENTRY_BUILD ) {
    send( "Whatever that is it isn't something you can build.\n\r", ch );
    return;
    } 

  recipe = &build_table[i];
 
  for( j = 0; j < 3; j++ )  
    if( ( skill = recipe->skill[j] ) >= 0 
      && ch->get_skill( skill ) == 0 ) {
      send( ch, "To build %s you need to know %s.\n\r",       
        result->Name( ), skill_table[skill].name );
      return;
      } 

  /*
  for( i = 0; i < 10; i++ ) {
    ingred = get_obj_index( recipe->ingredient[2*i] );
    if( ingred == NULL )
      continue;
    for( j = 0, obj = ch->contents; obj != NULL; obj = obj->next_content ) 
      if( obj->pIndexData == ingred && obj->wear_loc == -1 )
        j += obj->number;
    if( j == 0 ) {
      send( ch, "Building %s requires %s.\n\r", result->Name( ),
        ingred->Name( ) ); 
      return;
      }
    }
  */

  return;
}


