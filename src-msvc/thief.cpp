#include "define.h"
#include "struct.h"


bool can_hide      ( char_data*, bool );

void do_gouge( char_data* ch, char* argument )
{
  /*
  char_data* victim;
  int  delay;

  if( is_confused_pet( ch ) )
    return;
  
  if( is_mob( ch ) )
    return;

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not gouge with parry on.\r\n" );
    return;
    }
    
  if( ch->get_skill( SKILL_EYE_GOUGE )  < 1 ) {
    send( ch, "You do not know how to gouge eyes.\r\n" );
    return;
    }

  if( victim == ch ) {
    send( ch, "Gouging your own eyes is not very productive.\r\n" );
    return;
    }

  if( ( victim = get_victim( ch, argument, "Gouge whose eyes?\r\n" ) ) ==
    NULL )
    return;


  if( victim->species != NULL 
    && !is_set( &victim->species->act_flags, ACT_HAS_EYES ) ) {
    send( ch,  "This creature has no eyes and cannot be blinded.\r\n" );
    return;
  }

  if( victim->fighting != ch && ch->fighting != victim ) {
    if( !can_kill( ch, victim ) )
      return;
    check_killer( ch, victim );
    }


  delay = gouge_attack( ch, victim );

  start_fight( ch, victim );
  set_attack( ch, victim, delay );

  remove_bit( &ch->status, STAT_LEAPING );
  remove_bit( &ch->status, STAT_WIMPY );

  ch->improve_skill( SKILL_EYE_GOUGE );
  */
  return;

}


int gouge_attack( char_data* ch, char_data* victim )
{
  int            roll;

  roll = number_range( 0, 20 )+
    +ch->get_skill( SKILL_EYE_GOUGE )/2
    +(ch->shdata->dexterity-victim->shdata->dexterity)/2
    +( ( ch->shdata->level-victim->shdata->level )/4);


  if( roll < 6 ) {  
      send( ch, "You attempt to gouge %s but are unsuccessful.\r\n", victim );
      send( victim, "%s attempts to gouge you but is unsuccessful.\r\n", ch );
      send( *ch->array, 
        "%s attempts to gouge %s but is unsuccessful.\r\n",
        ch, victim );
    return 32;
    }

  if( roll > 20 ) {
    send( ch, "You gouge %s in the eye!!\r\n", victim );
    send( victim, "%s gouges you in the eye!!\r\n", ch );
    send( *ch->array, "%s gouges %s in the eye!!\r\n", ch, victim );
    set_bit( victim->affected_by, AFF_BLIND );

    disrupt_spell( victim ); 
    set_delay( victim, 32 );
    return 20;
    }


    send( ch, "You attempt to gouge %s but fail.\r\n", victim );
    send( victim, "%s attempts to gouge you but fails.\r\n", ch );
    send( *ch->array, 
      "%s attempts to gouge %s but fails.\r\n", ch, victim );
    return 20;
  
  
}

void do_disguise( char_data*, char* )
{
  return;
}


void do_garrote( char_data* ch, char* argument )
{
  /*
  char_data* victim;
  int      level  = ch->get_skill( SKILL_GARROTE );
  obj_data* garrote;

  for( garrote = ch->contents; garrote != NULL; garrote->next_content )
    if( garrote->pIndexData->item_type == ITEM_GARROTE )
      break;

  if( garrote == NULL ) {
    send( ch, "You don't have a garrote.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "You have no target for your garrote.\n\b");
    return;
    }
  
  if( level == 0 ) {
    send( "Garrote is not part of your repertoire\r\n", ch );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;
  
  set_bit( victim->affected_by, AFF_CHOKING );
  send( victim, "A garotte begins to tighten around your neck.\r\n" );
  send( *victim->array, "The garrote tightens around %s neck.\r\n", victim ); 
  
  start_fight( ch, victim );
  set_attack( ch, victim, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  set_delay( victim, 32 );
  ch->improve_skill( SKILL_GARROTE );
 
  attack( ch, victim, "garrote", garrote, -1, 0 );
  */
  
  return;
}


/* 
 *   BACKSTAB FUNCTIONS
 */


void do_backstab( char_data* ch, char* argument )
{
  /*
  char_data*  victim;
  obj_data*      obj;
  int          skill  = ch->get_skill( SKILL_BACKSTAB );

  if( ch->mount != NULL ) {
    send( ch, "Backstabbing while mounted is beyond your skill.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "Backstab whom?\r\n" );
    return;
    }

  if( ( victim = get_char_room( ch, argument, TRUE ) ) == NULL ) 
    return;

  if( opponent( ch ) != NULL ) {
    send( "You are already fighting someone.\r\n", ch );
    return;
    }

  if( victim == ch ) {
    send( "How can you sneak up on yourself?\r\n", ch );
    return;
    }

  if( skill == 0 ) {
    send( "Backstabbing is not part of your repertoire.\r\n", ch );
    return;
    }

  if( ( obj = get_eq_char( ch, WEAR_HELD_R ) ) == NULL ) {
    send( "You need to be wielding a weapon to backstab.\r\n", ch );
    return;
    }

  if( !is_set( obj->pIndexData->extra_flags, OFLAG_BACKSTAB ) ) {
    send( ch, "It isn't possible to use %s to backstab.\r\n", obj );
    return;
    }

  if( opponent( ch ) != NULL ) {
    send( ch, "You are unable to backstab while fighting someone.\r\n" );
    return;
    }

  if( ch->Seen( victim ) && includes( victim->aggressive, ch ) ) {
    send( ch, "%s is too wary of you for backstab to succeed.\r\n", victim );
    return;
    }

  if( !can_kill( ch, victim ) )
    return;

  check_killer( ch, victim );
  start_fight( ch, victim );
  set_attack( ch, victim, 20 );

  remove_bit( &ch->status, STAT_WIMPY );
  remove_bit( &ch->status, STAT_LEAPING );

  attack( ch, victim, "backstab", obj, -1, 0 );
  */
  return;
}


/*
 *   STEAL ROUTINES
 */


void do_steal( char_data* ch, char* argument )
{
  /*
  char           buf  [ MAX_INPUT_LENGTH ];
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  obj_data*      obj;

  if( is_confused_pet( ch ) )
    return;
 
  if( is_mob( ch ) )
    return;
 
  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not steal with parry on.\r\n" );
    return;
    }

  argument = one_argument( argument, arg );

  for( ; ; ) {
    argument = one_argument( argument, buf );
    if( buf[ 0 ] == '\0' || !strcasecmp( buf, "from" ) )
      break;
    sprintf( arg+strlen( arg ), " %s", buf );
    }

  if( arg[0] == '\0' || argument[0] == '\0' ) {
    send( "Syntax: steal <object> from <character>\r\n", ch );
    return;
    }

  if( ( victim = get_char_room( ch, argument, TRUE ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( "That's pointless.\r\n", ch );
    return;
    }

  if( !can_kill( ch, victim ) ) {
    send( "You can't steal from them.\r\n", ch );
    return; 
    } 

  remove_bit( ch->pcdata->pfile->flags, PLR_PARRY );

  if( !ch->check_skill( SKILL_STEAL )
    || number_range( 3, 35 ) < victim->Intelligence( ) ) {
    leave_shadows( ch );
    start_fight( ch, victim );
    set_attack( ch, victim, number_range( 5, 20 ) );
    set_delay( ch, 32 );
 
    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    send( victim, "%s tried to steal from you.\r\n", ch );
    send( *ch->array, "%s tried to steal from %s.\r\n", ch, victim );
    modify_reputation( ch, victim, REP_STOLE_FROM );
    if( victim->pShop != NULL ) {
      sprintf( buf, "Guards! %s is a thief.", ch->Name( victim ) );
      do_yell( victim, buf );
      summon_help( victim, ch );
      }
    return;
    }

  if( ( obj = get_obj_inv( victim, arg ) ) == NULL ) {
    send( "You can't find it.\r\n", ch );
    return;
    }
    
  if( !obj->droppable( ) ) {
    send( ch, "You can't pry it away.\r\n" );
    return;
    }

  if( ch->num_ins >= ch->can_carry_n( ) ) {
    send( "You have your hands full.\r\n", ch );
    return;
    }

  if( ch->wght_ins+weight( obj ) > ch->can_carry_w( ) ) {
    send( "You can't carry that much weight.\r\n", ch );
    return;
    }

  obj = remove( obj, 1 );
  put_obj( obj, ch );
  send( ch, "You succeeded in stealing %s.\r\n", obj );
  ch->improve_skill( SKILL_STEAL );
  */
  return;
}


void do_heist( char_data* ch, char* argument )
{
  /*
  char           buf  [ MAX_INPUT_LENGTH ];
  char           arg  [ MAX_INPUT_LENGTH ];
  char_data*  victim;
  obj_data*      obj;

  if( is_confused_pet( ch ) )
    return;
 
  if( is_mob( ch ) )
    return;
 
  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) ) {
    send( ch, "You can not steal with parry on.\r\n" );
    return;
    }

  argument = one_argument( argument, arg );

  for( ; ; ) {
    argument = one_argument( argument, buf );
    if( buf[ 0 ] == '\0' || !strcasecmp( buf, "from" ) )
      break;
    sprintf( arg+strlen( arg ), " %s", buf );
    }

  if( arg[0] == '\0' || argument[0] == '\0' ) {
    send( "Syntax: steal <object> from <character>\r\n", ch );
    return;
    }

  if( ( victim = get_char_room( ch, argument, TRUE ) ) == NULL ) 
    return;

  if( victim == ch ) {
    send( "That's pointless.\r\n", ch );
    return;
    }

  if( !can_kill( ch, victim ) ) {
    send( "You can't steal from them.\r\n", ch );
    return; 
    } 

  remove_bit( ch->pcdata->pfile->flags, PLR_PARRY );

  if( !ch->check_skill( SKILL_STEAL )
    || number_range( 3, 35 ) < victim->Intelligence( ) ) {
    leave_shadows( ch );
    start_fight( ch, victim );
    set_attack( ch, victim, number_range( 5, 20 ) );
    set_delay( ch, 32 );
 
    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    send( victim, "%s tried to steal from you.\r\n", ch );
    send( *ch->array, "%s tried to steal from %s.\r\n", ch, victim );
    modify_reputation( ch, victim, REP_STOLE_FROM );
    if( victim->pShop != NULL ) {
      sprintf( buf, "Guards! %s is a thief.", ch->Name( victim ) );
      do_yell( victim, buf );
      summon_help( victim, ch );
      }
    return;
    }

  if( ( obj = get_obj_inv( victim, arg ) ) == NULL ) {
    send( "You can't find it.\r\n", ch );
    return;
    }
    
  if( !obj->droppable( ) ) {
    send( ch, "You can't pry it away.\r\n" );
    return;
    }

  if( ch->num_ins >= ch->can_carry_n( ) ) {
    send( "You have your hands full.\r\n", ch );
    return;
    }

  if( ch->wght_ins+weight( obj ) > ch->can_carry_w( ) ) {
    send( "You can't carry that much weight.\r\n", ch );
    return;
    }

  obj = remove( obj, 1 );
  put_obj( obj, ch );
  send( ch, "You succeeded in stealing %s.\r\n", obj );
  ch->improve_skill( SKILL_STEAL );
  */
  return;
}

 
/*
 *   SNEAK FUNCTIONS
 */


void do_sneak( char_data* ch, char* argument )
{
  if( not_player( ch ) )
    return;

  if( ch->shdata->skill[ SKILL_SNEAK ] == 0 ) {
    send( ch, "Sneaking is not something you are adept at.\r\n" );
    return;
    }

  if( is_set( ch->pcdata->pfile->flags, PLR_SNEAK ) ) {
    remove_bit( ch->pcdata->pfile->flags, PLR_SNEAK );
    send( ch, "You stop sneaking.\r\n" );
    return;
    }

  if( !can_hide( ch, TRUE ) )
    return;

  set_bit( ch->pcdata->pfile->flags, PLR_SNEAK );

  send( ch, "You start sneaking.\r\n" );
  send( ch, "[Sneak increases movement point cost by 2 per move.]\r\n" );
}


/*
 *   HIDE ROUTINES
 */


bool can_hide( char_data* ch, bool msg )
{
  room_data*  room  = ch->in_room;

  if( ch->mount != NULL ) {
    if( msg ) 
      send( ch, "Hiding while mounted is impossible.\r\n" );
    return FALSE;
    }

  if( ch->fighting != NULL ) {
    if( msg )
      send( ch, "Hiding while fighting someone is difficult.\r\n" );
    return FALSE;
    }

  if( is_set( ch->affected_by, AFF_FIRE_SHIELD ) 
    || is_set( ch->affected_by, AFF_FAERIE_FIRE ) ) {
    if( msg ) 
      send( ch, "Your fiery glow rathers spoils that.\r\n" );
    return FALSE;
    }

  if( water_logged( room ) ) {
    if( msg )
      send( ch, "Hiding is unsoluble.\r\n" );
    return FALSE;
    }
    
  return TRUE;
}


void do_camouflage( char_data* ch, char* )
{
  /*
  char_data* rch;

  if( not_player( ch ) )
    return;

  if( ch->shdata->skill[ SKILL_CAMOUFLAGE ] == 0 ) {
    send( ch, "Camouflage is not something you are adept at.\r\n" );
    return;
    }

  if( leave_camouflage( ch ) )
    return;

  if( !can_hide( ch, TRUE ) )
    return;

  for( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    if( rch != ch && ch->Seen( rch ) )
      add( ch->seen_by, rch );

  send( ch, "You camouflage yourself and disappear from plain view.\r\n" );
  fsend_seen( ch, "%s tries to blend in with %s surroundings.\r\n", ch,
    ch->His_Her( )  );
  
  set_bit( &ch->status, STAT_CAMOUFLAGED );
  set_bit( ch->affected_by, AFF_CAMOUFLAGE );

  ch->improve_skill( SKILL_CAMOUFLAGE );
  */

  return;
}


void do_hide( char_data* ch, char* )
{
  char_data* rch;

  if( not_player( ch ) )
    return;

  if( ch->shdata->skill[ SKILL_HIDE ] == 0 ) {
    send( ch, "Hiding is not something you are adept at.\r\n" );
    return;
    }

  if( leave_shadows( ch ) )
    return;

  if( !can_hide( ch, TRUE ) )
    return;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL
      && rch != ch && ch->Seen( rch ) )
      ch->seen_by += rch;

  send( ch, "You step into the shadows.\r\n" );
  fsend_seen( ch, "%s steps into the shadows attempting to hide.\r\n", ch );
  
  set_bit( &ch->status, STAT_HIDING );

  ch->improve_skill( SKILL_HIDE );
}


bool leave_camouflage( char_data* ch )
{
  if( !is_set( &ch->status, STAT_CAMOUFLAGED ) 
    && !is_set( ch->affected_by, AFF_CAMOUFLAGE ) ) 
    return FALSE;

  remove_bit( &ch->status, STAT_CAMOUFLAGED );
  remove_bit( ch->affected_by, AFF_CAMOUFLAGE );

  clear( ch->seen_by );

  send( ch, "You stop camouflaging yourself.\r\n" );
  send_seen( ch, "%s suddenly appears from nowhere.\r\n", ch );

  return TRUE;
}


bool leave_shadows( char_data* ch )
{
  if( !is_set( &ch->status, STAT_HIDING ) )
    return FALSE;

  remove_bit( &ch->status, STAT_HIDING );

  clear( ch->seen_by );

  send( ch, "You stop hiding.\r\n" );
  send_seen( ch, "%s steps from the shadows.\r\n", ch );
  return TRUE;
}


/* 
 *   DIP ROUTINE
 */


void do_dip( char_data* ch, char* argument )
{
  char              arg  [ MAX_INPUT_LENGTH ];
  obj_data*   container;
  obj_data*         obj;
  int             value;
  affect_data    affect;
  int             spell;

  if( !two_argument( argument, "from", arg ) ) {
    send( ch, "Syntax: Dip <object> [into] <object>\r\n" );
    return;
    }

  if( ( obj = one_object( ch, arg, "dip",
    &ch->contents, ch->array ) ) == NULL ) 
    return;

  if( ( container = one_object( ch, argument, "dip into",
    &ch->contents, ch->array ) ) == NULL ) 
    return;

  if( container->pIndexData->item_type != ITEM_DRINK_CON
    && container->pIndexData->item_type != ITEM_FOUNTAIN ) {
    send( ch, "%s isn't something you can dip things into.\r\n", container );
    return;
    }

  if( container == obj ) {
    send( ch, "You can't dip %s into itself.\r\n", obj );
    return;
    }

  value               = container->value[1];
  container->value[1] = -2;

  if( value == 0 ) {
    send( ch, "%s is empty.\r\n", container );
    container->value[1] = value;
    return;
    }

  if( strip_affect( obj, AFF_BURNING ) ) {
    fsend( ch, "You extinguish %s by quickly dipping it into %s.",
      obj, container );
    fsend( *ch->array,
      "%s extinguishes %s by quickly dipping it into %s.",
      ch, obj, container );
    return;
    } 

  send( ch, "You dip %s into %s.\r\n", obj, container );
  send_seen( ch, "%s dips %s into %s.\r\n", ch, obj, container );
  container->value[1] = ( value == -1 ? -1 : max( 0, value-5 ) );

  if( ( spell = liquid_table[container->value[2]].spell ) == -1 ) 
    return;

  if( spell < SPELL_FIRST || spell >= WEAPON_FIRST ) {
    bug( "Do_dip: Liquid with non-spell skill." );
    return;
    }

  ( *spell_table[spell-SPELL_FIRST].function )( ch, NULL, obj, 10, -3 ); 

  return;
}  





