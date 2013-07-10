/*
 *   FUNCTIONS
 */


const char*  on_off       ( int*, int );
const char*  true_false   ( int*, int );
const char*  yes_no       ( int*, int );


/*
 *   GENERAL
 */


template < class T >
void swap( T& a, T& b )
{
  T temp;

  memcpy( &temp, &a,    sizeof( T ) );
  memcpy( &a,    &b,    sizeof( T ) ); 
  memcpy( &b,    &temp, sizeof( T ) );

  return;
}


template < class T >
bool exchange( T& value, T a, T b )
{ 
  if( value == a ) 
    value = b;
  else if( value == b )
    value = a;
  else
    return FALSE;

  return TRUE;
}


template < class T >
T element( int i, T* first, T* second )
{
  return *(first+i*(second-first));
}; 
         

/*
 *   LINKED LIST
 */


template < class T >
void add( T*& list, T* item )
{
  item->next = list;
  list       = item;

  return;
}     


template < class T >
void append( T*& list, T* item )
{
  item->next = NULL;
  cat( list, item );

  return;
}     


template < class T >
void cat( T*& list, T* item )
{
  T* prev;

  if( list == NULL ) {
    list = item;
    }
  else {
    for( prev = list; prev->next != NULL; prev = prev->next );
    prev->next = item;
    }

  return;
}     


template < class T >
void remove( T*& list, T* item )
{
  T* prev;

  if( list == item ) {
    list = item->next;
    }
  else {
    for( prev = list; prev != NULL && prev->next != item; prev = prev->next );
    if( prev != NULL )  
      prev->next = item->next;
    else {
      bug( "Remove: Not in list." );
      }
    }

  return;
}  


template < class T >
void remove( T*& list, T* item, const char* msg )
{
  T* prev;

  if( list == item ) {
    list = item->next;
    }
  else {
    for( prev = list; prev != NULL && prev->next != item; prev = prev->next );
    if( prev != NULL )  
      prev->next = item->next;
    else 
      bug( msg );
    }

  return;
}  


template < class T >
int count( T* list )
{
  int i;

  for( i = 0; list != NULL; i++ )
    list = list->next;

  return i;
}


template < class T >
T* locate( T* list, int i )
{
  for( ; list != NULL; list = list->next )
    if( --i == 0 )
      return list;

  return NULL;
}


template < class T >
void delete_list( T*& list )
{
  T* item;

  while( ( item = list ) != NULL ) {
    list = list->next;
    delete item;
    }
 
  return;
}


template < class T >
void extract_list( T*& list )
{
  T* item;
  T* item_next;

  for( item = list; item != NULL; item = item_next ) {
    item_next = item->next;
    extract( item );
    }

  list = NULL;
 
  return;
}


template < class T >
bool is_listed( T* list, T* element )
{
  for( ; list != NULL; list = list->next )
    if( list == element )
      return TRUE;

  return FALSE;
} 


/*
 *    ARRAY
 */


template < class T >
void vzero( T* item, int max )
{
  for( int i = 0; i < max; i++ )
    item[i] = NULL;
}


/*
 *   SEARCH/SORT TEMPLATES
 */


template < class T >
void sort( T* list, int size )
{
  bool  done;
  int   i, j;

  for( i = 0; i < size-1; i++ ) {
    done = TRUE;
    for( j = 0; j < size-1-i; j++ ) {
      if( strcasecmp( list[j].name, list[j+1].name ) > 0 ) {
        swap( list[j], list[j+1] );
        done = FALSE;
        }
      }
    if( done )
      break;
    }
}


template < class T >
int pntr_search( T** list, int max, const char* word )
{
  int      min  = 0;
  int    value;
  int      mid;

  if( list == NULL )
    return -1;

  max--;

  for( ; ; ) {
    if( max < min )
      return -min-1;

    mid    = (max+min)/2;
    value  = strcasecmp( name( list[mid] ), word );

    if( value == 0 )
      break;
    if( value < 0 )
      min = mid+1;
    else 
      max = mid-1;
    }

  return mid;
}


template < class T >
int pntr_search( T** list, int max, const char* word, int n )
{
  int      min  = 0;
  int    value;
  int      mid;

  if( list == NULL )
    return -1;

  max--;

  for( ; ; ) {
    if( max < min )
      return -min-1;

    mid    = (max+min)/2;
    value  = strncasecmp( name( list[mid] ), word, n );

    if( value == 0 )
      break;
    if( value < 0 )
      min = mid+1;
    else 
      max = mid-1;
    }

  return mid;
}


template < class T >
int search( T* list, int max, const char* word )
{
  int      min  = 0;
  int    value;
  int      mid;

  if( list == NULL )
    return -1;

  max--;

  for( ; ; ) {
    if( max < min )
      return -min-1;

    mid    = (max+min)/2;
    value  = strcasecmp( list[mid].name, word );

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
 *   STATIC LIST ROUTINES
 */


template < class T >
void insert( T*& prev, int& size, T element, int pos )
{
  T* next;

  if( prev == NULL ) {
    prev     = new T[1];
    prev[0]  = element;
    size     = 1;
    return;
    }

  next = new T[size+1];

  memcpy( next, prev, pos*sizeof( T ) );
  memcpy( &next[pos+1], &prev[pos], (size-pos)*sizeof( T ) );

  delete [] prev;

  next[pos]  = element;
  prev       = next;
  size++;
}  


template < class T >
void remove( T*& prev, int& size, int pos )
{
  T* next;

  if( pos < 0 || pos >= size ) 
    return;

  if( size == 1 ) {
    delete [] prev;
    prev  = NULL;
    size  = 0;
    return;
    }

  next = new T[size-1];

  memcpy( next, prev, pos*sizeof( T ) );
  memcpy( &next[pos], &prev[pos+1], (size-pos-1)*sizeof( T ) );

  delete [] prev;

  prev = next;
  size--;
}  
 
  
  










