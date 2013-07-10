#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


weather_data  weather;

const char* month_name [ 12 ] = {
  "January", "February", "March", "April", "May",
  "June", "July", "August", "October", "September",
  "November", "December" };

const int days_in_month [ 12 ] = {
  31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31, 31 };


void time_update( void )
{
  char         tmp  [ ONE_LINE ];
  link_data*  link;
  
  tmp[0] = '\0';
 
  if( ++weather.minute > 59 ) {
    weather.hour++;
    weather.minute = 0;

    switch ( weather.hour ) {
    case  5:
      strcat( tmp, "The day has begun.\n\r" );
      break;

    case  6:
      strcat( tmp, "The sun rises in the east.\n\r" );
      break;

    case 19:
      strcat( tmp, "The sun slowly disappears in the west.\n\r" );
      break;

    case 20:
      strcat( tmp, "The night has begun.\n\r" );
      break;

    case 24:
      weather.hour = 0;
      weather.day++;
      break;
      }
    }

  if( weather.day  > days_in_month[weather.month] ) {
    weather.day = 1;
    weather.month++;
    }

  if( weather.month > 12 ) {
    weather.month = 1;
    weather.year++;
    }

  weather.sunlight = sunlight( 60*weather.hour+weather.minute );

  if( *tmp != '\0' ) {
    for( link = link_list; link != NULL; link = link->next ) {
      if( link->connected == CON_PLAYING && IS_OUTSIDE( link->character )
        && IS_AWAKE( link->character )
        && is_set( &link->character->pcdata->message, MSG_WEATHER ) )
        send( tmp, link->character );
      }
    }
}


const char* sky_state( )
{
  if( weather.hour < 5 || weather.hour > 20 ) 
    return "Night";

  if( weather.hour < 6  )  return "Early Morning";
  if( weather.hour < 8  )  return "Mid-Morning";
  if( weather.hour < 14 )  return "Near-Noon";
  if( weather.hour < 17 )  return "Afternoon";
   
  return "Evening";
}
    

