#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"


const char* rflag_name[ MAX_RFLAG ] = { "Lit", "Safe", "Indoors", "No.Mob",
  "No.Recall", "No.Magic", "No.Autoscan", "Altar", "Altar.Good",
  "Altar.Neutral", "Altar.Evil", "Bank", "Shop", "Pet.Shop", "Office",
  "No.Pray", "Save.Items", "Underground", "Auction.House",
  "Reset0", "Reset1", "Reset2", "Status0", "Status1", "Status2",
  "No.Mount", "Arena", "Donation", "No.Pkill" };

const char* location_name[ MAX_LOCATION ] = { "!Indoors", "!Outside",
  "Sunlight", "Full Moon", "Forest", "!Underwater" };

int max_location = MAX_LOCATION;

flag_data location_flags = { "Location", &location_name[0],
  &location_name[1], &max_location };

const class terrain_type terrain [ MAX_TERRAIN ] =
{
  {  "Room",              1  },
  {  "Town Street",       1  },
  {  "Field",             2  },
  {  "Forest",            2  },
  {  "Hills",             3  },
  {  "Mountain",          4  },
  {  "Water",             3  },
  {  "Underwater",        5  },
  {  "River",             5  },
  {  "Air",               6  },  
  {  "Desert",            4  },
  {  "Cave",              4  },
  {  "Road",              2  }, 
  {  "Shallows",          4  },
};


/*
 *   LOCAL ROUTINES
 */


void load_room_items( room_data* );


/*
 *   DISK ROUTINES
 */


void load_rooms( FILE* fp, area_data* area )
{
  room_data *room;
  room_data *room2;

  if( strcmp( fread_word( fp ), "#ROOMS" ) )
    panic( "Load_rooms: missing header" );

  for( ; ; ) {
    int vnum;
    char letter;

    letter = fread_letter( fp );

    if( letter != '#' ) 
      panic( "Load_rooms: # not found." );

    if( ( vnum = fread_number( fp ) ) == 0 )
       break;

    if( get_room_index( vnum ) != NULL ) 
      panic( "Load_rooms: vnum %d duplicated.", vnum );

    room               = new room_data;
    room->area         = area;
    room->vnum         = vnum;
    room->name         = fread_string( fp, MEM_ROOM );
    room->description  = fread_string( fp, MEM_ROOM );
    room->comments     = fread_string( fp, MEM_ROOM );
    room->room_flags   = fread_number( fp );
    room->sector_type  = fread_number( fp );
    room->size         = fread_number( fp );
    room->reset        = NULL;

    if( room->size < 0 || room->size >= MAX_SIZE )
      room->size = SIZE_HORSE;

    fread_number( fp );

    read_exits( fp, room, vnum );
    read_extra( fp, room->extra_descr );
        
    for( ; ; ) {
      letter = fread_letter( fp );

      if( letter == 'S' )
        break;
  
      if( isdigit( letter ) || letter == '-' ) {
        ungetc( letter, fp );
        load( fp, room->reset );
        continue;
        }

      if( letter == 'A' ) {
        read( fp, room->action, room );
        continue;
        }
      
      panic( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
      }
         
    if( ( room2 = room->area->room_first ) == NULL ) {
      room->area->room_first = room;
      room->next = NULL;
      }
    else {
      if( room2->vnum > room->vnum ) {
        room->next = room2;
        room->area->room_first = room;
        }
      else {
        for( ; room2->next != NULL && room2->next->vnum <
          room->vnum; room2 = room2->next );
        room->next = room2->next;
        room2->next = room;
        }
      }
    }

  return;
}


/*
 *   SAVE/LOAD ROOM ITEMS
 */


void save_room_items( room_data* room )
{
  if( !is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) )
    return;

  FILE*        fp;
  char*      file  = static_string( );

  sprintf( file,   "Room.%d", room->vnum );
  rename_file( ROOM_DIR, file, ROOM_PREV_DIR, file );

  if( ( fp = open_file( ROOM_DIR, file, "w" ) ) == NULL ) 
    return;

  write_object( fp, room->contents );
  fclose( fp );
}  


void load_room_items( )
{
  area_data*  area;
  room_data*  room;

  fprintf( stdout, "Loading Room Items ...\n" ); 

  for( area = area_list; area != NULL; area = area->next )
    for( room = area->room_first; room != NULL; room = room->next ) 
      if( is_set( &room->room_flags, RFLAG_SAVE_ITEMS ) ) 
        load_room_items( room );

  return;
}


void load_room_items( room_data* room )
{ 
  FILE*        fp;
  char*      file  = static_string( );  
  char*       tmp  = static_string( );  
  bool       flag;    
           
  sprintf( file, "Room.%d", room->vnum );

  if( ( fp = open_file( ROOM_DIR, file, "r", FALSE ) ) != NULL ) {
    flag = read_object( fp, room->contents, tmp );
    fclose( fp );
    if( flag )
      return;  
    roach( "Load_Room_Items: Room %d corrupted.", room->vnum );
    }

  if( ( fp = open_file( ROOM_PREV_DIR, file, "r", FALSE ) ) == NULL )
    return; 

  if( !read_object( fp, room->contents, tmp ) ) 
    panic( "Previous file also corrupted!" );            

  fclose( fp );
}


/*
 *   GET_ROOM_INDEX FUNCTION
 */


room_data* get_room_index( int vnum, bool err )
{
  area_data*  area;
  room_data*  room;

  for( area = area_list; area != NULL; area = area->next ) {
    if( area->next != NULL && area->next->room_first != NULL 
      && area->next->room_first->vnum <= vnum )
      continue; 
    for( room = area->room_first; room != NULL;
      room = room->next )
      if( room->vnum == vnum )
        return room;
    }  

  if( err ) 
    panic( "Get_room_index: bad vnum %d.", vnum );

  return NULL;
}


/*
 *   SUPPORT ROUTINES
 */


bool can_edit( char_data* ch, room_data* room, bool msg )
{
  if( has_permission( ch, PERM_ALL_ROOMS )
    || is_name( ch->descr->name, room->area->creator ) )
    return TRUE;

  if( msg )
    send( ch, "You don't have permission to alter this room.\n\r" );

  return FALSE;
}


const char* room_name( room_data* room )
{
  static char tmp [ 15 ];  

  if( room == NULL )
    return "nowhere??";

  sprintf( tmp, "Room #%d", room->vnum );
  return tmp;
}
  

/*
 *   ONLINE ROOM COMMANDS
 */


void do_rbug( char_data* ch, char* argument ) 
{
  ch->in_room->area->modified = TRUE;

  ch->in_room->comments = edit_string( ch, argument, ch->in_room->comments,
    MEM_ROOM );
}


void do_rdesc( char_data *ch, char *argument ) 
{
  room_data*      room  = ch->in_room;
  wizard_data*  wizard  = (wizard_data*) ch;
 
  if( argument[0] != '\0' && !can_edit( ch, room ) ) 
    return;

  room->area->modified = TRUE;

  if( wizard->room_edit == NULL ) 
    room->description = edit_string( ch,
      argument, room->description, MEM_ROOM );
  else 
    wizard->room_edit->text = edit_string( ch, 
      argument, wizard->room_edit->text, MEM_EXTRA );
}

    
void do_rflag( char_data* ch, char* argument )
{
  int              flags;
  int               prev;
  room_data*        room  = ch->in_room;
  const char*   response;

  if( !get_flags( ch, argument, &flags, "a", "Rflag" ) )
    return;;

  if( *argument == '\0' ) {
    display_flags( "Room", &rflag_name[0],
      &rflag_name[1], &room->room_flags, MAX_RFLAG, ch );
    return;
    }
     
  if( !can_edit( ch, room ) )
    return;

  prev = room->room_flags;

  if( ( response = set_flags( &rflag_name[0], &rflag_name[1],
    &room->room_flags, MAX_RFLAG, NULL, ch, argument,
    FALSE, TRUE ) ) == NULL )  
    return;

  room_log( ch, ch->in_room->vnum, response );
  room->area->modified = TRUE;

  if( !is_set( &flags, 0 ) )
    return;

  for( room = room->area->room_first; room != NULL; room = room->next ) 
    alter_flags( &room->room_flags, &ch->in_room->room_flags,
      &prev, MAX_RFLAG );

  send( ch, "- Set on Area -\n\r" );
}


void do_rset( char_data* ch, char* argument )
{
  room_data*    room  = ch->in_room;
  area_data*    area  = room->area;
  int          flags;
  int           terr;
  int           size;

  if( !get_flags( ch, argument, &flags, "a", "Rset" ) )
    return;;
  
  if( *argument == '\0' ) {
    do_rstat( ch, "" );
    return;
    }

  if( *argument != '\0' ) {
    if( !can_edit( ch, room ) ) 
      return;
    ch->in_room->area->modified = TRUE;
    }

  class string_field string_list [] = {
    { "area",      MEM_AREA,  &area->name,     NULL            },
    { "creator",   MEM_AREA,  &area->creator,  NULL            },
    { "help",      MEM_AREA,  &area->help,     NULL            },
    { "filename",  MEM_AREA,  &area->file,     &set_area_file  },
    { "",          0,         NULL,            NULL            },   
    };

  if( process( string_list, ch, area->name, argument ) )
    return;

  class int_field int_list [] = {
    { "level",            1,     90,  &area->level       },
    { "reset time",       0,    200,  &area->reset_time  },
    { "",                 0,      0,  NULL               }, 
    };
               
  if( process( int_list, ch, area->name, argument ) )
    return;

  terr  = room->sector_type;
  size  = room->size;

#define as( i )   area_status[i]
#define tn( i )   terrain[i].name
#define sn( i )   size_name[i]

  class type_field type_list [] = {
    { "status",  MAX_AREA_STATUS,  &as(0),  &as(1),  &area->status       },
    { "terrain", MAX_TERRAIN,      &tn(0),  &tn(1),  &room->sector_type  },
    { "size",    MAX_SIZE,         &sn(0),  &sn(1),  &room->size         }, 
    { "",        0,                NULL,    NULL,    NULL                }
    };

#undef as
#undef tn
#undef sn

  if( process( type_list, ch, is_set( &flags, 0 ) ? area->name : "room",
    argument ) ) {
    if( is_set( &flags, 0 ) ) {
      if( room->size != size ) {
        size = room->size;
        for( room = room->area->room_first; room != NULL; room = room->next ) 
          room->size = size;
        }
      else if( room->sector_type != terr ) {
        terr = room->sector_type;
        for( room = room->area->room_first; room != NULL; room = room->next ) 
          room->sector_type = terr;
        }
      }
    return;
    }

  send( ch, "Syntax: rset <field> ....\n\r" );
}


void do_rname( char_data* ch, char* argument ) 
{
  if( !can_edit( ch, ch->in_room ) ) 
    return;

  if( *argument == '\0' ) {
    send( ch, "You need to call the room something.\n\r" );
    return;
    }

  ch->in_room->area->modified = TRUE;

  free_string( ch->in_room->name, MEM_ROOM );
  ch->in_room->name = alloc_string( argument, MEM_ROOM );
  send( "Room name changed.\n\r", ch );
}


void do_rstat( char_data* ch, char* )
{
  char           tmp  [ MAX_STRING_LENGTH ];
  room_data*    room  = ch->in_room;
  area_data*    area  = room->area;
  bool         found;

  page( ch, "        Name: %s.\n\r", room->name );
  page( ch, "        Area: %s\n\r", area->name );
  page( ch, "    Filename: %s%s.are\n\r", AREA_DIR, area->file );
  page( ch, "     Creator: %-12s\n\r", area->creator );
  page( ch, "        Vnum: %d\n\r", room->vnum );
  page( ch, "       Light: %-14d Weight: %.2f\n\r",
    room->Light( ), (float) room->contents.weight/100 );
  page( ch, "  Reset Rate: %-15d Level: %d\n\r",
    area->reset_time, area->level );
  page( ch, "     Terrain: %-12s Max_Size: %-13s Status: %s\n\r",
    terrain[ room->sector_type ].name, size_name[room->size],
    area_status[ area->status ] );

  found = FALSE;
  strcpy( tmp, "       Exits:" );
  for( int i = 0; i < room->exits; i++ ) {
    sprintf( tmp+strlen( tmp ), " %s",
      dir_table[ room->exits[i]->direction ].name );
    found = TRUE;
    }
  sprintf( tmp+strlen( tmp ), "%s\n\r", found ? "" : " none" );
  page( ch, tmp );

  found = FALSE;
  strcpy( tmp, "  Exits From:" );
  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next )
      for( int i = 0; i < room->exits; i++ )
        if( room->exits[i]->to_room == ch->in_room ) {
          found = TRUE;
          sprintf( tmp+strlen( tmp ), " %d", room->vnum );          
	  }
  sprintf( tmp+strlen( tmp ), "%s\n\r", found ? "" : " none" );
  page( ch, tmp );

  room = ch->in_room;
  page( ch, scroll_line[0] );

  sprintf( tmp, "Description:\n\r%s", room->description );
  page( ch, tmp );

  show_extras( ch, room->extra_descr );
}


bool can_extract( room_data* room, char_data* ch )
{
  area_data*          area;
  trainer_data*    trainer;

  if( room->area->room_first == room ) {
    send( ch, "You can't delete the first room in an area.\n\r" );
    return FALSE;
    }

  if( room->exits > 0 ) {
    send( ch, "You must remove all doors from the room.\n\r" );
    return FALSE;
    }

  if( room->reset != NULL ) {
    send( ch, "Remove all resets first.\n\r" );
    return FALSE;
    }	

  for( trainer = trainer_list; trainer != NULL; trainer = trainer->next )
    if( trainer->room == room ) {
      send( ch, "Room still contains a trainer reference.\n\r" );
      return FALSE;
      }

  for( area = area_list; area != NULL; area = area->next ) 
    for( room = area->room_first; room != NULL; room = room->next )
      for( int i = 0; i < room->exits; i++ )  
        if( room->exits[i]->to_room == ch->in_room ) {
          send( ch, "Room %d still has a connection to this room.\n\r",
            room->vnum );
          return FALSE;
          }

  return TRUE;
}


void do_redit( char_data *ch, char *argument )
{
  char               buf   [ TWO_LINES ];
  char_data*         rch;
  wizard_data*    wizard  = (wizard_data*) ch;

  if( !can_edit( ch, ch->in_room ) ) 
    return;
 
  ch->in_room->area->modified = TRUE;

  if( matches( argument, "room" ) ) {
    wizard->room_edit = NULL;
    send( ch, "Rdesc now operates on current room.\n\r" );
    return;
    }

  if( !strcasecmp( argument, "delete room" ) ) {
    if( !can_extract( ch->in_room, ch ) )
      return;

    remove( ch->in_room->area->room_first, ch->in_room );

    sprintf( buf, "Room deleted: %d (%s)",
      ch->in_room->vnum, ch->real_name( ) );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES );
    send( ch, "You delete the room.\n\r" );
    
    for( int i = *ch->array-1; i >= 0; i-- ) {
      if( ( rch = player( ch->array->list[i] ) ) != NULL ) {
        send( rch, "The room you are in disappears.\n\r" );
        send( rch, "You find yourself in the god's chat room.\n\r" );
        rch->From( );
        rch->To( get_room_index( ROOM_CHAT ) );
        }
      }

    return;
    } 

  edit_extra( ch->in_room->extra_descr, wizard, offset( &wizard->room_edit,
    wizard ), argument, "rdesc" );
}


/*
 *   ROOM PROPERTIES
 */


bool Room_Data :: is_dark( )
{
  return FALSE;
}


/*
 *   LOCATION
 */


bool allowed_location( char_data* ch, int* bit, const char* msg1,
  const char* msg2 )
{
  if( is_set( bit, LOC_INDOORS )
    && is_set( &ch->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "You cannot %s %s inside.\n\r", msg1, msg2 );
    return FALSE;
    } 

  if( is_set( bit, LOC_OUTDOORS )
    && is_set( &ch->in_room->room_flags, RFLAG_INDOORS ) ) {
    send( ch, "You cannot %s %s outside.\n\r", msg1, msg2 );
    return FALSE;
    } 

  if( is_set( bit, LOC_SUNLIGHT ) && !isday( ) ) {
    send( ch, "You can only %s %s during the day.\n\r", msg1, msg2 );
    return FALSE;
    } 

  if( is_set( bit, LOC_FULLMOON ) && isday( ) ) {
    send( ch, "You can only %s %s while the moon is full.\n\r",
      msg1, msg2 );
    return FALSE;
    } 

  if( is_set( bit, LOC_FOREST ) 
    && ch->in_room->sector_type != SECT_FOREST ) {
    send( ch, "You can only %s %s while in a forest.\n\r", msg1, msg2 );
    return FALSE;
    }

  if( is_set( bit, LOC_UNDERWATER ) 
    && ch->in_room->sector_type == SECT_UNDERWATER ) {
    send( ch, "You cannot %s %s underwater.\n\r", msg1, msg2 );
    return FALSE;
    }

  return TRUE;
}




