#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"
#include "unistd.h"


/*
 *   BUG ROUTINES
 */


inline void mob_bugs( char_data* ch, species_data* species, bool& found,
  bool make )
{
  mprog_data*   mprog;
  int               i  = 1;

  if( make && species->attack->binary == NULL )
    compile( species->attack );

  if( species->attack->corrupt ) {
    found = TRUE;
    page( ch, "  Mob #%-4d (%s) has no attack program.\n\r",
      species->vnum, species->descr->name ); 
    }

  for( mprog = species->mprog; mprog != NULL; i++, mprog = mprog->next ) {
    if( make && mprog->binary == NULL )
      compile( mprog );
    if( mprog->corrupt ) {
      found = TRUE;
      page( ch, "  Mprog #%d on mob #%d (%s) is not compiling.\n\r",
        i, species->vnum, species->descr->name ); 
      }
    }

  return;
}


inline void obj_bugs( char_data* ch, obj_clss_data* obj, bool& found,
  bool )
{
  bool           either  = FALSE;
  bool            after  = FALSE;
  bool           before  = FALSE;
  const char*   keyword;

  for( int i = 0; i < obj->extra_descr; i++ ) {
    keyword = obj->extra_descr[i]->keyword;     
    either |= !strcasecmp( keyword, "either" );
    after  |= !strcasecmp( keyword, "after" );
    before |= !strcasecmp( keyword, "before" );
    }

  if( !either && ( !after || ( !before && obj->fakes == obj->vnum ) ) ) {
    page( ch, "  Object #%-4d (%s) missing description oextra.\n\r",
      obj->vnum, obj->Name( ) );
    found = TRUE;
    }

  if( is_set( obj->extra_flags, OFLAG_NOSHOW )
    && is_set( &obj->wear_flags, ITEM_TAKE ) ) {
    page( ch, "  Object #%-4d (%s) takable and 'no.show'.\n\r",
      obj->vnum, obj->Name( ) );
    found = TRUE;
    }

  return;
}


inline void room_bugs( char_data* ch, room_data* room, bool& found,
  bool make )
{
  action_data*  action;
  int             i, j;

  for( i = 1, action = room->action; action != NULL;
    i++, action = action->next ) {
    if( make && action->binary == NULL )
      compile( action );
    if( action->corrupt ) {      
      found = TRUE;
      page( ch, "  Acode #%d in room %d is not compiling.\n\r",
        i, room->vnum );
      continue;
      }
    if( action->trigger == TRIGGER_ENTERING
      || action->trigger == TRIGGER_LEAVING ) {
      for( j = 0; j < MAX_DOOR; j++ )
        if( is_set( &action->flags, j ) )
          break;
      if( j == MAX_DOOR ) {
        found = TRUE;
        page( ch,
          "  Acode #%d in room %d has no direction flag checked.\n\r",
          i, room->vnum );
        }
      }
    }
 
  return;
}


void do_bugs( char_data* ch, char* argument )
{
  area_data*          area;
  reset_data*        reset;
  room_data*          room;
  species_data*    species;
  trainer_data*    trainer;
  obj_clss_data*       obj;
  bool               found  = FALSE;
  bool             compile;
  int                    i;
  int                flags;

  if( !get_flags( ch, argument, &flags, "omtrc", "Bugs" ) )
    return;

  if( ( flags & 0xf ) == 0 )
    flags = 0xf;

  compile = is_set( &flags, 4 );

  page( ch, "Bugs Found:\n\r" );

  /*-- OBJECTS --*/

  if( is_set( &flags, 0 ) ) 
    for( i = 0; i < MAX_OBJ_INDEX; i++ ) 
      if( ( obj = obj_index_list[i] ) != NULL )  
        obj_bugs( ch, obj, found, compile );

  /*-- TRAINERS --*/

  if( is_set( &flags, 2 ) ) {
    for( trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
      if( trainer->room == NULL ) {
        page( ch, "  Trainer nowhere??\n\r" );
        found = TRUE;
        continue;
        }
      if( ( species = get_species( trainer->trainer ) ) == NULL ) {
        page( ch, "  Trainer in room #%d with non-existent species.\n\r", 
          trainer->room->vnum );
        found = TRUE;
        continue;
        } 
      for( reset = trainer->room->reset; reset != NULL; reset = reset->next )
        if( reset->vnum == trainer->trainer
          && is_set( &reset->flags, RSFLAG_MOB ) ) 
          break;
      if( reset == NULL ) {
        page( ch,
          "  Trainer entry in room %d for mob %d with no reset.\n\r",
          trainer->room->vnum, trainer->trainer ); 
        found = TRUE;
        }
      }
    }

  /*-- MOBILE BUGS --*/

  if( is_set( &flags, 1 ) ) 
    for( i = 0; i < MAX_SPECIES; i++ ) 
      if( ( species = species_list[i] ) != NULL ) 
        mob_bugs( ch, species, found, compile );

  /*-- AREAS --*/

  if( is_set( &flags, 3 ) ) 
    for( area = area_list; area != NULL; area = area->next )
      if( area->status == AREA_OPEN )
        for( room = area->room_first; room != NULL; room = room->next ) 
          room_bugs( ch, room, found, compile );

  if( !found )
    send( "  none\n\r", ch );
}


