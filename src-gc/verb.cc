#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void do_climb( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to climb?\n\r" );
    return;
    }
 
  send( ch, "Whatever that is you can't climb it.\n\r" );

  return;
}


void do_enter( char_data* ch, char* argument )
{
  /*
  exit_data *exit;

  if( *argument == '\0' ) {
    send( ch, "What do you want to enter?\n\r" );
    return;
    }
  
  if( ( exit = ch->in_room->exit[ DIR_EXTRA ] ) == NULL
    || !is_name( argument, exit->name ) ) {
    send( "Whatever that is you are unable to enter it.\n\r", ch );
    return;
    }

  move_char( ch, DIR_EXTRA, FALSE );
  */
  return;
}


void do_move( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to move?\n\r" );
    return;
    }
 
  send( ch,
    "Whatever that is trying to move it does nothing interesting.\n\r" );

  return;
}

 
void do_pull( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to pull?\n\r" );
    return;
    }
 
  send( ch,
    "Whatever that is pulling it does nothing interesting.\n\r" );

  return;
}


void do_push( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to push?\n\r" );
    return;
    }
 
  send( ch,
    "Whatever that is pushing it does nothing interesting.\n\r" );

  return;
}


void do_read( char_data* ch, char* )
{
  send( "Whatever that is you can't read it.  Perhaps you should try looking at it.\n\r", ch );
  
  return;
}


 

