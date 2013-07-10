#include "ctype.h"
#include "dirent.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "syslog.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"


clan_data**   clan_list  = NULL;
int            max_clan  = 0;


void           rename_clan       ( clan_data*, char* );
void           create_clan       ( pfile_data*, char* );
bool           good_clan_name    ( char_data*, char* );
void           display           ( char_data*, clan_data* );
void           extract           ( clan_data* );
bool           edit_clan         ( char_data*, clan_data*, bool = FALSE );
void           expell            ( char_data*, clan_data*, pfile_data* );


const char* clan_flags [ MAX_CLAN_FLAGS ] = { "Approved", "Exist.Known",
  "Members.Known" };


inline bool knows( char_data* ch, clan_data* clan )
{
  return( ch->pcdata->pfile->clan == clan
    || ( is_set( clan->flags, CLAN_APPROVED )
    && is_set( clan->flags, CLAN_KNOWN ) )
    || has_permission( ch, PERM_CLANS ) );
}


/*
 *   CLAN ROUTINES
 */


clan_data* find_clan( char_data* ch, char* argument )
{
  clan_data*  clan;
  int            i;

  for( i = 0; i < max_clan; i++ ) 
    if( knows( ch, clan = clan_list[i] )
      && ( fmatches( argument, clan->name )
      || fmatches( argument, clan->abbrev ) ) )
      return clan;

  send( ch, "No such clan exists.\n\r" );

  return NULL;
}


bool edit_clan( char_data* ch, clan_data* clan, bool msg )
{
  title_data*  title;

  if( clan == NULL ) {
    if( msg ) 
      send( ch, "You are not in a clan so editing it makes no sense.\n\r" );
    return FALSE;
    }

  if( !has_permission( ch, PERM_CLANS ) ) {
    if( is_set( clan->flags, CLAN_APPROVED ) ) {
      if( msg )
        send( ch, "After the clan is approved it may not be edited.\n\r" );
      return FALSE;
      }
    if( ( title = get_title( ch->pcdata->pfile ) ) == NULL
      || !is_set( title->flags, TITLE_EDIT_CHARTER ) ) {
      if( msg )
        send( ch, "You don't have permission to edit the clan.\n\r" );
      return FALSE;
      }
    }

  return TRUE;
}


inline void clan_options( char_data* ch, clan_data* clan,
   char* argument )
{
  const char* response;

  #define types 5

  if( clan == NULL ) {
    send( ch,
      "You cannot edit or view options for a clan you are not in.\n\r" );
    return;
    }    

  const char* title [types] = {
    "*Options", "*Races", "*Classes",
    "*Alignments", "*Sexes" };
  int max [types] = {
    MAX_CLAN_FLAGS, MAX_PLYR_RACE, MAX_CLSS, MAX_ENTRY_ALIGNMENT, 2 };
  const char** name1 [types] = {
    &clan_flags[0], &race_table[0].name, &clss_table[0].name,
    &alignment_table[0].name, &sex_name[ SEX_MALE ] }; 
  const char** name2 [types] = {
    &clan_flags[1], &race_table[1].name, &clss_table[1].name,
    &alignment_table[1].name, &sex_name[ SEX_FEMALE ] }; 
  int* flag_value [types] = { clan->flags, &clan->races, &clan->classes,
    &clan->alignments, &clan->sexes };
  int can_edit [types] = { 1, -1, -1, -1, -1 };
  int not_edit [types] = { 1,  1,  1,  1,  1 };

  response = flag_handler( title, name1, name2, flag_value, max,
    edit_clan( ch, clan ) ? can_edit : not_edit,
    has_permission( ch, PERM_CLANS ) ? (const char*) NULL
    : "You do not have permission to alter that flag.\n\r",
    ch, argument, types );

  clan->modified = TRUE;

  #undef types

  return;
}


inline void clan_titles( char_data* ch, clan_data* clan, char* argument )
{
  title_data*  title;
  int              i;

  if( clan == NULL ) {
    send( ch, "Since you aren't in a clan the T option is meaningless.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    page_underlined( ch, "%-20s %s\n\r", "Title", "Character" );
    for( i = 0; i < clan->titles.size; i++ ) {
      title = clan->titles.list[i];
      page( ch, "%-20s %s\n\r", title->name,
        title->pfile == NULL ? "Noone" : title->pfile->name );
      }
    return;
    }

  send( ch, "Editting of titles is not yet enabled.\n\r" );
 
  return;
}


inline void create_clan( pfile_data* pfile, char* name )
{
  *name = toupper( *name );

  clan_data*    clan  = new clan_data( name );
  title_data*  title  = new title_data( "Founder", pfile );

  add_member( clan, pfile );
  insert( clan->titles.list, clan->titles.size, title, 0 );

  set_bit( title->flags, TITLE_SET_FLAGS    );
  set_bit( title->flags, TITLE_EDIT_CHARTER );
  set_bit( title->flags, TITLE_RECRUIT      );
  set_bit( title->flags, TITLE_REMOVE_NOTES );

  set_bit( &clan->classes, pfile->clss );
  set_bit( &clan->races,   pfile->race );

  save_clans( clan );
  save_notes( clan );

  return;
}


const char* create_msg = "Summoning a scribe daemon you request that the\
 paperwork be drawn up to form a clan by the name of '%s'.  He politely\
 nods his head, scribbles a few lines on the back of an envelope, hands you\
 the envelope, and then informs you that your bank account will be deducted\
 2000 sp for services rendered.";


void do_clans( char_data* ch, char* argument )
{
  player_data*  pc;
  clan_data*      clan;
  title_data*    title;
  pfile_data*    pfile;
  int            flags;
  int                i;

  if( is_mob( ch ) )
    return;

  pc = player( ch );
  clan   = ch->pcdata->pfile->clan;

  if( !get_flags( ch, argument, &flags, "fotcna", "Clans" ) )
    return;

  if( exact_match( argument, "expell" ) ) {
    if( ( title = get_title( ch->pcdata->pfile ) ) == NULL
      || !is_set( title->flags, TITLE_RECRUIT ) ) {
      send( ch, "You don't have permission to expell clan members.\n\r" );
      return;
      }
    if( *argument == '\0' ) {
      send( ch, "Who do you wish to expell from the clan?\n\r" );
      return;
      }
    if( ( pfile = find_pfile( argument, ch ) ) != NULL ) 
      expell( ch, clan, pfile );
    return;
    }

  if( has_permission( ch, PERM_CLANS )
    && exact_match( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which clan do you wish to delete?\n\r" );
      }
    else if( ( clan = find_clan( ch, argument ) ) != NULL ) {
      extract( clan );
      send( ch, "Clan deleted.\n\r" );
      }
    return;
    }

  if( is_set( &flags, 0 ) ) {
    if( pc->pcdata->pfile->clan != NULL ) {
      send( ch, "You cannot form a clan if already a member of one.\n\r" );
      return;
      }
    if( pc->shdata->level < 20 ) {
      send( ch, "You must be at least 15th level to form a clan.\n\r" );
      return;
      }
    if( pc->bank < 20000 ) {
      send( ch, "There is a 2000 sp charge, withdrawn from bank, for forming\
 a clan and you lack\n\rsufficent funds.\n\r" );
      return;
      } 
    if( *argument == '\0' ) {
      send( ch, "What name do you wish for the clan?\n\r" );
      return;
      }
    if( good_clan_name( ch, argument ) ) {
      create_clan( ch->pcdata->pfile, argument );
      pc->bank -= 20000;
      fsend( ch, create_msg, argument );
      }
    return;
    }

  if( is_set( &flags, 2 ) ) {
    clan_titles( ch, clan, argument );
    return;
    } 

  if( is_set( &flags, 1 ) ) {
    clan_options( ch, clan, argument );
    return;
    }

  if( is_set( &flags, 4 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the name of the clan to?\n\r" );
      return;
      }
    free_string( clan->name, MEM_CLAN );
    clan->name = alloc_string( argument, MEM_CLAN );
    send( ch, "Name of clan set to %s.\n\r", argument );
    return;
    }    

  if( is_set( &flags, 5 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    if( *argument == '\0' ) {
      send( ch, "What do you want to set the abbrev for the clan to?\n\r" );
      return;
      }
    if( !good_clan_name( ch, argument ) )
      return; 
    rename_clan( clan, argument );
    send( ch, "Clan abbrev changed to %s.\n\r", argument );
    return;
    }

  if( is_set( &flags, 3 ) ) {
    if( !edit_clan( ch, clan, TRUE ) )
      return;
    clan->modified = TRUE;
    clan->charter  = edit_string( ch, argument,
      clan->charter, MEM_CLAN );
    return;
    }

  if( *argument == '\0' ) {
    page_underlined( ch, "Abrv  %-50s Members\n\r", "Clan" );
    for( i = 0; i < max_clan; i++ ) 
      if( knows( ch, clan = clan_list[i] ) ) 
        page( ch, "%4s  %-50s %s\n\r", clan->abbrev, clan->name,
          knows_members( ch, clan ) ? number_word( clan->members.size )
          : "???" );
    return;
    }

  if( ( clan = find_clan( ch, argument ) ) != NULL ) 
    display( ch, clan ); 
}


/*
 *   CLAN EDITTING
 */


void display_edit( char_data* ch, clan_data* clan )
{
  title_data** list  = clan->titles.list;
  char*       flags  = "FCRN";
  char*         tmp  = static_string( );
  char*      letter;
  int          i, j;

  send( ch, "    Name: %s\n\r", clan->name );
  send( ch, "  Abbrev: %s\n\r\n\r", clan->abbrev );

  send_underlined( ch,
    "Nmbr  Title               Holder              Flags\n\r" );  

  for( i = 0; i < clan->titles.size; i++ ) { 
    letter = tmp;
    for( j = 0; j < MAX_TITLE; j++ )
      if( is_set( list[i]->flags, j ) )
        *letter++ =  flags[j];
    *letter = '\0';
    send( ch, "%-6d%-20s%-20s%s\n\r", 
      i+1, list[i]->name, list[i]->pfile == NULL ? "noone" 
      : list[i]->pfile->name, tmp );
    } 

  return;
}


void do_cedit( char_data* ch, char* argument )
{
  clan_data*    clan;
  title_data*  title;
  pfile_data*  pfile;
  int              i;

  if( ( clan = ch->pcdata->pfile->clan ) == NULL ) {
    send( ch, "Cedit operates on the clan you are in and you\
 aren't in a clan.\n\r" );
    return;
    }

  if( *argument == '\0' ) {
    display_edit( ch, clan );
    return;
    }

  if( matches( argument, "new" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What should the new title be called?\n\r" );
      return;
      }
    if( clan->titles.size > 5 ) {
      send( ch, "A clan is restricted to 5 titles.\n\r" );
      return;
      }
    insert( clan->titles.list, clan->titles.size,
      new title_data( argument, NULL ), clan->titles.size );
    send( ch, "New title %s added.\n\r", argument );
    return;
    }

  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "Which title do you wish to delete?\n\r" );
      return;
      }
    if( ( i = atoi( argument ) ) < 1 || i > clan->titles.size ) {
      send( ch, "No such title exists.\n\r" );
      return;
      }
    title = clan->titles.list[--i];
    send( ch, "Title '%s' removed.\n\r", title->name );
    remove( clan->titles.list, clan->titles.size, i );
    delete title;
    return;
    }

  if( number_arg( argument, i ) ) {
    if( i < 1 || i > clan->titles.size ) {
      send( ch, "No such title exists.\n\r" );
      return;
      }
    title = clan->titles.list[--i]; 
    set_flags( ch, argument, title->flags, "FCRN" );
    if( matches( argument, "title" ) ) {
      set_string( ch, argument, title->name, "title", MEM_CLAN );
      return;
      }  
    if( matches( argument, "holder" ) ) {
      if( ( pfile = find_pfile( argument, ch ) ) != NULL ) {
        if( pfile->clan != clan ) {
          fsend( ch, "%s isn't a member of %s.\n\r",
            pfile->name, clan->name );
	  }
        else {
          title->pfile = pfile;
          send( ch, "%s set to %s.\n\r", title->name, title->pfile->name );
	  }
        }
      return;
      }
    }

  send( ch, "Unknown syntax - see help cedit.\n\r" );

  return;
}


/*
 *   ALLEGIANCE ROUTINES
 */


inline bool can_join( char_data* ch, clan_data* clan )
{
  const char*  word;
  int             i;
  
  if( has_permission( ch, PERM_CLANS ) )
    return TRUE;

  if( !is_set( clan->flags, CLAN_APPROVED ) ) {
    fsend( ch, "%s has not been approved so may not accept members.",
      name( clan ) );
    return FALSE;
    }

  if( ch->shdata->level < clan->min_level ) {
    fsend( ch, "To join %s you must be at least level %d.",
      name( clan ), clan->min_level );
    return FALSE;
    }

 #define types 4

  const char** name1 [types] = {
    &race_table[0].name, &clss_table[0].name,
    &alignment_table[0].name, &sex_name[ SEX_MALE ] }; 
  const char** name2 [types] = {
    &race_table[1].name, &clss_table[1].name,
    &alignment_table[1].name, &sex_name[ SEX_FEMALE ] }; 
  int* flag_value [types] = {
    &clan->races, &clan->classes,
    &clan->alignments, &clan->sexes };
  int player_value [types] = {
    ch->shdata->race, ch->pcdata->clss, ch->shdata->alignment,
    ch->sex-SEX_MALE };

  for( i = 0; i < types; i++ ) {
    if( !is_set( flag_value[i], player_value[i] ) ) {
      word = *(name1[i]+player_value[i]*(name2[i]-name1[i]));
      fsend( ch, "As %s %s you would not be allowed to join %s.\n\r",
        isvowel( *word ) ? "an" : "a", word, name( clan ) );
      return FALSE;
      }
    }

  #undef types

  return TRUE;
}


void do_allegiance( char_data* ch, char* argument )
{
  char_data*    victim;
  player_data*  pc;
  clan_data*      clan;
  title_data*    title;

  if( is_mob( ch ) )
    return;

  pc = player( ch );
  clan   = ch->pcdata->pfile->clan;

  if( *argument == '\0' ) {
    if( clan == NULL ) {
      send( ch, "Swear allegiance to whom?\n\r" );
      }
    else {
      fsend( ch, "You have sworn allegiance to %s.\n\r", name( clan ) );
      send( ch, "Switch your allegiance to whom?\n\r" );
      }
    return; 
    }

  if( !strcasecmp( argument, "none" ) ) {
    if( clan == NULL ) {
      send( ch, "You have sworn allegiance to noone already.\n\r" );
      }
    else {
      fsend( ch, "You revoke your allegiance to %s.\n\r", name( clan ) );
      remove_member( pc );   
      remove_member( pc->pcdata->pfile );
      }
    return;
    }

  if( clan != NULL ) {
    fsend( ch, "You must first revoke your allegiance to %s.", name( clan ) );
    return;
    }

  if( ( victim = one_player( ch, argument, "swear allegiance to",
    ch->array ) ) == NULL )
    return;

  if( ch == victim ) {
    send( ch, "Allegiance to yourself is assumed.\n\r" );
    return;
    }

  clan = victim->pcdata->pfile->clan;

  if( clan == NULL || ( !knows_members( ch, clan )
    && !victim->Befriended( ch ) ) ) {
    send( ch, "They are not a member of any clan.\n\r" );
    return;
    }

  if( !can_join( ch, clan ) )
    return;

  if( !consenting( victim, ch ) ) {
    fsend( ch, "Swearing allegiance to %s, who has not consented to\
 you joining %s clan is pointless.", victim, victim->His_Her( ) );
    return;
    }

  title = get_title( victim->pcdata->pfile );

  if( title == NULL 
    || !is_set( title->flags, TITLE_RECRUIT ) ) {
    send( ch, "%s is not permitted to recruit members.\n\r", victim );
    return;
    }

  add_member( clan, ch->pcdata->pfile );
  save_clans( clan );

  fsend( ch, "You kneel down and swear allegiance to %s.", name( clan ) );
  fsend( *victim->array,
    "%s kneels down and swears allegiance to %s.", ch, name( clan ) );

  return;
}


/*
 *   INTERNAL ROUTINES
 */


bool good_clan_name( char_data* ch, char* name ) 
{
  int i;

  if( strlen( name ) != 4 ) {
    send( ch,
      "Clan abbreviation must be exactly 4 letters.\n\r" );
    return FALSE;
    }

  for( i = 0; i < 4; i++ ) 
    if( !isalpha( name[i] ) ) {
      send( ch, "The clan name may only contain letters.\n\r" );
      return FALSE;
      }

  for( i = 0; i < max_clan; i++ )
    if( !strncasecmp( name, clan_list[i]->name, 4 ) ) {
      send( ch, "There is already a clan using that abbreviation.\n\r" );
      return FALSE;
      }

  return TRUE;
} 


title_data* get_title( pfile_data* pfile )
{
  clan_data*  clan  = pfile->clan;
  int            i;

  if( clan == NULL )
    return NULL;

  for( i = 0; i < clan->titles.size; i++ )
    if( clan->titles.list[i]->pfile == pfile )
      return clan->titles.list[i];

  return NULL;
}


/*
 *   ADDING/REMOVING MEMBERS
 */


void add_member( clan_data* clan, pfile_data* pfile )
{
  if( pfile->clan != NULL ) {
    bug( "Add_Member: Character is already in a clan." );
    return;
    }

  clan->members += pfile;
  pfile->clan    = clan;
}


void remove_member( player_data* player )
{
  if( player->noteboard == NOTE_CLAN )
    player->noteboard = NOTE_GENERAL;

  if( player->note_edit != NULL
    && player->note_edit->noteboard == NOTE_CLAN )
    player->note_edit = NULL;

  return;
} 


void remove_member( pfile_data* pfile )
{
  clan_data*  clan;
  int            i;

  if( ( clan = pfile->clan ) == NULL )
    return;

  clan->members -= pfile;

  for( i = 0; i < clan->titles.size; i++ ) 
    if( clan->titles.list[i]->pfile == pfile )
      clan->titles.list[i]->pfile = NULL;

  pfile->clan = NULL;

  if( is_empty( clan->members ) ) {
    extract( clan );
    return;
    }

  save_clans( clan );

  return;
}


void extract( clan_data* clan )
{
  player_data*  pc;
  int                i;

  for( i = 0; i < max_clan; i++ ) 
    if( clan_list[i] == clan )
      break;

  if( i == max_clan ) {
    bug( "Extract( Clan ): Non-existent clan!?" );
    return;
    }

  remove( clan_list, max_clan, i );

  for( i = 0; i < player_list; i++ ) {
    pc = player_list[i]; 
    if( pc->Is_Valid( )
      && pc->pcdata->pfile->clan == clan )
      remove_member( pc );
    }

  for( i = 0; i < clan->members; i ++ )
    clan->members[i]->clan = NULL;

  delete_file( CLAN_DIR, clan->abbrev );
  delete_file( CLAN_NOTE_DIR, clan->abbrev );

  delete clan;
} 


void expell( char_data* ch, clan_data* clan, pfile_data* pfile )
{
  char*            tmp  = static_string( );
  player_data*  pc;

  if( pfile->clan != clan ) {
    send( ch, "%s already isn't in your clan.\n\r", pfile->name );
    return;
    }

  if( ch->pcdata->pfile == pfile ) {
    send( ch, "You can't expell yourself - use allegiance none.\n\r" );
    return;
    }

  for( int i = 0; i < player_list; i++ ) { 
    pc = player_list[i];
    if( pc->Is_Valid( )
      && pc->pcdata->pfile == pfile ) {
      send( pc, ">> %s has expelled you from your clan. <<\n\r", ch );
      remove_member( pc );
      break;
      }
    }

  remove_member( pfile );
  send( ch, "%s expelled.\n\r", pfile->name );

  sprintf( tmp, "%s has been expelled from the clan by %s.",
    pfile->name, ch->descr->name );

  info( tmp, 1, tmp, IFLAG_CLANS, 3, ch, clan );

  return;
}


/*
 *   DISK ROUTINES
 */


void rename_clan( clan_data* clan, char* new_name )
{
  rename_file( CLAN_DIR,      clan->abbrev, CLAN_DIR,      new_name );
  rename_file( CLAN_NOTE_DIR, clan->abbrev, CLAN_NOTE_DIR, new_name );

  free_string( clan->abbrev, MEM_CLAN );
  clan->abbrev = alloc_string( new_name, MEM_CLAN );

  return;
}


void save_clans( clan_data* clan )
{
  FILE*               fp;
  pfile_data*      pfile;
  title_array*    titles;

  if( clan == NULL ) {
    for( int i = 0; i < max_clan; i++ )
      save_clans( clan_list[i] );
    return;
    }

  clan->modified = FALSE;

  if( ( fp = open_file( CLAN_DIR, clan->abbrev, "w" ) ) == NULL ) 
    return;

  fprintf( fp, "#CLAN\n\n" );

  fprintf( fp, "%s~\n",   clan->name );
  fprintf( fp, "%s~\n",   clan->charter );
  fprintf( fp, "%d %d\n", clan->flags[0], clan->flags[1] );
  fprintf( fp, "%d %d %d %d %d\n",
    clan->min_level, clan->races, clan->classes,
    clan->alignments, clan->sexes );
  fprintf( fp, "%d\n",    clan->date );

  titles  = &clan->titles;

  fprintf( fp, "%d %d\n\n", titles->size, clan->members.size );

  for( int i = 0; i < titles->size; i++ ) {
    pfile = titles->list[i]->pfile;
    fprintf( fp, "%s~\n",      titles->list[i]->name );
    fprintf( fp, "%d %d %d\n", pfile == NULL ? -1 : pfile->ident,
      titles->list[i]->flags[0], titles->list[i]->flags[1] );
    }

  for( int i = 0; i < clan->members; i++ )
    fprintf( fp, "%d\n", clan->members[i]->ident );

  fclose( fp );
}


void load_clans( void )
{
  FILE*              fp;
  DIR*             dirp;
  struct dirent*     dp;
  clan_data*       clan;
  title_array*   titles;
  pfile_data*     pfile;
  int                 i;
  int            length;

  echo( "Loading Clans ...\n\r" );

  clan_list = NULL;
  max_clan  = 0;
  dirp      = opendir( CLAN_DIR );
 
  for( dp = readdir( dirp ); dp != NULL; dp = readdir( dirp ) ) {
    if( !strcmp( dp->d_name, "." ) || !strcmp( dp->d_name, ".." ) )
      continue;

    fp = open_file( CLAN_DIR, dp->d_name, "r" );

    if( strcmp( fread_word( fp ), "#CLAN" ) ) 
      panic( "Load_Clans: Missing header." );

    clan              = new clan_data( dp->d_name );
    clan->name        = fread_string( fp, MEM_CLAN );
    clan->charter     = fread_string( fp, MEM_CLAN );
    clan->flags[0]    = fread_number( fp );
    clan->flags[1]    = fread_number( fp );
    clan->min_level   = fread_number( fp );
    clan->races       = fread_number( fp );
    clan->classes     = fread_number( fp );
    clan->alignments  = fread_number( fp );
    clan->sexes       = fread_number( fp );
    clan->date        = fread_number( fp );
 
    titles        = &clan->titles;
    titles->size  = fread_number( fp );
    titles->list  = new title_data* [ titles->size ];
    length        = fread_number( fp );

    for( i = 0; i < titles->size; i++ ) {
      titles->list[i]           = new title_data;
      titles->list[i]->name     = fread_string( fp, MEM_CLAN );
      titles->list[i]->pfile    = get_pfile( fread_number( fp ) );
      titles->list[i]->flags[0] = fread_number( fp );
      titles->list[i]->flags[1] = fread_number( fp );
      }

    for( i = 0; i < length; i++ ) 
      if( ( pfile = get_pfile( fread_number( fp ) ) ) != NULL ) 
        add_member( clan, pfile );

    fclose( fp );
    load_notes( clan );
    }

  closedir( dirp );
}


/*
 *   DISPLAY ROUTINES
 */


void display( char_data* ch, clan_data* clan )
{
  char           tmp  [ 3*MAX_STRING_LENGTH ];
  pfile_data*  pfile;
  int           i, j;

  page( ch, scroll_line[0] );
  page_centered( ch, name( clan ) );
  page( ch, scroll_line[0] );

  page( ch, "\n\r" );

  if( knows_members( ch, clan ) ) {
    page_title( ch, "Leaders" );
    for( i = 0; i < clan->titles.size; i++ ) {
      pfile = clan->titles.list[i]->pfile;
      page( ch, "%38s :: %s\n\r", clan->titles.list[i]->name,
        pfile == NULL ? "Noone" : pfile->name );
      }

    page( ch, "\n\r" );
    page_title( ch, "Members" );
    for( i = j = 0; i < clan->members.size; i++ ) 
      page( ch, "%18s%s",  clan->members.list[i]->name,
        (j++)%4 != 3 ? "" : "\n\r" );
    page( ch, "\n\r%s", j%4 != 0 ? "\n\r" : "" );
    }

  page_title( ch, "Charter" );

  convert_to_ansi( ch, clan->charter, tmp );
  page( ch, tmp );
}









