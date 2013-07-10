#include "ctype.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/telnet.h>
#include "define.h"
#include "struct.h"


int         socket_one;
int           socket_two;
bool            wizlock  = FALSE;    
link_data*    link_list  = NULL; 
link_data*    link_next;

bool   process_output       ( link_data* );
bool   read_link            ( link_data* );
void   stop_idling          ( char_data* );

const    char    go_ahead_str    [] = { IAC, GA, '\0' };
const    char    echo_off_str    [] = { IAC, WILL, TELOPT_ECHO, '\0' };
const    char    echo_on_str     [] = { IAC, WONT, TELOPT_ECHO, '\0' };

struct in_addr *get_bind_addr();

/*
 *   PREVIOUS PROCESS SOCKET ROUTINES
 */


void recover_links( )
{
/*
  rlimit       lim;
  int            i;
  link_data*  link;

  if( getrlimit( RLIMIT_NOFILE, &lim ) != 0 )
    panic( "Init_Network: error getting file number." );

  for( i = 3; i < int( lim.rlim_cur ); i++ )
    if( int( write( i, "\n\r", 2 ) ) != - 1 ) { 
      link             = new link_data;
      link->channel    = i;
      link->connected  = CON_INTRO;
      link->next       = link_list;
      link_list        = link;
      }

  echo( "-- New process started. --\n\r" );
*/
  return;
}


void restart_links( )
{
  link_data*                  link;
  struct sockaddr_in   net_addr_in;
  int                      addrlen;

  for( ; ; ) {
    if( ( link = link_list ) == NULL )
      return;
     
    link_list = link_list->next;
    addrlen = sizeof( net_addr_in  );

    if( getpeername( link->channel, (struct sockaddr*) &net_addr_in,
      &addrlen ) == -1 )
      panic( "Open_Link: Error returned by getpeername." );

    write_host( link, (char*) &net_addr_in.sin_addr ); 
    }
}
 

/*
 *   SOCKET ROUTINES
 */


void open_link( int port )
{
  link_data*                  link;
  struct sockaddr         net_addr;
  struct sockaddr_in   net_addr_in;
  int                      addrlen;
  int                      fd_conn;

  addrlen = sizeof( net_addr );
  fd_conn = accept( port, &net_addr, &addrlen );

  if( fd_conn < 0 )
    return;

  fcntl( fd_conn, F_SETFL, O_NDELAY );

  addrlen = sizeof( net_addr_in  );

  if( getpeername( fd_conn, (struct sockaddr*) &net_addr_in,
    &addrlen ) == -1 ) {
    bug( "Open_Link: Error returned by getpeername." );
    return;
    }

  link             = new link_data;
  link->channel    = fd_conn;
  link->connected  = CON_INTRO;

  write_host( link, (char*) &net_addr_in.sin_addr ); 

  return;
}

// struct	hostent {
//	const char	*h_name;	/* official name of host */
//	char	**h_aliases;	/* alias list */
//	short	h_addrtype;	/* host address type */
//	short	h_length;	/* length of address */
//	char	**h_addr_list;	/* list of addresses from name server */
//#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
//};


struct in_addr *get_bind_addr()
{
  static struct in_addr bind_addr;

  /* Clear the structure */
  memset((char *) &bind_addr, 0, sizeof(bind_addr));

  bind_addr.s_addr = htonl(INADDR_ANY);
  printf("Binding to all IP interfaces on this host.\n\r");

  return &bind_addr;
}


int open_port( int portnum )
{
  struct sockaddr_in         server;
  struct linger         sock_linger;
  struct hostent*              host;
  char*                    hostname  = static_string( );
  int                          sock;
  int                             i  = 1;
  int                            sz  = sizeof( int ); 

//  WORD wVersionRequested;
//  WSADATA wsaData;

//  wVersionRequested = MAKEWORD(1, 1);

//  if (WSAStartup(wVersionRequested, &wsaData) != 0)
//    panic("SYSERR: WinSock not available!");

  sock_linger.l_onoff  = 1;
  sock_linger.l_linger = 0;

  if( gethostname( hostname, THREE_LINES ) != 0 ) 
    panic( "Open_Port: Gethostname failed." );

  printf( "Open_Port: gethostname: %s.\n\r", hostname );

  if( ( host = gethostbyname( hostname ) ) == NULL )
    panic( "Open_Port: Error in gethostbyname." );

  if( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) 
    panic( "Open_port: error in socket call" );

  if( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sz ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_REUSEADDR)" );

  if( setsockopt( sock, SOL_SOCKET, SO_LINGER, (char*) &sock_linger,
    sizeof( sock_linger ) ) < 0 ) 
    panic( "Open_port: error in setsockopt (SO_LINGER)" );

  if( fcntl( sock, F_SETFD, 1 ) == -1 )
    panic( "Open_port: Error setting close on Exec." );

  memset( &server, 0, sizeof( struct sockaddr_in ) );

  server.sin_family      = AF_INET;;
  server.sin_port        = htons( portnum );
  server.sin_addr        = *get_bind_addr();

//  memcpy( &server.sin_addr, host->h_addr_list[0], host->h_length );

  if( bind( sock, (struct sockaddr*) ( &server ), sizeof( server ) ) ) 
    panic( "Open_port: Error binding port %d at %s.",
      portnum, host->h_name );

  if( listen( sock, 3 ) ) 
    panic( "Open_port: error in listen" );

  printf( "Binding port %d at %s.\n\r", portnum, host->h_name );

  return sock;
}


void update_links( void )
{
  char_data*            ch;
  link_data*          link;
  text_data*       receive;
  fd_set          read_set;
  fd_set         write_set;
  fd_set          exec_set;
  struct timeval     start;  
  struct timeval   timeout;  

  gettimeofday( &start, NULL );

  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;

  FD_ZERO( &read_set );
  FD_ZERO( &write_set );
  FD_ZERO( &exec_set );

  FD_SET( socket_one, &read_set );
  FD_SET( socket_two, &read_set );

  for( link = link_list; link != NULL; link = link->next ) {
    FD_SET( link->channel, &read_set  );
    FD_SET( link->channel, &write_set );
    FD_SET( link->channel, &exec_set );
    }

  if( (int) select( FD_SETSIZE, &read_set, &write_set, &exec_set,
    &timeout ) < 0 ) 
    panic( "Update links: select" );

  if( FD_ISSET( socket_one, &read_set ) )
    open_link( socket_one );

  if( FD_ISSET( socket_two, &read_set ) )
    open_link( socket_two );

  for( link = link_list; link != NULL; link = link_next ) {
    link_next = link->next;
  
    if( FD_ISSET( link->channel, &exec_set ) ) {
      write( link->player );
      close_socket( link );
      continue;
      }
  
    if( FD_ISSET( link->channel, &read_set ) ) {
      link->idle = 0;
      if( link->player != NULL )
        link->player->timer = current_time;
      if( !read_link( link ) ) {
        write( link->player );
        close_socket( link );
        continue;
        }
      }

    if( link->idle++ > 10000 && link->connected != CON_PLAYING ) {
      send( link, "\n\r\n\r-- CONNECTION TIMEOUT --\n\r" );
      close_socket( link, TRUE );
      }
    }

  pulse_time[ TIME_READ_INPUT ] = stop_clock( start );  
  gettimeofday( &start, NULL );

  for( link = link_list; link != NULL; link = link_next ) {
    link_next = link->next;
    if( link->command = ( ( receive = link->receive ) != NULL ) ) {
      ampersand( receive );
      link->receive  = receive->next;
      link->idle     = 0; 
      if( link->connected == CON_PLAYING ) {
        stop_idling( ch = link->character );
        interpret( link->character, receive->message.text );
        }  
      else
        nanny( link, receive->message.text );
      delete receive;
      }
    }

  pulse_time[ TIME_COMMANDS ] = stop_clock( start );  
  gettimeofday( &start, NULL );

  for( link = link_list; link != NULL; link = link_next ) {
    link_next = link->next;
    if( link->idle%25 == 0 && FD_ISSET( link->channel, &write_set )
      && !process_output( link ) ) {
      write( link->player );
      close_socket( link );
      }
    }  
  
  pulse_time[ TIME_WRITE_OUTPUT ] = stop_clock( start );  

  return;
}


/*
 *   CLOSING OF SOCKETS
 */


void extract( link_data* prev )
{
  link_data* link;

  for( link = link_list; link != NULL; link = link->next )
    if( link->snoop_by == prev )
      link->snoop_by = NULL;

  send( prev->snoop_by, "Your victim has left the game.\n\r" );

  if( link_next == prev )
    link_next = prev->next;

  remove( link_list, prev );

  if( prev->account != NULL && prev->account->last_login == -1  ) 
    extract( prev->account );

  delete prev;

  return;
}


void close_socket( link_data* link, bool process )
{
  char              buf [ MAX_STRING_LENGTH ];
  char_data*         ch;
  int         connected  = link->connected;

  if( link->channel == -1 ) {
    bug( "Close_Socket: Closing a dead socket??" );
    return;
    }

  // unswitch
  if( ( ch = link->player ) != NULL && ch != link->character )
    do_return( link->character, "" );

  // send last of buffer
  if( process ) {
    link->connected = CON_CLOSING_LINK;
    process_output( link );
    }

  // send EOF
  send(link->channel, empty_string, 1, (unsigned int) 0);

  if( ch != NULL ) {
    if( connected == CON_PLAYING ) {
      send_seen( ch, "%s has lost %s link.\n\r",
        ch, ch->His_Her( ) );
      sprintf( buf, "%s has lost link.", ch->descr->name );
      info( buf, LEVEL_IMMORTAL, buf, IFLAG_LOGINS, 1, ch );
      ch->link = NULL;
      }
    else {
      if( ch->shdata->level == 0 && ch->pcdata->pfile != NULL ) 
        extract( ch->pcdata->pfile, link );
      ch->Extract( );
      }  
    }

  close( link->channel );
  extract( link );

  return;
}


/*
 *   INPUT ROUTINES
 */


bool erase_command_line( char_data* ch )
{
  char* tmp  = static_string( );

  if( ch == NULL || ch->pcdata->terminal == TERM_DUMB  
    || ch->link->connected != CON_PLAYING
    || !is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR ) )
    return TRUE;

  sprintf( tmp, "[%d;1H[J", ch->pcdata->lines );

  if( int( write( ch->link->channel, tmp, strlen( tmp ) ) ) == -1 )
    return FALSE; 

  return TRUE;
}


bool read_link( link_data* link )
{
  char            buf  [ 2*MAX_INPUT_LENGTH+100 ];
  text_data*  receive;
  int          length;
  int           nRead;
  char*         input;
  char*        output;

  strcpy( buf, link->rec_pending );

  length  = strlen( buf );
  nRead   = read( link->channel, buf+length, 100 );

  if( nRead <= 0 )
    return FALSE;

  free_string( link->rec_pending, MEM_LINK );
  link->rec_pending = empty_string;

  buf[ length+nRead ] = '\0';

  if( length+nRead > MAX_INPUT_LENGTH-2 ) {
    if( link->connected != CON_PLAYING )
      return FALSE;
    send( link->character, "!! Truncating input !!\n\r" );
    sprintf( buf+MAX_INPUT_LENGTH-3, "\n\r" );
    }

  for( input = output = buf; *input != '\0'; input++ ) {
    if( *input != '\n' ) {
      if( isprint( *input ) )
        *output++ = ( *input == '~' ? '-' : *input );
      continue;
      }

    for( ; --output >= buf && *output == ' '; );

    *(++output) = '\0';

    if( link->connected != CON_PLAYING )  
      receive = new text_data( buf );
    else if( *buf == '!' )
      receive = new text_data( link->rec_prev );
    else {
      receive = new text_data( subst_alias( link, buf ) );
      free_string( link->rec_prev, MEM_LINK );
      link->rec_prev = alloc_string( receive->message.text, MEM_LINK );
      }
    append( link->receive, receive );
    output = buf;

    if( !erase_command_line( link->character ) )
      return FALSE;
    }

  *output = '\0';
  link->rec_pending = alloc_string( buf, MEM_LINK ); 

  return TRUE;
}


/*
 *   OUTPUT ROUTINES
 */


bool process_output( link_data* link )
{
  text_data*      output;
  text_data*        next;
  char_data*          ch  = link->character;
  bool        status_bar;

  if( link->connected == CON_PLAYING && ch == NULL ) {
    bug( "Process_Output: Link playing with null character." );
    bug( "--     Host = '%s'", link->host );
    bug( "-- Rec_Prev = '%s'", link->rec_prev );
    return FALSE;
    }

  status_bar = ( link->connected == CON_PLAYING
    && is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR )
    && ch->pcdata->terminal != TERM_DUMB );

  if( link->send == NULL && !link->command )
    return TRUE;

  /* SAVE CURSOR */

  if( status_bar ) {
    next       = link->send;
    link->send = NULL;
    scroll_window( ch );
    if( next != NULL )
      send( ch, "\n\r" );
    cat( link->send, next );
    prompt_ansi( link );
    command_line( ch );
    }
  else {
    if( !link->command ) {
      next       = link->send;
      link->send = NULL;
      send( ch, "\n\r" );
      cat( link->send, next );
      }  
    if( link->connected == CON_PLAYING && link->receive == NULL ) 
      prompt_nml( link );
    }

  /* SEND OUTPUT */

  for( ; ( output = link->send ) != NULL; ) {
    if( int( write( link->channel, output->message.text,
      output->message.length ) ) == -1 )
      return FALSE; 
    link->send = output->next;
    delete output;
    }

  return TRUE;
}


/*
 *   LOGIN ROUTINES
 */


typedef void login_func( link_data*, char* );


struct login_handle
{
  login_func*  function;
  int          state;
};


void nanny( link_data* link, char* argument )
{
  char_data*        ch;
  int                i;

  login_handle nanny_list [] = {
    { nanny_intro,              CON_INTRO              },
    { nanny_acnt_name,          CON_ACNT_NAME          },
    { nanny_acnt_password,      CON_ACNT_PWD           },
    { nanny_acnt_email,         CON_ACNT_EMAIL         },
    { nanny_acnt_enter,         CON_ACNT_ENTER         },
    { nanny_acnt_confirm,       CON_ACNT_CONFIRM       },
    { nanny_acnt_check,         CON_ACNT_CHECK         },
    { nanny_acnt_check_pwd,     CON_ACNT_CHECK_PWD     },
    { nanny_old_password,       CON_PASSWORD_ECHO      },
    { nanny_old_password,       CON_PASSWORD_NOECHO    },
    { nanny_motd,               CON_READ_MOTD          },
    { nanny_imotd,              CON_READ_IMOTD         },
    { nanny_new_name,           CON_GET_NEW_NAME       },
    { nanny_acnt_request,       CON_ACNT_REQUEST       },
    { nanny_acnt_menu,          CON_ACNT_MENU          },
    { nanny_confirm_password,   CON_CONFIRM_PASSWORD   },
    { nanny_set_term,           CON_SET_TERM           },
    { nanny_show_rules,         CON_READ_GAME_RULES    },
    { nanny_agree_rules,        CON_AGREE_GAME_RULES   },
    { nanny_alignment,          CON_GET_NEW_ALIGNMENT  },
    { nanny_help_alignment,     CON_HELP_ALIGNMENT     },
    { nanny_disc_old,           CON_DISC_OLD           },
    { nanny_help_class,         CON_HELP_CLSS          },
    { nanny_class,              CON_GET_NEW_CLSS       },
    { nanny_help_race,          CON_HELP_RACE          },
    { nanny_race,               CON_GET_NEW_RACE       },
    { nanny_stats,              CON_DECIDE_STATS       },  
    { nanny_help_sex,           CON_HELP_SEX           },
    { nanny_sex,                CON_GET_NEW_SEX        },   
    { nanny_new_password,       CON_GET_NEW_PASSWORD   },
    { nanny_acnt_enter,         CON_CE_ACCOUNT         },
    { nanny_acnt_check_pwd,     CON_CE_PASSWORD        },
    { nanny_acnt_email,         CON_CE_EMAIL           },
    { nanny_acnt_enter,         CON_VE_ACCOUNT         },
    { nanny_ve_validate,        CON_VE_VALIDATE        },
    { nanny_acnt_confirm,       CON_VE_CONFIRM         },
    { NULL,                     -1                     }
    };

  skip_spaces( argument );

  ch = link->character;

  for( i = 0; nanny_list[i].function != NULL; i++ ) 
    if( link->connected == nanny_list[i].state ) {
      nanny_list[i].function( link, argument );
      return;
      }

  if( link->connected == CON_PAGE ) {
    write_greeting( link );
    link->connected = CON_INTRO;
    return;
    }

  if( link->connected == CON_FEATURES ) {
    help_link( link, "Features_2" );
    link->connected = CON_PAGE;
    return;
    }  

  if( link->connected == CON_POLICIES ) {
    help_link( link, "Policy_2" );
    link->connected = CON_PAGE;
    return;
    }

  if( link->connected == CON_DIGITALNATION ) {
    help_link( link, "Digitalnation_2" );
    link->connected = CON_PAGE;
    return;
    }

  bug( "Nanny: bad link->connected %d.", link->connected );
  close_socket( link );

  return;
}


void stop_idling( char_data* ch )
{
  if( ch == NULL
    || ch->link == NULL
    || ch->link->connected != CON_PLAYING
    || ch->was_in_room == NULL )
    return;

  ch->timer = current_time;
  if( ch->array != NULL )
    ch->From( );
  ch->To( ch->was_in_room );
  ch->was_in_room = NULL;

  send_seen( ch, "%s has returned from the void.\n\r", ch );

  return;
}


void write_greeting( link_data* link )
{
  help_link( link, "greeting" );
  send( link, "                   Choice: " );

  return;
}



