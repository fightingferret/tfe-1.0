#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


const char* color_key = "neEKrRgGyYbBmMcCW";

const char* color_fields [ MAX_COLOR ] = { "default", "room.name", "tell",
  "say", "gossip", "players", "mobs", "objects", "unused", "unused",
  "titles", "ctell", "chat", "ooc", "gtell", "auction", "info",
  "to.self", "to.group", "by.self", "by.group",  
  "mild.emphasis", "strong.emphasis", "black", "red1", "red2", "green1",
  "green2", "yellow1", "yellow2", "blue1", "blue2",
  "magenta1", "magenta2", "cyan1", "cyan2", "white1", "white2" };

const char* format_vt100 [] = {
  "none", "bold", "reverse", "underline" };

const int defaults_vt100 [ MAX_COLOR ] = {
  VT100_NORMAL,     VT100_BOLD,   VT100_REVERSE,  VT100_NORMAL,
  VT100_BOLD,       VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_BOLD,     VT100_NORMAL,
  VT100_NORMAL,     VT100_BOLD,   VT100_NORMAL,   VT100_UNDERLINE,
  VT100_NORMAL,     VT100_BOLD,   VT100_NORMAL,   VT100_UNDERLINE,
  VT100_UNDERLINE,  VT100_BOLD,   VT100_REVERSE,  VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL, VT100_NORMAL,   VT100_NORMAL,
  VT100_NORMAL,     VT100_NORMAL };

const int defaults_ansi [ MAX_COLOR ] = {
  ANSI_NORMAL,      ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BOLD,
  ANSI_BOLD_YELLOW, ANSI_NORMAL,       ANSI_NORMAL,     ANSI_NORMAL,
  ANSI_NORMAL,      ANSI_NORMAL,       ANSI_BOLD,       ANSI_BOLD_CYAN,
  ANSI_NORMAL,      ANSI_NORMAL,       ANSI_BOLD_GREEN, ANSI_WHITE,
  ANSI_NORMAL,      ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BLUE,  
  ANSI_YELLOW,      ANSI_BOLD,         ANSI_REVERSE,    ANSI_REVERSE, 
  ANSI_RED,         ANSI_BOLD_RED,     ANSI_GREEN,      ANSI_BOLD_GREEN,
  ANSI_YELLOW,      ANSI_BOLD_YELLOW,  ANSI_BLUE,       ANSI_BOLD_BLUE,
  ANSI_MAGENTA,     ANSI_BOLD_MAGENTA, ANSI_CYAN,       ANSI_BOLD_CYAN,
  ANSI_WHITE,       ANSI_BOLD_WHITE  };


const char* format_ansi [] = {
  "bold", "reverse", "blinking", "underlined",
  "black.text", "red.text", "green.text", "yellow.text",
  "blue.text", "magenta.text", "cyan.text", "white.text",
  "black.bg", "red.bg", "green.bg", "yellow.bg", "blue.bg",
  "magenta.bg", "cyan.bg", "white.bg" };

int index_ansi [] = { 1, 7, 5, 4, 30, 31, 32, 33, 34, 35, 36, 37,
  40, 41, 42, 43, 44, 45, 46, 47 };

term_func ansi_codes;
term_func vt100_codes;


const term_type term_table [] =
{
  { "dumb",   0,  NULL,          NULL,         NULL            },
  { "vt100",  4,  format_vt100,  vt100_codes,  defaults_vt100  },
  { "ansi",  20,  format_ansi,   ansi_codes,   defaults_ansi   }
};


/*
 *   ANSI
 */


const char* ansi_colors( int code )
{
  char*   tmp  = static_string( );
  int     i,j;

  if( code == 0 ) 
    return "none";

  *tmp = '\0';

  for( i = 0; code != 0 && i < 5; i++ ) {
    for( j = 0; code%64 != index_ansi[j]; j++ )
      if( j == term_table[ TERM_ANSI ].entries-1 ) 
        return "Impossible Index??";
    sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : " ",
      format_ansi[j] );
    code = code >> 6;
    }
 
  return tmp;
}


const char* ansi_codes( int code )
{
  static char   tmp  [ 150 ];
  static int   pntr;  
  int         array  [ 5 ];

  pntr = (pntr+1)%10;

  for( int i = 0; i < 5; i++ ) {
    array[i] = code%64;
    code = code >> 6;
    }

  sprintf( tmp+15*pntr, "[%d;%d;%d;%dm",
    array[3],array[2],array[1],array[0] );
 
  return tmp+15*pntr;
}


/*
 *   VT100
 */


const char* vt100_codes( int i )
{
  const char* escape_codes [] = { "[0m","[1m", "[7m", "[4m" };

  return escape_codes[i];
}


/*
 *   MAIN COLOR ROUTINE
 */


void do_color( char_data* ch, char* argument )
{
  char      tmp1  [ MAX_INPUT_LENGTH ];
  char      tmp2  [ MAX_INPUT_LENGTH ];
  char      tmp3  [ MAX_INPUT_LENGTH ];
  int       i, j;
  int*     color;
  int       term;
  int       code;

  if( ch->link == NULL ) 
    return;

  color = ch->pcdata->color;
  term  = ch->pcdata->terminal;

  if( *argument == '\0' ) {
    page_title( ch, "Terminal Types" );
    for( i = 0; i < MAX_TERM; i++ ) {
      page( ch, "%18s (%1c)%s", term_table[i].name,
        term == i ? '*' : ' ', i%3 == 2 ? "\n\r" : "" );
      }
    if( i%3 != 0 )
      page( ch, "\n\r" );
    page( ch, "\n\r" );

    if( term == TERM_DUMB )
       return;

    page( ch, "%35sLines: %d\n\r\n\r", "", ch->pcdata->lines );

    page_title( ch, "Color Options" );

    for( i = 0; i < term_table[term].entries; i++ ) {
      sprintf( tmp1, "%s", term_table[term].format[i] );
      sprintf( tmp2, "%%%ds%%s",
        18+strlen( tmp1 )-strlen( term_table[term].format[i] ) );
      sprintf( tmp3, tmp2, tmp1, i%4 == 3 ? "\n\r" : "" );
      page( ch, tmp3 );
      }
    if( i%4 != 0 )
      page( ch, "\n\r" );      
    page( ch, "\n\r" );

    page_title( ch, "Color Settings" );
    for( i = 0; i < MAX_COLOR; i++ ) {
      page( ch, "%16s : %s%s%s\n\r", color_fields[i],
        color_code( ch, i ),
        term == TERM_VT100 ? term_table[ TERM_VT100 ].format[
        ch->pcdata->color[i] ] : ansi_colors( ch->pcdata->color[i] ),
        normal( ch ) );
      }

    return;
    }

  if( matches( argument, "lines" ) ) {
    if( ( i = atoi( argument ) ) < 10 || i > 500 ) {
      send( ch, "Number of screen lines must be from 10 to 500.\n\r" );
      return;
      }
    ch->pcdata->lines = i;
    setup_screen( ch );
    send( ch, "Number of lines set to %d.\n\r", i );
    return;
    }

  for( i = 0; i < MAX_TERM; i++ )
    if( matches( argument, term_table[i].name ) ) {
      if( i != TERM_DUMB ) {
        for( j = 0; j < MAX_COLOR; j++ )
          color[j] = term_table[i].defaults[j];
        }
      reset_screen( ch );
      ch->pcdata->terminal = i;
      setup_screen( ch );
      send( ch, "Terminal type set to %s.\n\r", term_table[i].name );
      return;
      } 

  for( i = 0; i < MAX_COLOR; i++ )
    if( matches( argument, color_fields[i] ) )
      break;
 
  if( i == MAX_COLOR ) {
    send( ch, "Unknown item to color terminal type.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "To what color do you wish to set %s.\n\r",
      color_fields[i] );
    return;
    }

  if( !strcasecmp( argument, "none" ) ) {
    ch->pcdata->color[i] = 0;
    send( ch, "Color for %s removed.\n\r", color_fields[i] );
    return;
    }

  if( term == TERM_ANSI ) {
    for( code = 0; *argument != '\0'; ) {
      for( j = 0; !matches( argument, term_table[term].format[j] ); j++ ) {
        if( j == term_table[term].entries-1 ) {
          send( ch, "Unknown ansi format.\n\r" );
          return;
	  } 
        }
      code = ( code << 6 )+index_ansi[j];
      }
    ch->pcdata->color[i] = code;
    send( ch, "Color for %s set to %s.\n\r",
      color_fields[i], term == TERM_VT100
      ? term_table[ TERM_VT100 ].format[code] : ansi_colors( code ) );
    return;
    }
      
  for( j = 0; j < term_table[term].entries; j++ )
    if( matches( argument, term_table[term].format[j] ) )
      break;

  if( j == term_table[term].entries ) {
    send( ch, "Unknown format.\n\r" );
    return;
    }

  ch->pcdata->color[i] = j;

  send( ch, "Format of %s set to %s.\n\r",
    color_fields[i], term_table[term].format[j] );

  return;
}
  

/*
 *   CONVERT TEXT TO COLOR CODES
 */


void convert_to_ansi( char_data* ch, const char* input, char* output )
{
  int          term;
  int             i;

  if( ch->pcdata == NULL ) {
    *output = '\0';
    return;
    }

  term = ch->pcdata->terminal;

  for( ; *input != '\0'; input++ ) {
    if( *input != '@' ) {
      *output++ = *input;
      continue;
      }

    input++;
  
    switch( *input ) {
      case '@' : 
        *output++ = '@';
        continue;

      case 'I' :
        *output++ = ' ';
        *output++ = ' ';
        continue;
      }

    if( term == TERM_DUMB )
      continue;
 
    for( i = 0; color_key[i] != '\0'; i++ )
      if( color_key[i] == *input ) {
        strcpy( output, term_table[term].codes(
          ch->pcdata->color[ i == 0 ? COLOR_DEFAULT : COLOR_MILD+i-1 ] ) );
        output += strlen( output );
        }
    }

  if( term == TERM_DUMB ) {
    *output = '\0';
    return;
    }

  strcpy( output, term_table[term].codes(
    ch->pcdata->color[ COLOR_DEFAULT ] ) );

  return;
}     


/*
 *  WINDOW OPERATIONS
 */


void scroll_window( char_data* ch )
{
  int lines = ch->pcdata->lines;

  lock_keyboard( ch );
  save_cursor( ch );
  cursor_off( ch );
  scroll_region( ch, 1, lines-2 );
  move_cursor( ch, lines-2, 1 );
  
  return;
}


void command_line( char_data* ch )
{
  int lines = ch->pcdata->lines;

  scroll_region( ch, lines, lines );
  restore_cursor( ch );
  cursor_on( ch );
  unlock_keyboard( ch );

  return;
}


void setup_screen( char_data* ch )
{
  int lines = ch->pcdata->lines;

  reset_screen( ch );
  clear_screen( ch );

  if( ch->pcdata->terminal == TERM_DUMB 
    || !is_set( ch->pcdata->pfile->flags, PLR_STATUS_BAR ) ) 
    return;
 
  move_cursor( ch, lines, 1 );
  scroll_window( ch );

  return;
}


void clear_screen( char_data* ch )
{
  if( ch->pcdata->terminal != TERM_DUMB )
    send( ch, "[2J[1;1H" );
  else
    send( ch, "\n\r" );

  return;
}


void reset_screen( char_data* ch )
{
  if( ch->pcdata->terminal != TERM_DUMB ) {
    send( ch, "c" );
    send( ch, "[2J[1;1H" );
    }

  return;
}


/*
 *   SEND_COLOR
 */


void send_color( char_data* ch, int type, const char* msg )
{
  int   term  = ch->pcdata->terminal;
  int  color  = ch->pcdata->color[type];

  if( term != TERM_DUMB ) 
    send( ch, "%s%s%s",
      term_table[term].codes( color ), msg,
      term_table[term].codes( ch->pcdata->color[ COLOR_DEFAULT ] ) );
  else 
    send( ch, msg );

  return;
}


void page_color( char_data* ch, int type, const char* msg )
{
  int   term  = ch->pcdata->terminal;
  int  color  = ch->pcdata->color[type];

  if( term != TERM_DUMB ) 
    page( ch, "%s%s%s",
      term_table[term].codes( color ), msg,
      term_table[term].codes( ch->pcdata->color[ COLOR_DEFAULT ] ) );
  else 
    page( ch, msg );

  return;
}


/*
 *   VT100 COLOR ROUTINES
 */


const char* bold_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_WHITE );
    }

  return empty_string;
}


const char* bold_red_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_RED );
    }

  return empty_string;
}


const char* bold_cyan_v( char_data* ch )
{
  if( ch->pcdata == NULL )
    return empty_string;

  switch( ch->pcdata->terminal ) {
    case TERM_VT100:  return vt100_codes( VT100_BOLD );
    case TERM_ANSI:   return  ansi_codes( ANSI_BOLD*64+ANSI_CYAN );
    }

  return empty_string;
}


/*
 *   ANSI COLOR ROUTINES
 */


const char* normal( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_DEFAULT ] );
}


const char* red( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_RED ] );
}


const char* green( char_data *ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_GREEN ] );
}


const char* blue( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_BLUE ] );
}


const char* yellow( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_YELLOW ] );
}


/*
 *   COLOR SCALE 
 */


const char* color_scale( char_data* ch, int i )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal != TERM_ANSI )
    return empty_string;

  i = range( 0, i, 6 );

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_WHITE-2*i ] );
}


/*
 *   BATTLE COLOR FUNCTIONS
 */


const char* damage_color( char_data* rch, char_data* ch, char_data* victim )
{
  int term;

  if( rch->pcdata == NULL || ( term = rch->pcdata->terminal ) == TERM_DUMB )
    return empty_string;

  if( rch == victim )
    return term_table[term].codes( rch->pcdata->color[ COLOR_TO_SELF ] );

  if( is_same_group( rch, victim ) )
    return term_table[term].codes( rch->pcdata->color[ COLOR_TO_GROUP ] );

  if( rch == ch )
    return term_table[term].codes( rch->pcdata->color[ COLOR_BY_SELF ] );

  if( is_same_group( rch, ch ) )
    return term_table[term].codes( rch->pcdata->color[ COLOR_BY_GROUP ] );
 
  return empty_string;
}


const char* by_self( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_BY_SELF ] );
}


const char* to_self( char_data* ch )
{
  if( ch->pcdata == NULL || ch->pcdata->terminal == TERM_DUMB )
    return empty_string;

  return term_table[ ch->pcdata->terminal ].codes(
    ch->pcdata->color[ COLOR_TO_SELF ] );
}















