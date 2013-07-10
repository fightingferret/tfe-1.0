#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   LOCAL FUNCTIONS
 */


const char*   get_attack     ( char_data*, obj_data* );

bool    absorb_armor      ( char_data*, char_data*, const char*, int&,
                            const char*& );
bool    absorb_bracers    ( char_data*, char_data*, int&, int, const char* );
bool    absorb_shield     ( char_data*, char_data*, const char*, int& ); 
bool    can_defend        ( char_data*, char_data* );
bool    block_shield      ( char_data*, char_data*, const char* );
void    damage_modifiers  ( char_data*, char_data*, obj_data*, int& );
bool    damage_weapon     ( char_data*, obj_data*& );
bool    dodge             ( char_data*, char_data*, int& );
bool    tumble            ( char_data*, char_data*, int& );
bool    fire_shield       ( char_data*, char_data* );
bool    thorn_shield      ( char_data*, char_data* );
bool    ion_shield        ( char_data*, char_data* );
bool    flaming_weapon    ( char_data*, char_data*, obj_data* );
bool    misses_blindly    ( char_data*, char_data*, int&, const char* );
bool    parry             ( char_data*, char_data*, int& );
bool    counter_attack    ( char_data*, char_data*, int& );
bool    guard             ( char_data*, char_data*, int& );
bool    shadow_dance      ( char_data*, char_data*, const char* );
bool    switch_victim     ( char_data*, char_data* );
bool    trip              ( char_data*, char_data*, obj_data* );
int     damage_armor      ( char_data*, int );
int     backstab_damage   ( char_data* );
int     charge_damage     ( char_data* );
void    act_spam_notvict  ( char*, char_data*, obj_data*, char_data* );
void    confused_char     ( char_data* );
void    critical_hit      ( char_data*, char_data*, obj_data*, int& );
bool    is_blocked        ( char_data*, char_data*, obj_data*, char* );
void    leap_message      ( char_data*, char_data* );
void    modify_roll       ( char_data*, char_data*, obj_data*, int& );
void    power_strike      ( char_data*, char_data*, obj_data*, int& );
void    stun              ( char_data*, char_data*, const char* );
void    trigger_hit       ( char_data*, char_data*, obj_data* );
void    player_multi      ( char_data*, char_data* );
void    barkskin          ( char_data*, int& );
void    invulnerable      ( char_data*, int& );
void    protgood          ( char_data*, char_data*, int& );
void    protevil          ( char_data*, char_data*, int& );


/*
 *   LOCAL CONSTANTS
 */


const char* dam_loc_name [] = { "light", "finger", "finger", "neck", "neck",
  "body", "head", "leg", "feet", "hand", "arm", "shield", "about",
  "waist", "left wrist", "right wrist", "wield", "hold" };

const char* weapon_attack [MAX_WEAPON] = { "punch", "pierce", "slash",
  "bash", "swing", "slash", "pound", "whip", "chop", "missile weapon",
  "pierce" };


/*
 *   EXTERNAL SUPPORT ROUTINES
 */



int initiative( char_data* )
{
  return number_range( 5,30 );
}


int response( char_data* )
{
  return number_range( 5,30 );
}


char_data* opponent( char_data* ch )
{
  char_data* rch;

  if( ( rch = ch->fighting ) != NULL )
    return rch;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch->fighting == ch )
      return rch;

  return NULL;
} 


char_data* has_enemy( char_data* ch )
{
  char_data* rch;

  if( ch->fighting != NULL )
    return ch->fighting;

  for( int i = 0; i < *ch->array; i++ ) 
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch->position > POS_SLEEPING 
      && includes( rch->aggressive, ch ) )
      return rch;

  if( ch->cast != NULL
    && spell_table[ ch->cast->spell ].type == STYPE_OFFENSIVE )
    return (char_data*) ch->cast->target;

  return NULL;
}
 

/*
 *   TOP LEVEL ROUND HANDLER
 */


void leap_message( char_data* ch, char_data* victim )
{
  char_data* rch;

  if( victim->fighting == ch ) {
    send( ch, "You counterattack %s!!\n\r", victim );

    if( ch->Seen( victim ) )
      send( victim, "%s%s counterattacks you!!%s\n\r",
        bold_v( victim ), ch, normal( victim ) );

    for( int i = 0; i < *victim->array; i++ ) 
      if( ( rch = character( victim->array->list[i] ) ) != NULL 
        && rch != victim && rch != ch && ch->Seen( rch )
        && rch->link != NULL ) 
        send( rch, "%s%s counterattacks %s!!%s\n\r",
          damage_color( rch, ch, victim ), ch, victim, normal( rch ) );
    }
  else {
    send( ch, "You leap to attack %s!!\n\r", victim );
    if( ch->Seen( victim ) )
      send( victim, "%s%s leaps to attack you!!%s\n\r",
        bold_v( victim ), ch, normal( victim ) );

    for( int i = 0; i < *victim->array; i++ ) 
      if( ( rch = character( victim->array->list[i] ) ) != NULL 
        && rch != victim && rch != ch && ch->Seen( rch )
        && rch->link != NULL ) 
        send( rch, "%s%s leaps to attack %s!!%s\n\r",
          damage_color( rch, ch, victim ), ch, victim, normal( rch ) );
    }
}


bool jump_feet( char_data* ch )
{
  if( ch->position != POS_RESTING && ch->position != POS_MEDITATING ) 
    return FALSE;

  send( ch, "You quickly jump to your feet.\n\r" );
  send_seen( ch, "%s jumps to %s feet.\n\r",
    ch, ch->His_Her( ) );

  ch->position = POS_STANDING;

  return TRUE;
} 


/*
 *   PLAYER FIGHT ROUTINES
 */


int player_round( char_data* ch, char_data* victim )
{
  int attack_skills [] = { SKILL_SECOND, SKILL_THIRD,
    SKILL_FOURTH, SKILL_FIFTH, SKILL_OFFHAND_ATTACK };

  obj_data*      wield  = NULL;
  obj_data*  secondary  = NULL;
  obj_data*     shield  = NULL;
  obj_data*       left;
  obj_data*      right;
  int            skill;
  int             rate;
  int           weight;
  int                i;

  if( is_set( ch->pcdata->pfile->flags, PLR_PARRY ) )
    return 25;

  if( ( left = ch->Wearing( WEAR_HELD_L ) ) != NULL ) {
    if( left->pIndexData->item_type == ITEM_WEAPON )
      secondary = left;
    else if( left->pIndexData->item_type == ITEM_ARMOR )
      shield = left;
    }

  if( ( right = ch->Wearing( WEAR_HELD_R ) ) != NULL ) { 
    if( right->pIndexData->item_type == ITEM_WEAPON )
      wield = right;
    else if( right->pIndexData->item_type == ITEM_ARMOR )
      shield = right;
    }

  if( wield == NULL
    || ( number_range( 0,2 ) == 0 && secondary != NULL ) )
    wield = secondary;

  if( wield != NULL ) {
    i      = ( shield == NULL && secondary == NULL ? 3 : 2 );
    weight = max( 0, wield->Weight( 1 )-10*i*ch->Strength( ) ) ;
    skill  = WEAPON_FIRST+wield->value[3];
    }
  else {
    weight = 0;
    skill  = WEAPON_UNARMED;
    }

  if( ( i = number_range( -1, 4 ) ) == -1 )
    ch->improve_skill( skill );
  else
    ch->improve_skill( attack_skills[i] );

  attack( ch, victim, get_attack( ch, wield ), wield, -1, ATT_PHYSICAL );

  skill = ch->shdata->skill[ skill ];
  rate  = 0;  

  for( i = 0; i < ( secondary == NULL ? 3 : 4 ); i++ )
    rate += ch->shdata->skill[ attack_skills[i] ]; 

  rate = 10+rate/2;
  rate = (320+weight/5-5*skill-5*ch->Dexterity( ))/rate;
  rate = number_range( rate/2, 3*rate/2 );
  
  return rate;
}


/*
 *   SPECIES FIGHT ROUTINES
 */


int mob_round( char_data* ch, char_data* victim )
{
  ch->shown     = 1;
  victim->shown = 1;

  if( switch_victim( ch, victim ) )
    return 16;

  var_mob    = ch;
  var_room   = ch->in_room;
  var_ch     = victim;
  var_victim = victim;

  execute( ch->species->attack );

  return 32;
}


bool switch_victim( char_data* ch, char_data* victim )
{
  char_data* rch;

  if( victim->pcdata == NULL
    || !is_set( victim->pcdata->pfile->flags, PLR_PARRY ) 
    || number_range( 0, 4 ) != 0 )
    return FALSE;

  for( int i = 0; ; i++ ) { 
    if( i >= *ch->array )
      return FALSE;
    if( ( rch = character( ch->array->list[i] ) ) != NULL 
      && rch != victim && rch->fighting == ch ) 
      break;
    }

  ch->fighting = rch;

  fsend( victim, "%s stops attacking you and leaps to attack %s.", ch, rch );
  fsend( rch, "%s stops attacking %s and leaps to attack you.", ch, victim );
  fsend_seen( ch, "%s stops attacking %s and leaps to attack %s.",
    ch, victim, rch );

  return TRUE;
}


void add_round( species_data* species, int delay )
{
  if( number_range( 0,32 ) > delay )
    return;

  if( species->exp > 1e8 || species->rounds++ > 1e8 
    || species->damage > 1e8 || species->damage_taken > 1e8 ) {
    species->rounds          /= 2;
    species->damage          /= 2;
    species->damage_taken    /= 2;
    species->shdata->deaths  /= 2;
    species->shdata->kills   /= 2;
    species->exp             /= 2;
    species->special         /= 2;
    }
}


/*
 *   DAMAGE AND ATTACK NAME ROUTINES
 */


const char* get_attack( char_data* ch, obj_data* wield )
{
  int skill [] = { SKILL_DEMON_SLASH, SKILL_DRAGON_STRIKE, SKILL_EAGLE_CLAW,
    SKILL_BEAR_GRASP, SKILL_LOCUST_KICK, SKILL_TIGER_PAW };

  int  weapon;
  int       i;

  weapon = ( wield == NULL ? 0 : wield->value[3] );

  if( ch->species != NULL || weapon != 0 ) 
    return weapon_attack[weapon];

  if( ch->shdata->skill[ SKILL_TIGER_PAW ] != 0 ) 
    for( i = 0; i < 6; i++ ) {  
      if( number_range( 0, 63-9*i ) < ch->shdata->skill[ skill[i] ] ) {
        ch->improve_skill( skill[i] );
        return skill_table[ skill[i] ].name;
        }
      }

  if( wield == NULL && ch->shdata->race == RACE_LIZARD
    && ch->Wearing( WEAR_HANDS ) == NULL )
    return "claw";

  return "punch";
}


int get_damage( char_data* ch, obj_data* wield, char letter )
{
  int damage;

  if( wield != NULL ) {
    damage  = roll_dice( wield->value[1], wield->value[2] );
    }
  else {
    damage = roll_dice( 1, letter == 'c' ? 6 : 4 );
    }

  if( ch->species == NULL && ( wield == NULL || wield->value[3] == 0 ) ) {
    damage += ch->shdata->skill[SKILL_PUNCH]/3;
    switch( letter ) {
      case 't' : damage = roll_dice( 2, 6  );      break;
      case 'l' : damage = roll_dice( 2, 8  );      break;
      case 'b' : damage = roll_dice( 2, 10 );      break;
      case 'e' : damage = roll_dice( 2, 12 );      break;
      case 'd' : damage = roll_dice( 2, 14 );      break;
      case '*' : damage = roll_dice( 2, 16 );      break;
      }   
    }

  return damage;
}


void damage_modifiers( char_data* ch, char_data* victim, obj_data* wield,
  int& damage )
{

  damage += ch->Damroll( wield );
  
  if( ch->move > 0 && is_set( &ch->status, STAT_BERSERK ) ) {
    ch->move--;
    damage += 2+ch->shdata->skill[ SKILL_BERSERK ]/3;
    if( number_range( 1, 4 ) == 4 ) {
      if( ch->shdata->skill[ SKILL_FRENZY ] > 0 ) {
        ch->improve_skill( SKILL_FRENZY );
        damage += ch->shdata->skill[ SKILL_FRENZY ]/2;
        send( ch, "You attack with FRENZY!!\n\r" );
        send_seen( ch, "%s attacks with FRENZY!!\n\r",
          ch );
        }
      }
    }
  
  if( victim->position == POS_RESTING )
    damage = 3*damage/2;
  else if( victim->position <= POS_SLEEPING )
    damage *= 4;

  return;
}


float char_data :: Hitroll( obj_data* obj )
{
  float x;

  x = hitroll+float( Dexterity( ) )/3-3; 

  if( species == NULL
    && shdata->skill[SKILL_BLIND_FIGHTING] > 0 ) {
     x = x + 1+ float( shdata->skill[SKILL_BLIND_FIGHTING])/3 ;
     improve_skill( SKILL_BLIND_FIGHTING );
    }

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_UNARMED 
    && is_set( affected_by, AFF_AXE_PROF ) )
    x++;

  if( obj->value[3] == WEAPON_SWORD-WEAPON_UNARMED 
    && is_set( affected_by, AFF_SWORD_PROF ) )
    x++;

  x += obj->value[0];

  return x;
}


float char_data :: Damroll( obj_data* obj )
{
  float x;

  x = damroll+float( Strength( ) )/3-4;

  if( obj == NULL || obj->pIndexData->item_type != ITEM_WEAPON )
    return x;

  if( obj->value[3] == WEAPON_AXE-WEAPON_UNARMED 
    && is_set( affected_by, AFF_AXE_PROF ) )
    x++;

  if( obj->value[3] == WEAPON_SWORD-WEAPON_UNARMED 
    && is_set( affected_by, AFF_SWORD_PROF ) )
    x++;

  x += obj->value[0];

  return x;
}


/*
 *   MAIN BATTLE ROUTINES
 */


bool attack( char_data* ch, char_data* victim, const char* dt,
  obj_data* wield, int damage, int modifier, int type )
{
  int              roll;
  bool           active;
  bool         backstab;
  bool           charge;
  const char*  loc_name;

  if( ch->position < POS_FIGHTING )
    return FALSE;

  roll  = number_range( -250, 250 );
  roll += modifier;

  modify_roll( ch, victim, wield, roll );

  active   = can_defend( victim, ch );
  backstab = !strcmp( dt, "backstab" );
  charge = !strcmp( dt, "charge" );

  if( ( !backstab && !charge ) && type == ATT_PHYSICAL
    && misses_blindly( ch, victim, roll, dt ) )
    return FALSE;

  if( active && dodge( ch, victim, roll ) )
    return FALSE;

  if( active && tumble( ch, victim, roll ) )
    return FALSE;

  if( type == ATT_PHYSICAL ) {
    if( fire_shield( ch, victim )
    || ( ion_shield( ch, victim ) ) 
    || ( thorn_shield( ch, victim ) )  )
      return FALSE;

    if( damage_weapon( ch, wield )
      || ( active && !backstab && !charge && ( parry( ch, victim, roll ) 
      || ( active && !backstab && !charge && ( guard( ch, victim, roll )
      || block_shield( ch, victim, dt )
      || trip( ch, victim, wield ) ) ) ) ) )
      return FALSE;
    }

  if( active && ( !backstab && !charge  )
    && shadow_dance( ch, victim, dt ) )
    return FALSE;

  if( damage == -1 ) {
    damage = get_damage( ch, wield, dt[0] );
    if( backstab )
      damage *= backstab_damage( ch );
    if( charge )
      damage *= charge_damage(ch);
    }

  damage_modifiers( ch, victim, wield, damage );
  damage = max( 1, damage );

  if( victim->species != NULL && victim->hit > 0
    && !is_set( &victim->status, STAT_PET ) )
    victim->species->damage_taken += min( victim->hit, damage );
  if( ch->species != NULL )
    ch->species->damage += damage/2;

  switch( type ) {
    case ATT_PHYSICAL:
      critical_hit( ch, victim, wield, damage );
      power_strike( ch, victim, wield, damage );
      barkskin( victim, damage );
      invulnerable( victim, damage );
      protgood( victim, ch, damage );
      protevil( victim, ch, damage );
      if( absorb_shield( victim, ch, dt, damage )
        || absorb_armor( victim, ch, dt, damage, loc_name ) )
        return FALSE; 
      dam_local( victim, ch, damage, dt, loc_name );
      if( !inflict( victim, ch, damage, "" ) 
        && !flaming_weapon( ch, victim, wield ) ) { 
        stun( ch, victim, dt ); 
        trigger_hit( ch, victim, wield );
        }
      break;

    case ATT_ACID:
      damage_acid( victim, ch, damage, dt );
      break;

    case ATT_COLD:
      damage_cold( victim, ch, damage, dt );
      break;

    case ATT_SHOCK:
      damage_shock( victim, ch, damage, dt );
      break;

    case ATT_FIRE:
      damage_fire( victim, ch, damage, dt );
      break;

    default:
      bug( "Attack: Impossible attack type." );
      break;
    }

  if( ch->species != NULL )
    ch->species->damage -= damage/2;
  
  return( damage > 0 );
}


void modify_roll( char_data* ch, char_data* victim, obj_data* wield,
  int& roll )
{
  int weapon = ( wield == NULL ?  0 : wield->value[3] );

  roll -= victim->mod_armor;
  roll += ch->shdata->level+5*ch->Hitroll( wield );
  roll += ( ch->species == NULL
    ? 8*ch->get_skill( WEAPON_UNARMED+weapon ) : 80 );

  if( victim->position != POS_FIGHTING )
    roll += 50;
 
  if( victim->position <= POS_SLEEPING )
    roll += 100;

  return;
}


bool can_defend( char_data* victim, char_data* ch )
{
  if(  victim->position < POS_FIGHTING
    || is_set( victim->affected_by, AFF_ENTANGLED )
    || !ch->Seen( victim ) )
    return FALSE;

  return TRUE;
}


/*
 *   WEAPON ROUTINES
 */


bool damage_weapon( char_data* ch, obj_data*& wield )
{
  if( wield == NULL || number_range( 0, 15-4*wield->rust )
    > !is_set( wield->extra_flags, OFLAG_SANCT )
    || number_range( 1,4+wield->value[0] ) > 4
    || --wield->condition > 0 )
    return FALSE;

  send( ch, "%s you are wielding shatters into pieces.\n\r", wield );
  send_seen( ch, "%s which %s is wielding shatters into pieces.\n\r",
    wield, ch );

  wield->Extract( );
  wield = NULL;

  return TRUE;
}


void trigger_hit( char_data* ch, char_data* victim, obj_data* wield )
{
  oprog_data* oprog;

  if( wield == NULL || ch->fighting != victim )
    return;

  for( oprog = wield->pIndexData->oprog; oprog != NULL;
    oprog = oprog->next ) {
    if( oprog->trigger == OPROG_TRIGGER_HIT ) { 
      var_ch     = ch;
      var_victim = victim;
      var_obj    = wield;    
      var_room   = ch->in_room;
      execute( oprog );
      }
    }

  return;
}


bool flaming_weapon( char_data*, char_data*, obj_data* wield )
{
  if( wield == NULL || !is_set( wield->extra_flags, OFLAG_FLAMING ) )
    return FALSE;

  return FALSE;
}


/* 
 *   ARMOR ROUTINES
 */


bool absorb_armor( char_data* victim, char_data* ch, const char* dt,
  int& damage, const char*& loc_name )
{
  char      tmp  [ TWO_LINES ];
  int      roll;
  int  absorbed;
  int         i;
  int       loc;

  roll = number_range( 0, 1000 );

  if( victim->species == NULL
    || is_set( &victim->species->act_flags, ACT_HUMANOID ) ) {

         if( roll < 450 )  loc = WEAR_BODY;
    else if( roll < 600 )  loc = WEAR_HEAD;
    else if( roll < 700 )  loc = WEAR_LEGS;
    else if( roll < 750 )  loc = WEAR_FEET;
    else if( roll < 850 )  loc = WEAR_HANDS;
    else if( roll < 990 )  loc = WEAR_ARMS;
    else if( roll < 1000 ) loc = WEAR_FINGER_L;
    else                   loc = WEAR_FINGER_R;

    absorbed  = damage_armor( victim, loc );
    damage   -= absorbed;

    if( damage <= 0 ) {
      send( victim, "Your %s armor absorbs %s's %s.\n\r",
        dam_loc_name[loc], ch->Name( victim ), dt );
      send( ch, "%s's %s armor absorbs your %s.\n\r",
        victim->Name( ch ), dam_loc_name[loc], dt );
      send( *ch->array, "%s's %s armor absorbs %s's %s.\n\r",
        victim, dam_loc_name[loc], ch, dt );
      return TRUE;
      }

    if( absorb_bracers( victim, ch, damage, absorbed, dt ) )
      return TRUE;

    loc_name = dam_loc_name[loc];

    return FALSE;
    }
    
  for( i = 0; i < MAX_ARMOR-1; i++ ) 
    if( roll < victim->species->chance[i] )
       break;

  damage -= number_range( 0, victim->species->armor[i] );

  if( damage > 0 ) {
    loc_name = victim->species->part_name[i];
    return FALSE;
    }

  if( *victim->species->part_name[i] != '\0' ) {
    sprintf( tmp, "Your %s absorbs %s's attack.\n\r",
      victim->species->part_name[i], ch->Name( victim ) );
    spam_char( victim, tmp );
    sprintf( tmp, "You %s %s's %s inflicting no damage.\n\r",
      dt, victim->Name( ch ), victim->species->part_name[i] );
    spam_char( ch, tmp );
    sprintf( tmp, "%%s's %s absorbs %%s's %s.\n\r",
      victim->species->part_name[i], dt );
    spam_room( tmp, victim, ch );
    }
  else {
    spam_char( victim, "Your armor absorbs %s's attack.\n\r", ch );
    sprintf( tmp, "You %s %s inflicting no damage.\n\r",
      dt, victim->Name( ch ) );
    spam_char( ch, tmp );
    spam_room( "%s's armor absorbs %s's attack.\n\r", victim, ch );
    }

  return TRUE;
}


int damage_armor( char_data* ch, int loc )
{
  obj_data*     armor;
  int        absorbed  = 0;

  for( int i = ch->wearing-1; i >= 0; i-- ) {
    if( ( armor = object( ch->wearing[i] ) ) != NULL
      && armor->position == loc ) {
      if( armor->pIndexData->item_type == ITEM_ARMOR )
        absorbed += number_range( 0, armor_class( armor ) );
      if( number_range( 0, 15-4*armor->rust )
        <= !is_set( armor->extra_flags, OFLAG_SANCT )
        && number_range( 1,4+armor->value[0] ) <= 4
        && --armor->condition < 0 ) {
        send( ch, "%s you are wearing shatters into pieces.\n\r", armor );
        send_seen( ch, "%s which %s is wearing shatters into pieces.\n\r",
          armor, ch );
        armor->Extract( );
        }
      }
    }

  return absorbed;
}


/*
 *   ATTACK ROUTINES
 */


void critical_hit( char_data* ch, char_data* victim, obj_data* wield,
  int& damage )
{
  int  weapon;

  if( ch->species != NULL
    || ch->shdata->skill[SKILL_CRITICAL_HIT] == 0 )
    return;

  weapon = ( wield == NULL ? 0 : wield->value[3] );

  if( number_range( 1,50 ) > ch->get_skill( WEAPON_UNARMED+weapon ) )
    return; 

  if( ch->shdata->skill[SKILL_DEATH_STRIKE] != 0     
    && number_range( 0, 30 ) < ch->get_skill( SKILL_DEATH_STRIKE ) ) {
    send( ch, "You call upon the SHADOWS to DESTROY %s!\n\r", victim );
    send( victim, "%s calls upon the SHADOWS to DESTROY you!\n\r", ch );
    send( *ch->array, "%s calls upon the SHADOWS to DESTROY %s!\n\r",
      ch, victim );
    ch->improve_skill( SKILL_DEATH_STRIKE );
    damage *= 8;
    }
  else if( number_range( 0, 20 ) < ch->get_skill( SKILL_CRITICAL_HIT ) ) {
    send( ch, "You CRITICALLY hit %s!\n\r", victim );
    send( victim, "%s CRITICALLY hits you!\n\r", ch );
    send( *ch->array, "%s CRITICALLY hits %s!\n\r", ch, victim );
    ch->improve_skill( SKILL_CRITICAL_HIT );
    damage *= 5;
    }
}


void power_strike( char_data* ch, char_data* victim, obj_data* wield,
  int& damage )
{
  int weapon;


  if( ch->species != NULL
    || ch->shdata->skill[SKILL_POWER_STRIKE] == 0 )
    return;

  weapon = ( wield == NULL ? 0 : wield->value[3] );
  
  if( number_range( 1,75 ) > ch->get_skill( WEAPON_UNARMED+weapon ) )
    return;

  if( number_range( 0, 25 ) < ch->get_skill( SKILL_POWER_STRIKE ) ) {
    send(ch, "Your POWER STRIKE hits %s!\n\r", victim );
    send( victim, "%s hits you with a POWER STRIKE!\n\r", ch );
    send( *ch->array, "%s hits %s with a POWER STRIKE!\n\r", ch, victim );
    ch->improve_skill( SKILL_POWER_STRIKE );
    damage *= 3;
    }
}


/*
 *   OFFENSIVE ROUTINES
 */


void stun( char_data* ch, char_data* victim, const char* dt )
{
  char        tmp  [ MAX_INPUT_LENGTH ];
  char_data*  rch;

  if( victim->position <= POS_STUNNED
    || victim->shdata->race == RACE_PLANT )
    return;

  if( ch->pcdata == NULL
    || ch->shdata->skill[SKILL_STUN] == 0
    || ch->get_skill( SKILL_STUN ) <= number_range( 0, 100
    +5*victim->shdata->level ) ) 
    return;

  ch->improve_skill( SKILL_STUN );

  if( victim->Seen( ch ) ) 
    send( ch, "-* Your %s momentarily STUNS %s! *-\n\r",
      dt, victim->Seen_Name( ch ) ); 

  send( victim, "%s's %s momentarily STUNS you!\n\r",
    ch->Name( victim ), dt );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL 
      || rch == ch || rch == victim || rch->link == NULL
      || !victim->Seen( rch ) )
      continue;
    sprintf( tmp, "%s's %s momentarily STUNS %s.\n\r",
      ch->Name( rch ), dt, victim->Seen_Name( rch ) );
    send( tmp, rch ); 
    }

  disrupt_spell( victim );
  set_delay( victim, 40 ); 
}


bool trip( char_data* ch, char_data* victim, obj_data* wield )
{

  if( victim->position < POS_FIGHTING )
    return FALSE;
 
  if( ch->species == NULL )  {
    if( ( wield != NULL &&  wield->pIndexData->value[3] != 0 )
    && ch->shdata->skill[SKILL_TRIP] == 0 ) 
    return FALSE;

    if( victim->species != NULL
      && ( !is_set( &victim->species->act_flags, ACT_HUMANOID ) ) ) {
      if( ch->shdata->skill[SKILL_TRIP] == 0 ) 
      return FALSE; }
  }
 
 
  if( victim->species != NULL
    && ( is_set( &victim->species->act_flags, ACT_GHOST ) ) )
    return FALSE;

  if( ch->species != NULL  
    && ( !is_set( &ch->species->act_flags, ACT_HUMANOID ) 
    || is_set( &ch->species->act_flags, ACT_GHOST ) ) )
    return FALSE; 

  if( ch->species == NULL ) {
    if( number_range( 0, 500 ) > 2*ch->get_skill( SKILL_TRIP ) +
      ch->get_skill( SKILL_KICK ) )
      return FALSE; }
  else
    if( number_range( 0, 200 ) > 5 )
      return FALSE;

  send( ch,
    "With a well timed kick you knock the feet out from under %s!\n\r",
    victim );
  fsend( victim, "%s delivers a sweeping kick to your feet, knocking you to\
 the ground!", ch );

  send( *ch->array, "%s kicks %s's leg, knocking %s to the ground.\n\r",
    ch, victim, victim->Him_Her( ) );

  if( ch->species == NULL &&
      ch->shdata->skill[SKILL_TRIP] > 0 )
    ch->improve_skill( SKILL_TRIP );

  disrupt_spell( victim );

  victim->position = POS_RESTING;

  return TRUE;
} 


int charge_damage( char_data* ch )
{
  int i;

  if( ch->species != NULL )
    return 5;

  ch->improve_skill( SKILL_CHARGE );

  i = 10*ch->shdata->skill[ SKILL_CHARGE ] + ch->shdata->level/2 +
    ch->Strength( );
  i = 1+i/25;
  return i;

}

int backstab_damage( char_data* ch )
{
  int i;

  if( ch->species != NULL )
    return 5;

  if( ch->shdata->skill[ SKILL_ASSASSINATE] == 0 )
    ch->improve_skill( SKILL_BACKSTAB );
  else
    ch->improve_skill( SKILL_ASSASSINATE );

  i = 10*ch->shdata->skill[ SKILL_BACKSTAB ]
    +10*ch->shdata->skill[ SKILL_ASSASSINATE ]
    +ch->shdata->level;

  i = 1+i/25;

  return i;
}


/*
 *   DEFENSIVE ROUTINES
 */


bool absorb_bracers( char_data* victim, char_data* ch, int& damage,
  int armor, const char* dt )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  obj_data*  bracers;

  if( ( bracers = victim->Wearing( WEAR_WRIST_L ) ) != NULL )
    armor -= bracers->value[2];

  if( ( bracers = victim->Wearing( WEAR_WRIST_R ) ) != NULL )
    armor -= bracers->value[2];

  if(  armor >= 0
    || ( damage -= number_range( 0, -armor ) ) > 0 )
    return FALSE;

  spam_char( victim,
    "Your bracers glow briefly and %s's %s is deflected.\n\r", 
    ch, dt );

  spam_char( ch,
    "%s's bracers glow briefly and your %s is magically deflected.\n\r",
    victim, dt );

  sprintf( tmp, "%%s's bracers glow briefly and %%s's %s is deflected.", dt );
  spam_room( tmp, victim, ch );

  return TRUE;
}


bool absorb_shield( char_data* victim, char_data* ch, const char* dt,
  int& damage )
{
  char tmp [ TWO_LINES ];

  if( !is_set( victim->affected_by, AFF_PROTECT ) )
    return FALSE;

  damage -= number_range( 0, 3 );

  if( damage > 0 ) 
    return FALSE;

  spam_char( victim,
    "The air crackles as %s's %s is mysteriously blocked.\n\r",
    ch, dt );

  spam_char( ch,
    "The air around %s crackles as your %s is mysteriously blocked.\n\r",
    victim, dt );

  sprintf( tmp,
    "The air crackles as %%s's %s is mysteriously blocked.\n\r", dt );
  spam_room( tmp, ch, victim );

  return TRUE;
}


bool ion_shield( char_data* ch, char_data* victim )
{
  if( IS_AFFECTED( victim, AFF_ION_SHIELD ) ) {
    damage_shock( ch, victim, roll_dice( 4,8 ), "electric spark" );
    if( ch->hit <= 0 )
      return TRUE;
    }

  return FALSE;
}


bool thorn_shield( char_data* ch, char_data* victim )
{
  if( IS_AFFECTED( victim, AFF_THORN_SHIELD ) ) {
    damage_acid( ch, victim, roll_dice( 3,6 ), "thorn shield" );
    if( ch->hit <= 0 )
      return TRUE;
    }

  return FALSE;
}


bool fire_shield( char_data* ch, char_data* victim ) 
{
  if( IS_AFFECTED( victim, AFF_FIRE_SHIELD ) ) {
    damage_fire( ch, victim, roll_dice( 2,4 ), "fire shield" );
    if( ch->hit <= 0 )
      return TRUE;
    }

  return FALSE;
}


bool block_shield( char_data* ch, char_data* victim, const char* dt )
{
  char          tmp  [ TWO_LINES ];
  obj_data*  shield;
  int          roll;

  if( ( shield = victim->Wearing( WEAR_HELD_L ) ) == NULL  
    || shield->pIndexData->item_type != ITEM_ARMOR )
    return FALSE; 

  if( ( roll = number_range( 0, 100 ) ) > 25 )
    return FALSE;

  if( roll > shield->value[1]/2+( victim->pcdata == NULL ? 7
    : victim->get_skill( SKILL_SHIELD ) ) ) 
    return FALSE;

  damage_armor( ch, WEAR_SHIELD );
  victim->improve_skill( SKILL_SHIELD ); 

  spam_char( victim, "%s's %s is blocked by your shield.\n\r", ch, dt );
  spam_char( ch, "%s blocks your %s with %s shield.\n\r", victim, dt,
    victim->His_Her( ) );
  sprintf( tmp, "%%s blocks %%s's %s with %s shield.\n\r",
    dt, victim->His_Her( ) );
  spam_room( tmp, victim, ch );

  return TRUE;
}    


bool shadow_dance( char_data* ch, char_data* victim, const char* dt )
{
  char tmp [ TWO_LINES ];

  if( victim->pcdata == NULL || number_range( 0, 35 )
    >= victim->shdata->skill[SKILL_SHADOW_DANCE] )
    return FALSE;
  
  victim->improve_skill( SKILL_SHADOW_DANCE );

  spam_char( victim, "You meld with the shadows avoiding %s's %s.\n\r",
    ch, dt );
  spam_char( ch, "%s melds with the shadows avoiding your %s.\n\r",
    victim, dt );
  sprintf( tmp, "%%s melds with the shadows avoiding %%s's %s.\n\r", dt );
  spam_room( tmp, victim, ch );
 
  return TRUE;
}


bool counter_attack( char_data* ch, char_data* victim, int& roll )
{
  return FALSE;

  /*
  if( victim->get_skill( SKILL_COUNTER_ATTACK ) < 1 )
    return FALSE;

  if( victim->species != NULL ) {
    if( victim->Wearing( WEAR_HELD_R ) != NULL )
      roll -= 25;
    }
  else
    roll -= 4*victim->get_skill( SKILL_COUNTER_ATTACK );

  if( roll >= 0 )
    return FALSE;
  
  spam_char( victim, "You counter-attack %s's attack!\n\r", ch );
  spam_char( ch, "%s counters your attack!\n\r", victim );
  spam_room( "%s counters %s's attack!\n\r", victim, ch );
  
  one_round( ch, victim );
 
  victim->improve_skill( SKILL_COUNTER_ATTACK );
  return TRUE;
  */
}


bool parry( char_data* ch, char_data* victim, int& roll )
{
  if( victim->species != NULL ) {    
    if( victim->Wearing( WEAR_HELD_R ) != NULL )
      roll -= 25; 
    }
  else
    roll -= 4*victim->get_skill( SKILL_PARRY );

  if( roll >= 0 ) 
    return FALSE;

  spam_char( victim, "You parry %s's attack.\n\r", ch );
  spam_char( ch, "%s parries your attack.\n\r", victim );
  spam_room( "%s parries %s's attack.\n\r", victim, ch );

  victim->improve_skill( SKILL_PARRY );

  return TRUE;
}


bool guard( char_data* ch, char_data* victim, int& roll )
{
  if( victim->species != NULL ) {
    if( victim->Wearing( WEAR_HELD_R ) != NULL )
      roll -= 30;
    }
  else
    roll -= 5*victim->get_skill( SKILL_GUARD );
  

  if( roll >= 0 )
    return FALSE;

  spam_char( victim, "You guard yourself from %s's attack.\n\r", ch );
  spam_char( ch, "%s guards from your attack.\n\r", victim );
  spam_room( "%s guards from %s's attack.\n\r", victim, ch );

  victim->improve_skill( SKILL_GUARD );

  return TRUE;
}


bool tumble( char_data* ch, char_data* victim, int& roll )
{
  int dex;

  if( victim->species != NULL  )
    return FALSE;

  if( victim->get_skill( SKILL_TUMBLE ) < 1 )
    return FALSE;

  dex = victim->Dexterity( );

  roll -= victim->get_skill( SKILL_TUMBLE)+4*dex-20;

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s tumbles away, avoiding your attack.\n\r", victim );
  spam_char( victim, "You tumble away, avoiding %s's attack.\n\r", ch );
  spam_room( "%s tumbles away, avoiding %s's attack.\n\r", victim, ch );

  victim->improve_skill( SKILL_TUMBLE );

  return TRUE;
}


bool dodge( char_data* ch, char_data* victim, int& roll )
{
  int dex;

  dex = victim->Dexterity( ); 

  if( victim->species == NULL ) 
    roll -= 2*victim->get_skill( SKILL_DODGE )+4*dex-20; 
  else 
    roll -= 4*dex; 

  if( roll > 0 )
    return FALSE;

  spam_char( ch, "%s dodges your attack.\n\r", victim );
  spam_char( victim, "You dodge %s's attack.\n\r", ch );
  spam_room( "%s dodges %s's attack.\n\r", victim, ch );

  victim->improve_skill( SKILL_DODGE );

  return TRUE;
}


bool misses_blindly( char_data* ch, char_data* victim, int& chance,
  const char* dt )
{
  if( chance < 0 ) {
    spam_char( victim, "%s's attack misses you.\n\r", ch );
    spam_char( ch, "You miss %s.\n\r", victim );
    spam_room( "%s's attack misses %s.\n\r", ch, victim );
    return TRUE;
    }

  if( !ch->in_room->Seen( ch ) && ( chance -= 30 ) < 0 ) {
    spam_char( ch,
      "You swing wildly in the dark missing everything.\n\r" );
    spam_room( "%s swings wildly missing everything.\n\r", ch, NULL );
    return TRUE;
    }

  if( !victim->Seen( ch ) ) {
    if( ( chance -= 30 ) < 0 ) {
      spam_char( ch,
        "You swing at your unseen victim, but hit nothing.\n\r" );
      spam_char( victim,
        "%s tries to hit you, but swings in the wrong direction.\n\r", ch );
      spam_room( "%s tries to hit %s, but swings in the wrong direction.\n\r",
        ch, victim );
      return TRUE;
      }
    }
  else {
    if( is_set( victim->affected_by, AFF_DISPLACE )
      && ( chance -= 10 ) < 0 ) {
      spam_char( ch,
        "Your %s seems to hit %s, but passes through %s.\n\r",
        dt, victim, victim->Him_Her( ) );
      spam_char( victim,
        "%s's %s strikes your displaced image, doing you no harm.\n\r",
        ch, dt );
      spam_room( "%s strikes the displaced image of %s.\n\r",
        ch, victim );
      return TRUE;
      }
    }
 
  return FALSE;
}


void invulnerable( char_data* ch, int& damage )
{
  if( IS_AFFECTED( ch, AFF_INVULNERABILITY ) ) {
    if( ch->species == NULL && ch->pcdata->clss == CLSS_CLERIC )
      damage = 9*damage/10;
    else  
      damage = 8*damage/10;
  }
 
  return;
}


void barkskin( char_data* ch, int& damage )
{
  if( IS_AFFECTED( ch, AFF_BARKSKIN ) ) 
    damage = 9*damage/10;

  return;
}


void protgood( char_data* ch, char_data* victim, int& damage )
{
  if( IS_AFFECTED( ch, AFF_PROTECT_GOOD ) && is_good( victim )
    && is_evil( ch ) )
    damage = 8*damage/10;

  return;
}


void protevil( char_data* ch, char_data* victim, int& damage )
{
  if(IS_AFFECTED( ch, AFF_PROTECT_EVIL ) && is_evil( victim )
    && is_good( ch ) )
    damage = 8*damage/10;

  return;
}


/*
 *   CONFUSED CHARACTER ROUTINE
 */

   
void confused_char( char_data* ch )
{
  /*
  char *word[] = { "hop", "pout", "smile", "growl", "sulk", "drool",
    "clap", "fume" };

  char_data*  rch;
  int           i  = number_range( 0, 24 );

  if( i < 8 ) {
    interpret( ch, word[i] );
    return;
    }

  if( i < 16 ) 
    if( ( rch = rand_victim( ch ) ) != NULL && rch != ch ) {
      start_fight( ch, rch );
      multi_hit( ch, rch );
      }
      */
  return;
}

   














