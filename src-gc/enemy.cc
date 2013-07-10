#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   ENEMY_DATA CLASS
 */


class Enemy_Data
{
 public:
  enemy_data*   next;
  pfile_data*   pfile;
  int           damage;
  bool          attacker;

  Enemy_Data( );
  ~Enemy_Data( );
}; 


Enemy_Data :: Enemy_Data( )
{
  record_new( sizeof( enemy_data ), MEM_ENEMY );

  damage = 0;

  return;
};


Enemy_Data :: ~Enemy_Data( )
{
  record_delete( sizeof( enemy_data ), MEM_ENEMY );

  return;
};


/*
 *   GENERIC ROUTINES
 */


bool is_enemy( char_data* ch, char_data* victim )
{
  enemy_data* enemy;

  for( enemy = ch->enemy; enemy != NULL; enemy = enemy->next )
    if( enemy->pfile == victim->pcdata->pfile )
      return TRUE;

  return FALSE;
}


int damage_done( char_data* ch, char_data* victim )
{
  enemy_data* enemy;

  for( enemy = victim->enemy; enemy != NULL; enemy = enemy->next )
    if( enemy->pfile == ch->pcdata->pfile )
      return enemy->damage;
  
  return -1;
}


/*  
 *   DELETION ROUTINES
 */


void clear_enemies( char_data* victim )
{
  mob_data*       ch;
  enemy_data*  enemy;

  if( victim->pcdata == NULL )
    return;

  for( int i = 0; i < mob_list; i++ ) {
    ch = mob_list[i];
    for( enemy = ch->enemy; enemy != NULL; enemy = enemy->next ) 
      if( enemy->pfile == victim->pcdata->pfile ) {
        remove( ch->enemy, enemy );
        break;
        }
    }

  return;
}


void extract( enemy_data*& list )
{
  enemy_data* enemy;

  for( ; list != NULL; ) {
    enemy = list;
    list  = list->next;

    delete enemy;
    }

  return;
}


/*
 *   CREATION ROUTINES
 */


void add_enemy( char_data* ch, char_data* victim )
{
  enemy_data* enemy;

  if( victim->species == NULL || ch->pcdata == NULL )
    return;

  for( enemy = victim->enemy; enemy != NULL; enemy = enemy->next )
    if( enemy->pfile == ch->pcdata->pfile )
      return;

  enemy          = new enemy_data;
  enemy->pfile   = ch->pcdata->pfile;
  enemy->next    = victim->enemy;
  victim->enemy  = enemy;

  return;
}  


void share_enemies( char_data* ch1, char_data* ch2 )
{
  enemy_data*  e1;
  enemy_data*  e2;

  if( !is_set( &ch2->species->act_flags, ACT_ASSIST_GROUP ) )
    return; 

  for( e1 = ch1->enemy; e1 != NULL; e1 = e1->next ) {
    for( e2 = ch2->enemy; ; e2 = e2->next ) {
      if( e2 == NULL ) {
        e2          = new enemy_data;
        e2->pfile   = e1->pfile;
        e2->next    = ch2->enemy;
        ch2->enemy  = e2;
        break;
        }
      if( e2->pfile == e1->pfile )
        break;
      } 
    }

  return;
}


void record_damage( char_data* victim, char_data* ch, int damage )
{
  enemy_data* enemy;

  if( victim->species == NULL || ch == NULL
    || ch->pcdata == NULL )
    return;
 
  for( enemy = victim->enemy; enemy != NULL; enemy = enemy->next )
    if( enemy->pfile == ch->pcdata->pfile ) {
      enemy->damage += damage;
      return;
      } 

  enemy          = new enemy_data;
  enemy->pfile   = ch->pcdata->pfile;
  enemy->damage  = damage;
  enemy->next    = victim->enemy;
  victim->enemy  = enemy;

  return;
}  









