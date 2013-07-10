#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "define.h"
#include "struct.h"
#include "dirent.h"


obj_clss_data*   obj_index_list  [ MAX_OBJ_INDEX ];
quest_data*          quest_list  [ MAX_QUEST ];
species_data*      species_list  [ MAX_SPECIES ];
char*                  cmd_help  [ MAX_COMMAND ];

mob_array        mob_list;
player_array  player_list;
obj_array        obj_list;
int            boot_stage;
int         player_number  = 0;


/*
 *   LOCAL ROUTINES
 */


void  load_helps     ( void );
void  load_mobiles   ( void );
void  load_players   ( void );
void  load_trainers  ( void );
void  load_quests    ( void );

void  check_player   ( char_data* );
void  fix_exits      ( void );
void  set_relations  ( void );
void  fix_species    ( void );


/* 
 *   EXTERNAL ROUTINES
 */    


void  read_mail      ( pfile_data* ); 


void boot_db( void )
{
  FILE*      fp;

  boot_stage = 0;

  weather.hour     = 0;
  weather.minute   = 0;
  weather.sunlight = sunlight( 0 );
  weather.month    = number_range( 0,11 );
  weather.day      = number_range( 1,days_in_month[weather.month] );
  weather.year     = 0;

  load_objects( );
  load_mobiles( );
  load_tables( );
  load_lists( );
  load_rtables( );

  echo( "Loading Areas ...\n\r" ); 

  fp = open_file( AREA_LIST, "r", TRUE );
  for( ; ; ) {
    char* word = fread_word( fp );
    if( *word == '$' )
      break;
    load_area( word );
    }
  fclose( fp );

  fix_exits( );
  load_helps( );
  load_notes( );
  load_shops( );
  load_trainers( );
  load_quests( );
  load_banned( );
  load_badname( );
  load_room_items( );

  boot_stage = 1;

  load_accounts( );
  load_players( );
  load_clans( );

  boot_stage = 2;

  fix_species( );
  area_update( );
}



/*
 *   EXITS
 */


void fix_exits( void )
{
  area_data*  area;
  room_data*  room;
  exit_data*  exit;

  printf( "Fixing exits...\n\r" );

  for( area = area_list; area != NULL; area = area->next )
    for( room = area->room_first; room != NULL; room = room->next ) {
      set_bit( &room->room_flags, RFLAG_STATUS0 );
      set_bit( &room->room_flags, RFLAG_STATUS1 );
      set_bit( &room->room_flags, RFLAG_STATUS2 );

      for( int i = room->exits-1; i >= 0; i-- ) {
        exit = room->exits[i];
        if( ( exit->to_room = get_room_index(
          (int) exit->to_room ) ) == NULL ) {
          roach(
            "Fix_Exits: Deleting exit from %d to non-existent %d.",
            room->vnum, (int) exit->to_room );
          room->exits -= exit;
          delete exit;
          }
        }
      }

  return;
}


/*
 *   PLAYERS
 */


void load_players( void )
{
  DIR*            dirp;
  struct dirent*    dp;
  player_data*      ch;
  link_data*      link;

  echo( "Loading Players ...\n\r" );
  
  dirp            = opendir( PLAYER_DIR );
  link            = new link_data;
  link->connected = CON_PLAYING;

  for( dp = readdir( dirp ); dp != NULL; dp = readdir( dirp ) ) {
    if( !strcmp( dp->d_name, "." ) || !strcmp( dp->d_name, ".." ) )
      continue;

    if( !load_char( link, dp->d_name, PLAYER_DIR ) ) {
      bug( "Load_players: error reading player file. (%s)", dp->d_name );
      continue;
      }

    ch = link->player;

    if( current_time > ch->pcdata->pfile->last_on
      +2*weeks( ch->shdata->level ) && ch->shdata->level < 9 ) {
      fprintf( stdout, "  -Deleting %s.\n", dp->d_name );
      player_log( ch, "Deleted by file sweeper." );
      purge( ch );
      continue;
      }

    player_number++;

    read_mail( ch->pcdata->pfile ); 
    check_player( ch );

    ch->Extract( );
    delete_list( extracted );
    }

  closedir( dirp );

  return;
}


void check_player( char_data* ch )
{
  int       j;
  int  points;

  if( ch->shdata->level >= LEVEL_APPRENTICE )
    return; 

  if( ch->shdata->race >= MAX_PLYR_RACE ) {
    printf( "  -%s is a non-player race.\n", ch->descr->name );
    return;
    }

  int* stat [] = { &ch->shdata->strength, &ch->shdata->intelligence,
    &ch->shdata->wisdom, &ch->shdata->dexterity, &ch->shdata->constitution };
  int* bonus = plyr_race_table[ch->shdata->race].stat_bonus;

  for( j = 0, points = 500; j < 5; j++ )
    points -= (*stat[j]-bonus[j])*(*stat[j]-bonus[j]+1)/2;

  if( points < 0 ) 
    printf( "  -%s has impossible stats.\n", ch->descr->name );

  return;
}


