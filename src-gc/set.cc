#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   SET STRINGS
 */


void set_string( char_data* ch, char* argument, char*& string,
  const char* subject, int mem_type )
{
  if( *argument == '\0' ) {
    send( ch, "Set %s to what?\n\r[ Current Value: %s ]\n\r",
      subject, string );
    return;
    }  

  send( ch, "%s set.\n\r[ New:  %s ]\n\r[ Prev: %s ]\n\r",
    subject, argument, string );
  free_string( string, mem_type );
  string = alloc_string( argument, mem_type );

  return;
}


const char* string_field :: set( char_data* ch, const char* subject,
  char* argument )
{
  static char  buf  [ FOUR_LINES ];

  if( *argument == '\0' ) {
    send( ch, "Set %s of %s to?\n\r[ Current value: %s ]\n\r",
      name, subject, *value );
    return empty_string;
    }

  if( func == NULL ) {
    if( !strcasecmp( argument, "blank" ) ) {
      send( ch, "%s on %s set blank.\n\r[ Prev: %s ]\n\r",
        name, subject, *value );
      free_string( *value, mem_type );
      *value = empty_string;
      sprintf( buf, "%s set blank.", name );
      return buf;
      }
    }
  else {
    if( ( *func )( ch, argument, *value ) == FALSE )
      return empty_string;
    }

  send( ch, "%s on %s set.\n\r[ New:  %s ]\n\r[ Prev: %s ]\n\r",
    name, subject, argument, *value );
  free_string( *value, mem_type );
  *value = alloc_string( argument, mem_type );
  sprintf( buf, "%s set to %s.", name, argument );

  return buf;
};


/*
 *   SET TYPES
 */


void set_type( char_data* ch, char* argument, int& value,
  const char* field, const char* subject, int max,
  const char** word1, const char** word2 )
{
  int i;

  if( word2 == NULL )
     word2 = word1+1;

  if( *argument == '\0' ) {
    page( ch, "%s Options:\n\r", field );
    for( i = 0; i < max; i++ ) 
      page( ch, "%20s%s", element( i, word1, word2 ),
        i%3 == 2 ? "\n\r" : "" );
    page( ch, "\n\r%s[ Current Value: %s ]\n\r", i%3 != 0 ? "\n\r" : "",
      element( value, word1, word2 ) );
    return;
    }

  for( i = 0; i < max; i++ ) {
    if( fmatches( element( i, word1, word2 ), argument ) ) {
      value = i;
      send( ch, "%s on %s set to %s.\n\r", field, subject,
        element( i, word1, word2 ) );
      return;
      }
    }

  send( ch, "Unknown %s.\n\r", subject );

  return;
}


const char* type_field :: set( char_data* ch, const char* subject,
  char* argument )
{
  static char  buf  [ TWO_LINES ];
  char         tmp  [ TWO_LINES ];
  int            i;

  if( argument[0] == '\0' ) {
    send( ch, "%s Options:\n\r", name );
    for( i = 0; i < max; i++ ) {
      sprintf( tmp, "%20s%s", element( i ), i%3 == 2 ? "\n\r" : "" );
      send( tmp, ch );
      }
    sprintf( tmp, "\n\r%s[ Current Value: %s ]\n\r", i%3 != 0 ? "\n\r" : "",
      element( *value ) );
    send( tmp, ch );
    return empty_string;
    }

  int length = strlen( argument );

  for( i = 0; i < max; i++ ) {
    if( !strncasecmp( element( i ), argument, length ) ) {
      *value = i;
      sprintf( tmp, "%s on %s set to %s.\n\r", name, subject, element( i ) );
      tmp[0] = toupper( tmp[0] );
      send( tmp, ch );
      sprintf( buf, "%s set to %s.", name, element( i ) );
      return buf;
      }
    }

  sprintf( tmp, "Unknown %s.\n\r", name );
  send( tmp, ch );

  return empty_string;
}


/*
 *   INTEGERS
 */


const char* int_field :: set( char_data *ch, const char* subject,
  char *argument )
{
  char*  tmp  = static_string( );
  int    num;

  if( *argument == '\0' ) {
    send( ch, "Set %s of %s to?\n\r[ Allowed range: %d to %d - current\
 value: %d ]\n\r", name, subject, min, max, *value );
    return empty_string;
    }

  if( ( num = atoi( argument ) ) < min || num > max ) {
    send( ch, "The allowed range for %s is from %d to %d.\n\r",
      name, min, max );
    return empty_string;
    }

  if( num == *value ) {
    send( ch, "%s on %s is already set to %d\n\r", 
      name,subject, num );
    return empty_string;
    }

  send( ch, "%s on %s set to %d.\n\r[ Prev. Value: %d ]\n\r",
    name, subject, num, *value );
  sprintf( tmp, "%s set to %d from %d.", name, num, *value );

  *value = num;

  return tmp;
}


/*
 *   CENTS
 */


const char* cent_field :: set( char_data *ch, const char* subject,
  char *argument )
{
  char*  tmp  = static_string( );
  int    num;

  if( *argument == '\0' ) {
    send( ch, "Set %s of %s to?\n\r[ Allowed range: %.2f to %.2f - current\
 value: %.2f ]\n\r", name, subject, (float) min/100, (float) max/100, *value );
    return empty_string;
    }

  if( ( num = 100*atof( argument ) ) < min || num > max ) {
    send( ch, "The allowed range for %s is from %.2f to %.2f.\n\r",
      name, (float) min/100, (float) max/100 );
    return empty_string;
    }

  if( num == *value ) {
    send( ch, "%s on %s is already set to %.2f\n\r", 
      name, subject, (float) num/100 );
    return empty_string;
    }

  send( ch, "%s on %s set to %.2f.\n\r[ Prev. Value: %.2f ]\n\r",
    name, subject, (float) num/100, (float) *value/100 );
  sprintf( tmp, "%s set to %.2f from %.2f.",
    name, (float) num/100, (float) *value/100 );

  *value = num;

  return tmp;
}


/*
 *   DICE
 */


const char* dice_field :: set( char_data* ch, const char* subject,
  char* argument )
{
  static char    buf  [ TWO_LINES ];
  char           tmp  [ TWO_LINES ];
  dice_data    dice1;
  dice_data    dice2;

  dice1 = *value;

  if( argument[0] == '\0' ) {
    sprintf( tmp, "Set %s of %s to?\n\r[ Current value: %dd%d+%d ]\n\r", 
      name, subject, dice1.number, dice1.side, dice1.plus );
    send( tmp, ch );
    return empty_string;
    }

  dice2.number = atoi( argument );
  for( ; isdigit( *argument ); argument++ );
  if( toupper( *argument++ ) != 'D' ) {
    send( "Incorrect format: #d#+#.\n\r", ch );
    return empty_string;
    }
  dice2.side = atoi( argument );
  for( ; isdigit( *argument ); argument++ );
  dice2.plus = ( *argument++ == '+' ? atoi( argument ) : 0 );

  sprintf( tmp, "%s on %s set to %dd%d+%d.\n\r[ Prev. Value: %dd%d+%d ]\n\r",
    name, subject, dice2.number, dice2.side, dice2.plus,
    dice1.number, dice1.side, dice1.plus );
  send( tmp, ch );

  *value = (int) dice2;

  sprintf( buf, "%s set to %dd%d+%d.",
    name, dice2.number, dice2.side, dice2.plus );

  return buf; 
} 












