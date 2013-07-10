#include "define.h"
#include "struct.h"


/*
 *   GLOBAL CONSTANTS
 */


int max_permission  = MAX_PERMISSION;

flag_data affect_flags = { "Affect", &aff_char_table[0].name,
  &aff_char_table[1].name, &MAX_ENTRY_AFF_CHAR };

flag_data permission_flags = { "Permission", &permission_name[1],
  &permission_name[2], &max_permission };

flag_data material_flags = { "Materials", (const char **) &material_table[0].name,
  (const char **) &material_table[1].name, &MAX_ENTRY_METAL };


const char* flag_handler( const char** title, const char*** name1,
  const char*** name2, int** bit, int* max, int* uses_flag,
  const char* can_edit, char_data* ch, char* argument, int types )
{
  const char*  string;
  int            i, j;
  bool          exact;

  if( *argument == '\0' ) {
    for( i = j = 0; i < types; i++ )
      if( uses_flag[i] ) {
        if( j++ != 0 )
          page( ch, "\r\n" );
        display_flags( title[i], name1[i], name2[i], bit[i], max[i], ch );
        }
    return empty_string;
    }

  for( i = 0; i < types; i++ ) 
    if( uses_flag[i] && matches( argument, title[i] ) ) {
      display_flags( title[i], name1[i], name2[i], bit[i], max[i], ch );
      return empty_string;
      }

  for( exact = TRUE; ; exact = FALSE ) {
    for( i = 0; i < types; i++ )  
      if( uses_flag[i] && ( string = set_flags( name1[i], name2[i],
        bit[i], max[i], uses_flag[i] == 1 ? can_edit : NULL,
        ch, argument, exact, !exact && i == types-1 ) ) != NULL )
        return string;
    if( !exact )
      break;
    }

  return NULL;
}


/*
 *   DISPLAY ROUTINES
 */


void display_flags( const char* text, const char** name1,
  const char** name2, int* bit, int max, char_data* ch )
{
  char   buf  [ TWO_LINES ];
  int      i;

  if( *text == '*' ) 
    page_title( ch, "%s ", &text[1] );
  else 
    page( ch, "%s Flags:\r\n", text );

  for( i = 0; i < max; i++ ) {
    sprintf( buf, "%15s (%1c)%s", *(name1+i*(name2-name1)),
      is_set( bit, i ) ? '*' : ' ', i%4 == 3 ? "\r\n" : "" );
    page( ch, buf );
    }

  if( i%4 != 0 )
    page( ch, "\r\n" );     
}


void flag_data :: sprint( char* tmp, int* bit )
{
  int i;

  tmp[0] = '\0';

  if( *max < 16 && *bit+1 == ( 1 << *max ) ) {
    strcpy( tmp, "all" );
    return;
    }

  for( i = 0; i < *max; i++ )
    if( is_set( bit, i ) )
      sprintf( tmp+strlen( tmp ), "%s%s", *tmp == '\0' ? "" : ", ",
        *(name1+i*(name2-name1)) );

  if( *tmp == '\0' ) 
    strcpy( tmp, "none" );
}
  

void flag_data :: display( char_data* ch, int* bit )
{
  char   buf  [ TWO_LINES ];
  int      i;

  page( ch, "%s Flags:\r\n", title );

  for( i = 0; i < *max; i++ ) {
    sprintf( buf, "%15s (%1c)%s", *(name1+i*(name2-name1)),
      is_set( bit, i ) ? '*' : ' ',
      i%4 == 3 ? "\r\n" : "" );
    page( ch, buf );
    }

  if( i%4 != 0 )
    page( ch, "\r\n" );     
}


/*
 *   SET ROUTINE
 */


void flag_data :: set( char_data* ch, char* argument, int* bit )
{
  if( *argument == '\0' )
    display( ch, bit );
  else 
    set_flags( name1, name2, bit, *max, NULL, ch,
      argument, FALSE, TRUE );
}


const char* set_flags( const char** name1, const char** name2, int* bit,
  int max, const char* can_edit, char_data* ch, char* argument,
  bool exact, bool fail_msg )
{
  char* tmp = static_string( );

  for( int i = 0; i < max; i++ ) {
    if( matches( argument, *(name1+i*(name2-name1)), exact ) ) {
      if( can_edit != NULL ) {
        send( ch, can_edit );
        return can_edit;
        } 
      switch_bit( bit, i );
      sprintf( tmp, "%s set to %s.\r\n", *(name1+i*(name2-name1)),
        true_false( bit, i ) );
      *tmp = toupper( *tmp );
      send( ch, tmp );
      tmp[ strlen( tmp )-2 ] = '\0';
      return tmp;
      }
    }

  if( fail_msg )
    send( ch, "Unknown flag - use no argument for list.\r\n" );

  return NULL;
}


void set_flags( char_data* ch, char*& argument, int* bit, const char* flags )
{
  bool set;
  int    i;

  if( *argument != '+' && *argument != '-' )
    return;

  set = ( *argument++ == '+' ); 
  
  for( ; *argument != '\0' && *argument != ' '; argument++ ) { 
    for( i = 0; ; i++ ) {
      if( flags[i] == '\0' ) {
        send( ch, "Unknown flag: %c.\r\n", *argument );
        break; 
        }
      else if( flags[i] == *argument ) {
        send( ch, "%c bit %s.\r\n", *argument,
          set ? "set" : "removed" );
        assign_bit( bit, i, set );
        break;
        }
      }
    }
}


/* 
 *   TOGGLE FUNCTION
 */


void set_bool( char_data* ch, char* argument, const char* text,
  int& flag )
{
  if( !strncasecmp( "true", argument, strlen( argument ) ) ) {
    flag = TRUE;
    }
  else if( !strncasecmp( "false", argument, strlen( argument ) ) ) {
    flag = FALSE;
    }
  else {
    send( ch, "%s can be set either true or false.\r\n", text );
    return;
    }

  send( ch, "%s set %s.\r\n", text, flag ? "true" : "false" );
}


bool toggle( char_data* ch, char* arg, const char* text,
  int* bit, int flag )
{
  bool already;

  if( !strcasecmp( arg, "on" ) ) {
    if( ( already = is_set( bit, flag ) ) == FALSE ) 
      set_bit( bit, flag );
    }
  else if( !strcasecmp( arg, "off" )  ) {
    if( ( already = !is_set( bit, flag ) ) == FALSE )
      remove_bit( bit, flag );
    }
  else 
    return FALSE;

  if( already )
    send( ch, "%s already %s.\r\n", text, arg );
  else
    send( ch, "%s turned %s.\r\n", text, arg );

  return TRUE;
}


/*
 *   GET_FLAGS ROUTINE
 */


bool get_flags( char_data* ch, char*& argument, int* bit, const char* flags,
  const char* function )
{
  int     i;

  *bit = 0;
 
  if( *argument != '-' )
    return TRUE;

  argument++;

  for( ; *argument != ' ' && *argument != '\0'; argument ++ ) {
    for( i = 0; ; i++ ) {
      if( flags[i] == '\0' ) {
        send( ch, "%s: illegal option %c.\r\n", function, *argument ); 
        return FALSE;
        }
      if( *argument == flags[i] ) {
        set_bit( bit, i );
        break;
        }
      }
    }

  skip_spaces( argument );

  return TRUE;
}


/* 
 *   ALTER ROUTINE
 */


void alter_flags( int* modify, int* next, int* prev, int max )
{
  int i;

  for( i = 0; i < max; i++ )
    if( is_set( prev, i ) != is_set( next, i ) 
      && is_set( modify, i ) != is_set( next, i ) )
      switch_bit( modify, i );
}


/*
 *  LEVEL ROUTINES
 */


void display_levels( const char* text, const char** name1,
  const char** name2, int* bit, int max, char_data* ch )
{
  char   buf  [ TWO_LINES ];
  int      i;
  int  level;

  page_title( ch, "%s Levels", text );

  for( i = 0; i < max; i++ ) {
    level = level_setting( bit, i );
    sprintf( buf, "%15s (%c)%s", *(name1+i*(name2-name1)),
      level == 0 ? '-' : '0'+level, i%4 == 3 ? "\r\n" : "" );
    page( ch, buf );
    }

  if( i%4 != 0 )
    page( ch, "\r\n" );     
}


bool set_levels( const char** name1, const char** name2, int* bit,
  int max, char_data* ch, char* argument, bool exact )
{
  int        i;
  int    level;

  for( i = 0; i < max; i++ ) {
    if( matches( argument, *(name1+i*(name2-name1)), exact ) ) {
      if( *argument == '\0' || ( !is_number( argument )
        && *argument != '-' && !strcasecmp( argument, "off" ) ) ) {
        send( ch,
          "You must specify to what level you wish to set that.\r\n\r\n" );
        send( ch, "[ Current setting: %s ]\r\n",
          number_word( level_setting( bit, i ) ) );
        return TRUE;
        }
      if( ( level = atoi( argument ) ) < 0 || level > 3 ) {
        send( ch, "The allowed level range is from zero to three.\r\n" );
        return TRUE;
        }
      set_level( bit, i, level );
      if( level == 0 ) 
        send( ch, "%s turned off.\r\n",
          *(name1+i*(name2-name1)) );
      else
        send( ch, "%s set to level %s.\r\n",
          *(name1+i*(name2-name1)), number_word( level ) );
      return TRUE;
      }
    }

  return FALSE;
}



