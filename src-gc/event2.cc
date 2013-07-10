#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   
 */


inline int modify_delay( char_data* ch, int delay )
{
  int slow  = is_set( ch->affected_by, AFF_SLOW );
  int haste = is_set( ch->affected_by, AFF_HASTE );

  if( slow-haste == -1 ) {
    return 2*delay/3;
    }
  else if( slow-haste == 1 ) {
    return 3*delay/2;
    } 

  return delay;
}


/*
 *
 */


void struggle_web( char_data* ch )
{
  affect_data* affect;

  for( int i = 0; ; i++ ) {
    if( i > ch->affected ) {
      bug( "Execute_Web: Entangled Character with no affect." );
      remove_bit( ch->affected_by, AFF_ENTANGLED );
      return;
      }
    if( ch->affected[i]->type == AFF_ENTANGLED ) {
      affect = ch->affected[i];
      break;
      }
    }

  if( roll_dice( 2,7 )+number_range( 0, affect->level ) < ch->Strength( ) )
    strip_affect( ch, AFF_ENTANGLED );
}


void passive_action( char_data* ch )
{
  command_data*   cmd;

  if( ch->cast != NULL ) {
    spell_update( ch );
    return;
    }

  for( ; ; ) {
    if( ( cmd = pop( ch->cmd_queue ) ) == NULL )
      break;
    assign_bit( &ch->status, STAT_ORDERED, cmd->ordered );
    interpret( ch, cmd->string );
    remove_bit( &ch->status, STAT_ORDERED );
    delete cmd;
    if( ch->link != NULL )
      ch->link->idle = 0;
    if( ch->active.time != -1 )
      return;
    }

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    struggle_web( ch );
    add_queue( &ch->active, 32 );
    return;
    }

  if( ch->position < POS_STANDING
    && ( ch->fighting != NULL
    || !is_empty( ch->aggressive ) ) ) {
    jump_feet( ch );
    add_queue( &ch->active, 32 );
    }    
}


void execute_leap( event_data* event )
{
  char_data* ch      = (char_data*) event->owner;
  char_data* victim  = (char_data*) event->pointer;

  extract( event );

  if( ch->position <= POS_SLEEPING ) 
    return;

  if( ch->fighting != NULL || ch->active.time != -1 )
    return;

  ch->shown = 1;

  passive_action( ch );

  if( ch->active.time != -1 )
    return; 

  leap_message( ch, victim );

  ch->fighting = victim;
  react_attack( ch, victim );

  fight_round( ch );
}


void fight_round( char_data* ch )
{
  int time;

  ch->fighting->shown = 1;

  time = ( ch->species != NULL ? mob_round( ch, ch->fighting ) 
    : player_round( ch, ch->fighting ) );

  if( !ch->Is_Valid( ) )
    return;

  time = modify_delay( ch, time );

  if( ch->species != NULL )
    add_round( ch->species, time );

  if( ch->active.time == -1 ) 
    add_queue( &ch->active, time );
}


void next_action( event_data* event )
{
  char_data*  ch  = (char_data*) event->owner;

  if( ch->position <= POS_SLEEPING ) 
    return;

  ch->shown = 1;

  passive_action( ch );

  if( !ch->Is_Valid( ) || ch->active.time != -1 
    || ( ch->fighting == NULL && is_empty( ch->aggressive ) ) )
    return;

  if( ch->fighting != NULL ) 
    fight_round( ch );
  else
    init_attack( ch, ch->aggressive[0] );
}


/*
 *   WANDER EVENT
 */


void execute_wander( event_data* event )
{
  char_data*     ch  = (char_data*) event->owner;
  exit_data*   exit;

  if( is_set( &ch->status, STAT_ALERT ) && number_range( 0, 30 ) == 0 )
    remove_bit( &ch->status, STAT_ALERT );

  if(  ch->position == POS_STANDING
    && ch->leader == NULL
    && ( exit = random_movable_exit( ch ) ) != NULL
    && !is_set( &exit->to_room->room_flags, RFLAG_NO_MOB )
    && ( !is_set( &ch->species->act_flags, ACT_STAY_AREA )
    || exit->to_room->area == ch->in_room->area ) )
    move_char( ch, exit->direction, FALSE );

  if( ch->Is_Valid( ) ) 
    delay_wander( event );
}


void delay_wander( event_data* event )
{
  char_data*      ch  = (char_data*) event->owner;
  int         wander  = ch->species->wander;
 
  add_queue( event, number_range( wander/10, wander ) );
}


/*
 *   DROWN EVENT
 */


void execute_drown( event_data* event )
{
  char_data* ch = (char_data*) event->owner;

  if( !is_set( ch->affected_by, AFF_WATER_BREATHING ) ) {
    send( ch, "You try to breath but just swallow water.\n\r" );
    send_seen( ch, "%s is drowning!\n\r", ch );
    if( inflict( ch, NULL, 5, "drowning" ) )
      return;
    }

  add_queue( event, number_range( 50, 75 ) );
}














