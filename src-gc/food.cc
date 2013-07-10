#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool consume_trigger    ( char_data*, obj_data* );


const char*  cook_word [] = { "raw", "cooked", "burnt" };


/*
 *   DRINK ROUTINES
 */


bool consume_trigger( char_data *ch, obj_data *obj )
{
  oprog_data* oprog;

  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next )
    if( oprog->trigger == OPROG_TRIGGER_CONSUME ) {
      var_ch   = ch;
      var_obj  = obj; 
      var_room = ch->in_room;
      execute( oprog );
      return TRUE;
      }
  
  return FALSE;
}


/*
 *   EAT ROUTINES
 */


bool eats_corpses( char_data* ch )
{
  if( ch->species != NULL )
    return is_set( &ch->species->act_flags, ACT_CARNIVORE );

  return( ch->shdata->race == RACE_TROLL );
}


bool vegetarian( char_data* ch )
{
  if( ch->species != NULL )
    return !is_set( &ch->species->act_flags, ACT_CARNIVORE );

  return( ch->shdata->race == RACE_ENT );
}
 

void do_eat( char_data* ch, char* argument )
{
  obj_data* obj;
 
  if( ( obj = one_object( ch, argument,
    "eat", &ch->contents, ch->array ) ) == NULL ) 
    return;

  eat( ch, obj );
}


bool eat( char_data* ch, obj_data* obj )
{
  if( obj->pIndexData->item_type == ITEM_CORPSE ) {
    if( !eats_corpses( ch ) ) {
      send( ch, "What a disgusting suggestion!\n\r" );
      return FALSE;
      }
    }
  else if( obj->pIndexData->item_type != ITEM_FOOD ) {
    if( ch->shdata->race == RACE_TROLL ) {
      if( obj->metal( ) || obj->stone( ) ) {
        send( ch, "That is a bit too crunchy even for your taste.\n\r" );
        return FALSE;
        }
      }
    else {
      send( ch, "That's not edible.\n\r" );
      return FALSE;
      }
    }

  if( vegetarian( ch ) && is_set( &obj->materials, MAT_FLESH ) ) {
    send( ch, "You do not find meat edible.\n\r" );
    return FALSE;
    } 

  if( ch->species == NULL && ch->pcdata->condition[COND_FULL] > 40 ) {
    send( ch, "You are too full to eat more.\n\r" );
    return FALSE;
    }  

  if( !is_empty( obj->contents ) ) {
    send( ch, "That object is not empty, remove it's contents first.\n\r" );
    return FALSE;
    }

  switch ( obj->pIndexData->item_type ) {
    case ITEM_SCROLL:
      send( ch, "You rip %s into shreds and happily munch it.\n\rAnd who\
 said real trolls don't eat salad?\n\r", obj );
      fsend_seen( ch, "%s rips %s into shreds and happily munchs it.\n\r",
        ch, obj );
      break;

    case ITEM_CORPSE:
      if( obj != forbidden( obj, ch ) ) {
        fsend( ch,
          "You feel the gods would be unduely displeased if you ate %s.",
          obj );
        return FALSE;
        } 
      send( ch,
        "You gleefully pull apart %s and eat it piece by piece.\n\r",
        obj );
      fsend( *ch->array,
        "%s pulls apart %s and eats it piece by piece making a disgusting\
 sight.\n\r", ch, obj );
      break;

    case ITEM_FOOD:
      if( obj->value[3] != 0 && !ch->save_vs_poison( obj->value[3] ) ) {
        affect_data af;
        send( ch, "You choke and gag.\n\r" );
        send( *ch->array, "%s chokes and gags.\n\r", ch );
        af.type      = AFF_POISON;
        af.duration  = 2 * obj->value[0];
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.leech     = NULL; 
        add_affect( ch, &af );
        }

    default:
      send( ch, "You eat %s.\n\r", obj );
      send_seen( ch, "%s eats %s.\n\r",  ch, obj );
      break;
      }

  gain_condition( ch, COND_FULL, obj->pIndexData->item_type
    != ITEM_FOOD ? obj->weight/100 : obj->value[0] );

  if( !consume_trigger( ch, obj ) ) 
    obj->Extract( 1 );

  return TRUE;
}


/*
 *   COOK ROUTINES
 */


void do_cook( char_data* ch, char* argument )
{
  /*
  obj_data*   obj;
  obj_data*  fire;

  if( *argument == '\0' ) {
    send( "Cook what?\n\r", ch );
    return;
    }

  if( ( obj = get_obj_inv( ch, argument ) ) == NULL ) {
    send( "You do not have that item.\n\r", ch );
    return;
    }

  if( obj->selected != 1 && obj->next_list != NULL ) {
    send( "You can only cook one object at a time.\n\r", ch );
    return;
    }

  for( fire = *ch->array; fire != NULL; fire = fire->next_content )
    if( fire->pIndexData->item_type == ITEM_FIRE )
      break;

  if( fire == NULL ) {
    send( ch,
      "You see nothing in the room with which to cook %s.\n\r", obj );
    return;
    }

  send( ch, "You cook %s over %s.\n\r", obj, fire );

  if( obj->pIndexData->item_type == ITEM_FOOD
    && obj->value[1] >= 0 && obj->value[1] < 2 ) {
    if( obj->number > 1 ) {
      obj->number--;
      obj = duplicate( obj );
      obj->value[1]++;
      put_obj( obj, ch );
      }
    else {
      obj->value[1]++;
      }
    }
  */
  return;
}



