#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


void* code_doing_quest( void** argument )
{
  char_data*     ch  = (char_data*) argument[0];
  int         quest  = (int)        argument[1];

  if( quest < 0 || quest >= MAX_QUEST ) {
    code_bug( "Doing_Quest: impossible quest number." );
    return NULL;
    }

  if( ch == NULL ) {
    code_bug( "Doing_Quest: Null character." );
    return NULL;
    } 

  return (void*) ( ch->pcdata != NULL
    && ch->pcdata->quest_flags[quest] == 1 );
}


void* code_done_quest( void** argument )
{
  char_data*     ch  = (char_data*) argument[0];
  int         quest  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Code_Done_Quest: NULL character." );
    return NULL;
    }

  return (void*) ( ch->pcdata != NULL
    && ch->pcdata->quest_flags[quest] == -1 );
}


void* code_has_quest( void** argument )
{
  char_data*     ch  = (char_data*) argument[0];
  int         quest  = (int)        argument[1];

  if( quest < 0 || quest >= MAX_QUEST ) {
    code_bug( "Has_Quest: impossible quest number." );
    return NULL;
    }

  if( ch == NULL ) {
    code_bug( "Code_Has_Quest: Null character." );
    return NULL;
    }

  return (void*) ( ch->pcdata != NULL
    && ch->pcdata->quest_flags[quest] != 0 );
}


void* code_assign_quest( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  int        quest  = (int)        argument[1];

  if( quest < 0 || quest >= MAX_QUEST ) {
    code_bug( "Assign_Quest: impossible quest number." );
    return NULL;
    }

  if( ch != NULL && ch->species == NULL
    && ch->pcdata->quest_flags[quest] == 0 ) {
    ch->pcdata->quest_flags[quest] = 1;
    send( ch, "\n\r-*- You have been assigned a quest. -*-\n\r" ); 
    }

  return NULL;
}


void* code_update_quest( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  int              i  = (int)        argument[1];

  quest_data*  quest;  

  if( i < 0 || i >= MAX_QUEST ) {
    code_bug( "Update_Quest: impossible quest number." );
    return NULL;
    }

  if( ch == NULL || ch->pcdata == NULL )
    return NULL;

  quest = get_quest_index( i );

  if( ch->pcdata->quest_flags[i] == 1 ) {
    ch->pcdata->quest_flags[i] = -1;
    ch->pcdata->quest_pts += quest->points;
    if( quest->points != 0 ) 
      send( ch, "-*- You gain %d quest points! -*-\n\r", quest->points );
    }

  return NULL;
}






