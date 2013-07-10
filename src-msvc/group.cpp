#include "define.h"
#include "struct.h"


char_data*  list_followers   ( char_data* );


/*
 *   FOLLOWER ROUTINES
 */


void degroup( char_data* ch )
{
  remove_bit( &ch->status, STAT_IN_GROUP );

  for( int i = 0; i < ch->followers; i++ )
    degroup( ch->followers[i] );
}


void do_follow( char_data* ch, char* argument )
{
  char_data* victim;

  if( ( victim = one_character( ch, argument, "follow", ch->array ) ) == NULL )
    return;

  if( is_set( &ch->status, STAT_PET ) ) {
    interpret( ch, "sulk" );
    interpret( ch, "shake" );
    return;
    }

  if( victim == ch ) {
    if( ch->leader == NULL ) {
      send( ch, "You already follow yourself.\r\n" );
      return;
      }
    stop_follower( ch );
    return;
    }
 
  if( victim->species != NULL ) {
    send( ch, "You can only follow players.\r\n" );
    return;
    }

  if( !associate( ch, victim ) )
    return;

  if( ch->leader != NULL )
    stop_follower( ch );

  add_follower( ch, victim );
}


void add_follower( char_data* ch, char_data* victim, bool msg )
{
  char_data* leader;

  if( ch->leader != NULL ) {
    bug( "Add_follower: non-null leader." );
    return;
    }

  for( leader = victim->leader; leader != NULL; leader = leader->leader )
    if( leader == ch ) {
      send( ch, "No loops in follow allowed.\r\n" );
      return;
      }

  ch->leader         = victim;
  victim->followers += ch;

  degroup( ch );

  if( msg ) {
    if( ch->Seen( victim ) )
      send( victim, "%s now follows you.\r\n", ch );
    send( ch, "You now follow %s.\r\n", victim->Seen_Name( ch ) );
    send_seen( ch, "%s starts following %s.\r\n", ch, victim );
    }
}


void stop_follower( char_data* ch )
{
  char              tmp  [ TWO_LINES ];
  player_data*   pc;
 
  if( ch->leader == NULL ) {
    bug( "Stop_follower: null leader." );
    bug( "-- Ch = %s", ch->descr->name );
    return;
    }

  if( is_set( &ch->status, STAT_PET ) ) {
    remove_bit( &ch->status, STAT_PET );
    remove_bit( &ch->status, STAT_TAMED );
    remove_bit( &ch->status, STAT_FAMILIAR );
    pc = player( ch->leader );
    if( pc->familiar == ch ) 
      pc->familiar = NULL;
    sprintf( tmp, "%s [PET] stopped following.", ch->descr->name );
    player_log( pc, tmp );
    }

  if( ch->Seen( ch->leader ) )
    send( ch->leader, "%s stops following you.\r\n", ch );

  send( ch, "You stop following %s.\r\n", ch->leader );

  remove_bit( &ch->status, STAT_IN_GROUP );
  ch->leader->followers -= ch;

  ch->leader = NULL;
}


/*
 *   GROUP ROUTINES
 */


void add_group( char_data* ch, char_data* victim )
{
  if( victim == ch ) {
    send( ch, "You add yourself to your group.\r\n" );
    send_seen( ch, "%s joins %s own group.\r\n",
      ch, ch->His_Her( ) );
    set_bit( &ch->status, STAT_IN_GROUP ); 
    return;
    }

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    send( ch, "You need to group yourself first.\r\n" );
    return;
    }

  send( victim, "You join %s's group.\r\n", ch );
  send( ch, "%s joins your group.\r\n", victim );
  send_seen( victim, "%s joins %s's group.\r\n", victim, ch );
  set_bit( &victim->status, STAT_IN_GROUP ); 
}


void group_all( char_data* ch )
{
  char_data*   rch;
  bool       found  = FALSE;

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    add_group( ch, ch );
    found = TRUE;
    }

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch->leader == ch && !is_set( &rch->status, STAT_IN_GROUP )
      && rch->Seen( ch ) ) {
      add_group( ch, rch );
      found = TRUE;
      }

  if( !found ) 
    send( ch, "You and anyone following you are already grouped.\r\n" );
}   


void display_group( char_data* ch )
{
  char               tmp  [ TWO_LINES ];
  char              name  [ TWO_LINES ];
  char                hp  [ 15 ];
  char              move  [ 15 ];
  char            energy  [ 15 ];
  char_data*         gch;
  char_data*      leader;
  char_array*      array;
  char_array   incognito;
  bool             found  = FALSE;
  int i;

  if( !is_set( &ch->status, STAT_IN_GROUP ) ) {
    send( ch, "You aren't in any group.\r\n" );
    return;
    }

  for( leader = ch; leader->leader != NULL; leader = leader->leader );

  send( ch, "Leader: %s\r\n", leader );
  send_underlined( ch,
    "                                      Hits    Energy     Moves\
          Exp\r\n" );

  array = followers( leader );

  for( i = 0; i < *array; i++ ) {
    gch = array->list[i];

    if( !is_set( &gch->status, STAT_IN_GROUP ) )
      continue; 

    if( gch != ch && gch->pcdata != NULL
      && is_set( gch->pcdata->pfile->flags, PLR_GROUP_INCOG ) ) {
      incognito += gch;
      continue;
      }

    strcpy( name, gch->Seen_Name( ch ) );
    truncate( name, 17 );

    sprintf( hp,     "%d/%d", gch->hit,  gch->max_hit );
    sprintf( energy, "%d/%d", gch->mana, gch->max_mana );
    sprintf( move,   "%d/%d", gch->move, gch->max_move );

    sprintf( tmp,
      "[ %2d %3s %3s ] %-17s %9s %9s %9s %12d\r\n",
      gch->shdata->level, gch->species != NULL
      ? "Mob" : clss_table[ gch->pcdata->clss ].abbrev,
      race_table[ gch->shdata->race ].abbrev,
      name, hp, energy, move,
      gch->pcdata == NULL ? 0 : exp_for_level( gch )-gch->exp );
    tmp[15] = toupper( tmp[15] );
    send( ch, tmp );
    }

  delete array;

  for( i = 0; i < incognito; i++ ) {
    if( !found ) {
      found = TRUE;
      send( ch, "%46s\r\n", "-*-" );
      }
    send( ch, "[ Incognito  ] %-29s %s\r\n",
      incognito[i], condition_word( incognito[i] ) );
    }
}


void do_group( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    display_group( ch );
    return;
    }

  if( ch->leader != NULL ) {
    send( ch, "But you are following someone else!\r\n" );
    return;
    }

  if( !strcasecmp( argument, "all" ) ) {
    group_all( ch );
    return;
    }

  if( ( victim = one_character( ch, argument, "group",
    ch->array, (thing_array*) &ch->followers ) ) == NULL )
    return;

  if( victim->leader != ch && ch != victim ) {
    send( ch, "%s isn't following you.\r\n", victim );
    return;
    }

  if( is_set( &victim->status, STAT_IN_GROUP ) ) {
    if( victim != ch ) {
      send( ch, "You remove %s from your group.\r\n", victim );
      send( victim, "%s removes you from %s group.\r\n",
        ch, ch->His_Her( ) ); 
      send( *ch->array, "%s removes %s from %s group.\r\n",
        ch, victim, ch->His_Her( ) );
      if( !is_set( &victim->status, STAT_PET ) )
        stop_follower( victim );
      }
    else {
      send( "You remove yourself from your group.\r\n", ch );
      send_seen( ch, "%s removes %sself from %s group.\r\n",
        ch, ch->Him_Her( ), ch->His_Her( ) );
      }
    remove_bit( &victim->status, STAT_IN_GROUP );
    return;
    }

  add_group( ch, victim );  
}


/*
 *   UTILITY ROUTINES
 */


void add_followers( char_data* ch, char_array& array,
  content_array* where )
{
  if( where != NULL && ch->array != where )
    return;

  array += ch;

  for( int i = 0; i < ch->followers; i++ ) 
    add_followers( ch->followers[i], array, where );
}


char_array* followers( char_data* leader, content_array* where )
{
  char_array* array = new char_array;

  add_followers( leader, *array, where );

  return array;
}


int min_group_move( char_data* ch )
{
  char_data*   rch;
  int         move  = ch->move;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && is_same_group( ch, rch ) && rch != ch )
      move = min( move, rch->move );

  return move;
}


bool is_same_group( char_data* ach, char_data* bch )
{
  if( ach == NULL || bch == NULL )
    return FALSE;

  if( ach == bch )
    return TRUE;

  if( !is_set( &ach->status, STAT_IN_GROUP )
    || !is_set( &bch->status, STAT_IN_GROUP ) )
    return FALSE;

  for( ; ach->leader != NULL; ach = ach->leader );
  for( ; bch->leader != NULL; bch = bch->leader );

  return( ach == bch );
}


/*
 *   ORDER COMMAND
 */


void do_order( char_data* ch, char* argument )
{
  char          arg  [ MAX_INPUT_LENGTH ];
  char_data*    rch;
  thing_array  list;
  room_data*   room  = ch->in_room;
  bool        found  = FALSE;

  if( !two_argument( argument, "to", arg ) ) {
    argument = one_argument( argument, arg );
    if( *argument == '\0' ) {
      send( ch, "Syntax: Order <pet|all> [to] <command>\r\n" );
      return;
      }
    }

  if( strcasecmp( arg, "all" ) ) {
    if( ( rch = one_character( ch, arg, "order", ch->array ) ) == NULL )
      return;

    if( rch == ch ) {
      send( ch, "Ordering yourself makes no sense.\r\n" );
      return;
      }

    send( ch, "You order %s to '%s'.\r\n", rch, argument );
    if( rch->position > POS_SLEEPING )
      send( rch, "%s orders you to '%s'.\r\n", ch, argument );
    send( *ch->array, "%s orders %s to '%s'.\r\n",
      ch, rch, argument );

    if( !is_set( &rch->status, STAT_PET ) || rch->leader != ch ) {
      send( ch, "%s ignores you.\r\n", rch );
      return;
      }
    set_bit( &rch->status, STAT_ORDERED );
    interpret( rch, argument );
    remove_bit( &rch->status, STAT_ORDERED );
    return;
    }

  send( *ch->array, "%s orders all %s followers to '%s'.\r\n",
    ch, ch->His_Her( ), argument );

  copy( list, *ch->array );

  for( int i = 0; i < list; i++ ) {
    if( ( rch = character( list[i] ) ) == NULL    
      || rch->array != &room->contents
      || !is_set( &rch->status, STAT_PET )
      || rch->leader != ch )
      continue;

    if( !found ) {
      fsend( ch, "You order all your followers to '%s'.\r\n",
        argument );
      found = TRUE;
      }

    set_bit( &rch->status, STAT_ORDERED );
    interpret( rch, argument );
    remove_bit( &rch->status, STAT_ORDERED );
    }
 
  if( !found ) 
    send( ch, "You have no followers here.\r\n"  );
}



