#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void press_return( link_data* link )
{
  send( link, "\n\rPress <return> to continue." );     
  return;
}


bool no_input( link_data* link, char* argument )
{
  if( *argument != '\0' )
    return FALSE;

  send( link, "\n\r>> No input received - closing link <<\n\r" );
  close_socket( link, TRUE );

  return TRUE;
}


void set_playing( link_data* link )
{
  link_data* prev;

  link->connected = CON_PLAYING;

  remove( link_list, link );

  if( link_list == NULL || ( link_list->connected == CON_PLAYING 
    && strcmp( link_list->pfile->name, link->pfile->name ) > 0 ) ) {
    link->next = link_list;
    link_list  = link;
    return;
    }

  for( prev = link_list; prev->next != NULL; prev = prev->next ) 
    if( prev->next->connected == CON_PLAYING 
      && strcmp( prev->next->pfile->name, link->pfile->name ) > 0 )
      break;

  link->next = prev->next;
  prev->next = link;
}


bool game_wizlocked( link_data* link )
{
  if( !wizlock ) 
    return FALSE;

  help_link( link, "Game_Wizlocked" );
  close_socket( link, TRUE );

  return TRUE;
} 


/*
 *   ENTERING GAME ROUTINE
 */


room_data* get_temple( char_data* ch )
{
  room_data*  room  = NULL;
  
  if( ch->shdata->race < MAX_PLYR_RACE ) 
    room = get_room_index( plyr_race_table[
      ch->shdata->race ].start_room[ ch->shdata->alignment%3 ] );

  if( room == NULL )
    room = get_room_index( ROOM_CHIIRON_TEMPLE );

  return room;
}


room_data* get_portal( char_data* ch )
{
  room_data*  room  = NULL;
  
  if( ch->shdata->race < MAX_PLYR_RACE ) 
    room = get_room_index( plyr_race_table[
      ch->shdata->race ].portal );

  if( room == NULL )
    room = get_temple( ch );

  return room;
}



void enter_room( char_data* ch )
{
  char_data*   pet;
  room_data*  room; 
  int            i;

  if( ( !is_set( ch->pcdata->pfile->flags, PLR_CRASH_QUIT )
    && current_time-boot_time > 30*60 )
    || ch->pcdata->pfile->last_on > boot_time 
    || ch->shdata->level >= LEVEL_APPRENTICE
    || !is_set( ch->pcdata->pfile->flags, PLR_PORTAL ) ) {   
    ch->To( ch->was_in_room == NULL ? get_temple( ch )
      : ch->was_in_room );
    return;
    }
  
  room = get_portal( ch );

  for( i = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i]; 
    if( pet->was_in_room == ch->was_in_room )
      pet->was_in_room = room;
    }

  ch->To( room );
}


void enter_game( char_data* ch )
{
  char          tmp1  [ TWO_LINES ];
  char          tmp2  [ TWO_LINES ];
  char_data*     pet;
  pfile_data*  pfile  = ch->pcdata->pfile;
  int              i;

  if( ch->shdata->level == 0 ) 
    new_player( player( ch ) );
  else
    enter_room( ch );

  if( ch->position == POS_FIGHTING )
    ch->position = POS_STANDING;
   
  if( ch->shdata->race < MAX_PLYR_RACE ) {
    ch->affected_by[0] |= plyr_race_table[ch->shdata->race].affect[0];
    ch->affected_by[1] |= plyr_race_table[ch->shdata->race].affect[1];
    }
  
  sprintf( tmp1, "%s@%s has connected.", ch->descr->name, ch->link->host );
  sprintf( tmp2, "%s has connected.", ch->descr->name );

  info( tmp2, LEVEL_DEMIGOD, tmp1, IFLAG_LOGINS, 1, ch );

  ch->was_in_room = NULL;
  send_seen( ch, "%s has entered the game.\n\r", ch );

  clear_screen( ch );
  do_look( ch, "" );

  for( i = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( pet->was_in_room == NULL )
      pet->was_in_room = get_temple( ch );
    pet->To( pet->was_in_room );
    send_seen( pet, "%s comes out of the void.\n\r", pet );
    pet->was_in_room = NULL; 
    }      

  send( "\n\r", ch );
  send_centered( ch, "---==|==---" );
  send( "\n\r", ch );

  if( pfile->last_host[0] != '\0' ) {
    sprintf( tmp2, "Last login was %s from %s.",
      ltime( pfile->last_on ), pfile->last_host );
    }
  else 
    sprintf( tmp2, "Connection is from %s.", ch->link->host );
  send_centered( ch, tmp2 );

  if( pfile->guesses > 0 ) {
    send( ch, "\n\r" );  
    send_centered( ch, "INCORRECT PASSWORD ATTEMPTS: %d",
      pfile->guesses );
    pfile->guesses = 0;
    }

  if( pfile->account != NULL )
    pfile->account->last_login = current_time;

  if( strcasecmp( ch->link->host, pfile->last_host ) ) {
    remove_list( site_list, site_entries, pfile );
    free_string( pfile->last_host, MEM_PFILE );
    pfile->last_host = alloc_string( ch->link->host, MEM_PFILE );
    add_list( site_list, site_entries, pfile );
    }

  mail_message( ch );
  auction_message( ch );
  recent_notes( ch );

  set_bit( pfile->flags, PLR_SAFE_KILL );   
  set_bit( pfile->flags, PLR_CRASH_QUIT );
  set_bit( &ch->status, STAT_IN_GROUP );   
  remove_bit( &ch->status, STAT_BERSERK );
  remove_bit( &ch->status, STAT_REPLY_LOCK );
  remove_bit( &ch->status, STAT_FORCED ); 

  ch->timer = current_time;

  reconcile_recognize( ch );
}


/*
 *   NANNY ROUTINES
 */


void nanny_intro( link_data* link, char* argument )
{
  bool         suppress_echo;
  pfile_data*          pfile;

  for( ; !isalnum( *argument ) && *argument != '+' && *argument != '-'
    && *argument != '\0'; argument++ );

  switch( atoi( argument ) ) {
   case 1:
    help_link( link, "Acnt_Menu" );
    send( link, "                   Choice: " );
    link->connected = CON_ACNT_MENU;
    return;
   case 2:
    if( !game_wizlocked( link ) ) {
      help_link( link, "Enter_Acnt" );
      send( link, "Account: " );
      link->connected = CON_ACNT_ENTER;
      }
    return;
   case 3:
    help_link( link, "Features_1" );      
    link->connected = CON_FEATURES;
    return;
   case 4:
    help_link( link, "Policy_1" );
    link->connected = CON_POLICIES;
    return;
   case 5:
    help_link( link, "Digitalnation_1" );
    link->connected = CON_DIGITALNATION;
    return;
   case 6:
    help_link( link, "Problems" );
    link->connected = CON_PAGE;
    return;
    }
 
  if( suppress_echo = ( *argument == '-' ) )
    argument++; 

  if( *argument == '\0' ) {
    send( link, "\n\r>> No choice made - closing link. <<\n\r" ); 
    close_socket( link, TRUE );
    return;
    }

  pfile        = find_pfile_exact( argument );  
  link->pfile  = pfile;

  if( pfile == NULL ) {
    help_link( link, "Unfound_Char" );
    link->connected = CON_PAGE;
    return;
    }

  if( pfile->trust < LEVEL_APPRENTICE
    && game_wizlocked( link ) ) 
    return;

  if( is_banned( pfile->account, link ) )
    return;

  send( link, "                 Password: " );

  if( !suppress_echo ) {
    send( link, echo_off_str ); 
    link->connected = CON_PASSWORD_ECHO;
    }
  else {
    link->connected = CON_PASSWORD_NOECHO;
    }
}


/*
 *   NAME/PASSWORD
 */


void nanny_old_password( link_data* link, char* argument )
{
  char              buf  [ ONE_LINE ];
  player_data*       ch  = link->player;
  player_data*   ch_old;
  link_data*   link_old;

  send( link, "\n\r" );

  if( link->connected == CON_PASSWORD_ECHO )
    send( link, echo_on_str ); 

  if( strcmp( argument, link->pfile->pwd ) ) {
    if( *argument != '\0' && link->pfile->guesses++ > 25 ) {
      bug( "Attempting to crack password for %s?", link->pfile->name );
      bug( "--     Site: %s", link->host );
      bug( "-- Attempts: %d", link->pfile->guesses );
      bug( "--    Guess: %s", argument );
      } 
    help_link( link, "Wrong_Password" );
    close_socket( link, TRUE );
    return;
    }

  for( link_old = link_list; link_old != NULL; link_old = link_old->next ) 
    if( link_old != link && link_old->player != NULL 
      && link_old->pfile == link->pfile
      && past_password( link_old ) ) {
      send( link, "Already playing!\n\rDisconnect previous link? " );
      link->connected = CON_DISC_OLD;
      return;
      }

  for( int i = 0; i < player_list; i++ ) {
    ch_old = player_list[i];
    if( ch_old->Is_Valid( )
      && ch_old->pcdata->pfile == link->pfile ) {
      link->character = ch_old;
      link->player    = ch_old;
      ch_old->link    = link;
      ch_old->timer   = current_time;
      sprintf( buf, "%s@%s reconnected.", ch_old->descr->name, link->host );
      info( "", LEVEL_DEMIGOD, buf, IFLAG_LOGINS );
      set_playing( link );
      setup_screen( ch_old );
      send( ch_old, "Reconnecting.\n\r" );
      send_seen( ch_old, "%s has reconnected.\n\r", ch_old );
      return;
      }
    }

  if( !load_char( link, link->pfile->name, PLAYER_DIR ) ) {
    send( link, "\n\r+++ Error getting player file +++\n\r" );
    bug( "Error finding player file" );
    bug( "-- Player = '%s'", link->pfile->name );
    close_socket( link );
    return;
    }

  ch = link->player;

  if( get_trust( ch ) < LEVEL_APPRENTICE ) {
    nanny_imotd( link, "" );
    return;
    }

  send( link, "\n\r" );
  clear_screen( ch );
  do_help( ch, "imotd" );
  press_return( link );

  link->connected = CON_READ_IMOTD;     
}
    

/*
 *   MESSAGE OF THE DAY ROUTINES
 */


void nanny_imotd( link_data* link, char* )
{
  send( link, "\n\r" );
  clear_screen( link->character );

  do_help( link->character, "motd" );
  press_return( link );

  link->connected = CON_READ_MOTD;

  return;
}


void nanny_motd( link_data* link, char* )
{
  char_data*   ch  = link->character;

  send( link, "\n\r" );
  setup_screen( ch );
  set_playing( link );
  enter_game( ch );

  return;
}


void nanny_disc_old( link_data* link, char* argument )
{
  link_data* link_prev  = link;
 
  if( toupper( *argument ) != 'Y' ) {
    send( link, "Ok.  Good Bye then.\n\r" );
    close_socket( link, TRUE );
    return;
    }

  for( ; link_prev != NULL; link_prev = link_prev->next ) 
    if( link_prev != link && link_prev->pfile == link->pfile
      && past_password( link_prev ) ) 
      break;

  if( link_prev == NULL ) {
    link->connected = CON_PASSWORD_NOECHO;;
    nanny( link, link->pfile->pwd );
    return;
    }

  swap( link_prev->channel, link->channel );
  swap( link_prev->host,    link->host );

  send( link, "\n\r\n\r+++ Link closed by new login +++\n\r" );
  close_socket( link, TRUE );

  switch( link_prev->connected ) {
    case CON_PLAYING    : do_look( link_prev->character, "" );   break;
    case CON_READ_MOTD  : 
    case CON_READ_IMOTD : nanny_motd( link_prev, "" );           break;
    } 
}


