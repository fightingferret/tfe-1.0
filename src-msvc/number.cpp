#include "define.h"
#include "struct.h"

class double_complex {
  double im;
  double re;
public:
  double_complex() { im = 0; re = 0; }
  double_complex(double x, double y) { re = x; im = y; }
  double_complex(double x) { im = 0; re = x; }
  double imag() { return im; }
  double real() { return re; }
};

int              evaluate     ( char*, bool&, int, int, int, int ); 
double_complex   fevaluate    ( char*, bool&, double = 0, double = 0 );
double_complex   fevaluate    ( char*, bool&, double, double, int, int ); 


bool numeric = TRUE;


/*
 *   RENUMBER
 */


bool renumber( int& value, int i, int j )
{
  if( value == i )
    value = j;
  else if( i < j ) {
    if( value > i && value <= j ) {
      value--;
      return TRUE;
      }
    }
  else {
    if( value < i && value >= j ) {
      value++;
      return TRUE;
      }
    }

  return FALSE;
}


/*
 *   RANDOM NUMBER ROUTINES
 */


int number_range( int from, int to )
{
  static int value, width;
  register int bits, power;
  int number;

  if( ( to = to-from ) <= 0 )
    return from;

  for( bits = 1, power = 2; power <= to; bits++, power <<= 1 );

  do {
    if( width < bits ) {
      value = rand( );
      width = 15;
      }

    number = ( value & ( ( 1 << bits ) - 1 ) );
    value >>= bits;
    width -= bits;
    } while( number > to );

  return from+number;
}


/*
 *   DICE ROUTINES
 */


int roll_dice( int number, int size )
{
  int idice;
  int sum;

  switch( size ) {
    case 0: return 0;
    case 1: return number;
    }

  if( number > 100 )
    number =  100;

  for( idice = 0, sum = 0; idice < number; idice++ )
    sum += number_range( 1, size );

  return sum;
}


void sprintf_dice( char* tmp, int value )
{
  dice_data dice;

  dice = value;

  if( dice.number != 0 || dice.side != 0 ) {
    if( dice.plus != 0 ) 
      sprintf( tmp, "%dd%d+%d", dice.number, dice.side, dice.plus );
    else
      sprintf( tmp, "%dd%d", dice.number, dice.side );
    }
  else {
    sprintf( tmp, "%d", dice.plus );
    }

  return;
} 


/*
 *   NUMBER WORD FUNCTIONS
 */


const char* number_name[] = { "zero", "one", "two", "three", "four", "five",
  "six", "seven", "eight", "nine", "ten", "eleven", "twelve", "thirteen",
  "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen" }; 

const char* tens_name[] = { "twenty", "thirty", "forty", "fifty",
  "sixty", "seventy", "eighty", "ninety" };		      


const char* number_word( int i, char_data* ch )
{
  char* tmp;

  store_pntr = ( store_pntr+1 )%5;
  tmp        = &static_storage[store_pntr*THREE_LINES];

  if( numeric && ch != NULL && ch->pcdata != NULL
    && is_set( ch->pcdata->pfile->flags, PLR_NUMERIC ) ) {
    sprintf( tmp, "%d", i );
    return tmp;
    } 

  if( i < 0 )
    return "[BUG]";

  if( i < 20 ) 
    return number_name[i];

  if( i > 19999 )
    return "many";  

  if( i%1000 == 0 ) {
    sprintf( tmp, "%s thousand", number_name[i/1000] );
    return tmp;
    }

  if( i > 999 ) {
    sprintf( tmp, "%s thousand %s",
      number_name[i/1000], number_word( i%1000 ) );
    return tmp;
    }

  if( i%100 == 0 ) {
    sprintf( tmp, "%s hundred", number_name[i/100] );
    return tmp;
    }

  if( i > 99 ) {
    sprintf( tmp, "%s hundred ", number_name[i/100] );
    if( i%100 < 20 )
      strcat( tmp, number_name[i%100] );
    else {
      strcat( tmp, tens_name[(i/10)%10-2] );
      if( i%10 != 0 )
        sprintf( tmp+strlen( tmp ), "-%s", number_name[i%10] );
      }
    return tmp;
    }

  if( i%10 == 0 ) 
    return tens_name[i/10-2];

  sprintf( tmp, "%s-%s", tens_name[i/10-2], number_name[i%10] );
  return tmp;
}


const char* number_pos_word( int num ) 
{
  const char* data[] = { "zeroth ", "first ", "second ", "third ",
    "fourth ", "fifth ", "sixth ", "seventh ", "eighth ", "ninth " };

  if( num < 0 || num > 9 )
    return "[BUG] ";

  return data[num];
}


const char* number_suffix( int num )
{
  static const char* data [] = { "st", "nd", "rd", "th" };

  num = num%100;

  if( num > 3 && num < 21 )
    return data[3];
 
  num = min( (num+9)%10, 3 );

  return data[num];
};


/*
 *   INTEGER FORMATS
 */


const char* atos( int i )
{
  char* tmp = static_string( );

  sprintf( tmp, "%d", i );

  return tmp;
}


const char* int3( int i )
{
  char* tmp = static_string( );

  if( i > -99 && i < 999 ) {
    sprintf( tmp, "%3d", i );
    return tmp;
    }

  return "***";
}


const char* int4( int i )
{
  char* tmp = static_string( );

  if( i > -999 && i < 9999 ) {
    sprintf( tmp, "%4d", i );
    return tmp;
    }

  if( i > -99999 && i < 999999 ) {
    sprintf( tmp, "%3dk", i/1000 );
    return tmp;
    }

  return "****";
}


const char* int5( int i )
{
  char* tmp = static_string( );

  if( i > -9999 && i < 99999 ) {
    sprintf( tmp, "%5d", i );
    return tmp;
    }

  if( i > -999999 && i < 9999999 ) {
    sprintf( tmp, "%4dk", i/1000 );
    return tmp;
    }

  return "*****";
}


/*
 *   FLOAT FORMATS
 */


const char* float3( float x )
{
  char* tmp = static_string( );

  if( x < 1 ) {
    sprintf( tmp, ".%.2d", int( 100*x ) );
    return tmp;
    }

  if( x < 10 ) {
    sprintf( tmp, "%3.1f", x );
    return tmp;
    }

  if( x < 1000 ) {
    sprintf( tmp, "%3.0f", x );
    return tmp;
    }

  return "***";
}



/*
 *   STRING TO RANGE ROUTINE
 */


void atorange( char* string, int& a, int& b )
{
  for( a = 0; isdigit( *string ); string++ ) 
    a = 10*a+*string-'0';

  if( *string == '+' ) {
    b = 9999;
    return;
    }

  if( *string != '-' ) {
    b = a;
    return;
    }

  for( b = 0; isdigit( *string ); string++ ) 
    b = 10*b+*string-'0';

  return;    
}


/*
 *  
 */


inline bool left_bracket( char c )
{
  return( c == '(' || c == '[' );
} 


inline bool right_bracket( char c )
{
  return( c == ')' || c == ']' );
} 


int close_bracket( char* string, int left, int right )
{
  int     i;
  int 		level  = 1;

  for( i = left; i <= right; i++ ) {
    if( left_bracket( string[i] ) )
      level++;
    if( right_bracket( string[i] ) && --level == 0 )
      return i;
    }

  return -1;
}


/*
 *   INTEGER EVALUATE
 */
 

int evaluate( char* string, bool& error, int v1, int v2 )
{
  error = FALSE;

  return evaluate( string, error, v1, v2, 0, strlen( string )-1 );
}


int evaluate( char* string, bool& error, int v1, int v2,
  int left, int right ) 
{
  char  priority  = '4';
  int   division  = -1;
  int       a, b;
  int       i, j;

  char*  oper  = "+-*/d";
  char* order  = "11223";

  if( error ) {
    error = TRUE;
    return 0;
    }
  
  if( left > right ) 
    return 0;

  while( left_bracket( string[left] )
    && close_bracket( string, left+1, right ) == right ) {
    left++;
    right--;
    }

  if( left == right ) {
    if( string[left] == 'L' ) 
      return v1;
    if( string[left] == 'S' )
      return v2;
    }

  for( i = left; i <= right; i++ ) {
    if( left_bracket( string[i] ) ) {
      if( ( i = close_bracket( string, i+1, right ) ) == -1 ) {
        error = TRUE;
        return 0;
        }
      continue;
      }
    for( j = 0; j < 5; j++ )
      if( string[i] == oper[j] && priority >= order[j] ) {
        division = i;
        priority = order[j];
        break;
        }
    }

  if( division == -1 ) {
    for( a = 0, i = left; i <= right; i++ ) {
      if( isdigit( string[i] ) )
        a = 10*a+string[i]-'0'; 
      else {
        error = TRUE;
        return 0;
        }
      }
    return a;
    }

  a = evaluate( string, error, v1, v2, left, division-1 );
  b = evaluate( string, error, v1, v2, division+1, right );

  switch( string[division] ) {
    case '+' :  return a+b;
    case '-' :  return a-b;
    case '*' :  return a*b;
    case '/' :
      if( b == 0 ) {
        error = TRUE;
        return 0;
        }
      return a/b;
    case 'd' :  return roll_dice( a,b );
    }

  bug( "Evaluate: Impossible Operator!?" );
  error = TRUE;

  return 0;
}


/*
 *  EVALUATE USING DOUBLE PRECISION
 */

 
typedef double_complex math_func  ( const double_complex& );


const char* tostring( double_complex x )
{
  char* tmp = static_string( );

  if( x.imag( ) != 0 ) 
    if( x.real( ) != 0 ) 
      sprintf( tmp, "%g%+gi", x.real( ), x.imag( ) );
    else
      sprintf( tmp, "%gi", x.imag( ) );
  else
    sprintf( tmp, "%g", x.real( ) );
   
  return tmp;
}


void do_calculate( char_data* ch, char* argument )
{
  bool  error;
  double_complex     x;

  if( *argument == '\0' ) {
    send( ch, "What expression do you wish to evaluate?\r\n" );
    return;
    }

  x = fevaluate( argument, error );

  if( error ) {
    send( ch, "Expression fails to evaluate.\r\n" );
    return;
    }

  send( ch, "%s = %s\r\n", argument, tostring( x ) );

  return;
}
 

double_complex fevaluate( char* string, bool& error, double v1, double v2 )
{
  error = FALSE;

  return fevaluate( string, error, v1, v2, 0, strlen( string )-1 );
}


double_complex fevaluate( char* string, bool& error, double v1, double v2,
  int left, int right ) 
{
  char          priority      = '4';
  int           division      = -1;
//  double_complex    a, b;
  int               i, j;
  char*             oper      = "+-*/^";
  char*            order      = "11223";

  if( error ) {
    error = TRUE;
    return 0;
    }
  
  if( left > right ) 
    return 0;

  while( left_bracket( string[left] )
    && close_bracket( string, left+1, right ) == right ) {
    left++;
    right--;
    }
/*
#define NFUNC 5

  char*      funct_name  [ NFUNC ]  = { "sin", "cos", "sqrt", "exp", "log" };
  math_func* funct_call  [ NFUNC ]  = { sin, cos, sqrt, exp, log };

  for( i = 0; i < NFUNC; i++ ) { 
    j = strlen( funct_name[i] );
    if( !strncasecmp( string+left, funct_name[i], j ) 
      && left_bracket( string[left+j] ) && right_bracket( string[right] ) 
      && close_bracket( string, left+j+1, right ) == right ) { 
      a = fevaluate( string, error, v1, v2, left+j+1, right-1 );
      return( *funct_call[i] )( a );
      }
    }

#undef NFUNC
*/
#define NVAR 3

  char*   var_name   [ NVAR ]  = { "pi", "e", "i" };
  double_complex  var_value  [ NVAR ]  = { M_PI, M_E, double_complex( 0, 1 ) };

  for( i = 0; i < NVAR; i++ ) { 
    j = strlen( var_name[i] );
    if( j == right-left+1
      && !strncasecmp( string+left, var_name[i], j ) )
      return var_value[i];
    }

#undef NVAR

  if( left == right ) {
    if( string[left] == 'L' ) 
      return v1;
    if( string[left] == 'S' )
      return v2;
    }

  for( i = left; i <= right; i++ ) {
    if( left_bracket( string[i] ) ) {
      if( ( i = close_bracket( string, i+1, right ) ) == -1 ) {
        error = TRUE;
        return 0;
        }
      continue;
      }
    for( j = 0; oper[j] != '\0'; j++ )
      if( string[i] == oper[j] && priority >= order[j] ) {
        division = i;
        priority = order[j];
        break;
        }
    }

/*
  if( division == -1 ) {
    for( j = 0, a = 0, i = left; i <= right; i++ ) {
      if( isdigit( string[i] ) ) {
        a = 10*a+string[i]-'0'; 
        j *= 10;
        } 
      else if( string[i] == '.' && j == 0  ) {
        j = 1;
        }
      else {
        error = TRUE;
        return 0;
        }
      }
    return a/max(j,1);
    }

  a = fevaluate( string, error, v1, v2, left, division-1 );
  b = fevaluate( string, error, v1, v2, division+1, right );

  switch( string[division] ) {
    case '+' :  return a+b;
    case '-' :  return a-b;
    case '*' :  return a*b;
    case '^' :  return pow( a,b );
    case '/' :  return a/b;
    }

  bug( "Evaluate: Impossible Operator!?" );
  error = TRUE;
 */
  return 0;
}







