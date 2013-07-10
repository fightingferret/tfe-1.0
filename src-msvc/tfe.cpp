#include "define.h"
#include "struct.h"


char            str_boot_time  [ 26 ];
int             port           [ 3 ];


bool                 tfe_down  = FALSE;  
time_t           current_time; 
time_t              boot_time;
time_data           last_time;


void  init_memory         ( void );
void  init_variables      ( void );
void  main_loop           ( void );
void  set_time            ( void );
void  wait_pulse          ( void );
void  record_time         ( time_data& );


/*
 *   MAIN PROCEDURE
 */


int main( int argc, char **argv )
{
  time_data  start;

  port[0] = 23;
  port[1] = 4000;

  if( argc > 1 ) 
    port[0] = atoi( argv[1] );

  if( argc > 2 )
    port[1] = atoi( argv[2] );

  if( port[0] == port[1] ) 
    panic( "The Java and telnet ports both equal %d.", port[0] );

  gettimeofday( &start, NULL );

  set_time( );
  init_memory( );
  init_variables( );

  bug( -1, "** STARTING MUD **" );

//  recover_links( );
//  init_daemon( );
  boot_db( );

  socket_one = open_port( port[0] );
  socket_two = open_port( port[1] );

  startup_time = stop_clock( start ).tv_sec;
  gettimeofday( &last_time, NULL );

  echo( "TFE done booting\r\n" );
  bug( -1, "** MUD BOOTED **" );

  restart_links( );

  main_loop( );

  return 0;
}


void main_loop( void )
{
  while( !tfe_down ) {
    update_handler( );
    update_links( );
    wait_pulse( );
    }

  return;
}


/*
 *   TIME HANDLING ROUTINES
 */


void set_time( void )
{
  int i;

  gettimeofday( &last_time, NULL );

  current_time = last_time.tv_sec;
  boot_time    = current_time;

  strcpy( str_boot_time, ctime( &current_time ) );
  srand( current_time );

  for( i = 0; i < MAX_TIME; i++ ) {
    critical_time[i] = 0;
    total_time[i]    = 0;
    pulse_time[i]    = 0;
    }

  return;
}


void wait_pulse( void )
{
  time_data   now_time;
  int                i;

  gettimeofday( &now_time, NULL );

  last_time                 -= now_time;
  total_time[ TIME_ACTIVE ] -= last_time;

  now_time.tv_sec  = 0;
  now_time.tv_usec = 1000000/PULSE_PER_SECOND;
 
  last_time += now_time; 

  record_time( last_time );

  if( last_time.tv_sec == 0 && last_time.tv_usec > 0 ) {
    total_time[ TIME_WAITING ] += last_time;
    Sleep(last_time.tv_sec * 1000 + last_time.tv_usec / 1000);

//    if( select( 0, NULL, NULL, NULL, &last_time ) < 0 ) 
//      bug( "Wait_Pulse: error in select" );
  } else {
    critical_time[ TIME_ACTIVE ]  -= last_time;
    critical_time[ TIME_ACTIVE ]  += now_time;
    critical_time[ TIME_WAITING ] -= last_time;

    for( i = TIME_WAITING+1; i < MAX_TIME; i++ )
      critical_time[i] += pulse_time[i];
    }

  for( i = TIME_WAITING+1; i < MAX_TIME; i++ ) {
    total_time[i] += pulse_time[i];
    pulse_time[i] = 0;
    }

  gettimeofday( &last_time, NULL );
  current_time = last_time.tv_sec;

  return;
}


void record_time( time_data& time )
{
  int  lag  = -10*time.tv_sec-time.tv_usec/100000;
  int    i  = 0;

  if( link_list != NULL ) {
    for( i = 0; lag > 0 && i < 9; lag /= 2, i++ ); 
    time_history[i]++;
    }

  return;
}
 

/*
 *   ROUTINE TO INIT CONSTANTS
 */


void init_variables( void )
{
  int i;  

  for( i = 0; i < MAX_PFILE; i++ ) 
    ident_list[i] = NULL;

  for( i = 0; i < 5000; i++ )
    event_queue[i] = NULL;

  for( i = 0; i < MAX_IFLAG; i++ )
    info_history[i] = NULL;

  return;
}














