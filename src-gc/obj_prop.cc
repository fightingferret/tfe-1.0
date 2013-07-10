#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"



int obj_data :: metal( )
{
  int i;

  for( i = MAT_BRONZE; i < MAT_ADAMANTINE; i++ )
    if( is_set( &materials, i ) )
      return i;

  return 0;
}


int obj_clss_data :: metal( )
{
  int i;

  if( is_set( extra_flags, OFLAG_RANDOM_METAL ) )
    return -1;

  for( i = MAT_BRONZE; i < MAT_ADAMANTINE; i++ )
    if( is_set( &materials, i ) )
      return i;

  return 0;
}
