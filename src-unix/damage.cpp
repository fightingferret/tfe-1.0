#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


const char*   condition_prep  ( char_data* );

void          msg_attack      ( char_data*, char_data*, const char*,
                                const char*, char );
void          msg_other       ( char_data*, char_data*, const char*,
                                const char*, char );

/*
 *   GENERAL MESSAGE ROUTINES
 */


void dam_local( char_data* victim, char_data* ch, int damage,
  const char* dt, const char* loc_name )
{
  char               tmp  [ TWO_LINES ];
  char             punct;
  const char*   dam_name;
  char_data*         rch;
  int               flag;

  if( *loc_name == '\0' ) {
    dam_message( victim, ch, damage, dt, lookup( physical_index, damage ) );
    return;
    }

  punct        = ( damage <= 24 ? '.' : '!' );
  dam_name     = lookup( physical_index, damage );
  victim->hit -= damage;

  sprintf( tmp, "%s's %s %s%s%s your %s%c\n\r",
    ch->Name( victim,1, TRUE ),
    dt, to_self( victim ), dam_name, normal( victim ),
    loc_name, punct );
  corrupt( tmp, TWO_LINES, "Dam_Local" );
  *tmp = toupper( *tmp );
  send( victim, tmp );
 
  if( victim->Seen( ch ) ) 
    sprintf( tmp, "Your %s %s%s%s %s's %s%c",
      dt, by_self( ch ), dam_name, normal( ch ),
      victim->Name( ch, 1, TRUE ), loc_name, punct );
  else
    sprintf( tmp, "Your %s %s%s%s someone%c",
      dt, by_self( ch ),
      dam_name, normal( ch ), punct );

  corrupt( tmp, TWO_LINES, "Dam_Local" );
  strcat_cond( tmp, ch, victim );

  flag = ( victim->pcdata == NULL && !is_set( &victim->status, STAT_PET )
    ? MSG_DAMAGE_MOBS : MSG_DAMAGE_PLAYERS );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch == ch || rch == victim || rch->link == NULL
      || !is_set( &rch->pcdata->message, flag ) || !rch->Can_See( ) )
      continue;
    if( victim->Seen( rch ) ) 
      sprintf( tmp, "%s's %s %s%s%s %s's %s%c",
        ch->Name( rch, 1, TRUE ), dt, damage_color( rch, ch, victim ),
        dam_name, normal( rch ), victim->Seen_Name( rch ),
        loc_name, punct );
    else
      sprintf( tmp, "%s's %s %s%s%s someone%c",
        ch->Name( rch ), dt, damage_color( rch, ch, victim ),
        dam_name, normal( rch ), punct );
    corrupt( tmp, TWO_LINES, "Dam_Local" );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
    }

  victim->hit += damage;

  return;
}


void dam_message( char_data* victim, char_data* ch, int damage,
  const char* dt, const char* word )
{
  char      tmp  [ ONE_LINE ];
  int         i;
  char    punct;

  victim->hit -= damage;

  strcpy( tmp, dt );
  if( ( i = strlen( tmp ) ) > 2 && tmp[i-2] == '\n' )
    tmp[i-2] = '\0';

  punct = ( damage <= 24 ? '.' : '!' );

  if( ch == NULL || *dt == '*' ) 
    msg_other( victim, ch, *tmp == '*' ? &tmp[1] : tmp, word, punct );
  else 
    msg_attack( victim, ch, tmp, word, punct );

  victim->hit += damage;

  return;
}  
 

void msg_attack( char_data* victim, char_data* ch, const char* dt,
  const char* word, char punct )
{
  char           tmp  [ MAX_INPUT_LENGTH ];
  char_data*     rch;
  int           flag;

  sprintf( tmp, "Your %s %s%s%s %s%c",
    dt, by_self( ch ), word, normal( ch ),
    victim->Name( ch ), punct );
  strcat_cond( tmp, ch, victim );

  sprintf( tmp, "%s's %s %s%s%s you%c\n\r",
    ch->Name( victim ), dt, to_self( victim ),
    word, normal( victim ), punct );
  *tmp = toupper( *tmp );
  send( tmp, victim );

  flag = ( victim->pcdata == NULL && !is_set( &victim->status, STAT_PET )
    ? MSG_DAMAGE_MOBS : MSG_DAMAGE_PLAYERS );

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch == ch || rch == victim || rch->link == NULL
      || !is_set( &rch->pcdata->message, flag ) || !rch->Can_See( ) )
      continue;
    sprintf( tmp, "%s's %s %s%s%s %s%c",
      ch->Name( rch ), dt,
      damage_color( rch, ch, victim ), word, normal( rch ),
      victim->Name( rch ), punct );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
    }

  return;
}


void msg_other( char_data* victim, char_data* ch, const char* dt,
  const char* word, char punct )
{
  char                tmp  [ TWO_LINES ];
  char_data*          rch;

  sprintf( tmp, "%s %s%s%s you%c\n\r", dt,
    to_self( victim ), word, normal( victim ), punct );
  corrupt( tmp, TWO_LINES, "Msg_Other" );
  *tmp = toupper( *tmp );
  send( tmp, victim );

  for( int i = 0; i < *victim->array; i++ ) {
    if( ( rch = character( victim->array->list[i] ) ) == NULL
      || rch == victim || !rch->Can_See( ) )
      continue;
    sprintf( tmp, "%s %s%s%s %s%c", dt,
      damage_color( rch, ch, victim ), word, normal( rch ),
      victim->Name( rch ), punct );
    corrupt( tmp, TWO_LINES, "Msg_Other" );
    *tmp = toupper( *tmp );
    strcat_cond( tmp, rch, victim );
    }

  return;
}


/*
 *   CONDITION ROUTINES
 */


void strcat_cond( char* tmp, char_data* ch, char_data* victim )
{
  const char*  cond_word  = condition_word( victim ); 
  bool           visible  = TRUE;
  int             length  = 0;
 
  for( int i = 0; tmp[i] != '\0'; i++ ) {
    if( visible ) {
      if( tmp[i] == '' )
        visible = FALSE;
      else
        length++;
      }
    else if( tmp[i] == 'm' )
      visible = TRUE;
    }

  send( ch, "%s%s[ %s ]\n\r", tmp,
    length+strlen( cond_word ) > 74 ? "\n\r" : " ", cond_word );

  return;
}
  

const char* condition_prep( char_data *ch )
{
  int percent = (100*ch->hit)/ch->max_hit;

  if( ch->hit <= -10 )
    return "is";

       if( percent >= 100 ) return "is in";
  else if( percent >=  90 ) return "is";
  else if( percent >=  80 ) return "has a";
  else if( percent >=  70 ) return "has";
  else if( percent >=  60 ) return "has";
  else if( percent >=  50 ) return "is";
  else if( percent >=  40 ) return "has";
  else if( percent >=  30 ) return "is";
  else if( percent >=  20 ) return "is";
  else if( percent >=  10 ) return "is";
  else if( percent >=   0 ) return "is";
                            return "is";
}


const char* condition_word( char_data* ch )
{
  int percent = (100*ch->hit)/ch->max_hit;

  if( ch->hit <= -11 )
    return "DEAD!";

       if( percent >= 100 ) return "perfect health";
  else if( percent >=  90 ) return "slightly scratched";
  else if( percent >=  80 ) return "few bruises";
  else if( percent >=  70 ) return "some cuts";
  else if( percent >=  60 ) return "several wounds";
  else if( percent >=  50 ) return "badly wounded";
  else if( percent >=  40 ) return "many nasty wounds";
  else if( percent >=  30 ) return "bleeding freely";
  else if( percent >=  20 ) return "covered in blood";
  else if( percent >=  10 ) return "leaking guts";
  else if( percent >=   0 ) return "mostly dead";
                            return "dying";

}


const char* condition_short( char_data* ch, char_data* victim )
{
  if( victim == NULL )
    return "-";

  if( victim->array != ch->array || !victim->Seen( ch ) )
    return "??";

  return condition_short( victim );
}


const char* condition_short( char_data* ch )
{
  int percent;

  if( ch->hit <= -11 )
    return "DEAD!";

  percent = (100*ch->hit)/ch->max_hit;

       if( percent >= 100 ) return "perfect";
  else if( percent >=  90 ) return "scratched";
  else if( percent >=  80 ) return "bruised";
  else if( percent >=  70 ) return "cut";
  else if( percent >=  60 ) return "wounded";
  else if( percent >=  50 ) return "badly wounded";
  else if( percent >=  40 ) return "nastily wounded";
  else if( percent >=  30 ) return "bleeding freely";
  else if( percent >=  20 ) return "covered in blood";
  else if( percent >=  10 ) return "leaking guts";
  else if( percent >=   0 ) return "mostly dead";
                            return "dying";
}


/*
 *   GENERAL INFLICT ROUTINE
 */


bool inflict( char_data* victim, char_data* ch, int dam, char* dt )
{
  if( victim->position == POS_DEAD )
    return TRUE;

  if( dam > 1000 ) {
    bug( "Inflict: more than 1000 points!" );
    dam = 1000;
    }
  else if( dam <= 0 )
    return FALSE;

  record_damage( victim, ch, dam );

  victim->hit -= dam;

  update_pos( victim );
  update_max_move( victim );

  set_bit( &victim->status, STAT_WIMPY );

  if( victim->position == POS_SLEEPING ) {
    strip_affect( victim, AFF_SLEEP );
    send( victim, "You are suddenly awakened by the feeling of pain.\n\r" );
    send_seen( victim, "%s wakes up.\n\r", victim );
    victim->position = POS_RESTING;
    renter_combat( victim );
    }

  /*
  if( IS_AFFECTED( victim, AFF_CONFUSED ) && dam > number_range( 0, 100 ) ) {
    strip_affect( victim, AFF_CONFUSED );
    send( "You are knocked to your senses.\n\r", victim );
    act( "$n is knocked to $s senses.", victim, NULL, NULL, TO_ROOM );
    victim->fighting = NULL;
    victim->aggressive = NULL;
    for( rch = victim->in_room->people; rch != NULL; rch = rch->next_in_room )
      if( IS_NPC( rch ) && !IS_SET( rch->status, STAT_PET ) ) {
        if( rch->fighting == ch )
          rch->fighting = NULL;
        if( rch->aggressive == ch )
          rch->aggressive = NULL;
        }
    }
    */
    
  switch( victim->position ) {
    case POS_MORTAL:
      send( victim,
        "You are mortally wounded, and will die soon, if not aided.\n\r" );
      send_seen( victim,
        "%s is mortally wounded, and will die soon, if not aided.\n\r",
        victim );
      break;
 
    case POS_INCAP:
      send( victim, 
        "You are incapacitated and will slowly die, if not aided.\n\r" );
      send_seen( victim,
        "%s is incapacitated and will slowly die, if not aided.\n\r",
        victim );
      break;

    case POS_STUNNED:
      send( victim, "You are stunned, but will probably recover.\n\r" );
      send_seen( victim,
        "%s is stunned, but will probably recover.\n\r",
        victim );
      break;

    case POS_DEAD:
      death_message( victim );
      break;

    default:
      if( dam > victim->max_hit/4 )
        send( "That really did HURT!\n\r", victim );
      if( victim->pcdata != NULL && victim->hit < victim->max_hit/4
        && is_set( &victim->pcdata->message, MSG_BLEEDING ) )
        send( victim, "You sure are BLEEDING!\n\r" );
      break;
    }

  if( victim->rider != NULL && victim->position < POS_FIGHTING ) {
    send( victim->rider,
      "You are thrown to the ground as your mount is %s.\n\r",
      victim->position == POS_DEAD ? "killed" : "incapacitated" );
    victim->rider->position = POS_RESTING;
    victim->rider->mount    = NULL;
    victim->rider           = NULL;
    }

  if( ch != NULL && ch->species != NULL ) 
    ch->species->damage += dam;

  if( victim->position == POS_DEAD ) {
    death( victim, ch, dt );
    return TRUE;
    }

  return FALSE;
} 


/*
 *   GLANCE ROUTINES
 */


void do_glance( char_data* ch, char* argument )
{
  char_data*  victim;

  if( *argument == '\0' ) {
    send( ch, "Who do you want to glance at?\n\r" );
    return;
    }

  if( ( victim = one_character( ch, argument, "glance",
    ch->array ) ) == NULL )
    return;

  glance( ch, victim );

  return;
}


void glance( char_data* ch, char_data* victim )
{
  if( victim == ch ) {
    page( ch, "Your condition is %s.\n\r", condition_word( ch ) );
    return;
    }

  page( ch, "%s %s %s%s%s%c\n\r",
    victim->Seen_Name( ch ), condition_prep( victim ),
    color_scale( ch, 6-6*victim->hit/victim->max_hit ),
    condition_word( victim ),
    normal( ch ), victim->hit > 0 ? '.' : '!' );

  return;
}


/*
 *   SPAM ROUTINES
 */


void spam_char( char_data* ch, const char* text )
{
  if( ch->link == NULL || !is_set( &ch->pcdata->message, MSG_MISSES ) )
    return;

  send( ch, text );

  return;  
}


void spam_room( const char* text, char_data* ch, char_data* victim )
{
  char         tmp  [ TWO_LINES ];
  char_data*   rch;

  for( int i = 0; i < *ch->array; i++ ) {
    if( ( rch = character( ch->array->list[i] ) ) == NULL
      || rch->link == NULL || rch == ch || rch == victim
      || rch->position <= POS_SLEEPING
      || !is_set( &rch->pcdata->message, MSG_MISSES ) )
      continue;
    sprintf( tmp, text, ch->Name( rch ),
      victim == NULL ? "[BUG]" : victim->Name( rch ) );
    corrupt( tmp, TWO_LINES, "Spam_Room" );
    *tmp = toupper( *tmp );
    send( tmp, rch );
    }

  return;
}

 
/*
 *   PHYSICAL DAMAGE ROUTINES
 */


index_data physical_index [] = 
{
  { "misses",               "miss",                  0 },
  { "scratches",            "scratch",               1 },
  { "grazes",               "graze",                 2 },
  { "hits",                 "hit",                   4 },
  { "injures",              "injure",                6 },
  { "wounds",               "wound",                10 },
  { "mauls",                "maul",                 15 },
  { "decimates",            "decimate",             21 },
  { "devastates",           "devastate",            28 },
  { "maims",                "maim",                 35 },
  { "SAVAGES",              "SAVAGE",               45 },
  { "CRIPPLES",             "CRIPPLE",              55 },
  { "MUTILATES",            "MUTILATE",             70 },
  { "DISEMBOWELS",          "DISEMBOWEL",           85 },
  { "* DISMEMBERS *",       "* DISMEMBER *",       100 },
  { "* EVISCERATES *",      "* EVISCERATE *",      120 },
  { "* MASSACRES *",        "* MASSACRE *",        145 },
  { "* PULVERIZES *",       "* PULVERIZE *",       170 },
  { "** DEMOLISHES **",     "** DEMOLISH **",      200 },
  { "** EXTIRPATES **",     "** EXTIRPATE **",     240 },
  { "*** OBLITERATES ***",  "*** OBLITERATE ***",  280 },
  { "*** ERADICATES ***",   "*** ERADICATE ***",   330 },
  { "*** ANNIHILATES ***",  "*** ANNIHILATE ***",   -1 }
};


bool damage_physical( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  dam_message( victim, ch, damage, string,
    lookup( physical_index, damage, plural ) );
  
  return inflict( victim, ch, damage, "physical damage" );
}



bool damage_magic( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  damage *= 100-victim->shdata->resist[RES_MAGIC];
  damage /= 100;

  dam_message( victim, ch, damage, string,
    lookup( physical_index, damage, plural ) );
  
  return inflict( victim, ch, damage, "shock" );
}


bool damage_mind( char_data* victim, char_data* ch, int damage,
  const char* string, bool plural )
{
  damage *= 100-victim->shdata->resist[RES_MIND];
  damage /= 100;

  dam_message( victim, ch, damage, string,
    lookup( physical_index, damage, plural ) );
  
  return inflict( victim, ch, damage, "shock" );
}








