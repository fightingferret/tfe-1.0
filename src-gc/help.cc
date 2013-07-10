#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include <syslog.h>
#include "define.h"
#include "struct.h"


#define HELP_IMMORTAL         0
#define HELP_SPELLS           1
#define HELP_FUNCTIONS        2
#define HELP_LOGIN            3


help_data**      help_list;
int               max_help;
bool         help_modified  = TRUE;


void         spell_help      ( char_data*, int );
void         make_w3         ( help_data* help );


/*
 *   HELP_DATA CLASS
 */


class Help_Data
{
 public:
  char*      name;
  char*      text;
  char*  immortal;
  int       level  [ 2 ];
  int    category;

  Help_Data   ( );
  ~Help_Data  ( );

  friend char* name( help_data* help ) {
    return help->name;
    }
};


Help_Data :: Help_Data( )
{
  record_new( sizeof( help_data ), MEM_HELP );

  name      = (char*) empty_string;
  text      = (char*) empty_string;
  immortal  = (char*) empty_string;
  level[0]  = 0;
  level[1]  = 0;
  category  = 0;
}


Help_Data :: ~Help_Data( )
{
  int pos;

  record_delete( sizeof( help_data ), MEM_HELP ); 
  free_string( name, MEM_HELP );
  free_string( text, MEM_HELP );
  free_string( immortal, MEM_HELP );

  for( pos = 0; pos < max_help; pos++ )
    if( help_list[pos] == this ) {
      remove( help_list, max_help, pos );
      break;
      }
}


/*
 *   READ/WRITE ROUTINES
 */


void load_helps( void )
{
  help_data*  help;
  FILE*         fp;
  int          pos;

  echo( "Loading Help ...\n\r" );

  help_list = NULL;
  max_help  = 0;

  fp = open_file( HELP_FILE, "r" );

  if( strcmp( fread_word( fp ), "#HELPS" ) ) 
    panic( "Load_helps: missing header" );

  for( ; ; ) {
    help           = new help_data;
    help->level[0] = fread_number( fp );
    help->level[1] = fread_number( fp );
    help->category = fread_number( fp );
    help->name     = fread_string( fp, MEM_HELP );

    if( *help->name == '$' ) {
      delete help;
      break;
      }

    help->text     = fread_string( fp, MEM_HELP );
    help->immortal = fread_string( fp, MEM_HELP );  

    pos = pntr_search( help_list, max_help, help->name );

    if( pos < 0 )
      pos = -pos-1;

    insert( help_list, max_help, help, pos );
    }

  fclose( fp );
}


bool save_help( char_data* ch )
{
  char         buf  [ TWO_LINES ];
  help_data*  help;
  FILE*         fp;
  int          pos;

  if( !help_modified ) 
    return FALSE;

  if( ( fp = open_file( HELP_FILE, "w" ) ) == NULL )
    return FALSE;

  fprintf( fp, "#HELPS\n" );

  for( pos = 0; pos < max_help; pos++ ) {
    help = help_list[pos];
    fprintf( fp, "%d %d %d %s~\n", help->level[0], help->level[1],
      help->category, help->name );
    fprintf( fp, ".%s~\n\n", help->text );
    fprintf( fp, ".%s~\n\n", help->immortal );
    }
  fprintf( fp, "-1 -1 -1 $~\n" );
  fclose( fp );

  help_modified = FALSE;

  if( ch != NULL ) {
    send( ch, "Help file written.\n\r" );
    sprintf( buf, "Help file written (%s).", ch->real_name() );
    info( "", LEVEL_HELP, buf, IFLAG_WRITES, 1, ch );
    }

  return TRUE;
}


/*
 *   W3 ROUTINES
 */


void smash_color( char* tmp )
{
  int i;
  int j;

  for( i = j = 0; tmp[i] != '\0'; i++ ) {
    if( tmp[i] == '\n' || tmp[i] == '\r' ) 
      continue;
    if( tmp[i] == '@' ) {
      if( tmp[++i] == '\0' )
        break;
      if( tmp[i] != '@' ) 
        continue;
      }
    if( tmp[i] == '<' ) 
      tmp[j++] = '[';
    else if( tmp[i] == '>' ) 
      tmp[j++] = ']';
    else
      tmp[j++] = tmp[i];    
    }

  tmp[j] = '\0';

  return;
}


void w3_help( )
{
  char    tmp  [ FOUR_LINES ];
  int       i;
  int     pos;
  FILE*    fp;

  sprintf( tmp, "%shelp/index.html", W3_DIR );

  if( ( fp = open_file( tmp, "w" ) ) == NULL )
    return;

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body>\n" );

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) {
    sprintf( tmp, "./%s/index.html", help_cat_table[i].name );
    smash_spaces( tmp );    
    fprintf( fp, "<p><a href=\"%s\"> %s </a></p>\n",
      tmp, help_cat_table[i].name );
    sprintf( tmp, "mkdir %shelp/%s/", W3_DIR, help_cat_table[i].name );
    smash_spaces( &tmp[6] );    
    system( tmp );
    }

  fprintf( fp, "</html>\n" );
  fprintf( fp, "</body>\n" );
  
  fclose( fp );

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) {
    sprintf( tmp, "%shelp/%s/index.html", W3_DIR, help_cat_table[i].name );
    smash_spaces( tmp );

    if( ( fp = fopen( tmp, "w" ) ) == NULL ) {
      bug( "W3_Help: Fopen error" );
      return;
      }

    fprintf( fp, "<html>\n" );
    fprintf( fp, "<body>\n" );

    for( pos = 0; pos < max_help; pos++ ) {
      if( help_list[pos]->category == i ) {
        sprintf( tmp, "./%s", help_list[pos]->name );
        smash_spaces( tmp );
        fprintf( fp, "<p><a href=\"%s\"> %s </a></p>\n",
          tmp, help_list[pos]->name );
        }
      }

    fprintf( fp, "</html>\n" );
    fprintf( fp, "</body>\n" );
  
    fclose( fp );
    }

  for( pos = 0; pos < max_help; pos++ ) 
    make_w3( help_list[pos] );
    
  return;
}


void make_w3( help_data* help )
{
  char     tmp  [ FOUR_LINES ];
  char*  input;
  FILE*     fp;

  sprintf( tmp, "%shelp/%s/%s", W3_DIR,
    help_cat_table[ help->category ].name, help->name );
  smash_spaces( tmp );

  if( ( fp = fopen( tmp, "w" ) ) == NULL ) {
    bug( "Make_W3( help ): Fopen error" );
    return;
    }

  fprintf( fp, "<html>\n" );
  fprintf( fp, "<body>\n" );

  for( input = help->text; *input != '\0'; ) {
    input = one_line( input, tmp );
    smash_color( tmp );
    if( !strcasecmp( tmp, "Syntax" ) ) 
      fprintf( fp, "<h1> Syntax </h1>\n" );
    else if( !strcasecmp( tmp, "Description" ) ) 
      fprintf( fp, "<h1> Description </h1>\n" );
    else 
      fprintf( fp, "%s<br>\n", tmp );
    }

  fprintf( fp, "</html>\n" );
  fprintf( fp, "</body>\n" );
  
  fclose( fp );

  return;
}


/*
 *   FIND_HELP ROUTINE
 */


bool can_read( char_data* ch, help_data* help )
{
  return has_permission( ch, help->level );
}



help_data* find_help( char_data* ch, const char* argument )
{
  help_data*        help;
  int              first  = -2;
  int                pos;

  if( number_arg( argument, pos ) ) {
    if( pos < 0 || pos >= max_help ) {
      send( ch, "There is no help file with that index.\n\r" ); 
      return NULL;
      }
    if( !can_read( ch, help_list[pos] ) ) {
      send( ch, "You do not have the required permission.\n\r" );
      return NULL;
      }
    return help_list[pos];
    }

  pos = pntr_search( help_list, max_help, argument );

  if( pos >= 0 ) {
    if( can_read( ch, help_list[pos] ) )
      return help_list[pos];
    pos++;
    }
  else
    pos = -pos-1;

  for( ; pos < max_help; pos++ ) {
    help = help_list[pos];
    if( !fmatches( argument, help->name ) )
      break;
    if( can_read( ch, help ) ) {
      if( first != -2 ) {
        if( first != -1 ) {
          page( ch, "More than one match was found - please be more\
 specific in what topic you\n\rwant help on.\n\r\n\r" );
          page( ch, "  [%3d] %s:%15s%s\n\r", first,
            help_cat_table[help_list[first]->category].name, "",
            help_list[first]->name );
          first = -1;
          }
        page( ch, "  [%3d] %s:%15s%s\n\r", pos,
          help_cat_table[help->category].name, "", help->name );
        }
      else {
        first = pos;
        }
      }
    }

  if( first >= 0 )
    return help_list[first];

  if( first == -2 )
    send( ch, "No matching help file was found - use index to see a list of\
 topics for\n\rwhich help exists.\n\r" );
      
  return NULL;
}


/*
 *   MAIN HELP ROUTINE
 */


void do_help( char_data* ch, char* argument )
{
  char          tmp  [ 3*MAX_STRING_LENGTH ];
  help_data*   help;
  int             i;
  int        length  = strlen( argument );

  if( pet_help( ch ) || ch->link == NULL )
    return;
  
  if( ( help = find_help( ch,
    *argument == '\0' ? "summary" : argument ) ) == NULL ) 
    return;

  if( ch->link->connected != CON_PLAYING ) {
    send( ch, help->text );
    return;
    }

  if( help->category == HELP_SPELLS ) 
    for( i = 0; i < MAX_SPELL; i++ ) 
      if( !strncasecmp( spell_table[i].name, argument, length ) ) {
        spell_help( ch, i );
        return;
        }
 
  page( ch, "Topic: %s\n\r", help->name );

  if( is_builder( ch ) ) {
    strcpy( tmp, "Level: " );
    permission_flags.sprint( &tmp[7], help->level );
    strcat( tmp, "\n\r" );
    page( ch, tmp );
    }

  page( ch, "\n\r" );
  convert_to_ansi( ch, help->text, tmp );
  page( ch, tmp );

  if( help->immortal != empty_string && is_builder( ch ) ) {
    page( ch, "\n\r" );
    convert_to_ansi( ch, help->immortal, tmp );
    page( ch, tmp );
    }

  return;
}


void help_link( link_data* link, const char* argument )
{
  char         tmp  [ 3*MAX_STRING_LENGTH ];
  int          pos;

  pos = pntr_search( help_list, max_help, argument );

  if( pos < 0 ) {
    send( link, "Help subject %s not found.\n\r", argument );
    }
  else if( link->player != NULL && link->player->pcdata != NULL ) {
    convert_to_ansi( link->player, help_list[pos]->text, tmp );
    if( link->connected == CON_PLAYING )
      page( link->player, tmp );
    else
      send( link, tmp );
    }
  else {
    send( link, help_list[pos]->text );
    }

  return;
}


void do_motd( char_data* ch, char* )
{
  do_help( ch, "motd" );
  return;  
}


/*
 *   INDEX COMMAND
 */


void do_index( char_data* ch, char* argument )
{
  char           tmp  [ MAX_STRING_LENGTH ];
  int         length  = strlen( argument ); 
  help_data*    help;
  int           i, j;
  int            pos;
  int          trust  = get_trust( ch );

  if( *argument =='\0' ) {
    page_title( ch, "Help Categories" );
    for( i = j = 0; i < MAX_ENTRY_HELP_CAT; i++ ) 
      if( help_cat_table[i].level <= trust ) {
        sprintf( tmp, "%18s%s", help_cat_table[i].name,
          (j++)%4 != 3 ? "" : "\n\r" );
        page( ch, tmp );
        }
    page( ch, "\n\r%s", j%4 != 0 ? "\n\r" : "" );
    page_centered( ch, "[ Type index <category> to see a list of help\
 files in that category. ]" );
    return;
    }

  for( i = 0; i < MAX_ENTRY_HELP_CAT; i++ ) 
    if( help_cat_table[i].level <= trust
      && !strncasecmp( argument, help_cat_table[i].name, length ) ) 
      break;

  if( i == MAX_ENTRY_HELP_CAT ) {
    send( ch, "Unknown help category.\n\r" );
    return;
    }

  page_title( ch, "Help Files - %s", help_cat_table[i].name );      

  for( j = 0, pos = 0; pos < max_help; pos++ ) {
    help = help_list[pos];
    if( help->category != i || !can_read( ch, help ) )
      continue;
    page( ch, "%18s%s", help->name, ++j%4 ? "" : "\n\r" );
    }
  if( j%4 != 0 )
    page( ch, "\n\r" );
  page( ch, "\n\r" );
  page_centered( ch, "[ Type help <file> to read a help file. ]" );

  return;
}


/*
 *   ONLINE EDITING OF HELP
 */


void do_hedit( char_data *ch, char *argument )
{
  help_data*    help  = ch->pcdata->help_edit;
  wizard_data*   imm;
  int            pos;  

  if( *argument == '\0' ) {
    send( ch, "What help file do you wish to edit?\n\r" );
    return;
    }

  if( !strncasecmp( argument, "delete", 6 ) ) {
    if( help == NULL ) {
      send( ch, "You aren't editing any help file.\n\r" );
      return;
      }
    for( int i = 0; i < player_list; i++ ) {
      if( ( imm = wizard( player_list[i] ) ) != NULL
        && imm->Is_Valid( )
        && imm->pcdata->help_edit == help && imm != ch ) {
        send( ch, "The help file you are editing was just deleted.\n\r" );
        imm->pcdata->help_edit = NULL;
        }
      }
    help_modified         = TRUE;
    ch->pcdata->help_edit = NULL;
    send( ch, "Help file '%s' removed.\n\r", help->name );
    delete help;
    return; 
    }

  if( !strncasecmp( argument, "new ", 4 ) ) {
    argument += 4;
    if( argument[0] == '\0' ) {
      send( "On what subject do you want to create a help?\n\r", ch );
      return;
      }
    help        = new help_data;
    help->name  = alloc_string( argument, MEM_HELP );

    pos = pntr_search( help_list, max_help, "blank" );
    if( pos >= 0 ) 
      help->text = alloc_string( help_list[pos]->text, MEM_HELP );
    else
      help->text = (char*) empty_string;

    pos = pntr_search( help_list, max_help, argument );

    if( pos < 0 )
      pos = -pos-1;

    insert( help_list, max_help, help, pos );
    ch->pcdata->help_edit = help;
    send( "Help subject created.\n\r", ch );
    help_modified = TRUE;
    return;
    }

  if( ( help = find_help( ch, argument ) ) == NULL ) 
    return;

  ch->pcdata->help_edit = help;
  send( ch, "Hdesc and hset now operate on %s.\n\r", help->name );
}  


void do_hdesc( char_data *ch, char *argument ) 
{
  help_data* help = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\n\r" );
    return;
    }

  help->text    = edit_string( ch, argument, help->text, MEM_HELP );
  help_modified = TRUE;
}


void do_hbug( char_data* ch, char* argument ) 
{
  help_data* help = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\n\r" );
    return;
    }

  help->immortal = edit_string( ch, argument, help->immortal, MEM_HELP );
  help_modified  = TRUE;
}


void do_hset( char_data *ch, char *argument )
{
  help_data*   help  = ch->pcdata->help_edit;

  if( help == NULL ) {
    send( ch, "You are not editing any subject.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Syntax: hset <field> <value>\n\r" );
    return;
    }

  help_modified = TRUE;

  if( matches( argument, "level" ) ) {
    permission_flags.set( ch, argument, help->level );
    return;
    }

#define hcn( i )   help_cat_table[i].name

  class type_field type_list[] = {
    { "category",  MAX_ENTRY_HELP_CAT,  &hcn(0),  &hcn(1),  &help->category },
    { "",          0,                   NULL,     NULL,     NULL            }
    };

#undef hcn

  if( process( type_list, ch, help->name, argument ) )
    return;

  if( matches( argument, "name" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the help name to?\n\r" );
      return;
      }
    send( ch, "Help name changed from %s to %s.\n\r",
      help->name, argument );
    free_string( help->name, MEM_HELP );
    help->name = alloc_string( argument, MEM_HELP );
    return;
    }
 
  send( ch, "See help hset.\n\r" );
}


/*
 *  FUNCTION HELP
 */


void do_functions( char_data* ch, char* argument )
{
  char     tmp  [ MAX_STRING_LENGTH ];
  char     buf  [ MAX_STRING_LENGTH ];
  bool   found  = FALSE;
  int   length  = strlen( argument );
  int      pos;

  pos = pntr_search( help_list, max_help, argument );
  
  if( pos >= 0 && help_list[pos]->category == HELP_FUNCTIONS ) {
    convert_to_ansi( ch, help_list[pos]->text, buf );
    page( ch, buf );
    return;
    }

  for( int i = 0; cfunc_list[i].name[0] != '\0'; i++ ) {
    if( strncasecmp( argument, cfunc_list[i].name, length ) )
      continue;
    if( !found ) {
      found = TRUE;
      sprintf( tmp, "%-15s %-45s %s\n\r",
        "Function Name", "Arguments", "Returns" );
      sprintf( tmp+strlen( tmp ), "%-15s %-45s %s\n\r",
        "-------- ----", "---------", "-------" );
      page( ch, tmp );
      }
    sprintf( tmp, "%-15s (", cfunc_list[i].name );
    for( int j = 0; j < 4; j++ )
      if( cfunc_list[i].arg[j] != NONE )
        sprintf( tmp+strlen( tmp ), "%s %s", j == 0 ? "" : ",",
          arg_type_name[ cfunc_list[i].arg[j] ] );
    sprintf( tmp+strlen( tmp ), " )" ); 
    sprintf( buf, "%-61s %s\n\r", tmp,
      arg_type_name[ cfunc_list[i].type ] );
    page( ch, buf );
    }

  if( !found ) 
    send( ch, "No matching function found.\n\r" );
}


/*
 *   SPELL HELP ROUTINE
 */


void do_spells( char_data* ch, char* argument )
{
  int i;

  if( pet_help( ch ) || ch->link == NULL )
    return;

  if( argument[0] == '\0' ) {
    send( "What spell do you want info on?\n\r[To get a list of spells\
 type abil sp <class>.]\n\r", ch );
    return;
    }

  for( i = 0; i < MAX_SPELL; i++ )
    if( !strncasecmp( spell_table[i].name, argument,
      strlen( argument ) ) )
      break;

  if( i == MAX_SPELL ) {
    send( "Unknown Spell.\n\r", ch );
    }
  else {
    spell_help( ch, i );
    }

  return;
}


void spell_help( char_data* ch, int i )
{
  char                tmp  [ 3*MAX_STRING_LENGTH ];
  bool              found  = FALSE;
  obj_clss_data*  reagent;
  int                j, k;
  int                 pos;
  int              length;
  int               level;
  help_data*         help;

  sprintf( tmp,
    "%10sName: %s\n\r   Energy Cost: %s\n\r Turns to Cast: %d\n\r",
    "", spell_table[i].name, spell_table[i].cast_mana,
    spell_table[i].wait );
  page( ch, tmp );

  if( spell_table[i].duration != empty_string )
    page( ch, "      Duration: %s\n\r", spell_table[i].duration );
  if( spell_table[i].damage != empty_string ) 
    page( ch, "        Damage: %s\n\r", spell_table[i].damage );
  if( spell_table[i].leech_mana != empty_string ) 
    page( ch, "  Leech of Max: %s\n\r", spell_table[i].leech_mana );
  if( spell_table[i].regen != empty_string ) 
    page( ch, "   Regen Leech: %s\n\r", spell_table[i].regen );

  if( is_builder( ch ) )
    page( ch, "%10sSlot: %d\n\r", "", i );

  strcpy( tmp, "   Class/Level:" );
  for( j = 0; j < MAX_CLSS; j++ )
    if( ( level = skill_table[SPELL_FIRST+i].level[j] ) > 0
      && clss_table[j].open ) {
      length = strlen( tmp );
      sprintf( tmp+length, "%s %s %d%s",
        found ? "," : "", clss_table[j].name,
        level, number_suffix( level ) );
      length += found+1;
      tmp[length] = toupper( tmp[length] );
      found = TRUE;
      }
  sprintf( tmp+strlen( tmp ), "%s\n\r", found ? "" : " none" ); 
  page( ch, tmp );

  found = FALSE;

  page( ch, "--------------------------------\n\r" );
  page( ch, "Description:\n\r\n\r" );

  pos = pntr_search( help_list, max_help, spell_table[i].name );

  if( pos >= 0 ) {
    help = help_list[pos];
    convert_to_ansi( ch, help->text, tmp );
    page( ch, tmp );

    if( help->immortal != empty_string && is_builder( ch ) ) {
      page( ch, "\n\r" );
      convert_to_ansi( ch, help->immortal, tmp );
      page( ch, tmp );
      }
    }
  else
    page( ch, "none\n\r" );

  page( ch, "\n\rReagents:\n\r" );  
  for( j = 0; j < MAX_SPELL_WAIT; j++ ) {
    for( k = 0; k < j; k++ )
      if( abs( spell_table[i].reagent[j] )
        == abs( spell_table[i].reagent[k] ) )
        break;
    if( k != j )
      continue;
    if( ( reagent = get_obj_index(
      abs( spell_table[i].reagent[j] ) ) )  == NULL )
      continue;
    found = TRUE;
    page( ch, "   %s\n\r", reagent->Name( ) );
    }

  if( !found ) 
    page( ch, "  none\n\r" );

  return;
}








