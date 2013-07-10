#include <ctype.h>
#include <sys/types.h>
#include <stdio.h>
#include <syslog.h>
#include "define.h"
#include "struct.h"


/*
 *   BIT ROUTINES
 */



const char* true_false( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  if( ( array[i] & ( 1 << j ) ) != 0 )
    return "true";

  return "false";
}


const char* yes_no( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  if( ( array[i] & ( 1 << j ) ) != 0 )
    return "yes";

  return "no";
}


const char* on_off( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  if( ( array[i] & ( 1 << j ) ) != 0 )
    return "on";

  return "off";
}


/*
 *  INTEGER PACKING
 */


int pack_int( int* value )
{
  int byte;

  byte = value[0]+( value[1] << 8 )+( value[2] << 16 )
    +( value[3] << 24 ); 
  
  return byte;
}


int* unpack_int( int byte )
{
  static int value [4];

  value[0] = ( byte & 0xff );
  value[1] = ( ( byte >> 8 ) & 0xff ); 
  value[2] = ( ( byte >> 16 ) & 0xff );
  value[3] = ( ( byte >> 24 ) & 0xff );

  return value;
}


/*
 *   SEARCHES
 */


int search( int* list, int max, int value )
{
  int      min  = 0;
  int      mid;

  if( list == NULL )
    return -1;

  max--;

  for( ; ; ) {
    if( max < min )
      return -min-1;

    mid = (max+min)/2;

    if( value == list[mid] )
      break;
    if( value < list[mid] )
      min = mid+1;
    else 
      max = mid-1;
    }

  return mid;
}



