#include "define.h"
#include "struct.h"


char *consider_string      ( double ratio );
bool scan_room             ( char_data*, room_data*, char* );


int skill_index( const char* name )
{
  int i;

  if( *name != '\0' )
    for( i = 0; i < MAX_SKILL; i++ )
      if( !strcasecmp( name, skill_table[i].name ) )
       return i;

  return -1;
}


int char_data :: get_skill( int i )
{
  if( species != NULL ) {
    switch( i ) {
     case SKILL_CLIMB :
      return( IS_SET( species->act_flags, ACT_CAN_CLIMB ) ? 10 : 0 );
     default :
      return 10;
     }
    }

  if( i < 0 || i > MAX_SKILL )
    return 0;

  return shdata->skill[i];
}
   
   
bool char_data :: check_skill( int i )
{
  if( pcdata == NULL )
    return TRUE;

  if( shdata->skill[i] == UNLEARNT 
    || number_range( 1, 10 ) > shdata->skill[ i ] )
    return FALSE;

  return TRUE;
}


/*
 *   BATTLE SKILLS
 */


void do_consider( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    send( ch, "Who do you want to consider killing?\r\n" );
    return;
    }

  if( ( victim = one_character( ch, argument, "consider",
    ch->array ) ) == NULL )
    return;

  if( IS_AFFECTED( ch, AFF_HALLUCINATE ) ) {
    send( ch, "Hah! Death to all grid bugs!\r\n" );
    return;
    }

  if( victim->species != NULL && victim->shdata->deaths == 0 ) {
    send( ch,
      "You look at %s but are unable to determine how difficult %s is.\r\n",
      victim, victim->He_She( ) );
    }  
  else { 
    send( consider_string( ( double ) xp_compute( victim )
      /xp_compute( ch ) ), ch );
    }

  if( victim->species != NULL )
    send( ch, "[ Nation: %s ]\r\n",
      nation_table[ victim->species->nation ].name );

  return;
}


char* consider_string( double ratio )
{
  if( ratio > 16 )  return "Attacking and suicide are equivalent.\r\n";
  if( ratio > 8 )   return "You would require divine intervention.\r\n";
  if( ratio > 4 )   return "You don't have much of a hope.\r\n";
  if( ratio > 2 )   return "You'll need a lot of help.\r\n";
  if( ratio > 1.5 ) return "With luck on your side you might prevail.\r\n";
  if( ratio > 1 )   return "It will be a tough fight.\r\n";
  
  if( 1./ratio > 5 ) return "An unworthy opponent.\r\n";
  if( 1./ratio > 2 ) return "Should be an easy kill.\r\n";
  if( 1./ratio > 1.5 ) return "You shouldn't have much difficulty.\r\n";
  
  return "It looks like a fair fight.\r\n";
}
 

void do_inspect( char_data* ch, char* argument )
{
//  obj_data*   obj;
//  obj_data*  trap;

  if( is_confused_pet( ch ) )
    return;
  /*
  if( ( obj = get_obj_here( ch, argument ) ) == NULL ) {
    send( "What do you want to inspect?\r\n", ch );
    return;
    }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( "That's not a container.\r\n", ch );
    return;
    }

  if( !is_set( &obj->value[1], CONT_CLOSED ) ) {
    send( ch, "It's open - just look inside it.\r\n" );
    return;
    }

  for( trap = obj->contents; trap != NULL; trap = trap->next_content ) 
    if( trap->pIndexData->item_type == ITEM_TRAP
      && trap->Seen( ch ) )
      break;

  if( trap == NULL ) {
    send( ch, "You see nothing unusual.\r\n" );
    return;
    }

  send( ch, "YIKES! You find %s.\r\n", trap );        

  ch->improve_skill( SKILL_INSPECT );
  */
  return;
}


void do_untrap( char_data *ch, char *argument )
{
  char arg [ MAX_INPUT_LENGTH ];

//  obj_data *obj, *trap;
//  oprog_data *oprog;

  if( is_confused_pet( ch ) )
    return;

  argument = one_argument( argument, arg );

  if( *arg == '\0' ) {
    send( "What do you want to try to remove a trap from?\r\n", ch );
    return;
    }
  
  if( ch->species != NULL ) {
    send( "Only players can untrap.\r\n", ch );
    return;
    }
  /*
  if( ( obj = get_obj_here( ch, arg ) ) == NULL ) {
    send( "What do you want to remove a trap from?\r\n", ch );
    return;
    }
 
  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next )
    if( oprog->trigger == OPROG_TRIGGER_DISARM ) {
      var_obj  = obj;
      var_ch   = ch;    
      var_room = ch->in_room;
      execute( oprog ); 
      return;
      }

  if( obj->pIndexData->item_type != ITEM_CONTAINER ) {
    send( "That's not a container.\r\n", ch );
    return;
    }

  if( !IS_SET( obj->value[1], CONT_CLOSED ) ) {
    send( "It's open - just look inside it.\r\n", ch );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "What trap do you want to disarm?\r\n" );
    return;
    }

  for( trap = obj->contents; trap != NULL; trap = trap->next_content ) 
    if( trap->pIndexData->item_type == ITEM_TRAP
      && trap->Seen( ch ) )
      break;

  if( trap == NULL ) {
    send( ch, "You find no traps to disarm.\r\n" );
    return;
    }

  send( ch, "You disarm %s!!\r\n", trap );
  extract( trap );

  ch->improve_skill( SKILL_UNTRAP );
  */
  return;
}


void do_compare( char_data* ch, char* argument )
{
  char           arg  [ MAX_INPUT_LENGTH ];
  obj_data*     obj1;
  obj_data*     obj2;
  int          flags;

  if( !get_flags( ch, argument, &flags, "sS", "compare" ) )
    return;

  if( !two_argument( argument, "with", arg ) ) {
    send( ch, "Syntax: compare <obj1> [with] <obj2>.\r\n" );
    return;
    }  

  /*
  if( flags != 0 ) {
    if( ( keeper = find_keeper( ch ) ) == NULL )
      return;
    if( is_set( &ch->in_room->room_flags, RFLAG_PET_SHOP ) ) {
      send( ch, "You can't compare pets yet.\r\n" );
      return;
      }
    }
  */
    
  if( ( obj1 = one_object( ch, arg, "compare", &ch->contents ) ) == NULL )  
    return;

  if( ( obj2 = one_object( ch, argument, "compare", &ch->contents ) ) == NULL )  
    return;

  if( obj1 == obj2 ) {
    fsend( ch,
      "You compare %s with itself coming to no useful conclusion.",
      obj1 );
    return;
    }

  if( obj1->pIndexData->item_type != obj2->pIndexData->item_type ) {
    send( ch, "Objects must be of same type.\r\n" );
    return;
    }

  if( obj1->pIndexData->item_type == ITEM_ARMOR ) {
    int armor1 = armor_class( obj1 );
    int armor2 = armor_class( obj2 );
  
    if( ( obj1->pIndexData->wear_flags & obj2->pIndexData->wear_flags
      & ~( 1 << ITEM_TAKE ) ) == 0 ) {
      send( "Those items do not get worn on the same location, so\
 comparing them is not a\r\nuseful exercise.\r\n", ch );
      return;
      }

    if( armor1 == armor2 ) {
      fsend( ch, "You think %s and %s would offer similar protection.\r\n",
        obj1, obj2 );
      return;
      }

    if( armor1 < armor2 )
      swap( obj1, obj2 );

    fsend( ch, "You think %s would offer better protection than %s.",
      obj1, obj2 );
    return;
    }

  if( obj1->pIndexData->item_type == ITEM_WEAPON ) {
    int dam1 = obj1->value[1]*(obj1->value[2]+1)+2*obj1->value[0];
    int dam2 = obj2->value[1]*(obj2->value[2]+1)+2*obj2->value[0];

    if( dam1 == dam2 ) {
      fsend( ch, "You think %s and %s would do similar damage.",
        obj1, obj2 );
      return;
      }

    if( dam1 < dam2 ) 
      swap( obj1, obj2 );

    fsend( ch, "You think %s would do more damage than %s.",
      obj1, obj2 );
    return;
    }

  send( ch, "You can only compare weapons and armor.\r\n" );

  return;
}


void do_appraise( char_data *ch, char *argument )
{
  /*
  obj_data* obj;

  if( *argument == '\0' ) {
    send( ch, "Appraise what?\r\n" );
    return;
    }

  if( ( obj = get_obj_inv( ch, argument ) ) == NULL ) {
    send( ch, "You do not have that item.\r\n" );
    return;
    }

  if( IS_NPC( ch ) || ch->shdata->skill[ SKILL_APPRAISE ] == UNLEARNT ) {
    send( ch, "You know no more about an object's value than what you have learnt from\r\nlooking in the shops.\r\n" );
    return;
    }

  if( !is_set( obj->extra_flags, OFLAG_APPRAISED ) ) {
    ch->check_skill( SKILL_APPRAISE );
    set_bit( obj->extra_flags, OFLAG_APPRAISED );
    }

  if( obj->pIndexData->cost >
    ch->shdata->skill[ SKILL_APPRAISE ]*ch->shdata->skill[ SKILL_APPRAISE ]
    *ch->shdata->level ) {
    send( ch, "You think %s is valuable but are unsure how valuable.\r\n",
      obj );
    return;
    }

  if( obj->pIndexData->cost <= 0 ) {
    send( ch, "%s looks worthless to you.\r\n", obj );
    return;
    }

  send( ch, "You think %s would be worth about %d cp.\r\n",
    obj, obj->pIndexData->cost );

  ch->improve_skill( SKILL_APPRAISE );
  */
  return;
}


void do_bandage( char_data *ch, char *argument )
{
  /*
  char_data *victim;
  obj_data *obj;

  if( argument[0] == '\0' ) {
    send( "Bandage who?\r\n", ch );
    return;
    }

  if( ( victim = get_char_room( ch, argument ) ) == NULL ) {
    send( "They aren't here.\r\n", ch );
    return;
    }

  if( ch == victim ) {
    send(
      "If you are able to bandage you don't need to be bandaged.\r\n", ch );
    return;
    }

  if( victim->hit > 0 ) {
    send( "They don't need bandaging.\r\n", ch );
    return;
    }

  if( ch->position == POS_FIGHTING ) {
    send( "You can't bandage someone while fighting!\r\n", ch );
    return;
    }

  if( IS_NPC( ch ) || ch->shdata->skill[ SKILL_BANDAGE ] == UNLEARNT ) {
    send( "You really wouldn't know how to bandage wounds.\r\n", ch );
    return;
    }

  for( obj = ch->contents; obj != NULL; obj = obj->next_content ) 
    if( obj->pIndexData->item_type == ITEM_BANDAGE
      && obj->wear_loc == WEAR_NONE )
      break;

  if( obj == NULL ) {
    send( "You don't have anything to bandage them with!\r\n", ch );
    return;
    }
  
  obj->selected = 1;

  if( ch->check_skill( SKILL_BANDAGE ) ) {
    send( ch,
      "You use %s to bandage %s's wounds stabilizing %s injuries.\r\n",
      obj, victim, victim->His_Her( ) );
    act_neither(
      "$n uses a $p to bandage $N wounds stabilizing $S injuries.",
      ch, victim, obj );
    victim->hit = 1;
    update_pos( victim );
    ch->improve_skill( SKILL_BANDAGE );
    }
  else {
    send( ch,
      "You try to bandage %s's wounds but just make the situation worse.\r\n",
      victim );
    act( "$n tries to bandage $N's wounds but does more harm than good.",
      ch, NULL, victim, TO_NOTVICT );
    }

  obj->Extract( 1 );
  */  
  return;
}



















