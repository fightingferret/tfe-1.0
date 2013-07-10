#include "define.h"
#include "struct.h"


const char* burden_name [] = { "unburdened", "lightly burdened",
  "encumbered", "heavily burdened",
  "strained", "stressed", "over-taxed" };


/*
 *   CAN_CARRY FUNCTION
 */


bool can_carry( char_data* ch, obj_data *obj, bool msg )
{
//  int wght = obj->Weight( );
//  int num  = obj->Number( );
  /*
  if( ch->num_ins+num > ch->can_carry_n( ) ) {
    if( msg )
      send( ch, "[ %s: too many items. ]\r\n", obj );
    return FALSE;
    }

  if( ch->wght_ins+wght > ch->can_carry_w( ) ) {
    if( msg )
      send( ch, "[ %s: too heavy. ]\r\n", obj );
    return FALSE;
    }
  */
  return TRUE;
}


/*
 *   NUMBER FUNCTIONS
 */


int Thing_Data :: Number( int i )
{
  if( i == -1 )
    return number;

  return i;
}


int Obj_Data :: Number( int i ) 
{
  if( pIndexData->item_type == ITEM_MONEY )
    return 0;

  if( i == -1 )
    return number;

  return i;
}


/*
 *   BASE WEIGHT FUNCTIONS
 */


int thing_data :: Capacity( )          { return 0; }
int thing_data :: Empty_Capacity( )    { return 0; }
int Thing_Data :: Empty_Weight( int )  { return 0; }


int Thing_Data :: Weight( int )
{
  return contents.weight;
}


/*
 *   CHARACTER FUNCTIONS
 */


int Char_Data :: Empty_Capacity( )
{
  return 3000*Strength( );
}


int Char_Data :: Capacity( )
{
  return Empty_Capacity( )-contents.weight-wearing.weight/2;
}


int Char_Data :: Empty_Weight( int )
{
  int default_weight [] =
    { 10, 200, 2000, 5000, 15000, 35000,
      100000, 150000, 400000, 1000000 }; 

  int i  = 0;

  if( species != NULL ) {
    if( species->weight != 0 )
      i += species->weight;
    else 
      i += default_weight[ species->size ];
    }
  else {
    if( shdata->race < MAX_PLYR_RACE )
      i += plyr_race_table[ shdata->race ].weight;
    }

  return i;
}


int Char_Data :: Weight( int )
{
  int i;

  i  = contents.weight+wearing.weight;
  i += Empty_Weight( );

  return i;
}


/*
 *   OBJECT FUNCTIONS
 */


int Obj_Data :: Empty_Capacity( )
{
  return 100*pIndexData->value[0];
}


int Obj_Data :: Capacity( )
{
  return Empty_Capacity( )-contents.weight;
}


int Obj_Data :: Weight( int i )
{
  int sum  = contents.weight;

  if( pIndexData->item_type == ITEM_CONTAINER 
    && is_set( &value[1], CONT_HOLDING ) ) 
    sum /= 2;

  if( pIndexData->item_type == ITEM_DRINK_CON
    && value[1] > 0 )
    sum += int( 2.2*value[1] );

  sum += Empty_Weight( );
  sum *= ( i == -1 ? number : i );

  return sum; 
}


inline int metal_weight( obj_data* obj )
{
  for( int i = MAT_BRONZE; i <= MAT_ADAMANTINE; i++ )
    if( is_set( &obj->materials, i ) ) 
      return obj->weight*material_table[i].weight/10;

  return obj->weight;
}


int Obj_Data :: Empty_Weight( int )
{
  if( ( pIndexData->item_type != ITEM_WEAPON
    && pIndexData->item_type != ITEM_ARMOR )
    || !is_set( pIndexData->extra_flags, OFLAG_RANDOM_METAL ) )
    return weight;

  return metal_weight( this );
}

 












