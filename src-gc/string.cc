#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"
#include "unistd.h"


char* empty_string = (char*) "\0";


/*
 *   STRING COMPARISON
 */


inline bool null_strings( const char* astr, const char* bstr )
{
  if( astr != NULL && bstr != NULL )
    return FALSE;

  if( astr == NULL && bstr == NULL )
    bug( "Str*cmp: Null string (x2)." );
  else
    bug( "Str*cmp: Comparing '%s' to null.",
      astr == NULL ? bstr : astr );

  return TRUE;
}


int strcasecmp( const char* astr, const char* bstr )
{
  if( astr == bstr )
    return 0;

  if( null_strings( astr, bstr ) )
    return 1;

  for( ; *astr != '\0' || *bstr != '\0'; astr++, bstr++ )
    if( tolower( *astr ) != tolower( *bstr ) )
      return( tolower( *astr ) > tolower( *bstr ) ? 1 : -1 );

  return 0;
}


int strncasecmp( const char* astr, const char* bstr, int n )
{
  const char* end;

  if( null_strings( astr, bstr ) )
    return 1;

  end = astr+n;

  for( ; astr < end && ( *astr != '\0' || *bstr != '\0' ); astr++, bstr++ )
    if( tolower( *astr ) != tolower( *bstr ) )
      return( tolower( *astr ) > tolower( *bstr ) ? 1 : -1 );

  return 0;
}


int strncmp( const char* astr, const char* bstr, int n )
{
  const char* end;

  if( null_strings( astr, bstr ) )
    return 1;

  end = astr+n;

  for( ; astr < end && ( *astr != '\0' || *bstr != '\0' ); astr++, bstr++ )
    if( *astr != *bstr )
      return( *astr > *bstr ? 1 : -1 );

  return 0;
}


char* strcat( char* dst, const char* src )
{
  char* s;

  for( s = dst; *s != '\0'; s++ );
  for( ; *src != '\0'; )
    *s++ = *src++;

  *s = '\0';

  return dst;
}


int rstrcasecmp( const char* astr, const char* bstr )
{
  int  a, b, c;
  int        i;

  if( null_strings( astr, bstr ) )
    return 1;

  a = strlen( astr );
  b = strlen( bstr );
  c = min( a, b ); 

  for( i = 1; i <= c; i++ )
    if( tolower( astr[a-i] ) != tolower( bstr[b-i] ) ) 
      return( tolower( astr[a-i] ) > tolower( bstr[b-i] ) ? 1 : -1 );

  return( a < b ? -1 : a > b );
}


int rstrncasecmp( const char* astr, const char* bstr, int n )
{
  int   a, b, c;
  int         i;

  if( null_strings( astr, bstr ) )
    return 1;

  a = strlen( astr );
  b = strlen( bstr );
  c = min( min( a, b ), n ); 

  for( i = 1; i <= c; i++ ) 
    if( tolower( astr[a-i] ) != tolower( bstr[b-i] ) ) 
      return( tolower( astr[a-i] ) > tolower( bstr[b-i] ) ? 1 : -1 );

  return( c == n ? 0 : a < b );
}


/*
 *   GENERIC UTILITY ROUTINES
 */


bool fmatches( const char* argument, const char* word, int length )
{
  int i;

  for( i = 0; argument[i] != '\0'; i++ ) 
    if( tolower( word[i] ) != tolower( argument[i] ) )
      return FALSE;

  return( i > length );
}


bool matches( const char*& argument, const char* word )
{
  const char* str = argument;

  if( *str == '\0' )
    return FALSE;

  for( ; *str != '\0' && *str != ' '; word++, str++ ) 
    if( tolower( *word ) != tolower( *str ) )
      return FALSE;

  skip_spaces( str );
  argument = str;

  return TRUE;
}


bool exact_match( const char*& argument, const char* word )
{
  const char* str = argument;

  for( ; *word != '\0'; word++, str++ )
    if( tolower( *word ) != tolower( *str ) )
      return FALSE;

  if( *str != ' ' && *str != '\0' && *str != ',' && *str != ';' )
    return FALSE;

  skip_spaces( str );
  argument = str;

  return TRUE;
}


bool number_arg( const char*& argument, int& i )
{
  const char* str  = argument;
  int           j  = 0;
  bool      minus  = FALSE;

  if( *str == '-' ) {
    minus = TRUE;
    str++;
    }
  else if( *str == '+' )
    str++;

  for( ; ; ) {
    if( !isdigit( *str ) )
      return FALSE;
    j = 10*j+(*str++)-'0';
    if( *str == '\0' || *str == ' ' )
      break;
    }

  argument = str;
  i        = minus ? -j : j;

  skip_spaces( argument );

  return TRUE;
}


/*
 *
 */


char* seperate( char* string, bool identified )
{
  int i;

  if( *string != '{' )
    return string;

  for( i = 1; string[i] != '}' && string[i] != '\0'; i++ );

  if( !identified ) {
    if( i == 1 )
      return empty_string;
    char* tmp = static_string( );
    memcpy( tmp, &string[1], i-1 );
    tmp[i-1] = '\0';
    return tmp;
    }

  if( string[i] == '\0' )
    return empty_string;

  for( i++; string[i] == ' '; i++ );

  return &string[i];
}


char* one_condition( char *argument, char *cond )
{
  if( *argument == '\0' )
    *cond = '\0';
  else {
    for( ; *argument != '\n' && *argument != '\0'
      && *argument != '&'; argument++, cond++ )
      *cond = *argument;

    *cond = '\0';

    if( *argument == '&' )
      argument += 2;

    if( *argument == '\n' ) {
      argument++;
      if( *argument == '\r' )
        argument++;
      }
    }

  return argument;
}


/*
 *  IS_NAME ROUTINE
 */


bool is_name( char* str, const char* namelist, bool command )
{
  char           name  [ MAX_INPUT_LENGTH ];
  char           word  [ MAX_INPUT_LENGTH ];
  const char*    list;
  int          length;
  int             pos;

  for( ; ; ) {
    str = one_argument( str, word );

    if( *word == '\0' )
      return TRUE;

    list = namelist;
    length = 3;

    for( ; ; ) {
      list = one_argument( list, name );

      if( *name == '\0' )
        return FALSE;

      if( isdigit( *name ) ) {
        length = atoi( name );
        continue;
        }

      if( strncasecmp( word, name, strlen( word ) ) ) {
        length = 3;
        continue;
        }

      if( command ) {
        pos = search( command_table, MAX_ENTRY_COMMAND, name );
        if( pos >= 0 && command_table[pos].reqlen <= strlen( word ) )
          break;
        }

      if( strlen( word ) >= min( length, strlen( name ) ) )
        break;
      }
    }
}


int subset( const char* s1, const char* s2, bool cmd )
{
  int i = 100;

  for( ; *s1 != '\0'; ) {
    switch( member( s1, s2, cmd ) ) {
      case 0 : return 0;
      case 1 : i = 50;
      }
    for( ; *s1 != '\0' && *s1 != ' '; s1++ );
    for( ; *s1 == ' '; s1++ );
    }

  return( i-count( s2 ) );
}


int member( const char* s1, const char* s2, bool cmd )
{
  int  len  = 3;
  int    i  = 0;
  
  skip_spaces( s2 );

  if( *s1 == '\0' )
    return( *s2 == '\0' );

  for( ; *s2 != '\0'; ) {
    if( isdigit( *s2 ) ) {
      len = atoi( s2 );
      } 
    else {
      switch( compare( s1, s2, cmd, len ) ) {
        case 2 : return 2;
        case 1 : i = 1;
        }
      len = 3;
      }
    for( ; *s2 != '\0' && *s2 != ' '; s2++ );
    for( ; *s2 == ' '; s2++ );
    }

  return i;
}
 

int compare( const char* s1, const char* s2, bool cmd, int len )
{
  const char*  s3  = s2;
  int         pos;

  for( ; *s1 != ' ' && *s1 != '\0'; s1++, s2++ )
    if( toupper( *s1 ) != toupper( *s2 ) )
      return 0;

  if( cmd && ( pos = search( command_table, MAX_ENTRY_COMMAND, s3 ) ) > 0 ) {
    if( command_table[pos].reqlen > s2-s3 )
      return 0;
    }
  else if( s2-s3 < len && *s2 != '\0' && *s2 != ' ' )
    return 0;

  return( *s2 == ' ' || *s2 == '\0' ? 2 : 1 );
}


/*
 *   STRING TYPES
 */


bool is_number( char* arg )
{
  if( *arg == '\0' )
    return FALSE;

  if( ( *arg == '+' ) || ( *arg == '-' ) )
    arg++;

  for( ; *arg != '\0' && *arg != ' '; arg++ )
    if( !isdigit( *arg ) )
      return FALSE;

  return TRUE;
}


bool isperiod( char letter )
{
  switch( letter ) {
    case '.' :
    case '?' :
    case '!' :
    return TRUE;
    }

  return FALSE;
}


bool isvowel( char letter )
{
  switch( toupper( letter ) ) {
    case 'A' :
    case 'E' :
    case 'I' :
    case 'O' :
    case 'U' :
      return TRUE;
    }

  return FALSE;
}


/*
 *   PARSE INPUT
 */


bool contains_word( const char*& argument, const char* word, char* arg )
{
  const char*  s1  = argument;
  const char*  s2;

  for( ; *s1 != '\0'; ) {
    if( exact_match( s2 = s1, word ) ) {
      for( ; s1 != argument && *(s1-1) == ' '; s1-- );
      memcpy( arg, argument, s1-argument );
      arg[s1-argument] = '\0';
      argument = s2;
      return TRUE;
      } 
    for( ; *s1 != '\0' && *s1 != ' '; s1++ );
    for( ; *s1 == ' '; s1++ );
    }

  return FALSE;
}


bool two_argument( const char*& argument, const char* word, char* arg )
{
  int i;

  if( ( i = count( argument ) ) < 2 )
    return FALSE;

  if( i == 2 ) {
    argument = one_argument( argument, arg );
    return TRUE;
    }

  return( contains_word( argument, word, arg )
    && *argument != '\0' && *arg != '\0' );
}
 

char* one_argument( const char* argument, char* arg_first )
{
  char cEnd;

  while( isspace( *argument ) )
    argument++;

  cEnd = ' ';
  if( *argument == '\'' || *argument == '"' ) {
    cEnd = *argument++;
    if( *argument == ' ' || *argument == '\0' ) {
      argument--;
      cEnd = ' ';
      }
    }

  while( *argument != '\0' ) {
    if( *argument == cEnd ) {
      argument++;
      break;
      }
    *arg_first = *argument;
    arg_first++;
    argument++;
    }

  *arg_first = '\0';

  while( isspace( *argument ) )
    argument++;

  return (char*) argument;
}


int smash_argument( char* tmp, char* argument )
{
  int   number  = 0;

  skip_spaces( argument );

  if( !strncasecmp( argument, "all", 3 ) ) {
    if( argument[3] == '\0' ) {
      *tmp = '\0';
      return -1;
      }
    if( argument[3] == '*' || argument[3] == '.' ) {
      argument += 4;
      number = -1;
      }
    }

  if( number == 0 ) {
    if( !isdigit( *argument ) ) 
      number = 1;
    else { 
      for( ; *argument != '\0' && isdigit( *argument ); argument++ ) 
        number = 10*number+*argument-'0';
      if( *argument == '*' ) {
        number *= -1;
        argument++;  
        }
      else if( *argument == '.' )
        argument++;  
      }
    }

  for( ; *argument != '\0'; argument++ ) 
    *tmp++ = ( *argument == '.' ? ' ' : *argument );

  *tmp = '\0';

  return number;
}


/*
 *   FORMATTING STRINGS
 */


char* capitalize( char* arg )
{
  char* letter;

  if( *arg != '' ) {
    *arg = toupper( *arg );
    return arg;
    }

  for( letter = arg; *letter != '\0'; ) 
    if( *letter++ == 'm' ) {
      *letter = toupper( *letter );
      return arg;
      }

  bug( "Capitalize: Missing end of escape code?" );
  bug( "-- arg = %s", arg );

  return arg;
}


char* capitalize_words( const char* argument )
{
  char*  tmp  = static_string( );
  int      i; 

  *tmp = toupper( *argument );

  for( i = 1; argument[i] != '\0'; i++ ) 
    tmp[i] = ( argument[i-1] == ' ' ? toupper( argument[i] )
      : argument[i] );

  tmp[i] = '\0';

  return tmp;
}


void smash_spaces( char* tmp )
{
  int i;

  for( i = 0; i < strlen( tmp ); i++ )
    if( tmp[i] == ' ' )
      tmp[i] = '_';

  return;
}


void add_spaces( char* tmp, int i )
{
  if( i < 0 ) {
    roach( "Add_Spaces: Number to add negative." );
    return;
    }

  tmp += strlen( tmp );

  for( ; i > 0; i-- ) 
    *tmp++ = ' ';

  *tmp = '\0';

  return;
} 
