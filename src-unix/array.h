/*
 *   BASE ARRAY CLASS
 */


template <class something>
class Array
{
 public:
  int           size;
  something*    list;
  int         memory;

 public:
  Array( ) {
    record_new( sizeof( Array ), MEM_ARRAY );
    size   = 0;
    memory = 0;
    list   = NULL;
    } 

  ~Array( ) {
    record_delete( sizeof( Array ), MEM_ARRAY );
    clear( *this );
    }

  friend void clear( Array& array ) {
    if( array.memory != 0 ) {
      record_delete( array.memory*sizeof( something ), -MEM_ARRAY );
      delete [] array.list; 
      array.list   = NULL;
      array.size   = 0;
      array.memory = 0;
      }
    }

  friend bool includes( Array& array, something x ) {
    for( int i = 0; i < array.size; i++ )
      if( array.list[i] == x )
        return TRUE;
    return FALSE;
    }

  friend void remove( Array& array, int i ) {
    array.size--; 
    memcpy( &array.list[i], &array.list[i+1],
      (array.size-i)*sizeof( something ) );
    }

  friend void insert( Array& array, something x, int i ) {
    if( array.size == array.memory ) {
      array.memory = 2*array.memory+1;
      record_new( (array.memory-array.size)*sizeof( something ),
        -MEM_ARRAY );
      something* next = new something [ array.memory ];
      memcpy( next, array.list, i*sizeof( something ) );
      memcpy( &next[i+1], &array.list[i],
        (array.size-i)*sizeof( something ) );
      delete [] array.list;
      array.list = next;
      }
    else {
      memmove( &array.list[i+1], &array.list[i],
        (array.size-i)*sizeof( something ) );
      }
    array.list[i] = x;
    array.size++;
    }

  friend void copy( Array& to, Array& from ) {
    record_new( from.size*sizeof( something ), -MEM_ARRAY );
    to.size   = from.size;
    to.memory = from.size;
    to.list   = new (something) [ from.size ];
    memcpy( to.list, from.list, from.size*sizeof( something ) ); 
    }

  friend bool is_empty( Array& array ) { return array.size == 0; }

  /* OPERATORS */

  something operator[] ( int i ) { return list[i]; } 
  int array_size( ) { return size; } 

  friend bool operator<  ( int i, Array& a ) { return( i < a.size ); }
  friend bool operator>  ( int i, Array& a ) { return( i > a.size ); }
  friend bool operator>  ( Array& a, int i ) { return( a.size > i ); }
  friend bool operator>= ( Array& a, int i ) { return( a.size >= i ); }
  friend bool operator>= ( int i, Array& a ) { return( i >= a.size ); }
  friend bool operator== ( Array& a, int i ) { return( a.size == i ); }
  friend bool operator== ( int i, Array& a ) { return( i == a.size ); }
  friend int  operator-  ( Array& a, int i ) { return a.size-i; }
  friend int  operator-  ( int i, Array& a ) { return i-a.size; }

  friend int  operator+  ( Array& a, Array& b ) { return a.size+b.size; }

  friend void operator-=( Array& array, something x ) {
    for( int i = 0; i < array.size; i++ )
      if( array.list[i] == x ) {
        remove( array, i );
        break;
        }
    }

  friend void operator+=( Array& array, something x ) {
    if( !includes( array, x ) ) {
      if( array.size == array.memory ) {
        array.memory = 2*array.memory+1;
        record_new( (array.memory-array.size)*sizeof( something ),
          -MEM_ARRAY );
        something* next = new something [ array.memory ];
        memcpy( next, array.list, array.size*sizeof( something ) );
        delete [] array.list;
        array.list = next;
        }
      array.list[array.size] = x;
      array.size++;
      }
    }
};


template <class something>
void delete_list( Array<something>& x )
{
  for( int i = 0; i < x.size; i++ ) 
    delete x.list[i]; 

  clear( x );
}


/*
 *   BINARY SEARCH
 */


template < class S, class T >
int binary_search( Array<S>& array, T item, int func( S, T ) )
{
  int      min  = 0;
  int      max  = array.size-1;
  int      mid;
  int    value;

  for( ; ; ) {
    if( max < min )
      return min;
  
    mid    = (max+min)/2;
    value  = ( *func )( array[mid], item );

    if( value == 0 )
      break;
    if( value < 0 )
      min = mid+1;
    else 
      max = mid-1;
    }

  return mid;
}


/* 
 *   OBJECT ARRAYS
 */


class Thing_Array : public Array<Thing_Data*>
{
 public:
  const char* Seen_Name( char_data* = NULL ) {
    return "an array";
    }
};


class Content_Array : public Thing_Array
{
 public:
  int          weight;
  int          number;
  int           light;
  thing_data*   where;

  Content_Array( thing_data* thing = NULL ) {
    weight = 0;
    number = 0;
    light  = 0;
    where  = thing;
    }
};











