#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "syslog.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"


const char**  badname_array  = NULL;
int             max_badname  = 0;


class ban_data
{
 public:
  ban_data*    next;
  char*        name;

  ban_data( );
  ~ban_data( );
};


ban_data* ban_list = NULL;


ban_data :: ban_data( )
{
  name = empty_string;

  record_new( sizeof( ban_data ), MEM_BAN );
  append( ban_list, this );
};


ban_data :: ~ban_data( )
{
  free_string( name, MEM_BAN );

  record_delete( sizeof( ban_data ), MEM_BAN );
  remove( ban_list, this );
};


/*
 *   BAN/ALLOW ROUTINES
 */


void do_ban( char_data* ch, char* argument )
{
  account_data*   account;
  ban_data*           ban;
  pfile_data*       pfile  = NULL;
  int               flags;
  int                   i;
  bool              first  = TRUE;
  const char*        name;
  player_data*     victim;

  if( !get_flags( ch, argument, &flags, "srn", "Ban" ) )
    return;

  if( is_set( &flags, 2 ) ) {
    if( *argument == '\0' ) {
      if( badname_array == NULL ) {
        send( ch, "The badname array is empty.\n\r" );
        }
      else {
        display_array( ch, "Bad Name Array", &badname_array[0],
          &badname_array[1], max_badname );
        }
      return;
      }
    i = pntr_search( badname_array, max_badname, argument );
    if( is_set( &flags, 1 ) ) {
      if( i < 0 ) {
        send( ch, "The name '%s' wasn't in the badname array.\n\r",
          argument );
        }
      else {
        record_delete( sizeof( char* ), MEM_BADNAME );
        remove( badname_array, max_badname, i );
        send( ch, "Name removed from badname array.\n\r" );
        save_badname( );
        }
      }
    else {
      if( i > 0 ) {
        send( ch, "The name '%s' is already in the badname array.\n\r",
          argument );
        }
      else if( check_parse_name( ch->link, argument ) ) {
        i = -i-1;
        record_new( sizeof( char* ), MEM_BADNAME );
        name = alloc_string( argument, MEM_BADNAME );
        insert( badname_array, max_badname, name, i );
        send( ch, "The name '%s' is added to the badname array.\n\r",
          name );
        save_badname( );
        }
      }
    return;
    }

  if( is_set( &flags, 0 ) ) {
    if( *argument == '\0' ) {
      page_title( ch, "Banned Sites" );
      for( ban = ban_list; ban != NULL; ban = ban->next )
        page( ch, "%s\n\r", ban->name );
      return;
      }
    if( !is_set( &flags, 1 ) ) {
      for( ban = ban_list; ban != NULL; ban = ban->next ) {
        if( !strcasecmp( argument, ban->name ) ) {
          send( "That site is already banned!\n\r", ch );
          return;
          }
        }
      ban        = new ban_data;
      ban->name  = alloc_string( argument, MEM_BAN );
      save_banned( );
      send( ch, "Ok.\n\r" );
      }
    else {
      for( ban = ban_list; ban != NULL; ban = ban->next ) {
        if( !strcasecmp( argument, ban->name ) ) {
          delete ban;
          save_banned();
          send( ch, "Ok.\n\r" );
          return;
          }
        }
      send( ch, "Site is not banned.\n\r" );
      }
    return;
    }

  if( *argument == '\0' ) {
    page_title( ch, "Banned Accounts" );
    for( i = 0; i < max_account; i++ )
      if( account_list[i]->banned != -1 ) 
        display_account( ch, account_list[i], first );
    return;
    }

  if( ( account = account_arg( argument ) ) == NULL ) {
    if( ( pfile = player_arg( argument ) ) == NULL ) {
      send( ch, "No such account or player.\n\r" );
      return;
      }
    if( ( account = pfile->account ) == NULL ) {
      send( ch, "That player doesn't have an account which makes banning\
 it difficult.\n\r" );
      return;
      }
    }

  if( is_set( &flags, 1 ) ) {
    if( account->banned == -1 ) {
      send( ch, "That account was not banned.\n\r" );
      }
    else {
      send( ch, "Ban lifted.\n\r" );
      account->banned = -1;
      save_accounts( );
      }
    return;
    }

  if( ch->pcdata->pfile->account == account ) {
    send( ch, "Banning yourself is rather phulish.\n\r" );
    return;
    }

  for( i = 0; i < max_pfile; i++ )
    if( pfile_list[i]->account == account 
      && pfile_list[i]->trust >= ch->pcdata->trust ) {
      send( ch, "You do not have permission to ban that account.\n\r" );
      return;
      }

  if( *argument == '\0' ) {
    send( ch, "For how what time period?\n\r" );
    return;
    }

  if( ( i = time_arg( argument, ch ) ) == -1 ) 
    return;

  if( i == 0 ) {
    account->banned = 0;
    send( ch, "Account %s banned forever.\n\r",
      account->name );
    }
  else {
    account->banned = current_time+i;
    send( ch, "Account %s banned until %s.\n\r",
      account->name, ltime( account->banned )+4 );
    }
 
  save_accounts( );

  for( i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( victim->Is_Valid( )
      && victim->pcdata->pfile->account == account ) {
      send( victim, "-- You have been banned. --\n\r" );
      forced_quit( victim );
      }
    }

  return;
}


/*
 *   IS_BANNED ROUTINE
 */


bool is_banned( const char* host )
{
  ban_data*       ban;
  int        host_len  = strlen( host );
  int         ban_len;

  for( ban = ban_list; ban != NULL; ban = ban->next ) { 
    ban_len = strlen( ban->name );
    if( ban_len <= host_len
      && !strcasecmp( &host[host_len-ban_len], ban->name ) ) 
      break;
    }

  return( ban != NULL );
}


bool is_banned( account_data* account, link_data* link )
{
  if( account == NULL || account->banned == -1 )
    return FALSE;

  if( account->banned != 0 && account->banned < current_time ) {
    account->banned = -1;
    return FALSE;
    }

  help_link( link, "Banned_Acnt" );

  if( account->banned != 0 ) 
    send( link, "Ban Expires: %s\n\r", ltime( account->banned ) );

  close_socket( link, TRUE );
  return TRUE;
}


/*
 *   READ/WRITE BADNAME
 */


void load_badname( void )
{
  FILE*           fp;
  const char*   name;

  fprintf( stderr, "Loading Bad Names...\n\r" );

  if( ( fp = fopen( BADNAME_FILE, "r" ) ) == NULL ) {
    perror( BADNAME_FILE );
    exit( 1 );
    }

  if( strcmp( fread_word( fp ), "#BADNAME" ) )
    panic( "Load_badname: missing header" );

  for( ; ; ) {
    name = fread_string( fp, MEM_BADNAME );
    if( *name == '$' ) 
      break;
    record_new( sizeof( char* ), MEM_BADNAME );
    insert( badname_array, max_badname, name, max_badname );
    }

  free_string( name, MEM_BADNAME );
  fclose( fp );

  return;
}


void save_badname( void )
{
  FILE*    fp;
  int       i;

  if( ( fp = fopen( BADNAME_FILE, "w" ) ) == NULL ) {
    bug( "Save_badname: fopen" );
    return;
    }

  fprintf( fp, "#BADNAME\n\n" );

  for( i = 0; i < max_badname; i++ )
    fprintf( fp, "%s~\n", badname_array[i] );

  fprintf( fp, "$~\n" );
  fclose( fp );

  return;
}


/*
 *   READ/WRITE BANNED
 */


void load_banned( void )
{
  ban_data*    ban;
  char*       name;
  FILE*         fp;

  fprintf( stderr, "Loading Banned Sites...\n\r" );

  if( ( fp = fopen( BANNED_FILE, "r" ) ) == NULL ) {
    perror( BANNED_FILE );
    exit( 1 );
    }

  if( strcmp( fread_word( fp ), "#BANNED" ) )
    panic( "Load_banned: missing header" );

  for( ; ; ) {
    name = fread_string( fp, MEM_BAN );
    if( *name == '$' )
      break;
    ban        = new ban_data;
    ban->name  = name;
    }

  free_string( name, MEM_BAN );
  fclose( fp );

  return;
}


void save_banned( )
{
  ban_data*  ban;
  FILE*      fp;

  fp = open_file( BANNED_FILE, "w" ); 

  fprintf( fp, "#BANNED\n\n" );

  for( ban = ban_list; ban != NULL; ban = ban->next ) 
    fprintf( fp, "%s~\n", ban->name );
  fprintf( fp, "$~\n" );

  fclose( fp );
  return;
}




