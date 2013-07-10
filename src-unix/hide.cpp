#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


void spoil_hide( char_data* ch )
{
  char_data* rch;
 
  if( !is_set( &ch->status, STAT_HIDING ) )
    return;

  remove_bit( &ch->status, STAT_HIDING );

  for( int i = 0; i < ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && ch->Seen( rch )
      && !includes( ch->seen_by, rch ) ) {
      send( rch, "++ You notice %s hiding in the shadows! ++\n\r", ch );
      ch->seen_by += rch;
      }

  set_bit( &ch->status, STAT_HIDING );
}


