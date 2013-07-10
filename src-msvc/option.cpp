#include "define.h"
#include "struct.h"


const char* message_flags [] = { "Autosave", "Bleeding", "Damage.Mobs",
  "Damage.Players", "Followers", "Hungry", "Max.Hit", "Max.Energy",
  "Max_move", "Misses", "Queue", "Spell.Counter", "Thirsty", "Weather", 
  "Item.Lists", "Long.Names", "Door.Dir" };

const char* mess_settings [] = { "??", "??", "Banks",
  "Prepare", "Equipment", "Inventory", "??" };

const char *iflag_name [] = { "auction", "clans", "deaths", "levels",
  "logins", "bugs", "requests", "writes", "notes" };

const char* plyr_settings [] = { "Autoloot", "Autoscan",
  "Ignore", "Incognito", "Room.Info" }; 


int msg_type  = MSG_STANDARD;


/*
 *   INFO ROUTINES
 */


class Info_Data
{
 public:
  info_data*     next;
  char*       string1;
  char*       string2;
  int           level;
  int            type;
  clan_data*     clan;
  pfile_data*   pfile;

  Info_Data( const char* s1, const char* s2, int l, int t,
    clan_data* c, pfile_data* p ) {
    record_new( sizeof( Info_Data ), MEM_INFO );
    string1 = alloc_string( s1, MEM_INFO );
    string2 = alloc_string( s2, MEM_INFO );
    level   = l;
    type    = t;
    clan    = c;
    pfile   = p;
    }

  ~Info_Data( ) {
    record_delete( sizeof( Info_Data ), MEM_INFO );
    free_string( string1, MEM_INFO );
    free_string( string2, MEM_INFO );
    }
};


info_data* info_history [ MAX_IFLAG+1 ];


bool can_see_info( char_data* ch, pfile_data* pfile, int type )
{
  if( type == IFLAG_LOGINS || type == IFLAG_DEATHS
    || type == IFLAG_LEVELS ) 
    return( pfile == NULL || !is_incognito( pfile, ch ) );

  return TRUE;
}


const char* info_msg( char_data* ch, info_data* info )
{
  if( ( info->clan != NULL && info->clan != ch->pcdata->pfile->clan )
    || !can_see_info( ch, info->pfile, info->type ) )
    return empty_string;

  return( info->level > get_trust( ch ) ? info->string1
    : info->string2 );
}


void do_info( char_data* ch, char* argument )
{
  info_data*     info;
  const char*  string;
  bool          found;
  bool          first  = TRUE;
  int          length  = strlen( argument );
  int            i, j;

  if( not_player( ch ) )
    return;

  for( i = 0; i < MAX_IFLAG+1; i++ ) {
    if( *argument == '\0' ||
      !strncasecmp( argument, iflag_name[i], length ) ) {
      found = FALSE;
      for( j = 0, info = info_history[i]; info != NULL; info = info->next )
        if( info_msg( ch, info ) != empty_string )
          j++;
      for( info = info_history[i]; info != NULL; info = info->next ) {
        if( ( string = info_msg( ch, info ) ) != empty_string  
          && ( *argument != '\0' || j-- < 5  ) ) {
          if( !found ) {
            if( !first )
              page( ch, "\r\n" );              
            page( ch, "%s:\r\n", iflag_name[i] );
            found = TRUE;
            first = FALSE;
            }
          page( ch, "  %s\r\n", string );
          }
        }
      if( *argument != '\0' ) {
        if( first ) 
          send( ch, "The %s info history is blank.\r\n", iflag_name[i] );
        return;
        }
      }
    }

  if( *argument == '\0' ) { 
    if( first )
      send( ch, "The info history is blank.\r\n" );
    return;
    }

  if( toggle( ch, argument, "Info channel",
    ch->pcdata->pfile->flags, PLR_INFO ) ) 
    return;

  send( ch, "Illegal syntax - see help info.\r\n" );
}


void do_iflag( char_data* ch, char* argument )
{
  if( not_player( ch ) )
    return;

  player_data* pc = (player_data*) ch;

  if( *argument == '\0' ) {
    display_levels( "Info", &iflag_name[0],
      &iflag_name[1], &pc->iflag[0], MAX_IFLAG, ch );
    page( ch, "\r\n" );
    display_levels( "Noteboard", &noteboard_name[0],
      &noteboard_name[1], &pc->iflag[1], MAX_NOTEBOARD, ch );
    return;
    }

  if( set_levels( &iflag_name[0], &iflag_name[1], &pc->iflag[0],
    MAX_IFLAG, ch, argument, FALSE ) ) 
    return; 

  if( set_levels( &noteboard_name[0], &noteboard_name[1], &pc->iflag[1],
    MAX_NOTEBOARD, ch, argument, FALSE ) ) 
    return; 

  send( ch, "Unknown option.\r\n" );
}


void info( const char* arg1, int level, const char* arg2, int flag,
  int priority, char_data* ch, clan_data* clan )
{
  char*        bug_name  [] = { "## Roach ##",
                                "## Beetle ##", "## Aphid ##" };
  
  char             tmp1  [ THREE_LINES ];
  char             tmp2  [ THREE_LINES ];
  info_data*       info;
  info_data**   history;  
  link_data*       link;
  player_data*   victim  = NULL;
  const char*    string;
  int           setting;

  info    = new info_data( arg1, arg2, level, flag,
    clan, ch == NULL ? NULL : ch->pcdata->pfile );
  history = &info_history[ min( flag, MAX_IFLAG ) ];

  append( *history, info );

  if( count( *history ) > 20 ) {
    info     = *history;
    *history = info->next;
    delete info;
    }

  sprintf( tmp1, "%s || %%s", flag == IFLAG_AUCTION ? "Auction"
    : ( flag == IFLAG_BUGS ? bug_name[priority-1] : "Info" ) );

  for( link = link_list; link != NULL; link = link->next ) {
    if( link->connected != CON_PLAYING
      || ( victim = link->player ) == NULL 
      || victim == ch
      || !is_set( victim->pcdata->pfile->flags, PLR_INFO ) 
      || ( clan != NULL && victim->pcdata->pfile->clan != clan ) 
      || ( ch != NULL && !can_see_info( victim, ch->pcdata->pfile, flag ) ) )
      continue;

    if( flag < MAX_IFLAG )  
      setting = level_setting( &victim->iflag[0], flag );
    else
      setting = level_setting( &victim->iflag[1], flag-MAX_IFLAG );

    if( ch != NULL && ( flag == IFLAG_LOGINS || flag == IFLAG_DEATHS
      || flag == IFLAG_LEVELS ) ) {
      if( setting == 0
        || ( setting == 1 && !victim->Befriended( ch ) )
        || ( setting == 2 && !victim->Recognizes( ch ) ) 
        || !can_see_who( victim, ch ) )
        continue;
      }
    else if( setting < priority )
      continue;

    string = ( get_trust( victim ) >= level ? arg2 : arg1 );

    for( ; ; ) {
      string = break_line( string, tmp2, 65 );
      if( *tmp2 == '\0' )
        break;
      send_color( link->character,
        flag == IFLAG_AUCTION ? COLOR_AUCTION : COLOR_INFO,
        tmp1, tmp2 );
      }
    }

  return;
}


/*
 *   MESSAGE ROUTINE
 */


void do_message( char_data* ch, char* argument )
{
  if( not_player( ch ) )
    return;

  if( *argument == '\0' ) {
    display_flags( "*Message", &message_flags[0],
      &message_flags[1], &ch->pcdata->message, MAX_MESSAGE, ch );
    page( ch, "\r\n" );
    display_levels( "Message", &mess_settings[0],
      &mess_settings[1], &ch->pcdata->mess_settings, MAX_MESS_SETTING, ch );
    page( ch, "\r\n" );
    page_centered( ch, "[ Also see the iflag and option commands ]" );
    return;
    }

  if( set_flags( &message_flags[0], &message_flags[1], &ch->pcdata->message,
    MAX_MESSAGE, NULL, ch, argument, FALSE, FALSE ) != NULL ) 
    return;

  if( !set_levels( &mess_settings[0], &mess_settings[1],
    &ch->pcdata->mess_settings, MAX_MESS_SETTING, ch, argument, FALSE ) )
    send( ch, "Unknown option.\r\n" );
}


/*
 *   OPTION ROUTINE
 */


void do_options( char_data* ch, char* argument )
{
  int i;

  if( not_player( ch ) )
    return;

  if( *argument == '\0' ) {
    display_flags( "*Option Flags", &plr_name[0], &plr_name[1],
      ch->pcdata->pfile->flags, MAX_PLR_OPTION, ch );
    page( ch, "\r\n" );
    display_levels( "Option", &plyr_settings[0],
      &plyr_settings[1], &ch->pcdata->pfile->settings, MAX_SETTING, ch );
    page( ch, "\r\n" );
    page_centered( ch, "[ Also see the iflag and message commands ]" );
    return;
    }

  for( i = 0; i < MAX_PLR_OPTION; i++ ) {
    if( matches( argument, plr_name[i] ) ) {
      switch_bit( ch->pcdata->pfile->flags, i );

      if( i == PLR_STATUS_BAR ) 
        setup_screen( ch );

      send( ch, "%s set to %s.\r\n", plr_name[i],
        true_false( ch->pcdata->pfile->flags, i ) );

      if( is_set( ch->pcdata->pfile->flags, i ) ) {
        switch( i ) {
          case PLR_TRACK :
          case PLR_SEARCHING :
            send(
              "[This option increases movement point costs.]\r\n", ch );
            break;

          case PLR_PARRY :
            send( "[This option stops you fighting back.]\r\n", ch );
            break;
          }
        }

      return;
      }
    }

  if( !set_levels( &plyr_settings[0], &plyr_settings[1],
    &ch->pcdata->pfile->settings, MAX_SETTING, ch, argument, FALSE ) )
    send( ch, "Unknown flag or setting.\r\n" );
}


void do_configure( char_data* ch, char* )
{
  do_help( ch, "configure" );
  return;  
}













