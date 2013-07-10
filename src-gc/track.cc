#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   TRACK_DATA CLASS
 */


typedef class Track_Data  track_data;


class Track_Data
{
 public:
  track_data*   next;
  int           race;
  int           to_dir;
  long          decay_time;

  Track_Data( );
  ~Track_Data( );
};


Track_Data :: Track_Data( )
{
  record_new( sizeof( track_data ), MEM_TRACK );

  return;
}


Track_Data :: ~Track_Data( )
{
  record_delete( sizeof( track_data ), MEM_TRACK );

  return;
}


/*
 *  DO_TRACK ROUTINE
 */


void do_track( char_data* ch, char* )
{
  if( is_mob( ch ) )
    return;

if( !ch->check_skill( SKILL_TRACK ) > 0 ) {
  send( ch, "You don't know how to track\n\r" );
  return;
  }

  switch_bit( ch->pcdata->pfile->flags, PLR_TRACK );

  if( is_set( ch->pcdata->pfile->flags, PLR_TRACK ) ) {
    send( ch, "You are now tracking.\n\r" );
    send( ch, "[ Costs more movement points per move. ]\n\r" );
    }
  else
    send( ch, "You stop tracking.\n\r" );

  return;
}


/*
 *   MAKE TRACK ROUTINE
 */


void make_tracks( char_data* ch, room_data* room, int door )
{
  track_data*  track;
  track_data*   next;
  track_data*   prev;

  if( ch->mount != NULL )
    ch = ch->mount;

  if( ch->can_fly( ) || water_logged( room ) ) 
    return;

  track              = new track_data;
  track->decay_time  = current_time+number_range( 300,1000 );
  track->to_dir      = door;
  track->race        = ch->shdata->race;   
  track->next        = room->track;
  room->track        = track;

  for( prev = NULL; track != NULL; track = next ) {
    next = track->next;
    if( ( track->decay_time -= 150 ) < current_time ) {
      if( prev == NULL )
        room->track = next;
      else
        prev->next = next;
      delete track;
      }
    else
      prev = track;
    }

  return;
}


/*
 *   SHOW TRACK ROUTINE
 */


void show_tracks( char_data* ch )
{
  room_data*     room  = ch->in_room;
  track_data*   track;
  int            time;
  bool          found  = FALSE;

  if( ch->species != NULL
    || !is_set( ch->pcdata->pfile->flags, PLR_TRACK ) 
    || !ch->in_room->Seen( ch ) ) 
    return;

  time = current_time+750-75*ch->shdata->skill[ SKILL_TRACK ];

  for( track = room->track; track != NULL; track = track->next ) {
    if( track->decay_time > time ) {
      send( ch, "%sYou see %s tracks heading %s.\n\r",
        found ? "" : "\n\r", race_table[ track->race ].name,
        dir_table[ track->to_dir ].name );
      found = TRUE;
      }  
    }
 
  return;
}








