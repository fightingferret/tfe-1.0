#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "syslog.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"


const char* noteboard_name [ MAX_NOTEBOARD ] = { "general", "immortal",
  "ideas",
  "bugs", "jobs", "announcements", "information", "stories", "changes",
  "wanted", "fixed", "code", "avatar", "clan" };


note_data**  note_list [ MAX_NOTEBOARD ];
int          max_note  [ MAX_NOTEBOARD ];


void         save_mail        ( pfile_data* );   
bool         can_read         ( char_data*, int, bool = FALSE );
bool         can_write        ( char_data*, int, bool = FALSE );
bool         can_remove       ( char_data*, note_data* );
note_data*   find_mail        ( char_data*, int );
void         note_summary     ( char_data* );
void         display_notes    ( player_data* );
void         display_mail     ( char_data* );
void         send_mail        ( char_data*, char, char* );    
void         save_noteboard   ( char*, note_data**, int );
void         load_noteboard   ( char*, note_data**&, int&, int );


inline int noteboard_level( int board )
{
  if( board == NOTE_IMMORTAL || board == NOTE_JOBS
    || board == NOTE_CHANGES )
    return LEVEL_APPRENTICE;

  return( board == NOTE_AVATAR ? LEVEL_AVATAR : 0 );
}


/*
 *   NOTE_DATA CLASS
 */


Note_Data :: Note_Data( )
{
  record_new( sizeof( note_data ), MEM_NOTE );

  next      = NULL;
  from      = empty_string;
  title     = empty_string;
  message   = empty_string;
  date      = 0;
  noteboard = 0;
};


Note_Data :: ~Note_Data( )
{
  record_delete( sizeof( note_data ), MEM_NOTE );

  free_string( title,   MEM_NOTE );
  free_string( message, MEM_NOTE );
  free_string( from,    MEM_NOTE );
};


void extract( char_data* ch, note_data* note )
{
  player_data*    victim;
  int          noteboard  = note->noteboard;
  int                  i;

  for( i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( victim->note_edit == note ) {
      if( victim != ch )
        send( victim,
          "%s just deleted the note you were editing.\n\r", ch );
      victim->note_edit = NULL;
      }
    } 

  for( i = 0; i < max_note[noteboard]; i++ )
    if( note_list[noteboard][i] == note ) {
      remove( note_list[noteboard], max_note[noteboard], i );
      delete note;
      return;
      }

  bug( "Extract( note ): Non-existent note!?" );
}        


/*
 *   SUPPORT FUNCTIONS
 */


bool acceptable_title( char_data* ch, const char* title )
{ 
  if( strlen( title ) <= 40 ) 
    return TRUE;

  send( "The title of messages must be less than 40 characters.\n\r", ch );
  return FALSE;
}


void recent_notes( char_data* ch )
{
  char           tmp  [ TWO_LINES ];
  int           i, j;
  int         recent  = 0;
  clan_data*    clan;

  if( ( clan = ch->pcdata->pfile->clan ) != NULL ) {
    note_list[ NOTE_CLAN ] = clan->note_list;
    max_note[ NOTE_CLAN ]  = clan->max_note;
    }

  for( recent = i = 0; i < MAX_NOTEBOARD; i++ ) 
    if( can_read( ch, i ) ) 
      for( j = max_note[i]-1; j >= 0; j--, recent++ ) 
        if( note_list[i][j]->date < ch->pcdata->pfile->last_on )
          break;

  if( recent > 0 ) {
    sprintf( tmp, "%s note%s been posted since last login.",
      number_word( recent ), recent == 1 ? " has" : "s have" );
    *tmp = toupper( *tmp );
    send_centered( ch, tmp );
    }

  return;
}


bool can_remove( char_data* ch, note_data* note )
{
  title_data* title;

  if( !strcmp( ch->descr->name, note->from )
    || has_permission( ch, PERM_NOTEBOARD ) )
    return TRUE;

  if( note->noteboard == NOTE_BUGS && is_apprentice( ch ) )
    return TRUE;

  if( note->noteboard == NOTE_CLAN
    && ( title = get_title( ch->pcdata->pfile ) ) != NULL 
    && is_set( title->flags, TITLE_REMOVE_NOTES ) ) 
    return TRUE;

  return FALSE;
}


bool can_write( char_data* ch, int i, bool msg )
{
  if( !is_apprentice( ch ) && ( i == NOTE_IMMORTAL || i == NOTE_JOBS
    || i == NOTE_CHANGES || i == NOTE_FIXED || i == NOTE_ANNOUNCEMENTS
    || i == NOTE_CODE ) ) {
    if( msg ) 
      send( ch, "Only immortals can write on the %s noteboard.\n\r",
        noteboard_name[i] );
    return FALSE;
    }
  if( i == NOTE_AVATAR && !is_avatar( ch )  ) {
    if( msg )
      send( ch, "You aren't an avatar and cannot write to this board.\n\r" );
    return FALSE;
    }

  if( i == NOTE_CLAN && ch->pcdata->pfile->clan == NULL ) {
    if( msg )
      send( ch, "You aren't in a clan and thus have no noteboard.\n\r" );
    return FALSE;
    }

  return TRUE;
}


bool can_read( char_data* ch, int i, bool msg )
{ 
  if( noteboard_level( i ) > ch->pcdata->trust ) {
    if( msg )
      send( ch, "Only immortals can read the %s noteboard.\n\r",
        noteboard_name[i] );
    return FALSE;
    }

  if( i == NOTE_CLAN && ch->pcdata->pfile->clan == NULL ) {
    if( msg )
      send( ch, "You aren't in a clan and thus have no noteboard.\n\r" );
    return FALSE;
    }

  return TRUE;
}
 

int find_note( char_data* ch, int i, char* argument )
{
  int number = atoi( argument )-1;

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    number = max_note[i]-number-1;

  if( number < 0 || number >= max_note[i] ) {
    send( ch, "There is no note by that number.\n\r" );
    return -1;
    }

  return number;
}
    

int find_noteboard( char_data* ch, char*& argument, bool msg = FALSE )
{
  int i;

  for( i = 0; i < MAX_NOTEBOARD; i++ ) 
    if( matches( argument, noteboard_name[i] ) ) 
      return i;

  if( msg ) 
    send( ch, "No such noteboard exists.\n\r" );

  return -1;
}


void reverse( note_data*& list )
{
  note_data*  temp;
  note_data*  pntr;

  pntr = list;
  list = NULL;

  for( ; pntr != NULL; ) {
    temp       = pntr->next;
    pntr->next = list;
    list       = pntr;
    pntr       = temp;
    }

  return;
}


/*
 *   NOTEBOARD ROUTINE
 */


void do_notes( char_data* ch, char* argument )
{
  char            tmp1  [ 3*MAX_STRING_LENGTH ];
  char            tmp2  [ THREE_LINES ];
  note_data*      note;
  player_data*      pc;
  wizard_data*     imm;
  clan_data*      clan;
  int        noteboard;
  int           number;
  int                i;

  if( is_mob( ch ) ) 
    return;

  pc        = player( ch );
  imm       = wizard( ch );
  noteboard = pc->noteboard;

  if( ( clan = ch->pcdata->pfile->clan ) != NULL ) {
    note_list[ NOTE_CLAN ] = clan->note_list;
    max_note[ NOTE_CLAN ]  = clan->max_note;
    }

  if( ( note = pc->note_edit ) == NULL ) {
    if( *argument == '\0' ) {
      display_notes( pc );
      return;
      }

    if( ( i = find_noteboard( ch, argument ) ) != -1 ) {
      if( can_read( ch, i, TRUE ) ) {
        pc->noteboard = i;
        send( ch, "Note now works on the %s noteboard. ( %d notes )\n\r",
          noteboard_name[i], max_note[i] );
        }
      return;
      }

    if( fmatches( argument, "summary" ) ) {
      note_summary( ch );
      return;
      }

    if( matches( argument, "move" ) ) {
      if( ( noteboard = find_noteboard( ch, argument, TRUE ) ) == -1
        || !can_write( ch, noteboard, TRUE )
        || ( i = find_note( ch, pc->noteboard, argument ) ) == -1 )
        return;
      
      note = note_list[ pc->noteboard ][i];
      if( strcmp( ch->descr->name, note->from )
        && !has_permission( ch, PERM_NOTEBOARD ) ) {
        send( ch, "You are unable to move that note.\n\r" );
        return;
        }
      if( noteboard == pc->noteboard ) {
        send( ch, "The note is already on the %s board.\n\r",
          noteboard_name[noteboard] );
        return;
        }
      remove( note_list[ pc->noteboard ],
        max_note[ pc->noteboard ], i );
      insert( note_list[noteboard], max_note[noteboard], note,
        max_note[noteboard] );
      send( ch, "Note '%s' moved to %s board.\n\r",
        note->title, noteboard_name[noteboard] );
   
      sprintf( tmp1, "Note '%s' moved by %s to %s board from %s board.",
        note->title, ch->descr->name, noteboard_name[ noteboard ],
        noteboard_name[ pc->noteboard ] );
      info( empty_string, noteboard_level( noteboard ),
        tmp1, MAX_IFLAG+noteboard, 2, ch );
      note->noteboard = noteboard;
      save_notes( noteboard, clan );
      save_notes( pc->noteboard, clan );
      return;
      }

    if( matches( argument, "delete" ) ) {
      if( !strcasecmp( argument, "all" ) ) {
        if( !has_permission( ch, PERM_SHUTDOWN, TRUE ) ) 
          return;
        if( max_note[noteboard] == 0 ) {
          send( ch, "The %s noteboard is already clear.\n\r",
            noteboard_name[noteboard] );
          return;
	  }
        for( ; max_note[noteboard] > 0; ) 
          extract( ch, note_list[noteboard][0] );
        send( ch, "You clear the %s noteboard.\n\r", 
          noteboard_name[noteboard] );
        sprintf( tmp1, "%s noteboard cleared by %s.",
          noteboard_name[noteboard], ch->real_name( ) );
        *tmp1 = toupper( *tmp1 );
        info( empty_string, noteboard_level( noteboard ),
          tmp1, MAX_IFLAG+noteboard, 1, ch );
        save_notes( noteboard, clan );
        return;
        }
      if( *argument == '\0' ) {
        send( ch, "Which note do you wish to delete?\n\r" );
        return;
        }
      if( ( i = find_note( ch, noteboard, argument ) ) == -1 )
        return;
      note = note_list[noteboard][i];
      if( !can_remove( ch, note ) ) {
        send( ch, "You can't remove that note.\n\r" );
        return;
        }
      sprintf( tmp1, "Note '%s' deleted from the %s board by %s.",
        note->title, noteboard_name[noteboard], ch->descr->name );
      sprintf( tmp2, "Note '%s' deleted from the %s board by a mail daemon.",
        note->title, noteboard_name[noteboard] );
      info( tmp2, max( imm == NULL ? 0 : imm->wizinvis,
        noteboard_level( noteboard ) ),
        tmp1, MAX_IFLAG+noteboard, 3, ch, 
        noteboard == NOTE_CLAN ? ch->pcdata->pfile->clan : NULL );
      extract( ch, note );
      save_notes( noteboard, clan );
      send( ch, "Note removed.\n\r" );
      return;
      }

    if( isdigit( *argument ) ) {
      if( ( i = find_note( ch, noteboard, argument ) ) == -1 )
        return;
      note = note_list[noteboard][i]; 
      page( ch, "Title: %s\n\rFrom: %s\n\r\n\r",
        note->title, note->from );
      convert_to_ansi( ch, note->message, tmp1 );
      page( ch, tmp1 );
      return;
      }

    if( matches( argument, "edit" ) ) {
      if( ( number = atoi( argument )-1 ) < 0
        || number >= max_note[noteboard] ) {
        send( ch, "There is no note by that number.\n\r" );
        return;
        }

      if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
        number = max_note[noteboard]-number-1;
      note = note_list[noteboard][number];

      if( strcmp( ch->descr->name, note->from )
        && !has_permission( ch, PERM_NOTEBOARD ) ) {
        send( ch, "You can't edit that note.\n\r" );
        return;
        }

      send( ch, "You now edit that note.\n\r" );
      pc->note_edit = note;
      return;
      }
 
    if( !can_write( ch, noteboard, TRUE ) )
      return;

    if( !acceptable_title( ch, argument ) )
      return;

    note              = new note_data;
    note->title       = alloc_string( argument, MEM_NOTE );
    note->from        = alloc_string( ch->descr->name, MEM_NOTE );
    note->noteboard   = -1;
    pc->note_edit = note;
    send( ch, "Note created.\n\r" );
    return;
    }

  if( !strcasecmp( argument, "delete" ) ) {
    if( note->noteboard != -1 ) {
      send( ch, "You stop editing the note.\n\r" );
      pc->note_edit = NULL;
      return;
      }
    delete note;
    pc->note_edit = NULL;
    send( ch, "Message deleted.\n\r" );
    return;
    }

  if( !strncasecmp( "title ", argument, 6 ) ) {
    argument += 6;
    if( !acceptable_title( ch, argument ) )
      return;
    free_string( note->title, MEM_NOTE );
    note->title = alloc_string( argument, MEM_NOTE );
    send( ch, "Note title changed.\n\r" );
    return;
    }

  if( !strcasecmp( "post", argument ) ) {
    note->date = current_time;
    if( note->noteboard == -1 ) {
      note->noteboard = noteboard;
      send( ch, "Note posted on %s board.\n\r",
        noteboard_name[noteboard] );
      sprintf( tmp1, "Note posted by %s on the %s board.",
        ch->descr->name, noteboard_name[noteboard] );
      info( empty_string, noteboard_level( noteboard ),
        tmp1, MAX_IFLAG+noteboard, 1, ch, 
        noteboard == NOTE_CLAN ? ch->pcdata->pfile->clan : NULL );
      }
    else {
      send( ch, "Modifications saved.\n\r" );
      sprintf( tmp1, "Note '%s' on %s board modified by %s.",
        note->title, noteboard_name[ note->noteboard ],
        ch->descr->name );
      info( empty_string, noteboard_level( noteboard ),
        tmp1, MAX_IFLAG+noteboard, 2, ch,
        noteboard == NOTE_CLAN ? ch->pcdata->pfile->clan : NULL );
      for( i = 0; note_list[noteboard][i] != note; i++ )
        if( i == max_note[noteboard]-1 ) {
          bug( "Do_Note: Modified note not found!" );
          return;
	  } 
      remove( note_list[noteboard], max_note[noteboard], i );
      }
    insert( note_list[noteboard], max_note[noteboard], note,
      max_note[noteboard] );
    pc->note_edit = NULL;
    save_notes( noteboard, clan );
    return;
    }

  page( ch, "Title: %s\n\r\n\r", note->title );
 
  pc->note_edit->message = edit_string( ch, argument,
    pc->note_edit->message, MEM_NOTE );

  return;
}


void display_notes( player_data* ch )
{
  char           tmp  [ TWO_LINES ];
  note_data*    note;
  bool       reverse;
  int              i;
  int            max;
  int      noteboard;

  reverse   = is_set( ch->pcdata->pfile->flags, PLR_REVERSE );
  noteboard = ch->noteboard;
  max       = max_note[ noteboard ];

  *tmp = '\0';

  for( i = 0; i < max; i++ ) {
    note = note_list[ noteboard ][ reverse ? max-i-1 : i ];
    if( *tmp == '\0' ) { 
      page_title( ch, "%s Noteboard", noteboard_name[ noteboard ] );
      }
    sprintf( tmp, "[%2d] %-15s %-40s %s\n\r", i+1, note->from,
      note->title, ltime( note->date ) );
    page( ch, tmp );
    }

  if( *tmp == '\0' ) 
    send( ch, "There are no messages.\n\r" );

  return;
}


void note_summary( char_data* ch )
{
  clan_data*    clan;
  int           i, j;
  int            max;
  bool         found  = FALSE;

  if( ( clan = ch->pcdata->pfile->clan ) != NULL ) {
    note_list[ NOTE_CLAN ] = clan->note_list;
    max_note[ NOTE_CLAN ]  = clan->max_note;
    }

  for( i = 0; i < MAX_NOTEBOARD; i++ ) {
    if( !can_read( ch, i ) )
      continue; 
    max = max_note[i];
    for( j = 0; j < max; j++ )
      if( note_list[i][max-j-1]->date < ch->pcdata->pfile->last_on ) 
        break;
    if( j > 0 ) {
      if( !found ) { 
        send_underlined( ch, "New Notes\n\r" );
        found = TRUE;
        }
      send( ch, "%15s : %d\n\r", noteboard_name[i], j );
      }
    }

  if( !found )
    send( ch, "There have been no new notes since last login.\n\r" );

  return;
}


/*
 *   MAIL ROUTINES
 */


const char* message1 =
  "The mail daemon takes the letter, opens it, reads it carefully,\
 rolls on\n\rthe floor laughing, and wanders off to find %s.\n\r";

const char* message2 =
  "The mail daemon takes the letter and sprints off to find %s.\n\r";

const char* message3 =
  "You give a mail daemon your letter.  The mail daemon loiters nearby and\
 whistles a tune.  When he realizes you aren't going to pay postage, he\
 stamps around just out of reach and mumbles rudely about immortals,\
 before slowly wandering off in the wrong direction.";

const char* message4 =
  "You give a mail daemon your letter.  He stands nearby with his hand out\
 for a few minutes and then grumbles and wanders off while chewing on a\
 corner of the letter.";

const char* dream_msg =
  "You really don't want to invite a daemon into your dreams.\n\r";


void mail_message( char_data* ch )
{
  char           tmp  [ TWO_LINES ];
  note_data*    note;
  int          total  = 0;
  int         recent  = 0;

  for( note = ch->pcdata->pfile->mail; note != NULL; note = note->next ) {
    total++;
    if( note->date > ch->pcdata->pfile->last_on ) 
      recent++;
    }

  if( total != 0 ) {
    if( recent != 0 ) 
      sprintf( tmp, "You have %s mail message%s.",
        number_word( total ), total == 1 ? "" : "s" );
    else 
      sprintf( tmp, "You have %s mail message%s.",
        number_word( total ), total == 1 ? "" : "s" );
    send_centered( ch, tmp );
    }

  return;
}


note_data* find_mail( char_data* ch, int number )
{
  note_data*   note  = ch->pcdata->pfile->mail;
  int          line  = 0;

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    number = count( note )-number+1;

  for( ; note != NULL; note = note->next )
    if( ++line == number )
      return note;

  send( ch, "You have no mail by that number.\n\r" );

  return NULL;
}


void do_mail( char_data* ch, char* argument )
{
  char               tmp  [ 3*MAX_STRING_LENGTH ];
  note_data*        note;
  note_data*    note_new;
  int                  i;

  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( ch->species != NULL ) {
    send( ch, "You can only use the mail system in human form.\n\r" );
    return;
    }

  if( ( note = ch->pcdata->mail_edit ) == NULL ) {
    if( *argument == '\0' ) {
      display_mail( ch );
      return;
      }

    if( isdigit( *argument ) ) {
      if( ( note = find_mail( ch, atoi( argument ) ) ) == NULL )
        return;
      page( ch, "Title: %s\n\r", note->title );
      page( ch, "From:  %s\n\r\n\r", note->from );
      convert_to_ansi( ch, note->message, tmp );
      page( ch, tmp );
      return;
      }

    if( exact_match( argument, "delete" ) ) {
      if( ch->pcdata->pfile->mail == NULL ) {
        send( ch, "You have no mail to delete.\n\r" );
        return;
        }
      if( !strcasecmp( argument, "all" ) ) {
        delete_list( ch->pcdata->pfile->mail );
        send( ch, "All mail messages deleted.\n\r" );
        }
      else {
        if( ( note = find_mail( ch, atoi( argument ) ) ) == NULL )
          return;
        remove( ch->pcdata->pfile->mail, note );
        delete note;
        send( ch, "Mail message deleted.\n\r" );
        }
      save_mail( ch->pcdata->pfile );
      return;
      }

    if( exact_match( argument, "edit" ) ) {
      if( ( note = find_mail( ch, i = atoi( argument ) ) ) == NULL )
        return;
      note_new               = new note_data;
      note_new->title        = alloc_string( note->title, MEM_NOTE );
      note_new->message      = alloc_string( note->message, MEM_NOTE );
      note_new->from         = alloc_string( ch->real_name( ), MEM_NOTE );
      note_new->noteboard    = NOTE_PRIVATE;
      ch->pcdata->mail_edit  = note_new;
      send( ch, "Copied message %d to mail buffer.\n\r", i );
      return;
      }

    if( !acceptable_title( ch, argument ) )
      return;
 
    note                   = new note_data;
    note->title            = alloc_string( argument, MEM_NOTE );
    note->message          = alloc_string( "", MEM_NOTE );
    note->from             = alloc_string( ch->real_name( ), MEM_NOTE );
    note->noteboard        = NOTE_PRIVATE;
    ch->pcdata->mail_edit  = note;
    send( ch, "Starting new message with subject %s.\n\r", note->title );
    return;
    }

  if( exact_match( argument, "title" ) ) {
    if( !acceptable_title( ch, argument ) )
      return;
    free_string( note->title, MEM_NOTE );
    note->title = alloc_string( argument, MEM_NOTE );
    send( ch, "Mail title changed to '%s'.\n\r", argument );
    return;
    }    

  if( exact_match( argument, "send" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Whom do you want to send the message to?\n\r" );
      return;
      } 
    send_mail( ch, 'S', argument );
    return;
    }

  if( exact_match( argument, "cc" ) ) {
    send_mail( ch, 'C', argument );
    return;
    }

  if( !strcasecmp( argument, "delete" ) ) {
    send( ch, "The message you were editing has been deleted.\n\r" );
    delete note;
    ch->pcdata->mail_edit = NULL;
    return;
    }
  
  sprintf( tmp, "Subject: %s\n\r\n\r", note->title );
  page( ch, tmp );
 
  ch->pcdata->mail_edit->message = edit_string( ch, argument,
    ch->pcdata->mail_edit->message, MEM_NOTE );

  return;
}


void display_mail( char_data* ch )
{
  char          tmp  [ TWO_LINES ];
  note_data*   note;
  int          line  = 0;

  if( ch->pcdata->pfile->mail == NULL ) { 
    send( "You have no mail.\n\r", ch );
    return;
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    reverse( ch->pcdata->pfile->mail );

  for( note = ch->pcdata->pfile->mail; note != NULL; note = note->next ) {
    sprintf( tmp, "[%2d] %-34s %-15s %s\n\r", ++line, note->title,
      note->from, ltime( note->date ) );
    page( ch, tmp );
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_REVERSE ) )
    reverse( ch->pcdata->pfile->mail );

  return;
}


void send_mail( char_data* ch, char letter, char* argument )    
{
  char             tmp  [ TWO_LINES ];
  note_data*      note;
  note_data*  note_new;
  link_data*      link;
  pfile_data*    pfile;

  note = ch->pcdata->mail_edit;

  if( *argument == '\0' ) {
    send( ch, "%s the message to whom?\n\r",
      letter == 'C' ? "Cc" : "Send" );
    return;
    }

  if( ( pfile = find_pfile( argument ) ) == NULL ) {
    send( ch, "There is noone by that name.\n\r" );
    return;
    } 

  if( ch->position == POS_SLEEPING ) {
    send( ch, dream_msg );
    return;
    }

  if( ch->pcdata->pfile != pfile ) {
    sprintf( tmp, "You give a mail daemon your letter and" );
    if( !remove_coins( ch, 10, tmp ) ) {
      if( ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch,
          "You don't have the silver coin required to mail a letter.\n\r" );
        return;
        }
      fsend( ch, number_range( 0, 1 ) == 0 ? message3 : message4 );
      }
    else {
      send( ch, number_range( 0, 50 ) == 0 ? message1 : message2,
        pfile->name );
      }
    }
  else {
    send( ch, "You %s the letter to yourself.\n\r",
      letter == 'S' ? "send" : "cc a copy of" );
    }

  if( letter != 'S' ) {
    note_new              = new note_data;
    note_new->title       = alloc_string( note->title,    MEM_NOTE );
    note_new->message     = alloc_string( note->message,  MEM_NOTE );
    note_new->from        = alloc_string( note->from,     MEM_NOTE );
    note_new->noteboard   = NOTE_PRIVATE;
    note                  = note_new;
    }
  else {
    ch->pcdata->mail_edit = NULL;
    }  

  note->date = current_time;

  append( pfile->mail, note );
  save_mail( pfile );

  if( ch->pcdata->pfile != pfile ) 
    for( link = link_list; link != NULL; link = link->next ) {
      if( link->character != NULL && link->connected == CON_PLAYING
        && link->character->pcdata->pfile == pfile ) {
        send( link->character,
          "A mail daemon runs up and hands you a letter from %s.\n\r", ch );
        break;
        }
      }

  return;
}


/*
 *   DISK ROUTINES
 */


void read_mail( pfile_data *pfile )
{
  char          tmp  [ TWO_LINES ];
  note_data*   note;
  char*       title;
  FILE*          fp;

  sprintf( tmp, "%s%s", MAIL_DIR, pfile->name );

  if( ( fp = fopen( tmp, "r" ) ) != NULL ) { 
    if( strcmp( fread_word( fp ), "#MAIL" ) ) {
      bug( "Read_mail: missing header", 0 );
      exit( 1 );
      }

    for( ; ; ) {
      title = fread_string( fp, MEM_NOTE );
      if( title[0] == '$' ) {
        free_string( title, MEM_NOTE );
        break;
        }

      if( strlen( title ) > 40 )
        title[40] = '\0';

      note            = new note_data;
      note->title     = title;
      note->from      = fread_string( fp, MEM_NOTE );
      note->message   = fread_string( fp, MEM_NOTE ); 
      note->noteboard = NOTE_PRIVATE;
      note->date      = fread_number( fp ); 

      append( pfile->mail, note );
      }
    fclose( fp );
    }

  return;
}


void save_mail( pfile_data *pfile )
{
  FILE*         fp;
  note_data*  note;

  if( pfile->mail == NULL ) {
    delete_file( MAIL_DIR, pfile->name, FALSE );
    }     
  else if( ( fp = open_file( MAIL_DIR, pfile->name, "w" ) ) != NULL ) {
    fprintf( fp, "#MAIL\n\n" );

    for( note = pfile->mail; note != NULL; note = note->next ) {
      fprintf( fp, "%s~\n", note->title );
      fprintf( fp, "%s~\n", note->from );
      fprintf( fp, "%s~\n", note->message );
      fprintf( fp, "%d\n",  int( note->date ) );
      }

    fprintf( fp, "$~\n" );
    fclose( fp );
    }

  return;
}


/*
 *   NOTEBOARD DISK ROUTINES
 */


void load_notes( void )
{
  char          tmp  [ TWO_LINES ];
  int             i;

  echo( "Loading Notes ...\n\r" );

  for( i = 0; i < MAX_NOTEBOARD; i++ ) 
    if( i != NOTE_CLAN ) {
      sprintf( tmp, "%s%s", NOTE_DIR, noteboard_name[i] );
      load_noteboard( tmp, note_list[i], max_note[i], i );
      }

  return;
}


void load_notes( clan_data* clan )
{
  char tmp [ ONE_LINE ];

  sprintf( tmp, "%s%s", CLAN_NOTE_DIR, clan->abbrev );
  load_noteboard( tmp, clan->note_list, clan->max_note, NOTE_CLAN );

  return;
}


void load_noteboard( char* file, note_data**& list, int& max, int noteboard )
{
  note_data*   note;
  char*       title;
  FILE*          fp;

  list = NULL;
  max  = 0;

  fp = open_file( file, "r" );

  if( strcmp( fread_word( fp ), "#NOTES" ) )
    panic( "Load_notes: missing header" );

  for( ; ; ) {
    title = fread_string( fp, MEM_NOTE );

    if( *title == '$' ) {
      free_string( title, MEM_NOTE );
      break;
      }
 
    note            = new note_data;
    note->title     = title;
    note->from      = fread_string( fp, MEM_NOTE );
    note->message   = fread_string( fp, MEM_NOTE ); 
    note->date      = fread_number( fp ); 
    note->noteboard = noteboard;

    insert( list, max, note, max );
    }

  fclose( fp );
  return;
}


void save_notes( int i, clan_data* clan )
{
  char          tmp  [ TWO_LINES ];

  if( i == -1 ) {
    for( i = 0; i < MAX_NOTEBOARD; i++ )
      if( i != NOTE_CLAN ) 
        save_notes( i );
    return;
    }

  if( i == NOTE_CLAN ) {
    clan->note_list = note_list[ NOTE_CLAN ];
    clan->max_note  = max_note[ NOTE_CLAN ];
    save_notes( clan );
    }
  else {
    sprintf( tmp, "%s%s", NOTE_DIR, noteboard_name[i] );
    save_noteboard( tmp, note_list[i], max_note[i] );
    }
}


void save_notes( clan_data* clan )
{
  char tmp [ ONE_LINE ];

  sprintf( tmp, "%s%s", CLAN_NOTE_DIR, clan->abbrev );
  save_noteboard( tmp, clan->note_list, clan->max_note );

  return;
}


void save_noteboard( char* file, note_data** list, int max )
{ 
  FILE*          fp;
  note_data*   note;
  int             i;

  if( ( fp = open_file( file, "w" ) ) == NULL ) 
    return;

  fprintf( fp, "#NOTES\n\n" );

  for( i = 0; i < max; i++ ) {
    note = list[i];
    fprintf( fp, "%s~\n", note->title );
    fprintf( fp, "%s~\n", note->from );
    fprintf( fp, "%s~\n", note->message );
    fprintf( fp, "%d\n",  int( note->date ) );
    }

  fprintf( fp, "$~\n" );
  fclose( fp );

  return;
}





