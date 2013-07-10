#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "struct.h"


/*
 *   CHARACTER SAVE ROUTINES
 */


bool char_data :: save_vs_poison( int )
{
  return( number_range( 0, 40 ) < Constitution( )+shdata->level/10 );    
}

