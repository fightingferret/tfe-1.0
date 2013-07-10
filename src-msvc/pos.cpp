#include "define.h"
#include "struct.h"


/*
 *   LOCAL FUNCTIONS
 */


bool  can_wake     ( char_data* );
bool  sit_trigger  ( char_data*, obj_data* );
void  sit          ( char_data*, obj_data* );


/*
 *   CONSTANTS
 */


const char* position_name [] = { "dead", "mortally wounded",
  "incapacitated", "stunned", "sleeping", "meditating", "resting",
  "fighting", "standing", "flying" };


void update_pos( char_data* ch )
{
  if( ch->hit > 0 ) {
    if( ch->position < POS_SLEEPING ) {
      ch->position = POS_RESTING;
      renter_combat( ch );
      }
    return;
    }

       if( ch->hit <= -11 ) ch->position = POS_DEAD;
  else if( ch->hit <= -6 )  ch->position = POS_MORTAL;
  else if( ch->hit <= -3 )  ch->position = POS_INCAP;
  else                      ch->position = POS_STUNNED; 

  ch->fighting = NULL;
  disrupt_spell( ch );
}


/*
 *   CHAIR ROUTINES
 */


void sit( char_data* ch, obj_data* obj )
{
  /*
  ch->next_on_obj = obj->character;
  obj->character  = ch;
  ch->pos_obj     = obj;
  ch->position    = POS_RESTING;

  return;
  */
};


void unseat( char_data* ch )
{
  /*
  char_data*  sch;
  obj_data*   obj;
 
  if( ( obj = ch->pos_obj ) != NULL ) {
    if( obj->character == ch )
      obj->character = ch->next_on_obj;
    else {
      for( sch = obj->character; sch->next_on_obj != ch;
        sch = sch->next_on_obj );        
      sch->next_on_obj = ch->next_on_obj;
      }
    ch->pos_obj     = NULL;
    ch->next_on_obj = NULL;
    }

  return;
  */
}


/*
 *   MEDITATE
 */ 


void do_meditate( char_data* ch, char* )
{
  if( is_confused_pet( ch ) )
    return;

  if( ch->pcdata == NULL || ch->shdata->skill[SKILL_MEDITATE] == UNLEARNT ) {
    send( ch, "You do not have the knowledge required to bring yourself\
 into a state of\r\nmeditation.\r\n" );
    return;
    }

  if( ch->mount != NULL ) {
    send( "You can't meditate while mounted.\r\n", ch );
    return;
    }

  if( water_logged( ch->in_room ) ) {
    send( ch, "You can't meditate underwater.\r\n" );
    return;
    }

  if( ch->position == POS_FIGHTING ) {
    send( "Meditating while fighting requires more concentration than you can muster.\r\n", ch );
    return;
    }

  if( ch->position == POS_MEDITATING ) {
    send( "You are already meditating.\r\n", ch );
    return;
    }

  send( "You kneel down and start to meditate.\r\n", ch );
  send_seen( ch, "%s kneels down and starts to meditate.\r\n", ch );

  ch->position = POS_MEDITATING;
  return;
}


/*
 *   LIE DOWN
 */


void do_lie( char_data* ch, char* )
{
  if( ch->mount != NULL ) {
    send( ch, "You need to dismount first.\r\n" );
    return;
    }

  if( deep_water( ch->in_room ) ) {
    send( ch, "You can't lie down while swimming.\r\n" );
    return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "You refuse to lie down while facing adversity.\r\n" );
    return;
    }

  switch( ch->position ) {
    case POS_SLEEPING:
      if( can_wake( ch ) ) 
        send( ch, "You open your eyes.\r\n" );
      return;

    case POS_RESTING:
      send( ch, "You situp.\r\n" );
      break;

    case POS_MEDITATING:
      send( ch, "You stop meditating and lie down.\r\n" );
      send_seen( ch, "%s stops meditating and lies down.\r\n", ch );
      break;

    case POS_STANDING:
      send( ch, "You lie down on the ground.\r\n" );
      send_seen( ch, "%s lies down on the ground.\r\n", ch );
      break;
    }

  return;
}


/*
 *   SIT
 */


void do_sit( char_data* ch, char* argument )
{
//  obj_data*      obj;
//  thing_data*  thing;
//  char_data*     sch;
//  int              i;

  if( *argument == '\0' ) {
    do_rest( ch, "" );
    return;
    }

  send( ch, "Sitting on objects disabled.\r\n" );

  /*
  if( ch->position == POS_FIGHTING ) {
    send( ch, "You can't sit down while fighting!" );
    return;
    }

  if( ch->position != POS_STANDING ) {
    send( ch,
      "You need to be standing before you can sit on an object.\r\n" );
    return;
    }

  if( ch->pos_obj != NULL ) {
    send( "You are sitting and standing at the same time.!?\r\n", ch );
    bug( "Do_sit: Character standing, but pos_obj != NULL. (%s)",
      ch->real_name( ) );
    return;
    }

  if( ( thing = one_thing( ch, argument,
    "sit on", ch->array ) ) == NULL ) 
    return;

  if( ( obj = object( thing ) ) == NULL ) {
    send( ch, "Sit only applies to objects for now.\r\n" );
    return;
    }

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_CHAIR ) ) { 
    send( "That is not made to be sat on.\r\n", ch );
    return;
    }

  for( i = 0, sch = obj->character; sch != NULL;
    i++, sch = sch->next_on_obj ); 

  if( obj->value[0] <= i ) {
    send( "There is not enough room for you to sit on that.\r\n", ch );
    return;
    } 

  sit( ch, obj );

  send( ch, "You rest your weary bones and sit on %s.\r\n",
    obj->pIndexData->Name( ) );
  fsend_seen( ch, "%s sits on %s.", ch, obj );

  sit_trigger( ch, obj );
  */
  return;
}


bool sit_trigger( char_data *ch, obj_data *obj )
{
  oprog_data* oprog;

  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next )
    if( oprog->trigger == OPROG_TRIGGER_SIT ) {
      var_ch   = ch;
      var_obj  = obj; 
      var_room = ch->in_room;
      execute( oprog );
      return TRUE;
      }
  
  return FALSE;
}


void do_rest( char_data* ch, char*  )
{
  if( ch->mount != NULL ) {
    send( "You need to dismount first.\r\n", ch );
    return;
    }

  if( deep_water( ch->in_room ) ) {
    send( ch, "You can't rest while swimming.\r\n" );
    return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "Rest while fighting??\r\n" );
    return;
    }

  switch( ch->position ) {
    case POS_SLEEPING:
      if( can_wake( ch ) ) {
        send( ch, "You wake and sit up.\r\n" );
        send_seen( ch, "%s wakes and sits up.\r\n", ch );
        ch->position = POS_RESTING;
        }
      break;

    case POS_RESTING:
      send( ch,"You are already resting.\r\n" );
      break;

    case POS_MEDITATING:
      send( ch, "You stop meditating and begin resting.\r\n" );
      fsend_seen( ch, "%s stops meditating and begins resting.", ch );
      ch->position = POS_RESTING;
      break;

    case POS_STANDING:
      if( ch->rider != NULL ) {
        send( ch, "You lie down, dismounting %s.\r\n", ch->rider );
        send( ch->rider, "Your mount lies down, dismounting you.\r\n" );
        send_seen( ch, "%s lies down, dismounting %s.\r\n", ch, ch->rider );
        dismount( ch->rider );
        }
      else {  
        send( ch, "You sit down on the floor.\r\n" );
        send_seen( ch, "%s sits down on the floor.\r\n", ch );
        }
      ch->position = POS_RESTING;
      break;
    }

  return;
}


/*
 *   SLEEP 
 */


bool can_wake( char_data* ch )
{
  if( !is_set( ch->affected_by, AFF_SLEEP ) || is_apprentice( ch ) ) 
    return TRUE;
  
  send( ch, "You are magically slept and unable to wake.\r\n" );

  return FALSE;
} 


void do_sleep( char_data* ch, char* )
{
  char_data* keeper;

  if( ch->mount != NULL ) {
    send( ch, "You can't sleep while riding.\r\n" );
    return;
    }

  if( water_logged( ch->in_room ) ) { 
    send( ch, "You can't sleep underwater.\r\n" );
    return;
    } 

  if( ( keeper = active_shop( ch ) ) != NULL ) {
    send( ch, "%s won't allow vagrants to sleep in %s shop.\r\n", 
      keeper, keeper->His_Her( ) );
    return;
    }

  if( ch->fighting != NULL ) {
    send( ch, "It tends to be difficult to fall asleep while fighting.\r\n" );
    return;
    }

  switch( ch->position ) {
    case POS_SLEEPING:
      send( ch, "You are already asleep.\r\n" );
      break;

    case POS_MEDITATING:
    case POS_RESTING:
    case POS_STANDING: 
      sleep( ch );
      send( ch, "You sleep.\r\n" );
      send_seen( ch, "%s lies down and goes to sleep.\r\n", ch );
      break;
    }

  return;
}


void sleep( char_data *ch )
{
  disrupt_spell( ch );

  ch->position = POS_SLEEPING;

  if( ch->rider != NULL ) {
    send( ch->rider,
      "As %s drops to sleep, you quickly find yourself dismounted.\r\n", ch );
    fsend_seen( ch->rider,
      "As %s falls to sleep, %s quickly finds %sself dismounted.",
      ch->rider, ch->rider->He_She( ), ch->rider->Him_Her( ) );
    ch->rider->mount = NULL;
    ch->rider = NULL;
    }

  return;
}


/*
 *   STAND 
 */


void do_stand( char_data* ch, char* )
{
  if( ch->mount != NULL ) {
    send( ch, "Use dismount to stand on the ground.\r\n" );
    return;
    }

  switch ( ch->position ) {
    case POS_SLEEPING:
      if( can_wake( ch ) ) { 
        send( ch, "You wake and stand up.\r\n" );
        send_seen( ch, "%s wakes and stands up.\r\n", ch );
        ch->position = POS_STANDING;
        }
      break;

    case POS_MEDITATING:
      send( "You stop meditating and stand up.\r\n", ch );
      send_seen( ch, "%s stops meditating and stands up.\r\n", ch );
      ch->position = POS_STANDING;
      break;

    case POS_RESTING:
      if( ch->pos_obj != NULL ) {
        send( ch, "You get off %s and stand up.\r\n", ch->pos_obj );
        send( *ch->array, "%s gets off %s and stands up.\r\n",
          ch, ch->pos_obj );
        unseat( ch );
        }
      else {
        if( opponent( ch ) != NULL ) {
          send( ch, "Sitting in battle was not intentional.\r\n" );
          return;
	  }
        else { 
          send( ch, "You stand up.\r\n" );
          send_seen( ch, "%s stands up.\r\n", ch );
	  }
        }
      ch->position = POS_STANDING;
      break;

    case POS_STANDING:
      send( ch, "You are already standing.\r\n" );
      break;
    }

  return;
}


/*
 *   WAKE
 */


void do_wake( char_data* ch, char* argument )
{
  char_data* victim;

  if( *argument == '\0' ) {
    if( ch->position > POS_SLEEPING ) {
      send( ch, "You aren't sleeping.\r\n" );
      return;
      }
    do_stand( ch, "" );
    return;
    }

  if( ch->position <= POS_SLEEPING ) {
    send( ch, "You are asleep yourself!\r\n" );
    return;
    }

  if( ( victim = one_character( ch, argument, "wake",  ch->array ) ) == NULL )
    return;

  if( victim->position > POS_SLEEPING ) {
    send( ch, "%s is already awake.\r\n", victim );
    return;
    }

  if( victim->position < POS_SLEEPING || IS_AFFECTED( victim, AFF_SLEEP ) ) {
    send( ch, "You try to wake %s, but %s doesn't react.\r\n",
      victim, victim->He_She( ) );
    return;
    }

  victim->position = POS_RESTING;

  send( ch, "You wake %s.\r\n", victim->Him_Her( ) );
  send( victim, "%s wakes you.\r\n", ch );
  send_seen( ch, "%s wakes %s.\r\n", ch, victim );
}






