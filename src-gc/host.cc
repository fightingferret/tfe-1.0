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
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
#include "define.h"
#include "struct.h"


char       read_buffer  [ TWO_LINES ];
link_data*  host_stack  = NULL;
int          read_pntr  = 0;
int              input  = -1;
int             output  = -1;


void  connect_link   ( link_data*, const char* );


void broken_pipe( )
{
  roach( "Write_Host: Pipe to host daemon is broken." );
  roach( "Write_Host: Attempting to revive daemon." );
  init_daemon( );
  return;
}


bool init_daemon( )
{
  char       tmp  [ ONE_LINE ];
  pid_t      pid;
  int     pipe_a  [ 2 ];
  int     pipe_b  [ 2 ];

  printf( "Waking Daemon...\n" );

  if( input != -1 ) {
    close( input );
    close( output ); 
    }  

  pipe( pipe_a );
  pipe( pipe_b );  

  if( ( pid = fork( ) ) == (pid_t) 0 ) { 
    dup( pipe_a[0] );
    dup( pipe_b[1] );
    close( pipe_a[1] );
    close( pipe_b[0] );

    printf( "Init_Daemon: Pipes are %d and %d.\n",
      pipe_a[0], pipe_b[1] );

    sprintf( &tmp[0], "%d", pipe_a[0] );
    sprintf( &tmp[9], "%d", pipe_b[1] );

    if( execlp( "./daemon", "./daemon", &tmp[0], &tmp[9], (char*) 0 ) == -1 )
      printf( "Init Daemon: Error in Execlp.\n" ); 
     
    exit( 1 );
    } 
  else if( pid == -1 ) {
    return FALSE;
    }

  close( pipe_a[0] );
  close( pipe_b[1] );

  input  = pipe_b[0];
  output = pipe_a[1];

  fcntl( input,  F_SETFL, O_NDELAY );
  fcntl( output, F_SETFL, O_NDELAY );

  /* CLOSE PIPE ON EXEC */

  fcntl( input,  F_SETFD, 1 );
  fcntl( output, F_SETFD, 1 );
 
  return TRUE;
}


int players_on( )
{
  int          num  = 0;
  link_data*  link;

  for( link = link_list; link != NULL; link = link->next )
    if( link->connected == CON_PLAYING )
      num++;

  return num;
}


void write_host( link_data* link, char* name )
{
  char*  tmp1  = static_string( );
  char*  tmp2  = static_string( );
  int    addr;  

  sprintf_minutes( tmp1, current_time-boot_time );
  sprintf( tmp2, "\n\r%d players on.\n\rSystem started %s ago.\n\r\
Getting site info ...\n\r", players_on( ), tmp1 );

  write( link->channel, tmp2, strlen( tmp2 ) );
 
  if( count( host_stack ) > 5 ) {
    aphid( "Write_Host: Host daemon is swamped." );
    }        
  else if( write( output, name, sizeof( struct in_addr ) ) == -1 ) {  
    broken_pipe( );
    }
  else {
    append( host_stack, link );
    return;
    }

  memcpy( &addr, name, sizeof( int ) );
  addr = ntohl( addr );

  sprintf( tmp1, "%d.%d.%d.%d",
    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  connect_link( link, tmp1 );
  return;
}
 

void read_host( )
{ 
  struct timeval  start;  
  link_data*       link;  
  int              i, j;
  int             nRead;

  gettimeofday( &start, NULL );

  if( read_pntr < ONE_LINE ) {
    if( ( nRead = read( input, &read_buffer[read_pntr], 50 ) ) == -1 ) {
      broken_pipe( );
      return;
      } 
    read_pntr += nRead;
    }

  for( i = 0; i < read_pntr; i++ )
    if( read_buffer[i] == '\0' )
      break;

  if( i == read_pntr )
    return;

  if( strcmp( read_buffer, "Alive?" ) ) {
    link       = host_stack;
    host_stack = host_stack->next;
    connect_link( link, read_buffer );
    }

  for( j = i+1; j < read_pntr; j++ )
    read_buffer[j-i-1] = read_buffer[j];

  read_pntr -= i+1;

  pulse_time[ TIME_DAEMON ] = stop_clock( start );  

  return;
}


void connect_link( link_data* link, const char* host )
{  
  char  tmp  [ TWO_LINES ];

  link->host = alloc_string( host, MEM_LINK );
 
  link->next = link_list;
  link_list  = link;

  write_greeting( link );

  sprintf( tmp, "Connection from %s", link->host );
  info( "", LEVEL_DEMIGOD, tmp, IFLAG_LOGINS );

  return;
}



