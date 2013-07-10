#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


bool is_worn_armor       ( obj_data *obj );


bool is_worn_armor( obj_data* obj )
{
  switch( obj->position ) {
    case WEAR_BODY :
    case WEAR_HEAD :  
    case WEAR_LEGS :
    case WEAR_ARMS :
    case WEAR_SHIELD :
    case WEAR_ABOUT : 
    case WEAR_WAIST :
      return TRUE;
    }

  return FALSE;
}


int armor_class( obj_data* obj )
{ 
  int armor;

  armor = obj->value[1]*(5-obj->rust)/5+obj->value[0];

  return armor;
}


