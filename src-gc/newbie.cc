#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


void  use_start_table    ( char_data*, int );


/*
 *   CHARACTER CREATION ROUTINES
 */


void wrong_choice( link_data* link )
{
  send( link, "\n\rWhat you entered was unintelligible.  Please press\
 return and then select\n\rone of the choices given between the left\
 and right brackets.\n\r" );
  press_return( link );

  return;
}


/*
 *   NAME
 */


bool check_parse_name( link_data* link, char* name )
{
  species_data*  species;
  char*           letter;
  int             length  = strlen( name );
  int           capitals  = 0;

  /* ALPHA AND PUNCTUATION */

  if( ispunct( *name ) || ispunct( name[length] ) ) {
    send( link, "\n\rThe first and last letters of a name may not be\
 punctuation to stop the\n\rsegment of the population who insist on the\
 name --Starbright--.\n\r" );
    return FALSE;
    }

  for( letter = name; *letter != '\0'; letter++ ) {
    if( isdigit( *letter ) ) {
      send( link, "\n\rNames are not allowed to contain numbers.\n\r" );
      return FALSE;
      }
    if( ispunct( *letter ) && *letter != '-' && *letter != '\'' ) {
      send( link, "\n\rNames are not allowed to contain punctuation other\
 than dashes and\n\rapostrophes.\n\r" );
      return FALSE;
      }
    if( isupper( *letter ) && ++capitals > 2 ) {
      send( link, "\n\rNames can contain at most 3 capital letters.\n\r" );
      return FALSE;
      } 
    }

  if( length < 3 || length > 12 ) {
    send( link,
      "Names must be greater than 2 and less than 13 letters.\n\r" );
    return FALSE;
    }

  /* WORDS USED IN GAME */

  for( int i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL )
      continue;
    if( is_name( name, species->descr->name ) ) {
      send( link, "\n\rThat is the name of monster in the game and thus\
 unallowed.\n\r" );
      return FALSE;
      }
    }

  if( find_pfile_exact( name ) != NULL ) {
    send( link, "\n\rA player already exists with that name.\n\r" );
    return FALSE;
    }

  if( find_pfile( name ) != NULL || find_pfile_substring( name ) != NULL ) {
    help_link( link, "Name_Similiar" );
    return FALSE;
    }

  if( pntr_search( badname_array, max_badname, name ) >= 0 ) {
    help_link( link, "Bad_Name" );
    return FALSE;
    }

  return TRUE;
}


void nanny_new_name( link_data* link, char* argument )
{
  player_data*     ch;
  pfile_data*   pfile;

  if( no_input( link, argument ) )
    return;

  *argument = toupper( *argument );

  if( !check_parse_name( link, argument ) ) {
    send( link, "\n\rSelect another name: " );
    return;
    }

  pfile = new pfile_data( argument );
  ch    = new player_data( pfile->name );

  ch->pcdata->pfile = pfile;
  ch->link          = link;
  link->player      = ch;
  link->character   = ch;
  link->connected   = CON_GET_NEW_PASSWORD;
  link->pfile       = pfile;

  modify_pfile( ch );

  help_link( link, "Login_password" );
  send( link, "Password: " );
}


void nanny_new_password( link_data* link, char* argument )
{
  char_data*  ch  = link->character;
  char*        p;

  if( strlen( argument ) < 5 ) {
    send( link,
      "\n\rPassword must be at least five characters long.\n\rPassword: " );
    return;
    }

  for( p = argument; *p != '\0'; p++ ) {
    if( *p == '~' ) {
      send( link,
        "\n\rNew password not acceptable, try again.\n\rPassword: " );
      return;
      }
    }

  free_string( ch->pcdata->pfile->pwd, MEM_PLAYER );
  ch->pcdata->pfile->pwd = alloc_string( argument, MEM_PLAYER );

  send( link, "Please retype password: " );
  link->connected = CON_CONFIRM_PASSWORD;
}


void nanny_confirm_password( link_data* link, char* argument )
{
  char_data*  ch  = link->character;

  send( link, "\n\r" );

  if( strcmp( argument, ch->pcdata->pfile->pwd ) ) {
    send( link, "Passwords don't match.\n\rRetype password: " );
    link->connected = CON_GET_NEW_PASSWORD;
    return;
    }

  help_link( link, "Login.Display" );
  send( link, "Display type? [ None Ansi VT52 ]: " );

  link->connected = CON_SET_TERM;

  return;
}


/*
 *   TERMINAL TYPE
 */


void nanny_set_term( link_data* link, char* argument )
{
  char_data* ch  = link->character;
  int         i;

  switch( toupper( *argument ) ) {
    case 'V' :  ch->pcdata->terminal = TERM_VT100;  break;
    case 'A' :  ch->pcdata->terminal = TERM_ANSI;   break;
    default :   ch->pcdata->terminal = TERM_DUMB;   break;
    }

  if( ch->pcdata->terminal != TERM_DUMB ) 
    for( i = 0; i < MAX_COLOR; i++ )
      ch->pcdata->color[i] = term_table[ ch->pcdata->terminal ].defaults[i];

  setup_screen( ch );

  help_link( link, "Gamedesc_1" );
  press_return( link );

  link->connected = CON_READ_GAME_RULES;
}


/*
 *   GAME RULES
 */


void nanny_show_rules( link_data* link, char* )
{
  char_data*  ch  = link->character;

  clear_screen( ch );

  do_help( ch, "GAMEDESC_2" );
  send( link, "\r\nDo you wish to continue (Y/N)? " );

  link->connected = CON_AGREE_GAME_RULES;

  return;
}


void nanny_agree_rules( link_data* link, char* argument )
{
  if( toupper( *argument ) == 'N' ) {
    send( link, "Good bye.\r\n" );
    close_socket( link );
    return;
    }

  if( toupper( *argument ) != 'Y' ) {
    send( link, "Please answer Y or N. " );
    return;
    }

  nanny_help_sex( link, "" );

  return;
}    


/*
 *   SEX
 */


void nanny_help_sex( link_data* link, char* )
{
  char_data*  ch  = link->character;

  clear_screen( ch );

  do_help( ch, "login_sex" );
  send( link, "\n\rPlease enter your sex [Male Female]: " );

  link->connected = CON_GET_NEW_SEX;

  return;
}


void nanny_sex( link_data* link, char* argument )
{
  char_data*  ch  = link->character;

  switch( toupper( *argument ) ) {
    case 'M': ch->sex = SEX_MALE;    break;
    case 'F': ch->sex = SEX_FEMALE;  break;
    default:
      wrong_choice( link );
      link->connected = CON_HELP_SEX;
      return;
    }

  nanny_help_class( link, argument );
  return;
}


void nanny_help_class( link_data* link, char* )
{
  char       tmp  [ TWO_LINES ];
  int          i;
  char_data*  ch  = link->character;

  clear_screen( ch );

  do_help( ch, "login_classes" );

  strcpy( tmp, "\n\rSelect a class [" );

  for( i = 0; i < MAX_CLSS; i++ ) {
    if( clss_table[i].open ) {
      if( i > 0 )
        strcat( tmp, " " );
      strcat( tmp, clss_table[i].abbrev );
      }
    }
  strcat( tmp, "]: " );

  send( link, tmp );
  link->connected = CON_GET_NEW_CLSS;

  return;
}


void nanny_class( link_data* link, char* argument )
{
  char_data*    ch  = link->character;
  int            i;
  bool        help  = FALSE;

  link->connected = CON_HELP_CLSS;

  if( *argument == '\0' ) {
    wrong_choice( link );
    return;
    }

  if( *argument == '?' ) {
    argument++;
    help = TRUE;
    }

  for( i = 0; i < MAX_CLSS; i++ ) 
    if( clss_table[i].open 
      && fmatches( argument, clss_table[i].name ) )
      break;

  if( help ) {
    clear_screen( ch );
    help_link( link, clss_table[i].name );
    press_return( link );
    return;
    }    

  if( i == MAX_CLSS ) {
    wrong_choice( link );
    return;
    }

  ch->pcdata->clss = i;
  nanny_help_race( link, argument );

  return;
}


/*
 *   RACES
 */


bool allowed_race( char_data* ch, int race )
{
  int i;

  if( !plyr_race_table[race].open ) 
    return FALSE;

  for( i = 0; i < MAX_ENTRY_ALIGNMENT; i++ ) 
    if( is_set( &clss_table[ ch->pcdata->clss ].alignments, i ) 
      && is_set( &plyr_race_table[ race ].alignments, i ) ) 
      return TRUE;

  return FALSE;
}


void nanny_help_race( link_data* link, char* )
{
  char         tmp  [ TWO_LINES ];
  char_data*    ch  = link->character;
  bool       first  = TRUE;
  int            i;

  clear_screen( ch );

  do_help( ch, "login_races" );
  strcpy( tmp, "\n\rSelect a race [" );

  for( i = 0; i < MAX_PLYR_RACE; i++ ) {
    if( allowed_race( ch, i ) ) {
      if( !first ) 
        strcat( tmp, " " );
      strcat( tmp, race_table[i].abbrev );
      first = FALSE;
      }
    }      

  strcat( tmp, "]: " );
  send( link, tmp );

  link->connected = CON_GET_NEW_RACE; 
 
  return;
}

   
void nanny_race( link_data* link, char* argument )
{
  char_data*  ch  = link->character;
  int          i;
  bool      help  = FALSE;

  link->connected = CON_HELP_RACE;

  if( *argument == '?' ) {
    argument++;
    help = TRUE;
    }

  if( *argument == '\0' ) {
    wrong_choice( link );
    return;
    }

  for( i = 0; i < MAX_PLYR_RACE; i++ ) 
    if( plyr_race_table[i].open 
      && ( fmatches( argument, race_table[i].name )  
      || fmatches( argument, race_table[i].abbrev ) ) ) 
      break;
 
  if( i == MAX_PLYR_RACE ) {
    wrong_choice( link );
    return;
    }   

  if( help ) {
    clear_screen( ch );
    help_link( link, plyr_race_table[i].plural );
    press_return( link );
    return;
    }    

  ch->shdata->race = i;
  ch->pcdata->speaking = LANG_HUMANIC+i;
  ch->shdata->skill[ LANG_HUMANIC+i ] = 10;
  ch->shdata->skill[ LANG_PRIMAL ] = 10;

  link->connected = CON_DECIDE_STATS; 
  nanny_stats( link, "" );
}


/*
 *   STATS
 */


void nanny_stats( link_data* link, char* argument )
{
  char_data*       ch  = link->character;
  share_data*  shdata  = ch->shdata;
  int*          bonus  = plyr_race_table[ch->shdata->race].stat_bonus;
  int          points;
  int          number;
  int             i,j;
  char              c;

  int *stat[] = { &shdata->strength, &shdata->intelligence,
    &shdata->wisdom, &shdata->dexterity, &shdata->constitution };
  char letter[] = { 'S', 'I', 'W', 'D', 'C' };
  char *name[] = { "Strength", "Intelligence", "Wisdom", "Dexterity",
    "Constitution" };

  if( !strcasecmp( "done", argument ) ) { 
    nanny_help_alignment( link, "" );
    return; 
    }

  clear_screen( ch );

  if( strlen( argument ) < 2 || ( strlen( argument ) < 3 
    && isdigit( argument[1] ) ) ) {
    do_help( ch, "login_stats" );
    send( ch, "\n\r" );
    }
  else {
    for( j = 0, points = 500; j < 5; j++ )
      points -= (*stat[j]-bonus[j])*(*stat[j]-bonus[j]+1)/2;

    if( isdigit( argument[1] ) ) {
      c = toupper( argument[2] );
      number = argument[1]-'0';
      }
    else {
      c = toupper( argument[1] );
      number = 1;
      }

    for( i = 0; i < 5; i++ ) 
      if( c == letter[ i ] ) 
        break;

    if( i != 5 ) {
      switch( argument[0] ) {
      case '+' :
        if( *stat[i]+number > 18+bonus[ i ] )
          send( link, 
    "\n\rYou can not raise a stat over its racial maximum.\n\r" );
        else
          *stat[i] += number;
        break;
      case '-' :
        if( *stat[i]-number < 3 )
           send( link, 
            "\n\rYou can not lower a stat below 3.\n\r" );
        else
          *stat[i] -= number;
        break;
      default:
        i = 5;
        break;
        }
      }
   
    if( i == 5 ) {
      do_help( ch, "login_stats" );
      send( ch, "\n\r" );
      }
    else {
      for( j = 0, points = 500; j < 5; j++ )
        points -= (*stat[j]-bonus[j])*(*stat[j]-bonus[j]+1)/2;

      if( points < 0 ) {
        send( link,
          "\n\rYou don't have enough points for that.\n\r" );
        *stat[i] -= number;
        }
      } 
    }
  
  for( j = 0, points = 500; j < 5; j++ )
    points -= (*stat[j]-bonus[j])*(*stat[j]-bonus[j]+1)/2;

  send( link, "Points Left: %d\n\r\n\r", points );
  send( link,
    "Ability             Character    Racial Max    Cost to inc.\n\r" );
  send( link,
    "-------             ---------    ----------    ------------\n\r" );

  for( i = 0; i < 5; i++ ) {
    send( link, "%-15s%10d%13d%15d\n\r",
      name[i], *stat[i], 18+bonus[i],*stat[i]-bonus[i]+1 );
    }

  send( link, "\n\rEnter choice: " );
}


/*
 *   ALIGNMENT
 */


bool allowed_alignment( char_data* ch, int align )
{
  return( is_set( &clss_table[ ch->pcdata->clss ].alignments, align ) 
    && is_set( &plyr_race_table[ ch->shdata->race ].alignments, align ) );  
}


void nanny_help_alignment( link_data* link, char* )
{
  char         tmp  [ TWO_LINES ];
  char_data*    ch  = link->character;
  bool       first  = TRUE;
  int            i;

  clear_screen( ch );

  do_help( ch, "login_alignment" );
  strcpy( tmp, "\n\rEnter an alignment [" );

  for( i = 0; i < MAX_ENTRY_ALIGNMENT; i++ ) {
    if( allowed_alignment( ch, i ) ) {
      if( !first ) 
        strcat( tmp, " " );
      strcat( tmp, alignment_table[i].abbrev );
      first = FALSE;
      }
    }

  strcat( tmp, "]: " );
  send( link, tmp );

  link->connected = CON_GET_NEW_ALIGNMENT;
} 


void nanny_alignment( link_data* link, char* argument )
{
  char_data*      ch  = link->player;
  int              i;
  const char*   word  = empty_string; 
  bool          help  = FALSE;

  link->connected = CON_HELP_ALIGNMENT;

  if( *argument == '?' ) {
    argument++;
    help = TRUE;
    }

  if( *argument == '\0' ) {
    wrong_choice( link );
    return;
    }
  
  for( i = 0; i < MAX_ENTRY_ALIGNMENT; i++ )
    if( !strcasecmp( argument, alignment_table[i].abbrev )
      || fmatches( argument, alignment_table[i].name, 3 ) )
      break;

  if( i == MAX_ENTRY_ALIGNMENT ) {
    wrong_choice( link );
    return;
    }   

  if( help ) {
    clear_screen( ch );
    help_link( link, alignment_table[i].name );
    press_return( link );
    return;
    }    

  if( !is_set( &clss_table[ ch->pcdata->clss ].alignments, i ) ) {
    word = clss_table[ ch->pcdata->clss ].name;
    }
  else if( !is_set( &plyr_race_table[ ch->shdata->race ].alignments, i ) ) {
    word = race_table[ ch->shdata->race ].name;
    }
  else {
    ch->shdata->alignment = i;
    clear_screen( ch );
    help_link( link, "Introduction" );
    press_return( link );
    link->connected = CON_READ_IMOTD;
    return;
    }

  send( link, "\n\rA%c %s would not choose to be %s.\n\r",
    isvowel( *word ), word, alignment_table[i].name );

  press_return( link );
}


/*
 *   INITIALIZE NEW PLAYER
 */


void new_player( player_data* ch )
{
  char               tmp  [ MAX_STRING_LENGTH ];
  int               clss  = ch->pcdata->clss;
  int                  i;

  ch->shdata->level     = 1;
  ch->exp               = 0;
  ch->pcdata->practice  = 25;

  ch->base_hit          = 20;
  ch->base_mana         = 50;
  ch->base_move         = 100;
  ch->base_age          = plyr_race_table[ ch->shdata->race ].start_age;

  update_max_hit( ch );
  update_max_mana( ch );

  ch->hit  = ch->max_hit;
  ch->mana = ch->max_mana;
  
  update_max_move( ch );

  ch->move        = ch->max_move;

  /* OPTIONS */

  set_bit( ch->pcdata->pfile->flags, PLR_PROMPT );
  set_bit( ch->pcdata->pfile->flags, PLR_GOSSIP );
  set_bit( ch->pcdata->pfile->flags, PLR_SAY_REPEAT );
  set_bit( ch->pcdata->pfile->flags, PLR_AUTO_EXIT );
  set_bit( ch->pcdata->pfile->flags, PLR_SAFE_KILL );
  set_bit( ch->pcdata->pfile->flags, PLR_PORTAL );
  set_bit( ch->pcdata->pfile->flags, PLR_IDLE );
  set_bit( ch->pcdata->pfile->flags, PLR_INFO );
  set_bit( ch->pcdata->pfile->flags, PLR_LANG_ID );
  set_bit( ch->pcdata->pfile->flags, PLR_OOC );
  
  set_level( &ch->pcdata->pfile->settings, SET_ROOM_INFO, 3 );
  set_level( &ch->pcdata->pfile->settings, SET_AUTOLOOT, 1 );

  /* IFLAGS */

  for( i = 0; i < MAX_IFLAG; i++ )
    set_level( ch->iflag, i, 3 );

  set_level( ch->iflag, IFLAG_AUCTION, 1 );
  set_level( ch->iflag, IFLAG_LOGINS, 1 );
  set_level( ch->iflag, MAX_IFLAG+NOTE_ANNOUNCEMENTS, 1 );
  set_level( ch->iflag, MAX_IFLAG+NOTE_GENERAL, 1 );

  /* MESSAGES */

  remove_bit( &ch->pcdata->message, MSG_MAX_MOVE );

  modify_pfile( ch ); 

  ch->pcdata->pfile->account = ch->link->account;

  free_string( ch->descr->singular, MEM_DESCR );
  sprintf( tmp, "%s %s",
    race_table[ ch->shdata->race ].name,
    clss_table[ clss ].name );
  ch->descr->singular = alloc_string( tmp, MEM_DESCR );

  free_string( ch->descr->long_s, MEM_DESCR );
  sprintf( tmp, "%s is standing here.", ch->descr->singular );
  tmp[0] = toupper( tmp[0] );
  ch->descr->long_s = alloc_string( tmp, MEM_DESCR );

  free_string( ch->pcdata->title, MEM_PLAYER );
  sprintf( tmp, " the %s %s", race_table[ ch->shdata->race ].name,
    clss_table[ clss ].name );
  ch->pcdata->title = alloc_string( tmp, MEM_PLAYER );

  free_string( ch->descr->keywords, MEM_DESCR );
  sprintf( tmp, "%s %s", race_table[ch->shdata->race].name,
    clss_table[clss].name );
  ch->descr->keywords = alloc_string( tmp, MEM_DESCR );

  /* SET STANDARD EQUIP/SKILLS */

  use_start_table( ch, 0 );
  use_start_table( ch, clss+1 );
  use_start_table( ch, MAX_CLSS+ch->shdata->race+1 );

  ch->To( get_temple( ch ) );
}


void use_start_table( char_data* ch, int entry )
{
  obj_clss_data*  index;
  obj_data*         obj;
  int             skill;

  for( int i = 0; i < 5; i++ ) 
    if( ( skill = starting_table[entry].skill[i] ) >= 0 )
      ch->shdata->skill[skill] = starting_table[entry].level[i];

  for( int i = 0; i < 5; i++ ) 
    if( ( index = get_obj_index(
      starting_table[entry].object[2*i] ) ) != NULL ) {
      obj = create( index, starting_table[ entry ].object[ 2*i+1 ] );
      obj->To( ch );
      switch( obj->pIndexData->item_type ) {
        case ITEM_ARMOR :
        case ITEM_WEAPON : 
          needremove( obj, ch, NULL );
        }
      }
}


bool newbie_abuse( char_data* ch )
{
  if( ch->species != NULL || ch->played+current_time-ch->logon > 50 )
    return FALSE;

  send( ch, "To stop abuse you cannot drop or give anything for the\
 first 5 minutes\n\rof play.  To get rid of an item use junk.\n\r" );

  return TRUE;
}



