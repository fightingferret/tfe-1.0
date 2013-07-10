#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


const char* prog_msg( program_data* action, const char* msg,
  const char* standard )
{
  extra_data*  extra;

  if( action != NULL )
    for( int i = 0; i < action->data; i++ ) {
      extra = action->data[i];
      if( !strcasecmp( extra->keyword, msg ) )
        return extra->text;
      }

  return standard;
}


void show_defaults( char_data* ch, int trigger, const char*** data )
{
  char            tmp  [ TWO_LINES ];
  const char**   list;
  int               i;
    
  if( ( list = data[ trigger ] ) != NULL ) {
    page( ch, "Defaults:\n\r" );
    for( i = 0; *data[ trigger ][i] != '\0'; ) {
      sprintf( tmp, "[%s]  %s", list[i++], list[i++] ); 
      page( ch, tmp );
      }
    page( ch, "\n\r" );
    }

  return;
}
