#include "define.h"
#include "struct.h"


void* code_oload( void** argument )
{
  int       vnum  = (int) argument[0];
  obj_data*  obj;

  if( ( obj = create( get_obj_index( vnum ) ) ) != NULL ) 
    set_alloy( obj, 10 );

  return obj;
}


void* code_obj_value( void** argument )
{
  obj_data*  obj  = (obj_data*) argument[0];
  int          i  = (int)       argument[1];  

  if( obj == NULL ) {
    code_bug( "Obj_Value: Null object." );
    return NULL;
    }

  if( i < 0 || i > 3 ) {
    code_bug( "Obj_Value: Value out of range." );
    return NULL;
    }

  return (void*) obj->value[i];
}


void* code_junk_obj( void** argument )
{
  obj_data*     obj  = (obj_data*) argument[0];
  int             i  = (int)       argument[1];  

  if( obj == NULL ) {
    code_bug( "Junk_Obj: Null object." );
    return NULL;
    }

  if( i == NULL ) 
    obj->Extract( );
  else 
    obj->Extract( i );

  return NULL;
}


