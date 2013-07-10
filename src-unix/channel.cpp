#include "sys/types.h"
#include <ctype.h>
#include "stdio.h"
#include "define.h"
#include "struct.h"


int max_language = MAX_LANGUAGE-LANG_PRIMAL;

flag_data lang_flags = { "Languages", &skill_table[LANG_PRIMAL].name,
  &skill_table[LANG_PRIMAL+1].name, &max_language };


/*
 *   LOCAL FUNCTIONS
 */


const char*  say_verb       ( char_data*, char_data* );
void         say_routine    ( char_data*, char*, int );
void         add_tell       ( tell_data*&, const char*, const char*,
                              int = -1 );
void         display        ( tell_data*&, char_data*, char*,
                              char_data* = NULL );
void         garble_string  ( char*, char*, int );
void         tell_message   ( char_data*, char*, char_data*, int = 0 );


path_func   hear_yell;

tell_data*  ooc_history  = NULL;
tell_data*  atalk_history = NULL;


/*
 *   TELL CLASS AND SUPPORT ROUTINES
 */


Tell_Data :: Tell_Data( const char* pers, const char* txt,
  int tongue )
{
  record_new( sizeof( tell_data ), MEM_TELL );

  name     = alloc_string( pers, MEM_TELL );
  message  = alloc_string( txt, MEM_TELL ); 
  language = tongue; 
  *name    = toupper( *name );
  next     = NULL;
}


Tell_Data :: ~Tell_Data( )
{
  record_delete( sizeof( tell_data ), MEM_TELL );

  free_string( name, MEM_TELL );
  free_string( message, MEM_TELL );
}


void display( tell_data*& list, char_data* ch, char* text,
  char_data* victim )
{
  char         tmp  [ MAX_STRING_LENGTH ];
  int            i;
  tell_data*  tell;

  if( list == NULL ) {
    if( &list == &ooc_history ) {
      send( ch, "There have been no OOC's since the mud was started.\n\r" );
      }
    else if( victim == NULL || victim == ch ) {
      send( ch, "You haven't heard any %s since logging in.\n\r", text );
      }
    else {
      send( ch, "%s hasn't heard any %s since logging in.\n\r",
        victim, text );
      }
    return;
    }

  sprintf( tmp, "--- " );
  for( i = 0; i < int( strlen( text ) ); i++ )
    tmp[i+4] = toupper( text[i] );
  sprintf( &tmp[ strlen( text )+4 ], " ---" );

  page_centered( ch, tmp );

  for( tell = list; tell != NULL; tell = tell->next ) {
    if( tell->language != -1 )
      sprintf( tmp, "[%s >> %s]\n\r%s%s", tell->name,
        skill_table[tell->language].name, tell->message,
        tell->next == NULL ? "" : "\n\r" );
    else
      sprintf( tmp, "[%s]\n\r%s%s", tell->name,
        tell->message, tell->next == NULL ? "" : "\n\r" );

    page( ch, tmp );
    }

  return;
}


void add_tell( tell_data*& list, const char* pers, const char* message,
  int language )
{
  tell_data*  tell;

  tell = new tell_data( pers, message, language );

  append( list, tell );

  if( count( list ) > 10 ) {
    tell = list;
    list = tell->next;
    delete tell;
    }

  return;
} 


/*
 *   LANGUAGE ROUTINES
 */


void do_language( char_data *ch, char *argument )
{
  int i;

  if( is_confused_pet( ch ) || ch->pcdata == NULL )
    return;

  if( ch->pcdata->speaking < LANG_PRIMAL
    || ch->pcdata->speaking >= MAX_LANGUAGE )
    ch->pcdata->speaking = LANG_PRIMAL; 

  if( *argument == '\0' ) {
    send( ch, "You are currently speaking %s.\n\rType 'lang <lang\
 name>' to switch to another language.\n\r",
      skill_table[ch->pcdata->speaking].name );
    return;
    }

  for( i = LANG_PRIMAL; i < MAX_LANGUAGE; i++ )
    if( fmatches( argument, skill_table[i].name ) ) {
      if( i == LANG_PRIMAL && ch->shdata->level < LEVEL_APPRENTICE ) {
        send( ch, "Only immortals can speak in Primal.\n\r" );
        return;
        }
      if( ch->shdata->skill[i] == UNLEARNT
        && !is_set( ch->affected_by, AFF_TONGUES ) ) {
        send( ch, "You don't know that language.\n\r" );
        return;
        }
      ch->pcdata->speaking = i;
      send( ch, "You will now speak in %s.\n\r", skill_table[i].name );
      return;
      }

  send( ch, "Unknown language.\n\r" );
  return;
}


int get_language( char_data* ch, int i )
{
  if( i < LANG_PRIMAL || i >= MAX_LANGUAGE )
    return 0;

  if( ch->pcdata == NULL || is_set( ch->affected_by, AFF_TONGUES ) )
    return 10;

  return ch->shdata->skill[i];
}


void garble_string( char* output, char* input, int skill )
{
  skill = (200-10*skill)*10*skill;

  for( ; *input != '\0'; input++, output++ ) {
    if( number_range( 0, 10000 ) > skill ) {
      if( ispunct( *input ) || isspace( *input ) ) 
        *output = *input;
      else if( ( *input > 90 ) && ( *input < 97 ) )
        *output = *input-10;
      else
        *output = number_range( 'a', 'z' );
      }
    else 
      *output = *input;
    }
      
  *output = '\0';
  return;
}


char* slang( char_data* ch, int language )
{
  if( !is_set( ch->pcdata->pfile->flags, PLR_LANG_ID ) )
    return empty_string;

  if( get_language( ch, language ) == 0 )
    return " (in an unknown tongue)";

  char* tmp = static_string( );

  sprintf( tmp, " (in %s)", skill_table[ language ].name );
 
  return tmp;
}


bool munchkin( char_data* ch, char* text )
{
  int length =  strlen( text );
  int  punct =  0;
   
  for( int i = 0; i < length; i++ )    
    if( text[i] == '?' || text[i] == '!' )
      punct++; 

  if( punct > 3 && length/punct < 20 ) { 
    send( ch, "Excessive punctuation is sure sign that you shouldn't be heard from, please\n\rsurpress your munchkin tendencies.\n\r" );
    return TRUE;
    }

  return FALSE;
}


/*
 *   CHANNEL SUPPORT ROUTINES
 */


inline bool hear_channels( player_data* pc )
{
  return( pc->position != POS_EXTRACTED && ( pc->link == NULL
    || pc->link->connected == CON_PLAYING ) );
}


bool subtract_gsp( player_data* ch, const char* text, int cost )
{
  if( get_trust( ch ) < LEVEL_ARCHITECT ) {
    if( ch->gossip_pts < cost ) {
      send( ch, "%s requires %s gossip point%s.\n\r",
        text, number_word( cost ), cost == 1 ? "" : "s" );
      return FALSE;
      }
    ch->gossip_pts -= cost;
    }

  return TRUE;
}


bool can_talk( char_data* ch, const char* string )
{
  if( !is_set( ch->affected_by, AFF_SILENCE ) ) 
    return TRUE;

  if( string != NULL ) 
    fsend( ch, "You attempt to %s, but oddly fail to make any noise.",
      string );

  return FALSE;
}


/*
 *   STANDARD CHANNELS ROUTINES
 */


void do_ooc( char_data* ch, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  player_data*    pc;
  player_data*    victim;
  int         max_length;
  int             length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    display( ooc_history, ch, "OOC CHANNEL" );
    return;
    }

  if( toggle( ch, argument, "OOC channel",
    ch->pcdata->pfile->flags, PLR_OOC ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_OOC ) ) {
    send( ch, "You have OOC off.\n\r" );
    return;
    }

  if( munchkin( pc, argument ) )
    return;

  if( !subtract_gsp( pc, "Using OOC", 1 ) )
    return;
  
  sprintf( buf, "[OOC] %s:", ch->real_name( ) );
  length = strlen( buf );
  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[ length < max_length ] );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim )
      && is_set( victim->pcdata->pfile->flags, PLR_OOC )
      && !victim->Filtering( ch ) ) 
      send_color( victim, COLOR_OOC, buf );
    }

  add_tell( ooc_history, ch->real_name( ), tmp ); 
}


void do_chant( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*      pc;
  int         language;
  int           length;
  int       max_length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( ch->pcdata->religion == REL_NONE ) {
    send( ch, "Only those with a religion can chant.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    display( pc->chant, ch, "Chant" );
    return;
    }

  if( toggle( ch, argument, "Chant",
    ch->pcdata->pfile->flags, PLR_CHANT ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_CHANT ) ) {
    send( ch, "You have chant turned off.\n\r" );
    return;
    }

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );
  format_tell( tmp, buf );
  max_length = 70-strlen( tmp );

  sprintf( buf, "You chant:%s%s", 7 < max_length ? "" : "\n\r", 
    &tmp[7 < max_length] );
  send( ch, buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) 
      && pc->pcdata->religion == ch->pcdata->religion ) {
      add_tell( pc->chant, ch->Seen_Name( pc ), tmp, language ); 
      if( is_set( pc->pcdata->pfile->flags, PLR_CHANT ) ) {
        sprintf( buf, "%s chants:", ch->Seen_Name( pc ) );
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send( pc, buf );
        }
      }
    }
}


void do_chat( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*  victim;
  player_data*      pc;
  int         language;
  int       max_length;
  int           length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    display( pc->chat, ch, "chats" );
    return;
    }
 
  if( toggle( ch, argument, "Chat channel", 
    ch->pcdata->pfile->flags, PLR_CHAT ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_CHAT ) ) {
    send( "You have chat turned off.\n\r", ch );
    return;
    }

  if( !subtract_gsp( pc, "Chatting", 1 ) )
    return;

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You chat%s:", slang( ch, language ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );
  send_color( ch, COLOR_CHAT, buf );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim ) && are_allied( ch, victim ) ) { 
      add_tell( victim->chat, who_name( victim, ch ), tmp, language ); 
      if( victim != ch && victim->link != NULL
        && is_set( victim->pcdata->pfile->flags, PLR_CHAT )
        && !victim->Filtering( ch ) ) {
        sprintf( buf, "%s chats%s:", who_name( victim, ch ),
          slang( victim, language ) );
        *buf = toupper( *buf ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send_color( victim, COLOR_CHAT, buf );
        }
      }
    }

  return;
}


void do_gossip( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*       pc;
  player_data*   victim;
  int          language;
  int        max_length;
  int            length;

  if( is_mob( ch ) ) 
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    display( pc->gossip, ch, "gossips" );
    return;
    }

  if( toggle( ch, argument, "Gossip channel", 
    ch->pcdata->pfile->flags, PLR_GOSSIP ) ) 
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_GOSSIP ) ) {
    send( "You have gossip turned off.\n\r", ch );
    return;
    }

  if( munchkin( pc, argument ) )
    return;

  if( !subtract_gsp( pc, "Using gossip", 10 ) )
    return;

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );
  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You gossip%s:", slang( ch, language ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );
  send_color( ch, COLOR_GOSSIP, buf );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim ) ) {
      add_tell( victim->gossip, who_name( victim, ch ), tmp, language ); 
      if( victim != ch && victim->link != NULL
        && is_set( victim->pcdata->pfile->flags, PLR_GOSSIP )
        && !victim->Filtering( ch ) ) {
        sprintf( buf, "%s gossips%s:", who_name( victim, ch ),
          slang( victim, language ) );
        *buf = toupper( *buf ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send_color( victim, COLOR_GOSSIP, buf );
        }
      }
    }
}


void do_atalk( char_data* ch, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  player_data*    pc;
  player_data*    victim;
  int         max_length;
  int             length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    display( atalk_history, ch, "AUCTION CHANNEL" );
    return;
    }

  if( toggle( ch, argument, "Auction channel",
    ch->pcdata->pfile->flags, PLR_ATALK ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_ATALK ) ) {
    send( ch, "You have ATALK off.\n\r" );
    return;
    }

  if( !subtract_gsp( pc, "Using ATALK", 10 ) )
    return;
  
  sprintf( buf, "[ATALK] %s:", ch->real_name( ) );
  length = strlen( buf );
  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[ length < max_length ] );

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( hear_channels( victim )
      && is_set( victim->pcdata->pfile->flags, PLR_ATALK )
      && !victim->Filtering( ch ) ) 
      send_color( victim, COLOR_AUCTION, buf );
    }

  add_tell( atalk_history, ch->real_name( ), tmp ); 
}


/*
 *   IMMORTAL CHANNELS
 */


void do_avatar( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*   pc;
  wizard_data*   imm  = NULL;
  int            length;
  int        max_length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->avatar, ch, "avatar channel" );
    return;
    }

  if( toggle( ch, argument, "Avatar channel", 
    ch->pcdata->pfile->flags, PLR_AVATAR ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_AVATAR ) ) {
    send( "You have the avatar channel turned off.\n\r", ch );
    return;
    }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "%s --", ch->real_name( ) );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_AVATAR_CHAN ) ) {
      add_tell( imm->avatar, ch->real_name( ), tmp ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_AVATAR ) 
        && !imm->Filtering( ch ) ) {
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send( imm, buf );
        }
      }
    }
        
  return;
}


void do_buildchan( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  player_data*   pc;
  wizard_data*   imm  = NULL;
  int        max_length;
  int            length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->build_chan, ch, "builder channel" );
    return;
    }

  if( toggle( ch, argument, "Creator channel", 
    ch->pcdata->pfile->flags, PLR_BUILDCHAN ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_BUILDCHAN ) ) {
    send( ch, "You have the creator channel turned off.\n\r" );
    return;
    }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "%s:", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_BUILD_CHAN ) ) {
      add_tell( imm->build_chan, ch->descr->name, tmp, -1 ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_BUILDCHAN ) )
        send( buf, imm );
      }
    }
        
  return;
}


void do_immtalk( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char                tmp  [ MAX_STRING_LENGTH ];
  player_data*     pc;
  wizard_data*     imm  = NULL;
  int          max_length;
  int              length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->imm_talk, ch, "imm_talk" );
    return;
    }

  if( toggle( ch, argument, "Immortal channel", 
    ch->pcdata->pfile->flags, PLR_IMMCHAN ) )
    return;
        
  if( !is_set( ch->pcdata->pfile->flags, PLR_IMMCHAN ) ) {
    send( ch, "You have the immortal channel turned off.\n\r" );
    return;
    }   

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "[%s]", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_IMM_CHAN ) ) {
      add_tell( imm->imm_talk, ch->descr->name, tmp ); 
      if( is_set( imm->pcdata->pfile->flags, PLR_IMMCHAN ) ) 
        send( buf, imm );
      }
    }
        
  return;
}


void do_god( char_data* ch, char* argument )
{
  char                buf  [ MAX_STRING_LENGTH ];
  char                tmp  [ MAX_STRING_LENGTH ];
  player_data*     pc;
  wizard_data*     imm  = NULL;
  int          max_length;
  int              length;

  if( *argument == '\0' ) {
    imm = (wizard_data*) ch;
    display( imm->god_talk, ch, "God Channel" );
    return;
    }

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "{%s}", ch->descr->name );
  *buf   = toupper( *buf ); 
  length = strlen( buf );

  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( hear_channels( pc ) && ( imm = wizard( pc ) ) != NULL
      && has_permission( imm, PERM_GOD_CHAN ) ) {
      add_tell( imm->god_talk, ch->descr->name, tmp ); 
      send( buf, imm );
      }
    }

  return;
}


/*
 *   YELL ROUTINE
 */
 

void do_yell( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  char_data*        rch;
  player_data*       pc;
  int          language;
  int        max_length;
  int            length;

  if( is_confused_pet( ch ) )
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    if( pc != NULL )
      display( pc->yell, ch, "yells" );
    return;
    }

  if( !can_talk( ch, "yell" ) )
    return;

  if( ch->pcdata != NULL ) {
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
    }
  else {
    language = LANG_PRIMAL;
    format_tell( tmp, argument );
    }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    sprintf( buf, "You yell%s:", slang( ch, language ) );
    length = strlen( buf );
    sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
      &tmp[length < max_length] );
    send( ch, buf );
    }

  if( pc != NULL )
    add_tell( pc->yell, ch->descr->name, tmp, language ); 

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch != ch ) 
      hear_yell( rch, ch, tmp );

  exec_range( ch, 8, hear_yell, tmp );

  return;
}


const char* yell_name( char_data* ch, char_data* victim,
  int dir, int distance )
{
  char* tmp;

  if( distance == 0 ) {
    if( ch->Seen( victim ) ) 
      return ch->Seen_Name( victim );
    else
      return "someone nearby";
    }
  
  if( distance > 5 ) 
    return "someone far away";

  tmp = static_string( );
  sprintf( tmp, "someone %s", dir_table[dir].where );

  return tmp;
}


void hear_yell( char_data* victim, char_data* ch,
  char* message, int dir, int distance )
{
  char                 buf  [ MAX_STRING_LENGTH ];
  char                 tmp  [ MAX_STRING_LENGTH ];
  int               length;
  int           max_length;
  player_data*          pc  = player( victim );
  const char*         name;
  int             language;

  if( pc == NULL || !can_hear( victim ) ) 
    return;

  name       = yell_name( ch, victim, dir, distance );
  max_length = 70-strlen( message );
  language   = ch->pcdata != NULL ? ch->pcdata->speaking : LANG_PRIMAL;

  garble_string( tmp, message, get_language( victim, language ) );
  add_tell( pc->yell, name, tmp, language ); 

  sprintf( buf, "%s yells%s:", name,
    slang( victim, language ) );
  *buf = toupper( *buf ); 
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );
  send( victim, buf );

  return;
}


/*
 *   SAY ROUTINES
 */


void do_say( char_data* ch, char* argument )
{
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  char              arg  [ MAX_STRING_LENGTH ];
  char_data*     victim;
  player_data*       pc  = player( ch );
  int          language;
  int            length;
  int        max_length;

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( pc != NULL ) 
      display( pc->say, ch, "says" );
    return;
    }

  if( !can_talk( ch, "speak" ) )
    return;

  if( ch->pcdata != NULL ) {
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
    }
  else {
    language = LANG_PRIMAL;
    format_tell( tmp, argument );
    }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
      sprintf( buf, "You %s%s:", say_verb( ch, ch ), 
        slang( ch, language ) );
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
        &tmp[length < max_length] );
      send( buf, ch );
      }
    else
      send( ch, "Ok.\n\r" );
    if( pc != NULL )
      add_tell( pc->say, ch->descr->name, tmp, language ); 
    }

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( victim = character( ch->array->list[i] ) ) == NULL 
      || ch == victim || ( pc = player( victim ) ) == NULL
      || !can_hear( victim ) )
      continue;
    victim->improve_skill( language );
    sprintf( buf, "%s %s%s:", ch->Name( victim ),
      say_verb( ch, victim ), slang( victim, language ) );
    *buf = toupper( *buf );
    length = strlen( buf );
    garble_string( arg, tmp, get_language( victim, language ) );
    sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
      &arg[length < max_length] );
    send_color( victim, COLOR_SAYS, buf );
    add_tell( pc->say, ch->Name( victim ), arg, language ); 
    }

  return;
}


const char* say_verb( char_data* ch, char_data* victim )
{
  if( ch->shdata->race == RACE_LIZARD ) {
    return( ch == victim ? "hiss" : "hisses" );
    }

  return( ch == victim ? "say" : "says" );
}


/*
 *   SHOUT ROUTINE
 */


void do_shout( char_data* ch, char* argument )
{
  char             buf  [ MAX_STRING_LENGTH ];
  char             tmp  [ MAX_STRING_LENGTH ];
  player_data*  pc;
  player_data*  victim;
  int           length;
  int       max_length;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( *argument == '\0' ) {
    display( pc->shout, ch, "shouts" );
    return;
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_SHOUT ) ) {
    send( ch, "You have been forbidden from shouting by the gods.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( "Shout what?\n\r", ch );
    return;
    }

  if( !subtract_gsp( pc, "Shouting", 500 ) )
    return;

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  sprintf( buf, "You shout:%s%s", 10 < max_length ? "" : "\n\r", 
    &tmp[10 < max_length] );
  send( buf, ch );

  add_tell( pc->shout, ch->descr->name, tmp, -1 ); 

  for( int i = 0; i < player_list; i++ ) {
    victim = player_list[i];
    if( victim != ch && hear_channels( victim ) ) {
      add_tell( victim->shout, who_name( victim, ch ),tmp, -1 ); 
      sprintf( buf, "%s shouts:", who_name( victim, ch ) );
      *buf = toupper( *buf ); 
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
        &tmp[length < max_length] );
      send( victim, buf );
      }
    }

  return;
}


/*
 *   TELL ROUTINES
 */


void tell_message( char_data* ch, char* msg, char_data* victim, int i )
{
  char tmp  [ THREE_LINES ];

  sprintf( tmp,   "++ " );  
  sprintf( tmp+3, msg, victim->descr->name, i );
  strcat( tmp, " ++\n\r\n\r" );

  tmp[3] = toupper( tmp[3] );

  send( ch, tmp );
}


void process_tell( char_data* ch, char_data* victim, char* argument )
{
  char             buf  [ 3*MAX_STRING_LENGTH ];
  char             tmp  [ 3*MAX_STRING_LENGTH ];
  player_data*      pc;
  int           length;
  int       max_length;

  if( ch->pcdata != NULL && victim->pcdata != NULL ) {
    if( victim->Filtering( ch ) ) {
      fsend( ch, "%s is filtering you - please leave %s in peace.",
        victim->descr->name, victim->Him_Her( ) );
      return; 
      }
    if( ch->Filtering( victim ) ) {
      fsend( ch, "You are filtering %s and only a chebucto would want to\
 converse with someone they are filtering.", victim );
      return;
      } 
    }

  if( ( pc = player( victim ) ) == NULL
    && victim->link != NULL )
    pc = victim->link->player;

  if( victim->pcdata != NULL ) {
    if( victim->link == NULL && pc->switched == NULL ) 
      tell_message( ch, "%s is link dead", victim );
    else if( victim->timer+30 < current_time )
      tell_message( ch, "%s has been idle for %d seconds",
        victim, current_time-victim->timer );
    else if( victim->array != ch->array && opponent( victim ) != NULL )
      tell_message( ch, "%s is in battle", victim );
    }

  format_tell( tmp, argument );
  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
      sprintf( buf, "You tell %s:", victim->Seen_Name( ch ) );
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
        &tmp[length < max_length] );
      send( ch, buf );
      }
    else
      send( ch, "Ok.\n\r" );
    }

  if( pc == NULL )
    return;

  sprintf( buf, "%s tells %s:", ch->Seen_Name( victim ),
    victim != pc || pc->switched == NULL
    ? "you" : pc->descr->name );
  *buf = toupper( *buf );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );

  if( ch->pcdata == NULL ) {
    convert_to_ansi( victim, buf, tmp );
    send( victim, tmp );
    return;
    } 

  if( !is_set( &victim->status, STAT_REPLY_LOCK ) ) 
    victim->reply = ch;

  if( victim == pc && pc->switched != NULL )
    victim = pc->switched;

  send_color( victim, COLOR_TELLS, buf );
  add_tell( pc->tell, ch->Seen_Name( victim ), tmp );
}


void do_tell( char_data *ch, char *argument )
{
  char             arg  [ MAX_STRING_LENGTH ];
  char_data*    victim;
  player_data*  pc  = player( ch );

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( pc != NULL )
      display( pc->tell, ch, "tells" );
    return;
    }

  argument = one_argument( argument, arg );

  if( *arg == '\0' || *argument == '\0' ) {
    send( ch, "Tell whom what?\n\r" );
    return;
    }

  in_character = FALSE;

  if( ( victim = one_character( ch, arg, "tell",
    (thing_array*) &player_list ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "Talking to yourself is pointless.\n\r" );
    return;
    }

  if( ch->pcdata != NULL && victim->pcdata != NULL ) {
    if( victim->Ignoring( ch ) ) {
      fsend( ch, "%s has ignore set to level %d and you cannot tell to %s.",
        victim->descr->name, 
        level_setting( &victim->pcdata->pfile->settings, SET_IGNORE ),
        victim->Him_Her( ) );
      return;
      }
    }

  process_tell( ch, victim, argument );
}


void do_reply( char_data* ch, char* argument )
{
  char*   noone_msg  = "Noone has told to you or the person who last told\
 to you has quit.\n\r";
  char_data* victim  = ch->reply;

  in_character = FALSE;
 
  if( *argument == '\0' ) {
    if( ch->reply == NULL ) {
      send( ch, "Reply points to noone.\n\r" );
      }
    else {
      send( ch, "Reply %s to %s.\n\r",
        is_set( &ch->status, STAT_REPLY_LOCK ) ? "is locked" : "points",
        victim->Seen_Name( ch ) );
      }
    return;
    }

  if( !strcasecmp( argument, "lock" ) ) {
    if( victim == NULL ) { 
      send( ch, noone_msg );
      }
    else if( is_set( &ch->status, STAT_REPLY_LOCK ) ) {
      send( ch, "Your reply is already locked to %s.\n\r",
        victim->Seen_Name( ch ) );
      }
    else {
      set_bit( &ch->status, STAT_REPLY_LOCK );
      send( ch, "Reply locked to %s.\n\r", victim->Seen_Name( ch ) );    
      }
    return;
    }

  if( !strcasecmp( argument, "unlock" ) ) {
    if( !is_set( &ch->status, STAT_REPLY_LOCK ) ) {
      send( ch, "Your reply is already unlocked.\n\r" );
      }
    else {
      send( ch, "Reply unlocked.\n\r" );
      remove_bit( &ch->status, STAT_REPLY_LOCK );
      }
    return;
    }

  if( victim == NULL ) {
    send( ch, noone_msg );
    return;
    }

  process_tell( ch, victim, argument );
}


/*
 *   WHISPER/TO
 */


void trigger_say( char_data* ch, char_data* mob, char* argument )
{
  mprog_data* mprog;

  for( mprog = mob->species->mprog; mprog != NULL; mprog = mprog->next ) 
    if( mprog->trigger == MPROG_TRIGGER_TELL
      && ( is_name( argument, mprog->string )
      || *mprog->string == '\0' ) ) {
      var_ch   = ch;
      var_mob  = mob;
      var_arg  = argument;
      var_room = Room( ch->array->where );
      execute( mprog );
      return;
      }
}


void ic_tell( char_data* ch, char* argument, char* verb )
{ 
  char              buf  [ MAX_STRING_LENGTH ];
  char              tmp  [ MAX_STRING_LENGTH ];
  char              arg  [ MAX_STRING_LENGTH ];
  char_data*     victim;
  player_data*       pc;
  int          language;
  int            length;
  int        max_length;

  argument = one_argument( argument, arg );

  if( ( victim = one_character( ch, arg, verb,
    ch->array ) ) == NULL )
    return;
 
  if( *argument == '\0' ) {
    send( ch, "%s what to %s?\n\r", verb, victim );
    return;
    }

  if( ch == victim ) {
    send( ch, "%sing something to yourself does nothing useful.\n\r",
      verb );
    return;
    }

  if( !IS_AWAKE( victim ) ) {
    send( ch, "They are not in a state to hear you.\n\r" );
    return;
    }

  /*
  fsend_seen( ch, "%s whispers something to %s.\n\r", ch, victim );
  */

  if( ch->pcdata != NULL ) {
    language = ch->pcdata->speaking;
    garble_string( buf, argument, get_language( ch, language ) );
    format_tell( tmp, buf );
    }
  else {
    language = LANG_PRIMAL;
    format_tell( tmp, argument );
    }

  max_length = 70-strlen( tmp );

  if( ch->pcdata != NULL ) {
    if( is_set( ch->pcdata->pfile->flags, PLR_SAY_REPEAT ) ) {
      sprintf( buf, "You %s to %s%s:", verb, victim->Name( ch ), 
        slang( ch, language ) );
      length = strlen( buf );
      sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
        &tmp[length < max_length] );
      send( ch, buf );
      }
    else
      send( ch, "Ok.\n\r" );
    }

  if( ( pc = player( victim ) ) == NULL ) {
    trigger_say( ch, victim, argument );
    return;
    }

  sprintf( buf, "%s %ss to you%s:", ch->Name( victim ),
    verb, slang( victim, language ) );
  buf[0] = toupper( buf[0] );
  length = strlen( buf );
  garble_string( arg, tmp, get_language( victim, language ) );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &arg[length < max_length] );
  send_color( victim, COLOR_SAYS, buf );
  add_tell( pc->whisper, ch->Seen_Name( pc ), arg ); 
}


void do_to( char_data* ch, char* argument )
{
  player_data* pc;

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( ( pc = player( ch ) ) != NULL )
      display( pc->to, ch, "To" );
    return;
    }

  ic_tell( ch, argument, "say" );
}


void do_whisper( char_data* ch, char* argument )
{  
  player_data*  pc  = player( ch );

  if( is_confused_pet( ch ) )
    return;

  if( *argument == '\0' ) {
    if( pc != NULL )  
      display( pc->whisper, ch, "whispers" );
    return;
    }

  ic_tell( ch, argument, "whisper" );
}


/*
 *   EMOTE ROUTINE
 */


void do_emote( char_data *ch, char *argument )
{
  char_data*    rch;
  bool        space;

  if( ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_NO_EMOTE ) ) {
    send( ch, "You can't show your emotions.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Emote what?\n\r" );
    return;
    }

  space = strncmp( argument, "'s ", 3 ) && strncmp( argument, ", ", 2 );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL 
      || rch->position <= POS_SLEEPING )
      continue; 
    fsend( rch, "%s%s%s\n\r",
      ch->Name( rch ), space ? " " : "", argument );
    }

  return;
}


/*
 *   GROUP TELL
 */


void do_gtell( char_data* ch, char* argument )
{
  char              tmp  [ MAX_STRING_LENGTH ];
  char              buf  [ MAX_STRING_LENGTH ];
  char_data*     leader;
  player_data*       pc  = player( ch );
  int          language;
  int            length;
  int        max_length;

  if( is_mob( ch ) )
    return;

  if( *argument == '\0' ) {
    display( pc->gtell, ch, "group tells" );
    return;
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_NO_TELL ) ) {
    send( ch, "You are banned from using tell.\n\r" );
    return;
    }

  if( ( leader = group_leader( ch ) ) == NULL ) {
    send( ch, "You aren't in a group.\n\r" );
    return;
    }

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You tell your group%s:", 
    slang( ch, language ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );
  send( buf, ch );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->position != POS_EXTRACTED && ( pc->link == NULL
      || pc->link->connected == CON_PLAYING )
      && leader == group_leader( pc ) ) {
      add_tell( pc->gtell, ch->Seen_Name( pc ), tmp, language );
      if( pc != ch && pc->link != NULL ) {
        sprintf( buf, "%s tells the group%s:", ch->Seen_Name( pc ),
          slang( pc, language ) );
        buf[0] = toupper( buf[0] ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send_color( pc, COLOR_GTELL, buf );
        }
      }
    }

  return;
}


/*
 *   CLAN TELL
 */


void do_ctell( char_data* ch, char* argument )
{
  char               buf  [ MAX_STRING_LENGTH ];
  char               tmp  [ MAX_STRING_LENGTH ];
  int           language;
  int         max_length;
  int             length;
  player_data*    pc  = player( ch );

  if( is_mob( ch ) )
    return;

  if( ch->pcdata->pfile->clan == NULL ) {
    send( ch, "You aren't in a clan.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    display( pc->ctell, ch, "clan tells" );
    return;
    }

  if( toggle( ch, argument, "Clan channel", 
    ch->pcdata->pfile->flags, PLR_CTELL ) )
    return;

  if( !is_set( ch->pcdata->pfile->flags, PLR_CTELL ) ) {
    send( ch, "You have ctell turned off.\n\r" );
    return;
    }

  language = ch->pcdata->speaking;
  garble_string( buf, argument, get_language( ch, language ) );

  format_tell( tmp, buf );

  max_length = 70-strlen( tmp );

  sprintf( buf, "You tell the clan%s:", slang( ch, language ) );
  length = strlen( buf );
  sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
    &tmp[length < max_length] );
  send_color( ch, COLOR_CTELL, buf );

  for( int i = 0; i < player_list; i++ ) {
    pc = player_list[i];
    if( pc->In_Game( )
      && pc->pcdata->pfile->clan == ch->pcdata->pfile->clan ) {
      add_tell( pc->ctell, ch->Seen_Name( pc ), tmp ); 
      if( pc != ch && pc->link != NULL
        && is_set( pc->pcdata->pfile->flags, PLR_CTELL ) ) {
        sprintf( buf, "%s ctells%s:", ch->Seen_Name( pc ),
          slang( pc, language ) );
        *buf = toupper( *buf ); 
        length = strlen( buf );
        sprintf( &buf[length], "%s%s", length < max_length ? "" : "\n\r", 
          &tmp[length < max_length] );
        send_color( pc, COLOR_CTELL, buf );
        }
      }
    }

  return;
}


/*
 *   REVIEW COMMAND
 */


void do_review( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  player_data*   victim;

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( ch, "For whom do you wish to review recent conversation?\n\r" );
    return;
    }

  if( ( victim = (player_data*) one_character( ch, arg, "review", 
    (thing_array*) &player_list ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch,
      "There are simplier ways to review your own conversations.\n\r" );
    return;
    }

  if( get_trust( victim ) >= get_trust( ch ) ) {
    send( ch, "You are unable to review %s's conversations.\n\r", victim );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Which conversation channel do you want to review?\n\r" );
    return;
    }

  if( matches( argument, "say" ) ) {
    display( victim->say, ch, "says", victim );
    return;
    }

  if( matches( argument, "tell" ) ) {
    display( victim->tell, ch, "tells", victim );
    return;
    }

  send( ch, "Unknown history type - see help review.\n\r" );

  return;
}
