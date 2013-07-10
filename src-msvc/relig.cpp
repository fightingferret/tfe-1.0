#include "define.h"
#include "struct.h"


void do_pray( char_data* ch, char* argument )
{
  obj_data*         obj;
  char_data*     victim  = ch;
  player_data*       pc  = player( ch );
  int            prayer;
  int              need;

  if( is_mob( ch ) ) 
    return;

  if( ch->pcdata->religion == REL_NONE ) {
    send( ch, "You are not the follower of any god and cannot expect aid.\r\n" );
    return;
    }

  if( *argument != '\0'
    && ( victim = one_character( ch, argument, "pray for",
    ch->array ) ) == NULL ) 
    return;

  if( ch->shdata->alignment == ALGN_CHAOTIC_EVIL ) {
    send( ch,
      "As a chaotic evil, the gods will never answer your prayers.\r\n" );
    return;
    }

  if( is_set( &ch->in_room->room_flags, RFLAG_NO_PRAY ) ) {
    send( ch, "As you pray you feel a cold chill touch your soul, you\
 doubt there will be\r\nany aid forthcoming here.\r\n" );
    return;
    }

  prayer = pc->prayer;

  if( victim != ch )
    need = 0;
  else if( victim->fighting == NULL )
    need = victim->max_hit/3;
  else
    need = victim->max_hit/4;

  if( prayer > 150 && victim->hit < need ) {
    prayer      -= 150;
    victim->hit  = victim->max_hit;
    update_pos( victim );
    update_max_move( victim );
    victim->move = victim->max_move;
    send( victim, "A dim light surrounds you curing your wounds.\r\n" );
    send( *victim->array, "A dim light surrounds %s curing %s wounds.\r\n", 
      victim, victim->His_Her( ) );
    } 
 
  if( is_set( victim->affected_by, AFF_BLIND ) && prayer > 200 ) {
    prayer -= 200;
    strip_affect( victim, AFF_BLIND );
    }

  if( is_set( victim->affected_by, AFF_POISON ) && prayer > 150 ) {
    prayer -= 150;
    strip_affect( victim, AFF_POISON );
    }

  if( victim == ch ) {
    if( ch->pcdata->condition[COND_FULL] < 0 && prayer > 30 ) {
      prayer -= 30;
      ch->pcdata->condition[COND_FULL] = 30;
      send( ch, "Your stomach feels full.\r\n" );
      }

    if( ch->pcdata->condition[COND_THIRST] < 0 && prayer > 30 ) {
      prayer -= 30;
      ch->pcdata->condition[COND_THIRST] = 30;
      send( ch, "You no longer feel thirsty.\r\n" );
      } 

    if( ch->in_room->is_dark( ) && prayer > 30
      && !is_set( ch->affected_by, AFF_INFRARED )
      && !is_set( ch->affected_by, AFF_BLIND ) ) {
      prayer -= 30;
      obj     = create( get_obj_index( OBJ_BALL_OF_LIGHT ) );
      send( ch, "%s appears in your hand.\r\n", obj );
      send_seen( ch, "%s appears in %s's hand.\r\n", obj, ch );
      obj->To( ch );
      consolidate( obj );
      }

    if( ch->move < 10 && ch->max_move > 20 && prayer > 20 ) {
      prayer   -= 20;
      ch->move  = ch->max_move;
      send( ch, "You feel rejuvinated.\r\n" );
      }
    }

  if( prayer == pc->prayer ) {
    if( ch == victim )
      send( ch, "You pray to the gods to no avail.\r\n" );
    else
      fsend( ch, "You pray for the soul of %s to no avail.", victim );
    }

  pc->prayer = max( 0, prayer );
}


/*
 *   SACRIFICE 
 */


void do_sacrifice( char_data* ch, char* argument )
{
  char             arg  [ MAX_INPUT_LENGTH ];
  action_data*  action;
  obj_data*        obj;
  player_data*      pc;
//  int             cost;
  int                i;

  if( is_mob( ch ) ) 
    return;

  if( !is_set( &ch->in_room->room_flags, RFLAG_ALTAR ) ) {
    send( ch, "Sacrifices will only be recognized at altars.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "The gods do not value empty sacrifices.\r\n" );
    return;
    }

  if( !two_argument( argument, "to", arg ) ) {
    send( ch, "Syntax: sacrifice <object> to <diety>.\r\n" );
    return;
    }

  for( i = 1; ; i++ ) {
    if( i == MAX_ENTRY_RELIGION ) {
      send( ch, "Unknown diety.\r\n" );
      return;
      }
    if( matches( argument, religion_table[i].name ) )
      break;
    }

  if( ( obj = one_object( ch, arg, "sacrifice", 
    ch->array, &ch->contents ) ) == NULL )
    return;
 
  if( obj->array != ch->array ) {
    send( ch, "You must drop %s first to sacrifice it.\r\n", obj );
    return;
    }  

  if( is_set( obj->pIndexData->extra_flags, OFLAG_NOSACRIFICE ) ) {
    send( ch, "The gods grow angry at your impudence.\r\n" );
    return;
    }

  if( !can_wear( obj, ITEM_TAKE ) ) {
    send( ch,
      "%s is immovable and this makes the required ritual impossible.\r\n",
      obj );
    return;
    } 

  if( !forbidden( obj, ch ) ) {
    send( ch, "You are forbidden from sacrificing %s.\r\n", obj );
    return;
    } 

  for( action = ch->in_room->action; action != NULL; action = action->next ) 
    if( action->trigger == TRIGGER_SACRIFICE
      && obj->pIndexData->vnum == action->value ) {
      var_ch = ch;
      var_obj = obj;
      var_room = ch->in_room;
      execute( action );
      return;
      }

  ch->pcdata->religion = i;

  fsend( ch, "You sacrifice %s to %s.\r\n", obj,
    religion_table[i].name );
  fsend_seen( ch, "%s sacrifices %s to %s.\r\n",
    ch, obj, religion_table[i].name );

  if( ( pc = player( ch ) ) != NULL ) { 
    pc->reputation.gold += obj->Cost( );
    if( obj->pIndexData->item_type == ITEM_CORPSE )
      pc->reputation.blood++;
    if( is_set( obj->extra_flags, OFLAG_MAGIC ) )
      pc->reputation.magic += 1+obj->Cost( )/1000;
    }

  obj->Extract( 1 );
}




















