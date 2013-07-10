#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void vote_summary( char_data* ch, int i )
{
  int  count_array  [ MAX_PFILE ];
  int    sort_array  [ 20 ];
  int         j, k;

  vzero( count_array, MAX_PFILE );

  for( j = 0; j < 20; j++ )
    sort_array[j] = -1;

  /* COUNT VOTES */

  for( j = 0; j < max_pfile; j++ )
    if( pfile_list[j]->vote[i] != NULL )
      count_array[pfile_list[j]->vote[i]->ident]++;

  /* SORT VOTES */

  for( j = 0; j < max_pfile; j++ ) {
     for( k = 20; k > 0 && ( sort_array[k-1] == -1
       || count_array[pfile_list[j]->ident]
       > count_array[sort_array[k-1]] ); k-- )
       if( k < 20 )
         sort_array[k] = sort_array[k-1];
    if( k < 20 )
      sort_array[k] = pfile_list[j]->ident;
    }

  /* DISPLAY VOTES */

  for( j = 0; j < 20; j++ )
    send( ch, "%d  %s\n\r", 
      sort_array[j] == -1 ? 0 : count_array[sort_array[j]], 
      sort_array[j] == -1 ? "noone" : ident_list[sort_array[j]]->name );
  
  return;
}

void do_vote( char_data* ch, char* argument )
{
  pfile_data*  pfile;
  int              i;

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_VOTE; i++ ) {
      pfile = ch->pcdata->pfile->vote[i];     
      send( ch, "[%d] %s\n", i+1, pfile == NULL ? "noone" : pfile->name );
      }
    return;
    }
  
  if( exact_match( argument, "results" ) ) {
    if( !number_arg( argument, i ) ) {
      send( ch, "What vote number do you want summarized?\n\r" );
      return;
      }
    if( i < 1 || i > MAX_VOTE ) {
      send( ch, "Voting only has %d slots.\n\r", MAX_VOTE );
      return;
      }
    vote_summary( ch, i-1 );
    return;
    }

  if( !number_arg( argument, i ) ) {
    send( ch, "Syntax: Vote # <player>\n\r" );
    return;
    }

  if( i < 1 || i > MAX_VOTE ) {
    send( ch, "Voting only has %d slots.\n\r", MAX_VOTE );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "You must vote for someone.\n\r" );
    return;
    }

  if( ( pfile = find_pfile( argument, ch ) ) == NULL )  {
    send( ch, "That player does not exist.\n\r" );
    return;
    }

  send( ch, "For slot %d you vote for %s.\n\r",
    i, pfile->name );

  ch->pcdata->pfile->vote[i-1] = pfile;

  return;
}  
