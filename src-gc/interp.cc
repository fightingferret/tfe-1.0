#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool  check_progs   ( char_data*, char*, char* );
bool  check_social   ( char_data*, char*, char* );


/*
 *   DISALLOW COMMAND ROUTINES
 */


bool is_mob( char_data* ch )
{
  if( not_player( ch ) )
    return TRUE;

  if( ch->species != NULL ) {
    send( ch, "You unable to do that while switched.\n\r" );
    return TRUE;
    }

  return FALSE;
}


bool not_player( char_data* ch )
{
  if( is_confused_pet( ch ) )
    return TRUE;
 
  if( ch->pcdata == NULL ) {
    send_seen( ch, "%s looks around in confusion.\n\r", ch );
    return TRUE;
    }

  return FALSE;
}


bool is_confused_pet( char_data* ch )
{
  if( ch->pcdata != NULL || ch->leader == NULL
    || !is_set( &ch->status, STAT_ORDERED ) )
    return FALSE;
 
  send( ch->leader, "%s looks at you in bewilderment.\n\r", ch );
  send_seen( ch, "%s looks at %s in bewilderment.\n\r",
    ch, ch->leader );

  return TRUE;
}


bool pet_help( char_data* ch ) 
{
  if( ch->pcdata != NULL || ch->leader == NULL )
    return FALSE;
 
  send( ch->leader, "%s thinks you need help more than %s does.\n\r",
    ch, ch->He_She( ) );

  return TRUE;
} 


bool is_humanoid( char_data* ch )
{
  if( ch->species == NULL )
    return TRUE;

  send( ch, "You can only do that in humanoid form.\n\r" );

  return FALSE;
}


bool is_familiar( char_data* ch )
{
  if( ch->pcdata == NULL || ch->species == NULL
    || ch->shdata->level >= LEVEL_BUILDER )
    return FALSE;

  send( ch, "You can't do that while switched.\n\r" );

  return TRUE;
}


/*
 *   MAIN COMMAND HANDLER
 */


void interpret( char_data* ch, char* argument )
{
  char      command  [ MAX_INPUT_LENGTH ];
  int           cmd;
  int        length;
  bool        found;
  timeval     start;
  time_data    time;
  char*         arg;

  skip_spaces( argument );  

  if( *argument == '+' ) {
    argument++;
    clear( ch->cmd_queue );
    disrupt_spell( ch, TRUE );
    }

  if( *argument == '\0' ) {
    next_page( ch->link );
    return;
    }

  if( ch->link != NULL && ch->link->snoop_by != NULL ) {
    send( ch->link->snoop_by, argument );
    send( ch->link->snoop_by, "\n\r" );
    }

  clear_pager( ch );

  if( ch->pcdata != NULL && is_set( ch->pcdata->pfile->flags, PLR_FREEZE ) 
    && !is_god( ch ) ) {
    send( ch, "You're totally frozen!\n\r" );
    return;
    }

  if( !isalpha( *argument ) && !isdigit( *argument ) ) {
    command[0] = argument[0];
    command[1] = '\0';
    arg = argument+1;
    skip_spaces( arg );
    }
  else
    arg = one_argument( argument, command );

  if( check_progs( ch, command, arg ) ) 
    return;

  found   = TRUE;
  length  = strlen( command );
  cmd     = search( command_table, MAX_ENTRY_COMMAND, command );

  if( cmd < 0 || !has_permission( ch, command_table[cmd].level ) ) {
    if( cmd < 0 )
      cmd = -cmd-1;
    found = FALSE;
    for( ; cmd < MAX_ENTRY_COMMAND; cmd++ ) {
      if( !has_permission( ch, command_table[cmd].level ) )
        continue;
      if( strncasecmp( command_table[cmd].name, command, length ) )
        break;
      if( length >= command_table[cmd].reqlen ) {
        found = TRUE;
        break;
        }
      }
    }

  if( !found ) {
    if( !check_social( ch, command, arg )
      && !speed_walking( ch, command ) )
      send( ch, "<Type 'help' for help>\n\r" );
    return;
    }

  if( is_set( command_table[cmd].level, PERM_DISABLED ) ) {
    send( ch, "The command %s has been disabled.\n\r",
      command_table[cmd].name );
    return;
    }

  if( command_table[cmd].queue && ch->active.time != -1 ) {
    if( ( time_till( &ch->active ) > 5 || entries( ch->cmd_queue ) != 0 )
      && ch->pcdata != NULL
      && is_set( &ch->pcdata->message, MSG_QUEUE ) )
      send( ch, "Queued: %s\n\r", argument );
    push( ch->cmd_queue, argument, is_set( &ch->status, STAT_ORDERED ) );
    return;
    }

  if( command_table[cmd].position > POS_SLEEPING
    && is_set( ch->affected_by, AFF_PARALYSIS ) ) {
    send( ch, "You are paralysed and unable to move!\n\r" );
    return;
    }

  if( ch->position < command_table[cmd].position ) {
    switch( ch->position ) {
      case POS_DEAD:
        send( ch, "You have died, and are unable to do anything.\n\r" );
        break;

      case POS_MORTAL:
      case POS_INCAP:
        send( ch, "The bright white light has you distracted.\n\r" );
        break;

      case POS_STUNNED:
        send( ch, "You are stunned and cannot move.\n\r" );
        break;

      case POS_SLEEPING:
        send( ch, "You cannot do that while sleeping.\n\r" );
        break;

      case POS_MEDITATING:
        send( ch, "You are deep in meditation.\n\r" );
        break;

      case POS_RESTING:
        send( ch, "Perhaps you should stand first.\n\r" );
        break;
      }
    return;
    }

  if( command_table[cmd].position > POS_FIGHTING
    && ch->fighting != NULL ) {
    send( ch, "The current battle has you occupied.\n\r" );
    return;
    }

  if( ( is_avatar( ch ) && !is_god( ch ) ) 
    || ( ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_LOGFILE ) ) )
    immortal_log( ch, command, arg );

  if( command_table[cmd].reveal )
    spoil_hide( ch );

  if( command_table[cmd].function == NULL ) {
    send( ch, "Command %s has no routine assigned to it?!\n\r", 
      command_table[cmd].name );
    bug( "Interpret: command %s has no routine.",
      command_table[cmd].name );
    return;
    }

  gettimeofday( &start, NULL );

  ( *command_table[cmd].function ) ( ch, arg );

  in_character = TRUE;
  msg_type     = MSG_STANDARD;
  time         = stop_clock( start );

  if( command_table[cmd].max_time < time )
    command_table[cmd].max_time = time;

  command_table[cmd].total_time += time;
  command_table[cmd].calls++;

  next_page( ch->link );
}


bool check_progs( char_data* ch, char* command, char* argument )
{
  list_data*       list;
  obj_data*         obj;
  oprog_data*     oprog;
  int                 i;

  if( check_actions( ch, command, argument ) ) 
    return TRUE;

  /*
  list = content_list( ch->contents );
  for( i = 0; i < list->length; i++ ) {
    obj = (obj_data*) list->pntr[i];
    if( obj->valid != OBJ_DATA || obj->array != ch )
      continue;
    for( oprog = obj->pIndexData->oprog; oprog != NULL;
      oprog = oprog->next ) {
      if( is_name( command, oprog->command, TRUE ) &&
        ( oprog->target[0] == '\0' || ( argument[0] != '\0'
        && is_name( argument, oprog->target ) ) ) ) {
        disrupt_spell( ch );
        var_ch   = ch;
        var_room = ch->in_room;
        var_arg  = argument;
        var_obj  = obj;
        if( !execute( oprog ) ) {
          delete list;
          return TRUE; 
	  }
        }
      }
    }
  delete list;

  for( obj = *ch->array; obj != NULL; obj = obj->next_content ) {
    for( oprog = obj->pIndexData->oprog; oprog != NULL;
      oprog = oprog->next ) {
      if( is_name( command, oprog->command, TRUE ) &&
        ( oprog->target[0] == '\0' || ( argument[0] != '\0'
        && is_name( argument, oprog->target ) ) ) ) {
        disrupt_spell( ch );
        var_ch   = ch;
        var_room = ch->in_room;
        var_arg  = argument;
        var_obj  = obj;
        if( !execute( oprog ) )
          return TRUE; 
        }
      }
    }
  */

  return FALSE;
}


/*
 *   DO_COMMAND ROUTINE
 */


void do_commands( char_data* ch, char* argument )
{
  int        i;
  int        j;
  int    trust;
  int    flags;
  int       pf;
  bool   found  = FALSE;

  if( pet_help( ch ) )
    return;

  if( !get_flags( ch, argument, &flags, "p", "Commands" ) )
    return;;

  if( is_set( &flags, 0 ) ) {
    for( pf = 0; pf < MAX_PERMISSION; pf++ ) 
      if( matches( argument, permission_name[pf+1] ) )
        break;
    if( pf == MAX_PERMISSION ) {
      send( ch, "Unknown permission flag.\n\r" );
      return;
      }
    if( !has_permission( ch, pf ) ) {
      send( ch,
        "You cannot view commands you do not have permission for.\n\r" );
      return;
      } 
    for( i = 0, j = 0; i < MAX_ENTRY_COMMAND; i++ ) {
      if( is_set( command_table[i].level, pf ) ) {
        if( !found ) {
          page_title( ch, "Commands requiring %s Flag", 
            permission_name[pf] );
          found = TRUE;
          }
        page( ch, "%15s%s", command_table[i].name,
          ++j%4 ? "" : "\n\r" );
        } 
      }
    if( !found ) 
      send( ch,
        "There are no commands which require the %s permission flag.\n\r",
        permission_name[pf+1] );
    else if( j%4 != 0 )
      page( ch, "\n\r" );        
    return;       
    }

  trust = get_trust( ch );

  if( *argument == '\0' ) {
    page_title( ch, "Command Categories" );
    for( i = 0, j = 0; i < MAX_ENTRY_CMD_CAT; i++ ) {
      if( trust >= cmd_cat_table[i].level ) {
        page( ch, "%15s%s", cmd_cat_table[i].name,
          ++j%4 ? "" : "\n\r" );
        }
      }
    page( ch, "\n\r%s", j%4 != 0 ? "\n\r" : "" );
    page_centered( ch, "[ Type command <category> to see a list commands\
 in that category. ]" );
    return;
    }

  for( i = 0; i < MAX_ENTRY_CMD_CAT; i++ ) 
    if( trust >= cmd_cat_table[i].level 
      && matches( argument, cmd_cat_table[i].name ) ) {
      page_title( ch, "%s Commands", cmd_cat_table[i].name );
      for( j = 0; j < MAX_ENTRY_COMMAND; j++ ) {
        if( command_table[j].category == i 
          && has_permission( ch, command_table[j].level ) ) {
          page( ch, "%15s : %s\n\r", command_table[j].name,
            command_table[j].help );
          }
        }
      return;
      }

  send( ch, "Unknown command category.\n\r" );
}









