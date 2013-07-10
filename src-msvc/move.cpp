#include "define.h"
#include "struct.h"


/*
 *   CONSTANTS
 */


#define  MOVE_FLEE        -1
#define  MOVE_SLITHERS     0
#define  MOVE_SNEAK        1
#define  MOVE_WALK         2
#define  MOVE_FLY          3
#define  MOVE_SWIM         4
#define  MOVE_FLOAT        5
#define  MOVE_WADE         6


const char* leaving_verb [] = { "slithers", "sneaks", "leaves", "flies",
  "swims", "floats", "wades" }; 

const char* leaving_action [] = { "slither", "sneak", "leave", "fly",
  "swim", "float", "wade" };

const char* arriving_verb [] = { "slithers in", "sneaks in", "arrives",
  "flies in", "swims in", "floats in", "wades in" };


direction_type dir_table [] =
{ 
  { "north", 2, "the south",  "to the north"  },
  { "east",  3, "the west",   "to the east"   },
  { "south", 0, "the north",  "to the south"  },
  { "west",  1, "the east",   "to the west"   },
  { "up",    5, "below",      "above you"     },
  { "down",  4, "above",      "below you"     },
  { "extra", 6, "??",         "??"            }
};


char_data* leader = NULL;


/*
 *   LOCAL FUNCTIONS
 */


bool        passes_drunk      ( char_data* );
bool        trigger_entering  ( char_data*, room_data*, int );
bool        trigger_leaving   ( char_data*, room_data*, int, action_data*& );
bool        can_enter         ( char_data*, room_data* );
bool        can_leave         ( char_data*, int, bool );
bool        handle_terrain    ( char_data*, room_data*, room_data*,
                                int&, int& );
bool        is_exhausted      ( char_data*, int&, int );
int         get_motion        ( char_data* );
int         find_door         ( char_data*, char* );
exit_data*  valid_exit        ( char_data*, int );
void        act_leader        ( const char*, char_data* );
void        add_delays        ( char_data*, int& );

#define rd  room_data
#define cd  char_data

void   arrival_message   ( cd*, rd*, exit_data*, int, action_data* );
void   leaving_message   ( cd*, rd*, exit_data*, int, action_data* );
void   leaving_self      ( cd*, exit_data*, int, action_data* );
void   leaving_other     ( cd*, cd*, rd*, exit_data*, int, action_data* );

#undef rd
#undef cd


/*
 *   MOVEMENT ABILITIES
 */


bool char_data :: can_float( )
{
  if( is_set( affected_by, AFF_FLOAT ) )
    return TRUE;

  return FALSE;
}


bool char_data :: can_swim( )
{
  if( can_breath_underwater( ) )
    return TRUE;

  if( species != NULL )
    return is_set( &species->act_flags, ACT_CAN_SWIM );

  return( shdata->skill[SKILL_SWIMMING] != 0 );
} 


bool char_data :: can_fly( )
{
  if( species == NULL )
    return FALSE;

  return is_set( &species->act_flags, ACT_CAN_FLY );
} 


bool char_data :: can_breath_underwater( )
{
  if( species != NULL )
    return is_set( &species->act_flags, ACT_CAN_SWIM );

  return FALSE;
}


bool can_climb( char_data* ch )
{
  if( ch->species != NULL
    && !is_set( &ch->species->act_flags, ACT_CAN_CLIMB ) )
    return FALSE;
  
  return TRUE;
}


/*
 *   CAN MOVE ROUTINE
 */ 


bool Char_Data :: Can_Move( exit_data* exit )
{
  if( is_set( &exit->exit_info, EX_CLOSED ) 
    && !is_set( affected_by, AFF_PASS_DOOR ) )
    return FALSE;

  if( Size( ) > exit->to_room->size )
    return FALSE;

  if( pcdata == NULL
    && ( is_set( &species->act_flags, ACT_SENTINEL )
    || is_set( &exit->to_room->room_flags, RFLAG_NO_MOB )
    || ( is_set( &species->act_flags, ACT_STAY_AREA )
    && exit->to_room->area != in_room->area ) ) )
    return FALSE;

  return TRUE;
}


/*
 *   MAIN MOVEMENT ROUTINE
 */


void move_char( char_data* ch, int door, bool flee )
{
  action_data*   action;
  char_data*   follower;
  room_data*    in_room  = ch->in_room;
  room_data*    to_room;
  exit_data*       exit;
  char_array*      list  = NULL;
  int              move;
  int              type;
  int i;

  ch->shown = 1;

  if( ch->rider != NULL ) {
    move_char( ch->rider, door, flee );
    return;
    }

  if( ( exit = valid_exit( ch, door ) ) == NULL 
    || !can_leave( ch, door, flee ) )
    return;

  to_room = exit->to_room;

  if(  !can_enter( ch, to_room ) 
    || !handle_terrain( ch, in_room, to_room, move, type ) )
    return;

  add_delays( ch, move );

  if(   is_exhausted( ch, move, type ) 
    || !trigger_leaving( ch, in_room, door, action )
    || !passes_drunk( ch ) )
    return;

  if( ch->mount != NULL )
    ch->mount->move -= move;
  else
    ch->move -= move;

  if( flee )
    type = MOVE_FLEE;

  if( leader == NULL ) {
    leader = ch;
    list = followers( ch, ch->array );
    }

  leaving_message( ch, in_room, exit, type, action );
  make_tracks( ch, in_room, door );

  ch->From( );
  ch->To( to_room );

  if( ch->mount != NULL ) {
    ch->mount->From( );
    ch->mount->To( to_room );
    }

  ch->room_position = dir_table[door].reverse;

  arrival_message( ch, to_room, exit, type, action );

  if( list != NULL ) {
    for( i = 0; i < *list; i++ ) 
      if( ( follower = list->list[i] ) != leader
        && follower->leader->in_room != follower->in_room
        && follower->position == POS_STANDING )
        move_char( follower, door, FALSE );

    for( i = 0; i < *list; i++ ) {
      follower = list->list[i];
      if( follower->in_room == to_room ) {
        send( "\r\n", follower );
        show_room( follower, to_room, TRUE, TRUE );
        }
      }

    for( i = 0; i < *list; i++ ) {
      follower = list->list[i];
      if( follower->in_room == to_room ) 
        trigger_entering( follower, to_room, dir_table[door].reverse );
      remove_bit( &follower->status, STAT_FOLLOWER );
      }

    delete list;
    }

  if( leader == ch ) 
    leader = NULL;

  if( ch->active.time == -1 )
    add_queue( &ch->active, 5 );
}


/* 
 *   MESSAGE ROUTINES
 */


bool group_message( char_data* ch, char_data* actor )
{
  if( actor == leader || !is_set( &actor->status, STAT_FOLLOWER ) ) 
    return FALSE;    

  if( ch->pcdata == NULL
    || !is_set( &ch->pcdata->message, MSG_GROUP_MOVE ) )
    return TRUE;
  
  if( actor->Seen( ch ) ) {
    if( actor->leader == ch ) 
      send( ch, "%s follows you.\r\n", actor );
    else
      send( ch, "%s follows %s.\r\n", actor, actor->leader );
    }

  return TRUE;
}


void arrival_message( char_data* ch, room_data* room, exit_data* exit,
  int type, action_data* )
{
  char_data*        rch;
  room_data*    to_room;
  int              back  = dir_table[ exit->direction ].reverse;
  int i;

  if( type == MOVE_FLEE ) {
    if( ch->mount != NULL ) 
      fsend( *ch->array,
        "%s, riding %s, charges blindly in, fleeing something %s.\r\n",
        ch, ch->mount, dir_table[back].where );
    else
      fsend( *ch->array,
        "%s arrives, obviously fleeing something %s.\r\n",
        ch, dir_table[back].where );
    return;
    }

  for( i = 0; i < room->contents; i++ )  
    if( ( rch = character( room->contents[i] ) ) != NULL
      && rch != ch && rch->link != NULL
      && rch->position > POS_SLEEPING
      && !group_message( rch, ch ) ) {
      if( ch->Seen( rch ) ) {
        if( ch->mount != NULL ) 
          send( rch, "%s riding %s has arrived.\r\n", ch, ch->mount );
        else 
          send( rch, "%s %s from %s.\r\n", ch,
            arriving_verb[type],
            dir_table[ exit->direction ].arrival_msg );
        }
      /*
      else if( can_hear( rch ) ) 
        send( rch, "You hear someone or something arrive.\r\n" );
      */
      }

  for( i = 0; i < room->exits; i++ ) {
    to_room = room->exits[i]->to_room;
    for( int j = 0; j < to_room->contents; j++ )  
      if( ( rch = character( to_room->contents[j] ) ) != NULL
        && is_set( rch->affected_by, AFF_SENSE_DANGER ) )
        send( rch, "You sense %s %s.\r\n", ch,
          dir_table[ exit->direction ].where );
    }

  return;
}


/* 
 *   LEAVING MESSAGES
 */


const char* leaving_msg [] =
{
  "to_char",  "You $t $T.\r\n",
  "to_room",  "$1 $t $T.\r\n",
  ""
};


void leaving_message( char_data* ch, room_data* room, exit_data* exit,
  int type,  action_data* action )
{
  char_data* rch;

  if( type == MOVE_FLEE ) {
    if( ch->mount != NULL ) {
      send( ch, "Fleeing the battle, you ride %s %s.\r\n",
        ch->mount, dir_table[ exit->direction ].name );
      send( *ch->array, "Fleeing the battle, %s rides %s %s.\r\n",
        ch, ch->mount, dir_table[ exit->direction ].name );
      }
    else {
      send( ch, "You flee %s.\r\n", dir_table[ exit->direction ].name );
      send( *ch->array, "%s blindly flees %s.\r\n", ch,
        dir_table[ exit->direction ].name );
      }
    return;
    }

  leaving_self( ch, exit, type, action );

  for( int i = 0; i < room->contents; i++ ) 
    if( ( rch = character( room->contents[i] ) ) != NULL
      && ch != rch && rch->link != NULL
      && rch->position > POS_SLEEPING
      && !group_message( rch, ch ) ) 
      leaving_other( rch, ch, room, exit, type, action );
}


void leaving_self( char_data* ch, exit_data* exit,
  int type, action_data* action )
{
  if( ch->mount != NULL ) {
    send( ch, "You ride %s %s.\r\n", ch->mount,
      dir_table[ exit->direction ].name );
    } 
  else if( leader != NULL && leader != ch ) {
    send( ch, "You follow %s.\r\n", ch->leader );
    } 
  else if( is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( ch, "You %s through %s!\r\n",
      exit->direction == DIR_UP
      || exit->direction == DIR_DOWN ? "climb" : "step",
      exit->name );      
    }
  else {
    act( ch, prog_msg( action, leaving_msg[0], leaving_msg[1] ),
      ch, NULL, leaving_action[type],
      dir_table[ exit->direction ].name );
    }
      
  return;
}


void leaving_other( char_data* rch, char_data* ch, room_data* room,
  exit_data* exit, int type, action_data* action )
{
  if( !ch->Seen( rch ) ) {
    /*
    if( can_hear( rch, ch ) ) 
      send( rch, "You hear someone or something leave %s.\r\n", 
        dir_table[door].name );
	*/
    return;
    }

  if( is_set( &exit->exit_info, EX_CLOSED ) ) {
    send( rch, "%s %s %s passing through %s!\r\n",
      ch, leaving_verb[type], dir_table[ exit->direction ].name, 
      exit->name );
    }
  else if( ch->mount == NULL ) {
    act( rch, prog_msg( action, leaving_msg[2], leaving_msg[3] ),
      ch, NULL,leaving_verb[type],
      dir_table[ exit->direction ].name );
    }
  else {
    send( rch, "%s rides %s %s.\r\n", ch, ch->mount,
      dir_table[ exit->direction ].name );
    }
}


/*
 *   SUBROUTINES OF MAIN MOVEMENT FUNCTION
 */


bool can_leave( char_data* ch, int door, bool flee )
{
  char_data*    rch;
  bool        after;

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    send( "You are entangled in a web and quite stuck.\r\n", ch );
    act_leader(
      "** %s is entangled in a web and unable to follow you. **\r\n", ch );
    return FALSE;
    }

  if( flee )
    return TRUE;

  if( opponent( ch ) != NULL ) {
    send( "You can't walk away from a battle - use flee.\r\n", ch );
    act_leader( "** %s is fighting and unable to follow you. **\r\n", ch );  
    return FALSE;
    }

  after = TRUE;
  for( int i = 0; i < *ch->array; i++ ) { 
    if( ( rch = character( ch->array->list[i] ) ) != NULL ) {
      if( rch == ch ) {
        after = FALSE; 
        continue;
        }
      if( includes( rch->aggressive, ch )
        && rch->Size( ) > max( SIZE_GNOME,
        ch->Size( )-2 ) && rch->position >= POS_FIGHTING && ( ( after
        && rch->room_position == door ) || ( !after
        && ch->room_position != door ) ) 
        && !is_set( ch->affected_by, AFF_ENTANGLED ) ) {
        send( ch, "%s is blocking your exit %s.\r\n", rch,
          dir_table[door].name );
        return FALSE;
        }
      }    
    }

  return TRUE;
}


exit_data* valid_exit( char_data* ch, int door )
{
  exit_data* exit = exit_direction( ch->in_room, door );

  if( exit == NULL || !exit->Seen( ch ) ) {
    if( ch->in_room->Seen( ch ) )
      send( ch, "You see no exit %s.\r\n", 
        dir_table[ door ].where );
    else { 
      fsend( ch, "You attempt to move %s but find yourself unable to.\r\n",
        dir_table[door].name );
      fsend_seen( ch,
        "%s attempts to move %s and runs straight into a wall.",
        ch, dir_table[door].name );
      }
    return NULL;
    }

  if( is_set( &exit->exit_info, EX_CLOSED )
    && !is_set( ch->affected_by, AFF_PASS_DOOR ) ) {
    send( ch, "%s is closed.\r\n", exit );
    return NULL;
    }

  if( is_set( &exit->exit_info, EX_REQUIRES_CLIMB ) 
    && !can_climb( ch ) && !ch->can_fly( ) ) {
    act_leader( "** %s is unable to climb so can't follow you. **\r\n", ch );
    send( ch, "Leaving %s requires you to climb which you are incapable\
 of.\r\n", dir_table[door].name );
    return NULL;
    }   

  return exit;
}


bool can_enter( char_data* ch, room_data* room )
{
  if( ch->pcdata != NULL && room->area->status != AREA_OPEN
    && room->area->status != AREA_IMMORTAL
    && !is_apprentice( ch ) ) {
    send( ch, "That area is not open to players yet.\r\n" );
    return FALSE;
    }

  if( ch->Size( ) > room->size ) {
    send( ch, "You are too large to fit in there.\r\n" );
    act_leader( "** %s is too large to follow you there. **\r\n", ch );
    return FALSE;
    }
  
  if( ch->mount != NULL ) {
    if( ch->mount->Size( ) > room->size ) {
      send( ch, "%s is too large to fit in there.\r\n", ch->mount );
      return FALSE;
      }
    if( IS_SET( room->room_flags, RFLAG_NO_MOUNT ) ) {
      send( "You can not go there while mounted.\r\n", ch );
      return FALSE;
      }
    } 

  return TRUE;
}


/*
 *   DRUNK ROUTINE
 */


const char *drunk_message[] = {
  "You stumble and barely stay on your feet.\r\n",
  "$n stumbles and barely stays on $m feet.",

  "The ground moves quickly sending you reeling.\r\n",
  "$n staggers, obviously intoxicated.",

  "Your legs give way to gravity.\r\n",
  "$n suddenly sits down and looks quite surprised.",

  "You stumble.\r\n",
  "$n has had too much to drink and falls to the ground.",

  "You trip over your left foot.\r\n",
  "The feet of $n decide to fight and down $s falls."
  };


bool passes_drunk( char_data* ch )
{
//  int i;
  /*
  if( !IS_DRUNK( ch ) || number_range( 1, 20 ) < 18 || ch->mount != NULL )
    return TRUE;

  i = number_range( 0, 4 );

  send( ch, drunk_message[2*i] );
  act_room( drunk_message[2*i+1], ch, NULL, NULL, TO_ROOM );

  if( i <= 1 )
    return TRUE;

  ch->position  = POS_RESTING;
  ch->hit      -= 2;

  update_pos( ch );   
  */
  return TRUE;
}


/*
 *   EXHAUSTION
 */


void add_delays( char_data* ch, int& move )
{
  int   flag  [] = { PLR_TRACK, PLR_SEARCHING, PLR_SNEAK };
  int   cost  [] = { 2, 3, 2 };
  int      i;

  if( ch->pcdata == NULL ) 
    return;

  for( i = 0; i < 3; i++ ) 
    if( is_set( ch->pcdata->pfile->flags, flag[i] ) ) 
      move += cost[i];
}


bool is_exhausted( char_data* ch, int& move, int type )
{
  if( type == MOVE_SWIM && ch->pcdata != NULL )
    move *= 12-ch->shdata->skill[ SKILL_SWIMMING ];

  if( ch->mount != NULL ) {
    if( ch->mount->move < move ) {
      send( ch, "Your mount is exhausted.\r\n" );
      return TRUE;
      }
    }
  else {
    if( ch->move < move ) {
      send( "You are too exhausted.\r\n", ch );
      act_leader( "** %s is too exhausted to follow you. **\r\n", ch );
      return TRUE;
      }
    }

  return FALSE;
}


/*
 *   TERRAIN FUNCTIONS
 */


int get_motion( char_data* ch )
{
  if( ch->can_float( ) ) 
    return MOVE_FLOAT;

  if( ch->can_fly( ) ) 
    return MOVE_FLY;

  if( ch->species != NULL
    && is_set( &ch->species->act_flags, ACT_SLITHERS ) ) 
    return MOVE_SLITHERS;

  if( ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_SNEAK ) ) {
      ch->improve_skill( SKILL_SNEAK );
      return MOVE_SNEAK;
    }

  return MOVE_WALK;
}


bool handle_terrain( char_data* ch, room_data* from, room_data* to,
  int& move, int& type )
{
  char_data*   mount  = ch->mount;

  type = get_motion( mount == NULL ? ch : mount );
  
  switch( type ) {
    case MOVE_FLOAT:   move = 1;  break;
    case MOVE_FLY:     move = 3;  break;
    default:
      move = int( ( terrain[ from->sector_type ].movement_cost
        +terrain[ to->sector_type ].movement_cost )
        *( 1+5.*ch->contents.weight/ch->Capacity( ) )/3 );
    }

  if( to->sector_type == SECT_AIR ) {
    if( type == MOVE_FLY )
      return TRUE;
    if( mount != NULL ) {
      send( ch, "Your mount does not know how to fly.\r\n" );
      }
    else {
      send( ch, "You can't fly.\r\n" );
      act_leader(
        "** %s is unable to fly so does not follow you. **\r\n", ch );
      }
    return FALSE;
    }

  if(  to->sector_type == SECT_WATER_SURFACE 
    || to->sector_type == SECT_RIVER ) { 
    if( type <= MOVE_WALK ) {
      if( mount != NULL ) { 
        if( !mount->can_swim( ) ) {
          send( ch, "Your mount does not know how to swim.\r\n" );
          return FALSE;
          }
        }
      else {
        if( !ch->can_swim( ) ) {
          send( ch, "You don't know how to swim or fly.\r\n" );
          act_leader(
            "** %s can not swim or fly so fails to follow you. **\r\n", ch );
          return FALSE; 
          }
        }
      type = MOVE_SWIM;
      ch->improve_skill( SKILL_SWIMMING );
      }
    return TRUE;
    }   
  
  if( to->sector_type == SECT_UNDERWATER ) {
    if( mount != NULL ) {
      send( ch, "You can't ride underwater.\r\n" );
      return FALSE;
      }
    if( !ch->can_swim( ) ) {
      send( ch, "You don't how to swim.\r\n" );
      act_leader( "** %s can not swim so fails to follow you. **\r\n", ch ); 
      return FALSE;
      }
    type = MOVE_SWIM;
    return TRUE;
    }

  if( from->sector_type == SECT_SHALLOWS ) {
    type = ( ch->species != NULL
      && is_set( &ch->species->act_flags, ACT_CAN_SWIM ) )
      ? MOVE_SWIM : MOVE_WADE;
    return TRUE;
    }

  return TRUE;
}


/*
 *   ENTERING/LEAVNG TRIGGERS
 */


bool trigger_leaving( char_data *ch, room_data *room, int door,
  action_data*& action )
{
  char_data*       npc;
  mprog_data*    mprog;
  bool          result = TRUE;

  for( int i = 0; i < *ch->array; i++ ) { 
    if( ( npc = mob( ch->array->list[i] ) ) == NULL
      || npc->pcdata != NULL || npc->position < POS_RESTING
      || npc == ch )
      continue; 
    for( mprog = npc->species->mprog; mprog != NULL; mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_LEAVING
        && ( mprog->value == door || mprog->value == -1 ) ) {
        var_ch   = ch;
        var_mob  = npc;
        var_room = room;
        if( !execute( mprog ) || ch->in_room != room )
          return FALSE;
        }
    }
 
  for( action = ch->in_room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_LEAVING 
      && is_set( &action->flags, door ) ) {
      var_ch   = ch;
      var_room = room;
      result = execute( action );
      if( ch->in_room != room ) 
        return FALSE;
      if( result )
        return TRUE;
      }

  return result;
}


bool trigger_entering( char_data *ch, room_data *room, int door )
{
  action_data*   action;
  char_data*        npc;
  mprog_data*     mprog;
//  obj_data*         obj;
//  obj_data*    obj_next;
//  oprog_data*     oprog;

  for( int i = 0; i < room->contents; i++ ) {
    if( ( npc = mob( room->contents[i] ) ) == NULL
      || npc == ch || npc->position < POS_RESTING ) 
      continue;
    for( mprog = npc->species->mprog; mprog != NULL; mprog = mprog->next ) 
      if( mprog->trigger == MPROG_TRIGGER_ENTRY
        && ( mprog->value == door || mprog->value == -1 ) )  {
        var_ch = ch;
        var_mob = npc;
        var_room = room;
        execute( mprog );
        if( ch->in_room != room )
          return FALSE;
        }
    }

  for( action = room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_ENTERING
      && is_set( &action->flags, door ) ) {
      var_ch   = ch; 
      var_room = room;
      execute( action );
      if( ch->in_room != room )
        return FALSE;
      break;
      }

  /*
  for( obj = room->contents; obj != NULL; obj = obj_next ) {
    obj_next = obj->next_content;
    for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) 
      if( oprog->trigger == OPROG_TRIGGER_ENTERING ) {
        var_ch = ch;
        var_obj = obj;
        var_room = room;
        execute( oprog );
        if( ch->in_room != room )
          return FALSE;
        }
    }
  */

  return TRUE;
}


/*
 *   LEADER MESSAGE ROUTINES
 */


void act_leader( const char* text, char_data* follower )
{
  char buf [ MAX_INPUT_LENGTH ];

  if( leader == NULL )
    return;

  sprintf( buf, text, follower->Name( leader ) );
  buf[3] = toupper( buf[3] );
  send( buf, leader ); 

  return;
}


/*
 *   DO MOVE FUNCTIONS
 */


void do_north( char_data* ch, char* )
{
  move_char( ch, DIR_NORTH, FALSE );
  return;
}


void do_east( char_data *ch, char* )
{
  move_char( ch, DIR_EAST, FALSE );
  return;
}


void do_south( char_data *ch, char* )
{
  move_char( ch, DIR_SOUTH, FALSE );
  return;
}


void do_west( char_data* ch, char* )
{
  move_char( ch, DIR_WEST, FALSE );
  return;
}


void do_up( char_data *ch, char* )
{
  move_char( ch, DIR_UP, FALSE );
  return;
}


void do_down( char_data *ch, char* )
{
  move_char( ch, DIR_DOWN, FALSE );
  return;
};



/*
-------------------------------------------------------------------------
*/


void do_search( char_data* ch, char* argument )
{
  action_data*  action;
  room_data*      room  = ch->in_room;

  if( not_player( ch ) )
    return;

  if( toggle( ch, argument, "Searching",
    ch->pcdata->pfile->flags, PLR_SEARCHING ) ) {
    send( "[Searching increases movement point cost by 3 per\
 move.]\r\n", ch );
    return;
    }

  for( action = ch->in_room->action; action != NULL; action = action->next )
    if( action->trigger == TRIGGER_SEARCHING 
      && ( ( *action->target == '\0' && *argument == '\0' )
      || ( *argument != '\0' && is_name( argument, action->target ) ) ) ) {
      var_ch   = ch;
      var_room = ch->in_room;
      if( !execute( action ) || ch->in_room != room ) 
        return;
      break;
      }

  if( *argument == '\0' ) 
    send( "You rummage around but find nothing interesting.\r\n", ch );
  else
    send( "Whatever that is, searching it results in nothing\
 interesting.\r\n", ch ); 
}

  
/*
 *   SPEED WALKING
 */


bool speed_walking( char_data* ch, char* argument )
{
  int i;

  for( i = 0; argument[i] != '\0'; i++ ) {
    switch( toupper( argument[i] ) ) {
      case 'S':
      case 'N':
      case 'U':
      case 'D':
      case 'E':
      case 'W':
        break;

      default:
        return FALSE;
      }
    }

  send( ch, "Speed walking is not yet implemented.\r\n" );
  
  return TRUE;
}
