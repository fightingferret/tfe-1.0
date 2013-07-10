#include "define.h"
#include "struct.h"


void  connect_link   ( link_data*, const char* );

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
  sprintf( tmp2, "\r\n%d players on.\r\nSystem started %s ago.\r\n\
Getting site info ...\r\n", players_on( ), tmp1 );

  write( link->channel, tmp2, strlen( tmp2 ) );
 
  memcpy( &addr, name, sizeof( int ) );
  addr = ntohl( addr );

  sprintf( tmp1, "%d.%d.%d.%d",
    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );

  connect_link( link, tmp1 );
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


