#include "define.h"
#include "struct.h"


/*
 *   CHARACTERS
 */ 


bool target_offensive( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  if( *argument == '\0'
    && ( cast->target = opponent( ch ) ) != NULL )
    return TRUE;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp,
    ch->array ) ) == NULL )
    return FALSE;

  if( victim == ch ) {
    fsend( ch,
      "Your anima speaks up nervously and argues persuasively that casting\
 %s at yourself would be fatuous.", spell_table[ cast->spell ].name );
    return FALSE;
    }

  if( !can_kill( ch, victim ) )
    return FALSE;

  cast->target = victim;
  return TRUE;
}


bool target_self_only( char_data* ch, cast_data* cast, char* argument )
{
  if( *argument != '\0' ) {
    fsend( ch, "Casting %s requires no argument as the only possible victim\
 is yourself.", spell_table[ cast->spell ].name );
    return FALSE;
    }

  cast->target = ch;
  return TRUE;
}


bool target_peaceful( char_data* ch, cast_data* cast, char* argument )
{
  char tmp  [ ONE_LINE ];  

  if( *argument == '\0' ) {
    cast->target = ch;
    return TRUE;
    }

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  return( ( cast->target = one_character( ch, argument, tmp,
    ch->array ) ) != NULL );
}


bool target_peaceful_other( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  if( *argument == '\0'
    && ( cast->target = opponent( ch ) ) != NULL )
    return TRUE;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp,
    ch->array ) ) == NULL )
    return FALSE;

  if( ch == victim ) {
    send( ch, "You can't cast %s on yourself.\r\n",
      spell_table[ cast->spell ].name ); 
    return FALSE;
    }

  cast->target = victim;

  return TRUE;
}


bool target_mob_only( char_data* ch, cast_data* cast, char* argument )
{
  if( *argument == '\0' 
    && ( cast->target = opponent( ch ) ) != NULL ) 
    return TRUE;

  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp,
    ch->array ) ) == NULL )
    return FALSE;

  if( victim->Type( ) == PLAYER_DATA ) {
    send( ch, "You can't cast %s on players.\r\n", 
      spell_table[ cast->spell ].name );
    return FALSE;
    }

  cast->target = victim;
  return TRUE;
}


bool target_world( char_data* ch, cast_data* cast, char* argument )
{
  char           tmp  [ ONE_LINE ];  
  char_data*  victim;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( victim = one_character( ch, argument, tmp, 
    (thing_array*) &player_list ) ) == NULL )
    return FALSE;

  if( victim == ch ) {
    send( ch, "You can't cast that on yourself!\r\n" );
    return FALSE;
    }

  cast->target = victim;
  return TRUE;
}


/*
 *   OBJECTS
 */


bool target_replicate( char_data* ch, cast_data* cast, char* argument )
{
  obj_data* obj;

  if( ( obj = one_object( ch, argument, "replicate", 
    &ch->contents ) ) == NULL )
    return FALSE;

  if( !is_empty( obj->contents )
    || ( obj->pIndexData->item_type == ITEM_DRINK_CON
    && obj->value[1] != 0 ) ) {
    include_empty = FALSE;
    send( ch, "The contents of %s would disrupt the spell.\r\n", obj );
    include_empty = TRUE;
    return FALSE;
    }

  if( is_set( obj->extra_flags, OFLAG_MAGIC ) ) {
    send( ch, "You are unable to copy magical items.\r\n" ); 
    return FALSE;
    }

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_REPLICATE ) ) {
    send( ch, "%s is too complex for you to replicate.\r\n", obj );
    return FALSE;
    }

  cast->target = obj;

  return TRUE;
}


bool target_weapon_armor( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, 
    &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_WEAPON 
    && obj->pIndexData->item_type != ITEM_ARMOR ) {
    send( ch, "You can only cast %s on weapons and armor.\r\n",
      spell_table[ cast->spell ].name );
    return FALSE;
    }

  cast->target = obj;

  return TRUE;
}


bool target_object( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, 
    &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  cast->target = obj;

  return TRUE;
}


bool target_weapon( char_data* ch, cast_data* cast, char* argument ) 
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, 
    &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_WEAPON ) {
    send( ch, "You can only cast %s on weapons.\r\n",
      spell_table[ cast->spell ].name );
    return FALSE;
    }

  cast->target = obj;

  return TRUE;
}


bool target_corpse( char_data* ch, cast_data* cast, char* argument )
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, 
    ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "%s can only be cast on corpses.\r\n",
      spell_table[ cast->spell ].name );
    return FALSE;
    }

  if( obj->pIndexData->item_type != ITEM_CORPSE ) {
    send( ch, "%s has no affect on player corpses yet.\r\n",
      spell_table[ cast->spell ].name );
    return FALSE;
    }

  cast->target = obj;

  return TRUE;
}


bool target_drink_con( char_data* ch, cast_data* cast, char* argument )
{
  char         tmp  [ ONE_LINE ];  
  obj_data*    obj;

  sprintf( tmp, "cast %s on", spell_table[ cast->spell ].name );

  if( ( obj = one_object( ch, argument, tmp, 
    &ch->contents, ch->array ) ) == NULL )
    return FALSE;

  if( obj->pIndexData->item_type != ITEM_DRINK_CON ) {
    send( ch, "You can only cast that on a drink container.\r\n" );
    return FALSE;
    }

  cast->target = obj;

  return TRUE;
}


/*
 *   MAIN ROUTINE
 */


bool get_target( char_data* ch, cast_data* cast, char* argument )
{
  int           spell  = cast->spell;
  int            type  = spell_table[spell].type;

  #define parm ch, cast, argument

  switch( type ) {
    case STYPE_REPLICATE      : return target_replicate      ( parm );
    case STYPE_SELF_ONLY      : return target_self_only      ( parm ); 
    case STYPE_PEACEFUL       : return target_peaceful       ( parm ); 
    case STYPE_OFFENSIVE      : return target_offensive      ( parm ); 
    case STYPE_CORPSE         : return target_corpse         ( parm ); 
    case STYPE_DRINK_CON      : return target_drink_con      ( parm ); 
    case STYPE_WEAPON         : return target_weapon         ( parm ); 
    case STYPE_WEAPON_ARMOR   : return target_weapon_armor   ( parm );
    case STYPE_WORLD          : return target_world          ( parm );
    case STYPE_MOB_ONLY       : return target_mob_only       ( parm );
    case STYPE_PEACEFUL_OTHER : return target_peaceful_other ( parm );
    case STYPE_OBJECT         : return target_object         ( parm );
    }

  #undef parm
 
  /*
  skip_spaces( argument );

  if( type == STYPE_RECALL ) {
    if( *argument == '\0' ) {
      cast->target = ch;
      return TRUE;
      }
    for( i = 0; i < MAX_ENTRY_TOWN ; i++ ) {     
      if( matches( argument, town_table[i].name ) ) {
        cast->room = get_room_index( town_table[i].recall );
        break;
        }
      }
    if( *argument == '\0' ) {
      cast->target = ch;
      } 
    else if( ( cast->target = one_character( ch, argument,
     "cast on", ch->array ) ) == NULL ) {
      return FALSE;
      }
    return TRUE;
    }

  if( type == STYPE_ANNOYING ) 
    return !in_sanctuary( ch );

  if( type == STYPE_PEACEFUL_OTHER || type == STYPE_MOB_ONLY ) {
    if( *argument == '\0' ) {
      if( ( victim = opponent( ch ) ) == NULL ) {
        send( ch, "Who do you want to cast that on?\r\n" );
        return FALSE;
	}
      }
    else if( ( victim = get_char_room( ch, argument ) ) == NULL ) {
      send( ch, "They aren't here.\r\n" );
      return FALSE;
      }
    if( victim == ch ) {
      send( ch, "That is not intended to be cast on yourself.\r\n" );
      }
    else if( type == STYPE_MOB_ONLY && victim->pcdata != NULL ) {
      send( ch, "You can only cast that on mobs.\r\n" );
      }
    else {
      cast->target = victim;
      return TRUE;
      }
    return FALSE;
    }

  if( type == STYPE_ROOM ) {
    if( *argument == '\0' ) {
      send( ch, "What location do you want to cast it on?\r\n" );
      }
    else if( atoi( argument ) != 0 ) {
      send( "What type of location is that??\r\n", ch );
      }
    else if( ( room = find_location( ch, argument ) ) == NULL ) {
      send( "Unknown Location.\r\n", ch );
      }
    else {
      cast->room = room;
      return TRUE;
      }
    return FALSE;
    }

  if( type == STYPE_OBJECT || type == STYPE_WEAPON
    || type == STYPE_WEAPON_ARMOR ) {
    if( *argument == '\0' ) {
      send( ch, "What object do you want to cast that on?\r\n" );
      }
    else if( ( obj = get_obj_inv( ch, argument ) ) == NULL
      && ( obj = get_obj_wear( ch, argument ) ) == NULL ) {
      send( ch, "You do not see that here.\r\n" );
      }
    else if( type == STYPE_WEAPON_ARMOR
      && obj->pIndexData->item_type != ITEM_WEAPON
      && obj->pIndexData->item_type != ITEM_ARMOR ) {
      send( ch, "You can only cast that on weapons and armor.\r\n" );
      }
    else if( type == STYPE_WEAPON
      && obj->pIndexData->item_type != ITEM_WEAPON ) {
      send( ch, "You can only cast that on a weapon.\r\n" );
      }
    else {
      for( i = 0; i < MAX_SPELL_WAIT; i++ )
        if( cast->reagent[i] == obj && obj->number == 1 ) {
          send( ch, "You can't cast a spell on the required reagent.\r\n" );
          return FALSE;
	  }
      if( type != STYPE_REPLICATE || can_replicate( ch, obj ) ) { 
        cast->target = obj;
        return TRUE;
        }
      }
    return FALSE;
    }

  if( ch->fighting != NULL ) {
    send( ch, "You cannot concentrate enough.\r\n" );
    return FALSE;
    }

  if( type == STYPE_ASTRAL ) {
    if( *argument == '\0' ) {
      send( ch, "At where do you wish to focus the astral gate?\r\n" );
      return FALSE;
      }
    for( i = 0; i < MAX_ENTRY_ASTRAL; i++ ) {     
      if( fmatches( argument, astral_table[i].name ) ) {
        cast->room = get_room_index( astral_table[i].recall );
        return TRUE;
        }
      }
    send( ch, "Unknown astral focus point.\r\n" );
    return FALSE;
    }

  bug( "Get_Target: Unknown spell type." );
  */
  return FALSE;
}


