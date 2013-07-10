#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "define.h"
#include "struct.h"


/*
 *   THINGS 
 */


const char* Room_Data :: Location( content_array* )
{
  char* tmp = static_string( );

  sprintf( tmp, "Room %d, %s", vnum, name );

  return tmp;
}


const char* Obj_Data :: Location( content_array* )
{
  char* tmp = static_string( );

  sprintf( tmp, "Unknown, %s", Seen_Name( NULL ) );

  return tmp;
}


const char* Char_Data :: Location( content_array* array )
{
  char*   tmp  = static_string( );
  char*  word;

  if( array == &contents ) 
    word = "Inventory";
  else if( array == &wearing )
    word = "Worn";
  else
    word = "** BUG **";

  sprintf( tmp, "%s, %s", Name( NULL ), word );  

  return tmp;
}


const char* Player_Data :: Location( content_array* array )
{
  char*   tmp  = static_string( );
  char*  word;

  if( array == &junked )
    word = "Junked";
  else if( array == &locker )
    word = "Locker";
  else if( array == &contents ) 
    word = "Inventory";
  else if( array == &wearing )
    word = "Worn";
  else
    word = "** BUG **";

  sprintf( tmp, "%s, %s", Name( NULL ), word );  

  return tmp;
}


const char* Thing_Data :: Location( content_array* ) 
{
  return "** BUG **";
}


