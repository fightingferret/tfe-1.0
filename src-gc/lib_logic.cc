#include "ctype.h"
#include "sys/types.h"
#include "stdio.h"
#include "stdlib.h"
#include "define.h"
#include "struct.h"


/*
 *   SET EQUAL
 */


void* code_set_equal( void** argument )
{
  arg_type*  arg  = (arg_type*) argument[0];
  void*    value  = (void*)     argument[1];

  void**    pntr;

  pntr  = (void**) arg->value;
  *pntr = value;

  return NULL;
}


/*
 *   MATH OPERATORS
 */


void* code_plus_equal( void** argument )
{
  arg_type*  arg  = (arg_type*) argument[0];
  int          i  = (int)       argument[1];

  (int) arg->value += i;

  return NULL;
}


void* code_minus_equal( void** argument )
{
  arg_type*  arg  = (arg_type*) argument[0];
  int          i  = (int)       argument[1];

  (int) arg->value -= i;

  return NULL;
}


void* code_is_equal( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( i == j );
}


void* code_not_equal( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( i != j );
}


/*
 *   COMPARISIONS
 */


void* code_gt( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( i > j );
}


void* code_lt( void** argument )
{
  int  i  = (int) argument[0];
  int  j  = (int) argument[1];

  return (void*) ( i < j );
}


void* code_ge( void** argument )
{
  int  i  = (int) argument[0];
  int  j  = (int) argument[1];

  return (void*) ( i >= j );
}


void* code_le( void** argument )
{
  int  i  = (int) argument[0];
  int  j  = (int) argument[1];

  return (void*) ( i <= j );
}


/*
 *   LOGIC GATES
 */


void* code_and( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( i && j );
}


void* code_or( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( i || j );
}


void* code_eor( void** argument )
{
  int i  = (int) argument[0];
  int j  = (int) argument[1];

  return (void*) ( ( i && !j ) || ( j && !i ) );
}





