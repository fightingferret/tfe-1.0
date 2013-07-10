#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void   recruit_member        ( char_data *ch, char_data *member );
void   add_to_faction        ( char_data *ch, int faction );
void   remove_from_faction   ( char_data *ch, int faction );
char*  relation_string       ( int a, int b );


bool are_allied( char_data* c1, char_data* c2 )
{
  if( ( c1->shdata->race > RACE_LIZARD ) == 
    ( c2->shdata->race > RACE_LIZARD ) )
    return TRUE;

  return FALSE;
}


bool associate( char_data* c1, char_data* c2 )
{
  if( are_allied( c1, c2 ) )
    return TRUE;

  const char* race = race_table[ c2->shdata->race ].name;

  send( c1, "You would never associate with a%s %s.\n\r",
     isvowel( *race ) ? "n" : "", race );
      
  return FALSE;
}


/*
 *   MODIFY ROUTINE
 */


void modify_reputation( char_data*, char_data*, int )
{
  /*
  char             tmp  [ MAX_STRING_LENGTH ];
  pfile_data*    pfile;
  int         vic_clan  = victim->shdata->clan;
  int          ch_clan  = ch->shdata->clan;
  int              mod;
  int                i;
  
  if( is_set( &ch->in_room->room_flags, RFLAG_ARENA ) ) 
    return;
 
  if( ch->species != NULL || vic_clan < 2 || victim == ch )
    return;

  pfile = ch->pcdata->pfile;

  if( type == REP_STOLE_FROM ) {
    pfile->reputation[vic_clan-2] += IS_NPC( victim ) ? -100 : -200;
    send( ch, "Your reputation changes for stealing from %s.\n\r",
      victim->Seen_Name( ch ) ); 
    }

  if( type == REP_ATTACKED ) {
    pfile->reputation[vic_clan-2] += IS_NPC( victim ) ? -100 : -200; 
    send( ch, "Your reputation changes for attacking %s.\n\r",
      victim->Seen_Name( ch ) ); 
    }
  
  if( type == REP_KILLED ) {
    for( i = 2; i < MAX_ENTRY_CLAN; i++ ) {
      mod = ( victim->species == NULL ? -300 : -500 );
      if( i != vic_clan )
        mod /= 3;
      mod = mod*(clan_table[vic_clan].relation[i-2]-50)/100;
      pfile->reputation[i-2] += mod;
      }
    send( ch, "Your reputation changes for killing %s.\n\r",
      victim->Seen_Name( ch ) ); 
    }

  if( ch_clan >= 2 && ch_clan < MAX_ENTRY_CLAN
    && pfile->reputation[ch_clan-2] < 0 ) {
    send( ch, "%sYou are cast out of your clan!!%s\n\r",
      bold_v( ch ), normal( ch ) );  
    sprintf( tmp, "%s is cast out of the %s clan for fighting %s.",
      ch->descr->name, clan_table[ch_clan].name, victim->descr->name );
    info( tmp, LEVEL_BUILDER, tmp, IFLAG_CLANS, 1, ch );
    ch->shdata->clan = CLAN_NONE;
    }
    */
  return;
}


/*
 *   REPUTATION COMMAND
 */

 
index_data reputation_index [] =
{ 
  { "abhorred",  "",   -600 },
  { "despised",  "",   -400 },
  { "hated",     "",   -200 },
  { "wanted",    "",    -50 },
  { "suspect",   "",      0 },
  { "dubious",   "",     50 },
  { "accepted",  "",    200 },
  { "credible",  "",    400 },
  { "reputable", "",    600 },
  { "esteemed",  "",     -1 },
};


void do_reputation( char_data* ch, char* argument )
{
  player_data*     pc  = (player_data*) ch;
  char_data*   victim;

  if( not_player( ch ) )
    return;

  if( ch->shdata->level >= LEVEL_APPRENTICE && *argument != '\0' ) {
    if( ( victim = one_character( ch, argument, "show reputation",
      ch->array, (thing_array*) &player_list ) ) == NULL )
      return;
    if( ( pc = player( victim ) ) == NULL ) {
      send( ch, "Reputation cannot act on npcs.\n\r" );
      return;
      }
    }

  page_underlined( ch, "Sacrifices\n\r" );
  page( ch, "%17s: %d\n\r", "Cp", pc->reputation.gold );
  page( ch, "%17s: %d\n\r", "Blood", pc->reputation.blood );
  page( ch, "%17s: %d\n\r", "Magic", pc->reputation.magic );
  page( ch, "\n\r" );

  page_underlined( ch, "Nation Grievances\n\r" );
  for( int i = 1; i < MAX_ENTRY_NATION; i++ ) 
    page( ch, "%17s: %d\n\r", nation_table[i].name,
      pc->reputation.nation[i] );

  int total = 0;

  for( int i = 0; i < MAX_ENTRY_ALIGNMENT; i++ ) 
   total += pc->reputation.alignment[i];

  if( total == 0 )
    return;

  page( ch, "\n\r" );
  page_underlined( ch, "Alignment Exp (%)\n\r" );

  for( int i = 0; i < MAX_ENTRY_ALIGNMENT; i++ ) 
    page( ch, "%17s: %6.2f\n\r", alignment_table[i].name,
      (float) 100*pc->reputation.alignment[i]/total );
}


/*
 *   WANTED COMMAND
 */


void do_wanted( char_data*, char* )
{
  /*
  char   buf  [ MAX_STRING_LENGTH ];
  int   clan  = ch->shdata->clan;
  int  count;
  int      i; 
 
  if( is_confused_pet( ch ) )
    return;

  send( "Command is disabled.\n\r", ch );
  return;

  if( argument[0] != '\0' ) {
    for( clan = CLAN_IMMORTAL+1; clan < MAX_ENTRY_CLAN; clan++ ) 
      if( !strncasecmp( argument, clan_table[clan].name,
        strlen( argument ) ) )
        break;

    if( clan == MAX_ENTRY_CLAN ) {
      send( ch, "That clan is unknown.\n\r" );
      return;
      }
    }

  if( clan >= MAX_ENTRY_CLAN ) {
    send( ch, "Your clan does not have a wanted list.\n\r" );
    return;
    }

  count = 0;
  page_title( ch, "WANTED" );
  sprintf( buf, "%24sName%7sBounty  Kills\n\r", " ", " " );
  page( ch, buf );

  for( i = 0; i < max_pfile; i++ ) {
    if( pfile_list[i] != NULL && pfile_list[i]->reputation[clan] < 0 ) {
      sprintf( buf, "%24s%-10s  %-7d  %-5d\r\n", "", pfile_list[i]->name,
        -pfile_list[i]->reputation[clan], 0 );
      page( ch, buf );
      count++;
      }
    }

  if( count == 0 )
    page(
      "                        Wanted list is currently empty.\n\r", ch );
      */
  return;
}

/*
 *   RELATIONS COMMAND
 */


char* relation_string( int rel )
{
       if( rel > 90 )  return "+++";
  else if( rel > 75 )  return " ++";
  else if( rel > 55 )  return " + ";
  else if( rel > 45 )  return " = ";
  else if( rel > 35 )  return " - ";
  else if( rel > 15 )  return " --";
  else if( rel > -1 )  return "---";
                       return "???";
}


void do_relations( char_data* ch, char* )
{
  char   tmp  [ MAX_STRING_LENGTH ];

  page_title( ch, "Nation vs Race" );

  page( ch, "\n\r%16s", "" );
  for( int i = 0; i < MAX_PLYR_RACE; i++ )
    sprintf( tmp+i*4, "%s ", race_table[i].abbrev );
  strcat( tmp, "\n\r" );
  page( ch, tmp );

  for( int i = 2; i < MAX_ENTRY_NATION; i++ ) {
    page( ch, "%11s%s  ", "", nation_table[i].abbrev );
    for( int j = 0; j < MAX_PLYR_RACE; j++ ) 
      page( ch, "%3s%s", relation_string( nation_table[i].race[j] ),
        j != MAX_PLYR_RACE-1 ? " " : "\n\r" );
    }

  page( ch, "\n\r" );
  page_title( ch, "Nation vs Alignment" );

  page( ch, "\n\r%24s", "" );
  for( int i = 0; i < MAX_ENTRY_ALIGNMENT; i++ )
    sprintf( tmp+i*4, "%s  ", alignment_table[i].abbrev );
  strcat( tmp, "\n\r" );
  page( ch, tmp );

  for( int i = 2; i < MAX_ENTRY_NATION; i++ ) {
    page( ch, "%19s%s  ", "", nation_table[i].abbrev );
    for( int j = 0; j < MAX_ENTRY_ALIGNMENT; j++ ) 
      page( ch, "%3s%s", relation_string( nation_table[i].alignment[j] ),
        j != MAX_ENTRY_ALIGNMENT-1 ? " " : "\n\r" );
    } 

  page( ch, "\n\r" );
  page_centered( ch, "[ See help relations for explanation of table ]" );
}


