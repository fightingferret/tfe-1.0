#include "define.h"
#include "struct.h"


const char* stype_name [] = { "offensive", "peaceful", "self_only",
  "object", "room", "world", "peaceful_other", "weapon", "drink_container",
  "mob_only", "annoying", "corpse", "recall", "weapon_armor", "replicate",
  "astral" };


/*
 *   LOCAL FUNCTIONS 
 */


int    check_mana        ( char_data*, int );
int    mana_cost         ( char_data*, int );
void   spell_action      ( int action, char_data*, thing_data*, obj_data* );


/*
 *   CAST_DATA CREATOR/DESTRUCTOR
 */


Cast_Data :: Cast_Data( )
{
  record_new( sizeof( cast_data ), MEM_SPELL );
  target = NULL;
  vzero( reagent, MAX_SPELL_WAIT );
}
 

Cast_Data :: ~Cast_Data( )
{
  record_delete( sizeof( cast_data ), MEM_SPELL );
}


/*
 *   CHECK_MANA ROUTINES
 */


int check_mana( char_data* ch, int spell )
{
  int i  = mana_cost( ch, spell );

  if( ch->mana < i ) {
    send( ch, "You need %d energy points to cast that.\r\n", i );
    return -1;
    }

  return i;
}


int mana_cost( char_data* ch, int spell )
{
  int    mana;
  bool  error  = FALSE;

  if( ch->species != NULL )
    return 0;

  mana = evaluate( spell_table[spell].cast_mana, error,
    ch->shdata->skill[ SPELL_FIRST+spell ] );

  if( error ) 
    bug( "Mana_Cost: Evaluate failed for %s.", spell_table[spell].name );

  if( ch->pcdata != NULL && ch->pcdata->clss == CLSS_MAGE )
    mana += mana*mana_absorption( ch )/10000;

  return mana;
}

  
int mana_absorption( char_data* ch )
{
  obj_data*  obj;
  int       cost;
  int     number;
  int      total  = 0; 

  for( int i = 0; i < ch->wearing; i++ ) {
    obj = (obj_data*) ch->wearing[i];
    if( obj->metal( ) ) {
      cost   = 0;
      number = 0;
      for( int j = 0; j <= MAX_MATERIAL; j++ ) 
        if( is_set( &obj->materials, j ) ) {
          number++;
          if( j >= MAT_BRONZE )
            cost += obj->Empty_Weight( )*material_table[j].mana;
          }
      if( number > 0 )
        total += cost/number;
      }
    }

  return total;
}


/*
 *   REAGENT ROUTINE
 */


int used_reagent( cast_data* cast, obj_data* obj )
{
  int count  = 0;
  int     i;

  for( i = 0; i < MAX_SPELL_WAIT; i++ )
    if( cast->reagent[i] == obj 
      && spell_table[cast->spell].reagent[i] > 0 )
      count++;

  return count;
}


bool has_reagents( char_data* ch, cast_data* cast )
{ 
  int                spell  = cast->spell;
  bool             prepare  = cast->prepare;
  int         wait_prepare  = spell_table[spell].prepare;
  obj_data*            obj;
  obj_clss_data*  obj_clss;

  for( int i = ( prepare ? 0 : wait_prepare ); i < ( prepare
    ? wait_prepare : MAX_SPELL_WAIT ); i++ ) {

    if( ( obj_clss = get_obj_index(
      abs( spell_table[spell].reagent[i] ) ) ) == NULL )
      continue;
    
    for( int j = 0; j < ch->contents; j++ ) 
      if( ( obj = object( ch->contents[j] ) ) != NULL
        && ( obj->pIndexData == obj_clss
        || ( obj_clss->item_type == ITEM_CROSS
        && obj->pIndexData->item_type == ITEM_CROSS ) ) 
        && used_reagent( cast, obj ) < obj->number ) {
        cast->reagent[i] = obj;
        break;
        }

    if( cast->reagent[i] == NULL ) {
      if( ch->pcdata == NULL || ch->shdata->level >= LEVEL_APPRENTICE ) {
	send( ch, "You create %s.\r\n", obj_clss->Name( ) );
        obj = create( obj_clss );
        obj->To( ch );
        cast->reagent[i] = obj;
        continue;
        }
 
      send( ch, "You need %s to cast %s.\r\n",
        obj_clss->Name( ), spell_table[spell].name );
      return FALSE;
      }
    }

  return TRUE;
}


void remove_reagent( obj_data* reagent, char_data* ch )
{
  if( reagent->pIndexData->item_type != ITEM_REAGENT
    || reagent->value[0] <= 1 ) {
    reagent->Extract( 1 );
    return;
    }

  if( reagent->number > 1 ) {
    reagent->number--; 
    reagent = duplicate( reagent );
    reagent->value[0]--;
    reagent->To( ch );
    }          
  else {
    reagent->value[0]--;
    }
}


/*
 *   CASTING ROUTINES
 */


int find_spell( char_data* ch, char* argument, int length )
{
  int spell;

  for( spell = 0; spell < MAX_SPELL; spell++ ) 
    if( ( ch->species != NULL || ch->get_skill( SPELL_FIRST+spell ) != 0 )
      && !strncasecmp( spell_table[spell].name, argument, abs( length ) ) ) 
      return spell;

  if( length < 0 )
    return -1;

  for( spell = 0; spell < MAX_SPELL; spell++ ) 
    if( !strncasecmp( spell_table[spell].name, argument, length ) )
      break;

  if( spell == MAX_SPELL ) 
    send( ch, "Unknown Spell.\r\n" );
  else
    send( ch, "You don't know the spell %s.\r\n",
      spell_table[spell].name );

  return -1;
}


void do_focus( char_data *ch, char* )
{
  send( "Function disabled.\r\n", ch );
}


void do_cast( char_data* ch, char* argument )
{
  cast_data*           cast;
  cast_data*        prepare  = NULL;;
  int                 word1;
  int                 word2  = -1;
  int                 spell  = -1;
  int                  mana;
 
  if( is_confused_pet( ch ) || is_familiar( ch ) )
    return;

  if( is_set( ch->affected_by, AFF_SILENCE ) ) {
    send( ch, "You are silenced and unable to cast spells!\r\n" );
    return;
    }

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    send( ch, "You are too entangled to cast spells.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    send( ch, "What spell do you want to cast?\r\n" );
    return;
    }

  for( word1 = 0; argument[word1] != ' ' && argument[word1] != '\0';
    word1++ );

  if( argument[word1] != '\0' ) {
    for( word2 = word1+1; argument[word2] != ' ' && argument[word2] != '\0';
      word2++ );
    spell = find_spell( ch, argument, -word2 ); 
    }  

  if( spell == -1 || argument[word1] == '\0' ) {
    if( ( spell = find_spell( ch, argument, word1 ) ) == -1 )
      return;
    word2 = -1;
    }

  mana = 0;

  if( spell_table[spell].prepare != 0 ) {
    if( ( prepare = has_prepared( ch, spell ) ) == NULL ) {
      send( ch, "You don't have that spell prepared.\r\n" );
      return;
      }
    }
  else {
    if( ch->species == NULL && ( mana = check_mana( ch, spell ) ) < 0 )
      return;
    }

  if( !allowed_location( ch, &spell_table[spell].location,
    "cast", spell_table[spell].name ) )
    return;

  cast           = new cast_data;
  cast->spell    = spell;
  cast->prepare  = FALSE;
  cast->wait     = spell_table[spell].prepare-1;
  cast->mana     = mana;

  if( !get_target( ch, cast, word2 == -1 ? &argument[word1] : &argument[word2] )
    || !has_reagents( ch, cast ) ) {
    delete cast;
    return;
    }

  send( ch, "You begin casting %s.\r\n", spell_table[spell].name );

  if( ch->species == NULL && spell_table[spell].prepare != 0 ) {
    if( --prepare->times == 0 ) { 
      remove( ch->prepare, prepare );
      delete prepare;
      }
    else if( is_set( &ch->pcdata->message, MSG_SPELL_COUNTER ) ) {
      send( ch, "[ You have %s %s spell%s remaining. ]\r\n", 
        number_word( prepare->times ), spell_table[spell].name, 
        prepare->times == 1 ? "" : "s" );
      }
    }

  ch->cast    = cast;
  ch->mana   -= mana;

  add_queue( &ch->active, ch->species != NULL ? 6 
    : 10-ch->shdata->skill[ SPELL_FIRST+spell ]/2 );

  return;
}


void do_prepare( char_data* ch, char* argument )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  cast_data*    cast;
  int          spell;
  int           mana;
  int         length  = strlen( argument );

  if( ch->species != NULL ) {
    send( ch, "Only players can prepare spells.\r\n" );
    return;
    }

  if( is_set( ch->affected_by, AFF_SILENCE ) ) {
    send( "You are silenced and unable to prepare a spell!\r\n", ch );
    return;
    }

  if( is_set( ch->affected_by, AFF_ENTANGLED ) ) {
    send( ch, "You are too entangled to prepare a spell.\r\n" );
    return;
    }

  if( *argument == '\0' ) {
    if( ch->prepare == NULL ) {
      send( "You have no spells prepared.\r\n", ch );
      return;
      }
    page_underlined( ch, "Num  Spell                   Mana\r\n" );
    for( cast = ch->prepare; cast != NULL; cast = cast->next ) {
      sprintf( tmp, "%3d  %-25s%3d\r\n", cast->times,
        spell_table[cast->spell].name, cast->mana*cast->times );
      page( ch, tmp );
      }

    return;
    }

  if( !strcasecmp( "clear", argument ) ) {
    delete_list( ch->prepare );
    send( ch, "All prepared spells forgotten.\r\n" );
    update_max_mana( ch );
    return;
    }

  if( ch->position < POS_RESTING ) {
    send( ch, "You cannot prepare spells while sleeping or meditating.\r\n" );
    return;
    }

  if( ( spell = find_spell( ch, argument, length ) ) == -1 )
    return;

  if( spell_table[spell].prepare == 0 ) {
    send( ch, "That is not a spell which you prepare.\r\n" );
    return;
    } 

  if( ( mana = check_mana( ch, spell ) ) < 0 )
    return;

  cast           = new cast_data;
  cast->spell    = spell;    
  cast->wait     = -1;
  cast->prepare  = TRUE;
  cast->mana     = mana;

  if( !has_reagents( ch, cast ) ) {
    delete cast;
    return;
    }

  send( ch, "You begin preparing %s.\r\n", spell_table[spell].name );

  ch->cast    = cast;
  ch->mana   -= mana;

  set_delay( ch, ch->species != NULL ? 12 
    : 16-ch->shdata->skill[ SPELL_FIRST+spell ]/2 );

  return;
}


cast_data* has_prepared( char_data* ch, int spell )
{
  cast_data* prepare;

  if( ch->species == NULL && spell_table[spell].prepare != 0 ) 
    for( prepare = ch->prepare; prepare != NULL; prepare = prepare->next )
      if( prepare->spell == spell )
        return prepare;

  return NULL;
}


/*
 *   UPDATE ROUTINE
 */


void spell_update( char_data* ch )
{
  cast_data*     cast  = ch->cast;
  thing_data*  target  = cast->target;
  char_data*   victim  = character( target );
  int           spell  = cast->spell;
  bool        prepare  = cast->prepare;
  int            wait  = ++cast->wait;

  char_data*      rch;
  int           skill;
  obj_data*   reagent;
  int          action;
  cast_data*     prev;

  if( wait < ( cast->prepare ? spell_table[spell].prepare 
    : spell_table[spell].wait ) ) {

    reagent  = cast->reagent[wait];
    action   = spell_table[spell].action[wait];
    msg_type = cast->prepare ? MSG_PREPARE : MSG_STANDARD;
    
    spell_action( action, ch, target, reagent );
    if( reagent != NULL && spell_table[spell].reagent[wait] >= 0 ) 
      remove_reagent( reagent, ch );
    set_delay( ch, ch->species != NULL ? 31 
      : 35-ch->shdata->skill[ SPELL_FIRST+spell ] );
    return;
    }

  ch->cast = NULL;

  set_delay( ch, 2 );
  update_max_mana( ch );

  if( !prepare ) { 
    delete cast;

    if( is_set( &ch->in_room->room_flags, RFLAG_NO_MAGIC ) ) {
      fsend( ch, "As you cast %s, you feel the energy drain from you\
 and nothing happens.\r\n", spell_table[spell].name );
      send( *ch->array, "%s casts %s, but nothing happens.\r\n",
        ch, spell_table[spell].name );
      return;
      }
  
    send( ch, "+++ You cast %s. +++\r\n", spell_table[spell].name );
    send_seen( ch, "%s casts %s.\r\n", ch, spell_table[spell].name );

    skill = ( ch->species != NULL ? 8
      : ch->shdata->skill[ SPELL_FIRST+spell ] );
  
    if( spell_table[spell].type == STYPE_ANNOYING ) {
      for( int i = 0; i < *ch->array; i++ ) 
        if( ( rch = mob( ch->array->list[i] ) ) != NULL
          && !is_set( &rch->status, STAT_PET ) ) {
          check_killer( ch, rch );
          init_attack( victim, ch );
          }
      }
    else if( spell_table[spell].type == STYPE_OFFENSIVE ) {
      check_killer( ch, victim );
      ch->fighting = victim;
      react_attack( ch, victim );
      }

    remove_bit( &ch->status, STAT_LEAPING );
    remove_bit( &ch->status, STAT_WIMPY );
    remove_bit( &ch->status, STAT_HIDING );
    remove_bit( ch->affected_by, AFF_HIDE );
    remove_bit( &ch->status, STAT_CAMOUFLAGED );
    remove_bit( ch->affected_by, AFF_CAMOUFLAGE );
    remove_bit( ch->affected_by, AFF_INVISIBLE );
    
    strip_affect( ch, AFF_INVISIBLE );

    ch->improve_skill( SPELL_FIRST+spell );

    ( *spell_table[spell].function )( ch, character( target ),
       target, skill, 0 );

    return;
    }

  for( prev = ch->prepare; prev != NULL; prev = prev->next )
    if( prev->spell == spell ) {
      prev->times++;
      delete cast;
      break;
      }

  if( prev == NULL ) {
    cast->times = 1; 
    cast->next  = ch->prepare;
    ch->prepare = cast;
    prev        = cast;
    }

  if( prev->times > 1 ) {
    send( ch, "You now have %s incantations of %s prepared.\r\n",
      number_word( prev->times ), spell_table[spell].name );
    }
  else {
    send( ch, "You have prepared %s.\r\n", spell_table[spell].name );
    }

  update_max_mana( ch );
}


void spell_action( int action, char_data* ch, thing_data* target,
  obj_data* reagent )
{
  char_data* victim = NULL;

  if( action < 0 || action > table_max[ TABLE_SPELL_ACT ] ) {
    roach( "Spell_Action: Impossible action." );
    roach( "-- Caster = %s", ch->Name( ) );
    return;
    }

  if( reagent != NULL )
    reagent->selected = 1;

  if( target == ch ) {
    act( ch, spell_act_table[action].self_self,       ch, NULL, reagent );
    act_notchar( spell_act_table[action].others_self, ch, NULL, reagent );
    return;
    }

  act( ch, spell_act_table[action].self_other, ch, target, reagent );

  if( target != NULL
    && ( victim = character( target ) ) != NULL 
    && spell_act_table[action].victim_other != empty_string ) {
    if( victim->position > POS_SLEEPING )
      act( victim, spell_act_table[ action ].victim_other,
        ch, target, reagent );
    act_neither( spell_act_table[ action ].others_other,
      ch, victim, reagent );
    }
  else 
    act_notchar( spell_act_table[ action ].others_other,
      ch, NULL, reagent );
}


/*
 *   RESISTANCE ROUTINES  
 */


bool makes_save( char_data* victim, char_data* ch, int type, int spell,
  int level )
{
  int chance;

  if( ch == victim )
    return FALSE;

  switch( type ) {
    case RES_MAGIC:  chance = victim->Save_Magic( )+victim->shdata->level/2;
      return( number_range( 0, 99 ) < chance );
    break;

    case RES_MIND:  chance = victim->Save_Mind( )+victim->shdata->level/2; 
      return( number_range( 0, 99 ) < chance );
    break;
  
    case RES_DEXTERITY:
      if( victim->position <= POS_RESTING )
        return FALSE;
      chance = 3*victim->Dexterity( ) + victim->shdata->level/2;
      break;
    
    default:
      bug( "Makes_Save: Unknown Resistance." );
      return TRUE;
    }

  if( number_range( 0, 99 ) < chance )
    return TRUE;

  if( number_range( 0, 99 ) < 80-15*level/2 )
    return TRUE;

  if( ch != NULL ) {
    chance = victim->shdata->level-ch->shdata->level;
    if( ch->pcdata != NULL ) 
      chance += skill_table[spell].level[ ch->pcdata->clss ];
    }
  else {
    chance = victim->shdata->level;
    }
  
  return( number_range( 0, 99 ) < chance );
}


/*
 *   TABLE EVALUATE ROUTINES
 */


int spell_damage( int spell, int level, int var )
{
  int   damage;
  bool   error  = FALSE;

  if( level < 0 ) 
    level = (-level)%100;

  spell  -= SPELL_FIRST;
  damage  = evaluate( spell_table[spell].damage, error, level, var );

  if( error ) 
    bug( "Spell_Damage: Evaluate failed for %s.", spell_table[spell].name );

  return damage;
}


int duration( int spell, int level, int var )
{
  int    duration;
  bool      error  = FALSE;

  spell    -= SPELL_FIRST;
  duration  = evaluate( spell_table[spell].duration, error, level, var );

  if( error ) 
    bug( "Duration: Evaluate failed for %s.", spell_table[spell].name );

  return duration;
}


int leech_regen( int spell, int level, int var )
{
  int    regen;
  bool   error  = FALSE;

  spell -= SPELL_FIRST;
  regen  = evaluate( spell_table[spell].regen, error, level, var );

  if( error ) 
    bug( "Leech_Regen: Evaluate failed for %s.", spell_table[spell].name );

  return regen;
}


int leech_mana( int spell, int level, int var )
{
  int    mana;
  bool  error  = FALSE;

  spell -= SPELL_FIRST;
  mana   = evaluate( spell_table[spell].leech_mana, error, level, var );

  if( error ) 
    bug( "Leech_Mana: Evaluate failed for %s.", spell_table[spell].name );

  return mana;
}


void spell_affect( char_data* ch, char_data* victim, int level,
  int time, int spell, int type, int var )
{
  affect_data affect;

  affect.type = type;

  if( ch == NULL || time > 0 ) {
    affect.duration = time;
    affect.level    = level;
    }
  else {
    affect.level    = level;
    affect.duration = duration( spell, level, var );
    if( spell_table[ spell-SPELL_FIRST ].regen != empty_string ) {
      affect.leech_regen = leech_regen( spell, level, var );
      affect.leech_max   = leech_mana( spell, level, var );
      affect.leech       = ch;
      }
    }

  add_affect( victim, &affect );

  return;
}


/*
 *   CLERIC/PALADIN SPELLS
 */


bool spell_cause_light( char_data *ch, char_data *victim, void*,
  int level, int )
{
  damage_magic( victim, ch, spell_damage( SPELL_CAUSE_LIGHT, level ),
    "*The spell" );

  return TRUE;
}


bool spell_cause_serious( char_data *ch, char_data *victim, void*,
  int level, int )
{
  damage_magic( victim, ch, spell_damage( SPELL_CAUSE_SERIOUS, level ),
    "*The spell" );

  return TRUE;
}


bool spell_cause_critical( char_data *ch, char_data *victim, void*,
  int level, int )
{
  damage_magic( victim, ch, spell_damage( SPELL_CAUSE_CRITICAL, level ),
    "*The spell" );

  return TRUE;
}


bool spell_create_food( char_data *ch, char_data*, void*,
  int level, int )
{
  obj_data*            obj;  
  obj_clss_data*  obj_clss;
  int                 item;

  if( null_caster( ch, SPELL_CREATE_FOOD ) )
    return TRUE;

  level = range( 1, level, 10 );

  for( ; ; ) {
    item = number_range( 0, 2*level-1 ); 
    item = max( number_range( 0, 2*level-1 ), item ); 
    item = list_value[ LIST_CREATE_FOOD ][ item ];

    if( ( obj_clss = get_obj_index( item ) ) != NULL )
      break;
    }

  if( ( obj = create( obj_clss ) ) == NULL ) {
    bug( "Create_food: NULL object" );
    return TRUE;
    }  

  obj->To( ch );
  
  send( ch, "%s appears in your hand.\r\n", obj );
  send_seen( ch, "%s appears in %s's hand.\r\n", obj, ch );
  
  return TRUE;    
}



bool spell_create_feast( char_data *ch, char_data*, void*,
  int level, int )
{
//  int          i;
//  int     pieces;

  /*
  pieces = number_range( 0, 10 );
  for( i = 0; i < pieces; i++ )
    do_spell_create_food( ch, level );
*/

  return TRUE;
}

bool spell_cure_blindness( char_data* ch, char_data* victim, void*,
  int, int )
{
  if( !is_set( victim->affected_by, AFF_BLIND ) ) {
    if( ch != victim )
      send( ch, "%s wasn't blind.\r\n", victim );
    else
      send( ch, "You aren't blind!\r\n" );
    return TRUE;
    }

  strip_affect( victim, AFF_BLIND );

  if( is_set( victim->affected_by, AFF_BLIND ) ) {
    if( ch != victim )
      send( ch, "%s is still blind!!\r\n", victim );
    send( victim, "You are still blind!!\r\n" );
    }

  return TRUE;
}


bool spell_curse( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_CURSE, AFF_CURSE );

  return TRUE;
}


bool spell_detect_evil( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_EVIL,
    AFF_DETECT_EVIL );

  return TRUE;
}


bool spell_detect_good( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_GOOD,
    AFF_DETECT_GOOD );

  return TRUE;
}


bool spell_faerie_fire( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_FAERIE_FIRE,
    AFF_FAERIE_FIRE );

  return TRUE;
}


bool spell_harm( char_data *ch, char_data *victim, void*,
  int level, int )
{

  damage_magic( victim, ch, spell_damage( SPELL_HARM, level ),
    "heaven sent bolt of energy" );

  return TRUE;
}


bool spell_neutralize( char_data *ch, char_data *victim, void*, int, int )
{
  if( !is_set( victim->affected_by, AFF_HALLUCINATE )
    && !is_set( victim->affected_by, AFF_SILENCE ) ) {
    if( ch != victim )
      send( ch, "%s was neither hallucinating or silenced.\r\n", victim );
    else
      send( victim,
      "You weren't hallucinating or silenced so the spell had no affect.\r\n" );
    return TRUE;
    }

  strip_affect( victim, AFF_HALLUCINATE );
  strip_affect( victim, AFF_SILENCE );

  if( is_set( victim->affected_by, AFF_HALLUCINATE ) ) {
    send( ch, "%s is still hallucinating!!\r\n", victim );
    send( "You are still hallucinating!!\r\n", victim );
    }
  return TRUE;
}


bool spell_revitalize( char_data* ch, char_data* victim, void* obj,
  int level, int )
{
  int move;

  if( obj != NULL && ( victim == NULL || ch == NULL ) )
    return TRUE;

  move = victim->move+15*level;

  strip_affect( victim, AFF_DEATH );
  update_maxes( victim );
  
  if( move >= victim->max_move ) {
    send( victim, "You are completely revitalized.\r\n" );
    send( *victim->array, "%s is completely revitalized.\r\n", victim );
    victim->move = victim->max_move;
    return TRUE;
    }
  
  send( "You are partially revitalized.\r\n", victim );
  send( *victim->array, "%s is partially revitalized.\r\n", victim );
	      
  victim->move = move;

  return TRUE;
}


bool spell_slay( char_data *ch, char_data *victim, void*,
  int level, int  )
{

  damage_magic( victim, ch, spell_damage( SPELL_SLAY, level ),
    "*The divine fury of the channeled power" );

  return TRUE;
}


/*
 *   RANGER SPELLS
 */


bool spell_protection_plants( char_data *ch, char_data *victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_PROT_PLANTS,
    AFF_PROT_PLANTS );

  return TRUE;
}


/*
 *   MAGE SPELLS
 */


bool spell_detect_hidden( char_data *ch, char_data *victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_HIDDEN,
    AFF_DETECT_HIDDEN );

  return TRUE;
}


bool spell_displace( char_data *ch, char_data *victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DISPLACE, AFF_DISPLACE );

  return TRUE;
}


bool spell_invisibility( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INVISIBILITY,
    AFF_INVISIBLE );

  return TRUE;
}


bool spell_locust_swarm( char_data *ch, char_data*, void*,
  int level, int )
{
  obj_data *obj;

  if( ch == NULL ) {
    bug( "Locust_Swarm: NULL caster.", 0 );
    return TRUE;
    }

  if( ( obj = find_vnum( *ch->array, 271 ) ) != NULL ) {
    obj->value[0] = UMAX( level, obj->value[0] );
    return TRUE;
    }

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    send( ch, "The insects don't seem to be responding.\r\n" );
    fsend_seen( ch, "%s is looking around expectantly and frowning.\r\n", ch );
    return TRUE;
    }

  obj = create( get_obj_index( 271 ) );
  obj->timer = 1+level/2;
  obj->To( ch->array );

  return TRUE;
}


bool spell_poison_cloud( char_data *ch, char_data*, void*, 
  int level, int )
{
  obj_data *obj;
 
  if( null_caster( ch, SPELL_POISON_CLOUD ) )
    return TRUE;

  if( ( obj = find_vnum( *ch->array, 279 ) ) != NULL ) {
    obj->value[0] = max( level, obj->value[0] );
    return TRUE;
    }

  if( ch->in_room->sector_type == SECT_UNDERWATER ) {
    send( ch, "You fail to raise the cloud underwater.\r\n" );
    return TRUE;
    }

  obj = create( get_obj_index( 279 ) );
  obj->timer = 1+level/2;
  obj->To( ch->array );

  return TRUE;
}


bool spell_polymorph( char_data*, char_data*, void*, int, int )
{
  return TRUE;
}


bool spell_mystic_shield( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_MYSTIC_SHIELD,
    AFF_PROTECT );

  return TRUE;
}


bool spell_infravision( char_data *ch, char_data *victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_INFRAVISION,
    AFF_INFRARED );

  return TRUE; 
}


bool spell_detect_invisible( char_data* ch, char_data* victim, void*,
  int level, int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_DETECT_INVISIBLE,
    AFF_SEE_INVIS );

  return TRUE;
}    


bool spell_vitality( char_data *ch, char_data *victim, void*, int level,
  int duration )
{
  spell_affect( ch, victim, level, duration, SPELL_VITALITY, AFF_VITALITY );

  return TRUE;
}


/*
 *   PSIONIC SPELLS
 */


bool spell_calm( char_data* ch, char_data* victim, void*, int level, int )
{
  if( null_caster( ch, SPELL_CALM ) ) 
    return TRUE;

  if( victim->position < POS_RESTING ) {
    send( ch, "%s is unconscious and so the spell has no affect.\r\n",
      victim );
    return TRUE;
    }

  if( victim->fighting != ch ) {
    send( ch, "%s isn't fighting you.\r\n", victim );
    return TRUE;
    }

  if( makes_save( victim, ch, RES_MIND, SPELL_CALM, level ) ) {
    send( victim, "You are unaffected by the calm spell.\r\n" );
    send_seen( victim,
      "%s seems to pause a moment but then continues to fight!\r\n",
      victim );
    return TRUE;
    }

  stop_fight( victim );

  send( ch, "%s stops attacking you.\r\n", victim );
  send( victim, "You don't feel like fighting %s any more.\r\n", ch );
  send_seen( victim, "%s stops attacking %s.\r\n", ch, victim );

  return TRUE;
} 











