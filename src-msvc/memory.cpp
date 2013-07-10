#include "define.h"
#include "struct.h"


const char* memory_name [ MAX_MEMORY ] = { "Unknown", "Accounts",
  "Actions", "Affects", "Aliases", "Areas", "Arrays", "Auction",
  "Bad Names", "Bans", "Clans", "Customs", "Char. Descr.",
  "Enemies", "Event",
  "Exits", "Extras", "Helps", "Info", 
  "Links", "Memory", "Mobs", "Mob_Progs",
  "Notes", "Obj_Classes", "Objects", "Obj_Progs",
  "Pfiles", "Players", "Programs", "Quests", "Queue",
  "Recognize", "Requests",
  "Resets", "Rooms", "Shdata", "Shops", "Species",
  "Spells", "Tables", "Tells", "Tracks", "Trainers",
  "Wizards" };


int memory_number [ 2*MAX_MEMORY-1 ];
int memory_size   [ 2*MAX_MEMORY-1 ];


/*
 *   RECORD ROUTINES
 */


void record_new( const int size, const int type ) 
{
  memory_number [ MAX_MEMORY+type-1 ]++;
  memory_size   [ MAX_MEMORY+type-1 ] += size;
};


void record_delete( const int size, const int type )
{
  memory_number [ MAX_MEMORY+type-1 ]--;
  memory_size   [ MAX_MEMORY+type-1 ] -= size;
};


void do_memory( char_data* ch, char* )
{
  char    tmp  [ TWO_LINES ];
  int  number;
  int  blocks;
  int   bytes  = 0;
  int i;

  for( i = 0; i < 2*MAX_MEMORY-1; i++ )
    bytes += memory_size[i];

  page( ch, "      Mobs: %d\r\n", mob_list.size );
  page( ch, "   Objects: %d\r\n", obj_list.size );
  page( ch, "   Players: %d\r\n", player_list.size );
  page( ch, " Extracted: %d\r\n", extracted.size );
  page( ch, "\r\n" );

  page( ch, "Memory Allocated: %dk\r\n\r\n", bytes/1024 );

  sprintf( tmp, "%15s %8s %8s %8s\r\n", "Structure", "Number",
    "Blocks", "Bytes" );
  page_underlined( ch, tmp );

  for( i = 0; i < MAX_MEMORY; i++ ) {
    number = memory_number[ MAX_MEMORY+i-1];
    blocks = number;
    bytes  = memory_size[ MAX_MEMORY+i-1 ];
    if( i != 0 ) { 
      blocks += memory_number[ MAX_MEMORY-i-1 ];
      bytes  += memory_size[ MAX_MEMORY-i-1 ];
      }
    sprintf( tmp, "%15s %8d %8d %8d\r\n",
      memory_name[i], number, blocks, bytes );
    page( ch, tmp );
    }
}


/*
 *   INIT MEMORY
 */
 

void init_memory( void )
{
  for( int i = 0; i < 2*MAX_MEMORY-1; i++ ) {
    memory_number[i] = 0;
    memory_size[i] = 0;
    }
}


/*
 *   STRINGS 
 */


char* alloc_string( const char* string, int type )
{
  char*  string_new;
  int        length  = strlen( string )+1;

  if( length == 1 )
    return empty_string;

  string_new = new char[length];
  memcpy( string_new, string, length );

  record_new( length, -type );

  return string_new;
}


void free_string( char* string, int type )
{
  if( string != NULL && string != empty_string ) {
    record_delete( strlen( string )+1, -type );
    delete [] string;
    }
}


/*
 *   EXTRACT EDITED OBJECTS
 */


void extract( wizard_data* imm, int offset, const char* text )
{
  char             tmp  [ TWO_LINES ];
  player_data*      ch;
  void**          edit;
  void**          pntr; 

  sprintf( tmp, "** %s just deleted the %s you were editing. **\r\n",
    imm->real_name( ), text );
  edit = (void**)( int( imm )+offset );

  for( int i = 0; i < player_list; i++ ) {
    ch = player_list[i];
    if( !ch->Is_Valid( )
      || ch == imm || wizard( ch ) == NULL )
      continue;
    pntr = (void**)( int( ch )+offset );
    if( *edit == *pntr ) {
      *pntr = NULL;
      send( ch, tmp );
      }
    }

  *edit = NULL;
}


