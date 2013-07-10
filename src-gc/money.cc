#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


const int  coin_vnum  [] = { OBJ_COPPER, OBJ_SILVER, OBJ_GOLD, OBJ_PLATINUM }; 
const int   coin_value [] = { 1, 10, 100, 1000 };
const char*  coin_name [] = { "cp", "sp", "gp", "pp" };


/*
 *  VARIOUS MONEY ROUTINES
 */


int monetary_value( obj_data* obj )
{
  int i;

  if( obj->pIndexData->item_type == ITEM_MONEY ) 
    for( i = 0; i < MAX_COIN; i++ )
      if( obj->pIndexData->vnum == coin_vnum[i] ) 
        return obj->selected*coin_value[i];

  return 0;
}


char* coin_phrase( char_data* ch )
{
  int      coins  [ MAX_COIN ];
  obj_data*  obj;

  vzero( coins, MAX_COIN );

  for( int i = 0; i < ch->contents; i++ ) {
    if( ( obj = object( ch->contents[i] ) ) != NULL
      && obj->pIndexData->item_type == ITEM_MONEY ) 
      for( int j = 0; j < MAX_COIN; j++ )
        if( obj->pIndexData->vnum == coin_vnum[j] ) 
          coins[j] += obj->number;
    }

  return coin_phrase( coins );
}


char* coin_phrase( int* num )
{
  static char   buf  [ ONE_LINE ];
  bool         flag  = FALSE;
  int             i;
  int          last;
 
  for( last = 0; last < MAX_COIN; last++ )
    if( num[ last ] != 0 )
      break;

  buf[ 0 ] = '\0';

  for( i = MAX_COIN - 1; i >= last; i-- ) {
    if( num[ i ] == 0 )
      continue;
    sprintf( buf + strlen( buf ), "%s %s%d %s", flag ? "," : "",
      ( i == last && flag ) ? "and " : "", num[i], coin_name[i] );
    flag = TRUE;
    }

  if( !flag ) 
    sprintf( buf, " none" );

  return &buf[0];
}


int get_money( char_data* ch )
{
  obj_data*  obj;
  int        sum  = 0;

  for( int i = 0; i < ch->contents; i++ )
    for( int j = 0; j < 4; j++ )
      if( ( obj = object( ch->contents[i] ) ) != NULL
        && obj->pIndexData->vnum == coin_vnum[j] ) 
        sum += coin_value[j]*obj->number;

  return sum;
}


bool remove_silver( char_data* ch )
{
  obj_data* obj;

  if( ( obj = find_vnum( ch->contents, coin_vnum[1] ) ) != NULL ) {
    obj->Extract( 1 );
    return TRUE;
    }

  return FALSE;
}
 

void add_coins( char_data* ch, int amount, char* message )
{
  obj_data*  obj;
  int        num  [ 4 ];
  int          i;

  for( i = MAX_COIN - 1; i >= 0; i-- ) {
    if( ( num[i] = amount/coin_value[i] ) > 0 ) {
      amount -= num[i]*coin_value[i];
      obj = create( get_obj_index( coin_vnum[i] ), num[i] ); 
      obj->To( ch );
      consolidate( obj );
      }
    }

  if( message != NULL ) 
    send( ch, "%s%s.\n\r", message, coin_phrase( num ) );
}


bool remove_coins( char_data* ch, int amount, char* message ) 
{
  obj_data*       obj;
  obj_data*  coin_obj  [ 4 ];
  int           coins  [ 4 ];
  bool           flag  = FALSE;
  int             dum;
  int          number  [] = { 0, 0, 0, 0 };
  int             pos  [ MAX_COIN ];
  int             neg  [ MAX_COIN ];
  int               i;

  vzero( coin_obj, MAX_COIN );
  vzero( coins,    MAX_COIN );
  vzero( number,   MAX_COIN );   

  for( i = 0; i < ch->contents; i++ ) {
    obj = (obj_data*) ch->contents[i];
    for( int j = 0; j < MAX_COIN; j++ ) 
      if( obj->pIndexData->vnum == coin_vnum[j] ) {
        coin_obj[j] = obj; 
        coins[j] = obj->number;
        }
    }
 
  for( i = 0; i < MAX_COIN && amount > 0; i++ ) {
    amount -= coins[i]*coin_value[i];
    number[i] = coins[i];
    }

  if( amount > 0 ) 
    return FALSE;

  amount = -amount;
   
  for( i--; i >= 0; i-- ) {
    dum = amount/coin_value[i];
    amount -= dum*coin_value[i];
    number[i] -= dum;
    }
    
  for( i = MAX_COIN - 1; i >= 0; i-- ) {
    if( number[i] > 0 )
      coin_obj[i]->Extract( number[i] );
    pos[i] = max( 0, number[i] );
    neg[i] = max( 0, -number[i] );
    if( neg[i] != 0 ) {
      if( coin_obj[i] == NULL ) {
        obj = create( get_obj_index( coin_vnum[i] ), neg[i] );
        obj->To( ch );
        consolidate( obj );
        }
      else
        coin_obj[i]->number += neg[i];
      flag = TRUE;
      }
    }

  if( message != NULL ) {
    fsend( ch, "%s%s.\n\r", message, coin_phrase( pos ) );
    if( flag ) 
      send( ch, "You receive%s in change.\n\r", coin_phrase( neg ) );
    }

  return TRUE;
}


void do_split( char_data* ch, char* argument )
{
  int amount;

  if( *argument == '\0' ) {
    send( ch, "What amount do you wish to split?\n\r" );
    return;
    }

  amount = atoi( argument );

  if( amount < 2 ) {
    send( ch, "It is difficult to split anything less than 2 cp.\n\r" );
    return;
    }
 
  if( get_money( ch ) < amount ) {
    send( ch, "You don't have enough coins to split that amount.\n\r" );
    return;
    }

  split_money( ch, amount, TRUE );
}


void split_money( char_data* ch, int amount, bool msg )
{
  /*
  char_data*      gch;
  char_data*    group  = NULL;
  obj_data*       obj;
  obj_data*  coin_obj  [ 4 ];
  int      coins_held  [ 4 ];
  int     coins_split  [ 4 ];
  int           split;
  int           total;
  int               i;
  int         members  = 1;
  bool       anything  = FALSE;
  bool          found;
  char*        phrase;

  if( amount == 0 )
    return;

  for( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    if( gch != ch && is_same_group( gch, ch ) && ( gch->species == NULL
      || !is_set( &gch->status, STAT_PET ) ) && gch->Seen( ch ) ) {
      members++;
      gch->next_list = group;
      group = gch;
      }

  if( members < 2 ) {
    if( msg )
      send( ch, "There is noone here to split the coins with.\n\r" );
    return;
    }
  
  for( i = 0; i < 4; i++ ) {
    coin_obj[i]   = NULL;
    coins_held[i] = 0;
    } 

  for( obj = ch->contents; obj != NULL; obj = obj->next_content )
    for( i = 0; i < 4; i++ ) 
      if( obj->pIndexData->vnum == coin_vnum[i] ) {
        coin_obj[i] = obj; 
        coins_held[i] = obj->number;
        }

  split = amount/members;

  for( gch = group; gch != NULL; gch = gch->next_list ) {
    total = 0;  
    found = FALSE;

    for( i = MAX_COIN-1; i >= 0; i-- ) {
      coins_split[i] = min( ( split-total )/coin_value[i], coins_held[i] );
      if( coins_split[i] != 0 ) {
        total += coin_value[i]*coins_split[i];
        coins_held[i] -= coins_split[i];  
        obj = remove( coin_obj[i], coins_split[i] );
        set_owner( obj, gch, ch );
        put_obj( obj, gch );      
        found = TRUE;
        }
      }

    if( found ) { 
      phrase = coin_phrase( coins_split );
      send( ch, "You give%s to %s.\n\r", phrase, gch );
      send( gch, "%s gives%s to you.\n\r", ch, phrase );
      send( *ch->array, "%s gives%s to %s.\n\r", ch, phrase, gch );
      anything = TRUE;
      }
    }

  if( !anything )
    send( ch, "You lack the correct coins to split that amount.\n\r" );
  */
  return;
}




