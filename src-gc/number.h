/*
 *   STRUCTURES
 */


int    roll_dice        ( int, int );
void   damage           ( dice_data&, char_data*, obj_data* );
void   sprintf_dice     ( char*, int );


class Dice_Data
{
 public:
  int       number;    
  int       side;
  int       plus;

  Dice_Data& operator=( const int& value ) {
    number = ( value & 0x3F );
    side   = (( value >> 6 ) & 0xFFF );
    plus   = ( value >> 18 );
    return *this;
    };

  operator int( ) {
    int value;

    value = number;
    value += ( side << 6 );
    value += ( plus << 18 );

    return value;
    }; 

  inline friend int average( const dice_data dice ) {
    return dice.number*(dice.side+1)/2+dice.plus;
    };

  inline friend int roll_dice( const dice_data dice ) {
    return roll_dice( dice.number, dice.side )+dice.plus;
    };

  inline friend int roll_dice( int value ) {
    dice_data dice;
    dice = value;  
    return roll_dice( dice );
    }
};


/*
 *   VARIOUS INLINE MATH MACROS
 */


template < class T >
T sqr( T a )
{ 
  return( a*a );
}   


template < class T >
T cube ( T a )
{
  return( a*a*a );
} 


template < class T >
T pow4 ( T a )
{
  return( a*a*a*a );
} 


inline int max  ( int a, int b )  { return( a > b ? a : b );  }
inline int min  ( int a, int b )  { return( a < b ? a : b );  }
inline int sign ( int a )         { return( a > 0 ? 1 : -1 ); }


inline int range( int a, int b, int c )       
{
  return( b < a ? a : ( b > c ? c : b ) );
}


inline bool not_in_range( int i, int a, int b )
{
  return( i < a || i > b );
}


/*
 *   FUNCTIONS
 */


const char*   number_word     ( int, char_data* = NULL );
int           number_range    ( int, int );
int           evaluate        ( char* s1, bool&, int = 0, int = 0 );
void          atorange        ( char*, int&, int& );
const char*   atos            ( int );
bool          renumber        ( int&, int, int );

const char*   int3            ( int );
const char*   int4        ( int );
const char*   int5        ( int );
const char*   float3          ( float );

extern bool numeric;






