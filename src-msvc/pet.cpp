#include "define.h"
#include "struct.h"


/*
 *   NAME FUNCTIONS
 */


void do_name( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;

  argument = one_argument( argument, arg );

  if( ( victim = one_character( ch, arg, "name", ch->array ) ) == NULL )
    return;

  if( victim->species == NULL || victim->leader != ch
    || !is_set( &victim->status, STAT_PET ) ) {
    send( ch, "You can only name pets of your own.\r\n" );
    return;
    } 

  if( *argument == '\0' ) {
    send( ch, "What do you want to name them?\r\n" );
    return;
    }

  send( ch, "%s is now named '%s'.\r\n", victim, argument );

  free_string( victim->pet_name, MEM_MOBS );
  victim->pet_name = alloc_string( argument, MEM_MOBS );

  return;
}


/*
 *   PET ROUTINES
 */


void abandon( player_data* pc, char_data* pet )
{
  char* tmp;

  pet->shown = 1;
  send( pc, "You abandon %s.\r\n", pet );

  if( pc->familiar == pet )
    pc->familiar = NULL;

  remove_bit( &pet->status, STAT_PET );
  stop_follower( pet );
  free_string( pet->pet_name, MEM_MOBS );
  pet->pet_name = empty_string; 

  tmp = static_string( );
  sprintf( tmp, "Abandoned %s.", pet->Name( ) );
  player_log( pc, tmp );      
}


void do_pets( char_data* ch, char* argument )
{
  char_data*        pet;
  char_data*     victim;
  player_data*       pc;
  bool            found  = FALSE;
  int              i, j;

  if( is_mob( ch ) )
    return;

  pc = player( ch );

  if( exact_match( argument, "abandon" ) ) {
    j = atoi( argument );
    for( i = 0; i < ch->followers.size; i++ ) {
      if( is_pet( pet = ch->followers.list[i] ) && --j == 0 ) {
        abandon( pc, pet );
        return;
        }
      }
    send( ch, "You have no pet with that number.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    victim = ch;
    }
  else if( ch->shdata->level < LEVEL_BUILDER ) {
    send( ch, "Unknown syntax - See help pets.\r\n" );
    return;
    }
  else {
    if( ( victim = one_character( ch, argument, "pet", ch->array ) ) == NULL )
      return;

    if( victim->species != NULL ) {
      send( ch, "%s isn't a player and thus won't have pets.\r\n", victim );
      return;
      }     
    }
    
  for( i = j = 0; i < victim->followers.size; i++ ) {
    if( is_pet( pet = victim->followers.list[i] ) ) {
      if( !found ) {
        send_underlined( ch, "Num  Name                 Location\r\n" );
        found = TRUE;
        }
      send( ch, "%3d  %-20s %s\r\n", ++j, pet,
        pet->in_room == NULL ? "nowhere??" : pet->in_room->name );
      } 
    }

  if( !found ) {
    if( ch == victim )
      send( ch, "You have no pets.\r\n" );
    else
      send( ch, "%s has no pets.\r\n", victim );
    }
}


/*
 *   PET SUPPORT FUNCTIONS
 */


bool has_mount( char_data* ch )
{
  char_data*  mount;
  int            i;

  for( i = 0; i < ch->followers.size; i++ ) {
    mount = ch->followers.list[i];
    if( mount->species != NULL
      && is_set( &mount->species->act_flags, ACT_MOUNT ) ) {
      send( ch, "You are only able to acquire one mount at a time.\r\n" );
      return TRUE;
      }
    }

  return FALSE;
}



bool has_elemental( char_data* ch )
{
  char_data*  buddy;
  int            i;

  for( i = 0; i < ch->followers.size; i++ ) {
    buddy = ch->followers.list[i];
    if( buddy->species != NULL
      && is_set( &buddy->species->act_flags, ACT_ELEMENTAL ) ) {
      return TRUE;
      }
    }

  return FALSE;
}

int number_of_pets( char_data* ch )
{
  char_data*  pet;
  int         num;
  int           i;

  for( i = num = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( is_set( &pet->status, STAT_PET )
      && !is_set( &pet->status, STAT_TAMED )
      && !is_set( &pet->species->act_flags, ACT_MOUNT ) ) 
      num++;
    }

  return num;
}


int pet_levels( char_data* ch )
{
  char_data*    pet;
  int         level;
  int             i;

  for( i = level = 0; i < ch->followers.size; i++ ) {
    pet = ch->followers.list[i];
    if( is_set( &pet->status, STAT_TAMED ) )
      level += pet->shdata->level;
    }

  return level;
}






