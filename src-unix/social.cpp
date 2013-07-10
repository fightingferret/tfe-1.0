#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/* 
 *   MAIN PRINT ROUTINE
 */


void act_social( char_data* to, const char* msg, char_data* ch,
  char_data* victim, obj_data* obj = NULL )
{
  bool heard  = ( *msg == '!' );

  if( heard )
    msg++;

  if( heard || ch->Seen( to )
    || ( victim != NULL && victim->Seen( to ) ) )
    act( to, msg, ch, victim, obj );
}


void act_social( char_data* to, const char* msg, char_data* ch,
  const char* word )
{
  bool heard  = ( *msg == '!' );

  if( heard )
    msg++;

  if( heard || ch->Seen( to ) )
    act( to, msg, ch, NULL, word );
}


void act_social( char_data* to, const char* msg, char_data* ch,
  obj_data* obj )
{
  bool heard  = ( *msg == '!' );

  if( heard )
    msg++;

  if( heard || ch->Seen( to ) )
    act( to, msg, ch, obj );
}


/*
 *   SOCIAL ACTING ON CHARACTER
 */


void social_no_arg( char_data* ch, social_type* soc1, social_type* soc2 )
{
  char        tmp  [ THREE_LINES ];
  char_data*  rch;

  if( soc1->char_no_arg == empty_string )
    soc1 = soc2;

  convert_to_ansi( ch, soc1->char_no_arg, tmp );
  send( ch, "%s\n\r", tmp );

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING )
      act_social( rch, soc1->others_no_arg, ch,
        (char_data*) NULL );
}


void social_auto( char_data* ch, social_type* soc1, social_type* soc2 )
{
  char        tmp  [ THREE_LINES ];
  char_data*  rch;

  if( soc1->char_auto == empty_string )
    soc1 = soc2;

  convert_to_ansi( ch, soc1->char_auto, tmp );
  send( ch, "%s\n\r", tmp );

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING )
      act_social( rch, soc1->others_auto, ch, (char_data*) NULL );
}


void social_victim( char_data* ch, social_type* soc1,
  social_type* soc2, char_data* victim )
{
  char_data* rch;
 
  if( soc1->char_found == empty_string ) {
    if( soc2->char_found == empty_string ) {
      send( ch, "%s and %s do nothing together.\n\r",
        soc1->name, victim );
      return;
      }
    soc1 = soc2;
    }

  act( ch, soc1->char_found, ch, victim );
  act_social( victim, victim->position == POS_SLEEPING ?
    soc1->vict_sleep : soc1->vict_found, ch, victim );

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch != victim && rch->position > POS_SLEEPING )
      act_social( rch, soc1->others_found, ch, victim );
}


/*
 *   DIRECTION AND SOCIAL
 */


void social_dir( char_data* ch, social_type* soc1, social_type* soc2,
  int dir )
{
  char_data* rch;

  if( soc1->char_found == empty_string ) {
    soc1 = soc2;
    if( soc1->dir_self == empty_string ) {
      send( ch, "%s and a direction does nothing.\n\r", 
        soc1->name );
      return;
      }
    }

  act( ch, soc1->dir_self, ch, NULL, dir_table[dir].name );

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING )
      act_social( rch, soc1->dir_others, ch,
        dir_table[dir].name );
}


/*
 *   OBJECT AND SOCIAL
 */


void social_obj( char_data* ch, social_type* soc1, social_type* soc2,
  obj_data* obj )
{
  char_data* rch;

  if( soc1->obj_self == empty_string ) {
    if( soc2->obj_self == empty_string ) {
      send( ch, "%s and an object does nothing.\n\r", soc2->name );
      return;
      }
    soc1 = soc2;
    }

  act( ch, soc1->obj_self, obj );

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING )
      act_social( rch, soc1->obj_others, ch, obj );
}


/*
 *   OBJECT, VICTIM AND SOCIAL
 */


void social_self_obj( char_data* ch, social_type* soc1, social_type* soc2,
  obj_data* obj )
{
  char_data* rch;

  if( soc1->self_obj_self == empty_string ) {
    if( soc2->self_obj_self == empty_string ) {
      fsend( ch, "Mixing %s and %s does nothing interesting.\n\r",
        soc2->name, obj );
      return;
      }
    soc1 = soc2;
    }
  act( ch, soc1->self_obj_self, ch, obj );
  
  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch->position > POS_SLEEPING )
      act_social( rch, soc1->self_obj_others, ch, obj );
}


void social_ch_obj( char_data* ch, social_type* soc1, social_type* soc2,
  char_data* victim, obj_data* obj )
{
  char_data* rch;

  if( ch == victim ) {
    social_self_obj( ch, soc1, soc2, obj );
    return;
    }

  if( soc1->ch_obj_self == empty_string ) {
    if( soc2->ch_obj_self == empty_string ) {
      fsend( ch, "Mixing %s, %s and %s does nothing interesting.\n\r",
        soc2->name, victim, obj );
      return;
      }
    soc1 = soc2;
    }

  act( ch, soc1->ch_obj_self, ch, victim, obj );

  if( victim->position >= POS_SLEEPING ) {
    act( victim, victim->position == POS_SLEEPING
      ? soc1->ch_obj_sleep : soc1->ch_obj_victim,
      ch, victim, obj );
    }

  for( int i = 0; i < *ch->array; i++ )
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && rch != victim && rch->position > POS_SLEEPING )
      act_social( rch, soc1->ch_obj_others, ch, victim, obj );
}


social_type* find_social( social_type* table, int max, char* command )
{
  int cmd;

  if( max == 0 )
    return NULL;

  if( ( cmd = search( table, max, command ) ) < 0 )
    cmd = -cmd-1;

  if( cmd == max
    || strncasecmp( command, table[cmd].name, strlen( command ) ) )
    return NULL;

  return &table[cmd];
}


/*
 *   MAIN HANDLER
 */


bool check_social( char_data* ch, char* command, char* argument )
{
  char             arg  [ MAX_INPUT_LENGTH ];
  social_type*    soc1  = NULL;
  social_type*    soc2  = NULL;
  char_data*    victim;
  obj_data*        obj;
//  int              dir;
  thing_data*       t1;
  thing_data*       t2;

  if( ch->shdata->race < MAX_PLYR_RACE )
    soc1 = find_social( social_table[ ch->shdata->race+1 ],
      table_max[ ch->shdata->race+1 ], command );

  soc2 = find_social( social_table[0], 
    table_max[TABLE_SOC_DEFAULT], command );

  if( soc1 == NULL && ( soc1 = soc2 ) == NULL )
    return FALSE;

  if( soc2 == NULL )
    soc2 = soc1;

  if( ch->pcdata != NULL 
    && is_set( ch->pcdata->pfile->flags, PLR_NO_EMOTE ) ) {
    send( ch, "You are anti-social!\n\r" );
    return TRUE;
    }

  if( ch->position < soc1->position ) {
    switch( ch->position ) {
      case POS_DEAD:
        send( ch, "You have died and are unable to move.\n\r" );
        return TRUE;

      case POS_INCAP:
      case POS_MORTAL:
        send( ch, "The bright white light has you distracted.\n\r" );
        return TRUE;

      case POS_STUNNED:
        send( ch, "You are stunned and cannot move.\n\r" );
        return TRUE;
 
      case POS_MEDITATING:
        send( ch, "You are deep in meditation.\n\r" );
        return TRUE;

      case POS_SLEEPING:
        send( "You cannot do that while sleeping.\n\r", ch );
        return TRUE;
 
      case POS_RESTING:
        send( "You must be standing to do that.\n\r", ch );
        return TRUE;

      case POS_FIGHTING:
        send( "You are unable to do that while fighting.\n\r", ch );
        return TRUE;
      }
    }

  if( *argument == '\0' ) {
    social_no_arg( ch, soc1, soc2 );
    return TRUE;
    }

  argument = one_argument( argument, arg );

  if( ( t1 = one_thing( ch, arg, "social",
    &ch->contents, ch->array ) ) == NULL )
    return TRUE;

  obj    = object( t1 );
  victim = character( t1 );

  if( *argument == '\0' ) {
    if( obj != NULL )
      social_obj( ch, soc1, soc2, obj );
    else if( victim == ch )
      social_auto( ch, soc1, soc2 );
    else
      social_victim( ch, soc1, soc2, victim );
    return TRUE;
    }

  if( ( t2 = one_thing( ch, argument, "social",
    &ch->contents, ch->array ) ) == NULL )
    return TRUE;

  if( obj == NULL )
    obj = object( t2 );
  else 
    victim = character( t2 );

  if( obj != NULL && victim != NULL ) 
    social_ch_obj( ch, soc1, soc2, victim, obj );
  else 
    fsend( ch, "Mixing %s, %s, and %s does nothing interesting.",
      soc1->name, t1, t2 );
 
  return TRUE;
}


/* 
 *   DISPLAY SOCIAL LIST
 */


void do_socials( char_data* ch, char* argument )
{
  int                 i;
  int             table  = TABLE_SOC_DEFAULT;

  if( *argument == '\0' ) {
    page_title( ch, "Default Socials" );
    }
  else {
    for( table = 0; ; table++ ) {
      if( table == MAX_PLYR_RACE ) {
        send( ch, "Syntax: Social [race]\n\r" );
        return;
        }
      if( matches( argument, plyr_race_table[table].name ) )
        break;
      }
    page_title( ch, "%s Socials", plyr_race_table[table].name );
    table++;
    }

  for( i = 0; i < table_max[table]; ) {
    page( ch, "%16s ", social_table[table][i].name );
    if( ++i%4 == 0 )
      page( ch, "\n\r" );
    }
  if( i%4 != 0 )
    page( ch, "\n\r" );
}


