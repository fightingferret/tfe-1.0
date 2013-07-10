#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


obj_data*   has_key        ( char_data*, int );
room_data*  create_room    ( char_data* );
void        create_exit    ( wizard_data*, int, char*, bool );
void        delete_exit    ( wizard_data*, exit_data*, bool );
int         direction_arg  ( char*& argument );


const char* dflag_name [] = { "is.door", "closed", "locked", "secret",
  "pickproof", "no.show", "no.open", "reset.closed", "reset.locked",
  "reset.open", "requires.climb", "searchable" };


/*
 *   EXIT_DATA CLASS
 */


Exit_Data :: Exit_Data( ) 
{
  record_new( sizeof( exit_data ), MEM_EXIT );

  name      = empty_string;
  keywords  = empty_string;
  exit_info = 0;
  key       = -1;
  strength  = 10;
  light     = 10;
  size      = SIZE_OGRE;
  direction = -1;  
  to_room   = NULL;
}


Exit_Data :: ~Exit_Data( )
{
  record_delete( sizeof( exit_data ), MEM_EXIT );

  free_string( name,     MEM_EXIT );
  free_string( keywords, MEM_EXIT );
}


/*
 *
 */


const char* Exit_Data :: Name( char_data* ch, int, bool )
{
  return Seen_Name( ch );
}


const char* Exit_Data :: Seen_Name( char_data* ch, int, bool )
{
  char* tmp = static_string( );

  if( ch->pcdata == NULL
    || !is_set( &ch->pcdata->message, MSG_DOOR_DIRECTION ) ) 
    sprintf( tmp, "the %s", name );
  else
    sprintf( tmp, "the %s %s", name,
      dir_table[ direction ].where );  

  return tmp;
}


const char* Exit_Data :: Keywords( char_data* ch )
{
  char* tmp = static_string( );

  sprintf( tmp, "%s 1 %s %s",
    keywords, dir_table[ direction ].name,
    Seen_Name( ch ) );

  return tmp;
}


bool Exit_Data :: Seen( char_data* ch )
{
  wizard_data* imm;

  if( !is_set( &exit_info, EX_CLOSED ) ) 
    return TRUE;

  if( ( imm = wizard( ch ) ) != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_HOLYLIGHT ) )
    return TRUE;

  if( is_set( &exit_info, EX_NO_SHOW ) )
    return FALSE;
 
  if( is_set( &exit_info, EX_SECRET ) 
    && !includes( ch->seen_exits, this ) ) 
    return FALSE;

  return TRUE;
}


/*
 *   DISK ROUTINES
 */


exit_data* add_exit( room_data* room, int dir )
{
  exit_data* exit;

  exit             = new exit_data;
  exit->direction  = dir;

  for( int i = 0; ; i++ ) 
    if( i == room->exits.size || dir < room->exits[i]->direction ) {
      insert( room->exits.list, room->exits.size, exit, i );
      break;
      }

  return exit;
}


void read_exits( FILE* fp, room_data* room, int vnum )
{
  exit_data*    exit;
  char        letter;

  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != 'D' )
      break;

    exit = add_exit( room, fread_number( fp ) );

    exit->name        = fread_string( fp, MEM_EXIT );
    exit->keywords    = fread_string( fp, MEM_EXIT );
    exit->exit_info   = fread_number( fp );
    exit->key         = fread_number( fp );
    exit->to_room     = (room_data*) fread_number( fp );
    exit->strength    = fread_number( fp );
    exit->light       = fread_number( fp );
    exit->size        = fread_number( fp );

    if( exit->direction < 0 || exit->direction > 6 ) 
      panic( "Fread_rooms: vnum %d has bad door number.", vnum );
    }

  ungetc( letter, fp );
}


/*
 *   DISPLAY ROUTINES
 */


void do_exits( char_data* ch, char* )
{
  exit_data*   exit;
  bool        found  = FALSE;

  if( !ch->Can_See( TRUE ) )
    return;

  send_underlined( ch, "Obvious Exits\n\r" );

  for( int i = 0; i < ch->in_room->exits; i++ ) {
    exit = ch->in_room->exits[i];
    if( exit->Seen( ch ) ) {
      found = TRUE;
      if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
        send( ch, "%-5s - %s\n\r",
          dir_table[ exit->direction ].name, exit->to_room->is_dark( )
          ? "Too dark to tell" : exit->to_room->name );
        }
      else {
        send( ch, "%-5s - Closed %s\n\r",
          dir_table[ exit->direction ].name, exit->name );
        }
      }
    }

  if( !found )
    send( ch, "None.\n\r" );
}


int exits_prompt( char* tmp, char_data* ch )
{
  exit_data*  exit;
  char*     string  = tmp;
  bool       water  = FALSE;
  int        exits  = 0;

  if( !ch->Can_See( ) ) {
    strcpy( tmp, "??" );
    return 2;
    }

  if( !ch->in_room->Seen( ch ) ) {
    strcpy( tmp, "-Dark-" );
    return 6;
    }

  for( int i = 0; i < ch->in_room->exits; i++ ) {
    exit = ch->in_room->exits[i];

    if( !exit->Seen( ch ) )
      continue;

    if( !is_set( &exit->exit_info, EX_CLOSED ) 
      && water_logged( exit->to_room ) ) {
      if( !water ) {
        strcpy( string, blue( ch ) );
        string += strlen( string );
        water = TRUE;
        }
      }
    else {
      if( water ) {
        strcpy( string, normal( ch ) );
        string += strlen( string );
        water = FALSE;
        }
      }

    *string = *dir_table[ exit->direction ].name;

    if( !is_set( &exit->exit_info, EX_CLOSED ) )
      *string = toupper( *string );

    exits++;
    string++;
    } 

  if( water ) 
    strcpy( string, normal( ch ) );
  else if( string == tmp ) {
    strcpy( tmp, "none" );
    return 4;
    }
  else 
    *string = '\0';

  return exits;
}  


void autoexit( char_data* ch )
{ 
  char          buf  [ TWO_LINES ];
  exit_data*   exit;
  room_data*   room;

  if( !is_set( ch->pcdata->pfile->flags, PLR_AUTO_EXIT ) )
    return;

  *buf = '\0';

  for( int i = 0; i < ch->in_room->exits; i++ ) {
    exit = ch->in_room->exits[i];
    if( !is_set( &exit->exit_info, EX_CLOSED )
      || ( !is_set( &exit->exit_info, EX_SECRET ) 
      && !is_set( &exit->exit_info, EX_NO_SHOW ) ) ) {
      if( *buf == '\0' )
        strcpy( buf, "[Exits:" );
      room = exit->to_room;
      if( is_builder( ch ) )
        sprintf( buf+strlen( buf ), " #%d", room->vnum  );
      if( water_logged( room ) ) 
        sprintf( buf+strlen( buf ), " %s%s%s", blue( ch ),
          dir_table[ exit->direction ].name, normal( ch ) );
      else
        sprintf( buf+strlen( buf ), " %s",
          dir_table[ exit->direction ].name );
      }
    }

  if( *buf == '\0' ) {
    send( ch, "[Exits: none]\n\r" );
    }
  else {
    strcat( buf, "]\n\r" );
    send( ch, buf );
    }
}


/*
 *   RANDOM EXIT ROUTINE
 */


exit_data* random_movable_exit( char_data* ch )
{
  room_data*  room  = ch->in_room;
  int        count  = 0;

  for( int i = 0; i < room->exits; i++ )
    if( ch->Can_Move( room->exits[i] ) )
      count++;

  if( count == 0 )
    return NULL;

  count = number_range( 1, count );

  for( int i = 0; i < room->exits; i++ ) 
    if( ch->Can_Move( room->exits[i] ) && --count == 0  )
      return room->exits[i];

  return NULL;
}


exit_data* random_open_exit( room_data* room )
{
  int count  = 0;

  for( int i = 0; i < room->exits; i++ )
    if( !is_set( &room->exits[i]->exit_info, EX_CLOSED ) )
      count++;

  if( count == 0 )
    return NULL;

  count = number_range( 1, count );

  for( int i = 0; i < room->exits; i++ ) 
    if( !is_set( &room->exits[i]->exit_info, EX_CLOSED )
      && --count == 0  )
      return room->exits[i];

  return NULL;
}


exit_data* random_exit( room_data* room )
{
  int i;

  if( room->exits == 0 )
    return NULL;

  i = number_range( 0, room->exits.size-1 );

  return room->exits[i];
}


/*
 *   SUPPORT ROUTINES
 */


int direction_arg( char*& argument )
{
  for( int i = 0; i < MAX_DOOR; i++ )
    if( matches( argument, dir_table[i].name ) )
      return i;

  return -1;
}


exit_data* exit_direction( room_data* room, int door )
{
  for( int i = 0; i < room->exits; i++ )
    if( room->exits[i]->direction == door )
      return room->exits[i];

  return NULL;
}


exit_data* reverse( exit_data* exit )
{
  exit_array*  array  = &exit->to_room->exits;
  int           door  = dir_table[ exit->direction ].reverse;

  for( int i = 0; i < *array; i++ )
    if( array->list[i]->direction == door )
      return array->list[i];

  return NULL;
}


void show_secret( exit_data* )
{
}


/*
 *   OPEN/CLOSE ROUTINES
 */


void do_open( char_data* ch, char* argument )
{
  thing_data*  thing;
  obj_data*      obj;
  exit_data*    door;

  if( is_confused_pet( ch ) )
    return;

  if( ( thing = one_thing( ch, argument, "open",
    (thing_array*) &ch->in_room->exits, ch->array,
    &ch->contents ) ) == NULL )
    return;

  if( ( obj = object( thing ) ) != NULL ) {
    include_closed = FALSE;
    open_object( ch, obj );
    include_closed = TRUE;
    return;
    }

  if( ( door = exit( thing ) ) != NULL ) {
    open_door( ch, door );   
    return;
    }

  send( ch, "%s isn't something you can open.\n\r", thing );
}


const char* open_msg [] =
{
  "to_char",  "You open $1.\n\r",
  "to_room",  "$1 opens $2.\n\r",
  "to_side",  "The $1 opens.\n\r",
  ""
};


void open_object( char_data* ch, obj_data* obj )
{
  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( ch, "%s is not a container.\n\r", obj );
    return;
    }

  if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
    send( ch, "%s is already open.\n\r", obj );
    return;
    }

  if( !is_set( &obj->value[1], CONT_CLOSEABLE ) ) {
    send( ch, "You can't do that.\n\r", ch );
    return;
    }

  if( is_set( &obj->value[1], CONT_LOCKED ) ) {
    send( ch, "%s is locked.\n\r", obj );
    return;
    }

  remove_bit( &obj->value[1], CONT_CLOSED );
  send( ch, "You open %s.\n\r", obj );
  send( *ch->array, "%s opens %s.\n\r", ch, obj );
}


bool open_door( char_data* ch, exit_data* exit ) 
{
  action_data*   action;
  room_data*       room  = ch->in_room;
 
  if( !is_set( &exit->exit_info, EX_ISDOOR ) ) {
    send( ch, "There is no door or wall there.\n\r" );
    return FALSE;
    }

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s is already open.\n\r", exit );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_LOCKED ) ) {
    send( ch, "%s is locked.\n\r", exit );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_NO_OPEN ) ) {
    send( ch, "You see no way to open %s.\n\r", exit );
    return FALSE;
    }

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_OPEN_DOOR
      && is_set( &action->flags, exit->direction ) ) {
      var_ch   = ch;
      var_room = room;
      if( !execute( action ) || ch->in_room != room )
        return FALSE;
      break;
      }

  act( ch, prog_msg( action, open_msg[0], open_msg[1] ), exit );
  act_notchar( prog_msg( action, open_msg[2], open_msg[3] ), ch, exit );
  remove_bit( &exit->exit_info, EX_CLOSED );

  room = exit->to_room; 

  if( ( exit = reverse( exit ) ) != NULL ) {
    remove_bit( &exit->exit_info, EX_CLOSED );
    act_room( room, prog_msg( action, open_msg[4], open_msg[5] ),
      exit );
    }

  return TRUE;
}


/*
 *   CLOSE
 */


const char* close_msg [] =
{
  "to_char",  "You close $1.\n\r",
  "to_room",  "$1 closes $2.\n\r",
  "to_side",  "$1 closes.\n\r",
  ""
};


void do_close( char_data* ch, char* argument )
{
  obj_data*         obj;
  exit_data*       door;
  thing_data*     thing;

  if( ( thing = one_thing( ch, argument, "close",
    (thing_array*) &ch->in_room->exits, ch->array,
    &ch->contents ) ) == NULL )
    return;

  if( ( obj = object( thing ) ) != NULL ) {
    include_closed = FALSE;
    close_object( ch, obj );
    include_closed = TRUE;
    return;
    }

  if( ( door = exit( thing ) ) != NULL ) {
    close_door( ch, door );   
    return;
    }

  send( ch, "%s isn't closable.\n\r", thing );
}


bool close_door( char_data* ch, exit_data* exit ) 
{
  room_data*       room;
  action_data*   action;

  if( !is_set( &exit->exit_info, EX_ISDOOR ) ) {
    send( ch, "There is nothing there that closes.\n\r" );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s is already closed.\n\r", exit );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_NO_OPEN ) ) {
    send( ch, "You don't see anyway to close %s.\n\r", exit );
    return FALSE;
    }

  room = ch->in_room;

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_CLOSE_DOOR
      && is_set( &action->flags, exit->direction ) ) {
      var_ch   = ch;
      var_room = room;
      if( !execute( action ) || ch->in_room != room )
        return FALSE;
      break;
      }

  act( ch, prog_msg( action, close_msg[0], close_msg[1] ), exit );
  act_notchar( prog_msg( action, close_msg[2], close_msg[3] ), ch, exit );
  set_bit( &exit->exit_info, EX_CLOSED );

  room = exit->to_room;

  if( ( exit = reverse( exit ) ) != NULL ) {
    set_bit( &exit->exit_info, EX_CLOSED );
    act_room( room, prog_msg( action, close_msg[4], close_msg[5] ),
      exit );
    }

  return FALSE;
}


void close_object( char_data* ch, obj_data* obj )
{
  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( ch, "%s isn't a container.\n\r", obj );
    return;
    }

  if( is_set( &obj->value[1], CONT_CLOSED ) ) {
    send( ch, "%s is already closed.\n\r", obj );
    return;
    }

  if( !is_set( &obj->value[1], CONT_CLOSEABLE ) ) {
    send( ch, "You can't do that.\n\r" );
    return;
    }

 set_bit( &obj->value[1], CONT_CLOSED );
 send( ch, "You close %s.\n\r", obj );
 send( *ch->array, "%s closes %s.\n\r", ch, obj );
}


/*
 *   KEYS
 */


obj_data* has_key( char_data* ch, int vnum )
{
  obj_data*   obj;
  obj_data*  obj2;

  for( int i = 0; i < ch->contents; i++ ) {
    if( ( obj = object( ch->contents[i] ) ) == NULL )
      continue;
    if( obj->pIndexData->vnum == vnum ) {
      obj->selected = 1;
      return obj;
      }
    if( obj->pIndexData->item_type == ITEM_KEYRING )
      for( int j = 0; j < obj->contents; j++ ) {
        if( ( obj2 = object( obj->contents[j] ) ) != NULL
          && obj2->pIndexData->vnum == vnum ) {
          obj2->selected = 1;
          return obj2;
	  }
        }
    }

  return NULL;
}


/*
 *   LOCK ROUTINES
 */


const char* lock_msg [] =
{
  "to_char",  "You lock $1 with $2.\n\r",
  "to_room",  "$1 locks $2 with $3.\n\r",
  ""
};


const char* lock_door_msg [] =
{
  "to_char",  "You lock $1 with $2.\n\r",
  "to_room",  "$1 locks $2 with $3.\n\r",
  "to_side",  "You hear a key turn in $1.\n\r",
  ""
};


void do_lock( char_data* ch, char *argument )
{
  exit_data*       door;
  obj_data*         obj;
  obj_data*         key;
  oprog_data*     oprog;
  thing_data*     thing;

  if( ( thing = one_thing( ch, argument, "lock",
    (thing_array*) &ch->in_room->exits, ch->array,
    &ch->contents ) ) == NULL )
    return;
   
  if( ( obj = object( thing ) ) != NULL ) {
    if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
      send( ch, "That's not a container.\n\r" );
      return;
      }

    if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
      send( ch, "It's not closed.\n\r" );
      return;
      }

    if( obj->pIndexData->value[2] < 0 ) {
      send( ch, "It can't be locked.\n\r" );
      return;
      }

    if( ( key = has_key( ch, obj->pIndexData->value[2] ) ) == NULL ) {
      send( ch, "You lack the key.\n\r" );
      return;
      }

    if( is_set( &obj->value[1], CONT_LOCKED ) ) {
      send( ch, "It's already locked.\n\r" );
      return;
      }

    for( oprog = obj->pIndexData->oprog; oprog != NULL;
      oprog = oprog->next )
      if( oprog->trigger == OPROG_TRIGGER_LOCK ) {
        var_ch        = ch;
        var_room      = ch->in_room;
        var_obj       = key;
        var_container = obj;
        execute( oprog );
        break;
        }

    act( ch, prog_msg( oprog, lock_msg[0], lock_msg[1] ),
      obj, key );
    act_notchar( prog_msg( oprog, lock_msg[2], lock_msg[3] ),
      ch, obj, key );
    set_bit( &obj->value[1], CONT_LOCKED );
    return;
    }

  if( ( door = exit( thing ) ) != NULL ) {
    lock_door( ch, door );   
    return;
    }

  send( ch, "%s isn't lockable.\n\r", thing );
}

 
bool lock_door( char_data* ch, exit_data* exit )
{  
  room_data*       room;
  action_data*   action;
  obj_data*         key;

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s is not closed.\n\r", exit );
    return FALSE;
    }
  
  if( exit->key < 0 ) {
    send( ch, "%s can't be locked.\n\r", exit );
    return FALSE;
    }

  if( ( key = has_key( ch, exit->key ) ) == NULL ) {
    send( ch, "You lack the key.\n\r" );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_LOCKED ) ) {
    send( ch, "%s is already locked.\n\r", exit );
    return FALSE;
    }

  room = ch->in_room;

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_LOCK_DOOR
      && is_set( &action->flags, exit->direction ) ) {
      var_ch   = ch;
      var_room = room;
      if( !execute( action ) || ch->in_room != room )
        return FALSE;
      break;
      }

  act( ch, prog_msg( action, lock_door_msg[0], lock_door_msg[1] ),
    key, exit );
  act_notchar( prog_msg( action, lock_door_msg[2], lock_door_msg[3] ),
    ch, key, exit );
  set_bit( &exit->exit_info, EX_LOCKED );

  room = exit->to_room;

  if( ( exit = reverse( exit ) ) != NULL ) {
    set_bit( &exit->exit_info, EX_LOCKED );
    act_room( room, prog_msg( action, lock_door_msg[4], lock_door_msg[5] ),
      exit );
    }

  return TRUE;
}


/*
 *   UNLOCK ROUTINES
 */


const char* unlock_msg [] =
{
  "to_char",  "You unlock $1 with $2.\n\r",
  "to_room",  "$1 unlocks $2 with $3.\n\r",
  ""
};


const char* unlock_door_msg [] =
{
  "to_char",  "You unlock $1 with $2.\n\r",
  "to_room",  "$1 unlocks $2 with $3.\n\r",
  "to_side",  "You hear a key turn in $1.\n\r",
  ""
};


void do_unlock( char_data* ch, char* argument )
{
  obj_data*         key;
  exit_data*       door;
  obj_data*         obj;
  oprog_data*     oprog;
  thing_data*     thing;

  if( ( thing = one_thing( ch, argument, "unlock",
    (thing_array*) &ch->in_room->exits, ch->array,
    &ch->contents ) ) == NULL )
    return;
   
  if( ( obj = object( thing ) ) != NULL ) {
    if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
      send( ch, "%s isn't a container.\n\r", obj );
      return;
      }
    if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
      send( ch, "%s isn't closed.\n\r", obj );
      return;
      }
    if( obj->pIndexData->value[2] < 0 ) {
      send( ch, "%s can't be unlocked.\n\r", obj );
      return;
      }
    if( ( key = has_key( ch, obj->pIndexData->value[2] ) ) == NULL ) {
      send( ch, "You lack the key.\n\r" );
      return;
      }
    if( !is_set( &obj->value[1], CONT_LOCKED ) ) {
      send( ch, "%s is already unlocked.\n\r", obj );
      return;
      }

    for( oprog = obj->pIndexData->oprog; oprog != NULL;
      oprog = oprog->next )
      if( oprog->trigger == OPROG_TRIGGER_UNLOCK ) {
        var_ch        = ch;
        var_room      = ch->in_room;
        var_obj       = key;
        var_container = obj;
        execute( oprog );
        break;
        }

    act( ch, prog_msg( oprog, unlock_msg[0], unlock_msg[1] ),
      obj, key );
    act_notchar( prog_msg( oprog, unlock_msg[2], unlock_msg[3] ),
      ch, obj, key );
    remove_bit( &obj->value[1], CONT_LOCKED );

    return;
    }

  if( ( door = exit( thing ) ) != NULL ) {
    unlock_door( ch, door );
    return;
    }

  send( ch, "%s isn't unlockable.\n\r", thing );
}
  

bool unlock_door( char_data* ch, exit_data* exit )
{ 
  action_data*  action;
  room_data*      room;
  obj_data*        key;

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "The %s exit is not associated with a door.\n\r",
      dir_table[ exit->direction ].name );
    return FALSE;
    }

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s is not closed.\n\r", exit );
    return FALSE;
    }

  if( exit->key < 0 ) {
    send( ch, "You see no obvious way to unlock %s.\n\r", exit );
    return FALSE;
    }

  if( ( key = has_key( ch, exit->key ) ) == NULL ) {
    send( ch, "You lack the key - perhaps try picking it.\n\r" );
    return FALSE;
    }

  if( !is_set( &exit->exit_info, EX_LOCKED ) ) {
    send( ch, "%s is already unlocked.\n\r", exit );
    return FALSE;
    }

  room = ch->in_room;

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_UNLOCK_DOOR
      && is_set( &action->flags, exit->direction ) ) {
      var_ch   = ch;
      var_room = room;
      if( !execute( action ) || ch->in_room != room )
        return TRUE;
      break;
      }

  act( ch, prog_msg( action, unlock_door_msg[0], unlock_door_msg[1] ),
    key, exit );
  act_notchar( prog_msg( action, unlock_door_msg[2], unlock_door_msg[3] ),
    ch, key, exit );
  remove_bit( &exit->exit_info, EX_LOCKED );

  room = exit->to_room;

  if( ( exit = reverse( exit ) ) != NULL ) {
    remove_bit( &exit->exit_info, EX_LOCKED );
    act_room( room, prog_msg( action, unlock_door_msg[4],
      unlock_door_msg[5] ), exit );
    }

  return TRUE;
}


void do_pick( char_data* ch, char* argument )
{
  obj_data*       obj;
  thing_data*   thing;
  exit_data*     door; 

  if( ( thing = one_thing( ch, argument, "pick",
    (thing_array*) &ch->in_room->exits, ch->array,
    &ch->contents ) ) == NULL )
    return;

  if( ( obj = object( thing ) ) != NULL ) {
    if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
      send( ch, "%s isn't a container.\n\r", obj );
      return;
      }
    if( !is_set( &obj->value[1], CONT_CLOSED ) ) { 
      send( ch, "%s is not closed.\n\r", obj );
      return;
      }
    if( !is_set( &obj->value[1], CONT_LOCKED ) ) {
      send( ch, "%s is already unlocked.\n\r", obj );
      return;
      }
    if( ch->species != NULL
      || !ch->check_skill( SKILL_PICK_LOCK ) ) {
      send( ch, "You failed.\n\r" );
      return;
      }      
    if( is_set( &obj->value[1], CONT_PICKPROOF ) ) {
      send( ch, "You fail to pick the lock and are fairly sure it is impossible to pick.\n\r" ); 
      return;
      }
    send( ch, "*Click*\n\r" );
    send_seen( ch, "%s picks %s.\n\r", ch, obj );
    return;
    }

  if( ( door = exit( thing ) ) != NULL ) {
    pick_door( ch, door );
    return;
    }

  send( ch, "%s isn't pickable.\n\r", thing );
} 


bool pick_door( char_data* ch, exit_data* exit )
{
  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s is not closed.\n\r", exit );
    return FALSE;
    }

  if( !is_set( &exit->exit_info, EX_LOCKED ) ) {
    send( ch, "%s is already unlocked.\n\r", exit );
    return FALSE;
    }

  if( exit->key < 0 ) {
    send( ch, "You see no keyhole in the door to pick.\n\r" );
    return FALSE;
    }

  if( is_set( &exit->exit_info, EX_PICKPROOF ) ) {
    send( ch, "You failed.\n\r" );
    return FALSE;
    }
  
  if( ch->species != NULL || !ch->check_skill( SKILL_PICK_LOCK ) ) {
    send( ch, "You failed.\n\r" );
    return FALSE;
    }

  remove_bit( &exit->exit_info, EX_LOCKED );

  send( ch, "*Click*\n\r" );
  send_seen( ch, "%s picks %s.\n\r", ch, exit );
    
  ch->improve_skill( SKILL_PICK_LOCK );

  if( ( exit = reverse( exit ) ) != NULL ) 
    remove_bit( &exit->exit_info, EX_LOCKED );

  return TRUE;
}


/*
 *   KNOCK
 */


const char* knock_door_msg [] =
{
  "to_char",  "You knock on $1.\n\r",
  "to_room",  "$1 knocks $2.\n\r",
  "to_side",  "You hear a knock on $1.\n\r",
  ""
};


void do_knock( char_data* ch, char* argument )
{
  exit_data*     door;
  thing_data*   thing;

  if( is_confused_pet( ch ) )
    return;

  if( ( thing = one_thing( ch, argument, "knock",
    (thing_array*) &ch->in_room->exits ) ) == NULL )
    return;

  if( ( door = exit( thing ) ) != NULL ) {
    knock_door( ch, door );
    return;
    }

  send( ch, "Knocking on %s would serve no purpose.\n\r", thing );
}


void knock_door( char_data* ch, exit_data* exit )
{
  action_data*  action;
  room_data*      room;

  if( !is_set( &exit->exit_info, EX_ISDOOR ) ) {
    send( ch, "There is no door %s to knock on.\n\r",
      dir_table[ exit->direction ].where );
    return;
    }

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "It's not closed, so no reason to knock.\n\r" );
    return;
    }

  room = ch->in_room;

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_KNOCK_DOOR
      && is_set( &action->flags,  exit->direction  ) ) {
      var_ch   = ch;
      var_room = room;
      if( !execute( action ) || ch->in_room != room )
        return;
      break;
      }

  act( ch, prog_msg( action, knock_door_msg[0], knock_door_msg[1] ),
    exit );
  act_notchar( prog_msg( action, knock_door_msg[2], knock_door_msg[3] ),
    ch, exit );

  room = exit->to_room;

  if( ( exit = reverse( exit ) ) != NULL ) 
    act_room( room, prog_msg( action, knock_door_msg[4],
      knock_door_msg[5] ), exit );
}


/*
 *   DOOR BASH ROUTINE
 */


void bash_door( char_data* ch, exit_data* exit )
{
  if(  is_mounted( ch )
    || is_entangled( ch, "bash doors" ) )
    return;

  if( !is_set( &exit->exit_info, EX_ISDOOR ) ) {
    send( ch, "There is no door %s and you can't bash an open exit.\n\r",
      dir_table[ exit->direction ].where );
    return;
    }

  if( !is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "%s isn't closed and thus no point in bashing it.\n\r",
      exit->name );
    return;
    }

  if(  exit->direction  == DIR_UP ||  exit->direction  == DIR_DOWN ) {
    send( ch, "You can't effectively bash doors in the ceiling or\
 ground.\n\r" );
    return;
    }

  fsend( ch, "You take a running charge and throw yourself at %s,\
 but do no damage to it.", exit->name );
  fsend_seen( ch, "%s takes a running charges and throws %sself at %s,\
 but does no damage to it.", ch, ch->Him_Her( ), exit->name );
} 


/*
 *   ONLINE EDITING ROUTINES
 */


void do_dedit( char_data* ch, char* argument )
{
  wizard_data*    imm  = (wizard_data*) ch;
  exit_data*     exit;
  int           flags;
  int             dir;

  if( !get_flags( ch, argument, &flags, "1", "dedit" ) )
    return;

  if( matches( argument, "delete" ) ) {
    if( ( exit = (exit_data*) one_thing( ch, argument, "dedit delete", 
      (thing_array*) &ch->in_room->exits ) ) != NULL )
      delete_exit( imm, exit, is_set( &flags, 0 ) );
    return;
    }

  if( matches( argument, "new" ) ) {
    if( ( dir = direction_arg( argument ) ) != -1 ) 
      create_exit( imm, dir, argument, is_set( &flags, 0 ) );
    return;
    }

  if( ( exit = (exit_data*) one_thing( ch, argument, "dedit", 
    (thing_array*) &ch->in_room->exits ) ) == NULL )
    return;

  imm->exit_edit = exit;

  send( ch, "Dflag and dset now act on the exit %s.\n\r",
    dir_table[ exit->direction ].where );
}


void delete_exit( wizard_data* imm, exit_data* exit, bool one_way )
{
  exit_data* back;

  if( !can_edit( imm, imm->in_room ) ) 
    return;

  if( ( back = reverse( exit ) ) != NULL && !one_way ) {
    if( !can_edit( imm, exit->to_room, FALSE ) ) {
      send( imm,
        "You don't have permission to delete the return exit.\n\r" );
      return;
      }
    imm->exit_edit = back;
    extract( imm, offset( &imm->exit_edit, imm ), "exit" );
    exit->to_room->exits -= back;
    delete back;
    }

  imm->exit_edit = exit;
  extract( imm, offset( &imm->exit_edit, imm ), "exit" );
  imm->in_room->exits -= exit;
  delete exit;

  send( imm, "You remove %sthe exit %s.\n\r",
    back != NULL && one_way ? "just this side of " : "",
    dir_table[ exit->direction ].where );

  return;
}


void create_exit( wizard_data* ch, int dir, char* argument, bool one_way )
{
  room_data*  room  = ch->in_room;
  exit_data*  exit;

  if( exit_direction( room, dir ) != NULL ) {
    send( ch, "An exit already exists %s.\n\r",
      dir_table[ dir ].where );
    return;
    }

  if( *argument == '\0' ) {
    if( ( room = create_room( ch ) ) == NULL )
      return;
    }
  else {
    if( ( room = get_room_index( atoi( argument ), FALSE ) ) == NULL ) {
      send( ch, "No room with that vnum exists.\n\r" );
      return;
      }
    if( !one_way
      && exit_direction( room, dir_table[dir].reverse ) != NULL ) {
      send( ch, "Door returning already exists.\n\r" );  
      return;
      }
    if( !can_edit( ch, room, FALSE ) ) {
      send( ch, "You don't have permission to tunnel into that area.\n\r" );
      return;
      }
    }

  exit                        = add_exit( ch->in_room, dir );
  exit->to_room               = room;
  ch->exit_edit               = exit;
  ch->in_room->area->modified = TRUE;

  if( !one_way ) {
    exit                 = add_exit( room, dir_table[dir].reverse );
    exit->to_room        = ch->in_room;
    room->area->modified = TRUE;
    }

  send( ch, "%s-way exit %s to room %d added.\n\r",
    one_way ? "One" : "Two", dir_table[dir].name, room->vnum );
}


room_data* create_room( char_data* ch )
{
  room_data*  room;
  int         vnum  = 1;

  for( vnum = ch->in_room->area->room_first->vnum+1; ; vnum++ ) {
    if( ( room = get_room_index( vnum, FALSE ) ) == NULL )
      break;
    if( room->area != ch->in_room->area ) {
      send( ch, "Area is out of numbers.\n\r" );
      return NULL;
      }
    }

  room               = new room_data;
  room->area         = ch->in_room->area;
  room->vnum         = vnum;
  room->name         = alloc_string( ch->in_room->name, MEM_ROOM );
  room->description  = alloc_string( "Under Construction.\n\r", MEM_ROOM );
  room->comments     = empty_string;
  room->room_flags   = ch->in_room->room_flags;
  room->sector_type  = ch->in_room->sector_type;

  append( ch->in_room->area->room_first, room );

  return room;
}


void do_dflag( char_data* ch, char* argument )
{
  wizard_data*   imm  = (wizard_data*) ch; 
  exit_data*    exit;
  exit_data*    back;
  int          flags;

  if( !get_flags( ch, argument, &flags, "1", "dflag" ) )
    return;

  if( ( exit = imm->exit_edit ) == NULL ) {
    send( ch, "Use dedit to specify direction.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    display_flags( "Door Flags", &dflag_name[0], &dflag_name[1],
      &exit->exit_info, MAX_DFLAG, ch );
    return;
    }

  if( !can_edit( ch, ch->in_room ) ) 
    return;

  ch->in_room->area->modified = TRUE;

  for( int i = 0; i < MAX_DFLAG; i++ ) 
    if( fmatches( argument, dflag_name[i] ) ) {
      switch_bit( &exit->exit_info, i );
      if( ( back = reverse( exit ) ) != NULL
        && !is_set( &flags, 0 ) )
        assign_bit( &back->exit_info, i, is_set( &exit->exit_info,i ) );
      send( ch, "%s set %s on %sthe %s exit.\n\r",
        dflag_name[i], true_false( &exit->exit_info, i ),
        back != NULL && !is_set( &flags, 0 ) ? "both sides of " : "",
        dir_table[ exit->direction ].name );
      return;
      }

  send( ch, "Unknown flag - see dflag with no arguments for list.\n\r" );
}


void do_dstat( char_data* ch, char* argument )
{
  wizard_data*      imm  = (wizard_data*) ch;
  exit_data*       exit;
  obj_clss_data*    key;

  if( *argument == '\0' ) { 
    if( ( exit = imm->exit_edit ) == NULL ) {
      send( ch, "Use dedit to specify direction.\n\r" );
      return;
      }
    }
  else 
    if( ( exit = (exit_data*) one_thing( ch, argument, "dstat",
      (thing_array*) &ch->in_room->exits ) ) == NULL )
      return;
   
  send( ch, "[%s]\n\r", dir_table[ exit->direction ].name );
  send( ch, "  Leads to : %s (%d)\n\r",
    exit->to_room->name, exit->to_room->vnum );

  if( ( key = get_obj_index( exit->key ) ) == NULL )  
    send( ch, "       Key : None\n\r" );
  else  
    send( ch, "       Key : %s (%d)\n\r", key->Name( ), exit->key );

  send( ch, "      Name : %s\n\r",   exit->name );
  send( ch, "  Keywords : %s\n\r",   exit->keywords );
  send( ch, "  Strength : %d\n\r",   exit->strength );
  send( ch, "     Light : %d%%\n\r", exit->light );
  send( ch, "      Size : %s\n\r",   size_name[ exit->size ] );
}


void do_dset( char_data* ch, char* argument )
{ 
  wizard_data*   imm  = (wizard_data*) ch;
  exit_data*    exit;
  obj_data*      obj;
  int          flags;

  if( !get_flags( ch, argument, &flags, "1", "dset" ) )
    return;

  if( *argument == '\0' ) {
    do_dstat( ch, "" );
    return;
    }
  
  if( !can_edit( ch, ch->in_room ) )
    return;

  if( ( exit = imm->exit_edit ) == NULL ) {
    send( ch, "You must specify a direction with dedit.\n\r" );
    return;
    }

  ch->in_room->area->modified = TRUE;

  if( matches( argument, "key" ) ) {
    if( *argument == '\0' ) {
      exit->key = -1;
      send( ch, "Key for lock on %s exit set to none.\n\r",
        dir_table[ exit->direction ].name );
      return;
      }
    if( ( obj = one_object( ch, argument,
      "key", &ch->contents ) ) == NULL )
      return;
    if( obj->pIndexData->item_type != ITEM_KEY ) {
      send( ch, "%s isn't a key.\n\r", obj );
      return;
      }  
    exit->key = obj->pIndexData->vnum;
    send( ch, "Key for lock on %s exit set to %s.\n\r",
      dir_table[ exit->direction ].name, obj->pIndexData->Name( ) );
    return;
    }

  class type_field type_list[] = {
    { "size",  MAX_SIZE,  &size_name[0],  &size_name[1],  &exit->size },
    { "",      0,         NULL,           NULL,           NULL        }
    };

  if( process( type_list, ch, dir_table[ exit->direction ].name,
    argument ) )
    return;

  class string_field string_list[] = {
    { "name",      MEM_EXIT,  &exit->name,       NULL },
    { "keywords",  MEM_EXIT,  &exit->keywords,   NULL },
    { "",          0,         NULL,              NULL },   
    };

  if( process( string_list, ch, dir_table[ exit->direction ].name,
    argument ) )
    return;

  class int_field int_list[] = {
    { "strength",          0,   10,  &exit->strength  },
    { "light",             0,  100,  &exit->light     },
    { "",                  0,    0,  NULL             }
    };

  if( process( int_list, ch, dir_table[ exit->direction ].name,
    argument ) )
    return;

  send( ch, "Unknown field - See help dset.\n\r" );
}


/*
 *   SPELLS
 */


bool spell_wizard_lock( char_data*, char_data*, void*, int, int )
{
  return TRUE;
}








