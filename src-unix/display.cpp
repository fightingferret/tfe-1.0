#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   TITLE FUNCTIONS
 */


void send_title( char_data* ch, const char* text )
{
  char     tmp1  [ TWO_LINES ];
  char     tmp2  [ TWO_LINES ];
  char*  letter;
  int    length  = strlen( text );
 
  if( ch->link == NULL )
    return;

  if( length > 80 ) {
    roach( "Send_Title: Length of text > 80." );
    roach( "-- Text = %s", text );
    return;
    } 

  sprintf( tmp1, "%%%ds", 40-length/2 );
  sprintf( tmp2, tmp1, "" );
  sprintf( tmp1, "%s\n\r", text );

  *tmp1 = toupper( *tmp1 );
  for( letter = tmp1; *letter != '\n'; )
    if( *letter++ == ' ' )
      *letter = toupper( *letter );

  send( ch, tmp2 );
  send_color( ch, COLOR_TITLES, tmp1 );
  
  for( letter = tmp1; *letter != '\n'; letter++ )
     if( *letter != ' ' )
       *letter = '-';

  send( ch, tmp2 );
  send_color( ch, COLOR_TITLES, tmp1 );

  return;
}


void page_title( char_data* ch, const char* text )
{
  char     tmp1  [ TWO_LINES ];
  char     tmp2  [ TWO_LINES ];
  char*  letter;
  int    length  = strlen( text );

  if( ch->link == NULL )
    return;

  if( length > 80 ) {
    roach( "Page_Title: Length of text > 80." );
    roach( "-- Text = %s", text );
    return;
    } 

  sprintf( tmp1, "%%%ds", 40-length/2 );
  sprintf( tmp2, tmp1, "" );
  sprintf( tmp1, "%s\n\r", text );

  *tmp1 = toupper( *tmp1 );
  for( letter = tmp1; *letter != '\n'; )
    if( *letter++ == ' ' )
      *letter = toupper( *letter );

  page( ch, tmp2 );
  page_color( ch, COLOR_TITLES, tmp1 );
  
  for( letter = tmp1; *letter != '\n'; letter++ )
     if( *letter != ' ' )
       *letter = '-';

  page( ch, tmp2 );
  page_color( ch, COLOR_TITLES, tmp1 );

  return;
}


void display_array( char_data* ch, const char* title, const char** entry1,
  const char** entry2, int max )
{
  char             tmp  [ TWO_LINES ];
  char**        string;
  int                i;

  sprintf( tmp, "%s:\n\r", title );
  page( ch, tmp );

  for( i = 0; i < max; i++ ) {
    string = (char**) int( entry1+i*(entry2-entry1) );
    sprintf( tmp, "%18s%s", *string, i%4 == 3 ? "\n\r" : "" );
    page( ch, tmp );
    }
  if( i%4 != 0 )
    page( ch, "\n\r" );

  return;
}


/*
 *   WORD LIST
 */


const char* word_list( const char** list, int max, bool use_and )
{
  char*  tmp;
  int      i;

  if( max == 0 )
    return empty_string;

  store_pntr = ( store_pntr+1 )%5;
  tmp        = &static_storage[store_pntr*THREE_LINES];

  strcpy( tmp, list[0] );

  if( max > 1 ) {
    for( i = 1; i < max; i++ ) 
      sprintf( tmp+strlen( tmp ), ", %s%s",
        use_and && i+1 == max ? "and " : "", list[i] );
    }

  return tmp;
}
