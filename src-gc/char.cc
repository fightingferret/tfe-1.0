#include <sys/types.h>
#include <stdio.h>
#include "define.h"
#include "struct.h"


const char* sex_name [ MAX_SEX ] = {
  "neutral", "male", "female", "random" };

const char *size_name [ MAX_SIZE ] = {
  "Ant", "Rat", "Dog", "Gnome", "Human", "Ogre",
  "Horse", "Giant", "Elephant", "Dinosaur" };


Array<Thing_Data*> extracted;


/*
 *   EXTERNAL ROUTINES
 */


/*
 *   CHARACTER DATA
 */


Char_Data :: Char_Data( ) : wearing( this )
{
  descr   = NULL;
  shdata  = NULL;
  pcdata  = NULL;

  position  = POS_STANDING;

  logon      = current_time;
  save_time  = current_time;

  cast          = NULL;
  link          = NULL;
  enemy         = NULL;
  leader        = NULL;
  mount         = NULL;
  rider         = NULL;
  next_on_obj   = NULL;
  pos_obj       = NULL;
  prepare       = NULL;
  species       = NULL;
  pShop         = NULL;
  reply         = NULL;
  reset         = NULL;
  fighting      = NULL;

  was_in_room   = NULL;
  in_room       = NULL; 

  pet_name = empty_string;

  status        = 0;
  damroll       = 0;
  exp           = 0; 
  hit           = 0;
  hitroll       = 0;
  mana          = 0;
  max_hit       = 0;
  max_mana      = 0;
  max_move      = 0;
  mod_con       = 0;
  mod_dex       = 0;
  mod_int       = 0;
  mod_str       = 0;
  mod_wis       = 0;
  mod_armor     = 0;
  mod_hit       = 0;
  mod_mana      = 0;
  mod_move      = 0;
  move          = 0;
  played        = 0;
  timer         = 0;
  move_regen    = 0;
  mana_regen    = 0;
  hit_regen     = 0;

  vzero( mod_resist, MAX_RESIST );
  vzero( affected_by, AFFECT_INTS );

  active.func  = next_action;
  active.owner = this;
}


Char_Data :: ~Char_Data( )
{
  if( position == POS_DELETED ) {
    roach( "~Char_data : Deleting Twice!?" );
    return;
    }

  position = POS_DELETED;
}


/*
 *   PLAYER_DATA
 */


Player_Data :: Player_Data( char* name ) :
  locker( this ), junked( this )
{
  record_new( sizeof( player_data ), MEM_PLAYER );

  player_list += this;

  /*-- INITIALISE VARIABLES --*/

  valid      = PLAYER_DATA;
  switched   = NULL;
  familiar   = NULL;
  note_edit  = NULL;

  atalk   = NULL;
  gtell   = NULL;
  ctell   = NULL;
  chant   = NULL;
  chat    = NULL;
  gossip  = NULL;
  yell    = NULL;
  shout   = NULL;
  say     = NULL;
  tell    = NULL;
  to      = NULL;
  whisper = NULL;

  base_age   = 17;
  bank       = 0;
  noteboard  = 0;
  gossip_pts = 50;
  prayer     = 500;
  whistle    = 0;
  timezone   = 0;

  vzero( iflag, 2 );

  pcdata = new pc_data;
  shdata = new share_data;
  descr  = new descr_data;

  descr->name = alloc_string( name, MEM_DESCR );

  pcdata->pfile        = NULL;
  pcdata->help_edit    = NULL;
  pcdata->mail_edit    = NULL;
  pcdata->recognize    = NULL;

  pcdata->clss          = 0;
  pcdata->mod_age       = 0; 
  pcdata->piety         = 0;
  pcdata->speaking      = 0;
  pcdata->trust         = 0;
  pcdata->quest_pts     = 0;
  pcdata->terminal      = 0;
  pcdata->practice      = -1;
  pcdata->prac_timer    = 5;
  pcdata->religion      = REL_NONE;
  pcdata->lines         = 24;
  pcdata->max_level     = -1;
  pcdata->wimpy         = 0;

  vzero( pcdata->cflags, MAX_CFLAG );
  vzero( pcdata->color, MAX_COLOR );
  vzero( shdata->skill, MAX_SKILL );
  vzero( pcdata->quest_flags, MAX_QUEST );

  pcdata->condition[ COND_ALCOHOL ] = 0;
  pcdata->condition[ COND_FULL ]    = 24;
  pcdata->condition[ COND_THIRST ]  = 24;
  pcdata->condition[ COND_DRUNK ]   = 0;

  pcdata->tmp_short     = empty_string;
  pcdata->tmp_keywords  = empty_string;
  pcdata->title         = empty_string;
  pcdata->prompt        = empty_string;
  pcdata->buffer        = empty_string;

  pcdata->message       = ( 1 << MAX_MESSAGE )-1;
  pcdata->mess_settings = 0;
}


Player_Data :: ~Player_Data( )
{
  record_delete( sizeof( player_data ), MEM_PLAYER );
  player_list -= this;
}


/*
 *   WIZARD_DATA
 */


Wizard_Data :: Wizard_Data( char* name ) : player_data( name ) 
{
  record_new( sizeof( wizard_data ), MEM_WIZARD );
  record_delete( sizeof( player_data ), MEM_PLAYER );

  valid        = WIZARD_DATA;

  action_edit  = NULL;
  adata_edit   = NULL;
  room_edit    = NULL;
  mpdata_edit  = NULL;
  mprog_edit   = NULL;
  oextra_edit  = NULL;
  opdata_edit  = NULL;
  oprog_edit   = NULL;
  player_edit  = NULL;
  quest_edit   = NULL;
  obj_edit     = NULL;
  mob_edit     = NULL;
  exit_edit    = NULL;

  custom_edit  = 0;
  rtable_edit  = -1;
  list_edit    = 0;
  office       = 0;
  wizinvis     = 0;

  vzero( table_edit, 2 );

  bamfin       = empty_string;
  bamfout      = empty_string;
  level_title  = empty_string;

  build_chan   = NULL;
  imm_talk     = NULL;
  god_talk     = NULL;
  avatar       = NULL;

  vzero( permission, 2 );
}


Wizard_Data :: ~Wizard_Data( )
{
  record_delete( sizeof( wizard_data ), MEM_WIZARD );
  record_new( sizeof( player_data), MEM_PLAYER );
}


/*
 *   SHARE_DATA
 */


Share_Data :: Share_Data( )
{
  record_new( sizeof( share_data ), MEM_SHDATA );

  strength      = 10;
  intelligence  = 10; 
  wisdom        = 10;
  dexterity     = 10;
  constitution  = 10;
  deaths        = 0;
  kills         = 0;
  level         = 0;
  fame          = 0;
  race          = RACE_HUMAN;

  vzero( resist, MAX_RESIST );
}  


Share_Data :: ~Share_Data( )
{
  record_delete( sizeof( share_data ), MEM_SHDATA );
}


/*
 *   AGE
 */

 
int Player_Data :: Age( )
{
  return base_age+( played+current_time
    -logon )/144000+pcdata->mod_age;
}


/*
 *   RESISTANCES
 */


void calc_resist( char_data* ch )
{
  int i, j;

  for( i = 0; i < MAX_RESIST; i++ ) {
    ch->shdata->resist[i] = 0;

    if( ( j = clss_table[ch->pcdata->clss].resist[i] ) != 0 )
      ch->shdata->resist[i] += (ch->shdata->level+20)*j/10;
     
    if( ch->shdata->race < MAX_PLYR_RACE )
      ch->shdata->resist[i] += plyr_race_table[ch->shdata->race].resist[i];
    }
}    


int Char_Data :: Save_Magic( )
{
  int res;

  res = min( 100, shdata->resist[RES_MAGIC]+mod_resist[RES_MAGIC]
    +Intelligence( )-12 );

  return res;
}


int Char_Data :: Save_Fire( )
{
  int res;
 
  res = min( 100, shdata->resist[RES_FIRE]+mod_resist[RES_FIRE] );

  if( is_set( affected_by, AFF_RESIST_FIRE ) )
    res = 33+2*res/3;

  return res;
}


int Char_Data :: Save_Cold( )
{
  int res;

  res = min( 100, shdata->resist[RES_COLD]+mod_resist[RES_COLD] );

  if( is_set( affected_by, AFF_RESIST_COLD ) )
    res = 33+2*res/3;

  return res;
}


int Char_Data :: Save_Shock( )
{
  int res;
  
  res = min( 100, shdata->resist[RES_SHOCK]+mod_resist[RES_SHOCK] );

  if( is_set( affected_by, AFF_RESIST_SHOCK ) )
    res = 33+2*res/3;

  return res;

}


int Char_Data :: Save_Mind( )
{
  int i;

  i = shdata->resist[RES_MIND]+mod_resist[RES_MIND]
    +2*Intelligence( )-24;

  if( pcdata != NULL )
    i += 25;

  return( min( i, 100 ) );
}


int Char_Data :: Save_Acid( )
{
  int res;
  
  res = min( 100, shdata->resist[RES_ACID]+mod_resist[RES_ACID] );

  if( is_set( affected_by, AFF_RESIST_ACID ) )
    res = 33+2*res/3;
  
  return res;
}


int Char_Data :: Save_Poison( )
{
  int res;

  res = min( 100, shdata->resist[RES_POISON]
    +mod_resist[RES_POISON]+3*Constitution( )-40 );

  if( is_set( affected_by, AFF_RESIST_POISON ) )
    res = 33+2*res/3;

  return res;
}


/*
 *   ABILITY ROUTINES
 */


int Char_Data :: Strength( )
{
  int i = shdata->strength;

  if( is_set( affected_by, AFF_OGRE_STRENGTH ) )
    i = max( i, min( i+3, 17 ) );

  i -= affect_duration( this, AFF_DEATH )/2;

  return URANGE( 3, i+mod_str, 30 );
}


int Char_Data :: Intelligence( )
{
  return URANGE( 3, shdata->intelligence+mod_int, 30 );
}


int Char_Data :: Wisdom( )
{
  return URANGE( 3, shdata->wisdom+mod_wis, 30 );
}


int char_data :: Dexterity( )
{
  return URANGE( 3, shdata->dexterity+mod_dex-get_burden( ), 30 );
}


int char_data :: Constitution( )
{
  int i = shdata->constitution+mod_con;

  i -= affect_duration( this, AFF_DEATH )/2;

  return URANGE( 3, i, 30 );
}


/*
 *  HIT/MANA/MOVE
 */


void rejuvenate( char_data* ch )
{
  update_max_hit( ch );
  update_max_mana( ch );

  ch->hit  = ch->max_hit;
  ch->mana = ch->max_mana;

  update_max_move( ch );

  ch->move = ch->max_move;

  return;
}


void update_maxes( char_data* ch )
{
  if( ch != NULL ) {
    update_max_hit( ch );
    update_max_move( ch );
    update_max_mana( ch );
    }
}


void update_max_hit( char_data* ch )
{
  if( ch->species != NULL ) {
    ch->max_hit = ch->base_hit*ch->Constitution()/ch->shdata->constitution
      +ch->mod_hit;
    }
  else {
    ch->max_hit = max( 1, ch->base_hit+ch->mod_hit
      +ch->shdata->level*(ch->Constitution( )-12)/2 );
    }

  ch->hit = min( ch->hit, ch->max_hit );
}


void update_max_move( char_data* ch )
{
  int move;

  move = ch->base_move+ch->mod_move;
  move = ( ch->hit*move )/ch->max_hit;

  ch->max_move = max( 0, move );
  ch->move     = min( ch->move, ch->max_move );
}


void update_max_mana( char_data* ch )
{
  int           mana;

  mana  = ch->base_mana+ch->mod_mana+ch->shdata->level*ch->Intelligence( )/4;
  mana -= leech_max( ch );
  mana -= prep_max( ch );

  ch->max_mana = max( 0, mana );
  ch->mana     = min( ch->mana, ch->max_mana );
}


/*
 *   MISC ATTRIBUTES 
 */


int Mob_Data :: Size( )
{
  return species->size;
}


int Player_Data :: Size( )
{
  return( shdata->race < MAX_PLYR_RACE ?
    plyr_race_table[ shdata->race ].size : SIZE_OGRE );
}


/*
 *   MISC
 */


char_data* random_pers( room_data* room )
{
  int count  = 0;

  for( int i = 0; i < room->contents; i++ )
    if( character( room->contents[i] ) != NULL )
      count++;
  
  if( count == 0 )
    return NULL;

  count = number_range( 1, count );

  for( int i = 0; ; i++ )
    if( character( room->contents[i] ) != NULL && --count == 0 )
      return (char_data*) room->contents[i];
} 

















