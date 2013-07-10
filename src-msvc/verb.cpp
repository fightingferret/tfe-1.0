#include "define.h"
#include "struct.h"


void do_climb( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to climb?\r\n" );
    return;
    }
 
  send( ch, "Whatever that is you can't climb it.\r\n" );

  return;
}


void do_enter( char_data* ch, char* argument )
{
  /*
  exit_data *exit;

  if( *argument == '\0' ) {
    send( ch, "What do you want to enter?\r\n" );
    return;
    }
  
  if( ( exit = ch->in_room->exit[ DIR_EXTRA ] ) == NULL
    || !is_name( argument, exit->name ) ) {
    send( "Whatever that is you are unable to enter it.\r\n", ch );
    return;
    }

  move_char( ch, DIR_EXTRA, FALSE );
  */
  return;
}


void do_move( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to move?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is trying to move it does nothing interesting.\r\n" );

  return;
}

 
void do_pull( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to pull?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is pulling it does nothing interesting.\r\n" );

  return;
}


void do_push( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "What do you want to push?\r\n" );
    return;
    }
 
  send( ch,
    "Whatever that is pushing it does nothing interesting.\r\n" );

  return;
}


void do_read( char_data* ch, char* )
{
  send( "Whatever that is you can't read it.  Perhaps you should try looking at it.\r\n", ch );
  
  return;
}


 

