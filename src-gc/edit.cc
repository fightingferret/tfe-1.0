#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


char*        one_line          ( char*, char* );
void         indent            ( char*, char* );
void         display_text      ( char_data*, char* );


/*
 *   SUPPORT FUNCTIONS
 */


void display_text( char_data* ch, char* input )
{
  char  line  [ MAX_STRING_LENGTH ];
  char   buf  [ MAX_STRING_LENGTH ];
  int      i;

  for( i = 1; ; i += 2 ) {
    input = one_line( input, line );
    if( line[0] == '\0' )
      return;
    sprintf( buf, "[%2d]  %s", i, line );
    page( ch, buf );
    }
}


const char* break_line( const char* argument, char* line, int length )
{
  const char*   input  = argument;
  char*          word  = NULL;
  bool        newword  = TRUE;

  if( *argument == '\0' ) {
    *line = '\0';
    return argument;
    }

  for( ; *input != '\n' && *input != '\0' && input-argument < length; ) {
    if( *input == ' ' ) {
      if( newword ) {
        word = line;
        newword = FALSE;
        }
      }
    else {
      newword = TRUE;
      }
    *line++ = *input++;
    }

  if( *input == '\0' || *input == '\n' || word == NULL ) 
    word = line;

  strcpy( word, "\n\r" );

  argument = input+(word-line);
  skip_spaces( argument );

  return argument;
}


char* one_line( char* argument, char* line )
{
  if( *argument == '\0' ) {
    *line = '\0';
    }
  else {
    for( ; *argument != '\n' && *argument != '\0'; argument++, line++ )
      *line = *argument;

    *line = '\n';
    *(line+1) = '\r';
    *(line+2) = '\0';

    if( *argument == '\n' ) {
      argument++;
      if( *argument == '\r' )
        argument++;
      }
    }

  return argument;
}


/*
 *   SUB_PROCEDURES
 */


void format_tell( char* output, char* input )
{
  char*     word;
  char*     line;
  char*   letter;
  char*    start  = output;
  int     length;

  skip_spaces( input );
  sprintf( output, "  %c", '"' );

  line    = output;
  output += 3;
  word    = NULL; 
  length  = 73;

  for( ; *input != '\0'; ) {
    if( output-line > length ) {
      if( word == NULL ) 
        word = output;
      for( letter = output-1; letter > word; letter-- )
        *(letter+3) = *letter;
      memcpy( word, "\n\r  ", 4 );  
      output += 3;
      line    = word+2;
      word    = NULL;
      length  = 73;
      }
    if( isspace( *input ) ) {
      if( *input == '\n' && *(input+1) == '\r' )
        input++;
      word      = output;
      *output++ = ' ';
      }
    else {
      *output++ = *input;
      }
    input++;
    }

  for( ; isspace( *(output-1) ) && output > start; output-- );
  sprintf( output, "%c\n\r", '"' );

  return;
}


void format( char* output, const char* input )
{  
  char *word;
  char *line;
  const char *letter;
  char *start = output;

  for( ; isspace( *input ); input++ );
 
  line = output;
  word = NULL; 

  for( ; *input != '\0'; ) {
    if( output - line > 73 ) {
      if( word == NULL ) 
        word = output;
      for( line = output-1; line > word; line-- )
        *(line+1) = *line;
      output++;
      *word = '\n';
      *(word+1) = '\r';
      line = word+2;
      word = NULL;
      }
    if( isperiod( *input ) ) {
      do {
        *output = *input;
        input++;
        output++;
        } while( isperiod( *input ) );
      for( letter = input; *letter == ' '; letter++ );
      if( *letter == '\n' || *letter == '\0' )
        input = letter;  
      if( *input == '\0' )
        break;
      if( *input == ' ' ) {
        *output = ' ';
        output++;
        }
      }
    if( isspace( *input ) ) {
      word = output;
      *output = ' ';
      output++;
      do {
        input++;
        } while( isspace( *input ) );
      }
    *output = *input;
    input++;
    output++;
    }

  for( ; isspace( *(output-1) ) && output > start; output-- );
  *output = '\0';

  return;
}


/*
 *   FORMATS CODE 
 */


void indent( char* input, char* output )
{
  char     line  [ MAX_INPUT_LENGTH ];
  char*  letter;
  int     level  = 0;
  int    spaces  = 0;
  int         i;

  *output = '\0';

  for( ; ; ) {
    for( ; *input == ' '; input++ );
    input = one_line( input, line );

    if( *line == '\0' )
      break;

    for( i = 0; i < 2*level+spaces; i++ ) 
      *output++ = ' ';
 
    spaces = 0;
    strcpy( output, line );
    output += strlen( line );   

    if(  !strncmp( line, "if(", 3 )
      || !strncmp( line, "loop(", 5 )
      || !strncmp( line, "else", 4 )
      || !strncmp( line, "&&", 2 )
      || !strncmp( line, "||", 2 ) ) {
      for( letter = line; *letter != '\0' && *letter != '{'; letter++ );
      if( *letter == '{' )
        level++;
      else
        spaces = 2;
      }
    
    if( *line == '}' )
      level--;
    }

  *output = '\0';
  return;
}


/*
 *   MAIN ROUTINE
 */


char* edit_string( char_data* ch, char* argument, char* input,
  int mem_type )
{
  static char paragraph [ 3*MAX_STRING_LENGTH ];

  char buf  [ 3*MAX_STRING_LENGTH ];
  char line [ MAX_STRING_LENGTH ];
  char *pString, *word;
  int i, j, k;

  *buf       = '\0';
  *paragraph = '\0';
  pString    = input;

  if( !strcasecmp( argument, "undo" ) ) {
    if( ch->pcdata->buffer == empty_string ) {
      send( "You haven't edited anything since logging in.\n\r", ch );
      return pString;
      }
    pString = ch->pcdata->buffer;
    ch->pcdata->buffer = alloc_string( input, MEM_PLAYER );
    display_text( ch, pString );
    return pString;
    } 

  if( !strcmp( argument, "indent" ) ) {
    indent( input, paragraph );
    input = paragraph;
    }
  else if( *argument == '?' ) {
    argument = one_argument( ++argument, buf );
    i        = strlen( buf );
    word     = paragraph;

    for( ; *input != '\0'; ) {
      if( i > 0 && !strncmp( buf, input, i ) ) {
        strcpy( word, argument );   
        word  += strlen( argument );
        input += strlen( buf );
        }
      else {
        *word++ = *input++;
        }
      }
    *word = '\0';
    input = paragraph;
    }
  else if( *argument != '\0' ) {
    if( isdigit( *argument ) ) {
      for( i = 0; isdigit( *argument ); argument++ ) 
        i = 10*i + *argument - '0';
      }
    else
      i = -1;

    if( *argument == '-' && i != -1 ) {
      for( j = 0, argument++; isdigit( *argument ); argument++ )  
        j = 10*j + *argument - '0';
   
      if( j < i ) {
        send( "Format range incorrect.\n\r", ch );
        return pString;
        }

      for( k = 1; k < i; k += 2 ) {
        input = one_line( input, line );
        strcat( paragraph, line );
        }

      for( ; k <= j; k += 2 ) {
        input = one_line( input, line ); 
        if( line[ 0 ] == '\0' )
          break;
        line[ strlen( line ) - 2 ] = '\0';
        strcat( buf, line );
        strcat( buf, " " );
        }

      format( line, buf );
      strcat( paragraph, line );
      strcat( paragraph, "\n\r" );

      for( ; ; ) {
        input = one_line( input, line );
        if( *line == '\0' )
          break;
        strcat( paragraph, line );
        }
      }
    else {
      if( strlen( pString ) > 2*MAX_STRING_LENGTH && *argument != '\0' ) {
        send( "Due to internal limits files can be no longer then 6k\
 characters.\n\rPushing this will crash the mud.\n\r", ch );
        return pString;
        }
      if( *argument == ' ' && i != -1 )
        argument++;

      for( k = 1; ; k += 2 ) {
        if( ( k == i && *argument != '\0' ) || i + 1 == k ) {
          strcat( paragraph, argument );
          strcat( paragraph, "\n\r" );
          }
        input = one_line( input, line );
        if( line[0] == '\0' ) {
          if( i == -1 ) {
            strcat( paragraph, argument );
            strcat( paragraph, "\n\r" );
	    }
          break;
	  }
        if( k != i )
          strcat( paragraph, line );
        }
      }
    input = paragraph;
    }

  free_string( ch->pcdata->buffer, MEM_PLAYER );
  ch->pcdata->buffer = pString;  

  display_text( ch, input );

  return alloc_string( input, mem_type );
}





