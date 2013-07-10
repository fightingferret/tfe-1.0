#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


trainer_data*  trainer_list;


/*
 *  SET TRAINER 
 */


void set_trainer( mob_data* mob, room_data* room )
{
  trainer_data*   trainer;
  
  for( trainer = trainer_list; trainer != NULL; trainer = trainer->next ) 
    if( trainer->room == room && trainer->mob == NULL
      && trainer->trainer == mob->species->vnum ) {
      mob->pTrainer  = trainer;
      trainer->mob   = mob;
      break;
      }

  return;
}


/*
 *   TFIND ROUTINE
 */


void do_tfind( char_data* ch, char* argument )
{
  bool               first  = TRUE;
  bool               found;
  room_data*          room;
  species_data*    species;
  trainer_data*    trainer;
  int                    i;

  if( *argument == '\0' ) { 
    send( ch, "Syntax: tfind <skill>\n\r" );
    return;
    }

  for( i = 0; i < MAX_SKILL; i++ ) {
    if( !fmatches( argument, skill_table[i].name ) )
      continue;

    if( !first ) 
      page( ch, "\n\r" );

    page( ch, "%s:\n\r", skill_table[i].name );

    found = FALSE;
    first = FALSE;

    for( trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
      species = get_species( trainer->trainer );
      room    = trainer->room; 
      if( is_set( trainer->skill, i ) ) {
        page( ch, "  [%4d] %-30s [%5d] %s\n\r", trainer->trainer,
          species == NULL ? "noone?" : species->Name( ),
          trainer->room->vnum, room == NULL ? "nowhere?" : room->name );
        found = TRUE;
        }
      }
     
    if( !found )
      page( ch, "  no trainers found\n\r" );
    }

  if( first )
    send( ch, "No skill by that name found.\n\r" );

  return;      
}


/*
 *   TRAIN ROUTINE
 */


void do_train( char_data* ch, char* argument )
{
  char                arg  [ MAX_INPUT_LENGTH ];
  char_data*       victim;
  mob_data*           npc;
  trainer_data*   trainer;
  bool              found  = FALSE;
  int                   i;

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( ch, "Syntax: train [mob] <field> <value>\n\r" );
    return;
    }

  if( ( victim = one_character( ch, arg, "train", ch->array ) ) == NULL ) 
    return;

  if( ( npc = mob( victim ) ) == NULL ) {
    send( ch, "Players can't be trainers.\n\r" );
    return;
    }

  if( npc->reset == NULL ) {
    send( ch, "Only mobs with resets can be assigned as trainers.\n\r" );
    return;
    }

  trainer = npc->pTrainer;

  if( !strcasecmp( argument, "new" ) ) {
    if( trainer != NULL ) {
      send( ch, "Victim is already a trainer.\n\r" );
      return;
      }
    if( !is_listed( ch->in_room->reset, victim->reset ) ) {
      send( ch, "The mob must reset in the room you are in.\n\r" );
      return;
      }
    trainer          = new trainer_data;
    npc->pTrainer    = trainer;
    trainer->room    = ch->in_room;
    trainer->trainer = npc->species->vnum;
    append( trainer_list, trainer );
    send( ch, "%s set as a trainer.\n\r", victim );
    return;
    }

  if( trainer == NULL ) {
    send( ch, "%s is not a trainer.", victim );
    return;
    }

  if( !strcasecmp( argument, "delete" ) ) {
    remove( trainer_list, trainer );
    delete trainer;
    npc->pTrainer = NULL;
    send( ch, "%s is no longer a trainer.\n\r", victim );  
    return;
    }          

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_SKILL; i++ ) {
      if( is_set( trainer->skill, i ) ) {
        if( !found ) {
          found = TRUE;
          send( ch, "%s can train the following skills:\n\r",
            victim->Seen_Name( ch ) );
          } 
        send( ch, "  %s\n\r", skill_table[i].name );
        }
      }
    if( !found ) 
      send( ch, "%s trains no skills.\n\r", victim->Seen_Name( ch ) );
    return;
    }

  for( i = 0; i < MAX_SKILL; i++ ) {
    if( fmatches( argument, skill_table[i].name ) ) {
      switch_bit( trainer->skill, i );
      send( ch, "%s can %s train %s.\n\r",
        victim->descr->name, is_set( trainer->skill, i ) ?
        "now" : "no longer", skill_table[i].name );
      return;
      }
    }

  send( ch, "Unknown skill.\n\r" );

  return;
}    


/*
 *   DISK ROUTINES
 */


void load_trainers( void )
{
  FILE*                fp;
  trainer_data*   trainer;
  char*              word;
  char             letter;
  int                room;
  int                   i;

  fprintf( stderr, "Loading Trainers...\n\r" );

  if( MAX_TRAIN*32 < MAX_SKILL ) 
    panic( "Load_Trainers: Skill array insufficent." );

  fp = open_file( TRAINER_FILE, "r" );

  if( strcmp( fread_word( fp ), "#TRAINERS" ) ) 
    panic( "Load_trainers: header not found" );

  for( ; ; ) {
    if( ( room = fread_number( fp ) ) == -1 )
      break;

    trainer           = new trainer_data;
    trainer->trainer  = fread_number( fp );
    trainer->room     = get_room_index( room );

    if( trainer->room == NULL ) {
      roach( "Load_Trainers: Deleting as room %d is non-existent.", room );
      delete trainer;
      trainer = NULL;
      }
    else {
      append( trainer_list, trainer );
      }

    for( ; ; ) {
      letter = fread_letter( fp );
      ungetc( letter, fp );
      if( isdigit( letter ) || letter == '-' )
        break;
      if( strcmp( word = fread_word( fp ), "none" ) ) {
        for( i = 0; i < MAX_SKILL; i++ ) 
          if( !strcmp( word, skill_table[i].name ) ) {
            if( trainer != NULL )
              set_bit( trainer->skill, i );
            break;
	    }
        if( i == MAX_SKILL ) 
          panic( "Load_Trainers: Unknown Skill - %s", word );
        }
      }
    }

  fclose( fp );
  return;
}


void save_trainers( )
{
  trainer_data*  trainer;
  FILE*               fp;
  int                  i;

  if( ( fp = open_file( TRAINER_FILE, "w" ) ) == NULL ) 
    return;

  fprintf( fp, "#TRAINERS\n" );

  for( trainer = trainer_list; trainer != NULL; trainer = trainer->next ) {
    fprintf( fp, "%5d %5d  ", trainer->room->vnum, trainer->trainer );
    for( i = 0; i < MAX_SKILL; i++ )
      if( is_set( trainer->skill, i ) )
        fprintf( fp, " '%s'", skill_table[i].name );
    fprintf( fp, "\n" );
    }

  fprintf( fp, "-1\n\n#$\n\n" );
  fclose( fp );

  return;
}






