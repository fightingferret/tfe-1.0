#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


bool is_switched( char_data* ch )
{
  if( ch->pcdata == NULL || ch->species == NULL )
    return FALSE;

  send( "You can't do that while switched.\n\r", ch );
  return TRUE;
}
