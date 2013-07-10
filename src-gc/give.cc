#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool   give_npc       ( char_data*, char_data*, thing_array* );
bool   give_pet       ( char_data*, char_data*, obj_data* );


/*
 *   GIVE FUNCTIONS
 */


thing_data* given( thing_data* obj, char_data* receiver, thing_data* giver )
{
  obj = obj->From( obj->selected );
  set_owner( (obj_data*) obj, receiver, (char_data*) giver );
  obj->To( receiver );

  return obj;
}


void do_give( char_data* ch, char* argument )
{
  char            arg  [ MAX_STRING_LENGTH ];
  char_data*   victim;
  thing_array*  array;

  if( is_confused_pet( ch ) || newbie_abuse( ch ) ) 
     return;

  if( !two_argument( argument, "to", arg ) ) {
    send( ch, "Give what to whom?\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "give to",
    ch->array ) ) == NULL )
    return;

  if( ( array = several_things( ch, arg, "give",
    &ch->contents ) ) == NULL )
    return;

  if( victim->position <= POS_SLEEPING ) {
    send( ch, "%s isn't in a position to be handed items.\n\r", victim );
    return;
    }

  if( give_npc( ch, victim, array ) )
    return;

  thing_array   subset  [ 4 ];
  thing_func*     func  [ 4 ]  = { cursed, heavy, many, given };

  sort_objects( victim, *array, ch, 4, subset, func );

  page_priv( ch, NULL, empty_string );
  page_priv( ch, &subset[0], "can't let go of" );
  page_priv( ch, &subset[1], "can't lift", victim );
  page_priv( ch, &subset[2], "can't handle", victim );
  page_publ( ch, &subset[3], "give", victim, "to" );

  consolidate( subset[3] );

  delete array;
}


bool give_npc( char_data* ch, char_data* victim, thing_array* array )
{
  mprog_data*  mprog  = NULL;
  obj_data*      obj;

  if( victim->pcdata != NULL ) 
    return FALSE;

  if( array->size != 1 || array->list[0]->selected != 1 ) {
    fsend( ch,
      "%s would be much happier if items were offered one at a time.",
      victim );
    return TRUE;
    }
  
  if( ( obj = object( array->list[0] ) ) != NULL ) {
    if( is_set( &victim->status, STAT_PET ) ) 
      return give_pet( ch, victim, obj );

    for( mprog = victim->species->mprog; mprog != NULL; mprog = mprog->next )
      if( mprog->trigger == MPROG_TRIGGER_GIVE
        && ( mprog->value == obj->pIndexData->vnum || mprog->value == 0 ) )
        break;
    }

  if( mprog == NULL ) {
    fsend( ch, "%s doesn't seem interested in %s.",
      victim, array->list[0] );
    return TRUE;
    } 

  send( ch, "You give %s to %s.\n\r", obj, victim );
  send( *ch->array, "%s gives %s to %s.\n\r", ch, obj, victim );

  obj = (obj_data*) obj->From( obj->selected );
  set_owner( obj, ch, NULL );
  obj->To( victim );

  var_ch   = ch;
  var_mob  = victim;  
  var_obj  = obj; 
  var_room = ch->in_room;
  execute( mprog );

  return TRUE;
}


bool give_pet( char_data* ch, char_data* victim, obj_data* obj )
{
  fsend( ch, "You offer %s to %s.", obj, victim );
  fsend( *ch->array, "%s offers %s to %s.", ch, obj, victim );

  if( obj->pIndexData->item_type == ITEM_FOOD 
    && eat( victim, obj ) )
    return TRUE;

  if( obj->pIndexData->item_type == ITEM_DRINK_CON
    && drink( victim, obj ) ) 
    return TRUE;
  
  fsend( ch, "%s inspects it but doesn't seem interested.",
    victim->He_She( ) );

  return TRUE;
}











