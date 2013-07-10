#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   ALIAS NEW AND DELETE
 */


Alias_Data :: Alias_Data( char* arg, char* argument )
{
  record_new( sizeof( alias_data ), MEM_ALIAS );

  abbrev      = alloc_string( arg, MEM_ALIAS );
  command     = alloc_string( argument, MEM_ALIAS );

  return;
}


Alias_Data :: ~Alias_Data( )
{
  record_delete( sizeof( alias_data ), MEM_ALIAS );
  
  free_string( abbrev, MEM_ALIAS );
  free_string( command, MEM_ALIAS );
  
  return;
}
 
 
/*
 *   ALIAS ROUTINES
 */


void do_alias( char_data* ch, char* argument )
{
  char             arg  [ MAX_INPUT_LENGTH ];
  alias_data*    alias;
  player_data*      pc;
  int              pos;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    if( is_empty( pc->alias ) ) {
      send( ch, "You have no aliases.\n\r" );
      return;
      }
    page( ch, "Alias:       Substitution:\n\r" );
    for( pos = 0; pos < pc->alias.size; pos++ ) 
      page( ch, "%-12s %s\n\r",
        pc->alias[pos]->abbrev, pc->alias[pos]->command );
    return;
    }

  if( strlen( arg ) > 8 ) {
    send( ch, "Alias must be less than 9 letters.\n\r" );
    return;
    }

  pos = pntr_search( pc->alias.list, pc->alias.size, arg );

  if( *argument == '\0') {
    if( pos >= 0 ) {
      delete pc->alias[pos];
      remove( pc->alias.list, pc->alias.size, pos );
      send( ch, "Alias removed.\n\r" );
      return;
      }
    send( ch, "Alias not found.\n\r" );
    return;
    }

  if( strlen( argument ) > 200 ) {
    send( ch, "Substitution must be less than 201 letters.\n\r" );
    return;
    }

  if( is_number( arg ) ) {
    send( ch, "Due to the prevelant usage of numbers as arguments in\
 commands, it is\n\rill-advised to alias numbers without the use of an\
 asterisk.  See help alias\n\rfor more information.\n\r" );
    return;
    }

  if( !strcmp( arg, "*" ) ) {
    send( ch, "Aliasing nothing doesn't make much sense.\n\r" );
    return;
    }

  if( pos >= 0 ) {
    alias = pc->alias[pos];
    free_string( alias->command, MEM_ALIAS );
    alias->command = alloc_string( argument, MEM_ALIAS );
    }
  else {
    if( pc->alias.size >= 100 ) {
      send( ch, "You are limited to one hundred aliases.\n\r" );
      return;
      }
    alias = new alias_data( arg, argument );
    insert( pc->alias.list, pc->alias.size, alias, -pos-1 );
    }  

  send( ch, "Alias %s -> %s added.\n\r", alias->abbrev, alias->command );
  return;
}


char* subst_alias( link_data* link, char* message )
{
  static char         buf  [ MAX_STRING_LENGTH ];
  player_data*     player;
  alias_data*       alias;
  const char*      abbrev;
  int             i, j, k;
  int              length;
  bool            newline  = TRUE;

  if( ( player = link->player ) == NULL || link->connected != CON_PLAYING 
    || is_empty( player->alias )
    || !strncasecmp( message, "ali", 3 ) )
    return message;

  skip_spaces( message );

  for( i = j = 0; message[i] != '\0' && j < MAX_INPUT_LENGTH; ) {
    for( k = 0; k < player->alias.size; k++ ) {
      alias  = player->alias[k];
      if( *alias->abbrev == '*' ) {
        if( !newline )
          continue;
        abbrev = &alias->abbrev[1];    
        }
      else {
        abbrev = alias->abbrev;
        }
      length = strlen( abbrev );
      if( !strncasecmp( message+i, abbrev, length )
        && ( message[ length+i ] == ' '
        || message[ length+i ] == '\0' ) ) {
        strcpy( buf+j, alias->command );
        i += length;
        j += strlen( alias->command );
        break;
        }
      }
    if( k == player->alias.size ) {
      for( ; message[i] != ' ' && message[i] != '\0'; ) 
        buf[j++] = message[i++];
      }
    for( ; message[i] == ' '; i++ ) {
      buf[j++] = ' ';
      }
    if( message[i] == '&' && message[i+1] == ' ' ) {
      newline = TRUE;
      strcpy( buf+j, "& " );
      j += 2;
      i += 2;
      }
    else {
      newline = FALSE;
      }
    }  

  if( j >= MAX_INPUT_LENGTH ) {
    send( player, "!! Truncating input !!\n\r" );
    buf[ MAX_INPUT_LENGTH-1 ] = '\0';
    }
  else {
    buf[j] = '\0';
    }

  return buf;
}


void ampersand( text_data* receive )
{
  char*        letter;
  text_data*     next;

  if( !strncasecmp( receive->message.text, "ali", 3 ) )
    return; 

  for( letter = receive->message.text; *letter != '\0'; letter++ ) 
    if( !strncmp( letter, " & ", 3 ) ) {  
      next          = new text_data( letter+3 );
      next->next    = receive->next;
      receive->next = next;      
      *letter       = '\0';
      overwrite( receive, receive->message.text );
      break;
      }

  return;
}
 


