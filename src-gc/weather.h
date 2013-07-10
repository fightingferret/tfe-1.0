#include <sys/time.h>


/*
 *   TIME ROUTINES
 */


class Time_Data : public timeval
{
 public:
  Time_Data( ) {
    tv_sec  = 0;
    tv_usec = 0;
    return;
    }

  void operator+=( const time_data& t ) {
    tv_sec  += t.tv_sec;
    tv_usec += t.tv_usec;
 
    if( tv_usec >= 1000000 ) {
      tv_usec -= 1000000;
      tv_sec++;
      }

    return;
    }

  void operator-=( const time_data& t ) {
    tv_sec  -= t.tv_sec;
    if( ( tv_usec -= t.tv_usec ) < 0 ) {
      tv_usec += 1000000;
      tv_sec--;
      }
    return;
    }

  void operator=( const time_data& t ) {
    tv_sec  = t.tv_sec;
    tv_usec = t.tv_usec;
    }

  bool operator<( const time_data& t ) {
    return( tv_sec == t.tv_sec ? tv_usec < t.tv_usec
      : tv_sec < t.tv_sec );
    }    

  void operator=( int t ) {
    tv_sec  = 0;
    tv_usec = t;
    }

  float time( ) {
    return (float) 1e6*tv_sec+tv_usec;
    } 

  int hundred( ) {
    return 100*tv_sec+tv_usec/10000;
    }
};


#define TIME_ACTIVE         0
#define TIME_WAITING        1
#define TIME_COMMANDS       2
#define TIME_NETWORK        3
#define TIME_READ_INPUT     4
#define TIME_WRITE_OUTPUT   5
#define TIME_DAEMON         6
#define TIME_UPDATE         7
#define TIME_EVENT          8
#define TIME_REGEN          9
#define TIME_RESET         10
#define TIME_VIOLENCE      11
#define TIME_OBJ_LOOP      12
#define TIME_CHAR_LOOP     13
#define TIME_RNDM_ACODE    14
#define MAX_TIME           15


extern time_t      boot_time;
extern time_t      current_time;
extern time_t      startup_time;
extern int         time_history   [ 10 ];
extern time_data   pulse_time     [ MAX_TIME ];
extern time_data   total_time     [ MAX_TIME ];
extern time_data   critical_time  [ MAX_TIME ];
extern char        str_boot_time  [];


bool               is_day           ( void );
void               sprintf_minutes  ( char*, const time_t );
void               sprintf_date     ( char*, const time_t );
void               sprintf_time     ( char*, const time_t, char );
char*              ltime            ( const time_t& );
void               time_update      ( void );
const time_data&   stop_clock       ( struct timeval& );
int                time_arg         ( char*&, char_data* );

inline int weeks ( int sec ) { return 7*24*60*60*sec; }
inline int days  ( int sec ) { return 24*60*60*sec; }


/* 
 *   WEATHER ROUTINES
 */


extern  const int     days_in_month  [ 12 ];
extern  weather_data        weather;


class Weather_Data
{
 public:
  int            minute;
  int            hour;
  int            day;
  int            month;
  int            year;
  int            sunlight;
};


const char*  sky_state  ( );
int          sunlight   ( int );


inline bool isday( )
{
  return( weather.hour >= 5 && weather.hour <= 20 );
}
 





