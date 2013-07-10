#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


const char *action_trigger[ MAX_ATN_TRIGGER ] = { "none", "entering room",
  "random", "leaving room", "random_always", "sacrifice", "time", "attack",
  "open_door", "searching", "close_door", "lock_door", "unlock_door",
  "knock_door" };

const char *action_value[ MAX_ATN_TRIGGER ] = { "unused", "unused",
  "chance", "unused", "unused", "minutes past midnight", "unused",
  "unused", "difficulty", "unused", "unused", "unused", "unused" };


#define AFLAG_LOOP_FOLLOWER   0
#define AFLAG_MOUNTED         1
#define AFLAG_RESTING         2
#define AFLAG_SLEEPING        3
#define AFLAG_FIGHTING        4


const char* action_flags [ MAX_ATN_TRIGGER ][ 10 ] =
{
  { "loop_follower", "mounted", "resting", "sleeping", "fighting", "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "" },
  { "" },
  { "" },
  { "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "north", "east", "south", "west", "up", "down", "" },
  { "north", "east", "south", "west", "up", "down", "" },
};


const char** action_msgs [ MAX_ATN_TRIGGER ] =
{
  NULL,
  NULL,
  NULL,
  leaving_msg,
  NULL,
  NULL,
  NULL,
  NULL,
  open_msg,
  NULL,
  close_msg,
  unlock_door_msg,
  lock_door_msg,
  knock_door_msg,
};


/*
 *   DISPLAY ROUTINE
 */


void action_data :: display( char_data* ch )
{
  action_data*  action;
  int                i  = 1;

  if( room == NULL ) {
    send( ch, "%-10s %-10s %s\n\r", "??", "Acode", "Null Room??" );
    return;
    }

  for( action = room->action; action != this; action = action->next, i++ );
  send( ch, "Room %-10d Acode %-8d %s\n\r", room->vnum, i, room->name );

  return;
}     


const char* Keywords( action_data* action )
{
  const char*   list  [ 10 ];
  int              i;
  int              j  = 0;

  for( i = 0; *action_flags[action->trigger][i] != '\0'; i++ ) 
    if( is_set( &action->flags, i ) ) 
      list[j++] = action_flags[action->trigger][i];

  return word_list( list, j, FALSE );
}


/*
 *   DISK ROUTINES
 */


void read( FILE* fp, action_data*& list, room_data* room )
{
  action_data* action;

  action = new action_data;

  action->command = fread_string( fp, MEM_ACTION );
  action->target  = fread_string( fp, MEM_ACTION );
  action->code    = fread_string( fp, MEM_ACTION );

  read_extra( fp, action->data );

  action->trigger = fread_number( fp );
  action->value   = fread_number( fp );
  action->flags   = fread_number( fp );
  action->room    = room;

  append( list, action ); 

  return;  
}


void write( FILE* fp, action_data* action )
{
  fprintf( fp, "A\n" );
  fprintf( fp, "%s~\n", action->command );
  fprintf( fp, "%s~\n", action->target );
  fprintf( fp, "%s~\n", action->code );

  write_extras( fp, action->data );
  fprintf( fp, "!\n" );
 
  fprintf( fp, "%d %d %d\n", action->trigger, action->value, action->flags );
}



/*
 *   EXECUTION ROUTINES
 */


bool check_actions( char_data* ch, char* command, char* argument )
{
  action_data*  action;
  char_data*       rch;
  room_data*      room;
  int                i;

  if( ( room = Room( ch->array->where ) ) == NULL )
    return FALSE;

  for( action = room->action; action != NULL; action = action->next ) 
    if( member( command, action->command, TRUE )
      && member( argument, action->target ) ) 
      break;
          
  if( action == NULL )
    return FALSE;

  if( !check_action_flags( ch, action->flags ) )
    return TRUE;

/*
  if( is_set( &action->flags, AFLAG_LOOP_FOLLOWER ) ) 
    list = follower_list( ch );
*/
 
  disrupt_spell( ch );

  var_ch   = ch;
  var_room = room;
  var_arg  = argument;

  if( !execute( action ) ) 
    return TRUE; 

  /*
  if( list != NULL ) {
    for( i = 0; i < list->length; i++ ) {
      rch = (char_data*) list->pntr[i];
      if( rch->array == room
        && check_action_flags( rch, action->flags, FALSE ) ) {
        var_ch   = rch;
        var_room = room;  
        execute( action );
        }
      }
    delete list;
    }
  */

  return FALSE;
}


bool check_action_flags( char_data *ch, int flags, bool msg )
{
  #define types 4

  int bit[types] = { AFLAG_MOUNTED, AFLAG_FIGHTING, AFLAG_RESTING,
    AFLAG_SLEEPING };

  bool check[types] = {
    ch->mount != NULL,
    ch->fighting != NULL,
    ch->position == POS_RESTING || ch->position == POS_MEDITATING,
    ch->position <= POS_SLEEPING,
    };

  char *word[types] = { "mounted", "fighting", "sitting", "not awake" };

  for( int i = 0; i < types; i++ )
    if( check[i] && !is_set( &flags, bit[i] ) ) {
      if( msg ) 
        send( ch, "You are unable to do that while %s.\n\r", word[i] );
      return FALSE;
      }

  #undef types

  return TRUE;
}


/*
 *   RANDOM ACODES
 */


void action_update( )
{
  area_data*       area;
  room_data*       room;
  action_data*   action;
  
  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next ) 
      for( action = room->action; action != NULL; action = action->next )
        if( action->trigger == TRIGGER_TIME && action->value
          == weather.minute+60*weather.hour )
          execute( action );

  return;
} 
 

/*
 *   EDITING ROUTINES
 */


void do_aedit( char_data *ch, char *argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  char              tmp  [ TWO_LINES ];
  action_data*   action;
  room_data*       room;
  wizard_data*   wizard;
  int                 i;

  if( ( room = Room( ch->array->where ) ) == NULL ) {
    send( ch, "You aren't in a room.\n\r" );
    return;
    }

  wizard = (wizard_data*) ch;

  if( *argument == '\0' ) {
    if( ( action = room->action ) == NULL ) {
      send( ch, "This room has no actions.\n\r" );
      return;
      }
    for( i = 0, action = room->action; action != NULL;
      action = action->next ) {
      if( *action->command != '\0' )
        sprintf( tmp, "[%2d] %-20s %s - %s\n\r", ++i, "Command",
          action->command, action->target );
      else {
        sprintf( tmp, "[%2d] %-20s %s\n\r", ++i,
          action_trigger[action->trigger], Keywords( action ) );
        tmp[5] = toupper( tmp[5] );
        }
      send( ch, tmp );
      }
    return;
    }

  argument = one_argument( argument, arg );

  if( is_number( arg ) ) {
    i = atoi( arg );
    for( action = room->action; action != NULL && i != 1;
      action = action->next, i-- );
    if( action == NULL ) {
      send( ch, "No action by that number.\n\r" );
      return;
      }
    wizard->action_edit = action;
    wizard->adata_edit  = NULL;
    send( ch, "You now edit that action.\n\r" );
    return;
    }

  if( !can_edit( ch, room ) )
    return;

  room->area->modified = TRUE;

  if( !strcasecmp( arg, "new" ) ) {
    action               = new action_data;
    action->command      = alloc_string( argument, MEM_ACTION );
    wizard->action_edit  = action;
    wizard->adata_edit   = NULL;
    append( room->action, action );
    send( ch, "Action added.\n\r" );
    return;
    }

  if( !strcasecmp( arg, "delete" ) ) {
    if( ( action = wizard->action_edit ) == NULL ) {
      send( ch, "You aren't editing any action.\n\r" );
      return;
      }
    remove( room->action, action );  
    extract( wizard, offset( &wizard->action_edit, wizard ), "action" );
    delete action;
    send( ch, "Action deleted.\n\r" );
    return;
    }

  send( ch, "Illegal syntax.\n\r" );
  return;
}


void do_aflag( char_data* ch, char* argument )
{
  action_data*     action;
  const char*    response;
  wizard_data*     wizard;
  int                 max;

  wizard = (wizard_data*) ch;

  if( ( action = wizard->action_edit ) == NULL ) {
    send( ch, "You aren't editing any action.\n\r" );
    return;
    }

  for( max = 0; *action_flags[action->trigger][max] != '\0'; max++ );

  if( max == 0 ) {
    send( ch, "That type of trigger has no action flags.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    display_flags( "Action", &action_flags[action->trigger][0],
      &action_flags[action->trigger][1], &action->flags, max, ch );
    return;
    }

  if( set_flags( &action_flags[action->trigger][0],
    &action_flags[action->trigger][1], &action->flags,
    max, NULL, ch, argument, FALSE, TRUE ) == NULL ) 
    return;

  room_data* room = (room_data*) ch->array->where;
  room->area->modified = TRUE;
}


void do_astat( char_data* ch, char* )
{
  char              buf  [ 3*MAX_STRING_LENGTH ];
  action_data*   action;
  wizard_data*   wizard;

  wizard = (wizard_data*) ch;

  if( ( action = wizard->action_edit ) == NULL ) {
    send( ch, "You aren't editing any action.\n\r" );
    return;
    }

  page( ch, "%10s : %s\n\r", "Trigger", action_trigger[ action->trigger ] );
  page( ch, "%10s : %d\n\r", "Value", action->value );
  page( ch, "%10s : %s\n\r", "Command", action->command );
  page( ch, "%10s : %s\n\r\n\r", "Target", action->target );

  sprintf( buf, "[Code]\n\r%s\n\r", action->code );
  page( ch, buf );

  show_extras( ch, action->data );

  return;
}


void do_aset( char_data* ch, char* argument )
{
  action_data*   action;
  wizard_data*   wizard;
  room_data*       room  = (room_data*) ch->array->where;

  wizard = (wizard_data*) ch;

  if( ( action = wizard->action_edit ) == NULL ) {
    send( ch, "You aren't editing any action.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    do_astat( ch, argument );
    return;
    }

  if( !can_edit( ch, room ) )
    return;

  room->area->modified = TRUE;

#define att  action_trigger

  class type_field type_list[] = {
    { "trigger",  MAX_ATN_TRIGGER,  &att[0],  &att[1],  &action->trigger },
    { "",         0,                NULL,     NULL,     NULL             }
    };

#undef att

  if( process( type_list, ch, "action", argument ) )
    return;

  class string_field string_list[] = {
    { "target",    MEM_ACTION,  &action->target,   NULL },
    { "command",   MEM_ACTION,  &action->command,  NULL },
    { "",          0,           NULL,              NULL },   
    };

  if( process( string_list, ch, "action", argument ) )
    return;

  if( matches( argument, "value" ) ) {
    action->value = atoi( argument );
    send( ch, "Value on action set to %d.\n\r", action->value );
    return;
    }

  send( ch, "Syntax: aset <field> <value>\n\r" );
  return;
}


void do_acode( char_data* ch, char* argument )
{
  action_data*   action;
  wizard_data*   wizard;
  room_data*       room  = (room_data*) ch->array->where;

  wizard = (wizard_data*) ch;

  if( ( action = wizard->action_edit ) == NULL ) {
    send( ch, "You aren't editing any action.\n\r" );
    return;
    }

  if( argument[0] != '\0' && !can_edit( ch, room ) )
    return;

  room->area->modified = TRUE;
  action->code = edit_string( ch, argument, action->code, MEM_ACTION );

  var_ch =  ch;
  compile( action );
}


void do_adata( char_data* ch, char *argument )
{
  action_data*  action;
  wizard_data*  wizard;
  room_data*      room  = (room_data*) ch->array->where;

  wizard = (wizard_data*) ch;

  if( ( action = wizard->action_edit ) == NULL ) {
    send( ch, "You aren't editing any action.\n\r" );
    return;
    }

  if( wizard->adata_edit != NULL ) {
    if( !strcasecmp( argument, "exit" ) || !strcmp( argument, ".." ) ) {
      wizard->adata_edit = NULL;
      send( ch, "Adata now operates on the data list.\n\r" );
      return;
      }      
    wizard->adata_edit->text = edit_string( ch, 
      argument, wizard->adata_edit->text, MEM_EXTRA );
    }
  else {
    if( *argument == '\0' )
      show_defaults( ch, action->trigger, action_msgs );

    edit_extra( action->data, wizard, offset( &wizard->adata_edit, wizard ),
      argument, "adata" );
    room->area->modified = TRUE;
    }

  var_ch = ch;
  compile( action );
}














