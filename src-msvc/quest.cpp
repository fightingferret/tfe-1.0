#include "define.h"
#include "struct.h"


quest_data* get_quest_index( int i )
{
  if( i < 0 || i >= MAX_QUEST ) 
    return NULL;

  return quest_list[i];
}


/*
 *   LOCATING QUESTS
 */


void do_qwhere( char_data* ch, char *argument )
{
  action_data*     action;
  area_data*         area;
  mprog_data*       mprog;
  obj_clss_data*      obj;
  oprog_data*       oprog;
  room_data*         room;
  species_data*   species;
  int                i, j;
  int               index;
  bool              found  = FALSE;

  index = atoi( argument );

  if( get_quest_index( index ) == NULL ) {
    send( ch, "No quest found with that index.\r\n" );
    return;
    }

  for( area = area_list; area != NULL; area = area->next )
    for( room = area->room_first; room != NULL; room = room->next )
      for( j = 1, action = room->action; action != NULL;
        j++, action = action->next )
        if( search_quest( action->binary, index ) ) {
          found = TRUE;
          page( ch, "  Refered to in action #%d in room #%d\r\n",
            j, room->vnum );
          }

  for( i = 0; i < MAX_SPECIES; i++ ) {
    if( ( species = species_list[i] ) == NULL ) 
      continue;
    for( j = 1, mprog = species->mprog; mprog != NULL;
      j++, mprog = mprog->next )
      if( search_quest( mprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Refered to in mprog #%d on mob #%d\r\n", j, i );
        }
    }

  for( i = 0; i < MAX_OBJ_INDEX; i++ ) {
    if( ( obj = obj_index_list[i] ) == NULL ) 
      continue; 
    for( j = 1, oprog = obj->oprog; oprog != NULL; j++, oprog = oprog->next )
      if( search_quest( oprog->binary, index ) ) {
        found = TRUE;
        page( ch, "  Refered to in oprog #%d on obj #%d\r\n", j, i );
        }
    }

  if( !found )
    send( ch, "No references to that quest were found.\r\n" );

  return;
}


/*
 *   EDITTING OF QUESTS
 */


void do_qedit( char_data* ch, char *argument )
{
  quest_data*    quest;
  wizard_data*  wizard;
  bool           found = FALSE;  
  int                i;

  wizard = (wizard_data*) ch;

  if( *argument == '\0' ) {
    for( i = 0; i < MAX_QUEST; i++ ) {
      if( ( quest = quest_list[i] ) == NULL )
        continue;
      found = TRUE;
      page( ch, "[%3d] %-50s\n", i, quest->message );
      }
    if( !found ) 
      send( ch, "No quests found.\r\n" );
    return;
    }
 
  if( matches( argument, "delete" ) ) {
    if( *argument == '\0' ) {
      send( ch, "What quest do you want to delete?\r\n" );
      return;
      }
   
    if( ( i = atoi( argument ) ) < 0 || i >= MAX_QUEST
      || ( quest = quest_list[i] ) == NULL ) {
      send( ch, "Quest not found to remove.\r\n" );
      return;
      }

    extract( wizard, offset( &wizard->quest_edit, wizard ), "quest" );
    quest_list[i] = NULL;
    free_string( (char *) quest->message, MEM_QUEST );
    send( ch, "Quest removed.\r\n" );
    delete quest;

    return; 
    }

  if( matches( argument, "new" ) ) {
    for( i = 0; quest_list[i] != NULL; i++ )
      if( i == MAX_QUEST ) {
        send( ch, "Quest space is full.\r\n" );
        return;
        }

    quest              = new quest_data;
    quest->message     = empty_string;
    quest->vnum        = i;  
    quest->points      = 0;
    quest_list[i]      = quest;
    wizard->quest_edit = quest;

    send( ch, "Quest created and assigned #%d.\r\n", i );
    return;
    }

  if( ( i = atoi( argument ) ) < 0 || i >= MAX_QUEST ) {
    send( ch, "Quest number is out of range.\r\n" );
    return;
    }

  if( quest_list[i] == NULL ) {
    send( ch, "There is no quest by that number.\r\n" );
    return;
    }

  wizard->quest_edit = quest_list[i];
  send( ch, "Qedit now operates on specified quest.\r\n" );

  return;
}  


void do_qset( char_data* ch, char *argument )
{
  quest_data*     quest;
  wizard_data*   wizard;
 
  if( *argument == '\0' ) {
    do_qstat( ch, "" );
    return;
    }

  wizard = (wizard_data*) ch;

  if( ( quest = wizard->quest_edit ) == NULL ) {
    send( ch, "You aren't editing any quest.\r\n" );
    return;
    }

  class int_field int_list[] = {
    { "points",        0,  25,  &quest->points },
    { "",              0,   0,  NULL },   
    };

  if( process( int_list, ch, "quest", argument ) )
    return;
  
  class string_field string_list[] = {
    { "message",    MEM_QUEST,  &quest->message,  NULL },
    { "",           0,          NULL,             NULL }   
    };

  if( process( string_list, ch, "quest", argument ) )
    return;

  send( ch, "Unknown parameter.\r\n" );
  return;
}


void do_qstat( char_data* ch, char* )
{
  wizard_data*  wizard;
  quest_data*    quest;

  wizard = (wizard_data*) ch;

  if( ( quest = wizard->quest_edit ) == NULL ) {
    send( ch, "You aren't editing any quest.\r\n" );
    return;
    }

  send( ch, "Message: %s\r\n", quest->message );
  send( ch, "Points: %d\r\n", quest->points );

  return;
}


/*
 *   PLAYER COMMANDS
 */


void do_quests( char_data* ch, char* argument )
{
  quest_data*       quest;
  bool              found  = FALSE; 
  int                   i;
  int               flags;
  int               value;
  const char*   title_msg;
  const char*    none_msg; 

  if( not_player( ch ) ) 
    return;

  if( !get_flags( ch, argument, &flags, "d", "Quests" ) )
    return;

  if( is_set( &flags, 0 ) ) {
    title_msg = "Completed Quests";
    none_msg  = "not completed any";
    value     = -1;
    }
  else {
    title_msg = "Assigned Quests";
    none_msg  = "no unfinished, but completed";
    value     = 1;
    } 

  for( i = 0; i < MAX_QUEST; i++ ) 
    if( ( quest = quest_list[i] ) != NULL 
      && ch->pcdata->quest_flags[i] == value ) {
      if( !found ) {
        page_title( ch, title_msg );
        found = TRUE;
        }
      page_centered( ch, quest->message );   
      }

  if( !found ) 
    send( ch, "You have %s quests.\r\n", none_msg );

  return;
}


/*
 *   EDITTING QUESTS ON PLAYERS
 */


void do_qremove( char_data* ch, char* )
{
  int   i;

  for( i = 0; i < MAX_QUEST; i++ ) 
    ch->pcdata->quest_flags[i] = 0;

  ch->pcdata->quest_pts = 0;

  send( ch, "All quest records erased for your character.\r\n" );
  return;   
}


void do_cflag( char_data* ch, char *argument )
{
  player_data*  victim;
  wizard_data*  wizard  = (wizard_data*) ch;
  int                i;

  victim =  ( wizard->player_edit == NULL 
    ? (player_data*) ch : wizard->player_edit );

  if( *argument == '\0' ) {
    page_title( ch, "Cflags of %s", victim->descr->name );
    for( i = 0; i < 32*MAX_CFLAG; i++ ) {
      page( ch, "%5d (%1c)", i,
        is_set( victim->pcdata->cflags, i ) ? '*' : ' ' );
      if( i%8 == 7 )
        page( ch, "\r\n" );
      }
    if( i%8 != 0 )
      page( ch, "\r\n" );
    return;
    }

  i = atoi( argument );

  if( i < 0 || i >= 32*MAX_CFLAG ) {
    send( ch, "Cflag number out of range.\r\n" );
    return;
    }

  switch_bit( ch->pcdata->cflags, i );
 
  send( ch, "Cflag %d on %s set to %s.\r\n", i,
    victim == ch ? "yourself" : victim->descr->name,
    is_set( ch->pcdata->cflags, i ) ? "true" : "false" );

  return;
} 


/*
 *   FILE ROUTINES
 */

   
void load_quests( void )
{
  quest_data*  quest;
  FILE*           fp;
  char        letter;
  int           vnum;

  printf( "Loading quests...\r\n" );

  if( ( fp = fopen( QUEST_FILE, "r" ) ) == NULL ) 
    panic( "Load_quests: file not found" );

  if( strcmp( fread_word( fp ), "#QUESTS" ) ) 
    panic( "Load_quests: header not found" );
 
  for( ; ; ) {
    if( ( letter = fread_letter( fp ) ) != '#' ) 
      panic( "Load_quests: # not found." );
   
    if( ( vnum = fread_number( fp ) ) == -1 )
      break;

    quest          = new quest_data;
    quest->vnum    = vnum;      
    quest->message = fread_string( fp, MEM_QUEST );
    quest->points  = fread_number( fp );

    fread_number( fp );

    quest_list[vnum] = quest;
    }

  fclose( fp );
  return;
}


void save_quests( )
{
  quest_data*  quest;
  FILE*           fp;
  int              i;

  if( ( fp = fopen( QUEST_FILE, "w" ) ) == NULL ) {
    bug( "Save_quest: fopen" );
    return;
    }

  fprintf( fp, "#QUESTS\n\n" );
 
  for( i = 0; i < MAX_QUEST; i++ ) 
    if( ( quest = quest_list[i] ) != NULL ) {
      fprintf( fp, "#%d\n", quest->vnum );
      fprintf( fp, "%s~\n", quest->message );
      fprintf( fp, "%d 0\n", quest->points  );
      }

  fprintf( fp, "#-1\n" );
  fclose( fp );

  return;
}




