#include <arpa/telnet.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include "signal.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <unistd.h>

#include "machine.h"


void    delay           ( void );
void    update          ( void );


char  read_buffer  [ 160 ];
int     read_pntr  = 0;
int          idle  = 0;

int   input;
int   output;


int main( int, char **argv )
{
  signal( SIGPIPE, SIG_IGN );

  input   = atoi( argv[1] );
  output  = atoi( argv[2] );

  fprintf( stderr, "[Daemon] Starting\n" );

  fcntl( input, F_SETFL, O_NDELAY );

  for( ; ; ) {
    delay( );
    update( );
    }
    
  return 0;
}


void delay( )
{
  struct timeval time;

  time.tv_usec = 100000;
  time.tv_sec  = 0;

  if( select( 0, NULL, NULL, NULL, &time ) < 0 ) {
    fprintf( stderr, "[BUG] Delay: error in select.\n" );
    exit( 1 );
    }

  return;
}


void update( )
{
  char               buf  [ 80 ];
  int               addr;
  struct hostent*   from;
  int              nRead;
  int                  i;

  #define sa  int( sizeof( struct in_addr ) )

  if( read_pntr < sa
    && ( nRead = read( input, &read_buffer[read_pntr], 10 ) ) > 0 )
    read_pntr += nRead;

  if( read_pntr < sa ) {
    if( ++idle > 10000 ) {
      if( write( output, "Alive?", 7 ) < 0 ) {
        fprintf( stderr, "[Daemon] Exiting\n" );
        exit( 1 );
        }
      idle = 0;
      }
    return;
    }

  memcpy( &addr, read_buffer, sizeof( int ) );
  addr = ntohl( addr );

  sprintf( buf, "%d.%d.%d.%d",
    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  from = gethostbyaddr( read_buffer, sa, AF_INET );
 
  if( from != NULL ) 
    write( output, from->h_name, 1+strlen( from->h_name ) );
  else 
    write( output, buf, 1+strlen( buf ) );

  for( i = sa; i < read_pntr; i++ )
    read_buffer[i-sa] = read_buffer[i];

  read_pntr -= sa; 

  #undef sa

  return;
}




