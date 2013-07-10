#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   CONSOLIDATE ROUTINES
 */


void consolidate( thing_array& array )
{
  obj_data* obj;

  for( int i = array-1; i >= 0; i-- ) 
    if( ( obj = object( array[i] ) ) != NULL )
      consolidate( obj );
}


void consolidate( obj_data* obj1 ) 
{
  obj_data*  obj2;

  for( int i = 0; i < *obj1->array; i++ ) 
    if( ( obj2 = object( obj1->array->list[i] ) ) != NULL
      && obj1 != obj2 && is_same( obj1, obj2 ) ) {
      obj2->number += obj1->number;
      *obj1->array -= obj1;
      obj1->array   = NULL;
      obj1->Extract( );
      break;
      }
}




