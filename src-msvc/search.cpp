#include "define.h"
#include "struct.h"


/*
 *   RECURSIVE SEARCH FUNCTIONS
 */


bool search_oload( arg_type* arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int                     i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_oload( aif->yes, vnum ) || search_oload( aif->no, vnum ) 
      || search_oload( aif->condition, vnum ) )
      return TRUE;
    }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->func_call == &code_oload && afunc->arg[0] != NULL ) {
      if( int( afunc->arg[0]->value ) == vnum )
        return TRUE;
      }
    for( i = 0; i < 4 && afunc->arg[i] != NULL; i++ )
      if( search_oload( afunc->arg[i], vnum ) )
        return TRUE;
    }     

  return search_oload( arg->next, vnum );
}


bool search_mload( arg_type *arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;
   
  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_mload( aif->yes, vnum ) || search_mload( aif->no, vnum ) 
      || search_mload( aif->condition, vnum ) )
      return TRUE;
    }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( afunc->func->func_call == &code_mload && afunc->arg[0] != NULL ) {
      if( int( afunc->arg[0]->value ) == vnum )
        return TRUE;
      }
    for( i = 0; i < 4 && afunc->arg[i] != NULL; i++ )
      if( search_mload( afunc->arg[i], vnum ) )
        return TRUE;
    }     

  return search_mload( arg->next, vnum );
}


bool search_quest( arg_type* arg, int vnum )
{
  aif_type*      aif;
  afunc_type*  afunc;
  int              i;

  if( arg == NULL )
    return FALSE;

  if( arg->family == if_clause ) {
    aif = (aif_type*) arg;
    if( search_quest( aif->yes, vnum ) || search_quest( aif->no, vnum )
      || search_quest( aif->condition, vnum ) )
      return TRUE;
    }

  if( arg->family == function ) {
    afunc = (afunc_type*) arg;
    if( ( afunc->func->func_call == &code_assign_quest
      || afunc->func->func_call == &code_update_quest 
      || afunc->func->func_call == &code_has_quest 
      || afunc->func->func_call == &code_doing_quest
      || afunc->func->func_call == &code_done_quest )
      && afunc->arg[1] != NULL ) {
      if( int( afunc->arg[1]->value ) == vnum )
        return TRUE;
      }
    for( i = 0; i < 4 && afunc->arg[i] != NULL; i++ )
      if( search_quest( afunc->arg[i], vnum ) )
        return TRUE;
    }     

  return search_quest( arg->next, vnum );
}




