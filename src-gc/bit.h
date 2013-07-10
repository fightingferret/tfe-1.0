/*
 *   BIT ARRAY FUNCTIONS
 */


inline bool is_set( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  return( ( array[i] & ( 1 << j ) ) != 0 );
}


inline void switch_bit( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  array[i] ^= ( 1 << j );

  return;
}


inline void set_bit( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  array[i] |= ( 1 << j );

  return;
}


inline void remove_bit( int* array, int bit )
{
  int i = bit/32;
  int j = bit-32*i;

  array[i] &= ~( 1 << j ); 

  return;
}


inline void assign_bit( int* array, int bit, bool value )
{
  if( value )
    set_bit( array, bit );
  else
    remove_bit( array, bit );

  return;
}
  

/*
 *   LEVEL FUNCTIONS
 */


inline void set_level( int* array, int bit, int level )
{
  assign_bit( array, 2*bit, is_set( &level, 0 ) );
  assign_bit( array, 2*bit+1, is_set( &level, 1 ) );

  return;
}


inline int level_setting( int* array, int bit )
{
  int      i  = bit/16;
  int      j  = 2*(bit-16*i);
  int  level;

  level = ( array[i] >> j ) & 3;

  return level;
}


