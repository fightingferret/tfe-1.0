#include "define.h"
#include "struct.h"


time_t      startup_time;
int         time_history   [ 10 ];

time_data   pulse_time     [ MAX_TIME ];
time_data   total_time     [ MAX_TIME ];
time_data   critical_time  [ MAX_TIME ];


char *SWeekday[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

char *SMonth[12] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
   "Nov", "Dec" };

const char* timezones [24] = { "7", "8", "9", "pacific",
  "mountain",
  "central",  "eastern", "j", "k", "g", "h", "greenwich mean",
  "geneva", "central european", "russia",
  "m", "o", "p", "japan",
  "s", "t", "u", "v", "w" };

void  order          ( int*, float*, int, float );
void  display_lag    ( char_data*, int );
void display_time    ( char_data*, const char*, int, char* = empty_string );


/*
 *   SUPPORT ROUTINES
 */


bool is_day( )
{
  return( weather.hour >= 5 && weather.hour < 19 );
}


/*
 *   STRING PRINT FUNCTIONS
 */


void sprintf_minutes( char* tmp, const time_t time )
{
  int days    = time/(60*60*24);
  int hours   = (time/3600)%24;
  int minutes = (time/60)%60;
 
  tmp[0] = '\0';

  if( days > 0 )
    sprintf( tmp, "%d day%s ", days, days == 1 ? "" : "s" );

  if( hours > 0 )
    sprintf( tmp+strlen(tmp), "%d hour%s ", hours, hours == 1 ? "" : "s" );

  sprintf( tmp+strlen(tmp), "%d minute%s", minutes, minutes == 1 ? "" : "s" );
}


void sprintf_time( char* tmp, const time_t time, char letter )
{
  int hours   = (time/3600+letter-'0')%12+1;
  int minutes = (time/60)%60;
 
  sprintf( tmp, "%d:%02d", hours, minutes );
}


char* ltime( const time_t& time )
{
  struct tm*  tmPtr  = localtime( &time );
  char*         tmp  = static_string( );

  if( abs(int(time-current_time)) > 300*24*60*60 ) 
    sprintf( tmp, "%s %s %02d %04d",
      SWeekday[tmPtr->tm_wday],
      SMonth[tmPtr->tm_mon],
      tmPtr->tm_mday, tmPtr->tm_year );
  else
    sprintf( tmp, "%s %s %02d %02d:%02d",
      SWeekday[tmPtr->tm_wday],
      SMonth[tmPtr->tm_mon],
      tmPtr->tm_mday, tmPtr->tm_hour,
      tmPtr->tm_min );

  return tmp;
}


void sprintf_date( char* tmp, const time_t time )
{
  struct tm* tmPtr = localtime( &time );
 
  sprintf( tmp, "%s %d%s 19%d",
    SMonth[ tmPtr->tm_mon ], tmPtr->tm_mday,
    number_suffix( tmPtr->tm_mday ), tmPtr->tm_year );
}


/*
 *   LAG ROUTINES
 */


const time_data& stop_clock( struct timeval& start )
{
  static time_data   time;
  struct   timeval   stop;

  gettimeofday( &stop, NULL );

  time.tv_sec  = stop.tv_sec-start.tv_sec;
  time.tv_usec = stop.tv_usec-start.tv_usec;

  if( time.tv_usec < 0 ) {
    time.tv_usec += 1000000;
    time.tv_sec  -= 1;
    }

  return time;
}


void do_lag( char_data* ch, char* argument )
{
  char      tmp  [ ONE_LINE ];
  int       sum;
  int     flags;
  int      i, j;
  int      list  [ 50 ];
  float   value  [ 50 ];
  int    length  = strlen( argument );

  if( !get_flags( ch, argument, &flags, "hr", "Lag" ) )
    return;;

  if( is_set( &flags, 0 ) ) {
    for( sum = 0, i = 0; i < 10; i++ )
      sum += time_history[i];
    send_underlined( ch,
      "Sec. of Delay         # of Cycles       Percent\r\n" );
    for( j = 0, i = 0; i < 10; i++, j = ( j == 0 ? 1 : 2*j ) ) {
      sprintf( tmp, "%.1f", (float) j/10 );
      sprintf( tmp+10, "%8s %21d %16.2f\r\n", i == 0 ? "none" :
        ( i == 9 ? "more" : tmp ), time_history[i],
        (float) 100*time_history[i]/sum );
      send( tmp+10, ch );
      }
    return;
    }

  page_underlined( ch,
    "Command          Calls    Average    Max Time     Total\r\n" );

  if( is_set( &flags, 1 ) ) {
    vzero( list,  50 );
    vzero( value, 50 );

    for( i = 0; i < MAX_ENTRY_COMMAND; i++ )
      order( list, value, i, command_table[i].total_time.time() );
    for( i = 0; i < 50; i++ )
      display_lag( ch, list[i] );
 
    return;
    } 

  for( i = 0; i < MAX_ENTRY_COMMAND; i++ ) 
    if( !strncasecmp( command_table[i].name, argument, length ) )
      display_lag( ch, i );

  return;
}


void order( int* list, float* value, int label, float num )
{
  int i;

  if( value[49] > num )
    return;

  for( i = 49; i > 0; i-- ) {
    if( value[i-1] > num )
      break;
    list[i]  = list[i-1];
    value[i] = value[i-1];
    }

  list[i]  = label;
  value[i] = num;

  return;
}


void display_lag( char_data* ch, int i )
{
  char     tmp  [ ONE_LINE ];
  int    calls;
  float  total;
  float    max;

  calls = command_table[i].calls;
  total = command_table[i].total_time.time( );
  max   = command_table[i].max_time.time( );

  sprintf( tmp, "%-12s %8d %11.3f %11.3f %9.1f\r\n", 
    command_table[i].name, calls,
    calls == 0 ? 0 : (float) total/(1000*calls),
    (float) max/1000, (float) total/1000 );
  page( ch, tmp );

  return;
}


/*
 *   SYSTEM COMMAND
 */


void display_time( char_data* ch, const char* text, int num, char* c2 )
{
  float    i, j;

  i = (float) 100.*total_time[num].hundred( )
    /total_time[ TIME_ACTIVE ].hundred( );

  j = (float) 100.*critical_time[num].hundred( )
    /critical_time[ TIME_ACTIVE ].hundred( );

  send( ch, "%-22s%4.1f%13.1f       %s\r\n", text, i, j, c2 );

  return;
}


void do_system( char_data* ch, char* )
{
  char*        tmp  = static_string( );
  float          i;

  send_centered( ch, "--| System Info |--" );
  send( "\r\n", ch );

  send( ch, "System Time: %s\r", (char*) ctime( &current_time ) );
  send( ch, " Started at: %s\r\r\n", ctime( &boot_time ) );

  send_underlined( ch, "Function            Percent      Critical\r\n" );

  i = (float) total_time[ TIME_ACTIVE ].hundred( )
    +total_time[ TIME_WAITING ].hundred( );
  i = (float) 100.*critical_time[ TIME_WAITING ].hundred( )/i;

  sprintf( tmp, "Machine Lag: %3.1f%%", i );
  display_time( ch, "Command Handler", TIME_COMMANDS, tmp );

  i = (float) total_time[ TIME_ACTIVE ].hundred( )
    +total_time[ TIME_WAITING ].hundred( );

  i = (float) 100.*total_time[ TIME_ACTIVE ].hundred( )/i;

  sprintf( tmp, "  Cpu Usage: %3.1f%%", i );
  display_time( ch, "Network", TIME_NETWORK, tmp );

  display_time( ch, "  Read Input", TIME_READ_INPUT );

  sprintf( tmp, "Seconds to start: %d", int( startup_time ) );
  display_time( ch, "  Write Output", TIME_WRITE_OUTPUT, tmp );

  display_time( ch, "  Host Deamon",   TIME_DAEMON );
  display_time( ch, "Update Handler",  TIME_UPDATE );
  display_time( ch, "  Regeneration",  TIME_REGEN );
  display_time( ch, "  Events",        TIME_EVENT );
  display_time( ch, "  Violence",      TIME_VIOLENCE );
  display_time( ch, "  Area Resets",   TIME_RESET );  
  display_time( ch, "  Object Loop",   TIME_OBJ_LOOP );
  display_time( ch, "  Char Loop",     TIME_CHAR_LOOP );
  display_time( ch, "  Rndm Acodes",   TIME_RNDM_ACODE );

  return;
}


/* 
 *   TIME FUNCTION   
 */


void do_time( char_data* ch, char* argument )
{
  player_data*    pc;
  int          flags;

  if( !get_flags( ch, argument, &flags, "z", "Time" ) )
    return;

  if( flags == 0 ) {
    send( ch, "Minute: %d  Hour: %d  Day: %d  Month: %s\r\n",
      weather.minute, weather.hour, weather.day,
      month_name[weather.month] );
    send( ch, "[This is temporary]\r\n" );
    return;
    }

  if( ( pc = player( ch ) ) == NULL )
    return;

  class type_field zonetype =
    { "timezone", 24, &timezones[0], &timezones[1], &pc->timezone };

  zonetype.set( ch, empty_string, argument );

  return;
}


/*
 *   TIME ARGUMENTS
 */


int time_arg( char*& argument, char_data* ch )
{
  int i;

  if( !strcasecmp( argument, "forever" ) ) 
    return 0;

  if( !number_arg( argument, i ) ) {
    send( ch, "Length of time must be of format <number> <units>.\r\n" );
    return -1;
    }

  if( i < 1 ) {
    send( ch, "Only positive definite time periods are acceptable.\r\n" );
    return -1;
    }

  if( *argument == '\0' ) {
    send( ch, "Please specify a unit of time.\r\n" );
    return -1;
    }

  if( matches( argument, "seconds" ) )  return i;
  if( matches( argument, "minutes" ) )  return 60*i;
  if( matches( argument, "hours" ) )    return 60*60*i;
  if( matches( argument, "days" ) )     return 24*60*60*i;
  if( matches( argument, "years" ) )    return 365*24*60*60*i;

  send( ch, "Unknown unit of time.\r\nKnown units are seconds, minutes, hours,\
 days, and years.\r\n" ); 

  return -1;
}



