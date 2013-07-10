#include "ctype.h"
#include "errno.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "syslog.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"


/*
 *   FILE SUBROUTINES
 */


FILE* open_file( const char* dir, const char* file, const char* type,
  bool fatal )
{
  char  tmp  [ TWO_LINES ];

  sprintf( tmp, "%s%s", dir, file );

  return open_file( tmp, type, fatal );
}


FILE* open_file( const char* path, const char* type, bool fatal )
{
  FILE*   fp;

  if( ( fp = fopen( path, type ) ) != NULL )
    return fp;

  roach( "Open_File: %s", strerror( errno ) );
  roach( "-- File = '%s'", path );

  if( fatal )
    exit( 1 );

  return NULL;
}


void delete_file( const char* dir, const char* file, bool msg )
{
  char* path  = static_string( );
  int      i  = strlen( dir );

  sprintf( path, "%s%s", dir, file );
  path[i] = toupper( path[i] );

  if( unlink( path ) == -1 && msg ) {
    roach( "Delete_File: %s", strerror( errno ) );
    roach( "-- File = '%s'", path );
    }

  return;
}


void rename_file( const char* dir1, const char* file1,
  const char* dir2, const char* file2 )
{
  char* tmp1  = static_string( );
  char* tmp2  = static_string( );

  sprintf( tmp1, "%s%s", dir1, file1 );
  sprintf( tmp2, "%s%s", dir2, file2 );

  unlink( tmp2 );
  link( tmp1, tmp2 );
  unlink( tmp1 );

  return;
}


/*
 *   FREAD ROUTINES
 */


char fread_letter( FILE *fp )
{
  char c;

  do {
    c = getc( fp );
    } while( isspace( c ) );

  return c;
}


int fread_number( FILE *fp )
{
  int number = 0;
  int sign = 1;
  char c;

  do {
    c = getc( fp );
    } while( isspace( c ) );

  switch( c ) {
    case '-' :  sign = -1;
    case '+' :  c = getc( fp );
    }

  if( !isdigit( c ) ) {
    bug( "Fread_number: bad format." );
    printf( "%s\n", fread_string( fp, MEM_UNKNOWN ) );
    exit( 1 );
    }

  while( isdigit( c ) ) {
   number = number * 10 + c - '0';
   c = getc( fp );
   }

  number *= sign;

  while( c == ' ' ) {
    c = getc( fp );
    }

  ungetc( c, fp );

  return number;
}


char* fread_string( FILE* fp, int type )
{
  char        buf  [ 4*MAX_STRING_LENGTH ];
  int      length  = 0;
  char*    string;
 
  do {
    buf[0] = getc( fp );
    } while( buf[0] == '\n' || buf[0] == '\r' || buf[0] == ' ' );

  if( buf[0] == '.' )
    buf[0] = getc( fp );

  for( ; buf[ length ] != '~' && buf[ length ] != EOF; ) 
    buf[ ++length ] = getc( fp );    

  if( buf[ length ] == EOF ) 
    panic( "Fread_string: EOF" ); 

  buf[ length ] = '\0';
  string = alloc_string( buf, type );

  return string;
}


void fread_to_eol( FILE *fp )
{
  char c;

  do {
    c = getc( fp );
    } while( c != '\n' && c != '\r' && c != EOF );

  do {
    c = getc( fp );
    } while( c == '\n' || c == '\r' );

  ungetc( c, fp );
  return;
}


char *fread_word( FILE *fp )
{
  static char    buf  [ MAX_STRING_LENGTH ];
  char*        pWord;
  char        letter;
  char             c;

  do {
    letter = getc( fp );
    } while( isspace( letter ) );

  pWord = buf;

  if( letter != '\'') {
    *buf = letter;
    pWord++;
    }

  for( ; pWord < &buf[ MAX_STRING_LENGTH ]; pWord++ ) {
    *pWord = getc( fp );
    if( ( isspace( *pWord ) && letter != '\'' )
      || ( *pWord == '\'' && letter == '\'' ) ) {
      *pWord = '\0';
      do {
        c = getc( fp );
        } while( c == ' ' );
      ungetc( c, fp );
      return buf;
      }
    }
  
  bug( "Fread_word: word too long." );
  printf( "%s\n\r", buf );

  exit( 1 );

  return NULL;
}


char *fread_block( FILE *fp )
{
  static char buf[ MAX_STRING_LENGTH ];
  char *pWord;
  char letter, c;

  do {
    letter = getc( fp );
    } while( isspace( letter ) );

  pWord = buf;

  *buf = letter;
  pWord++;

  for( ; pWord < &buf[ MAX_STRING_LENGTH ]; pWord++ ) {
    *pWord = getc( fp );
    if( isspace( *pWord ) ) {
      *pWord = '\0';
      do {
        c = getc( fp );
        } while( c == ' ' );
      ungetc( c, fp );
      return buf;
      }
    }
  
  bug( "Fread_block: word too long." );
  printf( "%s\n\r", buf );
  exit( 1 );

  return NULL;
}


/*
 *   CHANGES
 */


void do_changes( char_data* ch, char* )
{
  species_data*   species;
  int                   i;
 
  do_help( ch, "Changes_Disclaimer" );

  page( ch, "Monsters:\n\r" );

  for( i = 0; i < MAX_SPECIES; i++ ) 
    if( ( species = species_list[i] ) != NULL ) 
      if( species->date > current_time-5*24*60*60 ) 
        page( ch, "  %s\n\r", species->Name( ) );

  return;
}


/*
 *   DO_WRITE ROUTINE
 */


void write_all( bool forced )
{
  area_data*  area;

  for( area = area_list; area != NULL; area = area->next )
    save_area( area, forced );

  save_mobs( );
  save_objects( );
  save_help( );
  save_trainers( );
  save_banned( );
  save_quests( );
  save_shops( );
  save_tables( );
  save_rtables( );
  save_lists( );
  save_clans( );
}


void do_load( char_data* ch, char* argument )
{
  if( *argument == '\0' ) {
    send( ch, "Which file do you wish to load?\n\r" );
    return;
    }

  if( matches( argument, "tables" ) ) {
    load_tables( );
    return;
    }

  send( ch, "Unknown file.\n\r" );
  return;
}


void do_write( char_data* ch, char* argument )
{
  char           buf  [ MAX_INPUT_LENGTH ];
  area_data*    area;
  int         number  = 0;
  int          flags;

  if( !get_flags( ch, argument, &flags, "f", "write" ) )
    return;

  if( !strcasecmp( argument, "all" ) ) {
    if( !has_permission( ch, PERM_WRITE_ALL, TRUE ) ) 
      return;
    write_all( is_set( &flags, 0 ) );
    send( ch, "All files written.\n\r" );
    sprintf( buf, "All files written (%s).", ch->descr->name );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( fmatches( argument, "areas" ) ) {
    if( !has_permission( ch, PERM_WRITE_AREAS, TRUE ) )
      return;
    for( area = area_list; area != NULL; area = area->next ) 
      number += save_area( area, is_set( &flags, 0 ) );
    if( number > 0 ) {
      send( ch, "All areas written. ( %d file )\n\r", number );
      sprintf( buf, "All areas written (%s).", ch->real_name() );
      info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
      }
    else {
      send( ch, "No area needed saving.\n\r" );
      }
    return;
    }

  if( fmatches( argument, "rtables" ) ) {
    if( !has_permission( ch, PERM_RTABLES, TRUE ) ) 
      return;
    save_rtables( );
    send( ch, "Rtables written.\n\r" );
    sprintf( buf, "Rtables written (%s).", ch->real_name() );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( fmatches( argument, "mobs" ) ) {
    if( !has_permission( ch, PERM_MOBS, TRUE ) )
      return;
    save_mobs( );
    send( ch, "Ok.\n\r" );
    sprintf( buf, "Mob file written (%s).", ch->real_name() );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( fmatches( argument, "objects" ) ) {
    if( !has_permission( ch, PERM_OBJECTS, TRUE ) )
      return;
    save_objects( );
    send( ch, "Ok.\n\r" );
    sprintf( buf, "Object file written (%s).", ch->real_name() );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "notes" ) ) {
    save_notes( -1 );
    send( ch, "All noteboards written.\n\r" );
    return;
    }

  if( !strcasecmp( argument, "shops" ) ) {
    if( !has_permission( ch, PERM_ROOMS, TRUE ) )
      return;
    save_shops( );
    send( "Ok.\n\r", ch );
    sprintf( buf, "Shop file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf , IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "tables" ) ) {
    if( !has_permission( ch, PERM_MISC_TABLES )
      && !has_permission( ch, PERM_SOCIALS, TRUE ) ) 
      return;
    save_tables( );
    send( "Ok.\n\r", ch );
    sprintf( buf, "Table file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "clans" ) ) {
    if( !has_permission( ch, PERM_CLANS, TRUE ) )
      return;
    save_clans( );
    send( ch, "Ok.\n\r" );
    sprintf( buf, "Clan files written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "lists" ) ) {
    if( !has_permission( ch, PERM_LISTS, TRUE ) ) 
      return;
    save_lists( );
    send( "Ok.\n\r", ch );
    sprintf( buf, "Table file written (%s).", ch->real_name() );
    info( "", LEVEL_IMMORTAL, buf , IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "trainers" ) ) {
    if( !IS_IMMORTAL( ch ) ) {
      send( "You don't have permission.", ch );
      return;
      }
    save_trainers( );
    send( "Ok.\n\r", ch );
    sprintf( buf, "Trainer file written (%s).", ch->real_name() );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "help" ) ) {
    if( !has_permission( ch, PERM_HELP_FILES, TRUE ) )
      return;
    if( !save_help( ch ) ) 
      send( ch, "Help was not modified so was not saved.\n\r" );
    return;
    }

  if( !strcasecmp( argument, "quests" ) ) {
    if( get_trust( ch ) < LEVEL_QUEST ) {
      send( ch, "You don't have permission." );
      return;
      }
    save_quests( );
    send( ch, "Ok.\n\r" );
    sprintf( buf, "Quest file written (%s).", ch->real_name() );
    info( "", LEVEL_BUILDER, buf, IFLAG_WRITES, 1, ch );
    return;
    }

  if( !strcasecmp( argument, "w3" ) ) {
    w3_help( );
    send( ch, "Webpage Updated.\n\r" );
    return;
    }

  if( *argument != '\0' ) {
    send( ch, "What do you want to write?\n\r" );
    return;
    }

  if( can_edit( ch, ch->in_room ) ) {
    if( !ch->in_room->area->modified ) {
      send( "Area has not been modified so was not saved.\n\r", ch );
      }
    else {
      save_area( ch->in_room->area );
      send( ch, "Area written.\n\r" );
      }
    }
}

