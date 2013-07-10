#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


event_data*  event_queue  [ QUEUE_LENGTH ];
int           event_pntr  = 0;


Event_Data :: Event_Data( event_func* f, thing_data* t )
{
  record_new( sizeof( event_data ), MEM_EVNT );

  func  = f;
  loop  = NULL;
  time  = -1;
  owner = t;

  t->events += this;
}


Event_Data :: Event_Data( )
{
  record_new( sizeof( event_data ), MEM_EVNT );

  func    = NULL;
  loop    = NULL;
  time    = -1;
  pointer = NULL;
}


Event_Data :: ~Event_Data( )
{
  if( time == -2 ) {
    roach( "Attempting to delete event twice." );
    return;
    } 

  record_delete( sizeof( event_data ), MEM_EVNT );

  if( time != -1 ) {
    roach( "Deleting active event." );
    roach( "-- Owner = %s", owner );
    roach( "--  Type = %s", name( this ) );
    }

  if( func == execute_path )
    delete (path_data*) pointer;

  time = -2;
}


/*
 *   DELAY FUNCTIONS
 */


void set_delay( char_data* ch, int delay )
{
  unlink( &ch->active );
  add_queue( &ch->active, delay );
}


void disrupt_spell( char_data* ch, bool self )
{
  if( ch->cast == NULL )
    return;

  if( self ) 
    send( ch, "You abort casting %s.\n\r",
      spell_table[ ch->cast->spell ].name );
  else
    send( ch, "\n\r>> Your spell is disrupted. <<\n\r\n\r" );

  delete ch->cast;
  ch->cast = NULL;

  unlink( &ch->active );
  add_queue( &ch->active, 3 );
}
 

/*
 *   SUPPORT ROUTINES
 */


int time_till( event_data* event )
{
  if( event->time < 0 )
    return QUEUE_LENGTH+1;

  return( (event->time-event_pntr)%QUEUE_LENGTH );
}


void add_queue( event_data* event, int delay )
{
  if( event->time != -1 ) {
    roach( "Attempting to add non-idle event to queue." );
    roach( "-- Owner = %s", event->owner );
    roach( "--  Type = %s", name( event ) );
    roach( "--  Time = %d", event->time );
    return;
    }
 
  if( delay > QUEUE_LENGTH ) {
    roach( "Attempting to add event with delay > queue length." );
    roach( "-- Owner = %s", event->owner );
    roach( "--  Type = %s", name( event ) );
    delay = QUEUE_LENGTH;
    }

  int pos = (event_pntr+delay)%QUEUE_LENGTH;

  event->loop      = event_queue[pos];
  event->time      = pos;
  event_queue[pos] = event;
}


const char* name( event_data* event )
{
  if( event->func == next_action )    return "Fight";
  if( event->func == execute_wander ) return "Wander";
  if( event->func == execute_drown )  return "Wander";
  if( event->func == execute_leap )   return "Wander";
  if( event->func == update_affect )  return "Wander";

  return "Unknown";
}


/* 
 *   EXTRACT
 */


void stop_events( thing_data* thing, event_func* func )
{
  event_data*  event;

  for( int i = thing->events.size-1; i >= 0; i-- ) {
    event = thing->events.list[i];
    if( event->func == func || func == NULL ) {
      event->owner = NULL;
      remove( thing->events, i );
      extract( event );
      }
    }
}


void extract( event_data* event )
{
  if( event->owner != NULL ) 
    event->owner->events -= event;

  unlink( event );
  delete event;
}


void unlink( event_data* event )
{
  event_data* prev;
  int         time;

  if( ( time = event->time ) < 0 )
    return;

  time        = event->time;
  prev        = event_queue[time];
  event->time = -1;

  if( prev == event ) {
    event_queue[time] = prev->loop;
    return;
    }

  for( ; prev->loop != event; prev = prev->loop );

  prev->loop = event->loop;
}


/*
 *   MAIN EVENT HANDLERS
 */
  

void event_update( void )
{
  struct timeval   start;
  event_data*      event;

  gettimeofday( &start, NULL );

  for( ; ; ) {
    if( ( event = event_queue[ event_pntr ] ) == NULL )
      break;

    if( event->time != event_pntr ) {
      roach( "Event_Update: Event with wrong time." );
      roach( "-- Owner = %s", event->owner );  
      panic( "--  Time = %d", event->time );
      }

    event_queue[ event_pntr ] = event->loop;
    event->time               = -1;
    event->loop               = NULL;

    ( *event->func )( event );
    }

  event_pntr = (event_pntr+1)%QUEUE_LENGTH;

  pulse_time[ TIME_EVENT ] = stop_clock( start );
}





















