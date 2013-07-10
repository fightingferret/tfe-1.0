#include "define.h"
#include "struct.h"


const char* arg_type_name[] = { "none", "any", "string", "integer",
  "character", "object", "room", "direction", "clan", "skill", "rflag",
  "stat", "class", "religion", "race", "thing" };


/*
 *   FUNCTION TABLE
 */


#define nn NONE
#define st STRING
#define ch CHARACTER
#define ob OBJECT
#define in INTEGER
#define rm ROOM
#define sk SKILL
#define sa STAT
#define di DIRECTION
#define rf RFLAG
#define na NATION
#define cl CLASS
#define re RELIGION
#define rc RACE
#define th THING

cfunc_type cfunc_list [] =
{
  { "acode",           &code_acode,         nn, { rm, in, nn, nn } },
  { "act_area",        &code_act_area,      nn, { st, ch, ob, ch } },
  { "act_room",        &code_act_room,      nn, { st, ch, ob, ch } },
  { "act_notchar",     &code_act_notchar,   nn, { st, ch, ob, ch } },
  { "act_notvict",     &code_act_notchar,   nn, { st, ch, ob, ch } },
  { "act_tochar",      &code_act_tochar,    nn, { st, ch, ob, ch } },
  { "act_neither",     &code_act_neither,   nn, { st, ch, ob, ch } },
  { "assign_quest",    &code_assign_quest,  nn, { ch, in, nn, nn } },
  { "attack",          &code_attack,        in, { ch, ch, in, st } },
  { "attack_acid",     &code_attack_acid,   in, { ch, ch, in, st } },
  { "attack_cold",     &code_attack_cold,   in, { ch, ch, in, st } },
  { "attack_fire",     &code_attack_fire,   in, { ch, ch, in, st } },
  { "attack_shock",    &code_attack_shock,  in, { ch, ch, in, st } },
  { "attack_room",     &code_attack_room,   nn, { ch, in, st, nn } },
  { "attack_weapon",   &code_attack_weapon, in, { ch, ch, in, st } },
  { "can_fly",         &code_can_fly,       in, { ch, nn, nn, nn } },
  { "cast_spell",      &code_cast_spell,    nn, { sk, ch, ch, nn } },
  { "char_in_room",    &code_char_in_room,  in, { ch, rm, nn, nn } },
  { "cflag",           &code_cflag,         in, { in, ch, nn, nn } },
  { "class",           &code_class,         cl, { ch, nn, nn, nn } },
  { "close",           &code_close,         nn, { rm, di, nn, nn } },
  { "coin_value",      &code_coin_value,    in, { ob, nn, nn, nn } },
  { "dam_message",     &code_dam_message,   nn, { ch, in, st, nn } },
  { "dice",            &code_dice,          in, { in, in, in, nn } },
  { "disarm",          &code_disarm,        in, { ch, in, nn, nn } },
  { "do_spell",        &code_do_spell,      nn, { sk, ch, ch, nn } },
  { "doing_quest",     &code_doing_quest,   in, { ch, in, nn, nn } },
  { "done_quest",      &code_done_quest,    in, { ch, in, nn, nn } },
  { "drain_exp",       &code_drain_exp,     nn, { ch, in, nn, nn } },
  { "drain_stat",      &code_drain_stat,    nn, { ch, sa, in, in } },
  { "find_room",       &code_find_room,     rm, { in, nn, nn, nn } },
  { "find_skill",      &code_find_skill,    in, { ch, sk, nn, nn } },
  { "find_stat",       &code_find_stat,     in, { ch, sa, nn, nn } },
  { "get_room",        &code_get_room,      rm, { ch, nn, nn, nn } },
  { "junk_mob",        &code_junk_mob,      nn, { ch, nn, nn, nn } },
  { "junk_obj",        &code_junk_obj,      nn, { ob, in, nn, nn } },
  { "has_obj",         &code_has_obj,       ob, { in, ch, nn, nn } },
  { "has_quest",       &code_has_quest,     in, { ch, in, nn, nn } },
  { "heal",            &code_heal,          nn, { ch, in, nn, nn } },
  { "inflict",         &code_inflict,       in, { ch, ch, in, st } },
  { "inflict_acid",    &code_inflict_acid,  in, { ch, ch, in, st } },
  { "inflict_cold",    &code_inflict_cold,  in, { ch, ch, in, st } },
  { "inflict_fire",    &code_inflict_fire,  in, { ch, ch, in, st } },
  { "inflict_shock",   &code_inflict_shock, in, { ch, ch, in, st } },
  { "interpret",       &code_interpret,     nn, { ch, st, ch, nn } },
  { "is_exhausted",    &code_is_exhausted,  in, { ch, in, nn, nn } },
  { "is_fighting",     &code_is_fighting,   in, { ch, nn, nn, nn } },
  { "is_follower",     &code_is_follower,   in, { ch, nn, nn, nn } },
  { "is_locked",       &code_is_locked,     in, { rm, di, nn, nn } },
  { "is_mounted",      &code_is_mounted,    in, { ch, nn, nn, nn } },
  { "is_name",         &code_is_name,       in, { st, st, nn, nn } },
  { "is_open",         &code_is_open,       in, { rm, di, nn, nn } },
  { "is_player",       &code_is_player,     in, { ch, nn, nn, nn } },
  { "is_resting",      &code_is_resting,    in, { ch, nn, nn, nn } },
  { "is_searching",    &code_is_searching,  in, { ch, nn, nn, nn } },
  { "is_silenced",     &code_is_silenced,   in, { ch, nn, nn, nn } },
  { "lock",            &code_lock,          nn, { rm, di, nn, nn } },
  { "num_in_room",     &code_num_in_room,   in, { rm, nn, nn, nn } },
  { "num_mob",         &code_num_mob,       in, { in, rm, nn, nn } },
  { "mpcode",          &code_mpcode,        nn, { ch, in, nn, nn } },
  { "mload",           &code_mload,         ch, { in, rm, nn, nn } },
  { "mob_in_room",     &code_mob_in_room,   ch, { in, rm, nn, nn } },
  { "modify_mana",     &code_modify_mana,   in, { ch, in, nn, nn } },
  { "modify_move",     &code_modify_move,   in, { ch, in, nn, nn } },
  { "obj_name",        &code_obj_name,      st, { ob, nn, nn, nn } },
  { "obj_in_room",     &code_obj_in_room,   ob, { in, rm, nn, nn } },
  { "obj_to_room",     &code_obj_to_room,   nn, { ob, rm, nn, nn } },
  { "obj_to_char",     &code_obj_to_char,   nn, { ob, ch, nn, nn } },
  { "obj_value",       &code_obj_value,     in, { ob, in, nn, nn } },
  { "oload",           &code_oload,         ob, { in, nn, nn, nn } },
  { "opcode",          &code_opcode,        nn, { ob, in, nn, nn } },
  { "open",            &code_open,          nn, { rm, di, nn, nn } },
  { "plague",          &code_plague,        nn, { ch, nn, nn, nn } },
  { "players_area",    &code_players_area,  in, { rm, nn, nn, nn } },
  { "players_room",    &code_players_room,  in, { rm, nn, nn, nn } },
  { "poison",          &code_poison,        nn, { ch, nn, nn, nn } },
  { "race",            &code_race,          rc, { ch, nn, nn, nn } },
  { "random",          &code_random,        in, { in, in, nn, nn } },
  { "rand_char",       &code_rand_char,     ch, { rm, nn, nn, nn } },
  { "rand_player",     &code_rand_player,   ch, { rm, nn, nn, nn } },
  { "remove_cflag",    &code_remove_cflag,  nn, { in, ch, nn, nn } },
  { "remove_rflag",    &code_remove_rflag,  nn, { rf, rm, nn, nn } },
  { "reputation",      &code_reputation,    in, { ch, na, nn, nn } },
  { "rflag",           &code_rflag,         in, { rf, rm, nn, nn } },
  { "send_to_area",    &code_send_to_area,  nn, { st, rm, nn, nn } },
  { "send_to_char",    &code_send_to_char,  nn, { st, ch, nn, nn } },
  { "send_to_room",    &code_send_to_room,  nn, { st, rm, nn, nn } },
  { "set_cflag",       &code_set_cflag,     nn, { in, ch, nn, nn } },
  { "set_religion",    &code_set_religion,  nn, { ch, re, nn, nn } },
  { "set_rflag",       &code_set_rflag,     nn, { rf, rm, nn, nn } },
  { "show",            &code_show,          nn, { ch, rm, in, nn } }, 
  { "size",            &code_size,          in, { ch, nn, nn, nn } }, 
  { "summon",          &code_summon,        nn, { ch, ch, nn, nn } },
  { "tell",            &code_tell,          nn, { ch, ch, st, nn } },
  { "transfer",        &code_transfer,      nn, { ch, rm, nn, nn } },
  { "transfer_all",    &code_transfer_all,  nn, { rm, rm, nn, nn } },
  { "unlock",          &code_unlock,        nn, { rm, di, nn, nn } },
  { "update_quest",    &code_update_quest,  nn, { ch, in, nn, nn } },
  { "wait",            &code_wait,          nn, { in, nn, nn, nn } },
  { "weight",          &code_weight,        in, { th, nn, nn, nn } },
  { "",                NULL,                nn, { nn, nn, nn, nn } }
};

#undef nn
#undef st
#undef ch
#undef ob
#undef in
#undef rm
#undef sk
#undef st
#undef di
#undef rf
#undef na
#undef re
#undef th


/*
 *   POSITION ROUTINES
 */

   
void* code_is_mounted( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  return (void*) ( ch->mount != NULL );
}

 
void* code_is_resting( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Resting: NULL character." );
    return FALSE;
    }

  return (void*) ( ch->position == POS_RESTING );
}


void* code_is_fighting( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Fighting: NULL character." );
    return (void*) FALSE;
    }

  return (void*) ( ch->position == POS_FIGHTING );
}


void* code_is_player( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Player: NULL character." );
    return (void*) FALSE;
    }

  return (void*) ( ch->species == NULL );
}


/*
 *   ROOM ROUTINES
 */


void* code_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return (void*) FALSE;
    }

  return (void*) is_set( &room->room_flags, flag );
}


void* code_set_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return NULL;
    }

  set_bit( &room->room_flags, flag );

  return NULL;
}


void* code_remove_rflag( void** argument )
{
  int         flag  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  if( room == NULL ) {
    code_bug( "Code_rflag: NULL room" );
    return NULL;
    }

  remove_bit( &room->room_flags, flag );

  return NULL;
}


void* code_get_room( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Get_room: NULL character" );
    return NULL;
    }

  return ch->in_room;
}


/*
 *   CHARACTER ROUTINES
 */


void* code_size( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Size: Null argument" );
    return NULL;
    }

  return (void*) ch->Size( );
}


void* code_weight( void** argument )
{
  thing_data*  thing  = (char_data*) argument[0];

  if( thing == NULL ) {
    code_bug( "Weight: Null argument" );
    return NULL;
    }

  return (void*) thing->Weight( );
}


void* code_race( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Race: Null character." );
    return NULL;
    }

  return (void*) ch->shdata->race;
}


void* code_class( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Class: Null character." );
    return NULL;
    }

  if( ch->species != NULL ) {
    code_bug( "Class: Non-Player character." );
    return NULL;
    }
   
  return (void*) ch->pcdata->clss;
}


/*
 *   CFLAG ROUTINES
 */


void* code_cflag( void** argument )
{
  int         flag  = (int)        argument[0];
  char_data*    ch  = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Cflag: NULL character." );
    return (void*) FALSE;
    }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Cflag: flag out of range." );
    return (void*) FALSE;
    }

  if( ch->pcdata == NULL )
    return (void*) FALSE;

  return (void*) is_set( ch->pcdata->cflags, flag );
}


void* code_set_cflag( void** argument )
{
  int        flag  = (int)        argument[0];
  char_data*   ch  = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Set_Cflag: NULL character." );
    return NULL;
    }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Set_Cflag: flag out of range." );
    return NULL;
    }

  if( ch->pcdata != NULL )
    set_bit( ch->pcdata->cflags, flag );

  return NULL;
}


void* code_remove_cflag( void** argument )
{
  int flag      = (int)        argument[0];
  char_data* ch = (char_data*) argument[1]; 

  if( ch == NULL ) {
    code_bug( "Code_Remove_Cflag: NULL character." );
    return NULL;
    }

  if( flag < 0 || flag >= 32*MAX_CFLAG ) {
    code_bug( "Code_Remove_Cflag: flag out of range." );
    return NULL;
    }

  if( ch->pcdata != NULL )
    remove_bit( ch->pcdata->cflags, flag );

  return NULL;
}


/*
 *   CHARACTER STATUS ROUTINES
 */


void* code_can_fly( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Can_Fly: NULL Character." );
    return (void*) FALSE;
    }

  return (void*) ch->can_fly( );
}


void* code_is_silenced( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];

  if( ch == NULL ) {
    code_bug( "Is_Silenced: NULL Character." );
    return (void*) FALSE;
    }

  return (void*) is_set( ch->affected_by, AFF_SILENCE );
}


/*
 *   UNCLASSIFIED 
 */


void* code_interpret( void** argument )
{
  char_data* ch     = (char_data*) argument[0];
  char* string      = (char*)      argument[1];
  char_data* victim = (char_data*) argument[2];

  char tmp [ MAX_INPUT_LENGTH ];

  if( ch != NULL ) {
    if( victim == NULL ) 
      interpret( ch, string );
    else {
      sprintf( tmp, "%s %s", string, victim->descr->name );
      interpret( ch, tmp );
      }
    }

  return NULL;
}   


void* code_send_to_char( void** argument )
{
  char      *string = (char*)      argument[0];
  char_data *ch     = (char_data*) argument[1];

  send( string, ch );
 
  return NULL;
}


void* code_send_to_room( void** argument )
{
  char*      string = (char*)      argument[0];
  room_data*   room   = (room_data*) argument[1];

  if( room != NULL )
    act_room( room, string );
 
  return NULL;
}


void* code_send_to_area( void** argument )
{
  char      *string = (char*)      argument[0];
  room_data *room   = (room_data*) argument[1];

  if( room != NULL )
    send_to_area( string, room->area ); 
 
  return NULL;
}


void* code_act_room( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
//  obj_data*      obj  = (obj_data*)  argument[2];
//  char_data*  victim  = (char_data*) argument[3];  

  if( ch != NULL && ch->in_room != NULL )
    act_room( ch->in_room, string );

  return NULL;
}


void* code_act_neither( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
  obj_data*      obj  = (obj_data*)  argument[2];
  char_data*  victim  = (char_data*) argument[3];  
  
  act_neither( string, ch, victim, obj );

  return NULL;
}


void* code_act_tochar( void** argument )
{
  char*      string  = (char*)      argument[0];
  char_data*     ch  = (char_data*) argument[1];
  obj_data*     obj  = (obj_data*)  argument[2];
  char_data* victim  = (char_data*) argument[3];  
  
  act( ch, string, ch, victim, obj );

  return NULL;
}


void* code_act_area( void** argument )
{
  char*       string  = (char*)      argument[0];
  char_data*      ch  = (char_data*) argument[1];
  obj_data*      obj  = (obj_data*)  argument[2];
  char_data*  victim  = (char_data*) argument[3];  
  
  if( ch == NULL ) {
    code_bug( "Act_Area: Character = null pointer." );
    return NULL;
    }

  act_area( string, ch, victim, obj );

  return NULL;
}


void* code_act_notchar( void** argument )
{
  char*      string  = (char*)      argument[0];
  char_data*     ch  = (char_data*) argument[1];
  obj_data*     obj  = (obj_data*)  argument[2];
  char_data* victim  = (char_data*) argument[3];  
  
  if( ch != NULL )
    act_notchar( string, ch, victim, obj );

  return NULL;
}


void* code_act_notvict( void** argument )
{
//  char      *string = (char*)      argument[0];
//  char_data *ch     = (char_data*) argument[1];
//  obj_data  *obj    = (obj_data*)  argument[2];
//  char_data *victim = (char_data*) argument[3];  
  
  /*
    act( string, ch, obj, victim, TO_NOTVICT );
  act_to( string, ch, obj, victim, var_ch );
  */
  return NULL;
}


void* code_junk_mob( void** argument )
{
  char_data* ch = (char_data*) argument[0];

  if( ch == NULL )
    return NULL;
 
  if( !IS_NPC( ch ) ) {
    code_bug( "Junk_mob: character is a player??" );
    return NULL;
    } 
  
  ch->Extract( );

  return NULL;
}


void* code_drain_stat( void** argument )
{
  char_data* ch  = (char_data*) argument[0];
  int         i  = (int)        argument[1]; 
  int         j  = (int)        argument[2];
  int         m  = (int)        argument[3];

  int loc[] = { APPLY_STR, APPLY_INT, APPLY_WIS, APPLY_DEX, APPLY_CON }; 
  affect_data affect;
 
  if( ch == NULL || i < 0 || i > 4 || j <= 0 || m < -1 || m > 1 )
    return NULL;
 
  affect.type      = AFF_NONE;
  affect.location  = loc[i];
  affect.modifier  = m;
  affect.duration  = j;
  affect.level     = 5;
  affect.leech     = NULL;

  add_affect( ch, &affect );
  return NULL;
}  


void* code_find_stat( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1]; 

  if( ch == NULL ) 
    return NULL;

  if( i < 0 || i > 8 ) {
    code_bug( "Find_Stat: Impossible field." );
    return NULL;
    } 

  if( ch->species != NULL ) {
    int value[] = { ch->Strength( ), ch->Intelligence( ),
      ch->Wisdom( ), ch->Dexterity( ), ch->Constitution( ),
      ch->shdata->level, 0, 0, ch->shdata->alignment };
    return (void*) value[i];
    }
  else { 
    int value[] = { ch->Strength( ), ch->Intelligence( ),
      ch->Wisdom( ), ch->Dexterity( ), ch->Constitution( ),
      ch->shdata->level, ch->pcdata->piety,
      ch->pcdata->clss, ch->shdata->alignment };
    return (void*) value[i];
    }
}


void* code_random( void** argument )
{
  int  n1  = (int) argument[0];
  int  n2  = (int) argument[1];

  return (void*) ( number_range( n1,n2 ) );
}


void* code_dice( void** argument )
{
  int  n1  = (int) argument[0];
  int  n2  = (int) argument[1];
  int  n3  = (int) argument[2];

  return (void*) ( roll_dice( n1,n2 )+n3 );
}


void* code_find_skill( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  int        skill  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Find_Skill: NULL character." );
    return NULL;
    }

  return (void*) ch->get_skill( skill );
}


void* code_summon( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];

  if( ch != NULL )
    summon_help( ch, victim );

  return NULL;
}


void* code_reputation( void** argument )
{
  char_data*     ch  = (char_data*) argument[0];
//  int        nation  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Reputation: Null character." );
    return NULL;
    }

  return (void*) 50;
}


void* code_find_room( void** argument )
{
  int         vnum   = (int) argument[0];
  room_data*  room;

  if( ( room = get_room_index( vnum, FALSE ) ) == NULL ) 
    code_bug( "Find_Room: Non-existent room." );
 
  return room;
}


void* code_cast_spell( void** argument )
{
  int          spell  = (int)        argument[0];
  char_data*      ch  = (char_data*) argument[1];
  char_data*  victim  = (char_data*) argument[2];

  if( ch == NULL || spell < SPELL_FIRST || spell >= WEAPON_UNARMED ) 
    return NULL;

  switch( spell_table[ spell-SPELL_FIRST ].type ) {
   case STYPE_SELF_ONLY :
   case STYPE_PEACEFUL :
    if( victim == NULL )
      victim = ch;
    break;

   case STYPE_OFFENSIVE :
    if( victim == NULL )
      return NULL;
    break;

   default :
    return NULL;
    }

  /*
  stop_active( ch );

  event = new cast_event( ch, victim );
  
  event->vo      = victim;
  event->spell   = *spell-SPELL_FIRST;
  event->prepare = FALSE;
  event->wait    = spell_table[*spell-SPELL_FIRST].prepare-1;
  ch->active     = event;

  has_reagents( ch, event );
  execute_cast( event );
  */

  return NULL;  
}


void* code_do_spell( void** argument )
{
  int          spell  = (int)        argument[0];
  char_data*      ch  = (char_data*) argument[1];
  char_data*  victim  = (char_data*) argument[2];

  if( ch == NULL )
    return NULL;

  if( victim == NULL )
    victim = ch;

  if( spell >= SPELL_FIRST && spell < WEAPON_UNARMED ) 
    ( spell_table[spell-SPELL_FIRST].function )( ch, victim,
      NULL, 10, -1 );

  return NULL;
}


void* code_num_in_room( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          num  = 0;

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      num += ( character( room->contents[i] ) != NULL );

  return (void*) num;
}  


void* code_players_area( void** argument )
{
  room_data*  room  = (room_data*) argument[0];

  if( room == NULL ) {
    code_bug( "Players_Area: NULL room." );
    return NULL;
    }

  return (void*) room->area->nplayer;
}  


void* code_players_room( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  int          num  = 0;  

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      num += ( player( room->contents[i] ) != NULL );

  return (void*) num;
}  


void* code_num_mob( void** argument )
{
  int         vnum  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1];
  mob_data*    rch;
  int          num  = 0;  

  if( room != NULL )
    for( int i = 0; i < room->contents; i++ )
      if( ( rch = mob( room->contents[i] ) ) != NULL
        && rch->species->vnum == vnum )
        num++;

  return (void*) num;
}  


void* code_transfer( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  room_data*  room  = (room_data*) argument[1];

  if( room != NULL && room->vnum == 4 ) 
    room = get_room_index( ROOM_PRISON );

  if( room == NULL ) {
    send( "A script attempts to send you to a non-existent room??\r\n", ch );
    return NULL;
    }

  if( ch == NULL )
    return NULL;

  if( ch->rider != NULL )
    ch = ch->rider;

  ch->From( );
  ch->To( room );

  if( ch->mount != NULL ) {
    ch->mount->From( );
    ch->mount->To( room );
    }

  send( "\r\n", ch );
  do_look( ch, "" );

  return NULL;
}


void* code_transfer_all( void** argument )
{
  room_data*       from  = (room_data*) argument[0];
  room_data*         to  = (room_data*) argument[1];
  char_data*        rch;
  thing_array      list;
  int i;

  if( to == NULL )
    return NULL;

  copy( list, from->contents );

  for( i = 0; i < list; i++ )
    if( ( rch = character( list[i] ) ) != NULL ) {
      rch->From( );
      rch->To( to );
      }

  for( i = 0; i < list; i++ )
    if( ( rch = character( list[i] ) ) != NULL ) {
      send( "\r\n", rch );
      show_room( rch, to, FALSE, FALSE );
      }

  return NULL;
}


void* code_mload( void** argument )
{
  int               vnum  = (int)        argument[0];
  room_data*        room  = (room_data*) argument[1];
  mob_data*          mob;
  species_data*  species;

  if( room == NULL || ( species = get_species( vnum ) ) == NULL ) {
    code_bug( "Mload: non-existent species or null room." );
    return NULL;
    }

  mob = create_mobile( species );
  mreset_mob( mob );
 
  mob->To( room );
  
  return mob;
}


void* code_rand_char( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  char_data*   rch;

  if( room == NULL )
    return NULL;

  rch = random_pers( room );
  
  return rch;
}


void* code_rand_player( void** argument )
{
  room_data*  room  = (room_data*) argument[0];
  char_data*   rch;

  if( room == NULL )
    return NULL;

  rch = rand_player( room );
  
  return rch;
}

   
void* code_obj_in_room( void** argument )
{
  int         vnum  = (int)       argument[0];
  room_data*  room  = (room_data*) argument[1];

  if( room == NULL ) {
    code_bug( "Obj_in_room: NULL room." );
    return NULL;
    }

  return find_vnum( room->contents, vnum );
}


void* code_mob_in_room( void** argument )
{
  int         vnum  = (int)        argument[0];
  room_data*  room  = (room_data*) argument[1];
  char_data*   rch;

  if( room == NULL ) {
    code_bug( "Mob_in_Room: Null Room??" );
    return NULL;
    }

  for( int i = 0; i < room->contents; i++ )
    if( ( rch = mob( room->contents[i] ) ) != NULL
      && rch->species->vnum == vnum )
      return rch; 
  
  return NULL;
}


void* code_has_obj( void** argument )
{
  int        vnum  = (int)        argument[0];
  char_data*   ch  = (char_data*) argument[1];

  if( ch == NULL ) {
    code_bug( "Has_Obj: NULL character." );
    return NULL;
    }
 
  return find_vnum( ch->contents, vnum );
}


void* code_obj_to_room( void** argument )
{
  obj_data*    obj  = (obj_data*)  argument[0];
  room_data*  room  = (room_data*) argument[1];

  if( obj == NULL || room == NULL )
    return NULL;

  obj->To( room );
  
  return NULL;
}


void* code_obj_to_char( void** argument )
{
  obj_data*   obj  = (obj_data*)  argument[0];
  char_data*   ch  = (char_data*) argument[1];

  if( obj == NULL || ch == NULL )
    return NULL;

  obj->To( ch );

  return obj;
}

   
void* code_coin_value( void** argument )
{
  obj_data*  obj  = (obj_data*) argument[0];
  int          i;

  if( obj == NULL || obj->pIndexData->item_type != ITEM_MONEY )  
    return NULL;

  for( i = 0; i < MAX_COIN; i++ ) 
    if( obj->pIndexData->vnum == coin_vnum[i] ) 
      return (void*) ( coin_value[i]*obj->number );

  return NULL;;
}


void* code_plague( void** argument )
{
  char_data*        ch  = (char_data*) argument[0];
  affect_data   affect;
 
  if( ch == NULL || ch->save_vs_poison( 5 ) )
    return NULL;

  if( var_mob != NULL && var_mob->species != NULL
    && !is_set( ch->affected_by, AFF_PLAGUE ) )
    var_mob->species->special += 20;

  affect.type      = AFF_PLAGUE;
  affect.duration  = 11;
  affect.level     = 0;
  affect.leech     = NULL;

  add_affect( ch, &affect );

  return NULL;
}


void* code_poison( void** argument )
{
  char_data*        ch  = (char_data*) argument[0];
  affect_data   affect;

  if( ch == NULL || ch->save_vs_poison( 5 ) )
    return NULL;

  if( var_mob != NULL && var_mob->species != NULL 
    && !is_set( ch->affected_by, AFF_POISON ) ) 
    var_mob->species->special += 10;

  affect.type      = AFF_POISON;
  affect.duration  = 10;
  affect.level     = 8;
  affect.leech     = NULL;

  add_affect( ch, &affect );

  return NULL;
}


void* code_heal( void** argument )
{
  char_data* ch = (char_data*) argument[0];
  int i         = (int)        argument[1];

  if( ch != NULL ) {
    ch->hit += i;
    ch->hit = min( ch->hit, ch->max_hit );
    }

  return NULL;
}


void* code_modify_mana( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->mana+i < 0 ) 
    return (void*) FALSE;

  ch->mana += i;
  ch->mana = min( ch->mana, ch->max_mana );

  return (void*) TRUE;
}


void* code_drain_exp( void** argument )
{
  char_data *ch = (char_data*) argument[0];
  int i         = (int)       argument[1];

  if( ch != NULL && ch->species == NULL ) 
    add_exp( ch, -i, "You lose %d exp!!\r\n" );

  return NULL;
}


/*
 *   FUNCTION CALLS
 */


void* code_acode( void** argument )
{
  room_data *room = (room_data*) argument[0];
  int i           = (int)        argument[1];

  action_data* action; 
  int               j  = 1;

  if( room == NULL ) {
    code_bug( "Code_acode: NULL room." );
    return NULL;
    }

  for( action = room->action; action != NULL; action = action->next ) 
    if( j++ == i ) {
      push( );
      execute( action );
      pop( );
      break;
      }

  if( action == NULL ) 
    code_bug( "Code_acode: NULL action." );

  return NULL;
}


void* code_mpcode( void** argument )
{
  char_data* mob = (char_data*) argument[0];
  int i          = (int)       argument[1];

  mprog_data* mprog;
  int             j  = 1;

  if( mob == NULL || mob->species == NULL ) {
    code_bug( "Code_mpcode: NULL mob or mob is a player." );
    return NULL;
    }

  for( mprog = mob->species->mprog; mprog != NULL; mprog = mprog->next ) 
    if( j++ == i ) {
      push( );
      execute( mprog );
      pop( );
      break;
      }

  if( mprog == NULL ) 
    code_bug( "Code_mpcode: NULL mprog." );

  return NULL;
}


void* code_opcode( void** argument )
{
  obj_data *obj = (obj_data*) argument[0];
  int i         = (int)       argument[1];

  oprog_data* oprog;
  int             j  = 1;

  if( obj == NULL ) {
    code_bug( "Opcode: NULL obj." );
    return NULL;
    }
 
  for( oprog = obj->pIndexData->oprog; oprog != NULL; oprog = oprog->next ) 
    if( j++ == i ) {
      push( );
      execute( oprog );
      pop( );
      break;
      }

  if( oprog == NULL )
    code_bug( "Opcode: NULL oprog." );

  return NULL;
}


void* code_wait( void** argument )
{
  int i = (int) argument[0];

  queue_data* queue = new queue_data;

  end_prog   = TRUE;
  queue_prog = TRUE;
  
  queue->time = i;

  queue->room = var_room;
  queue->mob  = var_mob;
  queue->ch   = var_ch;
  queue->obj  = var_obj;
  queue->i    = var_i;

  queue->arg  = curr_arg;
  queue->next = queue_list;
  queue_list  = queue;

  return NULL;
}


void* code_is_searching( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  return (void*) ( ch != NULL && ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_SEARCHING ) );
}


void* code_is_follower( void** argument )
{
  char_data*   ch  = (char_data*) argument[0];

  return (void*) ( ch == NULL || is_set( &ch->status, STAT_FOLLOWER ) );
}
  

void* code_char_in_room( void** argument )
{
  char_data*    ch  = (char_data*) argument[0];
  room_data*  room  = (room_data*) argument[1]; 

  return (void*) ( ch != NULL && ch->in_room == room );
}


void* code_is_name( void** argument )
{
  char*       str  = (char*) argument[0];
  char*  namelist  = (char*) argument[1];

  if( str == NULL || namelist == NULL )
    return (void*) FALSE;

  return (void*) is_name( str, namelist );
}


void* code_set_religion( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->pcdata == NULL )
    return NULL;

  if( i < 0 || i >= MAX_RELIGION ) {
    code_bug( "Code_set_religion: religion value out of range." );
    return NULL;
    } 

  ch->pcdata->religion = i;

  return NULL;
}  


void* code_tell( void** argument )
{
  char_data*      ch  = (char_data*) argument[0];
  char_data*  victim  = (char_data*) argument[1];
  char*       string  = (char*)      argument[2];

  char      tmp  [ 3*MAX_STRING_LENGTH ];

  if( victim == NULL || ch == NULL || victim->pcdata == NULL )
    return NULL;

  act_print( tmp, string, victim, ch, NULL, NULL, NULL, NULL, victim );
  process_tell( ch, victim, tmp );

  return NULL;
}


void* code_obj_name( void** argument )
{
  obj_data* obj = (obj_data*) argument[0];

  if( obj == NULL )
    return NULL;

  return obj->singular;
} 


void* code_disarm( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          j  = (int)        argument[1];

  if( ch == NULL ) {
    code_bug( "Disarm: NULL character or level." );
    return NULL;
    }

  return (void*) ( ch->check_skill( SKILL_UNTRAP )
    && number_range( 0, 20 ) > j );
}


/*
 *   MOVEMENT ROUTINES
 */


void* code_modify_move( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL || ch->move+i < 0 ) 
    return (void*) FALSE;

  ch->move += i;
  ch->move  = min( ch->move, ch->max_move );
 
  return (void*) TRUE;
}


void* code_is_exhausted( void** argument )
{
  char_data*  ch  = (char_data*) argument[0];
  int          i  = (int)        argument[1];

  if( ch == NULL )
    return (void*) TRUE;
  
  if( ch->move-i < 0 ) {
    send( ch, "You are too exhausted.\r\n" );
    return (void*) TRUE;
    }

  ch->move -= i;
 
  return (void*) FALSE;
}


